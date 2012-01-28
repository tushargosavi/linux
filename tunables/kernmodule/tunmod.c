#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#include "tunmod.h"
#include "tunables.h"

static int major;
static int device_open=0;
static char msg[BUF_LEN];
static char *msg_ptr;


static struct file_operations fops = {
	.read = tunmod_read,
	.write = tunmod_write,
	.open = tunmod_open,
	.release = tunmod_release,
	.ioctl = tunmod_ioctl
};

int tunmod_init()
{
	major=register_chrdev(MAJOR_NO,DEVICE_NAME,&fops);
	if(major<0) {
		printk(KERN_ALERT "Registration of module %s failed\n",DEVICE_NAME);
		return major;
	}
	printk(KERN_INFO "The %s assigned a major number %d",DEVICE_NAME,major);
	printk(KERN_INFO "Create a device file with mknode /dev/%s c %d 0\n",DEVICE_NAME,major);
	printk(KERN_INFO "Remove the module when done\n"); 
	return SUCCESS;
}

void tunmod_clean() 
{
	int ret=unregister_chrdev(major,DEVICE_NAME);
	if(ret<0) {
		printk(KERN_ALERT "Error while unloading module %s\n",DEVICE_NAME);
	}
}


module_init(tunmod_init);
module_exit(tunmod_clean);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tushar Gosavi");
MODULE_DESCRIPTION("Simple Tunable Driver");


static int tunmod_open(struct inode *inode,struct file *file)
{
	static int counter;
	if(device_open) {
		printk(KERN_INFO "Deive /dev/%s is already opened\n",DEVICE_NAME);
		return -EBUSY;
	}
	
	device_open++;
	sprintf(msg,"I already told you %d times Hello World\n",counter++);
	msg_ptr=msg;
	try_module_get(THIS_MODULE);
	return SUCCESS;
}


static int tunmod_release(struct inode *inode,struct file *file) 
{
	device_open--;
	module_put(THIS_MODULE);
	return SUCCESS;
}


static ssize_t tunmod_read(struct file *filep,char *buffer,size_t length,loff_t *offset)
{
	int byte_read=0;

	if(msg_ptr==0) return 0; 
	while(length && *msg_ptr) {
		printk(KERN_INFO "%c",*msg_ptr);
		put_user(*(msg_ptr++),buffer++);
		length--;
		byte_read++;
	}
	printk(KERN_INFO "\n");
	return byte_read;
}


static ssize_t tunmod_write(struct file* filep,const char *buffer,size_t length,loff_t *offset) 
{
	int bytes_to_write;
	int bytes_written=0;
	bytes_to_write=( length > (BUF_LEN-1))? (BUF_LEN-1) : length;
	if(bytes_to_write==0) return 0;
	
	msg_ptr=msg;
	while(bytes_to_write--) { 
		get_user(*(msg_ptr++),buffer++);
		bytes_written++;
	}
	*msg_ptr=0;
	printk(KERN_INFO "The data writen to the device %s\n",msg);
	return bytes_written;
}


static int tunmod_ioctl(struct inode *inode,struct file *file,unsigned int ioctl_num,unsigned long ioctl_param) 
{
	char *temp;
	char ch;
	int len;
	struct tunable_arg *tunarg=0;

	switch(ioctl_num) {
	case IOCTL_GET_MSG:
		printk(KERN_INFO "Ioctl IOCTL_GET_MSG command received\n");
		len=tunmod_read(file,(char*)ioctl_param,BUF_LEN-1,0);
		put_user('\0',(char*)ioctl_param +len );
		break;


	case IOCTL_SET_MSG :
		printk(KERN_INFO "Ioctl IOCTL_SET_MSG command received\n");
		temp=(char*)ioctl_param;

		len=0;
		get_user(ch,temp++);
		while(ch && len < BUF_LEN) {
			get_user(ch,temp++);
			len++;
		}
		
		tunmod_write(file,(char*)ioctl_param,len,0);
		break;


	case IOCTL_GET_NTH_BYTE :
		printk(KERN_INFO "Ioctl IOCTL_GET_NTH_BYTE command received\n");
		if((int)ioctl_param < BUF_LEN) 
			return msg_ptr[ioctl_param];
		return -EACCES;
		break;

	case IOCTL_SET_TUNABLE :
		printk(KERN_INFO "Ioctl IOCTL_SET_TUNABLE command received\n");
		tunarg=copy_tunable_arg((struct tunable_arg*)ioctl_param);
		if(tunarg==NULL) return -ENOMEM;
		return set_tunable(tunarg->name,tunarg->type,tunearg->value,tunarg->size);	
		break;

	default :
		printk(KERN_INFO "Invalid IOCTL command\n");
		return -EINVAL;
	}
	return SUCCESS;	
}



/* 
   This function will copy the argument required for set_tunable from user space 
   except the value* parameter 
   The set_tunable function will get the value from user space if required
*/
struct tunable_arg *copy_tunable_arg(struct tunable_arg *ptr)
{
	struct tunable_arg *tune=0;
	char *temp_ptr=0;
	char ch;
	tune=(struct tunable_arg*)kmalloc(sizeof(struct tunable));
	if(tune==NULL) {
		printk("Unable to allocate memory for tunable\n");
		return NULL;
	}
	copy_from_usr(tune,ptr,sizeof(struct tunable_arg));
	temp_ptr=tune->name;
	get_user(ch,temp_ptr);
	while(ch && (len < MAX_NAME_LEN)) { get_user(ch,temp_ptr); len++; }
	temp_ptr=tune->name;
	tune->name=(char *)kmalloc(len+1);
	copy_from_user(tune->name,temp_ptr,len);
	*(tune->name+len)=0;
	printk("Request received for tunable %s",tune->name);
	return tune;
};

