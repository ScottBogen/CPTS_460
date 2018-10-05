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
.global tswitch, scheduler, running
.global int_off, int_on, lock, unlock
	
reset_handler:
/* set Supervisor stack */
  // LDR sp, =stack_top // if P0 uses SVC mode stack in t.ld
  LDR r0, =proc
  LDR r1, =procsize
  LDR r2,[r1,#0]
  ADD r0, r0, r2
  MOV sp, r0
	
/* copy vector table to address 0 */
  BL copy_vectors
/* get Program Status Register */
  MRS r0, cpsr
/* go in IRQ mode */
  BIC r1, r0, #0x1F
  ORR r1, r1, #0x12
  MSR cpsr, r1
/* set IRQ stack */
  LDR sp, =irq_stack
/* Enable IRQs */
  BIC r0, r0, #0x80
/* go back in Supervisor mode */
  MSR cpsr, r0
/* call main */
  BL main
  B .

.align 4
irq_handler:

  sub	lr, lr, #4
  stmfd	sp!, {r0-r10, fp, ip, lr}

  bl	IRQ_handler  

  ldmfd	sp!, {r0-r10, fp, ip, pc}^

tswitch:
//       1  2  3  4  5  6  7  8  9  10  11  12  13  14
//       ---------------------------------------------
// stack=r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,r14
//       ---------------------------------------------
// disable IRQ interrupts : r0 is FREE for use
  MRS r0, cpsr
  ORR r0, r0, #0x80   // set bit means MASK off IRQ interrupt 
  MSR cpsr, r0

  stmfd	sp!, {r0-r12, lr}

  LDR r0, =running // r0=&running
  LDR r1, [r0,#0]  // r1->runningPROC
  str sp, [r1,#4]  // running->ksp = sp

  bl	scheduler

  LDR r0, =running
  LDR r1, [r0,#0]     // r1->runningPROC
  lDR sp, [r1,#4]

// enable IRQ interrupts : r0 is FREE for use
  MRS r0, cpsr
  BIC r0, r0, #0x80   // clear bit means UNMASK IRQ interrupt
  MSR cpsr, r0
	
  ldmfd	sp!, {r0-r12, pc}
	
// int_on()/int_off(): turn on/off IRQ interrupts
int_off: // may pass parameter in r0
  MRS r0, cpsr
  mov r1, r0
  ORR r1, r1, #0x80   // set bit means MASK off IRQ interrupt 
  MSR cpsr, r1
  mov pc, lr	

int_on: // pass parameter in r0
  MSR cpsr, r0
  mov pc, lr	

lock: // mask out IRQ interrupts 
  MRS r0, cpsr
  ORR r0, r0, #0x80   // set bit means MASK off IRQ interrupt 
  MSR cpsr, r0
  mov pc, lr	

unlock: // unmask IRQ interrupts
  MRS r0, cpsr
  BIC r0, r0, $0x80
  MSR cpsr, r0
  mov pc, lr	

vectors_start:
  LDR PC, reset_handler_addr
  LDR PC, undef_handler_addr
  LDR PC, swi_handler_addr
  LDR PC, prefetch_abort_handler_addr
  LDR PC, data_abort_handler_addr
  B .
  LDR PC, irq_handler_addr
  LDR PC, fiq_handler_addr

reset_handler_addr:          .word reset_handler
undef_handler_addr:          .word undef_handler
swi_handler_addr:            .word swi_handler
prefetch_abort_handler_addr: .word prefetch_abort_handler
data_abort_handler_addr:     .word data_abort_handler
irq_handler_addr:            .word irq_handler
fiq_handler_addr:            .word fiq_handler

vectors_end:

	
.end
