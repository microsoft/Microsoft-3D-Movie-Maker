/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	A base document class and its supporting gob classes.

***************************************************************************/
#ifndef DOCB_H
#define DOCB_H


/***************************************************************************
	base undo class
***************************************************************************/
typedef class UNDB *PUNDB;
#define UNDB_PAR BASE
#define kclsUNDB 'UNDB'
class UNDB : public UNDB_PAR
	{
	RTCLASS_DEC
	NOCOPY(UNDB)

protected:
	UNDB(void) {}

public:

	//General undo funtionality
	virtual bool FUndo(PDOCB pdocb) = 0;
	virtual bool FDo(PDOCB pdocb) = 0;
	};



/***************************************************************************
	base document class
***************************************************************************/
enum
	{
	fdocNil = 0,
	fdocSibling = 1,
	fdocForceClose = 2,	//for FQueryClose, etc
	fdocAssumeYes = 4,	//for FQueryClose, etc

	fdocUpdate = 8,		//update associated DDGs
	fdocInval = 16,		//invalidate associated DDGs
	};

#define DOCB_PAR CMH
#define kclsDOCB 'DOCB'
class DOCB : public DOCB_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

	friend class DTE;

protected:
	static long _cactLast;
	static PDOCB _pdocbFirst;

	PDOCB _pdocbPar;
	PDOCB _pdocbSib;
	PDOCB _pdocbChd;

	long _cactUntitled; //0 if titled
	bool _fDirty: 1;
	bool _fFreeing: 1;
	bool _fInternal: 1;
	PGL _pglpddg;		//keep track of the DDGs based on this doc

	PGL _pglpundb;		//keep track of undo items
	long _ipundbLimDone;
	long _cundbMax;

	bool _FFindDdg(PDDG pddg, long *pipddg);
	virtual bool _TQuerySave(bool fForce);

	DOCB(PDOCB pdocb = pvNil, ulong grfdoc = fdocNil);
	~DOCB(void);

public:
	static bool FQueryCloseAll(ulong grfdoc);
	static PDOCB PdocbFromFni(FNI *pfni);

	static PDOCB PdocbFirst(void)
		{ return _pdocbFirst; }
	PDOCB PdocbPar(void)
		{ return _pdocbPar; }
	PDOCB PdocbSib(void)
		{ return _pdocbSib; }
	PDOCB PdocbChd(void)
		{ return _pdocbChd; }

	virtual void Release(void);

	//high level call to create a new MDI window based on the doc.
	virtual PDMD PdmdNew(void);
	void ActivateDmd(void);

	//low level calls - generally not for public consumption
	virtual PDMW PdmwNew(PGCB pgcb);
	virtual PDSG PdsgNew(PDMW pdwm, PDSG pdsgSplit, ulong grfdsg, long rel);
	virtual PDDG PddgNew(PGCB pgcb);

	//DDG management - only to be called by DDGs
	bool FAddDdg(PDDG pddg);
	void RemoveDdg(PDDG pddg);
	void MakeFirstDdg(PDDG pddg);
	void CloseAllDdg(void);

	//General DDG management
	long Cddg(void)
		{ return pvNil == _pglpddg ? 0 : _pglpddg->IvMac(); }
	PDDG PddgGet(long ipddg);
	PDDG PddgActive(void);

	virtual void UpdateName(void);
	virtual void GetName(PSTN pstn);
	virtual bool FQueryClose(ulong grfdoc);
	virtual bool FQueryCloseDmd(PDMD pdmd);
	virtual bool FSave(long cid = cidSave);

	virtual bool FGetFni(FNI *pfni);
	virtual bool FGetFniSave(FNI *pfni);
	virtual bool FSaveToFni(FNI *pfni, bool fSetFni);
	virtual bool FDirty(void)
		{ return _fDirty && !FInternal(); }
	virtual void SetDirty(bool fDirty = fTrue)
		{ _fDirty = FPure(fDirty); }

	//General undo funtionality
	virtual bool FUndo(void);
	virtual bool FRedo(void);
	virtual bool FAddUndo(PUNDB pundb);
	virtual void ClearUndo(void);
	virtual void ClearRedo(void);
	virtual void SetCundbMax(long cundbMax);
	virtual long CundbMax(void);
	virtual long CundbUndo(void);
	virtual long CundbRedo(void);

	bool FInternal(void);
	void SetAsClipboard(void);
	void SetInternal(bool fInternal = fTrue);

	virtual void ExportFormats(PCLIP pclip);
	virtual bool FGetFormat(long cls, PDOCB *ppdocb = pvNil);
	};


/***************************************************************************
	document tree enumerator
***************************************************************************/
enum
	{
	// inputs
	fdteNil = 0,
	fdteSkipToSib = 1,	//legal to FNextDoc

	// outputs
	fdtePre = 2,
	fdtePost = 4,
	fdteRoot = 8
	};

#define DTE_PAR BASE
#define kclsDTE 'DTE'
class DTE : public DTE_PAR
	{
	RTCLASS_DEC
	ASSERT

private:
	// enumeration states
	enum
		{
		esStart,
		esGoDown,
		esGoLeft,
		esDone
		};

	long _es;
	PDOCB _pdocbRoot;
	PDOCB _pdocbCur;

public:
	DTE(void);
	void Init(PDOCB pdocb);
	bool FNextDoc(PDOCB *ppdocb, ulong *pgrfdteOut, ulong grfdteIn = fdteNil);
	};


/***************************************************************************
	document display gob - normally a child of a DSG but can be a child
	of any gob (for doc previewing, etc)
***************************************************************************/
#define DDG_PAR GOB
#define kclsDDG 'DDG'
class DDG : public DDG_PAR
	{
	RTCLASS_DEC
	CMD_MAP_DEC(DDG)
	ASSERT
	MARKMEM

protected:
	PDOCB _pdocb;
	bool _fActive;
	long _scvVert;	//scroll values
	long _scvHorz;

	DDG(PDOCB pdocb, PGCB pgcb);
	~DDG(void);

	virtual bool _FInit(void);
	virtual void _Activate(bool fActive);
	virtual void _NewRc(void);

	//scrolling support
	virtual long _ScvMax(bool fVert);
	virtual void _SetScrollValues(void);
	virtual void _Scroll(long scaHorz, long scaVert,
		long scvHorz = 0, long scvVert = 0);
	virtual void _ScrollDxpDyp(long dxp, long dyp);

	//clipboard support
	virtual bool _FCopySel(PDOCB *ppdocb = pvNil);
	virtual void _ClearSel(void);
	virtual bool _FPaste(PCLIP pclip, bool fDoIt, long cid);

public:
	static PDDG PddgNew(PDOCB pdocb, PGCB pgcb);

	PDOCB Pdocb(void)
		{ return _pdocb; }
	PDMD Pdmd(void);

	//activation
	virtual void Activate(bool fActive);
	bool FActive(void)
		{ return _fActive; }

	//members of GOB
	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FCmdActivateSel(PCMD pcmd);

	virtual bool FCmdScroll(PCMD pcmd);
	virtual bool FCmdCloseDoc(PCMD pcmd);
	virtual bool FCmdSave(PCMD pcmd);
	virtual bool FCmdClip(PCMD pcmd);
	virtual bool FEnableDdgCmd(PCMD pcmd, ulong *pgrfeds);
	virtual bool FCmdUndo(PCMD pcmd);
	};


/***************************************************************************
	Document mdi window - this communicates with the docb to coordinate
	closing and querying the user about saving
***************************************************************************/
#define DMD_PAR GOB
#define kclsDMD 'DMD'
class DMD : public DMD_PAR
	{
	RTCLASS_DEC

protected:
	PDOCB _pdocb;

	DMD(PDOCB pdocb, PGCB pgcb);
	virtual void _ActivateHwnd(bool fActive);

public:
	static PDMD PdmdNew(PDOCB pdocb);
	static PDMD PdmdTop(void);

	PDOCB Pdocb(void)
		{ return _pdocb; }
	virtual void ActivateNext(PDDG pddg);
	virtual bool FCmdCloseWnd(PCMD pcmd);
	};


/***************************************************************************
	Document main window
	provides basic pane management - including splitting, etc
***************************************************************************/
#define DMW_PAR GOB
#define kclsDMW 'DMW'
class DMW : public DMW_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	//DSG edge struct - these form a locally-balanced binary tree
	//with DSGs as the leafs.  Locally-balanced means that a node has a left
	//child iff it has a right child.
	struct DSED
		{
		bool fVert;	//splits its parent vertically, so the edge is horizontal
		long rel;	//where it splits its parent
		RC rcRel;	//current relative rectangle (in the DMW)
		long idsedLeft;
		long idsedRight;
		long idsedPar;
		PDSG pdsg;
		};
	PAL _paldsed;	//the tree of DSEDs
	long _idsedRoot;
	PDOCB _pdocb;

	DMW(PDOCB pdocb, PGCB pgcb);

	virtual bool _FInit(void);
	virtual void _NewRc(void);

	void _Layout(long idsedStart);
	long _IdsedNext(long idsed, long idsedRoot);
	long _IdsedEdge(long idsed, long idsedRoot);
	void _RemoveDsg(PDSG pdsg, long *pidsedStartLayout);
	DSED *_Qdsed(long idsed)
		{ return (DSED *)_paldsed->QvGet(idsed); }
	void _SplitRcRel(long idsed, RC *prcLeft, RC *prcRight);

public:
	static PDMW PdmwNew(PDOCB pdocb, PGCB pgcb);

	PDOCB Pdocb(void)
		{ return _pdocb; }

	bool FAddDsg(PDSG pdsg, PDSG pdsgSplit, ulong grfdsg, long rel);
	void RemoveDsg(PDSG pdsg);
	long Cdsg(void);

	void GetRcSplit(PDSG pdsg, RC *prcBounds, RC *prcSplit);
	void MoveSplit(PDSG pdsg, long relNew);
	bool TVert(PDSG pdsg);

	virtual void Release(void);
	};


/***************************************************************************
	document scroll gob - child gob of a DMW
	holds any scroll bars, splitter boxes and split movers
	dialogs tightly with DMW and DDG
***************************************************************************/
#define DSG_PAR GOB
#define kclsDSG 'DSG'
class DSG : public DSG_PAR
	{
	RTCLASS_DEC
	CMD_MAP_DEC(DSG)
	ASSERT

	friend DMW;

private:
	long _dsno; //this is how the DMW refers to this DSG
	PDDG _pddg;

protected:
	DSG(PGCB pgcb);
	~DSG(void);

	virtual bool _FInit(PDSG pdsgSplit, ulong grfdsg, long rel);

public:
	static PDSG PdsgNew(PDMW pdmw, PDSG pdsgSplit, ulong grfdsg, long rel);
	virtual void GetMinMax(RC *prcMinMax);

	PDMW Pdmw(void)
		{ return (PDMW)PgobPar(); }

	virtual void Split(ulong grfdsg, long rel);
	virtual bool FCmdScroll(PCMD pcmd);
	};


enum
	{
	fdsgNil  = 0,
	fdsgVert = 1,	//for splitting and PdsgNew
	fdsgHorz = 2,	//for splitting and PdsgNew
	fdsgAfter = 4	//for PdsgNew
	};

/***************************************************************************
	document scroll window splitter - must be a child of a DSG
***************************************************************************/
typedef class DSSP *PDSSP;
#define DSSP_PAR GOB
#define kclsDSSP 'DSSP'
class DSSP : public DSSP_PAR
	{
	RTCLASS_DEC

protected:
	DSSP(PGCB pgcb);

public:
	static long DypNormal(void)
		{ return SCB::DypNormal() / 2; }
	static long DxpNormal(void)
		{ return SCB::DxpNormal() / 2; }
	static PDSSP PdsspNew(PDSG pdsg, ulong grfdssp);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual void MouseDown(long xp, long yp, long cact, ulong grfcust);
	};

enum
	{
	fdsspNil = 0,
	fdsspVert = 1,
	fdsspHorz = 2
	};


/***************************************************************************
	document scroll split mover - must be a child of a DSG
***************************************************************************/
typedef class DSSM *PDSSM;
#define DSSM_PAR GOB
#define kclsDSSM 'DSSM'
class DSSM : public DSSM_PAR
	{
	RTCLASS_DEC

private:
	bool _fVert;

protected:
	DSSM(PGCB pgcb);

	void _DrawTrackBar(PGNV pgnv, RC *prcOld, RC *prcNew);

public:
	static PDSSM PdssmNew(PDSG pdsg);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual void MouseDown(long xp, long yp, long cact, ulong grfcust);
	bool TVert(void);
	};

#endif //!DOCB_H

