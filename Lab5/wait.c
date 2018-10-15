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

int ksleep(int event)
{
  int sr = int_off();
  printf("proc %d ksleep on %x\n", running->pid, event);  
  running->event = event;
  running->status = SLEEP;
  enqueue(&sleepList, running);
  printf("sleepList = "); printQ(sleepList);
  tswitch();
  int_on(sr);
}

int kwakeup(int event)
{
  PROC *p, *tmp=0;
  
  int sr = int_off();
  while((p = dequeue(&sleepList))!=0){
    if (p->event==event){
      printf("kwakeup %d\n", p->pid);
      p->status = READY;
      enqueue(&readyQueue, p);
    }
    else{
      enqueue(&tmp, p);
    }
  }
  sleepList = tmp;
  int_on(sr);
}

int kexit(int value)
{
  int i; PROC *p; int wk1;
  wk1 = 0;

  printf("%d in kexit, value=%d\n", running->pid, value);
  if (running->pid==1){
    kprintf("P1 never dies\n");
    return -1;
  }
  for (i=1; i<NPROC; i++){
    p = &proc[i];
    if ((p->status != FREE) && (p->ppid == running->pid)){
      printf("give %d to P1\n", p->pid);
      p->ppid = 1;
      p->parent = &proc[1];
      wk1++;
    }
  }
  running->exitCode = value;
  running->status = ZOMBIE;
  kwakeup((int)running->parent);
  if (wk1)
     kwakeup((int)&proc[1]);
  tswitch();
}

 int kwait(int *status)
 {
   int i; PROC *p;
   int child = 0;
   printf("%d in kwait() : ", running->pid);
   for (i=1; i<NPROC; i++){
     p = &proc[i];
     if (p->status != FREE && p->ppid == running->pid){
       child++;
     }
   }
   if (child==0){
     printf("no child\n");
     return -1;
   }
   
   while(1){
      for (i=1; i<NPROC; i++){
	p = &proc[i];
        if ((p->status==ZOMBIE) && (p->ppid == running->pid)){
	   kprintf("found a ZOMBIE child %d\n", p->pid);
           *status = p->exitCode;
	   p->status = FREE;
           enqueue(&freeList, p);
           return p->pid;
        }
      }
      printf("sleep on %x\n", running);
      ksleep((int)running); 
   }   
}



