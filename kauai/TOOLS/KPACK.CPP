/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Command line tool to pack or unpack a file.

***************************************************************************/
#include <stdio.h>
#include "frame.h"
ASSERTNAME

bool _FGetLwFromSzs(PSZS pszs, long *plw);


/***************************************************************************
	Main routine.  Returns non-zero iff there's an error.
***************************************************************************/
int __cdecl main(int cpszs, char *prgpszs[])
{
	FNI fniSrc, fniDst;
	STN stn;
	char chs;
	FLO floSrc, floDst;
	long lwSig;
	BLCK blck;
	bool fPacked;
	bool fCompress = fTrue;
	long cfni = 0;
	long cfmt = vpcodmUtil->CfmtDefault();

#ifdef UNICODE
	fprintf(stderr, "\nMicrosoft (R) Kauai Pack Utility (Unicode; "
		Debug("Debug; ") __DATE__ "; " __TIME__ ")\n");
#else //!UNICODE
	fprintf(stderr, "\nMicrosoft (R) Kauai Pack Utility (Ansi; "
		Debug("Debug; ") __DATE__ "; " __TIME__ ")\n");
#endif //!UNICODE
	fprintf(stderr,
		"Copyright (C) Microsoft Corp 1995. All rights reserved.\n\n");

	floSrc.pfil = pvNil;
	floDst.pfil = pvNil;
	for (prgpszs++; --cpszs > 0; prgpszs++)
		{
		chs = (*prgpszs)[0];
		if (chs == '/' || chs == '-')
			{
			switch ((*prgpszs)[1])
				{
			case 'd':
			case 'D':
				fCompress = fFalse;
				break;

			case 'c':
			case 'C':
				fCompress = fTrue;
				switch ((*prgpszs)[2])
					{
				case '\0':
					cfmt = vpcodmUtil->CfmtDefault();
					break;
				case '0':
					cfmt = cfmtNil;
					break;
				case '1':
					cfmt = kcfmtKauai;
					break;
				case '2':
					cfmt = kcfmtKauai2;
					break;
				default:
					fprintf(stderr, "Bad compression format\n\n");
					goto LUsage;
					}
				break;

			case 'p':
			case 'P':
				fCompress = fTrue;
				if (cpszs <= 0 || !_FGetLwFromSzs(prgpszs[1], &cfmt))
					{
					fprintf(stderr, "Bad compression format\n\n");
					goto LUsage;
					}
				cpszs--;
				prgpszs++;
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

	if (fCompress && cfmtNil != cfmt && !vpcodmUtil->FCanDo(cfmt, fTrue))
		{
		fprintf(stderr, "Bad compression type\n\n");
		goto LUsage;
		}

	if (pvNil == (floSrc.pfil = FIL::PfilOpen(&fniSrc)))
		{
		fprintf(stderr, "Can't open source file\n\n");
		goto LFail;
		}
	floSrc.fp = 0;
	floSrc.cb = floSrc.pfil->FpMac();

	if (fniDst.FEqual(&fniSrc) ||
			pvNil == (floDst.pfil = FIL::PfilCreate(&fniDst)))
		{
		fprintf(stderr, "Can't create destination file\n\n");
		goto LFail;
		}

	if (!fCompress)
		{
		long lwSwapped;

		if (floSrc.cb < size(long))
			goto LBadSrc;

		if (!floSrc.FReadRgb(&lwSig, size(long), 0))
			{
			fprintf(stderr, "Reading source file failed\n\n");
			goto LFail;
			}

		lwSwapped = lwSig;
		SwapBytesRglw(&lwSwapped, 1);
		if (lwSig == klwSigPackedFile || lwSwapped == klwSigPackedFile)
			fPacked = fTrue;
		else if (lwSig == klwSigUnpackedFile || lwSwapped == klwSigUnpackedFile)
			fPacked = fFalse;
		else
			{
LBadSrc:
			fprintf(stderr, "Source file is not packed\n\n");
			goto LFail;
			}

		blck.Set(floSrc.pfil, size(long), floSrc.cb - size(long), fPacked);
		if (fPacked && !blck.FUnpackData())
			{
			fprintf(stderr, "Unpacking source failed\n\n");
			goto LFail;
			}

		floDst.fp = 0;
		floDst.cb = blck.Cb();
		}
	else
		{
		blck.Set(floSrc.pfil, 0, floSrc.cb, fFalse);
		if (cfmtNil != cfmt)
			blck.FPackData(cfmt);

		// write the signature
		if (blck.FPacked())
			lwSig = klwSigPackedFile;
		else
			lwSig = klwSigUnpackedFile;
		if (!floDst.pfil->FWriteRgb(&lwSig, size(long), 0))
			{
			fprintf(stderr, "Writing destination failed\n\n");
			goto LFail;
			}

		floDst.fp = size(long);
		floDst.cb = blck.Cb(fTrue);
		}

	if (!blck.FWriteToFlo(&floDst, fTrue))
		{
		fprintf(stderr, "Writing destination failed\n\n");
		goto LFail;
		}

	ReleasePpo(&floSrc.pfil);
	ReleasePpo(&floDst.pfil);
	FIL::ShutDown();
	return 0;

LUsage:
	//print usage
	fprintf(stderr, "%s",
		"Usage:  kpack [-d] [-c[0|1|2]] [-p <format>] <srcFile> <dstFile>\n\n");

LFail:
	if (pvNil != floDst.pfil)
		floDst.pfil->SetTemp();
	ReleasePpo(&floDst.pfil);
	ReleasePpo(&floSrc.pfil);

	FIL::ShutDown();
	return 1;
}


/***************************************************************************
	Get a long value from a string. If the string isn't a number and the
	length is <= 4, assumes the characters are to be packed into a long
	(ala CTGs and FTGs).
***************************************************************************/
bool _FGetLwFromSzs(PSZS pszs, long *plw)
{
	STN stn;
	long ich;

	stn.SetSzs(pszs);
	if (stn.FGetLw(plw))
		return fTrue;

	if (stn.Cch() > 4)
		return fFalse;

	*plw = 0;
	for (ich = 0; ich < stn.Cch(); ich++)
		*plw = (*plw << 8) + (byte)stn.Prgch()[ich];

	return fTrue;
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

