/*************** type.h file ************************/
typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;

#define FREE        0
#define READY       1

#define BLKSIZE  1024
#define NMINODE    64
#define NFD        16
#define NMOUNT      4
#define NPROC       2

#define DIRECTORY 0x41ed

typedef struct minode{
  INODE INODE;
  int dev, ino;
  int refCount;
  int dirty;
  int mounted;
  struct mntable *mptr;
}MINODE;

typedef struct oft{
  int  mode;
  int  refCount;
  MINODE *mptr;
  int  offset;
}OFT;

typedef struct proc{
  struct proc *next;
  int          pid;
  int          ppid;
  int          status;
  int          uid, gid;
  MINODE      *cwd;
  OFT         *fd[NFD];
}PROC;

/*
typedef struct ext2_group_desc
{
	u32	bg_block_bitmap;	/* Blocks bitmap block *
	u32	bg_inode_bitmap;	/* Inodes bitmap block *
	u32	bg_inode_table;		/* Inodes table block *
	u16	bg_free_blocks_count;	/* Free blocks count *
	u16	bg_free_inodes_count;	/* Free inodes count *
	u16	bg_used_dirs_count;	/* Directories count *
  	u16	bg_pad;
	u32	bg_reserved[3];
}GD;

typedef struct ext2_inode {
	u16	i_mode;		/* File mode *
	u16	i_uid;		/* Owner Uid *
	u32	i_size;		/* Size in bytes *
	u32	i_atime;	/* Access time *
	u32	i_ctime;	/* Creation time *
	u32	i_mtime;	/* Modification time *
	u32	i_dtime;	/* Deletion Time *
	u16	i_gid;		/* Group Id *
	u16	i_links_count;	/* Links count *
	u32	i_blocks;	/* Blocks count *
	u32	i_flags;	/* File flags *
        u32     dummy;
	u32	i_block[15];    /* Pointers to blocks *
        u32     pad[5];         /* inode size MUST be 128 bytes *
        u32	i_date;         /* MTX date *
	u32	i_time;         /* MTX time *
}INODE;

typedef struct ext2_dir_entry_2 {
	u32	inode;			/* Inode number *
	u16	rec_len;		/* Directory entry length *
	u8	name_len;		/* Name length *
	u8	file_type;
	char	name[255];      	/* File name *
}DIR;

*/
