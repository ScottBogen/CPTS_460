/****************
qemu with -kernel file.bin ==> start to execute from 0x10000 ==>
file.bin must be loaded to 0x10000 ==> . = 0x10000
************/
/*ENTRY(vectors_start)*/
ENTRY(reset_handler)
SECTIONS
{
  . = 0x10000;
  .text : {
      ts.o
      t.o
 }
 .data : {
      ts.o(.data)
      t.o (.data)
  }
 .bss : { *(.bss) }

  . = ALIGN(8);
  . = . + 0x1000; /* 4kB of stack memory */
  stack_top = .;
}
