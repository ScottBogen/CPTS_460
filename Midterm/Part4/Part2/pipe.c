#include "pipe.h"

// write n bytes from buf to pipe
int write_pipe(PIPE *p, char *buf, int n) {
  int r = 0;
  if (n<=0) {
    return 0;
  }
  if (!p) { return 0; }
  if (p->status == FREE) { return 0; }

  while(n) {
    while(p->room) {
      p->buf[p->head++] = *buf++;
      p->head %= PSIZE;
      p->data++;
      p->room--;
      r++;
      n--;
      if (n==0) {
        break;
      }
    }
    kwakeup(&p->data);
    if (n==0) {
      return r;
    }
    ksleep(&p->room);
  }
}

// read n bytes from the pipe to the buffer
int read_pipe(PIPE *p, char *buf, int n) {
  int r = 0;
  if (n<=0) { printf("n<=0\n"); return 0; }
  if (!p) { printf("!p");return 0; }
  if (p->status == FREE) { printf("p == FREE"); return 0; }

  while (n) {
    while (p->data) {
      *buf++ = p->buf[p->tail++];
      p->tail %= PSIZE;
      p->data--;
      p->room++;
      r++;
      n--;
      if (n==0) {
        int wpid = kwakeup(&p->room);
        printf("Read pipe detected writer is dead\n");
        if (wpid==-1) {
          int found=0;

          // search for writer procs
          for (int i = 0; i < NPROC; i++) {
            PROC* tmp = &proc[i];
            if (tmp->event == &p->room && tmp->status == SLEEP) {
              kwakeup(&proc[i]);
              found++;
              break;
            }
          }
          if (!found) {
            printf("READER ERROR: no more writers found\n");
            return -1;
          }
        }
        break;
      }
    }

    if (r) {
      return r;
    }
    ksleep(&p->data);
  }
}
