#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/unistd.h>
#include <linux/kallsyms.h>
#include <linux/sched.h>
#include <asm/uaccess.h>

static void **sys_call_table = NULL;

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tushar R. Gosavi");

static int uid = 0;
static long addr = 0;

module_param(uid, int, 0644);
module_param(addr, long, 0644);

asmlinkage int (*original_call)(const char *, int, int);

asmlinkage int our_sys_open(const char *filename, int flags, int mode)
{
  int i = 0;
  char ch;

  if (uid == current_uid()) {
    printk("Opened file by %d :", uid);
    do {
      get_user(ch, filename + i);
      i++;
      printk("%c", ch);
    } while (ch != 0);
    printk("\n");
  }

  return original_call(filename, flags, mode);
}

int init_module()
{
  if (addr != 0)
    *sys_call_table = (void*)addr;

  /* *sys_call_table = (void*)kallsyms_lookup_name("sys_call_table"); */

  if (*sys_call_table == 0) {
    printk(KERN_INFO "init_module: Unable to address of sys_call_table symbol\n");
    return -1;
  }

  printk(KERN_INFO "init_module sys_call_table address is 0x%llx\n",
	 (unsigned long long)sys_call_table);
  original_call = sys_call_table[__NR_open];
  printk(KERN_INFO "Address of open function is 0x%llx\n", original_call);

  /* sys_call_table[__NR_open] = our_sys_open; */

  printk(KERN_INFO "Spying on UID %d\n", uid);
  return 0;
}

void cleanup_module()
{
  if (sys_call_table[__NR_open] != our_sys_open)
    printk(KERN_ALERT "Somebody changed the syscall table\n");

  /*  sys_call_table[__NR_open] = original_call; */
}
