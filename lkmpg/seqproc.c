#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define DRIVER_AUTHOR "Tushar R. Gosavi"
#define DRIVER_DESC    "A minimum char device driver"
MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_SUPPORTED_DEVICE("chardev");

#define procfs_name "seq"

static void *myseq_start(struct seq_file *s, loff_t *pos)
{
  static unsigned long counter = 0;
 
  printk(KERN_INFO "myseq_start called pos=%lld counter=%ld\n", *pos, counter);
  if (*pos == 0) {
    return &counter;
  } else {
    *pos = 0;
    return NULL;
  }
}

static void *myseq_next(struct seq_file *s, void *v, loff_t *pos)
{
  unsigned long *tmp_v = (unsigned long *)v;
  printk(KERN_INFO "myseq_next called temp_v = %ld\n", *tmp_v);
  (*tmp_v)++;
  (*pos) ++;
  return NULL;
}

static void myseq_stop(struct seq_file *s, void *v)
{
  /* nothing to do */
}

static int myseq_show(struct seq_file *s, void *v)
{
  loff_t *spos = (loff_t *)v;
  seq_printf(s, "%Ld", *spos);
  return 0;
}

static struct seq_operations myseq_ops = {
  .start = myseq_start,
  .next  = myseq_next,
  .stop  = myseq_stop,
  .show  = myseq_show
};

static int myseq_open(struct inode *inode, struct file *file)
{
  printk(KERN_INFO "mseq_open called \n");
  return seq_open(file, &myseq_ops);
}

static struct file_operations myseq_fops = {
  .owner = THIS_MODULE,
  .open = myseq_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = seq_release,
};

static int __init hello_init(void)
{

  struct proc_dir_entry *entry;

  entry = create_proc_entry(procfs_name, 0644, NULL);
  if (entry) {
    entry->proc_fops = &myseq_fops;
  }

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

