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

  char args[10][32];

  i = 0;
  while (1) {
    printf("\n\r scsh #%d $ ", getpid());
    gets(uinput);

    printf("\n\r your input: %s\n\r", uinput);

    //strtok(uinput, args[0], '|', 0);
    //printf("args[0] = %s\n\r", args[0]);
    //strtok(uinput, args[1], '|', 1);
    //rintf("args[0] = %s\n\r", args[1]);


    while(strtok(uinput, args[i], '|', i)) {
      printf("args[%d] === %s\n\r", i, args[i]);
      i++;
    }

    i = 0;
    // TODO: forked process isn't killed if 
    child = fork();
    if (child) {
      wait(&status);        // wait for child (still executing) to die
    }
    else {
      if (args[0]) {
        exec(args[0]);
      }
    }
    wait(&status);    // wait for child (that just started executing) to die

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
