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

#define procfs_name "hello"
#define PROCFS_MAX_SIZE 1024
static struct proc_dir_entry *proc_file = NULL;
static int procfs_buffer_size = 0;
static char procfs_buffer[PROCFS_MAX_SIZE];

static int procfile_read(char *buffer, char **buffer_location,
                         off_t offset, int buffer_len,
                         int *eof, void *data)
{
  int ret;
  printk(KERN_INFO "procfile_read /proc/%s called \n", procfs_name);
  
  if (offset > 0) {
    ret = 0;
  } else {
    memcpy(buffer, procfs_buffer, procfs_buffer_size);
    ret = procfs_buffer_size;
  }
  return ret;
}

static int procfile_write(struct file *file, const char *buffer, unsigned long count)
{
  procfs_buffer_size = count;
  if (procfs_buffer_size > PROCFS_MAX_SIZE) {
    procfs_buffer_size = PROCFS_MAX_SIZE;
  }
  
  if (copy_from_user(procfs_buffer, buffer, procfs_buffer_size)) {
    return -EFAULT;
  }
  return procfs_buffer_size;
}

static int __init hello_init(void)
{

  proc_file = create_proc_entry(procfs_name, 0644, NULL);

  if (proc_file == NULL) {
    remove_proc_entry(procfs_name, NULL);
    printk(KERN_ALERT "Error unable to create /proc/%s\n", procfs_name);
    return -ENOMEM;
  }

  proc_file->read_proc = procfile_read;
  proc_file->write_proc = procfile_write;
  proc_file->mode = S_IFREG | S_IRUGO;
  proc_file->uid = 0;
  proc_file->gid = 0;
  proc_file->size = 37;

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


