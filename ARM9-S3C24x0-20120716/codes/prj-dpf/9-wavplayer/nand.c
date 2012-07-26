
#define NFCONF		(*(volatile unsigned int *)0x4E000000)	
#define NFCONT		(*(volatile unsigned int *)0x4E000004)	
#define NFCMMD		(*(volatile unsigned int *)0x4E000008)	
#define NFADDR		(*(volatile unsigned int *)0x4E00000c)	
#define NFSTAT		(*(volatile unsigned int *)0x4E000020)	

#define NFDATA		(*(volatile unsigned char *)0x4E000010)	

#define PAGE_SIZE 	512

static int nand_read_page(int page, char * buf, int start_addr, int size)
{
	int i = 0;
	
	if (start_addr >= PAGE_SIZE)
		return -1;
		
	if (start_addr + size > PAGE_SIZE)
		size = PAGE_SIZE - start_addr;
		
	// begin to read nand	
	if (start_addr < PAGE_SIZE/2)
		NFCMMD = 0x00;
	else
		NFCMMD = 0x01;
	NFADDR = start_addr % (PAGE_SIZE/2);
	NFADDR = (page & 0xFF);
	NFADDR = ((page >> 8) & 0xFF);
	NFADDR = ((page >> 16) & 0xFF);
	
	while((NFSTAT & 0x1) == 0)
		;
	
	for (i = start_addr; i < start_addr + size; i++)
		*buf++ = NFDATA;
		
	return 0;
}

int nand_read2(int flash_addr, char * buf, int size)
{
	int page;
	int from = 0;
	int len;
	
	page = flash_addr / PAGE_SIZE;	// >>9	
	from = flash_addr % PAGE_SIZE;
	len = PAGE_SIZE - from;
	
	do
	{	
		nand_read_page(page, buf, from, len);		
		page++;
		buf += len;
		size -= len;
		
		from = 0;
		if (size < PAGE_SIZE)			
			len = size;
		else
			len = PAGE_SIZE;
	}	while (size > 0);
	
	return 0;
}
	
int nand_read(int flash_addr, char * buf, int size)
{
	int page;
	int from = 0;
	page = flash_addr / PAGE_SIZE;	// >>9	
	from = flash_addr % PAGE_SIZE;
	if (from != 0)
	{
		nand_read_page(page, buf, from, PAGE_SIZE-from);
		page++;
		buf += PAGE_SIZE-from;
		size -= PAGE_SIZE-from;
	}	
	while (size >= PAGE_SIZE)
	{
		nand_read_page(page, buf, 0, PAGE_SIZE);
		page++;
		buf += PAGE_SIZE;
		size -= PAGE_SIZE;
	}	
	if (size > 0)
		nand_read_page(page, buf, 0, size);	
	return 0;
}

void nand_init(void)
{
	// timing (4+1)*HCLK = 50ns
	NFCONF |= 4<<8;
	NFCONF |= 4<<4;
	// enable nFCE & Controller
	NFCONT |= 1<<0; 
	NFCONT &= ~(1<<1);
}
