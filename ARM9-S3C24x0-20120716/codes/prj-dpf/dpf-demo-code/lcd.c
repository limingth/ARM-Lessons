
#define COL	640
#define ROW	480
#define FB_ADDR		(0x32000000)

#define GPGCON    (*(volatile unsigned int *)0x56000060) //Port G control
#define GPGDAT    (*(volatile unsigned int *)0x56000064) //Port G data
#define GPGUP     (*(volatile unsigned int *)0x56000068) //Pull-up control G

// LCD CONTROLLER
#define LCDCON1    (*(volatile unsigned int *)0x4d000000) //LCD control 1
#define LCDCON2    (*(volatile unsigned int *)0x4d000004) //LCD control 2
#define LCDCON3    (*(volatile unsigned int *)0x4d000008) //LCD control 3
#define LCDCON4    (*(volatile unsigned int *)0x4d00000c) //LCD control 4
#define LCDCON5    (*(volatile unsigned int *)0x4d000010) //LCD control 5
#define LCDSADDR1  (*(volatile unsigned int *)0x4d000014) //STN/TFT Frame buffer start address 1
#define LCDSADDR2  (*(volatile unsigned int *)0x4d000018) //STN/TFT Frame buffer start address 2
#define LCDSADDR3  (*(volatile unsigned int *)0x4d00001c) //STN/TFT Virtual screen address set
#define REDLUT     (*(volatile unsigned int *)0x4d000020) //STN Red lookup table
#define GREENLUT   (*(volatile unsigned int *)0x4d000024) //STN Green lookup table 
#define BLUELUT    (*(volatile unsigned int *)0x4d000028) //STN Blue lookup table
#define DITHMODE   (*(volatile unsigned int *)0x4d00004c) //STN Dithering mode
#define TPAL       (*(volatile unsigned int *)0x4d000050) //TFT Temporary palette
#define LCDINTPND  (*(volatile unsigned int *)0x4d000054) //LCD Interrupt pending
#define LCDSRCPND  (*(volatile unsigned int *)0x4d000058) //LCD Interrupt source
#define LCDINTMSK  (*(volatile unsigned int *)0x4d00005c) //LCD Interrupt mask
#define LPCSEL     (*(volatile unsigned int *)0x4d000060) //LPC3600 Control
#define PALETTE     0x4d000400                         	//Palette start address

#define RED(c)		((c>>16) & 0xFF)
#define GREEN(c)	((c>>8) & 0xFF)
#define BLUE(c)		((c>>0) & 0xFF)
#define BIT24_TO_BIT16(color)	(RED(color) << 11) | (GREEN(color) << 5) | (BLUE(color) << 0)

#define RED5(c)		((c >> 3) & 0x1F)
#define GREEN6(c)	((c >> 2) & 0x3F)
#define BLUE5(c)	((c >> 3) & 0x1F)

#define BIT24_TO_BIT16_2(r, g, b)	(RED5(r) << 11) | (GREEN6(g) << 5) | (BLUE5(b) << 0)

void lcd_init(void)
{	
	// LCD IIC GPIO init
	*(int *)0x56000020 = 0xaaaaaaaa;		// GPCCON
	//*(int *)0x56000028 = 0xffffffff;		// GPCUP
	
	*(int *)0x56000030 = 0xaaaaaaaa;		// GPDCON	
	//*(int *)0x56000038 = 0xffffffff;		// GPDUP
	
	//*(int *)0x56000040 = 0xaaaaaaaa;

	// lcd SFR init
	//*(int *)0x4d000000 = 0x4800179;
	//LCDCON1 = 1<<0 | 0xC<<1 | 3<<5 | 1<<8;
	LCDCON1 = 0xC<<1 | 3<<5 | 1<<8;		// 16bpp
	//LCDCON1 = 0xD<<1 | 3<<5 | 1<<8;		// 24bpp

	//*(int *)0x4d000004 = 0x1977c141;
#define VBPD		31
#define LINEVAL		(480-1)
#define VFPD		10//+5
#define VSPW		1
	LCDCON2 = VSPW<<0 | VFPD<<6 | LINEVAL<<14 | VBPD<<24;

	//*(int *)0x4d000008 = 0x21a7f28;
#define HBPD		46//+67		// 0x43
#define HOZVAL		(640-1)		// 0x27f
#define HFPD		23//+40		// 0x28
	LCDCON3 = HFPD<<0 | HOZVAL<<8 | HBPD<<19;
	
	//*(int *)0x4d00000c = 0xd1f;
#define HSPW	93 //+31		
	LCDCON4 = HSPW<<0;
	
	//*(int *)0x4d000010 = 0x14b09;
	LCDCON5 = 1<<11 | 1<<10 | 1<<9 | 1<<8 | 1<<3 | 1<<2 | 1<<0;
	
#define LCD_ADDR (FB_ADDR)

#define M5D(n)  ((n)&0x1fffff)

//rLCDSADDR1 = ((LCD_ADDR >> 22) << 21) | ((M5D(LCD_ADDR >> 1)) <<  0);

//rLCDSADDR2 = M5D((LCD_ADDR + LCD_WIDTH * LCD_HEIGHT * 2) >> 1);

//rLCDSADDR3 = LCD_WIDTH;
	
	// LCDSADDR1 0X4D000014 R/W STN/TFT: Frame buffer start address 1 register
	//*(int *)0x4d000014 = ((LCD_ADDR >> 22) << 21) | ((M5D(LCD_ADDR >> 1)) <<  0);			// rLCDSADDR1
	//LCDSADDR1 = ((LCD_ADDR >> 22) << 21) | ((M5D(LCD_ADDR >> 1)) <<  0);
	LCDSADDR1 = LCD_ADDR>>1;
	
	//*(int *)0x4d000018 = M5D((LCD_ADDR + 640 * 480 * 2) >> 1);			// rLCDSADDR2
	LCDSADDR2 = (LCD_ADDR + (ROW*COL*2))>>1;		// 16bpp
	//LCDSADDR2 = (LCD_ADDR + (ROW*COL*4))>>1;		// 24bpp
	
	//*(int *)0x4d00001c = 0x280;			// rLCDSADDR3	width
	LCDSADDR3 = COL;
	
	// LCD_PWREN GPG4	
	 //GPG4 is setted as LCD_PWREN
	//GPGUP = GPGUP&(~(1<<4))|(1<<4); // Pull-up disable
	//GPGCON = GPGCON&(~(3<<8))|(3<<8); //GPG4=LCD_PWREN
	
	//Enable LCD POWER ENABLE Function
	//LCDCON5 = LCDCON5&(~(1<<3))|(1<<3);   // PWREN
	//LCDCON5 = LCDCON5&(~(1<<5))|(0<<5);   // INVPWREN
	
	// Enable LCD 
	LCDCON1 |= 1<<0 ;
	
	return;
}

#if 0
void lcd_draw_pixel(int row, int col, int color)
{
	int * pixel = (int *)FB_ADDR;
	
	*(pixel + (row * COL + col)) = color;	
}
#endif

#if 1
void lcd_draw_pixel(int row, int col, int color)
{
	short * pixel = (short *)FB_ADDR;
	short color_16;	//= BIT24_TO_BIT16(color);
	char r, g, b;
	
	r = RED(color);
	g = GREEN(color);
	b = BLUE(color);
	
	color_16 = BIT24_TO_BIT16_2(r, g, b);	
	
	*(pixel + (row * COL + col)) = color_16;	
}
#endif 

void lcd_clear_screen(int color)
{
	int i, j;
		
	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL; j++)
			lcd_draw_pixel(i, j, color);
}

void lcd_draw_hline(int row, int col1, int col2, int color)
{
	int j;
	
	for (j = col1; j <= col2; j++)
		lcd_draw_pixel(row, j, color);
}

void lcd_draw_vline(int col, int row1, int row2, int color)
{
	int i;
	
	for (i = row1; i <= row2; i++)
		lcd_draw_pixel(i, col, color);
}

void lcd_draw_cross(int row, int col, int halflen)
{
	int color = 0xff0000;
	
	lcd_draw_hline(row, col-halflen, col+halflen, color);
	
	lcd_draw_vline(col, row-halflen, row+halflen, color);
	
}

int get_bmpfile_color(int i, int j, char * p)
{
	char red, blue, green;
	int color;

	blue = *(p + (i * COL + j)*3);
	green = *(p + (i * COL + j)*3 + 1);
	red = *(p + (i * COL + j)*3 + 2);
	
	color = red << 16 | green << 8 | blue << 0;
	
	return color;
}

int get_bmpfile_color_k(int i, int j, char * p, double k)
{
	char red, blue, green;
	int color;

	blue = *(p + (i * COL + j)*3);
	green = *(p + (i * COL + j)*3 + 1);
	red = *(p + (i * COL + j)*3 + 2);
	
	blue *= k;
	green *= k;
	red *= k;
	
	color = red << 16 | green << 8 | blue << 0;
	
	return color;
}

void lcd_draw_bmp_eff1(int bmp_addr)
{
	char * p = (char *)bmp_addr;	
	int color;
	int i, j;

	// bmp file header is 54 bytes	
	p += 54;
	
	for (i = 0; i <= ROW/2; i++)
		for (j = 0; j < COL; j++)
		{
			//blue = *p++;
			//green = *p++;
			//red = *p++;
			color = get_bmpfile_color(i, j, p);
		
			lcd_draw_pixel(ROW-i, j, color);
			
			color = get_bmpfile_color(ROW-i, j, p);
		
			lcd_draw_pixel(i, j, color);
		}

	return;
}

void lcd_draw_bmp_eff2(int bmp_addr)
{
	char * p = (char *)bmp_addr;	
	int color;
	int i, j;

	// bmp file header is 54 bytes	
	p += 54;
	
	for (j = 0; j <= COL/2; j++)
		for (i = 0; i < ROW; i++)
		{
			color = get_bmpfile_color(i, j, p);
		
			lcd_draw_pixel(ROW-i, j, color);
			
			color = get_bmpfile_color(i, COL-j, p);
		
			lcd_draw_pixel(ROW-i, COL-j, color);
		}

	return;
}

void lcd_draw_bmp_eff3(int bmp_addr)
{
	char * p = (char *)bmp_addr;	
	int color;
	int i, j;
	double k = 0;
	
	// bmp file header is 54 bytes	
	p += 54;
	
	//for (k = 0; k < 1; k += 0.1)
	k = 0.5;
		for (i = ROW/2 - ROW*k/2; i < ROW/2 + ROW*k/2; i++)
			for (j = COL/2-COL*k/2; j < COL/2 + COL*k/2; j++)
			{
				color = get_bmpfile_color(i, j, p);
			
				lcd_draw_pixel(ROW-i, j, color);
			}

	return;
}

void lcd_draw_bmp_eff4(int bmp_addr)
{
	char * p = (char *)bmp_addr;		
	int i, j;
	int k = 10;
	
	short * pixel = (short *)FB_ADDR;
	
	// bmp file header is 54 bytes	
	p += 54;
	
	for (k = 0; k < 10; k += 2)
		for (i = 0; i < ROW; i++)
			for (j = 0; j < COL; j++)
			{
				char red, blue, green;
			//	int color;
				short color_16;	//= BIT24_TO_BIT16(color);
								
				blue = *(p + (i * COL + j)*3);
				green = *(p + (i * COL + j)*3 + 1);
				red = *(p + (i * COL + j)*3 + 2);

				switch(k)
				{
					case 0:
						blue *= 0;
						green *= 0;
						red *= 0;
						break;
					case 2:
						blue *= 0.2;
						green *= 0.2;
						red *= 0.2;
						break;
					case 4:
						blue *= 0.4;
						green *= 0.4;
						red *= 0.4;
						break;
					case 6:
						blue *= 0.6;
						green *= 0.6;
						red *= 0.6;
						break;			
					case 8:
						blue *= 0.8;
						green *= 0.8;
						red *= 0.8;
						break;	
					case 10:
						blue *= 1;
						green *= 1;
						red *= 1;
						break;	
				}
				
				color_16 = BIT24_TO_BIT16_2(red, green, blue);	
	
				*(pixel + ((ROW-i) * COL + j)) = color_16;
				//*((short *)0x33800000 + ((ROW-i) * COL + j)) = color_16;
				
			//	color = get_bmpfile_color_k(i, j, p, k);
				
			//	lcd_draw_pixel(ROW-i, j, color);
			}

	//dma_memcpy(0x33800000, 0x32000000, 0xFFFFF);
	
	return;
}

#define EFF_N		(2+1)

void lcd_draw_bmp(int bmp_addr)
{
	static int eff = 2;
	
	if (eff % EFF_N == 1)
		lcd_draw_bmp_eff1(bmp_addr);		
	else if (eff % EFF_N == 2)
		lcd_draw_bmp_eff2(bmp_addr);
	else if (eff % EFF_N == 3)
		lcd_draw_bmp_eff3(bmp_addr);
	else if (eff % EFF_N == 4)
		lcd_draw_bmp_eff4(bmp_addr);	
	else 
		lcd_draw_bmp_eff1(bmp_addr);
		
	eff++;		
}

void lcd_draw_bmp_to_mem(int bmp_addr, int mem_addr)
{
	char * p = (char *)bmp_addr;
	char red, blue, green;
	int color;
	int i, j;

	// bmp file header is 54 bytes	
	p += 54;
	
	for (i = 0; i < ROW; i++)
		for (j = 0; j < COL; j++)
		{
			short * pixel;
			short color_16;
	
			blue = *p++;
			green = *p++;
			red = *p++;
		
			color = red << 16 | green << 8 | blue << 0;
			
			pixel = (short *)mem_addr;
			color_16 = BIT24_TO_BIT16_2(red, green, blue);
				
			*(pixel + ((ROW-i) * COL + j)) = color_16;
		}

	return;
}