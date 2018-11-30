/********** test.c file *************/
#include "ucode.c"

char buf[1024];
char uinput[128];
char args[20][128];   // perhaps a little ugly

char* valid_cmds[50];

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

  int stdin = 0;
  char args[10][32];

  i = 0;
  while (1) {
    printf("\n\r scsh #%d $ ", getpid());
    gets(uinput);


    printf("\n\r your input: %s\n\r", uinput);


    while(strtok(uinput, args[i], '|', i)) {
      //printf("args[%d] === %s\n\r", i, args[i]);
      i++;
    }
    int count = i;
    i--;
    printf("count = %d\n\r", count);

    char* cp;
    int len;
    // remove spaces from the start
    for (; i >= 0; i--) {
      cp = args[i];
      len = 0;
      while(*cp == ' ') { cp++; }
      strcpy(args[i], cp);
      printf("args[%d] = %s\n\r", i, args[i]);
    }

    i = 0;
    // TODO: forked process isn't killed if
    child = fork();
    if (child) {
      wait(&status);        // wait for child (still executing) to die
    }
    else {    // child is running now

      if (count == 2) {
        prints("CREATING PIPE\n\r");
        do_pipe(args[0], args[1]);
        //wait(&status);
      }

      else if (count == 1) {
        exec(args[0]);
      }

    }
    //wait(&status);    // wait for child (that just started executing) to die
    memset(uinput, 0, 128);

    //for (i = 0; i < 10; i++) {
    //  memset(args[i], 0, sizeof(int) * 32);
    //}
    i = 0;

  }
}

int do_pipe(char* cmd1, char* cmd2){
  int pid, pd[2];
  int status;
  pipe(pd);     // create a pipe: pd[0] = READ, pd[1] = WRITE
  pid = fork();         // fork a child to share the pipe
  if (pid) {            //parent: as pipe READER
    close(pd[1]);       // close pipe WRITE end
    dup2(pd[0], 0);     // redirect stdin to pipe READ end
    exec(cmd2);
  }
  else {                // child: as pipe WRITER
    close(pd[0]);       // close pipe READ end
    dup2(pd[1], 1);     // redirect stdout to pipe WRITE end
    exec(cmd1);
  }
}
