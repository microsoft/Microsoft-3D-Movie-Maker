/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Standard Kauai codec.

	This codec compresses a byte stream into a bit stream. The bit stream
	is decoded by the following pseudo code:

		loop forever
			if (next bit is 0)
				// literal byte
				write next 8 bits
				continue
	
			// initialize length
			length = 1

			// find the offset
			if (next bit is 0)
				offset = next 6 bits + 0x01
			else if (next bit is 0)
				offset = next 9 bits + 0x41
			else if (next bit is 0)
				offset = next 12 bits + 0x0241
			else
				offset = next 20 bits
				if (offset is 0xFFFFF)
					break // we're done
				offset += 0x1241
				length++

			// get the length
			c = number of non-zero bits (up to 11)
			skip the zero bit
			length += next c bits + (1 << c)

			// write the destination bytes
			write length bytes from the current dest position minus offset.

	Essentially, previous uncompressed byte strings form a dictionary for
	the compression.

	As an example, the byte string consisting of 37 a's in a row would be
	compressed as:

		// first a encoded:
		0 			// literal byte
		01100001 	// 'a' = 0x61

		// next 36 a's encoded:
		01			// 6 bit offset
		000000		// offset = 0 + 1 = 1
		011111		// length encoded in next 5 bits
		00011		// length = 1 + 3 + (1 << 5) = 36

		1111		// 20 bit offset
		11111111111111111111	// special 20 bit offset indicating end of data

	The source stream is 296 bits long. The compressed stream is 28 bits (not
	counting the 24 bit termination).

	For a less trivial example: the string abaabaaabaaaab would be compressed
	to the following bit stream:

		// first aba encoded:
		0 			// literal byte
		01100001 	// 'a' = 0x61
		0			// literal byte
		01100010	// 'b = 0x62
		0 			// literal byte
		01100001 	// 'a' = 0x61

		// next abaa encoded:
		01			// 6 bit offset
		000010		// offset = 2 + 1 = 3
		01			// length encoded in next 1 bit
		1			// length = 1 + 1 + (1 << 1) = 4

		// next abaaa encoded:
		01			// 6 bit offset
		000011		// offset = 3 + 1 = 4
		011			// length encoded in next 2 bits
		00			// length = 1 + 0 + (1 << 2) = 5

		// next ab encoded:
		01			// 6 bit offset
		000100		// offset = 4 + 1 = 5
		0			// length encoded in next 0 bits
					// length = 1 + 0 + (1 << 0) = 2

		1111		// 20 bit offset
		11111111111111111111	// special 20 bit offset indicating end of data

	The uncompressed stream is 112 bits. The compressed stream is 60 bits (not
	including the 24 bit termination).

	All compressed streams are padded with extra 0xFF bytes so that at
	decompression time we can verify a priori that decompression will
	terminate (by verifying that there are at least 6 0xFF bytes at the end.

***************************************************************************/
#include "util.h"
#include "codkpri.h"
ASSERTNAME

// REVIEW shonk: should we turn on _Safety?
#define SAFETY

RTCLASS(KCDC)


/***************************************************************************
	Encode or decode a block.
***************************************************************************/
bool KCDC::FConvert(bool fEncode, long cfmt, void *pvSrc, long cbSrc,
	void *pvDst, long cbDst, long *pcbDst)
{
	AssertThis(0);
	AssertIn(cbSrc, 1, kcbMax);
	AssertPvCb(pvSrc, cbSrc);
	AssertIn(cbDst, 1, kcbMax);
	AssertPvCb(pvDst, cbDst);
	AssertVarMem(pcbDst);

	switch (cfmt)
		{
	default:
		return fFalse;

	case kcfmtKauai2:
		if (fEncode)
			return _FEncode2(pvSrc, cbSrc, pvDst, cbDst, pcbDst);
		return _FDecode2(pvSrc, cbSrc, pvDst, cbDst, pcbDst);

	case kcfmtKauai:
		if (fEncode)
			return _FEncode(pvSrc, cbSrc, pvDst, cbDst, pcbDst);
		return _FDecode(pvSrc, cbSrc, pvDst, cbDst, pcbDst);
		}
}


/***************************************************************************
	Bit array class - for writing the compressed data.
***************************************************************************/
class BITA
	{
protected:
	byte *_prgb;
	long _cb;

	long _ibit;
	long _ib;

public:
	void Set(void *pvDst, long cbDst);
	bool FWriteBits(ulong lu, long cbit);
	bool FWriteLogEncoded(ulong lu);
	long Ibit(void)
		{ return _ibit; }
	long Ib(void)
		{ return _ib; }
	};


/***************************************************************************
	Set the buffer to write to.
***************************************************************************/
void BITA::Set(void *pvDst, long cbDst)
{
	AssertPvCb(pvDst, cbDst);

	_prgb = (byte *)pvDst;
	_cb = cbDst;
	_ibit = _ib = 0;
}


/***************************************************************************
	Write some bits.
***************************************************************************/
bool BITA::FWriteBits(ulong lu, long cbit)
{
	long cb;

	// store the partial byte
	if (_ibit > 0)
		{
		AssertIn(_ib, 0, _cb);
		_prgb[_ib] = (byte)((_prgb[_ib] & ((1 << (_ibit)) - 1)) |
			(lu << _ibit));
		if (_ibit + cbit < 8)
			{
			_ibit += cbit;
			return fTrue;
			}
		cbit -= 8 - _ibit;
		lu >>= 8 - _ibit;
		_ib++;
		_ibit = 0;
		}

	Assert(_ibit == 0, 0);

	cb = cbit >> 3;
	cbit &= 0x07;

	if (_ib + cb + (cbit > 0) > _cb)
		return fFalse;

	while (cb-- > 0)
		{
		_prgb[_ib++] = (byte)lu;
		lu >>= 8;
		}

	if (cbit > 0)
		{
		_prgb[_ib] = (byte)lu;
		_ibit = cbit;
		}

	return fTrue;
}


/***************************************************************************
	Write the value logarithmically encoded.
***************************************************************************/
bool BITA::FWriteLogEncoded(ulong lu)
{
	Assert(lu > 0 && !(lu & 0x80000000),
		"bad value to encode logarithmically");
	long cbit;

	for (cbit = 1; (ulong)(1L << cbit) <= lu; cbit++)
		;
	cbit--;

	if (cbit > 0 && !FWriteBits(0xFFFFFFFF, cbit))
		return fFalse;

	return FWriteBits(lu << 1, cbit + 1);
}


/***************************************************************************
	Compress the data in pvSrc using the KCDC encoding.  Returns false if
	the data can't be compressed. This is not optimized (ie, it's slow).
***************************************************************************/
bool KCDC::_FEncode(void *pvSrc, long cbSrc, void *pvDst, long cbDst,
	long *pcbDst)
{
	AssertThis(0);
	AssertIn(cbSrc, 1, kcbMax);
	AssertPvCb(pvSrc, cbSrc);
	AssertIn(cbDst, 1, kcbMax);
	AssertPvCb(pvDst, cbDst);
	AssertVarMem(pcbDst);

	long ibSrc;
	long ibMatch, ibTest, cbMatch, ibMin, cbT;
	byte bMatchNew, bMatchLast;
	BITA bita;
	long *pmpsuibStart = pvNil;
	byte *prgbSrc = (byte *)pvSrc;
	long *pmpibibNext = pvNil;

	TrashVar(pcbDst);
	if (cbDst - kcbTailKcdc <= 1)
		return fFalse;

	// allocate the links
	if (!FAllocPv((void **)&pmpibibNext, LwMul(size(long), cbSrc),
			fmemNil, mprNormal) ||
		!FAllocPv((void **)&pmpsuibStart, LwMul(size(long), 0x10000),
			fmemNil, mprNormal))
		{
		Warn("failed to allocate memory for links");
		goto LFail;
		}

	// write the links
	// we need to set each entry of pmpsuibStart to a big negative number,
	// but not too big, or we risk overflow below.
	FillPb(pmpsuibStart, LwMul(size(long), 0x10000), 0xCC);
	for (ibSrc = 0; ibSrc < cbSrc - 1; ibSrc++)
		{
		ushort suCur = ((ushort)prgbSrc[ibSrc]) << 8 |
			(ushort)prgbSrc[ibSrc + 1];
		ibTest = pmpsuibStart[suCur];
		pmpsuibStart[suCur] = ibSrc;
		pmpibibNext[ibSrc] = ibTest;
		}
	pmpibibNext[cbSrc - 1] = 0xCCCCCCCCL;
	FreePpv((void **)&pmpsuibStart);

	// write flags byte
	bita.Set(pvDst, cbDst);
	AssertDo(bita.FWriteBits(0, 8), 0);

	for (ibSrc = 0; ibSrc < cbSrc; ibSrc += cbMatch)
		{
		byte *pbMatch;
		long cbMaxMatch;

		// get the new byte and the link
		ibTest = pmpibibNext[ibSrc];
		Assert(ibTest < ibSrc, 0);

		// assume we'll store a literal
		cbMatch = 1;
		ibMatch = ibSrc;

		if (ibTest <= (ibMin = ibSrc - kdibMinKcdc4))
			goto LStore;

		// we've seen this byte pair before - look for the longest match
		cbMaxMatch = LwMin(kcbMaxLenKcdc, cbSrc - ibSrc);
		pbMatch = prgbSrc + ibSrc;
		bMatchNew = prgbSrc[ibSrc + 1];
		bMatchLast = prgbSrc[ibSrc];
		for ( ; ibTest > ibMin; ibTest = pmpibibNext[ibTest])
			{
			Assert(prgbSrc[ibTest + 1] == prgbSrc[ibSrc + 1], "links are wrong");
			if (prgbSrc[ibTest + cbMatch] != bMatchNew ||
				prgbSrc[ibTest + cbMatch - 1] != bMatchLast ||
				cbMatch >= (cbT = CbEqualRgb(prgbSrc + ibTest,
					pbMatch, cbMaxMatch)))
				{
				Assert(pmpibibNext[ibTest] < ibTest, 0);
				continue;
				}

			AssertIn(cbT, cbMatch + 1, kcbMaxLenKcdc + 1);

			// if this run requires a 20 bit offset, we need to beat a
			// 9 or 6 bit offset by 2 bytes
			if (ibSrc - ibTest < kdibMinKcdc3 || cbT - cbMatch > 1 ||
					ibSrc - ibMatch >= kdibMinKcdc2)
				{
				cbMatch = cbT;
				ibMatch = ibTest;
				if (cbMatch == cbMaxMatch)
					break;
				bMatchNew = prgbSrc[ibSrc + cbMatch];
				bMatchLast = prgbSrc[ibSrc + cbMatch - 1];
				}

			Assert(pmpibibNext[ibTest] < ibTest, 0);
			}

		// write out the bits
LStore:
		AssertIn(ibMatch, 0, ibSrc + (cbMatch == 1));
		AssertIn(cbMatch,
			1 + (ibMatch < ibSrc) + (ibSrc - ibMatch >= kdibMinKcdc3),
			kcbMaxLenKcdc + 1);

		if (cbMatch == 1)
			{
			// literal
			if (!bita.FWriteBits((ulong)prgbSrc[ibSrc] << 1, 9))
				goto LFail;
			}
		else
			{
			// find the offset
			ulong luCode, luLen;
			long cbit;

			luCode = ibSrc - ibMatch;
			luLen = cbMatch - 1;
			if (luCode < kdibMinKcdc1)
				{
				// 6 bit offset
				cbit = 2 + kcbitKcdc0;
				luCode = ((luCode - kdibMinKcdc0) << 2) | 1;
				}
			else if (luCode < kdibMinKcdc2)
				{
				// 9 bit offset
				cbit = 3 + kcbitKcdc1;
				luCode = ((luCode - kdibMinKcdc1) << 3) | 0x03;
				}
			else if (luCode < kdibMinKcdc3)
				{
				// 12 bit offset
				cbit = 4 + kcbitKcdc2;
				luCode = ((luCode - kdibMinKcdc2) << 4) | 0x07;
				}
			else
				{
				// 20 bit offset
				Assert(luCode < kdibMinKcdc4, 0);
				cbit = 4 + kcbitKcdc3;
				luCode = ((luCode - kdibMinKcdc3) << 4) | 0x0F;
				luLen--;
				}
			AssertIn(cbit, 8, kcbitKcdc3 + 5);
			Assert(luLen > 0, "bad len");

			if (!bita.FWriteBits(luCode, cbit))
				goto LFail;

			if (!bita.FWriteLogEncoded(luLen))
				goto LFail;
			}
		}

	// fill the remainder of the last byte with 1's
	if (bita.Ibit() > 0)
		AssertDo(bita.FWriteBits(0xFF, 8 - bita.Ibit()), 0);

	// write the tail (kcbTailKcdc bytes of FF)
	for (cbT = 0; cbT < kcbTailKcdc; cbT += size(long))
		{
		if (!bita.FWriteBits(0xFFFFFFFF,
				LwMin(size(long), kcbTailKcdc - cbT) << 3))
			{
			goto LFail;
			}
		}

	*pcbDst = bita.Ib();

	FreePpv((void **)&pmpibibNext);
	return fTrue;

LFail:
	// can't compress the source
	FreePpv((void **)&pmpibibNext);
	return fFalse;
}


/***************************************************************************
	Decompress a compressed KCDC stream.
***************************************************************************/
bool KCDC::_FDecode(void *pvSrc, long cbSrc, void *pvDst, long cbDst,
	long *pcbDst)
{
	AssertThis(0);
	AssertIn(cbSrc, 1, kcbMax);
	AssertPvCb(pvSrc, cbSrc);
	AssertIn(cbDst, 1, kcbMax);
	AssertPvCb(pvDst, cbDst);
	AssertVarMem(pcbDst);

	long ib;
	byte bFlags;

	TrashVar(pcbDst);
	if (cbSrc <= kcbTailKcdc + 1)
		{
		Bug("bad source stream");
		return fFalse;
		}

	// verify that the last kcbTailKcdc bytes are FF.  This guarantees that
	// we won't run past the end of the source.
	for (ib = 0; ib++ < kcbTailKcdc; )
		{
		if (((byte *)pvSrc)[cbSrc - ib] != 0xFF)
			{
			Bug("bad tail of compressed data");
			return fFalse;
			}
		}

	bFlags = ((byte *)pvSrc)[0];
	if (bFlags != 0)
		{
		Bug("unknown flag byte");
		return fFalse;
		}

#ifdef IN_80386

	#include "kcdc_386.h"

	*pcbDst = cbTot;
	return fTrue;

#else //!IN_80386

	long cb, dib, ibit, cbit;
	register ulong luCur;
	byte *pbT;
	register byte *pbDst = (byte *)pvDst;
	byte *pbLimDst = (byte *)pvDst + cbDst;
	register byte *pbSrc = (byte *)pvSrc + 1;

	#define _FTest(ibit) (luCur & (1L << (ibit)))
	#ifdef LITTLE_ENDIAN
		#define _Advance(cb) ((pbSrc += (cb)), (luCur = *(ulong *)(pbSrc - 4)))
	#else //!LITTLE_ENDIAN
		#define _Advance(cb) ((pbSrc += (cb)), \
			(luCur = LwFromBytes(pbSrc[-1], pbSrc[-2], pbSrc[-3], pbSrc[-4])))
	#endif //!LITTLE_ENDIAN

	_Advance(4);

	for (ibit = 0; ; )
		{
		if (!_FTest(ibit))
			{
			// literal
#ifdef SAFETY
			if (pbDst >= pbLimDst)
				goto LFail;
#endif //SAFETY
			*pbDst++ = (byte)(luCur >> ibit + 1);
			ibit += 9;
			}
		else
			{
			// get the offset
			cb = 1;
			if (!_FTest(ibit + 1))
				{
				dib = ((luCur >> (ibit + 2)) & ((1 << kcbitKcdc0) - 1)) +
					kdibMinKcdc0;
				ibit += 2 + kcbitKcdc0;
				}
			else if (!_FTest(ibit + 2))
				{
				dib = ((luCur >> (ibit + 3)) & ((1 << kcbitKcdc1) - 1)) +
					kdibMinKcdc1;
				ibit += 3 + kcbitKcdc1;
				}
			else if (!_FTest(ibit + 3))
				{
				dib = ((luCur >> (ibit + 4)) & ((1 << kcbitKcdc2) - 1)) +
					kdibMinKcdc2;
				ibit += 4 + kcbitKcdc2;
				}
			else
				{
				dib = (luCur >> (ibit + 4)) & ((1 << kcbitKcdc3) - 1);
				if (dib == ((1 << kcbitKcdc3) - 1))
					break;
				dib += kdibMinKcdc3;
				ibit += 4 + kcbitKcdc3;
				cb++;
				}
			_Advance(ibit >> 3);
			ibit &= 0x07;

			// get the length
			for (cbit = 0; ; )
				{
				if (!_FTest(ibit + cbit))
					break;
				if (++cbit > kcbitMaxLenKcdc)
					goto LFail;
				}

			cb += (1 << cbit) + ((luCur >> (cbit + 1)) & ((1 << cbit) - 1));
			ibit += cbit + cbit + 1;

#ifdef SAFETY
			if (pbLimDst - pbDst < cb || pbDst - (byte *)pvDst < dib)
				goto LFail;
#endif //SAFETY
			for (pbT = pbDst - dib; cb-- > 0; )
				*pbDst++ = *pbT++;
			}
		_Advance(ibit >> 3);
		ibit &= 0x07;
		}

	*pcbDst = pbDst - (byte *)pvDst;
	return fTrue;

	#undef _FTest
	#undef _Advance

#endif //!IN_80386

LFail:
	Bug("bad compressed data");
	return fFalse;
}


/***************************************************************************
	Compress the data in pvSrc using the KCD2 encoding.  Returns false if
	the data can't be compressed. This is not optimized (ie, it's slow).
***************************************************************************/
bool KCDC::_FEncode2(void *pvSrc, long cbSrc, void *pvDst, long cbDst,
	long *pcbDst)
{
	AssertThis(0);
	AssertIn(cbSrc, 1, kcbMax);
	AssertPvCb(pvSrc, cbSrc);
	AssertIn(cbDst, 1, kcbMax);
	AssertPvCb(pvDst, cbDst);
	AssertVarMem(pcbDst);

	long ibSrc;
	long ibMatch, ibTest, cbMatch, ibMin, cbT;
	byte bMatchNew, bMatchLast;
	BITA bita;
	long cbRun;
	long *pmpsuibStart = pvNil;
	byte *prgbSrc = (byte *)pvSrc;
	long *pmpibibNext = pvNil;

	TrashVar(pcbDst);
	if (cbDst - kcbTailKcdc <= 1)
		return fFalse;

	// allocate the links
	if (!FAllocPv((void **)&pmpibibNext, LwMul(size(long), cbSrc),
			fmemNil, mprNormal) ||
		!FAllocPv((void **)&pmpsuibStart, LwMul(size(long), 0x10000),
			fmemNil, mprNormal))
		{
		Warn("failed to allocate memory for links");
		goto LFail;
		}

	// write the links
	// we need to set each entry of pmpsuibStart to a big negative number,
	// but not too big, or we risk overflow below.
	FillPb(pmpsuibStart, LwMul(size(long), 0x10000), 0xCC);
	for (ibSrc = 0; ibSrc < cbSrc - 1; ibSrc++)
		{
		ushort suCur = ((ushort)prgbSrc[ibSrc]) << 8 |
			(ushort)prgbSrc[ibSrc + 1];
		ibTest = pmpsuibStart[suCur];
		pmpsuibStart[suCur] = ibSrc;
		pmpibibNext[ibSrc] = ibTest;
		}
	pmpibibNext[cbSrc - 1] = 0xCCCCCCCCL;
	FreePpv((void **)&pmpsuibStart);

	// write flags byte
	bita.Set(pvDst, cbDst);
	AssertDo(bita.FWriteBits(0, 8), 0);

	cbRun = 0;
	for (ibSrc = 0; ; ibSrc += cbMatch)
		{
		byte *pbMatch;
		long cbMaxMatch;

		if (ibSrc >= cbSrc)
			{
			cbMatch = 0;
			goto LStore;
			}

		// get the new byte and the link
		ibTest = pmpibibNext[ibSrc];
		Assert(ibTest < ibSrc, 0);

		// assume we'll store a literal
		cbMatch = 1;
		ibMatch = ibSrc;

		if (ibTest <= (ibMin = ibSrc - kdibMinKcdc4))
			goto LStore;

		// we've seen this byte pair before - look for the longest match
		cbMaxMatch = LwMin(kcbMaxLenKcdc, cbSrc - ibSrc);
		pbMatch = prgbSrc + ibSrc;
		bMatchNew = prgbSrc[ibSrc + 1];
		bMatchLast = prgbSrc[ibSrc];
		for ( ; ibTest > ibMin; ibTest = pmpibibNext[ibTest])
			{
			Assert(prgbSrc[ibTest + 1] == prgbSrc[ibSrc + 1], "links are wrong");
			if (prgbSrc[ibTest + cbMatch] != bMatchNew ||
				prgbSrc[ibTest + cbMatch - 1] != bMatchLast ||
				cbMatch >= (cbT = CbEqualRgb(prgbSrc + ibTest,
					pbMatch, cbMaxMatch)))
				{
				Assert(pmpibibNext[ibTest] < ibTest, 0);
				continue;
				}

			AssertIn(cbT, cbMatch + 1, kcbMaxLenKcdc + 1);

			// if this run requires a 20 bit offset, we need to beat a
			// 9 or 6 bit offset by 2 bytes
			if (ibSrc - ibTest < kdibMinKcdc3 || cbT - cbMatch > 1 ||
					ibSrc - ibMatch >= kdibMinKcdc2)
				{
				cbMatch = cbT;
				ibMatch = ibTest;
				if (cbMatch == cbMaxMatch)
					break;
				bMatchNew = prgbSrc[ibSrc + cbMatch];
				bMatchLast = prgbSrc[ibSrc + cbMatch - 1];
				}

			Assert(pmpibibNext[ibTest] < ibTest, 0);
			}

		// write out the bits
LStore:
		if (cbMatch != 1 && cbRun > 0)
			{
			// write the previous literal run
			long ibit, ib;

			if (!bita.FWriteLogEncoded(cbRun))
				goto LFail;
			if (!bita.FWriteBits(0, 1))
				goto LFail;
			ibit = bita.Ibit();
			if (ibit > 0)
				{
				if (!bita.FWriteBits(prgbSrc[ibSrc - 1], 8 - ibit))
					goto LFail;
				}
			else
				ibit = 8;
			Assert(bita.Ibit() == 0, 0);

			for (ib = ibSrc - cbRun; ib < ibSrc - 1; ib++)
				{
				if (!bita.FWriteBits(prgbSrc[ib], 8))
					goto LFail;
				}
			if (!bita.FWriteBits(prgbSrc[ibSrc - 1] >> (8 - ibit), ibit))
				goto LFail;
			cbRun = 0;
			}

		if (cbMatch == 0)
			{
			// we're done
			break;
			}

		if (cbMatch == 1)
			cbRun++;
		else
			{
			AssertIn(ibMatch, 0, ibSrc);
			AssertIn(cbMatch, 2 + (ibSrc - ibMatch >= kdibMinKcdc3),
				kcbMaxLenKcdc + 1);

			// find the offset
			ulong luCode, luLen;
			long cbit;

			luCode = ibSrc - ibMatch;
			luLen = cbMatch - 1;
			if (luCode < kdibMinKcdc1)
				{
				// 6 bit offset
				cbit = 2 + kcbitKcdc0;
				luCode = ((luCode - kdibMinKcdc0) << 2) | 1;
				}
			else if (luCode < kdibMinKcdc2)
				{
				// 9 bit offset
				cbit = 3 + kcbitKcdc1;
				luCode = ((luCode - kdibMinKcdc1) << 3) | 0x03;
				}
			else if (luCode < kdibMinKcdc3)
				{
				// 12 bit offset
				cbit = 4 + kcbitKcdc2;
				luCode = ((luCode - kdibMinKcdc2) << 4) | 0x07;
				}
			else
				{
				// 20 bit offset
				Assert(luCode < kdibMinKcdc4, 0);
				cbit = 4 + kcbitKcdc3;
				luCode = ((luCode - kdibMinKcdc3) << 4) | 0x0F;
				luLen--;
				}
			AssertIn(cbit, 8, kcbitKcdc3 + 5);
			Assert(luLen > 0, "bad len");

			if (!bita.FWriteLogEncoded(luLen))
				goto LFail;

			if (!bita.FWriteBits(luCode, cbit))
				goto LFail;
			}
		}

	// fill the remainder of the last byte with 1's
	if (bita.Ibit() > 0)
		AssertDo(bita.FWriteBits(0xFF, 8 - bita.Ibit()), 0);

	// write the tail (kcbTailKcdc bytes of FF)
	for (cbT = 0; cbT < kcbTailKcdc; cbT += size(long))
		{
		if (!bita.FWriteBits(0xFFFFFFFF,
				LwMin(size(long), kcbTailKcdc - cbT) << 3))
			{
			goto LFail;
			}
		}

	*pcbDst = bita.Ib();

	FreePpv((void **)&pmpibibNext);
	return fTrue;

LFail:
	// can't compress the source
	FreePpv((void **)&pmpibibNext);
	return fFalse;
}


/***************************************************************************
	Decompress a compressed KCD2 stream.
***************************************************************************/
bool KCDC::_FDecode2(void *pvSrc, long cbSrc, void *pvDst, long cbDst,
	long *pcbDst)
{
	AssertThis(0);
	AssertIn(cbSrc, 1, kcbMax);
	AssertPvCb(pvSrc, cbSrc);
	AssertIn(cbDst, 1, kcbMax);
	AssertPvCb(pvDst, cbDst);
	AssertVarMem(pcbDst);

	long ib;
	byte bFlags;

	TrashVar(pcbDst);
	if (cbSrc <= kcbTailKcd2 + 1)
		{
		Bug("bad source stream");
		return fFalse;
		}

	// verify that the last kcbTailKcd2 bytes are FF.  This guarantees that
	// we won't run past the end of the source.
	for (ib = 0; ib++ < kcbTailKcd2; )
		{
		if (((byte *)pvSrc)[cbSrc - ib] != 0xFF)
			{
			Bug("bad tail of compressed data");
			return fFalse;
			}
		}

	bFlags = ((byte *)pvSrc)[0];
	if (bFlags != 0)
		{
		Bug("unknown flag byte");
		return fFalse;
		}

#ifdef IN_80386

	#include "kcd2_386.h"

	*pcbDst = cbTot;
	return fTrue;

#else //!IN_80386

	long cb, dib, ibit, cbit;
	register ulong luCur;
	byte bT;
	byte *pbT;
	register byte *pbDst = (byte *)pvDst;
	byte *pbLimDst = (byte *)pvDst + cbDst;
	register byte *pbSrc = (byte *)pvSrc + 1;
	byte *pbLimSrc = (byte *)pvSrc + cbSrc - kcbTailKcd2;

	#define _FTest(ibit) (luCur & (1L << (ibit)))
	#ifdef LITTLE_ENDIAN
		#define _Advance(cb) ((pbSrc += (cb)), (luCur = *(ulong *)(pbSrc - 4)))
	#else //!LITTLE_ENDIAN
		#define _Advance(cb) ((pbSrc += (cb)), \
			(luCur = LwFromBytes(pbSrc[-1], pbSrc[-2], pbSrc[-3], pbSrc[-4])))
	#endif //!LITTLE_ENDIAN

	_Advance(4);

	for (ibit = 0; ; )
		{
		// get the length
		for (cbit = 0; ; )
			{
			if (!_FTest(ibit + cbit))
				break;
			if (++cbit > kcbitMaxLenKcd2)
				goto LDone;
			}

		cb = (1 << cbit) + ((luCur >> (cbit + 1)) & ((1 << cbit) - 1));
		ibit += cbit + cbit + 1;
		_Advance(ibit >> 3);
		ibit &= 0x07;

		if (!_FTest(ibit))
			{
			// literal
#ifdef SAFETY
			if (pbDst + cb > pbLimDst)
				goto LFail;
			if (pbSrc - 3 + cb > pbLimSrc)
				goto LFail;
#endif //SAFETY
			ibit++;
			bT = (byte)~(luCur & ((1 << ibit) - 1));
			if (cb > 1)
				{
				CopyPb(pbSrc - 3, pbDst, cb - 1);
				pbDst += cb - 1;
				_Advance(cb);
				}
			else
				_Advance(1);
			bT |= luCur & ((1 << ibit) - 1);
			*pbDst++ = bT;
			}
		else
			{
			// get the offset
			cb++;
			if (!_FTest(ibit + 1))
				{
				dib = ((luCur >> (ibit + 2)) & ((1 << kcbitKcd2_0) - 1)) +
					kdibMinKcd2_0;
				ibit += 2 + kcbitKcd2_0;
				}
			else if (!_FTest(ibit + 2))
				{
				dib = ((luCur >> (ibit + 3)) & ((1 << kcbitKcd2_1) - 1)) +
					kdibMinKcd2_1;
				ibit += 3 + kcbitKcd2_1;
				}
			else if (!_FTest(ibit + 3))
				{
				dib = ((luCur >> (ibit + 4)) & ((1 << kcbitKcd2_2) - 1)) +
					kdibMinKcd2_2;
				ibit += 4 + kcbitKcd2_2;
				}
			else
				{
				dib = (luCur >> (ibit + 4)) & ((1 << kcbitKcd2_3) - 1) +
					kdibMinKcd2_3;
				ibit += 4 + kcbitKcd2_3;
				cb++;
				}

#ifdef SAFETY
			if (pbLimDst - pbDst < cb || pbDst - (byte *)pvDst < dib)
				goto LFail;
#endif //SAFETY
			for (pbT = pbDst - dib; cb-- > 0; )
				*pbDst++ = *pbT++;
			}

		_Advance(ibit >> 3);
		ibit &= 0x07;
		}

LDone:
	*pcbDst = pbDst - (byte *)pvDst;
	return fTrue;

	#undef _FTest
	#undef _Advance

#endif //!IN_80386

LFail:
	Bug("bad compressed data");
	return fFalse;
}



