/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyConstr.h,v $
   $Revision: 1.11 $ $Date: 1994/12/20 20:04:54 $
   Checkin by: $Author: a_tan $

   Description of types associated with the lists of constraints.

   (c) Copyright 1990-1994, SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_CONSTRAINT_H__
#define __TINY_CONSTRAINT_H__

#include "tyFcurve.h"
#include "tySpline.h"

typedef struct
{
   short x;
   short y;
   short z;
} DK_SVector;

/*---------------------------------------------------------------------*/
/* Relation data associated with type DK_REL_CNSPOSLIM: Position Limits */
/*---------------------------------------------------------------------*/
typedef struct
{
   DK_Vector	min;		/* minimum limits */
   DK_Vector	max;		/* maximum limits */
   DK_SVector	min_act;	/* indicates active min limits */
   DK_SVector	max_act;	/* indicates active max limits */
   float	damping_area;	/* % where damping is applied */
   float	damping_strength;/* 0 = weak damping, 1 = strong */
   short	absolute;	/* or relative to object */
   short	box;		/* or radius */
   float	radius;		/* max distance from reference point */
} DK_RelPoslimData;

/*----------------------------------------------------------------------*/
/* Relation data associated with type DK_REL_CNSROTLIM: Rotation Limits */
/*----------------------------------------------------------------------*/
typedef struct
{
   DK_Vector	min;		/* minimum limits in degrees */
   DK_Vector	max;		/* maximum limits in degrees */
   DK_SVector	min_act;	/* indicates active min limits */
   DK_SVector	max_act;	/* indicates active max limits */
   DK_Vector	rad_min; 	/* minimum limits in radians */
   DK_Vector	rad_max;	/* maximum limits in radians */
   float	damping_area;   /* % where damping is applied */
   float	damping_strength;/* 0 = weak damping, 1 = strong */
} DK_RelRotlimData;


/*----------------------------------------------------------------------*/
/* Relation data associated with type DK_REL_SYMMETRY: Symmetry 	*/
/*----------------------------------------------------------------------*/
/*--------------------*/
/* Planes of symmetry */
/*--------------------*/
#define	DK_XY_PLANE   0
#define	DK_XZ_PLANE   1
#define	DK_YZ_PLANE   2

typedef struct
{
   short  plane;	/* symm plane: DK_XY_PLANE,DK_XZ_PLANE or DK_YZ_PLANE */
   short  hcyclen;	/* half cycle length in frames */
   short  start;	/* frame start of the propagation */
   short  duration;	/* duration in half cycles of the prop. */
   short  reference;	/* world or ancestor... */
   DK_Vector init_lrot;	/* initial local rotation of constraint */
   DK_Vector init_grot;	/* initial global rotation of constraint */
   DK_Vector init_arot;	/* initial ancestor-relative rotation of constraint */
} DK_RelSymmData;

/*----------------------------------------------------------------------*/
/* Relation data associated with type DK_REL_PATHEXPL: Trajectory 	*/
/*----------------------------------------------------------------------*/
typedef struct
{
   int	  frameStart;	/* reference frame (frame corr. to first key) */
} DK_RelPathExplData;

/*----------------------------------------------------------------------*/
/* Relation data associated with type DK_REL_EXTRUSION: Extrusion 	*/
/*----------------------------------------------------------------------*/
/* Extrusion axis */
typedef enum
{
   DK_EXTRUDE_X,
   DK_EXTRUDE_Y,
   DK_EXTRUDE_Z,
   DK_EXTRUDE_USER	             /* Axis defined by the user	*/
} DK_ExtrudeAxis;

typedef struct 
{
   short		close;       /* Extrusion Closed                    */
   float		depth;       /* Extrusion X-Y-Z: Depth              */  
   int			step;        /* Mesh: step                          */ 
   int			subdivision; /* Mesh: subdivision                   */  
   DK_ExtrudeAxis	axis;        /* Extrusion Axis                      */ 
   short		isPatch;     /* Result in Patch, otherwise in Mesh  */ 
   DK_SplineType	splType;     /* Patch Type                          */
   short		bevel;       /* Mesh: Bevelling or no bevelling     */  
   short		nbBevel;     /* Mesh: subdivision for the bevel     */
   float		radius;      /* Mesh: radius for the bevel          */  
   short        	top;         /* Mesh: Top bevelling                 */  
   short        	side;        /* Mesh: Side bevelling                */ 
   short        	bottom;      /* Mesh: Bottom bevelling              */
} DK_ExtrusionData;

/*----------------------------------------------------------------------*/
/* Relation data associated with type DK_REL_REVOLUTION: Revolution 	*/
/*----------------------------------------------------------------------*/
/* Revolution Axis */
typedef enum
{
   DK_REVOL_X,
   DK_REVOL_Y,
   DK_REVOL_Z,
   DK_REVOL_FREE,		     /* Arbitrary Axis			*/
   DK_REVOL_USER                     /* Axis defined by the user	*/
} DK_RevolAxis;

typedef struct 
{
   short        	close;       /* Revolution Closed                  */
   float             	degree;      /* Degree of revolution               */
   int               	step;        /* Step for the revolution            */
   DK_RevolAxis      	axis;        /* Axis of revolution                 */
   int               	subdivision; /* Mesh: subdivision                  */
   short        	isPatch;     /* Result in Patch, otherwise in Mesh */
   DK_SplineType     	splType;     /* Patch Type                         */
}DK_RevolutionData;

/*----------------------------------------------------------------------*/
/* Relation data associated with type DK_REL_SKINNING: Skin 	        */
/*----------------------------------------------------------------------*/
typedef struct 
{
   short        	close;       /* Skinning Closed                    */
   int               	nbSpline;    /* Number of spline to skin           */
   int               	subdivision; /* Mesh: subdivision                  */
   short        	cap;         /* Mesh: capping                      */
   short        	isPatch;     /* Result in Patch, otherwise in Mesh */
   DK_SplineType     	splType;     /* Patch Type                         */
   short		NRBUniformity;/* uniform, non-uniform,             */
				     /* chor length, centripetal           */
   short		NRBType;     /* Linear , cubic                     */

} DK_SkinningData;

/*----------------------------------------------------------------------*/
/* Relation data associated with type DK_REL_ZIP: Zip Patches 	        */
/*----------------------------------------------------------------------*/
typedef struct
{
      int		indexSup;  /* 1st patch: support for zip            */
      int		indexZip;  /* 2nd patch: zipped on the support      */
      short		zipUSup;   /* zip support direction: 0 = U, 1 = V   */
      short		zipUZip;   /* zip zipped direction: 0 = U, 1 = V    */
      int		ptDw;	   /* Number of points down                 */
      int		ptUp;	   /* Number of points up                   */
      int		rwDw;	   /* Number of row down                    */
      int		rwUp;	   /* Number of row up                      */
} DK_ZipData;

/*--------------------------------------------------------------------*/
/* Relation data associated with type DK_REL_EXPRESSION: Expressions  */
/*--------------------------------------------------------------------*/
typedef struct
{
   char			*var_name;	/* Variable name		      */
   char			*var_expression;/* Expression defining the variable   */
} DK_ExpressionVarDef;

typedef struct
{
   char			*expression;	/* The expression expressed as 'A = B'*/
					/* (where 'A' is the element being    */
					/* affected, and 'B' is the expression*/
   int			nb_of_vars;	/* Number of local variables defined  */
   DK_ExpressionVarDef	*vars_array;	/* Array of variables		      */
} DK_ExpressionData;

/*--------------------------------------------------------------------*/
/* Relation data associated with type DK_REL_QSTRETCH: Qstretch       */
/*--------------------------------------------------------------------*/
typedef struct
{
   DK_Boolean chk;    /* TRUE = activate this mode, FALSE = don't. */
   DK_Boolean dirty;  /* TRUE when chk = TRUE and no fcvs. or param. changed */
   DK_Boolean opt;    /* option applied when TRUE.                 */
   DK_Boolean cx;     /* option applied when TRUE.                 */
   DK_Boolean cy;     /* option applied when TRUE.                 */
   DK_Boolean cz;     /* option applied when TRUE.                 */
   float      sen;    /* "sensitivity" to kinematic param.         */
   float      max;    /* maximum ratio of deformation.             */
   DK_Pointer *data;  /* work data */
} DK_QSMode;

typedef struct
{
   DK_QSMode flex;     /* flexing mode           */
   DK_QSMode eyld;     /* external yielding mode */
   DK_QSMode iyld;     /* internal yielding mode */
} DK_QSState;

typedef struct
{
   DK_QSState vit;     /* QS due to linear velocity      */
   DK_QSState a;       /* QS due to linear acceleration  */ 
   DK_QSState w;       /* QS due to angular velocity     */
   DK_QSState wdot;    /* QS due to angular acceleration */
   DK_Boolean parent;  /* FALSE = take parent's param, TRUE = override them. */
   DK_Boolean damp;    /* (unused so far) 		                     */
   DK_Boolean ontag;   /* TRUE = apply only on tagged points                 */
} DK_RelQSData;


/*-------------------------------*/
/* Possible values for 'reltype' */
/*-------------------------------*/

#define DK_REL_CNSPOSLIM	102  /* Position limit (data) */
#define DK_REL_CNSROTLIM	103  /* Rotation limit (data) */
#define DK_REL_CNSPOS		111  /* Position cns */
#define DK_REL_PATH		112  /* Path */
#define DK_REL_CNSORI		114  /* Orientation cns */
#define DK_REL_CNSSCL		115  /* Scaling cns */
#define DK_REL_CNSDIR		116  /* Direction cns */
#define DK_REL_CNSPLN		117  /* Bounding Plane */
#define DK_REL_CNSPIVPLN	118  /* Pivot plane */
#define DK_REL_CNS3PNTS		119  /* 3 points constraint */
#define DK_REL_CNSTANGENT	124  /* Tangency constraint */
#define DK_REL_CNSNORMAL	125  /* Normal constraint */
#define DK_REL_CNSOBJPNT	126  /* Object to point constraint */
#define DK_REL_CNS2PNTS		127  /* 2 points constraint */
#define DK_REL_CNSUPVECTOR	128  /* Up vector constraint */

#define DK_REL_SYMMETRY		150  /* Symmetry relation (data) */
#define DK_REL_PATHEXPL		160  /* (plotted) trajectory (data) */

#define DK_REL_QSCONTROL	238  /* QStretch controls */
#define DK_REL_QSTRETCH		239  /* QStretch (data)   */

#define DK_REL_CNSFPNTOBJ	246  /* Point to Object constraint */
#define DK_REL_EXTRUSION        250  /* Extusion (data)    */
#define DK_REL_REVOLUTION       251  /* Revolution (data)  */
#define DK_REL_SKINNING         252  /* Skin (data)        */
#define DK_REL_FOUR_SIDED       253  /* Four Sided         */
#define DK_REL_ZIP              255  /* Zip patches (data) */

#define DK_REL_WAVPATH		612  /* Path */

#define DK_REL_CAM_CNSPOS	1111 /* Position cns */
#define DK_REL_CAMPATH		1112 /* Path */
#define DK_REL_CAM_INTPATH	1114 /* Interest path */
#define DK_REL_CAM_INTCNSPOS	1116 /* Interest position cns */
#define DK_REL_CAMUPVECTOR      1117 /* Camera Up Vector */

#define DK_REL_LIG_CNSPOS	2111 /* Position cns */
#define DK_REL_LIGPATH		2112 /* Path */
#define DK_REL_LIGINTPATH	2114 /* Interest path */
#define DK_REL_LIGINTCNSPOS	2116 /* Interest position cns */

#define DK_REL_LIGSELECTION	2200 /* Light selection */
#define DK_REL_INSTANCE		20002 /* Instance */

#define DK_REL_EXPRESSION	20010 /* Expressions */

/*-------------------------------*/
/* Possible values for 'cnstype' */
/*-------------------------------*/

#define DK_ELEM_TYPE_MDL	1    /* A Model */
#define DK_ELEM_TYPE_CAM	2    /* A Camera */
#define DK_ELEM_TYPE_LIG	3    /* A Light */
#define DK_ELEM_TYPE_EXPRESS	4    /* An Expression */
#define DK_ELEM_TYPE_CLS	5    /* A Cluster */

/*----------------------------------------------------------------------
   Note: if reltype == DK_REL_LIGSELECTION ||
         	       DK_REL_LIGINTPATH ||
		       DK_REL_LIGPATH ||
		       DK_REL_CAM_INTPATH ||
		       DK_REL_CAMPATH ||
		       DK_REL_WAVPATH ||
		       DK_REL_PATH    ||
	    'active' and 'actfcv' are not defined (NULL)
         if reltype == DK_REL_CNSPOSLIM ||
		       DK_REL_CNSROTLIM ||
		       DK_REL_SYMMETRY
	    'prefix' and 'name' can point on the element itself!
-------------------------------------------------------------------------*/

/*==============================================================*/
/* Constraint list node						*/
/*								*/
/* Structure to keep track of scene's constraints, paths, and   */
/* other relations (such as light exclusion lists).		*/
/*								*/
/* Each constraint constists in a pair that relates an element  */
/* to another (identified with its type 'cnstype' and name      */
/* 'prefix' + 'name'). There may be more than one relation of   */
/* the same type (ex: many position constraints), and 		*/
/* constraints may be listed in any order in most of the cases  */
/* in the list.	(ex where order is important: a two pnts constr */
/* DK_REL_CNS2PNTS has two arguments - the 1st is the direction */
/* where the constrained object will be pointing).		*/
/*==============================================================*/

typedef
   struct DK_cns
   {
      int		reltype; /* type of constraint or relation */
      int		cnstype; /* type of element being pointed to */
      DK_String		prefix;  /* constraint element prefix */
      DK_String		name;    /* constraint element name */
      DK_actFcurve	actfcv;  /* boolean activation curve */
      DK_Pointer	reldata; /* data associated with the relation,*/
				 /* if any. */
      struct DK_cns	*next;	 /* next constraint node in list */
   } DK_Constraint;


extern  DK_Constraint *DK_constraintAllocate( DK_Constraint * );
extern  void	       DK_constraintDispose( DK_Constraint ** );

#endif
