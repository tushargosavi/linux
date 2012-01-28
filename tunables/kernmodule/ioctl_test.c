
#include "tunable_ioctl.h"

#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

int ioctl_set_msg(int fd,char *msg);
int ioctl_get_msg(int fd);
int ioctl_get_nth_byte(int fd);


int main(int argc,char **argv) 
{
	char msg[80];
	int fd=-1;

	if(argc!=2) {
		printf("Usage <dev file name>\n");
		return -1;
	}
	fd=open(argv[1],O_RDONLY|O_WRONLY);	
	if(fd < 0) {
		printf("Unable to open the device file\n");
		return -2;
	}
	printf("calling ioctl_get_msg function\n");
	ioctl_get_msg(fd);
	printf("calling ioctl_set_msg function\n");
	strcpy(msg,"Hello world\n");
	ioctl_set_msg(fd,msg);
	printf("calling ioctl_get_nth_byte functio\n");
	ioctl_get_nth_byte(fd);
	close(fd);
}


int ioctl_get_msg(int fd) 
{
	char msg[90];
	int ret=ioctl(fd,IOCTL_GET_MSG,msg);
	if(ret<0) {
		printf("ioctl IOCTL_GET_MSG failed\n");
	}
	printf("message :%s\n",msg);
	return ret;
}


int ioctl_set_msg(int fd,char *msg) 
{
	int ret=ioctl(fd,IOCTL_SET_MSG,msg);
	if(ret<0) {
		printf("ioctl IOCTL_SET_MSG failed\n");
	}
	return ret;
}

int ioctl_get_nth_byte(int fd)
{
	int i=0;
	char c=1;
	while(c) {
		c=ioctl(fd,IOCTL_GET_NTH_BYTE,i);
		if(c<0) {
			printf("ioctl IOCTL_GET_NTH_BYTE failed for byte no :%d \n",i);
		}
		putchar(c);
	}
	putchar('\n');	
}

