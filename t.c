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
INODE *ip;
DIR   *dp;


char buf1[BLK], buf2[BLK];
u8 ino;
int color = 0x0A;

int prints(char *s);
int gets(char *s);

int main()
{
  u16    i, iblk;
  char   c, temp[64];
  char *cp;

  // 1. WRITE YOUR CODE to get iblk = bg_inode_table block number
  prints("read block# 2 (GD)\n\r");
  getblk(2,buf1);         // get block #2: Group Descriptor, put in buf
  gp = (GD *)buf1;        // GD pointer points to GD
  iblk = gp->bg_inode_table;    // iblk is set to the inode table block
  prints("inode_block = ");
  putc(iblk+'0');     // 5
  prints("\n\r");

  // 2. WRITE YOUR CODE to get root inode
  getblk(iblk,buf1);  // iblk stored in buf1
  ip = (INODE*)buf1+1;   // inode pointer IP stores inode structure of iblk

  // look at all 15 block pointers of IP
  for (i=0; i<15; i++){
    if (ip->i_block[i]) {
      prints("i_block[");
      putc(i+'0');
      prints("] has contents\n\r");
    }
  }

  // we know iblk's block 0 has some block contents in it (returned 21)
  ip = ip->i_block[0];    // forward ip
  getblk(ip, buf1);       // get i_block[0] into buf1

  dp = (DIR *)buf1;
  cp = buf1;

  // print names until boot
  while(cp < buf1 + BLK){
     prints(dp->name);
     if(strstr(dp->name, "boot") != NULL) {
       prints("<--- boot here\n\r");
       break;
     }
     prints("\n\r");
     getc();
     cp += dp->rec_len;
     dp = (DIR *)cp;
  }

  // found boot at cp address
  // get that block in, point to the inode, look for mtx
  ino = dp->inode;    // inode #13
  ip = ip->i_block[ino];
  getblk(ip,buf1);
  prints("searching....\n\r");
  dp = (DIR *)buf1;
  cp = buf1;

  while(cp < buf1 + BLK){
     prints(dp->name);
     prints("\n\r");
     getc();
     cp += dp->rec_len;
     dp = (DIR *)cp;
  }

  prints("exit..");
  return 0;
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
