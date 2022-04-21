/***************************************************************************
	Author: ShonK
	Project: Kauai test app
	Reviewed:
	Copyright (c) Microsoft Corporation

	Utility tester.

***************************************************************************/
#include "util.h"
#include <stdio.h>
ASSERTNAME

void TestUtil(void);
void CheckForLostMem(void);
bool FFindPrime(long lwMax, long lwMaxRoot, long *plwPrime, long *plwRoot);


/***************************************************************************
	Call test routines.
***************************************************************************/
void __cdecl main(long cpszs, char **prgpszs)
{
#ifdef REVIEW //shonk: for counting lines
	FNE fne;
	FNI fniDir, fni;
	FTG rgftg[2];
	byte rgb[512];
	FP fpMac, fp;
	long cbTot, clnTot, ib, cbT, cln;
	PFIL pfil;
	STN stn;

	if (!fniDir.FGetOpen("All files\0*.*\0", hNil))
		return;

	fniDir.FSetLeaf(pvNil, kftgDir);
	rgftg[0] = 'H';
	rgftg[1] = 'CPP';
	if (!fne.FInit(&fniDir, rgftg, 2))
		return;

	cbTot = 0;
	clnTot = 0;
	while (fne.FNextFni(&fni))
		{
		fni.GetStnPath(&stn);

		if (pvNil == (pfil = FIL::PfilOpen(&fni)))
			return;
		fpMac = pfil->FpMac();
		cbTot += fpMac;

		fp = 0;
		cln = 0;
		while (fp < fpMac)
			{
			cbT = LwMin(fpMac - fp, size(rgb));
			if (!pfil->FReadRgbSeq(rgb, cbT, &fp))
				return;
			for (ib = 0; ib < cbT; ib++)
				{
				if (rgb[ib] == kchReturn)
					cln++;
				}
			}
		clnTot += cln;
		printf("%s: %d, %d\n", stn.Psz(), cln, fpMac);
		}

	printf("Total bytes: %d;  Total lines: %d\n", cbTot, clnTot);
#endif //REVIEW

#ifndef REVIEW //shonk: for finding a prime and a primitive root for the prime
	long lwPrime, lwRoot, lw;
	STN stn;

	lwPrime = 6000;
	if (cpszs > 1)
		{
		stn.SetSzs(prgpszs[1]);
		if (stn.FGetLw(&lw) && FIn(lw, 2, kcbMax))
			lwPrime = lw;
		}
	lwRoot = lwPrime / 2;
	if (cpszs > 2)
		{
		stn.SetSzs(prgpszs[2]);
		if (stn.FGetLw(&lw) && FIn(lw, 2, kcbMax))
			lwRoot = lw;
		}

	if (FFindPrime(lwPrime, lwRoot, &lwPrime, &lwRoot))
		printf("prime = %ld, primitive root = %ld\n", lwPrime, lwRoot);
#endif //REVIEW

#ifdef REVIEW //shonk: general testing stuff
	CheckForLostMem();
	TestUtil();
	CheckForLostMem();
#endif
}


#ifdef DEBUG
/***************************************************************************
	Returning true breaks into the debugger.
***************************************************************************/
bool FAssertProc(PSZS pszsFile, long lwLine, PSZS pszsMsg, void *pv, long cb)
{
	printf("An assert occurred: \n\r");
	if (pszsMsg != pvNil)
		printf("   Msg: %s\n\r", pszsMsg);
	if (pv != pvNil)
		{
		printf("   Address %x\n\r", pv);
		if (cb != 0)
			{
			printf("   Value: ");
			switch (cb)
				{
			default:
				{
				byte *pb;
				byte *pbLim;

				for (pb = (byte *)pv, pbLim = pb + cb; pb < pbLim; pb++)
					printf("%2x", (int)*pb);
				}
				break;

			case 2:
				printf("%4x", (int)*(short *)pv);
				break;

			case 4:
				printf("%8x", *(long *)pv);
				break;
				}
			printf("\n\r");
			}
		}
	printf("   File: %s\n\r", pszsFile);
	printf("   Line: %ld\n\r", lwLine);

	return fFalse;
}


/***************************************************************************
	Callback from util for warnings.
***************************************************************************/
void WarnProc(PSZS pszsFile, long lwLine, PSZS pszsMsg)
{
	printf("Warning\n\r");
	if (pszsMsg != pvNil)
		printf("   Msg: %s\n\r", pszsMsg);
	printf("   File: %s\n\r", pszsFile);
	printf("   Line: %ld\n\r", lwLine);
}
#endif //DEBUG


/***************************************************************************
	Unmarks all hqs, marks all hqs known to be in use, then asserts
	on all unmarked hqs.
***************************************************************************/
void CheckForLostMem(void)
{
	UnmarkAllMem();
	UnmarkAllObjs();

	MarkUtilMem(); //marks all util memory

	AssertUnmarkedMem();
	AssertUnmarkedObjs();
}


/***************************************************************************
	Find the largest prime that is less than lwMax and find a primitive root
	for it.
***************************************************************************/
bool FFindPrime(long lwMax, long lwMaxRoot, long *plwPrime, long *plwRoot)
{
	AssertIn(lwMax, 3, kcbMax);
	AssertVarMem(plwPrime);
	AssertVarMem(plwRoot);
	byte *prgb;
	long cb;
	long lw, ibit, lwT, clwHit;

	// make sure lwMax is even.
	lwMax = (lwMax + 1) & ~1;

	cb = LwDivAway(lwMax, 16);
	if (!FAllocPv((void **)&prgb, cb, fmemClear, mprNormal))
		return fFalse;

	for (lw = 3; lw < lwMax / 3; lw += 2)
		{
		ibit = lw / 2;
		if (prgb[ibit / 8] & (1 << (ibit % 8)))
			continue;

		for (lwT = 3 * lw; lwT < lwMax; lwT += 2 * lw)
			{
			ibit = lwT / 2;
			prgb[ibit / 8] |= (1 << (ibit % 8));
			}
		}

	for (lw = lwMax - 1; ; lw -= 2)
		{
		ibit = lw / 2;
		if (!(prgb[ibit / 8] & (1 << (ibit % 8))))
			break;
		}

	*plwPrime = lw;
	FreePpv((void **)&prgb);

	for (lw = LwMin(lwMaxRoot, *plwPrime - 1); ; lw--)
		{
		if (lw <= 1)
			{
			Assert(lw > 1, "bug");
			break;
			}
		for (lwT = lw, clwHit = 0; ; )
			{
			clwHit++;
			LwMulDivMod(lwT, lw, *plwPrime, &lwT);
			if (lwT == lw)
				break;
			}
		if (clwHit == *plwPrime - 1)
			break;
		}

	*plwRoot = lw;
	return fTrue;
}



