#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/irq.h>

MODULE_LICENSE("GPL");

#define CHAR_MAJOR		240
#define CHAR_NAME		"mychar"

static int led_no = 0;
static int key_no = 0;

wait_queue_head_t wq;
static int btn_pressed = 0;

#define printk	debug	

int debug(const char * fmt, ...)
{

	return 0;
}


static irqreturn_t k1_handler(int irq, void * dev_id)
{
	printk("K1 pressed! \n");

	key_no = 1;

	btn_pressed = 1;
	wake_up_interruptible(&wq);

	printk("wake up ok!\n");

	return IRQ_HANDLED;
}

static irqreturn_t k2_handler(int irq, void * dev_id)
{
	printk("K2 pressed! \n");

	key_no = 2;

	btn_pressed = 1;
	wake_up_interruptible(&wq);

	printk("wake up ok!\n");

	return IRQ_HANDLED;
}

int char_open(struct inode * inode, struct file * fp) 
{
	int ri;

	printk("---> char open called! \n");

	printk("dev is 0x%x\n", inode->i_rdev);
	printk("major is %d\n", MAJOR(inode->i_rdev));
	printk("minor is %d\n", MINOR(inode->i_rdev));

	led_no = MINOR(inode->i_rdev);

	ri = request_irq(160, k1_handler, IRQ_TYPE_EDGE_FALLING, "mybtnk1", NULL);
	ri = request_irq(161, k2_handler, IRQ_TYPE_EDGE_FALLING, "mybtnk2", NULL);

	init_waitqueue_head(&wq);

	return 0;
}

int char_release(struct inode * inode, struct file * fb) 
{
	printk("---> char release called! \n");

	free_irq(160, NULL);
	free_irq(161, NULL);

	return 0;
}

ssize_t char_read(struct file * a, char __user * buf, size_t count, loff_t * d) 
{
	static int counter = 0;

	counter++;

	printk("read: counter = %d, count = %d\n", counter, count);

	wait_event_interruptible(wq, btn_pressed);
	printk("read: btn_pressed = %d\n", btn_pressed);

	*buf = key_no + '0';
	btn_pressed = 0;
	printk("read: key_no = %d\n", key_no);

#if 0
	if (counter & 0x1)
		*buf = '1';
	else
		*buf = '2';
#endif

	return 1;	

	if (counter == 5)
		return 0;
	else
		return 1;	
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
