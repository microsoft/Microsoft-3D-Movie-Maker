/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Mac specific base application class methods.

***************************************************************************/
#include "frame.h"
ASSERTNAME

ulong _GrfcustFromEvt(PEVT pevt);

// by default grow the stack by 8K and call MoreMasters 10 times
const long _cbExtraStackDef = 0x2000L;
const long _cactMoreMastersDef = 10;

/***************************************************************************
	main for the entire frame work app.  Does system initialization and
	calls FrameMain.
***************************************************************************/
void __cdecl main(void)
{
	// Grow the stack and expand the heap. This MUST be done first!
	if (pvNil == vpappb)
		APPB::_SetupHeap(_cbExtraStackDef, _cactMoreMastersDef);
	else
		vpappb->SetupHeap();

	InitGraf(&qd.thePort);
	InitFonts();
	FlushEvents(everyEvent, 0);
	InitWindows();
	InitMenus();
	TEInit();
	InitDialogs(pvNil);
	InitCursor();

	// Go to the app's main entry point.
	FrameMain();
}


/***************************************************************************
	Static method to increase the stack size by cbExtraStack and call
	MoreMasters the specified number of times.
***************************************************************************/
void APPB::_SetupHeap(long cbExtraStack, long cactMoreMasters)
{
	// This is called before any Mac OS stuff is initialized, so
	// don't assert on anything.
	static _fCalled = fFalse;

	if (_fCalled)
		{
		Debug( Debugger(); )
		return;
		}
	_fCalled = fTrue;

	if (cbExtraStack > 0)
		{
		//limit the increase to 100K
		SetApplLimit(PvSubBv(GetApplLimit(), LwMin(cbExtraStack, 102400L)));
		}
	MaxApplZone();
	while (cactMoreMasters-- > 0)
		MoreMasters();
}


/***************************************************************************
	Method to set up the heap.
***************************************************************************/
void APPB::SetupHeap(void)
{
	// This is called before any Mac OS stuff is initialized, so
	// don't assert on anything.

	//add 8K to the stack and call MoreMasters 10 times.
	_SetupHeap(_cbExtraStackDef, _cactMoreMastersDef);
}


/***************************************************************************
	Shutdown immediately.
***************************************************************************/
void APPB::Abort(void)
{
	ExitToShell();
}


/***************************************************************************
	Do OS specific initialization.
***************************************************************************/
bool APPB::_FInitOS(void)
{
	AssertThis(0);

	// we already initialized everything
	return fTrue;
}


/***************************************************************************
	Open a desk accessory.
***************************************************************************/
bool APPB::FCmdOpenDA(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PRT *pprt;
	schar rgchs[256];
	long cchs;

	if (pcmd->pgg != pvNil && pcmd->pgg->IvMac() == 1 &&
			(cchs = pcmd->pgg->Cb(0)) < size(rgchs))
		{
		pcmd->pgg->Get(0, rgchs + 1);
		st[0] = (schar)cchs;
		GetPort(&pprt);
		OpenDeskAcc((byte *)rgchs);
		SetPort(pprt);
		}
	pcmd->cid = cidNil;	//don't record this command
	return fTrue;
}


/***************************************************************************
	Get the next event from the OS.
***************************************************************************/
bool APPB::_FGetNextEvt(EVT *pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	if (!WaitNextEvent(everyEvent, pevt, 0, hNil))
		pevt->what = nullEvent;
	_grfcust &= ~kgrfcustUser;
	_grfcust |= _GrfcustFromEvt(pevt);
	return pevt->what != nullEvent;
}


/***************************************************************************
	The given GOB is tracking the mouse.  See if there are any relevant
	mouse events in the system event queue.  Fill in *ppt with the location
	of the mouse relative to pgob. Also ensure that GrfcustCur() will
	return the correct mouse state.
***************************************************************************/
void APPB::TrackMouse(PGOB pgob, PT *ppt)
{
	AssertThis(0);
	AssertPo(pgob, 0);
	AssertVarMem(ppt);
	EVT evt;

	while (EventAvail(everyEvent, &evt))
		{
		if (!GetNextEvent(everyEvent, &evt))
			break;

		_grfcust &= ~kgrfcustUser;
		_grfcust |= _GrfcustFromEvt(&evt);
		switch (evt.what)
			{
		case mouseDown:
			_grfcust |= fcustMouse;
			goto LDone;

		case mouseUp:
			_grfcust &= ~fcustMouse;
			goto LDone;

		case keyDown:
		case autoKey:
			break;

		case updateEvt:
			_UpdateEvt(&evt);
			break;

		case activateEvt:
			_ActivateEvt(&evt);
			break;

		case diskEvt:
			break;

		case osEvt:
			if ((evt.message & 0xFF000000) == 0xFA000000)
				{
				// mouse move
				goto LDone;
				}
			if (evt.message & 0x01000000)
				{
				// suspend or resume
				Bug("How can this happen?");
				if (evt.message & 0x00000001)
					_ActivateApp(&evt);
				else
					_DeactivateApp(&evt);
				}
			break;
			}
		}

LDone:
	ppt->xp = evt.where.h;
	ppt->yp = evt.where.v;
	pgob->MapPt(ppt, cooGlobal, cooLocal);
}


/***************************************************************************
	Dispatch the OS level event to a translator.
***************************************************************************/
void APPB::_DispatchEvt(EVT *pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	CMD cmd;

	switch (pevt->what)
		{
	case mouseDown:
		_MouseDownEvt(pevt);
		break;

	case mouseUp:
		_MouseUpEvt(pevt);
		break;

	case keyDown:
	case autoKey:
		if (_FTranslateKeyEvt(pevt, (PCMD_KEY)&cmd))
			vpcex->EnqueueCmd(&cmd);
		break;

	case updateEvt:
		_UpdateEvt(pevt);
		break;

	case activateEvt:
		_ActivateEvt(pevt);
		break;

	case diskEvt:
		_DiskEvt(pevt);
		break;

	case osEvt:
		if (pevt->message & 0x01000000)
			{
			// suspend or resume
			if (pevt->message & 0x00000001)
				_ActivateApp(pevt);
			else
				_DeactivateApp(pevt);
			}
		else if ((pevt->message & 0xFF000000) == 0xFA000000)
			_MouseMovedEvt(pevt);
		break;
		}
}


/***************************************************************************
	Dispatch an OS level mouse down event.
***************************************************************************/
void APPB::_MouseDownEvt(EVT *pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	short in;
	HWND hwnd;
	PGOB pgob;

	in = FindWindow(pevt->where, (WindowPtr *)&hwnd);
	switch (in)
		{
	case inMenuBar:
		if (pvNil != vpmubCur)
			vpmubCur->FDoClick(pevt);
		break;

	case inDrag:
		RCS rcs = qd.screenBits.bounds;
		InsetRect(&rcs, 4, 4);
		DragWindow(&hwnd->port, pevt->where, &rcs);
		break;

	case inGoAway:
		if (TrackGoAway(&hwnd->port, pevt->where) &&
				(pgob = GOB::PgobFromHwnd(hwnd)) != pvNil)
			{
			vpcex->EnqueueCid(cidCloseWnd, pgob);
			}
		break;

	case inGrow:
		if ((pgob = GOB::PgobFromHwnd(hwnd)) != pvNil)
			pgob->TrackGrow(pevt);
		break;

	case inContent:
		if (hwnd != (HWND)FrontWindow())
			SelectWindow(&hwnd->port);
		else if ((pgob = GOB::PgobFromHwnd(hwnd)) != pvNil)
			{
			PPRT pprt;
			PT pt;
			PTS pts = pevt->where;

			GetPort(&pprt);
			SetPort(&hwnd->port);
			GlobalToLocal(&pts);
			SetPort(pprt);
			if ((pgob = pgob->PgobFromPt(pts.h, pts.v, &pt)) != pvNil)
				{
				if (_pgobMouse == pgob &&
						FIn(pevt->when - _tsMouse, 0, GetDblTime()))
					{
					_cactMouse++;
					}
				else
					_cactMouse = 1;
				_tsMouse = pevt->when;
				if (_pgobMouse != pgob && pvNil != _pgobMouse)
					{
					AssertPo(_pgobMouse, 0);
					vpcex->EnqueueCid(cidRollOff, _pgobMouse);
					}
				_pgobMouse = pgob;
				_xpMouse = klwMax;
				pgob->MouseDown(pt.xp, pt.yp, _cactMouse, GrfcustCur());
				}
			else
                _pgobMouse = pvNil;
			}
		break;

	case inSysWindow:
	case inZoomIn:
	case inZoomOut:
		break;
		}
}


/***************************************************************************
	Dispatch an OS level mouse up event.
***************************************************************************/
void APPB::_MouseUpEvt(EVT *pevt)
{
	//Ignore mouse up events
}


/***************************************************************************
	Translate an OS level key down event to a CMD.  This returns false if
	the key maps to a menu item.
	//REVIEW shonk: resolve (ch, vk) differences between Mac and Win
***************************************************************************/
bool APPB::_FTranslateKeyEvt(EVT *pevt, PCMD_KEY pcmd)
{
	AssertThis(0);
	AssertVarMem(pevt);
	AssertVarMem(pcmd);

	ClearPb(pcmd, size(*pcmd));
	pcmd->cid = cidKey;
	pcmd->grfcust = GrfcustCur();
	if ((pcmd->grfcust & fcustCmd) && vpmubCur != pvNil &&
			vpmubCur->FDoKey(pevt))
		{
		TrashVar(pcmd);
		return fFalse;
		}
	pcmd->ch = B0Lw(pevt->message);
	pcmd->vk = pcmd->ch;
	pcmd->cact = 1;
	return fTrue;
}


/***************************************************************************
	Look at the next system event and if it's a key, fill in the *pcmd with
	the relevant info.
***************************************************************************/
bool APPB::FGetNextKeyFromOsQueue(PCMD_KEY pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	EVT evt;

	if (!EventAvail(everyEvent, &evt))
		goto LFail;
	if (evt.what != keyDown && evt.what != autoKey)
		goto LFail;
	if (!GetNextEvent(keyDownMask | autoKeyMask, &evt))
		{
LFail:
		TrashVar(pcmd);
		return fFalse;
		}
	_grfcust &= ~kgrfcustUser;
	_grfcust |= _GrfcustFromEvt(&evt);
	return _FTranslateKeyEvt(&evt, pcmd);
}


/***************************************************************************
	Returns the grfcust for the given event.
***************************************************************************/
ulong _GrfcustFromEvt(PEVT pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	ulong grfcust = 0;

	if (pevt->modifiers & (cmdKey | controlKey))
		grfcust |= fcustCmd;
	if (pevt->modifiers & shiftKey)
		grfcust |= fcustShift;
	if (pevt->modifiers & optionKey)
		grfcust |= fcustOption;
	if (!(pevt->modifiers & btnState))
		grfcust |= fcustMouse;
	return grfcust;
}


/***************************************************************************
	Dispatch an OS level update event.
***************************************************************************/
void APPB::_UpdateEvt(EVT *pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	PPRT pprt, pprtSav;
	RCS rcs;
	RC rc;

	pprt = (PPRT)pevt->message;
	InvalMarked((HWND)pprt);
	rcs = (*((HWND)pprt)->updateRgn)->rgnBBox;
	GetPort(&pprtSav);
	SetPort(pprt);
	GlobalToLocal((PTS *)&rcs);
	GlobalToLocal((PTS *)&rcs + 1);
	rc = rcs;
	BeginUpdate(pprt);
	UpdateHwnd((HWND)pprt, &rc);
	EndUpdate(pprt);
	SetPort(pprtSav);
}


/***************************************************************************
	Dispatch an OS level activate event.
***************************************************************************/
void APPB::_ActivateEvt(EVT *pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	//Tell the gob code that an hwnd is being activated or deactivated
	GOB::ActivateHwnd((HWND)pevt->message, pevt->modifiers & 1);
}


/***************************************************************************
	Handle an OS level disk event.
***************************************************************************/
void APPB::_DiskEvt(EVT *pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);
}


/***************************************************************************
	Handle activation of the app.
***************************************************************************/
void APPB::_ActivateApp(EVT *pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	_Activate(fTrue);
}


/***************************************************************************
	Handle deactivation of the app.
***************************************************************************/
void APPB::_DeactivateApp(EVT *pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	_Activate(fFalse);
}


/***************************************************************************
	Handle an OS level mouse moved event.
***************************************************************************/
void APPB::_MouseMovedEvt(EVT *pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);
}


/****************************************
	Standard alert resources
****************************************/
enum
	{
	kridOkAlert = 128,
	kridOkCancelAlert,
	kridYesNoAlert,
	kridYesNoCancelAlert
	};


/***************************************************************************
	Put an alert up.  Return which button was hit.  Returns tYes for yes
	or ok; tNo for no; tMaybe for cancel.
***************************************************************************/
bool APPB::TGiveAlertSz(PSZ psz, long bk, long cok)
{
	AssertThis(0);
	AssertSz(psz);

	short rid, bid;
	STN stn;

	stn = psz;
	switch (bk)
		{
	default:
		BugVar("bad bk value", &bk);
		//fall through
	case bkOk:
		rid = kridOkAlert;
		break;
	case bkOkCancel:
		rid = kridOkCancelAlert;
		break;
	case bkYesNo:
		rid = kridYesNoAlert;
		break;
	case bkYesNoCancel:
		rid = kridYesNoCancelAlert;
		break;
		}

	ParamText(stn.Pst(), (uchar *)"", (uchar *)"", (uchar *)"");
	switch (cok)
		{
	default:
		BugVar("bad cok value", &cok);
		Debug( ParamText(stn.Pst(), (uchar *)"", (uchar *)"", (uchar *)""); )
		//fall through
	case cokNil:
		bid = Alert(rid, pvNil);
		break;
	case cokInformation:
		bid = NoteAlert(rid, pvNil);
		break;
	case cokQuestion:
	case cokExclamation:
		bid = CautionAlert(rid, pvNil);
		break;
	case cokStop:
		bid = StopAlert(rid, pvNil);
		break;
		}

	switch (bid)
		{
	default:
	case 1:
		return tYes;
    case 2:
		return tNo;
	case 3:
		return tMaybe;
		}
}


#ifdef DEBUG
const short kridAssert = 32000;
const short kbidDebugger = 1;
const short kbidIgnore = 2;
const short kbidQuit = 3;

/***************************************************************************
	Debug initialization.
***************************************************************************/
bool APPB::_FInitDebug(void)
{
	return fTrue;
}


/***************************************************************************
	Assert proc.
	REVIEW shonk: Mac FAssertProcApp: flesh out and fix for unicode.
***************************************************************************/
bool APPB::FAssertProcApp(PSZ pszFile, long lwLine, PSZ pszMsg,
	void *pv, long cb)
{
	short bid;
	achar stLine[kcbMaxSt];
	achar stFile[kcbMaxSt];
	achar stMessage[kcbMaxSt];

	if (_fInAssert)
		return fFalse;

	_fInAssert = fTrue;
	if (pszMsg != pvNil)
		CopySzSt(pszMsg, stMessage);
	else
		SetStCch(stMessage, 0);
	if (pvNil != pszFile)
        CopySzSt(pszFile, stFile);
	else
		CopySzSt("Some Header file", stFile);
	NumToString(lwLine, (byte *)stLine);
	ParamText((byte *)stLine, (byte *)stFile, (byte *)stMessage, (byte *)"\p");
	bid = Alert(kridAssert, pvNil);
	_fInAssert = fFalse;

	switch (bid)
		{
	case kbidDebugger:
		return 1;
	case kbidIgnore:
		break;
	case kbidQuit:
		ExitToShell();
	default:
		break;
		}
	return 0;
}
#endif

