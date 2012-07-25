#define S3C2440
//#define S3C2410

#ifdef S3C2440

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

#endif

#ifdef S3C2410

#define NFCONF	(*(volatile unsigned int *)0x4e000000)
//#define NFCONT	(*(volatile unsigned int *)0x4e000004)
#define NFCMMD	(*(volatile unsigned char *)0x4e000004)
#define NFADDR	(*(volatile unsigned char *)0x4e000008)
#define NFDATA	(*(volatile unsigned char *)0x4e00000c)
#define NFSTAT	(*(volatile unsigned char *)0x4e000010)

void nand_init(void)
{
	//NFCONF = 0x1400;
	//NFCONT = 0x01;
	
	// TWRPH0 [6:4] TWRPH0 duration setting value (0~7)
	// Duration = HCLK * (TWRPH0 + 1)
	NFCONF |= 1<<6;
	
	// Enable/Disable [15] NAND Flash controller enable/disable
	// 0 = Disable NAND Flash Controller
	// 1 = Enable NAND Flash Controller
	NFCONF |= 1<<15;
	
	// TACLS [10:8] CLE & ALE duration setting value (0~7)
	// Duration = HCLK * (TACLS + 1)
	//NFCONF |= 1<<8;
	
	// NAND Flash Memory	chip enable
  	//	[11]   NAND flash memory nFCE control
  	//	0 : NAND flash nFCE = L (active)
	//	1 : NAND flash nFCE = H (inactive)
  	//	(After auto-boot, nFCE will be inactive.)	
  	NFCONF &= ~(1<<11);
}
#endif


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

void nand_read_page(int addr, char * buf)
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
	for (i = 0; i < 512; i++)
		*buf++ = NFDATA;
		
	return;
}

void nand_read(int nand_addr, int sdram_addr, int size)
{	
	int pages;
	int i;
	char * buf = (char *)sdram_addr;

	if (size <= 0)
		return;

	// get how many pages to be read
	pages = (size - 1) / 512 + 1;

	for (i = 0; i < pages; i++)
		nand_read_page(nand_addr+i*512, buf+i*512);
}

/*
 * in this stdio.c, '\n' will be treated as '\r'+'\n'
 * if you putchar('\n') then you will put 2 char: Return and Newline ("\r\n")
 * in return for this, if you getchar() return '\n' that means Enter key pressed (which is actually '\r')
 */

int putchar(int c)
{
	if (c == '\n')
		uart_putchar('\r');

	if (c == '\b')
	{
		uart_putchar('\b');
		uart_putchar(' ');	
	}

	uart_putchar((char)c);

	return c;	
}

int getchar(void)
{
	int c;

	c = (int)uart_getchar();

	if (c == '\r')
		return '\n';

	return c;
}

int puts(const char * s)
{
	while (*s)
		putchar(*s++);

	return 0;
}

char * gets(char * s)
{
	char * p = s;

	// note: here we use getchar(), not uart_getchar() for portability
	while ((*p = getchar()) != '\n')
	{
		if (*p != '\b')
			putchar(*p++);
		else	
			if (p > s)
				putchar(*p--);	
	}

	*p = '\0';
	putchar('\n');

	return s;
}

int strcmp(const char * s1, const char * s2)
{
	while (*s1 == *s2)
	{
		if (*s1 == '\0')
			return 0;
		s1++;
		s2++;
	}
	
	return 	*s1 - *s2;
}

int shell_parse(char * buf, char * argv[])
{
	int argc = 0;
	int state = 0;

	while (*buf)
	{
		if (*buf != ' ' && state == 0)
		{
			argv[argc++] = buf;
			state = 1;
		}

		if (*buf == ' ' && state == 1)
		{
			*buf = '\0';
			state = 0;
		}

		buf++;	
	}

	return argc;
}

int atoi(char * buf)
{
	int value = 0;	
	int base = 10;
	int i = 0;

	if (buf[0] == '0' && buf[1] == 'x')
	{
		base = 16;
		i = 2;
	}

	// 123 = (1 * 10 + 2) * 10 + 3
	// 0x1F = 1 * 16 + F(15)	
	while (buf[i])
	{
		int tmp;

		if (buf[i] <= '9' && buf[i] >= '0') 
			tmp = buf[i] - '0';
		else
			tmp = buf[i] - 'a' + 10;

		value = value * base + tmp;

		i++;
	}

	return value;
}


#define DOWN_BIN_ADDR	(0x31000000)	// bin file load addr

int loadb(int argc, char * argv[])
{
	int i = 0;
	int size = 0;
	char * p = (char *)DOWN_BIN_ADDR;

	printf("load bin file to address 0x%x\n", DOWN_BIN_ADDR);

	if (argc >= 2)
		size = atoi(argv[1]);

	for (i = 0; i < size; i++)
		*p++ = uart_getchar();

	printf("load finished! \n");

	return 0;
}

void loadx(int argc, char *argv[])
{	
	char * p = (char *)DOWN_BIN_ADDR;

	if (argc >= 2)
		p = (char *)atoi(argv[1]);

	printf("load bin file by xmodem to address 0x%x\n", (int)p);		

	xmodem_recv(p);

	printf("loadx finished! \n");
}

int go(int argc, char * argv[])
{
	void (*fp)(void);

	printf("go to address 0x%x\n", DOWN_BIN_ADDR);

	fp = (void (*)(void))DOWN_BIN_ADDR;

	fp();

	return 0;
}

#include "uart.h"

#define NAK 0x15
#define EOT 0x04
#define ACK 0x06

// delay time depend on external clock setting, see lib.c
extern void delay(void);

void xmodem_recv(char *addr)
{
	char c;
	int i;
	for (c = '9'; c > '0'; c--)
	{
		uart_putchar(c);
		delay();
	}

	uart_putchar(NAK);
	while (uart_getchar() != EOT)
	{
		uart_getchar(); 
		uart_getchar();
		for (i = 0; i < 128; i++)
			*addr++ = uart_getchar();
		uart_getchar();
		uart_putchar(ACK);
	}
	uart_putchar(ACK);
}
