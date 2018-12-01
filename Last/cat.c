#include "ucode.c"

char buf[1024];
char buf2[1024];
char string[128];
char tty[32];

int main(int argc, char *argv[ ]) {

  int n;
  int in, out;
  int i, j;

  gettty(tty);

  int outtty = open(tty, O_WRONLY);


  // STDIN
  if (argc == 1) {
    in = 0;
    out = 1;
  }
  // FILE
  else {
    in = open(argv[1], O_RDONLY);
    out = 1;
  }

  i = j = 0;
  char last;

  // main loop
  while(1) {
    n = read(in, buf, 1);      // read from input
    if (n < 1) { break; }         // if no data, break
    string[i] = buf[0];

    if (!in) { prints(buf); }

    if (string[i] == 10) {     // 10 == '\n';
      string[++i] = '\r';        // 13 == '\r';

      write(out, string, i);
      write(outtty, "\r", 1);     // to bring the shit back without filling up string

      memset(string, 0, 128);
      i = 0;
    }

    // STDIN user presses "enter"
    else if (string[i] == 13 && !in) {
      string[i++] = 10;
      string[i++] = 13;
      write(outtty, "\r\n", 2);     // to bring the shit back without filling up string
      write(out, string, i);
      memset(string, 0, 128);
      i = 0;
    }

    else {
      i++;
    }
  }

  close(in); close(out);
}
