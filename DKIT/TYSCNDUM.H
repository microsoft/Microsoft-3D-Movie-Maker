/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyScnDump.h,v $
   $Revision: 1.3 $ $Date: 1994/11/08 22:50:51 $
   Checkin by: $Author: diane $

   This file contains the function prototypes to print the contents of
   a DKit scene.

   (c) Copyright 1994 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/
#ifndef __TINY_SCNDUMP_H__
#define __TINY_SCNDUMP_H__

extern void DK_aniDump  ( DK_Fcurve   **ptr, DK_actFcurve *act, int nbfcvs );
extern void DK_constraintDump
			( DK_Constraint *cns_list );
extern void DK_t2dDump  ( DK_Texture   *t2d, int global );
extern void DK_t3dDump  ( DK_Texture3D *t3d, int global );
extern void DK_matDump  ( DK_Material  *mat );

extern void DK_modelDump( DK_Model  *mdl );
extern void DK_camDump  ( DK_Camera *cam );
extern void DK_ligDump  ( DK_Light  *lig );
extern void DK_wavDump  ( DK_Wave   *wav );

extern void DK_scnDump  ( DK_Scene  *scn );

extern void DK_renderSetupDump( DK_RenderingSetup *ren );

#endif
