/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Kidspace class implementations.

***************************************************************************/
#include "kidframe.h"
ASSERTNAME


RTCLASS(GOK)
RTCLASS(GORP)
RTCLASS(GORF)
RTCLASS(GORB)
RTCLASS(GORT)
RTCLASS(GORV)

BEGIN_CMD_MAP(GOK, GOB)
	ON_CID_ME(cidClicked, &GOK::FCmdClickedCore, pvNil)
	ON_CID_ME(cidAlarm, &GOK::FCmdAlarm, pvNil)
	ON_CID_ME(cidRollOff, &GOK::FCmdMouseMoveCore, pvNil)
	ON_CID_ALL(cidKey, &GOK::FCmdAll, pvNil)
	ON_CID_ALL(cidSelIdle, &GOK::FCmdAll, pvNil)
END_CMD_MAP(&GOK::FCmdAll, pvNil, kgrfcmmAll)


const long kcmhlGok = -10000;


/***************************************************************************
	Create a new kidspace gob as described in the GOKD with given cno
	in the given RCA.
***************************************************************************/
PGOK GOK::PgokNew(PWOKS pwoks, PGOB pgobPar, long hid, PGOKD pgokd, PRCA prca)
{
	AssertPo(pgobPar, 0);
	AssertPo(pgokd, 0);
	AssertPo(prca, 0);
	GCB gcb;
	PGOK pgok;

	if (hidNil == hid)
		{
		Bug("nil handler id");
		return pvNil;
		}

	gcb.Set(hid, pgobPar, fgobNil, kginMark);
	if (pvNil == (pgok = NewObj GOK(&gcb)))
		return pvNil;

	if (!pgok->_FInit(pwoks, pgokd, prca))
		{
		ReleasePpo(&pgok);
		return pvNil;
		}
	if (!pgok->_FEnterState(ksnoInit))
		{
		Warn("GOK immediately destroyed!");
		return pvNil;
		}

	AssertPo(pgok, 0);
	return pgok;
}


/***************************************************************************
	Static method to find the GOB that should be before a new GOK with
	this zp.
***************************************************************************/
PGOB GOK::_PgobBefore(PGOB pgobPar, long zp)
{
	AssertPo(pgobPar, 0);
	PGOB pgobBefore, pgobT;

	//find the place in the GOB tree to put the GOK.
	for (pgobBefore = pvNil, pgobT = pgobPar->PgobFirstChild();
			pgobT != pvNil; pgobT = pgobT->PgobNextSib())
		{
		if (pgobT->FIs(kclsGOK))
			{
			if (((PGOK)pgobT)->_zp <= zp)
				break;
			pgobBefore = pgobT;
			}
		}
	return pgobBefore;
}


/***************************************************************************
	Constructor for the Kidspace graphic object.
***************************************************************************/
GOK::GOK(GCB *pgcb) : GOB(pgcb)
{
	_chidAnim = chidNil;
	_siiSound = siiNil;
	_siiMouse = siiNil;
	_hidToolTipSrc = Hid();
	_gmsCur = gmsNil;
}


/***************************************************************************
	Destructor for a GOK.
***************************************************************************/
GOK::~GOK(void)
{
	if (vpsndm != pvNil)
		{
		if (siiNil != _siiSound)
			vpsndm->Stop(_siiSound);
		if (siiNil != _siiMouse)
			vpsndm->Stop(_siiMouse);
		}

	if (_fStream)
		{
		if (pvNil != _pgokd)
			_pgokd->SetCrep(crepToss);
		}

	ReleasePpo(&_pgokd);
	ReleasePpo(&_pcrf);
	ReleasePpo(&_prca);
	ReleasePpo(&_pscegAnim);
	ReleasePpo(&_pgorp);
	ReleasePpo(&_pglcmflt);
}


/***************************************************************************
	Initialize this GOK given the cno for the gokd.
***************************************************************************/
bool GOK::_FInit(PWOKS pwoks, CNO cno, PRCA prca)
{
	AssertBaseThis(0);
	AssertPo(pwoks, 0);
	AssertPo(prca, 0);
	PGOKD pgokd;
	bool fRet;

	if (pvNil == (pgokd = pwoks->PgokdFetch(kctgGokd, cno, prca)))
		return fFalse;

	fRet = _FInit(pwoks, pgokd, prca);
	ReleasePpo(&pgokd);

	return fRet;
}


/***************************************************************************
	Initialize this GOK.
***************************************************************************/
bool GOK::_FInit(PWOKS pwoks, PGOKD pgokd, PRCA prca)
{
	AssertBaseThis(0);
	AssertPo(pwoks, 0);
	AssertPo(pgokd, 0);
	AssertPo(prca, 0);
	LOP lop;

	_pwoks = pwoks;
	_pgokd = pgokd;
	_pgokd->AddRef();
	_pgokd->GetLop(PgobPar()->Hid(), &lop);
	_dxp = lop.xp;
	_dyp = lop.yp;
	_zp = klwMax;
	_fRect = FPure(pgokd->Gokk() & gokkRectangle);
	_fNoHit = FPure(pgokd->Gokk() & gokkNoHitThis);
	_fNoHitKids = FPure(pgokd->Gokk() & gokkNoHitKids);
	_fNoSlip = FPure(pgokd->Gokk() & gokkNoSlip);

	_prca = prca;
	_prca->AddRef();
	_pcrf = pgokd->Pcrf();
	_pcrf->AddRef();
	AssertThis(0);
	SetZPlane(lop.zp);

	return fTrue;
}


/***************************************************************************
	This is a table of what relative chid value to use for the
	representation in the given mouse tracking state. relative means that
	the actual chid is gotten by putting the current gob state number in
	the high word.
***************************************************************************/
const CHID _mpgmschidRep[] =
	{
	chidNil,

	kchidEnterState,
	kchidUpOff,
	kchidUpOffOn,
	kchidUpOnOff,
	kchidUpOn,
	kchidDownUpOff,

	kchidUpDownOn,
	kchidDownUpOn,
	kchidDownOn,
	kchidDownOnOff,
	kchidDownOffOn,
	kchidDownOff,
	chidNil,
	};


/***************************************************************************
	Return the chid value for the current mouse tracking state.
***************************************************************************/
CHID GOK::_ChidMouse(void)
{
	AssertThisMem();
	CHID chid;

	chid = _mpgmschidRep[_gmsCur];
	if (chidNil != chid)
		chid = ChidFromSnoDchid(_sno, chid);
	return chid;
}


/***************************************************************************
	These are the actions that can occur at transitions between mouse
	tracking states.
***************************************************************************/
enum
	{
	factNil =			0x0,
	factEnqueueClick =	0x1,
	};


/***************************************************************************
	The following are the transition tables between the mouse tracking
	states when given various input.
***************************************************************************/
struct GMSE
	{
	Debug ( long gms; )
	long gmsDst;
	ulong grfact;
	};

#ifdef DEBUG
#define _Gmse(gms, gmsDst, grfact) \
	{ gms, gmsDst, grfact }
#else //!DEBUG
#define _Gmse(gms, gmsDst, grfact) \
	{ gmsDst, grfact }
#endif //!DEBUG


/***************************************************************************
	what to do when an animation ends
***************************************************************************/
GMSE _mpgmsgmseEnd[] =
	{
	_Gmse(gmsNil, kgmsEnterState, factNil),

	_Gmse(kgmsEnterState, kgmsIdle, factNil),
	_Gmse(kgmsIdle, kgmsIdle, factNil),
	_Gmse(kgmsRollOn, kgmsOn, factNil),
	_Gmse(kgmsRollOff, kgmsIdle, factNil),
	_Gmse(kgmsOn, kgmsOn, factNil),
	_Gmse(kgmsReleaseOff, kgmsIdle, factNil),

	_Gmse(kgmsPressOn, kgmsDownOn, factNil),
	_Gmse(kgmsReleaseOn, kgmsWait, factEnqueueClick),
	_Gmse(kgmsDownOn, kgmsDownOn, factNil),
	_Gmse(kgmsDragOff, kgmsDownOff, factNil),
	_Gmse(kgmsDragOn, kgmsDownOn, factNil),
	_Gmse(kgmsDownOff, kgmsDownOff, factNil),
	_Gmse(kgmsWait, kgmsWait, factNil),
	};


/***************************************************************************
	what to do when we get a cidMouseMove
***************************************************************************/
GMSE _mpgmsgmseMove[] =
	{
	_Gmse(gmsNil, gmsNil, factNil),

	_Gmse(kgmsEnterState, kgmsIdle, factNil),
	_Gmse(kgmsIdle, kgmsRollOn, factNil),
	_Gmse(kgmsRollOn, kgmsRollOn, factNil),
	_Gmse(kgmsRollOff, kgmsIdle, factNil),
	_Gmse(kgmsOn, kgmsOn, factNil),
	_Gmse(kgmsReleaseOff, kgmsIdle, factNil),

	// shouldn't get a cidMouseMove when tracking the mouse,
	// so all of these cause asserts
	_Gmse(kgmsPressOn, gmsNil, factNil),
	_Gmse(kgmsReleaseOn, gmsNil, factNil),
	_Gmse(kgmsDownOn, gmsNil, factNil),
	_Gmse(kgmsDragOff, gmsNil, factNil),
	_Gmse(kgmsDragOn, gmsNil, factNil),
	_Gmse(kgmsDownOff, gmsNil, factNil),
	_Gmse(kgmsWait, gmsNil, factNil),
	};


/***************************************************************************
	what to do when we get a cidRollOff
***************************************************************************/
GMSE _mpgmsgmseRollOff[] =
	{
	_Gmse(gmsNil, gmsNil, factNil),

	_Gmse(kgmsEnterState, kgmsEnterState, factNil),
	_Gmse(kgmsIdle, kgmsIdle, factNil),
	_Gmse(kgmsRollOn, kgmsOn, factNil),
	_Gmse(kgmsRollOff, kgmsRollOff, factNil),
	_Gmse(kgmsOn, kgmsRollOff, factNil),
	_Gmse(kgmsReleaseOff, kgmsReleaseOff, factNil),

	// shouldn't get a cidRollOff when tracking the mouse,
	// so all of these cause asserts
	_Gmse(kgmsPressOn, gmsNil, factNil),
	_Gmse(kgmsReleaseOn, gmsNil, factNil),
	_Gmse(kgmsDownOn, gmsNil, factNil),
	_Gmse(kgmsDragOff, gmsNil, factNil),
	_Gmse(kgmsDragOn, gmsNil, factNil),
	_Gmse(kgmsDownOff, gmsNil, factNil),

	// This one can happen if a modal comes up in response to our click
	_Gmse(kgmsWait, kgmsOn, factNil),
	};


/***************************************************************************
	what to do when we get a cidMouseDown
***************************************************************************/
GMSE _mpgmsgmseMouseDown[] =
	{
	_Gmse(gmsNil, gmsNil, factNil),

	_Gmse(kgmsEnterState, kgmsIdle, factNil),
	_Gmse(kgmsIdle, kgmsRollOn, factNil),
	_Gmse(kgmsRollOn, kgmsOn, factNil),
	_Gmse(kgmsRollOff, kgmsIdle, factNil),
	_Gmse(kgmsOn, kgmsPressOn, factNil),
	_Gmse(kgmsReleaseOff, kgmsIdle, factNil),

	// shouldn't get a cidMouseDown when tracking the mouse,
	// so all of these cause asserts
	_Gmse(kgmsPressOn, gmsNil, factNil),
	_Gmse(kgmsReleaseOn, gmsNil, factNil),
	_Gmse(kgmsDownOn, gmsNil, factNil),
	_Gmse(kgmsDragOff, gmsNil, factNil),
	_Gmse(kgmsDragOn, gmsNil, factNil),
	_Gmse(kgmsDownOff, gmsNil, factNil),
	_Gmse(kgmsWait, gmsNil, factNil),
	};


/***************************************************************************
	what to do when we get a cidTrackMouse with button down and mouse on
***************************************************************************/
GMSE _mpgmsgmseDownOn[] =
	{
	_Gmse(gmsNil, gmsNil, factNil),

	// shouldn't get a cidTrackMouse when not tracking the mouse,
	// so all of these cause asserts
	_Gmse(kgmsEnterState, gmsNil, factNil),
	_Gmse(kgmsIdle, gmsNil, factNil),
	_Gmse(kgmsRollOn, gmsNil, factNil),
	_Gmse(kgmsRollOff, gmsNil, factNil),
	_Gmse(kgmsOn, gmsNil, factNil),
	_Gmse(kgmsReleaseOff, gmsNil, factNil),

	_Gmse(kgmsPressOn, kgmsPressOn, factNil),
	_Gmse(kgmsReleaseOn, kgmsReleaseOn, factNil),
	_Gmse(kgmsDownOn, kgmsDownOn, factNil),
	_Gmse(kgmsDragOff, kgmsDownOff, factNil),
	_Gmse(kgmsDragOn, kgmsDragOn, factNil),
	_Gmse(kgmsDownOff, kgmsDragOn, factNil),
	_Gmse(kgmsWait, kgmsOn, factNil),
	};


/***************************************************************************
	what to do when we get a cidTrackMouse with button down and mouse off
***************************************************************************/
GMSE _mpgmsgmseDownOff[] =
	{
	_Gmse(gmsNil, gmsNil, factNil),

	// shouldn't get a cidTrackMouse when not tracking the mouse,
	// so all of these cause asserts
	_Gmse(kgmsEnterState, gmsNil, factNil),
	_Gmse(kgmsIdle, gmsNil, factNil),
	_Gmse(kgmsRollOn, gmsNil, factNil),
	_Gmse(kgmsRollOff, gmsNil, factNil),
	_Gmse(kgmsOn, gmsNil, factNil),
	_Gmse(kgmsReleaseOff, gmsNil, factNil),

	_Gmse(kgmsPressOn, kgmsDownOn, factNil),
	_Gmse(kgmsReleaseOn, kgmsReleaseOn, factNil),
	_Gmse(kgmsDownOn, kgmsDragOff, factNil),
	_Gmse(kgmsDragOff, kgmsDragOff, factNil),
	_Gmse(kgmsDragOn, kgmsDownOn, factNil),
	_Gmse(kgmsDownOff, kgmsDownOff, factNil),
	_Gmse(kgmsWait, kgmsOn, factNil),
	};


/***************************************************************************
	what to do when we get a cidTrackMouse with button up and mouse on
***************************************************************************/
GMSE _mpgmsgmseUpOn[] =
	{
	_Gmse(gmsNil, gmsNil, factNil),

	// shouldn't get a cidTrackMouse when not tracking the mouse,
	// so all of these cause asserts
	_Gmse(kgmsEnterState, gmsNil, factNil),
	_Gmse(kgmsIdle, gmsNil, factNil),
	_Gmse(kgmsRollOn, gmsNil, factNil),
	_Gmse(kgmsRollOff, gmsNil, factNil),
	_Gmse(kgmsOn, gmsNil, factNil),
	_Gmse(kgmsReleaseOff, gmsNil, factNil),

	_Gmse(kgmsPressOn, kgmsDownOn, factNil),
	_Gmse(kgmsReleaseOn, kgmsReleaseOn, factNil),
	_Gmse(kgmsDownOn, kgmsReleaseOn, factNil),
	_Gmse(kgmsDragOff, kgmsDownOff, factNil),
	_Gmse(kgmsDragOn, kgmsDownOn, factNil),
	_Gmse(kgmsDownOff, kgmsReleaseOff, factNil),
	_Gmse(kgmsWait, kgmsOn, factNil),
	};


/***************************************************************************
	what to do when we get a cidTrackMouse with button up and mouse off
***************************************************************************/
GMSE _mpgmsgmseUpOff[] =
	{
	_Gmse(gmsNil, gmsNil, factNil),

	// shouldn't get a cidTrackMouse when not tracking the mouse,
	// so all of these cause asserts
	_Gmse(kgmsEnterState, gmsNil, factNil),
	_Gmse(kgmsIdle, gmsNil, factNil),
	_Gmse(kgmsRollOn, gmsNil, factNil),
	_Gmse(kgmsRollOff, gmsNil, factNil),
	_Gmse(kgmsOn, gmsNil, factNil),
	_Gmse(kgmsReleaseOff, gmsNil, factNil),

	_Gmse(kgmsPressOn, kgmsDownOn, factNil),
	_Gmse(kgmsReleaseOn, kgmsReleaseOn, factNil),
	_Gmse(kgmsDownOn, kgmsDragOff, factNil),
	_Gmse(kgmsDragOff, kgmsDownOff, factNil),
	_Gmse(kgmsDragOn, kgmsDownOn, factNil),
	_Gmse(kgmsDownOff, kgmsReleaseOff, factNil),
	_Gmse(kgmsWait, kgmsOn, factNil),
	};


/***************************************************************************
	We got some input, make sure the mouse tracking state is correct,
	according to the given transition table.

	CAUTION: this GOK may not exist on return. Returns false iff the GOK
	doesn't exist on return.
***************************************************************************/
bool GOK::_FAdjustGms(GMSE *pmpgmsgmse)
{
	AssertThis(0);
	GMSE gmse;
	bool fStable = fTrue;
	Debug( bool fFirst = fTrue; )

	_DeferGorp(fTrue);
	for ( ; ; Debug(fFirst = fFalse))
		{
		gmse = pmpgmsgmse[_gmsCur];
		Assert(gmse.gms == _gmsCur, "bad gmse table");

		if (gmsNil == gmse.gmsDst || _gmsCur == gmse.gmsDst)
			{
			Assert(!fFirst || gmsNil != gmse.gmsDst,
				"invalid change in gms");

			if (fStable)
				break;

			// use the done table from here on out
			Assert(pmpgmsgmse != _mpgmsgmseEnd, "bug in _mpgmsgmseEnd");
			pmpgmsgmse = _mpgmsgmseEnd;
			continue;
			}

		if (!_FSetGmsCore(gmse.gmsDst, gmse.grfact, &fStable))
			return fFalse;

		if (fStable && _mpgmsgmseEnd == pmpgmsgmse)
			break;

		AssertThis(0);
		}
	_DeferGorp(fFalse);

	return fTrue;
}


/***************************************************************************
	Set the graphical mouse state. Note that the _gmsCur may end up different
	than gms. This is similar to _FAdjustGms(_mpgmsgmseEnd), except that
	the current representation is set before trying to advance.

	CAUTION: this GOK may not exist on return. Returns false iff the GOK
	doesn't exist on return.
***************************************************************************/
bool GOK::_FSetGms(long gms, ulong grfact)
{
	AssertThis(0);
	AssertIn(gms, gmsNil, kgmsLim);
	GMSE gmse;
	bool fStable;

	_DeferGorp(fTrue);
	for (;;)
		{
		if (!_FSetGmsCore(gms, grfact, &fStable))
			return fFalse;

		if (fStable)
			break;

		Assert(_gmsCur == gms, "bad _gmsCur");

		// Nothing was set so advance
		gmse = _mpgmsgmseEnd[gms];
		Assert(gmse.gms == gms, "bad gmse table");
		Assert(gmse.gmsDst != gms, "bad gmse table");

		// continue to the next state
		gms = gmse.gmsDst;
		grfact = gmse.grfact;
		}
	_DeferGorp(fFalse);
	return fTrue;
}


/***************************************************************************
	Set the graphical mouse state. Note that _gmsCur may end up different
	than gms. *pfStable is set to false iff the gms is an auto-advance gms
	and the representation wasn't set (didn't exist or failed).

	CAUTION: this GOK may not exist on return. Returns false iff the GOK
	doesn't exist on return.
***************************************************************************/
bool GOK::_FSetGmsCore(long gms, ulong grfact, bool *pfStable)
{
	AssertThis(0);
	AssertIn(gms, gmsNil, kgmsLim);
	AssertVarMem(pfStable);
	CHID chid;
	CTG ctg;

	// set the gms
	_gmsCur = gms;

	// do the actions
	if (FIn(gms, kgmsMinTrack, kgmsLimTrack))
		{
		if (pvNil == vpcex->PgobTracking())
			vpcex->TrackMouse(this);
		}
	else
		{
		if (this == vpcex->PgobTracking())
			vpcex->EndMouseTracking();
		}
	if (grfact & factEnqueueClick)
		{
		CMD_MOUSE cmd;

		ClearPb(&cmd, size(cmd));
		cmd.cid = cidClicked;
		cmd.pcmh = this;
		cmd.grfcust = _grfcust;
		cmd.cact = _cactMouse;
		vpcex->EnqueueCmd((PCMD)&cmd);
		}

	// change the representation
	ctg = (_mpgmsgmseEnd[gms].gmsDst == gms) ? ctgNil : kctgAnimation;
	chid = _ChidMouse();
	if (chidNil != chid)
		{
		if (!_FSetRep(chid, fgokMouseSound | fgokKillAnim, ctg,
				0, 0, pfStable))
			{
			return fFalse;
			}
		// we're stable if we didn't require an animation or the rep was set
		if (ctgNil == ctg)
			*pfStable = fTrue;
		}
	else
		*pfStable = (ctgNil == ctg);

	return fTrue;
}


/***************************************************************************
	Make the current graphical representation that indicated by the given
	chid value (if not nil).

	If ctg is not nil, only uses ctg representations. Moves the GOK by
	(dxp, dyp). Fills *pfSet with whether the representation was
	successfully set.

	CAUTION: this GOK may not exist on return. Returns false iff the GOK
	doesn't exist on return.
***************************************************************************/
bool GOK::_FSetRep(CHID chid, ulong grfgok, CTG ctg,
	long dxp, long dyp, bool *pfSet)
{
	AssertThis(0);
	long ikid;
	KID kid;
	PGORP pgorp;
	bool fSet = fFalse;
	bool fKillAnim = FPure(grfgok & fgokKillAnim);
	PCFL pcfl = _pcrf->Pcfl();

	if (chidNil == chid)
		goto LAdjust;

	if (!(grfgok & fgokNoAnim) && (ctgNil == ctg || kctgAnimation == ctg))
		{
		// animations are allowed - try to start one
		if (_chidAnim == chid && !(grfgok & fgokReset))
			{
			fSet = fTrue;
			fKillAnim = fFalse;
			goto LAdjust;
			}

		if (pcfl->FGetKidChidCtg(_pgokd->Ctg(), _pgokd->Cno(),
				chid, kctgAnimation, &kid))
			{
			PSCPT pscpt;
			PSCEG psceg = pvNil;

			if (pvNil != (pscpt = (PSCPT)_pcrf->PbacoFetch(kid.cki.ctg,
					kid.cki.cno, SCPT::FReadScript)) &&
				pvNil != (psceg = _pwoks->PscegNew(_prca, this)) &&
				psceg->FAttachScript(pscpt))
				{
				ReleasePpo(&pscpt);
				if (pvNil != _pscegAnim)
					{
					ReleasePpo(&_pscegAnim);
					_pwoks->PclokAnim()->RemoveCmh(this);
					_pwoks->PclokNoSlip()->RemoveCmh(this);
					}
				_pscegAnim = psceg;
				fSet = fTrue;
				fKillAnim = fFalse;
				_chidAnim = chid;
				if (!_FAdvanceFrame())
					return fFalse;
				goto LAdjust;
				}

			Warn("Couldn't attach animation script");
			ReleasePpo(&psceg);
			ReleasePpo(&pscpt);
			}
		}

	if (kctgAnimation == ctg)
		goto LAdjust;

	for (pcfl->FGetIkid(_pgokd->Ctg(), _pgokd->Cno(), 0, 0, chid, &ikid);
			pcfl->FGetKid(_pgokd->Ctg(), _pgokd->Cno(), ikid, &kid) &&
				kid.chid == chid;
			ikid++)
		{
		if (ctgNil != ctg && ctg != kid.cki.ctg ||
				kctgAnimation == kid.cki.ctg)
			{
			continue;
			}

		if (!(grfgok & fgokReset) && pvNil != _pgorp &&
				FEqualRgb(&kid.cki, &_ckiGorp, size(CKI)))
			{
			fSet = fTrue;
			break;
			}

		if (pvNil != (pgorp = _PgorpNew(_pcrf, kid.cki.ctg, kid.cki.cno)))
			{
			_SetGorp(pgorp, dxp, dyp);
			ReleasePpo(&pgorp);
			_ckiGorp = kid.cki;
			dxp = dyp = 0;
			fSet = fTrue;
			break;
			}
		}

LAdjust:
	// reposition the GOK
	if (dxp != 0 || dyp != 0)
		{
		RC rc;

		GetRc(&rc, cooParent);
		rc.Offset(dxp, dyp);
		SetPos(&rc);
		}

	if (fKillAnim && fSet)
		{
		ReleasePpo(&_pscegAnim);
		_pwoks->PclokAnim()->RemoveCmh(this);
		_pwoks->PclokNoSlip()->RemoveCmh(this);
		_chidAnim = chidNil;
		}

	if (grfgok & fgokMouseSound)
		_PlayMouseSound(chid);

	if (pvNil != pfSet)
		*pfSet = fSet;

	return fTrue;
}


/***************************************************************************
	Run the next section of the animation script.
***************************************************************************/
bool GOK::_FAdvanceFrame(void)
{
	AssertThis(0);

	bool fExists, fRet, fPaused;
	PWOKS pwoks = _pwoks;
	ulong dtim = 0;
	PSCEG psceg = _pscegAnim;
	long grid = Grid();

	if (pvNil == psceg)
		{
		Bug("no animation");
		return fTrue;
		}
	AssertPo(psceg, 0);
	psceg->AddRef();

	_DeferGorp(fTrue);
	for (;;)
		{
		fRet = psceg->FResume(pvNil, &fPaused);
		fExists = (this == pwoks->PgobFromGrid(grid));

		if (!fExists)
			{
			// this GOK went away
			ReleasePpo(&psceg);
			return fFalse;
			}

		AssertThis(0);
		if (!fRet || !fPaused || pvNil == _pscegAnim)
			{
			// end the animation, but keep _chidAnim set, so we don't restart
			// this animation until after the animation has been explicitly
			// killed.
			if (psceg == _pscegAnim)
				{
				ReleasePpo(&_pscegAnim);
				_pwoks->PclokAnim()->RemoveCmh(this);
				_pwoks->PclokNoSlip()->RemoveCmh(this);
				ReleasePpo(&psceg);
				if (!_FAdjustGms(_mpgmsgmseEnd))
					return fFalse;
				}
			else
				ReleasePpo(&psceg);

			goto LSetGorp;
			}

		if (!_fNoSlip)
			{
			_pwoks->PclokAnim()->FSetAlarm(_dtim, this);
			break;
			}

		dtim += LwMax(1, _dtim);
		if (dtim > _pwoks->PclokNoSlip()->DtimAlarm())
			{
			_pwoks->PclokNoSlip()->FSetAlarm(dtim, this);
			break;
			}
		}
	ReleasePpo(&psceg);

LSetGorp:
	_DeferGorp(fFalse);

	return fTrue;
}


/***************************************************************************
	Put the kidspace graphic object in the indicated state.

	CAUTION: this GOK may not exist on return. Returns false iff the GOK
	doesn't exist on return.
***************************************************************************/
bool GOK::_FEnterState(long sno)
{
	AssertThis(0);
	AssertIn(sno, 0, kswMax);

	struct GMMPE
		{
		Debug( long gms; )
		long gmsPar;
		};

	#ifdef DEBUG
	#define _Gmmpe(gms, gmsPar) \
		{ gms, gmsPar }
	#else //!DEBUG
	#define _Gmmpe(gms, gmsPar) \
		{ gmsPar }
	#endif //!DEBUG

	// this maps a gms to its parent in the natural traversal tree
	static GMMPE _mpgmsgmmpe[] =
		{
		_Gmmpe(gmsNil, gmsNil),

		_Gmmpe(kgmsEnterState, gmsNil),
		_Gmmpe(kgmsIdle, kgmsEnterState),
		_Gmmpe(kgmsRollOn, kgmsIdle),
		_Gmmpe(kgmsRollOff, kgmsOn),
		_Gmmpe(kgmsOn, kgmsRollOn),
		// Note: this is the only strange one - we can't go through kgmsDownOff
		// because that would require tracking the mouse. If some other button
		// is currently tracking, we couldn't start tracking and things might
		// break. Thus, we just go through kgmsIdle.
		_Gmmpe(kgmsReleaseOff, kgmsIdle),

		_Gmmpe(kgmsPressOn, kgmsOn),
		_Gmmpe(kgmsReleaseOn, kgmsDownOn),
		_Gmmpe(kgmsDownOn, kgmsPressOn),
		_Gmmpe(kgmsDragOff, kgmsDownOn),
		_Gmmpe(kgmsDragOn, kgmsDownOff),
		_Gmmpe(kgmsDownOff, kgmsDragOff),
		_Gmmpe(kgmsWait, kgmsReleaseOn),
		};

	long rggms[kgmsLim];
	long igms;
	long gms, gmsPrev;
	bool fStable;

	for (gms = _gmsCur, igms = 0; gmsNil != gms; )
		{
		rggms[igms++] = gms;
		Assert(gms == _mpgmsgmmpe[gms].gms, "bad _mpgmsgmmpe");
		gms = _mpgmsgmmpe[gms].gmsPar;
		}

	// defer rep changes
	_DeferGorp(fTrue);

	// change the state number
	_sno = (short)sno;

	// put it in gmsNil
	if (!_FSetGmsCore(gmsNil, factNil, &fStable))
		return fFalse;

	for (gmsPrev = gmsNil; igms-- > 0; gmsPrev = gms)
		{
		gms = rggms[igms];
		if (gmsPrev != _gmsCur)
			continue;

		if (!_FSetGmsCore(gms, factNil, &fStable))
			return fFalse;

		AssertThis(0);
		}

	if (!fStable && !_FAdjustGms(_mpgmsgmseEnd))
		return fFalse;

	_DeferGorp(fFalse);

	return fTrue;
}


/***************************************************************************
	See if the given point is in this GOK.
***************************************************************************/
bool GOK::FPtIn(long xp, long yp)
{
	AssertThis(0);
	RC rc;

	if (pvNil == _pgorp || _fNoHit || khidToolTip == Hid())
		return fFalse;

	GetRc(&rc, cooLocal);
	if (!rc.FPtIn(xp, yp))
		return fFalse;

	if (_fRect)
		return fTrue;

	return _pgorp->FPtIn(xp, yp);
}


/***************************************************************************
	See if the given point is in the rectangular bounds of this GOK (ie,
	whether there's any chance the point is in a child of this GOK).
***************************************************************************/
bool GOK::FPtInBounds(long xp, long yp)
{
	AssertThis(0);
	RC rc;

	if (_fNoHitKids || khidToolTip == Hid())
		return fFalse;

	GetRc(&rc, cooLocal);
	return rc.FPtIn(xp, yp);
}


/***************************************************************************
	Draw the kidspace object.
***************************************************************************/
void GOK::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	RC rc;

	if (pvNil != _pgorp)
		{
		GetRc(&rc, cooLocal);
		if (rc.FIntersect(prcClip))
			_pgorp->Draw(pgnv, prcClip);
		}
}


/***************************************************************************
	Respond to an alarm. If the clock is the animation clock or noslip clock
	we advance the animation. Otherwise we run an associated script.
	CAUTION: this GOK may not exist on return.
***************************************************************************/
bool GOK::FCmdAlarm(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CHID chid;
	long hid = pcmd->rglw[0];

	if (_pwoks->PclokAnim()->Hid() == hid ||
			_pwoks->PclokNoSlip()->Hid() == hid)
		{
		if (pvNil != _pscegAnim)
			_FAdvanceFrame();
		else
			Bug("Alarm sounding without an animation");
		}
	else
		{
		chid = pcmd->rglw[2];
		if (chidNil == chid || 0 == chid)
			chid = ChidFromSnoDchid(_sno, kchidAlarm);
		FRunScript(chid, &pcmd->rglw[0], 3);
		}

	return fTrue;
}


/***************************************************************************
	For command filtering.
	CAUTION: this GOK may not exist on return.
***************************************************************************/
bool GOK::FCmdAll(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	CMFLT cmflt;
	bool fFiltered;

	if (pvNil == _pglcmflt)
		return fFalse;

	if (pvNil != pcmd->pcmh)
		{
		if (_FFindCmflt(pcmd->cid, pcmd->pcmh->Hid(), &cmflt) &&
				_FFilterCmd(pcmd, cmflt.chidScript, &fFiltered))
			{
			return fFiltered;
			}

		if (_FFindCmflt(cidNil, pcmd->pcmh->Hid(), &cmflt) &&
				_FFilterCmd(pcmd, cmflt.chidScript, &fFiltered))
			{
			return fFiltered;
			}
		}

	if (_FFindCmflt(pcmd->cid, hidNil, &cmflt) &&
			_FFilterCmd(pcmd, cmflt.chidScript, &fFiltered))
		{
		return fFiltered;
		}

	return fFalse;
}


/***************************************************************************
	Determine if the command should be filtered out by calling the indicated
	script and checking its return value. This returns true iff the command
	is filtered out or the GOK goes away, in which case *pfFilter is set
	to whether the command was filtered.
***************************************************************************/
bool GOK::_FFilterCmd(PCMD pcmd, CHID chidScript, bool *pfFilter)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	AssertVarMem(pfFilter);
	long rglw[6];
	long lw;
	bool fGokExists;
	bool tRet;

	rglw[0] = (pvNil == pcmd->pcmh) ? hidNil : pcmd->pcmh->Hid();
	rglw[1] = pcmd->cid;
	rglw[2] = pcmd->rglw[0];
	rglw[3] = pcmd->rglw[1];
	rglw[4] = pcmd->rglw[2];
	rglw[5] = pcmd->rglw[3];
	fGokExists = FRunScript(chidScript, rglw, 6, &lw, &tRet);
	*pfFilter = FPure(lw) && tRet == tYes;

	return !fGokExists || *pfFilter;
}


/***************************************************************************
	Filter on a command id and/or a handler id. Either cid or hid may be
	nil. To turn off filtering for a (cid, hid), use chidNil for chidScript.
	If both cid and hid are nil, chidScript should be nil. If the cid, hid
	and chidScript are all nil, we turn off all filtering.
***************************************************************************/
bool GOK::FFilterCidHid(long cid, long hid, CHID chidScript)
{
	AssertThis(0);
	CMFLT cmflt;
	long icmflt;
	KID kid;

	if (chidNil == chidScript ||
		!_pcrf->Pcfl()->FGetKidChidCtg(_pgokd->Ctg(), _pgokd->Cno(),
			chidScript, kctgScript, &kid))
		{
		// turn off filtering
		if (chidNil != chidScript)
			Warn("script doesn't exist (for filtering)");

		if (cidNil != cid || hidNil != hid)
			{
			if (!_FFindCmflt(cid, hid, pvNil, &icmflt))
				return fTrue;
			_pglcmflt->Delete(icmflt);
			if (0 < _pglcmflt->IvMac())
				return fTrue;
			}

		// release the filters and remove ourself from the command handler list
		ReleasePpo(&_pglcmflt);
		vpcex->RemoveCmh(this, kcmhlGok);
		return fTrue;
		}

	if (cidNil == cid && hidNil == hid)
		{
		Bug("can't filter on (cidNil, hidNil)!");
		return fFalse;
		}

	if (_FFindCmflt(cid, hid, &cmflt, &icmflt))
		{
		cmflt.chidScript = chidScript;
		_pglcmflt->Put(icmflt, &cmflt);
		return fTrue;
		}

	if (pvNil == _pglcmflt && pvNil == (_pglcmflt = GL::PglNew(size(CMFLT))))
		return fFalse;

	cmflt.cid = cid;
	cmflt.hid = hid;
	cmflt.chidScript = chidScript;
	if (!_pglcmflt->FInsert(icmflt, &cmflt))
		return fFalse;

	vpcex->RemoveCmh(this, kcmhlGok);
	return vpcex->FAddCmh(this, kcmhlGok, kgrfcmmAll);
}


/***************************************************************************
	Look for the given (cid, hid) pair in the filtering map.
***************************************************************************/
bool GOK::_FFindCmflt(long cid, long hid, CMFLT *pcmflt, long *picmflt)
{
	AssertThis(0);
	Assert(cid != cidNil || hid != hidNil, "cid and hid are both nil!");
	AssertNilOrVarMem(pcmflt);
	AssertNilOrVarMem(picmflt);
	long ivMin, ivLim, iv;
	CMFLT cmflt;

	ivMin = 0;
	if (pvNil != _pglcmflt)
		{
		for (ivLim = _pglcmflt->IvMac(); ivMin < ivLim; )
			{
			iv = (ivMin + ivLim) / 2;
			_pglcmflt->Get(iv, &cmflt);
			if (cmflt.cid < cid)
				ivMin = iv + 1;
			else if (cmflt.cid > cid)
				ivLim = iv;
			else if (cmflt.hid < hid)
				ivMin = iv + 1;
			else if (cmflt.hid > hid)
				ivLim = iv;
			else
				{
				if (pvNil != pcmflt)
					*pcmflt = cmflt;
				if (pvNil != picmflt)
					*picmflt = iv;
				return fTrue;
				}
			}
		}

	TrashVar(pcmflt);
	if (pvNil != picmflt)
		*picmflt = ivMin;
	return fFalse;
}


/***************************************************************************
	This handles cidMouseMove and cidRollOff.
***************************************************************************/
bool GOK::FCmdMouseMove(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	pcmd->grfcust = _pwoks->GrfcustAdjust(pcmd->grfcust);
	if (pcmd->cid == cidRollOff)
		_FAdjustGms(_mpgmsgmseRollOff);
	else
		{
		Assert(cidMouseMove == pcmd->cid, "unknown command");
		CUME cume;
		bool fCanClick = _pgokd->FGetCume(pcmd->grfcust, _sno, &cume);

		if (!_FAdjustGms(fCanClick ? _mpgmsgmseMove : _mpgmsgmseRollOff))
			return fTrue;
		SetCursor(pcmd->grfcust);
		}

	return fTrue;
}


/***************************************************************************
	Set the cursor for this GOK and the given cursor state.
***************************************************************************/
void GOK::SetCursor(ulong grfcust)
{
	AssertThis(0);
	PGOK pgok;
	PGOB pgob;
	CUME cume;

	for (pgok = this;
		!pgok->_pgokd->FGetCume(grfcust, _sno, &cume) || cume.cnoCurs == cnoNil;
		grfcust |= fcustChildGok)
		{
		for (pgob = pgok; ; )
			{
			pgob = pgob->PgobPar();
			if (pvNil == pgob)
				{
				vpappb->SetCurs(pvNil);
				return;
				}
			if (pgob->FIs(kclsGOK))
				{
				pgok = (PGOK)pgob;
				break;
				}
			}
		}
	vpappb->SetCursCno(pgok->_prca, cume.cnoCurs);
}


/***************************************************************************
	Do mouse tracking.
***************************************************************************/
bool GOK::FCmdTrackMouse(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	pcmd->grfcust = _pwoks->GrfcustAdjust(pcmd->grfcust);
	if (pcmd->cid == cidMouseDown)
		{
		// first response to mouse down
		CUME cume;

		if (!_pgokd->FGetCume(pcmd->grfcust, _sno, &cume))
			return fTrue;

		_grfcust = pcmd->grfcust;
		_cactMouse = (short)pcmd->cact;
		if (!_FAdjustGms(_mpgmsgmseMouseDown))
			return fTrue;
		SetCursor(pcmd->grfcust);
		}
	else
		{
		Assert(pcmd->cid == cidTrackMouse, 0);
		bool fIn;
		GMSE *pmpgmsgmse;

		fIn = FPtIn(pcmd->xp, pcmd->yp);
		if (pcmd->grfcust & fcustMouse)
			{
			// mouse is down
			pmpgmsgmse = fIn ? _mpgmsgmseDownOn : _mpgmsgmseDownOff;
			}
		else
			{
			// mouse is up
			pmpgmsgmse = fIn ? _mpgmsgmseUpOn : _mpgmsgmseUpOff;
			}
		if (!_FAdjustGms(pmpgmsgmse))
			return fTrue;
		}

	return fTrue;
}


/***************************************************************************
	Put up a tool tip if this GOK has one. Return true if tool tips are
	still active.
***************************************************************************/
bool GOK::FEnsureToolTip(PGOB *ppgobCurTip, long xpMouse, long ypMouse)
{
	AssertThis(0);
	AssertVarMem(ppgobCurTip);
	AssertNilOrPo(*ppgobCurTip, 0);
	CNO cno;
	HTOP htop;
	PGOK pgokSrc;

	ReleasePpo(ppgobCurTip);
	if (_hidToolTipSrc == Hid())
		pgokSrc = this;
	else
		{
		if (pvNil == (pgokSrc = (PGOK)_pwoks->PgobFromHid(_hidToolTipSrc)) ||
				!pgokSrc->FIs(kclsGOK))
			{
			// abort tool tip mode
			return fFalse;
			}
		}

	if (cnoNil == (cno = pgokSrc->_CnoToolTip()))
		{
		// abort tool tip mode
		return fFalse;
		}

	if (kcnoToolTipNoAffect == cno)
		{
		// return true to stay in tool tip mode, but don't put up a tool tip
		return fTrue;
		}

	// put up the tool tip
	htop.cnoBalloon = cnoNil;
	htop.hidThis = khidToolTip;
	htop.hidTarget = Hid();
	htop.cnoScript = cnoNil;
	htop.dxp = 0;
	htop.dyp = 0;
	htop.ckiSnd.ctg = ctgNil;
	htop.ckiSnd.cno = cnoNil;

	*ppgobCurTip = _pwoks->PhbalNew(pvNil, _prca, cno, &htop);

	return fTrue;
}


/***************************************************************************
	Get the cno for the our tool tip (given the current modifier state).
***************************************************************************/
CNO GOK::_CnoToolTip(void)
{
	AssertThis(0);
	CUME cume;

	if (!_pgokd->FGetCume(_pwoks->GrfcustCur(), _sno, &cume))
		return cnoNil;

	return cume.cnoTopic;
}


/***************************************************************************
	Makes the GOK get its tool tip from the GOK having the given hid.
***************************************************************************/
void GOK::SetHidToolTip(long hidSrc)
{
	AssertThis(0);

	_hidToolTipSrc = hidSrc;
}


/***************************************************************************
	Get the state information for the GOK (for testing). High 16 bits are
	the state number; next 14 bits are _gmsCur; low 2 bits are set.
***************************************************************************/
long GOK::LwState(void)
{
	AssertThis(0);

	return (_sno << 16) | (_gmsCur << 2) & 0x0000FFFF | 0x03;
}


/***************************************************************************
	Get the registration point of the given GOK in its parent's coordinates.
***************************************************************************/
void GOK::GetPtReg(PT *ppt, long coo)
{
	AssertThis(0);
	AssertVarMem(ppt);
	RC rc;

	if (coo == cooLocal)
		rc.Zero();
	else
		GetRc(&rc, coo);
	ppt->xp = rc.xpLeft + _dxp;
	ppt->yp = rc.ypTop + _dyp;
}


/***************************************************************************
	Get the "content" rectangle of this GOK. This is primarily for GOKs
	that have a content area, such as help balloons and easels.
***************************************************************************/
void GOK::GetRcContent(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	if (pvNil != _pgorp)
		_pgorp->GetRcContent(prc);
	else
		GetRc(prc, cooLocal);
}


/***************************************************************************
	Set the GOK's z plane.
***************************************************************************/
void GOK::SetZPlane(long zp)
{
	AssertThis(0);
	PGOB pgobBefore;

	if (_zp == zp)
		return;

	pgobBefore = _PgobBefore(PgobPar(), zp);
	if (pgobBefore == this || pgobBefore == PgobPrevSib())
		{
		_zp = zp;
		return;
		}
	SendBehind(pgobBefore);
	InvalRc(pvNil);
	_zp = zp;
}


/***************************************************************************
	Set the slipping behavior of the GOK.
***************************************************************************/
void GOK::SetNoSlip(bool fNoSlip)
{
	AssertThis(0);

	_fNoSlip = FPure(fNoSlip);
}


/***************************************************************************
	If there is an attached script with the given chid value, run it.
	Returns false iff the GOK no longer exists on return. *ptSuccess is
	filled with tNo if the script doesn't exist, tMaybe if the script exists
	but there was a failure, tYes if the script exists and running it
	succeeded.
	CAUTION: this GOK may not exist on return.
***************************************************************************/
bool GOK::FRunScript(CHID chid, long *prglw, long clw, long *plwReturn,
	bool *ptSuccess)
{
	AssertThis(0);
	AssertNilOrVarMem(plwReturn);
	KID kid;

	if (!_pcrf->Pcfl()->FGetKidChidCtg(_pgokd->Ctg(), _pgokd->Cno(),
			chid, kctgScript, &kid))
		{
		if (pvNil != ptSuccess)
			*ptSuccess = tNo;
		return fTrue;
		}

	return FRunScriptCno(kid.cki.cno, prglw, clw, plwReturn, ptSuccess);
}


/***************************************************************************
	If there is a script with the given cno, run it. Returns false iff
	the GOK no longer exists on return. *ptSuccess is filled with tNo
	if the script doesn't exist, tMaybe if the script exists but there
	was a failure, tYes if the script exists and running it succeeded.
	CAUTION: this GOK may not exist on return.
***************************************************************************/
bool GOK::FRunScriptCno(CNO cno, long *prglw, long clw, long *plwReturn,
	bool *ptSuccess)
{
	AssertThis(0);
	AssertNilOrVarMem(plwReturn);
	PSCPT pscpt;
	bool fError, tRet, fExists;

	fExists = fTrue;
	pscpt = (PSCPT)_pcrf->PbacoFetch(kctgScript, cno,
		SCPT::FReadScript, &fError);
	if (pvNil != pscpt)
		{
		AssertPo(pscpt, 0);
		long grid = Grid();
		PWOKS pwoks = _pwoks;
		PSCEG psceg = _pwoks->PscegNew(_prca, this);

		//be careful not to use GOK variables here in case the GOK is
		//freed while the script is running.
		if (pvNil != psceg && psceg->FRunScript(pscpt, prglw, clw, plwReturn))
			tRet = tYes;
		else
			tRet = tMaybe;
		ReleasePpo(&psceg);
		ReleasePpo(&pscpt);

		//see if this GOK still exists
		fExists = (this == pwoks->PgobFromGrid(grid));
		}
	else
		tRet = fError ? tMaybe : tNo;

	if (pvNil != ptSuccess)
		*ptSuccess = tRet;
	return fExists;
}


/***************************************************************************
	Change the current state of the GOK to the given state number.
	CAUTION: this GOK may not exist on return.
***************************************************************************/
bool GOK::FChangeState(long sno)
{
	AssertThis(0);
	AssertIn(sno, 0, kswMax);
	return _FEnterState(sno);
}


/***************************************************************************
	Set the representation to the given chid (if it's not nil).

	Moves the GOK by dxp, dyp and sets its wait time for animation script
	wake up to dtim (unless it's zero).
	CAUTION: this GOK may not exist on return. Returns false iff the GOK
	doesn't exist on return.
***************************************************************************/
bool GOK::FSetRep(CHID chid, ulong grfgok, CTG ctg,
	long dxp, long dyp, ulong dtim)
{
	AssertThis(0);

	if (dtim != 0)
		_dtim = dtim;
	if (grfgok & fgokKillAnim)
		{
		if (!_FAdjustGms(_mpgmsgmseEnd))
			return fTrue;
		}
	return _FSetRep(chid, grfgok, ctg, dxp, dyp);
}


/***************************************************************************
	The GOK has been hit with the mouse, do the associated action.
	CAUTION: this GOK may not exist on return.
***************************************************************************/
bool GOK::FCmdClicked(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	PGOK pgok;
	PGOB pgob;
	long rglw[3];
	long lw;
	bool tRet;
	CUME cume;
	long hid = Hid();
	long grid = Grid();

	_DeferSnd(fTrue);
	if (kgmsWait == _gmsCur)
		{
		if (!_FSetGms(kgmsOn, factNil))
			return fTrue;
		}

	for (pgok = this; ; )
		{
		if (pgok->_pgokd->FGetCume(pcmd->grfcust, _sno, &cume))
			break;
		pgob = pgok->PgobPar();
		if (pvNil == pgob || !pgob->FIs(kclsGOK))
			{
			_DeferSnd(fFalse);
			return fTrue;
			}
		pgok = (PGOK)pgob;
		}

	// do the action associated with a mouse click
	rglw[0] = pcmd->grfcust;
	rglw[1] = hid;
	rglw[2] = pcmd->cact;

	if (!pgok->FRunScript(cume.chidScript, rglw, 3, &lw, &tRet))
		return fTrue;
	if (cidNil != cume.cidDefault && (tYes != tRet || lw != 0))
		{
		// do the default action
		vpcex->EnqueueCid(cume.cidDefault, pvNil, pvNil, pgok->Hid(), hid);
		}

	// undefer sound - be careful because this GOK may have gone away
	if (this == pgok || (this == pgok->_pwoks->PgobFromGrid(grid)))
		_DeferSnd(fFalse);

	return fTrue;
}


/***************************************************************************
	Make the current graphical representation that indicated by the given
	chid value. If we're already in that representation, don't do anything,
	ie, don't restart an animation.
	CAUTION: this GOK may not exist on return. Returns false iff the GOK
	doesn't exist on return.
***************************************************************************/
PGORP GOK::_PgorpNew(PCRF pcrf, CTG ctg, CNO cno)
{
	AssertThis(0);
	typedef PGORP (*PFNGORP)(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno);
	PFNGORP pfngorp;

	switch (ctg)
		{
	default:
		return pvNil;

	case kctgMbmp:
	case kctgMask:
		pfngorp = (PFNGORP)GORB::PgorbNew;
		break;

	case kctgFill:
		pfngorp = (PFNGORP)GORF::PgorfNew;
		break;

	case kctgTile:
		pfngorp = (PFNGORP)GORT::PgortNew;
		break;

	case kctgVideo:
		pfngorp = (PFNGORP)GORV::PgorvNew;
		break;
		}

	return (*pfngorp)(this, pcrf, ctg, cno);
}


/***************************************************************************
	Set the current graphical representation to this one.
***************************************************************************/
void GOK::_SetGorp(PGORP pgorp, long dxp, long dyp)
{
	AssertThis(0);
	AssertNilOrPo(pgorp, 0);
	RC rc, rcNew;

	if (_cactDeferGorp > 0)
		{
		if (pvNil != pgorp)
			{
			pgorp->AddRef();
			pgorp->Stream(_fStream);
			}
		ReleasePpo(&_pgorp);
		_pgorp = pgorp;

		GetRc(&rc, cooParent);
		rc.xpRight = rc.xpLeft;
		rc.ypBottom = rc.ypTop;
		rc.Offset(_dxp + dxp, _dyp + dyp);
		_dxp = _dyp = 0;
		SetPos(&rc, pvNil);
		_fGorpDirty = fTrue;
		}
	else
		{
		if (pvNil != pgorp)
			{
			pgorp->AddRef();
			pgorp->Stream(_fStream);
			pgorp->SetDxpDyp(_dxpPref, _dypPref);
			pgorp->GetRc(&rcNew);
			}
		else
			rcNew.Zero();
		ReleasePpo(&_pgorp);
		_pgorp = pgorp;

		GetRc(&rc, cooParent);
		rc.OffsetCopy(&rcNew, _dxp + rc.xpLeft + dxp, _dyp + rc.ypTop + dyp);
		_dxp = -rcNew.xpLeft;
		_dyp = -rcNew.ypTop;

		SetPos(&rc, pvNil);
		if (pvNil != _pgorp)
			{
			_pgorp->GotoNfr(0);
			_pgorp->FPlay();
			}
		_fGorpDirty = fFalse;
		}
}


/***************************************************************************
	Defer or finish defering marking and positioning the gorp.
***************************************************************************/
void GOK::_DeferGorp(bool fDefer)
{
	AssertThis(0);

	if (fDefer)
		_cactDeferGorp++;
	else if (_cactDeferGorp > 0)
		{
		--_cactDeferGorp;
		if (_cactDeferGorp == 0 && _fGorpDirty)
			_SetGorp(_pgorp, 0, 0);
		}
	else
		Bug("_cactDeferGorp is bad");

	_DeferSnd(fDefer);
}


/***************************************************************************
	Start playing the current representation.
***************************************************************************/
bool GOK::FPlay(void)
{
	AssertThis(0);

	return pvNil != _pgorp && _pgorp->FPlay();
}


/***************************************************************************
	Return whether the current representation is being played.
***************************************************************************/
bool GOK::FPlaying(void)
{
	AssertThis(0);

	return pvNil != _pgorp && _pgorp->FPlaying();
}


/***************************************************************************
	Stop playing the current representation.
***************************************************************************/
void GOK::Stop(void)
{
	AssertThis(0);

	if (pvNil != _pgorp)
		_pgorp->Stop();
}


/***************************************************************************
	Goto the given frame of the current representation.
***************************************************************************/
void GOK::GotoNfr(long nfr)
{
	AssertThis(0);

	if (pvNil != _pgorp)
		{
		long nfrOld = _pgorp->NfrCur();
		_pgorp->GotoNfr(nfr);
		if (nfrOld != _pgorp->NfrCur())
			InvalRc(pvNil, kginMark);
		}
}


/***************************************************************************
	Return the number of frames in the current representation.
***************************************************************************/
long GOK::NfrMac(void)
{
	AssertThis(0);

	return pvNil == _pgorp ? 0 : _pgorp->NfrMac();
}


/***************************************************************************
	Return the current frame number of the current representation.
***************************************************************************/
long GOK::NfrCur(void)
{
	AssertThis(0);

	return pvNil == _pgorp ? 0 : _pgorp->NfrCur();
}


/***************************************************************************
	Play a sound and attach the sound to this GOK so that when the GOK
	goes away, the sound will be killed.
***************************************************************************/
long GOK::SiiPlaySound(CTG ctg, CNO cno, long sqn, long vlm,
	long cactPlay, ulong dtsStart, long spr, long scl)
{
	AssertThis(0);

	if (pvNil == vpsndm)
		return siiNil;

	if (siiNil != _siiSound)
		vpsndm->Stop(_siiSound);

	if (cnoNil != cno)
		{
		_siiSound = vpsndm->SiiPlay(_prca, ctg, cno, sqn, vlm,
			cactPlay, dtsStart, spr, scl);
		}
	else
		_siiSound = siiNil;

	return _siiSound;
}


/***************************************************************************
	Defer or stop defering mouse sounds.
***************************************************************************/
void GOK::_DeferSnd(bool fDefer)
{
	AssertThis(0);

	if (fDefer)
		_cactDeferSnd++;
	else if (_cactDeferSnd > 0)
		{
		--_cactDeferSnd;
		if (_cactDeferSnd == 0 && _fMouseSndDirty)
			SiiPlayMouseSound(_ckiMouseSnd.ctg, _ckiMouseSnd.cno);
		}
	else
		Bug("_cactDeferSnd is bad");
}


/***************************************************************************
	Play a sound and attach the sound to this GOK as the mouse tracking
	sound, so that when the GOK goes away and the mouse state changes,
	the sound will be killed.
***************************************************************************/
long GOK::SiiPlayMouseSound(CTG ctg, CNO cno)
{
	AssertThis(0);

	_fMouseSndDirty = fFalse;
	if (pvNil == vpsndm)
		return siiNil;

	if (siiNil != _siiMouse)
		vpsndm->Stop(_siiMouse);

	if (cnoNil != cno)
		_siiMouse = vpsndm->SiiPlay(_prca, ctg, cno);
	else
		_siiMouse = siiNil;

	return _siiMouse;
}


/***************************************************************************
	Play a sound with the given chid as the current mouse tracking sound.
***************************************************************************/
void GOK::_PlayMouseSound(CHID chid)
{
	AssertThis(0);
	KID kid;

	if (pvNil == vpsndm)
		return;

	if (!_pcrf->Pcfl()->FGetKidChidCtg(_pgokd->Ctg(), _pgokd->Cno(),
			chid, kctgWave, &kid) &&
		!_pcrf->Pcfl()->FGetKidChidCtg(_pgokd->Ctg(), _pgokd->Cno(),
			chid, kctgMidi, &kid))
		{
		return;
		}

	if (_cactDeferSnd > 0)
		{
		_ckiMouseSnd = kid.cki;
		_fMouseSndDirty = fTrue;
		}
	else
		SiiPlayMouseSound(kid.cki.ctg, kid.cki.cno);
}


/***************************************************************************
	Our kidworld is being suspended. Make sure no AVIs or other things
	are running.
***************************************************************************/
void GOK::Suspend(void)
{
	AssertThis(0);

	if (pvNil != _pgorp)
		_pgorp->Suspend();
}


/***************************************************************************
	Our kidworld is being resumed. Undo anything we did in Suspend.
***************************************************************************/
void GOK::Resume(void)
{
	AssertThis(0);

	if (pvNil != _pgorp)
		_pgorp->Resume();
}


/***************************************************************************
	The streaming property is being set or reset. If streaming is set, the
	rep should get flushed when we're done with it.
***************************************************************************/
void GOK::Stream(bool fStream)
{
	AssertThis(0);
	_fStream = FPure(fStream);

	if (pvNil != _pgorp)
		_pgorp->Stream(fStream);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a GOK.
***************************************************************************/
void GOK::AssertValid(ulong grf)
{
	GOK_PAR::AssertValid(0);
	AssertPo(_pwoks, 0);
	AssertNilOrPo(_pscegAnim, 0);
	AssertPo(_pcrf, 0);
	AssertPo(_prca, 0);
	AssertNilOrPo(_pgorp, 0);
	AssertNilOrPo(_pglcmflt, 0);
	AssertIn(_gmsCur, gmsNil, kgmsLim);
	if (FIn(_gmsCur, kgmsMinTrack, kgmsLimTrack))
		{
		Assert(vpcex->PgobTracking() == this || vpappb->CactModal() > 0,
			"should be tracking the mouse");
		}
	else
		{
		Assert(vpcex->PgobTracking() != this,
			"shouldn't be tracking the mouse");
		}
}


/***************************************************************************
	Mark memory for the GOK.
***************************************************************************/
void GOK::MarkMem(void)
{
	AssertValid(0);
	GOK_PAR::MarkMem();
	MarkMemObj(_pscegAnim);
	MarkMemObj(_prca);
	MarkMemObj(_pcrf);
	MarkMemObj(_pgorp);
	MarkMemObj(_pglcmflt);
}
#endif //DEBUG


/***************************************************************************
	Stub method for non-frame based representations.
***************************************************************************/
long GORP::NfrMac(void)
{
	AssertThis(0);
	return 0;
}


/***************************************************************************
	Stub method for non-frame based representations.
***************************************************************************/
long GORP::NfrCur(void)
{
	AssertThis(0);
	return 0;
}


/***************************************************************************
	Stub method for non-frame based representations.
***************************************************************************/
void GORP::GotoNfr(long nfr)
{
	AssertThis(0);
}


/***************************************************************************
	Stub method for non-frame based representations.
***************************************************************************/
bool GORP::FPlaying(void)
{
	AssertThis(0);
	return fFalse;
}


/***************************************************************************
	Stub method for non-frame based representations.
***************************************************************************/
bool GORP::FPlay(void)
{
	AssertThis(0);
	return fFalse;
}


/***************************************************************************
	Stub method for non-frame based representations.
***************************************************************************/
void GORP::Stop(void)
{
	AssertThis(0);
}


/***************************************************************************
	Stub method for non-frame based representations.
***************************************************************************/
void GORP::Suspend(void)
{
	AssertThis(0);
}


/***************************************************************************
	Stub method for non-frame based representations.
***************************************************************************/
void GORP::Resume(void)
{
	AssertThis(0);
}


/***************************************************************************
	The streaming property is being set or reset. If streaming is set, we
	should flush our stuff from the RCA cache when we're done with it.
***************************************************************************/
void GORP::Stream(bool fStream)
{
	AssertThis(0);
}


//a FILL chunk
struct GOKFL
	{
	short bo;
	short osk;
	RC rc;
	long lwAcrFore;
	long lwAcrBack;
	byte rgbPat[8];
	};
const BOM kbomGokfl = 0x5FFF0000;


/***************************************************************************
	Static method to create a new fill representation.
***************************************************************************/
PGORF GORF::PgorfNew(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno)
{
	AssertPo(pgok, 0);
	AssertPo(pcrf, 0);

	GOKFL gokfl;
	PCFL pcfl;
	BLCK blck;
	PGORF pgorf;

	pcfl = pcrf->Pcfl();
	if (!pcfl->FFind(ctg, cno, &blck) || !blck.FUnpackData() ||
			blck.Cb() != size(GOKFL) || !blck.FRead(&gokfl))
		{
		Warn("couldn't load indicated fill");
		return pvNil;
		}
	if (kboOther == gokfl.bo)
		SwapBytesBom(&gokfl, kbomGokfl);
	if (kboCur != gokfl.bo)
		{
		Warn("Bad FILL chunk");
		return pvNil;
		}

	if (pvNil == (pgorf = NewObj GORF))
		return pvNil;

	pgorf->_acrFore.SetFromLw(gokfl.lwAcrFore);
	pgorf->_acrBack.SetFromLw(gokfl.lwAcrBack);
	CopyPb(gokfl.rgbPat, pgorf->_apt.rgb, size(gokfl.rgbPat));
	pgorf->_rc = gokfl.rc;
	pgorf->_dxp = gokfl.rc.Dxp();
	pgorf->_dyp = gokfl.rc.Dyp();

	AssertPo(pgorf, 0);
	return pgorf;
}



/***************************************************************************
	Fill the rectangle.
***************************************************************************/
void GORF::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	RC rc(0, 0, _dxp, _dyp);

	if (rc.FIntersect(prcClip) &&
			(_acrFore != kacrClear || _acrBack != kacrClear))
		{
		pgnv->FillRcApt(&rc, &_apt, _acrFore, _acrBack);
		}
}


/***************************************************************************
	Return whether the point is in the representation.
***************************************************************************/
bool GORF::FPtIn(long xp, long yp)
{
	AssertThis(0);
	return FIn(xp, 0, _dxp) && FIn(yp, 0, _dyp);
}


/***************************************************************************
	Set the internal state of the GORF to accomodate the given preferred
	size of the content area.
***************************************************************************/
void GORF::SetDxpDyp(long dxpPref, long dypPref)
{
	AssertThis(0);
	AssertIn(dxpPref, 0, kcbMax);
	AssertIn(dypPref, 0, kcbMax);

	if ((_dxp = dxpPref) == 0)
		_dxp = _rc.Dxp();
	if ((_dyp = dypPref) == 0)
		_dyp = _rc.Dyp();
}


/***************************************************************************
	Get the natural rectangle.
***************************************************************************/
void GORF::GetRc(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	prc->Set(0, 0, _dxp, _dyp);
	prc->CenterOnRc(&_rc);
}


/***************************************************************************
	Get the interior content rectangle.
***************************************************************************/
void GORF::GetRcContent(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	GetRc(prc);
	prc->OffsetToOrigin();
}


/***************************************************************************
	Create a new masked bitmap representation of a graphical object.
***************************************************************************/
PGORB GORB::PgorbNew(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno)
{
	AssertPo(pgok, 0);
	AssertPo(pcrf, 0);
	PGORB pgorb;

	if (pvNil == (pgorb = NewObj GORB))
		return pvNil;

	pgorb->_pcrf = pcrf;
	pgorb->_pcrf->AddRef();
	pgorb->_ctg = ctg;
	pgorb->_cno = cno;
	AssertPo(pgorb, 0);
	return pgorb;
}


/***************************************************************************
	Destructor for a GORB.
***************************************************************************/
GORB::~GORB(void)
{
	if (_fStream && pvNil != _pcrf && ctgNil != _ctg)
		_pcrf->FSetCrep(crepToss, _ctg, _cno, MBMP::FReadMbmp);
	ReleasePpo(&_pcrf);
}


/***************************************************************************
	Draw the bitmap.
***************************************************************************/
void GORB::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	PMBMP pmbmp;
	RC rc;

	if (kctgMbmp != _ctg)
		return;

	if (pvNil != (pmbmp = (PMBMP)_pcrf->PbacoFetch(_ctg, _cno,
			MBMP::FReadMbmp)))
		{
		AssertPo(pmbmp, 0);
		pmbmp->GetRc(&rc);
		rc.OffsetToOrigin();
		pgnv->DrawMbmp(pmbmp, &rc);
		ReleasePpo(&pmbmp);
		}
	else
		Warn("Couldn't load kidspace bitmap");
}


/***************************************************************************
	Return whether the point is in the bitmap.
***************************************************************************/
bool GORB::FPtIn(long xp, long yp)
{
	AssertThis(0);
	PMBMP pmbmp;
	RC rc;
	bool fRet;

	if (pvNil == (pmbmp = (PMBMP)_pcrf->PbacoFetch(_ctg, _cno,
			MBMP::FReadMbmp)))
		{
		Warn("Couldn't load kidspace bitmap");
		return fTrue;
		}

	AssertPo(pmbmp, 0);
	pmbmp->GetRc(&rc);
	fRet = pmbmp->FPtIn(xp + rc.xpLeft, yp + rc.ypTop);
	ReleasePpo(&pmbmp);
	return fRet;
}


/***************************************************************************
	Set the internal state of the GORB to accomodate the given preferred
	size of the content area.
***************************************************************************/
void GORB::SetDxpDyp(long dxpPref, long dypPref)
{
	AssertThis(0);
	AssertIn(dxpPref, 0, kcbMax);
	AssertIn(dypPref, 0, kcbMax);

	//GORB's are not resizeable, so we do nothing
}


/***************************************************************************
	Get the natural rectangle.
***************************************************************************/
void GORB::GetRc(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	PMBMP pmbmp;

	if (pvNil != (pmbmp = (PMBMP)_pcrf->PbacoFetch(_ctg, _cno,
			MBMP::FReadMbmp)))
		{
		AssertPo(pmbmp, 0);
		pmbmp->GetRc(prc);
		ReleasePpo(&pmbmp);
		}
	else
		{
		Warn("Couldn't load kidspace bitmap");
		prc->Zero();
		}
}


/***************************************************************************
	Get the interior content rectangle.
***************************************************************************/
void GORB::GetRcContent(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	GetRc(prc);
	prc->OffsetToOrigin();
}


/***************************************************************************
	The streaming property is being set or reset. If streaming is set, we
	should flush our stuff from the RCA cache when we're done with it.
***************************************************************************/
void GORB::Stream(bool fStream)
{
	AssertThis(0);
	_fStream = FPure(fStream);
}


/***************************************************************************
	Create a new tile representation.
***************************************************************************/
PGORT GORT::PgortNew(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno)
{
	AssertPo(pgok, 0);
	AssertPo(pcrf, 0);

	GOTIL gotil;
	PCFL pcfl;
	BLCK blck;
	PGORT pgort;
	KID kid;

	pcfl = pcrf->Pcfl();
	if (!pcfl->FFind(ctg, cno, &blck) || !blck.FUnpackData() ||
			blck.Cb() != size(GOTIL) || !blck.FRead(&gotil))
		{
		Warn("couldn't load indicated tile chunk");
		return pvNil;
		}

	if (kboOther == gotil.bo)
		SwapBytesRgsw(&gotil, size(gotil) / size(short));

	if (kboCur != gotil.bo ||

		gotil.rgdxp[idzpLeftBorder] < 0 ||
		gotil.rgdxp[idzpRightBorder] < 0 ||
		gotil.rgdxp[idzpLeft] < 0 ||
		gotil.rgdxp[idzpRight] < 0 ||
		gotil.rgdxp[idzpLeftFlex] < 0 ||
		gotil.rgdxp[idzpRightFlex] < 0 ||
		gotil.rgdxp[idzpLeftFlex] + gotil.rgdxp[idzpRightFlex] <= 0 ||
		gotil.rgdxp[idzpLeftInc] < (gotil.rgdxp[idzpLeftFlex] > 0) ||
		gotil.rgdxp[idzpRightInc] < (gotil.rgdxp[idzpRightFlex] > 0) ||
		gotil.rgdxp[idzpLeftInc] > gotil.rgdxp[idzpLeftFlex] ||
		gotil.rgdxp[idzpRightInc] > gotil.rgdxp[idzpRightFlex] ||

		gotil.rgdyp[idzpLeftBorder] < 0 ||
		gotil.rgdyp[idzpRightBorder] < 0 ||
		gotil.rgdyp[idzpLeft] < 0 ||
		gotil.rgdyp[idzpRight] < 0 ||
		gotil.rgdyp[idzpLeftFlex] < 0 ||
		gotil.rgdyp[idzpRightFlex] < 0 ||
		gotil.rgdyp[idzpLeftFlex] + gotil.rgdyp[idzpRightFlex] <= 0 ||
		gotil.rgdyp[idzpLeftInc] < (gotil.rgdyp[idzpLeftFlex] > 0) ||
		gotil.rgdyp[idzpRightInc] < (gotil.rgdyp[idzpRightFlex] > 0) ||
		gotil.rgdyp[idzpLeftInc] > gotil.rgdyp[idzpLeftFlex] ||
		gotil.rgdyp[idzpRightInc] > gotil.rgdyp[idzpRightFlex])
		{
		Warn("Bad TILE chunk");
		return pvNil;
		}

	if (!pcfl->FGetKidChidCtg(ctg, cno, 0, kctgMbmp, &kid))
		{
		Warn("no child MBMP of TILE chunk");
		return pvNil;
		}

	if (pvNil == (pgort = NewObj GORT))
		return pvNil;

	CopyPb(gotil.rgdxp, pgort->_rgdxp, size(gotil.rgdxp));
	CopyPb(gotil.rgdyp, pgort->_rgdyp, size(gotil.rgdyp));
	pgort->_pcrf = pcrf;
	pgort->_pcrf->AddRef();
	pgort->_ctg = kid.cki.ctg;
	pgort->_cno = kid.cki.cno;
	pgort->_dxp = pgort->_rgdxp[idzpLeft] + pgort->_rgdxp[idzpMid] +
		pgort->_rgdxp[idzpRight];
	pgort->_dyp = pgort->_rgdyp[idzpLeft] + pgort->_rgdyp[idzpMid] +
		pgort->_rgdyp[idzpRight];

	AssertPo(pgort, 0);
	return pgort;
}


/***************************************************************************
	Destructor for a GORT.
***************************************************************************/
GORT::~GORT(void)
{
	if (_fStream && pvNil != _pcrf && ctgNil != _ctg)
		_pcrf->FSetCrep(crepToss, _ctg, _cno, MBMP::FReadMbmp);
	ReleasePpo(&_pcrf);
}


/***************************************************************************
	Draw the GORT.
***************************************************************************/
void GORT::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);
	long ypLim, dyp;
	PMBMP pmbmp;
	RC rcRow;

	if (pvNil == (pmbmp = (PMBMP)_pcrf->PbacoFetch(_ctg, _cno,
			MBMP::FReadMbmp)))
		{
		Warn("Couldn't load kidspace bitmap");
		return;
		}

	AssertPo(pmbmp, 0);
	dyp = 0;

	// draw the top row
	ypLim = _rgdyp[idzpLeft] + _dypLeftFlex;
	rcRow.Set(0, 0, _dxp,
		LwMin(ypLim, _rgdyp[idzpLeft] + _rgdyp[idzpLeftFlex]));
	_DrawRow(pgnv, pmbmp, &rcRow, prcClip, 0, dyp);

	// draw the top flex rows
	dyp -= _rgdyp[idzpLeft];
	while (rcRow.ypBottom < ypLim)
		{
		rcRow.ypTop = rcRow.ypBottom;
		rcRow.ypBottom = LwMin(rcRow.ypTop + _rgdyp[idzpLeftFlex], ypLim);
		_DrawRow(pgnv, pmbmp, &rcRow, prcClip, 0, dyp);
		}

	// draw the middle row
	ypLim = rcRow.ypBottom + _rgdyp[idzpMid] + _dypRightFlex;
	rcRow.ypTop = rcRow.ypBottom;
	rcRow.ypBottom = LwMin(ypLim, rcRow.ypTop + _rgdyp[idzpMid] +
		_rgdyp[idzpRightFlex]);
	dyp -= _rgdyp[idzpLeftFlex];
	_DrawRow(pgnv, pmbmp, &rcRow, prcClip, 0, dyp);

	// draw the bottom flex rows
	dyp -= _rgdyp[idzpMid];
	while (rcRow.ypBottom < ypLim)
		{
		rcRow.ypTop = rcRow.ypBottom;
		rcRow.ypBottom = LwMin(rcRow.ypTop + _rgdyp[idzpRightFlex], ypLim);
		_DrawRow(pgnv, pmbmp, &rcRow, prcClip, 0, dyp);
		}

	// draw the bottom row
	rcRow.ypTop = rcRow.ypBottom;
	rcRow.ypBottom = _dyp;
	dyp -= _rgdyp[idzpRightFlex];
	_DrawRow(pgnv, pmbmp, &rcRow, prcClip, 0, dyp);

	ReleasePpo(&pmbmp);
}


/***************************************************************************
	Draw a row of the tiled bitmap.
***************************************************************************/
void GORT::_DrawRow(PGNV pgnv, PMBMP pmbmp, RC *prcRow, RC *prcClip,
	long dxp, long dyp)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertPo(pmbmp, 0);
	AssertVarMem(prcRow);
	AssertVarMem(prcClip);
	long xpLim;
	RC rcClip, rc, rcMbmp;

	if (!rcClip.FIntersect(prcRow, prcClip))
		return;

	pmbmp->GetRc(&rcMbmp);

	// draw the left cell
	xpLim = prcRow->xpLeft + _rgdxp[idzpLeft] + _dxpLeftFlex;
	rc = *prcRow;
	rc.xpRight = LwMin(xpLim, rc.xpLeft + _rgdxp[idzpLeft] +
		_rgdxp[idzpLeftFlex]);
	dyp += rc.ypTop;
	if (rcClip.FIntersect(prcClip, &rc))
		{
		pgnv->ClipRc(&rcClip);
		rcMbmp.Offset(rc.xpLeft + dxp - rcMbmp.xpLeft, dyp - rcMbmp.ypTop);
		pgnv->DrawMbmp(pmbmp, &rcMbmp);
		}

	// draw the left flex cells
	dxp -= _rgdxp[idzpLeft];
	while (rc.xpRight < xpLim)
		{
		rc.xpLeft = rc.xpRight;
		rc.xpRight = LwMin(rc.xpLeft + _rgdxp[idzpLeftFlex], xpLim);
		if (rcClip.FIntersect(prcClip, &rc))
			{
			pgnv->ClipRc(&rcClip);
			rcMbmp.Offset(rc.xpLeft + dxp - rcMbmp.xpLeft, dyp - rcMbmp.ypTop);
			pgnv->DrawMbmp(pmbmp, &rcMbmp);
			}
		}

	// draw the middle cell
	xpLim = rc.xpRight + _rgdxp[idzpMid] + _dxpRightFlex;
	rc.xpLeft = rc.xpRight;
	rc.xpRight = LwMin(xpLim, rc.xpLeft + _rgdxp[idzpMid] +
		_rgdxp[idzpRightFlex]);
	dxp -= _rgdxp[idzpLeftFlex];
	if (rcClip.FIntersect(prcClip, &rc))
		{
		pgnv->ClipRc(&rcClip);
		rcMbmp.Offset(rc.xpLeft + dxp - rcMbmp.xpLeft, dyp - rcMbmp.ypTop);
		pgnv->DrawMbmp(pmbmp, &rcMbmp);
		}

	// draw the right flex cells
	dxp -= _rgdxp[idzpMid];
	while (rc.xpRight < xpLim)
		{
		rc.xpLeft = rc.xpRight;
		rc.xpRight = LwMin(rc.xpLeft + _rgdxp[idzpRightFlex], xpLim);
		if (rcClip.FIntersect(prcClip, &rc))
			{
			pgnv->ClipRc(&rcClip);
			rcMbmp.Offset(rc.xpLeft + dxp - rcMbmp.xpLeft, dyp - rcMbmp.ypTop);
			pgnv->DrawMbmp(pmbmp, &rcMbmp);
			}
		}

	// draw the right cell
	rc.xpLeft = rc.xpRight;
	rc.xpRight = prcRow->xpRight;
	dxp -= _rgdxp[idzpRightFlex];
	if (rcClip.FIntersect(prcClip, &rc))
		{
		pgnv->ClipRc(&rcClip);
		rcMbmp.Offset(rc.xpLeft + dxp - rcMbmp.xpLeft, dyp - rcMbmp.ypTop);
		pgnv->DrawMbmp(pmbmp, &rcMbmp);
		}
}


/***************************************************************************
	Do hit testing on a tiled bitmap.
***************************************************************************/
bool GORT::FPtIn(long xp, long yp)
{
	AssertThis(0);
	PMBMP pmbmp;
	RC rc;
	bool fRet;

	if (!FIn(xp, 0, _dxp) || !FIn(yp, 0, _dyp))
		return fFalse;

	if (pvNil == (pmbmp = (PMBMP)_pcrf->PbacoFetch(_ctg, _cno,
			MBMP::FReadMbmp)))
		{
		Warn("Couldn't load kidspace bitmap");
		return fTrue;
		}
	AssertPo(pmbmp, 0);

	_MapZpToMbmp(&xp, _rgdxp, _dxpLeftFlex, _dxpRightFlex);
	_MapZpToMbmp(&yp, _rgdyp, _dypLeftFlex, _dypRightFlex);

	pmbmp->GetRc(&rc);
	fRet = pmbmp->FPtIn(xp + rc.xpLeft, yp + rc.ypTop);
	ReleasePpo(&pmbmp);
	return fRet;
}


/***************************************************************************
	Map the point from flexed coordinates to zero based mbmp coordinates
	for hit testing.
***************************************************************************/
void GORT::_MapZpToMbmp(long *pzp, short *prgdzp,
	long dzpLeftFlex, long dzpRightFlex)
{
	AssertThis(0);
	AssertVarMem(pzp);
	AssertPvCb(prgdzp, LwMul(idzpLimGort, size(short)));
	long dzp;

	if (*pzp < (dzp = prgdzp[idzpLeft]))
		;
	else if (*pzp < dzp + dzpLeftFlex)
		*pzp = (*pzp - dzp) % prgdzp[idzpLeftFlex] + dzp;
	else if ((*pzp += prgdzp[idzpLeftFlex] - dzpLeftFlex) <
			(dzp += prgdzp[idzpLeftFlex] + prgdzp[idzpMid]))
		{
		}
	else if (*pzp < dzp + dzpRightFlex)
		*pzp = (*pzp - dzp) % prgdzp[idzpRightFlex] + dzp;
	else
		*pzp += prgdzp[idzpRightFlex] - dzpRightFlex;
}


/***************************************************************************
	Set the internal state of the GORT to accomodate the given preferred
	size of the content area.
***************************************************************************/
void GORT::SetDxpDyp(long dxpPref, long dypPref)
{
	AssertThis(0);
	AssertIn(dxpPref, 0, kcbMax);
	AssertIn(dypPref, 0, kcbMax);

	dxpPref += _rgdxp[idzpLeftBorder] + _rgdxp[idzpRightBorder];
	dypPref += _rgdyp[idzpLeftBorder] + _rgdyp[idzpRightBorder];
	_ComputeFlexZp(&_dxpLeftFlex, &_dxpRightFlex, dxpPref, _rgdxp);
	_ComputeFlexZp(&_dypLeftFlex, &_dypRightFlex, dypPref, _rgdyp);
	_dxp = _dxpLeftFlex + _dxpRightFlex + _rgdxp[idzpLeft] +
		_rgdxp[idzpMid] + _rgdxp[idzpRight];
	_dyp = _dypLeftFlex + _dypRightFlex + _rgdyp[idzpLeft] +
		_rgdyp[idzpMid] + _rgdyp[idzpRight];
}


/***************************************************************************
	Compute the flex values in one direction.
***************************************************************************/
void GORT::_ComputeFlexZp(long *pdzpLeftFlex, long *pdzpRightFlex, long dzp,
	short *prgdzp)
{
	AssertThis(0);
	AssertVarMem(pdzpLeftFlex);
	AssertVarMem(pdzpRightFlex);
	AssertPvCb(prgdzp, LwMul(idzpLimGort, size(short)));

	*pdzpRightFlex = LwMax(0, dzp - prgdzp[idzpLeft] - prgdzp[idzpMid] -
		prgdzp[idzpRight]);
	*pdzpLeftFlex = LwMulDiv(*pdzpRightFlex, prgdzp[idzpLeftFlex],
		prgdzp[idzpLeftFlex] + prgdzp[idzpRightFlex]);
	if (*pdzpLeftFlex > 0)
		{
		long dzpT = LwRoundToward(*pdzpLeftFlex, prgdzp[idzpLeftFlex]);
		*pdzpLeftFlex = dzpT + LwMin(prgdzp[idzpLeftFlex],
			LwRoundAway(*pdzpLeftFlex - dzpT, prgdzp[idzpLeftInc]));
		}
	*pdzpRightFlex = LwMax(0, *pdzpRightFlex - *pdzpLeftFlex);
	if (*pdzpRightFlex > 0)
		{
		long dzpT = LwRoundToward(*pdzpRightFlex, prgdzp[idzpRightFlex]);
		*pdzpRightFlex = dzpT + LwMin(prgdzp[idzpRightFlex],
			LwRoundAway(*pdzpRightFlex - dzpT, prgdzp[idzpRightInc]));
		}
}


/***************************************************************************
	Get the bounding rectangle for the tiled bitmap.
***************************************************************************/
void GORT::GetRc(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);
	RC rc;
	PT pt;
	PMBMP pmbmp;

	prc->Set(0, 0, _dxp, _dyp);
	if (pvNil == (pmbmp = (PMBMP)_pcrf->PbacoFetch(_ctg, _cno,
			MBMP::FReadMbmp)))
		{
		Warn("Couldn't load kidspace bitmap");
		return;
		}
	AssertPo(pmbmp, 0);
	pmbmp->GetRc(&rc);
	ReleasePpo(&pmbmp);

	pt.xp = -rc.xpLeft;
	pt.yp = -rc.ypTop;

	// map the point from the MBMP to the flexed rectangle, scaling
	// proportionally if the point is in a flex portion of the MBMP
	_MapZpFlex(&pt.xp, _rgdxp, _dxpLeftFlex, _dxpRightFlex);
	_MapZpFlex(&pt.yp, _rgdyp, _dypLeftFlex, _dypRightFlex);

	// now make pt the registration point
	prc->Offset(-pt.xp, -pt.yp);
}


/***************************************************************************
	Map the point from MBMP coordinates to flexed coordinates for setting the
	registration point. WARNING: this is not the inverse of _MapZpToMbmp.
***************************************************************************/
void GORT::_MapZpFlex(long *pzp, short *prgdzp,
	long dzpLeftFlex, long dzpRightFlex)
{
	AssertThis(0);
	AssertVarMem(pzp);
	AssertPvCb(prgdzp, LwMul(idzpLimGort, size(short)));
	long dzp;

	if (*pzp < (dzp = prgdzp[idzpLeft]))
		;
	else if (*pzp < dzp + prgdzp[idzpLeftFlex])
		*pzp = LwMulDiv(*pzp - dzp, dzpLeftFlex, prgdzp[idzpLeftFlex]) + dzp;
	else if ((*pzp += dzpLeftFlex - prgdzp[idzpLeftFlex]) <
			(dzp += dzpLeftFlex + prgdzp[idzpMid]))
		{
		}
	else if (*pzp < dzp + prgdzp[idzpRightFlex])
		*pzp = LwMulDiv(*pzp - dzp, dzpRightFlex, prgdzp[idzpRightFlex]) + dzp;
	else
		*pzp += dzpRightFlex - prgdzp[idzpRightFlex];
}


/***************************************************************************
	Get the interior content rectangle.
***************************************************************************/
void GORT::GetRcContent(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	prc->xpLeft = _rgdxp[idzpLeftBorder];
	prc->xpRight = _dxp - _rgdxp[idzpRightBorder];
	prc->ypTop = _rgdyp[idzpLeftBorder];
	prc->ypBottom = _dyp - _rgdyp[idzpRightBorder];
}


/***************************************************************************
	The streaming property is being set or reset. If streaming is set, we
	should flush our stuff from the RCA cache when we're done with it.
***************************************************************************/
void GORT::Stream(bool fStream)
{
	AssertThis(0);
	_fStream = FPure(fStream);
}


/***************************************************************************
	Static method to create a new video representation.
***************************************************************************/
PGORV GORV::PgorvNew(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno)
{
	AssertPo(pgok, 0);
	AssertPo(pcrf, 0);
	PGORV pgorv;

	if (pvNil == (pgorv = NewObj GORV))
		return pvNil;

	if (!pgorv->_FInit(pgok, pcrf, ctg, cno))
		{
		ReleasePpo(&pgorv);
		return pvNil;
		}

	AssertPo(pgorv, 0);
	return pgorv;
}


/***************************************************************************
	Initialize the GORV - load the movie indicated byt (pcrf, ctg, cno).
***************************************************************************/
bool GORV::_FInit(PGOK pgok, PCRF pcrf, CTG ctg, CNO cno)
{
	AssertPo(pgok, 0);
	AssertBaseThis(0);

	PCFL pcfl;
	BLCK blck;
	STN stn;
	FNI fni;
	RC rc;
	byte bT;

	pcfl = pcrf->Pcfl();
	if (!pcfl->FFind(ctg, cno, &blck) || !blck.FUnpackData() || blck.Cb() < 2 ||
			!blck.FReadRgb(&bT, 1, 0) || !stn.FRead(&blck, 1))
		{
		goto LFail;
		}

	if (!pgok->Pwoks()->FFindFile(&stn, &fni) ||
			pvNil == (_pgvid = GVID::PgvidNew(&fni, pgok, bT)))
		{
LFail:
		Warn("couldn't load indicated video");
		return fFalse;
		}

	_pgvid->GetRc(&rc);
	_dxp = rc.Dxp();
	_dyp = rc.Dyp();
	_fHwndBased = FPure(bT);
	return fTrue;
}


/***************************************************************************
	Destructor for a video representation.
***************************************************************************/
GORV::~GORV(void)
{
	ReleasePpo(&_pgvid);
}


/***************************************************************************
	Draw the frame.
***************************************************************************/
void GORV::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	RC rc(0, 0, _dxp, _dyp);

	_pgvid->Draw(pgnv, &rc);
}


/***************************************************************************
	Return whether the point is in the representation.
***************************************************************************/
bool GORV::FPtIn(long xp, long yp)
{
	AssertThis(0);
	return FIn(xp, 0, _dxp) && FIn(yp, 0, _dyp);
}


/***************************************************************************
	Set the internal state of the GORF to accomodate the given preferred
	size of the content area.
***************************************************************************/
void GORV::SetDxpDyp(long dxpPref, long dypPref)
{
	AssertThis(0);
	AssertIn(dxpPref, 0, kcbMax);
	AssertIn(dypPref, 0, kcbMax);
	RC rc;

	_pgvid->GetRc(&rc);
	if ((_dxp = dxpPref) == 0)
		_dxp = rc.Dxp();
	if ((_dyp = dypPref) == 0)
		_dyp = rc.Dyp();
}


/***************************************************************************
	Get the natural rectangle.
***************************************************************************/
void GORV::GetRc(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	prc->Set(0, 0, _dxp, _dyp);
}


/***************************************************************************
	Get the interior content rectangle.
***************************************************************************/
void GORV::GetRcContent(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	prc->Set(0, 0, _dxp, _dyp);
}


/***************************************************************************
	Return the length of the video.
***************************************************************************/
long GORV::NfrMac(void)
{
	AssertThis(0);
	return _pgvid->NfrMac();
}


/***************************************************************************
	Return the current frame of the video.
***************************************************************************/
long GORV::NfrCur(void)
{
	AssertThis(0);
	return _pgvid->NfrCur();
}


/***************************************************************************
	Goto a particular frame.
***************************************************************************/
void GORV::GotoNfr(long nfr)
{
	AssertThis(0);
	_pgvid->GotoNfr(nfr);
}


/***************************************************************************
	Return whether the video is playing.
***************************************************************************/
bool GORV::FPlaying(void)
{
	AssertThis(0);
	return (_cactSuspend > 0 && _fPlayOnResume) || _pgvid->FPlaying();
}


/***************************************************************************
	Play from the current frame to the end.
***************************************************************************/
bool GORV::FPlay(void)
{
	AssertThis(0);
	RC rc(0, 0, _dxp, _dyp);

	if (_cactSuspend > 0 && _fHwndBased)
		rc.Zero();
	return _pgvid->FPlay(&rc);
}


/***************************************************************************
	Stop playing.
***************************************************************************/
void GORV::Stop(void)
{
	AssertThis(0);
	_pgvid->Stop();
}


/***************************************************************************
	Suspend the video.
***************************************************************************/
void GORV::Suspend(void)
{
	AssertThis(0);

	if (0 == _cactSuspend++)
		{
		_fPlayOnResume = FPure(_pgvid->FPlaying());
		_pgvid->Stop();
		if (_fHwndBased)
			{
			RC rc(0, 0, 0, 0);
			_pgvid->SetRcPlay(&rc);
			}
		}
}


/***************************************************************************
	Resume the video.
***************************************************************************/
void GORV::Resume(void)
{
	AssertThis(0);

	if (0 == --_cactSuspend && _fPlayOnResume)
		{
		FPlay();
		_fPlayOnResume = fFalse;
		}
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a GORV.
***************************************************************************/
void GORV::AssertValid(ulong grf)
{
	GORV_PAR::AssertValid(0);
	AssertPo(_pgvid, 0);
}


/***************************************************************************
	Mark memory for the GORV.
***************************************************************************/
void GORV::MarkMem(void)
{
	AssertValid(0);
	GORV_PAR::MarkMem();
	MarkMemObj(_pgvid);
}
#endif //DEBUG



