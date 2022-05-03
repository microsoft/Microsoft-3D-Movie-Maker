/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/*****************************************************************************
*
*  SHAREDEF.H
*
*  Copyright (C) Microsoft Corporation 1995.
*  All Rights reserved.
*
******************************************************************************
*
*  Module Intent
*
*  Contains definitions for shared objects (in src\shared.cht).  Needs to be
*	included by modules that want to use shared cursors or the portfolio or
*	the map.
*
******************************************************************************
*
*  Revision History:  Created 06/26/95 by *****.
*
*****************************************************************************/

#ifndef SHAREDEF_H
#define SHAREDEF_H

/*****************************************************************************
*                                                                            *
*                              Constants                                     *
*                                                                            *
*****************************************************************************/

// -----------------------------------------------------
// Shared constants.
// -----------------------------------------------------

#define kmaxVolume 			0x00010000

// Script to run in shared.chk to initialize utils.
#define kcnoInitShared		0x00050001

// -----------------------------------------------------
// Cursor Definitions
// -----------------------------------------------------

#define kcrsHand				0x00000001
#define kcrsHand1				0x00000001	// Different names in Studio, Building
#define kcrsArrow				0x00000002
#define kcrsArrow1			0x00000002	// Different names in Studio, Building
#define kcrsWand1				0x00000003
#define kcrsActionBrowser	0x00000003	// Different names in Studio, Building
#define kcrsStar				0x00000004
#define kcrsLeft				0x00000005
#define kcrsRight				0x00000006
#define kcrsDown				0x00000007
#define kcrsUp					0x00000008
#define kcrsTurn1				0x00000009

// -----------------------------------------------------
// Shared Object Definitions
// -----------------------------------------------------

#define kgobMapMain			0x00012000

#define kwavDing				0x00010004	// Wave file used when adjusting
													// master volume.
#define kwavPortOpenMovie       0x00010005
#define kwavPortSaveMovie       0x00010006
#define kwavPortOpenSound       0x00010007
#define kwavMapObject 			0x00010008
#define kwavMapOpen				0x00010009

// For error help
#define kgobBalloonError		0x00016050
#define ktpcErrorScript			0x00016051
#define kgobErrorMessageGlass	0x00016052
#define kcnoErrorMessage		0x00016053

#endif	// !SHAREDEF_H
