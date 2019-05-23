#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/kthread.h>

struct task_struct *task0;
static spinlock_t spinlock;

/* Function which runs for 30 seconds in the critical section with spin lock acquired */
int task(void *arg)
{
	unsigned long j0,j1;
	int delay = 30 * HZ;

	j0 = jiffies;
	j1 = j0 + delay;

	while(!kthread_should_stop()) {
		spin_lock(&spinlock);
		/* busy loop in critical section */
		while (time_before(jiffies, j1));
		spin_unlock(&spinlock);
	}

	return 0;
}

asmlinkage extern long (*sysptr6)(void *arg);

/* Code Reference:
 * https://www.kernel.org/doc/Documentation/lockup-watchdogs.txt
 * https://blog.seibert-media.com/2018/01/04/log-book-linux-cpu-lockups/
 * https://superuser.com/questions/1073890/how-to-generate-soft-lockup-in-linux-kernel-and-how-to-verify
 */
asmlinkage long call6(void *arg)
{
	int temp_val = 1;
	/* Initialize the spin lock */
	spin_lock_init(&spinlock);

	/* Calling thread run function on task defined above */
	task0 = kthread_run(&task,(void *)&temp_val,"softlockup_thread");
	set_cpus_allowed_ptr(task0, cpumask_of(0));
	return 0;
}

static int __init init_sys_call6(void)
{
	printk("installed new sys_call6 module\n");
	if (sysptr6 == NULL)
		sysptr6 = call6;
	return 0;
}
static void  __exit exit_sys_call6(void)
{
	if (sysptr6 != NULL)
		sysptr6 = NULL;
	printk("removed sys_call6 module\n");
}
module_init(init_sys_call6);
module_exit(exit_sys_call6);
MODULE_LICENSE("GPL");

