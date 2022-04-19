/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyDynamics.h,v $
   $Revision: 1.12 $ $Date: 1994/07/21 18:38:32 $
   Checkin by: $Author: rejean $

   This file contains a subset of the SOFTIMAGE's dynamics data structure.

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_DYNAMICS_H__
#define __TINY_DYNAMICS_H__


#include "tyFile.h"

#ifndef __FORWARD_TYPEDEFS_TINYMODEL
#define __FORWARD_TYPEDEFS_TINYMODEL
typedef struct DK_model      DK_Model;
#endif

/* types of dynamic constraints */
typedef
   enum DK_dynaconstr_type
   {
      DK_DYN_GRAVITY,
      DK_DYN_ROOT_POS_ATTACH,	/* unused so far */
      DK_DYN_ROOT_ORI_ATTACH,	/* unused so far */
      DK_DYN_IMPULSE,		/* unused so far */
      DK_DYN_EXT_FORCE,		/* */
      DK_DYN_EXT_TORQUE,	/* unused so far */
      DK_DYN_WIND,		/* global wind		  */
      DK_DYN_NAIL,		/* positional constraint  */
      DK_DYN_ORIENTATION,	/* unused so far */
      DK_DYN_COLLISION,		/* general collision	  */
      DK_DYN_DEF_GRAVITY,	/* default gravity	  */
      DK_DYN_DEF_WIND,		/* unused so far */
      DK_DYN_DEF_FLOOR,		/* unused so far */
      DK_DYN_FAN		/* local wind (ventilator) */
   } DK_DynaConstrType;

/* levels of shape evaluation (for collisions, wind...) */
typedef
   enum DK_shape_level
   {
      DK_GENERAL = -2,
      DK_BPLANE = -1,
      DK_BSPHERE,
      DK_BBOX,
      DK_SHAPE
   } DK_ShapeLevel;

typedef struct DK_dynamatter_info
{
   float	density;
   float	mu_s;		/* coef de frottement statique      */
   float	mu_k;		/* coef de frottement cinematique   */
   float	elasticity;	/* elasticite pour collisions 	    */
   float	rigidity;	/* */
   float	plasticity;	/* */
   int		node_cdlevel;	/* node coll. detection level	    */
} DK_DynaMatterInfo;

typedef struct DK_dynajntinfo
{
   DK_Vector	friction;	/* friction au joint (selon les 3 axes) */
} DK_DynaJntInfo;

typedef struct DK_dynaconstr
{
   DK_DynaConstrType     type;
   short	         n;
   char		        *model_name;
   short	         obstype;	/* obstacle type */
   struct DK_dynaconstr *next;
} DK_DynaConstrInfo;

typedef struct DK_dynachnvar {
   short	dorcpos;	/* do root positional constraint	*/
   short	dorcori;	/* do root orientation constraint	*/
   short	dograv;		/* do gravity				*/
   short	dofloor;	/* do check collisions against planes	*/
   short	dowind;		/* do wind : unused so far 		*/

   short	rcposaxe[3];	/* axes affected by root pos. constraint */
   short	rcoriaxe[3];	/* axes affected by root ori. constraint */
   short	rcposacc;	/* induce linear acceleration from root  */
   short	rcoriacc;	/* induce angular acceleration from root */

   DK_ShapeLevel cdetect_level;	/* coll detection: object precision level*/
   DK_ShapeLevel ddetect_level;	/* coll detection: decor precision level */
   DK_ShapeLevel wind_level;	/* wind precision level			 */

   short	geomupdate;	/* update geometry before each frame */
   short	fan_maxsubdiv;	/**/
} DK_DynaChnVars;

#define DK_DYN_DEF_GRAVITY_AMPL   -910.0
#define DK_DYN_DEF_FRICT           15.0
#define DK_DYN_DEF_CHN_FRICT       1.0     /* scaling factor */
#define DK_DYN_DEF_DENSITY         0.2
#define DK_DYN_DEF_MU_STATIC       0.9
#define DK_DYN_DEF_MU_KINETIC      0.3
#define DK_DYN_DEF_ELASTICITY      0.6
#define DK_DYN_DEF_RIGIDITY        1.0
#define DK_DYN_DEF_PLASTICITY      0.0

/* constraints */
#define DK_DYN_DEF_STIFFNESS       0.004

#define DK_A_DYNCTL_TOKEN		"dynamic_control"
#define DK_A_DYNROOT_TOKEN		"dynamic_root"
#define DK_A_DYNMATTER_TOKEN		"physical_properties"
#define DK_A_DYNJNTPROP_TOKEN		"joint_properties"
#define DK_A_DYN_GRAVITY_TOKEN		"gravity"
#define DK_A_DYN_DEFGRAV_TOKEN		"default_gravity"
#define DK_A_DYN_FORCE_TOKEN		"force"
#define DK_A_DYN_WIND_TOKEN		"wind"
#define DK_A_DYN_FAN_TOKEN		"fan"
#define DK_A_DYN_NAIL_TOKEN		"nail"

#define DK_A_DYNMATTER_DENSITY_TOKEN	"density"
#define DK_A_DYNMATTER_ELASTICITY_TOKEN	"elasticity"
#define DK_A_DYNMATTER_SROUGHNESS_TOKEN	"static_roughness"
#define DK_A_DYNMATTER_KROUGHNESS_TOKEN	"kinetic_roughness"
#define DK_A_DYNMATTER_RIGIDITY_TOKEN	"rigidity"
#define DK_A_DYNMATTER_PLASTICITY_TOKEN	"plasticity"
#define DK_A_DYNMATTER_NCDLEVEL_TOKEN	"node_coll_detect_level"

#define DK_A_DYNJNTPROP_FRICTION_TOKEN	"friction"
#define DK_A_DYNSETUP_GENERAL_TOKEN	"GENERAL"


extern	DK_DynaMatterInfo *DK_dynaMatterInfoAllocate( void );
extern	DK_DynaJntInfo    *DK_dynaJntInfoAllocate( void );
extern	DK_DynaConstrInfo *DK_dynaConstrInfoAllocate( void );
extern	DK_DynaChnVars    *DK_dynaChnVarAllocate( void );

extern	void            DK_dynaMatterInfoDispose( DK_DynaMatterInfo ** );
extern	void            DK_dynaJntInfoDispose( DK_DynaJntInfo ** );
extern	void            DK_dynaConstrInfoDispose( DK_DynaConstrInfo ** );
extern	void            DK_dynaChnVarDispose( DK_DynaChnVars ** );

extern  void 		DK_dynamatterWriteBinaryChunk ( FILE *, DK_DynaMatterInfo *);
extern  DK_DynaMatterInfo 	*DK_dynamatterReadBinaryChunk ( FILE *, DK_FileProt *);

extern  void		DK_dynamatterWriteAscii( FILE *, DK_DynaMatterInfo *, char *);
extern  DK_DynaMatterInfo *DK_dynamatterReadAscii( FILE *, char *);
extern  void	 	DK_dynajntpropWriteBinaryChunk ( FILE *, DK_DynaJntInfo *);
extern  void	 	DK_dynajntpropReadBinaryChunk( FILE *,DK_Model *,DK_FileProt *);
extern  void		DK_dynajntpropWriteAscii( FILE *, DK_DynaJntInfo *, char *);
extern  DK_DynaJntInfo    *DK_dynajntpropReadAscii( FILE *, char *);
extern  int 		DK_dynaConstraintWriteAsciiChunk ( FILE *, 
				DK_DynaConstrInfo *);
extern  int 		DK_dynaConstraintReadAsciiChunk ( FILE *, DK_Model *);
extern  int 		DK_dynachnvarWriteAsciiChunk ( FILE *, DK_DynaChnVars *);
extern  int 		DK_dynachnvarReadAsciiChunk ( FILE *, DK_Model *);
extern  void 		DK_odynamicWriteBinaryChunk ( FILE *, DK_Model *);
extern  void 		DK_odynamicReadBinaryChunk ( FILE *, DK_Model *);

#endif  /* ifndef __TINY_DYNAMICS_H__ */
