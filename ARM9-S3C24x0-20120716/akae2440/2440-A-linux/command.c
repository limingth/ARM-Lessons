#include "stdio.h"
#include "nand.h"
//#include <string.h>

int help(int argc, char * argv[])
{
//	printf("md - memory display\n");
//	printf("mw - memory write\n");
//	printf("loadb - load file to memory\n");
//	printf("loadx - load file with xmodem to memory\n");
	return 0;
}

int atoi(char * buf)
{
	// "123" -> 123
	// "0x10" -> 16
	int num = 0;
	int radix = 10;
	
	if ((*buf == '0' ) && (*(buf + 1) == 'x')) {
		buf += 2;
		radix = 16;
	}
	
	while (*buf) {
		if (*buf <= '9') 
			num = num * radix + *buf - '0';
		else
			num = num * radix + *buf - 'a' + 10;
		buf++;
	}
	
	return num;
}

//#define LOAD_ADDR	0xC00	// 3k
#define LOAD_ADDR	0x31000000
#define LOAD_SIZE 	324

int go(int argc, char * argv[])
{
	// go 0x0(addr) 
	int addr = LOAD_ADDR;
	void (*fp)(void);
	
	if (argc > 1) 	
		addr = atoi(argv[1]);
		
	fp = (void (*)(void))addr;
	
	fp();
	
	return 0;
}

typedef unsigned int __u32;
typedef unsigned char __u8;
typedef unsigned short __u16;

struct tag_header {
	__u32 size;
	__u32 tag;
};
struct tag_core {
	__u32 flags;		/* bit 0 = read-only */
	__u32 pagesize;
	__u32 rootdev;
};
struct tag_mem32 {
	__u32	size;
	__u32	start;	/* physical start address */
};

struct tag_initrd {
	__u32 start;	/* physical start address */
	__u32 size;	/* size of compressed ramdisk image in bytes */
};

struct tag_cmdline {
	char	cmdline[1];	/* this is the minimum size */
};

struct tag {
	struct tag_header hdr;
	union {
		struct tag_core		core;
		struct tag_mem32	mem;
		struct tag_initrd	initrd;
		struct tag_cmdline	cmdline;
	} u;
};

#define tag_next(t)	((struct tag *)((__u32 *)(t) + (t)->hdr.size))
#define tag_size(type)	((sizeof(struct tag_header) + sizeof(struct type)) >> 2)

/* The list must start with an ATAG_CORE node */
#define ATAG_CORE	0x54410001
/* it is allowed to have multiple ATAG_MEM nodes */
#define ATAG_MEM	0x54410002
/* describes where the compressed ramdisk image lives (physical address) */
#define ATAG_INITRD2	0x54420005
/* The list ends with an ATAG_NONE node. */
#define ATAG_NONE	0x00000000
/* command line: \0 terminated string */
#define ATAG_CMDLINE	0x54410009

char cmd[] =	
		"console=ttySAC0,115200 "	// must, 否则 最后无法进入 console
		//"init=/linuxrc "		// optional, 可省略
		"cs89x0_media=rj45 "		// must, 否则最后只有lo，没有 eth0
		//"initrd=0x30800000,0x800000 "	// optional, 可省略
		//"root=/dev/ram "		// optional, 可省略
		"ip=192.168.0.100:192.168.0.2:<NULL>:255.255.255.0::eth0:off"	// must, 否则最后无 lo 和 eth0
		;

void init_taglist(int addr)
{
	struct tag * p;
	int i;
	
	p = (struct tag*) addr;
	p->hdr.tag  =  ATAG_CORE;
	p->hdr.size = tag_size(tag_core);
	p->u.core.flags = 1;
	p->u.core.pagesize = 4096;
	p->u.core.rootdev = 0x00000000;

	p = tag_next(p);
	p->hdr.tag = ATAG_CMDLINE;
	p->hdr.size =  (sizeof (cmd) + sizeof(struct tag_header) + 3) >>2;	
	for(i=0; i< sizeof (cmd); i++)	
		p->u.cmdline.cmdline[i] = cmd[i];
	
	p = tag_next(p);
	p->hdr.tag = ATAG_MEM;
	p->hdr.size = tag_size(tag_mem32);
	p->u.mem.size = 64*1024*1024;
	p->u.mem.start = 0x30000000;

	p = tag_next(p);
	p->hdr.tag = ATAG_INITRD2;
	p->hdr.size = tag_size(tag_initrd);
	p->u.initrd.size = 8*1024*1024; //7M Bytes	/* decompressed ramdisk size in _kilo_ bytes */
	p->u.initrd.start = 0x30800000;	/* starting block of floppy-based RAM disk image */
	
	p = tag_next(p);
	p->hdr.tag = ATAG_NONE;
	p->hdr.size = 0;
}

int linux(int argc, char * argv[])
{
	// linux start address must be 0x30008000
	int addr = 0x30008000;	
	void (*fp)(int, int, int);
	int taglist_mem_address = 0x30000000;
			
	fp = (void (*)(int, int, int))addr;
	
	init_taglist(taglist_mem_address);
	
	fp(0, 0x753, taglist_mem_address);
	
	return 0;
}

int loadx(int argc, char * argv[])
{
	// loadb 0x0(addr) 100(size)
	int addr = LOAD_ADDR;
	int i;
	
	if (argc > 1)
		addr = atoi(argv[1]);
	
	for (i = 0; i < 0x900000; i++)
		if ((i & 0xfffff) == 0)
			printf("%d ", i>>20);
			
#define	SOH 0x01
#define	EOT 0x04
#define	ACK 0x06
#define	NAK 0x15
#define	CAN 0x18 

	putchar(NAK);
	while (getchar() != EOT)
	{
		// get 131 bytes
		// omit 2 bytes (serial num)
		getchar();
		getchar();
		
		// get 128 bytes (data)
		for (i = 0; i < 128; i++)
			*(char *)(addr++) = getchar();

		// omit 1 byte (parity)
		getchar();

		// send ACK
		putchar(ACK);
	}
	putchar(ACK);
	
	return 0;
}

int loadb(int argc, char * argv[])
{
	// loadb 0x0(addr) 100(size)
	int addr = LOAD_ADDR;
	int size = LOAD_SIZE;
	int i;
	
	if (argc > 2) 
	{
		addr = atoi(argv[1]);
		size = atoi(argv[2]);
	}
	
	for (i = 0; i < size; i++)
		*(char *)(addr + i) = getchar();		
		
	return 0;
}	

#define NAND_ADDR	0x1      
#define BUF_ADDR	0x32000000

int nr(int argc, char * argv[])
{
	int flash_addr = NAND_ADDR;
	char * buf = (char *)BUF_ADDR;
	int size = 0x1000;
	
	if (argc > 3)
	{
		flash_addr = atoi(argv[1]);
		buf = (char *)atoi(argv[2]);
		size = atoi(argv[3]);
	}
	
	nand_read(flash_addr, buf, size);
	printf("nandread 0x%x to buf 0x%x (size = 0x%x) OK!\n", flash_addr, (int)buf, size);
	
	return 0;
}

int mw(int argc, char * argv[])
{
	// mw 0x0 0x1234
	int addr = 0;
	int val = 0;
	
	if (argc < 3) 
		return 0;
		
	addr = atoi(argv[1]);	
	val = atoi(argv[2]);
	
	if (argc == 3)
		*(int *)addr = val;
	else
		*(char *)addr = val;
	
	return 0;
}

void autoplay(void)
{
#if 0	
	char * go_argv[2] = {"go", "0x31000000"};
	
	// flash 1M: -> SDRAM 0x32000000
	nand_read(0x100000, (char *)0x32000000, 0x100000);
	
	// flash 2M: -> SDRAM 0x33000000
	nand_read(0x200000, (char *)0x33000000, 0x100000);	
	
	// flash 512K: -> wavplayer.bin
	nand_read(0x80000, (char *)0x31000000, 0x80000);
	
	// go
	//((void (*)(void)0x31000000)();	
	go(2, go_argv);
#endif

#if 1
	// load linux kernel from flash 1M: -> SDRAM 0x30008000 (size = 2M)
	printf("load linux kernel from flash 1M: -> SDRAM 0x30008000 (size = 2M) \n");
	nand_read(0x100000, (char *)0x30008000, 0x200000);	
	
	// load rootfs from flash 4M: -> SDRAM 0x30800000 (size = 6M)	
	printf("load rootfs from flash 4M: -> SDRAM 0x30800000 (size = 6M) \n");
	nand_read(0x400000, (char *)0x30800000, 0x600000);
#endif
	//nand_read(0x4000, (char *)0x31000000, 0x8000);	// flash = 16k;	size = 32k
			
	//((void (*)(void)0x31000000)();
}

int play(int argc, char * argv[])
{
	autoplay();
	
	return 0;
}

int md(int argc, char * argv[])
{
	int addr = 0;
	int * p;
	int i, j;

	if (argc > 1)
		addr = atoi(argv[1]);
	
	p = (int *)addr;
	
	for (i = 0; i < 16; i++)
	{	
		printf("%x: ", (int)p);
		for (j = 0; j < 4; j++)	
			printf("%x ", *p++);			
		printf("\n");
	}
	return 0;
}

int strcmp(const char * s1, const char * s2)
{
	while (*s1 == *s2)
	{
		if (*s1 == '\0')
			return 0;
		s1++;
		s2++;
	}
	
	return 	*s1 - *s2;
}

struct CMD
{
	char name[8];
	int (*fp)(int argc, char *argv[]);
};

struct CMD command[] =
{
	{"help", help},
	{"md", md},
	{"mw", mw},
	{"loadb", loadb},
	{"go", go},
	{"loadx", loadx},
	{"nr", nr},
	{"play", play},
	{"linux", linux},
};

int command_do_main(int argc, char * argv[])
{
	int cmd_num;
	int i;
	
	cmd_num = sizeof(command) / sizeof(struct CMD);
	for (i = 0; i < cmd_num; i++)
	{	
		if (strcmp(argv[0], command[i].name) == 0)
		{
			command[i].fp(argc, argv);
			return 0;
		}
	}
	
	printf("unknown command <%s>\n", argv[0]);
	
#if 0
	if (strcmp(argv[0], "help") == 0)
		help(argc, argv);
	
	if (strcmp(argv[0], "md") == 0)
		md(argc, argv);
		
	if (strcmp(argv[0], "mw") == 0)
		mw(argc, argv);	

	if (strcmp(argv[0], "loadb") == 0)
		loadb(argc, argv);	

	if (strcmp(argv[0], "go") == 0)
		go(argc, argv);	
#endif	
	return 0;
}