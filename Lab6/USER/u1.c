#include "ucode.c"

main(int argc, char *argv[])
{
  int i, a,b,c, pid, ppid, mode, r;
  u32 usp, usp1;
  char line[64]; char uc;

  a = 123; b = 234; c = 345;
  mode = getcsr();
  mode = mode & (0x0000001F);
  printf("CPU mode=%x argc=%d\n", mode, argc);
  for (i=0; i<argc; i++){
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  
  while(1){
    pid  = getpid();
    ppid = getppid();
    printf("This is process %d in Umode at %x parent=%d\n", 
	   pid, getPA(), ppid);
    //printf("mysp=%x\n", getmysp());

    umenu();
    printf("input a command : ");
    ugetline(line); 
    uprintf("\n"); 
 
    if (strcmp(line, "ps")==0)
      ups();
    if (strcmp(line, "chname")==0)
       uchname();
    if (strcmp(line, "kfork")==0)
       ukfork();
    if (strcmp(line, "switch")==0)
       uswitch();
    if (strcmp(line, "exit")==0)
       uexit();

    if (strcmp(line, "wait")==0)
       uwait();
    if (strcmp(line, "fork")==0)
       ufork();
    if (strcmp(line, "exec")==0)
       uexec();

  }
}
