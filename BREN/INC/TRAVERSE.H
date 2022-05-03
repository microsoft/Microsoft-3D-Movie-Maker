/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: traverse.h 1.2 1995/02/22 21:37:24 sam Exp $
 * $Locker:  $
 *
 * Definitons for an traversals
 */
#ifndef _TRAVERSE_H_
#define _TRAVERSE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The system traversal types
 */
enum {
	BR_TRAVERSE_RENDER  = 0x0001,
	BR_TRAVERSE_UPDATE  = 0x0002,
	BR_TRAVERSE_COLLIDE = 0x0004,
	BR_TRAVERSE_PICK    = 0x0008,
	BR_TRAVERSE_FIND    = 0x0010,
	BR_TRAVERSE_BOUNDS  = 0x0020,

	/*
	 * Starting point for user defined values
	 */
	BR_TRAVERSE_USER_0  = 0x0100,
};

/*
 * Return flags from actor.traverse callback
 */
enum {
	BR_TRAVERSE_ACTOR		= 0x0001,
	BR_TRAVERSE_CHILDREN	= 0x0002,
};

#ifdef __cplusplus
};
#endif
#endif

