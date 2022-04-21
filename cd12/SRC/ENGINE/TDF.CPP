/***************************************************************************

	tdf.cpp: Three-D Font class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!


	TDFs (3-D Fonts) are simply collections of models, one model per ASCII
	character.	The TDF class holds general font information such as the
	count of characters in the font and the maximum height of the
	characters.  It also holds an array of widths and heights of every
	character, to allow proportional spacing.  Fetching a letter's model
	from a TDF involves	looking for a child chunk of the TDF chunk with a
	CHID equal to the ASCII value of the desired character:

	TDF  // Contains font info (width and height of characters)
	 |
	 +---BMDL (chid 0) // MODL for ASCII character 0
	 |
	 +---BMDL (chid 1) // MODL for ASCII character 1
     .
	 .
	 .

***************************************************************************/
#include "soc.h"
ASSERTNAME

RTCLASS(TDF)

const long kcchTdfDefault = 256; // for size estimates and authoring
const BRS kdxrSpacing = BR_SCALAR(0.0); // horizontal space between chars
const BRS kdyrLeading = BR_SCALAR(0.5); // vertical space between chars


/****************************************
	3-D Font On File
****************************************/
struct TDFF
	{
	short bo;
	short osk;
	long cch;
	BRS dyrMax;
// These variable-length arrays follow the TDFF in the TDF chunk
//  BRS rgdxr[cch];
//  BRS rgdyr[cch];
	};
const BOM kbomTdff = 0x5F000000; // don't forget to swap rgdxr & rgdyr!


/***************************************************************************
	A PFNRPO to read a TDF from a file.
***************************************************************************/
bool TDF::FReadTdf(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck, PBACO *ppbaco,
	long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, 0);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);

	TDF *ptdf;

	// Estimate TDF size in memory.
	if (pblck->FPacked())
		*pcb = size(TDF) + LwMul(kcchTdfDefault, size(BRS) + size(BRS));
	else
		*pcb = pblck->Cb();
	if (pvNil == ppbaco)
		return fTrue;
	ptdf = NewObj TDF;
	if (pvNil == ptdf || !ptdf->_FInit(pblck))
		{
		TrashVar(ppbaco);
		TrashVar(pcb);
		ReleasePpo(&ptdf);
		return fFalse;
		}
	AssertPo(ptdf, 0);
	*pcb = size(TDF) + LwMul(ptdf->_cch, size(BRS) + size(BRS));
	*ppbaco = ptdf;
	return fTrue;
}


/***************************************************************************
	Initialize the font.  Does not clean up on failure because the 
	destructor will.
***************************************************************************/
bool TDF::_FInit(PBLCK pblck)
{
	AssertBaseThis(0);
	AssertPo(pblck, 0);

	TDFF tdff;
	long cbrgdwr; // space taken by rgdxr or rgdyr

	if (!pblck->FUnpackData())
		return fFalse;
	if (pblck->Cb() < size(TDFF))
		{
		PushErc(ercSocBadTdf);
		return fFalse;
		}
	if (!pblck->FReadRgb(&tdff, size(TDFF), 0))
		return fFalse;
	if (kboCur != tdff.bo)
		SwapBytesBom(&tdff, kbomTdff);
	Assert(kboCur == tdff.bo, "bad TDFF");
	_cch = tdff.cch;
	cbrgdwr = LwMul(_cch, size(BRS));
	if (pblck->Cb() != size(TDFF) + cbrgdwr + cbrgdwr)
		{
		PushErc(ercSocBadTdf);
		return fFalse;
		}
	_dyrMax = tdff.dyrMax;

	// Read _prgdxr
	if (!FAllocPv((void **)&_prgdxr, cbrgdwr, fmemNil, mprNormal))
		return fFalse;
	if (!pblck->FReadRgb(_prgdxr, cbrgdwr, size(TDFF)))
		return fFalse;
	AssertBomRglw(kbomBrs, size(BRS));
	if (kboCur != tdff.bo)
		SwapBytesRglw(_prgdxr, _cch);

	// Read _prgdyr
	if (!FAllocPv((void **)&_prgdyr, cbrgdwr, fmemNil, mprNormal))
		return fFalse;
	if (!pblck->FReadRgb(_prgdyr, cbrgdwr, size(TDFF) + cbrgdwr))
		return fFalse;
	AssertBomRglw(kbomBrs, size(BRS));
	if (kboCur != tdff.bo)
		SwapBytesRglw(_prgdyr, _cch);

	return fTrue;
}


/***************************************************************************
	TDF destructor
***************************************************************************/
TDF::~TDF(void)
{
	AssertBaseThis(0);

	FreePpv((void **)&_prgdxr);
	FreePpv((void **)&_prgdyr);
}


/***************************************************************************
	This authoring-only API creates a new TDF chunk in pcrf, with child
	models as specified in pglkid.  This function does not create a new
	TDF instance in memory...to do that, call FReadTdf with the values 
	returned in pckiTdf.
***************************************************************************/
bool TDF::FCreate(PCRF pcrf, PGL pglkid, STN *pstn, CKI *pckiTdf)
{
	AssertPo(pcrf, 0);
	AssertPo(pglkid, 0);
	AssertPo(pstn, 0);
	AssertNilOrVarMem(pckiTdf);

	CKI ckiTdf;
	KID kid;
	KID kid2;
	TDFF tdff;
	BRS *prgdxr = pvNil;
	BRS *prgdyr = pvNil;
	PMODL pmodl;
	BLCK blck;
	long cbrgdwr;	// space taken by rgdxr or rgdyr
	long ikid;
	long ckid;
	CHID chidMax = 0;
	long ikidLetteri = -1;

	// Find chidMax
	ckid = pglkid->IvMac();
	for (ikid = 0; ikid < ckid; ikid++)
		{
		pglkid->Get(ikid, &kid);
		if (kid.chid > chidMax)
			chidMax = kid.chid;
		if (kid.chid == (CHID)ChLit('i'))
			ikidLetteri = ikid;
		}

	tdff.bo = kboCur;
	tdff.osk = koskCur;
	tdff.cch = chidMax + 1;
	tdff.dyrMax = rZero;
	cbrgdwr = LwMul(tdff.cch, size(BRS));
	if (!FAllocPv((void **)&prgdxr, cbrgdwr, fmemClear, mprNormal))
		goto LFail;
	if (!FAllocPv((void **)&prgdyr, cbrgdwr, fmemClear, mprNormal))
		goto LFail;

	// Create the TDF chunk
	ckiTdf.ctg = kctgTdf;
	if (!pcrf->Pcfl()->FAdd(size(TDFF) + cbrgdwr + cbrgdwr, ckiTdf.ctg, 
		&ckiTdf.cno, &blck))
		{
		goto LFail;
		}

	// Add the BMDL kids and remember widths, heights, and maximum height
	for (ikid = 0; ikid < ckid; ikid++)
		{
		pglkid->Get(ikid, &kid);
		pmodl = (PMODL)pcrf->PbacoFetch(kid.cki.ctg, kid.cki.cno,
			MODL::FReadModl);
		if (pmodl == pvNil)
			goto LFail;
		if (!pcrf->Pcfl()->FAdoptChild(ckiTdf.ctg, ckiTdf.cno, kid.cki.ctg, 
			kid.cki.cno, kid.chid))
			{
			goto LFail;
			}
		if (pmodl->Dxr() == 0 && kid.chid == (CHID)ChLit(' ') && 
			ikidLetteri != -1)
			{
			// Hack to turn null models into space characters:
			// space is the width and height of an "i" 
			ReleasePpo(&pmodl);
			pglkid->Get(ikidLetteri, &kid2);
			pmodl = (PMODL)pcrf->PbacoFetch(kid2.cki.ctg, kid2.cki.cno,
				MODL::FReadModl);
			if (pvNil == pmodl)
				goto LFail;
			}
		prgdxr[kid.chid] = pmodl->Dxr() + kdxrSpacing;
		prgdyr[kid.chid] = pmodl->Dyr() + kdyrLeading;
		if (prgdyr[kid.chid] > tdff.dyrMax)
			tdff.dyrMax = prgdyr[kid.chid];
		ReleasePpo(&pmodl);
	
		}
	if (!blck.FWriteRgb(&tdff, size(TDFF), 0))
		goto LFail;
	if (!blck.FWriteRgb(prgdxr, cbrgdwr, size(TDFF)))
		goto LFail;
	if (!blck.FWriteRgb(prgdyr, cbrgdwr, size(TDFF) + cbrgdwr))
		goto LFail;
	FreePpv((void **)&prgdxr);	
	FreePpv((void **)&prgdyr);	
	if (pvNil != pckiTdf)
		*pckiTdf = ckiTdf;
	return fTrue;
LFail:
	FreePpv((void **)&prgdxr);	
	FreePpv((void **)&prgdyr);	
	TrashVar(pckiTdf);
	return fFalse;
}


/***************************************************************************
	Get a model for a character from the font.  The chid is equal to the
	ASCII (or Unicode) value of the desired character.
***************************************************************************/
PMODL TDF::PmodlFetch(CHID chid)
{
	AssertThis(0);

	KID kid;

	if (!Pcrf()->Pcfl()->FGetKidChid(Ctg(), Cno(), chid, &kid))
		{
		STN stn;
		stn.FFormatSz(PszLit("Couldn't find BMDL for 3-D Font with chid %d."),
			chid);
		Warn(stn.Psz());
		PushErc(ercSocNoModlForChar);
		return pvNil;
		}
	return (PMODL)Pcrf()->PbacoFetch(kid.cki.ctg, kid.cki.cno,
		MODL::FReadModl);
}


/***************************************************************************
	Return the width of the given character
***************************************************************************/
BRS TDF::DxrChar(long ich)
{
	AssertThis(0);
	AssertIn(ich, 0, _cch);

	return _prgdxr[ich];
}


/***************************************************************************
	Return the height of the given character
***************************************************************************/
BRS TDF::DyrChar(long ich)
{
	AssertThis(0);
	AssertIn(ich, 0, _cch);

	return _prgdyr[ich];
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the TDF.
***************************************************************************/
void TDF::AssertValid(ulong grf)
{
	TDF_PAR::AssertValid(fobjAllocated);
	AssertIn(_cch, 0, klwMax);
	AssertIn(_dyrMax, 0, BR_SCALAR_MAX);
	AssertPvCb(_prgdxr, LwMul(_cch, size(BRS)));
	AssertPvCb(_prgdyr, LwMul(_cch, size(BRS)));
}


/***************************************************************************
	Mark memory used by the TDF
***************************************************************************/
void TDF::MarkMem(void)
{
	AssertThis(0);
	TDF_PAR::MarkMem();
	MarkPv(_prgdxr);
	MarkPv(_prgdyr);
}
#endif //DEBUG
