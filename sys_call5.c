#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/delay.h>

asmlinkage extern long (*sysptr5)(void *arg);

static struct task_struct *thread1,*thread2;
DEFINE_SPINLOCK(my_lock);

/* Thread function which acquires the lock successfully */
int thread_fn1(void *data)
{
	unsigned long j0,j1;
	int delay = 60*HZ;

	j0 = jiffies;
	j1 = j0 + delay;

	spin_lock(&my_lock);

	while (time_before(jiffies, j1))
		schedule();

	spin_unlock(&my_lock);
	return 0;
}

/* thread function unable to acquire spin lock */
int  thread_fn2(void *data)
{ 
	int ret = 0;

	/* 
	 * BUG INDUCED HERE!
	 * Putting Thread2 to sleep for 100 milli-seconds
	 * to allow thread 1 to acquire lock first
	 * and not allow Thread 2 to acquire the lock.
	 * Caught by: CONFIG_DEBUG_ATOMIC_SLEEP
	 */
	msleep(100);
	ret = spin_trylock(&my_lock);
	if (ret)
		spin_unlock(&my_lock);
	return 0;
}

/* Code Reference
 * http://tuxthink.blogspot.com/2013/06/using-spinlock-in-linux-example.html
 * https://cateee.net/lkddb/web-lkddb/DEBUG_ATOMIC_SLEEP.html
 * https://stackoverflow.com/questions/36413864/what-is-the-linux-3-16-equivalent-of-spin-lock-unlocked
 */  
asmlinkage long call5(void *arg)
{	
	char name[8] = "thread1";
	char name1[8] = "thread2";
	thread1 = kthread_create(thread_fn1, NULL, name);
	if (!thread1)
		return -ENOMEM;

	thread2 = kthread_create(thread_fn2, NULL, name1);
	if (!thread2)
		return -ENOMEM;

	wake_up_process(thread1);
	wake_up_process(thread2);
	return 0;
}

static int __init init_sys_call5(void)
{
	printk("installed new sys_call5 module\n");
	if (sysptr5 == NULL)
		sysptr5 = call5;
	return 0;
}
static void  __exit exit_sys_call5(void)
{
	if (sysptr5 != NULL)
		sysptr5 = NULL;
	printk("removed sys_call5 module\n");
}
module_init(init_sys_call5);
module_exit(exit_sys_call5);
MODULE_LICENSE("GPL");
