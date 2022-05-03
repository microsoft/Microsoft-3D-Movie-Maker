/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: scale.h 1.2 1995/02/22 21:37:23 sam Exp $
 * $Locker:  $
 *
 * Definitons for image scaling
 */

void BR_PUBLIC_ENTRY BrScaleBegin(void);
void BR_PUBLIC_ENTRY BrScaleEnd(void);

/*
 * scale pixelmap src to new_x,new_y with filter width fwidth
 * src must be BR_PMT_RGB_888
 */
br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapScale(br_pixelmap *src,br_uint_32 new_x,br_uint_32 new_y,float fwidth);

