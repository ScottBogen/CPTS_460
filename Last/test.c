/********** test.c file *************/
#include "ucode.c"

char buf[1024];

//char** names = {".", "..", "lost+found", "bin", "boot", "dev", "etc", "f", "f1", "sys", "user", "usr", NULL};
char* names[5];

int main(int argc, char *argv[ ]) {

  struct stat mystat, *sp;

  int r;
  char *s;
  char filename[1024], cwd[1024];

  names[0] = ".";
  names[1] = "..";
  names[2] = "bin";
  names[3] = "f1";
  names[4] = NULL;


  getcwd(cwd);

  s = argv[1];
  if (argc == 1) {
    s = cwd;
  }

  sp = &mystat;

  int i = 0;
  while(names[i] != NULL) {
    if ((r = stat(names[i],sp)) < 0) {
      prints("cannot stat\n\r"); return -1;
    }

    ls_entry(names[i], sp);
    i++;
  }
}

int ls_entry(char* name, struct stat *sp){
  printf("%x  ", sp->st_mode);
  printf("%d  ", sp->st_nlink);
  printf("%d  ", sp->st_uid);
  printf("%d  ", sp->st_gid);
  printf("%d  ", sp->st_size);
  printf("    %s\n\r", name);
}
