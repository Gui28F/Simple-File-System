CFLAGS = -Wall -g

tstBFS: tstBFS.o disk_driver.o ffs_bytemap.o ffs_super.o ffs_inode.o bfs_dir.o bfs_file.o
	gcc -g -o tstBFS tstBFS.o disk_driver.o ffs_bytemap.o ffs_super.o ffs_inode.o bfs_dir.o bfs_file.o

tstBFS.o: tstBFS.c
	gcc $(CFLAGS) -c tstBFS.c

disk_driver.o: disk_driver.c disk_driver.h bfs_errno.h
	gcc $(CFLAGS) -c disk_driver.c

ffs_bytemap.o: ffs_bytemap.c ffs_bytemap.h bfs_errno.h
	gcc $(CFLAGS) -c ffs_bytemap.c

ffs_super.o: ffs_super.c ffs_super.h disk_driver.h ffs_inode.h bfs_errno.h
	gcc $(CFLAGS) -c ffs_super.c

ffs_inode.o: ffs_inode.c ffs_super.h disk_driver.h ffs_inode.h bfs_errno.h
	gcc $(CFLAGS) -c ffs_inode.c

bfs_dir.o: bfs_dir.c bfs_dir.h ffs_super.h disk_driver.h bfs_errno.h
	gcc $(CFLAGS) -c bfs_dir.c

bfs_file.o: bfs_file.c bfs_file.h bfs_dir.h ffs_bytemap.h ffs_super.h bfs_errno.h
	gcc $(CFLAGS) -c bfs_file.c

clean:
	rm tstBFS tstBFS.o disk_driver.o ffs_bytemap.o ffs_super.o ffs_inode.o bfs_dir.o bfs_file.o

