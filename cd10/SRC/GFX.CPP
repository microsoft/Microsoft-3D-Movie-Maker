/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	GFX classes: graphics device (GDV), graphics environment (GNV)

***************************************************************************/
#include "frame.h"
ASSERTNAME


APT vaptGray = { 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55 };
APT vaptLtGray = { 0x22, 0x88, 0x44, 0x11, 0x22, 0x88, 0x44, 0x11 };
APT vaptDkGray = { 0xDD, 0x77, 0xBB, 0xEE, 0xDD, 0x77, 0xBB, 0xEE };

NTL vntl;


RTCLASS(GNV)
RTCLASS(GPT)
RTCLASS(NTL)
RTCLASS(OGN)


const long kdtsMaxTrans = 30 * kdtsSecond;
long vcactRealize;


/***************************************************************************
	Set the ACR from the lw.  The lw should have been returned by a call
	to ACR::LwGet().
***************************************************************************/
void ACR::SetFromLw(long lw)
{
	_lu = (ulong)lw;
	AssertThis(0);
}


/***************************************************************************
	Get an lw from the ACR.  The lw can then be stored on file, reread
	and passed to ACR::SetFromLw.  Valid non-nil colors always return
	non-zero, so zero can be used as a nil value.
***************************************************************************/
long ACR::LwGet(void) const
{
	return (long)_lu;
}


/***************************************************************************
	Get a clr from the ACR.  Asserts that the acr is an rgb color.
***************************************************************************/
void ACR::GetClr(CLR *pclr)
{
	AssertThis(facrRgb);
	AssertVarMem(pclr);

	pclr->bRed = B2Lw(_lu);
	pclr->bGreen = B1Lw(_lu);
	pclr->bBlue = B0Lw(_lu);
	pclr->bZero = 0;
}


#ifdef DEBUG
/***************************************************************************
	Assert that the acr is a valid color.
***************************************************************************/
void ACR::AssertValid(ulong grfacr)
{
	switch (B3Lw(_lu))
		{
	case kbNilAcr:
		Assert(grfacr == facrNil, "unexpected nil ACR");
		break;
	case kbRgbAcr:
		Assert(grfacr == facrNil || (grfacr & facrRgb), "unexpected RGB ACR");
		break;
	case kbIndexAcr:
		Assert(B2Lw(_lu) == 0 && B1Lw(_lu) == 0, "bad Index ACR");
		Assert(grfacr == facrNil || (grfacr & facrIndex), "unexpected Index ACR");
		break;
	case kbSpecialAcr:
		Assert(_lu == kluAcrClear || _lu == kluAcrInvert, "unknown acr");
		Assert(grfacr == facrNil, "unexpected Special ACR");
		break;
	default:
		BugVar("invalid ACR", &_lu);
		break;
		}
}
#endif //DEBUG


/***************************************************************************
	Change the origin on the pattern.
***************************************************************************/
void APT::MoveOrigin(long dxp, long dyp)
{
	//this cast to ulong works because 2^32 is a multiple of 8.
	dxp = (ulong)dxp % 8;
	dyp = (ulong)dyp % 8;
	if (dxp != 0)
		{
		byte *pb;

		for (pb = rgb + 8; pb-- != rgb; )
			*pb = (*pb >> dxp) | (*pb << (8 - dxp));
		}
	if (dyp != 0)
		SwapBlocks(rgb, 8 - dyp, dyp);
}


/***************************************************************************
	Constructor for Graphics environment.
***************************************************************************/
GNV::GNV(GPT *pgpt)
{
	AssertPo(pgpt, 0);

	_Init(pgpt);
	AssertThis(0);
}


/***************************************************************************
	Constructor for Graphics environment based on a pgob.
***************************************************************************/
GNV::GNV(PGOB pgob)
{
	AssertPo(pgob, 0);

	_Init(pgob->Pgpt());	//use the GOB's port
	SetGobRc(pgob);			//set the rc's according to the gob
	AssertThis(0);
}


/***************************************************************************
	Constructor for Graphics environment based on both a port and a pgob.
***************************************************************************/
GNV::GNV(PGOB pgob, PGPT pgpt)
{
	AssertPo(pgpt, 0);
	AssertPo(pgob, 0);

	_Init(pgpt);
	SetGobRc(pgob);
	AssertThis(0);
}


/***************************************************************************
	Destructor for the GNV.
***************************************************************************/
GNV::~GNV(void)
{
	AssertThis(0);
	Mac( _pgpt->Unlock(); )
	ReleasePpo(&_pgpt);
}


/***************************************************************************
	Fill in all fields of the gnv with default values.
***************************************************************************/
void GNV::_Init(PGPT pgpt)
{
	PT pt;

	_pgpt = pgpt;
	pgpt->AddRef();
	Mac( pgpt->Lock(); )
	_rcSrc.Set(0, 0, 1, 1);
	pgpt->GetPtBase(&pt);
	_rcDst.OffsetCopy(&_rcSrc, -pt.xp, -pt.yp);
	_xp = _yp = 0;

	_dsf.onn = vntl.OnnSystem();
	_dsf.grfont = fontNil;
	_dsf.dyp = vpappb->DypTextDef();
	_dsf.tah = tahLeft;
	_dsf.tav = tavTop;

	_gdd.dxpPen = _gdd.dypPen = 1;
	_gdd.prcsClip = pvNil;
	_rcVis.Max();
}


/***************************************************************************
	Set the mapping and vis according to the gob.
***************************************************************************/
void GNV::SetGobRc(PGOB pgob)
{
	RC rc;

	//set the mapping
	pgob->GetRc(&rc, cooGpt);
	SetRcDst(&rc);
	pgob->GetRc(&rc, cooLocal);
	SetRcSrc(&rc);
	pgob->GetRcVis(&rc, cooLocal);
	SetRcVis(&rc);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the gnv
***************************************************************************/
void GNV::AssertValid(ulong grf)
{
	GNV_PAR::AssertValid(0);
	AssertPo(_pgpt, 0);
	AssertPo(&_dsf, 0);
	Assert(!_rcSrc.FEmpty(), "empty src rectangle");
	Assert(!_rcDst.FEmpty(), "empty dst rectangle");
	Assert(_gdd.prcsClip == pvNil || _gdd.prcsClip == &_rcsClip,
		"bad _gdd.prcsClip");
}


/***************************************************************************
	Mark memory for the GNV.
***************************************************************************/
void GNV::MarkMem(void)
{
	AssertValid(0);
	GNV_PAR::MarkMem();
	MarkMemObj(_pgpt);
}
#endif //DEBUG


/***************************************************************************
	Fill a rectangle with a two color pattern.
***************************************************************************/
void GNV::FillRcApt(RC *prc, APT *papt, ACR acrFore, ACR acrBack)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertVarMem(papt);
	AssertPo(&acrFore, 0);
	AssertPo(&acrBack, 0);

	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs))
		return;

	_gdd.grfgdd = fgddFill | fgddPattern;
	_gdd.apt = *papt;
	_gdd.apt.MoveOrigin(_rcDst.xpLeft - _rcSrc.xpLeft, _rcDst.ypTop - _rcSrc.ypTop);
	_gdd.acrFore = acrFore;
	_gdd.acrBack = acrBack;

	_pgpt->DrawRcs(&rcs, &_gdd);
}


/***************************************************************************
	Fill a rectangle with a color.
***************************************************************************/
void GNV::FillRc(RC *prc, ACR acr)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertPo(&acr, 0);

	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs))
		return;

	_gdd.grfgdd = fgddFill;
	_gdd.acrFore = acr;

	_pgpt->DrawRcs(&rcs, &_gdd);
}


/***************************************************************************
	Frame a rectangle with a two color pattern.
***************************************************************************/
void GNV::FrameRcApt(RC *prc, APT *papt, ACR acrFore, ACR acrBack)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertVarMem(papt);
	AssertPo(&acrFore, 0);
	AssertPo(&acrBack, 0);

	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs) || _gdd.dxpPen == 0 && _gdd.dypPen == 0)
		return;

	_gdd.grfgdd = fgddFrame | fgddPattern;
	_gdd.apt = *papt;
	_gdd.apt.MoveOrigin(_rcDst.xpLeft - _rcSrc.xpLeft, _rcDst.ypTop - _rcSrc.ypTop);
	_gdd.acrFore = acrFore;
	_gdd.acrBack = acrBack;

	_pgpt->DrawRcs(&rcs, &_gdd);
}


/***************************************************************************
	Frame a rectangle with a color.
***************************************************************************/
void GNV::FrameRc(RC *prc, ACR acr)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertPo(&acr, 0);

	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs) || _gdd.dxpPen == 0 && _gdd.dypPen == 0)
		return;

	_gdd.grfgdd = fgddFrame;
	_gdd.acrFore = acr;

	_pgpt->DrawRcs(&rcs, &_gdd);
}


/***************************************************************************
	For hilighting text.  On mac, interchanges the system hilite color and
	the background color.  On Win, just inverts.
***************************************************************************/
void GNV::HiliteRc(RC *prc, ACR acrBack)
{
	AssertThis(0);
	AssertVarMem(prc);

	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs))
		return;
	_gdd.acrBack = acrBack;
	_pgpt->HiliteRcs(&rcs, &_gdd);
}


/***************************************************************************
	Fill an oval with a two color pattern.
***************************************************************************/
void GNV::FillOvalApt(RC *prc, APT *papt, ACR acrFore, ACR acrBack)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertVarMem(papt);
	AssertPo(&acrFore, 0);
	AssertPo(&acrBack, 0);

	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs))
		return;

	_gdd.grfgdd = fgddFill | fgddPattern;
	_gdd.apt = *papt;
	_gdd.apt.MoveOrigin(_rcDst.xpLeft - _rcSrc.xpLeft, _rcDst.ypTop - _rcSrc.ypTop);
	_gdd.acrFore = acrFore;
	_gdd.acrBack = acrBack;

	_pgpt->DrawOval(&rcs, &_gdd);
}


/***************************************************************************
	Fill an oval with a color.
***************************************************************************/
void GNV::FillOval(RC *prc, ACR acr)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertPo(&acr, 0);

	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs))
		return;

	_gdd.grfgdd = fgddFill;
	_gdd.acrFore = acr;

	_pgpt->DrawOval(&rcs, &_gdd);
}


/***************************************************************************
	Frame an oval with a two color pattern.
***************************************************************************/
void GNV::FrameOvalApt(RC *prc, APT *papt, ACR acrFore, ACR acrBack)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertVarMem(papt);
	AssertPo(&acrFore, 0);
	AssertPo(&acrBack, 0);

	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs) || _gdd.dxpPen == 0 && _gdd.dypPen == 0)
		return;

	_gdd.grfgdd = fgddFrame | fgddPattern;
	_gdd.apt = *papt;
	_gdd.apt.MoveOrigin(_rcDst.xpLeft - _rcSrc.xpLeft, _rcDst.ypTop - _rcSrc.ypTop);
	_gdd.acrFore = acrFore;
	_gdd.acrBack = acrBack;

	_pgpt->DrawOval(&rcs, &_gdd);
}


/***************************************************************************
	Frame an oval with a color.
***************************************************************************/
void GNV::FrameOval(RC *prc, ACR acr)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertPo(&acr, 0);

	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs) || _gdd.dxpPen == 0 && _gdd.dypPen == 0)
		return;

	_gdd.grfgdd = fgddFrame;
	_gdd.acrFore = acr;

	_pgpt->DrawOval(&rcs, &_gdd);
}


/***************************************************************************
	Draw a line with a pattern.  Sets the pen position to (xp2, yp2).
***************************************************************************/
void GNV::LineApt(long xp1, long yp1, long xp2, long yp2,
	APT *papt, ACR acrFore, ACR acrBack)
{
	AssertThis(0);
	AssertVarMem(papt);
	AssertPo(&acrFore, 0);
	AssertPo(&acrBack, 0);
	PTS pts1, pts2;

	if (_gdd.dxpPen != 0 || _gdd.dypPen != 0)
		{
		_gdd.grfgdd = fgddFrame | fgddPattern;
		_gdd.apt = *papt;
		_gdd.apt.MoveOrigin(_rcDst.xpLeft - _rcSrc.xpLeft, _rcDst.ypTop - _rcSrc.ypTop);
		_gdd.acrFore = acrFore;
		_gdd.acrBack = acrBack;
	
		_MapPtPts(xp1, yp1, &pts1);
		_MapPtPts(xp2, yp2, &pts2);
		_pgpt->DrawLine(&pts1, &pts2, &_gdd);
		}
	_xp = xp2;
	_yp = yp2;
}


/***************************************************************************
	Draw a line in a solid color.  Sets the pen position to (xp2, yp2).
***************************************************************************/
void GNV::Line(long xp1, long yp1, long xp2, long yp2, ACR acr)
{
	AssertThis(0);
	AssertPo(&acr, 0);
	PTS pts1, pts2;

	if (_gdd.dxpPen != 0 || _gdd.dypPen != 0)
		{
		_gdd.grfgdd = fgddFrame;
		_gdd.acrFore = acr;
	
		_MapPtPts(xp1, yp1, &pts1);
		_MapPtPts(xp2, yp2, &pts2);
		_pgpt->DrawLine(&pts1, &pts2, &_gdd);
		}
	_xp = xp2;
	_yp = yp2;
}


/***************************************************************************
	Fill a polygon with a pattern.
***************************************************************************/
void GNV::FillOgnApt(POGN pogn, APT *papt, ACR acrFore, ACR acrBack)
{
	AssertThis(0);
	AssertPo(pogn, 0);
	AssertVarMem(papt);
	AssertPo(&acrFore, 0);
	AssertPo(&acrBack, 0);
	HQ hqoly;

	if ((hqoly = _HqolyCreate(pogn, fognAutoClose)) == hqNil)
		{
		PushErc(ercGfxCantDraw);
		return;
		}

	_gdd.grfgdd = fgddFill | fgddPattern;
	_gdd.apt = *papt;
	_gdd.apt.MoveOrigin(_rcDst.xpLeft - _rcSrc.xpLeft, _rcDst.ypTop - _rcSrc.ypTop);
	_gdd.acrFore = acrFore;
	_gdd.acrBack = acrBack;

	_pgpt->DrawPoly(hqoly, &_gdd);
	FreePhq(&hqoly);
}


/***************************************************************************
	Fill a polygon with a color.
***************************************************************************/
void GNV::FillOgn(POGN pogn, ACR acr)
{
	AssertThis(0);
	AssertPo(pogn, 0);
	AssertPo(&acr, 0);
	HQ hqoly;

	if ((hqoly = _HqolyCreate(pogn, fognAutoClose)) == hqNil)
		{
		PushErc(ercGfxCantDraw);
		return;
		}

	_gdd.grfgdd = fgddFill;
	_gdd.acrFore = acr;

	_pgpt->DrawPoly(hqoly, &_gdd);
	FreePhq(&hqoly);
}


/***************************************************************************
	Frame a polygon with a pattern.
	NOTE: Using kacrInvert produces slightly different results on the Mac.
	(Mac only does alternate winding).
***************************************************************************/
void GNV::FrameOgnApt(POGN pogn, APT *papt, ACR acrFore, ACR acrBack)
{
	AssertThis(0);
	AssertPo(pogn, 0);
	AssertVarMem(papt);
	AssertPo(&acrFore, 0);
	AssertPo(&acrBack, 0);
	HQ hqoly;

	if (_gdd.dxpPen == 0 && _gdd.dypPen == 0)
		return;

	if (hqNil == (hqoly = _HqolyFrame(pogn, fognAutoClose)))
		{
		PushErc(ercGfxCantDraw);
		return;
		}

	_gdd.grfgdd = fgddFrame | fgddPattern;
	_gdd.apt = *papt;
	_gdd.apt.MoveOrigin(_rcDst.xpLeft - _rcSrc.xpLeft, _rcDst.ypTop - _rcSrc.ypTop);
	_gdd.acrFore = acrFore;
	_gdd.acrBack = acrBack;

	_pgpt->DrawPoly(hqoly, &_gdd);
	FreePhq(&hqoly);
}


/***************************************************************************
	Frame a polygon with a color.
	NOTE: Using kacrInvert produces slightly different results on the Mac.
***************************************************************************/
void GNV::FrameOgn(POGN pogn, ACR acr)
{
	AssertThis(0);
	AssertPo(pogn, 0);
	AssertPo(&acr, 0);
	HQ hqoly;

	if (_gdd.dxpPen == 0 && _gdd.dypPen == 0)
		return;

	if (hqNil == (hqoly = _HqolyFrame(pogn, fognAutoClose)))
		{
		PushErc(ercGfxCantDraw);
		return;
		}

	_gdd.grfgdd = fgddFrame;
	_gdd.acrFore = acr;

	_pgpt->DrawPoly(hqoly, &_gdd);
	FreePhq(&hqoly);
}


/***************************************************************************
	Frame a poly-line with a pattern.
	NOTE: Using kacrInvert produces slightly different results on the Mac.
***************************************************************************/
void GNV::FramePolyLineApt(POGN pogn, APT *papt, ACR acrFore, ACR acrBack)
{
	AssertThis(0);
	AssertPo(pogn, 0);
	AssertVarMem(papt);
	AssertPo(&acrFore, 0);
	AssertPo(&acrBack, 0);
	HQ hqoly;

	if (_gdd.dxpPen == 0 && _gdd.dypPen == 0)
		return;

	if (hqNil == (hqoly = _HqolyFrame(pogn, fognNil)))
		{
		PushErc(ercGfxCantDraw);
		return;
		}

	_gdd.grfgdd = fgddFrame | fgddPattern;
	_gdd.apt = *papt;
	_gdd.apt.MoveOrigin(_rcDst.xpLeft - _rcSrc.xpLeft, _rcDst.ypTop - _rcSrc.ypTop);
	_gdd.acrFore = acrFore;
	_gdd.acrBack = acrBack;

	_pgpt->DrawPoly(hqoly, &_gdd);
	FreePhq(&hqoly);
}


/***************************************************************************
	Frame a poly-line with a color.
	NOTE: Using kacrInvert produces slightly different results on the Mac.
***************************************************************************/
void GNV::FramePolyLine(POGN pogn, ACR acr)
{
	AssertThis(0);
	AssertPo(pogn, 0);
	AssertPo(&acr, 0);
	HQ hqoly;

	if (_gdd.dxpPen == 0 && _gdd.dypPen == 0)
		return;

	if (hqNil == (hqoly = _HqolyCreate(pogn, fognNil)))
		{
		PushErc(ercGfxCantDraw);
		return;
		}

	_gdd.grfgdd = fgddFrame;
	_gdd.acrFore = acr;

	_pgpt->DrawPoly(hqoly, &_gdd);
	FreePhq(&hqoly);
}


/***************************************************************************
	Convert an OGN into a polygon record (hqoly).  This maps the points and
	optionally closes the polygon and/or calculates the bounds (Mac only).
***************************************************************************/
HQ GNV::_HqolyCreate(POGN pogn, ulong grfogn)
{
	AssertThis(0);
	AssertPo(pogn, 0);
	HQ hqoly;
	long ipt;
	long cb;
	long cpt;
	OLY *poly;
	PT *ppt;
	PTS *ppts;

	if ((cpt = pogn->IvMac()) < 2)
		return hqNil;

	cb = kcbOlyBase + LwMul(cpt, size(PTS));
	if (cpt < 3)
		grfogn &= ~fognAutoClose;
	else if (grfogn & fognAutoClose)
		cb += size(PTS);

	if (!FAllocHq(&hqoly, cb, fmemNil, mprNormal))
		return hqNil;

	poly = (OLY *)PvLockHq(hqoly);
#ifdef MAC
	poly->cb = (short)cb;
	Assert(cb == poly->cb, "polygon too big");
	poly->rcs.left = kswMax;
	poly->rcs.right = kswMin;
	poly->rcs.top = kswMax;
	poly->rcs.bottom = kswMin;
#else //!MAC
	poly->cpts = cpt + FPure(grfogn & fognAutoClose);
#endif //!MAC

	ppt = pogn->PrgptLock();
	ppts = (PTS *)poly->rgpts;
	for (ipt = 0; ipt < cpt; ipt++, ppt++, ppts++)
		{
		_MapPtPts(ppt->xp, ppt->yp, ppts);
#ifdef MAC
		// Compute bounding rectangle.
		if (poly->rcs.top > ppts->v)
			poly->rcs.top = ppts->v;
		if (poly->rcs.left > ppts->h)
			poly->rcs.left = ppts->h;
		if (poly->rcs.bottom < ppts->v)
			poly->rcs.bottom = ppts->v;
		if (poly->rcs.right < ppts->h)
			poly->rcs.right = ppts->h;
#endif //MAC
		}
	if (grfogn & fognAutoClose)
		*ppts = poly->rgpts[0];

	pogn->Unlock();
	UnlockHq(hqoly);
	return hqoly;
}


/***************************************************************************
	Convert a polygon (OGN) into a polygon record (hqoly).  On Windows,
	this actually generates a new polygon that is the outline of the framed
	path (which we'll tell GDI to fill).  On the Mac, this just calls
	_HqolyCreate.
***************************************************************************/
HQ GNV::_HqolyFrame(POGN pogn, ulong grfogn)
{
#ifdef WIN
	AssertThis(0);
	AssertPo(pogn, 0);
	HQ hqoly;

	POGN pognUse;
	PT rgptPen[4]; // Pen rectangle vectors.

	rgptPen[0].xp = 0;
	rgptPen[0].yp = 0;
	rgptPen[1].xp = _gdd.dxpPen;
	rgptPen[1].yp = 0;
	rgptPen[2].xp = _gdd.dxpPen;
	rgptPen[2].yp = _gdd.dypPen;
	rgptPen[3].xp = 0;
	rgptPen[3].yp = _gdd.dypPen;

	if (pvNil == (pognUse = pogn->PognTraceRgpt(rgptPen, 4, grfogn)))
		return pvNil;
	hqoly = _HqolyCreate(pognUse, grfogn);
	ReleasePpo(&pognUse);

	return hqoly;
#elif defined(MAC)
	return _HqolyCreate(pogn, grfogn);
#endif //MAC
}


/***************************************************************************
	Scroll the given rectangle by (dxp, dyp).  If prc1 is not nil fill it
	with the first uncovered rectangle.  If prc2 is not nil fill it
	with the second uncovered rectangle (if there is one).
***************************************************************************/
void GNV::ScrollRc(RC *prc, long dxp, long dyp, RC *prc1, RC *prc2)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertNilOrVarMem(prc1);
	AssertNilOrVarMem(prc2);
	PTS pts;
	RCS rcs;
	PT pt;

	if (!_FMapRcRcs(prc, &rcs) || dxp == 0 && dyp == 0)
		{
		if (pvNil != prc1)
			prc1->Zero();
		if (pvNil != prc2)
			prc2->Zero();
		return;
		}

	_MapPtPts(prc->xpLeft + dxp, prc->ypTop + dyp, &pts);
	pt = pts;
	_pgpt->ScrollRcs(&rcs, pt.xp - rcs.left, pt.yp - rcs.top, &_gdd);

	GetBadRcForScroll(prc, dxp, dyp, prc1, prc2);
}


/***************************************************************************
	Static method to get the RC's that are uncovered during a scroll
	operation.
***************************************************************************/
void GNV::GetBadRcForScroll(RC *prc, long dxp, long dyp, RC *prc1, RC *prc2)
{
	AssertNilOrVarMem(prc1);
	AssertNilOrVarMem(prc2);

	if (pvNil != prc1)
		{
		*prc1 = *prc;
		if (0 < dxp)
			prc1->xpRight = prc1->xpLeft + dxp;
		else if (0 > dxp)
			prc1->xpLeft = prc1->xpRight + dxp;
		else if (0 < dyp)
			prc1->ypBottom = prc1->ypTop + dyp;
		else
			prc1->ypTop = prc1->ypBottom + dyp;
		prc1->FIntersect(prc);
		}

	if (pvNil != prc2)
		{
		if (0 == dxp || 0 == dyp)
			prc2->Zero();
		else
			{
			*prc2 = *prc;
			if (0 < dyp)
				prc2->ypBottom = prc2->ypTop + dyp;
			else
				prc2->ypTop = prc2->ypBottom + dyp;
			if (0 < dxp)
				prc2->xpLeft += dxp;
			else
				prc2->xpRight += dxp;
			prc2->FIntersect(prc);
			}
		}
}


/***************************************************************************
	Get the source rectangle.
***************************************************************************/
void GNV::GetRcSrc(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);
	*prc = _rcSrc;
}


/***************************************************************************
	Set the source rectangle.
***************************************************************************/
void GNV::SetRcSrc(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	_rcSrc = *prc;
	if (_rcSrc.FEmpty())
		{
		_rcSrc.xpRight = _rcSrc.xpLeft + 1;
		_rcSrc.ypBottom = _rcSrc.ypTop + 1;
		}
	AssertThis(0);
}


/***************************************************************************
	Get the destination rectangle.
***************************************************************************/
void GNV::GetRcDst(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);
	PT pt;

	*prc = _rcDst;
	_pgpt->GetPtBase(&pt);
	prc->Offset(pt.xp, pt.yp);
}


/***************************************************************************
	Set the destination rectangle.  Also opens up the vis rc and clipping
	and sets default font and pen values.
***************************************************************************/
void GNV::SetRcDst(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);
	PT pt;

	_rcDst = *prc;
	if (_rcDst.FEmpty())
		{
		_rcDst.xpRight = _rcDst.xpLeft + 1;
		_rcDst.ypBottom = _rcDst.ypTop + 1;
		}
	_pgpt->GetPtBase(&pt);
	_rcDst.Offset(-pt.xp, -pt.yp);
	_gdd.prcsClip = pvNil;
	_rcVis.Max();

	_dsf.onn = vntl.OnnSystem();
	_dsf.grfont = fontNil;
	_dsf.dyp = vpappb->DypTextDef();
	_dsf.tah = tahLeft;
	_dsf.tav = tavTop;
	_gdd.dxpPen = _gdd.dypPen = 1;

	AssertThis(0);
}


/***************************************************************************
	Set the visible rectangle.  When ClipRc is called, the rc is first
	intersected with the vis rc.  Passing pvNil opens up the vis rectangle
	(so there is no natural clipping).  This should be called _after_
	SetRcDst, since SetRcDst opens up the vis rc.  This also opens the
	clipping to the vis rc.
***************************************************************************/
void GNV::SetRcVis(RC *prc)
{
	AssertThis(0);
	AssertNilOrVarMem(prc);

	if (prc == pvNil)
		{
		_rcVis.Max();
		_gdd.prcsClip = pvNil;
		}
	else
		{
		_rcVis = *prc;
		_rcVis.Map(&_rcSrc, &_rcDst);
		_rcsClip = RCS(_rcVis);
		_gdd.prcsClip = &_rcsClip;
		}
	AssertThis(0);
}


/***************************************************************************
	Intersect the current vis rectangle with the given rectangle and make
	that the new vis rectangle.  Opens the clipping to the vis rectangle
	also.
***************************************************************************/
void GNV::IntersectRcVis(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);
	RC rc;

	rc = *prc;
	rc.Map(&_rcSrc, &_rcDst);
	_rcVis.FIntersect(&rc);
	_rcsClip = RCS(_rcVis);
	_gdd.prcsClip = &_rcsClip;
	AssertThis(0);
}


/***************************************************************************
	Set the clipping (in source coordinates).  If prc is pvNil, opens up
	the clipping (to the vis rectangle).  Otherwise, sets the clipping
	to the intersection of the vis rectangle and *prc.
***************************************************************************/
void GNV::ClipRc(RC *prc)
{
	AssertThis(0);
	AssertNilOrVarMem(prc);

	RC rc;

	if (prc == pvNil)
		{
		rc.Max();
		if (rc == _rcVis)
			{
			//no clipping
			_gdd.prcsClip = pvNil;
			return;
			}
		rc = _rcVis;
		}
	else
		{
		rc = *prc;
		rc.Map(&_rcSrc, &_rcDst);
		rc.FIntersect(&_rcVis);
		}

	_rcsClip = RCS(rc);
	_gdd.prcsClip = &_rcsClip;
}


/***************************************************************************
	Clip to the source rectangle.
***************************************************************************/
void GNV::ClipToSrc(void)
{
	AssertThis(0);
	ClipRc(&_rcSrc);
}


/***************************************************************************
	Set the pen size (in source coordinates).
***************************************************************************/
void GNV::SetPenSize(long dxpPen, long dypPen)
{
	AssertThis(0);
	AssertIn(dxpPen, 0, kswMax);
	AssertIn(dypPen, 0, kswMax);
	_gdd.dxpPen = LwMulDivAway(dxpPen, _rcDst.Dxp(), _rcSrc.Dxp());
	_gdd.dypPen = LwMulDivAway(dypPen, _rcDst.Dyp(), _rcSrc.Dyp());
}


/***************************************************************************
	Set the current font info.
***************************************************************************/
void GNV::SetFont(long onn, ulong grfont, long dypFont, long tah, long tav)
{
	AssertThis(0);
	_dsf.onn = onn;
	_dsf.grfont = grfont;
	_dsf.dyp = LwMulDivAway(dypFont, _rcDst.Dyp(), _rcSrc.Dyp());
	_dsf.tah = tah;
	_dsf.tav = tav;
	AssertThis(0);
}


/***************************************************************************
	Set the current font.
***************************************************************************/
void GNV::SetOnn(long onn)
{
	AssertThis(0);
	_dsf.onn = onn;
	AssertThis(0);
}


/***************************************************************************
	Set the current font style.
***************************************************************************/
void GNV::SetFontStyle(ulong grfont)
{
	AssertThis(0);
	_dsf.grfont = grfont;
	AssertThis(0);
}


/***************************************************************************
	Set the current font size.
***************************************************************************/
void GNV::SetFontSize(long dyp)
{
	AssertThis(0);
	_dsf.dyp = LwMulDivAway(dyp, _rcDst.Dyp(), _rcSrc.Dyp());
	AssertThis(0);
}


/***************************************************************************
	Set the current font alignment.
***************************************************************************/
void GNV::SetFontAlign(long tah, long tav)
{
	AssertThis(0);
	_dsf.tah = tah;
	_dsf.tav = tav;
	AssertThis(0);
}


/******************************************************************************
	Set the current font.  Font size must be specified in Dst units.
******************************************************************************/
void GNV::SetDsf(DSF *pdsf)
{
	AssertThis(0);
	AssertPo(pdsf, 0);

	_dsf = *pdsf;
	AssertThis(0);
}


/******************************************************************************
	Get the current font.  Font size is specified in Dst units.
******************************************************************************/
void GNV::GetDsf(DSF *pdsf)
{
	AssertThis(0);
	AssertVarMem(pdsf);
	*pdsf = _dsf;
}


/******************************************************************************
	Draw some text.
******************************************************************************/
void GNV::DrawRgch(achar *prgch, long cch, long xp, long yp, ACR acrFore, ACR acrBack)
{
	AssertThis(0);
	AssertIn(cch, 0, kcbMax);
	AssertPvCb(prgch, cch);
	AssertPo(&acrFore, 0);
	AssertPo(&acrBack, 0);

	PTS pts;

	if (cch == 0)
		return;

	_gdd.acrFore = acrFore;
	_gdd.acrBack = acrBack;
	_MapPtPts(xp, yp, &pts);
	_pgpt->DrawRgch(prgch, cch, pts, &_gdd, &_dsf);
}


/***************************************************************************
	Draw the given string.
***************************************************************************/
void GNV::DrawStn(PSTN pstn, long xp, long yp, ACR acrFore, ACR acrBack)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	AssertPo(&acrFore, 0);
	AssertPo(&acrBack, 0);

	DrawRgch(pstn->Prgch(), pstn->Cch(), xp, yp, acrFore, acrBack);
}


/******************************************************************************
	Return the bounding box of the text.  If the GNV has any scaling, this
	is approximate.  This even works if cch is 0 (just gives the height).
******************************************************************************/
void GNV::GetRcFromRgch(RC *prc, achar *prgch, long cch, long xp, long yp)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertIn(cch, 0, kcbMax);
	AssertPvCb(prgch, cch);

	PTS pts;
	RCS rcs;

	_MapPtPts(xp, yp, &pts);
	_pgpt->GetRcsFromRgch(&rcs, prgch, cch, pts, &_dsf);
	*prc = rcs;
	prc->Map(&_rcDst, &_rcSrc);
}


/******************************************************************************
	Return the bounding box of the text.  If the GNV has any scaling, this
	is approximate.  This even works if the string is empty (gives the height).
******************************************************************************/
void GNV::GetRcFromStn(RC *prc, PSTN pstn, long xp, long yp)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertPo(pstn, 0);

	GetRcFromRgch(prc, pstn->Prgch(), pstn->Cch(), xp, yp);
}


/***************************************************************************
	Copy bits from a GNV to this one.
***************************************************************************/
void GNV::CopyPixels(PGNV pgnvSrc, RC *prcSrc, RC *prcDst)
{
	AssertThis(0);
	AssertPo(pgnvSrc, 0);
	AssertVarMem(prcSrc);
	AssertVarMem(prcDst);
	RCS rcsSrc, rcsDst;

	if (!pgnvSrc->_FMapRcRcs(prcSrc, &rcsSrc) || !_FMapRcRcs(prcDst, &rcsDst))
		return;
	_pgpt->CopyPixels(pgnvSrc->_pgpt, &rcsSrc, &rcsDst, &_gdd);
}


ulong _mpgfdgrfpt[4] =
	{
	fptNegateXp,
	fptNil,
	fptNegateYp | fptTranspose,
	fptTranspose
	};


ulong _mpgfdgrfptInv[4] =
	{
	fptNegateXp,
	fptNil,
	fptNegateXp | fptTranspose,
	fptTranspose
	};


/***************************************************************************
	Get the old palette in *ppglclrOld, allocate a transitionary palette
	in *ppglclrTrans, and get init the palette animation.  On failure set
	the new palette and set *ppglclrOld and *ppglclrTrans to nil.
	If cbitPixel is not zero and not the depth of this device, this sets
	the palette and returns false.
***************************************************************************/
bool GNV::_FInitPaletteTrans(PGL pglclr, PGL *ppglclrOld, PGL *ppglclrTrans,
	long cbitPixel)
{
	AssertNilOrPo(pglclr, 0);
	AssertVarMem(ppglclrOld);
	AssertVarMem(ppglclrTrans);
	long cclr = pvNil == pglclr ? 256 : pglclr->IvMac();

	*ppglclrOld = pvNil;
	*ppglclrTrans = pvNil;

	// get the current palette and set up the temporary transitionary palette
	if (0 != cbitPixel && _pgpt->CbitPixel() != cbitPixel ||
			pvNil == (*ppglclrOld = GPT::PglclrGetPalette()) ||
			0 == (cclr = LwMin((*ppglclrOld)->IvMac(), cclr)) ||
			pvNil == (*ppglclrTrans = GL::PglNew(size(CLR), cclr)))
		{
		ReleasePpo(ppglclrOld);
		if (pvNil != pglclr)
			GPT::SetActiveColors(pglclr, fpalIdentity);
		return fFalse;
		}

	AssertDo((*ppglclrTrans)->FSetIvMac(cclr), 0);
	GPT::SetActiveColors(*ppglclrOld, fpalIdentity | fpalInitAnim);
	return fTrue;
}


/***************************************************************************
	Transition the palette.  Merge pglclrOld and pglclrNew into pglclrTrans
	and animate the palette to pglclrTrans.  If either source palette is nil,
	*pclrSub is used in place of the nil palette.  acrSub must be an RGB color.
***************************************************************************/
void GNV::_PaletteTrans(PGL pglclrOld, PGL pglclrNew, long lwNum, long lwDen,
	PGL pglclrTrans, CLR *pclrSub)
{
	AssertNilOrPo(pglclrOld, 0);
	AssertNilOrPo(pglclrNew, 0);
	AssertIn(lwDen, 1, kcbMax);
	AssertIn(lwNum, 0, lwDen + 1);
	AssertNilOrVarMem(pclrSub);

	long iclr;
	CLR clrOld, clrNew;
	CLR clrSub;

	iclr = pglclrTrans->IvMac();
	if (pvNil != pglclrOld)
		iclr = LwMin(pglclrOld->IvMac(), iclr);
	if (pvNil != pglclrNew)
		iclr = LwMin(pglclrNew->IvMac(), iclr);
	if (pvNil != pclrSub)
		clrSub = *pclrSub;
	else
		ClearPb(&clrSub, size(clrSub));

	while (iclr-- > 0)
		{
		clrOld = clrNew = clrSub;
		if (pvNil != pglclrOld)
			pglclrOld->Get(iclr, &clrOld);
		if (pvNil != pglclrNew)
			pglclrNew->Get(iclr, &clrNew);

		clrOld.bRed += (byte)LwMulDiv((long)clrNew.bRed - clrOld.bRed,
			lwNum, lwDen);
		clrOld.bGreen += (byte)LwMulDiv((long)clrNew.bGreen - clrOld.bGreen,
			lwNum, lwDen);
		clrOld.bBlue += (byte)LwMulDiv((long)clrNew.bBlue - clrOld.bBlue,
			lwNum, lwDen);
		pglclrTrans->Put(iclr, &clrOld);
		}

	GPT::SetActiveColors(pglclrTrans, fpalIdentity | fpalAnimate);
}


/***************************************************************************
	Create a temporary GNV that is a copy of the given rectangle in this
	GNV.  This is used for several transitions.
***************************************************************************/
bool GNV::_FEnsureTempGnv(PGNV *ppgnv, RC *prc)
{
	PGPT pgpt;
	PGNV pgnv;

	if (pvNil == (pgpt = GPT::PgptNewOffscreen(prc, 8)) ||
			pvNil == (pgnv = NewObj GNV(pgpt)))
		{
		ReleasePpo(&pgpt);
		*ppgnv = pvNil;
		return fFalse;
		}

	ReleasePpo(&pgpt);
	pgnv->CopyPixels(this, prc, prc);
	GPT::Flush();
	*ppgnv = pgnv;
	return fTrue;
}


/***************************************************************************
	Wipe the source gnv onto this one.  If acrFill is not kacrClear, first
	wipe acrFill on.  The source and destination rectangles must be the same
	size.  gfd indicates which direction the wipe is.  If pglclr is not
	nil and acrFill is clear, the palette transition is gradual.
***************************************************************************/
void GNV::Wipe(long gfd, ACR acrFill, PGNV pgnvSrc, RC *prcSrc, RC *prcDst,
	ulong dts, PGL pglclr)
{
	AssertThis(0);
	AssertPo(&acrFill, 0);
	AssertPo(pgnvSrc, 0);
	AssertVarMem(prcSrc);
	AssertVarMem(prcDst);
	AssertNilOrPo(pglclr, 0);

	ulong grfpt, grfptInv;
	ulong tsStart, dtsT;
	long dxp, dxpTot;
	long cact;
	RC rcSrc, rcDst;
	RC rc1, rc2;
	PGL pglclrOld = pvNil;
	PGL pglclrTrans = pvNil;

	Assert(prcSrc->Dyp() == prcDst->Dyp() && prcSrc->Dxp() == prcDst->Dxp(),
		"rc's are scaled");

	GPT::Flush();
	if (!FIn(dts, 1, kdtsMaxTrans))
		dts = kdtsSecond;

	for (cact = 0; cact < 2; cact++)
		{
		grfpt = _mpgfdgrfpt[gfd & 0x03];
		grfptInv = _mpgfdgrfptInv[gfd & 0x03];
		gfd >>= 2;

		if (cact == 0)
			{
			if (kacrClear == acrFill)
				continue;
			}
		else if (pvNil != pglclr &&
				!_FInitPaletteTrans(pglclr, &pglclrOld, &pglclrTrans, 8))
			{
			pglclr = pvNil; // so we don't try to transition
			}

		tsStart = TsCurrent();
		rcSrc = *prcSrc;
		rcDst = *prcDst;
		rcSrc.Transform(grfpt);
		rcDst.Transform(grfpt);
		dxpTot = rcDst.Dxp();
		for (dxp = 0; dxp < dxpTot; )
			{
			rc1 = rcSrc;
			rc2 = rcDst;
			rc1.xpLeft = rcSrc.xpLeft + dxp;
			rc2.xpLeft = rcDst.xpLeft + dxp;
			dtsT = LwMin(TsCurrent() - tsStart, dts);
			dxp = LwMulDiv(dxpTot, dtsT, dts);
			rc1.xpRight = rcSrc.xpLeft + dxp;
			rc2.xpRight = rcDst.xpLeft + dxp;

			if (cact == 1 && pglclr != pvNil)
				_PaletteTrans(pglclrOld, pglclr, dtsT, dts, pglclrTrans);

			if (!rc2.FEmpty())
				{
				rc1.Transform(grfptInv);
				rc2.Transform(grfptInv);
				if (cact == 0)
					FillRc(&rc2, acrFill);
				else
					CopyPixels(pgnvSrc, &rc1, &rc2);
				GPT::Flush();
				}
			}

		if (pvNil != pglclr)
			{
			// set the palette
			GPT::SetActiveColors(pglclr, fpalIdentity);
			pglclr = pvNil; // so we don't transition during the second wipe
			}
		}

	ReleasePpo(&pglclrOld);
	ReleasePpo(&pglclrTrans);
}


/***************************************************************************
	Slide the source gnv onto this one.  The source and destination
	rectangles must be the same size.
***************************************************************************/
void GNV::Slide(long gfd, ACR acrFill, PGNV pgnvSrc, RC *prcSrc, RC *prcDst,
	ulong dts, PGL pglclr)
{
	AssertThis(0);
	AssertPo(&acrFill, 0);
	AssertPo(pgnvSrc, 0);
	AssertVarMem(prcSrc);
	AssertVarMem(prcDst);
	AssertNilOrPo(pglclr, 0);

	ulong grfpt, grfptInv;
	ulong dtsT, tsStart;
	long cact;
	long dxp, dxpTot, dxpOld;
	RC rcSrc, rcDst;
	RC rc1, rc2;
	PGNV pgnv;
	PT dpt;
	PGL pglclrOld = pvNil;
	PGL pglclrTrans = pvNil;

	Assert(prcSrc->Dyp() == prcDst->Dyp() && prcSrc->Dxp() == prcDst->Dxp(),
		"rc's are scaled");

	// allocate the offscreen port and copy the destination into it.
	if (!_FEnsureTempGnv(&pgnv, prcDst))
		{
		if (pvNil != pglclr)
			GPT::SetActiveColors(pglclr, fpalIdentity);
		CopyPixels(pgnvSrc, prcSrc, prcDst);
		GPT::Flush();
		return;
		}

	if (!FIn(dts, 1, kdtsMaxTrans))
		dts = kdtsSecond;

	for (cact = 0; cact < 2; cact++)
		{
		grfpt = _mpgfdgrfpt[gfd & 0x03];
		grfptInv = _mpgfdgrfptInv[gfd & 0x03];
		gfd >>= 2;

		if (cact == 0)
			{
			if (kacrClear == acrFill)
				continue;
			}
		else if (pvNil != pglclr &&
				!_FInitPaletteTrans(pglclr, &pglclrOld, &pglclrTrans))
			{
			pglclr = pvNil; // so we don't try to transition
			}

		tsStart = TsCurrent();
		rcSrc = *prcSrc;
		rcDst = *prcDst;
		rcSrc.Transform(grfpt);
		rcDst.Transform(grfpt);
		dxpTot = rcDst.Dxp();
		for (dxp = 0; dxp < dxpTot; )
			{
			dxpOld = dxp;
			dtsT = LwMin(TsCurrent() - tsStart, dts);
			dxp = LwMulDiv(dxpTot, dtsT, dts);

			if (dxp != dxpOld)
				{
				// scroll the stuff that's already there
				dpt.xp = dxp - dxpOld;
				dpt.yp = 0;
				dpt.Transform(grfptInv);
				pgnv->ScrollRc(prcDst, dpt.xp, dpt.yp);

				// copy in the new stuff
				rc1 = rcSrc;
				rc2 = rcDst;
				rc1.xpLeft = rc1.xpRight - dxp;
				rc1.xpRight -= dxpOld;
				rc2.xpRight = rc2.xpLeft + dxp - dxpOld;
				rc1.Transform(grfptInv);
				rc2.Transform(grfptInv);
				if (cact == 0)
					pgnv->FillRc(&rc2, acrFill);
				else
					pgnv->CopyPixels(pgnvSrc, &rc1, &rc2);
				}

			if (pvNil != pglclr && 1 == cact)
				_PaletteTrans(pglclrOld, pglclr, dtsT, dts, pglclrTrans);

			if (dxp != dxpOld)
				{
				// copy the result to the destination
				CopyPixels(pgnv, prcDst, prcDst);
				GPT::Flush();
				}
			}

		if (pvNil != pglclr)
			{
			// set the palette
			GPT::SetActiveColors(pglclr, fpalIdentity);

			// if we're not in 8 bit and cact is 1, copy the pixels so we
			// make sure we've drawn the picture after the last palette change
			if (1 == cact && _pgpt->CbitPixel() != 8)
				{
				CopyPixels(pgnv, prcDst, prcDst);
				GPT::Flush();
				}
			pglclr = pvNil; // so we don't transition during the second wipe
			}
		}

	ReleasePpo(&pgnv);
	ReleasePpo(&pglclrOld);
	ReleasePpo(&pglclrTrans);
}



// klwPrime must be a prime and klwRoot must be a primitive root for klwPrime.
// the code under #ifdef SPECIAL_PRIME below assumes that klwPrime is
// (2^16 + 1) and klwRoot is (2 ^15 - 1).  If you change klwPrime and/or
// klwRoot, make sure that SPECIAL_PRIME is not defined.

#define SPECIAL_PRIME
#define klwPrime 65537 // a prime
#define klwRoot  32767 // a primitive root for the prime


/***************************************************************************
	Returns the next quasi-random number for Dissolve.
***************************************************************************/
inline long _LwNextDissolve(long lw)
{
	AssertIn(lw, 1, klwPrime);

#ifdef SPECIAL_PRIME

	Assert(klwPrime == 0x00010001, 0);
	Assert(klwRoot == 0x00007FFF, 0);

	// multiply by 2^15 - 1
	lw = (lw << 15) - lw;

	// mod by 2^16 + 1
	lw = (lw & 0x0000FFFFL) - (long)((ulong)lw >> 16);
	if (lw < 0)
		lw += klwPrime;

#else //!SPECIAL_PRIME

	LwMulDivMod(lw, klwRoot, klwPrime, &lw);

#endif //!SPECIAL_PRIME

	return lw;
}


/***************************************************************************
	Dissolve the source gnv into this one.  If acrFill is not kacrClear,
	first dissolve into a solid acrFill, then into the source. The source
	and destination rectangles must be the same size.  If pgnvSrc is nil,
	just dissolve into the solid color.  Each portion is done in dts time.
***************************************************************************/
void GNV::Dissolve(long crcWidth, long crcHeight, ACR acrFill,
	PGNV pgnvSrc, RC *prcSrc, RC *prcDst, ulong dts, PGL pglclr)
{
	AssertThis(0);
	AssertPo(&acrFill, 0);
	AssertNilOrPo(pgnvSrc, 0);
	AssertNilOrVarMem(prcSrc);
	AssertVarMem(prcDst);
	AssertNilOrPo(pglclr, 0);
	AssertIn(crcWidth, 0, kcbMax);
	AssertIn(crcHeight, 0, kcbMax);

	ulong tsStart, dtsT;
	byte bFill;
	long cbRowSrc, cbRowDst;
	RND rnd;
	long lw, cact, irc, crc, crcFill, crcT;
	RC rc1, rc2;
	bool fOnScreen;
	long dibExtra, dibRow, ibExtra;
	byte *pbRow;
	byte *prgbDst = pvNil;
	byte *prgbSrc = pvNil;
	PGNV pgnv = pvNil;
	PGL pglclrOld = pvNil;
	PGL pglclrTrans = pvNil;

	if (prcDst->FEmpty())
		return;

	if (crcWidth <= 0 || crcHeight <= 0)
		{
		// do off screen pixel level dissolve
		fOnScreen = fFalse;

		// allocate the offscreen port and copy the destination into it.
		if (pgnvSrc != pvNil)
			{
			PGPT pgptSrc;

			AssertVarMem(prcSrc);
			Assert(prcSrc->Dyp() == prcDst->Dyp() && prcSrc->Dxp() == prcDst->Dxp(),
				"rc's are scaled");
			pgptSrc = pgnvSrc->Pgpt();
			if (pgptSrc->CbitPixel() != 8 ||
					pvNil == (prgbSrc = pgptSrc->PrgbLockPixels(&rc2)))
				{
				Bug("Can't dissolve from this GPT");
				goto LFail;
				}
			rc1 = *prcSrc;
			rc1.Map(&pgnvSrc->_rcSrc, &pgnvSrc->_rcDst);
			if (!rc2.FContains(&rc1))
				{
				Bug("Source rectangle is outside the source bitmap");
				goto LFail;
				}
			cbRowSrc = pgptSrc->CbRow();
			prgbSrc += LwMul(rc1.ypTop - rc2.ypTop, cbRowSrc) +
				rc1.xpLeft - rc2.xpLeft;
			}

		if (!_FEnsureTempGnv(&pgnv, prcDst))
			{
LFail:
			if (pvNil != prgbSrc && pvNil != pgnvSrc)
				pgnvSrc->Pgpt()->Unlock();

			if (pvNil != pglclr)
				GPT::SetActiveColors(pglclr, fpalIdentity);
			if (pvNil != pgnvSrc)
				CopyPixels(pgnvSrc, prcSrc, prcDst);
			else
				FillRc(prcDst, acrFill);
			GPT::Flush();
			return;
			}
		prgbDst = pgnv->Pgpt()->PrgbLockPixels();
		cbRowDst = pgnv->Pgpt()->CbRow();

		if (acrFill != kacrClear)
			{
			// get the byte value to fill with
			byte bT = prgbDst[0];
	
			rc1.Set(prcDst->xpLeft, prcDst->ypTop,
				prcDst->xpLeft + 1, prcDst->ypTop + 1);
			pgnv->FillRc(&rc1, acrFill);
			GPT::Flush();
			bFill = prgbDst[0];
			prgbDst[0] = bT;
			}

		crcWidth = cbRowDst;
		crcHeight = prcDst->Dyp();
		crcFill = LwMul(crcWidth, crcHeight) + prcDst->Dxp() - cbRowDst;

		dibExtra = (klwPrime - 1) % crcWidth;
		dibRow = ((klwPrime - 1) / crcWidth) * cbRowSrc;
		}
	else
		{
		// on screen dissolve
		fOnScreen = fTrue;
		crcWidth = LwMin(prcDst->Dxp(), crcWidth);
		crcHeight = LwMin(prcDst->Dyp(), crcHeight);
		crcFill = LwMul(crcWidth, crcHeight);
		}

	if (!FIn(dts, 1, kdtsMaxTrans))
		dts = kdtsSecond;

	// the first time through, dissolve to the color; the second time
	// through dissolve to the source bitmap
	for (cact = 0; cact < 2; cact++)
		{
		if (cact == 0)
			{
			if (kacrClear == acrFill)
				continue;
			}
		else
			{
			if (pvNil == pgnvSrc)
				goto LSetPalette;
			if (pvNil != pglclr && !_FInitPaletteTrans(pglclr, &pglclrOld,
					&pglclrTrans, fOnScreen ? 8 : 0))
				{
				pglclr = pvNil; // so we don't try to transition
				}
			}

		// We start with lw a random value between 1 and (klwPrime - 1)
		// (inclusive). Subsequent values of lw are computed as
		// lw = lw * klwRoot % klwPrime. Because klwRoot is a primitive root
		// of unity for klwPrime, lw will take on all values from 1 thru
		// (klwPrime - 1) in seemingly random order.

		irc = rnd.LwNext(crcFill);
		lw = (crcFill - irc - 1) % (klwPrime - 1) + 1;

		if (!fOnScreen && cact != 0)
			{
			// pbRow points to the row of prgbSrc that the current source
			// pixel is in.  ibExtra is the offset of the source pixel
			// into the row (the "xp" coordinate of the source pixel).
			// dibRow and dibExtra are for finding the source pixel
			// incrementally.  When we subtract (klwPrime - 1) from irc,
			// we subtract dibRow from pbRow and dibExtra from ibExtra.
			// If ibExtra goes negative, we add cbRowSrc to pbRow and
			// crcWidth to ibExtra.

			ibExtra = irc % crcWidth;
			pbRow = prgbSrc + (irc / crcWidth) * cbRowSrc;
			}

		tsStart = TsCurrent();
		for (crc = 0; crc < crcFill; )
			{
			dtsT = LwMin(TsCurrent() - tsStart, dts);
			crcT = LwMulDiv(crcFill, dtsT, dts) - crc;
			if (crcT <= 0)
				goto LPaletteTrans;
			crc += crcT;

			if (fOnScreen)
				{
				for ( ; crcT > 0; crcT--)
					{
					// find the next rectangle to fill
					for (irc -= klwPrime - 1; irc < 0; irc = crcFill - lw)
						lw = _LwNextDissolve(lw);

					rc1.SetToCell(prcDst, crcWidth, crcHeight, irc % crcWidth,
						irc / crcWidth);
					if (cact == 0)
						FillRc(&rc1, acrFill);
					else
						{
						rc2 = rc1;
						rc2.Map(prcDst, prcSrc);
						CopyPixels(pgnvSrc, &rc2, &rc1);
						}
					}
				goto LPaletteTrans;
				}

			if (cact == 0)
				{
				// fill with bFill
				for ( ; crcT > 0; crcT--)
					{
					// find the next pixel to fill
					for (irc -= klwPrime - 1; irc < 0; irc = crcFill - lw)
						lw = _LwNextDissolve(lw);
					prgbDst[irc] = bFill;
					}
				goto LBlastToScreen;
				}

			// cact == 1, fill from prgbSrc
			for ( ; crcT > 0; crcT--)
				{
				// find the next rectangle to fill
				irc -= klwPrime - 1;
				if (irc >= 0)
					{
					pbRow -= dibRow;
					ibExtra -= dibExtra;
					if (ibExtra < 0)
						{
						pbRow -= cbRowSrc;
						ibExtra += crcWidth;
						}
					}
				else
					{
					do
						{
						lw = _LwNextDissolve(lw);
						}
					while (0 > (irc = crcFill - lw));

#ifdef IN_80386
					__asm
						{
						// ibExtra = irc % crcWidth;
						// pbRow = prgbSrc + (irc / crcWidth) * cbRowSrc;
						mov		edx,irc
						movzx	eax,dx
						shr		edx,16
						div		WORD PTR crcWidth	// 16 bit divide for speed
						imul	eax,cbRowSrc
						mov		ibExtra,edx
						add		eax,prgbSrc
						mov		pbRow,eax
						}
#else //!IN_80386
					ibExtra = irc % crcWidth;
					pbRow = prgbSrc + (irc / crcWidth) * cbRowSrc;
#endif //!IN_80386
					}

				prgbDst[irc] = pbRow[ibExtra];
				}

LBlastToScreen:
			CopyPixels(pgnv, prcDst, prcDst);
			GPT::Flush();

LPaletteTrans:
			if (cact == 1 && pglclr != pvNil)
				_PaletteTrans(pglclrOld, pglclr, dtsT, dts, pglclrTrans);
			}

LSetPalette:
		if (pvNil != pglclr)
			{
			// set the palette
			GPT::SetActiveColors(pglclr, fpalIdentity);

			// if we're not in 8 bit and cact is 1, copy the pixels so we
			// make sure we've drawn the picture after the last palette change
			if (pvNil != pgnv && 1 == cact && _pgpt->CbitPixel() != 8)
				{
				CopyPixels(pgnv, prcDst, prcDst);
				GPT::Flush();
				}
			pglclr = pvNil; // so we don't transition during the second wipe
			}
		}

	if (pvNil != prgbDst)
		pgnv->Pgpt()->Unlock();
	if (pvNil != prgbSrc && pvNil != pgnvSrc)
		pgnvSrc->Pgpt()->Unlock();
	ReleasePpo(&pgnv);
	ReleasePpo(&pglclrOld);
	ReleasePpo(&pglclrTrans);
}


/***************************************************************************
	Fade the palette to color acrFade, copy the pixels from pgnvSrc to
	this gnv, then fade to the new palette or original palette.  Each fade
	is given dts time.  Asserts that acrFade is an rgb color.  cactMax is
	the maximum number of palette interpolations to do.  It doesn't make
	sense for this to be bigger than 256.  If it's zero, we'll use 256.
***************************************************************************/
void GNV::Fade(long cactMax, ACR acrFade, PGNV pgnvSrc, RC *prcSrc, RC *prcDst,
	ulong dts, PGL pglclr)
{
	AssertThis(0);
	AssertIn(cactMax, 0, 257);
	AssertPo(&acrFade, facrRgb);
	AssertPo(pgnvSrc, 0);
	AssertVarMem(prcSrc);
	AssertVarMem(prcDst);
	AssertNilOrPo(pglclr, 0);

	ulong tsStart;
	long cact, cactOld;
	CLR clr;
	PGL pglclrOld = pvNil;
	PGL pglclrTrans = pvNil;

	cactMax = (cactMax <= 0) ? 256 : LwMin(cactMax, 256);

	if (!_FInitPaletteTrans(pglclr, &pglclrOld, &pglclrTrans, 8))
		{
		CopyPixels(pgnvSrc, prcSrc, prcDst);
		GPT::Flush();
		return;
		}

	GPT::Flush();

	if (!FIn(dts, 1, kdtsMaxTrans))
		dts = kdtsSecond;

	acrFade.GetClr(&clr);

	tsStart = TsCurrent();
	for (cact = 0; cact < cactMax; )
		{
		cactOld = cact;
		cact = LwMulDiv(cactMax, LwMin(TsCurrent() - tsStart, dts), dts);
		if (cactOld < cact)
			_PaletteTrans(pglclrOld, pvNil, cact, cactMax, pglclrTrans, &clr);
		}

	CopyPixels(pgnvSrc, prcSrc, prcDst);
	GPT::Flush();

	if (pvNil == pglclr)
		pglclr = pglclrOld;

	tsStart = TsCurrent();
	for (cact = 0; cact < cactMax; )
		{
		cactOld = cact;
		cact = LwMulDiv(cactMax, LwMin(TsCurrent() - tsStart, dts), dts);
		if (cactOld < cact)
			_PaletteTrans(pvNil, pglclr, cact, cactMax, pglclrTrans, &clr);
		}

	GPT::SetActiveColors(pglclr, fpalIdentity);
	ReleasePpo(&pglclrOld);
	ReleasePpo(&pglclrTrans);
}


/***************************************************************************
	Open and/or close a rectangular iris onto the gnvSrc with an
	intermediate color of acrFill (if not clear).  xp, yp are the focus
	point of the iris (in destination coordinates).
***************************************************************************/
void GNV::Iris(long gfd, long xp, long yp, ACR acrFill,
	PGNV pgnvSrc, RC *prcSrc, RC *prcDst, ulong dts, PGL pglclr)
{
	AssertThis(0);
	AssertPo(&acrFill, 0);
	AssertPo(pgnvSrc, 0);
	AssertVarMem(prcSrc);
	AssertVarMem(prcDst);
	AssertNilOrPo(pglclr, 0);

	ulong tsStart, dtsT;
	RC rc, rcOld, rcDst;
	PT pt, ptBase;
	long cact;
	bool fOpen;
	PREGN pregn, pregnClip;
	PGL pglclrOld = pvNil;
	PGL pglclrTrans = pvNil;

	GPT::Flush();

	if (pvNil == (pregn = REGN::PregnNew(prcDst)))
		goto LFail;

	if (!FIn(dts, 1, kdtsMaxTrans))
		dts = kdtsSecond;

	_pgpt->GetPtBase(&ptBase);

	pt.xp = LwBound(xp, prcDst->xpLeft, prcDst->xpRight + 1);
	pt.yp = LwBound(yp, prcDst->ypTop, prcDst->ypBottom + 1);
	pt.Map(&_rcSrc, &_rcDst);
	pt += ptBase;
	rcDst = *prcDst;
	rcDst.Map(&_rcSrc, &_rcDst);
	rcDst.Offset(ptBase.xp, ptBase.yp);

	for (cact = 0; cact < 2; cact++)
		{
		if (cact == 0)
			{
			if (kacrClear == acrFill)
				continue;
			}
		else if (pvNil != pglclr &&
				!_FInitPaletteTrans(pglclr, &pglclrOld, &pglclrTrans, 8))
			{
			pglclr = pvNil; // so we don't try to transition
			}

		fOpen = !(gfd & (1 << cact));
		if (fOpen)
			rc.Set(pt.xp, pt.yp, pt.xp, pt.yp);
		else
			rc = rcDst;

		pregnClip = pvNil;
		tsStart = TsCurrent();
		for (dtsT = 0; dtsT < dts; )
			{
			rcOld = rc;
			dtsT = LwMin(TsCurrent() - tsStart, dts);
			if (!fOpen)
				dtsT = dts - dtsT;
			rc.xpLeft = pt.xp + LwMulDiv(rcDst.xpLeft - pt.xp, dtsT, dts);
			rc.xpRight = pt.xp + LwMulDiv(rcDst.xpRight - pt.xp, dtsT, dts);
			rc.ypTop = pt.yp + LwMulDiv(rcDst.ypTop - pt.yp, dtsT, dts);
			rc.ypBottom = pt.yp + LwMulDiv(rcDst.ypBottom - pt.yp, dtsT, dts);
			if (!fOpen)
				dtsT = dts - dtsT;

			if (cact == 1 && pglclr != pvNil)
				_PaletteTrans(pglclrOld, pglclr, dtsT, dts, pglclrTrans);

			if (rc == rcOld)
				continue;

			_pgpt->ClipToRegn(&pregnClip);
			pregn->SetRc(fOpen ? &rc : &rcOld);
			if (!pregn->FDiffRc(fOpen ? &rcOld : &rc) ||
					pvNil != pregnClip && !pregn->FIntersect(pregnClip))
				{
				_pgpt->ClipToRegn(&pregnClip);
				ReleasePpo(&pregn);
LFail:
				if (pvNil != pglclr)
					GPT::SetActiveColors(pglclr, fpalIdentity);
				CopyPixels(pgnvSrc, prcSrc, prcDst);
				return;
				}
			_pgpt->ClipToRegn(&pregnClip);

			_pgpt->ClipToRegn(&pregn);
			if (cact == 0)
				FillRc(prcDst, acrFill);
			else
				CopyPixels(pgnvSrc, prcSrc, prcDst);
			GPT::Flush();
			_pgpt->ClipToRegn(&pregn);
			}

		if (pvNil != pglclr)
			{
			// set the palette
			GPT::SetActiveColors(pglclr, fpalIdentity);
			pglclr = pvNil; // so we don't transition during the second iris
			}
		}

	ReleasePpo(&pregn);
	ReleasePpo(&pglclrOld);
	ReleasePpo(&pglclrTrans);
}


/***************************************************************************
	Draw the picture in the given rectangle.
***************************************************************************/
void GNV::DrawPic(PPIC ppic, RC *prc)
{
	AssertThis(0);
	AssertPo(ppic, 0);
	AssertVarMem(prc);
	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs))
		return;
	_pgpt->DrawPic(ppic, &rcs, &_gdd);
}


/***************************************************************************
	Draw the mbmp with reference point at the given point.
***************************************************************************/
void GNV::DrawMbmp(PMBMP pmbmp, long xp, long yp)
{
	AssertThis(0);
	AssertPo(pmbmp, 0);
	RC rc;
	RCS rcs;

	pmbmp->GetRc(&rc);
	rc.Offset(xp - rc.xpLeft, yp - rc.ypTop);
	if (!_FMapRcRcs(&rc, &rcs))
		return;
	_pgpt->DrawMbmp(pmbmp, &rcs, &_gdd);
}


/***************************************************************************
	Draw the mbmp in the given rectangle.
***************************************************************************/
void GNV::DrawMbmp(PMBMP pmbmp, RC *prc)
{
	AssertThis(0);
	AssertPo(pmbmp, 0);
	AssertVarMem(prc);
	RCS rcs;

	if (!_FMapRcRcs(prc, &rcs))
		return;
	_pgpt->DrawMbmp(pmbmp, &rcs, &_gdd);
}


/***************************************************************************
	Map a rectangle to a system rectangle.  Return true iff the result
	is non-empty.
***************************************************************************/
bool GNV::_FMapRcRcs(RC *prc, RCS *prcs)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertVarMem(prcs);
	RC rc = *prc;

	rc.Map(&_rcSrc, &_rcDst);
	*prcs = RCS(rc);
	return prcs->left < prcs->right && prcs->top < prcs->bottom;
}


/***************************************************************************
	Map an (xp, yp) pair to a system point.
***************************************************************************/
void GNV::_MapPtPts(long xp, long yp, PTS *ppts)
{
	AssertThis(0);
	AssertVarMem(ppts);
	PT pt(xp, yp);

	pt.Map(&_rcSrc, &_rcDst);
	*ppts = PTS(pt);
}


#ifdef MAC
/***************************************************************************
	Set the port associated with the GNV as the current port and set the
	clipping as in the GNV and set the pen and fore/back color to defaults.
***************************************************************************/
void GNV::Set(void)
{
	AssertThis(0);
	_pgpt->Set(_gdd.prcsClip);
	ForeColor(blackColor);
	BackColor(whiteColor);
	PenNormal();
}


/***************************************************************************
	Restore the port.  Balances a call to GNV::Set.
***************************************************************************/
void GNV::Restore(void)
{
	_pgpt->Restore();
}
#endif //MAC


/***************************************************************************
	Clip to the region specified by *ppregn.  *ppregn is set to the previous
	clip region (may be pvNil).  The GPT takes over ownership of the region
	and relinquishes ownership of the old region.
***************************************************************************/
void GPT::ClipToRegn(PREGN *ppregn)
{
	if (_ptBase.xp != 0 || _ptBase.yp != 0)
		{
		if (pvNil != *ppregn)
			(*ppregn)->Offset(-_ptBase.xp, -_ptBase.yp);
		if (pvNil != _pregnClip)
			_pregnClip->Offset(_ptBase.xp, _ptBase.yp);
		}
	SwapVars(&_pregnClip, ppregn);
	_fNewClip = fTrue;
}


/***************************************************************************
	Set the base PT for the GPT.  This affects the mapping of any attached
	GNVs.
***************************************************************************/
void GPT::SetPtBase(PT *ppt)
{
	AssertThis(0);
	AssertVarMem(ppt);
	_ptBase = *ppt;
}


/***************************************************************************
	Get the base PT for the GPT.
***************************************************************************/
void GPT::GetPtBase(PT *ppt)
{
	AssertThis(0);
	AssertVarMem(ppt);
	*ppt = _ptBase;
}


#ifdef DEBUG
/***************************************************************************
	Mark memory for the GPT.
***************************************************************************/
void GPT::MarkMem(void)
{
	AssertValid(0);
	GPT_PAR::MarkMem();
	MarkMemObj(_pregnClip);
}


/******************************************************************************
	Assert the validity of a polygon descriptor.
******************************************************************************/
void OLY::AssertValid(ulong grf)
{
	AssertThisMem();
	AssertPvCb(rgpts, LwMul(Cpts(), size(PTS)));
}


/******************************************************************************
	Assert the validity of the font description.
******************************************************************************/
void DSF::AssertValid(ulong grf)
{
	AssertThisMem();
	AssertIn(dyp, 1, kswMax);
	AssertVar(vntl.FValidOnn(onn), "Invalid onn", &onn);
	AssertIn(tah, 0, tahLim);
	AssertIn(tav, 0, tavLim);
}
#endif //DEBUG


/******************************************************************************
	Initialize the graphics module.
******************************************************************************/
bool FInitGfx(void)
{
	return vntl.FInit();
}


/***************************************************************************
	Construct a new font list.
***************************************************************************/
NTL::NTL(void)
{
	_pgst = pvNil;
}


/***************************************************************************
	Destroy a font list.
***************************************************************************/
NTL::~NTL(void)
{
	ReleasePpo(&_pgst);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the font list.
***************************************************************************/
void NTL::AssertValid(ulong grf)
{
	NTL_PAR::AssertValid(0);
	AssertPo(_pgst, 0);
}


/***************************************************************************
	Mark memory for the font table.
***************************************************************************/
void NTL::MarkMem(void)
{
	AssertValid(0);
	NTL_PAR::MarkMem();
	MarkMemObj(_pgst);
}


/***************************************************************************
	Return whether the font number is valid.
***************************************************************************/
bool NTL::FValidOnn(long onn)
{
	return pvNil != _pgst && onn >= 0 && onn < _pgst->IstnMac();
}
#endif //DEBUG


/***************************************************************************
	Find the name of the given font.
***************************************************************************/
void NTL::GetStn(long onn, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	_pgst->GetStn(onn, pstn);
}


/***************************************************************************
	Get the font number for the given font name.
***************************************************************************/
bool NTL::FGetOnn(PSTN pstn, long *ponn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	return _pgst->FFindStn(pstn, ponn, fgstUserSorted);
}


/***************************************************************************
	Map a font name to an existing font.  Use platform information if
	possible.
	REVIEW shonk: implement font mapping for real.
***************************************************************************/
long NTL::OnnMapStn(PSTN pstn, short osk)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	long onn;

	if (!FGetOnn(pstn, &onn))
		onn = _onnSystem;
	return onn;
}


/***************************************************************************
	Return the font number mac.
***************************************************************************/
long NTL::OnnMac(void)
{
	AssertThis(0);
	return _pgst->IstnMac();
}


/***************************************************************************
	Create a new polygon by tracing the outline of this one with a
	convex polygon.
***************************************************************************/
POGN OGN::PognTraceOgn(POGN pogn, ulong grfogn)
{
	AssertThis(0);
	AssertPo(pogn, 0);

	POGN pognNew = PognTraceRgpt(pogn->PrgptLock(), pogn->IvMac(), grfogn);

	pogn->Unlock();
	return pognNew;
}


/***************************************************************************
	Create a new polygon by tracing the outline of this one with a
	convex polygon.
***************************************************************************/
POGN OGN::PognTraceRgpt(PT *prgpt, long cpt, ulong grfogn)
{
	AssertThis(0);
	AssertIn(cpt, 2, kcbMax);
	AssertPvCb(prgpt, LwMul(cpt, size(PT)));

	PT *prgptThis;
	AEI aei;
	long iptLast = IvMac() - 1;

	if (2 > cpt || iptLast < 0)
		return pvNil;

	aei.prgpt = prgpt;
	aei.cpt = cpt;
	if (pvNil == (aei.pogn = PognNew(IvMac() * 2 + cpt)))
		return pvNil;
	aei.pogn->SetMinGrow(cpt);

	prgptThis = PrgptLock();
	if (iptLast == 0 || prgptThis[0] == prgptThis[1])
		{
		//if all the points of the polygon are the same, doing this ensures
		//that we will encircle the pen
		aei.ptCur = prgpt[0] + prgptThis[0];
		if (!aei.pogn->FPush(&aei.ptCur))
			goto LError;
		aei.iptPenCur = 1;
		}
	else
		{
		aei.iptPenCur = IptFindLeftmost(prgpt, cpt,
			prgptThis[0].xp - prgptThis[1].xp,
			prgptThis[0].yp - prgptThis[1].yp);
		}
	aei.ptCur = prgpt[aei.iptPenCur] + prgptThis[0];
	if (!aei.pogn->FPush(&aei.ptCur))
		goto LError;

	// Walk to end, adding vertices.
	for (aei.dipt = 1, aei.ipt = 0; aei.ipt < iptLast; aei.ipt++)
		{
		if (!_FAddEdge(&aei))
			goto LError;
		}
	if (fognAutoClose & grfogn)
		{
		if (!_FAddEdge(&aei))
			goto LError;
		aei.ipt = 0;
		aei.dipt = iptLast;
		if (!_FAddEdge(&aei))
			goto LError;
		}

	// Walk back to start.
	for (aei.dipt = aei.ipt = iptLast; aei.ipt > 0; aei.ipt--)
		{
		if (!_FAddEdge(&aei))
			{
LError:
			ReleasePpo(&aei.pogn);
			break;
			}
		}

	Unlock();
	if (pvNil != aei.pogn)
		aei.pogn->FEnsureSpace(0, fgrpShrink);
	return aei.pogn;
}


/***************************************************************************
	Add the vertices encountered while walking an edge of the input polygon.
***************************************************************************/
bool OGN::_FAddEdge(AEI *paei)
{
	AssertVarMem(paei);
	AssertIn(paei->cpt, 1, kcbMax);
	AssertPvCb(paei->prgpt, LwMul(paei->cpt, size(PT)));
	AssertPo(paei->pogn, 0);
	AssertIn(paei->dipt, LwMin(IvMac() - 1, 1), LwMax(IvMac(), 2));
	AssertIn(paei->ipt , 0, IvMac());
	AssertIn(paei->iptPenCur, 0, paei->cpt);

	PT * prgptThis = (PT *)QvGet(0); // Already locked in PognTraceRgpt().
	long iptEnd = (paei->ipt + paei->dipt) % IvMac();
	long iptPenNew = IptFindLeftmost(paei->prgpt, paei->cpt,
		prgptThis[iptEnd].xp - prgptThis[paei->ipt].xp,
		prgptThis[iptEnd].yp - prgptThis[paei->ipt].yp);

	// Add vertices from current to leftmost.
	if (paei->iptPenCur != iptPenNew)
		{
		PT pt;
		long ipt = paei->iptPenCur;
		PT dpt = paei->ptCur - paei->prgpt[ipt];

		do
			{
			ipt = (ipt + 1) % paei->cpt;
			pt = paei->prgpt[ipt] + dpt;
			if (!paei->pogn->FPush(&pt))
				return fFalse;
			}
		while (ipt != iptPenNew);
		}

	// Add vertex at endpoint.
	paei->iptPenCur = iptPenNew;
	paei->ptCur = prgptThis[iptEnd] + paei->prgpt[iptPenNew];
	return paei->pogn->FPush(&paei->ptCur);
}


/***************************************************************************
	Find the leftmost vertex of the rgpt looking down the vector.
	dxp, dyp : Direction of vector to look down.
***************************************************************************/
long IptFindLeftmost(PT *prgpt, long cpt, long dxp, long dyp)
{
	AssertPvCb(prgpt, LwMul(cpt, size(PT)));
	AssertIn(cpt, 2, kcbMax);

	long ipt, iptLeftmost;
	long dzpMac; // Maximum cross product (z vector) found.
	long dzp;

	//reduces the chance of overflow
	if (1 < (dzp = LwGcd(dxp, dyp)))
		{
		dxp /= dzp;
		dyp /= dzp;
		}

	for (dzpMac = klwMin, ipt = 0; ipt < cpt; ipt++)
		{
		dzp = LwMul(dyp, prgpt[ipt].xp) - LwMul(dxp, prgpt[ipt].yp);
		if (dzp > dzpMac)
			{
			dzpMac = dzp;
			iptLeftmost = ipt;
			}
		}
	return iptLeftmost;
}


/***************************************************************************
	-- Allocate a new OGN and ensure that it has space for cptInit elements.
***************************************************************************/
POGN OGN::PognNew(long cptInit)
{
	AssertIn(cptInit, 0, kcbMax);
	POGN pogn;

	if (pvNil == (pogn = NewObj OGN()))
		return pvNil;
	if (cptInit > 0 && !pogn->FEnsureSpace(cptInit, fgrpNil))
		{
		ReleasePpo(&pogn);
		return pvNil;
		}
	AssertPo(pogn, 0);
	return pogn;
}


/***************************************************************************
	Constructor for OGN.
***************************************************************************/
OGN::OGN(void) : GL(size(PT))
{
	AssertThis(0);
}


/***************************************************************************
	This does a 2x stretch blt, clipped to prcClip and pregnClip. The
	clipping is expressed in destination coordinates.
***************************************************************************/
void DoubleStretch(byte *prgbSrc, long cbRowSrc, long dypSrc, RC *prcSrc,
	byte *prgbDst, long cbRowDst, long dypDst, long xpDst, long ypDst,
	RC *prcClip, PREGN pregnClip)
{
	AssertPvCb(prgbSrc, LwMul(cbRowSrc, dypSrc));
	AssertPvCb(prgbDst, LwMul(cbRowDst, dypDst));
	AssertVarMem(prcSrc);
	Assert(prcSrc->xpLeft >= 0 && prcSrc->ypTop >= 0 &&
		prcSrc->xpRight <= cbRowSrc && prcSrc->ypBottom <= dypSrc,
		"Source rectangle not in source bitmap!");
	AssertNilOrVarMem(prcClip);
	AssertNilOrPo(pregnClip, 0);

	long xpOn, xpOff, dypAdvance, dxpBase, yp;
	bool fSecondRow;
	REGSC regsc;
	RC rcT(xpDst, ypDst, xpDst + 2 * prcSrc->Dxp(), ypDst + 2 * prcSrc->Dyp());
	RC rcClip(0, 0, cbRowDst, dypDst);

	if (!rcClip.FIntersect(&rcT))
		return;
	if (pvNil != prcClip && !rcClip.FIntersect(prcClip))
		return;

	// Set up the region scanner
	if (pvNil != pregnClip)
		regsc.Init(pregnClip, &rcClip);
	else
		regsc.InitRc(&rcClip, &rcClip);
	dxpBase = rcClip.xpLeft - xpDst;

	// move to rcClip.ypTop
	yp = rcClip.ypTop;
	prgbSrc += prcSrc->xpLeft +
		LwMul(prcSrc->ypTop + ((yp - ypDst) >> 1), cbRowSrc);
	prgbDst += xpDst + LwMul(yp, cbRowDst);
	fSecondRow = (yp - ypDst) & 1;

	for (;;)
		{
		if (klwMax == (xpOn = regsc.XpCur()))
			{
			//empty strip of the region
			dypAdvance = regsc.DypCur();
			goto LAdvance;
			}
		xpOn += dxpBase;
		if (fSecondRow || (regsc.DypCur() == 1))
			goto LOneRow;

		// copy two rows to the destination
		for (;;)
			{
			xpOff = regsc.XpFetch() + dxpBase;
			AssertIn(xpOff - 1, xpOn, rcClip.Dxp() + dxpBase);

#ifdef IN_80386
			// copy two rows to the destination in native
			#define pbDstReg edi
			#define pbSrcReg esi
			#define pbDst2Reg ebx
			#define lwTReg edx

			__asm
				{
				// lwTReg = xpOn;
				// pbDstReg = prgbDst + xpOn;
				// pbSrcReg = prgbSrc + (xpOn >> 1);
				// pbDst2Reg = pbDstReg + cbRowDst;
				mov		lwTReg,xpOn
				mov		pbSrcReg,lwTReg
				mov		pbDstReg,lwTReg
				sar		pbSrcReg,1
				add		pbDstReg,prgbDst
				add		pbSrcReg,prgbSrc
				mov		pbDst2Reg,pbDstReg
				add		pbDst2Reg,cbRowDst

				// if (!(lwTReg & 1)) goto LGetCount2;
				test	lwTReg,1
				mov		ecx,xpOff
				jz		LGetCount2

				// move the single leading byte
				// lwTReg++;
				// al = *pbSrcReg++;
				// *pbDstReg++ = al;
				// *pbDstReg2++ = al;
				inc		lwTReg
				mov		al,[pbSrcReg]
				inc		pbSrcReg
				mov		[pbDstReg],al
				inc		pbDstReg
				mov		[pbDst2Reg],al
				inc		pbDst2Reg

LGetCount2:
				// ecx = xpOff - lwTReg;
				// ecx >>= 1;
				// if (ecx <= 0) goto LLastByte2;
				sub		ecx,lwTReg
				sar		ecx,1
				jle		LLastByte2

LLoop2:
				// al = *pbSrcReg++;
				// ah = al;
				// *(short *)pbDstReg = ax;
				// *(short *)pbDst2Reg = ax;
				// pbDstReg += 2;
				// pbDst2Reg += 2;
				mov		al,[pbSrcReg]
				inc		pbSrcReg
				mov		ah,al
				mov		[pbDstReg],ax
				add		pbDstReg,2
				mov		[pbDst2Reg],ax
				add		pbDst2Reg,2

				// if (--ecx != 0) goto LLoop2;
				loop	LLoop2

LLastByte2:
				// if (!(xpOff & 1)) goto LDone2;
				test	xpOff,1
				jz		LDone2

				// move the single trailing byte
				// al = *pbSrcReg;
				// *pbDstReg = al;
				// *pbDstReg2 = al;
				mov		al,[pbSrcReg]
				mov		[pbDstReg],al
				mov		[pbDst2Reg],al
LDone2:
				}

			#undef pbDstReg
			#undef pbSrcReg
			#undef pbDst2Reg
			#undef lwTReg
#else //!IN_80386
			// copy two rows to the destination in C code
			byte *pbSrc = prgbSrc + (xpOn >> 1);
			byte *pbDst = prgbDst + xpOn;
			byte *pbDst2 = pbDst + cbRowDst;
			byte bT;
			long cactLoop;

			if (xpOn & 1)
				{
				// do leading single byte
				bT = *pbSrc++;
				*pbDst++ = bT;
				*pbDst2++ = bT;
				xpOn++;
				}

			for (cactLoop = (xpOff - xpOn) >> 1; cactLoop > 0; cactLoop--)
				{
				bT = *pbSrc++;
				pbDst[0] = bT;
				pbDst[1] = bT;
				pbDst += 2;
				pbDst2[0] = bT;
				pbDst2[1] = bT;
				pbDst2 += 2;
				}

			if (xpOff & 1)
				{
				// do the trailing byte
				bT = *pbSrc;
				*pbDst = bT;
				*pbDst2 = bT;
				}
#endif //!IN_80386

			if (klwMax == (xpOn = regsc.XpFetch()))
				break;
			xpOn += dxpBase;
			AssertIn(xpOn - 1, xpOff, rcClip.Dxp() - 1 + dxpBase);
			}
		dypAdvance = 2;
		goto LAdvance;

LOneRow:
		// copy just one row to the destination
		for (;;)
			{
			xpOff = regsc.XpFetch() + dxpBase;
			AssertIn(xpOff - 1, xpOn, rcClip.Dxp() + dxpBase);

#ifdef IN_80386
			// copy just one row to the destination in native
			#define pbDstReg edi
			#define pbSrcReg esi
			#define lwTReg edx

			__asm
				{
				// lwTReg = xpOn;
				// pbDstReg = prgbDst + xpOn;
				// pbSrcReg = prgbSrc + (xpOn >> 1);
				mov		lwTReg,xpOn
				mov		pbSrcReg,lwTReg
				mov		pbDstReg,lwTReg
				sar		pbSrcReg,1
				add		pbDstReg,prgbDst
				add		pbSrcReg,prgbSrc

				// if (!(lwTReg & 1)) goto LGetCount1;
				test	lwTReg,1
				mov		ecx,xpOff
				jz		LGetCount1

				// move the single leading byte
				// lwTReg++;
				// al = *pbSrcReg++;
				// *pbDstReg++ = al;
				inc		lwTReg
				mov		al,[pbSrcReg]
				inc		pbSrcReg
				inc		pbDstReg
				mov		[pbDstReg-1],al

LGetCount1:
				// ecx = xpOff - lwTReg;
				// ecx >>= 1;
				// if (ecx <= 0) goto LLastByte1;
				sub		ecx,lwTReg
				sar		ecx,1
				jle		LLastByte1

LLoop1:
				// al = *pbSrcReg++;
				// ah = al;
				// *(short *)pbDstReg = ax;
				// pbDstReg += 2;
				mov		al,[pbSrcReg]
				inc		pbSrcReg
				add		pbDstReg,2
				mov		ah,al
				mov		[pbDstReg-2],ax

				// if (--ecx != 0) goto LLoop1;
				loop	LLoop1

LLastByte1:
				// if (!(xpOff & 1)) goto LDone1;
				test	xpOff,1
				jz		LDone1

				// move the single trailing byte
				// al = *pbSrcReg;
				// *pbDstReg = al;
				mov		al,[pbSrcReg]
				mov		[pbDstReg],al
LDone1:
				}

			#undef pbDstReg
			#undef pbSrcReg
			#undef lwTReg
#else //!IN_80386
			// copy just one row to the destination in C code
			byte bT;
			byte *pbSrc = prgbSrc + (xpOn >> 1);
			byte *pbDst = prgbDst + xpOn;
			long cactLoop;

			if (xpOn & 1)
				{
				// do leading single byte
				*pbDst++ = *pbSrc++;
				xpOn++;
				}

			for (cactLoop = (xpOff - xpOn) >> 1; cactLoop > 0; cactLoop--)
				{
				bT = *pbSrc++;
				pbDst[0] = bT;
				pbDst[1] = bT;
				pbDst += 2;
				}

			if (xpOff & 1)
				{
				// do the trailing byte
				*pbDst = *pbSrc;
				}
#endif //!IN_80386

			if (klwMax == (xpOn = regsc.XpFetch()))
				break;
			xpOn += dxpBase;
			AssertIn(xpOn - 1, xpOff, rcClip.Dxp() - 1 + dxpBase);
			}
		dypAdvance = 1;

LAdvance:
		if ((yp += dypAdvance) >= rcClip.ypBottom)
			break;
		regsc.ScanNext(dypAdvance);
		prgbDst += dypAdvance * cbRowDst;
		prgbSrc += (dypAdvance >> 1) * cbRowSrc;
		if (dypAdvance & 1)
			{
			if (fSecondRow)
				prgbSrc += cbRowSrc;
			fSecondRow = !fSecondRow;
			}
		}
}


/***************************************************************************
	This does a 2x vertical and 1x horizontal stretch blt, clipped to prcClip
	and pregnClip. The clipping is expressed in destination coordinates.
***************************************************************************/
void DoubleVertStretch(byte *prgbSrc, long cbRowSrc, long dypSrc, RC *prcSrc,
	byte *prgbDst, long cbRowDst, long dypDst, long xpDst, long ypDst,
	RC *prcClip, PREGN pregnClip)
{
	AssertPvCb(prgbSrc, LwMul(cbRowSrc, dypSrc));
	AssertPvCb(prgbDst, LwMul(cbRowDst, dypDst));
	AssertVarMem(prcSrc);
	Assert(prcSrc->xpLeft >= 0 && prcSrc->ypTop >= 0 &&
		prcSrc->xpRight <= cbRowSrc && prcSrc->ypBottom <= dypSrc,
		"Source rectangle not in source bitmap!");
	AssertNilOrVarMem(prcClip);
	AssertNilOrPo(pregnClip, 0);

	long xpOn, xpOff, dypAdvance, dxpBase, yp;
	bool fSecondRow;
	REGSC regsc;
	RC rcT(xpDst, ypDst, xpDst + prcSrc->Dxp(), ypDst + 2 * prcSrc->Dyp());
	RC rcClip(0, 0, cbRowDst, dypDst);

	if (!rcClip.FIntersect(&rcT))
		return;
	if (pvNil != prcClip && !rcClip.FIntersect(prcClip))
		return;

	// Set up the region scanner
	if (pvNil != pregnClip)
		regsc.Init(pregnClip, &rcClip);
	else
		regsc.InitRc(&rcClip, &rcClip);
	dxpBase = rcClip.xpLeft - xpDst;

	// move to rcClip.ypTop
	yp = rcClip.ypTop;
	prgbSrc += prcSrc->xpLeft +
		LwMul(prcSrc->ypTop + ((yp - ypDst) >> 1), cbRowSrc);
	prgbDst += xpDst + LwMul(yp, cbRowDst);
	fSecondRow = (yp - ypDst) & 1;

	for (;;)
		{
		if (klwMax == (xpOn = regsc.XpCur()))
			{
			//empty strip of the region
			dypAdvance = regsc.DypCur();
			goto LAdvance;
			}
		xpOn += dxpBase;
		if (fSecondRow || (regsc.DypCur() == 1))
			goto LOneRow;

		// copy two rows to the destination
		for (;;)
			{
			xpOff = regsc.XpFetch() + dxpBase;
			AssertIn(xpOff - 1, xpOn, rcClip.Dxp() + dxpBase);

#ifdef IN_80386

			// copy two rows to the destination in native
			__asm
				{
				// edi = prgbDst + xpOn;
				// esi = eax = prgbSrc + xpOn;
				// ebx = pbDstReg + cbRowDst;
				// ecx = xpOff - xpOn
				mov		edx,xpOn
				mov		ecx,xpOff
				mov		edi,edx
				mov		esi,edx
				sub		ecx,edx
				add		edi,prgbDst
				add		esi,prgbSrc
				mov		ebx,edi
				mov		eax,esi
				add		ebx,cbRowDst

				// copy the first row
				mov		edx,ecx
				shr		ecx,2
				rep		movsd
				mov		ecx,edx
				and		ecx,3
				rep		movsb

				// prepare to copy the second row
				mov		esi,eax
				mov		edi,ebx
				mov		ecx,edx

				// copy the second row
				shr		ecx,2
				and		edx,3
				rep		movsd
				mov		ecx,edx
				rep		movsb
				}

#else //!IN_80386

			// copy two rows to the destination in C code
			CopyPb(prgbSrc + xpOn, prgbDst + xpOn, xpOff - xpOn);
			CopyPb(prgbSrc + xpOn + cbRowDst, prgbDst + xpOn, xpOff - xpOn);

#endif //!IN_80386

			if (klwMax == (xpOn = regsc.XpFetch()))
				break;
			xpOn += dxpBase;
			AssertIn(xpOn - 1, xpOff, rcClip.Dxp() - 1 + dxpBase);
			}
		dypAdvance = 2;
		goto LAdvance;

LOneRow:
		// copy just one row to the destination
		for (;;)
			{
			xpOff = regsc.XpFetch() + dxpBase;
			AssertIn(xpOff - 1, xpOn, rcClip.Dxp() + dxpBase);

#ifdef IN_80386

			// copy one row to the destination in native
			__asm
				{
				// edi = prgbDst + xpOn;
				// esi = prgbSrc + xpOn;
				// ecx = xpOff - xpOn
				mov		edx,xpOn
				mov		ecx,xpOff
				mov		edi,edx
				mov		esi,edx
				sub		ecx,edx
				add		edi,prgbDst
				add		esi,prgbSrc

				// copy the row
				mov		edx,ecx
				shr		ecx,2
				and		edx,3
				rep		movsd
				mov		ecx,edx
				rep		movsb
				}

#else //!IN_80386

			// copy one row to the destination in C code
			CopyPb(prgbSrc + xpOn, prgbDst + xpOn, xpOff - xpOn);

#endif //!IN_80386

			if (klwMax == (xpOn = regsc.XpFetch()))
				break;
			xpOn += dxpBase;
			AssertIn(xpOn - 1, xpOff, rcClip.Dxp() - 1 + dxpBase);
			}
		dypAdvance = 1;

LAdvance:
		if ((yp += dypAdvance) >= rcClip.ypBottom)
			break;
		regsc.ScanNext(dypAdvance);
		prgbDst += dypAdvance * cbRowDst;
		prgbSrc += (dypAdvance >> 1) * cbRowSrc;
		if (dypAdvance & 1)
			{
			if (fSecondRow)
				prgbSrc += cbRowSrc;
			fSecondRow = !fSecondRow;
			}
		}
}


