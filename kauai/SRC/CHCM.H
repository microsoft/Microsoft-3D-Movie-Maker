/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Header file for the CHCM class - the chunky compiler class, and
	CHLX - its lexer.

***************************************************************************/
#ifndef CHCM_H
#define CHCM_H

// token types
enum
	{
	ttChunk = ttLimBase,	// chunk definition
	ttChild,				// child descriptor
	ttParent,				// parent descriptor
	ttAlign,				// align command
	ttFile,					// file import command
	ttMeta,					// metafile import command
	ttBitmap,				// bitmap import command
	ttFree,					// for AL, AG, AST - item is free
	ttItem,					// for GL, etc - start of data for new item
	ttVar,					// for GG and AG - variable sized data
	ttGl,					// GL command
	ttAl,					// AL command
	ttGg,					// GG command
	ttAg,					// AG command
	ttGst,					// GST command
	ttAst,					// AST command
	ttScript,				// infix script
	ttScriptP,				// postfix script
	ttModeStn,				// change mode to store strings as stn's
	ttModeStz,				// change mode to store strings as stz's
	ttModeSz,				// change mode to store strings as sz's
	ttModeSt,				// change mode to store strings as st's
	ttModeByte,				// change mode to accept a byte
	ttModeShort,			// change mode to accept a short
	ttModeLong,				// change mode to accept a long
	ttEndChunk,				// end of chunk
	ttAdopt,				// adopt command
	ttMacBo,				// use Mac byte order and OSK
	ttWinBo,				// use Win byte order and OSK
	ttMacOsk,				// use Mac byte order and OSK
	ttWinOsk,				// use Win byte order and OSK
	ttBo,					// insert the current byte order
	ttOsk,					// insert the current OSK
	ttMask,					// MASK command
	ttLoner,				// LONER command
	ttCursor,				// CURSOR command
	ttPalette,				// PALETTE command
	ttPrePacked,			// mark the change as packed
	ttPack,					// pack the data
	ttPackedFile,			// packed file import command
	ttMidi,					// midi file import command
	ttPackFmt,				// format to pack in
	ttSubFile,				// start an embedded chunk forest

	ttLimChlx
	};

// lookup table for keywords
struct KEYTT
	{
	PSZ pszKeyword;
	long tt;
	};


/***************************************************************************
	Chunky Compiler lexer class.
***************************************************************************/
typedef class CHLX *PCHLX;
#define CHLX_PAR LEXB
#define kclsCHLX 'CHLX'
class CHLX : public CHLX_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(CHLX)

protected:
	PGST _pgstVariables;

	bool _FDoSet(PTOK ptok);

public:
	CHLX(PBSF pbsf, PSTN pstnFile);
	~CHLX(void);

	// override the LEXB FGetTok to resolve variables, hande SET
	// and recognize our additional key words
	virtual bool FGetTok(PTOK ptok);
	virtual bool FGetTokSkipSemi(PTOK ptok);	// also skip ';' & ','
	virtual bool FGetPath(FNI *pfni);			// read a path
	};


// error types
enum
	{
	ertNil = 0,
	ertOom,
	ertOpenFile,
	ertReadMeta,
	ertRangeByte,
	ertRangeShort,
	ertBufData,
	ertParenOpen,
	ertEof,
	ertNeedString,
	ertNeedNumber,
	ertBadToken,
	ertParenClose,
	ertChunkHead,
	ertDupChunk,
	ertBodyChildHead,
	ertChildMissing,
	ertCycle,
	ertBodyParentHead,
	ertParentMissing,
	ertBodyAlignRange,
	ertBodyFile,
	ertNeedEndChunk,
	ertListHead,
	ertListEntrySize,
	ertVarUndefined,
	ertItemOverflow,
	ertBadFree,
	ertSyntax,
	ertGroupHead,
	ertGroupEntrySize,
	ertGstHead,
	ertGstEntrySize,
	ertScript,
	ertAdoptHead,
	ertNeedChunk,
	ertBodyBitmapHead,
	ertReadBitmap,
	ertBadScript,
	ertReadCursor,
	ertPackedFile,
	ertReadMidi,
	ertBadPackFmt,
	ertLonerInSub,
	ertNoEndSubFile,
	ertLim
	};

// string modes
enum
	{
	smStn,
	smStz,
	smSz,
	smSt
	};

#define kcbMinAlign 2
#define kcbMaxAlign 1024

/***************************************************************************
	Base chunky compiler class
***************************************************************************/
typedef class CHCM *PCHCM;
#define CHCM_PAR BASE
#define kclsCHCM 'CHCM'
class CHCM : public CHCM_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(CHCM)

protected:
	// Chunk sub file context
	struct CSFC
		{
		PCFL pcfl;
		CTG ctg;
		CNO cno;
		bool fPack;
		};

	PGL _pglcsfc;		// the stack of CSFCs for sub files

	PCFL _pcfl;			// current sub file
	PGL _pglckiLoner;	// the chunks that must be loners

	BSF _bsf;			// temporary buffer for the chunk data
	PCHLX _pchlx;		// lexer for compiling
	long _sm;			// current string mode
	long _cbNum;		// current numerical size (1, 2, or 4)
	short _bo;			// current byte order and osk
	short _osk;
	PMSNK _pmsnkError;	// error message sink
	long _cactError;	// how many errors we've encountered

protected:
	struct PHP // parenthesized header parameter
		{
		long lw;
		PSTN pstn;
		};

	void _Error(long ert, PSZ pszMessage = pvNil);
	void _GetRgbFromLw(long lw, byte *prgb);
	void _ErrorOnData(PSZ pszPreceed);
	bool _FParseParenHeader(PHP *prgphp, long cphpMax, long *pcphp);
	bool _FGetCleanTok(TOK *ptok, bool fEofOk = fFalse);
	void _SkipPastTok(long tt);
	void _ParseChunkHeader(CTG *pctg, CNO *pcno);
	void _AppendString(PSTN pstnValue);
	void _AppendNumber(long lwValue);
	void _ParseBodyChild(CTG ctg, CNO cno);
	void _ParseBodyParent(CTG ctg, CNO cno);
	void _ParseBodyAlign(void);
	void _ParseBodyFile(void);

	void _StartSubFile(bool fPack, CTG ctg, CNO cno);
	void _EndSubFile(void);

	void _ParseBodyMeta(bool fPack, CTG ctg, CNO cno);
	void _ParseBodyBitmap(bool fPack, bool fMask, CTG ctg, CNO cno);
	void _ParseBodyPalette(bool fPack, CTG ctg, CNO cno);
	void _ParseBodyMidi(bool fPack, CTG ctg, CNO cno);
	void _ParseBodyCursor(bool fPack, CTG ctg, CNO cno);
	bool _FParseData(PTOK ptok);
	void _ParseBodyList(bool fPack, bool fAl, CTG ctg, CNO cno);
	void _ParseBodyGroup(bool fPack, bool fAg, CTG ctg, CNO cno);
	void _ParseBodyStringTable(bool fPack, bool fAst, CTG ctg, CNO cno);
	void _ParseBodyScript(bool fPack, bool fInfix, CTG ctg, CNO cno);
	void _ParseBodyPackedFile(bool *pfPacked);
	void _ParseChunkBody(CTG ctg, CNO cno);
	void _ParseAdopt(void);
	void _ParsePackFmt(void);

	bool _FPrepWrite(bool fPack, long cb, CTG ctg, CNO cno, PBLCK pblck);
	bool _FEndWrite(bool fPack, CTG ctg, CNO cno, PBLCK pblck);

public:
	CHCM(void);
	~CHCM(void);

	bool FError(void)
		{ return _cactError > 0; }

	PCFL PcflCompile(PFNI pfniSrc, PFNI pfniDst, PMSNK pmsnk);
	PCFL PcflCompile(PBSF pbsfSrc, PSTN pstnFile, PFNI pfniDst, PMSNK pmsnk);
	};


/***************************************************************************
	Chunky decompiler class.
***************************************************************************/
typedef class CHDC *PCHDC;
#define CHDC_PAR BASE
#define kclsCHDC 'CHDC'
class CHDC : public CHDC_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(CHDC)

protected:
	long _ert;		// error type
	PCFL _pcfl;		// the chunky file to read from
	BSF _bsf;		// temporary buffer for the chunk data
	short _bo;		// current byte order and osk
	short _osk;
	CHSE _chse;		// chunky source emitter

protected:
	bool _FDumpScript(CKI *pcki);
	bool _FDumpList(PBLCK pblck, bool fAl);
	bool _FDumpGroup(PBLCK pblck, bool fAg);
	bool _FDumpStringTable(PBLCK pblck, bool fAst);
	void _WritePack(long cfmt);

public:
	CHDC(void);
	~CHDC(void);

	bool FError(void)
		{ return ertNil != _ert; }

	bool FDecompile(PCFL pcflSrc, PMSNK pmsnk, PMSNK pmsnkError);
	};

#endif // CHCM_H

