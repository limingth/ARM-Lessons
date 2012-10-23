#include <linux/kernel.h>
#include <linux/module.h>
//#include <linux/moduleparam.h>

#include <asm/io.h>		// ioremap()

MODULE_AUTHOR("liming");
MODULE_DESCRIPTION("for all akaedu students");
MODULE_VERSION("version 1.0");
MODULE_ALIAS("akae");
//MODULE_DEVICE_TABLE("gpio, uart, net");
MODULE_LICENSE("GPL");

static int * pled;

static int count = 3;
module_param(count, int, 0);

static char * str = "welcome";
module_param(str, charp, 0);

int led_init(void)
{
//	void * (*fp)(long, int, int);
	int i;

	printk("led module init\n");

	for (i = 0; i < count; i++)
		printk("str = %s, count=%d\n", str, count);

	printk("ioremap = %p\n", __arm_ioremap);

	// all 4 led on 
	//*(int *)0xE0200284 = 0;		// error!
	pled = ioremap(0xE0200284, 4);
#if 0
	//pled = __arm_ioremap(0xE0200284, 4, 0);
	printk("using fp !\n");
	fp = 0xc017695c;
	pled = fp(0xE0200284, 4, 0);
#endif

	*pled = 0;

	printk("led on ok!\n");

	return 0;
}

void led_exit(void)
{
	printk("led module exit\n");

	// all 4 led off 
	*pled = 0xF;

	printk("led off ok!\n");

	return;
}

module_init(led_init);
module_exit(led_exit);
