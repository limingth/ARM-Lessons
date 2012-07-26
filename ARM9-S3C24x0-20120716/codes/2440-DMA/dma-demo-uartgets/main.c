#define UTRSTAT0	(*(volatile unsigned int *)0x50000010)
#define UTXH0		(*(volatile unsigned int *)0x50000020)
#define UCON0		(*(volatile unsigned int *)0x50000004)  
#define UFCON0		(*(volatile unsigned int *)0x50000008)

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

void dma_gets(char * buf, int size)
{
	// init dma		
	DISRC0 = (int)0x50000024;
	DISRCC0 = 1<<1 | 1<<0;	// set APB and fixed address
	DIDST0 = (int)buf;	
	DIDSTC0 = 0x0;
	
	/* (1<<24) DMA request source 001:UART0 */ 
	/* (1<<23) DMA triggers is hardware */
	/* (1<<22) DMA channel (DMA REQ) is turned off after finish */
	DCON0 =(1<<24)|(1<<23)|(1<<22)| size<<0;	/* set len */
		
	DMASKTRIG0 = 1<<1;	/* do not need SW_TRIG */
}

#define URXH0 (*((volatile unsigned int *)0x50000024))
void dma_gets2(char *buff, int len)
{
  /* Receive Mode. UCON0[1:0]
     Determine which function is currently able to read data from UART
     receive buffer register. (UART Rx Enable/Disable)
     00 = Disable
     01 = Interrupt request or polling mode
     10 = DMA0 request (Only for UART0),DMA3 request (Only for UART2)
     11 = DMA1 request (Only for UART1)
  */
  UCON0 |= 1 << 1;
  UCON0 &= ~(1 << 0);


#if 1
  DISRC0 = (int)&URXH0;
  DISRCC0 |= (1 << 0);
  DISRCC0 |= (1 << 1);

  DIDST0 = (int)buff;
  DIDSTC0 &= ~(1 << 0);
  DIDSTC0 &= ~(1 << 1);

  /* SWHW_SEL     [23]
     Select the DMA source between software (S/W request mode) and
     hardware (H/W request mode).
     0: S/W request mode is selected and DMA is triggered by setting
     SW_TRIG bit of DMASKTRIG control register.
     1: DMA source selected by bit[26:24] triggers the DMA operation.
  */
  DCON0 |= 1 << 23;

  /* HWSRCSEL
     [26:24]
     Select DMA request source for each DMA.  DCON0: 
     000:nXDREQ0 
     001:UART0 
     100:USB device EP1 
  */
  DCON0 &= ~(1 << 26 | 1 << 25);
  DCON0 |= 1 << 24;

  /* INT [29] Enable/Disable the interrupt setting for CURR_TC (terminal count) 
     0: CURR_TC interrupt is disabled. The user has to view the transfer 
     count in the status register (i.e. polling). 
     1: Interrupt request is generated when all the transfer is done 
     (i.e. CURR_TC becomes 0). 
  */
  DCON0 |= 1 << 29;

  /* RELOAD[22]
     Set the reload on/off option.
  */
  DCON0 |= (1 << 22);

  /* DSZ     [21:20]
     Data size to be transferred.
     00 = Byte
     01 = Half word
     10 = Word
     11 = reserved
  */
  DCON0 &= ~(1 << 21 | 1 << 20);

  /* TSZ [28] Select the transfer size of an atomic transfer (i.e. transfer 
     performed each time DMA owns the bus before releasing the bus). 
     0: A unit transfer is performed. 
     1: A burst transfer of length four is performed. 
  */
  DCON0 &= ~(1 << 28);

  /* SERVMODE [27]
     Select the service mode between Single service mode and Whole service mode.
     0: Single service mode is selected in which after each atomic
     transfer (single or burst of length four) DMA stops and waits for
     another DMA request.
     1: Whole service mode is selected in which one request gets
     atomic transfers to be repeated until the transfer count reaches to
     0. In this mode, additional request are not required.
     Note that even in the Whole service mode, DMA releases the bus
     after each atomic transfer and then tries to re-get the bus to prevent
     starving of other bus masters.
  */
  //DCON0 |= 1 << 27;

  /* TC [19:0]
     Initial transfer count (or transfer beat).
     Note that the actual number of bytes that are transferred is
     computed by the following equation: DSZ x TSZ x TC. Where, DSZ,
     TSZ (1 or 4), and TC represent data size DCONn[21:20], transfer
     size DCONn[28], and initial transfer count, respectively. This value
     will be loaded into CURR_TC only if the CURR_TC is 0 and the
     DMA ACK is 1.
  */
  DCON0  &= ~(0xfffff);
  DCON0  |= len;

  /* ON_OFF     [1]
     DMA channel on/off bit.
     0: DMA channel is turned off. (DMA request to this channel is ignored.)
     1: DMA channel is turned on and the DMA request is handled. 
  */
  DMASKTRIG0 |= 1 << 1;

  /* SW_TRIG     [0]
     Trigger the DMA channel in S/W request mode.
     1: it requests a DMA operation to this controller.
  */
  //DMASKTRIG0 |= 1 << 0;
#endif
}

char src[100] = "hello, akaedu";

int c_entry(void)
{
	/* init UCON0 Transmit Mode & Receive Mode - DMA mode */	
	// Transmit Mode [3:2] 01 = Polling
	// Receive Mode [1:0] 10 = DMA0 request (Only for UART0),
	// Receive DMA
	UCON0 |= 1<<1;
	UCON0 &= ~(1<<0);
	
	// Txd Polling	[3:2] 01 = Polling
	// if Txd is DMA, then error!!!
	UCON0 |= 1<<2;
	UCON0 &= ~(1<<3);

	puts(src);
	dma_gets(src, 10);

	while(1)
	{	
		delay();
		puts(src);		
	}

	return 0;
}










