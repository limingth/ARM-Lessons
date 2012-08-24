
#include <linux/module.h>	// module_init
#include <asm/io.h>		// ioremap
#include <linux/fs.h>		// file_operations
#include <asm/uaccess.h>	// copy_from_user
#include <linux/sched.h>	// wait_queue 

#include <mach/gpio.h>		// S5PV210_GPH2
#include <mach/regs-gpio.h>
#include <linux/interrupt.h>	// requst_irq
#include <linux/irq.h>		// IRQ_TYPE_EDGE_BOTH


MODULE_LICENSE("GPL");

#define MA	240

volatile int * pbtn;
static int ev_press = 0;

wait_queue_head_t  btn_waitq;

static int irq;

struct btn_desc 
{
	int gpio;
	int number;
	char * name;
};

static struct btn_desc btns[] = 
{
	{ S5PV210_GPH2(0), 0, "KEY0" },
	{ S5PV210_GPH2(1), 1, "KEY0" },
	{ S5PV210_GPH2(2), 2, "KEY0" },
};


static irqreturn_t btn_irq_handler(int irq, void * dev_id)
{
	printk("btn irq handler !!!\n");

	ev_press = 1;
	wake_up_interruptible(&btn_waitq);

	return IRQ_HANDLED;
}

int btn_drv_open(struct inode *inode, struct file *filp)
{
	int major, minor;
	int err;

	major = MAJOR(inode->i_rdev);
	minor = MINOR(inode->i_rdev);

	printk("btn drv open: major %d, minor %d\n", major, minor);

	irq = gpio_to_irq(btns[0].gpio);
	printk("irq = %d\n", irq);

	// see include/linux/irq.h for more types
	//err = request_irq(irq, btn_irq_handler, IRQ_TYPE_EDGE_BOTH,
	err = request_irq(irq, btn_irq_handler, IRQ_TYPE_EDGE_FALLING,
			"btn0", NULL);
	if (err)
	{
		printk("request irq failed!\n");
	}

	printk("request irq ok! err = %d\n", err);

	init_waitqueue_head(&btn_waitq);

	return 0;
}

ssize_t btn_drv_read(struct file *filp, char __user * buf, size_t count, loff_t *f_pos)
{
	printk("btn read!\n");	

	wait_event_interruptible(btn_waitq, ev_press);
	ev_press = 0;

	return 0;
}

ssize_t btn_drv_write(struct file *filp, const char __user * buf, size_t count, loff_t *f_pos)
{
	//char kbuf[128];

	//buf[count] = '\0';
	printk("btn drv write %d\n", count);

//	printk("buf = %s\n", buf);
//	printk("buf at %p\n", buf);
	printk("count = %d\n", count);

//	copy_from_user(kbuf, buf, count);
	*pbtn = buf[0];

//	printk("kbuf = %s\n", kbuf);
//	printk("kbuf at %p\n", kbuf);

	return count;
}

int btn_drv_release(struct inode *inode, struct file *filp)
{
	printk("btn drv release ok!\n");

	free_irq(irq, NULL);

	return 0;
}

struct file_operations btn_fops = 
{
	.owner = THIS_MODULE,
	.open = btn_drv_open,
	.read = btn_drv_read,
	.write = btn_drv_write,
	.release = btn_drv_release,
};

static int btn_drv_init(void)
{
	int rc;

	printk("btn init \n");

	pbtn = ioremap(0xE0200284, 4);

	//*pbtn = 0;
	rc = register_chrdev(MA, "akae", &btn_fops);
	if (rc < 0)
	{
		printk("register failed\n");
		return -1;
	}

	printk("register char ok %d!\n", rc);

	return 0;
}

static void btn_drv_exit(void)
{
	printk("btn exit \n");

	//*pbtn = 0xF;
	unregister_chrdev(MA, "akae");

	printk("unregister char ok!\n");

	return;
}

module_init(btn_drv_init);
module_exit(btn_drv_exit);

