#include <stdio.h>
#include <string.h>
#include <math.h>

#ifndef DISK_DRIVER_H
#include "disk_driver.h"
extern struct disk_operations disk_ops;
#endif

#ifndef FFS_SUPER_H
#include "ffs_super.h"
#endif

#ifndef FFS_BYTEMAP_H
#include "ffs_bytemap.h"
extern struct bytemap_operations bmap_ops;
#endif

#ifndef FFS_INODE_H
#include "ffs_inode.h"
#endif


struct IMsuper ffs_IMsb; // one in-memory SB only

/***
  create: To be called by format(), requires NOT mounted as it
          overwrites the in-memory SB structure.
	  DOES NOT UPDATE the disk superblock, that must be an explicit
	  call to the disk driver write
    parameters:
     @in: disk size (blocks); number of blocks to allocate for inodes
     @out: pointer to superblock structure				***/

static void super_create(unsigned int nblocks, unsigned int ninodeblocks) {
  ffs_IMsb.sb.fsmagic= FS_MAGIC;
	ffs_IMsb.sb.nblocks = nblocks;
	ffs_IMsb.sb.nbmapblocksinodes = 1;
	ffs_IMsb.sb.ninodeblocks = ninodeblocks;
	ffs_IMsb.sb.ninodes = (LRG_INOS_PER_BLK + SML_INOS_PER_BLK)*(ninodeblocks/2);
	ffs_IMsb.sb.startInArea = ffs_IMsb.sb.nbmapblocksinodes + 1;
	ffs_IMsb.sb.startRotdir = ffs_IMsb.sb.startInArea + ninodeblocks;
	ffs_IMsb.sb.startDtBmap = ffs_IMsb.sb.startRotdir + 1;
	ffs_IMsb.sb.nbmapblocksdata = 1;
	ffs_IMsb.sb.startDtArea = ffs_IMsb.sb.startDtBmap + ffs_IMsb.sb.nbmapblocksdata;
	ffs_IMsb.sb.ndatablocks = nblocks - ffs_IMsb.sb.startDtArea;
  ffs_IMsb.sb.mounted= 0;
}


/***
  read: reads in a SB from disk, overwrites the in-mem SB structure.
	Requires disk open at the driver level.
    parameters:
     @out: pointer to superblock structure				***/

static int super_read() {
  int ercode;
  union sb_block sb_u;

  ercode=disk_ops.read(SB_OFFSET, sb_u.data);
  if (ercode < 0) return ercode;

  memcpy(&ffs_IMsb.sb, sb_u.data, sizeof(sb_u.data));

  return 0;
}


/***
  write: writes the in-mem SB structure to disk.
        Requires disk open at the driver level.
    parameters:
     @in: pointer to superblock structure				***/

static int super_write() {
  int ercode;
  union sb_block sb_u;

  memset(sb_u.data, 0, DISK_BLOCK_SIZE);	// clean...
  memcpy(&sb_u.sb,&ffs_IMsb.sb, sizeof(ffs_IMsb.sb));

  ercode=disk_ops.write(SB_OFFSET, sb_u.data);
  if (ercode < 0) return ercode;

  return 0;
}


/***
  mount: mount the superblock and optionally print its info
	 NO other disk can be mounted
    parameters:
     @in: disk name, print debugging information
    errors:
     those from disk driver						***/

static int super_mount( char *diskname, int debug ) {
  int ercode;

  ercode= disk_ops.open(diskname, 0);
  if (ercode < 0) return ercode;

  ercode=super_read();
  if (ercode < 0) return ercode;

  ffs_IMsb.sb.mounted= 1;
  ercode=super_write();
  if (ercode < 0) return ercode;

  if (debug) super_debug();

  bmap_ops.init(); // Compute the ranges for the small, large, data bytemaps

  return 0;
}


/***
  umount: umount the superblock
    parameters:
     none
    errors:
     those from disk driver						***/

static int super_umount() {
  int ercode;

  ffs_IMsb.sb.mounted= 0;
  ercode=super_write();
  if (ercode < 0) return ercode;

  ercode= disk_ops.close();
  if (ercode < 0) return ercode;

  return 0;
}

/***
  get*: gets the relevant BFS info from the in-mem SB
    parameters: none
    returns: unsigned int value or address of region			***/

unsigned int super_getStartRotdir() {
  return ffs_IMsb.sb.startRotdir;
}

unsigned int super_getStartDtBmap() {
  return ffs_IMsb.sb.startDtBmap;
}

unsigned int super_getStartDtArea() {
  return ffs_IMsb.sb.startDtArea;
}

unsigned int super_getNinodeblocks() {
  return ffs_IMsb.sb.ninodeblocks;
}

unsigned int super_getTotalInodes() {
  return ffs_IMsb.sb.ninodes;
}

unsigned int super_getNdatablocks() {
  return ffs_IMsb.sb.ndatablocks;
}


/* Helper functions */

void super_debug() {
  printf("Superblock:\n");
  printf("  fsmagic          = 0x%x\n", ffs_IMsb.sb.fsmagic);
  printf("  nblocks          = %u\n", ffs_IMsb.sb.nblocks);
  printf("  nbmapblocksinodes= %u\n", ffs_IMsb.sb.nbmapblocksinodes);
  printf("  ninodeblocks     = %u\n", ffs_IMsb.sb.ninodeblocks);
  printf("  ninodes          = %u\n", ffs_IMsb.sb.ninodes);
  printf("  nbmapblocksdata  = %u\n", ffs_IMsb.sb.nbmapblocksdata);
  printf("  ndatablocks      = %u\n", ffs_IMsb.sb.ndatablocks);
  printf("  startInArea      = %u\n", ffs_IMsb.sb.startInArea);
  printf("  startRotdir      = %u\n", super_getStartRotdir());
  printf("  startDtBmap      = %u\n", super_getStartDtBmap());
  printf("  startDtArea      = %u\n", super_getStartDtArea());
  printf("  mounted          = %s\n", (ffs_IMsb.sb.mounted)?"yes":"no");
  fflush(stdout);
}

void IMsuper_debug() {
  printf("In-Memory Superblock:\n");
  printf("  dirty       = %s\n", (ffs_IMsb.dirty)?"yes":"no");
  super_debug();
}


struct super_operations super_ops= {
	.create= &super_create,
	.read= &super_read,
	.write= &super_write,
	.mount= &super_mount,
	.umount= &super_umount,
	.getStartRotdir= &super_getStartRotdir,
	.getStartDtBmap= &super_getStartDtBmap,
	.getStartDtArea= &super_getStartDtArea,
	.getNinodeblocks= &super_getNinodeblocks,
	.getTotalInodes= &super_getTotalInodes,
	.getNdatablocks= &super_getNdatablocks
};

