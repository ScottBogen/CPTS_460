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


//void __attribute__((interrupt))svc_handler(int a, int b, int c, int d)

int svc_handler(volatile int a, int b, int c, int d)
{
  int r; int ocolor=color;
  int *usp;
  //color = RED;
  //  if (a<90)
  // kprintf("svc_handler: a=%d b=%d c=%x d=%x\n",a,b,c,d);
  switch(a){
     case 0: r = kgetpid(); break;
     case 1: r = kgetppid(); break;
     case 2: r = kps(); break;
     case 3: r = kchname((char *)b); break;
     case 4: r = kkfork(); break;
     case 5: r = ktswitch(); break;
     case 6: r = kkwait((int *)b); break;
     case 7: r = kexit(b); break;
     case 8: r = getusp(); break;
     case 9: r = fork(); break;
     case 10: r = exec((char *)b); break;
     case 90: r = kgetc() & 0x7F;  break;
     case 91: r = kputc(b); break;
     case 92: r = kgetPA(); break;
     default: printf("invalid syscall %d\n", a);
  }
  // inton();
  // kprintf("  svc_hamdler return ");
  color = ocolor;
  // kprintf("  r = %x\n", r); kgetc();
  // try to modify saved r0 in kstack: saved r0 is in
  // r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 r12 uLR
  // -14                                       -1
  running->kstack[SSIZE - 14] = r;
  return r; // return to to goUmode: which will replace r0 in Kstack with r
}
