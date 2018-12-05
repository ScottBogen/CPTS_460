/********** test.c file *************/
#include "ucode.c"

char buf[1024];
char buf2[1024];

int main(int argc, char *argv[ ]) {

  int fd, n;
  int in, out;
  in = out = 0;   // just by default
  char tty[32];
  gettty(tty);

  int outtty = open(tty, O_RDWR);


  int lines=0;
  int stopprint=0;

  //printf("in my MORE w arg %d\n\r", argc);

  if (argc < 2) {
    in = 0;
    out = 1;
  }
  else {
    in = open(argv[1], O_RDONLY);
    out = 1;
  }

  printf("OUTTTY = %d\n\r", outtty);



  // from file
  while ((n = read(in, buf, 1)) == 1) {

    if (!in) { write(outtty, buf, 1); }
    else { write(out, buf, 1); }

    if (buf[0] == '\n') {
      lines++;
      if (lines == 25) {
        stopprint=1;
      }
      if (stopprint) {
        while (1){
          if (!in) {
            close(0);
            open(0);
          }
          char c = 0;
          c = mygetc(outtty);
          if (c == 13) { break; }
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

  close(in); close(out); close(outtty);
}
