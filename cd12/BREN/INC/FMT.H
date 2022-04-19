/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: fmt.h 1.5 1995/02/22 21:37:06 sam Exp $
 * $Locker:  $
 *
 * Definitions for foreign file format support
 */
#ifndef _FMT_H_
#define _FMT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Type of things to load from file
 */
enum {
	BR_FMT_MODELS		= 0x0001,	/* Create model definitions from file	*/
	BR_FMT_ACTORS		= 0x0002,	/* Create actor							*/
	BR_FMT_LIGHTS		= 0x0004,	/* Create lights instances				*/
	BR_FMT_CAMERAS		= 0x0008,	/* Create cameras instances				*/
	BR_FMT_MATERIALS	= 0x0010	/* Create materials						*/
};

/*
 * Interchange structure for passing to file format loaders
 *
 * XXX Not yet used by anything
 */
typedef struct br_fmt_results {
	/*
	 * pointers to tables of each loaded allocated by BrFMTxxxLoad()
	 */
	br_model *models;
	br_model *materials;
	br_model *lights;
	br_model *cameras;
	br_model *actors;

	/*
	 * Size of each allocated array
	 */
	int nmodels;
	int nmaterials;
	int nlights;
	int ncameras;
	int nactors;

} br_fmt_results;

#ifndef _FMTPROTO_H_
#include "fmtproto.h"
#endif

#ifdef __cplusplus
};
#endif
#endif


