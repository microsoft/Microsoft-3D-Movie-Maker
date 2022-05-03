/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: custsupt.h 1.2 1995/02/22 21:37:05 sam Exp $
 * $Locker:  $
 *
 * Definitions for support routines available during render callbacks
 *
 */
#ifndef _CUSTSUPT_H_
#define _CUSTSUPT_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Outcode bits
 */
#define OUTCODE_LEFT		0x00000001
#define OUTCODE_RIGHT		0x00000002
#define OUTCODE_TOP			0x00000004
#define OUTCODE_BOTTOM		0x00000008
#define OUTCODE_HITHER		0x00000010
#define OUTCODE_YON			0x00000020

#define OUTCODE_USER		0x00000040
#define OUTCODE_USER_ALL	0x00000FC0

#define OUTCODES_ALL		0x00000FFF

#define OUTCODE_N_LEFT		0x00010000
#define OUTCODE_N_RIGHT		0x00020000
#define OUTCODE_N_TOP		0x00040000
#define OUTCODE_N_BOTTOM	0x00080000
#define OUTCODE_N_HITHER	0x00100000
#define OUTCODE_N_YON		0x00200000
#define OUTCODE_N_USER		0x00400000
#define OUTCODE_N_USER_ALL	0x0FC00000

#define OUTCODES_NOT		0x0FFF0000


/*
 * Values for on screen test
 */
enum {
	OSC_REJECT,
	OSC_PARTIAL,
	OSC_ACCEPT
};

#ifdef __cplusplus
};
#endif
#endif


