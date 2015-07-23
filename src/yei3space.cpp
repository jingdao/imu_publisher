#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>

#define USB_PATH "/dev/ttyACM0"
#define READ_RATE 10

int fd;

void process(void* data) {
	sensor_msgs::Imu *msg = (sensor_msgs::Imu*) data;
	char buffer[64];
	char *byte = buffer;
	*byte = ' ';
	ssize_t size = write(fd, ":0\n", 3);
	if (size < 3) return;
	while (1) {
		size = read(fd, byte, 1);
		if (size < 1) return;
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
	msg->orientation.x = x;
	msg->orientation.y = y;
	msg->orientation.z = z;
	msg->orientation.w = w;
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

	while (ros::ok()) {
		sensor_msgs::Imu msg;
		msg.header.frame_id = "/imu";
		process(&msg);
		pub.publish(msg);
		ros::spinOnce();
		loop_rate.sleep();
	}
}
