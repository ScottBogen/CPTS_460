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

//#include "type.h"
#define NULL 0

extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;
extern char gpath[256];
extern char *name[64];
extern int n;
extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
extern char line[256], cmd[32], pathname[256];

extern int ino;
extern char buf[BLKSIZE];

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

    //printf("tokenize pathname = %s\n", pathname);
    str = strtok(pathname, "/");

    while (str != NULL) {
        name[i++] = str;
        str = strtok(NULL, "/");
    }
}


// returns MINODE of the desired inode from integer number
MINODE *iget(int dev, int ino) {
	/*
	(1). Search minode[ ] for an existing entry with the needed (dev, ino):
			 if found: inc its refCount by 1;
								 return pointer to this minode;
	*/
	char buf[BLKSIZE];
	int i = 0;

	// search minode for existing entry
	for (int i = 0; i < 100; i++) {
		if (minode[i].ino == ino) {		// found
			minode[i].refCount++;
			return &minode[i];
		}
	}

	/*
	(2). // needed entry not in memory:
			 find a FREE minode (refCount = 0); Let mip-> to this minode;
			 set its refCount = 1;
			 set its dev, ino
		*/

	int index;
	for (int i = 0; i < 100; i++) {
		if (minode[i].ino == 0) {		// free minode
			index = i;
		}
	}

	// mailman
	int blk    = (ino-1) / 8 + inode_start;
	int offset = (ino-1) % 8;

	//printf("iget: ino=%d blk=%d offset=%d\n", ino, blk, offset);

	// load inode, set attributes
	get_block(dev, blk, buf);

  memcpy(&minode[index].INODE, ((INODE*)buf + offset), sizeof(INODE));

	minode[index].dev = dev;
	minode[index].dirty = 0;
	minode[index].ino = ino;
	minode[index].mounted = 0;
	minode[index].refCount = 1;

	return &minode[index];
}

// dispose a used minode by mip
int iput(MINODE *mip) {

	mip->refCount--;

	if (mip->refCount > 0) {
		return;
	}

	if (!mip->dirty) {
	 	return;
	}

	// YOUR C CODE: write INODE back to disk

	// mailman:
	int blk    = (ino-1) / 8 + inode_start;
	int offset = (ino-1) % 8;

  get_block(mip->dev, blk, buf);

	INODE* temp;
	temp = (INODE*) buf + offset;

	memcpy(temp, &mip->INODE, sizeof(INODE));
	put_block(fd, blk, buf);		// NOTE: Maybe use mip->dev instead of fd
}


// serach a DIRectory INODE for entry with given name
// return ino if found; return 0 if NOT
int search(MINODE *mip, char *name) {
	char* cp;
	DIR* dp;

	int ino = mip->ino;

	// 16 blocks in an inode
	for (int i = 0; i < 16; i++) {
		get_block(fd, mip->INODE.i_block[i], buf);
		cp = buf;
		dp = (DIR*) buf;

		while (cp < buf + BLKSIZE) {
			if (dp->inode == 0) {
				return 0;
			}
			if (!strncmp(name, dp->name, dp->name_len)) {
				return dp->inode;
			}

			cp += dp->rec_len;
			dp = (DIR*)cp;
		}
	}

	return 0;
}


// return inode number of pathname
// SAME as LAB6 program: just return the inode's ino;
int getino(char *pathname) {

	MINODE* mip;

	if(!strcmp(pathname, "/")) {
		return 2;
	}

	if (pathname[0] == "/") {		// absolute
		mip = iget(dev, 2);
		pathname++;
	} else {										// relative
		mip = iget(dev, running->cwd->ino);
		//pathname++;
	}

	tokenize(pathname);		// name = [abc,def,hij]

	int ino = 2;		// root by default
	int i = 0;
	while(name[i]) {
		ino = search(mip, name[i]);
		if (!ino) {
			return 0;
		}

		iput(mip);
		mip = iget(dev,ino);
		i++;
	}
	iput(mip);
	return ino;
}

// return name string of myino: SAME as search except by myino;
// copy entry name (string) into myname[ ];

// find the name of the parameter "myino"
int findmyname(MINODE *parent, u32 myino, char *myname) {

	// using parent, search through children
	// I already have search so this should be it
	return search(parent, myino);
}


int findino(MINODE *mip, u32 *myino) {
  // fill myino with ino of .
  // retrun ino of ..
}
