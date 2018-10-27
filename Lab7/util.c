/*********************************************************************
        You MAY use the util.o file to get a quick start.
 BUT you must write YOUR own util.c file in the final project
 The following are helps of how to wrtie functions in the util.c file
*********************************************************************/


/************** util.c file ****************
#include 
#include 
#include 
#include 
#include 
#include 
#include 
/**** globals defined in main.c file ****/

#include "type.h"
#define NULL 0

extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern char gpath[128];
extern char *name[64];
extern int n;
extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
extern char line[256], cmd[32], pathname[256];


int get_block(int fd, int blk, char buf[BLKSIZE])
{
	lseek(fd, (long)(blk * BLKSIZE), 0);
	read(fd, buf, BLKSIZE);
}

int put_block(int fd, int blk, char buf[BLKSIZE])
{
	lseek(fd, (long)(blk * BLKSIZE), 0);
	write(fd, buf, BLKSIZE);
}

  // tokenize pathname into n components: name[0] to name[n-1];
void tokenize(char* pathname) {
    char* str;
    int i = 0;

    printf("tokenize pathname = %s\n", pathname);

    str = strtok(pathname, "/");

    while (str != NULL) {
        name[i++] = str;
        str = strtok(NULL, "/");
    }
}


MINODE *iget(int dev, int ino) {
  // return minode pointer to loaded INODE
  (1). Search minode[ ] for an existing entry with the needed (dev, ino):
       if found: inc its refCount by 1;
                 return pointer to this minode;

  (2). // needed entry not in memory:
       find a FREE minode (refCount = 0); Let mip-> to this minode;
       set its refCount = 1;
       set its dev, ino

  (3). load INODE of (dev, ino) into mip->INODE:
       
       // get INODE of ino a char buf[BLKSIZE]    
       blk    = (ino-1) / 8 + inode_start;
       offset = (ino-1) % 8;

       printf("iget: ino=%d blk=%d offset=%d\n", ino, blk, offset);

       get_block(dev, blk, buf);
       ip = (INODE *)buf + offset;
       mip->INODE = *ip;  // copy INODE to mp->INODE

       return mip;
}


int iput(MINODE *mip) // dispose a used minode by mip
{
 mip->refCount--;
 
 if (mip->refCount > 0) return;
 if (!mip->dirty)       return;
 
 // YOUR C CODE: write INODE back to disk
} 


// serach a DIRectory INODE for entry with given name
int search(MINODE *mip, char *name)
{
   // return ino if found; return 0 if NOT
}


// retrun inode number of pathname

int getino(char *pathname)
{ 
   // SAME as LAB6 program: just return the inode's ino;
}


int findmyname(MINODE *parent, u32 myino, char *myname) 
{
   // return name string of myino: SAME as search except by myino;
   // copy entry name (string) into myname[ ];
}


int findino(MINODE *mip, u32 *myino) 
{
  // fill myino with ino of . 
  // retrun ino of ..
}

