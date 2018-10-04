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
  if (n<=0) { return 0; }
  if (!p) { return 0; }
  if (p->status == FREE) { return 0; }

  while (n) {
    while (p->data) {
      *buf++ = p->buf[p->tail++];
      p->tail %= PSIZE;
      p->data--;
      p->room++;
      r++;
      n--;
      if (n==0) {
        break;
      }
    }
    kwakeup(&p->room);
    if (r) {
      return r;
    }
    ksleep(&p->data);
  }
}
