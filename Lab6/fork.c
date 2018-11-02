int body(), goUmode();
char *istring = "init start";

int fork() {
  /*
          Algorithm of fork():

    1. get a PROC for the child and initialize it, e.g. ppid =
       parent pid, priority = 1 etc.
    2. copy parent Umode image to child, so that their Umode
       images are identical
    3. copy (part of) parent kstack to child kstack, ensure that
       the child return to the same virtual address as the parent
       but in its own umode image
    4. copy parent usp and spsr to child
    5. mark child PROC READY and enter it into readyqueue
    6. return child pid
  */

  int i;
  char *PA, *CA;
  PROC *p = dequeue(&freeList);
  if (p==0) { printf("fork failed\n"); return -1; }
  p->ppid = running->pid;
  p->parent = running;
  p->status = READY;
  p->priority = 1;

  PA = (char *)(running->pgdir[2048] & 0xFFFF0000); // parent Umode PA
  CA = (char *)(p->pgdir[2048] & 0xFFFF0000);

  // child
  // Umode PA
  memcpy(CA, PA, 0x100000); // copy 1MB Umode image
  for (i=1; i <= 14; i++){ // copy bottom 14 entries of kstack
    p->kstack[SSIZE-i] = running->kstack[SSIZE-i];
  }
  p->kstack[SSIZE - 14] = 0;
  // child return pid = 0
  p->kstack[SSIZE-15] = (int)goUmode; // child resumes to goUmode
  p->ksp = &(p->kstack[SSIZE-28]); // child saved ksp
  p->usp = running->usp; // same usp as parent
  p->ucpsr = running->ucpsr; // same spsr as parent

  enqueue(&readyQueue, p);
  return p->pid;
}


PROC *kfork(char *filename)
{
  int i;
  char *cp, *cq;
  char *addr;
  char line[8];
  int usize1, usize;
  int *ustacktop, *usp;
  u32 BA, Btop, Busp;

  PROC *p = dequeue(&freeList);
  if (p==0) {
    kprintf("kfork failed\n");
    return (PROC *)0;
  }
  p->ppid = running->pid;
  p->parent = running;
  p->parent = running;
  p->status = READY;
  p->priority = 1;

  // set kstack to resume to body
  for (i=1; i<29; i++)  // all 28 cells = 0
    p->kstack[SSIZE-i] = 0;

  p->kstack[SSIZE-15] = (int)goUmode;  // in dec reg=address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-28]);

  if (filename) {
    loader(filename, p);
  }

  // kstack must contain a resume frame FOLLOWed by a goUmode frame
  //  ksp
  //  -|-----------------------------------------
  //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 fp ip pc|
  //  -------------------------------------------
  //  28 27 26 25 24 23 22 21 20 19 18  17 16 15
  //
  //   usp      NOTE: r0 is NOT saved in svc_entry()
  // -|-----goUmode--------------------------------
  //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 ufp uip upc|
  //-------------------------------------------------
  //  14 13 12 11 10 9  8  7  6  5  4   3    2   1
  /********************

  // to go Umode, must set new PROC's Umode cpsr to Umode=10000
  // this was done in ts.s dring init the mode stacks ==>
  // user mode's cspr was set to IF=00, mode=10000

  ***********************/
  // must load filename to Umode image area at 7MB+(pid-1)*1MB

  // must fix Umode ustack for it to goUmode: how did the PROC come to Kmode?
  // by swi # from VA=0 in Umode => at that time all CPU regs are 0
  // we are in Kmode, p's ustack is at its Uimage (8mb+(pid-1)*1mb) high end
  // from PROC's point of view, it's a VA at 1MB (from its VA=0)
  // but we in Kmode must access p's Ustack directly

  /***** this sets each proc's ustack differently, thinking each in 8MB+
  ustacktop = (int *)(0x800000+(p->pid)*0x100000 + 0x100000);
  TRY to set it to OFFSET 1MB in its section; regardless of pid
  **********************************************************************/
  //p->usp = (int *)(0x80100000);

  BA = p->pgdir[2048] & 0xFFF00000;

  Btop = BA + 0x100000;
  Busp = Btop - sizeof(u32);  // stack pointer at -32

  cp = (char*) Busp;
  strcpy(cp, istring);  // put "init start" at high end address


  p->usp = (int *)VA(0x100000);

  p->kstack[SSIZE-1] = (int)0x80000000;
  p->kstack[SSIZE-1] = VA(0);

  // -|-----goUmode-------------------------------------------------
  //  r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 ufp uip upc|string       |
  //----------------------------------------------------------------
  //  14 13 12 11 10 9  8  7  6  5  4   3    2   1 |             |

  enqueue(&readyQueue, p);

  kprintf("proc %d kforked a child %d: ", running->pid, p->pid);
  printQ(readyQueue);

  return p;
}
