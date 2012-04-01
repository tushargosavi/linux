#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

static struct timer_list my_timer;

void my_timer_callback(unsigned long data)
{
  printk("my timer called called (%ld) \n", jiffies);
}

int init_module(void)
{
  int ret;
  printk("Timer module initializing\n");

  setup_timer(&my_timer, my_timer_callback, 0);

  printk("Starting timer to fire in 200ms %ld \n", jiffies);
  ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(200));
  if (ret) printk("Error in mod timer\n");
  return 0;
}

void cleanup_module(void)
{
  int ret;

  ret = del_timer(&my_timer);
  if (ret) printk("The timer is still in use \n");

  printk("Timer module uninstalling\n");

  return;
}
