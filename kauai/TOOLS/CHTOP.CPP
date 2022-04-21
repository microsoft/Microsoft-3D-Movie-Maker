/***************************************************************************

	Chunky help topic editor documents and their DDGs.

***************************************************************************/
#include "chelp.h"
ASSERTNAME


RTCLASS(HEDO)
RTCLASS(TSEL)
RTCLASS(HEDG)
RTCLASS(HETD)
RTCLASS(HETG)
RTCLASS(HTRU)

#ifndef UNICODE
#define SPELL
#else //UNICODE
#undef SPELL
#endif //UNICODE


BEGIN_CMD_MAP(HEDG, DDG)
	ON_CID_GEN(cidDeleteTopic, &HEDG::FCmdDeleteTopic, &HEDG::FEnableHedgCmd)
	ON_CID_GEN(cidEditTopic, &HEDG::FCmdEditTopic, &HEDG::FEnableHedgCmd)
	ON_CID_GEN(cidNewTopic, &HEDG::FCmdNewTopic, pvNil)
	ON_CID_GEN(cidExportText, &HEDG::FCmdExport, pvNil)
	ON_CID_GEN(cidFind, &HEDG::FCmdFind, &HEDG::FEnableHedgCmd)
	ON_CID_GEN(cidFindAgain, &HEDG::FCmdFind, &HEDG::FEnableHedgCmd)
	ON_CID_GEN(cidPrint, &HEDG::FCmdPrint, &HEDG::FEnableHedgCmd)
	ON_CID_GEN(cidSpellCheck, &HEDG::FCmdCheckSpelling, &HEDG::FEnableHedgCmd)
	ON_CID_GEN(cidDumpText, &HEDG::FCmdDump, &HEDG::FEnableHedgCmd)
END_CMD_MAP_NIL()


BEGIN_CMD_MAP(HETG, TXRG)
	ON_CID_GEN(cidGroupText, &HETG::FCmdGroupText, &HETG::FEnableHetgCmd)
	ON_CID_GEN(cidLineSpacing, &HETG::FCmdLineSpacing, pvNil)
	ON_CID_GEN(cidFormatPicture, &HETG::FCmdFormatPicture, &HETG::FEnableHetgCmd)
	ON_CID_GEN(cidFormatButton, &HETG::FCmdFormatButton, &HETG::FEnableHetgCmd)
	ON_CID_GEN(cidFormatEdit, &HETG::FCmdFormatEdit, &HETG::FEnableHetgCmd)
	ON_CID_GEN(cidInsertEdit, &HETG::FCmdInsertEdit, pvNil)
	ON_CID_GEN(cidEditHtop, &HETG::FCmdEditHtop, pvNil)
	ON_CID_GEN(cidNextTopic, &HETG::FCmdNextTopic, pvNil)
	ON_CID_GEN(cidPrevTopic, &HETG::FCmdNextTopic, pvNil)
	ON_CID_GEN(cidFind, &HETG::FCmdFind, pvNil)
	ON_CID_GEN(cidFindAgain, &HETG::FCmdFind, &HETG::FEnableHetgCmd)
	ON_CID_GEN(cidReplace, &HETG::FCmdFind, &HETG::FEnableHetgCmd)
	ON_CID_GEN(cidReplaceFind, &HETG::FCmdFind, &HETG::FEnableHetgCmd)
	ON_CID_GEN(cidFindNextTopic, &HETG::FCmdFind, pvNil)
	ON_CID_GEN(cidPrint, &HETG::FCmdPrint, pvNil)
	ON_CID_GEN(cidSpellCheck, &HETG::FCmdCheckSpelling, pvNil)
	ON_CID_GEN(cidSaveAs, pvNil, pvNil)
	ON_CID_GEN(cidSaveCopy, pvNil, pvNil)
	ON_CID_GEN(cidFontDialog, &HETG::FCmdFontDialog, pvNil)
END_CMD_MAP_NIL()


bool _fCaseSensitive;

void _TokenizeStn(PSTN pstn);
bool _FDoFindDlg(void);


/***************************************************************************
	Constructor for HEDO class.
***************************************************************************/
HEDO::HEDO(void)
{
}


/***************************************************************************
	Destructor for HEDO class.
***************************************************************************/
HEDO::~HEDO(void)
{
	ReleasePpo(&_pcfl);
}


/***************************************************************************
	Static method to create a new document based on the given fni.
	Use pfni == pvNil to create a new file, non-nil to open an
	existing file.
***************************************************************************/
PHEDO HEDO::PhedoNew(FNI *pfni, PRCA prca)
{
	AssertNilOrPo(pfni, ffniFile);
	AssertPo(prca, 0);
	PCFL pcfl;
	PHEDO phedo;

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

	if (pvNil == (phedo = NewObj HEDO()))
		{
		ReleasePpo(&pcfl);
		return pvNil;
		}

	phedo->_pcfl = pcfl;
	phedo->_prca = prca;
	AssertPo(phedo, 0);
	return phedo;
}


/***************************************************************************
	Create a new DDG for the HEDO.
***************************************************************************/
PDDG HEDO::PddgNew(PGCB pgcb)
{
	AssertThis(0);
	return HEDG::PhedgNew(this, _pcfl, pgcb);
}


/***************************************************************************
	Get the current FNI for the doc.  Return false if the doc is not
	currently based on an FNI (it's a new doc or an internal one).
***************************************************************************/
bool HEDO::FGetFni(FNI *pfni)
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
	that this is a normal save (not save as).  If pfni is not nil and
	fSetFni is false, this just writes a copy of the doc but doesn't change
	the doc one bit.
***************************************************************************/
bool HEDO::FSaveToFni(FNI *pfni, bool fSetFni)
{
	AssertThis(0);
	if (!fSetFni && pvNil != pfni)
		return _pcfl->FSaveACopy(kctgChelp, pfni);

	if (!_pcfl->FSave(kctgChelp, pfni))
		return fFalse;

	_fDirty = fFalse;
	_pcfl->SetTemp(fFalse);

	return fTrue;
}


/***************************************************************************
	Ask the user what file they want to save to.
***************************************************************************/
bool HEDO::FGetFniSave(FNI *pfni)
{
	AssertThis(0);
	AssertPo(pfni, 0);

	return FGetFniSaveMacro(pfni, 'CHN2', "\x9" "Save As: ", "",
		PszLit("All files\0*.*\0"), vwig.hwndApp);
}


/***************************************************************************
	Invalidate all DDGs on this HEDO.  Also dirties the document.  Should be
	called by any code that edits the document.
***************************************************************************/
void HEDO::InvalAllDdg(CNO cno)
{
	AssertThis(0);
	long ipddg;
	PDDG pddg;

	//mark the document dirty
	SetDirty();

	//inform the DDGs
	for (ipddg = 0; pvNil != (pddg = PddgGet(ipddg)); ipddg++)
		{
		if (pddg->FIs(kclsHEDG))
            ((PHEDG)pddg)->InvalCno(cno);
		else
			pddg->InvalRc(pvNil);
		}
}


/***************************************************************************
	Export the help topics in their textual representation for compilation
	by chomp.
	REVIEW shonk: this code is a major hack and very fragile.
***************************************************************************/
bool HEDO::FExportText(void)
{
	AssertThis(0);
	FNI fni;
	PFIL pfil;
	MSFIL msfil;

	if (!FGetFniSaveMacro(&fni, 'TEXT', "\x9" "Save As: ", "",
			PszLit("Chomp files\0*.cht\0"), vwig.hwndApp))
		{
		return fFalse;
		}

	if (pvNil == (pfil = FIL::PfilCreate(&fni)))
		{
		vpappb->TGiveAlertSz(PszLit("Can't create destination file!"),
			bkOk, cokExclamation);
		return fFalse;
		}

	msfil.SetFile(pfil);

	if (!FExportHelpText(_pcfl, &msfil))
		{
		vpappb->TGiveAlertSz(PszLit("Exporting file failed"),
			bkOk, cokExclamation);
		pfil->SetTemp();
		ReleasePpo(&pfil);
		return fFalse;
		}

	ReleasePpo(&pfil);
	return fTrue;
}


/***************************************************************************
	Resume searching in the topic at or after the given one, according to
	fAdvance.
***************************************************************************/
void HEDO::DoFindNext(PHETD phetd, CNO cno, bool fAdvance)
{
	AssertThis(0);
	AssertNilOrPo(phetd, 0);
	Assert(pvNil == phetd || phetd->PdocbPar() == this, "bad topic doc");
	long cpMin, cpLim;
	STN stn;
	PHETG phetg;
	PHETD phetdT;

	if (!vpstrg->FGet(kstidFind, &stn) || stn.Cch() <= 0)
		{
		vpappb->TGiveAlertSz(PszLit("Empty search string"), bkOk, cokExclamation);
		return;
		}

	if (cnoNil != cno)
		{
		if (pvNil != (phetd = HETD::PhetdFromChunk(this, cno)))
			phetd->AddRef();
		else if (pvNil == (phetd = HETD::PhetdNew(this, _prca, _pcfl, cno)))
			{
			// couldn't load the thing
			return;
			}
		}
	else if (pvNil != phetd)
		phetd->AddRef();

	if (fAdvance || pvNil == phetd)
		{
		phetdT = PhetdOpenNext(phetd);
		ReleasePpo(&phetd);
		phetd = phetdT;
		}

	while (pvNil != phetd)
		{
		// search phetd
		AssertPo(phetd, 0);

		if (phetd->FFind(stn.Prgch(), stn.Cch(), 0, &cpMin, &cpLim,
				_fCaseSensitive))
			{
			// found it!
			if (phetd->Cddg() == 0)
				{
				// need to open a window onto the doc.
				phetd->PdmdNew();
				}
			else
				phetd->ActivateDmd();
			phetg = (PHETG)phetd->PddgActive();
			if (pvNil != phetg)
				{
				AssertPo(phetg, 0);
				phetg->SetSel(cpMin, cpLim);
				phetg->ShowSel();
				ReleasePpo(&phetd);
				return;
				}
			}

		phetdT = PhetdOpenNext(phetd);
		ReleasePpo(&phetd);
		phetd = phetdT;
		}
}


/***************************************************************************
	Open the next topic subdocument.  If phetd is nil, open the first one.
***************************************************************************/
PHETD HEDO::PhetdOpenNext(PHETD phetd)
{
	AssertThis(0);
	AssertNilOrPo(phetd, 0);
	Assert(pvNil == phetd || phetd->PdocbPar() == this, "bad topic doc");
	long icki;
	CKI cki;
	PDOCB pdocb;

	if (pvNil == phetd)
		{
		// start the search
		_pcfl->FGetIcki(kctgHelpTopic, 0, &icki);
		}
	else if (cnoNil != (cki.cno = phetd->Cno()))
		{
		_pcfl->FGetIcki(kctgHelpTopic, cki.cno + 1, &icki);
		phetd = pvNil;
		}

	if (pvNil == (pdocb = phetd))
		{
		// icki is valid
		if (_pcfl->FGetCki(icki, &cki) && cki.ctg == kctgHelpTopic)
			{
			if (pvNil != (phetd = HETD::PhetdFromChunk(this, cki.cno)))
				{
				phetd->AddRef();
				return phetd;
				}
			return HETD::PhetdNew(this, _prca, _pcfl, cki.cno);
			}

		// we're done with the saved topics - get the first
		// new unsaved one
		if (pvNil == (pdocb = PdocbChd()))
			return pvNil;
		if (pdocb->FIs(kclsHETD) && ((PHETD)pdocb)->Cno() == cnoNil)
			{
			pdocb->AddRef();
			return (PHETD)pdocb;
			}
		}

	for (;;)
		{
		AssertPo(pdocb, 0);
		pdocb = pdocb->PdocbSib();
		if (pvNil == pdocb)
			break;
		if (pdocb->FIs(kclsHETD) && ((PHETD)pdocb)->Cno() == cnoNil)
			{
			pdocb->AddRef();
			return (PHETD)pdocb;
			}
		}

	return pvNil;
}


/***************************************************************************
	Open the previous topic subdocument.  If phetd is nil, open the last one.
***************************************************************************/
PHETD HEDO::PhetdOpenPrev(PHETD phetd)
{
	AssertThis(0);
	AssertNilOrPo(phetd, 0);
	Assert(pvNil == phetd || phetd->PdocbPar() == this, "bad topic doc");
	long icki;
	CKI cki;
	PDOCB pdocb;
	PHETD phetdNew;

	if (pvNil == phetd || (cki.cno = phetd->Cno()) == cnoNil)
		{
		// look for the last unsaved topic before phetd
		phetdNew = pvNil;
		for (pdocb = PdocbChd(); phetd != pdocb && pvNil != pdocb;
				pdocb = pdocb->PdocbSib())
			{
			if (pdocb->FIs(kclsHETD) && ((PHETD)pdocb)->Cno() == cnoNil)
				phetdNew = (PHETD)pdocb;
			}

		if (pvNil != phetdNew)
			{
			AssertPo(phetdNew, 0);
			phetdNew->AddRef();
			return phetdNew;
			}

		_pcfl->FGetIcki(kctgHelpTopic + 1, 0, &icki);
		}
	else
		_pcfl->FGetIcki(kctgHelpTopic, cki.cno, &icki);

	if (icki > 0 && _pcfl->FGetCki(icki - 1, &cki) && cki.ctg == kctgHelpTopic)
		{
		if (pvNil != (phetdNew = HETD::PhetdFromChunk(this, cki.cno)))
			{
			phetdNew->AddRef();
			return phetdNew;
			}
		return HETD::PhetdNew(this, _prca, _pcfl, cki.cno);
		}

	return pvNil;
}



#ifdef DEBUG
/***************************************************************************
	Assert the validity of the HEDO.
***************************************************************************/
void HEDO::AssertValid(ulong grf)
{
	HEDO_PAR::AssertValid(grf);
	AssertPo(_pcfl, 0);
}
#endif //DEBUG


/***************************************************************************
	Constructor for TSEL class.
***************************************************************************/
TSEL::TSEL(PCFL pcfl)
{
	AssertPo(pcfl, 0);
	_pcfl = pcfl;
	_SetNil();
	AssertThis(0);
}


/***************************************************************************
	Set the selection to a nil selection.
***************************************************************************/
void TSEL::_SetNil(void)
{
	_icki = ivNil;
	_cno = cnoNil;
	AssertThis(0);
}


/***************************************************************************
	Set the selection to the given line.  Return true iff the resulting
	selection is not nil.
***************************************************************************/
bool TSEL::FSetIcki(long icki)
{
	AssertThis(0);
	CKI cki;

	if (icki == ivNil || !_pcfl->FGetCkiCtg(kctgHelpTopic, icki, &cki))
		{
		_SetNil();
		return fFalse;
		}

	_cno = cki.cno;
	_icki = icki;
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Set the selection to the given cno.
***************************************************************************/
bool TSEL::FSetCno(CNO cno)
{
	AssertThis(0);

	_cno = cno;
	_icki = 0;

	Adjust();
	AssertThis(0);
	return ivNil != _icki;
}


/***************************************************************************
	Adjust the sel after an edit to the doc.  Assume icki is wrong
	(except as indicators of invalid cno).
***************************************************************************/
void TSEL::Adjust(void)
{
	AssertPo(_pcfl, 0);
	long dicki;

	if (ivNil == _icki || !_pcfl->FGetIcki(kctgHelpTopic, _cno, &_icki))
		_SetNil();
	_pcfl->FGetIcki(kctgHelpTopic, 0, &dicki);
	_icki -= dicki;
	AssertThis(0);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the sel.
***************************************************************************/
void TSEL::AssertValid(ulong grf)
{
	TSEL_PAR::AssertValid(0);
	AssertPo(_pcfl, 0);
	Assert((_cno == cnoNil) == (_icki == ivNil), "nil values not in sync");
}
#endif //DEBUG


/***************************************************************************
	Constructor for the HEDG.
***************************************************************************/
HEDG::HEDG(PHEDO phedo, PCFL pcfl, PGCB pgcb) : DDG(phedo, pgcb), _tsel(pcfl)
{
	AssertPo(pcfl, 0);
	RC rc;
	achar ch = kchSpace;
	GNV gnv(this);

	_pcfl = pcfl;

	_onn = vpappb->OnnDefFixed();
	gnv.SetOnn(_onn);
	gnv.GetRcFromRgch(&rc, &ch, 1, 0, 0);
	_dypLine = rc.Dyp();
	_dxpChar = rc.Dxp();
	_dypHeader = _dypLine;
	AssertThis(0);
}


/***************************************************************************
	Static method to create a new HEDG.
***************************************************************************/
PHEDG HEDG::PhedgNew(PHEDO phedo, PCFL pcfl, PGCB pgcb)
{
	PHEDG phedg;

	if (pvNil == (phedg = NewObj HEDG(phedo, pcfl, pgcb)))
		return pvNil;

	if (!phedg->_FInit())
		{
		ReleasePpo(&phedg);
		return pvNil;
		}
	phedg->Activate(fTrue);

	AssertPo(phedg, 0);
	return phedg;
}


/***************************************************************************
	We're being activated or deactivated, invert the sel.
***************************************************************************/
void HEDG::_Activate(bool fActive)
{
	AssertThis(0);
	DDG::_Activate(fActive);

	GNV gnv(this);
	_DrawSel(&gnv);
}


/***************************************************************************
	Invalidate the display from cno to the end of the display.  If we're
	the active HEDG, also redraw.
***************************************************************************/
void HEDG::InvalCno(CNO cno)
{
	AssertThis(0);
	long icki, ickiT;
	RC rc;

	//we need to recalculate the lnLim
	_pcfl->FGetIcki(kctgHelpTopic, cno, &icki);
	_pcfl->FGetIcki(kctgHelpTopic, 0, &ickiT);
	icki -= ickiT;

	//correct the sel
	ickiT = _tsel.Icki();
	if (ivNil != ickiT && ickiT >= icki)
		_tsel.Adjust();

	GetRc(&rc, cooLocal);
	rc.ypTop = LwMax(0, _YpFromIcki(icki));
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
	Draw the topic list.
***************************************************************************/
void HEDG::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	STN stn, stnT;
	RC rc;
	long yp, xp;
	long icki;
	CKI cki;

	pgnv->ClipRc(prcClip);
	pgnv->FillRc(prcClip, kacrWhite);
	pgnv->SetOnn(_onn);
	xp = _XpFromIch(0);

	//draw the header
	stn = PszLit("  Hex         CNO     Name");
	pgnv->DrawStn(&stn, xp, 0);
	pgnv->GetRcSrc(&rc);
	rc.ypTop = _dypHeader - 1;
	rc.ypBottom = _dypHeader;
	pgnv->FillRc(&rc, kacrBlack);

	//use the sel to find the first icki to draw
	icki = _IckiFromYp(LwMax(prcClip->ypTop, _dypHeader));
	for (yp = _YpFromIcki(icki);
			yp < prcClip->ypBottom && _pcfl->FGetCkiCtg(kctgHelpTopic, icki, &cki);
			icki++)
		{
		//draw the cki description
		_pcfl->FGetName(cki.ctg, cki.cno, &stnT);
		stn.FFormatSz(PszLit("%08x %10d   \"%s\""), cki.cno, cki.cno, &stnT);
		pgnv->DrawStn(&stn, xp, yp);
		yp += _dypLine;
		}

	//draw the selection
	if (_fActive)
		_DrawSel(pgnv);
}


/***************************************************************************
	Hilite the selection (if there is one)
***************************************************************************/
void HEDG::_DrawSel(PGNV pgnv)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	RC rc;
	long icki;

	if (ivNil == (icki = _tsel.Icki()))
		return;

	pgnv->GetRcSrc(&rc);
	rc.ypTop = _YpFromIcki(icki);
	rc.ypBottom = rc.ypTop + _dypLine;
	rc.ypTop = LwMax(rc.ypTop, _dypHeader);
	pgnv->HiliteRc(&rc, kacrWhite);
}


/***************************************************************************
	Set the selection to the given icki or cno.  If cno is not cnoNil,
	this uses the cno, otherwise it uses the icki.  If both are nil, it
	clears the selection.
***************************************************************************/
void HEDG::_SetSel(long icki, CNO cno)
{
	AssertThis(0);

	if (cnoNil != cno)
		{
		TSEL tsel(_pcfl);

		tsel.FSetCno(cno);
		icki = tsel.Icki();
		}

	if (_tsel.Icki() == icki)
		return;

	GNV gnv(this);

	//erase the old sel
	if (_fActive)
		_DrawSel(&gnv);

	//set the new sel and draw it
	_tsel.FSetIcki(icki);
	if (_fActive)
		_DrawSel(&gnv);
}


/***************************************************************************
	Scroll the sel into view.
***************************************************************************/
void HEDG::_ShowSel(void)
{
	AssertThis(0);
	RC rc;
	long icki, ccki;

	if (ivNil == (icki = _tsel.Icki()))
		return;

	if (icki < _scvVert)
		_Scroll(scaNil, scaToVal, 0, icki);
	else
		{
		_GetContent(&rc);
		ccki = LwMax(_scvVert + 1, _IckiFromYp(rc.ypBottom));
		if (icki >= ccki)
			_Scroll(scaNil, scaToVal, 0, _scvVert + icki + 1 - ccki);
		}
}


/***************************************************************************
	Handle a mouse down in our content.
***************************************************************************/
void HEDG::MouseDown(long xp, long yp, long cact, ulong grfcust)
{
	AssertThis(0);
	long icki, ickiNew;

	if (ivNil != (ickiNew = _IckiFromYp(yp)))
		{
		if ((icki = _tsel.Icki()) != ickiNew)
			_SetSel(ickiNew);
		ickiNew = _tsel.Icki();
		}

	if (!_fActive)
		Activate(fTrue);

	if (ivNil == ickiNew)
		return;

	if (cact > 1 && icki == ickiNew)
		_EditTopic(_tsel.Cno());
}


/***************************************************************************
	Handle key input.
***************************************************************************/
bool HEDG::FCmdKey(PCMD_KEY pcmd)
{
	AssertThis(0);
	long icki, ccki, ickiNew, cckiPage;
	RC rc;

	icki = _tsel.Icki();
	ccki = _pcfl->CckiCtg(kctgHelpTopic);
	switch (pcmd->vk)
		{
	case kvkDown:
		ickiNew = (0 > icki) ? 0 : (icki + 1) % ccki;
		goto LChangeSel;

	case kvkUp:
		ickiNew = (0 > icki) ? ccki - 1 : (icki + ccki - 1) % ccki;
		goto LChangeSel;

	case kvkPageUp:
	case kvkPageDown:
		_GetContent(&rc);
		cckiPage = LwMax(1, rc.Dyp() / _dypLine - 1);
		if (pcmd->vk == kvkPageDown)
			ickiNew = LwMin(ccki - 1, ivNil == icki ? cckiPage : icki + cckiPage);
		else
			ickiNew = LwMax(0, ivNil == icki ? 0 : icki - cckiPage);
		goto LChangeSel;

	case kvkHome:
		ickiNew = 0;
		goto LChangeSel;

	case kvkEnd:
		ickiNew = ccki - 1;
LChangeSel:
		if (ccki == 0)
			{
			Assert(ivNil == icki, "no lines, but non-nil sel");
			break;
			}

		AssertIn(ickiNew, 0, ccki);
		_SetSel(ickiNew);
		_ShowSel();
		break;

	case kvkDelete:
	case kvkBack:
		if (ivNil != icki && tYes == vpappb->TGiveAlertSz(
			PszLit("Are you sure you want to delete this topic?"),
			bkYesNo, cokQuestion))
			{
			_ClearSel();
			}
		break;

	case kvkReturn:
		//edit the topic
		if (ivNil != icki)
			_EditTopic(_tsel.Cno());
		break;

	default:
		break;
		}

	return fTrue;
}


/***************************************************************************
	Return the maximum for the indicated scroll bar.
***************************************************************************/
long HEDG::_ScvMax(bool fVert)
{
	AssertThis(0);
	if (fVert)
		{
		RC rc;

		_GetContent(&rc);
		return LwMax(0, _pcfl->CckiCtg(kctgHelpTopic) - rc.Dyp() / _dypLine + 1);
		}
	return 320;
}


/***************************************************************************
	Handle enabling/disabling HEDG commands.
***************************************************************************/
bool HEDG::FEnableHedgCmd(PCMD pcmd, ulong *pgrfeds)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	AssertVarMem(pgrfeds);
	CKI cki;
	STN stn;

	*pgrfeds = fedsEnable;
	switch (pcmd->cid)
		{
	default:
		if (ivNil == _tsel.Icki())
			*pgrfeds = fedsDisable;
		break;

	case cidFindAgain:
		if (!vpstrg->FGet(kstidFind, &stn) || stn.Cch() <= 0)
			*pgrfeds = fedsDisable;
		// fall thru
	case cidFind:
	case cidPrint:
	case cidSpellCheck:
	case cidDumpText:
		if (!_pcfl->FGetCkiCtg(kctgHelpTopic, 0, &cki) &&
				pvNil == Phedo()->PdocbChd())
			{
			*pgrfeds = fedsDisable;
			}
		break;
		}

	return fTrue;
}


/***************************************************************************
	Handle command to delete a chunk.
***************************************************************************/
bool HEDG::FCmdDeleteTopic(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	_ClearSel();
	return fTrue;
}


/***************************************************************************
	Handles commands to edit a topic.
***************************************************************************/
bool HEDG::FCmdEditTopic(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (ivNil == _tsel.Icki())
		return fTrue;

	_EditTopic(_tsel.Cno());
	return fTrue;
}


/***************************************************************************
	Create and edit a new topic in the help file.
***************************************************************************/
bool HEDG::FCmdNewTopic(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	_EditTopic(cnoNil);
	return fTrue;
}


/***************************************************************************
	Create and edit a new topic in the help file.
***************************************************************************/
bool HEDG::FCmdExport(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	Phedo()->FExportText();
	return fTrue;
}


/***************************************************************************
	Opens a window onto the given topic.
***************************************************************************/
void HEDG::_EditTopic(CNO cno)
{
	AssertThis(0);
	PHETD phetd;

	//check for a hetd already open on the chunk.
	if (cnoNil != cno && pvNil != (phetd = HETD::PhetdFromChunk(_pdocb, cno)))
		{
		phetd->ActivateDmd();
		return;
		}

	phetd = HETD::PhetdNew(_pdocb, Phedo()->Prca(), _pcfl, cno);
	if (pvNil != phetd)
		phetd->PdmdNew();

	ReleasePpo(&phetd);
}


/***************************************************************************
	Copy the selection to a new document.
***************************************************************************/
bool HEDG::_FCopySel(PDOCB *ppdocb)
{
	AssertThis(0);
	AssertNilOrVarMem(ppdocb);
	CNO cno;
	PHEDO phedo;

	if (ivNil == _tsel.Icki())
		return fFalse;

	if (pvNil == ppdocb)
		return fTrue;

	if (pvNil != (phedo = HEDO::PhedoNew(pvNil, Phedo()->Prca())) &&
			!_pcfl->FCopy(kctgHelpTopic, _tsel.Cno(), phedo->Pcfl(), &cno))
		{
		ReleasePpo(&phedo);
		}

	*ppdocb = phedo;
	return pvNil != *ppdocb;
}


/***************************************************************************
	Delete the current selection.
***************************************************************************/
void HEDG::_ClearSel(void)
{
	AssertThis(0);
	CNO cno;

	if (ivNil == _tsel.Icki())
		return;

	cno = _tsel.Cno();
	_pcfl->Delete(kctgHelpTopic, cno);
	Phedo()->InvalAllDdg(cno);
	HETD::CloseDeletedHetd(_pdocb);
}


/***************************************************************************
	Paste all the topics of the given document into the current document.
***************************************************************************/
bool HEDG::_FPaste(PCLIP pclip, bool fDoIt, long cid)
{
	AssertThis(0);
	AssertPo(pclip, 0);
	PHEDO phedo;
	PCFL pcfl;
	long icki;
	CKI cki;
	CNO cnoSel;
	bool fFailed = fFalse;

	if (cidPaste != cid || !pclip->FGetFormat(kclsHEDO))
		return fFalse;

	if (!fDoIt)
		return fTrue;

	if (!pclip->FGetFormat(kclsHEDO, (PDOCB *)&phedo))
		return fFalse;

	if (pvNil == (pcfl = phedo->Pcfl()) || pcfl->CckiCtg(kctgHelpTopic) <= 0)
		{
		ReleasePpo(&phedo);
		return fTrue;
		}

	_SetSel(ivNil);
	for (icki = 0; pcfl->FGetCkiCtg(kctgHelpTopic, icki, &cki); icki++)
		fFailed |= !pcfl->FClone(cki.ctg, cki.cno, _pcfl, &cnoSel);

	Phedo()->InvalAllDdg(0);
	if (fFailed)
		vpappb->TGiveAlertSz(PszLit("Couldn't paste everything"), bkOk, cokExclamation);
	else
		{
		_SetSel(ivNil, cnoSel);
		_ShowSel();
		}

	ReleasePpo(&phedo);
	return fTrue;
}


/***************************************************************************
	Get the content part of the HEDG minus header (and any future footer).
***************************************************************************/
void HEDG::_GetContent(RC *prc)
{
	GetRc(prc, cooLocal);
	prc->ypTop += _dypHeader;
}


/***************************************************************************
	Return the icki that corresponds with the given yp value.  If yp is in
	the header, returns ivNil.
***************************************************************************/
long HEDG::_IckiFromYp(long yp)
{
	AssertThis(0);
	if (yp < _dypHeader)
		return ivNil;
	return _scvVert + (yp - _dypHeader) / _dypLine;
}


/***************************************************************************
	Perform a scroll according to scaHorz and scaVert.
***************************************************************************/
void HEDG::_Scroll(long scaHorz, long scaVert, long scvHorz, long scvVert)
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
void HEDG::_ScrollDxpDyp(long dxp, long dyp)
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
	Do a find in some topics.
***************************************************************************/
bool HEDG::FCmdFind(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	switch (pcmd->cid)
		{
	case cidFind:
		if (!_FDoFindDlg())
			break;
		// fall thru
	case cidFindAgain:
		Phedo()->DoFindNext(pvNil, _tsel.Cno(), fFalse);
		break;
		}

	return fTrue;
}


/***************************************************************************
	Print some topics.
***************************************************************************/
bool HEDG::FCmdPrint(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

#ifdef WIN
	const long kdypFontTitle = 9;
	const long kdzpBox = 2;
	long icki;
	CKI cki;
	PDOCB pdocb;
	PRINTDLG pd;
	DOCINFO di;
	STN stn, stnT;
	STN stnDoc;
	RC rcPage, rcSrc, rcDst, rcT;
	long onnDef;
	long yp, ypTopic;
	long dxpTopic;
	long dypLine, dyp;
	long ilin;
	HTOP htop;

	PHETD phetd = pvNil;
	PHETG phetg = pvNil;
	PGPT pgpt = pvNil;
	PGNV pgnv = pvNil;
	long lwPage = 1;
	bool fInPage = fFalse;
	PHEDO phedo = Phedo();

	// set up the print dialog structure
	ClearPb(&pd, size(pd));
	pd.lStructSize = size(pd);
	pd.Flags = PD_RETURNDC | PD_HIDEPRINTTOFILE | PD_NOPAGENUMS |
		PD_NOSELECTION | PD_USEDEVMODECOPIES;
	pd.hwndOwner = vwig.hwndApp;

	if (!PrintDlg(&pd))
		goto LFail;

	// see if the device supports BitBlt
	if (!(GetDeviceCaps(pd.hDC, RASTERCAPS) & RC_BITBLT))
		goto LFail;

	if (pvNil == (pgpt = GPT::PgptNew(pd.hDC)))
		goto LFail;
	if (pvNil == (pgnv = NewObj GNV(pgpt)))
		goto LFail;

	rcDst.Zero();
	rcDst.xpRight = GetDeviceCaps(pd.hDC, LOGPIXELSX);
	rcDst.ypBottom = GetDeviceCaps(pd.hDC, LOGPIXELSY);
	pgnv->SetRcDst(&rcDst);
	rcSrc.Zero();
	rcSrc.xpRight = kdzpInch;
	rcSrc.ypBottom = kdzpInch;
	pgnv->SetRcSrc(&rcSrc);

	phedo->GetName(&stnDoc);
	di.cbSize = size(di);
	di.lpszDocName = stnDoc.Psz();
	di.lpszOutput = pvNil;

	if (SP_ERROR == StartDoc(pd.hDC, &di))
		goto LFail;

	rcPage.Set(0, 0, GetDeviceCaps(pd.hDC, HORZRES), GetDeviceCaps(pd.hDC, VERTRES));
	rcPage.Map(&rcDst, &rcSrc);
	rcPage.Inset(kdzpInch, kdzpInch);
	onnDef = vpappb->OnnDefVariable();

	if (0 >= StartPage(pd.hDC))
		goto LFail;
	yp = rcPage.ypTop;

	_StartPage(pgnv, &stnDoc, lwPage++, &rcPage, onnDef);

	// set the topic info font and get its height
	pgnv->SetFont(onnDef, fontNil, kdypFontTitle, tahLeft, tavTop);
	pgnv->GetRcFromRgch(&rcT, pvNil, 0, 0, 0);
	dypLine = rcT.Dyp();

	// print the topics
	for (icki = 0, pdocb = pvNil; ; )
		{
		if (ivNil != icki && _pcfl->FGetCkiCtg(kctgHelpTopic, icki++, &cki))
			{
			// get a saved topic
			if (pvNil != (phetd = HETD::PhetdFromChunk(phedo, cki.cno)))
				phetd->AddRef();
			else if (pvNil == (phetd = HETD::PhetdNew(phedo, phedo->Prca(),
					_pcfl, cki.cno)))
				{
				// couldn't load the thing
				continue;
				}
			}
		else
			{
			// get an unsaved topic
			icki = ivNil;
			if (pvNil == pdocb)
				pdocb = Phedo()->PdocbChd();
			else
                pdocb = pdocb->PdocbSib();

			if (pvNil == pdocb)
				break;
			AssertPo(pdocb, 0);
			if (!pdocb->FIs(kclsHETD) || ((PHETD)pdocb)->Cno() != cnoNil)
				continue;
			phetd = (PHETD)pdocb;
			}
		AssertPo(phetd, 0);

		if (pvNil == (phetg = (PHETG)phetd->PddgGet(0)))
			{
			// need to open a window onto the doc.
			GCB gcb(khidDdg, this);
			if (pvNil == (phetg = (PHETG)phetd->PddgNew(&gcb)))
				goto LFail;
			}
		else
			phetg->AddRef();
		AssertPo(phetg, 0);
		phetd->GetHtop(&htop);
		dxpTopic = phetd->DxpDef();

		if (yp > rcPage.ypTop)
			{
			// see if we should start a new page before the topic
			yp += 3 * dypLine;

			if (yp + phetg->DypLine(0) + dypLine * 3 > rcPage.ypBottom)
				{
				// start a new page
				if (0 >= EndPage(pd.hDC))
					goto LFail;
				if (0 >= StartPage(pd.hDC))
					goto LFail;
				yp = rcPage.ypTop;
				_StartPage(pgnv, &stnDoc, lwPage++, &rcPage, onnDef);
				}
			}

		// draw the topic header stuff
		rcT = rcPage;
		rcT.ypTop = yp;
		rcT.ypBottom = yp + 3 * dypLine;
		rcT.Inset(-3, -3);

		pgnv->SetFont(onnDef, fontNil, kdypFontTitle, tahLeft, tavTop);
		phetd->GetHtopStn(1, &stnT);
		stn.FFormatSz(PszLit("Topic 0x%08x (%s): "), phetd->Cno(), &stnT);
		phetd->GetHtopStn(-1, &stnT);
		stn.FAppendStn(&stnT);
		pgnv->DrawStn(&stn, rcPage.xpLeft, yp);
		yp += dypLine;

		phetd->GetHtopStn(4, &stnT);
		stn.FFormatSz(PszLit("Script: 0x%08x (%s);  Sound: '%f', 0x%08x ("),
			htop.cnoScript, &stnT, htop.ckiSnd.ctg, htop.ckiSnd.cno);
		phetd->GetHtopStn(5, &stnT);
		stn.FAppendStn(&stnT);
		stn.FAppendCh(ChLit(')'));
		pgnv->DrawStn(&stn, rcPage.xpLeft, yp);
		yp += dypLine;

		stn.FFormatSz(PszLit("Topic Width: %d"), dxpTopic);
		pgnv->DrawStn(&stn, rcPage.xpLeft, yp);
		yp += 2 * dypLine;

		ypTopic = yp;
		pgnv->SetPenSize(1, 1);
		pgnv->FrameRc(&rcT, kacrBlack);

		// draw the start box
		rcT.Set(rcPage.xpLeft, yp - kdzpBox, rcPage.xpLeft + dxpTopic, yp);
		pgnv->FillRc(&rcT, kacrBlack);

		// draw the lines
		for (ilin = 0; ; ilin++)
			{
			dyp = phetg->DypLine(ilin);
			if (0 >= dyp)
				break;

			if (ilin > 0 && yp + dyp > rcPage.ypBottom)
				{
				// end the page and start a new one
				ypTopic = -1;

				// draw the topic end box
				rcT.Set(rcPage.xpLeft, yp,
					rcPage.xpLeft + dxpTopic, yp + kdzpBox / 2);
				pgnv->FillRcApt(&rcT, &vaptGray, kacrGray, kacrWhite);

				if (0 >= EndPage(pd.hDC))
					goto LFail;
				if (0 >= StartPage(pd.hDC))
					goto LFail;
				yp = rcPage.ypTop;

				// draw the start box
				rcT.Set(rcPage.xpLeft, yp - kdzpBox / 2,
					rcPage.xpLeft + dxpTopic, yp);
				pgnv->FillRcApt(&rcT, &vaptGray, kacrGray, kacrWhite);

				_StartPage(pgnv, &stnDoc, lwPage++, &rcPage, onnDef);
				}

			phetg->DrawLines(pgnv, &rcPage, rcPage.xpLeft, yp,
				ilin, ilin + 1, ftxtgNoColor);
			yp += dyp;
			}

		// draw the topic end box
		rcT.Set(rcPage.xpLeft, yp, rcPage.xpLeft + dxpTopic, yp + kdzpBox);
		pgnv->FillRc(&rcT, kacrBlack);

		ReleasePpo(&phetg);
		ReleasePpo(&phetd);
		}

	if (0 >= EndPage(pd.hDC))
		goto LFail;

	// end the print job
	if (0 >= EndDoc(pd.hDC))
		{
LFail:
		vpappb->TGiveAlertSz(PszLit("Printing failed"), bkOk, cokExclamation);
		ReleasePpo(&phetd);
		ReleasePpo(&phetg);
		}

	if (pd.hDC != hNil)
		DeleteDC(pd.hDC);
	if (pd.hDevMode != hNil)
		GlobalFree(pd.hDevMode);
	if (pd.hDevNames != hNil)
		GlobalFree(pd.hDevNames);
	ReleasePpo(&pgnv);
	ReleasePpo(&pgpt);
#endif //WIN

	return fTrue;
}


#ifdef WIN
/***************************************************************************
	Print the page number and document name.
***************************************************************************/
void HEDG::_StartPage(PGNV pgnv, PSTN pstnDoc, long lwPage, RC *prcPage, long onn)
{
	STN stn;

	// draw the document name and page number
	pgnv->SetFont(onn, fontNil, 10, tahLeft, tavTop);
	pgnv->DrawStn(pstnDoc, prcPage->xpLeft, prcPage->ypBottom + 12);
	stn.FFormatSz(PszLit("- %d -"), lwPage);
	pgnv->SetFont(onn, fontNil, 10, tahCenter, tavTop);
	pgnv->DrawStn(&stn, prcPage->XpCenter(), prcPage->ypBottom + 12);
}
#endif //WIN


/***************************************************************************
	Check spelling in topics from the selected one on.
***************************************************************************/
bool HEDG::FCmdCheckSpelling(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

#ifdef SPELL
	CNO cno;
	PDMD pdmd;
	long cactT;
	PHETD phetd, phetdT;
	PHETG phetg;
	long cactTotal = 0;
	bool fContinue = fTrue;
	PHEDO phedo = Phedo();

	cno = _tsel.Cno();
	if (cnoNil == cno)
		phetd = phedo->PhetdOpenNext(pvNil);
	else
		{
		if (pvNil != (phetd = HETD::PhetdFromChunk(phedo, cno)))
			phetd->AddRef();
		else
			phetd = HETD::PhetdNew(phedo, phedo->Prca(), _pcfl, cno);
		}

	if (pvNil != vpsplc)
		{
		vpsplc->FlushIgnoreList();
		vpsplc->FlushChangeList(fTrue);
		}

	while (pvNil != phetd)
		{
		// check phetd
		AssertPo(phetd, 0);

		if (phetd->Cddg() == 0)
			{
			// need to open a window onto the doc.
			pdmd = phetd->PdmdNew();
			}
		else
			{
			phetd->ActivateDmd();
			pdmd = pvNil;
			}

		phetg = (PHETG)phetd->PddgActive();
		if (pvNil != phetg)
			{
			AssertPo(phetg, 0);
			fContinue = phetg->FCheckSpelling(&cactT);
			cactTotal += cactT;
			}

		if (pdmd != pvNil)
			{
			if (phetd->FQueryCloseDmd(pdmd))
				ReleasePpo(&pdmd);
			}

		phetdT = fContinue ? phedo->PhetdOpenNext(phetd) : pvNil;
		ReleasePpo(&phetd);
		phetd = phetdT;
		}

	if (fContinue)
		{
		STN stn;

		if (cactTotal == 0)
			stn = PszLit("No corrections made.");
		else
			stn.FFormatSz(PszLit("Corrected %d words."), cactTotal);
		vpappb->TGiveAlertSz(stn.Psz(), bkOk, cokExclamation);
		}
#else //!SPELL
	vpappb->TGiveAlertSz(PszLit("Spell checking not available"),
		bkOk, cokExclamation);
#endif //!SPELL

	return fTrue;
}


/***************************************************************************
	Dump the text of all topics.
***************************************************************************/
bool HEDG::FCmdDump(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	const long kcchMax = 1024;
	achar rgch[kcchMax];
	const long kcchEop = MacWin(1, 2);
	achar rgchEop[] = { kchReturn, kchLineFeed };
	long icki;
	CKI cki;
	PDOCB pdocb;
	FNI fni;
	PFIL pfil;
	long cpMac;
	long cp;
	long cch;
	bool fFirst;
	FP fpCur;

	PHETD phetd = pvNil;
	PHEDO phedo = Phedo();

	if (!FGetFniSaveMacro(&fni, kftgText, "\pFile to dump text to:", "\pDump",
			PszLit("Text Files\0*.txt\0"), vwig.hwndApp))
		{
		return fTrue;
		}

	if (pvNil == (pfil = FIL::PfilCreate(&fni)))
		return fTrue;
	fpCur = 0;

#ifdef UNICODE
	rgch[0] = kchwUnicode;
	pfil->FWriteRgbSeq(rgch, size(achar), &fpCur);
#endif //UNICODE

	// dump the topics
	for (icki = 0, pdocb = pvNil, fFirst = fTrue; ; )
		{
		if (ivNil != icki && _pcfl->FGetCkiCtg(kctgHelpTopic, icki++, &cki))
			{
			// get a saved topic
			if (pvNil != (phetd = HETD::PhetdFromChunk(phedo, cki.cno)))
				phetd->AddRef();
			else if (pvNil == (phetd = HETD::PhetdNew(phedo, phedo->Prca(),
					_pcfl, cki.cno)))
				{
				// couldn't load the thing
				continue;
				}
			}
		else
			{
			// get an unsaved topic
			icki = ivNil;
			if (pvNil == pdocb)
				pdocb = Phedo()->PdocbChd();
			else
                pdocb = pdocb->PdocbSib();

			if (pvNil == pdocb)
				break;
			AssertPo(pdocb, 0);
			if (!pdocb->FIs(kclsHETD) || ((PHETD)pdocb)->Cno() != cnoNil)
				continue;
			phetd = (PHETD)pdocb;
			}
		AssertPo(phetd, 0);

		if (!fFirst)
			{
			pfil->FWriteRgbSeq(rgchEop, kcchEop, &fpCur);
			pfil->FWriteRgbSeq(PszLit("------------------------------"),
				30 * size(achar), &fpCur);
			pfil->FWriteRgbSeq(rgchEop, kcchEop, &fpCur);
			pfil->FWriteRgbSeq(rgchEop, kcchEop, &fpCur);
			}
		else
			fFirst = fFalse;

		cpMac = phetd->CpMac() - 1;
		for (cp = 0; cp < cpMac; cp += cch)
			{
			cch = LwMin(cpMac - cp, kcchMax);
			phetd->FetchRgch(cp, cch, rgch);
			pfil->FWriteRgbSeq(rgch, cch * size(achar), &fpCur);
			}

		pfil->FWriteRgbSeq(rgchEop, kcchEop, &fpCur);
		ReleasePpo(&phetd);
		}

	ReleasePpo(&pfil);

	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of an object.
***************************************************************************/
void HEDG::AssertValid(ulong grf)
{
	HEDG_PAR::AssertValid(0);
	AssertPo(&_tsel, 0);
	AssertPo(_pcfl, 0);
}
#endif //DEBUG


/***************************************************************************
	Static method:  For all HETD children of the DOCB, checks if the chunk
	still exists and nukes the HETD if not.
***************************************************************************/
void HETD::CloseDeletedHetd(PDOCB pdocb)
{
	PDOCB pdocbNext;
	PHETD phetd;

	for (pdocb = pdocb->PdocbChd(); pvNil != pdocb; pdocb = pdocbNext)
		{
		pdocbNext = pdocb->PdocbSib();
		if (!pdocb->FIs(kclsHETD))
			continue;
		phetd = (PHETD)pdocb;
		//NOTE: can't assert the phetd here because the chunk may be gone
		//AssertPo(phetd, 0);
		AssertBasePo(phetd, 0);
		AssertNilOrPo(phetd->_pcfl, 0);
		if (phetd->_cno != cnoNil && pvNil != phetd->_pcfl &&
				!phetd->_pcfl->FFind(kctgHelpTopic, phetd->_cno))
			{
			phetd->CloseAllDdg();
			}
		else
			AssertPo(phetd, 0);
		}
}


/***************************************************************************
	Static method to look for a HETD for the given chunk.
***************************************************************************/
PHETD HETD::PhetdFromChunk(PDOCB pdocb, CNO cno)
{
	AssertPo(pdocb, 0);
	Assert(cnoNil != cno, 0);
	PHETD phetd;

	for (pdocb = pdocb->PdocbChd(); pvNil != pdocb; pdocb = pdocb->PdocbSib())
		{
		if (!pdocb->FIs(kclsHETD))
			continue;
		phetd = (PHETD)pdocb;
		AssertPo(phetd, 0);
		if (phetd->_cno == cno)
			return phetd;
		}
	return pvNil;
}


/***************************************************************************
	Constructor for a help topic document.
***************************************************************************/
HETD::HETD(PDOCB pdocb, PRCA prca, PCFL pcfl, CNO cno) : TXHD(prca, pdocb)
{
	AssertNilOrPo(pcfl, 0);
	_pcfl = pcfl;
	_cno = cno;
}


/***************************************************************************
	Destructor for a help topic editing document.
***************************************************************************/
HETD::~HETD(void)
{
	ReleasePpo(&_pgst);
}


/***************************************************************************
	Static method to read a help topic document from the given
	(pcfl, cno) and using the given prca as the source for pictures
	and buttons.
***************************************************************************/
PHETD HETD::PhetdNew(PDOCB pdocb, PRCA prca, PCFL pcfl, CNO cno)
{
	AssertNilOrPo(pdocb, 0);
	AssertPo(prca, 0);
	AssertNilOrPo(pcfl, 0);
	Assert(pcfl != pvNil || cnoNil == cno, "non-nil cno with nil CFL");
	PHETD phetd;

	if (pvNil == (phetd = NewObj HETD(pdocb, prca, pcfl, cno)))
		return pvNil;

	if ((cnoNil == cno) ? !phetd->_FInit() :
			!phetd->_FReadChunk(pcfl, kctgHelpTopic, cno, fTrue))
		{
		PushErc(ercHelpReadFailed);
		ReleasePpo(&phetd);
		return pvNil;
		}

	if (cnoNil == cno)
		phetd->_dxpDef = 200;

	// force the default font to be Comic Sans MS
	phetd->_stnFontDef = PszLit("Comic Sans MS");
	if (!vntl.FGetOnn(&phetd->_stnFontDef, &phetd->_onnDef))
		phetd->_onnDef = vpappb->OnnDefVariable();
	phetd->_oskFont = koskCur;

	// force the background color to clear
	phetd->SetAcrBack(kacrClear);

	return phetd;
}


/***************************************************************************
	Read the given chunk into this TXRD.
***************************************************************************/
bool HETD::_FReadChunk(PCFL pcfl, CTG ctg, CNO cno, bool fCopyText)
{
	AssertPo(pcfl, 0);
	BLCK blck;
	KID kid;

	if (!HETD_PAR::_FReadChunk(pcfl, ctg, cno, pvNil,
			fCopyText ? ftxhdCopyText : ftxhdNil))
		{
		return fFalse;
		}

	if (pcfl->FGetKidChidCtg(ctg, cno, 0, kctgGst, &kid))
		{
		// read the string table
		if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck) ||
				pvNil == (_pgst = GST::PgstRead(&blck)) ||
				_pgst->IvMac() != 6 && (_pgst->IvMac() != 5 ||
				!_pgst->FAddRgch(PszLit(""), 0)))
			{
			return fFalse;
			}
		}

	pcfl->FGetName(ctg, cno, &_stnDesc);

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Get the name of the document.
***************************************************************************/
void HETD::GetName(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	if (pvNil == _pdocbPar)
		HETD_PAR::GetName(pstn);
	else
		{
		STN stn;

		if (cnoNil == _cno)
			{
			if (_cactUntitled == 0)
				_cactUntitled = ++_cactLast;
			stn.FFormatSz(PszLit(": Untitled Topic %d"), _cactUntitled);
			}
		else if (pvNil != _pdocbPar)
			stn.FFormatSz(PszLit(": Topic %08x"), _cno);

		_pdocbPar->GetName(pstn);
		pstn->FAppendStn(&stn);
		}
}


/***************************************************************************
	Save the document.  Handles only cidSave.  Asserts on cidSaveAs and
	cidSaveCopy.
***************************************************************************/
bool HETD::FSave(long cid)
{
	AssertThis(0);
	CKI cki;

	if (cidSave != cid)
		{
		Bug("Bad cid");
		return fFalse;
		}

	if (pvNil == _pcfl)
		{
		vpappb->TGiveAlertSz(
			PszLit("Can't save this topic - it doesn't belong to a topic file"),
			bkOk, cokExclamation);
		return fFalse;
		}

	if (!FSaveToChunk(_pcfl, &cki, fFalse))
		{
		vpappb->TGiveAlertSz(PszLit("Saving topic failed"), bkOk, cokExclamation);
		return fFalse;
		}

	Assert(cki.ctg == kctgHelpTopic, "wrong ctg");
	if (cnoNil != _cno)
		{
		_pcfl->Delete(kctgHelpTopic, _cno);
		_pcfl->Move(cki.ctg, cki.cno, kctgHelpTopic, _cno);
		}
	else
		_cno = cki.cno;

	_pcfl->FSetName(cki.ctg, _cno, &_stnDesc);

	_fDirty = fFalse;
	if (Phedo() != pvNil)
		Phedo()->InvalAllDdg(0);
	UpdateName();
	return fTrue;
}


/***************************************************************************
	Save a help topic to the given chunky file.  Fill in *pcki with where
	we put the root chunk.
***************************************************************************/
bool HETD::FSaveToChunk(PCFL pcfl, CKI *pcki, bool fRedirectText)
{
	AssertThis(0);
	AssertPo(pcfl, 0);
	AssertVarMem(pcki);
	BLCK blck;
	CNO cno;

	if (!HETD_PAR::FSaveToChunk(pcfl, pcki, fRedirectText))
		return fFalse;

	if (pvNil != _pgst)
		{
		//add the string table chunk and write it
		if (!pcfl->FAddChild(pcki->ctg, pcki->cno, 0, _pgst->CbOnFile(),
				kctgGst, &cno, &blck) || !_pgst->FWrite(&blck))
			{
			pcfl->Delete(pcki->ctg, pcki->cno);
			PushErc(ercHelpSaveFailed);
			return fFalse;
			}
		}

	return fTrue;
}


/***************************************************************************
	Create a new Document MDI window for this help topic.
***************************************************************************/
PDMD HETD::PdmdNew(void)
{
	AssertThis(0);
	PDMD pdmd;
	PGOB pgob;
	RC rcRel, rcAbs;
	long dxpLig, ypT;
	GCB gcb;

	if (pvNil == (pdmd = HETD_PAR::PdmdNew()))
		return pvNil;

	dxpLig = kdxpCellLig + SCB::DxpNormal();
	if (pvNil == (pgob = pdmd->PgobFromHid(khidDmw)))
		goto LFail;
	pgob->GetPos(&rcAbs, &rcRel);
	rcAbs.xpLeft += dxpLig + kdxpCcg;
	pgob->SetPos(&rcAbs, &rcRel);

	rcRel.xpRight = rcRel.xpLeft;
	ypT = rcRel.ypBottom;
	rcRel.ypBottom = rcRel.YpCenter();

	rcAbs.xpRight = rcAbs.xpLeft;
	rcAbs.xpLeft = rcAbs.xpRight - kdxpCcg;
	rcAbs.ypTop = 0;
	rcAbs.ypBottom = kdxpFrameCcg / 2;
	gcb.Set(CMH::HidUnique(), pgob, fgobSibling, kginDefault, &rcAbs, &rcRel);
	if (pvNil == NewObj CCG(&gcb, this, fTrue))
		goto LFail;

	rcAbs.xpRight = rcAbs.xpLeft;
	rcAbs.xpLeft = rcAbs.xpRight - dxpLig;
	gcb.Set(khidLigButton, pgob, fgobSibling, kginDefault, &rcAbs, &rcRel);
	if (pvNil == vapp.PligNew(fTrue, &gcb, this))
		goto LFail;

	rcAbs.ypTop = rcAbs.ypBottom;
	rcAbs.ypBottom = kdxpFrameCcg;
	rcRel.ypTop = rcRel.ypBottom;
	rcRel.ypBottom = ypT;
	gcb.Set(khidLigPicture, pgob, fgobSibling, kginDefault, &rcAbs, &rcRel);
	if (pvNil == vapp.PligNew(fFalse, &gcb, this))
		goto LFail;

	rcAbs.xpLeft = rcAbs.xpRight;
	rcAbs.xpRight = rcAbs.xpLeft + kdxpCcg;
	gcb.Set(CMH::HidUnique(), pgob, fgobSibling, kginDefault, &rcAbs, &rcRel);
	if (pvNil == NewObj CCG(&gcb, this, fFalse))
		{
LFail:
		ReleasePpo(&pdmd);
		return pvNil;
		}

	AssertPo(pdmd, 0);
	return pdmd;
}


/***************************************************************************
	Create a new DDG for the HETD.
***************************************************************************/
PDDG HETD::PddgNew(PGCB pgcb)
{
	AssertThis(0);
	return HETG::PhetgNew(this, pgcb);
}


enum
	{
	kiditOkTopic,
	kiditCancelTopic,
	kiditBalnTopic,
	kiditBalnStnTopic,
	kiditHtopStnTopic,
	kiditHidTopic,
	kiditHidStnTopic,
	kiditHidTargetTopic,
	kiditHidTargetStnTopic,
	kiditScriptTopic,
	kiditScriptStnTopic,
	kiditDxpTopic,
	kiditDypTopic,
	kiditDescriptionTopic,
	kiditCtgSoundTopic,
	kiditCnoSoundTopic,
	kiditCnoSoundStnTopic,
	kiditWidthTopic,
	kiditLimTopic
	};


/***************************************************************************
	Put up a dialog for the user to edit the help topic properties.
***************************************************************************/
void HETD::EditHtop(void)
{
	AssertThis(0);
	PDLG pdlg;
	long dxp;
	STN stn;

	if (pvNil == (pdlg = DLG::PdlgNew(dlidTopicInfo)))
		return;

	if (pvNil != _pgst)
		{
		// initialize the string fields
		_pgst->GetStn(0, &stn);
		pdlg->FPutStn(kiditBalnStnTopic, &stn);
		_pgst->GetStn(1, &stn);
		pdlg->FPutStn(kiditHtopStnTopic, &stn);
		_pgst->GetStn(2, &stn);
		pdlg->FPutStn(kiditHidStnTopic, &stn);
		_pgst->GetStn(3, &stn);
		pdlg->FPutStn(kiditHidTargetStnTopic, &stn);
		_pgst->GetStn(4, &stn);
		pdlg->FPutStn(kiditScriptStnTopic, &stn);
		_pgst->GetStn(5, &stn);
		pdlg->FPutStn(kiditCnoSoundStnTopic, &stn);
		}
	else if (pvNil == (_pgst = GST::PgstNew(0, 6, 0)))
		return;
	else
		{
		stn.SetNil();
		if (!_pgst->FAddStn(&stn) || !_pgst->FAddStn(&stn) ||
				!_pgst->FAddStn(&stn) || !_pgst->FAddStn(&stn) ||
				!_pgst->FAddStn(&stn) || !_pgst->FAddStn(&stn))
			{
			ReleasePpo(&_pgst);
			return;
			}
		}

	pdlg->FPutStn(kiditDescriptionTopic, &_stnDesc);

	// initialize the numeric fields
	pdlg->FPutLwInEdit(kiditBalnTopic, _htop.cnoBalloon);
	pdlg->FPutLwInEdit(kiditHidTopic, _htop.hidThis);
	pdlg->FPutLwInEdit(kiditHidTargetTopic, _htop.hidTarget);
	pdlg->FPutLwInEdit(kiditScriptTopic, _htop.cnoScript);
	pdlg->FPutLwInEdit(kiditDxpTopic, _htop.dxp);
	pdlg->FPutLwInEdit(kiditDypTopic, _htop.dyp);
	if (_htop.ckiSnd.ctg == ctgNil)
		stn = PszLit("WAVE");
	else
		stn.FFormatSz(PszLit("%f"), _htop.ckiSnd.ctg);
	pdlg->FPutStn(kiditCtgSoundTopic, &stn);
	pdlg->FPutLwInEdit(kiditCnoSoundTopic, _htop.ckiSnd.cno);
	pdlg->FPutLwInEdit(kiditWidthTopic, DxpDef());

	if (kiditOkTopic != pdlg->IditDo(kiditBalnTopic))
		{
		ReleasePpo(&pdlg);
		return;
		}

	if (!pdlg->FGetLwFromEdit(kiditBalnTopic, (long *)&_htop.cnoBalloon))
		_htop.cnoBalloon = cnoNil;
	if (!pdlg->FGetLwFromEdit(kiditHidTopic, &_htop.hidThis))
		_htop.hidThis = hidNil;
	if (!pdlg->FGetLwFromEdit(kiditHidTargetTopic, &_htop.hidTarget))
		_htop.hidTarget = hidNil;
	if (!pdlg->FGetLwFromEdit(kiditScriptTopic, (long *)&_htop.cnoScript))
		_htop.cnoScript = cnoNil;
	if (!pdlg->FGetLwFromEdit(kiditDxpTopic, &_htop.dxp))
		_htop.dxp = 0;
	if (!pdlg->FGetLwFromEdit(kiditDypTopic, &_htop.dyp))
		_htop.dyp = 0;
	if (pdlg->FGetLwFromEdit(kiditWidthTopic, &dxp) && FIn(dxp, 1, kcbMax))
		SetDxpDef(dxp);

	if (!pdlg->FGetLwFromEdit(kiditCnoSoundTopic, (long *)&_htop.ckiSnd.cno))
		{
		_htop.ckiSnd.cno = cnoNil;
		_htop.ckiSnd.ctg = kctgWave;
		}
	else
		{
		pdlg->GetStn(kiditCtgSoundTopic, &stn);
		if (!FIn(stn.Cch(), 1, 5))
			{
			_htop.ckiSnd.cno = cnoNil;
			_htop.ckiSnd.ctg = kctgWave;
			}
		else
			{
			achar rgch[4];

			rgch[0] = rgch[1] = rgch[2] = rgch[3] = kchSpace;
			stn.GetRgch(rgch);

			//first character becomes the high byte
			_htop.ckiSnd.ctg = LwFromBytes((byte)rgch[0], (byte)rgch[1],
				(byte)rgch[2], (byte)rgch[3]);
			}
		}

	pdlg->GetStn(kiditBalnStnTopic, &stn);
	_TokenizeStn(&stn);
	_pgst->FPutStn(0, &stn);
	pdlg->GetStn(kiditHtopStnTopic, &stn);
	_TokenizeStn(&stn);
	_pgst->FPutStn(1, &stn);
	pdlg->GetStn(kiditHidStnTopic, &stn);
	_TokenizeStn(&stn);
	_pgst->FPutStn(2, &stn);
	pdlg->GetStn(kiditHidTargetStnTopic, &stn);
	_TokenizeStn(&stn);
	_pgst->FPutStn(3, &stn);
	pdlg->GetStn(kiditScriptStnTopic, &stn);
	_TokenizeStn(&stn);
	_pgst->FPutStn(4, &stn);
	pdlg->GetStn(kiditCnoSoundStnTopic, &stn);
	_TokenizeStn(&stn);
	_pgst->FPutStn(5, &stn);
	pdlg->GetStn(kiditDescriptionTopic, &_stnDesc);
	ReleasePpo(&pdlg);

	SetDirty();
}


/***************************************************************************
	Do a search on this topic document.
***************************************************************************/
bool HETD::FDoFind(long cpMin, long *pcpMin, long *pcpLim)
{
	AssertThis(0);
	AssertVarMem(pcpMin);
	AssertVarMem(pcpLim);
	STN stn;

	if (!vpstrg->FGet(kstidFind, &stn) || stn.Cch() == 0 ||
		!FFind(stn.Psz(), stn.Cch(), cpMin, pcpMin, pcpLim, _fCaseSensitive))
		{
		TrashVar(pcpMin);
		TrashVar(pcpLim);
		return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	Do a replace on this topic document.
***************************************************************************/
bool HETD::FDoReplace(long cp1, long cp2, long *pcpMin, long *pcpLim)
{
	AssertThis(0);
	AssertVarMem(pcpMin);
	AssertVarMem(pcpLim);
	STN stn;

	SortLw(&cp1, &cp2);
	vpstrg->FGet(kstidReplace, &stn);
	*pcpMin = cp1;
	*pcpLim = cp1 + stn.Cch();
	HideSel();
	return FReplaceRgch(stn.Psz(), stn.Cch(), cp1, cp2 - cp1);
}


/***************************************************************************
	Get a string corresponding to an entry in the HTOP. -1 means get the
	topic description.
***************************************************************************/
void HETD::GetHtopStn(long istn, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	if (istn == -1)
		*pstn = _stnDesc;
	else if (pvNil != _pgst && istn < _pgst->IvMac())
		_pgst->GetStn(istn, pstn);
	else
		pstn->SetNil;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a HETD.
***************************************************************************/
void HETD::AssertValid(ulong grf)
{
	HETD_PAR::AssertValid(0);
	AssertNilOrPo(_pcfl, 0);
	Assert(cnoNil == _cno ||
		pvNil != _pcfl && _pcfl->FFind(kctgHelpTopic, _cno), "bad _cno");
	AssertNilOrPo(_pgst, 0);
}


/***************************************************************************
	Mark memory for the HETD.
***************************************************************************/
void HETD::MarkMem(void)
{
	AssertValid(0);
	HETD_PAR::MarkMem();
	MarkMemObj(_pgst);
}
#endif //DEBUG


/***************************************************************************
	Constructor for a help text editing gob.
***************************************************************************/
HETG::HETG(PHETD phetd, PGCB pgcb) : HETG_PAR(phetd, pgcb)
{
	_fMark = fTrue;
}


/***************************************************************************
	Create a new help text editing gob.
***************************************************************************/
PHETG HETG::PhetgNew(PHETD phetd, PGCB pgcb)
{
	AssertPo(phetd, 0);
	AssertVarMem(pgcb);
	PHETG phetg;

	if (pvNil == (phetg = NewObj HETG(phetd, pgcb)))
		return pvNil;
	if (!phetg->_FInit())
		{
		ReleasePpo(&phetg);
		return pvNil;
		}
	phetg->ShowRuler();
	phetg->Activate(fTrue);
	return phetg;
}


/***************************************************************************
	Return the height of the ruler.
***************************************************************************/
long HETG::_DypTrul(void)
{
	AssertThis(0);
	return kdzpInch / 2 + 1;
}


/***************************************************************************
	Create the ruler.
***************************************************************************/
PTRUL HETG::_PtrulNew(PGCB pgcb)
{
	AssertThis(0);
	PAP pap;
	long dyp;

	_FetchPap(LwMin(_cpAnchor, _cpOther), &pap);
	GetNaturalSize(pvNil, &dyp);
	return HTRU::PhtruNew(pgcb, this, pap.dxpTab, _DxpDoc(), dyp,
		kdxpIndentTxtg - _scvHorz, vpappb->OnnDefVariable(), 12, fontNil);
}


enum
	{
	kiditOkPicture,
	kiditCancelPicture,
	kiditNamePicture,
	kiditLimPicture
	};


/***************************************************************************
	Insert a picture into the help text document.
***************************************************************************/
bool HETG::FInsertPicture(PCRF pcrf, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertPo(pcrf, 0);
	Assert(ctg == kctgMbmp, "bad mbmp chunk");
	long cpMin, cpLim;
	PDLG pdlg;
	STN stn;
	long cb;
	byte rgb[kcbMaxDataStn];

	pdlg = DLG::PdlgNew(dlidFormatPicture);
	if (pvNil == pdlg)
		return fFalse;

	pcrf->Pcfl()->FGetName(ctg, cno, &stn);
	_TokenizeStn(&stn);
	pdlg->FPutStn(kiditNamePicture, &stn);

	_SwitchSel(fFalse, ginNil);
	cpMin = LwMin(_cpAnchor, _cpOther);
	cpLim = LwMax(_cpAnchor, _cpOther);

	if (kiditOkPicture != pdlg->IditDo(kiditNamePicture))
		{
		ReleasePpo(&pdlg);
		goto LFail;
		}

	pdlg->GetStn(kiditNamePicture, &stn);
	_TokenizeStn(&stn);
	cb = stn.CbData();
	stn.GetData(rgb);
	ReleasePpo(&pdlg);

	if (Phetd()->FInsertPicture(cno, rgb, cb,
			cpMin, cpLim - cpMin, _fValidChp ? &_chpIns : pvNil))
		{
		cpMin++;
		SetSel(cpMin, cpMin);
		}
	else
		{
LFail:
		_SwitchSel(fTrue, kginMark);
		}

	ShowSel();
	return fTrue;
}


enum
	{
	kiditOkButton,
	kiditCancelButton,
	kiditNameButton,
	kiditTopicButton,
	kiditTopicNameButton,
	kiditLimButton
	};

bool _FDlgFormatButton(PDLG pdlg, long *pidit, void *pv);


/***************************************************************************
	Dialog proc for formatting a button.
***************************************************************************/
bool _FDlgFormatButton(PDLG pdlg, long *pidit, void *pv)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pidit);
	long lw;

	switch (*pidit)
		{
	case kiditCancelButton:
		return fTrue; //dismiss the dialog

	case kiditOkButton:
		if (!pdlg->FGetValues(0, kiditLimButton))
			{
			*pidit = ivNil;
			return fTrue;
			}
		if (!pdlg->FGetLwFromEdit(kiditTopicButton, &lw))
			{
			vpappb->TGiveAlertSz(PszLit("Topic number is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditTopicButton);
			return fFalse;
			}
		return fTrue;

	default:
		break;
		}

	return fFalse;
}


/***************************************************************************
	Insert a button into the help text document.
***************************************************************************/
bool HETG::FInsertButton(PCRF pcrf, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertPo(pcrf, 0);
	Assert(ctg == kctgGokd, "bad button chunk");
	long cpMin, cpLim;
	long lw;
	PDLG pdlg;
	STN stn;
	byte rgb[2 * kcbMaxDataStn];
	long cb;

	pdlg = DLG::PdlgNew(dlidFormatButton, _FDlgFormatButton);
	if (pvNil == pdlg)
		return fFalse;

	pcrf->Pcfl()->FGetName(ctg, cno, &stn);
	_TokenizeStn(&stn);
	pdlg->FPutStn(kiditNameButton, &stn);
	pdlg->FPutLwInEdit(kiditTopicButton, cnoNil);

	_SwitchSel(fFalse, ginNil);
	cpMin = LwMin(_cpAnchor, _cpOther);
	cpLim = LwMax(_cpAnchor, _cpOther);

	if (kiditOkButton != pdlg->IditDo(kiditNameButton) ||
			!pdlg->FGetLwFromEdit(kiditTopicButton, &lw))
		{
		ReleasePpo(&pdlg);
		goto LFail;
		}

	pdlg->GetStn(kiditNameButton, &stn);
	_TokenizeStn(&stn);
	cb = stn.CbData();
	stn.GetData(rgb);
	pdlg->GetStn(kiditTopicNameButton, &stn);
	_TokenizeStn(&stn);
	stn.GetData(rgb + cb);
	cb += stn.CbData();
	ReleasePpo(&pdlg);

	if (Phetd()->FInsertButton(cno, (CNO)lw, rgb, cb,
			cpMin, cpLim - cpMin, _fValidChp ? &_chpIns : pvNil))
		{
		cpMin++;
		SetSel(cpMin, cpMin);
		}
	else
		{
LFail:
		_SwitchSel(fTrue, kginMark);
		}

	ShowSel();
	return fTrue;
}


enum
	{
	kiditOkEdit,
	kiditCancelEdit,
	kiditWidthEdit,
	kiditLimEdit
	};


bool _FDlgFormatEdit(PDLG pdlg, long *pidit, void *pv);


/***************************************************************************
	Dialog proc for formatting an edit control.
***************************************************************************/
bool _FDlgFormatEdit(PDLG pdlg, long *pidit, void *pv)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pidit);
	long lw;

	switch (*pidit)
		{
	case kiditCancelEdit:
		return fTrue; //dismiss the dialog

	case kiditOkEdit:
		if (!pdlg->FGetValues(0, kiditLimEdit))
			{
			*pidit = ivNil;
			return fTrue;
			}
		if (!pdlg->FGetLwFromEdit(kiditWidthEdit, &lw) || !FIn(lw, 10, 20000))
			{
			vpappb->TGiveAlertSz(PszLit("Width is bad"), bkOk, cokStop);
			pdlg->SelectDit(kiditWidthEdit);
			return fFalse;
			}
		return fTrue;

	default:
		break;
		}

	return fFalse;
}


/***************************************************************************
	Insert a text edit control into the help text document.
***************************************************************************/
bool HETG::FCmdInsertEdit(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	long cpMin, cpLim;
	PDLG pdlg;
	ECOS ecos;

	ecos.ctg = 'EDIT';
	pdlg = DLG::PdlgNew(dlidFormatEdit, _FDlgFormatEdit);
	if (pvNil == pdlg)
		return fFalse;

	_SwitchSel(fFalse, ginNil);
	cpMin = LwMin(_cpAnchor, _cpOther);
	cpLim = LwMax(_cpAnchor, _cpOther);

	if (kiditOkEdit != pdlg->IditDo(kiditWidthEdit))
		goto LFail;

	if (!pdlg->FGetLwFromEdit(kiditWidthEdit, &ecos.dxp))
		goto LFail;
	ReleasePpo(&pdlg);

	if (Phetd()->FInsertObject(&ecos, size(ecos), cpMin, cpLim - cpMin,
			_fValidChp ? &_chpIns : pvNil))
		{
		cpMin++;
		SetSel(cpMin, cpMin);
		}
	else
		{
LFail:
		ReleasePpo(&pdlg);
		_SwitchSel(fTrue, kginMark);
		}

	ShowSel();
	return fTrue;
}


/***************************************************************************
	Copy the selection.
***************************************************************************/
bool HETG::_FCopySel(PDOCB *ppdocb)
{
	AssertNilOrVarMem(ppdocb);
	PHETD phetd;

	if (_cpAnchor == _cpOther)
		return fFalse;

	if (pvNil == ppdocb)
		return fTrue;

	if (pvNil != (phetd = HETD::PhetdNew(pvNil, Phetd()->Prca(), pvNil, cnoNil)))
		{
		long cpMin = LwMin(_cpAnchor, _cpOther);
		long cpLim = LwMax(_cpAnchor, _cpOther);

		phetd->SetInternal();
		phetd->SuspendUndo();
		if (!phetd->FReplaceTxrd((PTXRD)_ptxtb, cpMin, cpLim - cpMin,
				0, 0, fdocNil))
			{
			ReleasePpo(&phetd);
			}
		else
			phetd->ResumeUndo();
		}

	*ppdocb = phetd;
	return pvNil != *ppdocb;
}


/***************************************************************************
	Draw extra stuff for the line. In our case we put a box around grouped
	text.
***************************************************************************/
void HETG::_DrawLinExtra(PGNV pgnv, PRC prcClip, LIN *plin,
	long dxp, long yp, ulong grftxtg)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	AssertVarMem(plin);
	long cp, cpLimBox;
	long cpLim = plin->cpMin + plin->ccp;
	PHETD phetd = Phetd();
	RC rc;

	for (cp = plin->cpMin; cp < cpLim; )
		{
		if (phetd->FGrouped(cp, pvNil, &cpLimBox))
			{
			rc.ypTop = yp;
			rc.ypBottom = yp + plin->dyp;
			rc.xpLeft = plin->xpLeft + dxp + _DxpFromCp(plin->cpMin, cp)
				- kdxpIndentTxtg;
			if (cpLimBox >= cpLim)
				rc.xpRight = dxp + _DxpDoc();
			else
				{
				rc.xpRight = plin->xpLeft + dxp +
					_DxpFromCp(plin->cpMin, cpLimBox) - kdxpIndentTxtg;
				}
			pgnv->SetPenSize(1, 1);
			pgnv->FrameRcApt(&rc, &vaptGray, kacrInvert, kacrClear);
			}
		cp = cpLimBox;
		}
}


/***************************************************************************
	Draw the view on the help topic.
***************************************************************************/
void HETG::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	RC rc;
	APT apt = { 0x88, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0x00 };

	GetRc(&rc, cooLocal);
	pgnv->FillRcApt(prcClip, &apt, kacrLtGray, kacrWhite);
	HETG_PAR::Draw(pgnv, prcClip);
}


enum
	{
	kiditOkGroupText,
	kiditCancelGroupText,
	kiditLwGroupText,
	kiditTopicGroupText,
	kiditTopicStnGroupText,
	kiditLimGroupText,
	};


/***************************************************************************
	Handle grouping text.
***************************************************************************/
bool HETG::FCmdGroupText(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	long cpAnchor, cpOther;
	PDLG pdlg;
	CNO cnoTopic;
	STN stnTopic;
	byte bGroup;
	long lw;

	if (_cpAnchor == _cpOther)
		return fTrue;

	_SwitchSel(fFalse, ginNil);
	pdlg = DLG::PdlgNew(dlidGroupText);
	if (pvNil == pdlg)
		goto LCancel;
	Phetd()->FGrouped(LwMin(_cpAnchor, _cpOther), pvNil, pvNil, &bGroup,
		&cnoTopic, &stnTopic);
	pdlg->FPutLwInEdit(kiditLwGroupText, (long)bGroup);
	pdlg->FPutLwInEdit(kiditTopicGroupText, cnoTopic);
	_TokenizeStn(&stnTopic);
	pdlg->FPutStn(kiditTopicStnGroupText, &stnTopic);
	if (kiditOkGroupText != pdlg->IditDo(kiditLwGroupText))
		{
		ReleasePpo(&pdlg);
		goto LCancel;
		}
	if (!pdlg->FGetLwFromEdit(kiditLwGroupText, &lw) || !FIn(lw, 0, 256))
		{
		vpappb->TGiveAlertSz(PszLit("Group number must be between 0 and 255!"),
			bkOk, cokExclamation);
		lw = 0;
		}
	bGroup = (byte)lw;
	if (bGroup != 0)
		{
		pdlg->GetStn(kiditTopicStnGroupText, &stnTopic);
		_TokenizeStn(&stnTopic);
		if (!pdlg->FGetLwFromEdit(kiditTopicGroupText, &lw))
			{
			vpappb->TGiveAlertSz(PszLit("Topic number was bad!"),
				bkOk, cokExclamation);
			cnoTopic = cnoNil;
			}
		else
			cnoTopic = lw;
		}
	ReleasePpo(&pdlg);

	if (Phetd()->FGroupText(cpAnchor = _cpAnchor, cpOther = _cpOther, bGroup,
			cnoTopic, &stnTopic))
		{
       	SetSel(cpAnchor, cpOther);
		ShowSel();
		}
	else
		{
LCancel:
		_SwitchSel(fTrue, kginMark);
		}

	return fTrue;
}


enum
	{
	kiditOkSpace,
	kiditCancelSpace,
	kiditExtraLineSpace,
	kiditNumLineSpace,
	kiditExtraAfterSpace,
	kiditNumAfterSpace,
	kiditLimSpace,
	};


/***************************************************************************
	Put up the line & paragraph spacing dialog and handle any changes.
***************************************************************************/
bool HETG::FCmdLineSpacing(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	PDLG pdlg;
	PAP pap, papOld;
	long lw;

	pdlg = DLG::PdlgNew(dlidSpacing);
	if (pvNil == pdlg)
		return fTrue;
	_FetchPap(LwMin(_cpAnchor, _cpOther), &pap);
	pdlg->FPutLwInEdit(kiditExtraLineSpace, pap.dypExtraLine);
	pdlg->FPutLwInEdit(kiditNumLineSpace, pap.numLine);
	pdlg->FPutLwInEdit(kiditExtraAfterSpace, pap.dypExtraAfter);
	pdlg->FPutLwInEdit(kiditNumAfterSpace, pap.numAfter);

	if (kiditOkSpace != pdlg->IditDo(kiditExtraLineSpace))
		{
		ReleasePpo(&pdlg);
		return fTrue;
		}

	papOld = pap;
	if (pdlg->FGetLwFromEdit(kiditExtraLineSpace, &lw))
		{
		pap.dypExtraLine = (short)lw;
		papOld.dypExtraLine = (short)~lw;
		}
	if (pdlg->FGetLwFromEdit(kiditNumLineSpace, &lw))
		{
		pap.numLine = (short)lw;
		papOld.numLine = (short)~lw;
		}
	if (pdlg->FGetLwFromEdit(kiditExtraAfterSpace, &lw))
		{
		pap.dypExtraAfter = (short)lw;
		papOld.dypExtraAfter = (short)~lw;
		}
	if (pdlg->FGetLwFromEdit(kiditNumAfterSpace, &lw))
		{
		pap.numAfter = (short)lw;
		papOld.numAfter = (short)~lw;
		}
	ReleasePpo(&pdlg);

	FApplyPap(&pap, &papOld);
	return fTrue;
}


/***************************************************************************
	Handle enabling/disabling HETG commands.
***************************************************************************/
bool HETG::FEnableHetgCmd(PCMD pcmd, ulong *pgrfeds)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	AssertVarMem(pgrfeds);
	void *pv;
	long cp, cpT, cb;
	STN stn;

	*pgrfeds = fedsDisable;
	switch (pcmd->cid)
		{
	default:
		if (_cpAnchor != _cpOther)
			*pgrfeds = fedsEnable;
		break;

	case cidFormatPicture:
	case cidFormatButton:
	case cidFormatEdit:
		if (LwAbs(_cpAnchor - _cpOther) > 1)
			break;
		cp = LwMin(_cpAnchor, _cpOther);
		if (!Phetd()->FFetchObject(cp, &cpT, &pv, &cb))
			break;
		if (cp == cpT && cb >= size(CKI))
			{
			switch (*(CTG *)pv)
				{
			case kctgMbmp:
				if (pcmd->cid == cidFormatPicture)
					*pgrfeds = fedsEnable;
				break;
			case kctgGokd:
				if (pcmd->cid == cidFormatButton)
					*pgrfeds = fedsEnable;
				break;
			case kctgEditControl:
				if (pcmd->cid == cidFormatEdit)
					*pgrfeds = fedsEnable;
				break;
				}
			}
		FreePpv(&pv);
		break;

	case cidFindAgain:
	case cidReplace:
	case cidReplaceFind:
		if (vpstrg->FGet(kstidFind, &stn) && stn.Cch() > 0)
			*pgrfeds = fedsEnable;
		break;
		}

	return fTrue;
}


/***************************************************************************
	Allow the user to edit the properties of an embedded picture.
***************************************************************************/
bool HETG::FCmdFormatPicture(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	void *pv;
	PDLG pdlg;
	long cp, cpT, cb;
	byte rgb[size(CKI) + kcbMaxDataStn];
	STN stn;
	CKI *pcki = (CKI *)rgb;

	if (LwAbs(_cpAnchor - _cpOther) > 1)
		return fTrue;

	cp = LwMin(_cpAnchor, _cpOther);
	if (!Phetd()->FFetchObject(cp, &cpT, &pv, &cb))
		return fTrue;

	if (cp != cpT || !FIn(cb, size(CKI), size(rgb) + 1) ||
			((CKI *)pv)->ctg != kctgMbmp)
		{
		FreePpv(&pv);
		return fFalse;
		}

	CopyPb(pv, rgb, cb);
	FreePpv(&pv);

	pdlg = DLG::PdlgNew(dlidFormatPicture);
	if (pvNil == pdlg)
		return fFalse;

	if (cb > size(CKI) && stn.FSetData(rgb + size(CKI), cb - size(CKI)))
		{
		_TokenizeStn(&stn);
		pdlg->FPutStn(kiditNamePicture, &stn);
		}

	_SwitchSel(fFalse, ginNil);
	if (kiditOkPicture != pdlg->IditDo(kiditNamePicture))
		{
		ReleasePpo(&pdlg);
		goto LFail;
		}

	pdlg->GetStn(kiditNamePicture, &stn);
	_TokenizeStn(&stn);
	cb = stn.CbData() + size(CKI);
	stn.GetData(rgb + size(CKI));
	ReleasePpo(&pdlg);

	if (Phetd()->FApplyObjectProps(rgb, cb, cp))
		SetSel(cp, cp + 1);
	else
		{
LFail:
		_SwitchSel(fTrue, kginMark);
		}

	ShowSel();
	return fTrue;
}


/***************************************************************************
	Allow the user to edit the properties of an embedded button.
***************************************************************************/
bool HETG::FCmdFormatButton(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	void *pv;
	PDLG pdlg;
	long cp, cpT, cb, ib, cbRead;
	STN stn;
	byte rgb[size(CKI) + size(long) + 2 * kcbMaxDataStn];
	CKI *pcki = (CKI *)rgb;
	long *plw = (long *)(pcki + 1);

	if (LwAbs(_cpAnchor - _cpOther) > 1)
		return fTrue;

	cp = LwMin(_cpAnchor, _cpOther);
	if (!Phetd()->FFetchObject(cp, &cpT, &pv, &cb))
		return fTrue;

	if (cp != cpT || !FIn(cb, size(CKI) + size(long), size(rgb) + 1) ||
			((CKI *)pv)->ctg != kctgGokd)
		{
		FreePpv(&pv);
		return fFalse;
		}

	CopyPb(pv, rgb, cb);
	FreePpv(&pv);

	pdlg = DLG::PdlgNew(dlidFormatButton, _FDlgFormatButton);
	if (pvNil == pdlg)
		return fFalse;

	ib = size(CKI) + size(long);
	if (cb > ib && stn.FSetData(rgb + ib, cb - ib, &cbRead))
		{
		_TokenizeStn(&stn);
		pdlg->FPutStn(kiditNameButton, &stn);
		if ((ib += cbRead) < cb &&
				stn.FSetData(rgb + ib, cb - ib))
			{
			_TokenizeStn(&stn);
			pdlg->FPutStn(kiditTopicNameButton, &stn);
			}
		}
	pdlg->FPutLwInEdit(kiditTopicButton, *plw);

	_SwitchSel(fFalse, ginNil);
	if (kiditOkButton != pdlg->IditDo(kiditNameButton) ||
			!pdlg->FGetLwFromEdit(kiditTopicButton, plw))
		{
		ReleasePpo(&pdlg);
		goto LFail;
		}

	pdlg->GetStn(kiditNameButton, &stn);
	_TokenizeStn(&stn);
	ib = size(CKI) + size(long);
	stn.GetData(rgb + ib);
	ib += stn.CbData();
	pdlg->GetStn(kiditTopicNameButton, &stn);
	_TokenizeStn(&stn);
	stn.GetData(rgb + ib);
	ib += stn.CbData();
	ReleasePpo(&pdlg);

	if (Phetd()->FApplyObjectProps(rgb, ib, cp))
		SetSel(cp, cp + 1);
	else
		{
LFail:
		_SwitchSel(fTrue, kginMark);
		}

	ShowSel();
	return fTrue;
}


/***************************************************************************
	Allow the user to edit the properties of an embedded edit control.
***************************************************************************/
bool HETG::FCmdFormatEdit(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	void *pv;
	PDLG pdlg;
	long cp, cpT, cb;
	ECOS ecos;

	if (LwAbs(_cpAnchor - _cpOther) > 1)
		return fTrue;

	cp = LwMin(_cpAnchor, _cpOther);
	if (!Phetd()->FFetchObject(cp, &cpT, &pv, &cb))
		return fTrue;

	if (cp != cpT || cb != size(ecos) || *(CTG *)pv != kctgEditControl)
		{
		FreePpv(&pv);
		return fFalse;
		}

	CopyPb(pv, &ecos, size(ecos));
	FreePpv(&pv);

	pdlg = DLG::PdlgNew(dlidFormatEdit, _FDlgFormatEdit);
	if (pvNil == pdlg)
		return fFalse;

	pdlg->FPutLwInEdit(kiditWidthEdit, ecos.dxp);

	_SwitchSel(fFalse, ginNil);
	if (kiditOkEdit != pdlg->IditDo(kiditWidthEdit) ||
			!pdlg->FGetLwFromEdit(kiditWidthEdit, &ecos.dxp))
		{
		ReleasePpo(&pdlg);
		goto LFail;
		}
	ReleasePpo(&pdlg);

	if (Phetd()->FApplyObjectProps(&ecos, size(ecos), cp))
		SetSel(cp, cp + 1);
	else
		{
LFail:
		_SwitchSel(fTrue, kginMark);
		}

	ShowSel();
	return fTrue;
}


/***************************************************************************
	Edit the topic info.
***************************************************************************/
bool HETG::FCmdEditHtop(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	Phetd()->EditHtop();
	return fTrue;
}


/***************************************************************************
	Open the next or previous topic.
***************************************************************************/
bool HETG::FCmdNextTopic(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PHETD phetd;
	PHETD phetdThis = Phetd();
	PHEDO phedo = phetdThis->Phedo();

	if (pvNil == phedo)
		return fTrue;

	phetd = pcmd->cid == cidNextTopic ? phedo->PhetdOpenNext(phetdThis) :
		phedo->PhetdOpenPrev(phetdThis);
	if (pvNil == phetd)
		{
		phetd = pcmd->cid == cidNextTopic ? phedo->PhetdOpenNext(pvNil) :
			phedo->PhetdOpenPrev(pvNil);
		}

	if (pvNil == phetd || phetd == phetdThis)
		{
		ReleasePpo(&phetd);
		return fTrue;
		}

	if (phetdThis->Cno() == cnoNil || phetdThis->FDirty())
		phetdThis = pvNil;

	// open a DMD onto the topic
	if (phetd->Cddg() == 0)
		{
		// need to open a window onto the doc.
		if (phetd->PdmdNew() != pvNil && pvNil != phetdThis)
			phetdThis->CloseAllDdg();
		}
	else
		{
		phetd->ActivateDmd();
		if (pvNil != phetd->PddgActive() && pvNil != phetdThis)
			phetdThis->CloseAllDdg();
		}

	ReleasePpo(&phetd);
	return fTrue;
}


/***************************************************************************
	Handle cidFind and cidFindAgain.  Search for some text.
***************************************************************************/
bool HETG::FCmdFind(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	long cpMin, cpLim;
	PHETD phetd;

	switch (pcmd->cid)
		{
	case cidReplace:
	case cidReplaceFind:
		if (!Phetd()->FDoReplace(_cpAnchor, _cpOther, &cpMin, &cpLim))
			{
			vpappb->TGiveAlertSz(PszLit("Replace failed."), bkOk, cokStop);
			break;
			}
		SetSel(cpMin, cpLim);
		ShowSel();
		if (pcmd->cid == cidReplace)
			break;
		vpappb->UpdateMarked();
		cpMin = LwMin(cpMin + 1, cpLim);
		goto LFind;

	case cidFind:
		if (!_FDoFindDlg())
			break;
		cpMin = LwMin(_cpAnchor, _cpOther);
		goto LFind;

	case cidFindAgain:
		cpMin = LwMin(_cpAnchor, _cpOther);
		cpMin = LwMin(cpMin + 1, LwMax(_cpAnchor, _cpOther));
LFind:
		if (!Phetd()->FDoFind(cpMin, &cpMin, &cpLim))
			{
			vpappb->TGiveAlertSz(PszLit("Search string not found."),
				bkOk, cokStop);
			break;
			}
		SetSel(cpMin, cpLim);
		ShowSel();
		break;

	case cidFindNextTopic:
		phetd = Phetd();
		if (phetd->Phedo() != pvNil)
			phetd->Phedo()->DoFindNext(phetd, phetd->Cno());
		break;
		}

	return fTrue;
}


/***************************************************************************
	Handle printing of a topic.
***************************************************************************/
bool HETG::FCmdPrint(PCMD pcmd)
{
#ifdef WIN
	PRINTDLG pd;
	DOCINFO di;
	PGPT pgpt = pvNil;
	PGNV pgnv = pvNil;
	STN stn;
	RC rc;

	// set up the print dialog structure
	ClearPb(&pd, size(pd));
	pd.lStructSize = size(pd);
	pd.Flags = PD_RETURNDC | PD_HIDEPRINTTOFILE | PD_NOPAGENUMS |
		PD_NOSELECTION | PD_USEDEVMODECOPIES;
	pd.hwndOwner = vwig.hwndApp;

	if (!PrintDlg(&pd))
		goto LFail;

	// see if the device supports BitBlt
	if (!(GetDeviceCaps(pd.hDC, RASTERCAPS) & RC_BITBLT))
		goto LFail;

	if (pvNil == (pgpt = GPT::PgptNew(pd.hDC)))
		goto LFail;
	if (pvNil == (pgnv = NewObj GNV(pgpt)))
		goto LFail;

	rc.Zero();
	rc.xpRight = GetDeviceCaps(pd.hDC, LOGPIXELSX);
	rc.ypBottom = GetDeviceCaps(pd.hDC, LOGPIXELSY);
	pgnv->SetRcDst(&rc);
	rc.xpRight = kdzpInch;
	rc.ypBottom = kdzpInch;
	pgnv->SetRcSrc(&rc);

	Phetd()->GetName(&stn);
	di.cbSize = size(di);
	di.lpszDocName = stn.Psz();
	di.lpszOutput = pvNil;

	if (SP_ERROR == StartDoc(pd.hDC, &di))
		goto LFail;

	if (0 >= StartPage(pd.hDC))
		goto LFail;

	rc.Set(0, 0, GetDeviceCaps(pd.hDC, HORZRES), GetDeviceCaps(pd.hDC, VERTRES));
	rc.Inset(kdzpInch, kdzpInch);

	DrawLines(pgnv, &rc, rc.xpLeft, rc.ypTop, 0, klwMax, ftxtgNoColor);

	if (0 >= EndPage(pd.hDC))
		goto LFail;

	if (0 >= EndDoc(pd.hDC))
		{
LFail:
		vpappb->TGiveAlertSz(PszLit("Printing failed"), bkOk, cokExclamation);
		}

	if (pd.hDC != hNil)
		DeleteDC(pd.hDC);
	if (pd.hDevMode != hNil)
		GlobalFree(pd.hDevMode);
	if (pd.hDevNames != hNil)
		GlobalFree(pd.hDevNames);
	ReleasePpo(&pgnv);
	ReleasePpo(&pgpt);
#endif //WIN

	return fTrue;
}


enum
	{
	kiditIgnoreSpell,
	kiditCancelSpell,
	kiditChangeSpell,
	kiditChangeAllSpell,
	kiditIgnoreAllSpell,
	kiditAddSpell,
	kiditComboSpell,
	kiditBadSpell,
	kiditLimSpell
	};


/***************************************************************************
	Spell check the topic.
***************************************************************************/
bool HETG::FCmdCheckSpelling(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

#ifdef SPELL
	STN stn;
	long cactChanges;

	if (pvNil != vpsplc)
		{
		vpsplc->FlushIgnoreList();
		vpsplc->FlushChangeList(fTrue);
		}

	if (FCheckSpelling(&cactChanges))
		{
		if (cactChanges == 0)
			stn = PszLit("No corrections made.");
		else
			stn.FFormatSz(PszLit("Corrected %d words."), cactChanges);
		vpappb->TGiveAlertSz(stn.Psz(), bkOk, cokExclamation);
		}
#else //!SPELL
	vpappb->TGiveAlertSz(PszLit("Spell checking not available"),
		bkOk, cokExclamation);
#endif //!SPELL

	return fTrue;
}


/***************************************************************************
	Spell check the topic.
***************************************************************************/
bool HETG::FCheckSpelling(long *pcactChanges)
{
	AssertThis(0);
	AssertVarMem(pcactChanges);

#ifdef SPELL
	achar rgch[1024];
	long cpMin, cpMac;
	long cchBuf;
	long ichMin, ichLim;
	long idit;
	long cstn;
	STN stnSrc, stnDst;
	long scrs;
	PDLG pdlg = pvNil;

	*pcactChanges = 0;
	if (pvNil == vpsplc)
		{
		if (ksclidAmerican == vsclid)
			stnSrc = PszLit("Chelp");
		else
			stnSrc.FFormatSz(PszLit("Chp%d"), vsclid);

		if (pvNil == (vpsplc = SPLC::PsplcNew(vsclid, &stnSrc)))
			{
			vpappb->TGiveAlertSz(PszLit("Couldn't load the main dictionary"), bkOk,
				cokExclamation);
			return fFalse;
			}
		}

	cpMin = 0;
	cpMac = _ptxtb->CpMac() - 1;

	for (cchBuf = 0; cpMin < cpMac; )
		{
		if (cchBuf <= 0)
			{
			// fetch more text
			if ((cchBuf = CvFromRgv(rgch)) + cpMin < cpMac)
				{
				// make sure we end on a word boundary
				long cpT = _ptxtb->CpPrev(cpMin + cchBuf, fTrue);
				if (cpT > cpMin)
					cchBuf = cpT - cpMin;
				}
			else
				cchBuf = cpMac - cpMin;

			AssertIn(cchBuf, 1, CvFromRgv(rgch) + 1);
			_ptxtb->FetchRgch(cpMin, cchBuf, rgch);
			}
		AssertIn(cchBuf, 1, CvFromRgv(rgch) + 1);

		if (!vpsplc->FCheck(rgch, cchBuf, &ichMin, &ichLim, &stnDst, &scrs))
			{
			vpappb->TGiveAlertSz(PszLit("Spell checking failed!"),
				bkOk, cokExclamation);
			return fFalse;
			}

		if (ichMin >= cchBuf || ichMin >= ichLim)
			{
			cpMin += cchBuf;
			cchBuf = 0;
			continue;
			}

		// misspelled word
		SetSel(cpMin + ichMin, cpMin + ichLim);

		if (scrs == scrsReturningChangeAlways)
			{
			// change all word - change it and continue on
			goto LChange;
			}

		// put up the dialog
		if (pvNil == pdlg &&
			pvNil == (pdlg = DLG::PdlgNew(dlidCheckSpelling)))
			{
			vpappb->TGiveAlertSz(PszLit("Couldn't create spelling dialog!"),
				bkOk, cokExclamation);
			return fFalse;
			}

		stnSrc.SetRgch(rgch + ichMin, ichLim - ichMin);
		pdlg->FPutStn(kiditBadSpell, &stnSrc);
		if (scrs == scrsReturningChangeOnce)
			pdlg->FPutStn(kiditComboSpell, &stnDst);
		else
			pdlg->FPutStn(kiditComboSpell, &stnSrc);

		// fill in the suggestions
		pdlg->ClearList(kiditComboSpell);
		for (cstn = 0;
			cstn < 20 && vpsplc->FSuggest(rgch + ichMin, ichLim - ichMin,
				cstn == 0, &stnDst);
			cstn++)
			{
			pdlg->FAddToList(kiditComboSpell, &stnDst);
			}

		Clean();
		idit = pdlg->IditDo(kiditComboSpell);

		switch (idit)
			{
		default:
			Bug("unknown button");
			ReleasePpo(&pdlg);
			return fFalse;

		case kiditAddSpell:
			vpsplc->FAddToUser(&stnSrc);
			goto LIgnore;

		case kiditIgnoreAllSpell:
			vpsplc->FIgnoreAll(&stnSrc);
			// fall thru
		case kiditIgnoreSpell:
LIgnore:
			if (cchBuf > ichLim)
				BltPb(rgch + ichLim, rgch, (cchBuf - ichLim) * size(achar));
			cchBuf -= ichLim;
			cpMin += ichLim;
			break;

		case kiditChangeSpell:
		case kiditChangeAllSpell:
			pdlg->GetStn(kiditComboSpell, &stnDst);
			if (!stnDst.FEqualRgch(rgch + ichMin, ichLim - ichMin))
				{
				// tell the spell checker that we're doing a change
				vpsplc->FChange(&stnSrc, &stnDst, idit == kiditChangeAllSpell);

LChange:
				(*pcactChanges)++;
				HideSel();
				if (!_ptxtb->FReplaceRgch(stnDst.Prgch(), stnDst.Cch(),
						cpMin + ichMin, ichLim - ichMin))
					{
					vpappb->TGiveAlertSz(PszLit("Couldn't replace wrong word!"),
						bkOk, cokExclamation);
					ReleasePpo(&pdlg);
					return fFalse;
					}
				}

			if (cchBuf > ichLim)
				BltPb(rgch + ichLim, rgch, (cchBuf - ichLim) * size(achar));
			cchBuf -= ichLim;
			cpMac += stnDst.Cch() + ichMin - ichLim;
			cpMin += stnDst.Cch() + ichMin;
			break;

		case kiditCancelSpell:
			ReleasePpo(&pdlg);
			return fFalse;
			}
		}

	ReleasePpo(&pdlg);
#else //!SPELL
	vpappb->TGiveAlertSz(PszLit("Spell checking not available"),
		bkOk, cokExclamation);
	*pcactChanges = 0;
#endif //!SPELL

	return fTrue;
}


/***************************************************************************
	Handle idle stuff - update the ruler with our new height.
***************************************************************************/
void HETG::InvalCp(long cp, long ccpIns, long ccpDel)
{
	AssertThis(0);
	long dyp;

	HETG_PAR::InvalCp(cp, ccpIns, ccpDel);
	if (pvNil == _ptrul || !_ptrul->FIs(kclsHTRU))
		return;

	GetNaturalSize(pvNil, &dyp);
	((PHTRU)_ptrul)->SetDypHeight(dyp);
}


enum
	{
	kiditOkSize,
	kiditCancelSize,
	kiditSizeSize,
	kiditLimSize
	};


/***************************************************************************
	Get a font size from the user.
***************************************************************************/
bool HETG::_FGetOtherSize(long *pdypFont)
{
	AssertThis(0);
	AssertVarMem(pdypFont);
	PDLG pdlg;
	bool fRet;

	if (pvNil == (pdlg = DLG::PdlgNew(dlidFontSize)))
		return fFalse;

	pdlg->FPutLwInEdit(kiditSizeSize, *pdypFont);

	if (kiditOkSize != pdlg->IditDo(kiditSizeSize))
		{
		ReleasePpo(&pdlg);
		return fFalse;
		}

	fRet = pdlg->FGetLwFromEdit(kiditSizeSize, pdypFont);
	ReleasePpo(&pdlg);
	return fRet;
}


enum
	{
	kiditOkOffset,
	kiditCancelOffset,
	kiditSizeOffset,
	kiditSuperOffset,
	kiditLimOffset
	};


/***************************************************************************
	Get the amount to sub/superscript from the user.
***************************************************************************/
bool HETG::_FGetOtherSubSuper(long *pdypOffset)
{
	AssertThis(0);
	AssertVarMem(pdypOffset);
	PDLG pdlg;
	bool fRet;

	if (pvNil == (pdlg = DLG::PdlgNew(dlidSubSuper)))
		return fFalse;

	pdlg->FPutLwInEdit(kiditSizeSize, LwAbs(*pdypOffset));
	pdlg->PutCheck(kiditSuperOffset, *pdypOffset < 0);

	if (kiditOkOffset != pdlg->IditDo(kiditSizeOffset))
		{
		ReleasePpo(&pdlg);
		return fFalse;
		}

	fRet = pdlg->FGetLwFromEdit(kiditSizeOffset, pdypOffset);
	if (pdlg->FGetCheck(kiditSuperOffset))
		*pdypOffset = -*pdypOffset;
	ReleasePpo(&pdlg);
	return fRet;
}


/***************************************************************************
	Get the height of a particular line. Returns 0 if the line is past
	the end of the document.
***************************************************************************/
long HETG::DypLine(long ilin)
{
	AssertThis(0);
	AssertIn(ilin, 0, kcbMax);
	LIN lin;
	long ilinT;

	_FetchLin(ilin, &lin, &ilinT);
	if (ilin > ilinT)
		return 0;

	return LwMax(1, lin.dyp);
}


/***************************************************************************
	Constructor for a text ruler.
***************************************************************************/
HTRU::HTRU(GCB *pgcb, PTXTG ptxtg) : HTRU_PAR(pgcb)
{
	AssertPo(ptxtg, 0);
	_ptxtg = ptxtg;
}


/***************************************************************************
	Create a new text ruler.
***************************************************************************/
PHTRU HTRU::PhtruNew(GCB *pgcb, PTXTG ptxtg, long dxpTab, long dxpDoc,
	long dypDoc, long xpLeft, long onn, long dypFont, ulong grfont)
{
	AssertVarMem(pgcb);
	AssertPo(ptxtg, 0);
	AssertIn(dxpTab, 1, kcbMax);
	AssertIn(dxpDoc, 1, kcbMax);
	PHTRU phtru;

	if (pvNil == (phtru = NewObj HTRU(pgcb, ptxtg)))
		return pvNil;

	phtru->_dxpTab = dxpTab;
	phtru->_dxpDoc = dxpDoc;
	phtru->_dyp = dypDoc;
	phtru->_xpLeft = xpLeft;
	phtru->_onn = onn;
	phtru->_dypFont = dypFont;
	phtru->_grfont = grfont;
	AssertPo(phtru, 0);
	return phtru;
}


/***************************************************************************
	Draw the ruler.
***************************************************************************/
void HTRU::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	RC rc, rcT;
	STN stn;

	GetRc(&rc, cooLocal);
	pgnv->SetPenSize(0, 1);
	pgnv->FrameRc(&rc, kacrBlack);
	rc.Inset(0, 1);
	pgnv->FillRc(&rc, kacrWhite);

	rcT = rc;
	rcT.ypTop = rc.YpCenter();
	rcT.ypBottom = rcT.ypTop + 1;
	pgnv->FillRc(&rcT, kacrBlack);

	stn.FFormatSz(PszLit("Width = %d; Tab = %d; Height = %d"),
		_dxpDoc, _dxpTab, _dyp);
	pgnv->SetFont(_onn, _grfont, _dypFont);
	pgnv->DrawStn(&stn, rc.xpLeft + 8, rc.ypTop);

	rc.ypTop = rcT.ypBottom;
	rcT = rc;
	rcT.xpLeft = _xpLeft - 1;
	rcT.xpRight = _xpLeft + 1;
	pgnv->FillRcApt(&rcT, &vaptGray, kacrBlack, kacrWhite);

	rcT.Offset(_dxpDoc, 0);
	pgnv->FillRc(&rcT, kacrBlack);

	rcT.Inset(0, 2);
	rcT.Offset(_dxpTab - _dxpDoc, 0);
	pgnv->FillRc(&rcT, kacrBlack);
}


/***************************************************************************
	Track the mouse.
***************************************************************************/
bool HTRU::FCmdTrackMouse(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (pcmd->cid == cidMouseDown)
		{
		Assert(vpcex->PgobTracking() == pvNil, "mouse already being tracked!");
		RC rc;

		GetRc(&rc, cooLocal);
		if (pcmd->yp < rc.YpCenter())
			{
			_rtt = rttNil;
			return fTrue;
			}

		if (LwAbs(pcmd->xp - _xpLeft - _dxpTab) < 3)
			{
			_rtt = krttTab;
			_dxpTrack = _dxpTab - pcmd->xp;
			}
		else if (LwAbs(pcmd->xp - _xpLeft - _dxpDoc) < 3)
			{
			_rtt = krttDoc;
			_dxpTrack = _dxpDoc - pcmd->xp;
			}
		else
			{
			_rtt = rttNil;
			return fTrue;
			}
		vpcex->TrackMouse(this);
		}
	else
		{
		Assert(vpcex->PgobTracking() == this, "not tracking mouse!");
		Assert(pcmd->cid == cidTrackMouse, 0);
		Assert(_rtt != rttNil, "no track type");
		}

	switch (_rtt)
		{
	case krttTab:
		_ptxtg->SetDxpTab(pcmd->xp + _dxpTrack);
		break;
	case krttDoc:
		_ptxtg->SetDxpDoc(pcmd->xp + _dxpTrack);
		break;
		}

	if (pcmd->cid == cidMouseDown)
		_ptxtg->Ptxtb()->SuspendUndo();

	if (!(pcmd->grfcust & fcustMouse))
		{
		_ptxtg->Ptxtb()->ResumeUndo();
		vpcex->EndMouseTracking();
		_rtt = rttNil;
		}

	return fTrue;
}


enum
	{
	kiditOkFont,
	kiditCancelFont,
	kiditComboFont,
	kiditLimFont
	};


/***************************************************************************
	Give the fonts in a dialog.
***************************************************************************/
bool HETG::FCmdFontDialog(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	PDLG pdlg;
	CHP chpNew, chpOld;
	STN stn;
	long onn;

	if (pvNil == (pdlg = DLG::PdlgNew(dlidChooseFont)))
		return fTrue;

	// fill in the font list
	pdlg->ClearList(kiditComboFont);
	for (onn = 0; onn < vntl.OnnMac(); onn++)
		{
		vntl.GetStn(onn, &stn);
		pdlg->FAddToList(kiditComboFont, &stn);
		}

	_EnsureChpIns();
	vntl.GetStn(_chpIns.onn, &stn);
	pdlg->FPutStn(kiditComboFont, &stn);

	if (kiditOkFont == pdlg->IditDo(kiditComboFont))
		{
		pdlg->GetStn(kiditComboFont, &stn);
		if (vntl.FGetOnn(&stn, &onn))
			{
			chpNew.Clear();
			chpOld.Clear();
			chpNew.onn = onn;
			chpOld.onn = ~onn;
			FApplyChp(&chpNew, &chpOld);
			}
		}
	ReleasePpo(&pdlg);

	return fTrue;
}


/***************************************************************************
	Set the tab width.
***************************************************************************/
void HTRU::SetDxpTab(long dxpTab)
{
	AssertThis(0);
	if (dxpTab == _dxpTab)
		return;

	_dxpTab = dxpTab;
	AssertThis(0);
	InvalRc(pvNil, kginMark);
}


/***************************************************************************
	Set the document width.
***************************************************************************/
void HTRU::SetDxpDoc(long dxpDoc)
{
	AssertThis(0);
	if (dxpDoc == _dxpDoc)
		return;

	_dxpDoc = dxpDoc;
	AssertThis(0);
	InvalRc(pvNil, kginMark);
}


/***************************************************************************
	Change the location of the left edge of the document.
***************************************************************************/
void HTRU::SetXpLeft(long xpLeft)
{
	AssertThis(0);
	if (xpLeft == _xpLeft)
		return;

	_xpLeft = xpLeft;
	AssertThis(0);
	InvalRc(pvNil, kginMark);
}


/***************************************************************************
	Set the text height.
***************************************************************************/
void HTRU::SetDypHeight(long dyp)
{
	AssertThis(0);
	if (dyp == _dyp)
		return;

	_dyp = dyp;
	AssertThis(0);
	InvalRc(pvNil, kginMark);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a HTRU.
***************************************************************************/
void HTRU::AssertValid(ulong grf)
{
	HTRU_PAR::AssertValid(0);
	AssertPo(_ptxtg, 0);
	AssertIn(_dxpTab, 1, kcbMax);
	AssertIn(_dxpDoc, 1, kcbMax);
}
#endif //DEBUG


/***************************************************************************
	Munge the string so it is a valid token or empty.
***************************************************************************/
void _TokenizeStn(PSTN pstn)
{
	AssertPo(pstn, 0);
	bool fDigitOk;
	SZ sz;
	achar ch;
	achar *pch;

	fDigitOk = fFalse;
	pstn->GetSz(sz);
	for (pch = sz; *pch; pch++, fDigitOk = fTrue)
		{
		ch = *pch;
		if (!FIn(ch, ChLit('A'), ChLit('Z') + 1) &&
				!FIn(ch, ChLit('a'), ChLit('z') + 1))
			{
			if (!fDigitOk || !FIn(ch, ChLit('0'), ChLit('9') + 1))
				{
				*pch = ChLit('_');
				}
			}
		}
	*pch = 0;
	*pstn = sz;
}


enum
	{
	kiditOkFind,
	kiditCancelFind,
	kiditFindFind,
	kiditReplaceFind,
	kiditCaseSensitiveFind,
	kiditLimFind
	};


bool _FDlgFind(PDLG pdlg, long *pidit, void *pv);


/***************************************************************************
	Dialog proc for searching.
***************************************************************************/
bool _FDlgFind(PDLG pdlg, long *pidit, void *pv)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pidit);
	STN stn;

	switch (*pidit)
		{
	case kiditCancelFind:
		return fTrue; //dismiss the dialog

	case kiditOkFind:
		if (!pdlg->FGetValues(0, kiditLimFind))
			{
			*pidit = ivNil;
			return fTrue;
			}
		pdlg->GetStn(kiditFindFind, &stn);
		if (stn.Cch() <= 0)
			{
			vpappb->TGiveAlertSz(PszLit("Empty search string"), bkOk, cokStop);
			pdlg->SelectDit(kiditFindFind);
			return fFalse;
			}
		return fTrue;

	default:
		break;
		}

	return fFalse;
}


/***************************************************************************
	Do the find dialog.  Return true if the user OK'ed the dialog.
***************************************************************************/
bool _FDoFindDlg(void)
{
	PDLG pdlg;
	STN stn;
	bool fRet = fFalse;

	if (pvNil == (pdlg = DLG::PdlgNew(dlidFind, _FDlgFind)))
		return fFalse;
	vpstrg->FGet(kstidFind, &stn);
	pdlg->FPutStn(kiditFindFind, &stn);
	vpstrg->FGet(kstidReplace, &stn);
	pdlg->FPutStn(kiditReplaceFind, &stn);
	pdlg->PutCheck(kiditCaseSensitiveFind, _fCaseSensitive);

	if (kiditOkFind != pdlg->IditDo(kiditFindFind))
		goto LFail;
	pdlg->GetStn(kiditFindFind, &stn);
	if (stn.Cch() <= 0)
		goto LFail;
	vpstrg->FPut(kstidFind, &stn);
	pdlg->GetStn(kiditReplaceFind, &stn);
	vpstrg->FPut(kstidReplace, &stn);
	_fCaseSensitive = FPure(pdlg->FGetCheck(kiditCaseSensitiveFind));
	fRet = fTrue;

LFail:
	ReleasePpo(&pdlg);

	return fRet;
}
