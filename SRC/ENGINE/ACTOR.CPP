/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Actor Engine

	Primary Author : *****
	Review Status: Reviewed
	
	Actors contain:
	1. A path, pglrpt, which is a gl of route points, the nodes of the path.
	2. An event list, pggaev, which is a list of variable size events
		occurring on or between nodes on the path.
	3. A motion match list of sounds applicable during the current action.	
	4. State variables	
	
	See Movie.doc for a detailed description of the functionality.

	Routes consists of one or more subroutes which can be separately
	translated, deleted, etc.  
	
	This is a *where* based model of editing.  Events occur at specified
	locations along the path.  The path is linear between nodes.  Each
	node represents the point at which the actor was located in the original
	path recording session.  It is not redefined on resizing or motion fill.
	Instead, the event list is edited.  The mouse input is however, smoothed
	in movie before actor is called.

	The one exception to events being executed based on location is the
	aetAdd event, which is executed at its specified time.
	Due to motion fill inheriting the static or non-static path property,
	note that it is (no longer) possible for motion fill to cause an
	actor to not reach the end of a subroute.

	Misc comments:

	Actors begin each subroute with the	default costume, reimposed
	automatically by the code at every new subroute.	Any
	initialization events are then CHANGE events, post applied via
	events in the event stream.	When a new subroute is created, the
	new initialization events are copied from the previous subroute, 
	if one exists, and from the following subroute if no previous subroute 
	exists.

	Subroutes can be pushed in time by record, rerecord, & sooner/later.
	An actor is represented at at most one location at any given time.

	All events located <= _rtelCur on the path are executed at the end
	of any given frame. (See FGotoFrame()). 

	Each subroute has an aetAdd event as its first event in the event list.

	Rotation transformations are absolute, not cumulative. The transformation
	effective at the current frame is stored in _xfrm.  There are now two
	fundamentally different types of rotation.  The aetRotF represent forward
	rotation and have path orientation applied concurrently.  The aetRotH 
	apply to static segments only and do not have path orientation post
	applied.  _xfrm represents both, and _fUseBmat34 flags (for each frame)
	which transformation is to be applied when the body is positioned.
	The aetRotH events take precedence over aetRotF events if both exist in the
	same frame. This is necessary to allow the aetRotF events to control future
	path rotation while still allowing single frame orientation.  The aetRotF
	events nuke the aetRotH events, however, in order to allow the user to 
	visibly see the forward rotation currently being applied. 

	Costume changes are per body part set.	

	Freeze inhibits cel advancement. 
	Step size events control floating as well as cel advancement in place.
	Each subpath is terminated with a step=0 and a freeze event.

	Each actor is visible (not hidden, though possibly out of view) between
	aetAdd and aetRem events.

	The stretch events, aetPull, do not alter step size.  Uniform stretching,
	aetSize events, do.

	Move events are used to translate the path forward of the event by the
	delta-xyz value in the event.  These are therefore cumulative, so that
	(unlike rotations), a move at frame 50 followed by a move at frame 20
	will result in frame 50 -to- end-of-subroute being translated by the sum
	of the translations at frames 20 and 50.  Tweak events do not translate
	the path, but instead change the current display location only.
	
	This code carefully maintains wysiwyg.  There are two parts to this.
	One part is the maintenance of exact location, so that if a recorded
	actor follows a specific path (eg, through rather than into doorways)
	that path is retained exactly.  A second part is that by <calculating>
	exact locations (unaltered by numerical roundoff), a carefully edited
	and synchronized actor will not end up one frame off in time on replay.
	Both were considered high priority be design.

	Actors currently proceed to the end of their path, taking a potentially
	partial step at the end.  As a result, to maintain wysiwyg, actors
	display at the location of aetActn and aetStep events.  
	NOTE: This UI decision probably added more complexity to the
	code than it gained in UI functionality.  Had actors always
	displayed only at full step increments, these scenarios would not
	include wysiwyg issues.

	Frame numbers are stored with the events, but are not guaranteed to be 
	correct for frame numbers larger than current frame _nfrmCur. 
	FComputeLifetime() guarantees these for all frames, but FComputeLifetime()
	can fail -> the code should not rely on future nfrm values.

	The route is translated by _dxyzFullRte in class ACTR.
	Each subroute is additionally translated by _dxyzSubRte.
	_dxyzRte combines the overall actor translation for efficiency only.
																								
***************************************************************************/
#include "frame.h"
#include "soc.h"

ASSERTNAME

RTCLASS(ACTR)

/***************************************************************************

	Constructor for ACTR - private.

***************************************************************************/
ACTR::ACTR(void)
{
	_arid = aridNil;
	_tagTmpl.sid = ksidInvalid;
	_tagSnd.sid = ksidInvalid;
	_nfrmCur = _nfrmFirst = knfrmInvalid;
	_nfrmLast = klwMin;
	_fLifeDirty = fFalse;
}


/***************************************************************************
	
	Destructor for an ACTR

***************************************************************************/
ACTR::~ACTR(void)
{
	AssertBaseThis(0);

	_CloseTags();
	ReleasePpo(&_pbody);
	ReleasePpo(&_pggaev);
	ReleasePpo(&_pglrpt);
	ReleasePpo(&_pglsmm);
	ReleasePpo(&_ptmpl);
}


/***************************************************************************
	
	Initialize the actor
	The actor will not yet be grounded to any initial scene frame.

***************************************************************************/
bool ACTR::_FInit(TAG *ptagTmpl)
{
	AssertBaseThis(0);
	AssertVarMem(ptagTmpl);
	
	_ptmpl = (PTMPL)vptagm->PbacoFetch(ptagTmpl, TMPL::FReadTmpl);
	if (pvNil == _ptmpl)
		return fFalse;
	
	AssertPo(_ptmpl, 0);
	_tagTmpl = *ptagTmpl;
	TAGM::DupTag(&_tagTmpl);
	
	if (!_FCreateGroups())
		return fFalse;

	_SetStateRewound();
	if (!_ptmpl->FGetGrfactn(0, &_grfactn))
		return fFalse;
		
	return fTrue;
}


/***************************************************************************
	
	Initialize the transformation matrix & factors
	(Sets the absolute scale = 1, and use the rest orientation)

***************************************************************************/
void ACTR::_InitXfrm(void)
{
	AssertThis(0);

	_InitXfrmRot(&_xfrm.bmat34Fwd);
	_InitXfrmRot(&_xfrm.bmat34Cur);
	_xfrm.aevpull.rScaleX = rOne;
	_xfrm.aevpull.rScaleY = rOne;
	_xfrm.aevpull.rScaleZ = rOne;
	_xfrm.rScaleStep = rOne;
	_xfrm.xaPath = aZero;
	_xfrm.yaPath = aZero;
	_xfrm.zaPath = aZero;
}


/***************************************************************************
	
	Initialize the rotation part of the transformation
	The rest orientation is now applied post user-rotations so that
	user rotations can be with respect to the actor's coordinate system

***************************************************************************/
void ACTR::_InitXfrmRot(BMAT34 *pbmat34)
{
	AssertThis(0);
	AssertVarMem(pbmat34);
	BrMatrix34Identity(pbmat34);
}


/***************************************************************************
	
	Allocate a new actor
	The actor is not attached to a scene until SetPscen is called.
	The actor will not have an identifiable ID until SetArid is called.
	Note that an actor which is not yet added to a scene is neither
	grounded in time (_nfrmFirst) nor space (_dxyzFullRte).

***************************************************************************/
PACTR ACTR::PactrNew(TAG *ptagTmpl)
{
	AssertVarMem(ptagTmpl);

	PACTR pactr;

	if ((pactr = NewObj ACTR()) == pvNil)
		return pvNil;

	if (!pactr->_FInit(ptagTmpl))
		{
		ReleasePpo(&pactr);
		return pvNil;
		}

	return pactr;
}


/***************************************************************************
	
	Create the groups _pggaev, _pglrpt and _pglsmm

***************************************************************************/
bool ACTR::_FCreateGroups(void)
{
	AssertBaseThis(0);

	if (pvNil == (_pggaev = GG::PggNew(size(AEV), kcaevInit, kcbVarAdd)))
		return fFalse;

	if (pvNil == (_pglrpt = GL::PglNew(size(RPT), kcrptGrow)))
		return fFalse;
	_pglrpt->SetMinGrow(kcrptGrow);

	if (pvNil == (_pglsmm = GL::PglNew(size(SMM), kcsmmGrow)))
		return fFalse;
	_pglsmm->SetMinGrow(kcsmmGrow);

	return fTrue;
}


/***************************************************************************
	
	Set the owning scene and the brender world for the body.

***************************************************************************/
void ACTR::SetPscen(SCEN *pscen)
{
	AssertBaseThis(0);

	AssertVarMem(pscen);
	Assert(_pscen == pvNil || _pscen == pscen, "SetPscen logic error");
	
	if (_pscen != pvNil)
		{
		Assert(_pbody != pvNil, "Bad body pointer");
		return;
		}

	Assert(_pbody == pvNil, "Bad body pointer");
	_pscen = pscen;

	// Create the body parts
	if (pvNil == (_pbody = _ptmpl->PbodyCreate()))
		return;

	_pbody->SetBwld(pscen->Pmvie()->Pbwld());
}				


/***************************************************************************
	
	Hide the Actor and initialize the Actor State Variables
	
***************************************************************************/
void ACTR::_InitState(void)
{
	AssertBaseThis(0);

	_Hide();
	_fLifeDirty = fTrue;
	if (_ptmpl != pvNil && _pbody != pvNil)
		_ptmpl->FSetDefaultCost(_pbody);
	_SetStateRewound();
}



/***************************************************************************
	
	Set the actor state variables to the rewound position
	
***************************************************************************/
void ACTR::_SetStateRewound(void)
{
	AssertBaseThis(0);

	_fFrozen = fFalse;
	_fModeRecord = fFalse;
	_anidCur = _celnCur = 0;
	_rtelCur.irpt = 0;
	_rtelCur.dwrOffset = rZero;
	_rtelCur.dnfrm = -1;
	if (_pglrpt->IvMac() > 0)
		{
		RPT rpt;
		_pglrpt->Get(0, &rpt);
		_xyzCur = rpt.xyz;
		}
	
	_iaevCur = 0;
	_iaevActnCur = _iaevAddCur = ivNil;
	_dwrStep = rZero;
	_iaevFrmMin = 0;
	_InitXfrm();
}


/***************************************************************************
	
	Prepare the actor for display in frame nfrm.

	nfrm can be any frame number in the movie - it is actor independent.
	NOTE: The state of this actor is expected to be current for frame _nfrmCur
	at the time this routine is called.
	FGotoFrame exits with _nfrmCur == nfrm.
	(A scene level interrogation of all actors should find
	that each actor's _nfrmCur is the same).

	Note: The scope of fPosition dirty spans multiple calls to _FDoFrm
	
***************************************************************************/
bool ACTR::FGotoFrame(long nfrm, bool *pfSoundInFrame)
{
	AssertThis(0);
	AssertIn(nfrm, klwMin, klwMax);

	bool fPositionBody;
	bool fSuccess = fTrue;
	bool fPositionDirty = fFalse;
	bool fQuickMethodValid;
	long iaevT = -1;
	long iaev;
	AEV *paev;

	if (nfrm == _nfrmCur)
		return fTrue;

	// Initialization
	if (nfrm < _nfrmCur || _nfrmCur == knfrmInvalid)
		{
		bool fMidPath = FPure(_pggaev->IvMac() > 0 && _iaevCur > 0 &&
			 nfrm > _nfrmFirst);

		// Note: If nfrm < _nfrmFirst, we may be adding
		// a new earliest add event.
		if (nfrm < _nfrmFirst && _fOnStage)
			_Hide();

		if (nfrm > _nfrmFirst && _nfrmCur != knfrmInvalid )
			{
			Assert(0 < _iaevCur, "Invalid state variables in FGotoFrame()");
			// Optimize if there are no events in the current frame
			paev = (AEV *)_pggaev->QvFixedGet(_iaevCur - 1);
			if (paev->nfrm < nfrm)
				{
				if (!_FQuickBackupToFrm(nfrm, &fQuickMethodValid))
					return fFalse;
				// Vertical segments can prevent _FQuickBackupToFrm()
				// from being valid.  If so, use default method.
				if (fQuickMethodValid)
					return fTrue;
				}
			}

		// Will be walking forward from the nearest earlier add event
		// Search backward to find it.
		if (fMidPath)
			{
			for (iaev = _iaevCur - 1; iaev >= 0; iaev--)
				{
				paev = (AEV *)_pggaev->QvFixedGet(iaev);
				if (paev->aet == aetAdd && paev->nfrm <= nfrm)
					{
					iaevT = iaev;
					break;
					}
				}
			}
		_SetStateRewound();
		if (fMidPath)
			{
			AssertIn(iaevT, 0, _pggaev->IvMac());
			_iaevFrmMin = _iaevCur = _iaevAddCur = iaevT;
			paev = (AEV *)_pggaev->QvFixedGet(_iaevCur);
			_rtelCur = paev->rtel;
			_nfrmCur = paev->nfrm;  	
			Assert(paev->aet == aetAdd, "Illegal _iaevAddCur state var");
			_rtelCur.dnfrm--;
			_GetXyzFromRtel(&_rtelCur, &_xyzCur);
			}
		else
			{
		   	_nfrmCur = LwMin(nfrm, _nfrmFirst);
			}
		_ptmpl->FSetDefaultCost(_pbody);
		}
	else
		{
		_nfrmCur++;
		_iaevFrmMin = _iaevCur;	// Save 1st event of this frame
		}

	if (nfrm < _nfrmFirst)
		return fTrue;

	// Trivial case: no events for this actor
	// _nfrmCur always reflects the movie's current frame
	if (_pggaev->IvMac() == 0)
		{
		_nfrmCur = nfrm;
		return fTrue;
		}
	
	while (_nfrmCur <= nfrm)
		{
		fPositionBody = (_nfrmCur == nfrm);
		if (!_FDoFrm(fPositionBody, &fPositionDirty, pfSoundInFrame))
			fSuccess = fFalse;

		if (nfrm == _nfrmCur)
			break;

		_nfrmCur++;
		_iaevFrmMin = _iaevCur;
		AssertIn(_iaevActnCur, -1, _pggaev->IvMac());
		}

	return fSuccess;
}

/***************************************************************************
	
	Backup To a smaller frame.  	(Optimization)
	Return *pfQuickMethodValid fTrue on success.
	Return *pfQuickMethodValid fFalse to if this method invalid here.

***************************************************************************/
bool ACTR::_FQuickBackupToFrm(long nfrm, bool *pfQuickMethodValid)
{
 	AssertThis(0);
	AssertVarMem(pfQuickMethodValid);
	Assert(nfrm < _nfrmCur, "Illegal call to _FBackupToFrm");
	
	long ifrm;
	RTEL rtelT;
	XYZ xyzOld = _xyzCur;
	XYZ xyzT;
	long dnfrm = _nfrmCur - nfrm;

#ifdef DEBUG
	AEV *paev;
	paev = (AEV *)_pggaev->QvFixedGet(_iaevCur - 1);
	Assert(paev->nfrm < nfrm, "Invalid Call to _FQuickBackupToFrm()") ;
#endif //DEBUG

	// Don't try to back up during frames beyond the actor's lifetime
 	if (nfrm >= _nfrmLast)
		{
		// GotoFrame() must always exit with _nfrmCur being valid
		// Otherwise, edits will be located at incorrect frames
		_nfrmCur = nfrm;
#ifdef BUG1906
		_rtelCur.dnfrm -= dnfrm;
#else  //!BUG1906
		_rtelCur.dnfrm--;
#endif //!BUG1906
		return fTrue;
		}

	// There are no events between here and the destination frame
	// _iaevFrmMin need not change

	// Walk to the destination location
	// Set the cel of the action
	for (ifrm = _nfrmCur - 1; ifrm >= nfrm; ifrm--)
		{
		if (!_FGetRtelBack(&_rtelCur, fTrue))
			goto LFail;
		}

	_GetXyzFromRtel(&_rtelCur, &_xyzCur);
	_nfrmCur = nfrm;

	if (dnfrm > 1 || xyzOld.dxr != _xyzCur.dxr || xyzOld.dzr != _xyzCur.dzr)
		{
		// Check for transitions to vertical motion
		if (!_FGetRtelBack(&rtelT, fFalse))
			goto LFail;

		_GetXyzFromRtel(&rtelT, &xyzT);

		if (_xyzCur.dxr == xyzT.dxr && _xyzCur.dzr == xyzT.dzr)
			{
			// Vertical motion next	(backing up)
			// -> Require _fUseBmat34Cur == fTrue, but bmat34Cur is not
			// yet computed.
			// -> Quick backup insufficient.
			*pfQuickMethodValid = fFalse;
			return fTrue;
			}
		}

	// Send motion match sounds to Msq to play
	if (!(_pscen->GrfScen() & fscenSounds) && (nfrm <= _nfrmLast) && _fOnStage)
		_FEnqueueSmmInMsq();  	// Ignore failure

	// Position the actor
	_PositionBody(&_xyzCur);	
	if (_fOnStage)
		{
		if (!_ptmpl->FSetActnCel(_pbody, _anidCur, _celnCur, pvNil))
			goto LFail;
		}
	*pfQuickMethodValid = fTrue;	
	return fTrue;

LFail:
	*pfQuickMethodValid = fFalse;
	return fFalse;
}


/***************************************************************************
	
	Compute xyz for one step backwards from the current location, as per
	state variables

***************************************************************************/
bool ACTR::_FGetRtelBack(RTEL *prtel, bool fUpdateStateVar)
{
	AssertThis(0);
	AssertVarMem(prtel);

	long celnSav = _celnCur;
	long nfrmSav = _nfrmCur;
	RTEL rtelSav = _rtelCur;
	AEV *paev;
	BRS dwrStep;
	RTEL rtelAdd;
	BRS dwrT;

	paev = (AEV *)_pggaev->QvFixedGet(_iaevAddCur);
	rtelAdd = paev->rtel;
		
	if (!_fFrozen)
		_celnCur--;
	if (rZero == _dwrStep ||
		(rtelAdd.irpt == _rtelCur.irpt && rtelAdd.dwrOffset == _rtelCur.dwrOffset))
		{
		_nfrmCur--;
		_rtelCur.dnfrm--;
		goto LEnd;
		}

	// Set the location to display this actor
	if (!_FGetDwrPlay(&dwrStep))
		goto LFail;

	dwrT = BrsSub(_rtelCur.dwrOffset, dwrStep);

	if (dwrT >= rZero)
		_rtelCur.dwrOffset = dwrT;
	else
		{
		RPT rpt;
		while(dwrT < rZero)
			{
			if (_rtelCur.irpt <= 0)
				{
				Bug("Corrupted event list");
				_rtelCur.irpt = 0;
				dwrT = rZero;
				break;
				}
			_rtelCur.irpt--;
			_pglrpt->Get(_rtelCur.irpt, &rpt);
			dwrT = BrsAdd(rpt.dwr, dwrT);
			}
		_rtelCur.dwrOffset = dwrT;
		}
	_nfrmCur--;

LEnd:
	*prtel = _rtelCur;
	if (!fUpdateStateVar)
		{
		_celnCur = celnSav;
		_nfrmCur = nfrmSav;
		_rtelCur = rtelSav;
		}
	return fTrue;

LFail:
	_celnCur = celnSav;
	_nfrmCur = nfrmSav;
	_rtelCur = rtelSav;
	return fFalse;
}


/***************************************************************************
	
	Update the internal actor state variables to frame _nfrmCur.
	Ready the actor to display in frame _nfrmCur if fPositionBody is fTrue.
	Sets *pfPositionDirty if a change is encountered.
	Clears *pfPositionDirty after updating Brender.

	Assumes previous frame correctly rendered.
	Note: PositionBody is delayed until after Events are processed - Tweak,
	Actn, Step and Transform events all can alter the location or orientation.
	Also, new Action Events precede Cel Positioning

	Note: The scope of *pfPositionDirty spans multiple _FDoFrm calls

***************************************************************************/
bool ACTR::_FDoFrm(bool fPositionBody, bool *pfPositionDirty, bool *pfSoundInFrame)
{
	AssertThis(0);
	AssertVarMem(pfPositionDirty);

	AEV aev;
	BRS dwr;
	long iaev;
	long iaevAdd;
	bool fEndRoute;
	XYZ xyzOld = _xyzCur;
	bool fFreezeThisCel = _fFrozen;
	bool fEndSubEvents = fFalse;
	bool fSuccess = fTrue;
	bool fAdvanceCel;

	// Obtain distance to move.	This may be shortened on encountering
	// a aetActn event later in this same frame.
	fSuccess = _FGetDwrPlay(&dwr);
	_AdvanceRtel(dwr, &_rtelCur, _iaevCur, _nfrmCur, &fEndRoute);
	_GetXyzFromRtel(&_rtelCur, &_xyzCur);

	// Use the pre-path rotation matrix if the actor moves
	// so that the path orientation can later be post applied.
	_fUseBmat34Cur = (dwr == rZero ||
		(xyzOld.dxr == _xyzCur.dxr && xyzOld.dzr == _xyzCur.dzr));
		
	// Locate the next Add event before entering the next loop
	// Add events are executed when their absolute frame number == _nfrmCur
	if (!_fModeRecord)
		{
		iaevAdd = _iaevCur - 1;
		while(_FFindNextAevAet(aetAdd, iaevAdd + 1, &iaevAdd))
			{
			if (!_FIsAddNow(iaevAdd))
				break;

			// Add is now	
			_iaevCur = iaevAdd;
			if (!_FDoAevCur())
				return fFalse;

			*pfPositionDirty = fTrue;
			}

		// Process any events through a dwr step size, unless an aetActn event
		// shortens that distance.
		// An aetActn Event will change _rtelCur at the time it is executed
		for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
			{
			_pggaev->GetFixed(iaev, &aev);
			if (aev.aet == aetAdd)
				{
				if (!_FIsAddNow(iaev))
					break;
			
				// Add is now
				if (!_FDoAevCur())
					return fFalse;
					
				*pfPositionDirty = fTrue;
				continue;
				}
		
			if (aev.rtel > _rtelCur)
				break;

			if (aetRotF == aev.aet || aetSize == aev.aet ||
				aetPull == aev.aet || aetRotH == aev.aet || aetMove == aev.aet)
				{
				// The xyz position is not necessarily changing.
				// Specifically enforce Brender updating
				*pfPositionDirty = fTrue;
				}
			else if ((aetActn == aev.aet) || (aetAdd == aev.aet))
				{
				//Do not increment _celnCur before displaying actor on new entrance/action
				fFreezeThisCel = fTrue;	
				*pfPositionDirty = fTrue;   	
				}

			if (pfSoundInFrame != pvNil && aev.aet == aetSnd && fPositionBody)
				*pfSoundInFrame = fTrue;

			// Non-motion match sounds cannot depend on playing here as
			// there might not be a sound event at the ending frame.
			// Therefore _FDoAevCur() does not enqueue motion match sounds.
			// It enters mm-snds in the smm.  _FDoAevCur() enqueues non-mm snds.
			if (aev.aet != aetSnd ||
				(fPositionBody && !(_pscen->GrfScen() & fscenSounds)))
				{
				// Play non sounds
				// Play sounds if this is the final frame (mm or non mm)
				if (!_FDoAevCur())
					fSuccess = fFalse;
				}
			else
				{
				// Motion match sounds must be entered in the smm
				// whether this is the final frame or not
				AEVSND aevsnd;
				_pggaev->Get(iaev, &aevsnd);
				if (aevsnd.celn != smmNil)
					{
					if (!_FDoAevCur())
						fSuccess = fFalse;
					}
				else
					{
					// Skip the current sound event
					_iaevCur++;
					}
				}						
			}
	  	}

	fEndSubEvents = _FIsDoneAevSub(_iaevCur, _rtelCur);
	fAdvanceCel = (!fFreezeThisCel && !(fEndRoute && fEndSubEvents));

	if (fAdvanceCel)
		{
		_celnCur++;
		if (_ccelCur > 1)		
			*pfPositionDirty = fTrue;
		}

	// Force Brender to update if the xyz position has changed
	if (xyzOld != _xyzCur)
		*pfPositionDirty = fTrue;

	// Position even if hidden for clipping region detection
	// fPositionBody avoids extraneous positioning on intermed frames
	if (fPositionBody)
		{
		// Enqueue the motion match sounds from the smm.  Ie, enter in the msq
		if (!(_pscen->GrfScen() & fscenSounds) && (_nfrmCur <= _nfrmLast) && _fOnStage)
			_FEnqueueSmmInMsq();

		if (*pfPositionDirty)
			{
			_PositionBody(&_xyzCur);	
			*pfPositionDirty = fFalse;
			}

		// Position the actor in the next cel
		// Do not do so if at the "end of route" & "end of events"
		if (_fOnStage)
			{
			if (!_ptmpl->FSetActnCel(_pbody, _anidCur, _celnCur, pvNil))
				fSuccess = fFalse;
			}
		}
	else if (*pfPositionDirty)
		{
		BMAT34 bmat34;
		_MatrixRotUpdate(&_xyzCur, &bmat34);
		}
	return fSuccess;
}


/***************************************************************************

	FReplayFrame : Replays the sound for the current frame
		-> Re-enqueues sounds for the current frame
	grfscen - Things that are supposed to be played right now.

***************************************************************************/
bool ACTR::FReplayFrame(long grfscen)
{
	AssertThis(0);

	AEV aev;
	long iaev;

	// Check if there is anything to do
	if (!(grfscen & fscenSounds) || !_fOnStage)
		return fTrue;

	for (iaev = _iaevFrmMin; iaev < _iaevCur; iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.aet != aetSnd)
			continue;

		if (!_FDoAevCore(iaev))
			return fFalse;
		}
	
	// Also send any motion match sounds to msq to play
	return _FEnqueueSmmInMsq();
}
		
/***************************************************************************

	_FGetStatic : Returns true/false on success/failure
	Returns the bool value in *pfStatic

***************************************************************************/
bool ACTR::_FGetStatic(long anid, bool *pfStatic)
{
	AssertThis(0);
	AssertIn(anid, 0, klwMax);
	AssertVarMem(pfStatic);

	ulong grfactn;

	if (!_ptmpl->FGetGrfactn(anid, &grfactn))
		return fFalse;

	if (ivNil == _iaevActnCur)
		{
		*pfStatic = fTrue;
		return fTrue;
		}

	*pfStatic = FPure(grfactn & factnStatic);
	return fTrue;
}


/***************************************************************************
	
	Is the actor in the last active frame of the subroute?

***************************************************************************/
bool ACTR:: _FIsDoneAevSub(long iaev, RTEL rtel)
{
	AssertBaseThis(0);
	AssertIn(iaev, 0, _pggaev->IvMac() + 1);
	
	AEV aev;
	if (iaev == _pggaev->IvMac())
		return fTrue;
	
	for ( ; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.aet == aetAdd)
			return fTrue;
		if (aev.rtel > rtel)
			return fFalse;
		// Event at current frame.  Keep looking.
		}
	
	// No further events exist at future frames for this subpath
	return fTrue;
}


/***************************************************************************
	
	Is the actor at an AddOnStage event which can be applied now?

***************************************************************************/
bool ACTR::_FIsAddNow(long iaev)
{
	AssertBaseThis(0);
	AssertIn(iaev, 0, _pggaev->IvMac());

	AEV *paev;

	if (_fLifeDirty)
		{
		// Update the nfrm values
		if (!_FComputeLifetime())
			return fFalse;
		}

	Assert(_pggaev->Cb(iaev) == size(AEVADD), "Corrupt event list");
	paev = (AEV *)_pggaev->QvFixedGet(iaev);

	if (paev->nfrm <= _nfrmCur)
		return fTrue;

	return fFalse;
}


/***************************************************************************
	
	Return functional (sized) step size when playing
	Zero is a valid return value

***************************************************************************/
bool ACTR::_FGetDwrPlay(BRS *pdwr)
{
	AssertBaseThis(0);
	AssertVarMem(pdwr);

	*pdwr = rZero;

	if (kdwrNil == _dwrStep)
		{
		if (!_ptmpl->FGetDwrActnCel(_anidCur, _celnCur, pdwr))
			return fFalse;
		}
	else
		{
		*pdwr = _dwrStep;
		}

	*pdwr = BrsMul(*pdwr, _xfrm.rScaleStep);
	
	return fTrue;
}


/***************************************************************************
	
	Return functional (sized) step size when recording.
	The returned size is expected to be authored > 0

***************************************************************************/
bool ACTR::_FGetDwrRecord(BRS *pdwr)
{
	AssertBaseThis(0);
	AssertVarMem(pdwr);

	*pdwr = rZero;

	if (!_ptmpl->FGetDwrActnCel(_anidCur, _celnCur, pdwr))
		return fFalse;
	
	*pdwr = BrsMul(*pdwr, _xfrm.rScaleStep);

	return fTrue;
}


/***************************************************************************
	
	Execute the current event.
	State Variables are updated.

***************************************************************************/
bool ACTR::_FDoAevCur(void)
{
	AssertBaseThis(0);

	if (!_FDoAevCore(_iaevCur))
		return fFalse;

	_iaevCur++;
	Assert(_iaevCur <= _pggaev->IvMac(), "_iaevCur bug");

	return fTrue;
}


/***************************************************************************
	
	Execute the event iaev.

***************************************************************************/
bool ACTR::_FDoAevCore(long iaev)
{
	AssertBaseThis(0);
	AssertIn(iaev, 0, _pggaev->IvMac());

	AEV aev;
	COST cost;

	_pggaev->GetFixed(iaev, &aev);
	if (aev.nfrm != _nfrmCur)
		{
		aev.nfrm = _nfrmCur;
		_pggaev->PutFixed(iaev, &aev);
		}
	AssertIn(aev.aet, 0, aetLim);

	switch(aev.aet)
		{
	case aetActn:
		AEVACTN aevactn;
		ulong grfactn;

		_pggaev->Get(iaev, &aevactn);

		//Empty the motion match sound list	if the action <changed>
		if (aevactn.anid != _anidCur)
			_pglsmm->FSetIvMac(0);

		if (!_ptmpl->FSetActnCel(_pbody, aevactn.anid, aevactn.celn))
			{
			return fFalse;
			}
		_celnCur = aevactn.celn;
		_anidCur = aevactn.anid;
		if (!_ptmpl->FGetCcelActn(_anidCur, &_ccelCur))	 	// Cache the cel count
			return fFalse;
		_iaevActnCur = iaev;

		// Force the location to the step event
		// Avoids incorrect event ordering on static segments
		_rtelCur = aev.rtel;
		_GetXyzFromRtel(&_rtelCur, &_xyzCur);
		if (!_ptmpl->FGetGrfactn(_anidCur, &grfactn))
			{
			return fFalse;
			}
		_grfactn = grfactn;
		break;

	case aetAdd:
		AEVADD aevadd;
		RPT rpt;
		// Save old costume in case of error
		if (!cost.FGet(_pbody))
			return fFalse;
		// Invoke the default costume/orientation
		if (!_ptmpl->FSetDefaultCost(_pbody))
			return fFalse;
		// Put the actor on stage.	Set up models.
		if (!_ptmpl->FSetActnCel(_pbody, _anidCur, _celnCur))
			{
			cost.Set(_pbody); // restore old costume
			return fFalse;
			}

		//Empty the motion match sound list
		_pglsmm->FSetIvMac(0);

		// Set the translation for the subroute
		_pggaev->Get(iaev, &aevadd);
		_dxyzSubRte.dxr = aevadd.dxr;
		_dxyzSubRte.dyr = aevadd.dyr;
		_dxyzSubRte.dzr = aevadd.dzr;
		_UpdateXyzRte();

		// Load the initial orientation
		_InitXfrm();
		_LoadAddOrien(&aevadd);
		
		// Set state variables
		_iaevFrmMin = _iaevAddCur = iaev;	
		_rtelCur = aev.rtel;
		_GetXyzFromRtel(&_rtelCur, &_xyzCur);
		_pglrpt->Get(aev.rtel.irpt, &rpt);
		
		// Show the actor
		if (!_fOnStage)
			_pbody->Show();
		_fOnStage = fTrue;
		break;

	case aetRem:
		// Exit the actor from the stage
		_Hide();
		break;

	case aetCost:
		AEVCOST aevcost;
		_pggaev->Get(iaev, &aevcost);
		if (aevcost.fCmtl)
			{
			PCMTL pcmtl = _ptmpl->PcmtlFetch(aevcost.cmid);
			if (pvNil == pcmtl)
				return fFalse;
			_pbody->SetPartSetCmtl(pcmtl);
			ReleasePpo(&pcmtl);
			}
		else
			{
			PMTRL pmtrl;
			pmtrl = (PMTRL)vptagm->PbacoFetch(&aevcost.tag, MTRL::FReadMtrl);
			if (pvNil == pmtrl)
				return fFalse;
			_pbody->SetPartSetMtrl(aevcost.ibset, pmtrl);
			ReleasePpo(&pmtrl);
			}
		break;

	case aetRotF:
		// Actors are xformed in _FDoFrm, Rotate or Scale
		_pggaev->Get(iaev, &_xfrm.bmat34Fwd);
		_fUseBmat34Cur = fFalse;
		break;

	case aetRotH:
		// Actors are xformed in _FDoFrm, Rotate or Scale
		_pggaev->Get(iaev, &_xfrm.bmat34Cur);
		_fUseBmat34Cur = fTrue;
		break;

	case aetPull:
		// Actors are xformed in _FDoFrm, Rotate or Scale
		_pggaev->Get(iaev, &_xfrm.aevpull);
		break;

	case aetSize:
		// Actors are xformed in _FDoFrm, Rotate or Scale
		_pggaev->Get(iaev, &_xfrm.rScaleStep);
		break;

	case aetStep:		//Exists for timing control (eg walk in place)
		_pggaev->Get(iaev, &_dwrStep);
		// Force the location to the step event
		// Avoids incorrect event ordering on static segments
		if (rZero == _dwrStep)
			{
			_rtelCur = aev.rtel;
			_GetXyzFromRtel(&_rtelCur, &_xyzCur);
			}
		break;

	case aetFreeze:
		long fFrozen;  	//_fFrozen is a bit
		_pggaev->Get(iaev, &fFrozen);
		_fFrozen = FPure(fFrozen);
		break;

	case aetTweak:
		if (aev.rtel.dnfrm == _rtelCur.dnfrm)
			_pggaev->Get(iaev, &_xyzCur);
		// The actual locating of the actor is done in _FDoFrm or FTweakRoute
		break;

	case aetSnd:
		// Enqueue non-mm sounds
		if (!_FEnqueueSnd(iaev))		// Ignore failure
			return fFalse;		
		break;

	case aetMove:
		XYZ dxyz;
		_pggaev->Get(iaev, &dxyz);
		_dxyzSubRte.dxr = BrsAdd(dxyz.dxr, _dxyzSubRte.dxr);
		_dxyzSubRte.dyr = BrsAdd(dxyz.dyr, _dxyzSubRte.dyr);
		_dxyzSubRte.dzr = BrsAdd(dxyz.dzr, _dxyzSubRte.dzr);
		_UpdateXyzRte();
		break;

	default:
		Bug("Unimplemented actor event");
		}

	return fTrue;
}


/***************************************************************************
	
	Add the specified event to the event list at the current frame, and
	execute the event

	NOTE:  Overwrites events of the same type if appropriate
	NOTE:  If the new action event is the same as the most recent
	previous action, the new action is still inserted.	This avoids
	repositioning the actor based on a past Set Action.

***************************************************************************/
bool ACTR::_FAddDoAev(long aetNew, long cbNew, void *pvVar)
{
	AssertBaseThis(0);
	AssertIn(aetNew, 0, aetLim);
	AssertIn(cbNew, 0, 100);	//approximate upper bound
	AssertPvCb(pvVar, cbNew);
	Assert(_fOnStage || aetNew == aetAdd, 
		"Error!  Beginning subroute with no Add Onstage event");

	AEV aev;
	long iaevNew;
	// Setup fixed part of the gg
	aev.aet = aetNew;
	aev.rtel = _rtelCur;
	aev.nfrm = _nfrmCur;
	
	if (!_FInsertAev(_iaevCur, cbNew, pvVar, &aev))
		return fFalse;
	_MergeAev(_iaevFrmMin, _iaevCur, &iaevNew);

#ifdef BUG1870
// REVIEW *****: V2.0
//		Though the only situation in which this arises is aetMove,
//		it might be that the FDoAevCore() should be called before
//		merging events (so that any cumulative changes get executed
//		only once, without requiring this special casing of aetMove.
	if (aetMove == aetNew)
		{
		// Skip the "do" of the FAddDoAev().
		// The 'Do' part currently merges events before 
		// executing them, which in this case would cause
		// any existing same frame translaton to be added 
		// to the state	variables twice.
		// Instead, adjust state var translation here.
		_dxyzSubRte.dxr = BrsAdd(((XYZ *)pvVar)->dxr, _dxyzSubRte.dxr);
		_dxyzSubRte.dyr = BrsAdd(((XYZ *)pvVar)->dyr, _dxyzSubRte.dyr);
		_dxyzSubRte.dzr = BrsAdd(((XYZ *)pvVar)->dzr, _dxyzSubRte.dzr);
		}
	else if (!_FDoAevCore(iaevNew))
		return fFalse;
#else //!BUG1870
	if (!_FDoAevCore(iaevNew))
		return fFalse;
#endif //!BUG1870

	if (_iaevCur == iaevNew)
		_iaevCur++;
	
	_pscen->MarkDirty();

	Assert(aetNew != aetActn || _iaevActnCur == iaevNew,
		"_iaevActnCur not up to date");
	AssertIn(_iaevCur, 0, _pggaev->IvMac() + 1);
	Assert(!(_pggaev->IvMac() == 1 && _nfrmFirst != _nfrmCur), "check case");

	if (_nfrmCur < _nfrmFirst)
		{
		_pscen->InvalFrmRange();
		_nfrmFirst = _nfrmCur;
		}

	if (_nfrmCur > _nfrmLast)
		{
		_pscen->InvalFrmRange();
		_nfrmLast = _nfrmCur;
		}

   	AssertThis(fobjAssertFull);
	return fTrue;
}


/***************************************************************************
	
	Merge event iaevNew among preceding events, beginning with
	event iaevFirst (which is usually the first event in the frame).
	Unless equal to pvNil, Return *piaevRtn to be the final index of the
	merged event
	NOTE:  Add events (only) are not merged.
	
***************************************************************************/
void ACTR::_MergeAev(long iaevFirst, long iaevNew, long *piaevRtn)
{
	AssertBaseThis(0);
	AssertIn(iaevFirst, 0, _pggaev->IvMac());
	AssertIn(iaevNew, iaevFirst, _pggaev->IvMac());
	AssertNilOrVarMem(piaevRtn);

	AEV aev;
	AEV aevNew;
	void *pvVar;
	long cbVar;
	long iaev;
	BMAT34 bmat34;

	if ((_pggaev->IvMac() < iaevFirst) || (iaevFirst == iaevNew))
		{
		if (pvNil != piaevRtn)
			*piaevRtn = iaevNew;
		return;
		}

	_pggaev->GetFixed(iaevNew, &aevNew);

	//
	// Check if Aev is in the list already
	//
	for (iaev = iaevFirst; iaev < iaevNew; iaev++)
		{
		_pggaev->GetFixed(iaev,	&aev);

		if ((aev.aet != aevNew.aet) && (aev.aet != aetRem) && (aev.aet != aetRotH))
			continue;

		switch(aev.aet)
			{
		case aetRem:
			if (aevNew.aet == aetAdd && aevNew.nfrm != aev.nfrm)
				{
				_RemoveAev(iaev);
				iaevNew--;
				iaev--;
				}
			break;

		case aetAdd:
			// We cannot remove events associated with a same-frame Add event
			// or we will not retain costume etc information to propogate
			// forward (eg drag single pt actor offstage & roll call back)
			continue;
			break;
		
		case aetMove:	// Move is accumulation of previous moves
			XYZ dxyz;
			XYZ dxyzNew;
			_pggaev->Get(iaev, &dxyz);
			_pggaev->Get(iaevNew, &dxyzNew);
			// Note: Merging moves should not alter state variables!
#ifndef BUG1870
			// Remove these lines of code
			_dxyzSubRte.dxr = BrsSub(_dxyzSubRte.dxr, dxyz.dxr);
			_dxyzSubRte.dyr = BrsSub(_dxyzSubRte.dyr, dxyz.dyr);
			_dxyzSubRte.dzr = BrsSub(_dxyzSubRte.dzr, dxyz.dzr);
#endif //!BUG1870
			dxyz.dxr = BrsAdd(dxyz.dxr, dxyzNew.dxr);
			dxyz.dyr = BrsAdd(dxyz.dyr, dxyzNew.dyr);
			dxyz.dzr = BrsAdd(dxyz.dzr, dxyzNew.dzr);
			_pggaev->Put(iaev, &dxyz);
			goto LDeleteNew;
			break;

		case aetCost:
			AEVCOST aevcost;
			AEVCOST aevcostNew;

			// Check that the body parts match
			_pggaev->Get(iaev, &aevcost);
			_pggaev->Get(iaevNew, &aevcostNew);

			if (aevcost.ibset != aevcostNew.ibset)
				continue;

			if (aev.rtel != aevNew.rtel)
				goto LDeleteOld;
			_pggaev->Put(iaev, &aevcostNew);
			goto LDeleteNew;
			break;

		case aetSnd:
			AEVSND aevsnd;
			AEVSND aevsndNew;
			long ismm;
			SMM *psmm;
			// Check that the sound types match
			_pggaev->Get(iaev, &aevsnd);
			_pggaev->Get(iaevNew, &aevsndNew);
			if (MSND::SqnActr(aevsnd.sty, _arid) != MSND::SqnActr(aevsndNew.sty, _arid))
				continue;
			// Queued sounds need to have multiple events reside in a single frame
			if (aevsndNew.fQueue)
				continue;
			// Non queued sounds need to replace queued sounds of the same type
			// First, the _pggsmm needs to be updated
			for (ismm = 0; ismm < _pglsmm->IvMac(); ismm++)
				{
				psmm = (SMM *)_pglsmm->QvGet(ismm);
				if (psmm->aevsnd.sty == aevsnd.sty && aevsnd.celn == psmm->aevsnd.celn)
					{
					_pglsmm->Delete(ismm);
					break;
					}
				}
			_RemoveAev(iaev);
			iaevNew--;
			iaev--;
			continue;
			break;

		case aetRotH:
			if (aevNew.aet == aetRotF && aevNew.nfrm == aev.nfrm)
				{
				// Forward rotations must get rid of tweak rotations in the current frame
				_RemoveAev(iaev);
				iaevNew--;
				iaev--;
				}
			if (aevNew.aet != aetRotH)
				continue;
			if (aev.rtel != aevNew.rtel)
				goto LDeleteOld;
			_pggaev->Get(iaevNew, &bmat34);
			_pggaev->Put(iaev, &bmat34);
			goto LDeleteNew;
			break;

		case aetRotF:
			// New == old == forward-rotate
			// Need to replace the old rotation, but continue on to remove tweak-rotations
			_RemoveAev(iaev);
			iaevNew--;
			iaev--;
			continue;
			break;

		default:
			if (aev.rtel != aevNew.rtel)
				{
				goto LDeleteOld;
				}
			pvVar = _pggaev->QvGet(iaevNew, &cbVar);
			Assert(cbVar == _pggaev->Cb(iaev), "Wrong Var size");
			_pggaev->Put(iaev, pvVar);
			goto LDeleteNew;
			}
		}

    //
	// Leave it where it is.  No match found.
	//
	if (pvNil != piaevRtn)
		*piaevRtn = iaevNew;
		
	return;

LDeleteNew:
	if (pvNil != piaevRtn)
		{
		*piaevRtn = iaev;
		AssertIn(iaev, iaevFirst, iaevNew);
		}
	_RemoveAev(iaevNew);
	return;

LDeleteOld:
	_RemoveAev(iaev);
	iaevNew--;
	if (pvNil != piaevRtn)
		*piaevRtn = iaevNew;
}


/***************************************************************************
	
	Add (or replace) an action
	Add the event to the event list

***************************************************************************/
bool ACTR::FSetActionCore(long anid, long celn, bool fFreeze)
{
	AssertThis(0);
	AssertIn(anid, 0, klwMax);
	AssertIn(celn, klwMin, klwMax);

	bool fStatic;
	bool fNewAction = fFalse;
	AEVACTN aevactn;
	long anidPrev = _anidCur;
	long iaevMin;

	long cbVar = kcbVarActn + kcbVarFreeze + (kcbVarStep * 2);
	if (!_pggaev->FEnsureSpace( 4, cbVar, fgrpNil))
		return fFalse;

	if (!_FGetStatic(anid, &fStatic))
		return fFalse;
	
	// If the action is changing:
	// Remove all motion match sounds of the previous action
	// Query the template and insert new sound events
	if (_anidCur != anid)
		{
		fNewAction = fTrue;
		if (!_FRemoveAevMm(anid))
			return fFalse;
		}

	// var part of gg
	aevactn.anid = anid;	
	aevactn.celn = celn;
	
	// Add this action to the event list
	iaevMin = (_iaevFrmMin == _iaevAddCur) ? _iaevFrmMin + 1 : _iaevFrmMin;
	_PrepActnFill(iaevMin, _anidCur, anid,
		faetTweak | faetFreeze | faetActn);

	if (!_FAddDoAev(aetActn, kcbVarActn, &aevactn))
		return fFalse;

	// If the action is changing:
	// Query the template and insert new default motion match sound events
	if (fNewAction)
		{
		if (!_FAddAevDefMm(anid))
			return fFalse;
		}

	if (_nfrmCur != _nfrmLast)
		{
		_fLifeDirty = fTrue;
		_pscen->InvalFrmRange();
		}

	if (!fFreeze && !_FIsDoneAevSub(_iaevCur, _rtelCur))
		{
		if (!_FUnfreeze())
			return fFalse;
		}
	else
		{
		if (!_FFreeze())
			return fFalse;
		}

	// If the actor is in the middle of a static segment, the application of a
	// non-static action is supposed to make the actor start moving forward along
	// the remaining path.
	// The last frame in the subpath must retain its final step=0 event, however.
	if (!fStatic && !_ptmpl->FIsTdt() && !_FIsDoneAevSub(_iaevCur, _rtelCur))
		{
		if (!FSetStep(kdwrNil))
			return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	
	Remove an actor from the stage
	NOTE: This is a low level API and has no effect on the event list

***************************************************************************/
void ACTR::_Hide(void)
{
	AssertBaseThis(0);

	if (_fOnStage && _pbody != pvNil)
		_pbody->Hide();
	_fOnStage = fFalse;
	return;
}


/***************************************************************************
	
	Insert a node point *prpt in the route.
	Update *prpt.dwr, and the previous node's dwr.
	On input:
		*prpt.dwr == 0 iff the new node is a subpath terminating node
		*prpt.xyz has the new node coordinates
		dwrPrior is the distance from the previous node to the new one.

***************************************************************************/
bool ACTR::_FInsertGgRpt(long irpt, RPT *prpt, BRS dwrPrior)
{
	AssertBaseThis(0);
	AssertIn(irpt, 0, _pglrpt->IvMac() + 1);
	AssertNilOrVarMem(prpt);

	RPT rpt;
	BRS dwrTotal = rZero;  // Dist from prev node to node after the inserted node

	if (!_pglrpt->FInsert(irpt, prpt))
		return fFalse;

	if (0 < irpt)
		{
		_pglrpt->Get(irpt-1, &rpt);
		dwrTotal = rpt.dwr;	   	

		// Do not alter end of route dwr's
		if (rZero != rpt.dwr)
			{
			// Adjust the distance from the previous point here
			rpt.dwr = dwrPrior;
			if (rZero == rpt.dwr)
				rpt.dwr = rEps;	  	//Epsilon.  Prevent pathological incorrect end-of-path
			_pglrpt->Put(irpt-1, &rpt);
			}
		else Assert(dwrPrior == rZero, "Illegal distance");
		}

	if (irpt < _pglrpt->IvMac() - 1)
		{
		_pglrpt->Get(irpt+1, &rpt);
		if (rZero != prpt->dwr)	 	// If not at end of subroute
			{
			// Set the distance to the next point in this subroute
			Assert(rZero != dwrTotal, "Overwriting end of route");
			prpt->dwr = BrsSub(dwrTotal, dwrPrior);
			if (rZero >= prpt->dwr)
				prpt->dwr = rEps;	//Epsilon.  Prevent pathological incorrect end-of-path
			_pglrpt->Put(irpt, prpt);
			}
		}
	else Assert(rZero == prpt->dwr, "Invalid end sub-node dwr");

	return fTrue;
}


/***************************************************************************
	
	Set a Stepsize Event
	Add it to the event list

	dwrStep == 0 -> Stop the actor from forward motion.
	dwrStep == kdwrNil -> Use the scaled template step size
	Note that this is orthogonal and independent of freezing an actor

***************************************************************************/
bool ACTR::FSetStep(BRS dwrStep)
{
	AssertThis(0);
	Assert(kdwrNil == dwrStep || rZero <= dwrStep, "Invalid dwrStep argument");

	if (!_pggaev->FEnsureSpace( 1, kcbVarStep, fgrpNil))
		return fFalse;

	return _FAddDoAev(aetStep, kcbVarStep, &dwrStep);
}


/***************************************************************************
	
	Get the new origin for the to-be positioned actor.  Always place
	actors on the "floor" (Y = 0)...except 3-D Text actors, which are
	at Y = 1.0 meters.

***************************************************************************/
void ACTR::_GetNewOrigin(BRS *pxr, BRS *pyr, BRS *pzr)
{
	AssertThis(0);
	AssertVarMem(pxr);
	AssertVarMem(pyr);
	AssertVarMem(pzr);

	BRS xrCam = rZero;
	BRS yrCam = rZero;
	BRS zrCam = kzrDefault;
	BRS zrYon, zrHither;
	BMAT34 bmat34Cam;

	_pscen->Pbkgd()->GetActorPlacePoint(pxr, pyr, pzr);
	// A (0, 0, 0) place point means that one hasn't been authored yet,
	// so use the old system.
	if (*pxr == rZero && *pyr == rZero && *pzr == rZero)
		{	
		_pscen->Pmvie()->Pbwld()->GetCamera(&bmat34Cam, &zrHither, &zrYon);

		Assert(zrCam < 0, "Non-negative placement value");
		Assert(zrHither >= 0 && zrYon >= 0, "Negative camera values");
		if (BR_ABS(zrCam) > zrYon)
			zrCam = BR_CONST_DIV(BR_ADD(zrYon, zrHither), -2);

		*pxr = BR_MAC3(xrCam, bmat34Cam.m[0][0], yrCam, bmat34Cam.m[1][0],
			zrCam, bmat34Cam.m[2][0]) + bmat34Cam.m[3][0];
		*pyr = rZero;
		*pzr = BR_MAC3(xrCam, bmat34Cam.m[0][2], yrCam, bmat34Cam.m[1][2],
			zrCam, bmat34Cam.m[2][2]) + bmat34Cam.m[3][2];
		}
	if (_ptmpl->FIsTdt())
		*pyr += BR_SCALAR(10.0); // 1.0 meters
}


/***************************************************************************
	
	Add actor on the stage - ie, create a new subroute.
	Add the Add event to the event list
	Nukes existing subroute if obsoleted by current Add.

	Add the initialization events for costumes, xforms, etc from the
	preceding or subsequent subroute.
***************************************************************************/
bool ACTR::FAddOnStageCore(void)
{
	AssertThis(0);

	BRS xr;
	BRS yr;
	BRS zr;
	long cbVar;
	AEVADD aevadd;
	bool fUpdateFrmRange = fFalse;

	cbVar = kcbVarAdd + kcbVarActn + kcbVarStep + kcbVarFreeze;
	if (_pggaev->IvMac() == 0)
		{
		if (!_pggaev->FEnsureSpace( 4, cbVar, fgrpNil) ||
			!_pglrpt->FEnsureSpace(1, fgrpNil))
			return fFalse;
		}
	else
		{
		cbVar += kcbVarCost + kcbVarRot	+ kcbVarSize + kcbVarPull;
		if (!_pggaev->FEnsureSpace( 8, cbVar, fgrpNil) ||
			!_pglrpt->FEnsureSpace(1, fgrpNil))
			{
			return fFalse;
			}
		}

	_GetNewOrigin(&xr, &yr, &zr);
	aevadd.dxr = BrsSub(xr, _dxyzFullRte.dxr);
	aevadd.dyr = BrsSub(yr, _dxyzFullRte.dyr);
	aevadd.dzr = BrsSub(zr, _dxyzFullRte.dzr);
	aevadd.xa = aevadd.za = aZero;

	// Rotate the actor to be facing the camera
	// NOTE: 3D spletter code uses this also.
	aevadd.ya = _pscen->Pbkgd()->BraRotYCamera();
	
	if (_nfrmCur < _nfrmFirst)
		{
		fUpdateFrmRange = fTrue;
		_nfrmFirst = _nfrmCur;
		}
	if (_nfrmCur > _nfrmLast)
		{
		fUpdateFrmRange = fTrue;
		_nfrmLast = _nfrmCur;
		}

	RPT rptNil = {rZero, rZero, rZero, rZero};
	
	if (_fOnStage)		// May have walked offstage
		{
		// Delete the remnant subroute (non-inclusive of current frame)
		_DeleteFwdCore(fFalse);	 		
		}

	_rtelCur.dnfrm = 0;
	_rtelCur.dwrOffset = rZero;
	if (_iaevAddCur < 0)
		_rtelCur.irpt = 0;
	else
		_rtelCur.irpt++;
			
	AssertDo(_FInsertGgRpt(_rtelCur.irpt, &rptNil), "Logic error");
	_AdjustAevForRteIns(_rtelCur.irpt, 0);

	_GetXyzFromRtel(&_rtelCur, &_xyzCur);

	if (!_FAddDoAev(aetAdd, kcbVarAdd, &aevadd))
		return fFalse;

				
	// Copy costume and transform events forward if this is
	// the earliest Add event : each subroute needs all
	// initialization events.
	// Note: _iaevCur is already incremented at this point
	if (1 == _iaevCur)
		{
		// The earliest Add.  Gather later events and insert them
		if (!_FAddAevFromLater())
			return fFalse;		
		}
	else
		{																							
		// Not the earliest Add.  Gather earlier events	and insert them
		ulong grfaet = faetActn | faetCost | faetPull | faetSize | faetRotF;
		if (!_FAddAevFromPrev(_iaevCur - 1, grfaet))
			return fFalse;
		}

	_PositionBody(&_xyzCur);
	
	if (!FSetStep(rZero))
		return fFalse;
	if (!_FFreeze())
		return fFalse;
		
	if (fUpdateFrmRange)
		{
		_pscen->InvalFrmRange();
		}
	
	return fTrue;
}

/***************************************************************************
	
	Collect events < iaevLim & insert them beginning at event number
	_iaevCur.
	NOTE:  The primary complication is to OMIT copying events which can
	never be reached due to static actions (stalling) in the previous
	subroute.  Due to initialization now spec'd to occur in every subroute,
	note that only the immediately preceding subroute is of concern.
	NOTE: This is optimized to add only the latest event of each given type.

***************************************************************************/
bool ACTR::_FAddAevFromPrev(long iaevLim, ulong grfaet)
{
	AssertBaseThis(0);
	AssertIn(iaevLim, 0, _pggaev->IvMac() + 1);
		
	AEV aev;
	AEV *paev;
	AEV aevCur;
	long cb;
	long iaev;
	long iaevAdd;
	long iaevNew;
	long iaevLast;
	bool fPrunedPrevSubrte = fFalse;

	// Locate the next active (not stalled) region of the subroute
	// Note: Not finding a previous aev is not a failure
	_FFindPrevAevAet(aetAdd, iaevLim, &iaevAdd);
	_FindAevLastSub(iaevAdd, iaevLim, &iaevLast);

	_pggaev->GetFixed(_iaevCur - 1, &aevCur);
	
	// It is more efficient to insert non-costume events backwards
	for (iaev = iaevLast; iaev > iaevAdd; iaev--)
		{
		if (0 == grfaet)
			break;

		Assert(iaev >= 0, "Logic error");
		_pggaev->GetFixed(iaev, &aev);
		if (!(grfaet & (1 << aev.aet)) || aev.aet == aetCost)
			continue;

		// Non-costume events are added only once
		grfaet ^= (1 << aev.aet);

		// Allocate space
		cb = _pggaev->Cb(iaev);
		aev.rtel = aevCur.rtel;
		// aev.nfrm is set by _FDoAevCore()

		if (!_FInsertAev(_iaevCur, cb, pvNil, &aev))	
			return fFalse;

		//Insert event
		_pggaev->Put(_iaevCur, _pggaev->QvGet(iaev));

		// Merge events to avoid duplicates
		_MergeAev(_iaevFrmMin, _iaevCur, &iaevNew);
		
		if (!_FDoAevCore(iaevNew))
			return fFalse;

		if (iaevNew == _iaevCur)
			_iaevCur++;
		}

	if (!(grfaet & (1 << aetCost)))
		return fTrue;

	// Costumes needed to be gathered forward
	for (iaev = iaevAdd + 1; iaev <= iaevLast; iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.aet != aetCost)
			continue;

		aev.rtel = aevCur.rtel;
		// aev.nfrm is set by _FDoAevCore()

		// Allocate space
		cb = _pggaev->Cb(iaev);
		if (!_FInsertAev(_iaevCur, cb, pvNil, &aev))	
			return fFalse;

		//Insert event
		_pggaev->Put(_iaevCur, _pggaev->QvGet(iaev));

		// Merge events to avoid duplicates
		_MergeAev(_iaevFrmMin, _iaevCur, &iaevNew);
		
		if (!_FDoAevCore(iaevNew))
			return fFalse;

		if (iaevNew == _iaevCur)
			_iaevCur++;
		}
	
	// Remove redundant (same frame) events from previous Add
	if (iaevAdd < 0)
		return fTrue;
	
	// May need to delete entire previous subroute
	// if the prev Add occurred at the same frame	
	paev = (AEV *)_pggaev->QvFixedGet(iaevAdd);
	if (_nfrmCur == paev->nfrm)
		{
		// Deleting the entire subroute
		_DelAddFrame(iaevAdd, _iaevAddCur);
		return fTrue;
		}

	// May need to prune out same-frame events from prev subrte
	for (iaev = _iaevAddCur - 1; iaev >= iaevAdd; iaev--)
		{
		paev = (AEV *)_pggaev->QvFixedGet(iaev);
		if (_nfrmCur == paev->nfrm)
			{
			fPrunedPrevSubrte = fTrue;
			_RemoveAev(iaev);
			}
		}
	
	if (fPrunedPrevSubrte)
		{
		// The previous path requires pruning
		// Terminating stop & freeze events need to be inserted
		long nfrm = _nfrmCur;
		if (!FGotoFrame(_nfrmCur - 1))
			return fFalse;
		DeleteFwdCore(fFalse, pvNil, _iaevCur);
		return FGotoFrame(nfrm);
		}

	return fTrue;
}


/***************************************************************************
	
	Gather the initialization events of the subsequent subroute and insert
	them at the	current event index, _iaevCur.

***************************************************************************/
bool ACTR::_FAddAevFromLater(void)
{
	AssertBaseThis(0);

	AEV aev;
	long iaev;
	long iaevStart = -1;
	RTEL rtelAdd;
	bool fPositionBody = fFalse;

	Assert(1 == _iaevCur, "_FAddAevFromLater logic error");
	// Find the next Add event
	for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.aet == aetAdd)
			{
			iaevStart = iaev;
			rtelAdd = aev.rtel;
			break;
			}
		}

	if (iaevStart < 0)
		goto LEnd;

	// Stuff in the events from this next Add event
	for (iaev = iaevStart; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		
		if (aev.rtel != rtelAdd)
			{
			break;
			}

		if (aetCost == aev.aet || aetPull == aev.aet ||
			aetSize == aev.aet || aetRotF == aev.aet)
			{
			aev.rtel = _rtelCur;
			// aev.nfrm is set by _FDoAevCore()

			// Allocate space
			long cbNew = _pggaev->Cb(iaev);
			if (!_FInsertAev(_iaevCur, cbNew, pvNil, &aev))	
				return fFalse;
			iaev++;

			//Insert event
			_pggaev->Put(_iaevCur, _pggaev->QvGet(iaev));
			
			if (!_FDoAevCur())
				return fFalse;

			if (aev.aet != aetCost)	
				fPositionBody = fTrue;			
			}
		}
	
	if (fPositionBody)
		{
		_PositionBody(&_xyzCur);
		}	

	Assert(_pggaev->IvMac() > 0, "Logic Error");

LEnd:	
	return FSetActionCore(0, 0, 0);
}



/***************************************************************************
	
	Locate the event of type aet with index closest to but smaller than iaevCur
	Return true if found, with its index in *piaevAdd

***************************************************************************/
bool ACTR::_FFindPrevAevAet(long aet, long iaevCur, long *piaevAdd)
{
	AssertBaseThis(0);
	AssertIn(aet, 0, aetLim);
	AssertIn(iaevCur, 0, _pggaev->IvMac());
	AssertVarMem(piaevAdd);
	
	AEV aev;

	iaevCur--;
	for (; iaevCur >= 0; iaevCur--)
		{
		_pggaev->GetFixed(iaevCur, &aev);
		if (aev.aet == aet)
			{
			*piaevAdd = iaevCur;
			return fTrue;
			}
		}

	*piaevAdd = ivNil;
	return fFalse;
}


/***************************************************************************
	
	Locate the event of type aet with index closest to but >= iaevCur.
	Return true if one is found, with its index in *piaevAdd
	
***************************************************************************/
bool ACTR::_FFindNextAevAet(long aet, long iaevCur, long *piaevAdd)
{
	AssertBaseThis(0);
	AssertIn(aet, 0, aetLim);
	AssertIn(iaevCur, 0, _pggaev->IvMac() + 1);
	AssertVarMem(piaevAdd);
	
	AEV *paev;

	for (; iaevCur < _pggaev->IvMac(); iaevCur++)
		{
		paev = (AEV *)_pggaev->QvFixedGet(iaevCur);
		if (paev->aet == aet)
			{
			*piaevAdd = iaevCur;
			return fTrue;
			}
		}

	*piaevAdd = ivNil;
	return fFalse;
}


/***************************************************************************
	
	Locate the last reachable event in the specified subroute
	Note: This is complicated by static actions.
	Input:
		Begin the search at iaevAdd.
		The limit of the search is iaevLim, which, if == ivNil, is
		the extent of the ggaev.
	Return its index in *piaevAdd

***************************************************************************/
void ACTR::_FindAevLastSub(long iaevAdd, long iaevLim, long *piaevLast)
{
	AssertBaseThis(0);
	AssertIn(iaevAdd, 0, _pggaev->IvMac());
	if (iaevLim != ivNil)
		AssertIn(iaevLim, 0, _pggaev->IvMac() + 1);
	AssertVarMem(piaevLast);

	AEV aev;
	
	if (iaevLim == ivNil)
		iaevLim = _pggaev->IvMac();

	_pggaev->GetFixed(iaevLim - 1, &aev);
	
	if (_FIsStalled(iaevAdd, &aev.rtel, piaevLast))
		{
		// Last active event stored by _FIsStalled
		return;
		}

	*piaevLast = iaevLim - 1;
}


/***************************************************************************
	
	Remove the actor from the stage
	Add the event to the event list

***************************************************************************/
bool ACTR::FRemFromStageCore(void)
{
	AssertThis(0);
	AEV *paev;

	if (!_fOnStage)
		return fTrue;

	if (!_pggaev->FEnsureSpace( 1, kcbVarStep, fgrpNil))  //step
		return fFalse;

	AssertIn(_iaevAddCur, 0, _pggaev->IvMac());
	paev = (AEV *)_pggaev->QvFixedGet(_iaevAddCur);
	if (_nfrmCur == paev->nfrm)
		{
		if (!_FDeleteEntireSubrte())
			return fFalse;
		}

	if (_fOnStage)
		return _FAddDoAev(aetRem, kcbVarZero, pvNil);
	return fTrue;
}


/***************************************************************************
	
	Delete the current entire subroute & reposition actor accordingly.
	Based on current state variables

***************************************************************************/
bool ACTR::_FDeleteEntireSubrte(void)
{
	AssertThis(0);
	AEV *paev;
	long nfrmSav;
	
	paev = (AEV *)_pggaev->QvFixedGet(_iaevAddCur);
	Assert(paev->nfrm == _nfrmCur,
		"Logic error: trying to delete whole route from the middle");

	// Delete forward from here	(exclusive of current point)
	_DeleteFwdCore(fFalse, pvNil, _iaevCur);

	// Delete events & path for the sole remaining Add frame
	_DelAddFrame(_iaevAddCur, _iaevCur);

	// A hide should be done separately from event execution
	// because no subroute exists in this case
	_Hide();
	if (_ptmpl != pvNil && _pbody != pvNil)
		_ptmpl->FSetDefaultCost(_pbody);
	_SetStateRewound();
	nfrmSav = _nfrmCur;	
	_nfrmCur = knfrmInvalid;
	if (_pggaev->IvMac() > 0)
		{
		paev = (AEV *)_pggaev->QvFixedGet(0);
		Assert(paev->aet == aetAdd, "Corrupt event list");
		_nfrmFirst = paev->nfrm;
		return FGotoFrame(nfrmSav);	
		}
	_nfrmFirst = knfrmInvalid;
	return fTrue;
}


/***************************************************************************
	
	Delete the events and point corresponding to a single point subroute

***************************************************************************/
void ACTR::_DelAddFrame(long iaevAdd, long iaevLim)
{
	AssertThis(0);
	long iaev;
	long irptAdd;
	AEV *paev;

	AssertIn(iaevAdd, 0, iaevLim);

	paev = (AEV *)_pggaev->QvFixedGet(iaevAdd);
	irptAdd = paev->rtel.irpt;

	// Delete events in current frame of subroute
	for (iaev = iaevLim - 1; iaev >= iaevAdd; iaev--)
		{
		_RemoveAev(iaev);
		}
	
	// Delete current point	if unused
	if (iaevAdd > 0)
		{
		paev = (AEV *)_pggaev->QvFixedGet(iaevAdd - 1);
		if (paev->rtel.irpt == irptAdd ||
			(paev->rtel.irpt == (irptAdd - 1) && paev->rtel.dwrOffset > rZero))
			{
			return;
			}
		}
	_AdjustAevForRteDel(irptAdd, iaevAdd);
	_pglrpt->Delete(irptAdd);
	if (_rtelCur.irpt > irptAdd)
		_rtelCur.irpt--;

	return;
}


/***************************************************************************
	
	Set the Costume for a body part
	Add the event to the event list

***************************************************************************/
bool ACTR::FSetCostumeCore(long ibsetClicked, TAG *ptag, long cmid,
	bool fCmtl)
{
	AssertThis(0);
	Assert(fCmtl || ibsetClicked >= 0, "Invalid ibsetClicked argument");
	AssertVarMem(ptag);

	AEVCOST aevcost;
	PCMTL pcmtl;
	long ibsetApply;

	// For custom materials, the ibset is a property of the CMTL itself,
	// so read it from the CMTL rather than using the clicked ibset.
	if (fCmtl)
		{
		pcmtl = _ptmpl->PcmtlFetch(cmid);
		if (pvNil == pcmtl)
			return fFalse;
		ibsetApply = pcmtl->Ibset();
		ReleasePpo(&pcmtl);
		}
	else
		{
		ibsetApply = ibsetClicked;
		}

	aevcost.ibset = ibsetApply;
	aevcost.tag = *ptag;
	aevcost.cmid = cmid;
	aevcost.fCmtl = fCmtl;

	if (!_pggaev->FEnsureSpace( 1, kcbVarCost, fgrpNil))
		return fFalse;

	_PrepCostFill(_iaevCur, &aevcost);
	if (!_FAddDoAev(aetCost, kcbVarCost, &aevcost))
		return fFalse;

	return fTrue;
}


/***************************************************************************
	
	Freeze the actor (don't advance cels)
	Add the event to the event list

***************************************************************************/
bool ACTR::_FFreeze(void)
{
	AssertThis(0);

	long faevfrz = (long)fTrue;
	return _FAddDoAev(aetFreeze, kcbVarFreeze, &faevfrz);
}


/***************************************************************************
	
	Unfreeze the actor
	Add the event to the event list

***************************************************************************/
bool ACTR::_FUnfreeze(void)
{
	AssertThis(0);
	long faevfrz = (long)fFalse;
	return _FAddDoAev(aetFreeze, kcbVarFreeze, &faevfrz);
}


/***************************************************************************
	
	Modify the add event to re-orient the actor
	The add event retains the single point orientation.

	On Input: If pxyz == pvNil, no averaging is done in orientation
	grfbra are the flags indicating angles of rotation.  In directions
	in which no rotation has taken place, the wa are invalid.
	
	WARNING: Rotations are non-abelian.  Ordering is x then y then z.
	If ordering other than this is desired, separate calls must be made.

***************************************************************************/
void ACTR::SetAddOrient(BRA xa, BRA ya, BRA za, ulong grfbra, XYZ *pdxyz)
{
	AssertThis(0);
	AssertIn(_iaevAddCur, 0, _pggaev->IvMac());

	AEVADD aevadd;

	_pggaev->Get(_iaevAddCur, &aevadd);
	
	if (pvNil == pdxyz)
		{
		aevadd.xa = xa;
		aevadd.ya = ya;
		aevadd.za = za;
		}
	else
		{
		BRS rWeight;
		BRS dwrNew = BR_LENGTH3(pdxyz->dxr, pdxyz->dyr, pdxyz->dzr);
		rWeight = BrsDiv(dwrNew, kdwrFast);
		rWeight = LwBound(rWeight, krOriWeightMin, rOne);

		if (grfbra & fbraRotateX)
			aevadd.xa = _BraAvgAngle(xa, aevadd.xa, rWeight);

		if (grfbra & fbraRotateY)
			aevadd.ya = _BraAvgAngle(ya, aevadd.ya, rWeight);

		if (grfbra & fbraRotateZ)
			aevadd.za = _BraAvgAngle(za, aevadd.za, rWeight);
		}

	//Modify the event
	_pggaev->Put(_iaevAddCur, &aevadd);
	_xfrm.xaPath = aevadd.xa;
	_xfrm.yaPath = aevadd.ya;
	_xfrm.zaPath = aevadd.za;
}


/***************************************************************************
	
	Average two angles by (rw1 * a1 + (1-rw1)*a2)
	rw1 is the weighting for a1

***************************************************************************/
BRA ACTR::_BraAvgAngle(BRA a1, BRA a2, BRS rw1)
{
	AssertBaseThis(0);
	AssertIn(rw1, rZero, rOne + rEps);

	BRS rT = rZero;
	BRS r1 = BrAngleToScalar(a1);
	BRS r2 = BrAngleToScalar(a2);
	BRS rw2;

	rw2 = BrsSub(rOne, rw1);

	// Compensate for averaging across 0 degrees
	if (BrsAbs(BrsSub(r1, r2)) > rOneHalf)
		{
		if (r2 > r1)
			{
			// Add equiv of 360 degrees for each weight of r1
			rT = BrsAdd(rT, rw1);
			}
		else
			{
			// Add equiv of 360 degrees for each weight of r1
			rT = BrsAdd(rT, rw2);
			}							
		}

	rT = BrsAdd(rT, BrsMul(r1, rw1));
	rT = BrsAdd(rT, BrsMul(r2, rw2));
	
	if (rT > rOne)
		rT = rT & rFractMax;

	return(BrScalarToAngle(rT));
}


/***************************************************************************
	
	Rotate the actor
	Add the event to the event list
	If !fFromHereFwd, the rotation is to apply to the current frame only

	WARNING: Rotations are non-abelian.  Ordering is x then y then z.
	If ordering other than this is desired, separate calls must be made.

***************************************************************************/
bool ACTR::FRotate(BRA xa, BRA ya, BRA za, bool fFromHereFwd)
{
	AssertThis(0);

	BMAT34 *pbmat34;
	long aet;

	if (!_pggaev->FEnsureSpace(1, kcbVarRot, fgrpNil))
		return fFalse;

	if (fFromHereFwd)
		{
		aet = aetRotF;
		pbmat34 = &_xfrm.bmat34Fwd;
		if (_fUseBmat34Cur)		// _xfrm.bmat34Cur has last been used
			{
			// _xfrm.bmat34Cur stores the complete (no path added) orientation
			BrMatrix34Copy(pbmat34, &_xfrm.bmat34Cur);
		 	// Back the path orientation out from this matrix.
			// Otherwise the actor will jump in angle
			// This MUST be done in z then y then x order
			// Note: matrix inversion unnecessary
			if (_xfrm.zaPath != aZero)
				BrMatrix34PostRotateZ(pbmat34, -_xfrm.zaPath);
			if (_xfrm.yaPath != aZero)
				BrMatrix34PostRotateY(pbmat34, -_xfrm.yaPath);
			if (_xfrm.xaPath != aZero)
				BrMatrix34PostRotateX(pbmat34, -_xfrm.xaPath);
			}
		}
	else
		{
		// _xfrm.bmat34Cur is kept current at <all> frames so that
		// rotations can be post applied to it
		aet = aetRotH;
		pbmat34 = &_xfrm.bmat34Cur;  	
		}

	if (aZero != xa)
		{
		BrMatrix34PreRotateX(pbmat34, xa);
		}
	if (aZero != ya)
		{
		BrMatrix34PreRotateY(pbmat34, ya);
		}
	if (aZero != za)
		{
		BrMatrix34PreRotateZ(pbmat34, za);
		}

	Assert(_iaevCur <= _pggaev->IvMac(), "_iaevCur bug");

	//Add the event
	if (fFromHereFwd)
		_PrepXfrmFill(aetRotF, pbmat34, kcbVarRot, _iaevCur, ivNil, faetNil);
	AssertDo(_FAddDoAev(aet, kcbVarRot, pbmat34),
		"Ensure space insufficient");

	_PositionBody(&_xyzCur);
	return fTrue;
}


/***************************************************************************
	
	"Normalize" the actor
	The revert tool
	Insert normalize transformation events into the event list
									
***************************************************************************/
bool ACTR::FNormalizeCore(ulong grfnorm)
{
	AssertThis(0);
	long rScaleStepOld = _xfrm.rScaleStep;
	ulong faet;
	long cbVar = 0;

	if (grfnorm & fnormRotate)
		cbVar += kcbVarRot;
	if (grfnorm & fnormSize)
		cbVar += kcbVarSize + kcbVarPull;

	if (!_pggaev->FEnsureSpace( 3, cbVar, fgrpNil))
		return fFalse;

	if (grfnorm & fnormRotate)
		{
		_InitXfrmRot(&_xfrm.bmat34Fwd);
		_InitXfrmRot(&_xfrm.bmat34Cur);
		_PrepXfrmFill(aetRotF, &_xfrm.bmat34Fwd, kcbVarRot, _iaevCur);
		AssertDo(_FAddDoAev(aetRotF, kcbVarRot, &_xfrm.bmat34Fwd),
			"EnsureSpace insufficient");
		}

	if (grfnorm & fnormSize)
		{
		_xfrm.aevpull.rScaleX = rOne;
		_xfrm.aevpull.rScaleY = rOne;
		_xfrm.aevpull.rScaleZ = rOne;
		faet = (_xfrm.rScaleStep != rOne) ? faetTweak : faetNil;
		_xfrm.rScaleStep = rOne;
		_PrepXfrmFill(aetPull, &_xfrm.aevpull, kcbVarPull, _iaevCur);
		_PrepXfrmFill(aetSize, &_xfrm.rScaleStep, kcbVarSize, _iaevCur, ivNil, faet);
	
		AssertDo(_FAddDoAev(aetPull, kcbVarPull, &_xfrm.aevpull),
			"EnsureSpace insufficient");
		AssertDo(_FAddDoAev(aetSize, kcbVarSize, &_xfrm.rScaleStep),
			"EnsureSpace insufficient");
		
		// Possibly extend life of actor if shrunk
		if (rScaleStepOld > rOne &&
			pvNil != _pscen &&
			_rtelCur.irpt < _pglrpt->IvMac() - 1) 	//optimization
			{
			_fLifeDirty = fTrue;
			_pscen->InvalFrmRange();
			}	
		}

	Assert(_iaevCur <= _pggaev->IvMac(), "_iaevCur bug");

	_PositionBody(&_xyzCur);
	return fTrue;
}


/***************************************************************************
	
	Scale the actor.
	Add the event to the event list
	Bound the allowable scaling.

	Note that the stepsize and the actor lifetime is then subject to change,
	with the attendent specified tweak etc modifications.

***************************************************************************/
bool ACTR::FScale(BRS rScale)
{
	AssertThis(0);

	ulong faet = faetNil;
	long rScaleStep;
	long rScaleStepOld = _xfrm.rScaleStep;

	if (!_pggaev->FEnsureSpace( 1, kcbVarSize, fgrpNil))
		return fFalse;

	rScale = LwBound(rScale, krScaleMinFactor, krScaleMaxFactor);

	rScaleStep = BrsMul(_xfrm.rScaleStep, rScale);
	_xfrm.rScaleStep = LwBound(rScaleStep, krScaleMin, krScaleMax);

	// Remove tweaks when a transformation that changes stepsize occurs
	faet = (_xfrm.rScaleStep != rScale) ? faetTweak : faetNil;
	_PrepXfrmFill(aetSize, &_xfrm.rScaleStep, kcbVarSize, _iaevCur, ivNil, faet);

	if (!_FAddDoAev(aetSize, kcbVarSize, &_xfrm.rScaleStep))
		return fFalse;

	Assert(_iaevCur <= _pggaev->IvMac(), "_iaevCur bug");
	
	if (_xfrm.rScaleStep != rScale)	
		_GetXyzFromRtel(&_rtelCur, &_xyzCur);
	_PositionBody(&_xyzCur);
	
	// Possibly extend life of actor if shrunk
	if (rScaleStepOld != _xfrm.rScaleStep &&
		pvNil != _pscen &&
		_rtelCur.irpt < _pglrpt->IvMac() - 1) 	//optimization
		{
		_fLifeDirty = fTrue;
		_pscen->InvalFrmRange();
		}

	return fTrue;
}


/***************************************************************************
	
	Drag the actor forwards or backwards in time
	NOTE** Drag this subroute only -> sliding other subroutes in time

***************************************************************************/
bool ACTR::FSoonerLater(long dnfrm)
{
	AssertThis(0);
	
	AEV aev;
	AEV *paevPrev;
	long nfrmSav;
	long iaev;
	long fSuccess;
	long dnfrmSub;
	long dnfrmT;

	if (0 == dnfrm)
		return fTrue;
	
	if (_fLifeDirty)
		{
		if (!_FComputeLifetime(pvNil))
	  		PushErc(ercSocBadFrameSlider);
		}

	Assert(_iaevAddCur >= 0, "Invalid value for _iaevAddCur");

	// On a Sooner operation, slide the earlier subroutes
	// backward in time
	if (dnfrm < 0)	
		{
		dnfrmT = (-dnfrm);
		// The times of past events need to be correct.
		for (iaev = _iaevCur - 1; dnfrmT > 0 && iaev >= 0; iaev--)
			{
			_pggaev->GetFixed(iaev, &aev);
			if (aev.aet != aetAdd || iaev == 0)
				{
				aev.nfrm -= dnfrmT;
				_pggaev->PutFixed(iaev, &aev);
				continue;
				}

			// Account for subroute gaps
			paevPrev = (AEV *)_pggaev->QvFixedGet(iaev - 1);	
			dnfrmSub = aev.nfrm - (paevPrev->nfrm);
			aev.nfrm -= dnfrmT;
			_pggaev->PutFixed(iaev, &aev);
			dnfrmT -= (dnfrmSub - 1);
			}
		}
	else  			
		{
		// Later
		dnfrmT = dnfrm;
		for (iaev = _iaevAddCur; dnfrmT > 0 && iaev < _pggaev->IvMac(); iaev++)
			{
			_pggaev->GetFixed(iaev, &aev);
			if (aev.aet != aetAdd || iaev == _iaevAddCur)
				{
				nfrmSav = aev.nfrm;
				aev.nfrm += dnfrmT;
				_pggaev->PutFixed(iaev, &aev);
				continue;
				}
			// Adjust for the gap between subroutes
			dnfrmSub = aev.nfrm - nfrmSav;
			dnfrmT -= (dnfrmSub - 1);
			if (dnfrmT <= 0)
				break;
			aev.nfrm += dnfrmT;
			_pggaev->PutFixed(iaev, &aev);
			}
		}
	_pggaev->GetFixed(0, &aev);	
	_nfrmFirst = aev.nfrm;
	_pggaev->GetFixed(_pggaev->IvMac() - 1, &aev);
	_nfrmLast = aev.nfrm;
			
	_nfrmCur += dnfrm;

	// Invalidate, but do not recompute the range	
	_pscen->InvalFrmRange();	

	if (fSuccess = _pscen->FGotoFrm(_pscen->Nfrm() + dnfrm))
		{
		_pscen->Pmvie()->Pmcc()->UpdateScrollbars();
		}
	return fSuccess;
}


/***************************************************************************
	
	Pull, stretch, squash the actor.
	Add the event to the event list

	Bound the allowable scaling.

***************************************************************************/
bool ACTR::FPull(BRS rScaleX, BRS rScaleY, BRS rScaleZ)
{
	AssertThis(0);

	BRS rScaleXT;
	BRS rScaleYT;
	BRS rScaleZT;

	if (!_pggaev->FEnsureSpace( 1, kcbVarPull, fgrpNil))
		return fFalse;

	rScaleX = LwBound(rScaleX, krScaleMinFactor, krScaleMaxFactor);
	rScaleY = LwBound(rScaleY, krScaleMinFactor, krScaleMaxFactor);
	rScaleZ = LwBound(rScaleZ, krScaleMinFactor, krScaleMaxFactor);

	rScaleXT = BrsMul(_xfrm.aevpull.rScaleX, rScaleX);
	rScaleYT = BrsMul(_xfrm.aevpull.rScaleY, rScaleY);
	rScaleZT = BrsMul(_xfrm.aevpull.rScaleZ, rScaleZ);

	_xfrm.aevpull.rScaleX = LwBound(rScaleXT, krPullMin, krPullMax);
	_xfrm.aevpull.rScaleY = LwBound(rScaleYT, krPullMin, krPullMax);
	_xfrm.aevpull.rScaleZ = LwBound(rScaleZT, krPullMin, krPullMax);

	_PrepXfrmFill(aetPull, &_xfrm.aevpull, kcbVarPull, _iaevCur);

	if (!_FAddDoAev(aetPull, kcbVarPull, &_xfrm.aevpull))
		return fFalse;

	Assert(_iaevCur <= _pggaev->IvMac(), "_iaevCur bug");
		
	_PositionBody(&_xyzCur);
	return fTrue;	
}


/***************************************************************************
	
	Translate one point of the actor's path
	Add an event to the event list

	The (dxr,dyr,dzr) passed in is a delta distance from the previous
	location (set by GoToFrame, ie, determined by _rtelCur).

***************************************************************************/
bool ACTR::FTweakRoute(BRS dxr, BRS dyr, BRS dzr, ulong grfmaf)
{
	AssertThis(0);

	XYZ xyz;

	if (!_pggaev->FEnsureSpace( 1, kcbVarTweak, fgrpNil))
		return fFalse;

	xyz.dxr = BrsAdd(_xyzCur.dxr, dxr);
	xyz.dyr = BrsAdd(_xyzCur.dyr, dyr);
	xyz.dzr = BrsAdd(_xyzCur.dzr, dzr);

	if ((grfmaf & fmafGround) &&
		(BrsAdd(_xyzCur.dyr, _dxyzRte.dyr) >= rZero) &&
		(BrsAdd(xyz.dyr, _dxyzRte.dyr) < rZero))
		{
		xyz.dyr = -_dxyzRte.dyr;
		}

	if (!_FAddDoAev(aetTweak, kcbVarTweak, &xyz))
		return fFalse;
		
	_PositionBody(&xyz);
	return fTrue;
}


/***************************************************************************
	
	Update xyzRte from the overall origin & the subroute translation

***************************************************************************/
void ACTR::_UpdateXyzRte(void)
{
	AssertBaseThis(0);

	_dxyzRte.dxr = BrsAdd(_dxyzFullRte.dxr, _dxyzSubRte.dxr);
	_dxyzRte.dyr = BrsAdd(_dxyzFullRte.dyr, _dxyzSubRte.dyr);
	_dxyzRte.dzr = BrsAdd(_dxyzFullRte.dzr, _dxyzSubRte.dzr);
}


/***************************************************************************
	
	Move the current subroute.	(Compose tool)

	If fmafOrient is set, the actor is oriented and moved.

	If fmafEntireSubrte is set, this does not add an event to the aev list.
	It alters the data in the Add event & _dxyzSubRte.	Return fTrue.

	If fmafEntireScene is set, _dxyzFullRte is modified. Return fTrue.

	If !fmafEntireSubrte && !fmafEntireScene, a move event is added
	to the event list. This can therefore fail.

	Return success or failure
	Return *pfmoved indicating whether or not the path moved.

***************************************************************************/
bool ACTR::FMoveRoute(BRS dxr, BRS dyr, BRS dzr, bool *pfMoved, ulong grfmaf)
{
	AssertThis(0);
	AssertNilOrVarMem(pfMoved);
	
	AEVADD aevadd;
	AEV *paev;
	long iaev;
	bool fMoved;
	ulong grfbra;
	XYZ dxyz;
	BRS yrCurOld;
	BRA xa;
	BRA ya;
	BRA za;

	fMoved = (rZero != dxr || rZero != dyr || rZero != dzr);
	if (pvNil != pfMoved)
		*pfMoved = fMoved;

	if (!fMoved)
		return fTrue;

	if (!_pggaev->FEnsureSpace( 2, kcbVarMove + kcbVarRot, fgrpNil))
		return fFalse;

	// Edit dyr to respect ground
	yrCurOld = BrsAdd(_xyzCur.dyr, _dxyzRte.dyr);
	if (FPure(grfmaf & fmafGround) &&
		(yrCurOld >= rZero) &&
		(BrsAdd(dyr, yrCurOld) < rZero))
		{
		dyr = BrsSub(rZero, yrCurOld);
		}
	
	// Update Actor's orientation
	if (FPure(grfmaf & fmafOrient) && !_ptmpl->FIsTdt())
		{
		dxyz.dxr = dxr;
		dxyz.dyr = dyr;
		dxyz.dzr = dzr;

		// Back the path orientation out from this matrix & replace
		// with new orientation
		if (_xfrm.zaPath != aZero)
			BrMatrix34PostRotateZ(&_xfrm.bmat34Cur, -_xfrm.zaPath);
		if (_xfrm.yaPath != aZero)
			BrMatrix34PostRotateY(&_xfrm.bmat34Cur, -_xfrm.yaPath);
		if (_xfrm.xaPath != aZero)
			BrMatrix34PostRotateX(&_xfrm.bmat34Cur, -_xfrm.xaPath);

		_ApplyRotFromVec(&dxyz, pvNil, &xa, &ya, &za, &grfbra);
		SetAddOrient(xa, ya, za, grfbra, &dxyz);
		
		// Force speed to be inversely proportional to angular rotation
		if (_iaevAddCur != ivNil)
			{
			BRS dwra, drxa, drya, drza;
			BRS ra1, ra2;
#ifdef DEBUG
			{
			AEV *paev;
			paev = (AEV *)_pggaev->QvFixedGet(_iaevAddCur);
			Assert(_nfrmCur == paev->nfrm, "Unsupported use of fmafOrient");
			}
#endif //DEBUG
			// Set dwra = angular change (in scalar form)
			_pggaev->Get(_iaevAddCur, &aevadd);
			ra1 = BrAngleToScalar(xa);
			ra2 = BrAngleToScalar(aevadd.xa);
			drxa = BrsAbs(BrsSub(ra1, ra2));
			if (drxa > rOneHalf)
				{
				if (ra2 > ra1)
					ra1 = BrsAdd(rOne, ra1);
				else
					ra2 = BrsAdd(rOne, ra2);						
				drxa = BrsAbs(BrsSub(ra1, ra2));
				}

			ra1 = BrAngleToScalar(ya);
			ra2 = BrAngleToScalar(aevadd.ya);
			drya = BrsAbs(BrsSub(ra1, ra2));
			if (drya > rOneHalf)
				{
				if (ra2 > ra1)
					ra1 = BrsAdd(rOne, ra1);
				else
					ra2 = BrsAdd(rOne, ra2);						
				drya = BrsAbs(BrsSub(ra1, ra2));
				}

			ra1 = BrAngleToScalar(za);
			ra2 = BrAngleToScalar(aevadd.za);
			drza = BrsAbs(BrsSub(ra1, ra2));
			if (drza > rOneHalf)
				{
				if (ra2 > ra1)
					ra1 = BrsAdd(rOne, ra1);
				else
					ra2 = BrsAdd(rOne, ra2);						
				drza = BrsAbs(BrsSub(ra1, ra2));
				}

			dwra = BR_LENGTH3(drxa, drya, drza);

			// Compute a bounded inverse of the angular change
			dwra = LwBound(dwra, krAngleMin, krAngleMax);
			dwra = BrsDiv(BrsRcp(dwra), krAngleMinRcp);
			AssertIn(dwra, rZero, BrsAdd(rOne, rEps));

			// Adjust the distances
			dxr = BrsMul(dxr, dwra);
			dyr = BrsMul(dyr, dwra);
			dzr = BrsMul(dzr, dwra);
			}
		
		// _xfrm.bmat34Cur is to hold the current full rotation
		_LoadAddOrien(&aevadd, fTrue);

		// Update any type of rotate event in this frame
		for (iaev = _iaevAddCur + 1; iaev < _iaevCur; iaev++)
			{
			paev = (AEV *)_pggaev->QvFixedGet(iaev);
			if (aetRotH == paev->aet)
				_pggaev->Put(iaev, &_xfrm.bmat34Cur);
			if (aetRotF == paev->aet)
				{
				// Insert orientation-rotation event
				if (!_FAddDoAev(aetRotH, kcbVarRot, &_xfrm.bmat34Cur))
					{
					Bug("Should have ensured space");
					return fFalse;
					}
				}
			}
		}

	// Update actor's position
	if (FPure(grfmaf & fmafEntireScene))
		{
		_dxyzFullRte.dxr = BrsAdd(dxr, _dxyzFullRte.dxr);
		_dxyzFullRte.dyr = BrsAdd(dyr, _dxyzFullRte.dyr);
		_dxyzFullRte.dzr = BrsAdd(dzr, _dxyzFullRte.dzr);
		}
	else
		{
		if (_iaevAddCur == ivNil)
			return fFalse;
			
		if (!FPure(grfmaf & fmafEntireSubrte))
			{
			// Move actor just from this frame on (this subpath only)
			// Note: FAddDoAev will update _dxyzSubRte
			dxyz.dxr = dxr;
			dxyz.dyr = dyr;
			dxyz.dzr = dzr;
			if (!_FAddDoAev(aetMove, kcbVarMove, &dxyz))
				return fFalse;
			}
		else
			{
			// Translating whole subroute
#ifdef DEBUG
			long cbVar = _pggaev->Cb(_iaevAddCur);
			Assert(cbVar == kcbVarAdd, "Corrupt aev");
#endif //DEBUG
			// Adjust the translation state variables
			_dxyzSubRte.dxr = BrsAdd(dxr, _dxyzSubRte.dxr);
			_dxyzSubRte.dyr = BrsAdd(dyr, _dxyzSubRte.dyr);
			_dxyzSubRte.dzr = BrsAdd(dzr, _dxyzSubRte.dzr);

			// Adjust the position in the add event
			_pggaev->Get(_iaevAddCur, &aevadd);
			aevadd.dxr = BrsAdd(aevadd.dxr, dxr);
			aevadd.dyr = BrsAdd(aevadd.dyr, dyr);
			aevadd.dzr = BrsAdd(aevadd.dzr, dzr);
			_pggaev->Put(_iaevAddCur, &aevadd);
			}
		}
	_UpdateXyzRte();

	_pscen->MarkDirty();

	// Update Brender model
	fMoved = FPure(rZero != dxr || rZero != dyr || rZero != dzr);

	if (fMoved)
		_PositionBody(&_xyzCur);
	
	return fTrue;
}


/***************************************************************************
	
	Load starting point orientation into state variable _xfrm

***************************************************************************/
void ACTR::_LoadAddOrien(AEVADD *paevadd, bool fNoReset)
{
	AssertThis(0);

	// Set _xfrm.bmat34Cur always holds the current full rotation
	if (!fNoReset)
		_InitXfrmRot(&_xfrm.bmat34Cur);
	BrMatrix34PostRotateX(&_xfrm.bmat34Cur, paevadd->xa);
	BrMatrix34PostRotateY(&_xfrm.bmat34Cur, paevadd->ya);
	BrMatrix34PostRotateZ(&_xfrm.bmat34Cur, paevadd->za);
	_xfrm.xaPath = paevadd->xa;
	_xfrm.yaPath = paevadd->ya;
	_xfrm.zaPath = paevadd->za;
}

/***************************************************************************
	
	Remove redundant events of a targeted type.	 (Actor uses this for
	Xfrm editing)
	NOTE***: This is not valid for Actn or Cost fill.  Special case routines
	exist for those event types.

	This is called _before_ inserting the new event.

	Note: For transforms, this code will rarely have effect unless the client
	uses discretized increments of observable size, ie, it is statistically
	unlikely that a match of the same size will be found. This code was
	requested in anticipation of discretized increments.
	
	Input: aet == the event type
		*pvVar == the variable part of the new event being inserted
		cbVar == the size of the variable part
		iaevMin == the first possible event to delete
		iaevCmp == ivNil or the event being replaced
		grfaet == faetNil or types of events to nuke along the way

***************************************************************************/
void ACTR::_PrepXfrmFill(long aet, void *pvVar, long cbVar,
	long iaevMin, long iaevCmp, ulong grfaet)
{
	AssertBaseThis(0);
	Assert(aet == aetSize || aet == aetRotF || aet == aetPull,
		"Illegal argument aet");
	AssertPvCb(pvVar, cbVar);
	AssertIn(cbVar, 0, 100); 	// Approximate bound
	AssertIn(iaevMin, 0, _pggaev->IvMac() + 1);
	AssertIn(iaevCmp, -1, _pggaev->IvMac() + 1);
	Assert(aet != aetActn && aet != aetCost, "Illegal aet argument");

	long iaev;
	AEV aev;
	bool fReplacePrev = fTrue;
	void *pvVarCmp = pvNil;
	long cb;
	
	_pggaev->Lock();

	if (ivNil != iaevCmp)
		{
		pvVarCmp = _pggaev->QvGet(iaevCmp, &cb);
		Assert(cb == cbVar, "Logic error");
		}
	else
		{
		// Locate the most current event of this type
		// and store the ptr in pvVarCmp
		for (iaev = 0; iaev < iaevMin - 1; iaev++)
			{
			_pggaev->GetFixed(iaev, &aev);
			if (aet != aev.aet)
				continue;

			pvVarCmp = _pggaev->QvGet(iaev, &cb);
			Assert(cb == cbVar, "Logic error");
			}
		}

	if (pvNil == pvVarCmp)
		pvVarCmp = pvVar;

	for (iaev = iaevMin; iaev < _pggaev->IvMac(); iaev++)
		{
		bool fDelete = fFalse;
		_pggaev->GetFixed(iaev, &aev);
		
		// Stage entrance events are boundaries to edits
		if (aetAdd == aev.aet)
			goto LEnd;

		switch(aev.aet)
			{
		case aetSize:
		case aetPull:
		case aetRotF:
			void *pv1;
			void *pv2;
			if (aev.aet != aet)
				continue;

			if (fReplacePrev)
				{
				pv1 = pvVarCmp;
				pv2 = _pggaev->QvGet(iaev);
				if (fcmpEq != FcmpCompareRgb(pv1, pv2, cbVar))
					{
					//Prepare to test for a match with new event
					fReplacePrev = fFalse;
					pvVarCmp = pvVar;
					fDelete = fFalse;
					}
				else
					{
					fDelete = fTrue;
					goto LDelete;
					}
				}

			Assert(!fReplacePrev, "Logic Error");
			pv1 = pvVarCmp;
			pv2 = _pggaev->QvGet(iaev);
			if (fcmpEq != FcmpCompareRgb(pv1, pv2, cbVar))
				goto LEnd;
			
			//Events equal : delete event at iaev
			fDelete = fTrue;
			break;

		case aetTweak:
			if ((grfaet & faetTweak) && (aev.rtel >= _rtelCur))
				fDelete = fTrue;
			break;
		case aetRotH:
			if ((grfaet & faetRotF) && (aev.rtel == _rtelCur))
				fDelete = fTrue;
			break;
		case aetFreeze:
			if ((grfaet & faetFreeze) && (aev.rtel >= _rtelCur))
				fDelete = fTrue;
			break;
		case aetStep:
			if ((grfaet & faetStep) && (aev.rtel >= _rtelCur))
				fDelete = fTrue;;
			break;

		default:
			break;
			}
LDelete:
		if (fDelete)
			{
			_RemoveAev(iaev);
			iaev--;			
			}
		}

LEnd:
	_pggaev->Unlock();
}


/***************************************************************************

	Insert Aev.  Update state variables
	Since _pggaev is getting a copy of a tag, a call to DupTag is required

***************************************************************************/
bool ACTR::_FInsertAev(long iaev, long cbNew, void *pvVar, void *paev, bool fUpdateState)
{
	AssertBaseThis(0);
	AssertIn(iaev, 0, _pggaev->IvMac() + 1);
	AssertIn(cbNew, 0, 100);	// approximate bound
	if (pvNil != pvVar)
		AssertPvCb(pvVar, cbNew);
	AssertPvCb(paev, size(AEV));

	PTAG ptag;

	if (!_pggaev->FInsert(iaev, cbNew, pvVar, paev))
		return fFalse;

	// If not simply allocating space
	if (pvVar != pvNil)
		{
		// Increment tag count
		_pggaev->Lock();
		if (_FIsIaevTag(_pggaev, iaev, &ptag))
			TAGM::DupTag(ptag);
		_pggaev->Unlock();

		if (fUpdateState)
			{
			if (iaev < _iaevCur)
				_iaevCur++;
			if (iaev <= _iaevActnCur)
				_iaevActnCur++;
			if (iaev <= _iaevAddCur)
				_iaevAddCur++;
			if (iaev < _iaevFrmMin)
				_iaevFrmMin++;
			}
	   	}

	return fTrue;
}

	

/***************************************************************************

	Remove Aev.  Update state variables

***************************************************************************/
void ACTR::_RemoveAev(long iaev, bool fUpdateState)
{
	AssertBaseThis(0);
	AssertIn(iaev, 0, _pggaev->IvMac());

	bool fUpdateSndFrame;
	PTAG ptag;
	PAEV qaev;

	// First, close tags
	_pggaev->Lock();
	if (_FIsIaevTag(_pggaev, iaev, &ptag, &qaev))
		TAGM::CloseTag(ptag);

	/* Don't bother updating the frame sound indicator if we didn't change
		an event in the scene's current frame */
	fUpdateSndFrame = (qaev->aet == aetSnd) && (_pscen != pvNil) &&
		(qaev->nfrm == _pscen->Nfrm());

	_pggaev->Unlock();  // qaev is invalid past here!
	TrashVar(&qaev);

	_pggaev->Delete(iaev);

	if (fUpdateState)
		{
		if (iaev < _iaevCur)
			_iaevCur--;
		if (iaev < _iaevActnCur)
			_iaevActnCur--;
		if (iaev < _iaevAddCur)
			_iaevAddCur--;
		if (iaev < _iaevFrmMin)
			_iaevFrmMin--;
		}

	if (fUpdateSndFrame)
		_pscen->UpdateSndFrame();
}

	
/***************************************************************************
	
	Remove specified (eg, tweak, step, freeze) events for the current action
	If faetActn is set in the grfaet, also remove action events
	matching the replaced action followed by action events matching
	the new (current) action _anidCur.

	Note: This routine exists in addition to the more general PrepXfrmFill
	because actn events are	considered to match if the anid's match
	regardless of the value of the celn.

***************************************************************************/
void ACTR::_PrepActnFill(long iaevMin, long anidPrev, long anidNew, ulong grfaet)
{
	AssertBaseThis(0);
	AssertIn(iaevMin, 0, _pggaev->IvMac() + 1);
	
	AEV aev;
	long iaev;
	long anid = anidPrev;

	for (iaev = iaevMin; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		
		// Stage entrance events are boundaries to edits
		if (aetAdd == aev.aet)
			return;

		if (aetActn == aev.aet)
			{
			AEVACTN aevactn;
			_pggaev->Get(iaev, &aevactn);

			if (!(grfaet & faetActn))
				{
				if (aevactn.anid == anidNew)
					continue;
				else
					return;
				}

			if (aevactn.anid == anid)
				{
				_RemoveAev(iaev);
				iaev--;
				}
			else if (aevactn.anid == anidNew)
				{
				anid = anidNew;
				_RemoveAev(iaev);
				iaev--;
				}
			else
				return;
			}

		if (aev.rtel < _rtelCur)
			continue;

		if (aev.aet == aetTweak && (grfaet & faetTweak))
			{
			_RemoveAev(iaev);
			iaev--;
			}

		if (aev.aet == aetFreeze && (grfaet & faetFreeze))
			{
			// Do not remove end-of-subroute freeze events
			if (!_FIsDoneAevSub(iaev, aev.rtel))
				{
				_RemoveAev(iaev);
				iaev--;
				}
			}		
		}
}


/***************************************************************************
	
	Remove redundant costume changes
	Note:  	Delete all costume events that match the costume being replaced
			Then delete all subsequent costume events that match the
			costume being inserted

***************************************************************************/
void ACTR::_PrepCostFill(long iaevMin, AEVCOST *paevcost)
{
	AssertBaseThis(0);
	AssertIn(iaevMin, 0, _pggaev->IvMac() + 1);
	AssertVarMem(paevcost);
	
	long iaev;
	AEV aev;
	AEVCOST aevcost;
	bool fMtrl;
	bool fCmtl;
	MTRL *pmtrlCmp;
	CMTL *pcmtlCmp;
	MTRL *pmtrl = pvNil;
	CMTL *pcmtl = pvNil;
	bool fReplacePrev = fTrue;

	// Locate the most current costume for this body part
	_pbody->GetPartSetMaterial(paevcost->ibset, &fMtrl,
		&pmtrlCmp, &pcmtlCmp);
	fCmtl = !fMtrl;
			
	for (iaev = iaevMin; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);

		// Stage entrance events are boundaries to edits
		if (aetAdd == aev.aet)
			goto LEnd;

		if (aetCost != aev.aet)
			continue;

		_pggaev->Get(iaev, &aevcost);
		if (aevcost.ibset != paevcost->ibset)
			continue;

		if (fReplacePrev)
			{
			// Delete the event if the costumes are the same
			if ((fCmtl == aevcost.fCmtl) &&
				((!fCmtl &&
				(pmtrlCmp == (pmtrl = (PMTRL)vptagm->PbacoFetch(&aevcost.tag, MTRL::FReadMtrl))))
				||
				(fCmtl &&
				(pcmtlCmp == (pcmtl = _ptmpl->PcmtlFetch(aevcost.cmid))))))
				{
				goto LDelete;
				}
			
			// This event does not match the one being replaced
			// Prepare to test for a match with the event being inserted
			fReplacePrev = fFalse;
			if (!paevcost->fCmtl)
				{
				pmtrlCmp = (PMTRL)vptagm->PbacoFetch(&paevcost->tag, MTRL::FReadMtrl);
				pcmtlCmp = pvNil;
				}
			else
				{
				pcmtlCmp = _ptmpl->PcmtlFetch(paevcost->cmid);
				pmtrlCmp = pvNil;
				}
			
			}

		ReleasePpo(&pcmtl);
		ReleasePpo(&pmtrl);
		Assert(!fReplacePrev, "Logic error");
		if (paevcost->fCmtl != aevcost.fCmtl ||
			(!aevcost.fCmtl &&
			(pmtrlCmp != (pmtrl = (PMTRL)vptagm->PbacoFetch(&aevcost.tag, MTRL::FReadMtrl))))
					 ||
			(aevcost.fCmtl && (pcmtlCmp != (pcmtl = _ptmpl->PcmtlFetch(aevcost.cmid)))))
			{
			// If the costumes differ
			goto LEnd;
			}
LDelete:
		ReleasePpo(&pcmtl);
		ReleasePpo(&pmtrl);
		_RemoveAev(iaev);
		iaev--;
		}
LEnd:
	if (!fReplacePrev)
		{
		ReleasePpo(&pmtrlCmp);
		ReleasePpo(&pcmtlCmp);
		}
	ReleasePpo(&pmtrl);
	ReleasePpo(&pcmtl);
}


/***************************************************************************
	
	AdjustAevForRte for an already inserted point at irptAdjust
	adjust the rtel's of the subsequent affected events
	
***************************************************************************/
void ACTR::_AdjustAevForRteIns(long irptAdjust, long iaevMin)
{
	AssertBaseThis(0);
	long irptMac = _pglrpt->IvMac();
	AssertIn(irptAdjust, 0, irptMac);
	AssertIn(iaevMin, 0, _pggaev->IvMac() + 1);

	long iaev;
	AEV aev;
	RPT rptBack, rptAdjust;
	BRS dwrBack;

	if (irptAdjust > 0)
		{
		_pglrpt->Get(irptAdjust - 1, &rptBack);
		dwrBack = rptBack.dwr;
		}
	else
		dwrBack = rZero;

	_pglrpt->Get(irptAdjust, &rptAdjust);

	for (iaev = iaevMin; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.rtel.irpt >= irptAdjust)	//offset from later point
			{
			aev.rtel.irpt++;
			}
		else if (aev.rtel.irpt < irptAdjust - 1)
			{
			continue;
			}
		else
			{
			if ((irptAdjust == irptMac - 1)	|| (rZero == dwrBack))
				continue;

			if (aev.rtel.dwrOffset >= dwrBack)
				{
				aev.rtel.dwrOffset = BrsSub(aev.rtel.dwrOffset, dwrBack);
				aev.rtel.irpt++;
				}
			}
		_pggaev->PutFixed(iaev, &aev);
		}
}
/***************************************************************************
	
	AdjustAevForRte for a to-be deleted point at irptAdjust
	adjust the rtel's of the affected events

***************************************************************************/
void ACTR::_AdjustAevForRteDel(long irptAdjust, long iaevMin)
{
	AssertBaseThis(0);
	long irptMac = _pglrpt->IvMac();
	AssertIn(irptAdjust, 0, irptMac);
	AssertIn(iaevMin, 0, _pggaev->IvMac() + 1);

	long iaev;
	AEV aev;
	RPT rptBack, rptAdjust;
	BRS dwrBack, dwrFwd;

	if (0 == irptAdjust)
		dwrBack = rZero;
	else
		{
		_pglrpt->Get(irptAdjust-1, &rptBack);
		dwrBack = rptBack.dwr;
		}
		
	_pglrpt->Get(irptAdjust, &rptAdjust);
	dwrFwd = rptAdjust.dwr;

	for (iaev = iaevMin; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.rtel.irpt > irptAdjust)		//offset from later point
			{
			aev.rtel.irpt--;
			}
		else if (aev.rtel.irpt == irptAdjust)
			{
			if (rptAdjust.dwr == rZero)
				{
				// Point will be deleted.  Event must be also
				_RemoveAev(iaev);
				iaev--;
				continue;
				}
			aev.rtel.dwrOffset = BrsAdd(dwrBack, aev.rtel.dwrOffset);
			aev.rtel.irpt--;
			}
		else
			continue;

		_pggaev->PutFixed(iaev, &aev);
		}

}


/***************************************************************************
	
	Locate a point on the path dwrStep further along the route from *prtel
	Events which potentially modify this step (Action, Add) begin at iaevCur.
	*pfEndRoute is set only when an attempt is made to move past the last
	node of the subroute.
	
	NOTE: This routine is designed to be independent of all frame dependent
	state variables.
	Spec: The end of a subroute is <always> reached in a moving action.
***************************************************************************/
void ACTR::_AdvanceRtel(BRS dwrStep, RTEL *prtel, long iaevCur,
				long nfrmCur, bool *pfEndRoute)
{
	AssertBaseThis(0);
	AssertVarMem(prtel);
	AssertNilOrVarMem(pfEndRoute);
	
	BRS dwrT;
	bool fEndRoute = fFalse;
	long iaev;
	AEV aev;

	// If at start of path
	// Note: _nfrmFirst is independent with respect to the current frame
	if (nfrmCur <= _nfrmFirst)
		{
		prtel->irpt = 0;
		prtel->dwrOffset = rZero;
		prtel->dnfrm = 0;
		goto LDone;
		}

	// End of route means trying to move beyond the last point.
	if (rZero == dwrStep)		//Not an "end-of-route"
		{
		prtel->dnfrm++;
		goto LDone;
		}
	
	// Move to the correct path segment
	dwrT = ((RPT *)_pglrpt->QvGet(prtel->irpt))->dwr;
	dwrT = BrsSub(dwrT, prtel->dwrOffset);
	if (rZero == dwrT)
		{
		Assert(prtel->dwrOffset == rZero, "Expected zero offset");
		prtel->dnfrm++;
		fEndRoute = fTrue;	
		goto LDone;
		}

	while (dwrT <= dwrStep && rZero < dwrStep)
		{
		// Spec: Move the partial step
		// Not an "end-of-route" - didn't try to move beyond
		if (rZero == dwrT)
			{
			goto LDoneMove;
			}
		prtel->irpt++;
		AssertIn(prtel->irpt, 1, _pglrpt->IvMac());
		prtel->dwrOffset = rZero;
		dwrStep = BrsSub(dwrStep, dwrT);
		dwrT = ((RPT *)_pglrpt->QvGet(prtel->irpt))->dwr;
		}

	dwrT = ((RPT *)_pglrpt->QvGet(prtel->irpt))->dwr;
	prtel->dwrOffset = BrsAdd(prtel->dwrOffset, dwrStep);

LDoneMove:
	prtel->dnfrm = 0;

	if (!_fModeRecord && !_fRejoin)
		{
		// Spec: Ordinarily, the actor will display at the end of this step.
		// If Actn or Step events exist, the the actor is to display
		// at the location of the event.
		for (iaev = iaevCur; iaev < _pggaev->IvMac(); iaev++)
			{
			_pggaev->GetFixed(iaev, &aev);
			if ((aev.rtel <= *prtel) &&
				(aev.aet == aetStep || aev.aet == aetActn))
				{
				*prtel = aev.rtel;
				break;
				}
			if (aev.rtel > *prtel)
				break;
			}
		}

LDone:			
	if (pvNil != pfEndRoute)
		*pfEndRoute = fEndRoute;

	return;
}


/***************************************************************************
	
	Convert a route location (rtel) to an xyz point (in *pxyz)

***************************************************************************/
void ACTR::_GetXyzFromRtel(RTEL *prtel, PXYZ pxyz)
{
	AssertBaseThis(0);
	AssertVarMem(prtel);
	AssertVarMem(pxyz);

	RPT rpt;
	RPT rptFirst;
	RPT rptSecond;
	BRS rFract;

	if (rZero == prtel->dwrOffset ||
		(prtel->irpt) + 1 == _pglrpt->IvMac())
		{
		_pglrpt->Get(prtel->irpt, &rpt);
		*pxyz = rpt.xyz;
		return;
		}

	_pglrpt->Get(prtel->irpt, &rptFirst);
	_pglrpt->Get(1 + prtel->irpt, &rptSecond);
	Assert(rptFirst.dwr >= prtel->dwrOffset,
		"Possible offset bug in _GetXyzFromRtel");

	rFract = BrsDiv(prtel->dwrOffset, rptFirst.dwr);
	_GetXyzOnLine(&rptFirst.xyz, &rptSecond.xyz, rFract, pxyz);
 }


/***************************************************************************
	
	Find a point rFract fractional distance between two points.
	Store in *pxyz

***************************************************************************/
void ACTR::_GetXyzOnLine(PXYZ pxyzFirst, PXYZ pxyzSecond, BRS rFract,
	PXYZ pxyz)
{
	AssertBaseThis(0);
	AssertVarMem(pxyzFirst);
	AssertVarMem(pxyzSecond);
	AssertVarMem(pxyz);

	// New pt = first + (second - first) * fractoffset;
	pxyz->dxr = BrsSub(pxyzSecond->dxr, pxyzFirst->dxr);
	pxyz->dyr = BrsSub(pxyzSecond->dyr, pxyzFirst->dyr);
	pxyz->dzr = BrsSub(pxyzSecond->dzr, pxyzFirst->dzr);

	pxyz->dxr = BrsMul(rFract, pxyz->dxr);
	pxyz->dyr = BrsMul(rFract, pxyz->dyr);
	pxyz->dzr = BrsMul(rFract, pxyz->dzr);

	pxyz->dxr = BrsAdd(pxyz->dxr, pxyzFirst->dxr);
	pxyz->dyr = BrsAdd(pxyz->dyr, pxyzFirst->dyr);
	pxyz->dzr = BrsAdd(pxyz->dzr, pxyzFirst->dzr);

}


/***************************************************************************
	
	Locate the actor (in Brender terms), first adjusting the location.
	Post-impose a rotation looking forward along the route

***************************************************************************/
void ACTR::_PositionBody(XYZ *pxyz)
{
	AssertBaseThis(0);
	AssertVarMem(pxyz);
	XYZ xyz;
	BMAT34 bmat34; 		// Final orientation matrix

	_MatrixRotUpdate(pxyz, &bmat34);
	
	xyz.dxr = BrsAdd(_dxyzRte.dxr, pxyz->dxr);
	xyz.dyr = BrsAdd(_dxyzRte.dyr, pxyz->dyr);
	xyz.dzr = BrsAdd(_dxyzRte.dzr, pxyz->dzr);
	_pbody->LocateOrient(xyz.dxr, xyz.dyr, xyz.dzr, &bmat34);
}


/***************************************************************************
	
	Update the orientation matrices
	_xfrm.bmat34Cur must be kept current

***************************************************************************/
void ACTR::_MatrixRotUpdate(XYZ *pxyz, BMAT34 *pbmat34)
{
	AssertBaseThis(0);
	AssertVarMem(pxyz);
	AssertVarMem(pbmat34);

	RPT rpt;
	BRA xa, ya, za;
	BMAT34 bmat34TS;	// Scaling matrix
	BMAT34 bmat34TR;	// Rotation matrix
	bool fStretchSize;
	AEV *paev;

	_xyzCur = *pxyz;
	fStretchSize = (rOne != _xfrm.aevpull.rScaleX ||
					rOne != _xfrm.aevpull.rScaleY ||
					rOne != _xfrm.aevpull.rScaleZ ||
					rOne != _xfrm.rScaleStep);

	// Set the orientation & rotation to zero-change
	BrMatrix34Identity(&bmat34TS);

	// Post apply the rest (face the camera) orientation
	_ptmpl->GetRestOrien(&xa, &ya, &za);
	BrMatrix34PostRotateX(&bmat34TS, xa);
	BrMatrix34PostRotateY(&bmat34TS, ya);
	BrMatrix34PostRotateZ(&bmat34TS, za);
	
	if (fStretchSize)
		{
		// Apply any current stretching/squashing
		// This must be applied BEFORE rotation to avoid stretching
		// the actor along skewed axes	
		BrMatrix34PostScale(&bmat34TS, _xfrm.aevpull.rScaleX,
			_xfrm.aevpull.rScaleY, _xfrm.aevpull.rScaleZ);

		// Apply any current uniform sizing
		if (_xfrm.rScaleStep != rOne)
			{
			BrMatrix34PostScale(&bmat34TS, _xfrm.rScaleStep,
				_xfrm.rScaleStep, _xfrm.rScaleStep);
			}	
		}

	// bmat34Cur is the all inclusive orientation matrix
	if (_fUseBmat34Cur)	 	
		{
		// Single frame rotate events or static segments
		BrMatrix34Mul(pbmat34, &bmat34TS, &_xfrm.bmat34Cur);	 	// A = B * C
		}
	else
		{
		// Forward-rotate events or non-static segments
		BrMatrix34Copy(&bmat34TR, &_xfrm.bmat34Fwd); 	//copy to bmat34TR

		// Post apply the path orientation
	 	AssertIn(_iaevAddCur, 0, _pggaev->IvMac());
		paev = (AEV *)_pggaev->QvFixedGet(_iaevAddCur);
		_pglrpt->Get(paev->rtel.irpt, &rpt);
#ifdef BUG1899
		if (_ptmpl->FIsTdt() || 
			(_rtelCur.irpt == paev->rtel.irpt && _rtelCur.dwrOffset == rZero))
#else //!BUG1899
		if (_ptmpl->FIsTdt() || paev->nfrm == _nfrmCur || rpt.dwr == rZero)
#endif //!BUG1899
			{
			// Single point	subroute ->
			// Post apply single point orientation to event rotations
			AEVADD aevadd;
			_pggaev->Get(_iaevAddCur, &aevadd);
			BrMatrix34PostRotateX(&bmat34TR, aevadd.xa);
			BrMatrix34PostRotateY(&bmat34TR, aevadd.ya);
			BrMatrix34PostRotateZ(&bmat34TR, aevadd.za);
			// Save the path part of the orientation
			_xfrm.xaPath = aevadd.xa;
			_xfrm.yaPath = aevadd.ya;
			_xfrm.zaPath = aevadd.za;
			}
		else
			{
			// Orient along the route
			_CalcRteOrient(&bmat34TR, &_xfrm.xaPath, &_xfrm.yaPath, &_xfrm.zaPath);
			}

		// Now combine rotation with stretching
		BrMatrix34Mul(pbmat34, &bmat34TS, &bmat34TR);	

		// Note: The result of the entire rotation (including path) needs to
		// be saved - otherwise, the actor will jump when the user first tries
		// to do a tweak-rotate edit.
		BrMatrix34Copy(&_xfrm.bmat34Cur, &bmat34TR);		// Save final all-included rotation matrix
		}
}				


/***************************************************************************
		
	Calculate the Post-imposed rotation tangent to the route
	Calculation based on _iaevCur, _anidCur
	NOTE: *pbmat34 is not pre-initialized

***************************************************************************/
void ACTR::_CalcRteOrient(BMAT34 *pbmat34,
	BRA *pxa, BRA *pya, BRA *pza, ulong *pgrfbra)
{
	AssertBaseThis(0);
	AssertNilOrVarMem(pbmat34);
	AssertNilOrVarMem(pxa);
	AssertNilOrVarMem(pya);
	AssertNilOrVarMem(pza);

	long irptPrev;
	long irptAdd;
	XYZ xyz;
	AEV aev;
	RPT rpt;

	if (pvNil != pxa)
		*pxa = aZero;
	if (pvNil != pya)
		*pya = aZero;					
	if (pvNil != pza)
		*pza = aZero;

	AssertIn(_iaevAddCur, 0, _pggaev->IvMac());
	_pggaev->GetFixed(_iaevAddCur, &aev);
	irptAdd = aev.rtel.irpt;
	irptPrev = (rZero < _rtelCur.dwrOffset || _rtelCur.irpt == aev.rtel.irpt) ?
		_rtelCur.irpt : _rtelCur.irpt - 1;
	
	Assert(rZero == _rtelCur.dwrOffset || irptPrev < _pglrpt->IvMac(),
		"Incorrect offset in path");

	// If at end of subpath, or if on a static segment,
	// retain the orientation the actor last had
	_pglrpt->Get(irptPrev, &rpt);
	if (rZero == rpt.dwr)
		irptPrev--;

	if (irptPrev < irptAdd)
		return;		// Single point path

	//
	// Compute vector xyz as a weighted average of 3 vectors
	//
	xyz.dxr = rZero;
	xyz.dyr = rZero;
	xyz.dzr = rZero;

	_UpdateXyzTan(&xyz, irptPrev, rFour);

	if (irptPrev > irptAdd)
		{
		_UpdateXyzTan(&xyz, irptPrev - 1, rTwo);
		if (irptPrev - 1 > irptAdd)
			{
			_UpdateXyzTan(&xyz, irptPrev - 2, rOne);
			}
		}

	//
	// Apply the rotation determined by vector xyz
	//
	_ApplyRotFromVec(&xyz, pbmat34, pxa, pya, pza, pgrfbra);
	return;
}


/***************************************************************************
	
	Update vector *pxyz by adding to it the normalized vector from nodes
	(irpt to irpt + 1) * weighting factor rw.	The result
	accumulates a weighted average approximation to the tangent to the
	route.
	Optimized for movie PLAYing

***************************************************************************/
void ACTR::_UpdateXyzTan(XYZ *pxyz, long irpt, long rw)
{
	AssertBaseThis(0);

	RPT rpt1;
	RPT rpt2;
	BRS dwr, dxr, dyr, dzr;

	Assert(irpt + 1 < _pglrpt->IvMac(), "irpt out of range");
	
	_pglrpt->Get(irpt, &rpt1);
	_pglrpt->Get(irpt + 1, &rpt2);
	
	dxr = BrsSub(rpt2.xyz.dxr, rpt1.xyz.dxr);
	dyr = BrsSub(rpt2.xyz.dyr, rpt1.xyz.dyr);
	dzr = BrsSub(rpt2.xyz.dzr, rpt1.xyz.dzr);
	
	//
	// Normalize the vector	(norm ~ max + 1/2 min)
	//
	dwr = BrsAbsMax3(dxr, dyr, dzr);
	if (dxr == dwr)
		dwr = BrsAdd(dwr, (BRS)(LwMax(LwAbs((long)dyr), LwAbs((long)dzr)) >> 1));
	else
		{
		if (dyr == dwr)
			dwr = BrsAdd(dwr, (BRS)(LwMax(LwAbs((long)dxr), LwAbs((long)dzr)) >> 1));
		else
			dwr = BrsAdd(dwr, (BRS)(LwMax(LwAbs((long)dxr), LwAbs((long)dyr)) >> 1));
		}			
	dxr = BrsDiv(dxr, dwr);
	dyr = BrsDiv(dyr, dwr);
	dzr = BrsDiv(dzr, dwr);

	dxr = LwBound(dxr, -rOne, rOne);
	dyr = LwBound(dyr, -rOne, rOne);
	dzr = LwBound(dzr, -rOne, rOne);

	//
	// Apply the weight
	//
	if (rw != rOne)
		{
		dxr = BrsMul(rw, dxr);
		dyr = BrsMul(rw, dyr);
		dzr = BrsMul(rw, dzr);
		}

	pxyz->dxr = BrsAdd(pxyz->dxr, dxr);
	pxyz->dyr = BrsAdd(pxyz->dyr, dyr);
	pxyz->dzr = BrsAdd(pxyz->dzr, dzr);
}


/***************************************************************************
	
	Compute the angles of rotation defined by the vector *pxyz
	Apply the rotation to matrix *pbmat34
	Return the angles of rotation in *pxa, *pya, *pza
	Return a mask of the axes rotated

	Note: Banking can be done by approximating the second derivative
	which can be done using additional arguments.

***************************************************************************/
void ACTR::_ApplyRotFromVec(XYZ *pxyz, BMAT34 *pbmat34,
	BRA *pxa, BRA *pya, BRA *pza, ulong *pgrfbra)
{
	AssertBaseThis(0);
	AssertNilOrVarMem(pbmat34);
	AssertVarMem(pxyz);
	AssertNilOrVarMem(pxa);
	AssertNilOrVarMem(pya);
	AssertNilOrVarMem(pza);

	BRS dwr;
	BRS rX, rY, rZ;
	BRFR eX, eY, eZ, eW;
	BRA aRot = aZero;

	if (pvNil != pgrfbra)
		*pgrfbra = 0;
	
	/// Use cached tmpl _grfactn to determine the axes to rotate around
	dwr = BrsAdd(BrsAbsMax3(pxyz->dxr, pxyz->dyr, pxyz->dzr), rOne);

	// BrScalarToFraction required by Brender's BR_ATAN2 limitation
	if (_grfactn & (factnRotateY | factnRotateZ))
		{
		rX = BrsDiv(pxyz->dxr, dwr);
		eX = BrScalarToFraction(rX);
		}
	if (_grfactn & (factnRotateX | factnRotateZ))
		{
		rY = BrsDiv(pxyz->dyr, dwr);
		eY = BrScalarToFraction(rY);
		}	
	if (_grfactn & (factnRotateX | factnRotateY))
		{
		rZ = BrsDiv(pxyz->dzr, dwr);
		eZ = BrScalarToFraction(rZ);
		}
		
	if (_grfactn & factnRotateY)
		{
		if (pxyz->dxr != rZero || pxyz->dzr != rZero)
			{
			aRot = BR_ATAN2(eX, eZ);
			if (pvNil != pbmat34)
				BrMatrix34PostRotateY(pbmat34, aRot);

			if (pvNil != pgrfbra)
				*pgrfbra |= fbraRotateY;
			}
		}
	
	if (pvNil != pya)
		*pya = aRot;

	// Tilt the actor up / down	
	if (_grfactn & factnRotateX)
		{
		if (pxyz->dyr != rZero)
			{
			eW = BrScalarToFraction( BrsAdd(BrsAbs(rX), BrsAbs(rZ)));
			aRot = BR_ATAN2(-eY, eW);
			if (pvNil != pbmat34)
				BrMatrix34PreRotateX(pbmat34, aRot);

			if (pvNil != pgrfbra)
				*pgrfbra |= fbraRotateX;
			}
		}
  	if (pvNil != pxa)
		*pxa = aRot;
  	if (pvNil != pza)
		*pxa = aZero;
}

/***************************************************************************
	
	Get the actor's event lifetime
	Update nfrm values for all events
	Return false if the actor has not yet been placed in time
	pnfrmLast may be pvNil

***************************************************************************/
bool ACTR::FGetLifetime(long *pnfrmFirst, long *pnfrmLast)
{
	AssertThis(0);
	AssertNilOrVarMem(pnfrmFirst);
	AssertNilOrVarMem(pnfrmLast);

	if (klwMax == _nfrmFirst)
		return fFalse;

	if (pvNil != pnfrmFirst)
		*pnfrmFirst = _nfrmFirst;
	
	if (pvNil == pnfrmLast)
		return fTrue;

	if (!_fLifeDirty)
		{
		*pnfrmLast = _nfrmLast;
		Assert(_nfrmLast >= _nfrmFirst, "fLifeDirty incorrect");
		return fTrue;
		}
	
	if (!_FComputeLifetime(pnfrmLast))
	  	PushErc(ercSocBadFrameSlider);

	return fTrue;
}


/***************************************************************************
	
	Compute the actor's event lifetime
	Update nfrm values for all events.	Recording or rerecording pushes
	out later subroutes, whereas recording off screen followed by an earlier
	add does not - note therefore that when roll calling invisible actors,
	the end of the existing subroute must first be truncated.
	
	On a false return, it is possible that the template could not be
	accessed, so the update is incomplete.
	
	pnfrmLast may be pvNil
	NOTE: Now that by spec, edits do not cross subroute boundaries,
	performance optimization can be achieved by storing the
	index if the first aev edited when _fLifeDirty is set.

	Note: There is some amount of redundant checking here now
	that subroutes are now step=0 terminated.  Zero termination implies that
	fEndSubRoute should be false, as actors should not be attempting to move
	past the end of their paths.  Note that FIsStalled() now flags the end
	of step=0 terminated subroutes; stalls mid path should now be impossible
	as by spec, motion fill now inherits the original movement along the path.

***************************************************************************/
bool ACTR::_FComputeLifetime(long *pnfrmLast)
{
	AssertBaseThis(0);
	AssertNilOrVarMem(pnfrmLast);

	long iaev;
	long iaevAdd;
	BRS dwr;
	AEV aev;
	AEV *paev;
	RTEL rtel;
	bool fFreezeThisCel;
	long anid = 0;
	long celn = 0;
	long nfrmPrevSub = 0;
	BRS rScale = rOne;
	BRS dwrStep = rZero;
	bool fEndSubRoute = fFalse;         // moving past end of subroute (see Note above)
	bool fDoneSubRoute = fFalse;        // finished processing events on the current subroute

	// While recording, delay the upcoming lifetime calculation
	if (_fModeRecord)
		{
		if (pnfrmLast != pvNil)
			*pnfrmLast = _nfrmLast;
		_fLifeDirty = fTrue;
		return fTrue;
		}

	// Locate final subroute
	rtel.irpt = 0;
	rtel.dwrOffset = rZero;
	rtel.dnfrm = -1;
	iaevAdd = -1;
	// REVIEW *****(SeanSe): Why not start at the end and go backwards?
	for (iaev = 0; iaev < _pggaev->IvMac(); iaev++)
		{
		paev = (AEV *)_pggaev->QvFixedGet(iaev);
		if (aetAdd == paev->aet)
			iaevAdd = iaev;
		}

	if (iaevAdd < 0)
		{
		_nfrmLast = _nfrmFirst;
		if (pvNil != pnfrmLast)
			*pnfrmLast = _nfrmFirst;
		_fLifeDirty = fFalse;
		return fTrue;
		}

	// Loop through each frame
	long fFrozen = fFalse;
	long iaevNew = 0;
	RTEL rtelOld = rtel;
	rtelOld.dnfrm = rtel.dnfrm - 1;
	for (_nfrmLast = _nfrmFirst ;
		((rtel.irpt != _pglrpt->IvMac()) ||	(iaevNew != _pggaev->IvMac()));
		_nfrmLast++)
		{
		fFreezeThisCel = fFrozen;
		rtelOld = rtel;
		// Find distance to move
		// An aetActn event later in this same frame can modify this.
		if (kdwrNil == dwrStep)
			{
			if (!_ptmpl->FGetDwrActnCel(anid, celn, &dwr))
				{
				_fLifeDirty = fTrue;
				return fFalse;
				}
			}
		else
			{
			dwr = dwrStep;
			}
		dwr = BrsMul(dwr, rScale);
		_AdvanceRtel(dwr, &rtel, iaevNew, _nfrmLast, &fEndSubRoute);

		// Scan all events for this frame
		for (iaev = iaevNew; iaev < _pggaev->IvMac(); iaev++)
			{
			_pggaev->GetFixed(iaev, &aev);

			if ((aev.rtel > rtel) &&
				((aetAdd != aev.aet) ||	
				(_fModeRecord && !fEndSubRoute) ||
				(!fEndSubRoute && !fDoneSubRoute)))	
				{
				// To push out subroutes, do not process add events until finished
				// with the previous subroute
				goto LEndFrame;
				}

			switch(aev.aet)
				{
			case aetAdd:
				fFreezeThisCel = fTrue;
				anid = celn = 0;
				if (aev.nfrm > _nfrmLast)
					{
					goto LEndFrame;
					}
				else
					{
					rtel = aev.rtel;
					fDoneSubRoute = fEndSubRoute = fFalse;
					nfrmPrevSub = _nfrmLast;
					}
				break;

			case aetTweak:
			case aetSnd:
			case aetRem:
			case aetCost:
			case aetRotF:
			case aetRotH:
			case aetPull:
			case aetMove:
				break;

			case aetFreeze:
				long ffriz;
				_pggaev->Get(iaev, &ffriz);
				fFrozen = FPure(ffriz);
				break;

			case aetSize:	// Uniform size transformation
				// Adjust step size : affects actor lifetime
				_pggaev->Get(iaev, &rScale);
				break;

			case aetActn:
				AEVACTN aevactn;
				
				_pggaev->Get(iaev, &aevactn);
				anid = aevactn.anid;
				celn = aevactn.celn;
				fFrozen = fFalse;
				fFreezeThisCel = fTrue;
				break;

			case aetStep:
				_pggaev->Get(iaev, &dwrStep);
				break;

			default:
				Assert(0, "Unknown event type");
				break;
				}

			if (aev.rtel > rtel)
				goto LEndFrame;

			if (aev.nfrm != _nfrmLast)
				{
				aev.nfrm = _nfrmLast;
				_pggaev->PutFixed(iaev, &aev);
				}

			}

LEndFrame:
		
		// Stepsize and lifetime is a function of celn's
		if (!fFreezeThisCel &&
			!(fEndSubRoute && _FIsDoneAevSub(iaev, rtel)))
			{
			celn++;
			}

		// Check for end of subroute or stalled actor (eg breathe in place forever)
#ifdef BUG1960
		// We are not finished updating events in the current subpath until we are either
		// - beyond the subroute (fEndSubRoute)
		// - out of events
		// - stopped with no way to reach more events in the subpath (ie, stalled)
		//   and finished updating later events at this same path location 
		if (fEndSubRoute || 
			(dwrStep == rZero &&
			(iaev == _pggaev->IvMac() || 
			(!(aev.rtel.irpt == rtel.irpt && aev.rtel.dwrOffset == rtel.dwrOffset) && 
			_FIsStalled(iaevNew, &rtel)))))
#else //!BUG1960
		if (fEndSubRoute || ((dwrStep == rZero) &&
			(iaev == _pggaev->IvMac() || _FIsStalled(iaevNew, &rtel))))
#endif //!BUG1960
 			{
			if (!fDoneSubRoute)
				nfrmPrevSub = _nfrmLast;
			fDoneSubRoute = fTrue;

			// Are there more subroutes?
			if (!_FFindNextAevAet(aetAdd, iaev, &iaev))
				{
				// For non-static motions, fEndSubRoute is set once the actor is
				// beyond the end of the subroute.  Adjust back one frame.
				if (fEndSubRoute && (dwrStep != rZero) && (_nfrmLast > _nfrmFirst))
					_nfrmLast--;
			
				if (pvNil != pnfrmLast)
					*pnfrmLast = _nfrmLast;
				_fLifeDirty = fFalse;
				return fTrue;
				}

			// Add events jump in space.  Update rtel
			_pggaev->GetFixed(iaev, &aev);
			rtel = aev.rtel;
			
			// Initialization for _AdvanceRtel()
			anid = celn = 0;
			rtel.dnfrm--;
			}

		iaevNew = iaev;
		}

	Assert(0, "Logic error");
	if (pvNil != pnfrmLast)
		*pnfrmLast = _nfrmLast;
	return fFalse;
}



/***************************************************************************
	
	An actor is said to be stalled if more route exists, the step size is
	ever zero with no subsequent nonzero step size to follow.

	Determine whether an actor is stalled on a subroute
	Input: IaevFirst is	the first event to check on the subroute.
		*prtel is the route point to test for stalling
	Return fFalse if not stalled.
	If stalled, return the last active event.
***************************************************************************/
bool ACTR::_FIsStalled(long iaevFirst, RTEL *prtel, long *piaevLast)
{
	AssertBaseThis(0);
	AssertIn(iaevFirst, 0, _pggaev->IvMac());
   	AssertNilOrVarMem(piaevLast);

	AEV aev;
	long iaev;

	for (iaev = iaevFirst; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if ((aev.rtel.irpt > prtel->irpt) ||
			(aev.rtel.dwrOffset > prtel->dwrOffset))
			{
			if (piaevLast != pvNil)
				{
				*piaevLast = iaev;
				}
			return fTrue;
			}

		if ((aev.rtel.irpt == prtel->irpt) &&
			(aev.rtel.dwrOffset == prtel->dwrOffset))
			{
			if (aetStep == aev.aet)
				{
				BRS dwrAev;
				_pggaev->Get(iaev, &dwrAev);
				if (rZero != dwrAev)
					return fFalse;
				}
			}
		}
	return fFalse;	
 }


/***************************************************************************
	
	Has the mouse been down long enough or moved far enough to qualify this
	as a valid recording session (vs merely motion filling)?

	NOTE:  ** If sufficient time has not elapsed, intended motion fill will
	inadvertently delete the remaining subroute	is record is selected
	instead of rerecord.

	NOTE:  ** This requires that SetTsInsert() have been called on mouse
	down to initialize the timing of the recording session

***************************************************************************/
bool ACTR::FIsRecordValid(BRS dxr, BRS dyr, BRS dzr, ulong tsCurrent)
{	
	AssertThis(0);

	if (_fModeRecord)
		return fTrue;

	BRS dwrMouse;

	// Is this truly a motion fill or is there sufficient time or
	// distance travelled to make this an authentic route record?	
	dwrMouse = BR_LENGTH3(dxr, dyr, dzr);
							
	if ((dwrMouse < kdwrThreshRte) && ((tsCurrent - _tsInsert) < kdtsThreshRte))
		return fFalse;
	
	return fTrue;

}


/***************************************************************************
	
	Begin a new route
	fReplace = ftrue if recording, fFalse if rerecording
	On failure, actor is restored from pactrRestore

***************************************************************************/
bool ACTR::FBeginRecord(ulong tsCurrent, bool fReplace, PACTR pactrRestore)
{	
	AssertThis(0);
	Assert(tsCurrent >= 0, "Invalid type");

	RPT rpt;
	AEV aev;
	long iaev;
	long nfrmAdd;
	long nfrmPrev;
	bool fClosestSubrte = fTrue;

	_pglrpt->Get(_rtelCur.irpt, &rpt);
	_fPathInserted = fFalse;
	
	// Do not rejoin to a one point offstage stub.
	_fRejoin = !fReplace && !(rZero == rpt.dwr);		
	if (_fRejoin)
		{
		long iaev;
		AEV aev;
		
		long irptNext = _rtelCur.irpt + 1;
		
		for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
			{
			_pggaev->GetFixed(iaev, &aev);
			if (irptNext < aev.rtel.irpt)
				break;
			if (aetRem == aev.aet)
				{
				_fRejoin = fFalse;
				_RemoveAev(iaev);
				iaev--;
				_AdjustAevForRteDel(irptNext, _iaevCur);
				_pglrpt->Delete(irptNext);
				}
			}
		}

	// Recording must advance from the current point -> potentially
	// inserting the current point
	if (rZero != _rtelCur.dwrOffset)
		{
		BRS dwrCur = _rtelCur.dwrOffset;
		_GetXyzFromRtel(&_rtelCur, &rpt.xyz);
		_rtelCur.irpt++;
		_rtelCur.dwrOffset = rZero;
		_rtelCur.dnfrm = 0;
		if (!_FInsertGgRpt(_rtelCur.irpt, &rpt, dwrCur))
			goto LFail;
		_GetXyzFromRtel(&_rtelCur, &_xyzCur);	
		_AdjustAevForRteIns(_rtelCur.irpt, 0);
		}

	// Determine the gaps between subroutes
	_dnfrmGap = 0;
	iaev = _iaevCur;
	if (_fLifeDirty)
		{
		// Validate future nfrm values
		if (!_FComputeLifetime())
	  		{
	  		PushErc(ercSocBadFrameSlider);
			goto LFail;
			}
		}
		
	for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.aet != aetAdd)
			continue;
		nfrmAdd = aev.nfrm;
		if (fClosestSubrte)
			{
			nfrmPrev = _nfrmCur;
			fClosestSubrte = fFalse;
			}
		else
			{
			_pggaev->GetFixed(iaev-1, &aev);
			nfrmPrev = aev.nfrm;
			}
		_dnfrmGap -= (nfrmAdd - nfrmPrev);
		}

 	_rtelInsert = _rtelCur;
	_fModeRecord = fTrue;
	_dxyzRaw.dxr = rZero;
	_dxyzRaw.dyr = rZero;
	_dxyzRaw.dzr = rZero;
	return fTrue;

LFail:
	_RestoreFromUndo(pactrRestore);
	return fFalse;
}


/***************************************************************************
	
	Add to a new route

	On Input:
	grfmaf flags the Frozen/unfrozen state.
	*pfStepFrm returns whether the frame was advanced
	*pfStepRte returns whether the route was extended.

	Note: During recording, dwr = rZero marks the end of the new subroute
	NOTE: On failure, actor is restored from pactrRestore

***************************************************************************/
bool ACTR::FRecordMove(BRS dxr, BRS dyr, BRS dzr, ulong grfmaf,	
	ulong tsCurrent, bool *pfStepFrm, bool *pfStepRte, PACTR pactrRestore)
{
	AssertThis(0);
	AssertVarMem(pfStepFrm);
	AssertVarMem(pfStepRte);

	RPT rptCur;
	XYZ xyzMouse;
	XYZ dxyzT;
	BRS dwrCur;
	RPT rptNew;
	BRS rFractMoved;
	BRS dwrMouse;

	Assert(_fOnStage, "Recording an actor that wasn't selectable??");
	if (!_fModeRecord)
		return fTrue; 	// Potential client call on motion fill

	if (!_pggaev->FEnsureSpace( 2, kcbVarStep + kcbVarFreeze, fgrpNil) ||
		!_pglrpt->FEnsureSpace(1, fgrpNil))
		{
		goto LFail;
		}

	// Smooth the unpruned raw mouse movement *before* we create the path
	dxyzT.dxr = dxr;	  	
	dxyzT.dyr = dyr;
	dxyzT.dzr = dzr;
	dxr = BR_CONST_DIV(BrsAdd(_dxyzRaw.dxr, BR_CONST_MUL(2, dxr)), 3);
	dyr = BR_CONST_DIV(BrsAdd(_dxyzRaw.dyr, BR_CONST_MUL(2, dyr)), 3);
	dzr = BR_CONST_DIV(BrsAdd(_dxyzRaw.dzr, BR_CONST_MUL(2, dzr)), 3);

	if (_fFrozen && !(grfmaf & fmafFreeze))
		AssertDo(_FUnfreeze(), "Ensurespace should have ensured space");
	if (!_fFrozen && (grfmaf & fmafFreeze))
		AssertDo(_FFreeze(), "Ensurespace should have ensured space");

	if (pvNil != pfStepFrm)
		*pfStepFrm = fFalse;
	if (pvNil != pfStepRte)
		*pfStepRte = fFalse;

	// Update distance from previously current point
	Assert( _pglrpt->IvMac() > 0, "Illegal empty path");
		
	// In record mode, know prev point on a route Node
	// Determine if threshhold distance has been advanced
	_pglrpt->Get(_rtelCur.irpt, &rptCur);		
	xyzMouse.dxr = BrsAdd(dxr, rptCur.xyz.dxr);
	xyzMouse.dyr = BrsAdd(dyr, rptCur.xyz.dyr);
	xyzMouse.dzr = BrsAdd(dzr, rptCur.xyz.dzr);
	dwrMouse = BR_LENGTH3(dxr, dyr, dzr);

	if (!_FGetDwrRecord(&dwrCur))
		goto LFail;

	if (dwrMouse < dwrCur)
		{
		if ((tsCurrent - _tsInsert) < kdtsThreshRte)
			{
			//Insufficient length and insufficient time to record a point
			return fTrue;
			}

		if (rZero != _dwrStep)
			{
			AssertDo(FSetStep(rZero), "EnsureSpace insufficient");
			}

		_fPathInserted = fTrue;
		if (pvNil != pfStepFrm)
			{
			*pfStepFrm = fTrue;
			}
		if (!_pscen->FGotoFrm(_pscen->Nfrm() + 1))
			goto LFail;
		return fTrue;
		}
	
	_tsInsert = tsCurrent;
	_dxyzRaw = dxyzT;

	//
	// Insert new point.  (Overlays last point if offstage)
	// Length is sufficient.  Truncate to correct step size
	//
	rFractMoved = BrsDiv(dwrCur, dwrMouse);
	Assert (rZero != dwrCur, "Zero length step illegal in FRecordMove");					

	_GetXyzOnLine(&rptCur.xyz, &xyzMouse, rFractMoved, &rptNew.xyz);
	rptNew.dwr = rZero;
	
	// Adjust the point for ground zero.  The rule is if the mouse
	// is decreasing from above to below ground, stop at ground level
	// The original dwr is used *by design*
	if ((grfmaf & fmafGround) &&
		(BrsAdd(rptCur.xyz.dyr, _dxyzRte.dyr) >= rZero) &&
		(BrsAdd(rptNew.xyz.dyr, _dxyzRte.dyr) < rZero))
		{
		rptNew.xyz.dyr = -_dxyzRte.dyr;
		}
	
	if (!_pglrpt->FInsert(1 + _rtelCur.irpt, &rptNew))
		goto LFail;

	if (kdwrNil != _dwrStep)
		{
		AssertDo(FSetStep(kdwrNil), "EnsureSpace insufficient");
		}

	// Reset distance in previous xyz entry
	rptCur.dwr = dwrCur;
	Assert(dwrCur != rZero, "Bug in InsertRoute");
	_pglrpt->Put(_rtelCur.irpt, &rptCur);

	_AdjustAevForRteIns(1 + _rtelCur.irpt, _iaevCur);
	_fPathInserted = fTrue;
		
	if (pvNil != pfStepFrm)
		*pfStepFrm = fTrue;
	if (pvNil != pfStepRte)
		*pfStepRte = fTrue;


	_dnfrmGap++;
	if (_dnfrmGap > 0)
		_nfrmLast++;

	if (_pscen->FGotoFrm(_pscen->Nfrm() + 1))
		return fTrue;

LFail:
	_RestoreFromUndo(pactrRestore);
	return fFalse;
}

/***************************************************************************
	
	End recording a new route or route section	(mouse up)
	Insert a freeze event if at the end of the subroute.

	Input: fReplace is meaningful only if recording or rerecording (versus
	motion fill.  _fModeRecord is false on motion fill).
	fReplace = fTrue if recording, ie, if replacing the remaining subroute

	Path splicing rule:  Never rejoin across Add events
	Always reconnect to the existing subroute at the closest point which
	occurs later in the route than where the initial re-recording began.

	NOTE** Actn, Tweak, Freeze, Step & Rem events are deleted from the
	section of the route being replaced.  Other events are pushed up to
	the join point (if rejoining) or to the next dnfrm of the current point,
	if not rejoining.

	On Failure: The actor is restored from pactrRestore
***************************************************************************/ 
// REVIEW *****(*****): Ver 2.0 As motion fill is self sufficient, 
// 		FEndRecord() should be able to exit immediately on !_fModeRecord. 
//   	Also, pactrRestore should be state var _pactrRecordRestore.
bool ACTR::FEndRecord(bool fReplace, PACTR pactrRestore)
{
	AssertThis(0);

	AEV aev;
	BRS dwr;
	long iaev;
	long irpt;
	RPT rpt;
	RPT rptJoin;
	RPT rptCur;
	BRS dwrMin = kdwrMax;
	RTEL rtelJoin = _rtelCur;
	bool fJoin = fFalse;
	long irptLim = _pglrpt->IvMac();
	long iaevJoinFirst = _iaevCur;
	long iaevNew;

	// Determine whether to rejoin to the path
//REVIEW (*****): Can we assert _fOnStage?
	if (_fModeRecord && (!_fOnStage || fReplace))
		{
		// Delete remnant path
		_DeleteFwdCore(fFalse, pvNil, _iaevCur);
		if (!_FFreeze())
			goto LFail;
		if (!FSetStep(rZero)) 	
			goto LFail;
		goto LEndRecord;
		}

	//
	// Motion Fill or Path-rejoining
	//
	// _fModeRecord == fFalse on motion fill
	if (!_fModeRecord || !_fPathInserted || !_fOnStage)
		{
		// Set Action necessarily destroys end of path freeze events
		// Reinsert an end of path freeze event	if at end of subroute
		if (_FIsDoneAevSub(_iaevCur, _rtelCur))
			{
			if (!_FFreeze())
				goto LFail;
			}
		goto LEndRecord;	//Nothing inserted
		}
	
	// On !fReplace, force continuation to remainder of route.
	if (rZero == _dwrStep)	
		{
		if (!FSetStep(kdwrNil))
			goto LFail;
		}

	if (!_fRejoin)
		{
		// Gather the events at later time for the current path point
		// Equivalent to setting the join time ahead
		rtelJoin.dnfrm = 1;
		}
	else
		{
		Assert((_rtelCur.irpt < _pglrpt->IvMac() - 1),
			"Not enough points to rejoin");

		for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
			{
			_pggaev->GetFixed(iaev, &aev);
			if (aev.aet == aetAdd)
				{
				irptLim = aev.rtel.irpt;
				break;
				}
			}

		// Locate the Join node
		rtelJoin.irpt = _rtelCur.irpt + 1;
		rtelJoin.dwrOffset = rZero;
		rtelJoin.dnfrm = 0;
		for (irpt = 1 + _rtelCur.irpt; irpt < irptLim; irpt++)
			{
			_pglrpt->Get(irpt, &rpt);
			dwr = BR_LENGTH3(BrsSub(_xyzCur.dxr, rpt.xyz.dxr),
					BrsSub(_xyzCur.dyr, rpt.xyz.dyr),
					BrsSub(_xyzCur.dzr, rpt.xyz.dzr));
			if (dwr < dwrMin)
				{
				dwrMin = dwr;
				rtelJoin.irpt = irpt;
				}

			// Do not join across subroutes
			if (rZero == rpt.dwr)
				break;
			}		

		Assert(rtelJoin > _rtelCur, "Invalid Join point");
		Assert(rZero == _rtelCur.dwrOffset, "_rtelCur invalid");

		// Update the dwr of the most recently recorded point to be
		// the length to the join point on the path
		// NOTE: This cannot be zero (it would signal end of path)
		_pglrpt->Get(_rtelCur.irpt, &rptCur);
		_pglrpt->Get(rtelJoin.irpt, &rptJoin);
		rptCur.dwr = BR_LENGTH3(BrsSub(_xyzCur.dxr, rptJoin.xyz.dxr),
				BrsSub(_xyzCur.dyr, rptJoin.xyz.dyr),
				BrsSub(_xyzCur.dzr, rptJoin.xyz.dzr));

		if (rZero == rptCur.dwr)
			{
			//Prevent pathological end-of-route case
			rptCur.dwr = rEps;
			}	
		_pglrpt->Put(_rtelCur.irpt, &rptCur);
		}

	//
	// Note: Motion fill exited earlier.  This is rejoin-recording only (which
	// may or may not actually have a path point to rejoin to).
	// Move displaced events forward to the join point
	// Spec: Replace is viewed in UI as Action replace, not just path replace.	->
	// Delete intervening action, tweak, freeze, step and rem events.
	//
	for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);

#ifdef BUG1961
		// If we were rejoin-recording from a static segment, there is
		// no point to rejoin to (-> _fRejoin is fFalse).  So gather
		// events up to but not beyond the next add event.
		if (aev.aet == aetAdd || 
			(_fRejoin && aev.rtel >= rtelJoin))
#else
		if (_fRejoin && aev.rtel >= rtelJoin)
#endif //!BUG1961
			{
			break;
			}
	
		if (aev.rtel > _rtelInsert)
			{
			if (aev.aet == aetTweak ||
				aev.aet == aetStep ||
				aev.aet == aetRem ||
				aev.aet == aetActn ||
				aev.aet == aetFreeze)
				{
				_RemoveAev(iaev);
				iaev--;
				continue;
				}
			else
				{
				aev.rtel = rtelJoin;
				_pggaev->PutFixed(iaev, &aev);
				_MergeAev(iaevJoinFirst, iaev, &iaevNew);
				 if (iaevNew < iaev)
					iaev--;
				}
			}
		}

	if (_fRejoin)
		{
		// Delete the path segment before the join point
		for (irpt = rtelJoin.irpt - 1; irpt > _rtelCur.irpt; irpt--)
			{
			_AdjustAevForRteDel(irpt, _iaevCur);
			_pglrpt->Delete(irpt);
			}

		// Spec: Do an action fill forward.
		_PrepActnFill(iaevJoinFirst, _anidCur, _anidCur,
				faetTweak | faetFreeze | faetActn);
		}

#ifdef BUG1961
	if (!fReplace && !_fRejoin)
		{
		if (!_FFreeze())
			goto LFail;
		if (!FSetStep(rZero)) 	
			goto LFail;
		}
#endif //BUG1961

LEndRecord:
	_fLifeDirty = fTrue;
#ifdef BUG1973
	_fRejoin = fFalse;
#endif // BUG1973
	_fModeRecord = fFalse;
	_pscen->MarkDirty();
	_pscen->InvalFrmRange();
	AssertThis(fobjAssertFull);
	return fTrue;

LFail:
	_RestoreFromUndo(pactrRestore);
#ifdef BUG1973
	_fRejoin = fFalse;
#endif
	_fModeRecord = fFalse;		// Redundant safety net
	return fFalse;
}


/***************************************************************************
	
	Delete the path and events beyond this frame - and -
	Insert a Freeze event to terminate the truncated event list

***************************************************************************/
void ACTR::DeleteFwdCore(bool fDeleteAll, bool *pfAlive, long iaevCur)
{
	AssertThis(0);

	bool fAlive;
	long iaev;
	AEV aev;

	_DeleteFwdCore(fDeleteAll, &fAlive, iaevCur);

	if (fAlive && _nfrmCur >= _nfrmFirst)
		{
		// If no space exists, truncation isn't changing a thing
		if (_pggaev->FEnsureSpace( 2, kcbVarFreeze + kcbVarStep, fgrpNil ))
			{
#ifdef BUG1932
			if (_iaevAddCur < 0 || _pggaev->IvMac() == 0)
				{
				// Safety net code : don't add events if bug exists
				Bug("Invalid fAlive value");
				goto LEnd;
				}
#ifdef DEBUG
			_pggaev->GetFixed(_iaevAddCur, &aev);
			Assert(aetAdd == aev.aet, "Should be an add event at _iaevAddCur");
#endif //DEBUG
			if (_fOnStage)
				{
				// Insert terminating events ONLY if the actor is shown, not hidden
				AssertDo(_FFreeze(), "Expected freeze event to succeed");
				AssertDo(FSetStep(rZero), "Expected set step event to succeed");
				}
			else
				goto LEnd;
#else //BUG1932
				AssertDo(_FFreeze(), "Expected freeze event to succeed");
				AssertDo(FSetStep(rZero), "Expected set step event to succeed");
#endif //!BUG1932
			// By construct, step events SET the location of display
			// Tweak events override the last step event.
			for (iaev = _iaevCur - 1; iaev > 0; iaev--)
				{
				_pggaev->GetFixed(iaev, &aev);
				if (aev.rtel != _rtelCur)
					break;
				if (aev.aet == aetTweak)
					{
					_pggaev->Swap(iaev, _iaevCur - 1);
					break;
					}	
				}	
			}
		}

LEnd:
	if (pvNil != pfAlive)
		*pfAlive = fAlive;
}


/***************************************************************************
	
	Delete the path and events beyond this frame
	**NOTE:  This does not send the actor offstage.	See FRemFromStageCore.
	NOTE: While _fLifeDirty flags needed lifetime recomputation, scene's
	InvalFrmRange() is _not_ called due to the special artifact of the
	frame slider being spec'd to show extensions but not reductions.

	On Input:
		fDeleteAll specifies full route vs subroute deletion
		If one frame was backed up before the delete, iaevCur is the
		current event before the backup (else ivNil).
	Returns *pfAlive = false if all events and route for this actor
	have been deleted

***************************************************************************/
void ACTR::_DeleteFwdCore(bool fDeleteAll, bool *pfAlive, long iaevCur)
{
	AssertThis(0);
	
	long iaev, iaevDelLim;
	long irpt, irptDelLim;
	long irptDelFirst;
	RPT rpt;
	AEV *paev;

	if (ivNil == iaevCur)
		iaevCur = _iaevCur;

#ifndef BUG1870
	// Delete this section of code: Placement orientation should no
	// 		longer be overwritten.
	// Preserve the current orientation	
	if (_iaevAddCur >= 0)
		{
		// If we are reducing the current subroute to a single point,
		// it is necessary to store the current orientation for wysiwyg
		paev = (AEV *)_pggaev->QvFixedGet(_iaevAddCur);
		if (paev->nfrm == _nfrmCur)
			_SaveCurPathOrien();
		}
#endif //!BUG1870
	
	// If !fDeleteAll & only one subroute left, redefine fDeleteAll
	iaevDelLim = _pggaev->IvMac();
	irptDelLim = _pglrpt->IvMac();	
	if (!fDeleteAll)
		{
		fDeleteAll = fTrue;
		for (iaev = iaevCur; iaev < _pggaev->IvMac(); iaev++)
			{
			paev = (AEV *)_pggaev->QvFixedGet(iaev);
			if (aetAdd == paev->aet)
				{
				fDeleteAll = fFalse;
				irptDelLim = paev->rtel.irpt;
				iaevDelLim = iaev;
				break;
				}
			}
		}

	// Remove events beyond the current one
	// excluding the last freeze event
	if (0 < iaevDelLim)
		{
		for (iaev = iaevDelLim - 1; iaev >= _iaevCur; iaev--)
			{
			_RemoveAev(iaev);
			_fLifeDirty = fTrue;
			}
		}
	
	// Prune the corresponding route
	irptDelFirst = _rtelCur.irpt + 1;
	if (0 < irptDelLim)
		{
		// Note: Last remaining point on path is dependent on the event stream
		if (_iaevCur <= 0 && fDeleteAll)
			{
			_pglrpt->FSetIvMac(0);
			_fLifeDirty = fTrue;
			_rtelCur.irpt = -1;
			goto LDone;
			}
		else
			{
			// Delete the current path node only if no other events
			// use the same node
			if (_rtelCur.dwrOffset > rZero)
				{
				// Shorten the distance between the last two nodes
				_TruncateSubRte(irptDelLim);
				irptDelFirst++;
				}
			else
				{
				// Delete remaining subroute
				for (irpt = irptDelLim - 1; irpt >= irptDelFirst; irpt--)
					{
					_AdjustAevForRteDel(irpt, 0);
					_pglrpt->Delete(irpt);
					_fLifeDirty = fTrue;
					}
				}
			}		

		// Adjust the distance on the tail point
		if (_rtelCur.irpt >= 0)
			{
			Assert(_pglrpt->IvMac() > 0, "Logic Error");
			_pglrpt->Get(_rtelCur.irpt, &rpt);
			rpt.dwr = rZero;
			_pglrpt->Put(_rtelCur.irpt, &rpt);
			}
		}

LDone:
	_pscen->MarkDirty();
	if (_nfrmCur < _nfrmFirst)
		{
		if (fDeleteAll)
			{
			if (pvNil != pfAlive)
				*pfAlive = fFalse;
			_InitState();
			return;
			}
		else if (_pggaev->IvMac() > 0)	
			{
			paev = (AEV *)_pggaev->QvFixedGet(0);
			_nfrmFirst = paev->nfrm;
			Assert(aetAdd == paev->aet, "Bug in ACTR::DeleteFwdCore");
			}
		}

	if (pvNil != pfAlive)
		*pfAlive = fTrue;

	return;
}


/***************************************************************************
	
	Save the path specific part of the current orientation

***************************************************************************/
void ACTR::_SaveCurPathOrien(void)
{
	AssertBaseThis(0);

	RPT rpt;
	AEV aev;
	ulong grfbra = 0;

	if (_iaevAddCur >= 0 && !_ptmpl->FIsTdt())
		{	
		_pggaev->GetFixed(_iaevAddCur, &aev);
		_pglrpt->Get(aev.rtel.irpt, &rpt);
		if (rZero != rpt.dwr)		// ie, non-static path segment
			{
			// _xfrm.waPath is current and CalcRteOrient() cannot be called
			// in all cases
			SetAddOrient(_xfrm.xaPath, _xfrm.yaPath, _xfrm.zaPath, grfbra);
			}
		}
}

	
/***************************************************************************
	
	Delete the path and events prior to this frame

***************************************************************************/
void ACTR::DeleteBackCore(bool *pfAlive)
{
	AssertThis(0);
	AssertNilOrVarMem(pfAlive);

	long iaev;
	long iaevNew;
	AEV aev;
	BRS dwrOffsetT;
	BRS dwrNew;
	BRS dwrOld;
	RPT rptOld;
	long dnrpt;

	// Nop if not yet at first frame
	if (_nfrmCur <= _nfrmFirst)
		{
		if (pvNil != pfAlive)
			*pfAlive = FPure(_pggaev->IvMac() > 0);
		return;
		}

	// We need to update the nfrm field in aetAdd events in order to update
	// _nfrmFirst below.
	if (_fLifeDirty && !_FComputeLifetime())
		{
		// Below the new _nfrmFirst will be wrong, but that means the actor
		// will appear in the wrong frame (iff _fLifeDirty and the next subroute
		// got moved).  Not much we can do about it.
	  	PushErc(ercSocBadFrameSlider);
		}

	// If the current point is between nodes, a node will
	// be inserted as the new first node of the subroute.
	// Compute the dwr of the new node.
	_pglrpt->Get(_rtelCur.irpt, &rptOld);
	dwrOld = rptOld.dwr;
	dwrNew = BrsSub(dwrOld, _rtelCur.dwrOffset);

	// Preserve the current orientation
	_SaveCurPathOrien();

	// Adjust offsets for events occurring at points between the
	// previous and next nodes
	for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.rtel.irpt > _rtelCur.irpt)
			break;

		if (aev.rtel.dwrOffset == _rtelCur.dwrOffset)
			{
			aev.rtel.dwrOffset = rZero;
			if (aev.rtel.dnfrm < _rtelCur.dnfrm)
				{
				Bug("Event list out of order (dnfrm error)");
				aev.rtel.dnfrm = 0;
				}
			else
				{
				aev.rtel.dnfrm -= _rtelCur.dnfrm;
				}
			}

		if (dwrNew == rZero)
			aev.rtel.dwrOffset = rZero;
		else
			{
			// Set the event's dwrOffset to the correct
			// part of the new dwr
			dwrOffsetT = BrsSub(aev.rtel.dwrOffset, _rtelCur.dwrOffset);
			aev.rtel.dwrOffset = dwrOffsetT;
			}
		_pggaev->PutFixed(iaev, &aev);
		}

	//
	// Alter the route's new first point to be _rtelCur.
	// with the correct new dwr.  Then update _rtelCur.
	//
	_GetXyzFromRtel(&_rtelCur, &rptOld.xyz);
	rptOld.dwr = dwrNew;
	_pglrpt->Put(_rtelCur.irpt, &rptOld);
	dnrpt = _rtelCur.irpt;
	_rtelCur.dnfrm = 0;												
	_rtelCur.dwrOffset = rZero;
	_rtelCur.irpt = 0;

	//
	// Merge the earlier events
	// Update the necessary state variables
	//
	for (iaev = 0; iaev < _iaevCur; iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (!_fOnStage || iaev < _iaevAddCur)
			{
			_RemoveAev(iaev);
			iaev--;
			continue;
			}

		switch(aev.aet)
			{
		case aetRotH:
#ifdef BUG1870
			// Supercede orient-rotate with current rotation
			// 		so that static segment orientation will be preserved
			// Note: events prior to _iaevFrmMin need to be included
			// 		due to orient-rotations lasting the lifetime of static
			// 		segments.
			_pggaev->Put(iaev, &_xfrm.bmat34Cur);
			goto LDefault;
#endif //BUG1870
   		case aetTweak:
		case aetSnd:
			if (iaev < _iaevFrmMin)
				{
				_RemoveAev(iaev);
				iaev--;
				break;
				}
			if (iaev > 0)
				goto LDefault;
			break;

		case aetActn:
			AEVACTN aevactn;
			_pggaev->Get(iaev, &aevactn);
			aevactn.celn = _celnCur;
			_pggaev->Put(iaev, &aevactn);
			goto LDefault;

		case aetRem:
			_RemoveAev(iaev);
			iaev--;
			Bug("Offstage case should be already handled");
			break;

		default:
LDefault:
			aev.rtel = _rtelCur;
			aev.nfrm = _nfrmCur;
			_pggaev->PutFixed(iaev, &aev);
			if (iaev > 0)
				_MergeAev(0, iaev, &iaevNew);
			else
				iaevNew = iaev;

			if (iaevNew < iaev)
				iaev--;
			if (aev.aet == aetAdd)
				_iaevAddCur = iaevNew;
			if (aev.aet == aetActn)
				_iaevActnCur = iaevNew;		
			break;
			}
		}

	//
	// Adjust the irpt's of the later events
	// Adjust the absolute beginning frame number
	// Delete the first section of the route
	// NOTE: There is no translation in time
	//
	if (dnrpt > 0)
		{
		// Delete the first section of the route
		// If offstage, also delete the current point
#ifdef BUG1866
		if (!_fOnStage)
			dnrpt++;
#endif //BUG1866
		for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
			{
			_pggaev->GetFixed(iaev, &aev);
			aev.rtel.irpt -= dnrpt;
			_pggaev->PutFixed(iaev, &aev);
			}
#ifndef BUG1866
		if (!_fOnStage)
			dnrpt++;
#endif //!BUG1866
		_pglrpt->Delete(0, dnrpt);
		}

	if (pvNil != pfAlive)
		*pfAlive = FPure(_pggaev->IvMac() > 0);

	if (_pggaev->IvMac() == 0)
		{
		_InitState();
		}
	else
		{
		// Adjust remaining state variables
		_pggaev->GetFixed(0, &aev);
		Assert(aev.aet == aetAdd, "An aetAdd event should be the first event");
		_nfrmFirst = aev.nfrm;
		_fLifeDirty = fTrue;
		_iaevFrmMin = 0;
		}
	
	if (!_FComputeLifetime())
	  	PushErc(ercSocBadFrameSlider);
	_pscen->InvalFrmRange();
	
	return;
}


/***************************************************************************
	
	Truncate the last linear section of the subroute.
	Make the current point a node.
	
	Adjust event entries from the current rtel through events located
	at irptDelLim.
	Note:  This truncates the route only, not the event list.

***************************************************************************/
void ACTR::_TruncateSubRte(long irptDelLim)
{
	AssertBaseThis(0);
	
	AEV aev;
	long iaev;
	RPT rptNode1;
	RPT rptNode2;
	long iaevLim = _iaevCur;
	long irpt = _rtelCur.irpt;

	if (_rtelCur.dwrOffset == rZero)
		return;
				
	// Store the new length between nodes
	_pglrpt->Get(irpt, &rptNode1);
	rptNode1.dwr = _rtelCur.dwrOffset;
	_pglrpt->Put(irpt, &rptNode1);
	
	// Move node 2 rather than inserting a node
	_GetXyzFromRtel(&_rtelCur, &rptNode2.xyz);
	rptNode2.dwr = rZero;
	Assert(irpt + 1 < _pglrpt->IvMac(), "Error in truncation");
	_pglrpt->Put(irpt + 1, &rptNode2);

	// Update _rtelCur
	_rtelCur.dnfrm = 0;
	_rtelCur.dwrOffset = rZero;
	_rtelCur.irpt++;
	
	// Update aev's rtel's
	for (iaev = _iaevCur - 1; iaev >= 0; iaev--)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.rtel.irpt < irpt)
			break;
		if (aev.rtel.dwrOffset >= rptNode1.dwr)
			{
			aev.rtel.dwrOffset = rZero;
			aev.rtel.irpt++;
			_pggaev->PutFixed(iaev, &aev);
			}
		}

	// Delete remaining subroute
	for (irpt = irptDelLim - 1; irpt > _rtelCur.irpt; irpt--)
		{
		_AdjustAevForRteDel(irpt, 0);
		_pglrpt->Delete(irpt);
		_fLifeDirty = fTrue;
		}	
}


/***************************************************************************
	
	Is the mouse point within this actor.
			
***************************************************************************/
bool ACTR::FPtIn(long xp, long yp, long *pibset)
{
	AssertThis(0);
	AssertVarMem(pibset);

	return _pbody->FPtInBody(xp,  yp, pibset);
}

/***************************************************************************
	
	FMustRender.  Optimizaton: Is Rerendering necessary between _nfrmCur and
	nfrmLast, exclusive of the current frame?
	Returns fFalse only when rerendering *known* to be unnecessary.
	Otherwise returns fTrue.
		
***************************************************************************/
bool ACTR::FMustRender(long nfrmRenderLast)
{
	AssertThis(0);
	Assert(nfrmRenderLast >= _nfrmCur, "Invalid argument to FMustRender");
	Assert(!_fLifeDirty, "FMustRender was called when nfrm values were invalid");

	AEV *paev;
	long iaev;

	if (nfrmRenderLast == _nfrmCur)
		goto LStill;
	if (nfrmRenderLast < _nfrmFirst)
		goto LStill;
	if (_nfrmCur > _nfrmLast)
		goto LStill;
	if (_pglrpt->IvMac() == 0)
		goto LStill;

	// Intervening events?	Sounds don't affect rendering.
	if (_iaevCur < _pggaev->IvMac())
		{
		paev = (AEV *)_pggaev->QvFixedGet(_iaevCur);
		if (paev->nfrm <= nfrmRenderLast)
			{
			for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
				{
				paev = (AEV *)_pggaev->QvFixedGet(iaev);
				if (paev->nfrm > nfrmRenderLast)
					break;	 	
				if (aetSnd == paev->aet)
					continue;
				if (paev->nfrm < _nfrmLast)
					goto LMoving;
				// Freeze and step events affect future frames
				if (aetFreeze != paev->aet && aetStep != paev->aet)
					goto LMoving;
				}
			}
		}
	
	if (_dwrStep != rZero)
		goto LMoving;					// moving along path
	
	// Not advancing, but moving in place?
	if (_fFrozen || _ccelCur == 1)
		goto LStill;
LMoving:
	return fTrue;

LStill:
	return fFalse;
}


/***************************************************************************
	
	Get actor name
			
***************************************************************************/
void ACTR::GetName(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	if (!_pscen->Pmvie()->FGetName(_arid, pstn))
		{
		Ptmpl()->GetName(pstn);
		}
}

/***************************************************************************
	
	Get actor world coordinates
			
***************************************************************************/
void ACTR::GetXyzWorld(BRS *pxr, BRS *pyr, BRS *pzr)
{
	AssertThis(0);
	AssertNilOrVarMem(pxr);
	AssertNilOrVarMem(pyr);
	AssertNilOrVarMem(pzr);

	XYZ xyz;

	_GetXyzFromRtel(&_rtelCur, &xyz);

	if (pvNil != pxr)
		*pxr = BrsAdd(xyz.dxr, _dxyzRte.dxr);
	if (pvNil != pyr)
		*pyr = BrsAdd(xyz.dyr, _dxyzRte.dyr);
	if (pvNil != pzr)
		*pzr = BrsAdd(xyz.dzr, _dxyzRte.dyr);

}



/***************************************************************************
	
	Hilite the actor
			
***************************************************************************/
void ACTR::Hilite(void)
{
	AssertThis(0);

	BODY::SetHiliteColor(_fTimeFrozen ? kiclrTimeFreezeHilite : kiclrNormalHilite);
	_pbody->Hilite();
}

/***************************************************************************
	
	Change this actor's template.  This gets called if the actor is based
	on a 3-D Text template and the text gets edited.  In addition to
	changing the template itself, this function updates _pbody to conform
	to the new template, and removes all costume events on body part sets
	that no longer exist.  For instance, if a three-letter text actor was
	changed to two letters, all costume events on the third letter are
	deleted.
			
***************************************************************************/
bool ACTR::FChangeTagTmpl(TAG *ptagTmplNew)
{
	AssertThis(0);
	AssertVarMem(ptagTmplNew);

	PTMPL ptmpl;
	long cbsetNew;
	long iaev;
	AEV aev;
	AEVCOST aevcost;

	ptmpl = (PTMPL)vptagm->PbacoFetch(ptagTmplNew, TMPL::FReadTmpl);
	if (pvNil == ptmpl)
		return fFalse;
	if (ptmpl->FIsTdt())
		{
		if (!((PTDT)ptmpl)->FAdjustBody(_pbody))
			{
			ReleasePpo(&ptmpl);
			return fFalse;
			}
		}
	if (!ptmpl->FSetActnCel(_pbody, _anidCur, _celnCur))
		return fFalse;
	_PositionBody(&_xyzCur);
	ReleasePpo(&_ptmpl);
	_ptmpl = ptmpl;
	TAGM::CloseTag(&_tagTmpl);
	_tagTmpl = *ptagTmplNew;
	TAGM::DupTag(ptagTmplNew);

	// If the new TMPL is not a TDT, we're done...although currently this
	// function should only be called with ptagTmplNew being a TDT
	if (!ptmpl->FIsTdt())
		return fTrue;

	cbsetNew = _pbody->Cbset();
	// Need to remove any costume events acting on ibset >= cbsetNew
	// Loop backwards to prevent indexing problems since we are deleting
	// events in this GG as we go
	for (iaev = _pggaev->IvMac() - 1; iaev >= 0; iaev--)
		{
		_pggaev->GetFixed(iaev,	&aev);
		if (aev.aet == aetCost)
			{
			_pggaev->Get(iaev, &aevcost);
			if (aevcost.ibset >= cbsetNew)
				{
				_RemoveAev(iaev, fTrue);
				}
			}
		}
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************

	Assert the validity of the ACTR.

***************************************************************************/
void ACTR::AssertValid(ulong grfobj)
{
	
	ACTR_PAR::AssertValid(fobjAllocated);
	AssertNilOrPo(_pbody, 0);
	AssertNilOrPo(_ptmpl, 0);
	AssertPo(_pggaev, 0);
	AssertPo(_pglrpt, 0);
	AssertPo(_pglsmm, 0);

	long iaevMac = _pggaev->IvMac();
	long irptMac = _pglrpt->IvMac();
	bool fTracing = FPure(grfobj & fobjAssertFull);

	AssertIn(_iaevCur, -1, iaevMac + 1);
	if (irptMac > 0)
		AssertIn(_rtelCur.irpt, 0, irptMac);
	if (ivNil != _iaevActnCur)
		AssertIn(_iaevActnCur, 0, iaevMac);
	if (_iaevAddCur != ivNil)
		AssertIn(_iaevAddCur, 0, iaevMac);
	
	if (fTracing)
		{	
		RPT rpt;
		long irpt;
		long iaev;
		AEV aev;														
		bool mpaetfSeen[aetLim];
		RTEL rtel;

		ClearPb(mpaetfSeen, size(mpaetfSeen));

		rtel.irpt = -1;
		rtel.dwrOffset = rZero;
		rtel.dnfrm = 0;

		// Supply a debug readable view of br_scalar path
		for (irpt = 0; irpt < irptMac; irpt++)
			{
			_pglrpt->Get(irpt, &rpt);
			}

		AssertIn(_rtelCur.irpt, 0, irptMac + 1);	// Supply a debug readable view of the event stream
		_pggaev->GetFixed(0, &aev);
		Assert(aetAdd == aev.aet, "BUG: No add event at front of list");

		for (iaev = 0; iaev < _pggaev->IvMac(); iaev++)
			{
			_pggaev->GetFixed(iaev,	&aev);
			Assert(rtel <= aev.rtel, "Illegal ordering in event list");
			AssertIn(aev.aet, 0, aetLim);

			// Verify uniqueness of event types in a single frame
			bool fNewFrame = (aev.aet == aetAdd || rtel != aev.rtel);

			if (mpaetfSeen[aev.aet] == fTrue)
				{
				switch(aev.aet)
					{
				case aetCost:
				case aetAdd:
				case aetSnd:
					break;

				default:
					Assert(fNewFrame, "Duplicate events in a single frame" );
					}
				}

			if (fNewFrame)
				{
				ClearPb(mpaetfSeen, size(mpaetfSeen));
				}

			mpaetfSeen[aev.aet] = fTrue;
			rtel = aev.rtel;
			_pglrpt->Get(aev.rtel.irpt, &rpt);
			Assert(aev.rtel.dwrOffset < rpt.dwr || rpt.dwr == rZero, "Invalid rtel.dwrOffset");

			// Variable portion of aev retrieved for debug viewing
			switch(aev.aet)
				{
			case aetAdd:
				{
				AEVADD aevadd;
				Assert(_pggaev->Cb(iaev) == kcbVarAdd, "Corrupt size in event list");
				_pggaev->Get(iaev, &aevadd); 	
				break;
				}
			case aetActn:
				{
				AEVACTN aevactn;
				_pggaev->Get(iaev, &aevactn);
				Assert(_pggaev->Cb(iaev) == kcbVarActn, "Corrupt size in event list");
				break;
				}
			case aetCost:
				Assert(_pggaev->Cb(iaev) == kcbVarCost, "Corrupt size in event list");
				break;
			case aetRotF:
			case aetRotH:
				Assert(_pggaev->Cb(iaev) == kcbVarRot, "Corrupt size in event list");
				break;
			case aetSize:
				Assert(_pggaev->Cb(iaev) == kcbVarSize, "Corrupt size in event list");
				break;
			case aetPull:
				Assert(_pggaev->Cb(iaev) == kcbVarPull, "Corrupt size in event list");
				break;
			case aetSnd:
				break;
			case aetFreeze:
				{
				long faevfrz;
				_pggaev->Get(iaev, &faevfrz);
				Assert(_pggaev->Cb(iaev) == kcbVarFreeze, "Corrupt size in event list");
				break;
				}
			case aetTweak:
				Assert(_pggaev->Cb(iaev) == kcbVarTweak, "Corrupt size in event list");
				break;
			case aetStep:
				{
				BRS dwrAev;
				_pggaev->Get(iaev, &dwrAev);
				Assert(_pggaev->Cb(iaev) == kcbVarStep, "Corrupt size in event list");
				break;
				}
			case aetRem:
				break;
			case aetMove:
				Assert(_pggaev->Cb(iaev) == kcbVarMove, "Corrupt size in event list");
				break;
			default:
				Assert(0, "Unknown event type");
				}
			}
		}
}

/***************************************************************************

	Mark memory used by the ACTR

***************************************************************************/
void ACTR::MarkMem(void)
{
	AssertThis(0);
	long iaev;
	AEV *paev;
	AEVSND *paevsnd;

	ACTR_PAR::MarkMem();
	for (iaev = 0; iaev < _pggaev->IvMac(); iaev++)
		{
		paev = (AEV *)_pggaev->QvFixedGet(iaev);
		if (aetSnd == paev->aet)
			{
			paevsnd = (AEVSND *)_pggaev->QvGet(iaev);
			if (paevsnd->tag.sid == ksidUseCrf)
				paevsnd->tag.MarkMem();
			}
		}
	MarkMemObj(_pggaev);
	MarkMemObj(_pglrpt);
	MarkMemObj(_pbody);
	MarkMemObj(_ptmpl);
	MarkMemObj(_pglsmm);
	_tagTmpl.MarkMem();
}

#endif //DEBUG
