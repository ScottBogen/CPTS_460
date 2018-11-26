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

  //printf("in my MORE w arg %d\n\r", argc);

  if (argc < 2) {
    gettty(tty);
    printf("tty=%s\n\r", tty);
    in = 0;
    out = 1;
    fd = 0;
    printf("in=%d, out=%d, fd=%d\n\r",in,out,fd);
  }
  else {
    fd = open(argv[1], O_RDONLY);
    out = 1;
  }


  // from file
  if (fd>0) {
    while ((n = read(fd, buf, 1)) == 1) {
      write(out, buf, 1);

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
    prints("no arg\n");
    int i = 0;
    while ((n = read(in, buf, 1)) == 1) {
      //prints(buf);

      if (buf[0] == 13) {
          buf2[i++] = '\0';

          write(out, buf2, i);
          prints("\n\r");
          break;
      }
      buf2[i++] = buf[0];
    }
  }

  close(fd);
  close(in); close(out);
}
