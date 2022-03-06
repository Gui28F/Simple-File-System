/*
 *	     ADAPTED by Paulo Lopes from
 *      ISO C99 Standard: 7.5 Errors	<errno.h>
 */

#ifndef	_BFS_ERRNO_H
#define _BFS_ERRNO_H	1
#endif

#define EPERM            1      /* Operation not permitted */
#define ENOENT           2      /* No such file or directory */
#define EINTR            4      /* Interrupted system call */
#define EIO              5      /* I/O error */
#define ENXIO            6      /* No such device or address */
#define EBADF            9      /* Bad file number */
#define EAGAIN          11      /* Try again */
#define ENOMEM          12      /* Out of memory */
#define EBUSY           16      /* Device or resource busy */
#define EEXIST          17      /* File exists */
#define ENODEV          19      /* No such device */
#define ENOTDIR         20      /* Not a directory */
#define EISDIR          21      /* Is a directory */
#define EINVAL          22      /* Invalid argument */
#define ENFILE          23      /* File table overflow */
#define EMFILE          24      /* Too many open files */
#define EFBIG           27      /* File too large */
#define ENOSPC          28      /* No space left on device */
#define ESPIPE          29      /* Illegal seek */
#define ENAMETOOLONG    36      /* File name too long */
#define ENOSYS          38      /* Function not implemented */
#define EWOULDBLOCK     EAGAIN  /* Operation would block */

#define ENOMEDIUM       123     /* No medium found */
#define EMEDIUMTYPE     124     /* Wrong medium type */
