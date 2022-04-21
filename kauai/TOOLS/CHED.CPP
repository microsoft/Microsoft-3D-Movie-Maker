/***************************************************************************

	Chunky editor.

***************************************************************************/
#include "ched.h"
ASSERTNAME


BEGIN_CMD_MAP(APP, APPB)
	ON_CID_GEN(cidNew, &APP::FCmdOpen, pvNil)
	ON_CID_GEN(cidOpen, &APP::FCmdOpen, pvNil)
	ON_CID_GEN(cidNewText, &APP::FCmdOpen, pvNil)
	ON_CID_GEN(cidOpenText, &APP::FCmdOpen, pvNil)
END_CMD_MAP_NIL()

APP vapp;

RTCLASS(APP)


/***************************************************************************
	Main for a frame app.
***************************************************************************/
void FrameMain(void)
{
	vapp.Run(fappNil, fgobNil, kginDefault);
}


/***************************************************************************
	Initialize the APP - do the command line parsing thing.
***************************************************************************/
bool APP::_FInit(ulong grfapp, ulong grfgob, long ginDef)
{
	if (!APP_PAR::_FInit(grfapp, grfgob, ginDef))
		return fFalse;

#ifdef WIN
	//parse the command line and load any resource files and help files
	FNI fni;
	STN stn;
	bool fQuote, fScript, fSkip;
	PDMD pdmd;
	PDDG pddg;
	PDOCB pdocb;
	long lw;
	PSZ psz = vwig.pszCmdLine;

	fSkip = fTrue;
	fScript = fFalse;
	for (;;)
		{
		while (*psz == kchSpace)
			psz++;
		if (!*psz)
			break;

		stn.SetNil();
		fQuote = fFalse;
		while (*psz && (fQuote || *psz != kchSpace))
			{
			if (*psz == ChLit('"'))
				fQuote = !fQuote;
			else
				stn.FAppendCh(*psz);
			psz++;
			}

		if (stn.Cch() == 0)
			continue;

		if (fSkip)
			{
			// application path
			fSkip = fFalse;
			continue;
			}

		if (stn.Cch() == 2 && (stn.Psz()[0] == ChLit('/') ||
				stn.Psz()[0] == ChLit('-')))
			{
			// command line switch
			switch (stn.Psz()[1])
				{
			case ChLit('s'):
				fScript = fTrue;
				break;
				}
			continue;
			}

		if (fScript)
			{
			// this arg should be a cno
			fScript = fFalse;
			if (!stn.FGetLw(&lw))
				continue;

			if (pvNil == (pdmd = DMD::PdmdTop()))
				continue;
			if (pvNil == (pddg = pdmd->Pdocb()->PddgActive()) ||
					!pddg->FIs(kclsDCD))
				{
				continue;
				}
			((PDCD)pddg)->FTestScript(kctgScript, lw);
			continue;
			}

		if (!fni.FBuildFromPath(&stn) || fni.Ftg() == kftgDir)
			continue;

		if (pvNil != (pdocb = DOCB::PdocbFromFni(&fni)))
			{
			pdocb->ActivateDmd();
			continue;
			}
		if (pvNil == (pdocb = (PDOCB)DOC::PdocNew(&fni)))
			continue;
		pdocb->PdmdNew();
		ReleasePpo(&pdocb);
		}
#endif //WIN
	return fTrue;
}


/***************************************************************************
	Get the name for the frame tester app.
***************************************************************************/
void APP::GetStnAppName(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

#ifdef UNICODE
	STN stnDate;
	STN stnTime;

	stnDate.SetSzs(__DATE__);
	stnTime.SetSzs(__TIME__);
	pstn->FFormatSz(Debug(PszLit("Debug ")) PszLit("Ched (Unicode; %s; %s)"),
		&stnDate, &stnTime);
#else //!UNICODE
	*pstn = Debug("Debug ") "Ched (Ansi; " __DATE__ "; " __TIME__ ")";
#endif //!UNICODE
}


/***************************************************************************
	Update the given window.  *prc is the bounding rectangle of the update
	region.
***************************************************************************/
void APP::UpdateHwnd(HWND hwnd, RC *prc, ulong grfapp)
{
	AssertThis(0);
	PGOB pgob;

	if (pvNil == (pgob = GOB::PgobFromHwnd(hwnd)))
		return;

	//for script windows, do offscreen updating
	if (pgob->FIs(kclsTSCG))
		grfapp |= fappOffscreen;

	APP_PAR::UpdateHwnd(hwnd, prc, grfapp);
}


/***************************************************************************
	Do a fast update of the gob and its descendents into the given gpt.
***************************************************************************/
void APP::_FastUpdate(PGOB pgob, PREGN pregnClip, ulong grfapp, PGPT pgpt)
{
	AssertThis(0);
	AssertPo(pgob, 0);
	AssertPo(pregnClip, 0);
	AssertNilOrPo(pgpt, 0);

	if (pgob->FIs(kclsTSCG))
		grfapp |= fappOffscreen;

	APP_PAR::_FastUpdate(pgob, pregnClip, grfapp, pgpt);
}


/***************************************************************************
	Open an existing or new chunky file for editing.
	Handles cidNew and cidOpen.
***************************************************************************/
bool APP::FCmdOpen(PCMD pcmd)
{
	FNI fni;
	FNI *pfni;
	PDOCB pdocb;

	pfni = pvNil;
	switch (pcmd->cid)
		{
	default:
		Bug("why are we here?");
		return fTrue;

	case cidOpen:
		//do the standard dialog
		if (!FGetFniOpenMacro(&fni, pvNil, 0,
				PszLit("Chunky Files\0*.chk;*.cfl\0All Files\0*.*\0"),
				vwig.hwndApp))
			{
			return fTrue;
			}
		pfni = &fni;
		if (pvNil != (pdocb = DOCB::PdocbFromFni(&fni)))
			{
			pdocb->ActivateDmd();
			return fTrue;
			}
		//fall through
	case cidNew:
		pdocb = (PDOCB)DOC::PdocNew(pfni);
		break;

	case cidOpenText:
		//do the standard dialog
		if (!FGetFniOpenMacro(&fni, pvNil, 0,
			PszLit("Text Files\0*.txt;*.cht\0All Files\0*.*\0"), vwig.hwndApp))
			{
			return fTrue;
			}
		pfni = &fni;
		if (pvNil != (pdocb = DOCB::PdocbFromFni(&fni)))
			{
			pdocb->ActivateDmd();
			return fTrue;
			}
		//fall through
	case cidNewText:
		pdocb = (PDOCB)CHTXD::PchtxdNew(pfni, pvNil, oskNil);
		break;
		}

	if (pvNil == pdocb)
		return fTrue;

	pdocb->PdmdNew();
	ReleasePpo(&pdocb);

	return fTrue;
}


