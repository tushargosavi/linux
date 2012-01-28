#ifndef KERNEL_PRIVMEM_H
#define KERNEL_PRIVMEM_H

#include "privmem.h"

#define PROCESS_HASH_SIZE  10
#define CHUNK_HASH_SIZE    10

struct pmem_chunk {
	struct pmem_chunk *next; /* link list next pointer */
	struct pmem_chunk *prev; /* link list prev pointer */
	struct pmem_chunk *hnext; /* hash next pointer */
	struct pmem_chunk *hprev; /* hash prev pointer */
	chunkid_t id;	/* id of the chunk */
	ssize_t size;	/* size in bytes */
	void *ptr;	/* pointer to memory */
};


struct pmem_process_area {
	struct pmem_process_area *next; /* link list next pointer */
	struct pmem_process_area *prev; /* link list prev pointer */
	struct pmem_process_area *hnext; /* hash next pointer */
	struct pmem_process_area *hprev; /* hash prev pointer */
	int pid;						/* id of the process */
	int no_of_chunks;				 /* no of chunks for the process */
	struct pmem_chunk *chunks;			/* link list of chunks in the process*/	
	struct pmem_chunk *chunks_hash[CHUNK_HASH_SIZE];  /* hash table for the chunks */
};


typedef struct pmem_process_area pmem_process_area_t;
typedef struct pmem_chunk pmem_chunk_t;

int do_create_process_area(processid_t pid);
int do_delete_process_area(processid_t pid);
int do_create_chunk(privmem_t *mem);
int do_delete_chunk(privmem_t *mem);
int do_delete_all_chunks(processid_t pid);
int do_flush_chunk(privmem_t *mem);
int do_get_chunk(privmem_t *mem);

#endif

