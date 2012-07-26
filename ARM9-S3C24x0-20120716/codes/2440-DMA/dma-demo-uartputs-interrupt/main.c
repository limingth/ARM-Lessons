#define UTRSTAT0	(*(volatile unsigned int *)(0x50000010))
#define UTXH0	(*(volatile unsigned int *)(0x50000020))
#define UCON0		(*(volatile unsigned int *)0x50000004)  

#define GPFCON	(*(volatile unsigned int *)(0x56000050))
#define GPFDAT	(*(volatile unsigned int *)(0x56000054))

#define EXTINT0 (*(volatile unsigned int *)(0x56000088))

#define INTMSK (*(volatile unsigned int *)(0x4A000008))
#define INTMOD (*(volatile unsigned int *)(0x4A000004))

#define SRCPND	(*(volatile unsigned int *)(0X4A000000))
#define INTPND 	(*(volatile unsigned int *)(0X4A000010))

#define DISRC0		(*(volatile unsigned int *)0x4B000000)
#define DISRCC0		(*(volatile unsigned int *)0x4B000004) 
#define DIDST0		(*(volatile unsigned int *)0x4B000008)
#define DIDSTC0		(*(volatile unsigned int *)0x4B00000C)
#define DCON0		(*(volatile unsigned int *)0x4B000010)
#define DSTAT0		(*(volatile unsigned int *)0x4B000014)
#define DCSRC0		(*(volatile unsigned int *)0x4B000018)
#define DMASKTRIG0	(*(volatile unsigned int *)0x4B000020)

void uart_putchar(char ch)
{
	while (!(UTRSTAT0 & (1<<2)))
		;
		
	UTXH0= ch;
	
	return;
}

void delay(void)
{
	int i = 0;
	
	for (i = 0; i < 0x100000; i++)
		;
}

char * p = "123456789";

void irq_handler(void)
{
	static int size = 9;

	uart_putchar('^');

	SRCPND = 1<<17;
	INTPND = 1<<17;
	
	p++;
	size--;
	
	if(*p)
	{
		DISRC0 = (int)p;
		DCON0 &= 0xfff00000;
		DCON0 |= size;
		DMASKTRIG0 = 1<<1;
	}
			
	delay();
}

void asm_irq_handler(void);

int c_entry(void)
{
	// install 0x18 handler
	// ldr pc, [pc, #0]
	*(int *)0x18 = 0xE59ff000;
	*(int *)0x20 = (int)asm_irq_handler;

	/* init UCON0 Transmit Mode & Receive Mode - DMA mode */	
	// Transmit Mode [3:2] 10 = DMA0 request (Only for UART0),
	// Receive Mode [1:0] 10 = DMA0 request (Only for UART0),
	UCON0 |= 1<<3 | 1<<1;
	UCON0 &= ~(1<<2 | 1<<0);

#if 1
	/* use DMA to set uart = p */
	DISRC0 = (int)p;
	DISRCC0 = 0x0;
	DIDST0 = (int)0x50000020;	
	
	/* set APB and fixed address */
	DIDSTC0 = 1<<1 | 1<<0;
	
	/* (1<<24) DMA request source 001:UART0 */ 
	/* (1<<23) DMA triggers is hardware */
	/* (1<<22) DMA channel (DMA REQ) is turned off after finish */
	DCON0 = (1<<24)|(1<<23)|(1<<22);
	
	/* set len=strlen("123456789") */
	DCON0 |= 9<<0;
	
	/* Enable/Disable the interrupt setting */
	DCON0 |= 1<<29;
		
	DMASKTRIG0 = 1<<1;	/* do not need SW_TRIG */
#endif

	/* clear MASK bit 17, unmask DMA0 interrupt */
	INTMSK &= ~(1<<17);

	// step 3: enable IRQ : CPSR I-bit = 0
	// 0x53 : 0b 0101 0011
	__asm
	{
		mov r0, #0x53
		msr CPSR_cxsf, r0
	}
	
	while(1);

	return 0;
}










