#include <linux/kernel.h>   // printk()
#include <linux/module.h>   // module_init()
#include <linux/interrupt.h>	// request_irq()
#include <linux/irq.h>		// IRQ_TYPE_EDGE_FALLLING
#include <mach/gpio.h>		// S5PV210_XXX

MODULE_LICENSE("GPL");

//irq_handler_t k1_handler;
// see linux/interrupt.h
// typedef irqreturn_t (*irq_handler_t)(int, void *);

// see linux/irqreturn.h
// typedef enum irqreturn irqreturn_t;

irqreturn_t k1_handler(int irq, void * dev_id)
{
	printk("k1 handler called \n");
	printk("irq = %d\n", irq);
	return IRQ_HANDLED;
}

irqreturn_t k2_handler(int irq, void * dev_id)
{
	printk("k2 handler called \n");
	printk("irq = %d\n", irq);
	return IRQ_HANDLED;
}

int irq;
int myinit(void)
{
	int ri;

	printk("hello, my init\n");
	printk("install k1 button irq handler \n");
	
//	irq = 160;
	//irq = gpio_to_irq(S5PV210_GPH2(0));
	printk("S5PV210_GPH2(0) = %d \n", S5PV210_GPH2(0));

	irq = gpio_to_irq(146);
	printk("gpio to irq = %d \n", irq);

	ri = request_irq(irq, k1_handler, IRQ_TYPE_EDGE_FALLING, "mybtnirq", NULL); 
	printk("ri = %d \n", ri);

	return 0;
}

void myexit(void)
{
	printk("goodbye, my exit\n");
	free_irq(160, NULL);

	return;
}

module_init(myinit);
module_exit(myexit);
