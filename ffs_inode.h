#define FFS_INODE_H

#ifndef DISK_DRIVER_H
#include "disk_driver.h"
extern struct disk_operations disk_ops;
#endif

// need struct super
#ifndef FFS_SUPER_H
#include "ffs_super.h"
#endif


#define INODE_OFFSET	(SB_OFFSET + 2)		// Disk block where
						// inode table starts


// inodes for contiguous files and directories, a.k.a small inodes
struct smlInode {
  unsigned char	 isvalid;
  unsigned char	 type;
  unsigned short size;
  unsigned short start;
  unsigned short end;
};


// Data block addressing pointers for indexed files, a.k.a large inodes
// the last pointer is 1-level indirect, the others are direct
#define DPOINTERS_PER_INODE 5
#define IPOINTERS_PER_INODE 1

// inodes for indexed files, a.k.a large inodes
struct lrgInode {
  unsigned char	 isvalid;
  unsigned char	 type;
  unsigned short size;
  unsigned short dptr[DPOINTERS_PER_INODE];
  unsigned short iptr;
};

union sml_lrg {
  struct smlInode smlino;
  struct lrgInode lrgino;
};

// Note that inodes currently fit exactly in one block

#define SML_INOS_SIZE		(sizeof(struct smlInode))
#define SML_INOS_PER_BLK	(DISK_BLOCK_SIZE / SML_INOS_SIZE)

#define LRG_INOS_SIZE		(sizeof(struct lrgInode))
#define LRG_INOS_PER_BLK	(DISK_BLOCK_SIZE / LRG_INOS_SIZE)


union inode_block {
  struct smlInode smlino[SML_INOS_PER_BLK];
  struct lrgInode lrgino[LRG_INOS_PER_BLK];
  unsigned char data[DISK_BLOCK_SIZE];
};


/* operations on inode structures

  init: clears (zero) an inode in-memory
    parameters:
     @out: pointer to inode structure

  read: fills in an inode with its disk image
    parameters:
     @in: (absolute) inode number
     @out: pointer to inode structure
    errors:
     those resulting from disk operations

  write: writes an inode in its correct place in a disk
    parameters:
     @in: (absolute) inode number; pointer to inode structure
    errors:
     those resulting from disk operations

  debug: prints information about a single inode
    parameters:
     @in: inode number, pointer to inode, int to print all/valid only

*/


/* Helper function prototypes (public)

  inode_print_table: prints inode information
    parameters:
     @in: int if 0 prints all inodes of the inode table, if 1
	  prints only valid inodes
    errors:
      those from inode_read
*/


int inode_print_table(int validOnly);


struct inode_operations {
  void (*init)(union sml_lrg *in);
  int (*write)(unsigned int number, const union sml_lrg *in);
  int (*read)(unsigned int number, union sml_lrg *in);
};
