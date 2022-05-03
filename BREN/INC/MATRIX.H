/*
 * Copyright (c) 1992,1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: matrix.h 1.3 1995/02/22 21:37:13 sam Exp $
 * $Locker:  $
 *
 * Structrures describing matrices
 */
#ifndef _MATRIX_H_
#define _MATRIX_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct br_matrix4 {
	br_scalar m[4][4];
} br_matrix4;

typedef struct br_matrix34 {
	br_scalar m[4][3];
} br_matrix34;

typedef struct br_matrix23 {
	br_scalar m[3][2];
} br_matrix23;

#ifdef __cplusplus
};
#endif
#endif

