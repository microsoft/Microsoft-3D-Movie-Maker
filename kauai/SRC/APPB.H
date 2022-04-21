/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	The base application class.  Most apps will need to subclass this.

***************************************************************************/
#ifndef APPB_H
#define APPB_H


/***************************************************************************
	Misc types
***************************************************************************/
#ifdef WIN
typedef MSG EVT;
#endif //WIN
#ifdef MAC
typedef EventRecord EVT;
#endif //WIN
typedef EVT *PEVT;

#ifdef WIN
// windows specific globals
struct WIG
	{
	HINSTANCE hinst;
	HINSTANCE hinstPrev;
	LPTSTR pszCmdLine;
	int wShow;

	HWND hwndApp;
	HDC hdcApp;
	HWND hwndClient;		//MDI client window
	HACCEL haccel;			//main accelerator table
	HWND hwndNextViewer;	//next clipboard viewer
	long lwThreadMain;		//main thread
	};
extern WIG vwig;
#endif //WIN


/***************************************************************************
	The base application class.
***************************************************************************/
const long kcmhlAppb = klwMax;  // appb goes at the end of the cmh list

enum
	{
	fappNil = 0x0,
	fappOffscreen = 0x1,
	fappOnscreen = 0x2,
	};


typedef class APPB *PAPPB;
#define APPB_PAR CMH
#define kclsAPPB 'APPB'
class APPB : public APPB_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	CMD_MAP_DEC(APPB)

protected:
	//marked region - for fast updating
	struct MKRGN
		{
		HWND hwnd;
		PREGN pregn;
		};

	// map from a property id to its value
	struct PROP
		{
		long prid;
		long lw;
		};

	// modal context
	struct MODCX
		{
		long cactLongOp;
		PCEX pcex;
		PUSAC pusac;
		ulong luScale;
		};

	#ifdef DEBUG
		bool _fCheckForLostMem: 1;	// whether to check for lost mem at idle
		bool _fInAssert: 1;			// whether we're in an assert
		bool _fRefresh: 1;			// whether to refresh the entire display
	#endif //DEBUG
	bool _fQuit: 1;					// whether we're in the process of quitting
	bool _fOffscreen: 1;			// whether to do offscreen updates by default
	bool _fFullScreen: 1;			// when maximized, we hide the caption, etc
	bool _fToolTip: 1;				// whether we're in tool-tip mode
	bool _fForeground: 1;			// whether we're the foreground app
	bool _fEndModal: 1;				// set to end the topmost modal loop

	PGL _pglmkrgn;			// list of marked regions for fast updating
	long _onnDefFixed;		// default fixed pitch font
	long _onnDefVariable;	// default variable pitched font
	PGPT _pgptOff;			// cached offscreen GPT for offscreen updates
	long _dxpOff;			// size of the offscreen GPT
	long _dypOff;

	long _xpMouse;			// location of mouse on last reported mouse move
	long _ypMouse;
	PGOB _pgobMouse;		// gob mouse was last over
	ulong _grfcustMouse;	// cursor state on last mouse move

	// for determining the multiplicity of a click
	long _tsMouse;			// time of last mouse click
	long _cactMouse;		// multiplicity of last mouse click

	// for tool tips
	ulong _tsMouseEnter;	// when the mouse entered _pgobMouse
	ulong _dtsToolTip;		// time lag for tool tip
	PGOB _pgobToolTipTarget;// if there is a tool tip up, it's for this gob

	PCURS _pcurs;			// current cursor
	PCURS _pcursWait;		// cursor to use for long operations
	long _cactLongOp;		// long operation count
	ulong _grfcust;			// current cursor state

	long _gft;				// transition to apply during next fast update
	long _lwGft;			// parameter for transition
	ulong _dtsGft;			// how much time to give the transition
	PGL _pglclr;			// palette to transition to
	ACR _acr;				// intermediate color to transition to

	PGL _pglprop;			// the properties

	PGL _pglmodcx;			// The modal context stack
	long _lwModal;			// Return value from modal loop
	long _cactModal;		// how deep we are in application loops

	// initialization, running and clean up
	virtual bool _FInit(ulong grfapp, ulong grfgob, long ginDef);
	#ifdef DEBUG
		virtual bool _FInitDebug(void);
	#endif //DEBUG
	virtual bool _FInitOS(void);
	virtual bool _FInitMenu(void);
	virtual void _Loop(void);
	virtual void _CleanUp(void);
	virtual bool _FInitSound(long wav);

	// event fetching and dispatching
	virtual bool _FGetNextEvt(PEVT pevt);
	virtual void _DispatchEvt(PEVT pevt);
	virtual bool _FTranslateKeyEvt(EVT *pevt, PCMD_KEY pcmd);

	#ifdef MAC
		// event handlers
		virtual void _MouseDownEvt(EVT *pevt);
		virtual void _MouseUpEvt(EVT *pevt);
		virtual void _UpdateEvt(EVT *pevt);
		virtual void _ActivateEvt(EVT *pevt);
		virtual void _DiskEvt(EVT *pevt);
		virtual void _ActivateApp(EVT *pevt);
		virtual void _DeactivateApp(EVT *pevt);
		virtual void _MouseMovedEvt(EVT *pevt);
	#endif

	// fast updating
	virtual void _FastUpdate(PGOB pgob, PREGN pregnClip,
		ulong grfapp = fappNil, PGPT pgpt = pvNil);
	virtual void _CopyPixels(PGNV pgvnSrc, RC *prcSrc,
		PGNV pgnvDst, RC *prcDst);
	void _MarkRegnRc(PREGN pregn, RC *prc, PGOB pgobCoo);
	void _UnmarkRegnRc(PREGN pregn, RC *prc, PGOB pgobCoo);

	// to borrow the common offscreen GPT
	virtual PGPT _PgptEnsure(RC *prc);

	// property list management
	bool _FFindProp(long prid, PROP *pprop, long *piprop = pvNil);
	bool _FSetProp(long prid, long lw);

	// tool tip support
	void _TakeDownToolTip(void);
	void _EnsureToolTip(void);

	// window procs
	#ifdef WIN
		static LRESULT CALLBACK _LuWndProc(HWND hwnd, uint wm,
			WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK _LuMdiWndProc(HWND hwnd, uint wm,
			WPARAM wParam, LPARAM lParam);

		virtual bool _FFrameWndProc(HWND hwnd, uint wm,
			WPARAM wParam, LPARAM lw, long *plwRet);
		virtual bool _FMdiWndProc(HWND hwnd, uint wm,
			WPARAM wParam, LPARAM lw, long *plwRet);
		virtual bool _FCommonWndProc(HWND hwnd, uint wm,
			WPARAM wParam, LPARAM lw, long *plwRet);

		// remove ourself from the clipboard viewer chain
		void _ShutDownViewer(void);
	#endif //WIN

	// Activation
	virtual void _Activate(bool fActive);

public:
	APPB(void);
	~APPB(void);

	#ifdef MAC
		// setting up the heap
		static void _SetupHeap(long cbExtraStack, long cactMoreMasters);
		virtual void SetupHeap(void);
	#endif //MAC

	bool FQuitting(void) { return _fQuit; }
	bool FForeground(void) { return _fForeground; }

	// initialization, running and quitting
	virtual void Run(ulong grfapp, ulong grfgob, long ginDef);
	virtual void Quit(bool fForce);
	virtual void Abort(void);
	virtual void TopOfLoop(void);

	// look for the next key event in the system event queue
	virtual bool FGetNextKeyFromOsQueue(PCMD_KEY pcmd);

	// Look for mouse events and get the mouse location
	// GrfcustCur() is synchronized with this
	void TrackMouse(PGOB pgob, PT *ppt);

	// app name
	virtual void GetStnAppName(PSTN pstn);

	// command handler stuff
	virtual void BuryCmh(PCMH pcmh);
	virtual PCMH PcmhFromHid(long hid);

	// drawing
	virtual void UpdateHwnd(HWND hwnd, RC *prc, ulong grfapp = fappNil);
	virtual void MarkRc(RC *prc, PGOB pgobCoo);
	virtual void MarkRegn(PREGN pregn, PGOB pgobCoo);
	virtual void UnmarkRc(RC *prc, PGOB pgobCoo);
	virtual void UnmarkRegn(PREGN pregn, PGOB pgobCoo);
	virtual bool FGetMarkedRc(HWND hwnd, RC *prc);
	virtual void UpdateMarked(void);
	virtual void InvalMarked(HWND hwnd);
	virtual void SetGft(long gft, long lwGft, ulong dts = kdtsSecond,
		PGL pglclr = pvNil, ACR acr = kacrClear);

	// default fonts
	virtual long OnnDefVariable(void);
	virtual long OnnDefFixed(void);
	virtual long DypTextDef(void);

	// basic alert handling
	virtual bool TGiveAlertSz(PSZ psz, long bk, long cok);

	// common commands
	virtual bool FCmdQuit(PCMD pcmd);
	virtual bool FCmdShowClipboard(PCMD pcmd);
	virtual bool FEnableAppCmd(PCMD pcmd, ulong *pgrfeds);
	virtual bool FCmdIdle(PCMD pcmd);
	virtual bool FCmdChooseWnd(PCMD pcmd);
	#ifdef MAC
		virtual bool FCmdOpenDA(PCMD pcmd);
	#endif //MAC

	#ifdef DEBUG
		virtual bool FAssertProcApp(PSZS pszsFile, long lwLine, PSZS pszsMsg,
			void *pv, long cb);
		virtual void WarnProcApp(PSZS pszsFile, long lwLine, PSZS pszsMsg);
	#endif //DEBUG

	// cursor stuff
	virtual void SetCurs(PCURS pcurs, bool fLongOp = fFalse);
	virtual void SetCursCno(PRCA prca, CNO cno, bool fLongOp = fFalse);
	virtual void RefreshCurs(void);
	virtual ulong GrfcustCur(bool fAsynch = fFalse);
	virtual void ModifyGrfcust(ulong grfcustOr, ulong grfcustXor);
	virtual void HideCurs(void);
	virtual void ShowCurs(void);
	virtual void PositionCurs(long xpScreen, long ypScreen);
	virtual void BeginLongOp(void);
	virtual void EndLongOp(bool fAll = fFalse);

	// setting and fetching properties
	virtual bool FSetProp(long prid, long lw);
	virtual bool FGetProp(long prid, long *plw);

	// clipboard importing - normally only called by the clipboard object
	virtual bool FImportClip(long clfm, void *pv = pvNil, long cb = 0,
		PDOCB *ppdocb = pvNil, bool *pfDelay = pvNil);

	// reset tooltip tracking.
	virtual void ResetToolTip(void);

	// modal loop support
	virtual bool FPushModal(PCEX pcex = pvNil);
	virtual bool FModalLoop(long *plwRet);
	virtual void EndModal(long lwRet);
	virtual void PopModal(void);
	virtual bool FCmdEndModal(PCMD pcmd);
	long CactModal(void)
		{ return _cactModal; }
	virtual void BadModalCmd(PCMD pcmd);

	// Query save changes for a document
	virtual bool TQuerySaveDoc(PDOCB pdocb, bool fForce);

	// flush user generated events from the system event queue.
	virtual void FlushUserEvents(ulong grfevt = kgrfevtAll);

	// whether to allow a screen saver to come up
	virtual bool FAllowScreenSaver(void);
	};


extern PAPPB vpappb;
extern PCEX vpcex;
extern PSNDM vpsndm;


// main entry point for the client app
void FrameMain(void);


// alert button kinds
enum
	{
	bkOk,
	bkOkCancel,
	bkYesNo,
	bkYesNoCancel,
	};

// alert icon kinds
enum
	{
	cokNil,
	cokInformation,		//general info to/from the user
	cokQuestion,		//ask the user something
	cokExclamation,		//warn the user and/or ask something
	cokStop,			//inform the user that we can't do that
	};

#endif //!APPB_H

