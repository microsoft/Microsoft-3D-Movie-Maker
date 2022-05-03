/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
**
**	File:			DLLMAIN.C (Acme95 dll sample SAMPMAIN.C)
**	Purpose:		DLL main routine
**	Notes:
**
****************************************************************************/

#define SAMPMAIN_C

#include "stdinc.h"
#include "setupapi.h"
#include "cui.h"
#include "dialogs.h"
#include "stdtypes.h"
#include "datadef.h"
#include "billbrd.h"

BOOL WINAPI DllMain ( HANDLE hModule, DWORD dwReason, LPVOID lpReserved );

BOOL __export __loadds CALLBACK FBillbrdDlgProc ( HWND hdlg, UINT uiMsg,
											WPARAM wParam, LPARAM lParam );
LRESULT CALLBACK BBmpProc ( HWND hwnd, UINT uiMsg,
											WPARAM wParam, LPARAM lParam );

/* Private Functions
*/
static VOID    FAR PASCAL PaintBorder ( HWND hwnd );
static HFONT   FAR PASCAL HfontSetBBFont ( HWND hdlg );
static int     FAR PASCAL DyScaleToScreen ( HDC hdc, int dy );
static HBITMAP FAR PASCAL LoadBitmapItem ( HANDLE hInst, LPSTR lpName,
							BOOL * pfDefault );
static LONG    FAR PASCAL DrawDlgBmpItem(HWND, HDC, LPSTR, DWORD, int, int, int, int);
static BOOL    FAR PASCAL FRegisterWinClasses ( VOID );
static BOOL    FAR PASCAL FUnregisterWinClasses ( VOID );

/* System Color Macros
*/
#define NearestColor(hdc,rgb) (GetNearestColor(hdc,rgb) & 0x00FFFFFFL)
#define EraseRect(hwnd,hdc,prc) ExtTextOut(hdc,0,0,ETO_OPAQUE,prc,NULL,0,NULL)

/* Global Data
*/
HINSTANCE hinst = NULL;

static char *rgszPostman[] = { "postwm", "postbm", "postaf", "postwf" };
#define cszPostman	4

/* Full path to the font resource file we create. */
char szFor[cchSzMax];
char szTtf[cchSzMax];
int fFontReady = fFalse;
int fInitFont = fFalse;
HBRUSH hbrBillboard = NULL;

/* REVIEW: Need to see if the new code in DllMain using this guy is ok */
int fSetupFont = fFalse;

/* Names of the font files for the billboard headline */
#define szSetupTtfFile	"wwsetup.ttf"		
#define szSetupForFile	"wwsetup.for"


/*
**	Purpose:
**		Initialization routine for DLL.
**	Arguments:
**	Returns:
**		1 always
**
***************************************************************************/
BOOL WINAPI DllMain ( HANDLE hModule, DWORD dwReason, LPVOID lpReserved )
{
static WORD wCountAttach = 0;

#ifdef NEVER	/* REVIEW */
	int  cch;
#endif

	Unused(lpReserved);

	/* REVIEW: Need to see if this code in DllMain is ok since it is
 	*	different from the stuff we have in LibMain (16-bit).
 	*/

	switch( dwReason) {
	case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:

		if(0 == wCountAttach++)
      {
  		Assert(hinst == NULL);
  		hinst = hModule;

  		if (!FRegisterWinClasses())
  			return (0);

  		if (!FInitOffBillboards(hinst))
  			return (0);

  		szFor[0] = '\0';
  		szTtf[0] = '\0';

      #ifdef NEVER	/* REVIEW */
    		/* 
    		 * Locate the font file in the directory in which the DLL is found, 
    		 * create a resource in the same directory and then add the font 
    		 */
    		if ((cch = GetModuleFileName(hModule, szTtf, 256)) != 0)
    			{
    			char * pch;
		
    			/* Strip off the file name part.  Leave the backslash */
    			for (pch = szTtf + cch; *pch != '\\'; pch--)
    				;
    			*++pch = '\0';

    			/* Build the file names */
    			lstrcat(szTtf, szSetupTtfFile);
    			GetWindowsDirPath(szFor, 256);
    			lstrcat(szFor, szSetupForFile);

    			RemoveFile(szFor, cmoForce);
    	 		fSetupFont = CreateScalableFontResource(1, szFor, szTtf, NULL);
    			}
      #endif /* NEVER */
      } // if wCountAttach == 0

    #ifdef _DEBUG
    else
      {
      char Buf[80];

      wsprintf(Buf, "ExpSetup.dll now attached to %u times.", wCountAttach);
      MessageBox(NULL, Buf, "Warning", MB_OK);
      }
    #endif

		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		{
		if(0 == --wCountAttach)
      {
  		BOOL fOffRes = FCleanupOffBillboards(hinst);

      hinst = NULL;

  		/* Clean up our messes... */
  		if (szFor[0] != '\0')
  			RemoveFile(szFor, cmoForce);

  		if (szTtf[0] != '\0')
  			RemoveFile(szTtf, cmoForce);

  		if (!FUnregisterWinClasses() || !fOffRes)
  			return (0);
      }
		break;
		}
	}

	return TRUE;
}


/*
**	Purpose:
**		Function that must be implemented and exported from every
**		billboard/custom action .DLL called by Acme.  Used to validate
**		that the .DLL was built with the correct version of the Acme
**		header files.
**	Arguments:
**		none.
**	Returns:
**		Header files version string from datadef.h.
**	Notes:
**		Function prototype defined in datadef.h.
**
****************************************************************************/
SZ WINAPI SzGetDatadefVer ( VOID )
{
	return (SZ_DATADEF_VER);
}


/*
**	Purpose:
**		Billboard dialog proc.
**	Arguments:
**		Standard windows args.
**	Returns:
**		fTrue if message handled, fFalse otherwise.
**	Notes:
**
****************************************************************************/
BOOL __export __loadds CALLBACK FBillbrdDlgProc ( HWND hdlg, UINT uiMsg,
											WPARAM wParam, LPARAM lParam )
{
	/* WARNING: Using static hfont here assumes no two bb dialogs
	*	will be using this proc at the same time!
	*/
	static HFONT hfont = (HFONT)NULL;

	Unused(wParam);
	Unused(lParam);

	switch (uiMsg)
		{
		case WM_PAINT:
			PaintBorder(hdlg);
			return (fFalse);

		case WM_INITDIALOG:
			Assert(hfont == (HFONT)NULL);	/* Only one bb at a time. */
			hfont = HfontSetBBFont(hdlg);
			ReactivateSetupScript();
			return (fTrue);

		case STF_REINITDIALOG:
		case STF_ACTIVATEAPP:
			return (fTrue);

		case WM_CLOSE:
		case WM_COMMAND:
			Assert(fFalse);
			break;

		case WM_DESTROY:
			if (hfont != (HFONT)NULL)
				{
				DeleteObject(hfont);
				hfont = (HFONT)NULL;
				}
			break;
		}

	return (fFalse);
}


/*  E R A S E  R E C T  C O L */
/*-------------------------------------------------------------------------
    %%Function: EraseRectCol
    %%Contact: PhillipG
    %%Reviewed:

	Erase prect, with col as the background color.    
-------------------------------------------------------------------------*/
void EraseRectCol(HDC hdc, RECT *prect, COLORREF col)
{
	SetBkColor(hdc, col);
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, prect, (LPSTR)NULL, 0, NULL);
}


#define dxBrdr	7
#define colBorderLeft      RGB(  0,128,128)
#define colBorderTop       RGB(128,  0,128)
#define colBorderShadow    RGB(128,128,128)
#define colHeadlineLine    RGB(  0,128,128)

/*
**	Purpose:
**	Arguments:
**	Returns:
**	Notes:
**	
****************************************************************************/
static VOID FAR PASCAL PaintBorder(HWND hwnd)
{
	PAINTSTRUCT	ps;
	RECT rect;
	HDC	 hdc;
	int iBdr;
	int xpLeft;
	int xpRight;
	int ypTop;
	int ypBottom;
	
	GetClientRect(hwnd, &rect);
	xpLeft = rect.left;
	xpRight = rect.right;
	ypTop = rect.top;
	ypBottom = rect.bottom;
	
	if ((hdc = BeginPaint(hwnd, &ps)) != NULL)
		{
		/* draw dxBrdr lines */
		for (iBdr = 0; iBdr < dxBrdr; iBdr++)
			{
			/* draw the top line */
			rect.top = ypTop;
			rect.left = xpLeft + 1;
			rect.bottom = ypTop + 1;
			rect.right = xpRight;
			EraseRectCol(hdc, &rect, colBorderTop);

			/* Draw the left line */
			rect.top = ypTop;
			rect.left = xpLeft;
			rect.bottom = ypBottom;
			rect.right = xpLeft + 1;
			EraseRectCol(hdc, &rect, colBorderLeft);
			
			/* Draw the bottom line */
			rect.top = ypBottom - 1;
			rect.left = xpLeft;
			rect.bottom = ypBottom;
			rect.right = xpRight;
			EraseRectCol(hdc, &rect, colBorderShadow);
			
			/* Draw the right line */
			rect.top = ypTop + 1;
			rect.left = xpRight - 1;
			rect.bottom = ypBottom;
			rect.right = xpRight;
			EraseRectCol(hdc, &rect, colBorderShadow);

			/* Move all sides in one pixel. */
			xpLeft++;
			xpRight--;
			ypTop++;
			ypBottom--;
			}
		}
	EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
}


/*
**	Purpose:
**	Arguments:
**	Returns:
**	Notes:
**
****************************************************************************/
static HFONT FAR PASCAL HfontSetBBFont ( HWND hdlg )
{
	HFONT   hDlgFont;
	LOGFONT lFont;
	int     idc;

	if ((hDlgFont = (HFONT)SendMessage(hdlg, WM_GETFONT, 0, 0L)) != hfontNull)
		{
		if (GetObject(hDlgFont, sizeof(LOGFONT), (LPSTR)&lFont))
			{
			lFont.lfWeight = FW_NORMAL;
			if ((hDlgFont = CreateFontIndirect((LPLOGFONT)&lFont)) != NULL)
				{
				for (idc = IDC_BBTEXT1; GetDlgItem(hdlg,idc) != (HWND)NULL;
					  ++idc)
					{
					SendDlgItemMessage(hdlg, idc, WM_SETFONT,
							(WPARAM)hDlgFont, 0L);
					}
				}
			}
		}
	return (hDlgFont);
}


#define cbResNameMax  255
/*
**	Purpose:
**		Billboard bitmap window proc.
**	Arguments:
**	Returns:
**	Notes:
**
****************************************************************************/
LRESULT CALLBACK BBmpProc ( HWND hwnd, UINT uiMsg,
											WPARAM wParam, LPARAM lParam )
{
	static int iszPostman;	/* WARNING: Restricted to one instance */
	PAINTSTRUCT ps;
	TEXTMETRIC	tm;
	HDC   hdc;
	int	  DX, DY;
	char  rgchBmp[cbResNameMax];

	switch (uiMsg)
		{
		case WM_CREATE:
			iszPostman = (int)(LOWORD(GetTickCount()) % cszPostman);
			break;

		case WM_PAINT:
			GetWindowText(hwnd, rgchBmp, cbResNameMax);
			if (lstrcmp(rgchBmp, "postman") == 0)
				{
				Assert(iszPostman >= 0 && iszPostman < cszPostman);
				Assert(lstrlen(rgszPostman[iszPostman]) < sizeof rgchBmp);
				lstrcpy(rgchBmp, rgszPostman[iszPostman]);
				}
			/* 
			 * REVIEW: NYI: If ctl3d.dll is not active for some
			 *	reason (such as if video is EGA) we should use
			 *	default window color instead.
			 */
			if ((hdc = BeginPaint(hwnd, &ps)) != NULL)
				{
#ifdef NOTUSED	/* REVIEW */
				COLORREF col;
				
				if (GetSystemMetrics(SM_CYSCREEN) < 450) /* EGA */
					col = GetSysColor(COLOR_WINDOW);
				else
					col = GetSysColor(COLOR_BTNFACE);
				EraseRectCol(hdc, &ps.rcPaint, NearestColor(hdc, col));
#endif /* NOTUSED */
				GetTextMetrics(hdc, &tm);
				DX = DY = (tm.tmHeight) / 4;
				DrawDlgBmpItem(hwnd, hdc, rgchBmp, SRCCOPY, fFalse, fFalse, DX, DY);
				}
			EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
			return (0L);
		}
	return (DefWindowProc(hwnd, uiMsg, wParam, lParam));
}


/*
**	Purpose:
**	Arguments:
**	Returns:
**	Notes:
**
****************************************************************************/
static int FAR PASCAL DyScaleToScreen ( HDC hdc, int dy )
{
	int dyNew;

	switch (GetDeviceCaps(hdc, LOGPIXELSY))
		{
		case 72:	/* EGA */
			dyNew = (int) (((LONG) dy) * 350L / 480L);
			break;
		case 96:	/* VGA */
		case 120:	/* XGA (8514) */
		default:
			dyNew = dy;
			break;
		}
	return (dyNew);
}


/*
**	Purpose:
**		Load a device specific bitmap from a resource file.
**	Arguments:
**	Returns:
**	Notes:
**
****************************************************************************/
static HBITMAP FAR PASCAL LoadBitmapItem ( HANDLE hInst, LPSTR lpName,
											BOOL * pfDefault )
{
	char	szName[80];
	HBITMAP	hbm;
	int		nColors;
	int		dxScreen;
	int		dyScreen;
	HDC		hdc;
	BOOL	fDef = fFalse;

	hdc = GetDC(NULL);

	nColors  = GetDeviceCaps(hdc, NUMCOLORS);
	dxScreen = GetSystemMetrics(SM_CXSCREEN);
	dyScreen = GetSystemMetrics(SM_CYSCREEN);
	if (dyScreen < 479) /* double scan CGA is 400 */
		dyScreen = 350;

	ReleaseDC(NULL, hdc);

	/* Look for a resource of the form WxHxC.
	*/
	wsprintf(szName, "%s%dx%dx%d", lpName, dxScreen, dyScreen, nColors);
	hbm = LoadBitmap(hInst, szName);

	/* Look for a resource of the form WxH.
	*/
	if (!hbm)
		{
		wsprintf(szName, "%s%dx%d", lpName, dxScreen, dyScreen);
		hbm = LoadBitmap(hInst, szName);
		}

	/* Look for the default resource name.
	*/
	if (!hbm)
		{
		hbm = LoadBitmap(hInst, lpName);
		fDef = fTrue;
		}

	*pfDefault = fDef;
	return (hbm);
}


#define DPSoa    0x00A803A9L
#define DPa      0x00A000C9L
/*
**	Purpose:
**	Arguments:
**	Returns:
**	Notes:
**
****************************************************************************/
static LONG FAR PASCAL DrawDlgBmpItem(HWND hwnd, HDC hdc, LPSTR szBmp, 
		DWORD rop, int fShadow, int fCalcSizeOnly, int DX, int DY)
{
	HDC		hdcMem;
	HBITMAP	hbmpSav;
	HBITMAP	hbmp;
	BITMAP	bmp;
	int		bmH;
	int     xw;
	int 	yw;
	BOOL	fScale;
	RECT rect;

	if ((hbmp = LoadBitmapItem(hinst, szBmp, &fScale)) == NULL)
		return (0L);

	GetObject(hbmp, sizeof(BITMAP), (LPSTR)&bmp);

	if (fScale)
		bmH = DyScaleToScreen(hdc, bmp.bmHeight);
	else
		bmH = bmp.bmHeight;

	if (fCalcSizeOnly)
		{
		DeleteObject(hbmp);
		return (MAKELONG(bmp.bmWidth + 4, bmH + 4));
		}

	hdcMem = CreateCompatibleDC(hdc);
	hbmpSav = SelectObject(hdcMem, hbmp);

	SelectObject(hdc, GetStockObject(BLACK_BRUSH));
	if (fShadow)
		{
		/*
		 * WARNING! This code is broken.  This clock still draws the 
		 * shadow based on the top left corner, whereas the StretchBlt
		 * below draws the bitmap in the lower right corner.
		 */
		
		xw = DX + 2;
		yw = DY + 2;
		
		SelectObject(hdc, GetStockObject(GRAY_BRUSH));
		PatBlt(hdc, xw - 3, yw - 3, bmp.bmWidth + 4, bmH + 4, PATCOPY);
		rect.left = xw - DX;
		rect.right = rect.left + bmp.bmWidth;
		rect.top = yw - DY;
		rect.bottom = rect.top + bmH;
		InflateRect(&rect, 2, 2);
		SelectObject(hdc, GetStockObject(WHITE_BRUSH));
		PatBlt(hdc, rect.left, rect.top, rect.right - rect.left,
						rect.bottom - rect.top, PATCOPY);
		FrameRect(hdc, &rect, GetStockObject(BLACK_BRUSH));
		xw -= DX;
		yw -= DY;
		}
	else
		{
		xw = 0;
		yw = 0;
		}

	/* This draws the bitmap in the lower right corner of the control */
	GetClientRect(hwnd, &rect);

	StretchBlt(hdc, rect.right - bmp.bmWidth - xw, rect.bottom - bmH - yw, 
	           bmp.bmWidth, bmH, hdcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, rop);

	SelectObject(hdcMem, hbmpSav);
	DeleteDC(hdcMem);

	DeleteObject(hbmp);
	return (MAKELONG(bmp.bmWidth + 4, bmH + 4));
}


/*  W N D  P R O C  B I L L B O A R D  H E A D L I N E */
/*-------------------------------------------------------------------------
    %%Function: WndProcBillboardHeadline
    %%Contact: PhillipG
    %%Reviewed:

	Draw text according to the point size, font and text embedded in 
	the window text for this control.
-------------------------------------------------------------------------*/
long FAR PASCAL WndProcBillboardHeadline(HWND hwnd, WORD message, WORD wParam, 
		LONG lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	RECT rect;
	SIZE sSize;
	
	switch (message)
		{
	case WM_SETFONT:
		/* Don't want anyone messing with our fonts! */
		return 1;

	case WM_CREATE:
		/* 
		 * The first time only, we need to initialize the headline font.  
		 * We do this by copying the .TTF file to the Windows directory,
		 * creating the destination file name and then calling 
		 * CreateScalableFontResource.  Set the global fFontReady if this
		 * succeeds.
		 */
		if (!fInitFont)
			{
			int cch;
			int cchWin;
			char szSrcTtf[cchSzMax];
			
			fInitFont = fTrue;
			/* 
			 * Locate the TTF file in the DLL directory, and copy it to the
			 * Windows directory.  We must do this because the DLL could be
			 * on a UNC path, which Windows can't handle for a font resource.
			 */
			if ((cch = GetModuleFileName(hinst, (char far *)szSrcTtf, cchSzMax)) != 0 &&
				(cchWin = GetWindowsDirPath((char far *)szTtf, cchSzMax)) != 0)
				{
				char *pch;
				
				/* Replace DLL file name with TTF file name */
				for (pch = szSrcTtf + cch; *pch != '\\'; pch--)
					;
				lstrcpy((char far *)++pch, szSetupTtfFile);

				/*
				 * ACME API GetWindowsDirPath includes the null char when it
				 * returns char count, but GetWindowsDirectory does not.
				 * Decrement cchWin by one to adjust.
				 */
				cchWin--;
				if (szTtf[cchWin - 1] != '\\')
					szTtf[cchWin++] = '\\';
				lstrcpy((char far *)szTtf + cchWin, szSetupTtfFile);

				/* Delete any existing TTF file */

				RemoveFile(szTtf, cmoForce);
				CopyFile(szSrcTtf, szTtf, cmoOverwrite, 0);
				
				if (DoesFileExist(szTtf, femExists))
					{
					lstrcpy(szFor, szTtf);
					lstrcpy((char far *)szFor + cchWin, szSetupForFile);
					
					RemoveFile(szFor, cmoForce);
					fFontReady = CreateScalableFontResource(0,(char far *)szFor, 
					                          (char far *)szTtf, NULL);
					}
				}
			}
		break;
	
	case WM_PAINT:
		if ((hdc = BeginPaint(hwnd, &ps)) != NULL)
			{
			COLORREF col;				
			LOGFONT lf;
			HFONT hfont = NULL;
			HFONT hfontOld = NULL;
			RECT rectUL;
			POINT pt;
			int wPt;
			int cch;
			int dxp;
			int fRemFont;
			int dyp;
			char sz[256];
			DWORD dw = 0;
			TEXTMETRIC tm;
			char far *lpchEnd = sz + 256;
			char far *lpchFont;
			char far *lpch;
			
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

			GetClientRect(hwnd, &rect);
			if (GetSystemMetrics(SM_CYSCREEN) < 450) /* EGA */
				col = GetSysColor(COLOR_WINDOW);
			else
				col = GetSysColor(COLOR_BTNFACE);

			/* Use Arial as a fallback if the custom font isn't available */
			if ((fRemFont = (fFontReady && AddFontResource((char far *)szFor))) == fTrue)
				lstrcpy(lf.lfFaceName, lpchFont);
			else
				lstrcpy(lf.lfFaceName, "Arial");
			
			dxp = rect.right - rect.left;
			dyp = rect.bottom - rect.top;
			
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
				
			/* 
			 * Draw a 3-pixel line under the text of the headline.
			 * size of line is 1/2 dlg width or length of text 
			 */
			GetWindowRect(GetParent(hwnd), &rectUL);
			pt.x = rectUL.left + ((rectUL.right - rectUL.left) / 2);
			pt.y = 0;
			ScreenToClient(hwnd, &pt);
			
			rectUL.top = rect.bottom - 6;
			rectUL.bottom = rectUL.top + 3;
			rectUL.left = rect.left;
			rectUL.right = max(pt.x, (int)sSize.cx);
			EraseRectCol(hdc, &rectUL, NearestColor(hdc, colHeadlineLine));

			SetBkColor(hdc, col);
			SetBkMode(hdc, TRANSPARENT);
			DrawText(hdc, lpch, lstrlen(lpch), &rect, DT_BOTTOM | DT_LEFT | DT_SINGLELINE);

			/* Clean up the font objects */
			if (hfontOld != NULL)
				SelectObject(hdc, hfontOld);
			if (hfont != NULL)
				{
				Assert(hfontOld != NULL);
				DeleteObject(hfont);
				}
			if (fRemFont)
				RemoveFontResource((char far *)szFor);
			
			EndPaint(hwnd, &ps);
			return 0;
			}
		break;
		} 	/* switch */
	return DefWindowProc(hwnd, message, wParam, lParam);
}


/*
**	Purpose:
**	Arguments:
**	Returns:
**	Notes:
**
****************************************************************************/
static BOOL FAR PASCAL FRegisterWinClasses(VOID)
{
	WNDCLASS	wndclass;

	/* If EGA, use COLOR_WINDOW */
	if (hbrBillboard == NULL && GetSystemMetrics(SM_CYSCREEN) >= 450)
		hbrBillboard = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

	/* BBMP_CLASS
	*/
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon			= NULL;
	wndclass.lpszMenuName   = NULL;
	wndclass.lpszClassName  = BBMP_CLASS;
	wndclass.hbrBackground  = hbrBillboard != NULL ? hbrBillboard :
								(HBRUSH)COLOR_WINDOW;
	wndclass.hInstance      = hinst;
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc    = (WNDPROC)BBmpProc;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra 	= 0;

	if (!RegisterClass(&wndclass))
		{
		WNDCLASS wndclassCur;
		DWORD dwErr = GetLastError();

		if (!GetClassInfo(hinst, BBMP_CLASS, &wndclassCur))
			return (fFalse);
		}

	/* HEADLINE_CLASS
	*/
	wndclass.lpszClassName  = HEADLINE_CLASS;
	wndclass.lpfnWndProc    = (WNDPROC)WndProcBillboardHeadline;

#ifdef INEFFICIENT	
	/* NOTE: These fields are the same as previous */
	
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hIcon		   = NULL;
	wndclass.lpszMenuName   = NULL;
    wndclass.hbrBackground  =  
	wndclass.hInstance      = hinst;
	wndclass.style		   = CS_HREDRAW | CS_VREDRAW;
	wndclass.cbClsExtra     = 0;
	wndclass.cbWndExtra     = 0;
#endif

	if (!RegisterClass(&wndclass))
		{
		WNDCLASS wndclassCur;
		DWORD dwErr = GetLastError();

		if (!GetClassInfo(hinst, HEADLINE_CLASS, &wndclassCur))
			return (fFalse);
		}

	return (fTrue);
}


/*
**	Purpose:
**		Under Win95 (Chicago) it is required to unregister classes
**		when the .DLL is unloaded.
**	Arguments:
**	Returns:
**	Notes:
**
****************************************************************************/
static BOOL FAR PASCAL FUnregisterWinClasses ( VOID )
{
	if (hbrBillboard != NULL)
		DeleteObject(hbrBillboard);

	if (!UnregisterClass(BBMP_CLASS, hinst))
		{
		return (fFalse);
		}

	if (!UnregisterClass(HEADLINE_CLASS, hinst))
		{
		return (fFalse);
		}

	return (fTrue);
}


