#define FFS_BYTEMAP_H

#ifndef DISK_DRIVER_H
#include "disk_driver.h"
extern struct disk_operations disk_ops;
#endif

// For choosing the two inodes and data bytemaps
#define SML_INODE_BMAP	0
#define LRG_INODE_BMAP	1
#define DATA_BMAP	2

#define NBR_OF_BMAPS	3

// Declare as many entries of this structure as bytemaps you want.
// Current implementation in .c uses an array of these structures

struct bmapMData {
  unsigned int diskBlock;
  unsigned int BMstart;		// abs number of first entry
  unsigned int BMend;		// number of entries in this bmap
};


/* operations on bytemaps

   init: initializes in-memory structure that holds bmap addresses
     disk address, in-memory start/end indexes
   CAVEAT: assumes a bmap is 1 disk block

   getfree: first-fit get "howMany" contiguous free entries
     parameters:
       @in: bmapIDX (which bmap to access), "howMany" contiguous
     returns:
       position of the 1st free in the range
     errors:
       -ENOSPC there are no free entries
       those resulting from disk operations

   set: set an entry to some value (0/1), UPDATES disk image
     parameters:
       @in: bmapIDX (which bmap to access), entry, value
     errors:
       -EFBIG access outside bmap range
       -EINVAL accessed entry already holds that value
       those resulting from disk operations
*/

/* Helper function prototypes

  bytemap_print_table: prints the full table, 16 entries per line
    parameters:
     @in: bmapIDX (which bmap to access)

*/

int bytemap_print_table(unsigned int bmapIDX);


struct bytemap_operations {
  void (*init)();
  int (*getfree)(unsigned int bmapIDX, unsigned int howMany);
  int (*set)(unsigned int bmapIDX, \
		unsigned int entry, unsigned int howMany, unsigned char set);
};

