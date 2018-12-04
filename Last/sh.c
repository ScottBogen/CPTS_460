/********** test.c file *************/
#include "ucode.c"

char buf[1024];
char uinput[128];

// used to be local
char args[10][32];
char ios[10][2];

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

  while (1) {

    for (i = 0; i < 10; i++) {
      memset(args[i], 0, 32);       // reset args list
      memset(ios[i], 0, 2);         // reset ios list
    }
    i = 0;

    printf("\n\r scsh #%d $ ", getpid());       // get user input
    gets(uinput);


    while(strtok(uinput, args[i], '|', i)) {      // tokenize all the pipes into args[i]
      i++;
    }

    int count = i;            // how many different pipes we have
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

      // look for IO redirections while we're here
      int j = 0;
      while (args[i][j]) {
        if (args[i][j] == '>') {
          if (args[i][j+1] == '>') {
            prints("found misdirect >>\n\r");
            strcpy(ios[i], ">>");
            break;
          }
          else {
            prints("found misdirect >\n\r");
            strcpy(ios[i], ">");
            break;
          }
        }
        else if (args[i][j] == '<') {
          prints("found misdirect <\n\r");
          strcpy(ios[i], "<");
          break;
        }
        j++;
      }
    }

    i = 0;
    for (i = 0; i < 3; i++) {
      printf("args[i] = %s\t", args[i]);
      printf("ios[i] = %s\n\r", ios[i]);
    }


    /* at this point, args may contain something like {"cat f", "grep print > output"}

      here's how to interpret it:

      make a new array called io_locations[], make it the same length as args DONE

      if args[i] has an IO redirection designated by >, >>, or <, then put the RHS into io_locations[i]
      else io_locations[i] = 0 DONE

      before you execute a command, check to see if there is a redirect by seeing if io[i] is != -1
        if there is a redirect, handle it
      then do the pipe
    */





    i = 0;
    // TODO: forked process isn't killed if there are typos
    child = fork();

    if (child) {
      wait(&status);        // wait for child (still executing) to die
    }
    else {    // child is running now

      if (count == 2) {
        prints("CREATING PIPE\n\r");
        do_pipe(args[0], args[1]);      // pipes are handled recursively right to left
                                        // try calling it like do_pipe(args[max])
      }

      else if (count == 1) {
        if (ios[0][0] != 0) {
          handle_IO(args[0], ios[0]);
        }
        exec(args[0]);
      }
    }

    memset(uinput, 0, 128);
    i = 0;

  }
}

int handle_IO(char* arg, char* IO) {
  // split into cmd and file
  char cmd[32];
  char file[32];
  int type;
  int fd;

  int i=0;

  memset(cmd, 0, 32);
  memset(file, 0, 32);

  while(arg[i]) {
    if (arg[i] == '<' || arg[i] == '>') {
      i++;
      break;
    }
    else {
      cmd[i] = arg[i];
    }
    i++;
  }

  int j = 0;
  while (arg[i] == '>' || arg[i] == ' ') {
    i++;
  }
  while (arg[i] && arg[i] != ' ') {
    file[j] = arg[i];
    j++;
    i++;
  }


  printf("cmd = %s\n\r", cmd);
  printf("file = %s\n\r", file);

  strcpy(arg, cmd);

  if (!strcmp(IO, ">")) {
    fd = open(file, O_WRONLY|O_CREAT);
    if (fd<=0) { printf("Couldn't create file %s\n\r", file); return -1; }

    close(0);
    dup2(fd ,1);
    exec(cmd);
    return 1;
  }

  if (!strcmp(IO, ">>")) {
    fd = open(file, O_WRONLY|O_CREAT|O_APPEND);
    if (fd<=0) { printf("Couldn't create file %s\n\r", file); return -1; }

    close(0);
    dup2(fd ,1);
    exec(cmd);
    return 1;
  }
}




int do_pipe(char* cmd1, char* cmd2){          // consider changing to do_pipe(char* args[max]) and going backwards
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
