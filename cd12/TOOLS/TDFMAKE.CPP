/***************************************************************************

	tdfmake.cpp: Three-D Font authoring tool

	Primary Author: manders
	Review Status: Not yet reviewed

	TDFmake is a command-line tool for building 3-D Font files from a
	set of models.

	Usage:
		tdfmake <fontdir1> <fontdir2> ... <destChunkFile>

	All the DAT files for a given font should be in a directory.  The
	directory name becomes the name of the TDF chunk.  The DAT file names
	should contain a number, which becomes the CHID of the model chunk 
	under the TDF.

***************************************************************************/
#include "soc.h"
#include "tdfmake.h"
ASSERTNAME

const CTG kctgTdfMake = 'TDFM';

bool FMakeTdf(PFNI pfniSrc, PCFL pcflDst);

/***************************************************************************
	Main routine.  Returns non-zero	if there's an error.
***************************************************************************/
int __cdecl main(int cpsz, achar *prgpsz[])
{
	STN stnDst;
	STN stnSrc;
	FNI fniSrcDir;
	FNI fniDst;
	PCFL pcflDst;
	long ifniSrc;

	fprintf(stderr, "\nMicrosoft (R) TDF Maker\n");
	fprintf(stderr, "Copyright (C) Microsoft Corp 1995. All rights reserved.\n\n");

	BrBegin();
	if (cpsz < 2)
		{
		fprintf(stderr, "%s",
			"Usage:\n"
			"   tdfmake <fontdir1> <fontdir2> ... <destChunkFile>\n\n");
		goto LFail;
		}

	stnDst = prgpsz[cpsz - 1];
	if (!fniDst.FBuildFromPath(&stnDst))
		{
		fprintf(stderr, "Bad destination file name %s\n\n", stnDst.Psz());
		goto LFail;
		}
	fniDst.GetStnPath(&stnDst);
	pcflDst = CFL::PcflCreate(&fniDst, fcflWriteEnable);
	if (pvNil == pcflDst)
		{
		fprintf(stderr, "Couldn't create destination chunky file %s\n\n", stnDst.Psz());
		goto LFail;
		}

	for (ifniSrc = 0; ifniSrc < cpsz - 2; ifniSrc++)
		{
		stnSrc = prgpsz[ifniSrc + 1];
		if (stnSrc.Psz()[stnSrc.Cch() - 1] != ChLit('\\'))
			{
			if (!stnSrc.FAppendCh(ChLit('\\')))
				goto LFail;
			}
		if (!fniSrcDir.FBuildFromPath(&stnSrc))
			{
			fprintf(stderr, "Bad source directory %s\n\n", stnSrc.Psz());
			goto LFail;
			}
		fniSrcDir.GetStnPath(&stnSrc);
		fprintf(stderr, "%s  --->  %s\n", stnSrc.Psz(), stnDst.Psz());
		if (!FMakeTdf(&fniSrcDir, pcflDst))
			goto LFail;
		}
	if (!pcflDst->FSave(kctgTdfMake))
		{
		fprintf(stderr, "Couldn't save chunky file.\n\n");
		goto LFail;
		}
	BrEnd();
	return 0; // no error
LFail:
	BrEnd();
	fprintf(stderr, "TDF Maker failed.\n\n");
	return 1; // error
}


/***************************************************************************
	Writes a TDF chunk and child BMDL chunks based on all DAT files in
	pfniSrcDir to the destination file pcflDst.
***************************************************************************/
bool FMakeTdf(PFNI pfniSrcDir, PCFL pcflDst)
{
	AssertPo(pfniSrcDir, ffniDir);
	AssertPo(pcflDst, 0);

	FTG ftgDat = MacWin('bdat', 'DAT');
	FNE fne;
	FNI fni;
	STN stn;
	STN stn2;
	CHID chid;
	CHID chidMax = 0;
	PMODL pmodl;
	CNO cnoModl;
	PCRF pcrf;
	PSZ psz;
	long cch;
	long lw;
	PGL pglkid;
	KID kid;
	bool fFoundSpace = fFalse;	// 0x20
	bool fFoundSpace2 = fFalse; // 0xa0
	long cmodl = 0;

	pglkid = GL::PglNew(size(KID));
	if (pglkid == pvNil)
		goto LFail;
	pcrf = CRF::PcrfNew(pcflDst, 0);
	if (pvNil == pcrf)
		goto LFail;
	// get directory name (don't actually move up a dir)
	if (!pfniSrcDir->FUpDir(&stn, 0)) 
		goto LFail;
	if (stn.Psz()[0]==ChLit('\\'))
		stn2.SetSz(stn.Psz() + 1);
	else
		stn2 = stn;
	if (!fne.FInit(pfniSrcDir, &ftgDat, 1))
		goto LFail;
	while(fne.FNextFni(&fni))
		{
		fni.GetLeaf(&stn);
		psz = stn.Psz();
		while (*psz != ChLit('\0') && !FIn(*psz, ChLit('0'), ChLit('9') + 1))
			psz++;
		for (cch = 0; FIn(*(psz + cch), ChLit('0'), ChLit('9') + 1); cch++)
			;
		if (cch == 0)
			{
			fprintf(stderr, "Filename must include a number: %s\n\n", stn.Psz());
			goto LFail;
			}
		stn2.SetRgch(psz, cch);
		if (!stn2.FGetLw(&lw, 10))
			goto LFail;
		chid = lw;
		if (chid > chidMax)
			chidMax = chid;
		if (chid == (CHID)ChLit(' '))
			fFoundSpace = fTrue;
		if (chid == 0xa0) // nonbreaking space
			fFoundSpace2 = fTrue;
		pmodl = MODL::PmodlReadFromDat(&fni);
		if (pvNil == pmodl)
			return fFalse;
		pmodl->AdjustTdfCharacter();
		if (!pcflDst->FAdd(0, kctgBmdl, &cnoModl))
			goto LFail;
		if (!pmodl->FWrite(pcflDst, kctgBmdl, cnoModl))
			goto LFail;
		if (!pcflDst->FPackData(kctgBmdl, cnoModl))
			goto LFail;
		kid.chid = chid;
		kid.cki.ctg = kctgBmdl;
		kid.cki.cno = cnoModl;
		if (!pglkid->FAdd(&kid))
			goto LFail;
		cmodl++;
		}
	fprintf(stderr, "Converted %d characters\n", cmodl);

	// Hack to insert a space character if none specified
	if (!fFoundSpace)
		{
		pmodl = MODL::PmodlNew(0, pvNil, 0, pvNil);
		if (pvNil == pmodl)
			return fFalse;
		if (!pcflDst->FAdd(0, kctgBmdl, &cnoModl))
			goto LFail;
		if (!pmodl->FWrite(pcflDst, kctgBmdl, cnoModl))
			goto LFail;
		kid.chid = (CHID)ChLit(' ');
		kid.cki.ctg = kctgBmdl;
		kid.cki.cno = cnoModl;
		if (!pglkid->FAdd(&kid))
			goto LFail;
		fprintf(stderr, "Added a space character\n");
		}
	// Hack to insert a nonbreaking space character if none specified
	if (!fFoundSpace2)
		{
		pmodl = MODL::PmodlNew(0, pvNil, 0, pvNil);
		if (pvNil == pmodl)
			return fFalse;
		if (!pcflDst->FAdd(0, kctgBmdl, &cnoModl))
			goto LFail;
		if (!pmodl->FWrite(pcflDst, kctgBmdl, cnoModl))
			goto LFail;
		kid.chid = 0xa0;
		kid.cki.ctg = kctgBmdl;
		kid.cki.cno = cnoModl;
		if (!pglkid->FAdd(&kid))
			goto LFail;
		fprintf(stderr, "Added a nonbreaking space character\n");
		}
	if (!TDF::FCreate(pcrf, pglkid, &stn2))
		goto LFail;

	ReleasePpo(&pcrf);
	ReleasePpo(&pglkid);
	return fTrue;
LFail:
	ReleasePpo(&pcrf);
	ReleasePpo(&pglkid);
	return fFalse;
}


#ifdef DEBUG
bool _fEnableWarnings = fTrue;

/***************************************************************************
	Warning proc called by Warn() macro
***************************************************************************/
void WarnProc(PSZ pszFile, long lwLine, PSZ pszMessage)
{
	if (_fEnableWarnings)
		{
		fprintf(stderr, "%s(%ld) : warning", pszFile, lwLine);
		if (pszMessage != pvNil)
			{
			fprintf(stderr, ": %s", pszMessage);
			}
		fprintf(stderr, "\n");
		}
}


/***************************************************************************
	Returning true breaks into the debugger.
***************************************************************************/
bool FAssertProc(PSZ pszFile, long lwLine, PSZ pszMessage,
	void *pv, long cb)
{
	fprintf(stderr, "An assert occurred: \n");
	if (pszMessage != pvNil)
		fprintf(stderr, "   Message: %s\n", pszMessage);
	if (pv != pvNil)
		{
		fprintf(stderr, "   Address %x\n", pv);
		if (cb != 0)
			{
			fprintf(stderr, "   Value: ");
			switch (cb)
				{
			default:
				{
				byte *pb;
				byte *pbLim;

				for (pb = (byte *)pv, pbLim = pb + cb; pb < pbLim; pb++)
					fprintf(stderr, "%02x", (int)*pb);
				}
				break;

			case 2:
				fprintf(stderr, "%04x", (int)*(short *)pv);
				break;

			case 4:
				fprintf(stderr, "%08lx", *(long *)pv);
				break;
				}
			printf("\n");
			}
		}
	fprintf(stderr, "   File: %s\n", pszFile);
	fprintf(stderr, "   Line: %ld\n", lwLine);

	return fFalse;
}
#endif


