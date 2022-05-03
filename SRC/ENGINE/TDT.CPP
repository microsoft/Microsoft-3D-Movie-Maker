/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	tdt.cpp: Three-D Text class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	TDT, the 3-D Text class, is a derived class of TMPL.  Most clients
	(ACTR, MVIE, etc) can treat TDTs like regular TMPLs.  But they have
	some extra functionality and work internally very differently from
	TMPLs.  Chunkwise, all the information for a TDT is in the TMPL
	chunk or the single TDT child chunk:

	TMPL // template info
	 |
	 |
	 +---TDT  (chid 0) // TDT-specific info (shape and tag to TDF)

	In addition to the usual TMPL fields, TDTs have a _tagTdf and a _tdts.
	_tagTdf tells what font to use for the TDT, and _tdts tells what shape
	to draw the TDT in.  TDTs on file are very small, so it is practical to
	store them in the user's document.

	Rather than fetching ACTNs and the default costume from child chunks
	of the TMPL, TDTs generate them in memory and store them in _pactnCache
	and _pmtrlDefault.  _pactnCache keeps a copy of the last requested
	action so that it doesn't have to be continuously recomputed.
	
	The user can change a TDT's text, shape, and/or font with FChange().
	When this happens, all the internal lists affecting the TDT's shape,
	costume, etc., are changed via _FInitLists().  After changing a TDT,
	you should call FAdjustBody on any BODYs based on that TDT.  In
	Socrates, there should only be one BODY per TDT, so this shouldn't be
	a problem.

***************************************************************************/
#include "soc.h"
ASSERTNAME

RTCLASS(TDT)


const CHID kchidTdt = 0; // CHID of TDT under TMPL chunk

// All actions have a step size of kdwrStep, except tdaWalk
const BRS kdwrStepWalk = BR_SCALAR(1.0); // step size for walk action
const BRS kdwrStep = BR_SCALAR(5.0); // step size for all other actions


PGST TDT::_pgstAction = pvNil;


/***************************************************************************
	Set the GST of action names for TDTs
***************************************************************************/
bool TDT::FSetActionNames(PGST pgstAction)
{
	AssertPo(pgstAction, 0);
	Assert(pvNil == _pgstAction, "you already set the action names");

	_pgstAction = pgstAction;
	_pgstAction->AddRef();
	return fTrue;
}


/****************************************
	3-D Text On File...this gets put in
	a child chunk of a TMPL
****************************************/
struct TDTF
	{
	short bo;
	short osk;
	long tdts;
	TAG tagTdf;
	};
const BOM kbomTdtf = (0x5C000000 | kbomTag >> 6);


/***************************************************************************
	Return a list of all tags embedded in this TDT.  Note that a
	return value of pvNil does not mean an error occurred, but simply that
	this TDT has no embedded tags.

	Actually, as currently implemented, this function only returns pvNil
	if an error occurs.  The point is, look at *pfError, not the return
	value.
***************************************************************************/
PGL TDT::PgltagFetch(PCFL pcfl, CTG ctg, CNO cno, bool *pfError)
{
	AssertPo(pcfl, 0);
	AssertVarMem(pfError);

	PGL pgltag;
	KID kid;
	BLCK blck;
	TDTF tdtf;

	*pfError = fFalse;
	pgltag = GL::PglNew(size(TAG));
	if (pvNil == pgltag)
		goto LFail;
	if (!pcfl->FGetKidChidCtg(ctg, cno, kchidTdt, kctgTdt, &kid))
		goto LFail;
	if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		goto LFail;
	if (!blck.FUnpackData())
		goto LFail;
	if (blck.Cb() < size(TDTF))
		goto LFail;
	if (!blck.FReadRgb(&tdtf, size(TDTF), 0))
		goto LFail;
	if (kboCur != tdtf.bo)
		SwapBytesBom(&tdtf, kbomTdtf);
	Assert(kboCur == tdtf.bo, "bad TDTF");
	if (!pgltag->FAdd(&tdtf.tagTdf))
		goto LFail;
	return pgltag;
LFail:
	*pfError = fTrue;
	ReleasePpo(&pgltag);
	return pvNil;
}


/***************************************************************************
	Create a new TDT
***************************************************************************/
PTDT TDT::PtdtNew(PSTN pstn, long tdts, PTAG ptagTdf)
{
	AssertPo(pstn, 0);
	AssertIn(tdts, 0, tdtsLim);
	AssertVarMem(ptagTdf);

	PTDT ptdt;

	ptdt = NewObj TDT;
	if (pvNil == ptdt)
		return pvNil;
	ptdt->_stn = *pstn;

	// This is a bit of a hack, but it makes life easier.  Without it,
	// the code would have to deal with body part sets with no body
	// parts in them, and TMPL and BODY would have problems with that
	// (for one thing, they would compute _cbset incorrectly).
	if (ptdt->_stn.Cch() == 0)	
		{
		achar chSpace = ChLit(' ');
		ptdt->_stn.SetRgch(&chSpace, 1);
		}

	ptdt->_tdts = tdts;
	ptdt->_tagTdf = *ptagTdf;
	if (!ptdt->_FInitLists())
		{
		ReleasePpo(&ptdt);
		return pvNil;
		}
	AssertPo(ptdt, 0);

	return ptdt;
}


/***************************************************************************
	Read the generic TMPL info and the TDT-specific info (tdts and tagTdf),
	then call _FInitLists to build the rest of the TDT.
***************************************************************************/
bool TDT::_FInit(PCFL pcfl, CTG ctgTmpl, CNO cnoTmpl)
{
	AssertBaseThis(0);
	AssertPo(pcfl, 0);

	KID kid;
	BLCK blck;
	TDTF tdtf;

	if (!_FReadTmplf(pcfl, ctgTmpl, cnoTmpl))
		return fFalse;
	if (!pcfl->FGetKidChidCtg(ctgTmpl, cnoTmpl, kchidTdt, kctgTdt, &kid))
		return fFalse;
	if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		return fFalse;
	if (!blck.FUnpackData())
		return fFalse;
	if (blck.Cb() < size(TDTF))
		return fFalse;
	if (!blck.FReadRgb(&tdtf, size(TDTF), 0))
		return fFalse;
	if (kboCur != tdtf.bo)
		SwapBytesBom(&tdtf, kbomTdtf);
	Assert(kboCur == tdtf.bo, "bad TDTF");
	_tagTdf = tdtf.tagTdf;
	_tdts = tdtf.tdts;

	if (!_FInitLists())
		return fFalse;

	return fTrue;
}


/***************************************************************************
	Initialize or reinitialize the TDT data structures, using the current
	_stn and _tdts.  This function unwinds completely on failure (the
	TDT's members are untouched).
***************************************************************************/
bool TDT::_FInitLists(void)
{
	AssertBaseThis(0);
	AssertPo(&_stn, 0);
	AssertIn(_tdts, 0, tdtsLim);

	long cch;
	PGL pglibactParNew = pvNil;
	PGL pglibsetNew = pvNil;
	PGG pggcmidNew = pvNil;
  	PMTRL pmtrlDefaultNew = pvNil;

	pglibactParNew = _PglibactParBuild();
	if (pvNil == pglibactParNew)
		goto LFail;
	pglibsetNew = _PglibsetBuild();
	if (pvNil == pglibsetNew)
		goto LFail;
	pggcmidNew = _PggcmidBuild();
	if (pvNil == pggcmidNew)
		goto LFail;
	cch = _stn.Cch();
	if (pvNil == _pmtrlDefault)
		{
		pmtrlDefaultNew = MTRL::PmtrlNew(); // get default solid-color material
		}
	else
		{
		pmtrlDefaultNew = _pmtrlDefault; // keep _pmtrlDefault the same
		pmtrlDefaultNew->AddRef();
		}
	if (pvNil == pmtrlDefaultNew)
		goto LFail;

	// We're home free: the function will succeed. Update member variables.
	ReleasePpo(&_pglibactPar);
	_pglibactPar = pglibactParNew;
	ReleasePpo(&_pglibset);
	_pglibset = pglibsetNew;
	ReleasePpo(&_pggcmid);
	_pggcmid = pggcmidNew;
	ReleasePpo(&_pmtrlDefault);
	_pmtrlDefault = pmtrlDefaultNew;
	ReleasePpo(&_pactnCache);
	_tdaCache = tdaNil;
	_cactn = tdaLim;
	_ccmid = 1;
	_cbset = 1;
	_grftmpl |= ftmplTdt;

	return fTrue;
LFail:
	ReleasePpo(&pglibactParNew);
	ReleasePpo(&pglibsetNew);
	ReleasePpo(&pggcmidNew);
	ReleasePpo(&pmtrlDefaultNew);
	return fFalse;
}


/***************************************************************************
	Get the given action.  If we've built it before, use the cached copy.
	Else build the action, cache it, and return it.
***************************************************************************/
PACTN TDT::_PactnFetch(long tda)
{
	AssertThis(0);
	AssertIn(tda, 0, tdaLim);

	if (tda != _tdaCache)
		{
		ReleasePpo(&_pactnCache);
		_tdaCache = tdaNil;
		_pactnCache = _PactnBuild(tda);
		}
	if (pvNil != _pactnCache)
		{
		_pactnCache->AddRef();
		_tdaCache = tda;
		}
	return _pactnCache;
}


/***************************************************************************
	Build the given action
***************************************************************************/
PACTN TDT::_PactnBuild(long tda)
{
	AssertThis(0);
	AssertIn(tda, 0, tdaLim);

	PACTN pactn;
	PGG pggcel;
	PGL pglbmat34 = pvNil;
	ulong grfactn;

	pggcel = _PggcelBuild(tda);
	if (pvNil == pggcel)
		goto LFail;
	pglbmat34 = _Pglbmat34Build(tda);
	if (pvNil == pglbmat34)
		goto LFail;
	grfactn = factnStatic | factnRotateY;
	pactn = ACTN::PactnNew(pggcel, pglbmat34, grfactn);
	if (pvNil == pactn)
		goto LFail;
	ReleasePpo(&pggcel);
	ReleasePpo(&pglbmat34);
	return pactn;
LFail:
	ReleasePpo(&pggcel);
	ReleasePpo(&pglbmat34);
	return pvNil;
}


/***************************************************************************
	Get the name of the given action
***************************************************************************/
bool TDT::FGetActnName(long anid, PSTN pstn)
{
	AssertThis(0);
	AssertIn(anid, 0, tdaLim);
	AssertPo(pstn, 0);

	long istn;
	long anidT;

	for (istn = 0; istn < _pgstAction->IvMac(); istn++)
		{
		_pgstAction->GetExtra(istn, &anidT);
		if (anid == anidT)
			{
			_pgstAction->GetStn(istn, pstn);
			return fTrue;
			}
		}
	Warn("action name not found");
	pstn->SetNil();
	return fFalse;
}


/***************************************************************************
	Fetch the given model for this TDT (use the TDT's current font)
***************************************************************************/
PMODL TDT::_PmodlFetch(CHID chidModl)
{
	AssertThis(0);
	AssertIn(chidModl, 0, _stn.Cch());

	PTDF ptdf;
	PMODL pmodl;

	ptdf = (PTDF)vptagm->PbacoFetch(&_tagTdf, TDF::FReadTdf);
	if (pvNil == ptdf)
		return pvNil;
	pmodl = ptdf->PmodlFetch((uchar)_stn.Psz()[chidModl]);
	ReleasePpo(&ptdf);
	return pmodl;
}


/***************************************************************************
	Build the BACT tree GL for BODY creation.  TDTs all have the same
	body part tree: every part is a child of the root.
***************************************************************************/
PGL TDT::_PglibactParBuild(void)
{
	AssertBaseThis(0);

	long cch = _stn.Cch();
	long ich;
	short ibactPar = ivNil;
	PGL pglibactPar;

	pglibactPar = GL::PglNew(size(short), cch); // ibacts are shorts
	if (pvNil == pglibactPar)
		return pvNil;
	AssertDo(pglibactPar->FSetIvMac(cch),
		"PglNew should have ensured space!");
	for (ich = 0; ich < cch; ich++)
		pglibactPar->Put(ich, &ibactPar);
	return pglibactPar;
}


/***************************************************************************
	Build the body part set GL for BODY creation.  For TDTs, all body parts
	belong to a single body part set
***************************************************************************/
PGL TDT::_PglibsetBuild(void)
{
	AssertBaseThis(0);

	long cch = _stn.Cch();
	long ich;
	short ibset = 0;
	PGL pglibset;

	pglibset = GL::PglNew(size(short), cch);
	if (pvNil == pglibset)
		return pvNil;
	AssertDo(pglibset->FSetIvMac(cch),
		"PglNew should have ensured space!");
	for (ich = 0; ich < cch; ich++)
		pglibset->Put(ich, &ibset);
	return pglibset;
}


/***************************************************************************
	Build the costume GG for TMPL creation.  For TDTs, the costume is
	simple: all body part sets get cmid 0.
***************************************************************************/
PGG TDT::_PggcmidBuild(void)
{
	AssertBaseThis(0);

	long cch = _stn.Cch();
	long lwOne = 1;
	PGG pggcmid;
	long cmid = 0;

	pggcmid = GG::PggNew(size(long), 1, size(long));
	if (pvNil == pggcmid)
		return pvNil;
	if (!pggcmid->FAdd(size(long), pvNil, &cmid, &lwOne))
		{
		ReleasePpo(&pggcmid);
		return pvNil;
		}
	return pggcmid;
}


/***************************************************************************
	Build a GL of matrices for the action
***************************************************************************/
PGL TDT::_Pglbmat34Build(long tda)
{
	AssertBaseThis(0);
	AssertIn(tda, 0, tdaLim);

	PTDF ptdf;
	long cch = _stn.Cch();
	long ich;
	BMAT34 bmat34;
	PGL pglbmat34 = pvNil;
	BRS dxrTotal;	// width of string (before scaling)
	BRS dxrTotal2;	// width of string (after scaling)
	BRS dxrHalf;	// half width of string (before scaling)
	BRS xrChar; 	// "insertion point" of character in string
	BRS xrCharCenter; // origin of character in string (after scaling)
	BRS dxrChar; 	// width of each character
	long ccel;
	long icel;
	BRS dyrChar; 	// height of each character
	BRS dyrTotal; 	// total height of string, if vertical shape
	BRS dyrHalf; 	// half of height of string, if vertical shape
	BRS yrChar; 	// position of char in string

	ptdf = (PTDF)vptagm->PbacoFetch(&_tagTdf, TDF::FReadTdf);
	if (pvNil == ptdf)
		goto LFail;

	ccel = _CcelOfTda(tda);

	pglbmat34 = GL::PglNew(size(BMAT34), LwMul(ccel, cch));
	if (pvNil == pglbmat34)
		goto LFail;
	AssertDo(pglbmat34->FSetIvMac(LwMul(ccel, cch)),
		"PglNew should have ensured space!");

	dxrTotal = rZero;
	for (ich = 0; ich < cch; ich++)
		dxrTotal += ptdf->DxrChar((uchar)_stn.Psz()[ich]);
	dxrHalf = BrsHalf(dxrTotal);

	dyrTotal = rZero;
	for (ich = 0; ich < cch; ich++)
		dyrTotal += ptdf->DyrChar((uchar)_stn.Psz()[ich]);
	dyrHalf = BrsHalf(dyrTotal);

	for (icel = 0; icel < ccel; icel++)
		{
		xrChar = -dxrHalf;
		yrChar = rZero;
		for (ich = 0; ich < cch; ich++)
			{
			dxrChar = ptdf->DxrChar((uchar)_stn.Psz()[ich]);
			dyrChar = ptdf->DyrChar((uchar)_stn.Psz()[ich]);
			xrCharCenter = xrChar + BrsHalf(dxrChar);

			BrMatrix34Identity(&bmat34);
			// The only exception to my "apply shape, then action" approach
			// is tdaStretch, which has to change dxrTotal2 before calling
			// _ApplyShape.
			if (tda == tdaStretch)
				{
				BRS rPhase;
				BRS rTheta;
				BRA aTheta;
				BRS dxr;
				BRS rFractStretch;
				BRS rT;

				rPhase = BrsDiv(BrIntToScalar(icel), BrIntToScalar(ccel));
				rTheta = BrsMul(rPhase, krTwoPi) + krPi + krHalfPi;
				aTheta = BrRadianToAngle(rTheta);
				dxr = BR_SIN(aTheta);
				rFractStretch = BR_SCALAR(1.5) + BrsHalf(dxr);
				rT = BrsMul(xrCharCenter, rFractStretch);
				xrCharCenter = rT;
				rT = BrsMul(dxrTotal, rFractStretch);
				dxrTotal2 = rT;
				}
			else
				{
				dxrTotal2 = dxrTotal;
				}
			// apply shape
			_ApplyShape(&bmat34, _tdts, cch, ich, xrCharCenter,	dxrTotal2,
				yrChar + dyrChar, ptdf->DyrMax(), dyrTotal);
			// apply transforms based on the action
			_ApplyAction(&bmat34, tda, ich, ccel, icel,	xrCharCenter,
				dxrTotal2);

			if (_tdts == tdtsCircleZ)
				{
				BrMatrix34PostTranslate(&bmat34, rZero,
					BrsDiv(dxrTotal, krTwoPi) + ptdf->DyrMax(), rZero);
				}
 			xrChar += dxrChar;
			yrChar += dyrChar;
			pglbmat34->Put(LwMul(icel, cch) + ich, &bmat34);
			}
		}
	ReleasePpo(&ptdf);
	return pglbmat34;
LFail:
	ReleasePpo(&ptdf);
	ReleasePpo(&pglbmat34);
	return pvNil;
}


/***************************************************************************
	Build a GG of cels for the action
***************************************************************************/
PGG TDT::_PggcelBuild(long tda)
{
	AssertBaseThis(0);
	AssertIn(tda, 0, tdaLim);

	long cch = _stn.Cch();
	long ich;
	PGG pggcel;
	CEL cel;
	CPS *prgcps = pvNil;
	long iv;
	long ccel;
	long icel;

	ccel = _CcelOfTda(tda);

	pggcel = GG::PggNew(size(CEL));
	if (pvNil == pggcel)
		goto LFail;
	if (!FAllocPv((void **)&prgcps, LwMul(cch, size(CPS)), fmemClear,
		mprNormal))
		{
		goto LFail;
		}
	for (icel = 0; icel < ccel; icel++)
		{
		cel.chidSnd = 0;
		cel.dwr = (tda == tdaWalk ? kdwrStepWalk : kdwrStep);
		for (ich = 0; ich < cch; ich++)
			{
			prgcps[ich].chidModl = (short)ich;
			prgcps[ich].imat34 = (short)(LwMul(icel, cch) + ich);
			}
		if (!pggcel->FAdd(LwMul(cch, size(CPS)), &iv, prgcps, &cel))
			goto LFail;
		}
	FreePpv((void **)&prgcps);
	return pggcel;
LFail:
	FreePpv((void **)&prgcps);
	ReleasePpo(&pggcel);
	return pvNil;
}


/***************************************************************************
	Destruct the TDT
***************************************************************************/
TDT::~TDT(void)
{
	AssertBaseThis(0);

	ReleasePpo(&_pmtrlDefault);
	ReleasePpo(&_pactnCache);
}


/***************************************************************************
	Return a duplicate of this TDT
***************************************************************************/
PTDT TDT::PtdtDup(void)
{
	AssertThis(0);

	PTDT ptdtDup;
	
	ptdtDup = TDT::PtdtNew(&_stn, _tdts, &_tagTdf);
	AssertNilOrPo(ptdtDup, 0);

	return ptdtDup;
}


/***************************************************************************
	Change the text, shape, and/or font of the TDT
***************************************************************************/
bool TDT::FChange(PSTN pstn, long tdts, PTAG ptagTdf)
{
	AssertThis(0);
	AssertNilOrPo(pstn, 0);
	if (tdtsNil != tdts)
		AssertIn(tdts, 0, tdtsLim);
	AssertNilOrVarMem(ptagTdf);

	STN stnSave;
	long tdtsSave;
	TAG tagTdfSave;

	stnSave = _stn;
	tdtsSave = _tdts;
	tagTdfSave = _tagTdf;

	if (pvNil != pstn)
		_stn = *pstn;
	if (tdtsNil != tdts)
		_tdts = tdts;
	if (pvNil != ptagTdf)
		_tagTdf = *ptagTdf;

	// This is a bit of a hack, but it makes life easier.  Without it,
	// the code would have to deal with body part sets with no body
	// parts in them, and TMPL and BODY would have problems with that
	// (for one thing, they would compute _cbset incorrectly).
	if (_stn.Cch() == 0)	
		{
		achar chSpace = ChLit(' ');
		_stn.SetRgch(&chSpace, 1);
		}

	if (!_FInitLists()) // note: unwinds on failure
		{
		_stn = stnSave;
		_tdts = tdtsSave;
		_tagTdf = tagTdfSave;
		return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	Get stats of this TDT
***************************************************************************/
void TDT::GetInfo(PSTN pstn, long *ptdts, PTAG ptagTdf)
{
	AssertThis(0);
	AssertNilOrPo(pstn, 0);
	AssertNilOrVarMem(ptdts);
	AssertNilOrVarMem(ptagTdf);

	if (pvNil != pstn)
		*pstn = _stn;
	if (pvNil != ptdts)
		*ptdts = _tdts;
	if (pvNil != ptagTdf)
		*ptagTdf = _tagTdf;
}


/***************************************************************************
	Adjust the given body's shape, since its owning TDT has changed
***************************************************************************/
bool TDT::FAdjustBody(PBODY pbody)
{
	AssertThis(0);
	AssertPo(pbody, 0);

	if (!pbody->FChangeShape(_pglibactPar, _pglibset))
		return fFalse;
	return fTrue;
}


/***************************************************************************
	Set the default costume.  This always succeeds for TDTs because they
	keep the PMTRL in memory.
***************************************************************************/
bool TDT::FSetDefaultCost(PBODY pbody)
{
	AssertThis(0);
	AssertPo(pbody, 0);

	pbody->SetPartSetMtrl(0, _pmtrlDefault);
	return fTrue;
}


/***************************************************************************
	Get a custom material
***************************************************************************/
PCMTL TDT::PcmtlFetch(long cmid)
{
	AssertThis(0);
	AssertIn(cmid, 0, _ccmid);

	Bug("Shouldn't fetch CMTLs from a TDT");
	return pvNil;
}


/***************************************************************************
	Write the TDT out as a TMPL hierarchy.
***************************************************************************/
bool TDT::FWrite(PCFL pcfl, CTG ctg, CNO *pcno)
{
	AssertThis(0);
	AssertPo(pcfl, 0);
	AssertVarMem(pcno);

	TDTF tdtf;
	CNO cnoTdt;
	BLCK blck;

	if (!_FWriteTmplf(pcfl, ctg, pcno))
		return fFalse;

	// Add TDT chunk
	tdtf.bo = kboCur;
	tdtf.osk = koskCur;
	tdtf.tdts = _tdts;
	tdtf.tagTdf = _tagTdf;

	if (!pcfl->FAddChild(ctg, *pcno, kchidTdt, size(TDTF), kctgTdt,
		&cnoTdt, &blck))
		{
		return fFalse;
		}
	if (!blck.FWrite(&tdtf))
		{
		pcfl->DeleteChild(ctg, *pcno, kctgTdt, cnoTdt, kchidTdt);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Return the number of cels in the given action
***************************************************************************/
long TDT::_CcelOfTda(long tda)
{
	AssertThis(0);
	AssertIn(tda, 0, tdaLim);

	long ccel;

	switch(tda)
		{
	case tdaRest:
		ccel = 1;
		break;
	case tdaLetterRotX:
	case tdaLetterRotY:
	case tdaLetterRotZ:
	case tdaSwingX:
	case tdaSwingY:
	case tdaSwingZ:
	case tdaPulse:
	case tdaHop:
	case tdaStretch:
		ccel = 12;
		break;
	case tdaWordRotX:
	case tdaWordRotY:
	case tdaWordRotZ:
	case tdaWave:
	case tdaReveal:
	case tdaWalk:
		ccel = 24;
		break;
	default:
		Bug("Unknown tda");
		break;
		}
	return ccel;
}


/***************************************************************************
	Apply some transformations to each character based on tda.	The
	transformation matrix is returned in pbmat34.  Some transformations are
	pre-applied and some are post-applied, depending on the desired effect.
***************************************************************************/
void TDT::_ApplyAction(BMAT34 *pbmat34, long tda, long ich, long ccel,
	long icel, BRS xrChar, BRS dxrText)
{
	AssertThis(0);
	AssertVarMem(pbmat34);
	AssertIn(tda, 0, tdaLim);
	AssertIn(ccel, 0, klwMax);
	AssertIn(icel, 0, ccel);

	BRS dxrTextHalf = BrsHalf(dxrText);
	// fraction of width of text that this char is at
	BRS xrFract = BrsDiv(xrChar + dxrTextHalf, dxrText);
	BRS rFractCel = BrsDiv(BrIntToScalar(icel), BrIntToScalar(ccel));
	BRS rSwing = BR_SIN(BrRadianToAngle(BrsMul(krTwoPi, rFractCel)));

	switch(tda)
		{
	case tdaRest:
		break;
	case tdaLetterRotX:
		BrMatrix34PreRotateX(pbmat34,
			BrRadianToAngle(BrsMul(rFractCel, krTwoPi)));
		break;
	case tdaLetterRotY:
		BrMatrix34PreRotateY(pbmat34,
			BrRadianToAngle(BrsMul(rFractCel, krTwoPi)));
		break;
	case tdaLetterRotZ:
		BrMatrix34PreRotateZ(pbmat34,
			BrRadianToAngle(BrsMul(rFractCel, krTwoPi)));
		break;
	case tdaSwingX:
		BrMatrix34PreShearY(pbmat34, BrsMul(krQuarter, rSwing),	rZero);
		break;
	case tdaSwingY:
		BrMatrix34PreShearZ(pbmat34, rZero, BrsHalf(rSwing));
		break;
	case tdaSwingZ:
		BrMatrix34PreShearY(pbmat34, rZero, BrsHalf(rSwing));
		break;
	case tdaPulse:
		BrMatrix34PreScale(pbmat34, rOne + BrsMul(krQuarter, rSwing),
			rOne + BrsMul(krQuarter, rSwing),
			rOne + BrsMul(krQuarter, rSwing));
		break;
	case tdaWordRotX:
		BrMatrix34PostRotateX(pbmat34,
			BrRadianToAngle(BrsMul(rFractCel, krTwoPi)));
		break;
	case tdaWordRotY:
		BrMatrix34PostRotateY(pbmat34,
			-BrRadianToAngle(BrsMul(rFractCel, krTwoPi)));
		break;
	case tdaWordRotZ:
		BrMatrix34PostRotateZ(pbmat34,
			BrRadianToAngle(BrsMul(rFractCel, krTwoPi)));
		break;
	case tdaWave:
		{
		BRS rTheta;
		BRA aTheta;
		BRS rScaleY;

		rTheta = BrsMul(xrFract - rFractCel, krThreePi);
		if (rTheta < -krTwoPi)
			rTheta += krThreePi;
		rTheta = LwBound(rTheta, rZero, krPi);
		aTheta = BrRadianToAngle(rTheta);
		rScaleY = rOne + BR_SIN(aTheta);
		BrMatrix34PreScale(pbmat34, rOne, rScaleY, rOne);
		}
		break;
	case tdaReveal:
		{
		BRS dxr;
		BRS rTheta;
		BRA aTheta;
		BRS rScaleY;

		if (icel < ccel / 2)
			{
			dxr = BrsDiv(BrIntToScalar(icel), BrIntToScalar(ccel / 2));
			dxr = BrsDiv(BrsMul(dxr, BR_SCALAR(4.0)), BR_SCALAR(3.0));
			rTheta = BrsMul(xrFract - dxr, krThreePi);
			rTheta += krPi + krHalfPi;
			rTheta = LwBound(rTheta, krHalfPi, krPi + krHalfPi);
			}
		else
			{
			dxr = BrsDiv(BrIntToScalar(icel - ccel / 2),
				BrIntToScalar(ccel / 2));
			dxr = BrsDiv(BrsMul(dxr, BR_SCALAR(4.0)), BR_SCALAR(3.0));
			rTheta = BrsMul(xrFract - dxr, krThreePi);
			rTheta += krPi;
			rTheta = LwBound(rTheta, -krHalfPi, krHalfPi);
			}
		aTheta = BrRadianToAngle(rTheta);
		rScaleY = BR_SCALAR(0.1) + BR_SCALAR(0.5) + BrsMul(BR_SCALAR(0.5),
			BR_SIN(aTheta));
		BrMatrix34PreScale(pbmat34, rOne, rScaleY, rOne);
		}
		break;
	case tdaWalk:
		{
		bool fOdd = (ich % 2 != 0);
		BRS rPhase;
		BRS rTheta;
		BRA aTheta;
		BRS dyr;
		BRS dzr;
		
		rPhase = BrsDiv(BrIntToScalar(icel), BrIntToScalar(ccel));
		rTheta = BrsMul(rPhase, krTwoPi);
		if (fOdd)
			{
			if (icel < ccel / 2)
				dzr = BrsMul(BrIntToScalar(ccel / 2 - icel), kdwrStepWalk);
			else
				dzr = BrsMul(BrIntToScalar(icel - ccel / 2), kdwrStepWalk);
			}
		else
			{
			if (icel < ccel / 2)
				dzr = BrsMul(BrIntToScalar(icel), kdwrStepWalk);				
			else
				dzr = BrsMul(BrIntToScalar(ccel - icel), kdwrStepWalk);
			}
		if (fOdd)
			rTheta -= krPi;
		rTheta = LwBound(rTheta, rZero, krPi);
		aTheta = BrRadianToAngle(rTheta);
		dyr = BR_SIN(aTheta);
		BrMatrix34PreTranslate(pbmat34, rZero, dyr, dzr);
		}
		break;
	case tdaHop:
		{
		BRS rPhase;
		BRS rTheta;
		BRA aTheta;
		BRS dyr = rZero;
		BRS yrScale = rOne;

		if (icel < ccel * 2 / 3)
			{
			rPhase = BrsDiv(BrIntToScalar(icel),
				BrIntToScalar(ccel * 2 / 3));
			rTheta = BrsMul(rPhase, krPi);
			aTheta = BrRadianToAngle(rTheta);
			dyr = BR_SIN(aTheta);
			}
		else
			{
			rPhase = BrsDiv(BrIntToScalar(icel - (ccel * 2 / 3)),
				BrIntToScalar(ccel / 3));
			// rTheta goes from krPi to krTwoPi
			rTheta = BrsMul(rPhase, krPi) + krPi;
			aTheta = BrRadianToAngle(rTheta);
			yrScale = rOne + BrsDiv(BR_SIN(aTheta), rTwo);
			}
		BrMatrix34PreTranslate(pbmat34, rZero, dyr, rZero);
		BrMatrix34PreScale(pbmat34, rOne, yrScale, rOne);
		}
		break;
	case tdaStretch:
		{
		BRS rPhase;
		BRS rTheta;
		BRA aTheta;
		BRS dyr = rZero;
		BRS yrScale = rOne;

		rPhase = BrsDiv(BrIntToScalar(icel), BrIntToScalar(ccel));
		rTheta = BrsMul(rPhase, krTwoPi) + krPi + krHalfPi;
		aTheta = BrRadianToAngle(rTheta);
		dyr = BR_SIN(aTheta);

		BrMatrix34PreScale(pbmat34, BR_SCALAR(1.5) + BrsHalf(dyr),
			rOne, rOne);
		}
		break;
	default:
		Bug("Unknown tda");
		break;
		}
}


/***************************************************************************
	Shape the text based on the given tdts. This function is called once
	per character in the TDT.  pbmat34 receives the transformation matrix
	for the character.  xrChar is the position of the current character
	(you could also think of this as the distance of the center of this
	character from the origin of the TDT).	dxrText is the width of the
	entire string.  dyr is the height of the font.
***************************************************************************/
void TDT::_ApplyShape(BMAT34 *pbmat34, long tdts, long cch, long ich,
	BRS xrChar, BRS dxrText, BRS yrChar, BRS dyrMax, BRS dyrTotal)
{
	AssertThis(0);
	AssertVarMem(pbmat34);
	AssertIn(tdts, 0, tdtsLim);
	AssertIn(cch, 0, kcchMaxStn);
	AssertIn(ich, 0, cch);

	BRS dxr = rZero;
	BRS dyr = rZero;
	BRS dzr = rZero;

	BRS dxrTextHalf = BrsHalf(dxrText);
	BRS dxrTextQuarter = BrsHalf(dxrTextHalf);
	// fraction of width of text that this char is at
	BRS xrFract = BrsDiv(xrChar + dxrTextHalf, dxrText);
	BRS rRadius = BrsDiv(dxrText, krTwoPi);

	switch(tdts)
		{
	case tdtsNormal:
		BrMatrix34PostTranslate(pbmat34, xrChar, rZero, rZero);
		break;			
	case tdtsArchPositive:
		dyr = BrsMul(dxrTextQuarter,
			BR_SIN(BrRadianToAngle(BrsMul(krPi, xrFract))));
		BrMatrix34PostTranslate(pbmat34, xrChar, dyr, rZero);
		break;
	case tdtsArchNegative:
		dyr = BrsMul(dxrTextQuarter,
			rOne - BR_SIN(BrRadianToAngle(BrsMul(krPi, xrFract))));
		BrMatrix34PostTranslate(pbmat34, xrChar, dyr, rZero);
		break;
	case tdtsCircleY:
		BrMatrix34PostTranslate(pbmat34, rZero, rZero, rRadius);
		BrMatrix34PostRotateY(pbmat34,
			BrRadianToAngle(BrsMul(krTwoPi, xrFract)));
		break;
	case tdtsLargeMiddle:
		BrMatrix34PostTranslate(pbmat34, xrChar, rZero, rZero);
		BrMatrix34PostScale(pbmat34, rOne, rOne +
			BR_SIN(BrRadianToAngle(BrsMul(krPi, xrFract))), rOne);
		break;
	case tdtsArchZ:
		dzr = BrsMul(dxrTextQuarter,
			BR_SIN(BrRadianToAngle(BrsMul(krPi, xrFract))));
		BrMatrix34PostTranslate(pbmat34, xrChar, rZero, dzr);
		break;
	case tdtsCircleZ:
		BrMatrix34PostTranslate(pbmat34, rZero, rRadius, rZero);
		BrMatrix34PostRotateZ(pbmat34,
			-BrRadianToAngle(BrsMul(krTwoPi, xrFract)));
		break;
	case tdtsVertical:
		BrMatrix34PostTranslate(pbmat34, rZero, dyrTotal - yrChar, rZero);
		break;
	case tdtsGrowRight:
		BrMatrix34PostTranslate(pbmat34, xrChar, rZero, rZero);
		BrMatrix34PostScale(pbmat34, rOne, rOne + xrFract, rOne);
		break;
	case tdtsGrowLeft:
		BrMatrix34PostTranslate(pbmat34, xrChar, rZero, rZero);
		BrMatrix34PostScale(pbmat34, rOne, rOne + (rOne - xrFract), rOne);
		break;
	default:
		Bug("unknown tdts");
		break;
		}
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the TDT.
***************************************************************************/
void TDT::AssertValid(ulong grf)
{
	TDT_PAR::AssertValid(fobjAllocated);
	AssertPo(_pgstAction, 0); // must set _pgstAction before creating TDTs
	Assert(_tagTdf.ctg != ctgNil, "TDT has bad _tagTdf");
	AssertPo(_pmtrlDefault, 0);
	if (tdaNil != _tdaCache)
		{
		AssertIn(_tdaCache, 0, tdaLim);
		AssertPo(_pactnCache, 0);
		}
	else
		{
		Assert(pvNil == _pactnCache, "_tdaCache is wrong");
		}
	AssertIn(_tdts, 0, tdtsLim);
}


/***************************************************************************
	Mark memory used by the TDT
***************************************************************************/
void TDT::MarkMem(void)
{
	AssertThis(0);

	TDT_PAR::MarkMem();

	MarkMemObj(_pmtrlDefault);
	if (tdaNil != _tdaCache)
		MarkMemObj(_pactnCache);	
}


/***************************************************************************
	Mark memory used by the TDT action GST
***************************************************************************/
void TDT::MarkActionNames(void)
{
	MarkMemObj(_pgstAction);
}

#endif //DEBUG
