/********** test.c file *************/
#include "ucode.c"

char buf[1024];

int main(int argc, char *argv[ ]) {

  prints("ENTERED SCOTT'S GREP\n\r");

  int i, j, k, n, length;
  int in, out;
  char* pattern;
  char line[128];
  char c;

  if (argc < 2) {
    prints("Not enough args\n\r");
    return -1;
  }

  // STDIN
  if (argc == 2) {
    pattern = argv[1];
    in = 0;
    out = 1;
  }

  // FILE
  else {
    pattern = argv[1];
    in = open(argv[2], O_RDONLY);
    if (in < 1) {
      printf("Bad file: %s\n\r", argv[2]);
      return -1;
    }
    out = 1;
  }

  // make sure pattern is acceptable
  if (!pattern) { return -1; }

  //printf("Grep with in=%d and out=%d\n\r", in, out);
  //printf("PATTERN: \"%s\"\n\r", pattern);

  // find pattern length
  length = 0;
  i = 0;
  while (pattern[length] != '\0') {
    length++;
  }

  //printf("length = %d\n\r", length);
  //prints("Entering main loop...\n\r");

  // main loop

  i = j = k = 0;
  int to_print = 0;
  while (1) {
    n = read(in, buf, 1);      // read in from buf
    if (n < 1) { break; }         // leave if no more bytes

    //printf("buf = %s\n\r", buf);

    line[i] = buf[0];


    if (buf[0] == 10) {
      prints("\r");
      line[++i] = 13;
      j = 0;
      if (to_print) {
        write(out, line, 128);
      }
      to_print = 0;
      i = 0;
      memset(line, 0, 128);
    }

    else {
      if (line[i] == pattern[j]) {
        j++;
        if (j == length) {
          to_print = 1;
        }
      }
      else {
        j = 0;
      }
      i++;
    }
  }
  close(in); close(out);
}
