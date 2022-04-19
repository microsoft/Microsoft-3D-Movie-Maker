/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Macintosh memory management.

***************************************************************************/
#include "util.h"
ASSERTNAME


// REVIEW shonk: Mac: implement HQ statistics

// HQ header - actually goes at the end of the hq.
struct HQH
	{
#ifdef DEBUG
	short swMagic;		// to detect memory trashing
	short cactRef;		// for marking memory
	schar *pszsFile;	// source file that allocation request is coming from
	long lwLine;		// line in file that allocation request is coming from
	HQ hqPrev;			// previous hq in doubly linked list
	HQ hqNext;			// next hq in doubly linked list
#endif //DEBUG
	byte cactLock;		// lock count
	byte cbExtra;		// count of extra bytes
	};


#ifdef DEBUG
HQ _hqFirst;		// head of the doubly linked list
long vcactSuspendCheckPointers = 0;
#endif //DEBUG

inline void *_QvFromHq(HQ hq)
	{ return vadst.PvStrip(*(void **)hq); }
inline HQH *_QhqhFromHq(HQ hq)
	{
	return (HQH *)PvAddBv(vadst.PvStrip(*(void **)hq),
		GetHandleSize((HN)hq) - size(HQH));
	}
inline HQH *_QhqhFromHqBv(HQ hq, long bv)
	{ return (HQH *)PvAddBv(vadst.PvStrip(*(void **)hq), bv); }

long __pascal _CbFreeStuff(long cb);
ADST vadst;


/***************************************************************************
	This is the GrowZone proc (a call back from the Mac OS memory manager).
	If _fInAlloc is true, we don't do anything, cuz the code that's doing
	the allocation will free stuff and try again.

	If _fInAlloc is false, it probably means that the Toolbox is allocating
	something, in which case, the priority is critical.

	REVIEW shonk: should we just free an emergency buffer and return?
***************************************************************************/
long __pascal _CbFreeStuff(long cb)
{
	if (_fInAlloc || pvNil == vpfnlib)
		return 0;
	_fInAlloc = fTrue;
	cb = (*vpfnlib)(cb, mprCritical);
	_fInAlloc = fFalse;
	return cb;
}


/***************************************************************************
	Constructor for the address stripper - overloaded to also set the
	grow-zone proc.
***************************************************************************/
ADST::ADST(void)
{
	_lwMaskAddress = (long)StripAddress((void *)0xFFFFFFFFL);
	SetGrowZone(&_CbFreeStuff);
}


/***************************************************************************
	Allocates a new moveable block.
***************************************************************************/
#ifdef DEBUG
bool FAllocHqDebug(HQ *phq, long cb, ulong grfmem, long mpr,
	schar *pszsFile, long lwLine)
#else //!DEBUG
bool FAllocHq(HQ *phq, long cb, ulong grfmem, long mpr)
#endif //!DEBUG
{
	AssertVarMem(phq);
	AssertIn(cb, 0, kcbMax);
	HQH hqh;

	if (_fInAlloc)
		{
		Bug("recursing in FAllocHq");
		goto LFail;
		}

#ifdef DEBUG
	if (_hqFirst != hqNil)
		AssertHq(_hqFirst);
#endif //DEBUG

	if (cb > kcbMax)
		{
		BugVar("who's allocating a humongous block?", &cb);
		goto LFail;
		}

	hqh.cbExtra = (-cb) & 3;
	Assert((cb + hqh.cbExtra) % 4 == 0, 0);
	// assert we don't overflow (the limit of kcbMax should ensure this)
	Assert(cb + size(HQH) + hqh.cbExtra > cb, 0);

	cb += hqh.cbExtra + size(HQH);
	_fInAlloc = fTrue;
	do
		{
		*phq = (grfmem & fmemClear) ? NewHandleClear(cb) : NewHandle(cb);
		}
	while (hqNil == *phq && vpfnlib != pvNil && (*vpfnlib)(cb, mpr) > 0);
	_fInAlloc = fFalse;

	if (hqNil == *phq)
		{
LFail:
		*phq = hqNil;
		PushErc(ercOomHq);
		return fFalse;
		}

#ifdef DEBUG
	if (!(grfmem & fmemClear))
		FillPb(_QvFromHq(*phq), cb, kbGarbage);
	hqh.swMagic = kswMagicMem;
	hqh.pszsFile = pszsFile;
	hqh.lwLine = lwLine;
	hqh.hqPrev = hqNil;
	hqh.hqNext = _hqFirst;
	if (_hqFirst != hqNil)
		{
		Assert(_QhqhFromHq(_hqFirst)->hqPrev == hqNil,
			"_hqFirst's prev is not nil");
		_QhqhFromHq(_hqFirst)->hqPrev = *phq;
		}
	_hqFirst = *phq;
#endif //DEBUG
	hqh.cactLock = 0;
	*_QhqhFromHqBv(*phq, cb - size(HQH)) = hqh;
	AssertHq(*phq);
	return fTrue;
}


/***************************************************************************
	Resizes the given hq.  *phq may change (on Windows).  If fhqClear,
	clears any newly added space.
***************************************************************************/
bool FResizePhq(HQ *phq, long cb, ulong grfmem, long mpr)
{
	AssertVarMem(phq);
	AssertHq(*phq);
	AssertIn(cb, 0, kcbMax);
	HQH hqh;
	long cbOld;
	bool fInAllocSave;
	short err;

	if (cb > kcbMax)
		{
		BugVar("who's resizing a humongous block?", &cb);
		goto LFail;
		}
	cbOld = GetHandleSize((HN)*phq) - size(HQH);
	if (_fInAlloc && cb > cbOld)
		{
		Bug("recursing in FResizePhq");
		goto LFail;
		}

	hqh = *_QhqhFromHqBv(*phq, cbOld);
	if (hqh.cactLock != 0)
		{
		Bug("Resizing locked HQ");
		goto LFail;
		}

#ifdef DEBUG
	// trash the old stuff
	if (cbOld > cb)
		FillPb(PvAddBv(_QvFromHq(*phq), cb), cbOld + size(HQH) - cb, kbGarbage);
#endif
	cbOld -= hqh.cbExtra;

	hqh.cbExtra = (-cb) & 3;
	Assert((cb + hqh.cbExtra) % 4 == 0, 0);
	// assert we don't overflow (the limit of kcbMax should ensure this)
	Assert(cb + size(HQH) + hqh.cbExtra > cb, 0);

	fInAllocSave = _fInAlloc;
	_fInAlloc = fTrue;
	do
		{
		err = ErrSetHandleSize((HN)*phq, cb + hqh.cbExtra + size(HQH));
		}
	while (err != noErr && cb > cbOld && vpfnlib != pvNil &&
			(*vpfnlib)(cb - cbOld, mpr) > 0);
	_fInAlloc = fInAllocSave;

	if (err != noErr)
		{
LFail:
		AssertHq(*phq);
		PushErc(ercOomHq);
		return fFalse;
		}

	if (cbOld < cb)
		{
		if (grfmem & fmemClear)
			ClearPb(PvAddBv(_QvFromHq(*phq), cbOld), cb - cbOld);
#ifdef DEBUG
		else // trash the new stuff
			FillPb(PvAddBv(_QvFromHq(*phq), cbOld), cb - cbOld, kbGarbage);

		// trash the rest of the block
		FillPb(PvAddBv(_QvFromHq(*phq), cb),
			hqh.cbExtra + size(HQH), kbGarbage);
#endif //DEBUG
		}
	// put the HQH where it belongs
	*_QhqhFromHqBv(*phq, cb + hqh.cbExtra) = hqh;

	AssertHq(*phq);
	return fTrue;
}


/***************************************************************************
	If hq is not nil, frees it.
***************************************************************************/
void FreePhq(HQ *phq)
{
	AssertVarMem(phq);

	if (*phq == hqNil)
		return;

#ifdef DEBUG
	AssertHq(*phq);
	HQH hqh;

	hqh = *_QhqhFromHq(*phq);
	Assert(hqh.cactLock == 0, "Freeing locked HQ");

	// update prev's next pointer
	if (hqh.hqPrev == hqNil)
		{
		Assert(_hqFirst == *phq, "prev is wrongly nil");
		_hqFirst = hqh.hqNext;
		}
	else
		{
		AssertHq(hqh.hqPrev);
		Assert(_hqFirst != *phq, "prev should be nil");
		_QhqhFromHq(hqh.hqPrev)->hqNext = hqh.hqNext;
		}

	// update next's prev pointer
	if (hqh.hqNext != hqNil)
		{
		AssertHq(hqh.hqNext);
		_QhqhFromHq(hqh.hqNext)->hqPrev = hqh.hqPrev;
		}

	// fill the block with garbage
	FillPb(_QvFromHq(*phq), GetHandleSize((HN)*phq), kbGarbage);
#endif //DEBUG

	DisposHandle((HN)*phq);
	*phq = hqNil;
}


/***************************************************************************
	Return the size of the hq (the client area of the block).
***************************************************************************/
long CbOfHq(HQ hq)
{
	AssertHq(hq);
	long cbRaw;

	cbRaw = GetHandleSize((HN)hq) - size(HQH);
	return cbRaw - _QhqhFromHqBv(hq, cbRaw)->cbExtra;
}


/***************************************************************************
	Copy an hq to a new block.
***************************************************************************/
bool FCopyHq(HQ hqSrc, HQ *phqDst, long mpr)
{
	AssertHq(hqSrc);
	AssertVarMem(phqDst);
	HQH *qhqh;
	short err;

	if (_fInAlloc)
		{
		Bug("recursing in FCopyHq");
		goto LFail;
		}

	*phqDst = hqSrc;
	_fInAlloc = fTrue;
	do
		{
		err = HandToHand((HN *)phqDst);
		}
	while (err != noErr && vpfnlib != pvNil && (*vpfnlib)(CbOfHq(hqSrc), mpr) > 0);
	_fInAlloc = fFalse;

	if (err != noErr)
		{
LFail:
		*phqDst = hqNil;
		PushErc(ercOomHq);
		return fFalse;
		}

	qhqh = _QhqhFromHq(*phqDst);
#ifdef DEBUG
	qhqh->swMagic = kswMagicMem;
	qhqh->pszsFile = __szsFile;
	qhqh->lwLine = __LINE__;
	qhqh->hqPrev = hqNil;
	qhqh->hqNext = _hqFirst;
	if (_hqFirst != hqNil)
		{
		Assert(_QhqhFromHq(_hqFirst)->hqPrev == hqNil,
			"_hqFirst's prev is not nil");
		_QhqhFromHq(_hqFirst)->hqPrev = *phqDst;
		}
	_hqFirst = *phqDst;
#endif //DEBUG
	qhqh->cactLock = 0;

	AssertHq(*phqDst);
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Returns a volatile pointer from an hq.
***************************************************************************/
void *QvFromHq(HQ hq)
{
	AssertHq(hq);
	return _QvFromHq(hq);
}
#endif //DEBUG


/***************************************************************************
	Lock the hq and return a pointer to the data.
***************************************************************************/
void *PvLockHq(HQ hq)
{
	AssertHq(hq);
	HQH *qhqh;

	qhqh = _QhqhFromHq(hq);
	if (qhqh->cactLock++ == 0)
		HLock((HN)hq);
	Assert(_QhqhFromHq(hq)->cactLock > 0, "overflow in cactLock");
	return _QvFromHq(hq);
}


/***************************************************************************
	Unlock the hq.  Asserts and does nothing if the lock count is zero.
***************************************************************************/
void UnlockHq(HQ hq)
{
	AssertHq(hq);
	HQH *qhqh;

	qhqh = _QhqhFromHq(hq);
	Assert(qhqh->cactLock > 0, "hq not locked");
	if (qhqh->cactLock > 0)
		{
		if (--qhqh->cactLock == 0)
			HUnlock((HN)hq);
		}
}


#ifdef DEBUG
/***************************************************************************
	Assert that a given hq is valid.
***************************************************************************/
void AssertHq(HQ hq)
{
	static void *_pvMinZone;
	void *pvLimZone;
	void *qv;
	HQH hqh;
	long cb;
	short sw;

	// make sure hq isn't nil or odd
	if (hq == hqNil || (long(hq) & 1) != 0)
		{
		BugVar("hq is nil or odd", &hq);
		return;
		}

	// make sure *hq is not nil or odd
	if ((qv = _QvFromHq(hq)) == pvNil || (long(qv) & 1) != 0)
		{
		BugVar("*hq is nil or odd", &qv);
		return;
		}

	// get the heap limits and make sure the hq is in it
	if (pvNil == _pvMinZone)
		_pvMinZone = ApplicZone();
	pvLimZone = (void *)LMGetHeapEnd();

	if (hq <= _pvMinZone || hq >= pvLimZone)
		{
		BugVar("hq not in application heap", &hq);
		return;
		}

	if ((qv = _QvFromHq(hq)) <= _pvMinZone || qv >= pvLimZone)
		{
		BugVar("*hq not in the appication heap", &qv);
		return;
		}

	cb = GetHandleSize((HN)hq);
	AssertVar(cb >= size(HQH), "hq block is too small", &cb);
	AssertVar(cb <= BvSubPvs(pvLimZone, _QvFromHq(hq)),
		"hq block runs past end of heap", &cb);

	// verify the HQH
	hqh = *_QhqhFromHqBv(hq, cb - size(HQH));
	if (hqh.swMagic != kswMagicMem)
		{
		BugVar("end of hq block is trashed", &hqh);
		return;
		}
	AssertVar(cb >= size(HQH) + hqh.cbExtra,
		"cbExtra or cb is wrong", &cb);
	sw = HGetState((HN)hq);
	Assert((hqh.cactLock == 0) == !(sw & 0x0080), "lock count is wrong");
	Assert((hqh.hqPrev == hqNil) == (hq == _hqFirst), "hqPrev is wrong");

	// verify the links
	if (hqh.hqPrev != hqNil)
		Assert(_QhqhFromHq(hqh.hqPrev)->hqNext == hq, "hqNext in prev is wrong");
	if (hqh.hqNext != hqNil)
		Assert(_QhqhFromHq(hqh.hqNext)->hqPrev == hq, "hqPrev in next is wrong");
}


/***************************************************************************
	Increment the ref count on an hq.
***************************************************************************/
void MarkHq(HQ hq)
{
	if (hq != hqNil)
		{
		AssertHq(hq);
		_QhqhFromHq(hq)->cactRef++;
		}
}


/***************************************************************************
	Asserts on all unmarked HQs.
***************************************************************************/
void _AssertUnmarkedHqs(void)
{
	HQ hq;

	for (hq = _hqFirst; hq != hqNil; hq = _QhqhFromHq(hq)->hqNext)
		{
		AssertHq(hq);
		if (_QhqhFromHq(hq)->cactRef == 0)
			{
			HQH hqh;
			long cb;
			achar stz[kcbMaxStz];

			cb = CbOfHq(hq);
			hqh = *_QhqhFromHq(hq);
			FFormatStzSz(stz,
				"Lost hq of size %d allocated at line %d of file '%z'",
				cb, hqh.lwLine, hqh.pszsFile);
			Bug(PszStz(stz));
			}
		}
}


/***************************************************************************
	Clears all reference counts.
***************************************************************************/
void _UnmarkAllHqs(void)
{
	HQ hq;

	for (hq = _hqFirst; hq != hqNil; hq = _QhqhFromHq(hq)->hqNext)
		{
		AssertHq(hq);
		_QhqhFromHq(hq)->cactRef = 0;
		}
}


/***************************************************************************
	Assert on obviously bogus pointers.  Assert that [pv, pv+cb) resides
	in either the app zone or the system zone.  If cb is zero, pv can
	be anything (including nil).
***************************************************************************/
void AssertPvCb(void *pv, long cb)
{
	static long _lwStrip;
	static void *_pvMinZone, *_pvMinSysZone;
	void *pvLimZone, *pvLimSysZone;
	void *pvClean, *pvLim;

	if (vcactSuspendCheckPointers != 0 || cb == 0)
		return;

	if (_pvMinZone == 0)
		{
		_pvMinZone = ApplicZone();
		_pvMinSysZone = SystemZone();
		_lwStrip = (long)StripAddress((void *)-1L);
		}
	pvLimZone = (void *)LMGetCurrentA5();
	pvLimSysZone = (void *)(*(long *)_pvMinSysZone & _lwStrip);

	pvClean = (void *)((long)pv & _lwStrip);
	pvLim = PvAddBv(pvClean, cb);
	if (pvClean < _pvMinZone || pvLim > pvLimZone)
		{
		AssertVar(pvClean >= _pvMinSysZone && pvLim <= pvLimSysZone,
			"(pv,cb) not in app or sys zone", &pv);
		}
}
#endif //DEBUG

