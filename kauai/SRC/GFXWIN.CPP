/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Windows specific graphics routines.

***************************************************************************/
#include "frame.h"
ASSERTNAME


const SCR kscrBlack = PALETTERGB(0, 0, 0);
const SCR kscrWhite = PALETTERGB(0xFF, 0xFF, 0xFF);


HPAL GPT::_hpal = hNil;
HPAL GPT::_hpalIdentity = hNil;
long GPT::_cclrPal = 0;
CLR *GPT::_prgclr = pvNil;
long GPT::_cactPalCur = 0;
long GPT::_cactFlush = 1;
bool GPT::_fPalettized = fFalse;

#ifdef DEBUG
	bool GPT::_fFlushGdi;
	#define _Flush() \
		if (_fFlushGdi) \
			Flush(); \
		else \
			_cactDraw = _cactFlush
#else
	#define _Flush() _cactDraw = _cactFlush
#endif //DEBUG

HRGN _HrgnNew(RCS *prcs, bool fOval);


/***************************************************************************
	Creates a rectangular or oval region according to fOval.
***************************************************************************/
HRGN _HrgnNew(RCS *prcs, long dxpInset, long dypInset, bool fOval)
{
	AssertVarMem(prcs);
	return fOval ?
		CreateEllipticRgn(prcs->left + dxpInset, prcs->top + dypInset,
			prcs->right - dxpInset, prcs->bottom - dypInset) :
		CreateRectRgn(prcs->left + dxpInset, prcs->top + dypInset,
			prcs->right - dxpInset, prcs->bottom - dypInset);
}


/***************************************************************************
	Get a system color from the abstract color.
***************************************************************************/
SCR ACR::_Scr(void)
{
	AssertThis(facrRgb | facrIndex);
	return B3Lw(_lu) == kbIndexAcr ? PALETTEINDEX(B0Lw(_lu)) :
		PALETTERGB(B2Lw(_lu), B1Lw(_lu), B0Lw(_lu));
}


/***************************************************************************
	Draw a DIB using DrawDibDraw.
***************************************************************************/
void GNV::DrawDib(HDRAWDIB hdd, BITMAPINFOHEADER *pbi, RC *prc)
{
	AssertThis(0);
	AssertVarMem(pbi);
	AssertVarMem(prc);

	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs))
		return;

	_pgpt->DrawDib(hdd, pbi, &rcs, &_gdd);
}


/***************************************************************************
	Static method to flush any pending graphics operations.
***************************************************************************/
void GPT::Flush(void)
{
	GdiFlush();
	_cactFlush++;
}



/***************************************************************************
	These are the standard windows static colors. We use these on
	non-palettized displays.
***************************************************************************/
static PALETTEENTRY _rgpe[20] =
	{
	{    0,    0,    0, 0 },
	{ 0x80,    0,    0, 0 },
	{    0, 0x80,    0, 0 },
	{ 0x80, 0x80,    0, 0 },
	{    0,    0, 0x80, 0 },
	{ 0x80,    0, 0x80, 0 },
	{    0, 0x80, 0x80, 0 },
	{ 0xC0, 0xC0, 0xC0, 0 },
	{ 0xC0, 0xDC, 0xC0, 0 },
	{ 0xA6, 0xCA, 0xF0, 0 },

	{ 0xFF, 0xFB, 0xF0, 0 },
	{ 0xA0, 0xA0, 0xA4, 0 },
	{ 0x80, 0x80, 0x80, 0 },
	{ 0xFF,    0,    0, 0 },
	{    0, 0xFF,    0, 0 },
	{ 0xFF, 0xFF,    0, 0 },
	{    0,    0, 0xFF, 0 },
	{ 0xFF,    0, 0xFF, 0 },
	{    0, 0xFF, 0xFF, 0 },
	{ 0xFF, 0xFF, 0xFF, 0 },

	};


/***************************************************************************
	Static method to set the current color table.
	While using fpalIdentity the following cautions apply:

		1) The following indexes are reserved by the system, so shouldn't be used:
			{ 0, 1, 3, 15, 255 } (Mac)
			{ 0 - 9; 246 - 255 } (Win).
		2) While we're in the background, RGB values may get mapped to
			the wrong indexes, so the colors will change when we move
			to the foreground.  The solution is to always use indexed
			based color while using fpalIdentity.
		3) This should only be called when we are the foreground app.

	fpalInitAnim makes all non-reserved colors animatable.
	fpalAnimate means animate the palette with the new colors, don't do
		a normal palette set.
***************************************************************************/
void GPT::SetActiveColors(PGL pglclr, ulong grfpal)
{
	AssertNilOrPo(pglclr, 0);
	Assert(pvNil == pglclr || pglclr->CbEntry() == size(CLR), "wrong CbEntry");
	byte rgb[size(LOGPALETTE) + 256 * size(PALETTEENTRY)];
	LOGPALETTE *ppal = (LOGPALETTE *)rgb;
	PALETTEENTRY pe;
	long ipe, ipeLim;
	long cclr;
	CLR clr;
	byte rgbT[256 / 8];

	if (pvNil == _prgclr)
		{
		if (hNil == vwig.hdcApp)
			{
			Bug("Setting palette before vwig.hdcApp is set");
			return;
			}
		if (!FAllocPv((void **)&_prgclr, LwMul(256, size(CLR)), fmemNil,
				mprNormal))
			{
			PushErc(ercGfxCantSetPalette);
			return;
			}
		_fPalettized =
			FPure(GetDeviceCaps(vwig.hdcApp, RASTERCAPS) & RC_PALETTE);
		}

	if (grfpal & fpalInitAnim)
		grfpal &= ~fpalAnimate;

	ppal->palVersion = 0x0300;
	if (hNil == _hpal && _fPalettized)
		{
		grfpal &= ~fpalAnimate;

		// hack to make sure we don't share index space with a small palette
		// see the MSDN article "The Palette Manager: How and Why"
		pe.peRed = pe.peGreen = pe.peBlue = 0;
		pe.peFlags = PC_NOCOLLAPSE;
		for (ipe = 0; ipe < 256; ipe++)
			ppal->palPalEntry[ipe] = pe;
		ppal->palNumEntries = 256;
		if (hNil == (_hpal = CreatePalette(ppal)))
			{
			PushErc(ercGfxCantSetPalette);
			return;
			}
		SelectPalette(vwig.hdcApp, _hpal, fTrue);
		RealizePalette(vwig.hdcApp);
		_cclrPal = 256;
		}

	// need to resize the palette and fill in the entries
	if (pglclr == pvNil || 0 == (cclr = LwMin(pglclr->IvMac(), 256)))
		{
		if (grfpal & fpalAnimate)
			return;

		if (!_fPalettized)
			{
			_cclrPal = 0;
			goto LDone;
			}

		if (_cclrPal > 0)
			{
			GetSystemPaletteEntries(vwig.hdcApp, 0, 1, ppal->palPalEntry);
			SetPaletteEntries(_hpal, 0, 1, ppal->palPalEntry);
			AssertDo(ResizePalette(_hpal, 1), "shrinking palette failed");
			_cclrPal = 0;
			}
		SelectPalette(vwig.hdcApp, _hpal, fFalse);
		RealizePalette(vwig.hdcApp);
		goto LDone;
		}

	if (grfpal & fpalAnimate)
		cclr = LwMin(cclr, _cclrPal);
	else if (cclr > _cclrPal && _fPalettized && !ResizePalette(_hpal, cclr))
		{
		PushErc(ercGfxCantSetPalette);
		return;
		}

	ClearPb(rgbT, size(rgbT));
	if (grfpal & fpalIdentity)
		{
		// get the first 10 and last 10 system palette entries
		// (the static colors)
		if (_fPalettized)
			{
			// the screen is palettized - get them from the screen
			GetSystemPaletteEntries(vwig.hdcApp, 0, 10, ppal->palPalEntry);
			GetSystemPaletteEntries(vwig.hdcApp, 246, 10,
				ppal->palPalEntry + 246);
			}
		else
			{
			// the screen is non-palettized - get the standard colors.
			Assert(size(_rgpe[0]) == size(ppal->palPalEntry[0]), 0);
			CopyPb(_rgpe, ppal->palPalEntry, 10 * size(_rgpe[0]));
			CopyPb(_rgpe + 10, ppal->palPalEntry + 246, 10 * size(_rgpe[0]));
			}

		clr.bZero = 0;
		for (ipe = 0; ipe < 256; ipe++)
			{
			if (ipe == 10)
				ipe = 246;
			pe = ppal->palPalEntry[ipe];
			clr.bRed = pe.peRed;
			clr.bGreen = pe.peGreen;
			clr.bBlue = pe.peBlue;
			_prgclr[ipe] = clr;
			rgbT[IbFromIbit(pe.peRed)] |= Fbit(pe.peRed);
			}
		}

	pe.peFlags = (grfpal & (fpalInitAnim | fpalAnimate)) ? PC_RESERVED :
		PC_NOCOLLAPSE;
	ipeLim = LwMin(cclr, 246);
	for (ipe = 10; ipe < ipeLim; ipe++)
		{
		pglclr->Get(ipe, &clr);
		clr.bZero = 0;
		if (rgbT[IbFromIbit(clr.bRed)] & Fbit(clr.bRed))
			{
			// have to tweak the red component
			while (clr.bRed > 0 && (rgbT[IbFromIbit(clr.bRed)] & Fbit(clr.bRed)))
				clr.bRed--;
			while (rgbT[IbFromIbit(clr.bRed)] & Fbit(clr.bRed))
				clr.bRed++;
			}
		_prgclr[ipe] = clr;
		pe.peRed = clr.bRed;
		pe.peGreen = clr.bGreen;
		pe.peBlue = clr.bBlue;
		ppal->palPalEntry[ipe] = pe;
		}

	if (_fPalettized)
		{
		if (grfpal & fpalAnimate)
			{
			// starting at 10 and doing LwMin(cclr - 10, 236) is necessary
			// to work around a stupid GDI bug.
			AnimatePalette(_hpal, 10, LwMin(cclr - 10, 236),
				ppal->palPalEntry + 10);
			}
		else
			{
			SetPaletteEntries(_hpal, 0, cclr, ppal->palPalEntry);
			if (cclr < _cclrPal)
				AssertDo(ResizePalette(_hpal, cclr), "Shrinking palette failed");
			_cclrPal = cclr;
			SelectPalette(vwig.hdcApp, _hpal, fFalse);
			RealizePalette(vwig.hdcApp);
			}
		}
	else
		_cclrPal = cclr;

LDone:
	_cactPalCur++;
	vcactRealize++;
}


/***************************************************************************
	Static method to create a new pglclr containing the current palette.
***************************************************************************/
PGL GPT::PglclrGetPalette(void)
{
	PGL pglclr;

	if (pvNil == (pglclr = GL::PglNew(size(CLR), _cclrPal)))
		return pvNil;

	AssertDo(pglclr->FSetIvMac(_cclrPal), 0);
	CopyPb(_prgclr, pglclr->QvGet(0), LwMul(size(CLR), _cclrPal));
	return pglclr;
}


/***************************************************************************
	We've gotten a WM_QUERYNEWPALETTE or WM_PALETTECHANGED message, so
	select and realize our palette.
***************************************************************************/
long GPT::CclrSetPalette(HWND hwnd, bool fInval)
{
	HDC hdc;
	long lwRet;

	vcactRealize++;
	if (hNil == _hpal)
		return 0;

	hdc = GetDC(hwnd);
	SelectPalette(hdc, _hpal, fFalse);
	if ((lwRet = RealizePalette(hdc)) > 0 && fInval)
		{
		HWND hwndT, hwndT2;

		for (hwndT = hwnd; ; )
			{
			InvalidateRect(hwndT, pvNil, fFalse);
			if (hNil != (hwndT2 = GetWindow(hwndT, GW_CHILD)))
				{
				hwndT = hwndT2;
				continue;
				}
			while (hwndT != hwnd &&
					hNil == (hwndT2 = GetWindow(hwndT, GW_HWNDNEXT)))
				{
				hwndT = GetParent(hwndT);
				if (hwndT == hNil)
					{
					Bug("why doesn't the window have a parent?");
					break;
					}
				}
			if (hwndT == hwnd || hwndT == hNil)
				break;
			hwndT = hwndT2;
			}
		}
	ReleaseDC(hwnd, hdc);
	return lwRet;
}


/***************************************************************************
	Static method to create a new GPT for an HDC.
***************************************************************************/
PGPT GPT::PgptNew(HDC hdc)
{
	Assert(hNil != hdc, "Null hdc");
	PGPT pgpt;

	if (hNil == hdc || pvNil == (pgpt = NewObj GPT))
		return pvNil;

	if (!pgpt->_FInit(hdc))
		ReleasePpo(&pgpt);

	AssertNilOrPo(pgpt, 0);
	return pgpt;
}


/***************************************************************************
	Static method to create a new GPT for a window.
***************************************************************************/
PGPT GPT::PgptNewHwnd(HWND hwnd)
{
	Assert(hNil != hwnd, "Null hwnd");
	HDC hdc;
	PGPT pgpt;

	if (hNil == hwnd || hNil == (hdc = GetDC(hwnd)))
		{
		PushErc(ercOomGdi);
		return pvNil;
		}

	if (pvNil == (pgpt = PgptNew(hdc)))
		{
		ReleaseDC(hwnd, hdc);
		return pvNil;
		}

	pgpt->_hwnd = hwnd;
	AssertPo(pgpt, 0);
	return pgpt;
}


/***************************************************************************
	Initialize the GPT.
***************************************************************************/
bool GPT::_FInit(HDC hdc)
{
	Assert(hNil != hdc, "Null hdc");
	HPEN hpen;
	HBRUSH hbr;
	HFONT hfnt;
	HGDIOBJ hobj;

	_hdc = hdc;
	hpen = (HPEN)GetStockObject(NULL_PEN);
	Assert(hpen != hNil, "GetStockObject(NULL_PEN) failed");
	if (hNil != (hobj = SelectObject(_hdc, hpen)))
		DeleteObject(hobj);

	hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
	Assert(hbr != hNil, "GetStockObject(WHITE_BRUSH) failed");
	if (hNil != (hobj = SelectObject(_hdc, hbr)))
		DeleteObject(hobj);

	hfnt = (HFONT)GetStockObject(SYSTEM_FONT);
	Assert(hfnt != hNil, "GetStockObject(SYSTEM_FONT) failed");
	if (hNil != (hobj = SelectObject(_hdc, hfnt)))
		DeleteObject(hobj);
	_dsf.tah = tahLim;
	_dsf.tav = tavLim;

	_rcClip.Max();
	SelectClipRgn(_hdc, hNil);
	SetPolyFillMode(_hdc, WINDING);
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Destructor for a port.
***************************************************************************/
GPT::~GPT(void)
{
	Assert(_cactLock == 0, "pixels are still locked!");
	if (_fMetaFile)
		{
		if (hNil != _hdc)
			{
			HPIC hpic;

			hpic = CloseEnhMetaFile(_hdc);
			if (hNil != hpic)
				DeleteEnhMetaFile(hpic);
			}
		goto LDeleteObjects;
		}
	if (hNil != _hbmp)
		{
		DeleteDC(_hdc);
		DeleteObject(_hbmp);

LDeleteObjects:
		if (hNil != _hbr)
			DeleteObject(_hbr);
		if (hNil != _hfnt)
			DeleteObject(_hfnt);
		}
	else if (hNil != _hwnd)
		ReleaseDC(_hwnd, _hdc);

	ReleasePpo(&_pregnClip);
}


/***************************************************************************
	Get the system color for this abstract color.
***************************************************************************/
SCR GPT::_Scr(ACR acr)
{
	SCR scr;
	long iclr;
	CLR clr;

	if (!_fMapIndices && _fPalettized)
		return acr._Scr();

	scr = acr._Scr();
	if (B3Lw(scr) != 1 || pvNil == _prgclr)
		return scr;

	iclr = SwLow(scr);
	if (!FIn(iclr, 0, _cclrPal))
		return scr;
	clr = _prgclr[iclr];
	return RGB(clr.bRed, clr.bGreen, clr.bBlue);
}


/***************************************************************************
	Static method to create an offscreen port.
***************************************************************************/
PGPT GPT::PgptNewOffscreen(RC *prc, long cbitPixel)
{
	AssertVarMem(prc);
	Assert(!prc->FEmpty(), "empty rectangle for offscreen");
	PGPT pgpt;
	HDC hdc, hdcT;
	HBMP hbmp, hbmpOld;
	BITMAPINFO *pbmi;
	long cclr, cbRow, lwUsage;
	void *pvBits;

	//assert that cbitPixel is in {1,4,8,16,24,32}
	AssertIn(cbitPixel, 1, 33);
	AssertVar(cbitPixel == 32 || (1L << cbitPixel) & 0x01010112,
		"bad cbitPixel value", &cbitPixel);

	hdc = CreateCompatibleDC(vwig.hdcApp);
	if (hNil == hdc)
		return pvNil;

	if (cbitPixel > 8)
		cclr = 0;
	else
		cclr = 1L << cbitPixel;

	//the three longs are for 16 and 32 bit dibs.
	if (!FAllocPv((void **)&pbmi, size(BITMAPINFO) +
			LwMul(cclr, LwMax(size(short), size(RGBQUAD))) + LwMul(3, size(long)),
			fmemClear, mprNormal))
		{
		goto LFail;
		}

	pbmi->bmiHeader.biSize = size(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth = prc->Dxp();
	pbmi->bmiHeader.biHeight = -prc->Dyp();
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biBitCount = (short)cbitPixel;
	pbmi->bmiHeader.biCompression = (cbitPixel == 16 || cbitPixel == 32) ?
		BI_BITFIELDS : BI_RGB;
	cbRow = LwRoundAway(LwMulDivAway(prc->Dxp(), cbitPixel, 8), size(long));
	pbmi->bmiHeader.biSizeImage = LwMul(prc->Dyp(), cbRow);
	pbmi->bmiHeader.biXPelsPerMeter = 0;
	pbmi->bmiHeader.biYPelsPerMeter = 0;
	pbmi->bmiHeader.biClrUsed = cclr;
	pbmi->bmiHeader.biClrImportant = 0;

	lwUsage = DIB_RGB_COLORS;
	if (cclr == 0)
		{
		long *plw = (long *)&pbmi->bmiColors;

		//set the masks
		if (cbitPixel == 16)
			{
			*plw++ = 0x7C00;
			*plw++ = 0x03E0;
			*plw++ = 0x001F;
			}
		else if (cbitPixel == 32)
			{
			*plw++ = 0x00FF0000;
			*plw++ = 0x0000FF00;
			*plw++ = 0x000000FF;
			}
		}
	else if (cclr == 2)
		{
		pbmi->bmiColors[1].rgbBlue = 0xFF;
		pbmi->bmiColors[1].rgbGreen = 0xFF;
		pbmi->bmiColors[1].rgbRed = 0xFF;
		}
	else
		{
		if (_hpalIdentity == hNil)
			{
			LOGPALETTE *ppal;
			PALETTEENTRY pe;
			long ipe;

			if (!FAllocPv((void **)&ppal, size(LOGPALETTE) +
					256 * size(PALETTEENTRY), fmemNil, mprNormal))
				{
				FreePpv((void **)&pbmi);
				goto LFail;
				}

			ppal->palVersion = 0x0300;
			pe.peRed = pe.peGreen = pe.peBlue = 0;
			pe.peFlags = PC_EXPLICIT;
			for (ipe = 0; ipe < 256; ipe++)
				{
				*(short *)&pe = (short)ipe;
				ppal->palPalEntry[ipe] = pe;
				}
			ppal->palNumEntries = 256;
			_hpalIdentity = CreatePalette(ppal);
			FreePpv((void **)&ppal);
			if (_hpalIdentity == hNil)
				{
				FreePpv((void **)&pbmi);
				goto LFail;
				}
			}
		if (_prgclr != pvNil)
			{
			CopyPb(_prgclr, pbmi->bmiColors,
				LwMul(LwMin(cclr, _cclrPal), size(RGBQUAD)));
			}
		else
			{
			short *psw = (short *)pbmi->bmiColors;
			long isw;

			lwUsage = DIB_PAL_COLORS;
			for (isw = 0; isw < cclr; )
				*psw++ = (short)isw++;
			}
		}

	hdcT = GetDC(hNil);
	Assert(hNil != hdcT, "Failed to GetDC(hNil)");
	hbmp = CreateDIBSection(hdcT, pbmi, lwUsage, &pvBits, hNil, 0);
	ReleaseDC(NULL, hdcT);

	FreePpv((void **)&pbmi);

	if (hNil == hbmp)
		goto LFail;
	if (pvNil == (pgpt = PgptNew(hdc)))
		{
		DeleteObject(hbmp);
LFail:
		DeleteDC(hdc);
		return pvNil;
		}

	AssertDo(hbmpOld = (HBMP)SelectObject(hdc, hbmp), "cannot select bitmap");
	DeleteObject(hbmpOld);

	pgpt->_hbmp = hbmp;
	pgpt->_prgbPixels = (byte *)pvBits;
	pgpt->_cbitPixel = cbitPixel;
	pgpt->_cbRow = cbRow;
	pgpt->_ptBase = prc->PtTopLeft();
	pgpt->_rcOff = *prc;
	pgpt->_rcOff.OffsetToOrigin();
	SelectPalette(hdc, _hpalIdentity, fFalse);
	RealizePalette(hdc);
	pgpt->_cactPal = _cactPalCur;

	return pgpt;
}


/***************************************************************************
	Set the color table of an offscreen GPT.
***************************************************************************/
void GPT::SetOffscreenColors(PGL pglclr)
{
	AssertThis(0);
	AssertNilOrPo(pglclr, 0);

	if (hNil == _hbmp)
		{
		Bug("Can't set the palette of this port!");
		return;
		}

	if (pvNil == pglclr)
		{
		if (!_fOwnPalette)
			return;

		_fOwnPalette = fFalse;
		_cactPal = -1; // so we'll set the colors in _EnsurePalette
		return;
		}

	if (FIn(_cbitPixel, 2, 9))
		{
		SetDIBColorTable(_hdc, 0, LwMin(pglclr->IvMac(), 1L << _cbitPixel),
			(RGBQUAD *)pglclr->PvLock(0));
		pglclr->Unlock();
		_fOwnPalette = fTrue;
		}
}


/***************************************************************************
	If this is an offscreen bitmap, return the pointer to the pixels and
	optionally get the bounds. Must balance with a call to Unlock().
***************************************************************************/
byte *GPT::PrgbLockPixels(RC *prc)
{
	AssertThis(0);
	AssertNilOrVarMem(prc);

	if (hNil == _hbmp)
		return pvNil;

	if (_cactDraw >= _cactFlush)
		{
		Assert(_cactDraw == _cactFlush, "why is _cactDraw > _cactFlush?");
		Flush();
		}
	if (pvNil != prc)
		*prc = _rcOff;
	Lock();
	return _prgbPixels;
}


/***************************************************************************
	If this is an offscreen bitmap, return the number of bytes per row.
***************************************************************************/
long GPT::CbRow(void)
{
	AssertThis(0);

	if (hNil == _hbmp)
		return 0;
	return _cbRow;
}


/***************************************************************************
	Return the number of bits per pixel for the GPT.
***************************************************************************/
long GPT::CbitPixel(void)
{
	AssertThis(0);

	if (hNil == _hbmp && 0 == _cbitPixel &&
			(RC_PALETTE & GetDeviceCaps(_hdc, RASTERCAPS)))
		{
		_cbitPixel = GetDeviceCaps(_hdc, BITSPIXEL) *
			GetDeviceCaps(_hdc, PLANES);
		}

	return _cbitPixel;
}


/***************************************************************************
	Lock the pixels if this is an offscreen bitmap. Must be balanced by a
	call to Unlock.
***************************************************************************/
void GPT::Lock(void)
{
	AssertThis(0);
	_cactLock++;
}


/***************************************************************************
	Unlock the pixels if this is an offscreen bitmap.
***************************************************************************/
void GPT::Unlock(void)
{
	if (--_cactLock < 0)
		{
		Bug("Unmatched unlock call");
		_cactLock = 0;
		}
}


/***************************************************************************
	Static method to create a metafile dc and its an associated GPT.
	This should be balanced with a call to PpicRelease().
***************************************************************************/
PGPT GPT::PgptNewPic(RC *prc)
{
	AssertVarMem(prc);
	Assert(!prc->FEmpty(), "empty rectangle for metafile GPT");
	PGPT pgpt;
	RCS rcs;
	HDC hdc;

	// NOTE: have to use nil for the rect because GDI is messed up -
	// the rect is supposed to be in hi-metric but the transform seems
	// to be totally random (scaled by approximately 129/4 in one
	// direction and 125/4 in the other, but this is not exact).
	hdc = CreateEnhMetaFile(pvNil, pvNil, pvNil /*&rcs*/, pvNil);
	if (hNil == hdc)
		return pvNil;

	// fill the bounding rectangle with the null brush so the metafile
	// is at least the given size.
	rcs = *prc;
	rcs.right++;	//empirical GDI hack
	rcs.bottom++;
	FillRect(hdc, &rcs, (HBRUSH)GetStockObject(NULL_BRUSH));

	if (pvNil == (pgpt = PgptNew(hdc)))
		{
		HPIC hpic;
		if (hNil != (hpic = CloseEnhMetaFile(hdc)))
			DeleteEnhMetaFile(hpic);
		return pvNil;
		}

	pgpt->_fMetaFile = fTrue;
	pgpt->_rcOff = *prc;
	return pgpt;
}


/***************************************************************************
	Closes a metafile based GPT and returns the picture produced from
	drawing into the GPT.
***************************************************************************/
PPIC GPT::PpicRelease(void)
{
	AssertThis(0);
	PPIC ppic;
	HPIC hpic;

	if (!_fMetaFile)
		{
		Bug("not a MetaFile GPT");
		goto LFail;
		}
	if (hNil == _hdc)
		{
		Bug("_hdc already nil");
		goto LFail;
		}

	hpic = CloseEnhMetaFile(_hdc);
	_hdc = hNil;
	if (hNil == hpic)
		goto LFail;
	if (pvNil == (ppic = PIC::PpicNew(hpic, &_rcOff)))
		{
		DeleteEnhMetaFile(hpic);
LFail:
		Release();
		return pvNil;
		}
	Release();
	return ppic;
}


/***************************************************************************
	Hilites the rectangle by inverting.
***************************************************************************/
void GPT::HiliteRcs(RCS *prcs, GDD *pgdd)
{
	AssertThis(0);
	_SetClip(pgdd->prcsClip);
	InvertRect(_hdc, prcs);
	_Flush();
}


/***************************************************************************
	Fill/frame a rectangle with a 2-color pattern or solid color.
***************************************************************************/
void GPT::DrawRcs(RCS *prcs, GDD *pgdd)
{
	AssertThis(0);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);
	if (!(pgdd->grfgdd & fgddFrame) ||
			pgdd->dxpPen * 2 >= prcs->right - prcs->left ||
			pgdd->dypPen * 2 >= prcs->bottom - prcs->top)
		{
		//just fill it
		_Fill(prcs, pgdd, (PFNDRW)_FillRcs);
		}
	else
		_FrameRcsOval(prcs, pgdd, fFalse);
}


/***************************************************************************
	Fill/frame a rectangle with a 2-color pattern or solid color.
***************************************************************************/
void GPT::DrawOval(RCS *prcs, GDD *pgdd)
{
	AssertThis(0);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);
	if (!(pgdd->grfgdd & fgddFrame) ||
			pgdd->dxpPen * 2 >= prcs->right - prcs->left ||
			pgdd->dypPen * 2 >= prcs->bottom - prcs->top)
		{
		//just fill it
		_Fill(prcs, pgdd, (PFNDRW)_FillOval);
		}
	else
		_FrameRcsOval(prcs, pgdd, fTrue);
}


/***************************************************************************
	Frames either an RCS or an oval.  Client should have already handled
	the case when the pen is big enough to fill the entire rcs/oval.
***************************************************************************/
void GPT::_FrameRcsOval(RCS *prcs, GDD *pgdd, bool fOval)
{
	Assert((pgdd->grfgdd & fgddFrame) &&
		pgdd->dxpPen * 2 < prcs->right - prcs->left &&
		pgdd->dypPen * 2 < prcs->bottom - prcs->top, "use solid fill");

	// Create a region for the area to fill.  This is faster than
	// using a polygon.
	HRGN hrgnInside, hrgnOutside;

	if (hNil == (hrgnOutside = _HrgnNew(prcs, 0, 0, fOval)))
		{
		hrgnInside = hNil;
		goto LError;
		}

	if (hNil == (hrgnInside = _HrgnNew(prcs, pgdd->dxpPen, pgdd->dypPen, fOval)))
		goto LError;

	switch (CombineRgn(hrgnOutside, hrgnInside, hrgnOutside, RGN_XOR))
		{
	default:
		_Fill(&hrgnOutside, pgdd, (PFNDRW)_FillRgn);
		break;

	case NULLREGION:
		break;

	case ERROR:
LError:
		PushErc(ercOomGdi);
		PushErc(ercGfxCantDraw);
		break;
		}

	if (hNil != hrgnInside)
		DeleteObject(hrgnInside);
	if (hNil != hrgnOutside)
		DeleteObject(hrgnOutside);
}


/***************************************************************************
	Fill/frame a polygon with a 2-color pattern or solid color.
***************************************************************************/
void GPT::DrawPoly(HQ hqoly, GDD *pgdd)
{
	AssertThis(0);
	AssertHq(hqoly);
	AssertVarMem(pgdd);

	_Fill(PvLockHq(hqoly), pgdd, (PFNDRW)_FillPoly);
	UnlockHq(hqoly);
}


/***************************************************************************
	Draw a line.
***************************************************************************/
void GPT::DrawLine(PTS *ppts1, PTS *ppts2, GDD *pgdd)
{
	AssertThis(0);
	AssertVarMem(ppts1);
	AssertVarMem(ppts2);
	AssertVarMem(pgdd);

	//create an OLY to fill
	long dxp, dyp, dypPen;
	PTS *pptsDst;
	byte rgbOly[kcbOlyBase + 6 * size(PTS)];
	OLY *poly = (OLY *)rgbOly;

	poly->cpts = 6;

	if (ppts2->x < ppts1->x)
		{
		//swap them
		PTS *pptsT;

		pptsT = ppts2;
		ppts2 = ppts1;
		ppts1 = pptsT;
		}
	Assert(ppts2->x >= ppts1->x, "points still not in order");

	dxp = ppts2->x - ppts1->x;
	dyp = ppts2->y - ppts1->y;
	dypPen = pgdd->dypPen;

	pptsDst = poly->rgpts;
	*pptsDst = *ppts1;
	if (dyp < 0)
		{
		dypPen = -dypPen;
		pptsDst[1].x = pptsDst[0].x;
		pptsDst[1].y = pptsDst[0].y - dypPen;
		pptsDst++;
		}
	pptsDst[1].x = pptsDst[0].x + pgdd->dxpPen;
	pptsDst[1].y = pptsDst[0].y;
	pptsDst++;
	pptsDst[1].x = pptsDst[0].x + dxp;
	pptsDst[1].y = pptsDst[0].y + dyp;
	pptsDst++;
	pptsDst[1].x = pptsDst[0].x;
	pptsDst[1].y = pptsDst[0].y + dypPen;
	pptsDst++;
	pptsDst[1].x = pptsDst[0].x - pgdd->dxpPen;
	pptsDst[1].y = pptsDst[0].y;
	if (dyp >= 0)
		{
		pptsDst++;
		pptsDst[1].x = pptsDst[0].x - dxp;
		pptsDst[1].y = pptsDst[0].y - dyp;
		}
	Assert(pptsDst == &poly->rgpts[4], "wrote off end of OLY");

	_Fill(poly, pgdd, (PFNDRW)_FillPoly);
}


/***************************************************************************
	Low level call back to fill a rectangle.  Assumes the current pen
	is NULL and the brush is set as desired.
***************************************************************************/
void GPT::_FillRcs(RCS *prcs)
{
	AssertVarMem(prcs);

	Rectangle(_hdc, prcs->left, prcs->top, prcs->right + 1, prcs->bottom + 1);
	_Flush();
}


/***************************************************************************
	Low level call back to fill an oval (ellipse).  Assumes the current
	pen is NULL and the brush is set as desired.
***************************************************************************/
void GPT::_FillOval(RCS *prcs)
{
	AssertVarMem(prcs);

	Ellipse(_hdc, prcs->left, prcs->top, prcs->right + 1, prcs->bottom + 1);
	_Flush();
}


/***************************************************************************
	Low level call back to fill a region.  Assumes the current pen is
	NULL and the brush is set as desired.
***************************************************************************/
void GPT::_FillRgn(HRGN *phrgn)
{
	AssertVarMem(phrgn);

	FillRgn(_hdc, *phrgn, _hbr);
	_Flush();
}


/***************************************************************************
	Low level call back to fill a polygon.  Assumes the current pen is
	NULL and the brush is set as desired.
***************************************************************************/
void GPT::_FillPoly(OLY *poly)
{
	AssertVarMem(poly);

	Polygon(_hdc, poly->rgpts, poly->cpts);
	_Flush();
}


/***************************************************************************
	Make sure the clipping is set to this rectangle and make sure the
	palette is selected.
***************************************************************************/
void GPT::_SetClip(RCS *prcsClip)
{
	AssertNilOrVarMem(prcsClip);
	RC rc, rcT;

	if (pvNil == prcsClip)
		rc.Max();
	else
		rc = RC(*prcsClip);

	if (_fMetaFile)
		{
		//in a metafile GPT, clip to the bounding rectangle
		rc.FIntersect(&_rcOff);
		}

	if (_fNewClip || rc != _rcClip)
		{
		//have to set the clipping
		if (_fNewClip || !_rcClip.FMax())
			{
			//have to first set the clipping to the _pregnClip
			HRGN hrgn;

			if (pvNil == _pregnClip)
				SelectClipRgn(_hdc, hNil);
			else if (_pregnClip->FIsRc(&rcT) ||
					hNil == (hrgn = _pregnClip->HrgnEnsure()))
				{
				rcT.FIntersect(&rc);
				SelectClipRgn(_hdc, hNil);
				IntersectClipRect(_hdc, rcT.xpLeft, rcT.ypTop,
					rcT.xpRight, rcT.ypBottom);
				_rcClip = rc;
				goto LClipSet;
				}
			else
				SelectClipRgn(_hdc, hrgn);
			}

		if (!rc.FMax())
			{
			IntersectClipRect(_hdc, rc.xpLeft, rc.ypTop,
				rc.xpRight, rc.ypBottom);
			}

LClipSet:
		_fNewClip = fFalse;
		_rcClip = rc;
		}

	_EnsurePalette();
}


/***************************************************************************
	Make sure the palette is set up correctly in this GPT.
***************************************************************************/
void GPT::_EnsurePalette(void)
{
	if (_cactPal == _cactPalCur || pvNil == _prgclr)
		return;

	if (_prgbPixels != pvNil)
		{
		if (!_fOwnPalette && FIn(_cbitPixel, 2, 9))
			{
			SetDIBColorTable(_hdc, 0, LwMin(_cclrPal, 1L << _cbitPixel),
				(RGBQUAD *)_prgclr);
			}
		}
	else if (!_fMapIndices && hNil != _hpal)
		{
		if (!SelectPalette(_hdc, _hpal, fFalse))
			_fMapIndices = fTrue;
		else
			RealizePalette(_hdc);
		}
	_cactPal = _cactPalCur;
}


/***************************************************************************
	Low level routine to fill an object with a color.
***************************************************************************/
void GPT::_Fill(void *pv, GDD *pgdd, PFNDRW pfn)
{
	ACR acrFore;

	acrFore = pgdd->acrFore;
	_SetClip(pgdd->prcsClip);

	if (pgdd->grfgdd & fgddPattern)
		{
		//patterned fill
		ACR acrBack = pgdd->acrBack;
		APT apt = pgdd->apt;

		//check for a solid pattern
		if (apt.FSolidFore() || acrFore == acrBack)
			goto LFill;
		if (apt.FSolidBack())
			{
			acrFore = acrBack;
			goto LFill;
			}

		Assert(acrFore != acrBack, "fore and back colors still equal!");
		// Make sure we have one of these forms:
		//   (*, *)
		//   (clear, *)
		//   (invert, *)
		//   (invert, clear)
		if (acrBack == kacrInvert || acrBack == kacrClear && acrFore != kacrInvert)
			{
			// swap them and invert the pattern
			acrFore = acrBack;
			acrBack = pgdd->acrFore;
			apt.Invert();
			}

		//set the correct brush
		_SetAptBrush(&apt);

		if (acrFore == kacrInvert)
			{
			// do (invert, clear)
			SetTextColor(_hdc, kscrWhite);
			SetBkColor(_hdc, kscrBlack);

			SetROP2(_hdc, R2_XORPEN);
			(this->*pfn)(pv);

			if (acrBack != kacrClear)
				{
				// need (invert, *), have already done (invert, clear)
				// so still need to do (clear, *)
				goto LClear;
				}
			}
		else if (acrFore == kacrClear)
			{
LClear:
			// do (clear, *)
			SetTextColor(_hdc, kscrWhite);
			SetBkColor(_hdc, kscrBlack);
			SetROP2(_hdc, R2_MERGENOTPEN);
			(this->*pfn)(pv);

			SetBkColor(_hdc, _Scr(acrBack));
			SetROP2(_hdc, R2_MASKPEN);
			(this->*pfn)(pv);
			}
		else
			{
			// do (*, *)
			SetTextColor(_hdc, _Scr(acrFore));
			SetBkColor(_hdc, _Scr(acrBack));
			SetROP2(_hdc, R2_COPYPEN);
			(this->*pfn)(pv);
			}
		}
	else
		{
		//solid color fill
LFill:
		if (acrFore == kacrClear)
			return; //nothing to do

		if (acrFore == kacrInvert)
			{
			_SetStockBrush(WHITE_BRUSH);
			SetROP2(_hdc, R2_XORPEN);
			}
		else
			{
			_SetAcrBrush(acrFore);
			SetROP2(_hdc, R2_COPYPEN);
			}
		(this->*pfn)(pv);
		}
}


/***************************************************************************
	Scroll a rectangle.
***************************************************************************/
void GPT::ScrollRcs(RCS *prcs, long dxp, long dyp, GDD *pgdd)
{
	AssertThis(0);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);

	_SetClip(pgdd->prcsClip);
	ScrollDC(_hdc, dxp, dyp, prcs, prcs, hNil, pvNil);
	_Flush();
}


/***************************************************************************
	Draw some text.
***************************************************************************/
void GPT::DrawRgch(achar *prgch, long cch, PTS pts, GDD *pgdd, DSF *pdsf)
{
	AssertThis(0);
	AssertIn(cch, 0, kcbMax);
	AssertPvCb(prgch, cch);
	AssertVarMem(pgdd);
	AssertPo(pdsf, 0);

	ACR acrFore, acrBack;
	RCS rcs;

	_SetClip(pgdd->prcsClip);
	acrBack = pgdd->acrBack;
	if (acrBack == kacrInvert || (pdsf->grfont & fontBoxed) ||
			pdsf->tav == tavCenter)
		{
		//GetRcsFromRgch calls _SetTextProps, so we don't need to here
		GetRcsFromRgch(&rcs, prgch, cch, pts, pdsf);
		if (acrBack == kacrInvert)
			{
			GDD gdd = *pgdd;

			acrBack = kacrClear;
			gdd.grfgdd = fgddFill;
			gdd.acrFore = kacrInvert;
			DrawRcs(&rcs, &gdd);
			}
		}
	else
		_SetTextProps(pdsf);

	if (acrBack == kacrClear)
		SetBkMode(_hdc, TRANSPARENT);
	else
		{
		SetBkMode(_hdc, OPAQUE);
		SetBkColor(_hdc, _Scr(acrBack));
		}

	if (pgdd->acrFore == kacrClear || pgdd->acrFore == kacrInvert)
		{
		BugVar("Unsupported DrawRgch() foreground color", &pgdd->acrFore);
		acrFore = kacrBlack;
		}
	else
		acrFore = pgdd->acrFore;
	SetTextColor(_hdc, _Scr(acrFore));

	// Windows won't vertically center via text alignment flags, so we need
	// to do it ourselves.
	if (pdsf->tav == tavCenter)
		pts.y = rcs.top;

	TextOut(_hdc, pts.x, pts.y, prgch, cch);
	_Flush();

	if (pdsf->grfont & fontBoxed)
		{
		GDD gdd = *pgdd;

		gdd.dxpPen = gdd.dypPen = 1;
		gdd.grfgdd = fgddFrame | fgddPattern;
		gdd.apt = vaptGray;
		gdd.apt.MoveOrigin(-_ptBase.xp, -_ptBase.yp);
		gdd.acrBack = kacrClear;
		DrawRcs(&rcs, &gdd);
		}
}


/***************************************************************************
	Get the bounding text rectangle.
***************************************************************************/
void GPT::GetRcsFromRgch(RCS *prcs, achar *prgch, long cch, PTS pts, DSF *pdsf)
{
	AssertThis(0);
	AssertVarMem(prcs);
	AssertIn(cch, 0, kcbMax);
	AssertPvCb(prgch, cch);
	AssertPo(pdsf, 0);

	SIZE dpt;
	TEXTMETRIC txm;
	long dxp, dyp;

	_SetTextProps(pdsf);
	GetTextMetrics(_hdc, &txm);
	txm.tmHeight += txm.tmExternalLeading;
	GetTextExtentPoint32(_hdc, prgch, cch, &dpt);

	switch (pdsf->tav)
		{
	default:
		BugVar("bogus vertical alignment", &pdsf->tav);
		// Fall through.

	case tavTop:
		dyp = 0;
		break;

	case tavCenter:
		dyp = -(txm.tmHeight / 2);
		break;

	case tavBaseline:
		dyp = -txm.tmAscent;
		break;

	case tavBottom:
		dyp = -txm.tmHeight;
		break;
		}

	switch (pdsf->tah)
		{
	default:
		BugVar("bogus horizontal alignment", &pdsf->tah);
		// Fall through.

	case tahLeft:
		dxp = 0;
		break;

	case tahCenter:
		dxp = -dpt.cx / 2;
		break;

	case tahRight:
		dxp = -dpt.cx;
		break;
		}
	prcs->left = pts.x + dxp;
	prcs->right = pts.x + dpt.cx + dxp;
	prcs->top = pts.y + dyp;
	prcs->bottom = pts.y + txm.tmHeight + dyp;
}


/***************************************************************************
	Select a monochrome patterned brush corresponding to the APT.
***************************************************************************/
void GPT::_SetAptBrush(APT *papt)
{
	AssertVarMem(papt);
	short rgw[8];
	long iw;
	HBRUSH hbr;
	HBITMAP hbmp;

	if (_bk == bkApt && _apt == *papt)
		return;

	//create and select the brush
	for (iw = 0; iw < 8; iw++)
		rgw[iw] = ~papt->rgb[iw];
	if (hNil == (hbmp = CreateBitmap(8, 8, 1, 1, rgw)))
		goto LError;
	hbr = CreatePatternBrush(hbmp);
	DeleteObject(hbmp);
	if (hNil == hbr)
		goto LError;
	if (!SelectObject(_hdc, hbr))
		{
		DeleteObject(hbr);
LError:
		PushErc(ercOomGdi);
		return;
		}
	if (hNil != _hbr)
		DeleteObject(_hbr);
	_hbr = hbr;
	_bk = bkApt;
	_apt = *papt;
}


/***************************************************************************
	Select a solid brush corresponding to the acr.
***************************************************************************/
void GPT::_SetAcrBrush(ACR acr)
{
	HBRUSH hbr;

	if (_bk == bkAcr && _acr == acr)
		return;

	if (hNil == (hbr = CreateSolidBrush(_Scr(acr))))
		goto LError;
	if (!SelectObject(_hdc, hbr))
		{
		DeleteObject(hbr);
LError:
		PushErc(ercOomGdi);
		return;
		}
	if (hNil != _hbr)
		DeleteObject(_hbr);
	_hbr = hbr;
	_bk = bkAcr;
	_acr = acr;
}


/***************************************************************************
	Select a stock brush.
***************************************************************************/
void GPT::_SetStockBrush(int wType)
{
	HBRUSH hbr;

	if (_bk == bkStock && _wType == wType)
		return;

	if (hNil == (hbr = (HBRUSH)GetStockObject(wType)))
		goto LError;
	if (!SelectObject(_hdc, hbr))
		{
		DeleteObject(hbr);
LError:
		PushErc(ercOomGdi);
		return;
		}
	if (hNil != _hbr)
		DeleteObject(_hbr);
	_hbr = hbr;
	_bk = bkStock;
	_wType = wType;
}


/***************************************************************************
	Select a font corresponding to the DSF.  Also, set the alignment as
	specified in the DSF.
***************************************************************************/
void GPT::_SetTextProps(DSF *pdsf)
{
	AssertPo(pdsf, 0);
	static int _mptahw[] =
		{
		TA_LEFT,   // tahLeft
		TA_CENTER, // tahCenter
		TA_RIGHT   // tahRight
		};
	static _mptavw[] =
		{
		TA_TOP,      // tavTop
		TA_TOP,      // tavCenter + code
		TA_BASELINE, // tavBaseline
		TA_BOTTOM    // tavBottom
		};

	HFONT hfnt;

	if (_hfnt != hNil && pdsf->onn == _dsf.onn && pdsf->grfont == _dsf.grfont &&
			pdsf->dyp == _dsf.dyp)
		{
		goto LSetAlignment;
		}

	if (hNil == (hfnt = vntl.HfntCreate(pdsf)))
		goto LError;
	if (!SelectObject(_hdc, hfnt))
		{
		DeleteObject(hfnt);
LError:
		PushErc(ercGfxCantSetFont);
		return;
		}
	if (hNil != _hfnt)
		DeleteObject(_hfnt);
	_hfnt = hfnt;
	_dsf.onn = pdsf->onn;
	_dsf.grfont = pdsf->grfont;
	_dsf.dyp = pdsf->dyp;

LSetAlignment:
	// Set the text alignment.
	if (pdsf->tav != _dsf.tav || pdsf->tah != _dsf.tah)
		{
		SetTextAlign(_hdc, _mptahw[pdsf->tah] | _mptavw[pdsf->tav]);
		_dsf.tah = pdsf->tah;
		_dsf.tav = pdsf->tav;
		}
}


/***************************************************************************
	Copy bits from pgptSrc to this GPT.
***************************************************************************/
void GPT::CopyPixels(PGPT pgptSrc, RCS *prcsSrc, RCS *prcsDst, GDD *pgdd)
{
	AssertThis(0);
	AssertPo(pgptSrc, 0);
	AssertVarMem(prcsSrc);
	AssertVarMem(prcsDst);
	AssertVarMem(pgdd);
	long dxpSrc, dxpDst;

	// see if we're doing an offscreen (2x, 2x) stretch or (1x, 2x) stretch -
	// optimize for these
	if (pgptSrc->_cbitPixel == 8 && _cbitPixel == 8 &&
		((dxpSrc = prcsSrc->right - prcsSrc->left) ==
				(dxpDst = prcsDst->right - prcsDst->left) ||
			2 * dxpSrc == dxpDst) &&
		2 * (prcsSrc->bottom - prcsSrc->top) ==
			prcsDst->bottom - prcsDst->top &&
		!pgptSrc->_fOwnPalette && !_fOwnPalette)
		{
		RC rcClip;
		RC rcDouble(*prcsDst);
		RC rcSrc(*prcsSrc);

		if (!rcSrc.FIntersect(&pgptSrc->_rcOff))
			return;

		if (dxpSrc == dxpDst)
			{
			rcDouble.xpLeft += rcSrc.xpLeft - prcsSrc->left;
			rcDouble.xpRight += rcSrc.xpRight - prcsSrc->right;
			}
		else
			{
			rcDouble.xpLeft += 2 * (rcSrc.xpLeft - prcsSrc->left);
			rcDouble.xpRight += 2 * (rcSrc.xpRight - prcsSrc->right);
			}
		rcDouble.ypTop += 2 * (rcSrc.ypTop - prcsSrc->top);
		rcDouble.ypBottom += 2 * (rcSrc.ypBottom - prcsSrc->bottom);

		if (pvNil != pgdd->prcsClip)
			{
			rcClip = *pgdd->prcsClip;
			if (!rcClip.FIntersect(&rcDouble))
				return;
			}
		else
			rcClip = rcDouble;

		if (_cactDraw >= _cactFlush || pgptSrc->_cactDraw >= _cactFlush)
			Flush();

		if (dxpSrc == dxpDst)
			{
			DoubleVertStretch(pgptSrc->_prgbPixels, pgptSrc->_cbRow,
				pgptSrc->_rcOff.Dyp(), &rcSrc,
				_prgbPixels, _cbRow, _rcOff.Dyp(), prcsDst->left, prcsDst->top,
				&rcClip, _pregnClip);
			}
		else
			{
			DoubleStretch(pgptSrc->_prgbPixels, pgptSrc->_cbRow,
				pgptSrc->_rcOff.Dyp(), &rcSrc,
				_prgbPixels, _cbRow, _rcOff.Dyp(), prcsDst->left, prcsDst->top,
				&rcClip, _pregnClip);
			}
		return;
		}

	// see if we're doing an offscreen double vertical stretch, ie 2x in the
	// vertical direction, 1x in horizontal - optimize for this
	if (pgptSrc->_cbitPixel == 8 && _cbitPixel == 8 &&
		2 * (prcsSrc->right - prcsSrc->left) == prcsDst->right - prcsDst->left &&
		2 * (prcsSrc->bottom - prcsSrc->top) == prcsDst->bottom - prcsDst->top &&
		!pgptSrc->_fOwnPalette && !_fOwnPalette)
		{
		RC rcClip;
		RC rcDouble(*prcsDst);
		RC rcSrc(*prcsSrc);

		if (!rcSrc.FIntersect(&pgptSrc->_rcOff))
			return;

		rcDouble.xpLeft += 2 * (rcSrc.xpLeft - prcsSrc->left);
		rcDouble.ypTop += 2 * (rcSrc.ypTop - prcsSrc->top);
		rcDouble.xpRight += 2 * (rcSrc.xpRight - prcsSrc->right);
		rcDouble.ypBottom += 2 * (rcSrc.ypBottom - prcsSrc->bottom);

		if (pvNil != pgdd->prcsClip)
			{
			rcClip = *pgdd->prcsClip;
			if (!rcClip.FIntersect(&rcDouble))
				return;
			}
		else
			rcClip = rcDouble;

		if (_cactDraw >= _cactFlush || pgptSrc->_cactDraw >= _cactFlush)
			Flush();
		DoubleStretch(pgptSrc->_prgbPixels, pgptSrc->_cbRow,
			pgptSrc->_rcOff.Dyp(), &rcSrc,
			_prgbPixels, _cbRow, _rcOff.Dyp(), prcsDst->left, prcsDst->top,
			&rcClip, _pregnClip);
		return;
		}

	SetTextColor(_hdc, kscrWhite);
	SetBkColor(_hdc, kscrBlack);

	_SetClip(pgdd->prcsClip);
	pgptSrc->_EnsurePalette();
	SetStretchBltMode(_hdc, COLORONCOLOR);
	StretchBlt(_hdc, prcsDst->left, prcsDst->top,
			prcsDst->right - prcsDst->left, prcsDst->bottom - prcsDst->top,
		pgptSrc->_hdc, prcsSrc->left, prcsSrc->top,
			prcsSrc->right - prcsSrc->left, prcsSrc->bottom - prcsSrc->top,
		SRCCOPY);

	// we need to set the source's _cactDraw to _cactFlush because its bits
	// are referenced in the GDI queue until a flush occurs - so if we
	// write over them, the wrong stuff gets blasted to the destination.
	pgptSrc->_cactDraw = _cactFlush;
	_Flush();
}


/***************************************************************************
	Draw the picture in the given rectangle.
***************************************************************************/
void GPT::DrawPic(PPIC ppic, RCS *prcs, GDD *pgdd)
{
	AssertThis(0);
	AssertPo(ppic, 0);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);
	int wLevel;

	_SetClip(pgdd->prcsClip);
	if ((wLevel = SaveDC(_hdc)) == 0)
		{
		PushErc(ercGfxCantDraw);
		return;
		}

	PlayEnhMetaFile(_hdc, ppic->Hpic(), prcs);
	RestoreDC(_hdc, wLevel);
	_Flush();
}


/***************************************************************************
	Draw the masked bitmap in the given rectangle.  pgdd->prcsClip is the
	clipping rectangle.
***************************************************************************/
void GPT::DrawMbmp(PMBMP pmbmp, RCS *prcs, GDD *pgdd)
{
	AssertThis(0);
	AssertPo(pmbmp, 0);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);
	RC rcSrc, rcDst, rcClip;

	rcDst = *prcs;
	if (pvNil != pgdd->prcsClip)
		{
		rcClip = *pgdd->prcsClip;
		if (!rcClip.FIntersect(&rcDst))
			return;
		}
	else
		rcClip = rcDst;
	pmbmp->GetRc(&rcSrc);
	if (rcSrc.FEmpty())
		return;

	if (_cbitPixel == 8 && rcSrc.Dxp() == rcDst.Dxp() &&
			rcSrc.Dyp() == rcDst.Dyp())
		{
		Assert(_hbmp != hNil, 0);
		Assert(_rcOff.xpLeft == 0 && _rcOff.ypTop == 0, "bad _rcOff");
		if (!rcClip.FIntersect(&_rcOff))
			return;

		if (_cactDraw >= _cactFlush)
			{
			Assert(_cactDraw == _cactFlush, "why is _cactDraw > _cactFlush?");
			Flush();
			}
		pmbmp->Draw(_prgbPixels, _cbRow, _rcOff.Dyp(),
			rcDst.xpLeft - rcSrc.xpLeft, rcDst.ypTop - rcSrc.ypTop,
			&rcClip, _pregnClip);
		}
	else
		{
		//need to create a temporary offscreen GPT for the Mask, set the Mask
		//area to white in this GPT, then create an offscreen GPT for the
		//actual MBMP graphic, then blt to this GPT.
		PT ptSrc;
		PGPT pgpt;

		SetTextColor(_hdc, kscrWhite);
		SetBkColor(_hdc, kscrBlack);

		ptSrc = rcSrc.PtTopLeft();
		rcSrc.OffsetToOrigin();
		if (pvNil == (pgpt = GPT::PgptNewOffscreen(&rcSrc, 1)))
			{
			Warn("Drawing MBMP failed");
			return;
			}
		Assert(pgpt->_rcOff == rcSrc, 0);
		pmbmp->DrawMask(pgpt->_prgbPixels, pgpt->_cbRow, rcSrc.Dyp(),
			-ptSrc.xp, -ptSrc.yp, &rcSrc);

		//set the mask bits to white
		_SetClip(pgdd->prcsClip);
		StretchBlt(_hdc, rcDst.xpLeft, rcDst.ypTop, rcDst.Dxp(), rcDst.Dyp(),
			pgpt->_hdc, 0, 0, rcSrc.Dxp(), rcSrc.Dyp(), SRCPAINT);
		ReleasePpo(&pgpt);

		if (pvNil == (pgpt = GPT::PgptNewOffscreen(&rcSrc, 8)))
			{
			Warn("Drawing MBMP failed");
			return;
			}
		RCS rcs = rcSrc;
		FillRect(pgpt->_hdc, &rcs, (HBRUSH)GetStockObject(WHITE_BRUSH));

		Flush();
		pmbmp->Draw(pgpt->_prgbPixels, pgpt->_cbRow, rcSrc.Dyp(),
			-ptSrc.xp, -ptSrc.yp, &rcSrc);

		StretchBlt(_hdc, rcDst.xpLeft, rcDst.ypTop, rcDst.Dxp(), rcDst.Dyp(),
			pgpt->_hdc, 0, 0, rcSrc.Dxp(), rcSrc.Dyp(), SRCAND);
		ReleasePpo(&pgpt);
		}
}


/***************************************************************************
	Draw a dib using DrawDibDraw.
***************************************************************************/
void GPT::DrawDib(HDRAWDIB hdd, BITMAPINFOHEADER *pbi, RCS *prcs, GDD *pgdd)
{
	AssertThis(0);
	AssertVarMem(pbi);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);

	SetTextColor(_hdc, kscrWhite);
	SetBkColor(_hdc, kscrBlack);

	_SetClip(pgdd->prcsClip);
	DrawDibDraw(hdd, _hdc, prcs->left, prcs->top,
		prcs->right - prcs->left, prcs->bottom - prcs->top,
		pbi, pvNil, 0, 0, -1, -1, DDF_BACKGROUNDPAL);
	_Flush();
}


#ifdef DEBUG
/***************************************************************************
	Test the validity of the port.
***************************************************************************/
void GPT::AssertValid(ulong grf)
{
	GPT_PAR::AssertValid(0);
	HBRUSH hbr;
	HFONT hfnt;

	AssertIn(_cactRef, 1, kcbMax);
	AssertIn(_cactDraw, 0, _cactFlush + 1);
	if (hNil != _hwnd)
		AssertVar(IsWindow(_hwnd), "Invalid window handle", &_hwnd);
	if (hNil == _hdc)
		{
		Assert(_fMetaFile, "No DC");
		return;
		}

	if (hNil != _hbr)
		{
		hbr = (HBRUSH)SelectObject(_hdc, _hbr);
		if (hbr != hNil)
			AssertVar(hbr == _hbr, "internal brush mismatch", &hbr);
		}
	if (hNil != _hfnt)
		{
		hfnt = (HFONT)SelectObject(_hdc, _hfnt);
		if (hfnt != hNil)
			AssertVar(hfnt == _hfnt, "Internal logical font mismatch", &hfnt);
		}
}


/***************************************************************************
	Static method to mark static GPT memory.
***************************************************************************/
void GPT::MarkStaticMem(void)
{
	MarkPv(_prgclr);
}
#endif //DEBUG


/***************************************************************************
	Enumerate the system fonts and build the font list.
***************************************************************************/
bool NTL::FInit(void)
{
	Assert(hNil != vwig.hdcApp, "No DC");

	LOGFONT lgf;
	HFONT hfnt;

	if (pvNil == (_pgst = GST::PgstNew(offset(LOGFONT, lfFaceName))))
		goto LFail;

	// Make sure to explicitly add the system font since EnumFonts() won't.
	// System font always has font number 0.
	AssertDo(hNil != (hfnt = (HFONT)GetStockObject(SYSTEM_FONT)),
		"Can't get system font");
	AssertDo(GetObject(hfnt, size(lgf), &lgf) != 0, "Can't get logical font");
	if (!_pgst->FAddRgch(lgf.lfFaceName, CchSz(lgf.lfFaceName), &lgf) ||
			!EnumFonts(vwig.hdcApp, pvNil, _FEnumFont, (LPARAM)_pgst))
		{
LFail:
		PushErc(ercGfxNoFontList);
		return fFalse;
		}
	AssertDo(_pgst->FFindRgch(lgf.lfFaceName, CchSz(lgf.lfFaceName),
		&_onnSystem, fgstUserSorted), "system font not found!");

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	-- Font enumuration callback.
	-- If font is TrueType, it is added to the GST.
***************************************************************************/
int CALLBACK _FEnumFont(LOGFONT *plgf, TEXTMETRIC *ptxm, ulong luType,
	LPARAM luParam)
{
	long istz;
	PGST pgst = (PGST)luParam;
	AssertPo(pgst, 0);

	if (luType != TRUETYPE_FONTTYPE)
		return fTrue;

	AssertDo(!pgst->FFindRgch(plgf->lfFaceName, CchSz(plgf->lfFaceName),
		&istz, fgstUserSorted), "font already in list!");
	return pgst->FInsertRgch(istz, plgf->lfFaceName, CchSz(plgf->lfFaceName), plgf);
}


/***************************************************************************
	-- Create a logical GDI font from the given font attributes.
***************************************************************************/
HFONT NTL::HfntCreate(DSF *pdsf)
{
	AssertThis(0);
	AssertPo(pdsf, 0);
	LOGFONT lgf;
	long cch;

	_pgst->GetExtra(pdsf->onn, &lgf);
	_pgst->GetRgch(pdsf->onn, lgf.lfFaceName, size(lgf.lfFaceName) - 1, &cch);
	lgf.lfFaceName[cch] = 0;
	lgf.lfItalic = FPure(pdsf->grfont & fontItalic);
	lgf.lfWeight = (pdsf->grfont & fontBold) ? FW_BOLD : FW_NORMAL;
	lgf.lfUnderline = FPure(pdsf->grfont & fontUnderline);
	lgf.lfHeight = -pdsf->dyp;
	lgf.lfWidth = 0;
	return CreateFontIndirect(&lgf);
}


/***************************************************************************
	Return true iff the font is a fixed pitch font.
***************************************************************************/
bool NTL::FFixedPitch(long onn)
{
	AssertThis(0);
	Assert(FValidOnn(onn), "bad onn");
	LOGFONT lgf;

	_pgst->GetExtra(onn, &lgf);
	return (lgf.lfPitchAndFamily & 0x03) == FIXED_PITCH;
}


/***************************************************************************
	Create a new rectangular region.  If prc is nil, the region will be
	empty.
***************************************************************************/
bool FCreateRgn(HRGN *phrgn, RC *prc)
{
	AssertVarMem(phrgn);
	AssertNilOrVarMem(prc);
	RCS rcs;

	if (pvNil == prc)
		ClearPb(&rcs, size(rcs));
	else
		rcs = *prc;

	*phrgn = CreateRectRgnIndirect(&rcs);
	return *phrgn != hNil;
}


/***************************************************************************
	Free the region and set *phrgn to nil.
***************************************************************************/
void FreePhrgn(HRGN *phrgn)
{
	AssertVarMem(phrgn);

	if (*phrgn != hNil)
		{
		DeleteObject(*phrgn);
		*phrgn = hNil;
		}
}


/***************************************************************************
	Make the region rectangular.  If prc is nil, the region will be empty.
	If *phrgn is hNil, creates the region.  *phrgn may change even if
	*phrgn is not nil.
***************************************************************************/
bool FSetRectRgn(HRGN *phrgn, RC *prc)
{
	AssertVarMem(phrgn);
	AssertNilOrVarMem(prc);

	if (hNil == *phrgn)
		return FCreateRgn(phrgn, prc);
	if (pvNil == prc)
        return SetRectRgn(*phrgn, 0, 0, 0, 0);
	return SetRectRgn(*phrgn, prc->xpLeft, prc->ypTop,
		prc->xpRight, prc->ypBottom);
}


/***************************************************************************
	Put the union of hrgnSrc1 and hrgnSrc2 into hrgnDst.  The parameters
	need not be distinct.  Returns success/failure.
***************************************************************************/
bool FUnionRgn(HRGN hrgnDst, HRGN hrgnSrc1, HRGN hrgnSrc2)
{
	Assert(hNil != hrgnDst, "null dst");
	Assert(hNil != hrgnSrc1, "null src1");
	Assert(hNil != hrgnSrc2, "null src2");
	return ERROR != CombineRgn(hrgnDst, hrgnSrc1, hrgnSrc2, RGN_OR);
}


/***************************************************************************
	Put the intersection of hrgnSrc1 and hrgnSrc2 into hrgnDst.  The parameters
	need not be distinct.  Returns success/failure.
***************************************************************************/
bool FIntersectRgn(HRGN hrgnDst, HRGN hrgnSrc1, HRGN hrgnSrc2, bool *pfEmpty)
{
	Assert(hNil != hrgnDst, "null dst");
	Assert(hNil != hrgnSrc1, "null src1");
	Assert(hNil != hrgnSrc2, "null src2");
	long lw;

    lw = CombineRgn(hrgnDst, hrgnSrc1, hrgnSrc2, RGN_AND);
	if (ERROR == lw)
		return fFalse;
	if (pvNil != pfEmpty)
		*pfEmpty = (lw == NULLREGION);
	return fTrue;
}


/***************************************************************************
	Put hrgnSrc - hrgnSrcSub into hrgnDst.  The parameters need not be
	distinct.  Returns success/failure.
***************************************************************************/
bool FDiffRgn(HRGN hrgnDst, HRGN hrgnSrc, HRGN hrgnSrcSub, bool *pfEmpty)
{
	Assert(hNil != hrgnDst, "null dst");
	Assert(hNil != hrgnSrc, "null src");
	Assert(hNil != hrgnSrcSub, "null srcSub");
	long lw;

	lw = CombineRgn(hrgnDst, hrgnSrc, hrgnSrcSub, RGN_DIFF);
	if (ERROR == lw)
		return fFalse;
	if (pvNil != pfEmpty)
		*pfEmpty = (lw == NULLREGION);
	return fTrue;
}


/***************************************************************************
	Determine if the region is rectangular and put the bounding rectangle
	in *prc (if not nil).
***************************************************************************/
bool FRectRgn(HRGN hrgn, RC *prc)
{
	Assert(hNil != hrgn, "null rgn");
	RCS rcs;
	bool fRet;

	fRet = GetRgnBox(hrgn, &rcs) != COMPLEXREGION;
	if (pvNil != prc)
		*prc = rcs;
	return fRet;
}


/***************************************************************************
	Return true iff the region is empty.
***************************************************************************/
bool FEmptyRgn(HRGN hrgn, RC *prc)
{
	Assert(hNil != hrgn, "null rgn");
	RCS rcs;
	bool fRet;

	fRet = GetRgnBox(hrgn, &rcs) == NULLREGION;
	if (pvNil != prc)
		*prc = rcs;
	return fRet;
}


/***************************************************************************
	Return true iff the two regions are equal.
***************************************************************************/
bool FEqualRgn(HRGN hrgn1, HRGN hrgn2)
{
	Assert(hNil != hrgn1, "null rgn1");
	Assert(hNil != hrgn2, "null rgn2");
	return EqualRgn(hrgn1, hrgn2);
}


