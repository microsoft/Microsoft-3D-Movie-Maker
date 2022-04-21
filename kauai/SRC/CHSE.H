/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Header file for the CHSE class - the chunky source emitter class.

***************************************************************************/
#ifndef CHSE_H
#define CHSE_H

/***************************************************************************
	Chunky source emitter class
***************************************************************************/
#ifdef DEBUG
enum
	{
	fchseNil = 0,
	fchseDump = 0x8000,
	};
#endif // DEBUG

typedef class CHSE *PCHSE;
#define CHSE_PAR BASE
#define kclsCHSE 'CHSE'
class CHSE : public CHSE_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(CHSE)

protected:
	PMSNK _pmsnkDump;
	PMSNK _pmsnkError;
	BSF _bsf;
	bool _fError;

protected:
	void _DumpBsf(long cactTab);

public:
	CHSE(void);
	~CHSE(void);
	void Init(PMSNK pmsnkDump, PMSNK pmsnkError = pvNil);
	void Uninit(void);

	void DumpHeader(CTG ctg, CNO cno, PSTN pstnName = pvNil, bool fPack = fFalse);
	void DumpRgb(void *prgb, long cb, long cactTab = 1);
	void DumpParentCmd(CTG ctg, CNO cno, CHID chid);
	void DumpBitmapCmd(byte bTransparent, long dxp, long dyp, PSTN pstnFile);
	void DumpFileCmd(PSTN pstnFile, bool fPacked = fFalse);
	void DumpAdoptCmd(CKI *pcki, KID *pkid);
	void DumpList(PGLB pglb);
	void DumpGroup(PGGB pggb);
	bool FDumpStringTable(PGSTB pgstb);
	void DumpBlck(PBLCK pblck);
	bool FDumpScript(PSCPT pscpt, PSCCB psccb);

	// General sz emitting routines
	void DumpSz(PSZ psz)
		{ AssertThis(fchseDump); _pmsnkDump->ReportLine(psz); }
	void Error(PSZ psz)
		{
		AssertThis(fchseNil);
		_fError = fTrue;
		if (pvNil != _pmsnkError)
			_pmsnkError->ReportLine(psz);
		}
	bool FError(void)
		{ return _fError || pvNil != _pmsnkDump && _pmsnkDump->FError(); }
	};

#endif // !CHSE_H

