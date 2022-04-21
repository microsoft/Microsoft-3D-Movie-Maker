/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyFile.h,v $
   $Revision: 1.15 $ $Date: 1994/11/15 15:18:22 $
   Checkin by: $Author: ****** $

   This file contains a subset of control procedures and variables use in
   Softimage programming environment.

   Written by: Laurent Lauzon

   (c) Copyright 1990, 1991, 1992 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_FILE_H__
#define __TINY_FILE_H__

#include "tySoftTy.h"

typedef
   enum
   {
      DK_FIL_ASCII,
      DK_FIL_BINARY
   } DK_FileFormat;

/* structure containing prototypes description found in binary file */
typedef
   struct
   {
      short	 nbModel;
      short	 nbFace;
      short	 nbPatch;
      short	 nbMesh;
      short	 nbPolygon;
      short	 nbPolNode;
      short	 nbEdge;
      short	 nbVertex;
      short	 nbSpline;
      short	 nbSplKey;
      short	 nbBezier;
      short	 nbBezKey;
      short	 nbMaterial;
      short	 nbTexture3D;
      short	 nbTexture;
      short	 nbPhysical;
      short	 nbJoint;
      short	 nbEnvelopeData;
      short	 nbEnvelopeVertex;
      short	 nbCluster;
      short	 nbMetaball;
      short	 nbMetaSystem;
      short	 nbMetaGroup;
      short	 nbMetaUnblend;
      short	 nbNurbsCrv;
      short	 nbNurbsSrf;
      short	 nbNurbsCompCrv;
      short	 nbUdf;

      short	*model;
      short	*face;
      short	*patch;
      short	*mesh;
      short	*polygon;
      short	*polNode;
      short	*edge;
      short	*vertex;
      short	*spline;
      short	*splKey;
      short	*bezier;
      short	*bezKey;
      short	*material;
      short	*texture3D;
      short	*texture;
      short	*physical;
      short	*joint;
      short	*envelopeData;
      short	*envelopeVertex;
      short	*cluster;
      short	*metaball;
      short	*metaSystem;
      short	*metaGroup;
      short	*metaUnblend;
      short	*nurbsCrv;
      short	*nurbsSrf;
      short	*nurbsCompCrv;
      short	*udf;
   } DK_FileProt;

#define DK_ASCII_FILE_HEADER	"Softimage 4D Creative Environment"
#define DK_BINARY_MAGIC_NUMBER	0x5380f634   /* 'S' + HEX( 845 1636 ) */

/*---------------------------------------------*/
/* Magic Numbers to identify the files content */
/*---------------------------------------------*/
#define DK_MAGIC_OLD_BIN   0x5380f634   /* 'S' + HEX( 845 1636 ) */
#define DK_MAGIC_ANI_BIN   0x5380f635
#define DK_MAGIC_CAM_BIN   0x5380f636
#define DK_MAGIC_CLS_BIN   0x5380f637
#define DK_MAGIC_LAT_BIN   0x5380f638
#define DK_MAGIC_LIG_BIN   0x5380f639
#define DK_MAGIC_MAT_BIN   0x5380f640
#define DK_MAGIC_MOD_BIN   0x5380f641      /* not used yet */
#define DK_MAGIC_SHP_BIN   0x5380f642
#define DK_MAGIC_T2D_BIN   0x5380f643
#define DK_MAGIC_T3D_BIN   0x5380f644
#define DK_MAGIC_PIC_BIN   0x5380f645      /* not used yet */
#define DK_MAGIC_KEY_BIN   0x5380f646      /* not used yet */

#define DK_MAGIC_START_SI_BIN      0x5380f634
#define DK_MAGIC_END_SI_BIN        0x5380f646

#define DK_MAGIC_SCN_ASC   'SSCN'
#define DK_MAGIC_DSC_ASC   'SDSC'
#define DK_MAGIC_SET_ASC   'SSET'
#define DK_MAGIC_CHN_ASC   'SCHN'
#define DK_MAGIC_DRV_ASC   'SDRV'
#define DK_MAGIC_WAV_ASC   'SWAV'
#define DK_MAGIC_CLS_ASC   'CLUS'
#define DK_MAGIC_FLK_ASC   'SFLK'

/* Header found in every SOFTIMAGE binary file */
typedef
   struct
   {
      long	ID;
      float     version;	/* version of format (= Softimage release) */
      char      comment[80];
   } DK_BinaryHeader;

extern float SI_versionInput;


/* Global prototypes files functions */

extern DK_FileProt *DK_prototypeAllocate( void );
extern DK_FileProt *DK_prototypeRead( FILE * );
extern DK_Boolean   DK_prototypeWrite( FILE *, DK_FileProt * );
extern void 	    DK_prototypeDispose( DK_FileProt ** );
extern DK_Boolean   DK_prototypeInitModel( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitFace( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitSpline( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitMesh( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitPatch( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitMaterial( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitTexture3D( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitTexture( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitPhysical( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitJoint( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitEnvelopeData( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitEnvelopeVertex( DK_FileProt **, DK_Boolean);
extern DK_Boolean   DK_prototypeInitCluster( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitMetaball( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitNurbsCrv( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitNurbsSrf( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitNurbsCompCrv( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeInitUDF( DK_FileProt **, DK_Boolean );
extern DK_Boolean   DK_prototypeWrite( FILE *, DK_FileProt * );
extern DK_String    DK_stringReadBinary( FILE * );
extern DK_String    DK_stringReadAscii( FILE * );
extern DK_Boolean   DK_headerWriteAscii( FILE *, DK_String );
extern DK_Boolean   DK_headerWriteBinary( FILE *, DK_String );
extern long         DK_headerReadAscii( FILE *, float * );
extern long         DK_headerReadBinary( FILE *, float *, DK_String );
extern DK_Boolean   DK_stringWriteBinary( FILE *, DK_String );
extern void         DK_userDataWriteAscii( FILE *, DK_UserNode *, DK_String );
extern DK_UserNode *DK_userDataReadAscii( FILE * );

extern DK_Boolean   DK_fileSkipField( FILE *file, short tok );
extern long         DK_fileMarkPlace( FILE *file );
extern DK_Boolean   DK_fileBackWriteLength( FILE *file, long addr );


#endif  /* ifndef __TINY_FILE_H__ */


