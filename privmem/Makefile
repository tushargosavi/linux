TEST_OBJS=pmemtest.o privmemlib.o kernel_privmem.o misc.o
test : pmemtest
pmemtest : $(TEST_OBJS)
	gcc -o pmemtest $(TEST_OBJS)
pmemtest.o : pmemtest.c 
	gcc $(CCFLAGS) -c pmemtest.c
privmemlib.o : privmemlib.c privmemlib.h
	gcc $(CCFLAGS) -c privmemlib.c
kernel_privmem.o : kernel_privmem.c kernel_privmem.h misc.h
	gcc $(CCFLAGS) -c kernel_privmem.c
misc.o : misc.c misc.h
	gcc $(CCFLAGS) -c misc.c

kernel :
	make -f kernel.mk

clean :
	rm -f pmemtest *.o

debug : 
	make CCFLAGS=-g3 
