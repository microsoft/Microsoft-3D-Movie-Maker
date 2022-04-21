/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyCamera.h,v $
   $Revision: 1.11 $ $Date: 1994/11/15 15:17:11 $
   Checkin by: $Author: ****** $

   This file contains a subset of the SOFTIMAGE's camera data structure.

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_CAMERA_H__
#define __TINY_CAMERA_H__

#include "tySoftTy.h"
#include "tyFcurve.h"
#include "tyConstr.h"

/* correct spelling in userDoc BUT preserved in C code for Bwrd Compatibility */

#define focalLenght focalLength
/*-----------------------*/
/* Camera data structure */
/*-----------------------*/

typedef
   enum
   {
      DK_CAM_DOF_OFF,
      DK_CAM_DOF_AUTOMATIC,
      DK_CAM_DOF_CUSTOM
   } DK_DepthOfFieldType;

typedef
   struct
   {
      DK_Vector		position;	/* position of camera */
      DK_Vector		interest;	/* point of interest */

      /* position/interest constraints */
      DK_String		pos_constraint_name;	/* position constraint name */
      short		pos_constraint_type;	/* position constraint type */
      DK_String		int_constraint_name;	/* interest constraint name */
      short		int_constraint_type;	/* interest constraint type */

      /* position/interest translation path */
      struct DK_spline	*pos_path;		/* old scene file format */
      struct DK_spline	*int_path;
      DK_String		pos_path_name;		/* new v2.5 scene file format */
      DK_String		int_path_name;

      /* orientation */
      float		yaw;
      float		pitch;
      float		roll;		/* roll angle */

      /* clipping planes */
      float		nearPlane;	/* distance to near plane */
      float		farPlane;	/* distance to far plane */

      /* lens */
      float		focalLength;	/* focal length */
      float		fstop;		/* f/stop */
      float		focalDistance;	/* focal distance */
      float		fieldOfView;	/* field of view */

      /* depth of field */
      DK_DepthOfFieldType	depthOfField;	/* depth of field? */
      float		nearFocus;	/* near focus */
      float		farFocus;	/* far focus */
      float		maxCOC;		/* max COC */
      float		outOfFocus;	/* out of focus */

      DK_Boolean	enlargeNearObjects;	/* enlarge near objects? */

      /* display information */
      short		wire_colour;	/* wire frame colour */
      DK_LineType	line_type;	/* line display type */
      short 		selected;	/* selection type */
      DK_Boolean	hidden;		/* visible? */
      float             schem_posx;     /* schematic position x */
      float             schem_posy;     /* schematic position y */
      float             schem_intx;     /* schematic position of interest x */
      float             schem_inty;     /* schematic position of interest y */
      short             schem_userppos; /* schematic user position flag */
      short             schem_userpint; /* schematic user position flag */

      DK_Constraint	*constr_list;
      DK_Fcurve		*fcv[DK_FCV_CAM_NUM];	/* fcurves */
      DK_actFcurve	active_fcv[DK_FCV_CAM_NUM]; /* activation fcurves */
   } DK_Camera;

/* global routines */
extern void	DK_cameraInitialize( DK_Camera * );
extern int 	DK_cameraWriteAsciiAnimation ( FILE *, DK_Camera *);
extern int 	DK_cameraReadAsciiAnimation ( FILE *, DK_Camera *);

#endif  /* ifndef __TINY_CAMERA_H__ */
