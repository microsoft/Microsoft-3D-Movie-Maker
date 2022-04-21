/*******************************************************************************
********************************************************************************

   $Source: /drive3/SI2/creative/RCS/DKit_h/tinyByteswap.h,v $
   $Revision: 1.2 $ $Date: 1995/04/11 18:11:12 $
   Checkin by: $Author: ******$

   <DESCRIPTION>

   File supervisor: Allan Thorpe

   (c) Copyright 1994 Softimage, Inc.

********************************************************************************
*******************************************************************************/

/*****************************************************************************/

/*  This file defines SI_BYTESWAP if the Endian ordering needs to be changed. */

#ifndef _BYTESWAP_H_ 
#define _BYTESWAP_H_

#include "DKfilter.h"
#include "tyFile.h"
#include "tySoftTy.h"
				   
#ifdef _WIN32
#define SI_BYTESWAP 1
#endif

extern void DK_streamInitialize (void);

extern void SI_fix_SI_FileChunk( DK_BinaryHeader * ptr );
extern size_t SI_3doubles_fread( double * buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_3doubles_fwrite( const double * buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_byteswap_fread( void * buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_byteswap_fwrite( const void * buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_Vector_fread( DK_Vector * buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_Vector_fwrite( const DK_Vector * buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_String_fwrite( const char * buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_FileChunk_fwrite( const DK_BinaryHeader *, size_t size, size_t count, FILE * stream );
extern size_t SI_FileChunk_fread( const DK_BinaryHeader *, size_t size, size_t count, FILE * stream );
extern size_t SI_Matrix_fread( DK_Matrix buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_Matrix_fwrite( const DK_Matrix buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_RGB_fread( DK_Colour24 * buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_RGB_fwrite( const DK_Colour24 * buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_RGBA_fread( DK_Colour32 * buffer, size_t size, size_t count, FILE * stream );
extern size_t SI_RGBA_fwrite( const DK_Colour32 * buffer, size_t size, size_t count, FILE * stream );
extern void SI_swap4byte( unsigned long * a );

#endif
