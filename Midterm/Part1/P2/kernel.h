#ifndef _KERNEL_H_
#define _KERNEL_H_

int init();
void kexit();
PROC *kfork(int func, int priority);
int scheduler();
int body();



#endif
