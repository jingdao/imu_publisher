#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define USB_PATH "/dev/ttyACM1"
#define READ_RATE 1

int fd;

void *process(void* data) {
	char byte=' ';
	write(fd, ":0\n", 3);
	while (/*byte!='\0' && */byte!='\n') {
		ssize_t size = read(fd, &byte, 1);
		putchar(byte);
	}
}

int main() {
	fd = open(USB_PATH, O_RDWR);

	int interval = 1000000 / READ_RATE;
	while (1) {
		process(NULL);
		usleep(interval);
	}
}
