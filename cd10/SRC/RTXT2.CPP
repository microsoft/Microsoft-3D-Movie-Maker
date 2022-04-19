/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Rich text document and associated DDG, continued

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(TRUL)

const long kdxpMax = 0x01000000;

/***************************************************************************
	Character run data.
***************************************************************************/
typedef struct CHRD *PCHRD;
struct CHRD
	{
	long cpLim;
	long cpLimDraw;
	long xpLim;
	long xpLimDraw;
	};


/***************************************************************************
	Character run class. This is used to format a line, draw a line,
	map between cp and xp on a line, etc.
***************************************************************************/
const long kcchMaxChr = 128;
class CHR
	{
	ASSERT

private:
	CHP _chp;
	PAP _pap;
	PTXTB _ptxtb;
	PGNV _pgnv;
	bool _fMustAdvance: 1;
	bool _fBreak: 1;
	bool _fObject: 1;

	long _cpMin;
	long _cpLim;
	long _cpLimFetch;
	long _xpMin;
	long _xpBreak;

	CHRD _chrd;
	CHRD _chrdBop;

	long _dypAscent;
	long _dypDescent;

	achar _rgch[kcchMaxChr];

	bool _FFit(void);
	void _SetToBop(void);
	void _SkipIgnores(void);
	void _DoTab(void);

public:
	void Init(CHP *pchp, PAP *ppap, PTXTB ptxtb, PGNV pgnv,
		long cpMin, long cpLim, long xpBase, long xpLimLine, long xpBreak);

	void GetNextRun(bool fMustAdvance = fFalse);
	bool FBreak(void)
		{ return _fBreak; }
	void GetChrd(PCHRD pchrd, PCHRD pchrdBop = pvNil)
		{
		AssertNilOrVarMem(pchrd);
		AssertNilOrVarMem(pchrdBop);

		if (pvNil != pchrd)
			*pchrd = _chrd;
		if (pvNil != pchrdBop)
			*pchrdBop = _chrdBop;
		}
	long DypAscent(void)
		{ return _dypAscent; }
	long DypDescent(void)
		{ return _dypDescent; }
	long XpMin(void)
		{ return _xpMin; }
	long XpBreak(void)
		{ return _xpBreak; }
	achar *Prgch(void)
		{ return _rgch; }
	long CpMin(void)
		{ return _cpMin; }
	bool FObject(void)
		{ return _fObject; }
	};


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a CHR.
***************************************************************************/
void CHR::AssertValid(ulong grf)
{
	AssertThisMem();
	AssertPo(_ptxtb, 0);
	AssertPo(_pgnv, 0);
}
#endif //DEBUG


/***************************************************************************
	Initialize the CHR.
***************************************************************************/
void CHR::Init(CHP *pchp, PAP *ppap, PTXTB ptxtb, PGNV pgnv,
	long cpMin, long cpLim, long xpBase, long xpLimLine, long xpBreak)
{
	AssertVarMem(pchp);
	AssertVarMem(ppap);
	AssertPo(ptxtb, 0);
	AssertPo(pgnv, 0);
	AssertIn(cpMin, 0, ptxtb->CpMac());
	AssertIn(cpLim, cpMin + 1, ptxtb->CpMac() + 1);
	Assert(xpBase <= xpLimLine, "xpBase > xpLimLine");

	RC rc;

	_chp = *pchp;
	_pap = *ppap;
	_ptxtb = ptxtb;
	_pgnv = pgnv;
	_fBreak = fFalse;

	_cpMin = cpMin;
	_cpLim = cpLim;
	_cpLimFetch = cpMin;
	_xpMin = xpBase;
	_xpBreak = LwMin(xpBreak, xpLimLine);

	// apply any indenting
	if (0 == xpBase)
		{
		switch (_pap.nd)
			{
		case ndFirst:
			if (_ptxtb->FMinPara(_cpMin))
				_xpMin += _pap.dxpTab;
			break;
		case ndRest:
			if (!_ptxtb->FMinPara(_cpMin))
				_xpMin += _pap.dxpTab;
			break;
		case ndAll:
			_xpMin += _pap.dxpTab;
			break;
			}
		}
	if (ndAll == _pap.nd)
		_xpBreak = LwMin(_xpBreak, xpLimLine - _pap.dxpTab);

	_chrd.cpLim = _chrd.cpLimDraw = _cpMin;
	_chrd.xpLim = _chrd.xpLimDraw = _xpMin;
	_chrdBop = _chrd;

	// get the vertical dimensions
	_pgnv->SetFont(_chp.onn, _chp.grfont, _chp.dypFont, tahLeft, tavBaseline);

#ifndef SOC_BUG_1500 // REVIEW shonk: Win95 bug workaround
	// If we don't draw to the _pgnv before getting the metrics, the metrics
	// can be different than after we draw!
	achar ch = kchSpace;
	_pgnv->DrawRgch(&ch, 1, 0, 0);
#endif //!REVIEW

	_pgnv->GetRcFromRgch(&rc, pvNil, 0);
	_dypAscent = LwMax(0, -rc.ypTop - _chp.dypOffset);
	_dypDescent = LwMax(0, rc.ypBottom + _chp.dypOffset);

	AssertThis(0);
}


/***************************************************************************
	Get the next run (within the bounds we were inited with).
***************************************************************************/
void CHR::GetNextRun(bool fMustAdvance)
{
	AssertThis(0);
	ulong grfch;
	achar ch;
	RC rc;

	// Start the next run
	if (FIn(_chrd.cpLim, _cpMin + 1, _cpLimFetch))
		BltPb(_rgch + _chrd.cpLim - _cpMin, _rgch, _cpLimFetch - _chrd.cpLim);
	_cpMin = _chrd.cpLimDraw = _chrd.cpLim;
	_xpMin = _chrd.xpLimDraw = _chrd.xpLim;
	_xpBreak = LwMax(_xpBreak, _xpMin);
	_chrdBop = _chrd;

	if (_cpMin >= _cpLim)
		return;

	// Refill the buffer
	if (_cpLimFetch < _cpLim && _cpLimFetch < _cpMin + kcchMaxChr)
		{
		long cpFetch = LwMax(_cpMin, _cpLimFetch);

		_cpLimFetch = LwMin(kcchMaxChr, _cpLim - _cpMin) + _cpMin;
		_ptxtb->FetchRgch(cpFetch, _cpLimFetch - cpFetch,
			_rgch + cpFetch - _cpMin);
		}

	_fMustAdvance = FPure(fMustAdvance);
	_fBreak = fFalse;
	_fObject = fFalse;

	_pgnv->SetFont(_chp.onn, _chp.grfont, _chp.dypFont, tahLeft, tavBaseline);
	for (;;)
		{
		if (_chrd.cpLim >= _cpLimFetch ||
			(fchIgnore & (grfch = GrfchFromCh(
				ch = _rgch[_chrd.cpLim - _cpMin]))))
			{
			// we're out of characters or this is an ignoreable character -
			// return the run
			if (!_FFit())
				_SetToBop();
			else
				_SkipIgnores();
			break;
			}

		if (grfch & fchTab)
			{
			// handle the string of tabs, then return the run
			_DoTab();
			break;
			}

		if (grfch & fchBreak)
			{
			// This line must break after this character - return the run.
			if (!_FFit())
				{
				_SetToBop();
				break;
				}

			_chrd.cpLim++;
			_SkipIgnores();

			// this is a BOP
			_chrdBop = _chrd;
			_fBreak = fTrue;
			break;
			}

		if (grfch & fchMayBreak)
			{
			_chrd.cpLimDraw = ++_chrd.cpLim;
			if (_FFit())
				{
				// this is a BOP
				_chrdBop = _chrd;
				continue;
				}

			_fBreak = fTrue;
			if (grfch & fchWhiteOverhang)
				{
				// see if everything but this character fits
				long xp = _chrd.xpLim;

				_chrd.cpLimDraw--;
				if (_FFit())
					{
					// fits with the overhang
					_chrd.xpLim = xp;
					_chrdBop = _chrd;
					_SkipIgnores();
					break;
					}
				}

			_SetToBop();
			break;
			}

		if (kchObject == ch)
			{
			// this is an object character
			if (_chrd.cpLim > _cpMin)
				{
				// return the run before processing the object - objects
				// go in their own run.
				if (!_FFit())
					_SetToBop();
				else
					_chrdBop = _chrd;
				break;
				}

			// return just the object (if it really is an object)
			if (!_ptxtb->FGetObjectRc(_chrd.cpLim, _pgnv, &_chp, &rc))
				{
				// treat as a normal character
				_chrd.cpLimDraw = ++_chrd.cpLim;
				continue;
				}

			rc.Offset(0, _chp.dypOffset);
			Assert(!rc.FEmpty() && rc.xpRight >= 0,
				"bad rectangle for the object");

			if (fMustAdvance || _xpMin + rc.xpRight <= _xpBreak)
				{
				_fObject = fTrue;
				_chrd.cpLimDraw = ++_chrd.cpLim;
				_chrd.xpLim = _chrd.xpLimDraw = _xpMin + rc.xpRight;
				_chrdBop = _chrd;

				_dypAscent = LwMax(_dypAscent, -rc.ypTop);
				_dypDescent = LwMax(_dypDescent, rc.ypBottom);

				if (_xpMin + rc.xpRight >= _xpBreak)
					_fBreak = fTrue;
				_SkipIgnores();
				}
			break;
			}

		// normal character
		_chrd.cpLimDraw = ++_chrd.cpLim;
		}

	AssertThis(0);
}


/***************************************************************************
	Test whether everything from _cpMin to _chrd.cpLimDraw fits. Assumes the
	font is set in the _pgnv.
***************************************************************************/
bool CHR::_FFit(void)
{
	AssertThis(0);
	RC rc;

	if (_chrd.cpLimDraw == _cpMin)
		_chrd.xpLim = _chrd.xpLimDraw = _xpMin;
	else
		{
		_pgnv->GetRcFromRgch(&rc, _rgch, _chrd.cpLimDraw - _cpMin);
		_chrd.xpLim = _chrd.xpLimDraw = _xpMin + rc.Dxp();
		}

	return _chrd.xpLimDraw <= _xpBreak;
}


/***************************************************************************
	Set the CHR to the last break opportunity. If there wasn't one and
	_fMustAdvance is true, gobble as many characters as we can, but at least
	one.
***************************************************************************/
void CHR::_SetToBop(void)
{
	AssertThis(0);

	_fBreak = fTrue;
	if (_chrdBop.cpLim <= _cpMin && _fMustAdvance)
		{
		// no break opportunity seen - gobble as many characters as we can,
		// but at least one.
		// do a binary search for the character to break at
		Assert(_chrd.cpLimDraw > _cpMin, "why is _chrd.cpLimDraw == _cpMin?");

		RC rc;
		long ivMin, ivLim, iv;
		long dxp = _xpBreak - _xpMin;

		for (ivMin = 0, ivLim = _chrd.cpLimDraw - _cpMin; ivMin < ivLim; )
			{
			iv = (ivMin + ivLim) / 2 + 1;
			AssertIn(iv, ivMin + 1, ivLim + 1);
			_pgnv->GetRcFromRgch(&rc, _rgch, iv);
			if (rc.Dxp() <= dxp)
				ivMin = iv;
			else
				ivLim = iv - 1;
			}
		AssertIn(ivMin, 0, _chrd.cpLimDraw - _cpMin + 1);
		if (ivMin == 0)
			{
			// nothing fits - use one character
			ivMin = 1;
			}

		// set _chrd.cpLim and _chrd.cpLimDraw, then set the _xp values
		_chrd.cpLim = _chrd.cpLimDraw = _cpMin + ivMin;
		_FFit();
		}
	else
		_chrd = _chrdBop;

	_SkipIgnores();
}


/***************************************************************************
	Skip any trailing ignore characters. Just changes _chrd.cpLim.
***************************************************************************/
void CHR::_SkipIgnores(void)
{
	AssertThis(0);

	while (_chrd.cpLim < _cpLimFetch &&
			(fchIgnore & GrfchFromCh(_rgch[_chrd.cpLim - _cpMin])))
		{
		_chrd.cpLim++;
		}

	if (_chrd.cpLim == _cpLimFetch)
		{
		achar ch;
		long cpMac = _ptxtb->CpMac();

		while (_chrd.cpLim < cpMac)
			{
			_ptxtb->FetchRgch(_chrd.cpLim, 1, &ch);
			if (!(fchIgnore & GrfchFromCh(ch)))
				return;
			_chrd.cpLim++;
			}
		}
}


/***************************************************************************
	Swallow as many tabs as possible.
***************************************************************************/
void CHR::_DoTab(void)
{
	AssertThis(0);

	if (!_FFit())
		{
		_SetToBop();
		return;
		}

	while (_chrd.cpLim < _cpLimFetch &&
			(fchTab & GrfchFromCh(_rgch[_chrd.cpLim - _cpMin])))
		{
		_chrd.cpLim++;
		_chrd.xpLim = LwRoundAway(_chrd.xpLim + 1, _pap.dxpTab);
		if (_chrd.xpLim > _xpBreak)
			{
			// this tab would carry us over the edge.
			if (_chrd.cpLim == _cpMin + 1 && _fMustAdvance)
				{
				// the line is empty, so we have to force the tab onto the line
				_SkipIgnores();
				_fBreak = fTrue;
				}
			else
				_SetToBop();
			return;
			}

		// this is a BOP
		_chrd.xpLimDraw = _chrd.xpLim;
		_chrdBop = _chrd;
		}

	_SkipIgnores();
}


/***************************************************************************
	Constructor for the text document display GOB.
***************************************************************************/
TXTG::TXTG(PTXTB ptxtb, PGCB pgcb) : TXTG_PAR(ptxtb, pgcb)
{
	AssertBaseThis(0);
	_ptxtb = ptxtb;
	_fMark = (kginMark == pgcb->_gin ||
		 kginDefault == pgcb->_gin && kginMark == GOB::GinDefault());
	_pgnv = pvNil;
}


/***************************************************************************
	Destructor for TXTG.
***************************************************************************/
TXTG::~TXTG(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pgllin);
	ReleasePpo(&_pgnv);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a TXTG.
***************************************************************************/
void TXTG::AssertValid(ulong grf)
{
	TXTG_PAR::AssertValid(0);
	AssertPo(_pgllin, 0);
	AssertIn(_ilinInval, 0, _pgllin->IvMac() + 1);
	AssertPo(_pgnv, 0);
	AssertNilOrPo(_ptrul, 0);
	//REVIEW shonk: TXTG::AssertValid: fill out.
}


/***************************************************************************
	Mark memory for the TXTG.
***************************************************************************/
void TXTG::MarkMem(void)
{
	AssertValid(0);
	TXTG_PAR::MarkMem();
	MarkMemObj(_pgllin);
	MarkMemObj(_pgnv);
}
#endif //DEBUG


/***************************************************************************
	Initialize the text document display gob.
***************************************************************************/
bool TXTG::_FInit(void)
{
	AssertBaseThis(0);
	PGPT pgpt;

	if (!TXTG_PAR::_FInit())
		return fFalse;
	if (pvNil == (_pgllin = GL::PglNew(size(LIN))))
		return fFalse;

	// Allocate the GNV for formatting. Use an offscreen one iff _fMark
	// is set.
	if (_fMark)
		{
		RC rc(0, 0, 1, 1);

		if (pvNil == (pgpt = GPT::PgptNewOffscreen(&rc, 8)))
			return fFalse;
		}
	else
		{
		pgpt = Pgpt();
		pgpt->AddRef();
		}

	_pgnv = NewObj GNV(this, pgpt);
	ReleasePpo(&pgpt);

	if (pvNil == _pgnv)
		return fFalse;

	_pgllin->SetMinGrow(20);
	_ilinDisp = 0;
	_cpDisp = 0;
	_dypDisp = 0;
	_ilinInval = 0;
	if (_DxpDoc() > 0)
		{
		long cpMac = _ptxtb->CpMac();
		_Reformat(0, cpMac, cpMac);
		}

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Deactivate the TXTG - turn off the selection.
***************************************************************************/
void TXTG::_Activate(bool fActive)
{
	AssertThis(0);

	TXTG_PAR::_Activate(fActive);
	if (!fActive)
		_SwitchSel(fFalse, kginSysInval);
}


/***************************************************************************
	Get the LIN for the given ilin.  If ilin is past the end of _pgllin,
	_CalcLine is called repeatedly and new lines are added to _pgllin.
	The actual index of the returned line is put in *pilinActual (if not nil).
***************************************************************************/
void TXTG::_FetchLin(long ilin, LIN *plin, long *pilinActual)
{
	AssertThis(0);
	AssertIn(ilin, 0, kcbMax);
	AssertVarMem(plin);
	AssertNilOrVarMem(pilinActual);

	long cpLim, cpMac;
	long dypTot;
	LIN *qlin;
	bool fAdd;
	long ilinLim = LwMin(_pgllin->IvMac(), ilin + 1);

	if (pvNil != pilinActual)
		*pilinActual = ilin;

	if (_ilinInval < ilinLim)
		{
		qlin = (LIN *)_pgllin->QvGet(_ilinInval);
		if (_ilinInval == 0)
			{
			cpLim = 0;
			dypTot = 0;
			}
		else
			{
			qlin--;
			cpLim = qlin->cpMin + qlin->ccp;
			dypTot = qlin->dypTot + qlin->dyp;
			qlin++;
			}

		// adjust LINs up to ilinLim
		for ( ; _ilinInval < ilinLim; _ilinInval++, qlin++)
			{
			qlin->cpMin = cpLim;
			qlin->dypTot = dypTot;
			cpLim += qlin->ccp;
			dypTot += qlin->dyp;
			}
		}
	Assert(_ilinInval >= ilinLim, 0);

	if (ilin < _ilinInval)
		{
		*plin = *(LIN *)_pgllin->QvGet(ilin);
		return;
		}

	Assert(ilinLim == _pgllin->IvMac(), 0);
	if (ilinLim == 0)
		{
		cpLim = 0;
		dypTot = 0;
		ClearPb(plin, size(LIN));
		}
	else
		{
		// get the LIN in case we don't actuall calc any lines below
		*plin = *(LIN *)_pgllin->QvGet(ilinLim - 1);
		cpLim = plin->cpMin + plin->ccp;
		dypTot = plin->dypTot + plin->dyp;
		}

	cpMac = _ptxtb->CpMac();
	fAdd = fTrue;
	for ( ; ilinLim <= ilin && cpLim < cpMac; ilinLim++)
		{
		_CalcLine(cpLim, dypTot, plin);
		fAdd = fAdd && _pgllin->FAdd(plin);
		cpLim += plin->ccp;
		dypTot += plin->dyp;
		}
	_ilinInval = _pgllin->IvMac();
	if (pvNil != pilinActual)
		*pilinActual = ilinLim - 1;
}


/***************************************************************************
	Find the LIN that contains the given cpFind. pilin and/or plin can be nil.
	If fCalcLines is false, we won't calculate any new lines and the
	returned LIN may be before cpFind.
***************************************************************************/
void TXTG::_FindCp(long cpFind, LIN *plin, long *pilin, bool fCalcLines)
{
	AssertThis(0);
	AssertIn(cpFind, 0, _ptxtb->CpMac());
	AssertNilOrVarMem(pilin);
	AssertNilOrVarMem(plin);

	LIN *qlin;
	LIN lin;
	long dypTot;
	long cpLim;
	long ilinMac;
	bool fAdd;

	// get the starting cp and dypTot values for _ilinInval
	qlin = (LIN *)_pgllin->QvGet(_ilinInval);
	if (_ilinInval == 0)
		{
		cpLim = 0;
		dypTot = 0;
		}
	else
		{
		qlin--;
		cpLim = qlin->cpMin + qlin->ccp;
		dypTot = qlin->dypTot + qlin->dyp;
		qlin++;
		}

	if (cpFind < cpLim)
		{
		// do a binary search to find the LIN containing cpFind
		long ivMin, ivLim, iv;

		for (ivMin = 0, ivLim = _ilinInval; ivMin < ivLim; )
			{
			iv = (ivMin + ivLim) / 2;
			qlin = (LIN *)_pgllin->QvGet(iv);
			if (cpFind < qlin->cpMin)
				ivLim = iv;
			else if (cpFind >= qlin->cpMin + qlin->ccp)
				ivMin = iv + 1;
			else
				{
				if (pvNil != pilin)
					*pilin = iv;
				if (pvNil != plin)
					*plin = *qlin;
				return;
				}
			}

		Bug("Invalid LINs");
		cpLim = 0;
		dypTot = 0;
		_ilinInval = 0;
		}

	Assert(cpFind >= cpLim, "why isn't cpFind >= cpLim?");
	if (_ilinInval < (ilinMac = _pgllin->IvMac()))
		{
		// adjust LINs up to cpFind
		qlin = (LIN *)_pgllin->QvGet(_ilinInval);
		for ( ; _ilinInval < ilinMac && cpFind >= cpLim; _ilinInval++, qlin++)
			{
			qlin->cpMin = cpLim;
			qlin->dypTot = dypTot;
			cpLim += qlin->ccp;
			dypTot += qlin->dyp;
			}

		if (cpFind < cpLim)
			{
			AssertIn(cpFind, qlin[-1].cpMin, cpLim);
			if (pvNil != pilin)
				*pilin = _ilinInval - 1;
			if (pvNil != plin)
				*plin = qlin[-1];
			return;
			}
		}
	Assert(_ilinInval == ilinMac, "why isn't _ilinInval == ilinMac?");
	Assert(cpFind >= cpLim, "why isn't cpFind >= cpLim?");

	if (!fCalcLines)
		{
		if (pvNil != pilin)
			*pilin = ilinMac - (ilinMac > 0);
		if (pvNil != plin)
			{
			if (ilinMac > 0)
				*plin = *(LIN *)_pgllin->QvGet(ilinMac - 1);
			else
				ClearPb(plin, size(LIN));
			}
		return;
		}

	// have to calculate some lines
	for (fAdd = fTrue; cpFind >= cpLim; ilinMac++)
		{
		_CalcLine(cpLim, dypTot, &lin);
		fAdd = fAdd && _pgllin->FAdd(&lin);
		cpLim += lin.ccp;
		dypTot += lin.dyp;
		}
	_ilinInval = _pgllin->IvMac();

	if (pvNil != pilin)
		*pilin = ilinMac - 1;
	if (pvNil != plin)
		*plin = lin;
}


/***************************************************************************
	Find the LIN that contains the given dypFind value (measured from the top
	of the document). pilin and/or plin can be nil.
	If fCalcLines is false, we won't calculate any new lines and the
	returned LIN may be before dypFind.
***************************************************************************/
void TXTG::_FindDyp(long dypFind, LIN *plin, long *pilin, bool fCalcLines)
{
	AssertThis(0);
	AssertIn(dypFind, 0, kcbMax);
	AssertNilOrVarMem(pilin);
	AssertNilOrVarMem(plin);

	LIN *qlin;
	LIN lin;
	long dypTot;
	long cpLim, cpMac;
	long ilinMac;
	bool fAdd;

	// get the starting cp and dypTot values for _ilinInval
	qlin = (LIN *)_pgllin->QvGet(_ilinInval);
	if (_ilinInval == 0)
		{
		cpLim = 0;
		dypTot = 0;
		}
	else
		{
		qlin--;
		cpLim = qlin->cpMin + qlin->ccp;
		dypTot = qlin->dypTot + qlin->dyp;
		qlin++;
		}

	if (dypFind < dypTot)
		{
		// do a binary search to find the LIN containing dypFind
		long ivMin, ivLim, iv;

		for (ivMin = 0, ivLim = _ilinInval; ivMin < ivLim; )
			{
			iv = (ivMin + ivLim) / 2;
			qlin = (LIN *)_pgllin->QvGet(iv);
			if (dypFind < qlin->dypTot)
				ivLim = iv;
			else if (dypFind >= qlin->dypTot + qlin->dyp)
				ivMin = iv + 1;
			else
				{
				if (pvNil != pilin)
					*pilin = iv;
				if (pvNil != plin)
					*plin = *qlin;
				return;
				}
			}

		Bug("Invalid LINs");
		cpLim = 0;
		dypTot = 0;
		_ilinInval = 0;
		}

	Assert(dypFind >= dypTot, "why isn't dypFind >= dypTot?");
	if (_ilinInval < (ilinMac = _pgllin->IvMac()))
		{
		// adjust LINs up to cp
		qlin = (LIN *)_pgllin->QvGet(_ilinInval);
		for ( ; _ilinInval < ilinMac && dypFind >= dypTot; _ilinInval++, qlin++)
			{
			qlin->cpMin = cpLim;
			qlin->dypTot = dypTot;
			cpLim += qlin->ccp;
			dypTot += qlin->dyp;
			}

		if (dypFind < dypTot)
			{
			AssertIn(dypFind, qlin[-1].dypTot, dypTot);
			if (pvNil != pilin)
				*pilin = _ilinInval - 1;
			if (pvNil != plin)
				*plin = qlin[-1];
			return;
			}
		}
	Assert(_ilinInval == ilinMac, "why isn't _ilinInval == ilinMac?");
	Assert(dypFind >= dypTot, "why isn't dypFind >= dypTot?");

	if (!fCalcLines)
		{
		if (pvNil != pilin)
			*pilin = ilinMac - (ilinMac > 0);
		if (pvNil != plin)
			{
			if (ilinMac > 0)
				*plin = *(LIN *)_pgllin->QvGet(ilinMac - 1);
			else
				ClearPb(plin, size(LIN));
			}
		return;
		}

	cpMac = _ptxtb->CpMac();
	if (cpLim >= cpMac)
		{
		if (pvNil != plin)
			{
			// Get a valid lin.
			if (ilinMac > 0)
				_pgllin->Get(ilinMac - 1, &lin);
			else
				ClearPb(&lin, size(LIN));
			}
		_ilinInval = ilinMac;
		}
	else
		{
		Assert(dypFind >= dypTot && cpLim < cpMac, 0);
		for (fAdd = fTrue; dypFind >= dypTot && cpLim < cpMac; ilinMac++)
			{
			_CalcLine(cpLim, dypTot, &lin);
			fAdd = fAdd && _pgllin->FAdd(&lin);
			cpLim += lin.ccp;
			dypTot += lin.dyp;
			}
		_ilinInval = _pgllin->IvMac();
		}

	if (pvNil != pilin)
		*pilin = ilinMac - 1;
	if (pvNil != plin)
		*plin = lin;
}


/***************************************************************************
	Recalculate the _pgllin after an edit.  Sets *pyp, *pdypIns, *pdypDel
	to indicate the vertical display space that was affected.
***************************************************************************/
void TXTG::_Reformat(long cp, long ccpIns, long ccpDel,
	long *pyp, long *pdypIns, long *pdypDel)
{
	AssertThis(0);
	AssertIn(cp, 0, _ptxtb->CpMac());
	AssertIn(ccpIns, 0, _ptxtb->CpMac() - cp + 1);
	AssertIn(ccpDel, 0, kcbMax);
	AssertNilOrVarMem(pyp);
	AssertNilOrVarMem(pdypIns);
	AssertNilOrVarMem(pdypDel);

	long ypDel, dypDel, dypIns, dypCur;
	long ccp, cpCur, cpNext, cpMac;
	long ilin, ilinOld;
	LIN linOld, lin, linT;

	_fClear = _ptxtb->AcrBack() == kacrClear;
	_fXpValid = fFalse;
	cpMac = _ptxtb->CpMac();

	// Find the LIN that contains cp (if there is one) - don't calc any lines
	// to get the LIN.
	_FindCp(cp, &linOld, &ilinOld, fFalse);

	if (cp >= linOld.cpMin + linOld.ccp)
		{
		// the LIN for this cp was not cached - recalc from the beginning of
		// the last lin.
		ccpIns += cp - linOld.cpMin;
		ccpDel += cp - linOld.cpMin;
		cp = linOld.cpMin;
		}
	AssertIn(cp, linOld.cpMin, linOld.cpMin + linOld.ccp + (linOld.ccp == 0));

	// make sure the previous line is formatted correctly
	if (ilinOld > 0 && !_ptxtb->FMinPara(linOld.cpMin))
		{
		_FetchLin(ilinOld - 1, &lin);
		_CalcLine(lin.cpMin, lin.dypTot, &linT);
		if (linT.ccp != lin.ccp)
			{
			//edit affected previous line - so start
			//formatting from there
			ilinOld--;
			linOld = lin;
			}
		}
	AssertIn(cp, linOld.cpMin, cpMac);

	// remove deleted lines
	Assert(ilinOld <= _ilinInval, 0);
	_ilinInval = ilinOld;
	for (ccp = dypDel = 0;
			linOld.cpMin + ccp <= cp + ccpDel && ilinOld < _pgllin->IvMac(); )
		{
		_pgllin->Get(ilinOld, &lin);
		dypDel += lin.dyp;
		ccp += lin.ccp;
		_pgllin->Delete(ilinOld);
		}

	//insert the new lines
	cpCur = linOld.cpMin;
	dypCur = linOld.dypTot;

	// cpNext is the cp of the next (possibly stale) LIN in _pgllin
	if (ilinOld < _pgllin->IvMac())
		{
		cpNext = linOld.cpMin + ccp - ccpDel + ccpIns;
		AssertIn(cpNext, linOld.cpMin, cpMac + 1);
		}
	else
		cpNext = cpMac;

	AssertIn(ilinOld, 0, _pgllin->IvMac() + 1);
	dypIns = 0;
	for (ilin = ilinOld; ; )
		{
		AssertIn(cpCur, linOld.cpMin, cpMac + 1);
		AssertIn(dypCur, linOld.dypTot, kcbMax);
		while (cpNext < cpCur && ilin < _pgllin->IvMac())
			{
			_pgllin->Get(ilin, &lin);
			_pgllin->Delete(ilin);
			cpNext += lin.ccp;
			dypDel += lin.dyp;
			}
		AssertIn(cpNext, cpCur, cpMac + 1);

		if (ilin >= _pgllin->IvMac())
			{
			// no more LINs that might be preservable, so we may as well
			// stop trying.
			ilin = _pgllin->IvMac();
			if (cpNext < cpMac)
				dypDel = kswMax;
			if (cpCur < cpMac)
				dypIns = kswMax;
			break;
			}

		AssertIn(ilin, 0, _pgllin->IvMac());
		if (cpCur == cpNext)
			{
			// everything from here on should be correct - we still need to
			// set _ilinDisp
			break;
			}

		_CalcLine(cpCur, dypCur, &lin);
		if (!_pgllin->FInsert(ilin, &lin))
			{
			AssertIn(ilin, 0, _pgllin->IvMac());
			_pgllin->Get(ilin, &linT);
			cpNext += linT.ccp;
			dypDel += linT.dyp;
			_pgllin->Put(ilin, &lin);
			}

		dypIns += lin.dyp;
		cpCur += lin.ccp;
		dypCur += lin.dyp;
		ilin++;
		}
	_ilinInval = ilin;

	if (_cpDisp <= linOld.cpMin)
		ypDel = linOld.dypTot - _dypDisp;
	else
		{
		if (_cpDisp > cp)
			{
			if (_cpDisp >= cp + ccpDel)
				_cpDisp += ccpIns - ccpDel;
			else if (_cpDisp >= cp + ccpIns)
				_cpDisp = cp + ccpIns;
			}
		ypDel = 0;
		_FindCp(_cpDisp, &lin, &_ilinDisp);
		_cpDisp = lin.cpMin;
		dypDel = LwMax(0, linOld.dypTot + dypDel - _dypDisp);
		_dypDisp = lin.dypTot;
		dypIns = LwMax(0, linOld.dypTot + dypIns - _dypDisp);
		}

	if (pvNil != pyp)
		*pyp = ypDel;
	if (pvNil != pdypIns)
		*pdypIns = dypIns;
	if (pvNil != pdypDel)
		*pdypDel = dypDel;
}


/***************************************************************************
	Calculate the end of the line, the left position of the line, the height
	of the line and the ascent of the line.
***************************************************************************/
void TXTG::_CalcLine(long cpMin, long dypBase, LIN *plin)
{
	AssertThis(0);
	AssertIn(cpMin, 0, _ptxtb->CpMac());
	AssertVarMem(plin);

	struct RUN
		{
		long cpLim;
		long xpLim;
		long dypAscent;
		long dypDescent;
		};

	long dxpDoc;
	PAP pap;
	CHP chp;
	long cpLimPap, cpLimChp;
	CHR chr;
	CHRD chrd, chrdBop;
	RUN run, runSure, runT;

	dxpDoc = _DxpDoc();
	_FetchPap(cpMin, &pap, pvNil, &cpLimPap);

	cpLimChp = cpMin;
	run.cpLim = cpMin;
	run.xpLim = 0;
	run.dypAscent = run.dypDescent = 0;
	runSure = run;

	for (;;)
		{
		// make sure the chp is valid
		if (run.cpLim >= cpLimChp)
			{
			_FetchChp(run.cpLim, &chp, pvNil, &cpLimChp);
			cpLimChp = LwMin(cpLimChp, cpLimPap);
			if (cpLimChp <= run.cpLim)
				{
				Bug("why is run.cpLim >= cpLimChp?");
				break;
				}

			chr.Init(&chp, &pap, _ptxtb, _pgnv, run.cpLim, cpLimChp,
				run.xpLim, dxpDoc, dxpDoc);
			}

		chr.GetNextRun(runSure.cpLim == cpMin);
		chr.GetChrd(&chrd, &chrdBop);

		if (chrd.cpLim == run.cpLim)
			{
			// didn't move forward - use runSure
			Assert(runSure.cpLim > cpMin, "why don't we have a BOP?");
			run = runSure;
			break;
			}

		runT = run;
		run.cpLim = chrd.cpLim;
		run.xpLim = chrd.xpLimDraw;
		run.dypAscent = LwMax(run.dypAscent, chr.DypAscent());
		run.dypDescent = LwMax(run.dypDescent, chr.DypDescent());

		if (chr.FBreak())
			{
			// we know that this is the end of the line - use run or runT
			if (run.xpLim > chr.XpBreak() && runT.cpLim > cpMin)
				run = runT;
			break;
			}

		if (chrdBop.cpLim > runT.cpLim)
			{
			// put chrdBop info into runSure
			runSure.cpLim = chrdBop.cpLim;
			runSure.xpLim = chrdBop.xpLimDraw;
			runSure.dypAscent = LwMax(runSure.dypAscent, chr.DypAscent());
			runSure.dypDescent = LwMax(runSure.dypDescent,
				chr.DypDescent());
			}
		}

	Assert(run.dypAscent > 0 || run.dypDescent > 0, "bad run");
	Assert(run.cpLim > cpMin, "why is cch zero?");

	switch (pap.jc)
		{
	default:
		plin->xpLeft = 0;
		break;
	case jcRight:
		plin->xpLeft = (short)(dxpDoc - run.xpLim);
		break;
	case jcCenter:
		plin->xpLeft = (dxpDoc - run.xpLim) / 2;
		break;
		}

	plin->ccp = (short)(run.cpLim - cpMin);
	plin->dyp = (short)(run.dypAscent + run.dypDescent);
	if (pap.numLine != kdenLine)
		plin->dyp = (short)LwMulDiv(plin->dyp, pap.numLine, kdenLine);
	plin->dyp += pap.dypExtraLine;
	if (_ptxtb->FMinPara(run.cpLim) || run.cpLim == _ptxtb->CpMac())
		{
		if (pap.numAfter != kdenAfter)
			plin->dyp = (short)LwMulDiv(plin->dyp, pap.numAfter, kdenAfter);
		plin->dyp += pap.dypExtraAfter;
		}
	if (plin->dyp <= 0)
		plin->dyp = 1;
	plin->dypAscent = (short)run.dypAscent;
	if (plin->dypAscent <= 0)
		plin->dypAscent = 1;
	plin->cpMin = cpMin;
	plin->dypTot = dypBase;
}


/***************************************************************************
	Get the cp that the point is in.  If fClosest is true, this finds the
	cp boundary that the point is closest to (for traditional selection).
	If fClosest is false, it finds the character that the point is over.
***************************************************************************/
bool TXTG::_FGetCpFromPt(long xp, long yp, long *pcp, bool fClosest)
{
	AssertThis(0);
	AssertVarMem(pcp);

	LIN lin;
	RC rc;

	GetRc(&rc, cooLocal);
	if (!FIn(yp, 0, rc.ypBottom))
		return fFalse;

	_FindDyp(_dypDisp + yp, &lin);
	if (_dypDisp + yp >= lin.dypTot + lin.dyp)
		{
		*pcp = _ptxtb->CpMac() - 1;
		return fTrue;
		}

	//we've found the line, now get the cp on the line
	return _FGetCpFromXp(xp, &lin, pcp, fClosest);
}


/***************************************************************************
	Get the cp on the line given by *plin that the xp is in.  If fClosest
	is true, this finds the cp boundary that the point is closest to (for
	traditional selection).  If fClosest is false, it finds the character
	that the xp is over. This only returns false if fClosest is false and
	the xp is before the beginning of the line.
***************************************************************************/
bool TXTG::_FGetCpFromXp(long xp, LIN *plin, long *pcp, bool fClosest)
{
	AssertThis(0);
	AssertVarMem(plin);
	AssertIn(plin->cpMin, 0, _ptxtb->CpMac());
	AssertVarMem(plin);
	AssertIn(plin->ccp, 1, _ptxtb->CpMac() + 1 - plin->cpMin);
	AssertVarMem(pcp);

	CHP chp;
	PAP pap;
	CHR chr;
	long cpCur, cpLimChp, cpLim;
	long xpCur, dxpDoc;
	CHRD chrd;

	xp -= plin->xpLeft + kdxpIndentTxtg - _scvHorz;
	if (xp <= 0)
		{
		*pcp = plin->cpMin;
		return FPure(fClosest);
		}

	if (xp >= (dxpDoc = _DxpDoc()))
		{
		*pcp = _ptxtb->CpPrev(plin->cpMin + plin->ccp);
		return fTrue;
		}

	cpLimChp = cpCur = plin->cpMin;
	cpLim = cpCur + plin->ccp;
	xpCur = 0;
	_FetchPap(cpCur, &pap);

	for (;;)
		{
		// make sure the chp is valid
		if (cpCur >= cpLimChp)
			{
			if (cpCur >= cpLim)
				{
				// everything fit
				*pcp = LwMax(plin->cpMin, _ptxtb->CpPrev(cpLim));
				return fTrue;
				}

			_FetchChp(cpCur, &chp, pvNil, &cpLimChp);
			cpLimChp = LwMin(cpLimChp, cpLim);
			Assert(cpLimChp > cpCur, "why is cpCur >= cpLimChp?");
			chr.Init(&chp, &pap, _ptxtb, _pgnv, cpCur, cpLimChp, xpCur,
				dxpDoc, xp);
			}

		chr.GetNextRun(fTrue);
		chr.GetChrd(&chrd);

		if (chr.FBreak() &&
				(chrd.xpLim >= chr.XpBreak() || chrd.cpLim >= cpLim))
			{
			long cpPrev = _ptxtb->CpPrev(chrd.cpLim);

			if (!fClosest || chrd.cpLim >= cpLim)
				goto LPrev;

			if (cpPrev > cpCur)
				{
				CHRD chrdT;

				// get the length from cpCur to cpPrev
				chr.Init(&chp, &pap, _ptxtb, _pgnv, cpCur, cpPrev, xpCur,
					dxpDoc, xp);
				chr.GetNextRun(fTrue);
				chr.GetChrd(&chrdT);
				cpCur = chrdT.cpLim;
				xpCur = chrdT.xpLim;
				}

			Assert(xp >= xpCur && chrd.xpLim >= xp, "what?");
			if (xp - xpCur > chrd.xpLim - xp)
				{
				*pcp = chrd.cpLim;
				return fTrue;
				}
LPrev:
			*pcp = LwMax(plin->cpMin, cpPrev);
			return fTrue;
			}
		xpCur = chrd.xpLim;
		cpCur = chrd.cpLim;
		}
}


/***************************************************************************
	Get the vertical bounds of the line containing cp and the horizontal
	position of the cp on the line.  If fView is true, the values are in
	view coordinates. If fView is false, the values are in logical values
	(independent of the current scrolling of the view).
***************************************************************************/
void TXTG::_GetXpYpFromCp(long cp, long *pypMin, long *pypLim, long *pxp,
	long *pypBaseLine, bool fView)
{
	AssertThis(0);
	AssertIn(cp, 0, _ptxtb->CpMac());
	AssertNilOrVarMem(pypMin);
	AssertNilOrVarMem(pypLim);
	AssertNilOrVarMem(pxp);
	AssertNilOrVarMem(pypBaseLine);

	LIN lin;
	long xp;

	_FindCp(cp, &lin);
	xp = lin.xpLeft;
	if (fView)
		{
		lin.dypTot -= _dypDisp;
		xp -= _scvHorz;
		}

	if (pvNil != pypMin)
		*pypMin = lin.dypTot;
	if (pvNil != pypLim)
		*pypLim = lin.dypTot + lin.dyp;
	if (pvNil != pxp)
		*pxp = xp + _DxpFromCp(lin.cpMin, cp);
	if (pvNil != pypBaseLine)
		*pypBaseLine = lin.dypTot + lin.dypAscent;
}


/***************************************************************************
	Find the xp location of the given cp. Assumes that cpLine is the start
	of the line containing cp. This includes a buffer on the left of
	kdxpIndentTxtg, but doesn't include centering or right justification
	correction.
***************************************************************************/
long TXTG::_DxpFromCp(long cpLine, long cp)
{
	AssertThis(0);
	AssertIn(cpLine, 0, _ptxtb->CpMac());
	AssertIn(cp, cpLine, _ptxtb->CpMac());

	CHP chp;
	PAP pap;
	CHR chr;
	long cpCur, cpLimChp, cpLim;
	long xpCur;
	CHRD chrd;

	cpLimChp = cpCur = cpLine;
	cpLim = cp + (cp == cpLine);
	xpCur = 0;
	_FetchPap(cpCur, &pap);

	for (;;)
		{
		// make sure the chp is valid
		if (cpCur >= cpLimChp)
			{
			_FetchChp(cpCur, &chp, pvNil, &cpLimChp);
			cpLimChp = LwMin(cpLimChp, cpLim);
			Assert(cpLimChp > cpCur, "why is cpCur >= cpLimChp?");
			chr.Init(&chp, &pap, _ptxtb, _pgnv, cpCur, cpLimChp,
				xpCur, kdxpMax, kdxpMax);
			}

		chr.GetNextRun();
		chr.GetChrd(&chrd);

		if (chrd.cpLim >= cp || chr.FBreak())
			{
			if (cp == cpLine)
				return chr.XpMin() + kdxpIndentTxtg;
			else
				return chrd.xpLim + kdxpIndentTxtg;
			}
		cpCur = chrd.cpLim;
		xpCur = chrd.xpLim;
		}
}


/***************************************************************************
	Replaces the characters between cp1 and cp2 with the given ones.
***************************************************************************/
bool TXTG::FReplace(achar *prgch, long cch, long cp1, long cp2)
{
	AssertThis(0);
	AssertIn(cch, 0, kcbMax);
	AssertPvCb(prgch, cch);
	AssertIn(cp1, 0, _ptxtb->CpMac());
	AssertIn(cp2, 0, _ptxtb->CpMac());

	HideSel();
	SortLw(&cp1, &cp2);
	if (!_ptxtb->FReplaceRgch(prgch, cch, cp1, cp2 - cp1))
		return fFalse;

	cp1 += cch;
	SetSel(cp1, cp1);
	ShowSel();
	return fTrue;
}


/***************************************************************************
	Invalidate the display from cp.  If we're the active TXTG, also redraw.
***************************************************************************/
void TXTG::InvalCp(long cp, long ccpIns, long ccpDel)
{
	AssertThis(0);
	AssertIn(cp, 0, _ptxtb->CpMac() + 1);
	AssertIn(ccpIns, 0, _ptxtb->CpMac() + 1 - cp);
	AssertIn(ccpDel, 0, kcbMax);
	Assert(!_fSelOn, "selection is on in InvalCp!");
	long cpAnchor, cpOther;

	//adjust the sel
	cpAnchor = _cpAnchor;
	cpOther = _cpOther;
	FAdjustIv(&cpAnchor, cp, ccpIns, ccpDel);
	FAdjustIv(&cpOther, cp, ccpIns, ccpDel);
	if (cpAnchor != _cpAnchor || cpOther != _cpOther)
		SetSel(cpAnchor, cpOther, ginNil);

	_ReformatAndDraw(cp, ccpIns, ccpDel);

	if (pvNil != _ptrul)
		{
		PAP pap;

		_FetchPap(LwMin(_cpAnchor, _cpOther), &pap);
		_ptrul->SetDxpTab(pap.dxpTab);
		_ptrul->SetDxpDoc(_DxpDoc());
		}
}


/***************************************************************************
	Reformat the TXTG and update the display.  If this TXTG is not the
	active one, the display is invalidated instead of updated.
***************************************************************************/
void TXTG::_ReformatAndDraw(long cp, long ccpIns, long ccpDel)
{
	RC rcLoc, rc;
	long yp, dypIns, dypDel;
	RC rcUpdate(0, 0, 0, 0);

	//reformat
	_Reformat(cp, ccpIns, ccpDel, &yp, &dypIns, &dypDel);

	//determine the dirty rectangles and if we're active, update them
	GetRc(&rcLoc, cooLocal);
	if (!_fActive)
		{
		rc = rcLoc;
		rc.ypTop = yp;
		if (dypIns == dypDel)
			rc.ypBottom = yp + dypIns;
		InvalRc(&rc);
		return;
		}

	rc = rcLoc;
	rc.ypTop = yp;
	rc.ypBottom = yp + dypIns;
	if (dypIns != dypDel)
		{
		// Have some bits to blt vertically. If the background isn't clear,
		// but _fMark is set, still do the scroll, since _fMark is intended
		// to avoid flashing (allowing offscreen drawing) and scrolling doesn't
		// flash anyway.
		if (_fClear)
			rc.ypBottom = rcLoc.ypBottom;
		else
			{
			rc = rcLoc;
			rc.ypTop = LwMax(rc.ypTop, yp + LwMin(dypIns, dypDel));
			Scroll(&rc, 0, dypIns - dypDel, _fMark ? kginMark : kginDraw);
			rc.ypBottom = rc.ypTop;
			rc.ypTop = yp;
			}
		}

	if (!rc.FEmpty())
		InvalRc(&rc, _fClear || _fMark ? kginMark : kginDraw);

	_fXpValid = fFalse;
}


/***************************************************************************
	Perform a scroll according to scaHorz and scaVert.
***************************************************************************/
void TXTG::_Scroll(long scaHorz, long scaVert, long scvHorz, long scvVert)
{
	RC rc;
	long dxp, dyp;

	GetRc(&rc, cooLocal);
	dxp = 0;
	switch (scaHorz)
		{
	case scaPageUp:
		dxp = -LwMulDiv(rc.Dxp(), 9, 10);
		goto LHorz;
	case scaPageDown:
		dxp = LwMulDiv(rc.Dxp(), 9, 10);
		goto LHorz;
	case scaLineUp:
		dxp = -rc.Dxp() / 10;
		goto LHorz;
	case scaLineDown:
		dxp = rc.Dxp() / 10;
		goto LHorz;
	case scaToVal:
		dxp = scvHorz - _scvHorz;
LHorz:
		dxp = LwBound(_scvHorz + dxp, 0, _ScvMax(fFalse) + 1) - _scvHorz;
		_scvHorz += dxp;

		if (pvNil != _ptrul)
			_ptrul->SetXpLeft(kdxpIndentTxtg - _scvHorz);
		break;
		}

	dyp = 0;
	if (scaVert != scaNil)
		{
		long cpT;
		LIN lin, linDisp;
		long ilin;
		RC rc;

		switch (scaVert)
			{
		case scaToVal:
			cpT = LwBound(scvVert, 0, _ptxtb->CpMac());
			_FindCp(cpT, &lin, &ilin);
			dyp = lin.dypTot - _dypDisp;
			_ilinDisp = ilin;
			_cpDisp = lin.cpMin;
			_dypDisp = lin.dypTot;
			break;

		case scaPageDown:
			// scroll down a page
			GetRc(&rc, cooLocal);
			_FindDyp(rc.Dyp() + _dypDisp, &lin, &ilin);

			if (lin.cpMin <= _cpDisp)
				{
				// we didn't go anywhere so force going down a line
				_FetchLin(_ilinDisp + 1, &lin, &ilin);
				}
			else if (lin.dypTot + lin.dyp > _dypDisp + rc.Dyp() &&
					ilin > _ilinDisp + 1)
				{
				// the line crosses the bottom of the ddg so back up one
				Assert(ilin > 0, 0);
				_FetchLin(ilin - 1, &lin, &ilin);
				}

			dyp = lin.dypTot - _dypDisp;
			_ilinDisp = ilin;
			_cpDisp = lin.cpMin;
			_dypDisp = lin.dypTot;
			break;

		case scaLineDown:
			// scroll down a line
			_FetchLin(_ilinDisp + 1, &lin, &_ilinDisp);
			dyp = lin.dypTot - _dypDisp;
			_cpDisp = lin.cpMin;
			_dypDisp = lin.dypTot;
			break;

		case scaPageUp:
			// scroll down a page
			if (_ilinDisp <= 0)
				break;
			GetRc(&rc, cooLocal);
			_FetchLin(_ilinDisp, &linDisp);
			Assert(linDisp.dypTot == _dypDisp, 0);

			// determine where to scroll to - try to keep the top line
			// visible, but scroll up at least one line
			dyp = LwMax(0, linDisp.dypTot + linDisp.dyp - rc.Dyp());
			_FindDyp(dyp, &lin, &ilin);
			if (lin.cpMin >= linDisp.cpMin)
				{
				// we didn't go anywhere so force going up a line
				_FetchLin(_ilinDisp - 1, &lin, &ilin);
				}
			else if (linDisp.dypTot + linDisp.dyp > lin.dypTot + rc.Dyp() &&
					ilin < _ilinDisp - 1)
				{
				// the previous disp line crosses the bottom of the ddg, so move
				// down one line
				_FetchLin(ilin + 1, &lin, &ilin);
				}

			dyp = lin.dypTot - _dypDisp;
			_ilinDisp = ilin;
			_cpDisp = lin.cpMin;
			_dypDisp = lin.dypTot;
			break;

		case scaLineUp:
			// scroll up a line
			if (_ilinDisp <= 0)
				break;
			_FetchLin(_ilinDisp - 1, &lin, &_ilinDisp);
			dyp = lin.dypTot - _dypDisp;
			_cpDisp = lin.cpMin;
			_dypDisp = lin.dypTot;
			break;
			}

		AssertIn(_cpDisp, 0, _ptxtb->CpMac());
		_scvVert = _cpDisp;
		}

	_SetScrollValues();
	if (dxp != 0 || dyp != 0)
		_ScrollDxpDyp(dxp, dyp);
}


/***************************************************************************
	Move the bits in the window.
***************************************************************************/
void TXTG::_ScrollDxpDyp(long dxp, long dyp)
{
	AssertThis(0);
	RC rcLoc, rcBad1, rcBad2;

	// determine the dirty rectangles and update them
	GetRc(&rcLoc, cooLocal);
	if (_fClear)
		{
		InvalRc(&rcLoc, kginMark);
		vpappb->UpdateMarked();
		return;
		}

	Scroll(&rcLoc, -dxp, -dyp, _fMark ? kginMark : kginDraw);
	if (_fMark)
		vpappb->UpdateMarked();
}


/***************************************************************************
	Update the display of the document.
***************************************************************************/
void TXTG::Draw(PGNV pgnv, RC *prcClip)
{
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);

	DrawLines(pgnv, prcClip, kdxpIndentTxtg - _scvHorz, 0, _ilinDisp);
	if (_fSelOn)
		_InvertSel(pgnv);
	_SetScrollValues();
}


/***************************************************************************
	Draws some lines of the document.
***************************************************************************/
void TXTG::DrawLines(PGNV pgnv, RC *prcClip, long dxp, long dyp,
	long ilinMin, long ilinLim, ulong grftxtg)
{
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	CHP chp;
	PAP pap;
	CHR chr;
	CHRD chrd;
	RC rc;
	long xpBase, xp, yp, xpChr, dxpDoc;
	long cpLine, cpCur, cpLimChp, cpLim, cpLimLine;
	LIN lin;
	long ilin;

	cpLim = _ptxtb->CpMac();
	dxpDoc = _DxpDoc();
	_FetchLin(ilinMin, &lin);
	cpLine = lin.cpMin;

	pgnv->FillRc(prcClip, _ptxtb->AcrBack());
	yp = dyp;
	for (ilin = ilinMin; ilin < ilinLim; ilin++)
		{
		if (yp >= prcClip->ypBottom || cpLine >= cpLim)
			break;

		_FetchLin(ilin, &lin);
		if (yp + lin.dyp <= prcClip->ypTop)
			{
			yp += lin.dyp;
			cpLine += lin.ccp;
			continue;
			}

		_FetchPap(cpLine, &pap);
		xpBase = lin.xpLeft + dxp;
		cpLimChp = cpCur = cpLine;
		cpLimLine = cpLine + lin.ccp;
		xpChr = 0;

		// draw the line
		for (;;)
			{
			// make sure the chp is valid
			if (cpCur >= cpLimChp)
				{
				_FetchChp(cpCur, &chp, pvNil, &cpLimChp);
				cpLimChp = LwMin(cpLimChp, cpLimLine);
				Assert(cpLimChp > cpCur, "why is cpCur >= cpLimChp?");
				chr.Init(&chp, &pap, _ptxtb, _pgnv, cpCur, cpLimChp,
					xpChr, dxpDoc, dxpDoc);
				}

			chr.GetNextRun(fTrue);
			chr.GetChrd(&chrd);

			if (chrd.cpLimDraw > cpCur)
				{
				// draw some text
				xp = xpBase + chr.XpMin();

				if (chr.FObject())
					{
					// draw the object
					_ptxtb->FDrawObject(chr.CpMin(), pgnv, &xp,
						yp + lin.dypAscent + chp.dypOffset, &chp, prcClip);
					}
				else
					{
					pgnv->SetFont(chp.onn, chp.grfont, chp.dypFont,
						tahLeft, tavBaseline);
	
					pgnv->DrawRgch(chr.Prgch(), chrd.cpLimDraw - chr.CpMin(),
						xp, yp + lin.dypAscent + chp.dypOffset,
						chp.acrFore, chp.acrBack);
					}
				}

			if (chrd.cpLim >= cpLimLine || chr.FBreak())
				break;

			cpCur = chrd.cpLim;
			xpChr = chrd.xpLim;
			}

		_DrawLinExtra(pgnv, prcClip, &lin, dxp, yp, grftxtg);

		yp += lin.dyp;
		cpLine = cpLimLine;
		}
}


/***************************************************************************
	Gives a subclass an opportunity to draw extra stuff associated with
	the line. Default does nothing.
***************************************************************************/
void TXTG::_DrawLinExtra(PGNV pgnv, PRC prcClip, LIN *plin,
	long dxp, long yp, ulong grftxtg)
{
}


/***************************************************************************
	Handle a mousedown in the TXTG.
***************************************************************************/
bool TXTG::FCmdTrackMouse(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	RC rc;
	long cp;
	long scaHorz, scaVert;
	long xp = pcmd->xp;
	long yp = pcmd->yp;

	if (pcmd->cid == cidMouseDown)
		{
		Assert(vpcex->PgobTracking() == pvNil, "mouse already being tracked!");
		_fSelByWord = (pcmd->cact > 1) && !(pcmd->grfcust & fcustShift);
		vpcex->TrackMouse(this);
		}
	else
		{
		Assert(vpcex->PgobTracking() == this, "not tracking mouse!");
		Assert(pcmd->cid == cidTrackMouse, 0);
		}

	//do autoscrolling
	GetRc(&rc, cooLocal);
	if (!FIn(xp, rc.xpLeft, rc.xpRight))
		{
		scaHorz = (xp < rc.xpLeft) ? scaLineUp : scaLineDown;
		xp = LwBound(xp, rc.xpLeft, rc.xpRight);
		}
	else
		scaHorz = scaNil;
	if (!FIn(yp, rc.ypTop, rc.ypBottom))
		{
		scaVert = (yp < rc.ypTop) ? scaLineUp : scaLineDown;
		yp = LwBound(yp, rc.ypTop, rc.ypBottom);
		}
	else
		scaVert = scaNil;
	if (scaHorz != scaNil || scaVert != scaNil)
		_Scroll(scaHorz, scaVert);

	//set the selection
	if (_FGetCpFromPt(xp, yp, &cp, !_fSelByWord))
		{
		if (pcmd->cid != cidMouseDown || (pcmd->grfcust & fcustShift))
			{
			if (_fSelByWord)
				{
				cp = (cp < _cpAnchor) ? _ptxtb->CpPrev(cp + 1, fTrue) :
					_ptxtb->CpNext(cp, fTrue);
				}
			SetSel(_cpAnchor, cp);
			}
		else
			{
			if (_fSelByWord)
				cp = _ptxtb->CpPrev(cp + 1, fTrue);
			SetSel(cp, _fSelByWord ? _ptxtb->CpNext(cp, fTrue) : cp);
			}
		_fXpValid = fFalse;
		}
	_SwitchSel(fTrue);	//make sure the selection is on

	if (!(pcmd->grfcust & fcustMouse))
		vpcex->EndMouseTracking();

	return fTrue;
}


/***************************************************************************
	Do idle processing.  If this handler has the active selection, make sure
	the selection is on or off according to rglw[0] (non-zero means on)
	and set rglw[0] to false.  Always return false.
***************************************************************************/
bool TXTG::FCmdSelIdle(PCMD pcmd)
{
	AssertThis(0);

	// if rglw[1] is this one's hid, don't change the sel state.
	if (pcmd->rglw[1] != Hid())
		{
		if (!pcmd->rglw[0])
			_SwitchSel(fFalse, kginDefault);
		else if (_cpAnchor != _cpOther || _tsSel == 0)
			_SwitchSel(fTrue);
		else if (DtsCaret() < TsCurrent() - _tsSel)
			_SwitchSel(!_fSelOn);
		}
	pcmd->rglw[0] = fFalse;
	return fFalse;
}


/***************************************************************************
	Get the current selection.
***************************************************************************/
void TXTG::GetSel(long *pcpAnchor, long *pcpOther)
{
	AssertThis(0);
	AssertVarMem(pcpAnchor);
	AssertVarMem(pcpOther);

	*pcpAnchor = _cpAnchor;
	*pcpOther = _cpOther;
}


/***************************************************************************
	Set the selection.
***************************************************************************/
void TXTG::SetSel(long cpAnchor, long cpOther, long gin)
{
	AssertThis(0);
	long cpMac = _ptxtb->CpMac();

	cpAnchor = LwBound(cpAnchor, 0, cpMac);
	cpOther = LwBound(cpOther, 0, cpMac);

	if (cpAnchor == _cpAnchor && cpOther == _cpOther)
		return;

	if (_fSelOn)
		{
		if ((_fMark || _fClear) && gin == kginDraw)
			gin = kginMark;
		_pgnv->SetGobRc(this);
		if (_cpAnchor != cpAnchor || _cpAnchor == _cpOther ||
				cpAnchor == cpOther)
			{
			_InvertSel(_pgnv, gin);
			_cpAnchor = cpAnchor;
			_cpOther = cpOther;
			_InvertSel(_pgnv, gin);
			_tsSel = TsCurrent();
			}
		else
			{
			//they have the same anchor and neither is an insertion
			_InvertCpRange(_pgnv, _cpOther, cpOther, gin);
			_cpOther = cpOther;
			}
		}
	else
		{
		_cpAnchor = cpAnchor;
		_cpOther = cpOther;
		_tsSel = 0L;
		}
}


/***************************************************************************
	Make sure the selection is visible (at least the _cpOther end of it).
***************************************************************************/
void TXTG::ShowSel(void)
{
	AssertThis(0);
	long cpScroll;
	long ilinOther, ilinAnchor, ilin;
	long dxpScroll, dypSel;
	long xpMin, xpLim;
	RC rc;
	LIN linOther, linAnchor, lin;
	long cpAnchor = _cpAnchor;

	//find the lines we want to show
	_FindCp(_cpOther, &linOther, &ilinOther);
	_FindCp(_cpAnchor, &linAnchor, &ilinAnchor);
	linOther.dypTot -= _dypDisp;
	linAnchor.dypTot -= _dypDisp;
	GetRc(&rc, cooLocal);

	cpScroll = _cpDisp;
	if (!FIn(linOther.dypTot, 0, rc.Dyp() - linOther.dyp) ||
			!FIn(linAnchor.dypTot, 0, rc.Dyp() - linAnchor.dyp))
		{
		if (_cpOther < cpAnchor)
			dypSel = linAnchor.dypTot - linOther.dypTot + linAnchor.dyp;
		else
			dypSel = linOther.dypTot - linAnchor.dypTot + linOther.dyp;
		if (dypSel > rc.Dyp())
			{
			//just show _cpOther
            cpAnchor = _cpOther;
			ilinAnchor = ilinOther;
			linAnchor = linOther;
			dypSel = linOther.dyp;
			}

		if (linOther.dypTot < 0 || linAnchor.dypTot < 0)
			{
			//scroll up
			cpScroll = LwMin(linOther.cpMin, linAnchor.cpMin);
			}
		else if (linOther.dypTot + linOther.dyp >= rc.Dyp() ||
				linAnchor.dypTot + linAnchor.dyp >= rc.Dyp())
			{
			//scroll down
			ilin = LwMin(ilinOther, ilinAnchor);
			cpScroll = LwMin(linOther.cpMin, linAnchor.cpMin);
			dypSel = rc.Dyp() - dypSel;
			for (;;)
				{
				if (--ilin < 0)
					break;
				_FetchLin(ilin, &lin);
				if (lin.dyp > dypSel)
					break;
				cpScroll -= lin.ccp;
				dypSel -= lin.dyp;
				}
			}
		}

	//now do the horizontal stuff
	xpMin = _DxpFromCp(linOther.cpMin, _cpOther) + linOther.xpLeft - _scvHorz;
	xpLim = _DxpFromCp(linAnchor.cpMin, cpAnchor) + linAnchor.xpLeft - _scvHorz;
	if (LwAbs(xpLim - xpMin) > rc.Dxp())
		{
		//can't show both
		if (xpMin > xpLim)
			{
			xpLim = xpMin;
			xpMin = xpLim - rc.Dxp();
			}
		else
			xpLim = xpMin + rc.Dxp();
		}
	else
		SortLw(&xpMin, &xpLim);
	dxpScroll = LwMax(LwMin(0, rc.xpRight - xpLim), rc.xpLeft - xpMin);

	if (dxpScroll != 0 || cpScroll != _cpDisp)
		_Scroll(scaToVal, scaToVal, _scvHorz - dxpScroll, cpScroll);
}


/***************************************************************************
	Turn the selection off.
***************************************************************************/
void TXTG::HideSel(void)
{
	AssertThis(0);

	if (!_fSelOn)
		return;

	_SwitchSel(fFalse);
	_tsSel = 0;
}


/***************************************************************************
	Turn the sel on or off according to fOn.
***************************************************************************/
void TXTG::_SwitchSel(bool fOn, long gin)
{
	AssertThis(0);

	if (FPure(fOn) != FPure(_fSelOn))
		{
		if ((_fMark || _fClear) && gin == kginDraw)
			gin = kginMark;
		_pgnv->SetGobRc(this);
		_InvertSel(_pgnv, gin);
		_fSelOn = FPure(fOn);
		_tsSel = TsCurrent();
		}
}


/***************************************************************************
	Invert the current selection.
***************************************************************************/
void TXTG::_InvertSel(PGNV pgnv, long gin)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	RC rc, rcT;

	GetRc(&rc, cooLocal);
	if (_cpAnchor == _cpOther)
		{
		//insertion bar
		_GetXpYpFromCp(_cpAnchor, &rcT.ypTop, &rcT.ypBottom, &rcT.xpLeft);
		rcT.xpRight = --rcT.xpLeft + 2;
		if (rcT.FIntersect(&rc))
			{
			if (kginDraw == gin)
				pgnv->FillRc(&rcT, kacrInvert);
			else
				InvalRc(&rcT, gin);
			}
		}
	else
		_InvertCpRange(pgnv, _cpAnchor, _cpOther, gin);
}


/***************************************************************************
	Invert a range.
***************************************************************************/
void TXTG::_InvertCpRange(PGNV pgnv, long cp1, long cp2, long gin)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertIn(cp1, 0, _ptxtb->CpMac());
	AssertIn(cp2, 0, _ptxtb->CpMac());
	RC rc1, rc2, rcClip, rcT, rcDoc;

	if (cp1 == cp2)
		return;
	SortLw(&cp1, &cp2);
	_GetXpYpFromCp(cp1, &rc1.ypTop, &rc1.ypBottom, &rc1.xpLeft);
	_GetXpYpFromCp(cp2, &rc2.ypTop, &rc2.ypBottom, &rc2.xpRight);
	GetRc(&rcClip, cooLocal);
	if (rc1.ypTop >= rcClip.ypBottom || rc2.ypBottom <= rcClip.ypTop)
		return;

	if (rc1.ypTop == rc2.ypTop && rc1.ypBottom == rc2.ypBottom)
		{
		// only one line involved
		rc1.xpRight = rc2.xpRight;
		if (rcT.FIntersect(&rc1, &rcClip))
			{
			if (kginDraw == gin)
				pgnv->HiliteRc(&rcT, kacrWhite);
			else
				InvalRc(&rcT, gin);
			}
		return;
		}

	// invert the sel on the first line
	rc1.xpRight = kdxpIndentTxtg - _scvHorz + _DxpDoc();
	if (rcT.FIntersect(&rc1, &rcClip))
		{
		if (kginDraw == gin)
			pgnv->HiliteRc(&rcT, kacrWhite);
		else
			InvalRc(&rcT, gin);
		}

	//invert the main rectangular block
	rc1.xpLeft = kdxpIndentTxtg - _scvHorz;
	rc1.ypTop = rc1.ypBottom;
	rc1.ypBottom = rc2.ypTop;
	if (rcT.FIntersect(&rc1, &rcClip))
		{
		if (kginDraw == gin)
			pgnv->HiliteRc(&rcT, kacrWhite);
		else
			InvalRc(&rcT, gin);
		}

	//invert the last line
	rc2.xpLeft = rc1.xpLeft;
	if (rcT.FIntersect(&rc2, &rcClip))
		{
		if (kginDraw == gin)
			pgnv->HiliteRc(&rcT, kacrWhite);
		else
			InvalRc(&rcT, gin);
		}
}


/***************************************************************************
	Handle a key down.
***************************************************************************/
bool TXTG::FCmdKey(PCMD_KEY pcmd)
{
	const long kcchInsBuf = 64;
	AssertThis(0);
	AssertVarMem(pcmd);
	ulong grfcust;
	long vkDone;
	long ichLim;
	long cact;
	long dcp, cpT;
	CMD cmd;
	LIN lin, linT;
	long ilin, ilinT;
	RC rc;
	achar rgch[kcchInsBuf + 1];

	// keep fetching characters until we get a cursor key, delete key or
	// until the buffer is full.
	vkDone = vkNil;
	ichLim = 0;
	do
		{
		grfcust = pcmd->grfcust;
		switch (pcmd->vk)
			{
		//these keys all terminate the key fetching loop
		case kvkHome:
		case kvkEnd:
		case kvkLeft:
		case kvkRight:
		case kvkUp:
		case kvkDown:
		case kvkPageUp:
		case kvkPageDown:
		case kvkDelete:
		case kvkBack:
			vkDone = pcmd->vk;
			goto LInsert;

		default:
			if (chNil == pcmd->ch)
				break;
			for (cact = 0; cact < pcmd->cact && ichLim < kcchInsBuf; cact++)
				{
				rgch[ichLim++] = (achar)pcmd->ch;
#ifdef WIN
				if ((achar)pcmd->ch == kchReturn)
					rgch[ichLim++] = kchLineFeed;
#endif //WIN
				}
			break;
			}

		pcmd = (PCMD_KEY)&cmd;
		}
	while (ichLim < kcchInsBuf && vpcex->FGetNextKey(&cmd));

LInsert:
	if (ichLim > 0)
		{
		//have some characters to insert
		FReplace(rgch, ichLim, _cpAnchor, _cpOther);
		}

	dcp = 0;
	switch (vkDone)
		{
	case kvkHome:
		if (grfcust & fcustCmd)
			dcp = -_cpOther;
		else
			{
			_FindCp(_cpOther, &lin);
			dcp = lin.cpMin - _cpOther;
			}
		_fXpValid = fFalse;
		goto LSetSel;

	case kvkEnd:
		if (grfcust & fcustCmd)
			dcp = _ptxtb->CpMac() - _cpOther;
		else
			{
			_FindCp(_cpOther, &lin);
			cpT = _ptxtb->CpPrev(lin.cpMin + lin.ccp);
			AssertIn(cpT, _cpOther, _ptxtb->CpMac());
			dcp = cpT - _cpOther;
			}
		_fXpValid = fFalse;
		goto LSetSel;

	case kvkLeft:
		dcp = _ptxtb->CpPrev(_cpOther, FPure(grfcust & fcustCmd)) - _cpOther;
		_fXpValid = fFalse;
		goto LSetSel;

	case kvkRight:
		dcp = _ptxtb->CpNext(_cpOther, FPure(grfcust & fcustCmd)) - _cpOther;
		_fXpValid = fFalse;
		goto LSetSel;

	case kvkUp:
	case kvkPageUp:
	case kvkDown:
	case kvkPageDown:
		// get the LIN for _cpOther and make sure _xpSel is up to date
		_FindCp(_cpOther, &lin, &ilin);
		if (!_fXpValid)
			{
			// get the xp of _cpOther
			_xpSel = lin.xpLeft + _DxpFromCp(lin.cpMin, _cpOther);
			_fXpValid = fTrue;
			}

		switch (vkDone)
			{
		case kvkUp:
			if (ilin == 0)
				{
				dcp = -_cpOther;
				goto LSetSel;
				}
			_FetchLin(ilin - 1, &lin);
			break;

		case kvkPageUp:
			if (ilin == 0)
				{
				dcp = -_cpOther;
				goto LSetSel;
				}
			GetRc(&rc, cooLocal);
			_FindDyp(LwMax(0, lin.dypTot - rc.Dyp() + lin.dyp), &linT, &ilinT);

			if (linT.cpMin >= lin.cpMin)
				{
				// we didn't go anywhere so force going up a line
				_FetchLin(ilin - 1, &lin);
				}
			else if (lin.dypTot + lin.dyp > linT.dypTot + rc.Dyp() &&
					ilinT < ilin - 1)
				{
				// the previous line crosses the bottom of the ddg, so move
				// down one line
				_FetchLin(ilinT + 1, &lin);
				}
			else
				lin = linT;
			break;

		case kvkDown:
			if (lin.cpMin + lin.ccp >= _ptxtb->CpMac())
				{
				dcp = _ptxtb->CpMac() - _cpOther;
				goto LSetSel;
				}
			_FetchLin(ilin + 1, &lin);
			break;

		case kvkPageDown:
			if (lin.cpMin + lin.ccp >= _ptxtb->CpMac())
				{
				dcp = _ptxtb->CpMac() - _cpOther;
				goto LSetSel;
				}

			GetRc(&rc, cooLocal);
			_FindDyp(lin.dypTot + rc.Dyp(), &linT, &ilinT);

			if (linT.cpMin <= lin.cpMin)
				{
				// we didn't go anywhere so force going down a line
				_FetchLin(ilin + 1, &lin);
				}
			else if (linT.dypTot + linT.dyp > lin.dypTot + rc.Dyp()
					&& ilinT > ilin + 1)
				{
				// the line crosses the bottom of the ddg so back up one line
				Assert(ilinT > 0, 0);
				_FetchLin(ilinT - 1, &lin);
				}
			else
				lin = linT;
			break;
			}

		// we have the line, now find the position on the line
		_FGetCpFromXp(_xpSel - _scvHorz, &lin, &dcp);
		dcp -= _cpOther;

LSetSel:
		//move the selection
		if (grfcust & fcustShift)
			{
			//extend selection
			SetSel(_cpAnchor, _cpOther + dcp);
			ShowSel();
			}
		else
			{
			cpT = _cpOther;
			if (cpT == _cpAnchor || (grfcust & fcustCmd))
				cpT += dcp;
			else if ((dcp > 0) != (cpT > _cpAnchor))
				cpT = _cpAnchor;
			SetSel(cpT, cpT);
			ShowSel();
			}
		break;

	case kvkDelete:
	case kvkBack:
		if (_cpAnchor != _cpOther)
			dcp = _cpOther - _cpAnchor;
		else if (vkDone == kvkDelete)
			{
			dcp = _ptxtb->CpNext(_cpAnchor) - _cpAnchor;
			if (_cpAnchor + dcp >= _ptxtb->CpMac())
				dcp = 0;
			}
		else
			dcp = _ptxtb->CpPrev(_cpAnchor) - _cpAnchor;
		if (dcp != 0)
			FReplace(pvNil, 0, _cpAnchor, _cpAnchor + dcp);
		break;
		}

	return fTrue;
}


/***************************************************************************
	Return the maximum scroll value for this view of the doc.
***************************************************************************/
long TXTG::_ScvMax(bool fVert)
{
	RC rc;
	long dxp;

	if (fVert)
		return _ptxtb->CpMac() - 1;

	dxp = _DxpDoc() + 2 * kdxpIndentTxtg;
	GetRc(&rc, cooLocal);
	return LwMax(0, dxp - rc.Dxp());
}


/***************************************************************************
	Return the logical width of the text "page".
***************************************************************************/
long TXTG::_DxpDoc(void)
{
	return _ptxtb->DxpDef();
}


/***************************************************************************
	Set the tab width.  Default does nothing.
***************************************************************************/
void TXTG::SetDxpTab(long dxp)
{
	AssertThis(0);
}


/***************************************************************************
	Set the document width.  Default calls SetDxpDef on the TXTB.
***************************************************************************/
void TXTG::SetDxpDoc(long dxp)
{
	AssertThis(0);
	dxp = LwBound(dxp, 1, kcbMax);
	_ptxtb->SetDxpDef(dxp);
}


/***************************************************************************
	Show or hide the ruler.
***************************************************************************/
void TXTG::ShowRuler(bool fShow)
{
	AssertThis(0);
	RC rcAbs, rcRel;
	long dyp;

	if (FPure(fShow) == (_ptrul != pvNil))
		return;

	dyp = _DypTrul();
	if (fShow)
		{
		PGOB pgob;
		GCB gcb;

		pgob = PgobPar();
		if (pvNil == pgob || !pgob->FIs(kclsDSG))
			return;

		GetPos(&rcAbs, &rcRel);
		rcAbs.ypTop += dyp;
		SetPos(&rcAbs, &rcRel);

		rcRel.ypBottom = rcRel.ypTop;
		rcAbs.ypBottom = rcAbs.ypTop;
		rcAbs.ypTop -= dyp;
		gcb.Set(HidUnique(), pgob, fgobNil, kginDefault, &rcAbs, &rcRel);

		if (pvNil == (_ptrul = _PtrulNew(&gcb)))
			goto LFail;
		}
	else
		{
		ReleasePpo(&_ptrul);
LFail:
		GetPos(&rcAbs, &rcRel);
		rcAbs.ypTop -= dyp;
		SetPos(&rcAbs, &rcRel);
		}
}


/***************************************************************************
	Return the height of the ruler.
***************************************************************************/
long TXTG::_DypTrul(void)
{
	AssertThis(0);
	return 0;
}


/***************************************************************************
	Create the ruler.
***************************************************************************/
PTRUL TXTG::_PtrulNew(PGCB pgcb)
{
	AssertThis(0);
	return pvNil;
}


/***************************************************************************
	Get the natural width and height of the view on the document.
***************************************************************************/
void TXTG::GetNaturalSize(long *pdxp, long *pdyp)
{
	AssertThis(0);
	AssertNilOrVarMem(pdxp);
	AssertNilOrVarMem(pdyp);
	LIN lin;

	if (pvNil != pdxp)
		*pdxp = _DxpDoc() + 2 * kdxpIndentTxtg;
	if (pvNil == pdyp)
		return;

	_FetchLin(kcbMax - 1, &lin);
	*pdyp = lin.dypTot + lin.dyp;
}


/***************************************************************************
	Constructor for the plain line text document display gob.
***************************************************************************/
TXLG::TXLG(PTXTB ptxtb, PGCB pgcb, long onn, ulong grfont, long dypFont, long cchTab)
	: TXLG_PAR(ptxtb, pgcb)
{
	RC rc;
	achar ch = kchSpace;
	GNV gnv(this);

	gnv.SetFont(onn, fontNil, dypFont);
	gnv.GetRcFromRgch(&rc, &ch, 1);
	_dxpChar = rc.Dxp();
	_onn = onn;
	_grfont = grfont;
	_dypFont = dypFont;
	_cchTab = LwBound(cchTab, 1, kcbMax);
}


/***************************************************************************
	Static method to create a new plain line text doc display gob.
***************************************************************************/
PTXLG TXLG::PtxlgNew(PTXTB ptxtb, PGCB pgcb, long onn, ulong grfont,
	long dypFont, long cchTab)
{
	PTXLG ptxlg;

	if (pvNil == (ptxlg = NewObj TXLG(ptxtb, pgcb, onn, grfont, dypFont, cchTab)))
		return pvNil;
	if (!ptxlg->_FInit())
		{
		ReleasePpo(&ptxlg);
		return pvNil;
		}
	ptxlg->Activate(fTrue);
	return ptxlg;
}


/***************************************************************************
	Get the width of the logical "page".  For a TXLG, this is some big
	value, so we do no word wrap.
***************************************************************************/
long TXLG::_DxpDoc(void)
{
	return kswMax;
}


/***************************************************************************
	Set the tab width.
***************************************************************************/
void TXLG::SetDxpTab(long dxp)
{
	AssertThis(0);
	long cch;

	cch = LwBound(dxp / _dxpChar, 1, kswMax / _dxpChar);
	if (cch != _cchTab)
		{
		_cchTab = cch;
		InvalCp(0, _ptxtb->CpMac(), _ptxtb->CpMac());
		}
}


/***************************************************************************
	Set the document width.  Does nothing.
***************************************************************************/
void TXLG::SetDxpDoc(long dxp)
{
	AssertThis(0);
}


/***************************************************************************
	Get the character properties for display.  These are the same for all
	characters in the TXLG.
***************************************************************************/
void TXLG::_FetchChp(long cp, PCHP pchp, long *pcpMin, long *pcpLim)
{
	AssertIn(cp, 0, _ptxtb->CpMac());
	AssertVarMem(pchp);
	AssertNilOrVarMem(pcpMin);
	AssertNilOrVarMem(pcpLim);

	pchp->Clear();
	pchp->grfont = _grfont;
	pchp->onn = _onn;
	pchp->dypFont = _dypFont;
	pchp->acrFore = kacrBlack;
	pchp->acrBack = kacrWhite;
	if (pvNil != pcpMin)
		*pcpMin = 0;
	if (pvNil != pcpLim)
		*pcpLim = _ptxtb->CpMac();
}


/***************************************************************************
	Get the paragraph properties for disply.  These are constant for all
	characters in the TXLG.
***************************************************************************/
void TXLG::_FetchPap(long cp, PPAP ppap, long *pcpMin, long *pcpLim)
{
	AssertIn(cp, 0, _ptxtb->CpMac());
	AssertVarMem(ppap);
	AssertNilOrVarMem(pcpMin);
	AssertNilOrVarMem(pcpLim);

	ClearPb(ppap, size(PAP));
	ppap->dxpTab = (short)LwMul(_cchTab, _dxpChar);
	ppap->numLine = kdenLine;
	ppap->numAfter = kdenAfter;
	if (pvNil != pcpMin)
		*pcpMin = 0;
	if (pvNil != pcpLim)
		*pcpLim = _ptxtb->CpMac();
}


/***************************************************************************
	Copy the selection.
***************************************************************************/
bool TXLG::_FCopySel(PDOCB *ppdocb)
{
	AssertThis(0);
	AssertNilOrVarMem(ppdocb);
	PTXPD ptxpd;

	if (_cpAnchor == _cpOther)
		return fFalse;

	if (pvNil == ppdocb)
		return fTrue;

	if (pvNil != (ptxpd = TXPD::PtxpdNew(pvNil)))
		{
		long cpMin = LwMin(_cpAnchor, _cpOther);
		long cpLim = LwMax(_cpAnchor, _cpOther);

		ptxpd->SuspendUndo();
		if (!ptxpd->FReplaceBsf(_ptxtb->Pbsf(), cpMin, cpLim - cpMin,
				0, 0, fdocNil))
			{
			ReleasePpo(&ptxpd);
			}
		else
			ptxpd->ResumeUndo();
		}

	*ppdocb = ptxpd;
	return pvNil != *ppdocb;
}


/***************************************************************************
	Delete the selection.
***************************************************************************/
void TXLG::_ClearSel(void)
{
	AssertThis(0);

	FReplace(pvNil, 0, _cpAnchor, _cpOther);
	ShowSel();
}


/***************************************************************************
	Paste the selection.
***************************************************************************/
bool TXLG::_FPaste(PCLIP pclip, bool fDoIt, long cid)
{
	AssertThis(0);
	AssertPo(pclip, 0);
	long cp1, cp2;
	long ccp;
	PTXTB ptxtb;

	if (cid != cidPaste || !pclip->FGetFormat(kclsTXTB))
		return fFalse;

	if (!fDoIt)
		return fTrue;

	if (!pclip->FGetFormat(kclsTXTB, (PDOCB *)&ptxtb))
		return fFalse;

	AssertPo(ptxtb, 0);
	if ((ccp = ptxtb->CpMac() - 1) <= 0)
		{
		ReleasePpo(&ptxtb);
		return fTrue;
		}

	HideSel();
	cp1 = LwMin(_cpAnchor, _cpOther);
	cp2 = LwMax(_cpAnchor, _cpOther);
	if (!_ptxtb->FReplaceTxtb(ptxtb, 0, ccp, cp1, cp2 - cp1))
		{
		ReleasePpo(&ptxtb);
		return fFalse;
		}
	ReleasePpo(&ptxtb);

	cp1 += ccp;
	SetSel(cp1, cp1);
	ShowSel();
	return fTrue;
}


/***************************************************************************
	Constructor for a rich text document display gob.
***************************************************************************/
TXRG::TXRG(PTXRD ptxrd, PGCB pgcb) : TXRG_PAR(ptxrd, pgcb)
{
}


/***************************************************************************
	Create a new rich text document display GOB.
***************************************************************************/
PTXRG TXRG::PtxrgNew(PTXRD ptxrd, PGCB pgcb)
{
	PTXRG ptxrg;

	if (pvNil == (ptxrg = NewObj TXRG(ptxrd, pgcb)))
		return pvNil;
	if (!ptxrg->_FInit())
		{
		ReleasePpo(&ptxrg);
		return pvNil;
		}
	ptxrg->Activate(fTrue);
	return ptxrg;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a TXRG.
***************************************************************************/
void TXRG::AssertValid(ulong grf)
{
	TXRG_PAR::AssertValid(0);
	AssertNilOrPo(_ptrul, 0);
}
#endif //DEBUG


/***************************************************************************
	Get the character properties for displaying the given cp.
***************************************************************************/
void TXRG::_FetchChp(long cp, PCHP pchp, long *pcpMin, long *pcpLim)
{
	((PTXRD)_ptxtb)->FetchChp(cp, pchp, pcpMin, pcpLim);
}


/***************************************************************************
	Get the paragraph properties for displaying the given cp.
***************************************************************************/
void TXRG::_FetchPap(long cp, PPAP ppap, long *pcpMin, long *pcpLim)
{
	((PTXRD)_ptxtb)->FetchPap(cp, ppap, pcpMin, pcpLim);
}


/***************************************************************************
	Set the tab width for the currently selected paragraph(s).
***************************************************************************/
void TXRG::SetDxpTab(long dxp)
{
	AssertThis(0);
	long cpMin, cpLim, cpAnchor, cpOther;
	PAP papOld, papNew;

	dxp = LwRoundClosest(dxp, kdzpInch / 8);
	dxp = LwBound(dxp, kdzpInch / 8, 100 * kdzpInch);
	ClearPb(&papOld, size(PAP));
	papNew = papOld;

	cpMin = LwMin(cpAnchor = _cpAnchor, cpOther = _cpOther);
	cpLim = LwMax(_cpAnchor, _cpOther);

	papNew.dxpTab = (short)dxp;
	_SwitchSel(fFalse, ginNil);
	if (!((PTXRD)_ptxtb)->FApplyPap(cpMin, cpLim - cpMin, &papNew, &papOld,
			&cpMin, &cpLim))
		{
		_SwitchSel(fTrue, kginMark);
		}

	SetSel(cpAnchor, cpOther);
	ShowSel();
}


/***************************************************************************
	Set the selection for the TXRG.  Invalidates _chpIns if the selection
	changes.
***************************************************************************/
void TXRG::SetSel(long cpAnchor, long cpOther, long gin)
{
	AssertThis(0);
	long cpMac = _ptxtb->CpMac();

	cpAnchor = LwBound(cpAnchor, 0, cpMac);
	cpOther = LwBound(cpOther, 0, cpMac);

	if (cpAnchor == _cpAnchor && cpOther == _cpOther)
		return;

	_fValidChp = fFalse;
	TXRG_PAR::SetSel(cpAnchor, cpOther, gin);
	if (pvNil != _ptrul)
		{
		PAP pap;

		_FetchPap(LwMin(_cpAnchor, _cpOther), &pap);
		_ptrul->SetDxpTab(pap.dxpTab);
		}
}


/***************************************************************************
	Get the chp to use to replace the given range of characters. If the
	range is empty and not at the beginning of a paragraph, gets the chp
	of the previous character. Otherwise gets the chp of the character at
	LwMin(cp1, cp2).
***************************************************************************/
void TXRG::_FetchChpSel(long cp1, long cp2, PCHP pchp)
{
	AssertThis(0);
	AssertVarMem(pchp);

	long cp = LwMin(cp1, cp2);

	if (cp1 == cp2 && cp1 > 0)
		{
		if (!_ptxtb->FMinPara(cp) ||
				_ptxtb->FMinPara(cp2 = _ptxtb->CpNext(cp)) ||
				cp2 >= _ptxtb->CpMac())
			{
			cp = _ptxtb->CpPrev(cp);
			}
		}

	// NOTE: don't just call _FetchChp here.  _FetchChp allows derived
	// classes to modify the chp used for display without affecting the
	// chp used for editing.  This chp is an editing chp.  Same note
	// applies several other places in this file.
	((PTXRD)_ptxtb)->FetchChp(cp, pchp);
}


/***************************************************************************
	Make sure the _chpIns is valid.
***************************************************************************/
void TXRG::_EnsureChpIns(void)
{
	AssertThis(0);

	if (!_fValidChp)
		{
		_FetchChpSel(_cpAnchor, _cpOther, &_chpIns);
		_fValidChp = fTrue;
		}
}


/***************************************************************************
	Replaces the characters between cp1 and cp2 with the given ones.
***************************************************************************/
bool TXRG::FReplace(achar *prgch, long cch, long cp1, long cp2)
{
	AssertIn(cch, 0, kcbMax);
	AssertPvCb(prgch, cch);
	AssertIn(cp1, 0, _ptxtb->CpMac());
	AssertIn(cp2, 0, _ptxtb->CpMac());
	CHP chp;
	bool fSetChpIns = fFalse;

	HideSel();
	SortLw(&cp1, &cp2);
	if (cp1 == LwMin(_cpAnchor, _cpOther) && cp2 == LwMax(_cpAnchor, _cpOther))
		{
		_EnsureChpIns();
		chp = _chpIns;
		fSetChpIns = fTrue;
		}
	else
		_FetchChpSel(cp1, cp2, &chp);

	if (!((PTXRD)_ptxtb)->FReplaceRgch(prgch, cch, cp1, cp2 - cp1, &chp))
		return fFalse;

	cp1 += cch;
	SetSel(cp1, cp1);
	ShowSel();

	if (fSetChpIns)
		{
		// preserve the _chpIns
		_chpIns = chp;
		_fValidChp = fTrue;
		}

	return fTrue;
}


/***************************************************************************
	Copy the selection.
***************************************************************************/
bool TXRG::_FCopySel(PDOCB *ppdocb)
{
	AssertNilOrVarMem(ppdocb);
	PTXRD ptxrd;

	if (_cpAnchor == _cpOther)
		return fFalse;

	if (pvNil == ppdocb)
		return fTrue;

	if (pvNil != (ptxrd = TXRD::PtxrdNew(pvNil)))
		{
		long cpMin = LwMin(_cpAnchor, _cpOther);
		long cpLim = LwMax(_cpAnchor, _cpOther);

		ptxrd->SuspendUndo();
		if (!ptxrd->FReplaceTxrd((PTXRD)_ptxtb, cpMin, cpLim - cpMin,
				0, 0, fdocNil))
			{
			ReleasePpo(&ptxrd);
			}
		else
			ptxrd->ResumeUndo();
		}

	*ppdocb = ptxrd;
	return pvNil != *ppdocb;
}


/***************************************************************************
	Delete the selection.
***************************************************************************/
void TXRG::_ClearSel(void)
{
	FReplace(pvNil, 0, _cpAnchor, _cpOther);
	ShowSel();
}


/***************************************************************************
	Paste the selection.
***************************************************************************/
bool TXRG::_FPaste(PCLIP pclip, bool fDoIt, long cid)
{
	AssertThis(0);
	long cp1, cp2;
	long ccp;
	PTXTB ptxtb;
	bool fRet;

	if (cid != cidPaste || !pclip->FGetFormat(kclsTXTB))
		return fFalse;

	if (!fDoIt)
		return fTrue;

	if (!pclip->FGetFormat(kclsTXRD, (PDOCB *)&ptxtb) &&
			!pclip->FGetFormat(kclsTXTB, (PDOCB *)&ptxtb))
		{
		return fFalse;
		}

	AssertPo(ptxtb, 0);
	if ((ccp = ptxtb->CpMac() - 1) <= 0)
		{
		ReleasePpo(&ptxtb);
		return fTrue;
		}

	HideSel();
	cp1 = LwMin(_cpAnchor, _cpOther);
	cp2 = LwMax(_cpAnchor, _cpOther);
	_ptxtb->BumpCombineUndo();
	if (ptxtb->FIs(kclsTXRD))
		{
		fRet = ((PTXRD)_ptxtb)->FReplaceTxrd((PTXRD)ptxtb, 0, ccp,
			cp1, cp2 - cp1);
		}
	else
		{
		_EnsureChpIns();
		fRet = ((PTXRD)_ptxtb)->FReplaceTxtb(ptxtb, 0, ccp, cp1, cp2 - cp1,
			&_chpIns);
		}
	ReleasePpo(&ptxtb);

	if (fRet)
		{
		_ptxtb->BumpCombineUndo();
		cp1 += ccp;
		SetSel(cp1, cp1);
		ShowSel();
		}

	return fRet;
}


/***************************************************************************
	Apply the given character properties to the current selection.
***************************************************************************/
bool TXRG::FApplyChp(PCHP pchp, PCHP pchpDiff)
{
	AssertThis(0);
	AssertVarMem(pchp);
	AssertNilOrVarMem(pchpDiff);

	long cpMin, cpLim, cpAnchor, cpOther;
	ulong grfont;

	cpMin = LwMin(cpAnchor = _cpAnchor, cpOther = _cpOther);
	cpLim = LwMax(_cpAnchor, _cpOther);

	if (cpMin == cpLim)
		{
		if (pvNil == pchpDiff)
			{
			_chpIns = *pchp;
			_fValidChp = fTrue;
			return fTrue;
			}

		_EnsureChpIns();
		if (fontNil != (grfont = pchp->grfont ^ pchpDiff->grfont))
			_chpIns.grfont = (_chpIns.grfont & ~grfont) | (pchp->grfont & grfont);
		if (pchp->onn != pchpDiff->onn)
			_chpIns.onn = pchp->onn;
		if (pchp->dypFont != pchpDiff->dypFont)
			_chpIns.dypFont = pchp->dypFont;
		if (pchp->dypOffset != pchpDiff->dypOffset)
			_chpIns.dypOffset = pchp->dypOffset;
		if (pchp->acrFore != pchpDiff->acrFore)
			_chpIns.acrFore = pchp->acrFore;
		if (pchp->acrBack != pchpDiff->acrBack)
			_chpIns.acrBack = pchp->acrBack;
		AssertThis(0);
		return fTrue;
		}

	_SwitchSel(fFalse, ginNil);
	if (!((PTXRD)_ptxtb)->FApplyChp(cpMin, cpLim - cpMin, pchp, pchpDiff))
		{
		_SwitchSel(fTrue, kginMark);
		return fFalse;
		}
	_fValidChp = fFalse;
	SetSel(cpAnchor, cpOther);
	ShowSel();

	return fTrue;
}


/***************************************************************************
	Apply the given paragraph properties to the current selection.
***************************************************************************/
bool TXRG::FApplyPap(PPAP ppap, PPAP ppapDiff, bool fExpand)
{
	AssertThis(0);
	AssertVarMem(ppap);
	AssertNilOrVarMem(ppapDiff);
	long cpMin, cpLim, cpAnchor, cpOther;

	cpMin = LwMin(cpAnchor = _cpAnchor, cpOther = _cpOther);
	cpLim = LwMax(_cpAnchor, _cpOther);

	_SwitchSel(fFalse, ginNil);
	if (!((PTXRD)_ptxtb)->FApplyPap(cpMin, cpLim - cpMin, ppap, ppapDiff,
			pvNil, pvNil, fExpand))
		{
		_SwitchSel(fTrue, kginMark);
		return fFalse;
		}

	SetSel(cpAnchor, cpOther);
	ShowSel();

	return fTrue;
}


/***************************************************************************
	Apply a character or paragraph property
***************************************************************************/
bool TXRG::FCmdApplyProperty(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CHP chpOld, chpNew;
	PAP papOld, papNew;
	long onn;
	STN stn;

	ClearPb(&papOld, size(PAP));
	papNew = papOld;

	_EnsureChpIns();
	chpOld = chpNew = _chpIns;

	switch (pcmd->cid)
		{
	case cidPlain:
		chpNew.grfont = fontNil;
		chpOld.grfont = (ulong)~fontNil;
		goto LApplyChp;
	case cidBold:
		chpNew.grfont ^= fontBold;
		goto LApplyChp;
	case cidItalic:
		chpNew.grfont ^= fontItalic;
		goto LApplyChp;
	case cidUnderline:
		chpNew.grfont ^= fontUnderline;
		goto LApplyChp;
	case cidChooseFont:
		if (pvNil == pcmd->pgg || pcmd->pgg->IvMac() != 1 ||
				!stn.FSetData(pcmd->pgg->QvGet(0), pcmd->pgg->Cb(0)))
			{
			break;
			}
		if (!vntl.FGetOnn(&stn, &onn))
			break;
		chpNew.onn = onn;
		chpOld.onn = ~onn;
		goto LApplyChp;
	case cidChooseSubSuper:
		// the amount to offset by is pcmd->rglw[0] ^ (1L << 31), so 0 can be
		// used to indicate that we're to ask the user.
		if (pcmd->rglw[0] == 0)
			{
			long dyp = _chpIns.dypOffset;

			// ask the user for the amount to sub/superscript by
			if (!_FGetOtherSubSuper(&dyp))
				return fTrue;
			pcmd->rglw[0] = dyp ^ (1L << 31);
			}
		chpNew.dypOffset = pcmd->rglw[0] ^ (1L << 31);
		chpOld.dypOffset = ~chpNew.dypOffset;
		goto LApplyChp;
	case cidChooseFontSize:
		if (pcmd->rglw[0] == 0)
			{
			long dyp = _chpIns.dypFont;

			// ask the user for the font size
			if (!_FGetOtherSize(&dyp))
				return fTrue;
			pcmd->rglw[0] = dyp;
			}
		if (!FIn(pcmd->rglw[0], 6, 256))
			return fTrue;
		chpNew.dypFont = pcmd->rglw[0];
		chpOld.dypFont = ~pcmd->rglw[0];
LApplyChp:
		FApplyChp(&chpNew, &chpOld);
		break;

	case cidJustifyLeft:
		papNew.jc = jcLeft;
		papOld.jc = jcLim;
		goto LApplyPap;
	case cidJustifyCenter:
		papNew.jc = jcCenter;
		papOld.jc = jcLim;
		goto LApplyPap;
	case cidJustifyRight:
		papNew.jc = jcRight;
		papOld.jc = jcLim;
		goto LApplyPap;
	case cidIndentNone:
		papNew.nd = ndNone;
		papOld.nd = ndLim;
		goto LApplyPap;
	case cidIndentFirst:
		papNew.nd = ndFirst;
		papOld.nd = ndLim;
		goto LApplyPap;
	case cidIndentRest:
		papNew.nd = ndRest;
		papOld.nd = ndLim;
		goto LApplyPap;
	case cidIndentAll:
		papNew.nd = ndAll;
		papOld.nd = ndLim;
LApplyPap:
		FApplyPap(&papNew, &papOld);
		break;
		}

	return fTrue;
}


/***************************************************************************
	Get a font size from the user.
***************************************************************************/
bool TXRG::_FGetOtherSize(long *pdypFont)
{
	AssertThis(0);
	AssertVarMem(pdypFont);

	TrashVar(pdypFont);
	return fFalse;
}


/***************************************************************************
	Get the amount to sub/superscript from the user.
***************************************************************************/
bool TXRG::_FGetOtherSubSuper(long *pdypOffset)
{
	AssertThis(0);
	AssertVarMem(pdypOffset);

	TrashVar(pdypOffset);
	return fFalse;
}


/***************************************************************************
	Apply a character or paragraph property
***************************************************************************/
bool TXRG::FSetColor(ACR *pacrFore, ACR *pacrBack)
{
	AssertThis(0);
	AssertNilOrPo(pacrFore, 0);
	AssertNilOrPo(pacrBack, 0);
	CHP chp, chpDiff;

	chp.Clear();
	chpDiff.Clear();

	if (pvNil != pacrFore)
		{
		chp.acrFore = *pacrFore;
		chpDiff.acrFore.SetToInvert();
		if (chpDiff.acrFore == chp.acrFore)
			chpDiff.acrFore.SetToClear();
		}
	if (pvNil != pacrBack)
		{
		chp.acrBack = *pacrBack;
		chpDiff.acrBack.SetToInvert();
		if (chpDiff. acrBack == chp.acrBack)
			chpDiff.acrBack.SetToClear();
		}

	return FApplyChp(&chp, &chpDiff);
}


/***************************************************************************
	Enable, check/uncheck property commands.
***************************************************************************/
bool TXRG::FEnablePropCmd(PCMD pcmd, ulong *pgrfeds)
{
	PAP pap;
	bool fCheck;
	STN stn, stnT;

	_EnsureChpIns();
	((PTXRD)_ptxtb)->FetchPap(LwMin(_cpAnchor, _cpOther), &pap);

	switch (pcmd->cid)
		{
	case cidPlain:
		fCheck = (_chpIns.grfont == fontNil);
		break;
	case cidBold:
		fCheck = FPure(_chpIns.grfont & fontBold);
		break;
	case cidItalic:
		fCheck = FPure(_chpIns.grfont & fontItalic);
		break;
	case cidUnderline:
		fCheck = FPure(_chpIns.grfont & fontUnderline);
		break;

	case cidChooseFont:
		if (pvNil == pcmd->pgg || pcmd->pgg->IvMac() != 1)
			break;
		if (!stnT.FSetData(pcmd->pgg->PvLock(0), pcmd->pgg->Cb(0)))
			{
			pcmd->pgg->Unlock();
			break;
			}
		pcmd->pgg->Unlock();
		vntl.GetStn(_chpIns.onn, &stn);
		fCheck = stn.FEqual(&stnT);
		break;

	case cidChooseFontSize:
		fCheck = (_chpIns.dypFont == pcmd->rglw[0]);
		break;
	case cidChooseSubSuper:
		fCheck = (_chpIns.dypOffset == (pcmd->rglw[0] ^ (1L << 31)));
		break;

	case cidJustifyLeft:
		fCheck = (pap.jc == jcLeft);
		break;
	case cidJustifyCenter:
		fCheck = (pap.jc == jcCenter);
		break;
	case cidJustifyRight:
		fCheck = (pap.jc == jcRight);
		break;
	case cidIndentNone:
		fCheck = (pap.nd == ndNone);
		break;
	case cidIndentFirst:
		fCheck = (pap.nd == ndFirst);
		break;
	case cidIndentRest:
		fCheck = (pap.nd == ndRest);
		break;
	case cidIndentAll:
		fCheck = (pap.nd == ndAll);
		break;
		}
	*pgrfeds = fCheck ? fedsEnable | fedsCheck : fedsEnable | fedsUncheck;

	return fTrue;
}

