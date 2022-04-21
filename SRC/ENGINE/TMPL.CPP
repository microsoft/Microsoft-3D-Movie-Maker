/***************************************************************************

	tmpl.cpp: Actor template class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	Here's the TMPL chunk tree (* means more than one chunk can go here):

	TMPL - template flags
	 |
	 +--GLPI (chid 0) - parent IDs (BODY part hierarchy)
	 |
	 +--GLBS (chid 0) - body part sets for BODY
	 |
	 +--GGCM (chid 0) - custom costumes per body part set (GG of cmids)
	 |
	 +--CMTL* (chid <cmid>) - custom material...see mtrl.h
	 |   |
	 |   +--MTRL*
	 |       |
	 |       +--TMAP
	 |
	 +--MODL* - models used in this template
	 |
	 +--ACTN* (chid <anid>) - action for this template
	     |
		 +--GGCL (chid 0) - GG of cels for this action
		 |
		 +--GLXF (chid 0) - GL of transformation matrices for this action
		 |
		 +--GLMS (chid 0) - GL of motionmatch sounds for cels of this action

	About Actions: An action is an activity that a body can perform, such
	as walking, jumping, breathing, or resting.  Actions are broken down
	into cels, where each cel describes the position of each body part of
	the actor at one step or phase of the action.  After reaching the last
	cel, the action loops around to the beginning cel and repeats.  Cels
	consist of a list of cel part specs (CPS).  Each CPS refers to a single
	body part of an actor, such as a leg or head.  The CPS tells what
	BRender model to use for the body part for this cel and what matrix to
	use to orient it to its parent body part.  Each time the cel number is
	changed, TMPL reads each CPS of the new cel and updates each body part
	to use the new model and transformation matrix.

	About the GGCM: it is indexed by body part set, and tells how many 
	custom materials are available for each body part set, and what the
	CMIDs are for those materials.  The first CMID in each body part set
	is the default one (for the default costume).  Example: If a body had 
	3 body part sets (shirt, pants, and head), and there are 2 shirts,
	3 pants, and 1 head, the GGCM would be:

	0: fixed = 2, variable = (0, 1) 
	1: fixed = 3, variable = (2, 3, 4)
	2: fixed = 1, variable = (5)

	where the fixed part is the count of available CMIDs for each body
	part set, and the numbers in the variable part are the CMIDs for the 
	respective body part sets.  The default costume for this actor would 
	be CMID 0 for the shirt, 2 for the pants, and 5 for the head.

***************************************************************************/
#include "soc.h"
ASSERTNAME

RTCLASS(ACTN)
RTCLASS(TMPL)

/***************************************************************************
	Create a new action
***************************************************************************/
PACTN ACTN::PactnNew(PGG pggcel, PGL pglbmat34, ulong grfactn)
{
	AssertPo(pggcel, 0);
	AssertPo(pglbmat34, 0);

	PACTN pactn;

	pactn = NewObj ACTN;
	if (pvNil == pactn)
		goto LFail;
	pactn->_grfactn = grfactn;

	// Duplicate pggcel into pactn->_pggcel
	pactn->_pggcel = pggcel->PggDup();
	if (pvNil == pactn->_pggcel)	
		goto LFail;

	// Duplicate pglbmat34 into pactn->_pglbmat34
	pactn->_pglbmat34 = pglbmat34->PglDup();
	if (pvNil == pactn->_pglbmat34)
		goto LFail;

	AssertPo(pactn, 0);

	return pactn;
LFail:
	ReleasePpo(&pactn);
	return pvNil;
}


/***************************************************************************
	A PFNRPO (chunky resource reader function) to read an ACTN from a file
***************************************************************************/
bool ACTN::FReadActn(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, 0);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);

	ACTN *pactn;

	*pcb = pblck->Cb(fTrue);	// estimate ACTN size (not a good estimate)
	if (pvNil == ppbaco)
		return fTrue;

	if (!pblck->FUnpackData())
		goto LFail;
	*pcb = pblck->Cb();

	pactn = NewObj ACTN;
	if (pvNil == pactn || !pactn->_FInit(pcrf->Pcfl(), ctg, cno))
		{
		ReleasePpo(&pactn);
LFail:
		TrashVar(ppbaco);
		TrashVar(pcb);
		return fFalse;
		}
	AssertPo(pactn, 0);
	*ppbaco = pactn;
	*pcb = size(ACTN) + pactn->_pggcel->CbOnFile() +
		pactn->_pglbmat34->CbOnFile();
	return fTrue;
}


/***************************************************************************
	Read an ACTN from a chunk
***************************************************************************/
ACTN::_FInit(PCFL pcfl, CTG ctg, CNO cno)
{
	AssertPo(pcfl, 0);

	KID kid;
	BLCK blck;
	ACTNF actnf;
	short bo;
	long icel;

	if (!pcfl->FFind(ctg, cno, &blck) || !blck.FUnpackData())
		return fFalse;

	if (blck.Cb() < size(ACTNF))
		return fFalse;
	if (!blck.FReadRgb(&actnf, size(ACTNF), 0))
		return fFalse;
	if (kboOther == actnf.bo)
		SwapBytesBom(&actnf, kbomActnf);
	Assert(kboCur == actnf.bo, "bad ACTNF");
	_grfactn = actnf.grfactn;

	// read GG of cels (chid 0, ctg kctgGgcl):
	if (!pcfl->FGetKidChidCtg(ctg, cno, 0, kctgGgcl, &kid))
		return fFalse;
	if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		return fFalse;
	_pggcel = GG::PggRead(&blck, &bo);
	if (pvNil == _pggcel)
		return fFalse;
	AssertBomRglw(kbomCel, size(CEL));
	AssertBomRgsw(kbomCps, size(CPS));
	if (kboOther == bo)
		{
		for (icel = 0; icel < _pggcel->IvMac(); icel++)
			{
			SwapBytesRglw(_pggcel->QvFixedGet(icel), size(CEL) / size(long));
			SwapBytesRgsw(_pggcel->QvGet(icel), _pggcel->Cb(icel) /
				size(short));
			}
		}

	// read GL of transforms (chid 0, ctg kctgGlxf):
	if (!pcfl->FGetKidChidCtg(ctg, cno, 0, kctgGlxf, &kid))
		return fFalse;
	if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		return fFalse;
	_pglbmat34 = GL::PglRead(&blck, &bo);
	if (pvNil == _pglbmat34)
		return fFalse;
	AssertBomRglw(kbomBmat34, size(BMAT34));
	if (kboOther == bo)
		{
		SwapBytesRglw(_pglbmat34->QvGet(0), LwMul(_pglbmat34->IvMac(),
			size(BMAT34) / size(long)));
		}

	// read (optional) GL of motion-match sounds (chid 0, ctg kctgGlms):
	if (pcfl->FGetKidChidCtg(ctg, cno, 0, kctgGlms, &kid))
		{
		if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
			return fFalse;
		_pgltagSnd = GL::PglRead(&blck, &bo);
		if (pvNil == _pgltagSnd)
			return fFalse;
		AssertBomRglw(kbomTag, size(TAG));
		if (kboOther == bo)
			{
			SwapBytesRglw(_pgltagSnd->QvGet(0), LwMul(_pgltagSnd->IvMac(),
				size(TAG) / size(long)));
			}
		}

	return fTrue;
}


/***************************************************************************
	Destructor
***************************************************************************/
ACTN::~ACTN(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pggcel);
	ReleasePpo(&_pglbmat34);
}


/***************************************************************************
	Get a CEL
***************************************************************************/
void ACTN::GetCel(long icel, CEL *pcel)
{
	AssertThis(0);
	AssertIn(icel, 0, Ccel());
	AssertVarMem(pcel);

	_pggcel->GetFixed(icel, pcel);
}


/***************************************************************************
	Get a CPS
***************************************************************************/
void ACTN::GetCps(long icel, long icps, CPS *pcps)
{
	AssertThis(0);
	AssertIn(icel, 0, Ccel());
	AssertIn(icps, 0, _pggcel->Cb(icel) / size(CPS));
	AssertVarMem(pcps);

	CPS *prgcps = (CPS *)_pggcel->QvGet(icel);
	*pcps = prgcps[icps];
}


/***************************************************************************
	Get a sound for icel.  If there is no sound, ptag's CTG is set to 
	ctgNil.
***************************************************************************/
void ACTN::GetSnd(long icel, PTAG ptag)
{
	AssertThis(0);
	AssertIn(icel, 0, Ccel());
	AssertVarMem(ptag);

	CEL cel;

	ptag->ctg = ctgNil;
	if (pvNil != _pgltagSnd)
		{
		GetCel(icel, &cel);
		if (cel.chidSnd >= 0) // negative means no sound
			_pgltagSnd->Get(cel.chidSnd, ptag);
		}
}


/***************************************************************************
	Get a transformation matrix
***************************************************************************/
void ACTN::GetMatrix(long imat34, BMAT34 *pbmat34)
{
	AssertThis(0);
	AssertIn(imat34, 0, _pglbmat34->IvMac());
	AssertVarMem(pbmat34);

	_pglbmat34->Get(imat34, pbmat34);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the ACTN.
***************************************************************************/
void ACTN::AssertValid(ulong grf)
{
	ACTN_PAR::AssertValid(fobjAllocated);
	AssertPo(_pggcel, 0);
	AssertPo(_pglbmat34, 0);
	AssertNilOrPo(_pgltagSnd, 0);
}


/***************************************************************************
	Mark memory used by the ACTN
***************************************************************************/
void ACTN::MarkMem(void)
{
	AssertThis(0);
	ACTN_PAR::MarkMem();
	MarkMemObj(_pggcel);
	MarkMemObj(_pglbmat34);
	MarkMemObj(_pgltagSnd);
}
#endif //DEBUG


/***************************************************************************
	A PFNRPO (chunky resource reader function) to read TMPL objects.
***************************************************************************/
bool TMPL::FReadTmpl(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, 0);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);

	TMPL *ptmpl;
	KID kid;

	*pcb = pblck->Cb(fTrue); // estimate TMPL size (not a good estimate)
	if (pvNil == ppbaco)
		return fTrue;

	if (pcrf->Pcfl()->FGetKidChidCtg(ctg, cno, 0, kctgTdt, &kid))
		ptmpl = NewObj TDT;
	else
		ptmpl = NewObj TMPL;
	if (pvNil == ptmpl || !ptmpl->_FInit(pcrf->Pcfl(), ctg, cno))
		{
		ReleasePpo(&ptmpl);
		TrashVar(ppbaco);
		TrashVar(pcb);
		return fFalse;
		}
	AssertPo(ptmpl, 0);
	*ppbaco = ptmpl;
	if (ptmpl->_grftmpl & ftmplTdt)
		{
		*pcb = size(TDT);
		}
	else
		{
		*pcb = size(TMPL) + ptmpl->_pglibactPar->CbOnFile() +
			ptmpl->_pglibset->CbOnFile() + ptmpl->_pggcmid->CbOnFile();
		}
	return fTrue;
}

/***************************************************************************
	Read a TMPL from a chunk
***************************************************************************/
bool TMPL::_FReadTmplf(PCFL pcfl, CTG ctg, CNO cno)
{
	AssertBaseThis(0);

	BLCK blck;
	TMPLF tmplf;

	if (!pcfl->FFind(ctg, cno, &blck) || !blck.FUnpackData())
		return fFalse;

	if (blck.Cb() < size(TMPLF))
		return fFalse;
	if (!blck.FReadRgb(&tmplf, size(TMPLF), 0))
		return fFalse;

	if (kboOther == tmplf.bo)
		SwapBytesBom(&tmplf, kbomTmplf);
	Assert(kboCur == tmplf.bo, "freaky tmplf!");
	_xaRest = tmplf.xaRest;
	_yaRest = tmplf.yaRest;
	_zaRest = tmplf.zaRest;
	_grftmpl = tmplf.grftmpl;
	if (!pcfl->FGetName(ctg, cno, &_stn))
		return fFalse;
	return fTrue;
}

/***************************************************************************
	Write the TMPLF chunk.  Creates a new chunk and returns the CNO in pcno.
	
	Note: In Socrates, normal actor templates are read-only, but this
	function will get called for TDTs.
***************************************************************************/
bool TMPL::_FWriteTmplf(PCFL pcfl, CTG ctg, CNO *pcno)
{
	AssertThis(0);
	AssertPo(pcfl, 0);
	AssertVarMem(pcno);

	TMPLF tmplf;

	// Add TMPL chunk
	tmplf.bo = kboCur;
	tmplf.osk = koskCur;
	tmplf.xaRest = _xaRest;
	tmplf.yaRest = _yaRest;
	tmplf.zaRest = _zaRest;
	tmplf.grftmpl = _grftmpl;

	if (!pcfl->FAddPv(&tmplf, size(TMPLF), ctg, pcno))
		return fFalse;
	if (!pcfl->FSetName(kctgTmpl, *pcno, &_stn))
		{
		pcfl->Delete(ctg, *pcno);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Read a TMPL from a chunk
***************************************************************************/
TMPL::_FInit(PCFL pcfl, CTG ctg, CNO cno)
{
	AssertPo(pcfl, 0);

	KID kid;
	short bo;
	BLCK blck;
	long ibact;
	short ibset;

	if (!_FReadTmplf(pcfl, ctg, cno))
		return fFalse;
		
	// read GLPI (parent tree)
	if (!pcfl->FGetKidChidCtg(ctg, cno, 0, kctgGlpi, &kid))
		return fFalse;
	if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		return fFalse;
	_pglibactPar = GL::PglRead(&blck, &bo);
	if (pvNil == _pglibactPar)
		return fFalse;
	Assert(_pglibactPar->CbEntry() == size(short), "Bad _pglibactPar!");
	if (kboOther == bo)
		SwapBytesRgsw(_pglibactPar->QvGet(0), _pglibactPar->IvMac());

	// read GLBS (body-part-set ID list)
	if (!pcfl->FGetKidChidCtg(ctg, cno, 0, kctgGlbs, &kid))
		return fFalse;
	if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		return fFalse;
	_pglibset = GL::PglRead(&blck, &bo);
	if (pvNil == _pglibset)
		return fFalse;
	Assert(_pglibset->CbEntry() == size(short),	"Bad TMPL _pglibset!");
	if (kboOther == bo)
		SwapBytesRgsw(_pglibset->QvGet(0), _pglibset->IvMac());

#ifdef DEBUG
	// GLDC is obsolete
	if (pcfl->FGetKidChidCtg(ctg, cno, 0, kctgGldc, &kid))
		Warn("Obsolete GLDC structure...get rid of it");
#endif //DEBUG

	// _cbset is (highest entry in _pglibset) + 1.
	_cbset = -1;
	for (ibact = 0; ibact < _pglibset->IvMac(); ibact++)
		{
		_pglibset->Get(ibact, &ibset);
		if (ibset > _cbset)
			_cbset = ibset;
		}
	_cbset++;

	// Count custom costumes
	_ccmid = 0;
	while (pcfl->FGetKidChidCtg(ctg, cno, _ccmid, kctgCmtl, &kid))
		_ccmid++;

	// Count actions
	_cactn = 0;
	while (pcfl->FGetKidChidCtg(ctg, cno, _cactn, kctgActn, &kid))
		_cactn++;

	// read GGCM (costume info)
	if (!pcfl->FGetKidChidCtg(ctg, cno, 0, kctgGgcm, &kid))
		{
// REVIEW manders: temp until Pete updates sitobren
		goto LBuildGgcm;
//		return fFalse;
		}
	if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		return fFalse;
	_pggcmid = GG::PggRead(&blck, &bo);
	if (pvNil == _pggcmid)
		return fFalse;
	Assert(_pggcmid->CbFixed() == size(long), "Bad TMPL _pggcmid");
	Assert(_pggcmid->IvMac() == _cbset, "Bad TMPL _pggcmid");
	if (kboOther == bo)
		{
		for (ibset = 0; ibset < _cbset; ibset++)
			{
			SwapBytesRglw(_pggcmid->QvFixedGet(ibset), 1);
			SwapBytesRglw(_pggcmid->QvGet(ibset), 
				*(long *)_pggcmid->QvFixedGet(ibset));
			}
		}
	return fTrue;
// REVIEW manders: temp code until Pete converts our TMPL content
LBuildGgcm:
	long ikid;
	PCMTL pcmtl;
	PCRF pcrf;
	long rgcmid[50];
	long ccmid;

	Warn("missing GGCM...building one on the fly");

	pcrf = CRF::PcrfNew(pcfl, 0);
	if (pvNil == pcrf)
		return fFalse;

	_pggcmid = GG::PggNew(size(long));
	if (pvNil == _pggcmid)
		{
		ReleasePpo(&pcrf);
		return fFalse;
		}
	for (ibset = 0; ibset < _cbset; ibset++)
		{
		ikid = 0;
		ccmid = 0;

		while(pcfl->FGetKid(ctg, cno, ikid++, &kid))
			{
			if (kid.cki.ctg != kctgCmtl)
				continue;
			pcmtl = (PCMTL)pcrf->PbacoFetch(kid.cki.ctg, kid.cki.cno, CMTL::FReadCmtl);
			if (pvNil == pcmtl)	
				{
				ReleasePpo(&pcrf);
				return fFalse;
				}
			if (pcmtl->Ibset() == ibset)
				{
				rgcmid[ccmid++] = kid.chid;
				}	
			ReleasePpo(&pcmtl);
			}
		if (!_pggcmid->FAdd(ccmid * size(long), pvNil, rgcmid, &ccmid))
			{
			ReleasePpo(&pcrf);
			return fFalse;
			}
		}
	ReleasePpo(&pcrf);
	return fTrue;
}


/***************************************************************************
	Clean up and delete template
***************************************************************************/
TMPL::~TMPL(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pglibactPar);
	ReleasePpo(&_pglibset);
	ReleasePpo(&_pggcmid);
}


/***************************************************************************
	Return a list of all tags embedded in this TMPL.  Note that a
	return value of pvNil does not mean an error occurred, but simply that
	this TMPL has no embedded tags.
***************************************************************************/
PGL TMPL::PgltagFetch(PCFL pcfl, CTG ctg, CNO cno, bool *pfError)
{
	AssertPo(pcfl, 0);
	AssertVarMem(pfError);

	KID kid;

	*pfError = fFalse;
	if (pcfl->FGetKidChidCtg(ctg, cno, 0, kctgTdt, &kid))
		return TDT::PgltagFetch(pcfl, ctg, cno, pfError);
	else
		return pvNil; // standard TMPLs have no embedded tags
}


/***************************************************************************
	Creates a new tree of body parts (br_actors) based on this template.
	Note: ACTR also calls FSetDefaultCost after creating the body, but
	by calling it here, it is guaranteed that the body will have a material
	on each body part (no null pointers for bact->material).  So the user
	will never see a body part that isn't texture mapped.
***************************************************************************/
PBODY TMPL::PbodyCreate(void)
{
	AssertThis(0);
	PBODY pbody = BODY::PbodyNew(_pglibactPar, _pglibset);

	if (pvNil == pbody || !FSetDefaultCost(pbody))
		{
		ReleasePpo(&pbody);
		return pvNil;
		}
	return pbody;
}


/***************************************************************************
	Fills in the name of the given action
***************************************************************************/
bool TMPL::FGetActnName(long anid, PSTN pstn)
{
	AssertThis(0);
	AssertIn(anid, 0, _cactn);
	AssertPo(pstn, 0);
	
	KID kid;

	if (!Pcrf()->Pcfl()->FGetKidChidCtg(Ctg(), Cno(), anid, kctgActn, &kid))
		return fFalse;
	return Pcrf()->Pcfl()->FGetName(kid.cki.ctg, kid.cki.cno, pstn);
}


/***************************************************************************
	Reads an ACTN chunk from disk
***************************************************************************/
PACTN TMPL::_PactnFetch(long anid)
{
	AssertThis(0);
	AssertIn(anid, 0, _cactn);

	KID kid;
	ACTN *pactn;
	CHID chidActn = anid;

	if (!Pcrf()->Pcfl()->FGetKidChidCtg(Ctg(), Cno(), chidActn, kctgActn,
		&kid))
		{
		return pvNil;
		}
	pactn = (ACTN *)Pcrf()->PbacoFetch(kid.cki.ctg, kid.cki.cno,
		ACTN::FReadActn);
	AssertNilOrPo(pactn, 0);
	return pactn;
}


/***************************************************************************
	Reads a MODL chunk from disk
***************************************************************************/
PMODL TMPL::_PmodlFetch(CHID chidModl)
{
	AssertThis(0);

	KID kid;
	MODL *pmodl;

	if (!Pcrf()->Pcfl()->FGetKidChidCtg(Ctg(), Cno(), chidModl, kctgBmdl,
		&kid))
		{
		return pvNil;
		}
	pmodl = (MODL *)Pcrf()->PbacoFetch(kid.cki.ctg, kid.cki.cno,
		MODL::FReadModl);
	AssertNilOrPo(pmodl, 0);
	return pmodl;
}


/***************************************************************************
	Sets up the body part tree to use the correct models and transformation
	matrices for the given cel of the given action.  Also returns the
	distance to the next cel in *pdwr.
***************************************************************************/
bool TMPL::FSetActnCel(BODY *pbody, long anid, long celn, BRS *pdwr)
{
	AssertThis(0);
	AssertPo(pbody, 0);
	AssertIn(anid, 0, _cactn);
	AssertNilOrVarMem(pdwr);

	long icel;
	ACTN *pactn = pvNil;
	CEL cel;
	short ibprt;
	long cbprt = _pglibactPar->IvMac();
	CPS cps;
	PMODL *prgpmodl = pvNil;
	BMAT34 bmat34;
	bool fRet = fFalse;

	pactn = _PactnFetch(anid);
	if (pvNil == pactn)	
		goto LEnd;
	icel = celn % pactn->Ccel();
	if (icel < 0)
		icel += pactn->Ccel();
	pactn->GetCel(icel, &cel);

	if (!FAllocPv((void **)&prgpmodl, LwMul(cbprt, size (PMODL)), fmemClear,
		mprNormal))
		{
		goto LEnd;
		}
	for (ibprt = 0; ibprt < cbprt; ibprt++)
		{
		pactn->GetCps(icel, ibprt, &cps);
		if (chidNil == cps.chidModl) 
			{
			// Appendages for accessories...don't smash
			// accessory models with action models
			Assert(pvNil == prgpmodl[ibprt], "fmemClear didn't work?");
			}
		else
			{
			prgpmodl[ibprt] = _PmodlFetch(cps.chidModl);
			if (pvNil == prgpmodl[ibprt])
				goto LEnd;
			}	
		}

	if (pvNil != pdwr)
		*pdwr = cel.dwr;
	for (ibprt = 0; ibprt < cbprt; ibprt++)
		{
		if (pvNil != prgpmodl[ibprt])
			pbody->SetPartModel(ibprt, prgpmodl[ibprt]);
		pactn->GetCps(icel, ibprt, &cps);
		pactn->GetMatrix(cps.imat34, &bmat34);
		pbody->SetPartMatrix(ibprt, &bmat34);
		}
	fRet = fTrue;
LEnd:
	if (pvNil != prgpmodl)
		{
		for (ibprt = 0; ibprt < cbprt; ibprt++)
			ReleasePpo(&prgpmodl[ibprt]);
		FreePpv((void **)&prgpmodl);
		}
	ReleasePpo(&pactn);
	return fRet;
}


/***************************************************************************
	Retrieves the distance travelled by cel celn of action anid.
***************************************************************************/
bool TMPL::FGetDwrActnCel(long anid, long celn, BRS *pdwr)
{
	AssertThis(0);
	AssertIn(anid, 0, _cactn);
	AssertVarMem(pdwr);

	ACTN *pactn;
	long icel;
	CEL cel;

	pactn = _PactnFetch(anid);
	if (pvNil == pactn)
		return fFalse;
	icel = celn % pactn->Ccel();
	if (icel < 0)
		icel += pactn->Ccel();
	pactn->GetCel(icel, &cel);
	ReleasePpo(&pactn);
	*pdwr = cel.dwr;
	return fTrue;
}


/***************************************************************************
	Retrieves the number of cels in this action
***************************************************************************/
bool TMPL::FGetCcelActn(long anid, long *pccel)
{
	AssertThis(0);
	AssertIn(anid, 0, _cactn);
	AssertVarMem(pccel);

	ACTN *pactn;

	pactn = _PactnFetch(anid);
	if (pvNil == pactn)
		return fFalse;
	*pccel = pactn->Ccel();
	ReleasePpo(&pactn);
	return fTrue;
}


/***************************************************************************
	Retrieves the number of cels in this action
***************************************************************************/
bool TMPL::FGetSndActnCel(long anid, long celn, bool *pfSoundExists, 
	PTAG ptag)
{
	AssertThis(0);
	AssertIn(anid, 0, _cactn);
	AssertVarMem(pfSoundExists);
	AssertVarMem(ptag);

	ACTN *pactn;
	long icel; 

	*pfSoundExists = fFalse; 
	pactn = _PactnFetch(anid);
	if (pvNil == pactn)
		return fFalse;
	icel = celn % pactn->Ccel();
	if (icel < 0)
		icel += pactn->Ccel();
	pactn->GetSnd(icel, ptag);
	if (ptag->ctg != ctgNil)
		*pfSoundExists = fTrue;
	ReleasePpo(&pactn);
	return fTrue;
}




/***************************************************************************
	Retrieves the distance travelled by cel celn of action anid.
***************************************************************************/
bool TMPL::FGetGrfactn(long anid, ulong *pgrfactn)
{
	AssertThis(0);
	AssertIn(anid, 0, _cactn);
	AssertVarMem(pgrfactn);

	ACTN *pactn;

	pactn = _PactnFetch(anid);
	if (pvNil == pactn)
		return fFalse;
	*pgrfactn = pactn->Grfactn();
	ReleasePpo(&pactn);
	return fTrue;
}


/***************************************************************************
	Get orientation for template when actor has no path
***************************************************************************/
void TMPL::GetRestOrien(BRA *pxa, BRA *pya, BRA *pza)
{
	AssertThis(0);
	AssertVarMem(pxa);
	AssertVarMem(pya);
	AssertVarMem(pza);

	*pxa = _xaRest;
	*pya = _yaRest;
	*pza = _zaRest;
}


/***************************************************************************
	Puts default costume on pbody
***************************************************************************/
bool TMPL::FSetDefaultCost(BODY *pbody)
{
	AssertThis(0);
	AssertPo(pbody, 0);

	long ibset;
	long cmid;
	PCMTL *prgpcmtl;
	bool fRet = fFalse;

	if (!FAllocPv((void **)&prgpcmtl, LwMul(_cbset, size(PCMTL)), fmemClear,
		mprNormal))
		{
		goto LEnd;
		}
	for (ibset = 0; ibset < _cbset; ibset++)
		{
		cmid = CmidOfBset(ibset, 0);
		prgpcmtl[ibset] = PcmtlFetch(cmid);
		if (pvNil == prgpcmtl[ibset])
			goto LEnd;
		Assert(prgpcmtl[ibset]->Ibset() == ibset, "ibset's don't match");
		}
	for (ibset = 0; ibset < _cbset; ibset++)
		pbody->SetPartSetCmtl(prgpcmtl[ibset]);
	fRet = fTrue;
LEnd:
	if (pvNil != prgpcmtl)
		{
		for (ibset = 0; ibset < _cbset; ibset++)
			ReleasePpo(&prgpcmtl[ibset]);
		FreePpv((void **)&prgpcmtl);
		}
	return fRet;
}


/***************************************************************************
	Returns the number of custom materials available for ibset
***************************************************************************/
long TMPL::CcmidOfBset(long ibset)
{
	AssertThis(0);
	AssertIn(ibset, 0, _cbset);

	return *(long *)_pggcmid->QvFixedGet(ibset);
}


/***************************************************************************
	Returns the icmid'th CMID available for ibset
***************************************************************************/
long TMPL::CmidOfBset(long ibset, long icmid)
{
	AssertThis(0);
	AssertIn(ibset, 0, _cbset);
	AssertIn(icmid, 0, CcmidOfBset(ibset));

	long *prgcmid;
	
	prgcmid = (long *)_pggcmid->QvGet(ibset);
	return prgcmid[icmid];
}


/***************************************************************************
	Tells whether ibset holds accessories by checking to see if one of 
	its costumes has model children.
***************************************************************************/
bool TMPL::FBsetIsAccessory(long ibset)
{
	AssertThis(0);
	AssertIn(ibset, 0, _cbset);

	long cmid;
	KID kid;

	if (pvNil == Pcrf())
		return fFalse; // probably a TDT

	cmid = CmidOfBset(ibset, 0);
	if (!Pcrf()->Pcfl()->FGetKidChidCtg(Ctg(), Cno(), cmid, kctgCmtl,
		&kid))
		{
		return fFalse;
		}

	return CMTL::FHasModels(Pcrf()->Pcfl(), kid.cki.ctg, kid.cki.cno);
}


/***************************************************************************
	Returns the ibset of the accessory associated with ibset, if any.  If
	ibset is itself an accessory, it is returned in *pibsetAcc.  Otherwise,
	if ibset is the parent of an accessory, that accessory is returned in
	*pibsetAcc.
***************************************************************************/
bool TMPL::FIbsetAccOfIbset(long ibset, long *pibsetAcc)
{
	AssertThis(0);
	AssertIn(ibset, 0, _cbset);
	AssertVarMem(pibsetAcc);

	long ibsetT;
	long ibact;
	long ibactPar;
	short ibsetOfIbact;
	short ibsetOfIbactPar;

	if (FBsetIsAccessory(ibset))
		{
		*pibsetAcc = ibset;
		return fTrue;
		}

	for (ibsetT = 0; ibsetT < _cbset; ibsetT++)
		{
		if (FBsetIsAccessory(ibsetT))
			{
			// for each ibact in ibsetT, see if its parent is in ibset
			for (ibact = 0; ibact < _pglibactPar->IvMac(); ibact++)
				{
				ibsetOfIbact = *(short *)_pglibset->QvGet(ibact);
				if (ibsetT == ibsetOfIbact)
					{
					// see if ibact's parent in ibset
					ibactPar = *(short *)_pglibactPar->QvGet(ibact);
					ibsetOfIbactPar = *(short *)_pglibset->QvGet(ibactPar);
					if (ibsetOfIbactPar == ibset)
						{
						// so ibset is a parent bset of ibsetT
						*pibsetAcc = ibsetT;
						return fTrue;
						}
					}
				}
			}
		}
	return fFalse; // ibset is not a parent bset of any accessory bset.
}


/***************************************************************************
	See if cmid1 and cmid2 are for the same accessory by comparing child
	model chunks
***************************************************************************/
bool TMPL::FSameAccCmids(long cmid1, long cmid2)
{
	AssertThis(0);

	KID kid1;
	KID kid2;

	if (!Pcrf()->Pcfl()->FGetKidChidCtg(Ctg(), Cno(), cmid1, kctgCmtl,
		&kid1) || 
		!Pcrf()->Pcfl()->FGetKidChidCtg(Ctg(), Cno(), cmid2, kctgCmtl,
		&kid2))
		{
		return fFalse; // safer to assume they're different
		}
	return CMTL::FEqualModels(Pcrf()->Pcfl(), kid1.cki.cno, kid2.cki.cno);
}


/***************************************************************************
	Get a custom material.  The cmid is really the chid under the TMPL.
***************************************************************************/
PCMTL TMPL::PcmtlFetch(long cmid)
{
	AssertThis(0);
	AssertIn(cmid, 0, _ccmid);

	PCMTL pcmtl;
	KID kid;

	if (!Pcrf()->Pcfl()->FGetKidChidCtg(Ctg(), Cno(), cmid, kctgCmtl,
		&kid))
		{
		return pvNil;
		}
	pcmtl = (PCMTL)Pcrf()->PbacoFetch(kid.cki.ctg, kid.cki.cno,
		CMTL::FReadCmtl);
	AssertNilOrPo(pcmtl, 0);
	return pcmtl;
}


/***************************************************************************
	Puts the template's name into pstn
***************************************************************************/
void TMPL::GetName(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	*pstn = _stn;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the TMPL.
***************************************************************************/
void TMPL::AssertValid(ulong grftmpl)
{
	long ibset;
	long ccmid;

	TMPL_PAR::AssertValid(fobjAllocated);
	AssertPo(_pglibactPar, 0);
	AssertPo(_pglibset, 0);
	AssertPo(_pggcmid, 0);
	AssertPo(&_stn, 0);

	// Verify correctness of _pggcmid
	Assert(_pggcmid->IvMac() == _cbset, 0);
	for (ibset = 0; ibset < _cbset; ibset++)
		{
		ccmid = *(long *)_pggcmid->QvFixedGet(ibset);
		Assert(_pggcmid->Cb(ibset) / size(long) == ccmid, 0);
		}
}


/***************************************************************************
	Mark memory used by the TMPL
***************************************************************************/
void TMPL::MarkMem(void)
{
	AssertThis(0);
	TMPL_PAR::MarkMem();
	MarkMemObj(_pglibactPar);
	MarkMemObj(_pglibset);
	MarkMemObj(_pggcmid);
}
#endif //DEBUG
