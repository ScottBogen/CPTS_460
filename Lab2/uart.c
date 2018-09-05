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

#include <stdarg.h>     // for variable arguments used in fuprintf

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

  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x1000A000 + i*0x1000);     // A000, B000, C000
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
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

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != 13){
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

int uprinti(UART *up, int i)      // ex: 580    0  8  5
{
  int arr[5];
  int j;
  if (i<0) { uputc(up, '-'); }
  if (i==0) { uputc(up, '0'); return 1;}

  j=0;
  while (i != 0) {
    arr[j++] = i%10;
    i/=10;
  }
  while (j>=0) {
    uputc(up, arr[j]);
    j--;
  }
}

int fuprintf(UART *up, char *fmt, ...) {
  int flag;
  va_list args;
  int i;
  i=0;
  flag = 0;

  va_start(args, fmt);

  while(*fmt) {
    if (flag) {
      switch(*fmt){
        case 'd':     //integer
          uprinti(up, va_arg(args, int));
          break;
        case 'x':     // unsigned int in HEX
          break;
        case 's':     // string
          uprints(up, va_arg(args, char*));
          break;
        case 'c':     // char
          uputc(up, va_arg(args, char));
          break;
        default:
          break;
      }
      i++;
      flag = 0;
    }
    else if (*fmt!='%'){
      uputc(up, *fmt);
    }

    if (*fmt=='%') { flag = 1; }

    *fmt++;
  }
  va_end(args);
}
