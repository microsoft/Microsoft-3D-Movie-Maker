/***************************************************************************

	Actor Edit.   Cut/Copy/Paste/Undo
																		 
	Primary authors:
		ACLP::(clipbd)	Seanse
		AUND::(undo)	Seanse
		ACTR::(undo)	Seanse
		ACTR::(vacuum)  Susano
		ACTR::(dup/restore) Susano	
	Review Status:  Reviewed	
		
***************************************************************************/

#include "soc.h"

ASSERTNAME
RTCLASS(AUND)

/***************************************************************************
	
	Duplicate the actor from this frame through
	the end of subroute or (if fEntireScene) the end of the scene

***************************************************************************/
bool ACTR::FCopy(PACTR *ppactr, bool fEntireScene)
{
	AssertThis(0);
	AssertVarMem(ppactr);

	long iaev;
	long iaevLast;
	AEV aev;
	AEVACTN aevactn;
	AEVSND aevsnd;
	RPT rpt;
	RPT rptOld;
	RPT *prptSrc;
	RPT *prptDest;

	(*ppactr) = PactrNew(&_tagTmpl);

	if (*ppactr == pvNil)
		{
		return fFalse;
		}

	// If the current point is between nodes, a node will
	// be inserted.  Compute its dwr.
	_pglrpt->Get(_rtelCur.irpt, &rpt);
	rptOld.dwr = rpt.dwr;
	rpt.dwr = BrsSub(rptOld.dwr, _rtelCur.dwrOffset);

	//
	// Gather all earlier events & move to the current one
	// It is sufficient to begin with the current subroute
	// Note: Add events will require later translation of nfrm
	//
	for (iaev = _iaevAddCur; iaev < _iaevCur; iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);

		//Is this an event we want to copy?
		switch (aev.aet)
			{
		case aetActn:	// Copy, editing current cel only
			_pggaev->Get(iaev, &aevactn);
			aevactn.celn = _celnCur;
			break;

		// copy
		case aetAdd:
		case aetCost:
		case aetPull:
		case aetSize:
		case aetRotF:
		case aetFreeze:
		case aetStep:
		case aetMove:
			break;

		// The following events are not automatically copied
		case aetSnd:
			_pggaev->Get(iaev, &aevsnd);
			// Update the cno for the chid from the original movie
			// The scene this came from may be lost later
			if (!_pscen->Pmvie()->FResolveSndTag(&aevsnd.tag, aevsnd.chid))
				{
				goto LFail;
				}
			_pggaev->Put(iaev, &aevsnd);
			if (aevsnd.celn == smmNil && iaev >= _iaevFrmMin)
				break;
			if (iaev > _iaevActnCur &&
				aevsnd.celn != smmNil)
				{
				// Retain current motion match sounds
				break;
				}
			continue;
		case aetTweak:
		case aetRotH:
#ifdef BUG1950
//REVIEW (Susano):  Postponed till v2.0	
			if (iaev >= _iaevCur)  	// Code change not yet verified
#else //!BUG1950
			if (iaev >= _iaevFrmMin)
#endif //!BUG1950
				{
				// Retain these events from this frame
				break;
				}
			continue;
		case aetRem:
			continue;  		// Do not copy

		default: 	
			Bug("Unknown event type... Copy or Not?");
			break;
			}

		//set the event to happen right here.
		aev.rtel.dnfrm = 0;
		aev.rtel.irpt = 0;
		aev.rtel.dwrOffset = 0;
		iaevLast = (*ppactr)->_pggaev->IvMac();

		// Insert aev.  Tag ref count will be updated.
		_pggaev->Lock();
		if (!(*ppactr)->_FInsertAev(iaevLast,
			_pggaev->Cb(iaev),
			(aev.aet == aetActn) ? &aevactn : _pggaev->QvGet(iaev),
			&aev,
			fFalse))
			{
			_pggaev->Unlock();
			goto LFail;
			}
		_pggaev->Unlock();
		(*ppactr)->_MergeAev(0,	iaevLast);
		}

	//
	// Copy remaining events, adjusting their locations
	//
	for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);

		if ((!fEntireScene) &&
			(aetAdd == aev.aet) &&
			(*ppactr)->_pggaev->IvMac() > 0)
			{
			break;
			}

		//Adjust the event indicies.
		aev.rtel.irpt -= _rtelCur.irpt;

		if (aev.rtel.irpt == 0)
			{
			if (aev.rtel.dwrOffset == _rtelCur.dwrOffset)
				{
				aev.rtel.dwrOffset = rZero;
				if (aev.rtel.dnfrm < _rtelCur.dnfrm)
					{
					aev.rtel.dnfrm = 0;
					}
			    else
					{
					aev.rtel.dnfrm -= _rtelCur.dnfrm;
					}
				}
			else
				{
				if (rpt.dwr == rZero)
					aev.rtel.dwrOffset = rZero;
				else
					{
					aev.rtel.dwrOffset = BrsSub(aev.rtel.dwrOffset, _rtelCur.dwrOffset);
					}
				}
			}

		// Insert will update the tag ref count
		iaevLast = (*ppactr)->_pggaev->IvMac();
		_pggaev->Lock();
		if (!(*ppactr)->_FInsertAev(iaevLast,
			_pggaev->Cb(iaev), _pggaev->QvGet(iaev), &aev, fFalse))
			{
			_pggaev->Unlock();
			goto LFail;
			}
		_pggaev->Unlock();
		}

	//
	// Add the point we are at.
	//
	if (!(*ppactr)->_pglrpt->FInsert(0, &rpt))
		{
        goto LFail;
		}

	//
	// Copy the rest of the subroute or route
	//
	if ((_rtelCur.irpt + 1) < _pglrpt->IvMac())
		{
		// Locate the amount of route to copy
		//
		long irptLim = _pglrpt->IvMac();
		long irpt;

		if (!fEntireScene)
			{
			for (irpt = _rtelCur.irpt; irpt < irptLim; irpt++)
				{
				_pglrpt->Get(irpt, &rpt);
				if (rZero == rpt.dwr)
					{
					irptLim = irpt + 1;
					break;
					}
				}
			}

		if (!(*ppactr)->_pglrpt->FSetIvMac((*ppactr)->_pglrpt->IvMac() +
			irptLim - (_rtelCur.irpt + 1)))
            {
			goto LFail;
			}

		prptSrc = (RPT *)_pglrpt->QvGet(_rtelCur.irpt + 1);
		prptDest = (RPT *)(*ppactr)->_pglrpt->QvGet(1);
		CopyPb(prptSrc, prptDest,
			LwMul(irptLim - (_rtelCur.irpt + 1), size(RPT)));
		}
	else
		{
		// Mark the end of the path
		rpt.dwr = rZero;
		(*ppactr)->_pglrpt->Put(0,&rpt);
		}

	(*ppactr)->_SetStateRewound();

	AssertPo(*ppactr, 0);
	return fTrue;

LFail:
	ReleasePpo(ppactr);
	return fFalse;
}



/***************************************************************************
	
	Duplicate the entire actor from *this onto *ppactr
	This is not from this frame on.  The entire actor is duplicated.
	This does not duplicate the Brender body.
	Default is fReset = fFalse;

	NOTE:
	Upon exit, the new actor will still be in the scene.
	If (!fReset), all state information will have been retained.
	
***************************************************************************/
bool ACTR::FDup(PACTR *ppactr, bool fReset)
{
	AssertThis(0);
	AssertVarMem(ppactr);

	long cactRef;
	PACTR pactrSrc = this;
	PACTR pactrDest;

	// Due to state var duplication, using NewObj, not PactrNew
	pactrDest = (*ppactr) = NewObj ACTR(); 	
	if (*ppactr == pvNil)
		{
		return fFalse;
		}

	// AddRef only if attached to a scene
	if (pvNil != _pbody)
		_pbody->AddRef();
	_ptmpl->AddRef();
	TAGM::DupTag(&_tagTmpl);

	// Copy over all members
	// Note that both copies will point to the same *_pbody & *_ptmpl
    cactRef = pactrDest->_cactRef;
    *(pactrDest) = *pactrSrc;
	pactrDest->_cactRef = cactRef;
	pactrDest->_fTimeFrozen = fFalse;

	if (!pactrDest->_FCreateGroups())
		{
        goto LFail;
		}

	if (!_FDupCopy(pactrSrc, pactrDest))
		{
		goto LFail;
		}

	if (fReset)
		{
		(*ppactr)->Reset();
		}
	return fTrue;

LFail:
	ReleasePpo(ppactr);
	return fFalse;
	}


/***************************************************************************
	
	Restore the actor from *ppactr onto *this

***************************************************************************/
void ACTR::Restore(PACTR pactr)
{
	AssertThis(0);
	AssertVarMem(pactr);
	
	long cactRef;
	PACTR pactrSrc = pactr;
	PACTR pactrDest = this;

	Assert(pactr->_ptmpl == _ptmpl, "Restore ptmpl logic error");
	Assert(pactr->_pbody == _pbody, "Restore pbody logic error");

	// Copy over all members
	// Note that both copies will point to the same *_pbody & *_ptmpl
    cactRef = pactrDest->_cactRef;
	PGG pggaev = pactrDest->_pggaev;
	PGL pglrpt = pactrDest->_pglrpt;
	PGL pglsmm = pactrDest->_pglsmm;
    *(pactrDest) = *pactrSrc;
	pactrDest->_cactRef = cactRef;
	pactrDest->_pggaev = pggaev;
	pactrDest->_pglrpt = pglrpt;
	pactrDest->_pglsmm = pglsmm;

	// Swap the gl and gg structures
	SwapVars(&pactrSrc->_pggaev, &pactrDest->_pggaev);
	SwapVars(&pactrSrc->_pglrpt, &pactrDest->_pglrpt);
	SwapVars(&pactrSrc->_pglsmm, &pactrDest->_pglsmm);

	pactr->_pscen->Pmvie()->InvalViews();

	return;
}

/***************************************************************************
	
	Restore this actor from an undo object pactrRestore.

***************************************************************************/
void ACTR::_RestoreFromUndo(PACTR pactrRestore)
{
	AssertBaseThis(0);
	AssertVarMem(pactrRestore);
	Assert(pactrRestore->_pbody == pvNil, "Not restoring from undo object");

	long nfrmCur = _nfrmCur;
	PSCEN pscen = pactrRestore->_pscen;

	// Modify pactrRestore for Restore()
	pactrRestore->_pbody = _pbody;
	pactrRestore->_pscen = _pscen;
	_Hide();   // Added actor will show
	Restore(pactrRestore);

	// Restore pactrRestore to be unmodified
	pactrRestore->_pbody = pvNil;
	pactrRestore->_pscen = pscen;

	FGotoFrame(nfrmCur); 	// No further recovery meaningful
	_pscen->Pmvie()->InvalViews();
}

/***************************************************************************
	
	Copy the GG and GL structures for actor duplication/restoration
	
	NOTE:
	This is not from this frame on.  The entire actor is duplicated.

***************************************************************************/
bool ACTR::_FDupCopy(PACTR pactrSrc, PACTR pactrDest)
{
	AssertBaseThis(0);
	AssertPo(pactrDest->_pggaev, 0);
	AssertPo(pactrDest->_pglrpt, 0);
	
	RPT *prptSrc;
	RPT *prptDest;
	SMM *psmmSrc;
	SMM *psmmDest;

	//
	// Copy all events.
	//
	if (!pactrDest->_pggaev->FCopyEntries(pactrSrc->_pggaev, 0, 0,
		pactrSrc->_pggaev->IvMac()))
		{
		goto LFail;
		}

	{
	_pggaev->Lock();
	for (long iaev = 0; iaev < _pggaev->IvMac(); iaev++)
		{
		PTAG ptag;

		if (_FIsIaevTag(_pggaev, iaev, &ptag))
			TAGM::DupTag(ptag);
		}
	_pggaev->Unlock();
	}

	//
	// Copy Route
	//
	if (pactrSrc->_pglrpt->IvMac() > 0)
		{
		if (!pactrDest->_pglrpt->FSetIvMac(pactrSrc->_pglrpt->IvMac()))
            {
			goto LFail;
			}

		prptSrc = (RPT *)pactrSrc->_pglrpt->QvGet(0);
		prptDest = (RPT *)pactrDest->_pglrpt->QvGet(0);
		CopyPb(prptSrc, prptDest, LwMul(pactrSrc->_pglrpt->IvMac(), size(RPT)));
		}

  	//
	// Copy Smm
	//
	if (pactrSrc->_pglsmm->IvMac() > 0)
		{
		if (!pactrDest->_pglsmm->FSetIvMac(pactrSrc->_pglsmm->IvMac()))
            {
			goto LFail;
			}

		psmmSrc = (SMM *)pactrSrc->_pglsmm->QvGet(0);
		psmmDest = (SMM *)pactrDest->_pglsmm->QvGet(0);
		CopyPb(psmmSrc, psmmDest, LwMul(pactrSrc->_pglsmm->IvMac(), size(SMM)));
		}

	return fTrue;

LFail:
	if (this != pactrDest)
		ReleasePpo(&pactrDest);
	return fFalse;
}


/***************************************************************************
	
	Duplicate the indicated portion of the route from this frame on
	from actor "this" to actor *ppactr.
	Note **: The point on the route may not land on a node.  If between
	nodes, insert a point to make the two route sections identical.
	
***************************************************************************/
bool ACTR::FCopyRte(PACTR *ppactr, bool fEntireScene)
{
	AssertThis(0);
	AssertVarMem(ppactr);

	long irpt;
	long dnrpt;
	long irptLim;
	RPT rpt;
	RPT rpt1;
	RPT rptNode;

	(*ppactr) = PactrNew(&_tagTmpl);
	if (*ppactr == pvNil)
		{
		return fFalse;
		}

	//
	// Insert the current point
	//
	_GetXyzFromRtel(&_rtelCur, &rpt.xyz);
	_pglrpt->Get(_rtelCur.irpt, &rptNode);	
	rpt.dwr = rZero;
	
	if (_rtelCur.dwrOffset == rZero)
		{
		rpt.dwr = rptNode.dwr;
		}
	else if (rptNode.dwr > rZero && _rtelCur.irpt < _pglrpt->IvMac() - 1)
		{
		_pglrpt->Get(_rtelCur.irpt + 1, &rpt1);
		rpt.dwr = BR_LENGTH3(BrsSub(rpt1.xyz.dxr, rpt.xyz.dxr),
						BrsSub(rpt1.xyz.dyr, rpt.xyz.dyr),
						BrsSub(rpt1.xyz.dzr, rpt.xyz.dzr));
		if (rZero == rpt.dwr)
			{
			rpt.dwr = rEps;	  	//Epsilon.  Prevent pathological incorrect end-of-path
			}
		}

	if (!(*ppactr)->_pglrpt->FInsert(0, &rpt))
		{
		goto LFail;
		}

	if ((!fEntireScene) && rpt.dwr == rZero)
		{
		goto LEnd;
		}

	//
	// If copying subroute only, determine amount to copy
	//
	irptLim = _pglrpt->IvMac();
	if (!fEntireScene)
		{
		for (irpt = _rtelCur.irpt + 1; irpt < irptLim; irpt++)
			{
			_pglrpt->Get(irpt, &rpt);
			if (rZero == rpt.dwr)
				{
				irptLim = irpt + 1;
				break;
				}
			}
		}

	//
	// Copy indicated portion of the route
	//
	dnrpt = irptLim - (_rtelCur.irpt + 1);
	if (dnrpt > 0 && !(*ppactr)->_pglrpt->FEnsureSpace(dnrpt, fgrpNil))
			goto LFail;

	for (irpt = _rtelCur.irpt + 1; irpt < irptLim; irpt++)
		{	
		_pglrpt->Get(irpt, &rpt);

		if (!(*ppactr)->_pglrpt->FInsert(irpt - _rtelCur.irpt, &rpt))
			{
			goto LFail;
			}

		if ((!fEntireScene) && (rZero == rpt.dwr))
			{
            break;
			}
		}
	
LEnd:
	(*ppactr)->_SetStateRewound();
	return fTrue;

LFail:
	ReleasePpo(ppactr);
	return fFalse;
}


/***************************************************************************
	
	Paste the rte from the clipboard pactr to the current actor's current
	frame onward.
	On failure, unwinding is expected to be via the dup'd actor from undo.
	This overwrites the end of the current subroute.
	NOTE:  To be meaningful, the pasted route section is translated to
	extend from the current point.

***************************************************************************/
bool ACTR::FPasteRte(PACTR pactr)
{
	AssertThis(0);
	AssertVarMem(pactr);
	
	AEV aev;
	RPT rpt;
	RPT rptCur;
	XYZ dxyz;
	long iaev;
	long irpt;
#ifdef STATIC
	bool fStatic;
#endif //STATIC
	long crptDel = 0;
	long crptNew = pactr->_pglrpt->IvMac() - 1;
	
	if (crptNew <= 0)
		{
		PushErc(ercSocNothingToPaste);
		return fFalse;
		}
		
	if (!_pglrpt->FEnsureSpace(crptNew, fgrpNil) ||
		!_pggaev->FEnsureSpace( 1, kcbVarStep, fgrpNil))
		{
		return fFalse;
		}

	//
	// May be positioned between nodes -> potentially
	// insert the current point
	//
	_GetXyzFromRtel(&_rtelCur, &rptCur.xyz);
	if (rZero != _rtelCur.dwrOffset)
		{
		BRS dwrCur = _rtelCur.dwrOffset;
		_rtelCur.irpt++;
		_rtelCur.dwrOffset = rZero;
		_rtelCur.dnfrm = 0;
		if (!_FInsertGgRpt(_rtelCur.irpt, &rptCur, dwrCur))
			return fFalse;
		_GetXyzFromRtel(&_rtelCur, &_xyzCur);	
		_AdjustAevForRteIns(_rtelCur.irpt, 0);
		}
	
	//	
	// Delete to the end of this *sub*route
	//
	for (irpt = _rtelCur.irpt + 1; irpt < _pglrpt->IvMac(); irpt++)
		{
		_pglrpt->Get(irpt, &rpt);
		crptDel++;
		if (rpt.dwr == rZero)
            {
            break;	
			}
		}
	if (crptDel > 0)
		{
		_pglrpt->Delete(_rtelCur.irpt, crptDel);
		}

	//
	// Remove events until the end of the *sub*route
	// Space optimization: should precede paste
	// Update location pointer of events of later subroutes
	//
    for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.rtel.irpt > _rtelCur.irpt + crptDel)
			{
			aev.rtel.irpt += crptNew - crptDel;
			_pggaev->PutFixed(iaev, &aev);
			continue;
			}
		else
			{
			_RemoveAev(iaev, fFalse);
			iaev--;
			}
		}
	
	//
	// Paste in the new route.
	// Translate the points of this section of route
	// Adjust the aev presently
	//
	pactr->_pglrpt->Get(0, &rpt);
	dxyz.dxr = BrsSub(rptCur.xyz.dxr, rpt.xyz.dxr);
	dxyz.dyr = BrsSub(rptCur.xyz.dyr, rpt.xyz.dyr);
	dxyz.dzr = BrsSub(rptCur.xyz.dzr, rpt.xyz.dzr);
	for (irpt = 1; irpt <= crptNew; irpt++)
		{
		pactr->_pglrpt->Get(irpt, &rpt);
		rpt.xyz.dxr = BrsAdd(rpt.xyz.dxr, dxyz.dxr);
		rpt.xyz.dyr = BrsAdd(rpt.xyz.dyr, dxyz.dyr);
		rpt.xyz.dzr = BrsAdd(rpt.xyz.dzr, dxyz.dzr);
		AssertDo(_pglrpt->FInsert(_rtelCur.irpt + irpt, &rpt), "Logic error");
		}

	//
  	// Set the right dwr distance from the current point to
	// the first point on the new section of route
	//
	_pglrpt->Get(_rtelCur.irpt + 1, &rpt);
	rptCur.dwr = BR_LENGTH3(BrsSub(rpt.xyz.dxr, rptCur.xyz.dxr),
					BrsSub(rpt.xyz.dyr, rptCur.xyz.dyr),
					BrsSub(rpt.xyz.dzr, rptCur.xyz.dzr));
	if (rZero == rptCur.dwr)
		rptCur.dwr = rEps;	  	//Epsilon.  Prevent pathological incorrect end-of-path
	_pglrpt->Put(_rtelCur.irpt, &rptCur);

#ifdef STATIC
	//
	// Force floating behavior on a static action
	//
	Assert(_iaevActnCur >= 0, "Actor has no action");
	if (!_FGetStatic(_anidCur, &fStatic))
		return fFalse;
	if (fStatic)
		{
		if (!FSetStep(kdwrNil))
			return fFalse;
		}
#else //!STATIC
	// Force continuation onto newly pasted path
	if (!FSetStep(kdwrNil))
			return fFalse;
#endif //!STATIC			
	
	//
	// Set new end of path freeze & step events	
	//
	long faevfrz = (long)fTrue;
	BRS dwrStep = rZero;
	aev.aet = aetFreeze;
	aev.rtel.irpt = _rtelCur.irpt + crptNew;
	aev.rtel.dnfrm = 0;
	aev.rtel.dwrOffset = rZero;
	aev.nfrm = _nfrmCur;		//will be updated in ComputeLifetime()
	if (!_FInsertAev(_iaevCur, kcbVarFreeze, &faevfrz, &aev))
		return fFalse;
	aev.aet = aetStep;
	if (!_FInsertAev(_iaevCur, kcbVarStep, &dwrStep, &aev))
		return fFalse;

	_fLifeDirty = fTrue;
	_pscen->InvalFrmRange();
	_pscen->MarkDirty();

	_PositionBody(&_xyzCur);
	return fTrue;
}



/***************************************************************************
	
	Put an already existing actor in this scene.
		
***************************************************************************/
bool ACTR::FPaste(long nfrm, SCEN *pscen)
{
	AssertThis(0);

	AEV aev;
	RPT rpt;
	AEVADD aevadd;
	AEVSND aevsnd;
	BRS xrCam = rZero;
	BRS yrCam = rZero;
	BRS zrCam = kzrDefault;
	BRS xr, yr, zr;
	long iaev;
	long dnfrm;
#ifdef BUG1888
	PTMPL ptmpl;
	PCRF pcrf;
	TAG tag;

	//
	// Ensure that the tag to the TDT being pasted is in the current movie.
	//
	if (FIsTdt())
		{
		Assert(_tagTmpl.sid == ksidUseCrf, "TDTs should be stored in a document!");

		if (!pscen->Pmvie()->FEnsureAutosave(&pcrf))
			{
			return fFalse;
			}
		if (pcrf != _tagTmpl.pcrf)
			{
	 		// Need to save this actor's tagTmpl in this movie because it came from another movie

			tag = _tagTmpl;
			TAGM::DupTag(&tag);
			// Save the tag to the movie's _pcrfAutosave.  The tag now
			// points to the copy in this movie.
			if (!TAGM::FSaveTag(&tag, pcrf, fTrue))
				{
				TAGM::CloseTag(&tag);
				return fFalse;
				}
			// Get a template based on the new tag
			ptmpl = (PTMPL)vptagm->PbacoFetch(&tag, TMPL::FReadTmpl);
			if (pvNil == ptmpl)
				{
				TAGM::CloseTag(&tag);
				return fFalse;
				}
			// Change the actor to use the new tag and template
			TAGM::CloseTag(&_tagTmpl);
			_tagTmpl = tag;
			ReleasePpo(&_ptmpl);
			_ptmpl = ptmpl;
			}
		}
#endif //BUG1888

	//
	// Update lifetime
	//
	_nfrmFirst = nfrm;
	_fLifeDirty = fTrue;
	_nfrmCur = nfrm - 1;
	SetPscen(pscen);

	// Always place actors on the "floor"
	_GetNewOrigin(&xr, &yr, &zr);

	Assert(_pggaev->IvMac() > 0, "Nothing to paste!");	
	_pggaev->GetFixed(0, &aev);
	if (aev.aet != aetAdd)
		return fFalse;
	dnfrm = aev.nfrm - nfrm;

	//
	// Begin by locating the actor at (xr,yr,zr)
	// There are no Full path or Sub path translations at this point.
	// Note: In order to place a pasted actor at the insertion point,
	// the translation needs to compensate for the distance
	// recorded in each path point -> subtract the first path point.
	_pglrpt->Get(0, &rpt);
	_dxyzSubRte.dxr = rZero;
	_dxyzSubRte.dyr = rZero;
	_dxyzSubRte.dzr = rZero;
	_dxyzFullRte.dxr = BrsSub(xr, rpt.xyz.dxr);
	_dxyzFullRte.dyr = BrsSub(yr, rpt.xyz.dyr);
	_dxyzFullRte.dzr = BrsSub(zr, rpt.xyz.dzr);
	_pggaev->Get(0, &aevadd);
	_dxyzFullRte.dxr = BrsSub(_dxyzFullRte.dxr, aevadd.dxr);
	_dxyzFullRte.dyr = BrsSub(_dxyzFullRte.dyr, aevadd.dyr);
	_dxyzFullRte.dzr = BrsSub(_dxyzFullRte.dzr, aevadd.dzr);

    //
	// Translate the new actor in time
	// Update sound events
	for (iaev = 0; iaev < _pggaev->IvMac(); iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (dnfrm != 0)
			{
			aev.nfrm -= dnfrm;
			_pggaev->PutFixed(iaev, &aev);
			}
		if (aetSnd != aev.aet)
			continue;
		_pggaev->Get(iaev, &aevsnd);
		if (aevsnd.tag.sid != ksidUseCrf)
			continue;
		// Save tag (this may be a new movie)
		if (!aevsnd.tag.pcrf->Pcfl()->FFind(aevsnd.tag.ctg, aevsnd.tag.cno))
			{
			PushErc(ercSocNoSndOnPaste);
			_RemoveAev(iaev);
			iaev--;
			continue;
			}
		if (!_pscen->Pmvie()->FSaveTagSnd(&aevsnd.tag))
			{
			Bug("Expected to locate user sound chunk");
			_RemoveAev(iaev);
			iaev--;
			}
		else
			{
			// Adopt this sound into the new scene
			if (!_pscen->Pmvie()->FChidFromUserSndCno(aevsnd.tag.cno, &aevsnd.chid))
				return fFalse;
			// Update event
			_pggaev->Put(iaev, &aevsnd);
			}
		}

	// Rotate 3D spletters to face the camera
	if (_ptmpl->FIsTdt())
		{
		_pggaev->Get(0, &aevadd);
		aevadd.ya = _pscen->Pbkgd()->BraRotYCamera();
		_pggaev->Put(0, &aevadd);
		}

	_UpdateXyzRte();
	_pscen->MarkDirty();
    return fTrue;
}


/***************************************************************************
	
	Make an actor look like they were just read in, and never in a scene.
		
***************************************************************************/
void ACTR::Reset(void)
{
	_pscen = pvNil;
	ReleasePpo(&_pbody); 	// Sets _pbody = pvNil
	_nfrmCur = knfrmInvalid;

	_InitState();
}

//
//
//
//
//  BEGIN CLIPBOARD STUFF
//
//
//
//

RTCLASS(ACLP)

/***************************************************************************
	
	Create an actor clipboard object
	This is from the current frame forward

***************************************************************************/
PACLP ACLP::PaclpNew(PACTR pactr, bool fRteOnly, bool fEntireScene)
{
	AssertPo(pactr, 0);
	Assert(!fRteOnly || !fEntireScene, "Expecting subroute only");

	PACLP paclp;
	PACTR pactrTmp;
	STN stn, stnCopyOf;

	paclp = NewObj ACLP();

	if (paclp == pvNil)
		{
		return(pvNil);
		}

	if (fRteOnly)
		{
		if (!pactr->FCopyRte(&pactrTmp, fEntireScene))
    		{
            ReleasePpo(&paclp);
    		return(pvNil);
    		}
		}
    else
		{
		if (!pactr->FCopy(&pactrTmp, fEntireScene))
    		{
            ReleasePpo(&paclp);
    		return(pvNil);
    		}
        AssertPo(pactrTmp, 0);

		pactr->GetName(&stn);

		pactr->Pscen()->Pmvie()->Pmcc()->GetStn(idsEngineCopyOf, &stnCopyOf);
		
		if (!FEqualRgb(stn.Psz(), stnCopyOf.Psz(), CchSz(stnCopyOf.Psz()) * size(achar)))
			{
			paclp->_stnName = stnCopyOf;
			if (!paclp->_stnName.FAppendCh(kchSpace) || !paclp->_stnName.FAppendStn(&stn))
				{
				PushErc(ercSocNameTooLong);
				ReleasePpo(&paclp);
				return(pvNil);
				}

			}
		else
			{
			paclp->_stnName = stn;
			}

		}

    paclp->_pactr = pactrTmp;
	paclp->_fRteOnly = fRteOnly;
	AssertPo(paclp, 0);

	return(paclp);
}

/***************************************************************************
	
	Destroys an actor clipboard object
		
***************************************************************************/
ACLP::~ACLP(void)
{
	ReleasePpo(&_pactr);
}

/***************************************************************************
	
	Pastes an actor clipboard object
		
***************************************************************************/
bool ACLP::FPaste(PMVIE pmvie)
{
	AssertThis(0);
	AssertPo(pmvie, 0);

	PACTR pactrNew;

	if (_fRteOnly)
		{
		return (pmvie->FPasteActrPath(_pactr));
		}

	//
	// Duplicate the actor
	//
	if (!_pactr->FDup(&pactrNew, fTrue))
		{
		return(fFalse);
		}
    AssertPo(pactrNew, 0);
	
	if (!pmvie->FPasteActr(pactrNew))
		{
		ReleasePpo(&pactrNew);
		return(fFalse);
		}

	if (!pmvie->FNameActr(pactrNew->Arid(), &_stnName))
		{
		pmvie->Pscen()->RemActrCore(pactrNew->Arid());
		ReleasePpo(&pactrNew);
		return(fFalse);
		}

	ReleasePpo(&pactrNew);
	return(fTrue);

}

#ifdef DEBUG

/****************************************************
 * Mark memory used by the ACLP
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void ACLP::MarkMem(void)
{
	ACLP_PAR::MarkMem();
	MarkMemObj(_pactr);
}

/***************************************************************************
 * Assert the validity of the ACLP
 *
 * Parameters:
 *  grf - bit array of options
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void ACLP::AssertValid(ulong grf)
{
	ACLP_PAR::AssertValid(fobjAllocated);
	_pactr->AssertValid(grf);
}

#endif


/***************************************************************************
	
	Create an undo object

***************************************************************************/
bool ACTR::FCreateUndo(PACTR pactrDup, bool fSndUndo, PSTN pstn)
{
	AssertPo(pactrDup, 0);
	AssertNilOrPo(pstn, 0);

	PAUND paund;

    paund = AUND::PaundNew();

	if (paund == pvNil)
		{
		return fFalse;
		}

	paund->SetPactr(pactrDup);
	paund->SetArid(_arid);
	paund->SetSndUndo(fSndUndo);
	if (pvNil != pstn)
		paund->SetStn(pstn);

	if (!_pscen->Pmvie()->FAddUndo(paund))
		{
        _pscen->Pmvie()->ClearUndo();
	    ReleasePpo(&paund);
		return(fFalse);
		}

    ReleasePpo(&paund);

	//
	// Detach from the scene
	//
	pactrDup->Reset();

	return(fTrue);
}


/***************************************************************************
	
	Add (or replace) an action, and create an undo object

***************************************************************************/
bool ACTR::FSetAction(long anid, long celn, bool fFreeze, PACTR *ppactrDup)
{
	AssertThis(0);
	AssertNilOrVarMem(ppactrDup);

	PACTR pactrDup;

	if (!FDup(&pactrDup))
		{
		return(fFalse);
		}

    if (!FSetActionCore(anid, celn, fFreeze))
		{
		Restore(pactrDup);
        ReleasePpo(&pactrDup);
		return fFalse;
		}

	if (!FCreateUndo(pactrDup))
		{
		Restore(pactrDup);
        ReleasePpo(&pactrDup);
		return(fFalse);
		}

	if (pvNil == ppactrDup)
		ReleasePpo(&pactrDup);
	else
		*ppactrDup = pactrDup;

	return fTrue;
}


/***************************************************************************
	
	Add the event to the event list: Add actor on the stage, and create undo
	object.

***************************************************************************/
bool ACTR::FAddOnStage(void)
{
	AssertThis(0);

	PACTR pactrDup;

	if (!FDup(&pactrDup))
		{
		return(fFalse);
		}

	if (!FAddOnStageCore())
		{
		Restore(pactrDup);
        ReleasePpo(&pactrDup);
		return fFalse;
		}

	if (!FCreateUndo(pactrDup))
		{
		Restore(pactrDup);
        ReleasePpo(&pactrDup);
		return(fFalse);
		}

	ReleasePpo(&pactrDup);
	return fTrue;
}

/***************************************************************************
	
	Normalize an actor.

***************************************************************************/
bool ACTR::FNormalize(ulong grfnorm)
{
	AssertThis(0);

	PACTR pactrDup;

	if (!FDup(&pactrDup))
		{
		return(fFalse);
		}

	if (!FNormalizeCore(grfnorm))
		{
		Restore(pactrDup);
        ReleasePpo(&pactrDup);
		return fFalse;
		}

	if (!FCreateUndo(pactrDup))
		{
		Restore(pactrDup);
        ReleasePpo(&pactrDup);
		return(fFalse);
		}

	ReleasePpo(&pactrDup);
	return fTrue;
}

/***************************************************************************
	
	Set the Costume for a body part
	Add the event to the event list

***************************************************************************/
bool ACTR::FSetCostume(long ibset, TAG *ptag, long cmid, bool fCmtl)
{
	AssertThis(0);
	Assert(fCmtl || ibset >= 0, "Invalid ibset argument");
	AssertVarMem(ptag);

	PACTR pactrDup;

	FDup(&pactrDup);

	if (!FSetCostumeCore(ibset, ptag, cmid, fCmtl))
		{
		Restore(pactrDup);
		ReleasePpo(&pactrDup);
		return fFalse;
		}
	
	if (!FCreateUndo(pactrDup))
		{
		Restore(pactrDup);
        ReleasePpo(&pactrDup);
		return(fFalse);
		}

	ReleasePpo(&pactrDup);
	return fTrue;
}


/***************************************************************************
	
	Delete the path and events from this frame and beyond
	**NOTE:  This does not send the actor offstage.	See FRemFromStageCore.
	
	On Input: fDeleteAll specifies full route vs subroute deletion
	Returns *pfAlive = false if all events and route for this actor
	have been deleted

***************************************************************************/
bool ACTR::FDelete(bool *pfAlive, bool fDeleteAll)
{
	AssertThis(0);
	PACTR pactrDup;
	long iaevCurSav;
	AEV *paev;

	if (!FDup(&pactrDup))
		{
		if (pvNil != pfAlive)
			TrashVar(pfAlive);
		return fFalse ;
		}
   
    // Unless we are deleting to the end of the scene,
	// we need to special case deletion that begins at
	// the same frame as the Add event - otherwise, the
	// code backs up one frame, putting the current frame
	// on the previous subroute.
	// Note: FDelete() does not require that the current 
	// frame be	later than _nfrmFirst.
	if (_iaevAddCur >= 0 && !fDeleteAll)
		{
		paev = (AEV *)_pggaev->QvFixedGet(_iaevAddCur);
		if (_nfrmCur == paev->nfrm)
			{
			if (!_FDeleteEntireSubrte())
				goto LFail;
			if (pvNil != pfAlive)
				*pfAlive = FPure(_pggaev->IvMac() > 0);
			goto LDeleted;
			}
		}

	// Go to the previous frame to update state variables
	iaevCurSav = _iaevCur;
	if (!FGotoFrame(_nfrmCur - 1))
		{
		goto LFail;
		}

#ifndef BUG1870
	// The next two lines are obsolete & cause placement orientation bugs
	// Save the current orientation
	_SaveCurPathOrien();
#endif //!BUG1870

	DeleteFwdCore(fDeleteAll, pfAlive, iaevCurSav);
	
	// Return to original frame
	// _nfrmCur was decremented above
	if (!FGotoFrame(_nfrmCur + 1))
		{
		goto LFail;
		}

	/* Might have deleted some sound events */
	if (Pscen() != pvNil)
		Pscen()->UpdateSndFrame();

LDeleted:
	// The frame slider never required lifetime recomputation at this point.
	// Motion match sounds and prerendering both do, however.
	if (!_FComputeLifetime())
	  	PushErc(ercSocBadFrameSlider);

	if (!FCreateUndo(pactrDup))
		{
        Restore(pactrDup);
		ReleasePpo(&pactrDup);
		return fFalse;
		}

	ReleasePpo(&pactrDup);
	return fTrue;

LFail:
	PushErc(ercSocGotoFrameFailure);
	ReleasePpo(&pactrDup);
	return fFalse;
}


/***************************************************************************
	
	Add the event to the event list: Remove actor from the stage, and an Undo.
	NOTE: This should be called <before> the call to place the actor offstage
***************************************************************************/
bool ACTR::FRemFromStage(void)
{
	AssertThis(0);

	PACTR pactr;

	if (!FDup(&pactr))
		{
		return(fFalse);
		}

    if (!FRemFromStageCore())
		{
		Restore(pactr);
		return fFalse;
		}

	if (!FCreateUndo(pactr))
		{
		Restore(pactr);
        ReleasePpo(&pactr);
		return(fFalse);
		}

	ReleasePpo(&pactr);
	return fTrue;
	
}



/****************************************************
 *
 * Public constructor for actor undo objects.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PAUND AUND::PaundNew()
{
	PAUND paund;
	paund = NewObj AUND();
	AssertNilOrPo(paund, 0);
	return(paund);
}

/****************************************************
 *
 * Destructor for actor undo objects
 *
 ****************************************************/
AUND::~AUND(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pactr);
}


/****************************************************
 *
 * Does a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool AUND::FDo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	long nfrmTmp;
	bool fRet;

	if (!_fSoonerLater)
		{
		return(FUndo(pdocb));
		}

    nfrmTmp = _nfrm;
	_nfrm = _nfrmLast;

	fRet = FUndo(pdocb);

	_nfrm = nfrmTmp;

	return(fRet);
}

/****************************************************
 *
 * Undoes a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool AUND::FUndo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	PACTR pactr;
	PMVU pmvu;

	if (!_pmvie->FSwitchScen(_iscen))
		{
		return(fFalse);
		}

    if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
        _pmvie->ClearUndo();
   		return(fFalse);
		}

	_pmvie->Pmsq()->FlushMsq();

	pactr = _pmvie->Pscen()->PactrFromArid(_arid);
	AssertNilOrPo(pactr, 0);

	if (pactr != pvNil)
		{
		pactr->AddRef();
		}


	//
	// Have scene replace the old actor with this one
	//
	if (_pactr == pvNil)
		{
		_pmvie->Pscen()->RemActrCore(pactr->Arid());
		}
	else
		{
		if (_stn.Cch() != 0)
			{
			// Undo actor name change
			STN stn;
			if (_pmvie->FGetName(_arid, &stn))
				{
				// If FNameActr fails, the actor will not have
				// the correct name...not great, but the user's document
				// won't be corrupted or anything.  Someone will push a
				// ercOom, so I ignore the return value here.
				_pmvie->FNameActr(_pactr->Arid(), &_stn);
				_stn = stn;
				}
			}

		if (_arid != _pactr->Arid())
			{
			_pmvie->Pscen()->RemActrCore(_arid);
			_arid = _pactr->Arid();
			}

		if (!_pmvie->Pscen()->FAddActrCore(_pactr))
			{
			ReleasePpo(&pactr);
			return(fFalse);
			}

		pmvu = (PMVU)_pmvie->PddgGet(0);
		AssertNilOrPo(pmvu, 0);
			
		if ((pmvu != pvNil) && !pmvu->FTextMode())
			{
			_pmvie->Pscen()->SelectActr(_pactr);
			}

		ReleasePpo(&_pactr);	
		}

	_pmvie->Pscen()->InvalFrmRange();

	_pactr = pactr;

	if (pactr != pvNil)
		{
		pactr->Reset();
		}

	if (_fSndUndo)
		{
		_pmvie->Pmsq()->PlayMsq();
		}
	else
		{
		_pmvie->Pmsq()->FlushMsq();
		}

	return(fTrue);
}

/****************************************************
 * Set the actor for this undo object.
 *
 * Parameters:
 * 	pactr - Actor to use
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void AUND::SetPactr(PACTR pactr)
{
	AssertThis(0);

	_pactr = pactr;
	pactr->AddRef();
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the AUND
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void AUND::MarkMem(void)
{
	AssertThis(0);
	AUND_PAR::MarkMem();
	MarkMemObj(_pactr);
}

/***************************************************************************
	Assert the validity of the AUND.
***************************************************************************/
void AUND::AssertValid(ulong grf)
{
	AssertNilOrPo(_pactr, 0);
}
#endif
