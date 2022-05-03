/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: light.h 1.6 1995/05/25 13:37:32 sam Exp $
 * $Locker:  $
 *
 * Definitons for a light
 */
#ifndef _LIGHT_H_
#define _LIGHT_H_

#ifdef __cplusplus
extern "C" {
#endif

enum {
	/*
	 * Type of light
	 */
	BR_LIGHT_TYPE  = 0x0003,
	 BR_LIGHT_POINT  = 0x0000,
 	 BR_LIGHT_DIRECT = 0x0001,
	 BR_LIGHT_SPOT   = 0x0002,

	/*
     * Flag idicating that caluculations are done in view space
     */ 
	BR_LIGHT_VIEW  = 0x0004
};

typedef struct br_light {
	/*
	 * Optional identifier
	 */
	char *identifier;

	/*
	 * Type of light
	 */
	br_uint_8 type;

	/*
	 * Colour of light (if renderer supports it)
	 */
	br_colour colour;

	/*
	 * Attenuation of light with distance - constant, linear, and quadratic
	 * l & q only apply to point and spot lights
	 */
	br_scalar attenuation_c;
	br_scalar attenuation_l;
	br_scalar attenuation_q;

	/*
	 * Cone angles for spot light
	 */
	br_angle cone_outer;
	br_angle cone_inner;

} br_light;

#ifdef __cplusplus
};
#endif
#endif
