#include <stdio.h>
#include <stdlib.h>

#include "kernel_privmem.h"

int create_process_area(processid_t pid)
{
	return do_create_process_area(pid);
}

int delete_process_area(processid_t pid)
{
	return do_delete_process_area(pid);
}

int create_chunk(privmem_t *mem)
{
	return do_create_chunk(mem);
}

int delete_chunk(privmem_t *mem)
{
	return do_delete_chunk(mem);
}

int delete_all_chunks(processid_t pid)
{
	return do_delete_all_chunks(pid);
}

int flush_chunk(privmem_t *mem)
{
	return do_flush_chunk(mem);
}

int get_chunk(privmem_t *mem)
{
	return do_get_chunk(mem);
}

