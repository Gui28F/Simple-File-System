#define BFS_FILE_H

#ifndef FFS_BYTEMAP_H
#include "ffs_bytemap.h"
extern struct bytemap_operations bmap_ops;
#endif

#ifndef FFS_INODE_H
#include "ffs_inode.h"
extern struct inode_operations inode_ops;
#endif

#ifndef BFS_DIR_H
#include "bfs_dir.h"
extern struct dir_operations dir_ops;
#endif


/* operations on file structures

  create_C: creates a contiguous file
    parameters:
      @in: filename, max blocks, type (C or D)
     @out: return OK
    errors:
     from bytemap, inode, dir operations
     errors from disk driver

  create_C: creates an indexed file
    parameters:
      @in: filename
     @out: return OK
    errors:
     from bytemap, inode, dir operations
     errors from disk driver

*/


struct file_operations {
  int (*create_C)(char *name, unsigned int nblocks, char type);
  int (*create_I)(char *name);
};
