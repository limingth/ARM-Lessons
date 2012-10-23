#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int fd;

void led_running_horse(void)
{
	int id;

	for (id = 0; id < 4; id++)
	{
		ioctl(fd, 1, id);
		usleep(1000*100);

		ioctl(fd, 0, id);
	//	usleep(1000*100);
	}

	return;
}

//int led_open(int argc, char **argv)
int led_open(void)
{
#if 0
	int on;
	int led_no;

	if (argc != 3 || sscanf(argv[1], "%d", &led_no) != 1 || sscanf(argv[2],"%d", &on) != 1 ||
			on < 0 || on > 1 || led_no < 0 || led_no > 3) {
		fprintf(stderr, "Usage: leds led_no 0|1\n");
		exit(1);
	}
#endif

	printf("This is my led\n");
	fd = open("/dev/leds0", 0);
	if (fd < 0) {
		fd = open("/dev/leds", 0);
	}
	if (fd < 0) {
		perror("open device leds");
		exit(1);
	}

	return 0;
}

int led_close(void)
{
	close(fd);

	return 0;
}

