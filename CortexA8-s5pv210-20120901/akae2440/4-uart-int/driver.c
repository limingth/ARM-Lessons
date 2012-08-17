#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <asm/irq.h>         /* 它包含了<mach/irqs.h>，里面有IRQ_S3CUART_RX1  IRQ_S3CUART_TX1 */
#include <linux/interrupt.h> /* IRQF_TRIGGER_FALLING IRQF_TRIGGER_RISING …… */
#include <linux/irq.h>       /* set_irq_type() */
#include <linux/serial_core.h>
#include <asm-arm/plat-s3c/regs-serial.h> /* S3C24XX_VA_UART1 */
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/sched.h>  /* schedule_timeout */

#define iobase S3C24XX_VA_UART1  //内核中已经映射的Uart寄存器首地址
#define UART_ULCON1 iobase
#define UART_UCON1 iobase + 0x04
#define UART_UFCON1 iobase + 0x08
#define UART_UBRDIV1 iobase + 0x28
#define UART_UTXH1 iobase + 0x20
#define UART_URXH1 iobase + 0x24
#define UART_UTRSTAT1 iobase + 0x10

#define DEVICE_NAME 	"serial_char"
#define ADC_MAJOR		260

MODULE_AUTHOR("Wang");
MODULE_DESCRIPTION("module example ");
MODULE_LICENSE("GPL");

void ser_init_uart(void);

char ch_w;
char kwbuf[100];
wait_queue_head_t sched_wq;
wait_queue_head_t sched_wq_w;
int sched_flag = 0;
int sched_flag_w = 0;
dev_t dev;

static irqreturn_t serial_read_irq_handler(int irq, void *dev_id)
{
	printk("in read irq!!!\n");
	
	
	sched_flag = 1;
	wake_up_interruptible(&sched_wq); //唤醒read
	
	return IRQ_HANDLED;
}

static irqreturn_t serial_write_irq_handler(int irq, void *dev_id)
{
	static int i = 0;
	

	printk("in write irq!!!ch_w=%c\n", ch_w);
	printk("in write irq!!!i=%d\n", ++i);
	writeb(ch_w, UART_UTXH1);

	disable_irq_nosync(IRQ_S3CUART_TX1);//发送数据写入UTXH1寄存器后，禁止发送中断。没有这句，发送中断一注册就使能了，不停发生中断（中断优先级最高），造成模块入口函数的剩下的代码根本不执行的问题。

	return IRQ_HANDLED;
}

static ssize_t serial_read(struct file *file, char  *buf, size_t count, loff_t *ppos)
{
	char ch;
	printk("in serial read.\n");
	count = 1;
	//休眠
	wait_event_interruptible(sched_wq, sched_flag != 0);
	sched_flag = 0;
	
	ch = readb(UART_URXH1);
	printk("irq read %c\n", ch);
	
	if(copy_to_user(buf, &ch, count)) 
		return -1;	

	return count;
}

static ssize_t serial_write(struct file *file, const char  *buf, size_t count, loff_t *ppos)
{
	count = 1;

	if(copy_from_user(&ch_w, buf, count)) 
		return -1;
	printk("in serial write. ch=%c\n", ch_w);

	//开写中断，准备发送
	enable_irq(IRQ_S3CUART_TX1);
	
	return count;
}

void ser_init_uart(void)
{
	/* UFCON1 */
	writel(0, UART_UFCON1); //00000000
	
	/* 8N1 */
	writel(3, UART_ULCON1); //00000011

	/* poll mode */
	writel(5, UART_UCON1); //00000101 //采用pulse模式

	/* 115200 */
	writel(26, UART_UBRDIV1);

	return;
}

static int serial_open(struct inode * inode, struct file * filp)
{

	ser_init_uart();
	
 	printk("serial 1 opened!\n");

	printk("irq enabled!\n");

 	return 0;
}

static int serial_release(struct inode * inode, struct file * filp)
{
 	printk("serial 1 closed!\n");
 
 	return 0;
}

static struct file_operations serial_fops = {
 	.owner = THIS_MODULE,
 	.open = serial_open,
 	.read = serial_read, 
	.write = serial_write,
 	.release = serial_release,
};


int __init
serial_init(void)
{
	int rc;
	int rc_irq;
	
	rc = register_chrdev(ADC_MAJOR, DEVICE_NAME, &serial_fops);
	if (rc < 0) {
		printk("register %s serial char dev driver error\n", DEVICE_NAME);
		return -1;
	}
	printk("install serial device driver success\n");

	init_waitqueue_head(&sched_wq);
	init_waitqueue_head(&sched_wq_w);

	set_irq_type(IRQ_S3CUART_RX1, IRQF_TRIGGER_FALLING);
	set_irq_type(IRQ_S3CUART_TX1, IRQF_TRIGGER_FALLING);

	rc_irq = request_irq(IRQ_S3CUART_RX1, serial_read_irq_handler, IRQF_DISABLED, "serial_1_read_irq", NULL);
	if (rc_irq)
	{
		printk("serial read irq not registered. Err:%d\n", rc_irq);
	}
	printk("serial read irq registered success.\n");

	rc_irq = request_irq(IRQ_S3CUART_TX1, serial_write_irq_handler, IRQF_DISABLED, "serial_1_write_irq", NULL); 
	if (rc_irq)
	{
		printk("serial write irq not registered. Err:%d\n", rc_irq);
	}
	printk("serial write irq registered success.\n");

	printk("my serial driver starts at this moment.\n");
	
	return 0;
}

void __exit
serial_exit(void)
{
	disable_irq(IRQ_S3CUART_RX1);
	disable_irq(IRQ_S3CUART_TX1);
	free_irq(IRQ_S3CUART_RX1,NULL);
	free_irq(IRQ_S3CUART_TX1,NULL);
	unregister_chrdev(ADC_MAJOR, DEVICE_NAME);

	printk("module exit\n");
	return;
}

module_init(serial_init);
module_exit(serial_exit);
