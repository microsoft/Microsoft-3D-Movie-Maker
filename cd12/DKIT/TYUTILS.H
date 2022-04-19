/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyUtils.h,v $
   $Revision: 1.16 $ $Date: 1995/02/03 23:25:01 $
   Checkin by: $Author: a_tan $

   This file contains a subset of the SOFTIMAGE's utilities data structures
   and defines.

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#include "tySoftTy.h"
#include "tyFcurve.h"
#include "tySpline.h"
#include "tySceneT.h"

typedef
    enum
    {
        DK_UNSELECTED,
        DK_NODE_SELECTED,
        DK_INHERIT_SELECTED,
        DK_BRANCH_SELECTED
    } SelectType;

#define _DK_OUT_NEWLINE() \
	fprintf(file,"\n")

#define _DK_OUT_4SPACES() \
	fprintf(file,"    ")

#define _DK_OUT_INDENT() \
	fprintf(file,"%s", DK_animIndentString)

#define _DK_OUT_TOKEN(t) \
	fprintf(file,"%s ", (char *) t)

#define _DK_OUT_STRING(t) \
	if (t == NULL)\
	    fprintf(file,"'' ");\
	else\
	    fprintf(file,"'%s' ", (char *) t)

#define _DK_OUT_SVALUE(t) \
	fprintf(file, "%d ", (int) t)

#define _DK_OUT_FVALUE(t) \
	fprintf(file, "%g ", (DK_FVALUE) t)

#define _DK_OUT_BOOLEAN(t) \
        { \
	   if ( t ) \
	      _DK_OUT_TOKEN( DK_A_YES_TOKEN ); \
	   else \
	      _DK_OUT_TOKEN( DK_A_NO_TOKEN ); \
	}

#define _DK_SET_BOOLEAN( t, val ) \
        { \
	   if (strcmp( DK_A_YES_TOKEN, t ) == 0) \
	      (val) = TRUE; \
	   else \
	      (val) = FALSE; \
	}


#define _DK_IN_TOKEN()\
    /* skip white space */\
    DK_animTokenLine[0] = '\0';\
    while ((ci = fgetc(file)) != EOF && \
	(ci == ' ' || ci == '\t' || ci == '\n'))\
	_DK_NOP();\
    if (ci == EOF)\
	return(FALSE);\
    /* read up to and including the next white space */\
    DK_animTokenLine[0] = ci;\
    i = 1;\
    while ((ci = fgetc(file)) != EOF && ci != ' ' && ci != '\t' && ci != '\n')\
	DK_animTokenLine[i++] = ci;\
    DK_animTokenLine[i] = '\0';

/* misc */
#define DK_FCV_NUM 235

typedef 
    struct DK_fcurve_name 
    {
    	char *str;
    } DK_Fcurve_name;

#ifdef __TINY_UTILS_H__

/* GLOBAL variables */

/* fcurve names */
DK_Fcurve_name  DK_fcv_name[DK_FCV_NUM] = {"",
   "MDL_SCL_X",	  "MDL_SCL_Y",	 "MDL_SCL_Z",	"MDL_ROT_X",   "MDL_ROT_Y",
   "MDL_ROT_Z",	  "MDL_POS",	 "MDL_SHAPE",	"CAM_POS",     "CAM_INT",
   "CAM_ROLL",	  "CAM_FOV",	 "CAM_NEAR",	"CAM_FAR",     "CAM_FFCL",
   "CAM_FSTP",	  "CAM_FDST",	 "CAM_NFFC",	"CAM_FFFC",    "CAM_FOUT",
   "CAM_FCOC",	  "LIG_COL_R",	 "LIG_COL_G",	"LIG_COL_B",   "LIG_CONE",

   "LIG_SPREAD",  "LIG_DECAY",	 "LIG_START",	"LIG_END",     "LIG_SHADOW",
   "LIG_POS",	  "LIG_INT",	 "TXT_U_SCL",	"TXT_V_SCL",   "TXT_U_OFF",
   "TXT_V_OFF",	  "TXT_AMB",	 "TXT_DIFF",	"TXT_SPEC",    "TXT_REFL",
   "TXT_TRSP",	  "TXT_RIND",	 "TXT_NORM",	"TXT_ROT",     "MAT_AMB_R",
   "MAT_AMB_G",	  "MAT_AMB_B",	 "MAT_DIFF_R",	"MAT_DIFF_G",  "MAT_DIFF_B",

   "MAT_SPEC_R",  "MAT_SPEC_G",	 "MAT_SPEC_B",	"MAT_EXPN",    "MAT_REFL",
   "MAT_RFL_J",	  "MAT_TRSP",	 "MAT_TRSP_J",	"MAT_RIND",    "FAD_START",
   "FAD_END",	  "FAD_RED",	 "FAD_GREEN",	"FAD_BLUE",    "FAD_TRANSP",
   "LAT_NODE",	  "LAT_TREE",	 "TXT3D_STR",	"TXT3D_SPA",   "TXT3D_POW",
   "TXT3D_SRT",	  "TXT3D_TAP",	 "TXT3D_FAC1",	"TXT3D_FAC2",  "TXT3D_FAC3",

   "TXT3D_COL0R", "TXT3D_COL0G", "TXT3D_COL0B", "TXT3D_COL1R", "TXT3D_COL1G",
   "TXT3D_COL1B", "TXT3D_COL2R", "TXT3D_COL2G", "TXT3D_COL2B", "TXT3D_COL3R",
   "TXT3D_COL3G", "TXT3D_COL3B", "TXT3D_COL4R", "TXT3D_COL4G", "TXT3D_COL4B",
   "TXT3D_SCLX",  "TXT3D_SCLY",	 "TXT3D_SCLZ",	"TXT3D_ROTX",  "TXT3D_ROTY",
   "TXT3D_ROTZ",  "TXT3D_TRNX",	 "TXT3D_TRNY",	"TXT3D_TRNZ",  "TXT3D_AMB",

   "TXT3D_DIF",	  "TXT3D_SPC",	 "TXT3D_REF",	"TXT3D_TRS",   "TXT3D_RID",
   "TXT3D_ROU",	  "NFIT_SCLX",	 "NFIT_SCLY",	"NFIT_SCLZ",   "NFIT_ROLL",
   "NFIT_TRNX",	  "NFIT_TRNY",	 "NFIT_TRNZ",	"TFIT_SCLX",   "TFIT_SCLY",
   "TFIT_SCLZ",	  "TFIT_ROLL",	 "TFIT_TRNX",	"TFIT_TRNY",   "TFIT_TRNZ",
   "PNFIT_SCLX",  "PNFIT_SCLY",	 "PNFIT_SCLZ",	"PNFIT_ROTX",  "PNFIT_ROTY",

   "PNFIT_ROTZ",  "PNFIT_TRNX",	 "PNFIT_TRNY",	"PNFIT_TRNZ",  "PTFIT_SCLX",
   "PTFIT_SCLY",  "PTFIT_SCLZ",	 "PTFIT_ROTX",	"PTFIT_ROTY",  "PTFIT_ROTZ",
   "PTFIT_TRNX",  "PTFIT_TRNY",	 "PTFIT_TRNZ",	"MDL_POSX",    "MDL_POSY",
   "MDL_POSZ",	  "CAM_POSX",	 "CAM_POSY",	"CAM_POSZ",    "CAM_INTX",
   "CAM_INTY",	  "CAM_INTZ",	 "LIG_POSX",	"LIG_POSY",    "LIG_POSZ",

   "LIG_INTX",	  "LIG_INTY",	 "LIG_INTZ",	"SWEIGHT",     "TXT3D_BLEND",
   "TXT3D_COL0A", "TXT3D_COL1A", "TXT3D_COL2A", "TXT3D_COL3A", "TXT3D_COL4A",
   "TXT_BLEND",	  "TXT_REFL2",	 "WAV_SCLX",	"WAV_SCLY",    "WAV_SCLZ",
   "WAV_ROTX",	  "WAV_ROTY",	 "WAV_ROTZ",	"WAV_TRNX",    "WAV_TRNY",
   "WAV_TRNZ",	  "WAV_POS",	 "WAV_DCY",	"WAV_SPD",     "MDL_HID",

   "LIG_HID",	  "CAM_HID",	 "MDL_ALGO",	"DYN_ELAST",   "DYN_SROUGH",
   "DYN_KROUGH",  "DYN_ROOTC",	 "MDL_CLUSTER", "MDL_WCLUSTER","FOG_START",
   "FOG_END",	  "FOG_RED",	 "FOG_GREEN",	"FOG_BLUE",    "FOG_DENS",
   "FOG_BASE",	  "FOG_THICK",   "LIG_AROT_X",  "LIG_AROT_Y",  "LIG_AROT_Z",
   "LIG_ASCL_X",  "LIG_ASCL_Y",  "LIG_ASCL_Z"
	   };

char       *DK_animIndentString = NULL;
char        DK_animTokenLine[DK_MAXLINE];
char       *DK_animToken = &DK_animTokenLine[0];
float       DK_animFValue;
short       DK_animSValue;
float       DK_animFileVersion = 0.0;
DK_Boolean  DK_CLASSIC_SCALING = FALSE;

#else

extern DK_Fcurve_name  DK_fcv_name[];
extern char           *DK_animIndentString;
extern char            DK_animTokenLine[];
extern char           *DK_animToken;
extern float           DK_animFValue;
extern short           DK_animSValue;
extern float           DK_animFileVersion;
extern DK_Boolean      DK_CLASSIC_SCALING;

#endif

extern  char 	*DK_stringReadAsciiAnimation ( FILE *);
extern  int 	DK_in_animFValue_na ( FILE *);
extern  int 	DK_in_animFValue ( FILE *, DK_FVALUE *);
extern  int 	DK_in_animSValue ( FILE *, DK_SVALUE *);
extern  int 	DK_in_animSValue_na ( FILE *);
extern  int 	DK_in_animLValue ( FILE *, long *);
extern  int 	DK_in_animToken ( FILE *);
extern  void 	DK_incIndent ( int );
extern  void 	DK_setIndentLength ( int);
extern  int 	DK_vectorWriteAsciiAnimation ( FILE *, DK_Vector *);
extern  int 	DK_vectorReadAsciiAnimation ( FILE *, DK_Vector *);
extern  int 	DK_hvectorWriteAsciiAnimation ( FILE *, DK_HVector *);
extern  int 	DK_hvectorReadAsciiAnimation ( FILE *, DK_HVector *);
extern  int 	DK_valueReadAsciiAnimation ( FILE *, float *, 
			DK_Fcurve **, int);
extern  int 	DK_valueWriteAsciiAnimation ( FILE *, float, DK_Fcurve *);
extern  int 	DK_value3WriteAsciiAnimation ( FILE *, float, float, 
			float, DK_Fcurve *, DK_Fcurve *, DK_Fcurve *);
extern  int 	DK_value3ReadAsciiAnimation ( FILE *, float *, float *, 
			float *, DK_Fcurve **, DK_Fcurve **, DK_Fcurve **);
extern  int 	DK_positionWriteAsciiAnimation ( FILE *, DK_Vector *, 
			DK_Fcurve *, DK_Spline *, DK_String, 
			short, DK_Fcurve *, DK_Fcurve *, DK_Fcurve *, DK_String );
extern  int 	DK_positionReadAsciiAnimation ( FILE *, DK_Vector *, 
			DK_Fcurve **, DK_Spline **, DK_String *, short  *,
			DK_Fcurve **, DK_Fcurve **, DK_Fcurve **, DK_String * );

