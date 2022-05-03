/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Basic collection classes (continued from groups.cpp):
		General List (GL), Allocated List (AL),
		General Group (GG), Allocated Group (AG),
		General String Table (GST), Allocated String Table (AST).

		BASE ---> GRPB -+-> GLB -+-> GL
		                |        +-> AL
		                |
		                +-> GGB -+-> GG
		                |        +-> AG
		                |
		                +-> GSTB-+-> GST
		                         +-> AST

***************************************************************************/
#include "util.h"
ASSERTNAME


RTCLASS(GSTB)
RTCLASS(GST)
RTCLASS(AST)


/***************************************************************************
	Constructor for a base string table.
***************************************************************************/
GSTB::GSTB(long cbExtra, ulong grfgst)
{
	AssertIn(cbExtra, 0, kcbMax);
	Assert(cbExtra % size(long) == 0, "cbExtra not multiple of size(long)");

	_cbEntry = cbExtra + size(long);
	_cbstFree = (grfgst & fgstAllowFree) ? 0 : cvNil;

	// use some reasonable values for _cbMinGrow* - code can always set
	// set these to something else
	_cbMinGrow1 = 120;
	_cbMinGrow2 = 12 * _cbEntry;
	AssertThis(fobjAssertFull);
}


/***************************************************************************
	Duplicate the string table.
***************************************************************************/
bool GSTB::_FDup(PGSTB pgstbDst)
{
	AssertThis(fobjAssertFull);
	AssertPo(pgstbDst, fobjAssertFull);
	Assert(_cbEntry == pgstbDst->_cbEntry,
		"why do these have different sized entries?");

	if (!GSTB_PAR::_FDup(pgstbDst, _bstMac, LwMul(_cbEntry, _ivMac)))
		return fFalse;

	pgstbDst->_cbEntry = _cbEntry;
	pgstbDst->_bstMac = _bstMac;
	pgstbDst->_cbstFree = _cbstFree;
	AssertPo(pgstbDst, fobjAssertFull);

	return fTrue;
}


//string table on file
struct GSTF
	{
	short bo;
	short osk;
	long cbEntry;
	long ibstMac;
	long bstMac;
	long cbstFree;
	};
const BOM kbomGstf = 0x5FF00000L;


/***************************************************************************
	Return the amount of space on file needed for the string table.
***************************************************************************/
long GSTB::CbOnFile(void)
{
	AssertThis(0);
	return size(GSTF) + LwMul(_ivMac, _cbEntry) + _bstMac;
}


/***************************************************************************
	Write the string table to disk.  It is the client's responsibility to
	ensure that the extra data has the given byte order (bo) and osk.
	If the osk has a different character size than the current one, the
	strings are actually saved in the corresponding osk with the same sized
	characters.
***************************************************************************/
bool GSTB::FWrite(PBLCK pblck, short bo, short osk)
{
	AssertThis(fobjAssertFull);
	AssertPo(pblck, 0);
	Assert(kboCur == bo || kboOther == bo, "bad bo");
	AssertOsk(osk);

	GSTF gstf;
	bool fRet;

	if (osk != koskCur)
		{
		switch (osk)
			{
		case koskSbMac:
		case koskUniMac:
			osk = koskMac;
			break;

		case koskSbWin:
		case koskUniWin:
			osk = koskWin;
			break;

		default:
			Bug("unknown osk");
			return fFalse;
			}
		}

	gstf.bo = kboCur;
	gstf.osk = osk;
	gstf.cbEntry = _cbEntry;
	gstf.ibstMac = _ivMac;
	gstf.bstMac = _bstMac;
	gstf.cbstFree = _cbstFree;
	if (kboOther == bo)
		{
		SwapBytesBom(&gstf, kbomGstf);
		Assert(gstf.osk == osk, "osk not invariant under byte swapping");
		_SwapBytesRgbst();
		}
	if (koskCur != osk)
		_TranslateGrst(osk, fFalse);
	fRet = _FWrite(pblck, &gstf, size(gstf), _bstMac, LwMul(_cbEntry, _ivMac));
	if (koskCur != osk)
		_TranslateGrst(osk, fTrue);
	if (kboOther == bo)
		_SwapBytesRgbst();
	return fRet;
}


/***************************************************************************
	Read string table data from a block.
***************************************************************************/
bool GSTB::_FRead(PBLCK pblck, short *pbo, short *posk)
{
	AssertThis(0);
	AssertPo(pblck, 0);
	AssertNilOrVarMem(pbo);
	AssertNilOrVarMem(posk);

	GSTF gstf;
	long cbT;
	short bo;
	long cb;
	bool fRet = fFalse;

	if (!pblck->FUnpackData())
		goto LFail;

	cb = pblck->Cb();
	if (cb < size(gstf))
		goto LBug;

	if (!pblck->FReadRgb(&gstf, size(gstf), 0))
		goto LFail;

	if (pbo != pvNil)
		*pbo = gstf.bo;
	if (posk != pvNil)
		*posk = gstf.osk;

	if ((bo = gstf.bo) == kboOther)
		SwapBytesBom(&gstf, kbomGstf);

	cb -= size(gstf);
	//don't use LwMul, in case the file is corrupted, we don't want to assert,
	//we should detect it below.
	cbT = gstf.cbEntry * gstf.ibstMac;
	if (gstf.bo != kboCur || !FIn(gstf.cbEntry, size(long), kcbMax) ||
		(gstf.cbEntry % size(long)) != 0 || !FIn(gstf.ibstMac, 0, kcbMax) ||
		!FIn(gstf.bstMac, gstf.ibstMac - LwMax(0, gstf.cbstFree), kcbMax) ||
		cb != cbT + gstf.bstMac ||
		(gstf.cbstFree == cvNil) != (_cbstFree == cvNil) ||
		gstf.cbstFree != cvNil && !FIn(gstf.cbstFree, 0, LwMax(1, gstf.ibstMac)))
		{
LBug:
		Warn("file corrupt or not a GSTB");
		goto LFail;
		}

	_cbEntry = gstf.cbEntry;
	_ivMac = gstf.ibstMac;
	_bstMac = gstf.bstMac;
	_cbstFree = gstf.cbstFree;
	fRet = _FReadData(pblck, cb - cbT, cbT, size(gstf));
	if (fRet)
		{
		if (bo == kboOther)
			_SwapBytesRgbst();
		if (koskCur != gstf.osk)
			fRet = _FTranslateGrst(gstf.osk);
		}

LFail:
	TrashVarIf(!fRet, pbo);
	TrashVarIf(!fRet, posk);
	return fRet;
}


/***************************************************************************
	Ensures that there is room to add at least cstnAdd new strings with
	a total of cchAdd characters.  If there is more than enough room and
	fgrpShrink is passed, the GSTB may shrink.
***************************************************************************/
bool GSTB::FEnsureSpace(long cstnAdd, long cchAdd, ulong grfgrp)
{
	AssertThis(0);
	AssertIn(cstnAdd, 0, kcbMax);
	AssertIn(cchAdd, 0, kcbMax);
	long cbstAdd;
	long cbAdd = cchAdd * size(achar);

	if (cvNil == _cbstFree)
		cbstAdd = cstnAdd;
	else
        cbstAdd = LwMax(0, cstnAdd - _cbstFree);
	if (cbstAdd > kcbMax / _cbEntry - _ivMac ||
			cstnAdd > kcbMax - _bstMac ||
			cbAdd > kcbMax - _bstMac - cstnAdd)
		{
		Bug("why is this string table growing so large?");
		return fFalse;
		}

	return _FEnsureSizes(_bstMac + cbAdd + cstnAdd,
		LwMul(_ivMac + cbstAdd, _cbEntry), grfgrp);
}


/***************************************************************************
	Set the minimum that a GSTB should grow by.
***************************************************************************/
void GSTB::SetMinGrow(long cstnAdd, long cchAdd)
{
	AssertThis(0);
	AssertIn(cstnAdd, 0, kcbMax);
	AssertIn(cchAdd, 0, kcbMax);

	_cbMinGrow1 = CbRoundToLong(cchAdd * size(achar) + cstnAdd);
	_cbMinGrow2 = LwMul(cstnAdd, _cbEntry);
}


/***************************************************************************
	Append an stn to string table.
***************************************************************************/
bool GSTB::FAddStn(PSTN pstn, void *pvExtra, long *pistn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	return FAddRgch(pstn->Prgch(), pstn->Cch(), pvExtra, pistn);
}


/***************************************************************************
	Replace the ith string.
***************************************************************************/
bool GSTB::FPutRgch(long istn, achar *prgch, long cch)
{
	AssertThis(fobjAssertFull);
	AssertIn(istn, 0, _ivMac);
	Assert(!FFree(istn), "string entry is free!");
	AssertIn(cch, 0, kcchMaxGst + 1);
	AssertPvCb(prgch, cch * size(achar));

	long cchOld;
	long bstOld;
	achar *qst;

	qst = _Qst(istn);
	if ((cchOld = CchSt(qst)) == cch)
		{
		CopyPb(prgch, PrgchSt(qst), cch * size(achar));
		goto LDone;
		}
	if (cchOld < cch && !_FEnsureSizes(_bstMac + (cch - cchOld) * size(achar),
			LwMul(_ivMac, _cbEntry), fgrpNil))
		{
		return fFalse;
		}

	// remove the old one
	bstOld = _Bst(istn);
	_RemoveSt(bstOld);

	// append the new one
	*_Qbst(istn) = _bstMac;
	_AppendRgch(prgch, cch);

LDone:
	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Replace the ith string with stn.
***************************************************************************/
bool GSTB::FPutStn(long istn, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	return FPutRgch(istn, pstn->Prgch(), pstn->Cch());
}


/***************************************************************************
	Get up to cchMax characters for the istn'th string.
***************************************************************************/
void GSTB::GetRgch(long istn, achar *prgch, long cchMax, long *pcch)
{
	AssertThis(0);
	AssertIn(istn, 0, _ivMac);
	Assert(!FFree(istn), "string entry is free!");
	AssertIn(cchMax, 0, kcbMax);
	AssertPvCb(prgch, cchMax * size(achar));
	AssertVarMem(pcch);
	PST qst = _Qst(istn);

	*pcch = LwMin(cchMax, CchSt(qst));
	CopyPb(PrgchSt(qst), prgch, *pcch * size(achar));
}


/***************************************************************************
	Get the ith string.
***************************************************************************/
void GSTB::GetStn(long istn, PSTN pstn)
{
	AssertThis(0);
	AssertIn(istn, 0, _ivMac);
	Assert(!FFree(istn), "string entry is free!");
	AssertPo(pstn, 0);

	pstn->SetSt(_Qst(istn));
}


/***************************************************************************
	Find the given stn in the string table.
***************************************************************************/
bool GSTB::FFindStn(PSTN pstn, long *pistn, ulong grfgst)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	return FFindRgch(pstn->Prgch(), pstn->Cch(), pistn, grfgst);
}


/***************************************************************************
	Search for the string in the string table.  This version does a linear
	search.  GST overrides this to do a binary search if fgstSorted is
	passed in grfgst.
***************************************************************************/
bool GSTB::FFindRgch(achar *prgch, long cch, long *pistn, ulong grfgst)
{
	AssertThis(0);
	AssertIn(cch, 0, kcchMaxGst + 1);
	AssertPvCb(prgch, cch * size(achar));
	AssertVarMem(pistn);
	long istn, bst;
	PST qst;

	for (istn = 0; istn < _ivMac; istn++)
		{
		bst = _Bst(istn);
		if (bvNil == bst)
			{
			AssertIn(_cbstFree, 0, _ivMac);
			Assert(istn < _ivMac - 1, "last element free");
			continue;
			}
		AssertIn(bst, 0, _bstMac);
		qst = (achar *)_Qb1(bst);
		if (CchSt(qst) == cch && FEqualRgb(PrgchSt(qst), prgch, cch * size(achar)))
			{
			*pistn = istn;
			return fTrue;
			}
		}
	*pistn = istn;
	return fFalse;
}


/***************************************************************************
	Find the string with the given extra data in the string table.
***************************************************************************/
bool GSTB::FFindExtra(void *prgbFind, PSTN pstn, long *pistn)
{
	AssertThis(0);
	AssertPvCb(prgbFind, CbExtra());
	AssertNilOrPo(pstn, 0);
	AssertNilOrVarMem(pistn);

	long istn;
	byte *qbExtra;
	long cbExtra = CbExtra();
	long ivMac = IvMac();

	if (cbExtra == 0)
		{
		Bug("no extra data");
		TrashVar(pistn);
		return fFalse;
		}

	for (istn = 0; istn < ivMac; istn++)
		{
		qbExtra = (byte *)_Qbst(istn) + size(long);
		if (FEqualRgb(prgbFind, qbExtra, cbExtra))
			{
			if (pstn != pvNil)
				GetStn(istn, pstn);
			if (pistn != pvNil)
				*pistn = istn;
			return fTrue;
			}
		}

	if (pvNil != pstn)
		pstn->SetNil();
	TrashVar(pistn);
	return fFalse;
}


/***************************************************************************
	Fetch the extra data for element istn.
***************************************************************************/
void GSTB::GetExtra(long istn, void *pv)
{
	AssertThis(0);
	AssertIn(istn, 0, _ivMac);
	Assert(!FFree(istn), "string entry is free!");
	Assert(_cbEntry > size(long), "no extra data");
	AssertPvCb(pv, _cbEntry - size(long));

	byte *qb;

	qb = (byte *)_Qbst(istn) + size(long);
	CopyPb(qb, pv, _cbEntry - size(long));
}


/***************************************************************************
	Put the extra data for element istn.
***************************************************************************/
void GSTB::PutExtra(long istn, void *pv)
{
	AssertThis(0);
	AssertIn(istn, 0, _ivMac);
	Assert(!FFree(istn), "string entry is free!");
	Assert(_cbEntry > size(long), "no extra data");
	AssertPvCb(pv, _cbEntry - size(long));

	byte *qb;

	qb = (byte *)_Qbst(istn) + size(long);
	CopyPb(pv, qb, _cbEntry - size(long));
	AssertThis(0);
}


/***************************************************************************
	Return a volatile pointer to the string given the ibst (not the bst).
***************************************************************************/
achar *GSTB::_Qst(long ibst)
{
	AssertIn(ibst, 0, _ivMac);
	long bst = _Bst(ibst);
	AssertIn(bst, 0, _bstMac);
	return (achar *)_Qb1(bst);
}


/***************************************************************************
	Private api to append the string.  It's assumed that the first block
	is already big enough to accomodate the string.
***************************************************************************/
void GSTB::_AppendRgch(achar *prgch, long cch)
{
	AssertIn(cch, 0, kcchMaxGst + 1);
	AssertPvCb(prgch, cch * size(achar));

	achar *qch;

	Assert(_Cb1() >= _bstMac + (cch + 1) * size(achar),
		"first block not big enough");
	qch = (achar *)_Qb1(_bstMac);
	*qch++ = (achar)cch;
	CopyPb(prgch, qch, cch * size(achar));
	_bstMac += (cch + 1) * size(achar);
}


/***************************************************************************
	Private api to remove the string.
***************************************************************************/
void GSTB::_RemoveSt(long bst)
{
	AssertIn(bst, 0, _bstMac);

	long cb;
	byte *qb;

	qb = _Qb1(bst);
	cb = CchTotSt((PST)qb) * size(achar);
	AssertIn(bst + cb, 0, _bstMac + 1);
	if (bst + cb < _bstMac)
		{
		long cv;

		BltPb(qb + cb, qb, _bstMac - bst - cb);
		for (qb = _Qb2(0), cv = _ivMac; cv-- > 0; qb += _cbEntry)
			{
			if (*(long *)qb > bst)
				{
				Assert(*(long *)qb >= bst + cb, "overlapping strings in GSTB");
				*(long *)qb -= cb;
				}
			}
		}
	_bstMac -= cb;
}


/***************************************************************************
	Swap the bytes in the BST values.  Note that each bst is followed
	by the extra data, so we can't just use SwapBytesRglw.
***************************************************************************/
void GSTB::_SwapBytesRgbst(void)
{
	if (size(long) == _cbEntry)
		SwapBytesRglw(_Qb2(0), _ivMac);
	else
		{
		long cbst;
		byte *qb;

		for (cbst = _ivMac, qb = _Qb2(0); cbst-- > 0; qb += _cbEntry)
			SwapBytesRglw(qb, 1);
		}
}


/***************************************************************************
	Translate the strings to/from the platform osk.  This only works if
	CbCharOsk(osk) == CbCharOsk(koskCur) (it asserts otherwise).
***************************************************************************/
void GSTB::_TranslateGrst(short osk, bool fToCur)
{
	AssertOsk(osk);
	long bst;
	byte *qb;

	if (CbCharOsk(osk) != CbCharOsk(koskCur))
		{
		Bug("can't translate to this osk");
		return;
		}

	qb = _Qb1(0);
	for (bst = 0; bst < _bstMac; )
		{
		TranslateSt((achar *)(qb + bst), osk, fToCur);
		bst += CchTotSt((PST)(qb + bst)) * size(achar);
		}
}


/***************************************************************************
	Translate the strings to the current osk.
***************************************************************************/
bool GSTB::_FTranslateGrst(short osk)
{
	AssertOsk(osk);
	void *pvSrc;
	long cbSrc;
	long cbChar;
	long ibst, bstOld, cch;
	long *qbst;
	achar rgch[kcchMaxSt];
	bool fRet = fFalse;

	if ((cbChar = CbCharOsk(osk)) == CbCharOsk(koskCur))
		{
		_TranslateGrst(osk, fTrue);
		return fTrue;
		}

	if (cbChar != size(schar) && cbChar != size(wchar))
		{
		Bug("unknown osk");
		return fFalse;
		}

	if (_bstMac == 0)
		return fTrue;

	if (!FAllocPv(&pvSrc, cbSrc = _bstMac, fmemNil, mprNormal))
		return fFalse;

	CopyPb(_Qb1(0), pvSrc, cbSrc);
	_bstMac = 0;

	for (ibst = 0 ; ibst < _ivMac; ibst++)
		{
		qbst = _Qbst(ibst);
		bstOld = *qbst;
		*qbst = _bstMac;

		if (bstOld + cbChar > cbSrc)
			goto LFail;

		if (cbChar == size(schar))
			{
			schar chs = *(schar *)PvAddBv(pvSrc, bstOld);
			cch = (long)(byte)chs;
			}
		else
			{
			wchar chw;
			CopyPb(PvAddBv(pvSrc, bstOld), &chw, size(wchar));
			if (osk == MacWin(koskUniWin, koskUniMac))
				SwapBytesRgsw(&chw, 1);
			cch = (long)(ushort)chw;
			}

		if (!FIn(cch, 0, kcchMaxSt + 1) || bstOld + (cch + 1) * cbChar > cbSrc)
			goto LFail;

		cch = CchTranslateRgb(PvAddBv(pvSrc, bstOld + cbChar), cch * cbChar,
			osk, rgch, kcchMaxSt);

		if (!_FEnsureSizes(_bstMac + (cch + 1) * size(achar),
				LwMul(_ivMac, _cbEntry), fgrpNil))
			{
			goto LFail;
			}
		_AppendRgch(rgch, cch);
		}

	fRet = fTrue;
	AssertDo(_FEnsureSizes(_bstMac, LwMul(_ivMac, _cbEntry), fgrpShrink), 0);

LFail:
	FreePpv(&pvSrc);
	return fRet;
}


/***************************************************************************
	Returns true iff ibst is out of range or the corresponding bst is
	bvNil.
***************************************************************************/
bool GSTB::FFree(long istn)
{
	AssertIn(istn, 0, kcbMax);
	long bst;

	if (!FIn(istn, 0, _ivMac))
		return fTrue;
	bst = _Bst(istn);
	Assert(bvNil == bst || FIn(bst, 0, _bstMac), "bad bst");
	return bvNil == bst;
}


#ifdef DEBUG
/***************************************************************************
	Validate a string table.
***************************************************************************/
void GSTB::AssertValid(ulong grfobj)
{
	long ibst;
	long cchTot, cbstFree;
	long bst;

	GSTB_PAR::AssertValid(grfobj);
	AssertIn(_cbEntry, size(long), kcbMax);
	Assert(_cbEntry % size(long) == 0, "_cbEntry bad");
	AssertIn(_ivMac, 0, kcbMax);
	Assert(_Cb1() >= _bstMac, "grst area too small");
	Assert(_Cb2() >= LwMul(_ivMac, _cbEntry), "rgbst area too small");
	Assert(_cbstFree == cvNil || FIn(_cbstFree, 0, LwMax(1, _ivMac)),
		"_cbstFree is wrong");

	if (grfobj & fobjAssertFull)
		{
		// it would be nice to assert that no strings overlap each other,
		// but that's hard to do in linear time, so just assert that the
		// grst is the correct size.
		for (cbstFree = cchTot = ibst = 0; ibst < _ivMac; ibst++)
			{
			bst = _Bst(ibst);
			if (bvNil == bst)
				{
				Assert(ibst < _ivMac - 1, "last element free");
				cbstFree++;
				continue;
				}
			AssertIn(bst, 0, _bstMac);
			AssertSt((achar *)_Qb1(bst));
			cchTot += CchTotSt((PST)_Qb1(bst));
			}
		Assert(cchTot * size(achar) == _bstMac, "grst wrong size");
		Assert(cbstFree == _cbstFree || _cbstFree == cvNil && cbstFree == 0,
			"bad _cbstFree");
		}
}
#endif //DEBUG


/***************************************************************************
	Allocate a new string table and ensure that it has space for cstnInit
	strings, totalling cchInit characters.
***************************************************************************/
PGST GST::PgstNew(long cbExtra, long cstnInit, long cchInit)
{
	AssertIn(cbExtra, 0, kcbMax);
	Assert(cbExtra % size(long) == 0, "cbExtra not multiple of size(long)");
	AssertIn(cstnInit, 0, kcbMax);
	AssertIn(cchInit, 0, kcbMax);
	PGST pgst;

	if ((pgst = NewObj GST(cbExtra)) == pvNil)
		return pvNil;
	if ((cstnInit > 0 || cchInit > 0) &&
			!pgst->FEnsureSpace(cstnInit, cchInit, fgrpNil))
		{
		ReleasePpo(&pgst);
		return pvNil;
		}
	AssertPo(pgst, 0);
	return pgst;
}


/***************************************************************************
	Read a string table from a block and return it.
***************************************************************************/
PGST GST::PgstRead(PBLCK pblck, short *pbo, short *posk)
{
	AssertPo(pblck, 0);
	AssertNilOrVarMem(pbo);
	AssertNilOrVarMem(posk);

	PGST pgst;

	if ((pgst = NewObj GST(0)) == pvNil)
		goto LFail;
	if (!pgst->_FRead(pblck, pbo, posk))
		{
		ReleasePpo(&pgst);
LFail:
		TrashVar(pbo);
		TrashVar(posk);
		return pvNil;
		}
	AssertPo(pgst, 0);
	return pgst;
}


/***************************************************************************
	Read a string table from file and return it.
***************************************************************************/
PGST GST::PgstRead(PFIL pfil, FP fp, long cb, short *pbo, short *posk)
{
	BLCK blck(pfil, fp, cb);
	return PgstRead(&blck, pbo, posk);
}


/***************************************************************************
	Duplicate this GST.
***************************************************************************/
PGST GST::PgstDup(void)
{
	AssertThis(0);
	PGST pgst;

	if (pvNil == (pgst = PgstNew(_cbEntry - size(long))))
		return pvNil;

	if (!_FDup(pgst))
		ReleasePpo(&pgst);

	AssertNilOrPo(pgst, 0);
	return pgst;
}


/***************************************************************************
	Append a string to the string table.
***************************************************************************/
bool GST::FAddRgch(achar *prgch, long cch, void *pvExtra, long *pistn)
{
	AssertThis(0);
	AssertIn(cch, 0, kcchMaxGst + 1);
	AssertPvCb(prgch, cch * size(achar));
	AssertNilOrPvCb(pvExtra, _cbEntry - size(long));
	AssertNilOrVarMem(pistn);

	if (pistn != pvNil)
		*pistn = _ivMac;
	if (!FInsertRgch(_ivMac, prgch, cch, pvExtra))
		{
		TrashVar(pistn);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Find the given string and put its location in *pistn.  If it's not
	there, fill *pistn with where it would be.  If fgstSorted or fgstUserSorted
	is passed in, this does a binary search for the string; otherwise it
	does a linear search.
***************************************************************************/
bool GST::FFindRgch(achar *prgch, long cch, long *pistn, ulong grfgst)
{
	AssertThis(0);
	AssertIn(cch, 0, kcchMaxGst);
	AssertPvCb(prgch, cch * size(achar));
	AssertVarMem(pistn);

	if (!(grfgst & (fgstSorted | fgstUserSorted)))
		return GSTB::FFindRgch(prgch, cch, pistn, grfgst);

	//the table should be sorted, so do a binary search
	long ivMin, ivLim, iv;
	ulong fcmp;
	achar *qst;

	for (ivMin = 0, ivLim = _ivMac; ivMin < ivLim; )
		{
		iv = (ivMin + ivLim) / 2;
		qst = _Qst(iv);
		if (grfgst & fgstUserSorted)
			fcmp = FcmpCompareUserRgch(prgch, cch, PrgchSt(qst), CchSt(qst));
		else
			fcmp = FcmpCompareRgch(prgch, cch, PrgchSt(qst), CchSt(qst));

		if (fcmp == fcmpLt)
			ivLim = iv;
		else if (fcmp == fcmpGt)
			ivMin = iv + 1;
		else
			{
			Assert(fcmp == fcmpEq, "bad fcmp");
			*pistn = iv;
			return fTrue;
			}
		}
	// this is where it would be
	*pistn = ivMin;
	return fFalse;
}


/***************************************************************************
	Insert a new entry into the string text.
***************************************************************************/
bool GST::FInsertRgch(long istn, achar *prgch, long cch, void *pvExtra)
{
	AssertThis(fobjAssertFull);
	AssertIn(istn, 0, _ivMac + 1);
	AssertIn(cch, 0, kcchMaxGst + 1);
	AssertPvCb(prgch, cch * size(achar));
	AssertNilOrPvCb(pvExtra, _cbEntry - size(long));

	byte *qb;

	if (!_FEnsureSizes(_bstMac + (cch + 1) * size(achar),
			LwMul(_ivMac + 1, _cbEntry), fgrpNil))
		{
		return fFalse;
		}

	// make room for the entry
	qb = (byte *)_Qbst(istn);
	if (istn < _ivMac)
		BltPb(qb, qb + _cbEntry, LwMul(_ivMac - istn, _cbEntry));
	*(long *)qb = _bstMac;
	if (size(long) < _cbEntry)
		{
		qb += size(long);
		if (pvExtra != pvNil)
			CopyPb(pvExtra, qb, _cbEntry - size(long));
		else
			TrashPvCb(qb, _cbEntry - size(long));
		}
	else
		Assert(pvNil == pvExtra, "cbExtra is zero");

	_ivMac++;
	// put the string in
	_AppendRgch(prgch, cch);

	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Insert an stn into the string table
***************************************************************************/
bool GST::FInsertStn(long istn, PSTN pstn, void *pvExtra)
{
	AssertThis(0);
	AssertIn(istn, 0, _ivMac + 1);
	AssertPo(pstn, 0);
	AssertNilOrPvCb(pvExtra, _cbEntry - size(long));

	return FInsertRgch(istn, pstn->Prgch(), pstn->Cch(), pvExtra);
}


/***************************************************************************
	Delete the string at location istn.
***************************************************************************/
void GST::Delete(long istn)
{
	AssertThis(fobjAssertFull);
	AssertIn(istn, 0, _ivMac);

	byte *qb;
	long bst;

	qb = (byte *)_Qbst(istn);
	bst = *(long *)qb;
	if (istn < --_ivMac)
		BltPb(qb + _cbEntry, qb, LwMul(_ivMac - istn, _cbEntry));
	TrashPvCb(_Qbst(_ivMac), _cbEntry);
	_RemoveSt(bst);
	AssertThis(fobjAssertFull);
}


/***************************************************************************
	Move the entry at ivSrc to be immediately before the element that is
	currently at ivTarget.  If ivTarget > ivSrc, the entry actually ends
	up at (ivTarget - 1) and the entry at ivTarget doesn't move.  If
	ivTarget < ivSrc, the entry ends up at ivTarget and the entry at
	ivTarget moves to (ivTarget + 1).  Everything in between is shifted
	appropriately.  ivTarget is allowed to be equal to IvMac().
***************************************************************************/
void GST::Move(long ivSrc, long ivTarget)
{
	AssertThis(0);
	AssertIn(ivSrc, 0, _ivMac);
	AssertIn(ivTarget, 0, _ivMac + 1);

	MoveElement(_Qbst(0), _cbEntry, ivSrc, ivTarget);
	AssertThis(0);
}


#ifdef DEBUG
/***************************************************************************
	Validate a string table.
***************************************************************************/
void GST::AssertValid(ulong grfobj)
{
	GST_PAR::AssertValid(grfobj);
	AssertVar(_cbstFree == cvNil, "bad _cbstFree in GST", &_cbstFree);
}
#endif //DEBUG


/***************************************************************************
	Allocate a new allocated string table and ensure that it has space for
	cstnInit strings, totalling cchInit characters.
***************************************************************************/
PAST AST::PastNew(long cbExtra, long cstnInit, long cchInit)
{
	AssertIn(cbExtra, 0, kcbMax);
	Assert(cbExtra % size(long) == 0, "cbExtra not multiple of size(long)");
	AssertIn(cstnInit, 0, kcbMax);
	AssertIn(cchInit, 0, kcbMax);
	PAST past;

	if ((past = NewObj AST(cbExtra)) == pvNil)
		return pvNil;
	if ((cstnInit > 0 || cchInit > 0) &&
			!past->FEnsureSpace(cstnInit, cchInit, fgrpNil))
		{
		ReleasePpo(&past);
		return pvNil;
		}
	AssertPo(past, 0);
	return past;
}


/***************************************************************************
	Read an allocated string table from a block and return it.
***************************************************************************/
PAST AST::PastRead(PBLCK pblck, short *pbo, short *posk)
{
	AssertPo(pblck, 0);
	AssertNilOrVarMem(pbo);
	AssertNilOrVarMem(posk);

	PAST past;

	if ((past = NewObj AST(0)) == pvNil)
		goto LFail;
	if (!past->_FRead(pblck, pbo, posk))
		{
		ReleasePpo(&past);
LFail:
		TrashVar(pbo);
		TrashVar(posk);
		return pvNil;
		}
	AssertPo(past, 0);
	return past;
}


/***************************************************************************
	Read an allocated string table from file and return it.
***************************************************************************/
PAST AST::PastRead(PFIL pfil, FP fp, long cb, short *pbo, short *posk)
{
	BLCK blck;
	return PastRead(&blck, pbo, posk);
}


/***************************************************************************
	Duplicate this AST.
***************************************************************************/
PAST AST::PastDup(void)
{
	AssertThis(0);
	PAST past;

	if (pvNil == (past = PastNew(_cbEntry - size(long))))
		return pvNil;

	if (!_FDup(past))
		ReleasePpo(&past);

	AssertNilOrPo(past, 0);
	return past;
}


/***************************************************************************
	Append a string to the allocated string table.
***************************************************************************/
bool AST::FAddRgch(achar *prgch, long cch, void *pvExtra, long *pistn)
{
	AssertThis(fobjAssertFull);
	AssertIn(cch, 0, kcchMaxGst + 1);
	AssertPvCb(prgch, cch * size(achar));
	AssertNilOrPvCb(pvExtra, _cbEntry - size(long));
	AssertNilOrVarMem(pistn);

	long ibst;
	byte *qb;

	if (_cbstFree > 0)
		{
		/* find the first free bst */
		qb = _Qb2(0);
		for (ibst = 0; ibst < _ivMac; ibst++, qb += _cbEntry)
			{
			if (*(long *)qb == bvNil)
				break;
			}
		Assert(ibst < _ivMac - 1, "_cbstFree is wrong");
		_cbstFree--;
		}
	else
		ibst = _ivMac++;

	if (!_FEnsureSizes(_bstMac + (cch + 1) * size(achar),
			LwMul(_ivMac, _cbEntry), fgrpNil))
		{
		if (ibst == _ivMac - 1)
			_ivMac--;
		else
			_cbstFree++;
		TrashVar(pistn);
		return fFalse;
		}

	// fill in the bst and extra data
	qb = (byte *)_Qbst(ibst);
	*(long *)qb = _bstMac;
	if (size(long) < _cbEntry)
		{
		qb += size(long);
		if (pvExtra != pvNil)
			CopyPb(pvExtra, qb, _cbEntry - size(long));
		else
			TrashPvCb(qb, _cbEntry - size(long));
		}
	else
		Assert(pvNil == pvExtra, "cbExtra is zero");

	// put the string in
	_AppendRgch(prgch, cch);

	if (pvNil != pistn)
		*pistn = ibst;
	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Delete the string at location istn.
***************************************************************************/
void AST::Delete(long istn)
{
	AssertThis(fobjAssertFull);
	AssertIn(istn, 0, _ivMac);
	Assert(!FFree(istn), "entry already free!");

	byte *qb;
	long bst;

	qb = (byte *)_Qbst(istn);
	bst = *(long *)qb;

	if (istn == _ivMac - 1)
		{
		//move _ivMac back past any free entries on the end
		while (--_ivMac > 0 && *(long *)(qb -= _cbEntry) == bvNil)
			_cbstFree--;
		TrashPvCb(_Qbst(_ivMac), LwMul(istn - _ivMac + 1, _cbEntry));
		}
	else
		{
		*(long *)qb = bvNil;
		TrashPvCb(qb + size(long), _cbEntry - size(long));
		_cbstFree++;
		}
	_RemoveSt(bst);
	AssertThis(fobjAssertFull);
}


#ifdef DEBUG
/***************************************************************************
	Validate a string table.
***************************************************************************/
void AST::AssertValid(ulong grfobj)
{
	AST_PAR::AssertValid(grfobj);
	AssertIn(_cbstFree, 0, LwMax(1, _ivMac));
}
#endif //DEBUG



