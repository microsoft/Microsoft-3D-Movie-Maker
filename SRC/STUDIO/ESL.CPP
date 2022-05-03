/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	esl.cpp: Easel classes

	Primary Author: ******
	Listener Easel: *****
	Review Status: REVIEWED - any changes to this file must be reviewed!

	ESL is the base class for all easels.  It handles cidEaselOk and
	cidEaselCancel commands, and calls the virtual _FAcceptChanges()
	function for cidEaselOk.

	Custom easels have their own command maps to handle specific messages,
	and implement _FAcceptChanges() to do the right thing.

***************************************************************************/
#include "studio.h"
ASSERTNAME

const long kcchMaxTdt = 50; // max length of a spletter
const long kdtsMaxRecord = 10 * kdtsSecond; // max time to record a sound
const long kdtimMeterUpdate = kdtimSecond / 10; // interval to update meter
const long kcsampSec = 11025; // sampling rate for recorder easel
long csampSec; // sampling rate for recorder easel

RTCLASS(ESL)
RTCLASS(ESLT)
RTCLASS(ESLA)
RTCLASS(SNE)
RTCLASS(ESLL)
RTCLASS(LSND)
RTCLASS(ESLR)

/***************************************************************************
	Function to build a GCB for creating a child GOB
***************************************************************************/
bool FBuildGcb(PGCB pgcb, long kidParent, long kidChild)
{
	AssertVarMem(pgcb);

	PGOK pgokPar;
	RC rcRel;

	pgokPar = (PGOK)vpapp->Pkwa()->PgobFromHid(kidParent);
	if (pvNil == pgokPar)
		{
		TrashVar(pgcb);
		return fFalse;
		}

	rcRel.Set(krelZero, krelZero, krelOne, krelOne);
	pgcb->Set(kidChild, pgokPar, fgobNil, kginDefault, pvNil, &rcRel);
	return fTrue;
}


/***************************************************************************
	Sets the given gok to the given state
***************************************************************************/
void SetGokState(long kid, long st)
{
	PGOK pgok;

	pgok = (PGOK)vpapp->Pkwa()->PgobFromHid(kid);
	if (pvNil != pgok && pgok->FIs(kclsGOK) && pgok->Sno() != st)
		pgok->FChangeState(st); // ignore failure
}

//
//
//
//  ESL (generic easel) stuff begins here
//
//
//


BEGIN_CMD_MAP(ESL, GOK)
	ON_CID_GEN(cidEaselOk, FCmdDismiss, pvNil)
	ON_CID_GEN(cidEaselCancel, FCmdDismiss, pvNil)
END_CMD_MAP_NIL()


/***************************************************************************
	Create a new easel
***************************************************************************/
PESL ESL::PeslNew(PRCA prca, long kidParent, long kidEasel)
{
	AssertPo(prca, 0);

	GCB gcb;
	PESL pesl;

	if (!FBuildGcb(&gcb, kidParent, kidEasel))
		return pvNil;
	pesl = NewObj ESL(&gcb);
	if (pvNil == pesl)
		return pvNil;
	if (!pesl->_FInit(prca, kidEasel))
		{
		ReleasePpo(&pesl);
		return pvNil;
		}
	AssertPo(pesl, 0);
	vpcex->EnqueueCid(cidEaselVisible);
	return pesl;
}


/***************************************************************************
	Initialize the easel and make it visible
***************************************************************************/
bool ESL::_FInit(PRCA prca, long kidEasel)
{
	AssertBaseThis(0);
	AssertPo(prca, 0);

	if (!ESL_PAR::_FInit(vpapp->Pkwa(), kidEasel, prca))
		return fFalse;
	if (!_FEnterState(ksnoInit))
		return fFalse;

	vpapp->DisableAccel();

	STDIO::PauseActionButton();

	return fTrue;
}


/***************************************************************************
	Clean up and delete this easel
***************************************************************************/
ESL::~ESL(void)
{
	AssertBaseThis(0);

	vpapp->EnableAccel();
	STDIO::ResumeActionButton();
}


/***************************************************************************
	Dismiss and delete this easel
***************************************************************************/
bool ESL::FCmdDismiss(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (pcmd->cid == cidEaselOk)
		{
		bool fDismissEasel = fTrue;

		// Could return fTrue here if _FAcceptChanges fails to abort
		// dismissal of easel, but instead we release anyway to be
		// consistent with browsers.
		_FAcceptChanges(&fDismissEasel);

		// If we did not accept the changes, (but nothing failed),
		// then we will not dismiss the easel.
		if(!fDismissEasel)
			return fTrue;

		}
	Release(); // destroys entire gob tree
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the ESL.
***************************************************************************/
void ESL::AssertValid(ulong grf)
{
	ESL_PAR::AssertValid(fobjAllocated);
}


/***************************************************************************
	Mark memory used by the ESL
***************************************************************************/
void ESL::MarkMem(void)
{
	AssertThis(0);
	ESL_PAR::MarkMem();
}
#endif //DEBUG


//
//
//
//  ESLT (text easel) stuff begins here
//
//
//


BEGIN_CMD_MAP(ESLT, ESL)
	ON_CID_GEN(cidEaselRotate, FCmdRotate, pvNil)
	ON_CID_GEN(cidEaselTransmogrify, FCmdTransmogrify, pvNil)
	ON_CID_GEN(cidEaselFont, FCmdStartPopup, pvNil)
	ON_CID_GEN(cidEaselShape, FCmdStartPopup, pvNil)
	ON_CID_GEN(cidEaselTexture, FCmdStartPopup, pvNil)
	ON_CID_GEN(cidEaselSetFont, FCmdSetFont, pvNil)
	ON_CID_GEN(cidEaselSetShape, FCmdSetShape, pvNil)
	ON_CID_GEN(cidEaselSetColor, FCmdSetColor, pvNil)
END_CMD_MAP_NIL()


/***************************************************************************
	Create a new text easel.  If pactr is pvNil, this is for a new TDT
	and pstnNew, tdtsNew, and ptagTdfNew will be used as initial values.
***************************************************************************/
PESLT ESLT::PesltNew(PRCA prca, PMVIE pmvie, PACTR pactr, PSTN pstnNew,
	long tdtsNew, PTAG ptagTdfNew)
{
	AssertPo(prca, 0);
	AssertPo(pmvie, 0);
	AssertNilOrPo(pactr, 0);
	AssertNilOrPo(pstnNew, 0);
	if (tdtsNew != tdtsNil)
		AssertIn(tdtsNew, 0, tdtsLim);
	AssertNilOrVarMem(ptagTdfNew);

	PESLT peslt;
	GCB gcb;

	if (!FBuildGcb(&gcb, kidBackground, kidSpltGlass))
		return pvNil;

	peslt = NewObj ESLT(&gcb);
	if (pvNil == peslt)
		return pvNil;
	if (!peslt->_FInit(prca, kidSpltGlass, pmvie, pactr, pstnNew,
		tdtsNew, ptagTdfNew))
		{
		ReleasePpo(&peslt);
		return pvNil;
		}
	AssertPo(peslt, 0);
	vpcex->EnqueueCid(cidEaselVisible);

	return peslt;
}


/***************************************************************************
	Set up this easel
***************************************************************************/
bool ESLT::_FInit(PRCA prca, long kidEasel, PMVIE pmvie, PACTR pactr,
	PSTN pstnNew, long tdtsNew, PTAG ptagTdfNew)
{
	AssertBaseThis(0);
	AssertPo(prca, 0);
	AssertPo(pmvie, 0);
	AssertNilOrPo(pactr, 0);
	Assert(pvNil == pactr || pactr->Ptmpl()->FIsTdt(),
		"only use ESLT for TDTs");
	AssertNilOrPo(pstnNew, 0);
	if (tdtsNew != tdtsNil)
		AssertIn(tdtsNew, 0, tdtsLim);
	AssertNilOrVarMem(ptagTdfNew);

	GCB gcb;
	COST cost;
	STN stn;
	bool fNewTdt = (pactr == pvNil);
	PTDT ptdt;

	_prca = prca;
	_pactr = pactr;
	_pmvie = pmvie;

	if (!ESLT_PAR::_FInit(prca, kidEasel))
		return fFalse;
	if (!FBuildGcb(&gcb, kidSpltPreviewFrame, CMH::HidUnique()))
		return fFalse;

	_psflTdts = NewObj SFL;
	if (pvNil == _psflTdts)
		return fFalse;

	_psflTdf = NewObj SFL;
	if (pvNil == _psflTdf)
		return fFalse;

	_psflMtrl = NewObj SFL;
	if (pvNil == _psflMtrl)
		return fFalse;

	if (fNewTdt)
		{
		AssertPo(pstnNew, 0);
		AssertIn(tdtsNew, 0, tdtsLim);
		AssertVarMem(ptagTdfNew);

		ptdt = TDT::PtdtNew(pstnNew, tdtsNew, ptagTdfNew);
		if (pvNil == ptdt)	
			return fFalse;
		_pape = APE::PapeNew(&gcb, ptdt, pvNil, 0, fFalse, _prca);
		ReleasePpo(&ptdt);
		if (pvNil == _pape)
			return fFalse;
		stn = *pstnNew;
		}
	else
		{
		// We have to make a duplicate TDT rather than use the existing one
		// because the user may experimentally change the TDT, then hit Cancel.
		if (!cost.FGet(pactr->Pbody()))
			return fFalse;
		ptdt = ((PTDT)pactr->Ptmpl())->PtdtDup();
		if (pvNil == ptdt)	
			return fFalse;
		_pape = APE::PapeNew(&gcb, ptdt, &cost, 0, fFalse, _prca);
		ReleasePpo(&ptdt);
		if (pvNil == _pape)
			return fFalse;
		pactr->Ptmpl()->GetName(&stn);
		}

	// Set up the edit box
	if (!FBuildGcb(&gcb, kidSpltEditBox, CMH::HidUnique()))
		return fFalse;
	EDPAR edpar(gcb._hid, gcb._pgob, gcb._grfgob, gcb._gin, &gcb._rcAbs,
		&gcb._rcRel, vpappb->OnnDefVariable(), 0, vpappb->DypTextDef(),
		tahLeft, tavCenter);
	_psne = SNE::PsneNew(&edpar, this, &stn);
	if (pvNil == _psne)
		return fFalse;
	_psne->Activate(fTrue);
	_psne->SetSel(0, _psne->IchMac(), fFalse); // select all of the text
	_psne->SetCursCno(_prca, kcrsIBeam);

	return fTrue;
}


/***************************************************************************
	Clean up and delete this easel.  Note that I don't need to delete
	_psne or _pape because they're GOBs and are automatically destroyed
	with the gob tree.
***************************************************************************/
ESLT::~ESLT(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_psflTdts);
	ReleasePpo(&_psflTdf);
	ReleasePpo(&_psflMtrl);
	ReleasePpo(&_pbclTdf);
	ReleasePpo(&_pbclMtrl);
}


/***************************************************************************
	Handle a rotate command
***************************************************************************/
bool ESLT::FCmdRotate(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	_pape->ChangeView();

	return fTrue;	
}


/***************************************************************************
	Handle a transmogrify command (pick a random shape, font, and material)
***************************************************************************/
bool ESLT::FCmdTransmogrify(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	long tdts;
	CKI cki;
	long ithd;
	THD thd;
	TAG tagTdf;
	TAG tagMtrl;
	
	vpappb->BeginLongOp();

	// Pick a random shape
	tdts = _psflTdts->LwNext(tdtsLim);

	// Pick a random font
	if (pvNil == _pbclTdf)
		{
		cki.ctg = kctgTfth;
		cki.cno = cnoNil;
		_pbclTdf = BCL::PbclNew(pvNil, &cki, ctgNil, pvNil, fTrue);
		}
	if (_pbclTdf != pvNil && _pbclTdf->IthdMac() != 0)
		{
		ithd = _psflTdf->LwNext(_pbclTdf->IthdMac());
		_pbclTdf->GetThd(ithd, &thd);
		tagTdf = thd.tag;
		if (vptagm->FCacheTagToHD(&tagTdf))
			{
			// Failure here is harmless
			_pape->FChangeTdt(pvNil, tdts, &tagTdf);
			}
		}

	// Pick a random material
	if (pvNil == _pbclMtrl)
		{
		cki.ctg = kctgMtth;
		cki.cno = cnoNil;
		_pbclMtrl = BCL::PbclNew(pvNil, &cki, ctgNil, pvNil, fTrue);
		}
	if (_pbclMtrl != pvNil && _pbclMtrl->IthdMac() != 0)
		{
		ithd = _psflMtrl->LwNext(_pbclMtrl->IthdMac());
		_pbclMtrl->GetThd(ithd, &thd);
		tagMtrl = thd.tag;
		if (vptagm->FCacheTagToHD(&tagMtrl))
			{
			// Failure here is harmless
			_pape->FSetTdtMtrl(&tagMtrl);
			}
		}

	vpappb->EndLongOp();

	return fTrue;	
}


/***************************************************************************
	Start a popup
***************************************************************************/
bool ESLT::FCmdStartPopup(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	CKI ckiGPar;
	long kid;
	long ithumSelect = ivNil;
	long sidSelect = vpapp->SidProduct();
	long cidSelect;
	TAG tagTdf;
	long tdts;
	CNO cnoSelect;

	ckiGPar.cno = cnoNil;

	switch (pcmd->cid)
		{
	case cidEaselFont:
		ckiGPar.ctg = kctgTfth;
		kid = kidSpltsFont;
		_pape->GetTdtInfo(pvNil, pvNil, &tagTdf);
		ithumSelect = tagTdf.cno;
		sidSelect = tagTdf.sid;
		cidSelect = cidEaselSetFont;
		break;

	case cidEaselShape:
		ckiGPar.ctg = kctgTsth;
		kid = kidSpltsShape;
		_pape->GetTdtInfo(pvNil, &tdts, pvNil);
		ithumSelect = tdts;
		cidSelect = cidEaselSetShape;
		break;

	case cidEaselTexture:
		ckiGPar.ctg = kctgMtth;
		if (_pape->FGetTdtMtrlCno(&cnoSelect))
			ithumSelect = cnoSelect;
		else
			ithumSelect = ivNil;
		kid = kidSpltsColor;
		cidSelect = cidEaselSetColor;
		break;

	default:
		Bug("Invalid cid for ESLT::FCmdStartPopup");
		break;
		}

	MP::PmpNew(kidSpltBackground, kid, _prca, pcmd, kbwsCnoRoot, ithumSelect,
		sidSelect, ckiGPar, ctgNil, this, cidSelect, fTrue);

	return fTrue;
}


/***************************************************************************
	Handle a command to change the font
***************************************************************************/
bool ESLT::FCmdSetFont(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	TAG tagTdfOld;
	TAG tagTdfNew;

	tagTdfNew.sid = pcmd->rglw[1];
	tagTdfNew.pcrf = pvNil;
	tagTdfNew.ctg = kctgTdf;
	tagTdfNew.cno = pcmd->rglw[0];

	_pape->GetTdtInfo(pvNil, pvNil, &tagTdfOld);
	if (fcmpEq == vptagm->FcmpCompareTags(&tagTdfOld, &tagTdfNew))
		return fTrue; // nothing to do

	// If FChangeTdt fails, someone will report the error
	if (vptagm->FCacheTagToHD(&tagTdfNew))
		_pape->FChangeTdt(pvNil, tdtsNil, &tagTdfNew);

	return fTrue;
}


/***************************************************************************
	Handle a command to change the shape
***************************************************************************/
bool ESLT::FCmdSetShape(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	long tdtsOld;
	long tdtsNew = pcmd->rglw[0];

	_pape->GetTdtInfo(pvNil, &tdtsOld, pvNil);
	// If FChangeTdt fails, someone will report the error
	if (tdtsOld != tdtsNew)
		_pape->FChangeTdt(pvNil, tdtsNew, pvNil);

	return fTrue;
}


/***************************************************************************
	Handle a command to change the color
***************************************************************************/
bool ESLT::FCmdSetColor(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	TAG tagTdfNew;

	tagTdfNew.sid = pcmd->rglw[1];
	tagTdfNew.pcrf = pvNil;
	tagTdfNew.ctg = kctgMtrl;
	tagTdfNew.cno = pcmd->rglw[0];

	// If FSetTdtMtrl fails, someone will report the error
	if (vptagm->FCacheTagToHD(&tagTdfNew))
		_pape->FSetTdtMtrl(&tagTdfNew);

	return fTrue;
}


/***************************************************************************
	Update the APE when the text of the SNE changed
***************************************************************************/
bool ESLT::FTextChanged(PSTN pstnNew)
{
	AssertBaseThis(0); // we may still be setting up the ESLT
	AssertPo(pstnNew, 0);

	return _pape->FChangeTdt(pstnNew, tdtsNil, pvNil);
}


/***************************************************************************
	Make the changes to pactr
***************************************************************************/
bool ESLT::_FAcceptChanges(bool *pfDismissEasel)
{
	AssertThis(0);
	AssertVarMem(pfDismissEasel);

	long ich;
	bool fNonSpaceFound = fFalse;
	PACTR pactrDup = pvNil;
	bool fChangesMade = fFalse;
	PTDT ptdtOld;
	STN stnOld;
	long tdtsOld;
	TAG tagTdfOld;
	STN stnNew;
	long tdtsNew;
	TAG tagTdfNew;
	long cbset;
	long ibset;
	bool fMtrl;
	long cmid;
	TAG tagMtrl;

	if (pvNil == _pactr) // new TDT
		{
		_pape->GetTdtInfo(&stnNew, &tdtsNew, &tagTdfNew);
		for (ich = 0; ich < stnNew.Cch(); ich++)
			{
			if (stnNew.Psz()[ich] != ChLit(' '))
				{
				fNonSpaceFound = fTrue;
				break;
				}
			}
		if (!fNonSpaceFound)
			{
			// user deleted all text, so treat like a cancel
			vpcex->EnqueueCid(cidEaselClosing, pvNil, pvNil, fFalse);
			return fTrue;
			}
		if (!_pmvie->FInsTdt(&stnNew, tdtsNew, &tagTdfNew))
			goto LFail;
		_pactr = _pmvie->Pscen()->PactrSelected();

		cbset = _pape->Cbset();
		// Now apply costume changes
		for (ibset = 0; ibset < cbset; ibset++)
			{
			if (_pape->FGetMaterial(ibset, &fMtrl, &cmid, &tagMtrl))
				{
				if (!_pactr->FSetCostumeCore(ibset, &tagMtrl, cmid, !fMtrl))
					goto LFail;
				fChangesMade = fTrue;
				}
			}
		_pmvie->PmvuCur()->StartPlaceActor();
		}
	else
		{
		ptdtOld = (PTDT)_pactr->Ptmpl();

		if (!_pactr->FDup(&pactrDup))
			goto LFail;

		// first apply TDT changes
		ptdtOld->GetInfo(&stnOld, &tdtsOld, &tagTdfOld);
		_pape->GetTdtInfo(&stnNew, &tdtsNew, &tagTdfNew);
		if (!stnOld.FEqual(&stnNew) || tdtsOld != tdtsNew ||
			fcmpEq != TAGM::FcmpCompareTags(&tagTdfOld, &tagTdfNew))
			{
			if (!_pmvie->FChangeActrTdt(_pactr, &stnNew, tdtsNew, &tagTdfNew))
				goto LFail;
			fChangesMade = fTrue;
			}

		// Now apply costume changes
		cbset = _pape->Cbset();
		for (ibset = 0; ibset < cbset; ibset++)
			{
			if (_pape->FGetMaterial(ibset, &fMtrl, &cmid, &tagMtrl))
				{
				if (!_pactr->FSetCostumeCore(ibset, &tagMtrl, cmid, !fMtrl))
					goto LFail;
				fChangesMade = fTrue;
				}
			}

		if (fChangesMade)
			{
			if (!_pactr->FCreateUndo(pactrDup))
				goto LFail;
			}
		ReleasePpo(&pactrDup);
		}
	vpcex->EnqueueCid(cidEaselClosing, pvNil, pvNil, fTrue);
	_pmvie->Pscen()->SelectActr(_pactr);
	return fTrue;
LFail:
	if (_pactr != pvNil)
		{
		if (pvNil != pactrDup)
			{	
			_pactr->Restore(pactrDup);
			ReleasePpo(&pactrDup);
			}
		PushErc(ercSoc3DWordChange);
		}
	else
		PushErc(ercSoc3DWordCreate);
	return fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the ESLT.
***************************************************************************/
void ESLT::AssertValid(ulong grf)
{
	ESLT_PAR::AssertValid(fobjAllocated);
	AssertPo(_pmvie, 0);
	AssertNilOrPo(_pactr, 0);
	AssertPo(_pape, 0);
	AssertPo(_psne, 0);
	AssertPo(_prca, 0);
	AssertPo(_psflTdts, 0);
	AssertPo(_psflTdf, 0);
	AssertPo(_psflMtrl, 0);
	AssertNilOrPo(_pbclTdf, 0);
	AssertNilOrPo(_pbclMtrl, 0);
}


/***************************************************************************
	Mark memory used by the ESLT.  The _pape and _psne are marked
	automatically with the GOB tree.
***************************************************************************/
void ESLT::MarkMem(void)
{
	AssertThis(0);
	ESLT_PAR::MarkMem();
	MarkMemObj(_psflTdts);
	MarkMemObj(_psflTdf);
	MarkMemObj(_psflMtrl);
	MarkMemObj(_pbclTdf);
	MarkMemObj(_pbclMtrl);
}
#endif //DEBUG




//
//
//
//  SNE (spletter name editor) stuff begins here
//
//
//


/***************************************************************************
	Create a new spletter name editor with initial string pstnInit.  Text
	change notifications will be sent to peslt.
***************************************************************************/
PSNE SNE::PsneNew(PEDPAR pedpar, PESLT peslt, PSTN pstnInit)
{
	AssertVarMem(pedpar);
	AssertBasePo(peslt, 0);
	AssertPo(pstnInit, 0);

	PSNE psne;

	psne = NewObj SNE(pedpar);
	if (pvNil == psne)
		return pvNil;

	if (!psne->_FInit())
		{
		ReleasePpo(&psne);
		return pvNil;
		}

	psne->_peslt = peslt;
	psne->SetStn(pstnInit, fFalse);
	return psne;
}


/***************************************************************************
	Trap the default FReplace to prevent illegal strings and to notify the
	ESLT that the text has changed.
***************************************************************************/
bool SNE::FReplace(achar *prgch, long cchIns, long ich1, long ich2,
	long gin)
{
	AssertThis(0);

	STN stnOld;
	STN stnNew;

	GetStn(&stnOld);

	// Note that gin is forced to ginNil here so there's no flicker if
	// the resulting text is illegal
	if (!SNE_PAR::FReplace(prgch, cchIns, ich1, ich2, ginNil))
		return fFalse;

	// Look for illegal strings:
	GetStn(&stnNew);
	if (stnNew.Cch() > kcchMaxTdt)
		{
		SetStn(&stnOld, fFalse);
		GetStn(&stnNew);
		}
#ifdef DEBUG
	else if (stnNew.Cch() == 5 && stnNew.Psz()[0] == ChLit(')') &&
		stnNew.Psz()[4] == ChLit('('))
		{
		// Hack for testing: ")xxx(" changes the TDT to
		// all ASCII values from xxx to xxx + kcchMaxTdt (or up to chNil).
		STN stnT;
		achar rgch[kcchMaxTdt];
		long ichStart;
		long ich;
		long cch = 0;

		stnT.SetRgch(&stnNew.Psz()[1], 3);
		if (stnT.FGetLw(&ichStart, 10))
			{
			for (ich = 0; ich < kcchMaxTdt; ich++)
				{
				rgch[ich] = (achar)ichStart + (achar)ich;
				if (rgch[ich] == chNil)
					break;
				cch++;
				}
			SNE_PAR::FReplace(rgch, cch, 0, IchMac(), fFalse);
			GetStn(&stnNew);
			}
		}
#endif //DEBUG

	// Notify the easel so the TDT can be updated
	if (!_peslt->FTextChanged(&stnNew))
		SetStn(&stnOld, fFalse);

	// Now do the actual update
	_UpdateLn(0, 1, 1, _dypLine, gin);

	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the SNE.
***************************************************************************/
void SNE::AssertValid(ulong grf)
{
	SNE_PAR::AssertValid(fobjAllocated);
	AssertBasePo(_peslt, 0);
}


/***************************************************************************
	Mark memory used by the SNE
***************************************************************************/
void SNE::MarkMem(void)
{
	AssertThis(0);
	SNE_PAR::MarkMem();
}
#endif //DEBUG


//
//
//
//  ESLA (actor easel) stuff begins here
//
//
//


BEGIN_CMD_MAP(ESLA, ESL)
	ON_CID_GEN(cidEaselRotate, FCmdRotate, pvNil)
	ON_CID_GEN(cidEaselCostumes, FCmdTool, pvNil)
	ON_CID_GEN(cidEaselAccessories, FCmdTool, pvNil)
END_CMD_MAP_NIL()



/***************************************************************************
	Create a new actor easel
***************************************************************************/
PESLA ESLA::PeslaNew(PRCA prca, PMVIE pmvie, PACTR pactr)
{
	AssertPo(prca, 0);
	AssertPo(pmvie, 0);
	AssertPo(pactr, 0);

	PESLA pesla;
	GCB gcb;

	if (!FBuildGcb(&gcb, kidBackground, kidCostGlass))
		return pvNil;

	pesla = NewObj ESLA(&gcb);
	if (pvNil == pesla)
		return pvNil;
	if (!pesla->_FInit(prca, kidCostGlass, pmvie, pactr))
		{
		ReleasePpo(&pesla);
		return pvNil;
		}
	AssertPo(pesla, 0);
	vpcex->EnqueueCid(cidEaselVisible);

	return pesla;
}


/***************************************************************************
	Set up this easel
***************************************************************************/
bool ESLA::_FInit(PRCA prca, long kidEasel, PMVIE pmvie, PACTR pactr)
{
	AssertBaseThis(0);
	AssertPo(prca, 0);
	AssertPo(pmvie, 0);
	AssertPo(pactr, 0);

	GCB gcb;
	COST cost;
	STN stn;
	EDPAR edpar;
	
	if (!ESLA_PAR::_FInit(prca, kidEasel))
		return fFalse;
	if (!cost.FGet(pactr->Pbody()))
		return fFalse;
	if (!FBuildGcb(&gcb, kidCostPreviewFrame, CMH::HidUnique()))
		return fFalse;
	_pape = APE::PapeNew(&gcb, pactr->Ptmpl(), &cost, pactr->AnidCur(),
		fFalse, prca);
	if (pvNil == _pape)
		return fFalse;
	_pape->SetToolIncCmtl();

	if (!FBuildGcb(&gcb, kidCostEditBox, CMH::HidUnique()))
		return fFalse;
	edpar.Set(gcb._hid, gcb._pgob, gcb._grfgob, gcb._gin, &gcb._rcAbs,
		&gcb._rcRel, vpappb->OnnDefVariable(), 0, vpappb->DypTextDef(),
		tahLeft, tavCenter);
	_pmvie = pmvie;
	_pactr = pactr;
	_pactr->GetName(&stn);
	_pedsl = EDSL::PedslNew(&edpar);
	if (pvNil == _pedsl)
		return fFalse;
	_pedsl->SetStn(&stn);
	_pedsl->SetSel(0, _pedsl->IchMac()); // select all of the text
	_pedsl->Activate(fTrue);
	_pedsl->SetCursCno(prca, kcrsIBeam);

	return fTrue;
}


/***************************************************************************
	Clean up and delete this easel.  Note that I don't need to delete
	_pedsl or _pape because they're GOBs and are automatically destroyed
	with the gob tree.
***************************************************************************/
ESLA::~ESLA(void)
{
	AssertBaseThis(0);
}


/***************************************************************************
	Handle a rotate command
***************************************************************************/
bool ESLA::FCmdRotate(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	_pape->ChangeView();

	return fTrue;	
}


/***************************************************************************
	Handle a tool change command
***************************************************************************/
bool ESLA::FCmdTool(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	switch(pcmd->cid)
		{
	case cidEaselAccessories:
		_pape->SetToolIncAccessory();
		break;

	case cidEaselCostumes:
		_pape->SetToolIncCmtl();
		break;

	default:
		Bug("unexpected cid");
		break;
		}

	return fTrue;	
}


/***************************************************************************
	Make the changes to _pactr
***************************************************************************/
bool ESLA::_FAcceptChanges(bool *pfDismissEasel)
{
	AssertThis(0);
	AssertVarMem(pfDismissEasel);

	PACTR pactrDup;
	bool fNonSpaceFound;
	long ich;
	bool fChangesMade = fFalse;
	STN stnOld;
	STN stnNew;
	long cbset;
	long ibset;
	bool fMtrl;
	long cmid;
	TAG tagMtrl;

	if (!_pactr->FDup(&pactrDup))
		return fFalse;

	_pactr->GetName(&stnOld);
	_pedsl->GetStn(&stnNew);
	if (!stnOld.FEqual(&stnNew))
		{
		// Check for empty or "spaces only" string
		fNonSpaceFound = fFalse;
		for (ich = 0; ich < stnNew.Cch(); ich++)
			{
			if (stnNew.Psz()[ich] != ChLit(' '))
				{
				fNonSpaceFound = fTrue;
				break;
				}
			}
		if (fNonSpaceFound)
			{
			if (!_pmvie->FNameActr(_pactr->Arid(), &stnNew))
				goto LFail;
			fChangesMade = fTrue;
			}
		}

	// Now apply costume changes
	cbset = _pape->Cbset();
	for (ibset = 0; ibset < cbset; ibset++)
		{
		if (_pape->FGetMaterial(ibset, &fMtrl, &cmid, &tagMtrl))
			{
			if (!_pactr->FSetCostumeCore(ibset, &tagMtrl, cmid, !fMtrl))
				goto LFail;
			fChangesMade = fTrue;
			}
		}

	if (fChangesMade)
		{
		if (!_pactr->FCreateUndo(pactrDup, fFalse, &stnOld))
			goto LFail;
		}
	ReleasePpo(&pactrDup);
	return fTrue;
LFail:
	_pactr->Restore(pactrDup);
	ReleasePpo(&pactrDup);
	return fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the ESLA.
***************************************************************************/
void ESLA::AssertValid(ulong grf)
{
	ESLA_PAR::AssertValid(fobjAllocated);
	AssertPo(_pmvie, 0);
	AssertPo(_pactr, 0);
	AssertPo(_pape, 0);
	AssertPo(_pedsl, 0);
}


/***************************************************************************
	Mark memory used by the ESLA.  The _pape and _pedsl are marked
	automatically with the GOB tree.
***************************************************************************/
void ESLA::MarkMem(void)
{
	AssertThis(0);
	ESLA_PAR::MarkMem();
}
#endif //DEBUG


//
//
//  ESLL (EaSeL Listener)
//	The Listener displays either all background sounds or the
//  actor Sound Effects and the actor Speech sounds of highest precedence.
//  Note:  A sounder speech sound takes precedence over a motion match
//  speech sound
//

BEGIN_CMD_MAP(ESLL, ESL)
	ON_CID_GEN(cidEaselVol, FCmdVlm, pvNil)
	ON_CID_GEN(cidEaselPlay, FCmdPlay, pvNil)
END_CMD_MAP_NIL()


/***************************************************************************
	Create a new listener easel
***************************************************************************/
PESLL ESLL::PesllNew(PRCA prca, PMVIE pmvie, PACTR pactr)
{
	AssertPo(prca, 0);
	AssertPo(pmvie, 0);
	AssertNilOrPo(pactr, 0);

	PESLL pesll;
	GCB gcb;
	long kidGlass;

	if (pvNil == pactr)
		kidGlass = kidListenGlassBkgd;
	else
		kidGlass = kidListenGlassActor;

	if (!FBuildGcb(&gcb, kidBackground, kidGlass))
		return pvNil;

	pesll = NewObj ESLL(&gcb);
	if (pvNil == pesll)
		return pvNil;
	if (!pesll->_FInit(prca, kidGlass, pmvie, pactr))
		{
		ReleasePpo(&pesll);
		return pvNil;
		}
	AssertPo(pesll, 0);
	vpcex->EnqueueCid(cidEaselVisible);

	return pesll;
}


/***************************************************************************
	Set up this easel
***************************************************************************/
bool ESLL::_FInit(PRCA prca, long kidEasel, PMVIE pmvie, PACTR pactr)
{
	AssertBaseThis(0);
	AssertPo(prca, 0);
	AssertPo(pmvie, 0);
	AssertNilOrPo(pactr, 0);

	PGL pgltag;
	long vlm;
	bool fLoop;

	if (!ESLL_PAR::_FInit(prca, kidEasel))
		return fFalse;
	_pmvie = pmvie;
	_pscen = pmvie->Pscen();
	_pactr = pactr;

	if (pvNil == pactr)
		{
		// Scene sounds

		// Speech
		if (!_pscen->FQuerySnd(stySpeech, &pgltag, &vlm, &fLoop))
			return fFalse;
		if (!_lsndSpeech.FInit(stySpeech, kidListenVolSpeech,
			kidListenSpeechIcon, kidListenEditBoxSpeech, &pgltag,
			vlm, fLoop, 0, fFalse))
			{
			return fFalse;
			}
			
		// SFX
		if (!_pscen->FQuerySnd(stySfx, &pgltag, &vlm, &fLoop))
			return fFalse;
		if (!_lsndSfx.FInit(stySfx, kidListenVolFX, kidListenFXIcon,
			kidListenEditBoxFX, &pgltag, vlm, fLoop, 0, fFalse))
			{
			return fFalse;
			}
			
		// Midi
		if (!_pscen->FQuerySnd(styMidi, &pgltag, &vlm, &fLoop))
			return fFalse;
		if (!_lsndMidi.FInit(styMidi, kidListenVolMidi, kidListenMidiIcon,
			kidListenEditBoxMidi, &pgltag, vlm, fLoop, 0, fFalse))
			{
			return fFalse;
			}
		}
	else
		{
		// Actor sounds

		// Speech
		if (!_pactr->FQuerySnd(stySpeech, fFalse, &pgltag, &vlm, &fLoop))
			return fFalse;
		if (!_lsndSpeech.FInit(stySpeech, kidListenVolSpeech,
			kidListenSpeechIcon, kidListenEditBoxSpeech, &pgltag,
			vlm, fLoop, _pactr->Arid(), fFalse))
			{
			return fFalse;
			}

		// SFX
		if (!_pactr->FQuerySnd(stySfx, fFalse, &pgltag, &vlm, &fLoop))
			return fFalse;
		if (!_lsndSfx.FInit(stySfx, kidListenVolFX, kidListenFXIcon,
			kidListenEditBoxFX, &pgltag, vlm, fLoop, _pactr->Arid(), fFalse))
			{
			return fFalse;
			}

		// Motion-match speech
		// If no non-motion match speech sounds take precedence, display
		// motion match speech sounds
		if (!_lsndSpeech.FValidSnd())
			{
			if (!_pactr->FQuerySnd(stySpeech, fTrue, &pgltag, &vlm, &fLoop))
				return fFalse;
			if (!_lsndSpeechMM.FInit(stySpeech, kidListenVolSpeechMM,
				kidListenSpeechMMIcon, kidListenEditBoxSpeechMM, &pgltag, vlm,
				fLoop, _pactr->Arid(), fTrue))
				{
				return fFalse;
				}
			}

		// Motion-match SFX
		// If no non-motion match sound effects sounds take precedence, display
		// motion match sound effects sounds
		if (!_lsndSfx.FValidSnd())
			{
			if (!_pactr->FQuerySnd(stySfx, fTrue, &pgltag, &vlm, &fLoop))
				return fFalse;
			if (!_lsndSfxMM.FInit(stySfx, kidListenVolFXMM, kidListenFXMMIcon,
				kidListenEditBoxFXMM, &pgltag, vlm, fLoop, _pactr->Arid(), fTrue))
				{
				return fFalse;
				}
			}

		}
	return fTrue;														
}

/***************************************************************************
	Are there valid sounds in this Lsnd?
***************************************************************************/
bool LSND::FValidSnd(void)
{
	AssertThis(0);
	PMSND pmsnd;
	long itag;
	TAG tag;

	if (pvNil == _pgltag)
		return fFalse;

	for(itag = 0; itag < _pgltag->IvMac(); itag++)
		{
		_pgltag->Get(itag, &tag);
		pmsnd = (PMSND)vptagm->PbacoFetch(&tag, MSND::FReadMsnd);
		if (pvNil == pmsnd)
			continue;
		if (!pmsnd->FValid())
			{
			ReleasePpo(&pmsnd);
			continue;
			}
		ReleasePpo(&pmsnd);
		return fTrue;
		}
	 return fFalse;
}

/***************************************************************************
	Clean up and delete this easel
***************************************************************************/
ESLL::~ESLL(void)
{
	AssertBaseThis(0);
}


/***************************************************************************
	Handle a Listener volume change command
***************************************************************************/
bool ESLL::FCmdVlm(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	long kid = pcmd->rglw[0];
	long vlm = pcmd->rglw[1];

	switch (kid)
		{
	case kidListenVolSpeech:
		_lsndSpeech.SetVlmNew(vlm);
		break;
	case kidListenVolFX:
		_lsndSfx.SetVlmNew(vlm);
		break;
	case kidListenVolSpeechMM:
		_lsndSpeechMM.SetVlmNew(vlm);
		break;
	case kidListenVolFXMM:
		_lsndSfxMM.SetVlmNew(vlm);
		break;
	case kidListenVolMidi:
		_lsndMidi.SetVlmNew(vlm);
		break;
	default:
		Bug("Invalid kid");
		break;
		}

	return fTrue;	
}

/***************************************************************************
	Handle a Listener play command
***************************************************************************/
bool ESLL::FCmdPlay(PCMD pcmd)
{
	AssertVarMem(pcmd);
	AssertNilOrPo(_pactr, 0);

	long kid = pcmd->rglw[0];
	bool fPlay = FPure(pcmd->rglw[1]);

	if (!fPlay)
		{
		StopAllMovieSounds();
		return fTrue;
		}

	// Actor Sounds
	switch (kid)
		{
	case kidListenEditBoxSpeech:
		_lsndSpeech.Play();
		break;
	case kidListenEditBoxFX:
		_lsndSfx.Play();
		break;
	case kidListenEditBoxSpeechMM:
		_lsndSpeechMM.Play();
		break;
	case kidListenEditBoxFXMM:
		_lsndSfxMM.Play();
		break;
	case kidListenEditBoxMidi:
		_lsndMidi.Play();
		break;
	default:
		Bug("Invalid Vlm type");
		break;
		}
	return fTrue;	
}


/***************************************************************************
	Make the changes to _pscen or _pactr.  For actor sounds, we could
	create an undo object here, but we currently don't.  Scene sounds would
	require a new type of undo object so that's definitely not supported.
***************************************************************************/
bool ESLL::_FAcceptChanges(bool *pfDismissEasel)
{
	AssertThis(0);
	AssertVarMem(pfDismissEasel);

	long vlmNew;
	bool fNuked;

	if (pvNil == _pactr)
		{
		// Scene sounds
		if (_lsndSpeech.FChanged(&vlmNew, &fNuked))
			{
			if (fNuked)
				_pscen->RemSndCore(stySpeech);
			else
				_pscen->SetSndVlmCore(stySpeech, vlmNew);
			}

		if (_lsndSfx.FChanged(&vlmNew, &fNuked))
			{
			if (fNuked)
				_pscen->RemSndCore(stySfx);
			else
				_pscen->SetSndVlmCore(stySfx, vlmNew);
			}

		if (_lsndMidi.FChanged(&vlmNew, &fNuked))
			{
			if (fNuked)
				_pscen->RemSndCore(styMidi);
			else
				_pscen->SetSndVlmCore(styMidi, vlmNew);
			}

		}
	else
		{
		// Actor sounds
		if (_lsndSpeech.FChanged(&vlmNew, &fNuked))
			{
			if (fNuked)
				{
				if (!_pactr->FDeleteSndCore(stySpeech, fFalse))
					return fFalse;
				}
			else
				{
				if (!_pactr->FSetVlmSnd(stySpeech, fFalse, vlmNew))
					return fFalse;
				}
			}

		if (_lsndSfx.FChanged(&vlmNew, &fNuked))
			{
			if (fNuked)
				{
				if (!_pactr->FDeleteSndCore(stySfx, fFalse))
					return fFalse;
				}
			else
				{
				if (!_pactr->FSetVlmSnd(stySfx, fFalse, vlmNew))
					return fFalse;
				}
			}

		if (_lsndSpeechMM.FChanged(&vlmNew, &fNuked))
			{
			if (fNuked)
				{
				if (!_pactr->FDeleteSndCore(stySpeech, fTrue))
					return fFalse;
				}
			else
				{
				if (!_pactr->FSetVlmSnd(stySpeech, fTrue, vlmNew))
					return fFalse;
				}
			}

		if (_lsndSfxMM.FChanged(&vlmNew, &fNuked))
			{
			if (fNuked)
				{
				if (!_pactr->FDeleteSndCore(stySfx, fTrue))
					return fFalse;
				}
			else
				{
				if (!_pactr->FSetVlmSnd(stySfx, fTrue, vlmNew))
					return fFalse;
				}
			}

		}

	_pmvie->ClearUndo();
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the ESLL.
***************************************************************************/
void ESLL::AssertValid(ulong grf)
{
	ESLL_PAR::AssertValid(fobjAllocated);
}


/***************************************************************************
	Mark memory used by the ESLL
***************************************************************************/
void ESLL::MarkMem(void)
{
	AssertThis(0);
	ESLL_PAR::MarkMem();
	MarkMemObj(&_lsndSpeech);
	MarkMemObj(&_lsndSfx);
	MarkMemObj(&_lsndSpeechMM);
	MarkMemObj(&_lsndSfxMM);
	MarkMemObj(&_lsndMidi);
}
#endif //DEBUG


//
//
//  LSND (Listener Sound) stuff begins here
//
//


/***************************************************************************
	Initialize a LSND.  Note that the LSND takes over the reference to
	*ppgltag.
***************************************************************************/
bool LSND::FInit(long sty, long kidVol, long kidIcon, long kidEditBox,
		PGL *ppgltag, long vlm, bool fLoop, long objID, bool fMatcher)
{
	AssertBaseThis(0);
	AssertIn(sty, 0, styLim);
	AssertNilOrPo(*ppgltag, 0);

	long st;
	PGOK pgok;
	PTGOB ptgob;
	PMSND pmsnd;
	long itag;
	TAG tag;

	_sty = sty;
	_kidVol = kidVol;
	_kidIcon = kidIcon;
	_kidEditBox = kidEditBox;
	_pgltag = *ppgltag;
	*ppgltag = pvNil;
	_vlm = vlm;
	_vlmNew = vlm;
	_fLoop = fLoop;
	_objID = objID;
	_fMatcher = fMatcher;

	if (pvNil == _pgltag || 0 == _pgltag->IvMac())
		return fTrue;

	// Set button state based on the tool						
	if (_pgltag == pvNil || _pgltag->IvMac() == 0)
		st = kstListenDisabled;
	else if (_fMatcher)
		st = kstListenMatcher;
	else if (_pgltag->IvMac() > 1)
		st = kstListenSounderChain;
	else if (_fLoop)
		st = kstListenLooper;
	else
		st = kstListenSounder;

	// Create the TGOB to display the sound name
	// Search until a valid (not deleted) msnd is found
	for(itag = 0; itag < _pgltag->IvMac(); itag++)
		{
		_pgltag->Get(itag, &tag);
		pmsnd = (PMSND)vptagm->PbacoFetch(&tag, MSND::FReadMsnd);
		if (pvNil == pmsnd)
			return fFalse;
		if (!pmsnd->FValid())
			{
			ReleasePpo(&pmsnd);
			continue;
			}
		ptgob = TGOB::PtgobCreate(kidEditBox, idsListenFont, tavCenter);
		if (pvNil == ptgob)
			{
			Warn("missing edit box");
			ReleasePpo(&pmsnd);
			return fFalse;
			}
		ptgob->SetText(pmsnd->Pstn());
		ReleasePpo(&pmsnd);
		goto LFound;
		}
	// No valid msnd -> no UI change required
	return fTrue;

LFound:
	// Update the slider volume
	vpcex->EnqueueCid(cidListenVolSet,
		vpapp->Pkwa()->PgobFromHid(kidVol), pvNil, _vlm);

	pgok = (PGOK)vpapp->Pkwa()->PgobFromHid(kidIcon);
	if ((pgok != pvNil) && pgok->FIs(kclsGOK))
		pgok->FChangeState(st);

	return fTrue;
}


/***************************************************************************
	Play the LSND's sound
***************************************************************************/
void LSND::Play(void)
{
	AssertThis(0);

	long itag;
	TAG tag;
	PMSND pmsnd;

	if (pvNil == _pgltag || _pgltag->IvMac() == 0)
		return; // nothing to play
	if (_vlmNew < 0)
		return; // don't play nuked sounds

	// Stop sounds that are already playing.  This handles, among other
	// things, a problem that would otherwise occur when changing the
	// volume of a MIDI sound.  Normally, MSND doesn't restart a MIDI
	// sound that's currently playing, but we want to force it to
	// restart here.  
	StopAllMovieSounds();

	for (itag = 0; itag < _pgltag->IvMac(); itag++)
		{
		_pgltag->Get(itag, &tag);
		// Verify sound before including in the event list
		pmsnd = (PMSND)vptagm->PbacoFetch(&tag, MSND::FReadMsnd);
		if (pvNil == pmsnd)
			continue;	// ignore failure
		Assert(pmsnd->Sty() == _sty, 0);
		pmsnd->Play(_objID, _fLoop, (itag > 0), _vlmNew, 1, fTrue);
		ReleasePpo(&pmsnd);
		}
}


/***************************************************************************
	Return whether the LSND has changed.  Specifies new volume and whether
	the sound was nuked.
***************************************************************************/
bool LSND::FChanged(long *pvlmNew, bool *pfNuked)
{
	AssertThis(0);
	AssertVarMem(pvlmNew);
	AssertVarMem(pfNuked);

	*pfNuked = (_vlmNew < 0);
	*pvlmNew = _vlmNew;
	return (_pgltag != pvNil && _pgltag->IvMac() > 0 && _vlm != _vlmNew);
}


/***************************************************************************
	Destroy a LSND
***************************************************************************/
LSND::~LSND(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pgltag);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the LSND.
***************************************************************************/
void LSND::AssertValid(ulong grf)
{
	LSND_PAR::AssertValid(0);
	AssertNilOrPo(_pgltag, 0);
}


/***************************************************************************
	Mark memory used by the LSND
***************************************************************************/
void LSND::MarkMem(void)
{
	AssertThis(0);
	LSND_PAR::MarkMem();
	MarkMemObj(_pgltag);
}
#endif //DEBUG


//
//
//  ESLR (Sound recording easel) stuff begins here
//
//

BEGIN_CMD_MAP(ESLR, ESL)
	ON_CID_GEN(cidEaselRecord, FCmdRecord, pvNil)
	ON_CID_GEN(cidEaselPlay, FCmdPlay, pvNil)
	ON_CID_GEN(cidAlarm, FCmdUpdateMeter, pvNil)
END_CMD_MAP_NIL()


/***************************************************************************
	Create a new sound recording easel
***************************************************************************/
PESLR ESLR::PeslrNew(PRCA prca, PMVIE pmvie, bool fSpeech, PSTN pstnNew)
{
	AssertPo(prca, 0);
	AssertPo(pmvie, 0);
	AssertPo(pstnNew, 0);

	PESLR peslr;
	GCB gcb;

	if (!FBuildGcb(&gcb, kidBackground, kidRecordGlass))
		return pvNil;

	peslr = NewObj ESLR(&gcb);
	if (pvNil == peslr)
		return pvNil;
	if (!peslr->_FInit(prca, kidRecordGlass, pmvie, fSpeech, pstnNew))
		{
		ReleasePpo(&peslr);
		return pvNil;
		}
	AssertPo(peslr, 0);
	vpcex->EnqueueCid(cidEaselVisible);

	return peslr;
}


/***************************************************************************
	Set up this easel
***************************************************************************/
bool ESLR::_FInit(PRCA prca, long kidEasel, PMVIE pmvie, bool fSpeech,
	PSTN pstnNew)
{
	AssertBaseThis(0);
	AssertPo(prca, 0);
	AssertPo(pmvie, 0);
	AssertPo(pstnNew, 0);

	GCB gcb;
	EDPAR edpar;

	_pmvie = pmvie;
	_fSpeech = fSpeech;

	if (!ESLR_PAR::_FInit(prca, kidEasel))
		return fFalse;

	if (!FBuildGcb(&gcb, kidRecordSoundName, CMH::HidUnique()))
		return fFalse;
	edpar.Set(gcb._hid, gcb._pgob, gcb._grfgob, gcb._gin, &gcb._rcAbs,
		&gcb._rcRel, vpappb->OnnDefVariable(), 0, vpappb->DypTextDef(),
		tahLeft, tavCenter);
	_pedsl = EDSL::PedslNew(&edpar);
	if (pvNil == _pedsl)
		return fFalse;
	_pedsl->SetStn(pstnNew, fFalse);
	_pedsl->SetSel(0, _pedsl->IchMac(), fFalse); // select all of the text
	_pedsl->Activate(fTrue);
	_pedsl->SetCursCno(prca, kcrsIBeam);

	// added loop around psrecNew to drop sample rate until it worked
	// to work around Win95 SB16 bug. (Tom Laird-McConnell)
	csampSec = kcsampSec;
	do
		{
		_psrec = SREC::PsrecNew(csampSec, 1, 1, kdtsMaxRecord);
		
		// if we failed, then the pointer is nil
		if (pvNil == _psrec)
			{
			// then downgrade the samples per second to next level
			csampSec >>= 1;
			}
			// until it succeeds, or the sample rate drops below 11025.
		} while ((pvNil == _psrec) && (csampSec >= 11025));

	// if we still failed
	if (pvNil == _psrec)
		{
		PushErc(ercSndamWaveDeviceBusy);
		return fFalse;
		}

	_clok.Start(0);

	// Set sound meter to 0
	PGOK pgok = (PGOK)vpapp->Pkwa()->PgobFromHid(kidRecordSoundLength);
	if (pvNil != pgok)
		vpcex->EnqueueCid(cidRecordSetLength, pgok, 0, 0, 0, 0, 0);
	return fTrue;														
}


/***************************************************************************
	Clean up and delete this easel
***************************************************************************/
ESLR::~ESLR(void)
{
	AssertBaseThis(0);

	ReleasePpo(&_psrec);
}


/***************************************************************************
	Update the meter that shows how long we've been recording.
***************************************************************************/
void ESLR::_UpdateMeter(void)
{
	AssertThis(0);

	PGOK pgok;
	long dtsRec;
	long percentDone; // no good hungarian for a percent

	if (_fRecording)
		{
		dtsRec = TsCurrent() - _tsStartRec;

		percentDone = LwMulDiv(dtsRec, 100, kdtsMaxRecord);
		percentDone = LwBound(percentDone, 0, 100);
	
		pgok = (PGOK)vpapp->Pkwa()->PgobFromHid(kidRecordSoundLength);
		if (pvNil != pgok)
			vpcex->EnqueueCid(cidRecordSetLength, pgok, 0, percentDone, 0, 0, 0);
		else
			Bug("where's the sound length gok?");
		}
}


/***************************************************************************
	Start or stop recording
***************************************************************************/
bool ESLR::FCmdRecord(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (!_fRecording)
		{
		if (_psrec->FStart())
			{
			_tsStartRec = TsCurrent();
			_fRecording = fTrue;
			if (!_clok.FSetAlarm(0, this))
				return fFalse;
			}
		else
			{
			SetGokState(kidRecordRecord, kstDefault); // pop out rec button
			_fRecording = fFalse;
			_UpdateMeter();
			PushErc(ercSndamWaveDeviceBusy);
			return fFalse;
			}
		}
	else
		{
		if (_psrec->FStop())
			{
			SetGokState(kidRecordRecord, kstDefault); // pop out rec button
			_fRecording = fFalse;
			_UpdateMeter();
			}
		}

	return fTrue;	
}


/***************************************************************************
	Time to update the meter that shows how long we've been recording.
***************************************************************************/
bool ESLR::FCmdUpdateMeter(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	bool fRecordingOld = _fRecording;
	bool fPlayingOld = _fPlaying;

	_UpdateMeter();
	_fRecording = _psrec->FRecording();
	_fPlaying = _psrec->FPlaying();
	if (fRecordingOld && !_fRecording)
		{
		SetGokState(kidRecordRecord, kstDefault); // pop out rec button
		_UpdateMeter();
		}
	if (fPlayingOld && !_fPlaying)
		SetGokState(kidRecordPlay, kstDefault); // pop out play button
	if (_fRecording || _fPlaying)
		_clok.FSetAlarm(kdtimMeterUpdate, this);

	return fTrue;
}


/***************************************************************************
	Play the sound that was recorded
***************************************************************************/
bool ESLR::FCmdPlay(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (_fRecording)
		{
		// try to stop recording
		if (_psrec->FStop())
			{
			SetGokState(kidRecordRecord, kstDefault); // pop out rec button
			_fRecording = fFalse;
			_UpdateMeter();
			}
		else
			{
			// We can't stop the recording, so set the record btn to its
			// selected state. (Script had popped the record btn out as
			// soon as the play button was selected).

			SetGokState(kidRecordRecord, kstSelected); // pop in rec button
			}
		}

	if (!_fRecording && _psrec->FHaveSound())
		{
		if (!_fPlaying)
			{
			if (_psrec->FPlay())
				{
				_fPlaying = fTrue;
				if (!_clok.FSetAlarm(0, this))
					return fFalse;
				}
			}
		else
			{
			if (_psrec->FStop())
				{
				_fPlaying = fFalse;
				SetGokState(kidRecordPlay, kstDefault); // pop out play button
				}
			}
		}
	else
		{
		SetGokState(kidRecordPlay, kstDefault); // pop out play button
		}

	return fTrue;	
}


/***************************************************************************
	Save the new sound
***************************************************************************/
bool ESLR::_FAcceptChanges(bool *pfDismissEasel)
{
	AssertThis(0);
	AssertVarMem(pfDismissEasel);

	FNI fni;
	STN stn;
	PFIL pfil = pvNil;
	CNO cno;
	long sty = _fSpeech ? stySpeech : stySfx;
	long kid = _fSpeech ? kidSpeechGlass : kidFXGlass;

	if (_psrec->FRecording())
		{
		if (_psrec->FStop())
			{
			SetGokState(kidRecordRecord, kstDefault); // pop out rec button
			_fRecording = fFalse;
			_UpdateMeter();
			}
		}

	if (_psrec->FPlaying())
		{
		if (_psrec->FStop())
			{
			_fPlaying = fFalse;
			SetGokState(kidRecordPlay, kstDefault); // pop out play button
			}
		}

	if (!_psrec->FHaveSound())
		return fTrue; // user didn't record anything, so don't do anything

	_pedsl->GetStn(&stn);

	// If the user did not specify a name for the recorded sound, then
	// display an error and keep the recording easel displayed.

	if (stn.Cch() == 0)
		{
		PushErc(ercSocNoSoundName);

		*pfDismissEasel = fFalse;
		return fTrue;
		}

	if (!fni.FGetTemp())
		return fFalse;

	if (!_psrec->FSave(&fni))
		return fFalse;

	pfil = FIL::PfilOpen(&fni, ffilNil);
	if (pvNil == pfil)
		goto LFail;

	if (!_pmvie->FCopySndFileToMvie(pfil, sty, &cno, &stn))
		goto LFail;

	// Select the item, extend lists and hilite it
	vpcex->EnqueueCid(cidBrowserSelectThum, vpappb->PcmhFromHid(kid),
		pvNil, cno, ksidUseCrf, 1, 1);

	ReleasePpo(&pfil);
	if (!fni.FDelete())
		Warn("Couldn't delete temp sound file");
	return fTrue;
LFail:
	ReleasePpo(&pfil);
	if (!fni.FDelete())
		Warn("Couldn't delete temp sound file");
	return fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the ESLR.
***************************************************************************/
void ESLR::AssertValid(ulong grf)
{
	ESLL_PAR::AssertValid(fobjAllocated);
	AssertPo(_psrec, 0);
}


/***************************************************************************
	Mark memory used by the ESLR
***************************************************************************/
void ESLR::MarkMem(void)
{
	AssertThis(0);
	ESLL_PAR::MarkMem();
	MarkMemObj(_psrec);
}
#endif //DEBUG

