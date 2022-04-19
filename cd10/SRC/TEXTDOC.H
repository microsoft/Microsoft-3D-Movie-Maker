/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	For editing a text file or text stream as a document.  Unlike the edit
	controls in text.h/text.cpp, all the text need not be in memory (this
	uses a BSF) and there can be multiple views on the same text.

***************************************************************************/
#ifndef TEXTDOC_H
#define TEXTDOC_H

/***************************************************************************
	Text document.  A doc wrapper for a BSF.
***************************************************************************/
typedef class TXDC *PTXDC;
#define TXDC_PAR DOCB
#define kclsTXDC 'TXDC'
class TXDC : public TXDC_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PBSF _pbsf;
	PFIL _pfil;

	TXDC(PDOCB pdocb = pvNil, ulong grfdoc = fdocNil);
	~TXDC(void);
	bool _FInit(PFNI pfni = pvNil, PBSF pbsf = pvNil);

public:
	static PTXDC PtxdcNew(PFNI pfni = pvNil, PBSF pbsf = pvNil, PDOCB pdocb = pvNil,
		ulong grfdoc = fdocNil);

	PBSF Pbsf(void)
		{ return _pbsf; }

	virtual PDDG PddgNew(PGCB pgcb);
	virtual bool FGetFni(FNI *pfni);
	virtual bool FSaveToFni(FNI *pfni, bool fSetFni);
	};


/***************************************************************************
	Text document display GOB - DDG for a TXDC.
***************************************************************************/
const long kcchMaxLine = 512;
const long kdxpIndentTxdd = 5;

typedef class TXDD *PTXDD;
#define TXDD_PAR DDG
#define kclsTXDD 'TXDD'
class TXDD : public TXDD_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PBSF _pbsf;
	long _clnDisp;
	long _clnDispWhole;
	PGL _pglichStarts;

	//the selection
	long _ichAnchor;
	long _ichOther;
	bool _fSelOn: 1;
	ulong _tsSel;
	long _xpSel;
	bool _fXpValid;

	//the font
	long _onn;
	ulong _grfont;
	long _dypFont;
	long _dypLine;
	long _dxpTab;

	//the cache
	achar _rgchCache[kcchMaxLine];
	long _ichMinCache;
	long _ichLimCache;

	TXDD(PDOCB pdocb, PGCB pgcb, PBSF pbsf, long onn, ulong grfont, long dypFont);
	~TXDD(void);
	virtual bool _FInit(void);
	virtual void _NewRc(void);
	virtual void _Activate(bool fActive);

	void _Reformat(long lnMin, long *pclnIns = pvNil, long *pclnDel = pvNil);
	void _ReformatEdit(long ichMinEdit, long cchIns, long cchDel,
		long *pln, long *pclnIns = pvNil, long *pclnDel = pvNil);
	bool _FFetchCh(long ich, achar *pch);
	void _FetchLineLn(long ln, achar *prgch, long cchMax, long *pcch,
		long *pichMin = pvNil);
	void _FetchLineIch(long ich, achar *prgch, long cchMax, long *pcch,
		long *pichMin = pvNil);
	bool _FFindNextLineStart(long ich, long *pich,
		achar *prgch = pvNil, long cchMax = 0);
	bool _FFindLineStart(long ich, long *pich);
	bool _FFindNextLineStartCached(long ich, long *pich,
		achar *prgch = pvNil, long cchMax = 0);
	bool _FFindLineStartCached(long ich, long *pich);
	void _DrawLine(PGNV pgnv, RC *prcClip, long yp, achar *prgch, long cch);
	void _SwitchSel(bool fOn, bool fDraw);
	void _InvertSel(PGNV pgnv, bool fDraw);
	void _InvertIchRange(PGNV pgnv, long ich1, long ich2, bool fDraw);
	long _LnFromIch(long ich);
	long _IchMinLn(long ln);
	long _XpFromLnIch(PGNV pgnv, long ln, long ich);
	long _XpFromIch(long ich);
	long _XpFromRgch(PGNV pgnv, achar *prgch, long cch);
	long _IchFromLnXp(long ln, long xp);
	long _IchFromIchXp(long ich, long xp);
	long _IchFromRgchXp(achar *prgch, long cch, long ichMinLine, long xp);

	long *_QichLn(long ln)
		{ return (long *)_pglichStarts->QvGet(ln); }

	void _InvalAllTxdd(long ich, long cchIns, long cchDel);
	void _InvalIch(long ich, long cchIns, long cchDel);

	//scrolling support
	virtual long _ScvMax(bool fVert);
	virtual void _Scroll(long scaHorz, long scaVert,
		long scvHorz = 0, long scvVert = 0);

	//clipboard support
	virtual bool _FCopySel(PDOCB *ppdocb = pvNil);
	virtual void _ClearSel(void);
	virtual bool _FPaste(PCLIP pclip, bool fDoIt, long cid);

public:
	static PTXDD PtxddNew(PDOCB pdocb, PGCB pgcb, PBSF pbsf,
		long onn, ulong grfont, long dypFont);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FCmdTrackMouse(PCMD_MOUSE pcmd);
	virtual bool FCmdKey(PCMD_KEY pcmd);
	virtual bool FCmdSelIdle(PCMD pcmd);

	void SetSel(long ichAnchor, long ichOther, bool fDraw);
	void ShowSel(bool fDraw);
	bool FReplace(achar *prgch, long cch, long ich1, long ich2, bool fDraw);
	};


#endif //!TEXTDOC_H

