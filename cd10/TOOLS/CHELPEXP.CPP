/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Routines to export help topics to chomp input format.

***************************************************************************/
#include "kidframe.h"
#include "chelpexp.h"
ASSERTNAME


static bool _FWriteHelpChunk(PCFL pcfl, PCHSE pchse, KID *pkid, CKI *pckiPar);
static bool _FWriteHelpPropAg(PCFL pcfl, PCHSE pchse, KID *pkid, CKI *pckiPar);
static void _AppendHelpStnLw(PSTN pstn, PGST pgst, long istn, long lw);


/***************************************************************************
	Export the help topics in their textual representation for compilation
	by chomp.
	REVIEW shonk: this code is a major hack and very fragile.
***************************************************************************/
bool FExportHelpText(PCFL pcfl, PMSNK pmsnk)
{
	AssertPo(pcfl, 0);

	BLCK blck;
	PGST pgst;
	long icki;
	CKI cki, ckiPar;
	KID kid;
	CGE cge;
	ulong grfcge;
	HTOPF htopf;
	CHSE chse;
	STN stn, stnT;
	bool fRet = fFalse;

	chse.Init(pmsnk);

	chse.DumpSz(PszLit("#undef __HELP_NAME"));
	chse.DumpSz(PszLit("#undef __HELP_SYMBOL"));
	chse.DumpSz(PszLit("#undef __HELP_PACK"));
	chse.DumpSz(PszLit("#undef __HELP_PACK2"));

	chse.DumpSz(PszLit("#ifdef NO_HELP_NAMES"));
	chse.DumpSz(PszLit("#define __HELP_NAME(name) \"\""));
	chse.DumpSz(PszLit("#else"));
	chse.DumpSz(PszLit("#define __HELP_NAME(name) name"));
	chse.DumpSz(PszLit("#endif"));

	chse.DumpSz(PszLit("#ifdef NO_HELP_SYMBOLS"));
	chse.DumpSz(PszLit("#define __HELP_SYMBOL(stuff)"));
	chse.DumpSz(PszLit("#else"));
	chse.DumpSz(PszLit("#define __HELP_SYMBOL(stuff) stuff"));
	chse.DumpSz(PszLit("#endif"));

	chse.DumpSz(PszLit("#ifdef PACK_HELP"));
	chse.DumpSz(PszLit("#define __HELP_PACK PACK"));
	chse.DumpSz(PszLit("#else"));
	chse.DumpSz(PszLit("#define __HELP_PACK"));
	chse.DumpSz(PszLit("#endif"));

	chse.DumpSz(PszLit("#ifdef HELP_SINGLE_CHUNK"));
	chse.DumpSz(PszLit("#define __HELP_PACK2"));
	chse.DumpSz(PszLit("#else"));
	chse.DumpSz(PszLit("#define __HELP_PACK2 __HELP_PACK"));
	chse.DumpSz(PszLit("#endif"));

	pgst = pvNil;
	for (icki = 0; pcfl->FGetCkiCtg(kctgHelpTopic, icki, &cki); icki++)
		{
		// read the string table if it's there
		if (pcfl->FGetKidChidCtg(cki.ctg, cki.cno, 0, kctgGst, &kid) &&
			(!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck) ||
				pvNil == (pgst = GST::PgstRead(&blck)) ||
				pgst->IvMac() != 6 && (pgst->IvMac() != 5 ||
				!pgst->FAddRgch(PszLit(""), 0))))
			{
			goto LFail;
			}

		// read the topic
		if (!pcfl->FFind(cki.ctg, cki.cno, &blck) || !blck.FUnpackData() ||
				blck.Cb() != size(HTOPF) || !blck.FRead(&htopf))
			{
			goto LFail;
			}
		if (htopf.bo == kboOther)
			SwapBytesBom(&htopf.htop, kbomHtop);
		else if (htopf.bo != kboCur)
			goto LFail;

		// dump the htop's cno definition
		chse.DumpSz(PszLit(""));
		stn.FFormatSz(PszLit("SET _help_%x_%x="), cki.ctg, cki.cno);
		_AppendHelpStnLw(&stn, pgst, 1, cki.cno);
		chse.DumpSz(stn.Psz());

		// dump the single chunk prefix
		pcfl->FGetName(cki.ctg, cki.cno, &stnT);
		stnT.FExpandControls();
		chse.DumpSz(PszLit(""));
		chse.DumpSz(PszLit("#ifdef HELP_SINGLE_CHUNK"));
		stn.FFormatSz(
			PszLit("CHUNK('%f', _help_%x_%x, __HELP_NAME(\"%s\")) __HELP_PACK"),
			cki.ctg, cki.ctg, cki.cno, &stnT);
		chse.DumpSz(stn.Psz());
		chse.DumpSz(PszLit("SUBFILE"));
		chse.DumpSz(PszLit("#endif //HELP_SINGLE_CHUNK"));
		stn.FAppendCh(ChLit('2'));

		// dump the HTOPF
		chse.DumpSz(PszLit(""));
		chse.DumpSz(stn.Psz());

		stn = PszLit("\tBO OSK LONG");
		_AppendHelpStnLw(&stn, pgst, 0, htopf.htop.cnoBalloon);
		_AppendHelpStnLw(&stn, pgst, 2, htopf.htop.hidThis);
		_AppendHelpStnLw(&stn, pgst, 3, htopf.htop.hidTarget);
		_AppendHelpStnLw(&stn, pgst, 4, htopf.htop.cnoScript);
		stnT.FFormatSz(PszLit(" 0x%x 0x%x 0x%x"),
			htopf.htop.dxp, htopf.htop.dyp, htopf.htop.ckiSnd.ctg);
		stn.FAppendStn(&stnT);
		_AppendHelpStnLw(&stn, pgst, 5, htopf.htop.ckiSnd.cno);
		chse.DumpSz(stn.Psz());
		ReleasePpo(&pgst);

		chse.DumpSz(PszLit("ENDCHUNK"));

		// dump child chunks
		cge.Init(pcfl, cki.ctg, cki.cno);
		while (cge.FNextKid(&kid, &ckiPar, &grfcge, fcgeNil))
			{
			if (fcgeError & grfcge)
				goto LFail;

			if ((fcgeRoot & grfcge) || !(fcgePre & grfcge))
				continue;

			chse.DumpSz(PszLit(""));
			if (kid.cki.ctg == kctgTxtPropArgs)
				{
				// special handling of argument AG
				if (!_FWriteHelpPropAg(pcfl, &chse, &kid, &ckiPar))
					goto LFail;
				}
			else
				{
				if (kid.cki.ctg == kctgGst)
					{
					// put "#ifndef NO_HELP_SYMBOLS" before it
					chse.DumpSz(PszLit("#ifndef NO_HELP_SYMBOLS"));
					}
				if (!_FWriteHelpChunk(pcfl, &chse, &kid, &ckiPar))
					goto LFail;
				if (kid.cki.ctg == kctgGst)
					{
					// put "#endif //!NO_HELP_SYMBOLS" after it
					chse.DumpSz(PszLit("#endif //!NO_HELP_SYMBOLS"));
					}
				}
			}

		chse.DumpSz(PszLit(""));
		chse.DumpSz(PszLit("#ifdef HELP_SINGLE_CHUNK"));
		chse.DumpSz(PszLit("ENDCHUNK"));
		chse.DumpSz(PszLit("#endif //HELP_SINGLE_CHUNK"));
		chse.DumpSz(PszLit(""));
		}

	fRet = fTrue;

LFail:
	chse.Uninit();
	ReleasePpo(&pgst);

	return fRet;
}


/***************************************************************************
	Dump a chunk as text to the given chse.
***************************************************************************/
bool _FWriteHelpChunk(PCFL pcfl, PCHSE pchse, KID *pkid, CKI *pckiPar)
{
	AssertPo(pcfl, 0);
	AssertPo(pchse, 0);
	AssertVarMem(pkid);
	AssertVarMem(pckiPar);

	STN stn;
	STN stnT;
	BLCK blck;

	if (!pcfl->FFind(pkid->cki.ctg, pkid->cki.cno, &blck))
		return fFalse;

	// dump the CHUNK declaration
	pchse->DumpSz(PszLit("SET _help_cno++"));
	stn.FFormatSz(PszLit("SET _help_%x_%x=_help_cno"),
		pkid->cki.ctg, pkid->cki.cno);
	pchse->DumpSz(stn.Psz());
	pcfl->FGetName(pkid->cki.ctg, pkid->cki.cno, &stnT);
	stnT.FExpandControls();
	stn.FFormatSz(
		PszLit("CHUNK('%f', _help_cno, __HELP_NAME(\"%s\")) __HELP_PACK2"),
		pkid->cki.ctg, &stnT);
	pchse->DumpSz(stn.Psz());

	// dump the PARENT declaration
	stn.FFormatSz(PszLit("PARENT('%f', _help_%x_%x, 0x%x)"),
		pckiPar->ctg, pckiPar->ctg, pckiPar->cno, pkid->chid);
	pchse->DumpSz(stn.Psz());

	if (pkid->cki.ctg == kctgGst)
		{
		PGST pgst;
		short bo, osk;
		bool fPacked = blck.FPacked();
		bool fRet;

		pgst = GST::PgstRead(&blck, &bo, &osk);
		if (pvNil == pgst)
			return fFalse;

		if (fPacked)
			pchse->DumpSz(PszLit("PACK"));

		if (bo != kboCur)
			pchse->DumpSz(MacWin(PszLit("WINBO"), PszLit("MACBO")));

		fRet = pchse->FDumpStringTable(pgst);
		ReleasePpo(&pgst);

		if (bo != kboCur)
			pchse->DumpSz(MacWin(PszLit("MACBO"), PszLit("WINBO")));
		if (!fRet)
			return fFalse;
		}
	else
		{
		// dump the data
		pchse->DumpBlck(&blck);
		}

	// dump the ENDCHUNK
	pchse->DumpSz(PszLit("ENDCHUNK"));
	return fTrue;
}


/***************************************************************************
	Write the property AG.  This requires special processing
***************************************************************************/
bool _FWriteHelpPropAg(PCFL pcfl, PCHSE pchse, KID *pkid, CKI *pckiPar)
{
	AssertPo(pcfl, 0);
	AssertPo(pchse, 0);
	AssertVarMem(pkid);
	AssertVarMem(pckiPar);

	PAG pag;
	short bo, osk;
	STN stn, stnT, stnT2;
	byte rgb[2 * kcbMaxDataStn];
	BLCK blck;
	long iv, lw, cb, ib, cbRead;
	CKI cki;

	pag = pvNil;
	if (!pcfl->FFind(pkid->cki.ctg, pkid->cki.cno, &blck) ||
			pvNil == (pag = AG::PagRead(&blck, &bo, &osk)) ||
			bo != kboCur || osk != koskCur || size(long) != pag->CbFixed())
		{
		ReleasePpo(&pag);
		return fFalse;
		}

	// dump the CHUNK declaration
	pchse->DumpSz(PszLit("SET _help_cno++"));
	stn.FFormatSz(PszLit("SET _help_%x_%x=_help_cno"),
		pkid->cki.ctg, pkid->cki.cno);
	pchse->DumpSz(stn.Psz());
	pcfl->FGetName(pkid->cki.ctg, pkid->cki.cno, &stnT);
	stnT.FExpandControls();
	stn.FFormatSz(
		PszLit("CHUNK('%f', _help_cno, __HELP_NAME(\"%s\")) __HELP_PACK2"),
		pkid->cki.ctg, &stnT);
	pchse->DumpSz(stn.Psz());

	// dump the PARENT declaration
	stn.FFormatSz(PszLit("PARENT('%f', _help_%x_%x, 0x%x)"),
		pckiPar->ctg, pckiPar->ctg, pckiPar->cno, pkid->chid);
	pchse->DumpSz(stn.Psz());

	// dump the AG declaration
	pchse->DumpSz(PszLit("AG(4)"));

	// dump the items
	for (iv = 0; iv < pag->IvMac(); iv++)
		{
		if (pag->FFree(iv))
			{
			pchse->DumpSz(PszLit("\tFREE"));
			continue;
			}

		pag->GetFixed(iv, &lw);
		stn.FFormatSz(PszLit("\tITEM LONG 0x%x"), lw);
		pchse->DumpSz(stn.Psz());

		cb = pag->Cb(iv);
		if (0 == cb)
			continue;

		switch (B3Lw(lw))
			{
		case 64: //sprmGroup
			if (cb <= size(byte) + size(CNO))
				goto LWriteCore;
			if (cb > size(rgb))
				{
				Bug("bad group data");
				goto LWriteCore;
				}

			pag->GetRgb(iv, 0, cb, rgb);
			ib = size(byte) + size(CNO);
			if (!stnT.FSetData(rgb + ib, cb - ib) || stnT.Cch() == 0)
				{
				Bug("bad group data");
				goto LWriteCore;
				}

			stn.FFormatSz(
				PszLit("\t\tVAR BYTE %d LONG %s __HELP_SYMBOL( STN \""),
				(long)rgb[0], &stnT);
			stnT.FExpandControls();
			stn.FAppendStn(&stnT);
			stn.FAppendSz(PszLit("\" )"));
			pchse->DumpSz(stn.Psz());
			break;

		case 192: //sprmObject
			if (cb <= size(CKI))
				goto LWriteCore;

			// an object
			pag->GetRgb(iv, 0, size(CKI), &cki);
			switch (cki.ctg)
				{
			default:
				goto LWriteCore;

			case kctgMbmp:
				ib = size(CKI);
				if (ib >= cb)
					goto LWriteCore;
				if ((cb -= ib) > kcbMaxDataStn)
					{
					Bug("bad picture data");
					goto LWriteCore;
					}
				pag->GetRgb(iv, ib, cb, rgb);
				if (!stnT.FSetData(rgb, cb) || stnT.Cch() == 0)
					{
					Bug("bad picture data 2");
					goto LWriteCore;
					}
				stn.FFormatSz(
					PszLit("\t\tVAR LONG '%f' %s __HELP_SYMBOL( STN \""),
					cki.ctg, &stnT);
				stnT.FExpandControls();
				stn.FAppendStn(&stnT);
				stn.FAppendSz(PszLit("\" )"));
				pchse->DumpSz(stn.Psz());
				break;

			case kctgGokd:
				ib = size(CKI) + size(long);
				if (ib >= cb)
					goto LWriteCore;
				if ((cb -= ib) > size(rgb))
					{
					Bug("bad button data");
					goto LWriteCore;
					}
				pag->GetRgb(iv, ib, cb, rgb);
				if (!stnT.FSetData(rgb, cb, &cbRead) || cbRead >= cb ||
					!stnT2.FSetData(rgb + cbRead, cb - cbRead))
					{
					Bug("bad picture data 2");
					goto LWriteCore;
					}
				stn.FFormatSz(PszLit("\t\tVAR LONG '%f' "), cki.ctg);
				if (stnT.Cch() > 0)
					{
					stn.FAppendStn(&stnT);
					stnT.FExpandControls();
					}
				else
					{
					stnT.FFormatSz(PszLit("0x%x"), cki.cno);
					stn.FAppendStn(&stnT);
					stnT.SetNil();
					}
				stn.FAppendCh(kchSpace);
				if (stnT2.Cch() > 0)
					{
					stn.FAppendStn(&stnT2);
					stnT2.FExpandControls();
					}
				else
					{
					pag->GetRgb(iv, size(CKI), size(long), &lw);
					stnT2.FFormatSz(PszLit("0x%x"), lw);
					stn.FAppendStn(&stnT2);
					stnT2.SetNil();
					}
				stn.FAppendSz(PszLit(" __HELP_SYMBOL( STN "));
				if (stnT.Cch() > 0)
					{
					stn.FAppendCh(ChLit('"'));
					stn.FAppendStn(&stnT);
					stn.FAppendSz(PszLit("\" "));
					}
				if (stnT2.Cch() > 0)
					{
					if (stnT.Cch() > 0)
						stn.FAppendSz(PszLit("; "));
					stn.FAppendCh(ChLit('"'));
					stn.FAppendStn(&stnT2);
					stn.FAppendSz(PszLit("\" "));
					}
				stn.FAppendCh(ChLit(')'));
				pchse->DumpSz(stn.Psz());
				break;
				}
			break;

		default:
LWriteCore:
			// just write the data
			pchse->DumpSz(PszLit("\t\tVAR"));
			pchse->DumpRgb(pag->PvLock(iv), cb, 3);
			pag->Unlock();
			break;
			}
		}

	// dump the ENDCHUNK
	ReleasePpo(&pag);
	pchse->DumpSz(PszLit("ENDCHUNK"));

	return fTrue;
}


/***************************************************************************
	Append a string or number.
***************************************************************************/
void _AppendHelpStnLw(PSTN pstn, PGST pgst, long istn, long lw)
{
	AssertPo(pstn, 0);
	AssertNilOrPo(pgst, 0);
	STN stn;

	stn.SetNil();
	if (pvNil != pgst)
		pgst->GetStn(istn, &stn);
	if (stn.Cch() == 0)
		stn.FFormatSz(PszLit(" 0x%x"), lw);
	else
		pstn->FAppendCh(kchSpace);
	pstn->FAppendStn(&stn);
}


