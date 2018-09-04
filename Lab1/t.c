/*******************************************************
*                  @t.c file                          *
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
u16 NSEC = 2;
u32* up;

int prints(char *s);
int gets(char *s);
void search(INODE *ip);
void printdirs(char* cp, DIR * dp, char* buf1);

int prints(char *s)
{
  while(*s!='\0') {
    putc(*s++);
  }
  return 1;
}

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
}

void printdirs(char* cp, DIR * dp, char* buf1) {
  while(cp < buf1 + BLK){
     prints(dp->name);
     if((strstr(dp->name, "boot") != NULL) || strstr(dp->name,"mtx") != NULL) {
       prints(" <-- boot here");
       ino = dp->inode;
     }
     prints("\n\r");
     cp += dp->rec_len;
     dp = (DIR *)cp;
     getc();
  }
}

main()
{
   u16    i, iblk;
   char   c, temp[64];
   char *cp;

   getblk(2,buf1);         // get block #2: Group Descriptor, put in buf
   gp = (GD *)buf1;        // GD pointer points to GD
   iblk = gp->bg_inode_table;    // iblk is set to the inode table block

   getblk(iblk,buf1);  // iblk stored in buf1
   ip = (INODE*)buf1+1;   // inode pointer IP stores inode structure of iblk
   //search(ip);

   // we know iblk's block 0 has some block contents in it (returned 21)
   ip = ip->i_block[0];    // forward ip
   getblk(ip, buf1);       // get i_block[0] of direct block #0
   dp = (DIR *)buf1;
   cp = buf1;

   // print names until boot
   printdirs(cp,dp,buf1);
   // block = (ino-1) / 8 + inode_table (5) = 6
   // position = (ino-1) % 8 = 4
   getblk(6,buf1);
   ip = (INODE *)buf1 + 4;

   //search(ip);

   ip = ip->i_block[0];
   getblk(ip,buf1);
   dp = (DIR *)buf1;
   cp = buf1;
   printdirs(cp,dp,buf1);
   // block = (ino-1) / 8 + inode_table (5) = 9                 ino = 34
   // position = (ino-1) % 8 = 1
   getblk(9,buf1);
   ip = (INODE *)buf1 + 1;

   //search(ip);
   setes(0x1000);  // MTX loading segment = 0x1000

   for (i=0; i<12; i++){
      getblk((u16)ip->i_block[i], 0);
      putc('*');
      inces();
      getc();
   }

   if (ip->i_block[12]){
     prints("True");
     up = (u32 *)buf1;
     while(*up){
        getblk((u16)*up, 0); putc('.');
        inces();
        up++;
     }
  }

  prints("go?"); getc();
}
