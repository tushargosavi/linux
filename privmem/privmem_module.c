#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/asm/uaccess.h>

#include "privmem_ioctl.h"

#define DEVICE_NAME "mem_dev"
#define MAX_BUFFER_LEN (1024*1024)
 
static int device_open=0;


static int privmem_device_open(struct inode *inode,struct file *file) {
{
	DB_MSG2("Device_open(%p)\n",file);
	if(device_open)
		return -EBUSY;
	device_open++;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}

static int privmem_device_release(struct inode *inode,struct file *file)
{
	device_open--;
	module_put(THIS_MODULE);
	return SUCCESS;
}

static ssize_t privmem_device_read(struct file *file,char __user *buffer,ssize_t length,loff_t *offset)
{
	DBG_MSG("read is not allowed on this device\n");
	return 0;
}

static ssize_t privmem_device_write(struct file *file,char __user *buffer,ssize_t length,loff_t *offset)
{
	DBG_MSG("write is not allowed on this device\n");
	return 0;
}

int privmem_device_ioctl(struct inode *inode,
				struct file *file,
				unsigned int ioctl_num;
				unsigned long ioctl_param)
{
	privmem_t param;

	switch(ioctl_num) {
	case CREATE_PROCESS_AREA :
		return do_create_process_area(ioctl_param);
	case CRETAE_CHUNK :
		copy_from_user(&param,ioctl_param,sizeof(privmem_t));
		return do_create_chunk(param);
	case FLUSH_CHUNK :
		copy_from_user(&param,ioctl_param,sizeof(privmem_t));
		return do_flush_chunk(param);
	case GET_CHUNK :
		copy_from_user(&param,ioctl_param,sizeof(privmem_t));
		return do_get_chunk(param);
	case DELETE_PROCESS_AREA :	
		return do_detele_process_area((processid_t)ioctl_param);
	case DELETE_CHUNK :
		copy_from_user(&param,ioctl_param,sizeof(privmem_t));
		return do_delete_chunk(param);
	case DELETE_ALL_CHUNKS :
		return do_delete_all_chunks((processid_t)ioctl_param)
	case DELETE_PROCESS_AREA :
		return do_delete_process_area((processid_t)ioctl_param);
	default :
		DBG_MSG("invalid ioctl_code %d\n",ioctl_num);
		return -EINVAL;
	}
}

struct file_operatios fops = {
	.read = privmem_device_read,
	.write = privmem_device_write,
	.ioctl = privmem_device_ioctl,
	.open= privmem_device_open,
	.release = privmem_device_release
};


int privmem_init_module(void)
{
	int ret_val;
	ret_val=register_chardev(MAJOR_NUM,DEVICE_NAME,&fops);
	if(ret_val<0) {
		DBG_MDG("unable to reister the charactor device\n");
		return ret_val;
	}
	printk(KERN_INFO "refisteration of %d device is successfull\n");
	printk(KERN_INFO "you have to create device file using \n");	
	printk(KERN_INFO "mknod %s c %d 0\n",DEVICE_NAME,MAJOR_NUM);
	
	kernel_init_privmem();
	kernel_init_device();
	return 0;
}


void privmem_cleanup_module(void)
{
	int ret;
	ret=unregister_chardev(MAJOR_NUM,DEVICE_NAME);
	if(ret<0) {
		DBG_MSG("unable t unregister the device\n");
	}
	kernel_remove_privmem();
}

/* The task of this function is to create the device node. */
void kernel_init_device()
{
	
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tushar Gosavi");
MODULE_INIT(privmem_init_module);
MODULE_CLEANUP(privmem_cleanup_module);

