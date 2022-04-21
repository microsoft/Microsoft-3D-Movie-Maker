/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Mac specific graphics routines.

***************************************************************************/
#include "frame.h"
ASSERTNAME


HCLT GPT::_hcltDef;
bool GPT::_fForcePalOnSys;

#ifdef SYMC
ACR kacrBlack(0, 0, 0);
ACR kacrDkGray(0x3F, 0x3F, 0x3F);
ACR kacrGray(0x7F, 0x7F, 0x7F);
ACR kacrLtGray(0xBF, 0xBF, 0xBF);
ACR kacrWhite(kbMax, kbMax, kbMax);
ACR kacrRed(kbMax, 0, 0);
ACR kacrGreen(0, kbMax, 0);
ACR kacrBlue(0, 0, kbMax);
ACR kacrYellow(kbMax, kbMax, 0);
ACR kacrCyan(0, kbMax, kbMax);
ACR kacrMagenta(kbMax, 0, kbMax);
ACR kacrClear(fTrue, fTrue);
ACR kacrInvert(fFalse, fFalse);
#endif //SYMC

/***************************************************************************
	Set the color as the current foreground color.
***************************************************************************/
void ACR::_SetFore(void)
{
	AssertThis(facrRgb | facrIndex);
	SCR scr;
	byte b;

	if (B3Lw(_lu) == kbIndexAcr)
		PmForeColor(B0Lw(_lu));
	else
		{
		b = B2Lw(_lu);
		scr.red = SwHighLow(b, b);
		b = B1Lw(_lu);
		scr.green = SwHighLow(b, b);
		b = B0Lw(_lu);
		scr.blue = SwHighLow(b, b);
		RGBForeColor(&scr);
		}
}


/***************************************************************************
	Set the color as the current background color.
***************************************************************************/
void ACR::_SetBack(void)
{
	AssertThis(facrRgb | facrIndex);
	SCR scr;
	byte b;

	if (B3Lw(_lu) == kbIndexAcr)
		PmBackColor(B0Lw(_lu));
	else
		{
		b = B2Lw(_lu);
		scr.red = SwHighLow(b, b);
		b = B1Lw(_lu);
		scr.green = SwHighLow(b, b);
		b = B0Lw(_lu);
		scr.blue = SwHighLow(b, b);
		RGBBackColor(&scr);
		}
}


/***************************************************************************
	Static method to flush any pending graphics operations.
***************************************************************************/
void GPT::Flush(void)
{
	// does nothing on Mac.
}


/***************************************************************************
	Static method to set the current color table.
	While using fpalIdentity the following cautions apply:

		1) The following indexes are reserved by the system, so shouldn't be used:
			{ 0, 1, 3, 15, 255 } (Mac)
			{ 0 - 9; 246 - 255 } (Win).
		2) While we're in the background, RGB values may get mapped to
			the wrong indexes, so the colors will change when we move
			to the foreground.  The solution is to always use indexed
			based color while using fForceOnSystem.
		3) This should only be called when we are the foreground app.

	REVIEW shonk: Mac: implement fpalInitAnim and fpalAnimate.
***************************************************************************/
void GPT::SetActiveColors(PGL pglclr, ulong grfpal)
{
	AssertNilOrPo(pglclr, 0);
	long cclr, iclr, iv;
	HPAL hpal, hpalOld;
	SCR scr;
	CLR clr;
	HCLT hclt;
	HWND hwnd;

	if (hNil == (hclt = GetCTable(72)))
		goto LFail;
	(*hclt)->ctSeed = GetCTSeed();

	//REVIEW shonk: Mac: does it work to call SetPalette with a nil palette?
	if (pvNil != pglclr && 0 < (cclr = LwMin(256, pglclr->IvMac())))
		{
		if (hNil == (hpal = NewPalette((short)cclr, hNil,
				(grfpal & fpalIdentity) ? pmTolerant | pmExplicit : pmTolerant, 0)))
			{
			DisposeHandle((HN)hclt);
LFail:
			PushErc(ercGfxCantSetPalette);
			return;
			}
		for (iclr = 0; iclr < cclr; iclr++)
			{
			pglclr->Get(iclr, &clr);
			scr.red = SwHighLow(clr.bRed, clr.bRed);
			scr.green = SwHighLow(clr.bGreen, clr.bGreen);
			scr.blue = SwHighLow(clr.bBlue, clr.bBlue);
			SetEntryColor(hpal, (short)iclr, &scr);
			(*hclt)->ctTable[iclr].rgb = scr;
			}
		if (grfpal & fpalIdentity)
			{
			//set the first entry to white (with just pmExplicit)
			scr.red = scr.green = scr.blue = (ushort)-1;
			SetEntryColor(hpal, 0, &scr);
			SetEntryUsage(hpal, 0, pmExplicit, -1);
			(*hclt)->ctTable[0].rgb = scr;

			//set all possible ending entries to black (with just pmExplicit)
			scr.red = scr.green = scr.blue = 0;
			for (iv = 1; iv <= 8; iv <<= 1)
				{
				iclr = (1 << iv) - 1;
				if (iclr < cclr)
					{
					SetEntryColor(hpal, (short)iclr, &scr);
					SetEntryUsage(hpal, (short)iclr, pmExplicit, -1);
					}
				(*hclt)->ctTable[iclr].rgb = scr;
				}
			}
		}
	else
		hpal = hNil;

	hpalOld = GetPalette(PPRT(-1));
	SetPalette(PPRT(-1), hpal, fFalse);

	//activate the palette
	if (hNil != (hwnd = (HWND)FrontWindow()))
		ActivatePalette((PPRT)hwnd);
	else
		{
		//to activate the palette, create a window offscreen and then destroy it
		RCS rcs;

		rcs = qd.screenBits.bounds;
		rcs.top = qd.screenBits.bounds.top + GetMBarHeight() / 2;
		rcs.left = (qd.screenBits.bounds.left + qd.screenBits.bounds.right) / 2;
		rcs.bottom = rcs.top + 1;
		rcs.right = rcs.left + 1;
		hwnd = (HWND)NewCWindow(pvNil, &rcs, (byte *)"\p", fTrue, plainDBox,
			GrafPtr(-1), fTrue, 0);
		if (hNil != hwnd)
			DisposeWindow((PPRT)hwnd);
		}

	if (hNil != hpalOld)
		DisposePalette(hpalOld);
	if (hNil != _hcltDef)
		DisposeHandle((HN)_hcltDef);
	_hcltDef = hclt;
	_fForcePalOnSys = FPure(grfpal & fpalIdentity);
}


/***************************************************************************
	Static method to determine if the main screen supports this depth
	and color status.
***************************************************************************/
bool GPT::FCanScreen(long cbitPixel, bool fColor)
{
	if (cbitPixel == 24)
		cbitPixel = 32;

	//assert that cbitPixel is in {1,2,4,8,16,32}
	AssertIn(cbitPixel, 1, 33);
	AssertVar((cbitPixel & (cbitPixel - 1)) == 0, "bad cbitPixel value", &cbitPixel);
	HGD hgd;

	if (hNil == (hgd = GetMainDevice()))
		return fFalse;
	return HasDepth(hgd, (short)cbitPixel, 1, fColor ? 1 : 0) != 0;
}


/***************************************************************************
	Static method to attempt to set the depth and/or color status of the
	main screen (the one with the menu bar).
***************************************************************************/
bool GPT::FSetScreenState(long cbitPixel, bool tColor)
{
	if (cbitPixel == 24)
		cbitPixel = 32;

	//assert that cbitPixel is in {0,1,2,4,8,16,32}
	AssertIn(cbitPixel, 0, 33);
	AssertVar((cbitPixel & (cbitPixel - 1)) == 0, "bad cbitPixel value", &cbitPixel);
	AssertT(tColor);
	HGD hgd;
	short swT;

	if (hNil == (hgd = GetMainDevice()))
		return fFalse;
	if (0 == cbitPixel)
		cbitPixel = (*(*hgd)->gdPMap)->pixelSize;
	swT = (tColor == tYes ||
		tColor == tMaybe && TestDeviceAttribute(hgd, gdDevType)) ? 1 : 0;
	if (0 == HasDepth(hgd, (short)cbitPixel, 1, swT))
		return fFalse;
	return (noErr == SetDepth(hgd, (short)cbitPixel, 1, swT));
}


/***************************************************************************
	Static method to get the state of the main screen.
***************************************************************************/
void GPT::GetScreenState(long *pcbitPixel, bool *pfColor)
{
	AssertVarMem(pcbitPixel);
	AssertVarMem(pfColor);
	HGD hgd;

	if (hNil == (hgd = GetMainDevice()))
		{
		*pcbitPixel = 0;
		*pfColor = fFalse;
		}
	else
		{
		*pcbitPixel = (*(*hgd)->gdPMap)->pixelSize;
		*pfColor = FPure(TestDeviceAttribute(hgd, gdDevType));
		}
}


/***************************************************************************
	Static method to create a new GPT.
***************************************************************************/
PGPT GPT::PgptNew(PPRT pprt, HGD hgd)
{
	AssertVarMem(pprt);
	AssertNilOrVarMem(hgd);
	PGPT pgpt;

	if (pvNil == (pgpt = NewObj GPT))
		return pvNil;

	pgpt->_pprt = pprt;
	pgpt->_hgd = hgd;
	AssertThis(0);
}


/***************************************************************************
	Destructor for a port.
***************************************************************************/
GPT::~GPT(void)
{
	if (_fOffscreen)
		{
		Assert(_cactLock == 0, "pixels are still locked for GWorld being freed");
		if (hNil != _hpic)
			{
			Set(pvNil);
			ClosePicture();
			Restore();
			KillPicture(_hpic);
			_hpic = hNil;
			}
		DisposeGWorld((PGWR)_pprt);
		}
	ReleasePpo(&_pregnClip);
}


/***************************************************************************
	Return the clut that should be used for off-screen GPT's.
***************************************************************************/
HCLT GPT::_HcltUse(long cbitPixel)
{
	HGD hgd;
	HCLT hclt;

	if (cbitPixel > 8)
		return hNil;

	if (_fForcePalOnSys)
		{
		if (hNil == (hgd = GetMainDevice()) ||
				hNil == (hclt = (*(*hgd)->gdPMap)->pmTable))
			{
			Warn("Can't get Main Device's color table");
			}
		else if ((*hclt)->ctSize + 1 >= (1 << cbitPixel))
			return hclt;
		}
	return _hcltDef;
}


/***************************************************************************
	Static method to create an offscreen port.
***************************************************************************/
PGPT GPT::PgptNewOffscreen(RC *prc, long cbitPixel)
{
	AssertVarMem(prc);
	Assert(!prc->FEmpty(), "empty rc for offscreen");
	PGWR pgwr;
	RCS rcs;
	PGPT pgpt;

	if (cbitPixel == 24)
		cbitPixel = 32;

	//assert that cbitPixel is in {1,2,4,8,16,32}
	AssertIn(cbitPixel, 1, 33);
	AssertVar((cbitPixel & (cbitPixel - 1)) == 0, "bad cbitPixel value", &cbitPixel);

	rcs = *prc;
	if (noErr != NewGWorld(&pgwr, (short)cbitPixel, &rcs, _HcltUse(cbitPixel), hNil, 0))
		return pvNil;

	if (pvNil == (pgpt = PgptNew((PPRT)pgwr)))
		DisposeGWorld(pgwr);
	else
		{
		pgpt->_fOffscreen = fTrue;
		pgpt->_rcOff = *prc;
		pgpt->_cbitPixel = (short)cbitPixel;
		}

	return pgpt;
}


/***************************************************************************
	If this is an offscreen bitmap, return the pointer to the pixels and
	optionally get the bounds. Must balance with a call to Unlock().
***************************************************************************/
byte *GPT::PrgbLockPixels(RC *prc)
{
	AssertThis(0);
	AssertNilOrVarMem(prc);
	HPIX hpix;

	if (!_fOffscreen)
		return pvNil;

	Lock();
	hpix = _Hpix();
	if (pvNil != prc)
		*prc = _rcOff;

	return (byte *)(*hpix)->baseAddr;
}


/***************************************************************************
	If this is an offscreen bitmap, return the number of bytes per row.
***************************************************************************/
long GPT::CbRow(void)
{
	AssertThis(0);
	HPIX hpix;

	if (!_fOffscreen)
		return 0;
	hpix = _Hpix();
	return (*hpix)->rowBytes & 0x7FFF;
}


/***************************************************************************
	If this is an offscreen bitmap, return the number of bits per pixel.
***************************************************************************/
long GPT::CbitPixel(void)
{
	AssertThis(0);

	if (!_fOffscreen)
		return 0;
	return _cbitPixel;
}


/***************************************************************************
	Static method to create a PICT and its an associated GPT.
	This should be balanced with a call to PpicRelease().
***************************************************************************/
PGPT GPT::PgptNewPic(RC *prc)
{
	AssertVarMem(prc);
	Assert(!prc->FEmpty(), "empty rectangle for metafile GPT");
	PGPT pgpt;
	RCS rcs;
	RC rc(0, 0, 1, 1);

	if (pvNil == (pgpt = PgptNewOffscreen(&rc, 8)))
		return pvNil;

	rcs = RCS(*prc);
	pgpt->Set(&rcs);
	pgpt->_rcOff = *prc;
	pgpt->_hpic = OpenPicture(&rcs);
	pgpt->Restore();
	if (hNil == pgpt->_hpic)
		{
		ReleasePpo(&pgpt);
		return pvNil;
		}
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
	RCS rcs;

	if (hNil == _hpic)
		{
		Bug("not a Pict GPT");
		goto LRelease;
		}

	Set(pvNil);
	ClosePicture();
	Restore();
	rcs = (*_hpic)->picFrame;
	if (EmptyRect(&rcs) || pvNil == (ppic = PIC::PpicNew(_hpic, &_rcOff)))
		{
		KillPicture(_hpic);
		_hpic = hNil;
LRelease:
		Release();
		return pvNil;
		}
	_hpic = hNil;
	Release();
	return ppic;
}


/***************************************************************************
	Fill or frame a rectangle.
***************************************************************************/
void GPT::DrawRcs(RCS *prcs, GDD *pgdd)
{
	AssertThis(0);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);
	PFNDRW pfn;

	pfn = (pgdd->grfgdd & fgddFrame) ? (PFNDRW)_FrameRcs : (PFNDRW)_FillRcs;
	_Fill(prcs, pgdd, pfn);
}


/***************************************************************************
	Callback (PFNDRW) to fill a rectangle.
***************************************************************************/
void GPT::_FillRcs(RCS *prcs)
{
	AssertVarMem(prcs);
	PaintRect(prcs);
}


/***************************************************************************
	Callback (PFNDRW) to frame a rectangle.
***************************************************************************/
void GPT::_FrameRcs(RCS *prcs)
{
	AssertVarMem(prcs);
	FrameRect(prcs);
}


/***************************************************************************
	Hilite the rectangle by reversing white and the system hilite color.
***************************************************************************/
void GPT::HiliteRcs(RCS *prcs, GDD *pgdd)
{
	AssertThis(0);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);

	Set(pgdd->prcsClip);
	ForeColor(blackColor);
	pgdd->acrBack._SetBack();
	*(byte *) 0x938 &= 0x7f;	/* use color highlighting */
	InvertRect(prcs);
	Restore();
}


/***************************************************************************
	Fill or frame an oval.
***************************************************************************/
void GPT::DrawOval(RCS *prcs, GDD *pgdd)
{
	AssertThis(0);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);
	PFNDRW pfn;

	pfn = (pgdd->grfgdd & fgddFrame) ? (PFNDRW)_FrameOval : (PFNDRW)_FillOval;
	_Fill(prcs, pgdd, pfn);
}


/***************************************************************************
	Callback (PFNDRW) to fill an oval.
***************************************************************************/
void GPT::_FillOval(RCS *prcs)
{
	AssertVarMem(prcs);
	PaintOval(prcs);
}


/***************************************************************************
	Callback (PFNDRW) to frame an oval.
***************************************************************************/
void GPT::_FrameOval(RCS *prcs)
{
	AssertVarMem(prcs);
	FrameOval(prcs);
}


/***************************************************************************
	Fill or frame a polygon.
***************************************************************************/
void GPT::DrawPoly(HQ hqoly, GDD *pgdd)
{
	AssertThis(0);
	AssertHq(hqoly);
	AssertVarMem(pgdd);
	PFNDRW pfn;

	pfn = (pgdd->grfgdd & fgddFrame) ? (PFNDRW)_FramePoly : (PFNDRW)_FillPoly;
	_Fill(&hqoly, pgdd, pfn);
}


/***************************************************************************
	Callback (PFNDRW) to fill a polygon.
***************************************************************************/
void GPT::_FillPoly(HQ *phqoly)
{
	AssertVarMem(phqoly);
	AssertHq(*phqoly);
	PaintPoly((PolyHandle)*phqoly);
}


/***************************************************************************
	Callback (PFNDRW) to frame a polygon.
***************************************************************************/
void GPT::_FramePoly(HQ *phqoly)
{
	AssertVarMem(phqoly);
	AssertHq(*phqoly);
	FramePoly((PolyHandle)*phqoly);
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
	PTS rgpts[2];

	rgpts[0] = *ppts1;
	rgpts[1] = *ppts2;
	_Fill(rgpts, pgdd, (PFNDRW)_DrawLine);
}


/***************************************************************************
	Callback (PFNDRW) to draw a line.
***************************************************************************/
void GPT::_DrawLine(PTS *prgpts)
{
	AssertPvCb(prgpts, 2 * size(PTS));
	MoveTo(prgpts[0].h, prgpts[0].v);
	LineTo(prgpts[1].h, prgpts[1].v);
}


/***************************************************************************
	Low level routine to fill/frame a shape.
***************************************************************************/
void GPT::_Fill(void *pv, GDD *pgdd, PFNDRW pfn)
{
	ACR acrFore = pgdd->acrFore;

	Set(pgdd->prcsClip);
	if (pgdd->grfgdd & fgddFrame)
		PenSize((short)pgdd->dxpPen, (short)pgdd->dypPen);
	if (pgdd->grfgdd & fgddPattern)
		{
		//pattern fill
		APT apt = pgdd->apt;
		ACR acrBack = pgdd->acrBack;

		//check for a solid pattern
		if (apt.FSolidFore() || acrFore == acrBack)
			goto LSolid;
		if (apt.FSolidBack())
			{
			acrFore = acrBack;
			goto LSolid;
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

		PenPat((Pattern *)apt.rgb);
		if (acrFore == kacrInvert)
			{
			// do (invert, clear)
			PenMode(patXor);
			ForeColor(blackColor);
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
			PenMode(notPatOr);
			acrBack._SetFore();
			(this->*pfn)(pv);
			}
		else
			{
			// do (*, *)
			PenMode(patCopy);
			acrFore._SetFore();
			acrBack._SetBack();
			(this->*pfn)(pv);
			}
		}
	else
		{
		//solid color
LSolid:
		if (acrFore == kacrClear)
			goto LDone; //nothing to do

		PenPat(&qd.black);
		if (acrFore == kacrInvert)
			{
			PenMode(patXor);
			ForeColor(blackColor);
			}
		else
			{
			PenMode(patCopy);
			acrFore._SetFore();
			}
		(this->*pfn)(pv);
		}

LDone:
	Restore();
}


/***************************************************************************
	Scroll the given rectangle.
***************************************************************************/
void GPT::ScrollRcs(RCS *prcs, long dxp, long dyp, GDD *pgdd)
{
	AssertThis(0);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);
	HRGN hrgn;

	Set(pgdd->prcsClip);
	if (hNil == (hrgn = NewRgn()))
		PushErc(ercGfxCantDraw);
	else
		{
		ScrollRect(prcs, (short)dxp, (short)dyp, hrgn);
		DisposeRgn(hrgn);
		}
	Restore();
}


/***************************************************************************
	Draw the text.
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
	RCS *prcs = pvNil;

	if (pdsf->grfont & fontBoxed)
		prcs = &rcs;
	Set(pgdd->prcsClip);
	_GetRcsFromRgch(prcs, prgch, (short)cch, &pts, pdsf);
	acrFore = pgdd->acrFore;
	acrBack = pgdd->acrBack;

	ForeColor(blackColor);
	BackColor(whiteColor);
	if (acrFore == kacrInvert)
		{
		//do (invert, clear)
		TextMode(srcXor);
		MoveTo(pts.h, pts.v);
		DrawText(prgch, 0, (short)cch);
		}
	else if (acrFore != kacrClear)
		{
		acrFore._SetFore();
		if (acrBack != kacrClear && acrBack != kacrInvert)
			{
			TextMode(srcCopy);
			acrBack._SetBack();
			goto LDraw;
			}
		TextMode(srcOr);
		MoveTo(pts.h, pts.v);
		DrawText(prgch, 0, (short)cch);
		ForeColor(blackColor);
		}

	if (acrBack == kacrInvert)
		{
		TextMode(notSrcXor);
		goto LDraw;
		}
	else if (acrBack != kacrClear)
		{
		TextMode(notSrcCopy);
		acrBack._SetFore();
LDraw:
		MoveTo(pts.h, pts.v);
		DrawText(prgch, 0, (short)cch);
		}

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
	Restore();
}


/***************************************************************************
	Get the bounding text rectangle (in port coordinates).
***************************************************************************/
void GPT::GetRcsFromRgch(RCS *prcs, achar *prgch, long cch, PTS pts, DSF *pdsf)
{
	Set(pvNil);
	_GetRcsFromRgch(prcs, prgch, (short)cch, &pts, pdsf);
	Restore();
}


/***************************************************************************
	Set the text properties in the current port and get the bounding
	rectangle for the text.

	-- On input:
		-- ppts : Text origin, relative position is determined by tah and tav.
	-- On output:
		-- ppts : Text origin, relative to (left, baseline).
		-- prcs : Bounding box.

	prcs may be nil (saves a call to TextWidth if tah is tahLeft).
***************************************************************************/
void GPT::_GetRcsFromRgch(RCS *prcs, achar *prgch, short cch, PTS *ppts, DSF *pdsf)
{
	AssertNilOrVarMem(prcs);
	AssertIn(cch, 0, kcbMax);
	AssertPvCb(prgch, cch);
	AssertVarMem(ppts);
	AssertPo(pdsf, 0);

	FontInfo fin;
	short xpLeft, ypTop;
	short dyp;
	short dxp;
	short ftc;

	//REVIEW shonk: avoid small font sizes (the OS will crash) - is this true
	//on newer machines (020 and better)?
	ftc = vntl.FtcFromOnn(pdsf->onn);
	TextFont(ftc);
	TextFace((short)pdsf->grfont);
	TextSize((short)pdsf->dyp);

	dxp = (pvNil == prcs && pdsf->tah == tahLeft) ?
		0 : TextWidth(prgch, 0, cch);

	xpLeft = ppts->h;
	ypTop = ppts->v;
	switch (pdsf->tah)
		{
	case tahCenter:
		xpLeft -= dxp / 2;
		break;

	case tahRight:
		xpLeft -= dxp;
		break;
		}

	GetFontInfo(&fin);
	dyp = fin.ascent + fin.descent;
	switch (pdsf->tav)
		{
	case tavBaseline:
		ypTop -= fin.ascent;
		break;

	case tavCenter:
		ypTop -= dyp / 2;
		break;

	case tavBottom:
		ypTop -= dyp;
		break;
		}

	if (pvNil != prcs)
		{
		prcs->left = xpLeft;
		prcs->top = ypTop;
		prcs->right = xpLeft + dxp;
		prcs->bottom = ypTop + dyp;
		}
	ppts->h = xpLeft;
	ppts->v = ypTop + fin.ascent;
}


/***************************************************************************
	Lock the pixels for the port if this is an offscreen PixMap.
	Must be balanced by a call to Unlock.
***************************************************************************/
void GPT::Lock(void)
{
	if (_fOffscreen && 0 == _cactLock++)
		{
		AssertDo(LockPixels(GetGWorldPixMap((PGWR)_pprt)),
			"couldn't lock gworld pixmap pixels");
		}
}


/***************************************************************************
	Unlock the pixels for the port if this is an offscreen PixMap.
***************************************************************************/
void GPT::Unlock(void)
{
	if (_fOffscreen && 0 >= --_cactLock)
		{
		Assert(0 == _cactLock, "Unmatched Unlock call");
		UnlockPixels(GetGWorldPixMap((PGWR)_pprt));
		_cactLock = 0;
		}
}


/***************************************************************************
	Select our graf-port and device.  Must be balanced by a call to
	Restore.  Set/Restore combinations are nestable for distinct ports
	(but not for the same port).  If this is a picture GPT, intersect
	the clipping with _rcOff.
***************************************************************************/
void GPT::Set(RCS *prcsClip)
{
	HCLT hclt;
	RC rc, rcT;
	RCS rcs;
	HRGN hrgn = hNil;

	Assert(!_fSet, "this port is already set");
	Lock();
	GetGWorld((PGWR *)&_pprtSav, &_hgdSav);
	SetGWorld((PGWR)_pprt, _hgd);

	if (pvNil == prcsClip)
		rc.Max();
	else
		rc = RC(*prcsClip);

	if (hNil != _hpic)
		{
		//in a picture GPT, clip to the bounding rectangle
		rc.FIntersect(&_rcOff);
		}

	if (_fNewClip || rc != _rcClip)
		{
		//have to set the clipping
		if (pvNil == _pregnClip)
			rcT = rc;
		else
			{
			if (!_pregnClip->FIsRc(&rcT) &&
					hNil == (hrgn = _pregnClip->HrgnEnsure()))
				{
				Warn("clipping to region failed");
				}
			rcT.FIntersect(&rc);
			}

		rcs = RCS(rcT);
		ClipRect(&rcs);
		if (hNil != hrgn)
			{
			HRGN hrgnClip = qd.thePort->clipRgn;
			SectRgn(hrgnClip, hrgn, hrgnClip);
			}
		_fNewClip = fFalse;
		_rcClip = rc;
		}

	if (_fOffscreen && hNil != (hclt = _HcltUse(_cbitPixel)))
		{
		HPIX hpix = ((PCPRT)qd.thePort)->portPixMap;
		if ((*(*hpix)->pmTable)->ctSeed != (*hclt)->ctSeed)
			{
			//change the color table without doing any color mapping
			//REVIEW shonk: not sure we want to use UpdateGWorld - it does pixel mapping
			//REVIEW shonk: does UpdateGWorld just copy the color table or does it do
			//other stuff, including changing the seed?
			NewCode();
			long lw;
			RCS rcs = _rcOff;

			//REVIEW shonk: check for errors
			lw = UpdateGWorld((PGWR *)&_pprt, _cbitPixel, &rcs, hclt, hNil, keepLocal);
			}
		}
	_fSet = fTrue;
}


/***************************************************************************
	Restores the saved port and device (from a call to Set).
***************************************************************************/
void GPT::Restore(void)
{
	if (!_fSet)
		{
		Bug("Unmatched restore");
		return;
		}

#ifdef DEBUG
	PPRT pprt;
	HGD hgd;

	GetGWorld((PGWR *)&pprt, &hgd);
	Assert(pprt == _pprt, "why aren't we set - someone didn't restore");
	Assert(hgd == _hgd || _hgd == hNil, "gdevice set wrong");
#endif //DEBUG

	SetGWorld((PGWR)_pprtSav, _hgdSav);
	Unlock();
	_fSet = fFalse;
}


/***************************************************************************
	Return the PixMapHandle for the given port.
***************************************************************************/
HPIX GPT::_Hpix(void)
{
	if (_fOffscreen)
		return GetGWorldPixMap((PGWR)_pprt);
	return ((PCPRT)_pprt)->portPixMap;
}


/***************************************************************************
	Copy bits from pgptSrc to this GPT.
***************************************************************************/
void GPT::CopyPixels(PGPT pgptSrc, RCS *prcsSrc, RCS *prcsDst, GDD *pgdd)
{
	Set(pgdd->prcsClip);
	ForeColor(blackColor);
	BackColor(whiteColor);
	pgptSrc->Lock();
	CopyBits((PBMP)*pgptSrc->_Hpix(), (PBMP)*_Hpix(), prcsSrc, prcsDst, srcCopy, hNil);
	pgptSrc->Unlock();
	Restore();
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

	Set(pgdd->prcsClip);
	DrawPicture(ppic->Hpic(), prcs);
	Restore();
}


/***************************************************************************
	Draw the masked bitmap in the given rectangle with reference point
	*ppts.  pgdd->prcsClip is the clipping rectangle.
***************************************************************************/
void GPT::DrawMbmp(PMBMP pmbmp, RCS *prcs, GDD *pgdd)
{
	AssertThis(0);
	AssertPo(pmbmp, 0);
	AssertVarMem(prcs);
	AssertVarMem(pgdd);
	RC rc, rcT;
	HPIX hpix;

	//REVIEW shonk: fix DrawMbmp to use the rcs.

	pmbmp->GetRc(&rc);
	rc.Offset(prcs->left, prcs->top);
	if (pvNil != pgdd->prcsClip)
		{
		rcT = *pgdd->prcsClip;
		if (!rc.FIntersect(&rcT))
			return;
		}

	if (_cbitPixel == 8)
		{
		rcT = _rcOff;
		Assert(rcT.xpLeft == 0 && rcT.ypTop == 0, "bad _rcOff");
		if (!rc.FIntersect(&rcT))
			return;

		Lock();
		hpix = _Hpix();
		pmbmp->Draw((byte *)(*hpix)->baseAddr, (*hpix)->rowBytes & 0x7FFF, rcT.Dyp(),
			prcs->left, prcs->top, &rc, _pregnClip);
		Unlock();
		}
	else
		{
		//need to create a temporary offscreen GPT for the Mask, set the Mask
		//area to white in this GPT, then create an offscreen GPT for the
		//actual MBMP graphic, then blt these to this GPT.
		PT ptDst;
		PGPT pgpt;
		RCS rcsDst;
		RCS rcsSrc;

		ptDst = rc.PtTopLeft();
		rcsDst = RCS(rc);
		rc.OffsetToOrigin();
		if (pvNil == (pgpt = GPT::PgptNewOffscreen(&rc, 1)))
			{
			Warn("Drawing MBMP failed");
			return;
			}
		Assert(pgpt->_rcOff == rc, 0);
		pgpt->Lock();
		hpix = pgpt->_Hpix();
		pmbmp->DrawMask((byte *)(*hpix)->baseAddr, (*hpix)->rowBytes & 0x7FFF,
			rc.Dyp(), prcs->left - ptDst.xp, prcs->top - ptDst.yp);

		//set the mask bits to black
		Set(pgdd->prcsClip);
		ForeColor(blackColor);
		BackColor(whiteColor);
		rcsSrc = RCS(rc);
		CopyBits((PBMP)*hpix, (PBMP)*_Hpix(), &rcsSrc, &rcsDst, srcBic, hNil);
		Restore();
		pgpt->Unlock();
		ReleasePpo(&pgpt);

		if (pvNil == (pgpt = GPT::PgptNewOffscreen(&rc, 8)))
			{
			Warn("Drawing MBMP failed");
			return;
			}
		pgpt->Set(pvNil);
		EraseRect(&rcsSrc);
		pgpt->Restore();

		if (pvNil != _pregnClip)
			_pregnClip->Offset(-ptDst.xp, -ptDst.yp);
		pgpt->Lock();
		hpix = pgpt->_Hpix();
		pmbmp->Draw((byte *)(*hpix)->baseAddr, (*hpix)->rowBytes & 0x7FFF, rc.Dyp(),
			prcs->left - ptDst.xp, prcs->top - ptDst.yp, &rc, _pregnClip);
		if (pvNil != _pregnClip)
			_pregnClip->Offset(ptDst.xp, ptDst.yp);

		Set(pgdd->prcsClip);
		ForeColor(blackColor);
		BackColor(whiteColor);
		CopyBits((PBMP)*hpix, (PBMP)*_Hpix(), &rcsSrc, &rcsDst, srcOr, hNil);
		Restore();
		pgpt->Unlock();
		ReleasePpo(&pgpt);
		}
}


#ifdef DEBUG
/***************************************************************************
	Test the validity of the port.
***************************************************************************/
void GPT::AssertValid(ulong grf)
{
	GPT_PAR::AssertValid(0);
	AssertIn(_cactRef, 1, kcbMax);
	AssertVarMem(_pprt);
	AssertNilOrVarMem(_hgd);
	AssertNilOrVarMem(_pprtSav);
	AssertNilOrVarMem(_hgdSav);
}


/***************************************************************************
	Static method to mark static GPT memory.
***************************************************************************/
void GPT::MarkStaticMem(void)
{
}
#endif //DEBUG


/***************************************************************************
	Initialize the font table.
***************************************************************************/
bool NTL::FInit(void)
{
	MenuHandle hmenu;
	achar st[kcbMaxSt];
	short ftc;
	long ftcT;
	long cstz, istz;

	hmenu = NewMenu(1001, (byte *)"\pFont");
	AddResMenu(hmenu, 'FONT');
	cstz = CountMItems(hmenu);
	if ((_pgst = GST::PgstNew(size(long), cstz + 1, (cstz + 1) * 15)) == pvNil)
		goto LFail;

	for (istz = 0; istz < cstz; istz++)
		{
		GetItem(hmenu, istz + 1, (byte *)st);
		GetFNum((byte *)st, &ftc);
		ftcT = ftc;
		AssertDo(!_pgst->FFindSt(st, &istz, fgstUserSorted),
			"font already found!");
		if (!_pgst->FInsertSt(istz, st, &ftcT))
			goto LFail;
		}

	//add the system font
	GetFontName(0, (byte *)st);
	ftcT = 0;
	if (!_pgst->FFindSt(st, &_onnSystem, fgstUserSorted) &&
		!_pgst->FInsertSt(_onnSystem, st, &ftcT))
		{
LFail:
		PushErc(ercGfxNoFontList);
		return fFalse;
		}

	_pgst->FEnsureSpace(0, 0, fgrpShrink);
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Return the system font code for this font number.
***************************************************************************/
short NTL::FtcFromOnn(long onn)
{
	AssertThis(0);
	long ftc;

	_pgst->GetExtra(onn, &ftc);
	return (short)ftc;
}


/***************************************************************************
	Return true iff the font is a fixed pitch font.
***************************************************************************/
bool NTL::FFixedPitch(long onn)
{
#ifdef REVIEW //shonk: implement FFixedPitch on Mac
	AssertThis(0);
	Assert(FValidOnn(onn), "bad onn");
	LOGFONT lgf;

	_pgst->GetExtra(onn, &lgf);
	return (lgf.lfPitchAndFamily & 0x03) == FIXED_PITCH;
#else
	return fFalse;
#endif
}


/***************************************************************************
	Create a new rectangular region.  If prc is nil, the region will be
	empty.
***************************************************************************/
bool FCreateRgn(HRGN *phrgn, RC *prc)
{
	AssertVarMem(phrgn);
	AssertNilOrVarMem(prc);

	if (pvNil == (*phrgn = NewRgn()))
		return fFalse;
	if (pvNil != prc && !prc->FEmpty())
		{
		RCS rcs = *prc;
		RectRgn(*phrgn, &rcs);
		}
	return fTrue;
}


/***************************************************************************
	Free the region and set *phrgn to nil.
***************************************************************************/
void FreePhrgn(HRGN *phrgn)
{
	AssertVarMem(phrgn);

	if (*phrgn != hNil)
		{
		DisposeRgn(*phrgn);
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
        SetEmptyRgn(*phrgn);
	else
		{
		RCS rcs = *prc;
		RectRgn(*phrgn, &rcs);
		}
	return fTrue;
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
	UnionRgn(hrgnSrc1, hrgnSrc2, hrgnDst);
	return QDError() == noErr;
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
	SectRgn(hrgnSrc1, hrgnSrc2, hrgnDst);
	if (pvNil != pfEmpty)
		*pfEmpty = EmptyRgn(hrgnDst);
	return QDError() == noErr;
}


/***************************************************************************
	Put hrgnSrc - hrgnSrcSub into hrgnDst.  The parameters need not be
	distinct.  Returns success/failure.
***************************************************************************/
bool FDiffRgn(HRGN hrgnDst, HRGN hrgnSrc, HRGN hrgnSrcSub)
{
	Assert(hNil != hrgnDst, "null dst");
	Assert(hNil != hrgnSrc, "null src");
	Assert(hNil != hrgnSrcSub, "null srcSub");
	DiffRgn(hrgnSrc, hrgnSrcSub, hrgnDst);
	return QDError() == noErr;
}


/***************************************************************************
	Determine if the region is rectangular and put the bounding rectangle
	in *prc (if not nil).
***************************************************************************/
bool FRectRgn(HRGN hrgn, RC *prc)
{
	Assert(hNil != hrgn, "null rgn");
	Assert((*hrgn)->rgnSize >= 10, "bad region");

	if (pvNil != prc)
		{		
		RCS rcs = (*hrgn)->rgnBBox;
		*prc = rcs;
		}
	return (*hrgn)->rgnSize == 10;
}


/***************************************************************************
	Return true iff the region is empty.
***************************************************************************/
bool FEmptyRgn(HRGN hrgn, RC *prc)
{
	Assert(hNil != hrgn, "null rgn");

	if (pvNil != prc)
		{		
		RCS rcs = (*hrgn)->rgnBBox;
		*prc = rcs;
		}
	return EmptyRgn(hrgn);
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


