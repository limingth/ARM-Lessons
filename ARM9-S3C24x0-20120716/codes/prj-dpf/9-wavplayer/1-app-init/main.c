#define UTRSTAT0 (*(volatile unsigned int *)0x50000010)    
#define UTXH0	(*(volatile unsigned int *)0x50000020)    

void uart_putchar(char c)
{
	// polling status
	while ((UTRSTAT0 & (1<<2)) == 0) 
		;
	
	UTXH0 = c;	
}

#define IISFIFO	(*(volatile unsigned short *)0x55000010)
#define IISCON	(*(volatile unsigned int *)0x55000000)
#define IISMOD	(*(volatile unsigned int *)0x55000004)
#define IISFCON	(*(volatile unsigned int *)0x5500000C)
#define IISPSR	(*(volatile unsigned int *)0x55000008)

#define GPECON	(*(volatile unsigned int *)0x56000040)

#define GPBCON 	(*(volatile unsigned int *)0x56000010)
#define GPBDAT 	(*(volatile unsigned int *)0x56000014)

#define GPFCON *(volatile int *)0x56000050
#define GPFDAT *(volatile int *)0x56000054

/* L3 interface */
// GPB2 - L3MODE
// GPB3 - L3DATA
// GPB4 - L3CLOCK
#define	L3MODE_1	GPBDAT |= 1<<2
#define	L3MODE_0	GPBDAT &= ~(1<<2)

#define	L3DATA_1	GPBDAT |= 1<<3
#define	L3DATA_0	GPBDAT &= ~(1<<3)

#define	L3CLOCK_1	GPBDAT |= 1<<4
#define	L3CLOCK_0	GPBDAT &= ~(1<<4)

void delay_ns(int t)
{
	int i;
	
	for (i = 0; i < t; i++)
		;	
}

void L3_init()
{
	// GPB2,3,4 = output
	GPBCON |= 1<<4 | 1<<6 | 1<<8;
	GPBCON &= ~(1<<5 | 1<<7 | 1<<9);
}

void IIS_init(void)
{
	// GPE0,1,2,3,4
	// [9:8]	10 = I2SDO 
	// [7:6]	10 = I2SDI   
	// [5:4]	10 = CDCLK   
	// [3:2]	10 = I2SSCLK 
	// [1:0]	10 = I2SLRCK  
	GPECON |= 1<<1 | 1<<3 | 1<<5 | 1<<7 | 1<<9;
	
	// IIS init
	// IIS interface [0] 	0 = Disable (stop)
	//			1 = Enable  (start)
	IISCON |= 1<<0;
	
	// Transmit/receive mode select
	// [7:6] 00 = No transfer      01 = Receive mode
	//	 10 = Transmit mode    11 = Transmit and receive mode	
	IISMOD |= 1<<7;
	
	// Transmit FIFO [13] 0 = Disable     1 = Enable
	IISFCON |= 1<<13;

	// Master clock frequency select
	//	[2] 0 = 256fs        1 = 384fs
	//	(fs: sampling frequency)
	IISMOD |= 1<<2;	
	
	// IIS prescaler [1] 0 = Disable	1 = Enable
	IISCON |= 1<<1;	
	
	// PCLK / (N+1) = master clock
	// master clock = 384 * 22Khz
	// PCLK = 50M = 50000 Khz
	// N+1 = 50000Khz/ (384*22Khz) = 5.91 = 6
	IISPSR = 5<<5 | 5<<0;		// 22Khz
	// IISPSR = 2<<5 | 2<<0;	// 44Khz
	
	// Serial data bit per channel
	//	[3] 	0 = 8-bit         1 = 16-bit
	IISMOD |= 1<<3;
	
	// Serial bit clock frequency select
	//	[1:0] 	00 = 16fs        01 = 32fs
	//		10 = 48fs        11 = N/A
	IISMOD |= 1<<0;	
}

#define L3_MODE(v)		v? (GPBDAT |= (1<<2)): (GPBDAT &= ~(1<<2))
#define L3_DATA(v)		v? (GPBDAT |= (1<<3)): (GPBDAT &= ~(1<<3))
#define L3_CLOCK(v)		v? (GPBDAT |= (1<<4)): (GPBDAT &= ~(1<<4))

#if 0
#define L3(v)	do { \
					if (v) \
						GPBDAT |= (1<<2); \
					else \
						GPBDAT &= ~(1<<2); \
				} while (0)
	L3(1);
#endif

void L3_data(char data)
{	
	int i;
	
	L3_CLOCK(1);
	
	L3_MODE(0);
	delay_ns(190);		// th(L3)A	
	L3_MODE(1);	
	delay_ns(190);		// tsu(L3)A
	
	for (i = 0; i < 8; i++)
	{	
		int v = (data >> i) & 0x01;
		L3_CLOCK(0);	
		L3_DATA(v);
		delay_ns(250);
		L3_CLOCK(1);
		delay_ns(250);
	}
		
	L3_MODE(0);
}

void L3_address(char data)
{	
	int i;
	
	L3_CLOCK(1);
	
	L3_MODE(1);
	delay_ns(190);		// th(L3)A	
	L3_MODE(0);	
	delay_ns(190);		// tsu(L3)A	
	for (i = 0; i < 8; i++)
	{	
		int v = (data >> i) & 0x01;
		L3_CLOCK(0);	
		L3_DATA(v);
		delay_ns(250);
		L3_CLOCK(1);
		delay_ns(250);
	}		
	L3_MODE(1);
}

void L3_write(int addr, int data)
{
	L3_address(addr);
	L3_data(data);
}

void key_init()
{
	GPBCON |= 1<<16;
	GPBCON &= ~(1<<17);
	GPBCON |= 1<<18;
	GPBCON &= ~(1<<19);
}

int keyscan()
{
	GPBDAT &= ~(1<<8);
	GPBDAT |= (1<<9);
	if((GPFDAT & 1<<0) == 0)
		return 1;
	else if((GPFDAT & 1<<2) == 0)
		return 3;
	GPBDAT &= ~(1<<9);
	GPBDAT |= (1<<8);
	if((GPFDAT & 1<<0) == 0)
		return 2;
	else if((GPFDAT & 1<<2) == 0)
		return 4;		
	
	return 0;
}

int mymain(void)
{	
	int counter = 0;
	char vol = 0; 
	int i = 0;
	
	L3_init();
	key_init();
	
	L3_write(0x16, 0x50);	// reset
	delay_ns(1000);
	L3_write(0x14, 0x00);	// max volumn
	delay_ns(1000);
	
	// GPE0,1,2,3,4
	// [9:8]	10 = I2SDO 
	// [7:6]	10 = I2SDI   
	// [5:4]	10 = CDCLK   
	// [3:2]	10 = I2SSCLK 
	// [1:0]	10 = I2SLRCK  
	GPECON |= 1<<1 | 1<<3 | 1<<5 | 1<<7 | 1<<9;
	
	// IIS init
	// IIS interface [0] 	0 = Disable (stop)
	//			1 = Enable  (start)
	IISCON |= 1<<0;
	
	// Transmit/receive mode select
	// [7:6] 00 = No transfer      01 = Receive mode
	//	 10 = Transmit mode    11 = Transmit and receive mode	
	IISMOD |= 1<<7;
	
	// Transmit FIFO [13] 0 = Disable     1 = Enable
	IISFCON |= 1<<13;

	// Master clock frequency select
	//	[2] 0 = 256fs        1 = 384fs
	//	(fs: sampling frequency)		
	IISMOD |= 1<<2;	
	
	// IIS prescaler [1] 0 = Disable	1 = Enable
	IISCON |= 1<<1;	
	
	// PCLK / (N+1) = master clock
	// master clock = 384 * 22Khz
	// PCLK = 50M = 50000 Khz
	// N+1 = 50000Khz/ (384*22Khz) = 5.91 = 6
	IISPSR = 5<<5 | 5<<0;		// 22Khz
	//	IISPSR = 2<<5 | 2<<0;	// 44Khz
	
	// Serial data bit per channel
	//	[3] 	0 = 8-bit         1 = 16-bit
	IISMOD |= 1<<3;
	
	// Serial bit clock frequency select
	//	[1:0] 	00 = 16fs        01 = 32fs
	//		10 = 48fs        11 = N/A
	IISMOD |= 1<<0;	

	while (1)
	{			
		for (i = 0; i < 10000; i++)
			IISFIFO = i;
		
		counter++;				
		if (counter % 0x10 == 0)
		{
			if (keyscan() == 1)
				L3_write(0x14, 0xA4);	// Mute
			else if (keyscan() == 2)
				L3_write(0x14, 0xA0);	// no Mute
			else if (keyscan() == 3)	
				L3_write(0x14, vol++);
			else if (keyscan() == 4)
				L3_write(0x14, vol--);				
		}
		*(char *)0x20800000 = keyscan();
	}
	
	return 0;
}