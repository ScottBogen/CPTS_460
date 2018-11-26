#include "ucode.c"

char buf[1024];
char buf2[4096];

int main(int argc, char *argv[ ]) {

  int fd, n;
  int in, out;
  in = out = 0;   // just by default
  char tty[16];   // store tty
  int pid = getpid();

  //printf("in my CAT w arg %d\n\r", argc);

  // argv = "cat"
  if (argc == 1) {
    gettty(tty);
    //printf("tty=%s\n\r", tty);
    //in = open(tty, O_RDONLY);
    //out = open(tty, O_WRONLY);
    in = 0;
    out = 1;

    fd = 0;     // why not make it fd = in?     note: because fd isn't even used when arg==1
    //printf("in=%d, out=%d\n\r",in,out);
  }
  // argv = "cat filename"
  else {
    fd = open(argv[1], O_RDONLY);
    out = 1;
    //printf("CAT: opened file %s\n\r", argv[1]);
  }

  // from FILE
  if (fd>0) {
    //prints("CAT$$ READ from file \n\r");
    while ((n = read(fd, buf, 1)) == 1) {
      write(out, buf, 1);

      if (buf[0] == '\n') {       // TODO: It adds 1 byte, ask about this
        prints("\r");
      }

    }
  }


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
  }

  close(fd);
  close(in); close(out);
}
