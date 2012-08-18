
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void)
{
	int fd;

	fd = open("myuart", O_RDWR);

	printf("fd = %d\n", fd);

	while (1)
	{
		ioctl(fd, 19200, 0, 0);
		write(fd, "hello\n", 7);

		ioctl(fd, 115200, 0, 0);
		write(fd, "world\n", 7);
	}

	close(fd);

	return 0;
}
