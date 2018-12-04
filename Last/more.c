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
    in = 0;
    out = 1;
    printf("in=%d, out=%d, fd=%d\n\r",in,out,fd);
  }
  else {
    in = open(argv[1], O_RDONLY);
    out = 1;
  }


  // from file
  if (in>0) {
    while ((n = read(in, buf, 1)) == 1) {
      write(out, buf, 1);

      if (buf[0] == '\n') {
        lines++;
        if (lines == 25) {
          stopprint=1;
        }
        if (stopprint) {
          while (1){
            char c = getc();
            if (c == 13) { break;}
            else if (c == ' ') {
              lines = 0;
              stopprint = 0;
              break;
            }
          }
        }
        prints("\r");
      }
    }
  }

  close(in); close(out);
}
