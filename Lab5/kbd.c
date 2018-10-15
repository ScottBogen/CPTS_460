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

// kbd.c file

#include "keymap"

u8 keymaps[256] = {0};

typedef volatile struct kbd{ // base = 0x1000 6000
  u32 control; // 7- 6-    5(0=AT)  4=RxIntEn 3=TxIntEn  2   1   0
  u32 status;  // 7- 6=TxE 5=TxBusy 4=RXFull  3=RxBusy   2   1   0
  u32 data;
  u32 clock;
  u32 intstatus;
  // other fields;
}KBD;

volatile KBD *kbd;
volatile int *kdata = (int *)0x10006008;

volatile char kbuf[128];
volatile int khead, ktail, kchar, kroom;
volatile int kline;

void kbd_handler()
{
  unsigned char scode, c;
  volatile char *t, *tt;
  int i;

  //color=RED;
  while(kbd->status & 0x20);    // while TxBusy OR RXbusy
  while(kbd->status & 0x08);    // while TxBusy OR RXbusy
  while(!(kbd->status & 0x10)); // while RX not full

  scode = KBD_DR;

  //if (scode==0x1D || scode==0x9D) goto out;
  //if (scode==0x38 || scode==0xB8) goto out;
  if (scode & 0x80)               goto out;

  if (keymaps[scode]) {
    keymaps[scode]--;
    goto out;
  }

  keymaps[scode]++;

  c = unsh[scode];
  //kprintf("kbd interrupt: c=%x %c\n", c, c);
  kprintf("%c", c);
  kbuf[khead++] = c;
  khead %= 128;
  kchar++; kroom--;

  if (c == '\r'){
    //kbuf[khead-1] = 0; // this puts \r into the buffer
    kline++;
    /*******************
    //kprintf("kline=%d : ", kline);
    t = tt = &kbuf[ktail];
    for (i=0; i<kline; i++){
      //kprintf("[%s] ", t);
       while(*tt){
            tt++;
       }
       t = tt + 1;
       tt = t;
    }
    kprintf("\n");
    *******************/
  }
 out:
  kbd->status = 0xFF;
  VIC_VADDR = 0xFF;
}


int kputc(char);

int kbd_init()
{
  kprintf("kbd_init(): ");
  kbd = (KBD *)(0x10006000);
  kbd->control = 0x14; // 0001 0100
  kbd->clock = 8;
  //cursor = 128;      // cursor icon set in vid.c fbuf_init()
  kchar = 0; kroom = 128;
  khead = ktail = 0;
  kline = 0;
}
/**************
int kgetc()
{
    unsigned char scode, key;
    color = RED;
    while(kbd->status & 0x80);    // while RXbusy
    while(!(kbd->status & 0x10)); // while RX not full

    while(1){
       scode = kbd->data & 0xFF;
       if ((scode & 0x80))
	 continue;
       key = unsh[scode];
       uputc(up, key);
       kputc(key);

       if (key=='\r'){
	 kputc('\n'); uputc(up, '\n'); // to uart also
       }
       return key;
    }
}
***********************/
/***** with kbuf[], this is not used **************
int kgets(char s[ ])
{
  char c;
  while((c=kgetc()) != '\r'){
    *s = c;
    kputc(c);
    s++;
  }
  *s = 0;
  return strlen(s);
}
**********************************************/
char kgetc()
{
  char c; int i;
  //printf("in kgetc()\n");
  unlock();
  while(kchar==0);

  //printf("khead=%d ktail=%d\n", khead, ktail);
  if (ktail >= khead)
    ktail = khead-1;
  /*
 i = ktail;
  for (i=ktail-1; i<khead; i++)
    printf("[%x%c]", kbuf[i], kbuf[i]);
  */
  c = kbuf[ktail++];
  ktail %= 128;
  kchar--; kroom++;
  c = c & 0x7F;

  return c;
}

int kgetline(char s[ ])
{
  char c;
  if (kline==0){
    //kprintf("enter a line from KBD: ");
     while(kline==0); // wait until kline > 0
  }
  // fetch a line from kbuf[ ]

  while(1){
      c = kbuf[ktail++];
      *s++ = c;
      ktail %= 128;
      kchar--; kroom++;
      if (c=='\r')
	break;
  }
  *(s-1) = 0;
  kline--;
}
/*****
  while((*s = kbuf[ktail]) != '\r'){
    //kputc(*s);
    ktail++;
    ktail %= 128;
    kchar--; kroom++;
    s++;
  }
  *s = 0;
  kline--;
 }
**********/
