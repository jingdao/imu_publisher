#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <ros/ros.h>
#include <sensor_msgs/Imu.h>

#define USB_PATH "/dev/ttyACM1"
#define READ_RATE 10
#define GRAVITY 9.81

int fd;
unsigned int count = 0;

bool readFromDevice(char* command,char* response) {
	*response = ' ';
	ssize_t size = write(fd, command, strlen(command));
	if (size < 3) return false;
	while (1) {
		size = read(fd, response, 1);
		if (size < 1) return false;
		if ((*response) == '\n')
			break;
		response++;
	}
	*response = '\0';
	return true;
}

void process(void* data) {
	sensor_msgs::Imu *msg = (sensor_msgs::Imu*) data;
	char buffer[64];

	//write header
	msg->header.seq = count++;
	struct timespec clock;
	clock_gettime(CLOCK_REALTIME,&clock);
	msg->header.stamp.sec = clock.tv_sec;
	msg->header.stamp.nsec = clock.tv_nsec;
	msg->header.frame_id = "/imu";

	//get orientation
	if (readFromDevice(":0\n",buffer)) {
		char *byte = buffer;
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

	//get gyro data
	if (readFromDevice(":38\n",buffer)) {
		char *byte = buffer;
		double x = strtod(byte,&byte);
		byte++;
		double y = strtod(byte,&byte);
		byte++;
		double z = strtod(byte,&byte);
		msg->angular_velocity.x = x;
		msg->angular_velocity.y = y;
		msg->angular_velocity.z = z;
	}

	//get accelerometer data
	if (readFromDevice(":39\n",buffer)) {
		char *byte = buffer;
		double x = strtod(byte,&byte) * GRAVITY;
		byte++;
		double y = strtod(byte,&byte) * GRAVITY;
		byte++;
		double z = strtod(byte,&byte) * GRAVITY;
		msg->linear_acceleration.x = x;
		msg->linear_acceleration.y = y;
		msg->linear_acceleration.z = z;
	}
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
		process(&msg);
		pub.publish(msg);
		ros::spinOnce();
		loop_rate.sleep();
	}
}
