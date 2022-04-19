/*
 * Copyright (c) 1992,1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brfont.h 1.4 1995/03/29 16:42:55 sam Exp $
 * $Locker:  $
 *
 */
#ifndef _BRFONT_H_
#define _BRFONT_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct br_font {
		br_uint_32 flags;
		br_uint_16 glyph_x;
		br_uint_16 glyph_y;
		br_int_16 spacing_x;
		br_int_16 spacing_y;
		br_int_8 *width;
		br_uint_16 *encoding;
		br_uint_8 *glyphs;
} br_font;

/*
 * Flags
 */
#define BR_FONTF_PROPORTIONAL	1

/*
 * Default fonts that are available in framework
 */
extern struct br_font * BrFontFixed3x5;
extern struct br_font * BrFontProp4x6;
extern struct br_font * BrFontProp7x9;

#ifdef __cplusplus
};
#endif
#endif

