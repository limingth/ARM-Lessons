#include <linux/kernel.h>   // printk()
#include <linux/module.h>   // module_init()

//int myglobal = 100;

extern int myglobal;

int myinit2(void)
{
	int local = 200;

	printk("hello, my init\n");
	printk("init = %p\n", myinit2);
	printk("global = %p\n", &myglobal);
	printk("myglobal = %d\n", myglobal);
	printk("local = %p\n", &local);

	printk("jiffies = %ld\n", jiffies);
	printk("&jiffies = %p\n", &jiffies);

	return 0;
}

void myexit2(void)
{
	printk("goodbye, my exit\n");

	return;
}

module_init(myinit2);
module_exit(myexit2);
