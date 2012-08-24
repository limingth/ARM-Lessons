//#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/fs.h>
//#include <asm/uaccess.h>

int flag = 0;
#define MA	134

wait_queue_head_t myqueue;

ssize_t myread(struct file *fl, char * buf, size_t size, loff_t * loff)
{
	printk("myread() begin!\n");
	printk("myread() &flag = %p\n", &flag);

	printk("myread() wait begin!\n");
	wait_event_interruptible(myqueue, flag != 0);
	flag = 0;
	printk("myread() wait end!\n");

	//return sizeof(int);
	return 0;
}

int mywrite(struct file *fl, const char * buf, size_t size, loff_t * loff)
{
	printk("mywrite() begin!\n");
	printk("mywrite() &flag = %p\n", &flag);

	printk("mywrite() wake up begin!\n");
	wake_up_interruptible(&myqueue);
	flag = 1;
	printk("mywrite() wake up end!\n");

	return 1;
}

struct file_operations fops =
{
	.read = myread,
	.write = mywrite,
};


int int_init(void)
{
	int rc;

	printk("int init\n");

	rc = register_chrdev(MA, "mychar", &fops);

	if (rc < 0)
	{
		printk("register failed\n");
		return -1;
	}

	init_waitqueue_head(&myqueue);

	return 0;
}

void int_exit(void)
{
	printk("int exit\n");

	unregister_chrdev(MA, "mychar");

	return;
}

module_init(int_init);
module_exit(int_exit);
