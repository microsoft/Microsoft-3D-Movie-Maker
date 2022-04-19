/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Codec manager class.

***************************************************************************/
#include "util.h"
ASSERTNAME


RTCLASS(CODM)
RTCLASS(CODC)


/***************************************************************************
	The header on a compressed block consists of the cfmt (a long in big
	endian order) and the decompressed length (a long in big endian order).
***************************************************************************/
const long kcbCodecHeader = 2 * size(long);


/***************************************************************************
	Constructor for the compression manager. pcodc is an optional default
	codec. cfmt is the default compression format.
***************************************************************************/
CODM::CODM(PCODC pcodc, long cfmt)
{
	AssertNilOrPo(pcodc, 0);
	Assert(cfmtNil != cfmt, "nil default compression format");

	_cfmtDef = cfmt;
	_pcodcDef = pcodc;
	if (pvNil != _pcodcDef)
		_pcodcDef->AddRef();
	_pglpcodc = pvNil;

	AssertThis(0);
}


/***************************************************************************
	Destructor for the compression manager.
***************************************************************************/
CODM::~CODM(void)
{
	AssertThis(0);

	ReleasePpo(&_pcodcDef);
	if (pvNil != _pglpcodc)
		{
		long ipcodc;
		PCODC pcodc;

		for (ipcodc = _pglpcodc->IvMac(); ipcodc-- > 0; )
			{
			_pglpcodc->Get(ipcodc, &pcodc);
			ReleasePpo(&pcodc);
			}
		ReleasePpo(&_pglpcodc);
		}
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a CODM.
***************************************************************************/
void CODM::AssertValid(ulong grf)
{
	CODM_PAR::AssertValid(0);
	Assert(cfmtNil != _cfmtDef, "nil default compression");
	AssertNilOrPo(_pcodcDef, 0);
	AssertNilOrPo(_pglpcodc, 0);
}


/***************************************************************************
	Mark memory for the CODM.
***************************************************************************/
void CODM::MarkMem(void)
{
	AssertValid(0);
	CODM_PAR::MarkMem();

	MarkMemObj(_pcodcDef);
	if (pvNil != _pglpcodc)
		{
		long ipcodc;
		PCODC pcodc;

		for (ipcodc = _pglpcodc->IvMac(); ipcodc-- > 0; )
			{
			_pglpcodc->Get(ipcodc, &pcodc);
			MarkMemObj(pcodc);
			}
		MarkMemObj(_pglpcodc);
		}
}
#endif //DEBUG


/***************************************************************************
	Set the default compression type.
***************************************************************************/
void CODM::SetCfmtDefault(long cfmt)
{
	AssertThis(0);

	if (cfmt == cfmtNil)
		{
		Bug("can't set default compression to nil");
		return;
		}

	_cfmtDef = cfmt;
}


/***************************************************************************
	Add a codec to the compression manager.
***************************************************************************/
bool CODM::FRegisterCodec(PCODC pcodc)
{
	AssertThis(0);
	AssertPo(pcodc, 0);

	if (pvNil == _pglpcodc && pvNil == (_pglpcodc = GL::PglNew(size(PCODC))))
		return fFalse;

	if (!_pglpcodc->FAdd(&pcodc))
		return fFalse;

	pcodc->AddRef();
	return fTrue;
}


/***************************************************************************
	Return whether we can encode or decode the given format.
***************************************************************************/
bool CODM::FCanDo(long cfmt, bool fEncode)
{
	AssertThis(0);
	PCODC pcodc;

	if (cfmtNil == cfmt)
		return fFalse;

	return _FFindCodec(fEncode, cfmt, &pcodc);
}


/***************************************************************************
	Gets the type of compression used on the block (assuming it is
	compressed).
***************************************************************************/
bool CODM::FGetCfmtFromBlck(PBLCK pblck, long *pcfmt)
{
	AssertThis(0);
	AssertPo(pblck, 0);
	AssertVarMem(pcfmt);

	byte rgb[4];

	TrashVar(pcfmt);
	if (pblck->Cb(fTrue) < 2 * size(rgb))
		return fFalse;

	if (!pblck->FReadRgb(rgb, size(rgb), 0, fTrue))
		return fFalse;

	*pcfmt = LwFromBytes(rgb[0], rgb[1], rgb[2], rgb[3]);
	return fTrue;
}


/***************************************************************************
	Look for a codec that can handle the given format.
***************************************************************************/
bool CODM::_FFindCodec(bool fEncode, long cfmt, PCODC *ppcodc)
{
	AssertThis(0);
	AssertVarMem(ppcodc);
	Assert(cfmtNil != cfmt, "nil cfmt");

	if (pvNil != _pcodcDef && _pcodcDef->FCanDo(fEncode, cfmt))
		{
		*ppcodc = _pcodcDef;
		return fTrue;
		}

	if (pvNil != _pglpcodc)
		{
		long ipcodc;
		PCODC pcodc;

		for (ipcodc = _pglpcodc->IvMac(); ipcodc-- > 0; )
			{
			_pglpcodc->Get(ipcodc, &pcodc);
			if (pcodc->FCanDo(fEncode, cfmt))
				{
				*ppcodc = pcodc;
				return fTrue;
				}
			}
		}

	return fFalse;
}


/***************************************************************************
	Compress or decompress an hq of data. Note that the value of *phq
	may change. cfmt should be cfmtNil to decompress.
***************************************************************************/
bool CODM::_FCodePhq(long cfmt, HQ *phq)
{
	AssertThis(0);
	AssertVarMem(phq);
	AssertHq(*phq);

	HQ hqDst;
	void *pvSrc;
	long cbSrc;
	long cbDst;
	bool fRet;

	pvSrc = PvLockHq(*phq);
	cbSrc = CbOfHq(*phq);
	fRet = _FCode(cfmt, pvSrc, cbSrc, pvNil, 0, &cbDst);
	UnlockHq(*phq);
	if (!fRet)
		return fFalse;

	if (!FAllocHq(&hqDst, cbDst, fmemNil, mprNormal))
		return fFalse;

	pvSrc = PvLockHq(*phq);
	fRet = _FCode(cfmt, pvSrc, cbSrc, PvLockHq(hqDst), cbDst, &cbDst);
	UnlockHq(hqDst);
	UnlockHq(*phq);
	if (!fRet)
		{
		FreePhq(&hqDst);
		return fFalse;
		}
	Assert(cbDst <= CbOfHq(hqDst), "why is the final size larger?");
	if (cbDst < CbOfHq(hqDst))
		AssertDo(FResizePhq(&hqDst, cbDst, fmemNil, mprNormal), 0);

	FreePhq(phq);
	*phq = hqDst;
	return fTrue;
}


/***************************************************************************
	Compress or decompress a block of data. If pvDst is nil, just fill
	*pcbDst with the required destination buffer size. This is just an
	estimate in the compress case.
***************************************************************************/
bool CODM::_FCode(long cfmt, void *pvSrc, long cbSrc,
	void *pvDst, long cbDst, long *pcbDst)
{
	AssertIn(cbSrc, 1, kcbMax);
	AssertPvCb(pvSrc, cbSrc);
	AssertIn(cbDst, 0, kcbMax);
	AssertPvCb(pvDst, cbDst);
	AssertVarMem(pcbDst);

	byte *prgb;
	PCODC pcodc;

	if (cfmtNil != cfmt)
		{
		// Encode the data
		if (pvNil == pvDst || cbDst >= cbSrc)
			cbDst = cbSrc - 1;

		if (cbDst <= kcbCodecHeader)
			{
			// destination is smaller than the minimum compressed size, so
			// no sense trying.
			return fFalse;
			}

		// make sure we have a codec for this format
		if (!_FFindCodec(fTrue, cfmt, &pcodc))
			return fFalse;

		if (pvNil == pvDst)
			{
			// this is our best guess at the compressed size
			*pcbDst = cbDst;
			return fTrue;
			}

		prgb = (byte *)pvDst;
		if (!pcodc->FConvert(fTrue, cfmt, pvSrc, cbSrc, prgb + kcbCodecHeader,
				cbDst - kcbCodecHeader, pcbDst))
			{
			return fFalse;
			}

		AssertIn(*pcbDst, 1, cbDst - kcbCodecHeader + 1);
		*pcbDst += kcbCodecHeader;

		prgb[0] = B3Lw(cfmt);
		prgb[1] = B2Lw(cfmt);
		prgb[2] = B1Lw(cfmt);
		prgb[3] = B0Lw(cfmt);
		prgb[4] = B3Lw(cbSrc);
		prgb[5] = B2Lw(cbSrc);
		prgb[6] = B1Lw(cbSrc);
		prgb[7] = B0Lw(cbSrc);
		}
	else
		{
		// decode
		if (0 >= (cbSrc -= kcbCodecHeader))
			return fFalse;

		// get the format and decompressed size
		prgb = (byte *)pvSrc;
		cfmt = LwFromBytes(prgb[0], prgb[1], prgb[2], prgb[3]);
		*pcbDst = LwFromBytes(prgb[4], prgb[5], prgb[6], prgb[7]);
		if (!FIn(*pcbDst, 1, pvNil == pvDst ? kcbMax : cbDst + 1))
			return fFalse;

		// make sure we have a codec for this format
		if (!_FFindCodec(fFalse, cfmt, &pcodc))
			return fFalse;

		if (pvNil == pvDst)
			return fTrue;

		cbDst = *pcbDst;
		if (!pcodc->FConvert(fFalse, cfmt,
				prgb + kcbCodecHeader, cbSrc, pvDst, cbDst, pcbDst))
			{
			return fFalse;
			}

		if (cbDst != *pcbDst)
			{
			Bug("decompressed to wrong size");
			return fFalse;
			}
		}

	return fTrue;
}


