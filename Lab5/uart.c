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

// uart.c file
/*
UART0 base address: 0x101f1000;
UART1 base address: 0x101f2000;
UART2 base address: 0x101f3000;
UART3 base address: 0x10009000;

// UART's flag register at 0x18
//  7    6    5    4    3    2   1   0
// TXFE RXFF TXFF RXFE BUSY
// TX FULL : 0x20
// TX empty: 0x80
// RX FULL : 0x40
// RX empty: 0x10
// BUSY=1 :  0x08
*/
int kprintf(char *fmt, ...);

typedef volatile struct uart{
  u32 DR;                 // data reg
  u32 DSR; 
  u8  reserverd[16];      // 8+16=24 bytes to FR register
  u32 FR;                 // flag reg at 0x18 
}UART;

volatile UART *upp[4];    // 4 UART pointers to their base addresses
UART *up;

char sbuf[128];
int sdata, sroom, shead, stail;
volatile int sline;

void uart0_handler() {
  char c;
  volatile char *t, *tt;
  int i;

  color = GREEN;

  while(!(UART0_FR & 0x40));
  c = UART0_DR;
  kprintf("uart0 interrupt c=%x %c\n", c, c);

  sbuf[shead++] = c; 
  shead %= 128;
  sdata++; sroom--;

  if (c == '\r'){
    kprintf("sline=%d ", sline);
    sbuf[shead-1] = 0;
    sline++;

    kprintf("uart0 sline=%d : ", sline);
    t = tt = &sbuf[stail];
    for (i=0; i<sline; i++){
       kprintf("[%s] ", t);
       while(*tt){
            tt++;
       }
       t = tt + 1;
       tt = t;
    }
    kprintf("\n");
  }
  color=RED;
}

void uart1_handler() {
  char c;
  color=BLUE;
  while(!(UART1_FR & 0x40));
  c = UART1_DR; 
  kprintf("uart 1 interrupt c=%c %x\n", c, c);
  
  sbuf[shead++] = c; 
  shead %= 128;
  sdata++; sroom--;
  if (c == '\r'){
    sbuf[shead-1] = 0;
    sline++;
  }
  color=RED;
}

int uart_init()
{
  int i;
  kprintf("uart_init(): ");
  for (i=0; i<4; i++)            // uart0 to uart2 are adjacent 
      upp[i] = (UART *)(0x101f1000 + i*0x1000);
  upp[3] = (UART *)(0x10009000); // uart3 at 0x10009000

  sdata=0; sroom=128; shead=stail=0;
  sline = 0;
}

// TO DO: UART outputs should be intertupt-driven also 
int uputc(UART *up, char c)
{
  int i = up->FR;
  while((up->FR & 0x20));
  (up->DR) = (int)c;
}

int ugetc(UART *up)
{
  while( (up->FR) & 0x10);
  return (char)((up->DR) & 0xFF);
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uputs(char *s)
{
  while(*s){
    uputc(up, *s++);
    if (*s=='\n')
      uputc(up,'\r');
  }
   
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);}

int urpx(UART *up, int x)
{
  char c;
  if (x){
     c = tab[x % 16];
     urpx(up, x / 16);
  }
  uputc(up, c);
}

int uprintx(UART *up, int x)
{
  uprints(up, "0x");
  if (x==0)
    uputc(up, '0');
  else
    urpx(up, x);
  uputc(up, ' ');
}

int urpu(UART *up, int x)
{
  char c;
  if (x){
     c = tab[x % 10];
     urpu(up, x / 10);
  }
  uputc(up, c);
}

int uprintu(UART *up, int x)
{
  if (x==0)
    uputc(up, '0');
  else
    urpu(up, x);
  uputc(up, ' ');
}

int uprinti(UART *up, int x)
{
  if (x<0){
    uputc(up, '-');
    x = -x;
  }
  uprintu(up, x);
}

int ufprintf(UART *up, char *fmt,...)
{
  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt + 1;

  while(*cp){
    if (*cp != '%'){
      uputc(up, *cp);
      if (*cp=='\n')
	uputc(up, '\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c': uputc(up, (char)*ip);      break;
    case 's': uprints(up, (char *)*ip);  break;
    case 'd': uprinti(up, *ip);           break;
    case 'u': uprintu(up, *ip);           break;
    case 'x': uprintx(up, *ip);  break;
    }
    cp++; ip++;
  }
}

int uprintf(char *fmt, ...)
{
  int *ip;
  char *cp;
  cp = fmt;
  ip = (int *)&fmt + 1;

  UART *up = upp[0];

  while(*cp){
    if (*cp != '%'){
      uputc(up, *cp);
      if (*cp=='\n')
	uputc(up, '\r');
      cp++;
      continue;
    }
    cp++;
    switch(*cp){
    case 'c': uputc(up, (char)*ip);      break;
    case 's': uprints(up, (char *)*ip);   break;
    case 'd': uprinti(up, *ip);           break;
    case 'u': uprintu(up, *ip);           break;
    case 'x': uprintx(up, *ip);  break;
    }
    cp++; ip++;
  }
}

int sgetline(char *s)
{
    if (sline==0){
      kprintf("in sgetline sline=%d ", sline);
      while(sline==0);
    }
    while(1){
      *s = sbuf[stail++];
      sdata--; sroom++;
      if (*s == 0){
        sline--;
	return;
      }
      s++;
    }
}
