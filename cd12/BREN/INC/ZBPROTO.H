/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: zbproto.h 1.8 1995/02/22 21:37:28 sam Exp $
 * $Locker:  $
 *
 * Function prototypes for Z buffer renderer
 */
#ifndef _ZBPROTO_H_
#define _ZBPROTO_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BR_BOUNDS_MIN_X	0
#define BR_BOUNDS_MIN_Y	1
#define BR_BOUNDS_MAX_X	2
#define BR_BOUNDS_MAX_Y	3

/*
 * Callback function invoked when an actor is
 * rendered
 */
typedef void BR_CALLBACK br_renderbounds_cbfn(
	br_actor *actor,
	br_model *model,
	br_material *material,
	br_uint_8	style,
	br_matrix4 *model_to_screen,
	br_int_32 bounds[4]);

#ifndef _NO_PROTOTYPES

void BR_PUBLIC_ENTRY BrZbBegin(br_uint_8 colour_type, br_uint_8 depth_type);
void BR_PUBLIC_ENTRY BrZbEnd(void);

void BR_PUBLIC_ENTRY BrZbSceneRenderBegin(br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer,
					br_pixelmap *depth_buffer);

void BR_PUBLIC_ENTRY BrZbSceneRenderAdd(br_actor *tree);
void BR_PUBLIC_ENTRY BrZbSceneRenderEnd(void);

/*
 * Wrapper that inokes above therr calls in order
 */
void BR_PUBLIC_ENTRY BrZbSceneRender(
					br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer,
					br_pixelmap *depth_buffer);

/*
 * Used within custom model callbacks to render other models
 */
void BR_PUBLIC_ENTRY BrZbModelRender(br_actor *actor,
				  br_model *model,
				  br_material *material,
				  br_uint_8 style,
				  int on_screen,
				  int use_custom);

br_renderbounds_cbfn * BR_PUBLIC_ENTRY BrZbSetRenderBoundsCallback(br_renderbounds_cbfn *new_cbfn);

#ifdef __cplusplus
};
#endif
#endif
#endif
