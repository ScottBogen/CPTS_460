/********** test.c file *************/
#include "ucode.c"

char buf[1024];

int main(int argc, char *argv[ ]) {

  int i;
  int fd, n;
  int pid = getpid();

  getcwd(buf);
  printf("cwd = %s\n", buf);


  printf("argc = %d\n", argc);
  for (i=0; i<argc; i++) {
    printf("argv[%d] = %s\n", i, argv[i]);
  }

  fd = open("f1", O_RDONLY);
  printf("fd = %d\n", fd);
  if (fd < 0) {
    exit(1);
  }
  n = read(fd, buf, 1024);
  printf("n=%d buf=%s\n", n, buf);

  printf("exit\n");
}
