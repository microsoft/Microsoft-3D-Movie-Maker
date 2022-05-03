/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Region code.

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(REGN)
RTCLASS(REGSC)

long const kcdxpBlock = 100;

/***************************************************************************
	Region builder class.  For building the _pglxp of a region and getting
	its bounding rectangle and _dxp value.
***************************************************************************/
typedef class REGBL *PREGBL;
#define REGBL_PAR BASE
#define kclsREGBL 'rgbl'
class REGBL : public REGBL_PAR
	{
protected:
	long _ypCur;
	RC _rcRef;
	RC _rc;
	PGL _pglxp;
	bool _fResize;

	long _idypPrev;
	long _idypCur;
	long _ixpCur;

public:
	REGBL(void)
		{ _pglxp = pvNil; }
	~REGBL(void)
		{ ReleasePpo(&_pglxp); }

	bool FInit(RC *prcRef, PGL pglxp = pvNil);
	bool FStartRow(long dyp, long cxpMax);
	void EndRow(void);
	void AddXp(long xp)
		{
		// can only be called while building a row.
		AssertThis(0);
		Assert(_idypCur != ivNil, "calling AddXp outside a row");
		Assert(_ixpCur < _pglxp->IvMac(), "overflow in AddXp");

		if (_rc.xpLeft > xp)
			_rc.xpLeft = xp;
		if (_rc.xpRight < xp)
			_rc.xpRight = xp;
		*(long *)_pglxp->QvGet(_ixpCur++) = xp;
		}

	bool FDone(void)
		{
		AssertThis(0);
		return _ypCur >= _rcRef.ypBottom && _idypCur == ivNil;
		}
	PGL PglxpFree(RC *prc, long *pdxp);
	};


/***************************************************************************
	Initialize a region builder.
***************************************************************************/
bool REGBL::FInit(RC *prcRef, PGL pglxp)
{
	AssertVarMem(prcRef);
	AssertNilOrPo(pglxp, 0);
	Assert(!prcRef->FEmpty(), "empty reference rectangle");

	ReleasePpo(&_pglxp);
	if (pvNil != pglxp)
		{
		_pglxp = pglxp;
		_pglxp->AddRef();
		_fResize = fFalse;
		}
	else
		{
		if (pvNil == (_pglxp = GL::PglNew(size(long))))
			return fFalse;
		_pglxp->SetMinGrow(kcdxpBlock);
		_fResize = fTrue;
		}

	_rcRef = *prcRef;
	_ypCur = _rc.ypTop = _rc.ypBottom = prcRef->ypTop;
	_rc.xpLeft = prcRef->Dxp();
	_rc.xpRight = 0;

	_idypPrev = _idypCur = ivNil;
	_ixpCur = 0;
	return fTrue;
}


/***************************************************************************
	Begin a row.
***************************************************************************/
bool REGBL::FStartRow(long dyp, long cxpMax)
{
	AssertThis(0);
	Assert(_idypCur == ivNil, "row already started");
	AssertIn(dyp, 0, kcbMax);
	Assert(_ypCur < _rcRef.ypBottom, "already filled");
	AssertIn(cxpMax, 0, kcbMax);
	long ivLim;

	ivLim = _ixpCur + cxpMax + 2;
	if (_fResize)
		{
		ivLim = LwRoundAway(ivLim, kcdxpBlock);
		if (ivLim > _pglxp->IvMac() && !_pglxp->FSetIvMac(ivLim))
			{
			ReleasePpo(&_pglxp);
			return fFalse;
			}
		}
	else if (ivLim > _pglxp->IvMac())
		{
		Bug("overflowed provided pgl");
		return fFalse;
		}

	dyp = LwMin(dyp, _rcRef.ypBottom - _ypCur);
	_idypCur = _ixpCur;
	*(long *)_pglxp->QvGet(_ixpCur++) = dyp;
	_ypCur += dyp;
	return fTrue;
}


/***************************************************************************
	End a row in the region builder.
***************************************************************************/
void REGBL::EndRow(void)
{
	AssertThis(0);
	Assert((_ixpCur - _idypCur) & 1, "not an even number of xp values");
	Assert(_ixpCur < _pglxp->IvMac(), "overflow in EndRow");
	long czp;
	long *qrgxp = (long *)_pglxp->QvGet(0);

	qrgxp[_ixpCur++] = klwMax;
	if ((czp = _ixpCur - _idypCur) > 2)
		_rc.ypBottom = _ypCur;

	// see if this row matches the last one
	if (ivNil != _idypPrev && czp == _idypCur - _idypPrev &&
		FEqualRgb(&qrgxp[_idypPrev + 1], &qrgxp[_idypCur + 1],
			(czp - 1) * size(long)))
		{
		// this row matches the previous row
		qrgxp[_idypPrev] += qrgxp[_idypCur];
		_ixpCur = _idypCur;
		}
	else if (0 == _idypCur && _ixpCur == 2)
		{
		//at the top and still empty
		_rc.ypTop = _rc.ypBottom = _ypCur;
		_ixpCur = 0;
		}
	else
		_idypPrev = _idypCur;

	_idypCur = ivNil;
}


/***************************************************************************
	Clean up and return all the relevant information.
***************************************************************************/
PGL REGBL::PglxpFree(RC *prc, long *pdxp)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertVarMem(pdxp);
	PGL pglxp;

	// see if the last row is empty
	if (_ypCur > _rc.ypBottom)
		{
		// last row is empty
		_ixpCur = _idypPrev;
		}

	*pdxp = 0;
	if (0 == _ixpCur)
		{
		// empty
		Assert(_rc.FEmpty(), 0);
		ReleasePpo(&_pglxp);
		_rc.Zero();
		}
	else
		{
		Assert(!_rc.FEmpty(), 0);
		if (_ixpCur <= 4)
			{
			// rectangular
			ReleasePpo(&_pglxp);
			}
		else
			{
			AssertDo(_pglxp->FSetIvMac(_ixpCur), 0);
			AssertDo(_pglxp->FEnsureSpace(0, fgrpShrink), 0);
			*pdxp = -_rc.xpLeft;
			}
		_rc.Offset(_rcRef.xpLeft, 0);
		}

	*prc = _rc;
	pglxp = _pglxp;
	_pglxp = pvNil;
	return pglxp;
}


/***************************************************************************
	Constructor for the region scanner class.
***************************************************************************/
REGSC::REGSC(void)
{
	_pglxpSrc = pvNil;
}


/***************************************************************************
	Destructor for the region scanner class.
***************************************************************************/
REGSC::~REGSC(void)
{
	Free();
}


/***************************************************************************
	Release our hold on any memory.
***************************************************************************/
void REGSC::Free(void)
{
	if (pvNil != _pglxpSrc)
		{
		_pglxpSrc->Unlock();
		ReleasePpo(&_pglxpSrc);
		}
}


/***************************************************************************
	Initializes a region scanner.  The scanner implicitly intersects the
	region with *prcRel and returns xp values relative to prcRel->xpLeft.
***************************************************************************/
void REGSC::Init(PREGN pregn, RC *prcRel)
{
	RC rc = pregn->_rc;
	rc.Offset(pregn->_dxp, 0);
	_InitCore(pregn->_pglxp, &rc, prcRel);
}


/***************************************************************************
	Initializes a region scanner with the given rectangle.
***************************************************************************/
void REGSC::InitRc(RC *prc, RC *prcRel)
{
	_InitCore(pvNil, prc, prcRel);
}


/***************************************************************************
	Initializes a region scanner.  The scanner implicitly intersects the
	region with *prcRel and returns xp values relative to prcRel->xpLeft.
***************************************************************************/
void REGSC::_InitCore(PGL pglxp, RC *prc, RC *prcRel)
{
	Free();

	RC rc = *prc;
	if (pvNil != (_pglxpSrc = pglxp))
		{
		_pglxpSrc->AddRef();
		_pxpLimCur = (long *)_pglxpSrc->PvLock(0);
		_pxpLimSrc = _pxpLimCur + _pglxpSrc->IvMac();
		}
	else
		{
		rc.FIntersect(prcRel);
		_rgxpRect[0] = rc.Dyp();
		_rgxpRect[1] = 0;
		_rgxpRect[2] = rc.Dxp();
		_rgxpRect[3] = klwMax;
		_pxpLimCur = _rgxpRect;
		_pxpLimSrc = _rgxpRect + 4;
		}
	_dxp = rc.xpLeft - prcRel->xpLeft;
	_xpRight = prcRel->Dxp();

	_dyp = rc.ypTop - prcRel->ypTop;
	_dypTot = prcRel->Dyp();

	//initialize to an empty row
	_pxpLimRow = (_pxpMinRow = _pxpLimCur) - 1;
	_xpMinRow = klwMax;

	ScanNext(0);
}


/***************************************************************************
	Scan the next horizontal strip of the region.
***************************************************************************/
void REGSC::_ScanNextCore(void)
{
	long dxpT;

	Assert(_dyp <= 0, "why is _ScanNextCore being called?");
	if (_dypTot <= 0)
		{
		//ran out of region!
		goto LEndRegion;
		}

	for (;;)
		{
		//do the next scan
		if (_pxpLimCur >= _pxpLimSrc)
			{
			//ran out of region!
LEndRegion:
			//use kswMax, not klwMax, so clients can add this to
			//another yp value without fear of overflow
			_dyp = kswMax;
			_pxpLimRow = (_pxpMinRow = _pxpLimCur = _pxpLimSrc) - 1;
			_xpMinRow = klwMax;
			return;
			}
		if ((_dyp += *_pxpLimCur++) > 0)
			break;

		//find the start of the next row
		while (*_pxpLimCur != klwMax)
			{
			_pxpLimCur += 2;
			Assert(_pxpLimCur < _pxpLimSrc && _pxpLimCur[-1] != klwMax,
				"bad region 1");
			}
		_pxpLimCur++;
		}

	//_pxpLimCur now points to the beginning of the correct row
	//find the first xp value in our range
	if (*_pxpLimCur < -_dxp)
		{
		do
			{
			_pxpLimCur += 2;
			Assert(_pxpLimCur < _pxpLimSrc && _pxpLimCur[-1] != klwMax,
				"bad region 2");
			}
		while (*_pxpLimCur < -_dxp);

		//see if we went too far
		if (_pxpLimCur[-1] > -_dxp)
			{
			_xpMinRow = 0;
			_pxpMinRow = _pxpLimCur - 1;
			goto LFindLim;
			}
		}
	_xpMinRow = *_pxpLimCur++;

	_pxpMinRow = _pxpLimCur;
	if (_xpMinRow == klwMax)
		{
		//empty row
		_pxpLimRow = _pxpLimCur - 1;
		return;
		}

	if ((_xpMinRow += _dxp) >= _xpRight)
		{
		//empty row, but we need to find the start of the next row
		AssertIn(_xpMinRow, 0, kcbMax);
		Assert(*_pxpLimCur != klwMax, "bad region 3");

		//find the start of the next row
		_pxpLimCur++;
		while (*_pxpLimCur != klwMax)
			{
			_pxpLimCur += 2;
			Assert(_pxpLimCur < _pxpLimSrc && _pxpLimCur[-1] != klwMax,
				"bad region 4");
			}
		_pxpLimCur++;
		_pxpLimRow = (_pxpMinRow = _pxpLimCur) - 1;
		_xpMinRow = klwMax;
		return;
		}

	Assert(*_pxpLimCur != klwMax, "bad region 5");
	_pxpLimCur++;

LFindLim:
	dxpT = _xpRight - _dxp;
	while (*_pxpLimCur < dxpT)
		{
		_pxpLimCur += 2;
		Assert(_pxpLimCur < _pxpLimSrc && _pxpLimCur[-1] != klwMax,
			"bad region 6");
		}
	_pxpLimRow = _pxpLimCur;
	while (*_pxpLimCur != klwMax)
		{
		_pxpLimCur += 2;
		Assert(_pxpLimCur < _pxpLimSrc && _pxpLimCur[-1] != klwMax,
			"bad region 6");
		}
	_pxpLimCur++;
	Assert((_pxpLimRow - _pxpMinRow) & 1, "logic error above");
}


/***************************************************************************
	Static method to create a new region and set it to a rectangle.
***************************************************************************/
PREGN REGN::PregnNew(RC *prc)
{
	PREGN pregn;

	if (pvNil == (pregn = NewObj REGN))
		return pvNil;
	if (pvNil != prc)
		pregn->_rc = *prc;
	AssertPo(pregn, 0);
	return pregn;
}


/***************************************************************************
	Destructor for a region.
***************************************************************************/
REGN::~REGN(void)
{
	ReleasePpo(&_pglxp);
	FreePhrgn(&_hrgn);
}


/***************************************************************************
	Make the region rectangular.
***************************************************************************/
void REGN::SetRc(RC *prc)
{
	AssertThis(0);
	AssertNilOrVarMem(prc);

	if (pvNil == prc)
		_rc.Zero();
	else
		_rc = *prc;
	ReleasePpo(&_pglxp);
	FreePhrgn(&_hrgn);
	_dxp = 0;
	AssertThis(0);
}


/***************************************************************************
	Offset the region.
***************************************************************************/
void REGN::Offset(long xp, long yp)
{
	AssertThis(0);
	_rc.Offset(xp, yp);
	_dptRgn.Offset(xp, yp);
}


/***************************************************************************
	Return whether the region is empty and if prc is not nil, fill in *prc
	with the region's bounding rectangle.
***************************************************************************/
bool REGN::FEmpty(RC *prc)
{
	AssertThis(0);
	AssertNilOrVarMem(prc);

	if (pvNil != prc)
		*prc = _rc;
	return _rc.FEmpty();
}


/***************************************************************************
	Return whether the region is rectangular and if prc is not nil, fill in
	*prc with the region's bounding rectangle.
***************************************************************************/
bool REGN::FIsRc(RC *prc)
{
	AssertThis(0);
	if (pvNil != prc)
		*prc = _rc;
	return _pglxp == pvNil;
}


/***************************************************************************
	Scale the x values of the region by the given amount.
***************************************************************************/
void REGN::Scale(long lwNumX, long lwDenX, long lwNumY, long lwDenY)
{
	AssertThis(0);
	Assert(lwDenX > 0 && lwNumX >= 0, "bad X scaling");
	Assert(lwDenY > 0 && lwNumY >= 0, "bad Y scaling");
	RC rcScaled;
	long yp, ypScaled, dyp, dypScaled;
	long xp1, xp2;
	RAT ratXp(lwNumX, lwDenX);
	RAT ratYp(lwNumY, lwDenY);

	rcScaled.xpLeft = ratXp.LwScale(_rc.xpLeft);
	rcScaled.xpRight = ratXp.LwScale(_rc.xpRight);
	rcScaled.ypTop = ratYp.LwScale(_rc.ypTop);
	rcScaled.ypBottom = ratYp.LwScale(_rc.ypBottom);

	if (rcScaled.FEmpty())
		{
		_rc.Zero();
		ReleasePpo(&_pglxp);
		return;
		}

	if (pvNil == _pglxp)
		{
		_rc = rcScaled;
		_dxp = 0;
		return;
		}

	REGSC regsc;
	REGBL regbl;

	regsc.Init(this, &_rc);
	AssertDo(regbl.FInit(&rcScaled, _pglxp), 0);
	yp = _rc.ypTop;
	ypScaled = rcScaled.ypTop;

	for (;;)
		{
		dyp = regsc.DypCur();
		dypScaled = ratYp.LwScale(yp + dyp) - ypScaled;
		yp += dyp;
		if (dypScaled <= 0)
			{
			regsc.ScanNext(dyp);
			continue;
			}
		ypScaled += dypScaled;

		AssertDo(regbl.FStartRow(dypScaled, regsc.CxpCur()), 0);
		if (regsc.XpCur() == klwMax)
			goto LEndRow;

		xp1 = ratXp.LwScale(regsc.XpCur());
		xp2 = ratXp.LwScale(regsc.XpFetch());
		for (;;)
			{
			if (xp1 >= xp2)
				{
				// empty run - ignore both values
				if (regsc.XpFetch() == klwMax)
					break;
				xp1 = ratXp.LwScale(regsc.XpCur());
				}
			else
				{
				// write xp1 and advance
				regbl.AddXp(xp1);
				xp1 = xp2;
				}

			// fetch a new xp2
			if (regsc.XpFetch() == klwMax)
				{
				// write the last off
				regbl.AddXp(xp1);
				break;
				}
			xp2 = ratXp.LwScale(regsc.XpCur());
			}

LEndRow:
		regbl.EndRow();
		if (regbl.FDone())
			break;

		regsc.ScanNext(dyp);
		}

	ReleasePpo(&_pglxp);
	FreePhrgn(&_hrgn);

	// force the region scanner to let go of the pglxp before the
	// region builder tries to resize it.
	regsc.Free();

	_pglxp = regbl.PglxpFree(&_rc, &_dxp);
}


/***************************************************************************
	Union the two regions and leave the result in this one.  If pregn2 is
	nil, this region is used for pregn2.
***************************************************************************/
bool REGN::FUnion(PREGN pregn1, PREGN pregn2)
{
	AssertThis(0);
	AssertPo(pregn1, 0);
	AssertNilOrPo(pregn2, 0);
	REGSC regsc1;
	REGSC regsc2;
	RC rc;

	if (pvNil == pregn2)
		{
		if (pregn1->FEmpty())
			return fTrue;
		pregn2 = this;
		}

	rc.Union(&pregn1->_rc, &pregn2->_rc);
	if (pvNil == pregn1->_pglxp && rc == pregn1->_rc ||
			pvNil == pregn2->_pglxp && rc == pregn2->_rc)
		{
		// union is a rectangle
		SetRc(&rc);
		return fTrue;
		}

	regsc1.Init(pregn1, &rc);
	regsc2.Init(pregn2, &rc);
	return _FUnionCore(&rc, &regsc1, &regsc2);
}


/***************************************************************************
	Union the given rectangle and region and leave the result in this region.
	If pregn is nil, this region is used for pregn.
***************************************************************************/
bool REGN::FUnionRc(RC *prc, PREGN pregn)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertNilOrPo(pregn, 0);
	REGSC regsc1;
	REGSC regsc2;
	RC rc;

	if (pvNil == pregn)
		{
		if (prc->FEmpty())
			return fTrue;
		pregn = this;
		}

	rc.Union(prc, &pregn->_rc);
	if (rc == *prc)
		{
		// *prc totally contains the region
		SetRc(&rc);
		return fTrue;
		}

	// if this one is rectangular and already contains *prc, then
	// we're done
	if (this == pregn && pvNil == _pglxp && rc == _rc)
		return fTrue;

	regsc1.InitRc(prc, &rc);
	regsc2.Init(pregn, &rc);
	return _FUnionCore(&rc, &regsc1, &regsc2);
}


/***************************************************************************
	Core union routine.
***************************************************************************/
bool REGN::_FUnionCore(RC *prc, PREGSC pregsc1, PREGSC pregsc2)
{
	AssertThis(0);
	AssertPo(pregsc1, 0);
	AssertPo(pregsc2, 0);
	void *pvSwap;
	long dyp;
	REGBL regbl;

	if (!regbl.FInit(prc))
		return fFalse;

	for (;;)
		{
		dyp = LwMin(pregsc1->DypCur(), pregsc2->DypCur());
		if (!regbl.FStartRow(dyp, pregsc1->CxpCur() + pregsc2->CxpCur()))
			return fFalse;

		for (;;)
			{
			Assert(FPure(pregsc1->FOn()) == FPure(pregsc2->FOn()),
				"region scanners have different on/off states");
			if (pregsc1->XpCur() > pregsc2->XpCur())
				{
				//swap them
				pvSwap = pregsc1;
				pregsc1 = pregsc2;
				pregsc2 = (PREGSC)pvSwap;
				}
			if (pregsc2->XpCur() == klwMax)
				break;

			if (pregsc1->FOn())
				{
				//both on
				regbl.AddXp(pregsc1->XpCur());
				pregsc1->XpFetch();
				//1 off, 2 on
				if (pregsc1->XpCur() < pregsc2->XpCur())
					{
					regbl.AddXp(pregsc1->XpCur());
					pregsc1->XpFetch();
					continue;
					}
				pregsc2->XpFetch();
				}
			else
				{
				//both off
				pregsc1->XpFetch();
				//1 on, 2 off
				if (pregsc1->XpCur() <= pregsc2->XpCur())
					{
					pregsc1->XpFetch();
					continue;
					}
				regbl.AddXp(pregsc2->XpCur());
				pregsc2->XpFetch();
				}
			}
		Assert(pregsc1->FOn(), "bad regions");
		Assert(pregsc2->XpCur() == klwMax, 0);

		//copy the remainder of this row from pregsc1
		while (pregsc1->XpCur() < klwMax)
			{
			regbl.AddXp(pregsc1->XpCur());
			pregsc1->XpFetch();
			}
		Assert(pregsc1->FOn(), "bad region");

		regbl.EndRow();
		if (regbl.FDone())
			break;

		pregsc1->ScanNext(dyp);
		pregsc2->ScanNext(dyp);
		}

	ReleasePpo(&_pglxp);
	FreePhrgn(&_hrgn);

	_pglxp = regbl.PglxpFree(&_rc, &_dxp);
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Intersect the two regions and leave the result in this one.  If pregn2
	is nil, this region is used for pregn2.
***************************************************************************/
bool REGN::FIntersect(PREGN pregn1, PREGN pregn2)
{
	AssertThis(0);
	AssertPo(pregn1, 0);
	AssertNilOrPo(pregn2, 0);
	REGSC regsc1;
	REGSC regsc2;
	RC rc;

	if (pvNil == pregn2)
		pregn2 = this;

	if (!rc.FIntersect(&pregn1->_rc, &pregn2->_rc))
		{
		// result is empty
		SetRc(&rc);
		return fTrue;
		}

	// if pregn1 is rectangular and contains this one, then
	// we're done
	if (this == pregn2 && pvNil == pregn1->_pglxp && rc == _rc)
		return fTrue;
	if (pvNil == pregn1->_pglxp && pvNil == pregn2->_pglxp)
		{
		//both rectangles, so the intersection is a rectangle
		SetRc(&rc);
		return fTrue;
		}

	regsc1.Init(pregn1, &rc);
	regsc2.Init(pregn2, &rc);
	return _FIntersectCore(&rc, &regsc1, &regsc2);
}


/***************************************************************************
	Intersect the given rectangle and region and leave the result in this
	region.  If pregn is nil, this region is used for pregn.
***************************************************************************/
bool REGN::FIntersectRc(RC *prc, PREGN pregn)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertNilOrPo(pregn, 0);
	REGSC regsc1;
	REGSC regsc2;
	RC rc;

	if (pvNil == pregn)
		pregn = this;

	if (!rc.FIntersect(prc, &pregn->_rc))
		{
		// result is empty
		SetRc(&rc);
		return fTrue;
		}

	// if *prc contains this region, then we're done
	if (this == pregn && rc == _rc)
		return fTrue;
	if (pvNil == pregn->_pglxp)
		{
		//both rectangles, so the intersection is a rectangle
		SetRc(&rc);
		return fTrue;
		}

	regsc1.InitRc(prc, &rc);
	regsc2.Init(pregn, &rc);
	return _FIntersectCore(&rc, &regsc1, &regsc2);
}


/***************************************************************************
	Core intersect routine.
***************************************************************************/
bool REGN::_FIntersectCore(RC *prc, PREGSC pregsc1, PREGSC pregsc2)
{
	AssertThis(0);
	AssertPo(pregsc1, 0);
	AssertPo(pregsc2, 0);
	long dyp;
	void *pvSwap;
	REGBL regbl;

	if (!regbl.FInit(prc))
		return fFalse;

	for (;;)
		{
		dyp = LwMin(pregsc1->DypCur(), pregsc2->DypCur());
		if (!regbl.FStartRow(dyp, pregsc1->CxpCur() + pregsc2->CxpCur()))
			return fFalse;

		while (pregsc1->XpCur() < klwMax && pregsc2->XpCur() < klwMax)
			{
			Assert(FPure(pregsc1->FOn()) == FPure(pregsc2->FOn()),
				"region scanners have different on/off states");
			if (pregsc1->XpCur() > pregsc2->XpCur())
				{
				//swap them
				pvSwap = pregsc1;
				pregsc1 = pregsc2;
				pregsc2 = (PREGSC)pvSwap;
				}

			//NOTE: this is pretty much the adjoint of what's in _FUnionCore - ie,
			//swap the sense of FOn
			if (pregsc1->FOn())
				{
				//both on
				pregsc1->XpFetch();
				//1 off, 2 on
				if (pregsc1->XpCur() <= pregsc2->XpCur())
					{
					pregsc1->XpFetch();
					continue;
					}
				regbl.AddXp(pregsc2->XpCur());
				pregsc2->XpFetch();
				}
			else
				{
				//both off
				regbl.AddXp(pregsc1->XpCur());
				pregsc1->XpFetch();
				//1 on, 2 off
				if (pregsc1->XpCur() < pregsc2->XpCur())
					{
					regbl.AddXp(pregsc1->XpCur());
					pregsc1->XpFetch();
					continue;
					}
				pregsc2->XpFetch();
				}
			}
		Assert(pregsc1->FOn(), "bad regions");
		regbl.EndRow();
		if (regbl.FDone())
			break;

		pregsc1->ScanNext(dyp);
		pregsc2->ScanNext(dyp);
		}

	ReleasePpo(&_pglxp);
	FreePhrgn(&_hrgn);

	_pglxp = regbl.PglxpFree(&_rc, &_dxp);
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Compute regn2 minus regn1 and put the result in this region.  If
	pregn2 is nil, this region is used.
***************************************************************************/
bool REGN::FDiff(PREGN pregn1, PREGN pregn2)
{
	AssertThis(0);
	AssertPo(pregn1, 0);
	AssertNilOrPo(pregn2, 0);
	REGSC regsc1;
	REGSC regsc2;
	RC rc, rcT;

	if (pvNil == pregn2)
		{
		// use this region as the first operand
		if (!rc.FIntersect(&_rc, &pregn1->_rc))
			return fTrue;
		pregn2 = this;
		}

	if (pregn2->_rc.FEmpty())
		{
		rc.Zero();
		SetRc(&rc);
		return fTrue;
		}

	rc = pregn2->_rc;
	regsc1.Init(pregn1, &rc);
	regsc2.Init(pregn2, &rc);
	return _FDiffCore(&rc, &regsc2, &regsc1);
}


/***************************************************************************
	Compute regn minus the given rectangle and put the result in this region.
	If pregn is nil, this region is used.
***************************************************************************/
bool REGN::FDiffRc(RC *prc, PREGN pregn)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertNilOrPo(pregn, 0);
	REGSC regsc1;
	REGSC regsc2;
	RC rc;

	if (pvNil == pregn)
		{
		if (!rc.FIntersect(&_rc, prc))
			return fTrue;
		pregn = this;
		}

	if (pregn->_rc.FEmpty() || prc->FContains(&pregn->_rc))
		{
		// the rectangle contains the entire region or the region is empty
		rc.Zero();
		SetRc(&rc);
		return fTrue;
		}

	rc = pregn->_rc;
	regsc1.InitRc(prc, &rc);
	regsc2.Init(pregn, &rc);
	return _FDiffCore(&rc, &regsc2, &regsc1);
}


/***************************************************************************
	Compute *prc minus the given region and put the result in this region.
	If pregn is nil, this region is used.
***************************************************************************/
bool REGN::FDiffFromRc(RC *prc, PREGN pregn)
{
	AssertThis(0);
	AssertVarMem(prc);
	AssertNilOrPo(pregn, 0);
	REGSC regsc1;
	REGSC regsc2;
	RC rc;

	if (pvNil == pregn)
		pregn = this;

	if (!rc.FIntersect(prc, &pregn->_rc))
		{
		SetRc(prc);
		return fTrue;
		}

	if (pvNil == pregn->_pglxp && pregn->_rc.FContains(prc))
		{
		// the region is rectangle and contains *prc, so the diff is empty
		rc.Zero();
		SetRc(&rc);
		return fTrue;
		}

	regsc1.InitRc(prc, prc);
	regsc2.Init(pregn, prc);
	return _FDiffCore(prc, &regsc1, &regsc2);
}


/***************************************************************************
	Core diff routine.  Compute pregsc1 minus pregsc2.
***************************************************************************/
bool REGN::_FDiffCore(RC *prc, PREGSC pregsc1, PREGSC pregsc2)
{
	AssertThis(0);
	AssertPo(pregsc1, 0);
	AssertPo(pregsc2, 0);
	long dyp;
	long xp1, xp2;
	REGBL regbl;

	if (!regbl.FInit(prc))
		return fFalse;

	for (;;)
		{
		dyp = LwMin(pregsc1->DypCur(), pregsc2->DypCur());
		if (!regbl.FStartRow(dyp, pregsc1->CxpCur() + pregsc2->CxpCur()))
			return fFalse;

		xp1 = pregsc1->XpCur();
		xp2 = pregsc2->XpCur();
		while (xp1 < klwMax)
			{
			if (pregsc1->FOn())
				{
				while (xp2 <= xp1)
					xp2 = pregsc2->XpFetch();

				// xp1 < xp2
				if (pregsc2->FOn())
					{
					// both on
					regbl.AddXp(xp1);
					}
				xp1 = pregsc1->XpFetch();
				}

			Assert(!pregsc1->FOn(), "why are we here?");
			if (!pregsc2->FOn())
				{
				// both off
				if (xp1 <= xp2)
					{
					xp1 = pregsc1->XpFetch();
					continue;
					}

				// xp2 < xp1
				regbl.AddXp(xp2);
				xp2 = pregsc2->XpFetch();
				}

			Assert(!pregsc1->FOn() && pregsc2->FOn(), "we shouldn't be here");
			// 1 off, 2 on
			if (xp2 < xp1)
				{
				regbl.AddXp(xp2);
				xp2 = pregsc2->XpFetch();
				}
			else
				{
				regbl.AddXp(xp1);
				xp1 = pregsc1->XpFetch();
				}
			}

		Assert(pregsc1->FOn(), "bad region");
		regbl.EndRow();
		if (regbl.FDone())
			break;

		pregsc1->ScanNext(dyp);
		pregsc2->ScanNext(dyp);
		}

	ReleasePpo(&_pglxp);
	FreePhrgn(&_hrgn);

	_pglxp = regbl.PglxpFree(&_rc, &_dxp);
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Create a system region that is equivalent to the given region.
***************************************************************************/
HRGN REGN::HrgnCreate(void)
{
#ifdef WIN
	RGNDATAHEADER *prd;
	RCS *prcs;
	REGSC regsc;
	long crcMac, crc;
	HRGN hrgn;
	long yp, dyp;

	crcMac = _pglxp == pvNil ? 1 : _pglxp->IvMac() / 2;
	if (!FAllocPv((void **)&prd, size(RGNDATAHEADER) + LwMul(crcMac, size(RECT)),
			fmemNil, mprNormal))
		{
		return hNil;
		}

	prcs = (RCS *)(prd + 1);
	regsc.Init(this, &_rc);
	crc = 0;
	if (!_rc.FEmpty())
		{
		yp = _rc.ypTop;
		for (;;)
			{
			dyp = regsc.DypCur();
			while (regsc.XpCur() < klwMax)
				{
				prcs->left = regsc.XpCur() + _rc.xpLeft;
				prcs->right = regsc.XpFetch() + _rc.xpLeft;
				regsc.XpFetch();
				prcs->top = yp;
				prcs->bottom = yp + dyp;
				prcs++;
				crc++;
				}
			if ((yp += dyp) >= _rc.ypBottom)
				break;
			regsc.ScanNext(dyp);
			}
		}
	AssertIn(crc, 0, crcMac + 1);
	prd->dwSize = size(RGNDATAHEADER);
	prd->iType = RDH_RECTANGLES;
	prd->nCount = crc;
	prd->nRgnSize = 0;
	prd->rcBound = (RCS)_rc;

	hrgn = ExtCreateRegion(pvNil, size(RGNDATAHEADER) + LwMul(crc, size(RECT)),
		(RGNDATA *)prd);
	FreePpv((void **)&prd);
	return hrgn;
#endif //WIN
#ifdef MAC
	HRGN hrgn;

	if (pvNil == _pglxp)
		{
		RCS rcs;

		if (hNil == (hrgn = NewRgn()))
			return hNil;
		rcs = RCS(_rc);
		RectRgn(hrgn, &rcs);
		return hrgn;
		}

	REGSC regsc1;
	REGSC regsc2;
	RC rc;
	long yp;
	long cb;
	long xp1, xp2;
	short *psw;

	regsc1.Init(this, &_rc);
	rc = _rc;
	rc.ypTop--;
	regsc2.Init(this, &rc);
	for (yp = _rc.ypTop, cb = size(Region); ; )
		{
		cb += size(short);
		xp1 = regsc1.XpCur();
		xp2 = regsc2.XpCur();
		for (;;)
			{
			if (xp1 == xp2)
				{
				if (xp1 == klwMax)
					break;
				xp1 = regsc1.XpFetch();
				xp2 = regsc2.XpFetch();
				continue;
				}
			else if (xp1 < xp2)
				{
				cb += size(short);
				xp1 = regsc1.XpFetch();
				}
			else
				{
				cb += size(short);
				xp2 = regsc2.XpFetch();
				}
			}
		cb += size(short);
		if (yp >= _rc.ypBottom)
			break;
		yp += regsc1.DypCur();
		regsc1.ScanNext(regsc1.DypCur());
		regsc2.ScanNext(regsc2.DypCur());
		}
	cb += size(short);

	if (hNil == (hrgn = (HRGN)NewHandle(cb)))
		return hNil;

	HLock((HN)hrgn);
	(*hrgn)->rgnSize = (short)cb;
	(*hrgn)->rgnBBox = RCS(_rc);

	regsc1.Init(this, &_rc);
	rc = _rc;
	rc.ypTop--;
	regsc2.Init(this, &rc);
	for (yp = _rc.ypTop, psw = (short *)((*hrgn) + 1); ; )
		{
		*psw++ = (short)yp;
		xp1 = regsc1.XpCur();
		xp2 = regsc2.XpCur();
		for (;;)
			{
			if (xp1 == xp2)
				{
				if (xp1 == klwMax)
					break;
				xp1 = regsc1.XpFetch();
				xp2 = regsc2.XpFetch();
				continue;
				}
			else if (xp1 < xp2)
				{
				*psw++ = (short)xp1;
				xp1 = regsc1.XpFetch();
				}
			else
				{
				*psw++ = (short)xp2;
				xp2 = regsc2.XpFetch();
				}
			}
		*psw++ = 0x7FFF;
		if (yp >= _rc.ypBottom)
			break;
		yp += regsc1.DypCur();
		regsc1.ScanNext(regsc1.DypCur());
		regsc2.ScanNext(regsc2.DypCur());
		}
	*psw++ = 0x7FFF;
	Assert(psw == PvAddBv(*hrgn, cb), "wrong size!");
	HUnlock((HN)hrgn);
	return hrgn;
#endif //MAC
}


/***************************************************************************
	If we don't have a cached HRGN equivalent of this region, create one.
	In either case return it.
***************************************************************************/
HRGN REGN::HrgnEnsure(void)
{
	if (hNil != _hrgn)
		{
		if (_dptRgn.xp != 0 || _dptRgn.yp != 0)
			{
			OffsetRgn(_hrgn, (short)_dptRgn.xp, (short)_dptRgn.yp);
			_dptRgn.xp = _dptRgn.yp = 0;
			}
		return _hrgn;
		}

	_dptRgn.xp = _dptRgn.yp = 0;
	_hrgn = HrgnCreate();
	return _hrgn;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a REGN.
***************************************************************************/
void REGN::AssertValid(ulong grf)
{
	REGN_PAR::AssertValid(0);
	AssertNilOrPo(_pglxp, 0);
	Assert(_dxp <= 0, "bad _dxp");
	Assert(_dxp == 0 || _pglxp != pvNil, "_dxp should be zero");
	//REVIEW shonk: REGN::AssertValid: fill this in
}


/***************************************************************************
	Mark memory for the REGN.
***************************************************************************/
void REGN::MarkMem(void)
{
	AssertValid(0);
	REGN_PAR::MarkMem();
	MarkMemObj(_pglxp);
}
#endif //DEBUG


