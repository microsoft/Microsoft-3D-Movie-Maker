/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Command execution.  Manages the command filter list and dispatching
	commands to command handlers.

***************************************************************************/
#ifndef CMD_H
#define CMD_H


/***************************************************************************
	Command id, options and command struct
***************************************************************************/

// command handler forward declaration
class CMH;
typedef CMH *PCMH;

// command enable-disable status flags
enum
	{
	fedsNil = 0,
	fedsDisable = 1,
	fedsEnable = 2,
	fedsUncheck = 4,
	fedsCheck = 8,
	fedsBullet = 16
	};
const ulong kgrfedsMark	= fedsUncheck | fedsCheck | fedsBullet;

// command
#define kclwCmd 4	//if this ever changes, change the CMD_TYPE macro also
struct CMD
	{
	ASSERT

	PCMH pcmh;	// the target of the command - may be nil
	long cid;	// the command id
	PGG pgg;	// additional parameters for the command
	long rglw[kclwCmd];	// standard parameters
	};
typedef CMD *PCMD;


//command on file - for saving recorded macros
struct CMDF
	{
	long cid;
	long hid;
	long cact;
	CHID chidGg;	// child id of the pgg, 0 if none
	long rglw[kclwCmd];
	};


/***************************************************************************
	Custom command types
***************************************************************************/
// used to define a new CMD structure.  Needs a trailing semicolon.
#define CMD_TYPE(foo,a,b,c,d) \
	struct CMD_##foo \
		{ \
		PCMH pcmh; \
		long cid; \
		PGG pgg; \
		long a, b, c, d; \
		}; \
	typedef CMD_##foo * PCMD_##foo

CMD_TYPE(KEY, ch, vk, grfcust, cact);	//defines CMD_KEY and PCMD_KEY
CMD_TYPE(BADKEY, ch, vk, grfcust, hid);	//defines CMD_BADKEY and PCMD_BADKEY
CMD_TYPE(MOUSE, xp, yp, grfcust, cact);	//defines CMD_MOUSE and PCMD_MOUSE


/***************************************************************************
	Command Map stuff.  To attach a command map to a subclass of CMH,
	put a CMD_MAP_DEC(cls) in the definition of the class.  Then in the
	.cpp file, use BEGIN_CMD_MAP, ON_CID and END_CMD_MAP to define the
	command map.  This architecture was borrowed from MFC.
***************************************************************************/
enum
	{
	fcmmNil = 0,
	fcmmThis = 1,
	fcmmNobody = 2,
	fcmmOthers = 4,
	};
const ulong kgrfcmmAll = fcmmThis | fcmmNobody | fcmmOthers;


// for including a command map in this class
#define CMD_MAP_DEC(cls) \
	private: \
		static CMME _rgcmme##cls[]; \
	protected: \
		static CMM _cmm##cls; \
		virtual CMM *Pcmm(void) { return &_cmm##cls; }

// for defining the command map in a .cpp file
#define BEGIN_CMD_MAP_BASE(cls) \
	cls::CMM cls::_cmm##cls = { pvNil, cls::_rgcmme##cls }; \
	cls::CMME cls::_rgcmme##cls[] = {
#define BEGIN_CMD_MAP(cls, clsBase) \
	cls::CMM cls::_cmm##cls = { &(clsBase::_cmm##clsBase), cls::_rgcmme##cls }; \
	cls::CMME cls::_rgcmme##cls[] = {

#define ON_CID(cid,pfncmd,pfneds,grfcmm) \
	{ cid, (PFNCMD)pfncmd, (PFNEDS)pfneds, grfcmm },
#define ON_CID_ME(cid,pfncmd,pfneds) \
	{ cid, (PFNCMD)pfncmd, (PFNEDS)pfneds, fcmmThis },
#define ON_CID_GEN(cid,pfncmd,pfneds) \
	{ cid, (PFNCMD)pfncmd, (PFNEDS)pfneds, fcmmThis | fcmmNobody },
#define ON_CID_ALL(cid,pfncmd,pfneds) \
	{ cid, (PFNCMD)pfncmd, (PFNEDS)pfneds, kgrfcmmAll },

#define END_CMD_MAP(pfncmdDef, pfnedsDef, grfcmm) \
	{ cidNil, (PFNCMD)pfncmdDef, (PFNEDS)pfnedsDef, grfcmm } };
#define END_CMD_MAP_NIL() \
	{ cidNil, pvNil, pvNil, fcmmNil } };


/***************************************************************************
	Command handler class
***************************************************************************/
#define CMH_PAR BASE
#define kclsCMH 'CMH'
class CMH : public CMH_PAR
	{
	RTCLASS_DEC
	ASSERT

private:
	static long _hidLast;	// for HidUnique
	long _hid;				// handler id

protected:
	// command function
	typedef bool (CMH::*PFNCMD)(PCMD pcmd);

	//command enabler function
	typedef bool (CMH::*PFNEDS)(PCMD pcmd, ulong *pgrfeds);

	// command map entry
	struct CMME
		{
		long cid;
		PFNCMD pfncmd;
		PFNEDS pfneds;
		ulong grfcmm;
		};

	// command map
	struct CMM
		{
		CMM *pcmmBase;
		CMME *prgcmme;
		};

	CMD_MAP_DEC(CMH)

protected:
	virtual bool _FGetCmme(long cid, ulong grfcmmWanted, CMME *pcmme);

public:
	CMH(long hid);
	~CMH(void);

	// return indicates whether the command was handled, not success
	virtual bool FDoCmd(PCMD pcmd);
	virtual bool FEnableCmd(PCMD pcmd, ulong *pgrfeds);

	long Hid(void)
		{ return _hid; }

	static long HidUnique(long ccmh = 1);
	};


/***************************************************************************
	Command execution manager (dispatcher)
***************************************************************************/
// command stream recording error codes.
enum
	{
	recNil,
	recFileError,
	recMemError,
	recWrongPlatform,
	recAbort,
	recLim
	};


typedef class CEX *PCEX;
#define CEX_PAR BASE
#define kclsCEX 'CEX'
class CEX : public CEX_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(CEX)

protected:
	// an entry in the command handler list
	struct CMHE
		{
		PCMH pcmh;
		long cmhl;
		ulong grfcmm;
		};

	// command recording/playback state
	enum
		{
		rsNormal,
		rsRecording,
		rsPlaying,
		rsLim
		};

	// recording and playback
	long _rs;			// recording/playback state
	long _rec;			// recording/playback errors
	PCFL _pcfl;			// the file we are recording to or playing from
	PGL _pglcmdf;		// the command stream
	CNO _cno;			// which macro is being played
	long _icmdf;		// current command for recording or playback
	CHID _chidLast;		// last chid used for recording
	long _cact;			// number of times on this command
	CMD _cmd;			// previous command recorded or played

	// dispatching
	CMD _cmdCur;		// command being dispatched
	long _icmheNext;	// next command handler to dispatch to
	PGOB _pgobTrack;	// the gob that is tracking the mouse
	#ifdef WIN
		HWND _hwndCapture;	// the hwnd that we captured the mouse with
	#endif //WIN

	// filter list and command queue
	PGL _pglcmhe;		// the command filter list
	PGL _pglcmd;		// the command queue
	bool _fDispatching;	// whether we're currently in FDispatchNextCmd

	// Modal filtering
	PGOB _pgobModal;

	#ifdef DEBUG
		long _ccmdMax;	// running max
	#endif //DEBUG

	CEX(void);

	virtual bool _FInit(long ccmdInit, long ccmhInit);
	virtual bool _FFindCmhl(long cmhl, long *picmhe);

	virtual bool _FCmhOk(PCMH pcmh);
	virtual bool _TGetNextCmd(void);
	virtual bool _FSendCmd(PCMH pcmh);
	virtual void _CleanUpCmd(void);
	virtual bool _FEnableCmd(PCMH pcmh, PCMD pcmd, ulong *pgrfeds);

	//command recording and playback
	bool _FReadCmd(PCMD pcmd);

public:
	static PCEX PcexNew(long ccmdInit, long ccmhInit);
	~CEX(void);

	// recording and play back
	bool FRecording(void) { return _rs == rsRecording; }
	bool FPlaying(void) { return _rs == rsPlaying; }
	void Record(PCFL pcfl);
	void Play(PCFL pcfl, CNO cno);
	void StopRecording(void);
	void StopPlaying(void);

	void RecordCmd(PCMD pcmd);

	// managing the filter list
	virtual bool FAddCmh(PCMH pcmh, long cmhl, ulong grfcmm = fcmmNobody);
	virtual void RemoveCmh(PCMH pcmh, long cmhl);
	virtual void BuryCmh(PCMH pcmh);

	// queueing and dispatching
	virtual void EnqueueCmd(PCMD pcmd);
	virtual void PushCmd(PCMD pcmd);
	virtual void EnqueueCid(long cid, PCMH pcmh = pvNil, PGG pgg = pvNil,
		long lw0 = 0, long lw1 = 0, long lw2 = 0, long lw3 = 0);
	virtual void PushCid(long cid, PCMH pcmh = pvNil, PGG pgg = pvNil,
		long lw0 = 0, long lw1 = 0, long lw2 = 0, long lw3 = 0);
	virtual bool FDispatchNextCmd(void);
	virtual bool FGetNextKey(PCMD pcmd);
	virtual bool FCidIn(long cid);
	virtual void FlushCid(long cid);

	// menu marking
	virtual bool GrfedsForCmd(PCMD pcmd);
	virtual bool GrfedsForCid(long cid, PCMH pcmh = pvNil, PGG pgg = pvNil,
		long lw0 = 0, long lw1 = 0, long lw2 = 0, long lw3 = 0);

	// mouse tracking
	virtual void TrackMouse(PGOB pgob);
	virtual void EndMouseTracking(void);
	virtual PGOB PgobTracking(void);

	virtual void Suspend(bool fSuspend = fTrue);
	virtual void SetModalGob(PGOB pgob);
	};

#endif //!CMD_H

