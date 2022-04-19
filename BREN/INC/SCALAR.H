/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: scalar.h 1.20 1995/05/25 13:37:37 sam Exp $
 * $Locker:  $
 *
 * Scalar type
 *
 * External defines:
 *
 * BASED_FIXED
 *  True if system is based on signed 16.16 fixed point
 *
 * BASED_FLOAT
 *  True if system is based on 32 bit floating point
 *
 * One of these must be defined and the appropriate libraries must be used
 */
#ifndef _SCALAR_H_
#define _SCALAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(BASED_FIXED)
#define BASED_FIXED 0
#endif

#if !defined(BASED_FLOAT)
#define BASED_FLOAT 0
#endif

#if !BASED_FIXED && !BASED_FLOAT
#undef BASED_FLOAT
#define BASED_FLOAT 0
#undef BASED_FIXED
#define BASED_FIXED 1
#endif

/**
 ** Basic types - either fixed or float
 **/

#if BASED_FIXED

/*
 * Reference an external symbol to make sure
 * the right library is linked
 */
extern int _BR_Fixed_Point_Scalar;

	/*
	 * Fixed base types
	 */
typedef br_fixed_ls		br_scalar;
typedef br_fixed_lsf	br_fraction;
typedef br_fixed_luf	br_ufraction;

#define BR_SCALAR_EPSILON	0x0001
#define BR_SCALAR_MAX		0x7fffffff
#define BR_SCALAR_MIN		0x80000000

/*
 * Macros for static initialisation
 */
#define BR_SCALAR(x)		((br_scalar)(BR_ONE_LS * (x)))
#define BR_FRACTION(x)		((br_fraction)((BR_ONE_LSF * (x))>=BR_ONE_LSF?BR_ONE_LSF-1:BR_ONE_LSF * (x)))
#define BR_UFRACTION(x)		((br_ufraction)((BR_ONE_LUF * (x))>=BR_ONE_LUF?BR_ONE_LUF-1:BR_ONE_LUF * (x)))

/*
 * Macros for type conversion
 */
#define BrFloatToScalar(f)	((br_scalar)((f)*(float)BR_ONE_LS))
#define BrScalarToFloat(s)	((s)/(float)BR_ONE_LS)

#define BrIntToScalar(i)	((br_scalar)((i)*(int)BR_ONE_LS))
#define BrScalarToInt(s)	((s)/(int)BR_ONE_LS)

#define BrFixedToScalar(f)		(f)
#define BrScalarToFixed(s)		(s)

#define BrFractionToScalar(f)	((br_scalar)((f)*2))
#define BrScalarToFraction(s)	((br_fraction)((s)/2))

#define BrUFractionToScalar(f)	((br_scalar)(f))
#define BrScalarToUFraction(s)	((br_ufraction)(s))

/*
 * Various arithmetic operations
 */
#define BR_ADD(a,b)				((a)+(b))
#define BR_SUB(a,b)				((a)-(b))
#define BR_MUL(a,b)				BrFixedMul(a,b)
#define BR_SQR(a)				BrFixedSqr(a)

#define BR_ABS(a)				BrFixedAbs(a)

#define BR_DIV(a,b)				BrFixedDiv(a,b)
#define BR_DIVR(a,b)			BrFixedDivR(a,b)
#define BR_MULDIV(a,b,c)		BrFixedMulDiv(a,b,c)
#define BR_RCP(a)				BrFixedRcp(a)

#define BR_CONST_MUL(a,b)		((a)*(b))
#define BR_CONST_DIV(a,b)		((a)/(b))

#define BR_MAC2(a,b,c,d)		BrFixedMac2(a,b,c,d)
#define BR_MAC3(a,b,c,d,e,f)	BrFixedMac3(a,b,c,d,e,f)
#define BR_MAC4(a,b,c,d,e,f,g,h) BrFixedMac4(a,b,c,d,e,f,g,h)

#define BR_MAC2DIV(a,b,c,d,e)		BrFixedMac2Div(a,b,c,d,e)
#define BR_MAC3DIV(a,b,c,d,e,f,g)	BrFixedMac3Div(a,b,c,d,e,f,g)
#define BR_MAC4DIV(a,b,c,d,e,f,g,h,i) BrFixedMac4Div(a,b,c,d,e,f,g,h,i)

#define BR_SQR2(a,b)			BrFixedSqr2(a,b)
#define BR_SQR3(a,b,c)			BrFixedSqr3(a,b,c)
#define BR_SQR4(a,b,c,d)		BrFixedSqr4(a,b,c,d)

#define BR_FMAC2(a,b,c,d)		BrFixedFMac2(a,b,c,d)
#define BR_FMAC3(a,b,c,d,e,f)	  BrFixedFMac3(a,b,c,d,e,f)
#define BR_FMAC4(a,b,c,d,e,f,g,h) BrFixedFMac4(a,b,c,d,e,f,g,h)

#define BR_LENGTH2(a,b)			BrFixedLength2(a,b)
#define BR_LENGTH3(a,b,c)		BrFixedLength3(a,b,c)
#define BR_LENGTH4(a,b,c,d)		BrFixedLength4(a,b,c,d)

#define BR_RLENGTH2(a,b)		BrFixedRLength2(a,b)
#define BR_RLENGTH3(a,b,c)		BrFixedRLength3(a,b,c)
#define BR_RLENGTH4(a,b,c,d)	BrFixedRLength4(a,b,c,d)

#define BR_POW(a,b)				BrFixedPow((a),(b))
#define BR_SQRT(a)				BrFixedSqrt(a)

#endif

#if BASED_FLOAT

/*
 * Reference an external symbol to make sure
 * the right library is linked
 */
extern int _BR_Floating_Point_Scalar;

	/*
	 * Floating point base types
	 */
typedef float br_scalar;
typedef float br_fraction;
typedef float br_ufraction;

#define BR_SCALAR_EPSILON	1.192092896e-7f
#define BR_SCALAR_MAX		3.402823466e+38f
#define BR_SCALAR_MIN		(-3.402823466e+38f)

/*
 * Macros for static initialisation
 */
#define BR_SCALAR(x)		((br_scalar)(x))
#define BR_FRACTION(x)		((br_fraction)(x))
#define BR_UFRACTION(x)		((br_ufraction)(x))

/*
 * Macros for type conversion
 */
#define BrFloatToScalar(f)	(f)
#define BrScalarToFloat(c)	(c)

#define BrIntToScalar(i)	((br_scalar)(i))
#define BrScalarToInt(s)	((int)(s))

#define BrFixedToScalar(f)	((br_scalar)((f)/(float)BR_ONE_LS))
#define BrScalarToFixed(s)	(br_fixed_ls)((s)*(br_scalar)BR_ONE_LS)

#define BrFractionToScalar(f)	(f)
#define BrScalarToFraction(s)	(s)

#define BrUFractionToScalar(f)	(f)
#define BrScalarToUFraction(s)	(s)

#ifndef _NO_PROTOTYPES
#if defined(_MSC_VER) && !defined(__H2INC__)
#include <math.h>
#elif defined (__IBMC__)
#include <math.h>
#else
double fabs(double);
double fabs(double);
double sqrt(double);
double cos(double);
double sin(double);
double acos(double);
double asin(double);
double atan2(double,double);
#endif
#endif

/*
 * Various arithmetic operations
 */
#define BR_ADD(a,b)				((a)+(b))
#define BR_SUB(a,b)				((a)-(b))
#define BR_MUL(a,b)				((a)*(b))
#define BR_SQR(a)				((a)*(a))

#define BR_ABS(a)				fabs(a)

#define BR_DIV(a,b)				((a)/(b))
#define BR_DIVR(a,b)			((a)/(b))
#define BR_MULDIV(a,b,c)		((a)*(b)/(c))
#define BR_RCP(a)				(1.0/(a))

#define BR_CONST_MUL(a,b)		((a)*(b))
#define BR_CONST_DIV(a,b)		((a)/(b))


#define BR_MAC2(a,b,c,d)		((a)*(b)+(c)*(d))
#define BR_MAC3(a,b,c,d,e,f)	((a)*(b)+(c)*(d)+(e)*(f))
#define BR_MAC4(a,b,c,d,e,f,g,h) ((a)*(b)+(c)*(d)+(e)*(f)+(g)*(h))

#define BR_MAC2DIV(a,b,c,d,e)		(((a)*(b)+(c)*(d))/e)
#define BR_MAC3DIV(a,b,c,d,e,f,g)	(((a)*(b)+(c)*(d)+(e)*(f))/g)
#define BR_MAC4DIV(a,b,c,d,e,f,g,h,i) (((a)*(b)+(c)*(d)+(e)*(f)+(g)*(h))/i)

#define BR_SQR2(a,b)			((a)*(a)+(b)*(b))
#define BR_SQR3(a,b,c)			((a)*(a)+(b)*(b)+(c)*(c))
#define BR_SQR4(a,b,c,d)		((a)*(a)+(b)*(b)+(c)*(c)+(d)*(d))

#define BR_FMAC2(a,b,c,d)		((a)*(b)+(c)*(d))
#define BR_FMAC3(a,b,c,d,e,f)	((a)*(b)+(c)*(d)+(e)*(f))
#define BR_FMAC4(a,b,c,d,e,f,g,h) ((a)*(b)+(c)*(d)+(e)*(f)+(g)*(h))

#define BR_LENGTH2(a,b)			sqrt((a)*(a)+(b)*(b))
#define BR_LENGTH3(a,b,c)		sqrt((a)*(a)+(b)*(b)+(c)*(c))
#define BR_LENGTH4(a,b,c,d)		sqrt((a)*(a)+(b)*(b)+(c)*(c)+(d)*(d))

#define BR_RLENGTH2(a,b)		(1.0/sqrt((a)*(a)+(b)*(b)))
#define BR_RLENGTH3(a,b,c)		(1.0/sqrt((a)*(a)+(b)*(b)+(c)*(c)))
#define BR_RLENGTH4(a,b,c,d)	(1.0/sqrt((a)*(a)+(b)*(b)+(c)*(c)+(d)*(d)))

#define BR_POW(a,b)				pow((a),(b))
#define BR_SQRT(a)				sqrt(a)

#endif

/*
 * Make sure PI is defined
 */
#ifndef PI
#define PI		3.14159265358979323846
#endif

#ifdef __cplusplus
};
#endif
#endif

