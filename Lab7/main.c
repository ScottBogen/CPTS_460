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


MINODE *iget();

#include "util.c"


/******* WRITE YOUR OWN util.c and others ***********
#include "util.c"
#include "cd_ls_pwd.c"
***************************************************/
int init()
{
  int i;
  MINODE *mip;
  PROC   *p;

  printf("init()\n");

  for (i=0; i<NMINODE; i++){
      mip = &minode[i];
      // set all entries to 0, except refCount=1
  }
  for (i=0; i<NPROC; i++){
       p = &proc[i];
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

  get_block(dev, 2, buf);
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  iblk = inode_start = gp->bg_inode_table;

  // print bmap, imap, inodes_start

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

    if (strcmp(cmd, "ls")==0)
       //ls(pathname);

    if (strcmp(cmd, "cd")==0)
       //chdir(pathname);

    if (strcmp(cmd, "pwd")==0)
       print(root);

    if (strcmp(cmd, "quit")==0)
       quit();
  }
}

int print(MINODE *mip)
{
  int blk;
  char buf[1024];
  int i;
  DIR *dp;
  char *cp;

  INODE *ip = &mip->INODE;
  for (i=0; i < 12; i++){
    if (ip->i_block[i]==0)
      return 0;
    get_block(dev, ip->i_block[i], buf);
    dp = (DIR *)buf; cp = buf;
    while(cp < buf+1024){
      printf("%d %d %d %s\n", dp->inode, dp->rec_len, dp->name_len, dp->name);
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
