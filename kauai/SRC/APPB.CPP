/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Common base application class methods.

***************************************************************************/
#include "frame.h"
ASSERTNAME


PAPPB vpappb;
PCEX vpcex;
PSNDM vpsndm;


// basic commands common to most apps
BEGIN_CMD_MAP(APPB, CMH)
	ON_CID_GEN(cidQuit, &APPB::FCmdQuit, pvNil)
	ON_CID_GEN(cidShowClipboard, &APPB::FCmdShowClipboard, &APPB::FEnableAppCmd)
	ON_CID_GEN(cidChooseWnd, &APPB::FCmdChooseWnd, &APPB::FEnableAppCmd)
	#ifdef MAC
		ON_CID_GEN(cidOpenDA, &APPB::FCmdOpenDA, pvNil)
	#endif //MAC
	ON_CID_GEN(cidIdle, &APPB::FCmdIdle, pvNil)
	ON_CID_GEN(cidEndModal, &APPB::FCmdEndModal, pvNil)
END_CMD_MAP_NIL()


RTCLASS(APPB)


/***************************************************************************
	Constructor for the app class.  Assumes that the block is initially
	zeroed.  This implies that the block has to either be allocated
	(using NewObj) or a global.
***************************************************************************/
APPB::APPB(void) : CMH(khidApp)
{
	AssertBaseThis(0);

	vpappb = this;
	Debug( _fCheckForLostMem = fTrue; )
	_onnDefFixed = _onnDefVariable = onnNil;

	AssertThis(0);
}


/***************************************************************************
	Destructor for the app.  Assumes we don't have to free anything.
***************************************************************************/
APPB::~APPB(void)
{
	vpappb = pvNil;
}


/***************************************************************************
	Calls _FInit and if successful, calls _Loop then _CleanUp.
***************************************************************************/
void APPB::Run(ulong grfapp, ulong grfgob, long ginDef)
{
	AssertThis(0);

	if (_FInit(grfapp, grfgob, ginDef))
		_Loop();

	_CleanUp();
}


/***************************************************************************
	Quit routine.  May or may not initiate the quit sequence (depending
	on user input).
***************************************************************************/
void APPB::Quit(bool fForce)
{
	AssertThis(0);

	if (_fQuit || DOCB::FQueryCloseAll(fForce ? fdocForceClose : fdocNil) ||
			fForce)
		{
		_fQuit = fTrue;
		}
}


/***************************************************************************
	Return a default app name.
***************************************************************************/
void APPB::GetStnAppName(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	*pstn = PszLit("Generic");
}


/***************************************************************************
	Sets the cursor. Increments the reference count on the cursor. If
	fLongOp is true, the cursor will get used as the wait cursor, but
	won't necessarily be displayed immediately.
***************************************************************************/
void APPB::SetCurs(PCURS pcurs, bool fLongOp)
{
	AssertThis(0);
	AssertNilOrPo(pcurs, 0);

	PCURS *ppcurs = fLongOp ? &_pcursWait : &_pcurs;

	if (*ppcurs == pcurs)
		return;

	SwapVars(ppcurs, &pcurs);
	if (pvNil != *ppcurs)
		(*ppcurs)->AddRef();

	// set the new one before we release the old one.
	RefreshCurs();
	ReleasePpo(&pcurs);
}


/***************************************************************************
	Set the indicated cursor as the current one.
***************************************************************************/
void APPB::SetCursCno(PRCA prca, CNO cno, bool fLongOp)
{
	AssertThis(0);
	AssertPo(prca, 0);

	PCURS pcurs;

	if (pvNil == (pcurs = (PCURS)prca->PbacoFetch(kctgCursor,
			cno, CURS::FReadCurs)))
		{
		Warn("cursor not found");
		return;
		}
	SetCurs(pcurs, fLongOp);
	ReleasePpo(&pcurs);
}


/***************************************************************************
	Make sure the current cursor is being used by the system.
***************************************************************************/
void APPB::RefreshCurs(void)
{
	AssertThis(0);

	PCURS *ppcurs = _cactLongOp > 0 ? &_pcursWait : &_pcurs;

	if (pvNil != *ppcurs)
		(*ppcurs)->Set();
	else
		{
#ifdef WIN
		SetCursor(LoadCursor(hNil,
			_cactLongOp > 0 ? IDC_WAIT : IDC_ARROW));
#endif //WIN
#ifdef MAC
		HCURS hcurs;

		if (_cactLongOp > 0 && hNil != (hcurs = GetCursor(watchCursor)))
			SetCursor(*hcurs);
		else
			SetCursor(&qd.arrow);
#endif //MAC
		}
}


/***************************************************************************
	Starting a long operation, put up the wait cursor.
***************************************************************************/
void APPB::BeginLongOp(void)
{
	AssertThis(0);

	if (_cactLongOp++ == 0)
		RefreshCurs();
}


/***************************************************************************
	Done with a long operation. Decrement the long op count and if it
	becomes zero, use the normal cursor. If fAll is true, set the
	long op count to 0.
***************************************************************************/
void APPB::EndLongOp(bool fAll)
{
	AssertThis(0);

	if (_cactLongOp == 0)
		return;

	if (fAll)
		_cactLongOp = 0;
	else
		_cactLongOp--;

	if (_cactLongOp == 0)
		RefreshCurs();
}


/***************************************************************************
	Get the current cursor/modifier state.  If fAsync is set, the key state
	returned is the actual current values at the hardware level, ie, not
	synchronized with the command stream.
***************************************************************************/
ulong APPB::GrfcustCur(bool fAsync)
{
	AssertThis(0);

	#ifdef WIN
		short (*pfnT)(int);

		pfnT = fAsync ? GetAsyncKeyState : GetKeyState;
		_grfcust &= ~kgrfcustUser;
		if (pfnT(VK_CONTROL) < 0)
			_grfcust |= fcustCmd;
		if (pfnT(VK_SHIFT) < 0)
			_grfcust |= fcustShift;
		if (pfnT(VK_MENU) < 0)
			_grfcust |= fcustOption;
		if (pfnT(VK_LBUTTON) < 0)
			_grfcust |= fcustMouse;
	#endif //WIN
	#ifdef MAC
		Assert(!fAsync, "Unimplemented code"); //REVIEW shonk: Mac: implement
	#endif //MAC

	return _grfcust;
}


/***************************************************************************
	Modify the current cursor/modifier state.  Doesn't affect the key
	states or mouse state.
***************************************************************************/
void APPB::ModifyGrfcust(ulong grfcustOr, ulong grfcustXor)
{
	AssertThis(0);

	grfcustOr &= ~kgrfcustUser;
	grfcustXor &= ~kgrfcustUser;

	_grfcust |= grfcustOr;
	_grfcust ^= grfcustXor;
}


/***************************************************************************
	Hide the cursor
***************************************************************************/
void APPB::HideCurs(void)
{
	AssertThis(0);

	MacWin( HideCursor(), ShowCursor(fFalse) );
}


/***************************************************************************
	Show the cursor
***************************************************************************/
void APPB::ShowCurs(void)
{
	AssertThis(0);

	MacWin( ShowCursor(), ShowCursor(fTrue) );
}


/***************************************************************************
	Warp the cursor to (xpScreen, ypScreen)
***************************************************************************/
void APPB::PositionCurs(long xpScreen, long ypScreen)
{
	AssertThis(0);

	// REVIEW shonk: implement on Mac
	MacWin( RawRtn(), SetCursorPos(xpScreen, ypScreen) );
}


/***************************************************************************
	Return the default variable pitch font.
***************************************************************************/
long APPB::OnnDefVariable(void)
{
	AssertThis(0);

	if (_onnDefVariable == onnNil)
		{
		STN stn;

		stn = MacWin( PszLit("New York"), PszLit("Times New Roman") );
		if (!vntl.FGetOnn(&stn, &_onnDefVariable))
			_onnDefVariable = vntl.OnnSystem();
		}
	return _onnDefVariable;
}


/***************************************************************************
	Return the default fixed pitch font.
***************************************************************************/
long APPB::OnnDefFixed(void)
{
	AssertThis(0);

	if (_onnDefFixed == onnNil)
		{
		STN stn;

		stn = MacWin( PszLit("Courier"), PszLit("Courier New") );
		if (!vntl.FGetOnn(&stn, &_onnDefFixed))
			{
			// just use the first fixed pitch font
			for (_onnDefFixed = 0; ; _onnDefFixed++)
				{
				if (_onnDefFixed >= vntl.OnnMac())
					{
					_onnDefFixed = vntl.OnnSystem();
					break;
					}
				if (vntl.FFixedPitch(_onnDefFixed))
					break;
				}
			}
		}

	return _onnDefFixed;
}


/***************************************************************************
	Static method to return the default text size.
	REVIEW shonk: DypTextDef: what's the right way to do this?
***************************************************************************/
long APPB::DypTextDef(void)
{
	AssertThis(0);

	return 12;
}


/***************************************************************************
	Quit the app (don't force it).
***************************************************************************/
bool APPB::FCmdQuit(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	Quit(fFalse);
	return fTrue;
}


/***************************************************************************
	Open a window onto the clipboard, if it exists.
***************************************************************************/
bool APPB::FCmdShowClipboard(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	vpclip->Show();
	return fTrue;
}


/***************************************************************************
	Handles an idle command.
***************************************************************************/
bool APPB::FCmdIdle(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	static long _cactIdle = 0;
	PGOB pgob;

	if (_fQuit)
		return fFalse;

	++_cactIdle;

#ifdef DEBUG
	if ((_cactIdle & 0x00FF) == 0 && _fCheckForLostMem && _cactModal <= 0)
		{
		UnmarkAllMem();
		UnmarkAllObjs();
		MarkMem();			//marks all frame-work memory
		MarkUtilMem();		//marks all util memory
		AssertUnmarkedObjs();
		AssertUnmarkedMem();
		}
#endif //DEBUG

	if ((_cactIdle & 0x0F) == 1 && pvNil != (pgob = GOB::PgobScreen()))
		{
		//check to see if the mouse moved
		PT pt;
		bool fDown;
		ulong grfcust;
		CMD_MOUSE cmd;

		pgob->GetPtMouse(&pt, &fDown);
		if (fDown)
			return fTrue;

		pgob->MapPt(&pt, cooLocal, cooGlobal);
		pgob = GOB::PgobFromPtGlobal(pt.xp, pt.yp, &pt);
		grfcust = GrfcustCur();
		if (pgob != _pgobMouse || pt.xp != _xpMouse || pt.yp != _ypMouse ||
				_grfcustMouse != grfcust)
			{
			if (_pgobMouse != pgob)
				{
				_tsMouse = 0;
				if (pvNil != _pgobMouse)
					{
					AssertPo(_pgobMouse, 0);
					vpcex->EnqueueCid(cidRollOff, _pgobMouse);
					_TakeDownToolTip();
					}
				_tsMouseEnter = TsCurrent();
				_pgobMouse = pgob;
				}
			_xpMouse = pt.xp;
			_ypMouse = pt.yp;
			_grfcustMouse = grfcust;

			ClearPb(&cmd, size(cmd));
			cmd.cid = cidMouseMove;
			cmd.pcmh = pgob;
			cmd.xp = pt.xp;
			cmd.yp = pt.yp;
			cmd.grfcust = grfcust;
			vpcex->EnqueueCmd((PCMD)&cmd);
			}

		// adjust tool tips
		if (pvNil != _pgobMouse &&
				(_fToolTip || TsCurrent() - _tsMouseEnter > _dtsToolTip))
			{
			_EnsureToolTip();
			}
		}

	// Flush the sound manager occasionally to free up idle memory
	if (pvNil != vpsndm && (_cactIdle & 0x003F) == 3)
		vpsndm->Flush();

	return fTrue;
}


/***************************************************************************
	Make sure no tool tip is up.
***************************************************************************/
void APPB::_TakeDownToolTip(void)
{
	AssertThis(0);
	PGOB pgob;

	if (pvNil != (pgob = GOB::PgobFromHidScr(khidToolTip)))
		{
		if (pgob == _pgobMouse)
			{
			Bug("how did a tooltip become _pgobMouse???");
			_pgobMouse = pvNil;
			}
		ReleasePpo(&pgob);
		_pgobToolTipTarget = pvNil;
		}
}


/***************************************************************************
	Make sure a tool tip is up, if the current gob wants one.
***************************************************************************/
void APPB::_EnsureToolTip(void)
{
	AssertThis(0);
	PGOB pgob;

	if (pvNil == _pgobMouse)
		return;

	if (_pgobToolTipTarget == _pgobMouse)
		return;

	pgob = GOB::PgobFromHidScr(khidToolTip);
	_fToolTip = FPure(_pgobMouse->FEnsureToolTip(&pgob, _xpMouse, _ypMouse));
	if (!_fToolTip)
		{
		ReleasePpo(&pgob);
		_pgobToolTipTarget = pvNil;
		}
	else
		_pgobToolTipTarget = _pgobMouse;
}


/***************************************************************************
	Take down any existing tool tip and resest tool tip timing.
***************************************************************************/
void APPB::ResetToolTip(void)
{
	AssertThis(0);

	_TakeDownToolTip();
	_fToolTip = fFalse;
	_tsMouseEnter = TsCurrent();
}


/***************************************************************************
	Enable app level commands
***************************************************************************/
bool APPB::FEnableAppCmd(PCMD pcmd, ulong *pgrfeds)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	AssertVarMem(pgrfeds);

	*pgrfeds = fedsEnable;
	switch (pcmd->cid)
		{
	case cidShowClipboard:
		if (vpclip->FDocIsClip(pvNil))
			goto LDisable;
		break;

	case cidChooseWnd:
		if ((HWND)pcmd->rglw[0] == GOB::HwndMdiActive())
			*pgrfeds |= fedsCheck;
		else
			*pgrfeds |= fedsUncheck;
		break;

	default:
		BugVar("unhandled cid in FEnableAppCmd", &pcmd->cid);
LDisable:
		*pgrfeds = fedsDisable;
		break;
		}

	return fTrue;
}


/***************************************************************************
	Respond to a cidChooseWnd command.
***************************************************************************/
bool APPB::FCmdChooseWnd(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	GOB::MakeHwndActive((HWND)pcmd->rglw[0]);
	return fTrue;
}


/***************************************************************************
	Application initialization.
***************************************************************************/
bool APPB::_FInit(ulong grfapp, ulong grfgob, long ginDef)
{
	AssertThis(0);

	_fOffscreen = FPure(grfapp & fappOffscreen);

#ifdef DEBUG
	if (!_FInitDebug())
		return fFalse;
#endif

	// initialize the command dispatcher
	if (pvNil == (vpcex = CEX::PcexNew(20, 20)))
		return fFalse;

	// add the app as a handler (so it can catch menu commands)
	if (!vpcex->FAddCmh(vpappb, kcmhlAppb))
		return fFalse;

	// do OS specific initialization
	if (!_FInitOS())
		return fFalse;

	// Initialize the graphics stuff.
	if (!FInitGfx())
		return fFalse;

	// set up the menus
	if (!_FInitMenu())
		return fFalse;

	// initialize the screen gob
	if (!GOB::FInitScreen(grfgob, ginDef))
		return fFalse;

	// initialize sound functionality
	if (!_FInitSound(kwav22M16))
		return fFalse;

	// import any external clipboard
	vpclip->Import();

	return fTrue;
}


/***************************************************************************
	Initialize the sound manager.  Default is to return true whether or not
	we could create the sound manager.
***************************************************************************/
bool APPB::_FInitSound(long wav)
{
	AssertBaseThis(0);
	PSNDV psndv;

	if (pvNil != vpsndm)
		return fTrue;

	// create the Sound manager
	if (pvNil == (vpsndm = SNDM::PsndmNew()))
		return fTrue;

	if (pvNil != (psndv = SDAM::PsdamNew(wav)))
		{
		vpsndm->FAddDevice(kctgWave, psndv);
		ReleasePpo(&psndv);
		}

	// create the midi playback device - use the stream one
	if (pvNil != (psndv = MDPS::PmdpsNew()))
		{
		vpsndm->FAddDevice(kctgMidi, psndv);
		ReleasePpo(&psndv);
		}

	return fTrue;
}


/***************************************************************************
	Standard menu initialization.  Just loads menu number 128.
***************************************************************************/
bool APPB::_FInitMenu(void)
{
	AssertThis(0);

	return MUB::PmubNew(128) != pvNil;
}


/***************************************************************************
	Main program loop.
***************************************************************************/
void APPB::_Loop(void)
{
	AssertThis(0);

	EVT evt;
	Assert(chNil == 0 && vkNil == 0, "nil key events wrong");

	while (!_fQuit && (!_fEndModal || _cactModal <= 0))
		{
		// do top of the loop stuff
		TopOfLoop();

		// internal commands have priority
		if (vpcex->FDispatchNextCmd())
			continue;

		// handle system events
		if (_FGetNextEvt(&evt))
			_DispatchEvt(&evt);
		else
			{
			// nothing to do, so enqueue some idle commands
			vpcex->EnqueueCid(cidSelIdle, pvNil, pvNil, _fForeground);
			vpcex->EnqueueCid(cidIdle);
			}
		}
}


#ifdef WIN
STDAPI_(int) DetectLeaks(BOOL fDebugOut, BOOL fMessageBox);
#endif //WIN

/***************************************************************************
	Clean up routine for the app base class.
***************************************************************************/
void APPB::_CleanUp(void)
{
	AssertThis(0);

	PSNDM psndm;

	if (pvNil != vpsndm)		
		{
		// do this so if we pop into the debugger, or whatever while releasing
		// vpsndm, we don't deactivate the sound manager.
		psndm = vpsndm;
		vpsndm = pvNil;
		// deactivate the sndm to release all devices
		psndm->Activate(fFalse);
		ReleasePpo(&psndm);
		}

#ifdef WIN
#ifdef DEBUG
	// check for Audioman memory leaks
	DetectLeaks(fTrue, fFalse);
#endif //DEBUG
#endif //WIN

	GOB::ShutDown();
	FIL::ShutDown();
#ifdef WIN
	_ShutDownViewer();
#endif //WIN
}


/***************************************************************************
	Activate or deactivate the application.
***************************************************************************/
void APPB::_Activate(bool fActive)
{
	AssertThis(0);

	if (pvNil != vpsndm)
		vpsndm->Activate(fActive);
	_fForeground = FPure(fActive);
}


/***************************************************************************
	This gets called every time through the main app loop.
***************************************************************************/
void APPB::TopOfLoop(void)
{
	AssertThis(0);

#ifdef DEBUG
	if (_fRefresh)
		{
		//need to redraw all our windows - we ignored some paint
		//events while in an assert
		_fRefresh = fFalse;
		GTE gte;
		PGOB pgob;
		ulong grfgte;

		gte.Init(GOB::PgobScreen(), fgteNil);
		while (gte.FNextGob(&pgob, &grfgte, fgteNil))
			{
			if ((grfgte & fgtePre) && pgob->Hwnd() != hNil)
				pgob->InvalRc(pvNil);
			}
		}
#endif //DEBUG

	// update any marked stuff
	UpdateMarked();

	// take down the wait cursor
	EndLongOp(fTrue);
}


/***************************************************************************
	Update the given window.  *prc is the bounding rectangle of the update
	region.
***************************************************************************/
void APPB::UpdateHwnd(HWND hwnd, RC *prc, ulong grfapp)
{
	AssertThis(0);
	Assert(hNil != hwnd, "nil hwnd in UpdateHwnd");
	AssertVarMem(prc);

	PGOB pgob;
	PGPT pgpt = pvNil;

	if (pvNil == (pgob = GOB::PgobFromHwnd(hwnd)))
		return;

#ifdef DEBUG
	if (_fInAssert)
		{
		// don't do the update, just set _fRefresh so we'll invalidate
		// everything the next time thru the main loop.
		_fRefresh = fTrue;
		return;
		}
#endif //DEBUG

	if ((grfapp & fappOffscreen) || (_fOffscreen && !(grfapp & fappOnscreen)))
		{
		//do offscreen drawing
		pgpt = _PgptEnsure(prc);
		}

	//NOTE: technically we should map from hwnd to local coordinates
	//but they are the same for an hwnd based gob.
	pgob->DrawTree(pgpt, pvNil, prc, fgobUseVis);

	if (pvNil != pgpt)
		{
		// put the image on the screen
		GNV gnv(pgob);
		GNV gnvSrc(pgpt);

		gnv.CopyPixels(&gnvSrc, prc, prc);
		}
}


/***************************************************************************
	Map a handler id to a handler.
***************************************************************************/
PCMH APPB::PcmhFromHid(long hid)
{
	AssertThis(0);
	PCMH pcmh;

	switch (hid)
		{
	case hidNil:
		return pvNil;

	case khidApp:
		return this;
		}

	if (pvNil != (pcmh = CLOK::PclokFromHid(hid)))
		return pcmh;
	return GOB::PgobFromHidScr(hid);
}


/***************************************************************************
	The command handler is dying - take it out of any lists it's in.
***************************************************************************/
void APPB::BuryCmh(PCMH pcmh)
{
	AssertThis(0);
	long imodcx;
	MODCX modcx;

	// NOTE: don't do an AssertPo(pcmh, 0)!
	Assert(pvNil != pcmh, 0);

	if (pvNil != vpcex)
		vpcex->BuryCmh(pcmh);

	if (pvNil != _pglmodcx && 0 < (imodcx = _pglmodcx->IvMac()))
		{
		while (imodcx-- > 0)
			{
			_pglmodcx->Get(imodcx, &modcx);
			AssertPo(modcx.pcex, 0);
			modcx.pcex->BuryCmh(pcmh);
			}
		}

	CLOK::BuryCmh(pcmh);
	if (pcmh == _pgobMouse)
		{
		_pgobMouse = pvNil;
		_TakeDownToolTip();
		}
}


/***************************************************************************
	Mark the rectangle for a fast update.  The rectangle is given in
	pgobCoo coordinates.  If prc is nil, the entire rectangle for pgobCoo
	is used.
***************************************************************************/
void APPB::MarkRc(RC *prc, PGOB pgobCoo)
{
	AssertThis(0);
	AssertNilOrVarMem(prc);
	AssertPo(pgobCoo, 0);

	_MarkRegnRc(pvNil, prc, pgobCoo);
}


/***************************************************************************
	Mark a region dirty.
***************************************************************************/
void APPB::MarkRegn(PREGN pregn, PGOB pgobCoo)
{
	AssertThis(0);
	AssertNilOrPo(pregn, 0);
	AssertPo(pgobCoo, 0);

	_MarkRegnRc(pregn, pvNil, pgobCoo);
}


/***************************************************************************
	Mark the rectangle for a fast update.  The rectangle is given in
	pgobCoo coordinates.  If prc is nil, the entire rectangle for pgobCoo
	is used.
***************************************************************************/
void APPB::_MarkRegnRc(PREGN pregn, RC *prc, PGOB pgobCoo)
{
	AssertThis(0);
	AssertNilOrPo(pregn, 0);
	AssertNilOrVarMem(prc);
	AssertPo(pgobCoo, 0);

	MKRGN mkrgn;
	long imkrgn;
	RC rc;
	PT pt;
	HWND hwnd;

	//get the offset
	pgobCoo->GetRc(&rc, cooHwnd);
	pt = rc.PtTopLeft();

	if (pvNil == prc)
		{
		if (pvNil == pregn)
			{
			//use the full rectangle for the GOB
			if (rc.FEmpty())
				return;
			prc = &rc;
			}
		else if (pregn->FEmpty())
			return;
		}
	else
		{
		//offset *prc to hwnd coordinates
		rc.OffsetCopy(prc, pt.xp, pt.yp);
		if (rc.FEmpty() && (pvNil == pregn || pregn->FEmpty()))
			return;
		prc = &rc;
		}

	//offset the region to hwnd coordinates
	if (pvNil != pregn)
		pregn->Offset(pt.xp, pt.yp);
	hwnd = pgobCoo->HwndContainer();

	if (pvNil == _pglmkrgn)
		{
		if (pvNil == (_pglmkrgn = GL::PglNew(size(MKRGN))))
			goto LFail;
		}
	else
		{
		for (imkrgn = _pglmkrgn->IvMac(); imkrgn-- > 0; )
			{
			_pglmkrgn->Get(imkrgn, &mkrgn);
			if (mkrgn.hwnd == hwnd)
				{
				//already something marked dirty, union in the new stuff
				if (pvNil != prc && !mkrgn.pregn->FUnionRc(prc))
					goto LFail;
				if (pvNil != pregn && !mkrgn.pregn->FUnion(pregn))
					goto LFail;
				goto LDone;
				}
			}
		}

	//create a new entry
	mkrgn.hwnd = hwnd;
	if (pvNil == (mkrgn.pregn = REGN::PregnNew(prc)) ||
			pvNil != pregn && !mkrgn.pregn->FUnion(pregn) ||
			!_pglmkrgn->FPush(&mkrgn))
		{
		ReleasePpo(&mkrgn.pregn);
LFail:
		Warn("marking failed");
		pgobCoo->InvalRc(pvNil, kginSysInval);
		}

LDone:
	//put the region back the way it was
	if (pvNil != pregn)
		pregn->Offset(-pt.xp, -pt.yp);
}


/***************************************************************************
	Unmark the rectangle for a fast update.  The rectangle is given in
	pgobCoo coordinates.  If prc is nil, the entire rectangle for pgobCoo
	is used.
***************************************************************************/
void APPB::UnmarkRc(RC *prc, PGOB pgobCoo)
{
	AssertThis(0);
	AssertNilOrVarMem(prc);
	AssertPo(pgobCoo, 0);

	_UnmarkRegnRc(pvNil, prc, pgobCoo);
}


/***************************************************************************
	Mark a region clean.
***************************************************************************/
void APPB::UnmarkRegn(PREGN pregn, PGOB pgobCoo)
{
	AssertThis(0);
	AssertNilOrPo(pregn, 0);
	AssertPo(pgobCoo, 0);

	_UnmarkRegnRc(pregn, pvNil, pgobCoo);
}


/***************************************************************************
	Unmark the rectangle for a fast update.  The rectangle is given in
	pgobCoo coordinates.  If prc is nil, the entire rectangle for pgobCoo
	is used.
***************************************************************************/
void APPB::_UnmarkRegnRc(PREGN pregn, RC *prc, PGOB pgobCoo)
{
	AssertNilOrPo(pregn, 0);
	AssertNilOrVarMem(prc);
	AssertPo(pgobCoo, 0);

	MKRGN mkrgn;
	long imkrgn;
	RC rc;
	PT pt;
	HWND hwnd;

	if (pvNil == _pglmkrgn || _pglmkrgn->IvMac() == 0)
		return;

	//get the mkrgn and imkrgn
	hwnd = pgobCoo->HwndContainer();
	for (imkrgn = _pglmkrgn->IvMac(); ; )
		{
		if (imkrgn-- <= 0)
			return;
		_pglmkrgn->Get(imkrgn, &mkrgn);
		if (mkrgn.hwnd == hwnd)
			break;
		}

	//get the offset
	pgobCoo->GetRc(&rc, cooHwnd);
	pt = rc.PtTopLeft();

	if (pvNil == prc)
		{
		if (pvNil == pregn)
			{
			//use the full rectangle for the GOB
			if (rc.FEmpty())
				return;
			prc = &rc;
			}
		else if (pregn->FEmpty())
			return;
		}
	else
		{
		//offset *prc to hwnd coordinates
		rc.OffsetCopy(prc, pt.xp, pt.yp);
		if (rc.FEmpty() && (pvNil == pregn || pregn->FEmpty()))
			return;
		prc = &rc;
		}

	if (pvNil != pregn)
		{
		pregn->Offset(pt.xp, pt.yp);
		mkrgn.pregn->FDiff(pregn);
		pregn->Offset(-pt.xp, -pt.yp);
		}
	if (pvNil != prc)
		mkrgn.pregn->FDiffRc(prc);

	if (mkrgn.pregn->FEmpty())
		{
		ReleasePpo(&mkrgn.pregn);
		_pglmkrgn->Delete(imkrgn);
		}
}


/***************************************************************************
	Get the bounding rectangle of any marked portion of the given hwnd.
***************************************************************************/
bool APPB::FGetMarkedRc(HWND hwnd, RC *prc)
{
	AssertThis(0);
	Assert(hNil != hwnd, "bad hwnd");
	AssertVarMem(prc);

	MKRGN mkrgn;
	long imkrgn;

	if (pvNil != _pglmkrgn)
		{
		//get the mkrgn and imkrgn
		for (imkrgn = _pglmkrgn->IvMac(); imkrgn-- > 0; )
			{
			_pglmkrgn->Get(imkrgn, &mkrgn);
			if (mkrgn.hwnd == hwnd)
				return !mkrgn.pregn->FEmpty(prc);
			}
		}

	prc->Zero();
	return fFalse;
}


/***************************************************************************
	If there is a marked region for this HWND, remove it from the list
	and invalidate it.  This is called when we get a system paint/update
	event.
***************************************************************************/
void APPB::InvalMarked(HWND hwnd)
{
	AssertThis(0);
	Assert(hNil != hwnd, "bad hwnd");

	long imkrgn;
	MKRGN mkrgn;
	RC rc;
	RCS rcs;

	if (pvNil == _pglmkrgn)
		return;

	for (imkrgn = _pglmkrgn->IvMac(); imkrgn-- > 0; )
		{
		_pglmkrgn->Get(imkrgn, &mkrgn);
		if (mkrgn.hwnd == hwnd)
			{
			if (!mkrgn.pregn->FEmpty(&rc))
				{
				rcs = RCS(rc);
				InvalHwndRcs(hwnd, &rcs);
				}
			ReleasePpo(&mkrgn.pregn);
			_pglmkrgn->Delete(imkrgn);
			}
		}
}


/***************************************************************************
	Update all marked regions.
***************************************************************************/
void APPB::UpdateMarked(void)
{
	AssertThis(0);

	MKRGN mkrgn;
	PGOB pgob;

	if (pvNil == _pglmkrgn)
		return;

	while (_pglmkrgn->FPop(&mkrgn))
		{
		if (pvNil != (pgob = GOB::PgobFromHwnd(mkrgn.hwnd)))
			_FastUpdate(pgob, mkrgn.pregn);
		ReleasePpo(&mkrgn.pregn);
		}
}


/***************************************************************************
	Do a fast update of the gob and its descendents into the given gpt.
***************************************************************************/
void APPB::_FastUpdate(PGOB pgob, PREGN pregnClip, ulong grfapp, PGPT pgpt)
{
	AssertThis(0);
	AssertPo(pgob, 0);
	AssertPo(pregnClip, 0);
	AssertNilOrPo(pgpt, 0);

	RC rc, rcT;
	bool fOffscreen;

	if (pregnClip->FEmpty(&rc))
		return;

	pgob->GetRc(&rcT, cooLocal);
	if (!rc.FIntersect(&rcT))
		return;

	fOffscreen = (FPure(grfapp & fappOffscreen) ||
			(_fOffscreen && !(grfapp & fappOnscreen))) &&
		pvNil == pgpt && hNil != pgob->Hwnd() &&
        pvNil != (pgpt = _PgptEnsure(&rc));

	pgob->DrawTreeRgn(pgpt, pvNil, pregnClip, fgobUseVis);

	if (fOffscreen)
		{
		//copy the stuff to the screen
		GNV gnvOff(pgpt);
		GNV gnv(pgob);
		PGPT pgptDst = pgob->Pgpt();

		pgptDst->ClipToRegn(&pregnClip);
		_CopyPixels(&gnvOff, &rc, &gnv, &rc);
		pgptDst->ClipToRegn(&pregnClip);
		GPT::Flush();
		}
}


/***************************************************************************
	Set the transition to apply the next time we do offscreen fast updating.
	gft is the transition type.  The meaning of lwGft depends on the
	transition.  dts is how long each phase of the transition should take.
	pglclr is the new palette (may be nil).  acr is an intermediary color
	to transition to.  If acr is clear, no intermediary transition is done.

	If pglclr is not nil, this AddRef's it and holds onto it until after
	the transition is done.
***************************************************************************/
void APPB::SetGft(long gft, long lwGft, ulong dts, PGL pglclr, ACR acr)
{
	AssertThis(0);
	AssertNilOrPo(pglclr, 0);
	AssertPo(&acr, 0);

	_gft = gft;
	_lwGft = lwGft;
	_dtsGft = dts;
	ReleasePpo(&_pglclr);
	if (pvNil != pglclr)
		{
		_pglclr = pglclr;
		_pglclr->AddRef();
		}
	_acr = acr;
}


/***************************************************************************
	Copy pixels from an offscreen buffer (pgnvSrc, prcSrc) to the screen
	(pgnvDst, prcDst).  This gives the app a chance to do any transition
	affects they want.
***************************************************************************/
void APPB::_CopyPixels(PGNV pgnvSrc, RC *prcSrc, PGNV pgnvDst, RC *prcDst)
{
	AssertThis(0);
	AssertPo(pgnvSrc, 0);
	AssertVarMem(prcSrc);
	AssertPo(pgnvDst, 0);
	AssertVarMem(prcDst);

	switch (_gft)
		{
	default:
		pgnvDst->CopyPixels(pgnvSrc, prcSrc, prcDst);
		break;

	case kgftWipe:
		pgnvDst->Wipe(_lwGft, _acr, pgnvSrc, prcSrc, prcDst, _dtsGft, _pglclr);
		break;

	case kgftSlide:
		pgnvDst->Slide(_lwGft, _acr, pgnvSrc, prcSrc, prcDst, _dtsGft, _pglclr);
		break;

	case kgftDissolve:
		// high word of _lwGft is number for columns, low word is number of rows
		// of the dissolve grid.  If one or both is zero, the dissolve is done
		// at the pixel level offscreen.
		pgnvDst->Dissolve(SwHigh(_lwGft), SwLow(_lwGft), _acr,
			pgnvSrc, prcSrc, prcDst, _dtsGft, _pglclr);
		break;

	case kgftFade:
		pgnvDst->Fade(_lwGft, _acr, pgnvSrc, prcSrc, prcDst, _dtsGft, _pglclr);
		break;

	case kgftIris:
		// top 15 bits are the xp value, next 15 bits are the (signed) yp value,
		// bottom 2 bits are the gfd.
		pgnvDst->Iris(_lwGft & 0x03, _lwGft >> 17, (_lwGft << 15) >> 17, _acr,
			pgnvSrc, prcSrc, prcDst, _dtsGft, _pglclr);
		break;
		}

	_gft = gftNil;
	ReleasePpo(&_pglclr);
}


/***************************************************************************
	Get an offscreen GPT big enough to enclose the given rectangle.
	Should minimize reallocations.  Doesn't increment a ref count.
	APPB maintains ownership of the GPT.
***************************************************************************/
PGPT APPB::_PgptEnsure(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	RC rc;

	if (prc->Dxp() > _dxpOff || prc->Dyp() > _dypOff)
		{
		ReleasePpo(&_pgptOff);
		rc.Set(0, 0, LwMax(prc->Dxp(), _dxpOff), LwMax(prc->Dyp(), _dypOff));
		_pgptOff = GPT::PgptNewOffscreen(&rc, 8);
		if (pvNil != _pgptOff)
			{
			_dxpOff = rc.Dxp();
			_dypOff = rc.Dyp();
			}
		else
			_dxpOff = _dypOff = 0;
		}
	if (pvNil != _pgptOff)
		{
		PT pt = prc->PtTopLeft();
		_pgptOff->SetPtBase(&pt);
		}
	return _pgptOff;
}


/***************************************************************************
	See if the given property is in the property list.
***************************************************************************/
bool APPB::_FFindProp(long prid, PROP *pprop, long *piprop)
{
	AssertThis(0);
	AssertNilOrVarMem(pprop);
	AssertNilOrVarMem(piprop);
	long ivMin, ivLim, iv;
	PROP prop;

	if (pvNil == _pglprop)
		{
		if (pvNil != piprop)
			*piprop = 0;
		TrashVar(pprop);
		return fFalse;
		}

	for (ivMin = 0, ivLim = _pglprop->IvMac(); ivMin < ivLim; )
		{
		iv = (ivMin + ivLim) / 2;
		_pglprop->Get(iv, &prop);
		if (prop.prid < prid)
			ivMin = iv + 1;
		else if (prop.prid > prid)
			ivLim = iv;
		else
			{
			if (pvNil != piprop)
				*piprop = iv;
			if (pvNil != pprop)
				*pprop = prop;
			return fTrue;
			}
		}

	if (pvNil != piprop)
		*piprop = ivMin;
	TrashVar(pprop);
	return fFalse;
}


/***************************************************************************
	Set the given property in the property list.
***************************************************************************/
bool APPB::_FSetProp(long prid, long lw)
{
	AssertThis(0);
	PROP prop;
	long iprop;

	if (_FFindProp(prid, &prop, &iprop))
		{
		Assert(prop.prid == prid, 0);
		prop.lw = lw;
		_pglprop->Put(iprop, &prop);
		return fTrue;
		}

	if (pvNil == _pglprop)
		{
		Assert(iprop == 0, 0);
		if (pvNil == (_pglprop = GL::PglNew(size(PROP))))
			return fFalse;
		}

	prop.prid = prid;
	prop.lw = lw;
	return _pglprop->FInsert(iprop, &prop);
}


/***************************************************************************
	Set the indicated property, using the given parameter.
***************************************************************************/
bool APPB::FSetProp(long prid, long lw)
{
	AssertThis(0);

	switch (prid)
		{
	case kpridFullScreen:
		if (FPure(_fFullScreen) == FPure(lw))
			break;
#ifdef WIN
		long lwT;

		// if we're already maximized, we have to restore and maximize
		// to force the system to use our new MINMAXINFO.
		// REVIEW shonk: full screen: is there a better way to do this?
		if (!FGetProp(kpridMaximized, &lwT))
			return fFalse;
		if (lwT)
			{
			if (!FSetProp(kpridMaximized, fFalse))
				return fFalse;
			_fFullScreen = FPure(lw);
			if (!FSetProp(kpridMaximized, fTrue))
				{
				_fFullScreen = !lw;
				return fFalse;
				}
			}
#else //!WIN
		// REVIEW shonk: Mac: implement
#endif //!WIN
		_fFullScreen = FPure(lw);
		break;

	case kpridMaximized:
#ifdef WIN
		if (FPure(IsZoomed(vwig.hwndApp)) == FPure(lw))
			break;
		return FPure(ShowWindow(vwig.hwndApp,
			lw ? SW_SHOWMAXIMIZED : SW_SHOWNORMAL));
#else //!WIN
		// REVIEW shonk: Mac: implement
		return FPure(lw);
#endif //!WIN

	case kpridToolTipDelay:
		lw = LwBound(lw, 0, LwMulDiv(klwMax, kdtimSecond, kdtsSecond));
		_dtsToolTip = LwMulDiv(lw, kdtsSecond, kdtimSecond);
		ResetToolTip();
		break;

	default:
		return _FSetProp(prid, lw);
		}

	return fTrue;
}


/***************************************************************************
	Return the current value of the given property.
***************************************************************************/
bool APPB::FGetProp(long prid, long *plw)
{
	AssertThis(0);
	AssertVarMem(plw);
	PROP prop;

	TrashVar(plw);
	switch (prid)
		{
	case kpridFullScreen:
		*plw = _fFullScreen;
		break;

	case kpridMaximized:
#ifdef WIN
		*plw = FPure(IsZoomed(vwig.hwndApp));
#else //!WIN
		//REVIEW shonk: Mac: implement
		*plw = fTrue;
#endif //!WIN
		break;

	case kpridToolTipDelay:
		*plw = LwMulDivAway(_dtsToolTip, kdtimSecond, kdtsSecond);
		break;

	default:
		if (!_FFindProp(prid, &prop))
			return fFalse;
		*plw = prop.lw;
		break;
		}

	return fTrue;
}


/***************************************************************************
	Import some data in the given clip format to a docb.  If pv is nil
	(or cb is 0), just return whether we can import the format. Otherwise,
	actually create a document and set *ppdocb to point to it. To delay
	import, we create an empty document and set *pfDelay to true. If
	pfDelay is nil, importing cannot be delayed.  If *ppdocb is not nil,
	import into *ppdocb if we can.
***************************************************************************/
bool APPB::FImportClip(long clfm, void *pv, long cb, PDOCB *ppdocb,
	bool *pfDelay)
{
	AssertThis(0);
	AssertPvCb(pv, cb);
	AssertNilOrVarMem(ppdocb);
	AssertNilOrVarMem(pfDelay);
	long cpMac;

	switch (clfm)
		{
	default:
		return fFalse;

	// we can only import these types
	case kclfmText:
		break;
		}

	if (pvNil == pv || 0 == cb || pvNil == ppdocb)
		return fTrue;

	switch (clfm)
		{
	default:
		Bug("unknown clip format");
		return fFalse;

	case kclfmText:
		AssertNilOrPo(*ppdocb, 0);

		if (pvNil == *ppdocb || !(*ppdocb)->FIs(kclsTXTB))
			{
			ReleasePpo(ppdocb);
			if (pvNil == (*ppdocb = TXRD::PtxrdNew()))
				return fFalse;
			}

		// if we can delay and there is more than 1K of text, delay it
		if (pvNil != pfDelay)
			{
			if (cb > 0x0400)
				{
				*pfDelay = fTrue;
				return fTrue;
				}
			*pfDelay = fFalse;
			}

		((PTXTB)(*ppdocb))->SuspendUndo();
		cpMac = ((PTXTB)(*ppdocb))->CpMac();
		((PTXTB)(*ppdocb))->FReplaceRgch(pv, cb / size(achar), 0, cpMac - 1);
		((PTXTB)(*ppdocb))->ResumeUndo();
		return fTrue;
		}
}


/***************************************************************************
	Push the current modal context and create a new one. This should be
	balanced with a call to PopModal (if successful).
***************************************************************************/
bool APPB::FPushModal(PCEX pcex)
{
	AssertThis(0);
	MODCX modcx;
	PUSAC pusacNew = pvNil;

	if (pvNil == _pglmodcx &&
			pvNil == (_pglmodcx = GL::PglNew(size(MODCX))))
		{
		return fFalse;
		}

	modcx.cactLongOp = _cactLongOp;
	modcx.pcex = vpcex;
	modcx.pusac = vpusac;
	modcx.luScale = vpusac->LuScale();

	if (!_pglmodcx->FPush(&modcx))
		return fFalse;

	if (pcex != pvNil)
		pcex->AddRef();

	if (pvNil == pcex && pvNil == (pcex = CEX::PcexNew(20, 20)) ||
			!pcex->FAddCmh(vpappb, kcmhlAppb) ||
			pvNil == (pusacNew = NewObj USAC))
		{
		ReleasePpo(&pcex);
		AssertDo(_pglmodcx->FPop(), 0);
		return fFalse;
		}
	modcx.pcex->Suspend();
	vpcex = pcex;
	pcex->Suspend(fFalse);

	_cactLongOp = 0;
	vpusac->Scale(0);
	vpusac = pusacNew;

	return fTrue;
}


/***************************************************************************
	Go into a modal loop and don't return until _fQuit is set or a
	cidEndModal comes to the app. Normally should be bracketed by an
	FPushModal/PopModal pair. Returns false iff the modal terminated
	abnormally (eg, we're quitting).
***************************************************************************/
bool APPB::FModalLoop(long *plwRet)
{
	AssertThis(0);
	AssertVarMem(plwRet);
	bool fRet;

	_fEndModal = fFalse;
	_cactModal++;
	_Loop();
	_cactModal--;
	fRet = FPure(_fEndModal);
	_fEndModal = fFalse;
	*plwRet = _lwModal;

	AssertThis(0);
	return fRet;
}


/***************************************************************************
	Cause the topmost modal loop to terminate (next time through) with the
	given return value.
***************************************************************************/
void APPB::EndModal(long lwRet)
{
	AssertThis(0);

	_lwModal = lwRet;
	_fEndModal = fTrue;
}


/***************************************************************************
	Pop the topmost modal context.
***************************************************************************/
void APPB::PopModal(void)
{
	AssertThis(0);
	MODCX modcx;

	if (pvNil == _pglmodcx || !_pglmodcx->FPop(&modcx))
		{
		Bug("Unbalanced call to PopModal");
		return;
		}

	SwapVars(&vpcex, &modcx.pcex);
	SwapVars(&vpusac, &modcx.pusac);
	modcx.pcex->Suspend();
	vpcex->Suspend(fFalse);
	vpusac->Scale(modcx.luScale);
	_cactLongOp = modcx.cactLongOp;

	ReleasePpo(&modcx.pcex);
	ReleasePpo(&modcx.pusac);
}


/***************************************************************************
	End the topmost modal loop.
***************************************************************************/
bool APPB::FCmdEndModal(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (_cactModal <= 0)
		return fFalse;

	EndModal(pcmd->rglw[0]);
	return fTrue;
}


/***************************************************************************
	Handle any bad modal commands. Default is to put the command in the
	next modal context's CEX.
***************************************************************************/
void APPB::BadModalCmd(PCMD pcmd)
{
	AssertThis(0);
	AssertPo(pcmd, 0);
	long imodcx;
	MODCX modcx;

	if (pvNil == _pglmodcx || 0 >= (imodcx = _pglmodcx->IvMac()))
		{
		Bug("Don't know what to do with the modal command");
		return;
		}

	_pglmodcx->Get(--imodcx, &modcx);
	if (pvNil != pcmd->pgg)
		pcmd->pgg->AddRef();
	modcx.pcex->EnqueueCmd(pcmd);
}


/***************************************************************************
	Ask the user if they want to save changes to the given doc.
***************************************************************************/
bool APPB::TQuerySaveDoc(PDOCB pdocb, bool fForce)
{
	AssertThis(0);
	STN stn;
	STN stnName;

	pdocb->GetName(&stnName);
	stn.FFormatSz(PszLit("Save changes to \"%s\" before closing?"), &stnName);
	return vpappb->TGiveAlertSz(stn.Psz(), fForce ? bkYesNo : bkYesNoCancel,
		cokQuestion);
}


/***************************************************************************
	Return whether we should allow a screen saver to come up. Defaults
	to returning true.
***************************************************************************/
bool APPB::FAllowScreenSaver(void)
{
	AssertThis(0);

	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a APPB.
***************************************************************************/
void APPB::AssertValid(ulong grf)
{
	APPB_PAR::AssertValid(0);

	AssertNilOrPo(_pgptOff, 0);
	AssertNilOrPo(_pcurs, 0);
	AssertNilOrPo(_pcursWait, 0);
	AssertNilOrPo(_pglclr, 0);
	AssertNilOrPo(_pglprop, 0);
	AssertNilOrPo(_pglmkrgn, 0);
	AssertNilOrPo(_pglmodcx, 0);
}


/***************************************************************************
	Registers memory for frame specific memory (command dispatcher, menu
	bar, screen gobs, etc).
***************************************************************************/
void APPB::MarkMem(void)
{
	AssertThis(0);
	PGOB pgob;

	APPB_PAR::MarkMem();
	MarkMemObj(vpcex);
	MarkMemObj(vpmubCur);
	MarkMemObj(&vntl);
	MarkMemObj(vpclip);
	MarkMemObj(vpsndm);

	MarkMemObj(_pgptOff);
	MarkMemObj(_pcurs);
	MarkMemObj(_pcursWait);
	MarkMemObj(_pglclr);
	MarkMemObj(_pglprop);

	if (pvNil != _pglmkrgn)
		{
		long imkrgn;
		MKRGN mkrgn;

		MarkMemObj(_pglmkrgn);
		for (imkrgn = _pglmkrgn->IvMac(); imkrgn-- > 0; )
			{
			_pglmkrgn->Get(imkrgn, &mkrgn);
			MarkMemObj(mkrgn.pregn);
			}
		}

	if (pvNil != _pglmodcx)
		{
		long imodcx;
		MODCX modcx;

		MarkMemObj(_pglmodcx);
		for (imodcx = _pglmodcx->IvMac(); imodcx-- > 0; )
			{
			_pglmodcx->Get(imodcx, &modcx);
			MarkMemObj(modcx.pcex);
			MarkMemObj(modcx.pusac);
			}
		}

	GPT::MarkStaticMem();
	CLOK::MarkAllCloks();
	if ((pgob = GOB::PgobScreen()) != pvNil)
		pgob->MarkGobTree();
}


/***************************************************************************
	Assert proc - just calls the app's AssertProc.
***************************************************************************/
bool FAssertProc(PSZS pszsFile, long lwLine, PSZS pszsMsg, void *pv, long cb)
{
	if (vpappb == pvNil)
		return fTrue;
	return vpappb->FAssertProcApp(pszsFile, lwLine, pszsMsg, pv, cb);
}


/***************************************************************************
	Warning reporting proc.
***************************************************************************/
void WarnProc(PSZS pszsFile, long lwLine, PSZS pszsMsg)
{
	if (vpappb == pvNil)
		Debugger();
	else
		vpappb->WarnProcApp(pszsFile, lwLine, pszsMsg);
}


static MUTX _mutxWarn;

/***************************************************************************
	Default framework warning proc.
***************************************************************************/
void APPB::WarnProcApp(PSZS pszsFile, long lwLine, PSZS pszsMsg)
{
	static PFIL _pfilWarn;
	static bool _fInWarn;
	static FP _fpCur;
	STN stn;
	STN stnFile;
	STN stnMsg;

	if (_fInWarn)
		return;

	_mutxWarn.Enter();

	_fInWarn = fTrue;
	if (pvNil == _pfilWarn)
		{
		FNI fni;
		FTG ftg;

		//put the warning file at the root of the drive that temp files go on
		if (!fni.FGetTemp() || !fni.FSetLeaf(pvNil, kftgDir))
			goto LDone;
		while (fni.FUpDir(pvNil, ffniMoveToDir))
			;

		stn = PszLit("_Frame_W");
		if (!fni.FSetLeaf(&stn, kftgText))
			goto LDone;

		ftg = FIL::vftgCreator;
		FIL::vftgCreator = 'ttxt';
		_pfilWarn = FIL::PfilCreate(&fni);
		FIL::vftgCreator = ftg;
		if (pvNil == _pfilWarn)
			goto LDone;
		}
	stnFile.SetSzs(pszsFile);
	stnMsg.SetSzs(pszsMsg);

	stn.FFormatSz(PszLit("%s(%d): %s\xD") Win(PszLit("\xA")), &stnFile,
		lwLine, &stnMsg);
	Win( OutputDebugString(stn.Psz()); )
	_pfilWarn->FWriteRgbSeq(stn.Prgch(), stn.Cch(), &_fpCur);

LDone:
	_fInWarn = fFalse;
	_mutxWarn.Leave();
}
#endif //DEBUG

