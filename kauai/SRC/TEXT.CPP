/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Text management classes.

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(EDCB)
RTCLASS(EDSL)
RTCLASS(EDPL)
RTCLASS(EDML)
RTCLASS(EDMW)

const long kdxpInsetEdcb = 2;
const long kdxpInsetSled = 2;


/***************************************************************************
	Constructor for edit control parameter block.
***************************************************************************/
EDPAR::EDPAR(long hid, PGOB pgob, ulong grfgob, long gin, RC *prcAbs, RC *prcRel,
	long onn, ulong grfont, long dypFont, long tah, long tav, ACR acrFore,
	ACR acrBack, long cmhl) : _gcb(hid, pgob, grfgob, gin, prcAbs, prcRel)
{
	_onn = onn;
	_grfont = grfont;
	_dypFont = dypFont;
	_tah = tah;
	_tav = tav;
	_acrFore = acrFore;
	_acrBack = acrBack;
	_cmhl = cmhl;
}


/***************************************************************************
	Set the data in the EDPAR.
***************************************************************************/
void EDPAR::Set(long hid, PGOB pgob, ulong grfgob, long gin, RC *prcAbs, RC *prcRel,
	long onn, ulong grfont, long dypFont, long tah, long tav, ACR acrFore,
	ACR acrBack, long cmhl)
{
	_gcb.Set(hid, pgob, grfgob, gin, prcAbs, prcRel);
	_onn = onn;
	_grfont = grfont;
	_dypFont = dypFont;
	_tah = tah;
	_tav = tav;
	_acrFore = acrFore;
	_acrBack = acrBack;
	_cmhl = cmhl;
}


/***************************************************************************
	Set the font portion of the EDPAR.
***************************************************************************/
void EDPAR::SetFont(long onn, ulong grfont, long dypFont, long tah,
	long tav, ACR acrFore, ACR acrBack)
{
	_onn = onn;
	_grfont = grfont;
	_dypFont = dypFont;
	_tah = tah;
	_tav = tav;
	_acrFore = acrFore;
	_acrBack = acrBack;
}


/***************************************************************************
	Constructor for edit control.
***************************************************************************/
EDCB::EDCB(PGCB pgcb, long cmhl) : GOB(pgcb)
{
	AssertBaseThis(0);
	_cmhl = cmhl;
	_fMark = (kginMark == pgcb->_gin ||
		 kginDefault == pgcb->_gin && kginMark == GOB::GinDefault());
	_pgnv = pvNil;
}


/***************************************************************************
	Constructor for edit control.
***************************************************************************/
EDCB::~EDCB(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pgnv);
}


/***************************************************************************
	Initialize the edit control.
***************************************************************************/
bool EDCB::_FInit(void)
{
	AssertBaseThis(0);
	PGPT pgpt;

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

	return pvNil != _pgnv;
}


/***************************************************************************
	Draw the contents of the gob.
***************************************************************************/
void EDCB::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	long ln, lnLast;
	RC rc;

	_GetRcContent(&rc);
	if (rc.FIntersect(prcClip))
		{
		pgnv->IntersectRcVis(&rc);
		ln = _LnFromYp(prcClip->ypTop);
		lnLast = _LnFromYp(prcClip->ypBottom - 1);
	
		for ( ; ln <= lnLast; ln++)
			_DrawLine(pgnv, ln);
		if (_fSelOn)
			_InvertSel(pgnv);
		}
}


/***************************************************************************
	Handle a mousedown in the edit control.
***************************************************************************/
bool EDCB::FCmdTrackMouse(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	long ln, ich;

	if (pcmd->cid == cidMouseDown)
		{
		Assert(vpcex->PgobTracking() == pvNil, "mouse already being tracked!");
		vpcex->TrackMouse(this);
		_fSelByWord = (pcmd->cact > 1) && !(pcmd->grfcust & fcustShift);
		}
	else
		{
		Assert(vpcex->PgobTracking() == this, "not tracking mouse!");
		Assert(pcmd->cid == cidTrackMouse, 0);
		}

	ln = _LnFromYp(pcmd->yp);
	ich = _IchFromLnXp(ln, pcmd->xp, !_fSelByWord);
	ich = LwBound(ich, 0, IchMac() + 1);
	if (pcmd->cid != cidMouseDown || (pcmd->grfcust & fcustShift))
		{
		if (_fSelByWord)
			{
			ich = (ich < _ichAnchor) ? _IchPrev(ich + 1, fTrue) :
				_IchNext(ich, fTrue);
			}
		SetSel(_ichAnchor, ich);
		}
	else
		{
		if (_fSelByWord)
			ich = _IchPrev(ich + 1, fTrue);
		SetSel(ich, _fSelByWord ? _IchNext(ich, fTrue) : ich);
		}
	_fXpValid = fFalse;
	_SwitchSel(fTrue);	//make sure the selection is on
	ShowSel(fFalse);

	if (!(pcmd->grfcust & fcustMouse))
		vpcex->EndMouseTracking();

	return fTrue;
}


/***************************************************************************
	Handle a key down.
***************************************************************************/
bool EDCB::FCmdKey(PCMD_KEY pcmd)
{
	const long kcchInsBuf = 64;
	AssertThis(0);
	AssertVarMem(pcmd);
	ulong grfcust;
	long vkDone;
	long dich, ichLim;
	long dlnSel, ln, lnMac;
	long cact;
	CMD cmd;
	achar rgch[kcchInsBuf + 1];
	bool fPage;

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
			if (!_FFilterCh((achar)pcmd->ch))
				{
				cmd = *(CMD *)pcmd;
				cmd.cid = cidBadKey;
				cmd.pcmh = PgobPar();
				((PCMD_BADKEY)&cmd)->hid = Hid();
				vpcex->PushCmd(&cmd);
				goto LInsert;
				}

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
		FReplace(rgch, ichLim, _ichAnchor, _ichOther);
		}

	dich = 0;
	fPage = fFalse;
	switch (vkDone)
		{
	case kvkHome:
		if (grfcust & fcustCmd)
			dich = -_ichOther;
		else
			dich = _IchMinLn(_LnFromIch(_ichOther)) - _ichOther;
		_fXpValid = fFalse;
		goto LSetSel;

	case kvkEnd:
		if (grfcust & fcustCmd)
			dich = IchMac() - _ichOther;
		else
			{
			dich = _IchMinLn(_LnFromIch(_ichOther) + 1);
			dich = _IchPrev(dich) - _ichOther;
			}
		_fXpValid = fFalse;
		goto LSetSel;

	case kvkLeft:
		dich = _IchPrev(_ichOther, FPure(grfcust & fcustCmd)) - _ichOther;
		_fXpValid = fFalse;
		goto LSetSel;

	case kvkRight:
		dich = _IchNext(_ichOther, FPure(grfcust & fcustCmd)) - _ichOther;
		_fXpValid = fFalse;
		goto LSetSel;

	case kvkPageUp:
		fPage = fTrue;
	case kvkUp:
		dlnSel = -1;
		goto LMoveVert;
	case kvkPageDown:
		fPage = fTrue;
	case kvkDown:
		dlnSel = 1;
LMoveVert:
		lnMac = _LnMac();
		ln = _LnFromIch(_ichOther);

		if (dlnSel < 0 && ln == 0)
			{
			//do the same as the home key
			dich = -_ichOther;
			_fXpValid = fFalse;
			}
		else if (dlnSel > 0 && ln >= lnMac - 1)
			{
			//do the same as the end key
			dich = IchMac() - _ichOther;
			_fXpValid = fFalse;
			}
		else
			{
			if (!_fXpValid)
				{
				_xpSel = _XpFromIch(_ichOther) - _xp;
				_fXpValid = fTrue;
				}
			if (fPage)
				{
				//determine dlnSel
				RC rc;
				long yp, dyp;

				_GetRcContent(&rc);
				yp = _YpFromLn(ln);
				dyp = _YpFromLn(ln + 1) - yp;
				dyp = rc.Dyp() - dyp;
				Assert(LwAbs(dlnSel) == 1, "bad dlnSel");
				yp = yp + dlnSel * dyp; //the target yp
				dlnSel = dlnSel * LwMax(1, LwAbs(_LnFromYp(yp) - ln));
				}
			//to play it safe...
			ln = LwBound(ln + dlnSel, 0, lnMac);
			dich = _IchFromLnXp(ln, _xpSel + _xp) - _ichOther;
			}

LSetSel:
		//move the selection
		if (grfcust & fcustShift)
			{
			//extend selection
			SetSel(_ichAnchor, _ichOther + dich);
			ShowSel();
			}
		else
			{
			long ichT = _ichOther;

			if (ichT == _ichAnchor || (grfcust & fcustCmd))
				ichT += dich;
			else if ((dich > 0) != (ichT > _ichAnchor))
				ichT = _ichAnchor;
			SetSel(ichT, ichT);
			ShowSel();
			}
		break;

	case kvkDelete:
	case kvkBack:
		if (_ichAnchor != _ichOther)
			dich = _ichOther - _ichAnchor;
		else if (vkDone == kvkDelete)
			dich = _IchNext(_ichAnchor) - _ichAnchor;
		else
			dich = _IchPrev(_ichAnchor) - _ichAnchor;
		if (dich != 0)
			FReplace(pvNil, 0, _ichAnchor, _ichAnchor + dich);
		break;
		}

	return fTrue;
}


/***************************************************************************
	Do idle processing.  If this handler has the active selection, make sure
	the selection is on or off according to rglw[0] (non-zero means on)
	and set rglw[0] to false.  Always return false.
***************************************************************************/
bool EDCB::FCmdSelIdle(PCMD pcmd)
{
	AssertThis(0);

	//if rglw[1] is this one's hid, don't change the sel state.
	if (pcmd->rglw[1] != Hid())
		{
		if (!pcmd->rglw[0])
			_SwitchSel(fFalse, kginDefault);
		else if (_ichAnchor != _ichOther || _tsSel == 0)
			_SwitchSel(fTrue);
		else if (DtsCaret() < TsCurrent() - _tsSel)
			_SwitchSel(!_fSelOn);
		}
	pcmd->rglw[0] = fFalse;
	return fFalse;
}


/***************************************************************************
	Handle an activate sel command.
***************************************************************************/
bool EDCB::FCmdActivateSel(PCMD pcmd)
{
	Activate(fTrue);
	return fTrue;
}


/***************************************************************************
	Either make the selection for the EDCB active or inactive.
***************************************************************************/
void EDCB::Activate(bool fActive)
{
	AssertThis(0);
	vpcex->RemoveCmh(this, _cmhl);
	if (fActive)
		vpcex->FAddCmh(this, _cmhl);
	else
		_SwitchSel(fFalse, kginDefault);
}


/***************************************************************************
	Get the rectangle that is to contain the text.  This allows derived
	classes to have borders, etc.
***************************************************************************/
void EDCB::_GetRcContent(RC *prc)
{
	GetRc(prc, cooLocal);
}


/***************************************************************************
	Set the vis for the GNV to be the intersection of the GOB's vis and
	the content rc.
***************************************************************************/
void EDCB::_InitGnv(PGNV pgnv)
{
	RC rc;

	_GetRcContent(&rc);
	pgnv->IntersectRcVis(&rc);
}


/***************************************************************************
	The rectangle has changed - show the selection.
***************************************************************************/
void EDCB::_NewRc(void)
{
	ShowSel(fTrue, ginNil);
}


/***************************************************************************
	Set the selection.
***************************************************************************/
void EDCB::SetSel(long ichAnchor, long ichOther, long gin)
{
	AssertThis(0);
	long ichMac = IchMac();

	ichAnchor = LwBound(ichAnchor, 0, ichMac + 1);
	ichOther = LwBound(ichOther, 0, ichMac + 1);

	if (ichAnchor == _ichAnchor && ichOther == _ichOther)
		return;

	if (_fSelOn)
		{
		if ((_fMark || _fClear) && gin == kginDraw)
			gin = kginMark;
		_InitGnv(_pgnv);
		if (_ichAnchor != ichAnchor || _ichAnchor == _ichOther ||
				ichAnchor == ichOther)
			{
			_InvertSel(_pgnv, gin);
			_ichAnchor = ichAnchor;
			_ichOther = ichOther;
			_InvertSel(_pgnv, gin);
			_tsSel = TsCurrent();
			}
		else
			{
			//they have the same anchor and neither is an insertion
			_InvertIchRange(_pgnv, _ichOther, ichOther, gin);
			_ichOther = ichOther;
			}
		}
	else
		{
		_ichAnchor = ichAnchor;
		_ichOther = ichOther;
		_tsSel = 0L;
		}
}


/***************************************************************************
	Turn the sel on or off according to fOn.
***************************************************************************/
void EDCB::_SwitchSel(bool fOn, long gin)
{
	AssertThis(0);

	if (FPure(fOn) != FPure(_fSelOn))
		{
		if ((_fMark || _fClear) && gin == kginDraw)
			gin = kginMark;
		_InitGnv(_pgnv);
		_InvertSel(_pgnv, gin);
		_fSelOn = FPure(fOn);
		_tsSel = TsCurrent();
		}
}


/***************************************************************************
	Make sure the selection is visible (or at least _ichOther is).
***************************************************************************/
void EDCB::ShowSel(bool fForceJustification, long gin)
{
	AssertThis(0);
	long ln, lnHope;
	long dxpScroll, dypScroll;
	long zpMin, zpLim;
	RC rc;
	long ichAnchor = _ichAnchor;

	//find the lines we want to show
	ln = _LnFromIch(_ichOther);
	lnHope = _LnFromIch(ichAnchor);
	_GetRcContent(&rc);
	rc.Inset(kdxpInsetEdcb, 0);

	//find the height needed to display these
	zpMin = _YpFromLn(LwMin(ln, lnHope));
	zpLim = _YpFromLn(LwMax(ln, lnHope) + 1);
	if (zpLim > zpMin + rc.Dyp() && ln != lnHope)
		{
		//can't show both
		if (lnHope > ln)
			{
			zpMin = _YpFromLn(ln);
			zpLim = zpMin + rc.Dyp();
			}
		else
			{
			zpLim = _YpFromLn(ln + 1);
			zpMin = zpLim - rc.Dyp();
			}
		ichAnchor = _ichOther;
		}
	dypScroll = fForceJustification ? -_yp : 0;
	dypScroll = LwMax(LwMin(dypScroll, rc.ypBottom - zpLim), rc.ypTop - zpMin);

	// now do the horizontal stuff
	zpMin = _XpFromIch(_ichOther);
	zpLim = _XpFromIch(ichAnchor);
	if (LwAbs(zpLim - zpMin) > rc.Dxp())
		{
		// can't show both
		if (zpMin > zpLim)
			{
			zpLim = zpMin;
			zpMin = zpLim - rc.Dxp();
			}
		else
			zpLim = zpMin + rc.Dxp();
		}
	else
		SortLw(&zpMin, &zpLim);
	dxpScroll = fForceJustification ? -_xp : 0;
	dxpScroll = LwMax(LwMin(dxpScroll, rc.xpRight - zpLim), rc.xpLeft - zpMin);

	if (dxpScroll != 0 || dypScroll != 0)
		{
		if ((_fMark || _fClear) && gin == kginDraw)
			gin = kginMark;
		_Scroll(dxpScroll, dypScroll, gin);
		}
}


/***************************************************************************
	Invert the current selection.
***************************************************************************/
void EDCB::_InvertSel(PGNV pgnv, long gin)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	RC rc, rcT;
	long ln;

	if (_ichAnchor == _ichOther)
		{
		//insertion bar
		ln = _LnFromIch(_ichAnchor);
		rc.xpLeft = _XpFromIch(_ichAnchor) - 1;
		rc.xpRight = rc.xpLeft + 2;
		rc.ypTop = _YpFromLn(ln);
		rc.ypBottom = _YpFromLn(ln + 1);
		_GetRcContent(&rcT);
		if (rcT.FIntersect(&rc))
			{
			if (kginDraw == gin)
				pgnv->FillRc(&rcT, kacrInvert);
			else
				InvalRc(&rcT, gin);
			}
		}
	else
		_InvertIchRange(pgnv, _ichAnchor, _ichOther, gin);
}


/***************************************************************************
	Invert a range.
***************************************************************************/
void EDCB::_InvertIchRange(PGNV pgnv, long ich1, long ich2, long gin)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertIn(ich1, 0, IchMac() + 1);
	AssertIn(ich2, 0, IchMac() + 1);
	RC rc, rcClip, rcT;
	long ln1, ln2, xp2;

	if (ich1 == ich2)
		return;
	SortLw(&ich1, &ich2);
	ln1 = _LnFromIch(ich1);
	ln2 = _LnFromIch(ich2);

	_GetRcContent(&rcClip);
	rc.xpLeft = _XpFromIch(ich1);
	rc.ypTop = _YpFromLn(ln1);
	rc.ypBottom = _YpFromLn(ln1 + 1);
	xp2 = _XpFromIch(ich2);

	if (ln2 == ln1)
		{
		//only one line involved
		rc.xpRight = xp2;
		if (rcT.FIntersect(&rc, &rcClip))
			{
			if (kginDraw == gin)
				_HiliteRc(pgnv, &rcT);
			else
				InvalRc(&rcT, gin);
			}
		return;
		}

	// invert the sel on the first line
	rc.xpRight = rcClip.xpRight;
	if (rcT.FIntersect(&rc, &rcClip))
		{
		if (kginDraw == gin)
			_HiliteRc(pgnv, &rcT);
		else
			InvalRc(&rcT, gin);
		}

	//invert the main rectangular block
	rc.xpLeft = rcClip.xpLeft;
	rc.ypTop = rc.ypBottom;
	rc.ypBottom = _YpFromLn(ln2);
	if (rcT.FIntersect(&rc, &rcClip))
		{
		if (kginDraw == gin)
			_HiliteRc(pgnv, &rcT);
		else
			InvalRc(&rcT, gin);
		}

	//invert the last line
	rc.ypTop = rc.ypBottom;
	rc.ypBottom = _YpFromLn(ln2 + 1);
	rc.xpRight = xp2;
	if (rcT.FIntersect(&rc, &rcClip))
		{
		if (kginDraw == gin)
			_HiliteRc(pgnv, &rcT);
		else
			InvalRc(&rcT, gin);
		}
}


/***************************************************************************
	Update the correct lines on screen.
***************************************************************************/
void EDCB::_UpdateLn(long ln, long clnIns, long clnDel, long dypDel, long gin)
{
	AssertThis(0);
	AssertIn(ln, 0, _LnMac());
	AssertIn(clnIns, 0, _LnMac() + 1 - ln);
	AssertIn(clnDel, 0, kcbMax);
	AssertIn(dypDel, 0, kcbMax);
	RC rcLoc, rc;
	long yp, dypIns;
	long lnMac = _LnFromIch(IchMac());

	_GetRcContent(&rcLoc);

	yp = _YpFromLn(ln);
	dypIns = _YpFromLn(ln + clnIns) - yp;
	rc = rcLoc;
	rc.ypTop = yp;
	rc.ypBottom = yp + dypIns;
	if (lnMac > ln + clnIns - clnDel && dypIns != dypDel)
		{
		// Have some bits to blt vertically. If the background isn't clear,
		// but _fMark is set, still do the scroll, since _fMark is intended
		// to avoid flashing (allowing offscreen drawing) and scrolling
		// doesn't flash anyway.
		if (_fClear && kginDraw == gin)
			gin = kginMark;

		if (kginDraw != gin)
			rc.ypBottom = rcLoc.ypBottom;
		else
			{
			if (_fMark)
				gin = kginMark;

			rc = rcLoc;
			rc.ypTop = LwMax(rc.ypTop, yp + LwMin(dypIns, dypDel));
			Scroll(&rc, 0, dypIns - dypDel, gin);
			rc.ypBottom = rc.ypTop;
			rc.ypTop = yp;
			}
		}
	else if ((_fMark || _fClear) && gin == kginDraw)
		gin = kginMark;

	if (!rc.FEmpty())
		InvalRc(&rc, gin);

	ShowSel(fTrue, gin);
	_fXpValid = fFalse;
}


/***************************************************************************
	Scroll the text in the edit control.
***************************************************************************/
void EDCB::_Scroll(long dxp, long dyp, long gin)
{
	AssertThis(0);
	RC rc;

	_xp += dxp;
	_yp += dyp;
	_GetRcContent(&rc);
	Scroll(&rc, dxp, dyp, gin);
}


/***************************************************************************
	Return the yp for the given character.
***************************************************************************/
long EDCB::_YpFromIch(long ich)
{
	return _YpFromLn(_LnFromIch(ich));
}


/***************************************************************************
	Return the single character at ich.
***************************************************************************/
achar EDCB::_ChFetch(long ich)
{
	AssertThis(0);
	AssertIn(ich, 0, IchMac());
	achar ch;

	CchFetch(&ch, ich, 1);
	return ch;
}


/***************************************************************************
	Return ich of the previous character, skipping line feed characters. If
	fWord is true, skip to the beginning of a word.
***************************************************************************/
long EDCB::_IchPrev(long ich, bool fWord)
{
	AssertThis(0);
	AssertIn(ich, 0, IchMac() + 2);

	if (ich > IchMac())
		return IchMac();

	if (!fWord)
		{
		while (ich > 0 && (fchIgnore & GrfchFromCh(_ChFetch(--ich))))
			;
		}
	else
		{
		while (ich > 0 &&
				((fchIgnore | fchMayBreak) & GrfchFromCh(_ChFetch(ich - 1))))
			{
			ich--;
			}
		while (ich > 0 &&
				!((fchIgnore | fchMayBreak) & GrfchFromCh(_ChFetch(ich - 1))))
			{
			ich--;
			}
		}

	return ich;
}


/***************************************************************************
	Return ich of the next character, skipping line feed characters. If
	fWord is true, skip to the beginning of the next word.
***************************************************************************/
long EDCB::_IchNext(long ich, bool fWord)
{
	AssertThis(0);
	AssertIn(ich, 0, IchMac() + 1);
	long ichMac = IchMac();

	if (ich >= ichMac)
		return ichMac;

	if (!fWord)
		{
		while (++ich < ichMac && (fchIgnore & GrfchFromCh(_ChFetch(ich))))
			;
		}
	else
		{
		while (ich < ichMac &&
				!((fchIgnore | fchMayBreak) & GrfchFromCh(_ChFetch(ich))))
			{
			ich++;
			}
		while (ich < ichMac &&
				((fchIgnore | fchMayBreak) & GrfchFromCh(_ChFetch(ich))))
			{
			ich++;
			}
		}
	return ich;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the EDCB
***************************************************************************/
void EDCB::AssertValid(ulong grf)
{
	EDCB_PAR::AssertValid(0);
	AssertIn(_ichAnchor, 0, kcbMax);
	AssertIn(_ichOther, 0, kcbMax);
	AssertPo(_pgnv, 0);
	//REVIEW shonk: fill in EDCB::AssertValid
}


/***************************************************************************
	Mark memory for the EDML.
***************************************************************************/
void EDCB::MarkMem(void)
{
	AssertValid(0);
	EDCB_PAR::MarkMem();
	MarkMemObj(_pgnv);
}
#endif


/***************************************************************************
	Constructor for plain edit control.
***************************************************************************/
EDPL::EDPL(PEDPAR pedpar) : EDCB(&pedpar->_gcb, pedpar->_cmhl)
{
	// inputs are all asserted in AssertThis
	_onn = pedpar->_onn;
	_grfont = pedpar->_grfont;
	_dypFont = pedpar->_dypFont;
	_tah = pedpar->_tah;
	_tav = pedpar->_tav;
	_acrFore = pedpar->_acrFore;
	_acrBack = pedpar->_acrBack;
	_fClear = kacrClear == _acrBack;

	_dypLine = 0;
}


/***************************************************************************
	Initialize the EDPL.
***************************************************************************/
bool EDPL::_FInit(void)
{
	AssertBaseThis(0);
	RC rc;

	if (!EDPL_PAR::_FInit())
		return fFalse;

	// get the _dypLine value
	_pgnv->SetFont(_onn, _grfont, _dypFont, _tah);
	_pgnv->GetRcFromRgch(&rc, pvNil, 0, 0, 0);
	_dypLine = rc.Dyp();

	return fTrue;
}


/***************************************************************************
	Return the yp for the given line.
***************************************************************************/
long EDPL::_YpFromLn(long ln)
{
	AssertThis(0);
	AssertIn(ln, 0, _LnMac() + 1);
	RC rc;

	_GetRcContent(&rc);
	switch (_tav)
		{
	default:
		return rc.ypTop + _yp + LwMul(ln, _dypLine);
	case tavBottom:
		return rc.ypBottom + _yp - LwMul(_LnMac() - ln, _dypLine);
	case tavCenter:
		return rc.YpCenter() + _yp - LwMul(_LnMac(), _dypLine) / 2 +
			LwMul(ln, _dypLine);
		}
}


/***************************************************************************
	Return which line the yp belongs in.
***************************************************************************/
long EDPL::_LnFromYp(long yp)
{
	AssertThis(0);
	long yp0 = _YpFromLn(0);
	return LwBound((yp - yp0) / _dypLine, 0, _LnMac() + 1);
}


/***************************************************************************
	Hilite the rectangle.
***************************************************************************/
void EDPL::_HiliteRc(PGNV pgnv, RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);
	pgnv->HiliteRc(prc, _acrBack);
}


/***************************************************************************
	Return the xp for the given character.
***************************************************************************/
long EDPL::_XpFromIch(long ich)
{
	AssertThis(0);
	AssertIn(ich, 0, IchMac() + 1);
	RC rc;
	achar *prgch;
	long ln, ichMin, cch;
	long xp = _XpOrigin();

	ln = _LnFromIch(ich);
	ichMin = _IchMinLn(ln);
	if (!_FLockLn(ln, &prgch, &cch))
		return xp;
	AssertPvCb(prgch, cch * size(achar));
	AssertIn(ich, ichMin, ichMin + cch + 1);

	_pgnv->SetFont(_onn, _grfont, _dypFont, _tah);
	if (tahLeft != _tah)
		{
		RC rcAll;

		_pgnv->GetRcFromRgch(&rcAll, prgch, cch, xp, _yp);
		xp = rcAll.xpLeft;
		_pgnv->SetFontAlign(tahLeft, tavTop);
		}
	_pgnv->GetRcFromRgch(&rc, prgch, ich - ichMin, xp, _yp);
	_UnlockLn(ln, prgch);

	return rc.xpRight;
}


/***************************************************************************
	Find the character that the xp is in on the given line. If fClosest is
	true, this finds the character boundary that the point is closest to
	(for traditional selection). If fClosest is false, it finds the character
	that the xp value is over.
***************************************************************************/
long EDPL::_IchFromLnXp(long ln, long xp, bool fClosest)
{
	AssertThis(0);
	long xpT;
	long ich;
	long ichMin = _IchMinLn(ln);
	long ichMinLn = ichMin;
	long ichLim = _IchPrev(_IchMinLn(ln + 1));

	while (ichMin < ichLim)
		{
		ich = (ichMin + ichLim) / 2;
		xpT = _XpFromIch(ich);
		if (xpT < xp)
			ichMin = ich + 1;
		else
			ichLim = ich;
		}

	ich = LwMax(ichMinLn, _IchPrev(ichMin));
	if (fClosest && ich < ichMin)
		{
		// determine whether we should return ich or _IchNext(ich).
		if ((ichLim = _IchNext(ich)) <= ichMin &&
			LwAbs(xp - _XpFromIch(ichLim)) <
				LwAbs(xp - _XpFromIch(ich)))
			{
			ich = ichLim;
			}
		}
	return ich;
}


/***************************************************************************
	Draw the given line in the given GNV.
***************************************************************************/
void EDPL::_DrawLine(PGNV pgnv, long ln)
{
	AssertThis(0);
	AssertIn(ln, 0, _LnMac() + 1);
	RC rcSrc;
	achar *prgch;
	long cch;
	long ypTop;

	_GetRcContent(&rcSrc);
	ypTop = _YpFromLn(ln);
	if (_FLockLn(ln, &prgch, &cch))
		{
		AssertPvCb(prgch, cch * size(achar));
		long xp = _XpOrigin();

		pgnv->SetFont(_onn, _grfont, _dypFont, _tah);
		if (ln > 0)
			rcSrc.ypTop = ypTop;
		rcSrc.ypBottom = _YpFromLn(ln + 1);
		pgnv->FillRc(&rcSrc, _acrBack);
		pgnv->DrawRgch(prgch, cch, xp, ypTop, _acrFore, _acrBack);
		_UnlockLn(ln, prgch);
		}
	else
		{
		//fill to the bottom
		rcSrc.ypTop = ypTop;
		pgnv->FillRc(&rcSrc, _acrBack);
		}
}


/***************************************************************************
	Return the origin for drawing text.
***************************************************************************/
long EDPL::_XpOrigin(void)
{
	AssertThis(0);
	RC rc;

	_GetRcContent(&rc);
	switch (_tah)
		{
	default:
		return rc.xpLeft + _xp + kdxpInsetEdcb;
	case tahRight:
		return rc.xpRight + _xp - kdxpInsetEdcb;
	case tahCenter:
		return rc.XpCenter() + _xp;
		}
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a single-line edit control.
***************************************************************************/
void EDPL::AssertValid(ulong grf)
{
	EDPL_PAR::AssertValid(0);
	Assert(vntl.FValidOnn(_onn), 0);
	AssertIn(_dypFont, 1, kswMax);
	AssertIn(_tah, 0, tahLim);
	AssertIn(_tav, 0, tavLim);
	Assert(_tav != tavBaseline, "baseline not supported");
	AssertPo(&_acrFore, facrRgb | facrIndex);
	AssertPo(&_acrBack, facrRgb | facrIndex);
	AssertIn(_dypLine, 1, kcbMax);
}
#endif //DEBUG


/***************************************************************************
	Constructor for single line edit control.
***************************************************************************/
EDSL::EDSL(PEDPAR pedpar) : EDPL(pedpar)
{
	AssertBaseThis(0);
}


/***************************************************************************
	Create a new EDSL (single line edit control).
***************************************************************************/
PEDSL EDSL::PedslNew(PEDPAR pedpar)
{
	PEDSL pedsl;

	if (pvNil == (pedsl = NewObj EDSL(pedpar)))
		return pvNil;

	if (!pedsl->_FInit())
		ReleasePpo(&pedsl);

	AssertNilOrPo(pedsl, 0);
	return pedsl;
}


/***************************************************************************
	Get a pointer to the characters for the given line.
***************************************************************************/
bool EDSL::_FLockLn(long ln, achar **pprgch, long *pcch)
{
	AssertBaseThis(0);
	AssertVarMem(pprgch);
	AssertVarMem(pcch);
	if (ln > 0)
		{
		*pprgch = pvNil;
		*pcch = 0;
		return fFalse;
		}

	*pcch = _cch;
	*pprgch = _rgch;
	return fTrue;
}


/***************************************************************************
	Unlock a line.
***************************************************************************/
void EDSL::_UnlockLn(long ln, achar *prgch)
{
	AssertBaseThis(0);
	Assert(prgch == _rgch, "bad call to _UnlockLn");
}


/***************************************************************************
	Return the line that ich is on.
***************************************************************************/
long EDSL::_LnFromIch(long ich)
{
	AssertBaseThis(0);
	AssertIn(ich, 0, kcbMax);
	return ich <= IchMac() ? 0 : 1;
}


/***************************************************************************
	Return the first ich for the given line.
***************************************************************************/
long EDSL::_IchMinLn(long ln)
{
	AssertBaseThis(0);
	return ln == 0 ? 0 : IchMac() + 1;
}


/***************************************************************************
	Return the number of characters.
***************************************************************************/
long EDSL::IchMac(void)
{
	AssertBaseThis(0);
	return _cch;
}


/***************************************************************************
	Return the number of lines.
***************************************************************************/
long EDSL::_LnMac(void)
{
	AssertBaseThis(0);
	return 1;
}


/***************************************************************************
	Replace the characters between ich1 and ich2 with those in (prgch, cchIns).
	Calls _UpdateLn() to clean up the display.
***************************************************************************/
bool EDSL::FReplace(achar *prgch, long cchIns, long ich1, long ich2, long gin)
{
	AssertThis(0);
	AssertIn(cchIns, 0, kcbMax);
	AssertPvCb(prgch, cchIns * size(achar));
	AssertIn(ich1, 0, IchMac() + 1);
	AssertIn(ich2, 0, IchMac() + 1);

	ich1 = LwBound(ich1, 0, _cch + 1);
	ich2 = LwBound(ich2, 0, _cch + 1);
	if (ich1 == ich2 && cchIns == 0)
		return fTrue;

	_SwitchSel(fFalse, gin);
	_tsSel = 0;
	if (ich1 != ich2)
		{
		SortLw(&ich1, &ich2);
		if (_cch > ich2)
			BltPb(_rgch + ich2, _rgch + ich1, _cch - ich2);
		_cch -= (ich2 - ich1);
		_ichAnchor = _ichOther = ich1;
		}

	cchIns = LwBound(cchIns, 0, kcchMaxEdsl - ich1);
	if (0 < cchIns)
		{
		if (_cch > kcchMaxEdsl - cchIns)
			_cch = kcchMaxEdsl - cchIns;
		if (_cch > ich1)
			BltPb(_rgch + ich1, _rgch + ich1 + cchIns, _cch - ich1);
		CopyPb(prgch, _rgch + ich1, cchIns);
		_cch += cchIns;
		_ichAnchor = _ichOther = ich1 + cchIns;
		}

	_UpdateLn(0, 1, 1, _dypLine, gin);
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	If this is a character we can't accept, return false.
***************************************************************************/
bool EDSL::_FFilterCh(achar ch)
{
	return !(fchControl & GrfchFromCh(ch));
}


/***************************************************************************
	Get the text in the edit control.
***************************************************************************/
void EDSL::GetStn(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	pstn->SetRgch(_rgch, _cch);
}


/***************************************************************************
	Set the text in the edit control.  Sets the selection to an insertion
	point at the end of the text.
***************************************************************************/
void EDSL::SetStn(PSTN pstn, long gin)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	FReplace(pstn->Prgch(), pstn->Cch(), 0, IchMac(), gin);
}


/***************************************************************************
	Get some text.
***************************************************************************/
long EDSL::CchFetch(achar *prgch, long ich, long cchWant)
{
	AssertThis(0);
	AssertIn(cchWant, 0, kcbMax);
	AssertIn(ich, 0, IchMac() + 1);
	AssertPvCb(prgch, cchWant * size(achar));

	if (0 < (cchWant = LwBound(cchWant, 0, IchMac() + 1 - ich)))
		CopyPb(_rgch + ich, prgch, cchWant);
	return cchWant;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a single-line edit control.
***************************************************************************/
void EDSL::AssertValid(ulong grf)
{
	long ich;

	EDSL_PAR::AssertValid(0);
	AssertIn(_cch, 0, kcchMaxEdsl + 1);
	for (ich = _cch; ich-- > 0; )
		{
		Assert(_rgch[ich] != 0, "null character in EDSL");
		}
	AssertIn(_ichAnchor, 0, _cch + 1);
	AssertIn(_ichOther, 0, _cch + 1);
}
#endif //DEBUG


/***************************************************************************
	Constructor for multi line edit control.
***************************************************************************/
EDML::EDML(PEDPAR pedpar) : EDPL(pedpar)
{
	_bsm.SetMinGrow(256);
}


EDML::~EDML(void)
{
	ReleasePpo(&_pglich);
}


/***************************************************************************
	Create a new EDML (multi line edit control).
***************************************************************************/
PEDML EDML::PedmlNew(PEDPAR pedpar)
{
	PEDML pedml;

	if (pvNil == (pedml = NewObj EDML(pedpar)))
		return pvNil;
	if (!pedml->_FInit())
		{
		ReleasePpo(&pedml);
		return pvNil;
		}
	AssertPo(pedml, 0);
	return pedml;
}


/***************************************************************************
	Initialize the multi line edit control.
***************************************************************************/
bool EDML::_FInit(void)
{
	long ich;

	if (pvNil == (_pglich = GL::PglNew(size(long))))
		return fFalse;
	_pglich->SetMinGrow(20);
	ich = 0;
	return _pglich->FPush(&ich);
}


/***************************************************************************
	Get a pointer to the characters for the given line.
***************************************************************************/
bool EDML::_FLockLn(long ln, achar **pprgch, long *pcch)
{
	AssertThis(0);
	AssertIn(ln, 0, _pglich->IvMac() + 1);
	AssertVarMem(pprgch);
	AssertVarMem(pcch);
	long ich;

	if (ln >= _pglich->IvMac())
		{
		*pprgch = pvNil;
		*pcch = 0;
		return fFalse;
		}

	_pglich->Get(ln, &ich);
	if (ln + 1 == _pglich->IvMac())
		*pcch = IchMac() - ich;
	else
		{
		long ichT;

		_pglich->Get(ln + 1, pcch);
		ichT = LwMax(ich, _IchPrev(*pcch));
		if (ichT < *pcch &&
				((fchControl | fchMayBreak) & GrfchFromCh(_ChFetch(ichT))))
			{
			//don't include the break character(s)
			*pcch = ichT;
			}
		*pcch -= ich;
		}
	*pprgch = (achar *)_bsm.PvLock(ich * size(achar));
	return fTrue;
}


/***************************************************************************
	Unlock a line.
***************************************************************************/
void EDML::_UnlockLn(long ln, achar *prgch)
{
	AssertThis(0);
	_bsm.Unlock();
}


/***************************************************************************
	Return the line that ich is on.
***************************************************************************/
long EDML::_LnFromIch(long ich)
{
	AssertThis(fobjAssertFull);
	AssertIn(ich, 0, IchMac() + 1);
	long lnMin, lnLim, ln;
	long ichT;

	if (ich > IchMac())
		{
		Bug("Why did this happen?");
		return _pglich->IvMac();
		}

	for (lnMin = 0, lnLim = _pglich->IvMac(); lnMin < lnLim; )
		{
		ln = (lnMin + lnLim) / 2;
		_pglich->Get(ln, &ichT);
		if (ich < ichT)
			lnLim = ln;
		else if (ich > ichT)
			lnMin = ln + 1;
		else
			return ln;
		}
	Assert(lnMin > 0, "bad lnMin");
	return lnMin - 1;
}


/***************************************************************************
	Return the first ich for the given line.
***************************************************************************/
long EDML::_IchMinLn(long ln)
{
	AssertThis(0);
	long ich;

	if (ln >= _pglich->IvMac())
		return IchMac() + 1;
	_pglich->Get(ln, &ich);
	return ich;
}


/***************************************************************************
	Return the number of characters.
***************************************************************************/
long EDML::IchMac(void)
{
	AssertBaseThis(0);
	Assert(_bsm.IbMac() % size(achar) == 0, "ibMac not divisible by size(achar)");
	return _bsm.IbMac() / size(achar);
}


/***************************************************************************
	Return the number of lines.
***************************************************************************/
long EDML::_LnMac(void)
{
	AssertThis(0);
	return _pglich->IvMac();
}


/***************************************************************************
	Replace the characters between ich1 and ich2 with those in (prgch, cchIns).
	Calls _UpdateLn() to clean up the display.
***************************************************************************/
bool EDML::FReplace(achar *prgch, long cchIns, long ich1, long ich2, long gin)
{
	AssertThis(fobjAssertFull);
	AssertIn(cchIns, 0, kcbMax);
	AssertPvCb(prgch, cchIns * size(achar));
	AssertIn(ich1, 0, IchMac() + 1);
	AssertIn(ich2, 0, IchMac() + 1);
	long lnMin, clnDel, clnDel2, clnIns, ln;
	long dich;
	long ypOld, ypNew;

	ich1 = LwBound(ich1, 0, dich = IchMac() + 1);
	ich2 = LwBound(ich2, 0, dich);
	if (ich1 == ich2 && cchIns == 0)
		return fTrue;

	SortLw(&ich1, &ich2);
	lnMin = _LnFromIch(ich1);
	clnDel = _LnFromIch(ich2) - lnMin;
	ypOld = _YpFromLn(lnMin);

	// estimate the number of inserted lines
	clnIns = _ClnEstimate(prgch, cchIns);
	if (clnIns > clnDel && !_pglich->FEnsureSpace(clnIns - clnDel))
		return fFalse;

	_SwitchSel(fFalse, gin);
	_tsSel = 0;
	if (!_FReplaceCore(prgch, cchIns, ich1, ich2 - ich1))
		return fFalse;

	// delete lines { lnMin + 1, ..., lnMin + clnDel }
	for (ln = lnMin + clnDel; ln > lnMin; ln--)
		_pglich->Delete(ln);

	// adjust the ich's further in the _pglich
	if (0 != (dich = cchIns - ich2 + ich1))
		{
		long *pich;
		long lnLim;

		pich = (long *)_pglich->QvGet(++ln);
		for (lnLim = _pglich->IvMac(); ln < lnLim; ln++, pich++)
			*pich += dich;
		}

	// update the selection
	_ichAnchor = _ichOther = ich1 + cchIns;

	// reformat the lines
	ln = _LnReformat(lnMin, &clnDel2, &clnIns);
	clnDel += clnDel2;

	// keep the text in the same position vertically
	ypNew = _YpFromLn(lnMin);
	_yp += ypOld - ypNew;

	_UpdateLn(ln, clnIns + 1, clnDel + 1, LwMul(_dypLine, clnDel + 1), gin);
	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	Do a replace operation just on the text.
***************************************************************************/
bool EDML::_FReplaceCore(achar *prgch, long cchIns, long ich, long cchDel)
{
	return _bsm.FReplace(prgch, cchIns, ich, cchDel);
}


/***************************************************************************
	Estimate the number of new lines (exact for a simple EDML).
***************************************************************************/
long EDML::_ClnEstimate(achar *prgch, long cch)
{
	long cln;
	long ich;

	cln = 0;
	for (ich = 0; ich < cch; ich++)
		{
		if (fchBreak & GrfchFromCh(prgch[ich]))
			cln++;
		}
	return cln;
}


/***************************************************************************
	Find new line starts starting at lnMin.
***************************************************************************/
long EDML::_LnReformat(long lnMin, long *pclnDel, long *pclnIns)
{
	long ln;
	long ichPrev;
	long ich = _IchMinLn(lnMin);
	long ichNext = _IchMinLn(lnMin + 1);

	ln = lnMin + 1;
	while ((ich = _IchNext(ichPrev = ich)) < ichNext && ichPrev < ich)
		{
		if (fchBreak & GrfchFromCh(_ChFetch(ichPrev)))
			{
			if (!_pglich->FInsert(ln, &ich))
				{
				Warn("format failed");
				break;
				}
			ln++;
			}
		}
	*pclnDel = 0;
	*pclnIns = ln - lnMin - 1;
	return lnMin;
}


/***************************************************************************
	If this is a character we can't accept, return false.
***************************************************************************/
bool EDML::_FFilterCh(achar ch)
{
	ulong grfch = GrfchFromCh(ch);

	return !(fchControl & grfch) || (fchBreak & grfch);
}


/***************************************************************************
	Get some text.
***************************************************************************/
long EDML::CchFetch(achar *prgch, long ich, long cchWant)
{
	AssertThis(0);
	AssertIn(cchWant, 0, kcbMax);
	AssertIn(ich, 0, IchMac() + 1);
	AssertPvCb(prgch, cchWant * size(achar));

	if (0 < (cchWant = LwBound(cchWant, 0, IchMac() + 1 - ich)))
		_bsm.FetchRgb(ich, cchWant, prgch);
	return cchWant;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a multi-line edit control.
***************************************************************************/
void EDML::AssertValid(ulong grf)
{
	EDML_PAR::AssertValid(0);
	AssertPo(_pglich, 0);
	AssertPo(&_bsm, 0);
	AssertIn(_pglich->IvMac(), 1, kcbMax);

	long ibMac = _bsm.IbMac();
	long ichMac = ibMac / size(achar);

	Assert(ibMac % size(achar) == 0, "ibMac not a divisible by size(achar)");
	AssertIn(_ichAnchor, 0, ichMac + 1);
	AssertIn(_ichOther, 0, ichMac + 1);

	if (grf & fobjAssertFull)
		{
		long ichPrev, ich, ln;

		ichPrev = ichMac + 1;
		for (ln = _pglich->IvMac(); ln-- > 0; )
			{
			_pglich->Get(ln, &ich);
			AssertIn(ich, 0, ichPrev);
			ichPrev = ich;
			}
		Assert(ichPrev == 0, "bad first ich");
		}
}


/***************************************************************************
	Mark memory for the EDML.
***************************************************************************/
void EDML::MarkMem(void)
{
	AssertValid(0);
	EDML_PAR::MarkMem();
	MarkMemObj(&_bsm);
	MarkMemObj(_pglich);
}
#endif //DEBUG


/***************************************************************************
	Constructor for multi line edit control.
***************************************************************************/
EDMW::EDMW(PEDPAR pedpar) : EDML(pedpar)
{
}


/***************************************************************************
	Create a new multi-line wrapping edit control.
***************************************************************************/
PEDMW EDMW::PedmwNew(PEDPAR pedpar)
{
	PEDMW pedmw;

	if (pvNil == (pedmw = NewObj EDMW(pedpar)))
		return pvNil;
	if (!pedmw->_FInit())
		{
		ReleasePpo(&pedmw);
		return pvNil;
		}
	AssertPo(pedmw, 0);
	return pedmw;
}


/***************************************************************************
	Return an estimate of how many new lines there are in the text to insert.
***************************************************************************/
long EDMW::_ClnEstimate(achar *prgch, long cch)
{
	//the common case
	if (cch <= 1)
		return 1;

	RC rc;
	long dxp, cln;

	_GetRcContent(&rc);
	rc.Inset(kdxpInsetEdcb, 0);
	dxp = LwMax(10, rc.Dxp());
	_pgnv->SetFont(_onn, _grfont, _dypFont);
	_pgnv->GetRcFromRgch(&rc, prgch, cch);
	cln = LwMin(cch, 2 * rc.Dxp() / dxp);

	return cln + EDML::_ClnEstimate(prgch, cch);
}


/***************************************************************************
	Determine the line starts from line lnMin on.  This may also affect
	lnMin's line start.  Returns the first line that changed (1 less than
	the first line start that changed).
***************************************************************************/
long EDMW::_LnReformat(long lnMin, long *pclnDel, long *pclnIns)
{
	const long kcichMax = 128;
	long rgich[kcichMax];
	RC rc;
	long dxp;
	long ich, cich, ivMin, ivLim, iv, ichNew;
	long iichCur;
	long lnCur;
	ulong grfch;
	achar *prgch;
	long clnIns = 0, clnDel = 0;

	_GetRcContent(&rc);
	rc.Inset(kdxpInsetEdcb, 0);
	dxp = rc.Dxp();
	_pgnv->SetFont(_onn, _grfont, _dypFont, _tah);
	ich = _IchMinLn(lnMin);
	Assert((ich > 0) == (lnMin > 0), "bad beginning");
	prgch = (achar *)_bsm.PvLock(0);
	cich = _CichGetBreakables(prgch, ich, rgich, kcichMax);
	Assert(cich > 0, "bad cich");

	if (lnMin > 0 && !(fchBreak & GrfchFromCh(prgch[_IchPrev(ich)])))
		{
		// see if some of the text will fit on the previous line
		long ichPrev = _IchMinLn(lnMin - 1);

		_pgnv->GetRcFromRgch(&rc, prgch + ichPrev, rgich[0] - ichPrev);
		if (rc.Dxp() <= dxp)
			{
			//need to reformat the previous line
			_pglich->Delete(lnMin);
			clnDel++;
			ich = ichPrev;
			lnMin--;
			cich = _CichGetBreakables(prgch, ich, rgich, kcichMax);
			}
		}

	lnCur = lnMin + 1;
	for (iichCur = 0; ; )
		{
		if (ich > rgich[cich - 1])
			{
			cich = _CichGetBreakables(prgch, ich, rgich, kcichMax);
			iichCur = 0;
			}
		while (cich == kcichMax)
			{
			_pgnv->GetRcFromRgch(&rc, prgch + ich, rgich[cich - 1] - ich);
			if (rc.Dxp() >= dxp)
				break;
			// keep the last one and refill
			rgich[0] = rgich[cich - 1];
			cich = _CichGetBreakables(prgch, rgich[0], rgich + 1, kcichMax - 1);
			iichCur = 0;
			}

		for (ivMin = iichCur, ivLim = cich; ivMin < ivLim; )
			{
			iv = (ivMin + ivLim) / 2;
			_pgnv->GetRcFromRgch(&rc, prgch + ich, rgich[iv] - ich);
			if (rc.Dxp() > dxp)
				ivLim = iv;
			else
				ivMin = iv + 1;
			}

		if (ivMin == iichCur)
			{
			// have to break a non-breaking stream - oh well
			for (ivMin = ich, ivLim = rgich[iichCur]; ivMin < ivLim; )
				{
				iv = (ivMin + ivLim) / 2;
				_pgnv->GetRcFromRgch(&rc, prgch + ich, iv - ich);
				if (rc.Dxp() > dxp)
					ivLim = iv;
				else
					ivMin = iv + 1;
				}
			Assert(ivMin >= ich, "bad ivMin");
			ichNew = _IchPrev(ivMin);
			}
		else
			{
			// we found our break location
			Assert(rgich[ivMin - 1] >= ich, "bad entry in rgich");
			ichNew = rgich[ivMin - 1];
			iichCur = ivMin;
			}

		if (ichNew <= ich)
			ichNew = _IchNext(ich);

		if (ichNew >= IchMac())
			{
			// we've run out of text - delete all remaining lines
			clnDel += _pglich->IvMac() - lnCur;
			AssertDo(_pglich->FSetIvMac(lnCur), 0);
			break; // we're done
			}

		grfch = GrfchFromCh(_ChFetch(ichNew));
		if ((fchBreak | fchMayBreak) & grfch)
			ichNew = _IchNext(ichNew);
		ich = ichNew;

		while (lnCur < _pglich->IvMac())
			{
			_pglich->Get(lnCur, &iv);
			if (ich < iv)
				break;
			if (ich == iv)
				goto LDone; //we're done
			// delete this line start
			_pglich->Delete(lnCur);
			clnDel++;
			}

		if (!_pglich->FInsert(lnCur, &ich))
			{
			Warn("memory failure in EDMW::_LnReformat");
			break;
			}
		clnIns++;
		lnCur++;
		}

LDone:
	*pclnDel = clnDel;
	*pclnIns = clnIns;
	_bsm.Unlock();
	return lnMin;
}


/***************************************************************************
	Get the locations of possible breaking characters from prgch + ich.
	Doesn't continue past a return character.  Return the number of locations
	found.
***************************************************************************/
long EDMW::_CichGetBreakables(achar *prgch, long ich, long *prgich, long cichMax)
{
	AssertIn(ich, 0, IchMac() + 1);
	AssertPvCb(prgch, IchMac() * size(achar));
	AssertPvCb(prgich, LwMul(cichMax, size(long)));
	long cich;
	ulong grfch;

	for (cich = 0; ich < IchMac(); ich++)
		{
		grfch = GrfchFromCh(prgch[ich]);
		if (grfch & fchBreak)
			{
			prgich[cich++] = ich;
			return cich;
			}
		if (grfch & fchMayBreak)
			{
			prgich[cich++] = ich;
			if (cich >= cichMax)
				return cich;
			}
		}

	//add the end of stream loaction
	Assert(cich < cichMax, "what?");
	prgich[cich++] = IchMac();
	return cich;
}


/***************************************************************************
	The size of the GOB changed - relayout.
***************************************************************************/
void EDMW::_NewRc(void)
{
	AssertThis(0);
	long clnDel, clnIns;

	AssertDo(_pglich->FSetIvMac(1), 0);
	_LnReformat(0, &clnDel, &clnIns);
	ShowSel(fTrue, ginNil);
}


