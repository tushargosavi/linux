obj-m += privmem.o
privmem-objs := privmem_module.o kernel_privmem.o 

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

