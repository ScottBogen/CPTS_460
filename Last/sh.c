/********** test.c file *************/
#include "ucode.c"

char buf[1024];
char uinput[128];
char args[20][128];   // perhaps a little ugly

int child;

int main(int argc, char *argv[ ]) {

  int i;
  int n;
  int in, out;
  int status;
  int pid = getpid();
  char tty[32];
  gettty(tty);

  /*
    sh theory

    read stdin
    tokenizes it into cmd arg1 arg2 ...

    fork a child
    switch to it
      child: exec("cmd arg1 arg2")

    later we can look for pipes and redirection
  */

  while (1) {
    prints("SCOTT SHELL\n\r");
    printf("\n\r scsh #%d $ ", pid);
    gets(uinput);


    /*
    int i = 0;
    while(strtok(uinput, buf, ' ', i++)) {
      // buf now stores one of four things:
      //    1. cmd
      //    2.
    }*/

    printf("\n\r your input: %s\n\r", uinput);

    child = fork();
    if (child) {
      parent();   // child busy, wait for it to wake up
    }
    else {
      printf("SH forked child for exec\n\r");
      exec("cat f1");
    }
    memset(uinput, 0, 128);
  }
}

int parent() {
  int pid, status;
  pid = wait(&status);
  if (pid==child) {
    child = fork();
    if (child) {
      //continue;
    }
    else {
      printf("SH forked child for exec\n\r");
      exec("cat f1");
      return 0;
    }
  }
}
