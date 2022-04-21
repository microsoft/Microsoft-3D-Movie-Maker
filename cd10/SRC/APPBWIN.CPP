/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Windows base application class.

***************************************************************************/
#include "frame.h"
ASSERTNAME


WIG vwig;

/***************************************************************************
	WinMain for any frame work app. Sets up vwig and calls FrameMain.
***************************************************************************/
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hinstPrev, LPSTR pszs, int wShow)
{
	vwig.hinst = hinst;
	vwig.hinstPrev = hinstPrev;
	vwig.pszCmdLine = GetCommandLine();
	vwig.wShow = wShow;
	vwig.lwThreadMain = LwThreadCur();

	FrameMain();
	return 0;
}


/***************************************************************************
	Shutdown immediately.
***************************************************************************/
void APPB::Abort(void)
{
	_ShutDownViewer();
	FatalAppExit(0, PszLit("Fatal Error Termination"));
}


/***************************************************************************
	Do OS specific initialization.
***************************************************************************/
bool APPB::_FInitOS(void)
{
	AssertThis(0);
	STN stnApp;
	PSZ pszAppWndCls = PszLit("APP");

	// get the app name
	GetStnAppName(&stnApp);

	// register the window classes
	if (vwig.hinstPrev == hNil)
		{
		WNDCLASS wcs;

		wcs.style = CS_BYTEALIGNCLIENT | CS_OWNDC;
		wcs.lpfnWndProc = _LuWndProc;
		wcs.cbClsExtra = 0;
		wcs.cbWndExtra = 0;
		wcs.hInstance = vwig.hinst;
		wcs.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wcs.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcs.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcs.lpszMenuName = 0;
		wcs.lpszClassName = pszAppWndCls;
		if (!RegisterClass(&wcs))
			return fFalse;

		wcs.lpfnWndProc = _LuMdiWndProc;
		wcs.lpszClassName = PszLit("MDI");
		if (!RegisterClass(&wcs))
			return fFalse;
		}

	if ((vwig.hwndApp = CreateWindow(pszAppWndCls, stnApp.Psz(),
			WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
			hNil, hNil, vwig.hinst, pvNil)) == hNil)
		{
		return fFalse;
		}
	if (hNil == (vwig.hdcApp = GetDC(vwig.hwndApp)))
		return fFalse;

	//set a timer, so we can idle regularly.
	if (SetTimer(vwig.hwndApp, 0, 1, pvNil) == 0)
		return fFalse;

	vwig.haccel = LoadAccelerators(vwig.hinst, MIR(acidMain));
	ShowWindow(vwig.hwndApp, vwig.wShow);
	return fTrue;
}


/***************************************************************************
	Get the next event from the OS event queue. Return true iff it's a
	real event (not just an idle type event).
***************************************************************************/
bool APPB::_FGetNextEvt(PEVT pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	GetMessage(pevt, hNil, 0, 0);
	switch (pevt->message)
		{
	case WM_TIMER:
		return fFalse;

	case WM_MOUSEMOVE:
		// dispatch these so real Windows controls can receive them,
		// but return false so we can do our idle stuff - including
		// our own mouse moved stuff.
		_DispatchEvt(pevt);
		return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	The given GOB is tracking the mouse. See if there are any relevant
	mouse events in the system event queue. Fill in *ppt with the location
	of the mouse relative to pgob. Also ensure that GrfcustCur() will
	return the correct mouse state.
***************************************************************************/
void APPB::TrackMouse(PGOB pgob, PT *ppt)
{
	AssertThis(0);
	AssertPo(pgob, 0);
	AssertVarMem(ppt);

	EVT evt;
	PTS pts;

	for (;;)
		{
		if (!PeekMessage(&evt, hNil, 0, 0, PM_REMOVE | PM_NOYIELD))
			{
			GetCursorPos(&pts);
			break;
			}

		if (FIn(evt.message, WM_MOUSEFIRST, WM_MOUSELAST + 1))
			{
			pts = evt.pt;
			break;
			}

		//toss key events
		if (!FIn(evt.message, WM_KEYFIRST, WM_KEYLAST + 1))
			{
			TranslateMessage(&evt);
			DispatchMessage(&evt);
			}
		}

	ppt->xp = pts.x;
	ppt->yp = pts.y;
	pgob->MapPt(ppt, cooGlobal, cooLocal);
}


/***************************************************************************
	Dispatch an OS level event to someone that knows what to do with it.
***************************************************************************/
void APPB::_DispatchEvt(PEVT pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	CMD cmd;

	if (hNil != vwig.hwndClient &&
			TranslateMDISysAccel(vwig.hwndClient, pevt) ||
		hNil != vwig.haccel &&
			TranslateAccelerator(vwig.hwndApp, vwig.haccel, pevt))
		{
		return;
		}

	switch (pevt->message)
		{
	case WM_KEYDOWN:
	case WM_CHAR:
		if (_FTranslateKeyEvt(pevt, (PCMD_KEY)&cmd) && pvNil != vpcex)
			vpcex->EnqueueCmd(&cmd);
		ResetToolTip();
		break;

	case WM_KEYUP:
	case WM_DEADCHAR:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_SYSCHAR:
	case WM_SYSDEADCHAR:
		ResetToolTip();
		// fall thru
	default:
		TranslateMessage(pevt);
		DispatchMessage(pevt);
		break;
		}
}


/***************************************************************************
	Translate an OS level key down event to a CMD. This returns false if
	the key maps to a menu item.
***************************************************************************/
bool APPB::_FTranslateKeyEvt(PEVT pevt, PCMD_KEY pcmd)
{
	AssertThis(0);
	AssertVarMem(pevt);
	AssertVarMem(pcmd);

	EVT evt;

	ClearPb(pcmd, size(*pcmd));
	pcmd->cid = cidKey;

	if (pevt->message == WM_KEYDOWN)
		{
		TranslateMessage(pevt);
		if (PeekMessage(&evt, pevt->hwnd, 0, 0, PM_NOREMOVE) &&
			WM_CHAR == evt.message &&
			PeekMessage(&evt, pevt->hwnd, WM_CHAR, WM_CHAR, PM_REMOVE))
			{
			Assert(evt.message == WM_CHAR, 0);
			pcmd->ch = evt.wParam;
			}
		else
			pcmd->ch = chNil;
		pcmd->vk = pevt->wParam;
		}
	else
		{
		pcmd->vk = vkNil;
		pcmd->ch = pevt->wParam;
		}
	pcmd->grfcust = GrfcustCur();
	pcmd->cact = SwLow(pevt->lParam);

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

	for (;;)
		{
		if (!PeekMessage(&evt, hNil, 0, 0, PM_NOREMOVE) ||
			!FIn(evt.message, WM_KEYFIRST, WM_KEYLAST + 1) ||
			!PeekMessage(&evt, evt.hwnd, evt.message, evt.message, PM_REMOVE))
			{
			break;
			}

		if (hNil != vwig.hwndClient &&
				TranslateMDISysAccel(vwig.hwndClient, &evt) ||
			hNil != vwig.haccel &&
				TranslateAccelerator(vwig.hwndApp, vwig.haccel, &evt))
			{
			break;
			}

		switch (evt.message)
			{
		case WM_CHAR:
		case WM_KEYDOWN:
			if (!_FTranslateKeyEvt(&evt, pcmd))
				goto LFail;
			return fTrue;

		default:
			TranslateMessage(&evt);
			DispatchMessage(&evt);
			break;
			}
		}

LFail:
	TrashVar(pcmd);
	return fFalse;
}


/***************************************************************************
	Flush user generated events from the system event queue.
***************************************************************************/
void APPB::FlushUserEvents(ulong grfevt)
{
	AssertThis(0);
	EVT evt;

	while ((grfevt & fevtMouse) &&
			PeekMessage(&evt, hNil, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE) ||
		(grfevt & fevtKey) &&
			PeekMessage(&evt, hNil, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
		{
		}
}


/***************************************************************************
	Get our app window out of the clipboard viewer chain.
***************************************************************************/
void APPB::_ShutDownViewer(void)
{
	if (vwig.hwndApp != hNil)
		ChangeClipboardChain(vwig.hwndApp, vwig.hwndNextViewer);
	vwig.hwndNextViewer = hNil;
}


/***************************************************************************
	Main window procedure (a static method).
***************************************************************************/
LRESULT CALLBACK APPB::_LuWndProc(HWND hwnd, uint wm, WPARAM wParam, LPARAM lw)
{
	AssertNilOrPo(vpappb, 0);
	long lwRet;

	if (pvNil != vpappb &&
			vpappb->_FFrameWndProc(hwnd, wm, wParam, lw, &lwRet))
		{
		return lwRet;
		}

	return DefFrameProc(hwnd, vwig.hwndClient, wm, wParam, lw);
}


/***************************************************************************
	Handle Windows messages for the main app window. Return true iff the
	default window proc should _NOT_ be called.
***************************************************************************/
bool APPB::_FFrameWndProc(HWND hwnd, uint wm,
	WPARAM wParam, LPARAM lw, long *plwRet)
{
	AssertThis(0);
	AssertVarMem(plwRet);

	PGOB pgob;
	RC rc;
	PT pt;
	long xp, yp;
	long lwT;
	long lwStyle;

	*plwRet = 0;
	switch (wm)
		{
	default:
		return _FCommonWndProc(hwnd, wm, wParam, lw, plwRet);

	case WM_CREATE:
		Assert(vwig.hwndApp == hNil, 0);
		vwig.hwndNextViewer = SetClipboardViewer(hwnd);
		vwig.hwndApp = hwnd;
		return fTrue;

	case WM_CHANGECBCHAIN:
		if ((HWND)wParam == vwig.hwndNextViewer)
			vwig.hwndNextViewer = (HWND)lw;
		else if (hNil != vwig.hwndNextViewer)
			SendMessage(vwig.hwndNextViewer, wm, wParam, lw);
		return fTrue;

	case WM_DRAWCLIPBOARD:
		if (hNil != vwig.hwndNextViewer)
			SendMessage(vwig.hwndNextViewer, wm, wParam, lw);
		if (vwig.hwndApp != hNil && GetClipboardOwner() != vwig.hwndApp)
			vpclip->Import();
		return fTrue;

	case WM_DESTROY:
		_ShutDownViewer();
		vwig.hwndApp = hNil;
		PostQuitMessage(0);
		return fTrue;

	case WM_SIZE:
		// make sure the style bits are set correctly
		lwT = lwStyle = GetWindowLong(vwig.hwndApp, GWL_STYLE);
		if (_fFullScreen && wParam == SIZE_MAXIMIZED)
			{
			// in full screen mode, set popup and nuke the system menu stuff
			lwStyle |= WS_POPUP;
			lwStyle &= ~(WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
			}
		else
			{
			// in non-full screen mode, clear popup and set the system menu stuff
			lwStyle &= ~WS_POPUP;
			lwStyle |= (WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX);
			}
		if (lwT != lwStyle)
			SetWindowLong(vwig.hwndApp, GWL_STYLE, lwStyle);

		return _FCommonWndProc(hwnd, wm, wParam, lw, plwRet);

	case WM_PALETTECHANGED:
		if ((HWND)wParam == hwnd)
			return fTrue;
		// fall thru
	case WM_QUERYNEWPALETTE:
		*plwRet = GPT::CclrSetPalette(hwnd, fTrue) > 0;
		return fTrue;

	case WM_GETMINMAXINFO:
		BLOCK
			{
			long dypFrame, dypScreen, dypExtra;
			MINMAXINFO *pmmi;

			pmmi = (MINMAXINFO *)lw;

			*plwRet = DefFrameProc(hwnd, vwig.hwndClient, wm, wParam, (long)pmmi);
			dypFrame = GetSystemMetrics(SM_CYFRAME);
			dypScreen = GetSystemMetrics(SM_CYSCREEN);
			dypExtra = 0;

			FGetProp(kpridFullScreen, &lw);
			if (lw)
				dypExtra = GetSystemMetrics(SM_CYCAPTION);
			pmmi->ptMaxPosition.y = -dypFrame - dypExtra;
			pmmi->ptMaxSize.y = pmmi->ptMaxTrackSize.y =
				dypScreen + 2 * dypFrame + dypExtra;
			_FCommonWndProc(hwnd, wm, wParam, (long)pmmi, &lw);
			}
		return fTrue;

	case WM_CLOSE:
		if (pvNil != vpcex)
			vpcex->EnqueueCid(cidQuit);
		return fTrue;

	case WM_QUERYENDSESSION:
		if (!_fQuit)
			Quit(fFalse);
		*plwRet = _fQuit;
		return fTrue;

	case WM_COMMAND:
		if (GET_WM_COMMAND_HWND(wParm, lw) != hNil)
			break;

		lwT = GET_WM_COMMAND_ID(wParam, lw);
		if (!FIn(lwT, wcidMinApp, wcidLimApp))
			break;

		if (pvNil != vpmubCur)
			vpmubCur->EnqueueWcid(lwT);
		else if (pvNil != vpcex)
			vpcex->EnqueueCid(lwT);
		return fTrue;

	case WM_INITMENU:
		if (vpmubCur != pvNil)
			{
			vpmubCur->Clean();
			return fTrue;
			}
		break;

	// these are for automated testing support...
	case WM_GOB_STATE:
		if (pvNil != (pgob = GOB::PgobFromHidScr(lw)))
			*plwRet = pgob->LwState();
		return fTrue;

	case WM_GOB_LOCATION:
		*plwRet = -1;
		if (pvNil == (pgob = GOB::PgobFromHidScr(lw)))
			return fTrue;

		pgob->GetRcVis(&rc, cooLocal);
		if (rc.FEmpty())
			return fTrue;
		pt.xp = pt.yp = 0;
		pgob->MapPt(&pt, cooLocal, cooGlobal);
		for (lwT = 0; lwT < 256; lwT++)
			{
			for (yp = rc.ypTop + (lwT & 0x0F); yp < rc.ypBottom; yp += 16)
				{
				for (xp = rc.xpLeft + (lwT >> 4); xp < rc.xpRight; xp += 16)
					{
					if (pgob->FPtIn(xp, yp) &&
						pgob == GOB::PgobFromPtGlobal(xp + pt.xp, yp + pt.yp))
						{
						pt.xp += xp;
						pt.yp += yp;
						GOB::PgobScreen()->MapPt(&pt, cooGlobal, cooLocal);
						*plwRet = LwHighLow((short)pt.xp, (short)pt.yp);
						return fTrue;
						}
					}
				}
			}
		return fTrue;

	case WM_GLOBAL_STATE:
		*plwRet = GrfcustCur();
		return fTrue;

	case WM_CURRENT_CURSOR:
		if (pvNil != _pcurs)
			*plwRet = _pcurs->Cno();
		else
			*plwRet = cnoNil;
		return fTrue;

	case WM_GET_PROP:
		if (!FGetProp(lw, plwRet))
			*plwRet = wParam;
		return fTrue;

	case WM_SCALE_TIME:
		*plwRet = vpusac->LuScale();
		vpusac->Scale(lw);
		return fTrue;

	case WM_GOB_FROM_PT:
		pt.xp = wParam;
		pt.yp = lw;
		GOB::PgobScreen()->MapPt(&pt, cooLocal, cooGlobal);
		if (pvNil != (pgob = GOB::PgobFromPtGlobal(pt.xp, pt.yp)))
			*plwRet = pgob->Hid();
		return fTrue;

	case WM_FIRST_CHILD:
		if (pvNil != (pgob = GOB::PgobFromHidScr(lw)) &&
				pvNil != (pgob = pgob->PgobFirstChild()))
			{
			*plwRet = pgob->Hid();
			}
		return fTrue;

	case WM_NEXT_SIB:
		if (pvNil != (pgob = GOB::PgobFromHidScr(lw)) &&
				pvNil != (pgob = pgob->PgobNextSib()))
			{
			*plwRet = pgob->Hid();
			}
		return fTrue;

	case WM_PARENT:
		if (pvNil != (pgob = GOB::PgobFromHidScr(lw)) &&
				pvNil != (pgob = pgob->PgobPar()))
			{
			*plwRet = pgob->Hid();
			}
		return fTrue;

	case WM_GOB_TYPE:
		if (pvNil != (pgob = GOB::PgobFromHidScr(lw)))
			*plwRet = pgob->Cls();
		return fTrue;

	case WM_IS_GOB:
		if (pvNil != (pgob = GOB::PgobFromHidScr(lw)))
			*plwRet = pgob->FIs(wParam);
		return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	MDI window proc (a static method).
***************************************************************************/
LRESULT CALLBACK APPB::_LuMdiWndProc(HWND hwnd, uint wm,
	WPARAM wParam, LPARAM lw)
{
	AssertNilOrPo(vpappb, 0);
	long lwRet;

	if (pvNil != vpappb &&
			vpappb->_FMdiWndProc(hwnd, wm, wParam, lw, &lwRet))
		{
		return lwRet;
		}

	return DefMDIChildProc(hwnd, wm, wParam, lw);
}


/***************************************************************************
	Handle MDI window messages. Returns true iff the default window proc
	should _NOT_ be called.
***************************************************************************/
bool APPB::_FMdiWndProc(HWND hwnd, uint wm,
	WPARAM wParam, LPARAM lw, long *plwRet)
{
	AssertThis(0);
	AssertVarMem(plwRet);

	PGOB pgob;
	long lwT;

	*plwRet = 0;
	switch (wm)
		{
	default:
		return _FCommonWndProc(hwnd, wm, wParam, lw, plwRet);

	case WM_GETMINMAXINFO:
		*plwRet = DefMDIChildProc(hwnd, wm, wParam, lw);
		_FCommonWndProc(hwnd, wm, wParam, lw, &lwT);
		return fTrue;

	case WM_CLOSE:
		if ((pgob = GOB::PgobFromHwnd(hwnd)) != pvNil)
			vpcex->EnqueueCid(cidCloseWnd, pgob);
		return fTrue;

	case WM_MDIACTIVATE:
		GOB::ActivateHwnd(hwnd,
			GET_WM_MDIACTIVATE_FACTIVATE(hwnd, wParam, lw));
		break;
		}

	return fFalse;
}


/***************************************************************************
	Common stuff between the two window procs. Returns true if the default
	window proc should _NOT_ be called.
***************************************************************************/
bool APPB::_FCommonWndProc(HWND hwnd, uint wm,
	WPARAM wParam, LPARAM lw, long *plwRet)
{
	AssertThis(0);
	AssertVarMem(plwRet);

	PGOB pgob;
	PT pt;
	PSCB pscb;
	RC rc;
	HDC hdc;
	PAINTSTRUCT ps;
	HRGN hrgn;

	*plwRet = 0;
	switch (wm)
		{
	case WM_PAINT:
		if (IsIconic(hwnd))
			break;

		// make sure the palette is selected and realized....
		// theoretically, we shouldn't have to do this, but because
		// of past and present Win bugs, we do it to be safe.
		GPT::CclrSetPalette(hwnd, fFalse);

		// invalidate stuff that we have marked internally (may as well
		// draw everything that needs drawn).
		InvalMarked(hwnd);

		// NOTE: BeginPaint has a bug where it returns in ps.rcPaint the
		// bounds of the update region intersected with the current clip region.
		// This causes us to not draw everything we need to. To fix this we
		// save, open up, and restore the clipping region around the BeginPaint
		// call.
		hdc = GetDC(hwnd);
		if (hNil == hdc)
			goto LFailPaint;

		if (FCreateRgn(&hrgn, pvNil) && 1 != GetClipRgn(hdc, hrgn))
			FreePhrgn(&hrgn);
		SelectClipRgn(hdc, hNil);

		if (!BeginPaint(hwnd, &ps))
			{
			ReleaseDC(hwnd, hdc);
LFailPaint:
			Warn("Painting failed");
			break;
			}

		// Since we use CS_OWNDC, these DCs should be the same...
		Assert(hdc == ps.hdc, 0);

		rc = RC(ps.rcPaint);
		UpdateHwnd(hwnd, &rc);
		EndPaint(hwnd, &ps);

		// don't call the default window proc - or it will clear anything
		// that got invalidated while we were drawing (which can happen
		// in a multi-threaded pre-emptive environment).
		return fTrue;

	case WM_SYSCOMMAND:
		if (wParam == SC_SCREENSAVE && !FAllowScreenSaver())
			return fTrue;
		break;

	case WM_GETMINMAXINFO:
		if (pvNil != (pgob = GOB::PgobFromHwnd(hwnd)))
			{
			MINMAXINFO *pmmi = (MINMAXINFO far *)lw;

			pgob->GetMinMax(&rc);
			pmmi->ptMinTrackSize.x =
				LwMax(pmmi->ptMinTrackSize.x, rc.xpLeft);
			pmmi->ptMinTrackSize.y =
				LwMax(pmmi->ptMinTrackSize.y, rc.ypTop);
			pmmi->ptMaxTrackSize.x =
				LwMin(pmmi->ptMaxTrackSize.x, rc.xpRight);
			pmmi->ptMaxTrackSize.y =
				LwMin(pmmi->ptMaxTrackSize.y, rc.ypBottom);
			}
		return fTrue;

	case WM_SIZE:
		if (pvNil != (pgob = GOB::PgobFromHwnd(hwnd)))
			pgob->SetRcFromHwnd();
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		ResetToolTip();
		if (pvNil != (pgob = GOB::PgobFromHwnd(hwnd)) &&
				pvNil != (pgob = pgob->PgobFromPt(SwLow(lw), SwHigh(lw), &pt)))
			{
			long ts;

			// compute the multiplicity of the click - don't use Windows'
			// guess, since it can be wrong for our GOBs. It's even wrong
			// at the HWND level! (Try double-clicking the maximize button).
			ts = GetMessageTime();
			if (_pgobMouse == pgob &&
					FIn(ts - _tsMouse, 0, GetDoubleClickTime()))
				{
				_cactMouse++;
				}
			else
				_cactMouse = 1;
			_tsMouse = ts;
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
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		ResetToolTip();
		break;

	case WM_SETCURSOR:
		if (LOWORD(lw) != HTCLIENT)
			return fFalse;
		RefreshCurs();
		return fTrue;

	case WM_HSCROLL:
	case WM_VSCROLL:
		pscb = (PSCB)CTL::PctlFromHctl(GET_WM_HSCROLL_HWND(wParam, lw));
		if (pvNil != pscb && pscb->FIs(kclsSCB))
			{
			pscb->TrackScroll(GET_WM_HSCROLL_CODE(wParam, lw),
				GET_WM_HSCROLL_POS(wParam, lw));
			}
		break;

	case WM_ACTIVATEAPP:
		_Activate(FPure(wParam));
		break;
		}

	return fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Debug initialization.
***************************************************************************/
bool APPB::_FInitDebug(void)
{
	AssertThis(0);
	return fTrue;
}


// passes the strings to the assert dialog proc
STN *_rgpstn[3];

/***************************************************************************
	Dialog proc for assert.
***************************************************************************/
BOOL CALLBACK _FDlgAssert(HWND hdlg, UINT msg, WPARAM w, LPARAM lw)
{
	switch (msg)
		{
	case WM_INITDIALOG:
		SetDlgItemText(hdlg, 3, _rgpstn[0]->Psz());
		SetDlgItemText(hdlg, 4, _rgpstn[1]->Psz());
		SetDlgItemText(hdlg, 5, _rgpstn[2]->Psz());
		return fTrue;

	case WM_COMMAND:
		switch (GET_WM_COMMAND_ID(w, lw))
			{
		default:
			break;

		case 0:
		case 1:
		case 2:
			EndDialog(hdlg, GET_WM_COMMAND_ID(w, lw));
			return fTrue;
			}
		break;
		}
	return fFalse;
}


MUTX _mutxAssert;

/***************************************************************************
	The assert proc. Returning true breaks into the debugger.
***************************************************************************/
bool APPB::FAssertProcApp(PSZS pszsFile, long lwLine, PSZS pszsMsg,
	void *pv, long cb)
{
	const long kclwChain = 10;
	STN stn0, stn1, stn2;
	int tmc;
	PSZ psz;
	long cact;
	long *plw;
	long ilw;
	long rglw[kclwChain];

	_mutxAssert.Enter();

	if (_fInAssert)
		{
		_mutxAssert.Leave();
		return fFalse;
		}

	_fInAssert = fTrue;

	_rgpstn[0] = &stn0;
	_rgpstn[1] = &stn1;
	_rgpstn[2] = &stn2;

	// build the main assert message with file name and line number
	if (pszsMsg == pvNil || *pszsMsg == 0)
		psz = PszLit("Assert (%s line %d)");
	else
		{
		psz = PszLit("Assert (%s line %d): %s");
		stn2.SetSzs(pszsMsg);
		}
	if (pvNil != pszsFile)
		stn1.SetSzs(pszsFile);
	else
		stn1 = PszLit("Some Header file");
	stn0.FFormatSz(psz, &stn1, lwLine, &stn2);

	// call stack - follow the EBP chain....
	__asm { mov plw,ebp }
	for (ilw = 0; ilw < kclwChain; ilw++)
		{
		if (pvNil == plw || IsBadReadPtr(plw, 2 * size(long)) ||
				*plw <= (long)plw)
			{
			rglw[ilw] = 0;
			plw = pvNil;
			}
		else
			{
			rglw[ilw] = plw[1];
			plw = (long *)*plw;
			}
		}

	for (cact = 0; cact < 2; cact++)
		{
		// format data
		if (pv != pvNil && cb > 0)
			{
			byte *pb = (byte *)pv;
			long cbT = cb;
			long ilw;
			long lw;
			STN stnT;

			stn2.SetNil();
			for (ilw = 0; ilw < 20 && cb >= 4; cb -= 4, pb += 4, ++ilw)
				{
				CopyPb(pb, &lw, 4);
				stnT.FFormatSz(PszLit("%08x "), lw);
				stn2.FAppendStn(&stnT);
				}
			if (ilw < 20 && cb > 0)
				{
				lw = 0;
				CopyPb(pb, &lw, cb);
				stnT.FFormatSz((cb <= 2) ? PszLit("%04x") : PszLit("%08x"), lw);
				stn2.FAppendStn(&stnT);
				}
			}
		else
			stn2.SetNil();

		if (cact == 0)
			{
			pv = rglw;
			cb = size(rglw);
			stn1 = stn2;
			}
		}

	OutputDebugString(stn0.Psz());
	OutputDebugString(PszLit("\n"));

	if (stn1.Cch() > 0)
		{
		OutputDebugString(stn1.Psz());
		OutputDebugString(PszLit("\n"));
		}
	if (stn2.Cch() > 0)
		{
		OutputDebugString(stn2.Psz());
		OutputDebugString(PszLit("\n"));
		}

	if (LwThreadCur() != vwig.lwThreadMain)
		{
		// can't use a dialog - it may cause grid - lock
		long sid;
		ulong grfmb;

		stn0.FAppendSz(PszLit("\n"));
		stn0.FAppendStn(&stn1);
		stn0.FAppendSz(PszLit("\n"));
		stn0.FAppendStn(&stn2);

		grfmb = MB_SYSTEMMODAL | MB_YESNO | MB_ICONHAND;
		sid = MessageBox(hNil, stn0.Psz(),
			PszLit("Thread Assert! (Y = Ignore, N = Debugger)"), grfmb);

		switch (sid)
			{
		default:
			tmc = 0;
			break;
		case IDNO:
			tmc = 1;
			break;
			}
		}
	else
		{
		// run the dialog
		tmc = DialogBox(vwig.hinst, PszLit("AssertDlg"), vwig.hwndApp,
			&_FDlgAssert);
		}

	_fInAssert = fFalse;
	_mutxAssert.Leave();

	switch (tmc)
		{
	case 0:
		// ignore
		return fFalse;

	case 1:
		// break into debugger
		return fTrue;

	case 2:
		// abort
		Abort(); // shouldn't return
		Debugger();
		break;
		}

	return fFalse;
}
#endif //DEBUG


/***************************************************************************
	Put an alert up. Return which button was hit. Returns tYes for yes
	or ok; tNo for no; tMaybe for cancel.
***************************************************************************/
bool APPB::TGiveAlertSz(PSZ psz, long bk, long cok)
{
	AssertThis(0);
	AssertSz(psz);

	long sid;
	ulong grfmb;
	HWND hwnd;

	grfmb = MB_APPLMODAL;
	switch (bk)
		{
	default:
		BugVar("bad bk value", &bk);
		//fall through
	case bkOk:
		grfmb |= MB_OK;
		break;
	case bkOkCancel:
		grfmb |= MB_OKCANCEL;
		break;
	case bkYesNo:
		grfmb |= MB_YESNO;
		break;
	case bkYesNoCancel:
		grfmb |= MB_YESNOCANCEL;
		break;
		}

	switch (cok)
		{
	default:
		BugVar("bad cok value", &cok);
		//fall through
	case cokNil:
		break;
	case cokInformation:
		grfmb |= MB_ICONINFORMATION;
		break;
	case cokQuestion:
		grfmb |= MB_ICONQUESTION;
		break;
	case cokExclamation:
		grfmb |= MB_ICONEXCLAMATION;
		break;
	case cokStop:
		grfmb |= MB_ICONSTOP;
		break;
		}

	hwnd = GetActiveWindow();
	if (hNil == hwnd)
		hwnd = vwig.hwndApp;
	sid = MessageBox(hwnd, psz, PszLit(""), grfmb);

	switch (sid)
		{
	default:
	case IDYES:
	case IDOK:
		return tYes;
	case IDCANCEL:
		return tMaybe;
	case IDNO:
		return tNo;
		}
}

