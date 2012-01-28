tunable : main.o tunables.o
	gcc -g main.o tunables.o -o tunable

main.o : main.c 
	gcc -g -c main.c

tunables.o : tunables.c 
	gcc -g -c tunables.c

clean :
	rm *.o tunable

