char* RAMdisk = (char*) 0x4000000; // global at 4MB
#define BLK 1024

int get_block(u32 blk, char *buf) {
  char *cp = RAMdisk + blk*BLK;
  int i = RAMdisk + blk*BLK;
  memcpy(buf, cp, BLK);
}

int put_block(u32 blk, char *buf) // write to a 1KB block
{
  char *cp = RAMdisk + blk*BLK;
  memcpy(cp, buf, BLK);
}

int search(INODE *ip) {
  int i;
  for (i=0; i<15; i++){
    if (ip->i_block[i] > 0) {
      printf("i_block[%d] has contents = %d\n", i, ip->i_block[i]);
      return i;
    }
  }
  return -1;
}

void printdirs(char* cp, DIR * dp, char* buf1, char* str) {
  while(cp < buf1 + BLK){
     printf("%s\n", dp->name);
     //printf("\t\t\t");

     //printf("dp->name = %s\n", dp->name);

     if (strcmp(str, dp->name) == 0) {
       printf("---- %s found! ----\n", str);
       ino = dp->inode;
     }

     cp += dp->rec_len;
     dp = (DIR *)cp;
  }
  printf("\n");
}



// example call: loader("/bin/u1", running);
// routine:

// 1. tempblk = search(ip) to find a block that has contents
// 2. ip = ip->iblock[tempblk]
// 3. get_block(ip, buf1)
// 4. dp = (DIR*) buf1;
// 5. cp = buf1;
// 6. loop:
// 6.a.    while(cp < buf1 + BLK)
// 6.b         if (dp->name == string) then set ino to it
// 7. block = (ino - 1) / 8 + 5;
// 8. position = (ino - 1) % 8;
// 9. get_block(block, buf1);
// 10. ip = (INODE *)buf1 + position;
int loader(char* filename, PROC* p) {

  char* str = filename;
  char* cp;

  if (str[0] == '/') {
    str++;
  }

  printf("Loading filename %s\n", str);

  // str = bin/u1
  int i = 0;
  while(1) {
    char tmp[32];
    printf("str = %s\n", str);
    while(str[i] != '/' && str[i] != '\0') {
      tmp[i] = str[i];
      i++;
    }
    printf("tmp = %s\n", tmp);

    // str broke on \ or '\0'
    int tempblk = search(ip);       // Block #0 has contents!
    ip = ip->i_block[tempblk];      // ip set to block #0
    get_block(ip, buf1);
    dp = (DIR*) buf1;
    cp = buf1;

    printdirs(cp, dp, buf1, tmp);


    kgetc();
    int block = (ino - 1) / 8 + iblk;
    int position = (ino - 1) % 8;
    printf("Next location: block %d position %d, from ino = %d\n", block, position, ino);
    get_block(block, buf1);
    ip = (INODE*)buf1 + position;


    // found u1, u2, u3, or u4
    if (str[i] == '\0') {
      printf("Found %s at ino = %d, ip is set, returning to main\n", str, ino);
      printf("Testing inode: \n");
      printf("size = %d\n", ip->i_size);
      printf("mode = %x\n", ip->i_mode);
      printf("process %d pgtable at %x\n", p->pid, p->pgdir);

      /*
      All the processes run in the same virtual address space of [2 GB to 2 GB + 1 MB] but
      each has a separate physical memory area, which is isolated from other processes and
      protected by the MMU hardware
      */


      int location = 0x800000 + (p->pid-1)*0x100000;
      for (i = 0; i < 12; i++) {
        if (ip->i_block[i] != 0) {
          get_block(ip->i_block[i], buf2);
          memcpy(location, buf2, BLKSIZE); // copy block
          location += BLKSIZE; // move onto the next block
        }
      }

      printf("Wrote block %d to memory\n");

      // return to old ip state (for next usermode image)
      return 1;
    }
    else {
      str+=i+1;

      while(i>=0) {
        tmp[i] = 0;
        i--;
      }

      printf("Continuing on with str=%s\n", str);
      i = 0;
      kgetc();
    }
  }
}
