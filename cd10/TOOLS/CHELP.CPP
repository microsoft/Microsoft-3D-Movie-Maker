/***************************************************************************

	Help authoring tool.

***************************************************************************/
#include "chelp.h"
ASSERTNAME


BEGIN_CMD_MAP(APP, APPB)
	ON_CID_GEN(cidNew, &APP::FCmdOpen, pvNil)
	ON_CID_GEN(cidOpen, &APP::FCmdOpen, pvNil)
	ON_CID_GEN(cidOpenText, &APP::FCmdOpen, pvNil)
	ON_CID_GEN(cidOpenRichText, &APP::FCmdOpen, pvNil)
	ON_CID_GEN(cidLoadResFile, &APP::FCmdLoadResFile, pvNil)
	ON_CID_GEN(cidChooseLanguage, &APP::FCmdChooseLanguage,
		&APP::FEnableChooseLanguage)
END_CMD_MAP_NIL()

BEGIN_CMD_MAP(LIG, GOB)
	ON_CID_ME(cidDoScroll, FCmdScroll, pvNil)
	ON_CID_ME(cidEndScroll, FCmdScroll, pvNil)
END_CMD_MAP_NIL()


APP vapp;

RTCLASS(APP)
RTCLASS(LIG)
RTCLASS(LID)
RTCLASS(CCG)
RTCLASS(CCGT)


STRG _strg;
PSTRG vpstrg;
SC_LID vsclid = ksclidAmerican;
PSPLC vpsplc;


/***************************************************************************
	Main for a frame app.
***************************************************************************/
void FrameMain(void)
{
	vpstrg = &_strg;
	vapp.Run(fappNil, fgobNil, kginDefault);
	ReleasePpo(&vpsplc);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a APP.
***************************************************************************/
void APP::AssertValid(ulong grf)
{
	APP_PAR::AssertValid(0);
	AssertNilOrPo(_pcrm, 0);
	AssertNilOrPo(_plidPicture, 0);
	AssertNilOrPo(_plidButton, 0);
}


/***************************************************************************
	Mark memory for the APP.
***************************************************************************/
void APP::MarkMem(void)
{
	AssertValid(0);
	APP_PAR::MarkMem();
	MarkMemObj(_pcrm);
	MarkMemObj(_plidPicture);
	MarkMemObj(_plidButton);
	MarkMemObj(&_strg);
	MarkMemObj(vpsplc);
}
#endif //DEBUG


/***************************************************************************
	Initialize the app.  Add some stuff to the menus and do the command
	line parsing thing.
***************************************************************************/
bool APP::_FInit(ulong grfapp, ulong grfgob, long ginDef)
{
	static long _rgdypFont[] =
		{ 10, 11, 12, 13, 14, 15, 16, 17, 18, 20, 22, 24, 28, 32, 36, 0 };

	struct LANG
		{
		PSZ psz;
		long sclid;
		};
	static LANG _rglang[] =
		{
		{ PszLit("American"), ksclidAmerican },
		{ PszLit("Australian"), ksclidAustralian },
		{ PszLit("British"), ksclidBritish },

		{ PszLit("German"), ksclidGerman },
		{ PszLit("Swiss German"), ksclidSwissGerman },

		{ PszLit("French"), ksclidFrench },
		{ PszLit("French Canadian"), ksclidFrenchCanadian },

		{ PszLit("Spanish"), ksclidSpanish },
		{ PszLit("Catalan"), ksclidCatalan },

		{ PszLit("Italian"), ksclidItalian },

		{ PszLit("Other..."), 0 },
		};

	long iv, dyp;
	STN stn;

	if (!APP_PAR::_FInit(grfapp, grfgob, ginDef))
		return fFalse;

	vpmubCur->FRemoveAllListCid(cidChooseFontSize);
	stn = PszLit("Other...");
	vpmubCur->FAddListCid(cidChooseFontSize, 0, &stn);
	for (iv = 0; (dyp = _rgdypFont[iv]) != 0; iv++)
		{
		stn.FFormatSz(PszLit("%d"), dyp);
		vpmubCur->FAddListCid(cidChooseFontSize, dyp, &stn);
		}

	for (iv = 0; iv < CvFromRgv(_rglang); iv++)
		{
		stn = _rglang[iv].psz;
		vpmubCur->FAddListCid(cidChooseLanguage, _rglang[iv].sclid, &stn);
		}

#ifdef WIN
	//parse the command line and load any resource files and help files
	FNI fni;
	bool fQuote, fRes, fSkip;
	PSZ psz = vwig.pszCmdLine;

	// skip the first token since it is the path
	fSkip = fTrue;
	fRes = fFalse;
	for (;;)
		{
		while (*psz == kchSpace)
			psz++;
		if (!*psz)
			break;

		stn.SetNil();
		fQuote = fFalse;
		while (*psz && (fQuote || *psz != kchSpace))
			{
			if (*psz == ChLit('"'))
				fQuote = !fQuote;
			else
				stn.FAppendCh(*psz);
			psz++;
			}

		if (stn.Cch() == 0)
			continue;

		if (fSkip)
			{
			fSkip = fFalse;
			continue;
			}

		if (stn.Cch() == 2 && (stn.Psz()[0] == ChLit('/') ||
				stn.Psz()[0] == ChLit('-')))
			{
			// command line switch
			switch (stn.Psz()[1])
				{
			case ChLit('r'):
			case ChLit('R'):
				fRes = fTrue;
				break;
				}
			continue;
			}

		if (!fni.FBuildFromPath(&stn) || fni.Ftg() == kftgDir)
			{
			fRes = fFalse;
			continue;
			}

		if (fRes)
			FLoadResFile(&fni);
		else
			FOpenDocFile(&fni);
		fRes = fFalse;
		}
#endif //WIN
	return fTrue;
}


/***************************************************************************
	Get the name for the help editor app.
***************************************************************************/
void APP::GetStnAppName(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

#ifdef UNICODE
	STN stnDate;
	STN stnTime;

	stnDate.SetSzs(__DATE__);
	stnTime.SetSzs(__TIME__);
	pstn->FFormatSz(Debug(PszLit("Debug ")) PszLit("Chelp (Unicode; %s; %s)"),
		&stnDate, &stnTime);
#else //!UNICODE
	*pstn = Debug("Debug ") "Chelp (Ansi; " __DATE__ "; " __TIME__ ")";
#endif //!UNICODE
}


/***************************************************************************
	Update the given window.  *prc is the bounding rectangle of the update
	region.
***************************************************************************/
void APP::UpdateHwnd(HWND hwnd, RC *prc, ulong grfapp)
{
	AssertThis(0);
	PGOB pgob;

	if (pvNil == (pgob = GOB::PgobFromHwnd(hwnd)))
		return;

	//for text windows, do offscreen updating
	if (pgob->FIs(kclsDMD) && ((PDMD)pgob)->Pdocb()->FIs(kclsTXRD))
		grfapp |= fappOffscreen;

	APP_PAR::UpdateHwnd(hwnd, prc, grfapp);
}


/***************************************************************************
	Do a fast update of the gob and its descendents into the given gpt.
***************************************************************************/
void APP::_FastUpdate(PGOB pgob, PREGN pregnClip, ulong grfapp, PGPT pgpt)
{
	AssertThis(0);

	//for text windows, do offscreen updating
	if (pgob->FIs(kclsDMD) && ((PDMD)pgob)->Pdocb()->FIs(kclsTXRD))
		grfapp |= fappOffscreen;

	APP_PAR::_FastUpdate(pgob, pregnClip, grfapp, pgpt);
}


/***************************************************************************
	Open an existing or new chunky file for editing.
	Handles cidNew and cidOpen.
***************************************************************************/
bool APP::FCmdOpen(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	FNI fni;
	FNI *pfni;

	pfni = pvNil;
	switch (pcmd->cid)
		{
	default:
		Bug("why are we here?");
		return fTrue;

	case cidOpen:
		//do the standard dialog
		if (!FGetFniOpenMacro(&fni, pvNil, 0,
				PszLit("Kid Help Files\0*.khp;*.chk\0All Files\0*.*\0"),
				vwig.hwndApp))
			{
			return fTrue;
			}
		pfni = &fni;
		break;

	case cidOpenText:
		//do the standard dialog
		if (!FGetFniOpenMacro(&fni, pvNil, 0,
				PszLit("Text files\0*.txt\0All Files\0*.*\0"), vwig.hwndApp))
			{
			return fTrue;
			}
		pfni = &fni;
		break;

	case cidOpenRichText:
		//do the standard dialog
		if (!FGetFniOpenMacro(&fni, pvNil, 0,
				PszLit("Rich Text files\0*.rtx\0All Files\0*.*\0"), vwig.hwndApp))
			{
			return fTrue;
			}
		pfni = &fni;
		break;

	case cidNew:
		break;
		}

	FOpenDocFile(pfni, pcmd->cid);
	return fTrue;
}


/***************************************************************************
	Load a document file.
***************************************************************************/
bool APP::FOpenDocFile(PFNI pfni, long cid)
{
	AssertThis(0);
	AssertNilOrPo(pfni, 0);
	bool fRet;
	PDOCB pdocb;
	PHEDO phedo;
	PTXRD ptxrd;

	if (pvNil != pfni && pvNil != (pdocb = DOCB::PdocbFromFni(pfni)))
		{
		pdocb->ActivateDmd();
		return fTrue;
		}

	pdocb = pvNil;
	switch (cid)
		{
	case cidOpenText:
		if (pvNil == (ptxrd = TXRD::PtxrdNew(pvNil)))
			return fFalse;

		fRet = fFalse;
		if (pvNil != pfni)
			{
			FLO flo;

			if (pvNil == (flo.pfil = FIL::PfilOpen(pfni)))
				goto LFail;
			flo.fp = 0;
			flo.cb = flo.pfil->FpMac();

			fRet = ptxrd->FReplaceFlo(&flo, fTrue, 0, 0);
			ReleasePpo(&flo.pfil);
			if (!fRet)
				{
LFail:
				ReleasePpo(&ptxrd);
				return fFalse;
				}
			}
		pdocb = ptxrd;
		break;

	case cidOpenRichText:
		if (pvNil == (pdocb = TXRD::PtxrdNew(pfni)))
			return fFalse;
		break;

	default:
		if (pvNil == _pcrm && pvNil == (_pcrm = CRM::PcrmNew(1)))
			return fFalse;
		if (pvNil == _plidPicture &&
			pvNil == (_plidPicture = LID::PlidNew(_pcrm, kctgMbmp)))
			{
			return fFalse;
			}
		if (pvNil == _plidButton &&
			pvNil == (_plidButton = LID::PlidNew(_pcrm, kctgGokd,
				ChidFromSnoDchid(ksnoInit, 0))))
			{
			return fFalse;
			}
	
		phedo = HEDO::PhedoNew(pfni, _pcrm);
		if (pvNil == phedo)
			return fFalse;
		pdocb = phedo;
		break;
		}

	fRet = (pdocb->PdmdNew() != pvNil);
	ReleasePpo(&pdocb);

	return fRet;
}


/***************************************************************************
	Open an existing or new chunky file for editing.
	Handles cidNew and cidOpen.
***************************************************************************/
bool APP::FCmdLoadResFile(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	FNI fni;

	if (!FGetFniOpenMacro(&fni, pvNil, 0,
			PszLit("Chunky Resource Files\0*.chk\0All Files\0*.*\0"),
			vwig.hwndApp))
		{
		return fTrue;
		}

	FLoadResFile(&fni);
	return fTrue;
}


/***************************************************************************
	Load a resource file.
***************************************************************************/
bool APP::FLoadResFile(PFNI pfni)
{
	AssertThis(0);
	AssertPo(pfni, ffniFile);
	PCFL pcfl;
	long ipcrf;
	PCRF pcrf;
	BLCK blck;

	if (pvNil == _pcrm && pvNil == (_pcrm = CRM::PcrmNew(1)))
		return fFalse;

	if (pvNil == (pcfl = CFL::PcflOpen(pfni, fcflNil)))
		{
		vpappb->TGiveAlertSz(PszLit("Can't open that file"), bkOk, cokStop);
		return fFalse;
		}

	//see if it's already in the crm.
	for (ipcrf = _pcrm->Ccrf(); ipcrf-- > 0; )
		{
		pcrf = _pcrm->PcrfGet(ipcrf);
		if (pcfl == pcrf->Pcfl())
			return fTrue;
		}

	if (!_pcrm->FAddCfl(pcfl, 0x100000L))
		{
		ReleasePpo(&pcfl);
		return fFalse;
		}

	if (pcfl->FGetCkiCtg(kctgColorTable, 0, pvNil, pvNil, &blck))
		{
		PGL pglclr;

		if (pvNil != (pglclr = GL::PglRead(&blck)) &&
				pglclr->CbEntry() == size(CLR))
			{
			GPT::SetActiveColors(pglclr, fpalIdentity);
			}
		ReleasePpo(&pglclr);
		}
	ReleasePpo(&pcfl);

	if (pvNil != _plidPicture)
		_plidPicture->FRefresh();
	if (pvNil != _plidButton)
		_plidButton->FRefresh();

	return fTrue;
}


/***************************************************************************
	Check or uncheck the language as appropriate.
***************************************************************************/
bool APP::FEnableChooseLanguage(PCMD pcmd, ulong *pgrfeds)
{
	AssertThis(0);
	AssertPo(pcmd, 0);
	AssertVarMem(pgrfeds);

	*pgrfeds = fedsEnable | fedsUncheck;
	if (vsclid == pcmd->rglw[0])
		*pgrfeds = fedsEnable | fedsCheck;

	return fTrue;
}


enum
	{
	kiditOkLang,
	kiditCancelLang,
	kiditCodeLang,
	kiditLimLang
	};


/***************************************************************************
	Command to choose the language (for spelling).
***************************************************************************/
bool APP::FCmdChooseLanguage(PCMD pcmd)
{
	AssertThis(0);
	AssertPo(pcmd, 0);

	if (pcmd->rglw[0] == 0)
		{
		// ask the user
		PDLG pdlg;
		bool fRet;

		if (pvNil == (pdlg = DLG::PdlgNew(dlidFontSize)))
			return fTrue;

		pdlg->FPutLwInEdit(kiditCodeLang, vsclid);
		if (kiditOkLang != pdlg->IditDo(kiditCodeLang))
			{
			ReleasePpo(&pdlg);
			return fFalse;
			}

		fRet = pdlg->FGetLwFromEdit(kiditCodeLang, &pcmd->rglw[0]);
		ReleasePpo(&pdlg);
		if (!fRet)
			return fTrue;
		}

	if ((SC_LID)pcmd->rglw[0] == vsclid)
		return fTrue;

	ReleasePpo(&vpsplc);
	vsclid = (SC_LID)pcmd->rglw[0];

	return fTrue;
}


/***************************************************************************
	Create a new LIG for the given help text document.
***************************************************************************/
PLIG APP::PligNew(bool fButton, PGCB pgcb, PTXHD ptxhd)
{
	PLID plid = fButton ? _plidButton : _plidPicture;

	if (pvNil == plid)
		return pvNil;
	return LIG::PligNew(plid, pgcb, ptxhd);
}


/***************************************************************************
	Constructor for the list display gob.
***************************************************************************/
LID::LID(void) : LID_PAR()
{
}


/***************************************************************************
	Desctructor for the list display gob.
***************************************************************************/
LID::~LID(void)
{
	ReleasePpo(&_pcrm);
	ReleasePpo(&_pglcach);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a LID.
***************************************************************************/
void LID::AssertValid(ulong grf)
{
	LID_PAR::AssertValid(0);
	AssertPo(_pcrm, 0);
	AssertPo(_pglcach, 0);
}


/***************************************************************************
	Mark memory for the LIG.
***************************************************************************/
void LID::MarkMem(void)
{
	AssertValid(0);
	LID_PAR::MarkMem();
	MarkMemObj(_pcrm);
	MarkMemObj(_pglcach);
}
#endif //DEBUG


/***************************************************************************
	Static method to create a new list document.
***************************************************************************/
PLID LID::PlidNew(PCRM pcrm, CTG ctg, CHID chid)
{
	AssertPo(pcrm, 0);
	PLID plid;

	if (pvNil == (plid = NewObj LID))
		return pvNil;

	if (!plid->_FInit(pcrm, ctg, chid))
		ReleasePpo(&plid);

	return plid;
}


/***************************************************************************
	Initialization for the list document.
***************************************************************************/
bool LID::_FInit(PCRM pcrm, CTG ctg, CHID chid)
{
	AssertPo(pcrm, 0);
	GCB gcb;

	if (pvNil == (_pglcach = GL::PglNew(size(CACH))))
		return fFalse;
	_pglcach->SetMinGrow(100);

	_pcrm = pcrm;
	_pcrm->AddRef();
	_ctg = ctg;
	_chid = chid;

	return FRefresh();
}


/***************************************************************************
	Rebuild the list and update the DDGs.  The pcrm has changed.
***************************************************************************/
bool LID::FRefresh(void)
{
	AssertThis(0);
	long ipcrf, icki;
	CACH cach, cachT;
	PCRF pcrf;
	PCFL pcfl;
	CKI cki;
	long ivMin, ivLim, iv;
	KID kid;
	long ipddg;
	PDDG pddg;
	bool fRet = fFalse;

	_pglcach->FSetIvMac(0);
	for (ipcrf = 0; ipcrf < _pcrm->Ccrf(); ipcrf++)
		{
		pcrf = _pcrm->PcrfGet(ipcrf);
		pcfl = pcrf->Pcfl();
		for (icki = 0; pcfl->FGetCkiCtg(_ctg, icki, &cki); icki++)
			{
			cach.pcrf = pcrf;
			cach.cno = cach.cnoMbmp = cki.cno;
			if (kctgMbmp != _ctg)
				{
				if (!pcfl->FGetKidChidCtg(_ctg, cki.cno, _chid, kctgMbmp, &kid))
					continue;
				cach.cnoMbmp = kid.cki.cno;
				}

			// determine where this cno goes.
			for (ivMin = 0, ivLim = _pglcach->IvMac(); ivMin < ivLim; )
				{
				iv = (ivMin + ivLim) / 2;
				_pglcach->Get(iv, &cachT);
				if (cachT.cno < cach.cno)
					ivMin = iv + 1;
				else
					ivLim = iv;
				}
			if (ivMin < _pglcach->IvMac())
				{
				_pglcach->Get(ivMin, &cachT);
				if (cachT.cno == cach.cno)
					continue;
				}
			if (!_pglcach->FInsert(ivMin, &cach))
				goto LFail;
			}
		}
	fRet = fTrue;
LFail:

	//invalidate the LIGs
	for (ipddg = 0; pvNil != (pddg = PddgGet(ipddg)); ipddg++)
		{
		Assert(pddg->FIs(kclsLIG), 0);
		((PLIG)pddg)->Refresh();
		}

	return fTrue;
}


/***************************************************************************
	Return the number of items in the list.
***************************************************************************/
long LID::Ccki(void)
{
	AssertThis(0);
	return _pglcach->IvMac();
}


/***************************************************************************
	Get the CKI for the indicated item.
***************************************************************************/
void LID::GetCki(long icki, CKI *pcki, PCRF *ppcrf)
{
	AssertThis(0);
	AssertIn(icki, 0, _pglcach->IvMac());
	AssertVarMem(pcki);
	AssertNilOrVarMem(ppcrf);
	CACH cach;

	_pglcach->Get(icki, &cach);
	pcki->ctg = _ctg;
	pcki->cno = cach.cno;
	if (pvNil != ppcrf)
		*ppcrf = cach.pcrf;
}


/***************************************************************************
	Get an MBMP for the indicated item.
***************************************************************************/
PMBMP LID::PmbmpGet(long icki)
{
	AssertThis(0);
	AssertIn(icki, 0, _pglcach->IvMac());
	CACH cach;

	_pglcach->Get(icki, &cach);
	return (PMBMP)cach.pcrf->PbacoFetch(kctgMbmp, cach.cnoMbmp, MBMP::FReadMbmp);
}


/***************************************************************************
	Constructor for the list display gob.
***************************************************************************/
LIG::LIG(PLID plid, GCB *pgcb) : LIG_PAR(plid, pgcb)
{
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a LIG.
***************************************************************************/
void LIG::AssertValid(ulong grf)
{
	LIG_PAR::AssertValid(0);
	AssertPo(_pscb, 0);
	AssertPo(_ptxhd, 0);
	AssertIn(_dypCell, 1, kswMax);
}


/***************************************************************************
	Mark memory for the LIG.
***************************************************************************/
void LIG::MarkMem(void)
{
	AssertValid(0);
	LIG_PAR::MarkMem();
	MarkMemObj(_ptxhd);
}
#endif //DEBUG


/***************************************************************************
	Static method to create a new list display gob.
***************************************************************************/
PLIG LIG::PligNew(PLID plid, GCB *pgcb, PTXHD ptxhd, long dypCell)
{
	AssertPo(plid, 0);
	AssertVarMem(pgcb);
	AssertPo(ptxhd, 0);
	AssertIn(dypCell, 1, kswMax);
	PLIG plig;

	if (pvNil == (plig = NewObj LIG(plid, pgcb)))
		return pvNil;

	if (!plig->_FInit(ptxhd, dypCell))
		ReleasePpo(&plig);

	return plig;
}


/***************************************************************************
	Return the LID for this LIG.
***************************************************************************/
PLID LIG::Plid(void)
{
	AssertPo(_pdocb, 0);
	Assert(_pdocb->FIs(kclsLID), 0);
	return (PLID)_pdocb;
}


/***************************************************************************
	Initialization for the list display gob.
***************************************************************************/
bool LIG::_FInit(PTXHD ptxhd, long dypCell)
{
	AssertPo(ptxhd, 0);
	AssertIn(dypCell, 1, kswMax);
	GCB gcb;

	if (!LIG_PAR::_FInit())
		return fFalse;

	_ptxhd = ptxhd;
	_dypCell = dypCell;

	gcb.Set(khidVScroll, this);
	gcb._rcRel.Set(krelOne, 0, krelOne, krelOne);
	gcb._rcAbs.Set(-SCB::DxpNormal(), -1, 0, 1 - kdxpFrameCcg);
	if (pvNil == (_pscb = SCB::PscbNew(&gcb, fscbVert,
			0, 0, Plid()->Ccki() - 1)))
		{
		return fFalse;
		}

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	The LID has changed, reset the scroll bounds and invalidate the display
	area.
***************************************************************************/
void LIG::Refresh(void)
{
	AssertThis(0);
	long val;

	InvalRc(pvNil);
	val = LwMax(0, LwMin(_pscb->Val(), Plid()->Ccki() - 1));
	_pscb->SetValMinMax(val, 0, Plid()->Ccki() - 1);
}


/***************************************************************************
	Draw the list.
***************************************************************************/
void LIG::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	RC rc, rcT, rcCell, rcClip;
	long icki;
	PMBMP pmbmp;
	PLID plid;
	long ccki;

	plid = Plid();
	ccki = plid->Ccki();

	GetRc(&rc, cooLocal);
	rcT = rc;
	rc.ypBottom = rcT.ypTop = rcT.ypBottom - kdxpFrameCcg;
	pgnv->FillRc(&rcT, kacrBlack);
	_pscb->GetRc(&rcT, cooParent);
	rc.xpRight = rcT.xpLeft;

	rcCell = rc;
	if (!rc.FIntersect(prcClip))
		return;

	pgnv->FillRc(&rc, kacrWhite);
	rcCell.ypTop = LwRoundToward(rc.ypTop, _dypCell);
	icki = _pscb->Val() + rcCell.ypTop / _dypCell;
	for ( ; rcCell.ypTop < rc.ypBottom && icki < ccki;
			rcCell.ypTop += _dypCell, icki++)
		{
		rcCell.ypBottom = rcCell.ypTop + _dypCell;
		pmbmp = plid->PmbmpGet(icki);
		if (pvNil == pmbmp)
			continue;
		pmbmp->GetRc(&rcT);
		rcT.CenterOnRc(&rcCell);
		if (rcClip.FIntersect(&rc, &rcCell))
			{
			pgnv->ClipRc(&rcClip);
			pgnv->DrawMbmp(pmbmp, &rcT);
			}
		ReleasePpo(&pmbmp);
		}
}


/***************************************************************************
	Handles a scroll command.
***************************************************************************/
bool LIG::FCmdScroll(PCMD pcmd)
{
	long dval, val;
	RC rc, rcT;

	GetRc(&rc, cooLocal);
	rc.ypBottom -= kdxpFrameCcg;
	_pscb->GetRc(&rcT, cooParent);
	rc.xpRight = rcT.xpLeft;
	val = _pscb->Val();
	switch (pcmd->cid)
		{
	case cidDoScroll:
		switch (pcmd->rglw[1])
			{
		default:
			Bug("unknown sca");
			return fTrue;
		case scaLineUp:
			dval = -1;
			break;
		case scaPageUp:
			dval = -LwMax(1, rc.Dyp() / _dypCell - 1);
			break;
		case scaLineDown:
			dval = 1;
			break;
		case scaPageDown:
			dval = LwMax(1, rc.Dyp() / _dypCell - 1);
			break;
		case scaToVal:
			dval = pcmd->rglw[2] - val;
			break;
			}
		break;

	case cidEndScroll:
		dval = pcmd->rglw[1] - val;
		break;
		}

	dval = LwMax(0, LwMin(dval + val, _pscb->ValMax())) - val;
	if (dval == 0)
		return fTrue;

	_pscb->SetVal(val + dval);
	Scroll(&rc, 0, -dval * _dypCell, kginDraw);

	return fTrue;
}


/***************************************************************************
	The mouse was clicked in the LIG.  Insert the object in the active
	DDG.
***************************************************************************/
void LIG::MouseDown(long xp, long yp, long cact, ulong grfcust)
{
	AssertThis(0);
	long icki;
	CKI cki;
	RC rc, rcT;
	PHETG phetg;
	PCRF pcrf;
	PLID plid;

	plid = (PLID)_pdocb;
	Assert(plid->FIs(kclsLID), 0);
	AssertPo(plid, 0);

	GetRc(&rc, cooLocal);
	_pscb->GetRc(&rcT, cooParent);
	rc.xpRight = rcT.xpLeft;
	if (!rc.FPtIn(xp, yp))
		return;

	icki = _pscb->Val() + yp / _dypCell;
	if (!FIn(icki, 0, plid->Ccki()))
		return;

	phetg = (PHETG)_ptxhd->PddgActive();
	if (pvNil == phetg || !phetg->FIs(kclsHETG))
		return;
	plid->GetCki(icki, &cki, &pcrf);

	switch (cki.ctg)
		{
	case kctgMbmp:
		phetg->FInsertPicture(pcrf, cki.ctg, cki.cno);
		break;

	case kctgGokd:
		phetg->FInsertButton(pcrf, cki.ctg, cki.cno);
		break;
		}
}


/***************************************************************************
	Constructor for the CCG.
***************************************************************************/
CCG::CCG(GCB *pgcb, PTXHD ptxhd, bool fForeColor, long cacrRow) : CCG_PAR(pgcb)
{
	AssertPo(ptxhd, 0);
	AssertIn(cacrRow, 1, 257);
	_ptxhd = ptxhd;
	_cacrRow = cacrRow;
	_fForeColor = FPure(fForeColor);
}


/***************************************************************************
	Handle mousedown in a CCG.  Set the foreground or background color of
	the text in the active of DDG of the ptxhd.
***************************************************************************/
void CCG::MouseDown(long xp, long yp, long cact, ulong grfcust)
{
	AssertThis(0);
	PHETG phetg;
	ACR acr;

	if (!_FGetAcrFromPt(xp, yp, &acr))
		return;

	phetg = (PHETG)_ptxhd->PddgActive();
	if (pvNil != phetg && phetg->FIs(kclsHETG))
		phetg->FSetColor(_fForeColor ? &acr : pvNil, _fForeColor ? pvNil : &acr);
}


/***************************************************************************
	Draw the Color chooser gob.
***************************************************************************/
void CCG::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	long crcHeight, ircHeight, ircWidth;
	long iscr;
	RC rc, rcT;
	ACR acr;

	GetRc(&rc, cooLocal);
	rc.ypTop -= kdxpFrameCcg;
	rc.xpLeft -= kdxpFrameCcg;
	pgnv->SetPenSize(kdxpFrameCcg, kdxpFrameCcg);
	pgnv->FrameRc(&rc, kacrBlack);
	rc.Inset(kdxpFrameCcg, kdxpFrameCcg);
	pgnv->FillRc(&rc, kacrWhite);
	rc.Inset(kdxpFrameCcg, kdxpFrameCcg);

	crcHeight = LwDivAway(257, _cacrRow);
	for (iscr = 0, ircHeight = 0; ircHeight < crcHeight; ircHeight++)
		{
		for (ircWidth = 0; ircWidth < _cacrRow; ircWidth++, iscr++)
			{
			rcT.SetToCell(&rc, _cacrRow, crcHeight, ircWidth, ircHeight);
			switch (iscr)
				{
			default:
				acr.SetToIndex((byte)iscr);
				pgnv->FillRc(&rcT, acr);
				break;

			case 0:
				pgnv->FillRc(&rcT, kacrBlack);
				break;

			case 256:
				if (!_fForeColor)
					pgnv->FillRcApt(&rcT, &vaptGray, kacrBlack, kacrWhite);
				return;
				}
			}
		}
}


/***************************************************************************
	Map the given point to a color.
***************************************************************************/
bool CCG::_FGetAcrFromPt(long xp, long yp, ACR *pacr, RC *prc, long *piscr)
{
	AssertThis(0);
	AssertVarMem(pacr);
	AssertNilOrVarMem(prc);
	AssertNilOrVarMem(piscr);
	RC rc;
	long iscr;
	long ircWidth, ircHeight;

	GetRc(&rc, cooLocal);
	rc.ypTop += kdxpFrameCcg;
	rc.xpLeft += kdxpFrameCcg;
	rc.ypBottom -= 2 * kdxpFrameCcg;
	rc.xpRight -= 2 * kdxpFrameCcg;
	if (!rc.FMapToCell(xp, yp, _cacrRow, LwDivAway(257, _cacrRow),
			&ircWidth, &ircHeight))
		{
		TrashVar(prc);
		TrashVar(piscr);
		return fFalse;
		}

	iscr = LwMul(ircHeight, _cacrRow) + ircWidth;
	switch (iscr)
		{
	default:
		if (!FIn(iscr, 1, 256))
			return fFalse;
		pacr->SetToIndex((byte)iscr);
		break;

	case 0:
		*pacr = kacrBlack;
		break;

	case 256:
        if (_fForeColor)
			return fFalse;
		pacr->SetToClear();
		break;
		}

	if (pvNil != prc)
		{
		prc->SetToCell(&rc, _cacrRow, LwDivAway(257, _cacrRow),
			ircWidth, ircHeight);
		}
	if (pvNil != piscr)
		*piscr = iscr;

	return fTrue;
}


/***************************************************************************
	Put up the CCG's tool tip.
***************************************************************************/
bool CCG::FEnsureToolTip(PGOB *ppgobCurTip, long xpMouse, long ypMouse)
{
	AssertThis(0);
	AssertVarMem(ppgobCurTip);
	AssertNilOrPo(*ppgobCurTip, 0);
	RC rc;
	ACR acr;

	ReleasePpo(ppgobCurTip);

	GCB gcb(khidToolTip, this, fgobNil, kginMark);
	*ppgobCurTip = NewObj CCGT(&gcb, kacrBlack);

	return fTrue;
}


/***************************************************************************
	When the mouse moves over the CCG, update the tool tip.
***************************************************************************/
bool CCG::FCmdMouseMove(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	PCCGT pccgt;
	RC rc, rcOld;
	ACR acr;
	STN stn;
	long iscr;

	if (pvNil == (pccgt = (PCCGT)PgobFromHid(khidToolTip)) ||
			!pccgt->FIs(kclsCCGT))
		{
		return fTrue;
		}

	if (!_FGetAcrFromPt(pcmd->xp, pcmd->yp, &acr, &rc, &iscr))
		{
		rc.Zero();
		acr = kacrBlack;
		}
	else
		{
		rc.ypBottom = rc.ypTop;
		rc.ypTop -= 30;
		rc.Inset(-20, 0);
		if (acr == kacrClear)
			stn = PszLit(" Clear ");
		else
			stn.FFormatSz(PszLit(" %d "),	iscr);
		}

	if (rc.ypTop < 0 && pcmd->yp < rc.Dyp())
		rc.Offset(0, 60);
	GetRc(&rcOld, cooLocal);
	rcOld.ypBottom -= kdxpFrameCcg;
	rcOld.xpRight -= kdxpFrameCcg;
	rc.PinToRc(&rcOld);
	pccgt->GetPos(&rcOld, pvNil);
	if (rcOld != rc)
		pccgt->SetPos(&rc);
	pccgt->SetAcr(acr, &stn);
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a CCG.
***************************************************************************/
void CCG::AssertValid(ulong grf)
{
	CCG_PAR::AssertValid(0);
	AssertPo(_ptxhd, 0);
	AssertIn(_cacrRow, 1, 257);
}
#endif //DEBUG


/***************************************************************************
	Constructor for color chooser tool tip.
***************************************************************************/
CCGT::CCGT(PGCB pgcb, ACR acr, PSTN pstn) : CCGT_PAR(pgcb)
{
	AssertBaseThis(0);
	_acr = acr;
	if (pvNil != pstn)
		_stn = *pstn;
}


/***************************************************************************
	Set the color for the tool tip.
***************************************************************************/
void CCGT::SetAcr(ACR acr, PSTN pstn)
{
	AssertThis(0);
	AssertPo(&acr, 0);
	AssertNilOrPo(pstn, 0);

	if (_acr != acr || pvNil == pstn && _stn.Cch() > 0 || !_stn.FEqual(pstn))
		{
		_acr = acr;
		if (pvNil == pstn)
			_stn.SetNil();
		else
			_stn = *pstn;
		InvalRc(pvNil, kginMark);
		}
}


/***************************************************************************
	Draw the color tool tip.
***************************************************************************/
void CCGT::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	RC rc;
	ACR acr;

	GetRc(&rc, cooLocal);
	pgnv->SetPenSize(1, 1);
	pgnv->FrameRc(&rc, kacrBlack);
	rc.Inset(1, 1);
	pgnv->FrameRc(&rc, kacrWhite);
	rc.Inset(1, 1);
	if (_acr == kacrClear)
		pgnv->FillRcApt(&rc, &vaptGray, kacrBlack, kacrWhite);
	else
		pgnv->FillRc(&rc, _acr);

	if (_stn.Cch() > 0)
		{
		pgnv->ClipRc(&rc);
		pgnv->SetFont(vpappb->OnnDefVariable(), fontBold, 12, tahCenter, tavCenter);
		if (_acr == kacrClear)
			{
			pgnv->DrawStn(&_stn, rc.XpCenter(), rc.YpCenter(),
				kacrBlack, kacrWhite);
			}
		else
			pgnv->DrawStn(&_stn, rc.XpCenter(), rc.YpCenter(), _acr, kacrInvert);
		}
}

