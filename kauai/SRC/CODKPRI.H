/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Private defines for the Kauai codec.

***************************************************************************/
#ifndef CODKPRI_H
#define CODKPRI_H

/***************************************************************************
	Constants for the KCDC encoding. Note: these should be #defines and
	not consts because they are used in inline asm.
***************************************************************************/
#define kcbTailKcdc 6			// number of FF bytes required at the end
#define kcbMaxLenKcdc 4096		// maximum run length
#define kcbitMaxLenKcdc 11		// number of bits used to encode kcbMaxLenKcdc
#define kcbitKcdc0 6			// number of bits used for the offsets...
#define kcbitKcdc1 9
#define kcbitKcdc2 12
#define kcbitKcdc3 20
#define kdibMinKcdc0 0x01		// add to binary offset of corresponding size
#define kdibMinKcdc1 0x41
#define kdibMinKcdc2 0x0241
#define kdibMinKcdc3 0x1241
#define kdibMinKcdc4 0x101240	// used as a lim for 20 bit offsets

/***************************************************************************
	Constants for the KCD2 encoding. Note: these should be #defines and
	not consts because they are used in inline asm.
***************************************************************************/
#define kcbTailKcd2 6			// number of FF bytes required at the end
#define kcbMaxLenKcd2 4096		// maximum run length
#define kcbitMaxLenKcd2 11		// number of bits used to encode kcbMaxLenKcd2
#define kcbitKcd2_0 6			// number of bits used for the offsets...
#define kcbitKcd2_1 9
#define kcbitKcd2_2 12
#define kcbitKcd2_3 20
#define kdibMinKcd2_0 0x01		// add to binary offset of corresponding size
#define kdibMinKcd2_1 0x41
#define kdibMinKcd2_2 0x0241
#define kdibMinKcd2_3 0x1241
#define kdibMinKcd2_4 0x101240	// used as a lim for 20 bit offsets

#endif //!CODKPRI_H


