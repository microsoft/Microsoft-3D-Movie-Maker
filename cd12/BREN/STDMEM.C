/*
 * Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
 *
 * $Id: stdmem.c 1.3 1994/11/07 01:39:20 sam Exp $
 * $Locker:  $
 *
 * Default memory handler that uses malloc()/free() from C library
 */
#include <Windows.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "brender.h"

#ifdef DEBUG
static long _cb = 0; // Total memory allocated by BRender
#endif 

/*
 * Glue functions for malloc()/free()
 */
static void * BR_CALLBACK BrStdlibAllocate(br_size_t size, br_uint_8 type)
{
	void *m;
	long cbAlloc = size;

#ifdef DEBUG
	cbAlloc += sizeof(long);
#endif //DEBUG

	m = (void *)GlobalAlloc(GMEM_FIXED, cbAlloc);
	if(m == NULL)
		return NULL;

#ifdef DEBUG
	*(long *)m = size;
	_cb += size;
	m = (char *)m + sizeof(long);
#endif

	return m;
}

static void BR_CALLBACK BrStdlibFree(void *mem)
{
#ifdef DEBUG
	void *pmemReal = (char *)mem - sizeof(long);
	long size = *(long *)pmemReal;
	_cb -= size;
	mem = pmemReal;
#endif //DEBUG
	GlobalFree((HGLOBAL)mem);
}

static br_size_t BR_CALLBACK BrStdlibInquire(br_uint_8 type)
{
	return 0;
}

/*
 * Allocator structure
 */
br_allocator BrStdlibAllocator = {
	"malloc",
	BrStdlibAllocate,
	BrStdlibFree,
	BrStdlibInquire,
};

/*
 * Override global variable s.t. this is the default allocator
 */
br_allocator *_BrDefaultAllocator = &BrStdlibAllocator;

