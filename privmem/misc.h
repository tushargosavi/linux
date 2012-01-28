#ifndef MISC_H
#define MISC_H

#ifdef _KERNEL_
#include <stdlib.h>
#include <string.h>

#else

#include <linux/kernel.h>
#include <linux/module.h>

#endif

#ifndef _KERNEL_
/* user land functions */
#define MEM_COPY(dest,src,n) memcpy(dest,src,n)
#define DBG_MSG(a) printf(a)
#define MEM_ZERO(a,size) memset(a,0,size)
#define MEM_ZALLOC(size) zmemalloc(size)
#define MEM_FREE(ptr) free(ptr)
#define MEM_ALLOC(size) malloc(size)


#else

/* kernel land functions */
#define MEM_COPY(dest,src,n) memcpy(dest,src,n)
#define DBG_MSG(a) printk(a)
#define MEM_ZERO(a,size) memset(a,0,size)
#define MEM_ZALLOC(size) zmemalloc(size)
#define MEM_FREE(ptr) free(ptr)
#define MEM_ALLOC(size) malloc(size)

#endif

void *zmemalloc(unsigned long size);

#endif

