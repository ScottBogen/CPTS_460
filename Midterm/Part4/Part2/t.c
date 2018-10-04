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

#include "type.h"
#include "string.c"
#include "queue.c"
#include "kbd.c"
#include "vid.c"
#include "exceptions.c"
#include "kernel.c"
#include "timer.c"
#include "uart.c"
#include "pipe.c"

//PIPE *read, *write;
PIPE pipe[NPIPE];
PIPE *kpipe;

TIMER *tp[4];

/*
typedef struct pipe {
  char buf[PSIZE];
  int head, tail;
  int data, room;
  int status;
}PIPE;
*/

int pipe_init() {
  int i;
  for (i = 0; i < NPIPE; i++) {
    pipe[i].status=FREE;
  }
}

PIPE* create_pipe() {
  int i;
  for (i = 0; i < NPIPE; i++) {
    if (pipe[i].status == FREE) {
      pipe[i].status = BUSY;
      pipe[i].head = 0;
      pipe[i].tail = 0;
      pipe[i].data = 0;
      pipe[i].room = PSIZE;  // 64 bytes per pipe
      return &pipe[i];
    }
  }
  return 0;
}

int pipe_writer() {
  struct uart *up = &uart[0];
  char line[128];

  while(1) {
    fuprintf(up, "Enter a line for task 1 to get : ");
    printf("task %d waits for line from UART0\n", running->pid);
    ugets(up,line);
    uprints(up, "\r\n");
    if (!strcmp(line, "exit")) {
      ksleep(running);
      write_pipe(kpipe, line, strlen(line));
      break;
    }
    printf("task %d writes line=[%s] to pipe\n", running->pid, line);
    write_pipe(kpipe, line, strlen(line));
  }
}

int pipe_reader() {
  char line[128];
  int i,n;
  while(1) {
    printf("task%d reading from pipe\n", running->pid);
    n = read_pipe(kpipe, line, PSIZE);
    printf("task%d read n=%d bytes from pipe : [", running->pid, n);
    for (i = 0; i < n; i++) {
      printf("%c",line[i]);
    }
    printf("]\n");
  }
}


void copy_vectors(void) {
    extern u32 vectors_start;
    extern u32 vectors_end;
    u32 *vectors_src = &vectors_start;
    u32 *vectors_dst = (u32 *)0;

    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS; // VIC_STATUS=0x10140000=status reg
    sicstatus = SIC_STATUS;
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);
    if (vicstatus & 0x80000000){
       if (sicstatus & 0x08){
          kbd_handler();
       }
    }

    if (vicstatus & (1<<4)){   // timer0,1=bit4
         timer_handler(0);
    }
}

int main()
{
   int i;
   char line[128];
   u8 kbdstatus, key, scode;

   color = WHITE;
   row = col = 0;
   fbuf_init();
   /* enable timer0, 1, uart0, 1 SIC interrupts */

   VIC_INTENABLE |= (1<<4);  // timer0,1 at bit4
   VIC_INTENABLE |= (1<<5);  // timer2,3 at bit5

   /* enable KBD IRQ */
   VIC_INTENABLE |= (1<<31);  // SIC to VIC's IRQ31
   SIC_ENSET |= 1<<3;       // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;     // KBD int=3 on SIC

   timer_init();
   kbd_init();
   uart_init();
   pipe_init();
   kpipe = create_pipe();

   timer_start(0);
   kprintf("Welcome to WANIX in Arm\n");
   init();


   printf("attempting kfork....\n\n\n");
   kfork((int)pipe_writer, 1);
   kfork((int)pipe_reader, 1);

   while(1){
     if (readyQueue)
        tswitch();
   }
}
