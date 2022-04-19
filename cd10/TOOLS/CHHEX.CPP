/***************************************************************************

	Classes for the hex editor.

***************************************************************************/
#include "ched.h"
ASSERTNAME


#define kcbMaxLineDch 16

/***************************************************************************
	A document class that holds a stream and is naturally displayed by the
	hex editor (DCH).  Used for the clipboard.
***************************************************************************/
typedef class DHEX *PDHEX;
#define DHEX_PAR DOCB
#define kclsDHEX 'DHEX'
class DHEX : public DHEX_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	BSF _bsf;

	DHEX(PDOCB pdocb = pvNil, ulong grfdoc = fdocNil)
		: DHEX_PAR(pdocb, grfdoc) {}

public:
	static PDHEX PdhexNew(void);

	PBSF Pbsf(void)
		{ return &_bsf; }

	virtual PDDG PddgNew(PGCB pgcb);
	};


RTCLASS(DCH)
RTCLASS(DOCH)
RTCLASS(DHEX)


/***************************************************************************
	Static method to create a new text stream document to be displayed
	by the hex editor.
***************************************************************************/
PDHEX DHEX::PdhexNew(void)
{
	PDHEX pdhex;

	if (pvNil == (pdhex = NewObj DHEX()))
		return pvNil;

	return pdhex;
}


/***************************************************************************
	Create a new DCH displaying this stream.
***************************************************************************/
PDDG DHEX::PddgNew(PGCB pgcb)
{
	return DCH::PdchNew(this, &_bsf, fFalse, pgcb);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a DHEX.
***************************************************************************/
void DHEX::AssertValid(ulong grf)
{
	DHEX_PAR::AssertValid(0);
	AssertPo(&_bsf, 0);
}


/***************************************************************************
	Mark memory for the DHEX.
***************************************************************************/
void DHEX::MarkMem(void)
{
	AssertValid(0);
	DHEX_PAR::MarkMem();
	MarkMemObj(&_bsf);
}
#endif //DEBUG


/***************************************************************************
	Constructor for the DCH.
***************************************************************************/
DCH::DCH(PDOCB pdocb, PBSF pbsf, bool fFixed, PGCB pgcb) : DCLB(pdocb, pgcb)
{
	_pbsf = pbsf;
	_cbLine = kcbMaxLineDch;
	_dypHeader = _dypLine + 1;
	_fFixed = FPure(fFixed);
}


/***************************************************************************
	Static method to create a new DCH.
***************************************************************************/
PDCH DCH::PdchNew(PDOCB pdocb, PBSF pbsf, bool fFixed, PGCB pgcb)
{
	PDCH pdch;

	if (pvNil == (pdch = NewObj DCH(pdocb, pbsf, fFixed, pgcb)))
		return pvNil;

	if (!pdch->_FInit())
		{
		ReleasePpo(&pdch);
		return pvNil;
		}
	pdch->Activate(fTrue);

	AssertPo(pdch, 0);
	return pdch;
}


/***************************************************************************
	We're being activated or deactivated, invert the sel.
***************************************************************************/
void DCH::_Activate(bool fActive)
{
	AssertThis(0);
	RC rc;

	DDG::_Activate(fActive);
	GetRc(&rc, cooLocal);
	rc.ypBottom = _dypHeader;
	InvalRc(&rc);
	_SwitchSel(fActive);
}


/***************************************************************************
	Draw the Hex doc in the port.
***************************************************************************/
void DCH::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	STN stn;
	byte rgb[kcbMaxLineDch];
	RC rc, rcSrc;
	long xp, yp, cb, ib, cbT, ibT;
	byte bT;

	pgnv->ClipRc(prcClip);
	pgnv->GetRcSrc(&rcSrc);
	pgnv->SetOnn(_onn);

	if (prcClip->ypTop < _dypHeader)
		_DrawHeader(pgnv);

	Assert(_cbLine <= size(rgb), "lines too long");
	cb = _pbsf->IbMac();
	xp = _XpFromIch(0);
	ib = LwMul(_cbLine, _LnFromYp(LwMax(_dypHeader, prcClip->ypTop)));
	yp = _YpFromIb(ib);
	rc.xpLeft = _XpFromCb(_cbLine, fFalse);
	rc.xpRight = prcClip->xpRight;
	rc.ypTop = yp;
	rc.ypBottom = prcClip->ypBottom;
	pgnv->FillRc(&rc, kacrWhite);
	if (xp > 0)
		{
		//erase to the left of the text
		rc.xpLeft = 0;
		rc.xpRight = xp;
		pgnv->FillRc(&rc, kacrWhite);
		}

	for ( ; ib < cb && yp < prcClip->ypBottom; ib += _cbLine)
		{
		cbT = LwMin(_cbLine, cb - ib);
		_pbsf->FetchRgb(ib, cbT, rgb);

		//first comes the address of the first byte of the line
		stn.FFormatSz(PszLit("%08x "), ib);

		//now add the line's bytes in hex, with a space after every
		//four bytes
		for (ibT = 0; ibT < cbT; ibT++)
			{
			bT = rgb[ibT];
			if ((ibT & 0x03) == 0)
				stn.FAppendCh(kchSpace);
			stn.FAppendCh(vrgchHex[(bT >> 4) & 0x0F]);
			stn.FAppendCh(vrgchHex[bT & 0x0F]);
			}
		//pad the line with spaces
		if (ibT < _cbLine)
			{
			ibT = _cbLine - ibT;
			ibT = 2 * ibT + ibT / 4;
			while (ibT-- > 0)
				stn.FAppendCh(kchSpace);
			}
		stn.FAppendSz(PszLit("  "));

		//now comes the ascii characters.
		for (ibT = 0; ibT < cbT; ibT++)
			{
			bT = rgb[ibT];
			if (bT < 32 || bT == 0x7F)
				bT = '?';
			stn.FAppendCh((achar)bT);
			}
		//pad the line with spaces
		while (ibT++ < _cbLine)
			stn.FAppendCh(kchSpace);

		pgnv->DrawStn(&stn, xp, yp, kacrBlack, kacrWhite);
		yp += _dypLine;
		}
	if (yp < prcClip->ypBottom)
		{
		rc = rcSrc;
		rc.ypTop = yp;
		pgnv->FillRc(&rc, kacrWhite);
		}

	//draw the selection
	if (_fSelOn)
		_InvertSel(pgnv);
}


/***************************************************************************
	Draw the header for the DCH.
***************************************************************************/
void DCH::_DrawHeader(PGNV pgnv)
{
	STN stn;
	RC rc, rcSrc;

	pgnv->SetOnn(_onn);
	pgnv->GetRcSrc(&rcSrc);

	//erase the first part of the line
	rc.xpLeft = 0;
	rc.xpRight = _XpFromIch(0);
	rc.ypTop = 0;
	rc.ypBottom = _dypLine;
	pgnv->FillRc(&rc, kacrWhite);

	//draw the text
	stn.FFormatSz(PszLit("%08x"), _pbsf->IbMac());
	pgnv->DrawStn(&stn, rc.xpRight, 0, kacrBlack, kacrWhite);

	//erase the rest of the line
	rc.xpLeft = _XpFromIch(8);
	rc.xpRight = rcSrc.xpRight;
	pgnv->FillRc(&rc, kacrWhite);

	//draw the _fHex Marker
	rc.xpLeft = _XpFromCb(0, _fHexSel);
	rc.xpRight = rc.xpLeft + rc.Dyp();
	rc.Inset(rc.Dyp() / 6, rc.Dyp() / 6);
	if (_fActive)
		pgnv->FillRc(&rc, kacrBlack);
	else
		pgnv->FillRcApt(&rc, &vaptGray, kacrBlack, kacrWhite);

	//draw the line seperating the header from the data
	rc = rcSrc;
	rc.ypTop = _dypHeader - 1;
	rc.ypBottom = _dypHeader;
	pgnv->FillRc(&rc, kacrBlack);
}


/***************************************************************************
	Handle key input.
***************************************************************************/
bool DCH::FCmdKey(PCMD_KEY pcmd)
{
	AssertThis(0);
	ulong grfcust;
	long dibSel, dibDel, ibLim;
	long cact;
	CMD cmd;
	byte rgb[64], bT;
	bool fRight = fFalse;

	// keep fetching characters until we get a cursor key, delete key or
	// until the buffer is full.
	dibSel = 0;
	dibDel = 0;
	ibLim = 0;
	do
		{
		switch (pcmd->vk)
			{
		case kvkHome:
			dibSel = -_pbsf->IbMac() - ibLim - 1;
			break;
		case kvkEnd:
			dibSel = _pbsf->IbMac() + ibLim + 1;
			break;
		case kvkLeft:
			dibSel = -1;
			break;
		case kvkRight:
			dibSel = 1;
			fRight = fTrue;
			break;
		case kvkUp:
			dibSel = -_cbLine;
			fRight = _fRightSel;
			break;
		case kvkDown:
			dibSel = _cbLine;
			fRight = _fRightSel;
			break;

		case kvkDelete:
			if (!_fFixed)
				dibDel = 1;
			break;
		case kvkBack:
			if (!_fFixed)
				dibDel = -1;
			break;

		default:
			if (chNil == pcmd->ch)
				break;
			if (!_fHexSel)
				bT = (byte)pcmd->ch;
			else
				{
				//hex typing
				if (FIn(pcmd->ch, '0', '9' + 1))
					bT = pcmd->ch - '0';
				else if (FIn(pcmd->ch, 'A', 'F' + 1))
					bT = pcmd->ch - 'A' + 10;
				else if (FIn(pcmd->ch, 'a', 'f' + 1))
					bT = pcmd->ch - 'a' + 10;
				else
					break;
				}
			for (cact = 0; cact < pcmd->cact && ibLim < size(rgb); cact++)
				rgb[ibLim++] = bT;
			break;
			}

		grfcust = pcmd->grfcust;
		pcmd = (PCMD_KEY)&cmd;
		}
	while (0 == dibSel && 0 == dibDel && ibLim < size(rgb) && vpcex->FGetNextKey(&cmd));

	if (ibLim > 0)
		{
		//have some characters to insert
		if (!_fHexSel)
			{
			//just straight characters to insert
			_FReplace(rgb, ibLim, _ibAnchor, _ibOther);
			}
		else
			{
			//hex typing
			byte bT;
			long ibSrc, ibDst;
			long ibAnchor = _ibAnchor;

			if (_fHalfSel && ibAnchor > 0)
				{
				//complete the byte
				_pbsf->FetchRgb(--ibAnchor, 1, &bT);
				rgb[0] = (bT & 0xF0) | (rgb[0] & 0x0F);
				ibSrc = 1;
				}
			else
				ibSrc = 0;

			for (ibDst = ibSrc; ibSrc + 1 < ibLim; ibSrc += 2)
				rgb[ibDst++] = (rgb[ibSrc] << 4) | (rgb[ibSrc + 1] & 0x0F);

			if (ibSrc < ibLim)
				{
				Assert(ibSrc + 1 == ibLim, 0);
				rgb[ibDst++] = rgb[ibSrc] << 4;
				}

			_FReplace(rgb, ibDst, ibAnchor, _ibOther, ibSrc < ibLim);
			}
		}

	if (dibSel != 0)
		{
		//move the selection
		if (grfcust & fcustShift)
			{
			//extend selection
			_SetSel(_ibAnchor, _ibOther + dibSel, fRight);
			_ShowSel();
			}
		else
			{
			long ibOther = _ibOther;
			long ibAnchor = _ibAnchor;

			if (_fHalfSel)
				{
				if (dibSel < 0)
					{
					ibAnchor--;
					fRight = fFalse;
					}
				else
					fRight = fTrue;
				}
			else if (ibAnchor == ibOther)
				ibAnchor += dibSel;
			else if ((dibSel > 0) != (ibAnchor > ibOther))
				{
				ibAnchor = ibOther;
				fRight = dibSel > 0;
				}
			_SetSel(ibAnchor, ibAnchor, fRight);
			_ShowSel();
			}
		}
	else if (dibDel != 0)
		{
		if (_ibAnchor != _ibOther)
			dibDel = _ibOther - _ibAnchor;
		else
			dibDel = LwBound(_ibAnchor + dibDel, 0, _pbsf->IbMac() + 1) - _ibAnchor;
		if (dibDel != 0)
			_FReplace(pvNil, 0, _ibAnchor, _ibAnchor + dibDel);
		}

	return fTrue;
}


/***************************************************************************
	Replaces the bytes between ib1 and ib2 with the given bytes.
***************************************************************************/
bool DCH::_FReplace(byte *prgb, long cb, long ib1, long ib2, bool fHalfSel)
{
	_SwitchSel(fFalse);
	SortLw(&ib1, &ib2);
	if (_fFixed)
		{
		cb = LwMin(cb, _pbsf->IbMac() - ib1);
		ib2 = ib1 + cb;
		}
	if (!_pbsf->FReplace(prgb, cb, ib1, ib2 - ib1))
		return fFalse;

	_InvalAllDch(ib1, cb, ib2 - ib1);
	ib1 += cb;
	if (fHalfSel)
		_SetHalfSel(ib1);
	else
		_SetSel(ib1, ib1, fFalse /*REVIEW shonk*/);
	_ShowSel();
	return fTrue;
}


/***************************************************************************
	Invalidate all DCHs on this byte stream.  Also dirties the document.
	Should be called by any code that edits the document.
***************************************************************************/
void DCH::_InvalAllDch(long ib, long cbIns, long cbDel)
{
	AssertThis(0);
	long ipddg;
	PDDG pddg;

	//mark the document dirty
	_pdocb->SetDirty();

	//inform the DCDs
	for (ipddg = 0; pvNil != (pddg = _pdocb->PddgGet(ipddg)); ipddg++)
		{
		if (pddg->FIs(kclsDCH))
            ((PDCH)pddg)->_InvalIb(ib, cbIns, cbDel);
		}
}


/***************************************************************************
	Invalidate the display from ib to the end of the display.  If we're
	the active DCH, also redraw.
***************************************************************************/
void DCH::_InvalIb(long ib, long cbIns, long cbDel)
{
	AssertThis(0);
	Assert(!_fSelOn, "why is the sel on during an invalidation?");
	RC rc;
	long ibAnchor, ibOther;

	//adjust the sel
	ibAnchor = _ibAnchor;
	ibOther = _ibOther;
	FAdjustIv(&ibAnchor, ib, cbIns, cbDel);
	FAdjustIv(&ibOther, ib, cbIns, cbDel);
	if (ibAnchor != _ibAnchor || ibOther != _ibOther)
		_SetSel(ibAnchor, ibOther, _fRightSel);

	//caclculate the invalid rectangle
	GetRc(&rc, cooLocal);
	rc.ypTop = _YpFromIb(ib);
	if (cbIns == cbDel)
		rc.ypBottom = rc.ypTop + _dypLine;

	if (rc.FEmpty())
		return;

	if (_fActive)
		{
		ValidRc(&rc, kginDraw);
		InvalRc(&rc, kginDraw);
		}
	else
		InvalRc(&rc);

	if (cbIns != cbDel)
		{
		//invalidate the length
		GetRc(&rc, cooLocal);
		rc.xpLeft = _XpFromIch(0);
		rc.xpRight = _XpFromIch(8);
		rc.ypTop = 0;
		rc.ypBottom = _dypHeader;
		InvalRc(&rc);
		}
}


/***************************************************************************
	Turn the selection on or off.
***************************************************************************/
void DCH::_SwitchSel(bool fOn)
{
	if (FPure(fOn) != FPure(_fSelOn))
		{
		GNV gnv(this);
		_InvertSel(&gnv);
		_fSelOn = FPure(fOn);
		}
}


/***************************************************************************
	Make sure the ibOther of the selection is visible.  If possible, show
	both ends of the selection.
***************************************************************************/
void DCH::_ShowSel(void)
{
	long ln, lnHope, cln, dscv;
	RC rc;

	//find the line we definitely need to show
	ln = _ibOther / _cbLine;
	if (_ibOther % _cbLine == 0 && ln > 0)
		{
		//may have to adjust ln down by one
		if (_ibAnchor < _ibOther || _ibAnchor == _ibOther && _fRightSel)
			ln--;
		}

	//find the other end of the selection - which we hope to be able to show
	lnHope = _ibAnchor / _cbLine;

	_GetContent(&rc);
	cln = LwMax(1, rc.Dyp() / _dypLine);
	if (LwAbs(ln - lnHope) >= cln)
		lnHope = ln; //can't show both

	if (FIn(ln, _scvVert, _scvVert + cln) &&
			FIn(lnHope, _scvVert, _scvVert + cln))
		{
		//both are showing
		return;
		}

	Assert(LwAbs(lnHope - ln) < cln, "ln and lnHope too far apart");
	SortLw(&ln, &lnHope);
	if (ln < _scvVert)
		dscv = ln - _scvVert;
	else
		{
		dscv = lnHope - _scvVert - cln + 1;
		Assert(dscv > 0, "bad dscv (bad logic above)");
		}
	_Scroll(scaNil, scaToVal, 0, _scvVert + dscv);
}


/***************************************************************************
	Invert the selection.  Doesn't touch _fSelOn.
***************************************************************************/
void DCH::_InvertSel(PGNV pgnv)
{
	Assert(!_fFixed || _ibAnchor == _ibOther, "non-ins sel in fixed");
	long cb;
	RC rcClip;
	RC rc, rcT;

	_GetContent(&rcClip);
	if (_fFixed && _pbsf->IbMac() > 0 && !_fHalfSel)
		{
		rc.xpLeft = _XpFromIb(_ibAnchor, fTrue);
		rc.xpRight = rc.xpLeft + 2 * _dxpChar;
		rc.ypTop = _YpFromIb(_ibAnchor);
		rc.ypBottom = rc.ypTop + _dypLine;
		if (rcT.FIntersect(&rc, &rcClip))
			pgnv->HiliteRc(&rcT, kacrWhite);

		rc.xpLeft = _XpFromIb(_ibAnchor, fFalse);
		rc.xpRight = rc.xpLeft + _dxpChar;
		if (rcT.FIntersect(&rc, &rcClip))
			pgnv->HiliteRc(&rcT, kacrWhite);
		}
	else if (_ibAnchor == _ibOther)
		{
		//insertion or half sel
		Assert(!_fHalfSel || _fRightSel, "_fHalfSel set but not _fRightSel");
		cb = _ibAnchor % _cbLine;
		if (_fRightSel && cb == 0 && _ibAnchor > 0)
			{
			rc.ypTop = _YpFromIb(_ibAnchor - 1);
			cb = _cbLine;
			}
		else
			rc.ypTop = _YpFromIb(_ibAnchor);
		rc.ypBottom = rc.ypTop + _dypLine;

		//do the hex sel
		rc.xpLeft = _XpFromCb(cb, fTrue, _fRightSel);
		if (_fHalfSel && _ibAnchor > 0)
			{
			rc.xpRight = rc.xpLeft;
			rc.xpLeft -= _dxpChar;
			if (rcT.FIntersect(&rc, &rcClip))
				pgnv->HiliteRc(&rcT, kacrWhite);
			}
		else
			{
			rc.xpRight = --rc.xpLeft + 2;
			if (rcT.FIntersect(&rc, &rcClip))
				pgnv->FillRc(&rcT, kacrInvert);
			}

		//do the ascii sel
		rc.xpLeft = _XpFromCb(cb, fFalse) - 1;
		rc.xpRight = rc.xpLeft + 2;
		if (rcT.FIntersect(&rc, &rcClip))
			pgnv->FillRc(&rcT, kacrInvert);
		}
	else
		{
		_InvertIbRange(pgnv, _ibAnchor, _ibOther, fTrue);
		_InvertIbRange(pgnv, _ibAnchor, _ibOther, fFalse);
		}
}


/***************************************************************************
	Inverts a range on screen.  Does not mark insertion bars or half sels.
***************************************************************************/
void DCH::_InvertIbRange(PGNV pgnv, long ib1, long ib2, bool fHex)
{
	long ibMin, ibMac;
	long xp2, yp2;
	RC rc, rcT, rcClip;

	ibMin = _scvVert * _cbLine;
	ibMac = _pbsf->IbMac();
	ib1 = LwBound(ib1, ibMin, ibMac + 1);
	ib2 = LwBound(ib2, ibMin, ibMac + 1);

	if (ib1 == ib2)
		return;
	SortLw(&ib1, &ib2);

	_GetContent(&rcClip);
	rc.xpLeft = _XpFromIb(ib1, fHex);
	rc.ypTop = _YpFromIb(ib1);
	xp2 = _XpFromIb(ib2, fHex);
	yp2 = _YpFromIb(ib2);

	rc.ypBottom = rc.ypTop + _dypLine;
	if (yp2 == rc.ypTop)
		{
		//only one line involved
		rc.xpRight = xp2;
		if (rcT.FIntersect(&rc, &rcClip))
			pgnv->HiliteRc(&rcT, kacrWhite);
		return;
		}

	//invert the sel on the first line
	rc.xpRight = _XpFromCb(_cbLine, fHex);
	if (rcT.FIntersect(&rc, &rcClip))
		pgnv->HiliteRc(&rcT, kacrWhite);

	//invert the main rectangular block
	rc.xpLeft = _XpFromCb(0, fHex);
	rc.ypTop += _dypLine;
	rc.ypBottom = yp2;
	if (rcT.FIntersect(&rc, &rcClip))
		pgnv->HiliteRc(&rcT, kacrWhite);

	//invert the last line
	rc.ypTop = yp2;
	rc.ypBottom = yp2 + _dypLine;
	rc.xpRight = xp2;
	if (rcT.FIntersect(&rc, &rcClip))
		pgnv->HiliteRc(&rcT, kacrWhite);
}


/***************************************************************************
	Select the second half of the byte before ib.
***************************************************************************/
void DCH::_SetHalfSel(long ib)
{
	ib = LwBound(ib, 0, _pbsf->IbMac() + 1);
	if (ib == 0)
		{
		_SetSel(ib, ib, fFalse);
		return;
		}

	GNV gnv(this);
	if (_fSelOn)
		{
		//turn off the sel
		_InvertSel(&gnv);
		_fSelOn = fFalse;
		}
	_ibAnchor = _ibOther = ib;
	_fHalfSel = fTrue;
	_fRightSel = fTrue;
	if (_fActive)
		{
		_InvertSel(&gnv);
		_fSelOn = fTrue;
		}
}


/***************************************************************************
	Set the selection.  fRight is ignored for non-insertion bar selections.
***************************************************************************/
void DCH::_SetSel(long ibAnchor, long ibOther, bool fRight)
{
	long ibMac = _pbsf->IbMac();
	GNV gnv(this);

	if (_fFixed && ibMac > 0)
		{
		ibOther = ibAnchor = LwBound(ibOther, 0, ibMac);
		fRight = fFalse;
		}
	else
		{
		ibAnchor = LwBound(ibAnchor, 0, ibMac + 1);
		ibOther = LwBound(ibOther, 0, ibMac + 1);
		if (ibAnchor == ibOther)
			{
			if (fRight && ibAnchor == 0)
				fRight = fFalse;
			else if (!fRight && ibAnchor == ibMac)
				fRight = fTrue;
			}
		else
			fRight = fFalse;
		}

	if (!_fHalfSel && ibAnchor == _ibAnchor && ibOther == _ibOther &&
			FPure(fRight) == FPure(_fRightSel))
		{
		goto LDrawSel;
		}

	if (_fSelOn)
		{
		if (_ibAnchor != ibAnchor || _ibAnchor == _ibOther ||
				ibAnchor == ibOther)
			{
			_InvertSel(&gnv);
			_fSelOn = fFalse;
			}
		else
			{
			//they have the same anchor and neither is an insertion
			_InvertIbRange(&gnv, _ibOther, ibOther, fTrue);
			_InvertIbRange(&gnv, _ibOther, ibOther, fFalse);
			}
		}

	_ibAnchor = ibAnchor;
	_ibOther = ibOther;
	_fRightSel = FPure(fRight);
	_fHalfSel = fFalse;

LDrawSel:
	if (!_fSelOn && _fActive)
		{
		_InvertSel(&gnv);
		_fSelOn = fTrue;
		}
}


/***************************************************************************
	Changes the selection type from hex to ascii or vice versa.
***************************************************************************/
void DCH::_SetHexSel(bool fHex)
{
	if (FPure(fHex) == FPure(_fHexSel))
		return;
	_fHexSel = FPure(fHex);

	GNV gnv(this);
	_DrawHeader(&gnv);
}


/***************************************************************************
	Find the column for the given horizontal byte position.  cb is the number
	of bytes in from the left edge.  fHex indicates whether we want the
	position in the hex area or the ascii area.  fNoTrailSpace is ignored if
	fHex is false.  If fHex is true, fNoTrailSpace indicates whether a
	trailing space should be included (if the cb is divisible by 4).
***************************************************************************/
long DCH::_IchFromCb(long cb, bool fHex, bool fNoTrailSpace)
{
	AssertIn(cb, 0, _cbLine + 1);

	//skip over the address
	long ich = 10;

	if (fHex)
		{
		//account for the spaces every four hex digits
		ich += 2 * cb + cb / 4;
		if (fNoTrailSpace && (cb % 4) == 0 && cb > 0)
			ich--;
		}
	else
		{
		//skip over the hex area
		ich += 2 * _cbLine + _cbLine / 4 + 1 + cb;
		}
	return ich;
}


/***************************************************************************
	Find the xp for the given byte.  fHex indicates whether we want the
	postion in the hex area or the ascii area.
***************************************************************************/
long DCH::_XpFromIb(long ib, bool fHex)
{
	return _XpFromIch(_IchFromCb(ib % _cbLine, fHex));
}


/***************************************************************************
	Find the xp for the given horizontal byte position.  cb is the number
	of bytes in from the left edge.  fHex indicates whether we want the
    postion in the hex area or the ascii area.
***************************************************************************/
long DCH::_XpFromCb(long cb, bool fHex, bool fNoTrailSpace)
{
	return _XpFromIch(_IchFromCb(cb, fHex, fNoTrailSpace));
}


/***************************************************************************
	Find the yp for the given byte.
***************************************************************************/
long DCH::_YpFromIb(long ib)
{
	AssertIn(ib, 0, kcbMax);
	return LwMul((ib / _cbLine) - _scvVert, _dypLine) + _dypHeader;
}


/***************************************************************************
	Finds the byte that the given point is over.  *ptHex is both input and
	output.  If *ptHex is tMaybe on input, it will be set to tYes or tNo on
	output (unless the point is not in the edit area of the DCH).  If *ptHex
	is tYes or tNo on input, the ib is determined using *ptHex.
***************************************************************************/
long DCH::_IbFromPt(long xp, long yp, bool *ptHex, bool *pfRight)
{
	AssertVarMem(ptHex);
	AssertNilOrVarMem(pfRight);

	RC rc;
	long cbMin, cbLim, cb, ib;
	long xpFind, xpT;
	bool fHex;

	_GetContent(&rc);
	if (!rc.FPtIn(xp, yp))
		return ivNil;

	if (*ptHex == tMaybe)
		{
		xpT = (_XpFromCb(_cbLine, fTrue, fTrue)  + _XpFromCb(0, fFalse)) / 2;
		if (xp <= xpT)
			*ptHex = tYes;
		else
			*ptHex = tNo;
		}

	xpFind = xp - _dxpChar;
	if (*ptHex == tYes)
		{
		if (_fFixed)
			xpFind -= _dxpChar;
		fHex = fTrue;
		}
	else
		{
		if (!_fFixed)
			xpFind = xp - _dxpChar / 2;
		fHex = fFalse;
		}

	for (cbMin = 0, cbLim = _cbLine; cbMin < cbLim; )
		{
		cb = (cbMin + cbLim) / 2;
		xpT = _XpFromCb(cb, fHex);
		if (xpT < xpFind)
			cbMin = cb + 1;
		else
			cbLim = cb;
		}
	if (_fFixed && cbMin == _cbLine)
		cbMin--;
	ib = cbMin + _cbLine * _LnFromYp(yp);
	ib = LwMin(ib, _pbsf->IbMac());
	if (pvNil != pfRight)
		*pfRight = cbMin == _cbLine;
	return ib;
}


/***************************************************************************
	Handle a mouse down in our content.
***************************************************************************/
void DCH::MouseDown(long xp, long yp, long cact, ulong grfcust)
{
	AssertThis(0);
	bool tHex;
	bool fDown, fRight;
	PT pt, ptT;
	long ib;
	RC rc;

	//doing this before the activate avoids flashing the old selection
	tHex = tMaybe;
	ib = _IbFromPt(xp, yp, &tHex, &fRight);
	if (ivNil != ib)
		_SetSel((grfcust & fcustShift) ? _ibAnchor : ib, ib, fRight);

	if (!_fActive)
		Activate(fTrue);

	if (ivNil == ib)
		return;

	_SetHexSel(tYes == tHex);

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

		ib = _IbFromPt(pt.xp, pt.yp, &tHex, &fRight);
		if (ivNil != ib)
			_SetSel(_ibAnchor, ib, fRight);
		}
}


/***************************************************************************
	Return the maximum for the indicated scroll bar.
***************************************************************************/
long DCH::_ScvMax(bool fVert)
{
	RC rc;

	_GetContent(&rc);
	return LwMax(0, fVert ?
		(_pbsf->IbMac() + _cbLine - 1) / _cbLine + 1 - rc.Dyp() / _dypLine :
		_IchFromCb(_cbLine, fFalse) + 2 - rc.Dxp() / _dxpChar);
}


/***************************************************************************
	Copy the selection.
***************************************************************************/
bool DCH::_FCopySel(PDOCB *ppdocb)
{
	PDHEX pdhex;
	long ib1, ib2;

	ib1 = _ibOther;
	ib2 = _fFixed ? _pbsf->IbMac() : _ibAnchor;
	if (_ibOther == ib2)
		return fFalse;

	if (pvNil == ppdocb)
		return fTrue;

	SortLw(&ib1, &ib2);
	if (pvNil != (pdhex = DHEX::PdhexNew()))
		{
		if (!pdhex->Pbsf()->FReplaceBsf(_pbsf, ib1, ib2 - ib1, 0, 0))
			ReleasePpo(&pdhex);
		}

	*ppdocb = pdhex;
	return pvNil != *ppdocb;
}


/***************************************************************************
	Clear (delete) the selection.
***************************************************************************/
void DCH::_ClearSel(void)
{
	_FReplace(pvNil, 0, _ibAnchor, _ibOther, fFalse);
}


/***************************************************************************
	Paste over the selection.
***************************************************************************/
bool DCH::_FPaste(PCLIP pclip, bool fDoIt, long cid)
{
	AssertThis(0);
	AssertPo(pclip, 0);
	long ib1, ib2, cb;
	PDOCB pdocb;
	PBSF pbsf;

	if (cidPaste != cid)
		return fFalse;

	if (!pclip->FGetFormat(kclsDHEX) && !pclip->FGetFormat(kclsTXTB))
		return fFalse;

	if (!fDoIt)
		return fTrue;

	if (pclip->FGetFormat(kclsDHEX, &pdocb))
		{
		if (pvNil == (pbsf = ((PDHEX)pdocb)->Pbsf()) || 0 >= (cb = pbsf->IbMac()))
			{
			ReleasePpo(&pdocb);
			return fFalse;
			}
		}
	else if (pclip->FGetFormat(kclsTXTB, &pdocb))
		{
		if (pvNil == (pbsf = ((PTXTB)pdocb)->Pbsf()) ||
				0 >= (cb = pbsf->IbMac() - size(achar)))
			{
			ReleasePpo(&pdocb);
			return fFalse;
			}
		}
	else
		return fFalse;

	ib1 = _ibAnchor;
	ib2 = _ibOther;
	_SwitchSel(fFalse);
	SortLw(&ib1, &ib2);
	if (_fFixed)
		{
		cb = LwMin(cb, _pbsf->IbMac() - ib1);
		ib2 = ib1 + cb;
		}
	if (!_pbsf->FReplaceBsf(pbsf, 0, cb, ib1, ib2 - ib1))
		{
		ReleasePpo(&pdocb);
		return fFalse;
		}

	_InvalAllDch(ib1, cb, ib2 - ib1);
	ib1 += cb;
	_SetSel(ib1, ib1, fFalse /*REVIEW shonk*/);
	_ShowSel();

	ReleasePpo(&pdocb);
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of an object.
***************************************************************************/
void DCH::AssertValid(ulong grf)
{
	DCH_PAR::AssertValid(0);
	AssertPo(_pbsf, 0);
	AssertIn(_cbLine, 1, kcbMaxLineDch + 1);
	AssertIn(_ibAnchor, 0, kcbMax);
	AssertIn(_ibOther, 0, kcbMax);
}


/***************************************************************************
	Mark memory for the DCH.
***************************************************************************/
void DCH::MarkMem(void)
{
	AssertValid(0);
	DCH_PAR::MarkMem();
	MarkMemObj(_pbsf);
}
#endif //DEBUG


/***************************************************************************
	Constructor for a chunk hex editing doc.
***************************************************************************/
DOCH::DOCH(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno)
	: DOCE(pdocb, pcfl, ctg, cno)
{
}


/***************************************************************************
	Creates a new hex editing doc based on the given chunk.  Asserts that
	there are no open editing docs based on the chunk.
***************************************************************************/
PDOCH DOCH::PdochNew(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno)
{
	AssertPo(pdocb, 0);
	AssertPo(pcfl, 0);

	Assert(pvNil == DOCE::PdoceFromChunk(pdocb, pcfl, ctg, cno),
		"DOCE already exists for the chunk");
	PDOCH pdoch;

	if (pvNil == (pdoch = NewObj DOCH(pdocb, pcfl, ctg, cno)))
		return pvNil;
	if (!pdoch->_FInit())
		{
		ReleasePpo(&pdoch);
		return pvNil;
		}
	AssertPo(pdoch, 0);
	return pdoch;
}


/***************************************************************************
	Initialize the stream from the given flo.
***************************************************************************/
bool DOCH::_FRead(PBLCK pblck)
{
	FLO flo;
	bool fRet;

	if (!pblck->FUnpackData())
		return fFalse;

	if (pvNil == (flo.pfil = FIL::PfilCreateTemp()))
		return fFalse;
	flo.fp = 0;
	flo.cb = pblck->Cb();

	if (!pblck->FWriteToFlo(&flo))
		{
		ReleasePpo(&flo.pfil);
		return fFalse;
		}
	fRet = _bsf.FReplaceFlo(&flo, fFalse, 0, _bsf.IbMac());
	ReleasePpo(&flo.pfil);

	return fRet;
}


/***************************************************************************
	Create a new DDG for the doc.
***************************************************************************/
PDDG DOCH::PddgNew(PGCB pgcb)
{
	AssertThis(0);
	return DCH::PdchNew(this, &_bsf, fFalse, pgcb);
}


/***************************************************************************
	Returns the length of the data on file
***************************************************************************/
long DOCH::_CbOnFile(void)
{
	AssertThis(0);
	return _bsf.IbMac();
}


/***************************************************************************
	Writes the data and returns success/failure.
***************************************************************************/
bool DOCH::_FWrite(PBLCK pblck, bool fRedirect)
{
	AssertThis(0);
	if (!_bsf.FWriteRgb(pblck))
		return fFalse;
	_FRead(pblck);
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of an object.
***************************************************************************/
void DOCH::AssertValid(ulong grf)
{
	DOCH_PAR::AssertValid(0);
	AssertPo(&_bsf, 0);
}


/***************************************************************************
	Mark memory used by the DOCH.
***************************************************************************/
void DOCH::MarkMem(void)
{
	AssertThis(0);
	DOCH_PAR::MarkMem();
	MarkMemObj(&_bsf);
}
#endif //DEBUG

