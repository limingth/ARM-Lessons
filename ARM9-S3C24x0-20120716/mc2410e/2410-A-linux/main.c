#include "uart.h"
#include "stdio.h"
#include "command.h"
#include "nand.h"
//#include <stdio.h>

#define WTCON 	(*(volatile int *)0x53000000)
	
#define LED		(*(volatile unsigned char *)0x20800000)

#define GPBCON		(*(volatile unsigned int *)0x56000010)
#define GPBDAT		(*(volatile unsigned int *)0x56000014)

#define MPLLCON		(*(volatile unsigned int *)0x4C000004)
#define CLKDIVN 	(*(volatile unsigned int *)0x4c000014)
#define UTXH0 		(*(volatile unsigned int *)0x50000020)
#define UBRDIV0 	(*(volatile unsigned int *)0x50000028)

#define BWSCON 		(*(volatile unsigned int *)0x48000000)
#define BANKCON6	(*(volatile unsigned int *)0x4800001C)
#define BANKSIZE	(*(volatile unsigned int *)0x48000028)
#define REFRESH		(*(volatile unsigned int *)0x48000024)

#define UTRSTAT0 *(volatile int *)0x50000010
#define URXH0 *(volatile int *)0x50000024
	
void sys_init(void)
{
	WTCON = 0;//disable watch dog
	
	GPBCON |= 1<<0;//disable beeper
	GPBCON &= ~(1<<1);	
	GPBDAT = 0x0;
}

void clock_init(void)
{
#ifdef S3C2440
	MPLLCON = 0x44011;
	CLKDIVN = 0x7;
#endif
	
	// 1:2:4	
	// PCLK:HCLK:FCLK = 50M:100M:200M
	CLKDIVN = 0x1<<0 | 0x1<<1;

	MPLLCON = 0x0005C040;	
	
	//UBRDIV0 = 325;	
	UBRDIV0 = 26;		// 115200 baudrate	
}

void sdram_init(void)
{
	// DW6 [25:24]	10 = 32-bit  
	BWSCON |= 1<<25;
	
	// SCAN [1:0] Column address number		01 = 9-bit 
	BANKCON6 |= 1<<0;

	// BK76MAP [2:0] BANK6/7 memory map		001 = 64MB/64MB
	BANKSIZE = 0x1;
	
	// refresh period is 7.8 us and HCLK is 100MHz
	// Refresh count = 211 + 1 - 100x7.8 = 1269
	// 7.8 us = 64ms / 8192(8K) = 7.8125
	REFRESH |= 1269;
}

#if 0
int shell_parse2(char * buf, char * argv[])
{
	int argc = 0;
	int state = 0;
	int input = 0;
	
	while (*buf)
	{
		//input = get_input_type(*buf);
		if (*buf == ' ')
			input = 0;
		else
			input = 1;
		
		switch (state)
		{
			case 0:
				switch (input)
				{
					case 0: // ' '						
						break;
					case 1: // abc
						argv[argc++] = buf;
						state = 1;
						break;
				}
				break;
			case 1:	
				switch (input)
				{
					case 0: // ' '
						*buf = '\0';
						state = 0;
						break;
					case 1: // abc					
						break;
				}	
				break;
		}	
		buf++;
	}

	return argc;
}
#endif

int shell_parse(char * buf, char * argv[])
{
	int argc = 0;
	int state = 0;

	while (*buf != '\0')
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

void autoplay(void);

	
typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned long	U32;


#define IICBUFSIZE 0x20

#define WRDATA      (1)
#define POLLACK     (2)
#define RDDATA      (3)
#define SETRDADDR   (4)

static U8 __iicData[IICBUFSIZE];
static volatile int __iicDataCount;
static volatile int __iicStatus;
static volatile int __iicMode;
static int __iicPt;
static int __iic_error = 0;

#define rIICCON  (*(volatile unsigned *)0x54000000) //IIC control
#define rIICSTAT (*(volatile unsigned *)0x54000004) //IIC status
#define rIICADD  (*(volatile unsigned *)0x54000008) //IIC address
#define rIICDS   (*(volatile unsigned *)0x5400000c) //IIC data shift

#define IICCON  rIICCON  
#define IICSTAT rIICSTAT 
#define IICADD  rIICADD  
#define IICDS   rIICDS   

void _Delay(int time)
{
	volatile int i, j;

	for (i = 0; i < time * 512; i++)
		j = i;
}


#if 0
//**********************[IicPoll ]**************************************
void _IicPoll(void)
{
    U32 iicSt,i;
    
    iicSt = rIICSTAT; 
    if(iicSt & 0x8){}                   //When bus arbitration is failed.
    if(iicSt & 0x4){}                   //When a slave address is matched with IICADD
    if(iicSt & 0x2){}                   //When a slave address is 0000000b
    if(iicSt & 0x1){}                   //When ACK isn't received
printf("I 1\n");

    switch(__iicMode)
    {
        case POLLACK:
            __iicStatus = iicSt;
            break;

        case RDDATA:
            if((__iicDataCount--)==0)
            {
                __iicData[__iicPt++] = rIICDS;
            
                rIICSTAT = 0x90;                //Stop MasRx condition 
                rIICCON  = 0xaf;                //Resumes IIC operation.
                _Delay(1);                       //Wait until stop condtion is in effect.
                                                //Too long time... 
                                                //The pending bit will not be set after issuing stop condition.
                break;    
            }      
            __iicData[__iicPt++] = rIICDS;
                        //The last data has to be read with no ack.
            if((__iicDataCount)==0)
                rIICCON = 0x2f;                 //Resumes IIC operation with NOACK.  
            else 
                rIICCON = 0xaf;                 //Resumes IIC operation with ACK
            break;

        case WRDATA:
	printf("I 2\n");	
            if((__iicDataCount--)==0)
            {
                rIICSTAT = 0xd0;                //stop MasTx condition 
                rIICCON  = 0xaf;                //resumes IIC operation.
                _Delay(1);                       //wait until stop condtion is in effect.
                       //The pending bit will not be set after issuing stop condition.
                break;    
            }
            rIICDS = __iicData[__iicPt++];        //_iicData[0] has dummy.
            for(i=0;i<10;i++);                  //for setup time until rising edge of IICSCL
            rIICCON = 0xaf;                     //resumes IIC operation.
	printf("I 3\n");	    
            break;

        case SETRDADDR:
//          Uart_Printf("[S%d]",_iicDataCount);
            if((__iicDataCount--)==0)
            {
                break;                  //IIC operation is stopped because of IICCON[4]    
            }
            rIICDS = __iicData[__iicPt++];
            for(i=0;i<10;i++);          //for setup time until rising edge of IICSCL
            rIICCON = 0xaf;             //resumes IIC operation.
            break;

        default:
            break;      
    }
}

void _Run_IicPoll(void)
{
    if(rIICCON & 0x10)                  //Tx/Rx Interrupt Enable
       _IicPoll();
}     

void __Wr24C080(U32 slvAddr,U32 addr,U8 data)
{
   int times = 0;
   
    __iicMode      = WRDATA;
    __iicPt        = 0;
    __iicData[0]   = (U8)addr;
    __iicData[1]   = data;
    __iicDataCount = 2;

    if (__iic_error)
		return;
    
    printf("W 1\n");
    rIICDS        = slvAddr;            //0xa0
      //Master Tx mode, Start(Write), IIC-bus data output enable
      //Bus arbitration sucessful, Address as slave status flag Cleared,
      //Address zero status flag cleared, Last received bit is 0
    rIICSTAT      = 0xf0;   

      //Clearing the pending bit isn't needed because the pending bit has been cleared.
//    while(__iicDataCount!=-1)
//       _Run_IicPoll();

	__iicDataCount = -1;
	
    __iicMode = POLLACK;

    printf("W 2\n");
    while(1)
    {
        rIICDS     = slvAddr;
        __iicStatus = 0x100;             //To check if _iicStatus is changed 
        rIICSTAT   = 0xf0;              //Master Tx, Start, Output Enable, Sucessful, Cleared, Cleared, 0
        rIICCON    = 0xaf;              //Resumes IIC operation. 
        while(__iicStatus==0x100 && times++ < 512)  
            _Run_IicPoll();

	 if (times > 512)
	 	__iic_error = 1;
        if(!(__iicStatus & 0x1) || __iic_error)
            break;                      //When ACK is received
    }
    rIICSTAT = 0xd0;                    //Master Tx condition, Stop(Write), Output Enable
    rIICCON  = 0xaf;                    //Resumes IIC operation. 
    _Delay(1);                           //Wait until stop condtion is in effect.
      //Write is completed.
    printf("W 3\n"); 
}
#endif


#define VGA_ADDR	0xEA
//	#define VGA_ADDR	0x75
#if 1
#define VGA_REG_ADDR_MAX	0x3f
static unsigned char vga640480[] = 
	{
		0x61,0x02,0x0b,0x00,0x20,0x37,0x10,0x8c,
		0x01,0x69,0x30,0x20,0x05,0x03,0x1b,0x15,
		0x00,0xcb,0xff,0x40,0x3f,0x7e,0x0d,0x40,
		0x62,0x00,0x68,0x00,0xf0,0x00,0x00,0x00,
		0x0a,0x01,0x00,0x00,0x01,0x32,0x00,0x00,
		0x00,0x00,0x54,0x01,0xae,0xae,0xd7,0x81,
		0x08,0xfa,0xfa,0x1f,0x0f,0xfa,0xfa,0x03,
		0xfc,0x00,0x00,0x0f,0x0f,0x00,0x00,0x3f
	};
#endif


void _IIC_write(U32 slvAddr, U32 addr, U8 data)
{
	printf("IIC W 1\n");
	IICDS = slvAddr;
	IICSTAT = 0xf0;
	
	printf("W 1\n");	
	while ((IICCON & 0x10) == 0);	// INT
	printf("W 2\n");	
	while ((IICSTAT & 0x1));	// ACK
	_Delay(1);   
	
	printf("W 3\n");	
	
	IICDS = addr;
	IICCON = 0xaf;
	
	printf("IIC W 2\n");
	while ((IICCON & 0x10) == 0);	// INT
	while ((IICSTAT & 0x1));	// ACK
	_Delay(1);   
	
	IICDS = data;
	IICCON = 0xaf;
	
	printf("IIC W 3\n");
	while ((IICCON & 0x10) == 0);	// INT
	while ((IICSTAT & 0x1));	// ACK
	_Delay(1);   
	
	IICSTAT = 0xd0;	// stop write
	IICCON = 0xaf;
	_Delay(1);   
	
	printf("IIC W 4\n");
	while (1)
	{
		IICDS = slvAddr;
		IICSTAT = 0xf0;
		IICCON = 0xaf;
		
		while ((IICCON & 0x10) == 0);	// INT
		if (!(IICSTAT & 0x1))
		//_Delay(1);   
			break;
	}
	
	printf("IIC W 5\n");
	IICSTAT = 0xd0;
	IICCON = 0xaf;
	_Delay(1);   
}

	
void display_format(int mode)
{
	int i;
	unsigned char *ptr = 0;
	
	ptr = vga640480;
	if (0)
	{
		for (i = 0; i <= VGA_REG_ADDR_MAX; i++)
			//__Wr24C080(VGA_ADDR, i, ptr[i]);
			_IIC_write(VGA_ADDR, i, ptr[i]);
		//_IIC_write(VGA_ADDR, 0x4, 0x20);	
		//__Wr24C080(VGA_ADDR, 0x04, 0x20);
	}
	else
	{
		_IIC_write(VGA_ADDR, 4, 0x20);
		_IIC_write(VGA_ADDR, 7, 0x8c);
		_IIC_write(VGA_ADDR, 14, 0x1b);
		_IIC_write(VGA_ADDR, 13, 0x03);	
	}

	//printf("Display format changed to %s mode\n", "640*480");
	printf("%s mode\n", "640*480");
}

#define rGPECON    (*(volatile unsigned *)0x56000040) //Port E control
#define rGPEDAT    (*(volatile unsigned *)0x56000044) //Port E data
#define rGPEUP     (*(volatile unsigned *)0x56000048) //Pull-up control E
int display_init()
{	
	unsigned value;
	
//	IICCON = ACK_ENA | CLK_512 | 0xf;

//	rGPEUP  |= 0xc000;                  //Pull-up disable
//	rGPECON |= 0xa0000000;                //GPE15:IICSDA , GPE14:IICSCL    
	*(volatile int *)0x56000048 |= 0xc000;		// GPEUP
	*(volatile int *)0x56000040 |= 0xa0000000;		// GPECON: IICSDA, IICSCL

      //Enable ACK, Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
//	rIICCON  = (1<<7) | (0<<6) | (1<<5) | (0xf);
	*(volatile int *)0x54000000 = 0xaf;		// IICCON
	
//    rIICADD  = 0x10;                    //2410 slave address = [7:1]
//    rIICSTAT = 0x10;                    //IIC bus data output enable(Rx/Tx)
	*(volatile int *)0x54000008 = 0x10;		// IICADD
	*(volatile int *)0x54000004 = 0x10;		// IICSTAT
	
//	BANKCON6=0x00018001;  //TRCD=2 clocks /SDRAM
//	BANKCON7=0x00018001;  //TRCD=2 clocks /SDRAM
//	REFRESH=0x008e01e9;   //刷新时间为15.6us HCLK=@100M
//	MRSRB6=0x20;   //CL=2HCLK
//	MRSRB7=0x20;   //CL=2HCLK
	//*(int *)0x4800001C=0x00018001;  //TRCD=2 clocks /SDRAM
	//*(int *)0x48000020=0x00018001;  //TRCD=2 clocks /SDRAM
	//*(int *)0x48000024=0x008e01e9;   //刷新时间为15.6us HCLK=@100M
	//*(int *)0x4800002C=0x20;   //CL=2HCLK
	//*(int *)0x48000030=0x20;   //CL=2HCLK

    	//display_format(value);
	_IIC_write(0xEA, 0x4, 0x20);
	
	return 0;
}

int mymain()
{	
	char buf[100];
	int argc;
	char * argv[5];
	int i = 0;
	int userinput = 0;
	
	sys_init();
	uart_init();	
	clock_init();
	sdram_init();
	nand_init();
		
	puts("\n\nWelcome to my boot 1.0! \n");
	
	display_init();
	
//	printf("buf %c is %s, %d, %x %x \n", 'A', "abcd", 100, 101, 100);
	
	//printf("please input Enter to Shell mode\n");
	for (i = 0x300000; i > 0; i--)
	{
		if ((i & 0xfffff) == 0) 
			printf("time left %d\r", i>>20);
	
		if (UTRSTAT0 & (1<<0))
		{
			userinput = URXH0;	// no use
			userinput = 1;			
			break;
		}
	}
	printf("\n");
	
	// judge if user input ENTER
	if (userinput != 1)
	{
		// autoplay
		//printf("autoplay begin... \n");
		//autoplay();
		//printf("autorun begin... \n");
		autoplay();
	}
	
	while(1)
	{
		puts("myboot> ");
		gets(buf);
		//printf("input command is <%s> \n", buf);
		
		argc = shell_parse(buf, argv);
		//for (i = 0; i < argc; i++)
		//	printf("argv[%d] is <%s> \n", i, argv[i]);
			
		//printf("command is <%s> \n", argv[0]);
		command_do_main(argc, argv);
	}
}