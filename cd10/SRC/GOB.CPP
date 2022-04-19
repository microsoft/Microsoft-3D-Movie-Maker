/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Graphics object code.

***************************************************************************/
#include "frame.h"
ASSERTNAME


BEGIN_CMD_MAP(GOB, CMH)
	ON_CID_GEN(cidKey, &GOB::FCmdKeyCore, pvNil)
	ON_CID_GEN(cidSelIdle, &GOB::FCmdSelIdle, pvNil)
	ON_CID_ME(cidActivateSel, &GOB::FCmdActivateSel, pvNil)
	ON_CID_ME(cidBadKey, &GOB::FCmdBadKeyCore, pvNil)
	ON_CID_ME(cidCloseWnd, &GOB::FCmdCloseWnd, pvNil)
	ON_CID_ME(cidMouseDown, &GOB::FCmdTrackMouseCore, pvNil)
	ON_CID_ME(cidTrackMouse, &GOB::FCmdTrackMouseCore, pvNil)
	ON_CID_ME(cidMouseMove, &GOB::FCmdMouseMoveCore, pvNil)
END_CMD_MAP_NIL()

RTCLASS(GOB)
RTCLASS(GTE)


long GOB::_ginDefGob = kginSysInval;
long GOB::_gridLast;


/***************************************************************************
	Fill in the elements of the GCB.
***************************************************************************/
void GCB::Set(long hid, PGOB pgob, ulong grfgob, long gin, RC *prcAbs, RC *prcRel)
{
	Assert(hidNil != hid, "bad hid");
	AssertNilOrPo(pgob, 0);
	_hid = hid;
	_pgob = pgob;
	_grfgob = grfgob;
	_gin = gin;
	if (pvNil == prcAbs)
		_rcAbs.Zero();
	else
		_rcAbs = *prcAbs;
	if (pvNil == prcRel)
		_rcRel.Zero();
	else
		_rcRel = *prcRel;
}


/***************************************************************************
	Static method to shut down all GOBs.
***************************************************************************/
void GOB::ShutDown(void)
{
	while (pvNil != _pgobScreen)
		{
		_pgobScreen->FAttachHwnd(hNil);

		// freeing the _pgobScreen also updates _pgobScreen to its sibling.
		// _pgobScreen is really the root of the forest.
		_pgobScreen->Release();
		}
}


/***************************************************************************
	Constructor for a graphics object.  pgob is either the parent of the new
	gob or a sibling, according to (grfgob & fgobSibling).
***************************************************************************/
GOB::GOB(PGCB pgcb) : CMH(pgcb->_hid)
{
	_Init(pgcb);
}


/***************************************************************************
	Initialize the gob.
***************************************************************************/
void GOB::_Init(PGCB pgcb)
{
	AssertVarMem(pgcb);
	AssertNilOrPo(pgcb->_pgob, 0);

	_grid = ++_gridLast;
	_ginDefault = pgcb->_gin;
	_fCreating = fTrue;

	if (pvNil == pgcb->_pgob)
		{
		Assert(pvNil == _pgobScreen, "screen gob already created");
		_pgobScreen = this;
		}
	else if (pgcb->_grfgob & fgobSibling)
		{
		AssertPo(pgcb->_pgob, 0);
		_pgobPar = pgcb->_pgob->_pgobPar;
		_pgobSib = pgcb->_pgob->_pgobSib;
		pgcb->_pgob->_pgobSib = this;
		}
	else
		{
		AssertPo(pgcb->_pgob, 0);
		_pgobPar = pgcb->_pgob;
		_pgobSib = pgcb->_pgob->_pgobChd;
		pgcb->_pgob->_pgobChd = this;
		}

	if (pvNil != _pgobPar)
		_pgpt = _pgobPar->_pgpt;
	SetPos(&pgcb->_rcAbs, &pgcb->_rcRel);
	AssertThis(0);

	_fCreating = fFalse;
}


/***************************************************************************
	Constructor for GOB.
***************************************************************************/
GOB::GOB(long hid) : CMH(hid)
{
	GCB gcb(hid, GOB::PgobScreen());
	_Init(&gcb);
}


/***************************************************************************
	First tells the app that the gob is dying; then calls Release on all direct
	child gobs of this GOB; then calls delete on itself.
***************************************************************************/
void GOB::Release(void)
{
	AssertThis(0);
	PGOB pgob;

	if (--_cactRef > 0)
		return;

	//Mark this gob as being freed (may already be marked)
	_fFreeing = fTrue;

	//invalidate
	if (pvNil == _pgobPar || !_pgobPar->_fFreeing)
		InvalRc(pvNil);

	while (pvNil != (pgob = _pgobChd))
		pgob->Release();

	delete this;
}


/***************************************************************************
	Destructor for the graphics object class.
***************************************************************************/
GOB::~GOB(void)
{
	AssertThis(0);
	PGOB *ppgob;

	//remove it from the sibling list
	Assert(pvNil == _pgobChd, "gob still has children");
	for (ppgob = pvNil != _pgobPar ? &_pgobPar->_pgobChd : &_pgobScreen;
			*ppgob != this && pvNil != *ppgob;
			ppgob = &(*ppgob)->_pgobSib)
		{
		}
	if (*ppgob == this)
		*ppgob = _pgobSib;
	else
		Bug("corrupt gob tree");

	//nuke its port and hwnd
	if (pvNil != _pgpt && (pvNil == _pgobPar || _pgpt != _pgobPar->_pgpt))
		ReleasePpo(&_pgpt);
	if (_hwnd != hNil)
		_DestroyHwnd(_hwnd);
	ReleasePpo(&_pglrtvm);
	ReleasePpo(&_pcurs);
}


/***************************************************************************
	Called by OS specific code when an hwnd is activated or deactivated.
	We inform the entire gob subtree for the hwnd so individual elements
	can do whatever is necessary.  This is a static member function.
***************************************************************************/
void GOB::ActivateHwnd(HWND hwnd, bool fActive)
{
	PGOB pgob;

	if (pvNil == (pgob = PgobFromHwnd(hwnd)))
		return;

	//if it's becoming active, bring it to the front in our gob tree.
	if (fActive)
		pgob->SendBehind(pvNil);

	GTE gte;
	ulong grfgte;

	gte.Init(pgob, fgteBackToFront);
	while (gte.FNextGob(&pgob, &grfgte, fgteNil))
		{
		if (grfgte & fgtePre)
			pgob->_ActivateHwnd(fActive);
		}
}


/***************************************************************************
	Make this the first child of its parent.  Doesn't invalidate anything.
***************************************************************************/
void GOB::BringToFront(void)
{
	AssertThis(0);
	SendBehind(pvNil);
}


/***************************************************************************
	Put this GOB behind the given sibling.  If pgobBehind is nil, does
	the equivalent of a BringToFront.  Asserts that pgobBehind and this
	gob have the same parent.  Does no invalidation.
***************************************************************************/
void GOB::SendBehind(PGOB pgobBehind)
{
	AssertThis(0);
	AssertNilOrPo(pgobBehind, 0);
	PGOB pgob;

	if (pvNil != pgobBehind && pgobBehind->_pgobPar != _pgobPar)
		{
		Bug("don't have the same parent");
		return;
		}

	pgob = PgobPrevSib();
	if (pgob == pgobBehind)
		return; //nothing to do

	//take this gob out of the sibling list
	if (pvNil == pgob)
		{
		Assert(_pgobPar->_pgobChd == this, "corrupt GOB tree");
		_pgobPar->_pgobChd = _pgobSib;
		}
	else
		{
		Assert(pgob->_pgobSib == this, "corrupt GOB tree");
		pgob->_pgobSib = _pgobSib;
		}

	//now insert it after pgobBehind
	if (pvNil == pgobBehind)
		{
		_pgobSib = _pgobPar->_pgobChd;
		_pgobPar->_pgobChd = this;
		}
	else
		{
		_pgobSib = pgobBehind->_pgobSib;
		pgobBehind->_pgobSib = this;
		AssertPo(pgobBehind, 0);
		}
	AssertThis(0);
}


/***************************************************************************
	Invalidate the given rc in this gob.  If gin is ginNil, nothing is done.
	If gin is kginRedraw, the area is redraw.  If gin is kginMark, the area
	is marked dirty at the framework level.  If gin is kginSysInval, the
	area is marked dirty at the operating system level.  In all cases,
	passing pvNil for prc affects the whole gob.
***************************************************************************/
void GOB::InvalRc(RC *prc, long gin)
{
	AssertThis(0);
	AssertNilOrVarMem(prc);
	PT dpt;
	RC rc;
	PGOB pgob;
	RCS rcs;

	if (kginDefault == gin)
		{
		gin = _ginDefault;
		if (kginDefault == gin)
			gin = _ginDefGob;
		}

	if (ginNil == gin)
		return;

	GetRcVis(&rc, cooLocal);
	if (pvNil != prc)
		rc.FIntersect(prc);
	if (rc.FEmpty())
		return;

	for (pgob = this; pvNil != pgob && pgob->_hwnd == hNil;
			pgob = pgob->_pgobPar)
		{
		rc.Offset(pgob->_rcCur.xpLeft, pgob->_rcCur.ypTop);
		}
	if (pvNil == pgob)
		return;

	switch (gin)
		{
	default:
		Bug("bad gin value");
		break;

	case kginDraw:
		//do this so we do whatever the app does during a normal draw, such
		//as drawing offscreen....
		vpappb->UpdateHwnd(pgob->_hwnd, &rc);
		break;

	case kginMark:
		vpappb->MarkRc(&rc, pgob);
		break;

	case kginSysInval:
		rcs = RCS(rc);
		InvalHwndRcs(pgob->_hwnd, &rcs);
		break;
		}
}


/***************************************************************************
	Validate the given rc in this gob.  If gin is ginNil, nothing is done.
	If gin is kginRedraw, the area is validated at both the framework level
	and the system level.  If gin is kginMark or kginSysInval, the area is
	validated only at the given level.  In any case, passing pvNil for prc
	affects the whole gob.
***************************************************************************/
void GOB::ValidRc(RC *prc, long gin)
{
	AssertThis(0);
	RC rc;
	PT dpt;
	PGOB pgob;

	if (kginDefault == gin)
		{
		gin = _ginDefault;
		if (kginDefault == gin)
			gin = _ginDefGob;
		}

	if (ginNil == gin)
		return;

	GetRcVis(&rc, cooLocal);
	if (pvNil != prc)
		rc.FIntersect(prc);
	if (rc.FEmpty())
		return;

	for (pgob = this; pvNil != pgob && pgob->_hwnd == hNil;
			pgob = pgob->_pgobPar)
		{
		rc.Offset(pgob->_rcCur.xpLeft, pgob->_rcCur.ypTop);
		}
	if (pvNil == pgob)
		return;

	if (gin != kginSysInval)
		{
		// do a framework level validation
		vpappb->UnmarkRc(&rc, pgob);
		}

	if (gin != kginMark)
		{
		// do a system level validation
		RCS rcs;

		rcs = RCS(rc);
		ValidHwndRcs(pgob->_hwnd, &rcs);
		}
}


/***************************************************************************
	Get the dirty portion of this gob.  Return true iff the dirty rectangle
	is non-empty.  If gin is kginDraw, gets the union of the marked area
	and system-invalidated area.
***************************************************************************/
bool GOB::FGetRcInval(RC *prc, long gin)
{
	AssertThis(0);
	AssertVarMem(prc);
	RC rc;
	PGOB pgob;
	PT dpt(0, 0);

	prc->Zero();
	if (kginDefault == gin)
		{
		gin = _ginDefault;
		if (kginDefault == gin)
			gin = _ginDefGob;
		}

	GetRcVis(&rc, cooLocal);
	if (rc.FEmpty() || ginNil == gin)
		return fFalse;

	for (pgob = this; pvNil != pgob && pgob->_hwnd == hNil;
			pgob = pgob->_pgobPar)
		{
		dpt.Offset(pgob->_rcCur.xpLeft, pgob->_rcCur.ypTop);
		}
	rc.Offset(dpt.xp, dpt.yp);
	if (pvNil == pgob)
		return fFalse;

	if (kginSysInval != gin)
		{
		//get any marked area
		vpappb->FGetMarkedRc(pgob->_hwnd, prc);
		}

	if (kginMark != gin)
		{
		//get any system invalidated area
		RCS rcs;
		RC rcT;

#ifdef WIN
		GetUpdateRect(pgob->_hwnd, &rcs, fFalse);
#endif //WIN
#ifdef MAC
		PPRT pprt;

		rcs = (*pgob->_hwnd->updateRgn)->rgnBBox;
		GetPort(&pprt);
		SetPort(&pgob->_hwnd->port);
		GlobalToLocal((PTS *)&rcs);
		GlobalToLocal((PTS *)&rcs + 1);
		SetPort(pprt);
#endif //MAC
		rcT = RC(rcs);
		if (rcT.FIntersect(&rc))
			prc->Union(&rcT);
		}
	prc->Offset(-dpt.xp, -dpt.yp);

	return !prc->FEmpty();
}


/***************************************************************************
	Scrolls the given rectangle in the GOB.  Translates any invalid portion.
	Handles this being covered by any GOBs or system windows.  If prc is
	nil, the entire content rectangle is used.
***************************************************************************/
void GOB::Scroll(RC *prc, long dxp, long dyp, long gin,
	RC *prcBad1, RC *prcBad2)
{
	AssertThis(0);
	AssertNilOrVarMem(prc);
	AssertNilOrVarMem(prcBad1);
	AssertNilOrVarMem(prcBad2);
	RC rc, rcBad1, rcBad2, rcInval, rcT;
	PT dpt(0, 0);
	PGOB pgob, pgobT;
	GTE gte;
	ulong grfgte, grfgteIn;
	bool fFound;

	if (kginDefault == gin)
		{
		gin = _ginDefault;
		if (kginDefault == gin)
			gin = _ginDefGob;
		}

	if (pvNil != prcBad1)
		prcBad1->Zero();
	if (pvNil != prcBad2)
		prcBad2->Zero();

	if (dxp == 0 && dyp == 0)
		return;

	GetRcVis(&rc, cooLocal);
	if (pvNil != prc && !rc.FIntersect(prc))
		return;

	for (pgob = this; pvNil != pgob && pgob->_hwnd == hNil;
			pgob = pgob->_pgobPar)
		{
		dpt.Offset(pgob->_rcCur.xpLeft, pgob->_rcCur.ypTop);
		}
	rc.Offset(dpt.xp, dpt.yp);
	if (pvNil == pgob)
		return;

	//check for GOBs on top of this one.
	gte.Init(pgob, fgteBackToFront);
	fFound = fFalse;
	grfgteIn = fgteNil;
	while (gte.FNextGob(&pgobT, &grfgte, grfgteIn))
		{
		if (!(grfgte & fgtePre))
			continue;

		if (!fFound)
			{
			fFound = pgobT == this;
			continue;
			}
		pgobT->GetRc(&rcT, cooHwnd);
		if (rcT.FIntersect(&rc))
			{
			//there is a GOB on top of this one, just invalidate the
			//rectangle to be scrolled
			pgob->ValidRc(&rc, kginDraw);
			pgob->InvalRc(&rc, gin);
			if (pvNil != prcBad1)
				prcBad1->OffsetCopy(&rc, -dpt.xp, -dpt.yp);
			return;
			}
		grfgteIn = fgteSkipToSib;
		}

	//translate any marked area
	if (FGetRcInval(&rcT, kginMark))
		{
		//something is marked
		rcT.Offset(dpt.xp, dpt.yp);
		if (rcT.FIntersect(&rc))
			{
			pgob->ValidRc(&rcT, kginMark);
			rcT.Offset(dxp, dyp);
			if (rcT.FIntersect(&rc))
				pgob->InvalRc(&rcT, kginMark);
			}
		}

#ifdef WIN
	// SW_INVALIDATE invalidates any uncovered stuff and translates any
	// previously invalid stuff
	RCS rcs = RCS(rc);
	ScrollWindowEx(pgob->_hwnd,	dxp, dyp, pvNil, &rcs,
		hNil, pvNil, SW_INVALIDATE);

	// compute the bad rectangles
	GNV::GetBadRcForScroll(&rc, dxp, dyp, &rcBad1, &rcBad2);

	if (pvNil != prcBad1)
		prcBad1->OffsetCopy(&rcBad1, -dpt.xp, -dpt.yp);
	if (pvNil != prcBad2)
		prcBad2->OffsetCopy(&rcBad2, -dpt.xp, -dpt.yp);

	switch (gin)
		{
	default:
		Bug("bad gin");
		break;
	case kginDraw:
		UpdateWindow(pgob->_hwnd);
		break;
	case kginSysInval:
		break;
	case kginMark:
		vpappb->MarkRc(&rcBad1, pgob);
		vpappb->MarkRc(&rcBad2, pgob);
		// fall through
	case ginNil:
		if (!rcBad1.FEmpty())
			{
			rcs = RCS(rcBad1);
			ValidateRect(pgob->_hwnd, &rcs);
			}
		if (!rcBad2.FEmpty())
			{
			rcs = RCS(rcBad2);
			ValidateRect(pgob->_hwnd, &rcs);
			}
		break;
		}
#endif //WIN
#ifdef MAC
	HRGN hrgn;

	//Make sure the vis region intersected with the rectangle to scroll is
	//a rectangle
	if (!FCreateRgn(&hrgn, &rc) ||
		!FIntersectRgn(hrgn, pgob->_hwnd->port.visRgn, hrgn) ||
		!FRectRgn(hrgn, &rc))
		{
		//there is something on top of this one, just invalidate the
		//rectangle to be scrolled
		FreePhrgn(&hrgn);
		pgob->ValidRc(&rc, kginDraw);
		pgob->InvalRc(&rc, gin);
		if (pvNil != prcBad1)
			prcBad1->OffsetCopy(&rc, -dpt.xp, -dpt.yp);
		return;
		}
	FreePhrgn(&hrgn);

	GNV gnv(pgob);
	gnv.ScrollRc(&rc, dxp, dyp, &rcBad1, &rcBad2);

	//translate any invalid area
	if (FGetRcInval(&rcT, kginSysInval))
		{
		//something is invalid
		rcT.Offset(dpt.xp, dpt.yp);
		if (rcT.FIntersect(&rc))
			{
			pgob->ValidRc(&rcT, kginSysInval);
			rcT.Offset(dxp, dyp);
			if (rcT.FIntersect(&rc))
				pgob->InvalRc(&rcT, kginSysInval);
			}
		}

	if (pvNil != prcBad1)
		prcBad1->OffsetCopy(&rcBad1, -dpt.xp, -dpt.yp);
	if (pvNil != prcBad2)
		prcBad2->OffsetCopy(&rcBad2, -dpt.xp, -dpt.yp);

	switch (gin)
		{
	default:
		Bug("bad gin");
		//fall through
	case ginNil:
		break;
	case kginDraw:
		vpappb->MarkRc(&rcBad1, pgob);
		vpappb->MarkRc(&rcBad2, pgob);
		vpappb->UpdateMarked();
		break;
	case kginSysInval:
		pgob->InvalRc(&rcBad1);
		pgob->InvalRc(&rcBad2);
		break;
	case kginMark:
		vpappb->MarkRc(&rcBad1, pgob);
		vpappb->MarkRc(&rcBad2, pgob);
		break;
		}
#endif //MAC
}


/***************************************************************************
	Draw the gob and its children into the given port.  If the pgpt is nil,
	use the GOB's UI (natural) port.  If the prc is pvNil, use the GOB's
	rectangle based at (0, 0).  If prcClip is not nil, only GOB's that
	intersect prcClip will be drawn.  prcClip is in the GOB's local
	coordinates.
***************************************************************************/
void GOB::DrawTree(PGPT pgpt, RC *prc, RC *prcClip, ulong grfgob)
{
	AssertThis(0);
	AssertNilOrPo(pgpt, 0);
	AssertNilOrVarMem(prc);
	AssertNilOrVarMem(prcClip);
	RC rcSrc, rcClip, rcSrcGob, rcClipGob, rcVis, rc;
	//to translate from this->local to pgob->local coordinates add dpt
	PT dpt;

	if (pgpt == pvNil && (pgpt = _pgpt) == pvNil)
		{
		Bug("no port to draw to");
		return;
		}
	if (pvNil != prc && prc->FEmpty())
		return;

	dpt = _rcCur.PtTopLeft();

	//get the source and clip rectangles in local (this) coordinates
	rcSrc = _rcCur;
	if (rcSrc.FEmpty())
		return;
	rcSrc.OffsetToOrigin();
	if (pvNil == prcClip)
		rcClip = rcSrc;
	else if (!rcClip.FIntersect(prcClip, &rcSrc))
		return;

	GNV gnv(pgpt);
	GTE gte;
	ulong grfgte, grfgteIn;
	PGOB pgob;

	gte.Init(this, fgteBackToFront);
	grfgteIn = fgteNil;
	while (gte.FNextGob(&pgob, &grfgte, grfgteIn))
		{
		if (pgob->_pgpt != _pgpt || pgob->_rcCur.FEmpty())
			goto LNextSib;

		grfgteIn = fgteNil;
		if (grfgte & fgtePre)
			{
			if (grfgob & (fgobAutoVis | fgobUseVis))
				{
				pgob->GetRcVis(&rcVis, cooLocal);
				if (rcVis.FEmpty())
					goto LNextSib;
				}

			//get the source and clip rectangles in local (pgob) coordinates
			rcSrcGob = pgob->_rcCur;
			dpt.xp -= rcSrcGob.xpLeft;
			dpt.yp -= rcSrcGob.ypTop;
			rcSrcGob.OffsetToOrigin();
			rcClipGob = rcClip + dpt;
			if (!rcClipGob.FIntersect(&rcSrcGob))
				goto LOffsetNextSib;

			// set the source rectangle
			gnv.SetRcSrc(&rcSrcGob);

			//set the dest rc
			rc = rcSrcGob - dpt;
			if (pvNil != prc)
				rc.Map(&rcSrc, prc);
			gnv.SetRcDst(&rc);

			//set the vis rectangle
			if (grfgob & (fgobAutoVis | fgobUseVis))
				{
				if (!rcVis.FIntersect(&rcClipGob))
					{
LOffsetNextSib:
					dpt.xp += pgob->_rcCur.xpLeft;
					dpt.yp += pgob->_rcCur.ypTop;
LNextSib:
					grfgteIn = fgteSkipToSib;
					continue;
					}

				if (!(grfgob & fgobUseVis) && rcSrcGob == rcVis)
					gnv.SetRcVis(pvNil);
				else
					gnv.SetRcVis(&rcVis);
				rcClipGob = rcVis;
				}

			//draw the gob
			pgob->Draw(&gnv, &rcClipGob);
			}
		if (grfgte & fgtePost)
			{
			dpt.xp += pgob->_rcCur.xpLeft;
			dpt.yp += pgob->_rcCur.ypTop;
			}
		}
}


/***************************************************************************
	Draw the gob and its children into the given port.  If the pgpt is nil,
	use the GOB's UI (natural) port.  If the prc is pvNil, use the GOB's
	rectangle based at (0, 0).  Only GOB's that intersect pregn will be
	drawn.  pregn is in the GOB's local coordinates.
***************************************************************************/
void GOB::DrawTreeRgn(PGPT pgpt, RC *prc, REGN *pregn, ulong grfgob)
{
	AssertThis(0);
	AssertNilOrPo(pgpt, 0);
	AssertNilOrVarMem(prc);
	AssertPo(pregn, 0);
	RC rcSrc, rcSrcGob, rcClipGob, rcVis, rc;
	//to translate from this->local to pgob->local coordinates add dpt
	PT dpt;

	if (pgpt == pvNil && (pgpt = _pgpt) == pvNil)
		{
		Bug("no port to draw to");
		return;
		}
	if (pvNil != prc && prc->FEmpty())
		return;
	if (pregn->FEmpty())
		return;

	dpt = _rcCur.PtTopLeft();

	//get the source rectangle and clip region in local (this) coordinates
	rcSrc = _rcCur;
	rcSrc.OffsetToOrigin();
	if (rcSrc.FEmpty())
		return;

	GNV gnv(pgpt);
	GTE gte;
	ulong grfgte, grfgteIn;
	PGOB pgob;
	PREGN pregnClip;
	PREGN pregnClipGob = pvNil;

	if (pvNil == (pregnClip = REGN::PregnNew(&rcSrc)) ||
			pvNil == (pregnClipGob = REGN::PregnNew()) ||
			!pregnClip->FIntersect(pregn))
		{
		goto LFail;
		}
	if (pregnClip->FEmpty())
		goto LDone;

	gte.Init(this, fgteBackToFront);
	grfgteIn = fgteNil;
	while (gte.FNextGob(&pgob, &grfgte, grfgteIn))
		{
		if (pgob->_pgpt != _pgpt || pgob->_rcCur.FEmpty())
			goto LNextSib;

		grfgteIn = fgteNil;
		if (grfgte & fgtePre)
			{
			if (grfgob & (fgobAutoVis | fgobUseVis))
				{
				pgob->GetRcVis(&rcVis, cooLocal);
				if (rcVis.FEmpty())
					goto LNextSib;
				}

			//get the source and clip rectangles in local (pgob) coordinates
			rcSrcGob = pgob->_rcCur;
			dpt.xp -= rcSrcGob.xpLeft;
			dpt.yp -= rcSrcGob.ypTop;
			rcSrcGob.OffsetToOrigin();

			pregnClipGob->SetRc(&rcSrcGob);
			pregnClipGob->Offset(-dpt.xp, -dpt.yp);
			if (!pregnClipGob->FIntersect(pregnClip))
				goto LFail;
			if (pregnClipGob->FEmpty(&rcClipGob))
				goto LOffsetNextSib;

			rcClipGob.Offset(dpt.xp, dpt.yp);

			// set the source rectangle
			gnv.SetRcSrc(&rcSrcGob);

			//set the dest rc
			rc = rcSrcGob - dpt;
			if (pvNil != prc)
				rc.Map(&rcSrc, prc);
			gnv.SetRcDst(&rc);

			//set the vis rectangle
			if (grfgob & (fgobAutoVis | fgobUseVis))
				{
				if (!rcVis.FIntersect(&rcClipGob))
					{
LOffsetNextSib:
					dpt.xp += pgob->_rcCur.xpLeft;
					dpt.yp += pgob->_rcCur.ypTop;
LNextSib:
					grfgteIn = fgteSkipToSib;
					continue;
					}

				if (!(grfgob & fgobUseVis) && rcSrcGob == rcVis)
					gnv.SetRcVis(pvNil);
				else
					gnv.SetRcVis(&rcVis);
				}

			// draw the gob
			// NOTE: we use pregn and not pregnClip or pregnClipGob for speed.
			// Using pregn, the cached hrgn stuff kicks in to only require
			// one hrgn creation. If we use pregnClip we only have one hrgn
			// creation here, but another one when the offscreen bitmap is
			// copied to the screen. Using pregnClipGob would cause lots
			// of hregn creations.
			pgpt->ClipToRegn(&pregn);
			pgob->Draw(&gnv, &rcClipGob);
			pgpt->ClipToRegn(&pregn);
			}
		if (grfgte & fgtePost)
			{
			dpt.xp += pgob->_rcCur.xpLeft;
			dpt.yp += pgob->_rcCur.ypTop;
			}
		}

LDone:
	ReleasePpo(&pregnClip);
	ReleasePpo(&pregnClipGob);
	return;

LFail:
	pregn->FEmpty(&rc);
	DrawTree(pgpt, prc, &rc, grfgob);
}


/***************************************************************************
	Draw the GOB into the given graphics environment.  On entry, the source
	rectangle of the GNV is set to (0, 0, dxp, dyp), where dxp and dyp are
	the width and height of the gob.  The gob is free to change the source
	rectangle, but should not touch the destination rectangle.
***************************************************************************/
void GOB::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
}


/***************************************************************************
	Make this gob fill up its parent's interior.
***************************************************************************/
void GOB::Maximize(void)
{
	AssertThis(0);
	_rcAbs.Zero();
	_rcRel.xpLeft = _rcRel.ypTop = krelZero;
	_rcRel.xpRight = _rcRel.ypBottom = krelOne;
	_SetRcCur();
}


/***************************************************************************
	Set the gob's position.  Invalidates both the old and new position.
***************************************************************************/
void GOB::SetPos(RC *prcAbs, RC *prcRel)
{
	AssertThis(0);
	AssertNilOrVarMem(prcAbs);
	AssertNilOrVarMem(prcRel);
	if (prcAbs == pvNil)
		_rcAbs.Zero();
	else
		_rcAbs = *prcAbs;

	if (prcRel == pvNil)
		_rcRel.Zero();
	else
		_rcRel = *prcRel;

	_SetRcCur();
}


/***************************************************************************
	Get the gob's position.
***************************************************************************/
void GOB::GetPos(RC *prcAbs, RC *prcRel)
{
	AssertThis(0);
	AssertNilOrVarMem(prcAbs);
	AssertNilOrVarMem(prcRel);
	if (pvNil != prcAbs)
		*prcAbs = _rcAbs;
	if (pvNil != prcRel)
		*prcRel = _rcRel;
}


/***************************************************************************
	Set the gob's rectangle from its hwnd.
***************************************************************************/
void GOB::SetRcFromHwnd(void)
{
	AssertThis(0);
	Assert(_hwnd != hNil, "no hwnd");
	_SetRcCur();
}


/***************************************************************************
	Get the bounding rectangle of the gob in the given coordinates.
***************************************************************************/
void GOB::GetRc(RC *prc, long coo)
{
	AssertThis(0);
	AssertVarMem(prc);
	PT dpt;

	*prc = _rcCur;
	_HwndGetDptFromCoo(&dpt, coo);
	prc->Offset(dpt.xp - _rcCur.xpLeft, dpt.yp - _rcCur.ypTop);
}


/***************************************************************************
	Get the visible rectangle of the gob in the given coordinates.
***************************************************************************/
void GOB::GetRcVis(RC *prc, long coo)
{
	AssertThis(0);
	AssertVarMem(prc);
	PT dpt;

	*prc = _rcVis;
	_HwndGetDptFromCoo(&dpt, coo);
	prc->Offset(dpt.xp - _rcCur.xpLeft, dpt.yp - _rcCur.ypTop);
}


/***************************************************************************
	Get the rectangle for the gob in cooHwnd coordinates and return the
	enclosing hwnd (if there is one).  This is a protected API.
***************************************************************************/
HWND GOB::_HwndGetRc(RC *prc)
{
	PT dpt;
	HWND hwnd;

	*prc = _rcCur;
	hwnd = _HwndGetDptFromCoo(&dpt, cooHwnd);
	prc->Offset(dpt.xp - _rcCur.xpLeft, dpt.yp - _rcCur.ypTop);
	return hwnd;
}


/***************************************************************************
	Return the hwnd that contains this GOB.
***************************************************************************/
HWND GOB::HwndContainer(void)
{
	AssertThis(0);
	PGOB pgob = this;

	while (pvNil != pgob)
		{
		if (hNil != pgob->_hwnd)
			return pgob->_hwnd;
		pgob = pgob->_pgobPar;
		}
	return hNil;
}


/***************************************************************************
	Map a point from cooSrc coordinates to cooDst coordinates (relative
	to the gob).
***************************************************************************/
void GOB::MapPt(PT *ppt, long cooSrc, long cooDst)
{
	AssertThis(0);
	AssertVarMem(ppt);
	PT dpt;

	_HwndGetDptFromCoo(&dpt, cooSrc);
	ppt->xp -= dpt.xp;
	ppt->yp -= dpt.yp;
	_HwndGetDptFromCoo(&dpt, cooDst);
	ppt->xp += dpt.xp;
	ppt->yp += dpt.yp;
}


/***************************************************************************
	Map an rc from cooSrc coordinates to cooDst coordinates (relative to
	the gob).
***************************************************************************/
void GOB::MapRc(RC *prc, long cooSrc, long cooDst)
{
	AssertThis(0);
	AssertVarMem(prc);
	PT dpt;

	_HwndGetDptFromCoo(&dpt, cooSrc);
	prc->Offset(-dpt.xp, -dpt.yp);
	_HwndGetDptFromCoo(&dpt, cooDst);
	prc->Offset(dpt.xp, dpt.yp);
}


/***************************************************************************
	Get the dxp and dyp to map from local coordinates to coo coordinates.
	If coo is cooHwnd or cooGlobal, also return the containing hwnd
	(otherwise return hNil).
***************************************************************************/
HWND GOB::_HwndGetDptFromCoo(PT *pdpt, long coo)
{
	PGOB pgob, pgobT;
	HWND hwnd = hNil;

	switch (coo)
		{
	default:
		Assert(coo == cooLocal, "bad coo");
		pdpt->xp = pdpt->yp = 0;
		break;

	case cooParent:
		pdpt->xp = _rcCur.xpLeft;
		pdpt->yp = _rcCur.ypTop;
		break;

	case cooGpt:
		pdpt->xp = pdpt->yp = 0;
		for (pgob = this;
				(pgobT = pgob->_pgobPar) != pvNil && pgobT->_pgpt == _pgpt;
				pgob = pgobT)
			{
			pdpt->xp += pgob->_rcCur.xpLeft;
			pdpt->yp += pgob->_rcCur.ypTop;
			}
		break;

	case cooHwnd:
	case cooGlobal:
		pdpt->xp = pdpt->yp = 0;
		for (pgob = this; pgob != pvNil && pgob->_hwnd == hNil;
				pgob = pgob->_pgobPar)
			{
			pdpt->xp += pgob->_rcCur.xpLeft;
			pdpt->yp += pgob->_rcCur.ypTop;
			}
		if (pvNil != pgob)
			hwnd = pgob->_hwnd;
		if (cooGlobal == coo && hNil != hwnd)
			{
			//Map from Hwnd to screen
			PTS pts;
			pts = PTS(*pdpt);
#ifdef WIN
			ClientToScreen(hwnd, &pts);
#endif //WIN
#ifdef MAC
			PPRT pprt;
			GetPort(&pprt);
			SetPort(&hwnd->port);
			LocalToGlobal(&pts);
			SetPort(pprt);
#endif //MAC
			*pdpt = PT(pts);
			}
		break;
		}

	return hwnd;
}


/***************************************************************************
	Get the minimum and maximum size for a gob.
***************************************************************************/
void GOB::GetMinMax(RC *prcMinMax)
{
	prcMinMax->xpLeft = prcMinMax->ypTop = 0;
	//yes kswMax for safety
	prcMinMax->xpRight = prcMinMax->ypBottom = kswMax;
}


/***************************************************************************
	Static method to find the gob containing the given point (in global
	coordinates).  If the mouse isn't over a GOB, this returns pvNil and
	sets *pptLocal to the passed in (xp, yp).
***************************************************************************/
PGOB GOB::PgobFromPtGlobal(long xp, long yp, PT *pptLocal)
{
	AssertNilOrVarMem(pptLocal);
	HWND hwnd;
	PTS pts;
	PGOB pgob;

#ifdef MAC
	PPRT pprt;

	pts.h = (short)xp;
	pts.v = (short)yp;
	if (inContent != FindWindow(pts, (WindowPtr *)&hwnd) ||
		hNil == hwnd ||
		pvNil == (pgob = PgobFromHwnd(hwnd)))
		{
		if (pvNil != pptLocal)
			{
			pptLocal->xp = xp;
			pptLocal->yp = yp;
			}
		return pvNil;
		}
	GetPort(&pprt);
	SetPort(&hwnd->port);
	GlobalToLocal(&pts);
	SetPort(pprt);
	return pgob->PgobFromPt(pts.h, pts.v, pptLocal);
#endif //MAC
#ifdef WIN
	pts.x = xp;
	pts.y = yp;
	if (hNil == (hwnd = WindowFromPoint(pts)) ||
		pvNil == (pgob = PgobFromHwnd(hwnd)))
		{
		if (pvNil != pptLocal)
			{
			pptLocal->xp = xp;
			pptLocal->yp = yp;
			}
		return pvNil;
		}
	ScreenToClient(hwnd, &pts);
	return pgob->PgobFromPt(pts.x, pts.y, pptLocal);
#endif //WIN
}


/***************************************************************************
	Determine which gob in the tree starting with this GOB the given point
	is in.  This may return pvNil if no gob claims to contain the given
	point.  xp, yp is assumed to be in this gob's parent's coordinates.
	This is recursive, so a GOB can build it's own world and hit testing
	method.
***************************************************************************/
PGOB GOB::PgobFromPt(long xp, long yp, PT *pptLocal)
{
	AssertThis(0);

	xp -= _rcCur.xpLeft;
	yp -= _rcCur.ypTop;

	if (FPtInBounds(xp, yp))
		{
		// the point is in our bounding rectangle, so give the children
		// a whack at it
		PGOB pgob, pgobT;

		for (pgob = _pgobChd; pvNil != pgob; pgob = pgob->_pgobSib)
			{
			if (pvNil != (pgobT = pgob->PgobFromPt(xp, yp, pptLocal)))
				return pgobT;
			}
		}

	// call FPtIn whether or not FInBounds returned true so a parent can will some
	// extra space to a child
	if (FPtIn(xp, yp))
		{
		if (pptLocal != pvNil)
			{
			pptLocal->xp = xp;
			pptLocal->yp = yp;
			}
		return this;
		}

	return pvNil;
}


/***************************************************************************
	Determine whether the given point (in this gob's local coordinates)
	is in this gob. This will be subclassed by all non-rectangular gobs
	(including ones that don't want to respond to the mouse at all).
	We handle tool tips here to avoid bugs of omission and for convenience.
***************************************************************************/
bool GOB::FPtIn(long xp, long yp)
{
	AssertThis(0);
	RC rc;

	// tool tips and their children are "invisible".
	if (khidToolTip == Hid())
		return fFalse;

	GetRc(&rc, cooLocal);
	return rc.FPtIn(xp, yp);
}


/***************************************************************************
	Determine whether the given point (in this gob's local coordinates)
	is in this gob's bounding rectangle.  This indicates whether it's OK to
	ask the GOB's children whether the point is in them.  This will be
	subclassed by all GOBs that don't want to respond to the mouse.  We
	handle tool tips here to avoid bugs of omission and for convenience.
	If this returns false, PgobFromPt will still call FPtIn.
***************************************************************************/
bool GOB::FPtInBounds(long xp, long yp)
{
	AssertThis(0);
	RC rc;

	// tool tips and their children are "invisible".
	if (khidToolTip == Hid())
		return fFalse;

	GetRc(&rc, cooLocal);
	return rc.FPtIn(xp, yp);
}


/***************************************************************************
	Default mouse down handler just enqueues a cidActivateSel, cidSelIdle and
	a cidTrackMouse command.
***************************************************************************/
void GOB::MouseDown(long xp, long yp, long cact, ulong grfcust)
{
	AssertThis(0);
	Assert(grfcust & fcustMouse, "grfcust wrong");
	CMD_MOUSE cmd;

	vpcex->EnqueueCid(cidActivateSel, this);
	vpcex->EnqueueCid(cidSelIdle, pvNil, pvNil, fTrue, Hid());
	cmd.cid = cidMouseDown;
	cmd.pcmh = this;
	cmd.pgg = pvNil;
	cmd.xp = xp;
	cmd.yp = yp;
	cmd.grfcust = grfcust;
	cmd.cact = cact;
	vpcex->EnqueueCmd((PCMD)&cmd);
}


/***************************************************************************
	Set the _rcCur values based on _rcAbs and _rcRel.  If there is an OS
	window associated with this GOB, set _rcCur based on the hwnd.
	Invalidates the old and new rectangles.
***************************************************************************/
void GOB::_SetRcCur(void)
{
	PGOB pgob;
	GTE gte;
	ulong grfgte;
	RC rc, rcVis;

	//invalidate the original rc
	InvalRc(pvNil);

	gte.Init(this, fgteNil);
	while (gte.FNextGob(&pgob, &grfgte, fgteNil))
		{
		if (!(grfgte & fgtePre))
			continue;

		//get the new rc and the rcVis of the parent (in the parent's local
		//coordinates)

		if (pgob->_hwnd != hNil)
			{
			RCS rcs;

			GetClientRect(pgob->_hwnd, &rcs);
			rc = rcs;
			rcVis.Max();
			}
		else if (pgob->_pgobPar != pvNil)
			{
			long dxp;
			long dyp;

			dxp = pgob->_pgobPar->_rcCur.Dxp();
			dyp = pgob->_pgobPar->_rcCur.Dyp();
			rc.xpLeft = pgob->_rcAbs.xpLeft +
				LwMulDiv(dxp, pgob->_rcRel.xpLeft, krelOne);
			rc.ypTop = pgob->_rcAbs.ypTop +
				LwMulDiv(dyp, pgob->_rcRel.ypTop, krelOne);
			rc.xpRight = pgob->_rcAbs.xpRight +
				LwMulDiv(dxp, pgob->_rcRel.xpRight, krelOne);
			rc.ypBottom = pgob->_rcAbs.ypBottom +
				LwMulDiv(dyp, pgob->_rcRel.ypBottom, krelOne);
			pgob->_pgobPar->GetRcVis(&rcVis, cooLocal);
			}
		else
			{
			rc = pgob->_rcAbs;
			rcVis.Max();
			}

		//intersect the parents visible portion with the new rc to get
		//this gob's visible portion
		rcVis.FIntersect(&rc);

		pgob->_rcCur = rc;
		pgob->_rcVis = rcVis;

		if (grfgte & fgteRoot)
			{
			//invalidate the new rectangle - we do it here so children
			//can draw and validate themselves if they want
			InvalRc(pvNil);
			}

		//tell the gob that it has a new rectangle
		pgob->_NewRc();
		}
}


/***************************************************************************
	Return the previous sibling for the gob.
***************************************************************************/
PGOB GOB::PgobPrevSib(void)
{
	PGOB pgob;

	pgob = _pgobPar == pvNil ? _pgobScreen : _pgobPar->_pgobChd;
	if (pgob == this)
		return pvNil;

	for ( ; pgob != pvNil && pgob->_pgobSib != this; pgob = pgob->_pgobSib)
		;

	if (pgob == pvNil)
		{
		Bug("corrupt gob tree");
		return pvNil;
		}
	Assert(pgob->_pgobSib == this, "wrong logic");
	return pgob;
}


/***************************************************************************
	Return the last child of the gob.
***************************************************************************/
PGOB GOB::PgobLastChild(void)
{
	PGOB pgob;

	if ((pgob = _pgobChd) == pvNil)
		return pvNil;

	for ( ; pgob->_pgobSib != pvNil; pgob = pgob->_pgobSib)
		;

	Assert(pgob->_pgobSib == pvNil, "wrong logic");
	return pgob;
}


/***************************************************************************
	Create a new MDI window and attach it to the gob.
***************************************************************************/
bool GOB::FCreateAndAttachMdi(PSTN pstnTitle)
{
	AssertThis(0);
	AssertPo(pstnTitle, 0);
	HWND hwnd;

	if ((hwnd = _HwndNewMdi(pstnTitle)) == hNil)
		return fFalse;
	if (!FAttachHwnd(hwnd))
		{
		_DestroyHwnd(hwnd);
		return fFalse;
		}
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Static method: find the currently active MDI gob.
***************************************************************************/
PGOB GOB::PgobMdiActive(void)
{
	HWND hwnd;

	if (hNil == (hwnd = HwndMdiActive()))
		return pvNil;
	return PgobFromHwnd(hwnd);
}


/***************************************************************************
	Static method: find the first gob of the given class in the screen's gob
	tree.
***************************************************************************/
PGOB GOB::PgobFromClsScr(long cls)
{
	if (pvNil == _pgobScreen)
		return pvNil;
	return _pgobScreen->PgobFromCls(cls);
}


/***************************************************************************
	Find a gob in this gob's subtree that is of the given class.
***************************************************************************/
PGOB GOB::PgobFromCls(long cls)
{
	AssertThis(0);
	GTE gte;
	ulong grfgte;
	PGOB pgob;

	gte.Init(this, fgteNil);
	while (gte.FNextGob(&pgob, &grfgte, fgteNil))
		{
		if (pgob->FIs(cls))
			return pgob;
		}
	return pvNil;
}


/***************************************************************************
	Find a direct child of this gob of the given class.
***************************************************************************/
PGOB GOB::PgobChildFromCls(long cls)
{
	AssertThis(0);
	PGOB pgob;

	for (pgob = _pgobChd; pvNil != pgob; pgob = pgob->_pgobSib)
		{
		if (pgob->FIs(cls))
			return pgob;
		}
	return pvNil;
}


/***************************************************************************
	Find a gob of the given class in the parent chain of this gob.
***************************************************************************/
PGOB GOB::PgobParFromCls(long cls)
{
	AssertThis(0);
	PGOB pgob;

	for (pgob = _pgobPar; pvNil != pgob; pgob = pgob->_pgobPar)
		{
		if (pgob->FIs(cls))
			return pgob;
		}
	return pvNil;
}


/***************************************************************************
	Static method: find the first gob with the given hid in the screen's gob
	tree.
***************************************************************************/
PGOB GOB::PgobFromHidScr(long hid)
{
	Assert(hid != hidNil, "nil hid");
	if (pvNil == _pgobScreen)
		return pvNil;

	return _pgobScreen->PgobFromHid(hid);
}


/***************************************************************************
	Find a gob in this gobs subtree having the given hid.
***************************************************************************/
PGOB GOB::PgobFromHid(long hid)
{
	AssertThis(0);
	GTE gte;
	ulong grfgte;
	PGOB pgob;

	gte.Init(this, fgteNil);
	while (gte.FNextGob(&pgob, &grfgte, fgteNil))
		{
		if (pgob->Hid() == hid)
			return pgob;
		}
	return pvNil;
}


/***************************************************************************
	Find a direct child of this gob having the given hid.
***************************************************************************/
PGOB GOB::PgobChildFromHid(long hid)
{
	AssertThis(0);
	PGOB pgob;

	for (pgob = _pgobChd; pvNil != pgob; pgob = pgob->_pgobSib)
		{
		if (pgob->Hid() == hid)
			return pgob;
		}
	return pvNil;
}


/***************************************************************************
	Find a gob with the given hid in the parent chain of this gob.
***************************************************************************/
PGOB GOB::PgobParFromHid(long hid)
{
	AssertThis(0);
	PGOB pgob;

	for (pgob = _pgobPar; pvNil != pgob; pgob = pgob->_pgobPar)
		{
		if (pgob->Hid() == hid)
			return pgob;
		}
	return pvNil;
}


/***************************************************************************
	Find a gob in this gobs subtree having the given gob run-time id.
***************************************************************************/
PGOB GOB::PgobFromGrid(long grid)
{
	AssertThis(0);
	GTE gte;
	ulong grfgte;
	PGOB pgob;

	gte.Init(this, fgteNil);
	while (gte.FNextGob(&pgob, &grfgte, fgteNil))
		{
		if (pgob->Grid() == grid)
			return pgob;
		}
	return pvNil;
}


/***************************************************************************
	Handles a close command.
***************************************************************************/
bool GOB::FCmdCloseWnd(PCMD pcmd)
{
	AssertThis(0);
	Release();
	return fTrue;
}


/***************************************************************************
	Handles a mouse track command.
***************************************************************************/
bool GOB::FCmdTrackMouse(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Command function to handle a key stroke.
***************************************************************************/
bool GOB::FCmdKey(PCMD_KEY pcmd)
{
	return fFalse;
}


/***************************************************************************
	Command function to handle a bad key command (sent by a child to
	its parent).
***************************************************************************/
bool GOB::FCmdBadKey(PCMD_BADKEY pcmd)
{
	return fFalse;
}


/***************************************************************************
	Do selection idle processing.  Make sure the selection is on or off
	according to rglw[0] (non-zero means on) and set rglw[0] to false.
	Always return false.
***************************************************************************/
bool GOB::FCmdSelIdle(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	return fFalse;
}


/***************************************************************************
	Activate the selection.  Default does nothing.
***************************************************************************/
bool GOB::FCmdActivateSel(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	return fFalse;
}


/***************************************************************************
	The mouse moved in this GOB, set the cursor.
***************************************************************************/
bool GOB::FCmdMouseMove(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	vpappb->SetCurs(_pcurs);
	return fTrue;
}


/***************************************************************************
	Drag the rectangle, restricting to [zpMin, zpLim).  While zp is in
	[zpMinActive, zpLimActive), the bar is filled with solid invert, otherwise
	with patterned (50%) invert.
***************************************************************************/
long GOB::ZpDragRc(RC *prc, bool fVert, long zp, long zpMin, long zpLim,
	long zpMinActive, long zpLimActive)
{
	RC rcBound, rcActive;
	PT pt, dpt;
	bool fActive, fActiveNew, fDown;
	GNV gnv(this);

	if (fVert)
		{
		pt.xp = 0;
		pt.yp = zp;
		rcBound.Set(0, zpMin, 1, zpLim);
		rcActive.Set(0, zpMinActive, 1, zpLimActive);
		}
	else
		{
		pt.xp = zp;
		pt.yp = 0;
		rcBound.Set(zpMin, 0, zpLim, 1);
		rcActive.Set(zpMinActive, 0, zpLimActive, 1);
		}

	//draw the initial bar
	fActive = rcActive.FPtIn(pt.xp, pt.yp);
	if (fActive)
		gnv.FillRc(prc, kacrInvert);
	else
		gnv.FillRcApt(prc, &vaptGray, kacrInvert, kacrClear);

	for (;;)
		{
		GetPtMouse(&dpt, &fDown);
		if (!fDown)
			break;

		//pin the pt to rcBound
		rcBound.PinPt(&dpt);
		Assert(dpt.xp == 0 || dpt.yp == 0, "bad pinned point");
		if (pt == dpt)
			continue;

		//move the bar
		fActiveNew = rcActive.FPtIn(dpt.xp, dpt.yp);
		dpt -= pt;
		if (FPure(fActive) == FPure(fActiveNew))
			{
			//invert the two pieces of the difference between
			//the new and old rectangles
			RC rc1, rc2;
			long dzp;

			rc1 = *prc;
			if (fVert)
				rc1.Transform(fptTranspose);
			rc2 = rc1;
			Assert(dpt.xp == 0 || dpt.yp == 0, "bad pinned point");
			dzp = dpt.xp + dpt.yp;
			rc1.Offset(dzp, 0);
			if (dzp < 0)
				SortLw(&rc1.xpRight, &rc2.xpLeft);
			else
				SortLw(&rc2.xpRight, &rc1.xpLeft);
			if (fVert)
				{
				rc1.Transform(fptTranspose);
				rc2.Transform(fptTranspose);
				}
			if (fActive)
				{
				gnv.FillRc(&rc1, kacrInvert);
				gnv.FillRc(&rc2, kacrInvert);
				}
			else
				{
				gnv.FillRcApt(&rc1, &vaptGray, kacrInvert, kacrClear);
				gnv.FillRcApt(&rc2, &vaptGray, kacrInvert, kacrClear);
				}
			*prc += dpt;
			}
		else if (fActive)
			{
			//just draw the two
			gnv.FillRc(prc, kacrInvert);
			*prc += dpt;
			gnv.FillRcApt(prc, &vaptGray, kacrInvert, kacrClear);
			}
		else
			{
			//just draw the two
			gnv.FillRcApt(prc, &vaptGray, kacrInvert, kacrClear);
			*prc += dpt;
			gnv.FillRc(prc, kacrInvert);
			}
		fActive = fActiveNew;
		pt += dpt;
		}

	//erase the current bar
	if (fActive)
		gnv.FillRc(prc, kacrInvert);
	else
		gnv.FillRcApt(prc, &vaptGray, kacrInvert, kacrClear);
	return fVert ? pt.yp : pt.xp;
}


/***************************************************************************
	Set the cursor for this GOB to pcurs.
***************************************************************************/
void GOB::SetCurs(PCURS pcurs)
{
	AssertThis(0);
	AssertNilOrPo(pcurs, 0);

	SwapVars(&pcurs, &_pcurs);
	if (pvNil != _pcurs)
		_pcurs->AddRef();
	ReleasePpo(&pcurs);
}


/***************************************************************************
	Set the cursor for this GOB as indicated.
***************************************************************************/
void GOB::SetCursCno(PRCA prca, CNO cno)
{
	AssertPo(prca, 0);
	PCURS pcurs;

	if (pvNil == (pcurs = (PCURS)prca->PbacoFetch(kctgCursor,
			cno, CURS::FReadCurs)))
		{
		Warn("cursor not found");
		return;
		}
	SetCurs(pcurs);
	ReleasePpo(&pcurs);
}


/***************************************************************************
	Return the address of the variable list belonging to this gob.  When the
	gob is freed, the pointer is no longer valid.
***************************************************************************/
PGL *GOB::Ppglrtvm(void)
{
	AssertThis(0);
	return &_pglrtvm;
}


/***************************************************************************
	Put up a tool tip if this GOB has one.
***************************************************************************/
bool GOB::FEnsureToolTip(PGOB *ppgobCurTip, long xpMouse, long ypMouse)
{
	AssertThis(0);
	AssertVarMem(ppgobCurTip);
	AssertNilOrPo(*ppgobCurTip, 0);

	return fFalse;
}


/***************************************************************************
	Return the state of the GOB. Must be non-zero.
***************************************************************************/
long GOB::LwState(void)
{
	AssertThis(0);
	return 1;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the GOB.
***************************************************************************/
void GOB::AssertValid(ulong grf)
{
	GOB_PAR::AssertValid(0);
	if (hNil != _hwnd)
		{
		Assert(0 == _rcCur.xpLeft && 0 == _rcCur.ypTop,
			"_hwnd based gob not at (0, 0)");
		}
	if (pvNil != _pgpt)
		{
		AssertPo(_pgpt, 0);
		}
}


/***************************************************************************
	Mark memory referenced by the gob.
***************************************************************************/
void GOB::MarkMem(void)
{
	AssertValid(0);
	GOB_PAR::MarkMem();
	MarkMemObj(_pgpt);
	MarkMemObj(_pglrtvm);
}


/***************************************************************************
	Mark memory for this gob and all descendent gobs.
***************************************************************************/
void GOB::MarkGobTree(void)
{
	GTE gte;
	PGOB pgob;
	ulong grfgte;

	gte.Init(this, fgteNil);
	while (gte.FNextGob(&pgob, &grfgte, fgteNil))
		{
		if (grfgte & fgtePre)
			pgob->MarkMem();
		}
}
#endif //DEBUG


/***************************************************************************
	Constructor for a GOB tree enumerator.
***************************************************************************/
GTE::GTE(void)
{
	_es = esDone;
}


/***************************************************************************
	Initialize a GOB tree enumerator.
***************************************************************************/
void GTE::Init(PGOB pgob, ulong grfgte)
{
	_pgobRoot = pgob;
	_pgobCur = pvNil;
	_fBackWards = FPure(grfgte & fgteBackToFront);
	_es = pgob == pvNil ? esDone : esStart;
}


/***************************************************************************
	Goes to the next node in the sub tree being enumerated.  Returns false
	iff the enumeration is done.
***************************************************************************/
bool GTE::FNextGob(PGOB *ppgob, ulong *pgrfgteOut, ulong grfgte)
{
	PGOB pgobT;

	*pgrfgteOut = fgteNil;
	switch (_es)
		{
	case esStart:
		_pgobCur = _pgobRoot;
		*pgrfgteOut |= fgteRoot;
		goto LCheckForKids;

	case esGoDown:
		if (!(grfgte & fgteSkipToSib))
			{
			pgobT = _fBackWards ?
				_pgobCur->PgobLastChild() : _pgobCur->_pgobChd;
			if (pgobT != pvNil)
				{
				_pgobCur = pgobT;
				goto LCheckForKids;
				}
			}
		//fall through
	case esGoRight:
		//go to the sibling (if there is one) or parent
		if (_pgobCur == _pgobRoot)
			{
			_es = esDone;
			return fFalse;
			}
		pgobT = _fBackWards ?
			_pgobCur->PgobPrevSib() : _pgobCur->_pgobSib;
		if (pgobT != pvNil)
			{
			_pgobCur = pgobT;
LCheckForKids:
			*pgrfgteOut |= fgtePre;
			if (_pgobCur->_pgobChd == pvNil)
				{
				*pgrfgteOut |= fgtePost;
				_es = esGoRight;
				}
			else
				_es = esGoDown;
			}
		else
			{
			//no more siblings, go to parent
			_pgobCur = _pgobCur->_pgobPar;
			*pgrfgteOut |= fgtePost;
			if (_pgobCur == _pgobRoot)
				{
				_es = esDone;
				*pgrfgteOut |= fgteRoot;
				}
			else
				_es = esGoRight;
			}
		break;

	case esDone:
		return fFalse;
		}

	*ppgob = _pgobCur;
	return fTrue;
}
