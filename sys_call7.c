#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>   // for using jiffies
#include <linux/timer.h>
#include <linux/mutex.h>
#include <linux/delay.h>

asmlinkage extern long (*sysptr7)(void *arg);

struct st {
        int a;
        int b;
} __rcu *gval;

static struct task_struct *thread1, *thread2;

DEFINE_MUTEX(gval_lock);

int thread_fn1(void *data)
{
        struct st *var;
        unsigned long j0, j1;
        int delay = 40*HZ;

        j0 = jiffies;
        j1 = j0 + delay;

	printk("thread1 \n");

	/*
 	 * BUG : CONFIG_RCU_CPU_STALL_TIMEOUT=30
 	 * grace period set is 30 sec and cpu gives warning
 	 * after that for update thread
 	 */ 
	rcu_read_lock();
	while (time_before(jiffies, j1)){
		//do nothing
	}
        var = rcu_dereference(gval);
        printk("thread fn1  %d, %d\n", var->a, var->b);
	rcu_read_unlock();

        return 0;
}

int thread_fn2(void *data)
{
	struct st *new_var, *old_var;
        unsigned long j0, j1;
        int delay = 5*HZ;

        j0 = jiffies;
        j1 = j0 + delay;

	printk("Thread2\n");
        while (time_before(jiffies, j1))
                schedule();

	new_var = kmalloc(sizeof(*new_var), GFP_KERNEL);
	if (!new_var)
		return -ENOMEM;

	mutex_lock(&gval_lock);
	old_var = rcu_dereference_protected(gval, lockdep_is_held(&gval_lock));
	*new_var = *old_var;
	new_var->a = 100;
	new_var->b = 200;
	rcu_assign_pointer(gval, new_var);
	mutex_unlock(&gval_lock);
	synchronize_rcu();
	kfree(old_var);
	return 0;
}

asmlinkage long call7(void *arg)
{
	char first_thread[8] = "thread1";
	char sec_thread[8] = "thread2";

	struct st *vr = kmalloc(sizeof(*gval), GFP_KERNEL);

	mutex_lock(&gval_lock);
	vr->a = 10;
	vr->b = 20;
	rcu_assign_pointer(gval, vr);
	mutex_unlock(&gval_lock);

	thread1 = kthread_create(thread_fn1, NULL, first_thread);
	if (!thread1)
		return -ENOMEM;


	thread2 = kthread_create(thread_fn2, NULL, sec_thread);
	if (!thread2)
		return -ENOMEM;
	thread1->state = TASK_UNINTERRUPTIBLE;
	wake_up_process(thread1);
	wake_up_process(thread2);

	return 0;
}

static int __init init_sys_call7(void)
{
	printk("installed new sys_call7 module\n");
	if (sysptr7 == NULL)
		sysptr7 = call7;
	return 0;
}
static void  __exit exit_sys_call7(void)
{
	if (gval) {
		printk("free gval\n");
		kfree(gval);
	}
	if (sysptr7 != NULL)
		sysptr7 = NULL;
	printk("removed sys_call7 module\n");
}
module_init(init_sys_call7);
module_exit(exit_sys_call7);
MODULE_LICENSE("GPL");
