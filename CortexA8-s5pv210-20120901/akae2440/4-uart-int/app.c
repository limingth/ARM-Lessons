#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define filename "/dev/my_uart"

#define SET_A      1
#define GET_A      2

struct info{
	char a;
	int  b;
};

/*int test_ioctl (int fd)
{
	struct info seta;
	struct info geta;
	int ret;

	seta.a = 'a';
	seta.b = 15;

	ret = ioctl(fd,SET_A,&seta);
	printf ("ret SET_A = %d\n",ret);

	ret = ioctl(fd,GET_A,&geta);	
	printf ("ret GET_A = %d\n",ret);

	printf ("geta geta.a=%c,geta.b=%d\n",geta.a,geta.b);

	return 0;
}*/

int main (void)
{
	int fd;
	char buf = 'H';
	char *string ="driver_write";
	int ret, m = 10;
	int i = 0;


	fd = open (filename,O_RDWR);

	if (fd < 0)
	{
		printf ("Open %s file error!\n",filename);
		return -1;
	}

	while (m--)
	{
		read(fd, &buf, 1);
		putchar(buf);
		fflush(stdout);
		//fgets(&buf, 10, stdin);
		buf =  string[i++];
		if(buf == '\0') break;
		write(fd, &buf, 1);
		printf("in app, write!!! buf=%c\n", buf);
		usleep(100);

	}

/*
	ret = read (fd,buf,10);
	if (ret != 10)
	{
		printf ("NOTICE: read %s file,get %d Bytes\n",filename,ret);
	}

	printf ("read from driver %s\n",buf);

	ret = write (fd,string,10);
	if (ret != 10)
	{
		printf ("NOTICE: write %s file,get %d Bytes\n",filename,ret);
	}

//	test_ioctl (fd);
*/
	close (fd);

	return 0;

}
