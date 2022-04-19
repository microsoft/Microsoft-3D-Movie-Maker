/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyFace.h,v $
   $Revision: 1.5 $ $Date: 1994/10/31 15:42:54 $
   Checkin by: $Author: laurentr $

   This file contains a subset of the SOFTIMAGE's face data structure,
   its content is sufficient to read and write data structure into
   SOFTIMAGE's file format...

   Written by: Laurent Lauzon

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_FACE_H__
#define __TINY_FACE_H__

#include  "tySoftTy.h"
#include  "tySpline.h"
#include  "tyFile.h"
#include  "tyNurbs.h"

typedef
   struct
   {
      DK_Spline	 *exterior;	/* outside contour */
      short	  nbHoles;
      DK_Spline	**holes;	/* array of DK_spline pointers defining holes */
      DK_NurbsCrv** crv;	/* array of pointers to Nurbs Curves          */
				/* for NURBS faces                            */
      DK_Boolean isSpline;	/* TRUE if regular face, FALSE if NURBS face  */
   } DK_Face;

/* Global face prototypes */

extern	DK_Face    *DK_faceAllocate( void );
extern	void        DK_faceDispose( DK_Face ** );
extern	DK_Boolean  DK_faceWriteBinary( FILE *, DK_Face *, DK_FileProt *);
extern	DK_Face    *DK_faceReadBinary( FILE *, DK_FileProt *);
extern	DK_Boolean  DK_faceWriteAscii( FILE *, DK_Face *, DK_String );
extern	DK_Face    *DK_faceReadAscii( FILE * );

#endif  /* ifndef __TINY_FACE_H__ */
