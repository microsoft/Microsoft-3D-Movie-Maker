/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
**
**	File:			DETVIDEO.C
**	Purpose:		Sample Detection Custom Action Handler
**					routines for Acme Setup.
**	Notes:
**
****************************************************************************/

#define DETVIDEO_C

#include "stdinc.h"
#include <stdlib.h>		/* _MAX_PATH */
#include <string.h>
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"
#include "resource.h"
#include "detvideo.h"

extern HINSTANCE hinst;

RC FAR PASCAL DetectVideo ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData );
void DoTest(PDIB pdib);

long _tAnswer = 0;
CHAR rgchCaption[256];
CHAR rgchText[256];

#define kdxDib 50
#define kdyDib 50

#define kDps (165000 / (kdxDib * kdyDib))
#define kcactDraws 200
#define kdtimeTest (kcactDraws * 1000 / kDps)
#define kcactMulsPerPixel 3

STATIC_FN RC PRIVATE RcInitialize ( PCD pcd, POD pod );

/*
**	Purpose:
**		Custom Action Handler for determining if the machine is fast enough to run
**		3D Movie Maker.
**		Takes no .STF data arguments.
**
**	Arguments:
**		pcd   : Ptr to class data.
**		pod   : Ptr to object data.
**		camf  :	Custom action member function type.
**		pcamfd: Ptr to custom action member function data
**		        (variable size, depends on camf type).
**	Returns:
**		Appropriate RC (return code) value.
**
***************************************************************************/
RC FAR PASCAL DetectVideo ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData )
{
	RC	rc = rcDoDefault;

	Unused(szData);

	switch (camf)
		{
		case camfInitializeObject:
			if (!_tAnswer)
				{
				PDIB pdib;

				if (LoadString(hinst, IDS_MSGCAPTION, rgchCaption, sizeof rgchCaption) != 0)
					{
				    strcpy(rgchCaption, "3D Movie Maker - Profile");
					}

				//
				// Notify user
				//
				if (LoadString(hinst, IDS_VIDEO_TEST_START, rgchText, sizeof rgchText) != 0)
					{
					DoMsgBox(rgchText, rgchCaption, MB_OK);
					}
				else
					{
					DoMsgBox("Video test starting", rgchCaption, MB_OK);
					}

				//
				// Create DIB section.
				//
				pdib = DibCreate(8, kdxDib, kdyDib);
				if (pdib == NULL)
					{
					if (LoadString(hinst, IDS_TEST_OOM, rgchText, sizeof rgchText) != 0)
						{
						DoMsgBox(rgchText, rgchCaption, MB_OK);
						}
					else
						{
						DoMsgBox("Video test -- out of resources, quitting.", rgchCaption, MB_OK);
						}	
					return(rcFail);
					}

				//
				// Now run the test
				//
				DoTest(pdib);

				if (_tAnswer < 0)
					{
					int tResult;

					if (LoadString(hinst, IDS_MACHINE_SLOW, rgchText, sizeof rgchText) != 0)
						{
						tResult = DoMsgBox(rgchText, rgchCaption, MB_OKCANCEL);
						}
					else
						{
						tResult = DoMsgBox("Machine too slow.  Press OK to quit setup, or Cancel to continue setup.", rgchCaption, MB_OKCANCEL);
						}
					return(tResult == 1 ? rcFail : rcOk);
					}
				else
					{
					if (LoadString(hinst, IDS_TEST_COMPLETE, rgchText, sizeof rgchText) != 0)
						{
						DoMsgBox(rgchText, rgchCaption, MB_OK);
						}
					else
						{
						DoMsgBox("Test completed.", rgchCaption, MB_OK);
						}
					}
		
				}
			break;

		default:
			break;
		}

	return (rc);
}


void InitDC(HDC hdc)
{
    SetStretchBltMode(hdc, COLORONCOLOR);
}



PDIB DibCreate(int bits, int dx, int dy)
{
    LPBITMAPINFOHEADER lpbi ;
    DWORD       dwSizeImage;
    int         i;
    DWORD FAR  *pdw;
	LPBYTE		pb;
	int			y, x;

	if (bits != 8)
		return NULL;

    dwSizeImage = dy * dx * sizeof(DWORD);

    lpbi = (PDIB)GlobalAllocPtr(GHND,sizeof(BITMAPINFOHEADER)+dwSizeImage + 1024);

    if (lpbi == NULL)
        return NULL;

    lpbi->biSize            = sizeof(BITMAPINFOHEADER) ;
    lpbi->biWidth           = dx;
    lpbi->biHeight          = dy;
    lpbi->biPlanes          = 1;
    lpbi->biBitCount        = bits ;
    lpbi->biCompression     = BI_RGB ;
    lpbi->biSizeImage       = dwSizeImage;
    lpbi->biXPelsPerMeter   = 0 ;
    lpbi->biYPelsPerMeter   = 0 ;
    lpbi->biClrUsed         = 256;
    lpbi->biClrImportant    = 0 ;

    pdw = (DWORD FAR *)((LPBYTE)lpbi+(int)lpbi->biSize);
	pb = (LPBYTE)DibPtr(lpbi);

    for (i=0; i < 256; i++)
        *pdw++ = RGB(i,0,0);

    for (y=0; y < dy; y++)
        for (x=0; x < dx; x++)
            {
                i = (x + y) % 256;
                *pb++ = i;
            }

    return (PDIB)lpbi;
}



/*
 *  DibDraw()
 */
long DibDraw(HDC hdc,  int x,  int y,
            PDIB pdib, int x0, int y0,
            LONG rop, UINT wUsage)
{
    BOOL f;
    LPVOID lpBits;
	int dx0, dy0;
	long lw2, lwLim;
	long lw;

    if (!pdib)
        return FALSE;

    dx0 = DibWidth(pdib);
    dy0 = DibHeight(pdib);

    lpBits = DibPtr(pdib);

    f = StretchDIBits(
        hdc,
        x,y,
        dx0,dy0,
        x0,y0,
        dx0,dy0,
        lpBits,
		(LPBITMAPINFO)pdib,
        wUsage,
        rop);

	lw = rand();

	lwLim = dx0 * dy0 * kcactMulsPerPixel;

	for (lw2 = 0; lw2 < lwLim; lw2++)
		{
		lw = lw * lw;
		}

    return lw;
}


#define RandPT(pt, rc) ((pt).x = rc.left + (rand() % (rc.right-rc.left))), ((pt).y = rc.top + (rand() % (rc.bottom-rc.top)))

void Draw(HWND hwnd, HDC hdc, PDIB pdib, RECT rc)
{
    POINT pt;
    HDC hMemDC = NULL;

    RandPT(pt, rc);
    pt.x &= ~3;

	DibDraw(hdc, pt.x, pt.y, pdib, 0, 0, SRCCOPY, DIB_RGB_COLORS);
}


void DoTest(PDIB pdib)
{
	int i, Count = kcactDraws;
	int dx, dy;
	HWND hwnd;
	RECT rc;
	HDC hdc;
	LONG time0;

	//
	// Get window size
	//
    dx = GetSystemMetrics (SM_CXSCREEN) * 3 / 4;
    dy = GetSystemMetrics (SM_CYSCREEN) * 3 / 4;

    hwnd = CreateWindow ("STATIC",               // Class name
                            rgchCaption,            // Caption
                            WS_OVERLAPPEDWINDOW,    // Style bits
                            CW_USEDEFAULT, 0,       // Position
                            dx,dy,                  // Size
                            (HWND)NULL,             // Parent window (no parent)
                            (HMENU)NULL,            // use class menu
                            hinst,                  // handle to window instance
                            (LPSTR)NULL             // no params to pass on
                           );

	if (hwnd == NULL)
		{
		_tAnswer = -1;
		return;
		}

	ShowWindow(hwnd, SW_SHOW);

	GetClientRect(hwnd, &rc);

    hdc = GetDC(hwnd);

	if (hdc == NULL)
		{
		_tAnswer = -1;
		return;
		}

	UpdateWindow(hwnd);

    time0 = timeGetTime();
    for (i=0; i<Count; i++)
		{
		Draw(hwnd, hdc, pdib, rc);
		}
    time0 = timeGetTime() - time0;

	ReleaseDC(hwnd, hdc);

	DestroyWindow(hwnd);

	if (time0 > kdtimeTest)
		{
		_tAnswer = -1;
		}
	else
		{
		_tAnswer = 1;
		}

}

