/*******************************************************
*                      t.c file                        *
*******************************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
#include <string.h>
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
GD    *gp;
DIR   *dp;
INODE *ip;


char buf1[BLK], buf2[BLK];
u8 ino;
int color = 0x0A;

int prints(char *s);
int gets(char *s);
void search(INODE *ip);
void printdirs(char* cp, DIR * dp, char* buf1);


int main()
{
  u16    i, iblk;
  char   c, temp[64];
  char *cp;

  prints("read block# 2 (GD)\n\r");
  getblk(2,buf1);         // get block #2: Group Descriptor, put in buf
  gp = (GD *)buf1;        // GD pointer points to GD
  iblk = gp->bg_inode_table;    // iblk is set to the inode table block
  prints("inode_block = ");
  putc(iblk+'0');     // 5
  prints("\n\r");

  getblk(iblk,buf1);  // iblk stored in buf1
  ip = (INODE*)buf1+1;   // inode pointer IP stores inode structure of iblk

  search(ip);

  // we know iblk's block 0 has some block contents in it (returned 21)
  ip = ip->i_block[0];    // forward ip
  getblk(ip, buf1);       // get i_block[0] of direct block #0
  dp = (DIR *)buf1;
  cp = buf1;

  // print names until boot
  printdirs(cp,dp,buf1);

  prints("searching....\n\r");

  // block = (ino-1) / 8 + 5 = 6
  // position = (ino-1) % 8 = 4
  getblk(6,buf1);
  ip = (INODE *)buf1 + 4;

  search(ip);

  ip = ip->i_block[0];
  getblk(ip,buf1);
  dp = (DIR *)buf1;
  cp = buf1;

  printdirs(cp,dp,buf1);

  prints("exit..");
}

void printdirs(char* cp, DIR * dp, char* buf1) {
  while(cp < buf1 + BLK){
     prints(dp->name);
     if(strstr(dp->name, "boot") != NULL) {
       prints(" <-- boot here");
       ino = dp->inode;
     }
     prints("\n\r");
     cp += dp->rec_len;
     dp = (DIR *)cp;
     getc();
  }
}

void search(INODE *ip) {
  int i;
  for (i=0; i<15; i++){
    if (ip->i_block[i]) {
      prints("i_block[");
      putc(i+'0');
      prints("] has contents\n\r");
    }
  }
}

int prints(char *s)
{
  while(*s!='\0') {
    putc(*s++);
  }
  return 1;
}

int getblk(u16 blk, char *buf)
{
  // readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
  readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}
