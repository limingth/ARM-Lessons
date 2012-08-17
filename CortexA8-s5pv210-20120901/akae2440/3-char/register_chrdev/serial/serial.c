/*********************************************
 * Function: module example
 * Author: asmcos@gmail.com
 * Date: 2005-08-24
 * $Id: kegui.c, v 1.6 2006/06/22 13:20:50 asmcos Exp $
 *********************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/uaccess.h> /* copy_to_user, copy_from_user */
#include <linux/serial_core.h> /*此处相对2410有更改*/
#include <asm/plat-s3c/regs-serial.h> /* 寄存器宏 */ /*此处相对2410有更改*/
#include <asm/io.h>
#include <linux/fs.h>
#include <linux/sched.h>

#define AKAE_MAJOR 240

#define UART_UFCON1	0x50004008
#define UART_ULCON1	0x50004000
#define UART_UCON1	0x50004004
#define UART_UBRDIV1	0x50004028
#define UART_UTXH1	0x50004020
#define UART_URXH1	0x50004024
#define UART_UTRSTAT1	0x50004010

MODULE_AUTHOR("Asmcos");
MODULE_DESCRIPTION("module example ");
MODULE_LICENSE("GPL");

volatile unsigned int *VM_UFCON1;
volatile unsigned int *VM_ULCON1;
volatile unsigned int *VM_UCON1;
volatile unsigned int *VM_UBRDIV1;
volatile unsigned char *VM_UTXH1;
volatile unsigned char *VM_URXH1;
volatile unsigned int *VM_UTRSTAT1;



void ser_init(void);

int my_open(struct inode *inode, struct file *filp)
{
	ser_init();//初始化串口设备

	printk("my serial init ok!\n");

	return 0;
}

ssize_t my_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	int state;
	char ch;

	while(1)
	{
		state = readl(VM_UTRSTAT1);
		if((0x01 & state) == 1)
		{
			ch = readb(VM_URXH1);
			break;
		}
		else
		{
			set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(10);
		}

	}

	if(count > 1)
		count = 1;
	//copy_to_user(buf, &ch, count);
	put_user(ch, buf);//简单数据类型(int、char)可以调用put_user()函数发送给用户空间

	return count;
}

ssize_t my_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
	int state;
	int i = 0;
	//char *str = "this is my serial test.";
	char str[200];
	if(count > 200)
		count = 200;
	copy_from_user(str, buf, count);
	str[count] = 0;
	printk("user write:%s. Now we transmit it to Uart!\n", str);

	//把应用程序write过来的数据发送给串口
	while(*(str+i) != '\0')
	{
		state = readl(VM_UTRSTAT1);
		if((0x02 & state) == 2)
		{
			writeb(*(str+i), VM_UTXH1);
			i++;
		}
	}

	return count;
}

int my_release(struct inode *inode, struct file *filp)
{
	return 0;
}

void ser_init(void)
{
	//map the serial device registers
	VM_UFCON1 = ioremap(UART_UFCON1, 4);
	VM_ULCON1 = ioremap(UART_ULCON1, 4);
	VM_UCON1 = ioremap(UART_UCON1, 4);
	VM_UBRDIV1 = ioremap(UART_UBRDIV1, 4);
	VM_UTXH1 = ioremap(UART_UTXH1, 1);
	VM_URXH1 = ioremap(UART_URXH1, 1);
	VM_UTRSTAT1 = ioremap(UART_UTRSTAT1, 4);

	/* 8N1 */
	writel(3, VM_ULCON1); //00000011

	/* poll mode */
	writel(5, VM_UCON1); //00000101

	/* 115200 */
	writel(26, VM_UBRDIV1);

	/* 关闭FIFO */
	writel(0, VM_UFCON1);

	return;
}


//结构体初始化
struct file_operations my_fops ={
	.owner = THIS_MODULE,
	.open  = my_open,
	.read  = my_read,
	.write = my_write,
	.release = my_release,
};

int __init serial_init(void)
{
	int rc;

	//注册字符设备驱动
	rc = register_chrdev(AKAE_MAJOR, "my_serial", &my_fops);
	if(rc < 0)
	{
		printk("register %s char dev error\n", "my_serial");
		return -1;
	}

	printk("ok!\n");

	return 0;
}

void __exit serial_exit(void)
{
	//注销字符设备驱动
	unregister_chrdev(AKAE_MAJOR, "my_serial");
	printk("module exit\n");
	return;
}

module_init(serial_init);
module_exit(serial_exit);
