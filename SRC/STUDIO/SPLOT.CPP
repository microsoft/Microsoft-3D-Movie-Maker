/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	splot.cpp: Splot machine class

	Primary Author: ******
	Review Status: Reviewed

***************************************************************************/
#include "studio.h"
ASSERTNAME

RTCLASS(SPLOT)

BEGIN_CMD_MAP(SPLOT, GOK)
	ON_CID_GEN(cidSplotInit, FCmdInit, pvNil)
	ON_CID_GEN(cidSplotDo, FCmdSplot, pvNil)
	ON_CID_GEN(cidSplotUpdate, FCmdUpdate, pvNil)
	ON_CID_GEN(cidSplotOk, FCmdDismiss, pvNil)
	ON_CID_GEN(cidSplotCancel, FCmdDismiss, pvNil)
END_CMD_MAP_NIL()


/******************************************************************************
	PsplotNew
		Creates a new SPLOT instance.
	
	Returns: pvNil on failure, pointer to SPLOT on success
	
************************************************************ PETED ***********/
PSPLOT SPLOT::PsplotNew(long hidPar, long hid, PRCA prca)
{
	PSPLOT psplot = pvNil;
	PGL pglclr = pvNil;
	PGOB pgobPar;
	RC rcRel;
	GCB gcb;
	CLOK clok(CMH::HidUnique());

	if ((pgobPar = vapp.Pkwa()->PgobFromHid(hidPar)) == pvNil)
		{
		Bug("Couldn't find background GOB");
		goto LFail;
		}

	pglclr = GPT::PglclrGetPalette();
	if (pglclr == pvNil)
		goto LFail;

	rcRel.Set(krelZero, krelZero, krelOne, krelOne);
	gcb.Set(hid, pgobPar, fgobNil, kginDefault, pvNil, &rcRel);

	if ((psplot = NewObj SPLOT(&gcb)) == pvNil)
		goto LFail;

	if (!psplot->_FInit(vpapp->Pkwa(), hid, prca))
		{
		ReleasePpo(&psplot);
		goto LFail;
		}
	if (!psplot->_FEnterState(ksnoInit))
		{
		Warn("GOK immediately destroyed!");
		goto LFail;
		}

	/* Copy random number generator and palette */
	psplot->_pglclrSav = pglclr;
	psplot->_pglclrSav->AddRef();

LFail:
	ReleasePpo(&pglclr);
	return psplot;
}


/******************************************************************************
	FCmdInit
		Initializes the splot machine; right now, just sets up the movie
		and MVU for the splot machine, inside the given GOK parent.  In the
		future, perhaps the content data structures can be inited here, and
		only updated if we fail loading something (which would indicate that
		the user removed a CD or something during the splot machine).
	
	Arguments:
		PCMD pcmd
			rglw[0]  --  the kid of the parent frame to show the movie in
	
	Returns:  fTrue, always handles the cid
	
************************************************************ PETED ***********/
bool SPLOT::FCmdInit(PCMD pcmd)
{
	AssertThis(0);
	Assert(_pmvie == pvNil, "Already Inited the Splot Machine");

	PMCC pmcc = pvNil;
	PGOB pgobParent;
	RC rcRel;
	GCB gcb;

	if ((pgobParent = vpapp->Pkwa()->PgobFromHid(pcmd->rglw[0])) == pvNil)
		{
		Bug("Parent GOB for view doesn't exist");
		goto LFail;
		}

	pmcc = NewObj MCC(kdxpWorkspace, kdypWorkspace, kcbStudioCache);
	if (pmcc == pvNil)
		goto LFail;

	if ((_pmvie = MVIE::PmvieNew(vpapp->FSlowCPU(), pmcc)) == pvNil)
		goto LFail;

	_pmvie->SetFSoundsEnabled(fTrue);
	rcRel.Set(krelZero, krelZero, krelOne, krelOne);
	gcb.Set(khidDdg, pgobParent, fgobNil, kginDefault, pvNil, &rcRel);
	if (_pmvie->PddgNew(&gcb) == pvNil)
		{
		ReleasePpo(&_pmvie);
		goto LFail;
		}
	_pmvie->InvalViews();

LFail:
	ReleasePpo(&pmcc);
	return fTrue;
}


/******************************************************************************
	FCmdSplot
		Actually generates some random content for the splot machine.  Grab
		content here, since sometimes we'll have to retrieve content here
		anyway, so it's cleaner to just always do it here.
	
	Arguments:
		PCMD pcmd
			rglw[0]  --  1 if the background should be randomized
			rglw[1]  --  1 if the camera should be
			rglw[2]  --  1 if the actors/props should be
			rglw[3]  --  1 if the music should be
	
	Returns: fTrue, always handles the cid
	
************************************************************ PETED ***********/
bool SPLOT::FCmdSplot(PCMD pcmd)
{
	AssertThis(0);

	bool fDirty = fFalse;
	CKI cki;
	THD thd;

	vapp.BeginLongOp();

	/* On failure, pretend that we didn't change anything */
	_fDirty = fFalse;

	/* Background.  New background implies new camera view */
	if (_pbclBkgd == pvNil)
		{
		cki.ctg = kctgBkth;
		cki.cno = cnoNil;
		_pbclBkgd = BCL::PbclNew(pvNil, &cki, ctgNil, pvNil, fTrue);
		if (_pbclBkgd == pvNil || _pbclBkgd->IthdMac() == 0)
			{
			ReleasePpo(&_pbclBkgd);
			goto LFail;
			}
		pcmd->rglw[0] = 1;
		}
	if (pcmd->rglw[0] != 0)
		{
		long ithdBkgd;

		ithdBkgd = _sflBkgd.LwNext(_pbclBkgd->IthdMac());
		if (ithdBkgd != _ithdBkgd)
			ReleasePpo(&_pbclCam);
		_ithdBkgd = ithdBkgd;
		pcmd->rglw[1] = 1;
		fDirty = fTrue;
		}

	/* Camera */
	if (_pbclCam == pvNil)
		{
		_pbclBkgd->GetThd(_ithdBkgd, &thd);
		cki.ctg = kctgBkth;
		cki.cno = thd.tag.cno;
		_pbclCam = BCL::PbclNew(pvNil, &cki, kctgCath, pvNil, fTrue);
		if (_pbclCam == pvNil || _pbclCam->IthdMac() == 0)
			{
			ReleasePpo(&_pbclCam);
			goto LFail;
			}

		pcmd->rglw[1] = 1;
		}
	if (pcmd->rglw[1] != 0)
		{
		_ithdCam = _sflCam.LwNext(_pbclCam->IthdMac());
		fDirty = fTrue;
		}

	/* Actor & Props */
	if (_pbclActr == pvNil)
		{
		cki.ctg = kctgTmth;
		cki.cno = cnoNil;
		_pbclActr = BCL::PbclNew(pvNil, &cki, ctgNil, pvNil, fTrue);
		if (_pbclActr == pvNil || _pbclActr->IthdMac() == 0)
			{
			ReleasePpo(&_pbclActr);
			goto LFail;
			}
		pcmd->rglw[2] = 1;
		}
	if (_pbclProp == pvNil)
		{
		cki.ctg = kctgPrth;
		cki.cno = cnoNil;
		_pbclProp = BCL::PbclNew(pvNil, &cki, ctgNil, pvNil, fTrue);
		if (_pbclProp == pvNil || _pbclProp->IthdMac() == 0)
			{
			ReleasePpo(&_pbclProp);
			goto LFail;
			}
		pcmd->rglw[2] = 1;
		}
	if (pcmd->rglw[2] != 0)
		{
		_ithdActr = _sflActr.LwNext(_pbclActr->IthdMac());
		_ithdProp = _sflProp.LwNext(_pbclProp->IthdMac());
		fDirty = fTrue;
		}

	/* Background music */
	if (_pbclSound == pvNil)
		{
		cki.ctg = kctgSmth;
		cki.cno = cnoNil;
		_pbclSound = BCL::PbclNew(pvNil, &cki, ctgNil, pvNil, fTrue);
		if (_pbclSound == pvNil || _pbclSound->IthdMac() == 0)
			{
			ReleasePpo(&_pbclSound);
			goto LFail;
			}
		pcmd->rglw[3] = 1;
		}
	if (pcmd->rglw[3] != 0)
		{
		_ithdSound = _sflSound.LwNext(_pbclSound->IthdMac());
		fDirty = fTrue;
		}

	Assert(fDirty, "Why bother calling this with no random elements?");
	_fDirty = fDirty;
LFail:
	vapp.EndLongOp();
	return fTrue;
}


/******************************************************************************
	FCmdUpdate
		Show the new movie on the screen.
	
	Arguments:
		PCMD pcmd  --  no additional parameters
	
	Returns: fTrue, always handles the cid
	
************************************************************ PETED ***********/
bool SPLOT::FCmdUpdate(PCMD pcmd)
{
	AssertThis(0);

	if (_fDirty)
		{
		THD thd;
		TAG tagCam, tagBkgd;

		vapp.BeginLongOp();

		if (_pmvie->Cscen() > 0 && !_pmvie->FRemScen(0))
			goto LFail;

		_pmvie->Pmsq()->SndOff();

#ifdef BUG1907
		// We will run out of disk space eventually if the HD cache is not 
		// periodically cleared.  We don't want to do it on every pull of the
		// lever, because there's a significant time hit (especially if you're
		// only pulling a "small lever", which only has to cache one new thing
		// unless you clear the cache).  Ideally, this algorithm would only
		// purge the cache if disk space is getting low on the volume that 
		// tagman is caching to, but there's currently no way to do that, so
		// just purge every five pulls.
		static long _cactPullTilClearCache = 5;

		_pmvie->Pmsq()->StopAll(); // Make sure no sounds are streaming from HD cache
		if (--_cactPullTilClearCache == 0)
			{
			vptagm->ClearCache(sidNil, ftagmFile); // Clear content out of HD cache
			_cactPullTilClearCache = 5;
			}
#endif //BUG1907

		/* No tags need to be opened, because all are from installed content */
		/* Still need to ensure on HD though */
		_pbclBkgd->GetThd(_ithdBkgd, &thd);
		Assert(thd.tag.sid != ksidUseCrf, "Need to open tag before using it");
		if (!BKGD::FCacheToHD(&thd.tag))
			goto LFail;
		if (!_pmvie->FAddScen(&thd.tag))
			goto LFail;
		tagBkgd = thd.tag;
		AssertPo(_pmvie->Pscen(), 0);

		_pmvie->Pmsq()->SndOnLong();

		_pbclCam->GetThd(_ithdCam, &thd);
		if (!vptagm->FBuildChildTag(&tagBkgd, thd.chid, kctgCam, &tagCam))
			goto LFail;
		if (!vptagm->FCacheTagToHD(&tagCam))
			goto LFail;
		if (!_pmvie->Pscen()->FChangeCam(thd.chid))
			goto LFail;

		_pbclActr->GetThd(_ithdActr, &thd);
		Assert(thd.tag.sid != ksidUseCrf, "Need to open tag before using it");
		if (!vptagm->FCacheTagToHD(&thd.tag))
			goto LFail;
		if (!_pmvie->FInsActr(&thd.tag))
			goto LFail;

		_pbclProp->GetThd(_ithdProp, &thd);
		Assert(thd.tag.sid != ksidUseCrf, "Need to open tag before using it");
		if (!vptagm->FCacheTagToHD(&thd.tag))
			goto LFail;
		if (!_pmvie->FInsActr(&thd.tag))
			goto LFail;

		_pmvie->Pscen()->SelectActr(pvNil);
		_pmvie->PmvuCur()->SetTool(toolDefault);

		_pbclSound->GetThd(_ithdSound, &thd);
		Assert(thd.tag.sid != ksidUseCrf, "Need to open tag before using it");
		if (!vptagm->FCacheTagToHD(&thd.tag))
			goto LFail;
		if (!_pmvie->FAddBkgdSnd(&thd.tag, fTrue, fFalse))
			goto LFail;

		_pmvie->Pmsq()->PlayMsq();
		
		vapp.EndLongOp();
		}

LFail:
	return fTrue;
}


/******************************************************************************
	FCmdDismiss
		Okays or Cancels the Splot Machine.
	
	Arguments:
		PCMD pcmd  --  no additional parameters
	
	Returns: fTrue, always handles the cid
	
************************************************************ PETED ***********/
bool SPLOT::FCmdDismiss(PCMD pcmd)
{
	AssertThis(0);

	if (pcmd->cid == cidSplotOk)
		{
		/* Attempt to release the current scene so that we'll force a palette
			update when we come back into the studio. Don't sweat a failure */
		_pmvie->FSwitchScen(ivNil);
		_pmvie->ClearUndo();
		vapp.HandoffMovie(_pmvie);
		}
#ifdef BUG1907
	else
		{
		_pmvie->Pmsq()->StopAll(); // Make sure no sounds are streaming from HD cache
		vptagm->ClearCache(sidNil, ftagmFile); // Clear content out of HD cache
		// Note: could clear out the RAM cache too, but I'm keeping this change 
		// as small as possible.
		}
#endif //BUG1907

	Release();

	return fTrue;
}

SPLOT::~SPLOT(void)
{
	if (_pmvie != pvNil)
		{
		/* This should be freed when its parent, the Splot Machine View gob, is
			freed */
		Assert(_pmvie->PddgGet(0) == pvNil, "MVU wasn't freed");
	
		_pmvie->Pmsq()->StopAll();
		_pmvie->Pmsq()->SndOnShort();
		}

	if (_pglclrSav != pvNil)
		{
		vapp.UpdateMarked();
		GPT::SetActiveColors(_pglclrSav, fpalIdentity);
		ReleasePpo(&_pglclrSav);
		}

	ReleasePpo(&_pmvie);
	ReleasePpo(&_pbclBkgd);
	ReleasePpo(&_pbclCam);
	ReleasePpo(&_pbclActr);
	ReleasePpo(&_pbclProp);
	ReleasePpo(&_pbclSound);
}

#ifdef DEBUG
void SPLOT::AssertValid(ulong grf)
{
	SPLOT_PAR::AssertValid(grf);
	AssertPo(_pglclrSav, 0);

	AssertNilOrPo(_pmvie, 0);
	AssertNilOrPo(_pbclBkgd, 0);
	AssertNilOrPo(_pbclCam, 0);
	AssertNilOrPo(_pbclActr, 0);
	AssertNilOrPo(_pbclProp, 0);
	AssertNilOrPo(_pbclSound, 0);
}

void SPLOT::MarkMem(void)
{
	SPLOT_PAR::MarkMem();
	MarkMemObj(_pglclrSav);
	MarkMemObj(_pmvie);
	MarkMemObj(_pbclBkgd);
	MarkMemObj(_pbclCam);
	MarkMemObj(_pbclActr);
	MarkMemObj(_pbclProp);
	MarkMemObj(_pbclSound);
	_sflBkgd.MarkMem();
	_sflCam.MarkMem();
	_sflActr.MarkMem();
	_sflProp.MarkMem();
	_sflSound.MarkMem();
}
#endif // DEBUG
