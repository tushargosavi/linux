#ifndef PRIVMEMLIB_H
#define PRIVMEMLIB_H

#include "privmem.h"

int create_process_area(processid_t pid);
int delete_process_area(processid_t pid);
int create_chunk(privmem_t *mem);
int delete_chunk(privmem_t *mem);
int delete_all_chunks(processid_t pid);
int flush_chunk(privmem_t *mem);
int get_chunk(privmem_t *mem);

#endif

