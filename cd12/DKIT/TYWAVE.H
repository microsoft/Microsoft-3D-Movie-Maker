/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyWave.h,v $
   $Revision: 1.9 $ $Date: 1994/04/06 21:10:07 $
   Checkin by: $Author: ******$

   This file contains a subset of the SOFTIMAGE's wave data structure.

   Written by: Colin Hui

   (c) Copyright 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_WAVE_H__
#define __TINY_WAVE_H__

#include "tySoftTy.h"
#include "tyFcurve.h"
#include "tyConstr.h"

typedef
   struct DK_wave_amodel
   {
      DK_String			model_name;	/* attach model name */
      DK_Boolean		model_branch;	/* attach model branch */

      struct DK_wave_amodel	*next;
   } DK_Wave_amodel;

typedef
   struct DK_wave
   {
      DK_String		wave_name;
      DK_String		wave_prefix;
      DK_Vector         scale;
      DK_Vector         rotate;
      DK_Vector         translation;
      float		decay;
      float		spread;

      DK_String		pos_constraint_name;
      short		pos_constraint_type;
      struct DK_spline	*path;			/* old scene file format */
      DK_String		path_name;		/* new v2.5 scene file format */

      DK_Constraint	*constr_list;
      DK_Fcurve		*fcv[DK_FCV_WAVE_NUM];	/* fcurves */
      DK_actFcurve	active_fcv[DK_FCV_WAVE_NUM]; /* activation fcurves */

      struct DK_wave_amodel *w_amodel;
      struct DK_wave	    *next;
   } DK_Wave;

extern DK_Wave_amodel *DK_wave_amodelAllocate(void);
extern void	    DK_wave_amodelDispose(DK_Wave_amodel **);
extern DK_Wave	   *DK_waveAllocate(void);
extern void	    DK_waveDispose(DK_Wave **);
extern int 	    DK_waveWriteAsciiAnimation ( FILE *, DK_String, DK_Wave *);
extern int 	    DK_waveReadAsciiAnimation ( FILE *, DK_String *, DK_Wave **);

#endif  /* ifndef __TINY_WAVE_H__ */
