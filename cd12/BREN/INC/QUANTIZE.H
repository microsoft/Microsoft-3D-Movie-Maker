/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: quantize.h 1.2 1995/02/22 21:37:21 sam Exp $
 * $Locker:  $
 *
 * Definitons for image quantizing
 */

void BR_PUBLIC_ENTRY BrQuantBegin(void);
void BR_PUBLIC_ENTRY BrQuantEnd(void);

/*
 * add array of <size> rgb values to quantizer
 */
void BR_PUBLIC_ENTRY BrQuantAddColours(br_uint_8 *colours, br_uint_32 size);

/*
 * make optimum palette
 */
void BR_PUBLIC_ENTRY BrQuantMakePalette(int base,int num_entries,br_pixelmap *palette);

/*
 * given palette with base and num_entries, setup internal map
 */
void BR_PUBLIC_ENTRY BrQuantPrepareMapping(int base,int num_entries,br_pixelmap *palette);

/*
 * get <size> index values for array of <size> rgb values
 */
void BR_PUBLIC_ENTRY BrQuantMapColours(int base,br_uint_8 *colours, br_uint_8 *mapped_colours, int size);
