#include "ucode.c"

char cwd[32];
char buf2[1024];

// example call: ls /bin
int main(int argc, char *argv[ ]) {

  struct stat* sbuf, mystat;

  char filename[1024];

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
  int n;

  n = read(fd, buf2, 1024);
  char* cp = buf2;
  DIR* dp = (DIR*)cp;

  while (cp < buf2 + 1024) {
    printf("dp = %s\n\r", dp->name);

    int r = stat("./", sbuf);
    if (r < 0) { prints("BAD STAT\n\r"); }

    else {
      printf("r = %d\n\r", r);
      printf("ino = %d\n\r", sbuf->st_ino);
      printf("mode = %x\n\r", sbuf->st_mode);
    }

    cp += dp->rec_len;
    dp = (DIR*)cp;
  }

  close(fd);
}
