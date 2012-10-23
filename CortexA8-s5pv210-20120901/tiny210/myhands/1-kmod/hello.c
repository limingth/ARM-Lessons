#include <linux/kernel.h>   // printk()
#include <linux/module.h>   // module_init()

int myglobal = 100;
EXPORT_SYMBOL(myglobal);

int myinit(void)
{
	int local = 200;

	printk("hello, my init\n");
	printk("init = %p\n", myinit);
	printk("global = %p\n", &myglobal);
	printk("myglobal = %d\n", myglobal);
	printk("local = %p\n", &local);

	printk("jiffies = %ld\n", jiffies);
	printk("&jiffies = %p\n", &jiffies);

	return 0;
}

void myexit(void)
{
	printk("goodbye, my exit\n");

	return;
}

module_init(myinit);
module_exit(myexit);
