
#define UART_BASE	0x50000000

#define ULCON	(*(volatile unsigned int *)(UART_BASE+0x0))
#define UCON	(*(volatile unsigned int *)(UART_BASE+0x4))
#define UBRDIV	(*(volatile unsigned int *)(UART_BASE+0x28))

#define UTRSTAT	(*(volatile unsigned int *)(UART_BASE+0x10))
#define UTXH 	(*(volatile unsigned int *)(UART_BASE+0x20))
#define URXH 	(*(volatile unsigned int *)(UART_BASE+0x24))

#define MPLLCON	(*(volatile unsigned int *)(0x4C000004))
#define CLKDIVN	(*(volatile unsigned int *)(0x4C000014))
#define GPHCON	(*(volatile unsigned int *)(0x56000070))

void uart_putchar(char c)
{
	// polling status
	while ((UTRSTAT & (1<<2)) == 0)
		;

	UTXH = c;
	
	return;
}

char uart_getchar(void)
{
	char c;
	
	// polling status
	while ((UTRSTAT & (1<<0)) == 0)
		;
	
	c = URXH;
	
	return c;
}

void uart_init(void)
{
	// 1. clock init
//	MPLLCON = 0x44011;
	CLKDIVN = 0x7;
	
	// 2. gpio init
	GPHCON = 0xaa0;		// UART0 & UART1
	
	// 3. uart sfr init
	ULCON = 0x3;
	UCON = 0x5;
	UBRDIV = 0x1a;
	
	return;
}
