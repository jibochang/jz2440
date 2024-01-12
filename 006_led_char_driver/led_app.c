#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int fd;
	int val = 1;
	if (argc != 2)
	{
		printf("Usage :\n");
		printf("%s <on|off>\n", argv[0]);
		return 0;
	}

	fd = open("/dev/leds", O_RDWR);
	if (fd < 0) 
	{
		printf("can't open /dev/leds!\n");
	}

	if (strcmp(argv[1], "on") == 0) {
		printf("open led");
		val = 1;
	}
	else if (strcmp(argv[1], "off") == 0) {
		printf("close led");
		val = 0;
	} else {
		printf("Usage :\n");
		printf("%s <on|off>\n", argv[0]);
		close(fd);
		return 0;
	}

	write(fd, &val, sizeof(val));


	return 0;
}

