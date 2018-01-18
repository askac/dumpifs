/*
 * $QNXLicenseC:
 * Copyright 2007, 2009, QNX Software Systems. All Rights Reserved.
 *
 * You must obtain a written license from and pay applicable license fees to QNX
 * Software Systems before you may reproduce, modify or distribute this software,
 * or any work that includes all or part of this software.   Free development
 * licenses are available for evaluation and non-commercial purposes.  For more
 * information visit http://licensing.qnx.com or email licensing@qnx.com.
 *
 * This file may contain contributions from others.  Please review this entire
 * file for other proprietary rights or license notices, as well as the QNX
 * Development Suite License Guide at http://licensing.qnx.com/license-guide/
 * for other information.
 * $
 */

#ifndef __IMAGE_H_INCLUDED
#define __IMAGE_H_INCLUDED

#define __FLEXARY(__type, __name) __type __name[1]

enum {
	IMAGE_FLAGS_BIGENDIAN	= 0x01,	/* header, trailer, dirents in big-endian format  */
	IMAGE_FLAGS_READONLY	= 0x02,	/* do not try to write to image (rom/flash) */
	IMAGE_FLAGS_INO_BITS	= 0x04	/* inode bits valid */
};


#define IMAGE_SIGNATURE		"imagefs"
#define IMAGE_SIGNATURE_REV	"sfegami"
struct image_header {
	char				signature[7];		/* Image filesystem signature */
	unsigned char		flags;				/* endian neutral flags */
	unsigned int		image_size;			/* size from header to end of trailer */
	unsigned int		hdr_dir_size;		/* size from header to last dirent */
	unsigned int		dir_offset;			/* offset from header to first dirent */
	unsigned int		boot_ino[4];		/* inode of files for bootstrap pgms */
	unsigned int		script_ino;			/* inode of file for script */
	unsigned int		chain_paddr;		/* offset to next filesystem signature */
	unsigned int		spare[10];
	unsigned int		mountflags;			/* default _MOUNT_* from sys/iomsg.h */
	__FLEXARY(char, mountpoint);			/* default mountpoint for image */
};

#define IFS_BOOTSTRAP_SIGNATURE (('B'<<24)|('S'<<16)|('H'<<8)|('D'<<0))
struct ifs_bootstrap_head {
	unsigned int	signature;
	void *	bootstrap; /* points to a 'struct ifs_bootstrap_data' */
};

struct ifs_bootstrap_data {
	unsigned int	size;
	void*	next_entry;
	void*	args;
	unsigned int	spare;
};

#define IFS_INO_PROCESSED_ELF	0x80000000
#define IFS_INO_RUNONCE_ELF		0x40000000
#define IFS_INO_BOOTSTRAP_EXE	0x20000000

union image_dirent {
	struct image_attr {
		unsigned short		size;			/* size of dirent */
		unsigned short		extattr_offset;	/* If zero, no extattr data */
		unsigned int		ino;			/* If zero, skip entry */
		unsigned int		mode;			/* Mode and perms of entry */
		unsigned int		gid;
		unsigned int		uid;
		unsigned int		mtime;
	}					attr;
	struct image_file {						/* (attr.mode & S_IFMT) == S_IFREG */
		struct image_attr	attr;
		unsigned int		offset;			/* Offset from header */
		unsigned int		size;
		__FLEXARY(char, path);				/* null terminated path (No leading slash) */
	}					file;
	struct image_dir {						/* (attr.mode & S_IFMT) == S_IFDIR */
		struct image_attr	attr;
		__FLEXARY(char, path);				/* null terminated path (No leading slash) */
	}					dir;
	struct image_symlink {					/* (attr.mode & S_IFMT) == S_IFLNK */
		struct image_attr	attr;
		unsigned short		sym_offset;
		unsigned short		sym_size;
		__FLEXARY(char, path);				/* null terminated path (No leading slash) */
		/*	char				sym_link[1];*/	/* symlink contents */
	}					symlink;
	struct image_device {					/* (attr.mode & S_IFMT) == S_IFCHR|BLK|FIFO|NAM|SOCK */
		struct image_attr	attr;
		unsigned int		dev;
		unsigned int		rdev;
		__FLEXARY(char, path);				/* null terminated path (No leading slash) */
	}					device;
};

struct image_trailer {
	unsigned int			cksum;				/* Checksum from start of header to start of trailer */
};


#define SCRIPT_FLAGS_EXTSCHED	0x01
#define SCRIPT_FLAGS_SESSION	0x02
#define SCRIPT_FLAGS_SCHED_SET	0x04
#define SCRIPT_FLAGS_CPU_SET	0x08
#define SCRIPT_FLAGS_BACKGROUND	0x20
#define SCRIPT_FLAGS_KDEBUG		0x40

#define SCRIPT_POLICY_NOCHANGE	0
#define SCRIPT_POLICY_FIFO		1
#define SCRIPT_POLICY_RR		2
#define SCRIPT_POLICY_OTHER		3

#define SCRIPT_TYPE_EXTERNAL	0
#define SCRIPT_TYPE_WAITFOR		1
#define SCRIPT_TYPE_REOPEN		2
#define SCRIPT_TYPE_DISPLAY_MSG	3
#define SCRIPT_TYPE_PROCMGR_SYMLINK	4
#define SCRIPT_TYPE_EXTSCHED_APS	5

#define SCRIPT_CHECKS_MS		100

#define SCRIPT_SCHED_EXT_NONE		0
#define SCRIPT_SCHED_EXT_APS		1

#define SCRIPT_APS_SYSTEM_PARTITION_ID		0
#define SCRIPT_APS_SYSTEM_PARTITION_NAME	"System"
#define SCRIPT_APS_PARTITION_NAME_LENGTH	15
#define SCRIPT_APS_MAX_PARTITIONS			8

union script_cmd {
	struct script_hdr {
		unsigned char	size_lo;		/* size of cmd entry  */
		unsigned char	size_hi;		/* .... */
		unsigned char	type;
		unsigned char	spare;
	} hdr;
	struct script_external {
		struct script_hdr hdr;
		unsigned char	cpu;			/* CPU (turn into runmask) */
		unsigned char	flags;
		union script_external_extsched {
			unsigned char	reserved[2];
			struct {
				unsigned char	id;
				unsigned char	reserved[1];
			}				aps;
		}				extsched;		/* extended scheduller */
		unsigned char	policy;			/* POLICY_FIFO, POLICY_RR, ... */
		unsigned char	priority;		/* priority to run cmd at */
		unsigned char	argc;			/* # of args */
		unsigned char	envc;			/* # of environment entries */
		__FLEXARY(char, args);			/* null padded to 32-bit align */
	} external;
	struct script_waitfor_reopen {
		struct script_hdr hdr;
		unsigned char	checks_lo;
		unsigned char	checks_hi;
		__FLEXARY(char, fname);			/* char fname[] */
	} waitfor_reopen;
	struct script_display_msg {
		struct script_hdr hdr;
		__FLEXARY(char, msg);			/* char msg[] */
	} display_msg;
	struct script_procmgr_symlink {
		struct script_hdr hdr;
		__FLEXARY(char, src_dest);		/* <src_name>, '\0', <dest_name> '\0' */
	} procmgr_symlink;
	struct script_extsched_aps {
		struct script_hdr	hdr;
		unsigned char		parent;
		unsigned char		budget;
		unsigned char		critical_lo;
		unsigned char		critical_hi;
		unsigned char		id;
		__FLEXARY(char, pname);			/* char pname[] */
	} extsched_aps;
};

#endif
