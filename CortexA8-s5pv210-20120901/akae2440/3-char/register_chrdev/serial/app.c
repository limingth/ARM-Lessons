#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#define filename "/dev/akae"

int main(void)
{
	int fd;
	char buf[11];
	char ch;
	int ret;

	fd = open(filename, O_RDWR);

	if (fd < 0)
	{
		printf("Open %s file error, please use sudo", filename);
		return -1;	
	}

//测试1，收跟发分开测试
#if 1
	ret = read(fd, buf, 11);
	if(ret < 0)
	{
		perror("read error:");
		exit(1);
	}

	printf("read from driver %s\n", buf);

	printf("please input some string to send:\n");
	fgets(buf, 11, stdin);
	
	ret = write(fd, buf, 10);
	if(ret < 0)
	{
		perror("write error:");
		exit(1);
	}
#endif

//测试2，收到的字符小写转大写，再回写回去
#if 1
	while(1)
	{
		read(fd, &ch, 1);
		ch = toupper(ch);
		write(fd, &ch, 1);
		
	}
#endif	
	close(fd);

	return 0;
}
