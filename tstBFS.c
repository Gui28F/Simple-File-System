#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#ifndef DISK_DRIVER_H
#include "disk_driver.h"
extern struct disk_operations disk_ops;
#endif

#ifndef FFS_SUPER_H
#include "ffs_super.h"
extern struct super_operations super_ops;
#endif

#ifndef FFS_INODE_H
#include "ffs_inode.h"
extern struct inode_operations inode_ops;
#endif

#ifndef FFS_BYTEMAP_H
#include "ffs_bytemap.h"
extern struct bytemap_operations bmap_ops;
#endif

#ifndef BFS_FILE_H
#include "bfs_file.h"
extern struct file_operations file_ops;
#endif


/***
  Close root directory
  args[0]: C; args[1]: directory name (currently /)
***/
void run_C(int argc, char* args[]) {
  int ercode;

  ercode= dir_ops.close(args[1]);
  if (ercode < 0)
    printf("%s\t\tERROR\n", args[0]);
  else
    printf("%s\t\tOK\n", args[0]);
}


/***
  Create or Open a disk. Do not use Open because we now Close the disk here
  args[0]= D, args[1]= disk name, args[2]= # of blocks (0 to open) 
***/
void run_D(int argc, char* args[]) {
  int ercode;
  int nblocks= 0;

  if (argc == 3) nblocks= atoi(args[2]);
  ercode= disk_ops.open(args[1], nblocks);
  if (ercode < 0) {
    if (argc == 3) printf("%s %s %s\tCreate ERROR\n",args[0],args[1],args[2]);
    else printf("%s %s\t\tOpen ERROR\n", args[0], args[1]);
    printf( "create():disk_ops.open %s\n", strerror(-ercode) );
    return;
  } else {
    if (argc == 3) printf("%s %s %s\tCreate OK\n",args[0],args[1],args[2]);
    else printf("%s %s\t\tOpen OK\n", args[0], args[1]);
  }

  // Create will now close the disk
  ercode= disk_ops.close();
  if (ercode < 0) {
    printf("%s\t\tUnexpected Close ERROR\n", args[0]);
    printf( "create():disk_ops.close %s\n", strerror(-ercode) );
  }
}


/***
  Helper: this will be the format function
***/
void format(char *diskname, unsigned int ninodeblocks) {
  int ercode;

  if ( ninodeblocks % 2 ) {
    printf("format(): ninodeblocks MUST be EVEN!\n"); 
    return;
  }

  ercode= disk_ops.open(diskname, 0);
  if (ercode < 0) {
    printf( "format():disk_ops.open %s\n", strerror(-ercode) );
    return;
  }

  ercode= disk_ops.stat(diskname);
  if (ercode < 0) {
    printf( "format():disk_ops.stat %s\n", strerror(-ercode) );
    return;
  }

  unsigned int nblocks= ercode;  
  super_ops.create(nblocks, ninodeblocks);

  ercode= super_ops.write();
  if (ercode < 0) {
    printf( "format():super_ops.write %s\n", strerror(-ercode) );
    return;
  }

  // Format will now close the disk
  ercode= disk_ops.close();
  if (ercode < 0) {
    printf( "format():disk_ops.close %s\n", strerror(-ercode) );
  }
}
/***
  Format a disk
  args[0]= F, args[1]= disk name, args[2]= even ninodeblocks
***/
void run_F(int argc, char* args[]) {
  unsigned int ninodeblocks;

  if (argc == 3) ninodeblocks= atoi(args[2]);
  else {
    printf("%s %s\t\tERROR: missing nidodeblocks\n", args[0], args[1]);
    return;
  }

  printf("%s %s\t%s\tFormatting...\n", args[0], args[1], args[2]);
  format( args[1], ninodeblocks );
}


/***
  Mount: Read in the superblock and optionally debug. Should NOT be mounted
  args[0]= M, args[1]= disk name
***/
void run_M(int argc, char* args[]) {
  int ercode;
  
  ercode= super_ops.mount(args[1], 1);
  if (ercode < 0) {
    printf("mount: %s %s\tERROR\n",args[0],args[1]);
    return;
  }

  //bmap_ops.init();
}


/***
  Open directory
  args[0]: O; args[1]: directory name (currently /)
***/
void run_O(int argc, char* args[]) {
  int ercode;

  ercode= dir_ops.open(args[1]);
  if (ercode < 0)
    printf("%s\t%s\tERROR\n", args[0], strerror(-ercode));
  else
    printf("%s\t\tOK\n", args[0]);
}


/***
  Umount: Set mounted= 0 and write the superblock. Close the disk
  args[0]= U
***/
void run_U(int argc, char* args[]) {
  int ercode;

  ercode=super_ops.umount();
  if (ercode < 0) printf("umount: %s \tERROR\n",args[0]);
}


/***
  Allocate i-node: Write an inode
  args[0]= a, args[1]= abs number, args[2]= type
***/
void run_a(int argc, char* args[]) {
  int ercode;
  union sml_lrg in;

  inode_ops.init(&in);
  in.smlino.isvalid= 1; // don't care if large or small, common header
  in.smlino.type= args[2][0]; // don't care if large or small, common header
  ercode= inode_ops.write(atoi(args[1]), &in); 
  if (ercode < 0) {
    printf("inode write: %s %s %s\tERROR\n",args[0],args[1],args[2]);
    return;
  }
}


/***
  Create a Contiguous file
  args[0]= c, args[1]= name args[2]= blocks args[3]= type
***/
void run_c(int argc, char* args[]) {
  int ercode;

  ercode= file_ops.create_C(args[1], 1, args[3][0]);
  if (ercode < 0) printf("ERROR file_ops.create_C %s\n", strerror(-ercode));
}


/***
  Deallocate i-node: Write a clean inode in-place
  args[0]= d, args[1]= abs number
***/
void run_d(int argc, char* args[]) {
  int ercode;
  union sml_lrg in;

  inode_ops.init(&in);
  ercode= inode_ops.write(atoi(args[1]), &in); 
  if (ercode < 0) {
    printf("inode write: %s %s %s\tERROR\n",args[0],args[1],args[2]);
    return;
  }
}


/***
  Create a directory entry
  args[0]: e; args[1]: name;  args[2]: inode number
***/
void run_e(int argc, char* args[]) {
  int ercode;

  ercode= dir_ops.create(args[1], atoi(args[2]));
  if (ercode < 0)
    printf("%s\t%s\t%s\tERROR\n", args[0], args[1], args[2]);
  else
    printf("%s\t%s\t%s\tOK\n", args[0], args[1], args[2]);
}


/***
  Create an indexed file
  args[0]= i, args[1]= name
***/
void run_i(int argc, char* args[]) {
  int ercode;

  ercode= file_ops.create_I(args[1]);
  if (ercode < 0) printf("ERROR file_ops.create_I %s\n", strerror(-ercode));
}


/***
  Bytemap get free test
  args[0]= g, args[1]= 0 SML_INODE_BMAP 1 LRG_INODE_BMAP, 2 DATA_BMAP
***/
void run_g(int argc, char* args[]) {

  printf("Bmap get free: %d\n", bmap_ops.getfree(atoi(args[1]), atoi(args[2])));
}  


/***
  Bytemap set test
  args[0]= s, args[1]= 0 SML_INODE_BMAP 1 LRG_INODE_BMAP, 2 DATA_BMAP
  args[2]= entry args[3]= howMany args[4]=value2set
***/
void run_s(int argc, char* args[]) {

  printf("Bmap set: bmap %d target %d value %d entry %d\n", \
		atoi(args[1]), atoi(args[2]), atoi(args[3]),
		bmap_ops.set(atoi(args[1]), atoi(args[2]), \
				 atoi(args[3]), atoi(args[4])));
}  


/***
  Delete a directory entry
  args[0]: t; args[1]: name
***/
void run_t(int argc, char* args[]) {
  int ercode;

  ercode= dir_ops.delete(args[1]);
  if (ercode < 0)
    printf("%s\t%s\tERROR\n", args[0], args[1]);
  else
    printf("%s\t%s\tOK\n", args[0], args[1]);
}


/***
  Bytemap print table
  args[0]= B, args[1]= 0 SML_INODE_BMAP 1 LRG_INODE_BMAP, 2 DATA_BMAP
***/
void run_B(int argc, char* args[]) {
  int ercode;

  //bmap_ops.init();
  ercode= bytemap_print_table(atoi(args[1]));
  if (ercode < 0) printf("ERROR bytemap_print_table\n");
}

// Inode print table
// args[0]= t, args[1]= 0 all entries 1 valid only
void run_I(int argc, char* args[]) {
  inode_print_table(atoi(args[1]));
}


/***
  Print (valid) directory entries using readdir()
  args[0]= Y
***/
void run_Y(int argc, char* args[]) {
  int ercode;

  ercode= readdir_print_table();
  if (ercode < 0) printf("ERROR readdir_print_table %s\n", strerror(-ercode));
}



void runcommand(int argc, char* args[]){

  switch (args[0][0]) {
    case 'C': // Close current directory, flush to disk
      run_C(argc, args);
      break;
    case 'D': // Disk open
      run_D(argc, args);
      break;
    case 'F': // Format disk
      run_F(argc, args);
      break;
    case 'M': // Mount and optionally debug, must be unmounted
      run_M(argc, args);
      break;
    case 'O': // Open directory, read to memory, becomes current dir
      run_O(argc, args);
      break;
    case 'U': // Umount
      run_U(argc, args);
      break;
    case 'a': // Inode allocate (write)
      run_a(argc, args);
      break;
    case 'c': // Create contiguous file with nblocks of some type
      run_c(argc, args);
      break;
    case 'd': // Inode deallocate (write clean inode in-place)
      run_d(argc, args);
      break;
    case 'e': // Create a directory entry
      run_e(argc, args);
      break;
    case 'g': // Bytemap getfree
      run_g(argc, args);
      break;
    case 's': // Bytemap set
      run_s(argc, args);
      break;
    case 't': // Delete a directory entry
      run_t(argc, args);
      break;
    case 'B': // Print Bytemap tables
      run_B(argc, args);
      break;
    case 'I': // Print the Inode table
      run_I(argc, args);
      break;
    case 'Y': // Print valid directory entries
      run_Y(argc, args);
      break;
    case '#': // Comment line
      break;
    default:
      printf("WRONG SPEC FILE?\n");
      break;
  }
}

/************************* MAIN & related code ***************************/

#define LINESIZE	64
#define ARGVMAX		32

int makeargv(char *s, char* argv[ARGVMAX+1]) {
  // in: s points a text string with words
  // out: argv[] points to all words in the string s (*s is modified!)
  // pre: argv is predefined as char *argv[ARGVMAX+1]
  // return: number of words pointed to by the elements in argv (or -1 in case of error)

  int ntokens;

  if ( s == NULL || argv == NULL || ARGVMAX == 0)
    return -1;

  ntokens = 0;
  argv[ntokens] = strtok(s, " \t\n");
  while ( (argv[ntokens] != NULL) && (ntokens < ARGVMAX) ) {
    ntokens++;
    argv[ntokens] = strtok(NULL, " \t\n");
  }
  argv[ntokens] = NULL; // it must terminate with NULL
  return ntokens;
}

void interp(FILE *fdesc) {
  char line[LINESIZE];
  char* av[ARGVMAX];
  int argc;

  while ( fgets( line, LINESIZE, fdesc ) != NULL ) {
    if ( (argc= makeargv(line, av) ) > 0 )
    runcommand(argc, av);
  }
}

int main (int argc, char *argv[]) {
  FILE *fdesc;

  if ( (argc > 1) && (strcmp(argv[1], "<")) ) {
    fdesc= fopen(argv[1], "r");
    if (fdesc == NULL) abort();
  } else
    fdesc= stdin;

  interp(fdesc);

  return 0;
}
