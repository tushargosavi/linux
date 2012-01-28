#ifndef PRIVMEM_H
#define PRIVMEM_H

#define SUCCESS (0)

typedef unsigned int processid_t;
typedef unsigned int chunkid_t;

struct privmem {
	processid_t pid;
	chunkid_t chunk;
	ssize_t size;
	void *ptr;
};

typedef struct privmem privmem_t;

#endif

