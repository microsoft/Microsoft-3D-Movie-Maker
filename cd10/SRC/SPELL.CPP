/***************************************************************************
	Author: ******
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Spell checker support using CSAPI compliant spell checkers.

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(SPLC)

/***************************************************************************
	Constructor for the spell checker.
***************************************************************************/
SPLC::SPLC(void)
{
}


/***************************************************************************
	Destructor for the spell checker.
***************************************************************************/
SPLC::~SPLC(void)
{
	AssertThis(0);

	if (_fSplidValid)
		{
		if (_fMdrsValid)
			SpellCloseMdr(_splid, &_mdrs);
		if (_fUdrValid)
			SpellCloseUdr(_splid, _udr, fTrue);
		SpellTerminate(_splid, fTrue);
		}

#ifdef WIN
	if (hNil != _hlib)
		FreeLibrary(_hlib);
#endif //WIN
}


/***************************************************************************
	Static method to create a new spell checker.
***************************************************************************/
PSPLC SPLC::PsplcNew(SC_LID sclid, PSTN pstnCustom)
{
	PSPLC psplc;
	AssertNilOrPo(pstnCustom, 0);

	if (pvNil == (psplc = NewObj SPLC))
		return pvNil;

	if (!psplc->_FInit(sclid, pstnCustom))
		ReleasePpo(&psplc);

	AssertNilOrPo(psplc, 0);
	return psplc;
}


/***************************************************************************
	Initializes the spell checker - finds the dll, loads the default
	dictionary.
***************************************************************************/
bool SPLC::_FInit(SC_LID sclid, PSTN pstnCustom)
{
	AssertThis(0);
	AssertNilOrPo(pstnCustom, 0);

	SC_WSC wsc;
	FNI fni;

	if (!_FEnsureDll(sclid))
		return fFalse;

	ClearPb(&wsc, size(wsc));
	wsc.bHyphenHard = '-';
	wsc.bEmDash = 151;
	wsc.bEnDash = 150;
	wsc.bEllipsis = 133;
	wsc.rgParaBreak[0] = kchReturn;
	Win( wsc.rgParaBreak[1] = kchLineFeed; )

	if (secNOERRORS != SpellInit(&_splid, &wsc))
		return fFalse;
	_fSplidValid = fTrue;

	if (!_FEnsureMainDict(sclid, &fni))
		return fFalse;
	if (pvNil != pstnCustom && !_FEnsureUserDict(pstnCustom, &fni))
		return fFalse;

	return fTrue;
}


/***************************************************************************
	Find the spelling dll and link the functions we need.
***************************************************************************/
bool SPLC::_FEnsureDll(SC_LID sclid)
{
	AssertThis(0);

#ifdef WIN
	AssertVar(_hlib == hNil, "why is _hlib not nil?", &_hlib);

	HKEY hkey;
	long cb, lwType;
	STN stn;
	SZ sz;

	stn.FFormatSz(
			PszLit("SOFTWARE\\Microsoft\\Shared Tools\\Proofing Tools")
			PszLit("\\Spelling\\%d\\Normal"), sclid);
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, stn.Psz(), 0,
			KEY_QUERY_VALUE, &hkey))
		{
		goto LError;
		}

	if (ERROR_SUCCESS != RegQueryValueEx(hkey, PszLit("Engine"), pvNil,
			(ulong *)&lwType, pvNil, (ulong *)&cb) ||
		lwType != REG_SZ || cb >= size(sz) ||
		ERROR_SUCCESS != RegQueryValueEx(hkey, PszLit("Engine"), pvNil,
			(ulong *)&lwType, (byte *)sz, (ulong *)&cb))
		{
		RegCloseKey(hkey);
		goto LError;
		}
	RegCloseKey(hkey);

	if (hNil == (_hlib = LoadLibrary(sz)))
		goto LError;

	if (pvNil == (*(void **)&_pfnInit = (void *)GetProcAddress(_hlib, MIR(2))) ||
		pvNil == (*(void **)&_pfnOptions = (void *)GetProcAddress(_hlib, MIR(3))) ||
		pvNil == (*(void **)&_pfnCheck = (void *)GetProcAddress(_hlib, MIR(4))) ||
		pvNil == (*(void **)&_pfnTerminate = (void *)GetProcAddress(_hlib, MIR(5))) ||
		pvNil == (*(void **)&_pfnOpenMdr = (void *)GetProcAddress(_hlib, MIR(7))) ||
		pvNil == (*(void **)&_pfnOpenUdr = (void *)GetProcAddress(_hlib, MIR(8))) ||
		pvNil == (*(void **)&_pfnAddUdr = (void *)GetProcAddress(_hlib, MIR(9))) ||
		pvNil == (*(void **)&_pfnAddChangeUdr = (void *)GetProcAddress(_hlib, MIR(10))) ||
		pvNil == (*(void **)&_pfnClearUdr = (void *)GetProcAddress(_hlib, MIR(12))) ||
		pvNil == (*(void **)&_pfnCloseMdr = (void *)GetProcAddress(_hlib, MIR(15))) ||
		pvNil == (*(void **)&_pfnCloseUdr = (void *)GetProcAddress(_hlib, MIR(16))))
		{
		goto LError;
		}

	AssertThis(0);
	return fTrue;

LError:
	PushErc(ercSpellNoDll);
	return fFalse;

#else //!WIN
	RawRtn(); //REVIEW shonk: Mac: implement _FEnsureDll
	return fFalse;
#endif //!WIN
}


/***************************************************************************
	Find the main dictionary and load it
***************************************************************************/
bool SPLC::_FEnsureMainDict(SC_LID sclid, PFNI pfni)
{
	AssertThis(0);
	AssertNilOrPo(pfni, 0);

#ifdef WIN
	HKEY hkey;
	long cb, lwType;
	STN stn;
	SZ sz;

	stn.FFormatSz(
			PszLit("SOFTWARE\\Microsoft\\Shared Tools\\Proofing Tools")
			PszLit("\\Spelling\\%d\\Normal"), sclid);
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, stn.Psz(), 0,
			KEY_QUERY_VALUE, &hkey))
		{
		goto LError;
		}

	if (ERROR_SUCCESS != RegQueryValueEx(hkey, PszLit("Dictionary"), pvNil,
			(ulong *)&lwType, pvNil, (ulong *)&cb) ||
		lwType != REG_SZ || cb >= size(sz) ||
		ERROR_SUCCESS != RegQueryValueEx(hkey, PszLit("Dictionary"), pvNil,
			(ulong *)&lwType, (byte *)sz, (ulong *)&cb))
		{
		RegCloseKey(hkey);
		goto LError;
		}
	RegCloseKey(hkey);

	if (pvNil != pfni)
		{
		STN stn = sz;

		if (!pfni->FBuildFromPath(&stn))
			goto LError;
		}

	if (!_FLoadDictionary(sclid, sz, &_mdrs))
		goto LError;
	_fMdrsValid = fTrue;

	AssertThis(0);
	return fTrue;

LError:
	PushErc(ercSpellNoDict);
	return fFalse;

#else //!WIN
	RawRtn(); //REVIEW shonk: Mac: implement _FEnsureMainDict
	return fFalse;
#endif //!WIN
}


/***************************************************************************
	Find the main dictionary and load it
***************************************************************************/
bool SPLC::_FEnsureUserDict(PSTN pstnCustom, PFNI pfniDef)
{
	AssertThis(0);
	AssertPo(pstnCustom, 0);

#ifdef WIN
	HKEY hkey;
	long cb, lwType;
	SZ sz;
	STN stn;
	FNI fni;

	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			PszLit("SOFTWARE\\Microsoft\\Shared Tools Location"),
			0, KEY_QUERY_VALUE, &hkey))
		{
		goto LNoKey;
		}

	if (ERROR_SUCCESS != RegQueryValueEx(hkey, PszLit("PROOF"), pvNil,
			(ulong *)&lwType, pvNil, (ulong *)&cb) ||
		lwType != REG_SZ || cb >= size(sz) ||
		ERROR_SUCCESS != RegQueryValueEx(hkey, PszLit("PROOF"), pvNil,
			(ulong *)&lwType, (byte *)sz, (ulong *)&cb))
		{
		RegCloseKey(hkey);
LNoKey:
		if (pvNil == pfniDef)
			goto LError;
		fni = *pfniDef;
		}
	else
		{
		RegCloseKey(hkey);

		stn = sz;
		if (!fni.FBuildFromPath(&stn, kftgDir))
			goto LError;
		}

	if (!fni.FSetLeaf(pstnCustom, kftgDictionary))
		goto LError;
	fni.GetStnPath(&stn);
	if (!_FLoadUserDictionary(stn.Psz(), &_udr, fTrue))
		goto LError;
	_fUdrValid = fTrue;

	AssertThis(0);
	return fTrue;

LError:
	PushErc(ercSpellNoUserDict);
	return fFalse;

#else //!WIN
	RawRtn(); //REVIEW shonk: Mac: implement _FEnsureUserDict
	return fFalse;
#endif //!WIN
}


/***************************************************************************
	Load a particular dictionary given its path.
***************************************************************************/
bool SPLC::_FLoadDictionary(SC_LID sclid, PSZ psz, SC_MDRS *pmdrs)
{
	AssertThis(0);
	AssertSz(psz);
	AssertVarMem(pmdrs);

	if (!_fSplidValid)
		{
		Bug("spell checker not initialized");
		return fFalse;
		}

	if (secNOERRORS != SpellOpenMdr(_splid, psz, pvNil, fFalse, fTrue,
			sclid, pmdrs))
		{
		return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	Load a particular user dictionary given its path.
***************************************************************************/
bool SPLC::_FLoadUserDictionary(PSZ psz, SC_UDR *pudr, bool fCreate)
{
	AssertThis(0);
	AssertSz(psz);
	AssertVarMem(pudr);
	SC_BOOL fReadOnly;

	if (!_fSplidValid)
		{
		Bug("spell checker not initialized");
		return fFalse;
		}

	if (secNOERRORS != SpellOpenUdr(_splid, psz, FPure(fCreate),
			IgnoreAlwaysProp, pudr, &fReadOnly))
		{
		return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	Set spelling options.
***************************************************************************/
bool SPLC::FSetOptions(ulong grfsplc)
{
	AssertThis(0);

	if (!_fSplidValid)
		{
		Bug("spell checker not initialized");
		return fFalse;
		}

	if (secNOERRORS != SpellOptions(_splid, grfsplc))
		return fFalse;

	return fTrue;
}


/***************************************************************************
	Check the spelling of stuff in the given buffer.
***************************************************************************/
bool SPLC::FCheck(achar *prgch, long cch, long *pichMinBad, long *pichLimBad,
	PSTN pstnReplace, long *pscrs)
{
	AssertThis(0);
	AssertIn(cch, 0, ksuMax);
	AssertPvCb(prgch, cch * size(achar));
	AssertVarMem(pichMinBad);
	AssertVarMem(pichLimBad);
	AssertPo(pstnReplace, 0);
	AssertVarMem(pscrs);

	SC_SIB sib;
	SC_SRB srb;
	SC_SEC sec;
	SZ sz;
	byte bRate;

	pstnReplace->SetNil();
	*pichMinBad = *pichLimBad = cch;
	if (!_fSplidValid)
		{
		Bug("spell checker not initialized");
		return fFalse;
		}

	ClearPb(&sib, size(sib));
	sib.cch = (ushort)cch;
	sib.lrgch = prgch;
	sib.cMdr = 1;
	sib.lrgMdr = &_mdrs.mdr;
	if (_fUdrValid)
		{
		sib.cUdr = 1;
		sib.lrgUdr = &_udr;
		}

	ClearPb(&srb, size(srb));
	srb.lrgsz = sz;
	srb.cch = size(sz) / size(achar);
	srb.lrgbRating = &bRate;
	srb.cbRate = 1;

	sec = SpellCheck(_splid, sccVerifyBuffer, &sib, &srb);

	if (sec != secNOERRORS)
		return fFalse;

	*pscrs = srb.scrs;
	switch (srb.scrs)
		{
	case scrsNoErrors:
		return fTrue;

	case scrsReturningChangeAlways:
	case scrsReturningChangeOnce:
		*pstnReplace = srb.lrgsz;
		break;
		}

	*pichMinBad = srb.ichError;
	*pichLimBad = srb.ichError + srb.cchError;
	return fTrue;
}


/***************************************************************************
	Get the istn'th suggestion for the given word.
***************************************************************************/
bool SPLC::FSuggest(achar *prgch, long cch, bool fFirst, PSTN pstn)
{
	AssertThis(0);
	AssertIn(cch, 1, ksuMax);
	AssertPvCb(prgch, cch * size(achar));
	AssertPo(pstn, 0);

	SC_SIB sib;
	SC_SRB srb;
	SC_SEC sec;
	SC_CC sccc;

	pstn->SetNil();
	if (!_fSplidValid)
		{
		Bug("spell checker not initialized");
		return fFalse;
		}

	if (!fFirst && _ichSuggest < CvFromRgv(_rgchSuggest) &&
			_rgchSuggest[_ichSuggest] != 0)
		{
		// have another suggestion in the buffer
		*pstn = _rgchSuggest + _ichSuggest;
		_ichSuggest += CchSz(_rgchSuggest + _ichSuggest) + 1;
		return fTrue;
		}

	ClearPb(&sib, size(sib));
	sib.cch = (ushort)cch;
	sib.lrgch = prgch;
	sib.cMdr = 1;
	sib.lrgMdr = &_mdrs.mdr;
	if (_fUdrValid)
		{
		sib.cUdr = 1;
		sib.lrgUdr = &_udr;
		}

	ClearPb(&srb, size(srb));
	srb.lrgsz = _rgchSuggest;
	srb.cch = CvFromRgv(_rgchSuggest);

	_ichSuggest = 0;
	sccc = fFirst ? sccSuggest : sccSuggestMore;
	for ( ; ; sccc = sccSuggestMore)
		{
		ClearPb(_rgchSuggest, size(_rgchSuggest));

		sec = SpellCheck(_splid, sccc, &sib, &srb);
		if (sec != secNOERRORS)
			{
			// invalidate the buffer
			_ichSuggest = CvFromRgv(_rgchSuggest);
			return fFalse;
			}

		switch (srb.scrs)
			{
		case scrsNoMoreSuggestions:
			// invalidate the buffer
			_ichSuggest = CvFromRgv(_rgchSuggest);
			return fFalse;

		default:
			if (srb.csz > 0)
				{
				*pstn = _rgchSuggest;
				_ichSuggest = CchSz(_rgchSuggest) + 1;
				return fTrue;
				}
			break;
			}
		}
}


/***************************************************************************
	Add this word to the ignore all list.
***************************************************************************/
bool SPLC::FIgnoreAll(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	if (!_fSplidValid)
		{
		Bug("spell checker not initialized");
		return fFalse;
		}

	return secNOERRORS == SpellAddUdr(_splid, udrIgnoreAlways, pstn->Psz());
}


/***************************************************************************
	Add this word pair to the change once list.
***************************************************************************/
bool SPLC::FChange(PSTN pstnSrc, PSTN pstnDst, bool fAll)
{
	AssertThis(0);
	AssertPo(pstnSrc, 0);
	AssertPo(pstnDst, 0);

	if (!_fSplidValid)
		{
		Bug("spell checker not initialized");
		return fFalse;
		}

	return secNOERRORS == SpellAddChangeUdr(_splid,
		fAll ? udrChangeAlways : udrChangeOnce, pstnSrc->Psz(), pstnDst->Psz());
}


/***************************************************************************
	Add this word pair to the user dictionary.
***************************************************************************/
bool SPLC::FAddToUser(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	if (!_fUdrValid)
		{
		Bug("user dictionary not loaded");
		return fFalse;
		}

	return secNOERRORS == SpellAddUdr(_splid, _udr, pstn->Psz());
}


/***************************************************************************
	Add this word to the ignore all list.
***************************************************************************/
void SPLC::FlushIgnoreList(void)
{
	AssertThis(0);

	if (!_fSplidValid)
		{
		Bug("spell checker not initialized");
		return;
		}

	SpellClearUdr(_splid, udrIgnoreAlways);
}


/***************************************************************************
	Add this word pair to the change once list.
***************************************************************************/
void SPLC::FlushChangeList(bool fAll)
{
	AssertThis(0);

	if (!_fSplidValid)
		{
		Bug("spell checker not initialized");
		return;
		}

	SpellClearUdr(_splid, fAll ? udrChangeAlways : udrChangeOnce);
}


/***************************************************************************
	These are the stubs for the dll entry points.
***************************************************************************/
#ifdef WIN
/***************************************************************************
	Stub for SpellInit
***************************************************************************/
SC_SEC SPLC::SpellInit(SC_SPLID *psplid, SC_WSC *pwsc)
{
	AssertThis(0);

	if (pvNil == _pfnInit)
		{
		Bug("nil _pfnInit");
		return secModuleError;
		}

	return (*_pfnInit)(psplid, pwsc);
}


/***************************************************************************
	Stub for SpellOptions
***************************************************************************/
SC_SEC SPLC::SpellOptions(SC_SPLID splid, long grfso)
{
	AssertThis(0);

	if (pvNil == _pfnOptions)
		{
		Bug("nil _pfnOptions");
		return secModuleError;
		}

	return (*_pfnOptions)(splid, grfso);
}


/***************************************************************************
	Stub for SpellCheck
***************************************************************************/
SC_SEC SPLC::SpellCheck(SC_SPLID splid, SC_CC sccc, LPSC_SIB psib, LPSC_SRB psrb)
{
	AssertThis(0);

	if (pvNil == _pfnCheck)
		{
		Bug("nil _pfnCheck");
		return secModuleError;
		}

	return (*_pfnCheck)(splid, sccc, psib, psrb);
}


/***************************************************************************
	Stub for SpellTerminate
***************************************************************************/
SC_SEC SPLC::SpellTerminate(SC_SPLID splid, SC_BOOL fForce)
{
	AssertThis(0);

	if (pvNil == _pfnTerminate)
		{
		Bug("nil _pfnTerminate");
		return secModuleError;
		}

	return (*_pfnTerminate)(splid, fForce);
}


/***************************************************************************
	Stub for SpellOpenMdr
***************************************************************************/
SC_SEC SPLC::SpellOpenMdr(SC_SPLID splid, LPSC_PATH ppath,
	LPSC_PATH ppathExclude, SC_BOOL fCreateExclude, SC_BOOL fCache,
	SC_LID sclidExpected, LPSC_MDRS pmdrs)
{
	AssertThis(0);

	if (pvNil == _pfnOpenMdr)
		{
		Bug("nil _pfnOpenMdr");
		return secModuleError;
		}

	return (*_pfnOpenMdr)(splid, ppath, ppathExclude, fCreateExclude,
		fCache, sclidExpected, pmdrs);
}


/***************************************************************************
	Stub for SpellOpenUdr
***************************************************************************/
SC_SEC SPLC::SpellOpenUdr(SC_SPLID splid, LPSC_PATH ppath, SC_BOOL fCreate,
	SC_WORD udrprop, SC_UDR * pudr, SC_BOOL *pfReadOnly)
{
	AssertThis(0);

	if (pvNil == _pfnOpenUdr)
		{
		Bug("nil _pfnOpenUdr");
		return secModuleError;
		}

	return (*_pfnOpenUdr)(splid, ppath, fCreate, udrprop, pudr, pfReadOnly);
}


/***************************************************************************
	Add a word to the given user dictionary
***************************************************************************/
SC_SEC SPLC::SpellAddUdr(SC_SPLID splid, SC_UDR udr, SC_CHAR *pszAdd)
{
	AssertThis(0);

	if (pvNil == _pfnAddUdr)
		{
		Bug("nil _pfnAddUdr");
		return secModuleError;
		}

	return (*_pfnAddUdr)(splid, udr, pszAdd);
}


/***************************************************************************
	Add a word pair to the given user dictionary
***************************************************************************/
SC_SEC SPLC::SpellAddChangeUdr(SC_SPLID splid, SC_UDR udr,
	SC_CHAR *pszAdd, SC_CHAR *pszChange)
{
	AssertThis(0);

	if (pvNil == _pfnAddChangeUdr)
		{
		Bug("nil _pfnAddChangeUdr");
		return secModuleError;
		}

	return (*_pfnAddChangeUdr)(splid, udr, pszAdd, pszChange);
}


/***************************************************************************
	Stub for SpellClearUdr
***************************************************************************/
SC_SEC SPLC::SpellClearUdr(SC_SPLID splid, SC_UDR udr)
{
	AssertThis(0);

	if (pvNil == _pfnClearUdr)
		{
		Bug("nil _pfnClearUdr");
		return secModuleError;
		}

	return (*_pfnClearUdr)(splid, udr);
}


/***************************************************************************
	Stub for SpellCloseMdr
***************************************************************************/
SC_SEC SPLC::SpellCloseMdr(SC_SPLID splid, LPSC_MDRS pmdrs)
{
	AssertThis(0);

	if (pvNil == _pfnCloseMdr)
		{
		Bug("nil _pfnCloseMdr");
		return secModuleError;
		}

	return (*_pfnCloseMdr)(splid, pmdrs);
}


/***************************************************************************
	Stub for SpellCloseUdr
***************************************************************************/
SC_SEC SPLC::SpellCloseUdr(SC_SPLID splid, SC_UDR udr, SC_BOOL fForce)
{
	AssertThis(0);

	if (pvNil == _pfnCloseUdr)
		{
		Bug("nil _pfnCloseUdr");
		return secModuleError;
		}

	return (*_pfnCloseUdr)(splid, udr, fForce);
}
#endif //WIN


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a SPLC.
***************************************************************************/
void SPLC::AssertValid(ulong grf)
{
	SPLC_PAR::AssertValid(0);
}


/***************************************************************************
	Mark memory for the SPLC.
***************************************************************************/
void SPLC::MarkMem(void)
{
	AssertValid(0);
	SPLC_PAR::MarkMem();
}
#endif //DEBUG
