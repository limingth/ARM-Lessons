#include "uart.h"

void print_hex(int value)
{
	char * hex = "0123456789abcdef";		// good!
	//char hex[] = "0123456789abcdef";		// not good!
	
	int i;
	
	for (i = 7; i >= 0; i--)
	{
		int index;
		
		index = (value >> (i*4)) & 0xF;
		
		uart_putchar(hex[index]);	
	}
	
	uart_putchar(' ');
}

void print_addr(int addr)
{
	unsigned int value;
	
	value = *(volatile unsigned int *)addr;	
	print_hex(addr);
	uart_putchar(':');
	print_hex(value);
	
	uart_putchar('\n');
	uart_putchar('\r');

	return;
}

void print_regs(int addr, int n)
{
	int i;
	
	for (i = 0; i < n; i++)
		print_addr(addr + i*4);
	
	return;
}