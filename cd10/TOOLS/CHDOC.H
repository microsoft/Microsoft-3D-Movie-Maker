/***************************************************************************

	CHED document class

***************************************************************************/
#ifndef CHDOC_H
#define CHDOC_H

typedef class DOC *PDOC;
typedef class DOCE *PDOCE;
typedef class DOCH *PDOCH;
typedef class DOCG *PDOCG;
typedef class DOCI *PDOCI;
typedef class DOCPIC *PDOCPIC;
typedef class DOCMBMP *PDOCMBMP;
typedef class SEL *PSEL;
typedef class DCD *PDCD;
typedef class DCH *PDCH;
typedef class DCGB *PDCGB;
typedef class DCGL *PDCGL;
typedef class DCGG *PDCGG;
typedef class DCST *PDCST;
typedef class DCPIC *PDCPIC;
typedef class DCMBMP *PDCMBMP;


bool FGetCtgFromStn(CTG *pctg, PSTN pstn);

#define lnNil (-1L)


/***************************************************************************

	Various document classes. DOC is the chunky file based document.
	DOCE is a virtual class for documents that represent an individual
	chunk in a DOC. A DOCE is a child document of a DOC. All other
	document classes in this header are derived from DOCE.

***************************************************************************/

/***************************************************************************
	chunky file doc
***************************************************************************/
#define DOC_PAR DOCB
#define kclsDOC 'DOC'
class DOC : public DOC_PAR
	{
	RTCLASS_DEC
	ASSERT

protected:
	PCFL _pcfl;		//the chunky file

	DOC(void);
	~DOC(void);

public:
	static PDOC PdocNew(FNI *pfni);

	PCFL Pcfl(void)
		{ return _pcfl; }
	virtual PDDG PddgNew(PGCB pgcb);
	virtual bool FGetFni(FNI *pfni);
	virtual bool FGetFniSave(FNI *pfni);
	virtual bool FSaveToFni(FNI *pfni, bool fSetFni);
	};


/***************************************************************************
	Chunky editing doc - abstract class for editing a single chunk in a
	Chunky file. An instance of this class is a child doc of a DOC. Many
	document classes below are all derived from this.
***************************************************************************/
#define DOCE_PAR DOCB
#define kclsDOCE 'DOCE'
class DOCE : public DOCE_PAR
	{
	RTCLASS_DEC
	ASSERT

protected:
	PCFL _pcfl;		//which chunk is being edited
	CTG _ctg;
	CNO _cno;

	DOCE(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno);
	bool _FInit(void);

	virtual bool _FSaveToChunk(CTG ctg, CNO cno, bool fRedirect);
	virtual bool _FWrite(PBLCK pblck, bool fRedirect) = 0;
	virtual long _CbOnFile(void) = 0;
	virtual bool _FRead(PBLCK pblck) = 0;

public:
	static PDOCE PdoceFromChunk(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno);
	static void CloseDeletedDoce(PDOCB pdocb);

	virtual void GetName(PSTN pstn);
	virtual bool FSave(long cid);
	};


/***************************************************************************
	Hex editor document - for editing any chunk as a hex stream.
***************************************************************************/
#define DOCH_PAR DOCE
#define kclsDOCH 'DOCH'
class DOCH : public DOCH_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	BSF _bsf;		//the byte stream

	DOCH(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno);
	virtual bool _FWrite(PBLCK pblck, bool fRedirect);
	virtual long _CbOnFile(void);
	virtual bool _FRead(PBLCK pblck);

public:
	static PDOCH PdochNew(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno);
	virtual PDDG PddgNew(PGCB pgcb);
	};



/***************************************************************************
	Group editor document - for editing GL, AL, GG, AG, GST, and AST.
***************************************************************************/
#define DOCG_PAR DOCE
#define kclsDOCG 'DOCG'
class DOCG : public DOCG_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PGRPB _pgrpb;
	long _cls;		//which class the group belongs to
	short _bo;
	short _osk;

	DOCG(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno, long cls);
	~DOCG(void);
	virtual bool _FWrite(PBLCK pblck, bool fRedirect);
	virtual long _CbOnFile(void);
	virtual bool _FRead(PBLCK pblck);

public:
	static PDOCG PdocgNew(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno, long cls);
	virtual PDDG PddgNew(PGCB pgcb);

	PDOCI PdociFromItem(long iv, long dln);
	void CloseDeletedDoci(long iv, long cvDel);
	PGRPB Pgrpb(void) { return _pgrpb; }
	};


/***************************************************************************
	Item hex editor document - for editing an item in a GRPB. An instance
	of this class is normally a child doc of a DOCG (but doesn't have to be).
***************************************************************************/
#define DOCI_PAR DOCB
#define kclsDOCI 'DOCI'
class DOCI : public DOCI_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PGRPB _pgrpb;	//the group the data came from and gets written to.
	long _cls;
	long _iv;		//which item is being edited
	long _dln;
	bool _fFixed;	//indicates if the data is fixed length
	BSF _bsf;		//the byte stream we're editing

	DOCI(PDOCB pdocb, PGRPB pgrpb, long cls, long iv, long dln);
	bool _FInit(void);

	virtual bool _FSaveToItem(long iv, bool fRedirect);
	virtual bool _FWrite(long iv);
	virtual HQ _HqRead();

public:
	static PDOCI PdociNew(PDOCB pdocb, PGRPB pgrpb, long cls, long iv, long dln);
	virtual PDDG PddgNew(PGCB pgcb);

	long Iv(void) { return _iv; }
	long Dln(void) { return _dln; }

	virtual void GetName(PSTN pstn);
	virtual bool FSave(long cid);
	};


/***************************************************************************
	Picture display document.
***************************************************************************/
#define DOCPIC_PAR DOCE
#define kclsDOCPIC 'dcpc'
class DOCPIC : public DOCPIC_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PPIC _ppic;

	DOCPIC(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno);
	~DOCPIC(void);

	virtual bool _FWrite(PBLCK pblck, bool fRedirect);
	virtual long _CbOnFile(void);
	virtual bool _FRead(PBLCK pblck);

public:
	static PDOCPIC PdocpicNew(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno);

	virtual PDDG PddgNew(PGCB pgcb);
	PPIC Ppic(void)
		{ return _ppic; }
	};


/***************************************************************************
	MBMP display document.
***************************************************************************/
#define DOCMBMP_PAR DOCE
#define kclsDOCMBMP 'docm'
class DOCMBMP : public DOCMBMP_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PMBMP _pmbmp;

	DOCMBMP(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno);
	~DOCMBMP(void);

	virtual bool _FWrite(PBLCK pblck, bool fRedirect);
	virtual long _CbOnFile(void);
	virtual bool _FRead(PBLCK pblck);

public:
	static PDOCMBMP PdocmbmpNew(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno);

	virtual PDDG PddgNew(PGCB pgcb);
	PMBMP Pmbmp(void)
		{ return _pmbmp; }
	};


/***************************************************************************
	Document editing window classes follow. These are all DDG's.
	Most are also DCLB's (the first class defined below).  DCLB is
	an abstract class that handles a line based editing window.
	The DCD class is for displaying a DOC (chunky file document).
***************************************************************************/


/***************************************************************************
	abstract class for line based document windows
***************************************************************************/
#define DCLB_PAR DDG
#define kclsDCLB 'DCLB'
class DCLB : public DCLB_PAR
	{
	RTCLASS_DEC
	ASSERT

protected:
	long _onn;				//fixed width font to use
	long _dypHeader;		//height of the header
	long _dypLine;			//height of one line
	long _dxpChar;			//width of a character

	DCLB(PDOCB pdocb, PGCB pgcb);
	virtual void _Scroll(long scaHorz, long scaVert,
		long scvHorz = 0, long scvVert = 0);
	virtual void _ScrollDxpDyp(long dxp, long dyp);
	virtual void GetMinMax(RC *prcMinMax);

	long _YpFromLn(long ln)
		{ return LwMul(ln - _scvVert, _dypLine) + _dypHeader; }
	long _XpFromIch(long ich)
		{ return LwMul(ich - _scvHorz + 1, _dxpChar); }
	long _LnFromYp(long yp);

	void _GetContent(RC *prc);
	};


/***************************************************************************
	SEL: used to track a selection in a chunky file doc
***************************************************************************/
enum
	{
	fselNil = 0,
	fselCki = 1,
	fselKid = 2
	};

#define SEL_PAR BASE
#define kclsSEL 'SEL'
class SEL : public SEL_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PCFL _pcfl;
	long _icki;
	long _ikid;
	CKI _cki;
	KID _kid;
	long _ln;
	long _lnLim;		// this is lnNil if we haven't yet calculated the lim
	PGL _pglctg;		// the ctgs to filter on
	bool _fHideList: 1;	// whether to hide the ctgs in the list or show them
	bool _fHideKids: 1;	// whether to hide the kids

	void _SetNil(void);
	bool _FFilter(CTG ctg, CNO cno);

public:
	SEL(PCFL pcfl);
	SEL(SEL &selT);
	~SEL(void);
	SEL & operator = (SEL &selT);

	void Adjust(bool fExact = fFalse);

	long Icki(void) { return _icki; }
	long Ikid(void) { return _ikid; }
	long Ln(void) { return _ln; }
	ulong GrfselGetCkiKid(CKI *pcki, KID *pkid);

	bool FSetLn(long ln);
	bool FAdvance(void);
	bool FRetreat(void);
	bool FSetCkiKid(CKI *pcki, KID *pkid = pvNil, bool fExact = fTrue);
	long LnLim(void);
	void InvalLim(void)
		{ _lnLim = lnNil; }

	bool FHideKids(void)
		{ return _fHideKids; }
	void HideKids(bool fHide);

	bool FHideList(void)
		{ return _fHideList; }
	void HideList(bool fHide);
	bool FGetCtgFilter(long ictg, CTG *pctg);
	void FreeFilterList(void);
	bool FAddCtgFilter(CTG ctg);
	};


/***************************************************************************
	Display for chunky document - displays a DOC.
***************************************************************************/
#define DCD_PAR DCLB
#define kclsDCD 'DCD'
class DCD : public DCD_PAR
	{
	RTCLASS_DEC
	CMD_MAP_DEC(DCD)
	ASSERT
	MARKMEM

protected:
	long _dypBorder;		//height of border (included in _dypLine)
	PCFL _pcfl;				//the chunky file
	SEL _sel;				//the current selection

	DCD(PDOCB pdocb, PCFL pcfl, PGCB pgcb);
	void _DrawSel(PGNV pgnv);
	void _HiliteLn(long ln);
	void _SetSel(long ln, CKI *pcki = pvNil, KID *pkid = pvNil);
	void _ShowSel(void);

	virtual void _Activate(bool fActive);
	virtual long _ScvMax(bool fVert);
	bool _FAddChunk(CTG ctgDef, CKI *pcki, bool *pfCreated);
	bool _FEditChunkInfo(CKI *pckiOld);
	bool _FChangeChid(CKI *pcki, KID *pkid);

	bool _FDoAdoptChunkDlg(CKI *pcki, KID *pkid);
	void _EditCki(CKI *pcki, long cid);

	void _InvalCkiKid(CKI *pcki = pvNil, KID *pkid = pvNil);

	//clipboard support
	virtual bool _FCopySel(PDOCB *ppdocb = pvNil);
	virtual void _ClearSel(void);
	virtual bool _FPaste(PCLIP pclip, bool fDoIt, long cid);

public:
	static PDCD PdcdNew(PDOCB pdocb, PCFL pcfl, PGCB pgcb);
	static void InvalAllDcd(PDOCB pdocb, PCFL pcfl,
		CKI *pcki = pvNil, KID *pkid = pvNil);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual void MouseDown(long xp, long yp, long cact, ulong grfcust);
	virtual bool FCmdKey(PCMD_KEY pcmd);

	virtual bool FEnableDcdCmd(PCMD pcmd, ulong *pgrfeds);
	virtual bool FCmdAddChunk(PCMD pcmd);
	virtual bool FCmdDeleteChunk(PCMD pcmd);
	virtual bool FCmdAdoptChunk(PCMD pcmd);
	virtual bool FCmdUnadoptChunk(PCMD pcmd);
	virtual bool FCmdEditChunk(PCMD pcmd);
	virtual bool FCmdAddPicChunk(PCMD pcmd);
	virtual bool FCmdAddBitmapChunk(PCMD pcmd);
	virtual bool FCmdImportScript(PCMD pcmd);
	virtual bool FCmdTestScript(PCMD pcmd);
	virtual bool FCmdDisasmScript(PCMD pcmd);
	virtual bool FCmdAddFileChunk(PCMD pcmd);
	virtual bool FCmdEditChunkInfo(PCMD pcmd);
	virtual bool FCmdChangeChid(PCMD pcmd);
	virtual bool FCmdSetColorTable(PCMD pcmd);
	virtual bool FCmdFilterChunk(PCMD pcmd);
	virtual bool FCmdPack(PCMD pcmd);
	virtual bool FCmdStopSound(PCMD pcmd);
	virtual bool FCmdCloneChunk(PCMD pcmd);
	virtual bool FCmdReopen(PCMD pcmd);

	bool FTestScript(CTG ctg, CNO cno, long cbCache = 0x00300000L);
	bool FPlayMidi(CTG ctg, CNO cno);
	bool FPlayWave(CTG ctg, CNO cno);
	};


/***************************************************************************
	Display chunk in hex - displays a BSF (byte stream), but
	doesn't necessarily display a DOCH.
***************************************************************************/
#define DCH_PAR DCLB
#define kclsDCH 'DCH'
class DCH : public DCH_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PBSF _pbsf;			//the byte stream
	long _cbLine;		//number of bytes per line

	//the selection
	long _ibAnchor;
	long _ibOther;

	bool _fSelOn: 1;	//selection is showing
	bool _fRightSel: 1;	//selection if on a line boundary is at the right edge
	bool _fHalfSel: 1;	//second half of hex character is selected
	bool _fHexSel: 1;	//hex area active
	bool _fFixed: 1;	//indicates if the data is fixed length

	DCH(PDOCB pdocb, PBSF pbsf, bool fFixed, PGCB pgcb);

	virtual void _Activate(bool fActive);
	virtual long _ScvMax(bool fVert);

	long _IchFromCb(long cb, bool fHex, bool fNoTrailSpace = fFalse);
	long _XpFromCb(long cb, bool fHex, bool fNoTrailSpace = fFalse);
	long _XpFromIb(long ib, bool fHex);
	long _YpFromIb(long ib);
	long _IbFromPt(long xp, long yp, bool *ptHex, bool *pfRight = pvNil);

	void _SetSel(long ibAnchor, long ibOther, bool fRight);
	void _SetHalfSel(long ib);
	void _SetHexSel(bool fHex);
	void _SwitchSel(bool fOn);
	void _ShowSel(void);
	void _InvertSel(PGNV pgnv);
	void _InvertIbRange(PGNV pgnv, long ib1, long ib2, bool fHex);

	bool _FReplace(byte *prgb, long cb, long ibMin, long ibLim, bool fHalfSel = fFalse);
	void _InvalAllDch(long ib, long cbIns, long cbDel);
	void _InvalIb(long ib, long cbIns, long cbDel);

	void _DrawHeader(PGNV pgnv);

	//clipboard support
	virtual bool _FCopySel(PDOCB *ppdocb = pvNil);
	virtual void _ClearSel(void);
	virtual bool _FPaste(PCLIP pclip, bool fDoIt, long cid);

public:
	static PDCH PdchNew(PDOCB pdocb, PBSF pbsf, bool fFixed, PGCB pgcb);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual void MouseDown(long xp, long yp, long cact, ulong grfcust);
	virtual bool FCmdKey(PCMD_KEY pcmd);
	};


/***************************************************************************
	Virtual class that supports displaying a group chunk - displays a GRPB.
	Usually displays a DOCG, but doesn't have to.
***************************************************************************/
#define DCGB_PAR DCLB
#define kclsDCGB 'DCGB'
class DCGB : public DCGB_PAR
	{
	RTCLASS_DEC
	CMD_MAP_DEC(DCGB)
	ASSERT
	MARKMEM

protected:
	long _dypBorder;		//height of border (included in _dypLine)
	long _clnItem;			//number of lines for each item
	long _ivCur;			//which item is selected
	long _dlnCur;			//which line in the item is selected
	PGRPB _pgrpb;			//the group we're displaying
	long _cls;				//the class of the group
	bool _fAllocated;		//whether the class is allocated or general

	DCGB(PDOCB pdocb, PGRPB pgrpb, long cls, long clnItem, PGCB pgcb);

	virtual void _Activate(bool fActive);
	virtual long _ScvMax(bool fVert);
	long _YpFromIvDln(long iv, long dln)
		{ return _YpFromLn(LwMul(iv, _clnItem) + dln); }
	long _LnFromIvDln(long iv, long dln)
		{ return LwMul(iv, _clnItem) + dln; }
	long _LnLim(void)
		{ return LwMul(_pgrpb->IvMac(), _clnItem); }
	void _SetSel(long ln);
	void _ShowSel(void);
	void _DrawSel(PGNV pgnv);
	void _InvalIv(long iv, long cvIns, long cvDel);
	void _EditIvDln(long iv, long dln);
	void _DeleteIv(long iv);

public:
	static void DCGB::InvalAllDcgb(PDOCB pdocb, PGRPB pgrpb,
		long iv, long cvIns, long cvDel);
	virtual bool FCmdKey(PCMD_KEY pcmd);
	virtual void MouseDown(long xp, long yp, long cact, ulong grfcust);

	virtual bool FEnableDcgbCmd(PCMD pcmd, ulong *pgrfeds);
	virtual bool FCmdEditItem(PCMD pcmd);
	virtual bool FCmdDeleteItem(PCMD pcmd);
	virtual bool FCmdAddItem(PCMD pcmd) = 0;
	};


/***************************************************************************
	Display GL or AL chunk.
***************************************************************************/
#define DCGL_PAR DCGB
#define kclsDCGL 'DCGL'
class DCGL : public DCGL_PAR
	{
	RTCLASS_DEC

protected:
	DCGL(PDOCB pdocb, PGLB pglb, long cls, PGCB pgcb);

public:
	static PDCGL PdcglNew(PDOCB pdocb, PGLB pglb, long cls, PGCB pgcb);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FCmdAddItem(PCMD pcmd);
	};


/***************************************************************************
	Display GG or AG chunk.
***************************************************************************/
#define DCGG_PAR DCGB
#define kclsDCGG 'DCGG'
class DCGG : public DCGG_PAR
	{
	RTCLASS_DEC

protected:
	DCGG(PDOCB pdocb, PGGB pggb, long cls, PGCB pgcb);

public:
	static PDCGG PdcggNew(PDOCB pdocb, PGGB pggb, long cls, PGCB pgcb);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FCmdAddItem(PCMD pcmd);
	};


/***************************************************************************
	Display GST or AST chunk.
***************************************************************************/
#define DCST_PAR DCGB
#define kclsDCST 'DCST'
class DCST : public DCST_PAR
	{
	RTCLASS_DEC

protected:
	DCST(PDOCB pdocb, PGSTB pgstb, long cls, PGCB pgcb);

public:
	static PDCST PdcstNew(PDOCB pdocb, PGSTB pgstb, long cls, PGCB pgcb);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FCmdAddItem(PCMD pcmd);
	};


/***************************************************************************
	Display a picture chunk.
***************************************************************************/
#define DCPIC_PAR DDG
#define kclsDCPIC 'dpic'
class DCPIC : public DCPIC_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PPIC _ppic;

	DCPIC(PDOCB pdocb, PPIC ppic, PGCB pgcb);
	virtual void GetMinMax(RC *prcMinMax);

public:
	static PDCPIC PdcpicNew(PDOCB pdocb, PPIC ppic, PGCB pgcb);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	};


/***************************************************************************
	Display a MBMP chunk.
***************************************************************************/
#define DCMBMP_PAR DDG
#define kclsDCMBMP 'dmbp'
class DCMBMP : public DCMBMP_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PMBMP _pmbmp;

	DCMBMP(PDOCB pdocb, PMBMP pmbmp, PGCB pgcb);
	virtual void GetMinMax(RC *prcMinMax);

public:
	static PDCMBMP PdcmbmpNew(PDOCB pdocb, PMBMP pmbmp, PGCB pgcb);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	};


/***************************************************************************
	Main Kidspace world for testing a script.
***************************************************************************/
typedef class TSCG *PTSCG;
#define TSCG_PAR WOKS
#define kclsTSCG 'TSCG'
class TSCG : public TSCG_PAR
	{
	RTCLASS_DEC
	CMD_MAP_DEC(TSCG)

public:
	TSCG(PGCB pgcb) : TSCG_PAR(pgcb) {}

	virtual void Draw(PGNV pgnv, RC *prcClip);
	};


/***************************************************************************
	Text doc for the chunky editor.
***************************************************************************/
typedef class CHTXD *PCHTXD;
#define CHTXD_PAR TXPD
#define kclsCHTXD 'chtx'
class CHTXD : public CHTXD_PAR
	{
protected:
	CHTXD(PDOCB pdocb = pvNil, ulong grfdoc = fdocNil);

public:
	static PCHTXD PchtxdNew(PFNI pfni = pvNil, PBSF pbsf = pvNil,
		short osk = koskCur, PDOCB pdocb = pvNil, ulong grfdoc = fdocNil);

	virtual PDDG PddgNew(PGCB pgcb);
	};


/***************************************************************************
	Text display gob for the chunky editor.
***************************************************************************/
typedef class CHTDD *PCHTDD;
#define CHTDD_PAR TXLG
#define kclsCHTDD 'chtd'
class CHTDD : public CHTDD_PAR
	{
	CMD_MAP_DEC(CHTDD)

protected:
	CHTDD(PTXTB ptxtb, PGCB pgcb,
		long onn, ulong grfont, long dypFont, long cchTab);

public:
	static PCHTDD PchtddNew(PTXTB ptxtb, PGCB pgcb,
		long onn, ulong grfont, long dypFont, long cchTab);

	virtual bool FCmdCompileChunky(PCMD pcmd);
	virtual bool FCmdCompileScript(PCMD pcmd);
	};

void OpenSinkDoc(PMSFIL pmsfil);

#endif //!CHDOC_H
