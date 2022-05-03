/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Windows specific picture (metafile) routines.

***************************************************************************/
#include "frame.h"
ASSERTNAME


/***************************************************************************
	Constructor for a picture.
***************************************************************************/
PIC::PIC(void)
{
	_hpic = hNil;
	_rc.Zero();
}


/***************************************************************************
	Destructor for a picture.
***************************************************************************/
PIC::~PIC(void)
{
	AssertBaseThis(0);
	if (hNil != _hpic)
		DeleteEnhMetaFile(_hpic);
}


/***************************************************************************
	Read a picture from a chunky file.  This routine only reads or converts
	OS specific representations with the given chid value.
***************************************************************************/
PPIC PIC::PpicFetch(PCFL pcfl, CTG ctg, CNO cno, CHID chid)
{
	AssertPo(pcfl, 0);
	BLCK blck;
	KID kid;

	if (!pcfl->FFind(ctg, cno))
		return pvNil;
	if (pcfl->FGetKidChidCtg(ctg, cno, chid, kctgMeta, &kid) &&
			pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		{
		return PpicRead(&blck);
		}

	//REVIEW shonk: convert another type to a MetaFile...
	return pvNil;
}


/***************************************************************************
	Read a picture from a chunky file.  This routine only reads a system
	specific pict (Mac PICT or Windows MetaFile) and its header.
***************************************************************************/
PPIC PIC::PpicRead(PBLCK pblck)
{
	AssertPo(pblck, fblckReadable);
	HPIC hpic;
	HQ hq;
	PICH *ppich;
	PPIC ppic;
	RC rc;
	long cb;

	if (!pblck->FUnpackData())
		return pvNil;

	cb = pblck->Cb();
	if (cb <= size(PICH))
		return pvNil;

	if (hqNil == (hq = pblck->HqFree()))
		return pvNil;

	ppich = (PICH *)PvLockHq(hq);
	rc = ppich->rc;
	if (rc.FEmpty() || ppich->cb != cb)
		hpic = hNil;
	else
		hpic = SetEnhMetaFileBits(cb - size(PICH), (byte *)(ppich + 1));
	UnlockHq(hq);
	FreePhq(&hq);
	if (hNil == hpic)
		return pvNil;

	if (pvNil == (ppic = NewObj PIC))
		{
		DeleteEnhMetaFile(hpic);
		return pvNil;
		}

	ppic->_hpic = hpic;
	ppic->_rc = rc;
	AssertPo(ppic, 0);
	return ppic;
}


/***************************************************************************
	Return the total size on file.
***************************************************************************/
long PIC::CbOnFile(void)
{
	AssertThis(0);
	return GetEnhMetaFileBits(_hpic, 0, pvNil) + size(PICH);
}


/***************************************************************************
	Write the meta file (and its header) to the given BLCK.
***************************************************************************/
bool PIC::FWrite(PBLCK pblck)
{
	AssertThis(0);
	AssertPo(pblck, 0);
	long cb, cbTot;
	bool fT;
	PICH *ppich;

	cb = GetEnhMetaFileBits(_hpic, 0, pvNil);
	if (cb == 0 || (cbTot = cb + size(PICH)) != pblck->Cb())
		return fFalse;

	if (!FAllocPv((void **)&ppich, cbTot, fmemNil, mprNormal))
		return fFalse;
	ppich->rc = _rc;
	ppich->cb = cbTot;
	fT = (GetEnhMetaFileBits(_hpic, cb, (byte *)(ppich + 1)) == (ulong)cb) &&
		pblck->FWrite(ppich);
	FreePpv((void **)&ppich);
	return fT;
}


/***************************************************************************
	Static method to read the file as a native picture (EMF or WMF file).
***************************************************************************/
PPIC PIC::PpicReadNative(FNI *pfni)
{
	AssertPo(pfni, ffniFile);
	HPIC hpic;
	PPIC ppic;
	RC rc;
	ENHMETAHEADER emh;
	STN stn;

	switch (pfni->Ftg())
		{
	default:
		return pvNil;

	case kftgMeta:
		hpic = _HpicReadWmf(pfni);
		break;

	case kftgEnhMeta:
		pfni->GetStnPath(&stn);
		hpic = GetEnhMetaFile(stn.Psz());
		break;
		}

	if (hNil == hpic)
		return pvNil;

	if (pvNil == (ppic = NewObj PIC))
		{
		DeleteEnhMetaFile(hpic);
		return pvNil;
		}

	GetEnhMetaFileHeader(hpic, size(emh), &emh);
	rc.Set(
		LwMulDiv(emh.rclFrame.left, 72, 2540),
		LwMulDiv(emh.rclFrame.top, 72, 2540),
		LwMulDiv(emh.rclFrame.right, 72, 2540),
		LwMulDiv(emh.rclFrame.bottom, 72, 2540));

	ppic->_hpic = hpic;
	ppic->_rc = rc;
	AssertPo(ppic, 0);
	return ppic;
}



/* placeable metafile data definitions */
#define lwMEFH 0x9AC6CDD7

/* placeable metafile header */
typedef struct _MEFH
	{
	DWORD lwKey;
	short w1;
	short xpLeft;
	short ypTop;
	short xpRight;
	short ypBottom;
	WORD w2;
	DWORD dw1;
	WORD w3;
	} MEFH;


/***************************************************************************
	Static method to read an old style WMF file.
***************************************************************************/
HPIC PIC::_HpicReadWmf(FNI *pfni)
{
	MEFH mefh;
	METAHEADER mh;
	HPIC hpic;
	long lw;
	long cb;
	PFIL pfil;
	void *pv;
	bool fT;
	FP fp;

	const long kcbMefh = 22;
	const long kcbMetaHeader  = 18;

	if (pvNil == (pfil = FIL::PfilOpen(pfni)))
		return hNil;

	// check for type of meta file
	if (!pfil->FReadRgb(&lw, size(long), 0))
		goto LFail;

	// read placeable meta file header - NOTE: we can't just use size(MEFH) for
	// the cb because the MEFH is padded to a long boundary by the compiler
	fp = 0;
	if (lw == lwMEFH && !pfil->FReadRgbSeq(&mefh, kcbMefh, &fp))
		goto LFail;

	// read METAHEADER structure - NOTE: we can't just use size(METAHEADER) for
	// the cb because the METAHEADER is padded to a long boundary by the
	// compiler
	if (!pfil->FReadRgbSeq(&mh, kcbMetaHeader, &fp) || mh.mtVersion < 0x0300 ||
			2 * mh.mtHeaderSize != kcbMetaHeader ||
			!FIn(cb = 2 * mh.mtSize, kcbMetaHeader + 1, kcbMax))
		{
		goto LFail;
		}

	if (!FAllocPv(&pv, cb, fmemNil, mprNormal))
		{
LFail:
		ReleasePpo(&pfil);
		return hNil;
		}

	*(METAHEADER *)pv = mh;
	fT = pfil->FReadRgbSeq(PvAddBv(pv, kcbMetaHeader), cb - kcbMetaHeader, &fp);
	ReleasePpo(&pfil);

	if (!fT)
		{
		FreePpv(&pv);
		return hNil;
		}

	//convert the old style metafile to an enhanced metafile
	hpic = SetWinMetaFileBits(cb, (byte *)pv, hNil, pvNil);
	FreePpv(&pv);

	return hpic;
}
