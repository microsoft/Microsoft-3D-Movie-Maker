/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Macintosh picture routines.

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
		KillPicture(_hpic);
}


/***************************************************************************
	Read a picture from a chunky file.  This routine only reads or converts
	OS specific representations with the given chid value.
***************************************************************************/
PPIC PIC::PpicFetch(PCFL pcfl, CTG ctg, CNO cno, CHID chid)
{
	AssertPo(pcfl, 0);
	KID kid;
	BLCK blck;

	if (!pcfl->FFind(ctg, cno))
		return pvNil;
	if (pcfl->FGetKidChidCtg(ctg, cno, chid, kctgMacPict, &kid) &&
			pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		{
		return PpicRead(&blck);
		}

	//REVIEW shonk: convert another type to a Mac Pict...
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
	PICH pich;
	PPIC ppic;
	RC rc;
	bool fT;

	if (!pblck->FUnpackData() || pblck->Cb() <= size(PICH) + size(Picture) ||
			!pblck->FReadRgb(&pich, size(PICH), 0) ||
			pich.rc.FEmpty() || pich.cb != pblck->Cb())
		{
		return pvNil;
		}
	if (hNil == (hpic = (HPIC)NewHandle(pich.cb - size(PICH))))
		return pvNil;
	HLock((HN)hpic);
	fT = pblck->FReadRgb(*hpic, pich.cb - size(PICH), size(PICH));
	HUnlock((HN)hpic);
	if (!fT || pvNil == (ppic = NewObj PIC))
		{
		KillPicture(hpic);
		return pvNil;
		}

	ppic->_hpic = hpic;
	ppic->_rc = pich.rc;
	AssertPo(ppic, 0);
	return ppic;
}


/***************************************************************************
	Return the total size on file.
***************************************************************************/
long PIC::CbOnFile(void)
{
	AssertThis(0);
	return GetHandleSize((HN)_hpic) + size(PICH);
}


/***************************************************************************
	Write the meta file (and its header) to the given BLCK.
***************************************************************************/
bool PIC::FWrite(PBLCK pblck)
{
	AssertThis(0);
	AssertPo(pblck, 0);
	long cb;
	bool fT;
	PICH pich;
	achar ch;

	cb = GetHandleSize((HN)_hpic);
	if ((pich.cb = cb + size(PICH)) != pblck->Cb())
		return fFalse;

	ch = HGetState((HN)_hpic);
	HLock((HN)_hpic);
	pich.rc = _rc;
	fT = pblck->FWriteRgb(&pich, size(PICH), 0) &&
		pblck->FWriteRgb(*_hpic, cb, size(PICH));
	HSetState((HN)_hpic, ch);
	return fT;
}


/***************************************************************************
	Static method to read the file as a native picture (PICT file on Mac).
***************************************************************************/
PPIC PIC::PpicReadNative(FNI *pfni)
{
	AssertPo(pfni, ffniFile);
	PFIL pfil;
	FP fpMac;
	HPIC hpic;
	PPIC ppic;
	bool fT;
	RCS rcs;

	if (pfni->Ftg() != kftgPict ||
			pvNil == (pfil = FIL::PfilOpen(pfni)))
		{
		return pvNil;
		}

	if (512 + size(Picture) >= (fpMac = pfil->FpMac()))
		goto LFail;

	if (hNil == (hpic = (HPIC)NewHandle(fpMac - 512)))
		{
LFail:
		ReleasePpo(&pfil);
		return pvNil;
		}

	HLock((HN)hpic);
	fT = pfil->FReadRgb(*hpic, fpMac - 512, 512);
	rcs = (*hpic)->picFrame;
	HUnlock((HN)hpic);
	ReleasePpo(&pfil);
	if (!fT || pvNil == (ppic = NewObj PIC))
		{
		DisposHandle((HN)hpic);
		return pvNil;
		}

	ppic->_hpic = hpic;
	ppic->_rc = RC(rcs);
	AssertPo(ppic, 0);
	return ppic;
}
