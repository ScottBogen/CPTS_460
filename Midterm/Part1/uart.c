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
#define DR   0x00
#define FR   0x18

#define RXFE 0x10
#define TXFF 0x20

typedef struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init()
{
  int i; UART *up;

      //for realview-pbx-a9
  uart[0].base = (char *)(0x10009000);
  uart[1].base = (char *)(0x1000A000);
  uart[2].base = (char *)(0x1000B000);
  uart[3].base = (char *)(0x1000C000);


  uart[0].n = 0;
  uart[1].n = 1;
  uart[2].n = 2;
  uart[3].n = 3;

        // for versatilepb:     ARM PL011
  /*
  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x101F1000 + i*0x1000);     // A000, B000, C000
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
  */
}

int ugetc(UART *up)
{
  while (*(up->base + FR) & RXFE);
  return *(up->base + DR);
}

int uputc(UART *up, char c)
{
  while(*(up->base + FR) & TXFF);
  *(up->base + DR) = c;
}

int ugets(UART *up, char *s)          // s is an address, remember this or you'll be in trouble
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}

int uprinti(UART *up, int i)
{
  if (i<0) { uputc(up, '-'); i*=-1; }
  if (i==0) { uputc(up, '0'); return 1;}

  uprintu(up, i);
}

int uprintu(UART *up, int i)
{
  if (i==0){ uputc(up, '0'); }
  else {
    upru(up, i);
  }
}

int upru(UART *up, int i) {
  char c;
  if (i) {
    c = (i%10) + '0';
    upru(up, i/10);
  }
  uputc(up,c);
}

int fuprintf(UART *up, char *fmt, ...) {

  char *cp = fmt;
  int *ip = (int *)&fmt + 1;

  // until end of str
  while (*cp) {


    if (*cp != '%') {
      uputc(up, *cp);
      if (*cp=='\n') {
        uputc(up, '\r');
      }
      cp++;
      continue;
    }
    // else handle %_
    cp++;

    switch(*cp) {
      case 'c':
        uputc(up,(char)*ip);
        break;
      case 's':
        uprints(up,(char *)*ip);
        break;
      case 'u':
        uprintu(up,(u32)*ip);
        break;
      case 'd':
        uprinti(up,(int)*ip);
        break;
    }

    cp++;
    ip++;
  }
}
