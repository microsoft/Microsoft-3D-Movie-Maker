/***************************************************************************
	Author: ******
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Command line tool to make an mbmp from a bitmap file and optionally
	compress it.

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
	FLO flo;
	long lwSig;
	PMBMP pmbmp = pvNil;
	long cfni = 0;
	long xp = 0;
	long yp = 0;
	long lwTrans = 0;
	long cfmt = cfmtNil;

#ifdef UNICODE
	fprintf(stderr, "\nMicrosoft (R) Make Mbmp Utility (Unicode; "
		Debug("Debug; ") __DATE__ "; " __TIME__ ")\n");
#else //!UNICODE
	fprintf(stderr, "\nMicrosoft (R) Make Mbmp Utility (Ansi; "
		Debug("Debug; ") __DATE__ "; " __TIME__ ")\n");
#endif //!UNICODE
	fprintf(stderr,
		"Copyright (C) Microsoft Corp 1995. All rights reserved.\n\n");

	flo.pfil = pvNil;
	for (prgpszs++; --cpszs > 0; prgpszs++)
		{
		chs = (*prgpszs)[0];
		if (chs == '/' || chs == '-')
			{
			switch ((*prgpszs)[1])
				{
			case 'c':
			case 'C':
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
				if (cpszs <= 0 || !_FGetLwFromSzs(prgpszs[1], &cfmt))
					{
					fprintf(stderr, "Bad compression format\n\n");
					goto LUsage;
					}
				cpszs--;
				prgpszs++;
				break;

			case 'o':
			case 'O':
				// get the coordinates of the reference point
				if (cpszs < 2)
					goto LUsage;
				cpszs -= 2;
				stn.SetSzs(prgpszs[1]);
				if (!stn.FGetLw(&xp))
					goto LUsage;
				stn.SetSzs(prgpszs[2]);
				if (!stn.FGetLw(&yp))
					goto LUsage;
				prgpszs += 2;
				break;

			case 't':
			case 'T':
				// get the transparent pixel value
				if (cpszs < 1)
					goto LUsage;
				cpszs--;
				stn.SetSzs(prgpszs[1]);
				if (!stn.FGetLw(&lwTrans))
					goto LUsage;
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

	if (cfmtNil != cfmt && !vpcodmUtil->FCanDo(cfmt, fTrue))
		{
		fprintf(stderr, "Bad compression type\n\n");
		goto LUsage;
		}

	pmbmp = MBMP::PmbmpReadNative(&fniSrc, B0Lw(lwTrans), xp, yp);
	if (pvNil == pmbmp)
		{
		fprintf(stderr, "reading bitmap failed\n\n");
		goto LFail;
		}

	if (pvNil == (flo.pfil = FIL::PfilCreate(&fniDst)))
		{
		fprintf(stderr, "Couldn't create destination file\n\n");
		goto LFail;
		}
	flo.fp = size(long);
	flo.cb = pmbmp->CbOnFile();

	if (cfmtNil != cfmt)
		{
		BLCK blck;

		if (!blck.FSetTemp(flo.cb) || !pmbmp->FWrite(&blck))
			{
			fprintf(stderr, "allocation failure\n\n");
			goto LFail;
			}
		ReleasePpo(&pmbmp);
		if (!blck.FPackData(cfmt))
			lwSig = klwSigUnpackedFile;
		else
			{
			lwSig = klwSigPackedFile;
			flo.cb = blck.Cb(fTrue);
			}
		if (!flo.pfil->FWriteRgb(&lwSig, size(long), 0) ||
				!blck.FWriteToFlo(&flo, fTrue))
			{
			fprintf(stderr, "writing to destination file failed\n\n");
			goto LFail;
			}
		}
	else
		{
		lwSig = klwSigUnpackedFile;
		if (!flo.pfil->FWriteRgb(&lwSig, size(long), 0) ||
				!pmbmp->FWriteFlo(&flo))
			{
			fprintf(stderr, "writing to destination file failed\n\n");
			goto LFail;
			}
		ReleasePpo(&pmbmp);
		}

	ReleasePpo(&flo.pfil);
	FIL::ShutDown();
	return 0;

LUsage:
	//print usage
	fprintf(stderr, "%s",
		"Usage:  mkmbmp [-c[0|1|2]] [-p <format>] [-o <x> <y>] [-t <b>] <srcBitmapFile> <dstMbmpFile>\n\n");

LFail:
	ReleasePpo(&pmbmp);
	if (pvNil != flo.pfil)
		flo.pfil->SetTemp();
	ReleasePpo(&flo.pfil);

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

