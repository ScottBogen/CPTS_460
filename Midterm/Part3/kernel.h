#ifndef _KERNEL_H_
#define _KERNEL_H_

int init();
void kexit();
PROC *kfork(int func, int priority);
int scheduler();
int body();

typedef struct semaphore {
  int spinlock;
  int value;
  PROC* queue;
}SEMAPHORE;

#endif
