/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Graphic object class.

***************************************************************************/
#include "frame.h"
ASSERTNAME


PGOB GOB::_pgobScreen;

#define kswKindGob 0x526F

/***************************************************************************
	Create the screen gob.  If fgobEnsureHwnd is set, ensures that the
	screen gob has an OS window associated with it.
***************************************************************************/
bool GOB::FInitScreen(ulong grfgob, long ginDef)
{
	PGOB pgob;

	switch (ginDef)
		{
	case kginDraw:
	case kginMark:
	case kginSysInval:
		_ginDefGob = ginDef;
		break;
		}

	if ((pgob = NewObj GOB(khidScreen)) == pvNil)
		return fFalse;
	Assert(pgob == _pgobScreen, 0);

	if (grfgob & fgobEnsureHwnd)
		{
		//REVIEW shonk: create the hwnd and attach it
		RawRtn();
		}

	return fTrue;
}


/***************************************************************************
	Make the GOB a wrapper for the given system window.
***************************************************************************/
bool GOB::FAttachHwnd(HWND hwnd)
{
	if (_hwnd != hNil)
		{
		ReleasePpo(&_pgpt);
		//don't destroy the hwnd
		_hwnd = hNil;
		_hwnd->refCon = 0;
		}
	if (hwnd != hNil)
		{
		if ((_pgpt = GPT::PgptNew(&hwnd->port)) == pvNil)
			return fFalse;
		_hwnd = hwnd;
		if (_hwnd->windowKind != dialogKind)
			_hwnd->windowKind = kswKindGob;
		_hwnd->refCon = (long)this;
		SetRcFromHwnd();
		}
	return fTrue;
}


/***************************************************************************
	Find the GOB associated with the given hwnd (if there is one).
***************************************************************************/
PGOB GOB::PgobFromHwnd(HWND hwnd)
{
	Assert(hwnd != hNil, "nil hwnd");
	PGOB pgob;

	if (hwnd->windowKind != kswKindGob && hwnd->windowKind != dialogKind)
		return pvNil;
	pgob = (PGOB)hwnd->refCon;
	AssertNilOrPo(pgob, 0);
	return pgob;
}


/***************************************************************************
	Static method to get the next
***************************************************************************/
HWND GOB::HwndMdiActive(void)
{
	HWND hwnd;

	if (hNil == (hwnd = (HWND)FrontWindow()))
		return hNil;
	if (hwnd->windowKind < userKind)
		return hNil;
	if (pvNil != _pgobScreen && _pgobScreen->_hwnd == hwnd)
		return hNil;
	return hwnd;
}


/***************************************************************************
	Creates a new MDI window and returns it.  This is normally then
	attached to a gob.
***************************************************************************/
HWND GOB::_HwndNewMdi(PSTZ pstzTitle)
{
	HWND hwnd;
	RCS rcs;
	static long _cact = 0;

	rcs = qd.screenBits.bounds;
	rcs.top += GetMBarHeight() + 25; //menu bar and title
	rcs.left += 5;
	rcs.right -= 105;
	rcs.bottom -= 105;
	OffsetRect(&rcs, _cact * 20, _cact * 20);
	_cact = (_cact + 1) % 5;

	hwnd = (HWND)NewCWindow(pvNil, &rcs, (byte *)pstzTitle, fTrue,
		documentProc, GrafPtr(-1), fTrue, 0);
	if (hNil != hwnd && pvNil != vpmubCur)
		vpmubCur->FAddListCid(cidChooseWnd, (long)hwnd, pstzTitle);
	return hwnd;
}


/***************************************************************************
	Destroy an hwnd.
***************************************************************************/
void GOB::_DestroyHwnd(HWND hwnd)
{
	if (pvNil != vpmubCur)
		vpmubCur->FRemoveListCid(cidChooseWnd, (long)hwnd);
	DisposeWindow((PPRT)hwnd);
}


/***************************************************************************
	The grow area has been hit, track it and resize the window.
***************************************************************************/
void GOB::TrackGrow(PEVT pevt)
{
	Assert(_hwnd != hNil, "gob has no hwnd");
	Assert(pevt->what == mouseDown, "wrong EVT");

	long lw;
	RC rc;
	RCS rcs;

	GetMinMax(&rc);
	rcs = RCS(rc);
	if ((lw = GrowWindow(&_hwnd->port, pevt->where, &rcs)) != 0)
		{
		SizeWindow(&_hwnd->port, SwLow(lw), SwHigh(lw), fFalse);
		_SetRcCur();
		}
}


/***************************************************************************
	Gets the current mouse location in this gob's coordinates (if ppt is
	not nil) and determines if the mouse button is down (if pfDown is
	not nil).
***************************************************************************/
void GOB::GetPtMouse(PT *ppt, bool *pfDown)
{
	if (ppt != pvNil)
		{
		PTS pts;
		long xp, yp;
		PGOB pgob;
		PPRT pprtSav, pprt;

		xp = yp = 0;
		for (pgob = this; pgob != pvNil && pgob->_hwnd == hNil;
				pgob = pgob->_pgobPar)
			{
			xp += pgob->_rcCur.xpLeft;
			yp += pgob->_rcCur.ypTop;
			}

		if (pvNil != pgob)
			pprt = &pgob->_hwnd->port;
		else
			GetWMgrPort(&pprt);
		GetPort(&pprtSav);
		SetPort(pprt);
		GetMouse(&pts);
		SetPort(pprtSav);

		*ppt = pts;
		ppt->xp -= xp;
		ppt->yp -= yp;
		}
	if (pfDown != pvNil)
		*pfDown = FPure(Button());
}


/***************************************************************************
	Makes sure the GOB is clean (no update is pending).
***************************************************************************/
void GOB::Clean(void)
{
	AssertThis(0);
	HWND hwnd;
	RC rc, rcT;
	RCS rcs;
	PPRT pprt;

	if (hNil == (hwnd = _HwndGetRc(&rc)))
		return;

	vpappb->InvalMarked(hwnd);
	rcs = (*hwnd->updateRgn)->rgnBBox;
	GetPort(&pprt);
	SetPort(&hwnd->port);
	GlobalToLocal((PTS *)&rcs);
	GlobalToLocal((PTS *)&rcs + 1);
	rcT = rcs;
	if (!rc.FIntersect(&rcT))
		{
		SetPort(pprt);
		return;
		}

	BeginUpdate(&hwnd->port);
	vpappb->UpdateHwnd(hwnd, &rc);
	EndUpdate(&hwnd->port);
	SetPort(pprt);
}


/***************************************************************************
	Set the window name.
***************************************************************************/
void GOB::SetHwndName(PSTZ pstz)
{
	if (hNil == _hwnd)
		{
		Bug("GOB doesn't have an hwnd");
		return;
		}
	if (pvNil != vpmubCur)
		{
		vpmubCur->FChangeListCid(cidChooseWnd, (long)_hwnd, pvNil,
			(long)_hwnd, pstz);
		}
	SetWTitle(&_hwnd->port, (byte *)pstz);
}


/***************************************************************************
	Static method.  If this hwnd is one of our MDI windows, make it the
	active MDI window.
***************************************************************************/
void GOB::MakeHwndActive(HWND hwnd)
{
	Assert(hwnd != hNil, "nil hwnd");
	GTE gte;
	ulong grfgte;
	PGOB pgob;

	gte.Init(_pgobScreen, fgteNil);
	while (gte.FNextGob(&pgob, &grfgte, fgteNil))
		{
		if (pgob->_hwnd == hwnd)
			{
			SelectWindow(&hwnd->port);
			return;
			}
		}
}


