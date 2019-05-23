#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>

asmlinkage extern long (*sysptr1)(void *arg);

asmlinkage long call1(void *arg)
{
	void *buf1 = kmalloc(10, GFP_KERNEL);
	memcpy(buf1, "abcdef", 7);

	/*
 	 * BUG : CONFIG_DEBUG_KMEMLEAK
 	 * not freeing the memory allocated to buf1
 	 */ 
	return 0;
}

static int __init init_sys_call1(void)
{
	printk("installed new sys_call1 module\n");
	if (sysptr1 == NULL)
		sysptr1 = call1;
	return 0;
}
static void  __exit exit_sys_call1(void)
{
	if (sysptr1 != NULL)
		sysptr1 = NULL;
	printk("removed sys_call1 module\n");
}
module_init(init_sys_call1);
module_exit(exit_sys_call1);
MODULE_LICENSE("GPL");
