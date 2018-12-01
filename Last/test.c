/********** test.c file *************/
#include "ucode.c"

char buf[1024];

int main(int argc, char *argv[ ]) {

  int i;
  int fd, n;
  int pid = getpid();
  int status;

  char* line1 = "cat f1";
  char* line2 = "more";

  char* pd[2] = {line1, line2};

  pid = fork();
  if (pid) {
    wait(&status);
  }
  else {
    pipe(pd);
  }


  write_pipe(1, line1, 1024);
  read_pipe(0, buf, 1024);

  printf("buf\n\r", buf);



  // calling read(fd, buf, nbytes):
    // returns read_pipe(fd, buf, nbytes);

  // calling read(fd, buf, nbytes):
    // returns write_pipe(fd, buf, nbytes);

  // 8.12.5.4
  // read/write pipes are implemented in the pipe mechanism in the EOS kernel
  // 8.15.3 - the sh program
  /*
      If a line contains IO redirections, the child sh handles IO
      redirections first. then it uses exec to change image to cmd file.

      If a cmd line contains a pipe symbol, the child sh handles
      the pipe by doing the following do_pipe algorithm:


      int pid, pd[2];
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


  */





  /*
  int left = fork();
  int right = fork();

  if (!left) {
    exec(line1);
  }
  else if (right) {
    exec(line2);
  }
  */




}
