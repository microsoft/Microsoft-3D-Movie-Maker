/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: blockops.h 1.6 1995/02/22 21:36:56 sam Exp $
 * $Locker:  $
 *
 * Memory operations, block fill, block copy etc. Thse are local to the
 * library becuase some compiler's versions are not that good.
 */
#if defined(__WATCOMC__)

/*
 * Inline block operations for Watcom C
 */

void BrIBlockFill(void *dest_ptr, int value, int dwords);
#pragma aux BrIBlockFill = \
	"rep stosd" \
	parm [edi] [eax] [ecx] ;

void BrIBlockCopy(void *dest_ptr, void *src_ptr, int dwords);
#pragma aux BrIBlockCopy = \
	"rep movsd" \
	parm [edi] [esi] [ecx] ;

/* Inline copy near block to far block
 */
void BrIFarBlockCopy(void __far *dest_ptr, void *src_ptr, int dwords);
#pragma aux BrIFarBlockCopy = \
	"push	es"\
	"mov 	es,dx"\
	"rep 	movsd"\
	"pop 	es"\
	parm [dx edi] [esi] [ecx] ;

#define BrBlockFill(d,v,c) BrIBlockFill(d,v,c)	
#define BrBlockCopy(d,s,c) BrIBlockCopy(d,s,c)
#define BrFarBlockCopy(d,s,c) BrIFarBlockCopy(d,s,c)

#endif

