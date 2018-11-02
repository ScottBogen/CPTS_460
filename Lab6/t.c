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

#include <stdint.h>
#include "type.h"
#include "string.c"

char *tab = "0123456789ABCDEF";
int BASE;
int color;

#define BUSYWAIT   0
#define NOBUSYWAIT 1
int how;

char *uimage;
char buf1[BLKSIZE], buf2[BLKSIZE];

typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
typedef struct ext2_super_block SUPER;
GD    *gp;
DIR   *dp;
INODE *ip, *ip_save;
SUPER *sp;


u8 ino;
u16 iblk;   // for storing inode table block
u16 blocks_per_inode = BLKSIZE / sizeof(INODE);

#include "kbd.c"
#include "uart.c"
#include "timer.c"
#include "vid.c"
#include "interrupts.c"
#include "queue.c"
#include "kernel.c"
#include "wait.c"
#include "fork.c"
#include "exec.c"
#include "svc.c"
#include "load.c"
#include "sdc.c"


void copy_vectors(void) {
    extern uint32_t vectors_start;
    extern uint32_t vectors_end;
    uint32_t *vectors_src = &vectors_start;
    uint32_t *vectors_dst = (uint32_t *)0;
    while(vectors_src < &vectors_end)
       *vectors_dst++ = *vectors_src++;
}

int kprintf(char *fmt, ...);
void timer0_handler();

void DATA_handler()
{
  printf("data exception\n");
}

// IRQ interrupts handler entry point
void IRQ_handler()
{
    int vicstatus, sicstatus;
    int ustatus, kstatus;

    // read VIC status register to find out which interrupt
    vicstatus = VIC_STATUS;
    sicstatus = SIC_STATUS;
    //kprintf("vicstatus=%x sicstatus=%x\n", vicstatus, sicstatus);

    if (vicstatus & 0x0010){
         timer0_handler();
	 // kprintf("TIMER "); // verify timer handler return to here
    }
    if (vicstatus & 0x1000){
         uart0_handler();
	 // kprintf("U0 ");
    }
    if (vicstatus & 0x2000){
         uart1_handler();
    }
    if (vicstatus & 0x80000000){
      if (sicstatus & (1<<3)){
          kbd_handler();
       }
       if (sicstatus & (1<<22)){
          sdc_handler();
       }
    }
}

int main()
{
   char line[32];

   color = RED;
   row = col = 0;
   BASE = 10;

   fbuf_init();
   kprintf("                     Welcome to WANIX in Arm\n");
   kprintf("LCD display initialized : fbuf = %x\n", fb);
   color = CYAN;

   kbd_init();
   uart_init();
   up = upp[0];

   /* enable UART0 IRQ */
   VIC_INTENABLE |= (1<<4);  // timer0,1 at 4
   VIC_INTENABLE |= (1<<12); // UART0 at 12
   VIC_INTENABLE |= (1<<13); // UART1 at 13
   VIC_INTENABLE = 1<<31;    // SIC to VIC's IRQ31
   /* enable UART0 RXIM interrupt */
   UART0_IMSC = 1<<4;
   /* enable UART1 RXIM interrupt */
   UART1_IMSC = 1<<4;
   /* enable KBD IRQ */
   SIC_ENSET = 1<<3;  // KBD int=3 on SIC
   SIC_PICENSET = 1<<3;  // KBD int=3 on SIC
   /* enable KBD and SDC IRQ */
   SIC_INTENABLE |= (1<<3);  // KBD int=bit3 on SIC
   SIC_INTENABLE |= (1<<22); // SDC int=bit22 on SIC
   SIC_ENSET |= (1<<3);   // KBD int=3 on SIC
   SIC_ENSET |= (1<<22);  // SDC int=22 on SIC

   timer_init();
   timer_start(0);
   sdc_init();
   init();

   //kprintf("RAMdisk start=%x size=%x\n", disk, dsize);
   kfork("/bin/init");

   kprintf("P0 switch to P1 : enter a line : ");
   kgetline(line);

   tswitch();  // switch to run P1 ==> never return again
}
