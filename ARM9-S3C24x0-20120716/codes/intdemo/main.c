
#define UART_BASE	0x50000000

#define UTRSTAT	(*(volatile unsigned int *)(UART_BASE+0x10))
#define UTXH 	(*(volatile unsigned int *)(UART_BASE+0x20))
#define URXH 	(*(volatile unsigned int *)(UART_BASE+0x24))

void uart_putchar(char c)
{
	// polling status
	while ((UTRSTAT & (1<<2)) == 0)
		;

	UTXH = c;
	
	return;
}

void __irq myhandler(void)
{	
	int i = 0;	
	for (i = 0; i < 10000; i++)
		;
		
	*(volatile int *)0x4A000000 = 1<<0;		// SRC
	*(volatile int *)0x4A000010 = 1<<0;		// INT
	
	uart_putchar('^');
	
	return;
}

void delay(void)
{
	int i = 0;	
	for (i = 0; i < 1000000; i++)
		;
}

int mymain(void)
{
	// fill memory 0x8 & 0x38
	// 0x8: ldr pc, [pc, #0x28]	swi
	// 0x18: ldr pc, [pc, #0x18]	irq
	// 0x38: handler address
	*(int *)0x18 = 0xe59ff018;
	*(int *)0x38 = (int)myhandler;

	// GPFCON = GPF0 -> EINT0
	*(volatile int *)0x56000050 = 0x2<<0;
	
	// EXTINT0 = 01x falling edge trigger
	*(volatile int *)0x56000088 = 0x2<<0;	
	
#if 1	// 2440 must include this!
	// GPB8 = 0
	*(volatile int *)0x56000010 = 1<<16 | 1<<0;	
	*(volatile int *)0x56000014 = 0;	
#endif

	// Pending bit (PND reg)
	// SRCPND
	*(volatile int *)0x4A000000 = 0xFFFFFFFF;
	*(volatile int *)0x4A000010 = 0xFFFFFFFF;
	*(volatile int *)0x4A000008 = 0xFFFFFFFE;
	
	// clear Mask bit 0
	*(volatile int *)0x4A000008 &= ~(1<<0);
	
	// clear CPSR I-bit
	__asm
	{
		mov r0, #0x53		// IRQ enable
		msr CPSR_cxsf, r0
	}
	
	while (1)
	{	
		uart_putchar('.');		
						
		delay();
	}
	
	while (1);
	
	return 0;
}