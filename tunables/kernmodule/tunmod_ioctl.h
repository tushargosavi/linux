#ifndef TUNABLE_IOCTL_H
#define TUNABLE_IOCTL_H

#include <linux/ioctl.h>

#define MAJOR_NO   253

struct	tunable_arg {
	char *name;
	int type;
	void *value;
	int size;
};

 
#define IOCTL_SET_MSG  _IOR(MAJOR_NO,0,char *)
#define IOCTL_GET_MSG  _IOR(MAJOR_NO,1,char *)
#define IOCTL_GET_NTH_BYTE _IOR(MAJOR_NO,2,int)
#define IOCTL_SET_TUNABLE _IOR(MAJOR_NO,3,struct tunable_arg*)

#endif

