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

void putchar(char c)
{
	while((UTRSTAT0 & (1<<2)) == 0);
	UTXH0 = c;
}

void print_hex(int val)
{
	int i = 0;
	int tmp = 0;
	//char * str = "0123456789abcdef";
	
	for(i = 7; i >= 0; i--)
	{
		tmp = (val >> (i*4)) & 0xf;
		if(tmp < 10)	
			putchar(tmp + '0');	
		else 
			putchar(tmp - 10 + 'A');
		// putchar(str[tmp]);
	}
	
	return;
}

struct wav_info
{
	int channel;	// 1, 2
	int databit;	// 8, 16
	int fs;			// 22050, 44100
	int datasize;	// 
	int datapos;	// data pointer
};	

struct wav_info parse_wav(int wav_addr)
{
	struct wav_info	ret;

	int flag;
	
	flag = *(short *)(wav_addr + 0x10);
	if (flag == 0x10)
	{
		if (*(char *)(wav_addr + 0x24) == 'd') // "data"
		{	
			ret.datapos = wav_addr + 0x2c;
			ret.datasize = *(int *)(wav_addr + 0x28);
		}
		else // "fact"
		{
			ret.datapos = wav_addr + 0x2c + 12;
			ret.datasize = *(int *)(wav_addr + 0x28 + 12);		
		}
	}
	else if (flag == 0x12)
	{
		if (*(char *)(wav_addr + 0x26) == 'd') // "data"
		{	
			char s0, s1, s2, s3;
			ret.datapos = wav_addr + 0x2e;
			//size = *(int *)(wav_addr + 0x2a);
			s0 = *(char *)(wav_addr + 0x2a);
			s1 = *(char *)(wav_addr + 0x2b);
			s2 = *(char *)(wav_addr + 0x2c);
			s3 = *(char *)(wav_addr + 0x2d);
			ret.datasize = (s0<<0) | (s1<<8) | (s2<<16) | (s3<<24);
		}
		else // "fact"
		{	
			char s0, s1, s2, s3;
			ret.datapos = wav_addr + 0x2e + 12;
			//size = *(int *)(wav_addr + 0x2a);
			s0 = *(char *)(wav_addr + 0x2a + 12);
			s1 = *(char *)(wav_addr + 0x2b + 12);
			s2 = *(char *)(wav_addr + 0x2c + 12);
			s3 = *(char *)(wav_addr + 0x2d + 12);
			ret.datasize = (s0<<0) | (s1<<8) | (s2<<16) | (s3<<24);
		}		
	}
	
	return ret;
}
#define DISRC2		(*(volatile unsigned int *)0x4B000080)
#define DISRCC2		(*(volatile unsigned int *)0x4B000084)
#define DIDST2		(*(volatile unsigned int *)0x4B000088)
#define DIDSTC2		(*(volatile unsigned int *)0x4B00008C)
#define DCON2		(*(volatile unsigned int *)0x4B000090)
#define DMASKTRIG2		(*(volatile unsigned int *)0x4B0000A0)
 
#define DSTAT2		(*(volatile unsigned int *)0x4B000094) 
   
  
void dma_play_wav(int wav_addr)
{
	struct wav_info wi;
		
	wi = parse_wav(wav_addr);
	// IIS DMA mode init	1 = DMA
	IISFCON |= 1<<15;
	IISCON |= 1<<5;	
	// DMA init
	DISRC2 = wi.datapos;
	DISRCC2 = 0;	// AHB, inc
	DIDST2 = 0x55000010;	
	DIDSTC2 = 1<<0 | 1<<1;	// APB, fixed	
	// whole service, HW request, 2byte, datasize
	DCON2 = 1<<23 | 1<<20 | (wi.datasize / 2);
	DCON2 |= 1<<22; 	// no auto-reload	
	
	// Enable DMA interrupt
	DCON2 |= 1<<29;
	
	// DMA start	// channel ON
	DMASKTRIG2 = 1<<1;
}

void play_wav(int wav_addr)
{
	int counter = 0;
	char vol = 0; 
	struct wav_info wi;
	short * p;
	int size;

	wi = parse_wav(wav_addr);
	
	p = (short *)wi.datapos;
	size = wi.datasize;
	
	print_hex(size);	
	
	while (size > 0)
	{		
		while ((IISCON & (1<<7)) == (1<<7))
			;
	
		IISFIFO = *p;
		p++;
		
		size -= 2;
		
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
	//	IISPSR = 2<<5 | 2<<0;	// 44Khz
	
	// Serial data bit per channel
	//	[3] 	0 = 8-bit         1 = 16-bit
	IISMOD |= 1<<3;
	
	// Serial bit clock frequency select
	//	[1:0] 	00 = 16fs        01 = 32fs
	//		10 = 48fs        11 = N/A
	IISMOD |= 1<<0;	
}

int mymain(void)
{		
	L3_init();
	key_init();
	
	L3_write(0x16, 0x50);	// reset
	delay_ns(1000);
	L3_write(0x14, 0x00);	// max volumn
	delay_ns(1000);
	
	IIS_init();
	
	//play_wav(0x32000000);
	dma_play_wav(0x32000000);
	
#define SRCPND 	(*(volatile unsigned int *)0X4A000000)	
#define INTPND 	(*(volatile unsigned int *)0X4A000010)	
//	while (DSTAT2 & 0xfffff)
//		;
	
	while ((SRCPND & (1<<19)) == 0)	
		;
		
	GPBDAT |= 0x1;
	delay_ns(1000000);
	GPBDAT &= ~0x1;
	
	return 0;
}