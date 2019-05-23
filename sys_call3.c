#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>

asmlinkage extern long (*sysptr3)(void *arg);

struct k_list {
	struct list_head test_list;
	int value;
};

asmlinkage long call3(void *arg)
{
	int ret = 0;
	struct k_list *one = NULL, *two = NULL;
	struct list_head test_head;

	one = kmalloc(sizeof(struct k_list *), GFP_KERNEL);
	if (!one) {
		ret = -ENOMEM;
		goto out_kfree;
	}

	two = kmalloc(sizeof(struct k_list *), GFP_KERNEL);
	if (!two) {
		ret = -ENOMEM;
		goto out_kfree;
	}

	one->value = 10;
	two->value = 20;

	INIT_LIST_HEAD(&test_head);
	list_add(&one->test_list, &test_head);

	/*
 	 * Bug added to corrupt the list catched by
 	 * CONFIG_BUG_ON_DATA_CORRUPTION
 	*/
	one->test_list.prev = NULL;

	list_add(&two->test_list, &test_head);

out_kfree:
	kfree(one);
	kfree(two);
	return 0;
}

static int __init init_sys_call3(void)
{
	printk("installed new sys_call3 module\n");
	if (sysptr3 == NULL)
		sysptr3 = call3;
	return 0;
}
static void  __exit exit_sys_call3(void)
{
	if (sysptr3 != NULL)
		sysptr3 = NULL;
	printk("removed sys_call3 module\n");
}
module_init(init_sys_call3);
module_exit(exit_sys_call3);
MODULE_LICENSE("GPL");
