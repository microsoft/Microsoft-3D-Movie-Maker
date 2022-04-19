/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyModel.h,v $
   $Revision: 1.31 $ $Date: 1995/04/02 00:46:48 $
   Checkin by: $Author: davidle $

   This file contains a subset of the SOFTIMAGE's model data structure,
   its content is sufficient to read and write data structure into
   SOFTIMAGE's file format...

   Written by: Laurent Lauzon

   (c) Copyright 1990 - 1994 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_MODEL_H__
#define __TINY_MODEL_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include  "tySoftTy.h"
#include  "tyFcurve.h"
#include  "tyConstr.h"
#include  "tyFile.h"
#include  "tyFace.h"
#include  "tyPatch.h"
#include  "tyMesh.h"
#include  "tyMateri.h"
#include  "tyTextur.h"
#include  "tyFcurve.h"
#include  "tyFittin.h"
#include  "tyLattic.h"
#include  "tyActor.h"
#include  "tyDynami.h"
#include  "tyCluste.h"
#include  "tyMetaba.h"

#ifndef __FORWARD_TYPEDEFS_TINYMODEL
#define __FORWARD_TYPEDEFS_TINYMODEL
typedef struct DK_model      DK_Model;
#endif

typedef
   enum
   {
      DK_MDL_LINK = -1, /* a temporary link for the rendering */
      DK_MDL_NILL,      /* a NULL node */
      DK_MDL_PTCH,      /* a Patch node */
      DK_MDL_FACE,      /* a Face node */
      DK_MDL_PMSH,      /* a Pmesh node (no more used) */
      DK_MDL_MESH,      /* a Super-mesh node */
      DK_MDL_JNT,       /* a Joint node for skeleton animation */
      DK_MDL_SPLN,      /* a spline !!!! */
      DK_MDL_META,	/* a MetaSystem node */
      DK_MDL_BALL,	/* a Metaball node */
      DK_MDL_CRV,	/* NURBS curve node */
      DK_MDL_SRF	/* NURBS surface node */
   } DK_ModelType;

/* types of envelopes to cover a Joint node: */
#ifndef DK_envelope_type
typedef
   enum DK_envelope_type
   {
      DK_ENONE,         /* no envelope */
      DK_EUMDL,         /* an unmodified DK_model as envelope */
      DK_EFMDL,         /* a DK_model 'fitted' to the limb (not used) */
      DK_ETUBG,         /* tubing (not used) */
      DK_EFLEX,         /* flexible envelope */
      DK_ELOCL,         /* local envelope */
      DK_EGLOB,         /* global envelope */
      DK_ERIGD,         /* rigid envelope */
      DK_EAUTO          /* automatic envelope */
   } DK_EnvelopeType;
#endif

/* types of algorithm */
typedef
   enum DK_algo_type
   {
      DK_ASTD,        /* + No algorithm specified 			*/
      DK_AIKIN,       /* + Inverse kinematics 				*/
      DK_ADYNA,       /* + Dynamics 					*/
      DK_AIKLEAF,     /* . Inverse kinematic chain's leaf		*/
      DK_ADYNLEAF,    /* . Dynamic chain's leaf				*/
      DK_ADYNGRAV,    /* . Dynamic Gravity constraint			*/
      DK_ADYNFRCE,    /* . Dynamic Force constraint			*/
      DK_ADYNWIND,    /* . Dynamic Wind (Force considered as...)	*/
      DK_ADYNDGRAV,   /* . Dynamic Default Gravity constraint		*/
      DK_ADYNFAN,     /* . Dynamic Local Wind (ventilator)		*/
      DK_ADYNNAIL,    /* . Dynamic Positional Constraint		*/
      DK_ADYNSPRDMP,  /* . Dynamic Spring and Damper Unit (not used)    */
      DK_ADYNMDL,     /* + Dynamic DK_model not supported by a joint	*/
      DK_ACUSICON     /* + Motion custom icon.                          */
   } DK_AlgoType;

typedef
   enum 
   {
      DK_NULL_SHAPE_INTERP = -1,
      DK_LINEAR_SHAPE_INTERP,
      DK_CARDINAL_SHAPE_INTERP,
      DK_WEIGHT_SHAPE_INTERP
   } DK_ShapeInterpType;
   
typedef
   struct DK_srt_vect
   {
      DK_Vector	  scl;
      DK_Vector	  rot;
      DK_Vector	  trs;
   } DK_SRT_Vect;

/* for Render Setup stuff */
/* Visibility Flags       */
#define   DK_PRIMARY_VIS        1
#define   DK_TRANSMIT_VIS       2
#define   DK_SHADOW_VIS         4

/* Motion Blur Flags      */
#define   DK_MB_NONE            0
#define   DK_MB_LINEAR          1
#define   DK_MB_EXACT           2

/*----------------------------------------------------------------*/
/* Main structure for a DKIT Model. Some fields in this structure */
/* are maintained only for compatibility with older versions.     */
/*----------------------------------------------------------------*/
struct DK_model
   {
      /*--------------------*/
      /* Global information */
      /*--------------------*/
      DK_String		 filename;	/* model filename */
      DK_String		 name;		/* model name */
      DK_String		 prefix;	/* model prefix */
      DK_ModelType	 type;		/* type of data found in definition */
      DK_Pointer	 definition;	/* geometry */
      DK_Pointer	 Kdefinition;	/* deformed geometry */

      /*-------------------------*/
      /* transformation (NOTE 1) */
      /*-------------------------*/
      DK_Vector		 scaling;
      DK_Vector		 rotation;	/* vector of rotation (NOTE 2) */
      DK_Vector		 translation;

      /*----------------------------------------------------------*/
      /* constraints      					  */
      /*							  */
      /* Field compatible with CREATIVE Environment 2.6 and below */
      /* For later scenes, use the field 'constr_list' (list)     */
      /*----------------------------------------------------------*/
      DK_String	 pos_constraint_name;	/* position constraint name */
      short	 pos_constraint_type;	/* position constraint type */

      /*----------------------------------------------------------*/
      /* translation path 					  */
      /*							  */
      /* Field compatible with CREATIVE Environment 2.6 and below */
      /* For later scenes, use the field 'constr_list' (list)     */
      /*----------------------------------------------------------*/
      DK_Spline	         *path;		/* old scene file format */
      DK_String 	 path_name;	/* new v2.5 scene file format */

      /*---------------------------------------------------------*/
      /* shapes 						 */
      /* The field shape_filename is used only to pass the shape */
      /* file name to the shape read/write functions. That's all.*/
      /*---------------------------------------------------------*/
      DK_String		shape_filename;		/* shape filename */
      DK_ShapeInterpType shape_interp_type;	/* shape interpolation type */
      int		nbshape;		/* number of shape definition */
      DK_Pointer        *shapes;                /* shapes geometry */

      /*---------------------------------------------------------*/
      /* clusters 						 */
      /*							 */
      /* a) The definition of the clusters themselvse 		 */
      /* b) Some information related to the animated clusters    */
      /*---------------------------------------------------------*/
      DK_Cluster	*cluster;		/* (a) */
      DK_Kluster	*klusters;		/* (b) */

      /*----------------------------------*/
      /* materials & textures information */
      /*----------------------------------*/
      short		 currentMaterial;
      short		 nbMaterials;
      DK_Material	*materials;	/* chained list of DK_materials */
      short		 nbTextures;
      DK_Texture	*textures;	/* chained list of global textures */
      short		 nbTextures3D;  /* NOTE 3 */
      DK_Texture3D	*textures3D;	/* chained list of global 3D textures */

      /*----------------------------------*/
      /* Lattices... 			  */
      /*----------------------------------*/
      short         nbnodelat;   /* number of node lattice                    */
      DK_Lattice   *nodelat;	 /* node lattice of deformation               */
      short         nbtreelat;   /* number of tree lattice                    */
      DK_Lattice   *treelat;	 /* tree lattice of deformation               */
      short         curlatdef;   /* current def shown in array of data 	      */

      /*----------------------------------*/
      /* Spline Fitting... 		  */
      /*----------------------------------*/
      DK_Spl_fitting     *nodefit;	/* node spline fitting */
      DK_Spl_fitting     *brchfit;  	/* branch spline fitting */

      /*----------------------------------*/
      /* Patch Fitting... 		  */
      /*----------------------------------*/
      DK_Ptch_fitting    *nodepfit;	/* node patch fitting */
      DK_Ptch_fitting    *brchpfit;  	/* branch patch fitting */

      /*--------------------------------------------------------*/
      /* Object substitution 					*/
      /*							*/
      /* These fields will be filled/written down only when     */
      /* reading/writing old scenes (version v2.5 and below)    */
      /* There are ignored with scenes v2.6 and up. The type of */
      /* object substitution below is not supported in the same */
      /* way anymore.						*/
      /*--------------------------------------------------------*/
      short 	      sub_active;
      DK_String	      sub_model_name;   /* model substitution filename */
      short	      sub_start;	/* starting frame of substitution */
      short	      sub_end;		/* ending frame of substitution */
      short	      sub_inmemory;	/* 1 if objs loaded into memory */
      DK_String	      sub_pre_script_file;   /* pre model script file */
      DK_String	      sub_post_script_file;  /* post model script file */

      /*--------------------------------------------------------*/
      /* Dynamics 						*/
      /*							*/
      /* These fields are included in models only for storage,  */
      /* when reading/writing scenes. They should not be set by */
      /* the DKit user.						*/
      /*--------------------------------------------------------*/
      struct DK_dynaconstr  *dcinfo;      /* constraint info */
      struct DK_dynachnvar  *dchnvar;     /* properties */
      struct DK_dynamatter_info *dmatter; /* physical properties */
      struct DK_dynajntinfo *djntinfo;    /* physical properties of joint */
      DK_AlgoType           algotype;	  /* type of algorithm for joints */

      /*--------------------------------------------------------*/
      /* Inverse Kinematics					*/
      /*							*/
      /* These fields are included in models only for storage,  */
      /* when reading/writing scenes. They should not be set by */
      /* the DKit user.						*/
      /*--------------------------------------------------------*/
      struct DK_inv_kin    *ikvar;         /* Inverse-kinematics stuff */

      /*--------------------------------------------------------*/
      /* Flexible Envelopes					*/
      /*							*/
      /* These fields are included in models only for storage,  */
      /* when reading/writing scenes. They should not be set by */
      /* the DKit user.						*/
      /*--------------------------------------------------------*/
      DK_EnvelopeType       envtype;       /* type of envelope   */
      struct DK_envdata    *envdata;       /* envelope data      */
      DK_SRT_Vect	   *init_srt;	   /* init. transfos. for rigid env. */

      /*-----------------------*/
      /* Hierarchy information */
      /*-----------------------*/
      struct DK_model	*parent;
      struct DK_model	*sibling;	/* or brother */
      struct DK_model	*child;		/* or son     */

      /*-----------------------*/
      /* Display information   */
      /*-----------------------*/
      short		wire_colour;	/* wire frame colour      */
      DK_LineType	line_type;	/* line display type      */
      short 		selected;	/* selection type         */
      short		bbox;		/* bounding box ON or OFF */
      DK_Vector		bbox_min;	/* bounding box min       */
      DK_Vector 	bbox_max;	/* bounding box max       */
      float		schem_posx;	/* schematic position x   */
      float		schem_posy;	/* schematic position y   */
      short		schem_coll;	/* schematic collapsed?   */
      short		schem_userp;	/* schematic user position flag */

      /*--------------------------------------------------------*/
      /* New list of constraints and relations 			*/
      /*							*/
      /* Handles all constraints, paths and other references to */
      /* other elements. See Release notes for the details.     */
      /*--------------------------------------------------------*/
      DK_Constraint	*constr_list;

      /*--------------------------------------------------------*/
      /* Animation curves 					*/
      /*--------------------------------------------------------*/
      DK_Fcurve		*fcv[DK_FCV_MDL_NUM];	     /* array of fcurves */
      DK_actFcurve	active_fcv[DK_FCV_MDL_NUM]; /* activation fcurves */

      /*--------------------------------------------------------*/
      /* Animation curves for shape weights			*/
      /*--------------------------------------------------------*/
      DK_Fcurve		**wfcv;		/* shape weight fcurve array */
      DK_actFcurve	*active_wfcv;	/* activation fcurves -weight */

      /*--------------------------------------------------------*/
      /* User Data List						*/
      /*							*/
      /* DKit users may use this list to store their own pieces */
      /* of information. It will be saved as is, (binary chunk) */
      /* in model files and preserved by Creative so it can     */
      /* later be used again in the DKit.			*/
      /*--------------------------------------------------------*/
      DK_UserNode	*userDataList;

      struct DK_model	*next;

      /* for Render stuff */
      short          visibilityFlags;
      short          motionBlurFlags;

   };


/*******************************************************************************

NOTES 

  (1) The SOFTIMAGE hierarchy transformation is not totally standard;
      Scaling is applied at the end. The Classic Sclaing switch under
      the Preferences of Creative allows to turn to a classical matrix
      construction, less useful for character animation.

  (2) in XYZ order, expressed in RADIANS.

  (3) The field 'nbTextures3D' should have a value of 0 or 1. Creative
      doesn't support multiple global or local 3D textures.

*******************************************************************************/

/* Global Model prototypes */
extern	DK_Model   *DK_modelAllocate( void );
extern	void	    __cdecl DK_modelDispose( DK_Model ** );
extern  DK_Model   *DK_fdmodelReadBinaryFile( FILE *fd );
extern	DK_Model   * __cdecl DK_modelReadFile( DK_String );
extern  DK_Boolean  DK_fdmodelWriteBinaryFile( FILE *fd, DK_Model *mdl );
extern	DK_Boolean  DK_modelWriteFile( DK_String, DK_Model *, DK_FileFormat );
extern	DK_Boolean  DK_modelCreateParentship( DK_Model *, DK_Model * );
extern	DK_Boolean  DK_modelCut( DK_Model * );
extern  DK_Model   *DK_modelNextNode( DK_Model * );
extern  DK_Model   *DK_modelNodeFindByName ( DK_Model *, DK_String);
extern  int         DK_objectWriteAsciiAnimation ( FILE *, DK_Model *);
extern  int         DK_objectReadAsciiAnimation ( FILE *, DK_Model **, char *);
extern  int	    DK_modelReadShapeBinaryFile( DK_Model * );
extern	DK_Boolean  DK_modelAttachUserData( DK_Model *, DK_String, int, char * );
extern	DK_UserNode *DK_modelSearchUserData( DK_Model *, DK_String );
extern	void	    DK_modelDisposeUserData( DK_Model *, DK_String );

/* These are temporarily required for the Minerva effects */
#ifdef _WIN32
extern DK_Model *DK_modelReadAsciiNode( FILE *, DK_Model * );
extern DK_Model *DK_modelReadBinaryNode( FILE *, DK_Model *, DK_FileProt * );
extern DK_Boolean DK_modelWriteBinaryNode( FILE *, DK_Model *, DK_FileProt *,
					   DK_Boolean );
#endif

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif  /* ifndef __TINY_MODEL_H__ */
