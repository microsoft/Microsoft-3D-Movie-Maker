/*
 * Copyright (c) 1992,1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: fixed.h 1.17 1995/02/22 21:37:06 sam Exp $
 * $Locker:  $
 */
#ifndef _FIXED_H_
#define _FIXED_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INLINE_FIXED
#define INLINE_FIXED 1
#endif

/*
 * Fixed point types
 */
typedef long br_fixed_ls;			/* Long Signed fixed			15.16 */
typedef short br_fixed_lsf;			/* Long Signed fraction			 0.15 */

typedef unsigned long br_fixed_lu;	/* Long unsigned fixed			16.16 */
typedef unsigned short br_fixed_luf;/* Long unsigned fixed fraction  0.16 */

typedef short br_fixed_ss;			/* Short Signed fixed			 7.8  */

typedef char br_fixed_ssf;			/* Short Signed fraction		 0.7  */

typedef unsigned short br_fixed_su;	/* Short unsigned fixed			 8.8  */
typedef unsigned char br_fixed_suf;	/* Short unsigned fixed fraction 0.8  */

/*
 * 1 in various fixed point forms
 */
#define BR_ONE_LS 	(1<<16)
#define BR_ONE_LSF  (1<<15)
#define BR_ONE_LU 	(1<<16)
#define BR_ONE_LUF  (1<<16)

#define BR_ONE_SS 	(1<<8)
#define BR_ONE_SSF  (1<<7)
#define BR_ONE_SU 	(1<<8)
#define BR_ONE_SUF  (1<<8) 

#define BrIntToFixed(i) 	((i) << 16)
#define BrFixedToInt(i) 	((i) >> 16)
#define BrFloatToFixed(f)	((br_scalar)((f)*(float)BR_ONE_LS))
#define BrFixedToFloat(s)	((s)*(1.0/(float)BR_ONE_LS))


#ifdef __cplusplus
};
#endif
#endif

