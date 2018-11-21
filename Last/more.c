/********** test.c file *************/
#include "ucode.c"

char buf[1024];
char buf2[4096];



int main(int argc, char *argv[ ]) {

  int fd, n;
  int in, out;
  in = out = 0;   // just by default
  char tty[16];

  int lines=0;
  int stopprint=0;

  printf("in my MORE w arg %d\n\r", argc);

  if (argc < 2) {
    gettty(tty);
    printf("tty=%s\n\r", tty);
    in = open(tty, O_RDONLY);
    out = open(tty, O_WRONLY);
    fd = 0;
    printf("in=%d, out=%d, fd=%d\n\r",in,out,fd);
  }
  else {
    fd = open(argv[1], O_RDONLY);
  }


  // from file
  if (fd>0) {
    while ((n = read(fd, buf, 1)) == 1) {
      prints(buf);
      if (buf[0] == '\n') {
        lines++;
        if (lines == 25) {
          stopprint=1;
        }
        if (stopprint) {
          while (getc() != 13);
        }
        prints("\r");
      }
    }
  }
  // from stdin
  else {
    printf("no arg\n");
    int i = 0;
    while ((n = read(in, buf, 1)) == 1) {
      prints(buf);

      if (buf[0] == 13) {
          buf2[i] = '\0';
          printf("\n\r%s\n\r", buf2);
          //write(out, buf2, i);
          break;
      }
      buf2[i++] = buf[0];
    }
  }

  close(fd);
  close(in); close(out);
}
