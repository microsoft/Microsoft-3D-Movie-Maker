/***************************************************************************************************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyTag.h,v $
   $Revision: 1.2 $ $Date: 1992/12/28 15:30:21 $
   Checkin by: $Author: claude $

   This file contains a subset of the SOFTIMAGE's tag data structure,
   its content is sufficient to read and write data structure into
   SOFTIMAGE's file format...

   Written by: Laurent Lauzon

   (c) Copyright 1992, 1993 SOFTIMAGE Inc.

***************************************************************************************************************************************************************/

#ifndef __TINY_TAG_H__
#define __TINY_TAG_H__

#include  "tySoftTy.h"
#include  "tyFile.h"

/* value for the fields field */
#define  DK_NO_FIELD       0
#define  DK_ODD_FIELD      1
#define  DK_EVEN_FIELD     2
#define  DK_FULL_FRAME     3

typedef
   struct
   {
      short	       width;
      short	       height;
      float	       ratio;
      short	       fields;
      short 	       depth;
      unsigned char ***tags;
   } DK_TagImage;

/* return nb of bytes necessary to hold 'width' bits */
#define _DK_SCAN_LENGTH( width )	 (width + 7 - ((width - 1) & 7))

/* return a non-zero number if the k-th pixel of the j-th */
/* scanline of the i-th plane is ON */

#define _DK_GET_PIXEL( tag, i, j, k ) \
   ( (tag)->tags[(i)][(j)][(k)>>3] & (1<<(7-((k)&7))) )

#define _DK_SET_PIXEL( tag, i, j, k, val ) \
   if( val != 0 ) \
      (tag)->tags[(i)][(j)][(k)>>3] |= (1<<(7-((k)&7))); \
   else \
      (tag)->tags[(i)][(j)][(k)>>3] &= ~(1<<(7-((k)&7)))


extern FILE        *DK_tagWriteFileHeader( DK_String, DK_TagImage * );
extern DK_Boolean   DK_tagWriteFile( DK_String, DK_TagImage * );
extern FILE        *DK_tagReadFileHeader( DK_String, DK_TagImage * );
extern DK_TagImage *DK_tagReadFile( DK_String );
extern void         DK_tagReadScanline( FILE *, DK_TagImage *, int );
extern void         DK_tagWriteScanline( FILE *, DK_TagImage *, int );
extern DK_Boolean   DK_tagImageDispose( DK_TagImage ** );
extern DK_Boolean   DK_tagAllocateChannels( DK_TagImage * );
extern DK_TagImage *DK_tagAllocate( void );

#endif  /* ifndef __TINY_TAG_H__ */
