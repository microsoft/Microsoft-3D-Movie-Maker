/***************************************************************************
	Author: ******
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Tool to dump chomp source for the help topics contained in a chunky
	file.

***************************************************************************/
#include <stdio.h>
#include "util.h"
#include "mssio.h"
#include "chelpexp.h"
ASSERTNAME


/***************************************************************************
	Main routine.  Returns non-zero iff there's an error.
***************************************************************************/
int __cdecl main(int cpszs, char *prgpszs[])
{
	schar chs;
	STN stn;
	FNI fni;
	PFIL pfil = pvNil;
	PMSNK pmsnk = pvNil;
	PCFL pcflSrc = pvNil;

#ifdef UNICODE
	fprintf(stderr, "\nMicrosoft (R) Chunky Help Dump Utility (Unicode; "
		Debug("Debug; ") __DATE__ "; " __TIME__ ")\n");
#else //!UNICODE
	fprintf(stderr, "\nMicrosoft (R) Chunky Help Dump Utility (Ansi; "
		Debug("Debug; ") __DATE__ "; " __TIME__ ")\n");
#endif //!UNICODE
	fprintf(stderr,
		"Copyright (C) Microsoft Corp 1995. All rights reserved.\n\n");

	for (prgpszs++; --cpszs > 0; prgpszs++)
		{
		chs = (*prgpszs)[0];
		if (chs == '/' || chs == '-')
			{
			// no command line switches
			goto LUsage;
			}
		else if (pvNil == pcflSrc)
			{
			// this is the first file name
			stn.SetSzs(*prgpszs);
			if (!fni.FBuildFromPath(&stn))
				{
				fprintf(stderr, "Error: Bad file name: %s\n\n", *prgpszs);
				goto LUsage;
				}
			if (pvNil == (pcflSrc = CFL::PcflOpen(&fni, fcflNil)))
				{
				fprintf(stderr, "Error: Couldn't open %s\n\n", *prgpszs);
				goto LUsage;
				}
			fni.SetNil();
			}
		else if (fni.Ftg() == ftgNil)
			{
			// get the destination file name
			stn.SetSzs(*prgpszs);
			if (!fni.FBuildFromPath(&stn))
				{
				fprintf(stderr, "Error: Bad file name: %s\n\n", *prgpszs);
				goto LUsage;
				}
			}
		else
			{
			fprintf(stderr, "Too many file names");
			goto LUsage;
			}
		}

	if (pvNil == pcflSrc)
		goto LUsage;

	if (fni.Ftg() == ftgNil)
		pmsnk = NewObj MSSIO(stdout);
	else
		{
		if (pvNil == (pfil = FIL::PfilCreate(&fni)))
			{
			fprintf(stderr, "Couldn't create destination file");
			goto LFail;
			}
		pmsnk = NewObj MSFIL(pfil);
		}

	if (pvNil == pmsnk)
		{
		fprintf(stderr, "Memory failure");
		goto LFail;
		}

	// do the export
	if (!FExportHelpText(pcflSrc, pmsnk))
		{
		fprintf(stderr, "Dump failed");
		goto LFail;
		}

	FIL::ShutDown();
	return 0;

LUsage:
	//print usage
	fprintf(stderr, "%s",
		"Usage:  chelpdmp <srcFile> [<dstFile>]\n\n");

LFail:
	if (pvNil != pfil)
		pfil->SetTemp();
	FIL::ShutDown();
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


