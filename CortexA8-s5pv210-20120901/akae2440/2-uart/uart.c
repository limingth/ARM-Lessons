/*
 *		c program for uart 
 *		author: 
 *		date: 
 *		version: 
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/sched.h>

#define UFCON1 0x50004008
#define ULCON1 0x50004000     /* ULCON1[0:1]=11 for 8bits data */
#define UCON1 0x50004004      /* 0101 = Interrupt request or polling mode  */
#define UBRDIV1 0x50004028   /* set 26 forR/W Baud rate divisior register 1   */
#define UTRSTAT1 0x50004010  
/*
 *[0]= 1 The buffer register has a received data 
 *[1]=1 transmit buffer register is empty.
 */
#define UTXH1 0x50004020      /*  UART channel 1 transmit buffer register */
#define URXH1 0x50004024      /*  UART channel 1 receive buffer register  */

typedef unsigned int uin_t;
typedef unsigned short ushor_t;
typedef unsigned char uchar_t;


MODULE_AUTHOR("SBSO");
MODULE_DESCRIPTION("UART1 TEST ");
MODULE_LICENSE("GPL");

volatile uin_t     *VM_ULCON1, *VM_UCON1, *VM_UTRSTAT1, *VM_UFCON1;
volatile ushor_t   *VM_UBRDIV1;
volatile uchar_t 	*VM_UTXH1, *VM_URXH1;

/*		init the uart1 to communication		*/
void sbso_uart_init(void)
{
	*VM_UFCON1 = 0;
	*VM_ULCON1 = 3;
	*VM_UCON1 = 5;
	*VM_UBRDIV1 = 26;
}
char sbso_uart_read(void)
{
	char ch;

	while ( !((*VM_UTRSTAT1) & (1 << 0)))
	{
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(10);
	}
			ch = *VM_URXH1;
			
			return ch;
}

int sbso_uart_write(char ch)
{
	while ( !((*VM_UTRSTAT1) & (1 << 1)))
		;
			
			*VM_UTXH1 = ch;
			
			return 0;
}
	int __init
sbso_init(void)
{
	char c;
	int k = 0;
	VM_ULCON1 = ioremap(ULCON1, 4);
	VM_UCON1 = ioremap(UCON1, 4);	
	VM_UTRSTAT1 = ioremap(UTRSTAT1, 4);
	VM_UFCON1 = ioremap(UFCON1, 4);

	VM_UBRDIV1 = ioremap(UBRDIV1, 2);

	VM_UTXH1 = ioremap(UTXH1, 1);
	VM_URXH1 = ioremap(URXH1, 1);

	sbso_uart_init();
	while ((k++) < 32)
	{
		c = sbso_uart_read();
		sbso_uart_write(c);	
		printk("%c",c);
	}

	return 0;
}

	void __exit
sbso_exit(void)
{
	iounmap(VM_ULCON1);
	iounmap(VM_UCON1);
	iounmap(VM_UTRSTAT1);
	iounmap(VM_UBRDIV1);
	iounmap(VM_UTXH1);
	iounmap(VM_URXH1);
	iounmap(VM_UFCON1);

	printk("module uart test exit...\n");
	return ;
}

module_init(sbso_init);
module_exit(sbso_exit);



