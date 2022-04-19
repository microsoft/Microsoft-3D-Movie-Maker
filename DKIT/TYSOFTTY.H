/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinySoftType.h,v $
   $Revision: 1.12 $ $Date: 1994/11/15 15:25:08 $
   Checkin by: $Author: a_tan $

   This file contains a subset of standard types, constants and macros use in
   Softimage programming environment.

   Written by: Laurent Lauzon

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_SOFTTYPE_H__
#define __TINY_SOFTTYPE_H__

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <malloc.h>
#include <string.h>

typedef void *					DK_Pointer;
typedef long					DK_Boolean;
typedef char *					DK_String;
typedef float					DK_Matrix[4][4];

typedef struct { float  x, y, z; }		DK_Vector;
typedef struct { float  x, y, z, w; }		DK_HVector;

#define	DK_Point	                        DK_Vector

typedef struct { float  r, g, b; }		DK_Colour24;
typedef struct { float  r, g, b, a; }		DK_Colour32;
typedef struct { unsigned char  r, g, b; }	DK_Pixel24;
typedef struct { unsigned char  r, g, b, a; }	DK_Pixel32;

typedef
   struct DK_userNode
   {
      char              type[4];
      int               size;
      char              *data;
      struct DK_userNode   *next;
   } DK_UserNode;

#ifndef FALSE
#define FALSE		0
#endif	/* ifndef FALSE */

#ifndef TRUE
#define TRUE		1
#endif	/* ifndef TRUE */

#ifndef NULL
#define NULL		0L
#endif	/* ifndef NULL */

#define _DK_VectorInit( vct, X, Y, Z ) \
   (vct).x = (X), \
   (vct).y = (Y), \
   (vct).z = (Z)

#define _DK_Deg2Rad( deg ) 	( (deg) * 0.017453292 )
#define _DK_Rad2Deg( rad )		( (rad) * 57.2957795 )

/* macros */
#define _DK_CALLOC( type, nb, id ) \
   (type *) calloc( (nb), (size_t) sizeof(type) ) 
#define _DK_MALLOC( type, nb, id ) \
   (type *) malloc( (nb) ) 
#define _DK_FREE( ptr, id ) \
   if ( (ptr) != NULL ) \
   { \
      free( (char *) (ptr) ); \
      (ptr) = NULL; \
   }
#define _DK_NOP() \
   /* Null Statement */

/* display types */

typedef
   enum
   {
      DK_HIDDEN,
      DK_GHOST,
      DK_VISIBLE
   } DK_LineType;

#define DK_DEFAULT_WIRE_COLOUR 490

/* control variable to write either in the corrent version format */
#define DK_v2_5		0
#define DK_v2_6		1
#define DK_v2_6_2	2
#define DK_v2_6_5	3
#define DK_v2_6_6	4
#define DK_v3_0		5

#define DK_RELEASE_NUMBER	DK_CUR_REL
#define DK_CUR_REL 	DK_VERSION_NAME[sizeof(DK_VERSION_NAME)/sizeof(float)-1]

static float	DK_VERSION_NAME[] = {	(float)2.5, 
					(float)2.6, 
					(float)2.62, 
					(float)2.65, 
					(float)2.66, 
					(float)3.0 }; 

extern	int	DK_VERSION;

/* Formatting variables for ASCII print out of real (%FM.FDf) */
extern	int	 DK_FM;
extern	int	 DK_FD;

/* variable to save normals mesh in file */
extern	DK_Boolean	 DK_NORMAL;

/* variable to save animation in scene file */
extern	DK_Boolean	 DK_WRITE_ANIMATION;

typedef
    struct DK_option
    {
	int verbose;	/* true if error messages are printed */
	FILE *msgFd;	/* error message file pointer */
    } DK_Option;

#ifdef MAIN
DK_Option DK_option = {TRUE, stderr};
#else
extern DK_Option DK_option;
#endif  /* MAIN */

#endif  /* ifndef __TINY_SOFTTYPE_H__ */
