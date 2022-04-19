/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Masked bitmap routines that a tool might use.

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(MBMP)


/***************************************************************************
	Destructor for a masked bitmap.
***************************************************************************/
MBMP::~MBMP(void)
{
	AssertBaseThis(0);
	FreePhq(&_hqrgb);
}


/***************************************************************************
	Static method to create a new MBMP based on the given prgbPixels with
	extracting rectangle *prc and transparent color bTransparent.  prgbPixels
	should be a two-dimensional matrix of 8-bit pixels with width cbRow and
	height dyp.  prc should be the desired extracting rectangle.  xp and yp
	are the coordinates for the reference point of the MBMP (with (0,0) being
	the upper-left corner).  bTransparent should be the pixel value for the
	transparent color for the MBMP.
***************************************************************************/
PMBMP MBMP::PmbmpNew(byte *prgbPixels, long cbRow, long dyp, RC *prc,
	long xpRef, long ypRef, byte bTransparent, ulong grfmbmp, byte bDefault)
{
	AssertIn(cbRow, 1, kcbMax);
	AssertIn(dyp, 1, kcbMax);
	AssertPvCb(prgbPixels, LwMul(cbRow, dyp));
	AssertVarMem(prc);
	PMBMP pmbmp;

	if (pvNil != (pmbmp = NewObj MBMP) && !pmbmp->_FInit(prgbPixels, cbRow,
			dyp, prc, xpRef, ypRef, bTransparent, grfmbmp, bDefault))
		{
		ReleasePpo(&pmbmp);
		}
	return pmbmp;
}


/***************************************************************************
	Initialize the MBMP based on the given pixels.
***************************************************************************/
bool MBMP::_FInit(byte *prgbPixels, long cbRow, long dyp, RC *prc,
	long xpRef, long ypRef, byte bTransparent, ulong grfmbmp, byte bDefault)
{
	AssertIn(cbRow, 1, kcbMax);
	AssertIn(dyp, 1, kcbMax);
	AssertPvCb(prgbPixels, LwMul(cbRow, dyp));
	AssertVarMem(prc);
	Assert(!prc->FEmpty() && prc->xpLeft >= 0 && prc->xpRight <= cbRow &&
		prc->ypTop >= 0 && prc->ypBottom <= dyp, "Invalid rectangle");
	short *qrgcb;
	byte *pb, *pbRow, *pbLimRow;
	byte *qbDst, *qbDstPrev;
	long cbPixelData, cbPrev, cbOpaque, cbRun;
	long dibRow;
	bool fTrans, fMask;
	long xpMin, xpLim, ypLim, xp, yp;
	RC rc = *prc;

	// allocate enough space for the rgcb
	if (!FAllocHq(&_hqrgb, size(MBMPH) + LwMul(rc.Dyp(), size(short)),
			fmemNil, mprNormal))
		{
		return fFalse;
		}

	_Qmbmph()->fMask = fMask = FPure(grfmbmp & fmbmpMask);
	_Qmbmph()->bFill = bDefault;
	dibRow = (grfmbmp & fmbmpUpsideDown) ? -cbRow : cbRow;

	// crop the bitmap and get an upper bound on the size of the pixel data
	pbRow = prgbPixels + LwMul(cbRow,
		((grfmbmp & fmbmpUpsideDown) ? dyp - rc.ypTop - 1 : rc.ypTop));
	qrgcb = _Qrgcb();
	xpMin = rc.xpRight;
	xpLim = rc.xpLeft;
	ypLim = rc.ypTop;
	cbPixelData = 0;
	for (yp = rc.ypTop; yp < rc.ypBottom; pbRow += dibRow, yp++)
		{
		pb = pbRow + rc.xpLeft;
		pbLimRow = pbRow + rc.xpRight;
		cbPrev = cbPixelData;
		cbRun = cbOpaque = 0;
		fTrans = fTrue;
		for (;;)
			{
			// check for overflow or change in transparent status, or the
			// end of the row
			if (pb == pbLimRow || fTrans != (*pb == bTransparent) ||
					cbRun == kbMax)
				{
				if (fTrans && pb == pbLimRow)
					break;
				cbPixelData++;
				if (!fTrans && cbRun > 0)
					{
					xp = pb - pbRow;
					if (xpMin > xp - cbRun)
						xpMin = xp - cbRun;
					if (xpLim < xp)
						xpLim = xp;
					cbOpaque += cbRun;
					if (!fMask)
						cbPixelData += cbRun;
					}
				if (pb == pbLimRow)
					break;
				cbRun = 0;
				fTrans = !fTrans;
				}
			else
				{
				// Increment pixel count and go on to next pixel.
				cbRun++;
				pb++;
				}
			}

		if (0 == cbOpaque)
			{
			// nothing in this row but transparent pixels
			if (yp == rc.ypTop)
				rc.ypTop++;
			else
				qrgcb[yp - rc.ypTop] = 0;
			cbPixelData = cbPrev;
			}
		else
			{
			// set the row length in the rgcb.
			AssertIn(cbPixelData - cbPrev, 2 + !fMask, kswMax + 1);
			qrgcb[yp - rc.ypTop] = (short)(cbPixelData - cbPrev);
			ypLim = yp + 1;
			}
		}
	rc.ypBottom = ypLim;
	rc.xpLeft = xpMin;
	rc.xpRight = xpLim;
	if (rc.FEmpty())
		{
		Warn("Empty source bitmap for MBMP");
		rc.Zero();
		}

	//reallocate the _hqrgb to the size actually needed
	AssertIn(LwMul(rc.Dyp(), size(short)), 0,
		CbOfHq(_hqrgb) - size(MBMPH) + 1);

	_cbRgcb = LwMul(rc.Dyp(), size(short));
	if (!FResizePhq(&_hqrgb, _cbRgcb + size(MBMPH) + cbPixelData,
			fmemNil, mprNormal))
		{
		return fFalse;
		}

	//now actually construct the pixel data
	qrgcb = _Qrgcb();
	qbDst = (byte *)PvAddBv(qrgcb, _cbRgcb);
	pbRow = prgbPixels + LwMul(cbRow,
		((grfmbmp & fmbmpUpsideDown) ? dyp - rc.ypTop - 1 : rc.ypTop));
	for (yp = rc.ypTop; yp < rc.ypBottom; pbRow += dibRow, yp++)
		{
		if (qrgcb[yp - rc.ypTop] == 0)
			{
			//empty row, no need to scan it
			AssertIn(yp, rc.ypTop + 1, rc.ypBottom);
			continue;
			}

		pb = pbRow + rc.xpLeft;
		pbLimRow = pbRow + rc.xpRight;
		qbDstPrev = qbDst;
		cbRun = 0;
		fTrans = fTrue;
		for (;;)
			{
			// check for overflow or change in transparent status, or the
			// end of the row
			if (pb == pbLimRow || fTrans != (*pb == bTransparent) ||
					cbRun == kbMax)
				{
				if (fTrans && pb == pbLimRow)
					break;
				*qbDst++ = (byte)cbRun;
				if (!fTrans)
					{
					if (!fMask)
						{
						CopyPb(pb - cbRun, qbDst, cbRun);
						qbDst += cbRun;
						}
					if (pb == pbLimRow)
						break;
					}
				cbRun = 0;
				fTrans = !fTrans;
				}
			else
				{
				// Increment pixel count and go on to next pixel.
				cbRun++;
				pb++;
				}
			}

		// Set the row length in the rgcb.
		cbRun = qbDst - qbDstPrev;
		AssertIn(cbRun, 2 + !fMask, qrgcb[yp - rc.ypTop] + 1);
		qrgcb[yp - rc.ypTop] = (short)cbRun;
		}

	//shrink _hqrgb to the actual size needed
	cbRun = BvSubPvs(qbDst, QvFromHq(_hqrgb));
	AssertIn(cbRun, 0, CbOfHq(_hqrgb) + 1);
	AssertDo(FResizePhq(&_hqrgb, cbRun, fmemNil, mprNormal),
		"shrinking failed!");

	//set the bounding rectangle of the MBMP
	rc.Offset(-xpRef, -ypRef);
	_Qmbmph()->rc = rc;

	AssertThis(0);
	return fTrue;
}


/****************************************************************************
	This function will read in a bitmap file and return a PMBMP made from it.
	(xp, yp) will be the reference point of the mbmp [(0,0) is uppper-left].
	All pixels with the same value as bTransparent will be read in as
	transparent. The bitmap file must be uncompressed and have a bit depth
	of 8.  The palette information is be ignored.
****************************************************************************/
PMBMP MBMP::PmbmpReadNative(FNI *pfni, byte bTransparent, long xp, long yp,
	ulong grfmbmp, byte bDefault)
{
	AssertPo(pfni, ffniFile);
	byte *prgb;
	RC rc;
	long dxp, dyp;
	bool fUpsideDown;
	PMBMP pmbmp = pvNil;

	if (!FReadBitmap(pfni, &prgb, pvNil, &dxp, &dyp, &fUpsideDown, bTransparent))
		return pvNil;

	rc.Set(0, 0, dxp, dyp);
	pmbmp = MBMP::PmbmpNew(prgb, CbRoundToLong(rc.xpRight),
		rc.ypBottom, &rc, xp, yp, bTransparent,
		(fUpsideDown ? grfmbmp : grfmbmp ^ fmbmpUpsideDown), bDefault);
	FreePpv((void **)&prgb);

	return pmbmp;
}


/***************************************************************************
	Read a masked bitmap from a block.  May free the block or modify it.
***************************************************************************/
PMBMP MBMP::PmbmpRead(PBLCK pblck)
{
	AssertPo(pblck, 0);
	PMBMP pmbmp;
	MBMPH *qmbmph;
	long cbRgcb;
	long cbTot;
	bool fSwap;
	RC rc;
	HQ hqrgb = hqNil;

	if (!pblck->FUnpackData())
		return pvNil;
	cbTot = pblck->Cb();

	if (cbTot < size(MBMPH) || hqNil == (hqrgb = pblck->HqFree()))
		return pvNil;

	qmbmph = (MBMPH *)QvFromHq(hqrgb);
	fSwap = (kboOther == qmbmph->bo);
	if (fSwap)
		SwapBytesBom(qmbmph, kbomMbmph);
	else if (qmbmph->bo != kboCur)
		goto LFail;

	if (qmbmph->swReserved != 0 || qmbmph->cb != cbTot)
		goto LFail;

	rc = qmbmph->rc;
	if (rc.FEmpty())
		{
		if (cbTot != size(MBMPH))
			goto LFail;
		qmbmph->rc.xpRight = rc.xpLeft;
		qmbmph->rc.ypBottom = rc.ypTop;
		}

	cbRgcb = LwMul(rc.Dyp(), size(short));
	if (size(MBMPH) + cbRgcb > cbTot)
		goto LFail;

	if (pvNil == (pmbmp = NewObj MBMP))
		{
LFail:
		FreePhq(&hqrgb);
		return pvNil;
		}

	pmbmp->_cbRgcb = cbRgcb;
	pmbmp->_hqrgb = hqrgb;

	if (fSwap)
		{
		//swap bytes in the rgcb
		SwapBytesRgsw(pmbmp->_Qrgcb(), rc.Dyp());
		}

#ifdef DEBUG
	//verify the rgcb and rgb
	long ccb, cb, dxp;
	byte *qb;
	bool fMask = pmbmp->_Qmbmph()->fMask;
	short *qcb = pmbmp->_Qrgcb();
	byte *qbRow = (byte *)PvAddBv(qcb, cbRgcb);

	cbTot -= size(MBMPH) + cbRgcb;
	for (ccb = rc.Dyp(); ccb-- > 0; )
		{
		cb = *qcb++;
		if (!FIn(cb, 0, cbTot + 1))
			goto LFailDebug;
		cbTot -= cb;
		qb = qbRow;
		dxp = 0;
		while (qb < qbRow + cb)
			{
			dxp += *qb++;
			if (qb >= qbRow + cb)
				break;
			dxp += *qb;
			if (fMask)
				qb++;
			else
				qb += *qb + 1;
			}
		if (dxp > rc.Dxp() || qb != qbRow + cb)
			goto LFailDebug;
		qbRow = qb;
		}
	if (cbTot != 0)
		{
LFailDebug:
		Bug("Attempted to read bad MBMP");
		ReleasePpo(&pmbmp);
		}
#endif //DEBUG

	AssertNilOrPo(pmbmp, 0);
	return pmbmp;
}


/***************************************************************************
	Return the total size on file.
***************************************************************************/
long MBMP::CbOnFile(void)
{
	AssertThis(0);
	return CbOfHq(_hqrgb);
}


/***************************************************************************
	Write the masked bitmap (and its header) to the given block.
***************************************************************************/
bool MBMP::FWrite(PBLCK pblck)
{
	AssertThis(0);
	AssertPo(pblck, 0);
	MBMPH *qmbmph;

	qmbmph = _Qmbmph();
	qmbmph->bo = kboCur;
	qmbmph->osk = koskCur;
	qmbmph->swReserved = 0;
	qmbmph->cb = CbOfHq(_hqrgb);

	if (qmbmph->cb != pblck->Cb())
		{
		Bug("Wrong sized block");
		return fFalse;
		}

	if (!pblck->FWriteHq(_hqrgb, 0))
		return fFalse;

	return fTrue;
}


/***************************************************************************
	Get the natural rectangle for the mbmp.
***************************************************************************/
void MBMP::GetRc(RC *prc)
{
	AssertThis(0);
	*prc = _Qmbmph()->rc;
}


/***************************************************************************
	Return whether the given (xp, yp) is in a non-transparent pixel of
	the MBMP.  (xp, yp) should be given in MBMP coordinates.
***************************************************************************/
bool MBMP::FPtIn(long xp, long yp)
{
	AssertThis(0);
	byte *qb, *qbLim;
	short *qcb;
	short cb;
	MBMPH *qmbmph;

	qmbmph = _Qmbmph();
	if (!qmbmph->rc.FPtIn(xp, yp))
		return fFalse;

	qcb = _Qrgcb();
	qb = (byte *)PvAddBv(qcb, _cbRgcb);
	for (yp -= qmbmph->rc.ypTop; yp-- > 0; )
		qb += *qcb++;

	qbLim = qb + *qcb;
	for (xp -= qmbmph->rc.xpLeft; qb < qbLim; )
		{
		if (0 > (xp -= *qb++) || qb >= qbLim)
			break;
		cb = *qb++;
		if (0 > (xp -= cb))
			return fTrue;
		if (!qmbmph->fMask)
			qb += cb;
		}
	Assert(qb <= qbLim, "bad row in MBMP");
	return fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a MBMP.
***************************************************************************/
void MBMP::AssertValid(ulong grf)
{
	long ccb;
	long cbTot;
	short *qcb;
	RC rc;

	MBMP_PAR::AssertValid(0);
	AssertHq(_hqrgb);

	rc = _Qmbmph()->rc;
	ccb = rc.Dyp();
	Assert(_cbRgcb == LwMul(rc.Dyp(), size(short)),
		"_cbRgcb wrong");
	cbTot = 0;
	qcb = _Qrgcb();
	while (ccb-- > 0)
		{
		AssertIn(*qcb, 0, kcbMax);
		cbTot += *qcb++;
		}
	Assert(cbTot + _cbRgcb + size(MBMPH) == CbOfHq(_hqrgb),
		"_hqrgb wrong size");
}


/***************************************************************************
	Mark memory for the MBMP.
***************************************************************************/
void MBMP::MarkMem(void)
{
	AssertValid(0);
	MBMP_PAR::MarkMem();
	MarkHq(_hqrgb);
}
#endif //DEBUG


/***************************************************************************
	A PFNRPO to read an MBMP.
***************************************************************************/
bool MBMP::FReadMbmp(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, fblckReadable);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);
	PMBMP pmbmp;

	*pcb = pblck->Cb(fTrue);
	if (pvNil == ppbaco)
		return fTrue;

	if (!pblck->FUnpackData())
		goto LFail;
	*pcb = pblck->Cb();

	if (pvNil == (pmbmp = PmbmpRead(pblck)))
		{
LFail:
		TrashVar(ppbaco);
		TrashVar(pcb);
		return fFalse;
		}
	*ppbaco = pmbmp;
	return fTrue;
}


/***************************************************************************
	Given an FNI refering to a bitmap file, returns the interesting parts of
	the header and the pixel data and palette.  Fails if the bitmap is not
	8 bits, uncompressed.  Any or all of the output pointers may be nil.

	input:
		pfni	--	the file from which to read the data

	output:
		The following variable parameters are valid only if FReadBitmap
		returns true (*pprgb and *ppglclr will be nil if this fails):
			pprgb			-- pointer to the allocated memory for pixel data
			ppglclr			-- the palette
			pdxp			-- the width of the bitmap
			pdyp			-- the height of the bitmap
			pfUpsideDown	--	fTrue if the bitmap is upside down
		returns fTrue if it succeeds
***************************************************************************/
bool FReadBitmap(FNI *pfni, byte **pprgb, PGL *ppglclr, long *pdxp, long *pdyp,
	bool *pfUpsideDown, byte bTransparent)
{
	AssertPo(pfni, ffniFile);
	AssertNilOrVarMem(pprgb);
	AssertNilOrVarMem(ppglclr);
	AssertNilOrVarMem(pdxp);
	AssertNilOrVarMem(pdyp);
	AssertNilOrVarMem(pfUpsideDown);

#ifdef WIN
	#pragma pack(2) //the stupid bmfh is an odd number of shorts
	struct BMH
		{
		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER bmih;
		};
	#pragma pack()

	PFIL pfil;
	RC rc;
	long fpMac, cbBitmap, cbSrc;
	BMH bmh;
	bool fRet, fRle;
	FP fpCur = 0;

	if (pvNil != pprgb)
		*pprgb = pvNil;
	if (pvNil != ppglclr)
		*ppglclr = pvNil;

	if (pvNil == (pfil = FIL::PfilOpen(pfni)))
		return fFalse;
	fpMac = pfil->FpMac();
	if (size(BMH) >= fpMac || !pfil->FReadRgbSeq(&bmh, size(BMH), &fpCur))
		goto LFail;

	fRle = (bmh.bmih.biCompression == BI_RLE8);
	cbSrc = bmh.bmih.biSizeImage;
	if (((long)bmh.bmfh.bfSize != fpMac) ||
		bmh.bmfh.bfType != 'MB' || !FIn(bmh.bmfh.bfOffBits, size(BMH), fpMac) ||
		bmh.bmfh.bfReserved1 != 0 || bmh.bmfh.bfReserved2 != 0 ||
		bmh.bmih.biSize != size(bmh.bmih) || bmh.bmih.biPlanes != 1)
		{
		Warn("bad bitmap file");
		goto LFail;
		}
	if (bmh.bmih.biBitCount != 8)
		{
		Warn("not an 8-bit bitmap");
		goto LFail;
		}
	if (bmh.bmih.biCompression != BI_RGB && !fRle ||
		cbSrc != fpMac - (long)bmh.bmfh.bfOffBits && (cbSrc != 0 || fRle))
		{
		Warn("bad compression type or bitmap file is wrong length");
		goto LFail;
		}

	rc.Set(0, 0, bmh.bmih.biWidth, LwAbs(bmh.bmih.biHeight));
	cbBitmap = CbRoundToLong(rc.xpRight) * rc.ypBottom;
	if (rc.FEmpty())
		{
		Warn("Empty bitmap rectangle");
		goto LFail;
		}
	if (!fRle)
		{
		if (cbSrc == 0)
			cbSrc = cbBitmap;
		else if (cbSrc != cbBitmap)
			{
			Warn("Bitmap data wrong size");
			goto LFail;
			}
		}

	if (pvNil != ppglclr)
		{
		// get the palette
		if (bmh.bmih.biClrUsed != 0 && bmh.bmih.biClrUsed != 256)
			{
			Warn("palette is wrong size");
			goto LFail;
			}

		if (pvNil == (*ppglclr = GL::PglNew(size(CLR), 256)))
			goto LFail;

		AssertDo((*ppglclr)->FSetIvMac(256), 0);
		fRet = pfil->FReadRgbSeq((*ppglclr)->PvLock(0),
			LwMul(size(CLR), 256), &fpCur);
		(*ppglclr)->Unlock();
		if (!fRet)
			goto LFail;
		}

	if (pvNil == pprgb)
		goto LDone;

	if (fRle)
		{
		byte *pbSrc, *pbDst, *prgbSrc, *pbLimSrc, *pbLimDst;
		byte bT;
		long xp, cbT;
		long cbRowDst = CbRoundToLong(rc.xpRight);

		// get the source
		if (!FAllocPv((void **)&prgbSrc, cbSrc, fmemNil, mprNormal))
			goto LFail;

		if (!pfil->FReadRgb(prgbSrc, cbSrc, bmh.bmfh.bfOffBits) ||
			!FAllocPv((void **)pprgb, cbBitmap, fmemNil, mprNormal))
			{
			goto LBad;
			}
		pbDst = *pprgb;
		pbSrc = prgbSrc;

		pbLimSrc = pbSrc + cbSrc;
		pbLimDst = pbDst + cbBitmap;
		xp = 0;
		for (;;)
			{
			if (2 > pbLimSrc - pbSrc)
				goto LBad;

			bT = *pbSrc++;
			if (bT != 0)
				{
				if (bT > pbLimDst - pbDst || bT > cbRowDst - xp)
					goto LBad;
				FillPb(pbDst, bT, *pbSrc++);
				pbDst += bT;
				xp += bT;
				continue;
				}

			// escaped
			bT = *pbSrc++;
			switch (bT)
				{
			case 0:
				// end of line
				if (cbRowDst > xp)
					FillPb(pbDst, cbRowDst - xp, bTransparent);
				pbDst += cbRowDst - xp;
				xp = 0;
				break;

			case 1:
				// end of bitmap
				if (pbLimDst > pbDst)
					FillPb(pbDst, pbLimDst - pbDst, bTransparent);
				goto LRleDone;

			case 2:
				// delta
				if (2 > pbLimSrc - pbSrc)
					goto LBad;
				cbT = pbSrc[0] + cbRowDst * pbSrc[1];
				if (cbT > pbLimDst - pbDst || pbSrc[0] > cbRowDst - xp)
					goto LBad;
				FillPb(pbDst, cbT, bTransparent);
				pbDst += cbT;
				xp += pbSrc[0];
				pbSrc += 2;
				break;

			default:
				// literal run
				cbT = LwRoundAway(bT, 2);
				if (cbT > pbLimSrc - pbSrc || bT > pbLimDst - pbDst ||
						bT > cbRowDst - xp)
					{
					goto LBad;
					}
				CopyPb(pbSrc, pbDst, bT);
				pbDst += bT;
				pbSrc += cbT;
				xp += bT;
				break;
				}
			}
LBad:
		// rle encoding is bad
		FreePpv((void **)&prgbSrc);
		Warn("compressed bitmap is bad");
		goto LFail;

LRleDone:
		FreePpv((void **)&prgbSrc);
		}
	else
		{
		// non-rle: get the bits
		if (!FAllocPv((void **)pprgb, cbBitmap, fmemNil, mprNormal))
			goto LFail;

		if (!pfil->FReadRgb(*pprgb, cbBitmap, bmh.bmfh.bfOffBits))
			{
			FreePpv((void **)pprgb);
LFail:
			PushErc(ercMbmpCantOpenBitmap);
			if (pvNil != pprgb)
				*pprgb = pvNil;
			if (pvNil != ppglclr)
				ReleasePpo(ppglclr);
			TrashVar(pdxp);
			TrashVar(pdyp);
			TrashVar(pfUpsideDown);
			ReleasePpo(&pfil);
			return fFalse;
			}
		}

LDone:
	if (pvNil != pdxp)
		*pdxp = bmh.bmih.biWidth;
	if (pvNil != pdyp)
		*pdyp = LwAbs(bmh.bmih.biHeight);
	if (pvNil != pfUpsideDown)
		*pfUpsideDown = bmh.bmih.biHeight < 0;
	ReleasePpo(&pfil);
	return fTrue;
#endif //WIN
#ifdef MAC
	if (pvNil != pprgb)
		*pprgb = pvNil;
	if (pvNil != ppglclr)
		*ppglclr = pvNil;

	TrashVar(pdxp);
	TrashVar(pdyp);
	TrashVar(pfUpsideDown);
	RawRtn();	// REVIEW peted: Mac FReadBitmap NYI
	return fFalse;
#endif //MAC
}


/***************************************************************************
	Writes a given bitmap to a given file.

	Arguments:
		FNI *pfni         -- the name of the file to write
		byte *prgb        -- the bits in the bitmap
		PGL pglclr        -- the palette of the bitmap
		long dxp          -- the width of the bitmap
		long dyp          -- the height of the bitmap
		bool fUpsideDown  -- indicates if the rows should be inverted

	Returns: fTrue if it could write the file
***************************************************************************/
bool FWriteBitmap(FNI *pfni, byte *prgb, PGL pglclr, long dxp, long dyp,
	bool fUpsideDown)
{
	AssertPo(pfni, ffniFile);
	AssertVarMem(prgb);
	AssertPo(pglclr, 0);
	Assert(pglclr->IvMac() == 256, "Invalid color palette");
	Assert(dxp >= 0, "Invalid width");
	Assert(dyp >= 0, "Invalid height");

#ifdef WIN
	Assert(pglclr->CbEntry() == size(RGBQUAD),
		"Palette has different format from Windows");

	#pragma pack(2) //the stupid bmfh is an odd number of shorts
	struct BMH
		{
		BITMAPFILEHEADER bmfh;
		BITMAPINFOHEADER bmih;
		};
	#pragma pack()

	bool fRet = fFalse;
	long cbSrc;
	PFIL pfil = pvNil;
	FP fpCur = 0;
	BMH bmh;

	cbSrc = CbRoundToLong(dxp) * dyp;

	/* Fill in the header */
	bmh.bmfh.bfType = 'MB';
	bmh.bmfh.bfSize = size(bmh) + LwMul(size(RGBQUAD), 256) + cbSrc;
	bmh.bmfh.bfOffBits = size(bmh) + LwMul(size(RGBQUAD), 256);
	bmh.bmfh.bfReserved1 = bmh.bmfh.bfReserved2 = 0;

	bmh.bmih.biSize = size(bmh.bmih);
	bmh.bmih.biWidth = dxp;
	bmh.bmih.biHeight = dyp;
	bmh.bmih.biPlanes = 1;
	bmh.bmih.biBitCount = 8;
	bmh.bmih.biCompression = BI_RGB;
	bmh.bmih.biSizeImage = 0;
	bmh.bmih.biXPelsPerMeter = 0;
	bmh.bmih.biYPelsPerMeter = 0;
	bmh.bmih.biClrUsed = 256;
	bmh.bmih.biClrImportant = 256;

	/* Write the header */
	if (pvNil == (pfil = FIL::PfilCreate(pfni)))
		goto LFail;
	if (!pfil->FWriteRgbSeq(&bmh, size(BMH), &fpCur))
		goto LFail;

	/* Write the palette */
	if (!pfil->FWriteRgbSeq(pglclr->PvLock(0), LwMul(size(CLR), 256), &fpCur))
		{
		pglclr->Unlock();
		goto LFail;
		}
	pglclr->Unlock();

	/* Write the bits */
	Assert((ulong)fpCur == bmh.bmfh.bfOffBits,
		"Current file pos is wrong");
	if (fUpsideDown)
		{
		byte *pbCur;
		long cbRow = CbRoundToLong(dxp);

		pbCur = prgb + dyp * cbRow;
		fRet = fTrue;
		while (pbCur > prgb && fRet)
			{
			pbCur -= cbRow;
			fRet = pfil->FWriteRgbSeq(pbCur, cbRow, &fpCur);
			}
		}
	else
		fRet = pfil->FWriteRgbSeq(prgb, cbSrc, &fpCur);

LFail:
	if (pfil != pvNil)
		{
		if (!fRet)
			pfil->SetTemp();
		ReleasePpo(&pfil);
		}
	return fRet;
#endif //WIN
#ifdef MAC
	RawRtn();	// REVIEW peted: Mac FWriteBitmap NYI
	return fFalse;
#endif //MAC
}

