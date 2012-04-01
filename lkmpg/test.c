#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

char buffer[2048];

int main(int argc, char **argv)
{
  int fin = -1;
  int count = 0;
  int ret, i;

  count = atoi(argv[1]);
  fin = open(argv[2], O_RDONLY);
  if (fin < 0) {
    fprintf(stderr, "Unable to open file %s\n", argv[2]);
    return errno;
  }

  for (i=0; i < count; i++) {
    ret = read(fin, buffer, sizeof(buffer));
    if (ret < 0) {
      fprintf(stderr, "Read failed \n");
      break;
    }
    printf("Appempt %d Read %d bytes str=%s\n", i, ret, buffer);
  }

  close(fin);
}
