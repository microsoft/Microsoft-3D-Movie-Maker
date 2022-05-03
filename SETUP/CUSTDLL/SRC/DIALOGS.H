/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/****************************************************************************
**
**	File:			SAMPDLGS.H
**	Purpose:		Header file for dialog controls in the Sample App.
**	Notes:
**
****************************************************************************/

#ifndef SAMPDLGS_H
#define SAMPDLGS_H


/*
 *	Dialog ID's
 */
#define DLG_BBREGCARD	990
#define DLG_BBINSTANT	991
#define DLG_BBREQUESTS	992
#define DLG_BBANALYSIS	993
#define DLG_BBLEARN		994

#define DLG_SINGSELLIST 995


/*
 * Pushbutton controls.
 *	NOTE: Pushbuttons must be within the range PUSHMIN to PUSHMAX.
 */
#define	IDC_PUSHMIN				0x0000
#define	IDC_PUSHMAX				0x0100

#define	IDC_OK					IDOK		/* 0x0001 (from windows.h) */
#define	IDC_CANCEL				IDCANCEL
#define	IDC_ABORT				IDABORT
#define	IDC_RETRY				IDRETRY
#define	IDC_IGNORE				IDIGNORE
#define	IDC_YES					IDYES
#define	IDC_NOPE				IDNO		/* 0x0007 */


/*
 *	Control ID's		 ALWAYS KEEP TEXT CONTROL IDs CONTIGUOUS!!!!1
 */
#define IDC_BBTEXT1      0x0301
#define IDC_BBTEXT2      0x0302
#define IDC_BBTEXT3      0x0303
#define IDC_BBTEXT4		 0x0304
#define IDC_BBTEXTMAX    0x0305

#define IDC_DESTDIR      0x0401
#define IDC_LISTBOX      0x0402
#define IDC_LISTBOXLABEL 0x0403


/*
 *	Icon ID's
 */
#define ICON_SAMPAPP   201

/*
 *	Window Classes
 */
#define BBMP_CLASS		"BBMPGNGK"
#define HEADLINE_CLASS	"HEADLINE"




#endif  /* SAMPDLGS_H */
