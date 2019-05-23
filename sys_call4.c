#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/sched.h>  // for task_struct
#include <linux/time.h>   // for using jiffies 
#include <linux/timer.h>
#include <linux/mutex.h>
#include <linux/delay.h>

asmlinkage extern long (*sysptr4)(void *arg);

static struct task_struct *thread1, *thread2;
static DEFINE_MUTEX(lock1);
static DEFINE_MUTEX(lock2);

int thread_fn1(void *data)
{
	unsigned long j0, j1;
	int delay = 60 * HZ;


	mutex_lock(&lock1);
	msleep(50);
	mutex_lock(&lock2);
	
	j0 = jiffies;
        j1 = j0 + delay;
	while (time_before(jiffies, j1))
        	schedule();

	mutex_unlock(&lock2);
	mutex_unlock(&lock1);
	return 0;
}

int thread_fn2(void *data)
{
        unsigned long j0, j1;
        int delay = 60 * HZ;

	/*
 	 * BUG of ciruclar dependency detected by
 	 * CONFIG_DEBUG_MUTEXES and
 	 * hang detected by CONFIG_DETECT_HUNG_TASK
 	 */
	mutex_lock(&lock2);
	msleep(50);
	mutex_lock(&lock1);

	j0 = jiffies;
        j1 = j0 + delay;
        while (time_before(jiffies, j1))
                schedule();

	mutex_unlock(&lock1);
	mutex_unlock(&lock2);

        return 0;
}

asmlinkage long call4(void *arg)
{
	char first_thread[8] = "thread1";
	char sec_thread[8] = "thread2";

	thread1 = kthread_create(thread_fn1, NULL, first_thread);
	if (!thread1)
		return -ENOMEM;

	thread2 = kthread_create(thread_fn2, NULL, sec_thread);
	if(!thread2)
		return -ENOMEM;

	wake_up_process(thread1);
	wake_up_process(thread2);
	return 0;
}

static int __init init_sys_call4(void)
{
	printk("installed new sys_call4 module\n");
	if (sysptr4 == NULL)
		sysptr4 = call4;
	return 0;
}
static void  __exit exit_sys_call4(void)
{
	if (sysptr4 != NULL)
		sysptr4 = NULL;
	printk("removed sys_call4 module\n");
}
module_init(init_sys_call4);
module_exit(exit_sys_call4);
MODULE_LICENSE("GPL");
