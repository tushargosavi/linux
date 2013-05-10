#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "chardev.h"

#define DRIVER_AUTHOR "Tushar R. Gosavi"
#define DRIVER_DESC    "A minimum char device driver"
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("chardev");

static char *dirpath = NULL;
module_param(dirpath, charp, 0000);
MODULE_PARM_DESC(dirpath, "A directory to monitor");

#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUF_LEN 1024

static int Major;
static int opened = 0;

static char msg[BUF_LEN];
static char *msgPtr;
static ssize_t device_read(struct file*, char *, size_t, loff_t *);
static ssize_t device_write(struct file*, const char __user*, size_t, loff_t *);
static int device_open(struct inode*, struct file*);
static int device_release(struct inode *, struct file *);
static long device_ioctl(struct file *, unsigned int, unsigned long);

static struct file_operations fops = {
  .read = device_read,
  .write = device_write,
  .unlocked_ioctl = device_ioctl,
  .open = device_open,
  .release = device_release
};

static int __init hello_init(void)
{
  int ret_val;

  ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
  
  if (ret_val < 0) {
    printk(KERN_ALERT "Registration of char device driver failed\n");
    return Major;
  }

  printk(KERN_INFO "I was assisgned major number %d \n", MAJOR_NUM);
  printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, MAJOR_NUM);

  return SUCCESS;
}

static void __exit hello_exit(void)
{
  unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
  
  /* if (ret < 0)  */
  /*   printk(KERN_ALERT "Error in unregister of chardev %d\n", Major); */
 
  printk(KERN_INFO "Removing driver");
}

static int device_open(struct inode *inode, struct file *file)
{
  static int counter = 0;
  if (opened)
    return -EBUSY;

  opened ++;
  sprintf(msg, "I already told you %d times Hello World\n", counter++);
  msgPtr = msg;
  try_module_get(THIS_MODULE);
  return SUCCESS;
}


static int device_release(struct inode *inode, struct file *file)
{
  opened--;
  module_put(THIS_MODULE);
  return SUCCESS;
}

static ssize_t device_read(struct file *filp,
                       char *buffer, size_t len, loff_t *offset)
{
  int bytes_read = 0;

  if (*msgPtr == 0)
    return 0;

  while (len && *msgPtr) {
    put_user(*(msgPtr++), buffer++);
    len --;
    bytes_read++;
  }

  return bytes_read;
}

static ssize_t device_write(struct file *filp,
			    const char __user *buff,
			    size_t len,
			    loff_t *off)
{
  int i;

  for (i = 0; i < len && i < BUF_LEN; i++)
    get_user(msg[i], buff + i);

  msgPtr =  msg;

  return i;
}

static long device_ioctl(struct file *file,
			unsigned int ioctl_num,
			unsigned long ioctl_param)
{
  int i;
  char *temp;
  char ch;

  switch (ioctl_num) {

  case IOCTL_SET_MSG :
    printk(KERN_INFO "ioctl : Setting a new msg\n");
    /* Receive a pointer to a msg (in user space) */
    temp = (char*)ioctl_param;
    get_user(ch, temp);
    for (i = 0; ch && i < BUF_LEN; i++, temp++)
      get_user(ch, temp);

    device_write(file, (char*)ioctl_param, i, 0);
    break;

  case IOCTL_GET_MSG :
    printk(KERN_INFO "ioctl : returning the information %s\n", msg);
    i = device_read(file, (char*) ioctl_param, 99, 0);
    put_user('\0', (char*) ioctl_param+i);
    break;

  case IOCTL_GET_NTH_BYTE :
    printk(KERN_INFO "ioctl : returning %c\n", msg[ioctl_param]);
    return msg[ioctl_param];
    break;
  }
  return SUCCESS;
}

module_init(hello_init);
module_exit(hello_exit);


