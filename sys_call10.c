#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

asmlinkage extern long (*sysptr10)(void *arg);

asmlinkage long call10(void *arg)
{
	/*
 	* BUG: CONFIG_DEBUG_STACKOVERFLOW
 	* allocating a buffer of PAGE_SIZE statically causes the stack to overflow
 	*/
	int buf[PAGE_SIZE], retval = 0, file_size;
	struct file *filpin = NULL;

	filpin = filp_open("input.txt", O_RDONLY, 0);
	if (!filpin || IS_ERR(filpin))
	{
		retval = (int) PTR_ERR(filpin);
		return retval;
	}

	file_size = filpin->f_inode->i_size;
	while (filpin->f_pos < file_size)
		kernel_read(filpin, buf, PAGE_SIZE, &(filpin->f_pos));
	
	filp_close(filpin, NULL);
	return 0;
}

static int __init init_sys_call10(void)
{
	printk("installed new sys_call10 module\n");
	if (sysptr10 == NULL)
		sysptr10 = call10;
	return 0;
}
static void  __exit exit_sys_call10(void)
{
	if (sysptr10 != NULL)
		sysptr10 = NULL;
	printk("removed sys_call10 module\n");
}
module_init(init_sys_call10);
module_exit(exit_sys_call10);
MODULE_LICENSE("GPL");
