////////////////////////////////////////////////////////////////////////////////
//	File:			OWNRDRAW.CPP
//	Purpose:		Sample code for owner draw buttons and lists
////////////////////////////////////////////////////////////////////////////////


#include "ui-hook.h"


static VOID DrawButtonFace(PDRAWITEMSTRUCT pdis, PRECT prc, HBITMAP hbmp);
static VOID DrawRectRgb(HDC, LONG, LONG, LONG, LONG, DWORD);
static VOID BitmapColorTranslate(HDC hdcBits, BITMAP *pbm, DWORD rgb);
static VOID GetButtonColors(HDC hdc);
static LONG CchThisTab(CHAR *pchCur);
static CHAR *PchNextTab(CHAR *pchCur, LONG cch);


static DWORD rgbButtonHighLt;
static DWORD rgbButtonFocus;
static DWORD rgbButtonFace;
static DWORD rgbButtonText;
static DWORD rgbButtonShadow;


#define EraseButton(hwnd,hdc,prc) ExtTextOut(hdc,0,0,ETO_OPAQUE,prc,NULL,0,NULL)
#define rgbWhite RGB(255,255,255)
#define rgbBlack RGB(0,0,0)
#define DPSoa						0x00A803A9L
#define DSPDxax						0x00E20746L
#define BEVEL    2
#define FRAME    1


BOOL PUBLIC FRunningChicagoUI( VOID )
{
	DWORD dwVersion = GetVersion();

	return ( ((LOWORD(dwVersion) & 0x00ff) >= 0x0004) );

}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Draws the button
//	Arguments:
//		pdis - draw item struct for the button
//		hbmp - handle to bitmap to use
////////////////////////////////////////////////////////////////////////////////
VOID DrawButton(PDRAWITEMSTRUCT pdis, HBITMAP hbmp)
{
	RECT    rc;
	LONG    dx;
	LONG    dy;
	HBRUSH  hbr;
	LONG    i;
	LONG    iFrame,
	        iBevel;

	GetButtonColors(pdis->hDC);
	SetBkColor(pdis->hDC, GetSysColor(COLOR_WINDOW));

	hbr =(HBRUSH)SendMessage(GetParent(pdis->hwndItem), WM_CTLCOLORBTN,
		(WPARAM)pdis->hDC, (LPARAM)pdis->hwndItem);
	FillRect(pdis->hDC, &pdis->rcItem, hbr);

	rc = pdis->rcItem;
	dx = rc.right - rc.left;
	dy = rc.bottom - rc.top;

  iFrame = FRAME;
	iBevel = BEVEL;
	if (pdis->itemState & ODS_FOCUS)
		iFrame *= 2;

  	// Paint Frame.
	if (FRunningChicagoUI())
		{
		iBevel /= 2;
		if (pdis->itemState & ODS_FOCUS)
			{
			DrawRectRgb(pdis->hDC, rc.left + 1, rc.top + 1, dx, 1, rgbBlack);
			DrawRectRgb(pdis->hDC, rc.left + 1, rc.top + 2, 1, dy - 1, rgbBlack);
			}
    	if (pdis->itemState & ODS_SELECTED)
			{
			DrawRectRgb(pdis->hDC, rc.left + 1, rc.bottom - 1, dx - 1, 1, rgbBlack);
    		DrawRectRgb(pdis->hDC, rc.right - 1, rc.top + 1, 1, dy - 1, rgbBlack);
			}
		else
			{
			DrawRectRgb(pdis->hDC, rc.left + 1, rc.bottom - iFrame, dx - 1, iFrame, rgbBlack);
    		DrawRectRgb(pdis->hDC, rc.right - iFrame, rc.top + 1, iFrame, dy - 1, rgbBlack);
			}
		}
	else
		{
		DrawRectRgb(pdis->hDC, rc.left + 1, rc.top, dx - 2, iFrame, rgbBlack);
		DrawRectRgb(pdis->hDC, rc.left, rc.top + 1, iFrame, dy - 2, rgbBlack);
		DrawRectRgb(pdis->hDC, rc.left + 1, rc.bottom - iFrame, dx - 2, iFrame, rgbBlack);
    	DrawRectRgb(pdis->hDC, rc.right - iFrame, rc.top + 1, iFrame, dy - 2, rgbBlack);
		}

    InflateRect(&rc, -iFrame, -iFrame);
    dx = rc.right - rc.left;
    dy = rc.bottom - rc.top;

    SetBkColor(pdis->hDC, rgbButtonFace);

      	// Paint Shadow.
    if (pdis->itemState & ODS_SELECTED)
	    {
        DrawRectRgb(pdis->hDC, rc.left, rc.top, 1, dy, rgbButtonShadow);
        DrawRectRgb(pdis->hDC, rc.left, rc.top, dx, 1, rgbButtonShadow);
		if (FRunningChicagoUI())
			{
        	DrawRectRgb(pdis->hDC, rc.right, rc.top, 1, dy + 1, rgbButtonShadow);
        	DrawRectRgb(pdis->hDC, rc.left, rc.bottom, dx + 1, 1, rgbButtonShadow);
			}
        rc.left += iBevel * 2;
        rc.top  += iBevel * 2;
	    }
	else
    	{
        for (i = 0; i < iBevel; i++)
        	{
            DrawRectRgb(pdis->hDC, rc.right - 1, rc.top, 1, dy, rgbButtonShadow);
            DrawRectRgb(pdis->hDC, rc.left, rc.bottom - 1, dx, 1, rgbButtonShadow);
            DrawRectRgb(pdis->hDC, rc.left, rc.top, 1, dy, rgbButtonHighLt);
            DrawRectRgb(pdis->hDC, rc.left, rc.top, dx, 1, rgbButtonHighLt);
            InflateRect(&rc, -1, -1);
            dx -= 2;
            dy -= 2;
	        }
	    }
	if (FRunningChicagoUI())
		{
		/* On Win95 iBevel is 1/2 as big, so we have to shrink the
		*	rect some more to keep the same size button face area.
		*/
        InflateRect(&rc, -iBevel, -iBevel);
		}

    SetBkColor(pdis->hDC, rgbButtonFace);
    if (pdis->itemState & ODS_FOCUS)
        SetTextColor(pdis->hDC, rgbButtonFocus);
    else
        SetTextColor(pdis->hDC, rgbButtonText);

	DrawButtonFace(pdis, &rc, hbmp);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Renders the bitmap on the button
//	Arguments:
//		pdis - draw item struct for the button
//		prc - rectangle to centre bitmap on
//		hbmp - handle to bitmap to use
////////////////////////////////////////////////////////////////////////////////
static VOID DrawButtonFace(PDRAWITEMSTRUCT pdis, PRECT prc, HBITMAP hbmp)
{
	RECT rc;
	HDC hdcBits;
	BITMAP bm;
	BOOL fMono;

	rc = *prc;
	IntersectClipRect(pdis->hDC, prc->left, prc->top, prc->right, prc->bottom);
	if ((HBITMAP)NULL != hbmp)
		{
		hdcBits = CreateCompatibleDC(pdis->hDC);
		SelectObject(hdcBits, hbmp);
		GetObject(hbmp, sizeof bm, (PSTR)&bm);
		fMono = (bm.bmPlanes == 1) && (bm.bmBitsPixel == 1);

		BitmapColorTranslate(hdcBits, &bm, rgbButtonFace);

		LONG iFrame = 1;

		if ((pdis->itemState & ODS_FOCUS) == 0)
			{
			// Shrink rect to defpushbutton size.
			InflateRect(&rc, iFrame, iFrame);
			iFrame *= 2;
			InflateRect(&rc, -iFrame, -iFrame);
			}

		LONG dx = rc.right - rc.left;
		LONG dy = rc.bottom - rc.top;

		if (dx < bm.bmWidth || dy < bm.bmHeight)
			{
			// Scale bitmap to button size.
			if (dx * bm.bmHeight < dy * bm.bmWidth)
				{
				dy = bm.bmHeight * dx / bm.bmWidth;
				}
			else
				{
				dx = bm.bmWidth * dy / bm.bmHeight;
				}
			}
		else
			{
			dx = bm.bmWidth;
			dy = bm.bmHeight;
			}

		// Center it on the button face.
		rc.left +=(rc.right - rc.left - dx)/ 2;
		rc.top +=(rc.bottom - rc.top - dy)/ 2;
		rc.right = rc.left + dx;
		rc.bottom = rc.top + dy;

		if (pdis->itemState & ODS_DISABLED)
			{
			// initialize the gray brush
			LONG patGray[4];
			LONG i;
			HBITMAP hbmGray;
			HBRUSH hbrGray = (HBRUSH)NULL;
			HBRUSH hbrSav = (HBRUSH)NULL;

			for (i = 0; i < 4; i++)
				patGray[i] = 0xAAAA5555L;
			hbmGray = CreateBitmap(8, 8, 1, 1,(LPSTR)patGray);
			if ((HBITMAP)NULL != hbmGray)
				{
				hbrGray = CreatePatternBrush(hbmGray);
				DeleteObject(hbmGray);
				if ((HBRUSH)NULL != hbrGray)
					hbrSav = (HBRUSH)SelectObject(pdis->hDC, hbrGray);
				}
			SetBkColor(pdis->hDC, RGB(255,255,255));
			SetTextColor(pdis->hDC, RGB(0,0,0));
			StretchBlt(pdis->hDC, rc.left, rc.top, rc.right - rc.left,
				rc.bottom - rc.top, hdcBits, 0, 0, bm.bmWidth,
				bm.bmHeight, DPSoa);
			if ((HBRUSH)NULL != hbrSav)
				SelectObject(pdis->hDC, hbrSav);
			if ((HBRUSH)NULL != hbrGray)
				DeleteObject(hbrGray);
			}
		else
			{
			StretchBlt(pdis->hDC, rc.left, rc.top, rc.right - rc.left,
				rc.bottom - rc.top, hdcBits, 0, 0, bm.bmWidth,
				bm.bmHeight, SRCCOPY);
			}
		DeleteDC(hdcBits);
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Using the first pixel as the "transparent" colour, make all pixels in the
//		hdc that are equal to the "transparent" colour the passed colour.
//	Arguments:
//		hdcBits - device context for the bitmap
//		pbm - pointer to BITMAP struct for the bitmap
//		rgb - background colour to show through
////////////////////////////////////////////////////////////////////////////////
static VOID BitmapColorTranslate(HDC hdcBits, BITMAP *pbm, DWORD rgb)
{
	HDC hdcMask;
	HBITMAP hbmMask;
	HBITMAP hbmT;
	HBRUSH hbrT;
	BOOL fMono;

	// is the bitmap mono, or the first pixel is already equal to the
	// passed colour? if so we have nothing to do.
	fMono = pbm->bmPlanes == 1 && pbm->bmBitsPixel == 1;
	if (fMono || GetPixel(hdcBits, 0, 0) == rgb)
		return;
	// create a mask bitmap and associated DC
	hbmMask = CreateBitmap(pbm->bmWidth, pbm->bmHeight, 1, 1, NULL);
	if ((HBITMAP)NULL != hbmMask)
		{
		hdcMask = CreateCompatibleDC(hdcBits);
		// select the mask bitmap into the mono DC
		hbmT =(HBITMAP)SelectObject(hdcMask, hbmMask);
		// create the brush and select it into the bits DC
		hbrT =(HBRUSH)SelectObject(hdcBits, CreateSolidBrush(rgb));
		// do a colour to mono bitblt to build the mask
		// generate 1's where the source is equal to the background is
		SetBkColor(hdcBits, GetPixel(hdcBits, 0, 0));
		BitBlt(hdcMask, 0, 0, pbm->bmWidth, pbm->bmHeight, hdcBits, 0, 0,
			SRCCOPY);
		// where the mask is 1 lay down the brush,
		// where it is 0 leave the desitnation
		SetBkColor(hdcBits, RGB(255,255,255));
		SetTextColor(hdcBits, RGB(0,0,0));
		BitBlt(hdcBits, 0, 0, pbm->bmWidth, pbm->bmHeight, hdcMask, 0, 0,
			DSPDxax);

		DeleteObject(SelectObject(hdcBits, hbrT));
		DeleteObject(SelectObject(hdcMask, hbmT));

		DeleteDC(hdcMask);
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Fast Solid draw using ExtTextOut
//	Arguments:
//		hdc - device context to draw into
//		x,y,dx,dy - rectangle to bound solid colour
//		rgb - colour to draw
////////////////////////////////////////////////////////////////////////////////
static VOID DrawRectRgb(HDC hdc, LONG x, LONG y, LONG dx, LONG dy, DWORD rgb)
{
	RECT rc;

	SetBkColor(hdc, rgb);
	rc.left = x;
	rc.top = y;
	rc.right = x + dx;
	rc.bottom = y + dy;
	ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		intialize colour values for bitmap buttons.
//	Arguments:
//		hdc - device context to use when initializing colour values
////////////////////////////////////////////////////////////////////////////////
static VOID GetButtonColors(HDC hdc)
{
  rgbButtonHighLt = GetSysColor(COLOR_BTNHIGHLIGHT);
	rgbButtonFace = GetNearestColor(hdc, GetSysColor(COLOR_BTNFACE)) & 0x00FFFFFFL;
	rgbButtonShadow = GetNearestColor(hdc, GetSysColor(COLOR_BTNSHADOW)) & 0x00FFFFFFL;
	rgbButtonText = GetNearestColor(hdc, GetSysColor(COLOR_BTNTEXT)) & 0x00FFFFFFL;
	rgbButtonFocus = GetNearestColor(hdc, RGB(255,255,255)) & 0x00FFFFFFL;
	if (rgbButtonFocus == rgbButtonFace)
		rgbButtonFocus = rgbButtonText;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Draws the button
//	Arguments:
//		pdis - draw item struct for the button
//		ctab - number of tabstops
//		prgtab - array of TAB structs for each tabstop
////////////////////////////////////////////////////////////////////////////////
VOID DrawList(PDRAWITEMSTRUCT pdis, LONG ctab, PTAB prgtab, BOOL fCombo)
{
	HFONT hfont;
	HFONT hfontSav;
	COLORREF rgbTextOld;
	COLORREF rgbBkgrndOld;
	RECT rc;
	CHAR *pchT;
	CHAR rgch[cbMaxSz];
	TEXTMETRIC tm;
	LONG y;
	LONG itab;
	LONG cchT;

	if (-1 != pdis->itemID)
		SendMessage(pdis->hwndItem, fCombo ? CB_GETLBTEXT : LB_GETTEXT,
			pdis->itemID, (LPARAM)rgch);
	else
		rgch[0] = '\0';

	rgbTextOld = SetTextColor(pdis->hDC, GetNearestColor(pdis->hDC,
		GetSysColor(pdis->itemState & ODS_SELECTED ? COLOR_HIGHLIGHTTEXT :
			COLOR_WINDOWTEXT)) & 0x00FFFFFFL);
	rgbBkgrndOld = SetBkColor(pdis->hDC, GetNearestColor(pdis->hDC,
		GetSysColor(pdis->itemState & ODS_SELECTED ? COLOR_HIGHLIGHT :
			COLOR_WINDOW)) & 0x00FFFFFFL);

	if ((HFONT)NULL !=
		(hfont = (HFONT)SendMessage(pdis->hwndItem, WM_GETFONT, 0, 0L)))
		{
		hfontSav = (HFONT)SelectObject(pdis->hDC, hfont);
		}

	pchT = rgch;
	rc = pdis->rcItem;
	// clear the whole rc to ensure erased
	ExtTextOut(pdis->hDC, rc.left, rc.top, ETO_CLIPPED | ETO_OPAQUE, &rc,
		NULL, 0, (LPINT)NULL);
	GdiFlush();
	GetTextMetrics(pdis->hDC, &tm);
	if (tm.tmHeight > rc.bottom - rc.top)
		y = rc.top;
	else
		y = (rc.top + rc.bottom - tm.tmHeight) / 2;
	for (itab = 0; itab < ctab; itab++)
		{
		RECT rcT;
		LONG x;
		UINT ta;
		UINT taSav;

		cchT = CchThisTab(pchT);
		rc.right = rc.left + prgtab[itab].dx * LOWORD(GetDialogBaseUnits());
		IntersectRect(&rcT, &rc, &pdis->rcItem);
		switch (prgtab[itab].ttab)
			{
		case ttabLeft:
			x = rc.left;
			ta = TA_LEFT;
			break;
		case ttabRight:
			x = rc.right;
			ta = TA_RIGHT;
			break;
		case ttabCentre:
			x = rc.left;
			ta = TA_CENTER;
			break;
			}
		taSav = SetTextAlign(pdis->hDC, ta);
		ExtTextOut(pdis->hDC, x, y, ETO_CLIPPED | ETO_OPAQUE, &rcT,
			pchT, cchT, (LPINT)NULL);
		SetTextAlign(pdis->hDC, taSav);
		GdiFlush();
		pchT = PchNextTab(pchT, cchT);
		rc.left = rc.right;
		}
	if (pdis->itemState & ODS_FOCUS)
		{
		DrawFocusRect(pdis->hDC, &pdis->rcItem);
		}

	if ((HFONT)NULL != hfontSav)
		SelectObject(pdis->hDC, hfontSav);
	SetTextColor(pdis->hDC, rgbTextOld);
	SetBkColor(pdis->hDC, rgbBkgrndOld);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Determines the number of characters until the next tab stop
//	Arguments:
//		prgch - pointer to the array of characters
//		pchCur - pointer to the current place in the array
////////////////////////////////////////////////////////////////////////////////
static LONG CchThisTab(CHAR *pchCur)
{
	LONG cch;
	cch = 0;
	while ('\t' != *pchCur && '\0' != *pchCur)
		{
		pchCur++;
		cch++;
		}
	return cch;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Returns the character pointer for the next tab stop
//	Arguments:
//		prgch - pointer to the array of characters
//		pchCur - pointer to the current place in the array
////////////////////////////////////////////////////////////////////////////////
static CHAR *PchNextTab(CHAR *pchCur, LONG cch)
{
	pchCur += cch;
	if ('\t' == *pchCur)
		pchCur++;
	return pchCur;
}
