#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/ioctl.h>

#define filename "/dev/akae"

//#define SET_A 1
//#define GET_A 2
#define SET_A _IOWR(300,1,struct info)
#define GET_A _IOWR(300,2,struct info)

struct info{
	char a;
	int b;
};

int test_ioctl(int fd)
{
	struct info seta;
	struct info geta;
	int ret;

	seta.a = 'a';
	seta.b = 15;

	ret = ioctl(fd, SET_A, &seta);
	printf("ret SET_A = %d\n", ret);

	ret = ioctl(fd, GET_A, &geta);
	printf("ret GET_A = %d\n", ret);

	printf("geta geta.a=%c, geta.b=%d\n", geta.a, geta.b);

	return 0;
}

int main(void)
{
	int fd;
	char buf[10];
	char *string = "driver_write";
	int ret;

	//printf("%x,%x\n", SET_A, GET_A);

	fd = open(filename, O_RDWR);

	if (fd < 0)
	{
		printf("Open %s file error, please use sudo", filename);
		return -1;	
	}

	ret = read(fd, buf,10);

	if (ret != 10)
	{
		printf("NOTICE: read %s file,get %d Bytes\n", filename, ret);		
	}

	printf("read from driver %s\n", buf);

	ret = write(fd, buf,10);

	if (ret != 10)
	{
		printf("NOTICE: write %s file,get %d Bytes\n", filename, ret);		
	}

	test_ioctl(fd);

	close(fd);

	return 0;
}
