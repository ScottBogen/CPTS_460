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
/*
int enqueue(PROC** queue, PROC *p) {

  int SR = int_off();  // IRQ interrupts off, return CPSR
  PROC *qcur = queue;

  printf("attempting nqueue with proc p and pid %d\n", p->pid);
  printList("queue", queue);

  // list is empty
  if (qcur == NULL) {
    printf("Putting p in empty queue\n");
    qcur = &p;
    qcur->parent = qcur;
  }

  // list is not empty
  else {
    while (qcur->next != NULL) {
      if (p->priority < qcur->priority) {       // insert p here
        printf("\ninserting %d after %d\n", p->pid, qcur->pid);
        printf("p priority = %d, qcur priority = %d\n", p->priority, qcur->priority);
        qcur->parent->next = &p;
        p->parent = qcur->parent;
        p->next = qcur;
        qcur->parent = &p;
        break;
      }
      else {
        printf("skipping pid=%d with priority %d", qcur->pid, qcur->priority);
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
*/

int enqueue(PROC **queue, PROC *p)
{
  int SR = int_off();

  PROC *tmp = *queue;

  if (!tmp) {
    p->next = tmp;
    *queue = p;
    return 1;
  }
  //else we should be at front
  if(p->priority > tmp->priority)
  {
    //set to top
    p->next = tmp;
    *queue = p;
    int_on(SR);
    return 1;
  }

  //else at least one node to find place
  //check p->priority against the NEXT items->priority to make insert easier
  while(tmp->next && p->priority <= (tmp->next)->priority)
  {
    tmp = tmp->next;
  }
  //insert p into list after tmp
  p->next = tmp->next;
  tmp->next = p;
  int_on(SR);

  return 1;
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
