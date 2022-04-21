/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyActor.h,v $
   $Revision: 1.7 $ $Date: 1994/08/24 22:12:38 $
   Checkin by: $Author: ******$

   This file contains a subset of the SOFTIMAGE's Actor data structure.

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_ACTOR_H__
#define __TINY_ACTOR_H__

#include "tyFile.h"

/* envelope defines */
#define DK_SAMP	100

#define _DK_MatrixIdInit( mat ) \
   { \
      mat[0][0] = mat[1][1] = mat[2][2] = mat[3][3] = 1.0; \
      mat[0][1] = mat[0][2] = mat[0][3] = 0.0; \
      mat[1][0] = mat[1][2] = mat[1][3] = 0.0; \
      mat[2][0] = mat[2][1] = mat[2][3] = 0.0; \
      mat[3][0] = mat[3][1] = mat[3][2] = 0.0; \
   }

/* chain and joint */
#define _DK_MODEL_IS_JNT(mdl)              ((mdl)->type == DK_MJNT)
#define _DK_MODEL_IS_CHNROOT(mdl) \
   ((mdl)->type == DK_MDL_NILL && \
   ((mdl)->algotype == DK_AIKIN || (mdl)->algotype == DK_ADYNA))
#define _DK_MODEL_IS_CHNLEAF(mdl) \
   ((mdl)->type == DK_MDL_NILL && \
   ((mdl)->algotype == DK_AIKLEAF || (mdl)->algotype == DK_ADYNLEAF))

typedef
   struct DK_inv_kin
   {
      int       type;         	/* type of chain */
      int       nbjts;        	/* number of joints in chain */
      DK_Vector prot;      	/* prefered joint angle */
   } DK_InvKin;

typedef
   struct DK_envvert
   {
      int	         index;
      DK_Vector          coord;	/* Obsolete */
      DK_Matrix          trmat;	/* Obsolete */
      float 	         ratio, thickness;
      float	         weight;
      struct DK_envvert *next;
   } DK_EnvVert;

typedef
   struct DK_envlistvert
   {
      struct DK_model       *env;
      DK_EnvVert            *segvert;
      struct DK_envlistvert *prev;
      struct DK_envlistvert *next;
   } DK_EnvListVert;

typedef
   enum DK_def_type
   {
      DK_FINGER,
      DK_BICEPS
   } DK_DefType;

typedef
   struct DK_defcrvparam
   {
      DK_DefType  type;
      float      *data;
      float       beg, end;
      float       ampl;
      float       scale;
   } DK_DefCrvParam;

typedef
   struct DK_envdata
   {
      int             level;
      DK_EnvListVert *envlist;
      DK_Matrix       transmat;
      DK_Vector       initvrot;
      DK_Vector       oldvrot;
      DK_Vector       rootscl;
      DK_Vector       rootrot;
      DK_Vector       roottrs;
      float           jointarea;
      DK_DefCrvParam  indefcrv;
      DK_DefCrvParam  outdefcrv;
   } DK_EnvData;
 

#define DK_A_CHAIN_TOKEN		"articulated_chain"
#define DK_A_CHNROOT_TOKEN		"chain_root"
#define DK_A_CHNLEAF_TOKEN		"chain_leaf"
#define DK_A_CHNJNT_TOKEN		"joint"

#define DK_A_ENVTYPE_TOKEN		"envelope_type"

#define DK_A_INVKINDATA_TOKEN		"inverse_kinematics"
#define DK_A_IKTYPE_TOKEN		"ik_type"
#define DK_A_IKNBJNTS_TOKEN		"ik_nbjnts"
#define DK_A_IKPROT_TOKEN		"ik_pref_angles"

#define DK_W_ENVELOPDATA_TOKEN		"flexible_envelope   "
#define DK_W_ENVELOP_LEVEL_TOKEN	"level               "
#define DK_W_ENVELOP_INITVROT_TOKEN	"initial_vrot        "
#define DK_W_ENVELOP_ROOTSCL_TOKEN	"initial_root_scl    "
#define DK_W_ENVELOP_ROOTROT_TOKEN	"initial_root_rot    "
#define DK_W_ENVELOP_ROOTTRS_TOKEN	"initial_root_trs    "
#define DK_W_ENVELOP_JNTAREA_TOKEN	"joint_area          "
#define DK_W_ENVELOP_INCRVTYP_TOKEN	"inner_curve_type    "
#define DK_W_ENVELOP_INCRVBEG_TOKEN	"inner_curve_beg     "
#define DK_W_ENVELOP_INCRVEND_TOKEN	"inner_curve_end     "
#define DK_W_ENVELOP_INCRVAMP_TOKEN	"inner_curve_ampl    "
#define DK_W_ENVELOP_INCRVSCL_TOKEN	"inner_curve_scale   "
#define DK_W_ENVELOP_OUTCRVTYP_TOKEN	"outer_curve_type    "
#define DK_W_ENVELOP_OUTCRVBEG_TOKEN	"outer_curve_beg     "
#define DK_W_ENVELOP_OUTCRVEND_TOKEN	"outer_curve_end     "
#define DK_W_ENVELOP_OUTCRVAMP_TOKEN	"outer_curve_ampl    "
#define DK_W_ENVELOP_OUTCRVSCL_TOKEN	"outer_curve_scale   "
#define DK_W_ENVELOP_INCRVNBS_TOKEN	"inner_curve_nbpts   "
#define DK_W_ENVELOP_INCRVDAT_TOKEN	"inner_curve_data    "
#define DK_W_ENVELOP_OUTCRVNBS_TOKEN	"outer_curve_nbpts   "
#define DK_W_ENVELOP_OUTCRVDAT_TOKEN	"outer_curve_data    "
#define DK_W_ENVELOP_NBENV_TOKEN	"nb_envelopes        "
#define DK_W_ENVELOP_NBVERT_TOKEN	"nb_vertices         "

#define DK_W_ENVVERT_INDEX_TOKEN	"vert_index          "
#define DK_W_ENVVERT_TRMAT_TOKEN	"transform_matrix    "
#define DK_W_ENVVERT_RATIO_TOKEN	"vert_ratio          "
#define DK_W_ENVVERT_THICK_TOKEN	"vert_thickness      "
#define DK_W_ENVVERT_WEIGHT_TOKEN	"vert_weight         "

#define DK_R_ENVELOPDATA_TOKEN		"flexible_envelope"
#define DK_R_ENVELOP_LEVEL_TOKEN	"level"
#define DK_R_ENVELOP_INITVROT_TOKEN	"initial_vrot"
#define DK_R_ENVELOP_ROOTSCL_TOKEN	"initial_root_scl"
#define DK_R_ENVELOP_ROOTROT_TOKEN	"initial_root_rot"
#define DK_R_ENVELOP_ROOTTRS_TOKEN	"initial_root_trs"
#define DK_R_ENVELOP_JNTAREA_TOKEN	"joint_area"
#define DK_R_ENVELOP_INCRVTYP_TOKEN	"inner_curve_type"
#define DK_R_ENVELOP_INCRVBEG_TOKEN	"inner_curve_beg"
#define DK_R_ENVELOP_INCRVEND_TOKEN	"inner_curve_end"
#define DK_R_ENVELOP_INCRVAMP_TOKEN	"inner_curve_ampl"
#define DK_R_ENVELOP_INCRVSCL_TOKEN	"inner_curve_scale"
#define DK_R_ENVELOP_OUTCRVTYP_TOKEN	"outer_curve_type"
#define DK_R_ENVELOP_OUTCRVBEG_TOKEN	"outer_curve_beg"
#define DK_R_ENVELOP_OUTCRVEND_TOKEN	"outer_curve_end"
#define DK_R_ENVELOP_OUTCRVAMP_TOKEN	"outer_curve_ampl"
#define DK_R_ENVELOP_OUTCRVSCL_TOKEN	"outer_curve_scale"
#define DK_R_ENVELOP_INCRVNBS_TOKEN	"inner_curve_nbpts"
#define DK_R_ENVELOP_INCRVDAT_TOKEN	"inner_curve_data"
#define DK_R_ENVELOP_OUTCRVNBS_TOKEN	"outer_curve_nbpts"
#define DK_R_ENVELOP_OUTCRVDAT_TOKEN	"outer_curve_data"
#define DK_R_ENVELOP_NBENV_TOKEN	"nb_envelopes"
#define DK_R_ENVELOP_NBVERT_TOKEN	"nb_vertices"

#define DK_R_ENVVERT_INDEX_TOKEN	"vert_index"
#define DK_R_ENVVERT_TRMAT_TOKEN	"transform_matrix"
#define DK_R_ENVVERT_RATIO_TOKEN	"vert_ratio"
#define DK_R_ENVVERT_THICK_TOKEN	"vert_thickness"
#define DK_R_ENVVERT_WEIGHT_TOKEN	"vert_weight"


extern	DK_InvKin         *DK_invKinAllocate( void );
extern	DK_EnvData        *DK_envelopDataAllocate( int );
extern	DK_EnvListVert    *DK_envelopListVertAllocate( struct DK_model * );
extern	DK_EnvVert        *DK_envelopVertAllocate( void );

extern	void            DK_invKinDispose( DK_InvKin ** );
extern	void            DK_envelopDataDispose( DK_EnvData ** );
extern	void            DK_envelopVertDispose( DK_EnvVert ** );

extern	void 		DK_modelActorDataWriteAscii(FILE *,struct DK_model *,char *);
extern	void 		DK_modelActorDataReadAscii(FILE *,struct DK_model *,char *);

extern  void 		DK_invKinWriteBinaryChunk ( FILE *, DK_InvKin *);
extern  DK_InvKin 	*DK_invKinReadBinaryChunk ( FILE *);

extern  void 		DK_envelopWriteBinaryChunk ( FILE *, DK_EnvData *);
extern  DK_EnvData 	*DK_envelopReadBinaryChunk ( FILE *, struct DK_model *, DK_FileProt *);
extern  DK_EnvData 	*DK_oEnvelopeReadBinaryChunk ( FILE *);

#endif  /* ifndef __TINY_ACTOR_H__ */
