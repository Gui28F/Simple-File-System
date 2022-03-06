#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef FFS_INODE_H
#include "ffs_inode.h"
#endif

#ifndef FFS_SUPER_H
#include "ffs_super.h"
#endif
extern struct super_operations super_ops;
extern struct IMsuper ffs_IMsb;



// Returns 1 if numinode refers to a large inode, else 0
static int largeInode(unsigned int numinode) {
  unsigned int ninodeblocks= super_ops.getNinodeblocks();
  unsigned int ninodes= super_ops.getTotalInodes();
  unsigned int nlrgInodes= (ninodeblocks/2)*(LRG_INOS_PER_BLK);

  if (numinode >= ninodes) return -EINVAL;

  return ( (numinode < nlrgInodes)? 1 : 0 );
}


/* inode (global) number is decomposed into inode block number
   and offset within that block. The inode block number starts at 0 */

static int inode_location(unsigned int numinode,\
		 unsigned int *numblock, unsigned int *offset) {
  unsigned int halfBlks= super_ops.getNinodeblocks()/2;
  int lrg= largeInode(numinode); 

  if (lrg < 0) return lrg; // -EINVAL

  if ( lrg ) {
    *numblock= numinode/LRG_INOS_PER_BLK;
    *offset= numinode%LRG_INOS_PER_BLK;
  } else {
    unsigned int smIno= numinode-(halfBlks*LRG_INOS_PER_BLK);
    *numblock= smIno/SML_INOS_PER_BLK + halfBlks;
    *offset= smIno%SML_INOS_PER_BLK;
  }

  return 0;
}

/***
  init: clears (zero) an inode in-memory
    parameters:
     @out: pointer to inode structure					***/

static void inode_init(union sml_lrg *in) {
  memset(in, 0, sizeof(union sml_lrg));
}


/***
  write: writes an inode in its correct place in a disk
    parameters:
     @in: (absolute) inode number; pointer to inode structure
    errors:
     those resulting from disk operations				***/

static int inode_write(unsigned int numinode, const union sml_lrg *in) {
  int ercode;
  unsigned int block, offset;
  union inode_block i_b;

  if ( inode_location(numinode, &block, &offset) < 0) return -EINVAL;

  // read inode block from disk into local mem
  ercode= disk_ops.read(block + INODE_OFFSET, i_b.data);
  if (ercode < 0) return ercode;

  // merge inode into block
  if ( largeInode(numinode) )
    memcpy(&i_b.lrgino[offset], &(in->lrgino), sizeof(struct lrgInode) );
  else
    memcpy(&i_b.smlino[offset], &(in->smlino), sizeof(struct smlInode) );

  // write inode block to disk
  ercode= disk_ops.write(block + INODE_OFFSET, i_b.data);
  if (ercode < 0) return ercode;

  return 0;
}


/***
  read: fills in an inode with its disk image
    parameters:
     @in: (absolute) inode number
     @out: pointer to inode structure
    errors:
     those resulting from disk operations				***/

static int inode_read(unsigned int numinode, union sml_lrg *in) {
  int ercode;
  unsigned int block, offset;
  union inode_block i_b;

  if ( inode_location(numinode, &block, &offset) < 0) return -EINVAL;

  // read inode block from disk into local mem
  ercode= disk_ops.read(block + INODE_OFFSET, i_b.data);
  if (ercode < 0) return ercode;

  // extract inode from block
  if ( largeInode(numinode) )
    memcpy(&(in->lrgino), &i_b.lrgino[offset], sizeof(struct lrgInode) );
  else
    memcpy(&(in->smlino), &i_b.smlino[offset], sizeof(struct smlInode) );

  return 0;
}

/***
  debug: prints information about a single inode
    parameters:
     @in: inode number, pointer to inode, int to print all/valid only	***/

void inode_debug(unsigned int i, union sml_lrg *ino, int validOnly) {

  if ( (!ino->lrgino.isvalid) && (validOnly) ) return;

  printf("Inode: %d\n", i);
  printf("  isvalid= %s\n", (ino->lrgino.isvalid)?"yes":"no");
  printf("  type   = %c\n", ino->lrgino.type);
  printf("  size   = %u\n", ino->lrgino.size);

  if ( largeInode(i) ) {
    printf("  Direct ptrs:\n");
    for (int i= 0; i < DPOINTERS_PER_INODE; i++)
      printf("    [%d]= %u\n", i, ino->lrgino.dptr[i]);
    printf("  Indirect ptrs:\n");
      printf("    [%d]= %u\n", 5, ino->lrgino.iptr);
  } else {
    printf("  start  = %u\n", ino->smlino.start);
    printf("  end    = %u\n", ino->smlino.end);
  }
    
  fflush(stdout);
}

/***
  inode_print_table: prints inode information
    parameters:
     @in: int if 0 prints all inodes of the inode table,
          if 1 prints only valid inodes
    errors:
      those from inode_read						***/

int inode_print_table(int validOnly) {
  int ercode;
  union sml_lrg ino;

  unsigned int ninodesLeft= super_ops.getTotalInodes();

  for (unsigned int i= 0; i < ninodesLeft; i++) {

    ercode=inode_read(i, &ino);
    if (ercode < 0) return ercode;

    inode_debug(i, &ino, validOnly);
  }

  return 0;
}


struct inode_operations inode_ops= {
	.init= &inode_init,
	.write= &inode_write,
	.read= &inode_read
};
