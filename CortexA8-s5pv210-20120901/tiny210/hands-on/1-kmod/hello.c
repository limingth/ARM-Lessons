/*********************************************
 * Function: module example
 * Author: asmcos@gmail.com
 * Date: 2005-08-24
 * $Id: kegui.c, v 1.6 2006/06/22 13:20:50 asmcos Exp $
 *********************************************/

#include <linux/init.h>
#include <linux/module.h>

//MODULE_LICENSE("GPL");

int __init
akae_init(void)
{
	int local = 0;
	printk("I am liming\n");
	printk("module name is %s\n", KBUILD_MODNAME);
	printk("akae_init at %p\n", akae_init);
	printk("local at %p\n", &local);
	printk("jiffies at %p\n", &jiffies);
	printk("jiffies is %ld\n", jiffies);
	return 0;
}

void __exit
akae_exit(void)
{
	printk("module liming exit\n");
	printk("akae_exit at %p\n", akae_exit);
	return;
}

module_init(akae_init);
module_exit(akae_exit);
