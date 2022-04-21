/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinySpline.h,v $
   $Revision: 1.4 $ $Date: 1992/12/18 14:01:56 $
   Checkin by: $Author: ******$

   This file contains a subset of the SOFTIMAGE's DK_spline data structure,
   its content is sufficient to read and write data structure into
   SOFTIMAGE's file format...

   Written by: Laurent Lauzon

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_SPLINE_H__
#define __TINY_SPLINE_H__

#include  "tySoftTy.h"
#include  "tyFile.h"

#define   DK_BEZ_DEFAULT  0

/* segment type */
#define   DK_BEZ_CURVE    1
#define   DK_BEZ_LINEAR   2
#define   DK_BEZ_ARC      3

/* constraint */
#define   DK_BEZ_C0   1
#define   DK_BEZ_C1   2
#define   DK_BEZ_C2   3
#define   DK_BEZ_G1   4
#define   DK_BEZ_G2   5

#define DK_KEY_SELECTED	0x0001

typedef
   struct DK_splinekey
   {
      DK_Point      previous;
      DK_Point      position;
      DK_Point      next;
      short         constraint;
      short         segment;
      short         flag;
   } DK_SplineKey;

typedef
   enum
   {
      DK_SPL_LINEAR,
      DK_SPL_BEZIER,
      DK_SPL_BSPLINE,
      DK_SPL_CARDINAL
   } DK_SplineType;

typedef
   struct DK_spline
   {
      DK_String		 name;		/* name of DK_spline */
      short		 phantom;	/* automatic phantom key (extremity)
						(CARDINAL, B-SPLINE) */
      short		 close;		/* is curve is close and wrap around */
      DK_SplineType	 type;		/* type of curve */
      short	 	 nbKeys;	/* number of keys in array */
      DK_SplineKey	 *keys;		/* Keys (Control DK_Points) array */
      float		 tension;	/* tension of curve (CARDINAL) */
      unsigned short	 step;		/* number of drawing step */
      short		 constraint;	/* default keys constraint (BEZIER) */
      short		 segment;	/* default segment for key (BEZIER) */
      DK_LineType	 line_type;	/* display line type */
      short		 wire_colour;	/* display wire frame colour */
      short		 selected;	/* display selection type */
      short		 keyframed;	

      struct DK_spline	*next;
   } DK_Spline;

/* Globals DK_spline prototypes */
extern	DK_Spline     *DK_splineAllocate( void );
extern	DK_SplineKey  *DK_splineKeyAllocate( short );
extern	void           DK_splineDispose( DK_Spline ** );
extern	DK_Boolean     DK_splineWriteBinary( FILE *, DK_Spline *, DK_FileProt * );
extern	DK_Boolean     DK_splineWriteAscii( FILE *, DK_Spline *, DK_String );
extern	DK_Spline     *DK_splineReadBinary( FILE *, DK_FileProt * );
extern	DK_Spline     *DK_splineReadAscii( FILE *, short *, DK_Spline *** );
extern	DK_SplineType  DK_splineGetTypeByName( char * );
extern	DK_Spline     *DK_splineReadFile( DK_String );
extern	DK_Boolean     DK_splineWriteFile( DK_String, DK_Spline *, DK_FileFormat );
extern  void        DK_splineShrinkBezier( DK_Spline * );
extern  int 	    DK_splineReadAsciiAnimation ( FILE *, DK_Spline *);
extern  int 	    DK_splineWriteAsciiAnimation ( FILE *, DK_Spline *);

#endif  /* ifndef __TINY_SPLINE_H__ */
