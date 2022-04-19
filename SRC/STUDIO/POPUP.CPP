/***************************************************************************

	popup.cpp: Popup menu classes

	Primary Author: manders
	         MPFNT: peted
	Review Status: REVIEWED - any changes to this file must be reviewed!


***************************************************************************/
#include "studio.h"
ASSERTNAME

RTCLASS(MP)
RTCLASS(MPFNT)

BEGIN_CMD_MAP(MP, BRWD)
	ON_CID_GEN(cidSelIdle, FCmdSelIdle, pvNil)
END_CMD_MAP_NIL()


/***************************************************************************
	Create a new popup menu
***************************************************************************/
PMP MP::PmpNew(long kidParent, long kidMenu, PRCA prca, PCMD pcmd,
	BWS bws, long ithumSelect, long sidSelect, CKI ckiRoot, CTG ctg,
	PCMH pcmh, long cid, bool fMoveTop)
{
	AssertPo(prca, 0);
	AssertVarMem(pcmd);
	AssertPo(pcmh, 0);

	PMP pmp;
	GCB gcb;
	PSTDIO pstdio;
	long cthum;
	long cfrm;

	pstdio = vpapp->Pstdio();
	if (pvNil == pstdio)
		{
		Bug("pstdio is nil");
		return pvNil;
		}

	if (!_FBuildGcb(&gcb, kidParent, kidMenu))
		return pvNil;

	pmp = NewObj MP(&gcb);
	if (pvNil == pmp)
		goto LFail;
	if (!pmp->_FInitGok(prca, kidMenu))
		goto LFail;
	if (!pmp->FInit(pcmd, bws, ithumSelect, sidSelect, ckiRoot,
		ctg, pstdio, pvNil, fFalse, 1))
		{
		goto LFail;
		}

	//
	// Add ourselves as a CMH with a high priority so that we can
	// filter out SelIdle messages
	//
	if (!vpcex->FAddCmh(pmp, -1000))
		{
		goto LFail;
		}
	if (!pmp->FDraw())
		goto LFail;

	pmp->_cid = cid;
	pmp->_pcmh = pcmh;

	// Need to adjust the size if cthum <= cfrm
	// New height should be dypTop + cthum * dypFrm + dypTop
	cthum = pmp->_Cthum();
	cfrm = pmp->_cfrm;

	if (cthum <= cfrm)
		{
		PGOB pgob;
		long dypFrm;
		long dypTop;
		RC rc;
		RC rcAbs;
		RC rcRel;

		pgob = vpapp->Pkwa()->PgobFromHid(pmp->_kidFrmFirst);
		AssertPo(pgob, 0);
		pgob->GetRc(&rc, cooParent);
		dypFrm = rc.Dyp();
		dypTop = rc.ypTop;
		pmp->GetPos(&rcAbs, &rcRel);
		if (fMoveTop)
			rcAbs.ypTop = rcAbs.ypBottom - (dypTop + LwMul(cthum, dypFrm) + dypTop);
		else
			rcAbs.ypBottom = rcAbs.ypTop + (dypTop + LwMul(cthum, dypFrm) + dypTop);
		pmp->SetPos(&rcAbs, &rcRel);
		}
	AssertPo(pmp, 0);
	return pmp;
LFail:
	ReleasePpo(&pmp);
	return pvNil;
}


/***************************************************************************
	Enqueue a cid saying what was selected
***************************************************************************/
void MP::_ApplySelection(long ithumSelect, long sid)
{
	AssertThis(0);

	if (_ckiRoot.ctg == kctgTyth)
		{
		THD thd;

		_pglthd->Get(ithumSelect, &thd);
		vpcex->EnqueueCid(_cid, _pcmh, pvNil, thd.grfontMask, thd.grfont);
		return;
		}
	vpcex->EnqueueCid(_cid, _pcmh, pvNil, ithumSelect, sid);
}


/******************************************************************************
	_IthumFromThum
		Override the default BWRL routine so that we can map a font bitfield
		to the proper ithum.
	
	
	Arguments:
		long thumSelect
		long sidSelect
	
	Returns:
	
	Keywords:
	
************************************************************ PETED ***********/
long MP::_IthumFromThum(long thumSelect, long sidSelect)
{
	AssertBaseThis(0);

	long ithum;

	if (_ckiRoot.ctg == kctgTyth)
		{
		THD thd;

		ithum = _pglthd->IvMac();
		while (ithum-- > 0)
			{
			_pglthd->Get(ithum, &thd);
			if (thd.grfont == (thumSelect & thd.grfontMask))
				break;
			}
		Assert(ithum >= 0 || ithum == ivNil, "Returning invalid ithum");
		}
	else
		ithum = MP_PAR::_IthumFromThum(thumSelect, sidSelect);
	return ithum;
}


/***************************************************************************
	Do selection idle processing.  Make sure not to change any selection
	states.
***************************************************************************/
bool MP::FCmdSelIdle(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	MP_PAR::FCmdSelIdle(pcmd);
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the MP.
***************************************************************************/
void MP::AssertValid(ulong grf)
{
	MP_PAR::AssertValid(fobjAllocated);
	AssertBasePo(_pcmh, 0);
}


/***************************************************************************
	Mark memory used by the MP
***************************************************************************/
void MP::MarkMem(void)
{
	AssertThis(0);
	MP_PAR::MarkMem();
	MarkMemObj(_pcmh);
}
#endif //DEBUG


//
//
//
//  MPFNT (font menu) stuff begins here
//
//
//


BEGIN_CMD_MAP(MPFNT, BRWD)
	ON_CID_GEN(cidSelIdle, FCmdSelIdle, pvNil)
END_CMD_MAP_NIL()

/***************************************************************************
	Create a new font menu
***************************************************************************/
PMPFNT MPFNT::PmpfntNew(PRCA prca, long kidParent, long kidMenu, PCMD pcmd,
	long ithumSelect, PGST pgst)
{
	AssertPo(prca, 0);
	AssertVarMem(pcmd);
	AssertPo(pgst, 0);

	PMPFNT pmpfnt;
	GCB gcb;
	PSTDIO pstdio = vpapp->Pstdio();

	if (pstdio == pvNil)
		{
		Bug("pstdio is nil");
		return pvNil;
		}

	if (pgst->CbExtra() != size(long))
		{
		Bug("GST CbExtra isn't the right size for an onn");
		return pvNil;
		}

	if (!_FBuildGcb(&gcb, kidParent, kidMenu))
		return pvNil;

	pmpfnt = NewObj MPFNT(&gcb);
	if (pmpfnt == pvNil)
		return pvNil;

	pmpfnt->_pgst = pgst;
	pmpfnt->_pgst->AddRef();

	if (!pmpfnt->_FInitGok(prca, kidMenu))
		goto LFail;

	if (!pmpfnt->FInit(pcmd, ithumSelect, ithumSelect, pstdio, fFalse, 1))
		goto LFail;

	//
	// Add ourselves as a CMH with a high priority so that we can
	// filter out SelIdle messages
	//
	if (!vpcex->FAddCmh(pmpfnt, -1000))
		{
		goto LFail;
		}

	if (!pmpfnt->FDraw())
		goto LFail;
	pmpfnt->_AdjustRc(pmpfnt->_Cthum(), pmpfnt->_cfrm);

	AssertPo(pmpfnt, 0);
	return pmpfnt;

LFail:
	ReleasePpo(&pmpfnt);
	return pvNil;
}


/***************************************************************************
	Set the font of the TGOB to the font listed in the menu item
***************************************************************************/
bool MPFNT::_FSetThumFrame(long istn, PGOB pgobPar)
{
	if (MPFNT_PAR::_FSetThumFrame(istn, pgobPar))
		{
		PTGOB ptgob = (PTGOB)pgobPar->PgobFirstChild();
		long onn;

		/* By the time we get this far, MPFNT_PAR should have already checked
			these */
		Assert(ptgob != pvNil, "No TGOB for the text");
		Assert(ptgob->FIs(kclsTGOB), "GOB isn't a TGOB");

		_pgst->GetExtra(istn, &onn);
		ptgob->SetFont(onn);
		ptgob->SetAlign(tahLeft, tavCenter);
		return fTrue;
		}
	return fFalse;
}


/***************************************************************************
	Tell the studio that the font was selected
***************************************************************************/
void MPFNT::_ApplySelection(long ithumSelect, long sid)
{
	AssertThis(0);

	long onn;

	_pgst->GetExtra(ithumSelect, &onn);
	vpcex->EnqueueCid(cidTextSetFont, _pstdio, pvNil, onn);
}


/***************************************************************************
	Hide the scroll arrows if necessary
***************************************************************************/
void MPFNT::_AdjustRc(long cthum, long cfrm)
{
	PGOB pgob;
	long dypFrm;
	long dypTop;
	RC rc;
	RC rcAbs;
	RC rcRel;

	/* Still need to adjust if cthum == cfrm to hide scroll arrows */
	if (cthum > cfrm)
		return;

	/* For the font popup, the GOBs that are interesting for adjusting the
		entire browser are actually the parents of the thumbnail frames */
	pgob = vapp.Pkwa()->PgobFromHid(_kidFrmFirst);
	AssertPo(pgob, 0);
	pgob = pgob->PgobPar();
	AssertPo(pgob, 0);
	pgob->GetRc(&rc, cooParent);
	dypFrm = rc.Dyp();
	dypTop = rc.ypTop;

	GetPos(&rcAbs, &rcRel);
	rcAbs.ypBottom = rcAbs.ypTop + (dypTop + LwMul(cthum, dypFrm) + dypTop);
	SetPos(&rcAbs, &rcRel);
}


/***************************************************************************
	Do selection idle processing.  Make sure not to change any selection
	states.
***************************************************************************/
bool MPFNT::FCmdSelIdle(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	MPFNT_PAR::FCmdSelIdle(pcmd);
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the MPFNT.
***************************************************************************/
void MPFNT::AssertValid(ulong grf)
{
	MPFNT_PAR::AssertValid(0);
}


/***************************************************************************
	Mark memory used by the MPFNT
***************************************************************************/
void MPFNT::MarkMem(void)
{
	AssertThis(0);
	MPFNT_PAR::MarkMem();
}
#endif //DEBUG
