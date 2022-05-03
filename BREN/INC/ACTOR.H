/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: actor.h 1.14 1995/05/25 13:37:23 sam Exp $
 * $Locker:  $
 *
 * Definitons for an Actor
 */
#ifndef _ACTOR_H_
#define _ACTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 ** Definition of base actor structure
 **/

/*
 * Basic types of actor
 */
enum {
	BR_ACTOR_NONE,
	BR_ACTOR_MODEL,
	BR_ACTOR_LIGHT,
	BR_ACTOR_CAMERA,
	_BR_ACTOR_RESERVED,
	BR_ACTOR_BOUNDS,
	BR_ACTOR_BOUNDS_CORRECT,
	BR_ACTOR_CLIP_PLANE,
	BR_ACTOR_MAX
};

/*
 * Render styles - an actor inherits it's style from the most _distant_
 * ancestor included in this traversal that does not have default set
 * (unlike model & material which are inherited from the nearest ancestor)
 */
enum {
	BR_RSTYLE_DEFAULT,
	BR_RSTYLE_NONE,
	BR_RSTYLE_POINTS,
	BR_RSTYLE_EDGES,
	BR_RSTYLE_FACES,
	BR_RSTYLE_BOUNDING_POINTS,
	BR_RSTYLE_BOUNDING_EDGES,
	BR_RSTYLE_BOUNDING_FACES,
	BR_RSTYLE_MAX
};

/*
 * The actor structure
 */
typedef struct br_actor {
	/*
	 * Doubly linked list of siblings
	 * br_actor can be used as br_simple_node
	 */
	struct br_actor *next;
	struct br_actor **prev;

	/*
	 * First in list of children, or NULL if none
	 * br_actor.children can be used as br_simple_list
	 */
	struct br_actor *children;

	/*
	 * Parent, or NULL, if no higher parent
	 */
	struct br_actor *parent;

	/*
	 * Depth of actor from furthest ancestor
	 */
	br_uint_16 depth;

	/*
	 * Type of actor
	 */
	br_uint_8 type;

	/*
	 * Optional name of actor
	 */
	char *identifier;

	/*
	 * Reference to model associated with this actor, NULL will inherit
	 * from parent (root inherits default_model)
	 */
	br_model *model;

	/*
	 * Default material for model - NULL will inherit from parent. Root
	 * inherits default material
	 */
	br_material *material;

	/*
	 * Render style for actor, and all it's decendants if not BR_RSTYLE_DEFAULT
	 */
	br_uint_8 render_style;

	/*
	 * Postiton of this actor within parent space
	 */
	br_transform t;

	/*
	 * Reference to any type specific data block
	 */
	void *type_data;
} br_actor;

#ifdef __cplusplus
};
#endif
#endif

