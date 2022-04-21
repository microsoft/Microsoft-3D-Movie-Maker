/*****************************************************************************\
 *	stdioscb.cpp
 *
 *	Author: ******
 *	Date: March, 1995
 *
 *	This file contains the studio scrollbars class SSCB.  These are the frame
 *	and scene scrollbar master controls.
 *
\*****************************************************************************/

#include "soc.h"
#include "studio.h"
ASSERTNAME

/*****************************************************************************\
 *
 *	The studio scrollbars class.
 *
\*****************************************************************************/

RTCLASS(SSCB)

/*****************************************************************************\
 *
 *	Constructor for the studio scrollbars.  This function is private; use
 *	PsscbNew() for public construction.
 *
\*****************************************************************************/
SSCB::SSCB(PMVIE pmvie)
{
	_pmvie = pmvie;
#ifdef SHOW_FPS
	_itsNext = 1;
	for (long its = 0; its < kctsFps; its++)
		_rgfdsc[its].ts = 0;
#endif // SHOW_FPS
}

/*****************************************************************************\
 *
 *	Public constructor for the studio scrollbars.
 *
 *	Parameters:
 *		pmvie	-- the owner movie
 *
 *	Returns:
 *		A pointer to the scrollbars object, pvNil if failed.
 *
\*****************************************************************************/
PSSCB SSCB::PsscbNew(PMVIE pmvie)
{
	AssertNilOrPo(pmvie, 0);

	PSSCB	psscb;
	PGOB 	pgob;
	STN		stn;
	GCB 	gcb;
	RC		rcRel, rcAbs;
	long 	hid;

	//
	// Create the view
	//
	if (pvNil == (psscb = NewObj SSCB(pmvie)))
		return pvNil;

	rcRel.xpLeft = rcRel.ypTop = 0;
	rcRel.xpRight = rcRel.ypBottom = krelOne;

	rcAbs.Set(0, 0, 0, 0);

	pgob = ((APP *)vpappb)->Pkwa()->PgobFromHid(kidFrameText);

	if (pgob == pvNil)
		{
		ReleasePpo(&psscb);
		return(pvNil);
		}

	hid = GOB::HidUnique();
	gcb.Set(hid, pgob, fgobNil, kginDefault, &rcAbs, &rcRel);

	if (pvNil == (psscb->_ptgobFrame = NewObj TGOB(&gcb)))
		{
		ReleasePpo(&psscb);
		return(pvNil);
		}

	pgob = ((APP *)vpappb)->Pkwa()->PgobFromHid(kidSceneText);

	if (pgob == pvNil)
		{
		ReleasePpo(&psscb);
		return(pvNil);
		}

	hid = GOB::HidUnique();
	gcb.Set(hid, pgob, fgobNil, kginDefault, &rcAbs, &rcRel);

	if (pvNil == (psscb->_ptgobScene = NewObj TGOB(&gcb)))
		{
		ReleasePpo(&psscb);
		return(pvNil);
		}

#ifdef SHOW_FPS
	pgob = ((APP *)vpappb)->Pkwa()->PgobFromHid(kidFps);

	if (pgob == pvNil)
		{
		ReleasePpo(&psscb);
		return(pvNil);
		}

	hid = GOB::HidUnique();
	gcb.Set(hid, pgob, fgobNil, kginDefault, &rcAbs, &rcRel);

	if (pvNil == (psscb->_ptgobFps = NewObj TGOB(&gcb)))
		{
		ReleasePpo(&psscb);
		return(pvNil);
		}
#endif // SHOW_FPS

	AssertPo(psscb, 0);
	return psscb;
}


/****************************************************
 *
 * Destructor for studio scroll bars.
 *
 ****************************************************/
SSCB::~SSCB(void)
{
	AssertBaseThis(0);

	// Don't need to Release these tgobs, since they get destroyed with
	// the gob tree.
	_ptgobFrame = pvNil;
	_ptgobScene = pvNil;
#ifdef SHOW_FPS
	_ptgobFps = pvNil;
#endif // SHOW_FPS
}


/*****************************************************************************\
 *
 *	FCmdScroll
 *		Handles scrollbar commands.  Cids to the SSCB are enqueued
 *		in the following format:
 *
 *		EnqueueCid(cid, khidSscb, chtt, param1, param2, param3);
 *
 *		where: cid  = cidFrameScrollbar or cidSceneScrollbar
 *		       chtt = the tool type
 *	
 *	Parameters:
 *		pcmd -- pointer to command info
 *	
 *	Returns:
 *		fTrue if the command was handled
 *
\*****************************************************************************/
bool SSCB::FCmdScroll(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	bool	fScene;
	bool	fThumbDrag = fFalse;
	long	lwDest, cxScrollbar, lwDestOld, xp;
	long	cRangeDest, iRangeDestFirst;
	long 	tool = -1;
	RC		rc;
	PSCEN	pscen = pvNil;

	// verify that the command is for the studio scrollbars
	if ((pcmd->cid != cidSceneScrollbar) &&
		(pcmd->cid != cidFrameScrollbar) &&
		(pcmd->cid != cidSceneThumb) &&
		(pcmd->cid != cidFrameThumb) &&
		(pcmd->cid != cidStartScroll))
		return fFalse;

	fScene = ((pcmd->cid == cidSceneScrollbar) || (pcmd->cid == cidSceneThumb));

	if (pcmd->cid == cidFrameScrollbar)
		{
		vpcex->FlushCid(cidFrameScrollbar);
		}

	// need a valid movie ptr, also a ptr to the current scene
	// when dealing with frame scrollbar
	if ((pvNil == _pmvie) || (!fScene && (pvNil == (pscen = _pmvie->Pscen()))))
		return fTrue;

	if (pcmd->cid == cidStartScroll)
		{

		if ((pcmd->rglw[0] == chttFButtonFW) || (pcmd->rglw[0] == chttFButtonRW))
			{
			_fBtnAddsFrames = ((pcmd->rglw[1] & fcustCmd) &&
			                   (((pscen->NfrmLast() == pscen->Nfrm()) && (pcmd->rglw[0] == chttFButtonFW)) ||
							    ((pscen->NfrmFirst() == pscen->Nfrm()) && (pcmd->rglw[0] == chttFButtonRW))));
			StartNoAutoadjust();
			}

		if (!_fBtnAddsFrames)
			{
			return fTrue;
			}

		}
	else
		{
		EndNoAutoadjust();
		}

	// what's the tool we are handling? (chtt is param0)
	switch (pcmd->rglw[0])
		{

		case chttFButtonFW:
		case chttSButtonFW:
		// forward one frame / scene
		if (fScene)
			{
			lwDest = _pmvie->Iscen() + 1;
			tool = toolFWAScene;
			}
		else
			{
			lwDest = pscen->Nfrm() + 1;
			if ((pscen->Nfrm() == pscen->NfrmLast()) && _fBtnAddsFrames)
				{
				_fBtnAddsFrames = fFalse;
				tool = toolAddAFrame;
				goto LExecuteCmd;
				}
			tool = toolFWAFrame;
			}
		break;

	case chttButtonFWEnd:
		// forward to end of scene / movie
		lwDest = klwMax;
		break;

	case chttFButtonRW:
	case chttSButtonRW:
		// back one frame / scene
		if (fScene)
			{
			lwDest = _pmvie->Iscen() - 1;
			tool = toolRWAScene;
			}
		else
			{
			lwDest = pscen->Nfrm() - 1;
			if ((pscen->Nfrm() == pscen->NfrmFirst()) && _fBtnAddsFrames)
				{
				_fBtnAddsFrames = fFalse;
				tool = toolAddAFrame;
				goto LExecuteCmd;
				}
			tool = toolRWAFrame;
			}
		break;

	case chttButtonRWEnd:
		// back to beginning of scene / movie
		lwDest = klwMin;
		break;

	case chttThumb:
		// released thumb tab - simulate a hit on scrollbar at this point
        // fall through

	case chttScrollbar:
		// hit scrollbar directly

		// calculate the source range of the mapping: mouse range along scrollbar
		cxScrollbar = _CxScrollbar(fScene ? kidSceneScrollbar : kidFrameScrollbar,
			fScene ? kidSceneThumb : kidFrameThumb);

		// param1 is the source value: the mouse click position
		// it should be within the source range (closed on the upper bound)
		xp = LwBound(pcmd->rglw[1], 0, cxScrollbar + 1);

		// calculate the destination range
		if (fScene)
			{
    		// scenes: [0, ..., number_of_scenes - 1]
			cRangeDest = _pmvie->Cscen();
			iRangeDestFirst = 0;
			}
		else
			{
    		// frames: [first_frame, ..., last_frame]
			cRangeDest = pscen->NfrmLast() - pscen->NfrmFirst() + 1;
			iRangeDestFirst = pscen->NfrmFirst();
			}

		// map the source value from the source range to the destination range
		lwDest = LwMulDiv(xp, cRangeDest, cxScrollbar) +
		    iRangeDestFirst;

		// if param2 is 0, we are dragging the thumb tab, just need to update
		// the counters
		if (pcmd->rglw[2] == 0)
			fThumbDrag = fTrue;
		break;

	default:
		Assert(fFalse, "invalid chtt from studio scroll");
		break;

		}

	// restrict to valid range
	lwDestOld = lwDest;
	lwDest = LwMin(fScene ? _pmvie->Cscen() - 1 : pscen->NfrmLast(),
		LwMax(fScene ? 0 : pscen->NfrmFirst(), lwDest));
	if (lwDestOld != lwDest)
		{
		tool = -1;
		}

LExecuteCmd:
	if (!_pmvie->FPlaying())
		{
		STN stn;

		if (fScene)
			{

			if (fThumbDrag)
				{
				// update scene counter
				stn.FFormatSz(PszLit("%4d"), lwDest + 1);
				_ptgobScene->SetText(&stn);
				return fTrue;
				}
			else
				{
				if (tool != -1)
					{
					_pmvie->Pmcc()->PlayUISound(tool);
					}

				// scene change
				if (!_pmvie->FSwitchScen(lwDest))
					{
					return fFalse;
					}

				if (_pmvie->FSoundsEnabled())
					{
					_pmvie->Pmsq()->PlayMsq();
					}
				else
					{
					_pmvie->Pmsq()->FlushMsq();
					}
				Update();
				}

			}
		else
			{
			if (fThumbDrag)
				{
				// update frame counter
				stn.FFormatSz(PszLit("%4d"), lwDest - pscen->NfrmFirst() + 1);
				_ptgobFrame->SetText(&stn);
				return fTrue;
				}
			else
				{
				// frame change
				if (tool != -1)
					{
					_pmvie->Pmcc()->PlayUISound(tool);
					}

				if (!pscen->FGotoFrm(lwDest))
					{
                	return fFalse;
					}

				if (_pmvie->FSoundsEnabled())
					{
					_pmvie->Pmsq()->PlayMsq();
					}
				else
					{
					_pmvie->Pmsq()->FlushMsq();
					}
				Update();
				}

			}

		}

		return fTrue;

	
}

/*****************************************************************************\
 *
 *	_CxScrollbar
 *		Calculates the slidable length of a scrollbar based on a non-zero
 *		width thumb tab which has its position associated with its leftmost
 *		pixel.
 *	
 *	Parameters:
 *		kidScrollbar	- id for the scrollbar gob
 *		kidThumb		- id for the thumb tab gob
 *	
 *	Returns:
 *		Length of the slidable region of the scrollbar
 *
\*****************************************************************************/
long SSCB::_CxScrollbar(long kidScrollbar, long kidThumb)
{
	AssertThis(0);

	long	cxThumb, cxScrollbar;
	PGOB	pgob;
	RC		rc;

	// rightmost pos we can slide the thumb tab is the pos where it has
	// its right edge at the max pos of the scrollbar, or in other words, it
	// is its own width away from the max pos of the scrollbar

	// calculate the thumb tab width
	if (pvNil == (pgob = ((APP *)vpappb)->Pkwa()->PgobFromHid(kidThumb)))
		return 0;
	pgob->GetRc(&rc, cooLocal);
	cxThumb = rc.xpRight - rc.xpLeft + 1;
	// calculate the scrollbar width
	if (pvNil == (pgob = ((APP *)vpappb)->Pkwa()->PgobFromHid(kidScrollbar)))
		return 0;
	pgob->GetRc(&rc, cooLocal);
	cxScrollbar = rc.xpRight - rc.xpLeft + 1 - cxThumb;

	return cxScrollbar;

}

/*****************************************************************************\
 *
 *	Update
 *		Update the studio scrollbars.
 *	
 *	Parameters:
 *		None.
 *	
 *	Returns:
 *		Nothing.
 *
\*****************************************************************************/
void SSCB::Update(void)
{
	AssertThis(0);

	PSCEN	pscen;
	STN		stn;
	PGOB	pgob;
	RC		rc;
	long	xp, dxp;
	long	cxScrollbar;

	// need a valid movie ptr, also a ptr to the current scene
	if ((pvNil == _pmvie) || (pvNil == (pscen = _pmvie->Pscen())))
		return;

	// update the frame scrollbar
	if (pvNil != (pgob = ((APP *)vpappb)->Pkwa()->PgobFromHid(kidFrameScrollbar)))
		{
		// source range: [first_frame, ..., last_frame]
		long cfrm = pscen->NfrmLast() - pscen->NfrmFirst() + 1;
		// source value: index of the current frame
		long ifrm = pscen->Nfrm() - pscen->NfrmFirst();

		pgob->GetRc(&rc, cooParent);
		if (cfrm < 2)
			{
			// special case to avoid divide by zero in scaling
			// we use cfrm - 1 as the size of the destination range so
			// the highest value will get mapped to the right endpoint
			xp = 0;
			}
		else
			{
			// source value should be in source range
			AssertIn(ifrm, 0, cfrm);

			// calculate destination range: mouse positions on scrollbar
  			cxScrollbar = _CxScrollbar(kidFrameScrollbar, kidFrameThumb);

			// map the source value from the source range to the destination range
			xp = LwMulDiv(ifrm, cxScrollbar, cfrm - 1);

			}

		// move the frame thumb tab to the appropriate position
		if (pvNil != (pgob = ((APP *)vpappb)->Pkwa()->PgobFromHid(kidFrameThumb)))
			{
			pgob->GetPos(&rc, pvNil);
			dxp = xp - rc.xpLeft;
			((PGOK)pgob)->FSetRep(chidNil, fgokNoAnim, ctgNil, dxp, 0, 0);
			}
		}

	// update the scene scrollbar
	if (pvNil != (pgob = ((APP *)vpappb)->Pkwa()->PgobFromHid(kidSceneScrollbar)))
		{
		// source range: [0, ..., num_scenes - 1]
		long cscen = _pmvie->Cscen();
		// source value: index of current scene
		long iscen = _pmvie->Iscen();

		pgob->GetRc(&rc, cooParent);
		if (cscen < 2)
			{
			// special case to avoid divide by zero in scaling
			// we use cscen - 1 as the size of the destination range so
			// the highest value will get mapped to the right endpoint
			xp = 0;
			}
		else
			{
			// source value should be in source range
			AssertIn(iscen, 0, cscen);

			// calculate destination range: mouse positions on scrollbar
			cxScrollbar = _CxScrollbar(kidSceneScrollbar, kidSceneThumb);

			// map the source value from the source range to the destination range
			xp = LwMulDiv(iscen, cxScrollbar, cscen - 1);

			}

		// move the scene thumb tab to the appropriate position
		if (pvNil != (pgob = ((APP *)vpappb)->Pkwa()->PgobFromHid(kidSceneThumb)))
			{
			pgob->GetPos(&rc, pvNil);
			dxp = xp - rc.xpLeft;
			((PGOK)pgob)->FSetRep(chidNil, fgokNoAnim, ctgNil, dxp, 0, 0);
			}
		}

	// update the other stuff, frame and scene counters, fps

	// update frame counter
	if (_fNoAutoadjust)
		{
		stn.FFormatSz(PszLit("%4d"), pscen->Nfrm() - _nfrmFirstOld + 1);
		}
    else
		{
		stn.FFormatSz(PszLit("%4d"), pscen->Nfrm() - pscen->NfrmFirst() + 1);
		}
	_ptgobFrame->SetText(&stn);

	// update scene counter
	stn.FFormatSz(PszLit("%4d"), _pmvie->Iscen() + 1);
	_ptgobScene->SetText(&stn);

#ifdef SHOW_FPS
	{
	long cfrmTail, cfrmCur;
	ulong tsTail, tsCur;
	float fps;

	/* Get current info */
	tsCur = TsCurrent();
	cfrmCur = _pmvie->Cnfrm();

	/* Get least recent frame registered */
	tsTail = _rgfdsc[_itsNext].ts;
	cfrmTail = _rgfdsc[_itsNext].cfrm;

	/* Register current frame */
	_rgfdsc[_itsNext].ts = tsCur;
	_rgfdsc[_itsNext++].cfrm = cfrmCur;

	if (tsTail < _pmvie->TsStart())
		{
		tsTail = _pmvie->TsStart();
		cfrmTail = 0;
		}

	Assert(_itsNext <= kctsFps, "Bogus fps next state");
	if (_itsNext == kctsFps)
		_itsNext = 0;

	fps = ((float)((cfrmCur - cfrmTail) * kdtsSecond))/((float)(tsCur - tsTail));

	stn.FFormatSz(PszLit("%2d.%02d fps"), (int)fps,
		(int)((fps - (int)fps) * 100));
	_ptgobFps->SetText(&stn);
	}
#endif // SHOW_FPS

}

/*****************************************************************************\
 *
 *	Change the owner movie for these scroll bars
 *
 *	Parameters:
 *		pmvie - new movie
 *
 *	Returns:
 *		Nothing.
 *
\*****************************************************************************/
void SSCB::SetMvie(PMVIE pmvie)
{
	_pmvie = pmvie;
	Update();
}

/*****************************************************************************\
 *
 *	Start a period of no autoadjusting on the scroll bars
 *
 *	Parameters:
 *		Nothing.
 *
 *	Returns:
 *		Nothing.
 *
\*****************************************************************************/
void SSCB::StartNoAutoadjust(void)
{
	AssertThis(0);

	_fNoAutoadjust = fTrue;

	Assert(_pmvie->Pscen() != pvNil, "Bad scene");
	_nfrmFirstOld = _pmvie->Pscen()->NfrmFirst();
	Update();
}


void SSCB::SetSndFrame(bool fSoundInFrame)
{
	long snoNew = fSoundInFrame ? kst2 : kst1;
	PGOK pgokThumb = (PGOK)vapp.Pkwa()->PgobFromHid(kidFrameThumb);

	if (pgokThumb != pvNil && pgokThumb->FIs(kclsGOK))
		{
		if (pgokThumb->Sno() != snoNew)
			pgokThumb->FChangeState(snoNew);
		}
	else
		Bug("Missing or invalid thumb GOB");
}


#ifdef DEBUG

/*****************************************************************************\
 *
 *	Mark memory used by the SSCB
 *
 *	Parameters:
 *		None.
 *
 *	Returns:
 *		Nothing.
 *
\*****************************************************************************/
void SSCB::MarkMem(void)
{
	AssertThis(0);

	SSCB_PAR::MarkMem();
}

/*****************************************************************************\
 *
 *	Assert the validity of the SSCB
 *
 *	Parameters:
 *		grf - bit array of options.
 *
 *	Returns:
 *		Nothing.
 *
\*****************************************************************************/
void SSCB::AssertValid(ulong grf)
{
	SSCB_PAR::AssertValid(fobjAllocated);
}

#endif // DEBUG
