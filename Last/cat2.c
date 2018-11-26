#include "ucode.c"

char buf[1024];
char buf2[4096];
char line[128];

int main(int argc, char *argv[ ]) {

  int n;
  int in, out;
  int i, j, k;

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

  // main loop
  while(1) {
    n = read(in, buf, 1024);      // read from input
    if (n < 1) { break; }         // if no data, break

    i = j = k = 0;
    while (strtok(buf, line, '\n', i++)) {    // if we can tokenize
      while (line[j++]);
      write(out, line, j);
    }

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
