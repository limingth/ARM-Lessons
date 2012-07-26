#define UTRSTAT0	(*(volatile unsigned int *)(0x50000010))
#define UTXH0	(*(volatile unsigned int *)(0x50000020))

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
		
	UTXH0= ch;
	
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

void dma_memcpy(char * src, char * dst, int size)
{
	// init dma		
	DISRC0 = (int)src;
	DISRCC0 = 0x0;
	DIDST0 = (int)dst;
	DIDSTC0 = 0x0;
	DCON0 = (size)<<0 | 1<<27;		/* set len = size */

	// start dma
	DMASKTRIG0 = 1<<0 | 1<<1;
	delay();
}

char src[100] = "akaedu";
char dst[100] = "xxxxxx";

int c_entry(void)
{
	puts("before DMA");
	puts(src);
	puts(dst);
	puts("------");	

	dma_memcpy(src, dst, 100);

	puts("after DMA");
	puts(src);
	puts(dst);
	puts("------");
	
	while(1);

	return 0;
}










