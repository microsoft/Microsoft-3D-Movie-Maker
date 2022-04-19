/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Error registration and reporting.  Uses a stack of error code.
	Not based on lists (which seem like an obvious choice), since we don't
	want to do any allocation when registering an error.

***************************************************************************/
#include "util.h"
ASSERTNAME

ERS _ers;
ERS *vpers = &_ers;

RTCLASS(ERS)


/***************************************************************************
	Initialize the error code stack.
***************************************************************************/
ERS::ERS(void)
{
	_cerd = 0;
}


/***************************************************************************
	Push an error code onto the stack.  If overflow occurs, blt the top
	kcerdMax - 1 entries down by one (and lose the bottom entry).
***************************************************************************/
#ifdef DEBUG
void ERS::Push(long erc, PSZS pszsFile, long lwLine)
#else //!DEBUG
void ERS::Push(long erc)
#endif //!DEBUG
{
	AssertThis(0);

#ifdef DEBUG
	STN stn;
	SZS szs;

	stn.FFormatSz(PszLit("Error %d"), erc);
	stn.GetSzs(szs);
	WarnProc(pszsFile, lwLine, szs);
#endif //DEBUG

	_mutx.Enter();

	if (_cerd > 0 && erc == _rgerd[_cerd - 1].erc)
		goto LDone;

	if (_cerd == kcerdMax)
		{
		Warn("Warning: error code stack has filled");
		BltPb(_rgerd + 1, _rgerd, LwMul(size(_rgerd[0]), kcerdMax - 1));
		_cerd--;
		}
#ifdef DEBUG
	_rgerd[_cerd].pszsFile = pszsFile;
	_rgerd[_cerd].lwLine = lwLine;
#endif //DEBUG
	_rgerd[_cerd++].erc = erc;

LDone:
	_mutx.Leave();

	AssertThis(0);
}


/***************************************************************************
	Pop the top error from the stack.  Return fFalse if underflow.
***************************************************************************/
bool ERS::FPop(long *perc)
{
	AssertThis(0);
	AssertNilOrVarMem(perc);

	_mutx.Enter();

	if (_cerd == 0)
		{
		TrashVar(perc);
		_mutx.Leave();
		return fFalse;
		}
	--_cerd;
	if (pvNil != perc)
		*perc = _rgerd[_cerd].erc;

	_mutx.Leave();
	AssertThis(0);

	return fTrue;
}


/***************************************************************************
	Clear the error stack.
***************************************************************************/
void ERS::Clear(void)
{
	AssertThis(0);

	_mutx.Enter();
	_cerd = 0;
	_mutx.Leave();
}


/***************************************************************************
	Return the size of the error stack.
***************************************************************************/
long ERS::Cerc(void)
{
	AssertThis(0);
	return _cerd;
}


/***************************************************************************
	See if the given error code is on the stack.
***************************************************************************/
bool ERS::FIn(long erc)
{
	AssertThis(0);
	long ierd;

	_mutx.Enter();
	for (ierd = 0; ierd < _cerd; ierd++)
		{
		if (_rgerd[ierd].erc == erc)
			{
			_mutx.Leave();
			return fTrue;
			}
		}
	_mutx.Leave();
	return fFalse;
}


/***************************************************************************
	Return the i'th entry.
***************************************************************************/
long ERS::ErcGet(long ierc)
{
	AssertThis(0);
	long erc;

	_mutx.Enter();
	if (!::FIn(ierc, 0, _cerd))
		erc = ercNil;
	else
		erc = _rgerd[ierc].erc;
	_mutx.Leave();

	return erc;
}


/***************************************************************************
	Flush all instances of the given error code from the error stack.
***************************************************************************/
void ERS::Flush(long erc)
{
	AssertThis(0);
	long ierdSrc, ierdDst;

	_mutx.Enter();
	for (ierdSrc = ierdDst = 0; ierdSrc < _cerd; ierdSrc++)
		{
		if (_rgerd[ierdSrc].erc != erc)
			{
			if (ierdDst < ierdSrc)
				_rgerd[ierdDst] = _rgerd[ierdSrc];
			ierdDst++;
			}
		}
	_cerd = ierdDst;
	_mutx.Leave();
}


#ifdef DEBUG
/***************************************************************************
	Assert the error stack is valid.
***************************************************************************/
void ERS::AssertValid(ulong grf)
{
	ERS_PAR::AssertValid(0);
	AssertIn(_cerd, 0, kcerdMax + 1);
}
#endif //DEBUG

