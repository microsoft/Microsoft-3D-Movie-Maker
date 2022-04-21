/***************************************************************************************************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyMetaball.h,v $
   $Revision: 1.2 $ $Date: 1992/12/28 15:30:12 $
   Checkin by: $Author: ******$

   This file contains a subset of the SOFTIMAGE's metaball data structure,
   its content is sufficient to read and write data structure into
   SOFTIMAGE's file format...

   Written by: Laurent Lauzon

   (c) Copyright 1992, 1993 SOFTIMAGE Inc.

***************************************************************************************************************************************************************/

#ifndef __TINY_METABALL_H__
#define __TINY_METABALL_H__

#include  "tySoftTy.h"
#include  "tyFile.h"

typedef
   struct DK_metaball
   {
     DK_Matrix	centerMat;
     double	weight;
     double	ratio;
     double	exponent[3];
   } DK_Metaball;

typedef
   struct DK_metaGroup
   {
      DK_String		   name;
      struct DK_metaLink  *prims;
      struct DK_metaGroup *next;
      struct DK_metaGroup *prev;
   } DK_MetaGroup;

typedef
   struct DK_metaUnblend
   {
      DK_String		     groups[2];	/* name of the 2 groups */
      struct DK_metaUnblend *prev;
      struct DK_metaUnblend *next;
   } DK_MetaUnblend;

typedef
   struct DK_metaLink
   {
      DK_String		  prim;		/* name of the metaball model */
      struct DK_metaLink *next;
      struct DK_metaLink *prev;
   } DK_MetaLink;

typedef
   struct
   {
      float		wfResolution;
      float		renResolution;
      short		showSurface;
      DK_MetaGroup	*groupList;
      DK_MetaUnblend	*unblendList;
   } DK_MetaSystem;

/* Global metaball prototypes */

extern DK_Metaball *DK_metaballAllocate( void );
extern void         DK_metaballDispose( DK_Metaball ** );
extern DK_Boolean   DK_metaballWriteBinary(FILE *,DK_Metaball *, DK_FileProt *);
extern DK_Metaball *DK_metaballReadBinary( FILE *, DK_FileProt * );
extern DK_Boolean   DK_metaballWriteAscii( FILE *, DK_Metaball *, DK_String );
extern DK_Metaball *DK_metaballReadAscii( FILE * );

extern DK_MetaSystem *DK_metaSystemAllocate( void );
extern void           DK_metaSystemDispose( DK_MetaSystem ** );
extern DK_Boolean     DK_metaSystemWriteBinary(
			 FILE *, DK_MetaSystem *, DK_FileProt *
		      );
extern DK_MetaSystem *DK_metaSystemReadBinary( FILE *, DK_FileProt * );
extern DK_Boolean     DK_metaSystemWriteAscii( 
			 FILE *, DK_MetaSystem *, DK_String
		      );
extern DK_MetaSystem *DK_metaSystemReadAscii( FILE * );

extern DK_MetaGroup *DK_metaGroupAllocate( DK_MetaSystem * );
extern void         *DK_metaGroupDispose( DK_MetaSystem *, DK_MetaGroup ** );
extern DK_Boolean    DK_metaGroupAddPrimitive( DK_MetaGroup *, DK_String );
extern DK_Boolean    DK_metaGroupRemovePrimitive( DK_MetaGroup *, DK_String );
extern DK_Boolean    DK_metaGroupsBlend( 
			DK_MetaSystem *, DK_String, DK_String 
		     );
extern DK_Boolean    DK_metaGroupsUnblend( 
			DK_MetaSystem *, DK_String, DK_String 
		     );

#endif  /* ifndef __TINY_METABALL_H__ */
