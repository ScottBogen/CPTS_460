#ifndef _PIPE_H_
#define _PIPE_H_

#define NPIPE 10
#define PSIZE 16

typedef struct pipe {
  char buf[PSIZE];
  int head, tail;
  int data, room;
  int status;
}PIPE;


#endif
