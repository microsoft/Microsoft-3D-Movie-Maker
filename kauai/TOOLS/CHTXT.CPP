/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	Chunky editor text document management

***************************************************************************/
#include "ched.h"
ASSERTNAME



/***************************************************************************
	Constructor for a chunky text doc.
***************************************************************************/
CHTXD::CHTXD(PDOCB pdocb, ulong grfdoc) : CHTXD_PAR(pdocb, grfdoc)
{
}


/***************************************************************************
	Create a new chunky text doc.
***************************************************************************/
PCHTXD CHTXD::PchtxdNew(PFNI pfni, PBSF pbsf, short osk, PDOCB pdocb, ulong grfdoc)
{
	AssertNilOrPo(pfni, ffniFile);
	AssertNilOrPo(pbsf, 0);
	AssertNilOrPo(pdocb, 0);
	PCHTXD pchtxd;

	if (pvNil == (pchtxd = NewObj CHTXD(pdocb, grfdoc)))
		return pvNil;

	if (!pchtxd->_FInit(pfni, pbsf, osk))
		ReleasePpo(&pchtxd);

	return pchtxd;
}


/***************************************************************************
	Create a new document display gob for the chunky text doc.
***************************************************************************/
PDDG CHTXD::PddgNew(PGCB pgcb)
{
	return CHTDD::PchtddNew(this, pgcb, vpappb->OnnDefFixed(),
		fontNil, vpappb->DypTextDef(), 4);
}


BEGIN_CMD_MAP(CHTDD, DDG)
	ON_CID_GEN(cidCompileChunky, &CHTDD::FCmdCompileChunky, pvNil)
	ON_CID_GEN(cidCompileScript, &CHTDD::FCmdCompileScript, pvNil)
	ON_CID_GEN(cidAssembleScript, &CHTDD::FCmdCompileScript, pvNil)
END_CMD_MAP_NIL()


/***************************************************************************
	Constructor.
***************************************************************************/
CHTDD::CHTDD(PTXTB ptxtb, PGCB pgcb,
	long onn, ulong grfont, long dypFont, long cchTab)
		: CHTDD_PAR(ptxtb, pgcb, onn, grfont, dypFont, cchTab)
{
	_fMark = fFalse;
}


/***************************************************************************
	Create a new one.
***************************************************************************/
PCHTDD CHTDD::PchtddNew(PTXTB ptxtb, PGCB pgcb,
	long onn, ulong grfont, long dypFont, long cchTab)
{
	PCHTDD pchtdd;

	if (pvNil == (pchtdd = NewObj CHTDD(ptxtb, pgcb,
			onn, grfont, dypFont, cchTab)))
		{
		return pvNil;
		}

	if (!pchtdd->_FInit())
		{
		ReleasePpo(&pchtdd);
		return pvNil;
		}
	pchtdd->Activate(fTrue);

	AssertPo(pchtdd, 0);
	return pchtdd;
}


/***************************************************************************
	Compile this text file into a chunky file and open it.
***************************************************************************/
bool CHTDD::FCmdCompileChunky(PCMD pcmd)
{
	FNI fni;
	PCFL pcfl;
	STN stnFile;
	MSFIL msfil;
	CHCM chcm;
	PDOC pdoc;

	if (!fni.FGetTemp())
		return fTrue;

	Ptxtb()->GetName(&stnFile);
	pcfl = chcm.PcflCompile(Ptxtb()->Pbsf(), &stnFile, &fni, &msfil);
	if (pvNil == pcfl)
		{
		vpappb->TGiveAlertSz(PszLit("Compiling chunky file failed"),
			bkOk, cokExclamation);
		pcmd->cid = cidNil; //don't record

		//if the error file isn't empty, open it
		OpenSinkDoc(&msfil);
		return fTrue;
		}
	pdoc = DOC::PdocNew(&fni);
	pcfl->SetTemp(fTrue);
	ReleasePpo(&pcfl);
	if (pvNil == pdoc)
		{
		vpappb->TGiveAlertSz(PszLit("Can't open new chunky file"),
			bkOk, cokExclamation);
		pcmd->cid = cidNil; //don't record
		return fTrue;
		}
	pdoc->PdmdNew();
	ReleasePpo(&pdoc);
	return fTrue;
}


/***************************************************************************
	Compile this text file into a script and put it in a chunky file and
	open it.
***************************************************************************/
bool CHTDD::FCmdCompileScript(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	SCCG sccg;
	MSFIL msfil;
	STN stnFile;
	PSCPT pscpt;
	PDOC pdoc = pvNil;

	Ptxtb()->GetName(&stnFile);
	LEXB lexb(Ptxtb()->Pbsf(), &stnFile);
	pscpt = sccg.PscptCompileLex(&lexb, pcmd->cid == cidCompileScript, &msfil);
	if (pvNil == pscpt)
		{
		vpappb->TGiveAlertSz(PszLit("Compiling script failed"),
			bkOk, cokExclamation);
		pcmd->cid = cidNil; //don't record

		//if the error file isn't empty, open it
		OpenSinkDoc(&msfil);
		return fTrue;
		}

	//add the chunk and write the data
	if (pvNil == (pdoc = DOC::PdocNew(pvNil)))
		goto LFail;

	if (!pscpt->FSaveToChunk(pdoc->Pcfl(), kctgScript, 0))
		{
LFail:
		vpappb->TGiveAlertSz(PszLit("Can't create new chunky file"),
			bkOk, cokExclamation);
		ReleasePpo(&pscpt);
		ReleasePpo(&pdoc);
		pcmd->cid = cidNil; //don't record
		return fTrue;
		}
	ReleasePpo(&pscpt);

	pdoc->PdmdNew();
	ReleasePpo(&pdoc);
	return fTrue;
}


/***************************************************************************
	Open the file based message sink file as a chunky text document.
***************************************************************************/
void OpenSinkDoc(PMSFIL pmsfil)
{
	PDOCB pdocb;
	PFIL pfil;
	FNI fni;
	bool fTemp;

	if (pvNil == (pfil = pmsfil->PfilRelease()))
		return;
	if (pfil->FpMac() == 0)
		{
		ReleasePpo(&pfil);
		return;
		}
	pfil->GetFni(&fni);
	fTemp = pfil->FTemp();
	pdocb = (PDOCB)CHTXD::PchtxdNew(&fni);
	pfil->SetTemp(fTemp);
	ReleasePpo(&pfil);
	if (pvNil != pdocb)
		{
		pdocb->PdmdNew();
		ReleasePpo(&pdocb);
		}
}

