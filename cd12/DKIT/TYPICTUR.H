/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyPicture.h,v $
   $Revision: 1.3 $ $Date: 1994/11/15 15:17:56 $
   Checkin by: $Author: ****** $

   This file contains a subset of the SOFTIMAGE's picture data structure,
   its content is sufficient to read and write data structure into
   SOFTIMAGE's file format...

   NB: In the present version of SOFTIMAGE, only the 8 bits unsigned char
       for the red, green, blue and alpha channel are supported...

   Written by: Laurent Lauzon

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_PICTURE_H__
#define __TINY_PICTURE_H__

#include  "tySoftTy.h"
#include  "tyFile.h"

/* value for the fields field */
#define  DK_NO_FIELD	0
#define  DK_ODD_FIELD	1
#define  DK_EVEN_FIELD	2
#define  DK_FULL_FRAME	3

typedef
   struct
   {
      short		 width;
      short		 height;
      float		 ratio;
      short		 fields;
      unsigned char	*red;
      unsigned char	*green;
      unsigned char	*blue;
      unsigned char	*alpha;
      unsigned char	*shadow;	/* for future expansion */
      float		*depth;		/* for future expansion */
      unsigned char	*auxiliary1;	/* for future expansion */
      unsigned char	*auxiliary2;	/* for future expansion */

      /* DK_Pointer to current scanline (used by to read and write scanline) */
      unsigned char	*ptrRed;
      unsigned char	*ptrGreen;
      unsigned char	*ptrBlue;
      unsigned char	*ptrAlpha;
      unsigned char	*ptrShadow;	/* for future expansion */
      float		*ptrDepth;	/* for future expansion */
      unsigned char	*ptrAuxiliary1;	/* for future expansion */
      unsigned char	*ptrAuxiliary2;	/* for future expansion */
   } DK_Picture;

/* 1st nibble of type fields */
#define DK_UNSIGNED_INTEGER	0x00
#define DK_SIGNED_INTEGER	0x10	/* for future expansion */
#define DK_SIGNED_FLOAT		0x20	/* for future expansion */

/* correct spelling BUT preserve it for bwrd compatibility and promote new one in userDoc */
#define DK_PURE_RUN_LENGHT	DK_PURE_RUN_LENGTH
#define DK_MIXED_RUN_LENGHT	DK_MIXED_RUN_LENGTH

/* 2nd nibble of type fields */
#define DK_UNCOMPRESSED		0x00
#define DK_PURE_RUN_LENGTH	0x01
#define DK_MIXED_RUN_LENGTH	0x02

/* value for channels fields (can be ORed) */
#define DK_RED_CHANNEL		0x80
#define DK_GREEN_CHANNEL	0x40
#define DK_BLUE_CHANNEL		0x20
#define DK_ALPHA_CHANNEL	0x10
#define DK_SHADOW_CHANNEL	0x08	/* for future expansion */
#define DK_DEPTH_CHANNEL	0x04	/* for future expansion */
#define DK_AUXILIARY_1_CHANNEL	0x02	/* for future expansion */
#define DK_AUXILIARY_2_CHANNEL	0x01	/* for future expansion */

/* structure describing the way it is code in file... */

typedef
   struct DK_channelPacket
   {
      char   			 size;		/* nb of bits by channel (8) */
      char   			 type;
      char   			 channels; 
      struct DK_channelPacket	*next;
   } DK_channelPacket;

/* Global DK_Picture prototypes */

extern	DK_Picture *DK_pictureAllocate( void );
extern	DK_Boolean  DK_pictureDispose( DK_Picture ** );
extern	DK_Picture *DK_pictureReadFile( DK_String );
extern	DK_Boolean  DK_pictureWriteFile( DK_String, DK_Picture *, DK_channelPacket * );
extern	FILE *DK_pictureWriteFileHeader( DK_String, DK_Picture *, DK_channelPacket ** );
extern	FILE *DK_pictureReadFileHeader( DK_String, DK_Picture *, DK_channelPacket ** );
extern	DK_Boolean  DK_pictureReadScanline( FILE *, DK_Picture *, DK_channelPacket * );
extern	DK_Boolean  DK_pictureWriteScanline( FILE *, DK_Picture *, DK_channelPacket * );
extern	void     DK_channelPacketDispose( DK_channelPacket ** );


#endif  /* ifndef __TINY_PICTURE_H__ */
