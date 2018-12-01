#include "ucode.c"

char buf[1024];
char buf2[1024];
char string[128];

int main(int argc, char *argv[ ]) {

  int n;
  int in, out;
  int i, j;

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
    n = read(in, buf, 1024);      // read from input
    if (n < 1) { break; }         // if no data, break

    // (almost definitely) user is entering from keyboard
    if (n == 1) {
      //prints("cat from STDIN\n\r");
      prints(buf);              // user can see the buttons they've pressed
      buf2[i++] = buf[0];       // add char to buf2


      // upon pressing enter:
      if (buf[0] == 13) {
        if (last != 10 && last != 0 && last != 13) {
          buf2[i++] = '\0';
          prints("\n\r");
          write(out, buf2, i);
          prints("\n\r");
          break;
        }
      }
      last = buf[0];
    }


    // reading from file
    else {
      //prints("cat from FILE\n\r");
      i = j = 0;
      while (strtok(buf, string, '\n', i++)) {    // if we can tokenize
        while (string[j++]);
        write(out, string, j);
        prints("\n\r");
      }
    }

    j = 0;
    memset(buf, 0, 1024);
    memset(string, 0, 128);
  }

  /*
  // from STDIN
  else {
    prints("cat from STDIN\n");
    int i = 0;
    while ((n = read(in, buf, 1)) == 1) {

      //prints(buf);      // IDEA: make it so that n reads in multiple bytes, but if n==1, then it prints

      if (buf[0] == '\n') {
        buf2[i++] = buf[0];
        buf2[i] = '\0';
        write(out, buf2, i);
        i = 0;
        continue;
      }
      else if (buf[0] == 13) {
        if (buf2[i-1] != 10 && buf2[i-1] != 0 && buf2[i-1] != 13) {
          buf2[i] = '\0';
          write(out, buf2, i);
          break;
        }
      }

      buf2[i++] = buf[0];
    }
  }*/

  close(in); close(out);
}
