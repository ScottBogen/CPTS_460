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


	.text
.code 32
.global reset_handler
.global vectors_start
.global vectors_end
.global proc
.global procsize
.global tswitch, scheduler, running, goUmode
.global inton, getcsr
.global switchPgdir
.global int_off, int_on

//.set Mtable, 0x4000  // level-1 page table must be at 16K boundary
/********************************************************
mode:	USER: 10000  0x10
	FIQ : 10001  0x11
	IRQ : 10010  0x12
	SVC : 10011  0x13
        ABT : 10111  0x17
	UND : 11011  0x1B
	SYS : 11111  0x1F
********************************************************/
reset_handler:
  /* KCW: set up MM try ID map sections first */
  // disable MMU : not really needed
  mrc p15, 0, r2, c1, c0, 0
  bic r2, r2, #0x00000000
  mcr p15, 0, r2, c1, c0, 0
  nop
  nop
  nop
/*********L1 section entry ***********************************
 |3            2|1|1111111|11|0|0000|0|00|00
 |1            0|9|8765432|10|9|8765|4|32|10|
 |     addr     | |       |AP|0|DOM |1|CB|10|
 |              |000000000|01|0|0000|1|00|10|
                           KRW  dom0
                          0x 4    1      2
	FIRST TRY: AP=11 for both KU r/w ==> 0xC12
*************************************************************/
// first, ID map all 4096 entries
 // mov r0, #Mtable          // r0 = 0x4000 at 16KB

 ldr r0, Mtable
// actually, Versatilepb, arm926ej-s only have 128MB RAM + 2MB I/O at 256MB,
// so the first 258 entries should be sufficient, but mov r1, #value
// must use a value accepatable to the mov # instructon, so try 512 or 260
//  mov r1, #4096            // 4096 will certainly work
  mov r1, #1024              // OK; 512 entries should be enough
  mov r2, #0x100000          // r2=1M
  // mov r3, #(0x01 << 10)   // r3=AP=01 (KRW, user no) AP=11: both KU r/w
  mov r3, #(0x11 << 10)      // r3=AP=01 (KRW, user no) AP=11: both KU r/w
  orr r3, r3, #0x12          // r3 = 0x412 OR 0xC12 if AP=11: used 0xC12
  //mov r3, #0x412 // should be SAME as this but not allowed by ARM's # value
1:
  str r3, [r0],#0x04         // store r3 to [r0]; inc r0 by 4
  add r3, r3, r2             // inc r3 by 1M
  subs r1,r1,#1              // r1--
  bgt 1b                     // loop r1=4096 times

///******** do NOT map kernel VA, so that VA=low PA ********************

// map va 0x80000000 to 0x0
  //mov r0, #Mtable          // r0=0x4000 again
  ldr r0, Mtable             // r0=0x4000 again

  add r0, r0,#(2048*4)       // add 8K = 2048th entry
  mov r2, #0x100000
  mov r1, #2                 // 2 I/O 1MB pages at 256MB
  ldr r3, VAbase             // r3 contains 0x00000C12
3:
  str r3, [r0],#0x04         // store r3 to [r0]; inc r0 by 4
  add r3, r3, r2             // inc r3 by 1M
  subs r1,r1, #1             // r1--
  bgt 3b                     // loop r1=128 times
/**********
// 2 I/O sections at 0x8000000+256MB
  //mov r0, #Mtable          // r0=0x4000 again
  ldr r0, Mtable
  add r0, r0,#((2048+256)*4) // add 1KB = 256th entry : I/O at 256MB
  mov r2, #0x100000
  mov r1, #2                 // 2 entries for 2MB 0x10000000-0x10200000
  ldr r3, IObase             // r3 = 0x10000C20
4:
  str r3, [r0],#0x04         // store r3 to [r0]; inc r0 by 4
  add r3, r3, r2             // inc r3 by 1M
  subs r1,r1,#1              // r1--
  bgt 4b                     // loop r1=2 times
**************************************************/
 // mov r0, #Mtable
  ldr r0, Mtable
  mcr p15, 0, r0, c2, c0, 0  // set TTBase
  mcr p15, 0, r0, c8, c7, 0  // flush TLB

  // set domain: all 01=client(check permission) 11=master(no check)
  mov r0,#0x3                // 11 for MASER
  mcr p15, 0, r0, c3, c0, 0
/************
// enable MMU later AFTER set up the rest here
  mrc p15, 0, r0, c1, c0, 0
  orr r0, r0, #0x00000001     // set bit0
  mcr p15, 0, r0, c1, c0, 0   // write to c1
  nop
  nop
  nop
  mrc p15,0, r2, c2, c0
  mov r2, r2
************/
// change to SVC mode

  /* get Program Status Register; this is the RESET mode SR */
  MRS r0, cpsr
  mov r2, r0         // save RESET mode cpsr in r2

  BIC r1, r0, #0x1F  // r1 = r0 = cspr's lowest 5 bits cleared to 0
  ORR r1, r1, #0x13  // OR in 0x13=1011 = SVC mode
  MSR cpsr, r1       // write to cspr, so in SVC mode now

  /* set SVC stack to HIGH END of proc[0].kstack[] */
  LDR r0, =proc      // r0 points to proc's
  LDR r1, =procsize  // r1 -> procsize
  LDR r2,[r1,#0]     // r2 = procsize
  ADD r0, r0, r2     // r0 -> high end of proc[0]
  MOV sp, r0

  /* go in IRQ mode to set IRQ stack and enable IRQ interrupts */
  BIC r1, r0, #0x1F  // r1 = r0 = cspr's lowest 5 bits cleared to 0
  ORR r1, r1, #0x12  // OR in 0x12=1010 = IRQ mode
  MSR cpsr, r1       // write to cspr, so in IRQ mode now
  // LDR sp, =irq_stack_top  // IS THIS REALLY NEEDED? WHY not just proc's kstack?

  /* Enable IRQs */
  BIC r0, r0, #0x80  // set r0=cspr.I bit to 0 to unmask IRQ interrupts

  /* go back in SVC mode */
  mov r0, r2        // restore r0 to SVC cpsr
  BIC r1, r0, #0x1F  // r1 = r0 = cspr's lowest 5 bits cleared to 0
  ORR r1, r1, #0x13  // OR in 0x13=1011 = SVC mode
  MSR cpsr, r1       // write to cspr, so in SVC mode now

  // KCW: set previous mode of SVC mode to USER mode
  MRS r0, spsr       // get previous mode SR in r0
  BIC r1, r0, #0x1F   @ clear lowest 5 bits
  ORR r1, r1, #0x10   @ 10000 = USERMODE
  MSR spsr, r1       // write to previous mode spsr

  // still in SVC mode
  mov r0, r2          // restore r0 to SVC cpsr
  BIC r0, r0, #0xC0   // I and F bits=0 enable IRQ,FIQ
  MSR cpsr, r0        // I and F interrupts are enables

  /* copy vector table to address 0 */
  BL copy_vectors

// enable MMU
  mrc p15, 0, r0, c1, c0, 0
  orr r0, r0, #0x00000001     // set bit0
  mcr p15, 0, r0, c1, c0, 0   // write to c1
  nop
  nop
  nop
  mrc p15,0, r2, c2, c0
  mov r2, r2

  /* jump to main */
  //  BL main

  //ldr r1, mainstart
  //mov pc, r1

  adr r0, mainstart
  //ldr r0, mainstart
  ldr pc, [r0]

  B .

.align 4
Mtable:	    .word 0x4000
IObase:	    .word 0x10000C12
VAbase:	    .word 0x900C12
mainstart:  .word main

.align 4

irq_handler:
  sub	lr, lr, #4   // ARM's linkReg must be -4; if write irq_handler() with
	             // __attribute__((interrupt))svc_handler(),then no need
  stmfd	sp!, {r0-r10, fp, ip, lr}  // save all Umode regs in kstack

  bl	IRQ_handler  // call irq_handler() in C in svc.c file

  ldmfd	sp!, {r0-r10, fp, ip, pc}^ // pop from kstack but restore Umode SR

data_handler:

  sub	lr, lr, #4
  stmfd	sp!, {r0-r10, fp, ip, lr}

  bl	DATA_handler

  ldmfd	sp!, {r0-r10, fp, ip, pc}^


tswitch: // tswitch() in Kmode
//       1  2  3  4  5  6  7  8  9  10  11  12  13  14
//       ---------------------------------------------
// stack=r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
//       ---------------------------------------------
  stmfd	sp!, {r0-r10, fp, ip, lr}

  LDR r0, =running // r0=&running
  LDR r1, [r0,#0]  // r1->runningPROC
  str sp, [r1,#4]  // running->ksp = sp

  bl	scheduler

  LDR r0, =running
  LDR r1, [r0,#0]     // r1->runningPROC
  lDR sp, [r1,#4]     // sp = running->ksp

  ldmfd	sp!, {r0-r10, fp, ip, pc}

klr:	.word 0
// XIA
svc_entry: // r4=usp, r5=pc before swi in Umode
// KCW: save r0 also ==> in kfork(), must use

   stmfd sp!, {r0-r12, lr}
// NOE: this saved lr returns to svc entry, NOT the Umode PC at syscall.

   ldr r5, =running   // r5=&running
   ldr r6, [r5, #0]   // r6 -> PROC of running

   mrs r7, spsr
   str r7, [r6, #16]

// ksp needed in fork()
   str sp, [r6, #4]   // save ksp into PROC.ksp at offset 4

// get usp=r13 from USER mode
   mrs r7, cpsr       // r7 = SVC mode cpsr
   mov r8, r7         // save a copy in r8
   orr r7, r7, #0x1F  // r7 = SYS mode
   msr cpsr, r7       // change cpsr in SYS mode

// now in SYS mode, r13 same as User mode sp r14=user mode lr
   str sp, [r6, #8]   // save usp into proc.usp at offset 8
   str lr, [r6, #12]  // save Umode PC into proc.ups at offset 12

// change back to SVC mode
   msr cpsr, r8
/*
// saved lr in kstack return to svc entry, NOT Umode PC at syscall
// replace saved lr in kstak with Umode PC at syscall
   mov r11, sp
   add r11, r11, #52    // offset = 13*4 bytes from sp
   ldr r4, [r6, #12]
   str r4, [r11]
*/
// enable interrupts
   MRS r7, cpsr
   BIC r7, r7, #0xC0  // I and F bits=0 enable IRQ,FIQ
   MSR cpsr, r7

  bl	svc_handler

 // replace saved r0 on stack with the return value r from svc_handler()
   add sp, sp, #4      // effectively pop saved r0 off stack
   stmfd sp!,{r0}     // push r as the saved r0 to Umode

goUmode:
// later: may have switched process ==>
// must restore saved usp from the NEW PROC.usp
// temporarily mov CPU to SYS mode; load running->usp to Umode sp;

   ldr r5, =running   // r5=&running
   ldr r6, [r5, #0]   // r6 -> PROC of running

   ldr r7, [r6, #16]
   msr spsr, r7

   // set cpsr to SYS mode to access user mode sp
   mrs r2, cpsr       // r2 = SVC mode cpsr
   mov r3, r2         // save a copy in r3
   orr r2, r2, #0x1F  // r0 = SYS mode
   msr cpsr, r2       // change cpsr in SYS mode

// now in SYS mode
   ldr sp, [r6, #8]   // restore usp from PROC.usp

// back to SVC mode
   msr cpsr, r3       // back to SVC mode

// ^: pop regs from kstack BUT also copy spsr into cpsr ==> back to Umode
   ldmfd sp!, {r0-r12, pc}^ // ^ : pop kstack AND to previous mode

int_on:
  MRS r0, cpsr
  BIC r0, r0, #0x80
  MSR cpsr, r0
  mov pc,lr

int_off:
  MRS r0, cpsr
  ORR r0, r0, #0x80
  MSR cpsr, r0
  mov pc,lr

getcsr:
   mrs r0, cpsr
   mov pc, lr

vectors_start:
  LDR PC, reset_handler_addr
  LDR PC, undef_handler_addr
  LDR PC, svc_handler_addr
  LDR PC, prefetch_abort_handler_addr
  LDR PC, data_abort_handler_addr
  B .
  LDR PC, irq_handler_addr
  LDR PC, fiq_handler_addr

reset_handler_addr:          .word reset_handler
undef_handler_addr:          .word undef_handler
svc_handler_addr:            .word svc_entry
prefetch_abort_handler_addr: .word prefetch_abort_handler
//data_abort_handler_addr:     .word data_abort_handler
data_abort_handler_addr:     .word data_handler
irq_handler_addr:            .word irq_handler
fiq_handler_addr:            .word fiq_handler

vectors_end:

switchPgdir:	// switch pgdir to new PROC's pgdir; passed in r0
  // r0 contains address of PROC's pgdir address
  mcr p15, 0, r0, c2, c0, 0  // set TTBase
  mov r1, #0
  mcr p15, 0, r1, c8, c7, 0  // flush TLB
  mcr p15, 0, r1, c7, c10, 0  // flush TLB
  mrc p15, 0, r2, c2, c0, 0

  // set domain: all 01=client(check permission) 11=master(no check)
  mov r0, #0x3                // 11 for MASER
  mcr p15, 0, r0, c3, c0, 0

  mov pc, lr                  // return

.end

// KCW: to get user mode registers in kmode:
//(1). change cpsr to SYS mode, get r11 or any other (banked) regs, r13,14
//     switch cpsr back to kmode, OR seems a simpler way by
//(2). stmfd sp,{regs}^, which pushes umode regs to kstack, then pop off kstack
//
// get user mode r13 (sp) by the same technique
