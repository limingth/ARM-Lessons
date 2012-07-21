
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

	// load linux kernel from flash 1M: -> SDRAM 0x30008000 (size = 3M)
	puts("load linux kernel from flash 1M: -> SDRAM 0x30008000 (size = 3M) \n");
	nand_read(0x100000, (char *)0x30008000, 0x300000);	
	
	// load rootfs from flash 4M: -> SDRAM 0x30800000 (size = 6M)	
	puts("load rootfs from flash 4M: -> SDRAM 0x30800000 (size = 6M) \n");
	nand_read(0x400000, (char *)0x30800000, 0x600000);	
	
	fp = (void (*)(int, int, int))addr;
	
	init_taglist(taglist_mem_address);
	
	fp(0, 0x753, taglist_mem_address);
	
	return 0;
}