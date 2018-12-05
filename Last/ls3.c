#include "ucode.c"

char cwd[32];
char buf2[1024];

char* t1 = "xwrxwrxwr-------";
char* t2 = "----------------";

// example call: ls /bin
int main(int argc, char *argv[]) {
  int r, fd, n;
  char *s;
  char filename[1024], cwd[1024];
  struct stat* sp, mystat;


  if (argc > 1) {     // ex: ls /bin
    printf("location = %s\n\r", argv[1]);
    fd = open(argv[1], O_RDONLY);
  }
  else {
    getcwd(cwd);      // ex: ls
    printf("cwd = %s\n", cwd);
    fd = open(cwd, O_RDONLY);
  }


  if (fd < 0) {
    prints("File not opened successfully\n");
    close(fd);
    return 0;
  }
  else {
    printf("%d opened\n\r", fd);
  }

  n = read(fd, buf2, 1024);
  char* cp = buf2;
  DIR* dp = (DIR*)cp;

  close(fd);

  while(cp < buf2 + 1024) {

    printf("Directory: %s\n\r", dp->name);
    char* name = dp->name;

    // cannot do this part until s is set
    sp = &mystat;

    if ((r = stat(name,sp)) < 0) {
      prints("Cannot ls\n\r");
      return -1;
    }
    else {
      printf("r = %d\n\r", r);
    }

    strcpy(filename, name);

    if (s[0] != '/') {
      strcpy(filename, cwd);
      strcat(filename, "/");
      strcat(filename, name);
    }

    printf("filename = %s\n\r", filename);

    if (strcmp(name, ".") && strcmp(name, "..") && strcmp(name, "lost+found")) {
      printf("ino = %d\n\r", sp->st_ino);
      printf("mode = %x\n\r", sp->st_mode);
    }
    else {
      printf("skipping %s\n\r", name);
    }

    cp += dp->rec_len;
    dp = (DIR*)cp;
  }

  close(fd);
  return 0;
}
