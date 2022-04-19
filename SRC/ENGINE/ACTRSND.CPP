/***************************************************************************

	Actor Sound.

	Primary author: Susano
	Status:  Reviewed
		
***************************************************************************/

#include "soc.h"

ASSERTNAME


/***************************************************************************
	
	Add a sound event to the actor event list, and create an undo object

***************************************************************************/
bool ACTR::FSetSnd(PTAG ptag, bool fLoop, bool fQueue, bool fMotionMatch, long vlm, long sty)
{
	AssertThis(0);
	AssertVarMem(ptag);
	AssertIn(sty, 0, styLim);
	
	PACTR pactrDup;

	if (!FDup(&pactrDup))
		{
		return fFalse;
		}

    if (!FSetSndCore(ptag, fLoop, fQueue, fMotionMatch, vlm, sty))
		{
		Restore(pactrDup);
        ReleasePpo(&pactrDup);
		return fFalse;
		}

	if (!FCreateUndo(pactrDup, fTrue))
		{
		Restore(pactrDup);
        ReleasePpo(&pactrDup);
		return fFalse;
		}

	ReleasePpo(&pactrDup);
	return fTrue;
}


/***************************************************************************
	
	Add a sound event to the actor event list
	For user sounds, the chid must also be fetched for the argument *ptag
	and stored with the event, since for user sounds, the cno can change
	(eg on scene import)
	Note: ptag is current because the sound browser has just completed building
	a current tag.	

***************************************************************************/
bool ACTR::FSetSndCore(PTAG ptag, bool fLoop, bool fQueue, bool fMotionMatch, long vlm, long sty)
{
	AssertThis(0);
	AssertVarMem(ptag);
	AssertIn(sty, 0, styLim);

	PMSND pmsnd = pvNil;
	AEVSND aevsnd;
	long ccel;
	long iaev;
	AEV *paev;
	long sqn;
	long cbVar;

	// Verify sound before including in the event list
	pmsnd = (PMSND)vptagm->PbacoFetch(ptag, MSND::FReadMsnd);
	if (pvNil == pmsnd)
		goto LFail;
	
	if (pmsnd->Sty() == styMidi)
		{
		// There are no midi actor sounds in the product
		PushErc(ercSocNoActrMidi);
		goto LFail;
		}

	// Verify space up front
	cbVar = kcbVarStep + kcbVarSnd;
	if (!_pggaev->FEnsureSpace(2, cbVar, fgrpNil))
		return fFalse;
		
	aevsnd.fLoop = fLoop;
	aevsnd.fQueue = fQueue;
	aevsnd.vlm = (vlm == vlmNil) ? pmsnd->Vlm() : vlm;
	aevsnd.tag = *ptag;
	aevsnd.fNoSound = pmsnd->FNoSound();
	aevsnd.sty = (sty == styNil) ? pmsnd->Sty() : sty;
	sty = aevsnd.sty;
	vlm = aevsnd.vlm;

	if (ptag->sid != ksidUseCrf)
		TrashVar(&aevsnd.chid);
	else
		{
		if (!_pscen->Pmvie()->FChidFromUserSndCno(aevsnd.tag.cno, &aevsnd.chid))
			goto LFail;
		}

	if (!fMotionMatch)
		aevsnd.celn = smmNil;	// Not a motion match sound	  	
	else
		{
		if (!_ptmpl->FGetCcelActn(_anidCur, &ccel))
			goto LFail;
		aevsnd.celn = _celnCur % ccel;
		}

	if (!_FAddDoAev(aetSnd, kcbVarSnd, &aevsnd))
		goto LFail;

	ReleasePpo(&pmsnd);

	// Remove "no sounds" of this sqn between here and the next
	// sound of this sqn
	sqn = MSND::SqnActr(sty, _arid);
	for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
		{
		AEVSND aevsndT;
		paev = (AEV *)_pggaev->QvFixedGet(iaev);
		if (aetAdd == paev->aet)
			break;
		if (aetSnd != paev->aet)
			continue;
		_pggaev->Get(iaev, &aevsndT);
		if (MSND::SqnActr(aevsndT.sty, _arid) != sqn)
			continue;
		
		// Quit when reach real sound of same sqn
		if (!aevsndT.fNoSound)
			break;	
		_RemoveAev(iaev);
		iaev--;
		}


	// Enqueue the motion match sounds in the Msq
	// Allow the edit even if all sounds do not play
	if (!(_pscen->GrfScen() & fscenSounds))
		_FEnqueueSmmInMsq();		//Ignore errors : other sounds involved

	Pscen()->Pmvie()->Pmcc()->SetSndFrame(fTrue);
	return fTrue;

LFail:
	ReleasePpo(&pmsnd);
	return fFalse;
}


/***************************************************************************
	
	Enqueue the sound at event iaev.
	Enter motion match sounds in the smm gl.
	Enter non-motion match sounds in the Msq

***************************************************************************/
bool ACTR::_FEnqueueSnd(long iaev)
{
	AssertThis(0);
	AssertIn(iaev, 0, _pggaev->IvMac());

	AEVSND aevsnd;
	PMSND pmsnd = pvNil;
	long tool;

	_pggaev->Get(iaev, &aevsnd);

	//
	// Motion match sounds are not enqueued when event iaev is seen
	// Otherwise the sound would be played twice
	//
	if (aevsnd.celn != smmNil)
		{
		// Insert Smm for this cel in the gl
		return _FInsertSmm(iaev);
		}
	
	if (aevsnd.fLoop)
		tool = toolLooper;
	else
		tool = toolSounder;

	// If the scene was imported, the sound will need to be resolved
	if (aevsnd.tag.sid == ksidUseCrf)
		{
		if (!_pscen->Pmvie()->FResolveSndTag(&aevsnd.tag, aevsnd.chid))
			{
			Bug("Actrsnd: Expected to resolve snd tag");
			goto LFail;
			}
		_pggaev->Put(iaev, &aevsnd);	// Update event
		}

 	pmsnd = (PMSND)vptagm->PbacoFetch(&aevsnd.tag, MSND::FReadMsnd);
	if (pvNil == pmsnd)
		goto LFail;

	if (!_pscen->Pmvie()->Pmsq()->FEnqueue(pmsnd, _arid, aevsnd.fLoop, aevsnd.fQueue,
		aevsnd.vlm, pmsnd->Spr(tool), fTrue))
		{
		goto LFail;
		}
	
	ReleasePpo(&pmsnd);
	return fTrue;

LFail:
	ReleasePpo(&pmsnd);
	return fFalse;
}


/***************************************************************************
	
	Play the motion match sounds appropriate to the current cel

***************************************************************************/
bool ACTR::_FEnqueueSmmInMsq(void)
{
	AssertThis(0);

	long celn;
	long ccel;
	long ismm;
	SMM *psmm;
	bool fSuccess = fTrue;
	PMSND pmsnd = pvNil;

	if (!_ptmpl->FGetCcelActn(_anidCur, &ccel))
		return fFalse;
	celn = _celnCur % ccel;

	for (ismm = 0; ismm < _pglsmm->IvMac(); ismm++)
		{
		psmm = (SMM *)(_pglsmm->QvGet(ismm));
		Assert(psmm->aevsnd.celn != smmNil, "Logic error in smm");
		if (psmm->aevsnd.celn != celn)
			{
			continue;
			}

		if (psmm->aevsnd.tag.sid == ksidUseCrf)
			{
			if (!_pscen->Pmvie()->FResolveSndTag(&psmm->aevsnd.tag, psmm->aevsnd.chid))
				{
				Bug("Actrsnd: Expected to resolve snd tag");
				goto LFail;
				}
			_pglsmm->Put(ismm, psmm);	// Update event
			}

		pmsnd = (PMSND)vptagm->PbacoFetch(&psmm->aevsnd.tag, MSND::FReadMsnd);
		if (pvNil == pmsnd)
			{
			fSuccess = fFalse;
			continue;
			}

	  	// Motion match sounds need to be entered at low priority as sounds of the
		// same type take precedence over them
		if (!_pscen->Pmvie()->Pmsq()->FEnqueue(pmsnd, _arid, psmm->aevsnd.fLoop,
			psmm->aevsnd.fQueue, psmm->aevsnd.vlm, pmsnd->Spr(toolMatcher), fTrue, 0, fTrue))
			{
			// Continuing would result in additional error
			goto LFail;
			}

		ReleasePpo(&pmsnd);
		}

	return fSuccess;
LFail:
	ReleasePpo(&pmsnd);
	return fFalse;
}


/***************************************************************************
	
	Insert a motion match sound in the smm queue

***************************************************************************/
bool ACTR::_FInsertSmm(long iaev)
{
	AssertThis(0);
	AssertIn(iaev, 0, _pggaev->IvMac());

	long ismm;
	SMM smm;
	SMM *psmm;
	AEV *paev;
	AEVSND *paevsnd;

	paev = (AEV *)_pggaev->QvFixedGet(iaev);
	paevsnd = (AEVSND *)_pggaev->QvGet(iaev);

	smm.aev = *paev;
	smm.aevsnd = *paevsnd;

	for (ismm = 0; ismm < _pglsmm->IvMac(); ismm++)
		{
		psmm = (SMM *)_pglsmm->QvGet(ismm);
		if (psmm->aevsnd.celn != paevsnd->celn)
			continue;
		if (psmm->aevsnd.sty != paevsnd->sty)
			continue;

		// Replace the current smm with the new
		_pglsmm->Put(ismm, &smm);
		return fTrue;
		}

	// No sound for this cel was found
	// Add a new mm sound
	return _pglsmm->FAdd(&smm);
}


/***************************************************************************
	
	Delete old motion sounds in event list

***************************************************************************/
bool ACTR::_FRemoveAevMm(long anid)
{
	AssertThis(0);

	long iaev;
	AEV *paev;
	AEVSND aevsnd;
	AEVACTN aevactn;

	// Remove motion match sounds from the previous action
	for (iaev = _iaevCur; iaev < _pggaev->IvMac(); iaev++)
		{
		paev = (AEV *)_pggaev->QvFixedGet(iaev);
		if (aetAdd == paev->aet)
			break;
		if (aetActn == paev->aet)
			{
			_pggaev->Get(iaev, &aevactn);
			if (aevactn.anid != anid)
				break;
			continue;
			}
		if (aetSnd != paev->aet)
			continue;

		// Test if sound is motion match
		_pggaev->Get(iaev, &aevsnd);
		if (smmNil == aevsnd.celn)
			continue;
		_RemoveAev(iaev);
		iaev--;
		}

	return fTrue;
}

/***************************************************************************
	
	Insert default motion match sounds in event list

***************************************************************************/
bool ACTR::_FAddAevDefMm(long anid)
{
	AssertThis(0);
	TAG tag;
	PMSND pmsnd;
	long iceln;
	long ccel;
	long vlm;
	long sty;
	bool fSoundExists;

	// Insert new motion match sounds
	if (!_ptmpl->FGetCcelActn(anid, &ccel))
		return fFalse;

	for (iceln = 0; iceln < ccel; iceln++)
		{
		if (!_ptmpl->FGetSndActnCel(anid, iceln, &fSoundExists, &tag))
			continue;		//Ignore failure
		if (!fSoundExists)
			continue;

		pmsnd = (PMSND)vptagm->PbacoFetch(&tag, MSND::FReadMsnd);
		if (pvNil == pmsnd)
			continue;  		//Ignore failure
	
		vlm = pmsnd->Vlm();
		sty = pmsnd->Sty();

		if (!FSetSndCore(&tag, fFalse, vlm, fFalse,	sty, fTrue))
			{
			goto LFail;		// Continuing pointless
			}

		ReleasePpo(&pmsnd);
		}

	 return fTrue;

LFail:
	ReleasePpo(&pmsnd);
	return fFalse;
}


/***************************************************************************
	
	Set the Volume for sounds in the current frame of the specified sty

***************************************************************************/
bool ACTR::FSetVlmSnd(long sty, bool fMotionMatch, long vlm)
{
	AssertThis(0);
	AssertIn(sty, 0, styLim);
	AssertIn(vlm, 0, kvlmFull + 1);

	AEVSND aevsnd;
	bool fActnThisFrame = fFalse;
	long celn;
	long ccel;
	long iaev;
	long ismm;
	AEV aev;
	SMM smm;
	long nfrmMM = ivNil;

	if (!_ptmpl->FGetCcelActn(_anidCur, &ccel))
		return fFalse;

	celn = _celnCur % ccel;

	// Set the volume for any events in the actor list for this frame
	for (iaev = _iaevFrmMin; iaev < _iaevCur; iaev++)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (aev.aet == aetActn)
			fActnThisFrame = fTrue;

		if (aev.aet != aetSnd)
			continue;
		
		_pggaev->Get(iaev, &aevsnd);
		if (FPure(aevsnd.celn != smmNil) != fMotionMatch)
		   	continue;

		if (aevsnd.sty != sty)
			continue;
		aevsnd.vlm = vlm;
		_pggaev->Put(iaev, &aevsnd);		
		}

	// Set the volume for any earlier motion match sounds
	// which land in this cel
	if (fMotionMatch && !fActnThisFrame)
		{
		for (iaev = _iaevFrmMin - 1; iaev > 0; iaev--)
			{
			_pggaev->GetFixed(iaev, &aev);

			// Quit when the action changes
			if (aev.aet == aetActn || (nfrmMM != ivNil && nfrmMM > aev.nfrm))
				break;

			if (aev.aet != aetSnd)
				continue;
			_pggaev->Get(iaev, &aevsnd);

			if (aevsnd.sty != sty)
				continue;

			// Only adjust sounds that match this cel
			if (aevsnd.celn != celn)
				continue;
			aevsnd.vlm = vlm;
			_pggaev->Put(iaev, &aevsnd);
			nfrmMM = aev.nfrm;
			// There are no queued motion match sounds by spec
			break;
			}
		}
	
	if (fMotionMatch)
		{
		// Adjust the volume in the smm also
		for (ismm = 0; ismm < _pglsmm->IvMac(); ismm++)
			{
			_pglsmm->Get(ismm, &smm);
			Assert(smm.aevsnd.celn != smmNil, "Logic error in smm");

			if (smm.aevsnd.celn != celn)
				continue;
		
			if (smm.aevsnd.sty != sty)
				continue;

			// Set the volume in the smm
			smm.aevsnd.vlm = vlm;
			_pglsmm->Put(ismm, &smm);
			}
		}

	_pscen->MarkDirty();
	return fTrue;	
}


/***************************************************************************
	
	Query actor for current frame sounds

***************************************************************************/
bool ACTR::FQuerySnd(long sty, bool fMotionMatch, PGL *pglTagSnd, long *pvlm, bool *pfLoop)
{
	AssertThis(0);
	AssertIn(sty, 0, styLim);
	AssertVarMem(pvlm);
	AssertVarMem(pfLoop);

	PGL pgltag = pvNil;
	AEVSND aevsnd;
	long ccel;
	long celn;
	AEV aev;
	long iaev;
	long ismm;
	SMM smm;

	if (pvNil == (pgltag = GL::PglNew(size(TAG), kctagSndGrow)))
		return fFalse;
	pgltag->SetMinGrow(kctagSndGrow);

	if (!fMotionMatch)
		{
		// Non-motion match sounds
		for (iaev = _iaevFrmMin; iaev < _iaevCur; iaev++)
			{
			_pggaev->GetFixed(iaev, &aev);
			if (aev.aet != aetSnd)
				continue;
		
			_pggaev->Get(iaev, &aevsnd);
			if (aevsnd.sty != sty)
				continue;

			// Handle motion match sounds later
			if (aevsnd.celn != smmNil)
				continue;
			if (aevsnd.tag.sid == ksidUseCrf)
				{
				if (!_pscen->Pmvie()->FResolveSndTag(&aevsnd.tag, aevsnd.chid))
					goto LFail;
				}
			if (!pgltag->FAdd(&aevsnd.tag))
				goto LFail;
			*pvlm = aevsnd.vlm;
			*pfLoop = aevsnd.fLoop;
			}
		}

	if (fMotionMatch)
		{
		if (!_ptmpl->FGetCcelActn(_anidCur, &ccel))
			goto LFail;

		celn = _celnCur % ccel;

		// Motion match sounds
		for (ismm = 0; ismm < _pglsmm->IvMac(); ismm++)
			{
			_pglsmm->Get(ismm, &smm);
			if (smm.aevsnd.sty != sty)
				continue;
			if (smm.aevsnd.celn != celn)
				continue;
			if (smm.aevsnd.tag.sid == ksidUseCrf)
				{
				if (!_pscen->Pmvie()->FResolveSndTag(&smm.aevsnd.tag, smm.aevsnd.chid))
					goto LFail;
				}
			if (!pgltag->FAdd(&smm.aevsnd.tag))
				goto LFail;
			*pvlm = smm.aevsnd.vlm;
			*pfLoop = smm.aevsnd.fLoop;
			break;
			}
		}

	if (pgltag->IvMac() == 0)
		{
		ReleasePpo(&pgltag);
		*pglTagSnd = pvNil;
		return fTrue;
		}

	*pglTagSnd = pgltag;
	return fTrue;

LFail:
	ReleasePpo(&pgltag);
	return fFalse;

}


/***************************************************************************
	
	Delete a sound in this frame

***************************************************************************/
bool ACTR::FDeleteSndCore(long sty, bool fMotionMatch)
{
	AssertThis(0);
	AssertIn(sty, 0, styLim);

	AEVSND aevsnd;
	long iaevFirst;
	long iaev;
	long ismm;
	long ccel;
	long celn;
	AEV aev;
	SMM smm;
	long nfrmMM = ivNil;		// For motion match, this may be an earlier frame

	// First the actor event list
	if (!_ptmpl->FGetCcelActn(_anidCur, &ccel))
		return fFalse;
	celn = _celnCur % ccel;

	iaevFirst = (fMotionMatch) ? _iaevActnCur : _iaevFrmMin;
	for (iaev = _iaevCur - 1; iaev >= iaevFirst; iaev--)
		{
		_pggaev->GetFixed(iaev, &aev);
		if (fMotionMatch && nfrmMM != ivNil && nfrmMM > aev.nfrm)
			break;

		if (aev.aet != aetSnd)
			continue;
		
		_pggaev->Get(iaev, &aevsnd);
		if (aevsnd.sty != sty)
			continue;
 		if ((aevsnd.celn != smmNil) != fMotionMatch)
		   	continue;

		// Only delete the sound that match this cel
		if (fMotionMatch && aevsnd.celn != celn)
			continue;

		// Delete this sound & continue to remove any
		// other sounds which are possibly chained
		_RemoveAev(iaev);
		nfrmMM = aev.nfrm;
		}

	
	if (fMotionMatch)
		{
	  	// Also the motion match sound list : smm
		if (!_ptmpl->FGetCcelActn(_anidCur, &ccel))
			return fFalse;
		celn = _celnCur % ccel;

		// Motion match sounds
		for (ismm = 0; ismm < _pglsmm->IvMac(); ismm++)
			{
			_pglsmm->Get(ismm, &smm);
			if (smm.aevsnd.sty != sty)
				continue;
			if (smm.aevsnd.celn != celn)
				continue;
			_pglsmm->Delete(ismm);
			ismm--;
			}
		}

	_pscen->MarkDirty();
	return fTrue;
}


/******************************************************************************
	FSoundInFrm
		Enumerates all actor events for current frame looking for a sound
		event.
	
	Returns: fTrue if a sound event was found, fFalse otherwise
	
************************************************************ PETED ***********/
bool ACTR::FSoundInFrm(void)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	/* Can't provide useful info if we're not at the current scene frame */
	if (Pscen()->Nfrm() != _nfrmCur)
		return fFalse;

	for (long iaev = _iaevFrmMin; iaev < _iaevCur; iaev++)
		{
		AEV aev;

		_pggaev->GetFixed(iaev, &aev);
		if (aev.aet == aetSnd)
			return fTrue;
		}
	return fFalse;
}

/******************************************************************************
	Resolve all sound tags
	
******************************************************************************/
bool ACTR::FResolveAllSndTags(CNO cnoScen)
{
	AssertThis(0);
	long iaev;
	AEV *paev;
	AEVSND aevsnd;
	bool fSuccess = fTrue;

	for (iaev = 0; iaev < _pggaev->IvMac(); iaev++)
		{
		paev = (AEV *)_pggaev->QvFixedGet(iaev);
		if (paev->aet != aetSnd)
			continue;
		_pggaev->Get(iaev, &aevsnd);
		if (aevsnd.tag.sid == ksidUseCrf)
			{
			if (!_pscen->Pmvie()->FResolveSndTag(&aevsnd.tag, aevsnd.chid, cnoScen))
				{
				fSuccess = fFalse;
				continue;
				}
			_pggaev->Put(iaev, &aevsnd);
			}
		}
	return fSuccess;
}
