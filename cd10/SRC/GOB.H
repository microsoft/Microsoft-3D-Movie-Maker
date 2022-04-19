/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Graphic object class.

***************************************************************************/
#ifndef GOB_H
#define GOB_H


enum
	{
	fgobNil = 0,
	fgobSibling = 1,		//for Constructors
	fgobEnsureHwnd = 2,		//for FInitScreen
	fgobNoVis = 0,			//for DrawTree
	fgobAutoVis = 4,		//for DrawTree
	fgobUseVis = 8,			//for DrawTree
	};

// GOB invalidation types
enum
	{
	ginNil,
	kginDraw,
	kginMark,
	kginSysInval,
	kginDefault
	};

const long krelOne  = 0x00010000L; //denominator for relative rectangles
const long krelZero = 0;

#ifdef MAC
inline void GetClientRect(HWND hwnd, RCS *prcs)
	{ *prcs = hwnd->port.portRect; }
inline void InvalHwndRcs(HWND hwnd, RCS *prcs)
	{
	PPRT pprt;

	GetPort(&pprt);
	SetPort(&hwnd->port);
	InvalRect(prcs);
	SetPort(pprt);
	}
inline void ValidHwndRcs(HWND hwnd, RCS *prcs)
	{
	PPRT pprt;

	GetPort(&pprt);
	SetPort(&hwnd->port);
	ValidRect(prcs);
	SetPort(pprt);
	}
#endif //MAC
#ifdef WIN
inline void InvalHwndRcs(HWND hwnd, RCS *prcs)
	{ InvalidateRect(hwnd, prcs, fFalse); }
inline void ValidHwndRcs(HWND hwnd, RCS *prcs)
	{ ValidateRect(hwnd, prcs); }
#endif //WIN


//coordinates
enum
	{
	cooLocal,	//top-left is (0,0)
	cooParent,	//relative to parent
	cooGpt,		//relative to the UI port
	cooHwnd,	//relative to the enclosing hwnd
	cooGlobal,	//global coordinates
	cooLim
	};

/****************************************
	GOB creation block
****************************************/
struct GCB
	{
	long _hid;
	PGOB _pgob;
	ulong _grfgob;
	long _gin;
	RC _rcAbs;
	RC _rcRel;

	GCB(void) {}
	GCB(long hid, PGOB pgob, ulong grfgob = fgobNil, long gin = kginDefault,
			RC *prcAbs = pvNil, RC *prcRel = pvNil)
		{ Set(hid, pgob, grfgob, gin, prcAbs, prcRel); }
	void Set(long hid, PGOB pgob, ulong grfgob = fgobNil, long gin = kginDefault,
		RC *prcAbs = pvNil, RC *prcRel = pvNil);
	};
typedef GCB *PGCB;


/****************************************
	Graphics object
****************************************/
#define GOB_PAR CMH
#define kclsGOB 'GOB'
class GOB : public GOB_PAR
	{
	RTCLASS_DEC
	CMD_MAP_DEC(GOB)
	ASSERT
	MARKMEM

	friend class GTE;

private:
	static PGOB _pgobScreen;

	HWND _hwnd; //the OS window (may be nil)
	PGPT _pgpt;	//the graphics port (may be shared with _pgobPar)
	PCURS _pcurs; // the cursor to show over this gob

	RC _rcCur;	//current position
	RC _rcVis;	//current visible rectangle (in its parent)
	RC _rcAbs;	//_rcAbs and _rcRel describe the position of this
	RC _rcRel;	//gob in its parent.

	// tree management
	PGOB _pgobPar;
	PGOB _pgobChd;
	PGOB _pgobSib;

	// variables
	PGL _pglrtvm;

	void _SetRcCur(void);
	HWND _HwndGetDptFromCoo(PT *pdpt, long coo);

protected:
	static long _ginDefGob;
	static long _gridLast;

	long _grid;
	long _ginDefault: 8;
	long _fFreeing: 1;
	long _fCreating: 1;

	~GOB(void);

	static HWND _HwndNewMdi(PSTN pstnTitle);
	static void _DestroyHwnd(HWND hwnd);

	void _Init(PGCB pgcb);
	HWND _HwndGetRc(RC *prc);
	virtual void _NewRc(void) {}
	virtual void _ActivateHwnd(bool fActive) {}

public:
	static bool FInitScreen(ulong grfgob, long ginDef);
	static void ShutDown(void);
	static PGOB PgobScreen(void) { return _pgobScreen; }
	static PGOB PgobFromHwnd(HWND hwnd);
	static PGOB PgobFromClsScr(long cls);
	static PGOB PgobFromHidScr(long hid);
	static void MakeHwndActive(HWND hwnd);
	static void ActivateHwnd(HWND hwnd, bool fActive);
	static HWND HwndMdiActive(void);
	static PGOB PgobMdiActive(void);
	static PGOB PgobFromPtGlobal(long xp, long yp, PT *pptLocal = pvNil);
	static long GinDefault(void)
		{ return _ginDefGob; }

	GOB(GCB *pgcb);
	GOB(long hid);
	virtual void Release(void);

	//hwnd stuff
	bool FAttachHwnd(HWND hwnd);
	bool FCreateAndAttachMdi(PSTN pstnTitle);
	HWND Hwnd(void)
		{ return _hwnd; }
	HWND HwndContainer(void);
	virtual void GetMinMax(RC *prcMinMax);
	void SetHwndName(PSTN pstn);

	// unique gob run-time id.
	long Grid(void)
		{ return _grid; }

	//tree management
	PGOB PgobPar(void)
		{ return _pgobPar; }
	PGOB PgobFirstChild(void)
		{ return _pgobChd; }
	PGOB PgobLastChild(void);
	PGOB PgobNextSib(void)
		{ return _pgobSib; }
	PGOB PgobPrevSib(void);
	PGOB PgobFromCls(long cls);
	PGOB PgobChildFromCls(long cls);
	PGOB PgobParFromCls(long cls);
	PGOB PgobFromHid(long hid);
	PGOB PgobChildFromHid(long hid);
	PGOB PgobParFromHid(long hid);
	PGOB PgobFromGrid(long grid);
	void BringToFront(void);
	void SendBehind(PGOB pgobBefore);

	//rectangle management
	void SetPos(RC *prcAbs, RC *prcRel = pvNil);
	void GetPos(RC *prcAbs, RC *prcRel);
	void GetRc(RC *prc, long coo);
	void GetRcVis(RC *prc, long coo);
	void SetRcFromHwnd(void);
	virtual void Maximize(void);

	void MapPt(PT *ppt, long cooSrc, long cooDst);
	void MapRc(RC *prc, long cooSrc, long cooDst);

	//variables
	virtual PGL *Ppglrtvm(void);

	PGPT Pgpt(void) { return _pgpt; }
	void InvalRc(RC *prc, long gin = kginDefault);
	void ValidRc(RC *prc, long gin = kginDefault);
	bool FGetRcInval(RC *prc, long gin = kginDefault);
	void Scroll(RC *prc, long dxp, long dyp, long gin,
		RC *prcBad1 = pvNil, RC *prcBad2 = pvNil);

	virtual void Clean(void);
	virtual void DrawTree(PGPT pgpt, RC *prc, RC *prcUpdate, ulong grfgob);
	virtual void DrawTreeRgn(PGPT pgpt, RC *prc, REGN *pregn, ulong grfgob);
	virtual void Draw(PGNV pgnv, RC *prcClip);

	//mouse handling and hit testing
	void GetPtMouse(PT *ppt, bool *pfDown);
	virtual PGOB PgobFromPt(long xp, long yp, PT *pptLocal = pvNil);
	virtual bool FPtIn(long xp, long yp);
	virtual bool FPtInBounds(long xp, long yp);
	virtual void MouseDown(long xp, long yp, long cact, ulong grfcust);
	virtual long ZpDragRc(RC *prc, bool fVert, long zp, long zpMin, long zpLim,
		long zpMinActive, long zpLimActive);
	void SetCurs(PCURS pcurs);
	void SetCursCno(PRCA prca, CNO cno);

	#ifdef MAC
		virtual void TrackGrow(PEVT pevt);
	#endif //MAC

	//command functions
	virtual bool FCmdCloseWnd(PCMD pcmd);
	virtual bool FCmdTrackMouse(PCMD_MOUSE pcmd);
	bool FCmdTrackMouseCore(PCMD pcmd)
		{ return FCmdTrackMouse((PCMD_MOUSE)pcmd); }
	virtual bool FCmdMouseMove(PCMD_MOUSE pcmd);
	bool FCmdMouseMoveCore(PCMD pcmd)
		{ return FCmdMouseMove((PCMD_MOUSE)pcmd); }

	//key commands
	virtual bool FCmdKey(PCMD_KEY pcmd);
	bool FCmdKeyCore(PCMD pcmd)
		{ return FCmdKey((PCMD_KEY)pcmd); }
	virtual bool FCmdBadKey(PCMD_BADKEY pcmd);
	bool FCmdBadKeyCore(PCMD pcmd)
		{ return FCmdBadKey((PCMD_BADKEY)pcmd); }
	virtual bool FCmdSelIdle(PCMD pcmd);
	virtual bool FCmdActivateSel(PCMD pcmd);

	// tool tips
	virtual bool FEnsureToolTip(PGOB *ppgobCurTip, long xpMouse, long ypMouse);

	// gob state (for automated testing)
	virtual long LwState(void);

	#ifdef DEBUG
		void MarkGobTree(void);
	#endif //DEBUG
	};


/****************************************
	Gob Tree Enumerator
****************************************/
enum
	{
	// inputs
	fgteNil = 0x0000,
	fgteSkipToSib = 0x0001,		//legal to FNextGob
	fgteBackToFront = 0x0002,	//legal to Init

	// outputs
	fgtePre = 0x0010,
	fgtePost = 0x0020,
	fgteRoot = 0x0040
	};

#define GTE_PAR BASE
#define kclsGTE 'GTE'
class GTE : public GTE_PAR
	{
	RTCLASS_DEC
	ASSERT

private:
	// enumeration states
	enum
		{
		esStart,
		esGoDown,
		esGoRight,
		esDone
		};

	long _es;
	bool _fBackWards;	//which way to walk sibling lists
	PGOB _pgobRoot;
	PGOB _pgobCur;

public:
	GTE(void);
	void Init(PGOB pgob, ulong grfgte);
	bool FNextGob(PGOB *ppgob, ulong *pgrfgteOut, ulong grfgteIn);
	};


#endif //!GOB_H

