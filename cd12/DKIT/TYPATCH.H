/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyPatch.h,v $
   $Revision: 1.3 $ $Date: 1994/06/10 14:54:35 $
   Checkin by: $Author: martin $

   This file contains a subset of the SOFTIMAGE's patch data structure,
   its content is sufficient to read and write data structure into
   SOFTIMAGE's file format...

   Written by: Laurent Lauzon

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_PATCH_H__
#define __TINY_PATCH_H__

#include  "tySoftTy.h"
#include  "tySpline.h"
#include  "tyFile.h"

#define DK_PARAMETRIC 1
#define DK_SPACIAL    2
#define DK_CURVATURE  3


typedef struct
{
   DK_Boolean  viewDep; /* View dependencies:: FALSE: cam space, TRUE: pixel */
   float    spacial; /* maximum edge lenth */
   float    curv_u, curv_v;   /* Curvature */
   unsigned short   recMin, recMax;   /* recursivity */
   DK_Boolean  recursion;      /* recursion or not */
} DK_MRApprox;


typedef
   struct
   {
      DK_SplineType uType,     vType;	/* type of spline in both direction */
      short         uNbPoints, vNbPoints; /* dimension of ctrl points array */
      short         uClose,    vClose;	/* wrap around of array in both way */
      float         uTension,  vTension;/* tension of spline in both way */
      short         uStep,     vStep;	/* drawing step of patch (is also the
					   subdivision) */
      short         uCurve,    vCurve;	/* number of curve drawn */
      short         uBottom,   vBottom;	/* automatic closing of extremyty */
      short         uTop,      vTop;	/* 	"	"	"	*/
      short	    faceted;		/* Future use. patch will be rendered
					   faceted */
      DK_Point      *points;		/* array of control points (**1) */
      short         *sel;               /* tagged points */
      
      DK_MRApprox    approx;         /* mental ray approximation method */
      unsigned short approx_type;
   } DK_Patch;

/* Global patch prototypes */

extern	DK_Patch   *DK_patchAllocate( void );
extern	void        DK_patchDispose( DK_Patch ** );
extern	DK_Vector  *DK_patchAllocatePoints( int, int );
extern	DK_Patch   *DK_patchReadAscii( FILE * );
extern	DK_Patch   *DK_patchReadBinary( FILE *, DK_FileProt * );
extern	DK_Boolean  DK_patchWriteAscii( FILE *, DK_Patch *, DK_String );
extern	DK_Boolean  DK_patchWriteBinary( FILE *, DK_Patch *, DK_FileProt * );


/*******************************************************************************

Notes (1) the array is organize in V increment first
	  i.e. [0,0], [0,1], [0,2], ... [0,vNbPoints-1], [1,0], ...

*******************************************************************************/


#endif  /* ifndef __TINY_PATCH_H__ */
