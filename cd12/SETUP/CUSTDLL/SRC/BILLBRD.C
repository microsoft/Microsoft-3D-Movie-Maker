/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
**
**	File:			BillBrd.C (sample from ACME95 - offbb.c)
**	Purpose:		Sample billboard handling for Office 95.
**	Notes:
**
**			THIS IS SAMPLE CODE ONLY.  Sample dialog templates and
**			bitmaps will need to be updated for each application,
**			as well as the sample 'heading' font (offsetup.ttf).
**
**	-----------------------------------------------------------------------
**	Initialization and Cleanup:
**
**		Before using Office billboards, the Office billboard classes
**		must be initialized by a call to FInitOffBillboards on the
**		DLL_PROCESS_ATTACH message in your .dll's DllMain routine.
**		To clean up, call FCleanupOffBillboards on DLL_PROCESS_DETACH.
**
**	Usage:
**
**		Dialog Proc:       FOffBlbrdDlgProc
**
**		Dialog Templates:  Defined in offbb.h and intl\usa\offbb.rc
**
**		Background Bitmap: Uses the bitmap (.BMP) file associated with
**		                   the BmpOffBkgrnd BITMAP in offbb.rc.
**
**			NOTE: The billboard dialog is resized to fit the background
**			bitmap.  The various dialog controls, including the heading
**			and text controls, are also resized to fit.
**
**		Heading Font:      Specified in the window text of the IDC_OFFBBHEAD
**		                   control using the following format:
**
**			Format:  "<point size><tab><font name><tab><text>"
**			Example: "20\tOnyx\tNew Feature"
**
**		Text Controls:     Text controls from IDC_OFFBBTXT1 to IDC_OFFBBTXT4
**		                   will be painted with transparent backgrounds,
**		                   using the dialog font.
**
**		Other Bitmaps:
**			- Name and position are specified in the IDC_OFFBBBMP control
**			  of type BBMPOFFB in the dialog template (only one per
**			  billboard).
**			- All bitmap pixels of the same RGB value as the top left-most
**			  bitmap pixel will be painted as transparent.
**			- Each bitmap is drawn into the lower right corner of its
**			  dialog control, unless the control is positioned against the
**			  upper-right corner of the dialog template, in which case it
**			  is drawn into the upper-right corner of the control.
**			- When the dialog is resized to match the background bitmap,
**			  the lower-right corners of bitmap controls are also resized
**			  by the same amount.
**
**
****************************************************************************/

#define OFFBB_C

#include "stdinc.h"
#include "stdtypes.h"
#include "setupapi.h"
#include "setupkit.h"
#include "stdassrt.h"
EnableAssertCode

#include "datadef.h"

#include "billbrd.h"

BOOL __export __loadds CALLBACK FOffBlbrdDlgProc ( HWND hdlg, UINT uiMsg,
											WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK OffBBmpProc ( HWND hwnd, UINT uiMsg,
											WPARAM wParam, LPARAM lParam );

/* Globals
*/
static HINSTANCE vhinstOffDll = hinstNull;		/* Bilboard .dll instance */
static HBITMAP   vhbmBkg      = hbmNull;		/* Background bitmap */
static CHAR vrgchOffTtfPath[_MAX_PATH] = "";	/* Paths to heading font */
static CHAR vrgchOffForPath[_MAX_PATH] = "";

/* Private Functions
*/
static BOOL WINAPI FLoadOffBkgBitmap ( HWND hdlg, PLONG pdxOld, PLONG pdyOld,
										PLONG pdxNew, PLONG pdyNew );
static BOOL WINAPI FResizeDlgControls ( HWND hdlg, LONG dxOld, LONG dyOld,
										LONG dxNew, LONG dyNew );
static VOID WINAPI ScaleDialogControl ( HWND hdlg, HWND hwnd,
						LONG xNewSz, LONG xOldSz, LONG yNewSz, LONG yOldSz );
static BOOL WINAPI FPaintOffBackground ( HDC hdc, HWND hwnd, HBITMAP hbmBk );
static BOOL WINAPI FDrawOffText ( HDC hdc, HWND hwnd );
static BOOL WINAPI FPaintBBmpOff ( HWND hwnd, HBITMAP hbm );
static BOOL WINAPI FDrawHeading ( HDC hdc, HWND hdlg, IDC idc );
static BOOL WINAPI FDrawTransTextCtrl ( HDC hdc, HWND hdlg, IDC idc,
										COLORREF crText );
static VOID WINAPI DrawTransparentBitmap ( HDC hdc, HBITMAP hbmSrc,
					INT xDst, INT yDst, INT dx, INT dy, INT xSrc, INT ySrc,
					COLORREF crTrans );
static BOOL WINAPI FInitHeadFont ( VOID );
static HFONT WINAPI HfontSetHeadFont ( HWND hdlg, BOOL * pfFontResAdded );
static HFONT WINAPI HfontSetTextFont ( HWND hdlg, LONG dyOld, LONG dyNew );
static BOOL WINAPI FRegOffbbWinClasses ( HINSTANCE hinst );
static BOOL WINAPI FUnregOffbbWinClasses ( HINSTANCE hinst );


/*
**	Purpose:
**		Billboard dialog proc for Office 95.
**	Arguments:
**		Standard windows args.
**	Returns:
**		fTrue if message handled, fFalse otherwise.
**	Notes:
**
****************************************************************************/
BOOL __export __loadds CALLBACK FOffBlbrdDlgProc ( HWND hdlg, UINT uiMsg,
											WPARAM wParam, LPARAM lParam )
{
	static HFONT hfontText = hfontNull;
	static HFONT hfontHead = hfontNull;
	static BOOL  fHeadFontAdded = fFalse;

	Unused(wParam);
	Unused(lParam);

	switch (uiMsg)
		{
		case WM_PAINT:
			{
			PAINTSTRUCT	ps;
			HDC	hdc;

			if ((hdc = BeginPaint(hdlg, &ps)) != hdcNull)
				{
				FPaintOffBackground(hdc, hdlg, vhbmBkg);
				FDrawOffText(hdc, hdlg);
				}
			EndPaint(hdlg, (LPPAINTSTRUCT)&ps);
			return (fFalse);
			}

		case WM_INITDIALOG:
			{
			HWND hwnd;
			IDC  idc;
			LONG dxOldDlg, dyOldDlg, dxNewDlg, dyNewDlg;

			FLoadOffBkgBitmap(hdlg, &dxOldDlg, &dyOldDlg, &dxNewDlg, &dyNewDlg);
			FResizeDlgControls(hdlg, dxOldDlg, dyOldDlg, dxNewDlg, dyNewDlg);

			Assert(hfontHead == hfontNull);	/* Only one bb at a time. */
			Assert(hfontText == hfontNull);
			hfontHead = HfontSetHeadFont(hdlg, &fHeadFontAdded);
			hfontText = HfontSetTextFont(hdlg, dyOldDlg, dyNewDlg);

			/* Hide static text controls; we'll draw them ourselves.
			*/
			hwnd = GetDlgItem(hdlg, IDC_OFFBBHEAD);
			Assert(hwnd != hwndNull);
			ShowWindow(hwnd, SW_HIDE);
			for (idc = IDC_OFFBBTXT1;
					(hwnd = GetDlgItem(hdlg, idc)) != hwndNull;
					++idc)
				{
				Assert(idc < IDC_OFFBBTXTMAX);
				ShowWindow(hwnd, SW_HIDE);
				}

			ReactivateSetupScript();
			return (fTrue);
			}

		case STF_REINITDIALOG:
		case STF_ACTIVATEAPP:
			return (fTrue);

		case WM_CLOSE:
		case WM_COMMAND:
			Assert(fFalse);
			break;

		case WM_DESTROY:
			/* Clean up.
			*/
			if (vhbmBkg != hbmNull)
				{
				DeleteObject(vhbmBkg);
				vhbmBkg = hbmNull;
				}
			if (hfontText != hfontNull);
				{
				DeleteObject(hfontText);
				hfontText = hfontNull;
				}
			if (hfontHead != hfontNull);
				{
				DeleteObject(hfontHead);
				hfontHead = hfontNull;
				}
			if (fHeadFontAdded)
				{
				Assert(FValidPath(vrgchOffForPath));
				RemoveFontResource(vrgchOffForPath);
				fHeadFontAdded = fFalse;
				}
			break;
		}

	return (fFalse);
}


/*
**	Load background bitmap and resize dialog to fit.
**
****************************************************************************/
static BOOL WINAPI FLoadOffBkgBitmap ( HWND hdlg, PLONG pdxOld, PLONG pdyOld,
										PLONG pdxNew, PLONG pdyNew )
{
	BITMAP bm;
	RECT   rc;
	LONG   dx, dy;

	Assert(vhbmBkg == hbmNull); 	/* Only one at a time */
	Assert(vhinstOffDll != hinstNull);

	/* Load background bitmap.
	*/
	vhbmBkg = LoadBitmap(vhinstOffDll, "BmpOffBkgrnd");
	if (vhbmBkg == hbmNull)
		{
		SetErrorParam(1, "BmpOffBkgrnd");
		DebugMessage("Billboard .DLL Error{}Setup was not able to "
						"load the billboard background bitmap '%1'.");
		return (fFalse);
		}

	/* Resize dialog to fit background bitmap.
	*/
	GetObject(vhbmBkg, sizeof(BITMAP), (LPSTR)&bm);
	GetClientRect(hdlg, &rc);
	dx = bm.bmWidth  - (rc.right  - rc.left);
	dy = bm.bmHeight - (rc.bottom - rc.top);

	GetWindowRect(hdlg, &rc);
	*pdxOld = rc.right  - rc.left;
	*pdyOld = rc.bottom - rc.top;
	*pdxNew = *pdxOld + dx;
	*pdyNew = *pdyOld + dy;

	SetWindowPos(hdlg, NULL, 0, 0, *pdxNew, *pdyNew, SWP_NOMOVE | SWP_NOZORDER);

	return (fTrue);
}


/*
**	Scale size and position of text controls to match resized dialog.
**
****************************************************************************/
static BOOL WINAPI FResizeDlgControls ( HWND hdlg, LONG dxOld, LONG dyOld,
										LONG dxNew, LONG dyNew )
{
	RECT rc;
	HWND hwnd;
	IDC  idc;

	hwnd = GetDlgItem(hdlg, IDC_OFFBBHEAD);
	Assert(hwnd != hwndNull);
	ScaleDialogControl(hdlg, hwnd, dxNew, dxOld, dyNew, dyOld);

	for (idc = IDC_OFFBBTXT1;
			(hwnd = GetDlgItem(hdlg, idc)) != hwndNull;
			++idc)
		{
		Assert(idc < IDC_OFFBBTXTMAX);
		ScaleDialogControl(hdlg, hwnd, dxNew, dxOld, dyNew, dyOld);
		}

	/*	Resize bitmap control by dx,dy of new edges.
	*/
	hwnd = GetDlgItem(hdlg, IDC_OFFBBBMP);
	Assert(hwnd != hwndNull);
	GetWindowRect(hwnd, &rc);
	MapWindowPoints(HWND_DESKTOP, hdlg, (LPPOINT)&rc, 2);
	rc.right  += dxNew - dxOld;
	rc.bottom += dyNew - dyOld;

	SetWindowPos(hwnd, NULL, 0, 0, rc.right - rc.left, rc.bottom - rc.top,
					SWP_NOMOVE | SWP_NOZORDER);

	return (fTrue);
}


/*
**	Purpose:
**		Scales a dialog control in x by the ratio xNewSz/xOldSz,
**		and in y by the ration yNewSz/yOldSz.
**	Arguments:
**		hdlg - handle to dialog window
**		hwnd - handle to control window
**		xNewSz, xOldSz - horizontal scale ratio
**		yNewSz, yOldSz - vertical scale ratio
**	Returns:
**		none.
**	Notes:
**
****************************************************************************/
static VOID WINAPI ScaleDialogControl ( HWND hdlg, HWND hwnd,
						LONG xNewSz, LONG xOldSz, LONG yNewSz, LONG yOldSz )
{
	RECT   rc;

	GetWindowRect(hwnd, &rc);
	MapWindowPoints(HWND_DESKTOP, hdlg, (LPPOINT)&rc, 2);
	rc.left   = (rc.left   * xNewSz) / xOldSz;
	rc.right  = (rc.right  * xNewSz) / xOldSz;
	rc.top    = (rc.top    * yNewSz) / yOldSz;
	rc.bottom = (rc.bottom * yNewSz) / yOldSz;
	SetWindowPos(hwnd, NULL, rc.left, rc.top, rc.right - rc.left,
							rc.bottom - rc.top, SWP_NOZORDER);
}


/*
****************************************************************************/
static BOOL WINAPI FPaintOffBackground ( HDC hdc, HWND hwnd, HBITMAP hbmBk )
{
	HDC		hdcMem;
	BITMAP  bm;
	HBITMAP	hbmSav;

	if (vhbmBkg == hbmNull)
		return (fFalse);

	Assert(hdc   != hdcNull);
	Assert(hbmBk != hbmNull);

	GetObject(hbmBk, sizeof(BITMAP), (LPSTR)&bm);
	hdcMem = CreateCompatibleDC(hdc);
	hbmSav = SelectObject(hdcMem, hbmBk);

	StretchBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0,
				bm.bmWidth, bm.bmHeight, SRCCOPY);

	SelectObject(hdcMem, hbmSav);
	DeleteDC(hdcMem);

	return (fTrue);
}


/*
**	Draw the static text ourselves so we can set its font and
**	background, etc.
**
****************************************************************************/
static BOOL WINAPI FDrawOffText ( HDC hdc, HWND hdlg )
{
	HWND hwnd;
	IDC  idc;

	Assert(hdc  != hdcNull);
	Assert(hdlg != hwndNull);

	FDrawHeading(hdc, hdlg, IDC_OFFBBHEAD);

	for (idc = IDC_OFFBBTXT1;
			(hwnd = GetDlgItem(hdlg, idc)) != hwndNull;
			++idc)
		{
		Assert(idc < IDC_OFFBBTXTMAX);
		FDrawTransTextCtrl(hdc, hdlg, idc, crBlack);
		}

	return (fTrue);
}


/*
****************************************************************************/
static BOOL WINAPI FDrawHeading ( HDC hdc, HWND hdlg, IDC idc )
{
	CHAR rgch[cbResNameMax] = "";
	HWND hwndCtrl;
	RECT rc;
	INT  xIndent;
	INT  iModSav;
	COLORREF crSav;
	HFONT hfont;
	HFONT hfontSav = hfontNull;

	Assert(hdc  != hdcNull);
	Assert(hdlg != hwndNull);

	hwndCtrl = GetDlgItem(hdlg, idc);
	Assert(hwndCtrl != hwndNull);
	GetWindowRect(hwndCtrl, &rc);
	MapWindowPoints(HWND_DESKTOP, hdlg, (LPPOINT)&rc, 2);
	xIndent = rc.left;

	/* Draw Background
	*/
	crSav = SetBkColor(hdc, crBlack);
	rc.left = 0;		/* Extend to left edge */
	EraseRect(hdlg, hdc, &rc);
	SetBkColor(hdc, crSav);

	/* Draw Text
	*/
	hfont = (HFONT)SendDlgItemMessage(hdlg, idc, WM_GETFONT, 0, 0L);
	if (hfont != hfontNull)
		hfontSav = SelectObject(hdc, hfont);

	GetDlgItemText(hdlg, idc, rgch, sizeof rgch);
	crSav   = SetTextColor(hdc, crWhite);
	iModSav = SetBkMode(hdc, TRANSPARENT);
	rc.left = xIndent;		/* Restore left indent */
	DrawText(hdc, rgch, CbStrLen(rgch), &rc, DT_VCENTER | DT_SINGLELINE |
				DT_LEFT | DT_NOCLIP | DT_NOPREFIX);
	SetTextColor(hdc, crSav);
	SetBkMode(hdc, iModSav);
	if (hfontSav != hfontNull)
		SelectObject(hdc, hfontSav);

	return (fTrue);
}


/*
**	Draws the text of a static text control using a transparent background
**  and the current font and text color.
**
****************************************************************************/
static BOOL WINAPI FDrawTransTextCtrl ( HDC hdc, HWND hdlg, IDC idc,
										COLORREF crText )
{
	CHAR rgch[cbResNameMax] = "";
	HWND hwndCtrl;
	RECT rc;
	INT  iModSav;
	COLORREF crSav;
	HFONT hfont;
	HFONT hfontSav = hfontNull;

	Assert(hdc  != hdcNull);
	Assert(hdlg != hwndNull);

	hfont = (HFONT)SendDlgItemMessage(hdlg, idc, WM_GETFONT, 0, 0L);
	if (hfont != hfontNull)
		hfontSav = SelectObject(hdc, hfont);

	GetDlgItemText(hdlg, idc, rgch, sizeof rgch);

	hwndCtrl = GetDlgItem(hdlg, idc);
	Assert(hwndCtrl != hwndNull);
	GetWindowRect(hwndCtrl, &rc);
	MapWindowPoints(HWND_DESKTOP, hdlg, (LPPOINT)&rc, 2);

	crSav   = SetTextColor(hdc, crText);
	iModSav = SetBkMode(hdc, TRANSPARENT);
	DrawText(hdc, rgch, CbStrLen(rgch), &rc,
				DT_LEFT | DT_NOCLIP | DT_NOPREFIX);
	SetTextColor(hdc, crSav);
	SetBkMode(hdc, iModSav);
	if (hfontSav != hfontNull)
		SelectObject(hdc, hfontSav);

	return (fTrue);
}


/*
**	Purpose:
**		Billboard bitmap window proc for Office 95.
**	Arguments:
**	Returns:
**	Notes:
**
****************************************************************************/
LRESULT CALLBACK OffBBmpProc ( HWND hwnd, UINT uiMsg,
											WPARAM wParam, LPARAM lParam )
{
	HBITMAP hbm;
	CHAR  rgchBm[cbResNameMax] = "";

	switch (uiMsg)
		{
		case WM_CREATE:
			/* Load the bitmap and store the handle.
			*/
			GetWindowText(hwnd, rgchBm, cbResNameMax);
			Assert(vhinstOffDll != hinstNull);
			hbm = LoadBitmap(vhinstOffDll, rgchBm);
			if (hbm == hbmNull)
				{
				SetErrorParam(1, rgchBm);
				DebugMessage("Billboard .DLL Error{}Setup was not able to "
								"load the billboard bitmap '%1'.");
				}
			SetWindowLong(hwnd, GWL_USERDATA, (LONG)hbm);
			break;

		case WM_PAINT:
			hbm = (HBITMAP)GetWindowLong(hwnd, GWL_USERDATA);
			if (hbm != hbmNull)
				FPaintBBmpOff(hwnd, hbm);
			return (0L);

		case WM_DESTROY:
			hbm = (HBITMAP)GetWindowLong(hwnd, GWL_USERDATA);
			if (hbm != hbmNull)
				{
				DeleteObject(hbm);
				SetWindowLong(hwnd, GWL_USERDATA, (LONG)hbmNull);
				}
			break;

		}


	return (DefWindowProc(hwnd, uiMsg, wParam, lParam));
}


/*
**	Draw a transparent bitmap in upper right of control if the control
**	is at its parent's top edge, else draw it in the lower right.
**
****************************************************************************/
static BOOL WINAPI FPaintBBmpOff ( HWND hwnd, HBITMAP hbm )
{
	PAINTSTRUCT ps;
	HDC   hdc;

	Assert(hwnd != hwndNull);
	Assert(hbm != hbmNull);

	if ((hdc = BeginPaint(hwnd, &ps)) != NULL)
		{
		HDC		 hdcBits;
		HBITMAP  hbmSav;
		COLORREF crTrans;
		BITMAP	 bm;
		RECT     rc;
		HWND     hwndPar;
		RECT     rcPar;
		INT      x, y;

		/* Get the transparent color from upper left of bitmap.
		*/
		hdcBits = CreateCompatibleDC(hdc);
		hbmSav  = SelectObject(hdcBits, hbm);
		crTrans = GetPixel(hdcBits, 0, 0);
		SelectObject(hdcBits, hbmSav);
		DeleteDC(hdcBits);

		/* Get parent rect in bitmap control coordinates.
		*/
		hwndPar = GetParent(hwnd);
		Assert(hwndPar != hwndNull);
		GetClientRect(hwndPar, &rcPar);
		MapWindowPoints(hwndPar, hwnd, (LPPOINT)&rcPar, 2);

		/* Draw bitmap in appropriate corner of bitmap control.
		*/
		GetObject(hbm, sizeof(BITMAP), (LPSTR)&bm);
		GetClientRect(hwnd, &rc);
		x = rc.right - bm.bmWidth;
		y = (rc.top == rcPar.top) ? 0 : rc.bottom - bm.bmHeight;

		DrawTransparentBitmap(hdc, hbm, x, y, bm.bmWidth, bm.bmHeight,
								0, 0, crTrans);
		}
	EndPaint(hwnd, (LPPAINTSTRUCT)&ps);

	return (fTrue);
}


/*
**	Purpose:
**		Draws a portion of a given bitmap, treating pixels of a
**		particular color as transparent.
**
**	Arguments:
**		hdc        - Device context
**		hbmSrc     - Source bitmap
**		xDst, yDst - Destination location (upper left)
**		dx, dy     - Size of portion to draw
**		xSrc, ySrc - Offset into source bitmap of bits to draw (upper left)
**		crTrans    - Transparent color
**		
**	Returns:
**		none.
**
**	Notes:
**		Adapted from RegWiz, regutil.cpp routine of the same name.
**
****************************************************************************/
static VOID WINAPI DrawTransparentBitmap ( HDC hdc, HBITMAP hbmSrc,
					INT xDst, INT yDst, INT dx, INT dy, INT xSrc, INT ySrc,
					COLORREF crTrans )
{
	COLORREF   crColor;
	HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
	HBITMAP    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
	HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
	POINT      ptSize;

	hdcTemp = CreateCompatibleDC(hdc);
	SelectObject(hdcTemp, hbmSrc);
	ptSize.x = dx;
	ptSize.y = dy;
	DPtoLP(hdcTemp, &ptSize, 1);	/* Convert device point to logical point */

	/* Create some DCs to hold temporary data.
	*/
	hdcBack   = CreateCompatibleDC(hdc);
	hdcObject = CreateCompatibleDC(hdc);
	hdcMem    = CreateCompatibleDC(hdc);
	hdcSave   = CreateCompatibleDC(hdc);
   
	/* Create a bitmap for each DC. DCs are required for a number
	*	of GDI functions.
	*/

	/* Monochrome DC */
	bmAndBack   = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	
	/* Monochrome DC */
	bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);
	bmAndMem    = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
	bmSave      = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
   
	/* Each DC must select a bitmap object to store pixel data. */
	bmBackOld   = SelectObject(hdcBack, bmAndBack);
	bmObjectOld = SelectObject(hdcObject, bmAndObject);
	bmMemOld    = SelectObject(hdcMem, bmAndMem);
	bmSaveOld   = SelectObject(hdcSave, bmSave);
	
	/* Set proper mapping mode. */
	SetMapMode(hdcTemp, GetMapMode(hdc));
	
	/* Save the bitmap sent here, because it will be overwritten. */
	BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, xSrc, ySrc, SRCCOPY);
   
	/* Set the background color of the source DC to the color.
	*	contained in the parts of the bitmap that should be transparent
	*/
	crColor = SetBkColor(hdcTemp, crTrans);
  
  	/* Create the object mask for the bitmap by performing a BitBlt
	*	from the source bitmap to a monochrome bitmap.
	*/
	BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, xSrc, ySrc, SRCCOPY);

	/* Set the background color of the source DC back to the original color. */
	SetBkColor(hdcTemp, crColor);
   
	/* Create the inverse of the object mask. */
	BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, NOTSRCCOPY);
   
	/* Copy the background of the main DC to the destination. */
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xDst, yDst, SRCCOPY);
   
	/* Mask out the places where the bitmap will be placed. */
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);
   
	/* Mask out the transparent colored pixels on the bitmap. */
	BitBlt(hdcTemp, xSrc, ySrc, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);
	
	/* XOR the bitmap with the background on the destination DC. */
	BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, xSrc, ySrc, SRCPAINT);
   
	/* Copy the destination to the screen. */
	BitBlt(hdc, xDst, yDst, ptSize.x, ptSize.y, hdcMem, 0, 0, SRCCOPY);
   
	/* Place the original bitmap back into the bitmap sent here. */
	BitBlt(hdcTemp, xSrc, ySrc, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);
   
	/* Delete the memory bitmaps. */
	DeleteObject(SelectObject(hdcBack, bmBackOld));
	DeleteObject(SelectObject(hdcObject, bmObjectOld));
	DeleteObject(SelectObject(hdcMem, bmMemOld));
	DeleteObject(SelectObject(hdcSave, bmSaveOld));

	/* Delete the memory DCs. */
	DeleteDC(hdcMem);
	DeleteDC(hdcBack);
	DeleteDC(hdcObject);
	DeleteDC(hdcSave);
	DeleteDC(hdcTemp);
}

/* KLUDGE: Prototype from mssetup.dll's errchk.h.
*
*	Need to use this lower-level routine to avoid the user-quit handling
*	of the standard CopyFile API.  If the user hit 'Cancel' in the copy
*	gauge dialog while the .TTF file was getting copied in the billboard
*	dialog proc, Acme would try to exit immediately and GP-Fault on its
*	call to DestroyWindow for the Frame window.
*/
extern VOID WINAPI STF_CopyFile ( SZ szFullPathSrc, SZ szFullPathDst, CMO cmo, BOOL fAppend,
									PSEC psecError );

/*
**	Purpose:
**		Initializes the heading font by copying its .TTF file to the
**		Windows directory and calling CreateScalableFontResource.
**	Arguments:
**		none.
**	Returns:
**		fTrue if success, fFalse otherwize.
**
****************************************************************************/
static BOOL WINAPI FInitHeadFont ( VOID )
{
	static BOOL fInitFont  = fFalse;
	static BOOL fFontReady = fFalse;

	/* If first time, create font from our .TTF file.
	*/
	if (!fInitFont)
		{
		CHAR rgchSrcTtf[_MAX_PATH];
		
		fInitFont = fTrue;

		/* Locate the TTF file in the DLL directory, and copy it to the
		*	Windows directory.  We must do this because the DLL could be
		*	on a UNC path, which Windows can't handle for a font resource.
		*/
		if (GetModuleFileName(vhinstOffDll, rgchSrcTtf, sizeof rgchSrcTtf) != 0
			&& GetWindowsDirPath(vrgchOffTtfPath, sizeof vrgchOffTtfPath) != 0)
			{
			SZ  sz;
			SEC secError;

			/* Replace DLL file name with TTF file name.
			*/
			for (sz = SzLastChar(rgchSrcTtf);
					*sz != '\\';
					sz = SzPrevChar(rgchSrcTtf, sz))
				;
			Assert(*sz == '\\');
			sz = SzNextChar(sz);
			SzStrCopy(sz, szOffTtf);
			Assert(CbStrLen(rgchSrcTtf) < sizeof rgchSrcTtf);

			/* Build dest file paths.
			*/
			Assert(*SzLastChar(vrgchOffTtfPath) == '\\');
			SzStrCopy(vrgchOffForPath, vrgchOffTtfPath);
			SzStrCat(vrgchOffForPath, szOffFor);
			Assert(CbStrLen(vrgchOffForPath) < sizeof vrgchOffForPath);
			SzStrCat(vrgchOffTtfPath, szOffTtf);
			Assert(CbStrLen(vrgchOffTtfPath) < sizeof vrgchOffTtfPath);

			/* Replace any existing TTF file.
			*/
			RemoveFile(vrgchOffTtfPath, cmoForce);
			STF_CopyFile(rgchSrcTtf, vrgchOffTtfPath, cmoOverwrite, 0,
							&secError);
			
			if (DoesFileExist(vrgchOffTtfPath, femExists))
				{
				RemoveFile(vrgchOffForPath, cmoForce);
				fFontReady = CreateScalableFontResource(0, vrgchOffForPath, 
					                        vrgchOffTtfPath, NULL);
				if (!fFontReady)
					*vrgchOffForPath = chEos; /* Not created, so don't remove */
				}
			else
				*vrgchOffTtfPath = chEos; /* Not created, so don't remove */
			}
		}

	return (fFontReady);
}


/*
**	Purpose:
**		Creates the heading font of the corret type and size.
**	Arguments:
**		hdlg - Handle to billboard dialog
**		pfFontResAdded - Flag set to fTrue if a font resource was
**				created which will need to be removed when .dll exits.
**	Returns:
**		Handle to font created, or NULL if failed.
**	Notes:
**		Adapted from WinWord 6.0 setup's wword_bb.dll source.
**
****************************************************************************/
static HFONT WINAPI HfontSetHeadFont ( HWND hdlg, BOOL * pfFontResAdded )
{
	HFONT hfont      = NULL;
	HFONT hfontOld   = NULL;
	BOOL  fFontReady = fFalse;
	LOGFONT  lf;
	int   wPt;
	int   cch;
	int   dxp;
	int   fRemFont = fFalse;
	int   dyp;
	char  sz[256];
	char far *lpchEnd = sz + 256;
	char far *lpchFont;
	char far *lpch;
	DWORD dw = 0;
	TEXTMETRIC tm;
	HWND hwnd;
	RECT rect;
	SIZE sSize;
	HDC  hdc;
	
	/* Initialize font from .TTF file. */
	fFontReady = FInitHeadFont();

	hwnd = GetDlgItem(hdlg, IDC_OFFBBHEAD);
	Assert(hwnd != hwndNull);

	/*
	* The window text is divided into three parts, separated by
	* tab characters.  
	*
	* The first part is the font size.  (TAB) 
	* The second part is the font name. (TAB)
	* The third part is the headline text.
	*/
	GetWindowText(hwnd, sz, 256);

	/* init all logfont fields to 0s */
	for (lpch = (char *) &lf; lpch < (char *) &lf + sizeof(LOGFONT); lpch++)
		*lpch = 0;

	/* 
	* The font size is the sum of the digit characters up to 
	* the first tab.
	*/
	for (lpch = sz, wPt = 0; *lpch != '\t' && lpch < lpchEnd; lpch++)
		{
		char ch;
		
		wPt *= 10;
		if ((ch = *lpch) >= '0' && ch <= '9')
			wPt += *lpch - '0';
		}

	/* For overflow, set point size to something safe, like 10 */
	if (lpch >= lpchEnd)
		wPt = 10;
	
	/* Beginning of the font name is after the size and a tab */
	for (lpchFont = ++lpch; *lpch != '\t' && lpch < lpchEnd; lpch++)
		;

	/* 
	* lpch should now be pointing to the tab between the font name
	* and the control text.  End off the font name string, point to
	* beginning of text.
	*/
	*lpch++ = '\0';
	
	/* If overflow, make the font and text null strings */
	if (lpch >= lpchEnd)
		*lpch = *lpchFont = '\0';
	cch = lstrlen(lpch);

	/* Use Arial as a fallback if the custom font isn't available */
	fRemFont = fFontReady && AddFontResource(vrgchOffForPath);
	if (fRemFont)
		lstrcpy(lf.lfFaceName, lpchFont);
	else
		lstrcpy(lf.lfFaceName, "Arial");
	
	GetClientRect(hwnd, &rect);
	dxp = rect.right - rect.left;
	dyp = rect.bottom - rect.top;
	
	hdc = GetDC(hwnd);
	/* block */
		{
		/* 
		* Loop until the text will fit in the control.  Decrease the 
		* size by two points per iteration until it fits.
		*/
		dw = (DWORD) -1;
		do 
			{
			/* Set the size and get the font. */
			lf.lfHeight = - MulDiv(wPt, GetDeviceCaps(hdc, LOGPIXELSY), 72);

			if (hfont != NULL)
				{
				Assert(hfontOld != NULL);
				SelectObject(hdc, hfontOld);
				DeleteObject(hfont);
				}
		
			if ((hfont = CreateFontIndirect(&lf)) != NULL)
				{
				/* Only remember the original font. */
				if (hfontOld == NULL)
					hfontOld = SelectObject(hdc, hfont);
				else
					SelectObject(hdc, hfont);
				}

			GetTextMetrics(hdc, &tm);

			/* 
			* We always want to do this once, but unless the horiz
			* size is too big, don't bother doing it again because
			* the call is too expensive. 
			*/
			if (LOWORD(dw) > (unsigned)dxp)
				GetTextExtentPoint(hdc, lpch, cch, &sSize);
			wPt -= 2;
			} 
		while (hfont != NULL && wPt > 0 &&
				((int)sSize.cx > dxp || tm.tmHeight > dyp));

		if (hfontOld != NULL)
			SelectObject(hdc, hfontOld);
		}
	ReleaseDC(hwnd, hdc);

	if (hfont != NULL)
		SendDlgItemMessage(hdlg, IDC_OFFBBHEAD, WM_SETFONT, (WPARAM)hfont, 0L);
	SetDlgItemText(hdlg, IDC_OFFBBHEAD, lpch);	/* Remove embedded font info */

	*pfFontResAdded = fRemFont;
	return (hfont);
}


/*
**	Purpose:
**		Creates a bold font from the dialog font, scales its height,
**		and sets the text fields to that font.
**	Arguments:
**		hdlg - Dialog window handle
**		dyOld, dyNew - Font height scale ratio
**	Returns:
**		Handle to the font created, hfontNull if fails.
**
****************************************************************************/
static HFONT WINAPI HfontSetTextFont ( HWND hdlg, LONG dyOld, LONG dyNew )
{
	HFONT   hfontDlg;
	LOGFONT lFont;
	int     idc;

	if ((hfontDlg = (HFONT)SendMessage(hdlg, WM_GETFONT, 0, 0L)) != hfontNull)
		{
		if (GetObject(hfontDlg, sizeof(LOGFONT), (LPSTR)&lFont))
			{
			/* Scale font, always rounding to larger font.
			*	Note: Need to scale so Win95 large fonts won't
			*	mess us up.
			*/
			lFont.lfHeight = (lFont.lfHeight * dyNew
						+ (lFont.lfHeight < 0 ? 0 - dyOld + 1 : dyOld - 1))
						/ dyOld;

			lFont.lfWeight = FW_BOLD;
			if ((hfontDlg = CreateFontIndirect((LPLOGFONT)&lFont)) != hfontNull)
				{
				for (idc = IDC_OFFBBTXT1; GetDlgItem(hdlg,idc) != hwndNull;
					  ++idc)
					{
					Assert(idc < IDC_OFFBBTXTMAX);
					SendDlgItemMessage(hdlg, idc, WM_SETFONT,
										(WPARAM)hfontDlg, 0L);
					}
				}
			}
		}

	return (hfontDlg);
}


/*
****************************************************************************/
static BOOL WINAPI FRegOffbbWinClasses ( HINSTANCE hinst )
{
	WNDCLASS	wndclass;

	/* BBMP_OFFICE_CLASS
	*/
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon			= NULL;
	wndclass.lpszMenuName   = NULL;
	wndclass.lpszClassName  = BBMP_OFFICE_CLASS;
	wndclass.hbrBackground  = (HBRUSH)NULL;
	wndclass.hInstance      = hinst;
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc    = (WNDPROC)OffBBmpProc;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra 	= 0;

	if (!RegisterClass(&wndclass))
		{
		WNDCLASS wndclassCur;
		DWORD dwErr = GetLastError();

		if (!GetClassInfo(hinst, BBMP_OFFICE_CLASS, &wndclassCur))
			return (fFalse);
		}

	return (fTrue);
}


/*
****************************************************************************/
static BOOL WINAPI FUnregOffbbWinClasses ( HINSTANCE hinst )
{
	if (!UnregisterClass(BBMP_OFFICE_CLASS, hinst))
		{
		return (fFalse);
		}

	return (fTrue);
}



/*
**	Called from DllMain on DLL_PROCESS_ATTACH.
**
****************************************************************************/
BOOL WINAPI FInitOffBillboards ( HINSTANCE hinst )
{
	Assert(hinst != hinstNull);
	Assert(vhinstOffDll == hinstNull);

	vhinstOffDll = hinst;
	return (FRegOffbbWinClasses(hinst));
}


/*
**	Called from DllMain on DLL_PROCESS_DETACH.
**
****************************************************************************/
BOOL WINAPI FCleanupOffBillboards ( HINSTANCE hinst )
{
	Assert(hinst != hinstNull);
	Assert(hinst == vhinstOffDll);

	/* Clean up our heading font files.
	*/
	if (*vrgchOffTtfPath != chEos)
		{
		RemoveFile(vrgchOffTtfPath, cmoForce);
		*vrgchOffTtfPath = chEos;
		}
	if (*vrgchOffForPath != chEos)
		{
		RemoveFile(vrgchOffForPath, cmoForce);
		*vrgchOffForPath = chEos;
		}

	vhinstOffDll = hinstNull;
	return (FUnregOffbbWinClasses(hinst));
}



