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
  int shpid = getpid();
  int pid;
  char tty[32];
  gettty(tty);

  /*
    sh theory

    read stdin
    stdin is dummy data right now

    fork a child
    if fork returns 0, we are the child
      exec("cat f1");
    if fork returns something else, child is busy
      wait for child to die
      when child is dead, fork again
        assuming fork works, exec("cat f1");



    later we can look for pipes and redirection
  */

  int stdin = 0;

  while (1) {
    prints("------SCOTT SHELL------\n\r");
    printf("\n\r scsh #%d $ ", getpid());
    gets(uinput);

    printf("\n\r your input: %s\n\r", uinput);


    child = fork();
    //printf("child = %d\n\r", child);
    if (child) {
      wait(&status);
    }
    else {
      // funny fork happens first, it goes FUNNY -> SCOTSH
      //printf("SH funny forked child %d for exec\n\r", getpid());
      //pause(1);
      exec("cat f1");
    }
    wait(&status);

    //printf("Now we are back at shell again, pid = %d, time to loop\n\r", getpid());
    // when I take away while(1), it will print the above line after SCOTSH is called.
    memset(uinput, 0, 128);
  }
}

/*
int parent() {
  int pid, status;
  pid = wait(&status);
  if (pid==child) {
    child = fork();
    if (child) {

    }
    else {
      //printf("SCOTSH forked child %d for exec\n\r", getpid());
      //pause(1);
      exec("cat f1");
      return 0;
    }
  }
}
*/ 
