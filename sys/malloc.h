#ifndef _SYS_MALLOC_H_
#define	_SYS_MALLOC_H_

/*
 * flags to malloc
 */
#define	M_WAITOK	0x0000
#define	M_NOWAIT	0x0001

/*
 * Types of memory to be allocated
 */
#define	M_FREE		0	/* should be on free list */
#define	M_MBUF		1	/* mbuf */
#define	M_DEVBUF	2	/* device driver memory */
#define	M_SOCKET	3	/* socket structure */
#define	M_PCB		4	/* protocol control block */
#define	M_RTABLE	5	/* routing tables */
#define	M_HTABLE	6	/* IMP host tables */
#define	M_FTABLE	7	/* fragment reassembly header */
#define	M_ZOMBIE	8	/* zombie proc status */
#define	M_IFADDR	9	/* interface address */
#define	M_SOOPTS	10	/* socket options */
#define	M_SONAME	11	/* socket name */
#define	M_NAMEI		12	/* namei path name buffer */
#define	M_GPROF		13	/* kernel profiling buffer */
#define	M_IOCTLOPS	14	/* ioctl data buffer */
#define	M_MAPMEM	15	/* mapped memory descriptors */
#define	M_CRED		16	/* credentials */
#define	M_PGRP		17	/* process group header */
#define	M_SESSION	18	/* session header */
#define	M_IOV		19	/* large iov's */
#define	M_MOUNT		20	/* vfs mount struct */
#define	M_FHANDLE	21	/* network file handle */
#define	M_NFSREQ	22	/* NFS request header */
#define	M_NFSMNT	23	/* NFS mount structure */
#define	M_NFSNODE	24	/* NFS vnode private part */
#define	M_VNODE		25	/* Dynamically allocated vnodes */
#define	M_CACHE		26	/* Dynamically allocated cache entries */
#define	M_DQUOT		27	/* UFS quota entries */
#define	M_UFSMNT	28	/* UFS mount structure */
#define	M_SHM		29	/* SVID compatible shared memory segments */
#define	M_VMMAP		30	/* VM map structures */
#define	M_VMMAPENT	31	/* VM map entry structures */
#define	M_VMOBJ		32	/* VM object structure */
#define	M_VMOBJHASH	33	/* VM object hash structure */
#define	M_VMPMAP	34	/* VM pmap */
#define	M_VMPVENT	35	/* VM phys-virt mapping entry */
#define	M_VMPAGER	36	/* XXX: VM pager struct */
#define	M_VMPGDATA	37	/* XXX: VM pager private data */
#define	M_FILE		38	/* Open file structure */
#define	M_FILEDESC	39	/* Open file descriptor table */
#define	M_LOCKF		40	/* Byte-range locking structures */
#define	M_PROC		41	/* Proc structures */
#define	M_SUBPROC	42	/* Proc sub-structures */
#define	M_SEGMENT	43	/* Segment for LFS */
#define	M_LFSNODE	44	/* LFS vnode private part */
#define	M_FFSNODE	45	/* FFS vnode private part */
#define	M_MFSNODE	46	/* MFS vnode private part */
#define	M_NQLEASE	47	/* Nqnfs lease */
#define	M_NQMHOST	48	/* Nqnfs host address table */
#define	M_NETADDR	49	/* Export host address structure */
#define	M_NFSSVC	50	/* Nfs server structure */
#define	M_NFSUID	51	/* Nfs uid mapping structure */
#define	M_NFSD		52	/* Nfs server daemon structure */
#define	M_IPMOPTS	53	/* internet multicast options */
#define	M_IPMADDR	54	/* internet multicast address */
#define	M_IFMADDR	55	/* link-level multicast address */
#define	M_MRTABLE	56	/* multicast routing tables */
#define M_ISOFSMNT	57	/* ISOFS mount structure */
#define M_ISOFSNODE	58	/* ISOFS vnode private part */
#define M_NFSRVDESC	59	/* NFS server socket descriptor */
#define M_NFSDIROFF	60	/* NFS directory offset data */
#define M_NFSBIGFH	61	/* NFS version 3 file handle */
#define	M_TEMP		74	/* misc temporary data buffers */
#define	M_LAST		75	/* Must be last type + 1 */

//liai todo change the function name
#define malloc(size, type, canwait) liai_malloc(size);
#endif
