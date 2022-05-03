/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Header file for edit controls and rich text editing.

***************************************************************************/
#ifndef TEXT_H
#define TEXT_H


//edit control parameters
typedef class EDPAR *PEDPAR;
class EDPAR
	{
public:
	GCB _gcb;
	long _onn;
	ulong _grfont;
	long _dypFont;
	long _tah;
	long _tav;
	ACR _acrFore;
	ACR _acrBack;
	long _cmhl;

	EDPAR(void) {}
	EDPAR(long hid, PGOB pgob, ulong grfgob, long gin, RC *prcAbs, RC *prcRel,
		long onn, ulong grfont, long dypFont, long tah = tahLeft,
		long tav = tavTop, ACR acrFore = kacrBlack, ACR acrBack = kacrWhite,
		long cmhl = 0);

	void Set(long hid, PGOB pgob, ulong grfgob, long gin, RC *prcAbs, RC *prcRel,
		long onn, ulong grfont, long dypFont, long tah = tahLeft,
		long tav = tavTop, ACR acrFore = kacrBlack, ACR acrBack = kacrWhite,
		long cmhl = 0);
	void SetFont(long onn, ulong grfont, long dypFont, long tah = tahLeft,
		long tav = tavTop, ACR acrFore = kacrBlack, ACR acrBack = kacrWhite);
	};


/***************************************************************************
	Edit control base class.
***************************************************************************/
typedef class EDCB *PEDCB;
#define EDCB_PAR GOB
#define kclsEDCB 'EDCB'
class EDCB : public EDCB_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	//handler level
	long _cmhl;

	//the selection
	PGNV _pgnv;
	long _ichAnchor;
	long _ichOther;
	bool _fSelOn: 1;
	bool _fXpValid: 1;
	bool _fSelByWord: 1;
	bool _fMark: 1;
	bool _fClear: 1;

	ulong _tsSel;
	long _xpSel;	//for avoiding migration when changing selection by lines

	//the origin
	long _xp;
	long _yp;

	EDCB(PGCB pgcb, long cmhl);

	virtual bool _FInit(void);

	// pure virtual functions
	virtual long _LnFromIch(long ich) = 0;
	virtual long _IchMinLn(long ln) = 0;
	virtual long _XpFromIch(long ich) = 0;
	virtual long _YpFromIch(long ich);
	virtual long _YpFromLn(long ln) = 0;
	virtual long _LnFromYp(long yp) = 0;
	virtual long _IchFromLnXp(long ln, long xp, bool fClosest = fTrue) = 0;
	virtual long _LnMac(void) = 0;
	virtual void _DrawLine(PGNV pgnv, long ln) = 0;
	virtual void _HiliteRc(PGNV pgnv, RC *prc) = 0;
	virtual bool _FFilterCh(achar ch) = 0;

	void _SwitchSel(bool fOn, long gin = kginDraw);
	void _InvertSel(PGNV pgnv, long gin = kginDraw);
	void _InvertIchRange(PGNV pgnv, long ich1, long ich2, long gin = kginDraw);
	void _Scroll(long dxp, long dyp, long gin = kginDraw);
	void _UpdateLn(long ln, long clnIns, long dlnDel, long dypDel,
		long gin = kginDraw);
	long _IchPrev(long ich, bool fWord = fFalse);
	long _IchNext(long ich, bool fWord = fFalse);
	achar _ChFetch(long ich);

	virtual void _NewRc(void);

	virtual void _GetRcContent(RC *prc);
	virtual void _InitGnv(PGNV pgnv);

public:
	~EDCB(void);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FCmdTrackMouse(PCMD_MOUSE pcmd);
	virtual bool FCmdKey(PCMD_KEY pcmd);
	virtual bool FCmdSelIdle(PCMD pcmd);
	virtual bool FCmdActivateSel(PCMD pcmd);
	virtual void Activate(bool fActive);

	long IchAnchor(void)
		{ return _ichAnchor; }
	long IchOther(void)
		{ return _ichOther; }
	void SetSel(long ichAnchor, long ichOther, long gin = kginDraw);
	void ShowSel(bool fForceJustification = fTrue, long gin = kginDraw);

	virtual long IchMac(void) = 0;
	virtual bool FReplace(achar *prgch, long cchIns,
		long ich1, long ich2, long gin = kginDraw) = 0;
	virtual long CchFetch(achar *prgch, long ich, long cchWant) = 0;
	};


/***************************************************************************
	Plain edit control - virtual class supporting single line and multi
	line edit controls with a single font.
***************************************************************************/
typedef class EDPL *PEDPL;
#define EDPL_PAR EDCB
#define kclsEDPL 'EDPL'
class EDPL : public EDPL_PAR
	{
	RTCLASS_DEC
	ASSERT

protected:
	//drawing parameters
	long _onn;
	ulong _grfont;
	long _dypFont;
	long _tah;
	long _tav;
	ACR _acrFore;
	ACR _acrBack;
	long _dypLine;

	EDPL(PEDPAR pedpar);

	//methods of EDCB
	virtual bool _FInit(void);
	virtual long _XpFromIch(long ich);
	virtual long _YpFromLn(long ln);
	virtual long _LnFromYp(long yp);
	virtual long _IchFromLnXp(long ln, long xp, bool fClosest = fTrue);
	virtual void _DrawLine(PGNV pgnv, long ln);
	virtual void _HiliteRc(PGNV pgnv, RC *prc);

	long _XpOrigin(void);
	virtual bool _FLockLn(long ln, achar **pprgch, long *pcch) = 0;
	virtual void _UnlockLn(long ln, achar *prgch) = 0;
	};


/***************************************************************************
	Single line edit control.
***************************************************************************/
const long kcchMaxEdsl = kcchMaxStn;

typedef class EDSL *PEDSL;
#define EDSL_PAR EDPL
#define kclsEDSL 'EDSL'
class EDSL : public EDSL_PAR
	{
	RTCLASS_DEC
	ASSERT

protected:
	//the text
	long _cch;
	achar _rgch[kcchMaxEdsl];

	EDSL(PEDPAR pedpar);

	//methods of EDCB
	virtual long _LnFromIch(long ich);
	virtual long _IchMinLn(long ln);
	virtual long _LnMac(void);
	virtual bool _FFilterCh(achar ch);

	//methods of EDPL
	virtual bool _FLockLn(long ln, achar **pprgch, long *pcch);
	virtual void _UnlockLn(long ln, achar *prgch);

public:
	static PEDSL PedslNew(PEDPAR pedpar);

	virtual long IchMac(void);
	virtual bool FReplace(achar *prgch, long cchIns,
		long ich1, long ich2, long gin = kginDraw);
	virtual long CchFetch(achar *prgch, long ich, long cchWant);

	//additional text APIs
	void GetStn(PSTN pstn);
	void SetStn(PSTN pstn, long gin = kginDraw);
	};


/***************************************************************************
	Multi line edit control.
***************************************************************************/
typedef class EDML *PEDML;
#define EDML_PAR EDPL
#define kclsEDML 'EDML'
class EDML : public EDML_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	//the text
	BSM _bsm;
	PGL _pglich;

	EDML(PEDPAR pedpar);

	//methods of EDCB
	virtual bool _FInit(void);
	virtual long _LnFromIch(long ich);
	virtual long _IchMinLn(long ln);
	virtual long _LnMac(void);
	virtual bool _FFilterCh(achar ch);

	//methods of EDPL
	virtual bool _FLockLn(long ln, achar **pprgch, long *pcch);
	virtual void _UnlockLn(long ln, achar *prgch);

	virtual long _ClnEstimate(achar *prgch, long cch);
	virtual long _LnReformat(long lnMin, long *pclnDel, long *pclnIns);
	virtual bool _FReplaceCore(achar *prgch, long cchIns, long ich,
		long cchDel);

public:
	static PEDML PedmlNew(PEDPAR pedpar);
	~EDML(void);

	virtual long IchMac(void);
	virtual bool FReplace(achar *prgch, long cchIns,
		long ich1, long ich2, long gin = kginDraw);
	virtual long CchFetch(achar *prgch, long ich, long cchWant);
	};


/***************************************************************************
	Multi line wrapping edit control.
***************************************************************************/
typedef class EDMW *PEDMW;
#define EDMW_PAR EDML
#define kclsEDMW 'EDMW'
class EDMW : public EDMW_PAR
	{
	RTCLASS_DEC

protected:
	EDMW(PEDPAR pedpar);

	//methods EDMW
	virtual long _ClnEstimate(achar *prgch, long cch);
	virtual long _LnReformat(long lnMin, long *pclnDel, long *pclnIns);

	long _CichGetBreakables(achar *prgch, long ich, long *prgich, long cichMax);
	virtual void _NewRc(void);

public:
	static PEDMW PedmwNew(PEDPAR pedpar);
	};


#endif //!TEXT_H

