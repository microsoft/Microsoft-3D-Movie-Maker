/***************************************************************************

	zbmp.cpp: Z-buffer bitmap class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

***************************************************************************/
#include "bren.h"
ASSERTNAME

RTCLASS(ZBMP)

/***************************************************************************
	Create a ZBMP of all 0xff's
***************************************************************************/
PZBMP ZBMP::PzbmpNew(long dxp, long dyp)
{
	AssertIn(dxp, 1, klwMax);
	AssertIn(dyp, 1, klwMax);

	PZBMP pzbmp;
	
	pzbmp = NewObj ZBMP;
	if (pvNil == pzbmp)
		goto LFail;
	pzbmp->_rc.Set(0, 0, dxp, dyp);
	pzbmp->_cbRow = LwMul(dxp, kcbPixelZbmp);
	pzbmp->_cb = LwMul(dyp, pzbmp->_cbRow);
	if (!FAllocPv((void **)&pzbmp->_prgb, pzbmp->_cb, fmemNil, mprNormal))
		goto LFail;	
	FillPb(pzbmp->_prgb, pzbmp->_cb, 0xff);
	AssertPo(pzbmp, 0);
	return pzbmp;
LFail:
	ReleasePpo(&pzbmp);
	return pvNil;
}


/***************************************************************************
	Create a ZBMP from a BPMP.  For authoring use only.
***************************************************************************/
PZBMP ZBMP::PzbmpNewFromBpmp(BPMP *pbpmp)
{
	AssertVarMem(pbpmp);

	PZBMP pzbmp;
	
	pzbmp = PzbmpNew(pbpmp->width, pbpmp->height);
	if (pvNil == pzbmp)
		return pvNil;
	CopyPb(pbpmp->pixels, pzbmp->_prgb, pzbmp->_cb);
	return pzbmp;
}


/***************************************************************************
	Chunky resource reader for ZBMP
***************************************************************************/
bool ZBMP::FReadZbmp(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, 0);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);

	ZBMP *pzbmp;

	*pcb = pblck->Cb(fTrue); // estimate ZBMP size
	if (pvNil == ppbaco)
		return fTrue;

	if (!pblck->FUnpackData())
		goto LFail;
	*pcb = pblck->Cb();

	pzbmp = PzbmpRead(pblck);
	if (pvNil == pzbmp)
		{
LFail:
		TrashVar(ppbaco);
		TrashVar(pcb);
		return fFalse;
		}
	AssertPo(pzbmp, 0);
	*ppbaco = pzbmp;
	*pcb = size(ZBMP) + pzbmp->_cb;
	return fTrue;
}


/***************************************************************************
	Read a ZBMP from a BLCK.
***************************************************************************/
PZBMP ZBMP::PzbmpRead(PBLCK pblck)
{
	AssertPo(pblck, 0);

	PZBMP pzbmp;
	ZBMPF zbmpf;

	if (!pblck->FUnpackData())
		return pvNil;

	pzbmp = NewObj ZBMP;
	if (pvNil == pzbmp)
		goto LFail;

	if (!pblck->FReadRgb(&zbmpf, size(ZBMPF), 0))
		goto LFail;

	pzbmp->_rc.Set(zbmpf.xpLeft, zbmpf.ypTop, zbmpf.xpLeft + zbmpf.dxp,
		zbmpf.ypTop + zbmpf.dyp);
	pzbmp->_cbRow = LwMul(pzbmp->_rc.Dxp(), kcbPixelZbmp);
	pzbmp->_cb = LwMul(pzbmp->_rc.Dyp(), pzbmp->_cbRow);
	if (pblck->Cb() != size(ZBMPF) + pzbmp->_cb)
		goto LFail;
	if (!FAllocPv((void **)&pzbmp->_prgb, pzbmp->_cb, fmemNil, mprNormal))
		goto LFail;
	if (!pblck->FReadRgb(pzbmp->_prgb, pzbmp->_cb, size(ZBMPF)))
		goto LFail;	
	AssertPo(pzbmp, 0);
	return pzbmp;

LFail:
	Warn("Couldn't load ZBMP");
	ReleasePpo(&pzbmp);
	return pvNil;
}


/***************************************************************************
	Destructor
***************************************************************************/
ZBMP::~ZBMP(void)
{
	AssertBaseThis(0);
	FreePpv((void **)&_prgb);
}


/***************************************************************************
	Draw the ZBMP into prgbPixels
***************************************************************************/
void ZBMP::Draw(byte *prgbPixels, long cbRow, long dyp, long xpRef,
	long ypRef,	RC *prcClip, PREGN pregnClip)
{
	AssertThis(0);
	AssertPvCb(prgbPixels, LwMul(cbRow, dyp));
	AssertNilOrVarMem(prcClip);
	AssertNilOrVarMem(pregnClip);

	long yp;
	long cbRowCopy;
	byte *pbSrc;
	byte *pbDst;
	REGSC regsc;
	RC rcZbmp = _rc;
	RC rcRegnBounds;
	RC rcClippedRegnBounds;
	long xpLeft, xpRight;
	
	// Translate the zbmp's rc into coordinate system of prgbPixels' rc
	rcZbmp.Offset(xpRef, ypRef);

	if (pvNil == pregnClip)
		{
		if (pvNil == prcClip)
			return;
		if (!rcClippedRegnBounds.FIntersect(&rcZbmp, prcClip))
			return;
		regsc.InitRc(prcClip, &rcClippedRegnBounds);
		}
	else
		{
		if (pregnClip->FEmpty(&rcRegnBounds))
			return;
		if (!rcClippedRegnBounds.FIntersect(&rcZbmp, &rcRegnBounds))
			return;
		if (pvNil != prcClip && !rcClippedRegnBounds.FIntersect(prcClip))
			return;
		regsc.Init(pregnClip, &rcClippedRegnBounds);
		}

	for (yp = rcClippedRegnBounds.ypTop; yp < rcClippedRegnBounds.ypBottom;
		yp++)
		{
		while (regsc.XpCur() < klwMax)
			{
			xpLeft = regsc.XpCur() + rcClippedRegnBounds.xpLeft;
			xpRight = regsc.XpFetch() + rcClippedRegnBounds.xpLeft;
			AssertIn(xpLeft, rcClippedRegnBounds.xpLeft,
				rcClippedRegnBounds.xpRight + 1);
			AssertIn(xpRight, rcClippedRegnBounds.xpLeft,
				rcClippedRegnBounds.xpRight + 1);
			regsc.XpFetch();
			cbRowCopy = LwMul(kcbPixelZbmp, xpRight - xpLeft);
			pbSrc = _prgb + LwMul(yp, _cbRow) + LwMul(kcbPixelZbmp, xpLeft);
			pbDst = prgbPixels + LwMul(yp, cbRow) + LwMul(kcbPixelZbmp,
				xpLeft);
			CopyPb(pbSrc, pbDst, cbRowCopy);
			}
		regsc.ScanNext(1);
		}
}


/***************************************************************************
	Draw the ZBMP into prgbPixels, squashing the clip region vertically by
	two (for BWLD's "half mode")
***************************************************************************/
void ZBMP::DrawHalf(byte *prgbPixels, long cbRow, long dyp, long xpRef,
	long ypRef,	RC *prcClip, PREGN pregnClip)
{
	AssertThis(0);
	AssertPvCb(prgbPixels, LwMul(cbRow, dyp / 2));
	AssertNilOrVarMem(prcClip);
	AssertNilOrVarMem(pregnClip);

	long yp;
	long cbRowCopy;
	byte *pbSrc;
	byte *pbDst;
	REGSC regsc;
	RC rcZbmp = _rc;
	RC rcRegnBounds;
	RC rcClippedRegnBounds;
	long xpLeft, xpRight;

	rcZbmp.ypBottom *= 2;
	
	// Translate the zbmp's rc into coordinate system of prgbPixels' rc
	rcZbmp.Offset(xpRef, ypRef);

	if (pvNil == pregnClip)
		{
		if (pvNil == prcClip)
			return;
		if (!rcClippedRegnBounds.FIntersect(&rcZbmp, prcClip))
			return;
		regsc.InitRc(prcClip, &rcClippedRegnBounds);
		}
	else
		{
		if (pregnClip->FEmpty(&rcRegnBounds))
			return;
		if (!rcClippedRegnBounds.FIntersect(&rcZbmp, &rcRegnBounds))
			return;
		if (pvNil != prcClip && !rcClippedRegnBounds.FIntersect(prcClip))
			return;
		regsc.Init(pregnClip, &rcClippedRegnBounds);
		}

	for (yp = rcClippedRegnBounds.ypTop; yp < rcClippedRegnBounds.ypBottom;
		yp += 2)
		{
		while (regsc.XpCur() < klwMax)
			{
			xpLeft = regsc.XpCur() + rcClippedRegnBounds.xpLeft;
			xpRight = regsc.XpFetch() + rcClippedRegnBounds.xpLeft;
			AssertIn(xpLeft, rcClippedRegnBounds.xpLeft,
				rcClippedRegnBounds.xpRight + 1);
			AssertIn(xpRight, rcClippedRegnBounds.xpLeft,
				rcClippedRegnBounds.xpRight + 1);
			regsc.XpFetch();
			cbRowCopy = LwMul(kcbPixelZbmp, xpRight - xpLeft);
			pbSrc = _prgb + LwMul(yp/2, _cbRow) + LwMul(kcbPixelZbmp, xpLeft);
			pbDst = prgbPixels + LwMul(yp/2, cbRow) + LwMul(kcbPixelZbmp,
				xpLeft);
			CopyPb(pbSrc, pbDst, cbRowCopy);
			}
		regsc.ScanNext(2);
		}
}


/***************************************************************************
	Write the ZBMP
***************************************************************************/
bool ZBMP::FWrite(PCFL pcfl, CTG ctg, CNO *pcno)
{
	AssertThis(0);
	AssertPo(pcfl, 0);
	AssertVarMem(pcno);

	ZBMPF zbmpf;
	BLCK blck;

	zbmpf.bo = kboCur;
	zbmpf.osk = koskCur;
	zbmpf.xpLeft = (short)_rc.xpLeft;
	zbmpf.ypTop = (short)_rc.ypTop;
	zbmpf.dxp = (short)_rc.Dxp();
	zbmpf.dyp = (short)_rc.Dyp();

	if (!pcfl->FAdd(size(ZBMPF) + _cb, ctg, pcno, &blck))
		return fFalse;
	if (!blck.FWriteRgb(&zbmpf, size(ZBMPF), 0))
		return fFalse;
	if (!blck.FWriteRgb(_prgb, _cb, size(ZBMPF)))
		return fFalse;
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the ZBMP.
***************************************************************************/
void ZBMP::AssertValid(ulong grf)
{
	ZBMP_PAR::AssertValid(fobjAllocated);
	AssertPvCb(_prgb, _cb);
	Assert(_cbRow == LwMul(_rc.Dxp(), kcbPixelZbmp), "bad _cbRow");
	Assert(_cb == LwMul(_rc.Dyp(), _cbRow), "bad _cb");
}


/***************************************************************************
	Mark memory used by the ZBMP
***************************************************************************/
void ZBMP::MarkMem(void)
{
	AssertThis(0);
	ZBMP_PAR::MarkMem();
	MarkPv(_prgb);
}
#endif //DEBUG


