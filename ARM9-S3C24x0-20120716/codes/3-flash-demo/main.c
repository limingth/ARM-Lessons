#include "uart.h"
#include "lib.h"

#define BWSCON		(*(volatile unsigned int *)(0x48000000))
#define BANKCON6	(*(volatile unsigned int *)(0x4800001C))
#define REFRESH		(*(volatile unsigned int *)(0x48000024))
#define BANKSIZE	(*(volatile unsigned int *)(0x48000028))

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
}


#define NFCONF		(*(volatile unsigned int *)(0x4E000000))
#define NFCONT		(*(volatile unsigned int *)(0x4E000004))
#define NFCMMD		(*(volatile unsigned char *)(0x4E000008))
#define NFADDR		(*(volatile unsigned char *)(0x4E00000C))
#define NFDATA		(*(volatile unsigned char *)(0x4E000010))
#define NFSTAT		(*(volatile unsigned char *)(0x4E000020))

 void nand_init(void)
{
	// TACLS [13:12]
	// TWRPH0 [10:8]
	// TWRPH1 [6:4]
	// AdvFlash (Read only) [3] 0: Support   256 or   512 byte/page NAND flash memory
	// PageSize (Read only) [2]	1: 512 Bytes/page
	// AddrCycle (Read only) [1]	1: 4 address cycle
	// BusWidth (R/W) [0]		0: 8-bit bus
	NFCONF = 3<<12 | 7<<8 | 7<<4;
	
	// Reg_nCE [1]		0: Force nFCE to low (Enable chip select)
	// MODE [0]		1: NAND flash controller enable	
	NFCONT = 1<<0 | 0<<1;
	
	return;
}

int nand_read_id(void)
{	
	int id = 0;
	
	// see k9f1208.pdf  P27
	// write CMD = 0x90
	NFCMMD = 0x90;
	
	// write ADDR = 0x00
	NFADDR = 0x00;	
		
	id = NFDATA << 24 | NFDATA << 16 | NFDATA << 8 | NFDATA << 0;
	
	return id;
}

void nand_read_page(int addr, char * buf, int n)
{
	int i = 0;
	
	// see k9f1208.pdf  P22
	// write CMD = 0x00
	NFCMMD = 0x00;
	
	// write 4 ADDR = 0x00
	NFADDR = addr & 0xFF;				// A0-A7
	NFADDR = (addr >> 9) & 0xFF;			// A9-A16
	NFADDR = (addr >> 17) & 0xFF;			// A17-A24
	NFADDR = (addr >> 25) & 0x1;			// A25
	
	// wait for Ready
	while ((NFSTAT & 0x1) == 0)
		;
	
	// read data
	for (i = 0; i < n; i++)
		*buf++ = NFDATA;
		
	return;
}

int mymain(void)
{	
	int id = 0x12345678;
	char buf[512];
	
	// beep off
	GPBCON = 0x1;	// output
	GPBDAT = 0x0;	// Tout = 0;
	
	// watchdog timer off
	WTCON = 0;
		
	//uart_init();
	
	sdram_init();	
	
	print_addr(TEST);

	nand_init();
	
	id = nand_read_id();
	print_hex(id);
	
	nand_read_page(0x100000/2, buf, 512);
	print_hex(*(int *)(buf+0x0));
	print_hex(*(int *)(buf+0x4));
	print_hex(*(int *)(buf+0x8));
	print_hex(*(int *)(buf+0xc));
		
	while (1)
	{
		char c;
		
		c = uart_getchar();			
		uart_putchar(c);			
	}
	
	while (1);
	
	return 0;
}