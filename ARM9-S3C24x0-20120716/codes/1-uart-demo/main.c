#include "uart.h"
#include "lib.h"

int mymain(void)
{	
	uart_init();
	
	print_addr(0x4c000004);
	print_addr(0x4c000014);
	
	print_regs(0x48000000, 13);
	
	print_regs(0x4c000000, 10);
	
	print_regs(0x4d000000, 30);
	
	print_regs(0x4e000000, 16);
	
	print_regs(0x56000000, 64);
	
	while (1)
	{
		char c;
		
		c = uart_getchar();			
		uart_putchar(c);			
	}
	
	while (1);
	
	return 0;
}