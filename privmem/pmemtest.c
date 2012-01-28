#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "privmemlib.h"

/* to be filled :-) */
int main(int argc,char **argv)
{
	privmem_t pmem;
	privmem_t pmem1;

	printf("This program is only for testing of the promem implementation\n");
	if(create_process_area(1)==0) {
		printf("process area created\n");
		if(!delete_process_area(1)) 
			printf("process area deleted\n");
	}


	create_process_area(1);
	create_process_area(2);
	create_process_area(3);
	create_process_area(4);
	create_process_area(13);
	
	delete_process_area(3);
	
	delete_process_area(1);
	
	if(create_process_area(1)==0) {
		pmem.pid=1;
		pmem.chunk=1;
		pmem.size=10;
		pmem.ptr=(void*)malloc(10);
		strcpy(pmem.ptr,"tushar");
		if(create_chunk(&pmem)==0) {
			printf("chunk is created \n");
		}
		printf("getting the chunk\n");
		pmem1.pid=1;
		pmem1.chunk=1;
		pmem1.size=10;
		pmem1.ptr=(void*)malloc(10);
		if(get_chunk(&pmem1)==0) {
			printf("name : %s\n",(char*)pmem1.ptr);
		}
		delete_process_area(1);		
	}
	return 0;
}


