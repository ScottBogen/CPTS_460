/********** login.c file *************/

/*
  passwd file:

  root:12345:0:0:super user:/:sh\n
  kcw:abcde:1:1:kcw user:/user/kcw:sh\n
  ...

  username:password:uid:gid:alias mode:directory:execcmd

*/

#include "ucode.c"
char buf[1024];

// upon entry, argv[0]=login, argv[1]=/dev/ttyX  (virtual console)
int main(int argc, char *argv[ ]) {
  int i;
  int in, out, err;
  int passfd, n;

  char* token;
  char name[128];
  char pass[128];

  if (argc < 2) {
    printf("ERRNO [0]: Fewer than 2 arguments.\n");
    printf("Sample usage: login /dev/tty0\n");
    return -1;
  }

  // 1. close file descriptors 0,1 inherited from INIT
  close(0);
  close(1);

  // 2. open argv[1] 3 times as in(0), out(1), err(2)
  in =  open(argv[1], 0);
  out = open(argv[1], 1);
  err = open(argv[1], 2);

  // 3. settty(argv[1]); // set tty name string in PROC.tty
  settty(argv[1]);

  // 4. open /etc/passwd file for READ
  passfd = open("/etc/passwd", O_RDONLY);
  while (1) {
    // 5.  printf("login"); gets(name);
    //     printf("pass"); gets(pass);
    prints("login:\n\r"); gets(name);
    prints("password:\n\r"); gets(pass);
    prints("%s\n\r");

    n = read(passfd, buf, 4096);

    if (n <= 0) {
      prints("ERRNO [1]: no passwords found.\n");
      close(passfd); close(in); close(out); close(err);
      return -1;
    }

    int toki = 0;
    char token[256];
    int result = strtok(buf, token, 10, toki++);  // token = username
    if (!result) { printf("nothing found!\n\r"); return -1; }

    // for each line in /etc/passwd do:
    while (token) {
      char token_name[32];
      strtok(token, token_name, ':', 0);
      if (!strcmp(token_name,name)) {
        char token_pass[32];
        strtok(token, token_pass, ':', 1);
        printf("pass = %s\n\r", token_pass);
        if (!strcmp(token_pass, pass)) {
          //username:password:uid:gid:alias mode:directory:execcmd
          char uid[16];
          char gid[16];
          char am[32];
          char alias[16];
          char mode[16];
          char directory[16];
          char execcmd[16];

          strtok(token, uid, ':', 2);    // uid
          strtok(token, gid, ':', 3);  // gid
          strtok(token, am, ':', 4);   // alias_mode
          strtok(am, mode, ' ', 1);  // mode
          strtok(am, alias, ' ', 0); // alias
          strtok(token, directory, ':', 5);  // directory
          strtok(token, execcmd, ':',6); // execcmd

          //       change uid, gid to users uid, gid
          //       change cwd to user's home dir
          //       close opened /etc/passwd file
          //       exec to program in user account

          printf("uid=%s, gid=%s, alias=%s, mode=%s, dir=%s, cmd=%s\n",
                  uid, gid, alias, mode, directory, execcmd);

          chuid(atoi(uid), atoi(gid));        //uid, gid
          chdir(directory);

          close(passfd); //close(1); close(2); close(3);

          printf("Welcome %s. Taking you to %s\n\r", alias, directory);
          exec(execcmd);
          return 1;
        }
      }

      strtok(buf, token, 10, toki++);
      if (toki>5) { // hard coded for now
        printf("login failed\n\r");
        return -1;
      }
    }
  }
  printf("exit\n");
}
