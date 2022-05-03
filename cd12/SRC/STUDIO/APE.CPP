/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	ape.cpp: Actor preview entity class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	The APE is used to preview a single actor.  The APE doesn't actually
	contain an ACTR...more of an "ACTR Jr.": a _ptmpl, _pbody, _anid, and
	a _pbwld to display in.  If fCycleCels is set to fTrue in PapeNew(),
	the APE will cycle through the cels of the current action.

	APE supports some simple editing operations, such as changing
	materials and TDT properties.  The client must query these changes
	before destroying the APE to make changes to the real actor.

	The APE is currently used by the action browser, TDT easel, and
	costume changer easel.

***************************************************************************/
#include "studio.h"
ASSERTNAME

#define kdtimFrame (kdtimSecond / kfps) // clock ticks per frame

const BRA kaFov = BR_ANGLE_DEG(60.0);	// camera field of view

RTCLASS(APE)

BEGIN_CMD_MAP(APE, GOB)
	ON_CID_GEN(cidAlarm, FCmdNextCel, pvNil)
END_CMD_MAP_NIL()


/***************************************************************************
	Create a new APE
***************************************************************************/
PAPE APE::PapeNew(PGCB pgcb, PTMPL ptmpl, PCOST pcost, long anid,
	bool fCycleCels, PRCA prca)
{
	AssertVarMem(pgcb);
	AssertPo(ptmpl, 0);
	AssertNilOrPo(pcost, 0);
	AssertNilOrPo(prca, 0);

	PAPE pape;

	pape = NewObj APE(pgcb);
	if (pvNil == pape)
		return pvNil;
	if (!pape->_FInit(ptmpl, pcost, anid, fCycleCels, prca))
		{
		ReleasePpo(&pape);
		return pvNil;
		}
	return pape;
}


/***************************************************************************
	Set up the APE
***************************************************************************/
bool APE::_FInit(PTMPL ptmpl, PCOST pcost, long anid, bool fCycleCels,
	PRCA prca)
{
	AssertBaseThis(0);
	AssertPo(ptmpl, 0);
	AssertNilOrPo(pcost, 0);

	long cbset;
	long ibset;
	RC rc;
	GMS gms;

	_fCycleCels = fCycleCels;
	_prca = prca;
	_ptmpl = ptmpl;
	_ptmpl->AddRef();
	_pbody = _ptmpl->PbodyCreate();	// note: also sets default costume
	if (pvNil == _pbody)
		return fFalse;
	if (pvNil != pcost)
		pcost->Set(_pbody);
	cbset = _pbody->Cbset();

	// If there is exactly one accessory body part set, set _ibsetOnlyAcc 
	// to that ibset.  Else set it to ivNil. 
	_ibsetOnlyAcc = ivNil;
	for (ibset = 0; ibset < cbset; ibset++)
		{
		if (_ptmpl->FBsetIsAccessory(ibset))
			{
			if (ivNil == _ibsetOnlyAcc)
				{
				// First one found.  Remember it and keep looking
				_ibsetOnlyAcc = ibset;
				}
			else
				{
				// Found a second accessory ibset.  Forget it.
				_ibsetOnlyAcc = ivNil;
				break;
				}
			}
		}

	_pglgms = GL::PglNew(size(GMS), cbset);
	if (pvNil == _pglgms)
		return fFalse;
	AssertDo(_pglgms->FSetIvMac(cbset), "PglNew should have ensured space");
	TrashVar(&gms);
	gms.fValid = fFalse;
	for (ibset = 0; ibset < cbset; ibset++)
		_pglgms->Put(ibset, &gms);

	GetRc(&rc, cooLocal);
	_pbwld = BWLD::PbwldNew(rc.Dxp(), rc.Dyp());
	if (pvNil == _pbwld)
		return fFalse;

	// Add a light source to the BWLD
	_blit.type = BR_LIGHT_DIRECT;
	_blit.colour = BR_COLOUR_RGB(0xff, 0xff, 0xff);
	_blit.attenuation_c = rOne;
	_bact.type = BR_ACTOR_LIGHT;
	_bact.type_data = &_blit;
	_bact.t.type = BR_TRANSFORM_MATRIX34;
	BrMatrix34Identity(&_bact.t.t.mat);
	BrMatrix34PostRotateX(&_bact.t.t.mat, BR_ANGLE_DEG(-40.0));
	BrMatrix34PostRotateY(&_bact.t.t.mat, BR_ANGLE_DEG(-40.0));
	_pbwld->AddActor(&_bact);
	BrLightEnable(&_bact);
	_pbody->SetBwld(_pbwld);	
	_pbody->Show();

	_InitView();
	if (!FSetAction(anid))
		return fFalse;
	if (fCycleCels)
		{
		_clok.Start(0);
		if (!_clok.FSetAlarm(0, this))
			return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Destroy the APE
***************************************************************************/
APE::~APE()
{
	AssertBaseThis(0);
	
	// If _blit is initialized, then it was added to _pbwld and should
	// be removed and disabled.  We check whether _blit is initialized by
	// seeing if _blit.type has been changed from the initial 0.
	Assert(BR_LIGHT_DIRECT != 0,
		"need a new test for whether _blit was added");
	if (_blit.type == BR_LIGHT_DIRECT)
		{
		BrLightDisable(&_bact);
		BrActorRemove(&_bact);
		}
	ReleasePpo(&_ptmpl);
	ReleasePpo(&_pbody);
	ReleasePpo(&_pbwld);
	ReleasePpo(&_pglgms);
}


/***************************************************************************
	Load the brush with ptagMtrl (for a stock material)
***************************************************************************/
void APE::SetToolMtrl(PTAG ptagMtrl)
{
	AssertThis(0);
	AssertVarMem(ptagMtrl);

	_apet.apt = aptGms;
	_apet.gms.fValid = fTrue;
	_apet.gms.fMtrl = fTrue;
	_apet.gms.tagMtrl = *ptagMtrl;
	TrashVar(&_apet.gms.cmid);
}


/***************************************************************************
	Load the brush with cmid (for a custom material)
***************************************************************************/
void APE::SetToolCmtl(long cmid)
{
	AssertThis(0);

	_apet.apt = aptGms;
	_apet.gms.fValid = fTrue;
	_apet.gms.fMtrl = fFalse;
	_apet.gms.cmid = cmid;
	TrashVar(&_apet.gms.tagMtrl);
}


/***************************************************************************
	Load the brush with the aptIncCmtl tool
***************************************************************************/
void APE::SetToolIncCmtl(void)
{
	AssertThis(0);

	_apet.apt = aptIncCmtl;
	TrashVar(&_apet.gms);
	_apet.gms.fValid = fFalse;
}


/***************************************************************************
	Load the brush with the aptIncAccessory tool
***************************************************************************/
void APE::SetToolIncAccessory(void)
{
	AssertThis(0);

	_apet.apt = aptIncAccessory;
	TrashVar(&_apet.gms);
	_apet.gms.fValid = fFalse;
}


/***************************************************************************
	Change the actor's action to anid
***************************************************************************/
bool APE::FSetAction(long anid)
{
	AssertThis(0);

	if (!_ptmpl->FSetActnCel(_pbody, anid, 0))
		return fFalse;
	_anid = anid;
	_celn = 0;
	_SetScale();
	_UpdateView();
	return fTrue;
}


/***************************************************************************
	Time to move to the next cel of the action.  Resets alarm
***************************************************************************/
bool APE::FCmdNextCel(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (_fCycleCels)
		{
		FDisplayCel(_celn + 1);
		}

  	_clok.FSetAlarm(kdtimFrame, this);
	return fTrue;
}


/***************************************************************************
	Time to move to the next cel of the action.
***************************************************************************/
bool APE::FDisplayCel(long celn)
{
	AssertThis(0);

	// Someone else will push an error if this fails
	if (_ptmpl->FSetActnCel(_pbody, _anid, celn))
		{
		_celn = celn;
		_UpdateView();
		}
			
	return fTrue;
}

/***************************************************************************
	Enable or disable cel cycling.
***************************************************************************/
void APE::SetCycleCels(bool fOn)
{
	AssertThis(0);
	_fCycleCels = fOn;
}


/***************************************************************************
	Go to the default view
***************************************************************************/
void APE::_InitView(void)
{
	AssertThis(0);

	BMAT34 bmat34;
	BRA xa, ya, za;

	_iview = 0;
	_ptmpl->GetRestOrien(&xa, &ya, &za);
	BrMatrix34Identity(&bmat34);
	BrMatrix34PostRotateX(&bmat34, xa);
	BrMatrix34PostRotateY(&bmat34, ya);
	BrMatrix34PostRotateZ(&bmat34, za);
	_pbody->LocateOrient(rZero, rZero, rZero, &bmat34);
	_SetScale();
	_UpdateView();
}


/***************************************************************************
	Set a custom view
***************************************************************************/
void APE::SetCustomView(BRA xa, BRA ya, BRA za)
{
	AssertThis(0);

	BMAT34 bmat34;
	BRA xaTmpl, yaTmpl, zaTmpl;

	_iview = -1;
	_ptmpl->GetRestOrien(&xaTmpl, &yaTmpl, &zaTmpl);
	BrMatrix34Identity(&bmat34);
	BrMatrix34PostRotateX(&bmat34, xaTmpl);
	BrMatrix34PostRotateY(&bmat34, yaTmpl);
	BrMatrix34PostRotateZ(&bmat34, zaTmpl);
	BrMatrix34PostRotateX(&bmat34, xa);
	BrMatrix34PostRotateY(&bmat34, ya);
	BrMatrix34PostRotateZ(&bmat34, za);
	_pbody->LocateOrient(rZero, rZero, rZero, &bmat34);
	_SetScale();
	_UpdateView();
}


/***************************************************************************
	Rotate the BODY to see a different view
***************************************************************************/
void APE::ChangeView(void)
{
	AssertThis(0);

	BMAT34 bmat34;
	BRA xa, ya, za;

	_iview = (_iview + 1) % 5;
	_ptmpl->GetRestOrien(&xa, &ya, &za);
	BrMatrix34Identity(&bmat34);
	BrMatrix34PostRotateX(&bmat34, xa);
	BrMatrix34PostRotateY(&bmat34, ya);
	BrMatrix34PostRotateZ(&bmat34, za);

	switch(_iview)
		{
	case 0:
		_InitView();
		return;
	case 1:
		BrMatrix34PostRotateY(&bmat34, BR_ANGLE_DEG(90.0));
		break;
	case 2:
		BrMatrix34PostRotateY(&bmat34, BR_ANGLE_DEG(180.0));
		break;
	case 3:
		BrMatrix34PostRotateY(&bmat34, BR_ANGLE_DEG(270.0));
		break;
	case 4:
		BrMatrix34PostRotateX(&bmat34, BR_ANGLE_DEG(90.0));
		break;
		}

	_pbody->LocateOrient(rZero, rZero, rZero, &bmat34);
	_SetScale();
	_UpdateView();
}


/***************************************************************************
	Position the camera so the actor is centered and properly sized
***************************************************************************/
void APE::_SetScale(void)
{
	AssertThis(0);

	RC rcView;
 	BCB bcbBody; // 3-D bounds of BODY
	BRS dxrBody, dyrBody, dzrBody;
	BRS rDydxBody;
	BRS xrCam, yrCam, zrCam;
	BRS zrHither, zrYon;
	BMAT34 bmat34Camera;
	BRS dxrView, dyrView;
	BRS rDydxView;
	BRA aFov;
	BRS rAtan;

	_pbody->GetBcbBounds(&bcbBody, fTrue);
	dxrBody = bcbBody.xrMax - bcbBody.xrMin;
	dyrBody = bcbBody.yrMax - bcbBody.yrMin;
	dzrBody = bcbBody.zrMax - bcbBody.zrMin;

	rDydxBody = BrsDiv(dyrBody, dxrBody);

	// The x and y camera distances are functions of the body only
	xrCam = BrsHalf(bcbBody.xrMax + bcbBody.xrMin);
	yrCam = BrsHalf(bcbBody.yrMax + bcbBody.yrMin);

 	GetRc(&rcView, cooLocal);
	dxrView = BrIntToScalar(rcView.Dxp());
	dyrView = BrIntToScalar(rcView.Dyp());
	rDydxView = BrsDiv(dyrView, dxrView);

 	aFov = BrScalarToAngle(BrsHalf(BrAngleToScalar(kaFov)));
	rAtan = BrsDiv(BR_COS(aFov), BR_SIN(aFov));

	if (rDydxBody > rDydxView)
		{
		// Tall actor : Compute z based on y
		zrCam = BrsMul(BrsHalf(dyrBody), rAtan);
		}
	else
		{
		// Wide actor : Compute z based on x
		// Fit the actor width into a narrow rc
		dxrBody = BrsMul(dxrBody, rDydxView);
		zrCam = BrsMul(BrsHalf(dxrBody), rAtan);
		}

	// Adjust for body depth
	zrCam += bcbBody.zrMax;

	// Set hither and yon.  Note: there's probably a good algorithm to
	//    compute the optimal values for zrHither and zrYon, but I
	//    don't have time to figure it out right now. :-) The goal is
	//    to make zrHither as large as possible and zrYon as small as
	//    possible without intersecting the actor.  -*****
	if (zrCam < BR_SCALAR(10.0))
		zrHither = BR_SCALAR(0.1);
	else
		zrHither = rOne;
	zrYon = BR_SCALAR(1000.0);

	BrMatrix34Translate(&bmat34Camera, xrCam, yrCam, zrCam);
	_pbwld->SetCamera(&bmat34Camera, zrHither, zrYon, kaFov);
}


/***************************************************************************
	Render and force redraw of the APE
***************************************************************************/
void APE::_UpdateView(void)
{
	AssertThis(0);

	_pbwld->Render();
	_pbwld->MarkRenderedRegn(this, 0, 0);
}


/***************************************************************************
	Draw the contents of the APE's bwld
***************************************************************************/
void APE::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);

	_pbwld->Draw(pgnv, prcClip, 0, 0);
}


/***************************************************************************
	Set the cursor appropriately
***************************************************************************/
bool APE::FCmdMouseMove(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PBACT pbact;
	PBODY pbody;
	long ibset;
	long ibsetAcc;

	if (pvNil != _prca)
		{
		SetCursCno(_prca, kcrsDefault);
		if (_pbwld->FClickedActor(pcmd->xp, pcmd->yp, &pbact))
			{
			pbody = BODY::PbodyFromBact(pbact, &ibset);
			Assert(pbody == _pbody, "what BODY is this?");
			if (_apet.apt == aptIncCmtl)
				{
				if (_ptmpl->CcmidOfBset(ibset) > 1)
					SetCursCno(_prca, kcrsCostume);
				}
			else if (_apet.apt == aptIncAccessory)
				{
				// Change cursor if there's only one accessory
				// body part set or if cursor is on either an accessory
				// body part set or the parent of one.
				if (_ibsetOnlyAcc != ivNil || 
					_ptmpl->FIbsetAccOfIbset(ibset, &ibsetAcc))
					{
					SetCursCno(_prca, kcrsHand);
					}
				}
			}
		}
	return APE_PAR::FCmdMouseMove(pcmd);
}


/***************************************************************************
	Handle mouse-down.  Do the right thing depending on the current tool.
***************************************************************************/
bool APE::FCmdTrackMouse(PCMD_MOUSE pcmd)
{
	AssertThis(0);

	PBACT pbact;
	PBODY pbody;
	long ibset;
	long ibsetAcc;
	GMS gms;

	if (pcmd->cid != cidMouseDown)
		{
		Bug("Should only get a mousedown here!");
		return fTrue;
		}
	if (!_pbwld->FClickedActor(pcmd->xp, pcmd->yp, &pbact))
		return fTrue;
	pbody = BODY::PbodyFromBact(pbact, &ibset);
	Assert(pbody == _pbody, "what BODY is this?");

	switch(_apet.apt)
		{
	case aptNil:
		break;

	case aptGms:
		gms = _apet.gms;
		Assert(gms.fValid, 0);
		if (_FApplyGms(&gms, ibset))
			_pglgms->Put(ibset, &gms);
		break;

	case aptIncCmtl:
		_pglgms->Get(ibset, &gms);
		if (_FIncCmtl(&gms, ibset, fFalse))
			_pglgms->Put(ibset, &gms);
		break;

	case aptIncAccessory:
		if ((ibsetAcc = _ibsetOnlyAcc) != ivNil ||
			_ptmpl->FIbsetAccOfIbset(ibset, &ibsetAcc))
			{
			_pglgms->Get(ibsetAcc, &gms);
			if (_FIncCmtl(&gms, ibsetAcc, fTrue))
				_pglgms->Put(ibsetAcc, &gms);
			_SetScale();
			}
		break;

	default:
		BugVar("weird tool", &_apet.apt);
		break;
		}

	_UpdateView();
	return fTrue;
}


/***************************************************************************
	Apply a GMS to ibset
***************************************************************************/
bool APE::_FApplyGms(GMS *pgms, long ibset)
{
	AssertThis(0);
	AssertVarMem(pgms);
	Assert(pgms->fValid, "bad gms");
	AssertIn(ibset, 0, _pbody->Cbset());

	PMTRL pmtrl;
	PCMTL pcmtl;

	if (pgms->fMtrl)
		{
		pmtrl = (PMTRL)vptagm->PbacoFetch(&pgms->tagMtrl, MTRL::FReadMtrl);
		if (pvNil == pmtrl)
			return fFalse;
		_pbody->SetPartSetMtrl(ibset, pmtrl);
		ReleasePpo(&pmtrl);
		}
	else // cmtl
		{
		pcmtl = _ptmpl->PcmtlFetch(pgms->cmid);
		if (pvNil == pcmtl)
			return fFalse;
		_pbody->SetPartSetCmtl(pcmtl);
		ReleasePpo(&pcmtl);
		}
	return fTrue;
}


/***************************************************************************
	Fill in pgms with the next CMTL available for ibset and applies it
	to _pbody.
***************************************************************************/
bool APE::_FIncCmtl(GMS *pgms, long ibset, bool fNextAccessory)
{
	AssertThis(0);
	AssertVarMem(pgms);
	AssertIn(ibset, 0, _pbody->Cbset());

	long cmid;
	long cmidNext;
	long icmid;
	long ccmid;
	PCMTL pcmtl;
	PCMTL pcmtlOld;
	PMTRL pmtrlOld;
	bool fMtrl;

	// Default cmidNext is default cmid for this body part set
	cmidNext = _ptmpl->CmidOfBset(ibset, 0);

	ccmid = _ptmpl->CcmidOfBset(ibset);

	// Need to find out what cmid (if any) is currently attached to ibset.
	// If the GMS is valid, use it.  Otherwise, figure out which cmid
	// generates the same pcmtl that is currently attached to the body.
	if (!pgms->fValid || pgms->fMtrl)
		{
		// GMS is useless...look at the body
		_pbody->GetPartSetMaterial(ibset, &fMtrl, &pmtrlOld, &pcmtlOld);
		if (!fMtrl)
			{	
			for (icmid = 0; icmid < ccmid; icmid++)
				{
				cmid = _ptmpl->CmidOfBset(ibset, icmid);
				pcmtl = _ptmpl->PcmtlFetch(cmid);
				if (pvNil == pcmtl)
					return fFalse;
				if (pcmtl == pcmtlOld)
					{
					ReleasePpo(&pcmtl);
					cmidNext = _CmidNext(ibset, icmid, fNextAccessory);
					break;
					}
				ReleasePpo(&pcmtl);
				}
			}
		}
	else
		{
		// GMS has current cmid
		cmid = pgms->cmid;
		for (icmid = 0; icmid < ccmid; icmid++)
			{
			if (cmid == _ptmpl->CmidOfBset(ibset, icmid))
				{
				cmidNext = _CmidNext(ibset, icmid, fNextAccessory);
				break;
				}
			}
		}
	pcmtl = _ptmpl->PcmtlFetch(cmidNext);
	if (pvNil == pcmtl)
		return fFalse;
	_pbody->SetPartSetCmtl(pcmtl);
	ReleasePpo(&pcmtl);
	pgms->cmid = cmidNext;
	pgms->fMtrl = fFalse;
	pgms->fValid = fTrue;
	return fTrue;
}


/***************************************************************************
	Return the CMID that follows the given CMID.  If fNextAccessory, it
	returns the next CMID that has a different set of models than icmidCur.
	Otherwise it returns the next CMID that has the same set of models as
	icmidCur.
***************************************************************************/
long APE::_CmidNext(long ibset, long icmidCur, bool fNextAccessory)
{
	AssertThis(0);

	long cmidCur;
	long ccmid;
	long icmidNext;
	long cmidNext;
	long dicmid;

	cmidCur = _ptmpl->CmidOfBset(ibset, icmidCur);
	ccmid = _ptmpl->CcmidOfBset(ibset);

	for (dicmid = 1; dicmid < ccmid; dicmid++)
		{
		icmidNext = (icmidCur + dicmid) % ccmid;
		cmidNext = _ptmpl->CmidOfBset(ibset, icmidNext);
		if (fNextAccessory)
			{
			if (!_ptmpl->FSameAccCmids(cmidCur, cmidNext))
				return cmidNext;
			}
		else
			{
			if (_ptmpl->FSameAccCmids(cmidCur, cmidNext))
				return cmidNext;
			}
		}
	return cmidCur;	// couldn't find anything...just use this cmid
}


/***************************************************************************
	Change this APE's TDT properties
***************************************************************************/
bool APE::FChangeTdt(PSTN pstn, long tdts, PTAG ptagTdf)
{
	AssertThis(0);
	Assert(_ptmpl->FIsTdt(), "FChangeTdt is only for TDTs");
	AssertNilOrPo(pstn, 0);
	AssertNilOrVarMem(ptagTdf);

	PTDT ptdtNew = pvNil;
	PBODY pbodyNew = pvNil;

	ptdtNew = ((PTDT)_ptmpl)->PtdtDup();
	if (pvNil == ptdtNew)
		goto LFail;
	pbodyNew = _pbody->PbodyDup();
	if (pvNil == pbodyNew)
		goto LFail;

	if (!ptdtNew->FChange(pstn, tdts, ptagTdf))
		goto LFail;
	if (!ptdtNew->FAdjustBody(pbodyNew))
		goto LFail;
	if (!ptdtNew->FSetActnCel(pbodyNew, _anid, _celn))
		goto LFail;

	ReleasePpo(&_ptmpl);
	_ptmpl = ptdtNew;
	_pbody->Restore(pbodyNew);
	ReleasePpo(&pbodyNew);
	_SetScale();
	_UpdateView();
	return fTrue;
LFail:
	ReleasePpo(&ptdtNew);
	ReleasePpo(&pbodyNew);
	return fFalse;
}



/***************************************************************************
	Set material of this TDT to ptagMtrl
***************************************************************************/
bool APE::FSetTdtMtrl(PTAG ptagMtrl)
{
	AssertThis(0);
	Assert(_ptmpl->FIsTdt(), "FSetTdtMtrl is only for TDTs");
	Assert(_pbody->Cbset() == 1,
		"TDTs should only have one body part set");
	AssertVarMem(ptagMtrl);

	PMTRL pmtrl;
	GMS gms;

	pmtrl = (PMTRL)vptagm->PbacoFetch(ptagMtrl, MTRL::FReadMtrl);
	if (pvNil == pmtrl)
		return fFalse;
	_pbody->SetPartSetMtrl(0, pmtrl);
	ReleasePpo(&pmtrl);

	gms.fValid = fTrue;
	gms.fMtrl = fTrue;
	TrashVar(&gms.cmid);
	gms.tagMtrl = *ptagMtrl;
	_pglgms->Put(0, &gms);

	_UpdateView();
	return fTrue;
}


/***************************************************************************
	Get the CNO of the MTRL attached to this TDT.  Returns fFalse if there
	is no MTRL attached or the MTRL didn't come from a chunk.
***************************************************************************/
bool APE::FGetTdtMtrlCno(CNO *pcno)
{
	AssertThis(0);
	Assert(_ptmpl->FIsTdt(), "FGetTdtMtrlCno is only for TDTs");
	AssertVarMem(pcno);

	PMTRL pmtrl;
	PCMTL pcmtl;
	bool fMtrl;

	_pbody->GetPartSetMaterial(0, &fMtrl, &pmtrl, &pcmtl);
	if (!fMtrl)
		return fFalse;
	if (pvNil == pmtrl->Pcrf())
		return fFalse;
	*pcno = pmtrl->Cno();
	return fTrue;
}


/***************************************************************************
	Return info about the TDT
***************************************************************************/
void APE::GetTdtInfo(PSTN pstn, long *ptdts, PTAG ptagTdf)
{
	AssertThis(0);
	Assert(_ptmpl->FIsTdt(), "GetTdtInfo is only for TDTs");
	AssertNilOrVarMem(pstn);
	AssertNilOrVarMem(ptdts);
	AssertNilOrVarMem(ptagTdf);

	((PTDT)_ptmpl)->GetInfo(pstn, ptdts, ptagTdf);
}


/***************************************************************************
	Fills in pfMtrl, pcmid, and ptagMtrl with the material that was
	attached to ibset, if any.  If nothing was done to ibset, returns
	fFalse.
***************************************************************************/
bool APE::FGetMaterial(long ibset, bool *pfMtrl, long *pcmid, TAG *ptagMtrl)
{
	AssertThis(0);
	AssertIn(ibset, 0, _pbody->Cbset());
	AssertVarMem(pfMtrl);
	AssertVarMem(pcmid);
	AssertVarMem(ptagMtrl);

	GMS gms;

	_pglgms->Get(ibset, &gms);
	if (!gms.fValid)
		return fFalse; // nothing new for this ibset
	*pfMtrl = gms.fMtrl;
	*pcmid = gms.cmid;
	*ptagMtrl = gms.tagMtrl;

	return fTrue;
}



#ifdef DEBUG
/***************************************************************************
	Assert the validity of the APE.
***************************************************************************/
void APE::AssertValid(ulong grf)
{
	APE_PAR::AssertValid(fobjAllocated);
	AssertPo(_pbwld, 0);
	AssertPo(_ptmpl, 0);
	AssertPo(_pbody, 0);
	AssertPo(_pglgms, 0);
	Assert(_pglgms->IvMac() == _pbody->Cbset(), "_pglgms wrong size");
	AssertNilOrPo(_prca, 0);
}


/***************************************************************************
	Mark memory used by the APE
***************************************************************************/
void APE::MarkMem(void)
{
	AssertThis(0);
	APE_PAR::MarkMem();
	MarkMemObj(_pbwld);
	MarkMemObj(_ptmpl);
	MarkMemObj(_pbody);
	MarkMemObj(_pglgms);
	MarkMemObj(_prca);
}
#endif //DEBUG
