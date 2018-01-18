/*
 * $QNXLicenseC:
 * Copyright 2007, QNX Software Systems. All Rights Reserved.
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



/*
 *  sys/elftypes.h
 *

 */
#ifndef __ELFTYPES_H_INCLUDED
#define __ELFTYPES_H_INCLUDED

__BEGIN_DECLS

typedef __uint32_t	Elf32_Addr;
typedef __uint32_t	Elf32_Off;
typedef __int32_t		Elf32_Sword;
typedef __uint32_t	Elf32_Word;
typedef __uint16_t	Elf32_Half;

typedef __uint64_t	Elf64_Addr;
typedef __uint64_t	Elf64_Off;
typedef __int32_t		Elf64_Sword;
typedef __int64_t		Elf64_Sxword;
typedef __uint32_t	Elf64_Word;
typedef __uint64_t	Elf64_Xword;
typedef __uint16_t	Elf64_Half;

__END_DECLS

#endif

#ifdef __QNXNTO__
#include <sys/srcversion.h>
__SRCVERSION( "$URL: http://svn.ott.qnx.com/product/branches/deckard/BB10_2_0X/lib/elf/public/sys/elftypes.h $ $Rev: 219996 $" )
#endif
