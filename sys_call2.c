#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>

asmlinkage extern long (*sysptr2)(void *arg);

asmlinkage long call2(void *arg)
{
	int *arr = kmalloc(10 * sizeof(int), GFP_KERNEL);
	if (!arr)
		return -ENOMEM;

	/* BUG : CONFIG_DEBUG_KASAN
	 * Out of bound and use-after-free
	 */
	printk("out of bound : %d \n", arr[12]);

	kfree(arr);
	return 0;
}

static int __init init_sys_call2(void)
{
	printk("installed new sys_call2 module\n");
	if (sysptr2 == NULL)
		sysptr2 = call2;
	return 0;
}
static void  __exit exit_sys_call2(void)
{
	if (sysptr2 != NULL)
		sysptr2 = NULL;
	printk("removed sys_call2 module\n");
}
module_init(init_sys_call2);
module_exit(exit_sys_call2);
MODULE_LICENSE("GPL");
