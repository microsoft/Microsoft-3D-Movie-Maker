/***************************************************************************
	Author: ******
	Project: Kauai test app
	Reviewed:
	Copyright (c) Microsoft Corporation

	Test code used in both ut and ft.

***************************************************************************/
#include "util.h"
ASSERTNAME

#ifdef DEBUG
extern void CheckForLostMem(BASE *po);
#else //!DEBUG
#define CheckForLostMem(po)
#endif //!DEBUG

void TestInt(void);
void TestMem(void);
void TestGl(void);
void TestFni(void);
void TestFil(void);
void TestGg(void);
void TestCfl(void);
void TestErs(void);
void TestCrf(void);


/******************************************************************************
	Test util code.
******************************************************************************/
void TestUtil(void)
{
	Bug("Assert Test");
	long lw = 0x12345678;
	BugVar("AssertVar test", &lw);

	TestInt();
	TestMem();
	TestErs();
	TestGl();
	TestGg();
	//TestFni();
	//TestFil();
	//TestCfl();
	TestCrf();
}


/***************************************************************************
	Test utilint stuff.
***************************************************************************/
void TestInt(void)
{
	AssertDo(SwHigh(0x12345678) == (short)0x1234, 0);
	AssertDo(SwHigh(0xABCDEF01) == (short)0xABCD, 0);
	AssertDo(SwLow(0x12345678) == (short)0x5678, 0);
	AssertDo(SwLow(0xABCDEF01) == (short)0xEF01, 0);

	AssertDo(LwHighLow(0x1234, 0x5678) == 0x12345678, 0);
	AssertDo(LwHighLow((short)0xABCD, (short)0xEF01) == 0xABCDEF01, 0);

	AssertDo(BHigh(0x1234) == 0x12, 0);
	AssertDo(BHigh((short)0xABCD) == 0xAB, 0);
	AssertDo(BLow(0x1234) == 0x34, 0);
	AssertDo(BLow((short)0xABCD) == 0xCD, 0);

	AssertDo(SwHighLow(0x12, 0x56) == (short)0x1256, 0);
	AssertDo(SwHighLow(0xAB, 0xEF) == (short)0xABEF, 0);

	AssertDo(SwMin(kswMax, kswMin) == kswMin, 0);
	AssertDo(SwMin(kswMin, kswMax) == kswMin, 0);
	AssertDo(SwMax(kswMax, kswMin) == kswMax, 0);
	AssertDo(SwMax(kswMin, kswMax) == kswMax, 0);

	AssertDo(SuMin(ksuMax, 0) == 0, 0);
	AssertDo(SuMin(0, ksuMax) == 0, 0);
	AssertDo(SuMax(ksuMax, 0) == ksuMax, 0);
	AssertDo(SuMax(0, ksuMax) == ksuMax, 0);

	AssertDo(LwMin(klwMax, klwMin) == klwMin, 0);
	AssertDo(LwMin(klwMin, klwMax) == klwMin, 0);
	AssertDo(LwMax(klwMax, klwMin) == klwMax, 0);
	AssertDo(LwMax(klwMin, klwMax) == klwMax, 0);

	AssertDo(LuMin(kluMax, 0) == 0, 0);
	AssertDo(LuMin(0, kluMax) == 0, 0);
	AssertDo(LuMax(kluMax, 0) == kluMax, 0);
	AssertDo(LuMax(0, kluMax) == kluMax, 0);

	AssertDo(SwAbs(kswMax) == kswMax, 0);
	AssertDo(SwAbs(kswMin) == -kswMin, 0);
	AssertDo(LwAbs(klwMax) == klwMax, 0);
	AssertDo(LwAbs(klwMin) == -klwMin, 0);

	AssertDo(LwMulSw(kswMax, kswMax) == 0x3FFF0001, 0);
	AssertDo(LwMulSw(kswMin, kswMin) == 0x3FFF0001, 0);
	AssertDo(LwMulSw(kswMax, kswMin) == -0x3FFF0001, 0);

	AssertDo(LwMulDiv(klwMax, klwMax, klwMin) == klwMin, 0);
	AssertDo(LwMulDiv(klwMax, klwMin, klwMax) == klwMin, 0);

	AssertDo(CbRoundToLong(0) == 0, 0);
	AssertDo(CbRoundToLong(1) == 4, 0);
	AssertDo(CbRoundToLong(2) == 4, 0);
	AssertDo(CbRoundToLong(3) == 4, 0);
	AssertDo(CbRoundToLong(4) == 4, 0);
	AssertDo(CbRoundToLong(5) == 8, 0);

	AssertDo(CbRoundToShort(0) == 0, 0);
	AssertDo(CbRoundToShort(1) == 2, 0);
	AssertDo(CbRoundToShort(2) == 2, 0);
	AssertDo(CbRoundToShort(3) == 4, 0);
	AssertDo(CbRoundToShort(4) == 4, 0);
	AssertDo(CbRoundToShort(5) == 6, 0);

	AssertDo(LwGcd(10000, 350) == 50, 0);
	AssertDo(LwGcd(10000, -560) == 80, 0);

	AssertDo(FcmpCompareFracs(50000, 30000, 300000, 200000) == fcmpGt, 0);
	AssertDo(FcmpCompareFracs(-50000, 30000, -300000, 200000) == fcmpLt, 0);
	AssertDo(FcmpCompareFracs(-50000, 30000, 300000, 200000) == fcmpLt, 0);
	AssertDo(FcmpCompareFracs(50000, 30000, -300000, 200000) == fcmpGt, 0);
	AssertDo(FcmpCompareFracs(50000, 30000, 500000, 300000) == fcmpEq, 0);
	AssertDo(FcmpCompareFracs(0x1FFF0000, 0x10, 0x11000000, 0x10) == fcmpGt, 0);
}


/***************************************************************************
	Test the memory manager.
***************************************************************************/
void TestMem(void)
{
	#define kchq 18
	static HQ rghq[kchq]; //static so it's initially zeros
	HQ hqT, hq;
	long cb, ihq;

	for (ihq = 0; ihq < kchq; ihq++)
		{
		cb = (1L << ihq) - 1 + ihq;
		if (!FAllocHq(&hq, cb, fmemNil, mprDebug))
			{
			BugVar("HqAlloc failed on size:", &cb);
			break;
			}
		rghq[ihq] = hq;
		AssertDo(CbOfHq(hq) == cb, 0);

		FillPb(QvFromHq(hq), cb, (byte)cb);
		if (cb > 0)
			{
			AssertDo(*(byte *)QvFromHq(hq) == (byte)cb, 0);
			AssertDo(*(byte *)PvAddBv(QvFromHq(hq), cb - 1) == (byte)cb, 0);
			}

		AssertDo(PvLockHq(hq) == QvFromHq(hq), 0);
		if (!FCopyHq(hq, &hqT, mprDebug))
			Bug("FCopyHq failed");
		else
			{
			AssertDo(CbOfHq(hqT) == cb, 0);
			if (cb > 0)
				{
				AssertDo(*(byte *)QvFromHq(hqT) == (byte)cb, 0);
				AssertDo(*(byte *)PvAddBv(QvFromHq(hqT), cb - 1) == (byte)cb, 0);
				}
			FreePhq(&hqT);
			}
		Assert(hqT == hqNil, 0);
		FreePhq(&hqT);

		UnlockHq(hq);
		}

	for (ihq = 0; ihq < kchq; ihq++)
		{
		hq = rghq[ihq];
		if (hqNil == hq)
			break;
		cb = CbOfHq(hq);
		if (!FResizePhq(&rghq[ihq], 2 * cb, fmemClear, mprDebug))
			{
			Bug("FResizePhq failed");
			break;
			}
		hq = rghq[ihq];
		if (cb > 0)
			{
			AssertDo(*(byte *)QvFromHq(hq) == (byte)cb, 0);
			AssertDo(*(byte *)PvAddBv(QvFromHq(hq), cb - 1) == (byte)cb, 0);
			AssertDo(*(byte *)PvAddBv(QvFromHq(hq), cb) == 0, 0);
			AssertDo(*(byte *)PvAddBv(QvFromHq(hq), 2 * cb - 1) == 0, 0);
			}
		}

	for (ihq = 0; ihq < kchq; ihq++)
		{
		if (hqNil != rghq[ihq])
			{
			cb = (1L << (kchq - 1 - ihq)) - 1 + (kchq - 1 - ihq);
			AssertDo(FResizePhq(&rghq[ihq], cb, fmemNil, mprDebug), "FResizePhq failed");
			}
		FreePhq(&rghq[ihq]);
		}
}


/***************************************************************************
	Test list code.
***************************************************************************/
void TestGl(void)
{
	short sw;
	long isw;
	short *qsw;
	PGL pglsw;

	pglsw = GL::PglNew(size(short));
	if (pvNil == pglsw)
		{
		Bug("PglNew failed");
		return;
		}

	for (sw = 0; sw < 10; sw++)
		{
		AssertDo(pglsw->FAdd(&sw, &isw), 0);
		AssertDo(isw == sw, 0);
		}
	AssertDo(pglsw->IvMac() == 10, 0);

	for (isw = 0; isw < 10; isw++)
		{
		qsw = (short *)pglsw->QvGet(isw);
		AssertDo(*qsw == isw, 0);
		pglsw->Get(isw, &sw);
		AssertDo(sw == isw, 0);
		}

	for (isw = 10; isw > 0; )
		pglsw->Delete(isw -= 2);
	AssertDo(pglsw->IvMac() == 5, 0);

	for (isw = 0; isw < 5; isw++)
		{
		pglsw->Get(isw, &sw);
		AssertDo(sw == isw * 2 + 1, 0);
		sw = (short)isw * 100;
		pglsw->Put(isw, &sw);
		qsw = (short *)pglsw->QvGet(isw);
		AssertDo(*qsw == isw * 100, 0);
		*qsw = (short)isw;
		}

	AssertDo(pglsw->IvMac() == 5, 0);
	AssertDo(pglsw->FEnsureSpace(0, fgrpShrink), 0);
	AssertDo(pglsw->IvMac() == 5, 0);

	for (isw = 5; isw-- != 0; )
		{
		sw = (short)isw;
		pglsw->FInsert(isw, &sw);
		}

	AssertDo(pglsw->IvMac() == 10, 0);
	for (isw = 10; isw-- != 0; )
		{
		pglsw->Get(isw, &sw);
		AssertDo(sw == isw / 2, 0);
		}

	ReleasePpo(&pglsw);
}


/***************************************************************************
	Test the fni code.
***************************************************************************/
void TestFni(void)
{
	FNI fni1, fni2;
	STN stn1, stn2, stn3;

	AssertDo(fni1.FGetTemp(), 0);
	AssertDo(fni1.Ftg() == vftgTemp, 0);
	AssertDo(!fni1.FDir(), 0);
	fni2 = fni1;
	fni1.GetLeaf(&stn3);
	fni2.GetLeaf(&stn2);
	AssertDo(stn3.FEqual(&stn2), 0);

	AssertDo(fni1.FSetLeaf(pvNil, kftgDir), 0);
	AssertDo(fni1.FDir(), 0);
	AssertDo(fni1.Ftg() == kftgDir, 0);

	AssertDo(fni1.FUpDir(&stn1, ffniNil), 0);
	AssertDo(fni1.FUpDir(&stn2, ffniMoveToDir), 0);
	AssertDo(stn1.FEqual(&stn2), 0);
	AssertDo(!fni1.FSameDir(&fni2), 0);
	AssertDo(!fni1.FEqual(&fni2), 0);
	AssertDo(fni1.Ftg() == kftgDir, 0);
	AssertDo(fni1.FDownDir(&stn1, ffniNil), 0);
	AssertDo(!fni1.FSameDir(&fni2), 0);
	AssertDo(fni1.FDownDir(&stn1, ffniMoveToDir), 0);
	AssertDo(fni1.FSameDir(&fni2), 0);
	AssertDo(fni1.FSetLeaf(&stn3, vftgTemp), 0);

	AssertDo(fni1.FEqual(&fni2), 0);
	AssertDo(fni1.TExists() == tNo, 0);
	AssertDo(fni2.FSetLeaf(pvNil, kftgDir), 0);
	AssertDo(fni2.FGetUnique(vftgTemp), 0);
	AssertDo(!fni1.FEqual(&fni2), 0);
}


/***************************************************************************
	File test code.
***************************************************************************/
void TestFil(void)
{
	PFIL pfil;
	FNI fni;

	while (FGetFniSaveMacro(&fni, 'TEXT', "\x9" "Save As: ", "\x4" "Junk",
			PszLit("All files\0*.*\0"), NULL))
		{
		AssertDo(fni.TExists() == tNo, 0);
		pfil = FIL::PfilCreate(&fni);
		AssertPo(pfil, 0);
		AssertDo(pfil->FSetFpMac(100), 0);
		AssertDo(pfil->FpMac() == 100, 0);
		AssertDo(pfil->FWriteRgb(&fni, size(fni), 0), 0);
		pfil->SetTemp(fTrue);
		pfil->Mark();
		ReleasePpo(&pfil);
		}

	FIL::CloseUnmarked();
	FIL::ClearMarks();
	FIL::CloseUnmarked();
}


/***************************************************************************
	Test the group api.
***************************************************************************/
void TestGg(void)
{
	PGG pgg;
	ulong grf;
	long cb, iv;
	byte *qb;
	PSZ psz = PszLit("0123456789ABCDEFG");
	achar rgch[100];

	AssertDo((pgg = GG::PggNew(0)) != pvNil, 0);
	for (iv = 0; iv < 10; iv++)
		{
		AssertDo(pgg->FInsert(iv / 2, iv + 1, psz), 0);
		}
	AssertDo(pgg->FAdd(16, &iv, psz), 0);
	AssertDo(iv == 10, 0);
	AssertDo(pgg->IvMac() == 11, 0);

	grf = 0;
	for (iv = pgg->IvMac(); iv--; )
		{
		cb = pgg->Cb(iv);
		qb = (byte *)pgg->QvGet(iv);
		AssertDo(FEqualRgb(psz, qb, cb), 0);
		grf |= 1L << cb;
		if (cb & 1)
			pgg->Delete(iv);
		}
	AssertDo(grf == 0x000107FE, 0);

	grf = 0;
	for (iv = pgg->IvMac(); iv--; )
		{
		cb = pgg->Cb(iv);
		AssertDo(!(cb & 1), 0);
		pgg->Get(iv, rgch);
		qb = (byte *)pgg->QvGet(iv);
		AssertDo(FEqualRgb(rgch, qb, cb), 0);
		AssertDo(FEqualRgb(rgch, psz, cb), 0);
		grf |= 1L << cb;
		CopyPb(psz, rgch + cb, cb);
		AssertDo(pgg->FPut(iv, cb + cb, rgch), 0);
		}
	AssertDo(grf == 0x00010554, 0);

	grf = 0;
	for (iv = pgg->IvMac(); iv--; )
		{
		cb = pgg->Cb(iv);
		AssertDo(!(cb & 3), 0);
		cb /= 2;
		grf |= 1L << cb;
		pgg->DeleteRgb(iv, LwMin(cb, iv), cb);

		qb = (byte *)pgg->QvGet(iv);
		AssertDo(FEqualRgb(psz, qb, cb), 0);
		}
	AssertDo(grf == 0x00010554, 0);
	ReleasePpo(&pgg);
}


/***************************************************************************
	Test the chunky file stuff.
***************************************************************************/
void TestCfl(void)
{
	enum
		{
		relPaul,
		relMarge,
		relTig,
		relCarl,
		relPriscilla,
		relGreg,
		relShon,
		relClaire,
		relMike,
		relStephen,
		relBaby,
		relCathy,
		relJoshua,
		relRachel,
		relLim
		};

	struct EREL
		{
		CTG ctg;
		CNO cno;
		PSZ psz;
		short relPar1, relPar2;
		};

	const CTG kctgLan = 0x41414141;
	const CTG kctgKatz = 0x42424242;
	const CTG kctgSandy = 0x43434343;
	EREL dnrel[relLim] =
		{
		{ kctgLan, 0, PszLit("Paul"), relLim, relLim },
		{ kctgLan, 0, PszLit("Marge"), relLim, relLim },
		{ kctgLan, 0, PszLit("Tig"), relPaul, relMarge },
		{ kctgKatz, 0, PszLit("Carl"), relLim, relLim },
		{ kctgKatz, 0, PszLit("Priscilla"), relLim, relLim },
		{ kctgKatz, 0, PszLit("Greg"), relCarl, relPriscilla },
		{ kctgKatz, 0, PszLit("Shon"), relCarl, relPriscilla },
		{ kctgKatz, 0, PszLit("Claire"), relPaul, relMarge },
		{ kctgKatz, 0, PszLit("Mike"), relCarl, relPriscilla },
		{ kctgKatz, 0, PszLit("Stephen"), relGreg, relLim },
		{ kctgKatz, 0, PszLit("Baby"), relShon, relClaire },
		{ kctgSandy, 0, PszLit("Cathy"), relCarl, relPriscilla },
		{ kctgSandy, 0, PszLit("Joshua"), relCathy, relLim },
		{ kctgSandy, 0, PszLit("Rachel"), relCathy, relLim },
		};


	FNI fni, fniDst;
	PCFL pcfl, pcflDst;
	BLCK blck;
	short rel;
	long icki;
	CNO cno;
	CKI cki;
	EREL *perel, *perelPar;
	STN stn;
	achar rgch[kcchMaxSz];

	while (FGetFniSaveMacro(&fni, 'TEXT', "\x9" "Save As: ", "\x4" "Junk",
			PszLit("All files\0*.*\0"), NULL))
		{
		AssertDo((pcfl = CFL::PcflCreate(&fni, fcflNil)) != pvNil, 0);
		AssertDo(fniDst.FGetTemp(), 0);
		AssertDo((pcflDst = CFL::PcflCreate(&fniDst, fcflNil)) != pvNil, 0);

		for (rel = 0; rel < relLim; rel++)
			{
			perel = &dnrel[rel];
			AssertDo(pcfl->FAddPv(perel->psz, CchSz(perel->psz), perel->ctg,
				&perel->cno), 0);
			stn = perel->psz;
			AssertDo(pcfl->FSetName(perel->ctg, perel->cno, &stn), 0);
			if (perel->relPar1 < relLim)
				{
				perelPar = &dnrel[perel->relPar1];
				AssertDo(pcfl->FAdoptChild(perelPar->ctg, perelPar->cno,
					perel->ctg, perel->cno), 0);
				}
			if (perel->relPar2 < relLim)
				{
				perelPar = &dnrel[perel->relPar2];
				AssertDo(pcfl->FAdoptChild(perelPar->ctg, perelPar->cno,
					perel->ctg, perel->cno), 0);
				}
			AssertDo(pcfl->FCopy(perel->ctg, perel->cno, pcflDst, &cno),
				"copy failed");
			}
		AssertDo(pcfl->Ccki() == 14, 0);
		for (rel = 0; rel < relLim; rel++)
			{
			perel = &dnrel[rel];
			pcfl->FGetName(perel->ctg, perel->cno, &stn);
			AssertDo(FEqualRgb(stn.Prgch(), perel->psz, stn.Cch()), 0);
			AssertDo(pcfl->FFind(perel->ctg, perel->cno, &blck), 0);
			AssertDo(blck.FRead(rgch), 0);
			AssertDo(FEqualRgb(rgch, perel->psz, CchSz(perel->psz) * size(achar)), 0);
			}

		// copy all the chunks - they should already be there, but this
		// should set up all the child links
		for (rel = 0; rel < relLim; rel++)
			{
			perel = &dnrel[rel];
			AssertDo(pcfl->FCopy(perel->ctg, perel->cno, pcflDst, &cno),
				"copy failed");
			}
		AssertPo(pcflDst, fcflFull);

		// this should delete relShon, but not relBaby
		perelPar = &dnrel[relCarl];
		perel = &dnrel[relShon];
		pcfl->DeleteChild(perelPar->ctg, perelPar->cno, perel->ctg, perel->cno);
		perelPar = &dnrel[relPriscilla];
		pcfl->DeleteChild(perelPar->ctg, perelPar->cno, perel->ctg, perel->cno);
		AssertDo(pcfl->Ccki() == 13, 0);

		// this should delete relGreg and relStephen
		perelPar = &dnrel[relCarl];
		perel = &dnrel[relGreg];
		pcfl->DeleteChild(perelPar->ctg, perelPar->cno, perel->ctg, perel->cno);
		perelPar = &dnrel[relPriscilla];
		pcfl->DeleteChild(perelPar->ctg, perelPar->cno, perel->ctg, perel->cno);
		AssertDo(pcfl->Ccki() == 11, 0);

		// this should delete relCarl, relPriscilla, relCathy, relJoshua,
		// relRachel and relMike
		pcfl->Delete(perelPar->ctg, perelPar->cno);
		perelPar = &dnrel[relCarl];
		pcfl->Delete(perelPar->ctg, perelPar->cno);
		AssertDo(pcfl->Ccki() == 5, 0);

		for (icki = 0; pcfl->FGetCki(icki, &cki); icki++)
			{
			AssertDo(pcfl->FGetName(cki.ctg, cki.cno, &stn), 0);
			AssertDo(pcfl->FFind(cki.ctg, cki.cno, &blck), 0);
			AssertDo(blck.FRead(rgch), 0);
			AssertDo(FEqualRgb(rgch, stn.Prgch(), stn.Cch() * size(achar)), 0);
			AssertDo(stn.Cch() * size(achar) == blck.Cb(), 0);
			}

		// copy all the chunks back
		for (icki = 0; pcflDst->FGetCki(icki, &cki); icki++)
			{
			AssertDo(pcflDst->FCopy(cki.ctg, cki.cno, pcfl, &cno),
				"copy failed");
			}
		AssertPo(pcfl, fcflFull);
		AssertDo(pcfl->Ccki() == 14, 0);
		ReleasePpo(&pcflDst);

		AssertDo(pcfl->FSave(BigLittle('JUNK', 'KNUJ'), pvNil), 0);
		ReleasePpo(&pcfl);
		}

	while (FGetFniOpenMacro(&fni, pvNil, 0, PszLit("All files\0*.*\0"), NULL))
		{
		AssertDo(fni.TExists() == tYes, 0);
		pcfl = CFL::PcflOpen(&fni, fcflNil);
		if (pcfl == pvNil)
			continue;
		AssertPo(pcfl, 0);
		if (FGetFniSaveMacro(&fni, 'TEXT', "\x9" "Save As: ", "\x4" "Junk",
				PszLit("All files\0*.*\0"), NULL))
			{
			AssertDo(pcfl->FSave(BigLittle('JUNK', 'KNUJ'), &fni), 0);
			}
		ReleasePpo(&pcfl);
		}

	CFL::CloseUnmarked();
	CFL::ClearMarks();
	CFL::CloseUnmarked();
}

/******************************************************************************
	Test the error registration code.
******************************************************************************/
void TestErs(void)
{
	const long cercTest = 30;
	long erc, ercT;

	vpers->Clear();
	Assert(vpers->Cerc() == 0, "bad count of error codes on stack");

	for (erc = 0; erc < cercTest; erc++)
		{
		Assert(vpers->Cerc() == LwMin(erc, kcerdMax),
			"bad count of error codes on stack");
		PushErc(erc);
		AssertVar(vpers->FIn(erc), "error code not found", &erc);
		}

	for (erc = cercTest - 1; vpers->FIn(erc); erc--)
		;
	AssertVar(erc == cercTest - kcerdMax - 1, "lost error code", &erc);

	for (erc = 0; erc < vpers->Cerc(); erc++)
		{
		AssertVar((ercT = vpers->ErcGet(erc)) == cercTest - kcerdMax + erc,
			"invalid error code", &ercT);
		}

	for (erc = cercTest - 1; vpers->FPop(&ercT); erc--)
		AssertVar(ercT == erc, "bogus error code returned", &ercT);
	AssertVar(erc == cercTest - kcerdMax - 1, "lost error code", &erc);

	for (erc = 0; erc < cercTest; erc++)
		{
		Assert(vpers->Cerc() == LwMin(erc, kcerdMax),
			"bad count of error codes on stack");
		PushErc(erc);
		AssertVar(vpers->FIn(erc), "error code not found", &erc);
		}
	vpers->Clear();
	Assert(vpers->Cerc() == 0, "bad count of error codes on stack");
}


/******************************************************************************
	Test chunky resource file
******************************************************************************/
void TestCrf(void)
{
	const CNO cnoLim = 10;
	FNI fni;
	CTG ctg = 'JUNK';
	CNO cno;
	PGHQ rgpghq[cnoLim];
	PCFL pcfl;
	PCRF pcrf;
	HQ hq;
	PGHQ pghq;

	if (!fni.FGetTemp() || pvNil == (pcfl = CFL::PcflCreate(&fni, fcflWriteEnable | fcflTemp)))
		{
		Bug("creating chunky file failed");
		return;
		}

	for (cno = 0; cno < cnoLim; cno++)
		{
		AssertDo(pcfl->FPutPv("Test string", 11, ctg, cno), 0);
		}

	if (pvNil == (pcrf = CRF::PcrfNew(pcfl, 50)))
		{
		Bug("creating CRF failed");
		ReleasePpo(&pcfl);
		return;
		}
	ReleasePpo(&pcfl);

	for (cno = 0; cno < cnoLim; cno++)
		pcrf->TLoad(ctg, cno, GHQ::FReadGhq, rscNil, 10);

	for (cno = 0; cno < cnoLim; cno++)
		pcrf->TLoad(ctg, cno, GHQ::FReadGhq, rscNil, 20);

	for (cno = 0; cno < cnoLim; cno++)
		pcrf->TLoad(ctg, cno, GHQ::FReadGhq, rscNil, 20 + cno);

	for (cno = 0; cno < cnoLim; cno++)
		{
		pghq = (PGHQ)pcrf->PbacoFetch(ctg, cno, GHQ::FReadGhq);
		if (pvNil == pghq)
			continue;
		hq = pghq->hq;
		Assert(CbOfHq(hq) == 11, "wrong length");
		Assert(FEqualRgb(QvFromHq(hq), "Test string", 11), "bad bytes");
		ReleasePpo(&pghq);
		}

	for (cno = 0; cno < cnoLim; cno++)
		{
		pghq = (PGHQ)pcrf->PbacoFetch(ctg, cno, GHQ::FReadGhq);
		rgpghq[cno] = pghq;
		if (pvNil == pghq)
			continue;
		hq = pghq->hq;
		Assert(CbOfHq(hq) == 11, "wrong length");
		Assert(FEqualRgb(QvFromHq(hq), "Test string", 11), "bad bytes");
		}

	for (cno = 0; cno < cnoLim; cno++)
		{
		ReleasePpo(&rgpghq[cno]);
		}

	ReleasePpo(&pcrf);
}


