#define FFS_DIR_H

#ifndef DISK_DRIVER_H
#include "disk_driver.h"
extern struct disk_operations disk_ops;
#endif

// need super operations
#ifndef FFS_SUPER_H
#include "ffs_super.h"
extern struct super_operations super_ops;
#endif


// Disk block where the directory starts
#define ROOT_DIR_OFFSET	super_ops.getStartRotdir()

// Note that dentries currently fit exactly in one block
#define DENTRIES_PER_BLOCK	(DISK_BLOCK_SIZE / sizeof(struct dentry))

#define FNAME_LENGTH	4

struct dentry {
  char name[FNAME_LENGTH];
  unsigned int inode;
};

union dir_block {
  struct dentry dir[DENTRIES_PER_BLOCK];
  unsigned char data[DISK_BLOCK_SIZE];
};


/* operations on directory structures

  dir_open: Open a directory and read-in the directory block
		MUST O / first, and cwd -> root
		then, either C / OR O another, and cwd -> other
		if another is open, MUST C another and then C /
    parameters: name
    errors:
     -EINVAL invalid name
     -ENOENT Directory doe not exist
     those resulting from disk operations

  dir_close: Close the directory and write-out the directory block
    parameters: name
    errors:
     -EINVAL invalid name
     -ENOTDIR Directory not open
     those resulting from disk operations

  dir_read: Get the next (valid) file entry
    parameters: none
    returns:
     pointer to the next valid directory entry, NULL if no more
    errors:
     if NULL and Directory not open, -ENOTDIR returned on errno
     to mimic POSIX readdir()

  dir_rewind: Reposition the pointer used by dir_read to the beggining
    parameters: none
    errors:
     -ENOTDIR Directory not open

  dir_create: Create a new dentry into a free directory slot, if any
	      DOES NOT perform inode or bmap operations
    parameters:
     @in:  name, inode
    errors:
     -ENOTDIR Directory not open
     -EINVAL Invalid filename
     -EEXIST File exists
     -ENOSPC there are no free entries in the directory table

  dir_delete: Delete a dentry from the directory
	      DOES NOT perform inode or bmap operations
    parameters:
     @in:  name
    errors:
     -ENOTDIR Directory not open
     -EINVAL Invalid filename
     -ENOENT No such file or directory

*/

/* Helper function prototypes

  getfree: get the first available (free) entry
    parameters: none
    returns:
     free entry number (index)
    errors:
     -ENOSPC there are no free entries

  dir_print_table: prints the directory table, all or only valid entries,
		   one per line 
    parameters:
     @in:  all or only valid entries

  readdir_print_table: prints the directory table using readdir; when
		       finished, executes rewinddir

*/

int dir_print_table(int all);

int readdir_print_table();


struct dir_operations {
  int (*open)(char *name);
  int (*close)(char *name);
  struct dentry * (*readdir)();
  int (*rewinddir)();
  int (*create)(char *name, unsigned int inodeNbr);
  int (*delete)(char *name);
};

