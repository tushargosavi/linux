obj-m += hello.o
obj-m += chardrv.o
obj-m += procfs.o
obj-m += proc1.o
obj-m += seqproc.o
obj-m += syscallhack.o
obj-m += blocking.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

