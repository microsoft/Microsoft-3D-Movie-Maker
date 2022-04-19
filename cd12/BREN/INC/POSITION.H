/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: position.h 1.3 1995/02/22 21:37:19 sam Exp $
 * $Locker:  $
 *
 * Structure descibing a position in 3D space - used for actors
 */
#ifndef _POSITION_H_
#define _POSITION_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Type of actor position
 */
enum p3d_actor_position_type {
	P3D_POSITION_IDENTITY,
	P3D_POSITION_TRANSFORM,
	P3D_POSITION_EULER,
	P3D_POSITION_QUATERNION,
	P3D_POSITION_TRANSLATION,
	P3D_POSITION_LOOK_UP
};

struct p3d_position {
						   
	/*
	 * Type of position
	 */
	uint_16 t;

	/*
	 * Union of the various means of describing a position
	 */
	union {
		/*
		 * General matrix - (XXX Should also have affine (3x4))
		 */
		struct p3d_transform transform;

		/*
		 * Euler angles and translation
		 */
		struct {
			struct p3d_euler e;
			point3 t;
		} euler;

		/*
		 * Unit quaternion and translation
		 */
		struct {
			struct p3d_quaternion q;
			point3 t;
		} quaternion;

		/*
		 * Lookat vector, up vector and translation
		 */
		struct {
			vector3 look;
			vector3 up;
			point3 t;
		} look;

		/*
		 * Just a translation
		 */
		struct {
			point3 t;
		} translation;
	} p;
};

#ifdef __cplusplus
};
#endif
#endif

