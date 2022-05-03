/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: material.h 1.19 1995/05/25 13:37:32 sam Exp $
 * $Locker:  $
 *
 * Describes the appearance of a material than can be applied to a surface
 */
#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct br_material {

	char *identifier;

	br_colour colour;

	br_uint_8 opacity;

	br_ufraction ka;
	br_ufraction kd;
	br_ufraction ks;

	/*
	 * Specular power
	 */
	br_scalar power;

	/*
	 * Controls how texture is rendered
	 */
	br_uint_32 flags;

	/*
	 * 2x3 transform applied to model U,V
	 */
	br_matrix23 map_transform;

	/*
	 * The direct index ramp base and range
	 */
	br_uint_8 index_base;
	br_uint_8 index_range;

	/*
	 * Pointers to colour map
	 */
	br_pixelmap *colour_map;

	/*
	 * Pointers to tables
	 */
	br_pixelmap *screendoor;		/* Screen door opacity  */

	br_pixelmap *index_shade;		/* Index shading		*/
	br_pixelmap *index_blend;		/* Index blending		*/

	/*
	 * Private fields
	 */
	br_uint_8 prep_flags;
	
	void	  *rptr;

} br_material;

/*
 * Bits for br_material->flags
 */
enum {
	BR_MATF_LIGHT			= 0x00000001,
	BR_MATF_PRELIT			= 0x00000002,

	BR_MATF_SMOOTH			= 0x00000004,

	BR_MATF_ENVIRONMENT_I	= 0x00000008,
	BR_MATF_ENVIRONMENT_L	= 0x00000010,
	BR_MATF_PERSPECTIVE		= 0x00000020,
	BR_MATF_DECAL			= 0x00000040,

	BR_MATF_I_FROM_U		= 0x00000080,
	BR_MATF_I_FROM_V		= 0x00000100,
	BR_MATF_U_FROM_I		= 0x00000200,
	BR_MATF_V_FROM_I		= 0x00000400,

	BR_MATF_ALWAYS_VISIBLE	= 0x00000800,
	BR_MATF_TWO_SIDED		= 0x00001000,

	BR_MATF_FORCE_Z_0		= 0x00002000,

	BR_MATF_DITHER			= 0x00004000
#if 0
	BR_MATF_CUSTOM			= 0x00008000
#endif
};

/*
 * Backwards compatibility
 */
#define BR_MATF_GOURAUD		BR_MATF_SMOOTH
#define BR_MATF_MAP_COLOUR	0

/*
 * Flags to BrMaterialUpdate()
 */
enum {
	BR_MATU_MAP_TRANSFORM	= 0x0001,
	BR_MATU_RENDERING		= 0x0002,
	BR_MATU_LIGHTING		= 0x0004,
	BR_MATU_COLOURMAP		= 0x0008,
	BR_MATU_ALL				= 0x7fff
};

/*
 * Flags to BrMapUpdate()
 */
enum {
	BR_MAPU_ALL				= 0x7fff
};

/*
 * Flags to BrTableUpdate()
 */
enum {
	BR_TABU_ALL				= 0x7fff
};

#ifdef __cplusplus
};
#endif
#endif

