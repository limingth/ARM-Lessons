

#define COMMAND_LINE_SIZE 128

/* This is the old deprecated way to pass parameters to the kernel */
struct param_struct {
    union {
        struct {
            unsigned long page_size; /* 0 */
            unsigned long nr_pages; /* 4 */
            unsigned long ramdisk_size; /* 8 */
            unsigned long flags; /* 12 */
#define FLAG_READONLY 1
#define FLAG_RDLOAD 4
#define FLAG_RDPROMPT 8
            unsigned long rootdev; /* 16 */
            unsigned long video_num_cols; /* 20 */
            unsigned long video_num_rows; /* 24 */
            unsigned long video_x; /* 28 */
            unsigned long video_y; /* 32 */
            unsigned long memc_control_reg; /* 36 */
            unsigned char sounddefault; /* 40 */
            unsigned char adfsdrives; /* 41 */
            unsigned char bytes_per_char_h; /* 42 */
            unsigned char bytes_per_char_v; /* 43 */
            unsigned long pages_in_bank[4]; /* 44 */
            unsigned long pages_in_vram; /* 60 */
            unsigned long initrd_start; /* 64 */
            unsigned long initrd_size; /* 68 */
            unsigned long rd_start; /* 72 */
            unsigned long system_rev; /* 76 */
            unsigned long system_serial_low; /* 80 */
            unsigned long system_serial_high; /* 84 */
            unsigned long mem_fclk_21285; /* 88 */
        } s;
        char unused[256];
    } u1;
    union {
        char paths[8][128];
        struct {
            unsigned long magic;
            char n[1024 - sizeof(unsigned long)];
        } s;
    } u2;
    char commandline[COMMAND_LINE_SIZE];
};
 
#define LINUX_KERNEL_OFFSET	0x8000
#define LINUX_PARAM_OFFSET	0x100
#define LINUX_PAGE_SIZE		0x1000	// 4K
#define LINUX_PAGE_SHIFT	12
#define LINUX_ZIMAGE_MAGIC	0x016f2818
#define DRAM_SIZE		(0x100000*64)	// 64M
char * linux_cmd = "root=/dev/mtdblock3 "
				"init=/linuxrc console=ttyS0,115200 "
			//	"cs89x0_media=rj45 ";
				"ip=192.168.0.101:192.168.0.100:<NULL>:255.255.255.0::eth0:on";
 
int memset0(char * p, int v, int size)
{
	while (size--)
		*p++ = v;
	
	return 0;
}

int memcpy0(char * s1, char * s2, int size)
{
	int i = 0;
	
	for (i = 0; i < size; i++)
		*s1++ = *s2++;
	
	return 0;
}

int strlen0(char * s)
{
	int len = 0;
	
	while (*s++)
		len++;
		
	return len;
}

void setup_linux_param(unsigned long param_base)
{
	struct param_struct *params = (struct param_struct *)param_base; 

	memset0((char *)params, 0, sizeof(struct param_struct));
	
	/* 参数传递 */
	params->u1.s.page_size = LINUX_PAGE_SIZE;
	params->u1.s.nr_pages = (DRAM_SIZE >> LINUX_PAGE_SHIFT);

	/* set linux command line */
//	linux_cmd = get_linux_cmd_line();
	memcpy0(params->commandline, linux_cmd, strlen0(linux_cmd) + 1);
}

int linux(void)
{
	int taglist = 0x30000100;
	void (*fp)(int, int, int);
	
	// load linux kernel from flash 1M: -> SDRAM 0x30008000 (size = 2M)
	puts("load linux kernel from flash 0x30000: -> SDRAM 0x30008000 (size = 2M) \n");
	//nand_read(0x30000, (char *)0x30008000, 0x200000);	
	
	fp = (void (*)(int, int, int))0x30008000;
	
	puts("linux go to 0x30008000\n");
	
	setup_linux_param(taglist);
	fp(0, 193, taglist);
	
	return 0;
}
