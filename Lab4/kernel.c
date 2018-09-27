/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

/********************
#define  SSIZE 1024
#define  NPROC  9
#define  FREE   0
#define  READY  1
#define  SLEEP  2
#define  BLOCK  3
#define  ZOMBIE 4
#define  printf  kprintf

typedef struct proc{
  struct proc *next;
  int    *ksp;
  int    status;
  int    pid;

  int    priority;
  int    ppid;
  struct proc *parent;
  int    event;
  int    exitCode;
  int    kstack[SSIZE];
}PROC;
***************************/
#define NPROC 9
#include "kernel.h"

PROC proc[NPROC], *running, *freeList, *readyQueue;
int procsize = sizeof(PROC);

int init()
{
  int i, j;
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = READY;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; // circular proc list
  freeList = &proc[0];
  readyQueue = 0;

  printf("create P0 as initial running process\n");
  p = dequeue(&freeList);
  p->priority = 0;
  p->ppid = 0; p->parent = p;  // P0's parent is itself
  running = p;
  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);
}

void kexit()
{
  printf("proc %d kexit\n", running->pid);

  /*
    P1 never dies;
    give away children, if any, to P1
    record exitValue in caller's PROC.exitCode;
    mark the caller status as ZOMBIE;
    wakeup P1 if has given any child to P1; // kwakeup(&proc[1]);
    wakeup parent;                          // kwakeup(running->parent);
    tswitch() to switch process;
  */


  running->status = FREE;
  running->priority = 0;
  enqueue(&freeList, running);   // putproc(running);
  tswitch();
}

PROC *kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);   // take ready process from freelist
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  else {
    printf("dequeued proc#%d\n", p->pid);
  }

  // part 6 here
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running;

  // this area reserved for PROC binary tree work.

  // when a PROC inits, it must be placed in its parent's child list.

  // if parent has no children, make it the child.
  if (p->parent->child == NULL) {
    p->parent->child = p;
  }

  // else go to the child and insert it as the last child.
  else {
    PROC* temp = p->parent->child;
    while (temp->sibling != NULL) {
      temp = temp->sibling;
    }
    // temp-> sibling is now null;
    temp->sibling = p;
  }

  // set kstack to resume to body
  // stack = r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
  //         1  2  3  4  5  6  7  8  9  10 11  12  13  14
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;

  p->kstack[SSIZE-1] = (int)func;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);

  enqueue(&readyQueue, p);
  printf("%d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p;
}

int scheduler()
{
  kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
      enqueue(&readyQueue, running);
  running = dequeue(&readyQueue);
  kprintf("next running = %d\n", running->pid);
}

// process code
int body(int pid, int ppid, int func, int priority)
{
  char c; char line[64];
  int color;
  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    pid = running->pid;
    if (pid==0) color=BLUE;
    if (pid==1) color=WHITE;
    if (pid==2) color=GREEN;
    if (pid==3) color=CYAN;
    if (pid==4) color=YELLOW;
    if (pid==5) color=WHITE;
    if (pid==6) color=GREEN;
    if (pid==7) color=WHITE;
    if (pid==8) color=CYAN;

    printList("readyQueue", readyQueue);

    kprintf("PROC || pid:%d, ppid:%d, func:%x, priority:%d\n",
      pid, ppid, func, priority);

    printf("freelist:::\n");
    printList("freeList",freeList);

    kprintf("proc %d running, parent = %d  ", running->pid, running->ppid);
    kprintf("input a char [s|f|q] : ");
    c = kgetc();

    printf("%c\n", c);



    switch(c){
      case 's': tswitch();           break;
      case 'f': kfork((int)body, 1); break;
      case 'q': kexit();             break;
    }
  }
}
