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
  int    *usp;
  int    status;
  int    priority;
  int    pid;
  int    ppid;
  struct proc *parent;
  int    event;
  int    exitCode;
  int    *pgdir // level-1 page table; 1MB sections only
  char   name[32];
  int    kstack[SSIZE];
}PROC;
***************************/
extern PROC *kfork();
PROC proc[NPROC], *freeList, *readyQueue, *sleepList, *running;

int procsize = sizeof(PROC);

char *pname[NPROC]={"sun", "mercury", "venus", "earth", "mars", "jupiter",
                     "saturn","uranus","neptune"};

u32 *MTABLE = (u32 *)0x4000;
int kernel_init()
{
  int i, j;
  PROC *p; char *cp;
  int *MTABLE, *mtable;
  int paddr;

  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->status = FREE;
    p->priority = 0;
    p->ppid = 0;
    // p->usp = &ustack[i][1024]; // ustack is at the high end of Uimage
    strcpy(p->name, pname[i]);
    p->next = p + 1;
    p->pgdir = (int *)(0x600000 + p->pid*0x4000); // must be on 16KB boundary
  }
  proc[NPROC-1].next = 0;
  freeList = &proc[0];
  readyQueue = 0;
  sleepList = 0;
  running = getproc();
  running->status = READY;

  printList(freeList);
  //printQ(readyQueue);
  //kprintf("running = %d\n", running->pid);

  printf("building pgdirs at 6MB\n");
  // create pgdir's for ALL PROCs at 7MB; Mtable at 0x4000 by hardcode in ts.s
  MTABLE = (int *)0x4000;     // Mtable at 0x4000
  mtable = (int *)0x600000;   // mtables begin at 7MB

  // Each pgdir MUST be at a 16K boundary ==>
  // 1MB at 6M has space for 64 pgdirs for 64 PROCs
  for (i=0; i<64; i++){       // for 64 PROC mtables
    for (j=0; j<2048; j++){
       mtable[j] = MTABLE[j]; // copy low 2048 entries of mtable
       // in mtable: sectionDesc=0x41E:AP=01, domain=0; CB01=1101
    }
    mtable += 4096;           // advance mtable to next 16KB
  }
  mtable = (int *)0x600000;   // mtables begin at 6MB
  for (i=0; i<64; i++){
    for (j=2049; j<4096; j++){ // zero out high 2048 entries
      mtable[j] = 0;
    }        //   109|8765|4|3210|
    if (i)   //  |AP0|doma|1|CB01|=|010|0000|1|1101|=0100 0001 1101=0x41E
      //mtable[2048]=(0x800000 + (i-1)*0x100000)|0xC32; //0xC12; 0x432=domain1
      // (1). Umode sectionSesc AP=11, domain1 AP=01 : OK
      mtable[2048]=(0x700000 + i*0x100000)|0xC3E; //AP=11,DO=1,CB=11(OK)
      // (2). Umode sectionDesc AP=01  Domain1 AP=11 => OK
      // (3). HOWEVER, secDes   AP=01, domain1 AP=01 do NOT work, WHY?
      // REASON: AP=01 allows access in privileged mode, NOT in Umode. =>
      // while in Umode, one of the Desc or Domain AP must be set to 11.

    //mtable[2048]=(0x700000 + (i-1)*0x100000)|0x43E; //AP=11,DO=1,CB=11
    mtable += 4096;
  }

  // verify some of the pgdir contents
  //printf("Verifying pgdir integrity...\n");
  mtable = (int *)0x700000;   // mtables begin at 7MB
  for (i=0; i<4; i++){
     mtable += 4096;
  }
}

int scheduler()
{
  char line[8];
  int pid; PROC *old=running;
  char *cp;
  kprintf("proc %d in scheduler\n", running->pid);
  if (running->status==READY)
     enqueue(&readyQueue, running);
  printQ(readyQueue);
  running = dequeue(&readyQueue);

  kprintf("next running = %d\n", running->pid);
  pid = running->pid;
  if (pid==1) color=WHITE;
  if (pid==2) color=GREEN;
  if (pid==3) color=CYAN;
  if (pid==4) color=YELLOW;
  if (pid==5) color=BLUE;
  if (pid==6) color=PURPLE;
  if (pid==7) color=RED;
  // must switch to new running's pgdir; possibly need also flush TLB
  if (running != old){
    printf("switch to proc %d pgdir at %x ", running->pid, running->pgdir);
    printf("pgdir[2048] = %x\n", running->pgdir[2048]);
    switchPgdir((u32)running->pgdir);
    //printf("XIAHUA after\n");
  }
}

int do_tswitch()
{
  tswitch();
}

int do_sleep()
{
  int event;
  printf("enter an event value to sleep : ");
  event = geti();
  ksleep(event);
}

int do_wakeup()
{
  int event;
  printf("enter an event value to wakeup : ");
  event = geti();
  kwakeup(event);
}

int do_exit()
{
  int value;
  if (running->pid == 1){
    printf("P1 should not die\n");
    return -1;
  }
  printf("enter an exit value : ");
  value = geti();
  kexit(value);
}


int do_wait()
{
  int pid, status;
  printf("proc %d wait for ZOMBIE child\n", running->pid);
  pid = kwait(&status);
  printf("proc %d waited for a dead child=%d ", running->pid, pid);
  if (pid>0){
    printf("status=%x%d", status, status);
  }
  printf("\n");
}

int do_kfork()
{
  kfork("/bin/u1");
}

int body()
{
  char c; char line[64];
  int pid;
  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    pid = running->pid;
    if (pid==1) color=RED;
    if (pid==2) color=GREEN;
    if (pid==3) color=CYAN;
    if (pid==4) color=PURPLE;
    if (pid==5) color=YELLOW;
    if (pid==6) color=BLUE;
    if (pid==7) color=WHITE;

    printf("----------------------------------------------\n");
    printList(freeList);
    printQ(readyQueue);
    printSleepList(sleepList);
    printf("----------------------------------------------\n");
    kprintf("proc %d in body(), parent = %d, input a char [s|f|q|z|a|w|u] : ",
	    running->pid, running->ppid);
    kprintf("pidaddr=%x\n", &pid);
    kgetline(line);
    c = line[0];

    switch(c){
      case 's': do_tswitch(); break;
      case 'f': do_kfork();   break;
      case 'q': do_exit();    break;

      case 'z': do_sleep();   break;
      case 'a': do_wakeup();  break;
      case 'w': do_wait();    break;
      case 'u': do_goUmode();   break;
    }
  }
}


int do_goUmode()
{
  char line[32];
  int i;
  int *usp = running->usp;

  kprintf("%d goUmode: usp=%x\n", running->pid, usp);
  for (i=0; i<8+14; i++){
    kprintf("%x ", usp[i]);
  }
  kprintf("enter a line ");
  kgetline(line);
  goUmode();
}

int kgetpid()
{
  //kprintf("kgetpid: pid = %d\n", running->pid);
  return running->pid;
}

int kgetppid()
{
  //kprintf("kgetppid: pppid = %d\n", running->ppid);
  return running->ppid;
}
char *pstatus[]={"FREE   ","READY  ","SLEEP  ","BLOCK  ","ZOMBIE", " RUN  "};
int kps()
{
  int i; PROC *p;
  for (i=0; i<NPROC; i++){
     p = &proc[i];
     kprintf("proc[%d]: pid=%d ppid=%d", i, p->pid, p->ppid);
     if (p==running)
       printf("%s ", pstatus[5]);
     else
       printf("%s", pstatus[p->status]);
     printf("name=%s\n", p->name);
  }
  printQ(readyQueue);
}

int kchname(char *s)
{
  kprintf("kchname: name=%s\n", s);
  strcpy(running->name, s);
  return 123;
}

int kgetname() {
  kprintf("name = %s\n", running->name);
  return 1;
}

int kkfork()
{
  PROC *p = kfork("/bin/u1");
  if (p)
    return p->pid;
  return -1;
}

int kkwait(int *status)
{
    int pid, e;
    pid = kwait(&e);
    printf("write %x to status at %x in Umode\n", e, status);
    *status = e;
    return pid;
}

int ktswitch()
{
  //kprintf("%d in ktswitch()\n", running->pid);
  tswitch();
  //kprintf("%d exit ktswitch()\n", running->pid);
}

int getusp()
{
  return (int)running->usp;
}
int kgetPA()
{
  return running->pgdir[2048] & 0xFFFF0000;
}
