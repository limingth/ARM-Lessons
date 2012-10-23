#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

#define CHAR_MAJOR		240
#define CHAR_NAME		"char dev driver"

static int led_no = 0;

int char_open(struct inode * inode, struct file * fp) 
{
	printk("---> char open called! \n");

	printk("dev is 0x%x\n", inode->i_rdev);
	printk("major is %d\n", MAJOR(inode->i_rdev));
	printk("minor is %d\n", MINOR(inode->i_rdev));

	led_no = MINOR(inode->i_rdev);

	return 0;
}

int char_release(struct inode * inode, struct file * fb) 
{
	printk("---> char release called! \n");

	return 0;
}

ssize_t char_read(struct file * a, char __user * buf, size_t count, loff_t * d) 
{
	static int flag = 0;
	int i;

	printk("---> char read called! \n");
	printk("buf = 0x%x\n",(int)buf);

	for (i = 0; i < 7; i++)
		*buf++ = 'A' + i;
	*buf = '\n';

	flag++;

	if (flag == 2)
		return 0;
	else 
		return 8;
}

#define PA_TO_VA(x)		ioremap(x, 4)	
//#define PA_TO_VA(x)		(x)	

ssize_t char_write(struct file * a, const char __user * buf, size_t count, loff_t * d) 
{
	printk("---> char write called! \n");
	printk("count = %d\n", count);
	printk("buf = 0x%x\n",(int)buf);
	printk("*buf = %c(%x)\n", *buf, *buf);

	printk("led no = %d\n", led_no);

	if (*buf & 0x1)
//		*(int *)(ioremap(0xe0200284,4)) &= ~(1 << led_no);
		*(int *)(PA_TO_VA(0xe0200284)) &= ~(1 << led_no);
	else
//		*(int *)(ioremap(0xe0200284,4)) |= 1 << led_no;
		*(int *)(PA_TO_VA(0xe0200284)) |= (1 << led_no);

	return count;
}

struct file_operations char_fops =
{
	.owner = THIS_MODULE,
	.open = char_open,
	.release = char_release,

	.read = char_read,
	.write = char_write,
};

int char_init(void)
{
	int rc;

	printk("char init ok \n");
	printk("MAJOR = %d\n", CHAR_MAJOR);

#if 0
	char_fops.open = char_open;
	char_fops.release = char_release;

	char_fops.read = char_read;
	char_fops.write = char_write;
#endif

	// register char dev
	rc = register_chrdev(CHAR_MAJOR, CHAR_NAME, &char_fops);
	printk("rc = %d\n", rc);

	return 0;
}

void char_exit(void)
{
	printk("char exit ok \n");

	// unregister
	unregister_chrdev(CHAR_MAJOR, CHAR_NAME);

	return;
}

module_init(char_init);
module_exit(char_exit);
