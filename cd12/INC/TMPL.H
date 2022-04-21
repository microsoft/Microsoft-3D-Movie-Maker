/*************************************************************************

	tmpl.h: Actor template class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	BASE ---> BACO ---> ACTN
	BASE ---> BACO ---> TMPL

	A TMPL encapsulates all the data that distinguishes one actor
	"species" from another, including the species' models, actions,
	and custom texture maps.  One or more BODY classes are created based
	on a TMPL, and the TMPL attaches models and materials to the body
	based on more abstract concepts like actions and costumes.

*************************************************************************/
#ifndef TMPL_H
#define TMPL_H

/****************************************
	Cel part spec: tells what model and
	xfrm to apply to a body part for
	one cel
****************************************/
struct CPS
	{
	short chidModl;	// CHID (under TMPL chunk) of model for this body part
	short imat34;	// index into ACTN's GL of transforms
	};
const BOM kbomCps = 0x50000000;


/****************************************
	Cel: tells what CPS's to apply to an
	actor for one cel.  It also tells
	what sound to play (if any), and how
	far the actor should move from the
	previous cel (dwr).
****************************************/
struct CEL
	{
	CHID chidSnd;	// sound to play at this cel (CHID under ACTN chunk)
	BRS dwr;		// distance from previous cel
//	CPS rgcps[];	// list of cel part specs (variable part of pggcel)
	};
const BOM kbomCel = 0xf0000000;

// template on file
struct TMPLF
	{
	short bo;
	short osk;
	BRA xaRest; // reminder: BRAs are shorts
	BRA yaRest;
	BRA zaRest;
	short swPad; // so grftmpl (and the whole TMPLF) is long-aligned
	ulong grftmpl;
	};
#define kbomTmplf 0x554c0000

// action chunk on file
struct ACTNF
	{
	short bo;
	short osk;
	long grfactn;
	};
const ulong kbomActnf = 0x5c000000;


// grfactn flags
enum
	{
	factnRotateX = 1,	// Tells whether actor should rotate around this
	factnRotateY = 2,   //   axis when following a path
	factnRotateZ = 4,
	factnStatic = 8,	// Tells whether this is a stationary action
	};

/****************************************
	ACTN (action): all the information
	for an action like 'rest' or 'walk'.
****************************************/
typedef class ACTN *PACTN;
#define ACTN_PAR BACO
#define kclsACTN 'ACTN'
class ACTN : public ACTN_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PGG _pggcel;	// GG of CELs; variable part is a rgcps[]
	PGL _pglbmat34;	// GL of transformation matrices used in this action
	PGL _pgltagSnd;	// GL of motion-match sounds for this action
	ulong _grfactn;	// various flags for this action

protected:
	ACTN(void) {}	// can't instantiate directly; must use FReadActn
	bool _FInit(PCFL pcfl, CTG ctg, CNO cno);

public:
	static PACTN PactnNew(PGG pggcel, PGL pglbmat34, ulong grfactn);
	static bool FReadActn(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
		PBACO *ppbaco, long *pcb);
	~ACTN(void);

	ulong Grfactn(void) { return _grfactn; }

	long Ccel(void) { return _pggcel->IvMac(); }
	void GetCel(long icel, CEL *pcel);
	void GetCps(long icel, long icps, CPS *pcps);
	void GetMatrix(long imat34, BMAT34 *pbmat34);
	void GetSnd(long icel, PTAG ptagSnd);
	};


// grftmpl flags
enum
	{
	ftmplOnlyCustomCostumes = 1,  // fTrue means don't apply generic MTRLs
	ftmplTdt = 2,				  // fTrue means this is a 3-D Text object
	ftmplProp = 4,				  // fTrue means this is a "prop" actor
	};

/****************************************
	TMPL: The template class.
	anid is an action ID.
	cmid is a costume ID.
	celn is a cel number.
****************************************/
typedef class TMPL *PTMPL;
#define TMPL_PAR BACO
#define kclsTMPL 'TMPL'
class TMPL : public TMPL_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	BRA _xaRest;			// Rest orientation
	BRA _yaRest;
	BRA _zaRest;
	ulong _grftmpl;
	PGL _pglibactPar;		// GL of parent IDs (shorts) to build BODY
	PGL _pglibset;			// GL of body-part-set IDs to build BODY
	PGG _pggcmid;			// List of costumes for each body part set
	long _ccmid;			// Count of custom costumes
	long _cbset;			// Count of body part sets
	long _cactn;			// Count of actions
	STN _stn;			 	// Template name

protected:
	TMPL(void) {}		// can't instantiate directly; must use FReadTmpl
	bool _FReadTmplf(PCFL pcfl, CTG ctg, CNO cno);
	virtual bool _FInit(PCFL pcfl, CTG ctgTmpl, CNO cnoTmpl);
	virtual PACTN _PactnFetch(long anid);
	virtual PMODL _PmodlFetch(CHID chidModl);
	bool _FWriteTmplf(PCFL pcfl, CTG ctg, CNO *pcno);

public:
	static bool FReadTmpl(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
		PBACO *ppbaco, long *pcb);
	~TMPL(void);
	static PGL PgltagFetch(PCFL pcfl, CTG ctg, CNO cno, bool *pfError);

	// TMPL / BODY stuff
	void GetName(PSTN pstn); // default name of actor or text of the TDT
	PBODY PbodyCreate(void);	// Creates a body based on this TMPL
	void GetRestOrien(BRA *pxa, BRA *pya, BRA *pza);
	bool FIsTdt(void) { return FPure(_grftmpl & ftmplTdt); }
	bool FIsProp(void) { return FPure(_grftmpl & ftmplProp); }

	// Action stuff
	long Cactn(void) { return _cactn; } // count of actions
	virtual bool FGetActnName(long anid, PSTN pstn);
	bool FSetActnCel(PBODY pbody, long anid, long celn,	BRS *pdwr = pvNil);
	bool FGetGrfactn(long anid, ulong *pgrfactn);
	bool FGetDwrActnCel(long anid, long celn, BRS *pdwr);
	bool FGetCcelActn(long anid, long *pccel);
	bool FGetSndActnCel(long anid, long celn, bool *pfSoundExists, 
		PTAG ptag);

	// Costume stuff
	virtual bool FSetDefaultCost(PBODY pbody); // applies default costume
	virtual PCMTL PcmtlFetch(long cmid);
	long CcmidOfBset(long ibset);
	long CmidOfBset(long ibset, long icmid);
	bool FBsetIsAccessory(long ibset); // whether ibset holds accessories
	bool FIbsetAccOfIbset(long ibset, long *pibsetAcc);
	bool FSameAccCmids(long cmid1, long cmid2);
	};


#endif TMPL_H
