#include "ucode.c"

char cwd[32];
char buf2[1024];

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
    fd = open(cwd, O_RDONLY);
  }

  if (fd < 0) {
    printf("File not opened successfully\n");
    close(fd);
    return 0;
  }

  prints("READING dir:\n\r");
  //int n = read(fd, buf2, 1024);
  int n;

  n = read(fd, buf2, 1024);
  char* cp = buf2;
  DIR* dp = (DIR*)cp;

  while (cp < buf2 + 1024) {
    printf("dp=%s\n\r", dp->name);
    cp += dp->rec_len;
    dp = (DIR*)cp;
  }


  /*
  while (n = read(fd, buf2, sizeof(DIR)) > 0) {
    stat("/", sbuf);
    printf("ssize=%d\n\r", sbuf->st_size);

    prints("loop\n\r");
  }
  */

  /*
  printf("n=%d\n", n);

  prints("trying to stat...\n\r");
  stat("/", sbuf);
  printf("ssize=%d\n\r", sbuf->st_size);

  */
  close(fd);
}
