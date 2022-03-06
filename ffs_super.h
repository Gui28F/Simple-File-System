#define FFS_SUPER_H

/* Disk layout
   +-----+-----+-----+-----+-----+-----+-----+-----+-----+
   | SuB | BMi | Ino | Dir | BMd | Dt1 | Dt2 | ... | END |
   +-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/

#define SB_OFFSET	0
#define BMi_OFFSET	1

#define FS_MAGIC	0xf0f03410

struct super {
  unsigned int fsmagic;
  unsigned int nblocks;
  unsigned int nbmapblocksinodes;	// Always 1 in BFS
  unsigned int ninodeblocks;
  unsigned int ninodes;
  unsigned int startInArea;
  unsigned int startRotdir;
  unsigned int startDtBmap;
  unsigned int nbmapblocksdata;		// Always 1 in BFS
  unsigned int startDtArea;
  unsigned int ndatablocks;
  unsigned int mounted;
};

union sb_block {
  struct super sb;
  unsigned char data[DISK_BLOCK_SIZE];
};

/* strucure for in-memory (IM) variable(s) */
struct IMsuper {
  unsigned int dirty;
  struct super sb;
};


/* operations on superblock structures
  create: To be called by format(), requires NOT mounted as it
          overwrites the in-memory SB structure.
          DOES NOT UPDATE the disk superblock, that must be an explicit                   call to the disk driver write
    parameters:
     @in: disk size (blocks), number of blocks to allocate for inodes
     @out: pointer to superblock structure

  read: reads in a SB from disk, overwrites the in-mem SB structure.
        Requires disk open at the driver level
    parameters:
     @out: pointer to superblock structure

  write: writes the in-mem SB structure to disk.
        Requires disk open at the driver level
    parameters:
     @in: pointer to superblock structure

  get*: gets the relevant BFS info from the in-mem SB
    parameters: none
    returns: unsigned int value or address of region

  NOTE: dirty "bit" of in-memory SB must be set/cleared separately
*/


/* Helper function prototypes */
void super_debug();
void IMsuper_debug();


/* NEW: mount (a disk)
        umount (a disk) */


struct super_operations {
  void (*create)(unsigned int nblocks, unsigned int ninodeblocks);
  int (*read)();
  int (*write)();
  int (*mount)( char *diskname, int debug );
  int (*umount)();
  unsigned int (*getStartRotdir)();
  unsigned int (*getStartDtBmap)();
  unsigned int (*getStartDtArea)();
  unsigned int (*getNinodeblocks)();
  unsigned int (*getTotalInodes)();
  unsigned int (*getNdatablocks)();
};
