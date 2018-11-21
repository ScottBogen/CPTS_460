#include "ucode.c"

char cwd[32];
char buf2[4096];

// example call: ls /bin
int main(int argc, char *argv[ ]) {

  struct stat* sbuf;

  getcwd(cwd);
  printf("cwd = %s\n", cwd);
  int fd;

  if (argc > 1) {
    fd = open(argv[1], O_RDONLY);
  }
  else {
    fd = open("f1", O_RDONLY);
  }

  if (fd < 0) {
    printf("File not opened successfully\n");
    close(fd);
    return 0;
  }

  int n = read(fd, buf2, 4096);
  printf("n=%d\n", n);

  prints("trying to stat...\n");
  stat("f1", sbuf);

  printf("ssize=%d\n", sbuf->st_size);
  prints("statted fine \n\n\r");

  close(fd);
}
