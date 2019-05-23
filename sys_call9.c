#include <linux/linkage.h>
#include <linux/moduleloader.h>
#include <linux/slab.h>
#include <linux/scatterlist.h>
#include <crypto/skcipher.h>
#include <linux/random.h>

asmlinkage extern long (*sysptr9)(void *arg);

int skcipher_driver (char *key, char *ivdata)
{
	struct crypto_skcipher *skc = NULL;
	struct skcipher_request *req = NULL;
	struct crypto_wait wait;
	struct scatterlist sg;
	char *buf;
	int retval = 0, bytes_read = 40; 

	buf = vmalloc(50);
	if (!buf)
		return -ENOMEM;

	skc = crypto_alloc_skcipher("ctr-aes-aesni", 0, 0);
	if (IS_ERR(skc)) {
		printk("Allocation of skcipher handler failed.\n");
		retval = PTR_ERR(skc);
		goto out;
	}

	req = skcipher_request_alloc(skc, GFP_KERNEL);
	if (!req) {
		printk("Allocation of skcipher request failed.\n");
		retval = -ENOMEM;
		goto out;
	}

	skcipher_request_set_callback(req, CRYPTO_TFM_REQ_MAY_BACKLOG, crypto_req_done, &wait);

	if (crypto_skcipher_setkey(skc, key, 16)){
		printk("Key not set.\n");
		retval = -EAGAIN;
		goto out;
	}

	/* BUG : CONFIG_DEBUG_SG
 	 * triggers BUG_ON if the the passed buffer (buf) isn't a valid virtual address
 	 * 
 	*/ 
	sg_init_one(&sg, buf, bytes_read);
	skcipher_request_set_crypt(req, &sg, &sg, bytes_read, ivdata);
	crypto_init_wait(&wait);

	retval = crypto_wait_req(crypto_skcipher_encrypt(req), &wait);

out:
	vfree(buf);

	if(skc)
		crypto_free_skcipher(skc);
	if(req)
		skcipher_request_free(req);
	return retval;
}

asmlinkage long call9(void *arg)
{
	int retval = 0;
	char keybuf[17] = "somepassword1234";
	char *ivdata = NULL;

	ivdata = kmalloc(16, GFP_KERNEL);
	if(!ivdata) {
		printk("Could not allocate ivdata\n");
		return -ENOMEM;
	}

	get_random_bytes(ivdata, 16);
	retval = skcipher_driver(keybuf, ivdata);

	kfree(ivdata);
	return 0;
}

static int __init init_sys_call9(void)
{
	printk("installed new sys_call9 module\n");
	if (sysptr9 == NULL)
		sysptr9 = call9;
	return 0;
}
static void  __exit exit_sys_call9(void)
{
	if (sysptr9 != NULL)
		sysptr9 = NULL;
	printk("removed sys_call2 module\n");
}
module_init(init_sys_call9);
module_exit(exit_sys_call9);
MODULE_LICENSE("GPL");
