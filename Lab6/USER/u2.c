#include "ucode.c"

main(int argc, char *argv[ ])
{
  int pid, ppid, mode, r, usp;
  char line[64];
  int i;

  printf("argc=%d\n", argc);
  for (i=0; i<argc; i++)
    printf("argv[%d]=%s\n", i, argv[i]);
   
  while(1){
    pid = getpid();
    ppid = getppid();

    printf("DAS IST PROZESS %d IM USER-MODUS at %x ELTERN=%d\n", 
	   pid, getPA(), ppid);
 
    umenu();
    printf("BEFEHL : ");
    ugetline(line); 
    printf("\n");

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
