/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Cursor class.

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(CURS)
	
/***************************************************************************
	Destructor for the cursor class.
***************************************************************************/
CURS::~CURS(void)
{
	#ifdef WIN
		if (hNil != _hcrs)
			DestroyCursor(_hcrs);
	#endif //WIN
}


/***************************************************************************
	Read a cursor out of a CRF.
***************************************************************************/
bool CURS::FReadCurs(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	PGG pggcurf;
	long icurf, icurfBest;
	CURF curf;
	short bo;
	long dxp, dyp, dzpT;
	long dzpBest;
	long cbRowDst, cbRowSrc, cbT;
	byte *prgb, *qrgb;
	PCURS pcurs = pvNil;

	*pcb = size(CURS);
	if (pvNil == ppbaco)
		return fTrue;

	if (pvNil == (pggcurf = GG::PggRead(pblck, &bo)) || pggcurf->IvMac() == 0)
		{
		ReleasePpo(&pggcurf);
		return fFalse;
		}

#ifdef MAC
	dxp = dyp = 16;
#endif //MAC
#ifdef WIN
	dxp = GetSystemMetrics(SM_CXCURSOR);
	dyp = GetSystemMetrics(SM_CYCURSOR);
#endif //WIN

	icurfBest = 0;
	dzpBest = klwMax;
	for (icurf = 0; icurf < pggcurf->IvMac(); icurf++)
		{
		pggcurf->GetFixed(icurf, &curf);
		if (kboOther == bo)
			SwapBytesBom(&curf, kbomCurf);
		if (curf.dxp > dxp || curf.dyp > dyp || curf.curt != curtMonochrome ||
				pggcurf->Cb(icurf) != (long)curf.dxp * curf.dyp / 4)
			{
			continue;
			}

		dzpT = (dxp - curf.dxp) + (dyp - curf.dyp);
		if (dzpBest > dzpT)
			{
			icurfBest = icurf;
			if (dzpT == 0)
				break;
			dzpBest = dzpT;
			}
		}
	AssertIn(icurfBest, 0, pggcurf->IvMac());
	pggcurf->GetFixed(icurfBest, &curf);
	if (kboOther == bo)
		SwapBytesBom(&curf, kbomCurf);
	cbRowSrc = LwRoundAway(LwDivAway(curf.dxp, 8), 2);
	cbRowDst = LwRoundAway(LwDivAway(dxp, 8), 2);

	if (!FAllocPv((void **)&prgb, LwMul(cbRowDst, 2 * dyp), fmemClear, mprNormal))
		goto LFail;

	if (pvNil == (pcurs = NewObj CURS))
		goto LFail;

	FillPb(prgb, LwMul(cbRowDst, dyp), 0xFF);
	qrgb = (byte *)pggcurf->QvGet(icurfBest);
	cbT = LwMin(cbRowSrc, cbRowDst);
	for (dzpT = LwMin(dyp, curf.dyp); dzpT-- > 0; )
		{
		CopyPb(qrgb + LwMul(dzpT, cbRowSrc), prgb + LwMul(dzpT, cbRowDst), cbT);
		CopyPb(qrgb + LwMul(curf.dyp + dzpT, cbRowSrc),
			prgb + LwMul(dyp + dzpT, cbRowDst), cbT);
		}

#ifdef WIN
	pcurs->_hcrs = CreateCursor(vwig.hinst, curf.xp, curf.yp,
		dxp, dyp, prgb, prgb + LwMul(dxp, cbRowDst));
	if (hNil == pcurs->_hcrs)
		ReleasePpo(&pcurs);
#endif //WIN
#ifdef MAC
	Assert(dxp == 16, 0);
	long *plwAnd, *plwXor;
	long ilw;

	plwAnd = (long *)prgb;
	plwXor = plwAnd + 8;
	pcurs->_crs.hotSpot.h = curf.xp;
	pcurs->_crs.hotSpot.v = curf.yp;
	for (ilw = 0; ilw < 8; ilw++)
		{
		((long *)pcurs->_crs.mask)[ilw] = ~*plwAnd;
		((long *)pcurs->_crs.data)[ilw] = ~*plwAnd++ ^ *plwXor++;
		}
#endif //MAC

LFail:
	FreePpv((void **)&prgb);
	ReleasePpo(&pggcurf);

	*ppbaco = pcurs;
	return pvNil != pcurs;
}


/***************************************************************************
	Set the cursor.
***************************************************************************/
void CURS::Set(void)
{
	#ifdef WIN
		SetCursor(_hcrs);
	#endif //WIN
	#ifdef MAC
		SetCursor(&_crs);
	#endif //MAC
}

