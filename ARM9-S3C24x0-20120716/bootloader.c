
void nand_read(int sdram_addr, int nand_addr, int size)
{
	int pages = (size - 1)/PAGE_SIZE + 1;
	int i;

	for (i = 0; i < pages; i++)
		nand_read_page(nand_addr + i*PAGE_SIZE, (char *)(sdram_addr + i*PAGE_SIZE));

	return;
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
		*p++ = getchar();

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