#include <linux/module.h>
#include <linux/fs.h>	// register_chrdev
#include <asm/io.h>
//#include <asm/uaccess.h>

#define MA	243

MODULE_LICENSE("GPL");

struct uart_sfr
{
	int ulcon;
	int ucon;
	int ufcon;
	int umcon;
	int utrstat;
	int uerstat;
	int ufstat;
	int umstat;
	int utxh;
	int urxh;
	int ubrdiv;
};

typedef struct uart_sfr USFR;

static volatile USFR *puart;

//#define printk 		noprintk	

int noprintk(char * fmt, ...)
{
	return 0;
}

#define PRINT(x)	printk(#x " = 0x%x\n", x);

int uart_open(struct inode * inode, struct file * filp)
{
	int major = MAJOR(inode->i_rdev);
	int minor = MINOR(inode->i_rdev);
	int * p;
	int i;

	printk("uart open: major %d, minor %d\n", major, minor);

	puart = ioremap(0xe2900000, sizeof(USFR));
	p = (int *)puart;

	PRINT((int)p);	

	for (i = 0; i < sizeof(USFR)/4; i++)
	{
		PRINT(*p++);	
	}

	puart->ufcon = 0;

	return 0;
}

int uart_release(struct inode * inode, struct file * filp)
{
	printk("uart release\n");

	return 0;
}

int uart_putchar(char c)
{
	while ((puart->utrstat & (1<<2)) == 0)
		;

	puart->utxh = c;

	return 0;
}

int myputchar(char c)
{
	if (c == '\n')
		uart_putchar('\r');

	uart_putchar(c);

	return 0;
}

int uart_write(struct file * filp, const char __user * buf, size_t count, loff_t *f_pos)
{
	int i;

	printk("uart write\n");

	printk("buf = %c\n", buf[0]);
	printk("count = %d\n", count);


	for (i = 0; i < count; i++)
	{
		myputchar(buf[i]);
	}


	return count;
}

int uart_ioctl(struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg)
{
	printk("uart ioctl\n");

	printk("cmd = %d\n", cmd);

	if (cmd == 19200)
		puart->ubrdiv = 0xd5;

	if (cmd == 115200)
		puart->ubrdiv = 0x23;

	return 0;
}

struct file_operations uart_fops = 
{
	.owner = THIS_MODULE,
	.open = uart_open,
	.release = uart_release,
	.write = uart_write,
	.ioctl = uart_ioctl,
};

int uart_init(void)
{
	int rc;

	rc = register_chrdev(MA, "myuart", &uart_fops);

	if (rc < 0)
	{
		printk("register chrdev failed! %d\n", rc);
		return -1;
	}

	printk("uart init ok \n");

	return 0;
}

void uart_exit(void)
{
	printk("uart exit ok \n");

	unregister_chrdev(MA, "myuart");

	return;
}


module_init(uart_init);
module_exit(uart_exit);
