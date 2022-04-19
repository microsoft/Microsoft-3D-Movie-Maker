/*****************************************************************************
*
*  BUILDGOB.H
*
*  Copyright (C) Microsoft Corporation 1995.
*  All Rights reserved.
*
******************************************************************************
*
*  Module Intent
*
*  Contains all of the kids for the Building place gobs.
*
******************************************************************************
*
*  Revision History:  Pulled out of soc\src\building\socdefn.h 06/08/95.
*
*****************************************************************************/

#ifndef BUILDGOB_H
#define BUILDGOB_H

/*****************************************************************************
*                                                                            *
*                              Constants                                     *
*                                                                            *
*****************************************************************************/

// -----------------------------------------------------
// Building Location Definitions
// -----------------------------------------------------

#define kgobLogin				0x00010200
#define kgobCloset 			0x00010280
#define kgobImagin			0x00010300
#define kgobStreet			0x00010400

#define kgobTicket1			0x00010500		// Far away view of ticket booth
#define kgobTicket2			0x00010580		// Zoomed view of ticket booth

#define kgobLobby1			0x00010600		// Front view of lobby
#define kgobLobby2			0x00010700		// Back wall of lobby
#define kgobSnackBar			0x00010800
#define kgobWaiting			0x00010900

#define kgobTheatre1			0x00010c00		// Front view of theatre
#define kgobTheatre2			0x00010d00		// Back wall of theatre
#define kgobTheatre3			0x00010e00		// Front Left view of theatre
#define kgobTheatre4			0x00010f00		// Front Right view of theatre

#define kgobBackstage1		0x00011000		// Front view of backstage area
#define kgobBackstage2		0x00011100		// Back wall of backstage area

#define kgobInspiration1	0x00011400		// Front view of ideas room
#define kgobInspiration2	0x00011500		// Back wall of ideas room
#define kgobInspiration3	0x00011600		// Left wall of ideas room
#define kgobInspiration4	0x00011700		// Right wall of ideas room

#define kgobStudio1			0x00011800		// Front view of studio
#define kgobStudio2			0x00011900		// Back wall of studio
#define kgobStudio3			0x00011a00		// Left wall of studio
#define kgobStudio4			0x00011b00		// Right wall of studio

#define kgobProjects1		0x00011c00		// Front view of projects room
#define kgobProjects2		0x00011d00		// Back wall of projects room
#define kgobProjects3		0x00011e00		// Left wall of projects room
#define kgobProjects4		0x00011f00		// Right wall of projects room

#endif	// !BUILDGOB_H
