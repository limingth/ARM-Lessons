#include "uart.h"
#include "lcd.h"
#include "nand.h"
#include "audio.h"

void vga_init(void);

#define BMP_ADDR	(0x33000000)
#define WAV_ADDR	(0x33300000)

int puts(const char * s)
{	
	while (*s)
	{
		if (*s == '\n')
			uart_putchar('\r');
		uart_putchar(*s);
		
		s++;
	}
	
	return 0;
}

#define DISRC0		(*(volatile unsigned int *)0x4B000000)
#define DISRCC0		(*(volatile unsigned int *)0x4B000004) 
#define DIDST0		(*(volatile unsigned int *)0x4B000008)
#define DIDSTC0		(*(volatile unsigned int *)0x4B00000C)
#define DCON0		(*(volatile unsigned int *)0x4B000010)
#define DSTAT0		(*(volatile unsigned int *)0x4B000014)
#define DCSRC0		(*(volatile unsigned int *)0x4B000018)
#define DCDST0		(*(volatile unsigned int *)0x4B00001C)
#define DMASKTRIG0	(*(volatile unsigned int *)0x4B000020)
 
void dma_memcpy(int src, int dst, int size)
{
	// init dma		
	DISRC0 = (int)src;
	DISRCC0 = 0x0;
	DIDST0 = (int)dst;
	DIDSTC0 = 0x0;
	DCON0 = (size)<<0 | 1<<27;		/* set len = size */

	// start dma
	DMASKTRIG0 = 1<<0 | 1<<1;	
}

int wavdatasize;
int wavdatapos;

#define SRCPND 	(*(volatile unsigned int *)0X4A000000)
#define INTPND 	(*(volatile unsigned int *)0X4A000010)		
#define INTMSK 	(*(volatile unsigned int *)0X4A000008)	

void dma_play_wav_bigfile(int wavfile_addr);
void dma_play_wavdata(int wavdata_addr, int size);

void __irq dma_handler(void)
{
	// clean dma irq pending bit
	SRCPND = 1<<19;
	INTPND = 1<<19;

	puts("^irq DMA play finished!\n");
	
	//dma_play_wav(WAV_ADDR);
	wavdatapos += 0x200000;
	wavdatasize -= 0x200000;
	
	if (wavdatasize >= 0x200000)
		dma_play_wavdata(wavdatapos, 0x200000);
	else if (wavdatasize >= 0)
		dma_play_wavdata(wavdatapos, wavdatasize);
	else
		dma_play_wav_bigfile(WAV_ADDR);
		
}

void delay(void)
{
	int i = 0;
	
	for (i = 0; i < 300000; i++)
		;
}

void dma_clear_lcd(void)
{
	dma_memcpy(0x30000000, 0x32000000, 0xFFFFF);
}

int mymain(void)
{	
	int i;
	
	for (i = 0; i < 0x100000; i++)
	{
		*(char *)(0x30000000 + i) = 0;
	}
#if 0
//*(volatile unsigned int *)0x48000000 = 0x2211d120;
//*(volatile unsigned int *)0x48000004 = 0x00000700;
//*(volatile unsigned int *)0x48000008 = 0x00000700;
//*(volatile unsigned int *)0x4800000c = 0x00000700;
//*(volatile unsigned int *)0x48000010 = 0x00001f4c;
//*(volatile unsigned int *)0x48000014 = 0x00000700;
//*(volatile unsigned int *)0x48000018 = 0x00000700;
//*(volatile unsigned int *)0x4800001c = 0x00018005;
//*(volatile unsigned int *)0x48000020 = 0x00018005;
//*(volatile unsigned int *)0x48000024 = 0x008e0459;
//*(volatile unsigned int *)0x48000028 = 0x00000032;
*(volatile unsigned int *)0x4800002c = 0x00000030;
//*(volatile unsigned int *)0x48000030 = 0x00000030;
;
#endif

#if 0
*(volatile unsigned int *)0x4c000000 = 0x00ffffff;
*(volatile unsigned int *)0x4c000004 = 0x00044011;
*(volatile unsigned int *)0x4c000008 = 0x00038042;
*(volatile unsigned int *)0x4c00000c = 0x00fffff0;
*(volatile unsigned int *)0x4c000010 = 0x00000004;
*(volatile unsigned int *)0x4c000014 = 0x00000007;
*(volatile unsigned int *)0x4c000018 = 0x00000000;
*(volatile unsigned int *)0x4c00001c = 0x00000000;
*(volatile unsigned int *)0x4c000020 = 0x00ffffff;
*(volatile unsigned int *)0x4c000024 = 0x00044011;
;
#endif

#if 0
*(volatile unsigned int *)0x4d000000 = 0x00000000;
*(volatile unsigned int *)0x4d000004 = 0x00000000;
*(volatile unsigned int *)0x4d000008 = 0x00000000;
*(volatile unsigned int *)0x4d00000c = 0x00000000;
*(volatile unsigned int *)0x4d000010 = 0x00000000;
*(volatile unsigned int *)0x4d000014 = 0x00000000;
*(volatile unsigned int *)0x4d000018 = 0x00000000;
*(volatile unsigned int *)0x4d00001c = 0x00000000;
*(volatile unsigned int *)0x4d000020 = 0x00000000;
*(volatile unsigned int *)0x4d000024 = 0x00000000;
*(volatile unsigned int *)0x4d000028 = 0x00000000;
*(volatile unsigned int *)0x4d00002c = 0x0000a5a5;
*(volatile unsigned int *)0x4d000030 = 0x0ba5da65;
*(volatile unsigned int *)0x4d000034 = 0x000a5a5f;
*(volatile unsigned int *)0x4d000038 = 0x00000d6b;
*(volatile unsigned int *)0x4d00003c = 0x0eb7b5ed;
*(volatile unsigned int *)0x4d000040 = 0x00007dbe;
*(volatile unsigned int *)0x4d000044 = 0x0007ebdf;
*(volatile unsigned int *)0x4d000048 = 0x07fdfbfe;
*(volatile unsigned int *)0x4d00004c = 0x00000000;
*(volatile unsigned int *)0x4d000050 = 0x00000000;
*(volatile unsigned int *)0x4d000054 = 0x00000000;
*(volatile unsigned int *)0x4d000058 = 0x00000001;
*(volatile unsigned int *)0x4d00005c = 0x00000003;
*(volatile unsigned int *)0x4d000060 = 0x00000ce6;
*(volatile unsigned int *)0x4d000064 = 0x00000000;
*(volatile unsigned int *)0x4d000068 = 0x00000000;
*(volatile unsigned int *)0x4d00006c = 0x00000000;
*(volatile unsigned int *)0x4d000070 = 0x00000000;
*(volatile unsigned int *)0x4d000074 = 0x00000000;
;
#endif

#if 0
	*(volatile unsigned int *)0x56000000 = 0x007fffff;
	*(volatile unsigned int *)0x56000004 = 0x00000000;
	*(volatile unsigned int *)0x56000008 = 0x00000000;
	*(volatile unsigned int *)0x5600000c = 0x00000000;
	*(volatile unsigned int *)0x56000010 = 0x002a9655;
	*(volatile unsigned int *)0x56000014 = 0x0000028e;
	*(volatile unsigned int *)0x56000018 = 0x000007ff;
	*(volatile unsigned int *)0x5600001c = 0x00000000;
	*(volatile unsigned int *)0x56000020 = 0xaaaaaaaa;
	*(volatile unsigned int *)0x56000024 = 0x00000000;
	*(volatile unsigned int *)0x56000028 = 0x0000ffff;
	*(volatile unsigned int *)0x5600002c = 0x00000000;
	*(volatile unsigned int *)0x56000030 = 0xaaaaaaaa;
	*(volatile unsigned int *)0x56000034 = 0x00000000;
	*(volatile unsigned int *)0x56000038 = 0x0000ffff;
	*(volatile unsigned int *)0x5600003c = 0x00000000;
	*(volatile unsigned int *)0x56000040 = 0xaaaaaaaa;
	*(volatile unsigned int *)0x56000044 = 0x0000cfe5;
	*(volatile unsigned int *)0x56000048 = 0x0000ffff;
	*(volatile unsigned int *)0x5600004c = 0x00000000;
	*(volatile unsigned int *)0x56000050 = 0x000055aa;
	*(volatile unsigned int *)0x56000054 = 0x00000005;
	*(volatile unsigned int *)0x56000058 = 0x000000ff;
	*(volatile unsigned int *)0x5600005c = 0x00000000;
	*(volatile unsigned int *)0x56000060 = 0xfd95ffba;
	*(volatile unsigned int *)0x56000064 = 0x0000b020;
	*(volatile unsigned int *)0x56000068 = 0x0000ffff;
	*(volatile unsigned int *)0x5600006c = 0x00000000;
	*(volatile unsigned int *)0x56000070 = 0x00000aa0;
	*(volatile unsigned int *)0x56000074 = 0x0000013c;
	*(volatile unsigned int *)0x56000078 = 0x000007ff;
	*(volatile unsigned int *)0x5600007c = 0x00000000;
	*(volatile unsigned int *)0x56000080 = 0x00010330;
	*(volatile unsigned int *)0x56000084 = 0x00000000;
	*(volatile unsigned int *)0x56000088 = 0x00000000;
	*(volatile unsigned int *)0x5600008c = 0x00000000;
	*(volatile unsigned int *)0x56000090 = 0x00000000;
	*(volatile unsigned int *)0x56000094 = 0x00000000;
	*(volatile unsigned int *)0x56000098 = 0x00000000;
	*(volatile unsigned int *)0x5600009c = 0x00000000;
	*(volatile unsigned int *)0x560000a0 = 0x00000000;
	*(volatile unsigned int *)0x560000a4 = 0x00fffff0;
	*(volatile unsigned int *)0x560000a8 = 0x00080b00;
	*(volatile unsigned int *)0x560000ac = 0x0000000b;
	*(volatile unsigned int *)0x560000b0 = 0x32440001;
	*(volatile unsigned int *)0x560000b4 = 0x00000001;
	*(volatile unsigned int *)0x560000b8 = 0x00000000;
	*(volatile unsigned int *)0x560000bc = 0x00000000;
	*(volatile unsigned int *)0x560000c0 = 0x00b00000;
	*(volatile unsigned int *)0x560000c4 = 0x00000000;
	*(volatile unsigned int *)0x560000c8 = 0x00000000;
	*(volatile unsigned int *)0x560000cc = 0x00000000;
	*(volatile unsigned int *)0x560000d0 = 0x02aaaaaa;
	*(volatile unsigned int *)0x560000d4 = 0x000007ff;
	*(volatile unsigned int *)0x560000d8 = 0x00000000;
	*(volatile unsigned int *)0x560000dc = 0x00000000;
	*(volatile unsigned int *)0x560000e0 = 0x00000000;
	*(volatile unsigned int *)0x560000e4 = 0x00000000;
	*(volatile unsigned int *)0x560000e8 = 0x00000000;
	*(volatile unsigned int *)0x560000ec = 0x00000000;
	*(volatile unsigned int *)0x560000f0 = 0x00000000;
	*(volatile unsigned int *)0x560000f4 = 0x00000000;
	*(volatile unsigned int *)0x560000f8 = 0x00000000;
	*(volatile unsigned int *)0x560000fc = 0x00000000;

	*(volatile unsigned int *)0x4c000000 = 0x00ffffff;
	*(volatile unsigned int *)0x4c000004 = 0x00044011;
	*(volatile unsigned int *)0x4c000008 = 0x00038042;
	*(volatile unsigned int *)0x4c00000c = 0x00fffff0;
	*(volatile unsigned int *)0x4c000010 = 0x00000004;
	*(volatile unsigned int *)0x4c000014 = 0x00000007;
	*(volatile unsigned int *)0x4c000018 = 0x00000000;
	*(volatile unsigned int *)0x4c00001c = 0x00000000;
#endif	
	puts("uboot init regs finished \n");	
#if 0	
	*(volatile unsigned int *)0x56000020 = 0xaaaaaaaa;
	*(volatile unsigned int *)0x56000024 = 0x00000000;
	*(volatile unsigned int *)0x56000028 = 0x0000ffff;
	
	*(volatile unsigned int *)0x56000030 = 0xaaaaaaaa;
	*(volatile unsigned int *)0x56000034 = 0x00000000;
	*(volatile unsigned int *)0x56000038 = 0x0000ffff;
#endif
	puts("vga init\n");
	vga_init();
#if 1
	puts("lcd init\n");
	lcd_init();
	
	lcd_clear_screen(0x000000);	// black	

	puts("lcd draw line\n");
	lcd_draw_hline(100, 100, 640-100, 0xff0000);	// red
	lcd_draw_hline(200, 100, 640-100, 0x00ff00);	// green
	lcd_draw_hline(300, 100, 640-100, 0x0000ff);	// blue
	lcd_draw_hline(400, 100, 640-100, 0xffffff);	// white

	lcd_draw_vline(640/2, 50, 480-50, 0xffffff);	// white
	
	#define POS	50
	#define HALF	20
	lcd_draw_cross(POS, POS, HALF);
	lcd_draw_cross(POS, 640-POS, HALF);
	
	lcd_draw_cross(480-POS, POS, HALF);
	lcd_draw_cross(480-POS, 640-POS, HALF);
	
	puts("lcd test over\n");
#endif	
	//while (1);
	
	puts("nand init\n");
	// nand read 16M (size = 1M) to sdram  BMP_ADDR 0x33000000
	nand_init();
	
	// read bmp from 16M
	puts("nand read bmp 13M\n");
	nand_read(0x1000000, BMP_ADDR, 0xD00000);	
	puts("nand read bmp finished \n");
	
	// read wav from 32M
	nand_read(0x2000000, WAV_ADDR, 0x100000);
#if 0		
	// show BMP file
	puts("draw bmp file\n");	
	lcd_draw_bmp(BMP_ADDR);	
	lcd_clear_screen(0x000000);	// black	
	
	puts("draw bmp file using DMA \n");	
	lcd_draw_bmp_to_mem(BMP_ADDR, 0x33800000);
	dma_memcpy(0x33800000, 0x32000000, 0xFFFFF);
	puts("draw bmp file finished \n");
	
	audio_init();
	puts("play wav file\n");		
	play_wav(WAV_ADDR);		
	puts("play wav file finished \n");
	
	puts("play wav file with DMA \n");		
	dma_play_wav2(WAV_ADDR);		
	puts("play wav file with DMA finished \n");
#endif
	
	// deal with IRQ	
	*(int *)0x18 = 0xE59ff000;		// ldr pc, [pc]
	*(int *)0x20 = (int)dma_handler;
		
	// unmask 
	INTMSK &= ~(1<<19);
	
	// enable CPSR IRQ-bit
	// write to CPSR with value 0x53
	// CPSR = 0x53;
	__asm
	{	
		msr CPSR_cxsf, #0x53
	}
	
	while (1)
	{
		puts("while draw bmp file \n");
		lcd_draw_bmp(BMP_ADDR);	
		//lcd_clear_screen(0x000000);	// black	
		delay();
		
		dma_clear_lcd();		
	}
		
	//while (1)
	{	
			
	}
	
	
		
	while (1);
	
	return 0;
}