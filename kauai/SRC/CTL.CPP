/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Standard controls.

***************************************************************************/
#include "frame.h"
ASSERTNAME


//what we set the system max to for scroll bars
const long _klwMaxScroll = 20000;  //should be less than 32K

#ifdef WIN
achar _szCtlProp[] = PszLit("CTL");
#endif //WIN

RTCLASS(CTL)
RTCLASS(SCB)
RTCLASS(WSB)

/***************************************************************************
	Constructor for a control.
***************************************************************************/
CTL::CTL(PGCB pgcb) : GOB(pgcb)
{
	_hctl = hNil;
}


/***************************************************************************
	Destructor for controls.
***************************************************************************/
CTL::~CTL(void)
{
	if (_hctl != hNil)
		{
#ifdef MAC
		RC rc;
		GNV gnv(this);

		rc.Zero();
		gnv.ClipRc(&rc);
		gnv.Set();
		DisposeControl(_hctl);
		gnv.Restore();
#endif //MAC
#ifdef WIN
		RemoveProp(_hctl, _szCtlProp);
		DestroyWindow(_hctl);
#endif //WIN
		_hctl = hNil;
		}
}


/***************************************************************************
	Sets the OS control for the CTL.  If this fails, it frees the control.
***************************************************************************/
bool CTL::_FSetHctl(HCTL hctl)
{
	Assert(_hctl == hNil, "CTL already has an OS control");
	if (hctl != hNil)
		{
#ifdef MAC
		SetCRefCon(hctl, (long)this);
#endif //MAC
#ifdef WIN
		if (!SetProp(hctl, _szCtlProp, (HANDLE)this))
			{
			DestroyWindow(hctl);
			return fFalse;
			}
#endif //WIN
		_hctl = hctl;
		}
	return fTrue;
}


/***************************************************************************
	Return the CTL associated with the given HCTL.
***************************************************************************/
PCTL CTL::PctlFromHctl(HCTL hctl)
{
#ifdef MAC
	return (PCTL)GetCRefCon(hctl);
#endif //MAC
#ifdef WIN
	return (PCTL)GetProp(hctl, _szCtlProp);
#endif //WIN
}


/***************************************************************************
	The control may have been moved - move the OS control.
***************************************************************************/
void CTL::_NewRc(void)
{
	RC rc;
	RCS rcs;
	HWND hwnd;

	if (_hctl == hNil)
		return;

	hwnd = _HwndGetRc(&rc);
	Assert(hwnd != hNil, "control isn't based in an hwnd");
	rcs = RCS(rc);
#ifdef MAC
	RCS rcsOld = (*_hctl)->contrlRect;

	if (!EqualRect(&rcs, &rcsOld))
		{
		GNV gnv(this);

		//clip out everything - hide it, then move and size it
		//don't make it visible again - we do that in the Draw
		//routine
		rc.Zero();
		gnv.ClipRc(&rc);
		gnv.Set();
		HideControl(_hctl);
		MoveControl(_hctl, rcs.left, rcs.top);
		SizeControl(_hctl, rcs.right - rcs.left, rcs.bottom - rcs.top);
		gnv.Restore();
		}
#endif //MAC
#ifdef WIN
	MoveWindow(_hctl, rcs.left, rcs.top,
		rcs.right - rcs.left, rcs.bottom - rcs.top, fFalse);
	InvalidateRect(_hctl, pvNil, fFalse);
#endif //WIN
}


#ifdef MAC
/***************************************************************************
	Draw routine for a control.
***************************************************************************/
void CTL::Draw(PGNV pgnv, RC *prcClip)
{
	if (_hctl == hNil)
		return;
	if (pgnv->Pgpt() == Pgpt())
		{
		pgnv->Set();
		if (!(*_hctl)->contrlVis)
			ShowControl(_hctl);
		else
			Draw1Control(_hctl);
		pgnv->Restore();
		}
	else
		{
		RC rc;
		GNV gnv(this);

		gnv.Set();
		if (!(*_hctl)->contrlVis)
			ShowControl(_hctl);
		else
			Draw1Control(_hctl);
		gnv.Restore();
		GetRcVis(&rc, cooLocal);
		pgnv->CopyPixels(&gnv, &rc, &rc);
		}
}
#endif //MAC


/***************************************************************************
	Static method to create a scroll bar.
***************************************************************************/
PSCB SCB::PscbNew(PGCB pgcb, ulong grfscb, long val, long valMin, long valMax)
{
	Assert(FPure(grfscb & fscbHorz) != FPure(grfscb & fscbVert),
		"exactly one of (fscbHorz,fscbVert) should be set");
	PSCB pscb;
	GCB gcb;

	if (grfscb & fscbStandardRc)
		{
		gcb.Set(pgcb->_hid, pgcb->_pgob, pgcb->_grfgob, pgcb->_gin);
		GetStandardRc(grfscb, &gcb._rcAbs, &gcb._rcRel);
		pgcb = &gcb;
		}

	if (pvNil == (pscb = NewObj SCB(pgcb)))
		return pvNil;

	if (!pscb->_FCreate(val, valMin, valMax, grfscb))
		ReleasePpo(&pscb);

	return pscb;
}


/***************************************************************************
	Static method to return the normal width of a vertical scroll bar.
***************************************************************************/
long SCB::DxpNormal(void)
{
#ifdef WIN
	static _dxp = 0;

	if (_dxp > 0)
		return _dxp;
	return (_dxp = GetSystemMetrics(SM_CXVSCROLL));
#endif //WIN
#ifdef MAC
	return 16;
#endif //MAC
}


/***************************************************************************
	Static method to return the normal width of a horizontal scroll bar.
***************************************************************************/
long SCB::DypNormal(void)
{
#ifdef WIN
	static _dyp = 0;

	if (_dyp > 0)
		return _dyp;
	return (_dyp = GetSystemMetrics(SM_CYHSCROLL));
#endif //WIN
#ifdef MAC
	return 16;
#endif //MAC
}


/***************************************************************************
	Get the standard rectangles for document window scroll bars.  grfscb
	should contain fscbHorz or fscbVert.
***************************************************************************/
void SCB::GetStandardRc(ulong grfscb, RC *prcAbs, RC *prcRel)
{
	if (FPure(grfscb & fscbVert))
		{
		prcRel->ypTop = krelZero;
		prcRel->xpLeft = prcRel->xpRight = prcRel->ypBottom = krelOne;
		prcAbs->ypTop = -!(grfscb & fscbShowTop);
		prcAbs->xpLeft = -SCB::DxpNormal() + !(grfscb & fscbShowRight);
		prcAbs->xpRight = !(grfscb & fscbShowRight);
		prcAbs->ypBottom = -SCB::DypNormal() + 1 + !(grfscb & fscbShowBottom);
		}
	else
		{
		prcRel->xpLeft = krelZero;
		prcRel->ypTop = prcRel->xpRight = prcRel->ypBottom = krelOne;
		prcAbs->ypTop = -SCB::DypNormal() + !(grfscb & fscbShowBottom);
		prcAbs->xpLeft = -!(grfscb & fscbShowLeft);
		prcAbs->xpRight = -SCB::DxpNormal() + 1 + !(grfscb & fscbShowRight);
		prcAbs->ypBottom = !(grfscb & fscbShowBottom);
		}
}


/***************************************************************************
	Get the standard client window rectangle (assuming the given set of
	scroll bars).
***************************************************************************/
void SCB::GetClientRc(ulong grfscb, RC *prcAbs, RC *prcRel)
{
	prcRel->ypTop = prcRel->xpLeft = krelZero;
	prcRel->ypBottom = prcRel->xpRight = krelOne;
	prcAbs->Zero();
	if (grfscb & fscbVert)
		prcAbs->xpRight = -SCB::DxpNormal() + !(grfscb & fscbShowRight);
	if (grfscb & fscbHorz)
		prcAbs->ypBottom = -SCB::DypNormal() + !(grfscb & fscbShowBottom);
}


/***************************************************************************
	Create the actual system scroll bar.
***************************************************************************/
bool SCB::_FCreate(long val, long valMin, long valMax, ulong grfscb)
{
	Assert(_Hctl() == hNil, "scb already created");
	RC rc;
	RCS rcs;
	HWND hwnd;
	HCTL hctl;

	_fVert = FPure(grfscb & fscbVert);
	if ((hwnd = _HwndGetRc(&rc)) == hNil)
		{
		Bug("can only add controls to hwnd based gobs");
		return fFalse;
		}
	rcs = RCS(rc);

#ifdef MAC
	GNV gnv(this);
	gnv.Set();
	hctl = NewControl(&hwnd->port, &rcs, (byte *)"\p", fTrue, 0, 0, 0,
		scrollBarProc, 0);
	gnv.Restore();
	if (hctl == hNil || !_FSetHctl(hctl))
		return fFalse;
	ValidRc(pvNil);
#endif //MAC
#ifdef WIN
	hctl = CreateWindow(PszLit("SCROLLBAR"), PszLit(""),
		(_fVert ? SBS_VERT : SBS_HORZ) | WS_CHILD | WS_VISIBLE,
		rcs.left, rcs.top, rcs.right - rcs.left, rcs.bottom - rcs.top,
		hwnd, hNil, vwig.hinst, pvNil);
	if (hctl == hNil || !_FSetHctl(hctl))
		return fFalse;
	SetScrollRange(hctl, SB_CTL, 0, _klwMaxScroll, fFalse);
	SetScrollPos(hctl, SB_CTL, 0, fFalse);
#endif //WIN

	SetValMinMax(val, valMin, valMax, MacWin(fTrue, fFalse));
	return fTrue;
}


/***************************************************************************
	Set the value of the scroll bar.
***************************************************************************/
void SCB::SetVal(long val, bool fRedraw)
{
	long lwCur;

	val = LwMin(_valMax, LwMax(_valMin, val));
	if (val == _val)
		return;
	_val = val;
	lwCur = LwMulDiv(_klwMaxScroll, _val - _valMin,
		LwMax(1, _valMax - _valMin));

#ifdef MAC
	//REVIEW shonk: Mac: implement fRedraw false
	GNV gnv(this);
	gnv.Set();
	SetCtlValue(_Hctl(), (short)lwCur);
	gnv.Restore();
#endif //MAC
#ifdef WIN
	SetScrollPos(_Hctl(), SB_CTL, lwCur, fRedraw);
#endif //WIN
}


/***************************************************************************
	Set the min and max of the scroll bar.
***************************************************************************/
void SCB::SetValMinMax(long val, long valMin, long valMax, bool fRedraw)
{
	long lwCur;

	valMax = LwMax(valMin, valMax);
	val = LwMin(valMax, LwMax(valMin, val));

	if (val == _val && valMin == _valMin && valMax == _valMax)
		return;
	_val = val;
	_valMin = valMin;
	_valMax = valMax;
	lwCur = LwMulDiv(_klwMaxScroll, _val - _valMin,
		LwMax(1, _valMax - _valMin));

#ifdef MAC
	//REVIEW shonk: Mac: implement fRedraw false
	GNV gnv(this);
	gnv.Set();
	if (_valMax == _valMin)
		SetCtlMax(_Hctl(), 0);
	else
		SetCtlMax(_Hctl(), (short)_klwMaxScroll);
	SetCtlValue(_Hctl(), (short)lwCur);
	gnv.Restore();
#endif //MAC
#ifdef WIN
	SetScrollPos(_Hctl(), SB_CTL, lwCur, fRedraw);
#endif //WIN
}


#ifdef MAC
/***************************************************************************
	The hwnd has been activated or deactivated - redraw and validate.
***************************************************************************/
void SCB::_ActivateHwnd(bool fActive)
{
	if (_valMin < _valMax)
		{
		GNV gnv(this);
		long lwCur;

		gnv.Set();
		SetCtlMax(_Hctl(), (short)(fActive ? _klwMaxScroll : 0));
		if (fActive)
			{
			lwCur = LwMulDiv(_klwMaxScroll, _val - _valMin,
				LwMax(1, _valMax - _valMin));
			SetCtlValue(_Hctl(), (short)lwCur);
			}
		gnv.Restore();
		if (!(*_Hctl())->contrlVis)
			{
			ValidRc(pvNil, kginDraw);
			InvalRc(pvNil, kginDraw); //this makes it visible
			}
		}
}


/***************************************************************************
	Handle mouse tracking for a scroll bar.
***************************************************************************/
void SCB::MouseDown(long xp, long yp, long cact, ulong grfcust)
{
	PTS pts;
	short in;
	PT pt;
	bool fDown, fLit;
	CMD cmd;

	GNV gnv(this);

	pts.h = (short)xp + (*_Hctl())->contrlRect.left;
	pts.v = (short)yp + (*_Hctl())->contrlRect.top;
	in = TestControl(_Hctl(), pts);
	switch (in)
		{
	default:
		break;

	case inThumb:
		gnv.Set();
		in = TrackControl(_Hctl(), pts, pvNil);
		gnv.Restore();

		//send the final position - this will get recorded
		//note that the _Hctl() has the wrong value so the
		//gob should call SetValue in response to this command
		vpcex->EnqueueCid(cidEndScroll, PgobPar(), pvNil, Hid(),
			_valMin + LwMulDiv((long)GetCtlValue(_Hctl()),
				_valMax - _valMin, _klwMaxScroll));
		break;

	case inUpButton:
	case inDownButton:
	case inPageUp:
	case inPageDown:
		fDown = fTrue;
		pt.xp = xp;
		pt.yp = yp;
		for ( ; ; GetPtMouse(&pt, &fDown))
			{
			//get the new hilite state
			pts.h = (short)pt.xp + (*_Hctl())->contrlRect.left;
			pts.v = (short)pt.yp + (*_Hctl())->contrlRect.top;
			fLit = fDown && (TestControl(_Hctl(), pts) == in);

			//hilite the scroll bar appropriately
			gnv.Set();
			if (fLit)
				HiliteControl(_Hctl(), in);
			else
				HiliteControl(_Hctl(), 0);
			gnv.Restore();

			//see if we're done
			if (!fDown)
				break;

			if (fLit)
				{
				//send the command - this doesn't get recorded
				ClearPb(&cmd, size(cmd));
				cmd.cid = cidDoScroll;
				cmd.pcmh = PgobPar();
				cmd.rglw[0] = Hid();
				switch (in)
					{
				case inUpButton:
					cmd.rglw[1] = scaLineUp;
					break;
				case inPageUp:
					cmd.rglw[1] = scaPageUp;
					break;
				case inDownButton:
					cmd.rglw[1] = scaLineDown;
					break;
				case inPageDown:
					cmd.rglw[1] = scaPageDown;
					break;
				default:
					BugVar("what is the in value?", &in);
					cmd.rglw[1] = scaNil;
					break;
					}
				cmd.pcmh->FDoCmd(&cmd);
				}
			}

		//send the final position - this will get recorded
		vpcex->EnqueueCid(cidEndScroll, PgobPar(), pvNil, Hid(), _val);
		break;
		}
}
#endif //MAC


#ifdef WIN
/***************************************************************************
	Called in response to a Win WM_HSCROLL or WM_VSCROLL message.
***************************************************************************/
void SCB::TrackScroll(long sb, long lwVal)
{
	AssertThis(0);
	CMD cmd;
	long val;

	ClearPb(&cmd, size(cmd));
	cmd.cid = cidDoScroll;
	cmd.pcmh = PgobPar();
	cmd.rglw[0] = Hid();

	switch (sb)
		{
	case SB_LINEUP:
		cmd.rglw[1] = scaLineUp;
		break;
	case SB_LINEDOWN:
		cmd.rglw[1] = scaLineDown;
		break;

	case SB_PAGEUP:
		cmd.rglw[1] = scaPageUp;
		break;
	case SB_PAGEDOWN:
		cmd.rglw[1] = scaPageDown;
		break;
	case SB_THUMBTRACK:
		lwVal = LwMax(0, LwMin(_klwMaxScroll, lwVal));
		cmd.rglw[1] = scaToVal;
		cmd.rglw[2] = _valMin + LwMulDiv(lwVal, _valMax - _valMin, _klwMaxScroll);
		break;

	//these values just push an end-scroll command
	case SB_THUMBPOSITION:
		_fSentEndScroll = fFalse;
		lwVal = LwMax(0, LwMin(_klwMaxScroll, lwVal));
		val = _valMin + LwMulDiv(lwVal, _valMax - _valMin, _klwMaxScroll);
		goto LEndScroll;
	case SB_TOP:
		_fSentEndScroll = fFalse;
		val = _valMin;
		goto LEndScroll;
	case SB_BOTTOM:
		_fSentEndScroll = fFalse;
		val = _valMax;
		goto LEndScroll;
	case SB_ENDSCROLL:
		//NOTE: _fSentEndScroll is so we don't send another EndScroll
		//here if we've already sent one (in response to an SB_THUMBPOSITION,
		//SB_TOP or SB_BOTTOM), since this value will generally be wrong.
		val = _val;
		goto LEndScroll;

	default:
		//do nothing
		return;
		}

	_fSentEndScroll = fFalse;
	cmd.pcmh->FDoCmd(&cmd);
	return;

LEndScroll:
	if (!_fSentEndScroll)
		{
		cmd.cid = cidEndScroll;
		cmd.rglw[1] = val;
		vpcex->EnqueueCmd(&cmd);
		_fSentEndScroll = fTrue;
		}
}
#endif //WIN


/***************************************************************************
	Static method to create a window size box.
***************************************************************************/
PWSB WSB::PwsbNew(PGOB pgob, ulong grfgob)
{
	RC rcRel, rcAbs;
	PWSB pwsb;

	rcRel.xpLeft = rcRel.xpRight = rcRel.ypTop = rcRel.ypBottom = krelOne;
	rcAbs.xpLeft = -SCB::DxpNormal() + 1;
	rcAbs.ypTop = -SCB::DypNormal() + 1;
	rcAbs.xpRight = rcAbs.ypBottom = 1;

	GCB gcb(khidSizeBox, pgob, grfgob, kginDefault, &rcAbs, &rcRel);
	if ((pwsb = NewObj WSB(&gcb)) == pvNil)
		return pvNil;

	Assert(pwsb->PgobPar() != pvNil, "nil parent");
	Assert(pwsb->PgobPar()->Hwnd() != hNil,
		"parent of size box doesn't have an hwnd");

#ifdef WIN
	RC rc;
	RCS rcs;
	HWND hwnd;
	HCTL hctl;

	hwnd = pwsb->_HwndGetRc(&rc);
	rcs = RCS(rc);

	hctl = CreateWindow(PszLit("SCROLLBAR"), PszLit(""),
		SBS_SIZEBOX | WS_CHILD | WS_VISIBLE,
		rcs.left, rcs.top, rcs.right - rcs.left, rcs.bottom - rcs.top,
		hwnd, hNil, vwig.hinst, pvNil);

	if (hctl == hNil || !pwsb->_FSetHctl(hctl))
		ReleasePpo(&pwsb);
#endif

	return pwsb;
}


#ifdef MAC
/***************************************************************************
	Draw the size box icon.
***************************************************************************/
void WSB::Draw(PGNV pgnv, RC *prcClip)
{
	HWND hwnd;
	RC rc;

	hwnd = PgobPar()->Hwnd();
	Assert(hwnd != hNil, "hwnd nil");
	GetRcVis(&rc, cooLocal);

	if (pgnv->Pgpt() == Pgpt())
		{
		pgnv->ClipRc(&rc);
		pgnv->Set();
		DrawGrowIcon(&hwnd->port);
		pgnv->Restore();
		}
	else
		{
		GNV gnv(this);

		gnv.ClipRc(&rc);
		gnv.Set();
		DrawGrowIcon(&hwnd->port);
		gnv.Restore();
		pgnv->CopyPixels(&gnv, &rc, &rc);
		}
}


/***************************************************************************
	The hwnd has been activated or deactivated - redraw and validate.
***************************************************************************/
void WSB::_ActivateHwnd(bool fActive)
{
	ValidRc(pvNil, kginDraw);
	InvalRc(pvNil, kginDraw);
}
#endif //MAC


