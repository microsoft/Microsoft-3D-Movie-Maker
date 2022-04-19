/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyLight.h,v $
   $Revision: 1.17 $ $Date: 1994/11/02 01:50:20 $
   Checkin by: $Author: rejean $

   This file contains a subset of the SOFTIMAGE's light data structure.

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_LIGHT_H__
#define __TINY_LIGHT_H__

#include "tySoftTy.h"
#include "tyFcurve.h"
#include "tyConstr.h"
#include "tyMental.h"
#include <sys/types.h>
#include <time.h>


/*----------------------*/
/* Light data structure */
/*----------------------*/

typedef
   enum
   {
      DK_LIGHT_POINT = 1,
      DK_LIGHT_SPOT,
      DK_LIGHT_INFINITE,
      DK_LIGHT_SUN
   } DK_LightType;

typedef
   enum
   {
      DK_LIGHT_NOSHADOW = 1,
      DK_LIGHT_SHADOW,
      DK_LIGHT_ZSHADOW,
      DK_LIGHT_SOFTSHADOW
   } DK_ShadowType;

typedef struct
{
   char*    name;          /* Name of the observer's position. */
   int      latitude;      /* Latitude of the observer.        */
   int      longitude;     /* Longitude of the observer.       */
   short   dayst;         /* Day light saving time.           */
   time_t      date_time;     /* Current date and time.           */
} DK_SunInfo;


/*************************************/
/* Rectangle geometry for area light */
/*************************************/
typedef struct {
   DK_Vector     vertex[4];
} DK_AREA_LIGHT_RECT;

/**********************************/
/* Sphere geometry for area light */
/**********************************/
typedef struct {
   float      radius;
} DK_AREA_LIGHT_SPHERE;

/********************************/
/* Disk geometry for area light */
/********************************/
typedef struct {
   DK_Vector     dir;
   float      radius;
} DK_AREA_LIGHT_DISK;

/****************************/
/* Area light geometry id's */
/****************************/
typedef enum {
   DK_AREA_RECT,
   DK_AREA_SPHERE,
   DK_AREA_DISK,
   DK_AREA_NONE
} DK_AREA_LIGHT_TYPE;


typedef
   struct DK_light_def
   {
      DK_String		name;		/* name of light */
      DK_String		prefix;		/* prefix of light */
      DK_LightType	type;		/* type of light */

      DK_Vector		position;	/* location */
      DK_Colour24	colour;		/* r,g,b colour */
      float		startFalloff;	/* start to fall off */
      float		endFalloff;	/* end to fall off */
      float		shadowFactor;	/* shadow factor (0..1) */

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

      /* used only if type light is a spot */
      DK_Vector		spotInterest;		/* point of interest */
      float		spotConeAngle;		/* cone angle */
      float		spotSpreadAngle;	/* spread angle */

      /* shadow */
      DK_ShadowType	shadowType;
      int		shadowMapRes;
      int		shadowFilterSize;
      int		shadowFilterStep;
      float		shadowFadeStart;
      float		shadowFadeRate;

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
      DK_Fcurve		*fcv[DK_FCV_LIG_NUM];	/* fcurves */
      DK_actFcurve	active_fcv[DK_FCV_LIG_NUM]; /* activation fcurves */

      int		selective;
      int		inc_exc;   /* include the light of exclude this light */

      DK_SunInfo	*sun;	    /* Info for the sun position.   */
      DK_UDF		*shader;    /* user defined shader mental ray */
      struct DK_light_def   *next;	/* next light */

      /*********************/
      /* Area light fields */
      /*********************/
      DK_Boolean          area_light;    /* Indicates if area light */
      DK_AREA_LIGHT_TYPE  area_type;     /* Type of area light */
      int                 area_u_sample; /* U sampling rate */
      int                 area_v_sample; /* V sampling rate */
      union {                            /* Area light geometry */
	 DK_AREA_LIGHT_RECT         rect;
	 DK_AREA_LIGHT_SPHERE       sphere;
	 DK_AREA_LIGHT_DISK         disk;
      } area_geo;
      DK_Boolean          area_local;    /* TRUE if working with respect to
					    light's local coordinate system
                                            (valid only for spots)          */
      DK_Vector           area_scale;    /* Scaling factor for area light */
      DK_Vector           area_rot;      /* Rotation for area light */
      DK_Boolean          area_selected; /* TRUE if area geometry is selected */
      DK_Matrix           area_rot_mat;
   } DK_Light;

/* Global DK_Light prototypes */
extern	DK_Light  *DK_lightAllocate( void );
extern	void       DK_lightDispose( DK_Light ** );
extern  int 	   DK_lightReadAsciiAnimation ( FILE *, DK_Light *);
extern  int 	   DK_lightWriteAsciiAnimation ( FILE *, DK_Light *);

#endif  /* ifndef __TINY_LIGHT_H__ */
