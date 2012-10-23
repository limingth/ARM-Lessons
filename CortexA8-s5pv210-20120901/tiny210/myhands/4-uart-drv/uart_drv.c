#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>

MODULE_LICENSE("GPL");

#define UART_MAJOR	245

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
	int udivslot;
};

typedef struct uart_sfr USFR;

static volatile USFR *puart;

void uart_init(void)
{
#if 0
	// see how linux set UART0 regs
	puart = ioremap(0xe2900000, sizeof(USFR));
	printk("reg ulcon = %x\n", puart->ulcon);
	printk("reg ucon = %x\n", puart->ucon);
	printk("reg ubrdiv = %x\n", puart->ubrdiv);
	printk("reg udivslot = %x\n", puart->udivslot);
#endif

	puart = ioremap(0xe2900c00, sizeof(USFR));
	puart->ulcon = 0x3;
	puart->ucon = 0x7c5;
	puart->ubrdiv = 0x23;
	puart->udivslot = 0x808;

	return;
}

int uart_putchar(char c)
{
	while ((puart->utrstat & (1<<2)) == 0)
		;

	puart->utxh = c;

	return 0;
}

char uart_getchar(void)
{
	char c;

	while ((puart->utrstat & (1<<0)) == 0)
		;

	c = puart->urxh;

	return c;
}

int uart_drv_open(struct inode * inode, struct file * filp)
{
	printk("uart open\n");

	uart_init();

	uart_putchar('o');
	uart_putchar('p');
	uart_putchar('e');
	uart_putchar('n');
	uart_putchar('\n');

	return 0;
}

int uart_drv_release(struct inode * inode, struct file * filp)
{
	printk("uart release\n");

	uart_putchar('c');
	uart_putchar('l');
	uart_putchar('o');
	uart_putchar('s');
	uart_putchar('e');
	uart_putchar('\n');

	return 0;
}

static char kbuf[4096];

int uart_drv_write(struct file * filp, const char __user * buf, size_t count, loff_t *f_pos)
{
	char c;
	int i;

	printk("uart write %d bytes\n", count);

//	c = *buf;

	if (count > 4096)
		printk("error!\n");

	copy_from_user(kbuf, buf, count);

	for (i = 0; i < count; i++)
	{
		c = kbuf[i];
		//printk("%c", c);

		uart_putchar(c);

		if (c == '\n')
			uart_putchar('\r');
	}

	return count;
}

int uart_drv_read(struct file * filp, char __user * buf, size_t count, loff_t *f_pos)
{
	char c;

	printk("uart read %d bytes\n", count);
	printk("read buf user: %p\n", buf);

	c = uart_getchar();

//	*buf = c;

	*kbuf = c;
	copy_to_user(buf, kbuf, 1);

	return 1;
}

struct file_operations uart_drv_fops = 
{
    .owner = THIS_MODULE,
    .open = uart_drv_open,
    .release = uart_drv_release,
    .write = uart_drv_write,
    .read = uart_drv_read,
};

struct cdev uart_cdev;
dev_t uart_dev_no;

int uart_drv_init(void)
{
	printk("uart_drv init ok \n");

	//register_chrdev(UART_MAJOR, "myttyS3", &uart_drv_fops);

	// use cdev
	uart_dev_no = MKDEV(UART_MAJOR, 3);
	printk("dev no = 0x%x\n", (int)uart_dev_no);
	register_chrdev_region(uart_dev_no, 1, "myttyS3");
	cdev_init(&uart_cdev, &uart_drv_fops);
	cdev_add(&uart_cdev, uart_dev_no, 1);

	return 0;
}

void uart_drv_exit(void)
{
	printk("uart_drv exit ok \n");

	//unregister_chrdev(UART_MAJOR, "myttyS3");
	
	// use cdev 
	unregister_chrdev_region(uart_dev_no, 1);
	cdev_del(&uart_cdev);

	return;
}

module_init(uart_drv_init);
module_exit(uart_drv_exit);
