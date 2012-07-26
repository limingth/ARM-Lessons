#include "uart.h"
#include "lib.h"

#define BWSCON		(*(volatile unsigned int *)(0x48000000))
#define BANKCON6	(*(volatile unsigned int *)(0x4800001C))
#define REFRESH		(*(volatile unsigned int *)(0x48000024))
#define BANKSIZE	(*(volatile unsigned int *)(0x48000028))
#define MRSRB6		(*(volatile unsigned int *)(0x4800002C))
 

#define GPBCON		(*(volatile unsigned int *)(0x56000010))
#define GPBDAT		(*(volatile unsigned int *)(0x56000014))

#define WTCON		(*(volatile unsigned int *)(0x53000000))
 
#define TEST	0x32800000

void sdram_init(void)
{
	// DW6 [25:24]	10 = 32-bit       
	BWSCON = 0x2 << 24;	
	
	// MT [16:15]	11 = Sync. DRAM
	// SCAN [1:0]	01 = 9-bit	Column address number 
	// Trcd [3:2] RAS to CAS delay	min=20ns 100Mhz=10ns min=2clocks
	BANKCON6 = 3<<15 | 1<<0 | 0<<2;
	
	// Trp [21:20] SDRAM RAS pre-charge Time 	20ns	 00 = 2 clocks  
	// Tsrc [19:18] SDRAM Semi Row cycle time	65ns/2	 00 = 4 clocks
	// 0x4F5 = 1269 = 2^11 + 1 - 7.8*100
	REFRESH = 0x4F5 | 0<<18 | 0<<20;
	
	// BK76MAP [2:0]	001 = 64MB/64MB
	BANKSIZE = 1<<0;
	
	// CAS latency
	//	000 = 1 clock,     010 = 2 clocks,    011=3 clocks
	MRSRB6 = 3<<4;
}

int mymain(void)
{	
	// beep off
	GPBCON = 0x1;	// output
	GPBDAT = 0x0;	// Tout = 0;
	
	// watchdog timer off
	WTCON = 0;
		
	uart_init();
	uart_putchar('a');
	
	sdram_init();
	
	*(int *)TEST = 0x12345678;
	
	print_addr(TEST);

	while (1)
	{
		char c;
		
		c = uart_getchar();			
		uart_putchar(c);			
	}
	
	while (1);
	
	return 0;
}