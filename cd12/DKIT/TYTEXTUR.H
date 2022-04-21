/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyTexture.h,v $
   $Revision: 1.13 $ $Date: 1994/12/09 23:42:10 $
   Checkin by: $Author: ****** $

   This file contains a subset of the SOFTIMAGE's DK_texture data structure,
   its content is sufficient to read and write data structure into
   SOFTIMAGE's file format...

   Written by: Laurent Lauzon

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_TEXTURE_H__
#define __TINY_TEXTURE_H__

#include  "tySoftTy.h"
#include  "tyConstr.h"
#include  "tyFile.h"
#include  "tyFcurve.h"

/******************************************************************************/
/*** 2D Texture maps **********************************************************/
/******************************************************************************/

typedef
   enum
   {
      DK_TXT_STATIC,
      DK_TXT_SEQUENCE,
      DK_TXT_LIST
   } DK_TextureAnim;

typedef
   enum
   {
      DK_TXT_XY = 1,
      DK_TXT_XZ,
      DK_TXT_YZ,
      DK_TXT_UV,
      DK_TXT_CYLINDRICAL,
      DK_TXT_SPHERICAL,
      DK_TXT_AFTER_REFLECT,
      DK_TXT_WITHOUT_REFLECT
   } DK_TextureMethod;

#define DK_TXT_ALPHA		1
#define DK_TXT_INTENSITY	2
#define DK_TXT_NO_MASK		3

typedef
   struct DK_texture
   {
      DK_String		 pic_name;	/* filename of picture (**1) */
      DK_String		 txt_name;	/* name of the texture itself */
      DK_String		 txt_prefix;	/* prefix of the texture itself */
      DK_TextureAnim	 animation;	/* tell how to get the right DK_texture
					   picture with name */
      DK_TextureMethod	 method;	/* projection mode */
      short		 uCount;	/* repetition of picture in U */
      short		 vCount;	/*	"	"	 in V */
      float		 uScal;		/* size of application zone in U (**2)*/
      float		 vScal;		/* 	"	"	"   in V */
      float		 uOffset;	/* offset of application zone in U */
      float		 vOffset;	/*	"	"	"     in V */
      short		 pixelInterp;	/* inter pixel smoothing */
      float		 ambientFct;	/* application factor on ambient (**3)*/
      float		 diffuseFct;	/*	""	""    on diffuse */
      float		 specularFct;	/*	""	""    on specular */
      float		 reflectFct;	/*	""	""    on reflectivity */
      float		 transpFct;	/*	""	""    on transparency */
      float		 roughnessFct;	/*	""	""    on roughness */
      float		 reflectMappingFct;/*	""	""    on reflectivity mapping */
      float		 rotation;	/* environment map rotation */
      short		 uvSwap;	/* swap col. and row of picture */
      short		 uAlternate;	/* alternate U in tiling (repetition) */
      short		 vAlternate;	/*	"    V	"	"	"  */
      short		 mask;		/* way the mix is done with DK_material */
      short		 effect;	/* channel use for bump, transp, refl */
      float		 blend;		/* blending factor */
      short		 cropping;	/* is sub image is took for DK_texture */
      short		 uMinCrop;	/* cropping infos */
      short		 uMaxCrop;	/*	"	  */
      short		 vMinCrop;	/*	"	  */
      short		 vMaxCrop;	/*	"	  */
      short		 pad;		/* buffer to avoid overide of i/o */

      DK_Vector   	 geo;		/* Geometry of txtsup 		*/
      DK_Vector   	 ctr;		/* Center of txtsup 		*/
      DK_Vector   	 rot;		/* Rotation of txtsup 		*/
      DK_Vector   	 trans;		/* Translation of txtsup 	*/
      DK_Vector   	 scal;		/* Scaling of txtsup		*/
      DK_Matrix   	 txtsup;	/* Matrix of transformation	*/
      short		 blkwht;	/* mapping Black and/or White	*/
      DK_Fcurve		*fcv[DK_FCV_TXT_NUM];	/* fcurves */
      DK_actFcurve	active_fcv[DK_FCV_TXT_NUM]; /* activation fcurves */

      struct DK_texture	*next;
	
      struct DK_udf     *shader;        /* user defined shader, mental ray */
      DK_Boolean   	displacement;   /* disp mapping, mental ray */

      DK_Constraint	*constr_list;

      /* Display information */
      float             schem_posx;     /* schematic position x */
      float             schem_posy;     /* schematic position y */
      short             schem_userppos; /* schematic user position flag */

  } DK_Texture;

/* Global prototypes of DK_Texture */

extern	DK_Texture *DK_textureAllocate( void );
extern	void        DK_textureDispose( DK_Texture ** );
extern	DK_Boolean  DK_textureWriteAscii( FILE *, DK_Texture *, DK_String, short );
extern	DK_Boolean  DK_textureWriteBinary( FILE *, DK_Texture *, DK_FileProt * );
extern	DK_Texture *DK_textureReadBinary( FILE *, DK_FileProt * );
extern	DK_Texture *DK_textureReadAscii( FILE * );
extern	DK_Texture *DK_textureReadFile( DK_String );
extern	DK_Boolean  DK_textureWriteFile( DK_String, DK_Texture *, DK_FileFormat );

/*******************************************************************************

Notes: (1) if animation == STATIC	
		DK_texture picture = name(.pic)
	   if animation == SEQUENCE
		DK_texture picture = name(.currentFrame)(.pic)
	   if animation == LIST
		DK_texture picture =((currentFrame)th line of file name)(.pic)

       (2) the size interpretation differ from projection methods.

       (3) The 'switch' to say affect this channel is based on the value
	   of this factor; the switch is ON when the value is different than
	   0.0. 

*******************************************************************************/

/******************************************************************************/
/*** 3D Procedural textures ***************************************************/
/******************************************************************************/

typedef
   enum
   {
      DK_TXT_3D_MARBLE,
      DK_TXT_3D_WOOD,
      DK_TXT_3D_CLOUD
   } DK_Texture3DType;

typedef
   struct DK_texture3D
   {
      DK_String		 name;		/* name of the tetxure */
      DK_String		 prefix;	/* prefix of the tetxure */
      DK_Texture3DType	 type;		/* type of method used */
      float		 spacing;	/* frequency of slice */
      float		 taper;		/* angle for WOOD cone */
      float		 strength;	/* strength of pertubation */
      short		 nbIterations;	/* level sub-refinement computation */
      float		 power;		/* 'noise' parameter */

      short		 mask;		/* way the mix is done with DK_material */
      short		 effect;	/* channel for bump, transp & refl */
      float		 blend;		/* blending factor */

      DK_Colour32	 colour0;	/* colour key 0 to make colour spread */
      DK_Colour32	 colour1;	/*	"     1		"	"     */
      DK_Colour32	 colour2;	/*	"     2		"	"     */
      DK_Colour32	 colour3;	/*	"     3		"	"     */
      DK_Colour32	 colour4;	/*	"     4		"	"     */

      float		 factor1;	/* position of colour key 1 (**1) */
      float		 factor2;	/*	"	"	  2 */
      float		 factor3;	/*	"	"	  3 */

      DK_Vector		 scaling;	/* scaling in DK_texture coordinate (**2)*/
      DK_Vector		 rotation;	/* rotation 	" 	"	*/
      DK_Vector		 translation;	/* translation	"	"	*/

      float		 ambientFct;	/* application factor on ambient (**3)*/
      float		 diffuseFct;	/*	""	""    on diffuse */
      float		 specularFct;	/*	""	""    on specular */
      float		 reflectFct;	/*	""	""    on reflectivity */
      float		 transpFct;	/*	""	""    on transparency */
      float		 roughnessFct;	/*	""	""    on roughness */

      DK_Fcurve		*fcv[DK_FCV_TXT3D_NUM];	/* fcurves */
      DK_actFcurve	active_fcv[DK_FCV_TXT3D_NUM]; /* activation fcurves */

      struct DK_texture3D	*next;
      
      struct DK_udf     *shader;        /* user defined shader, mental ray */
      DK_Boolean   	displacement;   /* disp mapped */

      DK_Constraint	*constr_list;

      /* Display information */
      float             schem_posx;     /* schematic position x */
      float             schem_posy;     /* schematic position y */
      short             schem_userppos; /* schematic user position flag */

   } DK_Texture3D;

/* Globals prototypes of 3D texture */
extern	DK_Texture3D *DK_texture3DAllocate( void );
extern	void          DK_texture3DDispose( DK_Texture3D ** );
extern	DK_Boolean    DK_texture3DWriteBinary( FILE *, DK_Texture3D *, DK_FileProt * );
extern	DK_Boolean    DK_texture3DWriteAscii( FILE *, DK_Texture3D *, DK_String, short );
extern	DK_Texture3D *DK_texture3DReadBinary( FILE *, DK_FileProt * );
extern	DK_Texture3D *DK_texture3DReadAscii( FILE * );
extern	DK_Texture3D *DK_texture3DReadFile( DK_String );
extern	DK_Boolean    DK_texture3DWriteFile( DK_String, DK_Texture3D *, DK_FileFormat );
extern int DK_texture3DWriteAsciiAnimation ( FILE *, DK_Texture3D *);
extern int DK_texture3DReadAsciiAnimation ( FILE *, DK_Texture3D *);
extern int DK_textureWriteAsciiAnimation ( FILE *, DK_Texture *);
extern int DK_textureReadAsciiAnimation ( FILE *, DK_Texture *);

/*******************************************************************************

Notes: (1) position for colour key 0 is 0.0, and for key 4 it's 1.0.

       (2) you can see it as a cube animated in the world, the cube is the
	   bounding box of the object and the world is a 3D DK_texture one
	   define by either slice or concentric cylinder.

       (3) The 'switch' to say affect this channel is based on the value
	   of this factor; the switch is ON when the value is different than
	   0.0. 

*******************************************************************************/

#endif  /* ifndef __TINY_TEXTURE_H__ */

