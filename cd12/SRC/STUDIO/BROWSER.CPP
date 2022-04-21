/***************************************************************************

  browser.cpp

  Author: Sean Selitrennikoff

  Date: March, 1995

	Review Status: Reviewed

  This file contains the browser display code.
  Browsers (abbrev Brws) include display, list and text classes.

	Studio Independent Browsers:
	BASE --> CMH --> GOK	-->	BRWD  (Browser display class)
	BRWD --> BRWL  (Browser list class; chunky based)
	BRWD --> BRWT  (Browser text class)
	BRWD --> BRWL --> BRWN  (Browser named list class)

	Studio Dependent Browsers:
	BRWD --> BRWR  (Roll call class)
	BRWD --> BRWT --> BRWA  (Browser action class)
	BRWD --> BRWL --> BRWP	(Browser prop/actor class)
	BRWD --> BRWL --> BRWB	(Browser background class)
	BRWD --> BRWL --> BRWC	(Browser camera class)
	BRWD --> BRWL --> BRWN --> BRWM (Browser music class)
	BRWD --> BRWL --> BRWN --> BRWM --> BRWI (Browser import sound class)
		
	Note: An "frm" refers to the displayed frames on any page.
	A "thum" is a generic Browser Thumbnail, which may be a
	chid, cno, cnoPar, gob, stn, etc.	A browser will display,
	over multiple pages, as many browser entities as there are
	thum's.

	This file contains the browser display code.

	To add additional browsers, create a derived class of
	the BRWD, BRWL or BRWT classes.
		
	If a browser is to be chunky file based, the BRWL class can be used.
	It includes GOKD chunks which are grandchildren of _ckiRoot (cnoNil
	implies wildcarding) and children of _ctgContent  - from .thd files
	entered in the registry of this product.
	The BRWL class allows the option of displaying either all the thumbnails
	of a particular ctg across registry specified directories (eg, scenes,
	actors)
		-or-
	of filling frames from GOKD thumbnails which are children of a single
	given chunk.

	Text class browsers (BRWT) create child TGOBs for each frame.

	Only cidBrowserCancel and cidBrowserOk exit the browser.
	cidBrowserSelect is selection only, not application of the selection.

	On creation of a new browser,
	 pcmd->rglw[0] = kid of Browser (type)
	 pcmd->rglw[1] = kid first frame.  Thumb kid is this + kidBrowserThumbOffset
	 pcmd->rglw[2] = kid of first control
	 pcmd->rglw[3] = x,y offsets

	Upon exiting, some browser classes retain BRCN (or derived from BRCN)
	context information for optimization.

	The kid of a single frame may be overridden (eg, for project help) using
	the override prids.

***************************************************************************/

#include "soc.h"
#include "studio.h"

ASSERTNAME

RTCLASS(BRCN)
RTCLASS(BRCNL)
RTCLASS(BRWD)
RTCLASS(BRWL)
RTCLASS(BRWT)
RTCLASS(BRWA)
RTCLASS(BRWP)
RTCLASS(BRWB)
RTCLASS(BRWC)
RTCLASS(BRWN)
RTCLASS(BRWM)
RTCLASS(BRWR)
RTCLASS(BRWI)
RTCLASS(BCL)
RTCLASS(BCLS)
RTCLASS(FNET)

BEGIN_CMD_MAP(BRWD, GOK)
	ON_CID_GEN(cidBrowserFwd, FCmdFwd, pvNil)
	ON_CID_GEN(cidBrowserBack, FCmdBack, pvNil)
	ON_CID_GEN(cidBrowserCancel, FCmdCancel, pvNil)
	ON_CID_GEN(cidBrowserOk, FCmdOk, pvNil)
	ON_CID_GEN(cidBrowserSelect, FCmdSelect, pvNil)
	ON_CID_GEN(cidBrowserSelectThum, FCmdSelectThum, pvNil)
	ON_CID_GEN(cidPortfolioFile, FCmdFile, pvNil)
	ON_CID_GEN(cidBrowserChangeCel, FCmdChangeCel, pvNil)
	ON_CID_GEN(cidBrowserDel, FCmdDel, pvNil)
END_CMD_MAP_NIL()


/****************************************************
 *
 * Create a browser display object
 *
 * Returns:
 *	A pointer to the object, else pvNil.
 *
 ****************************************************/
PBRWD BRWD::PbrwdNew(PRCA prca, long kidPar, long kidGlass)
{
	AssertPo(prca, 0);

	PBRWD pbrwd;
	GCB gcb;

	if (!_FBuildGcb(&gcb, kidPar, kidGlass))
		return pvNil;

	if ((pbrwd = NewObj BRWD(&gcb)) == pvNil)
		return pvNil;

	// Initialize the gok
	if (!pbrwd->_FInitGok(prca, kidGlass))	
		{
		ReleasePpo(&pbrwd);
		return pvNil;
		}

	return pbrwd;
}

/****************************************************
 *
 * Build the GOB creation block
 *
 ****************************************************/
bool BRWD::_FBuildGcb(GCB *pgcb, long kidPar, long kidGlass)
{
	AssertVarMem(pgcb);

	PGOB pgobPar;
	RC rcRel;

	pgobPar = vapp.Pkwa()->PgobFromHid(kidPar);
	if (pvNil == pgobPar)
		{
		TrashVar(pgcb);
		return fFalse;
		}

#ifdef DEBUG
	Assert(pgobPar->FIs(kclsGOK), "Parent isn't a GOK");
	{
	PGOB pgob = vapp.Pkwa()->PgobFromHid(kidGlass);

	Assert(pgob == pvNil, "GOK already exists with given ID");
	}
#endif // DEBUG

	rcRel.Set(krelZero, krelZero, krelOne, krelOne);
	pgcb->Set(kidGlass, pgobPar, fgobNil, kginDefault, pvNil, &rcRel);
	return fTrue;
}

/****************************************************
 *
 * Initialize the Gok :
 * Do everything PgokNew would have done but didn't
 *
 ****************************************************/
bool BRWD::_FInitGok(PRCA prca, long kid)
{
	AssertBaseThis(0);
	AssertPo(prca, 0);

	if (!BRWD_PAR::_FInit(vapp.Pkwa(), kid, prca))
		return fFalse;
	if (!_FEnterState(ksnoInit))
		return fFalse;
	return fTrue;
}


/****************************************************
 *
 * Initialize a new browser object
 * Init knows how to either initialize or reinitialize
 * itself on reinstantiation of a browser.
 *
 * NOTE:  This does not display - see FDraw()
 *
 * On input,
 * ithumSelect is the thumbnail to be hilited
 * ithumDisplay is a thumbnail to be on the first
 *		page displayed
 * pcmd->rglw[0] = kid of Browser (type)
 * pcmd->rglw[1] = kid first frame.  Thumb kid is
 *	   this + kidBrowserThumbOffset
 * pcmd->rglw[2] = kid of first control
 * pcmd->rglw[3] = x,y offsets
 *
 ****************************************************/
void BRWD::Init(PCMD pcmd, long ithumSelect, long ithumDisplay, PSTDIO pstdio,
	bool fWrapScroll, long cthumScroll)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	// Split the initialization into two parts. The first part initializes the
	// state variables and can be done before the number of thumbnails on the
	// browser is known. The second part of the initialization relates to
	// browser display and requires the number of thumbnails to be known.

	_InitStateVars(pcmd, pstdio, fWrapScroll, cthumScroll);
	_InitFromData(pcmd, ithumSelect, ithumDisplay);
}

/****************************************************
 *
 * Initialize state variables for a new browser object.
 * A call will be made later to InitFromData to finish
 * the browser initialization based on thumbnail specific
 * information.
 *
 * On input,
 * pcmd->rglw[0] = kid of Browser (type)
 * pcmd->rglw[1] = kid first frame.  Thumb kid is
 *	   this + kidBrowserThumbOffset
 * pcmd->rglw[2] = kid of first control
 * pcmd->rglw[3] = x,y offsets
 *
 ****************************************************/
void BRWD::_InitStateVars(PCMD pcmd, PSTDIO pstdio,
	bool fWrapScroll, long cthumScroll)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	// Save parameters for this browser
	_pstdio = pstdio;
	_kidFrmFirst = pcmd->rglw[1];
	_kidControlFirst = pcmd->rglw[2];
	_dxpFrmOffset = SwHigh(pcmd->rglw[3]);
	_dypFrmOffset = SwLow(pcmd->rglw[3]);
	_cthumScroll = cthumScroll;
	_fWrapScroll = fWrapScroll;
	_fNoRepositionSel = fFalse;
}

/****************************************************
 *
 * Complete the initialization of  a new browser object.
 * The number of thumbnails for this browser has
 * already been determined. This call follows a
 * call to InitStateVars earlier.
 *
 * On input,
 * ithumSelect is the thumbnail to be hilited
 * ithumDisplay is a thumbnail to be on the first
 *		page displayed
 *
 ****************************************************/
void BRWD::_InitFromData(PCMD pcmd, long ithumSelect, long ithumDisplay)
{
	AssertThis(0);
	long cthum;

	// Context carryover
	if (_pbrcn != pvNil)
		{
		_pbrcn->brwdid = pcmd->rglw[0];
		}

	// Initialize variables
	cthum = _Cthum();
	AssertIn(ithumDisplay, -1, cthum);
	AssertIn(ithumSelect, -1, cthum);
	_ithumSelect = ithumSelect;
	_cfrmPageCur = 0;
	_cfrm = _CfrmCalc();
	_SetScrollState();		// Set the state of the scroll arrows

	// Adjust the display ifrm to begin at a page boundary
	if (ithumDisplay != ivNil)
		_ithumPageFirst = ithumDisplay;
	_SetVarForOverride();
	_CalcIthumPageFirst();
}


/****************************************************
 *
 * _SetVarForOverride : Projects may override kids
 *
 ****************************************************/
void BRWD::_SetVarForOverride(void)
{
	AssertThis(0);
	long thumOverride = -1;
	long thumSidOverride = -1;

	// Projects need to be able to hard wire one of the gob id's
	if (vpappb->FGetProp(kpridBrwsOverrideThum, &thumOverride) &&
		vpappb->FGetProp(kpridBrwsOverrideSidThum, &thumSidOverride) &&
		vpappb->FGetProp(kpridBrwsOverrideKidThum, &_kidThumOverride))
		{
		if (thumOverride >= 0)
			{
			_ithumOverride = _IthumFromThum(thumOverride, thumSidOverride);
			
			// Make sure this thum is on the displayed page
			_ithumPageFirst = _ithumOverride;	
			}
		else
			{
			_kidThumOverride = -1;
			_ithumOverride = -1;		
			}
		}
}


/****************************************************
 *
 * _GetThumFromIthum
 *
 ****************************************************/

void BRWD::_GetThumFromIthum(long ithum, void *pThumSelect, long *psid)
{
	AssertThis(0);
	AssertVarMem((long *)pThumSelect);
	AssertVarMem(psid);

	*((long *)pThumSelect) = ithum;
	TrashVar(psid);
}

/****************************************************
 *
 * Count the number of frames possible per page
 *
 ****************************************************/
long BRWD::_CfrmCalc(void)
{
	AssertThis(0);

	PGOB pgob;
	long ifrm;

	for (ifrm = 0; ; ifrm++)
		{
		//
		// If there is no GOKD parent, there are no more thumbnail
		// slots on this page
		//
		pgob = vapp.Pkwa()->PgobFromHid(_kidFrmFirst + ifrm);
		if (pvNil == pgob)
			{
			return ifrm;
			}
		AssertPo(pgob, 0);
		}
	return 0;
}

/****************************************************
 *
 * Determine the first thumbnail to display on the page
 *
 ****************************************************/
void BRWD::_CalcIthumPageFirst(void)
{
	AssertThis(0);
	long cthum = _Cthum();

	// Place the selection as close to the top as _cthumScroll permits
	if (!_fNoRepositionSel || _cthumScroll == ivNil)
		{
		if (_cthumScroll != ivNil)
			_ithumPageFirst = _ithumPageFirst - (_ithumPageFirst % _cthumScroll);
		else
			_ithumPageFirst = _ithumPageFirst - (_ithumPageFirst % _cfrm);
		}

	// Verify that the viewed thumbnail is within range
	if (_ithumPageFirst >= cthum)
		{
		if (cthum == 0)
			_ithumPageFirst = 0;
		else if (_cthumScroll != ivNil)
			_ithumPageFirst = (cthum - 1) - ((cthum - 1) % _cthumScroll);
		else
			_ithumPageFirst = (cthum - 1) - ((cthum - 1) % _cfrm);
		}

	// Display a full last page if not wrapping around
	if (!_fWrapScroll &&
		(_ithumPageFirst + _cfrm > cthum) &&
		_cthumScroll != ivNil)
		{
		while((_ithumPageFirst - _cthumScroll) + _cfrm >= cthum)
			_ithumPageFirst -= _cthumScroll;
		if (_ithumPageFirst	< 0)
			_ithumPageFirst = 0;
		}
}

/****************************************************
 *
 * Browser Display
 *
 ****************************************************/
bool BRWD::FDraw(void)
{
	AssertThis(0);

	PGOB pgobPar;
	GCB gcb;
	long ithum;
	long ifrm;
	long cthum = _Cthum();

	_CalcIthumPageFirst();

	// Begin a new page
	_cfrmPageCur = 0;

	for (ifrm = 0;  ifrm < _cfrm; ifrm++)
		{
		ithum = _ithumPageFirst + ifrm;
 		// Release the previouly attached thum from this frame
		_ReleaseThumFrame(ifrm);

		pgobPar = vapp.Pkwa()->PgobFromHid(_kidFrmFirst + ifrm);
 		if (pvNil == pgobPar)
			goto LContinue;
 		Assert(pgobPar->FIs(kclsGOK), "Invalid class");

		if (ithum >= cthum)
			{
			// Render invisible
			((PGOK)pgobPar)->FChangeState(kstBrowserInvisible);	// Ignore error
			_FClearHelp(ifrm); 	// Clear rollover help
			continue;
			}

		// Get next gob from list
		if (!_FSetThumFrame(ithum, pgobPar))
			goto LContinue;

		if(ithum == _ithumSelect)
			{
			((PGOK)pgobPar)->FChangeState(kstBrowserSelected);
			}
		else
			{
			((PGOK)pgobPar)->FChangeState(kstBrowserEnabled);
			}
LContinue:
		if (ithum < cthum)
			_cfrmPageCur++;
		}

	
	// Update page number
	if (pvNil != _ptgobPage)
		{
		long pagen;
		long cpage;
		STN stn;
		STN stnT;
		pagen = (_cfrm == 0) ? 0 : (_ithumPageFirst / _cfrm);
		cpage = ((cthum - 1) / _cfrm) + 1;
		_pstdio->GetStnMisc(idsBrowserPage, &stnT);
		stn.FFormat(&stnT, pagen + 1, cpage);
		_ptgobPage->SetText(&stn);
		}	

	return fTrue;
}


/****************************************************
 *
 * Find a unique hid for the current frame
 *
 ****************************************************/
long BRWD::_KidThumFromIfrm(long ifrm)
{
	AssertBaseThis(0);
	long kidThum;

	if (_ithumPageFirst + ifrm == _ithumOverride)
		kidThum = _kidThumOverride;
	else
		kidThum = GOB::HidUnique();

	return kidThum;
}


/****************************************************
 *
 * Compute the pgob of the parent for frame ifrm
 *
 ****************************************************/
PGOB BRWD::_PgobFromIfrm(long ifrm)
{
	AssertBaseThis(0);
	PGOB pgob;
	pgob = vapp.Pkwa()->PgobFromHid(_kidFrmFirst + ifrm);
	if (pvNil == pgob)
		return pvNil;
	return 	pgob->PgobLastChild();
}


/****************************************************
 *
 * Set the state of the scroll arrows for the Display
 *
 ****************************************************/
void BRWD::_SetScrollState(void)
{
	AssertThis(0);

	PGOB pgob;
	long st = (_Cthum() <= _cfrm) ? kstBrowserInvisible : kstBrowserEnabled;
 	
	pgob = vapp.Pkwa()->PgobFromHid(_kidControlFirst);
	if (pvNil != pgob)
		{
		Assert(pgob->FIs(kclsGOK), "Invalid class");
		if (!((PGOK)pgob)->FChangeState(st))
			Warn("Failed to change state Page Fwd button");
		}

	pgob = vapp.Pkwa()->PgobFromHid(_kidControlFirst + 1);
	if (pvNil != pgob)
		{
		Assert(pgob->FIs(kclsGOK), "Invalid class");
		if (!((PGOK)pgob)->FChangeState(st))
			Warn("Failed to change state Page Back button");
		}
	
}


/****************************************************
 *
 * Browser Command Handler : Browser Forward
 *
 ****************************************************/
bool BRWD::FCmdFwd(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	long cthumAdd;

	// Default _cthumScroll -> page scrolling
	if (ivNil == _cthumScroll)
		cthumAdd = _cfrm;
	else
		cthumAdd = _cthumScroll;

	// If either wrapping or there is more to see, increment first thumbnail index
	if (_fWrapScroll || (_ithumPageFirst + _cfrm < _Cthum()))
		_ithumPageFirst += cthumAdd;
	
	if (_ithumPageFirst >= _Cthum())
		{
		_ithumPageFirst = 0;
		}

	FDraw();	// Ignore failure

	return fTrue;
}

/****************************************************
 *
 * Browser Command Handler :
 * Set viewing page based on thumSelect == rglw[0]
 * and sid == rglw[1]
 * rglw[2] -> Hilite
 * rglw[3] -> Update Lists
 *
 ****************************************************/
bool BRWD::FCmdSelectThum(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (pcmd->rglw[3] != 0)
		{
		if (!_FUpdateLists())
			return fTrue;
		}

	if (pcmd->rglw[2] > 0)
		_ithumSelect = _IthumFromThum(pcmd->rglw[0], pcmd->rglw[1]);
	else
		_ithumSelect = -1;
	_ithumPageFirst = _ithumSelect - (_ithumSelect % _cfrm);

	_SetScrollState();		// Set the state of the scroll arrows
	FDraw();	// Ignore failure
	return fTrue;
}


/****************************************************
 *
 * Browser Command Handler : Browser Back
 * Scroll back one page in the browser
 *
 ****************************************************/
bool BRWD::FCmdBack(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (_ithumPageFirst == 0)
		{
		if (_fWrapScroll)
			{
			// Wrap back to the last page
			_ithumPageFirst = _Cthum() - (_Cthum() % _cfrm);
			}
		}
	else
		{
		// Normal non-wrap page scroll back
		if (ivNil == _cthumScroll)
			_ithumPageFirst -= _cfrmPageCur;
		else
			_ithumPageFirst -= _cthumScroll;

		}

	// FDraw updates _cfrmPageCur
	FDraw();		// Ignore failure
	return fTrue;
}

/****************************************************
 *
 * Browser Command Handler : Browser Frame Selected
 * (Thumbnail clicked)
 *
 * The script is expected to make the BrowserOK call
 * FCmdSelect does not exit
 *
 * pcmd->rglw[0] is browser id of thumb
 *
 ****************************************************/
bool BRWD::FCmdSelect(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	long ifrmSelect = pcmd->rglw[0] - _kidFrmFirst;

	_UnhiliteCurFrm();
	_ithumSelect = _ithumPageFirst + ifrmSelect;
	_FHiliteFrm(ifrmSelect);

	// Handle any derived class special actions (eg previews)
	_ProcessSelection();
	return fTrue;
}


/****************************************************
 *
 * Create Tgob's for any of the text based browsers
 * These will be destroyed when the browser exits
 * NOTE:: FCreateAllTgob() requires previous initialization
 * of the override parameters from BRWD::Init()
 *
 ****************************************************/	
bool BRWD::FCreateAllTgob(void)
{
	AssertThis(0);

	long ifrm;
	long hid;
	PTGOB ptgob;
	RC rcAbs;
	RC rcRel;

	for (ifrm = 0; ifrm < _cfrm; ifrm++)
			{
		if (_ithumPageFirst + ifrm == _ithumOverride)
			hid = _kidThumOverride;
		else
			hid = hidNil;
		
		ptgob = TGOB::PtgobCreate(_kidFrmFirst + ifrm, _idsFont, tavTop, hid);
		if (pvNil == ptgob)
			return fFalse;
		ptgob->SetAlign(tahLeft);

		ptgob->GetPos(&rcAbs, &rcRel);
		rcAbs.Inset(_dxpFrmOffset, _dypFrmOffset);
		ptgob->SetPos(&rcAbs, &rcRel);
		}

	return fTrue;
}


/****************************************************
 *
 * Hilite frame
 *
 ****************************************************/
bool BRWD::_FHiliteFrm(long ifrmSelect)
{
	AssertThis(0);
	AssertIn(ifrmSelect, 0, _cfrm);
	PGOB pgob;

	// Hilite currently selected frame
	AssertIn(ifrmSelect, 0, _cfrmPageCur);
	pgob = vapp.Pkwa()->PgobFromHid(_kidFrmFirst + ifrmSelect);
	if (pvNil == pgob)
		return fFalse;
		
	Assert(pgob->FIs(kclsGOK), "Invalid class");
	if (!((PGOK)pgob)->FChangeState(kstBrowserSelected))
		{
		_ithumSelect = ivNil;
		return fFalse;
		}
	return fTrue;
}

/****************************************************
 *
 * Unhilite currently selected Frame
 *
 ****************************************************/
void BRWD::_UnhiliteCurFrm(void)
{
	AssertThis(0);
	PGOB pgob;
	long ifrmSelectOld = _ithumSelect - _ithumPageFirst;

	// Unhilite currently selected frame
	if ((_ithumPageFirst <= _ithumSelect) && (_ithumPageFirst + _cfrmPageCur > _ithumSelect))
		{
		pgob = vapp.Pkwa()->PgobFromHid(_kidFrmFirst + ifrmSelectOld);
		if (pvNil == pgob)
			return;
		Assert(pgob->FIs(kclsGOK), "Invalid class");
		((PGOK)pgob)->FChangeState(kstBrowserEnabled);		// Ignore failure
		}
}

/****************************************************
 *
 * Browser Command Handler : Browser Cancel
 * Exit without applying selection
 *
 ****************************************************/
bool BRWD::FCmdCancel(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	vpsndm->StopAll();
	Release();			// OK/Cancel common cleanup

	return fTrue;
}


/****************************************************
 *
 * Browser cleanup for OK & Cancel
 *
 ****************************************************/
void BRWD::Release(void)
{
	_CacheContext();

	// Minimize the cache size on low mem machines
	_SetCbPcrmMin();
	BRWD_PAR::Release();
}		

/****************************************************
 *
 * Browser Command Handler : Browser Ok
 * Apply selection & exit
 *
 ****************************************************/
bool BRWD::FCmdOk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	
	long sid;
	long thumSelect;

	_pstdio->SetDisplayCast(fFalse);

	if (ivNil != _ithumSelect)
		{
		// Get Selection from virtual function
		_GetThumFromIthum(_ithumSelect, &thumSelect, &sid);

		// Apply the selection (could take awhile)
		vapp.BeginLongOp();
		_ApplySelection(thumSelect, sid);
		vapp.EndLongOp();
		}

	// Cleanup & Dismiss browser
	Release();

	return fTrue;
}


/****************************************************
 *
 * BRWD _CacheContext
 *
 ****************************************************/
void BRWD::_CacheContext(void)
{
	if (_pbrcn != pvNil)
		_pbrcn->ithumPageFirst = _ithumPageFirst;
}


/****************************************************

   Browser Lists
   Derived from the BRWD display class

 ****************************************************/

/****************************************************
 *
 * Create a browser list object
 *
 * Returns:
 *  A pointer to the view, else pvNil.
 *
 ****************************************************/
PBRWL BRWL::PbrwlNew(PRCA prca, long kidPar, long kidGlass)
{
	AssertPo(prca, 0);

	PBRWL pbrwl;
	PGOK pgok;
	GCB gcb;

	if (!_FBuildGcb(&gcb, kidPar, kidGlass))
		return pvNil;

	if ((pbrwl = NewObj BRWL(&gcb)) == pvNil)
		return pvNil;

	// Initialize the gok
	if (!pbrwl->_FInitGok(prca, kidGlass))	
		{
		ReleasePpo(&pbrwl);
		return pvNil;
		}

	//
	// Stop the studio action button animation while
	// any browser is up.
	//
	pgok = (PGOK)vapp.Pkwa()->PgobFromHid(kidActorsActionBrowser);

	if ((pgok != pvNil) && pgok->FIs(kclsGOK))
		{
		Assert(pgok->FIs(kclsGOK), "Invalid class");
		pgok->FChangeState(kstFreeze);
		}					

	return pbrwl;
}
							

/****************************************************
 *
 * Initialize a browser list object
 *
 * Returns:
 *  A pointer to the view, else pvNil.
 *
 * On Input,
 *	bws is the browser selection type
 *	thumSelect is the thumbnail to be hilited
 *
 ****************************************************/
bool BRWL::FInit(PCMD pcmd, BWS bws, long thumSelect, long sidSelect,
	CKI ckiRoot, CTG ctgContent, PSTDIO pstdio, PBRCNL pbrcnl,
	bool fWrapScroll, long cthumScroll)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	long ithumDisplay;
	long ccrf = 1;
	long ithumSelect;
	bool fBuildGl;

	_bws = bws;

   	_kidFrmFirst = pcmd->rglw[1];
	_cfrm = _CfrmCalc();

	// Initialize the state variables for the browser. This is required for
	// the creation of the tgobs beneath the call to _FCreateBuildThd below.
	// The call must be later followed by a call to BRWD::InitFromData().
	_InitStateVars(pcmd, pstdio, fWrapScroll, cthumScroll);

	if (pvNil == pbrcnl ||
		pbrcnl->brwdid == 0 ||
		pbrcnl->ckiRoot.cno != ckiRoot.cno)
		{
		fBuildGl = fTrue;

		// Cache the GOKD's by first creating a
		// chunky resource manager
		Assert(pvNil == _pcrm, "Logic error releasing pcrm");
		_pcrm = CRM::PcrmNew(ccrf);
		if (pvNil == _pcrm)
			goto LDismiss;
	
		if (!_FInitNew(pcmd, bws, thumSelect, ckiRoot, ctgContent))
			{
			goto LDismiss;
			}
		_cthumCD = _pglthd->IvMac();
		}
	else
		{
		// Reinitialize based on pbrcnl
		fBuildGl = fFalse;

		_cthumCD = pbrcnl->cthumCD;
		_pglthd = pbrcnl->pglthd;
		Assert(_cthumCD == _pglthd->IvMac(), "Logic error");
		_pglthd->AddRef();

		_pgst = pbrcnl->pgst;
		if (_pgst != pvNil)
			_pgst->AddRef();

		_pcrm = pbrcnl->pcrm;
		_pcrm->AddRef();
		for (long icrf = 0; icrf < _pcrm->Ccrf(); icrf++)
			_pcrm->PcrfGet(icrf)->SetCbMax(kcbMaxCrm);

		// First thumbnail on display page is inherited
		_ithumPageFirst = pbrcnl->ithumPageFirst;

		// This will make sure that we build any TGOBs
		if (!_FCreateBuildThd(ckiRoot, ctgContent, fBuildGl))
			goto LDismiss;
		}

	// Context carryover
	Assert(_pbrcn == pvNil, "Lost BRCN");
	_pbrcn = pbrcnl;
	if (pvNil != pbrcnl)
		{
		_pbrcn->AddRef();
		pbrcnl->ckiRoot = ckiRoot;
		if (fBuildGl)
			{
			// Rebuilding the GL ->
			// Remove old context
			// Save new context later

			/* Release */
			ReleasePpo(&pbrcnl->pglthd);
			ReleasePpo(&pbrcnl->pgst);
			ReleasePpo(&pbrcnl->pcrm);
			}
		}

	// Virtual function - used, eg., by sound browser to include user sounds
	_FUpdateLists();

	ithumSelect = _IthumFromThum(thumSelect, sidSelect);
  	if (fBuildGl)
		{
		// Display the selection if one exists
		if (ithumSelect != ivNil)
			ithumDisplay = ithumSelect;
		else
			ithumDisplay = _IthumFromThum(_thumDefault, _sidDefault);
		}
	else
		{
		// Display last page shown
		ithumDisplay = ivNil;
		}

	// Now initialize the display part of the browser.
	_InitFromData(pcmd, ithumSelect, ithumDisplay);

	vapp.DisableAccel();
	_fEnableAccel = fTrue;

	return fTrue;

LDismiss:
	return fFalse;
}


/****************************************************
 *
 * Initialization specific to the first time a list
 * browser is invoked
 *
 ****************************************************/
bool BRWL::_FInitNew(PCMD pcmd, BWS bws,
	long thumSelect, CKI ckiRoot, CTG ctgContent)
{
	AssertThis(0);

	// Selection type
	_bws = bws;

	// Set the display default
	_sidDefault = ((APP *)vpappb)->SidProduct();
	if (!vpappb->FGetProp(kpridBrwsDefaultThum, &_thumDefault))
		{
		Warn("couldn't get property kpridBrwsDefaultThum");
		_thumDefault = 0;
		}


	// Build the Thd
	if (!_FCreateBuildThd(ckiRoot, ctgContent))
		{
		return fFalse;
		}

	return fTrue;
}


/****************************************************
 *
 * Initialization
 * Called each time a browser is either first
 * instantiated or reinvoked.
 *
 * Each browser will have its own cache of gokd's.
 * The cache will vanish when the browser goes
 * away.
 *
 ****************************************************/
bool BRWL::_FCreateBuildThd(CKI ckiRoot, CTG ctgContent, bool fBuildGl)
{
	AssertThis(0);

	_fSinglePar = (ckiRoot.cno != cnoNil);
	_ckiRoot = ckiRoot;
	_ctgContent = ctgContent;

	if (fBuildGl)
		{
		//
		// Create the gl's
		//
		if (pvNil == (_pglthd = GL::PglNew(size(THD), kglthdGrow)))
			return fFalse;			
		_pglthd->SetMinGrow(kglthdGrow);
		}

	if (!_FGetContent(_pcrm, &ckiRoot, ctgContent, fBuildGl))
		goto LFail;

	if (_Cthum() == 0)
		{
		PushErc(ercSocNoThumbnails);
		return fFalse;	 	
		}

	if (fBuildGl)
		_SortThd();

	return fTrue;
LFail:
	return fFalse;
}


/****************************************************
 *
 * BRWL _FGetContent : Enum files & build the THD
 *
 ****************************************************/
bool BRWL::_FGetContent(PCRM pcrm, CKI *pcki, CTG ctg, bool fBuildGl)
{
	AssertThis(0);

	bool fRet = fFalse;
	PBCL pbcl = pvNil;

	if (!fBuildGl)
		return fTrue;

	//
	// Enumerate the files & build the THD
	//
	Assert(ctg != cnoNil || pcki->ctg == ctgNil, "Invalid browser call");

	pbcl = BCL::PbclNew(pcrm, pcki, ctg, _pglthd);
	if (pbcl == pvNil)
		goto LFail;

	/* We passed the pglthd and pgst in, so no need to get them back before
		releasing the BCLS */
	Assert(_pglthd->CactRef() > 1, "GL of THDs will be lost!");

	fRet = fTrue;
LFail:
	ReleasePpo(&pbcl);
	return fRet;
}


/****************************************************
 *
 * Get list selection from the ithum
 * Virtual function
 *									
 ****************************************************/
void BRWL::_GetThumFromIthum(long ithum, void *pvthumSelect, long *psid)
{
	AssertThis(0);
	AssertIn(ithum, 0, _Cthum());
	AssertVarMem(psid);

	THD thd;						
	
	if (_bws == kbwsIndex)
		{
		*psid = thd.tag.sid;
		*((long *)pvthumSelect) = ithum;
		return;
		}

	_pglthd->Get(ithum, &thd);
	*psid = thd.tag.sid;

	switch(_bws)
		{
	case kbwsChid:
		Assert(thd.chid != chidNil, "Bogus sort order for THD list");
		*((CHID *)pvthumSelect) = thd.chid;
		break;
	case kbwsCnoRoot:
		*((CNO *)pvthumSelect) = thd.tag.cno;
		break;
	default:
		Bug("Unhandled bws case");
		}

	return;
}


/****************************************************
 *
 * BRWL _CacheContext for reentry into the browser
 *
 ****************************************************/
void BRWL::_CacheContext(void)
{
	if (_pbrcn != pvNil)
		{
		PBRCNL pbrcnl = (PBRCNL)_pbrcn;

		Assert(pbrcnl->FIs(kclsBRCNL), "Bad context buffer");

		BRWL_PAR::_CacheContext();

		if (pbrcnl->pglthd == pvNil)
			{
			Assert(pbrcnl->pgst == pvNil, "Inconsistent state");
			Assert(pbrcnl->pcrm == pvNil, "Inconsistent state");

			/* Copy */
			pbrcnl->pglthd = _pglthd;
			pbrcnl->pgst = _pgst;
			pbrcnl->pcrm = _pcrm;

			/* AddRef */
			pbrcnl->pglthd->AddRef();
			if (pbrcnl->pgst != pvNil)
				pbrcnl->pgst->AddRef();
			pbrcnl->pcrm->AddRef();
			}
		else
			{
			Assert(pbrcnl->pglthd == _pglthd, "Inconsistent state");
			Assert(pbrcnl->pgst == _pgst, "Inconsistent state");
			Assert(pbrcnl->pcrm == _pcrm, "Inconsistent state");
			}

		/* Munge */
		/* Should never fail, since we should never be growing */
		AssertDo(pbrcnl->pglthd->FSetIvMac(_cthumCD),
			"Tried to grow pglthd (bad) and failed (also bad)");
		if (pbrcnl->pgst != pvNil)
			{
			long istn;

			istn = pbrcnl->pgst->IstnMac();
			while (istn > _cthumCD)
				pbrcnl->pgst->Delete(--istn);
			}
		for (long icrf = 0; icrf < _pcrm->Ccrf(); icrf++)
			_pcrm->PcrfGet(icrf)->SetCbMax(kcbMaxCrm);

		pbrcnl->cthumCD = _cthumCD;
		}
}


/****************************************************
 *
 * Sort the Thd gl by _bws
 * Uses bubble sort
 *
 ****************************************************/
void BRWL::_SortThd(void)
{
	AssertThis(0);

	long ithd;
	long jthd;
	long *plwJ, *plwI;
	THD thdi;
	THD thdj;
	long sid;
	long jthdMin = 0;
	bool fSortBySid;

	if (_bws == kbwsIndex)
		return;

	switch (_bws)
		{
	case kbwsChid:
		Assert(size(thdj.chidThum) == size(long), "Bad pointer cast");
		plwJ = (long *)&thdj.chidThum;
		plwI = (long *)&thdi.chidThum;
		fSortBySid = fFalse;
		break;
	case kbwsCnoRoot:
		Assert(size(thdj.tag.cno) == size(long), "Bad pointer cast");
		plwJ = (long *)&thdj.tag.cno;
		plwI = (long *)&thdi.tag.cno;
		fSortBySid = fTrue;
		break;
	default:
		Bug("Illegal _bws value");
		}

	if (fSortBySid && _pglthd->IvMac() > 0)
		{
		_pglthd->Get(0, &thdi);
		sid = thdi.tag.sid;
		}
	for (ithd = 1; ithd < _pglthd->IvMac(); ithd++)
		{
		_pglthd->Get(ithd, &thdi);
		if (fSortBySid && thdi.tag.sid != sid)
			{
			sid = thdi.tag.sid;
			jthdMin = ithd;
			}

		for (jthd = jthdMin; jthd < ithd; jthd++)
			{
			_pglthd->Get(jthd, &thdj);
			if (*plwJ < *plwI)
				continue;
			
			// Allow products to share identical content
			// but only view it once
			if (*plwJ == *plwI)
				{
				long ithdT;
				THD thdT;
				if (pvNil != _pgst && thdi.ithd < _pgst->IvMac())
					{
					Assert(thdi.ithd == ithd, "Logic error cleaning pgst");
					_pgst->Delete(thdi.ithd);
					}

				for (ithdT = ithd + 1; ithdT < _pglthd->IvMac(); ithdT++)
					{
					_pglthd->Get(ithdT, &thdT);
					thdT.ithd--;
					_pglthd->Put(ithdT, &thdT);
					}
#ifdef DEBUG
				// Note: Files for the current product was enumerated first.
				// This determines precedence.
				if (thdj.tag.sid == _sidDefault || thdi.tag.sid == _sidDefault)
					Assert(thdj.tag.sid == _sidDefault, "Browser deleting the wrong duplicate");
#endif //DEBUG
				_pglthd->Delete(ithd);
				ithd--;
				break;
				}	
			// Switch places
			_pglthd->Put(ithd, &thdj);
			_pglthd->Put(jthd, &thdi);
			_pglthd->Get(ithd, &thdi);
			}
		}
}


/****************************************************
 *
 * Get the index to the Thi for the given selection
 * Note: The selection thumSelect is a cno, chid, etc,
 * not an index into either pgl.
 *
 ****************************************************/
long BRWL::_IthumFromThum(long thumSelect, long sidSelect)
{
	AssertThis(0);		   

	long *plw;
	THD thd;

	if (thumSelect == ivNil)
		return ivNil;

	switch (_bws)
		{
	case kbwsIndex:
		return thumSelect;
		
	case kbwsChid:
		Assert(size(thd.chid) == size(long), "Bad pointer cast");
		plw = (long *)&thd.chid;
		break;

	case kbwsCnoRoot:
		Assert(size(thd.tag.cno) == size(long), "Bad pointer cast");
		plw = (long *)&thd.tag.cno;
		break;

	default:
		Assert(0,"Invalid _bws");
		}

	for (long ithd = 0; ithd < _pglthd->IvMac(); ithd++)
		{
		_pglthd->Get(ithd, &thd);
		if ((*plw == thumSelect) &&
				(sidSelect == ksidInvalid || thd.tag.sid == sidSelect))
			{
			return ithd;
			}
		}

	Warn("Selection not available");
	return ivNil;
}


/****************************************************
 *
 * Sets the ith Gob as a child of the current frame
 * Advance the gob (thumbnail) index
 *
 ****************************************************/	
bool BRWL::_FSetThumFrame(long ithd, PGOB pgobPar)
{
	AssertThis(0);
	AssertPo(pgobPar, 0);

	THD thd;
	PGOK pgok;
	RC rcAbs;
	RC rcRel;
	long kidThum;

	// Associate the gob with the current frame
	_pglthd->Get(ithd, &thd);
	
	kidThum = _KidThumFromIfrm(_cfrmPageCur);
	pgok = vapp.Pkwa()->PgokNew(pgobPar, kidThum, thd.cno, _pcrm);
	if (pvNil == pgok)
		return fFalse;

	((PGOB)pgok)->GetPos(&rcAbs, &rcRel);
	rcAbs.Offset(_dxpFrmOffset, _dypFrmOffset);
	((PGOB)pgok)->SetPos(&rcAbs, &rcRel);

	return fTrue;
}


/****************************************************
 *
 * Release previous thum from frame ifrm
 * Assumes gob based.
 *
 ****************************************************/	
void BRWL::_ReleaseThumFrame(long ifrm)
{
	AssertThis(0);
	PGOB pgob;

	// Release previous gob associated with the current frame
    pgob = _PgobFromIfrm(ifrm);
	if (pvNil != pgob)
		{
		ReleasePpo(&pgob);
		}
}


/****************************************************
 *
 * BCL class routines
 *
 ****************************************************/	
PBCL BCL::PbclNew(PCRM pcrm, CKI *pckiRoot, CTG ctgContent, PGL pglthd,
	bool fOnlineOnly)
{
	PBCL pbcl;

	pbcl = NewObj BCL;
	if (pbcl == pvNil)
		return pvNil;

	if (!pbcl->_FInit(pcrm, pckiRoot, ctgContent, pglthd))
		ReleasePpo(&pbcl);

	return pbcl;
}


bool BCLS::_FInit(PCRM pcrm, CKI *pckiRoot, CTG ctgContent, PGST pgst, PGL pglthd)
{
	AssertNilOrPo(pgst, 0);

	if (pgst == pvNil)
		{
		if ((pgst = GST::PgstNew()) == pvNil)
			goto LFail;
		}
	else
		pgst->AddRef();
	_pgst = pgst;

	if (!BCLS_PAR::_FInit(pcrm, pckiRoot, ctgContent, pglthd))
		goto LFail;

	return fTrue;
LFail:
	return fFalse;
}


bool BCL::_FInit(PCRM pcrm, CKI *pckiRoot, CTG ctgContent, PGL pglthd)
{
	AssertNilOrPo(pcrm, 0);
	Assert(pckiRoot->ctg != ctgNil, "Bad CKI");
	AssertNilOrPo(pglthd, 0);

	if (pglthd == pvNil)
		{
		if ((pglthd = GL::PglNew(size(THD))) == pvNil)
			goto LFail;
		}
	else
		pglthd->AddRef();
	_pglthd = pglthd;

	_ctgRoot = pckiRoot->ctg;
	_cnoRoot = pckiRoot->cno;
	_ctgContent = ctgContent;
	_fDescend = _cnoRoot != cnoNil;
	Assert(!_fDescend || _ctgContent != ctgNil, "Bad initialization");

	if (!_FBuildThd(pcrm))
		goto LFail;

	return fTrue;
LFail:
	return fFalse;
}


PBCLS BCLS::PbclsNew(PCRM pcrm, CKI *pckiRoot, CTG ctgContent, PGL pglthd,
	PGST pgst, bool fOnlineOnly)
{
	PBCLS pbcls;

	pbcls = NewObj BCLS;
	if (pbcls == pvNil)
		goto LFail;

	if (!pbcls->_FInit(pcrm, pckiRoot, ctgContent, pgst, pglthd))
		ReleasePpo(&pbcls);

LFail:
	return pbcls;
}


/****************************************************
 *
 * Enumerate thumbnail files & create the THD
 * (Thumbnail descriptor gl)
 *
 * Note: In the case of actions & views, this is one
 * entry.
 * Input: fBuildGl is false if only the pcrm requires
 * initialization; otherwise the _pglthd is built.
 *
 * Sort based on _bws (browser selection flag)
 *
 ****************************************************/
bool BCL::_FBuildThd(PCRM pcrm)
{
	AssertThis(0);
	AssertNilOrPo(pcrm, 0);

	bool fRet = fTrue;
	PCFL pcfl;
	long sid;
	FNET fnet;
	FNI fniThd;

	if (!fnet.FInit())
		return fFalse;
	while(fnet.FNext(&fniThd, &sid) && fRet)
		{
		if (fniThd.Ftg() != kftgThumbDesc)
			continue;
		pcfl = CFL::PcflOpen(&fniThd, fcflNil);
		if (pvNil == pcfl)
			{
			// Error reported elsewhere
			continue;		
			}

		/* Don't use this file if it doesn't have what we're looking for */
		if (_fDescend ? !pcfl->FFind(_ctgRoot, _cnoRoot) :
				pcfl->CckiCtg(_ctgRoot) == 0)
			{
			goto LContinue;
			}

		// Add the file to the crm
		if (pcrm != pvNil && !pcrm->FAddCfl(pcfl, kcbMaxCrm))
			{
			// fatal error
			fRet = fFalse;
			goto LContinue;
			}
		
		if (!_FAddFileToThd(pcfl, sid))
			{
			// Error issued elsewhere
			fRet = fFalse;
			goto LContinue;
			}

LContinue:
		ReleasePpo(&pcfl);
		}

	return fRet;
}


/****************************************************
 *
 *  Add the chunks of file pcfl to the THD
 *
 *	Requires pre-definition of _ckiRoot, _ctgContent
 *
 *	Filtering is based on ctg, cno, but not chid
 *	A value of cnoNil => wild card search
 *
 ****************************************************/
bool BCL::_FAddFileToThd(PCFL pcfl, long sid)
{
	AssertThis(0);
	AssertPo(pcfl, 0);

	long ickiRoot;
	long cckiRoot;
	CKI ckiRoot;
	KID kidPar;
	long ikidPar;	
	long ckidPar;
	
	Assert(ctgNil != _ctgRoot, "Illegal call");

	if (!_fDescend)
		cckiRoot = pcfl->CckiCtg(_ctgRoot);
	else
		{
		ckiRoot.ctg = _ctgRoot;
		ckiRoot.cno = _cnoRoot;
		cckiRoot = 1;
		}

	// For each grandparent, define ckiRoot
	for (ickiRoot = 0; ickiRoot < cckiRoot; ickiRoot++)
		{
		if (!_fDescend)
			{
			// Get the cki of the grandparent
			if (!pcfl->FGetCkiCtg(_ctgRoot, ickiRoot, &ckiRoot))	
				return fFalse;
	
			//
			// If only one level of parent specified in the search
			// then add the gokd from this parent and continue
			//
			if (!_FAddGokdToThd(pcfl, sid, &ckiRoot))
				return fFalse;
			continue;
			}

		//
		// Drop down one more level
		//
		if (!pcfl->FFind(ckiRoot.ctg, ckiRoot.cno))
			continue;
	
		ckidPar = pcfl->Ckid(ckiRoot.ctg, ckiRoot.cno);

		for (ikidPar = 0; ikidPar < ckidPar; ikidPar++)
			{
			if (!pcfl->FGetKid(ckiRoot.ctg, ckiRoot.cno, ikidPar, &kidPar))
				return fFalse;

			if (kidPar.cki.ctg != _ctgContent)
				continue;												

			// On failure, continue to add other files
			// Error reported elsewhere
			_FAddGokdToThd(pcfl, sid, &kidPar);
			}
	  	}
	
	return fTrue;
}


bool BCL::_FAddGokdToThd(PCFL pcfl, long sid, CKI *pcki)
{
	KID kid;

	kid.cki = *pcki;
	kid.chid = chidNil;
	return _FAddGokdToThd(pcfl, sid, &kid);
}


/****************************************************
 *
 *  Add a single GOKD to the THD
 *	The GOKD is a child of ckiPar.
 *  cnoPar is read from the ckiPar chunk
 *
 ****************************************************/
bool BCL::_FAddGokdToThd(PCFL pcfl, long sid, KID *pkid)
{
	AssertThis(0);
	AssertPo(pcfl, 0);
	AssertVarMem(pkid);

	CKI cki = pkid->cki;
	KID kid;
	THD thd;
	BLCK blck;
	TFC tfc;

	// Read the Par chunk to find the cno of the CD content
	if (!pcfl->FFind(cki.ctg, cki.cno, &blck) ||
		!blck.FUnpackData())
		{
		goto LFail;
		}
	if (blck.Cb() != size(TFC))
		goto LFail;
	if (!blck.FReadRgb(&tfc, size(TFC), 0))
		goto LFail;
	if (kboCur != tfc.bo)
		SwapBytesBom(&tfc, kbomTfc);
	Assert(kboCur == tfc.bo, "bad TFC");
	if (!_fDescend)
		{
		thd.tag.cno = tfc.cno;
		thd.tag.ctg = tfc.ctg;
		}
	else
		{
		thd.tag.ctg = tfc.ctg;
		thd.chid = tfc.chid;
		}

	Assert(thd.grfontMask == tfc.grfontMask, "Font style browser broken");
	Assert(thd.grfont == tfc.grfont, "Font style browser broken");

	thd.tag.sid = sid;
	thd.chidThum = pkid->chid;
	if (pcfl->FGetKidChidCtg(cki.ctg, cki.cno, 0, kctgGokd, &kid))
		thd.cno = kid.cki.cno;
	else
		{
		// If there are no GOKD children, enter the reference to the named
		// parent chunk
		thd.cno = tfc.cno;
		}

	thd.ithd = _pglthd->IvMac();
	if (!_pglthd->FInsert(thd.ithd, &thd))
		goto LFail;

	return fTrue;
LFail:
	return fFalse;
}



bool BCLS::_FAddGokdToThd(PCFL pcfl, long sid, KID *pkid)
{
	AssertThis(0);
	AssertPo(pcfl, 0);
	AssertVarMem(pkid);

	if (!_FSetNameGst(pcfl, pkid->cki.ctg, pkid->cki.cno))
		goto LFail;

	if (!BCLS_PAR::_FAddGokdToThd(pcfl, sid, pkid))
		goto LFail;

	return fTrue;
LFail:
	return fFalse;
}


/****************************************************
 *
 * Save the name of the Par chunk in the Gst
 *
 ****************************************************/
bool BCLS::_FSetNameGst(PCFL pcfl, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertPo(pcfl, 0);
	STN stn;

	if (!pcfl->FGetName(ctg, cno, &stn))
		return fFalse;
	if (!_pgst->FAddStn(&stn))
		return fFalse;
	return fTrue;
}

/****************************************************
 *
 * Enumerate thumbnail files
 *
 * Initialization
 *
 ****************************************************/
bool FNET::FInit(void)
{
	AssertThis(0);

	FTG ftgThd = kftgThumbDesc;

	vapp.GetFniProduct(&_fniDirProduct); // look for THD files in the product FIRST
	_fniDir = _fniDirProduct;
	_fniDirMSK = _fniDirProduct;
	_fInitMSKDir = fTrue;

	if (!_fniDirMSK.FUpDir(pvNil, ffniMoveToDir))
		return fFalse;
 	if (!_fne.FInit(&_fniDir, &ftgThd, 1))
		return fFalse;
	_fInited = fTrue;
	return fTrue;
}


/****************************************************
 *
 * Enumerate thumbnail files
 *
 * Return the fni & (optionally) the sid
 * Note: _idir == 0 -> current product
 *
 ****************************************************/
bool FNET::FNext(FNI *pfni, long *psid)
{
	AssertThis(0);
	AssertPo(pfni, 0);
	AssertNilOrVarMem(psid);

	FTG ftgThd = kftgThumbDesc;
	FTG ftgDir = kftgDir;
	STN stnProduct;

	if (!_fInited)
		return fFalse;

	// Return files from currently enumerated directory
	// Note: This returns files from current product before 
	// initializing _fneDir
	if (_FNextFni(pfni, psid))
		return fTrue;

	if (_fInitMSKDir)
		{
		if (!_fneDir.FInit(&_fniDirMSK, &ftgDir, 1))
			return fFalse;
		_fInitMSKDir = fFalse;		
		}

	while (_fneDir.FNextFni(&_fniDir))
		{
		if (_fniDir.FSameDir(&_fniDirProduct)) 	// already enumerated
			continue;

		if (!_fne.FInit(&_fniDir, &ftgThd, 1))	// Initialize the file enumeration
			return fFalse;
		if(_FNextFni(pfni, psid))
			return fTrue;
		}

	return fFalse;						// all done		
}

/****************************************************
 *
 * Gets the file from the current FNET enumeration
 * This uses the current fne.
 *
 ****************************************************/
bool FNET::_FNextFni(FNI *pfni, long *psid)
{
	AssertThis(0);
	STN stnProduct;

	Assert(_fniDir.Ftg() == kftgDir, "Logic error");
	if (pvNil != psid)
		{
		if (!_fniDir.FUpDir(&stnProduct, ffniNil))
			return fFalse;
		if (!vptagm->FGetSid(&stnProduct, psid))		
			return fFalse;
		}

	return (_fne.FNextFni(pfni));
}


/****************************************************
 *
 * BRWN Initialization
 * -> BRWL Initialization plus tgob creation
 *
 ****************************************************/	
bool BRWN::FInit(PCMD pcmd, BWS bws, long thumSelect, long sidSelect,
	CKI ckiRoot, CTG ctgContent, PSTDIO pstdio, PBRCNL pbrcnl,
	bool fWrapScroll, long cthumScroll)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (!BRWN_PAR::FInit(pcmd, bws, thumSelect, sidSelect, ckiRoot, ctgContent,
		pstdio, pbrcnl, fWrapScroll, cthumScroll))
		{
		return fFalse;
		}	

	if (!FCreateAllTgob())
		return fFalse;
	return fTrue;
}

/****************************************************
 *
 * Build the thd
 *
 ****************************************************/	
bool BRWN::_FGetContent(PCRM pcrm, CKI *pcki, CTG ctg, bool fBuildGl)
{
	AssertThis(0);

	bool fRet = fFalse;
	PBCLS pbcls = pvNil;

	if (!fBuildGl)
		{
		return fTrue;
		}

	if (pvNil == (_pgst = GST::PgstNew(0)))
		goto LFail;

	//
	// Enumerate the files & build the THD
	//
	Assert(ctg != cnoNil || pcki->ctg == ctgNil, "Invalid browser call");

	pbcls = BCLS::PbclsNew(pcrm, pcki, ctg, _pglthd, _pgst);
	if (pbcls == pvNil)
		goto LFail;

	/* We passed the pglthd and pgst in, so no need to get them back before
		releasing the BCLS */
	Assert(_pglthd->CactRef() > 1, "GL of THDs will be lost!");
	Assert(_pgst->CactRef() > 1, "GST will be lost!");

	fRet = fTrue;
LFail:
	ReleasePpo(&pbcls);
	return fRet;
}


/****************************************************
 *
 * Removes the text from the unused tgob
 *
 ****************************************************/	
void BRWN::_ReleaseThumFrame(long ifrm)
{
	AssertThis(0);
	AssertIn(ifrm, 0, _cfrm);

	STN stn;
	PGOB pgob;

	pgob = _PgobFromIfrm(ifrm);
	if (pvNil != pgob)
		{
		stn.SetNil();
		((PTGOB)pgob)->SetText(&stn);
		}

	// The BRWN class retains the tgob while the browser is up
}

/****************************************************
 *
 * Sets the ith text item in the child of the
 * current frame
 *
 ****************************************************/	
bool BRWN::_FSetThumFrame(long ithd, PGOB pgobPar)
{
	AssertThis(0);
	AssertIn(ithd, 0, _pglthd->IvMac());
	AssertPo(pgobPar, 0);

	PTGOB ptgob;
	STN stnLabel;
	THD thd;

	_pglthd->Get(ithd, &thd);
	_pgst->GetStn(thd.ithd, &stnLabel);
	ptgob = (PTGOB)(pgobPar->PgobFirstChild());

	Assert(pvNil != ptgob, "No TGOB for the text");
	if (pvNil != ptgob)
		ptgob->SetText(&stnLabel);

	return fTrue;
}


/****************************************************
 *
 * Browser Command Handler : Browser Ok
 * Apply selection & exit
 *
 ****************************************************/
bool BRWN::FCmdOk(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (ivNil == _ithumSelect)
		{
		TAG tag;
		PMVU pmvu;
		pmvu = (PMVU)(_pstdio->Pmvie()->PddgGet(0));
		tag.sid = ksidInvalid;
		tag.pcrf = pvNil;
	 	pmvu->SetTagTool(&tag);	  	// No need to close tag with ksidInvalid
		}

	if (BRWD::FCmdOk(pcmd))
		{
		//
		// Stop any playing sounds in a sound browser.  We do
		// not stop playing sounds in non-sound browsers,
		// because the selection sound may be playing.
		//
		vpsndm->StopAll();
		return fTrue;	
		}

	return fFalse;
}

/****************************************************
 *
 * Create a BRoWser Music Sound object
 *
 ****************************************************/
PBRWM BRWM::PbrwmNew(PRCA prca, long kidGlass, long sty, PSTDIO pstdio)
{
	AssertPo(prca, 0);
	AssertPo(pstdio, 0);

	PBRWM pbrwm;
	GCB gcb;

	if (!_FBuildGcb(&gcb, kidBackground, kidGlass))
		return pvNil;

	if ((pbrwm = NewObj BRWM(&gcb)) == pvNil)
		return pvNil;

	// Initialize the gok
	if (!pbrwm->_FInitGok(prca, kidGlass))	
		{
		ReleasePpo(&pbrwm);
		return pvNil;
		}

	pbrwm->_sty = sty;
	pbrwm->_pstdio = pstdio;
	if (!pstdio->Pmvie()->FEnsureAutosave(&pbrwm->_pcrf))
		{
		ReleasePpo(&pbrwm);
		return pvNil;
		}

	pbrwm->_ptgobPage = TGOB::PtgobCreate(kidBrowserPageNum, idsBrwsPageFont, tavCenter);
	return pbrwm;
}

/****************************************************
 *
 * Add all of the sounds of the movie from _pcrf
 * to the current BRWL lists:
 *	pglthd, pgst
 *
 * Used at initialization time by both the BRWI and
 * the BRWM classes.
 *
 * It is assumed that initialization has created
 * (if empty) lists at this point.
 *
 ****************************************************/
bool BRWM::_FUpdateLists(void)
{
	AssertThis(0);

	STN stn;
	THD thd;
	long ithd;
	long ithdOld;
	long ccki;
	long icki;
	CKI cki;
	TAG tag;
	PMSND pmsnd = pvNil;
	PCFL pcfl = _pcrf->Pcfl();

	// Enum through current movie for user sounds
	// For each	one, extend the lists to include the new sound

	ccki = pcfl->CckiCtg(kctgMsnd);
	for (icki = 0; icki < ccki; icki++)
		{
		if (!pcfl->FGetCkiCtg(kctgMsnd, icki, &cki))
			goto LNext;
		tag.sid = ksidUseCrf;  // Non-CD-loaded content
		tag.pcrf = _pcrf;
		tag.ctg = kctgMsnd;
		tag.cno = cki.cno;

		// Read the msnd chunk and continue if sty's do not match
		pmsnd = (PMSND)vptagm->PbacoFetch(&tag, MSND::FReadMsnd);
		if (pvNil == pmsnd)
			goto LNext;

		// Don't add user sounds multiple times on mult reinstantiations
		if (_FSndListed(cki.cno, &ithdOld))
			{
			if (pmsnd->FValid())
				goto LNext;
			// Remove invalid snds from the lists
			_pglthd->Delete(ithdOld);
			_pgst->Delete(ithdOld);
			for (ithd = ithdOld; ithd < _pglthd->IvMac(); ithd++)
				{
				_pglthd->Get(ithd, &thd);
				thd.ithd--;
				_pglthd->Put(ithd, &thd);
				}
			goto LNext;
			}

		if (_sty != pmsnd->Sty())
			goto LNext;

		if (!pmsnd->FValid())
			goto LNext;

		if (!pcfl->FGetName(kctgMsnd, cki.cno, &stn))
			goto LNext;

		if (!_FAddThd(&stn, &cki))
			goto LNext;

LNext:
		ReleasePpo(&pmsnd);
		}
	return fTrue;
}


/****************************************************
 *
 * Test to see if a sound is already in the lists
 *
 ****************************************************/
bool BRWM::_FSndListed(CNO cno, long *pithd)
{
	AssertBaseThis(0);

	long ithd;
	THD *pthd;

	for (ithd = _cthumCD; ithd < _pglthd->IvMac(); ithd++)
		{
		pthd = (THD *)_pglthd->QvGet(ithd);
		if (pthd->tag.cno == cno)
			{
			if (pvNil != pithd)
				*pithd = ithd;
			return fTrue;
			}
	    }
	return fFalse;
}

/****************************************************
 *
 * Extend the BRWL lists
 *
 ****************************************************/
bool BRWM::_FAddThd(STN *pstn, CKI *pcki)
{
	AssertBaseThis(0);
	THD thd;

	if (!_pgst->FAddStn(pstn))
		return fFalse;;
		
	thd.tag.sid = ksidUseCrf;
	thd.tag.pcrf = _pcrf;
	thd.tag.ctg = pcki->ctg;
	thd.tag.cno = pcki->cno;
	thd.cno = cnoNil; 		//unused
	thd.chidThum = chidNil; //unused
	thd.ithd = _pglthd->IvMac();
	if (!_pglthd->FAdd(&thd))
		goto LFail1;
	
	return fTrue;
LFail1:
	_pgst->Delete(_pgst->IstnMac() - 1);
	return fFalse;
}


/****************************************************
 *
 * Process Browser Music Selection
 * Process selection from either main sound browser or
 * Import sound browser
 *
 ****************************************************/
void BRWM::_ProcessSelection(void)
{
	AssertThis(0);
	PMSND pmsnd;
	TAG tag;
	long thumSelect;
	long sid;

 	// Get Selection from virtual function
	_GetThumFromIthum(_ithumSelect, &thumSelect, &sid);

	tag.sid = sid;
	tag.pcrf = (ksidUseCrf == sid) ? _pcrf : pvNil;
	tag.ctg = kctgMsnd;
	tag.cno = (CNO)thumSelect;

	if (!vptagm->FCacheTagToHD(&tag))
		{
		Warn("cache tag failed");
		return;
		}

	pmsnd = (PMSND)vptagm->PbacoFetch(&tag, MSND::FReadMsnd);

	if (pvNil == pmsnd)
		return;
	pmsnd->Play(0, fFalse, fFalse, pmsnd->Vlm(), fTrue);
	ReleasePpo(&pmsnd);

	return;
}


/****************************************************
 *
 * Browser Command Handler : (For importing sounds)
 * This command will be generated following portfolio
 * execution
 *
 ****************************************************/
bool BRWM::FCmdFile(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	FNI fni;
	long kidBrws;
	PFIL pfil = pvNil;	// Wave or midi file
	PCFL pcfl = pvNil;	// Movie file
	long icki;
	long ccki;
	STN stn;
	CKI cki;
	CMD cmd;

	
	vapp.GetPortfolioDoc(&fni);
	vpappb->BeginLongOp();

	switch(fni.Ftg())
		{
	case kftgMidi:
		// The non-score browsers only import wav files
		Assert(_sty == styMidi, "Portfolio should filter out Wave for this browser");
		if (_sty != styMidi)
			goto LEnd;
		cki.ctg = kctgMidi;
		break;

	case kftgWave:
		Assert(_sty != styMidi, "Portfolio should filter out Midi for this browser");
		// The score browser does not import wave files
		if (_sty == styMidi)
			goto LEnd;
		cki.ctg = kctgWave;
		break;

	case kftg3mm:
	default:
		// Import <user> sounds from a movie
		// Verify version numbers before accepting this file
		pcfl = CFL::PcflOpen(&fni, fcflNil);

		if(pvNil == pcfl)
			goto LEnd;
		if (!_pstdio->Pmvie()->FVerifyVersion(pcfl))
			{
			goto LEnd;	 	// Erc already pushed
			}

		ccki = pcfl->CckiCtg(kctgMsnd);
		if (ccki == 0)
			{
			// There may be sounds in the new movie, but no user sounds
			PushErc(ercSocNoKidSndsInMovie);   		
			goto LEnd;
			}
		
		// Are the user sounds valid?
		for (icki = 0; icki < ccki; icki++)
			{
			bool fInvalid;
			long sty;
			if (!pcfl->FGetCkiCtg(kctgMsnd, icki, &cki))
				goto LEnd;
			if (!MSND::FGetMsndInfo(pcfl, kctgMsnd, cki.cno, &fInvalid, &sty))
				goto LEnd;
			
			if (!fInvalid && sty == _sty)
				goto LValidSnds;  	// There are valid user snds
			}

		PushErc(ercSocNoKidSndsInMovie);   		
		goto LEnd;

LValidSnds:
		// Bring up a new import browser on top of this one
		// Launch the script
		switch(_sty)
			{
		case stySfx:
			kidBrws = kidBrwsImportFX;	
			break;
		case stySpeech:
			kidBrws = kidBrwsImportSpeech;
			break;
		case styMidi:
			kidBrws = kidBrwsImportMidi;
			break;
		default:
			Assert(0, "Invalid _sty in browser");
			break;
			}
		// Tell the script to launch the import browser & send a
		// cidBrowserReady to kidBrws when ready
		vpcex->EnqueueCid(cidLaunchImport, pvNil, pvNil, kidBrws);
		goto LEnd;
		break;	
		}

	// Import sound from a wave or midi file
	pfil = FIL::PfilOpen(&fni);
	if (pvNil == pfil)
		goto LEnd;  	// Error will have been reported

	// Read the file & store as a chunk	in the current movie
	if (!_pstdio->Pmvie()->FCopySndFileToMvie(pfil, _sty, &cki.cno))
		goto LEnd;		// Error will have been reported

	// Add (eg) the movie sounds from &fni to the current
	// browser lists, which are BRWL derived.
	cki.ctg = kctgMsnd;
	fni.GetLeaf(&stn);	 	// name of sound
	if (!_FAddThd(&stn, &cki))
		goto LEnd;

	// Select the item, extend lists, hilite it	& redraw
	cmd.rglw[0] = cki.cno;
	cmd.rglw[1] = ksidUseCrf;
	cmd.rglw[2] = 1; 	// Hilite
	cmd.rglw[3] = 0;	// Lists already updated
	if (!FCmdSelectThum(&cmd))
		goto LEnd;


LEnd:
	vpappb->EndLongOp();
	ReleasePpo(&pcfl);
	ReleasePpo(&pfil);
	return fTrue;
}


/****************************************************
 *
 * Browser Command Handler : (For deleting user snds)
 *
 ****************************************************/
bool BRWM::FCmdDel(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	long sid;
	TAG tag;
	PTAG ptag;
	PMSND pmsnd;
	STN stnErr;
	STN stnSnd;
	PMVU pmvu;

	if (_ithumSelect < _cthumCD)
		return fTrue;		// CD Sounds cannot be deleted

	pmvu = (PMVU)(_pstdio->Pmvie()->PddgGet(0));
 	AssertPo(pmvu, 0);
	_pgst->GetStn(_ithumSelect, &stnSnd);

	// Display the sound name in the help topic.
	AssertDo(vapp.FGetStnApp(idsDeleteSound, &stnErr), "String not present");
	if (vapp.TModal(vapp.PcrmAll(), ktpcQuerySoundDelete, &stnErr, bkYesNo,
			kstidQuerySoundDelete, &stnSnd) != tYes)
		{
		return fTrue;
		}

 	// Delete the midi or wave child chunk from the msnd
	// Invalidate the sound
	tag.sid = ksidUseCrf; 			// Non-CD-loaded content
	tag.pcrf = _pcrf;
	tag.ctg = kctgMsnd;
	_GetThumFromIthum(_ithumSelect, &tag.cno, &sid);

	pmsnd = (PMSND)vptagm->PbacoFetch(&tag, MSND::FReadMsnd);
	if (pvNil == pmsnd)
		return fTrue;

	if (!pmsnd->FInvalidate())
		Warn("Error invalidating sound");
	ReleasePpo(&pmsnd);

	if (!_FUpdateLists())
		{
		Release();	 	// Release browser; labels might be wrong
		return fTrue;
		}

	if (_ithumPageFirst >= _pglthd->IvMac())
		_ithumPageFirst--;
	if (_ithumPageFirst < 0)
		_ithumPageFirst = 0;

	_ithumSelect = ivNil;

	// Clear the selection if deleted
	tag.sid = ksidInvalid;
	ptag = pmvu->PtagTool();
	if (ptag->sid == ksidUseCrf && ptag->cno == tag.cno)
 		{
 		pmvu->SetTagTool(&tag);	 	// No need to close tag with ksidInvalid	
		}
	else if (ptag->sid != ksidInvalid)
		{
		// Show current cursor selection
		_ithumSelect = _IthumFromThum(ptag->cno, ptag->sid);
		}

	_SetScrollState();		// Set the state of the scroll arrows
	_pstdio->Pmvie()->Pmsq()->StopAll();
	FDraw();
	return fTrue;
}


/****************************************************
 *
 * Create a browser text object
 *
 * Returns:
 *	A pointer to the object, else pvNil.
 *
 ****************************************************/
PBRWT BRWT::PbrwtNew(PRCA prca, long kidPar, long kidGlass)
{
	AssertPo(prca, 0);

	PBRWT pbrwt;
	GCB gcb;

	if (!_FBuildGcb(&gcb, kidPar, kidGlass))
		return pvNil;

	if ((pbrwt = NewObj BRWT(&gcb)) == pvNil)
		return pvNil;

	// Initialize the gok
	if (!pbrwt->_FInitGok(prca, kidGlass))	
		{
		ReleasePpo(&pbrwt);
		return pvNil;
		}

	pbrwt->_pgst = pvNil;

	return pbrwt;
}


/****************************************************
 *
 * Set the Gst for BRWT text
 *
 ****************************************************/	
void BRWT::SetGst(PGST pgst)
{
	AssertThis(0);
	AssertPo(pgst, 0);

	ReleasePpo(&_pgst);

	_pgst = pgst;
	_pgst->AddRef();
}

/****************************************************
 *
 * Initialize BRWT TGOB & text
 *
 ****************************************************/	
bool BRWT::FInit(PCMD pcmd, long thumSelect, long thumDisplay,
	PSTDIO pstdio, bool fWrapScroll, long cthumScroll)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	BRWD::Init(pcmd, thumSelect, thumDisplay, pstdio, fWrapScroll, cthumScroll);

	//
	// Create the tgob's for each frame on the page
	//
	if (!FCreateAllTgob())
		 return fFalse;

	vapp.DisableAccel();
	_fEnableAccel = fTrue;

	return fTrue;
}


/****************************************************
 *
 * Sets the ith text item in the child of the
 * current frame
 *
 ****************************************************/	
bool BRWT::_FSetThumFrame(long istn, PGOB pgobPar)
{
	AssertThis(0);
	AssertIn(istn, 0, _pgst->IvMac());
	AssertPo(pgobPar, 0);

	PTGOB ptgob;
	STN stnLabel;

	_pgst->GetStn(istn, &stnLabel);
	ptgob = (PTGOB)(pgobPar->PgobFirstChild());

	Assert(pvNil != ptgob, "No TGOB for the text");
	if (pvNil != ptgob)
		{
		Assert(ptgob->FIs(kclsTGOB), "GOB isn't a TGOB");
		if (ptgob->FIs(kclsTGOB))
			{
			ptgob->SetText(&stnLabel);
			return fTrue;
			}
		}

	return fFalse;
}


/****************************************************
 *
 * Create a BRoWser ActioN object
 *
 * Returns:
 *  A pointer to the view, else pvNil.
 *
 ****************************************************/
PBRWA BRWA::PbrwaNew(PRCA prca)
{
	AssertPo(prca, 0);

	PBRWA pbrwa;
	PGOK pgok;
	GCB gcb;

	if (!_FBuildGcb(&gcb, kidBackground, kidActionGlass))
		return pvNil;

	if ((pbrwa = NewObj BRWA(&gcb)) == pvNil)
		return pvNil;

	// Initialize the gok
	if (!pbrwa->_FInitGok(prca, kidActionGlass))	
		{
		ReleasePpo(&pbrwa);
		return pvNil;
		}

	//
	// Stop the action browser animation while the browser is up.
	//
	pgok = (PGOK)vapp.Pkwa()->PgobFromHid(kidActorsActionBrowser);

	if ((pgok != pvNil) && pgok->FIs(kclsGOK))
		{
		Assert(pgok->FIs(kclsGOK), "Invalid class");
		pgok->FChangeState(kstSelected);
		}
							
	pbrwa->_ptgobPage = TGOB::PtgobCreate(kidBrowserPageNum, idsBrwsPageFont, tavCenter);
	return pbrwa;
}

/****************************************************
 *
 * Build the ape
 *
 ****************************************************/
bool BRWA::FBuildApe(PACTR pactr)
{
	AssertThis(0);
	AssertPo(pactr, 0);
	
	COST cost;
	PGOK pgokFrame;
	RC rcRel;
	GCB gcb;

	if (!cost.FGet(pactr->Pbody()))
		return fFalse;
	pgokFrame = (PGOK)vapp.Pkwa()->PgobFromHid(kidBrwsActionPrev);
	Assert(pgokFrame->FIs(kclsGOK), "Invalid class");
	rcRel.Set(krelZero, krelZero, krelOne, krelOne);
	gcb.Set(kidBrwsActionPrev, pgokFrame, fgobNil, kginDefault, pvNil, &rcRel);
	_pape = APE::PapeNew(&gcb, pactr->Ptmpl(), &cost, pactr->AnidCur(), fTrue);
	if (pvNil == _pape)
		return fFalse;
	return fTrue;
}


/***************************************************************************
 *
 * Build the string table for actions prior to action initialization
 * Transfer the string table to BRWA
 *
 * NOTE:  The string table is built from the template as the code already
 * has action names cached on the hard drive for selected actors. The
 * string table is built for fast scrolling.
 *
 **************************************************************************/
bool BRWA::FBuildGst(PSCEN pscen)
{
	AssertThis(0);

	STN stn;
	PTMPL ptmpl;
	long cactn;
	long iactn;
	PGST pgst;
	
	Assert(pvNil != pscen && pvNil != pscen->PactrSelected(),
		"kidBrwsAction: Invalid actor");

	ptmpl = pscen->PactrSelected()->Ptmpl();
	Assert(pvNil != ptmpl, "Actor has no template");

	if (pvNil == (pgst = GST::PgstNew(0)))
		return fFalse;

	cactn = ptmpl->Cactn();
	for (iactn = 0; iactn < cactn; iactn++)
		{
		if (!ptmpl->FGetActnName(iactn, &stn))
			goto LFail;
		if (!pgst->FAddStn(&stn))
			goto LFail;
		}

	SetGst(pgst);
	ReleasePpo(&pgst);

	if (!FBuildApe(pscen->PactrSelected()))
		goto LFail;

	if (!ptmpl->FIsTdt())
		_pape->SetCustomView(BR_ANGLE_DEG(0.0), BR_ANGLE_DEG(-30.0), BR_ANGLE_DEG(0.0));

	return fTrue;

LFail:
	ReleasePpo(&pgst);
	return fFalse;		
}


/****************************************************
 *
 * Process Browser Action Selection
 *
 ****************************************************/
void BRWA::_ProcessSelection(void)
{
	AssertThis(0);
	_pape->FSetAction(_ithumSelect);
	_pape->SetCycleCels(fTrue);
	_pape->FDisplayCel(_celnStart);	// Restart cycling at current display
	_celnStart = 0;		// When applying action, apply celn == 0
}

/****************************************************
 *
 * Browser Command Handler : Change cel (action brws)
 * pcmd->rglw[0] = kid of Change, Fwd, Back
 * pcmd->rglw[1] = bool reflection start/stop change cel
 *
 * Make button(s) invisible on single cel actions
 *
 ****************************************************/
bool BRWA::FCmdChangeCel(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	long st;
	long ccel;
	PACTR pactr = _pstdio->Pmvie()->Pscen()->PactrSelected();

	if (!pactr->Ptmpl()->FGetCcelActn(_pape->Anid(), &ccel))
		return fTrue;

	if (1 == ccel)
		st = kstBrowserInvisible;
	else
		st = kstBrowserSelected;

	if (_pape->FIsCycleCels())
		{
		_pape->SetCycleCels(fFalse);
		_celnStart = _pape->Celn();
		}

	if (1 == ccel)
		return fTrue;

	switch (pcmd->rglw[0])
		{
	case kidBrowserActionFwdCel:
		_celnStart++;
		_pape->FDisplayCel(_celnStart);
		break;
	case kidBrowserActionBackCel:
		_celnStart--;
		_pape->FDisplayCel(_celnStart);
		break;
	default:
		Bug("Unimplemented Change Cel kid");
		break;
		}
	return fTrue;
}


/****************************************************
 *
 * Create a BRoWser Import object
 * 		(A browser on top of a sound browser)
 * Returns:
 *  A pointer to the view, else pvNil.
 *
 ****************************************************/
PBRWI BRWI::PbrwiNew(PRCA prca, long kidGlass, long sty)
{
	AssertPo(prca, 0);

	PBRWI pbrwi;
	GCB gcb;

	if (!_FBuildGcb(&gcb, kidBackground, kidGlass))
		return pvNil;

	if ((pbrwi = NewObj BRWI(&gcb)) == pvNil)
		return pvNil;

	// Initialize the gok
	if (!pbrwi->_FInitGok(prca, kidGlass))
		{
		ReleasePpo(&pbrwi);
		return pvNil;
		}

	pbrwi->_sty = sty;
	pbrwi->_ptgobPage = TGOB::PtgobCreate(kidImportPageNum, idsBrwsPageFont, tavCenter);
	return pbrwi;
}


/***************************************************************************
 *
 * Initialize the BRWI	 (Import Browser)
 *
 **************************************************************************/
bool BRWI::FInit(PCMD pcmd, CKI ckiRoot, PSTDIO pstdio)
{
	AssertBaseThis(0);

	PCFL pcfl;
	FNI fni;
	STN stn;

	_pstdio = pstdio;
	//
	// Create the gl's
	//
	if (pvNil == (_pglthd = GL::PglNew(size(THD), kglthdGrow)))
		return fFalse;			
	_pglthd->SetMinGrow(kglthdGrow);

	if (pvNil == (_pgst = GST::PgstNew(0)))
		return fFalse;

	_ckiRoot = ckiRoot;
	_bws = kbwsCnoRoot;

	// Fill the lists with sounds from the portfolio movie
	vapp.GetPortfolioDoc(&fni);
	pcfl = CFL::PcflOpen(&fni, fcflNil);
	if (pvNil == pcfl)
		goto LFail;	  // Error already reported
	_pcrf = CRF::PcrfNew(pcfl, 0);
	ReleasePpo(&pcfl);

	if (pvNil == _pcrf)
		goto LFail;
	if (!_FUpdateLists()) 	// to include sounds from the selected movie
		goto LFail;

	BRWD::Init(pcmd, ivNil, ivNil, pstdio, fTrue);
	if (!FCreateAllTgob())
		goto LFail;
	
	return fTrue;

LFail:
	return fFalse;
}


/****************************************************
 *
 * Create a BRoWser Prop/Actor object
 *
 * Returns:
 *  A pointer to the view, else pvNil.
 *
 ****************************************************/
PBRWP BRWP::PbrwpNew(PRCA prca, long kidGlass)
{
	AssertPo(prca, 0);

	PBRWP pbrwp;
	GCB gcb;

	if (!_FBuildGcb(&gcb, kidBackground, kidGlass))
		return pvNil;

	if ((pbrwp = NewObj BRWP(&gcb)) == pvNil)
		return pvNil;

	// Initialize the gok
	if (!pbrwp->_FInitGok(prca, kidGlass))	
		{
		ReleasePpo(&pbrwp);
		return pvNil;
		}

	pbrwp->_ptgobPage = TGOB::PtgobCreate(kidBrowserPageNum, idsBrwsPageFont, tavCenter);
	return pbrwp;
}


/****************************************************
 *
 * Create a BRoWser Background object
 *
 * Returns:
 *  A pointer to the view, else pvNil.
 *
 ****************************************************/
PBRWB BRWB::PbrwbNew(PRCA prca)
{
	AssertPo(prca, 0);

	PBRWB pbrwb;
	GCB gcb;

	if (!_FBuildGcb(&gcb, kidBackground, kidSettingsGlass))
		return pvNil;

	if ((pbrwb = NewObj BRWB(&gcb)) == pvNil)
		return pvNil;

	// Initialize the gok
	if (!pbrwb->_FInitGok(prca, kidSettingsGlass))	
		{
		ReleasePpo(&pbrwb);
		return pvNil;
		}

	pbrwb->_ptgobPage = TGOB::PtgobCreate(kidBrowserPageNum, idsBrwsPageFont, tavCenter);
	return pbrwb;
}

/****************************************************
 *
 * Background's virtual FCmdCancel
 *
 ****************************************************/
bool BRWB::FCmdCancel(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	AssertVarMem(_pstdio);

	PMVU pmvu;

 	// Update the tool
 	pmvu = (PMVU)(_pstdio->Pmvie()->PddgActive());
 	AssertPo(pmvu, 0);
 	pmvu->SetTool(toolDefault);

 	// Update the UI
 	_pstdio->Pmvie()->Pmcc()->ChangeTool(toolDefault);

	if ((vpappb->GrfcustCur() & fcustCmd) && (_pstdio->Pmvie()->Cscen() == 0))
		_pstdio->SetDisplayCast(fTrue);
	else
		_pstdio->SetDisplayCast(fFalse);
	return BRWB_PAR::FCmdCancel(pcmd);
}


/****************************************************
 *
 * Set the size of the pcrm 
 *
 ****************************************************/
void BRWL::_SetCbPcrmMin(void)
{
	AssertThis(0);
	long dwTotalPhys;
	long dwAvailPhys;

	// If short on memory, pull in the cache
	((APP *)vpappb)->MemStat(&dwTotalPhys, &dwAvailPhys);
	if (dwTotalPhys > kdwTotalPhysLim && dwAvailPhys > kdwAvailPhysLim)
		return;

	if (pvNil != _pcrm)
		{
		for (long icrf = 0; icrf < _pcrm->Ccrf(); icrf++)
			{
			_pcrm->PcrfGet(icrf)->SetCbMax(0);
			}
		}
}


/****************************************************
 *
 * Create a BRoWser Camera object
 *
 * Returns:
 *  A pointer to the view, else pvNil.
 *
 ****************************************************/
PBRWC BRWC::PbrwcNew(PRCA prca)
{
	AssertPo(prca, 0);

	PBRWC pbrwc;
	GCB gcb;

	if (!_FBuildGcb(&gcb, kidBackground, kidCameraGlass))
		return pvNil;

	if ((pbrwc = NewObj BRWC(&gcb)) == pvNil)
		return pvNil;

	// Initialize the gok
	if (!pbrwc->_FInitGok(prca, kidCameraGlass))	
		{
		ReleasePpo(&pbrwc);
		return pvNil;
		}

	pbrwc->_ptgobPage = TGOB::PtgobCreate(kidBrowserPageNum, idsBrwsPageFont, tavCenter);
	return pbrwc;
}


/****************************************************
 *
 * Camera's virtual FCmdCancel
 *
 ****************************************************/
bool BRWC::FCmdCancel(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	AssertVarMem(_pstdio);

	PMVU pmvu;

 	// Update the tool
 	pmvu = (PMVU)(_pstdio->Pmvie()->PddgActive());
 	AssertPo(pmvu, 0);
 	pmvu->SetTool(toolDefault);

 	// Update the UI
 	_pstdio->Pmvie()->Pmcc()->ChangeTool(toolDefault);

	return BRWC_PAR::FCmdCancel(pcmd);
}

/****************************************************
 *
 * Create a BRoWser Roll Call object
 *
 ****************************************************/
PBRWR BRWR::PbrwrNew(PRCA prca, long kid)
{
	AssertPo(prca, 0);

	PBRWR pbrwr;
	GCB gcb;

	if (!_FBuildGcb(&gcb, kidBackground, kid))
		return pvNil;

	if ((pbrwr = NewObj BRWR(&gcb)) == pvNil)
		return pvNil;

	// Initialize the gok
	if (!pbrwr->_FInitGok(prca, kid))	
		{
		ReleasePpo(&pbrwr);
		return pvNil;
		}

	return pbrwr;
}

/****************************************************
 *
 * Initialize a BRoWser Roll Call object
 *
 ****************************************************/
bool BRWR::FInit(PCMD pcmd, CTG ctgTmplThum, long ithumDisplay, PSTDIO pstdio)
{
	AssertThis(0);
	
	long ccrf = 1;
	PCFL pcfl;
	BLCK blck;
	long ccki;
	long icki;
	CKI cki;
	TFC tfc;
	KID kid;
	FNET fnet;
	FNI fniThd;

	if (!fnet.FInit())
		return fFalse;

	_ctg = ctgTmplThum;
	BRWD::Init(pcmd, ivNil, ithumDisplay, pstdio, fFalse, 1);

	_pcrm = CRM::PcrmNew(ccrf);
	if (pvNil == _pcrm)
		goto LFail;		// Error already reported

	while(fnet.FNext(&fniThd))
		{
		if (fniThd.Ftg() != kftgThumbDesc)
			continue;
		pcfl = CFL::PcflOpen(&fniThd, fcflNil);
		if (pvNil == pcfl)
			{
			goto LFail;	  // Error already reported
			}
	
		// Add the file to the crm
		if (!_pcrm->FAddCfl(pcfl, kcbMaxCrm))
			goto LFail;	// Error already reported	

		// Create the cno map from tmpl->gokd
		if (ctgTmplThum == kctgTmth)
			ccki = pcfl->CckiCtg(kctgTmth);
		else
			ccki = pcfl->CckiCtg(kctgPrth);
		for (icki = 0; icki < ccki; icki++)
			{
			if (!pcfl->FGetCkiCtg(ctgTmplThum, icki, &cki))
				continue;

			// Read the chunk to map the cno of the CD content
			if (!pcfl->FFind(cki.ctg, cki.cno, &blck) ||
				!blck.FUnpackData())
				{
				goto LFail;
				}
			if (blck.Cb() != size(TFC))
				goto LFail;
			if (!blck.FReadRgb(&tfc, size(TFC), 0))
				goto LFail;
			if (kboCur != tfc.bo)
				SwapBytesBom(&tfc, kbomTfc);
			Assert(kboCur == tfc.bo, "bad TFC");

			if (!pcfl->FGetKidChidCtg(ctgTmplThum, cki.cno, 0, kctgGokd, &kid))
				{
				Warn("Actor content missing gokd");
				continue;
				}

			if (!_pstdio->FAddCmg(tfc.cno, kid.cki.cno))
				goto LFail;
			}
		}

	_fNoRepositionSel = fTrue;
	return fTrue;
LFail:
	Warn("Failed to initialize RollCall");
	return fFalse;
}

/****************************************************
 *
 * Update the RollCall : Select actor arid
 *
 ****************************************************/
bool BRWR::FUpdate(long arid, PSTDIO pstdio)
{
	AssertThis(0);
	long ithumDisplay;

	_pstdio = pstdio;
	_ithumSelect = ithumDisplay = _IthumFromArid(arid);

	// Define the index of the first thum on the page
	if (ithumDisplay != ivNil)
		{
		if (!_fNoRepositionSel || 
			ithumDisplay < _ithumPageFirst || 
			ithumDisplay >= _ithumPageFirst + _cfrm)
			{
			_ithumPageFirst = ithumDisplay;
			}
		}

	// Define the number of frames on the previous page and
	// account for wrap-around
	_SetScrollState();		// Set the state of the scroll arrows

	return FDraw();
}


/****************************************************
 *
 * Process Browser Roll Call Selection
 * As the RollCall Browsers do not have OK buttons,
 * the selection is applied now.
 *
 * NOTE: The RollCall scripts cannot generate FCmdOk
 * as that would abort the browser script out from
 * underneath the code.
 *
 ****************************************************/
void BRWR::_ProcessSelection(void)
{
	AssertThis(0);
	
	long thumSelect;
	long sid;

	// Get Selection from virtual function
	_GetThumFromIthum(_ithumSelect, &thumSelect, &sid);
	
	// Apply the selection
	_ApplySelection(thumSelect, sid);
}


/****************************************************
 *
 * Browser Roll Call Cthum
 *
 ****************************************************/
long BRWR::_Cthum(void)
{
	AssertThis(0);
	STN stn;
	long iarid;
	long arid;
	long cactRef;
	long cthum = 0;
	bool fProp;

	Assert(_ctg == kctgPrth || _ctg == kctgTmth,
		"Invalid BRWR initialization");

	if (_pstdio->Pmvie() == pvNil)
		return 0;

	for (iarid = 0;
		_pstdio->Pmvie()->FGetArid(iarid, &arid, &stn, &cactRef);
		iarid++)
		{
		// Verify actor in correct browser
		fProp = _pstdio->Pmvie()->FIsPropBrwsIarid(iarid);

		if (_ctg == kctgPrth && !fProp)
			continue;
		if (_ctg == kctgTmth && fProp)
			continue;
		if (!cactRef)
			continue;
		cthum++;
		}
	return cthum;
}


/****************************************************
 *
 * Iarid from Ithum
 *
 ****************************************************/
long BRWR::_IaridFromIthum(long ithum, long iaridFirst)
{
	AssertThis(0);
	STN stn;
	long iarid;
	long arid;
	long cactRef;
	bool fProp;
	long cthum = 0;

	if (_pstdio->Pmvie() == pvNil)
		return ivNil;

	for (iarid = iaridFirst;
		_pstdio->Pmvie()->FGetArid(iarid, &arid, &stn, &cactRef);
		iarid++)
		{
		// Verify actor in correct browser
		fProp = _pstdio->Pmvie()->FIsPropBrwsIarid(iarid);

		if (_ctg == kctgPrth && !fProp)
			continue;
		if (_ctg == kctgTmth && fProp)
			continue;
		if (!cactRef)
			continue;
		if (cthum == ithum)
			return iarid;
		cthum++;
		}
	return iarid;
}

/****************************************************
 *
 * Ithum from AridSelect
 * (eg, find the thumbnail corresponding to the correct
 * copy of Billy)
 *
 ****************************************************/
long BRWR::_IthumFromArid(long aridSelect)
{
	AssertThis(0);

	STN stn;
	long iarid;								
	long arid;
	long cactRef;
	bool fProp;
	long ithum = 0;

	if (_pstdio->Pmvie() == pvNil)
		return ivNil;

	for (iarid = 0;
		_pstdio->Pmvie()->FGetArid(iarid, &arid, &stn, &cactRef);
		iarid++)
		{
		AssertDo(_pstdio->Pmvie()->FGetArid(iarid, &arid, &stn, &cactRef),
			"Arid should exist");
		// Verify actor in correct browser
		fProp = _pstdio->Pmvie()->FIsPropBrwsIarid(iarid);
		if (arid == aridSelect)
			{
			if ((_ctg == kctgPrth) && !fProp)
				return ivNil;
			if ((_ctg == kctgTmth) && fProp)
				return ivNil;
			return ithum;
			}

		if (_ctg == kctgPrth && !fProp)
			continue;
		if (_ctg == kctgTmth && fProp)
			continue;
		if (!cactRef)
			continue;
		ithum++;
		}
	return ivNil;
}


/****************************************************
 *
 * Sets the ith Gob as a child of the current frame
 * Advance the gob (thumbnail) index
 *
 ****************************************************/	
bool BRWR::_FSetThumFrame(long ithum, PGOB pgobPar)
{
	AssertThis(0);
	AssertIn(ithum, 0, _pstdio->Pmvie()->CmactrMac());
	AssertPo(pgobPar, 0);

	RC rcAbsPar;
	RC rcAbs;
	RC rcRel;
	STN stnLabel;
	long stid;
	long cactRef;
	long arid;
	long iarid;
	TAG tag;
	STN stn;
	long dxp;
	long dyp;

	// Associate the gob with the current frame
	iarid = _IaridFromIthum(ithum);
	if (!_pstdio->Pmvie()->FGetArid(iarid, &arid, &stn, &cactRef, &tag))
		return fFalse;
	
	_pstdio->Pmvie()->FGetName(arid, &stnLabel);

	if (!_pstdio->Pmvie()->FIsIaridTdt(iarid))
		{
		PGOK pgok;
		CNO cno = _pstdio->CnoGokdFromCnoTmpl(tag.cno);
		long kidThum = _KidThumFromIfrm(_cfrmPageCur);
		pgok = vapp.Pkwa()->PgokNew(pgobPar, kidThum, cno, _pcrm);
		if (pvNil == pgok)
			return fFalse;

		// Note: The graphic is not the correct size
		((PGOB)pgok)->GetPos(&rcAbs, &rcRel);
		pgobPar->GetPos(&rcAbsPar, pvNil);
		dxp = (rcAbs.Dxp() - rcAbsPar.Dxp()) / 2;
		dyp = (rcAbs.Dyp() - rcAbsPar.Dyp()) / 2;
		rcAbs.xpLeft += (_dxpFrmOffset - dxp);
		rcAbs.ypTop += (_dypFrmOffset - dyp);
		rcAbs.xpRight += (dxp - _dxpFrmOffset);
		rcAbs.ypBottom += (dyp - _dypFrmOffset);
		((PGOB)pgok)->SetPos(&rcAbs, &rcRel);
		}
	else
		{
		PTGOB ptgob;
		STN stn = stnLabel;
		long cch = stn.Cch();
		long hidThum;

		// Display the text as the thumbnail
		hidThum = _KidThumFromIfrm(_cfrmPageCur);
		ptgob = TGOB::PtgobCreate(_kidFrmFirst + _cfrmPageCur, _idsFont, tavCenter, hidThum);

		if (pvNil != ptgob)
			{
			ptgob->SetText(&stn);
			}
		}
		
	// Put the name in the global string registry for rollover help
	stid = (_ctg == kctgPrth) ? kstidProp : kstidActor;
	stid += _cfrmPageCur;
	return vapp.Pkwa()->Pstrg()->FPut(stid, &stnLabel);
}

 /****************************************************
 *
 * Clear rollover help
 *
 ****************************************************/	
bool BRWR::_FClearHelp(long ifrm)
{
	AssertThis(0);
	AssertIn(ifrm, 0, _cfrm);
	
	STN stn;
	stn.SetNil();
	long stid = (_ctg == kctgPrth) ? kstidProp : kstidActor;
	stid += ifrm;
	return vapp.Pkwa()->Pstrg()->FPut(stid, &stn);
}

 /****************************************************
 *
 * Release previous thum from frame ifrm
 * Assumes gob based.
 *
 ****************************************************/	
void BRWR::_ReleaseThumFrame(long ifrm)
{
	AssertThis(0);
	AssertIn(ifrm, 0, _cfrm);
	PGOB pgob;

	// Release previous gob associated with the current frame
	pgob = _PgobFromIfrm(ifrm);
	if (pvNil != pgob)
		{
		ReleasePpo(&pgob);
		}
}



/****************************************************
 *
 * Browser Destructor
 *
 ****************************************************/
BRWD::~BRWD(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pbrcn);
	vpappb->FSetProp(kpridBrwsOverrideThum, -1);
	vpappb->FSetProp(kpridBrwsOverrideKidThum, -1);
}

/****************************************************
 *
 * Browser List Destructor
 *
 ****************************************************/
BRWL::~BRWL(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pglthd);
	ReleasePpo(&_pcrm);
	ReleasePpo(&_pgst);

	if (_fEnableAccel)
		vapp.EnableAccel();
}

/****************************************************
 *
 * Browser Text Destructor
 *
 ****************************************************/
BRWT::~BRWT(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pgst);

	if (_fEnableAccel)
		vapp.EnableAccel();
}

/****************************************************
 *
 * Browser RollCall Destructor
 *
 ****************************************************/
BRWR::~BRWR(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pcrm);
}

/****************************************************
 *
 * Browser Music Import Destructor
 *
 ****************************************************/
BRWI::~BRWI(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pcrf);
}

/****************************************************
 *
 * Browser Context Destructor
 *
 ****************************************************/
BRCNL::~BRCNL(void)
{
	AssertBaseThis(0);
	ReleasePpo(&pglthd);
	ReleasePpo(&pgst);
	ReleasePpo(&pcrm);
}

#ifdef DEBUG
/****************************************************

	Mark memory used by the BRCN

 ****************************************************/
void BRCN::MarkMem(void)
{
	AssertThis(0);
	BRCN_PAR::MarkMem();
}

/****************************************************

	Mark memory used by the BRCNL

 ****************************************************/
void BRCNL::MarkMem(void)
{
	AssertThis(0);
	MarkMemObj(pglthd);
	MarkMemObj(pgst);
	MarkMemObj(pcrm);
	BRCNL_PAR::MarkMem();
}

/****************************************************

	Mark memory used by the BRWR

 ****************************************************/
void BRWR::MarkMem(void)
{
	AssertThis(0);
	MarkMemObj(_pcrm);
	BRWR_PAR::MarkMem();
}


/****************************************************

	Mark memory used by the BRWI

 ****************************************************/
void BRWI::MarkMem(void)
{
	AssertThis(0);
	MarkMemObj(_pcrf);
	BRWI_PAR::MarkMem();
}


/****************************************************

	Mark memory used by the BRWD

 ****************************************************/
void BRWD::MarkMem(void)
{
	AssertThis(0);
	BRWD_PAR::MarkMem();
	MarkMemObj(_pbrcn);
}


/****************************************************

	Mark memory used by the BRWL

 ****************************************************/
void BRWL::MarkMem(void)
{
	AssertThis(0);

	BRWL_PAR::MarkMem();
	MarkMemObj(_pglthd);
	MarkMemObj(_pcrm);
	MarkMemObj(_pgst);
}


/****************************************************

	Mark memory used by the BRWT

 ****************************************************/
void BRWT::MarkMem(void)
{
	AssertThis(0);

	MarkMemObj(_pgst);
	BRWT_PAR::MarkMem();
}

/****************************************************

	Mark memory used by the BRWA

 ****************************************************/
void BRWA::MarkMem(void)
{
	AssertThis(0);

	MarkMemObj(_pape);
	BRWA_PAR::MarkMem();
}

/****************************************************

	BCL Markmem

 ****************************************************/
void BCL::MarkMem(void)
{
	BCL_PAR::MarkMem();
	MarkMemObj(_pglthd);
}

void BCLS::MarkMem(void)
{
	BCLS_PAR::MarkMem();
	MarkMemObj(_pgst);
}
  

/****************************************************

	Assert the validity of the BRCN

 ****************************************************/
void BRCN::AssertValid(ulong grfobj)
{
	BRCN_PAR::AssertValid(fobjAllocated);
}

/****************************************************

	Assert the validity of the BRCNL

 ****************************************************/
void BRCNL::AssertValid(ulong grfobj)
{
	BRCNL_PAR::AssertValid(fobjAllocated);
}

/****************************************************

	BCL AssertValid

 ****************************************************/
void BCLS::AssertValid(ulong grf)
{
	BCLS_PAR::AssertValid(grf);
	AssertPo(_pgst, 0);
}
void BCL::AssertValid(ulong grf)
{
	BCL_PAR::AssertValid(grf);
	AssertPo(_pglthd, 0);
}


/****************************************************

	Assert the validity of the BRWD

 ****************************************************/
void BRWD::AssertValid(ulong grfobj)
{
	BRWD_PAR::AssertValid(fobjAllocated);
	AssertNilOrPo(_pbrcn, 0);
}

/****************************************************

	Assert the validity of the BRWL

 ****************************************************/
void BRWL::AssertValid(ulong grfobj)
{
	
	BRWL_PAR::AssertValid(fobjAllocated);
	AssertNilOrPo(_pgst, 0);
	AssertNilOrPo(_pglthd, 0);
	AssertNilOrPo(_pcrm, 0);
}

/****************************************************

	Assert the validity of the BRWR

 ****************************************************/
void BRWR::AssertValid(ulong grfobj)
{
	
	BRWR_PAR::AssertValid(fobjAllocated);
	AssertNilOrPo(_pcrm, 0);
}

/****************************************************

	Assert the validity of the BRWI

 ****************************************************/
void BRWI::AssertValid(ulong grfobj)
{
	
	BRWI_PAR::AssertValid(fobjAllocated);
	AssertNilOrPo(_pcrf, 0);
}

/****************************************************

	Assert the validity of the BRWT

 ****************************************************/
void BRWT::AssertValid(ulong grfobj)
{
	BRWT_PAR::AssertValid(fobjAllocated);
	AssertNilOrPo(_pgst, 0);
}	

/****************************************************

	Assert the validity of the BRWA

 ****************************************************/
void BRWA::AssertValid(ulong grfobj)
{
	BRWA_PAR::AssertValid(fobjAllocated);
	AssertNilOrPo(_pape, 0);
}	
#endif //DEBUG
