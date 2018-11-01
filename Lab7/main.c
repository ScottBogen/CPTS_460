/****************************************************************************
*                   mount_root Program                                      *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include "type.h"

MINODE minode[NMINODE];
MINODE *root;
PROC   proc[NPROC], *running;

char gpath[256]; // holder of component strings in pathname
char *name[64];  // assume at most 64 components in pathnames
int  n;

int  fd, dev;
int  nblocks, ninodes, bmap, imap, iblk, inode_start;
char line[256], cmd[32], pathname[256];

int ino;
char buf[BLKSIZE];

#include "util.c"


/******* WRITE YOUR OWN util.c and others ***********
#include "util.c"
#include "cd_ls_pwd.c"
***************************************************/
int init() {
  int i;
  MINODE *mip;
  PROC   *p;

  printf("init()\n");

  for (i=0; i<NMINODE; i++){
      mip = &minode[i];
      // set all entries to 0, except refCount=1
      mip->dev = 0;
      mip->ino = 0;
      mip->refCount = 1;
      mip->dirty = 0;
      mip->mounted = 0;
  }
  for (i=0; i<NPROC; i++){
       p = &proc[i];
       p->pid = i;
       p->uid = i;
       p->cwd = 0;
      // set pid = i; uid = i; cwd = 0;
  }
}

// load root INODE and set root pointer to it
int mount_root()
{
  char buf[BLKSIZE];
  SUPER *sp;
  GD    *gp;

  printf("mount_root()\n");
  /********** read super block at 1024 ****************/
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  /* verify it's an ext2 file system *****************/
  if (sp->s_magic != 0xEF53){
      printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
      exit(1);
  }
  printf("EXT2 FS OK\n");

  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;
  // print nblocks, ninodes
  printf("ninodes = %d   nblocks = %d\n", ninodes, nblocks);

  get_block(dev, 2, buf);
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  iblk = inode_start = gp->bg_inode_table;

  // print bmap, imap, inodes_start
  printf("bmap = %d   imap = %d   inodes_start = %d\n", bmap, imap, iblk);

  root = iget(dev, 2);       // get #2 INODE into minoe[ ]
  printf("mounted root OK\n");
}

char *disk = "mydisk";
int main(int argc, char *argv[ ])
{
  int ino;
  char buf[BLKSIZE];
  if (argc > 1)
     disk = argv[1];

  fd = open(disk, O_RDWR);
  if (fd < 0){
     printf("open %s failed\n", disk);
     exit(1);
  }
  dev = fd;

  init();
  mount_root();
  printf("root refCount = %d\n", root->refCount);

  printf("creating P0 as running process\n");
  running = &proc[0];
  running->status = READY;
  running->cwd = iget(dev, 2);
  printf("root refCount = %d\n", root->refCount);

  while(1){
    printf("input command : [ls|cd|pwd|quit] ");
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;

    if (line[0]==0)
       continue;
    pathname[0] = 0;

    sscanf(line, "%s %s", cmd, pathname);
    printf("cmd=%s pathname=%s\n", cmd, pathname);

    if (strcmp(cmd, "ls")==0) {
      ls_dir(pathname);
    }

    if (strcmp(cmd, "cd")==0) {
       cd(pathname);
    }

    if (strcmp(cmd, "pwd")==0) {
       pwd();
    }

    if (strcmp(cmd, "quit")==0) {
       quit();
    }
  }
}

int cd(char* path) {
  ino = getino(path);
  MINODE* mip = iget(fd, ino);

  if (path[0] == '\0') {    // no path
    printf("returning to root\n");
    running->cwd = iget(fd, 2);
    return 0;
  }

  if (mip->INODE.i_mode != DIRECTORY) {
    iput(mip);
    printf("Error cd: not a DIR --- %x", mip->INODE.i_mode);
    return -1;
  }

  running->cwd = mip;
  printf("cd to %s at ino %d\n", path, mip->ino);
  iput(mip);
}

int ls_dir(char* path) {
  char* cp;
  DIR* dp;

  if (!path) {
    path = "/";
  }

  ino = getino(path)
  MINODE* mip = iget(dev, ino);
  INODE* ip = &mip->INODE;

  get_block(ip->i_block[0], BLKSIZE, buf);

  cp = buf;
  dp = (DIR*) buf;

  // skip .
  cp += dp->rec_len;
  dp = (DIR*)cp;
  // skip ..
  cp += dp->rec_len;
  dp = (DIR*)cp;

  while (cp < buf + BLKSIZE) {
    if (dp->inode == 0) {
      return 0;
    }

    ls_file(dp->inode);
    printf("%20s\n", dp->name);

    cp += dp->rec_len;
    dp = (DIR*)cp;
  }
  return 1;
}

int ls_file(int ino) {
  MINODE* mip = iget(dev, ino);
  INODE* ip = &mip->INODE;

  // file type:
  if      (ip->i_mode == 0x41ed) { printf("d"); }
  else if (ip->i_mode == 0x81a4) { printf("r"); }
  else if (ip->i_mode == 0xa1ff) { printf("l"); }
  else                           { printf("-"); }

  char permissions[9] = "xwrxwrxwr";
  for (int i = 8; i >= 0; i--) {
    if (ip->imode & (1 << i)) { printf(permissions[i]); }
    else { printf("-"); }
  }

  printf("%4d", ip->i_links_count);
  printf("%2d", ip->i_uid);
  printf("%2d", ip->i_gid);


  char temp[32];
  ctime_r((time_t *)&mip->INODE.i_mtime, temp);   // asshole time
  temp[strlen(temp)-1] = 0;    // null terminate
  printf("%.*s", 12, temp+4);  // format time just like ls -l
  printf(" %6d", ip->i_size);

  iput(mip);

  return 1;
}



int pwd() {
  if (running->cwd->ino == 2) {
    printf("/\n");
  }

  rpwd(0, running->cwd);
  printf("\n");
}

int rpwd(int pino, MINODE* wd) {
  int temp_ino;
  char buf[BLKSIZE];
  DIR* dp;
  char* cp;

  MINODE* mip, *pmip;

  pmip = wd;
  get_block(fd, pmip->INODE.i_block[0], buf);

  // .
  dp = (DIR*)buf;
  cp = buf + dp->rec_len;

  // ..
  dp = (DIR*)cp;

  // call rpwd recursively until root
  if (wd->ino != root->ino) {
    temp_ino = dp->inode;
    mip = iget(fd, temp_ino);

    rpwd(pmip->ino, mip);
  }

  if (pino == 0) {
    return -1;
  }

  // hard coded for 12 entries, change later TODO
  for (int i = 0; i < 12; i++) {
    if (dp->inode != pino) {
      cp += dp->rec_len;
      dp = (DIR*)cp;
    }
  }

  // at position
  printf("%s", dp->name);

  // TODO: Bug where %s/ or even %s / will cause it to be /ir1
}


// supplementary but not the same as ls
int print(MINODE *mip) {
  int blk;
  char buf[1024];
  int i;
  DIR *dp;
  char *cp;
  printf("print()\n");

  INODE *ip = &mip->INODE;
  for (i=0; i < 12; i++){

    if (ip->i_block[i]==0) {
      return 0;
    }

    get_block(dev, ip->i_block[i], buf);
    dp = (DIR *)buf; cp = buf;

    printf("INODE\tRECLEN\tNAMELEN\tNAME\n");
    while(cp < buf+1024) {
      printf("%d\t%d\t%d\t%s\n", dp->inode, dp->rec_len, dp->name_len, dp->name);
      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }
}

int quit()
{
  int i;
  MINODE *mip;
  for (i=0; i<NMINODE; i++){
    mip = &minode[i];
    if (mip->refCount > 0)
      iput(mip);
  }
  exit(0);
}
