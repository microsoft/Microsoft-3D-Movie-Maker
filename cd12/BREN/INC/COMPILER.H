/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: compiler.h 1.20 1995/05/25 13:37:28 sam Exp $
 * $Locker: sam $
 *
 * Misc host compiler configuration (types & special declarators etc.)
 */
#ifndef _COMPILER_H_
#define _COMPILER_H_

#ifdef __cplusplus
	extern "C" {
#endif

/*
 * Fixed bitsize integers
 */
typedef signed long br_int_32;
typedef unsigned long br_uint_32;

typedef signed short br_int_16;
typedef unsigned short br_uint_16;

typedef signed char br_int_8;
typedef unsigned char br_uint_8;

/*
 * Generic size type (in case target environment does not have size_t)
 */
typedef unsigned int br_size_t;

/*
 * Boolean type
 */
typedef int br_boolean;

#define BR_TRUE		1
#define BR_FALSE	0

#define BR_BOOLEAN(a)	((a) != 0)

/**
 ** Compiler specific declarations
 **
 *
 * BR_PUBLIC_ENTRY are the declarators for public entry points into the library
 *
 * BR_CALLBACK are the declarators for function pointers to callbacks
 *
 * BR_ASM_DATA is the declarator for data that is shared with ASM files
 *
 * BR_ASM_CALL is the declarator for function declarations that are ASM functions
 * called from C
 *
 * BR_ASM_CALLBACK is the declarator for function definitions that are C functions
 * called from ASM
 *
 * BR_SUFFIX_HOST is a string used for banners/titles etc
 *
 * br_framebuffer_ptr is a type used for pointing at the pixels of a frame buffer
 *
 * BR_HAS_FAR is !0 if language supports __far pointers
 */

/*
 * WATCOM C/C++ 32
 */
#if defined(__WATCOMC__)

/*
 * Use the current default calling convention
 */
#if defined(__TARGET_MSC__)

#define stricmp _stricmp
#define strnicmp _strnicmp
#define memccpy _memccpy

#pragma aux __cdecl "_*" parm caller [] modify [eax ecx edx] ;

#define BR_PUBLIC_ENTRY __cdecl
#define BR_CALLBACK __cdecl
#else
#define BR_PUBLIC_ENTRY
#define BR_CALLBACK
#endif

#ifdef __cplusplus
#define BR_ASM_DATA
#define BR_ASM_DATA_EXTRA(x)
#else
#define BR_ASM_DATA __cdecl
#endif

#define BR_ASM_CALL __cdecl
#define BR_ASM_CALLBACK __cdecl

#define BR_SUFFIX_HOST "-WTC"

#define BR_HAS_FAR	1
/*
 * Stop unreferenced variables producing a warning
 * Things like "rcsid" and unused fucntion arguments
 */
#pragma off (unreferenced);

/*
 * Zortech C++
 */
#elif defined(__ZTC__)
#define BR_PUBLIC_ENTRY __cdecl
#define BR_CALLBACK __cdecl

#define BR_ASM_DATA __cdecl
#define BR_ASM_CALL __cdecl
#define BR_ASM_CALLBACK __cdecl

#define BR_SUFFIX_HOST "-ZTC"

#define BR_HAS_FAR	1

/*
 * GNU C
 */
#elif defined (__GNUC__)

#define BR_PUBLIC_ENTRY
#define BR_CALLBACK

#define BR_ASM_DATA
#define BR_ASM_CALL
#define BR_ASM_CALLBACK

#define BR_SUFFIX_HOST "-GCC"

#define BR_HAS_FAR	0

/*
 * Apple MPW C
 */
#elif defined (__MPW__)
#define BR_PUBLIC_ENTRY
#define BR_CALLBACK

#define BR_ASM_DATA
#define BR_ASM_CALL
#define BR_ASM_CALLBACK

#define BR_SUFFIX_HOST "-MPW"

#define BR_HAS_FAR	0

/*
 * Intel Proton
 */
#elif defined (__PROTONC__)
#define BR_PUBLIC_ENTRY __cdecl
#define BR_CALLBACK __cdecl

#define BR_ASM_DATA __cdecl
#define BR_ASM_CALL __cdecl
#define BR_ASM_CALLBACK __cdecl

#define BR_SUFFIX_HOST "-PROTON"

#define BR_HAS_FAR	1

/*
 * Microsoft Visual C++
 */
#elif defined (_MSC_VER)
#define BR_PUBLIC_ENTRY __cdecl
#define BR_CALLBACK __cdecl

#define BR_ASM_DATA __cdecl
#define BR_ASM_CALL __cdecl
#define BR_ASM_CALLBACK __cdecl

#define BR_SUFFIX_HOST "-VISUALC"

#define BR_HAS_FAR	0

/*
 * Metaware High-C Version 1
 */
#elif defined (__HIGHC_V1__)

#pragma On(Align_members)

#define BR_PUBLIC_ENTRY
#define BR_CALLBACK

#define BR_ASM_DATA
#define BR_ASM_CALL
#define BR_ASM_CALLBACK

#define BR_SUFFIX_HOST "-HIGHC1"

#define BR_HAS_FAR	0

#define stricmp _stricmp

/*
 * Metaware High-C Version 3
 */
#elif defined (__HIGHC__)

#pragma Align_members(4)

#define BR_PUBLIC_ENTRY
#define BR_CALLBACK

#define BR_ASM_DATA
#define BR_ASM_CALL
#define BR_ASM_CALLBACK

#define BR_SUFFIX_HOST "-HIGHC3"

#define BR_HAS_FAR	0

#define stricmp _stricmp

/*
 * Borland BC 4
 */
#elif defined (__BORLANDC__)
#define BR_PUBLIC_ENTRY
#define BR_CALLBACK

#define BR_ASM_DATA __cdecl
#define BR_ASM_CALL __cdecl
#define BR_ASM_CALLBACK __cdecl

#define BR_SUFFIX_HOST "-BORLAND"

#define BR_HAS_FAR	0

/*
 * IBM CSet++
 */
#elif defined (__IBMC__)
#define BR_PUBLIC_ENTRY _System
#define BR_CALLBACK _System

#define BR_ASM_DATA
#define BR_ASM_CALL _System
#define BR_ASM_CALLBACK _System

#define BR_SUFFIX_HOST "-CSET"

#define BR_HAS_FAR	0

#endif


#if defined (__H2INC__)
/*
 * Avoid some tokens that masm chokes on
 */
#define type _type
#define a _a
#define b _b
#define c _c
#define width _width
#define end _end

/*
 * Supress compiler specific declarators
 */
#undef BR_CALLBACK
#undef BR_ASM_DATA
#undef BR_ASM_CALL
#undef BR_ASM_CALLBACK

#define BR_CALLBACK
#define BR_ASM_DATA
#define BR_ASM_CALL
#define BR_ASM_CALLBACK

#endif

#if DEBUG
#	define BR_SUFFIX_DEBUG "-DEBUG"
#else
#	define BR_SUFFIX_DEBUG ""
#endif

/*
 * Macros for producing banners & copyright messages
 */
#define BR_BANNER(title,year,revision)do {\
	static char _revision[] = revision;\
	fprintf(stderr,title);\
	fwrite(_revision+10,1,sizeof(_revision)-12,stderr);\
	fprintf(stderr,"Copyright (C) " year " by Argonaut Technologies Limited\n");\
} while(0);

/*
 * Useful macro for sizing an array
 */
#define BR_ASIZE(a) (sizeof(a)/sizeof((a)[0]))

#ifdef __cplusplus
	};
#endif
#endif


