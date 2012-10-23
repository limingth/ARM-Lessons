#include <stdio.h>

//#define UTXH	0xE2900020
#define UART_BASE 0xE2900000

int global = 100;

#include <fcntl.h>     // open(), O_RDWR
#include <sys/mman.h>  // mmap(), PROT_READ
#include <stdio.h>
//#include <stdlib.h>

#if 0
void * init(int addr)
{
	void * vmem;
	int fd = open("/dev/mem", O_RDWR);

	if (fd < 0)
	{
		printf("FD error!\n");
		exit(1);
	}

	vmem = mmap(0,1,PROT_READ|PROT_WRITE, MAP_SHARED, fd, addr);
	if (vmem == MAP_FAILED)
	{
		printf("Mmap error!\n");
		exit(1);
	}

	return vmem;
}
#endif

int main(void)
{
	int local = 200;
	volatile int * pVA;
	int i = 0;

	printf("hello, demo write driver in user space\n");

	printf(".text = 0x%x\n", (int)main);
	printf(".data = 0x%x\n", (int)&global);
	printf("stack = 0x%x\n", (int)&local);
	printf("main ok!\n");

	int fd = open("/dev/mem", O_RDWR);
	pVA = mmap(0,1,PROT_READ|PROT_WRITE, MAP_SHARED, fd, UART_BASE);
	printf("pVA = %p\n", pVA);

	pVA += 0x20/4;

	for (i = 0; i < 1000; i++)
	{
//		*(volatile int *)UTXH = 'a';
	//	*(volatile int *)0x40020020 = '9';
		*pVA = 'a';
	}

	return 0;
}



