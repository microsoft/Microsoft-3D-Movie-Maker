/******************************************************************************
	Author: ******
	Project: Socrates
	Review Status: Reviewed

	Main module for the scene sorter class.

************************************************************ PETED ***********/

#include "studio.h"

ASSERTNAME

BEGIN_CMD_MAP(SCRT, GOK)
	ON_CID_ME(cidSceneSortInit, FCmdInit, pvNil)
	ON_CID_ME(cidSceneSortSelect, FCmdSelect, pvNil)
	ON_CID_ME(cidSceneSortInsert, FCmdInsert, pvNil)
	ON_CID_ME(cidSceneSortScroll, FCmdScroll, pvNil)
	ON_CID_ME(cidSceneSortNuke, FCmdNuke, pvNil)
	ON_CID_ME(cidSceneSortOk, FCmdDismiss, pvNil)
	ON_CID_ME(cidSceneSortCancel, FCmdDismiss, pvNil)
	ON_CID_ME(cidSceneSortPortfolio, FCmdPortfolio, pvNil)
	ON_CID_ME(cidSceneSortTransition, FCmdTransition, pvNil)
END_CMD_MAP_NIL()


RTCLASS(SCRT)

#ifdef DEBUG
void SCRT::AssertValid(ulong grf)
{
	SCRT_PAR::AssertValid(0);
	if (_iscenMac > 0)
		AssertIn(_iscenCur, 0, _iscenMac);
	else
		Assert(_iscenCur == 0, "Non-zero _iscenCur for empty movie");
	AssertIn(_iscenTop, 0, _iscenMac + 1);

	/* The CMVI has loose rules about the format of its data structures, but
		it's important to the SCRT that we keep things in the right order. */
	if (_cmvi.pglscend != pvNil)
		{
		long imviedMac;

		AssertPo(_cmvi.pglscend, 0);
		AssertPo(_cmvi.pglmvied, 0);
		imviedMac = _cmvi.pglmvied->IvMac();
		for (long iscend = 0; iscend < _cmvi.pglscend->IvMac(); iscend++)
			{
			SCEND scend;
	
			_cmvi.pglscend->Get(iscend, &scend);
			Assert(iscend < _iscenMac ?
				!scend.fNuked : scend.fNuked, "Bad GL of SCENDs");
			Assert(scend.imvied < imviedMac, "Bogus scene entry in pglscend");
			}
		}
}

void SCRT::MarkMem(void)
{
	AssertThis(0);

	SCRT_PAR::MarkMem();
	MarkMemObj(_pmvie);
	_cmvi.MarkMem();
}
#endif /* DEBUG */

SCRT::SCRT(PGCB pgcb) : SCRT_PAR(pgcb)
{
	Assert(_pmvie == pvNil, "SCRT block not cleared");
	Assert(_pstdio == pvNil, "SCRT block not cleared");
	Assert(_cmvi.pglscend == pvNil, "SCRT block not cleared");
	Assert(_cmvi.pglmvied == pvNil, "SCRT block not cleared");
	Assert(_fError == fFalse, "SCRT block not cleared");
	Assert(_fInited == fFalse, "SCRT block not cleared");
	Assert(_iscenMac == 0, "SCRT block not cleared");
}

SCRT::~SCRT(void)
{
	PGOB pgob;

	/* We might get released on Quit without exiting the Scene Sorter. */
	if (_cmvi.pglscend != pvNil || _cmvi.pglmvied != pvNil)
		_cmvi.Empty();
	if (_pmvie != pvNil)
		ReleasePpo(&_pmvie);

	/* Kill the glass GOB (it's not a child of me) */
	pgob = vpapp->Pkwa()->PgobFromHid(kidGenericDisableGlass);
	ReleasePpo(&pgob);

	if (_fInited)
		vpapp->EnableAccel();

	/* Report generic error */
	if (_fError)
		PushErc(ercSocSceneSortError);
}


/******************************************************************************
	PscrtNew
		Allocates and initializes a brand new SCRT.  If any necessary
		initialization fails, cleans up and returns a nil pointer.
	
	Arguments:
		long hid -- the command hander ID for the SCRT
	
	Returns: the pointer to the new SCRT, pvNil if the routine fails
	
************************************************************ PETED ***********/
PSCRT SCRT::PscrtNew(long hid, PMVIE pmvie, PSTDIO pstdio, PRCA prca)
{
	AssertPo(pmvie, 0);
	AssertPo(pstdio, 0);

	PSCRT pscrt = pvNil;
	PGOB pgobPar;
	RC rcRel;
	GCB gcb;

	if ((pgobPar = vapp.Pkwa()->PgobFromHid(kidBackground)) == pvNil)
		{
		Bug("Couldn't find background GOB");
		goto LFail;
		}

	rcRel.Set(krelZero, krelZero, krelOne, krelOne);
	gcb.Set(hid, pgobPar, fgobNil, kginDefault, pvNil, &rcRel);

	if ((pscrt = NewObj SCRT(&gcb)) == pvNil)
		goto LFail;

	if (!pscrt->_FInit(vpapp->Pkwa(), hid, prca))
		goto LOom;
	if (!pscrt->_FEnterState(ksnoInit))
		{
		Warn("GOK immediately destroyed!");
		pscrt = pvNil;
		goto LFail;
		}

	if (!pmvie->FAddToCmvi(&pscrt->_cmvi, &pscrt->_iscenMac))
		{
LOom:
		ReleasePpo(&pscrt);
		goto LFail;
		}

	pscrt->_pmvie = pmvie;
	pscrt->_pmvie->AddRef();
	pscrt->_iscenCur = LwMax(0, pscrt->_pmvie->Iscen());
	pscrt->_iscenMac = pscrt->_pmvie->Cscen();
	pscrt->_pstdio = pstdio;

LFail:
	return pscrt;
}


/******************************************************************************
	_ErrorExit
		Handle an error.  Destroys the easel and enqueues our cancel cid.
	
************************************************************ PETED ***********/
void SCRT::_ErrorExit(void)
{
	/* If someone's already reported an error, don't do any more work */
	if (_fError)
		return;
	_fError = fTrue;

	/* Notify myself that we're exiting */
	vpcex->EnqueueCid(cidSceneSortCancel, this);
}


/******************************************************************************
	FCmdInit
		Initializes the Scene Sorter with the information about the easel
		in kidspace.
	
	Arguments:
		PCMD pcmd -- pointer to the CMD data.  Extra params are as follows:
			rglw[0] -- kid of the first thumbnail frame GOK
			rglw[1] -- kid of the first scrollbar GOK button (scroll up)
			rglw[2] -- number of GOKs in a single frame
	
	Returns: fTrue if the command was handled by this routine.
	
************************************************************ PETED ***********/
bool SCRT::FCmdInit(PCMD pcmd)
{
	AssertThis(0);

	bool fSuccess = fFalse;
	long kidCur, kidThumb;
	PGOK pgokFrame;

	/* If I'm already inited, this must be for some other scene sorter */
	if (_fInited)
		return fFalse;

	_fInited = fTrue;
	vpapp->DisableAccel();

	_kidFrameMin = pcmd->rglw[0];
	_kidScbtnsMin = pcmd->rglw[1];
	_cgokFrame = pcmd->rglw[2];
	_cfrmPage = 0;

	kidThumb = _kidFrameMin - 1;
	kidCur = _kidFrameMin;

	while ((pgokFrame = (PGOK)vpapp->Pkwa()->PgobFromHid(kidCur)) != pvNil)
		{
		PGOMP pgomp;
		PGOB pgobThumb;

		Assert(pgokFrame->FIs(kclsGOK), "Frame GOB isn't a GOK");

		pgobThumb = (PGOK)pgokFrame->PgobFirstChild();
		Assert(pgobThumb != pvNil, "Frame has no children");

		pgomp = GOMP::PgompNew(pgobThumb, kidThumb--);
		if (pgomp == pvNil)
			goto LFail;
		_cfrmPage++;
		kidCur += _cgokFrame;
		}

	_cfrmPage--;

	_iscenTop = (_iscenCur / _cfrmPage) * _cfrmPage;
	fSuccess = _FResetThumbnails(fFalse);

LFail:
	if (!fSuccess)
		_ErrorExit();

	return fTrue;
}


/******************************************************************************
	FCmdSelect
		Selects the scene corresponding to the given thumbnail frame.
	
	Arguments:
		PCMD pcmd -- pointer to the CMD data.  Extra params are as follows:
			rglw[0] -- the kid of the thumbnail frame
	
	Returns: fTrue if the command was handled by this routine.
	
************************************************************ PETED ***********/
bool SCRT::FCmdSelect(PCMD pcmd)
{
	AssertThis(0);
	Assert(pcmd->rglw[0] - _kidFrameMin < _cfrmPage * _cgokFrame,
		"Bogus kid for select");
	Assert(_iscenMac > 0, "Can't select scene in an empty movie");

	long iscen;

	iscen = _IscenFromKid(pcmd->rglw[0]);
	if (iscen < _iscenMac)
		{
		_iscenCur = iscen;

		/* Fill in thumbnail for selection GOB */
		if (_FResetThumbnails(fFalse))
			_SetSelectionVis(fTrue);
		else
			_ErrorExit();
		}

	return fTrue;
}


/******************************************************************************
	FCmdInsert
		Inserts the currently selected scene before the scene that
		corresponds to the given thumbnail frame.  If the given thumbnail
		frame is larger than the last visible thumbnail frame, the scene is
		inserted after the last scene visible on the easel.
	
	Arguments:
		PCMD pcmd -- pointer to the CMD data.  Extra params are as follows:
			rglw[0] -- the kid of the frame to insert the scene before
	
	Returns: fTrue if the command was handled by this routine.
	
************************************************************ PETED ***********/
bool SCRT::FCmdInsert(PCMD pcmd)
{
	AssertThis(0);
	Assert(pcmd->rglw[0] - _kidFrameMin <= _cfrmPage * _cgokFrame,
		"Bogus kid for insert");

	long iscenTo = _IscenFromKid(pcmd->rglw[0]);

	if (iscenTo != _iscenCur)
		{
		AssertIn(iscenTo, 0, _iscenMac+1);
	
		_cmvi.pglscend->Move(_iscenCur, iscenTo);
		if (iscenTo > _iscenCur)
			iscenTo--;
		_iscenCur = iscenTo;
		}

	/* Refill the thumbnails */
	if (!_FResetThumbnails(fFalse))
		_ErrorExit();

	return fTrue;
}


/******************************************************************************
	FCmdScroll
		Scrolls the thumbnails by the given number of frames.  The sign of
		the number of frames indicates whether to scroll forward (positive)
		or backward (negative).  Sets the selected frame to kstBrowserSelected
		if we're not drag & drop, or to kstBrowserScrollingSel if we are.
	
	Arguments:
		PCMD pcmd -- pointer to the CMD data.  Extra params are as follows:
			rglw[0] -- the number of frames to scroll
			rglw[1] -- non-zero if we're scrolling during drag & drop
	
	Returns: fTrue if the command was handled by this routine.
	
************************************************************ PETED ***********/
bool SCRT::FCmdScroll(PCMD pcmd)
{
	AssertThis(0);

	bool fHideSel = pcmd->rglw[1] != 0;
	long iscenT;

	iscenT = _iscenTop + pcmd->rglw[0];
	if (FIn(iscenT, 0, _iscenMac) && _iscenTop != iscenT)
		{
		_SetSelectionVis(fFalse, fHideSel);
		_iscenTop = iscenT;
		}

	/* Refill the thumbnails */
	if (!_FResetThumbnails(fHideSel))
		_ErrorExit();

	_SetSelectionVis(fTrue, fHideSel);

	return fTrue;
}


/******************************************************************************
	_EnableScroll
		Enables or disables the scrolling buttons, as appropriate.
	
************************************************************ PETED ***********/
void SCRT::_EnableScroll(void)
{
	PGOK pgok;

	/* Enable or disable scroll up */
	pgok = (PGOK)vapp.Pkwa()->PgobFromHid(_kidScbtnsMin);
	if (pgok != pvNil)
		{
		long lwState;

		lwState = (_iscenTop > 0) ? kstBrowserEnabled : kstBrowserDisabled;
		Assert(pgok->FIs(kclsGOK), "Scroll up button is not a GOK");
		if (pgok->Sno() != lwState)
			pgok->FChangeState(lwState);
		}
	else
		Bug("Can't find scroll up button");

	/* Enable or disable scroll down */
	pgok = (PGOK)vapp.Pkwa()->PgobFromHid(_kidScbtnsMin + 1);
	if (pgok != pvNil)
		{
		long lwState;

		lwState = (_iscenTop + _cfrmPage < _iscenMac) ?
			kstBrowserEnabled : kstBrowserDisabled;
		Assert(pgok->FIs(kclsGOK), "Scroll down button is not a GOK");
		if (pgok->Sno() != lwState)
			pgok->FChangeState(lwState);
		}
	else
		Bug("Can't find scroll down button");
}


/******************************************************************************
	FCmdNuke
		Deletes the currently selected scene from the movie.  Makes the
		following scene the currently selected scene, unless there is no
		such scene, in which case the scene immediately before the deleted
		scene is the new currently selected scene.
	
	Arguments:
		PCMD pcmd -- pointer to the CMD data.  No additional parameters.
	
	Returns: fTrue if the command was handled by this routine.
	
************************************************************ PETED ***********/
bool SCRT::FCmdNuke(PCMD pcmd)
{
	AssertThis(0);
	Assert(_iscenMac > 0, "Can't nuke a scene from an empty movie");

	SCEND scend;

	_cmvi.pglscend->Get(_iscenCur, &scend);
	Assert(!scend.fNuked, "Nuking an already nuked scene");
	scend.fNuked = fTrue;
	_cmvi.pglscend->Put(_iscenCur, &scend);
	_cmvi.pglscend->Move(_iscenCur, _cmvi.pglscend->IvMac());
	_iscenMac--;
	if (_iscenMac > 0 && _iscenMac == _iscenCur)
		_iscenCur--;

	/* Refill the thumbnails */
	if (!_FResetThumbnails(fFalse))
		_ErrorExit();
	
	return fTrue;
}


/******************************************************************************
	FCmdDismiss
		Alerts the Scene Sorter that the easel is about to go away.  If the
		easel was not cancelled, the changes made in the easel are applied
		to the movie.
	
	Arguments:
		PCMD pcmd -- pointer to the CMD data.  Checks cid to determine ok/cancel
	
	Returns: fTrue if the command was handled by this routine.
	
************************************************************ PETED ***********/
bool SCRT::FCmdDismiss(PCMD pcmd)
{
	AssertThis(0);

	PMVU pmvu;

	if (pcmd->cid == cidSceneSortOk)
		{
		vapp.BeginLongOp();
		if (_pmvie->FSetCmvi(&_cmvi))
			{
			if (_pmvie->Cscen() > 0 && _pmvie->Iscen() != _iscenCur)
				_pmvie->FSwitchScen(_iscenCur);
			}
		else
			_fError = fTrue;
		_pmvie->ClearUndo();
		vapp.EndLongOp();
		}

	/* Change to the tool for scenes */
 	pmvu = (PMVU)(_pmvie->PddgActive());
 	AssertPo(pmvu, 0);
 	pmvu->SetTool(toolDefault);
	_pstdio->ChangeTool(toolDefault);

	/* Clean up the internal data structures */
	_cmvi.Empty();
	ReleasePpo(&_pmvie);

	Release();

	return fTrue;
}


/******************************************************************************
	FCmdPortfolio
		Brings up the portfolio so that the user can append movies from file.
	
	Arguments:
		PCMD pcmd -- pointer to the CMD data.  No additional parameters.
	
	Returns: fTrue if the command was handled by this routine.
	
************************************************************ PETED ***********/
bool SCRT::FCmdPortfolio(PCMD pcmd)
{
	AssertThis(0);

	FNI fni;
	MCC mcc(2, 2, 0);
	PMVIE pmvie = pvNil;

	if (!_pstdio->FGetFniMovieOpen(&fni))
		goto LFail;

	/* Specific reasons for failures are reported by lower-level routines.
		There should be no reason to display an error here. */
	pmvie = MVIE::PmvieNew(vpapp->FSlowCPU(), &mcc, &fni, cnoNil);
	if (pmvie == pvNil)
		goto LFail;

	if (!pmvie->FAddToCmvi(&_cmvi, &_iscenMac))
		goto LFail;

	/* Refill the thumbnails */
	if (!_FResetThumbnails(fFalse))
		_ErrorExit();
	
LFail:
	ReleasePpo(&pmvie);
	return fTrue;
}


/******************************************************************************
	FCmdTransition
		Sets the transition for the scene corresponding to the given frame.
	
	Arguments:
		PCMD pcmd -- pointer to command data.  Extra parms are as follows:
			rglw[0] -- GOK id of the frame
			rglw[1] -- which transition to use
	
	Returns: fTrue if the command was handled by this routine
	
************************************************************ PETED ***********/
bool SCRT::FCmdTransition(PCMD pcmd)
{
	AssertThis(0);
	Assert(_iscenMac > 0, "Can't set transition when movie is empty");

	long iscen = _IscenFromKid(pcmd->rglw[0]);
	SCEND scend;
	PGOK pgokFrame = (PGOK)vapp.Pkwa()->PgobFromHid(pcmd->rglw[0]);

	if (pgokFrame == pvNil)
		{
		Bug("kid doesn't exist");
		return fTrue;
		}
	Assert(pgokFrame->FIs(kclsGOK), "kid isn't a GOK");

	AssertIn(iscen, 0, _iscenMac);
	_cmvi.pglscend->Get(iscen, &scend);
	scend.trans = _TransFromLw(pcmd->rglw[1] - 1);
	_cmvi.pglscend->Put(iscen, &scend);

	if (_FResetTransition(pgokFrame, scend.trans))
		pgokFrame->InvalRc(pvNil);

	return fTrue;
}


/******************************************************************************
	_SetSelectionVis
		Shows or hides the current selection.
	
	Arguments:
		bool fShow -- fTrue if we're to make the selection different from the
			other frames
		bool fHideSel -- fTrue if the selection state is hidden (actually,
			whatever "dragging" state the script defines) instead of the usual
			hilite.
	
************************************************************ PETED ***********/
void SCRT::_SetSelectionVis(bool fShow, bool fHideSel)
{
	if (_iscenCur < _iscenMac && FIn(_iscenCur - _iscenTop, 0, _cfrmPage))
		{
		PGOK pgok = (PGOK)vapp.Pkwa()->PgobFromHid(_KidFromIscen(_iscenCur));

		if (pgok != pvNil)
			{
			long lwState;

			Assert(pgok->FIs(kclsGOK), "Didn't get a GOK GOB");
			lwState = fShow ?
				(fHideSel ? kstBrowserScrollingSel : kstBrowserSelected) :
				kstBrowserEnabled;
			if (pgok->Sno() != lwState)
				pgok->FChangeState(lwState);
			}
		else
			Bug("Where's my frame to select?");
		}
}


/******************************************************************************
	_FResetThumbnails
		Refills the thumbnails for the scenes frames.  If the thumbnail
		actually changed, force a redraw.

	Arguments:
		bool fHideSel -- fTrue if the current selection should be hidden
			rather than highlighted

	Returns: fTrue if it was successful in refilling the thumbnails
	
************************************************************ PETED ***********/
bool SCRT::_FResetThumbnails(bool fHideSel)
{
	bool fSuccess = fFalse;
	long kidCur = _kidFrameMin - 1, kidFrameCur = _kidFrameMin;
	long iscen = _iscenTop, cFrame = _cfrmPage;
	long lwSelState = fHideSel ? kstBrowserScrollingSel : kstBrowserSelected;
	PGOK pgokFrame;
	PGOMP pgomp;
	SCEND scend;

	while (cFrame--)
		{
		bool fNewTransition;
		PMBMP pmbmp;
		RC rc;

		pgokFrame = (PGOK)vapp.Pkwa()->PgobFromHid(kidFrameCur);
		pgomp = GOMP::PgompFromHidScr(kidCur);

		if (pgokFrame == pvNil || pgomp == pvNil)
			{
			Bug("Couldn't find thumbnail or its frame");
			goto LFail;
			}
		Assert(pgomp->FIs(kclsGOMP), "Thumbnail GOB isn't a GOMP");
		Assert(pgokFrame->FIs(kclsGOK), "Frame GOB isn't a GOK");
		if (iscen < _iscenMac)
			{
			long lwStateNew;

			_cmvi.pglscend->Get(iscen, &scend);
			pmbmp = scend.pmbmp;
			lwStateNew = (iscen == _iscenCur) ? lwSelState : kstBrowserEnabled;
			if (pgokFrame->Sno() != lwStateNew &&
					!pgokFrame->FChangeState(lwStateNew))
				{
				goto LFail;
				}

			fNewTransition = _FResetTransition(pgokFrame, scend.trans);
			iscen++;
			}
		else
			{
			pmbmp = pvNil;
			if (pgokFrame->Sno() != kstBrowserDisabled &&
					!pgokFrame->FChangeState(kstBrowserDisabled))
				{
				goto LFail;
				}
			fNewTransition = fFalse;
			}
		if (pgomp->FSetMbmp(pmbmp) || fNewTransition)
			pgokFrame->InvalRc(pvNil);

		kidCur--;
		kidFrameCur += _cgokFrame;
		}

	/* Fill in selection frame; do this outside of the above loop, since the
		selection is not always actually within the range of displayed scene
		frames. */
	if (_iscenMac > 0)
		{
		_cmvi.pglscend->Get(_iscenCur, &scend);
		pgokFrame = (PGOK)vapp.Pkwa()->PgobFromHid(kidFrameCur);
		Assert(pgokFrame != pvNil, "Selection GOK is missing");
		Assert(pgokFrame->FIs(kclsGOK), "Frame GOB isn't a GOK");
		pgomp = GOMP::PgompFromHidScr(kidCur);
		Assert(pgomp != pvNil, "Selection GOMP is missing");
		_FResetTransition(pgokFrame, scend.trans);
		pgomp->FSetMbmp(scend.pmbmp);
		}

	_EnableScroll();

	fSuccess = fTrue;

LFail:
	return fSuccess;
}


const TRANS SCRT::_mplwtrans[] =
	{
	transCut,
	transDissolve,
	transFadeToBlack,
	transFadeToWhite,
	};
#define kctrans (size(_mplwtrans) / size(_mplwtrans[0]))


/******************************************************************************
	_FResetTransition
		Updates the transition buttons for a given scene frame.
	
	Arguments:
		PGOK pgokPar -- the parent scene frame GOK
		TRANS trans  -- the transition state for this scene
	
	Returns: fTrue if the scene has a new transition and the frame needs
		to be updated
	
************************************************************ PETED ***********/
bool SCRT::_FResetTransition(PGOK pgokPar, TRANS trans)
{
	bool fRedrawTrans = fFalse;
	PGOK pgokTrans, pgokThumb;
	long lwTrans = kctrans, lwThis = _LwFromTrans(trans);
	long lwStateCur, lwStateNew;

	pgokThumb = (PGOK)pgokPar->PgobFirstChild();
	Assert(pgokThumb->FIs(kclsGOK), "First child wasn't a GOK");
	pgokTrans = (PGOK)pgokThumb->PgobNextSib();
	Assert(pgokTrans->FIs(kclsGOK), "Transition GOB isn't a GOK");
	while (lwTrans--)
		{
		lwStateCur = pgokTrans->Sno();
		lwStateNew = (lwTrans != lwThis) ? kstDefault : kstSelected;
		if (lwStateCur != lwStateNew)
			{
			fRedrawTrans = fTrue;
			pgokTrans->FChangeState(lwStateNew);
			}
		pgokTrans = (PGOK)pgokTrans->PgobNextSib();
		Assert(pgokTrans == pvNil ||
			pgokTrans->FIs(kclsGOK), "Transition GOB isn't a GOK");
		}

	return fRedrawTrans;
}


/******************************************************************************
	_TransFromLw
		Map a long word to a transition
	
	Arguments:
		long lwTrans -- the scene sorter index of the transition
	
	Returns: the actual transition
	
************************************************************ PETED ***********/
TRANS SCRT::_TransFromLw(long lwTrans)
{
	AssertIn(lwTrans, 0, kctrans);

	return _mplwtrans[lwTrans];
}

/******************************************************************************
	_LwFromTrans
		Map a transition to the scene sorter long word
	
	Arguments:
		TRANS trans -- the transition
	
	Returns: the scene sorter long
	
************************************************************ PETED ***********/
long SCRT::_LwFromTrans(TRANS trans)
{
	long lw;

	for (lw = 0; lw < kctrans; lw++)
		if (_mplwtrans[lw] == trans)
			break;

#ifdef DEBUG
	if (lw == kctrans)
		{
		Bug("Invalid trans");
		lw = 0;
		}
#endif /* DEBUG */

	return lw;
}


RTCLASS(GOMP)

#ifdef DEBUG
void GOMP::AssertValid(ulong grf)
{
	GOMP_PAR::AssertValid(0);
	AssertNilOrPo(_pmbmp, 0);
}

void GOMP::MarkMem(void)
{
	AssertThis(0);

	GOMP_PAR::MarkMem();
	MarkMemObj(_pmbmp);
}
#endif /* DEBUG */


/******************************************************************************
	GOMP
		Constructor for the GOMP class.
	
	Arguments:
		PGCB pgcb    -- Gob Creation Block to be passed to the parent class
		PMBMP pmbmp  -- the MBMP to use when drawing this GOMP
	
************************************************************ PETED ***********/
GOMP::GOMP(PGCB pgcb) : GOB(pgcb)
{
	_pmbmp = pvNil;
	AssertThis(0);
}


/******************************************************************************
	PgompNew
		Creates a GOMP as a child of the given parent.  The new GOMP will
		be exactly the same size as the parent, and will have the given hid.
	
	Arguments:
		PMBMP pmbmp  -- the MBMP to draw as this GOMP
		PGOB pgobPar -- the parent of this GOMP
		long hid     -- the hid (kid) of this GOMP
	
	Returns: pointer to the GOMP if it succeeds, pvNil otherwise
	
************************************************************ PETED ***********/
PGOMP GOMP::PgompNew(PGOB pgobPar, long hid)
{
	AssertPo(pgobPar, 0);

	PGOMP pgomp = pvNil;
	GCB gcb;
	RC rcRel(0, 0, krelOne, krelOne);

	gcb.Set(hid, pgobPar, fgobNil, kginDefault, pvNil, &rcRel);

	pgomp = NewObj GOMP(&gcb);

	if (pgomp != pvNil && pgobPar->FIs(kclsGOK))
		{
		PT pt;
		RC rcAbs;

		((GOK *)pgobPar)->GetPtReg(&pt);
		pgomp->GetPos(&rcAbs, &rcRel);
		rcAbs.Offset(-pt.xp, -pt.yp);
		pgomp->SetPos(&rcAbs, &rcRel);
		}

	return pgomp;
}


/******************************************************************************
	Draw
		Draws the given GOMP
	
	Arguments:
		PGNV pgnv    -- the graphics environment in which to draw
		RC *prcClip  -- the clipping rect; this is ignored
	
************************************************************ PETED ***********/
void GOMP::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);

	RC rc;

	if (_pmbmp == pvNil)
		return;

	_pmbmp->GetRc(&rc);
	rc.OffsetToOrigin();
	pgnv->DrawMbmp(_pmbmp, &rc);
}


/******************************************************************************
	FSetMbmp
		Replaces the MBMP for this GOMP with the given MBMP.
	
	Arguments:
		PMBMP pmbmp -- pointer to the MBMP

	Returns: fTrue if the new MBMP is different from the old one
	
************************************************************ PETED ***********/
bool GOMP::FSetMbmp(PMBMP pmbmp)
{
	AssertNilOrPo(pmbmp, 0);

	bool fRedraw = (_pmbmp != pmbmp);

	ReleasePpo(&_pmbmp);
	if (pmbmp != pvNil)
		pmbmp->AddRef();
	_pmbmp = pmbmp;

	return fRedraw;
}


/******************************************************************************
	PgompFromHidScr
		Given an HID, return the GOMP with that hid
	
	Arguments:
		long hid -- the hid of the GOMP to find
	
	Returns: pointer to the GOMP
	
************************************************************ PETED ***********/
PGOMP GOMP::PgompFromHidScr(long hid)
{
	PGOMP pgomp;

	pgomp = (PGOMP)vapp.Pkwa()->PgobFromHid(hid);
	if (pgomp != pvNil)
		Assert(pgomp->FIs(kclsGOMP), "GOB isn't a GOMP");
	return pgomp;
}
