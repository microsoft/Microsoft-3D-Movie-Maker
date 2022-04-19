/***************************************************************************

	Status: All changes must be code reviewed.

	Movie Stuff

		A single view on a movie (MVU)

				DDG  	--->	MVU

        Callbacks to client (MCC)

				BASE 	--->	MCC

        A single movie (MVIE)

				DOCB	--->	MVIE

***************************************************************************/

#ifndef MOVIE_H
#define MOVIE_H

//
// Tools that can be "loaded" on the mouse cursor
//
enum
	{
	toolPlace,		// (to position an actor when you first add it)
	toolCompose,	
	toolAction,	 	// action is in _anidTool if we're motion filling
	toolTweak,
	toolRotateX,
	toolRotateY,
	toolRotateZ,
	toolCostumeCmid,// cmid is in _cmidTool
	toolSquashStretch,
	toolSoonerLater,
	toolResize,
	toolNormalizeRot,
	toolCopyObject,
	toolCutObject,
	toolPasteObject,
	toolCopyRte,
	toolDefault,
	toolTboxMove,
	toolTboxUpDown,
	toolTboxLeftRight,
	toolTboxFalling,
	toolTboxRising,
	toolSceneNuke,
	toolIBeam,
	toolSceneChop,
	toolSceneChopBack,
	toolCutText,
	toolCopyText,
	toolPasteText,
	toolPasteRte,
	toolActorEasel,
	toolActorSelect,
	toolActorNuke,
	toolTboxPaintText,
	toolTboxFillBkgd,
	toolTboxStory,
	toolTboxCredit,
	toolNormalizeSize,
	toolRecordSameAction,
	toolSounder,
	toolLooper,
	toolMatcher,
	toolTboxFont,
	toolTboxStyle,
	toolTboxSize,
	toolListener,
	toolAddAFrame,
	toolFWAFrame,
	toolRWAFrame,
	toolComposeAll,
	toolUndo,		// For playing UI Sound only
	toolRedo,		// For playing UI Sound only
	toolFWAScene,	// For playing UI Sound only
	toolRWAScene,	// For playing UI Sound only

	toolLimMvie
	};

//
// Used to tell the client the change the state of the undo UI
//
enum
	{
	undoDisabled = 1,
	undoUndo,
	undoRedo
	};

//
// grfbrws flags
//
enum
	{
	fbrwsNil = 0,
	fbrwsProp = 1,  // fTrue implies prop or 3d
	fbrwsTdt = 2	// fTrue means this is a 3-D Text object
	};


//
//
// A class for handling a single Movie view
//
//


//
// The class definition
//
#define MVU_PAR DDG

typedef class MVU *PMVU;
#define kclsMVU 'MVU'
class MVU : public MVU_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	CMD_MAP_DEC(MVU)


protected:

	/* Make these static; we want to be able to set and restore without having
		an actual MVU, and they shouldn't be getting set per-MVU anyway */
	static bool _fKbdDelayed;       // fTrue == we have delayed the keyboard
	static long _dtsKbdDelay;       // System keyboard delay before first repeat
	static long _dtsKbdRepeat;      // System keyboard delay between repeats

	long _dxp;
	long _dyp;						// width and height rendered area.

	bool _fTrackingMouse;			// Is the mouse currently being tracked?
	long _xpPrev;					// X location of the mouse.
	long _ypPrev;					// Y location of the mouse.
	BRS _dzrPrev;					// Z motion of the "mouse" (arrow keys)
	long _grfcust;					// Options in effect when mouse was down.
	PCURS _pcursDefault;			// Default cursor for when a tool is not applicable.
	PACTR _pactrListener;			// Pactr of the actor being auditioned
	PACTR _pactrRestore;			// Restore for actor recording

	long _anidTool;					// Current selected action	
	TAG _tagTool;					// Tag associated with current tool.
	PTMPL _ptmplTool;				// Template associated with the current tool.
	long _cmidTool;					// Costume id associated with current tool.
	bool _fCyclingCels;				// Are we cycling cels in toolRecord?
	long _tool;						// Current tool loaded on cursor

	// REVIEW Seanse(SeanSe): MVIE should not be creating/mucking with actor undo
	//   objects.  V2.666 should revisit this issue and see if we can get ACTR to
	//   do all its own undo objects (e.g. Growing over a long drag could become
	//   a StartGrow/Grow/EndGrow sequence).  This also effects _pactrRestore.
	PAUND _paund;					// Actor undo object to save from mouse down to drag.
	ulong _tsLast;					// Last time a cell was recorded.
	ulong _tsLastSample;				// Last time mouse/kbd sampled
	RC _rcFrame;					// Frame for creating a text box.

	BRS _rgrAxis[3][3];				// Conversion from mouse points to 3D points.
	bool _fRecordDefault; 			// fTrue = Record; fFalse = Rerecord

	bool _fPause:1;					// fTrue if pausing play until a click.
	bool _fTextMode:1;				// fTrue if Text boxes are active.
	bool _fRespectGround:1;			// fTrue if Y=0 is enforced.
	bool _fSetFRecordDefault:1;		// fTrue if using hotkeys to rerecord.
	bool _fMouseOn:1;
	bool _fEntireScene:1;			// Does positioning effect the entire scene?

	bool _fMouseDownSeen;			// Was the mouse depressed during a place.
	PACTR _pactrUndo;				// Actor to use for undo object when roll-calling.

	ACR _acr;						// Color for painting text.
	long _onn;                      // Font for text
	long _dypFont;                  // Font size for text
	ulong _grfont;                   // Font style for text
	long _lwLastTime;				// State variable for the last time through.

	MVU(PDOCB pdocb, PGCB pgcb) : DDG(pdocb, pgcb) { }

	//
	// Clipboard support
	//
	bool _FCopySel(PDOCB *ppdocb, bool fRteOnly);
	void _ClearSel(void);
	bool _FPaste(PCLIP pclip);

	void _PositionActr(BRS dxrWld, BRS dyrWld, BRS dzrWld);
	void _MouseDown(CMD_MOUSE *pcmd);
	void _MouseDrag(CMD_MOUSE *pcmd);
	void _MouseUp(CMD_MOUSE *pcmd);

	void _ActorClicked(PACTR pactr, bool fDown);

public:

	static void SlowKeyboardRepeat(void);
	static void RestoreKeyboardRepeat(void);

	//
	// Constructors and desctructors
	//
	static MVU *PmvuNew(PMVIE pmvie, PGCB pgcb, long dxy, long dyp);
    ~MVU(void);

	//
	// Accessor for getting the owning movie
	//
	PMVIE Pmvie() { return (PMVIE)_pdocb; }

	//
	// Command handlers
	//
	virtual bool FCmdTrackMouse(PCMD_MOUSE pcmd);
	virtual bool FCmdMouseMove(PCMD_MOUSE pcmd);
	virtual bool FCmdClip(CMD *pcmd);
	virtual bool FCmdUndo(PCMD pcmd);
	virtual bool FCloseDoc(bool fAssumeYes, bool fSaveDDG = fFalse);
	virtual bool FCmdSave(PCMD pcmd);
	bool FDoClip(long tool);
	bool FCmdIdle(CMD *pcmd);					// Called whenever an idle loop is seen.
	bool FCmdRollOff(CMD *pcmd);

	//
	// View specific functions.
	//
	void SetTool(long tool);
	long Tool(void) { return _tool; }
	long AnidTool(void) { return _anidTool; }
	long CmidTool(void) { return _cmidTool; }
	PTAG PtagTool(void) { return &_tagTool; }
	void SetAnidTool(long anid) { _anidTool = anid; }
	void SetTagTool(PTAG ptag);
	void SetCmidTool(long cmid) { _cmidTool = cmid; }
	void StartPlaceActor(bool fEntireScene = fFalse);
	void EndPlaceActor(void);
	void WarpCursToCenter(void);
	void WarpCursToActor(PACTR pactr);
	void AdjustCursor(long xp, long yp);
	void MouseToWorld(BRS dxrMouse, BRS dyrMouse, BRS dzrMouse, BRS *pdxrWld,
		BRS *pdyrWld, BRS *pdzrWld, bool fRecord);
	void SetAxis(BRS rgrAxis[3][3]) {BltPb(rgrAxis, _rgrAxis, size(BRS) * 9); }
	void SetFRecordDefault(bool f) { _fRecordDefault = f; }
	void SetFRespectGround(bool f) { _fRespectGround = f; }
	bool FRecordDefault() { return _fRecordDefault; }
	void PauseUntilClick(bool fPause) { _fPause = fPause; }
	bool FPausing(void) { return _fPause; }
	bool FTextMode(void) { return _fTextMode; }
	bool FActrMode(void) { return !_fTextMode; }
	bool FRespectGround(void) { return _fRespectGround; }
	void SetActrUndo(PACTR pactr) { _pactrUndo = pactr; }
	void SetPaintAcr(ACR acr) { _acr = acr; }
	ACR AcrPaint(void) { return _acr; }
	void SetOnnTextCur(long onn) { _onn = onn; }
	long OnnTextCur(void) { return _onn; }
	void SetDypFontTextCur(long dypFont) { _dypFont = dypFont; }
	long DypFontTextCur(void) { return _dypFont; }
	void SetStyleTextCur(ulong grfont) { _grfont = grfont; }
	ulong GrfontStyleTextCur(void) { return _grfont; }

	//
	// Routines for communicating with the framework
	//
	void Draw(PGNV pgnv, RC *prcClip);

	};




//
//
// Movie Client Callbacks.  Used for filling in
// parameters for this movie, and for notifying
// client of state changes.
//
//

#define MCC_PAR BASE

class MCC;

typedef MCC *PMCC;
#define kclsMCC 'MCC'
class MCC : public MCC_PAR
	{
protected:
	long _dxp;
	long _dyp;
	long _cbCache;

public:

	MCC(long dxp, long dyp, long cbCache)
		{
		_dxp = dxp;
		_dyp = dyp;
		_cbCache = cbCache;
		}
	virtual long Dxp(void) { return _dxp; }			// Width of the rendering area
	virtual long Dyp(void) { return _dyp; }			// Height of the rendering area
	virtual long CbCache(void) { return _cbCache; }	// Number of bytes to use for caching.
	virtual void SetCurs(long tool) { }				// Sets the cursor based on the tool, may be pvNil.
	virtual void UpdateRollCall(void) { }			// Tells the client to update its roll call.
	virtual void UpdateAction(void) { }				// Tells the client to update its action menu.
	virtual void UpdateScrollbars(void) { }			// Tells the client to update its scrollbars.
	virtual void PlayStopped(void) { }				// Tells the client that playback was stopped internally.
	virtual void ChangeTool(long tool) { }			// Tells the client that the tool was changed internally.
	virtual void SceneNuked(void) { }				// Tells the client that a scene was nuked.
	virtual void SceneUnnuked(void) { }				// Tells the client that a scene was nuked and now undone.
	virtual void ActorNuked(void) { }				// Tells the client that an actor was nuked.
	virtual void EnableActorTools(void) { }			// Tells the client that the first actor was added.
	virtual void EnableTboxTools(void) { } 			// Tells the client that the first textbox was added.
	virtual void TboxSelected(void) { }				// Tells the client that a new text box was selected.
	virtual void ActorSelected(long arid) { }		// Tells the client that an actor was selected
	virtual void ActorEasel(bool *pfActrChanged) { } // Lets client edit 3-D Text or costume
	virtual void SetUndo(long undo) { }				// Tells the client the state of the undo buffer.
	virtual void SceneChange(void) { }				// Tells the client that a different scene is the current one
	virtual void PauseType(WIT wit) { }				// Tells the client of a new pause type for this frame.
	virtual void Recording(bool fRecording, bool fRecord) { } 	// Tells the client that the movie engine is recording or not.
	virtual void StartSoonerLater(void) { } 		// Tells the client that an actor is selected for sooner/latering.
	virtual void EndSoonerLater(void) { }			// Tells the client that an actor is done for sooner/latering.
	virtual void NewActor(void) { }					// Tells the client that an actor has just been placed.
	virtual void StartActionBrowser(void) { }		// Tells the client to start up the action browser.
	virtual void StartListenerEasel(void) { }		// Tells the client to start up the listener easel.
	virtual bool GetFniSave(FNI *pfni, long lFilterLabel, long lFilterExt, long lTitle,
					LPTSTR lpstrDefExt, PSTN pstnDefFileName) { return fFalse; } // Tells the client to start up the save portfolio.
	virtual void PlayUISound(long tool, long grfcust = 0) { }	// Tells the client to play sound associated with use of tool.
	virtual void StopUISound(void) { }				// Tells the client to stop sound associated with use of tools.
	virtual void UpdateTitle(PSTN pstnTitle) { }	// Tells the client that the movie name has changed.
	virtual void EnableAccel(void) { }		// Tells the client to enable keyboard accelerators.
	virtual void DisableAccel(void) { }	// Tells the client to disable keyboard accelerators.
	virtual void GetStn(long ids, PSTN pstn) { }	// Requests the client to fetch the given ids string.
	virtual long DypTextDef(void) { return vpappb->DypTextDef(); }
	virtual long DypTboxDef(void) { return 14; }
	virtual void SetSndFrame(bool fSoundInFrame) { }
	virtual bool FMinimized(void) { return fFalse; }
	virtual bool FQueryPurgeSounds(void) { return fFalse; }
	};



/* A SCENe Descriptor */
typedef struct _scend
	{
/* The first fields are private...the client shouldn't change them, and
	in fact, generally shouldn't even look at them */
	long imvied;         // index of the MVIED for this scene
	CNO cno;             // the CNO of this scene chunk
	CHID chid;           // the original CHID
	PMBMP pmbmp;         // pointer to thumbnail MBMP
/* The client can read or write the following fields */
	TRANS trans;         // the transition that will occur after this scene
	bool fNuked:1;       // fTrue if this scene has been deleted
	} SCEND, *PSCEND;

/* A MoVIE Descriptor */
typedef struct _mvied
	{
	PCRF pcrf;           // the file this scene's movie is in
	CNO cno;             // CNO of the MVIE chunk
	long aridLim;        // _aridLim from the MVIE
	} MVIED, *PMVIED;

/* A Composite MoVIe */
typedef struct _cmvi
	{
	PGL pglmvied;        // GL of movie descriptors
	PGL pglscend;        // GL of scene descriptors

	void Empty(void);
#ifdef DEBUG
	void MarkMem(void);
#endif
	} CMVI, *PCMVI;

//
//
// Movie Class.
//
//

const long kccamMax = 9;

typedef class MVIE *PMVIE;

#define MVIE_PAR DOCB
#define kclsMVIE 'MVIE'
class MVIE : public MVIE_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT
	CMD_MAP_DEC(MVIE)

protected:

	long _aridLim;								// Highest actor id in use.

	PCRF _pcrfAutoSave;							// CRF/CFL of auto save file.
	PFIL _pfilSave;								// User's document

	CNO _cno;									// CNO of movie in current file.

	STN _stnTitle;								// Title of the movie

	PGST _pgstmactr;							// GST of actors in the movie (for roll call)
	PSCEN _pscenOpen;							// Index of current open scene.
	long _cscen;								// Number of scenes in the movie.
	long _iscen;								// Number of scene open in the movie.
	bool _fAutosaveDirty:1;						// Is the movie in memory different than disk
	bool _fFniSaveValid:1;						// Does _fniSave contain a file name.
	bool _fPlaying:1;							// Is the movie playing?
	bool _fScrolling:1;							// During playback only, are we scrolling textboxes
	bool _fPausing:1;							// Are we executing a pause.
	bool _fIdleSeen:1;							// fTrue if we have seen an idle since this was cleared.
	bool _fStopPlaying:1;						// Should we stop the movie playing
	bool _fSoundsEnabled:1;						// Should we play sounds or not.
	bool _fOldSoundsEnabled:1;					// Old value of above.
	bool _fDocClosing:1;						// Flags doc is to be closed
	bool _fGCSndsOnClose:1;						// Garbage collection of sounds on close
	bool _fReadOnly:1;                          // Is the original file read-only?

	PBWLD _pbwld;								// The brender world for this movie
	PMSQ _pmsq;									// Message Sound Queue
	CLOK  _clok;								// Clock for playing the film
	ulong _tsStart;								// Time last play started.
	long _cnfrm;								// Number of frames since last play started.

	PMCC _pmcc;									// Parameters and callbacks.

	WIT _wit;									// Pausing type
	long _dts;									// Number of clock ticks to pause.
	TRANS _trans;								// Transition type to execute.
	
	long _vlmOrg;								// original SNDM volume, before fadeout, if we are done with fadeout, then 0

#ifdef DEBUG
	bool _fWriteBmps;
	long _lwBmp;
#endif // DEBUG

	PGL _pglclrThumbPalette;					// Palette to use for thumbnail rendering.

private:

	MVIE(void);
	PTAGL _PtaglFetch(void);					// Returns a list of all tags used in movie
	bool _FCloseCurrentScene(void);				// Closes and releases current scene, if any
	bool _FMakeCrfValid(void);					// Makes sure there is a file to work with.
	bool _FUseTempFile(void);					// Switches to using a temp file.
	void _MoveChids(CHID chid, bool fDown);		// Move the chids of scenes in the movie.
	bool _FDoGarbageCollection(PCFL pcfl);		// Remove unused chunks from movie.
	void _DoSndGarbageCollection(bool fPurgeAll);  // Remove unused user sounds from movie
	bool _FDoMtrlTmplGC(PCFL pcfl);				// Material and template garbage collection
	CHID _ChidScenNewSnd(void);					// Choose an unused chid for a new scene child user sound
	CHID _ChidMvieNewSnd(void);					// Choose an unused chid for a new movie child user sound
	void _SetTitle(PFNI pfni = pvNil);			// Set the title of the movie based on given file name.
	bool _FIsChild(PCFL pcfl, CTG ctg, CNO cno);
	bool _FSetPfilSave(PFNI pfni);

public:


	//
	// Begin client useable functions
	//

#ifdef DEBUG
	void SetFWriteBmps(bool fWriteBmps)
	{
		if (fWriteBmps && !_fWriteBmps)
			_lwBmp = 0;
		_fWriteBmps = fWriteBmps;
	}
	bool FWriteBmps(void) { return _fWriteBmps; }
#endif // DEBUG

	//
	// Getting views
	//
	PMVU PmvuCur(void);
	PMVU PmvuFirst(void);

	//
	// Create and Destroy
	//
	static PMVIE PmvieNew(bool fHalfMode, PMCC pmcc, FNI *pfni = pvNil, CNO cno = cnoNil);	
												// Create a movie and read it if
												//   pfni != pvNil
	static bool FReadRollCall(PCRF pcrf, CNO cno, PGST *ppgst, long *paridLim = pvNil);
		// reads roll call for a given movie
	void ForceSaveAs(void)
	{
		ReleasePpo(&_pfilSave);
		_fFniSaveValid = fFalse;
	}
	void Flush(void);
	bool FReadOnly(void) { return FPure(_fReadOnly); }

	~MVIE(void);

	//
	// MCC maintenance
	//
	PMCC Pmcc(void) { return _pmcc; } // Accessor for getting to client callbacks.
	void SetMcc(PMCC pmcc) { ReleasePpo(&_pmcc); _pmcc = pmcc; _pmcc->AddRef(); }

	//
	// Title stuff
	//
	void GetName(PSTN pstnTitle);			// Gets the title of the movie.
	PSTN PstnTitle(void) { return &_stnTitle; }
	void ResetTitle(void);

	//
	// Scene stuff
	//
	long Cscen(void) 			   			  	// Returns number of scenes in movie
		{ return _cscen; }
    long Iscen(void) 							// Returns the current scene number
		{ return _iscen; }
	bool FSwitchScen(long iscen); 				// Loads and returns pointer to scene iscen,
												//   saving any current scene.
	bool FRemScen(long iscen); 			    	// Removes a scene from the movie, and undo	
	bool FChangeCam(long camid);				// Change the camera view in the scene.
	bool FInsTbox(RC *prc, bool fStory);		// Insert a text box into the scene.
	bool FHideTbox(void);						// Hide selected text box from the scene at this fram.
	bool FNukeTbox(void);						// Remove selected text box from the scene.
	void SelectTbox(long itbox);				// Select the itbox'th text box in the frame.
	void SetPaintAcr(ACR acr);					// Sets color that painting will occur with.
	void SetOnnTextCur(long onn);               // Sets font that text will be in
	void SetDypFontTextCur(long dypFont);       // Sets font size that text will be in
	void SetStyleTextCur(ulong grfont);         // Sets font style that text will be in
	bool FInsActr(PTAG ptag);					// Insert an actor into the scene.
	bool FRemActr(void);						// Remove selected actor from scene.
	bool FAddOnstage(long arid);				// Bring this actor onto the stage.
	bool FRotateActr(BRA xa, BRA ya, BRA za, bool fFromHereFwd);	// Rotate selected actor by degrees
	bool FSquashStretchActr(BRS brs);			// Squash/Stretch selected actor
	bool FSoonerLaterActr(long nfrm);			// Sooner/Later selected actor
	bool FScaleActr(BRS brs);					// Scale selected actor
	bool FCostumeActr(long ibprt, PTAG ptag, long cmid, bool fCustom);
	bool FAddScen(PTAG ptag);					// Add a scene after the current one, or change
												//   change bkgd if scene is empty
	bool FSetTransition(TRANS trans);			// Set the transition type for the current scene.
	void Play(void);							// Start/Stop a movie playing.
	bool FPause(WIT wit, long dts);				// Insert a pause here.

	bool FAddToCmvi(PCMVI pcmvi, long *piscendIns);
	                                            // Add this movie to the CMVI
	bool FSetCmvi(PCMVI pcmvi);                 // Re-build the movie from the CMVI
	bool _FAddMvieToRollCall(CNO cno, long aridMin);
	                                            // Updates roll call for an imported movie
	bool _FInsertScend(PGL pglscend, long iscend, PSCEND pscend);
	                                            // Insert an imported scene
	void _DeleteScend(PGL pglscend, long iscend);// Delete an imported scene
	bool _FAdoptMsndInMvie(PCFL pcfl, CNO cnoScen);	// Adopt msnd chunks as children of the movie


	bool FAddBkgdSnd(PTAG ptag, bool fLoop, bool fQueue, long vlm = vlmNil,
		long sty = styNil); // Adds a sound
	bool FAddActrSnd(PTAG ptag, bool fLoop, bool fQueue, bool fActnCel,
		long vlm, long sty);	// Adds a sound

	//
	// Auto save stuff
	//
	bool FAutoSave(PFNI pfni = pvNil, bool fCleanRollCall = fFalse);// Save movie in temp file
	bool FSaveTagSnd(TAG *ptag) { return TAGM::FSaveTag(ptag, _pcrfAutoSave, fTrue); }
	bool FCopySndFileToMvie(PFIL pfil, long sty, CNO *pcno, PSTN pstn = pvNil);
 	bool FVerifyVersion(PCFL pcfl, CNO *pcno = pvNil);
	bool FEnsureAutosave(PCRF *pcrf = pvNil);
	bool FCopyMsndFromPcfl(PCFL pcfl, CNO cnoSrc, CNO *pcnoDest);
	bool FResolveSndTag(PTAG ptag, CHID chid, CNO cnoScen = cnoNil, PCRF pcrf = pvNil);
	bool FChidFromUserSndCno(CNO cno, CHID *pchid);
	void SetDocClosing(bool fClose) { _fDocClosing = fClose; }
	bool FQueryDocClosing(void) { return _fDocClosing; }
	bool FQueryGCSndsOnClose(void) { return _fGCSndsOnClose; }
	bool FUnusedSndsUser(bool *pfHaveValid = pvNil);

	//
	// Roll call
	//
	bool FGetArid(long iarid, long *parid, PSTN pstn, long *pcactRef, PTAG ptagTmpl = pvNil); // return actors one by one
	bool FChooseArid(long arid);				// user chose arid in roll call
	long AridSelected(void);
	bool FGetName(long arid, PSTN pstn);		// Return the name of a specific actor.
	bool FNameActr(long arid, PSTN pstn);		// Set the name of this actor.
	void ChangeActrTag(long arid, PTAG ptag);	// Change an actor's TMPL tag
	long CmactrMac(void) {AssertThis(0);  return _pgstmactr->IvMac(); }
	bool FIsPropBrwsIarid(long iarid);			// Identify the roll call browser iarids
	bool FIsIaridTdt(long iarid);                   // 3d spletter

	//
	// Overridden DOCB functions
	//
	bool FGetFni(FNI *pfni);					// For saving to a file
	bool FSave(long cid);						// For saving to a file, (calls FGetFni and FSaveToFni)
	bool FSaveToFni(FNI *pfni, bool fSetFni);	// For doing a Save As or Save
	PDMD PdmdNew(void);							// Do not use!
	bool FGetFniSave(FNI *pfni);				// For saving via the portfolio.

	//
	// Drawing stuff
	//
	void InvalViews(void);						// Invalidates all views on the movie.
	void InvalViewsAndScb(void);				// Invalidates all views of movie and scroll bars.
	void MarkViews(void);						// Marks all views on the movie.
	TRANS Trans(void) { return _trans; }		// Current transition in effect.
	void SetTrans(TRANS trans) { _trans = trans; } // Set transition
	void DoTrans(PGNV pgnvDst, PGNV pgnvSrc, RC *prcDst, RC *prcSrc); // Draw current transition into GNVs

	//
	// Sound stuff
	//
	bool FSoundsEnabled(void) { return(_fSoundsEnabled); }
	void SetFSoundsEnabled(bool f) { _fSoundsEnabled = f; }

	//
	// End client callable functions
	//




	//
	// Begin internal movie engine functions
	//

	//
	// Command handlers
	//
   	bool FCmdAlarm(CMD *pcmd);					// Called at timer expiration (playback).
   	bool FCmdRender(CMD *pcmd);					// Called to render a frame during playback.

	//
	// Automated test APIs
	//
	long LwQueryExists(long lwType, long lwId);	// Called by other apps to find if an actor/tbox exists.
    long LwQueryLocation(long lwType, long lwId);	// Called by other apps to find where actor/tbox exists.
	long LwSetMoviePos(long lwScene, long lwFrame);	// Called by other apps to set the movie position.


	//
	// Scene stuff
	//
    PSCEN Pscen(void) { return _pscenOpen; }	// The currently open scene.
	bool FInsScenCore(long iscen, SCEN *pscen); // Insert this scene as scene number.
	bool FNewScenInsCore(long iscen); 			// Inserts a blank scene before iscen
	bool FRemScenCore(long iscen); 			    // Removes a scene from the movie
	bool FPasteActr(PACTR pactr);				// Pastes an actor into current scene.
	bool FPasteActrPath(PACTR pactr);			// Pastes the path onto selected actor.
	PMSQ Pmsq(void) { return _pmsq; }			// Sound queue

	//
	// Runtime Pausing
	//
	void DoPause(WIT wit, long dts)			// Make the movie pause during run.
			{ _wit = wit; _dts = dts; }

	//
	// Material stuff
	//
	bool FInsertMtrl(PMTRL pmtrl, PTAG ptag);	// Inserts a material into this movie.


	//
	// 3-D Text stuff
	//
	bool FInsTdt(PSTN pstn, long tdts, PTAG ptagTdf); // Inserts a TDT into this movie.
	bool FChangeActrTdt(PACTR pactr, PSTN pstn, long tdts, PTAG ptagTdf);

	//
	// Marking (overridden DOCB methods)
	//
	virtual bool FDirty(void) 							// Has the movie changed since last saved?
		{ return _fAutosaveDirty || _fDirty; }
	virtual void SetDirty(bool fDirty = fTrue)			// Mark the movie as changed.
		{ _fAutosaveDirty = fDirty; }

	//
	// Roll call
	//
	bool FAddToRollCall(ACTR *pactr, PSTN pstn);// Add an actor to the roll call
	void RemFromRollCall(ACTR *pactr, bool fDelIfOnlyRef = fFalse); // Remove an actor from the roll call.
	void BuildActionMenu(void);					// Called when the selected actor has changed.

	//
	// Overridden DOCB functions
	//
	PDDG PddgNew(PGCB pgcb);					// For creating a view on a movie.
	bool FAddUndo(PMUNB pmunb);					// Add an item to the undo list
	void ClearUndo(void);

	//
	// Accessors for MVUs only.
	//
#ifdef DEBUG
	void SetFPlaying(bool f) { _fPlaying = f; if (!f) SetFWriteBmps(fFalse); }	// Set the playing flag.
#else // DEBUG
	void SetFPlaying(bool f) { _fPlaying = f; }	// Set the playing flag.
#endif // !DEBUG
	void SetFStopPlaying(bool f) { _fStopPlaying = f; } // INTERNAL USE ONLY
	bool FStopPlaying(void) { return(_fStopPlaying); }  // INTERNAL USE ONLY
	bool FPlaying(void) { return _fPlaying; }	// Query the playing flag.
	PCLOK Pclok() { return &_clok; }			// For getting the clock for playing
	void SetFIdleSeen(bool fIdle) { _fIdleSeen = fIdle; }
	bool FIdleSeen(void) { return _fIdleSeen; }

	//
	// Accessor for getting to the Brender world.
	//
	PBWLD Pbwld(void) { return _pbwld; }

	//
	// Frame rate information
	//
	long Cnfrm(void) { return _cnfrm; }
	ulong TsStart(void) { return _tsStart; }

	//
	// Thumbnail stuff
	//
	PGL PglclrThumbPalette(void) { AssertThis(0); return _pglclrThumbPalette; }
	void SetThumbPalette(PGL pglclr)
		{ ReleasePpo(&_pglclrThumbPalette); _pglclrThumbPalette = pglclr; pglclr->AddRef(); }

	};


#endif // !MOVIE_H
