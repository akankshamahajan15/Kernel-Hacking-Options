#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>
#include <linux/notifier.h>

static ATOMIC_NOTIFIER_HEAD(my_noti_chain);
EXPORT_SYMBOL(my_noti_chain);

asmlinkage extern long (*sysptr8_1)(void *arg);

asmlinkage long call8_1(void *arg)
{
	int val = 10;
	atomic_notifier_call_chain(&my_noti_chain, val, NULL);
	return 0;
}

static int __init init_sys_call8_1(void)
{
	printk("installed new sys_call8_1 module\n");

	if (sysptr8_1 == NULL)
		sysptr8_1 = call8_1;
	return 0;
}
static void  __exit exit_sys_call8_1(void)
{
	if (sysptr8_1 != NULL)
		sysptr8_1 = NULL;
	printk("removed sys_call8_1 module\n");
}
module_init(init_sys_call8_1);
module_exit(exit_sys_call8_1);
MODULE_LICENSE("GPL");
