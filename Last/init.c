/********** test.c file *************/
#include "ucode.c"

char buf[1024];
int console;
int s1;
int s0;

int in1, out1, in2, out2, in3, out3;

int parent() {
  int pid, status;
  while(1) {
    prints("INIT: wait for zombie child\n");
    pid = wait(&status);

    if (pid==console) {
      console = fork();
      if (console){
        continue;
      }
      else {
        char* line = "Login on port: console\n\r";
        write(out1, line, 24);
        close(out1);
        exec("login /dev/tty0");
      }
    }

    else if (pid == s1) {
      s1 = fork();
      if (s1){
        continue;
      }
      else {
        char* line = "Login on port: ttyS1\n\r";
        write(out2, line, 22);
        close(out2);
        exec("login /dev/ttyS1");
      }
    }

    else if (pid == s0) {
      s0 = fork();
      if (s0){
        continue;
      }
      else {
        char* line = "Login on port: ttyS0\n\r";
        write(out3, line, 22);
        close(out3);
        exec("login /dev/ttyS0");
      }
    }

    printf("INIT: i buried an orphan child proc %d\n", pid);
  }
}

int main(int argc, char *argv[ ]) {
  in1 = open("/dev/tty0", O_RDONLY);   // file descriptor 0
  out1 = open("/dev/tty0", O_WRONLY);  // for display to console
  in2 = open("/dev/ttyS1", O_RDONLY);
  out2 = open("/dev/ttyS1", O_WRONLY);  // for display to console
  in3 = open("/dev/ttyS0", O_RDONLY);
  out3 = open("/dev/ttyS0", O_WRONLY);

  console = fork();

  if (console) {    // parent
    s1 = fork();
    if (s1) {
      s0 = fork();
      if (s0) {
        parent();
      }
      else {
        char* line = "Login on port: ttyS0\n\r";
        write(out3, line, 22);
        close(out3);
        exec("login /dev/ttyS0");
      }
    }
    else {
      char* line = "Login on port: ttyS1\n\r";
      write(out2, line, 22);
      close(out2);
      exec("login /dev/ttyS1");
    }
  }
  else {            // child: exec to login on tty0
    char* line = "Login on port: console\n\r";
    write(out1, line, 24);
    close(out1);
    exec("login /dev/tty0");
  }
}
