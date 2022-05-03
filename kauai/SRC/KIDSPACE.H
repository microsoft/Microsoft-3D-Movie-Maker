/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Kidspace class declarations

***************************************************************************/
#ifndef KIDSPACE_H
#define KIDSPACE_H


#define ChidFromSnoDchid(sno, dchid) LwHighLow((short)(sno), (short)(dchid))


/***************************************************************************
	Graphical object representation.  A bitmap, fill, tiled bitmap, etc.
***************************************************************************/
typedef class GORP *PGORP;
#define GORP_PAR BASE
#define kclsGORP 'GORP'
class GORP : public GORP_PAR
	{
	RTCLASS_DEC

protected:
	GORP(void) {}

public:
	virtual void Draw(PGNV pgnv, RC *prcClip) = 0;
	virtual bool FPtIn(long xp, long yp) = 0;
	virtual void SetDxpDyp(long dxpPref, long dypPref) = 0;
	virtual void GetRc(RC *prc) = 0;
	virtual void GetRcContent(RC *prc) = 0;

	virtual long NfrMac(void);
	virtual long NfrCur(void);
	virtual void GotoNfr(long nfr);
	virtual bool FPlaying(void);
	virtual bool FPlay(void);
	virtual void Stop(void);
	virtual void Suspend(void);
	virtual void Resume(void);
	virtual void Stream(bool fStream);
	};


/***************************************************************************
	Graphical object fill representation.
***************************************************************************/
typedef class GORF *PGORF;
#define GORF_PAR GORP
#define kclsGORF 'GORF'
class GORF : public GORF_PAR
	{
	RTCLASS_DEC

protected:
	ACR _acrFore;
	ACR _acrBack;
	APT _apt;
	RC _rc;
	long _dxp;
	long _dyp;

public:
	static PGORF PgorfNew(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FPtIn(long xp, long yp);
	virtual void SetDxpDyp(long dxpPref, long dypPref);
	virtual void GetRc(RC *prc);
	virtual void GetRcContent(RC *prc);
	};


/***************************************************************************
	Graphical object bitmap representation.
***************************************************************************/
typedef class GORB *PGORB;
#define GORB_PAR GORP
#define kclsGORB 'GORB'
class GORB : public GORB_PAR
	{
	RTCLASS_DEC

protected:
	PCRF _pcrf;
	CTG _ctg;
	CNO _cno;
	bool _fStream;

	~GORB(void);

public:
	static PGORB PgorbNew(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FPtIn(long xp, long yp);
	virtual void SetDxpDyp(long dxpPref, long dypPref);
	virtual void GetRc(RC *prc);
	virtual void GetRcContent(RC *prc);
	virtual void Stream(bool fStream);
	};


/***************************************************************************
	Graphical object tiled bitmap representation.
***************************************************************************/
enum
	{
	idzpLeftBorder,
	idzpRightBorder,
	idzpLeft,
	idzpLeftFlex,
	idzpLeftInc,
	idzpMid,
	idzpRightFlex,
	idzpRightInc,
	idzpRight,
	idzpLimGort
	};

typedef class GORT *PGORT;
#define GORT_PAR GORP
#define kclsGORT 'GORT'
class GORT : public GORT_PAR
	{
	RTCLASS_DEC

protected:
	//a TILE chunk
	struct GOTIL
		{
		short bo;
		short osk;
		short rgdxp[idzpLimGort];
		short rgdyp[idzpLimGort];
		};

	PCRF _pcrf;
	CTG _ctg;
	CNO _cno;
	short _rgdxp[idzpLimGort];
	short _rgdyp[idzpLimGort];

	long _dxpLeftFlex;
	long _dxpRightFlex;
	long _dypLeftFlex;
	long _dypRightFlex;
	long _dxp;	// the total width
	long _dyp;	// the total height

	bool _fStream;

	~GORT(void);
	void _DrawRow(PGNV pgnv, PMBMP pmbmp, RC *prcRow, RC *prcClip,
		long dxp, long dyp);
	void _ComputeFlexZp(long *pdzpLeftFlex, long *pdzpRightFlex, long dzp,
		short *prgdzp);
	void _MapZpToMbmp(long *pzp, short *prgdzp,
		long dzpLeftFlex, long dzpRightFlex);
	void _MapZpFlex(long *pzp, short *prgdzp,
		long dzpLeftFlex, long dzpRightFlex);

public:
	static PGORT PgortNew(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FPtIn(long xp, long yp);
	virtual void SetDxpDyp(long dxpPref, long dypPref);
	virtual void GetRc(RC *prc);
	virtual void GetRcContent(RC *prc);
	virtual void Stream(bool fStream);
	};


/***************************************************************************
	Graphical object video representation.
***************************************************************************/
typedef class GORV *PGORV;
#define GORV_PAR GORP
#define kclsGORV 'GORV'
class GORV : public GORV_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PGVID _pgvid;
	long _dxp;
	long _dyp;
	long _cactSuspend;
	bool _fHwndBased: 1;
	bool _fPlayOnResume: 1;

	~GORV(void);

	virtual bool _FInit(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno);

public:
	static PGORV PgorvNew(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FPtIn(long xp, long yp);
	virtual void SetDxpDyp(long dxpPref, long dypPref);
	virtual void GetRc(RC *prc);
	virtual void GetRcContent(RC *prc);

	virtual long NfrMac(void);
	virtual long NfrCur(void);
	virtual void GotoNfr(long nfr);
	virtual bool FPlaying(void);
	virtual bool FPlay(void);
	virtual void Stop(void);
	virtual void Suspend(void);
	virtual void Resume(void);
	};


/***************************************************************************
	These are the mouse tracking states for kidspace gobs. gmsNil is an
	illegal state used in the tables to signal that we should assert.

	In the lists below, tracking indicates that when in the state, we are
	tracking the mouse. These are static states:

		kgmsIdle (mouse is not over us, _not_ tracking)
		kgmsOn (mouse is over us, _not_ tracking)
		kgmsDownOn (mouse is over us, tracking)
		kgmsDownOff (mouse isn't over us, tracking)

	These are auto-advance states (only animations are allowed):

		kgmsEnterState (entering the state, _not_ tracking)
		kgmsRollOn (mouse rolled onto us, _not_ tracking)
		kgmsRollOff (mouse rolled off of us, _not_ tracking)
		kgmsPressOn (mouse down on us, tracking)
		kgmsReleaseOn (mouse up on us, tracking)
		kgmsDragOff (mouse rolled off us, tracking)
		kgmsDragOn (mouse rolled on us, tracking)
		kgmsReleaseOff (mouse was released off us, _not_ tracking)

	There is one special state: kgmsWait (tracking). This indicates that
	we've completed a kgmsReleaseOn and are waiting for a cidClicked command
	or a cidTrackMouse command to come through. If the former comes through,
	we handle the click, stop tracking and advance to kgmsOn. If the latter,
	the cidClicked was filtered out by someone else, so we stop tracking and
	goto kgmsOn.

***************************************************************************/
enum
	{
	gmsNil,

	// non-tracking states
	kgmsEnterState,
	kgmsIdle,
	kgmsRollOn,
	kgmsRollOff,
	kgmsOn,
	kgmsReleaseOff,

	// tracking states
	kgmsMinTrack,
	kgmsPressOn = kgmsMinTrack,
	kgmsReleaseOn,
	kgmsDownOn,
	kgmsDragOff,
	kgmsDragOn,
	kgmsDownOff,
	kgmsWait,
	kgmsLimTrack,

	kgmsLim = kgmsLimTrack
	};


/***************************************************************************
	Graphic Object in Kidspace.  Because of script invocation, the GOK
	may be destroyed in just about every method of the GOK.  So most methods
	return a boolean indicating whether the GOK still exists.
***************************************************************************/
enum
	{
	fgokNil = 0,
	fgokKillAnim = 1,	// kill current animation
	fgokNoAnim = 2,		// don't launch an animation
	fgokReset = 4,		// if the current rep is requested, totally reset it
	fgokMouseSound = 8,	// set the mouse sound as well
	};

typedef class GOK *PGOK;
#define GOK_PAR GOB
#define kclsGOK 'GOK'
class GOK : public GOK_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	CMD_MAP_DEC(GOK)

protected:
	long _dxp;			// offset from top-left to the registration point
	long _dyp;
	long _zp;			// z-coord (for placing GOK's relative to this one)

	long _dxpPref;		// preferred size (if non-zero)
	long _dypPref;

	PWOKS _pwoks;		// the kidspace world that this GOK belongs to
	PRCA _prca;			// Chunky resource chain
	PCRF _pcrf;			// Chunky resource file

	short _sno;			// state number
	short _cactMouse;	// mouse click count of last mouse down
	ulong _grfcust;		// cursor state at last mouse down
	long _gmsCur;		// gob mouse tracking state

	bool _fRect: 1;		// whether to use rectangular hit testing exclusively
	bool _fNoHit: 1;	// invisible to the mouse
	bool _fNoHitKids: 1;// children of this GOK are invisible to the mouse
	bool _fNoSlip: 1;	// animations shouldn't slip
	bool _fGorpDirty: 1;// whether the GORP changed while deferred
	bool _fMouseSndDirty: 1;	// whether playing the mouse sound was deferred
	bool _fStream: 1;	// once we switch reps, we won't use this one again

	long _cactDeferGorp;// defer marking and positioning the gorp
	PGORP _pgorp;		// the graphical representation
	CKI _ckiGorp;		// cki of the current gorp

	long _dtim;			// current time increment for animation
	PSCEG _pscegAnim;	// animation script
	CHID _chidAnim;		// chid of current animation

	PGOKD _pgokd;

	long _siiSound;		// sound to kill when we go away
	long _siiMouse;		// mouse tracking sound - kill it when we go away
	CKI _ckiMouseSnd;	// for deferred playing of the mouse sound
	long _cactDeferSnd;	// defer starting the mouse sound if this is > 0

	// cid/hid filtering
	struct CMFLT
		{
		long cid;
		long hid;
		CHID chidScript;
		};
	PGL _pglcmflt;			// list of cmd filtering structs, sorted by cid

	long _hidToolTipSrc;	// get the tool tip info from this GOK

	GOK(GCB *pgcb);
	~GOK(void);

	static PGOB _PgobBefore(PGOB pgobPar, long zp);

	virtual bool _FInit(PWOKS pwoks, PGOKD pgokd, PRCA prca);
	virtual bool _FInit(PWOKS pwoks, CNO cno, PRCA prca);

	virtual bool _FAdjustGms(struct GMSE *pmpgmsgmse);
	virtual bool _FSetGmsCore(long gms, ulong grfact, bool *pfStable);
	virtual bool _FSetGms(long gms, ulong grfact);

	virtual bool _FEnterState(long sno);
	virtual bool _FSetRep(CHID chid, ulong grfgok = fgokKillAnim,
		CTG ctg = ctgNil, long dxp = 0, long dyp = 0, bool *pfSet = pvNil);
	virtual bool _FAdvanceFrame(void);

	virtual void _SetGorp(PGORP pgorp, long dxp, long dyp);
	virtual PGORP _PgorpNew(PCRF pcrf, CTG ctg, CNO cno);

	bool _FFindCmflt(long cid, long hid, CMFLT *pcmflt = pvNil,
		long *picmflt = pvNil);
	bool _FFilterCmd(PCMD pcmd, CHID chidScript, bool *pfFilter);
	void _PlayMouseSound(CHID chid);
	CNO _CnoToolTip(void);
	CHID _ChidMouse(void);
	void _DeferGorp(bool fDefer);
	void _DeferSnd(bool fDefer);

public:
	static PGOK PgokNew(PWOKS pwoks, PGOB pgobPar, long hid,
		PGOKD pgokd, PRCA prca);

	PWOKS Pwoks(void)
		{ return _pwoks; }
	long Sno(void)
		{ return _sno; }
	void GetPtReg(PT *ppt, long coo = cooParent);
	void GetRcContent(RC *prc);
	long ZPlane(void)
		{ return _zp; }
	void SetZPlane(long zp);
	void SetNoSlip(bool fNoSlip);
	void SetHidToolTip(long hidSrc);

	virtual void SetCursor(ulong grfcust);
	virtual bool FPtIn(long xp, long yp);
	virtual bool FPtInBounds(long xp, long yp);
	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FCmdTrackMouse(PCMD_MOUSE pcmd);
	virtual bool FCmdAlarm(PCMD pcmd);
	virtual bool FCmdMouseMove(PCMD_MOUSE pcmd);
	virtual bool FCmdClicked(PCMD_MOUSE pcmd);
	bool FCmdClickedCore(PCMD pcmd)
		{ return FCmdClicked((PCMD_MOUSE)pcmd); }
	virtual bool FCmdAll(PCMD pcmd);
	virtual bool FFilterCidHid(long cid, long hid, CHID chidScript);

	virtual bool FEnsureToolTip(PGOB *ppgobCurTip, long xpMouse, long ypMouse);
	virtual long LwState(void);

	virtual bool FRunScript(CHID chid, long *prglw = pvNil, long clw = 0,
		long *plwReturn = pvNil, bool *ptSuccess = pvNil);
	virtual bool FRunScriptCno(CNO cno, long *prglw = pvNil, long clw = 0,
		long *plwReturn = pvNil, bool *ptSuccess = pvNil);
	virtual bool FChangeState(long sno);
	virtual bool FSetRep(CHID chid, ulong grfgok = fgokKillAnim,
		CTG ctg = ctgNil, long dxp = 0, long dyp = 0, ulong dtim = 0);

	virtual bool FPlay(void);
	virtual bool FPlaying(void);
	virtual void Stop(void);
	virtual void GotoNfr(long nfr);
	virtual long NfrMac(void);
	virtual long NfrCur(void);

	virtual long SiiPlaySound(CTG ctg, CNO cno, long sqn, long vlm,
		long cactPlay, ulong dtsStart, long spr, long scl);
	virtual long SiiPlayMouseSound(CTG ctg, CNO cno);

	virtual void Suspend(void);
	virtual void Resume(void);
	virtual void Stream(bool fStream);
	};

#endif //!KIDSPACE_H

