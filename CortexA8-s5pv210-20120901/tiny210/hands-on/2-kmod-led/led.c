/*********************************************
 * Function: module example
 * Author: asmcos@gmail.com
 * Date: 2005-08-24
 * $Id: kegui.c, v 1.6 2006/06/22 13:20:50 asmcos Exp $
 *********************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/ioctl.h>
#include <linux/cdev.h>
#include <linux/delay.h>

#include <mach/gpio.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

#include <linux/sched.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");

static int led_gpios[] = {
	S5PV210_GPJ2(0),
	S5PV210_GPJ2(1),
	S5PV210_GPJ2(2),
	S5PV210_GPJ2(3),
};

volatile int * pled;

int __init
akae_init(void)
{
	printk("led driver installed\n");

//	gpio_set_value(led_gpios[2], 0);
//	*(volatile int *)0xE0200284 = 0x0;
	pled = ioremap(0xE0200284, 4);
	*pled = 0x0;

	printk("led on \n");
	printk("pled is %p\n", pled);
	printk("gpio_set_value is %p\n", gpio_set_value);
	printk("MISC_DYNAMIC_MINOR is %d\n", MISC_DYNAMIC_MINOR);

	return 0;
}

void __exit
akae_exit(void)
{
	printk("led driver exit\n");

//	gpio_set_value(led_gpios[2], 1);
	//*(volatile int *)0xE0200284 = 0xf;
	*pled = 0xF;

	printk("led off \n");
	printk("pled is %p\n", pled);

	return;
}

module_init(akae_init);
module_exit(akae_exit);
