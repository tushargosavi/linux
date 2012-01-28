#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>

#define DRIVER_AUTHOR "Tushar R. Gosavi"
#define DRIVER_DESC    "A minimum fs driver"

static char *dirpath = NULL;
module_param(dirpath, charp, 0000);
MODULE_PARM_DESC(dirpath, "A directory to monitor");

static int __init hello_init(void)
{
	printk(KERN_INFO "Dir monitor ver 0.0001\n");
    printk(KERN_INFO "Dir monitor path is %s\n",
          dirpath? dirpath : "Not provided");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodby dirmonitor.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);

MODULE_SUPPORTED_DEVICE("testdevice");

