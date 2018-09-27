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
  PROC *qcur = queue;

  // list is empty
  if (qcur == NULL) {
    qcur = &p;
    qcur->parent = qcur;
  }

  // list is not empty
  else {
    while (qcur->next != NULL) {
      if (p->priority < qcur->priority) {       // insert p here
        qcur->parent->next = p;
        p->parent = qcur->parent;
        p->next = qcur;
        qcur->parent = p;
        break;
      }
      else {
        qcur = qcur->next;
      }
    }
    // we may be at the end of the list...
    if (qcur->next == NULL) {
      qcur->next = p;
      p->parent = qcur;
    }
  }

  int_on(SR);          //  restore CPSR
}

// remove and return first PROC from queue
PROC *dequeue(PROC **queue)
{
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
