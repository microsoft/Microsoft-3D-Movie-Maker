/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: colour.h 1.3 1995/02/22 21:37:03 sam Exp $
 * $Locker:  $
 *
 * Colour type and macros
 *
 */
#ifndef _COLOUR_H_
#define _COLOUR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long int br_colour;

#define BR_COLOUR_RGB(r,g,b) \
		((((unsigned int)(r))<<16) |\
		(((unsigned int)(g))<<8) |\
		((unsigned int)(b)))

#define BR_COLOUR_RGBA(r,g,b,a) \
		((((unsigned int)(a))<<24) |\
		((((unsigned int)(r))<<16) |\
		(((unsigned int)(g))<<8) |\
		((unsigned int)(b)))

#define BR_ALPHA(c) ((c >> 24) & 0xFF)
#define BR_RED(c) ((c >> 16) & 0xFF)
#define BR_GRN(c) ((c >> 8) & 0xFF)
#define BR_BLU(c) ((c) & 0xFF)

#ifdef __cplusplus
};
#endif
#endif

