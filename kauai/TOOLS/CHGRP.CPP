/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	Handles editing a chunk consisting of a group
	(GL, AL, GG, AG, GST, AST)

***************************************************************************/
#include "ched.h"
ASSERTNAME


#define kcbMaxDispGrp 16

bool _FDlgGrpbNew(PDLG pdlg, long *pidit, void *pv);


BEGIN_CMD_MAP(DCGB, DDG)
	ON_CID_GEN(cidEditNatural, &DCGB::FCmdEditItem, &DCGB::FEnableDcgbCmd)
	ON_CID_GEN(cidEditHex, &DCGB::FCmdEditItem, &DCGB::FEnableDcgbCmd)
	ON_CID_GEN(cidInsertItem, &DCGB::FCmdAddItem, &DCGB::FEnableDcgbCmd)
	ON_CID_GEN(cidAddItem, &DCGB::FCmdAddItem, pvNil)
	ON_CID_GEN(cidDeleteItem, &DCGB::FCmdDeleteItem, &DCGB::FEnableDcgbCmd)
END_CMD_MAP_NIL()


RTCLASS(DOCG)
RTCLASS(DOCI)
RTCLASS(DCGB)
RTCLASS(DCGL)
RTCLASS(DCGG)
RTCLASS(DCST)

/***************************************************************************
	Constructor for a group document.  cls indicates which group class
	the edited chunk belongs to.  cls should be one of GL, AL, GG, AG,
	GST, AST.
***************************************************************************/
DOCG::DOCG(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno, long cls)
	: DOCE(pdocb, pcfl, ctg, cno)
{
	_pgrpb = pvNil;
	_cls = cls;
}


/***************************************************************************
	Destructor for DOCG.  Free the GRPB.
***************************************************************************/
DOCG::~DOCG(void)
{
	ReleasePpo(&_pgrpb);
}


/***************************************************************************
	Static method to create a new document based on a group in a chunk.
	Asserts that there are no other editing docs open on this chunk.
***************************************************************************/
PDOCG DOCG::PdocgNew(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno, long cls)
{
	AssertPo(pdocb, 0);
	AssertPo(pcfl, 0);

	Assert(pvNil == DOCE::PdoceFromChunk(pdocb, pcfl, ctg, cno),
		"DOCE already exists for the chunk");
	PDOCG pdocg;

	if (pvNil == (pdocg = NewObj DOCG(pdocb, pcfl, ctg, cno, cls)))
		return pvNil;
	if (!pdocg->_FInit())
		{
		ReleasePpo(&pdocg);
		return pvNil;
		}
	AssertPo(pdocg, 0);
	return pdocg;
}


enum
	{
	kiditOkGrpbNew,
	kiditCancelGrpbNew,
	kiditSizeGrpbNew,
	kiditLimGrpbNew
	};


/***************************************************************************
	Dialog proc for Adopt Chunk dialog.
***************************************************************************/
bool _FDlgGrpbNew(PDLG pdlg, long *pidit, void *pv)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pidit);
	long cb;
	bool fEmpty;

	switch (*pidit)
		{
	case kiditCancelGrpbNew:
		return fTrue; //dismiss the dialog

	case kiditOkGrpbNew:
		if (!pdlg->FGetValues(0, kiditLimGrpbNew))
			{
			*pidit = ivNil;
			return fTrue;
			}

		//check the size
		if (!pdlg->FGetLwFromEdit(kiditSizeGrpbNew, &cb, &fEmpty) && !fEmpty ||
				!FIn(cb, *(long *)pv, kcbMax))
			{
			vpappb->TGiveAlertSz(PszLit("Bad Size"), bkOk, cokStop);
			pdlg->SelectDit(kiditSizeGrpbNew);
			return fFalse;
			}
		return fTrue;

	default:
		break;
		}

	return fFalse;
}

/***************************************************************************
	Read the group from the given flo.
***************************************************************************/
bool DOCG::_FRead(PBLCK pblck)
{
	AssertPo(pblck, 0);
	Assert(pvNil == _pgrpb, "group not nil");

	if (0 == pblck->Cb(fTrue))
		{
		//create a new one - need to ask for a size from the user
		PDLG pdlg;
		long dlid, idit;
		long cb, cbMin;
		bool fEmpty;

		//determine which dialog to use
		cbMin = 0;
		switch (_cls)
			{
		case kclsGL:
		case kclsAL:
			dlid = dlidGlbNew;
			cbMin = 1;
			break;
		case kclsGG:
		case kclsAG:
			dlid = dlidGgbNew;
			break;
		case kclsGST:
		case kclsAST:
			dlid = dlidGstbNew;
			break;
		default:
			BugVar("bad cls value", &_cls);
			return fFalse;
			}

		pdlg = DLG::PdlgNew(dlid, _FDlgGrpbNew, &cbMin);
		pdlg->FPutLwInEdit(kiditSizeGrpbNew, cbMin);
		if (pvNil == pdlg)
			goto LFail;
		idit = pdlg->IditDo(kiditSizeGrpbNew);
		if (idit != kiditOkGrpbNew)
			goto LFail;

		if (!pdlg->FGetLwFromEdit(kiditSizeGrpbNew, &cb, &fEmpty) && !fEmpty ||
				!FIn(cb, cbMin, kcbMax / size(long)))
			{
			goto LFail;
			}

		switch (_cls)
			{
		case kclsGL:
			_pgrpb = GL::PglNew(cb);
			break;
		case kclsAL:
			_pgrpb = AL::PalNew(cb);
			break;
		case kclsGG:
			_pgrpb = GG::PggNew(cb);
			break;
		case kclsAG:
			_pgrpb = AG::PagNew(cb);
			break;
		case kclsGST:
			_pgrpb = GST::PgstNew(cb * size(long));
			break;
		case kclsAST:
			_pgrpb = AST::PastNew(cb * size(long));
			break;
		default:
			BugVar("bad cls value", &_cls);
			return fFalse;
			}
LFail:
		ReleasePpo(&pdlg);
		_bo = kboCur;
		_osk = koskCur;
		_fDirty = fTrue;
		}
	else
		{
		switch (_cls)
			{
		case kclsGL:
			_pgrpb = GL::PglRead(pblck, &_bo, &_osk);
			break;
		case kclsAL:
			_pgrpb = AL::PalRead(pblck, &_bo, &_osk);
			break;
		case kclsGG:
			_pgrpb = GG::PggRead(pblck, &_bo, &_osk);
			break;
		case kclsAG:
			_pgrpb = AG::PagRead(pblck, &_bo, &_osk);
			break;
		case kclsGST:
			_pgrpb = GST::PgstRead(pblck, &_bo, &_osk);
			break;
		case kclsAST:
			_pgrpb = AST::PastRead(pblck, &_bo, &_osk);
			break;
		default:
			BugVar("bad cls value", &_cls);
			return fFalse;
			}
		}

	return pvNil != _pgrpb;
}


/***************************************************************************
	Create a new DDG onto the document.
***************************************************************************/
PDDG DOCG::PddgNew(PGCB pgcb)
{
	AssertThis(0);
	PDDG pddg;

	pddg = pvNil;
	switch (_cls)
		{
	case kclsGL:
	case kclsAL:
		pddg = DCGL::PdcglNew(this, (PGLB)_pgrpb, _cls, pgcb);
		break;
	case kclsGG:
	case kclsAG:
		pddg = DCGG::PdcggNew(this, (PGGB)_pgrpb, _cls, pgcb);
		break;
	case kclsGST:
	case kclsAST:
		pddg = DCST::PdcstNew(this, (PGSTB)_pgrpb, _cls, pgcb);
		break;
	default:
		BugVar("bad cls value", &_cls);
		break;
		}
	return pddg;
}


/***************************************************************************
	Write the group to disk.
***************************************************************************/
bool DOCG::_FWrite(PBLCK pblck, bool fRedirect)
{
	AssertThis(0);
	AssertPo(pblck, 0);
	return _pgrpb->FWrite(pblck, _bo, _osk);
}


/***************************************************************************
	Get the size of the group on file.
***************************************************************************/
long DOCG::_CbOnFile(void)
{
	AssertThis(0);
	return _pgrpb->CbOnFile();
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of an object.
***************************************************************************/
void DOCG::AssertValid(ulong grf)
{
	DOCG_PAR::AssertValid(0);
	AssertPo(_pgrpb, 0);
	Assert(_pgrpb->FIs(_cls), "group doesn't match cls");
	switch (_cls)
		{
	case kclsGL:
	case kclsAL:
	case kclsGG:
	case kclsAG:
	case kclsGST:
	case kclsAST:
		break;
	default:
		BugVar("bad cls value", &_cls);
		break;
		}
}


/***************************************************************************
	Mark memory for the DOCG.
***************************************************************************/
void DOCG::MarkMem(void)
{
	AssertValid(0);
	DOCG_PAR::MarkMem();
	MarkMemObj(_pgrpb);
}
#endif //DEBUG


/***************************************************************************
	Constructor for a DCGB.
***************************************************************************/
DCGB::DCGB(PDOCB pdocb, PGRPB pgrpb, long cls, long clnItem, PGCB pgcb)
	: DCLB(pdocb, pgcb)
{
	AssertIn(clnItem, 1, 10);
	_dypBorder = 1;
	_dypLine = _dypLine + _dypBorder;
	_clnItem = clnItem;
	_ivCur = ivNil;
	_dlnCur = 0;
	_pgrpb = pgrpb;
	_cls = cls;
	switch (_cls)
		{
	default:
#ifdef DEBUG
		BugVar("bad cls value", &_cls);
	case kclsGL:
	case kclsGG:
	case kclsGST:
#endif //DEBUG
		_fAllocated = fFalse;
		break;
	case kclsAL:
	case kclsAG:
	case kclsAST:
		_fAllocated = fTrue;
		break;
		}
}


/***************************************************************************
	Static method to invalidate all DCGB's on this GRPB.  Also dirties the
	document.  Should be called by any code that edits the document.
***************************************************************************/
void DCGB::InvalAllDcgb(PDOCB pdocb, PGRPB pgrpb, long iv, long cvIns, long cvDel)
{
	long ipddg;
	PDDG pddg;
	PDCGB pdcgb;

	//mark the document dirty
	pdocb->SetDirty();

	//inform the DCGB's
	for (ipddg = 0; pvNil != (pddg = pdocb->PddgGet(ipddg)); ipddg++)
		{
		if (!pddg->FIs(kclsDCGB))
			continue;
		pdcgb = (PDCGB)pddg;
		if (pdcgb->_pgrpb == pgrpb)
            pdcgb->_InvalIv(iv, cvIns, cvDel);
		}
}


/***************************************************************************
	Invalidate the display from iv to the end of the display.  If we're
	the active DCGB, also redraw.
***************************************************************************/
void DCGB::_InvalIv(long iv, long cvIns, long cvDel)
{
	AssertThis(0);
	AssertIn(iv, 0, kcbMax);
	AssertIn(cvIns, 0, kcbMax);
	AssertIn(cvDel, 0, kcbMax);
	RC rc;

	//adjust the sel
	if (_ivCur > iv)
		{
		if (_ivCur < iv + cvDel)
			{
			_ivCur = ivNil;
			_dlnCur = 0;
			}
		else
			_ivCur += cvIns - cvDel;
		}

	//caclculate the invalid rectangle
	GetRc(&rc, cooLocal);
	rc.ypTop = _YpFromIvDln(iv, 0);
	if (cvIns == cvDel)
		rc.ypBottom = _YpFromIvDln(iv + cvIns, 0);

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
	We're being activated or deactivated so invert the sel.
***************************************************************************/
void DCGB::_Activate(bool fActive)
{
	AssertThis(0);
	DDG::_Activate(fActive);

	GNV gnv(this);
	_DrawSel(&gnv);
}


/***************************************************************************
	Return the max for the scroll bar.
***************************************************************************/
long DCGB::_ScvMax(bool fVert)
{
	AssertThis(0);
	RC rc;
	long ichLim;

	_GetContent(&rc);
	if (fVert)
		{
		return LwMax(0,
			LwMul(_clnItem, _pgrpb->IvMac()) - rc.Dyp() / _dypLine + 1);
		}

	ichLim = 3 * kcbMaxDispGrp + kcbMaxDispGrp / 4 + 12;
	return LwMax(0, ichLim - rc.Dxp() / _dxpChar);
}


/***************************************************************************
	Set the selection to the given line.
***************************************************************************/
void DCGB::_SetSel(long ln)
{
	AssertThis(0);
	long iv, dln;

	if (ln < 0 || ln >= _LnLim())
		{
		Assert(ln == lnNil, "bad non-nil selection requested");
		iv = ivNil;
		dln = 0;
		}
	else
		{
		iv = ln / _clnItem;
		dln = ln % _clnItem;
		}

	if (_ivCur == iv && _dlnCur == dln)
		return;

	if (_fActive)
		{
		GNV gnv(this);
	
		//erase the old sel
		if (_fActive)
			_DrawSel(&gnv);
	
		//set the new sel and draw it
		_ivCur = iv;
		_dlnCur = dln;
		if (_fActive)
			_DrawSel(&gnv);
		}
	else
		{
		//just set the new selection
		_ivCur = iv;
		_dlnCur = dln;
		}
}


/***************************************************************************
	Scroll the sel into view.
***************************************************************************/
void DCGB::_ShowSel(void)
{
	AssertThis(0);
	RC rc;
	long cln, ln;

	if (ivNil == _ivCur)
		return;

	ln = _LnFromIvDln(_ivCur, _dlnCur) - _scvVert;
	if (ln < 0)
		_Scroll(scaNil, scaToVal, 0, _scvVert + ln);
	else
		{
		_GetContent(&rc);
		cln = LwMax(1, rc.Dyp() / _dypLine);
		if (ln >= cln)
			_Scroll(scaNil, scaToVal, 0, _scvVert + ln - cln + 1);
		}
}


/***************************************************************************
	Hilite the selection (if there is one)
***************************************************************************/
void DCGB::_DrawSel(PGNV pgnv)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	RC rc;

	if (ivNil != _ivCur)
		{
		pgnv->GetRcSrc(&rc);
		rc.ypTop = _YpFromIvDln(_ivCur, _dlnCur);
		rc.ypBottom = rc.ypTop + _dypLine - _dypBorder;
		pgnv->HiliteRc(&rc, kacrWhite);
		}
}

/***************************************************************************
	Handle key input.
***************************************************************************/
bool DCGB::FCmdKey(PCMD_KEY pcmd)
{
	AssertThis(0);
	long ln, lnLim, lnNew, cln;
	RC rc;

	ln = _LnFromIvDln(_ivCur, _dlnCur);
	lnLim = _LnLim();
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
		if (ivNil != _ivCur && !_pgrpb->FFree(_ivCur))
			_DeleteIv(_ivCur);
		break;

	default:
		switch (pcmd->ch)
			{
		case kchReturn:
			if (ivNil != _ivCur && !_pgrpb->FFree(_ivCur))
				_EditIvDln(_ivCur, _dlnCur);
			break;
			}
		break;
		}

	return fTrue;
}


/***************************************************************************
	Handle mouse-down in a DCGB - track the mouse and select the last
	item the mouse is over.
***************************************************************************/
void DCGB::MouseDown(long xp, long yp, long cact, ulong grfcust)
{
	AssertThis(0);
	bool fDown;
	PT pt, ptT;
	long ln, ivCur, dlnCur;
	RC rc;

	//do this before the activate to avoid flashing the selection
	ivCur = _ivCur;
	dlnCur = _dlnCur;
	ln = _LnFromYp(yp);
	if (ln >= _LnLim())
		ln = lnNil;
	_SetSel(ln);

	if (!_fActive)
		Activate(fTrue);

	if (cact > 1 && ivCur == _ivCur && dlnCur == _dlnCur &&
			_ivCur != ivNil && !_pgrpb->FFree(_ivCur))
		{
		//edit the thing
		_EditIvDln(_ivCur, _dlnCur);
		return;
		}

	Clean();
	_GetContent(&rc);
	for (GetPtMouse(&pt, &fDown); fDown; GetPtMouse(&pt, &fDown))
		{
		if (!rc.FPtIn(pt.xp, pt.yp))
			{
			//do autoscroll
			ptT = pt;
			rc.PinPt(&pt);
			_Scroll(scaToVal, scaToVal,
				_scvHorz + LwDivAway(ptT.xp - pt.xp, _dxpChar),
				_scvVert + LwDivAway(ptT.yp - pt.yp, _dypLine));
			}
		if (!rc.FPtIn(pt.xp, pt.yp) || (ln = _LnFromYp(pt.yp)) >= _LnLim())
			ln = lnNil;
		_SetSel(ln);
		}
}


/***************************************************************************
	Handle enabling/disabling commands.
***************************************************************************/
bool DCGB::FEnableDcgbCmd(PCMD pcmd, ulong *pgrfeds)
{
	bool fT;

	switch (pcmd->cid)
		{
	case cidEditNatural:
	case cidEditHex:
	case cidDeleteItem:
		fT = (ivNil != _ivCur) && !_pgrpb->FFree(_ivCur);
		break;

	case cidInsertItem:
		fT =  (ivNil != _ivCur) && !_fAllocated;
		break;

	default:
		BugVar("unhandled cid", &pcmd->cid);
		fT = fFalse;
		break;
		}

	*pgrfeds = fT ? fedsEnable : fedsDisable;
	return fTrue;
}


/***************************************************************************
	Handles commands to edit the current line of the group.
***************************************************************************/
bool DCGB::FCmdEditItem(PCMD pcmd)
{
	if (ivNil == _ivCur)
		return fFalse;

	_EditIvDln(_ivCur, _dlnCur);
	return fTrue;
}


/***************************************************************************
	Edit the indicated line of the group.
***************************************************************************/
void DCGB::_EditIvDln(long iv, long dln)
{
	PDOCI pdoci;

	if (pvNil == (pdoci = DOCI::PdociNew(_pdocb, _pgrpb, _cls, iv, dln)))
		return;

	pdoci->PdmdNew();
	ReleasePpo(&pdoci);
}


/***************************************************************************
	Handle cidDeleteItem.
***************************************************************************/
bool DCGB::FCmdDeleteItem(PCMD pcmd)
{
	_DeleteIv(_ivCur);
	return fTrue;
}


/***************************************************************************
	Delete the indicated item.
***************************************************************************/
void DCGB::_DeleteIv(long iv)
{
	long ivMac;

	if (ivNil == iv || _pgrpb->FFree(iv))
		return;

	//turn off the selection, delete the element and invalidate stuff
	_SetSel(lnNil);
	_pgrpb->Delete(iv);
	ivMac = _pgrpb->IvMac();
	if (iv < ivMac)
		{
		InvalAllDcgb(_pdocb, _pgrpb, iv, _fAllocated ? 1 : 0, 1);
		_SetSel(_LnFromIvDln(iv, 0));
		}
	else
		{
		InvalAllDcgb(_pdocb, _pgrpb, ivMac, 0, iv - ivMac + 1);
		if (ivMac > 0)
			_SetSel(_LnFromIvDln(ivMac - 1, 0));
		}
	_ShowSel();
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a DCGB.
***************************************************************************/
void DCGB::AssertValid(ulong grf)
{
	//REVIEW shonk: fill out
	DCGB_PAR::AssertValid(0);
	AssertPo(_pgrpb, 0);
}


/***************************************************************************
	Mark memory for the DCGB.
***************************************************************************/
void DCGB::MarkMem(void)
{
	AssertValid(0);
	DCGB_PAR::MarkMem();
	MarkMemObj(_pgrpb);
}
#endif //DEBUG


/***************************************************************************
	Constructor for the DCGL class.  This class displays (and allows
	editing of) a GL or AL.
***************************************************************************/
DCGL::DCGL(PDOCB pdocb, PGLB pglb, long cls, PGCB pgcb)
	: DCGB(pdocb, pglb, cls, 1, pgcb)
{
}


/***************************************************************************
	Static method to create a new DCGL for the GL or AL.
***************************************************************************/
PDCGL DCGL::PdcglNew(PDOCB pdocb, PGLB pglb, long cls, PGCB pgcb)
{
	AssertVar(cls == kclsGL || cls == kclsAL, "bad cls", &cls);
	PDCGL pdcgl;

	if (pvNil == (pdcgl = NewObj DCGL(pdocb, pglb, cls, pgcb)))
		return pvNil;

	if (!pdcgl->_FInit())
		{
		ReleasePpo(&pdcgl);
		return pvNil;
		}
	pdcgl->Activate(fTrue);

	AssertPo(pdcgl, 0);
	return pdcgl;
}


/***************************************************************************
	Draw the contents of the DCGL.
***************************************************************************/
void DCGL::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	PGLB pglb;
	STN stn;
	byte rgb[kcbMaxDispGrp];
	long ivMac, iv;
	long cbDisp, cbEntry;
	long xp, yp, ibT;
	byte bT;

	pgnv->ClipRc(prcClip);
	pgnv->FillRc(prcClip, kacrWhite);
	pgnv->SetOnn(_onn);

	pglb = (PGLB)_pgrpb;
	ivMac = pglb->IvMac();
	xp = _XpFromIch(0);
	cbDisp = cbEntry = pglb->CbEntry();
	if (cbDisp > kcbMaxDispGrp)
		{
		//the rest will be filled with periods to indicate that
		//not everything is displayed
		cbDisp = kcbMaxDispGrp - 2;
		}

	iv = _LnFromYp(LwMax(_dypHeader, prcClip->ypTop));
	yp = _YpFromIvDln(iv, 0);

	for ( ; iv < ivMac && yp < prcClip->ypBottom; iv++)
		{
		if (pglb->FFree(iv))
			{
			stn.FFormatSz(PszLit("%5d)  FREE"), iv);
			goto LDisplay;
			}

		CopyPb(pglb->QvGet(iv), rgb, cbDisp);

		//first comes the item number
		stn.FFormatSz(PszLit("%5d) "), iv);

		//now add the bytes in hex, with a space after every four bytes
		for (ibT = 0; ibT < cbDisp; ibT++)
			{
			bT = rgb[ibT];
			if ((ibT & 0x03) == 0)
				stn.FAppendCh(kchSpace);
			stn.FAppendCh(vrgchHex[(bT >> 4) & 0x0F]);
			stn.FAppendCh(vrgchHex[bT & 0x0F]);
			}
		//add the periods
		if (cbDisp < cbEntry)
			stn.FAppendSz(PszLit("...."));
		stn.FAppendSz(PszLit("  "));

		//now comes the ascii characters.
		for (ibT = 0; ibT < cbDisp; ibT++)
			{
			bT = rgb[ibT];
			if (bT < 32 || bT == 0x7F)
				bT = '?';
			stn.FAppendCh((achar)bT);
			}
		//add the periods
		if (cbDisp < cbEntry)
			stn.FAppendSz(PszLit(".."));

LDisplay:
		pgnv->DrawStn(&stn, xp, yp);
		yp += _dypLine;
		}

	//draw the selection
	if (_fActive)
		_DrawSel(pgnv);
}


/***************************************************************************
	Handle cidAddItem and cidInsertItem.
***************************************************************************/
bool DCGL::FCmdAddItem(PCMD pcmd)
{
	HQ hq;
	long cb;
	void *pv;
	long ivNew;
	bool fT;
	PGLB pglb;

	pglb = (PGLB)_pgrpb;
	cb = pglb->CbEntry();
	if (!FAllocHq(&hq, cb, fmemClear, mprNormal))
		return fTrue;

	pv = PvLockHq(hq);
	fT = fFalse;
	switch (pcmd->cid)
		{
	case cidAddItem:
		fT = pglb->FAdd(pv, &ivNew);
		break;
	case cidInsertItem:
		if (_fAllocated)
			break;
		Assert(pglb->FIs(kclsGL), "bad grpb");
		fT = ((PGL)pglb)->FInsert(ivNew = _ivCur, pv);
		}
	UnlockHq(hq);
	FreePhq(&hq);
	if (!fT)
		return fTrue;

	//turn off the selection and invalidate stuff
	_SetSel(lnNil);
	InvalAllDcgb(_pdocb, pglb, ivNew, 1,
		(_fAllocated && ivNew == pglb->IvMac() - 1) ? 1 : 0);
	_SetSel(_LnFromIvDln(ivNew, 0));
	_ShowSel();

	return fTrue;
}


/***************************************************************************
	Constructor for the DCGG class.  This class displays (and allows
	editing of) a GG or AG.
***************************************************************************/
DCGG::DCGG(PDOCB pdocb, PGGB pggb, long cls, PGCB pgcb)
	: DCGB(pdocb, pggb, cls, pggb->CbFixed() > 0 ? 2 : 1, pgcb)
{
}


/***************************************************************************
	Static method to create a new DCGG for the GG or AG.
***************************************************************************/
PDCGG DCGG::PdcggNew(PDOCB pdocb, PGGB pggb, long cls, PGCB pgcb)
{
	AssertVar(cls == kclsGG || cls == kclsAG, "bad cls", &cls);
	PDCGG pdcgg;

	if (pvNil == (pdcgg = NewObj DCGG(pdocb, pggb, cls, pgcb)))
		return pvNil;

	if (!pdcgg->_FInit())
		{
		ReleasePpo(&pdcgg);
		return pvNil;
		}
	pdcgg->Activate(fTrue);

	AssertPo(pdcgg, 0);
	return pdcgg;
}


/***************************************************************************
	Draw the contents of the DCGL.
***************************************************************************/
void DCGG::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	PGGB pggb;
	STN stn;
	byte rgb[kcbMaxDispGrp];
	long ivMac, iv, dln;
	long cbDisp, cbEntry;
	long xp, yp, ibT;
	byte bT;
	RC rc;

	pgnv->ClipRc(prcClip);
	pgnv->GetRcSrc(&rc);
	pgnv->FillRc(prcClip, kacrWhite);
	pgnv->SetOnn(_onn);

	pggb = (PGGB)_pgrpb;
	ivMac = pggb->IvMac();
	xp = _XpFromIch(0);

	iv = _LnFromYp(LwMax(_dypHeader, prcClip->ypTop)) / _clnItem;
	yp = _YpFromIvDln(iv, 0);

	for ( ; iv < ivMac && yp < prcClip->ypBottom; iv++)
		{
		if (pggb->FFree(iv))
			{
			stn.FFormatSz(PszLit("%5d)  FREE"), iv);
			pgnv->DrawStn(&stn, xp, yp);
			yp += _clnItem * _dypLine;
			goto LDrawBorder;
			}

		for (dln = 0; dln < _clnItem; dln++)
			{
			if (dln == 0)
				cbEntry = pggb->Cb(iv);
			else
				cbEntry = pggb->CbFixed();
			if ((cbDisp = cbEntry) > kcbMaxDispGrp)
				{
				//the rest will be filled with periods to indicate that
				//not everything is displayed
				cbDisp = kcbMaxDispGrp - 2;
				}

			if (dln == 0)
				{
				CopyPb(pggb->QvGet(iv), rgb, cbDisp);
				stn.FFormatSz(PszLit("%5d) "), iv);
				}
			else
				{
				CopyPb(pggb->QvFixedGet(iv), rgb, cbDisp);
				stn = PszLit("       ");
				}

			//now add the bytes in hex, with a space after every four bytes
			for (ibT = 0; ibT < cbDisp; ibT++)
				{
				bT = rgb[ibT];
				if ((ibT & 0x03) == 0)
					stn.FAppendCh(kchSpace);
				stn.FAppendCh(vrgchHex[(bT >> 4) & 0x0F]);
				stn.FAppendCh(vrgchHex[bT & 0x0F]);
				}
			//add the periods
			if (cbDisp < cbEntry)
				stn.FAppendSz(PszLit("...."));
			else if (cbDisp < kcbMaxDispGrp)
				{
				for ( ; ibT < kcbMaxDispGrp; ibT++)
					{
					stn.FAppendRgch(PszLit("   "), (ibT & 0x03) == 0 ? 3 : 2);
					}
				}
			stn.FAppendSz(PszLit("  "));

			//now comes the ascii characters.
			for (ibT = 0; ibT < cbDisp; ibT++)
				{
				bT = rgb[ibT];
				if (bT < 32 || bT == 0x7F)
					bT = '?';
				stn.FAppendCh((achar)bT);
				}
			//add the periods
			if (cbDisp < cbEntry)
				stn.FAppendSz(PszLit(".."));

			pgnv->DrawStn(&stn, xp, yp);
			yp += _dypLine;
			}
LDrawBorder:
		rc.ypTop = yp - _dypBorder;
		rc.ypBottom = yp;
		pgnv->FillRc(&rc, kacrBlack);
		}

	//draw the selection
	if (_fActive)
		_DrawSel(pgnv);
}


/***************************************************************************
	Handle cidAddItem and cidInsertItem.
***************************************************************************/
bool DCGG::FCmdAddItem(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	long cb;
	long ivNew;
	bool fT;
	PGGB pggb;

	pggb = (PGGB)_pgrpb;
	cb = (pggb->CbFixed() == 0);

	fT = fFalse;
	switch (pcmd->cid)
		{
	case cidAddItem:
		fT = pggb->FAdd(cb, &ivNew);
		break;
	case cidInsertItem:
		if (_fAllocated)
			break;
		Assert(pggb->FIs(kclsGG), "bad grpb");
		fT = ((PGG)pggb)->FInsert(ivNew = _ivCur, cb);
		}
	if (!fT)
		return fTrue;

	if (cb > 0)
		ClearPb(pggb->QvGet(ivNew), cb);
	else
		ClearPb(pggb->QvFixedGet(ivNew), pggb->CbFixed());

	//turn off the selection and invalidate stuff
	_SetSel(lnNil);
	InvalAllDcgb(_pdocb, pggb, ivNew, 1,
		(_fAllocated && ivNew == pggb->IvMac() - 1) ? 1 : 0);
	_SetSel(_LnFromIvDln(ivNew, 0));
	_ShowSel();

	return fTrue;
}


/***************************************************************************
	Constructor for the DCST class.  This class displays (and allows
	editing of) a GST or AST.
***************************************************************************/
DCST::DCST(PDOCB pdocb, PGSTB pgstb, long cls, PGCB pgcb)
	: DCGB(pdocb, pgstb, cls, pgstb->CbExtra() > 0 ? 2 : 1, pgcb)
{
}


/***************************************************************************
	Static method to create a new DCST for the GST or AST.
***************************************************************************/
PDCST DCST::PdcstNew(PDOCB pdocb, PGSTB pgstb, long cls, PGCB pgcb)
{
	AssertVar(cls == kclsGST || cls == kclsAST, "bad cls", &cls);
	PDCST pdcst;

	if (pvNil == (pdcst = NewObj DCST(pdocb, pgstb, cls, pgcb)))
		return pvNil;

	if (!pdcst->_FInit())
		{
		ReleasePpo(&pdcst);
		return pvNil;
		}
	pdcst->Activate(fTrue);

	AssertPo(pdcst, 0);
	return pdcst;
}


/***************************************************************************
	Draw the contents of the DCGL.
***************************************************************************/
void DCST::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	PGSTB pgstb;
	STN stn;
	STN stnT;
	byte rgb[1024];
	long ivMac, iv;
	long cbDisp, cbExtra;
	long xp, yp, ibT;
	byte bT;
	RC rc;
	HQ hqExtra = hqNil;
	byte *pbExtra = rgb;

	Assert(size(rgb) >= kcbMaxDispGrp, "rgb too small");
	pgnv->ClipRc(prcClip);
	pgnv->GetRcSrc(&rc);
	pgnv->FillRc(prcClip, kacrWhite);
	pgnv->SetOnn(_onn);

	pgstb = (PGSTB)_pgrpb;
	if (size(rgb) < (cbExtra = pgstb->CbExtra()))
		{
		if (FAllocHq(&hqExtra, cbExtra, fmemNil, mprNormal))
			pbExtra = (byte *)PvLockHq(hqExtra);
		else
			pbExtra = pvNil;
		}

	if ((cbDisp = cbExtra) > kcbMaxDispGrp)
		{
		//the rest will be filled with periods to indicate that
		//not everything is displayed
		cbDisp = kcbMaxDispGrp - 2;
		}

	ivMac = pgstb->IvMac();
	xp = _XpFromIch(0);

	iv = _LnFromYp(LwMax(_dypHeader, prcClip->ypTop)) / _clnItem;
	yp = _YpFromIvDln(iv, 0);

	for ( ; iv < ivMac && yp < prcClip->ypBottom; iv++)
		{
		if (pgstb->FFree(iv))
			{
			stn.FFormatSz(PszLit("%5d)  FREE"), iv);
			pgnv->DrawStn(&stn, xp, yp);
			goto LDrawBorder;
			}

		pgstb->GetStn(iv, &stnT);
		stn.FFormatSz(PszLit("%5d)  \"%s\""), iv, &stnT);
		pgnv->DrawStn(&stn, xp, yp);

		if (_clnItem > 1 && pvNil != pbExtra)
			{
			pgstb->GetExtra(iv, pbExtra);
			stn = PszLit("       ");

			//now add the bytes in hex, with a space after every four bytes
			for (ibT = 0; ibT < cbDisp; ibT++)
				{
				bT = pbExtra[ibT];
				if ((ibT & 0x03) == 0)
					stn.FAppendCh(kchSpace);
				stn.FAppendCh(vrgchHex[(bT >> 4) & 0x0F]);
				stn.FAppendCh(vrgchHex[bT & 0x0F]);
				}
			//add the periods
			if (cbDisp < cbExtra)
				stn.FAppendSz(PszLit("...."));
			stn.FAppendSz(PszLit("  "));

			//now comes the ascii characters.
			for (ibT = 0; ibT < cbDisp; ibT++)
				{
				bT = pbExtra[ibT];
				if (bT < 32 || bT == 0x7F)
					bT = '?';
				stn.FAppendCh((achar)bT);
				}
			//add the periods
			if (cbDisp < cbExtra)
				stn.FAppendSz(PszLit(".."));

			pgnv->DrawStn(&stn, xp, yp + _dypLine);
			}

LDrawBorder:
		yp += _clnItem * _dypLine;
		rc.ypTop = yp - _dypBorder;
		rc.ypBottom = yp;
		pgnv->FillRc(&rc, kacrBlack);
		}

	if (hqNil != hqExtra)
		{
		UnlockHq(hqExtra);
		FreePhq(&hqExtra);
		}

	//draw the selection
	if (_fActive)
		_DrawSel(pgnv);
}


/***************************************************************************
	Handle cidAddItem and cidInsertItem.
***************************************************************************/
bool DCST::FCmdAddItem(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	long ivNew;
	bool fT;
	PGSTB pgstb;
	HQ hq;
	long cb;
	STN stn;

	stn.SetNil();
	pgstb = (PGSTB)_pgrpb;
	fT = fFalse;
	switch (pcmd->cid)
		{
	case cidAddItem:
		fT = pgstb->FAddStn(&stn, pvNil, &ivNew);
		break;
	case cidInsertItem:
		if (_fAllocated)
			break;
		Assert(pgstb->FIs(kclsGST), "bad grpb");
		fT = ((PGST)pgstb)->FInsertStn(ivNew = _ivCur, &stn, pvNil);
		}
	if (!fT)
		return fTrue;

	if ((cb = pgstb->CbExtra()) > 0 && FAllocHq(&hq, cb, fmemClear, mprNormal))
		{
		pgstb->PutExtra(ivNew, PvLockHq(hq));
		UnlockHq(hq);
		FreePhq(&hq);
		}

	//turn off the selection and invalidate stuff
	_SetSel(lnNil);
	InvalAllDcgb(_pdocb, pgstb, ivNew, 1,
		(_fAllocated && ivNew == pgstb->IvMac() - 1) ? 1 : 0);
	_SetSel(_LnFromIvDln(ivNew, 0));
	_ShowSel();

	return fTrue;
}


/***************************************************************************
	Constructor for DOCI.  DOCI holds an item in a GRPB contained in a
	chunky file.  Doesn't free the GRPB when the doc goes away.
***************************************************************************/
DOCI::DOCI(PDOCB pdocb, PGRPB pgrpb, long cls, long iv, long dln)
	: DOCB(pdocb)
{
	AssertPo(pgrpb, 0);
	AssertVar(pgrpb->FIs(cls), "wrong cls value", &cls);
	AssertIn(iv, 0, pgrpb->IvMac());
	_pgrpb = pgrpb;
	_cls = cls;
	_iv = iv;
	_dln = dln;
}


/***************************************************************************
	Static member to create a new DOCI.
***************************************************************************/
PDOCI DOCI::PdociNew(PDOCB pdocb, PGRPB pgrpb, long cls, long iv, long dln)
{
	AssertPo(pgrpb, 0);
	AssertPo(pdocb, 0);

	PDOCI pdoci;

	if (pvNil == (pdoci = NewObj DOCI(pdocb, pgrpb, cls, iv, dln)))
		return pvNil;
	if (!pdoci->_FInit())
		{
		ReleasePpo(&pdoci);
		return pvNil;
		}
	AssertPo(pdoci, 0);
	return pdoci;
}


/***************************************************************************
	Reads the data (from the GRPB) and initializes the stream.
***************************************************************************/
bool DOCI::_FInit(void)
{
	bool fRet;
	HQ hq;
	long cb;

	if (hqNil == (hq = _HqRead()))
		return fFalse;

	if (0 < (cb = CbOfHq(hq)))
		{
		fRet = _bsf.FReplace(PvLockHq(hq), cb, 0, 0);
		UnlockHq(hq);
		}
	else
		fRet = fTrue;
	FreePhq(&hq);
	return fRet;
}


/***************************************************************************
	Create a new DDG for this doc.
***************************************************************************/
PDDG DOCI::PddgNew(PGCB pgcb)
{
	return DCH::PdchNew(this, &_bsf, _fFixed, pgcb);
}


/***************************************************************************
	Get the document's name.
***************************************************************************/
void DOCI::GetName(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	STN stn;

	stn.FFormatSz(PszLit(": item %d"), _iv);
	_pdocbPar->GetName(pstn);
	pstn->FAppendStn(&stn);
}


/***************************************************************************
	Handle saving.  This is a virtual member of DOCB.
***************************************************************************/
bool DOCI::FSave(long cid)
{
	AssertThis(0);
	long iv;

	switch (cid)
		{
	case cidSave:
		iv = _iv;
		break;

	case cidSaveAs:
	case cidSaveCopy:
		RawRtn(); //REVIEW shonk: implement
		return fFalse;

	default:
		return fFalse;
		}

	return _FSaveToItem(iv, cid != cidSaveCopy);
}


/***************************************************************************
	Save the data to an item.
***************************************************************************/
bool DOCI::_FSaveToItem(long iv, bool fRedirect)
{
	if (!_FWrite(iv))
		return fFalse;

	//REVIEW shonk: update window names
	if (fRedirect)
		{
		_iv = iv;
		_fDirty = fFalse;
		}

	DCGB::InvalAllDcgb(_pdocbPar, _pgrpb, iv, 1, 1);
	return fTrue;
}


/***************************************************************************
	Write the data to the GRPB at the given item number.
***************************************************************************/
bool DOCI::_FWrite(long iv)
{
	long cb;
	HQ hq;
	void *pv;
	bool fRet;

	cb = _bsf.IbMac();
	if (!FAllocHq(&hq, LwMax(1, cb), fmemNil, mprNormal))
		return fFalse;
	_bsf.FetchRgb(0, cb, pv = PvLockHq(hq));
	fRet = fTrue;
	switch (_cls)
		{
	case kclsGL:
	case kclsAL:
		Assert(cb == ((PGLB)_pgrpb)->CbEntry(), "bad cb in GL/AL");
        ((PGLB)_pgrpb)->Put(iv, pv);
		break;
	case kclsGG:
	case kclsAG:
		if (_dln == 0)
			fRet = ((PGGB)_pgrpb)->FPut(iv, cb, pv);
		else
			{
			Assert(cb == ((PGGB)_pgrpb)->CbFixed(), "bad cb in GG/AG");
			((PGGB)_pgrpb)->PutFixed(iv, pv);
			}
		break;
	case kclsGST:
	case kclsAST:
		if (_dln == 0)
			{
			cb = LwMin(cb, kcchMaxStz);
			fRet = ((PGSTB)_pgrpb)->FPutRgch(iv, (achar *)pv, cb / size(achar));
			}
		else
			{
			Assert(cb == ((PGSTB)_pgrpb)->CbExtra(), "bad cb in GST/AST");
			((PGSTB)_pgrpb)->PutExtra(iv, pv);
			}
		break;
	default:
		BugVar("bad cls value", &_cls);
		fRet = fFalse;
		break;
		}
	UnlockHq(hq);
	FreePhq(&hq);

	return fRet;
}


/***************************************************************************
	Read the data from the GRPB and return it in an hq.  Also set the
	_fFixed flag (which indicates if the data is fixed length).
***************************************************************************/
HQ DOCI::_HqRead(void)
{
	long cb;
	HQ hq;
	void *pv;
	STN stn;

	_fFixed = fTrue;
	switch (_cls)
		{
	case kclsGL:
	case kclsAL:
		cb = ((PGLB)_pgrpb)->CbEntry();
		break;
	case kclsGG:
	case kclsAG:
		if (_dln == 0)
			{
			cb = ((PGGB)_pgrpb)->Cb(_iv);
			_fFixed = fFalse;
			}
		else
			cb = ((PGGB)_pgrpb)->CbFixed();
		break;
	case kclsGST:
	case kclsAST:
		if (_dln == 0)
			{
			((PGSTB)_pgrpb)->GetStn(_iv, &stn);
			cb = stn.Cch() * size(achar);
			_fFixed = fFalse;
			}
		else
			cb = ((PGSTB)_pgrpb)->CbExtra();
		break;
	default:
		BugVar("bad cls value", &_cls);
		return hqNil;
		}

	if (!FAllocHq(&hq, cb, fmemNil, mprNormal))
		return hqNil;
	pv = PvLockHq(hq);

	switch (_cls)
		{
	case kclsGL:
	case kclsAL:
        ((PGLB)_pgrpb)->Get(_iv, pv);
		break;
	case kclsGG:
	case kclsAG:
		if (_dln == 0)
			((PGGB)_pgrpb)->Get(_iv, pv);
		else
			((PGGB)_pgrpb)->GetFixed(_iv, pv);
		break;
	case kclsGST:
	case kclsAST:
		if (_dln == 0)
			CopyPb(stn.Prgch(), pv, stn.Cch() * size(achar));
		else
			((PGSTB)_pgrpb)->GetExtra(_iv, pv);
		break;
		}
	UnlockHq(hq);
	return hq;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a DOCI.
***************************************************************************/
void DOCI::AssertValid(ulong grf)
{
	DOCI_PAR::AssertValid(0);
	AssertPo(_pgrpb, 0);
}


/***************************************************************************
	Mark memory for the DOCI.
***************************************************************************/
void DOCI::MarkMem(void)
{
	AssertValid(0);
	DOCI_PAR::MarkMem();
	MarkMemObj(_pgrpb);
}
#endif //DEBUG


