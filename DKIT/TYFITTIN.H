/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyFitting.h,v $
   $Revision: 1.8 $ $Date: 1994/04/06 21:19:01 $
   Checkin by: $Author: diane $

   This file contains a subset of the SOFTIMAGE's fitting data structure.

   Written by: Colin Hui

   (c) Copyright 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_FITTING_H__
#define __TINY_FITTING_H__

#include "tySoftTy.h"
#include "tyFcurve.h"

typedef
   struct DK_spl_fitting
   {
      int 		active;
      struct DK_spline  *spl;		/* old scene file format */
      DK_String		name;		/* new v2.5 scene file format */
      DK_Vector         scal;
      float             roll;
      DK_Vector         trans;

      DK_Fcurve		*fcv[DK_FCV_SFIT_NUM];	/* fcurves */
      DK_actFcurve	active_fcv[DK_FCV_SFIT_NUM]; /* activation fcurves */
   } DK_Spl_fitting;

typedef
   struct DK_ptch_fitting
   {
      int 		active;
      DK_String		name;
      DK_Vector         scal;
      DK_Vector         rot;
      DK_Vector         trans;

      DK_Fcurve		*fcv[DK_FCV_PFIT_NUM];	/* fcurves */
      DK_actFcurve	active_fcv[DK_FCV_PFIT_NUM]; /* activation fcurves */
   } DK_Ptch_fitting;

/* Global DK_spline fitting prototypes */
extern  DK_Spl_fitting	*DK_spl_fittingAllocate(void);
extern	void         	DK_spl_fittingDispose( DK_Spl_fitting ** );
extern  int 		DK_splineFitWriteAsciiAnimation ( FILE *, DK_Spl_fitting *);
extern  int 		DK_splineFitReadAsciiAnimation ( FILE *, DK_Spl_fitting **);

/* Global patch fitting prototypes */
extern  DK_Ptch_fitting	*DK_ptch_fittingAllocate(void);
extern	void          	DK_ptch_fittingDispose( DK_Ptch_fitting ** );
extern  int 		DK_patchFitWriteAsciiAnimation ( FILE *, DK_Ptch_fitting *);
extern  int 		DK_patchFitReadAsciiAnimation ( FILE *, DK_Ptch_fitting **);

#endif  /* ifndef __TINY_FITTING_H__ */
