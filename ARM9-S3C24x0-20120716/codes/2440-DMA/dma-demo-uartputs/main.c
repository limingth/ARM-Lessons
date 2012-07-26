#define UTRSTAT0	(*(volatile unsigned int *)0x50000010)
#define UTXH0		(*(volatile unsigned int *)0x50000020)
#define UCON0		(*(volatile unsigned int *)0x50000004)  

#define DISRC0		(*(volatile unsigned int *)0x4B000000)
#define DISRCC0		(*(volatile unsigned int *)0x4B000004) 
#define DIDST0		(*(volatile unsigned int *)0x4B000008)
#define DIDSTC0		(*(volatile unsigned int *)0x4B00000C)
#define DCON0		(*(volatile unsigned int *)0x4B000010)
#define DSTAT0		(*(volatile unsigned int *)0x4B000014)
#define DCSRC0		(*(volatile unsigned int *)0x4B000018)
#define DCDST0		(*(volatile unsigned int *)0x4B00001C)
#define DMASKTRIG0	(*(volatile unsigned int *)0x4B000020)
 
void uart_putchar(char ch)
{
	while (!(UTRSTAT0 & (1<<2)))
		;
		
	UTXH0 = ch;
	
	return;
}

void puts(char * buf)
{
	while (*buf)
		uart_putchar(*buf++);

	uart_putchar('\r');
	uart_putchar('\n');
}

void delay(void)
{
	int i = 0;
	
	for (i = 0; i < 100000; i++)
		;
}

void dma_puts(char * buf, int size)
{
	// init dma		
	DISRC0 = (int)buf;
	DISRCC0 = 0x0;
	DIDST0 = (int)0x50000020;	
	DIDSTC0 = 1<<1 | 1<<0;	// set APB and fixed address
	
	/* (1<<24) DMA request source 001:UART0 */ 
	/* (1<<23) DMA triggers is hardware */
	/* (1<<22) DMA channel (DMA REQ) is turned off after finish */
	DCON0 =(1<<24)|(1<<23)|(1<<22)| size<<0;	/* set len */
		
	DMASKTRIG0 = 1<<1;	/* do not need SW_TRIG */

	//delay();
}

char src[100] = "123456789";

int c_entry(void)
{
	/* init UCON0 Transmit Mode & Receive Mode - DMA mode */	
	// Transmit Mode [3:2] 10 = DMA0 request (Only for UART0),
	// Receive Mode [1:0] 10 = DMA0 request (Only for UART0),
	UCON0 |= 1<<3 | 1<<1;
	UCON0 &= ~(1<<2 | 1<<0);

	puts("DMA:");
	dma_puts(src, 10);

	while(1);

	return 0;
}










