/********** test.c file *************/
#include "ucode.c"

char buf[1024];

int main(int argc, char *argv[ ]) {

  int status;
  char* cmd = "cat f1";
  char* direct = "cat f1 > outfile";
  char* filename = "output";
  int fd = open(filename, O_WRONLY|O_CREAT|O_APPEND);
  //printf("fd = %d\n\r", fd);
  //close(fd);
  //fd = open(filename, 1);
  printf("fd = %d\n\r", fd);

  // file opened or created

  char command[32];
  memset(command, 0, 32);

  strtok(direct, command, '>', 0);


  printf("command = %s\n\r", command);

  int child = fork();

  if (!child) {
    //close(0);
    dup2(fd, 1);
    exec(cmd);
  }
  else {
    wait(&status);
  }

  close(fd);

  /*
  else {                // child: as pipe WRITER
    close(pd[0]);       // close pipe READ end
    dup2(pd[1], 1);     // redirect stdout to pipe WRITE end
    exec(cmd1);
  }
  */






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

}
