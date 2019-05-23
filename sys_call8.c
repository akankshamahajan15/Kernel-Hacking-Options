#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>
#include <linux/notifier.h>
#include <linux/netdevice.h>

extern struct atomic_notifier_head my_noti_chain;

asmlinkage extern long (*sysptr8)(void *arg);

int val = 8;

int my_event_handler (struct notifier_block *self,
                unsigned long val, void *data);

/*
 * BUG : CONFIG_DEBUG_NOTIFIER
 * invalid callback function called
 */
static struct notifier_block my_notifier = {
	.notifier_call = my_event_handler,
};

int my_event_handler (struct notifier_block *self,
		unsigned long val, void *data)
{
	printk ("my_event: Val=%ld\n", val);
	return 0;
}

asmlinkage long call8(void *arg)
{
	return 0;
}

static int __init init_sys_call8(void)
{
	printk("installed new sys_call8 module\n");
	if (sysptr8 == NULL)
		sysptr8 = call8;
	
	/* Register User defined Notifier */
	atomic_notifier_chain_register (&my_noti_chain, &my_notifier);
	
	return 0;
}

static void  __exit exit_sys_call8(void)
{
	if (sysptr8 != NULL)
		sysptr8 = NULL;

	/* Unregister User defined Notifier */
	atomic_notifier_chain_unregister (&my_noti_chain, &my_notifier);
}

module_init(init_sys_call8);
module_exit(exit_sys_call8);
MODULE_LICENSE("GPL");
