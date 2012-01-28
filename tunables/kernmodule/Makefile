obj-m += tunmod.o
tunmod-objs := tunmod.o tunables.o

all :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean :
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

	
