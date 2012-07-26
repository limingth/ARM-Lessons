
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
	
	// modify clock (60+8) = (127+8)/2 (2,1)  -> 400M
	*(int *)0x4C000004 = 60<<12 | 2<<4 | 1<<0;	// 200M
	
	// 200 / 2 = 100M HCLK 	100M/2 = 50M 	PCLK = 50M
//	*(int *)0x4C000014 = 0x3;		// 200:100:50
	*(int *)0x4C000014 = 0x4;		// 200:50:50
	
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