/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyLattice.h,v $
   $Revision: 1.4 $ $Date: 1992/12/18 14:01:40 $
   Checkin by: $Author: claude $

   This file contains a subset of the SOFTIMAGE's lattice data structure.

   Written by: Colin Hui

   (c) Copyright 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_LATTICE_H__
#define __TINY_LATTICE_H__

#include "tySoftTy.h"
#include "tyFcurve.h"

typedef
    enum 
    {
	DK_LAT_LINEAR,
	DK_LAT_CURVE
    } DK_LatInterp;

typedef
   struct DK_lattice
   {
      DK_Boolean    active;
      short         nbx;	/* number of points in the x axis */
      short	    nby;	/* number of points in the y axis */
      short	    nbz;	/* number of points in the z axis */
      DK_LatInterp  interpx;	/* interpolation of points in the x axis */
      DK_LatInterp  interpy;	/* interpolation of points in the y axis */
      DK_LatInterp  interpz;	/* interpolation of points in the z axis */
      DK_Vector     *array;	/* point array */
      short	    *flag;	/* point flag, e.g. selected/unselected */
      DK_Vector      min;
      DK_Vector	     max;
      DK_Vector	     scl;

      struct DK_lattice *next;
   } DK_Lattice;

extern DK_Lattice	*DK_latticeAllocate(void);
extern void 		DK_latticeDispose ( DK_Lattice **);
extern int 		DK_latticeWriteAsciiAnimation ( FILE *, short, 
				DK_Lattice *, DK_Fcurve *);
extern int 		DK_latticeReadAsciiAnimation ( FILE *, int, short *, 
				DK_Lattice **, DK_Fcurve **);

#endif  /* ifndef __TINY_LATTICE_H__ */
