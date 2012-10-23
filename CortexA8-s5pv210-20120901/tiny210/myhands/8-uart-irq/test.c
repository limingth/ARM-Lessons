#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <unistd.h>

int fd;

char mygetchar(void)
{
	char c;

	read(fd, &c, 1);

	// echo
	write(fd, &c, 1);

	return c;
}

void myputchar(char c)
{
	write(fd, &c, 1);

	return;
}

void myputs(char * s)
{
	while (*s)
		myputchar(*s++);

	return;
}

void mygets(char * s)
{
	char c;

	while ((c = mygetchar()) != '\r')
		*s++ = c;

	*s = '\0';
	myputs("\r\n");

	return;
}

int main(void)
{
	fd = open("myttyS3", O_RDWR);

	while (1)
	{
		char buf[512];

		myputs("akaedu $ ");
		mygets(buf);
		myputs(buf);
		myputs("\r\n");

	#if 0
		char c;
		c = mygetchar();
		myputchar(c);
	#endif
	}

	close(fd);

	return 0;
}
