#define DISK_DRIVER_H

#define DISK_BLOCK_SIZE		512

#ifndef _BFS_ERRNO_H
#include "bfs_errno.h"
#endif

/*
Synopsis:	open(char *filename, unsigned int n) 
Description:	If the file (“disk”) named <filename> does not exist, it
		will be created with a length of n > 0 blocks (where a block
		is of size DISK_BLOCK_SIZE bytes); if <filename> does exist,
		then if n is zero the disk will be “just opened”. However,
		if n > 0, the disk will be recreated with the new size.
		When blocks are written as a result of the open(), they are
		zeroed.
Errors:
-EBUSY		A disk is already open
-ENOENT		A open with n == 0 was specified, but the file named <filename>
		does not exist
-1		an unforeseen error has occurred


Synopsis:	stat() 
Description:	reports the size (in blocks) of an opened disk.
Errors:
-ENODEV		There is no open(ed) disk


Synopsis:	read(unsigned int blknmbr, unsigned char *buf)
Description:	reads in a disk block specified by its number.
Errors:
-ENODEV		There is no open(ed) disk
-ENOSPC		A request was made to read a block that does not exist
-1		an unforeseen error has occurred


Synopsis:	write(unsigned int blknmbr, const unsigned char *buf)
Description:	writes out a disk block specified by its number.
Errors:
-ENODEV		There is no open(ed) disk
-ENOSPC		A request was made to read a block that does not exist
-1		an unforeseen error has occurred

Synopsis:	close() 
Description:	closes an opened disk.
Errors:
-ENODEV		There is no open(ed) disk
-1		an unforeseen error has occurred

*/

struct disk_operations {
  int (*open)(const char *filename, unsigned int n);
  int (*stat)();
  int (*read)(unsigned int blknmbr, unsigned char *buf);
  int (*write)(unsigned int blknmbr, const unsigned char *buf);
  int (*close)();
};

