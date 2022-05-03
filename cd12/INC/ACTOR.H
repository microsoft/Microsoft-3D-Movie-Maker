/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/*******************************************************************

 Actor class

 Primary Author : ******
 Review Status: Reviewed

 BASE -------> ACTR
																														
 Each actor has a unique route which is defined by one or more nodes,
 and which may be a concatenation of one or more subroutes.
 Each actor also has an event list describing events which happen
 at a specified time and point along the route.  A point may coincide
 with a node or lie between nodes.

 *******************************************************************/
#ifndef ACTOR_H
#define ACTOR_H


//
//	XYZ : A point in x,y,z along an actor's RouTE.
//
struct XYZ
    {
 	BRS dxr;
	BRS dyr;
	BRS dzr;

	bool operator==(XYZ &xyz)
		{ return ((dxr == xyz.dxr) &&
					(dyr == xyz.dyr) &&
					(dzr == xyz.dzr)); }
	bool operator!=(XYZ &xyz)
		{ return ((dxr != xyz.dxr) ||
					(dyr != xyz.dyr) ||
					(dzr != xyz.dzr)); }
    };

typedef XYZ * PXYZ;

const BOM kbomXyz = 0xfc000000;

//
// 	A RouTE is a general list (GL) of Route PoinTs(RPT)
//  A Subroute is a contiguous section of a route.
//
struct RPT
	{
	XYZ xyz;
	BRS dwr;	//Distance from this node to the next node on the route
	};
const BOM kbomRpt = 0xff000000;

const long knfrmInvalid = klwMax;		// invalid frame state.  Regenerate correct state
const long kcrptGrow = 32;				// quantum growth for rpt
const long kcsmmGrow = 2;				// quantum growth for smm
const long kctagSndGrow = 2;			// quantum growth for tagSnd
const long smmNil = -1; 				// Not motion match sound
const BRS kdwrNil = BR_SCALAR(-1.0);	// flags use of template cel stepsize
const BRS kzrDefault = BR_SCALAR(-25.0);	// initial default z position
const BRS kdwrMax = BR_SCALAR(32767.0);	// large BRS value
const ulong kdtsThreshRte = (kdtsSecond >> 2) + (kdtsSecond >> 1);	// time threshhold before record in place
const BRS kdwrThreshRte = BR_SCALAR(2.0);	// distance threshhold before entering record mode
const long kcaevInit = 10;
const BRS kdwrFast = BR_SCALAR(3.0);	// delta world coord change for fast mouse move
const BRS krOriWeightMin = BR_SCALAR(0.1);		// orientation weighting for slow mouse move
const BRS krAngleMin = BR_SCALAR(0.1);	// Min angle impacting amount of forward movement on placement
const BRS krAngleMinRcp = BR_RCP(krAngleMin);	
const BRS krAngleMax = BR_SCALAR(0.4);	// Max angle impacting amount of forward movement on placement
const BRS krScaleMinFactor = (BR_SCALAR(0.1));	// Min scaling between Brender update
const krScaleMaxFactor = BrsDiv(rOne, krScaleMinFactor);	// Max scaling between Brender update
const krScaleMin = (BR_SCALAR(0.25));	// Min scaling 
const krScaleMax = (BR_SCALAR(10.0));	// Max scaling 
const krPullMin = krScaleMin;
const krPullMax = BrsDiv(rOne, krPullMin);

#define aridNil	ivNil

// Angle valid flags
enum
	{
	fbraRotateX = factnRotateX,
	fbraRotateY = factnRotateY,
	fbraRotateZ = factnRotateZ
	};

// Normalize flags
enum
	{
	fnormSize = 1,
	fnormRotate = 2
	};
		
// Mouse Actor flags
enum
	{
	fmafNil = 0,
	fmafFreeze = 0x01,			// freeze cels
	fmafGround = 0x02,			// respect ground level
	fmafOrient = 0x04,			// orient actor during move
	fmafEntireScene = 0x08,		// position over entire scene, vs subroute only
	fmafEntireSubrte = 0x10		// position over entire subroute
	};

struct RTEL 		// RouTE Location - a function of space and time
	{
	int irpt;		// The preceding node for the given point
	BRS dwrOffset;	// Absolute linear distance beyond node irpt
	long dnfrm; 	// Delta frame number (ie, time) at this point

	bool operator==(RTEL &rtel)
		{ return (irpt == rtel.irpt &&
			dwrOffset == rtel.dwrOffset &&
			dnfrm == rtel.dnfrm); }

	bool operator!=(RTEL &rtel)
		{ return (irpt != rtel.irpt ||
			dwrOffset != rtel.dwrOffset ||
			dnfrm != rtel.dnfrm); }

	bool operator<=(RTEL &rtel)
		{ return (irpt < rtel.irpt ||
			(irpt == rtel.irpt && (dwrOffset < rtel.dwrOffset ||
			(dwrOffset == rtel.dwrOffset && dnfrm <= rtel.dnfrm)))); }

	bool operator>=(RTEL &rtel)
		{ return (irpt > rtel.irpt ||
			(irpt == rtel.irpt && (dwrOffset > rtel.dwrOffset ||
			(dwrOffset == rtel.dwrOffset && dnfrm >= rtel.dnfrm)))); }

	bool operator<(RTEL &rtel)
		{ return (irpt < rtel.irpt ||
			(irpt == rtel.irpt && (dwrOffset < rtel.dwrOffset ||
			(dwrOffset == rtel.dwrOffset && dnfrm < rtel.dnfrm)))); }

	bool operator>(RTEL &rtel)
		{ return (irpt > rtel.irpt ||
			(irpt == rtel.irpt && (dwrOffset > rtel.dwrOffset ||
			(dwrOffset == rtel.dwrOffset && dnfrm > rtel.dnfrm)))); }
	};

// Actor EVents are stored in a GG (general group)
// Fixed part of the GG:
struct AEV
	{
	long aet;			//Actor Event Type
	long nfrm;			//Absolute frame number (* Only valid < current event)
	RTEL rtel;			//RouTE Location for this event
	};					//Additional event parameters (in the GG)
typedef AEV *PAEV;
	
//
//	Actor level Event Types which live in a GG.
//	The fixed part of a GG entry is an actor event (aev)
//	The variable part is documented in the following comments
//
enum AET
	{
	aetAdd,			// Add Actor onstage: aevadd - internal (no api)
	aetActn, 		// Animate Actor : aevactn
	aetCost, 		// Set Costume : aevcost
	aetRotF,		// Transform Actor rotate: BMAT34
	aetPull,		// Transform Actor Pull : aevpull
	aetSize,		// Transform Actor size uniformly : BRS
	aetSnd, 		// Play a sound : aevsnd
	aetMove,		// Translate the path at this point : XYZ
	aetFreeze,		// Freeze (or Un) Actor : long
	aetTweak,		// Path tweak : XYZ
	aetStep,		// Force step size (eg, float, wait) : BRS
	aetRem, 		// Remove an actor from the stage : nil
	aetRotH,		// Single frame rotation : BMAT34
	aetLim
	};

const BOM kbomAet = 0xc0000000;
const BOM kbomAev = 0xff000000;

//
//	Variable part of the Actor EVent GG:
//
struct AEVPULL			// Squash/stretch
	{
	BRS rScaleX;
	BRS rScaleY;
	BRS rScaleZ;
	};
const BOM kbomAevpull = 0xfc000000;

// Every subroute is normalized.  The normalization translation is
// stored in the Add Event
// ** nfrmPrev valid for nfrmSub <= _nfrmCur only (optimization)	
struct AEVADD
	{
	BRS dxr;		// Translation in x for this subroute
	BRS dyr;		// Translation in y for this subroute
	BRS dzr;		// Translation in z for this subroute
	BRA xa;			// Single point orientation
	BRA ya;			// Single point orientation
	BRA za;			// Single point orientation
	};
const BOM kbomAevadd = 0xffc00000 | kbomBmat34>>10;

struct AEVACTN
	{
	long anid;
	long celn;			// starting cel of action
	};
const BOM kbomAevactn = 0xf0000000;

struct AEVCOST
	{
	long ibset;			// body part set
	long cmid;			// costume ID (for custom costumes)
	bool fCmtl; 		// vs fMtrl
	TAG tag;
	};
const BOM kbomAevcost = 0xfc000000 | (kbomTag >> 6);

struct AEVSND
	{
	bool fLoop;			// loop count
	bool fQueue;		// queued sound
	long vlm;			// volume
	long celn;			// motion match	: ivNil if not
	long sty;			// sound type
	bool fNoSound;		// no sound
	CHID chid;			// user sound requires chid
	TAG tag;
	};
const BOM kbomAevsnd = 0xfff00000 | (kbomTag >> 12);

const BOM kbomAevsize = 0xc0000000;
const BOM kbomAevfreeze = 0xc0000000;
const BOM kbomAevstep = 0xc0000000;
const BOM kbomAevmove = kbomXyz;
const BOM kbomAevtweak = kbomXyz;
const BOM kbomAevrot = kbomBmat34;

// Separate ggaev variable portion sizes
#define kcbVarAdd (size(AEVADD))
#define kcbVarActn (size(AEVACTN))
#define kcbVarCost (size(AEVCOST))
#define kcbVarRot	(size(BMAT34))
#define kcbVarSize (size(BRS))
#define kcbVarPull (size(AEVPULL))
#define	kcbVarSnd	(size(AEVSND))
#define kcbVarFreeze (size(long))
#define kcbVarMove (size(XYZ))
#define kcbVarTweak (size(XYZ))
#define kcbVarStep (size(BRS))
#define kcbVarZero (0)

//Actor Event Flags
enum
	{
	faetNil = 0,
	faetAdd = 1 << aetAdd,
	faetActn = 1 << aetActn,
	faetCost = 1 << aetCost,
	faetRotF = 1 << aetRotF,
	faetPull = 1 << aetPull,
	faetSize = 1 << aetSize,
	faetFreeze = 1 << aetFreeze,
	faetTweak = 1 << aetTweak,
	faetStep = 1 << aetStep,
	faetRem = 1 << aetRem,
	faetMove = 1 << aetMove,
	faetRotH = 1 << aetRotH
	};

//
// Because rotations are non-abelian, cumulative rotations are stored
// in a BMAT34.  It would not be correct to multiply (xa,ya,za) values
// treating x,y,z independently.
//
// Bmat34Fwd does NOT include the path portion of the orientation.
// Bmat34Cur includes all current angular rotation - path rotation is NOT post 
// applied to this.
//
// Careful: The task is complicated by the fact that users can apply single 
// frame and frame forward orientations in the same frame and must not see the 
// actor jump in angle when choosing between the two methods of editing.
//
struct XFRM
	{
	BMAT34 bmat34Fwd;	// Rotation	fwd	: path rotation post applied to this
	AEVPULL aevpull;	// Stretching (pulling) constants
	BRS rScaleStep;		// Uniform scaling to be applied to step size
	BMAT34 bmat34Cur;	// <<Path independent>> Single frame & static segment rotation
	BRA xaPath;			// Path portion of the current frame's rotation
	BRA yaPath;			// Path portion of the current frame's rotation
	BRA zaPath;			// Path portion of the current frame's rotation
	};

//
// Current action Motion Match Sounds
// Aev & Aevsnd grouped together form a gl
// Not to be saved with a movie
//
struct SMM
	{
	AEV aev;			// event for the sound
	AEVSND aevsnd;
	};


//
// Default hilite colors
//
#define kiclrNormalHilite		108
#define kiclrTimeFreezeHilite	43

/***********************************************
   Actor Class
***********************************************/
typedef class ACTR *PACTR;
#define ACTR_PAR BASE
#define kclsACTR 'ACTR'
class ACTR :  public ACTR_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
		
	// Components of an actor
	// Note: In addition to these components, any complete actor must
	// have either fLifeDirty set or _nfrmLast current.
	// Note: _tagTmpl cannot be derived from _ptmpl
	PGG _pggaev;			//GG pointer to Actor EVent list
	PGL _pglrpt;			//GL pointer to actor's route
	TMPL *_ptmpl;			//Actor body & action list template
	BODY *_pbody;			//Actor's body
	TAG _tagTmpl;			//Note: The sid cannot be queried at save time
	TAG _tagSnd;			//Sound (played on entrance)
	SCEN *_pscen;			//Underlying scene
	XYZ _dxyzFullRte; 		//Origin of the route
	long _nfrmFirst;		//klwMax -or- First frame : Set	when event created
	long _arid; 			//Unique id assigned to this actor.
	ulong _grfactn;			//Cached current grfactn

	// Frame Dependent State Information
	XYZ _dxyzRte;			//_dxyzFullRte + _dxyzSubRte : Set when Add processed
	XYZ _dxyzSubRte;		//Subpath translation : Set when Add processed
	bool _fOnStage : 1;		//Versus Brender hidden.  Set by Add, Rem only
	bool _fFrozen : 1;		//Path offset > 0 but not moving
	bool _fLifeDirty : 1;	//Set if _nfrmLast requires recomputation
	bool _fPrerendered : 1; //Set if actor is prerendered
	bool _fUseBmat34Cur : 1;//_xfrm.bmat34Cur last used in orienting actor
	BRS _dwrStep;			//Current step size in use
	long _anidCur;			//Current action in template
	long _ccelCur;			//Cached cel count.  Retrieving this was hi profile.
	long _celnCur;			//Current cell
	long _nfrmCur;			//Current frame number : Set by FGotoFrame
	long _nfrmLast; 		//Latest known frame for this actor.
	long _iaevCur;			//Current event in event list
	long _iaevFrmMin;		//First event in current frame
	long _iaevActnCur;		//Event defining current action
	long _iaevAddCur;		//Most recent add (useful for Compose)
	RTEL _rtelCur;			//Current location on route	(excludes tweak info)
	XYZ _xyzCur;			//Last point displayed (may be tweak modified)
	XFRM _xfrm;				//Current transformation
	PGL _pglsmm;			//Current action motion match sounds

	// Path Recording State Information
	RTEL _rtelInsert;		//Joining information
	ulong _tsInsert;		//Starting time of route recording
	bool _fModeRecord : 1;	//Record a route mode
	bool _fRejoin : 1;		//Rerecord is extending a subpath from the end
	bool _fPathInserted : 1;	//More path inserted
	bool _fTimeFrozen: 1;	//Is the actor frozen wrt time?
	long _dnfrmGap;			//Frames between subroutes
	XYZ _dxyzRaw;			//Raw mouse movement from previous frame
	
//
//	Protected functions
//
	ACTR(void);
	bool _FInit(TAG *ptmplTag);             //Constructor allocation & file I/O
	void _InitXfrmRot(BMAT34 *pbmat34);     //Initialize rotation only
	void _InitXfrm(void);                   //Initialize rotation & scaling
	bool _FCreateGroups(void);
	void _InitState(void);
	void _GetNewOrigin(BRS *pxr, BRS *pyr, BRS *pzr);
	void _SetStateRewound(void);

	bool _FQuickBackupToFrm(long nfrm, bool *pfQuickMethodValid);
	bool _FGetRtelBack(RTEL *prtel, bool fUpdateStateVar);
	bool _FDoFrm(bool fPositionBody, bool *pfPositionDirty, bool *pfSoundInFrame = pvNil);
	bool _FGetStatic(long anid, bool *pfStatic);
	bool _FIsDoneAevSub(long iaev, RTEL rtel);
	bool _FIsAddNow(long iaev);
	bool _FGetDwrPlay(BRS *pdwr);			//Step size if playing
	bool _FGetDwrRecord(BRS *pdwr);			//Step size if recording
	bool _FDoAevCur(void);
	bool _FDoAevCore(long iaev);
	bool _FDoAetVar(long aet, void *pvVar, long cbVar);

	bool _FEnqueueSnd(long iaev);
	bool _FEnqueueSmmInMsq(void);
	bool _FInsertSmm(long iaev);
	bool _FRemoveAevMm(long anid);
	bool _FAddAevDefMm(long anid);

	bool _FAddDoAev(long aetNew, long kcbNew, void *pvVar);
	void _MergeAev(long iaevFirst, long iaevLast, long *piaevNew = pvNil);
	bool _FFreeze(void);					//insert freeze event
	bool _FUnfreeze(void);					//insert unfreeze event
	void _Hide(void);
	bool _FInsertGgRpt(long irpt, RPT *prpt, BRS dwrPrior = rZero);
	bool _FAddAevFromPrev(long iaevLim, ulong grfaet);
	bool _FAddAevFromLater(void);
	bool _FFindNextAevAet(long aet, long iaevCur, long *piaevAdd);
	bool _FFindPrevAevAet(long aet, long iaevCur, long *piaevAdd);
	void _FindAevLastSub(long iaevAdd, long iaevLim, long *piaevLast);
	void _DeleteFwdCore(bool fDeleteAll, bool *pfAlive = pvNil, long iaevCur = ivNil);
	bool _FDeleteEntireSubrte(void);
	void _DelAddFrame(long iaevAdd, long iaevLim);

	void _UpdateXyzRte(void);
	bool _FInsertAev(long iaev, long cbNew, void *pvVar, void *paev, bool fUpdateState = fTrue);
	void _RemoveAev(long iaev, bool fUpdateState = fTrue);
	void _PrepXfrmFill(long aet, void *pvVar,long cbVar,
		long iaevMin, long iaevCmp = ivNil, ulong grfaet = faetNil);
	void _PrepActnFill(long iaevMin, long anidPrev, long anidNew, ulong grfaet);
	void _PrepCostFill(long iaevMin, AEVCOST *paevcost);
	void _AdjustAevForRteIns(long irptAdjust, long iaevMin);															
	void _AdjustAevForRteDel(long irptAdjust, long iaevMin);
	bool _FInsertStop(void);
	void _CalcRteOrient(BMAT34 *pbmat34,
		BRA *pxa = pvNil, BRA *pya = pvNil, BRA *pza = pvNil, ulong *pgrfbra = pvNil);
	void _ApplyRotFromVec(XYZ *pxyz, BMAT34 *pbmat34,
		BRA *pxa = pvNil, BRA *pya = pvNil, BRA *pza = pvNil, ulong *grfbra = pvNil);
	void _SaveCurPathOrien(void);
	void _LoadAddOrien(AEVADD *paevadd, bool fNoReset = fFalse);
	BRA  _BraAvgAngle(BRA a1, BRA a2, BRS rw);
	void _UpdateXyzTan(XYZ *pxyz, long irptTan, long rw) ;

	void _AdvanceRtel(BRS dwrStep, RTEL *prtel, long iaevCur,
		long nfrmCur, bool *pfEndRoute);
	void _GetXyzFromRtel(RTEL *prtel, PXYZ pxyz);
	void _GetXyzOnLine(PXYZ pxyzFirst, PXYZ pxyzSecond, BRS dwrOffset, PXYZ pxyz);
	void _PositionBody(PXYZ pxyz);
	void _MatrixRotUpdate(XYZ *pxyz, BMAT34 *pbmat34);
	void _TruncateSubRte(long irptDelLim);
	bool _FComputeLifetime(long *pnfrmLast = pvNil);
	bool _FIsStalled(long iaevFirst, RTEL *prtel, long *piaevLast = pvNil);

	void _RestoreFromUndo(PACTR pactrRestore);
	bool _FDupCopy(PACTR pactrSrc, PACTR pactrDest);

	bool _FWriteTmpl(PCFL pcfl, CNO cno);
	bool _FReadActor(PCFL pcfl, CNO cno);
	bool _FReadRoute(PCFL pcfl, CNO cno);
	bool _FReadEvents(PCFL pcfl, CNO cno);
	static void _SwapBytesPggaev(PGG pggaev);
	bool _FOpenTags(PCRF pcrf);
	static bool _FIsIaevTag(PGG pggaev, long iaev, PTAG *pptag, PAEV *pqaev = pvNil);
	void _CloseTags(void);

public:
	~ACTR(void);
	static PACTR PactrNew(TAG *ptagTmpl);			
	void SetPscen(SCEN *pscen);
	void SetArid(long arid) { AssertBaseThis(0); _arid = arid; }
	void SetLifeDirty(void) { AssertBaseThis(0); _fLifeDirty = fTrue; }
	PSCEN Pscen(void) { AssertThis(0); return _pscen; }

	// ActrSave Routines
	static PACTR PactrRead(PCRF pcrf, CNO cno);		//Construct from a document
	bool FWrite(PCFL pcfl, CNO cno, CNO cnoScene);	//Write to a document
	static PGL PgltagFetch(PCFL pcfl, CNO cno, bool *pfError);
	static bool FAdjustAridOnFile(PCFL pcfl, CNO cno, long darid);

	// Visibility
	void Hilite(void);
	void Unhilite(void) { AssertBaseThis(0); _pbody->Unhilite(); }
	void Hide(void) { AssertBaseThis(0); _pbody->Hide(); }
	void Show(void) { AssertBaseThis(0); _pbody->Show(); }
	bool FIsInView(void) { AssertBaseThis(0); return _pbody->FIsInView(); }
	void GetCenter(long *pxp, long *pyp) { AssertBaseThis(0); _pbody->GetCenter(pxp, pyp); }
	void GetRcBounds(RC *prc) { AssertBaseThis(0); _pbody->GetRcBounds(prc); }
	void SetPrerendered(bool fPrerendered) { AssertBaseThis(0); _fPrerendered = fPrerendered; }
	bool FPrerendered(void) { AssertBaseThis(0); return _fPrerendered; }

	// Actor Information
	long Arid(void) { AssertBaseThis(0); return (_arid); }
	bool FOnStage(void) { AssertBaseThis(0); return (_fOnStage); }
	bool FIsMyBody(BODY *pbody) { AssertBaseThis(0); return pbody == _pbody;}
	bool FIsMyTmpl(TMPL *ptmpl) { AssertBaseThis(0); return _ptmpl == ptmpl; }
	bool FIsModeRecord(void) { AssertBaseThis(0); return FPure(_fModeRecord); }
	bool FIsRecordValid(BRS dxr, BRS dyr, BRS dzr, ulong tsCurrent);
	PTMPL Ptmpl(void) {AssertBaseThis(0); return _ptmpl; }
	PBODY Pbody(void) { AssertBaseThis(0); return _pbody; }
	long AnidCur(void) { AssertBaseThis(0); return _anidCur; }
	long CelnCur(void) { AssertBaseThis(0); return _celnCur; }
	bool FFrozen(void) { AssertBaseThis(0); return _fFrozen; }
	bool FGetLifetime(long *pnfrmFirst, long *pnfrmLast);	//allows nil ptrs
	bool FPtIn(long xp, long yp, long *pibset);
	void GetTagTmpl(PTAG ptag) { AssertBaseThis(0); *ptag = _tagTmpl; }
	void GetName(PSTN pstn);
	bool FChangeTagTmpl(PTAG ptagTmplNew);
	bool FTimeFrozen(void) { AssertBaseThis(0); return _fTimeFrozen; }
	void SetTimeFreeze(bool fTimeFrozen) { AssertBaseThis(0); _fTimeFrozen = fTimeFrozen; }
	bool FIsPropBrws(void) { AssertThis(0); return FPure(_ptmpl->FIsProp() || _ptmpl->FIsTdt()); }
	bool FIsTdt(void) { AssertThis(0); return FPure(_ptmpl->FIsTdt()); }
	bool FMustRender(long nfrmRenderLast);
	void GetXyzWorld(BRS *pxr, BRS *pyr, BRS *pzr);

	// Animation
	bool FGotoFrame(long nfrm, bool *pfSoundInFrame = pvNil); //Prepare for display at frame nfrm
	bool FReplayFrame(long grfscen);	//Replay a frame.
		
	// Event Editing
	bool FAddOnStageCore(void);	
	bool FSetActionCore(long anid, long celn, bool fFreeze);
	bool FRemFromStageCore(void);		
	bool FSetCostumeCore(long ibsetClicked, TAG *ptag, long cmid, bool fCustom);
	bool FSetStep(BRS dwrStep);
	bool FRotate(BRA xa, BRA ya, BRA za, bool fFromHereFwd);
	bool FNormalizeCore(ulong grfnorm);				
	void SetAddOrient(BRA xa, BRA ya, BRA za, ulong grfbra, XYZ *pdxyz = pvNil);
	bool FScale(BRS rScaleStep);
	bool FPull(BRS rScaleX, BRS rScaleY, BRS rScaleZ);
	void DeleteFwdCore(bool fDeleteAll, bool *pfAlive = pvNil, long iaevCur = ivNil);
	void DeleteBackCore(bool *pfAlive = pvNil);
	bool FSoonerLater(long dnfrm);

	// ActrEdit Routines
	bool FDup(PACTR *ppactr, bool fReset = fFalse);	//Duplicate everything
	void Restore(PACTR pactr);

	bool FCreateUndo(PACTR pactr, bool fSndUndo = fFalse, PSTN pstn = pvNil);	//Create undo object
	void Reset(void);
	bool FAddOnStage(void);				//add actor to the stage, w/Undo
	bool FSetAction(long anid, long celn, bool fFreeze, PACTR *ppactrDup = pvNil);
	bool FSetCostume(long ibset, TAG *ptag, long cmid, bool fCustom);
	bool FRemFromStage(void);			//add event: rem actor from stage, w/Undo
	bool FCopy(PACTR *ppactr, bool fEntireScene = fFalse);	//Duplicate actor from this frame on
	bool FCopyRte(PACTR *ppactr, bool fEntireScene = fFalse);	//Duplicate path from this frame on
	bool FPasteRte(PACTR pactr);		//Paste from clipboard from this frame on
	bool FNormalize(ulong grfnorm);				
	bool FPaste(long nfrm, SCEN *pscen);
	bool FDelete(bool *pfAlive, bool fDeleteAll);
   	
   	// ActrSnd Routines
	bool FSetSnd(PTAG ptag, bool fLoop, bool fQueue, bool fActnCel, long vlm, long sty);
   	bool FSetSndCore(PTAG ptag, bool fLoop, bool fQueue, bool fActnCel, long vlm, long sty);
	bool FSetVlmSnd(long sty, bool fMotionMatch, long vlm);	// Set the volume of a sound
	bool FQuerySnd(long sty, bool fMotionMatch, PGL *pglTagSnd, long *pvlm,
		bool *pfLoop);
	bool FDeleteSndCore(long sty, bool fMotionMatch);
	bool FSoundInFrm(void);
	bool FResolveAllSndTags(CNO cnoScen);

	// Route Definition
	void SetTsInsert(ulong tsCurrent) { AssertBaseThis(0); _tsInsert = tsCurrent; }
	bool FBeginRecord(ulong tsCurrent, bool fReplace, PACTR pactrRestore);	
	bool FRecordMove(BRS dxr, BRS dyr, BRS dzr, ulong grfmaf,
			ulong tsCurrent, bool *pfLonger, bool *pfStep, PACTR pactrRestore);
	bool FEndRecord(bool fReplace, PACTR pactrRestore);				
	bool FTweakRoute(BRS dxr, BRS dyr, BRS dzr, ulong grfmaf = fmafNil);	
	bool FMoveRoute(BRS dxr, BRS dyr, BRS dzr, bool *pfMoved = pvNil,
		ulong grfmaf = fmafNil);

	};


//
// Actor document for clipping
//
typedef class ACLP *PACLP;
#define ACLP_PAR DOCB
#define kclsACLP 'ACLP'
class ACLP : public ACLP_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:
    PACTR _pactr;
	bool _fRteOnly;
	STN _stnName;

	ACLP(void) {}

public:

	//
	// Constructors and destructors
	//
	static PACLP PaclpNew(PACTR pactr, bool fRteOnly, bool fEndScene = fFalse);
	~ACLP(void);

	//
	// Pasting function
	//
	bool FPaste(PMVIE pmvie);

	bool FRouteOnly(void) { return _fRteOnly; }
	};

#endif //!ACTOR_H
