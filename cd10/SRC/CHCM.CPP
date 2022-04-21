/***************************************************************************
	Author: ******
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Chunky file compiler and decompiler class implementations.

***************************************************************************/
#include "kidframe.h" //because we need scrcomg
ASSERTNAME

RTCLASS(CHCM)
RTCLASS(CHLX)
RTCLASS(CHDC)


PSZ _mpertpsz[] =
	{
	PszLit("no error"),
	PszLit("Internal allocation error"), //ertOom
	PszLit("Can't open the given file"), //ertOpenFile
	PszLit("Can't read the given metafile"), //ertReadMeta
	PszLit("Number not in range for BYTE"), //ertRangeByte
	PszLit("Number not in range for SHORT"), //ertRangeShort
	PszLit("Invalid data before atomic chunk"), //ertBufData
	PszLit("Open parenthesis '(' expected"), //ertParenOpen
	PszLit("Unexpected end of file"), //ertEof
	PszLit("String expected"), //ertNeedString
	PszLit("Numeric value expected"), //ertNeedNumber
	PszLit("Unexpected Token"), //ertBadToken
	PszLit("Close parenthesis ')' expected"), //ertParenClose
	PszLit("Invalid CHUNK declaration"), //ertChunkHead
	PszLit("Duplicate CHUNK declaration"), //ertDupChunk
	PszLit("Invalid CHILD declaration"), //ertBodyChildHead
	PszLit("Child chunk doesn't exist"), //ertChildMissing
	PszLit("A cycle would be created by this adoption"), //ertCycle
	PszLit("Invalid PARENT declaration"), //ertBodyParentHead
	PszLit("Parent chunk doesn't exist"), //ertParentMissing
	PszLit("Alignment parameter out of range"), //ertBodyAlignRange
	PszLit("File name expected"), //ertBodyFile
	PszLit("ENDCHUNK expected"), //ertNeedEndChunk
	PszLit("Invalid GL or AL declaration"), //ertListHead
	PszLit("Invalid size for list entries"), //ertListEntrySize
	PszLit("Variable undefined"), //ertVarUndefined
	PszLit("Too much data for item"), //ertItemOverflow
	PszLit("Can't have a free item in a general collection"), //ertBadFree
	PszLit("Syntax error"), //ertSyntax
	PszLit("Invalid GG or AG declaration"), //ertGroupHead
	PszLit("Invalid size for fixed group data"), //ertGroupEntrySize
	PszLit("Invalid GST or AST declaration"), //ertGstHead
	PszLit("Invalid size for extra string table data"), //ertGstEntrySize
	PszLit("Script compilation failed"), //ertScript
	PszLit("Invalid ADOPT declaration"), //ertAdoptHead
	PszLit("CHUNK declaration expected"), //ertNeedChunk
	PszLit("Invalid BITMAP declaration"), //ertBodyBitmapHead
	PszLit("Can't read the given bitmap file"), //ertReadBitmap
	PszLit("Disassembling the script failed"), //ertBadScript
	PszLit("Can't read the given cursor file"), //ertReadCursor
	PszLit("Can't read given file as a packed file"), //ertPackedFile
	PszLit("Can't read the given midi file"), //ertReadMidi
	PszLit("Bad pack format"), //ertBadPackFmt
	PszLit("Illegal LONER primitive in SUBFILE"), //ertLonerInSub
	PszLit("Unterminated SUBFILE"), //ertNoEndSubFile
	};


/***************************************************************************
	Constructor for the CHCM class.
***************************************************************************/
CHCM::CHCM(void)
{
	_pglcsfc = pvNil;
	_pcfl = pvNil;
	_pchlx = pvNil;
	_pglckiLoner = pvNil;
	_pmsnkError = pvNil;
	_cactError = 0;
	AssertThis(0);
}


/***************************************************************************
	Destructor for the CHCM class.
***************************************************************************/
CHCM::~CHCM(void)
{
	if (pvNil != _pglcsfc)
		{
		CSFC csfc;

		while (_pglcsfc->FPop(&csfc))
			{
			ReleasePpo(&_pcfl);
			_pcfl = csfc.pcfl;
			}
		ReleasePpo(&_pglcsfc);
		}
	ReleasePpo(&_pcfl);
	ReleasePpo(&_pchlx);
	ReleasePpo(&_pglckiLoner);
}


#ifdef DEBUG
/***************************************************************************
	Assert that the CHCM is a valid object.
***************************************************************************/
void CHCM::AssertValid(ulong grf)
{
	CHCM_PAR::AssertValid(grf);
	AssertNilOrPo(_pcfl, 0);
	AssertPo(&_bsf, 0);
	AssertNilOrPo(_pchlx, 0);
	AssertNilOrPo(_pglckiLoner, 0);
	AssertNilOrPo(_pmsnkError, 0);
}


/***************************************************************************
	Mark memory for the CHCM object.
***************************************************************************/
void CHCM::MarkMem(void)
{
	AssertThis(0);

	CHCM_PAR::MarkMem();
	MarkMemObj(_pglcsfc);
	MarkMemObj(_pcfl);
	MarkMemObj(&_bsf);
	MarkMemObj(_pchlx);
	MarkMemObj(_pglckiLoner);
}
#endif //DEBUG


/***************************************************************************
	Registers an error, prints error message with filename and line number.
	pszMessage may be nil.
***************************************************************************/
void CHCM::_Error(long ert, PSZ pszMessage)
{
	AssertThis(0);
	AssertIn(ert, ertNil, ertLim);
	AssertPo(_pchlx, 0);
	STN stnFile;
	STN stn;

	_pchlx->GetStnFile(&stnFile);
	if (ertNil == ert)
		{
		stn.FFormatSz(
			pszMessage == pvNil ? PszLit("%s(%d:%d) : warning") :
				PszLit("%s(%d:%d) : warning : %z"),
			&stnFile, _pchlx->LwLine(), _pchlx->IchLine(), pszMessage);
		}
	else
		{
		_cactError++;
		stn.FFormatSz(
			pszMessage == pvNil ? PszLit("%s(%d:%d) : error : %z") :
				PszLit("%s(%d:%d) : error : %z : %z"),
			&stnFile, _pchlx->LwLine(), _pchlx->IchLine(),
			_mpertpsz[ert], pszMessage);
		}

	_pmsnkError->ReportLine(stn.Psz());
}


/***************************************************************************
	Checks that lw could be accepted under the current numerical mode.
***************************************************************************/
void CHCM::_GetRgbFromLw(long lw, byte *prgb)
{
	AssertThis(0);
	AssertPvCb(prgb, size(long));

	switch(_cbNum)
		{
	case size(byte):
		if (lw < -128 || lw > kbMax)
			_Error(ertRangeByte);
		prgb[0] = B0Lw(lw);
		break;

	case size(short):
		if ((lw < kswMin) || (lw > ksuMax))
			_Error(ertRangeShort);
		*(short *)prgb = SwLow(lw);
		break;

	default:
		Assert(_cbNum == size(long), "invalid numerical mode");
		_cbNum = size(long);
		*(long *)prgb = lw;
		break;
		}

	if (_bo != kboCur && _cbNum > 1)
		ReversePb(prgb, _cbNum);
}


/***************************************************************************
	Checks if data is already in the buffer (and issues an error) for a
	non-buffer command such as metafile import.
***************************************************************************/
void CHCM::_ErrorOnData(PSZ pszPreceed)
{
	AssertThis(0);
	AssertSz(pszPreceed);

	if (_bsf.IbMac() > 0)
		{
		// already data
		_Error(ertBufData, pszPreceed);

		// clear buffer
		_bsf.FReplace(pvNil, 0, 0, _bsf.IbMac());
		}
}


/***************************************************************************
	Get a token, automatically handling mode change commands and negatives.
	Return true iff *ptok is valid, not whether an error occurred.
***************************************************************************/
bool CHCM::_FGetCleanTok(TOK *ptok, bool fEofOk)
{
	AssertThis(0);
	AssertVarMem(ptok);

	long cactNegate = 0;

	for (;;)
		{
		if (!_pchlx->FGetTokSkipSemi(ptok))
			{
			if (cactNegate > 0)
				_Error(ertSyntax);
			if (!fEofOk)
				_Error(ertEof);
			return fFalse;
			}

		switch (ptok->tt)
			{
		default:
			if (cactNegate > 0)
				_Error(ertSyntax);
			return fTrue;
		case ttLong:
			if (cactNegate & 1)
				ptok->lw = -ptok->lw;
			return fTrue;
		case ttSub:
			cactNegate++;
			break;
		case ttModeStn:
			_sm = smStn;
			break;
		case ttModeStz:
			_sm = smStz;
			break;
		case ttModeSz:
			_sm = smSz;
			break;
		case ttModeSt:
			_sm = smSt;
			break;
		case ttModeByte:
			_cbNum = size(byte);
			break;
		case ttModeShort:
			_cbNum = size(short);
			break;
		case ttModeLong:
			_cbNum = size(long);
			break;
		case ttMacBo:
			_bo = MacWin(kboCur, kboOther);
			break;
		case ttWinBo:
			_bo = MacWin(kboOther, kboCur);
			break;
		case ttMacOsk:
			_osk = koskMac;
			break;
		case ttWinOsk:
			_osk = koskWin;
			break;
			}
		}
}


/***************************************************************************
	Skip tokens until we encounter the given token type.
***************************************************************************/
void CHCM::_SkipPastTok(long tt)
{
	AssertThis(0);
	TOK tok;

	while (_FGetCleanTok(&tok) && tt != tok.tt)
		;
}


/***************************************************************************
	Parse a parenthesized header from the source file.
***************************************************************************/
bool CHCM::_FParseParenHeader(PHP *prgphp, long cphpMax, long *pcphp)
{
	AssertThis(0);
	AssertIn(cphpMax, 1, kcbMax);
	AssertPvCb(prgphp, LwMul(cphpMax, size(PHP)));
	AssertVarMem(pcphp);

	TOK tok;
	long iphp;

	if (!_pchlx->FGetTok(&tok))
		{
		TrashVar(pcphp);
		return fFalse;
		}

	if (ttOpenParen != tok.tt)
		{
		_Error(ertParenOpen);
		goto LFail;
		}

	for (iphp = 0; iphp < cphpMax; iphp++)
		{
		AssertNilOrPo(prgphp[iphp].pstn, 0);

		if (!_FGetCleanTok(&tok))
			{
			TrashVar(pcphp);
			return fFalse;
			}

		if (ttCloseParen == tok.tt)
			{
			// close paren = end of header
			*pcphp = iphp;

			// empty remaining strings
			for ( ; iphp < cphpMax; iphp++)
				{
				AssertNilOrPo(prgphp[iphp].pstn, 0);
				if (pvNil != prgphp[iphp].pstn)
					prgphp[iphp].pstn->SetNil();
				}
			return fTrue;
			}

		if (ttLong == tok.tt)
			{
			// numerical value
			if (prgphp[iphp].pstn == pvNil)
				prgphp[iphp].lw = tok.lw;
			else
				{
				_Error(ertNeedString);
				prgphp[iphp].pstn->SetNil();
				}
			}
		else if (ttString == tok.tt)
			{
			// string
			if (prgphp[iphp].pstn != pvNil)
				*prgphp[iphp].pstn = tok.stn;
			else
				{
				_Error(ertNeedNumber);
				prgphp[iphp].lw = 0;
				}
			}
		else
			{
			// invalid token in header
			_Error(ertBadToken);
			}
		}

	// get closing paren
	if (!_pchlx->FGetTok(&tok))
		{
		TrashVar(pcphp);
		return fFalse;
		}

	if (ttCloseParen != tok.tt)
		{
		_Error(ertParenClose);
LFail:
		_SkipPastTok(ttCloseParen);
		return fFalse;
		}

	*pcphp = cphpMax;
	return fTrue;
}


/***************************************************************************
	Parse a chunk header from the source file.
***************************************************************************/
void CHCM::_ParseChunkHeader(CTG *pctg, CNO *pcno)
{
	AssertThis(0);
	AssertVarMem(pctg);
	AssertVarMem(pcno);

	STN stnChunkName;
	PHP rgphp[3];
	long cphp;

	ClearPb(rgphp, size(rgphp));
	rgphp[2].pstn = &stnChunkName;
	if (!_FParseParenHeader(rgphp, 3, &cphp) || cphp < 2)
		{
		_Error(ertChunkHead);
		goto LFail;
		}

	*pctg = rgphp[0].lw;
	*pcno = rgphp[1].lw;

	// write empty chunk
	if (_pcfl->FFind(*pctg, *pcno))
		{
		// duplicate chunk!
		_Error(ertDupChunk);
LFail:
		_SkipPastTok(ttEndChunk);
		TrashVar(pctg);
		TrashVar(pcno);
		return;
		}

	// create the chunk and set its name
	if (!_pcfl->FPutPv(pvNil, 0, *pctg, *pcno) ||
		stnChunkName.Cch() > 0 && !FError() &&
			!_pcfl->FSetName(*pctg, *pcno, &stnChunkName))
		{
		_Error(ertOom);
		}
}


/***************************************************************************
	Append a string to the chunk data stream.
***************************************************************************/
void CHCM::_AppendString(PSTN pstnValue)
{
	AssertThis(0);
	AssertPo(pstnValue, 0);

	void *pv;
	long cb;
	byte rgb[kcbMaxDataStn];

	switch (_sm)
		{
	default:
		Bug("Invalid string mode");
		//fall through
	case smStn:
		cb = pstnValue->CbData();
		pstnValue->GetData(rgb);
		pv = rgb;
		break;
	case smStz:
		pv = pstnValue->Pstz();
		cb = CchTotStz((PSTZ)pv) * size(achar);
		break;
	case smSz:
		pv = pstnValue->Psz();
		cb = CchTotSz((PSZ)pv) * size(achar);
		break;
	case smSt:
		pv = pstnValue->Pst();
		cb = CchTotSt((PST)pv) * size(achar);
		break;
		}

	if (!FError() && !_bsf.FReplace(pv, cb, _bsf.IbMac(), 0))
		_Error(ertOom);
}


/***************************************************************************
	Stores a numerical value in the chunk data stream.
***************************************************************************/
void CHCM::_AppendNumber(long lwValue)
{
	AssertThis(0);
	byte rgb[size(long)];

	_GetRgbFromLw(lwValue, rgb);
	if (!FError() && !_bsf.FReplace(rgb, _cbNum, _bsf.IbMac(), 0))
		_Error(ertOom);
}


/***************************************************************************
	Parse a child statement from the source file.
***************************************************************************/
void CHCM::_ParseBodyChild(CTG ctg, CNO cno)
{
	AssertThis(0);
	CTG ctgChild;
	CNO cnoChild;
	CHID chid;
	PHP rgphp[3];
	long cphp;

	ClearPb(rgphp, size(rgphp));
	if (!_FParseParenHeader(rgphp, 3, &cphp) || cphp < 2)
		{
		_Error(ertBodyChildHead);
		return;
		}

	ctgChild = rgphp[0].lw;
	cnoChild = rgphp[1].lw;
	chid = rgphp[2].lw;

	// check if chunk exists
	if (!_pcfl->FFind(ctgChild, cnoChild))
		{
		_Error(ertChildMissing);
		return;
		}

	// check if cycle would be created
	if (_pcfl->TIsDescendent(ctgChild, cnoChild, ctg, cno) != tNo)
		{
		_Error(ertCycle);
		return;
		}

	// do the adoption
	if (!FError() && !_pcfl->FAdoptChild(ctg, cno, ctgChild, cnoChild,
			chid, fTrue))
		{
		_Error(ertOom);
		}
}


/***************************************************************************
	Parse a parent statement from the source file.
***************************************************************************/
void CHCM::_ParseBodyParent(CTG ctg, CNO cno)
{
	AssertThis(0);
	CTG ctgParent;
	CNO cnoParent;
	CHID chid;
	PHP rgphp[3];
	long cphp;

	ClearPb(rgphp, size(rgphp));
	if (!_FParseParenHeader(rgphp, 3, &cphp) || cphp < 2)
		{
		_Error(ertBodyParentHead);
		return;
		}

	ctgParent = rgphp[0].lw;
	cnoParent = rgphp[1].lw;
	chid = rgphp[2].lw;

	// check if chunk exists
	if (!_pcfl->FFind(ctgParent, cnoParent))
		{
		_Error(ertParentMissing);
		return;
		}

	// check if cycle would be created
	if (_pcfl->TIsDescendent(ctg, cno, ctgParent, cnoParent) != tNo)
		{
		_Error(ertCycle);
		return;
		}

	// do the adoption
	if (!FError() && !_pcfl->FAdoptChild(ctgParent, cnoParent, ctg, cno,
			chid, fTrue))
		{
		_Error(ertOom);
		}
}


/***************************************************************************
	Parse an align statement from the source file.
***************************************************************************/
void CHCM::_ParseBodyAlign(void)
{
	AssertThis(0);
	TOK tok;

	if (!_FGetCleanTok(&tok))
		return;

	if (tok.tt != ttLong)
		{
		_Error(ertNeedNumber);
		return;
		}

	if (!FIn(tok.lw, kcbMinAlign, kcbMaxAlign + 1))
		{
		STN stn;

		stn.FFormatSz(PszLit("legal range for alignment is (%d, %d)"),
			kcbMinAlign, kcbMaxAlign);
		_Error(ertBodyAlignRange, stn.Psz());
		return;
		}

	if (!FError())
		{
		//actually do the padding
		byte rgb[100];
		long cb;
		long ibMac = _bsf.IbMac();
		long ibMacNew = LwRoundAway(ibMac, tok.lw);

		AssertIn(ibMacNew, ibMac, ibMac + tok.lw);
		while ((ibMac = _bsf.IbMac()) < ibMacNew)
			{
			cb = LwMin(ibMacNew - ibMac, size(rgb));
			ClearPb(rgb, cb);
			if (!_bsf.FReplace(rgb, cb, ibMac, 0))
				{
				_Error(ertOom);
				return;
				}
			}
		}
}


/***************************************************************************
	Parse a file statement from the source file.
***************************************************************************/
void CHCM::_ParseBodyFile(void)
{
	AssertThis(0);
	FNI fni;
	FLO floSrc;

	if (!_pchlx->FGetPath(&fni))
		{
		_Error(ertBodyFile);
		_SkipPastTok(ttEndChunk);
		return;
		}

	if (pvNil == (floSrc.pfil = FIL::PfilOpen(&fni)))
		{
		_Error(ertOpenFile);
		return;
		}

	floSrc.fp = 0;
	floSrc.cb = floSrc.pfil->FpMac();
	if (!_bsf.FReplaceFlo(&floSrc, fFalse, _bsf.IbMac(), 0))
		_Error(ertOom);

	ReleasePpo(&floSrc.pfil);
}


/***************************************************************************
	Start a write operation. If fPack is true, allocate a temporary block.
	Otherwise, get the block on the CFL. The caller should write its data
	into the pblck, then call _FEndWrite to complete the operation.
***************************************************************************/
bool CHCM::_FPrepWrite(bool fPack, long cb, CTG ctg, CNO cno, PBLCK pblck)
{
	AssertThis(0);
	AssertPo(pblck, 0);

	if (fPack)
		{
		pblck->Free();
		Assert(!pblck->FPacked(), "why is block packed?");
		return pblck->FSetTemp(cb);
		}

	return _pcfl->FPut(cb, ctg, cno, pblck);
}


/***************************************************************************
	Balances a call to _FPrepWrite.
***************************************************************************/
bool CHCM::_FEndWrite(bool fPack, CTG ctg, CNO cno, PBLCK pblck)
{
	AssertThis(0);
	AssertPo(pblck, fblckUnpacked);

	if (fPack)
		{
		// we don't fail if we can't compress it
		pblck->FPackData();
		return _pcfl->FPutBlck(pblck, ctg, cno);
		}

	AssertPo(pblck, fblckFile);
	return fTrue;
}


/***************************************************************************
	Parse a metafile import command from the source file.
***************************************************************************/
void CHCM::_ParseBodyMeta(bool fPack, CTG ctg, CNO cno)
{
	AssertThis(0);
	FNI fni;
	BLCK blck;
	PPIC ppic;
	TOK tok;

	if (!_pchlx->FGetPath(&fni))
		{
		_Error(ertBodyFile);
		_SkipPastTok(ttEndChunk);
		return;
		}

	if (pvNil == (ppic = PIC::PpicReadNative(&fni)))
		{
		_Error(ertReadMeta);
		return;
		}

	if (!FError())
		{
		if (!_FPrepWrite(fPack, ppic->CbOnFile(), ctg, cno, &blck) ||
				!ppic->FWrite(&blck) || !_FEndWrite(fPack, ctg, cno, &blck))
			{
			_Error(ertOom);
			}
		}
	ReleasePpo(&ppic);

	if (_FGetCleanTok(&tok) && ttEndChunk != tok.tt)
		{
		_Error(ertNeedEndChunk);
		_SkipPastTok(ttEndChunk);
		}
}


/***************************************************************************
	Parse a bitmap import command from the source file.
***************************************************************************/
void CHCM::_ParseBodyBitmap(bool fPack, bool fMask, CTG ctg, CNO cno)
{
	AssertThis(0);
	FNI fni;
	BLCK blck;
	TOK tok;
	PHP rgphp[3];
	long cphp;
	PMBMP pmbmp = pvNil;

	ClearPb(rgphp, size(rgphp));
	if (!_FParseParenHeader(rgphp, 3, &cphp))
		{
		_Error(ertBodyBitmapHead);
		return;
		}

	if (!_pchlx->FGetPath(&fni))
		{
		_Error(ertBodyFile);
		goto LFail;
		}

	if (pvNil == (pmbmp = MBMP::PmbmpReadNative(&fni, (byte)rgphp[0].lw,
			rgphp[1].lw, rgphp[2].lw, fMask ? fmbmpMask : fmbmpNil)))
		{
		STN stn;
		fni.GetStnPath(&stn);
		_Error(ertReadBitmap, stn.Psz());
		goto LFail;
		}

	if (!FError())
		{
		if (!_FPrepWrite(fPack, pmbmp->CbOnFile(), ctg, cno, &blck) ||
				!pmbmp->FWrite(&blck) || !_FEndWrite(fPack, ctg, cno, &blck))
			{
			_Error(ertOom);
			}
		}
	ReleasePpo(&pmbmp);

	if (_FGetCleanTok(&tok) && ttEndChunk != tok.tt)
		{
		_Error(ertNeedEndChunk);
LFail:
		_SkipPastTok(ttEndChunk);
		}
}


/***************************************************************************
	Parse a palette import command from the source file.
***************************************************************************/
void CHCM::_ParseBodyPalette(bool fPack, CTG ctg, CNO cno)
{
	AssertThis(0);
	FNI fni;
	BLCK blck;
	TOK tok;
	PGL pglclr;

	if (!_pchlx->FGetPath(&fni))
		{
		_Error(ertBodyFile);
		goto LFail;
		}

	if (!FReadBitmap(&fni, pvNil, &pglclr, pvNil, pvNil, pvNil))
		{
		_Error(ertReadBitmap);
		goto LFail;
		}

	if (!FError())
		{
		if (!_FPrepWrite(fPack, pglclr->CbOnFile(), ctg, cno, &blck) ||
				!pglclr->FWrite(&blck) || !_FEndWrite(fPack, ctg, cno, &blck))
			{
			_Error(ertOom);
			}
		}
	ReleasePpo(&pglclr);

	if (_FGetCleanTok(&tok) && ttEndChunk != tok.tt)
		{
		_Error(ertNeedEndChunk);
LFail:
		_SkipPastTok(ttEndChunk);
		}
}


/***************************************************************************
	Parse a midi import command from the source file.
***************************************************************************/
void CHCM::_ParseBodyMidi(bool fPack, CTG ctg, CNO cno)
{
	AssertThis(0);
	FNI fni;
	BLCK blck;
	TOK tok;
	PMIDS pmids;

	if (!_pchlx->FGetPath(&fni))
		{
		_Error(ertBodyFile);
		goto LFail;
		}

	if (pvNil == (pmids = MIDS::PmidsReadNative(&fni)))
		{
		_Error(ertReadMidi);
		goto LFail;
		}

	if (!FError())
		{
		if (!_FPrepWrite(fPack, pmids->CbOnFile(), ctg, cno, &blck) ||
				!pmids->FWrite(&blck) || !_FEndWrite(fPack, ctg, cno, &blck))
			{
			_Error(ertOom);
			}
		}
	ReleasePpo(&pmids);

	if (_FGetCleanTok(&tok) && ttEndChunk != tok.tt)
		{
		_Error(ertNeedEndChunk);
LFail:
		_SkipPastTok(ttEndChunk);
		}
}


/***************************************************************************
	Parse a cursor import command from the source file.
***************************************************************************/
void CHCM::_ParseBodyCursor(bool fPack, CTG ctg, CNO cno)
{
	// These are for parsing a Windows cursor file
	struct CURDIR
		{
		byte dxp;
		byte dyp;
		byte bZero1;
		byte bZero2;
		short xp;
		short yp;
		long cb;
		long bv;
		};

	struct CURH
		{
		long cbCurh;
		long dxp;
		long dyp;
		short swOne1;
		short swOne2;
		long lwZero1;
		long lwZero2;
		long lwZero3;
		long lwZero4;
		long lwZero5;
		long lwZero6;
		long lw1;
		long lw2;
		};

	AssertThis(0);
	FNI fni;
	BLCK blck;
	FLO floSrc;
	TOK tok;
	long ccurdir, cbBits;
	CURF curf;
	short rgsw[3];
	byte *prgb;
	CURDIR *pcurdir;
	CURH *pcurh;
	PGG pggcurf = pvNil;
	HQ hq = hqNil;

	floSrc.pfil = pvNil;
	if (!_pchlx->FGetPath(&fni))
		{
		_Error(ertBodyFile);
		_SkipPastTok(ttEndChunk);
		return;
		}

	if (pvNil == (floSrc.pfil = FIL::PfilOpen(&fni)))
		{
		_Error(ertReadCursor);
		goto LFail;
		}

	floSrc.fp = 0;
	floSrc.cb = floSrc.pfil->FpMac();
	if (floSrc.cb < size(rgsw) + size(CURDIR) + size(CURH) + 128 ||
		!floSrc.FReadRgb(rgsw, size(rgsw), 0) ||
		rgsw[0] != 0 || rgsw[1] != 2 ||
		!FIn(ccurdir = rgsw[2], 1, (floSrc.cb - size(rgsw)) / size(CURDIR)))
		{
		_Error(ertReadCursor);
		goto LFail;
		}

	floSrc.cb -= size(rgsw);
	floSrc.fp = size(rgsw);
	if (!floSrc.FReadHq(&hq))
		{
		_Error(ertOom);
		goto LFail;
		}
	ReleasePpo(&floSrc.pfil);

	prgb = (byte *)PvLockHq(hq);
	pcurdir = (CURDIR *)prgb;
	if (pvNil == (pggcurf = GG::PggNew(size(CURF), ccurdir)))
		{
		_Error(ertOom);
		goto LFail;
		}
	while (ccurdir-- > 0)
		{
		cbBits = pcurdir->dxp == 32 ? 256 : 128;
		if (pcurdir->dxp != pcurdir->dyp ||
			pcurdir->dxp != 16 && pcurdir->dxp != 32 ||
			pcurdir->bZero1 != 0 || pcurdir->bZero2 != 0 ||
			pcurdir->cb != size(CURH) + cbBits ||
			!FIn(pcurdir->bv -= size(rgsw), LwMul(rgsw[2], size(CURDIR)),
				floSrc.cb - pcurdir->cb + 1) ||
			CbRoundToLong(pcurdir->bv) != pcurdir->bv)
			{
			_Error(ertReadCursor);
			goto LFail;
			}
		curf.curt = curtMonochrome;
		curf.xp = (byte)pcurdir->xp;
		curf.yp = (byte)pcurdir->yp;
		curf.dxp = pcurdir->dxp;
		curf.dyp = pcurdir->dyp;

		pcurh = (CURH *)PvAddBv(prgb, pcurdir->bv);
		if (pcurh->cbCurh != size(CURH) - 2 * size(long) ||
			pcurh->dxp != pcurdir->dxp || pcurh->dyp != 2 * pcurdir->dyp ||
			pcurh->swOne1 != 1 || pcurh->swOne2 != 1 ||
			pcurh->lwZero1 != 0 ||
			(pcurh->lwZero2 != 0 && pcurh->lwZero2 != pcurdir->cb - size(CURH)) ||
			pcurh->lwZero3 != 0 || pcurh->lwZero4 != 0 ||
			pcurh->lwZero5 != 0 || pcurh->lwZero6 != 0)
			{
			_Error(ertReadCursor);
			goto LFail;
			}

		//The bits are stored in upside down DIB order!
		ReversePb(pcurh + 1, pcurdir->cb - size(CURH));
		SwapBytesRglw(pcurh + 1, (pcurdir->cb - size(CURH)) / size(long));

		if (pcurdir->dxp == 16)
			{
			//need to consolidate the bits, because they are stored 4 bytes per
			//row (2 bytes wasted) instead of 2 bytes per row.
			long csw = 32;
			short *pswSrc, *pswDst;

			pswSrc = pswDst = (short *)(pcurh + 1);
			while (csw-- != 0)
				{
				*pswDst++ = *pswSrc++;
				pswSrc++;
				}
			}

		if (!pggcurf->FInsert(pggcurf->IvMac(),
				(long)curf.dxp * curf.dyp / 4, pcurh + 1, &curf))
			{
			_Error(ertOom);
			goto LFail;
			}
		pcurdir++;
		}

LFail:
	//success comes through here also
	ReleasePpo(&floSrc.pfil);
	if (hqNil != hq)
		{
		UnlockHq(hq);
		FreePhq(&hq);
		}

	if (!FError())
		{
		if (!_FPrepWrite(fPack, pggcurf->CbOnFile(), ctg, cno, &blck) ||
				!pggcurf->FWrite(&blck) || !_FEndWrite(fPack, ctg, cno, &blck))
			{
			_Error(ertOom);
			}
		}
	ReleasePpo(&pggcurf);

	if (_FGetCleanTok(&tok) && ttEndChunk != tok.tt)
		{
		_Error(ertNeedEndChunk);
		_SkipPastTok(ttEndChunk);
		}
}


/***************************************************************************
	Parse a data section from the source file.  ptok should be pre-loaded
	with the first token and when _FParseData returns it contains the next
	token to be processed.  Returns false iff no tokens were consumed.
***************************************************************************/
bool CHCM::_FParseData(PTOK ptok)
{
	enum
		{
		psNil,
		psHaveLw,
		psHaveBOr,
		};

	AssertThis(0);
	AssertVarMem(ptok);

	long cbNum;
	long lw;
	long cbNumPrev = _cbNum;
	long ps = psNil;
	bool fRet = fFalse;

	for (;;)
		{
		switch (ptok->tt)
			{
		case ttBOr:
			if (ps == psNil)
				return fRet;
			if (ps != psHaveLw)
				{
				ptok->tt = ttError;
				return fRet;
				}
			ps = psHaveBOr;
			break;

		case ttLong:
			if (ps == psHaveLw)
				{
				SwapVars(&_cbNum, &cbNumPrev);
				_AppendNumber(lw);
				_cbNum = cbNumPrev;
				ps = psNil;
				}
			if (ps == psNil)
				{
				lw = ptok->lw;
				cbNumPrev = _cbNum;
				ps = psHaveLw;
				}
			else
				{
				Assert(ps == psHaveBOr, 0);
				if (cbNumPrev != _cbNum)
					{
					ptok->tt = ttError;
					return fRet;
					}
				lw |= ptok->lw;
				ps = psHaveLw;
				}
			break;

		default:
			if (ps == psHaveBOr)
				{
				ptok->tt = ttError;
				return fRet;
				}
			if (ps == psHaveLw)
				{
				SwapVars(&_cbNum, &cbNumPrev);
				_AppendNumber(lw);
				_cbNum = cbNumPrev;
				ps = psNil;
				}

			switch (ptok->tt)
				{
			case ttString:
				_AppendString(&ptok->stn);
				break;
			case ttAlign:
				_ParseBodyAlign();
				break;
			case ttFile:
				_ParseBodyFile();
				break;
			case ttBo:
				//insert the current byte order
				cbNum = _cbNum;
				_cbNum = size(short);
				_AppendNumber(kboCur);
				_cbNum = cbNum;
				break;
			case ttOsk:
				//insert the current osk
				cbNum = _cbNum;
				_cbNum = size(short);
				_AppendNumber(_osk);
				_cbNum = cbNum;
				break;
			default:
				return fRet;
				}
			break;
			}

		fRet = fTrue;
		if (!_FGetCleanTok(ptok, fTrue))
			{
			ptok->tt = ttError;
			return fRet;
			}
		}
}


/***************************************************************************
	Parse a list structure from the source file.
***************************************************************************/
void CHCM::_ParseBodyList(bool fPack, bool fAl, CTG ctg, CNO cno)
{
	AssertThis(0);
	TOK tok;
	PHP rgphp[1];
	long cphp;
	long cbEntry, cb;
	byte *prgb;
	long iv, iiv;
	BLCK blck;
	PGLB pglb = pvNil;
	PGL pglivFree = pvNil;

	// get size of entry data
	ClearPb(rgphp, size(rgphp));
	if (!_FParseParenHeader(rgphp, 1, &cphp) || cphp < 1)
		{
		_Error(ertListHead);
		_SkipPastTok(ttEndChunk);
		return;
		}

	if (!FIn(cbEntry = rgphp[0].lw, 1, kcbMax))
		{
		_Error(ertListEntrySize);
		_SkipPastTok(ttEndChunk);
		return;
		}

	pglb = fAl ? (PGLB)AL::PalNew(cbEntry) : (PGLB)GL::PglNew(cbEntry);
	if (pvNil == pglb)
		{
		_Error(ertOom);
		_SkipPastTok(ttEndChunk);
		return;
		}
	pglb->SetMinGrow(20);

	//prefetch a token
	if (!_FGetCleanTok(&tok))
		goto LFail;

	for (;;)
		{
		//empty the BSF
		_bsf.FReplace(pvNil, 0, 0, _bsf.IbMac());

		if (ttFree == tok.tt)
			{
			if (!fAl)
				_Error(ertBadFree);
			else if (!FError())
				{
				iv = pglb->IvMac();
				if (pvNil == pglivFree &&
						pvNil == (pglivFree = GL::PglNew(size(long))) ||
					!pglivFree->FAdd(&iv))
					{
					_Error(ertOom);
					}
				}

			if (!_FGetCleanTok(&tok))
				goto LFail;
			}
		else if (ttItem != tok.tt)
			break;
		else
			{
			if (!_FGetCleanTok(&tok))
				goto LFail;
			_FParseData(&tok);
			}

		if ((cb = _bsf.IbMac()) > cbEntry)
			{
			_Error(ertItemOverflow);
			continue;
			}

		AssertIn(cb, 0, cbEntry + 1);
		if (FError())
			continue;

		if (!pglb->FAdd(pvNil, &iv))
			_Error(ertOom);
		else
			{
			Assert(iv == pglb->IvMac() - 1, "what?");
			prgb = (byte *)pglb->PvLock(iv);
			if (cb > 0)
				_bsf.FetchRgb(0, cb, prgb);
			if (cb < cbEntry)
				ClearPb(prgb + cb, cbEntry - cb);
			pglb->Unlock();
			}
		}

	if (ttEndChunk != tok.tt)
		{
		_Error(ertNeedEndChunk);
		_SkipPastTok(ttEndChunk);
		}

	if (FError())
		goto LFail;

	if (pvNil != pglivFree)
		{
		Assert(fAl, "why did GL have free entries?");
		for (iiv = pglivFree->IvMac(); iiv-- > 0; )
			{
			pglivFree->Get(iiv, &iv);
			AssertIn(iv, 0, pglb->IvMac());
			pglb->Delete(iv);
			}
		}

	// write list to disk
	if (!_FPrepWrite(fPack, pglb->CbOnFile(), ctg, cno, &blck) ||
			!pglb->FWrite(&blck, _bo, _osk) ||
			!_FEndWrite(fPack, ctg, cno, &blck))
		{
		_Error(ertOom);
		}

LFail:
	ReleasePpo(&pglb);
	ReleasePpo(&pglivFree);
}


/***************************************************************************
	Parse a group structure from the source file.
***************************************************************************/
void CHCM::_ParseBodyGroup(bool fPack, bool fAg, CTG ctg, CNO cno)
{
	AssertThis(0);
	TOK tok;
	PHP rgphp[1];
	long cphp;
	long cbFixed, cb;
	byte *prgb;
	long iv, iiv;
	BLCK blck;
	bool fFree;
	PGGB pggb = pvNil;
	PGL pglivFree = pvNil;

	// get size of fixed data
	ClearPb(rgphp, size(rgphp));
	if (!_FParseParenHeader(rgphp, 1, &cphp) || cphp < 1)
		{
		_Error(ertGroupHead);
		_SkipPastTok(ttEndChunk);
		return;
		}

	if (!FIn(cbFixed = rgphp[0].lw, 0, kcbMax))
		{
		_Error(ertGroupEntrySize);
		_SkipPastTok(ttEndChunk);
		return;
		}

	pggb = fAg ? (PGGB)AG::PagNew(cbFixed) : (PGGB)GG::PggNew(cbFixed);
	if (pvNil == pggb)
		{
		_Error(ertOom);
		_SkipPastTok(ttEndChunk);
		return;
		}
	pggb->SetMinGrow(10, 100);

	//prefetch a token
	if (!_FGetCleanTok(&tok))
		goto LFail;

	for (;;)
		{
		//empty the BSF
		_bsf.FReplace(pvNil, 0, 0, _bsf.IbMac());

		fFree = (ttFree == tok.tt);
		if (fFree)
			{
			if (!fAg)
				_Error(ertBadFree);
			else if (!FError())
				{
				iv = pggb->IvMac();
				if (pvNil == pglivFree &&
						pvNil == (pglivFree = GL::PglNew(size(long))) ||
					!pglivFree->FAdd(&iv))
					{
					_Error(ertOom);
					}
				}

			if (!_FGetCleanTok(&tok))
				goto LFail;
			}
		else if (ttItem != tok.tt)
			break;
		else
			{
			//get the fixed part
			if (!_FGetCleanTok(&tok))
				goto LFail;
			_FParseData(&tok);
			}

		if ((cb = _bsf.IbMac()) > cbFixed)
			{
			_Error(ertItemOverflow);
			cb = cbFixed;
			}

		AssertIn(cb, 0, cbFixed + 1);
		if (!FError())
			{
			//add the item
			if (!pggb->FAdd(pvNil, &iv))
				_Error(ertOom);
			else
				{
				Assert(iv == pggb->IvMac() - 1, "what?");
				prgb = (byte *)pggb->PvFixedLock(iv);
				if (cb > 0)
					_bsf.FetchRgb(0, cb, prgb);
				if (cb < cbFixed)
					ClearPb(prgb + cb, cbFixed - cb);
				pggb->Unlock();
				}
			}

		//check for a variable part
		if (fFree || ttVar != tok.tt)
			continue;

		if (!_FGetCleanTok(&tok))
			goto LFail;
		_bsf.FReplace(pvNil, 0, 0, _bsf.IbMac());
		_FParseData(&tok);

		if (FError() || (cb = _bsf.IbMac()) <= 0)
			continue;

		Assert(iv == pggb->IvMac() - 1, "iv wrong");
		if (!pggb->FInsertRgb(iv, 0, cb, pvNil))
			_Error(ertOom);
		else
			{
			prgb = (byte *)pggb->PvLock(iv);
			_bsf.FetchRgb(0, cb, prgb);
			pggb->Unlock();
			}
		}

	if (ttEndChunk != tok.tt)
		{
		_Error(ertNeedEndChunk);
		_SkipPastTok(ttEndChunk);
		}

	if (FError())
		goto LFail;

	if (pvNil != pglivFree)
		{
		Assert(fAg, "why did GG have free entries?");
		for (iiv = pglivFree->IvMac(); iiv-- > 0; )
			{
			pglivFree->Get(iiv, &iv);
			AssertIn(iv, 0, pggb->IvMac());
			pggb->Delete(iv);
			}
		}

	// write list to disk
	if (!_FPrepWrite(fPack, pggb->CbOnFile(), ctg, cno, &blck) ||
			!pggb->FWrite(&blck, _bo, _osk) ||
			!_FEndWrite(fPack, ctg, cno, &blck))
		{
		_Error(ertOom);
		}

LFail:
	ReleasePpo(&pggb);
	ReleasePpo(&pglivFree);
}


/***************************************************************************
	Parse a string table from the source file.
***************************************************************************/
void CHCM::_ParseBodyStringTable(bool fPack, bool fAst, CTG ctg, CNO cno)
{
	AssertThis(0);
	TOK tok;
	PHP rgphp[1];
	long cphp;
	long cbExtra, cb;
	long iv, iiv;
	STN stn;
	BLCK blck;
	bool fFree;
	PGSTB pgstb = pvNil;
	PGL pglivFree = pvNil;
	void *pvExtra = pvNil;

	// get size of attached data
	ClearPb(rgphp, size(rgphp));
	if (!_FParseParenHeader(rgphp, 1, &cphp) || cphp < 1)
		{
		_Error(ertGstHead);
		_SkipPastTok(ttEndChunk);
		return;
		}

	if (!FIn(cbExtra = rgphp[0].lw, 0, kcbMax) || cbExtra % size(long) != 0)
		{
		_Error(ertGstEntrySize);
		_SkipPastTok(ttEndChunk);
		return;
		}

	pgstb = fAst ? (PGSTB)AST::PastNew(cbExtra) : (PGSTB)GST::PgstNew(cbExtra);
	if (pvNil == pgstb || cbExtra > 0 &&
			!FAllocPv(&pvExtra, cbExtra, fmemNil, mprNormal))
		{
		_Error(ertOom);
		_SkipPastTok(ttEndChunk);
		goto LFail;
		}
	pgstb->SetMinGrow(10, 100);

	//prefetch a token
	if (!_FGetCleanTok(&tok))
		goto LFail;

	for (;;)
		{
		fFree = (ttFree == tok.tt);
		if (fFree)
			{
			if (!fAst)
				_Error(ertBadFree);
			else if (!FError())
				{
				iv = pgstb->IvMac();
				if (pvNil == pglivFree &&
						pvNil == (pglivFree = GL::PglNew(size(long))) ||
					!pglivFree->FAdd(&iv))
					{
					_Error(ertOom);
					}
				}

			if (!_FGetCleanTok(&tok))
				goto LFail;
			stn.SetNil();
			}
		else if (ttItem != tok.tt)
			break;
		else
			{
			if (!_FGetCleanTok(&tok))
				goto LFail;
			if (ttString != tok.tt)
				{
				_Error(ertNeedString);
				_SkipPastTok(ttEndChunk);
				goto LFail;
				}
			stn = tok.stn;
			if (!_FGetCleanTok(&tok))
				goto LFail;
			}

		if (!FError() && !pgstb->FAddStn(&stn, pvNil, &iv))
			_Error(ertOom);

		if (cbExtra <= 0 || fFree)
			continue;

		//empty the BSF and get the extra data
		_bsf.FReplace(pvNil, 0, 0, _bsf.IbMac());
		_FParseData(&tok);

		if ((cb = _bsf.IbMac()) > cbExtra)
			{
			_Error(ertItemOverflow);
			cb = cbExtra;
			}

		AssertIn(cb, 0, cbExtra + 1);
		if (!FError())
			{
			//add the item
			Assert(iv == pgstb->IvMac() - 1, "what?");
			if (cb > 0)
				_bsf.FetchRgb(0, cb, pvExtra);
			if (cb < cbExtra)
				ClearPb(PvAddBv(pvExtra, cb), cbExtra - cb);
			pgstb->PutExtra(iv, pvExtra);
			}
		}

	if (ttEndChunk != tok.tt)
		{
		_Error(ertNeedEndChunk);
		_SkipPastTok(ttEndChunk);
		}

	if (FError())
		goto LFail;

	if (pvNil != pglivFree)
		{
		Assert(fAst, "why did GST have free entries?");
		for (iiv = pglivFree->IvMac(); iiv-- > 0; )
			{
			pglivFree->Get(iiv, &iv);
			AssertIn(iv, 0, pgstb->IvMac());
			pgstb->Delete(iv);
			}
		}

	// write list to disk
	if (!_FPrepWrite(fPack, pgstb->CbOnFile(), ctg, cno, &blck) ||
			!pgstb->FWrite(&blck, _bo, _osk) ||
			!_FEndWrite(fPack, ctg, cno, &blck))
		{
		_Error(ertOom);
		}

LFail:
	ReleasePpo(&pgstb);
	ReleasePpo(&pglivFree);
	FreePpv(&pvExtra);
}


/***************************************************************************
	Parse a script from the source file.
***************************************************************************/
void CHCM::_ParseBodyScript(bool fPack, bool fInfix, CTG ctg, CNO cno)
{
	AssertThis(0);
	SCCG sccg;
	PSCPT pscpt;

	if (pvNil == (pscpt = sccg.PscptCompileLex(_pchlx, fInfix, _pmsnkError,
			ttEndChunk)))
		{
		_Error(ertScript);
		return;
		}

	if (!pscpt->FSaveToChunk(_pcfl, ctg, cno, fPack))
		_Error(ertOom);

	ReleasePpo(&pscpt);
}


/***************************************************************************
	Parse a script from the source file.
***************************************************************************/
void CHCM::_ParseBodyPackedFile(bool *pfPacked)
{
	AssertThis(0);
	long lw, lwSwapped;
	TOK tok;

	_ParseBodyFile();
	if (_bsf.IbMac() < size(long))
		{
		_Error(ertPackedFile, PszLit("bad packed file"));
		_SkipPastTok(ttEndChunk);
		return;
		}

	_bsf.FetchRgb(0, size(long), &lw);
	lwSwapped = lw;
	SwapBytesRglw(&lwSwapped, 1);
	if (lw == klwSigPackedFile || lwSwapped == klwSigPackedFile)
		*pfPacked = fTrue;
	else if (lw == klwSigUnpackedFile || lwSwapped == klwSigUnpackedFile)
		*pfPacked = fFalse;
	else
		{
		_Error(ertPackedFile, PszLit("not a packed file"));
		_SkipPastTok(ttEndChunk);
		return;
		}

	_bsf.FReplace(pvNil, 0, 0, size(long));

	if (!_FGetCleanTok(&tok) || ttEndChunk != tok.tt)
		{
		_Error(ertNeedEndChunk);
		_SkipPastTok(ttEndChunk);
		}
}


/***************************************************************************
	Start a sub file.
***************************************************************************/
void CHCM::_StartSubFile(bool fPack, CTG ctg, CNO cno)
{
	AssertThis(0);
	CSFC csfc;

	if (pvNil == _pglcsfc && pvNil == (_pglcsfc = GL::PglNew(size(CSFC))))
		goto LFail;

	csfc.pcfl = _pcfl;
	csfc.ctg = ctg;
	csfc.cno = cno;
	csfc.fPack = FPure(fPack);

	if (!_pglcsfc->FPush(&csfc))
		goto LFail;

	if (pvNil == (_pcfl = CFL::PcflCreateTemp()))
		{
		_pglcsfc->FPop();
		_pcfl = csfc.pcfl;
LFail:
		_Error(ertOom);
		}
}


/***************************************************************************
	End a sub file.
***************************************************************************/
void CHCM::_EndSubFile(void)
{
	AssertThis(0);
	CSFC csfc;

	if (pvNil == _pglcsfc || !_pglcsfc->FPop(&csfc))
		{
		_Error(ertSyntax);
		return;
		}

	AssertPo(csfc.pcfl, 0);

	if (!FError())
		{
		long icki;
		CKI cki;
		long cbTot, cbT;
		FP fpDst;
		PFIL pfilDst = pvNil;
		bool fRet = fFalse;

		// get the size of the data
		cbTot = 0;
		for (icki = 0; _pcfl->FGetCki(icki, &cki); icki++)
			{
			if (_pcfl->CckiRef(cki.ctg, cki.cno) > 0)
				continue;
			if (!_pcfl->FWriteChunkTree(cki.ctg, cki.cno, pvNil, 0, &cbT))
				goto LFail;
			cbTot += cbT;
			}

		// setup pfilDst and fpDst for writing the chunk trees
		if (csfc.fPack)
			{
			pfilDst = FIL::PfilCreateTemp();
			fpDst = 0;
			}
		else
			{
			FLO floDst;

			// resize the chunk
			if (!csfc.pcfl->FPut(cbTot, csfc.ctg, csfc.cno))
				goto LFail;
			csfc.pcfl->FFindFlo(csfc.ctg, csfc.cno, &floDst);

			pfilDst = floDst.pfil;
			pfilDst->AddRef();
			fpDst = floDst.fp;
			}

		// write the data to (pfilDst, fpDst)
		for (icki = 0; _pcfl->FGetCki(icki, &cki); icki++)
			{
			if (_pcfl->CckiRef(cki.ctg, cki.cno) > 0)
				continue;
			if (!_pcfl->FWriteChunkTree(cki.ctg, cki.cno, pfilDst, fpDst, &cbT))
				goto LFail;
			fpDst += cbT;
			Debug( cbTot -= cbT; )
			}
		Assert(cbTot == 0, "FWriteChunkTree messed up!");

		if (csfc.fPack)
			{
			BLCK blck(pfilDst, 0, fpDst);

			// pack the data and put it in the chunk.
			blck.FPackData();
			if (!csfc.pcfl->FPutBlck(&blck, csfc.ctg, csfc.cno))
				goto LFail;
			}

		csfc.pcfl->SetForest(csfc.ctg, csfc.cno, fTrue);
		fRet = fTrue;

LFail:
		if (!fRet)
			_Error(ertOom);

		ReleasePpo(&pfilDst);
		}

	ReleasePpo(&_pcfl);
	_pcfl = csfc.pcfl;
}


/***************************************************************************
	Parse a PACKFMT command, which is used to specify the packing format
	to use.
***************************************************************************/
void CHCM::_ParsePackFmt(void)
{
	AssertThis(0);
	PHP rgphp[1];
	long cphp;
	long cfmt;

	// get the format
	ClearPb(rgphp, size(rgphp));
	if (!_FParseParenHeader(rgphp, 1, &cphp) || cphp < 1)
		{
		_Error(ertSyntax);
		return;
		}

	cfmt = rgphp[0].lw;
	if (!vpcodmUtil->FCanDo(cfmt, fTrue))
		_Error(ertBadPackFmt);
	else
		vpcodmUtil->SetCfmtDefault(cfmt);
}


/***************************************************************************
	Parse the chunk body from the source file.
***************************************************************************/
void CHCM::_ParseChunkBody(CTG ctg, CNO cno)
{
	AssertThis(0);
	TOK tok;
	BLCK blck;
	CKI cki;
	bool fFetch;
	bool fPack, fPrePacked;

	//empty the BSF
	_bsf.FReplace(pvNil, 0, 0, _bsf.IbMac());

	fFetch = fTrue;
	fPack = fPrePacked = fFalse;
	for (;;)
		{
		if (fFetch && !_FGetCleanTok(&tok))
			return;

		fFetch = fTrue;
		switch (tok.tt)
			{
		default:
			if (!_FParseData(&tok))
				{
				_Error(ertBadToken);
				_SkipPastTok(ttEndChunk);
				return;
				}
			//don't fetch next token
			fFetch = fFalse;
			break;
		case ttChild:
			_ParseBodyChild(ctg, cno);
			break;
		case ttParent:
			_ParseBodyParent(ctg, cno);
			break;
		case ttLoner:
			if (pvNil != _pglcsfc && 0 < _pglcsfc->IvMac())
				{
				_Error(ertLonerInSub);
				break;
				}
			cki.ctg = ctg;
			cki.cno = cno;
			if (pvNil == _pglckiLoner &&
					pvNil == (_pglckiLoner = GL::PglNew(size(CKI))) ||
				!_pglckiLoner->FPush(&cki))
				{
				_Error(ertOom);
				}
			break;

		case ttPrePacked:
			fPrePacked = fTrue;
			break;

		case ttPack:
			fPack = fTrue;
			break;

		case ttPackFmt:
			_ParsePackFmt();
			break;

// We're done after all the cases below
		case ttPackedFile:
			fPack = fFalse;
			_ErrorOnData(PszLit("Packed File"));
			_ParseBodyPackedFile(&fPrePacked);
			// fall thru
		case ttEndChunk:
			if (!FError())
				{
				if (!_FPrepWrite(fPack, _bsf.IbMac(), ctg, cno, &blck) ||
						!_bsf.FWriteRgb(&blck) ||
						!_FEndWrite(fPack, ctg, cno, &blck))
					{
					_Error(ertOom);
					}
				}
			_bsf.FReplace(pvNil, 0, 0, _bsf.IbMac());
			if (!FError() && fPrePacked)
				_pcfl->SetPacked(ctg, cno, fTrue);
			return;
		case ttMeta:
			_ErrorOnData(PszLit("Metafile"));
			_ParseBodyMeta(fPack, ctg, cno);
			return;
		case ttBitmap:
		case ttMask:
			_ErrorOnData(PszLit("Bitmap"));
			_ParseBodyBitmap(fPack, ttMask == tok.tt, ctg, cno);
			return;
		case ttPalette:
			_ErrorOnData(PszLit("Palette"));
			_ParseBodyPalette(fPack, ctg, cno);
			return;
		case ttMidi:
			_ErrorOnData(PszLit("Midi"));
			_ParseBodyMidi(fPack, ctg, cno);
			return;
		case ttCursor:
			_ErrorOnData(PszLit("Cursor"));
			_ParseBodyCursor(fPack, ctg, cno);
			return;
		case ttAl:
		case ttGl:
			_ErrorOnData(PszLit("List"));
			_ParseBodyList(fPack, ttAl == tok.tt, ctg, cno);
			return;
		case ttAg:
		case ttGg:
			_ErrorOnData(PszLit("Group"));
			_ParseBodyGroup(fPack, ttAg == tok.tt, ctg, cno);
			return;
		case ttAst:
		case ttGst:
			_ErrorOnData(PszLit("String Table"));
			_ParseBodyStringTable(fPack, ttAst == tok.tt, ctg, cno);
			return;
		case ttScript:
		case ttScriptP:
			_ErrorOnData(PszLit("Script"));
			_ParseBodyScript(fPack, ttScript == tok.tt, ctg, cno);
			return;
		case ttSubFile:
			_ErrorOnData(PszLit("Sub File"));
			_StartSubFile(fPack, ctg, cno);
			return;
			}
		}
}


/***************************************************************************
	Parse an adopt parenthesized header from the source file.
***************************************************************************/
void CHCM::_ParseAdopt(void)
{
	AssertThis(0);
	CTG ctgParent, ctgChild;
	CNO cnoParent, cnoChild;
	CHID chid;
	PHP rgphp[5];
	long cphp;

	ClearPb(rgphp, size(rgphp));
	if (!_FParseParenHeader(rgphp, 5, &cphp) || cphp < 4)
		{
		_Error(ertAdoptHead);
		return;
		}

	ctgParent = rgphp[0].lw;
	cnoParent = rgphp[1].lw;
	ctgChild = rgphp[2].lw;
	cnoChild = rgphp[3].lw;
	chid = rgphp[4].lw;

	// check if parent exists
	if (!_pcfl->FFind(ctgParent, cnoParent))
		{
		_Error(ertParentMissing);
		return;
		}

	// check if child exists
	if (!_pcfl->FFind(ctgChild, cnoChild))
		{
		_Error(ertChildMissing);
		return;
		}

	// check if cycle would be created
	if (_pcfl->TIsDescendent(ctgChild, cnoChild, ctgParent, cnoParent) != tNo)
		_Error(ertCycle);
	else if (!FError() && !_pcfl->FAdoptChild(ctgParent, cnoParent,
			ctgChild, cnoChild, chid, fTrue))
		{
		_Error(ertOom);
		}
}


/***************************************************************************
	Compile the given file.
***************************************************************************/
PCFL CHCM::PcflCompile(PFNI pfniSrc, PFNI pfniDst, PMSNK pmsnk)
{
	AssertThis(0);
	AssertPo(pfniSrc, ffniFile);
	AssertPo(pfniDst, ffniFile);
	AssertPo(pmsnk, 0);
	BSF bsfSrc;
	STN stnFile;
	FLO flo;
	bool fRet;

	if (pvNil == (flo.pfil = FIL::PfilOpen(pfniSrc)))
		{
		pmsnk->ReportLine(PszLit("opening source file failed"));
		return pvNil;
		}

	flo.fp = 0;
	flo.cb = flo.pfil->FpMac();
	fRet = flo.FTranslate(oskNil) && bsfSrc.FReplaceFlo(&flo, fFalse, 0, 0);
	ReleasePpo(&flo.pfil);
	if (!fRet)
		return pvNil;

	pfniSrc->GetStnPath(&stnFile);
	return PcflCompile(&bsfSrc, &stnFile, pfniDst, pmsnk);
}


/***************************************************************************
	Compile the given BSF, using initial file name given by pstnFile.
***************************************************************************/
PCFL CHCM::PcflCompile(PBSF pbsfSrc, PSTN pstnFile, PFNI pfniDst, PMSNK pmsnk)
{
	AssertThis(0);
	AssertPo(pbsfSrc, ffniFile);
	AssertPo(pstnFile, 0);
	AssertPo(pfniDst, ffniFile);
	AssertPo(pmsnk, 0);
	TOK tok;
	CTG ctg;
	CNO cno;
	PCFL pcfl;
	bool fReportBadTok;

	if (pvNil == (_pchlx = NewObj CHLX(pbsfSrc, pstnFile)))
		{
		pmsnk->ReportLine(PszLit("Memory failure"));
		return pvNil;
		}

	if (pvNil == (_pcfl = CFL::PcflCreate(pfniDst, fcflWriteEnable)))
		{
		pmsnk->ReportLine(PszLit("Couldn't create destination file"));
		ReleasePpo(&_pchlx);
		return pvNil;
		}

	_pmsnkError = pmsnk;
	_sm = smStz;
	_cbNum = size(long);
	_bo = kboCur;
	_osk = koskCur;

	fReportBadTok = fTrue;
	while (_FGetCleanTok(&tok, fTrue))
		{
		switch(tok.tt)
			{
		case ttChunk:
			_ParseChunkHeader(&ctg, &cno);
			_ParseChunkBody(ctg, cno);
			fReportBadTok = fTrue;
			break;

		case ttEndChunk:
			// ending a sub file
			_EndSubFile();
			fReportBadTok = fTrue;
			break;

		case ttAdopt:
			_ParseAdopt();
			fReportBadTok = fTrue;
			break;

		case ttPackFmt:
			_ParsePackFmt();
			fReportBadTok = fTrue;
			break;

		default:
			if (fReportBadTok)
				{
				_Error(ertNeedChunk);
				fReportBadTok = fFalse;
				}
			break;
			}

		if (_cactError > 100)
			{
			pmsnk->ReportLine(PszLit("Too many errors - compilation aborted"));
			break;
			}
		}

	// empty the BSF
	_bsf.FReplace(pvNil, 0, 0, _bsf.IbMac());

	// make sure we're not in any subfiles
	if (pvNil != _pglcsfc && _pglcsfc->IvMac() > 0)
		{
		CSFC csfc;

		_Error(ertNoEndSubFile);
		while (_pglcsfc->FPop(&csfc))
			{
			ReleasePpo(&_pcfl);
			_pcfl = csfc.pcfl;
			}
		}

	if (!FError() && pvNil != _pglckiLoner)
		{
		CKI cki;
		long icki;

		for (icki = _pglckiLoner->IvMac(); icki-- > 0; )
			{
			_pglckiLoner->Get(icki, &cki);
			_pcfl->SetLoner(cki.ctg, cki.cno, fTrue);
			}
		}

	if (!FError() && !_pcfl->FSave(kctgChkCmp, pvNil))
		_Error(ertOom);

	if (FError())
		{
		ReleasePpo(&_pcfl);
		pfniDst->FDelete();
		}
	ReleasePpo(&_pchlx);
	ReleasePpo(&_pglckiLoner);

	pcfl = _pcfl;
	_pcfl = pvNil;
	_pmsnkError = pvNil;
	return pcfl;
}


/***************************************************************************
	Keyword-tokentype mappings
***************************************************************************/
static KEYTT _rgkeytt[] =
	{
	PszLit("ITEM"), ttItem,
	PszLit("FREE"), ttFree,
	PszLit("VAR"), ttVar,
	PszLit("BYTE"), ttModeByte,
	PszLit("SHORT"), ttModeShort,
	PszLit("LONG"), ttModeLong,
	PszLit("CHUNK"), ttChunk,
	PszLit("ENDCHUNK"), ttEndChunk,
	PszLit("ADOPT"), ttAdopt,
	PszLit("CHILD"), ttChild,
	PszLit("PARENT"), ttParent,
	PszLit("BO"), ttBo,
	PszLit("OSK"), ttOsk,
	PszLit("STN"), ttModeStn,
	PszLit("STZ"), ttModeStz,
	PszLit("SZ"), ttModeSz,
	PszLit("ST"), ttModeSt,
	PszLit("ALIGN"), ttAlign,
	PszLit("FILE"), ttFile,
	PszLit("PACKEDFILE"), ttPackedFile,
	PszLit("META"), ttMeta,
	PszLit("BITMAP"), ttBitmap,
	PszLit("MASK"), ttMask,
	PszLit("MIDI"), ttMidi,
	PszLit("SCRIPT"), ttScript,
	PszLit("SCRIPTPF"), ttScriptP,
	PszLit("GL"), ttGl,
	PszLit("AL"), ttAl,
	PszLit("GG"), ttGg,
	PszLit("AG"), ttAg,
	PszLit("GST"), ttGst,
	PszLit("AST"), ttAst,
	PszLit("MACBO"), ttMacBo,
	PszLit("WINBO"), ttWinBo,
	PszLit("MACOSK"), ttMacOsk,
	PszLit("WINOSK"), ttWinOsk,
	PszLit("LONER"), ttLoner,
	PszLit("CURSOR"), ttCursor,
	PszLit("PALETTE"), ttPalette,
	PszLit("PREPACKED"), ttPrePacked,
	PszLit("PACK"), ttPack,
	PszLit("PACKFMT"), ttPackFmt,
	PszLit("SUBFILE"), ttSubFile,
	};

#define kckeytt (size(_rgkeytt)/size(_rgkeytt[0]))


/***************************************************************************
	Constructor for the chunky compiler lexer.
***************************************************************************/
CHLX::CHLX(PBSF pbsf, PSTN pstnFile) : CHLX_PAR(pbsf, pstnFile)
{
	_pgstVariables = pvNil;
	AssertThis(0);
}


/***************************************************************************
	Destructor for the chunky compiler lexer.
***************************************************************************/
CHLX::~CHLX(void)
{
	ReleasePpo(&_pgstVariables);
}


/***************************************************************************
	Reads in the next token.  Resolves certain names to keyword tokens.
***************************************************************************/
bool CHLX::FGetTok(PTOK ptok)
{
	AssertThis(0);
	AssertVarMem(ptok);
	long ikeytt;
	long istn;

	for (;;)
		{
		if (!CHLX_PAR::FGetTok(ptok))
			return fFalse;

		if (ttName != ptok->tt)
			return fTrue;

		//check for a keyword
		for (ikeytt = 0; ikeytt < kckeytt; ikeytt++)
			{
			if (ptok->stn.FEqualSz(_rgkeytt[ikeytt].pszKeyword))
				{
				ptok->tt = _rgkeytt[ikeytt].tt;
				return fTrue;
				}
			}

		//if the token isn't SET, check for a variable
		if (!ptok->stn.FEqualSz(PszLit("SET")))
			{
			//check for a variable
			if (pvNil != _pgstVariables &&
					_pgstVariables->FFindStn(&ptok->stn, &istn, fgstSorted))
				{
				ptok->tt = ttLong;
				_pgstVariables->GetExtra(istn, &ptok->lw);
				}
			break;
			}

		//handle a SET
		if (!_FDoSet(ptok))
			{
			ptok->tt = ttError;
			break;
			}
		}

	return fTrue;
}


/***************************************************************************
	Reads in the next token.  Skips semicolons and commas.
***************************************************************************/
bool CHLX::FGetTokSkipSemi(PTOK ptok)
{
	AssertThis(0);
	AssertVarMem(ptok);

	// skip comma and semicolon separators
	while (FGetTok(ptok))
		{
		if (ttComma != ptok->tt && ttSemi != ptok->tt)
			return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	Reads a path and builds an FNI.
***************************************************************************/
bool CHLX::FGetPath(FNI *pfni)
{
	AssertThis(0);
	AssertPo(pfni, 0);
	achar ch;
	STN stn;

	if (!_FSkipWhiteSpace())
		{
		_fSkipToNextLine = fTrue;
		return fFalse;
		}

	if (!_FFetchRgch(&ch) || '"' != ch)
		return fFalse;
	_Advance();

	stn.SetNil();
	for (;;)
		{
		if (!_FFetchRgch(&ch))
			return fFalse;
		_Advance();
		if ('"' == ch)
			break;
		if (kchReturn == ch)
			return fFalse;
		stn.FAppendCh(ch);
		}

#ifdef WIN
	static achar _szInclude[1024];
	SZ szT;

	if (_szInclude[0] == 0)
		{
		GetEnvironmentVariable(PszLit("include"), _szInclude,
			CvFromRgv(_szInclude) - 1);
		}

	if (0 != SearchPath(_szInclude, stn.Psz(), pvNil, kcchMaxSz, szT, pvNil))
		stn = szT;
	return pfni->FBuildFromPath(&stn);
#endif //WIN
#ifdef MAC
	return pfni->FBuild(0, 0, &stn, kftgText);
#endif //MAC
}


/***************************************************************************
	Handle a set command.
***************************************************************************/
bool CHLX::_FDoSet(PTOK ptok)
{
	AssertThis(0);
	AssertVarMem(ptok);
	long tt;
	long lw;
	long istn;
	bool fNegate;

	if (!CHLX_PAR::FGetTok(ptok) || ttName != ptok->tt)
		return fFalse;

	lw = 0;
	istn = ivNil;
	if (pvNil != _pgstVariables ||
			pvNil != (_pgstVariables = GST::PgstNew(size(long))))
		{
		if (_pgstVariables->FFindStn(&ptok->stn, &istn, fgstSorted))
			_pgstVariables->GetExtra(istn, &lw);
		else if (!_pgstVariables->FInsertStn(istn, &ptok->stn, &lw))
			istn = ivNil;
		}

	if (!CHLX_PAR::FGetTok(ptok))
		return fFalse;

	switch (ptok->tt)
		{
	case ttInc:
		lw++;
		break;
	case ttDec:
		lw--;
		break;

	case ttAssign:
	case ttAAdd:
	case ttASub:
	case ttAMul:
	case ttADiv:
	case ttAMod:
	case ttABOr:
	case ttABAnd:
	case ttABXor:
	case ttAShr:
	case ttAShl:
		tt = ptok->tt;
		fNegate = fFalse;
		for (;;)
			{
			if (!CHLX_PAR::FGetTok(ptok))
				return fFalse;
			if (ttLong == ptok->tt)
				break;
			if (ttName == ptok->tt)
				{
				long istnT;
				if (!_pgstVariables->FFindStn(&ptok->stn, &istnT, fgstSorted))
					return fFalse;
				_pgstVariables->GetExtra(istnT, &ptok->lw);
				ptok->tt = ttLong;
				break;
				}
			if (ttSub != ptok->tt)
				return fFalse;
			fNegate = !fNegate;
			}
		if (fNegate)
			ptok->lw = -ptok->lw;

		switch (tt)
			{
		case ttAssign:
			lw = ptok->lw;
			break;
		case ttAAdd:
			lw += ptok->lw;
			break;
		case ttASub:
			lw -= ptok->lw;
			break;
		case ttAMul:
			lw *= ptok->lw;
			break;
		case ttADiv:
			if (ptok->lw == 0)
				return fFalse;
			lw /= ptok->lw;
			break;
		case ttAMod:
			if (ptok->lw == 0)
				return fFalse;
			lw %= ptok->lw;
			break;
		case ttABOr:
			lw |= ptok->lw;
			break;
		case ttABAnd:
			lw &= ptok->lw;
			break;
		case ttABXor:
			lw ^= ptok->lw;
			break;
		case ttAShr:
			// do logical shift
			lw = (ulong)lw >> ptok->lw;
			break;
		case ttAShl:
			lw <<= ptok->lw;
			break;
			}
		break;

	default:
		return fFalse;
		}

	if (ivNil != istn)
		_pgstVariables->PutExtra(istn, &lw);
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert that the CHLX is a valid object.
***************************************************************************/
void CHLX::AssertValid(ulong grf)
{
	CHLX_PAR::AssertValid(grf);
	AssertNilOrPo(_pgstVariables, 0);
}


/***************************************************************************
	Mark memory for the CHLX object.
***************************************************************************/
void CHLX::MarkMem(void)
{
	AssertValid(0);
	CHLX_PAR::MarkMem();
	MarkMemObj(_pgstVariables);
}
#endif


/***************************************************************************
	Constructor for the CHDC class. This is the chunky decompiler.
***************************************************************************/
CHDC::CHDC(void)
{
	_ert = ertNil;
	_pcfl = pvNil;
	AssertThis(0);
}


/***************************************************************************
	Destructor for the CHDC class.
***************************************************************************/
CHDC::~CHDC(void)
{
	ReleasePpo(&_pcfl);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a CHDC.
***************************************************************************/
void CHDC::AssertValid(ulong grf)
{
	CHDC_PAR::AssertValid(0);
	AssertNilOrPo(_pcfl, 0);
	AssertPo(&_bsf, 0);
	AssertPo(&_chse, 0);
}


/***************************************************************************
	Mark memory for the CHDC.
***************************************************************************/
void CHDC::MarkMem(void)
{
	AssertValid(0);
	CHDC_PAR::MarkMem();
	MarkMemObj(&_bsf);
	MarkMemObj(&_chse);
}
#endif //DEBUG


/***************************************************************************
	Decompile a chunky file.
***************************************************************************/
bool CHDC::FDecompile(PCFL pcflSrc, PMSNK pmsnk, PMSNK pmsnkError)
{
	AssertThis(0);
	AssertPo(pcflSrc, 0);
	long icki, ikid, ckid;
	CTG ctg;
	CKI cki;
	KID kid;
	BLCK blck;

	_pcfl = pcflSrc;
	_ert = ertNil;
	_chse.Init(pmsnk, pmsnkError);

	_bo = kboCur;
	_osk = koskCur;

	_chse.DumpSz(PszLit("BYTE"));
	_chse.DumpSz(PszLit(""));
	for (icki = 0; _pcfl->FGetCki(icki, &cki, pvNil, &blck); icki++)
		{
		STN stnName;

		// don't dump these, because they're embedded in the script
		if (cki.ctg == kctgScriptStrs)
			continue;

		_pcfl->FGetName(cki.ctg, cki.cno, &stnName);
		_chse.DumpHeader(cki.ctg, cki.cno, &stnName);

		// look for special CTGs
		ctg = cki.ctg;

		//handle 4 character ctg's
		switch (ctg)
			{
		case kctgScript:
			if (_FDumpScript(&cki))
				goto LEndChunk;
			_pcfl->FGetCki(icki, &cki, pvNil, &blck);
			break;
			}

		//handle 3 character ctg's
		ctg = ctg & 0xFFFFFF00L | 0x00000020L;
		switch (ctg)
			{
		case kctgGst:
		case kctgAst:
			if (_FDumpStringTable(&blck, kctgAst == ctg))
				goto LEndChunk;
			_pcfl->FGetCki(icki, &cki, pvNil, &blck);
			break;
			}

		//handle 2 character ctg's
		ctg = ctg & 0xFFFF0000L | 0x00002020L;
		switch (ctg)
			{
		case kctgGl:
		case kctgAl:
			if (_FDumpList(&blck, kctgAl == ctg))
				goto LEndChunk;
			_pcfl->FGetCki(icki, &cki, pvNil, &blck);
			break;
		case kctgGg:
		case kctgAg:
			if (_FDumpGroup(&blck, kctgAg == ctg))
				goto LEndChunk;
			_pcfl->FGetCki(icki, &cki, pvNil, &blck);
			break;
			}

		_chse.DumpBlck(&blck);

LEndChunk:
		_chse.DumpSz(PszLit("ENDCHUNK"));
		_chse.DumpSz(PszLit(""));
		}

	// now output parent-child relationships
	for (icki = 0; _pcfl->FGetCki(icki++, &cki, &ckid); )
		{
		for (ikid = 0; ikid < ckid; )
			{
			AssertDo(_pcfl->FGetKid(cki.ctg, cki.cno, ikid++, &kid), 0);
			if (kid.cki.ctg == kctgScriptStrs && cki.ctg == kctgScript)
				continue;
			_chse.DumpAdoptCmd(&cki, &kid);
			}
		}
	_pcfl = pvNil;
	_chse.Uninit();

	return !FError();
}


/***************************************************************************
	Disassemble the script and dump it.
***************************************************************************/
bool CHDC::_FDumpScript(CKI *pcki)
{
	AssertThis(0);
	AssertVarMem(pcki);
	PSCPT pscpt;
	bool fRet;
	SCCG sccg;
	long cfmt;
	bool fPacked;
	BLCK blck;

	_pcfl->FFind(pcki->ctg, pcki->cno, &blck);
	fPacked = blck.FPacked(&cfmt);

	if (pvNil == (pscpt = SCPT::PscptRead(_pcfl, pcki->ctg, pcki->cno)))
		return fFalse;

	if (fPacked)
		_WritePack(cfmt);

	fRet = _chse.FDumpScript(pscpt, &sccg);

	ReleasePpo(&pscpt);

	return fRet;
}


/***************************************************************************
	Try to read the chunk as a list and dump it out.  If the chunk isn't
	a list, return false so it can be dumped in hex.
***************************************************************************/
bool CHDC::_FDumpList(PBLCK pblck, bool fAl)
{
	AssertThis(0);
	AssertPo(pblck, fblckReadable);

	PGLB pglb;
	short bo, osk;
	long cfmt;
	bool fPacked = pblck->FPacked(&cfmt);

	pglb = fAl ? (PGLB)AL::PalRead(pblck, &bo, &osk) :
		(PGLB)GL::PglRead(pblck, &bo, &osk);
	if (pvNil == pglb)
		return fFalse;

	if (fPacked)
		_WritePack(cfmt);

	if (bo != _bo)
		{
		_chse.DumpSz(MacWin(bo != kboCur, bo == kboCur) ?
			PszLit("WINBO") : PszLit("MACBO"));
		_bo = bo;
		}
	if (osk != _osk)
		{
		_chse.DumpSz(osk == koskWin ? PszLit("WINOSK") : PszLit("MACOSK"));
		_osk = osk;
		}

	_chse.DumpList(pglb);
	ReleasePpo(&pglb);

	return fTrue;
}

/***************************************************************************
	Try to read the chunk as a group and dump it out.  If the chunk isn't
	a group, return false so it can be dumped in hex.
***************************************************************************/
bool CHDC::_FDumpGroup(PBLCK pblck, bool fAg)
{
	AssertThis(0);
	AssertPo(pblck, fblckReadable);

	PGGB pggb;
	short bo, osk;
	long cfmt;
	bool fPacked = pblck->FPacked(&cfmt);

	pggb = fAg ? (PGGB)AG::PagRead(pblck, &bo, &osk) :
		(PGGB)GG::PggRead(pblck, &bo, &osk);
	if (pvNil == pggb)
		return fFalse;

	if (fPacked)
		_WritePack(cfmt);

	if (bo != _bo)
		{
		_chse.DumpSz(MacWin(bo != kboCur, bo == kboCur) ?
			PszLit("WINBO") : PszLit("MACBO"));
		_bo = bo;
		}
	if (osk != _osk)
		{
		_chse.DumpSz(osk == koskWin ? PszLit("WINOSK") : PszLit("MACOSK"));
		_osk = osk;
		}

	_chse.DumpGroup(pggb);
	ReleasePpo(&pggb);

	return fTrue;
}

/***************************************************************************
	Try to read the chunk as a string table and dump it out.  If the chunk
	isn't a string table, return false so it can be dumped in hex.
***************************************************************************/
bool CHDC::_FDumpStringTable(PBLCK pblck, bool fAst)
{
	AssertThis(0);
	AssertPo(pblck, fblckReadable);

	PGSTB pgstb;
	short bo, osk;
	long cfmt;
	bool fPacked = pblck->FPacked(&cfmt);
	bool fRet;

	pgstb = fAst ? (PGSTB)AST::PastRead(pblck, &bo, &osk) :
		(PGSTB)GST::PgstRead(pblck, &bo, &osk);
	if (pvNil == pgstb)
		return fFalse;

	if (fPacked)
		_WritePack(cfmt);

	if (bo != _bo)
		{
		_chse.DumpSz(MacWin(bo != kboCur, bo == kboCur) ?
			PszLit("WINBO") : PszLit("MACBO"));
		_bo = bo;
		}
	if (osk != _osk)
		{
		_chse.DumpSz(osk == koskWin ? PszLit("WINOSK") : PszLit("MACOSK"));
		_osk = osk;
		}

	fRet = _chse.FDumpStringTable(pgstb);
	ReleasePpo(&pgstb);

	return fRet;
}


/***************************************************************************
	Write out the PACKFMT and PACK commands
***************************************************************************/
void CHDC::_WritePack(long cfmt)
{
	AssertThis(0);
	STN stn;

	if (cfmtNil == cfmt)
		_chse.DumpSz(PszLit("PACK"));
	else
		{
		stn.FFormatSz(PszLit("PACKFMT (0x%x) PACK"), cfmt);
		_chse.DumpSz(stn.Psz());
		}
}

