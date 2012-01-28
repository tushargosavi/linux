#ifndef PRIVMEM_IOCTL_H
#define PRIVMEM_IOCTL_H

#include "privmem.h"

#define CREATE_PROCESS_AREA _IOR(MAJOR_NUMBER,0,processid_t);
#define DELETE_PROCESS_AREA _IOR(MAJOR_NUMBER,1,processid_t);
#define CREATE_CHUNK _IOR(MAJOR_NUMBER,2,privmem_t);
#define DELETE_CHUNK _IOR(MAJOR_NUMBER,3,privmem_t);
#define DELETE_ALL_CHUNKS _IOR(MAJOR_NUMBER,4,processid_t);
#define FLUSH_CHUNK _IOR(MAJOR_NUMBER,5,privmem_t);
#define FLUSH_CHUNK _IOR(MAJOR_NUMBER,6,privmem_t);

#endif

