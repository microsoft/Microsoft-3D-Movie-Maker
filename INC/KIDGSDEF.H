/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

//
// KIDGSDEF.H  --- Cool defines for commonly used
// information between code and script.
//
// File is named kidgs, because it's a Glen-Sean
// wrapper around kidspace to make scripting a bit
// easier :-)
//


#ifndef KIDGSDEF_H
#define KIDGSDEF_H

#include "kiddef.h"
#include "framedef.h"
#include "kidsanim.h"


// ---------------------------------------------------------
// the default transparent color index for bitmaps and masks
// ---------------------------------------------------------
#define kiTrans                                 0x00

// ---------------------------------------------------------
// cno of script to run when the application is started
// ---------------------------------------------------------
#define kcnoStartApp                    0

// ---------------------------------------------------------
// The following are child id constants
// ---------------------------------------------------------

// predefined script child id's (utility values)
#define kchidScript0                    0x0020
#define kchidScript1                    0x0021
#define kchidScript2                    0x0022
#define kchidScript3                    0x0023
#define kchidScript4                    0x0024
#define kchidScript5                    0x0025
#define kchidScript6                    0x0026
#define kchidScript7                    0x0027
#define kchidScript8                    0x0028
#define kchidScript9                    0x0029
#define kchidScript10                   0x002A
#define kchidScript11                   0x002B


// ---------------------------------------------------------
// The following are constants for the state portion of the
// above.  State 0 is an undefined state.
// ---------------------------------------------------------
#define kstNil                                  0x0000
#define kst1                                    0x0001
#define kst2                                    0x0002
#define kst3                                    0x0003
#define kst4                                    0x0004
#define kst5                                    0x0005
#define kst6                                    0x0006
#define kst7                                    0x0007
#define kst8                                    0x0008
#define kst9                                    0x0009
#define kst10                                   0x000A
#define kst11                                   0x000b
#define kst12                                   0x000c
#define kst13                                   0x000d
#define kst14                                   0x000e
#define kst15                                   0x000f
#define kst16                                   0x0010
#define kst17                                   0x0011
#define kst18                                   0x0012
#define kst19                                   0x0013
#define kst20                                   0x0014
#define kst21                                   0x0015
#define kst22                                   0x0016
#define kst23                                   0x0017
#define kst24                                   0x0018
#define kst25                                   0x0019
#define kst26                                   0x001a
#define kst27                                   0x001b
#define kst28                                   0x001c
#define kst29                                   0x001d
#define kst30                                   0x001e
#define kst31                                   0x001f
#define kst32                                   0x0020

// Nil runtime Gob ID
#define kidNil                                  0

#endif	// !KIDGSDEF_H
