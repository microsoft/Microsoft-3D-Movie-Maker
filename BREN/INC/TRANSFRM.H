/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: transfrm.h 1.11 1995/02/22 21:37:24 sam Exp $
 * $Locker:  $
 *
 * Structure describing an affine transform from one coordinate space
 * to another
 */
#ifndef _TRANSFRM_H_
#define _TRANSFRM_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Type of actor position
 */
enum {
	BR_TRANSFORM_MATRIX34,
	BR_TRANSFORM_MATRIX34_LP,
	BR_TRANSFORM_QUAT,
	BR_TRANSFORM_EULER,
	BR_TRANSFORM_LOOK_UP,
	BR_TRANSFORM_TRANSLATION,
	BR_TRANSFORM_IDENTITY,
	BR_TRANSFORM_MAX
};

typedef struct br_transform {
						   
	/*
	 * Type of position
	 */
	br_uint_16 type;

	/*
	 * Union of the various means of describing a transform -
	 * these are explicity arrranged so that any exlicit transform
	 * will always be available as br_transform.t.translate
	 */
	union {
		/*
		 * Affine 3x4 matrix
		 */
		br_matrix34 mat;

		/*
		 * Euler angles and translation
		 */
		struct {
			br_euler e;
			br_scalar _pad[7];
			br_vector3 t;
		} euler;

		/*
		 * Unit quaternion and translation
		 */
		struct {
			br_quat q;
			br_scalar _pad[5];
			br_vector3 t;
		} quat;

		/*
		 * Lookat vector, up vector and translation
		 */
		struct {
			br_vector3 look;
			br_vector3 up;
			br_scalar _pad[3];
			br_vector3 t;
		} look_up;

		/*
		 * Just a translation
		 */
		struct {
			br_scalar _pad[9];
			br_vector3 t;
		} translate;
	} t;
} br_transform;

#ifdef __cplusplus
};
#endif
#endif

