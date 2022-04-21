/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Shared (platform independent) file APIs.

***************************************************************************/
#include "util.h"
ASSERTNAME


FTG FIL::vftgCreator = '____';
PFIL FIL::_pfilFirst;
MUTX FIL::_mutxList;


RTCLASS(FIL)
RTCLASS(BLCK)
RTCLASS(MSFIL)


/***************************************************************************
	Constructor for a file.
***************************************************************************/
FIL::FIL(FNI *pfni, ulong grffil)
{
	AssertPo(pfni, ffniFile);
	_fni = *pfni;
	_grffil = grffil;

	// add it to the linked list
	_mutxList.Enter();
	_Attach(&_pfilFirst);
	_mutxList.Leave();
}


/***************************************************************************
	Destructor.  This is private.
***************************************************************************/
FIL::~FIL(void)
{
	// make sure the file is closed.
	_Close(fTrue);

	_mutxList.Enter();
	_Attach(pvNil);
	_mutxList.Leave();
}


/***************************************************************************
	Static method to open an existing file.  Increments the open count.
***************************************************************************/
PFIL FIL::PfilOpen(FNI *pfni, ulong grffil)
{
	AssertPo(pfni, ffniFile);
	PFIL pfil;

	Assert(!(grffil & ffilTemp), "can't open a file as temp");
	if (pvNil != (pfil = PfilFromFni(pfni)))
		{
		if (!pfil->FSetGrffil(grffil))
			return pvNil;

		// increment the open count
		pfil->AddRef();
		return pfil;
		}

	if ((pfil = NewObj FIL(pfni, grffil)) == pvNil)
		goto LFail;

	if (!pfil->_FOpen(fFalse, grffil))
		{
		delete pfil;
LFail:
		PushErc(ercFileOpen);
		return pvNil;
		}

	AssertPo(pfil, 0);
	return pfil;
}


/***************************************************************************
	Create a new file.  Increments the open count.
***************************************************************************/
PFIL FIL::PfilCreate(FNI *pfni, ulong grffil)
{
	AssertPo(pfni, ffniFile);
	PFIL pfil;

	if (pvNil != (pfil = FIL::PfilFromFni(pfni)))
		{
		Bug("trying to create an open file");
		return pvNil;
		}

	grffil |= ffilWriteEnable;
	if ((pfil = NewObj FIL(pfni, grffil)) == pvNil)
		goto LFail;

	if (!pfil->_FOpen(fTrue, grffil))
		{
		delete pfil;
LFail:
		PushErc(ercFileCreate);
		return pvNil;
		}

	AssertPo(pfil, 0);
	return pfil;
}


/***************************************************************************
	Static method to create a temp file in the same directory as fni with
	the same ftg, or, if pfni is nil, in the standard place with vftgTemp.
	The file is not marked.
***************************************************************************/
PFIL FIL::PfilCreateTemp(FNI *pfni)
{
	AssertNilOrPo(pfni, ffniFile);
	FNI fni;

	if (pvNil != pfni)
		{
		fni = *pfni;
		if (!fni.FGetUnique(pfni->Ftg()))
			goto LFail;
		}
	else if (!fni.FGetTemp())
		{
LFail:
		PushErc(ercFileCreate);
		return pvNil;
		}

	return PfilCreate(&fni, ffilTemp | ffilWriteEnable | ffilDenyWrite);
}


/***************************************************************************
	If we have the file indicated by fni open, returns the pfil, otherwise
	returns pvNil.  Doesn't affect the open count.
***************************************************************************/
PFIL FIL::PfilFromFni(FNI *pfni)
{
	AssertPo(pfni, ffniFile);
	PFIL pfil;
	bool fRet;

	_mutxList.Enter();
	for (pfil = _pfilFirst; pfil != pvNil; pfil = pfil->PfilNext())
		{
		AssertPo(pfil, 0);
		pfil->_mutx.Enter();
		fRet = pfni->FEqual(&pfil->_fni);
		pfil->_mutx.Leave();
		if (fRet)
			break;
		}
	_mutxList.Leave();

	return pfil;
}


/***************************************************************************
	Set the file flags according to grffil and grffilMask.  Write enabling
	is only set, never cleared.  Same with marking.
***************************************************************************/
bool FIL::FSetGrffil(ulong grffil, ulong grffilMask)
{
	AssertThis(0);
	bool fRet = fFalse;

	grffil &= grffilMask;

	_mutx.Enter();

	// make sure the permissions are high enough
	if ((~_grffil & grffil & kgrffilPerm) && !_FOpen(fFalse, grffil))
		{
		PushErc(ercFilePerm);
		goto LRet;
		}

	// adjust the mark flag
	if (grffil & ffilMark)
		_grffil |= ffilMark;

	// adjust the temp flag
	if (grffilMask & ffilTemp)
		{
		if (grffil & ffilTemp)
			_grffil |= ffilTemp;
		else
			_grffil &= ~ffilTemp;
		}

	fRet = fTrue;

LRet:
	_mutx.Leave();
	return fTrue;
}


/***************************************************************************
	Decrement the open count.  If it is zero and the file isn't marked,
	the file is closed.
***************************************************************************/
void FIL::Release(void)
{
	AssertThis(0);
	if (_cactRef <= 0)
		{
		Bug("calling Release without an AddRef");
		_cactRef = 0;
		return;
		}
	if (--_cactRef == 0 && !(_grffil & ffilMark))
		delete this;
}


/***************************************************************************
	Get a string representing the path of the file.
***************************************************************************/
void FIL::GetStnPath(PSTN pstn)
{
	AssertThis(0);

	_mutx.Enter();
	_fni.GetStnPath(pstn);
	_mutx.Leave();
}


/***************************************************************************
	Set the temporary status of a file.
***************************************************************************/
void FIL::SetTemp(bool fTemp)
{
	AssertThis(0);

	_mutx.Enter();
	if (fTemp)
		_grffil |= ffilTemp;
	else
		_grffil &= ~ffilTemp;
	_mutx.Leave();
}


/***************************************************************************
	Rename the file to the given file name.  If an open file exists with
	the same name, we rename it and swap names with it as a temporary file.
	Otherwise, we delete any existing file with the same name.  The rules
	for *pfni are the same as for FRename.
***************************************************************************/
bool FIL::FSetFni(FNI *pfni)
{
	AssertPo(pfni, ffniFile);
	PFIL pfilOld;

	if (pvNil != (pfilOld = FIL::PfilFromFni(pfni)))
		{
		if (this == pfilOld)
			return fTrue;

		if (!FSwapNames(pfilOld))
			return fFalse;

		pfilOld->SetTemp(fTrue);
		return fTrue;
		}

	// delete any existing file with this name, then rename our
	// file to the given name
	if (pfni->TExists() != tNo)
		pfni->FDelete();
	return FRename(pfni);
}


/***************************************************************************
	Static method to clear the marks for files.
***************************************************************************/
void FIL::ClearMarks(void)
{
	PFIL pfil;

	_mutxList.Enter();
	for (pfil = _pfilFirst; pfil != pvNil; pfil = pfil->PfilNext())
		{
		AssertPo(pfil, 0);
		pfil->FSetGrffil(ffilNil, ffilMark);
		}
	_mutxList.Leave();
}


/***************************************************************************
	Static method to close any files that are unmarked and have 0 open count.
***************************************************************************/
void FIL::CloseUnmarked(void)
{
	PFIL pfil, pfilNext;

	_mutxList.Enter();
	for (pfil = _pfilFirst; pfil != pvNil; pfil = pfilNext)
		{
		AssertPo(pfil, 0);
		pfilNext = pfil->PfilNext();
		if (!(pfil->_grffil & ffilMark) && pfil->_cactRef == 0)
			delete pfil;
		}
	_mutxList.Leave();
}


/***************************************************************************
	Static method to close all files.
***************************************************************************/
void FIL::ShutDown(void)
{
	PFIL pfil;

	_mutxList.Enter();
	for (pfil = _pfilFirst; pfil != pvNil; pfil = pfil->PfilNext())
		{
		AssertPo(pfil, 0);
		pfil->_Close(fTrue);
		}
	_mutxList.Leave();
}


#ifdef DEBUG
/***************************************************************************
	Validate a pfil.
***************************************************************************/
void FIL::AssertValid(ulong grf)
{
	PFIL pfil;

	FIL_PAR::AssertValid(fobjAllocated);

	_mutx.Enter();
	AssertPo(&_fni, ffniFile);
	_mutx.Leave();

	_mutxList.Enter();
	for (pfil = _pfilFirst; pfil != pvNil; pfil = pfil->PfilNext())
		{
		if (pfil == this)
			break;
		}
	_mutxList.Leave();

	Assert(this == pfil, "not in file list");
}
#endif //DEBUG


/***************************************************************************
	Determine if the given range is within cbTot.
***************************************************************************/
priv bool _FRangeIn(long cbTot, long cb, long ib)
{
	return FIn(ib, 0, cbTot + 1) && FIn(cb, 0, cbTot - ib + 1);
}


/***************************************************************************
	Read a piece of a flo into pv.
***************************************************************************/
bool FLO::FReadRgb(void *pv, long cbRead, FP dfp)
{
	AssertThis(ffloReadable);

	if (!_FRangeIn(this->cb, cbRead, dfp))
		{
		Bug("reading outside flo");
		return fFalse;
		}

	if (cbRead == 0)
		return fTrue;
	return this->pfil->FReadRgb(pv, cbRead, this->fp + dfp);
}


/***************************************************************************
	Write a piece of a flo from pv.
***************************************************************************/
bool FLO::FWriteRgb(void *pv, long cbWrite, FP dfp)
{
	AssertThis(0);

	if (!_FRangeIn(this->cb, cbWrite, dfp))
		{
		Bug("writing outside flo");
		return fFalse;
		}

	if (cbWrite == 0)
		return fTrue;
	return this->pfil->FWriteRgb(pv, cbWrite, this->fp + dfp);
}


/***************************************************************************
	Copy data from this flo to another.
***************************************************************************/
bool FLO::FCopy(PFLO pfloDst)
{
	AssertThis(ffloReadable);
	AssertPo(pfloDst, 0);
	byte rgb[1024];
	long cbBlock, cbT;
	void *pv;
	bool fRet = fFalse;

	if (this->cb != pfloDst->cb)
		{
		Bug("different sized FLOs");
		return fFalse;
		}

	if (this->cb <= size(rgb) ||
			!FAllocPv(&pv, cbBlock = this->cb, fmemNil, mprForSpeed))
		{
		pv = (void *)rgb;
		cbBlock = size(rgb);
		}

	for (cbT = 0; cbT < this->cb; cbT += cbBlock)
		{
		if (cbBlock > this->cb - cbT)
			cbBlock = this->cb - cbT;
		// read the source
		if (!this->pfil->FReadRgb(pv, cbBlock, this->fp + cbT))
			goto LFail;
		// write to the dest
		if (!pfloDst->pfil->FWriteRgb(pv, cbBlock, pfloDst->fp + cbT))
			goto LFail;
		}
	fRet = fTrue;

LFail:
	if ((void *)rgb != pv)
		FreePpv(&pv);

	return fRet;
}


/***************************************************************************
	Allocate an hq and read the flo into it.
***************************************************************************/
bool FLO::FReadHq(HQ *phq, long cbRead, FP dfp)
{
	AssertThis(ffloReadable);
	AssertVarMem(phq);
	bool fT;

	if (!_FRangeIn(this->cb, cbRead, dfp))
		{
		Bug("reading outside flo 2");
		return fFalse;
		}

	if (!FAllocHq(phq, cbRead, fmemNil, mprNormal))
		return fFalse;
	fT = FReadRgb(PvLockHq(*phq), cbRead, dfp);
	UnlockHq(*phq);
	if (!fT)
		FreePhq(phq);
	return fT;
}


/***************************************************************************
	Write the contents of an hq to the flo.
***************************************************************************/
bool FLO::FWriteHq(HQ hq, long dfp)
{
	AssertThis(0);
	AssertHq(hq);
	bool fRet;
	long cbWrite = CbOfHq(hq);

	if (!_FRangeIn(this->cb, cbWrite, dfp))
		{
		Bug("writing outside flo 2");
		return fFalse;
		}

	fRet = FWriteRgb(PvLockHq(hq), cbWrite, dfp);
	UnlockHq(hq);
	return fRet;
}


/***************************************************************************
	Translate the text in a flo from the given osk to the current osk.
	If the text changes, creates a temp file and redirects the flo to the
	temp file (and releases a ref count on the pfil).
***************************************************************************/
bool FLO::FTranslate(short osk)
{
	AssertThis(0);
	short oskSig;
	byte rgbSrc[512];
	byte rgbDst[1024];
	void *pvSrc;
	void *pvDst;
	long cchDst, cch;
	long cbBlock, cbT;
	PFIL pfilNew;
	FP fpSrc, fpDst;
	bool fRet = fFalse;

	// look for a unicode byte order signature
	oskSig = MacWin(koskSbMac, koskSbWin);
	if (this->cb >= size(wchar) && this->cb % size(wchar) == 0)
		{
		wchar chw;

		if (!FReadRgb(&chw, size(wchar), 0))
			return fFalse;
		if (chw == kchwUnicode)
			{
			oskSig = MacWin(koskUniMac, koskUniWin);
			this->fp += size(wchar);
			this->cb -= size(wchar);
			}
		else if (chw == kchwUnicodeSwap)
			{
			oskSig = MacWin(koskUniWin, koskUniMac);
			this->fp += size(wchar);
			this->cb -= size(wchar);
			}
		}

	// determine the probable osk
	if (oskSig != osk)
		{
		if (oskNil == osk)
			osk = oskSig;
		else
			{
			long dcb = CbCharOsk(osk) - CbCharOsk(oskSig);

			if (dcb < 0 || dcb == 0 && CbCharOsk(osk) == size(wchar))
				osk = oskSig;
			}
		}

	if (osk == koskCur)
		return fTrue;

	if (pvNil == (pfilNew = FIL::PfilCreateTemp()))
		return fFalse;

	if (this->cb <= size(rgbSrc) ||
			!FAllocPv(&pvSrc, cbBlock = this->cb, fmemNil, mprForSpeed))
		{
		pvSrc = (void *)rgbSrc;
		cbBlock = size(rgbDst);
		}

	pvDst = (void *)rgbDst;
	cchDst = size(rgbDst) / size(achar);
	fpSrc = this->fp;
	fpDst = 0;
	for (cbT = 0; cbT < this->cb; cbT += cbBlock)
		{
		if (cbBlock > this->cb - cbT)
			cbBlock = this->cb - cbT;

		// read the source
		if (!this->pfil->FReadRgbSeq(pvSrc, cbBlock, &fpSrc))
			goto LFail;

		// translate
		cch = CchTranslateRgb(pvSrc, cbBlock, osk, pvNil, 0);
		if (cch <= 0)
			continue;

		if (cch > cchDst)
			{
			if (pvDst != (void *)rgbDst)
				FreePpv(&pvDst);
			cchDst = cch;
			if (!FAllocPv(&pvDst, cchDst * size(achar), fmemNil, mprNormal))
				goto LFail;
			}
		if (cch != CchTranslateRgb(pvSrc, cbBlock, osk, (achar *)pvDst, cch))
			{
			Bug("why did CchTranslateRgb fail?");
			goto LFail;
			}

		// write to the dest
		if (!pfilNew->FWriteRgbSeq(pvDst, cch * size(achar), &fpDst))
			goto LFail;
		}
	fRet = fTrue;
	ReleasePpo(&this->pfil);
	this->pfil = pfilNew;
	this->fp = 0;
	this->cb = this->pfil->FpMac();
	pfilNew = pvNil;

LFail:
	if ((void *)rgbSrc != pvSrc)
		FreePpv(&pvSrc);
	if (pvDst != (void *)rgbDst)
		FreePpv(&pvDst);
	ReleasePpo(&pfilNew);

	return fRet;
}


#ifdef DEBUG
/***************************************************************************
	Assert this is a valif FLO.
***************************************************************************/
void FLO::AssertValid(ulong grfflo)
{
	AssertPo(pfil, 0);
	AssertIn(fp, 0, kcbMax);
	AssertIn(cb, 0, kcbMax);
	FP fpMac = pfil->FpMac();

	if (pfil->ElError() < kelSeek)
		{
		AssertIn(fp, 0, fpMac + 1);
		if (grfflo & ffloReadable)
			AssertIn(fp + cb, cb, fpMac + 1);
		}
}
#endif //DEBUG


/***************************************************************************
	Constructor for a data block.
***************************************************************************/
BLCK::BLCK(PFLO pflo, bool fPacked)
{
	AssertBaseThis(0);
	AssertPo(pflo, 0);

	_flo = *pflo;
	_flo.pfil->AddRef();
	_hq = hqNil;
	_fPacked = FPure(fPacked);
	AssertThis(0);
}


/***************************************************************************
	Constructor for a data block.
***************************************************************************/
BLCK::BLCK(PFIL pfil, FP fp, long cb, bool fPacked)
{
	AssertBaseThis(0);
	AssertPo(pfil, 0);

	_flo.pfil = pfil;
	_flo.pfil->AddRef();
	_flo.fp = fp;
	_flo.cb = cb;
	_hq = hqNil;
	_fPacked = FPure(fPacked);
	AssertThis(0);
}


/***************************************************************************
	Another constructor for a data block.  Assumes ownership of the hq
	(and sets *phq to hqNil).
***************************************************************************/
BLCK::BLCK(HQ *phq, bool fPacked)
{
	AssertBaseThis(0);
	AssertVarMem(phq);
	AssertHq(*phq);

	_flo.pfil = pvNil;
	_hq = *phq;
	*phq = hqNil;
	_ibMin = 0;
	_ibLim = CbOfHq(_hq);
	_fPacked = FPure(fPacked);
	AssertThis(0);
}


/***************************************************************************
	Another constructor for a data block.
***************************************************************************/
BLCK::BLCK(void)
{
	AssertBaseThis(0);
	_flo.pfil = pvNil;
	_hq = hqNil;
	_fPacked = fFalse;
	AssertThis(0);
}


/***************************************************************************
	The destructor.
***************************************************************************/
BLCK::~BLCK(void)
{
	AssertThis(0);
	Free();
}


/***************************************************************************
	Set the data block to refer to the given flo.
***************************************************************************/
void BLCK::Set(PFLO pflo, bool fPacked)
{
	AssertThis(0);
	AssertPo(pflo, 0);

	Free();
	_flo = *pflo;
	_flo.pfil->AddRef();
	_fPacked = FPure(fPacked);
	AssertThis(0);
}


/***************************************************************************
	Set the data block to refer to the given range on the file.
***************************************************************************/
void BLCK::Set(PFIL pfil, FP fp, long cb, bool fPacked)
{
	AssertThis(0);
	AssertPo(pfil, 0);

	Free();
	_flo.pfil = pfil;
	_flo.pfil->AddRef();
	_flo.fp = fp;
	_flo.cb = cb;
	_fPacked = FPure(fPacked);
	AssertThis(0);
}


/***************************************************************************
	Set the data block to the given hq.  Assumes ownership of the hq and
	sets *phq to hqNil.
***************************************************************************/
void BLCK::SetHq(HQ *phq, bool fPacked)
{
	AssertThis(0);
	AssertVarMem(phq);
	AssertHq(*phq);

	Free();
	_hq = *phq;
	*phq = hqNil;
	_ibMin = 0;
	_ibLim = CbOfHq(_hq);
	_fPacked = FPure(fPacked);
	AssertThis(0);
}


/***************************************************************************
	Free the block (make it empty).
***************************************************************************/
void BLCK::Free(void)
{
	AssertThis(0);
	ReleasePpo(&_flo.pfil);
	FreePhq(&_hq);
	_fPacked = fFalse;
	AssertThis(0);
}


/***************************************************************************
	Return an hq to the data.  If the blck is a memory based block, the
	block is also "freed".  If the block hasn't been packed or unpacked
	or had its min or lim moved, the hq returned is the one originally
	passed to the constructor or SetHq.
***************************************************************************/
HQ BLCK::HqFree(bool fPackedOk)
{
	AssertThis(0);
	HQ hq;

	if (!fPackedOk && _fPacked)
		{
		Bug("accessing packed data");
		return hqNil;
		}

	if (pvNil != _flo.pfil)
		{
		_flo.FReadHq(&hq);
		return hq;
		}

	if (pvNil != _hq)
		{
		hq = _hq;
		_hq = hqNil;
		_fPacked = fFalse;

		if (_ibMin > 0)
			{
			byte *qrgb = (byte *)QvFromHq(hq);
			BltPb(qrgb + _ibMin, qrgb, _ibLim - _ibMin);
			_ibLim -= _ibMin;
			}
		if (CbOfHq(hq) > _ibLim)
			AssertDo(FResizePhq(&hq, _ibLim, fmemNil, mprNormal), 0);
		AssertThis(0);
		return hq;
		}

	return hqNil;
}


/***************************************************************************
	Return the length of the data block.
***************************************************************************/
long BLCK::Cb(bool fPackedOk)
{
	AssertThis(fPackedOk ? 0 : fblckUnpacked);

	if (pvNil != _flo.pfil)
		return _flo.cb;
	if (hqNil != _hq)
		return _ibLim - _ibMin;
	return 0;
}


/***************************************************************************
	Create a temporary buffer.
***************************************************************************/
bool BLCK::FSetTemp(long cb, bool fForceFile)
{
	AssertThis(0);
	PFIL pfil;

	if (!fForceFile && cb < (1L << 23) /* 8 MB */)
		{
		// try to allocate enough mem
		HQ hq;

		if (FAllocHq(&hq, cb, fmemNil, mprNormal))
			{
			SetHq(&hq, _fPacked);
			return fTrue;
			}
		}

	if (pvNil == (pfil = FIL::PfilCreateTemp()))
		return fFalse;

	Set(pfil, 0, cb, _fPacked);
	ReleasePpo(&pfil);

	return fTrue;
}


/***************************************************************************
	Move the beginning of the block.  Doesn't change the location of the
	end of the block.  Fails if you try to move before the beginning of
	the physical storage or after the lim of the block.
***************************************************************************/
bool BLCK::FMoveMin(long dib)
{
	AssertThis(0);

	if (pvNil != _flo.pfil)
		{
		if (!FIn(dib, -_flo.fp, _flo.cb + 1))
			return fFalse;
		_flo.fp += dib;
		_flo.cb -= dib;
		return fTrue;
		}

	if (hqNil != _hq)
		{
		if (!FIn(dib + _ibMin, 0, _ibLim + 1))
			return fFalse;
		_ibMin += dib;
		return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	Move the end of the block.  Doesn't change the location of the
	beginning of the block.  Fails if you try to move before the min of the
	block or after the end of the physical storage.
***************************************************************************/
bool BLCK::FMoveLim(long dib)
{
	AssertThis(0);

	if (pvNil != _flo.pfil)
		{
		if (!FIn(dib, -_flo.cb, kcbMax - _flo.fp))
			return fFalse;
		_flo.cb += dib;
		return fTrue;
		}

	if (hqNil != _hq)
		{
		if (!FIn(dib + _ibLim, _ibMin, CbOfHq(_hq) + 1))
			return fFalse;
		_ibLim += dib;
		return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	Read a range of bytes from the data block.
***************************************************************************/
bool BLCK::FReadRgb(void *pv, long cb, long ib, bool fPackedOk)
{
	AssertThis(0);
	AssertPvCb(pv, cb);

	if (!fPackedOk && _fPacked)
		{
		Bug("reading packed data");
		return fFalse;
		}

	if (!_FRangeIn(Cb(fTrue), cb, ib))
		{
		Bug("reading outside blck");
		return fFalse;
		}

	if (pvNil != _flo.pfil)
		return _flo.FReadRgb(pv, cb, ib);

	if (hqNil != _hq)
		{
		CopyPb((byte *)QvFromHq(_hq) + ib + _ibMin, pv, cb);
		return fTrue;
		}

	Assert(cb == 0 && ib == 0, 0);
	return fTrue;
}


/***************************************************************************
	Write a range of bytes to the data block.
***************************************************************************/
bool BLCK::FWriteRgb(void *pv, long cb, long ib, bool fPackedOk)
{
	AssertThis(0);
	AssertPvCb(pv, cb);

	if (!fPackedOk && _fPacked)
		{
		Bug("writing packed data");
		return fFalse;
		}

	if (!_FRangeIn(Cb(fTrue), cb, ib))
		{
		Bug("writing outside blck");
		return fFalse;
		}

	if (pvNil != _flo.pfil)
		return _flo.FWriteRgb(pv, cb, ib);

	if (hqNil != _hq)
		{
		CopyPb(pv, (byte *)QvFromHq(_hq) + ib + _ibMin, cb);
		return fTrue;
		}

	Assert(cb == 0 && ib == 0, 0);
	return fTrue;
}


/***************************************************************************
	Read a range of bytes from the data block and put it in an hq.
***************************************************************************/
bool BLCK::FReadHq(HQ *phq, long cb, long ib, bool fPackedOk)
{
	AssertThis(0);
	AssertVarMem(phq);

	*phq = hqNil;
	if (!fPackedOk && _fPacked)
		{
		Bug("reading packed data");
		return fFalse;
		}

	if (!_FRangeIn(Cb(fTrue), cb, ib))
		{
		Bug("reading outside blck 2");
		return fFalse;
		}

	if (pvNil != _flo.pfil)
		return _flo.FReadHq(phq, cb, ib);

	if (hqNil != _hq)
		{
		if (!FAllocHq(phq, cb, fmemNil, mprNormal))
			return fFalse;
		CopyPb((byte *)QvFromHq(_hq) + ib + _ibMin, QvFromHq(*phq), cb);
		return fTrue;
		}

	Assert(cb == 0 && ib == 0, 0);
	return fTrue;
}


/***************************************************************************
	Write an hq to the data block.
***************************************************************************/
bool BLCK::FWriteHq(HQ hq, long ib, bool fPackedOk)
{
	AssertThis(0);
	AssertHq(hq);
	long cb = CbOfHq(hq);

	if (!fPackedOk && _fPacked)
		{
		Bug("writing packed data");
		return fFalse;
		}

	if (!_FRangeIn(Cb(fTrue), cb, ib))
		{
		Bug("writing outside blck 2");
		return fFalse;
		}

	if (pvNil != _flo.pfil)
		return _flo.FWriteHq(hq, ib);

	if (hqNil != _hq)
		{
		CopyPb(QvFromHq(hq), (byte *)QvFromHq(_hq) + ib + _ibMin, cb);
		return fTrue;
		}

	Assert(cb == 0 && ib == 0, 0);
	return fTrue;
}


/***************************************************************************
	Write the block to a flo.
***************************************************************************/
bool BLCK::FWriteToFlo(PFLO pfloDst, bool fPackedOk)
{
	AssertThis(fblckReadable);
	AssertPo(pfloDst, 0);

	if (!fPackedOk && _fPacked)
		{
		Bug("copying packed data");
		return fFalse;
		}

	if (Cb(fTrue) != pfloDst->cb)
		{
		Bug("flo is wrong size");
		return fFalse;
		}

	if (pvNil != _flo.pfil)
		return _flo.FCopy(pfloDst);

	if (hqNil != _hq)
		{
		bool fRet;
		fRet = pfloDst->FWrite(PvAddBv(PvLockHq(_hq), _ibMin));
		UnlockHq(_hq);
		return fRet;
		}

	return fTrue;
}


/***************************************************************************
	Write this block to another block.
***************************************************************************/
bool BLCK::FWriteToBlck(PBLCK pblckDst, bool fPackedOk)
{
	AssertThis(fblckReadable);
	AssertPo(pblckDst, 0);
	long cb;

	if (!fPackedOk && _fPacked)
		{
		Bug("copying packed data");
		return fFalse;
		}

	if ((cb = Cb(fTrue)) != pblckDst->Cb(fTrue))
		{
		Bug("blck is wrong size");
		return fFalse;
		}

	if (pvNil != pblckDst->_flo.pfil)
		return FWriteToFlo(&pblckDst->_flo, fPackedOk);

	if (hqNil != pblckDst->_hq)
		{
		bool fRet;
		fRet = FReadRgb((byte *)PvLockHq(pblckDst->_hq) + pblckDst->_ibMin,
			cb, 0, fTrue);
		UnlockHq(pblckDst->_hq);
		return fRet;
		}

	return fTrue;
}


/***************************************************************************
	Get a flo to the data in the block.
***************************************************************************/
bool BLCK::FGetFlo(PFLO pflo, bool fPackedOk)
{
	AssertThis(0);
	AssertVarMem(pflo);

	if (!fPackedOk && _fPacked)
		{
		Bug("accessing packed data");
		return fFalse;
		}

	if (pvNil != _flo.pfil)
		{
		*pflo = _flo;
		pflo->pfil->AddRef();
		return fTrue;
		}

	if (hqNil != _hq && _ibLim > _ibMin)
		{
		bool fRet;

		if (pvNil == (pflo->pfil = FIL::PfilCreateTemp()))
			goto LFail;
		pflo->fp = 0;
		pflo->cb = _ibLim - _ibMin;
		fRet = pflo->FWrite(PvAddBv(PvLockHq(_hq), _ibMin));
		UnlockHq(_hq);
		if (!fRet)
			{
LFail:
			ReleasePpo(&pflo->pfil);
			TrashVar(pflo);
			return fFalse;
			}
		return fTrue;
		}

	pflo->pfil = pvNil;
	pflo->fp = pflo->cb = 0;
	return fTrue;
}


/***************************************************************************
	Return whether the block is packed. If the block is compressed, but
	determining the compression type failed, *pcfmt is set to cfmtNil and
	true is returned.
***************************************************************************/
bool BLCK::FPacked(long *pcfmt)
{
	AssertThis(0);
	AssertNilOrVarMem(pcfmt);

	if (pvNil != pcfmt &&
			(!_fPacked || !vpcodmUtil->FGetCfmtFromBlck(this, pcfmt)))
		{
		*pcfmt = cfmtNil;
		}

	return _fPacked;
}


/***************************************************************************
	If the block is unpacked, pack it. If cfmt is cfmtNil, use the default
	packing format, otherwise use the one specified. If the block is
	already packed, this doesn't change the packing format.
***************************************************************************/
bool BLCK::FPackData(long cfmt)
{
	AssertThis(0);
	HQ hq;

	if (_fPacked)
		return fTrue;

	if (pvNil != _flo.pfil)
		{
		if (!_flo.FReadHq(&hq))
			return fFalse;
		if (!vpcodmUtil->FCompressPhq(&hq, cfmt))
			{
			FreePhq(&hq);
			AssertThis(fblckUnpacked | fblckFile);
			return fFalse;
			}
		SetHq(&hq, fTrue);
		}
	else if (hqNil == _hq)
		return fFalse;
	else
		{
		AssertHq(_hq);
		if (_ibMin != 0 || _ibLim != CbOfHq(_hq))
			{
			hq = HqFree();
			SetHq(&hq, fFalse);
			}
		Assert(_ibMin == 0 && _ibLim == CbOfHq(_hq), 0);
		if (!vpcodmUtil->FCompressPhq(&_hq, cfmt))
			{
			AssertThis(fblckUnpacked | fblckMemory);
			return fFalse;
			}
		_ibMin = 0;
		_ibLim = CbOfHq(_hq);
		_fPacked = fTrue;
		}

	AssertThis(fblckPacked | fblckMemory);
	return fTrue;
}


/***************************************************************************
	If the block is packed, unpack it.
***************************************************************************/
bool BLCK::FUnpackData(void)
{
	AssertThis(0);
	HQ hq;

	if (!_fPacked)
		return fTrue;

	if (pvNil != _flo.pfil)
		{
		if (!_flo.FReadHq(&hq))
			return fFalse;
		if (!vpcodmUtil->FDecompressPhq(&hq))
			{
			FreePhq(&hq);
			AssertThis(fblckPacked | fblckFile);
			return fFalse;
			}
		SetHq(&hq, fFalse);
		}
	else if (hqNil == _hq)
		return fFalse;
	else
		{
		AssertHq(_hq);
		if (_ibMin != 0 || _ibLim != CbOfHq(_hq))
			{
			hq = HqFree();
			SetHq(&hq, fTrue);
			}
		Assert(_ibMin = 0 && _ibLim == CbOfHq(_hq), 0);
		if (!vpcodmUtil->FDecompressPhq(&_hq))
			{
			AssertThis(fblckPacked | fblckMemory);
			return fFalse;
			}
		_ibMin = 0;
		_ibLim = CbOfHq(_hq);
		_fPacked = fFalse;
		}

	AssertThis(fblckUnpacked | fblckMemory);
	return fTrue;
}


/***************************************************************************
	Return the amount of memory the block is using (roughly).
***************************************************************************/
long BLCK::CbMem(void)
{
	AssertThis(0);

	if (pvNil == _flo.pfil && hqNil != _hq)
		return CbOfHq(_hq);
	return 0;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a BLCK.
***************************************************************************/
void BLCK::AssertValid(ulong grfblck)
{
	BLCK_PAR::AssertValid(0);

	if (pvNil != _flo.pfil)
		{
		AssertPo(&_flo, (grfblck & fblckReadable) ? ffloReadable: 0);
		Assert(hqNil == _hq, "both the _flo and _hq are non-nil");
		Assert(!(grfblck & fblckMemory) || (grfblck & fblckFile),
			"block should be memory based");
		}
	else if (hqNil != _hq)
		{
		Assert(!(grfblck & fblckFile) || (grfblck & fblckMemory),
			"block should be file based");
		AssertHq(_hq);
		long cb = CbOfHq(_hq);

		AssertIn(_ibMin, 0, cb + 1);
		AssertIn(_ibLim, _ibMin, cb + 1);
		}

	Assert(_fPacked || !(grfblck & fblckPacked), "block should be packed");
	Assert(!_fPacked || !(grfblck & fblckUnpacked), "block should be unpacked");
}


/***************************************************************************
	Mark memory for the BLCK.
***************************************************************************/
void BLCK::MarkMem(void)
{
	AssertValid(0);
	BLCK_PAR::MarkMem();
	MarkHq(_hq);
}
#endif //DEBUG


/***************************************************************************
	Constructor for a file based message sink.
***************************************************************************/
MSFIL::MSFIL(PFIL pfil)
{
	AssertNilOrPo(pfil, 0);

	_fError = fFalse;
	_pfil = pvNil;
	_fpCur = 0;
	if (pvNil != pfil)
		SetFile(pfil);
}


/***************************************************************************
	Destructor for a file based message sink.
***************************************************************************/
MSFIL::~MSFIL(void)
{
	AssertThis(0);
	ReleasePpo(&_pfil);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a MSFIL.
***************************************************************************/
void MSFIL::AssertValid(ulong grf)
{
	MSFIL_PAR::AssertValid(0);
	AssertNilOrPo(_pfil, 0);
	Assert(_fError || pvNil == _pfil || _fpCur == _pfil->FpMac() ||
		_pfil->ElError() != elNil, "bad _fpCur");
}
#endif //DEBUG


/***************************************************************************
	Set the current file to use for the MSFIL.
***************************************************************************/
void MSFIL::SetFile(PFIL pfil)
{
	AssertThis(0);
	AssertNilOrPo(pfil, 0);

	_fError = fFalse;
	if (pfil != pvNil)
		pfil->AddRef();
	ReleasePpo(&_pfil);
	_pfil = pfil;
	_fpCur = 0;
	if (pvNil != _pfil)
		{
		_fError |= !_pfil->FSetFpMac(0);
#ifdef UNICODE
		wchar chw = kchwUnicode;
		_fError |= !_pfil->FWriteRgbSeq(&chw, size(wchar), &_fpCur);
#endif //UNICODE
		}
}


/***************************************************************************
	Return the output file and give the caller our reference count on it.
***************************************************************************/
PFIL MSFIL::PfilRelease(void)
{
	AssertThis(0);
	PFIL pfil = _pfil;
	_pfil = pvNil;
	return pfil;
}


/***************************************************************************
	Dump a line to the file.
***************************************************************************/
void MSFIL::ReportLine(PSZ psz)
{
	AssertThis(0);
	AssertNilOrPo(_pfil, 0);
	achar rgch[2] = { kchReturn, kchLineFeed };

	Report(psz);
	if (pvNil != _pfil)
		{
		_fError |= !_pfil->FWriteRgbSeq(rgch,
			MacWin(size(achar), 2 * size(achar)), &_fpCur);
		}
}


/***************************************************************************
	Dump some text to the file.
***************************************************************************/
void MSFIL::Report(PSZ psz)
{
	AssertThis(0);
	AssertNilOrPo(_pfil, 0);

	if (pvNil == _pfil)
		{
		SetFile(FIL::PfilCreateTemp());
		if (pvNil == _pfil)
			{
			_fError = fTrue;
			return;
			}
		}

	_fError |= !_pfil->FWriteRgbSeq(psz, CchSz(psz) * size(achar), &_fpCur);
}


/***************************************************************************
	Return whether there has been an error writing to this message sink.
***************************************************************************/
bool MSFIL::FError(void)
{
	AssertThis(0);
	return _fError;
}

