/***************************************************************************
	Author: PeteD
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Chunky source emitter class implementation.

***************************************************************************/
#include "util.h"
ASSERTNAME

RTCLASS(CHSE)


/***************************************************************************
	Constructor for a chunky source emitter.
***************************************************************************/
CHSE::CHSE(void)
{
	AssertBaseThis(0);
	_pmsnkError = pvNil;
	_pmsnkDump = pvNil;
	AssertThis(0);
}


/***************************************************************************
	Destructor for a chunky source emitter.
***************************************************************************/
CHSE::~CHSE(void)
{
	Uninit();
}


/***************************************************************************
	Initialize the chunky source emitter.
***************************************************************************/
void CHSE::Init(PMSNK pmsnkDump, PMSNK pmsnkError)
{
	AssertThis(0);
	AssertPo(pmsnkDump, 0);
	AssertNilOrPo(pmsnkError, 0);

	Uninit();

	_pmsnkDump = pmsnkDump;
	_pmsnkDump->AddRef();
	_pmsnkError = pmsnkError;
	if (_pmsnkError != pvNil)
		_pmsnkError->AddRef();

	AssertThis(fchseDump);
}


/***************************************************************************
	Clean up and return the chse to an inactive state.
***************************************************************************/
void CHSE::Uninit(void)
{
	AssertThis(0);
	ReleasePpo(&_pmsnkDump);
	ReleasePpo(&_pmsnkError);
	if (_bsf.IbMac() > 0)
		_bsf.FReplace(pvNil, 0, 0, _bsf.IbMac());
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a CHSE.
***************************************************************************/
void CHSE::AssertValid(ulong grfchse)
{
	CHSE_PAR::AssertValid(0);
	AssertPo(&_bsf, 0);
	if (grfchse & fchseDump)
		AssertPo(_pmsnkDump, 0);
	AssertNilOrPo(_pmsnkError, 0);
}


/***************************************************************************
	Mark memory for the CHSE.
***************************************************************************/
void CHSE::MarkMem(void)
{
	AssertValid(0);
	CHSE_PAR::MarkMem();
	MarkMemObj(_pmsnkError);
	MarkMemObj(_pmsnkDump);
	MarkMemObj(&_bsf);
}
#endif //DEBUG


/***************************************************************************
	Dumps chunk header.
***************************************************************************/
void CHSE::DumpHeader(CTG ctg, CNO cno, PSTN pstnName, bool fPack)
{
	AssertThis(fchseDump);
	AssertNilOrPo(pstnName, 0);

	STN stnT;

	if (pstnName != pvNil)
		{
		STN stnName;

		stnName = *pstnName;
		stnName.FExpandControls();
		stnT.FFormatSz(PszLit("CHUNK('%f', %d, \"%s\")"), ctg, cno, &stnName);
		}
	else
		stnT.FFormatSz(PszLit("CHUNK('%f', %d)"), ctg, cno);

	DumpSz(stnT.Psz());
	if (fPack)
		DumpSz(PszLit("\tPACK"));
}


/***************************************************************************
	Dump a raw data chunk
***************************************************************************/
void CHSE::DumpBlck(PBLCK pblck)
{
	AssertThis(fchseDump);
	AssertPo(pblck, 0);

	FLO flo;

	if (pblck->FPacked())
		DumpSz(PszLit("PREPACKED"));
	if (!pblck->FGetFlo(&flo, fTrue))
		{
		Error(PszLit("Dumping chunk failed"));
		return;
		}
	_bsf.FReplaceFlo(&flo, fFalse, 0, _bsf.IbMac());
	ReleasePpo(&flo.pfil);
	_DumpBsf(1);
}


/***************************************************************************
	Dump raw data from memory.
***************************************************************************/
void CHSE::DumpRgb(void *prgb, long cb, long cactTab)
{
	AssertThis(fchseDump);
	AssertIn(cb, 0, kcbMax);
	AssertPvCb(prgb, cb);

	if (cb > 0)
		{
		_bsf.FReplace(prgb, cb, 0, _bsf.IbMac());
		_DumpBsf(cactTab);
		}
}


/***************************************************************************
	Dump a parent directive
***************************************************************************/
void CHSE::DumpParentCmd(CTG ctgPar, CNO cnoPar, CHID chid)
{
	AssertThis(fchseDump);

	STN stn;

	stn.FFormatSz(PszLit("PARENT('%f', %d, %d)"), ctgPar, cnoPar, chid);
	DumpSz(stn.Psz());
}


/***************************************************************************
	Dump a bitmap directive
***************************************************************************/
void CHSE::DumpBitmapCmd(byte bTransparent, long dxp, long dyp, PSTN pstnFile)
{
	AssertThis(fchseDump);
	AssertPo(pstnFile, 0);

	STN stn;

	stn.FFormatSz(PszLit("BITMAP(%d, %d, %d) \"%s\""), (long)bTransparent, dxp,
		dyp, pstnFile);
	DumpSz(stn.Psz());
}


/***************************************************************************
	Dump a file directive
***************************************************************************/
void CHSE::DumpFileCmd(PSTN pstnFile, bool fPacked)
{
	AssertThis(fchseDump);
	AssertPo(pstnFile, 0);

	STN stn;

	if (fPacked)
		stn.FFormatSz(PszLit("PACKEDFILE \"%s\""), pstnFile);
	else
		stn.FFormatSz(PszLit("FILE \"%s\""), pstnFile);
	DumpSz(stn.Psz());
}


/***************************************************************************
	Dump an adopt directive
***************************************************************************/
void CHSE::DumpAdoptCmd(CKI *pcki, KID *pkid)
{
	AssertThis(fchseDump);
	AssertVarMem(pcki);
	AssertVarMem(pkid);

	STN stn;

	stn.FFormatSz(PszLit("ADOPT('%f', %d, '%f', %d, %d)"),
		pcki->ctg, pcki->cno, pkid->cki.ctg, pkid->cki.cno, pkid->chid);
	DumpSz(stn.Psz());
}


/***************************************************************************
	Dump the data in the _bsf
***************************************************************************/
void CHSE::_DumpBsf(long cactTab)
{
	AssertThis(fchseDump);
	AssertIn(cactTab, 0, kcchMaxStn + 1);

	byte rgb[8], bT;
	STN stn1;
	STN stn2;
	long cact;
	long ib, ibMac;
	long cb, ibT;

	stn1.SetNil();
	for (cact = cactTab; cact-- > 0; )
		stn1.FAppendCh(kchTab);
	stn1.FAppendSz(PszLit("BYTE"));
	DumpSz(stn1.Psz());

	ibMac = _bsf.IbMac();
	for (ib = 0; ib < ibMac; )
		{
		stn1.SetNil();
		for (cact = cactTab; cact-- > 0; )
			stn1.FAppendCh(kchTab);

		cb = LwMin(ibMac - ib, size(rgb));
		_bsf.FetchRgb(ib, cb, rgb);

		//append the hex
		for (ibT = 0; ibT < size(rgb); ibT++)
			{
			if (ibT >= cb)
				stn1.FAppendSz(PszLit("     "));
			else
				{
				stn2.FFormatSz(PszLit("0x%02x "), rgb[ibT]);
				stn1.FAppendStn(&stn2);
				}
			}
		stn1.FAppendSz(PszLit("   // '"));

		//append the ascii
		for (ibT = 0; ibT < cb; ibT++)
			{
			bT = rgb[ibT];
			if (bT < 0x20 || bT == 0x7F)
				bT = '?';
			stn1.FAppendCh((achar)bT);
			}
		stn1.FAppendSz(PszLit("' "));

		DumpSz(stn1.Psz());
		ib += cb;
		}
}


/******************************************************************************
	Disassembles a script (pscpt) using the given script compiler (psccb)
	and dumps the result (including a "SCRIPTPF" directive).
******************************************************************************/
bool CHSE::FDumpScript(PSCPT pscpt, PSCCB psccb)
{
	AssertThis(fchseDump);
	AssertPo(pscpt, 0);
	AssertPo(psccb, 0);

	DumpSz(PszLit("SCRIPTPF"));
	if (!psccb->FDisassemble(pscpt, _pmsnkDump, _pmsnkError))
		{
		Error(PszLit("Dumping script failed"));
		return fFalse;
		}
	return fTrue;
}


/******************************************************************************
	Dumps a GL or AL, including the GL or AL directive. pglb is the GL or AL
	to dump.
******************************************************************************/
void CHSE::DumpList(PGLB pglb)
{
	AssertThis(fchseDump);
	AssertPo(pglb, 0);

	long cbEntry;
	long iv, ivMac;
	STN stn;
	bool fAl = pglb->FIs(kclsAL);

	Assert(fAl || pglb->FIs(kclsGL), "neither a GL or AL!");

	// have a valid GL or AL -- print it out in readable format
	cbEntry = pglb->CbEntry();
	AssertIn(cbEntry, 0, kcbMax);
	ivMac = pglb->IvMac();

	stn.FFormatSz(fAl ? PszLit("\tAL(%d)") : PszLit("\tGL(%d)"), cbEntry);
	DumpSz(stn.Psz());

	// print out the entries
	for (iv = 0; iv < ivMac; iv++)
		{
		if (pglb->FFree(iv))
			DumpSz(PszLit("\tFREE"));
		else
			{
			DumpSz(PszLit("\tITEM"));
			_bsf.FReplace(pglb->PvLock(iv), cbEntry, 0, _bsf.IbMac());
			pglb->Unlock();
			_DumpBsf(2);
			}
		}
}


/******************************************************************************
	Dumps a GG or AG, including the GG or AG directive. pggb is the GG or AG
	to dump.
******************************************************************************/
void CHSE::DumpGroup(PGGB pggb)
{
	AssertThis(fchseDump);
	AssertPo(pggb, 0);

	long cbFixed, cb;
	long iv, ivMac;
	STN stnT;
	bool fAg = pggb->FIs(kclsAG);

	Assert(fAg || pggb->FIs(kclsGG), "neither a GG or AG!");

	// have a valid GG or AG -- print it out in readable format
	cbFixed = pggb->CbFixed();
	AssertIn(cbFixed, 0, kcbMax);
	ivMac = pggb->IvMac();

	stnT.FFormatSz(fAg ? PszLit("\tAG(%d)") : PszLit("\tGG(%d)"), cbFixed);
	DumpSz(stnT.Psz());

	// print out the entries
	for (iv = 0; iv < ivMac; iv++)
		{
		if (pggb->FFree(iv))
			{
			DumpSz(PszLit("\tFREE"));
			continue;
			}
		DumpSz(PszLit("\tITEM"));
		if (cbFixed > 0)
			{
			_bsf.FReplace(pggb->PvFixedLock(iv), cbFixed, 0, _bsf.IbMac());
			pggb->Unlock();
			_DumpBsf(2);
			}
		if (0 < (cb = pggb->Cb(iv)))
			{
			DumpSz(PszLit("\tVAR"));
			_bsf.FReplace(pggb->PvLock(iv), cb, 0, _bsf.IbMac());
			pggb->Unlock();
			_DumpBsf(2);
			}
		}
}


/******************************************************************************
	Dumps a GST or AST, including the GST or AST directive. pggb is the GST or
	AST to dump.
******************************************************************************/
bool CHSE::FDumpStringTable(PGSTB pgstb)
{
	AssertThis(fchseDump);
	AssertPo(pgstb, 0);

	long cbExtra;
	long iv, ivMac;
	STN stn1;
	STN stn2;
	void *pvExtra = pvNil;
	bool fAst = pgstb->FIs(kclsAST);

	Assert(fAst || pgstb->FIs(kclsGST), "neither a GST or AST!");

	// have a valid GST or AST -- print it out in readable format
	cbExtra = pgstb->CbExtra();
	AssertIn(cbExtra, 0, kcbMax);
	ivMac = pgstb->IvMac();

	if (cbExtra > 0 && !FAllocPv(&pvExtra, cbExtra, fmemNil, mprNormal))
		return fFalse;

	stn1.FFormatSz(fAst ? PszLit("\tAST(%d)") : PszLit("\tGST(%d)"),
		cbExtra);
	DumpSz(stn1.Psz());

	// print out the entries
	for (iv = 0; iv < ivMac; iv++)
		{
		if (pgstb->FFree(iv))
			{
			DumpSz(PszLit("\tFREE"));
			continue;
			}
		DumpSz(PszLit("\tITEM"));

		pgstb->GetStn(iv, &stn2);
		stn2.FExpandControls();
		stn1.FFormatSz(PszLit("\t\t\"%s\""), &stn2);
		DumpSz(stn1.Psz());

		if (cbExtra > 0)
			{
			pgstb->GetExtra(iv, pvExtra);
			_bsf.FReplace(pvExtra, cbExtra, 0, _bsf.IbMac());
			_DumpBsf(2);
			}
		}

	FreePpv(&pvExtra);
	return fTrue;
}


