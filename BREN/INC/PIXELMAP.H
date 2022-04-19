/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pixelmap.h 1.15 1995/06/30 16:08:58 sam Exp $
 * $Locker: sam $
 *
 * A stopgap 2D pixelmap structure for brender. This should really be the
 * pixelmap data type from the underlying 2D system (whatever that will
 * be)
 *
 * Used for input (maps) and output (render buffer)
 */
#ifndef _PIXELMAP_H_
#define _PIXELMAP_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Various types of pixel
 */
enum {
	/*
	 * Each pixel is an index into a colour map
	 */
	BR_PMT_INDEX_1,
	BR_PMT_INDEX_2,
	BR_PMT_INDEX_4,
	BR_PMT_INDEX_8,

	/*
	 * True colour RGB
	 */
	BR_PMT_RGB_555,		/* 16 bits per pixel */
	BR_PMT_RGB_565,		/* 16 bits per pixel */
	BR_PMT_RGB_888,		/* 24 bits per pixel */
	BR_PMT_RGBX_888,	/* 32 bits per pixel */
	BR_PMT_RGBA_8888,	/* 32 bits per pixel */

	/*
	 * YUV
	 */
	BR_PMT_YUYV_8888,	/* YU YV YU YV ... */
	BR_PMT_YUV_888,

	/*
	 * Depth
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_DEPTH_32,

	/*
	 * Opacity
	 */
	BR_PMT_ALPHA_8,

	/*
	 * Opacity + Index
	 */
	BR_PMT_INDEXA_88,

	/*
	 * Bump maps
	 */
	BR_PMT_NORMAL_INDEX_8,
	BR_PMT_NORMAL_XYZ,

	/*
	 * Wrong way around 15 bit true colour
	 */
	BR_PMT_BGR_555,
};

/*
 * pixelmap flags
 */
enum {
		/*
		 * No direct access to pixels
		 */
	BR_PMF_NO_ACCESS		= 0x01,

	BR_PMF_LINEAR			= 0x02,
	BR_PMF_ROW_WHOLEPIXELS	= 0x04
};


typedef struct br_pixelmap {
	/*
	 * Optional identifier (when maps used as textures/tables etc.)
	 */
	char *identifier;

	/*
	 * pointer to raw pixel data
	 */
	void * pixels;

	br_uint_32 _reserved0;

	/*
	 * Optional pixel map when pixels are indexed.
	 */
	struct br_pixelmap *map;

	/*
	 * Byte difference between pixels at same column of adjacent rows
	 */
	br_int_16	row_bytes;

	/*
	 * Type of pixels
	 */
	br_uint_8	type;

	/*
	 * Flags
	 */
	br_uint_8	flags;
	
	/*
	 * top left visible region in pixels from pixel at 'pixel' pointer
	 */
	br_uint_16	base_x;
	br_uint_16	base_y;
	
	/*
	 * Width and height of bitmap in pixels
	 */
	br_uint_16	width;
	br_uint_16	height;

	/*
	 * Local origin for any graphics system rendering into map, relative
	 * to 'base'
	 */
	br_int_16	origin_x;
	br_int_16	origin_y;

	/*
	 * Device pointer - if pixelmap originated from a device
	 */
	void	*context;

} br_pixelmap;

/*
 * Flags to BrPixelMapAllocate
 */
enum br_pixelmap_allocate_flags {
	BR_PMAF_INVERTED = 0x0001,		/* Setup pixelmap so that 0th scanline is at high memory */
	BR_PMAF_NORMAL   = 0x0000		/* Setup pixelmap so that 0th scanline is at low memory */
};

/*
 * Channel flags
 */
enum br_pixelmap_channel_mask {
	BR_PMCHAN_INDEX	= 0x0001,
	BR_PMCHAN_RGB 	= 0x0002,
	BR_PMCHAN_DEPTH = 0x0004,
	BR_PMCHAN_ALPHA = 0x0008,
	BR_PMCHAN_YUV 	= 0x0010
};

/*
 * Matching pixelmap types
 */
enum br_pmmatch_type {
	BR_PMMATCH_OFFSCREEN,
	BR_PMMATCH_DEPTH_16,

	BR_PMMATCH_MAX
};

#ifdef __cplusplus
};
#endif
#endif

