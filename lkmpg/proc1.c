#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define DRIVER_AUTHOR "Tushar R. Gosavi"
#define DRIVER_DESC    "A minimum char device driver"
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("chardev");

#define procfs_name "buffer2k"
#define PROCFS_MAX_SIZE 2048
static struct proc_dir_entry *proc_file = NULL;
static unsigned long procfs_buffer_size = 0;
static char procfs_buffer[PROCFS_MAX_SIZE];

static ssize_t procfs_read(struct file *filp,
                         char *buffer,
                         size_t length,
                         loff_t* offset)
{
  static int finished = 0;

  if (finished) {
    printk(KERN_INFO "procfs_read: END \n");
    finished = 0;
    return 0;
  }

  finished = 1;

  if (copy_to_user(buffer, procfs_buffer, procfs_buffer_size))
      return -EFAULT;

  printk(KERN_INFO "procfs_read read %lu bytes\n", procfs_buffer_size);
  return procfs_buffer_size;
}

static ssize_t procfs_write(struct file *file, const char *buffer,
                        size_t len, loff_t *off)
{
  procfs_buffer_size = len;
  if (procfs_buffer_size > PROCFS_MAX_SIZE) {
    procfs_buffer_size = PROCFS_MAX_SIZE;
  }
  
  if (copy_from_user(procfs_buffer, buffer, procfs_buffer_size))
    return -EFAULT;

  printk(KERN_INFO "procfs_write: write %lu bytes\n", procfs_buffer_size);
  return procfs_buffer_size;
}

static int module_permission(struct inode *inode, int op)
{
  return 0;
}

static int procfs_open(struct inode *inode, struct file *file)
{
  try_module_get(THIS_MODULE);
  return 0;
}

static int procfs_close(struct inode *inode, struct file *file)
{
  module_put(THIS_MODULE);
  return 0;
}

static struct file_operations our_procfs_fops = {
  .read = procfs_read,
  .write = procfs_write,
  .open = procfs_open,
  .release = procfs_close,
};

static struct inode_operations our_procfs_iops = {
  .permission = module_permission,
};


static int __init hello_init(void)
{

  proc_file = create_proc_entry(procfs_name, 0644, NULL);

  if (proc_file == NULL) {
    remove_proc_entry(procfs_name, NULL);
    printk(KERN_ALERT "Error unable to create /proc/%s\n", procfs_name);
    return -ENOMEM;
  }

  proc_file->proc_iops = &our_procfs_iops;
  proc_file->proc_fops = &our_procfs_fops;
  proc_file->mode = S_IFREG | S_IRUGO;
  proc_file->uid = 0;
  proc_file->gid = 0;
  proc_file->size = 80;

  printk(KERN_INFO "/proc/%s created\n", procfs_name);
  return 0;
}

static void __exit hello_exit(void)
{
  remove_proc_entry(procfs_name, NULL);
  printk(KERN_INFO "/proc/%s removed \n", procfs_name);
}

module_init(hello_init);
module_exit(hello_exit);

