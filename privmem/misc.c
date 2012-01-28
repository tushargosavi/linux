#include <stdio.h>
#include "misc.h"

void *zmemalloc(unsigned long size)
{
	void *ptr;
	ptr=malloc(size);
	if(!ptr) return ptr;
	memset(ptr,0,size);
	return ptr;
}

