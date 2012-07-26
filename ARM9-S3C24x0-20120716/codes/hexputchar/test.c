
void test(void);

int uart_putchar(char c);

void uart_hexputchar(char c)
{
	char * hex = "0123456789abcdef";
	
	uart_putchar( hex[((c>>4) & 0xF)]  );
	uart_putchar( hex[((c>>0) & 0xF)]  );	
}

void uart_hexputint(int n)
{
	uart_hexputchar((n>>24) & 0xFF);
	uart_hexputchar((n>>16) & 0xFF);
	uart_hexputchar((n>>8) & 0xFF);
	uart_hexputchar((n>>0) & 0xFF);
}

void print_hex(int addr)
{
	uart_hexputint( *(int *)addr );
	
	uart_putchar(' ');
}

void uart_test(void)
{
	//uart_putchar('a');
	
	//uart_putchar('b');
	
	//uart_hexputchar(0x9F);
	
	//uart_hexputint(0x12345678);
	
	//uart_putchar(' ');
	
	//uart_hexputint(0x6789abcd);

	print_hex(0x4C000004);
	
	print_hex(0x4C000014);
	
	return;
}

int uart_putchar(char c)
{
	int i;
	
	*(int *)0x50000020 = c;

	for (i = 0; i < 10000; i++)
			;	
}

int abcd(void)
{
	int i = 0;
	int j = 0;
	
	// GPB8 = 0 (output)
	// GPF0 (input)	read status (1: up, 0: down)
#if 0	
	// GPB8 CON = output;
	*(int *)0x56000010 = 0x1 << 16;
	
	// GPB8 DAT = 0;
	*(int *)0x56000014 = 0 << 8;
	
	// GPF0 CON = input;
	*(int *)0x56000050 = 0 << 0;
#endif
	// GPB0 = output 	01 = Output
	*(int *)0x56000010 = 1 << 0;
	
	// modify clock
	//*(int *)0x4C000004 = 173<<12 | 2<<4 | 2<<0;	// 271M
	//*(int *)0x4C000004 = 127<<12 | 2<<4 | 1<<0;	// 271M

	test();
	
	// modify clock
	*(int *)0x4C000004 = 73<<12 | 2<<4 | 2<<0;	// 171M
	
	test();
	
	while (1);
	
	while (1)
	{
		// polling GPF0 DAT
		if ((*(volatile int *)0x56000054 & 1) == 0)
			*(int *)0x50000020 = 'a';
		else
			*(int *)0x50000020 = 'b';
	}
		
	return 0;
}

void test(void)
{
	int i = 0;
	int j = 0;
	
	for (j = 0; j < 3; j++)
	{
		*(int *)0x56000014 |= 1 << 0;			
		*(int *)0x50000020 = 'a';		
		for (i = 0; i < 1000000; i++)
			;
		
		*(int *)0x56000014 &= ~(1 << 0);		
		*(int *)0x50000020 = 'b';
		for (i = 0; i < 1000000; i++)
			;
	}
}
