/*
 * Copyright (c) 1992,1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: g386ifix.h 1.6 1995/02/22 21:37:10 sam Exp $
 * $Locker:  $
 *
 * Inline fixed point maths for GNU-C (386)
 */


/*
 * Old version using inline functions
 */
#if 0
__inline static const long IFixedMul(long a, long b)
{
	register long __r;

	__asm (
		 "imull %2\n"
		 "shrdl $16,%%edx,%%eax\n"
	: "=a" (__r)
	: "0" (a), "rm" (b)
	: "eax", "edx", "cc");

	return __r;
}

__inline static const long IFixedDiv(long a, long b)
{
	register long __r;

	__asm (
		"movl	%%eax,%%edx\n" 
		"shll	$16,%%eax\n"
		"sarl	$16,%%edx\n"
		"idivl	%2\n"
	: "=a" (__r)				/* outputs 	*/
	: "0" (a), "rm" (b)			/* inputs 	*/
	: "eax", "edx", "cc"		/* uses 	*/
	);

	return __r;
}

__inline static const long IFixedDivR(long a, long b)
{
	register long __r;

	__asm (
		"cdq\n"
		"xchg %%edx,%%eax\n"
		"shrdl $16,%%edx,%%eax\n"
		"sarl $16,%%edx\n"
		"idivl %2\n"
	: "=a" (__r)
	: "0" (a), "rm" (b)
	: "eax", "edx", "cc");

	return __r;
}

__inline static const long IFixedDivF(long a, long b)
{
	register long __r;

	__asm (
		"xorl	%%eax,%%eax\n"
		"sarl	$1,%%edx\n"
		"rcrl	$1,%%eax\n"
		"divl	%2\n"
	: "=a" (__r)
	: "d" (a), "rm" (b)
	: "eax", "edx", "cc");

	return __r;
}

__inline static const long IFixedRcp(long a)
{
	register long __r;

	__asm (
		"movl	$1,%%edx\n"
		"xorl	%%eax,%%eax\n"
		"idivl	%1\n"
	: "=a" (__r)
	: "rm" (a)
	: "eax", "edx", "cc");

	return __r;
}

__inline static const long IFixedMulDiv(long a, long b, long c)
{
	register long __r;

	__asm (
		"imull %2\n"
		"idivl %3\n"
	: "=a" (__r)
	: "0" (a), "rm" (b), "rm" (c)
	: "eax", "edx", "cc");

	return __r;
}

__inline static const long IFixedMac2(
	long a, long b,
	long c, long d)
{
	register long __r;

	__asm (
		 "imull %2\n"
		 "movl  %%eax,%%ebx\n"
		 "movl  %%edx,%%ecx\n"

		 "movl  %3,%%eax\n"
		 "imull %4\n"
		 "addl	%%ebx,%%eax\n"
		 "adcl	%%ecx,%%edx\n"
		 "shrdl $16,%%edx,%%eax\n"
	: "=a" (__r)
	: "0" (a), "rm" (b), "rm" (c), "rm" (d)
	: "eax", "ebx", "ecx", "edx", "cc");

	return __r;
}

__inline static const long IFixedMac3(
	long a, long b,
	long c, long d,
	long e, long f)
{
	register long __r;

	__asm (
		 "imull %2\n"
		 "movl  %%eax,%%ebx\n"
		 "movl  %%edx,%%ecx\n"

		 "movl  %3,%%eax\n"
		 "imull %4\n"
		 "addl	%%eax,%%ebx\n"
		 "adcl	%%edx,%%ecx\n"

		 "movl  %5,%%eax\n"
		 "imull %6\n"
		 "addl	%%ebx,%%eax\n"
		 "adcl	%%ecx,%%edx\n"

		 "shrdl $16,%%edx,%%eax\n"
	: "=a" (__r)
	:  "0" (a), "rm" (b), "rm" (c), "rm" (d), "rm" (e), "rm" (f)
	: "eax", "ebx", "ecx", "edx", "cc");

	return __r;
}

__inline static const long IFixedMac4(
	long a, long b,
	long c, long d,
	long e, long f,
	long g, long h)
{
	register long __r;

	__asm (
		 "imull %2\n"
		 "movl  %%eax,%%ebx\n"
		 "movl  %%edx,%%ecx\n"

		 "movl  %3,%%eax\n"
		 "imull %4\n"
		 "addl	%%eax,%%ebx\n"
		 "adcl	%%edx,%%ecx\n"

		 "movl  %5,%%eax\n"
		 "imull %6\n"
		 "addl	%%eax,%%ebx\n"
		 "adcl	%%edx,%%ecx\n"

		 "movl  %7,%%eax\n"
		 "imull %8\n"
		 "addl	%%ebx,%%eax\n"
		 "adcl	%%ecx,%%edx\n"

		 "shrdl $16,%%edx,%%eax\n"
	: "=a" (__r)
	: "0" (a), "rm" (b), "rm" (c), "rm" (d), "rm" (e), "rm" (f), "rm" (g), "rm" (h)
	: "eax", "ebx", "ecx", "edx", "cc");

	return __r;
}

__inline static const long IFixedSqr(long a)
{
	__asm (
		 "imull %%eax\n"
		 "shrdl $16,%%edx,%%eax\n"
	: "=a" (a)
	: "0" (a)
	: "eax", "edx", "cc");

	return a;
}

__inline static const long IFixedSqr2(long a, long b)
{
	register long __r;

	__asm (
		 "imull %%eax\n"
		 "movl  %%eax,%%ebx\n"
		 "movl  %%edx,%%ecx\n"

		 "movl  %2,%%eax\n"
		 "imull %%eax\n"
		 "addl	%%ebx,%%eax\n"
		 "adcl	%%ecx,%%edx\n"
		 "shrdl $16,%%edx,%%eax\n"
	: "=a" (__r)
	: "0" (a), "rm" (b)
	: "eax", "ebx", "ecx", "edx", "cc");

	return __r;
}

__inline static const long IFixedSqr3(
	long a, long b,long c)
{
	register long __r;

	__asm (
		 "imull %%eax\n"
		 "movl  %%eax,%%ebx\n"
		 "movl  %%edx,%%ecx\n"

		 "movl  %2,%%eax\n"
		 "imull %%eax\n"
		 "addl	%%eax,%%ebx\n"
		 "adcl	%%edx,%%ecx\n"

		 "movl  %3,%%eax\n"
		 "imull %%eax\n"
		 "addl	%%ebx,%%eax\n"
		 "adcl	%%ecx,%%edx\n"

		 "shrdl $16,%%edx,%%eax\n"
	: "=a" (__r)
	: "0" (a), "rm" (b), "rm" (c)
	: "eax", "ebx", "ecx", "edx", "cc");

	return __r;
}

__inline static const long IFixedSqr4(
	long a, long b,
	long c, long d)
{
	register long __r;

	__asm (
		 "imull %%eax\n"
		 "movl  %%eax,%%ebx\n"
		 "movl  %%edx,%%ecx\n"

		 "movl  %2,%%eax\n"
		 "imull %%eax\n"
		 "addl	%%eax,%%ebx\n"
		 "adcl	%%edx,%%ecx\n"

		 "movl  %3,%%eax\n"
		 "imull %%eax\n"
		 "addl	%%eax,%%ebx\n"
		 "adcl	%%edx,%%ecx\n"

		 "movl  %4,%%eax\n"
		 "imull %%eax\n"
		 "addl	%%ebx,%%eax\n"
		 "adcl	%%ecx,%%edx\n"

		 "shrdl $16,%%edx,%%eax\n"
	: "=a" (__r)
	: "0" (a), "rm" (b), "rm" (c), "rm" (d)
	: "eax", "ebx", "ecx", "edx", "cc");

	return __r;
}

#define BrFixedMul(a,b)					IFixedMul(a,b)
#define BrFixedMac2(a,b,c,d)	 		IFixedMac2(a,b,c,d)
#define BrFixedMac3(a,b,c,d,e,f)		IFixedMac3(a,b,c,d,e,f)
#define BrFixedMac4(a,b,c,d,e,f,g,h)	IFixedMac4(a,b,c,d,e,f,g,h)

#define BrFixedDiv(a,b)				  	IFixedDiv(a,b)
#define BrFixedDivR(a,b)				IFixedDivR(a,b)
#define BrFixedDivF(a,b)				IFixedDivF(a,b)
#define BrFixedMulDiv(a,b,c)			IFixedMulDiv(a,b,c)
#define BrFixedRcp(a)					IFixedRcp(a)

#define BrFixedSqr(a)					IFixedSqr(a)
#define BrFixedSqr2(a,b)				IFixedSqr2(a,b)
#define BrFixedSqr3(a,b,c)				IFixedSqr3(a,b,c)
#define BrFixedSqr4(a,b,c,d)			IFixedSqr4(a,b,c,d)
#endif

/*
 * New version using direct expansion
 */
#if 0
#define BrFixedMul(__a,__b)							\
({													\
	long __r;										\
													\
	__asm (											\
"		imull %2\n"									\
"		shrdl $16,%%edx,%%eax\n"					\
	: "=a" (__r)									\
	: "0" ((long)(__a)), "rm" ((long)(__b))			\
	: "edx", "cc");									\
													\
	__r;											\
})

#define BrFixedDiv(__a,__b)							\
({													\
	register long __r;								\
													\
	__asm (											\
"		movl	%%eax,%%edx\n" 						\
"		shll	$16,%%eax\n"						\
"		sarl	$16,%%edx\n"						\
"		idivl	%2\n"								\
	: "=a" (__r)									\
	: "a" ((long)(__a)), "rm" ((long)(__b))			\
	: "edx", "cc"									\
	);												\
													\
	__r;											\
})


#define BrFixedMulDiv(__a,__b,__c)					\
({													\
	register long __r;								\
													\
	__asm (											\
		"imull %2\n"								\
		"idivl %3\n"								\
	: "=a" (__r)									\
	: "0" ((long)(__a)), "rm" ((long)(__b)),		\
	  "rm" ((long)(__c))							\
	: "edx", "cc");									\
													\
	__r;											\
})

#define BrFixedMac2(__a,__b,__c,__d)				\
({													\
	register long __r;								\
													\
	__asm (											\
		 "imull %2\n"								\
		 "movl  %%eax,%%ebx\n"						\
		 "movl  %%edx,%%ecx\n"						\
													\
		 "movl  %3,%%eax\n"							\
		 "imull %4\n"								\
		 "addl	%%ebx,%%eax\n"						\
		 "adcl	%%ecx,%%edx\n"						\
		 "shrdl $16,%%edx,%%eax\n"					\
	: "=a" (__r)									\
	: 	"0" ((long)(__a)), "rm" ((long)(__b)),		\
		"rm" ((long)(__c)), "rm" ((long)(__d))		\
	: "ebx", "ecx", "edx", "cc");					\
													\
	__r;											\
})

#define BrFixedMac3(__a,__b,__c,__d,__e,__f)		\
({													\
	register long __r;								\
													\
	__asm (											\
		 "imull %2\n"								\
		 "movl  %%eax,%%ebx\n"						\
		 "movl  %%edx,%%ecx\n"						\
													\
		 "movl  %3,%%eax\n"							\
		 "imull %4\n"								\
		 "addl	%%eax,%%ebx\n"						\
		 "adcl	%%edx,%%ecx\n"						\
													\
		 "movl  %5,%%eax\n"							\
		 "imull %6\n"								\
		 "addl	%%ebx,%%eax\n"						\
		 "adcl	%%ecx,%%edx\n"						\
													\
		 "shrdl $16,%%edx,%%eax\n"					\
	: "=a" (__r)									\
	:	"0" ((long)(__a)), "rm" ((long)(__b)),		\
		"rm" ((long)(__c)), "rm" ((long)(__d)),		\
		"rm" ((long)(__e)), "rm" ((long)(__f)) 		\
	: "ebx", "ecx", "edx", "cc");					\
													\
	__r;											\
})

#define BrFixedMac4(__a,__b,__c,__d,__e,__f,__g,__h)\
({													\
	register long __r;								\
													\
	__asm (											\
		 "imull %2\n"								\
		 "movl  %%eax,%%ebx\n"						\
		 "movl  %%edx,%%ecx\n"						\
													\
		 "movl  %3,%%eax\n"							\
		 "imull %4\n"								\
		 "addl	%%eax,%%ebx\n"						\
		 "adcl	%%edx,%%ecx\n"						\
													\
		 "movl  %5,%%eax\n"							\
		 "imull %6\n"								\
		 "addl	%%eax,%%ebx\n"						\
		 "adcl	%%edx,%%ecx\n"						\
													\
		 "movl  %7,%%eax\n"							\
		 "imull %8\n"								\
		 "addl	%%ebx,%%eax\n"						\
		 "adcl	%%ecx,%%edx\n"						\
													\
		 "shrdl $16,%%edx,%%eax\n"					\
	: "=a" (__r)									\
	:	"0" ((long)(__a)), "rm" ((long)(__b)),		\
		"rm" ((long)(__c)), "rm" ((long)(__d)),		\
		"rm" ((long)(__e)), "rm" ((long)(__f)),		\
		"rm" ((long)(__g)), "rm" ((long)(__h))		\
	: "ebx", "ecx", "edx", "cc");					\
													\
	__r;											\
})
#endif

