#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "kernel_privmem.h"
#include "misc.h"

/* just return 0 before we implement actual hashing :-) */
#define PROCESS_HASH(a) (a%PROCESS_HASH_SIZE)
#define CHUNK_HASH(a) (a%CHUNK_HASH_SIZE)

pmem_process_area_t *search_process_area(processid_t pid);
pmem_chunk_t *search_chunk(pmem_process_area_t *parea,chunkid_t chunk);
void clear_chunk_hash_table(pmem_chunk_t **chash);
void clear_process_hash_table(pmem_process_area_t **phash);

/* freeing functions :-) */
void free_chunk(pmem_chunk_t *chunk);
void kernel_delete_all_chunks(pmem_chunk_t *chunks);
void free_process_area(pmem_process_area_t *parea);
void delete_all_processes(pmem_process_area_t *plist);

pmem_process_area_t *pmem_process_list=NULL;
pmem_process_area_t *process_hash[PROCESS_HASH_SIZE];
static int no_of_processes=0;

int do_create_process_area(processid_t pid)
{
	pmem_process_area_t *parea=0;
	int hash=0;

	if((parea=search_process_area(pid))!=0) {
		DBG_MSG("process area already exists \n");
		return -EBUSY;
	}
	/* allocate a new process area */	
	parea=(pmem_process_area_t*)MEM_ZALLOC(sizeof(pmem_process_area_t));
	if(parea==0) return -ENOMEM;

	parea->pid=pid;
	parea->no_of_chunks=0;
	
	/* update the link list */
	if(pmem_process_list==0) {
		/* first process area */
		pmem_process_list=parea;
	}else {	
		parea->next=pmem_process_list;
		pmem_process_list->prev=parea;
		pmem_process_list=parea;
	}

	/* update the hash for faster access */
	hash=PROCESS_HASH(pid);
	if(process_hash[hash]!=0) {
		/* somebody have the same hash , chain it :-) */
		parea->hnext=process_hash[hash];
		process_hash[hash]->hprev=parea;
		process_hash[hash]=parea;
	}else {
		process_hash[hash]=parea;
	}
	return SUCCESS;
}

int do_delete_process_area(processid_t pid)
{
	pmem_process_area_t *parea;
	pmem_chunk_t *chunk;
	int hash;
	
	parea=search_process_area(pid);
	if(parea==0) {
		DBG_MSG("Process area does not exists\n");
		return -ENOENT;
	}

	/* delete from global process list */
	if(parea->prev==0) {
		pmem_process_list=parea->next;
		if(pmem_process_list) pmem_process_list->prev=0; 
	}else {
		parea->prev->next=parea->next;
		if(parea->next) parea->next->prev=parea->prev;
	}

	/* delete from hash */
	hash=PROCESS_HASH(pid);
	if(process_hash[hash]->hprev==0) {
		/*first in hash list */
		process_hash[hash]=process_hash[hash]->hnext;
		if(process_hash[hash]) process_hash[hash]->hprev=0;
	}else {
		process_hash[hash]->hprev->hnext=process_hash[hash]->hnext;
		if(process_hash[hash]->hnext) process_hash[hash]->hnext->hprev=process_hash[hash]->hprev;
	}
	
	/* free the memory for the chunk */
	free_process_area(parea);

}

int do_create_chunk(privmem_t *mem) {
	pmem_process_area_t *parea;
	pmem_chunk_t *chunk;
	int hash=0;

	if((parea=search_process_area(mem->pid))==0)
	{
		DBG_MSG("No such process area\n");
		return -ENOENT;
	}
	
	chunk=(pmem_chunk_t*)MEM_ZALLOC(sizeof(pmem_chunk_t));
	if(chunk==NULL) {
		DBG_MSG("Error while allocating memory\n");
		return -ENOMEM;
	} 
	
	chunk->id=mem->chunk;
	chunk->size=mem->size;
	/* allocate memory for the chunk */
	chunk->ptr=MEM_ZALLOC(chunk->size);	
	if(chunk->ptr==0) {
		DBG_MSG("Unable to allocate memory \n");
		/* delete the chunk allocated */
		MEM_FREE(chunk);
		return -ENOMEM;
	}
	/* copy data while creating chunk */
	if(mem->ptr!=0) {
		/* we have provided some data so add it */
		MEM_COPY(chunk->ptr,mem->ptr,chunk->size);
	}
		
	
	/* update the link list */
	if(parea->chunks==0) {
		parea->chunks=chunk;	
	}else {
		chunk->next=parea->chunks;
		parea->chunks->prev=chunk;
		parea->chunks=chunk;
	}

	/* updatae the chuck hash */
	hash=CHUNK_HASH(mem->chunk);
	if(parea->chunks_hash[hash]==0) {
		parea->chunks_hash[hash]=chunk;
	}else {
		/* some chunks are already present so chain it */
		chunk->hnext=parea->chunks_hash[hash];
		parea->chunks_hash[hash]->hprev=chunk;
		parea->chunks_hash[hash]=chunk;
	}

	return SUCCESS;	
}

int do_delete_chunk(privmem_t *mem) {
	pmem_process_area_t *parea;
	pmem_chunk_t *chunk;
	int hash;
	
	parea=search_process_area(mem->pid);
	if(parea==0) {
		DBG_MSG("Process area does not exists\n");
		return -ENOENT;
	}
	/* seach the chunk in the process */ 
	chunk=search_chunk(parea,mem->chunk);
	if(chunk==0) {
		DBG_MSG("The chunk not found\n");	
		return -ENOENT;
	}

	/* delete from linked list */
	if(chunk->prev==0) {
		/* chunk at the start of the list */
		parea->chunks=chunk->next;
		if(parea->chunks) parea->chunks->prev=0;
	}else {
		chunk->prev->next=chunk->next;
		chunk->next->prev=chunk->prev;
	}

	/* delete from hash */
	hash=CHUNK_HASH(chunk->id);
	if(chunk->hprev==0) {
		/*first in hash list */
		parea->chunks_hash[hash]=chunk->hnext;
		parea->chunks_hash[hash]->hprev=0;	
	}else {
		chunk->hprev->hnext=chunk->hnext;
		chunk->hnext->hprev=chunk->hprev;
	}
	
	/* free the memory for the chunk */
	free_chunk(chunk);
}


int do_delete_all_chunks(processid_t pid) {
	pmem_process_area_t *parea;
	pmem_chunk_t *chunk,*next_chunk;

	parea=search_process_area(pid);
	if(parea==0) {
		DBG_MSG("process area can not be found \n");
		return -ENOENT;
	}

	kernel_delete_all_chunks(parea->chunks);
	
	parea->chunks=NULL;
	/* clear the hash table */
	clear_chunk_hash_table(parea->chunks_hash);
	return SUCCESS;	
}

int do_flush_chunk(privmem_t *mem) 
{
	pmem_process_area_t *parea;
	pmem_chunk_t *chunk;
	parea=search_process_area(mem->pid);
	if(parea==0) {
		DBG_MSG("process area can not be found\n");
		return -ENOENT;
	}
	chunk=search_chunk(parea,mem->chunk);
	if(chunk==0) {
		DBG_MSG("chunk can not be found\n");
		return -ENOENT;
	}
	MEM_COPY(chunk->ptr,mem->ptr,chunk->size);
}

int do_get_chunk(privmem_t *mem) 
{
	pmem_process_area_t *parea;
	pmem_chunk_t *chunk;
	parea=search_process_area(mem->pid);
	if(parea==0) {
		DBG_MSG("process area can not be found\n");
		return -ENOENT;
	}
	chunk=search_chunk(parea,mem->chunk);
	if(chunk==0) {
		DBG_MSG("chunk can not be found\n");
		return -ENOENT;
	}
	MEM_COPY(mem->ptr,chunk->ptr,chunk->size);
	return SUCCESS;
}


pmem_process_area_t *search_process_area(processid_t pid)
{
	int hash=PROCESS_HASH(pid);
	pmem_process_area_t *parea=NULL;	
	parea=process_hash[hash]; 
	while(parea) {
		if(parea->pid==pid) 
			break;
		else parea=parea->hnext;
	}
	return parea;	
}

pmem_chunk_t *search_chunk(pmem_process_area_t *parea,chunkid_t chunkid)
{
	int hash=CHUNK_HASH(chunkid);
	pmem_chunk_t *chunk=NULL;	
	chunk=parea->chunks_hash[hash];
	while(chunk) {
		if(chunk->id==chunkid) 
			break;
		else chunk=chunk->hnext;
	}
	return chunk;	
}


/* 
	For clearing the hash table just clear the entries
	the caller will free the memory from link list  
*/
void clear_chunk_hash_table(pmem_chunk_t **chash)
{
	MEM_ZERO(chash,sizeof(pmem_chunk_t*)*CHUNK_HASH_SIZE);
}

void clear_process_hash_table(pmem_process_area_t **phash)
{
	MEM_ZERO(phash,sizeof(pmem_process_area_t*)*PROCESS_HASH_SIZE);
}


void free_chunk(pmem_chunk_t *chunk)
{
	DBG_MSG("freeing chunk\n");
	MEM_FREE(chunk);
}

void kernel_delete_all_chunks(pmem_chunk_t *chunks)
{
	pmem_chunk_t *chunk,*next_chunk;
	for(chunk=chunks;chunk;chunk=next_chunk) {
		next_chunk=chunk->next;
		free_chunk(chunk);
	}
}

void free_process_area(pmem_process_area_t *parea) {
	/* We need to the free the chunks which are there in the process area */
	kernel_delete_all_chunks(parea->chunks);
	MEM_FREE(parea);
}

void delete_all_processes(pmem_process_area_t *plist)
{
	pmem_process_area_t *next_process,*parea;
	for(parea=plist;parea;parea=next_process) {
		next_process=parea->next;
		free_process_area(parea);
	}
}

void kernel_init_privmem()
{
	clear_process_hash_table(process_hash);
	no_of_processes=0;
	pmem_process_list=0;
}

void kernel_remove_privmem()
{
	delete_all_processes(pmem_process_list);
}

