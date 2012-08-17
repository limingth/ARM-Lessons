/*********************************************
 * Function: module example
 * Author: asmcos@gmail.com
 * Date: 2005-08-24
 * $Id: kegui.c, v 1.6 2006/06/22 13:20:50 asmcos Exp $
 *********************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>

MODULE_AUTHOR("Asmcos");
MODULE_DESCRIPTION("module example ");
MODULE_LICENSE("GPL");
static int counter=0;
static char * p= "hello\n";
module_param(counter, int, 0644);
module_param(p, charp, 0);

int __init
akae_init(void)
{

	printk("Hello , world\n");
	printk("I am WangBo\n");
	printk("module name is %s\n", KBUILD_MODNAME);
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(10*HZ);
	printk("HZ=%d\n", HZ);
	printk("counter=%d\n", counter);
	printk("p=%s\n", p);
	return 0;
}

void __exit
akae_exit(void)
{
	printk("module exit\n");
	return;
}

module_init(akae_init);
module_exit(akae_exit);
