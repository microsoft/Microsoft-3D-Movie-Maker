/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brmem.h 1.13 1995/05/25 13:37:26 sam Exp $
 * $Locker: sam $
 *
 * Brender's interface to memory allocation
 */

#ifndef _BRMEM_H_
#define _BRMEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Instance of a memory allocator
 */
typedef void * BR_CALLBACK brmem_allocate_cbfn(br_size_t size, br_uint_8 type);
typedef void BR_CALLBACK brmem_free_cbfn(void *block);
typedef	br_size_t BR_CALLBACK brmem_inquire_cbfn(br_uint_8 type);

typedef struct br_allocator {
	char *identifier;

	/*
	 * Allocate a new chunk of memory - must return the new pointer
	 * or handle the error itself
	 */
	brmem_allocate_cbfn *allocate;

	/*
	 * Release previously allocated block
	 */
	brmem_free_cbfn *free;

	/*
	 * Inquire as to the amount of memeory available for a given type
	 */
	brmem_inquire_cbfn *inquire;

} br_allocator;

/*
 * Classes of resource that brender allocates
 *
 * Valid values are 1 to 255
 */
enum br_memory_classes {
	/*
	 * System classes
	 */
	BR_MEMORY_SCRATCH=1,
	BR_MEMORY_PIXELMAP,
	BR_MEMORY_PIXELS,
	BR_MEMORY_VERTICES,
	BR_MEMORY_FACES,
	BR_MEMORY_GROUPS,
	BR_MEMORY_MODEL,
	BR_MEMORY_MATERIAL,
	BR_MEMORY_MATERIAL_INDEX,
	BR_MEMORY_ACTOR,
	BR_MEMORY_PREPARED_VERTICES,
	BR_MEMORY_PREPARED_FACES,
	BR_MEMORY_LIGHT,
	BR_MEMORY_CAMERA,
	BR_MEMORY_BOUNDS,
	BR_MEMORY_CLIP_PLANE,
	BR_MEMORY_STRING,
	BR_MEMORY_REGISTRY,
	BR_MEMORY_TRANSFORM,
	BR_MEMORY_RESOURCE_CLASS,
	BR_MEMORY_FILE,
	BR_MEMORY_ANCHOR,
	BR_MEMORY_POOL,
	BR_MEMORY_RENDER_MATERIAL,
	BR_MEMORY_DATAFILE,
	BR_MEMORY_IMAGE,
	BR_MEMORY_IMAGE_ARENA,
	BR_MEMORY_IMAGE_SECTIONS,
	BR_MEMORY_IMAGE_NAMES,
	BR_MEMORY_EXCEPTION_HANDLER,

	/*
	 * Application classes
	 */
	BR_MEMORY_APPLICATION = 0x80,

	/*
	 * User defined classed are BR_MEMORY_APPLICATION + 1 ... 127
	 */
	BR_MEMORY_MAX = 256
};

/*
 * A resource class structure
 */
typedef void BR_CALLBACK br_resourcefree_cbfn(void *res, br_uint_8 res_class, br_size_t size);

typedef struct br_resource_class {
	char *identifier;
	br_uint_8 res_class;
	br_resourcefree_cbfn *free_cb;
} br_resource_class;

#ifdef __cplusplus
};
#endif
#endif

