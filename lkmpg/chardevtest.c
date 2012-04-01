#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

#include "chardev.h"

char buf[2048];

int main(int argc, char **argv)
{
  int fin;
  int i;
  char ch = 'a';

  fin = open(argv[1], O_RDONLY);
  if (fin < 0)
    return errno;

  ioctl(fin, IOCTL_SET_MSG, "This is a new msg\n");
  ioctl(fin, IOCTL_GET_MSG, buf);
  printf("The message is : %s\n", buf);

  printf("\n");
  i = 0;
  while (ch != 0) {
    ch = ioctl(fin, IOCTL_GET_NTH_BYTE, i++);
    printf("%c", ch);
  }

  printf("\n");
  close(fin);
  return 0;
}

