/********** test.c file *************/
#include "ucode.c"

char buf[1024];

int main(int argc, char *argv[ ]) {

  prints("ENTERED SCOTT'S GREP\n\r");

  int i, j, k, n, length;
  int in, out;
  char* pattern;
  char line[128];

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
  while (1) {
    n = read(in, buf, 1024);      // read in from buf
    if (n < 1) { break; }         // leave if no more bytes

    //printf("buf = %s\n\r", buf);

    i = j = k = 0;
    while (strtok(buf, line, '\n', i++)) {    // if we can tokenize
      //printf("LINE = %s\n\r", line);
      while (line[j]) {
        if (line[j] == pattern[k]) {
          k++;
          if (k == length) {
            //write(out, line, 128);
            prints(line);
            prints("\n\r");
            continue;
          }
        }
        else {
          k = 0;
        }
        j++;
      }
      j = 0;
      memset(line, 0, 128);
    }
  }

  close(in); close(out);
}
