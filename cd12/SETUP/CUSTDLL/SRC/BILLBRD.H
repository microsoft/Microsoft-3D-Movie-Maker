/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/****************************************************************************
**
**	File:			OFFBB.H
**	Purpose:		Header file for sample Office 95-style billboards.
**	Notes:
**
****************************************************************************/

#ifndef OFFBB_H
#define OFFBB_H


#define cbResNameMax			255
#define crWhite					RGB(255,255,255)
#define crBlack					RGB(0,0,0)

#define EraseRect(hwnd,hdc,prc) ExtTextOut(hdc,0,0,ETO_OPAQUE,prc,NULL,0,NULL)


/* Font files for the billboard headings
*/
#define szOffTtf	"offsetup.ttf"		
#define szOffFor	"offsetup.for"


/*	Dialog Templates
*/
#define BB_OFFTEST1				501
#define BB_OFFTEST2				502


/* Control ID's
*/
#define IDC_STATIC				-1
#define IDC_OFFBBBMP			0x0350
#define IDC_OFFBBHEAD			0x0360

#define IDC_OFFBBTXT1			0x0361	/* TXT ID's MUST be sequential! */
#define IDC_OFFBBTXT2			0x0362
#define IDC_OFFBBTXT3			0x0363
#define IDC_OFFBBTXT4			0x0364
#define IDC_OFFBBTXTMAX			0x0365


/*	Window Classes
 */
#define BBMP_OFFICE_CLASS		"BBMPOFFB"


/*	Public Functions
*/
extern BOOL WINAPI FInitOffBillboards ( HINSTANCE hinst );
extern BOOL WINAPI FCleanupOffBillboards ( HINSTANCE hinst );



#endif  /* OFFBB_H */

