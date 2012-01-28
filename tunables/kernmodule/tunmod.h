#ifndef TUNABLES_H
#define TUNABLES_H

#include <linux/ioctl.h>

#include "tunmod_ioctl.h"

#define DEVICE_NAME  "tune"

#define SUCCESS 0
#define BUF_LEN 80


int tunmod_init(void);
void tunmod_clean(void);
static int tunmod_open(struct inode *,struct file *);
static int tunmod_release(struct inode *,struct file *);
static ssize_t tunmod_read(struct file *,char *,size_t,loff_t *);
static ssize_t tunmod_write(struct file*,const char *,size_t,loff_t *);
static int tunmod_ioctl(struct inode *,struct file *,unsigned int ,unsigned long ioctl_param);

#endif
