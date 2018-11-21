/********** test.c file *************/
#include "ucode.c"

char buf[1024];
int console;
int s1;
int s0;

int parent() {
  int pid, status;
  while(1) {
    prints("INIT: wait for zombie child\n");
    pid = wait(&status);

    if (pid==console) {
      prints("INIT: fork a new console login\n");
      console = fork();
      if (console){
        continue;
      }
      else {
        exec("login /dev/tty0");
      }
    }
    else if (pid == s1) {
      prints("INIT: fork a new TTYS1 login\n");
      s1 = fork();
      if (s1){
        continue;
      }
      else {
        exec("login /dev/ttyS1");
      }
    }
    else if (pid == s0) {
      prints("INIT: fork a new TTYS0 login\n");
      s0 = fork();
      if (s0){
        continue;
      }
      else {
        exec("login /dev/ttyS0");
      }
    }

    printf("INIT: i buried an orphan child proc %d\n", pid);
  }
}

int main(int argc, char *argv[ ]) {
  int in1, out1, in2, out2, in3, out3;
  in1 = open("/dev/tty0", O_RDONLY);   // file descriptor 0
  out1 = open("/dev/tty0", O_WRONLY);  // for display to console
  in2 = open("/dev/ttyS1", O_RDONLY);
  out2 = open("/dev/ttyS1", O_WRONLY);  // for display to console
  in3 = open("/dev/ttyS0", O_RDONLY);
  out3 = open("/dev/ttyS0", O_WRONLY);

  prints("INIT: fork a login proc on console\n");
  console = fork();

  if (console) {    // parent
    s1 = fork();
    if (s1) {
      s0 = fork();
      if (s0) {
        parent();
      }
      else {
        exec("login /dev/ttyS0");
      }
    }
    else {
      exec("login /dev/ttyS1");
    }
  }
  else {            // child: exec to login on tty0
    exec("login /dev/tty0");
  }
}
