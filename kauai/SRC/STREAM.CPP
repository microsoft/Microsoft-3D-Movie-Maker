/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Stream classes.  BSM is totally in memory.  BSF allows a stream
	to have pieces in files and other pieces in memory.

***************************************************************************/
#include "util.h"
ASSERTNAME


//min size of initial piece on file
const long kcbMinFloFile = 2048;

RTCLASS(BSM)
RTCLASS(BSF)


/***************************************************************************
	Constructor for an in-memory byte stream.
***************************************************************************/
BSM::BSM(void)
{
	_hqrgb = hqNil;
	_ibMac = 0;
	_cbMinGrow = 0;
	AssertThis(fobjAssertFull);
}


/***************************************************************************
	Destructor for an in-memory byte stream.
***************************************************************************/
BSM::~BSM(void)
{
	AssertThis(fobjAssertFull);

	FreePhq(&_hqrgb);
}


/***************************************************************************
	Set the amount to grow by.
***************************************************************************/
void BSM::SetMinGrow(long cb)
{
	AssertThis(0);
	AssertIn(cb, 0, kcbMax);

	_cbMinGrow = cb;
}


/***************************************************************************
	Make sure there is at least cb bytes of space.  If fShrink is true,
	the amount of memory allocated by the BSM may decrease.
***************************************************************************/
bool BSM::FEnsureSpace(long cb, bool fShrink)
{
	AssertThis(0);

	return _FEnsureSize(_ibMac + cb, fShrink);
}


/***************************************************************************
	Return a locked pointer into the byte stream.  The stream is stored
	contiguously.
***************************************************************************/
void *BSM::PvLock(long ib)
{
	AssertThis(0);
	AssertIn(ib, 0, _ibMac + 1);

	if (hqNil == _hqrgb)
		return pvNil;
	return PvAddBv(PvLockHq(_hqrgb), ib);
}


/***************************************************************************
	Unlock the stream.
***************************************************************************/
void BSM::Unlock(void)
{
	AssertThis(0);

	if (hqNil != _hqrgb)
		UnlockHq(_hqrgb);
}


/***************************************************************************
	Fetch some bytes from the stream.
***************************************************************************/
void BSM::FetchRgb(long ib, long cb, void *prgb)
{
	AssertThis(0);
	AssertIn(ib, 0, _ibMac + 1);
	AssertIn(cb, 0, _ibMac + 1 - ib);
	AssertPvCb(prgb, cb);

	if (cb > 0)
		CopyPb(PvAddBv(QvFromHq(_hqrgb), ib), prgb, cb);
}


/***************************************************************************
	Replace the range [ib,ib + cbDel) with cbIns bytes from prgb.  If cbIns
	is zero, prgb may be nil.
***************************************************************************/
bool BSM::FReplace(void *prgb, long cbIns, long ib, long cbDel)
{
	AssertThis(fobjAssertFull);
	AssertIn(cbIns, 0, kcbMax);
	AssertPvCb(prgb, cbIns);
	AssertIn(ib, 0, _ibMac + 1);
	AssertIn(cbDel, 0, _ibMac + 1 - ib);
	byte *qrgb;

	if (!_FEnsureSize(_ibMac + cbIns - cbDel, fFalse))
		return fFalse;

	qrgb = (byte *)QvFromHq(_hqrgb);
	if (ib < _ibMac - cbDel && cbDel != cbIns)
		BltPb(qrgb + ib + cbDel, qrgb + ib + cbIns, _ibMac - cbDel - ib);
	if (cbIns > 0)
		CopyPb(prgb, qrgb + ib, cbIns);
	_ibMac += cbIns - cbDel;
	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Write the byte stream to a file.
***************************************************************************/
bool BSM::FWriteRgb(PFLO pflo, long ib)
{
	AssertThis(0);
	AssertPo(pflo, 0);
	BLCK blck(pflo);

	return FWriteRgb(&blck, ib);
}


/***************************************************************************
	Write the byte stream to a block.
***************************************************************************/
bool BSM::FWriteRgb(PBLCK pblck, long ib)
{
	AssertThis(fobjAssertFull);
	AssertPo(pblck, 0);
	AssertIn(ib, 0, CbOfHq(_hqrgb) - pblck->Cb() + 1);
	bool fRet;

	if (ib + pblck->Cb() > _ibMac)
		{
		Bug("blck is too big");
		return fFalse;
		}
	if (pblck->Cb() == 0)
		return fTrue;
	fRet = pblck->FWrite(PvAddBv(PvLockHq(_hqrgb), ib));
	UnlockHq(_hqrgb);
	return fRet;
}


/***************************************************************************
	Make sure the hq is at least cbMin bytes.  If fShrink is true, make the
	hq exactly cbMin bytes long.
***************************************************************************/
bool BSM::_FEnsureSize(long cbMin, bool fShrink)
{
	AssertThis(fobjAssertFull);
	AssertIn(cbMin, 0, kcbMax);
	long cb = hqNil == _hqrgb ? 0 : CbOfHq(_hqrgb);

	if (cbMin <= cb && (!fShrink || cbMin == cb))
		return fTrue;

	if (cbMin == 0)
		{
		if (fShrink)
			FreePhq(&_hqrgb);
		return fTrue;
		}

	if (hqNil == _hqrgb)
		{
		return FAllocHq(&_hqrgb, LwMax(cbMin, _cbMinGrow), fmemNil, mprNormal) ||
			_cbMinGrow > cbMin && FAllocHq(&_hqrgb, cbMin, fmemNil, mprNormal);
		}

	return FResizePhq(&_hqrgb, LwMax(cbMin, cb + _cbMinGrow), fmemNil, mprNormal) ||
		cb + _cbMinGrow > cbMin && FResizePhq(&_hqrgb, cbMin, fmemNil, mprNormal);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a byte stream (BSF).
***************************************************************************/
void BSM::AssertValid(ulong grf)
{
	BSM_PAR::AssertValid(grf);
	AssertIn(_ibMac, 0, kcbMax);
	AssertIn(_cbMinGrow, 0, kcbMax);
	if (_ibMac > 0)
		{
		AssertHq(_hqrgb);
		AssertIn(_ibMac, 0, CbOfHq(_hqrgb) + 1);
		}
	else if (hqNil != _hqrgb)
		AssertHq(_hqrgb);
}


/***************************************************************************
	Mark memory for the BSM.
***************************************************************************/
void BSM::MarkMem(void)
{
	AssertValid(fobjAssertFull);
	BSM_PAR::MarkMem();
	MarkHq(_hqrgb);
}
#endif //DEBUG


/***************************************************************************
	Constructor for a stream.
***************************************************************************/
BSF::BSF(void)
{
	_pggflo = pvNil;
	_ibMac = 0;
	AssertThis(fobjAssertFull);
}


/***************************************************************************
	Destructor for a stream.
***************************************************************************/
BSF::~BSF(void)
{
	AssertThis(fobjAssertFull);
	long iflo;
	FLO flo;

	if (0 < _ibMac)
		{
		//release all our reference counts
		for (iflo = _pggflo->IvMac(); iflo-- > 0; )
			{
			_pggflo->GetFixed(iflo, &flo);
			if (pvNil != flo.pfil)
				ReleasePpo(&flo.pfil);
			}
		}
	ReleasePpo(&_pggflo);
}


/***************************************************************************
	Find the flo that contains the given ib and assign *pib the postion
	of the first byte in the flo and *pcb the size of the flo.
***************************************************************************/
long BSF::_IfloFind(long ib, long *pib, long *pcb)
{
	AssertBaseThis(0);
	AssertIn(ib, 0, _ibMac + 1);
	AssertVarMem(pib);
	AssertNilOrVarMem(pcb);
	long iflo, cb;
	FLO flo;

	iflo = 0;
	if (_ibMac > 0)
		{
		for (cb = ib; iflo < _pggflo->IvMac(); iflo++)
			{
			_pggflo->GetFixed(iflo, &flo);
			if (flo.cb > cb)
				{
				*pib = ib - cb;
				if (pvNil != pcb)
					*pcb = flo.cb;
				return iflo;
				}
			cb -= flo.cb;
			}
		}
	*pib = _ibMac;
	if (pvNil != pcb)
		*pcb = 0;
	return iflo;
}


/***************************************************************************
	Make sure ib is on a piece boundary.
***************************************************************************/
bool BSF::_FEnsureSplit(long ib, long *piflo)
{
	AssertBaseThis(0);
	AssertIn(ib, 0, _ibMac + 1);
	AssertNilOrVarMem(piflo);
	long iflo, ibMin, cbT;
	FLO flo;

	if (pvNil == _pggflo)
		{
		if (pvNil == (_pggflo = GG::PggNew(size(FLO))))
			return fFalse;
		//REVIEW shonk: what values should we use for SetMinGrow?
		//_pggflo->SetMinGrow(2, 100);
		}

	iflo = _IfloFind(ib, &ibMin, &cbT);
	AssertIn(ib, ibMin, ibMin + cbT + (ibMin == _ibMac));
	if (ib == ibMin)
		{
		if (pvNil != piflo)
			*piflo = iflo;
		return fTrue;
		}

	_pggflo->GetFixed(iflo, &flo);
	Assert(cbT == flo.cb, 0);

	cbT = ib - ibMin;
	flo.cb -= cbT;
	if (pvNil != flo.pfil)
		flo.fp += cbT;
	if (!_pggflo->FInsert(iflo + 1, 0, pvNil, &flo))
		{
		TrashVar(piflo);
		return fFalse;
		}
	if (pvNil == flo.pfil && !_pggflo->FMoveRgb(iflo, cbT, iflo + 1, 0, flo.cb))
		{
		_pggflo->Delete(iflo + 1);
		TrashVar(piflo);
		return fFalse;
		}
	flo.cb = cbT;
	if (pvNil != flo.pfil)
		{
		flo.fp -= cbT;
		flo.pfil->AddRef();
		}
	_pggflo->PutFixed(iflo, &flo);
	if (pvNil != piflo)
		*piflo = iflo + 1;
	return fTrue;
}


/***************************************************************************
	Enumerate over all pieces spanning ibMin to ibLim and attempt to merge
	any adjacent ones.
***************************************************************************/
void BSF::_AttemptMerge(long ibMin, long ibLim)
{
	AssertBaseThis(0);
	AssertIn(ibMin, 0, _ibMac + 1);
	AssertIn(ibLim, ibMin, _ibMac + 1);
	long iflo, ib;
	FLO flo, floT;

	if (pvNil == _pggflo)
		return;

	iflo = _IfloFind(LwMax(0, ibMin - 1), &ib);
	while (ib < ibLim && iflo < _pggflo->IvMac() - 1)
		{
		_pggflo->GetFixed(iflo, &flo);
		_pggflo->GetFixed(iflo + 1, &floT);
		if (flo.pfil != floT.pfil ||
				pvNil != flo.pfil && flo.fp + flo.cb != floT.fp)
			{
			//cant merge them, try the next
			iflo++;
			ib += flo.cb;
			continue;
			}

		if (pvNil == flo.pfil)
			_pggflo->Merge(iflo + 1, iflo);
		else
			{
			//merge the two file based flo's
			_pggflo->Delete(iflo + 1);
			ReleasePpo(&floT.pfil);
			AssertPo(flo.pfil, 0);
			}
		flo.cb += floT.cb;
		_pggflo->PutFixed(iflo, &flo);
		}
}


/***************************************************************************
	Replace a range in this bsf with the range in the given bsf.  This
	does the complete insertion, then the deletion.
***************************************************************************/
bool BSF::FReplaceBsf(PBSF pbsfSrc, long ibSrc, long cbSrc, long ibDst, long cbDel)
{
	AssertThis(fobjAssertFull);
	AssertPo(pbsfSrc, 0);
	AssertIn(ibSrc, 0, pbsfSrc->_ibMac + 1);
	AssertIn(cbSrc, 0, pbsfSrc->_ibMac + 1 - ibSrc);
	AssertIn(ibDst, 0, _ibMac + 1);
	AssertIn(cbDel, 0, _ibMac + 1 - ibDst);

	long ifloMinSrc, ifloLimSrc, ifloMinWhole, ifloDst, iflo;
	long ibMinSrc, ibLimSrc, ibMinWhole, ib;
	long cbMinFlo, cbLimFlo, cbIns, cbT;
	FLO flo;
	byte *pb;
	bool fRet;

	//REVIEW shonk: if we're only inserting a non-file piece of pbsfSrc, should
	//we optimize and redirect this to FReplace?

	if (cbSrc == 0 && cbDel == 0)
		return fTrue;

	//make sure the piece table exists and is split at ibDst
	if (!_FEnsureSplit(ibDst, &ifloDst))
		return fFalse;

	//cbIns is the number of bytes already inserted (for cleanup)
	cbIns = 0;

	//get the flo's to insert
	ifloMinSrc = pbsfSrc->_IfloFind(ibSrc, &ibMinSrc, &cbMinFlo);
	ifloLimSrc = pbsfSrc->_IfloFind(ibSrc + cbSrc, &ibLimSrc, &cbLimFlo);

	if (ifloMinSrc < ifloLimSrc)
		{
		//first insert the whole pieces
		if (ibSrc > ibMinSrc)
			{
			ifloMinWhole = ifloMinSrc + 1;
			ibMinWhole = ibMinSrc + cbMinFlo;
			}
		else
			{
			ifloMinWhole = ifloMinSrc;
			ibMinWhole = ibMinSrc;
			}
		if (ifloMinWhole < ifloLimSrc)
			{
			if (!_pggflo->FCopyEntries(pbsfSrc->_pggflo, ifloMinWhole, ifloDst,
					ifloLimSrc - ifloMinWhole))
				{
				goto LFail;
				}
			cbIns = ibLimSrc - ibMinWhole;
			_ibMac += cbIns;

			//adjust the usage counts on the file pieces
			for (iflo = ifloDst + ifloLimSrc - ifloMinWhole; iflo-- > ifloDst; )
				{
				_pggflo->GetFixed(iflo, &flo);
				if (pvNil != flo.pfil)
					flo.pfil->AddRef();
				}
			}

		//insert the front piece
		if (ifloMinSrc < ifloMinWhole)
			{
			cbT = cbMinFlo - ibSrc + ibMinSrc;
			pbsfSrc->_pggflo->GetFixed(ifloMinSrc, &flo);
			if (pvNil == flo.pfil)
				{
				//a memory piece
				pb = (byte *)pbsfSrc->_pggflo->PvLock(ifloMinSrc);
				fRet = FReplace(pb + ibSrc - ibMinSrc, cbT, ibDst, 0);
				pbsfSrc->_pggflo->Unlock();
				}
			else
				{
				//a file piece
				flo.fp += ibSrc - ibMinSrc;
				flo.cb = cbT;
				fRet = FReplaceFlo(&flo, fFalse, ibDst, 0);
				}
			if (!fRet)
				goto LFail;
			cbIns += cbT;
			}
		else
			_AttemptMerge(ibDst, ibDst);
		}

	//insert the back piece
	Assert(ibLimSrc <= ibSrc + cbSrc, 0);
	if (ibLimSrc < ibSrc + cbSrc || cbDel > 0)
		{
		ib = LwMax(ibSrc, ibLimSrc);
		cbT = ibSrc + cbSrc - ib;
		Assert(cbIns + cbT == cbSrc, "didn't insert the correct amount!");
		if (cbT <= 0)
			fRet = FReplace(pvNil, 0, ibDst + cbIns, cbDel);
		else
			{
			pbsfSrc->_pggflo->GetFixed(ifloLimSrc, &flo);
			if (pvNil == flo.pfil)
				{
				//a memory piece
				pb = (byte *)pbsfSrc->_pggflo->PvLock(ifloLimSrc);
				fRet = FReplace(pb + ib - ibLimSrc, cbT, ibDst + cbIns, cbDel);
				pbsfSrc->_pggflo->Unlock();
				}
			else
				{
				//a file piece
				flo.fp += ib - ibLimSrc;
				flo.cb = cbT;
				fRet = FReplaceFlo(&flo, fFalse, ibDst + cbIns, cbDel);
				}
			}
		if (!fRet)
			{
LFail:
			if (cbIns > 0)
				AssertDo(FReplace(pvNil, 0, ibDst, cbIns), "cleanup failed!");
			AssertThis(fobjAssertFull);
			return fFalse;
			}
		}
	else
		{
		Assert(cbIns == cbSrc, "didn't insert the correct amount!");
		_AttemptMerge(ibDst + cbSrc, ibDst + cbSrc);
		}

	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Replace the range [ib, ib + cbDel) with cbIns bytes from prgb.
***************************************************************************/
bool BSF::FReplace(void *prgb, long cbIns, long ib, long cbDel)
{
	AssertThis(fobjAssertFull);
	AssertIn(ib, 0, _ibMac + 1);
	AssertIn(cbDel, 0, _ibMac - ib + 1);
	AssertIn(cbIns, 0, kcbMax);
	AssertPvCb(prgb, cbIns);
	long ibT;
	long iflo, cbT;
	FLO flo;

	if (cbDel == 0 && cbIns == 0)
		return fTrue;

	//make sure the _pggflo exists
	if (!_FEnsureSplit(0))
		return fFalse;

	//get the flo of interest and the ibMin of the flo
	iflo = _IfloFind(ib, &ibT);

	//from here on, ibT is the offset into the current flo (iflo)
	ibT = ib - ibT;

	//if ib is in a file flo, split it
	if (ibT > 0)
		{
		_pggflo->GetFixed(iflo, &flo);
		if (pvNil != flo.pfil)
			{
			if (!_FEnsureSplit(ib, &iflo))
				goto LFail;
			ibT = 0;
			}
		}
	if (cbIns > 0 && !_pggflo->FEnsureSpace(1, cbIns))
		{
LFail:
		_AttemptMerge(ib, ib);
		AssertThis(fobjAssertFull);
		return fFalse;
		}

	if (ibT > 0 && cbDel > 0)
		{
		//we need to start deleting in the middle of a piece
		_pggflo->GetFixed(iflo, &flo);
		Assert(pvNil == flo.pfil, "why wasn't this flo split?");
		if (ibT + cbDel < flo.cb)
			{
			//deleting just part of this piece and no others
			_pggflo->DeleteRgb(iflo, ibT, cbDel);
			flo.cb -= cbDel;
			_pggflo->PutFixed(iflo, &flo);
			_ibMac -= cbDel;
			cbDel = 0;
			}
		else
			{
			//deleting to the end of this piece
			cbT = flo.cb - ibT;
			_pggflo->DeleteRgb(iflo, ibT, cbT);
			flo.cb = ibT;
			_pggflo->PutFixed(iflo, &flo);
			_ibMac -= cbT;
			cbDel -= cbT;
			iflo++;
			ibT = 0;
			}
		}

	Assert(ibT == 0 || cbDel == 0, "wrong ib");
	while (cbDel > 0 && iflo < _pggflo->IvMac())
		{
		//deleting from the beginning of the flo
		_pggflo->GetFixed(iflo, &flo);
		if (cbDel < flo.cb)
			{
			//just remove the first part
			if (pvNil != flo.pfil)
				flo.fp += cbDel;
			else
				_pggflo->DeleteRgb(iflo, 0, cbDel);
			flo.cb -= cbDel;
			_ibMac -= cbDel;
			cbDel = 0;
			_pggflo->PutFixed(iflo, &flo);
			}
		else
			{
			//delete the whole flo
			ReleasePpo(&flo.pfil);
			_pggflo->Delete(iflo);
			cbDel -= flo.cb;
			_ibMac -= flo.cb;
			}
		}

	//now insert the new stuff
	if (cbIns > 0)
		{
		if (iflo < _pggflo->IvMac())
			{
			_pggflo->GetFixed(iflo, &flo);
			if (pvNil == flo.pfil)
				goto LInsertInMem;
			}
		Assert(ibT == 0, "wrong ib 2");
		if (iflo > 0)
			{
			//see if the previous flow is a memory one
			_pggflo->GetFixed(iflo - 1, &flo);
			if (pvNil == flo.pfil)
				{
				iflo--;
				ibT = flo.cb;
LInsertInMem:
				AssertDo(_pggflo->FInsertRgb(iflo, ibT, cbIns, prgb),
					"this shouldn't fail!");
				flo.cb += cbIns;
				_pggflo->PutFixed(iflo, &flo);
				_ibMac += cbIns;
				goto LTryMerge;
				}
			}
		//create a new memory flo
		flo.pfil = pvNil;
		flo.cb = cbIns;
		flo.fp = 0;
		AssertDo(_pggflo->FInsert(iflo, cbIns, prgb, &flo), "why fail?");
		_ibMac += cbIns;
		}

LTryMerge:
	_AttemptMerge(ib, ib + cbIns);
	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Replace the range [ib, ib + cbDel) with the flo.
***************************************************************************/
bool BSF::FReplaceFlo(PFLO pflo, bool fCopy, long ib, long cbDel)
{
	AssertThis(fobjAssertFull);
	AssertPo(pflo, ffloReadable);
	AssertIn(ib, 0, _ibMac + 1);
	AssertIn(cbDel, 0, _ibMac - ib + 1);
	long iflo;
	FLO flo;
	bool fRet;

	if (pflo->cb <= 0)
		return FReplace(pvNil, 0, ib, cbDel);

	if (pflo->cb < kcbMinFloFile)
		{
		//use a memory piece
		HQ hq;

		if (!pflo->FReadHq(&hq))
			return fFalse;
		fRet = FReplace(PvLockHq(hq), pflo->cb, ib, cbDel);
		UnlockHq(hq);
		FreePhq(&hq);
		return fRet;
		}

	//insert a file flo
	if (fCopy)
		{
		if (pvNil == (flo.pfil = FIL::PfilCreateTemp()))
			return fFalse;
		flo.cb = pflo->cb;
		flo.fp = 0;
		if (!pflo->FCopy(&flo))
			{
			ReleasePpo(&flo.pfil);
			AssertThis(fobjAssertFull);
			return fFalse;
			}
		pflo = &flo;
		}
	else
		pflo->pfil->AddRef();

	if (!_FEnsureSplit(ib, &iflo) || !_pggflo->FInsert(iflo, 0, pvNil, pflo))
		{
		pflo->pfil->Release();
		_AttemptMerge(ib, ib);
		AssertThis(fobjAssertFull);
		return fFalse;
		}

	_ibMac += pflo->cb;
	if (cbDel > 0)
		{
		//this shouldn't fail because we've already ensured a split
		//at ib + pflo->cb
		AssertDo(FReplace(pvNil, 0, ib + pflo->cb, cbDel), 0);
		}
	_AttemptMerge(ib, ib + pflo->cb);
	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Fetch cb bytes from position ib into prgb.
***************************************************************************/
void BSF::FetchRgb(long ib, long cb, void *prgb)
{
	AssertThis(fobjAssertFull);
	AssertIn(ib, 0, _ibMac + 1);
	AssertIn(cb, 0, _ibMac - ib + 1);
	AssertPvCb(prgb, cb);
	long iflo, cbT, ibMin;
	FLO flo;

	iflo = _IfloFind(ib, &ibMin);
	ib -= ibMin;
	for ( ; cb > 0 && iflo < _pggflo->IvMac(); iflo++)
		{
		_pggflo->GetFixed(iflo, &flo);
		Assert(flo.cb > ib, "_IfloFind messed up");

		//get min(cb, flo.cb - ib) bytes from position ib
		//then set ib to 0 and update cb
		cbT = LwMin(cb, flo.cb - ib);
		if (pvNil == flo.pfil)
			{
			//the data is in the GG
			Assert(_pggflo->Cb(iflo) == flo.cb, "group element wrong size");
			_pggflo->GetRgb(iflo, ib, cbT, prgb);
			}
		else
			{
			//the data is on file
			if (!flo.FReadRgb(prgb, cbT, ib))
				{
				Warn("read failed in fetch");
				FillPb(prgb, cbT, kbGarbage);
				}
			}
		prgb = PvAddBv(prgb, cbT);
		ib = 0;
		cb -= cbT;
		}
}


/***************************************************************************
	Write a portion of a BSF to the given flo.
***************************************************************************/
bool BSF::FWriteRgb(PFLO pflo, long ib)
{
	AssertThis(0);
	AssertPo(pflo, 0);
	BLCK blck(pflo);

	return FWriteRgb(&blck, ib);
}


/***************************************************************************
	Write a portion of a BSF to the given block.
***************************************************************************/
bool BSF::FWriteRgb(PBLCK pblck, long ib)
{
	AssertThis(fobjAssertFull);
	AssertPo(pblck, 0);
	AssertIn(ib, 0, _ibMac + 1);
	AssertIn(pblck->Cb(), 0, kcbMax);
	long iflo;
	FLO flo;
	long cb, ibT, ibDst;
	long cbWrite = pblck->Cb();
	bool fRet = fTrue;

	if (ib + cbWrite > _ibMac)
		{
		Bug("blck is too big");
		return fFalse;
		}

	if (cbWrite <= 0)
		return fTrue;

	iflo = _IfloFind(ib, &ibT);
	ib -= ibT;
	_pggflo->Lock();
	AssertDo(pblck->FMoveLim(-cbWrite), 0);
	for (ibDst = 0; fRet && ibDst < cbWrite && iflo < _pggflo->IvMac(); iflo++)
		{
		Assert(pblck->Cb() == 0, 0);
		_pggflo->GetFixed(iflo, &flo);
		Assert(flo.cb > ib, "_IfloFind messed up");

		// write min(cbWrite, flo.cb - ib) bytes from position ib
		// then set ib to 0 and update cbWrite
		cb = LwMin(cbWrite, flo.cb - ib);
		AssertDo(pblck->FMoveLim(cb), 0);
		Assert(pblck->Cb() == cb, 0);
		if (pvNil == flo.pfil)
			{
			//the data is in the GG
			Assert(_pggflo->Cb(iflo) == flo.cb, "group element wrong size");
			fRet = pblck->FWrite(PvAddBv(_pggflo->QvGet(iflo), ib));
			}
		else
			{
			//the data is on file
			BLCK blck(flo.pfil, flo.fp + ib, cb);
			fRet = blck.FWriteToBlck(pblck);
			}
		ib = 0;
		ibDst += cb;
		AssertDo(pblck->FMoveMin(cb), 0);
		}
	Assert(!fRet || ibDst == cbWrite, "wrong number of bytes written!");
	Assert(pblck->Cb() == 0, 0);
	AssertDo(pblck->FMoveMin(-ibDst), 0);
	AssertDo(pblck->FMoveLim(cbWrite - ibDst), 0);
	Assert(pblck->Cb() == cbWrite, 0);
	_pggflo->Unlock();

	return fRet;
}


/***************************************************************************
	Write the stream out to a temp file and redirect the stream to just
	refernce the temp file.  This makes the stream's memory footprint
	minimal.
***************************************************************************/
bool BSF::FCompact(void)
{
	AssertThis(fobjAssertFull);
	FLO flo;
	bool fRet = fFalse;

	if (_ibMac == 0 || _pggflo->IvMac() == 1 && _pggflo->Cb(1) == 0)
		{
		fRet = fTrue;
		goto LShrinkGg;
		}

	if (pvNil == (flo.pfil = FIL::PfilCreateTemp()))
		goto LShrinkGg;

	flo.fp = 0;
	flo.cb = _ibMac;
	fRet = FWriteRgb(&flo) && FReplaceFlo(&flo, fFalse, 0, _ibMac);
	ReleasePpo(&flo.pfil);

LShrinkGg:
	if (pvNil != _pggflo)
		_pggflo->FEnsureSpace(0, fgrpShrink);

	AssertThis(fobjAssertFull);
	return fRet;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a byte stream (BSF).
***************************************************************************/
void BSF::AssertValid(ulong grfobj)
{
	BSF_PAR::AssertValid(grfobj);
	if (pvNil == _pggflo)
		{
		AssertVar(0 == _ibMac, "wrong _ibMac", &_ibMac);
		return;
		}
	AssertPo(_pggflo, 0);
	if (!(grfobj & fobjAssertFull))
		return;

	long cb, cbT, iflo;
	FLO flo;

	for (cb = 0, iflo = _pggflo->IvMac(); iflo-- > 0; )
		{
		_pggflo->GetFixed(iflo, &flo);
		cbT = _pggflo->Cb(iflo);
		if (pvNil == flo.pfil)
			Assert(cbT == flo.cb, "wrong sized entry");
		else
			{
			AssertPo(flo.pfil, 0);
			Assert(cbT == 0, "wrong sized file entry");
			AssertIn(flo.fp, 0, kcbMax);
			AssertIn(flo.cb, 0, kcbMax);
			cbT = flo.cb;
			}
		cb += cbT;
		}
	AssertVar(cb == _ibMac, "bad _ibMac", &_ibMac);
}


/***************************************************************************
	Mark memory for the BSF.
***************************************************************************/
void BSF::MarkMem(void)
{
	AssertThis(fobjAssertFull);
	BSF_PAR::MarkMem();
	MarkMemObj(_pggflo);
}
#endif //DEBUG


