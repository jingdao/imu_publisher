#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define USB_PATH "/dev/ttyACM0"
#define READ_RATE 1

int fd;

void *process(void* data) {
	char buffer[64];
	char *byte = buffer;
	*byte = ' ';
	write(fd, ":0\n", 3);
	while (1) {
		ssize_t size = read(fd, byte, 1);
		if ((*byte) == '\n')
			break;
		byte++;
	}
	*byte = '\0';
	byte = buffer;
	double x = strtod(byte,&byte);
	byte++;
	double y = strtod(byte,&byte);
	byte++;
	double z = strtod(byte,&byte);
	byte++;
	double w = strtod(byte,&byte);
	printf("%f %f %f %f\n",x,y,z,w);
}

int main() {
	fd = open(USB_PATH, O_RDWR);

	int interval = 1000000 / READ_RATE;
	while (1) {
		process(NULL);
		usleep(interval);
	}
}
