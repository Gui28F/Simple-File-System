#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "disk_driver.h"

static int dfd= -1; // CHANGE: inserted static
static unsigned int nblocks= -1; // i.e., = 0xfffff...

#ifndef HAS_CACHE

static int disk_open_nc( const char *filename, unsigned int n ) {
  struct stat s;
  unsigned char buf[DISK_BLOCK_SIZE];

  if (dfd != -1) return -EBUSY;

  // open existing file
  if (!n) {
    if ( ( dfd= open(filename, O_RDWR) ) == -1) return -ENOENT;
    if (fstat(dfd, &s) == -1) return -1;
    nblocks= s.st_size / DISK_BLOCK_SIZE;
    return 0;
  }

  // create or re-create
  if ( ( dfd= open(filename, O_RDWR|O_CREAT|O_TRUNC, 0666) ) == -1) return -1;

  memset(buf, 0, DISK_BLOCK_SIZE);
  for (int i= 0; i < n; i++)
    if ( write(dfd, buf, DISK_BLOCK_SIZE) == -1) return -1;

  nblocks = n;

  return 0;
}


static int disk_stat_nc() {
  if (dfd == -1) return -ENODEV;
  return nblocks;
}


static int disk_read_nc(unsigned int blknmbr, unsigned char *buf) {
  if (dfd == -1) return -ENODEV;
  if ( blknmbr >= nblocks) return -ENOSPC;
  if ( lseek(dfd, blknmbr*DISK_BLOCK_SIZE, SEEK_SET) == -1) return -1;
  if ( read(dfd, buf, DISK_BLOCK_SIZE) < DISK_BLOCK_SIZE) return -1;

  return 0;
}


static int disk_write_nc(unsigned int blknmbr, const unsigned char *buf) {
  if (dfd == -1) return -ENODEV;
  if ( blknmbr >= nblocks) return -ENOSPC;
  if ( lseek(dfd, blknmbr*DISK_BLOCK_SIZE, SEEK_SET) == -1) return -1;
  if ( write(dfd, buf, DISK_BLOCK_SIZE) == -1) return -1;

  return 0;
}


static int disk_close_nc() {
  if (dfd == -1) return -ENODEV;
  if (close(dfd) == -1) return -1;
  dfd= -1;
  return 0;
}


struct disk_operations disk_ops= {
	.open= &disk_open_nc,
	.stat= &disk_stat_nc,
	.read= &disk_read_nc,
	.write= &disk_write_nc,
	.close= &disk_close_nc
};

#else

#error CACHE NOT IMPLEMENTED

#endif
