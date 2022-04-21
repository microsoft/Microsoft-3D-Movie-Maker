/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Compression/decompression manager and default Kauai codec.

***************************************************************************/
#ifndef CODEC_H
#define CODEC_H


/***************************************************************************
	Codec object.
***************************************************************************/
typedef class CODC *PCODC;
#define CODC_PAR BASE
#define kclsCODC 'CODC'
class CODC : public CODC_PAR
	{
	RTCLASS_DEC

public:
	// return whether this codec can handle the given format.
	virtual bool FCanDo(bool fEncode, long cfmt) = 0;

	// Decompression should be extremely fast. Compression may be
	// (painfully) slow.
	virtual bool FConvert(bool fEncode, long cfmt, void *pvSrc, long cbSrc,
		void *pvDst, long cbDst, long *pcbDst) = 0;
	};


/***************************************************************************
	Codec manager.
***************************************************************************/
typedef class CODM *PCODM;
#define CODM_PAR BASE
#define kclsCODM 'CODM'
class CODM : public CODM_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	long _cfmtDef;
	PCODC _pcodcDef;
	PGL _pglpcodc;

	virtual bool _FFindCodec(bool fEncode, long cfmt, PCODC *ppcodc);
	virtual bool _FCodePhq(long cfmt, HQ *phq);
	virtual bool _FCode(long cfmt, void *pvSrc, long cbSrc,
		void *pvDst, long cbDst, long *pcbDst);

public:
	CODM(PCODC pcodc, long cfmt);
	~CODM(void);

	long CfmtDefault(void)
		{ return _cfmtDef; }
	void SetCfmtDefault(long cfmt);
	virtual bool FRegisterCodec(PCODC pcodc);
	virtual bool FCanDo(long cfmt, bool fEncode);

	// Gets the type of compression used on the block (assuming it is
	// compressed).
	virtual bool FGetCfmtFromBlck(PBLCK pblck, long *pcfmt);

	// FDecompress allows pvDst to be nil (in which case *pcbDst is filled
	// in with the buffer size required).
	// FCompress also allows pvDst to be nil, but the value returned in
	// *pcbDst will just be cbSrc - 1.
	bool FDecompressPhq(HQ *phq)
		{ return _FCodePhq(cfmtNil, phq); }
	bool FCompressPhq(HQ *phq, long cfmt = cfmtNil)
		{ return _FCodePhq(cfmtNil == cfmt ? _cfmtDef : cfmt, phq); }

	bool FDecompress(void *pvSrc, long cbSrc,
			void *pvDst, long cbDst, long *pcbDst)
		{
		return _FCode(cfmtNil, pvSrc, cbSrc, pvDst, cbDst, pcbDst);
		}
	bool FCompress(void *pvSrc, long cbSrc,
			void *pvDst, long cbDst, long *pcbDst, long cfmt = cfmtNil)
		{
		return _FCode(cfmtNil == cfmt ? _cfmtDef : cfmt,
			pvSrc, cbSrc, pvDst, cbDst, pcbDst);
		}
	};


/***************************************************************************
	The standard Kauai Codec object.
***************************************************************************/
typedef class KCDC *PKCDC;
#define KCDC_PAR CODC
#define kclsKCDC 'KCDC'
class KCDC : public KCDC_PAR
	{
	RTCLASS_DEC

protected:
	bool _FEncode(void *pvSrc, long cbSrc, void *pvDst, long cbDst,
		long *pcbDst);
	bool _FDecode(void *pvSrc, long cbSrc, void *pvDst, long cbDst,
		long *pcbDst);
	bool _FEncode2(void *pvSrc, long cbSrc, void *pvDst, long cbDst,
		long *pcbDst);
	bool _FDecode2(void *pvSrc, long cbSrc, void *pvDst, long cbDst,
		long *pcbDst);

public:
	virtual bool FCanDo(bool fEncode, long cfmt)
		{ return kcfmtKauai2 == cfmt || kcfmtKauai == cfmt; }
	virtual bool FConvert(bool fEncode, long cfmt, void *pvSrc, long cbSrc,
		void *pvDst, long cbDst, long *pcbDst);
	};


#endif //!CODEC_H

