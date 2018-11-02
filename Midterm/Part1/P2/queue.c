#define NULL 0

// queue.c file
extern PROC *freeList;

/*
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
*/

// enter p into queue by priority

int enqueue(PROC** queue, PROC *p) {

  int SR = int_off();  // IRQ interrupts off, return CPSR
  PROC *qcur = *queue;

  //printf("attempting nqueue with proc p and pid %d\n", p->pid);

  // list is empty
  if (qcur == NULL) {
      p->next = qcur;
      *queue = p;
  }

  else if (p->priority > qcur->priority) {    // insert p at front
    // p = 1
    // queue: 2 NULL
    //        q
    p->next = qcur;
    *queue = p;
  }

  // list is not empty
  else {
    while (qcur->next) {
      if (p->priority > qcur->next->priority) {       // insert p here

        // p = 4
        //
        // queue: 1 2 3 _ 5 NULL
        //            q
        //

        p->next = qcur->next; // 4 -> 5
        qcur->next = p;       // 3 -> 4
        int_on(SR);
        return 1;
      }

      else {
        qcur = qcur->next;
      }
    }

    // we may be at the end of the list...
    if (qcur->next == NULL) {
      // p = 5
      // queue: 1 2 3 4 NULL
      //              q

      qcur->next = p;
      p->next = NULL;
    }
  }
  int_on(SR);          //  restore CPSR
}

// remove and return first PROC from queue
PROC *dequeue(PROC **queue) {
  int SR = int_off();  // IRQ interrupts off, return CPSR
  PROC* p = queue;

  if (p == NULL) {
    kprintf("QUEUE is empty.\n");
  }

  else {
    p = *queue;
    *queue = (*queue)->next;
    p->next = NULL;
  }
  // remove the FISRT element from *queue;
  int_on(SR);          //  restore CPSR

  // return pointer to dequeued PROC;
  return p;
}


int printList(char *name, PROC *p)
{
  kprintf("%s = ", name);
  while(p) {
    kprintf("[%d%d]->", p->pid, p->priority);
    p = p->next;
  }
  kprintf("NULL\n");
}
