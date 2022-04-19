/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brlimits.h 1.5 1995/02/22 21:37:01 sam Exp $
 * $Locker:  $
 *
 * Various limits imposed by the renderer
 */

#ifndef _BRLIMITS_H_
#define _BRLIMITS_H_

#ifdef __cplusplus
	extern "C" {
#endif

/*
 * Maximum length of an item's name
 */
#define BR_MAX_NAME	256

/*
 * Maximum number of active lights
 */
#define BR_MAX_LIGHTS 16

/*
 * Maximum number of active clip planes
 */
#define BR_MAX_CLIP_PLANES 6

/*
 * Maximum number of resource classes
 */
#define BR_MAX_RESOURCE_CLASSES 256

/*
 * Maximum depth of camera in hierachy
 */
#define MAX_CAMERA_DEPTH 16


#ifdef __cplusplus
};
#endif
#endif
