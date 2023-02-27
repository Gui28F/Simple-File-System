# Simple-File-System

Implementation of a simple filesystem (SFS) inspired in the Unix basic filesystem structures, i.e., i-nodes, bitmaps and data blocks and the way to program and access those structures in C. We will use an object-oriented (OO) like approach, although the C programming language that does not support OO

| Command | Description |
| --- | --- |
| T 4 | The i-node and bytemap tables only have 4 entries |
| O disk0 100 | Create a file named “disk0” with 100 blocks (x 512 bytes). Blocks are zeroed |
| A 2 | Allocate i-node number 2 (mark it valid and mark the bytemap entry 2 as “in-use”) |  
| A -1 | Allocate the 1st free i-node found (mark it valid and mark the corresponding bytemap entry as “in-use”) |
| D 2 | Deallocate i-node number 2 (mark it not valid, mark the bytemap entry 2 as “free”) |
| C | Close the disk (if your implementation keeps things in-memory, now you must write them to disk) |
| O disk0 | Re-use a file named “disk0” (do not write anything, i.e., do not zero blocks) |
| B / I | Print the bytemap information / Print the i-node information (only the valid field) |
| S 4 | Read the i-node and bytemap tables from disk, but behave has both only have 4 etries each |
| o | Open the root directory and read-in the directory block |
| c | Close the root directory and write-out the directory block|
| r | Get the next (valid) file entry |
| e #### | Create a file into a (free) directory entry (#### are the letters used for the name) |
| d ## | Delete a file named ## from the directory |
| p # | [NOT a dir_operations member function, but useful] Print the directory. If # is 0 (zero), prints only valid entries; if # is 1 (one), prints all entries (assumes that free directory entries have name=”\0\0\0\0” and inode= 0, and prints 4 spaces and 4 zeros. |
