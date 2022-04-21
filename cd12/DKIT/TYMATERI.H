/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyMaterial.h,v $
   $Revision: 1.13 $ $Date: 1995/03/29 23:13:35 $
   Checkin by: $Author: ****** $

   This file contains a subset of the SOFTIMAGE's material data structure,
   its content is sufficient to read and write data structure into
   SOFTIMAGE's file format...

   Written by: Laurent Lauzon

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_MATERIAL_H__
#define __TINY_MATERIAL_H__

#include  "tySoftTy.h"
#include  "tyConstr.h"
#include  "tyTextur.h"
#include  "tyFile.h"
#include  "tyMental.h"

typedef
   enum
   {
      DK_MAT_CONSTANT = 1,
      DK_MAT_FLAT,
      DK_MAT_LAMBERT,
      DK_MAT_PHONG,
      DK_MAT_BLINN,
      DK_MAT_SHADOW = 6
   } DK_MaterialType;

#define DK_DECAY_TAB_SIZE 200
typedef float   DK_Rarray[2];
typedef struct
{
                   /* an exponential decay table */
    float       value;
    DK_Rarray     table[DK_DECAY_TAB_SIZE];
} DK_DecayTable;


typedef
   struct DK_material
   {
      DK_String		 name;		/* name of DK_material */
      DK_String		 prefix;	/* prefix of DK_material */
      DK_MaterialType	 type;		/* shading DK_model */
      DK_Colour24	 ambient; 	/* ambient colour */
      DK_Colour24	 diffuse;	/* diffuse colour */
      DK_Colour24	 specular;	/* specular colour */
      DK_Colour24	 attenuate;	/* attenuation */
      float     	 exponent;	/* decay exponant (for PHONG) */
      float		 reflectivity;	/* reflectivity factor */
      float		 reflectivityJitter;/* reflectivity jitter factor */
      float		 transparency;	/* transparency factor */
      float		 transparencyJitter;/* transparency jitter factor */
      float		 refracIndex;	/* refractive index */
      float		 motiontransp;	/* motion transparency */
      short		 nbTextures;
      DK_Texture	*textures;	/* list of globals DK_textures */
      short		 nbTextures3D;
      DK_Texture3D  	*textures3D;	/* list of globals 3D DK_textures */
      short		 glow;		/* object glow? */
      float		 coc;		/* amount of glow */

      DK_Fcurve		*fcv[DK_FCV_MAT_NUM];/* fcurves */
      DK_actFcurve	active_fcv[DK_FCV_MAT_NUM]; /* activation fcurves */

      struct DK_material    *next;
      
      /* static blur */
      DK_Boolean        staticblur;
      float             width;
      DK_DecayTable     decay;
      
      /* Shaders */
      DK_UDF	    *mat_shader;    /* user defined shader mental ray */
      DK_UDF	    *vol_shader;    /* user defined shader mental ray */
      DK_UDF	    *shdw_shader;    /* user defined shader mental ray */
      DK_Constraint	*constr_list;

      /* Display information */
      float             schem_posx;     /* schematic position x */
      float             schem_posy;     /* schematic position y */
      short             schem_userppos; /* schematic user position flag */

  } DK_Material;

/* globals DK_Materials prototypes */
extern	DK_Material *DK_materialReadBinary( FILE *, DK_FileProt * );
extern	DK_Material *DK_materialReadAscii( FILE * );
extern	DK_Material *DK_materialAllocate( void );
extern	void         DK_materialDispose( DK_Material ** );
extern	DK_Boolean   DK_materialWriteAscii( FILE *, DK_Material *, DK_String, short );
extern	DK_Boolean   DK_materialWriteBinary( FILE *, DK_Material *, DK_FileProt *);
extern	DK_Material *DK_materialReadFile( DK_String );
extern	DK_Boolean   DK_materialWriteFile( DK_String, DK_Material *, DK_FileFormat );

extern int DK_materialWriteAsciiAnimation ( FILE *, DK_Material *);
extern int DK_materialReadAsciiAnimation ( FILE *, DK_Material *);
#endif  /* ifndef __TINY_MATERIAL_H__ */
