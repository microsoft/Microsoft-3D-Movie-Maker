/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyFcurve.h,v $
   $Revision: 1.17 $ $Date: 1994/11/22 17:30:54 $
   Checkin by: $Author: diane $

   This file contains a subset of the SOFTIMAGE's fcurve data structure.

   Written by: Dominique Boisvert, Colin Hui

   (c) Copyright 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_FCURVE_H__
#define __TINY_FCURVE_H__

#include  "tySoftTy.h"

/* integer fcurve values */
#define DK_FCURVE_TRUE  1
#define DK_FCURVE_FALSE 0

/* pre/post extrapolation types */
#define DK_CONSTANT_EXTRAP 0
#define DK_GRADIENT_EXTRAP 1
#define DK_CYCLE_EXTRAP    2

/* keyframe type */
#define  DK_FREEFORM		0
#define  DK_KEYFRAME		1
#define  DK_KEYPATH		2
#define  DK_RAWDATA		3

/* interpolation type of segment of fcurve (mutually exclusive) */
#define  DK_CONSTANT_INTERP		0
#define  DK_LINEAR_INTERP		1
#define  DK_HERMITE_INTERP		2

#define DK_FCV_MDL_NUM		51	/* DK_model */
#define DK_FCV_CAM_NUM		20	/* camera */
#define DK_FCV_LIG_NUM		24	/* light */
#define DK_FCV_TXT_NUM		23	/* 2D DK_texture */
#define DK_FCV_MAT_NUM		20	/* DK_material */
#define DK_FCV_FAD_NUM		6	/* fade */
#define DK_FCV_FOG_NUM		8	/* Layer-fog */
#define DK_FCV_TXT3D_NUM	45	/* 3D DK_texture */
#define DK_FCV_SFIT_NUM		7	/* DK_spline fit */
#define DK_FCV_PFIT_NUM		9	/* patch fit */
#define DK_FCV_WAVE_NUM		12	/* DK_wave */

/* block of keys in fcurve are allocated in multiple of (KEY_MASK + 1) */
#define  DK_KEY_MASK		7

#define _DK_BLOCK_SIZE( nb, mask )  \
   ( nb + mask - ( (nb - 1) & mask ) )

/* fcurve type */
#define DK_FCV_DEFAULT 0

#define DK_FCV_MDL_SCLX 	0
#define DK_FCV_MDL_SCLY 	1
#define DK_FCV_MDL_SCLZ 	2
#define DK_FCV_MDL_ROTX 	3
#define DK_FCV_MDL_ROTY 	4
#define DK_FCV_MDL_ROTZ 	5
#define DK_FCV_MDL_POS 		6
#define DK_FCV_MDL_SHAPE 	7
#define DK_FCV_MDL_EPOSX	8
#define DK_FCV_MDL_EPOSY	9
#define DK_FCV_MDL_EPOSZ	10
#define DK_FCV_MDL_HIDDEN	11
#define DK_FCV_MDL_ALGO		12
#define DK_FCV_MDL_NLAT		13
#define DK_FCV_MDL_TLAT		14
#define DK_FCV_MDL_LINVEL_X	15   
#define DK_FCV_MDL_LINVEL_Y	16
#define DK_FCV_MDL_LINVEL_Z	17
#define DK_FCV_MDL_LINACC_X	18
#define DK_FCV_MDL_LINACC_Y	19
#define DK_FCV_MDL_LINACC_Z	20
#define DK_FCV_MDL_ANGVEL_X	21
#define DK_FCV_MDL_ANGVEL_Y	22	
#define DK_FCV_MDL_ANGVEL_Z	23	
#define DK_FCV_MDL_ANGACC_X	24	
#define DK_FCV_MDL_ANGACC_Y	25	
#define DK_FCV_MDL_ANGACC_Z	26	
#define DK_FCV_MDL_LVFLEX_MAX	27
#define DK_FCV_MDL_LVSTCH_MAX	28
#define DK_FCV_MDL_LVYILD_MAX	29
#define DK_FCV_MDL_LAFLEX_MAX	30
#define DK_FCV_MDL_LASTCH_MAX	31
#define DK_FCV_MDL_LAYILD_MAX	32
#define DK_FCV_MDL_AVFLEX_MAX	33
#define DK_FCV_MDL_AVSTCH_MAX	34
#define DK_FCV_MDL_AVYILD_MAX	35
#define DK_FCV_MDL_AAFLEX_MAX	36
#define DK_FCV_MDL_AASTCH_MAX	37
#define DK_FCV_MDL_AAYILD_MAX	38
#define DK_FCV_MDL_LVFLEX_SEN   39
#define DK_FCV_MDL_LVSTCH_SEN	40
#define DK_FCV_MDL_LVYILD_SEN	41
#define DK_FCV_MDL_LAFLEX_SEN	42
#define DK_FCV_MDL_LASTCH_SEN	43
#define DK_FCV_MDL_LAYILD_SEN	44
#define DK_FCV_MDL_AVFLEX_SEN	45
#define DK_FCV_MDL_AVSTCH_SEN	46
#define DK_FCV_MDL_AVYILD_SEN	47
#define DK_FCV_MDL_AAFLEX_SEN	48
#define DK_FCV_MDL_AASTCH_SEN	49
#define DK_FCV_MDL_AAYILD_SEN	50


#define DK_FCV_CAM_POS		0
#define DK_FCV_CAM_INT		1
#define DK_FCV_CAM_ROLL		2
#define DK_FCV_CAM_FOV		3
#define DK_FCV_CAM_NEAR		4
#define DK_FCV_CAM_FAR		5
#define DK_FCV_CAM_FFCL		6
#define DK_FCV_CAM_FSTP		7
#define DK_FCV_CAM_FDST		8
#define DK_FCV_CAM_FNFC		9
#define DK_FCV_CAM_FFFC		10
#define DK_FCV_CAM_FOUT		11
#define DK_FCV_CAM_FCOC		12
#define DK_FCV_CAM_EPOSX	13
#define DK_FCV_CAM_EPOSY	14
#define DK_FCV_CAM_EPOSZ	15
#define DK_FCV_CAM_EINTX	16
#define DK_FCV_CAM_EINTY	17
#define DK_FCV_CAM_EINTZ	18
#define DK_FCV_CAM_HIDDEN	19

#define DK_FCV_LIG_COL_R	0
#define DK_FCV_LIG_COL_G	1
#define DK_FCV_LIG_COL_B	2
#define DK_FCV_LIG_CONE		3
#define DK_FCV_LIG_SPREAD	4
#define DK_FCV_LIG_DECAY	5
#define DK_FCV_LIG_START	6
#define DK_FCV_LIG_END		7
#define DK_FCV_LIG_SHADOW	8
#define DK_FCV_LIG_POS		9
#define DK_FCV_LIG_INT		10
#define DK_FCV_LIG_EPOSX	11
#define DK_FCV_LIG_EPOSY	12
#define DK_FCV_LIG_EPOSZ	13
#define DK_FCV_LIG_EINTX	14
#define DK_FCV_LIG_EINTY	15
#define DK_FCV_LIG_EINTZ	16
#define DK_FCV_LIG_HIDDEN	17
#define DK_FCV_LIG_AROT_X	18
#define DK_FCV_LIG_AROT_Y	19
#define DK_FCV_LIG_AROT_Z	20
#define DK_FCV_LIG_ASCL_X	21
#define DK_FCV_LIG_ASCL_Y	22
#define DK_FCV_LIG_ASCL_Z	23

#define DK_FCV_TXT_U_SCL	0
#define DK_FCV_TXT_V_SCL	1
#define DK_FCV_TXT_U_OFF	2
#define DK_FCV_TXT_V_OFF	3
#define DK_FCV_TXT_AMB		4
#define DK_FCV_TXT_DIFF		5
#define DK_FCV_TXT_SPEC		6
#define DK_FCV_TXT_REFLM	7
#define DK_FCV_TXT_TRSP		8
#define DK_FCV_TXT_RIND		9
#define DK_FCV_TXT_ROU		10
#define DK_FCV_TXT_ROT		11
#define DK_FCV_TXT_BLD		12
#define DK_FCV_TXT_REFL		13
#define DK_FCV_TXT_SROT_X	14
#define DK_FCV_TXT_SROT_Y	15
#define DK_FCV_TXT_SROT_Z	16
#define DK_FCV_TXT_SSCAL_X	17
#define DK_FCV_TXT_SSCAL_Y	18
#define DK_FCV_TXT_SSCAL_Z	19
#define DK_FCV_TXT_STRAN_X	20
#define DK_FCV_TXT_STRAN_Y	21
#define DK_FCV_TXT_STRAN_Z	22

#define DK_FCV_MAT_AMB_R	0
#define DK_FCV_MAT_AMB_G	1
#define DK_FCV_MAT_AMB_B	2
#define DK_FCV_MAT_DIFF_R	3
#define DK_FCV_MAT_DIFF_G	4
#define DK_FCV_MAT_DIFF_B	5
#define DK_FCV_MAT_SPEC_R	6
#define DK_FCV_MAT_SPEC_G	7
#define DK_FCV_MAT_SPEC_B	8
#define DK_FCV_MAT_ATTEN_R	9
#define DK_FCV_MAT_ATTEN_G	10
#define DK_FCV_MAT_ATTEN_B	11
#define DK_FCV_MAT_EXPN		12
#define DK_FCV_MAT_REFL		13
#define DK_FCV_MAT_REFL_J	14
#define DK_FCV_MAT_TRSP		15
#define DK_FCV_MAT_TRSP_J	16
#define DK_FCV_MAT_RIND		17
#define DK_FCV_MAT_BLURWIDTH	18
#define DK_FCV_MAT_BLURDECAY	19

#define DK_FCV_FOG_START	0
#define DK_FCV_FOG_STOP		1
#define DK_FCV_FOG_RED		2
#define DK_FCV_FOG_GREEN	3
#define DK_FCV_FOG_BLUE		4
#define DK_FCV_FOG_DENS		5
#define DK_FCV_FOG_BASE		6
#define DK_FCV_FOG_THICK	7

#define DK_FCV_FAD_START	0
#define DK_FCV_FAD_STOP		1
#define DK_FCV_FAD_RED		2
#define DK_FCV_FAD_GREEN	3
#define DK_FCV_FAD_BLUE		4
#define DK_FCV_FAD_TRANSP	5

#define DK_FCV_TXT3D_STR	0
#define DK_FCV_TXT3D_SPA	1
#define DK_FCV_TXT3D_POW	2
#define DK_FCV_TXT3D_SRT	3
#define DK_FCV_TXT3D_TAP	4
#define DK_FCV_TXT3D_FAC1	5
#define DK_FCV_TXT3D_FAC2	6
#define DK_FCV_TXT3D_FAC3	7
#define DK_FCV_TXT3D_COL0R	8
#define DK_FCV_TXT3D_COL0G	9
#define DK_FCV_TXT3D_COL0B	10
#define DK_FCV_TXT3D_COL0A	11
#define DK_FCV_TXT3D_COL1R	12
#define DK_FCV_TXT3D_COL1G	13
#define DK_FCV_TXT3D_COL1B	14
#define DK_FCV_TXT3D_COL1A	15
#define DK_FCV_TXT3D_COL2R	16
#define DK_FCV_TXT3D_COL2G	17
#define DK_FCV_TXT3D_COL2B	18
#define DK_FCV_TXT3D_COL2A	19
#define DK_FCV_TXT3D_COL3R	20
#define DK_FCV_TXT3D_COL3G	21
#define DK_FCV_TXT3D_COL3B	22
#define DK_FCV_TXT3D_COL3A	23
#define DK_FCV_TXT3D_COL4R	24
#define DK_FCV_TXT3D_COL4G	25
#define DK_FCV_TXT3D_COL4B	26
#define DK_FCV_TXT3D_COL4A	27
#define DK_FCV_TXT3D_SCLX	28
#define DK_FCV_TXT3D_SCLY	29
#define DK_FCV_TXT3D_SCLZ	30
#define DK_FCV_TXT3D_ROTX	31
#define DK_FCV_TXT3D_ROTY	32
#define DK_FCV_TXT3D_ROTZ	33
#define DK_FCV_TXT3D_TRNX	34
#define DK_FCV_TXT3D_TRNY	35
#define DK_FCV_TXT3D_TRNZ	36
#define DK_FCV_TXT3D_AMB	37
#define DK_FCV_TXT3D_DIF	38
#define DK_FCV_TXT3D_SPC	39
#define DK_FCV_TXT3D_REF	40
#define DK_FCV_TXT3D_TRS	41
#define DK_FCV_TXT3D_RID	42
#define DK_FCV_TXT3D_ROU	43
#define DK_FCV_TXT3D_BLD	44

#define DK_FCV_SFIT_SCLX	0
#define DK_FCV_SFIT_SCLY	1
#define DK_FCV_SFIT_SCLZ	2
#define DK_FCV_SFIT_ROLL	3
#define DK_FCV_SFIT_TRNX	4
#define DK_FCV_SFIT_TRNY	5
#define DK_FCV_SFIT_TRNZ	6

#define DK_FCV_PFIT_SCLX	0
#define DK_FCV_PFIT_SCLY	1
#define DK_FCV_PFIT_SCLZ	2
#define DK_FCV_PFIT_ROTX	3
#define DK_FCV_PFIT_ROTY	4
#define DK_FCV_PFIT_ROTZ	5
#define DK_FCV_PFIT_TRNX	6
#define DK_FCV_PFIT_TRNY	7
#define DK_FCV_PFIT_TRNZ	8

#define DK_FCV_WAVE_SCLX	0
#define DK_FCV_WAVE_SCLY	1
#define DK_FCV_WAVE_SCLZ	2
#define DK_FCV_WAVE_ROTX	3
#define DK_FCV_WAVE_ROTY	4
#define DK_FCV_WAVE_ROTZ	5
#define DK_FCV_WAVE_TRNX	6
#define DK_FCV_WAVE_TRNY	7
#define DK_FCV_WAVE_TRNZ	8
#define DK_FCV_WAVE_POS		9
#define DK_FCV_WAVE_DCY		10
#define DK_FCV_WAVE_SPD		11

/* fcurve type */
typedef
   enum DK_fcurve_type
   {
      DK_FCV_TYPE_DEFAULT,

      DK_FCV_TYPE_MDL_SCL_X,
      DK_FCV_TYPE_MDL_SCL_Y,
      DK_FCV_TYPE_MDL_SCL_Z,
      DK_FCV_TYPE_MDL_ROT_X,
      DK_FCV_TYPE_MDL_ROT_Y,
      DK_FCV_TYPE_MDL_ROT_Z,
      DK_FCV_TYPE_MDL_POS,
      DK_FCV_TYPE_MDL_SHAPE,

      DK_FCV_TYPE_CAM_POS,
      DK_FCV_TYPE_CAM_INT,
      DK_FCV_TYPE_CAM_ROLL,
      DK_FCV_TYPE_CAM_FOV,
      DK_FCV_TYPE_CAM_NEAR,
      DK_FCV_TYPE_CAM_FAR,
      DK_FCV_TYPE_CAM_FFCL,
      DK_FCV_TYPE_CAM_FSTP,
      DK_FCV_TYPE_CAM_FDST,
      DK_FCV_TYPE_CAM_FNFC,
      DK_FCV_TYPE_CAM_FFFC,
      DK_FCV_TYPE_CAM_FOUT,
      DK_FCV_TYPE_CAM_FCOC,

      DK_FCV_TYPE_LIG_COL_R,
      DK_FCV_TYPE_LIG_COL_G,
      DK_FCV_TYPE_LIG_COL_B,
      DK_FCV_TYPE_LIG_CONE,
      DK_FCV_TYPE_LIG_SPREAD,
      DK_FCV_TYPE_LIG_DECAY,
      DK_FCV_TYPE_LIG_START,
      DK_FCV_TYPE_LIG_END,
      DK_FCV_TYPE_LIG_SHADOW,
      DK_FCV_TYPE_LIG_POS,
      DK_FCV_TYPE_LIG_INT,

      DK_FCV_TYPE_TXT_U_SCL,
      DK_FCV_TYPE_TXT_V_SCL,
      DK_FCV_TYPE_TXT_U_OFF,
      DK_FCV_TYPE_TXT_V_OFF,
      DK_FCV_TYPE_TXT_AMB,
      DK_FCV_TYPE_TXT_DIFF,
      DK_FCV_TYPE_TXT_SPEC,
      DK_FCV_TYPE_TXT_REFLM,
      DK_FCV_TYPE_TXT_TRSP,
      DK_FCV_TYPE_TXT_RIND,
      DK_FCV_TYPE_TXT_NORM,
      DK_FCV_TYPE_TXT_ROT,

      DK_FCV_TYPE_MAT_AMB_R,
      DK_FCV_TYPE_MAT_AMB_G,
      DK_FCV_TYPE_MAT_AMB_B,
      DK_FCV_TYPE_MAT_DIFF_R,
      DK_FCV_TYPE_MAT_DIFF_G,
      DK_FCV_TYPE_MAT_DIFF_B,
      DK_FCV_TYPE_MAT_SPEC_R,
      DK_FCV_TYPE_MAT_SPEC_G,
      DK_FCV_TYPE_MAT_SPEC_B,
      DK_FCV_TYPE_MAT_EXPN,
      DK_FCV_TYPE_MAT_REFL,
      DK_FCV_TYPE_MAT_REFL_J,
      DK_FCV_TYPE_MAT_TRSP,
      DK_FCV_TYPE_MAT_TRSP_J,
      DK_FCV_TYPE_MAT_RIND,

      DK_FCV_TYPE_FAD_START,
      DK_FCV_TYPE_FAD_STOP,
      DK_FCV_TYPE_FAD_RED,
      DK_FCV_TYPE_FAD_GREEN,
      DK_FCV_TYPE_FAD_BLUE,
      DK_FCV_TYPE_FAD_TRANSP,
      
      DK_FCV_TYPE_LAT_NODE,
      DK_FCV_TYPE_LAT_TREE,

      DK_FCV_TYPE_TXT3D_STR,
      DK_FCV_TYPE_TXT3D_SPA,
      DK_FCV_TYPE_TXT3D_POW,
      DK_FCV_TYPE_TXT3D_SRT,
      DK_FCV_TYPE_TXT3D_TAP,
      DK_FCV_TYPE_TXT3D_FAC1,
      DK_FCV_TYPE_TXT3D_FAC2,
      DK_FCV_TYPE_TXT3D_FAC3,
      DK_FCV_TYPE_TXT3D_COL0R,
      DK_FCV_TYPE_TXT3D_COL0G,
      DK_FCV_TYPE_TXT3D_COL0B,
      DK_FCV_TYPE_TXT3D_COL1R,
      DK_FCV_TYPE_TXT3D_COL1G,
      DK_FCV_TYPE_TXT3D_COL1B,
      DK_FCV_TYPE_TXT3D_COL2R,
      DK_FCV_TYPE_TXT3D_COL2G,
      DK_FCV_TYPE_TXT3D_COL2B,
      DK_FCV_TYPE_TXT3D_COL3R,
      DK_FCV_TYPE_TXT3D_COL3G,
      DK_FCV_TYPE_TXT3D_COL3B,
      DK_FCV_TYPE_TXT3D_COL4R,
      DK_FCV_TYPE_TXT3D_COL4G,
      DK_FCV_TYPE_TXT3D_COL4B,
      DK_FCV_TYPE_TXT3D_SCLX,
      DK_FCV_TYPE_TXT3D_SCLY,
      DK_FCV_TYPE_TXT3D_SCLZ,
      DK_FCV_TYPE_TXT3D_ROTX,
      DK_FCV_TYPE_TXT3D_ROTY,
      DK_FCV_TYPE_TXT3D_ROTZ,
      DK_FCV_TYPE_TXT3D_TRNX,
      DK_FCV_TYPE_TXT3D_TRNY,
      DK_FCV_TYPE_TXT3D_TRNZ,
      DK_FCV_TYPE_TXT3D_AMB,
      DK_FCV_TYPE_TXT3D_DIF,
      DK_FCV_TYPE_TXT3D_SPC,
      DK_FCV_TYPE_TXT3D_REF,
      DK_FCV_TYPE_TXT3D_TRS,
      DK_FCV_TYPE_TXT3D_RID,
      DK_FCV_TYPE_TXT3D_ROU,

      DK_FCV_TYPE_NFIT_SCLX,
      DK_FCV_TYPE_NFIT_SCLY,
      DK_FCV_TYPE_NFIT_SCLZ,
      DK_FCV_TYPE_NFIT_ROLL,
      DK_FCV_TYPE_NFIT_TRNX,
      DK_FCV_TYPE_NFIT_TRNY,
      DK_FCV_TYPE_NFIT_TRNZ,

      DK_FCV_TYPE_TFIT_SCLX,
      DK_FCV_TYPE_TFIT_SCLY,
      DK_FCV_TYPE_TFIT_SCLZ,
      DK_FCV_TYPE_TFIT_ROLL,
      DK_FCV_TYPE_TFIT_TRNX,
      DK_FCV_TYPE_TFIT_TRNY,
      DK_FCV_TYPE_TFIT_TRNZ,

      DK_FCV_TYPE_PNFIT_SCLX,
      DK_FCV_TYPE_PNFIT_SCLY,
      DK_FCV_TYPE_PNFIT_SCLZ,
      DK_FCV_TYPE_PNFIT_ROTX,
      DK_FCV_TYPE_PNFIT_ROTY,
      DK_FCV_TYPE_PNFIT_ROTZ,
      DK_FCV_TYPE_PNFIT_TRNX,
      DK_FCV_TYPE_PNFIT_TRNY,
      DK_FCV_TYPE_PNFIT_TRNZ,

      DK_FCV_TYPE_PTFIT_SCLX,
      DK_FCV_TYPE_PTFIT_SCLY,
      DK_FCV_TYPE_PTFIT_SCLZ,
      DK_FCV_TYPE_PTFIT_ROTX,
      DK_FCV_TYPE_PTFIT_ROTY,
      DK_FCV_TYPE_PTFIT_ROTZ,
      DK_FCV_TYPE_PTFIT_TRNX,
      DK_FCV_TYPE_PTFIT_TRNY,
      DK_FCV_TYPE_PTFIT_TRNZ,

      DK_FCV_TYPE_MDL_EPOSX,
      DK_FCV_TYPE_MDL_EPOSY,
      DK_FCV_TYPE_MDL_EPOSZ,

      DK_FCV_TYPE_CAM_EPOSX,
      DK_FCV_TYPE_CAM_EPOSY,
      DK_FCV_TYPE_CAM_EPOSZ,

      DK_FCV_TYPE_CAM_EINTX,
      DK_FCV_TYPE_CAM_EINTY,
      DK_FCV_TYPE_CAM_EINTZ,

      DK_FCV_TYPE_LIG_EPOSX,
      DK_FCV_TYPE_LIG_EPOSY,
      DK_FCV_TYPE_LIG_EPOSZ,

      DK_FCV_TYPE_LIG_EINTX,
      DK_FCV_TYPE_LIG_EINTY,
      DK_FCV_TYPE_LIG_EINTZ,

      DK_FCV_TYPE_MDL_WSHP,
      DK_FCV_TYPE_TXT3D_BLD,

      DK_FCV_TYPE_TXT3D_COL0A,
      DK_FCV_TYPE_TXT3D_COL1A,
      DK_FCV_TYPE_TXT3D_COL2A,
      DK_FCV_TYPE_TXT3D_COL3A,
      DK_FCV_TYPE_TXT3D_COL4A,
      DK_FCV_TYPE_TXT_BLD,

      DK_FCV_TYPE_TXT_REFL,

      DK_FCV_TYPE_WAVE_SCLX,
      DK_FCV_TYPE_WAVE_SCLY,
      DK_FCV_TYPE_WAVE_SCLZ,
      DK_FCV_TYPE_WAVE_ROTX,
      DK_FCV_TYPE_WAVE_ROTY,
      DK_FCV_TYPE_WAVE_ROTZ,
      DK_FCV_TYPE_WAVE_TRNX,
      DK_FCV_TYPE_WAVE_TRNY,
      DK_FCV_TYPE_WAVE_TRNZ,
      DK_FCV_TYPE_WAVE_POS,
      DK_FCV_TYPE_WAVE_DCY,
      DK_FCV_TYPE_WAVE_SPD,
      DK_FCV_TYPE_MDL_HIDDEN,
      DK_FCV_TYPE_LIG_HIDDEN,
      DK_FCV_TYPE_CAM_HIDDEN,
      DK_FCV_TYPE_MDL_ALGO,

      DK_FCV_TYPE_DYN_ELAST,    /* not supported in DKit */
      DK_FCV_TYPE_DYN_MU_S,     /* not supported in DKit */
      DK_FCV_TYPE_DYN_MU_K,     /* not supported in DKit */
      DK_FCV_TYPE_DYN_ROOTC,    /* not supported in DKit */

      DK_FCV_TYPE_MDL_CLUSTER,
      DK_FCV_TYPE_MDL_WCLUSTER,

      DK_FCV_TYPE_CLAY_WEIGHT,  /* not supported in DKit */
      DK_FCV_TYPE_CLAY_EXPX,    /* not supported in DKit */
      DK_FCV_TYPE_CLAY_EXPY,    /* not supported in DKit */
      DK_FCV_TYPE_CLAY_EXPZ,    /* not supported in DKit */
      DK_FCV_TYPE_SUN_TIME,     /* not supported in DKit */

      DK_FCV_TYPE_FOG_START,
      DK_FCV_TYPE_FOG_STOP,
      DK_FCV_TYPE_FOG_RED,
      DK_FCV_TYPE_FOG_GREEN,
      DK_FCV_TYPE_FOG_BLUE,
      DK_FCV_TYPE_FOG_DENS,
      DK_FCV_TYPE_FOG_BASE,
      DK_FCV_TYPE_FOG_THICK,

      DK_FCV_TYPE_TXT_SROT_X,	
      DK_FCV_TYPE_TXT_SROT_Y,
      DK_FCV_TYPE_TXT_SROT_Z,
      DK_FCV_TYPE_TXT_SSCAL_X,
      DK_FCV_TYPE_TXT_SSCAL_Y,
      DK_FCV_TYPE_TXT_SSCAL_Z,
      DK_FCV_TYPE_TXT_STRANS_X,
      DK_FCV_TYPE_TXT_STRANS_Y,
      DK_FCV_TYPE_TXT_STRANS_Z,

      DK_FCV_TYPE_LIG_AROT_X,
      DK_FCV_TYPE_LIG_AROT_Y,
      DK_FCV_TYPE_LIG_AROT_Z,
      DK_FCV_TYPE_LIG_ASCL_X,
      DK_FCV_TYPE_LIG_ASCL_Y,
      DK_FCV_TYPE_LIG_ASCL_Z,
      
      DK_FCV_TYPE_MDL_LINVEL_X,
      DK_FCV_TYPE_MDL_LINVEL_Y, 
      DK_FCV_TYPE_MDL_LINVEL_Z,
      DK_FCV_TYPE_MDL_LINACC_X,
      DK_FCV_TYPE_MDL_LINACC_Y,
      DK_FCV_TYPE_MDL_LINACC_Z,
      DK_FCV_TYPE_MDL_ANGVEL_X,
      DK_FCV_TYPE_MDL_ANGVEL_Y,
      DK_FCV_TYPE_MDL_ANGVEL_Z,
      DK_FCV_TYPE_MDL_ANGACC_X,
      DK_FCV_TYPE_MDL_ANGACC_Y,
      DK_FCV_TYPE_MDL_ANGACC_Z,

      DK_FCV_TYPE_MDL_LVFLEX_MAX,
      DK_FCV_TYPE_MDL_LVSTCH_MAX,
      DK_FCV_TYPE_MDL_LVYILD_MAX,
      DK_FCV_TYPE_MDL_LAFLEX_MAX,
      DK_FCV_TYPE_MDL_LASTCH_MAX,
      DK_FCV_TYPE_MDL_LAYILD_MAX,
      DK_FCV_TYPE_MDL_AVFLEX_MAX,
      DK_FCV_TYPE_MDL_AVSTCH_MAX,
      DK_FCV_TYPE_MDL_AVYILD_MAX,
      DK_FCV_TYPE_MDL_AAFLEX_MAX,
      DK_FCV_TYPE_MDL_AASTCH_MAX,
      DK_FCV_TYPE_MDL_AAYILD_MAX,
      DK_FCV_TYPE_MDL_LVFLEX_SEN,
      DK_FCV_TYPE_MDL_LVSTCH_SEN,
      DK_FCV_TYPE_MDL_LVYILD_SEN,
      DK_FCV_TYPE_MDL_LAFLEX_SEN,
      DK_FCV_TYPE_MDL_LASTCH_SEN,
      DK_FCV_TYPE_MDL_LAYILD_SEN,
      DK_FCV_TYPE_MDL_AVFLEX_SEN,
      DK_FCV_TYPE_MDL_AVSTCH_SEN,
      DK_FCV_TYPE_MDL_AVYILD_SEN,
      DK_FCV_TYPE_MDL_AAFLEX_SEN,
      DK_FCV_TYPE_MDL_AASTCH_SEN,
      DK_FCV_TYPE_MDL_AAYILD_SEN,

      DK_FCV_TYPE_BLUR_WIDTH,
      DK_FCV_TYPE_BLUR_DECAY
    } DK_FCURVE_TYPE;

typedef
   struct DK_fcvkey
   {
      float	time;		/* time of this key */
      float     value;		/* value at this key */
      float     rslope;		/* right slope at this key */
      float     lslope;		/* left slope at this key */
      short     slopeFlag;	/* define management of the slope */
      short     interp;		/* interpolation type at right */
      short     keyType;	/* 0 = free form, 1 = keyframe, 2 = keypath,
				   3 = raw key */
      short     spIndex; 	/* DK_spline point index */
   } DK_Fcvkey;

typedef
   struct
   {
      short	   	type;		/* fcurve type */
      struct DK_fcvkey *keys;		/* array of key (allocated in block) */
      short        	size;		/* nb of key in the fcurve */
      short        	pre_extrap;	/* pre-extrapolation type */
      short        	post_extrap;	/* post-extrapolation type */
      short        	slopeFlag;	/* default slope flag for this curve */
      short        	interp;		/* default interp. for this curve */
   } DK_Fcurve;

typedef
   struct
   {
      short		active;		/* overall activation flag   */
      DK_Fcurve		*fcv;		/* activation function curve */
   } DK_actFcurve;

/* Global DK_Fcurve prototypes */
extern	DK_Fcvkey *DK_fcvkeyAllocate( int );
extern	void	   DK_fcvkeyDispose( DK_Fcvkey ** );
extern	DK_Fcurve *DK_fcurveAllocate( void );
extern  void       DK_fcurveArrayDispose( DK_Fcurve *[], int );
extern  void       DK_actFcurveArrayDispose( DK_actFcurve [], int );
extern	void	   DK_fcurveDispose( DK_Fcurve ** );
extern	void	   DK_fcurveCopy( DK_Fcurve *, DK_Fcurve ** );
extern  int 	   DK_fcurveWriteAsciiAnimation ( FILE *, DK_Fcurve *);
extern  float	   DK_fcurveValue(DK_Fcurve *, float);
extern  int	   DK_fcurveIntegerValue(DK_Fcurve *, float, int);
extern  float	   DK_fcurveFloatValue(DK_Fcurve *, float);
extern  int 	   DK_fcurveReadAsciiAnimation ( FILE *, DK_Fcurve **);

#endif  /* ifndef __TINY_FCURVE_H__ */
