/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyScene.h,v $
   $Revision: 1.18 $ $Date: 1995/02/02 19:03:33 $
   Checkin by: $Author: a_tan $

   This file contains a subset of the SOFTIMAGE's scene data structure,
   its content is sufficient to read and write data structure into
   SOFTIMAGE's scene file format...

   Written by: Dominique Boisvert, Colin Hui

   (c) Copyright 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_SCENE_H__
#define __TINY_SCENE_H__

#include  "tySoftTy.h"
#include  "tyCamera.h"
#include  "tyFcurve.h"
#include  "tyFile.h"
#include  "tyLight.h"
#include  "tyModel.h"
#include  "tySpline.h"
#include  "tyTextur.h"
#include  "tyWave.h"
#include  "tyMental.h"


/*--------------------------------*/
/* Rendering setup data structure */
/*--------------------------------*/

typedef
   enum
   {
      DK_REN_WIREFRAME,
      DK_REN_HIDDENLINE_SMOOTHED,
      DK_REN_HIDDENLINE,           /* Faceted */
      DK_REN_DEPTHCUE,
      DK_REN_SOFTIMAGE,
      DK_REN_HARDWARE,
      DK_REN_SUBREGION,
      DK_REN_GHOST,
      DK_REN_ROTOSCOPE_WIRE,
      DK_REN_ROTOSCOPE_SHADE,
      DK_REN_MENTALRAY
   } DK_RenderType;

typedef
   enum
   {
      DK_REN_CLIP_COMPONENT,
      DK_REN_CLIP_DESATURATE
   } DK_ColourClipping;

typedef
   enum
   {
      DK_REN_ANTIALIAS_BARTLET,
      DK_REN_ANTIALIAS_ADAPTIVE,
      DK_REN_ANTIALIAS_NONE
   } DK_AntialiasType;

typedef
   enum
   {
     DK_FMT_35MM_137,
     DK_FMT_35MM_166,
     DK_FMT_35MM_185,
     DK_FMT_70MM_PANAVISION,
     DK_FMT_70MM_IMAX,
     DK_FMT_70MM_OMNIMAX,
     DK_FMT_NTSC,
     DK_FMT_PAL,
     DK_FMT_HDTV,
     DK_FMT_SLIDE_35MM,
     DK_FMT_6CM_X_6CM,
     DK_FMT_6CM_X_9CM,
     DK_FMT_4in_X_5in,
     DK_FMT_CUSTOM_ASPECT,
     DK_FMT_CUSTOM_SIZE
   } DK_PictureFormat;

typedef
   struct
   {
      DK_Boolean	active;			/* active? */
      float		start;			/* fade start */
      float		end;			/* fade end */
      DK_Colour24	colour;			/* colour to fade to */
      float		transparency;		/* fade transparency factor */
      DK_Fcurve		*fcv[DK_FCV_FAD_NUM];	/* fcurves */
      DK_actFcurve	active_fcv[DK_FCV_FAD_NUM]; /* activation fcurves */
   } DK_Fade;

/* Layer-Fog -MKL- */
typedef
   struct
   {
      DK_Boolean        active;                 /* active? */
      float             start;                  /* fog start */
      float             end;                    /* fog end */
      DK_Colour24       colour;                 /* colour to fog to */
      float             density;           	/* fog density factor */
      float             base;           	/* fog base factor */
      float             thickness;           	/* fog thickness factor */
      DK_Fcurve         *fcv[DK_FCV_FOG_NUM];   /* fcurves */
      DK_actFcurve	active_fcv[DK_FCV_FOG_NUM]; /* activation fcurves */
   } DK_Fog;



typedef
   struct
   {
      DK_RenderType	type;		/* type of rendering */

      /* frame sequence */
      short		start;		/* first frame */
      short		end;		/* last frame */
      short		step;		/* frame step */

      /* output */
      DK_Boolean	frameBuffer;	/* to frame buffer? */
      DK_Boolean	pictureFile;	/* to picture file? */
      DK_String		pictureFileName; /* picture file name */

      /* pixels */
      short		xResolution;	/* horizontal resolution */
      short		yResolution;	/* vertical resolution */
      float		pixelRatio;	/* pixel ratio */

      /* picture format */
      DK_PictureFormat	picFormat;	/* picture format */
      float		aspectRatio;	/* custom aspect ratio */
      float		aspectWidth;	/* custom size width */
      float		aspectHeight;	/* custom size height */

      /* antialiasing */
      DK_AntialiasType	antialiasType;		/* antialiasing type */
      short		samplingFilter;		/* sampling filter */
      short		maxLevel;		/* sampling level */
      float		adaptiveThreshold;	/* adaptive threshold */

      DK_Boolean	backCulling;		/* back culling? */
      DK_ColourClipping	colourClip;		/* type of colour clipping */
      DK_Colour24	ambiance;		/* ambiant colour */

      /* ray-tracing */
      short		rayMaxDepth;		/* max ray depth */
      short		rayTrianglesPerLeaf;	/* triangles per tree leaf */
      short		rayMaxTreeDepth;	/* max tree depth */

      /* Layer-Fog */
      DK_Fog		fog;			/* Layer-Fog information */

      /* fading */
      DK_Fade		fade;			/* fading information */

      /* pre-frame & post-frame script files */
      DK_String		preFrameFile;	/* pre-frame script file name */
      DK_String		postFrameFile;	/* post-frame script file name */

      /* motion blur */
      short		motion_blur;
      float		shutter_speed;
      float		min_movement;

      /* field rendering */
      DK_Boolean    	onfields;           	/* field rendering */
      short      	firstfield;         	/* first field even or odd */

      /* statistics */
      DK_String		statFile;		/* statistics file name */
      DK_Boolean	statTiming;		/* timing info? */
      DK_Boolean	statMemory;		/* memory info? */
      DK_Boolean	statTree;		/* BSP tree info? */
      DK_Boolean	statLine;		/* line info? */

      /* Dithering */
      int		do_dither;
      int		add_noise;
      float		noise_amount;

      /* Mental Ray Custom shaders */
      int               nbOutputShader;
      int               nbLensShader;
      DK_UDF            *outputShader;
      DK_UDF            *lensShader;
      DK_UDF		*atmosphere;
   } DK_RenderingSetup;

typedef 
   struct DK_udfFileInfo
   {
     char 			*filename, 
				*instance;
     struct DK_udfFileInfo 	*next;
   } DK_UdfFileInfo;

/*----------------------*/
/* DK_Scene data structure */
/*----------------------*/

typedef
   struct DK_scene
   {
      DK_String	name;		/* scene filename */

      /* paths */
      char	*object_path;	/* object path */
      char	*texture_path;	/* DK_texture path */
      char	*picture_path;	/* picture path */
      char	*material_lib;	/* DK_material lib */
      char	*texture_lib;	/* DK_texture lib */
      char	*palette_lib;	/* palette lib */

      /* play control */
      short	start;		/* first frame */
      short	end;		/* last frame */
      short	step;		/* step frame */
      short	pause;		/* pause frame */
      short	rate;		/* frame rate */
      short	current;	/* current frame */

      DK_Camera	camera;		/* camera */
      DK_Light	*light;		/* linked list of lights */
      DK_Model	*model;		/* linked list of DK_models in scene */
      DK_Spline *spline;	/* unattached DK_splines */

    /* DK_Waves */
      char	*wave_filename;	/* wave filename      */
      DK_Wave	*wave;		/* wave nodes 	      */

      DK_RenderingSetup	render;	/* setup for the renderer */

      char      *setup_filename;/* original setup file path */
      DK_UdfFileInfo *udfFileInfo; /* udf file names */
   } DK_Scene;

/* Global DK_Scene prototypes */
extern	DK_Scene  *DK_sceneAllocate( void );
extern	void	   DK_sceneDispose( DK_Scene ** );
extern  int 	   DK_sceneReadAsciiAnimation ( float *, DK_Scene *);
extern  int 	   DK_sceneWriteAsciiAnimation ( char *, float, DK_Scene *);


#endif  /* ifndef __TINY_SCENE_H__ */
