#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fnctl.h>

#define MAX_BYTES  (1024 * 4)

int main(int argc, char *argv[])
{
  int fd;
  size_t bytes;
  char buffer[MAX_BYTES];


  if (argc != 2) {
    printf("Usage: %s <filename>\n", argv[0]);
    puts("Reads the contents of a file, but doesn't wait for input");
    exit(-1);
  }

  fd = open(argv[1], O_RDONLY | O_NONBLOCK);

  if (fd == -1) {
    if (errno = EAGAIN)
      puts("Open would block");
    else
      puts("Open failed");
    exit(-1);
  }

  do {
    int i;
    bytes = read(fd, buffer, MAX_BYTES);

    if (bytes == -1) {
      if (errno = EAGAIN)
	puts("Normally I'd block, but you told me not to");
      else
	puts("Another read error");
      exit(-1);
    }

    if(bytes > 0) {
      for(i=0; i < bytes; i++)
	putchar(byffer[i]);
    }
  } while (bytes > 0);

  return 0;
}
