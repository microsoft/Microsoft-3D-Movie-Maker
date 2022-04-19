/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Some "random" stuff

***************************************************************************/
#include "util.h"

ASSERTNAME


RTCLASS(RND)
RTCLASS(SFL)


/***************************************************************************
	Constructs a pseudo-random number generator.  If luSeed is zero,
	generates a seed from the current system time (TsCurrentSystem).
***************************************************************************/
RND::RND(ulong luSeed)
{
	if (0 == luSeed)
		{
		luSeed = TsCurrentSystem();
		SwapBytesRglw((long *)&luSeed, 1);
		}
	_luSeed = luSeed;
	AssertThis(0);
}


/***************************************************************************
	Return the next pseudo-random number within the range 0 to lwLim - 1,
	inclusive.
***************************************************************************/
long RND::LwNext(long lwLim)
{
	AssertThis(0);
	AssertIn(lwLim, 1, kcbMax);

	// high bits are more random than the low ones
	// See Knuth vol 2, page 102, line 24 of table 1.
	// value of kdluRand doesn't matter much
	const ulong kluRandMul = 1566083941L;
	const kdluRand = 2531011L;
	long lw;

	_luSeed = _luSeed * kluRandMul + kdluRand;

	// multiply lw by lwLim and divide by 2^32
#ifdef IN_80386
	ulong luSeedT = _luSeed;
	__asm
		{
		mov		eax,luSeedT
		mul		lwLim
		mov		lw,edx
		}
#else //!IN_80386
	double dou;

	dou = (double)_luSeed * lwLim / (double)0x40000000 / 4;
	lw = (long)dou;
#endif //!IN_80386

	Assert(lw < lwLim, "random number out of range");
	return lw;
}


/***************************************************************************
	Constructs a shuffled array.
***************************************************************************/
SFL::SFL(ulong luSeed) : RND(luSeed)
{
	_clw = 0;
	_ilw = 0;
	_fCustom = fFalse;
	_hqrglw = hqNil;
	AssertThis(0);
}


/***************************************************************************
	Destructs a shuffled array.
***************************************************************************/
SFL::~SFL(void)
{
	AssertThis(0);
	FreePhq(&_hqrglw);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a SFL.
***************************************************************************/
void SFL::AssertValid(ulong grf)
{
	SFL_PAR::AssertValid(0);
	if (_hqrglw != hqNil)
		{
		AssertHq(_hqrglw);
		Assert(CbOfHq(_hqrglw) == LwMul(_clw, size(long)), "HQ wrong size");
		}
	else
		Assert(0 == _clw, "_clw wrong");
}


/***************************************************************************
	Mark memory for the SFL.
***************************************************************************/
void SFL::MarkMem(void)
{
	AssertValid(0);
	SFL_PAR::MarkMem();
	MarkHq(_hqrglw);
}
#endif //DEBUG


/***************************************************************************
	Shuffle the numbers [0, lwLim).
***************************************************************************/
void SFL::Shuffle(long lwLim)
{
	AssertThis(0);
	AssertIn(lwLim, 0, kcbMax);
	long ilw;
	long *qrglw;

	if (!_FEnsureHq(lwLim))
		return;
	_ilw = 0;
	Assert(_clw == lwLim, "wrong _clw");

	qrglw = (long *)QvFromHq(_hqrglw);
	// fill the array with [0, _clw)
	for (ilw = 0; ilw < _clw; ilw++)
		qrglw[ilw] = ilw;

	_fCustom = fFalse;
	_ShuffleCore();
}


/***************************************************************************
	Fill the SFL with the values in prglw and shuffle them.
***************************************************************************/
void SFL::ShuffleRglw(long clw, long *prglw)
{
	AssertThis(0);
	AssertIn(clw, 0, kcbMax);
	AssertPvCb(prglw, LwMul(clw, size(long)));

	if (!_FEnsureHq(clw))
		return;
	_ilw = 0;
	Assert(_clw == clw, "wrong _clw");

	// fill the HQ with the stuff in prglw
	CopyPb(prglw, QvFromHq(_hqrglw), LwMul(clw, size(long)));

	_fCustom = fTrue;
	_ShuffleCore();
}


/***************************************************************************
	Shuffle the entries in the HQ.
***************************************************************************/
void SFL::_ShuffleCore(void)
{
	AssertThis(0);
	Assert(_clw > 0, 0);
	long lw;
	long ilw, ilwSwap;
	long *qrglw;

	//swap stuff
	qrglw = (long *)QvFromHq(_hqrglw);
	for (ilw = _clw; --ilw > 0; )
		{
		ilwSwap = RND::LwNext(ilw + 1);
		if (ilwSwap < ilw)
			{
			lw = qrglw[ilw];
			qrglw[ilw] = qrglw[ilwSwap];
			qrglw[ilwSwap] = lw;
			}
		}
}


/***************************************************************************
	Make sure the HQ is the correct size and set clw appropriately.
***************************************************************************/
bool SFL::_FEnsureHq(long clw)
{
	AssertThis(0);
	AssertIn(clw, 0, kcbMax);

	if (clw <= 0)
		goto LFail;

	if (_hqrglw == hqNil && !FAllocHq(&_hqrglw, LwMul(_clw = clw, size(long)),
			fmemNil, mprNormal))
		{
		goto LFail;
		}
	if (clw != _clw && !FResizePhq(&_hqrglw, LwMul(_clw = clw, size(long)),
			fmemNil, mprNormal))
		{
LFail:
		// we are low on memory, so be nice and give some up
		FreePhq(&_hqrglw);
		_clw = 0;
		AssertThis(0);
		return fFalse;;
		}
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Returns the next number in a shuffled array of numbers.  If lwLim is
	zero, uses the numbers already in the SFL.  Otherwise, numbers
	range from 0 to (lwLim - 1), inclusive.
***************************************************************************/
long SFL::LwNext(long lwLim)
{
	AssertThis(0);
	AssertIn(lwLim, 0, kcbMax);

	if (lwLim > 0 && (lwLim != _clw || _fCustom))
		{
		//need to reshuffle with standard values
		Shuffle(lwLim);
		_ilw = 0;
		if (0 == _clw)
			{
			//shuffling failed, just use the regular random number
			return RND::LwNext(lwLim);
			}
		}
	else if (_clw == 0)
		{
		//no values in the HQ, just return 0
		_ilw = 0;
		return 0;
		}
	else if (_ilw >= _clw)
		{
		//need to reshuffle the values already in the HQ
		_ShuffleCore();
		_ilw = 0;
		}

	AssertIn(_ilw, 0, _clw);
	return ((long *)QvFromHq(_hqrglw))[_ilw++];
}


