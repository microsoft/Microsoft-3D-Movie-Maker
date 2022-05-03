/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Tool to merge two or more chunky files.

***************************************************************************/
#include <stdio.h>
#include "util.h"
ASSERTNAME

/***************************************************************************
	Main routine.  Returns non-zero iff there's an error.
***************************************************************************/
int __cdecl main(int cpszs, char *prgpszs[])
{
	schar chs;
	STN stn;
	FNI fniSrc, fniT;
	CKI cki;
	long icki;
	CNO cnoDst;
	PCFL pcflSrc;
	bool fPreOrder = fFalse;
	PCFL pcflMerge = pvNil;

#ifdef UNICODE
	fprintf(stderr, "\nMicrosoft (R) Chunky File Merge Utility (Unicode; "
		Debug("Debug; ") __DATE__ "; " __TIME__ ")\n");
#else //!UNICODE
	fprintf(stderr, "\nMicrosoft (R) Chunky File Merge Utility (Ansi; "
		Debug("Debug; ") __DATE__ "; " __TIME__ ")\n");
#endif //!UNICODE
	fprintf(stderr,
		"Copyright (C) Microsoft Corp 1995. All rights reserved.\n\n");

	for (prgpszs++; --cpszs > 0; prgpszs++)
		{
		chs = (*prgpszs)[0];
		if (chs == '/' || chs == '-')
			{
			// -r means final chunks should be layed out in subgraph pre-order
			switch ((*prgpszs)[1])
				{
			case 'r':
			case 'R':
				fPreOrder = fTrue;
				if (pvNil != pcflMerge)
					{
					fprintf(stderr, "-r must come before any file names");
					goto LUsage;
					}

				if (pvNil == (pcflMerge = CFL::PcflCreateTemp()))
					goto LFail;
				break;

			default:
				goto LUsage;
				}
			}
		else if (pvNil == pcflMerge)
			{
			// this is the first file name
			stn.SetSzs(*prgpszs);
			if (!fniSrc.FBuildFromPath(&stn))
				{
				fprintf(stderr, "Error: Bad file name: %s\n\n", *prgpszs);
				goto LUsage;
				}
			if (pvNil == (pcflMerge = CFL::PcflOpen(&fniSrc, fcflNil)))
				{
				fprintf(stderr, "Error: Couldn't open %s\n\n", *prgpszs);
				goto LUsage;
				}
			fniSrc.SetNil();
			}
		else
			{
			// open the file to merge into pcflMerge
			stn.SetSzs(*prgpszs);
			if (!fniT.FBuildFromPath(&stn))
				{
				fprintf(stderr, "Error: Bad file name: %s\n\n", *prgpszs);
				goto LUsage;
				}
			if (fniSrc.Ftg() == ftgNil)
				{
				fniSrc = fniT;
				continue;
				}

			// merge fniSrc into pcflMerge
			if (pvNil == (pcflSrc = CFL::PcflOpen(&fniSrc, fcflNil)))
				{
				fprintf(stderr, "Error: Couldn't open %s\n\n", *prgpszs);
				goto LUsage;
				}
			fniSrc = fniT;

			for (icki = 0; pcflSrc->FGetCki(icki, &cki); icki++)
				{
				if (!pcflSrc->FLoner(cki.ctg, cki.cno))
					continue;
				if (!pcflSrc->FCopy(cki.ctg, cki.cno, pcflMerge, &cnoDst))
					{
					fprintf(stderr, "Error: Copying chunk failed\n\n");
					goto LFail;
					}
				if (cnoDst != cki.cno)
					{
					fprintf(stderr, "Error: Duplicate chunks: '%c%c%c%c', 0x%08X\n",
						B3Lw(cki.ctg), B2Lw(cki.ctg), B1Lw(cki.ctg), B0Lw(cki.ctg),
						cki.cno);
					}
				}
			ReleasePpo(&pcflSrc);
			}
		}

	if (fniSrc.Ftg() == ftgNil)
		{
		fprintf(stderr, "Error: Not enough chunky files\n\n");
		goto LUsage;
		}

	if (fPreOrder)
		{
		FLO floSrc, floDst;

		if (!pcflMerge->FSave('CHMR'))
			goto LFail;
		pcflMerge->GetFni(&fniT);
		if (pvNil == (floSrc.pfil = FIL::PfilFromFni(&fniT)))
			{
			Bug("what happened?");
			goto LFail;
			}
		floSrc.fp = 0;
		floSrc.cb = floSrc.pfil->FpMac();

		if (pvNil == (floDst.pfil = FIL::PfilCreate(&fniSrc)))
			goto LFail;
		floDst.fp = 0;
		floDst.cb = floSrc.cb;

		if (!floSrc.FCopy(&floDst))
			{
			floDst.pfil->SetTemp(fTrue);
			goto LFail;
			}
		}
	else
		{
		// do a save as of the file that we merged into.
		if (!pcflMerge->FSave('CHMR', &fniSrc))
			goto LFail;
		}

	FIL::ShutDown();
	return 0;

LUsage:
	//print usage
	fprintf(stderr, "%s",
		"Usage:  chmerge [-r] <srcFile0> [<srcFile1> ...] <dstFile>\n\n");

LFail:
	FIL::ShutDown();
	fprintf(stderr, "Something failed\n");
	return 1;
}


#ifdef DEBUG
bool _fEnableWarnings = fTrue;

/***************************************************************************
	Warning proc called by Warn() macro
***************************************************************************/
void WarnProc(PSZS pszsFile, long lwLine, PSZS pszsMessage)
{
	if (_fEnableWarnings)
		{
		fprintf(stderr, "%s(%ld) : warning", pszsFile, lwLine);
		if (pszsMessage != pvNil)
			{
			fprintf(stderr, ": %s", pszsMessage);
			}
		fprintf(stderr, "\n");
		}
}


/***************************************************************************
	Returning true breaks into the debugger.
***************************************************************************/
bool FAssertProc(PSZS pszsFile, long lwLine, PSZS pszsMessage,
	void *pv, long cb)
{
	fprintf(stderr, "An assert occurred: \n");
	if (pszsMessage != pvNil)
		fprintf(stderr, "   Message: %s\n", pszsMessage);
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
	fprintf(stderr, "   File: %s\n", pszsFile);
	fprintf(stderr, "   Line: %ld\n", lwLine);

	return fFalse;
}
#endif //DEBUG


