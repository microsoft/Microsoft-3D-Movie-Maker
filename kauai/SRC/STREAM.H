/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Stream classes.

	A BSM is a byte stream in memory.  The data is stored contiguously, so
	should be used only for relatively small streams.

	A BSF is a byte stream with pieces stored in files and other pieces
	stored in memory.

***************************************************************************/
#ifndef STREAM_H
#define STREAM_H


/***************************************************************************
	Byte stream in memory.  The entire stream is in contiguous memory.
***************************************************************************/
typedef class BSM *PBSM;
#define BSM_PAR BASE
#define kclsBSM 'BSM'
class BSM : public BSM_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(BSM)

protected:
	HQ _hqrgb;
	long _ibMac;
	long _cbMinGrow;

	bool _FEnsureSize(long cbMin, bool fShrink);

public:
	BSM(void);
	~BSM(void);

	void SetMinGrow(long cb);
	bool FEnsureSpace(long cb, bool fShrink);
	void *PvLock(long ib);
	void Unlock(void);

	long IbMac(void)
		{ return _ibMac; }
	void FetchRgb(long ib, long cb, void *prgb);
	bool FReplace(void *prgb, long cbIns, long ib, long cbDel);
	bool FWriteRgb(PFLO pflo, long ib = 0);
	bool FWriteRgb(PBLCK pblck, long ib = 0);
	};


/***************************************************************************
	Byte stream on file.  Parts of the stream may be in files.
***************************************************************************/
typedef class BSF *PBSF;
#define BSF_PAR BASE
#define kclsBSF 'BSF'
class BSF : public BSF_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(BSF)

protected:
	PGG _pggflo;
	long _ibMac;

	long _IfloFind(long ib, long *pib, long *pcb = pvNil);
	bool _FEnsureSplit(long ib, long *piflo = pvNil);
	void _AttemptMerge(long ibMin, long ibLim);
	bool _FReplaceCore(void *prgb, long cbIns, PFLO pflo, long ib, long cbDel);

public:
	BSF(void);
	~BSF(void);

	long IbMac(void)
		{ return _ibMac; }
	void FetchRgb(long ib, long cb, void *prgb);
	bool FReplace(void *prgb, long cbIns, long ib, long cbDel);
	bool FReplaceFlo(PFLO pflo, bool fCopy, long ib, long cbDel);
	bool FReplaceBsf(PBSF pbsfSrc, long ibSrc, long cbSrc, long ibDst, long cbDel);
	bool FWriteRgb(PFLO pflo, long ib = 0);
	bool FWriteRgb(PBLCK pblck, long ib = 0);
	bool FCompact(void);
	};

#endif //!STREAM_H

