#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>

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

int main(int argc,char* argv[]) {
	ros::init(argc,argv,"imu");
	ros::NodeHandle n;
	ros::Publisher pub = n.advertise<sensor_msgs::Imu>("imu/data",1000);
	ros::Rate loop_rate(READ_RATE);

	fd = open(USB_PATH, O_RDWR);
	if (fd<0) {
		printf("Cannot open %s\n",USB_PATH);
		return 1;
	}

	int interval = 1000000 / READ_RATE;
	while (ros::ok()) {
		//process(NULL);
		//usleep(interval);
		sensor_msgs::Imu msg;
		msg.header.frame_id = "/imu";
		process(&msg);
		pub.publish(msg);
		ros::spinOnce();
		loop_rate.sleep();
	}
}
