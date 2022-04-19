/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyMental.h,v $
   $Revision: 1.2 $ $Date: 1994/10/27 18:37:17 $
   Checkin by: $Author: c_benoit $


   Written by: Martin-Karl Lefrancois

   (c) Copyright 1990 - 1994 SOFTIMAGE Inc.

********************************************************************************
*******************************************************************************/

#ifndef __TINY_MENTAL_H__
#define __TINY_MENTAL_H__

#include "tySoftTy.h"
#include "tyFile.h"

/* mental ray user defined function */
typedef struct DK_udf
{
   char *name;               /* name of user defined function */
   char *description;   /* one line description of this instance */
   short active;        /* 0 if disabled, 1 if enabled */
   short order;         /* order to use in multiple shaders */
   short internal;	/* non-zero if internal shader. */
} DK_UDF;

extern float DK_udfVersion;

extern DK_UDF    *DK_udfCopy        ( DK_UDF *shader );
extern DK_UDF    *DK_udfAllocate    ( void );
extern void       DK_udfDispose     ( DK_UDF **shader );
extern DK_Boolean DK_udfWriteAscii  ( FILE *, DK_UDF *, DK_String, DK_String );
extern DK_UDF    *DK_udfReadAscii   ( FILE * );
extern DK_UDF    *DK_udfReadBinary  ( FILE *, DK_FileProt * );
extern DK_Boolean DK_udfWriteBinary ( FILE *, DK_FileProt *, DK_UDF * );

#endif
