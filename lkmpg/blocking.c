#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/cred.h>
#include <linux/version.h>

#define MESSAGE_LENEGHT  80

static char msg[MESSAGE_LENEGHT];

static struct proc_dir_entry *proc_file = NULL;
#define PROC_ENTRY_FILENAME  "sleep"

static ssize_t module_output(struct file *file,
			     char *buf,
			     size_t len,
			     loff_t *offset)
{
  static int finished = 0;
  int i;
  char message[MESSAGE_LENEGHT + 30 ];

  if (finished) {
    finished = 0;
    return 0;
  }

  sprintf(message , "Last Input :%s\n", msg);
  for (i = 0; i < len && msg[i]; i++)
      put_user(msg[i], buf+i);

  finished = 1;
  return i;


static ssize_t module_input(struct file *file,
			    const char *buf,
			    size_t length,
			    loff_t *offset)
{
  int i;

  for (i = 0; i < MESSAGE_LENEGHT -1 && i < length; i++)
      get_user(msg[i], buf + i);

  msg[i] = '\0';
  return i;
}

int already_opened = 0;

DECLARE_WAIT_QUEUE_HEAD(WaitQ);

static int module_open(struct inode *inode, struct file *file)
{
  if ((file->f_flags & O_NONBLOCK) && already_opened)
    return -EAGAIN;

  try_module_get(THIS_MODULE);

  while (already_opened) {
    int i, is_sig = 0;

    wait_event_interruptible(WaitQ, !already_opened);

    for (i=0; i < _NSIG_WORDS && !is_sig; i++)
      is_sig = current->pending.signal.sig[i] & ~current->blocked.sig[i];

    if (is_sig) {
      module_put(THIS_MODULE);
      return -EINTR;
    }
  }

  already_opened = 1;
  return 0;
}

int module_close(struct inode *inode, struct file *file)
{
  already_opened = 0;
  wake_up(&WaitQ);
  module_put(THIS_MODULE);
  return 0;
}

static int module_permissions(struct inode *inode, int op
#if ( LINUX_VERSION_CODE >= KERNEL_VERSION(3, 0 ,0) ) 
			      ,unsigned int nd
#endif
	                     )
{
  if ((op == 4 || op == 2) && (current_uid() == 0))
    return 0;

  return -EACCES;
}

static struct file_operations proc_file_ops = {
  .read = module_output,
  .write = module_input,
  .open = module_open,
  .release = module_close
};

static struct inode_operations proc_inode_ops = {
  .permission = module_permissions,
};

int init_module()
{
  proc_file = create_proc_entry(PROC_ENTRY_FILENAME, 0644, NULL);
  if (proc_file == NULL) {
    remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
    return -ENOMEM;
  }

  proc_file->proc_iops = &proc_inode_ops;
  proc_file->proc_fops = &proc_file_ops;
  proc_file->mode = S_IFREG | S_IRUGO | S_IWUSR;
  proc_file->uid = 0;
  proc_file->gid = 0;
  proc_file->size = 80;

  printk(KERN_INFO "%s cretaed\n", PROC_ENTRY_FILENAME);
  return 0;
}

void cleanup_module()
{
  remove_proc_entry(PROC_ENTRY_FILENAME, NULL);
  printk(KERN_INFO "%s removed\n", PROC_ENTRY_FILENAME);
}

