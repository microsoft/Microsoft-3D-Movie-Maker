/***************************************************************************
	Author: ******
	Project: Soc
	Copyright (c) Microsoft Corporation

	Command line tool to make a tmap from a bitmap file and optionally
	compress it. Stores the file in chomp's "compressed file" format
	to be imported using the chomp PACKEDFILE command.

***************************************************************************/
#include <stdio.h>
#include "frame.h"
#include "bren.h"
#include "mssio.h"
ASSERTNAME


/***************************************************************************
	Main routine.  Returns non-zero iff there's an error.
***************************************************************************/
int __cdecl main(int cpszs, char *prgpszs[])
{
	FNI fniSrc, fniDst;
	STN stn;
	char chs;
	PTMAP ptmap = pvNil;
	long cfni = 0;
	bool fCompress = fFalse;
	MSSIO mssioErr(stderr);

#ifdef UNICODE
	fprintf(stderr, "\nMicrosoft (R) Make Tmap Utility (Unicode; "
		__DATE__ "; " __TIME__ ")\n");
#else //!UNICODE
	fprintf(stderr, "\nMicrosoft (R) Make Tmap Utility (Ansi; "
		__DATE__ "; " __TIME__ ")\n");
#endif //!UNICODE
	fprintf(stderr,
		"Copyright (C) Microsoft Corp 1995. All rights reserved.\n\n");

	for (prgpszs++; --cpszs > 0; prgpszs++)
		{
		chs = (*prgpszs)[0];
		if (chs == '/' || chs == '-')
			{
			switch ((*prgpszs)[1])
				{
			case 'c':
			case 'C':
				fCompress = fTrue;
				break;

			default:
				goto LUsage;
				}
			}
		else if (cfni >= 2)
			{
			fprintf(stderr, "Too many file names\n\n");
			goto LUsage;
			}
		else
			{
			stn.SetSzs(prgpszs[0]);
			if (!fniDst.FBuildFromPath(&stn))
				{
				fprintf(stderr, "Bad file name\n\n");
				goto LUsage;
				}
			if (cfni == 0)
				fniSrc = fniDst;
			cfni++;
			}
		}

	if (cfni != 2)
		{
		fprintf(stderr, "Wrong number of file names\n\n");
		goto LUsage;
		}

	ptmap = TMAP::PtmapReadNative(&fniSrc);
	if (pvNil == ptmap)
		{
		fprintf(stderr, "reading texture map failed\n\n");
		goto LFail;
		}

	if (!ptmap->FWriteTmapChkFile(&fniDst, fCompress, &mssioErr))
		goto LFail;
	ReleasePpo(&ptmap);

	FIL::ShutDown();
	return 0;

LUsage:
	//print usage
	fprintf(stderr, "%s",
		"Usage:  mktmap [-c] <srcBitmapFile> <dstTmapFile>\n\n");

LFail:
	ReleasePpo(&ptmap);

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

