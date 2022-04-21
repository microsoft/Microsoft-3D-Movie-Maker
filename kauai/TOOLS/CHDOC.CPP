/***************************************************************************

	Chunky editor document management

***************************************************************************/
#include "ched.h"
ASSERTNAME


BEGIN_CMD_MAP(DCD, DDG)
	ON_CID_GEN(cidAddChunk, &DCD::FCmdAddChunk, pvNil)
	ON_CID_GEN(cidAddPicChunk, &DCD::FCmdAddPicChunk, pvNil)
	ON_CID_GEN(cidAddMbmpChunk, &DCD::FCmdAddBitmapChunk, pvNil)
	ON_CID_GEN(cidAddMaskChunk, &DCD::FCmdAddBitmapChunk, pvNil)
	ON_CID_GEN(cidCompilePostScript, &DCD::FCmdImportScript, pvNil)
	ON_CID_GEN(cidCompileInScript, &DCD::FCmdImportScript, pvNil)
	ON_CID_GEN(cidTestScript, &DCD::FCmdTestScript, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidRunScriptCache, &DCD::FCmdTestScript, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidAddFileChunk, &DCD::FCmdAddFileChunk, pvNil)
	ON_CID_GEN(cidAdoptChunk, &DCD::FCmdAdoptChunk, pvNil)
	ON_CID_GEN(cidDeleteChunk, &DCD::FCmdDeleteChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidUndeleteChunk, &DCD::FCmdDeleteChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidUnadoptChunk, &DCD::FCmdUnadoptChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditNatural, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditHex, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditGL, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditAL, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditGG, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditAG, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditGST, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditAST, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditPic, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditMbmp, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidPlaySound, &DCD::FCmdEditChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidStopSound, &DCD::FCmdStopSound, pvNil)
	ON_CID_GEN(cidDisassembleScript, &DCD::FCmdDisasmScript, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidEditChunkInfo, &DCD::FCmdEditChunkInfo, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidChangeChid, &DCD::FCmdChangeChid, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidSetColorTable, &DCD::FCmdSetColorTable, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidFilterChunk, &DCD::FCmdFilterChunk, pvNil)
	ON_CID_GEN(cidPack, &DCD::FCmdPack, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidTogglePack, &DCD::FCmdPack, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidCloneChunk, &DCD::FCmdCloneChunk, &DCD::FEnableDcdCmd)
	ON_CID_GEN(cidReopen, &DCD::FCmdReopen, pvNil)
END_CMD_MAP_NIL()


BEGIN_CMD_MAP(TSCG, GOB)
	ON_CID_GEN(cidClose, &GOB::FCmdCloseWnd, pvNil)
END_CMD_MAP_NIL()


bool _FGetCtg(PDLG pdlg, long idit, CTG *pctg);
void _PutCtgStn(PDLG pdlg, long idit, CTG ctg);

RTCLASS(DOC)
RTCLASS(DOCE)
RTCLASS(DCLB)
RTCLASS(DCD)
RTCLASS(SEL)
RTCLASS(TSCG)


/****************************************
	Add Chunk dialog
****************************************/
enum
	{
	kiditOkInfo,
	kiditCancelInfo,
	kiditCtgInfo,
	kiditCnoInfo,
	kiditNameInfo,
	kiditLimInfo
	};

//add chunk data
struct ADCD
	{
	PCFL pcfl;
	bool fCkiValid;
	CKI cki;
	};

bool _FDlgAddChunk(PDLG pdlg, long *pidit, void *pv);


/***************************************************************************
	Dialog proc for Add Chunk dialog. pv should be a padcd.
***************************************************************************/
bool _FDlgAddChunk(PDLG pdlg, long *pidit, void *pv)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pidit);
	CTG ctg;
	long lw;
	bool fEmpty;
	ADCD *padcd = (ADCD *)pv;

	switch (*pidit)
		{
	case kiditCancelInfo:
		return fTrue; //dismiss the dialog

	case kiditOkInfo:
		if (!pdlg->FGetValues(0, kiditLimInfo))
			{
			*pidit = ivNil;
			return fTrue;
			}
		if (!_FGetCtg(pdlg, kiditCtgInfo, &ctg))
			{
			vpappb->TGiveAlertSz(PszLit("CTG is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditCtgInfo);
			return fFalse;
			}
		if (!pdlg->FGetLwFromEdit(kiditCnoInfo, &lw, &fEmpty) && !fEmpty)
			{
			vpappb->TGiveAlertSz(PszLit("CNO is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditCnoInfo);
			return fFalse;
			}
		AssertPo(padcd->pcfl, 0);
		if (!fEmpty && (!padcd->fCkiValid || ctg != padcd->cki.ctg ||
				lw != (long)padcd->cki.cno) &&
				padcd->pcfl->FFind(ctg, lw))
			{
			bool tRet = vpappb->TGiveAlertSz(PszLit("A chunk with this CTG & CNO")
				PszLit(" already exists. Replace the existing chunk?"),
				bkYesNoCancel, cokQuestion);

			switch (tRet)
				{
			case tMaybe:
				*pidit = kiditCancelInfo;
				return fTrue;

			case tNo:
				pdlg->SelectDit(kiditCnoInfo);
				return fFalse;
				}
			}
		return fTrue;

	default:
		break;
		}

	return fFalse;
}


/***************************************************************************
	Constructor for DOC class.
***************************************************************************/
DOC::DOC(void)
{
	_pcfl = pvNil;
}


/***************************************************************************
	Destructor for DOC class.
***************************************************************************/
DOC::~DOC(void)
{
	ReleasePpo(&_pcfl);
}


/***************************************************************************
	Static method to create a new document based on the given fni.
	Use pfni == pvNil to create a new file, non-nil to open an
	existing file.
***************************************************************************/
PDOC DOC::PdocNew(FNI *pfni)
{
	PCFL pcfl;
	PDOC pdoc;

	if (pvNil == pfni)
		pcfl = CFL::PcflCreateTemp();
	else
		{
		AssertPo(pfni, ffniFile);

		//make sure no other docs are based on this pcfl.
		if (pvNil != DOCB::PdocbFromFni(pfni))
			return pvNil;
		pcfl = CFL::PcflOpen(pfni, fcflNil);
		}

	if (pvNil == pcfl)
		return pvNil;

	if (pvNil == (pdoc = NewObj DOC()))
		{
		ReleasePpo(&pcfl);
		return pvNil;
		}

	pdoc->_pcfl = pcfl;
	AssertPo(pdoc, 0);
	return pdoc;
}


/***************************************************************************
	Create a new DDG for the doc.
***************************************************************************/
PDDG DOC::PddgNew(PGCB pgcb)
{
	AssertThis(0);
	return DCD::PdcdNew(this, _pcfl, pgcb);
}


/***************************************************************************
	Get the current FNI for the doc. Return false if the doc is not
	currently based on an FNI (it's a new doc or an internal one).
***************************************************************************/
bool DOC::FGetFni(FNI *pfni)
{
	AssertThis(0);
	AssertBasePo(pfni, 0);
	if (_pcfl->FTemp())
		return fFalse;

	_pcfl->GetFni(pfni);
	return fTrue;
}


/***************************************************************************
	Save the document and optionally set this fni as the current one.
	If the doc is currently based on an FNI, pfni may be nil, indicating
	that this is a normal save (not save as). If pfni is not nil and
	fSetFni is false, this just writes a copy of the doc but doesn't change
	the doc one bit.
***************************************************************************/
bool DOC::FSaveToFni(FNI *pfni, bool fSetFni)
{
	AssertThis(0);
	if (!fSetFni && pvNil != pfni)
		{
		if (!_pcfl->FSaveACopy(kctgChed, pfni))
			goto LFail;
		return fTrue;
		}

	if (!_pcfl->FSave(kctgChed, pfni))
		{
LFail:
		vpappb->TGiveAlertSz(PszLit("Saving chunky file failed!"),
			bkOk, cokExclamation);
		return fFalse;
		}

	_fDirty = fFalse;
	_pcfl->SetTemp(fFalse);

	return fTrue;
}


/***************************************************************************
	Ask the user what file they want to save to.
***************************************************************************/
bool DOC::FGetFniSave(FNI *pfni)
{
	AssertThis(0);
	return FGetFniSaveMacro(pfni, 'CHN2', "\x9" "Save As: ", "",
		PszLit("All files\0*.*\0"), vwig.hwndApp);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the DOC.
***************************************************************************/
void DOC::AssertValid(ulong grf)
{
	DOC_PAR::AssertValid(grf);
	AssertPo(_pcfl, 0);
}
#endif //DEBUG


/***************************************************************************
	Constructor for chunk editing doc.
***************************************************************************/
DOCE::DOCE(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno) : DOCB(pdocb)
{
	_pcfl = pcfl;
	_ctg = ctg;
	_cno = cno;
	AssertThis(0);
}


/***************************************************************************
	Initialize the DOCH - initialize the stream.
***************************************************************************/
bool DOCE::_FInit(void)
{
	BLCK blck;

	if (!_pcfl->FFind(_ctg, _cno, &blck))
		{
		Bug("Chunk not found");
		return fFalse;
		}
	if (!_FRead(&blck))
		return fFalse;

	return fTrue;
}


/***************************************************************************
	Static method to look for a DOCE for the given chunk.
***************************************************************************/
PDOCE DOCE::PdoceFromChunk(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno)
{
	PDOCE pdoce;

	for (pdocb = pdocb->PdocbChd(); pvNil != pdocb; pdocb = pdocb->PdocbSib())
		{
		if (!pdocb->FIs(kclsDOCE))
			continue;
		pdoce = (PDOCE)pdocb;
		AssertPo(pdoce, 0);
		if (pdoce->_pcfl == pcfl && pdoce->_ctg == ctg && pdoce->_cno == cno)
			return pdoce;
		}
	return pvNil;
}


/***************************************************************************
	Static method: For all DOCE children of the DOCB, checks if the chunk
	still exists and nukes the DOCE if not.
***************************************************************************/
void DOCE::CloseDeletedDoce(PDOCB pdocb)
{
	PDOCB pdocbNext;
	PDOCE pdoce;

	for (pdocb = pdocb->PdocbChd(); pvNil != pdocb; pdocb = pdocbNext)
		{
		pdocbNext = pdocb->PdocbSib();
		if (!pdocb->FIs(kclsDOCE))
			continue;
		pdoce = (PDOCE)pdocb;
		//NOTE: can't assert the pdoce here because the chunk may be gone
		//AssertPo(pdoce, 0);
		AssertPo(pdoce->_pcfl, 0);
		if (!pdoce->_pcfl->FFind(pdoce->_ctg, pdoce->_cno))
			pdoce->CloseAllDdg();
		else
			AssertPo(pdoce, 0);
		}
}


/***************************************************************************
	Get the name of the item document.
***************************************************************************/
void DOCE::GetName(PSTN pstn)
{
	AssertThis(0);
	STN stn;

	stn.FFormatSz(PszLit(": %f %08x"), _ctg, _cno);
	_pdocbPar->GetName(pstn);
	pstn->FAppendStn(&stn);
}


/***************************************************************************
	Save the document. Handles cidSave, cidSaveAs and cidSaveCopy.
***************************************************************************/
bool DOCE::FSave(long cid)
{
	AssertThis(0);
	CTG ctg;
	CNO cno;
	bool fCreated = fFalse;
	PDLG pdlg = pvNil;

	switch (cid)
		{
	case cidSave:
		ctg = _ctg;
		cno = _cno;
		break;

	case cidSaveAs:
	case cidSaveCopy:
		long idit;
		long lw;
		ADCD adcd;

		//put up the dialog
		adcd.pcfl = _pcfl;
		adcd.fCkiValid = fTrue;
		adcd.cki.ctg = _ctg;
		adcd.cki.cno = _cno;
		pdlg = DLG::PdlgNew(dlidChunkInfo, _FDlgAddChunk, &adcd);
		if (pvNil == pdlg)
			goto LCancel;
		_PutCtgStn(pdlg, kiditCtgInfo, _ctg);
		idit = kiditCnoInfo;
		idit = pdlg->IditDo(idit);
		if (idit != kiditOkInfo)
			goto LCancel;

		if (!_FGetCtg(pdlg, kiditCtgInfo, &ctg) ||
			!pdlg->FGetLwFromEdit(kiditCnoInfo, &lw, &fCreated) &&
			!fCreated)
			{
			goto LFail;
			}
		ReleasePpo(&pdlg);
		cno = lw;

		if (fCreated)
			{
			if (!_pcfl->FAdd(0, ctg, &cno))
				goto LFail;
			}
		break;

	default:
		return fFalse;
		}

	if (_FSaveToChunk(ctg, cno, cid != cidSaveCopy))
		{
		PDOCB pdocb;
		PDDG pddg;

		if (pvNil != (pdocb = PdocbPar()) && pvNil != (pddg = pdocb->PddgGet(0)) &&
				pddg->FIs(kclsDCD))
			{
			((PDCD)pddg)->InvalAllDcd(pdocb, _pcfl);
			}
		if (cid != cidSaveCopy)
			UpdateName();
		return fTrue;
		}

	//saving failed
	if (fCreated)
		_pcfl->Delete(ctg, cno);

LFail:
	vpappb->TGiveAlertSz(PszLit("Saving chunk failed"), bkOk, cokExclamation);
LCancel:
	ReleasePpo(&pdlg);
	return fFalse;
}


/***************************************************************************
	Save the chunk data to a chunk.
***************************************************************************/
bool DOCE::_FSaveToChunk(CTG ctg, CNO cno, bool fRedirect)
{
	AssertThis(0);
	CNO cnoT;
	CKI cki;
	BLCK blck;
	long cb = _CbOnFile();

	//if the chunk already exists, add a temporary chunk, swap the data
	//between the two chunks, then delete the temporary chunk.
	//if the chunk doesn't yet exist, create it.
	if (!_pcfl->FFind(ctg, cno))
		{
		cnoT = cno;
		if (!_pcfl->FPut(cb, ctg, cno, &blck))
			return fFalse;
		}
	else if (!_pcfl->FAdd(cb, ctg, &cnoT, &blck))
		return fFalse;

	if (!_FWrite(&blck, fRedirect))
		{
		_pcfl->Delete(ctg, cnoT);
		return fFalse;
		}

	if (cno != cnoT)
		{
		_pcfl->SwapData(ctg, cno, ctg, cnoT);
		_pcfl->Delete(ctg, cnoT);
		}

	if (fRedirect)
		{
		_ctg = ctg;
		_cno = cno;
		_fDirty = fFalse;
		}

	//need to invalidate the line
	cki.ctg = ctg;
	cki.cno = cno;
	DCD::InvalAllDcd(_pdocbPar, _pcfl, &cki);

	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of an object.
***************************************************************************/
void DOCE::AssertValid(ulong grf)
{
	DOCE_PAR::AssertValid(0);
	AssertPo(_pcfl, 0);
	Assert(_pcfl->FFind(_ctg, _cno), "chunk not in CFL");
}
#endif //DEBUG



/***************************************************************************
	Constructor for a DCLB.
***************************************************************************/
DCLB::DCLB(PDOCB pdocb, PGCB pgcb) : DDG(pdocb, pgcb)
{
	achar ch;
	RC rc;
	GNV gnv(this);

	_dypHeader = 0;
	_onn = vpappb->OnnDefFixed();
	gnv.SetOnn(_onn);
	ch = 'A';
	gnv.GetRcFromRgch(&rc, &ch, 1, 0, 0);
	_dypLine = rc.Dyp();
	_dxpChar = rc.Dxp();
	AssertThis(0);
}


/***************************************************************************
	Return the ln that corresponds with the given yp value. If yp is in
	the header, returns lnNil.
***************************************************************************/
long DCLB::_LnFromYp(long yp)
{
	AssertThis(0);
	if (yp < _dypHeader)
		return lnNil;
	return _scvVert + (yp - _dypHeader) / _dypLine;
}


/***************************************************************************
	Perform a scroll according to scaHorz and scaVert.
***************************************************************************/
void DCLB::_Scroll(long scaHorz, long scaVert, long scvHorz, long scvVert)
{
	AssertThis(0);
	RC rc;
	long dscvHorz, dscvVert;
	long dxp, dyp;

	_GetContent(&rc);
	dscvHorz = dscvVert = 0;
	dxp = 0;
	switch (scaHorz)
		{
	case scaPageUp:
		dscvHorz = -LwMax(1, LwMulDiv(rc.Dxp(), 9, 10) / _dxpChar);
		goto LHorz;
	case scaPageDown:
		dscvHorz = LwMax(1, LwMulDiv(rc.Dxp(), 9, 10) / _dxpChar);
		goto LHorz;
	case scaLineUp:
		dscvHorz = -LwMax(1, rc.Dxp() / 10 / _dxpChar);
		goto LHorz;
	case scaLineDown:
		dscvHorz = LwMax(1, rc.Dxp() / 10 / _dxpChar);
		goto LHorz;
	case scaToVal:
		dscvHorz = scvHorz - _scvHorz;
LHorz:
		dscvHorz = LwBound(_scvHorz + dscvHorz, 0, _ScvMax(fFalse) + 1) - _scvHorz;
		_scvHorz += dscvHorz;
		dxp = LwMul(dscvHorz, _dxpChar);
		break;
		}

	dyp = 0;
	switch (scaVert)
		{
	case scaPageUp:
		dscvVert = -LwMax(1, rc.Dyp() / _dypLine - 1);
		goto LVert;
	case scaPageDown:
		dscvVert = LwMax(1, rc.Dyp() / _dypLine - 1);
		goto LVert;
	case scaLineUp:
		dscvVert = -1;
		goto LVert;
	case scaLineDown:
		dscvVert = 1;
		goto LVert;
	case scaToVal:
		dscvVert = scvVert - _scvVert;
LVert:
		dscvVert = LwBound(_scvVert + dscvVert, 0, _ScvMax(fTrue) + 1) - _scvVert;
		_scvVert += dscvVert;
		dyp = LwMul(dscvVert, _dypLine);
		break;
		}

	_SetScrollValues();
	if (dxp != 0 || dyp != 0)
		_ScrollDxpDyp(dxp, dyp);
}


/***************************************************************************
	Move the bits in the window.
***************************************************************************/
void DCLB::_ScrollDxpDyp(long dxp, long dyp)
{
	AssertThis(0);
	RC rc;

	_GetContent(&rc);
	Scroll(&rc, -dxp, -dyp, kginDraw);
	if (0 != dxp)
		{
		//scroll the header
		rc.ypTop = 0;
		rc.ypBottom = _dypHeader - 1;
		Scroll(&rc, -dxp, 0, kginDraw);
		}
}


/***************************************************************************
	Get the content part of the DCLB minus header (and any future footer).
***************************************************************************/
void DCLB::_GetContent(RC *prc)
{
	GetRc(prc, cooLocal);
	prc->ypTop += _dypHeader;
}


/***************************************************************************
	Fill in reasonable minimum and maximum sizes.
***************************************************************************/
void DCLB::GetMinMax(RC *prcMinMax)
{
	prcMinMax->xpLeft = 20 * _dxpChar;
	prcMinMax->ypTop = _dypHeader + 2 * _dypLine;
	prcMinMax->xpRight = kswMax;
	prcMinMax->ypBottom = kswMax;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of an object.
***************************************************************************/
void DCLB::AssertValid(ulong grf)
{
	DCLB_PAR::AssertValid(0);
	AssertIn(_dypLine, 1, 1000);
	AssertIn(_dxpChar, 1, 1000);
	AssertIn(_dypHeader, 0, 1000);
}
#endif //DEBUG


/***************************************************************************
	Constructor for the DCD.
***************************************************************************/
DCD::DCD(PDOCB pdocb, PCFL pcfl, PGCB pgcb) : DCLB(pdocb, pgcb), _sel(pcfl)
{
	_pcfl = pcfl;
	_dypBorder = 1;
	_dypLine += _dypBorder;
	_dypHeader = _dypLine;
	AssertThis(0);
}


/***************************************************************************
	Static method to create a new DCD.
***************************************************************************/
PDCD DCD::PdcdNew(PDOCB pdocb, PCFL pcfl, PGCB pgcb)
{
	PDCD pdcd;

	if (pvNil == (pdcd = NewObj DCD(pdocb, pcfl, pgcb)))
		return pvNil;

	if (!pdcd->_FInit())
		{
		ReleasePpo(&pdcd);
		return pvNil;
		}
	pdcd->Activate(fTrue);

	AssertPo(pdcd, 0);
	return pdcd;
}


/***************************************************************************
	We're being activated or deactivated, invert the sel.
***************************************************************************/
void DCD::_Activate(bool fActive)
{
	AssertThis(0);
	DDG::_Activate(fActive);

	GNV gnv(this);
	_DrawSel(&gnv);
}


/***************************************************************************
	Static method: should be called by any code that edits the pcfl.
	*pcki and *pkid should be at or before the first line modified.
	pcki and pkid can be nil.
***************************************************************************/
void DCD::InvalAllDcd(PDOCB pdocb, PCFL pcfl, CKI *pcki, KID *pkid)
{
	AssertPo(pdocb, 0);
	AssertPo(pcfl, 0);
	AssertNilOrVarMem(pcki);
	AssertNilOrVarMem(pkid);
	long ipddg;
	PDDG pddg;
	PDCD pdcd;

	//mark the document dirty
	pdocb->SetDirty();

	//inform the DCDs
	for (ipddg = 0; pvNil != (pddg = pdocb->PddgGet(ipddg)); ipddg++)
		{
		if (!pddg->FIs(kclsDCD))
			continue;
		pdcd = (PDCD)pddg;
		AssertPo(pdcd, 0);
		if (pdcd->_pcfl == pcfl)
			pdcd->_InvalCkiKid(pcki, pkid);
		}
}


/***************************************************************************
	Invalidate the display from (pcki, pkid) to the end of the display. If
	we're the active DCD, also redraw.
***************************************************************************/
void DCD::_InvalCkiKid(CKI *pcki, KID *pkid)
{
	AssertThis(0);
	AssertNilOrVarMem(pcki);
	AssertNilOrVarMem(pkid);
	RC rc;
	long lnMin;

	//we need to recalculate the lnLim
	_sel.InvalLim();

	if (pvNil != pcki)
		{
		SEL sel = _sel;

		sel.FSetCkiKid(pcki, pkid, fFalse);
		lnMin = sel.Ln();
		}
	else
		lnMin = lnNil;

	//correct the sel
	if (lnNil != _sel.Ln() && _sel.Ln() >= lnMin)
		_sel.Adjust();

	GetRc(&rc, cooLocal);
	rc.ypTop = LwMax(0, _YpFromLn(lnMin) - _dypBorder);
	if (rc.FEmpty())
		return;

	if (_fActive)
		{
		ValidRc(&rc, kginDraw);
		InvalRc(&rc, kginDraw);
		}
	else
		InvalRc(&rc);
}


/***************************************************************************
	Draw the chunk list
***************************************************************************/
void DCD::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	STN stn;
	STN stnT;
	RC rc;
	long yp, xp;
	long ikid, cckiRef;
	CKI cki;
	KID kid;
	BLCK blck;
	ulong grfsel;
	SEL sel = _sel;

	pgnv->ClipRc(prcClip);
	pgnv->SetOnn(_onn);

	xp = _XpFromIch(0);
	//draw the header
	rc = *prcClip;
	rc.ypTop = 0;
	rc.ypBottom = _dypHeader - _dypBorder;
	pgnv->FillRc(&rc, kacrWhite);
	stn.FFormatSz(
		PszLit("LPF  PARS   SIZE     (CHID)   CTG     CNO     Name   Lines: %d"),
		_sel.LnLim());
	pgnv->DrawStn(&stn, xp, 0);
	rc = *prcClip;
	rc.ypTop = _dypHeader - _dypBorder;
	rc.ypBottom = _dypHeader;
	pgnv->FillRc(&rc, kacrBlack);

	//use the sel to find the first (icki, ikid) to draw
	if (!sel.FSetLn(_LnFromYp(LwMax(prcClip->ypTop, _dypHeader))))
		{
		//no visible lines
		rc = *prcClip;
		rc.ypTop = _dypHeader;
		pgnv->FillRc(&rc, kacrWhite);
		return;
		}

	for (yp = _YpFromLn(sel.Ln()); yp < prcClip->ypBottom; )
		{
		if (sel.Ln() == lnNil)
			{
			rc = *prcClip;
			rc.ypTop = yp;
			pgnv->FillRc(&rc, kacrWhite);
			break;
			}

		grfsel = sel.GrfselGetCkiKid(&cki, &kid);
		rc = *prcClip;
		rc.ypTop = yp;
		rc.ypBottom = yp + _dypLine;
		pgnv->FillRc(&rc, kacrWhite);
		if (grfsel & fselKid)
			{
			stn.FFormatSz(PszLit("                    %08x  %f  %08x"),
				kid.chid, kid.cki.ctg, kid.cki.cno);
			pgnv->DrawStn(&stn, xp, yp);
			yp += _dypLine;
			}
		else
			{
			Assert(grfsel & fselCki, "bad grfsel");

			//draw the cki description
			_pcfl->FFind(cki.ctg, cki.cno, &blck);
			_pcfl->FGetName(cki.ctg, cki.cno, &stnT);
			cckiRef = _pcfl->CckiRef(cki.ctg, cki.cno);
			ikid = _pcfl->Ckid(cki.ctg, cki.cno);

			stn.FFormatSz(PszLit("%c%c%c  %3d  %08x  --------  %f  %08x  \"%s\""),
				_pcfl->FLoner(cki.ctg, cki.cno) ? ChLit('L') : ChLit(' '),
				blck.FPacked() ? ChLit('P') : ChLit(' '),
				_pcfl->FForest(cki.ctg, cki.cno) ? ChLit('F') : ChLit(' '),
				cckiRef, blck.Cb(fTrue), cki.ctg, cki.cno, &stnT);
			pgnv->DrawStn(&stn, xp, yp);
			yp += _dypLine;
			}

		sel.FAdvance();

		//draw seperating line
		if (yp > _dypHeader)
			{
			rc = *prcClip;
			rc.ypTop = yp - _dypBorder;
			rc.ypBottom = yp;
			pgnv->FillRc(&rc, (sel.Ikid() == ivNil) ? kacrGray : kacrWhite);
			}
		}

	//draw the selection
	if (_fActive)
		_DrawSel(pgnv);
}


/***************************************************************************
	Hilite the selection (if there is one)
***************************************************************************/
void DCD::_DrawSel(PGNV pgnv)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	RC rc;
	long ln;

	if (lnNil == (ln = _sel.Ln()))
		return;

	pgnv->GetRcSrc(&rc);
	rc.ypTop = _YpFromLn(ln);
	rc.ypBottom = rc.ypTop + _dypLine - _dypBorder;
	rc.ypTop = LwMax(rc.ypTop, _dypHeader);
	pgnv->HiliteRc(&rc, kacrWhite);
}


/***************************************************************************
	Set the selection to the given ln.
***************************************************************************/
void DCD::_SetSel(long ln, CKI *pcki, KID *pkid)
{
	AssertThis(0);
	AssertNilOrVarMem(pcki);
	AssertNilOrVarMem(pkid);
	SEL sel = _sel;

	if (pvNil != pcki)
		{
		Assert(lnNil == ln, "ln and pcki both not nil");
		sel.FSetCkiKid(pcki, pkid);
		}
	else
		{
		Assert(pvNil == pkid, 0);
		sel.FSetLn(ln);
		}

	if (_sel.Ln() == sel.Ln())
		return;

	GNV gnv(this);

	//erase the old sel
	if (_fActive)
		_DrawSel(&gnv);

	//set the new sel and draw it
	_sel = sel;
	if (_fActive)
		_DrawSel(&gnv);
}


/***************************************************************************
	Scroll the sel into view.
***************************************************************************/
void DCD::_ShowSel(void)
{
	AssertThis(0);
	RC rc;
	long ln, lnLim;

	if (lnNil == (ln = _sel.Ln()))
		_Scroll(scaNil, scaToVal, 0, _scvVert);
	else if (ln < _scvVert)
		_Scroll(scaNil, scaToVal, 0, ln);
	else
		{
		_GetContent(&rc);
		lnLim = LwMax(_scvVert + 1, _LnFromYp(rc.ypBottom));
		if (ln >= lnLim)
			_Scroll(scaNil, scaToVal, 0, _scvVert + ln + 1 - lnLim);
		}
}


/***************************************************************************
	Handle a mouse down in our content.
***************************************************************************/
void DCD::MouseDown(long xp, long yp, long cact, ulong grfcust)
{
	AssertThis(0);
	PGOB pgob;
	PDCD pdcd, pdcdNew;
	PT pt, ptT;
	bool fDown;
	long ln, lnNew;
	CKI cki, ckiNew;
	KID kid;
	ulong grfsel;

	ln = _sel.Ln();
	if (lnNil != (lnNew = _LnFromYp(yp)))
		{
		_SetSel(lnNew);
		grfsel = _sel.GrfselGetCkiKid(&cki, &kid);
		if ((grfsel & fselCki) && !(grfsel & fselKid))
			{
			kid.cki = cki;
			kid.chid = 0;
			}
		lnNew = _sel.Ln();
		}

	if (!_fActive)
		Activate(fTrue);

	if (lnNil == lnNew)
		return;

	// handle a control/command click--edit chunk information/change CHID
	if (grfcust & fcustCmd)
		{
		_SetSel(lnNil);
		if (grfsel & fselKid)
			{
			if (_FChangeChid(&cki, &kid))
				InvalAllDcd(_pdocb, _pcfl);
			_SetSel(lnNil, &cki, &kid);
			_ShowSel();
			}
		else
			{
			if (_FEditChunkInfo(&cki))
				InvalAllDcd(_pdocb, _pcfl);
			_SetSel(lnNil, &cki);
			_ShowSel();
			}
		return;
		}

	// handle a double click - edit or goto the item
	if (cact > 1 && ln == lnNew)
		{
		if (!(grfsel & fselKid))
			_EditCki(&cki, cidEditNatural);
		else
			{
			_SetSel(lnNil, &kid.cki);
			_ShowSel();
			}
		return;
		}

	pt.xp = xp;
	pt.yp = yp;
	pdcd = pvNil;
	ln = lnNil;
	for (fDown = fTrue; fDown; GetPtMouse(&pt, &fDown))
		{
		ptT = pt;
		MapPt(&ptT, cooLocal, cooGlobal);
		pgob = GOB::PgobFromPtGlobal(ptT.xp, ptT.yp, &ptT);
		if (pgob != this && (pvNil == pgob || !pgob->FIs(kclsDCD) ||
				_pcfl != ((PDCD)pgob)->_pcfl))
			{
			pdcdNew = pvNil;
			}
		else
			{
			pdcdNew = (PDCD)pgob;
			lnNew = pdcdNew->_LnFromYp(ptT.yp);

			SEL sel = pdcdNew->_sel;
			sel.FSetLn(lnNew);
			grfsel = sel.GrfselGetCkiKid(&ckiNew, pvNil);

			if (!(grfsel & fselCki) ||
				tNo != _pcfl->TIsDescendent(kid.cki.ctg, kid.cki.cno,
					ckiNew.ctg, ckiNew.cno) ||
				(pdcdNew == this && _sel.Ln() == sel.Ln()))
				{
				pdcdNew = pvNil;
				}
			else
				{
				if (grfsel & fselKid)
					sel.FSetCkiKid(&ckiNew);
				lnNew = sel.Ln();
				if (lnNew < pdcdNew->_scvVert)
					pdcdNew = pvNil;
				}
			}
		if (pvNil == pdcdNew)
			lnNew = lnNil;

		if (pdcd != pdcdNew || ln != lnNew)
			{
			//target change
			if (pvNil != pdcdNew)
				pdcdNew->_HiliteLn(lnNew);
			if (pvNil != pdcd)
				pdcd->_HiliteLn(ln);
			pdcd = pdcdNew;
			ln = lnNew;
			}
		}
	if (pvNil != pdcd)
		{
		pdcd->_HiliteLn(ln);
		if (pdcd != this || ln != _sel.Ln())
			_FDoAdoptChunkDlg(&ckiNew, &kid);
		}
}


/***************************************************************************
	Hilite the line (to indicate a drag target)
***************************************************************************/
void DCD::_HiliteLn(long ln)
{
	AssertThis(0);
	RC rc;
	GNV gnv(this);

	gnv.GetRcSrc(&rc);
	rc.ypTop = _YpFromLn(ln);
	rc.ypBottom = rc.ypTop + _dypLine - _dypBorder;
	gnv.SetPenSize(2 * _dypBorder, 2 * _dypBorder);
	gnv.FrameRcApt(&rc, &vaptGray, kacrInvert, kacrClear);
}


/***************************************************************************
	Reopen the file - nuking all changes since last saved.
***************************************************************************/
bool DCD::FCmdReopen(PCMD pcmd)
{
	AssertThis(0);
	AssertPo(pcmd, 0);
	PDOCB pdocb;
	CKI cki;
	KID kid;
	ulong grfsel;
	long lnOld;

	if (tYes != vpappb->TGiveAlertSz(
			PszLit("Nuke all changes since the last save?"),
			bkYesNo, cokQuestion))
		{
		return fTrue;
		}

	grfsel = _sel.GrfselGetCkiKid(&cki, &kid);
	lnOld = _sel.Ln();
	_SetSel(lnNil);

	if (!_pcfl->FReopen())
		{
		_SetSel(lnOld);
		return fTrue;
		}

	InvalAllDcd(_pdocb, _pcfl);
	if (lnNil != lnOld)
		{
        _SetSel(lnNil, &cki, (grfsel & fselKid) ? &kid : pvNil);
		_ShowSel();
		}

	// get rid of all child documents
	while (pvNil != (pdocb = _pdocb->PdocbChd()))
		{
		pdocb->CloseAllDdg();
		if (pdocb == _pdocb->PdocbChd())
			{
			//	REVIEW shonk: Release: is this the right thing to do?  What if
			// someone else has a reference count to this child DOCB?
			Bug("why wasn't this child doc released?");
			ReleasePpo(&pdocb);
			}
		}

	_pdocb->SetDirty(fFalse);
	return fTrue;
}


/***************************************************************************
	Handle key input.
***************************************************************************/
bool DCD::FCmdKey(PCMD_KEY pcmd)
{
	AssertThis(0);
	long ln, lnLim, lnNew, cln;
	ulong grfsel;
	KID kid;
	CKI cki;
	RC rc;

	ln = _sel.Ln();
	lnLim = _sel.LnLim();
	switch (pcmd->vk)
		{
	case kvkDown:
		lnNew = (0 > ln) ? 0 : (ln + 1) % lnLim;
		goto LChangeSel;

	case kvkUp:
		lnNew = (0 > ln) ? lnLim - 1 : (ln + lnLim - 1) % lnLim;
		goto LChangeSel;

	case kvkPageUp:
	case kvkPageDown:
		_GetContent(&rc);
		cln = LwMax(1, rc.Dyp() / _dypLine - 1);
		if (pcmd->vk == kvkPageDown)
			lnNew = LwMin(lnLim - 1, lnNil == ln ? cln : ln + cln);
		else
			lnNew = LwMax(0, lnNil == ln ? 0 : ln - cln);
		goto LChangeSel;

	case kvkHome:
		lnNew = 0;
		goto LChangeSel;

	case kvkEnd:
		lnNew = lnLim - 1;
LChangeSel:
		if (lnLim == 0)
			{
			Assert(lnNil == ln, "no lines, but non-nil sel");
			break;
			}

		AssertIn(lnNew, 0, lnLim);
		_SetSel(lnNew);
		_ShowSel();
		break;

	case kvkDelete:
	case kvkBack:
		_ClearSel();
		break;

	case kvkReturn:
		grfsel = _sel.GrfselGetCkiKid(&cki, &kid);
		if (pcmd->grfcust & fcustCmd)
			{
			_SetSel(lnNil);
			if (grfsel & fselKid)
				{
				if (_FChangeChid(&cki, &kid))
					InvalAllDcd(_pdocb, _pcfl);
				_SetSel(lnNil, &cki, &kid);
				_ShowSel();
				}
			else
				{
				if (_FEditChunkInfo(&cki))
					InvalAllDcd(_pdocb, _pcfl);
				_SetSel(lnNil, &cki);
				_ShowSel();
				}
			}
		else
			{
			if (grfsel & fselKid)
				{
				_SetSel(lnNil, &kid.cki);
				_ShowSel();
				}
			else if (grfsel & fselCki)
				{
				//edit the chunk
				_ShowSel();
				_EditCki(&cki, cidEditNatural);
				}
			}
		break;

	default:
		break;
		}

	return fTrue;
}


/***************************************************************************
	Return the maximum for the indicated scroll bar.
***************************************************************************/
long DCD::_ScvMax(bool fVert)
{
	AssertThis(0);

	if (fVert)
		{
		RC rc;

		_GetContent(&rc);
		return LwMax(0, _sel.LnLim() - rc.Dyp() / _dypLine + 1);
		}
	return 320;
}


/***************************************************************************
	Handle enabling/disabling DCD commands.
***************************************************************************/
bool DCD::FEnableDcdCmd(PCMD pcmd, ulong *pgrfeds)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	AssertVarMem(pgrfeds);
	CKI cki;

	switch (pcmd->cid)
		{
	case cidUnadoptChunk:
	case cidChangeChid:
		if (ivNil == _sel.Ikid())
			goto LDisable;
		break;

	case cidPlaySound:
		if (fselCki != _sel.GrfselGetCkiKid(&cki, pvNil))
			goto LDisable;
		if (cki.ctg != kctgWave && cki.ctg != kctgMidi)
			goto LDisable;
		break;

	case cidDeleteChunk:
	case cidUndeleteChunk:
	case cidEditNatural:
	case cidEditHex:
	case cidEditGL:
	case cidEditAL:
	case cidEditGG:
	case cidEditAG:
	case cidEditGST:
	case cidEditAST:
	case cidEditPic:
	case cidEditMbmp:
	case cidTestScript:
	case cidRunScriptCache:
	case cidDisassembleScript:
	case cidEditChunkInfo:
	case cidSetColorTable:
	case cidPack:
	case cidTogglePack:
	case cidCloneChunk:
		if (fselCki != _sel.GrfselGetCkiKid(pvNil, pvNil))
			goto LDisable;
		break;

	default:
		BugVar("unhandled command in FEnableDcdCmd", &pcmd->cid);
LDisable:
		*pgrfeds = fedsDisable;
		return fTrue;
		}

	*pgrfeds = fedsEnable;
	return fTrue;
}


/***************************************************************************
	Put up the dialog and add the chunk.
***************************************************************************/
bool DCD::_FAddChunk(CTG ctgDef, CKI *pcki, bool *pfCreated)
{
	AssertVarMem(pcki);
	AssertVarMem(pfCreated);
	long idit;
	long lw;
	bool fEmpty;
	PDLG pdlg;
	ADCD adcd;
	STN stn;

	//put up the dialog
	adcd.pcfl = _pcfl;
	adcd.fCkiValid = fFalse;
	pdlg = DLG::PdlgNew(dlidChunkInfo, _FDlgAddChunk, &adcd);
	if (pvNil == pdlg)
		goto LCancel;
	if (ctgNil != ctgDef)
		{
		_PutCtgStn(pdlg, kiditCtgInfo, ctgDef);
		idit = kiditCnoInfo;
		}
	else
		idit = kiditCtgInfo;
	idit = pdlg->IditDo(idit);
	if (idit != kiditOkInfo)
		goto LCancel;

	if (!_FGetCtg(pdlg, kiditCtgInfo, &pcki->ctg) ||
			!pdlg->FGetLwFromEdit(kiditCnoInfo, &lw, &fEmpty) && !fEmpty)
		{
		goto LFail;
		}
	pcki->cno = lw;
	pdlg->GetStn(kiditNameInfo, &stn);
	ReleasePpo(&pdlg);

	if (fEmpty)
		{
		if (!_pcfl->FAdd(0, pcki->ctg, &pcki->cno))
			goto LFail;
		*pfCreated = fTrue;
		}
	else
		{
		*pfCreated = !_pcfl->FFind(pcki->ctg, pcki->cno);
		if (*pfCreated && !_pcfl->FPut(0, pcki->ctg, pcki->cno))
			{
LFail:
			vpappb->TGiveAlertSz(PszLit("Writing chunk failed"),
				bkOk, cokExclamation);
LCancel:
			ReleasePpo(&pdlg);
			TrashVar(pcki);
			TrashVar(pfCreated);
			return fFalse;
			}
		}
	if (stn.Cch() != 0)
		{
		if (!_pcfl->FSetName(pcki->ctg, pcki->cno, &stn))
			Warn("can't set name");
		}
	return fTrue;
}


/***************************************************************************
	Handle command to add a chunk.
***************************************************************************/
bool DCD::FCmdAddChunk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	long lnOld;
	bool fCreated;

	//save and clear the sel
	lnOld = _sel.Ln();
	_SetSel(lnNil);

	if (!_FAddChunk(ctgNil, &cki, &fCreated))
		{
		pcmd->cid = cidNil; //don't record
		_SetSel(lnOld);
		return fTrue;
		}

	InvalAllDcd(_pdocb, _pcfl);
	_SetSel(lnNil, &cki);
	_ShowSel();
	return fTrue;
}


/***************************************************************************
	Handle command to add a chunk.
***************************************************************************/
bool DCD::FCmdAddPicChunk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	BLCK blck;
	long cb;
	long lnOld;
	FNI fni;
	bool fCreated;
	PPIC ppic = pvNil;

	//save and clear the sel
	lnOld = _sel.Ln();
	_SetSel(lnNil);

	//get the fni of the file to add
	Mac( FTG ftg = 'PICT'; )
	if (!FGetFniOpenMacro(&fni, &ftg, 1, PszLit("MetaFiles\0*.EMF;*.WMF\0"),
			vwig.hwndApp))
		{
		goto LCancel;
		}
	if (pvNil == (ppic = PIC::PpicReadNative(&fni)))
		{
		vpappb->TGiveAlertSz(PszLit("Reading picture file failed"),
			bkOk, cokExclamation);
		goto LCancel;
		}

	//add the chunk and write the data
	if (!_FAddChunk(kctgPictNative, &cki, &fCreated))
		goto LCancel;
	cb = ppic->CbOnFile();
	if (!_pcfl->FPut(cb, cki.ctg, cki.cno, &blck) ||
			!ppic->FWrite(&blck))
		{
		if (fCreated)
			_pcfl->Delete(cki.ctg, cki.cno);
		vpappb->TGiveAlertSz(PszLit("Writing chunk failed"), bkOk, cokExclamation);
LCancel:
		ReleasePpo(&ppic);
		pcmd->cid = cidNil; //don't record
		_SetSel(lnOld);
		return fTrue;
		}
	ReleasePpo(&ppic);

	InvalAllDcd(_pdocb, _pcfl);
	_SetSel(lnNil, &cki);
	_ShowSel();
	return fTrue;
}

/***************************************************************************
	Masked bitmap dialog
***************************************************************************/
enum
	{
	kiditOkMbmp,
	kiditCancelMbmp,
	kiditPixelMbmp,
	kiditXPosMbmp,
	kiditYPosMbmp,
	kiditDefaultMbmp,
	kiditLimMbmp
	};
bool _FDlgMbmp(PDLG pdlg, long *pidit, void *pv);


/****************************************************************************
	Dialog proc for input of transparent pixel value and reference point
****************************************************************************/
bool _FDlgMbmp(PDLG pdlg, long *pidit, void *pv)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pidit);
	long lw;

	switch (*pidit)
		{
	case kiditCancelMbmp:
		return fTrue;
	case kiditOkMbmp:
		if (!pdlg->FGetValues(0, kiditLimMbmp))
			{
			*pidit = ivNil;
			return fTrue;
			}
		if (!pdlg->FGetLwFromEdit(kiditPixelMbmp, &lw) ||
			!FIn(lw, 0, (long) kbMax + 1))
			{
			vpappb->TGiveAlertSz(PszLit("The transparent pixel value must be ")
				PszLit("between 0 and 255."), bkOk, cokStop);
			pdlg->SelectDit(kiditPixelMbmp);
			return fFalse;
			}
		if (!pdlg->FGetLwFromEdit(kiditXPosMbmp, &lw))
			{
			vpappb->TGiveAlertSz(PszLit("The X Position must be an integer."),
				bkOk, cokStop);
			pdlg->SelectDit(kiditXPosMbmp);
			return fFalse;
			}
		if (!pdlg->FGetLwFromEdit(kiditYPosMbmp, &lw))
			{
			vpappb->TGiveAlertSz(PszLit("The Y Position must be an integer."),
				bkOk, cokStop);
			pdlg->SelectDit(kiditYPosMbmp);
			return fFalse;
			}
		return fTrue;
	default:
		break;
		}
	return fFalse;
}

/***************************************************************************
	Handle command to add Mbmp or chunk.
***************************************************************************/
bool DCD::FCmdAddBitmapChunk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	BLCK blck;
	long lw;
	long lnOld;
	FNI fni;
	bool fCreated;
	byte bTransparent;
	long xp, yp;
	bool fMask = pcmd->cid != cidAddMbmpChunk;
	PBACO pbaco = pvNil;
	CTG ctg;
	PDLG pdlg = pvNil;

	// save and clear the sel
	lnOld = _sel.Ln();
	_SetSel(lnNil);

	// get the fni of the file to add
	Mac(FTG ftg = '\0BMP';) //REVIEW shonk: this is bogus
	if (!FGetFniOpenMacro(&fni, &ftg, 1, PszLit("Bitmaps\0*.BMP\0"), vwig.hwndApp))
		goto LCancel;


	// get the transparent pixel value
	pdlg = DLG::PdlgNew(dlidMbmp, _FDlgMbmp);
	if (pvNil == pdlg)	
		goto LCancel;
	// put up the initial values
	pdlg->FPutLwInEdit(kiditPixelMbmp, 0);
	pdlg->FPutLwInEdit(kiditXPosMbmp, 0);
	pdlg->FPutLwInEdit(kiditYPosMbmp, 0);
	if (pdlg->IditDo(kiditPixelMbmp) != kiditOkMbmp)
		goto LErrDlg;
	if (!pdlg->FGetLwFromEdit(kiditPixelMbmp, &lw) ||
		!FIn(lw, 0, (long) kbMax + 1))
		{
		goto LErrDlg;
		}
	bTransparent = (byte) lw;

	if (!pdlg->FGetLwFromEdit(kiditXPosMbmp, &lw))
		goto LErrDlg;
	xp = lw;
	if (!pdlg->FGetLwFromEdit(kiditXPosMbmp, &lw))
		{
LErrDlg:
		ReleasePpo(&pdlg);
		goto LCancel;
		}
	yp = lw;
	ReleasePpo(&pdlg);

	if (pvNil == (pbaco = MBMP::PmbmpReadNative(&fni, bTransparent, xp, yp,
			fMask ? fmbmpMask : fmbmpNil)))
		{
		vpappb->TGiveAlertSz(PszLit("Reading bitmap file failed"),
			bkOk, cokExclamation);
		goto LCancel;
		}
	ctg = fMask ? kctgMask : kctgMbmp;

	// add the chunk and write the data
	if (!_FAddChunk(ctg, &cki, &fCreated))
		goto LCancel;

	if (!_pcfl->FPut(pbaco->CbOnFile(), cki.ctg, cki.cno, &blck) ||
			!pbaco->FWrite(&blck))
		{
		if (fCreated)
			_pcfl->Delete(cki.ctg, cki.cno);
		vpappb->TGiveAlertSz(PszLit("Writing chunk failed"), bkOk, cokExclamation);
LCancel:
		ReleasePpo(&pbaco);
		pcmd->cid = cidNil;	// don't record
		_SetSel(lnOld);
		return fTrue;
		}
	ReleasePpo(&pbaco);

	InvalAllDcd(_pdocb, _pcfl);
	_SetSel(lnNil, &cki);
	_ShowSel();
	return fTrue;
}


/***************************************************************************
	Handle command to add a chunk that is a copy of a file's contents.
***************************************************************************/
bool DCD::FCmdAddFileChunk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	PFIL pfil;
	BLCK blck;
	long lnOld;
	FNI fni;
	bool fCreated;

	//save and clear the sel
	lnOld = _sel.Ln();
	_SetSel(lnNil);

	//get the fni of the file to add
	if (!FGetFniOpenMacro(&fni, pvNil, 0, PszLit("All files\0*.*\0"), vwig.hwndApp))
		goto LCancel;
	if (pvNil == (pfil = FIL::PfilOpen(&fni)))
		{
		vpappb->TGiveAlertSz(PszLit("Opening file failed"), bkOk, cokExclamation);
		goto LCancel;
		}
	blck.Set(pfil, 0, pfil->FpMac());
	ReleasePpo(&pfil);

	//add the chunk and write the data
	if (!_FAddChunk(ctgNil, &cki, &fCreated))
		goto LCancel;
	if (!_pcfl->FPutBlck(&blck, cki.ctg, cki.cno))
		{
		if (fCreated)
			_pcfl->Delete(cki.ctg, cki.cno);
		vpappb->TGiveAlertSz(PszLit("Writing chunk failed"), bkOk, cokExclamation);
LCancel:
		pcmd->cid = cidNil; //don't record
		_SetSel(lnOld);
		return fTrue;
		}

	InvalAllDcd(_pdocb, _pcfl);
	_SetSel(lnNil, &cki);
	_ShowSel();
	return fTrue;
}

/**************************************************************************
	Edit Chunk Information dialog
**************************************************************************/

// This struct is needed so _FDlgEditChunkInfo() has access to the chunk
// and chunky file and so _FDlgChangeChunk() has access to the parent and
// child chunks and the chunky file.
struct CLAN
	{
	CKI cki;
	KID kid;
	PCFL pcfl;
	};


bool _FDlgEditChunkInfo(PDLG pdlg, long *pidit, void *pv);

/**************************************************************************
	Dialog proc for Edit Chunk Info dialog. *pv should be a CLAN *, with
	the cki and pcfl fields filled in (the kid field is not used).
**************************************************************************/
bool _FDlgEditChunkInfo(PDLG pdlg, long *pidit, void *pv)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pidit);
	CKI cki;
	long lw;
	bool fEmpty;
	CLAN *pclan = (CLAN *)pv;
	AssertVarMem(pclan);
	AssertPo(pclan->pcfl, 0);

	switch (*pidit)
		{
	case kiditCancelInfo:
		return fTrue;		// dismiss the dialog
	case kiditOkInfo:
		if (!pdlg->FGetValues(0, kiditLimInfo))
			{
			*pidit = ivNil;
			return fTrue;
			}

		// check the chunk
		if (!_FGetCtg(pdlg, kiditCtgInfo, &cki.ctg))
			{
			vpappb->TGiveAlertSz(PszLit("CTG is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditCtgInfo);
			return fFalse;
			}
		if (!pdlg->FGetLwFromEdit(kiditCnoInfo, &lw, &fEmpty))
			{
			vpappb->TGiveAlertSz(PszLit("CNO is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditCnoInfo);
			return fFalse;
			}
		cki.cno = lw;
		if (pclan->pcfl->FFind(cki.ctg, cki.cno) &&
			(cki.ctg != pclan->cki.ctg || cki.cno != pclan->cki.cno))
			{
			vpappb->TGiveAlertSz(PszLit("That CTG/CNO pair already exists"),
				bkOk, cokStop);
			pdlg->SelectDit(kiditCtgInfo);
			return fFalse;
			}
		return fTrue;
	default:
		break;
		}
	return fFalse;
}


/****************************************************************************
	Put up the dialog with initial values and edit the chunk. This function
	will update *pckiOld to contain the new ctg and cno values.
****************************************************************************/
bool DCD::_FEditChunkInfo(CKI *pckiOld)
{
	AssertVarMem(pckiOld);
	long idit;
	long lw;
	PDLG pdlg;
	STN stn;
	CLAN clan;
	CKI cki;

	clan.pcfl = _pcfl;
	clan.cki = *pckiOld;
	TrashVar(&clan.kid);
	// put up the dialog
	pdlg = DLG::PdlgNew(dlidChunkInfo, _FDlgEditChunkInfo, &clan);
	if (pvNil == pdlg)
		return fFalse;

	// set the initial values
	idit = kiditCtgInfo;
	_PutCtgStn(pdlg, kiditCtgInfo, pckiOld->ctg);
	pdlg->FPutLwInEdit(kiditCnoInfo, pckiOld->cno);
	_pcfl->FGetName(pckiOld->ctg, pckiOld->cno, &stn);
	pdlg->FPutStn(kiditNameInfo, &stn);

	idit = pdlg->IditDo(idit);
	if (idit != kiditOkInfo ||
		!_FGetCtg(pdlg, kiditCtgInfo, &cki.ctg) ||
		!pdlg->FGetLwFromEdit(kiditCnoInfo, &lw))
		{
		ReleasePpo(&pdlg);
		return fFalse;
		}
	pdlg->GetStn(kiditNameInfo, &stn);
	ReleasePpo(&pdlg);
	cki.cno = lw;

	if (cki.ctg != pckiOld->ctg || cki.cno != pckiOld->cno)
		_pcfl->Move(pckiOld->ctg, pckiOld->cno, cki.ctg, cki.cno);
	
	// set the original cki to the new cki so the caller has the new chunk
	pckiOld->ctg = cki.ctg;
	pckiOld->cno = cki.cno;

	if (!_pcfl->FSetName(cki.ctg, cki.cno, &stn))
		Warn("can't set new name");

	return fTrue;
}


/**************************************************************************
	Handle command to edit chunk information.
**************************************************************************/
bool DCD::FCmdEditChunkInfo(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	long lnOld;

	// record, save, and clear the sel
	if (fselCki != _sel.GrfselGetCkiKid(&cki, pvNil))
		goto LFail;
	lnOld = _sel.Ln();
	_SetSel(lnNil);

	if (!_FEditChunkInfo(&cki))
		{
		_SetSel(lnOld);
LFail:
		pcmd->cid = cidNil; // don't record
		return fTrue;
		}

	// refresh the window and reset the selection
	InvalAllDcd(_pdocb, _pcfl);
	_SetSel(lnNil, &cki);
	_ShowSel();
	return fTrue;
}


/***************************************************************************
	Handle command to delete a chunk.
***************************************************************************/
bool DCD::FCmdDeleteChunk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;

	if (fselCki != _sel.GrfselGetCkiKid(&cki, pvNil))
		goto LCancel;

	if (pcmd->cid == cidDeleteChunk)
		{
		_pcfl->SetLoner(cki.ctg, cki.cno, fFalse);
		DOCE::CloseDeletedDoce(_pdocb);
		InvalAllDcd(_pdocb, _pcfl);
		_ShowSel();
		}
	else
		{
		_pcfl->SetLoner(cki.ctg, cki.cno, fTrue);
		InvalAllDcd(_pdocb, _pcfl, &cki);
		_ShowSel();
		}

	return fTrue;

LCancel:
	pcmd->cid = cidNil; //don't record
	return fTrue;
}


/**************************************************************************
	Change CHID dialog
**************************************************************************/
enum
	{
	kiditOkChid,
	kiditCancelChid,
	kiditChidChid,
	kiditLimChid
	};

bool _FDlgChangeChid(PDLG pdlg, long *pidit, void *pv);

/**************************************************************************
	Dialog proc for Change CHID dialog.
**************************************************************************/
bool _FDlgChangeChid(PDLG pdlg, long *pidit, void *pv)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pidit);
	CHID chid;
	long lw;
	long ikid;
	CLAN *pclan = (CLAN *)pv;
	AssertVarMem(pclan);
	AssertPo(pclan->pcfl, 0);

	switch (*pidit)
		{
	case kiditCancelChid:
		return fTrue;		// dismiss the dialog
	case kiditOkChid:
		if (!pdlg->FGetValues(kiditChidChid, kiditLimChid))
			{
			*pidit = ivNil;
			return fTrue;
			}

		// get the new chid
		if (!pdlg->FGetLwFromEdit(kiditChidChid, &lw))
			{
			vpappb->TGiveAlertSz(PszLit("CHID value is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditChidChid);
			return fFalse;
			}
		chid = lw;
		
		// check the new chid to make sure that the CTG/CNO/new CHID does not
		// already exist
		if (pclan->pcfl->FGetIkid(pclan->cki.ctg, pclan->cki.cno,
				pclan->kid.cki.ctg, pclan->kid.cki.cno, chid, &ikid) &&
			chid != pclan->kid.chid)
			{
			vpappb->TGiveAlertSz(
				PszLit("This parent/child relationship already exists."),
				bkOk, cokStop);
			pdlg->SelectDit(kiditChidChid);
			return fFalse;
			}
		return fTrue;
	default:
		break;
		}
	return fFalse;
}


/****************************************************************************
	Put up the dialog with initial values and change the CHID. *pkid will be
	affected to reflect this change.
****************************************************************************/
bool DCD::_FChangeChid(CKI *pcki, KID *pkid)
{
	AssertVarMem(pcki);
	AssertVarMem(pkid);
	long idit;
	CHID chid;
	PDLG pdlg;
	long lw;
	CLAN clan;

	clan.cki = *pcki;
	clan.kid = *pkid;
	clan.pcfl = _pcfl;

	// put up the dialog
	pdlg = DLG::PdlgNew(dlidChangeChid, _FDlgChangeChid, &clan);
	if (pvNil == pdlg)
		return fFalse;

	// set the initial value
	pdlg->FPutLwInEdit(kiditChidChid, pkid->chid);

	idit = pdlg->IditDo(kiditChidChid);
	if (idit != kiditOkChid ||
		!pdlg->FGetLwFromEdit(kiditChidChid, &lw))
		{
		ReleasePpo(&pdlg);
		return fFalse;
		}
	chid = lw;
	ReleasePpo(&pdlg);

	// If new CHID is different, unadopt and adopt child to change CHID.
	// This is easier than directly changing the GG which keeps track of the
	// parent and its children.
	if (chid != pkid->chid)
		{
		_pcfl->ChangeChid(pcki->ctg, pcki->cno, pkid->cki.ctg,
			pkid->cki.cno, pkid->chid, chid);
 		// set the new CHID so the caller has the right kid
 		pkid->chid = chid;
		}

	return fTrue;
}

/**************************************************************************
	Handle command to change the CHID.
**************************************************************************/
bool DCD::FCmdChangeChid(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	KID kid;
	long lnOld;

	// record, save, and clear the sel
	if (ivNil == _sel.Icki() || ivNil == _sel.Ikid())
		goto LFail;
	_sel.GrfselGetCkiKid(&cki, &kid);
	lnOld = _sel.Ln();
	_SetSel(lnNil);

	if (!_FChangeChid(&cki, &kid))
		{
		_SetSel(lnOld);
LFail:
		pcmd->cid = cidNil; // don't record
		return fTrue;
		}

	InvalAllDcd(_pdocb, _pcfl);
	_SetSel(lnNil, &cki, &kid);
	_ShowSel();
	return fTrue;
}


/****************************************
	Adopt Chunk dialog
****************************************/
enum
	{
	kiditOkAdopt,
	kiditCancelAdopt,
	kiditCtgParAdopt,
	kiditCnoParAdopt,
	kiditCtgChdAdopt,
	kiditCnoChdAdopt,
	kiditChidAdopt,
	kiditLimAdopt
	};
bool _FDlgAdoptChunk(PDLG pdlg, long *pidit, void *pv);


/***************************************************************************
	Dialog proc for Adopt Chunk dialog.
***************************************************************************/
bool _FDlgAdoptChunk(PDLG pdlg, long *pidit, void *pv)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pidit);
	long lw;
	CKI cki;
	KID kid;
	long ikid;
	bool fEmpty;
	PCFL pcfl = (PCFL)pv;

	AssertPo(pcfl, 0);
	switch (*pidit)
		{
	case kiditCancelAdopt:
		return fTrue; //dismiss the dialog

	case kiditOkAdopt:
		if (!pdlg->FGetValues(0, kiditLimAdopt))
			{
			*pidit = ivNil;
			return fTrue;
			}

		//check the parent
		if (!_FGetCtg(pdlg, kiditCtgParAdopt, &cki.ctg))
			{
			vpappb->TGiveAlertSz(PszLit("Parent CTG is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditCtgParAdopt);
			return fFalse;
			}
		if (!pdlg->FGetLwFromEdit(kiditCnoParAdopt, &lw))
			{
			vpappb->TGiveAlertSz(PszLit("Parent CNO is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditCnoParAdopt);
			return fFalse;
			}
		cki.cno = lw;
		if (!pcfl->FFind(cki.ctg, cki.cno))
			{
			vpappb->TGiveAlertSz(PszLit("Parent chunk doesn't exist"),
				bkOk, cokStop);
			pdlg->SelectDit(kiditCtgParAdopt);
			return fFalse;
			}

		//check the child
		if (!_FGetCtg(pdlg, kiditCtgChdAdopt, &kid.cki.ctg))
			{
			vpappb->TGiveAlertSz(PszLit("Child CTG is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditCtgChdAdopt);
			return fFalse;
			}
		if (!pdlg->FGetLwFromEdit(kiditCnoChdAdopt, &lw))
			{
			vpappb->TGiveAlertSz(PszLit("Child CNO is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditCnoChdAdopt);
			return fFalse;
			}
		kid.cki.cno = lw;
		if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno))
			{
			vpappb->TGiveAlertSz(PszLit("Child chunk doesn't exist"),
				bkOk, cokStop);
			pdlg->SelectDit(kiditCtgChdAdopt);
			return fFalse;
			}

		//check the chid
		if (!pdlg->FGetLwFromEdit(kiditChidAdopt, &lw, &fEmpty) && !fEmpty)
			{
			vpappb->TGiveAlertSz(PszLit("Child ID value is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditChidAdopt);
			return fFalse;
			}
		kid.chid = lw;

		//see if this child-parent relationship already exists
		if (pcfl->FGetIkid(cki.ctg, cki.cno, kid.cki.ctg, kid.cki.cno,
			kid.chid, &ikid) && !fEmpty)
			{
			vpappb->TGiveAlertSz(
				PszLit("This parent/child relationship already exists."),
				bkOk, cokStop);
			pdlg->SelectDit(kiditChidAdopt);
			return fFalse;
			}

		//see if a loop would be formed
		if (pcfl->TIsDescendent(kid.cki.ctg, kid.cki.cno, cki.ctg, cki.cno))
			{
			vpappb->TGiveAlertSz(PszLit("Adopting this child would form a loop")
				PszLit(" in the chunky file graph structure"), bkOk, cokStop);
			return fFalse;
			}

		return fTrue;

	default:
		break;
		}

	return fFalse;
}


/***************************************************************************
	Put up and handle the adopt dialog with the given initial values
	(if not nil).
***************************************************************************/
bool DCD::_FDoAdoptChunkDlg(CKI *pcki, KID *pkid)
{
	AssertThis(0);
	AssertNilOrVarMem(pcki);
	AssertNilOrVarMem(pkid);
	long idit;
	CKI cki;
	KID kid;
	long lnOld;
	long lw1, lw2, lw3;
	bool fEmptyChid;
	PDLG pdlg = pvNil;

	//put up the dialog
	pdlg = DLG::PdlgNew(dlidAdoptChunk, _FDlgAdoptChunk, _pcfl);
	if (pvNil == pdlg)
		return fFalse;

	//set the initial values
	idit = kiditCtgParAdopt;
	if (pvNil != pcki)
		{
		_PutCtgStn(pdlg, kiditCtgParAdopt, pcki->ctg);
		pdlg->FPutLwInEdit(kiditCnoParAdopt, pcki->cno);
		idit = kiditCtgChdAdopt;
		}
	if (pvNil != pkid)
		{
		_PutCtgStn(pdlg, kiditCtgChdAdopt, pkid->cki.ctg);
		pdlg->FPutLwInEdit(kiditCnoChdAdopt, pkid->cki.cno);
		pdlg->FPutLwInEdit(kiditChidAdopt, pkid->chid);
		idit = kiditChidAdopt;
		}

	//save and clear the old sel
	lnOld = _sel.Ln();
	_SetSel(lnNil);

	idit = pdlg->IditDo(idit);
	if (idit != kiditOkAdopt)
		goto LCancel;

	if (!_FGetCtg(pdlg, kiditCtgParAdopt, &cki.ctg) ||
		!pdlg->FGetLwFromEdit(kiditCnoParAdopt, &lw1) ||
		!_FGetCtg(pdlg, kiditCtgChdAdopt, &kid.cki.ctg) ||
		!pdlg->FGetLwFromEdit(kiditCnoChdAdopt, &lw2) ||
		!pdlg->FGetLwFromEdit(kiditChidAdopt, &lw3, &fEmptyChid) && !fEmptyChid)
		{
		goto LFail;
		}
	ReleasePpo(&pdlg);
	cki.cno = lw1;
	kid.cki.cno = lw2;
	kid.chid = lw3;

	if (!_pcfl->FFind(cki.ctg, cki.cno) ||
			!_pcfl->FFind(kid.cki.ctg, kid.cki.cno))
		{
		goto LFail;
		}
	if (!_pcfl->FAdoptChild(cki.ctg, cki.cno,
			kid.cki.ctg, kid.cki.cno, kid.chid))
		{
LFail:
		vpappb->TGiveAlertSz(PszLit("Adopting chunk failed."),
			bkOk, cokExclamation);
LCancel:
		ReleasePpo(&pdlg);
		_SetSel(lnOld);
		return fFalse;
		}

	InvalAllDcd(_pdocb, _pcfl);
	_SetSel(lnNil, &cki, &kid);
	_ShowSel();
	return fTrue;
}


/***************************************************************************
	Handle command to adopt a chunk.
***************************************************************************/
bool DCD::FCmdAdoptChunk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	CKI *pcki;
	PDLG pdlg = pvNil;

	if (fselCki & _sel.GrfselGetCkiKid(&cki, pvNil))
		pcki = &cki;
	else
		pcki = pvNil;

	if (!_FDoAdoptChunkDlg(pcki, pvNil))
		pcmd->cid = cidNil;

	return fTrue;
}


/***************************************************************************
	Handle command to unadopt a chunk.
***************************************************************************/
bool DCD::FCmdUnadoptChunk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	KID kid;

	if (!(_sel.GrfselGetCkiKid(&cki, &kid) & fselKid))
		{
		pcmd->cid = cidNil; //don't record
		return fTrue;
		}

	_pcfl->DeleteChild(cki.ctg, cki.cno, kid.cki.ctg, kid.cki.cno, kid.chid);

	InvalAllDcd(_pdocb, _pcfl);
	_ShowSel();
	return fTrue;
}


/***************************************************************************
	Handles commands to edit a chunk.
***************************************************************************/
bool DCD::FCmdEditChunk(PCMD pcmd)
{
	CKI cki;

	if (fselCki != _sel.GrfselGetCkiKid(&cki, pvNil))
		return fFalse;

	_EditCki(&cki, pcmd->cid);
	return fTrue;
}



/***************************************************************************
	Opens a window onto the CKI's data.
***************************************************************************/
void DCD::_EditCki(CKI *pcki, long cid)
{
	AssertThis(0);
	AssertVarMem(pcki);
	PDOCE pdoce;
	long cls;
	CTG ctg;

	//check for a doce already open on the chunk.
	if (pvNil != (pdoce = DOCE::PdoceFromChunk(_pdocb, _pcfl, pcki->ctg, pcki->cno)))
		{
		pdoce->ActivateDmd();
		return;
		}

	switch (cid)
		{
	case cidEditNatural:
		ctg = pcki->ctg;

		//handle 4 character ctg's
		switch (ctg)
			{
		case kctgPictNative:
			goto LPic;
		case kctgMbmp:
		case kctgMask:
			goto LMbmp;
		case kctgWave:
		case kctgMidi:
			goto LSound;
		default:
			break;
			}

		//handle 3 character ctg's
		ctg = ctg & 0xFFFFFF00L | 0x00000020L;
		switch (ctg)
			{
		case kctgGst:
			cls = kclsGST;
			goto LDocg;
		case kctgAst:
			cls = kclsAST;
			goto LDocg;
		default:
			break;
			}

		//handle 2 character ctg's
		ctg = ctg & 0xFFFF0000L | 0x00002020L;
		switch (ctg)
			{
		case kctgGl:
			cls = kclsGL;
			goto LDocg;
		case kctgAl:
			cls = kclsAL;
			goto LDocg;
		case kctgGg:
			cls = kclsGG;
			goto LDocg;
		case kctgAg:
			cls = kclsAG;
			goto LDocg;
		default:
			break;
			}
		//fall through
	case cidEditHex:
LHex:
		cid = cidEditHex;
		pdoce = DOCH::PdochNew(_pdocb, _pcfl, pcki->ctg, pcki->cno);
		break;

	case cidEditGL:
		cls = kclsGL;
		goto LDocg;
	case cidEditAL:
		cls = kclsAL;
		goto LDocg;
	case cidEditGG:
		cls = kclsGG;
		goto LDocg;
	case cidEditAG:
		cls = kclsAG;
		goto LDocg;
	case cidEditGST:
		cls = kclsGST;
		goto LDocg;
	case cidEditAST:
		cls = kclsAST;
LDocg:
		pdoce = DOCG::PdocgNew(_pdocb, _pcfl, pcki->ctg, pcki->cno, cls);
		break;

	case cidEditPic:
LPic:
		pdoce = DOCPIC::PdocpicNew(_pdocb, _pcfl, pcki->ctg, pcki->cno);
		break;
	case cidEditMbmp:
LMbmp:
		pdoce = DOCMBMP::PdocmbmpNew(_pdocb, _pcfl, pcki->ctg, pcki->cno);
		break;

	case cidPlaySound:
LSound:
		if ((pcki->ctg == kctgMidi || pcki->ctg == kctgWave) && pvNil != vpsndm)
			{
			// play once
			PCRF pcrf;

			if (pvNil == (pcrf = CRF::PcrfNew(_pcfl, 1)))
				{
				ReleasePpo(&pcrf);
				// edit as hex
				break;
				}

			vpsndm->SiiPlay(pcrf, pcki->ctg, pcki->cno);
			ReleasePpo(&pcrf);
			return;
			}

		// edit as hex
		break;

	default:
		BugVar("unknown cid", &cid);
		return;
		}

	if (pvNil == pdoce)
		{
		if (cid == cidEditNatural)
			{
			//our guess at a natural type may have been wrong, so just
			//edit as hex
			goto LHex;
			}
		else if (cid != cidEditHex)
			{
			vpappb->TGiveAlertSz(PszLit("This chunk doesn't fit that format."),
				bkOk, cokInformation);
			}
		return;
		}

	pdoce->PdmdNew();
	ReleasePpo(&pdoce);
}


/***************************************************************************
	Handle command to compile and add a script chunk.
***************************************************************************/
bool DCD::FCmdImportScript(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	SCCG sccg;
	CKI cki;
	long lnOld;
	FNI fni;
	bool fCreated;
	MSFIL msfil;
	PSCPT pscpt = pvNil;

	//save and clear the sel
	lnOld = _sel.Ln();
	_SetSel(lnNil);

	//get the fni of the file to add
	Mac( FTG ftg = 'TEXT'; )
	if (!FGetFniOpenMacro(&fni, &ftg, 1, PszLit("All files\0*.*\0"), vwig.hwndApp))
		goto LCancel;
	if (pvNil == (pscpt = sccg.PscptCompileFni(&fni,
			pcmd->cid == cidCompileInScript, &msfil)))
		{
		//if the error file isn't empty, open it
		vpappb->TGiveAlertSz(PszLit("Compiling script failed"),
			bkOk, cokExclamation);
		pcmd->cid = cidNil; //don't record
		_SetSel(lnOld);
		OpenSinkDoc(&msfil);
		return fTrue;
		}

	//add the chunk and write the data
	if (!_FAddChunk(kctgScript, &cki, &fCreated))
		goto LCancel;
	if (!pscpt->FSaveToChunk(_pcfl, cki.ctg, cki.cno))
		{
		if (fCreated)
			_pcfl->Delete(cki.ctg, cki.cno);
		vpappb->TGiveAlertSz(PszLit("Writing chunk failed"), bkOk, cokExclamation);
LCancel:
		ReleasePpo(&pscpt);
		pcmd->cid = cidNil; //don't record
		_SetSel(lnOld);
		return fTrue;
		}
	ReleasePpo(&pscpt);

	InvalAllDcd(_pdocb, _pcfl, &cki);
	_SetSel(lnNil, &cki);
	_ShowSel();
	return fTrue;
}


/***************************************************************************
	Draw routine for a test-script gob - just erase the gob.
***************************************************************************/
void TSCG::Draw(PGNV pgnv, RC *prcClip)
{
	pgnv->FillRc(prcClip, kacrWhite);
}


enum
	{
	kiditOkScript,
	kiditCancelScript,
	kiditSizeScript,
	kiditLimScript
	};


/***************************************************************************
	Open a new window and run a script in it.
***************************************************************************/
bool DCD::FCmdTestScript(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	long cbCache;

	if (fselCki != _sel.GrfselGetCkiKid(&cki, pvNil))
		return fFalse;

	if (pcmd->cid == cidRunScriptCache)
		{
		// get the cache size from the user
		PDLG pdlg;

		// get the cache size
		pdlg = DLG::PdlgNew(dlidScriptCache);
		if (pvNil == pdlg)
			return fTrue;

		// put up the initial values
		pdlg->FPutLwInEdit(kiditSizeScript, 3072);
		if (pdlg->IditDo(kiditSizeScript) != kiditOkScript)
			goto LCancel;
		if (!pdlg->FGetLwFromEdit(kiditPixelMbmp, &cbCache) ||
			!FIn(cbCache, 0, kcbMax / 1024))
			{
LCancel:
			ReleasePpo(&pdlg);
			return fTrue;
			}
		cbCache *= 1024;
		ReleasePpo(&pdlg);
		}
	else
		cbCache = 0x00300000L;

	FTestScript(cki.ctg, cki.cno, cbCache);
	return fTrue;
}


/***************************************************************************
	Command handler to Edit a WAVE chunk
***************************************************************************/
bool DCD::FCmdStopSound(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (pvNil != vpsndm)
		vpsndm->StopAll();

	return fTrue;
}


/***************************************************************************
	Command handler to pack or unpack (toggle) a chunk.  Also handle just
	toggling the packed flag.
***************************************************************************/
bool DCD::FCmdPack(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	bool fPack;

	if (fselCki != _sel.GrfselGetCkiKid(&cki, pvNil))
		return fFalse;

	fPack = !_pcfl->FPacked(cki.ctg, cki.cno);
	if (cidPack == pcmd->cid)
		{
		// actually pack or unpack the data
		if (fPack ? _pcfl->FPackData(cki.ctg, cki.cno) :
				_pcfl->FUnpackData(cki.ctg, cki.cno))
			{
			InvalAllDcd(_pdocb, _pcfl, &cki);
			}
		else
			{
			vpappb->TGiveAlertSz(
				fPack ?
					PszLit("Packing failed - chunk may be incompressible...") :
					PszLit("Unpacking failed - chunk may be corrupt"),
				bkOk, cokExclamation);
			}
		}
	else
		{
		// just toggle the packed flag
		_pcfl->SetPacked(cki.ctg, cki.cno, fPack);
		InvalAllDcd(_pdocb, _pcfl, &cki);
		}

	return fTrue;
}


/***************************************************************************
	Run a script. Make the crf cache cbCache large.
***************************************************************************/
bool DCD::FTestScript(CTG ctg, CNO cno, long cbCache)
{
	AssertThis(0);
	AssertIn(cbCache, 0, kcbMax);
	STN stn;
	PTSCG ptscg;
	PSCPT pscpt = pvNil;
	PSCEG psceg = pvNil;
	PCRF pcrf = pvNil;

	GCB gcb(khidMdi, GOB::PgobScreen());
	if (pvNil == (ptscg = NewObj TSCG(&gcb)))
		goto LFail;
	vpcex->FAddCmh(ptscg, 0);

	stn.FFormatSz(PszLit("Run Script: %f %08x"), ctg, cno);
	if (!ptscg->FCreateAndAttachMdi(&stn) ||
		pvNil == (pcrf = CRF::PcrfNew(_pcfl, cbCache)) ||
		pvNil == (psceg = ptscg->PscegNew(pcrf, ptscg)) ||
		pvNil == (pscpt = (PSCPT)pcrf->PbacoFetch(ctg, cno, SCPT::FReadScript)) ||
		!psceg->FRunScript(pscpt))
		{
LFail:
		vpappb->TGiveAlertSz(PszLit("Running script failed"),
			bkOk, cokExclamation);
		}

	ReleasePpo(&psceg);
	ReleasePpo(&pcrf);
	ReleasePpo(&pscpt);
	return fFalse;
}


/***************************************************************************
	Disassemble a script and display it in a new window.
***************************************************************************/
bool DCD::FCmdDisasmScript(PCMD pcmd)
{
	CKI cki;
	PSCPT pscpt;
	SCCG sccg;
	FNI fni;
	MSFIL msfil, msfilError;

	if (fselCki != _sel.GrfselGetCkiKid(&cki, pvNil))
		return fFalse;

	if (pvNil == (pscpt = SCPT::PscptRead(_pcfl, cki.ctg, cki.cno)))
		{
		vpappb->TGiveAlertSz(PszLit("Error reading script (or it's not a script)"),
			bkOk, cokExclamation);
		return fTrue;
		}

	if (!sccg.FDisassemble(pscpt, &msfil, &msfilError))
		{
		vpappb->TGiveAlertSz(PszLit("Error disassembling script"),
			bkOk, cokExclamation);
		OpenSinkDoc(&msfilError);
		}
	else
		OpenSinkDoc(&msfil);
	ReleasePpo(&pscpt);
	return fTrue;
}


/***************************************************************************
	Copy the selection to a new document.
***************************************************************************/
bool DCD::_FCopySel(PDOCB *ppdocb)
{
	AssertNilOrVarMem(ppdocb);
	CKI cki;
	PDOC pdoc;

	if (fselCki != _sel.GrfselGetCkiKid(&cki, pvNil))
		return fFalse;

	if (pvNil == ppdocb)
		return fTrue;

	if (pvNil != (pdoc = DOC::PdocNew(pvNil)))
		{
		if (!_pcfl->FCopy(cki.ctg, cki.cno, pdoc->Pcfl(), &cki.cno))
			ReleasePpo(&pdoc);
		}

	*ppdocb = pdoc;
	return pvNil != *ppdocb;
}


/***************************************************************************
	Delete the current selection.
***************************************************************************/
void DCD::_ClearSel(void)
{
	ulong grfsel;
	CMD cmd;

	grfsel = _sel.GrfselGetCkiKid(pvNil, pvNil);
	if (!(grfsel & fselCki))
		return;
	ClearPb(&cmd, size(cmd));
	cmd.pcmh = this;
	if (grfsel & fselKid)
		cmd.cid = cidUnadoptChunk;
	else
		cmd.cid = cidDeleteChunk;
	FDoCmd(&cmd);
	ReleasePpo(&cmd.pgg);
}


/***************************************************************************
	Paste all non-child chunk of the given document into the current document.
	REVIEW shonk: should this delete the current selection?
	REVIEW shonk: is there an easy way to make this atomic?
***************************************************************************/
bool DCD::_FPaste(PCLIP pclip, bool fDoIt, long cid)
{
	AssertThis(0);
	AssertPo(pclip, 0);
	PDOC pdoc;
	PCFL pcfl;
	long icki;
	CKI cki, ckiSel;
	bool fFailed = fFalse;

	if (!pclip->FGetFormat(kclsDOC))
		return fFalse;

	if (!fDoIt)
		return fTrue;

	if (!pclip->FGetFormat(kclsDOC, (PDOCB *)&pdoc))
		return fFalse;

	if (pvNil == (pcfl = pdoc->Pcfl()) || pcfl->Ccki() <= 0)
		{
		ReleasePpo(&pdoc);
		return fTrue;
		}

	_SetSel(lnNil);
	for (icki = 0; pcfl->FGetCki(icki, &cki); icki++)
		{
		if (!pcfl->FLoner(cki.ctg, cki.cno))
			continue;
		if (cid == cidPasteSpecial)
			fFailed |= !pcfl->FClone(cki.ctg, cki.cno, _pcfl, &ckiSel.cno);
		else
			fFailed |= !pcfl->FCopy(cki.ctg, cki.cno, _pcfl, &ckiSel.cno);
		ckiSel.ctg = cki.ctg;
		}

	InvalAllDcd(_pdocb, _pcfl);
	ReleasePpo(&pdoc);

	if (fFailed)
		{
		vpappb->TGiveAlertSz(PszLit("Couldn't paste everything"),
			bkOk, cokExclamation);
		}
	else
		{
		_SetSel(lnNil, &ckiSel);
		_ShowSel();
		}

	return fTrue;
}


/***************************************************************************
	Use the selected chunk as the current color table.
***************************************************************************/
bool DCD::FCmdSetColorTable(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki;
	BLCK blck;
	PGL pglclr;

	if (fselCki != _sel.GrfselGetCkiKid(&cki, pvNil))
		return fFalse;
	if (!_pcfl->FFind(cki.ctg, cki.cno, &blck))
		return fFalse;

	if (pvNil != (pglclr = GL::PglRead(&blck)) && pglclr->CbEntry() == size(CLR))
		GPT::SetActiveColors(pglclr, fpalIdentity);

	ReleasePpo(&pglclr);
	return fTrue;
}



enum
	{
	kiditOkFilter,
	kiditCancelFilter,
	kiditHideKidsFilter,
	kiditHideListFilter,
	kiditCtgEditFilter,
	kiditLimFilter,
	};


/***************************************************************************
	Change the filtering.
***************************************************************************/
bool DCD::FCmdFilterChunk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	STN stn;
	STN stnT;
	PDLG pdlg;
	ulong grfsel;
	CKI cki;
	KID kid;
	CTG ctg;
	long ictg;
	achar *prgch;
	achar chQuote, *psz;

	grfsel = _sel.GrfselGetCkiKid(&cki, &kid);
	_SetSel(lnNil);

	//put up the dialog
	pdlg = DLG::PdlgNew(dlidFilter);
	if (pvNil == pdlg)
		goto LCancel;

	stn.SetNil();
	for (ictg = 0; _sel.FGetCtgFilter(ictg, &ctg); ictg++)
		{
		stnT.FFormatSz(PszLit("'%f'"), ctg);
		if (stn.Cch() > 0)
			stn.FAppendCh(kchSpace);
		stn.FAppendStn(&stnT);
		}

	pdlg->FPutStn(kiditCtgEditFilter, &stn);
	pdlg->PutCheck(kiditHideKidsFilter, _sel.FHideKids());
	pdlg->PutCheck(kiditHideListFilter, _sel.FHideList());

	if (kiditOkFilter != pdlg->IditDo(kiditCtgEditFilter))
		goto LCancel;

	// set the filtering on the sel
	_sel.HideKids(pdlg->FGetCheck(kiditHideKidsFilter));
	_sel.HideList(pdlg->FGetCheck(kiditHideListFilter));

	pdlg->GetStn(kiditCtgEditFilter, &stn);
	_sel.FreeFilterList();
	for (psz = stn.Psz(); ; )
		{
		while (*psz == kchSpace)
			psz++;
		if (!*psz)
			break;

		stnT.SetNil();
		chQuote = 0;
		while (*psz && (chQuote != 0 || *psz != kchSpace))
			{
			if (chQuote == 0 && (*psz == ChLit('"') || *psz == ChLit('\'')))
				chQuote = *psz;
			if (*psz == chQuote)
				chQuote = 0;
			else
				stnT.FAppendCh(*psz);
			psz++;
			}

		if (stnT.Cch() == 0)
			continue;

		while (stnT.Cch() < 4)
			stnT.FAppendCh(kchSpace);
		prgch = stnT.Psz();
		ctg = LwFromBytes((byte)prgch[0], (byte)prgch[1], (byte)prgch[2],
			(byte)prgch[3]);
		_sel.FAddCtgFilter(ctg);
		}

LCancel:
	ReleasePpo(&pdlg);
	_InvalCkiKid();
	_SetSel(lnNil, (grfsel & fselCki) ? &cki : pvNil,
		(grfsel & fselKid) ? &kid : pvNil);
	_ShowSel();

	return fTrue;
}


/***************************************************************************
	Clone a chunk and its subgraph
***************************************************************************/
bool DCD::FCmdCloneChunk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CKI cki, ckiNew;

	if (fselCki != _sel.GrfselGetCkiKid(&cki, pvNil))
		goto LFail;

	_SetSel(lnNil);
	ckiNew.ctg = cki.ctg;
	if (!_pcfl->FClone(cki.ctg, cki.cno, _pcfl, &ckiNew.cno))
		{
		_SetSel(lnNil, &cki);
		vpappb->TGiveAlertSz(PszLit("Cloning failed"), bkOk, cokExclamation);
LFail:
		pcmd->cid = cidNil; // don't record
		return fTrue;
		}

	InvalAllDcd(_pdocb, _pcfl);
	_SetSel(lnNil, &ckiNew);
	_ShowSel();
	return fTrue;

}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of an object.
***************************************************************************/
void DCD::AssertValid(ulong grf)
{
	DCD_PAR::AssertValid(0);
	AssertPo(&_sel, 0);
	AssertPo(_pcfl, 0);
}


/***************************************************************************
	Mark memory for the DCD.
***************************************************************************/
void DCD::MarkMem(void)
{
	AssertValid(0);
	DCD_PAR::MarkMem();
	_sel.MarkMem();
}
#endif //DEBUG


/***************************************************************************
	Parses the stn as a ctg. Pads with spaces. Fails if pstn is longer
	than 4 characters or empty.
***************************************************************************/
bool FGetCtgFromStn(CTG *pctg, PSTN pstn)
{
	achar rgch[4];

	AssertVarMem(pctg);
	AssertPo(pstn, 0);

	if (!FIn(pstn->Cch(), 1, 5))
		{
		TrashVar(pctg);
		return fFalse;
		}
	rgch[0] = rgch[1] = rgch[2] = rgch[3] = kchSpace;
	CopyPb(pstn->Psz(), rgch, pstn->Cch() * size(achar));

	//first character becomes the high byte
	*pctg = LwFromBytes((byte)rgch[0], (byte)rgch[1], (byte)rgch[2],
		(byte)rgch[3]);
	return fTrue;
}


/***************************************************************************
	Get the indicated edit item from the dialog and convert it to a CTG.
	Pads with spaces. Fails if the text in the edit item is longer
	than 4 characters or empty.
***************************************************************************/
bool _FGetCtg(PDLG pdlg, long idit, CTG *pctg)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pctg);
	STN stn;

	pdlg->GetStn(idit, &stn);
	return FGetCtgFromStn(pctg, &stn);
}


/***************************************************************************
	Put the ctg into the indicated edit item.
***************************************************************************/
void _PutCtgStn(PDLG pdlg, long idit, CTG ctg)
{
	AssertPo(pdlg, 0);
	STN stn;

	stn.FFormatSz(PszLit("%f"), ctg);
	pdlg->FPutStn(idit, &stn);
}


/***************************************************************************
	Constructor for SEL class.
***************************************************************************/
SEL::SEL(PCFL pcfl)
{
	AssertPo(pcfl, 0);
	_pcfl = pcfl;
	_lnLim = lnNil;
	_pglctg = pvNil;
	_SetNil();
	AssertThis(0);
}


/***************************************************************************
	Constructor for a selection based on another selection. If the source
	selection is filtered, this selection will share the same filter list.
***************************************************************************/
SEL::SEL(SEL &selT)
{
	_pglctg = pvNil;
	*this = selT;
}


/***************************************************************************
	Destructor for a selection. Release the GL of ctg's to filter on.
***************************************************************************/
SEL::~SEL(void)
{
	ReleasePpo(&_pglctg);
}


/***************************************************************************
	Assignment operator from one selection to another. If the source
	selection is filtered, this selection will share the same filter list.
***************************************************************************/
SEL & SEL::operator = (SEL &selT)
{
	PGL pglctgOld = _pglctg;

	SEL_PAR::operator=(selT);
	CopyPb(PvAddBv(&selT, size(SEL_PAR)), PvAddBv(this, size(SEL_PAR)),
		size(SEL) - size(SEL_PAR));
	if (pvNil != _pglctg)
		_pglctg->AddRef();
	ReleasePpo(&pglctgOld);
	return *this;
}


/***************************************************************************
	Set the selection to a nil selection.
***************************************************************************/
void SEL::_SetNil(void)
{
	_icki = ivNil;
	_ikid = ivNil;
	_ln = lnNil;
	AssertThis(0);
}


/***************************************************************************
	Get the cki and kid. Return which of the elements are valid. One or
	both of pcki, pkid may be nil.
***************************************************************************/
ulong SEL::GrfselGetCkiKid(CKI *pcki, KID *pkid)
{
	AssertThis(0);
	AssertNilOrVarMem(pcki);
	AssertNilOrVarMem(pkid);

	TrashVar(pcki);
	TrashVar(pkid);
	if (ivNil == _icki)
		return fselNil;
	if (pvNil != pcki)
		*pcki = _cki;
	if (ivNil == _ikid)
		return fselCki;
	if (pvNil != pkid)
		*pkid = _kid;
	return fselCki | fselKid;
}


/***************************************************************************
	Set the selection to the given line. Return true iff the resulting
	selection is not nil.
***************************************************************************/
bool SEL::FSetLn(long ln)
{
	AssertThis(0);
	Assert(ln == lnNil || ln >= 0 && ln < kcbMax, "bad ln");

	if (ln == _ln)
		return fTrue;

	if (lnNil == ln || lnNil != _lnLim && ln >= _lnLim)
		{
		_SetNil();
		return fFalse;
		}

	if (_ln > ln)
		{
		if (_ln - ln < ln)
			{
			// move backwards
			SuspendAssertValid();
			SuspendCheckPointers();
			while (_ln > ln)
				AssertDo(FRetreat(), 0);
			ResumeAssertValid();
			ResumeCheckPointers();
			return fTrue;
			}
		_SetNil();
		}

	Assert(_ln < ln, "what?");
	SuspendAssertValid();
	SuspendCheckPointers();
	while (_ln < ln)
		{
		if (!FAdvance())
			return fFalse;
		}
	ResumeCheckPointers();
	ResumeAssertValid();

	return fTrue;
}


/***************************************************************************
	Get the number of active lines in the sel.
***************************************************************************/
long SEL::LnLim(void)
{
	AssertThis(0);

	if (lnNil == _lnLim)
		{
		SEL sel = *this;

		_lnLim = (lnNil == _ln) ? 0 : _ln + 1;
		SuspendAssertValid();
		SuspendCheckPointers();
		while (sel.FAdvance())
			_lnLim++;
		ResumeAssertValid();
		ResumeCheckPointers();
		}
	return _lnLim;
}


/***************************************************************************
	Advance the selection to the next item. If the selection is nil,
	advances it to the first item. Returns false and makes the selection
	nil iff there is no next item.
***************************************************************************/
bool SEL::FAdvance(void)
{
	AssertThis(0);

	if (lnNil == _ln)
		{
		_ln = 0;
		_icki = 0;
		}
	else
		{
		_ln++;
		if (!_fHideKids)
			{
			// check for the next child
			for (_ikid = (ivNil == _ikid) ? 0 : _ikid + 1;
					_pcfl->FGetKid(_cki.ctg, _cki.cno, _ikid, &_kid); _ikid++)
				{
				if (_FFilter(_kid.cki.ctg, _kid.cki.cno))
					return fTrue;
				}
			}
		_icki++;
		}

	_ikid = ivNil;
	for ( ; _pcfl->FGetCki(_icki, &_cki); _icki++)
		{
		if (_FFilter(_cki.ctg, _cki.cno))
			return fTrue;
		}

	_SetNil();
	return fFalse;
}


/***************************************************************************
	Move the selection to the previous item. If the selection is nil or
	at the first item, this sets the selection nil and returns false.
***************************************************************************/
bool SEL::FRetreat(void)
{
	AssertThis(0);

	if (lnNil == _ln)
		return fFalse;
	if (0 == _ln)
		goto LSetNil;

	_ln--;
	if (ivNil == _ikid)
		{
		// find the previous cki
		do
			{
			if (_icki-- <= 0)
				{
				Bug("initial _ln was wrong");
LSetNil:
				_SetNil();
				return fFalse;
				}
			AssertDo(_pcfl->FGetCki(_icki, &_cki, &_ikid), 0);
			}
		while (!_FFilter(_cki.ctg, _cki.cno));

		if (_ikid == 0 || _fHideKids)
			{
			_ikid = ivNil;
			return fTrue;
			}
		}

	//move to the previous child
	while (--_ikid >= 0)
		{
		AssertDo(_pcfl->FGetKid(_cki.ctg, _cki.cno, _ikid, &_kid), 0);
		if (_FFilter(_kid.cki.ctg, _kid.cki.cno))
			return fTrue;
		}
	_ikid = ivNil;
	return fTrue;
}


/***************************************************************************
	Set the selection to represent *pcki, *pkid. pkid may be nil. If
	fExact is false, this sets the sel to the last item at or before
	the given (pcki, pkid).
***************************************************************************/
bool SEL::FSetCkiKid(CKI *pcki, KID *pkid, bool fExact)
{
	AssertThis(0);
	AssertVarMem(pcki);
	AssertNilOrVarMem(pkid);

	_cki = *pcki;
	_icki = 0;
	if (pvNil == pkid)
		_ikid = ivNil;
	else
		{
		_kid = *pkid;
		_ikid = 0;
		}
	_ln = 0;

	Adjust(fExact);
	AssertThis(0);
	return ivNil != _icki;
}


/***************************************************************************
	Adjust the sel after an edit to the doc. Assume icki and ikid are wrong
	(except as indicators of invalid cki and kid fields), and assume ln
	is wrong. If fExact is false and the current (cki, kid) is no longer
	in the CFL or our filtering on it, we select the item immediately before
	where this one would be.
***************************************************************************/
void SEL::Adjust(bool fExact)
{
	AssertPo(_pcfl, 0);
	long icki, ikid;

	if (ivNil == _icki)
		goto LSetNil;

	// get the icki and ikid
	if (!_pcfl->FGetIcki(_cki.ctg, _cki.cno, &icki) ||
			!_FFilter(_cki.ctg, _cki.cno))
		{
		if (fExact)
			goto LSetNil;
		ikid = ivNil;
		}
	else if (ivNil != (ikid = _ikid))
		{
		if (_fHideKids)
			{
			if (fExact)
				goto LSetNil;
			ikid = ivNil;
			}
		else if (!_pcfl->FGetIkid(_cki.ctg, _cki.cno, _kid.cki.ctg, _kid.cki.cno,
				_kid.chid, &ikid) && fExact)
			{
LSetNil:
			_SetNil();
			return;
			}
		}

	// find the icki and ikid
	_SetNil();
	SEL sel = *this;

	SuspendAssertValid();
	SuspendCheckPointers();
	while (sel.FAdvance())
		{
		if (sel._icki >= icki && (sel._icki > icki || sel._ikid > ikid))
			break;
		*this = sel;
		}
	ResumeAssertValid();
	ResumeCheckPointers();

	if (fExact && (_icki != icki || _ikid != ikid))
		_SetNil();
}


/***************************************************************************
	Hide or show children according to fHide.
***************************************************************************/
void SEL::HideKids(bool fHide)
{
	AssertThis(0);

	if (FPure(fHide) == _fHideKids)
		return;
	_fHideKids = FPure(fHide);
	Adjust();
}


/***************************************************************************
	Hide or show the filter list according to fHide.
***************************************************************************/
void SEL::HideList(bool fHide)
{
	AssertThis(0);

	if (FPure(fHide) == _fHideList)
		return;
	_fHideList = FPure(fHide);
	Adjust();
}


/***************************************************************************
	Get the ictg'th ctg that we're filtering on.
***************************************************************************/
bool SEL::FGetCtgFilter(long ictg, CTG *pctg)
{
	AssertThis(0);
	AssertVarMem(pctg);

	if (pvNil == _pglctg || !FIn(ictg, 0, _pglctg->IvMac()))
		return fFalse;
	_pglctg->Get(ictg, pctg);
	return fTrue;
}


/***************************************************************************
	Reset the filter list to be empty.
***************************************************************************/
void SEL::FreeFilterList(void)
{
	AssertThis(0);

	if (pvNil != _pglctg)
		{
		ReleasePpo(&_pglctg);
		Adjust();
		}
}


/***************************************************************************
	Add an element to the filter list.
***************************************************************************/
bool SEL::FAddCtgFilter(CTG ctg)
{
	AssertThis(0);

	if (pvNil == _pglctg && pvNil == (_pglctg = GL::PglNew(size(CTG), 1)))
		return fFalse;
	if (!_pglctg->FAdd(&ctg))
		return fFalse;
	Adjust();
	return fTrue;
}


/***************************************************************************
	Return true iff (ctg, cno) passes our filtering criteria.
***************************************************************************/
bool SEL::_FFilter(CTG ctg, CNO cno)
{
	long cctg;
	CTG *qctg;

	if (pvNil == _pglctg || 0 == (cctg = _pglctg->IvMac()))
		return fTrue;

	for (qctg = (CTG *)_pglctg->QvGet(0); cctg-- > 0; qctg++)
		{
		if (*qctg == ctg)
			return !_fHideList;
		}

	return _fHideList;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the sel.
***************************************************************************/
void SEL::AssertValid(ulong grf)
{
	SEL_PAR::AssertValid(0);
	AssertPo(_pcfl, 0);
	Assert((_ln == lnNil) == (_icki == ivNil && _ikid == ivNil),
		"nil values not in sync");
	AssertNilOrPo(_pglctg, 0);
}


/***************************************************************************
	Mark memory for the SEL.
***************************************************************************/
void SEL::MarkMem(void)
{
	AssertValid(0);
	SEL_PAR::MarkMem();
	MarkMemObj(_pglctg);
}
#endif //DEBUG
