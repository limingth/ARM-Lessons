
#include <linux/module.h>	// module_init
#include <asm/io.h>		// ioremap
#include <linux/fs.h>		// file_operations
#include <asm/uaccess.h>	// copy_from_user

MODULE_LICENSE("GPL");

#define MA	240

volatile int * pled;

int led_drv_open(struct inode *inode, struct file *filp)
{
	int major, minor;

	major = MAJOR(inode->i_rdev);
	minor = MINOR(inode->i_rdev);

	printk("led drv open: major %d, minor %d\n", major, minor);

	return 0;
}

ssize_t led_drv_write(struct file *filp, const char __user * buf, size_t count, loff_t *f_pos)
{
	char kbuf[128];

	//buf[count] = '\0';
	printk("led drv write %d\n", count);

//	printk("buf = %s\n", buf);
//	printk("buf at %p\n", buf);
	printk("count = %d\n", count);

//	copy_from_user(kbuf, buf, count);
	*pled = buf[0];

//	printk("kbuf = %s\n", kbuf);
//	printk("kbuf at %p\n", kbuf);

	return count;
}

int led_drv_release(struct inode *inode, struct file *filp)
{
	printk("led drv release ok!\n");

	return 0;
}

struct file_operations led_fops = 
{
	.owner = THIS_MODULE,
	.open = led_drv_open,
	.write = led_drv_write,
	.release = led_drv_release,
};

static int led_drv_init(void)
{
	int rc;

	printk("led init \n");

	pled = ioremap(0xE0200284, 4);

	//*pled = 0;
	rc = register_chrdev(MA, "akae", &led_fops);
	if (rc < 0)
	{
		printk("register failed\n");
		return -1;
	}

	printk("register char ok %d!\n", rc);

	return 0;
}

static void led_drv_exit(void)
{
	printk("led exit \n");

	//*pled = 0xF;
	unregister_chrdev(MA, "akae");
	printk("unregister char ok!\n");

	return;
}

module_init(led_drv_init);
module_exit(led_drv_exit);

