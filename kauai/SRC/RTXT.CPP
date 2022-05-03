/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Rich text document and associated DDG.

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(TXTB)
RTCLASS(TXPD)
RTCLASS(TXRD)
RTCLASS(TXTG)
RTCLASS(TXLG)
RTCLASS(TXRG)
RTCLASS(RTUN)


BEGIN_CMD_MAP(TXRG, DDG)
	ON_CID_GEN(cidPlain, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidBold, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidItalic, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidUnderline, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidJustifyLeft, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidJustifyCenter, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidJustifyRight, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidIndentNone, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidIndentFirst, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidIndentRest, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidIndentAll, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidChooseFont, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidChooseFontSize, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
	ON_CID_GEN(cidChooseSubSuper, &TXRG::FCmdApplyProperty, &TXRG::FEnablePropCmd)
END_CMD_MAP_NIL()


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the rich text doc.
***************************************************************************/
void TXTB::AssertValid(ulong grfobj)
{
	achar ch;
	long ibMac;
	long cpMac;

	TXTB_PAR::AssertValid(grfobj);
	AssertPo(_pbsf, 0);
	ibMac = _pbsf->IbMac();
	cpMac = ibMac / size(achar);
	Assert(ibMac % size(achar) == 0, "ibMac not divisible by size(achar)");
	AssertIn(ibMac, size(achar), kcbMax);
	AssertIn(_cpMinCache, 0, cpMac + 1);
	AssertIn(_cpLimCache, 0, cpMac + 1);
	AssertIn(_cpLimCache, _cpMinCache, _cpMinCache + size(_rgchCache) + 1);
	AssertNilOrPo(_pfil, 0);
	if (grfobj & fobjAssertFull)
		{
		_pbsf->FetchRgb(ibMac - size(achar), size(achar), &ch);
		Assert(ch == kchReturn, "stream doesn't end with a carriage return");
		}
}


/***************************************************************************
	Mark memory for the TXTB.
***************************************************************************/
void TXTB::MarkMem(void)
{
	AssertThis(fobjAssertFull);
	TXTB_PAR::MarkMem();
	MarkMemObj(_pbsf);
}
#endif //DEBUG


/***************************************************************************
	Constructor for the base text document class
***************************************************************************/
TXTB::TXTB(PDOCB pdocb, ulong grfdoc) : TXTB_PAR(pdocb, grfdoc)
{
	_acrBack = kacrWhite;
	_dxpDef = kdxpDocDef;
}


/***************************************************************************
	Destructor for the base text document class
***************************************************************************/
TXTB::~TXTB(void)
{
	ReleasePpo(&_pbsf);
	ReleasePpo(&_pfil);
}


/***************************************************************************
	Initializer for the base text document class.
***************************************************************************/
bool TXTB::_FInit(PFNI pfni, PBSF pbsf, short osk)
{
	AssertNilOrPo(pfni, ffniFile);
	AssertNilOrPo(pbsf, 0);
	achar ch;

	if (pvNil != pfni && pvNil == (_pfil = FIL::PfilOpen(pfni)))
		return fFalse;

	if (pvNil != pbsf)
		{
		if (osk != koskCur)
			{
			Bug("Can't translate a BSF");
			return fFalse;
			}
		if (pbsf->IbMac() % size(achar) != 0)
			{
			Bug("BSF has a partial character");
			return fFalse;
			}
		pbsf->AddRef();
		_pbsf = pbsf;
		}
	else if (pvNil == (_pbsf = NewObj BSF))
		return fFalse;
	else if (!_FLoad(osk))
		return fFalse;

	//append a return character
	ch = kchReturn;
	if (!_pbsf->FReplace(&ch, size(achar), _pbsf->IbMac(), 0))
		return fFalse;

	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Load the document from its file
***************************************************************************/
bool TXTB::_FLoad(short osk)
{
	//initialize the BSF to just point to the file
	FLO flo;
	bool fRet = fFalse;

	if (pvNil == (flo.pfil = _pfil) || 0 == (flo.cb = _pfil->FpMac()))
		return fTrue;

	flo.pfil->AddRef();
	flo.fp = 0;
	if (flo.FTranslate(osk))
		{
		fRet = flo.cb == 0 ||
			_pbsf->FReplaceFlo(&flo, fFalse, 0, _pbsf->IbMac());
		}

	ReleasePpo(&flo.pfil);
	return fRet;
}


/***************************************************************************
	Return the length of the text in the text document.
***************************************************************************/
long TXTB::CpMac(void)
{
	//Note: we don't do an AssertThis(0) here for debug performance
	AssertThisMem();
	AssertVarMem(_pbsf);
	Assert(_pbsf->IbMac() % size(achar) == 0,
		"IbMac() not divisible by size(achar)");

	return _pbsf->IbMac() / size(achar);
}


/***************************************************************************
	Suspend undo. This increments a count.
***************************************************************************/
void TXTB::SuspendUndo(void)
{
	AssertThis(0);
	_cactSuspendUndo++;
}


/***************************************************************************
	Resume undo. This decrements a count.
***************************************************************************/
void TXTB::ResumeUndo(void)
{
	AssertThis(0);
	Assert(_cactSuspendUndo > 0, "bad _cactSuspendUndo");
	_cactSuspendUndo--;
}


/***************************************************************************
	Set up undo for an action. If this succeeds, you must call either
	CancelUndo or CommitUndo. Default TXTB doesn't create an undo record.
***************************************************************************/
bool TXTB::FSetUndo(long cp1, long cp2, long ccpIns)
{
	AssertThis(0);
	AssertIn(cp1, 0, CpMac() + 1);
	AssertIn(cp2, 0, CpMac() + 1);
	AssertIn(ccpIns, 0, kcbMax);

	_cactSuspendUndo++;
	return fTrue;
}


/***************************************************************************
	Cancel undo.
***************************************************************************/
void TXTB::CancelUndo(void)
{
	ResumeUndo();
}


/***************************************************************************
	Commit the setup undo.
***************************************************************************/
void TXTB::CommitUndo(void)
{
	ResumeUndo();
}


/***************************************************************************
	Increments the _cactCombineUndo counter. This is used to determine
	whether the new undo record can be combined with the previous one
	(during typing etc).
***************************************************************************/
void TXTB::BumpCombineUndo(void)
{
	AssertThis(0);
	if (_cactSuspendUndo <= 0)
		_cactCombineUndo++;
}


/***************************************************************************
	Search for some text. If fCaseSensitive is false, we map prgch to all
	lower case. The pcpLim parameter is in case we support regular
	expressions in the future.
***************************************************************************/
bool TXTB::FFind(achar *prgch, long cch, long cpMin,
	long *pcpMin, long *pcpLim, bool fCaseSensitive)
{
	AssertThis(fobjAssertFull);
	AssertIn(cch, 1, kcbMax);
	AssertPvCb(prgch, cch * size(achar));
	AssertIn(cpMin, 0, CpMac() + 1);
	AssertVarMem(pcpMin);
	AssertVarMem(pcpLim);
	const long kcbCharSet = 256 / 8;
	byte grfbitUsed[kcbCharSet];
	long ibit;
	achar *pch, *pchLast;
	achar ch;
	long cpT, cpMac;

	if (!fCaseSensitive)
		LowerRgch(prgch, cch);

	cch--;
	pchLast = prgch + cch;

	// calculate the grfbitUsed bit field - indicating, which characters
	// appear in the search string.
	ClearPb(grfbitUsed, size(grfbitUsed));
	for (pch = prgch; pch <= pchLast; pch++)
		{
		ibit = (long)(byte)*pch;
		AssertIn(ibit, 0, 256);
		grfbitUsed[IbFromIbit(ibit)] |= Fbit(ibit);
		}

	for (cpMac = CpMac(); (cpT = cpMin + cch) < cpMac; )
		{
		for (pch = pchLast; ; cpT--, pch--)
			{
			// get the character and map it to lower case if we're doing
			// a case insensitive search
			ch = _ChFetch(cpT);
			if (!fCaseSensitive)
				ch = ChLower(ch);

			// see if the character is used anywhere within the search string
			ibit = (long)(byte)ch;
			if (!(grfbitUsed[IbFromIbit(ibit)] & Fbit(ibit)))
				{
				// this character isn't anywhere in the search string,
				// so we can jump by a lot.
				cpMin = cpT + 1;
				break;
				}

			// see if the character matches the corresponding character in
			// the search string
			if (ch != *pch)
				{
				// character doesn't match, just jump by one
				cpMin++;
				break;
				}

			// if cpT is back to cpMin, we've got a match
			if (cpT == cpMin)
				{
				// we matched
				*pcpMin = cpMin;
				*pcpLim = cpMin + cch + 1;
				return fTrue;
				}
			}
		}

	TrashVar(pcpMin);
	TrashVar(pcpLim);
	return fFalse;
}


/***************************************************************************
	Turn the selection of this TXTB's active DDG off.
***************************************************************************/
void TXTB::HideSel(void)
{
	AssertThis(0);
	PDDG pddg;

	if (pvNil != (pddg = PddgActive()) && pddg->FIs(kclsTXTG))
		((PTXTG)pddg)->HideSel();
}


/***************************************************************************
	Set the selection of this TXTB's active DDG to the given range.
***************************************************************************/
void TXTB::SetSel(long cpAnchor, long cpOther, long gin)
{
	AssertThis(0);
	PDDG pddg;

	if (pvNil != (pddg = PddgActive()) && pddg->FIs(kclsTXTG))
		((PTXTG)pddg)->SetSel(cpAnchor, cpOther, gin);
}


/***************************************************************************
	Make sure the selection of this TXTB's acttive DDG is visible (at least
	the _cpOther end of it).
***************************************************************************/
void TXTB::ShowSel(void)
{
	AssertThis(0);
	PDDG pddg;

	if (pvNil != (pddg = PddgActive()) && pddg->FIs(kclsTXTG))
		((PTXTG)pddg)->ShowSel();
}


/***************************************************************************
	Fetch a character of the stream through the cache.
***************************************************************************/
void TXTB::_CacheRange(long cpMin, long cpLim)
{
	AssertThis(0);
	AssertIn(cpMin, 0, CpMac() + 1);
	AssertIn(cpLim, cpMin, CpMac() + 1);
	AssertIn(cpLim - cpMin, 0, kcchMaxTxtbCache + 1);
	long cpMac;

	if (cpMin >= _cpMinCache && cpLim <= _cpLimCache)
		return;

	cpMac = CpMac();
	if (_cpLimCache <= _cpMinCache)
		{
		//nothing currently cached
		goto LNewCache;
		}

	if (cpMin < _cpMinCache)
		{
		long cpT;

		if (_cpMinCache >= cpMin + kcchMaxTxtbCache)
			goto LNewCache;

		//keep front end of old stuff
		cpT = LwMax(0, LwMin(cpMin, _cpLimCache - kcchMaxTxtbCache));
		_cpLimCache = LwMin(cpT + kcchMaxTxtbCache, _cpLimCache);
		BltPb(_rgchCache, _rgchCache + (_cpMinCache - cpT),
			(_cpLimCache - _cpMinCache) * size(achar));
		_pbsf->FetchRgb(cpT * size(achar),
			(_cpMinCache - cpT) * size(achar), _rgchCache);
		_cpMinCache = cpT;
		}
	else
		{
		Assert(cpLim > _cpLimCache, 0);
		if (cpLim >= _cpLimCache + kcchMaxTxtbCache)
			{
LNewCache:
			//just cache around [cpMin, cpLim)
			_cpMinCache = LwMax(0, LwMin((cpMin + cpLim - kcchMaxTxtbCache) / 2,
				cpMac - kcchMaxTxtbCache));
			_cpLimCache = LwMin(_cpMinCache + kcchMaxTxtbCache, cpMac);
			_pbsf->FetchRgb(_cpMinCache * size(achar),
				(_cpLimCache - _cpMinCache) * size(achar), _rgchCache);
			}
		else
			{
			//keep back end old stuff
			long cpLimCache, cpMinCache;

			cpLimCache = LwMin(cpMac, LwMax(cpLim, _cpMinCache + kcchMaxTxtbCache));
			cpMinCache = LwMax(cpLimCache - kcchMaxTxtbCache, _cpMinCache);
			if (cpMinCache != _cpMinCache)
				{
				BltPb(_rgchCache + (cpMinCache - _cpMinCache), _rgchCache,
					(_cpLimCache - cpMinCache) * size(achar));
				}
			_pbsf->FetchRgb(_cpLimCache * size(achar),
				(cpLimCache - _cpLimCache) * size(achar),
				_rgchCache + _cpLimCache - cpMinCache);
			_cpMinCache = cpMinCache;
			_cpLimCache = cpLimCache;
			}
		}
}


/***************************************************************************
	Characters have changed, so fix the cache.
***************************************************************************/
void TXTB::_InvalCache(long cp, long ccpIns, long ccpDel)
{
	long dcpFront, dcpBack;

	if (_cpLimCache <= cp || _cpLimCache <= _cpMinCache ||
			ccpIns == 0 && ccpDel == 0)
		{
		//cache is before the edit or cache is already empty
		return;
		}

	if (_cpMinCache >= cp + ccpDel)
		{
		//cache is after the edit
		_cpMinCache += ccpIns - ccpDel;
		_cpLimCache += ccpIns - ccpDel;
		return;
		}

	dcpFront = cp - _cpMinCache;
	dcpBack = _cpLimCache - cp - ccpDel;
	if (dcpFront <= 0 && dcpBack <= 0)
		_cpMinCache = _cpLimCache = 0;
	else if (dcpFront >= dcpBack)
		{
		//keep the front end of the cache
		_cpLimCache = cp;
		}
	else
		{
		//keep the tail end of the cache
		BltPb(_rgchCache + cp + ccpDel - _cpMinCache, _rgchCache,
			dcpBack * size(achar));
		_cpMinCache = cp + ccpIns;
		_cpLimCache = _cpMinCache + dcpBack;
		}
}


/***************************************************************************
	Fetch a character of the stream through the cache.
***************************************************************************/
achar TXTB::_ChFetch(long cp)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac() + 1);

	if (!FIn(cp, _cpMinCache, _cpLimCache))
		{
		//not a cache hit
		_CacheRange(cp, cp + 1);
		}

	return _rgchCache[cp - _cpMinCache];
}


/***************************************************************************
	Fetch some characters from the text document.
***************************************************************************/
void TXTB::FetchRgch(long cp, long ccp, achar *prgch)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac() + 1);
	AssertIn(ccp, 0, CpMac() - cp + 1);
	AssertPvCb(prgch, ccp * size(achar));
	long ccpT;

	while (ccp > 0)
		{
		ccpT = LwMin(ccp, kcchMaxTxtbCache);
		_CacheRange(cp, cp + ccpT);
		CopyPb(_rgchCache + cp - _cpMinCache, prgch, ccpT * size(achar));
		prgch += ccpT;
		cp += ccpT;
		ccp -= ccpT;
		}
}


/***************************************************************************
	Returns non-zero iff cp is the beginning of a paragraph.
***************************************************************************/
bool TXTB::FMinPara(long cp)
{
	AssertThis(0);
	ulong grfch;

	if (cp <= 0)
		return cp == 0;
	if (cp >= CpMac())
		return fFalse;
	if (GrfchFromCh(_ChFetch(cp)) & fchIgnore)
		return fFalse;

	// return true iff the first non-ignore character we see
	// is a break character
	AssertIn(cp, 1, CpMac());
	while (cp-- > 0)
		{
		grfch = GrfchFromCh(_ChFetch(cp));
		if (fchBreak & grfch)
			return fTrue;
		if (!(fchIgnore & grfch))
			return fFalse;
		}

	//just line feeds!
	Warn("isolated line feeds");
	return fFalse;
}


/***************************************************************************
	Find the beginning of the paragraph that cp is in. If cp <= 0,
	returns 0. If cp >= CpMac(), returns the beginning of the last
	paragraph.
***************************************************************************/
long TXTB::CpMinPara(long cp)
{
	AssertThis(0);
	ulong grfch;
	long cpOrig;
	long dcpLine = 1;

	if (cp <= 0)
		return 0;

	if (cp >= CpMac())
		cp = CpMac() - 1;
	else if (GrfchFromCh(_ChFetch(cp)) & fchIgnore)
		dcpLine++;
	cpOrig = cp;

	AssertIn(cp, 1, CpMac());
	while (cp-- > 0)
		{
		grfch = GrfchFromCh(_ChFetch(cp));
		if (fchBreak & grfch)
			{
			if (cp + dcpLine <= cpOrig)
				return cp + dcpLine;
			dcpLine = 1;
			}
		else if (fchIgnore & grfch)
			dcpLine++;
		else
			dcpLine = 1;
		}

	return 0;
}


/***************************************************************************
	Find the end of the paragraph that cp is in. If cp < 0, returns 0.
	If cp >= CpMac(), returns CpMac().
***************************************************************************/
long TXTB::CpLimPara(long cp)
{
	AssertThis(0);
	ulong grfch;
	bool fCr = fFalse;
	long cpLim = CpMac();

	if (cp < 0)
		return 0;
	if (cp >= cpLim)
		return cpLim;

	//cp should now be a legal index into the character stream
	AssertIn(cp, 0, cpLim);
	while (cp > 0 && (GrfchFromCh(_ChFetch(cp)) & fchIgnore))
		cp--;

	for ( ; cp < cpLim; cp++)
		{
		grfch = GrfchFromCh(_ChFetch(cp));
		if (fchBreak & grfch)
			{
			if (fCr)
				return cp;
			fCr = fTrue;
			}
		else if (!(fchIgnore & grfch) && fCr)
			return cp;
		}
	Assert(fCr, "last character is not a return character");
	return cpLim;
}


/***************************************************************************
	Return cp of the previous character, skipping line feed characters. If
	fWord is true, skip to the beginning of a word.
***************************************************************************/
long TXTB::CpPrev(long cp, bool fWord)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac() + 1);

	if (!fWord)
		{
		while (cp > 0 && (fchIgnore & GrfchFromCh(_ChFetch(--cp))))
			;
		}
	else
		{
		while (cp > 0 &&
				((fchIgnore | fchMayBreak) & GrfchFromCh(_ChFetch(cp - 1))))
			{
			cp--;
			}
		while (cp > 0 &&
				!((fchIgnore | fchMayBreak) & GrfchFromCh(_ChFetch(cp - 1))))
			{
			cp--;
			}
		}

	return cp;
}


/***************************************************************************
	Return cp of the next character, skipping line feed characters. If
	fWord is true, skip to the beginning of the next word.
***************************************************************************/
long TXTB::CpNext(long cp, bool fWord)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac() + 1);
	long cpMac = CpMac();

	if (cp >= cpMac)
		return cpMac;

	if (!fWord)
		{
		while (++cp < cpMac && (fchIgnore & GrfchFromCh(_ChFetch(cp))))
			;
		}
	else
		{
		while (cp < cpMac &&
				!((fchIgnore | fchMayBreak) & GrfchFromCh(_ChFetch(cp))))
			{
			cp++;
			}
		while (cp < cpMac &&
				((fchIgnore | fchMayBreak) & GrfchFromCh(_ChFetch(cp))))
			{
			cp++;
			}
		}
	return cp;
}


/***************************************************************************
	Invalidate all DDGs on this text doc. Also dirties the document.
	Should be called by any code that edits the document.
***************************************************************************/
void TXTB::InvalAllDdg(long cp, long ccpIns, long ccpDel, ulong grfdoc)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac() + 1);
	AssertIn(ccpIns, 0, CpMac() + 1 - cp);
	AssertIn(ccpDel, 0, kcbMax);
	long ipddg;
	PDDG pddg;

	//mark the document dirty
	SetDirty();
	if (fdocNil == grfdoc)
		return;

	//inform the DDGs
	for (ipddg = 0; pvNil != (pddg = PddgGet(ipddg)); ipddg++)
		{
		if ((grfdoc & fdocUpdate) && pddg->FIs(kclsTXTG))
			((PTXTG)pddg)->InvalCp(cp, ccpIns, ccpDel);
		else
			pddg->InvalRc(pvNil);
		}
}


/***************************************************************************
	Set the background color of the document.
***************************************************************************/
void TXTB::SetAcrBack(ACR acr, ulong grfdoc)
{
	AssertThis(0);
	AssertPo(&acr, 0);
	Assert(acr != kacrInvert, "can't use invert as a background color");

	if (_acrBack != acr)
		{
		_acrBack = acr;
		SetDirty();
		if (grfdoc & (fdocUpdate | fdocInval))
			InvalAllDdg(0, 0, 0, fdocInval);
		}
}


/***************************************************************************
	Set the default width of the document.
***************************************************************************/
void TXTB::SetDxpDef(long dxp)
{
	AssertThis(0);
	AssertIn(dxp, 1, kcbMax);
	long cpMac;

	if (_dxpDef == dxp)
		return;

	_dxpDef = dxp;
	SetDirty();
	HideSel();
	cpMac = CpMac();
	InvalAllDdg(0, cpMac, cpMac);
}


/***************************************************************************
	Replace cp to cp + ccpDel with ccpIns characters from prgch. If ccpIns
	is zero, prgch can be nil. The last character should never be replaced.
***************************************************************************/
bool TXTB::FReplaceRgch(void *prgch, long ccpIns, long cp, long ccpDel,
	ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertIn(ccpIns, 0, kcbMax);
	AssertPvCb(prgch, ccpIns * size(achar));
	AssertIn(cp, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cp);

	if (!_pbsf->FReplace(prgch, ccpIns * size(achar),
			cp * size(achar), ccpDel * size(achar)))
		{
		return fFalse;
		}
	_InvalCache(cp, ccpIns, ccpDel);
	AssertThis(fobjAssertFull);
	InvalAllDdg(cp, ccpIns, ccpDel, grfdoc);
	return fTrue;
}


/***************************************************************************
	Replace cp to cp + ccpDel with the characters in the given FLO.
***************************************************************************/
bool TXTB::FReplaceFlo(PFLO pflo, bool fCopy, long cp, long ccpDel,
	short osk, ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertPo(pflo, 0);
	AssertIn(cp, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cp);
	bool fRet = fFalse;
	FLO flo = *pflo;

	flo.pfil->AddRef();
	if (flo.FTranslate(osk) &&
		_pbsf->FReplaceFlo(&flo, fCopy && flo.pfil == pflo->pfil,
			cp * size(achar), ccpDel * size(achar)))
		{
		_InvalCache(cp, flo.cb / size(achar), ccpDel);
		AssertThis(fobjAssertFull);
		InvalAllDdg(cp, flo.cb / size(achar), ccpDel, grfdoc);
		fRet = fTrue;
		}

	ReleasePpo(&flo.pfil);
	return fRet;
}


/***************************************************************************
	Replace cp to cpDst + ccpDel with ccpSrc characters from pbsfSrc starting
	at cpSrc.
***************************************************************************/
bool TXTB::FReplaceBsf(PBSF pbsfSrc, long cpSrc, long ccpSrc,
	long cpDst, long ccpDel, ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertPo(pbsfSrc, 0);
	AssertIn(cpSrc, 0, pbsfSrc->IbMac() / size(achar) + 1);
	AssertIn(ccpSrc, 0, pbsfSrc->IbMac() / size(achar) + 1 - cpSrc);
	AssertIn(cpDst, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cpDst);

	if (!_pbsf->FReplaceBsf(pbsfSrc, cpSrc * size(achar), ccpSrc * size(achar),
			cpDst * size(achar), ccpDel * size(achar)))
		{
		return fFalse;
		}
	_InvalCache(cpDst, ccpSrc, ccpDel);
	AssertThis(fobjAssertFull);
	InvalAllDdg(cpDst, ccpSrc, ccpDel, grfdoc);
	return fTrue;
}


/***************************************************************************
	Replace cp to cpDst + ccpDel with ccpSrc characters from ptxtbSrc starting
	at cpSrc.
***************************************************************************/
bool TXTB::FReplaceTxtb(PTXTB ptxtbSrc, long cpSrc, long ccpSrc,
	long cpDst, long ccpDel, ulong grfdoc)
{
	AssertPo(ptxtbSrc, 0);
	AssertIn(cpSrc, 0, ptxtbSrc->CpMac() + 1);
	AssertIn(ccpSrc, 0, ptxtbSrc->CpMac() + 1 - cpSrc);

	return FReplaceBsf(ptxtbSrc->_pbsf, cpSrc, ccpSrc, cpDst, ccpDel, grfdoc);
}


/***************************************************************************
	Get the bounds of an object - since plain text doesn't have objects,
	just return false.
***************************************************************************/
bool TXTB::FGetObjectRc(long cp, PGNV pgnv, PCHP pchp, RC *prc)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac());
	AssertPo(pgnv, 0);
	AssertVarMem(pchp);
	AssertVarMem(prc);

	TrashVar(prc);
	return fFalse;
}


/***************************************************************************
	Draw an object - since plain text doesn't have objects, just return
	false.
***************************************************************************/
bool TXTB::FDrawObject(long cp, PGNV pgnv, long *pxp, long yp,
	PCHP pchp, RC *prcClip)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac());
	AssertPo(pgnv, 0);
	AssertVarMem(pxp);
	AssertVarMem(pchp);
	AssertVarMem(prcClip);

	return fFalse;
}


/***************************************************************************
	Get the current FNI for the doc. Return false if the doc is not
	currently based on an FNI (it's a new doc or an internal one).
***************************************************************************/
bool TXTB::FGetFni(FNI *pfni)
{
	AssertThis(0);
	AssertBasePo(pfni, 0);

	if (pvNil == _pfil || _pfil->FTemp())
		return fFalse;

	_pfil->GetFni(pfni);
	return fTrue;
}


/***************************************************************************
	Export the text.
***************************************************************************/
void TXTB::ExportFormats(PCLIP pclip)
{
	AssertThis(0);
	AssertPo(pclip, 0);
	void *pv;
	long ccp = CpMac() - 1;

	if (ccp <= 0)
		return;

	if (!pclip->FInitExport())
		return;

	if (pvNil != (pv = pclip->PvExport(ccp * size(achar), kclfmText)))
		_pbsf->FetchRgb(0, ccp * size(achar), pv);

	pclip->EndExport();
}


/***************************************************************************
	Constructor for plain text doc.
***************************************************************************/
TXPD::TXPD(PDOCB pdocb, ulong grfdoc) : TXPD_PAR(pdocb, grfdoc)
{
}


/***************************************************************************
	Static method to create a new plain text document.
***************************************************************************/
PTXPD TXPD::PtxpdNew(PFNI pfni, PBSF pbsf, short osk, PDOCB pdocb, ulong grfdoc)
{
	AssertNilOrPo(pfni, ffniFile);
	AssertNilOrPo(pbsf, 0);
	AssertNilOrPo(pdocb, 0);
	PTXPD ptxpd;

	if (pvNil != (ptxpd = NewObj TXPD(pdocb, grfdoc)) &&
			!ptxpd->_FInit(pfni, pbsf, osk))
		{
		ReleasePpo(&ptxpd);
		}
	return ptxpd;
}


/***************************************************************************
	Create a new TXLG to display the TXPD.
***************************************************************************/
PDDG TXPD::PddgNew(PGCB pgcb)
{
	AssertThis(fobjAssertFull);
	long onn = vpappb->OnnDefFixed();
	long dypFont = vpappb->DypTextDef();

	return TXLG::PtxlgNew(this, pgcb, onn, fontNil, dypFont, 4);
}


/***************************************************************************
	Save the document and optionally set this fni as the current one.
	If the doc is currently based on an FNI, pfni may be nil, indicating
	that this is a normal save (not save as). If pfni is not nil and
	fSetFni is false, this just writes a copy of the doc but doesn't change
	the doc one bit.
***************************************************************************/
bool TXPD::FSaveToFni(FNI *pfni, bool fSetFni)
{
	AssertThis(fobjAssertFull);
	AssertNilOrPo(pfni, ffniFile);
	FLO flo;
	FNI fniT;

	if (pvNil == pfni)
		{
		if (pvNil == _pfil)
			{
			Bug("Can't do a normal save - no file");
			return fFalse;
			}
		_pfil->GetFni(&fniT);
		pfni = &fniT;
		fSetFni = fTrue;
		}

	if (pvNil == (flo.pfil = FIL::PfilCreateTemp(pfni)))
		goto LFail;

	flo.fp = 0;
	flo.cb = _pbsf->IbMac() - size(achar);
#ifdef UNICODE
	flo.cb += size(achar);
	achar ch;

	ch = kchwUnicode;
	if (!flo.FWriteRgb(&ch, size(achar), 0))
		goto LFail;
	flo.fp += size(achar);
	flo.cb -= size(achar);
#endif //UNICODE

	if (!_pbsf->FWriteRgb(&flo))
		goto LFail;

	// redirect the BSF to the new file
	if (fSetFni)
		_pbsf->FReplaceFlo(&flo, fFalse, 0, flo.cb);

	if (!flo.pfil->FSetFni(pfni))
		{
LFail:
		ReleasePpo(&flo.pfil);
		return fFalse;
		}
	flo.pfil->SetTemp(fFalse);

	if (fSetFni)
		{
		ReleasePpo(&_pfil);
		_pfil = flo.pfil;
		_fDirty = fFalse;
		}
	else
		ReleasePpo(&flo.pfil);

	return fTrue;
}


/***************************************************************************
	Constructor for a rich text document.
***************************************************************************/
TXRD::TXRD(PDOCB pdocb, ulong grfdoc) : TXRD_PAR(pdocb, grfdoc)
{
}


/***************************************************************************
	Destructor for a rich text document.
***************************************************************************/
TXRD::~TXRD(void)
{
	ReleasePpo(&_pglmpe);
	ReleasePpo(&_pagcact);
	ReleasePpo(&_pcfl);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a TXRD.
***************************************************************************/
void TXRD::AssertValid(ulong grfobj)
{
	TXRD_PAR::AssertValid(grfobj);
	AssertPo(_pglmpe, 0);
	AssertNilOrPo(_pagcact, 0);
	AssertIn(_dypFontDef, 1, kswMax);
	AssertIn(_pbsf->IbMac() / size(achar), 1, kcpMaxTxrd);

	if (grfobj & fobjAssertFull)
		{
		MPE mpePrev, mpe;
		long impe;

		mpePrev.spcp = (ulong)-1;
		for (impe = _pglmpe->IvMac(); impe-- > 0; )
			{
			_pglmpe->Get(impe, &mpe);
			Assert(mpe.spcp < mpePrev.spcp, "non-increasing mpe's");
			mpePrev = mpe;
			}
		//REVIEW shonk: TXRD::AssertValid: fill out
		}
}


/***************************************************************************
	Mark memory for the TXRD.
***************************************************************************/
void TXRD::MarkMem(void)
{
	AssertThis(fobjAssertFull);
	TXRD_PAR::MarkMem();
	MarkMemObj(_pglmpe);
	MarkMemObj(_pagcact);
}
#endif //DEBUG


/***************************************************************************
	Static method to create or open a rich text document.
***************************************************************************/
PTXRD TXRD::PtxrdNew(PFNI pfni)
{
	AssertNilOrPo(pfni, ffniFile);
	PTXRD ptxrd;

	if (pvNil != (ptxrd = NewObj TXRD) && !ptxrd->_FInit(pfni))
		ReleasePpo(&ptxrd);
	return ptxrd;
}


/***************************************************************************
	Initialize the rich text document.
***************************************************************************/
bool TXRD::_FInit(PFNI pfni, CTG ctg)
{
	AssertBaseThis(0);
	AssertNilOrPo(pfni, 0);

	if (pvNil != pfni)
		{
		PCFL pcfl;
		CKI cki;

		if (pvNil == (pcfl = CFL::PcflOpen(pfni, fcflNil)))
			{
			PushErc(ercRtxdReadFailed);
			return fFalse;
			}
		if (!pcfl->FGetCkiCtg(ctg, 0, &cki) ||
				!_FReadChunk(pcfl, cki.ctg, cki.cno, fFalse))
			{
			ReleasePpo(&pcfl);
			PushErc(ercRtxdReadFailed);
			return fFalse;
			}
		_pcfl = pcfl;
		AssertThis(fobjAssertFull);
		return fTrue;
		}

	if (!TXRD_PAR::_FInit())
		return fFalse;
	if (pvNil == (_pglmpe = GL::PglNew(size(MPE))))
		return fFalse;
	_pglmpe->SetMinGrow(10);
	_onnDef = vpappb->OnnDefVariable();
	_oskFont = koskCur;
	vntl.GetStn(_onnDef, &_stnFontDef);

	// NOTE: don't use vpappb->DypTextDef() so all TXRD's will have the
	// same _dypFontDef unless explicitly changed in code.
	_dypFontDef = 12;

	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Static method to read a rich text document from a chunk.
***************************************************************************/
PTXRD TXRD::PtxrdReadChunk(PCFL pcfl, CTG ctg, CNO cno, bool fCopyText)
{
	AssertPo(pcfl, 0);
	PTXRD ptxrd;

	if (pvNil != (ptxrd = NewObj TXRD) &&
			!ptxrd->_FReadChunk(pcfl, ctg, cno, fCopyText))
		{
		PushErc(ercRtxdReadFailed);
		ReleasePpo(&ptxrd);
		}
	return ptxrd;
}


/***************************************************************************
	Read the given chunk into this TXRD.
***************************************************************************/
bool TXRD::_FReadChunk(PCFL pcfl, CTG ctg, CNO cno, bool fCopyText)
{
	AssertPo(pcfl, 0);
	BLCK blck;
	FLO floText;
	PFIL pfilT;
	KID kid;
	long icact;
	long cact;
	RDOP rdop;
	short bo, osk;

	if (!TXRD_PAR::_FInit())
		return fFalse;

	if (!pcfl->FFind(ctg, cno, &blck) || !blck.FUnpackData() ||
			blck.Cb() < size(RDOP) || !blck.FReadRgb(&rdop, size(RDOP), 0))
		{
		return fFalse;
		}

	if (rdop.bo == kboOther)
		SwapBytesBom(&rdop, kbomRdop);
	else if (rdop.bo != kboCur)
		return fFalse;

	// do a sanity check on the default font size and width
	if (!FIn(rdop.dypFont, 4, 256) || !FIn(rdop.dxpDef, 1, kcbMax))
		{
		Bug("bad default font size");
		return fFalse;
		}
	_dxpDef = rdop.dxpDef;
	_dypFontDef = rdop.dypFont;
	_acrBack.SetFromLw(rdop.lwAcrBack);

	if (_stnFontDef.FRead(&blck, size(rdop)) && _stnFontDef.Cch() > 0)
		_oskFont = rdop.oskFont;
	else
		{
		_oskFont = koskCur;
		_stnFontDef.SetNil();
		}
	_onnDef = vntl.OnnMapStn(&_stnFontDef, _oskFont);

	// get the text
	if (!pcfl->FGetKidChidCtg(ctg, cno, 0, kctgText, &kid) ||
			!pcfl->FFindFlo(kid.cki.ctg, kid.cki.cno, &floText) ||
			floText.cb < size(short) || !floText.FReadRgb(&osk, size(short), 0))
		{
		return fFalse;
		}
	floText.fp += size(short);
	floText.cb -= size(short);
	floText.pfil->AddRef();
	pfilT = floText.pfil;
	if (!floText.FTranslate(osk) ||
		!_pbsf->FReplaceFlo(&floText, fCopyText && pfilT == floText.pfil,
			0, CpMac() - 1))
		{
		ReleasePpo(&floText.pfil);
		return fFalse;
		}
	ReleasePpo(&floText.pfil);

	// get the text properties
	if (!pcfl->FGetKidChidCtg(ctg, cno, 0, kctgTxtProps, &kid) ||
			!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck) ||
			pvNil == (_pglmpe = GL::PglRead(&blck, &bo)) ||
			size(MPE) != _pglmpe->CbEntry())
		{
		return fFalse;
		}
	if (bo == kboOther)
		{
		SwapBytesRglw(_pglmpe->QvGet(0),
			LwMul(_pglmpe->IvMac(), size(MPE)) / size(long));
		}

	// get the text property arguments
	if (pcfl->FGetKidChidCtg(ctg, cno, 0, kctgTxtPropArgs, &kid))
		{
		if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck) ||
				pvNil == (_pagcact = AG::PagRead(&blck, &bo, &osk)) ||
				size(long) != _pagcact->CbFixed())
			{
			return fFalse;
			}
		for (icact = _pagcact->IvMac(); icact-- > 0; )
			{
			if (_pagcact->FFree(icact))
				continue;
			_pagcact->GetFixed(icact, &cact);
			if (bo == kboOther)
				{
				SwapBytesRglw(&cact, 1);
				_pagcact->PutFixed(icact, &cact);
				}
			if (!_FOpenArg(icact, B3Lw(cact), bo, osk))
				return fFalse;
			}
		}

	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Do any necessary munging of the AG entry on open. Return false if
	we don't recognize this argument type.
***************************************************************************/
bool TXRD::_FOpenArg(long icact, byte sprm, short bo, short osk)
{
	AssertBaseThis(0);
	long onn, cb;
	byte *prgb;
	STN stn;

	cb = _pagcact->Cb(icact);
	switch (sprm)
		{
	case sprmFont:
		cb -= size(long) + size(short); //onn, osk
		if (cb < 0)
			{
			Bug("bad font entry");
			return fFalse;
			}
		_pagcact->GetRgb(icact, size(long), size(short), &osk);
		prgb = (byte *)PvAddBv(_pagcact->PvLock(icact), size(long) + size(short));
		if (!stn.FSetData(prgb, cb))
			{
			Bug("bad font entry");
			_pagcact->Unlock();
			return fFalse;
			}
		_pagcact->Unlock();

		onn = vntl.OnnMapStn(&stn, osk);
		_pagcact->PutRgb(icact, 0, size(long), &onn);
		break;

	default:
		return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	Get the current FNI for the doc. Return false if the doc is not
	currently based on an FNI (it's a new doc or an internal one).
***************************************************************************/
bool TXRD::FGetFni(FNI *pfni)
{
	AssertThis(0);
	AssertBasePo(pfni, 0);

	if (pvNil == _pcfl)
		return fFalse;

	_pcfl->GetFni(pfni);
	return fTrue;
}


/***************************************************************************
	Ask the user what file they want to save to.
***************************************************************************/
bool TXRD::FGetFniSave(FNI *pfni)
{
	//REVIEW shonk: what file type to use on Mac?
	AssertThis(0);
	return FGetFniSaveMacro(pfni, 'CHN2', "\x9" "Save As: ", "",
		PszLit("All files\0*.*\0"), vwig.hwndApp);
}


/***************************************************************************
	Save the rich text document and optionally set this fni as the current
	one. If the doc is currently based on an FNI, pfni may be nil, indicating
	that this is a normal save (not save as). If pfni is not nil and
	fSetFni is false, this just writes a copy of the doc but doesn't change
	the doc one bit.
***************************************************************************/
bool TXRD::FSaveToFni(FNI *pfni, bool fSetFni)
{
	AssertThis(fobjAssertFull);
	AssertNilOrPo(pfni, ffniFile);
	FNI fni;
	PCFL pcfl;
	CKI cki;

	if (pvNil == pfni)
		{
		if (pvNil == _pcfl)
			{
			Bug("Can't do a normal save - no file");
			return fFalse;
			}
		fSetFni = fTrue;
		_pcfl->GetFni(&fni);
		pfni = &fni;
		}

	if (pvNil == (pcfl = CFL::PcflCreateTemp(pfni)))
		goto LFail;

	if (!FSaveToChunk(pcfl, &cki, fSetFni) || !pcfl->FSave(kctgFramework, pvNil))
		goto LFail;

	if (!pcfl->FSetFni(pfni))
		{
LFail:
		ReleasePpo(&pcfl);
		PushErc(ercRtxdSaveFailed);
		return fFalse;
		}
	pcfl->SetTemp(fFalse);

	if (fSetFni)
		{
		ReleasePpo(&_pcfl);
		_pcfl = pcfl;
		_fDirty = fFalse;
		}
	else
		ReleasePpo(&pcfl);

	return fTrue;
}


/***************************************************************************
	Save a rich text document to the given chunky file. Fill in *pcki with
	where we put the root chunk.
***************************************************************************/
bool TXRD::FSaveToChunk(PCFL pcfl, CKI *pcki, bool fRedirectText)
{
	AssertThis(fobjAssertFull);
	AssertPo(pcfl, 0);
	AssertVarMem(pcki);
	RDOP rdop;
	BLCK blck, blckText;
	CNO cno, cnoText;
	long cb;
	short osk = koskCur;

	pcki->ctg = kctgRichText;
	rdop.bo = kboCur;
	rdop.oskFont = _oskFont;
	rdop.dxpDef = _dxpDef;
	rdop.dypFont = _dypFontDef;
	rdop.lwAcrBack = _acrBack.LwGet();
	cb = _stnFontDef.CbData();
	if (!pcfl->FAdd(size(RDOP) + cb, pcki->ctg, &pcki->cno, &blck))
		{
		PushErc(ercRtxdSaveFailed);
		return fFalse;
		}
	if (!blck.FWriteRgb(&rdop, size(RDOP), 0) ||
			!_stnFontDef.FWrite(&blck, size(RDOP)))
		{
		goto LFail;
		}

	//add the text chunk and write it
	if (!pcfl->FAddChild(pcki->ctg, pcki->cno, 0,
			_pbsf->IbMac() - size(achar) + size(short), kctgText, &cnoText,
			&blckText) ||
		!blckText.FWriteRgb(&osk, size(short), 0))
		{
		goto LFail;
		}
	AssertDo(blckText.FMoveMin(size(short)), 0);
	if (!_pbsf->FWriteRgb(&blckText))
		goto LFail;

	cb = _pglmpe->CbOnFile();
	if (!pcfl->FAddChild(pcki->ctg, pcki->cno, 0, cb, kctgTxtProps, &cno, &blck) ||
			!_pglmpe->FWrite(&blck))
		{
		goto LFail;
		}

	if (pvNil != _pagcact && 0 < _pagcact->IvMac())
		{
		cb = _pagcact->CbOnFile();
		if (!pcfl->FAddChild(pcki->ctg, pcki->cno, 0, cb,
				kctgTxtPropArgs, &cno, &blck) ||
			!_pagcact->FWrite(&blck))
			{
			goto LFail;
			}
		}

	if (fRedirectText)
		{
		FLO flo;
		AssertDo(blckText.FGetFlo(&flo), 0);
		_pbsf->FReplaceFlo(&flo, fFalse, 0, _pbsf->IbMac() - size(achar));
		ReleasePpo(&flo.pfil);
		}
	return fTrue;

LFail:
	pcfl->Delete(pcki->ctg, pcki->cno);
	PushErc(ercRtxdSaveFailed);
	return fFalse;
}


/***************************************************************************
	Create a new TXRG to display the TXRD.
***************************************************************************/
PDDG TXRD::PddgNew(PGCB pgcb)
{
	AssertThis(fobjAssertFull);
	return TXRG::PtxrgNew(this, pgcb);
}


/***************************************************************************
	Look for an MPE for the given spcp. Return false iff there isn't one.
	In either event, fill pimpe with where one would go if it did exist.
***************************************************************************/
bool TXRD::_FFindMpe(ulong spcp, MPE *pmpe, long *pcpLim, long *pimpe)
{
	AssertThis(0);
	AssertNilOrVarMem(pmpe);
	AssertNilOrVarMem(pcpLim);
	AssertNilOrVarMem(pimpe);
	long impe, impeMin, impeLim;
	bool fRet;
	MPE mpe;
	byte sprm = _SprmFromSpcp(spcp);

	for (impeMin = 0, impeLim = _pglmpe->IvMac(); impeMin < impeLim; )
		{
		impe = (impeMin + impeLim) / 2;
		_pglmpe->Get(impe, &mpe);
		if (mpe.spcp < spcp)
			impeMin = impe + 1;
		else if (mpe.spcp > spcp)
			impeLim = impe;
		else
			{
			impeMin = impe + 1;
			goto LSuccess;
			}
		}

	//assume there isn't one
	if (pvNil != pimpe)
		*pimpe = impeMin;
	TrashVar(pmpe);
	fRet = fFalse;

	if (impeMin > 0)
		{
		_pglmpe->Get(impeMin - 1, &mpe);
		if (_SprmFromSpcp(mpe.spcp) == sprm)
			{
LSuccess:
			if (pvNil != pimpe)
				*pimpe = impeMin - 1;
			if (pvNil != pmpe)
				*pmpe = mpe;
			fRet = fTrue;
			}
		else
			Assert(_SprmFromSpcp(mpe.spcp) < sprm, 0);
		}

	if (pvNil != pcpLim)
		{
		*pcpLim = CpMac();
		if (impeMin < _pglmpe->IvMac())
			{
			_pglmpe->Get(impeMin, &mpe);
			if (_SprmFromSpcp(mpe.spcp) == sprm)
				*pcpLim = _CpFromSpcp(mpe.spcp);
			}
		}
	return fRet;
}


/***************************************************************************
	Fetch the impe'th property, returning all the relevant info about it.
***************************************************************************/
bool TXRD::_FFetchProp(long impe, byte *psprm, long *plw,
	long *pcpMin, long *pcpLim)
{
	MPE mpe;

	if (!FIn(impe, 0, _pglmpe->IvMac()))
		return fFalse;

	_pglmpe->Get(impe, &mpe);
	if (pvNil != psprm)
		*psprm = _SprmFromSpcp(mpe.spcp);
	if (pvNil != plw)
		*plw = mpe.lw;
	if (pvNil != pcpMin)
		*pcpMin = _CpFromSpcp(mpe.spcp);
	if (pvNil != pcpLim)
		{
		*pcpLim = CpMac();
		if (impe + 1 < _pglmpe->IvMac())
			{
			MPE mpeNext;

			_pglmpe->Get(impe + 1, &mpeNext);
			if (_SprmFromSpcp(mpeNext.spcp) == _SprmFromSpcp(mpe.spcp))
				*pcpLim = _CpFromSpcp(mpeNext.spcp);
			}
		}
	return fTrue;
}


/***************************************************************************
	Adjust the MPE's after an edit. This may involve deleting some MPE's
	and/or updating the cp's.
***************************************************************************/
void TXRD::_AdjustMpe(long cp, long ccpIns, long ccpDel)
{
	AssertBaseThis(0);
	AssertIn(cp, 0, CpMac());
	AssertIn(ccpIns, 0, CpMac() - cp);
	AssertIn(ccpDel, 0, kcbMax);
	MPE *qmpe;
	long impe, impeMin, cpT;
	byte sprm;
	bool fBefore, fKeep;

	_cpMinChp = _cpLimChp = _cpMinPap = _cpLimPap = 0;
	for (impe = 0; impe < _pglmpe->IvMac(); )
		{
		qmpe = (MPE *)_pglmpe->QvGet(impe);
		cpT = _CpFromSpcp(qmpe->spcp);
		AssertIn(cpT, 0, kcpMaxTxrd);
		if (cpT < cp)
			{
			impe++;
			continue;
			}
		sprm = _SprmFromSpcp(qmpe->spcp);
		if (cpT >= cp + ccpDel + (sprm < sprmMinObj))
			{
			AssertIn(cpT + ccpIns - ccpDel, 0, kcpMaxTxrd);
			qmpe->spcp = _SpcpFromSprmCp(sprm, cpT + ccpIns - ccpDel);
			impe++;
			continue;
			}
		AssertIn(cpT, cp, cp + ccpDel + (sprm < sprmMinObj));

		// special case object sprms
		if (sprm >= sprmMinObj)
			{
			// delete it
			AssertIn(cpT, cp, cp + ccpDel);
			_ReleaseSprmLw(sprm, qmpe->lw);
			_pglmpe->Delete(impe);
			continue;
			}

		//fBefore indicates if the last MPE for this sprm should go at cp
		//or at the beginning of the paragraph at or after cp + ccpIns.
		//This is only used for paragraph sprms.
		fBefore = (cpT == cp);

		impeMin = impe;
		while (cpT <= cp + ccpDel)
			{
			if (++impe >= _pglmpe->IvMac())
				break;
			qmpe = (MPE *)_pglmpe->QvGet(impe);
			if (_SprmFromSpcp(qmpe->spcp) != sprm)
				break;
			cpT = _CpFromSpcp(qmpe->spcp);
			AssertIn(cpT, 0, kcpMaxTxrd);
			}

		//the entries from impeMin to impe are all in or
		//at the end of the deleted zone
		Assert(impe > impeMin, "no MPE's!");

		//fKeep indicates whether we kept the last MPE
		fKeep = fTrue;
		if (sprm < sprmMinPap)
			{
			// a character sprm - put the last one at cp + ccpIns
			AssertIn(cp + ccpIns, 0, CpMac());
			qmpe = (MPE *)_pglmpe->QvGet(--impe);
			qmpe->spcp = _SpcpFromSprmCp(sprm, cp + ccpIns);
			}
		else if (fBefore)
			{
			// a paragraph sprm - the last one goes at cp
			AssertIn(cp, 0, kcpMaxTxrd);
			qmpe = (MPE *)_pglmpe->QvGet(--impe);
			qmpe->spcp = _SpcpFromSprmCp(sprm, cp);
			}
		else
			{
			// a paragraph sprm - the last MPE goes at the beginning of the
			// next paragraph (if there is one and it doesn't already have an MPE).
			cpT = CpLimPara(cp + ccpIns - 1);
			if (cpT < CpMac() && (impe >= _pglmpe->IvMac() ||
					_SprmFromSpcp((qmpe = (MPE *)_pglmpe->QvGet(impe))->spcp) != sprm ||
					_CpFromSpcp(qmpe->spcp) + ccpIns - ccpDel > cpT))
				{
				//put the last one at cpT - if there isn't already one
				AssertIn(cpT, 0, kcpMaxTxrd);
				qmpe = (MPE *)_pglmpe->QvGet(--impe);
				qmpe->spcp = _SpcpFromSprmCp(sprm, cpT);
				}
			else
				fKeep = fFalse;
			}

		// delete all the previous ones
		while (impeMin < impe)
			{
			qmpe = (MPE *)_pglmpe->QvGet(--impe);
			_ReleaseSprmLw(sprm, qmpe->lw);
			_pglmpe->Delete(impe);
			}

		// skip the one we kept at the end
		if (fKeep)
			impe++;
		}
	AssertThis(0);
}


/***************************************************************************
	Make sure there is an entry in the AG with the given data. If it's
	already there, increment its reference count. Otherwise, set its
	reference count to 1.
***************************************************************************/
bool TXRD::_FEnsureInAg(byte sprm, void *pv, long cb, long *pjv)
{
	AssertIn(cb, 1, kcbMax);
	AssertPvCb(pv, cb);
	AssertVarMem(pjv);
	long cact, iv, cbT;
	void *qv;

	if (pvNil == _pagcact && pvNil == (_pagcact = AG::PagNew(size(long), 1, cb)))
		{
		TrashVar(pjv);
		return fFalse;
		}

	//see if it's already there
	for (iv = _pagcact->IvMac(); iv-- > 0; )
		{
		if (_pagcact->FFree(iv))
			continue;
		qv = _pagcact->QvGet(iv, &cbT);
		if (cb != cbT)
			continue;
		if (FEqualRgb(qv, pv, cb))
			{
			_pagcact->GetFixed(iv, &cact);
			if (B3Lw(cact) == sprm)
				{
				*pjv = iv + 1;
				AssertIn(cact & 0x00FFFFFFL, 1, 0x00FFFFFFL);
				cact++;
				Assert(B3Lw(cact) == sprm, "cact overflowed");
				_pagcact->PutFixed(iv, &cact);
				return fTrue;
				}
			}
		}

	//need to add it
	cact = (((ulong)sprm) << 24) | 1;
	if (!_pagcact->FAdd(cb, pjv, pv, &cact))
		{
		TrashVar(pjv);
		return fFalse;
		}
	(*pjv)++;
	return fTrue;
}


/***************************************************************************
	Decrement the reference count on the given element of the AG and if
	the reference count becomes zero, delete the element.
***************************************************************************/
void TXRD::_ReleaseInAg(long jv)
{
	long cact, cactT;

	if (pvNil == _pagcact || !FIn(jv, 1, _pagcact->IvMac() + 1) ||
			_pagcact->FFree(jv - 1))
		{
		Bug("bad index into AG");
		return;
		}

	_pagcact->GetFixed(jv - 1, &cact);
	cactT = cact & 0x00FFFFFFL;
	if (--cactT <= 0)
		_pagcact->Delete(jv - 1);
	else
		{
		cact--;
		_pagcact->PutFixed(jv - 1, &cact);
		}
}


/***************************************************************************
	Increment the reference count on the given element of the AG.
***************************************************************************/
void TXRD::_AddRefInAg(long jv)
{
	long cact;

	if (pvNil == _pagcact || !FIn(jv, 1, _pagcact->IvMac() + 1) ||
			_pagcact->FFree(jv - 1))
		{
		Bug("bad index into AG");
		return;
		}

	_pagcact->GetFixed(jv - 1, &cact);
	AssertIn(cact & 0x00FFFFFFL, 1, 0x00FFFFFFL);
	++cact;
	_pagcact->PutFixed(jv - 1, &cact);
}


/***************************************************************************
	Static method: return true iff the given sprm has its argument in
	the _pagcact.
***************************************************************************/
bool TXRD::_FSprmInAg(byte sprm)
{
	if (sprm < sprmMinChpClient || FIn(sprm, sprmMinPap, sprmMinPapClient))
		{
		// a base TXRD sprm
		return sprm == sprmFont;
		}

	// The sprm is a client or object sprm. Even ones are in the AG, odd
	// ones aren't. See note in rtxt.h where the sprms are defined.
	return !(sprm & 1);
}


/***************************************************************************
	If the sprm allocates stuff in the ag, release it. The inverse operation
	of _AddRefSprmLw.
***************************************************************************/
void TXRD::_ReleaseSprmLw(byte sprm, long lw)
{
	if (lw > 0 && _FSprmInAg(sprm))
		_ReleaseInAg(lw);
}


/***************************************************************************
	If the sprm allocates stuff in the ag, addref it. The inverse operation
	of _ReleaseSprmLw.
***************************************************************************/
void TXRD::_AddRefSprmLw(byte sprm, long lw)
{
	if (lw > 0 && _FSprmInAg(sprm))
		_AddRefInAg(lw);
}


/***************************************************************************
	Get an array of SPVM's that describe the differences between pchp and
	pchpDiff. If pchpDiff is nil, pchp is just described. If this
	succeeds, either _ApplyRgspvm or _ReleaseRgspvm should be called.
***************************************************************************/
bool TXRD::_FGetRgspvmFromChp(PCHP pchp, PCHP pchpDiff,
	SPVM *prgspvm, long *pcspvm)
{
	AssertVarMem(pchp);
	AssertNilOrVarMem(pchpDiff);
	AssertPvCb(prgspvm, LwMul(size(SPVM), sprmLimChp - sprmMinChp));
	AssertVarMem(pcspvm);
	byte sprm;
	long ispvm;
	SPVM spvm;

	//first get the values and masks for the sprm's we have to deal with
	ispvm = 0;
	for (sprm = sprmMinChp; sprm < sprmLimChp; sprm++)
		{
		switch (_TGetLwFromChp(sprm, pchp, pchpDiff, &spvm.lw, &spvm.lwMask))
			{
		case tYes:
			spvm.sprm = sprm;
			prgspvm[ispvm++] = spvm;
			break;
		case tMaybe:
			_ReleaseRgspvm(prgspvm, ispvm);
			TrashVar(pcspvm);
			return fFalse;
			}
		}

	*pcspvm = ispvm;
	return fTrue;
}


/***************************************************************************
	Get an array of SPVM's that describe the differences between ppap and
	ppapDiff. If ppapDiff is nil, ppap is just described. If this
	succeeds, either _ApplyRgspvm or _ReleaseRgspvm should be called.
***************************************************************************/
bool TXRD::_FGetRgspvmFromPap(PPAP ppap, PPAP ppapDiff,
	SPVM *prgspvm, long *pcspvm)
{
	AssertVarMem(ppap);
	AssertNilOrVarMem(ppapDiff);
	AssertPvCb(prgspvm, LwMul(size(SPVM), sprmLimPap - sprmMinPap));
	AssertVarMem(pcspvm);
	byte sprm;
	long ispvm;
	SPVM spvm;

	//first get the values and masks for the sprm's we have to deal with
	ispvm = 0;
	for (sprm = sprmMinPap; sprm < sprmLimPap; sprm++)
		{
		switch (_TGetLwFromPap(sprm, ppap, ppapDiff, &spvm.lw, &spvm.lwMask))
			{
		case tYes:
			spvm.sprm = sprm;
			prgspvm[ispvm++] = spvm;
			break;
		case tMaybe:
			_ReleaseRgspvm(prgspvm, ispvm);
			TrashVar(pcspvm);
			return fFalse;
			}
		}

	*pcspvm = ispvm;
	return fTrue;
}


/***************************************************************************
	Release the SPVM's.
***************************************************************************/
void TXRD::_ReleaseRgspvm(SPVM *prgspvm, long cspvm)
{
	AssertIn(cspvm, 0, kcbMax);
	AssertPvCb(prgspvm, LwMul(size(SPVM), cspvm));
	long ispvm;

	for (ispvm = 0; ispvm < cspvm; ispvm++)
		_ReleaseSprmLw(prgspvm[ispvm].sprm, prgspvm[ispvm].lw);
}


/***************************************************************************
	Apply the SPVM's to the given range of characters. Assumes that the
	inserts can't fail. If you call this, don't also call _ReleaseRgspvm.
	The caller should have ensured that _pglmpe has room for 2 * cspvm
	additional entries.
***************************************************************************/
void TXRD::_ApplyRgspvm(long cp, long ccp, SPVM *prgspvm, long cspvm)
{
	AssertIn(cp, 0, CpMac());
	AssertIn(ccp, 1, CpMac() + 1 - cp);
	MPE mpe, mpeT;
	long lwRevert, lwLast;
	long cpLimT, cpLim;
	long ispvm, impe;
	SPVM spvm;

	_cpMinChp = _cpLimChp = _cpMinPap = _cpLimPap = 0;
	cpLim = LwMin(cp + ccp, CpMac());
	for (ispvm = cspvm; ispvm-- > 0; )
		{
		spvm = prgspvm[ispvm];
		mpe.spcp = _SpcpFromSprmCp(spvm.sprm, cp);
		mpe.lw = spvm.lw;

		if (!_FFindMpe(mpe.spcp, &mpeT, pvNil, &impe))
			{
			//there are no MPE's for this sprm at or before this cp
			lwRevert = 0;
			if (mpe.lw != 0)
				{
				//add the mpe
				_AddRefSprmLw(spvm.sprm, mpe.lw);
				AssertDo(_pglmpe->FInsert(impe++, &mpe), 0);
				}
			lwLast = mpe.lw;
			}
		else
			{
			//see if the MPE we're in the range of starts before this cp
			//or at it
			lwRevert = mpeT.lw;
			if (_CpFromSpcp(mpeT.spcp) < cp)
				{
				//before
				impe++;
				if (mpe.lw != mpeT.lw)
					{
					//add an mpe
					mpeT.lw = mpe.lw | (mpeT.lw & ~spvm.lwMask);
					mpeT.spcp = mpe.spcp;
					_AddRefSprmLw(spvm.sprm, mpeT.lw);
					AssertDo(_pglmpe->FInsert(impe++, &mpeT), 0);
					}
				lwLast = mpeT.lw;
				}
			else
				{
				//at it - get the valid lwLast value
				Assert(_CpFromSpcp(mpeT.spcp) == cp, 0);
				lwLast = 0;
				if (impe > 0)
					{
					_pglmpe->Get(impe - 1, &mpeT);
					if (_SprmFromSpcp(mpeT.spcp) == spvm.sprm)
						lwLast = mpeT.lw;
					}
				}
			}
		_AddRefSprmLw(spvm.sprm, lwRevert);

		//adjust all mpe's before cpLim
		for (;;)
			{
			if (impe >= _pglmpe->IvMac())
				goto LEndOfSprm;
			_pglmpe->Get(impe, &mpeT);
			if (_SprmFromSpcp(mpeT.spcp) != spvm.sprm)
				{
LEndOfSprm:
				cpLimT = CpMac();
				break;
				}
			if ((cpLimT = _CpFromSpcp(mpeT.spcp)) >= cpLim)
				{
				//see if this MPE can be deleted
				if (cpLimT == cpLim && lwLast == mpeT.lw)
					{
					_ReleaseSprmLw(spvm.sprm, mpeT.lw);
					_pglmpe->Delete(impe);
					}
				break;
				}

			//modify the mpeT
			_ReleaseSprmLw(spvm.sprm, lwRevert);
			lwRevert = mpeT.lw;
			mpeT.lw = mpe.lw | (mpeT.lw & ~spvm.lwMask);
			//note: the ref count for this MPE was transferred to lwRevert,
			//so a _ReleaseSprmLw call is not needed after the Delete and Put
			//below
			if (lwLast == mpeT.lw)
				_pglmpe->Delete(impe);
			else
				{
				_AddRefSprmLw(spvm.sprm, mpeT.lw);
				_pglmpe->Put(impe++, &mpeT);
				lwLast = mpeT.lw;
				}
			}

		Assert(cpLimT >= cpLim, 0);
		if (cpLimT > cpLim && lwRevert != lwLast)
			{
			//add another mpe at cpLim with lwRevert
			mpeT.spcp = _SpcpFromSprmCp(spvm.sprm, cpLim);
			mpeT.lw = lwRevert;
			AssertDo(_pglmpe->FInsert(impe, &mpeT), 0);
			}
		else
			_ReleaseSprmLw(spvm.sprm, lwRevert);
		_ReleaseSprmLw(spvm.sprm, spvm.lw);
		}
}


/***************************************************************************
	Get the long associated with the sprm and the difference between the
	two CHPs. pchpOld may be nil. *plwMask indicates which portion of
	the lw is significant. Returns tYes or tNo depending on whether the
	values of pchpOld and pchpNew differ for the given sprm. Returns
	tMaybe on error.
***************************************************************************/
bool TXRD::_TGetLwFromChp(byte sprm, PCHP pchpNew, PCHP pchpOld,
	long *plw, long *plwMask)
{
	AssertIn(sprm, sprmMinChp, sprmLimChp);
	AssertVarMem(pchpNew);
	AssertNilOrVarMem(pchpOld);
	AssertVarMem(plw);
	AssertVarMem(plwMask);

	*plwMask = -1;
	switch (sprm)
		{
	default:
		*plwMask = 0;
		break;

	case sprmStyle:
		*plw = LwHighLow((short)(pchpNew->dypFont - _dypFontDef),
			SwHighLow((byte)pchpNew->dypOffset, (byte)pchpNew->grfont));
		if (pvNil != pchpOld)
			{
			*plwMask = LwHighLow(-(pchpOld->dypFont != pchpNew->dypFont),
				SwHighLow(-(pchpNew->dypOffset != pchpOld->dypOffset),
					(byte)(pchpOld->grfont ^ pchpNew->grfont)));
			}
		break;

	case sprmFont:
		if (pvNil != pchpOld && pchpOld->onn == pchpNew->onn)
			*plwMask = 0;
		else if (pchpNew->onn == _onnDef)
			*plw = 0;
		else
			{
			// data for sprmFont is: long onn, short osk, stn data
			STN stn;
			long cb;
			short osk = koskCur;
			byte rgb[kcbMaxDataStn + size(long) + size(short)];

			CopyPb(&pchpNew->onn, rgb, size(long));
			CopyPb(&osk, rgb + size(long), size(short));
			vntl.GetStn(pchpNew->onn, &stn);
			cb = size(long) + size(short);
			stn.GetData(rgb + cb);
			cb += stn.CbData();
			if (!_FEnsureInAg(sprm, rgb, cb, plw))
				{
				*plwMask = 0;
				TrashVar(plw);
				return tMaybe;
				}
			}
		break;

	case sprmForeColor:
		if (pvNil != pchpOld && pchpOld->acrFore == pchpNew->acrFore)
			*plwMask = 0;
		else
			*plw = pchpNew->acrFore.LwGet() - kacrBlack.LwGet();
		break;

	case sprmBackColor:
		if (pvNil != pchpOld && pchpOld->acrBack == pchpNew->acrBack)
			*plwMask = 0;
		else
			*plw = pchpNew->acrBack.LwGet() - kacrClear.LwGet();
		break;
		}

	if (0 != *plwMask)
		{
		*plw &= *plwMask;
		return tYes;
		}
	TrashVar(plw);
	return tNo;
}


/***************************************************************************
	Get the character properties for the given character.
***************************************************************************/
void TXRD::FetchChp(long cp, PCHP pchp, long *pcpMin, long *pcpLim)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac());
	AssertVarMem(pchp);
	AssertNilOrVarMem(pcpMin);
	AssertNilOrVarMem(pcpLim);
	MPE mpe;
	byte sprm;
	ulong spcp;
	long cpLimT;
	long cb;
	bool fRet;

	if (FIn(cp, _cpMinChp, _cpLimChp))
		goto LDone;

	_chp.Clear();
	_chp.onn = _onnDef;
	_chp.dypFont = _dypFontDef;
	_chp.acrFore = kacrBlack;
	_chp.acrBack = kacrClear;

	_cpLimChp = CpMac();
	_cpMinChp = 0;
	for (sprm = sprmMinChp; sprm < sprmLimChp; sprm++)
		{
		spcp = _SpcpFromSprmCp(sprm, cp);
		fRet = _FFindMpe(spcp, &mpe, &cpLimT);
		_cpLimChp = LwMin(_cpLimChp, cpLimT);
		if (!fRet)
			continue;
		_cpMinChp = LwMax(_cpMinChp, _CpFromSpcp(mpe.spcp));

		switch (sprm)
			{
		case sprmStyle:
			_chp.grfont = B0Lw(mpe.lw);
			_chp.dypOffset = (long)(achar)B1Lw(mpe.lw);
			_chp.dypFont += SwHigh(mpe.lw);
			break;

		case sprmFont:
			if (mpe.lw > 0)
				{
				byte *qrgb;

				qrgb = (byte *)_pagcact->QvGet(mpe.lw - 1, &cb);
				cb -= size(long) + size(short); //onn, osk
				if (!FIn(cb, 0, kcbMaxDataStn + 1))
					{
					Warn("bad group element");
					break;
					}
				CopyPb(qrgb, &_chp.onn, size(long));
				Assert(vntl.FValidOnn(_chp.onn), "invalid onn");
				}
			break;

		case sprmForeColor:
			if (mpe.lw != 0)
				_chp.acrFore.SetFromLw(kacrBlack.LwGet() + mpe.lw);
			break;

		case sprmBackColor:
			if (mpe.lw != 0)
				_chp.acrBack.SetFromLw(kacrClear.LwGet() + mpe.lw);
			break;
			}
		}

LDone:
	*pchp = _chp;
	if (pvNil != pcpMin)
		*pcpMin = _cpMinChp;
	if (pvNil != pcpLim)
		*pcpLim = _cpLimChp;
}


/***************************************************************************
	Apply the given character properties to the given range of characters.
***************************************************************************/
bool TXRD::FApplyChp(long cp, long ccp, PCHP pchp, PCHP pchpDiff, ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertIn(cp, 0, CpMac() - 1);
	AssertIn(ccp, 1, CpMac() - cp);
	long cspvm;
	SPVM rgspvm[sprmLimChp - sprmMinChp];
	PRTUN prtun = pvNil;

	BumpCombineUndo();
	if (!FSetUndo(cp, cp + ccp, ccp))
		return fFalse;

	if (!_FGetRgspvmFromChp(pchp, pchpDiff, rgspvm, &cspvm))
		{
		CancelUndo();
		return fFalse;
		}

	//now make sure that _pglmpe has enough room - we need at most 2
	//entries per sprm.
	if (!_pglmpe->FEnsureSpace(2 * cspvm))
		{
		_ReleaseRgspvm(rgspvm, cspvm);
		CancelUndo();
		AssertThis(fobjAssertFull);
		return fFalse;
		}

	_ApplyRgspvm(cp, ccp, rgspvm, cspvm);
	CommitUndo();
	BumpCombineUndo();

	AssertThis(fobjAssertFull);
	InvalAllDdg(cp, ccp, ccp, grfdoc);
	return fTrue;
}


/***************************************************************************
	Find the bounds of the whole paragraphs overlapping the range
	[*pcpMin, *pcpMin + *pccp). If fExpand is false, the bounds are of
	whole paragraphs totally contained in the range, with the possible
	exception of the trailing end of paragraph marker. If fExpand is
	true, *pcpMin will only decrease and *pcpLim will only increase;
	while if fExpand is false, *pcpMin will only increase and
	*pcpLim may decrease or at most increase past an end of
	paragraph marker and its associated line feed characters.
***************************************************************************/
void TXRD::_GetParaBounds(long *pcpMin, long *pcpLim, bool fExpand)
{
	AssertVarMem(pcpMin);
	AssertVarMem(pcpLim);
	AssertIn(*pcpMin, 0, CpMac());
	AssertIn(*pcpLim, *pcpMin, CpMac() + 1);

	if (fExpand)
		{
		*pcpLim = CpLimPara(*pcpLim - (*pcpLim > 0 && *pcpLim > *pcpMin));
		*pcpMin = CpMinPara(*pcpMin);
		}
	else
		{
		if (!FMinPara(*pcpMin))
			*pcpMin = CpLimPara(*pcpMin);
		if (!FMinPara(*pcpLim))
			{
			*pcpLim = CpNext(*pcpLim);
			if (*pcpLim < CpMac())
				*pcpLim = CpMinPara(*pcpLim);
			}
		}
}


/***************************************************************************
	Get the long associated with the sprm and the difference between the
	two PAPs. ppapOld may be nil. *plwMask indicates which portion of
	the lw is significant. Returns tYes or tNo depending on whether the
	values of pchpOld and pchpNew differ for the given sprm. Returns
	tMaybe on error.
***************************************************************************/
bool TXRD::_TGetLwFromPap(byte sprm, PPAP ppapNew, PPAP ppapOld,
	long *plw, long *plwMask)
{
	AssertIn(sprm, sprmMinPap, sprmLimPap);
	AssertVarMem(ppapNew);
	AssertNilOrVarMem(ppapOld);
	AssertVarMem(plw);

	*plwMask = -1;
	switch (sprm)
		{
	case sprmHorz:
		if (pvNil != ppapOld)
			{
			if (ppapOld->jc == ppapNew->jc)
				*plwMask &= 0x00FFFFFFL;
			if (ppapOld->nd == ppapNew->nd)
				*plwMask &= 0xFF00FFFFL;
			if (ppapOld->dxpTab == ppapNew->dxpTab)
				*plwMask &= 0xFFFF0000L;
			}
		*plw = LwHighLow(SwHighLow(ppapNew->jc, ppapNew->nd),
			ppapNew->dxpTab - kdxpTabDef);
		break;
	case sprmVert:
		if (pvNil != ppapOld)
			{
			if (ppapOld->numLine == ppapNew->numLine)
				*plwMask &= 0x0000FFFFL;
			if (ppapOld->dypExtraLine == ppapNew->dypExtraLine)
				*plwMask &= 0xFFFF0000L;
			}
		*plw = LwHighLow(ppapNew->numLine - kdenLine, ppapNew->dypExtraLine);
		break;
	case sprmAfter:
		if (pvNil != ppapOld)
			{
			if (ppapOld->numAfter == ppapNew->numAfter)
				*plwMask &= 0x0000FFFFL;
			if (ppapOld->dypExtraAfter == ppapNew->dypExtraAfter)
				*plwMask &= 0xFFFF0000L;
			}
		*plw = LwHighLow(ppapNew->numAfter - kdenAfter, ppapNew->dypExtraAfter);
		break;
		}

	if (0 != *plwMask)
		{
		*plw &= *plwMask;
		return tYes;
		}
	TrashVar(plw);
	return tNo;
}


/***************************************************************************
	Get the paragraph properties for the paragraph containing the given
	character.
***************************************************************************/
void TXRD::FetchPap(long cp, PPAP ppap, long *pcpMin, long *pcpLim)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac());
	AssertVarMem(ppap);
	AssertNilOrVarMem(pcpMin);
	AssertNilOrVarMem(pcpLim);
	MPE mpe;
	byte sprm;
	ulong spcp;
	long cpLimT;
	bool fRet;

	if (FIn(cp, _cpMinPap, _cpLimPap))
		goto LDone;

	ClearPb(&_pap, size(PAP));
	_pap.dxpTab = kdxpTabDef;
	_pap.numLine = kdenLine;
	_pap.numAfter = kdenAfter;

	_cpLimPap = CpMac();
	_cpMinPap = 0;
	for (sprm = sprmMinPap; sprm < sprmLimPap; sprm++)
		{
		spcp = _SpcpFromSprmCp(sprm, cp);
		fRet = _FFindMpe(spcp, &mpe, &cpLimT);
		_cpLimPap = LwMin(_cpLimPap, cpLimT);
		if (!fRet)
			continue;
		_cpMinPap = LwMax(_cpMinPap, _CpFromSpcp(mpe.spcp));

		switch (sprm)
			{
		case sprmHorz:
			_pap.jc = B3Lw(mpe.lw);
			_pap.nd = B2Lw(mpe.lw);
			_pap.dxpTab += SwLow(mpe.lw);
			break;
		case sprmVert:
			_pap.numLine += SwHigh(mpe.lw);
			_pap.dypExtraLine = SwLow(mpe.lw);
			break;
		case sprmAfter:
			_pap.numAfter += SwHigh(mpe.lw);
			_pap.dypExtraAfter = SwLow(mpe.lw);
			break;
			}
		}

LDone:
	*ppap = _pap;
	if (pvNil != pcpMin)
		*pcpMin = _cpMinPap;
	if (pvNil != pcpLim)
		*pcpLim = _cpLimPap;
}


/***************************************************************************
	Apply the given paragraph properties to the given range of characters.
	If fExpand is true, the properties of the current paragraph containing
	cp are set regardless of whether cp is at the beginning of the
	paragraph.
***************************************************************************/
bool TXRD::FApplyPap(long cp, long ccp, PPAP ppap, PPAP ppapDiff,
	long *pcpMin, long *pcpLim, bool fExpand, ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertIn(cp, 0, CpMac());
	AssertIn(ccp, 0, CpMac() + 1 - cp);
	AssertVarMem(ppap);
	AssertNilOrVarMem(ppapDiff);
	AssertNilOrVarMem(pcpMin);
	AssertNilOrVarMem(pcpLim);
	long cpLim;
	long cspvm;
	SPVM rgspvm[sprmLimPap - sprmMinPap];

	//see if there are any paragraphs to deal with
	cpLim = cp + ccp;
	_GetParaBounds(&cp, &cpLim, fExpand);
	if (pvNil != pcpMin)
		*pcpMin = cp;
	if (pvNil != pcpLim)
		*pcpLim = cpLim;

	if (cpLim <= cp)
		{
		//no paragraphs to affect
		return fTrue;
		}

	BumpCombineUndo();
	if (!FSetUndo(cp, cpLim, cpLim - cp))
		return fFalse;

	if (!_FGetRgspvmFromPap(ppap, ppapDiff, rgspvm, &cspvm))
		{
		CancelUndo();
		return fFalse;
		}

	//now make sure that _pglmpe has enough room - we need at most 2
	//entries per sprm.
	if (!_pglmpe->FEnsureSpace(2 * cspvm))
		{
		_ReleaseRgspvm(rgspvm, cspvm);
		CancelUndo();
		AssertThis(fobjAssertFull);
		return fFalse;
		}

	_ApplyRgspvm(cp, cpLim - cp, rgspvm, cspvm);
	CommitUndo();
	BumpCombineUndo();

	AssertThis(fobjAssertFull);
	InvalAllDdg(cp, cpLim - cp, cpLim - cp, grfdoc);
	return fTrue;
}


/***************************************************************************
	Set up undo for an action. If this succeeds, you must call either
	CancelUndo or CommitUndo.
***************************************************************************/
bool TXRD::FSetUndo(long cp1, long cp2, long ccpIns)
{
	if (_cactSuspendUndo++ > 0 || _cundbMax == 0)
		return fTrue;

	Assert(_prtun == pvNil, "why is there an active undo record?");
	if (cp1 == cp2 && ccpIns == 0)
		return fTrue;

	if (pvNil == (_prtun = RTUN::PrtunNew(_cactCombineUndo, this,
			cp1, cp2, ccpIns)))
		{
		ResumeUndo();
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Cancel undo.
***************************************************************************/
void TXRD::CancelUndo(void)
{
	ResumeUndo();
	if (_cactSuspendUndo == 0)
		ReleasePpo(&_prtun);
}


/***************************************************************************
	Commit the setup undo.
***************************************************************************/
void TXRD::CommitUndo(void)
{
	PRTUN prtunPrev;

	ResumeUndo();
	if (_cactSuspendUndo != 0 || _prtun == pvNil)
		return;

	AssertPo(_prtun, 0);
	// see if this one can be combined with the last one
	if (_ipundbLimDone > 0)
		{
		_pglpundb->Get(_ipundbLimDone - 1, &prtunPrev);
		if (prtunPrev->FIs(kclsRTUN) && prtunPrev->FCombine(_prtun))
			{
			ClearRedo();
			goto LDone;
			}
		}
	FAddUndo(_prtun);

LDone:
	ReleasePpo(&_prtun);
}


/***************************************************************************
	Replace cp to cp + ccpDel with ccpIns characters from prgch. If ccpIns
	is zero, prgch can be nil.
***************************************************************************/
bool TXRD::FReplaceRgch(void *prgch, long ccpIns, long cp, long ccpDel,
	ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertIn(ccpIns, 0, kcbMax);
	AssertPvCb(prgch, ccpIns * size(achar));
	AssertIn(cp, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cp);

	if (CpMac() + ccpIns >= kcpMaxTxrd)
		{
		PushErc(ercRtxdTooMuchText);
		return fFalse;
		}

	if (!FSetUndo(cp, cp + ccpDel, ccpIns))
		return fFalse;

	if (!TXRD_PAR::FReplaceRgch(prgch, ccpIns, cp, ccpDel, fdocNil))
		{
		CancelUndo();
		return fFalse;
		}
	_AdjustMpe(cp, ccpIns, ccpDel);
	CommitUndo();
	AssertThis(fobjAssertFull);
	InvalAllDdg(cp, ccpIns, ccpDel, grfdoc);
	return fTrue;
}


/***************************************************************************
	Replace cp to cp + ccpDel with ccpIns characters from prgch, using
	the given chp and pap. If ccpIns is zero, prgch can be nil. pchp
	and/or ppap can be nil.
***************************************************************************/
bool TXRD::FReplaceRgch(void *prgch, long ccpIns, long cp, long ccpDel,
	PCHP pchp, PPAP ppap, ulong grfdoc)
{
	AssertThis(0);
	AssertIn(ccpIns, 0, kcbMax);
	AssertPvCb(prgch, ccpIns * size(achar));
	AssertIn(cp, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cp);
	AssertNilOrVarMem(pchp);
	AssertNilOrVarMem(ppap);

	return _FReplaceCore(prgch, pvNil, fFalse, pvNil, 0, ccpIns, cp, ccpDel,
		pchp, ppap, grfdoc);
}


/***************************************************************************
	Replace cp to cp + ccpDel with ccpIns characters from prgch, pflo or pbsf,
	using the given chp and pap. If ccpIns is zero, prgch can be nil. pchp
	and/or ppap can be nil.
***************************************************************************/
bool TXRD::_FReplaceCore(void *prgch, PFLO pflo, bool fCopy, PBSF pbsf,
	long cpSrc, long ccpIns, long cp, long ccpDel, PCHP pchp, PPAP ppap,
	ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertIn(ccpIns, 0, kcbMax);
	AssertNilOrPvCb(prgch, ccpIns * size(achar));
	AssertNilOrPo(pflo, 0);
	AssertNilOrPo(pbsf, 0);
	AssertIn(cp, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cp);
	AssertNilOrVarMem(pchp);
	AssertNilOrVarMem(ppap);
	SPVM rgspvm[sprmLimChp - sprmMinChp + sprmLimPap - sprmMinPap];
	long cspvmChp, cspvmPap;
	long cpMinUndo, cpLimUndo;

	if (CpMac() + ccpIns >= kcpMaxTxrd)
		{
		PushErc(ercRtxdTooMuchText);
		return fFalse;
		}

	cpMinUndo = cp;
	cpLimUndo = cp + ccpDel;

	cspvmChp = cspvmPap = 0;
	if (pvNil != pchp && ccpIns > 0 &&
			!_FGetRgspvmFromChp(pchp, pvNil, rgspvm, &cspvmChp))
		{
		return fFalse;
		}

	if (pvNil != ppap && ccpIns > 0)
		{
		_GetParaBounds(&cpMinUndo, &cpLimUndo, fFalse);
		if (cpMinUndo < cpLimUndo &&
				!_FGetRgspvmFromPap(ppap, pvNil, rgspvm + cspvmChp, &cspvmPap))
			{
			goto LFail;
			}
		cpMinUndo = LwMin(cp, cpMinUndo);
		cpLimUndo = LwMax(cp + ccpDel, cpLimUndo);
		}

	//now make sure that _pglmpe has enough room - we need at most 2
	//entries per sprm.
	if (!_pglmpe->FEnsureSpace(2 * (cspvmChp + cspvmPap)))
		goto LFail;

	if (!FSetUndo(cpMinUndo, cpLimUndo, cpLimUndo - cpMinUndo - ccpDel + ccpIns))
		goto LFail;

	if (pvNil != pflo)
		{
		Assert(cpSrc == 0 && ccpIns * size(achar) == pflo->cb, "bad flo");
		if (!FReplaceFlo(pflo, fCopy, cp, ccpDel, koskCur, fdocNil))
			goto LCancel;
		}
	else if (pvNil != pbsf)
		{
		if (!FReplaceBsf(pbsf, cpSrc, ccpIns, cp, ccpDel, fdocNil))
			goto LCancel;
		}
	else
		{
		Assert(cpSrc == 0, "bad cpSrc");
		AssertPvCb(prgch, ccpIns * size(achar));
		if (!FReplaceRgch(prgch, ccpIns, cp, ccpDel, fdocNil))
			{
LCancel:
			CancelUndo();
LFail:
			_ReleaseRgspvm(rgspvm, cspvmChp + cspvmPap);
			return fFalse;
			}
		}

	if (cspvmChp > 0)
		_ApplyRgspvm(cp, ccpIns, rgspvm, cspvmChp);
	if (cspvmPap > 0)
		{
		long cpMin = cp;
		long cpLim = cp + ccpIns;

		_GetParaBounds(&cpMin, &cpLim, fFalse);
		Assert(cpMin >= cpMinUndo, "new para before old one");
		Assert(cpLim <= cpLimUndo - ccpDel + ccpIns, "new para after old one");
		_ApplyRgspvm(cpMin, cpLim - cpMin, rgspvm + cspvmChp, cspvmPap);
		}
	CommitUndo();

	AssertThis(fobjAssertFull);
	InvalAllDdg(cpMinUndo, cpLimUndo - cpMinUndo - ccpDel + ccpIns,
		cpLimUndo - cpMinUndo, grfdoc);
	return fTrue;
}


/***************************************************************************
	Replace cp to cp + ccpDel with the characters in the given FLO.
***************************************************************************/
bool TXRD::FReplaceFlo(PFLO pflo, bool fCopy, long cp, long ccpDel,
	short osk, ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertPo(pflo, 0);
	AssertIn(cp, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cp);
	long ccpIns;
	FLO flo = *pflo;

	flo.pfil->AddRef();
	if (!flo.FTranslate(osk))
		goto LFail;

	ccpIns = flo.cb / size(achar);
	if (CpMac() + ccpIns >= kcpMaxTxrd)
		{
		PushErc(ercRtxdTooMuchText);
		goto LFail;
		}

	if (!FSetUndo(cp, cp + ccpDel, ccpIns))
		goto LFail;

	if (!TXRD_PAR::FReplaceFlo(&flo, fCopy, cp, ccpDel, koskCur, fdocNil))
		{
		CancelUndo();
LFail:
		ReleasePpo(&flo.pfil);
		return fFalse;
		}
	ReleasePpo(&flo.pfil);

	_AdjustMpe(cp, ccpIns, ccpDel);
	CommitUndo();
	AssertThis(fobjAssertFull);
	InvalAllDdg(cp, ccpIns, ccpDel, grfdoc);
	return fTrue;
}


/***************************************************************************
	Replace cp to cp + ccpDel with the characters in the given FLO, using
	the given chp and pap. pchp and/or ppap can be nil.
***************************************************************************/
bool TXRD::FReplaceFlo(PFLO pflo, bool fCopy, long cp, long ccpDel,
	PCHP pchp, PPAP ppap, short osk, ulong grfdoc)
{
	AssertThis(0);
	AssertPo(pflo, 0);
	AssertIn(cp, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cp);
	AssertNilOrVarMem(pchp);
	AssertNilOrVarMem(ppap);
	bool fRet;
	FLO flo = *pflo;

	flo.pfil->AddRef();
	fRet = flo.FTranslate(osk) &&
		_FReplaceCore(pvNil, &flo, fCopy && pflo->pfil == flo.pfil, pvNil,
			0, pflo->cb, cp, ccpDel, pchp, ppap, grfdoc);
	ReleasePpo(&flo.pfil);
	return fRet;
}


/***************************************************************************
	Replace cp to cpDst + ccpDel with ccpSrc characters from pbsfSrc starting
	at cpSrc.
***************************************************************************/
bool TXRD::FReplaceBsf(PBSF pbsfSrc, long cpSrc, long ccpSrc,
	long cpDst, long ccpDel, ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertPo(pbsfSrc, 0);
	AssertIn(cpSrc, 0, pbsfSrc->IbMac() / size(achar) + 1);
	AssertIn(ccpSrc, 0, pbsfSrc->IbMac() / size(achar) + 1 - cpSrc);
	AssertIn(cpDst, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cpDst);

	if (CpMac() + ccpSrc >= kcpMaxTxrd)
		{
		PushErc(ercRtxdTooMuchText);
		return fFalse;
		}

	if (!FSetUndo(cpDst, cpDst + ccpDel, ccpSrc))
		return fFalse;

	if (!TXRD_PAR::FReplaceBsf(pbsfSrc, cpSrc, ccpSrc, cpDst, ccpDel, fdocNil))
		{
		CancelUndo();
		return fFalse;
		}

	_AdjustMpe(cpDst, ccpSrc, ccpDel);
	CommitUndo();
	AssertThis(fobjAssertFull);
	InvalAllDdg(cpDst, ccpSrc, ccpDel, grfdoc);
	return fTrue;
}


/***************************************************************************
	Replace cp to cpDst + ccpDel with ccpSrc characters from pbsfSrc starting
	at cpSrc, using the given chp and pap. pchp and/or ppap can be nil.
***************************************************************************/
bool TXRD::FReplaceBsf(PBSF pbsfSrc, long cpSrc, long ccpSrc,
	long cpDst, long ccpDel, PCHP pchp, PPAP ppap, ulong grfdoc)
{
	AssertThis(0);
	AssertPo(pbsfSrc, 0);
	AssertIn(cpSrc, 0, pbsfSrc->IbMac() + 1);
	AssertIn(ccpSrc, 0, pbsfSrc->IbMac() + 1 - cpSrc);
	AssertIn(cpDst, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cpDst);
	AssertNilOrVarMem(pchp);
	AssertNilOrVarMem(ppap);

	return _FReplaceCore(pvNil, pvNil, fFalse, pbsfSrc, cpSrc, ccpSrc,
		cpDst, ccpDel, pchp, ppap, grfdoc);
}


/***************************************************************************
	Replace cp to cpDst + ccpDel with ccpSrc characters from pbsfSrc starting
	at cpSrc, using the given chp and pap. pchp and/or ppap can be nil.
***************************************************************************/
bool TXRD::FReplaceTxtb(PTXTB ptxtbSrc, long cpSrc, long ccpSrc,
	long cpDst, long ccpDel, ulong grfdoc)
{
	AssertThis(0);
	AssertPo(ptxtbSrc, 0);
	AssertIn(cpSrc, 0, ptxtbSrc->CpMac() + 1);
	AssertIn(ccpSrc, 0, ptxtbSrc->CpMac() + 1 - cpSrc);
	AssertIn(cpDst, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cpDst);

	// NOTE: this cast to PTXRD is just a rude hack to get around
	// an oddity of C++. TXRD cannot access _pbsf in a TXTB, only in a TXRD.
	// ptxtbSrc is probably not a TXRD, but the cast still works.
	return FReplaceBsf(((PTXRD)ptxtbSrc)->_pbsf, cpSrc, ccpSrc,
		cpDst, ccpDel, grfdoc);
}


/***************************************************************************
	Replace cp to cpDst + ccpDel with ccpSrc characters from pbsfSrc starting
	at cpSrc, using the given chp and pap. pchp and/or ppap can be nil.
***************************************************************************/
bool TXRD::FReplaceTxtb(PTXTB ptxtbSrc, long cpSrc, long ccpSrc,
	long cpDst, long ccpDel, PCHP pchp, PPAP ppap, ulong grfdoc)
{
	AssertThis(0);
	AssertPo(ptxtbSrc, 0);
	AssertIn(cpSrc, 0, ptxtbSrc->CpMac() + 1);
	AssertIn(ccpSrc, 0, ptxtbSrc->CpMac() + 1 - cpSrc);
	AssertIn(cpDst, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cpDst);
	AssertNilOrVarMem(pchp);
	AssertNilOrVarMem(ppap);

	// NOTE: this cast to PTXRD is just a rude hack to get around
	// an oddity of C++. TXRD cannot access _pbsf in a TXTB, only in a TXRD.
	// ptxtbSrc is probably not a TXRD, but the cast still works.
	return _FReplaceCore(pvNil, pvNil, fFalse, ((PTXRD)ptxtbSrc)->_pbsf, cpSrc,
		ccpSrc, cpDst, ccpDel, pchp, ppap, grfdoc);
}


/***************************************************************************
	Replace stuff in this document with stuff in the given document.
	REVIEW shonk: this doesn't preserve font size if the two docs have
	different default font sizes!
***************************************************************************/
bool TXRD::FReplaceTxrd(PTXRD ptxrd, long cpSrc, long ccpSrc, long cpDst,
	long ccpDel, ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertPo(ptxrd, 0);
	AssertIn(cpSrc, 0, ptxrd->CpMac());
	AssertIn(ccpSrc, 0, ptxrd->CpMac() + 1 - cpSrc);
	AssertIn(cpDst, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() + 1 - cpDst);
	Assert(ptxrd != this, "can't copy from a rich text doc to itself!");
	long dcp = 0;

	//REVIEW shonk: is there an easy way to make this atomic?

	if (CpMac() + ccpSrc >= kcpMaxTxrd)
		{
		PushErc(ercRtxdTooMuchText);
		return fFalse;
		}

	if (!FSetUndo(cpDst, cpDst + ccpDel, ccpSrc))
		return fFalse;

	//protect the trailing EOP
	if (cpDst + ccpDel == CpMac())
		{
#ifdef DEBUG
		achar ch;
		ptxrd->FetchRgch(cpSrc + ccpSrc - 1, 1, &ch);
		Assert(ch == kchReturn, "trying to replace trailing EOP");
#endif //DEBUG
		Assert(ccpDel > 0 && ccpSrc > 0, "bad parameters to FReplaceTxrd");
		ccpDel--;
		ccpSrc--;
		dcp = 1;
		}

	// insert the text
	if ((ccpSrc > 0 || ccpDel > 0) &&
			!FReplaceBsf(ptxrd->_pbsf, cpSrc, ccpSrc, cpDst, ccpDel, fdocNil))
		{
		CancelUndo();
		return fFalse;
		}

	if (ccpSrc > 0)
		_CopyProps(ptxrd, cpSrc, cpDst, ccpSrc, ccpSrc, 0, sprmMinPap);

	if (ccpSrc + dcp > 0)
		{
		long ccpSrcPap;
		long cpMinPap = cpDst;
		long cpLimPap = cpDst + ccpSrc;

		_GetParaBounds(&cpMinPap, &cpLimPap, fFalse);
		ccpSrcPap = ccpSrc - cpMinPap + cpDst;
		if (ccpSrcPap > 0 && cpMinPap < cpLimPap)
			{
			_CopyProps(ptxrd, cpSrc + cpMinPap - cpDst, cpMinPap,
				ccpSrcPap, cpLimPap - cpMinPap, sprmMinPap, sprmMinObj);
			}
		}

	if (ccpSrc > 0)
		{
		// object properties
		long cp;
		byte sprm;
		long impe, impeNew;
		MPE mpe, mpeNew;
		void *pv;
		bool fRet;

		ptxrd->_FFindMpe(_SpcpFromSprmCp(sprmMinObj, cpSrc), pvNil, pvNil, &impe);
		while (impe < ptxrd->_pglmpe->IvMac())
			{
			ptxrd->_pglmpe->Get(impe++, &mpe);
			cp = _CpFromSpcp(mpe.spcp);
			if (!FIn(cp, cpSrc, cpSrc + ccpSrc))
				continue;
			sprm = _SprmFromSpcp(mpe.spcp);
			mpeNew.spcp = _SpcpFromSprmCp(sprm, cpDst + cp - cpSrc);
			pv = ptxrd->_pagcact->PvLock(mpe.lw - 1);
			fRet = _FEnsureInAg(sprm, pv, ptxrd->_pagcact->Cb(mpe.lw - 1), &mpeNew.lw);
			ptxrd->_pagcact->Unlock();
			if (!fRet)
				continue;
	
			//insert the object sprm
			if (_FFindMpe(mpeNew.spcp, pvNil, pvNil, &impeNew))
				impeNew++;
			if (!_pglmpe->FInsert(impeNew, &mpeNew))
				_ReleaseInAg(mpeNew.lw);
			}
		}

	CommitUndo();
	AssertThis(fobjAssertFull);
	InvalAllDdg(cpDst, ccpSrc + dcp, ccpDel + dcp, grfdoc);
	return fTrue;
}


/***************************************************************************
	Copy properties from a TXRD to this one. Properties from sprmMin to
	sprmLim are copied.
***************************************************************************/
void TXRD::_CopyProps(PTXRD ptxrd, long cpSrc, long cpDst,
	long ccpSrc, long ccpDst, byte sprmMin, byte sprmLim)
{
	AssertThis(0);
	AssertPo(ptxrd, 0);
	AssertIn(cpSrc, 0, ptxrd->CpMac());
	AssertIn(cpDst, 0, CpMac());
	AssertIn(ccpSrc, 1, ptxrd->CpMac() + 1 - cpSrc);
	AssertIn(ccpDst, 1, CpMac() + 1 - cpDst);
	SPVM spvm;
	byte sprm;
	long impe;
	long cpMin, cpLim;
	bool fRet;
	long lw;
	long cb;
	void *pv;

	// zero the character properties over the inserted text
	spvm.sprm = sprmNil;
	spvm.lw = 0;
	spvm.lwMask = -1;
	_FFindMpe(_SpcpFromSprmCp(sprmMin, 0), pvNil, pvNil, &impe);
	while (_FFetchProp(impe++, &sprm) && sprm < sprmLim)
		{
		if (spvm.sprm == sprm)
			continue;
		spvm.sprm = sprm;
		if (_pglmpe->FEnsureSpace(2))
			_ApplyRgspvm(cpDst, ccpDst, &spvm, 1);
		else
			Warn("growing _pglmpe failed");
		}

	// apply the source properties
	ptxrd->_FFindMpe(_SpcpFromSprmCp(sprmMin, 0), pvNil, pvNil, &impe);
	while (ptxrd->_FFetchProp(impe++, &sprm, &lw, &cpMin, &cpLim) &&
			sprm < sprmLim)
		{
		// if this MPE doesn't overlap our source range, ignore it.
		if (cpLim <= cpSrc || cpMin >= cpSrc + ccpSrc)
			continue;

		// if this MPE goes to the end of the source range, also carry it
		// to the end of the destination
		if (cpLim >= cpSrc + ccpSrc)
			cpLim = cpSrc + ccpDst;

		// adjust the min and lim to destination cp values and restrict
		// them to [cpDst, cpDst + ccpDst).
		if ((cpMin += cpDst - cpSrc) < cpDst)
			cpMin = cpDst;
		if ((cpLim += cpDst - cpSrc) > cpDst + ccpDst)
			cpLim = cpDst + ccpDst;

		// if the range is empty, ignore it
		if (cpMin >= cpLim)
			continue;

		spvm.sprm = sprm;
		if (_FSprmInAg(sprm) && lw > 0)
			{
			cb = ptxrd->_pagcact->Cb(lw - 1);
			pv = ptxrd->_pagcact->PvLock(lw - 1);
			fRet = _FEnsureInAg(sprm, pv, cb, &spvm.lw);
			ptxrd->_pagcact->Unlock();
			if (!fRet)
				{
				Warn("ensure in ag failed");
				continue;
				}
			}
		else
			spvm.lw = lw;

		if (_pglmpe->FEnsureSpace(2))
			_ApplyRgspvm(cpMin, cpLim - cpMin, &spvm, 1);
		else
			{
			Warn("growing _pglmpe failed");
			_ReleaseRgspvm(&spvm, 1);
			}
		}
}


/***************************************************************************
	Find the first object at or after cpMin. Put its location in *pcp and
	allocate a buffer to hold its extra data and put it in *ppv (if ppv is
	not nil). If an object was found, but the buffer couldn't be allocated,
	*ppv is set to nil.
***************************************************************************/
bool TXRD::FFetchObject(long cpMin, long *pcp, void **ppv, long *pcb)
{
	AssertThis(0);
	AssertIn(cpMin, 0, CpMac());
	AssertVarMem(pcp);
	AssertNilOrVarMem(ppv);
	AssertNilOrVarMem(pcb);

	MPE mpe;
	long impe;
	long cb;
	bool fRet;

	fRet = _FFindMpe(_SpcpFromSprmCp(sprmObject, cpMin), &mpe, pcp, &impe);
	if (fRet && cpMin == _CpFromSpcp(mpe.spcp))
		*pcp = cpMin;
	else if (*pcp >= CpMac())
		return fFalse;
	else
		{
		if (fRet)
			impe++;
		Assert(impe < _pglmpe->IvMac(), "bad cpLim from _FFindMpe");
		_pglmpe->Get(impe, &mpe);
		Assert(_CpFromSpcp(mpe.spcp) == *pcp, "_FFindMpe messed up");
		}

	if (pvNil != pcb)
		*pcb = _pagcact->Cb(mpe.lw - 1);
	if (pvNil != ppv)
		{
		mpe.lw--;
		AssertIn(mpe.lw, 0, _pagcact->IvMac());
		cb = _pagcact->Cb(mpe.lw);
		if (FAllocPv(ppv, cb, fmemNil, mprNormal))
			CopyPb((byte *)_pagcact->QvGet(mpe.lw), *ppv, cb);
		}

	return fTrue;
}


/***************************************************************************
	Insert a picture into the rich text document.
***************************************************************************/
bool TXRD::FInsertObject(void *pv, long cb, long cp, long ccpDel,
	PCHP pchp, ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertPvCb(pv, cb);
	AssertIn(cp, 0, CpMac());
	AssertIn(ccpDel, 0, CpMac() - cp);
	AssertNilOrVarMem(pchp);
	SPVM rgspvm[sprmLimChp - sprmMinChp];
	MPE mpe;
	long impe;
	achar ch = kchObject;
	long cspvmChp = 0;

	BumpCombineUndo();
	if (!FSetUndo(cp, cp + ccpDel, 1))
		return fFalse;

	mpe.spcp = _SpcpFromSprmCp(sprmObject, cp);
	if (!_FEnsureInAg(sprmObject, pv, cb, &mpe.lw))
		goto LFail;

	if (pvNil != pchp && !_FGetRgspvmFromChp(pchp, pvNil, rgspvm, &cspvmChp))
		goto LFail;

	//now make sure that _pglmpe has enough room - we need at most 2
	//entries per sprm, plus one for the object
	if (!_pglmpe->FEnsureSpace(2 * cspvmChp + 1))
		goto LFail;

	if (!FReplaceRgch(&ch, 1, cp, ccpDel, fdocNil))
		{
		_ReleaseRgspvm(rgspvm, cspvmChp);
LFail:
		_ReleaseSprmLw(sprmObject, mpe.lw);
		CancelUndo();
		return fFalse;
		}

	if (cspvmChp > 0)
		_ApplyRgspvm(cp, 1, rgspvm, cspvmChp);

	//insert the object sprm
	if (_FFindMpe(mpe.spcp, pvNil, pvNil, &impe))
		impe++;
	AssertDo(_pglmpe->FInsert(impe, &mpe), "should have been ensured");
	CommitUndo();
	BumpCombineUndo();

	AssertThis(fobjAssertFull);
	InvalAllDdg(cp, 1, ccpDel, grfdoc);
	return fTrue;
}


/***************************************************************************
	Insert a picture into the rich text document.
***************************************************************************/
bool TXRD::FApplyObjectProps(void *pv, long cb, long cp, ulong grfdoc)
{
	AssertThis(fobjAssertFull);
	AssertPvCb(pv, cb);
	AssertIn(cp, 0, CpMac());
	MPE mpe;
	long impe;
	long lwOld;

	if (!_FFindMpe(_SpcpFromSprmCp(sprmObject, cp), &mpe, pvNil, &impe) ||
			cp != _CpFromSpcp(mpe.spcp))
		{
		Bug("cp not an object");
		return fFalse;
		}

	BumpCombineUndo();
	if (!FSetUndo(cp, cp + 1, 1))
		return fFalse;

	lwOld = mpe.lw;
	if (!_FEnsureInAg(sprmObject, pv, cb, &mpe.lw))
		{
		CancelUndo();
		return fFalse;
		}

	_pglmpe->Put(impe, &mpe);
	_ReleaseSprmLw(sprmObject, lwOld);
	CommitUndo();
	BumpCombineUndo();

	AssertThis(fobjAssertFull);
	InvalAllDdg(cp, 1, 1, grfdoc);
	return fTrue;
}


/***************************************************************************
	Get the bounds of an object.
***************************************************************************/
bool TXRD::FGetObjectRc(long cp, PGNV pgnv, PCHP pchp, RC *prc)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac());
	AssertPo(pgnv, 0);
	AssertVarMem(pchp);
	AssertVarMem(prc);
	MPE mpe;
	ulong spcp = _SpcpFromSprmCp(sprmObject, cp);

	if (!_FFindMpe(spcp, &mpe) || _CpFromSpcp(mpe.spcp) != cp)
		return fFalse;

	return _FGetObjectRc(mpe.lw - 1, _SprmFromSpcp(mpe.spcp), pgnv, pchp, prc);
}


/***************************************************************************
	Get the object bounds from the AG entry.
***************************************************************************/
bool TXRD::_FGetObjectRc(long icact, byte sprm, PGNV pgnv, PCHP pchp, RC *prc)
{
	AssertIn(icact, 0, _pagcact->IvMac());
	Assert(sprm >= sprmObject, 0);
	AssertPo(pgnv, 0);
	AssertVarMem(pchp);
	AssertVarMem(prc);

	//TXRD has no acceptable object types
	TrashVar(prc);
	return fFalse;
}


/***************************************************************************
	Draw an object.
***************************************************************************/
bool TXRD::FDrawObject(long cp, PGNV pgnv, long *pxp, long yp,
	PCHP pchp, RC *prcClip)
{
	AssertThis(0);
	AssertIn(cp, 0, CpMac());
	AssertPo(pgnv, 0);
	AssertVarMem(pxp);
	AssertVarMem(pchp);
	AssertVarMem(prcClip);
	MPE mpe;
	ulong spcp = _SpcpFromSprmCp(sprmObject, cp);

	if (!_FFindMpe(spcp, &mpe) || _CpFromSpcp(mpe.spcp) != cp)
		return fFalse;

	return _FDrawObject(mpe.lw - 1, _SprmFromSpcp(mpe.spcp),
		pgnv, pxp, yp, pchp, prcClip);
}


/***************************************************************************
	Draw the object.
***************************************************************************/
bool TXRD::_FDrawObject(long icact, byte sprm, PGNV pgnv, long *pxp, long yp,
	PCHP pchp, RC *prcClip)
{
	AssertIn(icact, 0, _pagcact->IvMac());
	Assert(sprm >= sprmObject, 0);
	AssertPo(pgnv, 0);
	AssertVarMem(pxp);
	AssertVarMem(pchp);
	AssertVarMem(prcClip);

	//TXRD has no acceptable object types
	return fFalse;
}


/***************************************************************************
	Create a new rich text undo object for the given rich text document.
***************************************************************************/
PRTUN RTUN::PrtunNew(long cactCombine, PTXRD ptxrd,
	long cp1, long cp2, long ccpIns)
{
	AssertPo(ptxrd, 0);
	AssertIn(cp1, 0, ptxrd->CpMac() + 1);
	AssertIn(cp2, 0, ptxrd->CpMac() + 1);
	AssertIn(ccpIns, 0, kcbMax);
	PRTUN prtun;

	SortLw(&cp1, &cp2);
	AssertIn(cp1, 0, ptxrd->CpMac());
	if (pvNil == (prtun = NewObj RTUN))
		return pvNil;

	prtun->_cactCombine = cactCombine;
	prtun->_cpMin = cp1;
	prtun->_ccpIns = ccpIns;
	if (cp1 < cp2)
		{
		// copy the piece of the txrd.
		if (pvNil == (prtun->_ptxrd = TXRD::PtxrdNew(pvNil)))
			goto LFail;
		prtun->_ptxrd->SetInternal();
		prtun->_ptxrd->SuspendUndo();
		if (!prtun->_ptxrd->FReplaceTxrd(ptxrd, cp1, cp2 - cp1, 0, 0))
			{
LFail:
			ReleasePpo(&prtun);
			}
		}

	AssertNilOrPo(prtun, 0);
	return prtun;
}


/***************************************************************************
	Destructor for a rich text undo object.
***************************************************************************/
RTUN::~RTUN(void)
{
	ReleasePpo(&_ptxrd);
}


/***************************************************************************
	Undo this rich text undo object on the given document.
***************************************************************************/
bool RTUN::FUndo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);
	PTXRD ptxrd;
	long ccpIns;
	PTXRD ptxrdNew = pvNil;

	if (!pdocb->FIs(kclsTXRD))
		goto LAssert;
	ptxrd = (PTXRD)pdocb;
	AssertPo(ptxrd, 0);

	if (!FIn(_cpMin, 0, ptxrd->CpMac()) ||
			!FIn(_ccpIns, 0, ptxrd->CpMac() + 1 - _cpMin))
		{
LAssert:
		Bug("This rich text undo object cannot be applied to this document");
		return fFalse;
		}

	if (_ccpIns > 0)
		{
		// copy the piece of the txrd.
		if (pvNil == (ptxrdNew = TXRD::PtxrdNew(pvNil)))
			return fFalse;
		ptxrdNew->SetInternal();
		ptxrdNew->SuspendUndo();
		if (!ptxrdNew->FReplaceTxrd(ptxrd, _cpMin, _ccpIns, 0, 0))
			goto LFail;
		}

	ptxrd->SuspendUndo();
	ptxrd->HideSel();
	if (pvNil == _ptxrd)
		{
		ccpIns = 0;
		if (!ptxrd->FReplaceRgch(pvNil, 0, _cpMin, _ccpIns))
			goto LFail;
		}
	else
		{
		ccpIns = _ptxrd->CpMac() - 1;
		if (!ptxrd->FReplaceTxrd(_ptxrd, 0, ccpIns, _cpMin, _ccpIns))
			{
LFail:
			ReleasePpo(&ptxrdNew);
			ptxrd->ResumeUndo();
			return fFalse;
			}
		}
	ptxrd->ResumeUndo();
	ptxrd->SetSel(_cpMin, _cpMin + ccpIns, ginNil);
	ptxrd->ShowSel();
	ptxrd->SetSel(_cpMin + ccpIns, _cpMin + ccpIns);

	ReleasePpo(&_ptxrd);
	_ptxrd = ptxrdNew;
	_ccpIns = ccpIns;
	return fTrue;
}


/***************************************************************************
	Redo this rich text undo object on the given document.
***************************************************************************/
bool RTUN::FDo(PDOCB pdocb)
{
	AssertThis(0);
	return FUndo(pdocb);
}


/***************************************************************************
	If possible, combine the given rtun with this one. Returns success.
***************************************************************************/
bool RTUN::FCombine(PRTUN prtun)
{
	AssertThis(0);
	AssertPo(prtun, 0);
	long ccp;

	// if the _cactCombine numbers are different, they can't be combined
	if (prtun->_cactCombine != _cactCombine)
		return fFalse;

	// if the new record doesn't delete anything and the new text is
	// at the end of the old text, just adjust _ccpIns.
	if (prtun->_ptxrd == pvNil && _cpMin + _ccpIns == prtun->_cpMin)
		{
		_ccpIns += prtun->_ccpIns;
		return fTrue;
		}

	// if either of the new records inserts anything, we can't combine the two
	if (prtun->_ccpIns != 0 || _ccpIns != 0)
		return fFalse;

	// handle repeated delete keys
	AssertPo(_ptxrd, 0);
	AssertPo(prtun->_ptxrd, 0);
	ccp = prtun->_ptxrd->CpMac() - 1;
	if (prtun->_cpMin == _cpMin)
		{
		return _ptxrd->FReplaceTxrd(prtun->_ptxrd, 0, ccp,
			_ptxrd->CpMac() - 1, 0);
		}

	// handle repeated backspace keys
	if (prtun->_cpMin + ccp == _cpMin)
		{
		if (!_ptxrd->FReplaceTxrd(prtun->_ptxrd, 0, ccp, 0, 0))
			return fFalse;
		_cpMin = prtun->_cpMin;
		return fTrue;
		}

	return fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a RTUN.
***************************************************************************/
void RTUN::AssertValid(ulong grf)
{
	RTUN_PAR::AssertValid(grf);
	AssertNilOrPo(_ptxrd, 0);
	AssertIn(_cpMin, 0, kcbMax);
	AssertIn(_ccpIns, 0, kcbMax);
	Assert(_ccpIns > 0 || _ptxrd != pvNil, "empty RTUN");
}


/***************************************************************************
	Mark memory for the RTUN.
***************************************************************************/
void RTUN::MarkMem(void)
{
	AssertThis(fobjAssertFull);
	RTUN_PAR::MarkMem();
	MarkMemObj(_ptxrd);
}
#endif //DEBUG

