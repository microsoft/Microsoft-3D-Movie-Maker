/***************************************************************************

  stdiobrw.cpp

  Author: Susano

  Date: April, 1995

	Review Status: Reviewed

	This file contains the code which invokes browsers
	and applies browser selections.

	Studio Independent Browsers:
	BASE --> CMH --> GOK	-->	BRWD  (Browser display class)
	BRWD --> BRWL  (Browser list class; chunky based)
	BRWD --> BRWT  (Browser text class)
	BRWD --> BRWL --> BRWN  (Browser named list class)

	Studio Dependent Browsers:
	BRWD --> BRWT --> BRWA  (Browser action class)
	BRWD --> BRWL --> BRWP	(Browser prop/actor class)
	BRWD --> BRWL --> BRWB	(Browser background class)
	BRWD --> BRWL --> BRWC	(Browser camera class)
	BRWD --> BRWL --> BRWN --> BRWM (Browser music class)
	BRWD --> BRWL --> BRWN --> BRWM --> BRWI (Browser import sound class)

NOTE:  In this implementation, browsers are considered to be studio related.
If for any reason one wanted to decouple them from the studio, then	it would
be easy for browser.cpp to enqueue cids for
	1) SetTagTool and
	2) ApplySelection (which would take a browser identifying argument).
The studio (or anyone else) could then apply all browser based selections.

The chunky based browsers come in two categories:
	1) Content spanning potentially multiple products (eg, bkgds, actors)
	2) Content which is a child of an existing selection.
Browsers of type 1) are cno based.  They sort on the basis of the cno of the
thumbnail chunk. The contents of the thumbnail chunk then point to the cno
of the CD content.
Browsers of type 2) are chid based. They sort on the basis of the chid of the
thumbnail chunk.  The contents of the thumbnail chunk then point to the chid
of the CD content.

***************************************************************************/

#include "soc.h"
#include "studio.h"

ASSERTNAME

/***************************************************************************
 *
 * Handle Browser Ready command
 * A Browser Ready command signals the invocation of an empty browser
 *
 * Parameters:
 *	pcmd - Pointer to the command to process.
 *	pcmd[0] = kid of Browser (type)
 *  pcmd[1] = kid first frame.  Thumb kid is this + kidBrowserThumbOffset
 *  pcmd[2] = kid of first control
 *  pcmd[3] = x,y offsets
 *
 * Returns:
 * 	fTrue if it handled the command, else fFalse.
 *
 **************************************************************************/
const long kglpbrcnGrow = 5;

bool STDIO::FCmdBrowserReady(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	bool fSuccess = fFalse;
	PBRCN pbrcn = pvNil;		// Browser context carryover
	PBRWD pbrwd = pvNil;
	CKI ckiRoot;
	TAG tag;
	PTAG ptag;
	PMVU pmvu;
	long thumSelect;
	long sid = ((APP *)vpappb)->SidProduct();
	long brwdid = pcmd->rglw[0];

	vapp.BeginLongOp();
	
	if (pvNil == _pmvie)
		goto LFail;

	AssertPo(_pmvie, 0);

	// Optionally Save/Retrieve Browser Context
	if (_pglpbrcn == pvNil)
		{
		if (pvNil == (_pglpbrcn = GL::PglNew(size(PBRCN), kglpbrcnGrow)))
			goto LFail;
		}
	
	// Include optional argument pbrcn if you want context carryover
	pbrcn = _PbrcnFromBrwdid(brwdid);
	AssertNilOrPo(pbrcn, 0);

	switch(brwdid)
		{
	case kidBrwsBackground:
		// Search for background thumbs of any cno
		ckiRoot.cno = cnoNil;
		ckiRoot.ctg = kctgBkth;

		if (pvNil == _pmvie->Pscen())
			{
			thumSelect = (long)cnoNil;
			TrashVar(&sid);
			}
		else
			{
			Assert(pvNil != _pmvie->Pscen()->Pbkgd(), "Pbkgd() Nil");
			thumSelect = _pmvie->Pscen()->Pbkgd()->Cno();
			AssertDo(_pmvie->Pscen()->FGetTagBkgd(&tag), "Missing background event");
			sid = tag.sid;
			}

		pbrwd = (PBRWD)(BRWB::PbrwbNew(_pcrm));
		if (pvNil == pbrwd)
			goto LFail;

		// Create BRCNL for context carryover (optional choice)
		if (pbrcn == pvNil)
			pbrcn = NewObj BRCNL;

		// Selection is cno based
		if (!((PBRWB)pbrwd)->FInit(pcmd, kbwsCnoRoot, thumSelect,
			sid, ckiRoot, ctgNil, this, (PBRCNL)pbrcn))
			{
			goto LFail;
			}
		break;

	case kidBrwsCamera:
		if (pvNil == _pmvie->Pscen())
			goto LFail;
		AssertPo(_pmvie->Pscen(), 0);
		Assert(pvNil != _pmvie->Pscen()->Pbkgd(), "Pbkgd() Nil");

		// Search for camera views, children of the current bkgd
		ckiRoot.ctg = kctgBkth;
		ckiRoot.cno = _pmvie->Pscen()->Pbkgd()->Cno();

		thumSelect = _pmvie->Pscen()->Pbkgd()->Icam();
		pbrwd = (PBRWD)(BRWC::PbrwcNew(_pcrm));
 		if (pvNil == pbrwd)
			goto LFail;

		// Create BRCNL for context carryover (optional choice)
		if (pbrcn == pvNil)
			pbrcn = NewObj BRCNL;
		AssertDo(_pmvie->Pscen()->FGetTagBkgd(&tag), "Missing background event");
		if (!((PBRWC)pbrwd)->FInit(pcmd, kbwsChid, thumSelect,
			tag.sid, ckiRoot, kctgCath, this, (PBRCNL)pbrcn))
			{
			goto LFail;
			}
		break;

	case kidBrwsProp:
		ckiRoot.ctg = kctgPrth;
		pbrwd = (PBRWD)(BRWP::PbrwpNew(_pcrm, kidPropGlass));
		goto LActor;
	case kidBrwsActor:
		ckiRoot.ctg = kctgTmth;
		pbrwd = (PBRWD)(BRWP::PbrwpNew(_pcrm, kidActorGlass));
LActor:
		ckiRoot.cno = cnoNil;
		Assert(pvNil != _pmvie->Pscen(), "Actor browser requires scene");
		thumSelect = ivNil;
		if (pvNil == pbrwd)
			goto LFail;

		// Create BRCNL for context carryover (optional choice)
		if (pbrcn == pvNil)
			pbrcn = NewObj BRCNL;
		if (!((PBRWP)pbrwd)->FInit(pcmd, kbwsCnoRoot, thumSelect,
			0, ckiRoot, ctgNil, this, (PBRCNL)pbrcn))
			{
			goto LFail;
			}
		break;

	case kidBrwsAction:
		if ((pvNil == _pmvie->Pscen()) ||
			(pvNil == _pmvie->Pscen()->PactrSelected()))
			{
			Bug("No actor selected in action browser");
			goto LFail;
			}
		thumSelect = _pmvie->Pscen()->PactrSelected()->AnidCur();
		pbrwd = (PBRWD)BRWA::PbrwaNew(_pcrm);

		if (pvNil == pbrwd)
			goto LFail;
	
		// Build the string table before initializing
		if (!((PBRWA)pbrwd)->FBuildGst(_pmvie->Pscen()))
			goto LFail;
		if (!((PBRWT)pbrwd)->FInit(pcmd, thumSelect, thumSelect, this))
			goto LFail;
		break;

	case kidSSorterBackground:
		if (SCRT::PscrtNew(brwdid, _pmvie, this, _pcrm) == pvNil)
			PushErc(ercSocCantInitSceneSort);
		vapp.EndLongOp();
		return fTrue;

	case kidBrwsFX:
		ckiRoot.ctg = kctgSfth;
		pbrwd = (PBRWD)(BRWM::PbrwmNew(_pcrm, kidFXGlass, stySfx, this));
		goto LMusic;
   	case kidBrwsSpeech:
		ckiRoot.ctg = kctgSvth;
		pbrwd = (PBRWD)(BRWM::PbrwmNew(_pcrm, kidSpeechGlass, stySpeech, this));
		goto LMusic;
	case kidBrwsMidi:
		ckiRoot.ctg = kctgSmth;
		pbrwd = (PBRWD)(BRWM::PbrwmNew(_pcrm, kidMidiGlass, styMidi, this));
LMusic:
		if (pvNil == pbrwd)
			goto LFail;

		// Search for background thumbs of any cno
		ckiRoot.cno = cnoNil;
		thumSelect = (long)cnoNil;

		// Create BRCNL for context carryover (optional choice)
		if (pbrcn == pvNil)
			pbrcn = NewObj BRCNL;

		pmvu = (PMVU)(Pmvie()->PddgGet(0));
		ptag = pmvu->PtagTool();
		if (ptag->sid != ksidInvalid)
			{
			thumSelect = ptag->cno;
			sid = ptag->sid;
			}
		// Selection is cno based
		if (!((PBRWM)pbrwd)->FInit(pcmd, kbwsCnoRoot, thumSelect, sid,
			ckiRoot, ctgNil, this, (PBRCNL)pbrcn))
			{
			goto LFail;
			}
		break;

	//
	// The import browser is set up on top of the normal sound browser
	// rglw[1] = pfniMovie of movie to be scanned.
	//
	case kidBrwsImportFX:
		ckiRoot.ctg = kctgSfth;
		pbrwd = (PBRWD)(BRWI::PbrwiNew(_pcrm, kidSoundsImportGlass, stySfx));
		goto LImport;
	case kidBrwsImportSpeech:
		ckiRoot.ctg = kctgSvth;
		pbrwd = (PBRWD)(BRWI::PbrwiNew(_pcrm, kidSoundsImportGlass, stySpeech));
		goto LImport;
	case kidBrwsImportMidi:
		ckiRoot.ctg = kctgSmth;
		pbrwd = (PBRWD)(BRWI::PbrwiNew(_pcrm, kidSoundsImportGlass, styMidi));
LImport:
		if (pvNil == pbrwd)
			goto LFail;
		// Build the string table before initializing the BRWD
		if (!((PBRWI)pbrwd)->FInit(pcmd, ckiRoot, this))
			{
			goto LFail;
			}
		break;

	case kidRollCallProp:
		Assert(pvNil == _pbrwrProp, "Roll Call browser already up");
		_pbrwrProp = BRWR::PbrwrNew(_pcrm, kidRollCallProp);
		pbrwd = (PBRWD)_pbrwrProp;
		if (pvNil == pbrwd)
			goto LFail;
	 	
	 	// Create the cno map from tmpl-->gokd
		if (_pglcmg == pvNil)
			{
			if (pvNil == (_pglcmg = GL::PglNew(size(CMG), kglcmgGrow)))
				goto LFail;			
			_pglcmg->SetMinGrow(kglcmgGrow);
			}

 		if (!_pbrwrProp->FInit(pcmd, kctgPrth, ivNil, this))
			goto LFail;
		break;

	case kidRollCallActor:
		Assert(pvNil == _pbrwrActr, "Roll Call browser already up");
		_pbrwrActr = BRWR::PbrwrNew(_pcrm, kidRollCallActor);
		pbrwd = (PBRWD)_pbrwrActr;
		if (pvNil == pbrwd)
			goto LFail;

	 	// Create the cno map from tmpl-->gokd
		if (_pglcmg == pvNil)
			{
			if (pvNil == (_pglcmg = GL::PglNew(size(CMG), kglcmgGrow)))
				goto LFail;			
			_pglcmg->SetMinGrow(kglcmgGrow);
			}

		if (!_pbrwrActr->FInit(pcmd, kctgTmth, ivNil, this))
			goto LFail;
		break;


	default:
		RawRtn();
		break;
		}

	Assert(pvNil != pbrwd, "Logic error");
	pbrwd->FDraw(); 	// Ignore failure : reported elsewhere

	//
	// Optionally Add new browser to the gl for context
	// carryover between browser instantiations
	//
	if (pvNil != pbrcn && pvNil == _PbrcnFromBrwdid(brwdid))
		{
		if (!_pglpbrcn->FAdd(&pbrcn))
			{
			goto LFail;
			}
		}

	fSuccess = fTrue;
LFail:
	if (!fSuccess)
		{
		ReleasePpo(&pbrcn);
		ReleasePpo(&pbrwd);
		}
	vpcex->EnqueueCid(cidBrowserVisible, pvNil, pvNil, fSuccess ? 1 : 0);		// For projects
	vapp.EndLongOp();
	return fTrue;
}



/***************************************************************************
 *
 * Destroy browser context (when Studio destructs)
 *
 **************************************************************************/
void STDIO::ReleaseBrcn(void)
{
	long ipbrcn;
	PBRCN pbrcn;

	if (pvNil == _pglpbrcn)
		return;

	for (ipbrcn = 0; ipbrcn < _pglpbrcn->IvMac(); ipbrcn++)
		{
		_pglpbrcn->Get(ipbrcn, &pbrcn);
		ReleasePpo(&pbrcn);
		}
	ReleasePpo(&_pglpbrcn);
}


/***************************************************************************
 *
 * Locate a browser pbrwd
 *
 **************************************************************************/
PBRCN STDIO::_PbrcnFromBrwdid(long brwdid)
{
	AssertThis(0);
	long ipbrcn;
	PBRCN pbrcn;

	for (ipbrcn = 0; ipbrcn < _pglpbrcn->IvMac(); ipbrcn++)
		{
		_pglpbrcn->Get(ipbrcn, &pbrcn);
		if (pbrcn->brwdid == brwdid)
			{
			return pbrcn;
			}	
		}
	return pvNil;
}


/***************************************************************************
 *
 * Apply a Camera Selection.	(Browser callback)
 * thumSelect is an index and a chid
 *
 **************************************************************************/
void BRWC::_ApplySelection(long thumSelect, long sid)
{
	AssertThis(0);

	PMVU pmvu;

	_pstdio->Pmvie()->Pscen()->FChangeCam(thumSelect);

 	// Update the tool
 	pmvu = (PMVU)(_pstdio->Pmvie()->PddgActive());
 	AssertPo(pmvu, 0);
 	pmvu->SetTool(toolDefault);

 	// Update the UI
 	_pstdio->Pmvie()->Pmcc()->ChangeTool(toolDefault);

}


/***************************************************************************
 *
 * Apply a Background Selection.	(Browser callback)
 * thumSelect is a cno
 *
 **************************************************************************/
void BRWB::_ApplySelection(long thumSelect, long sid)
{
	AssertThis(0);

	TAG tag;
	CMD cmd;
	PMVU pmvu;

	tag.sid = sid;
	tag.pcrf = pvNil;
	tag.ctg = kctgBkgd;
	tag.cno = (CNO)thumSelect;

	ClearPb(&cmd, size(cmd));
	cmd.cid = cidNewScene;
	cmd.pcmh = _pstdio;
	Assert(size(TAG) <= size(cmd.rglw), "Insufficient space in rglw");
	*((PTAG)&cmd.rglw) = tag;

	vpcex->EnqueueCmd(&cmd);

 	// Update the tool
 	pmvu = (PMVU)(_pstdio->Pmvie()->PddgActive());
 	AssertPo(pmvu, 0);
 	pmvu->SetTool(toolDefault);

 	// Update the UI
 	_pstdio->Pmvie()->Pmcc()->ChangeTool(toolDefault);

	return;
}


/***************************************************************************
 *
 * Apply an Actor Selection.
 *
 * thumSelect is a cno
 *
 *
 **************************************************************************/
void BRWP::_ApplySelection(long thumSelect, long sid)
{
	AssertThis(0);

	TAG tag;
	PMVU pmvu;

	pmvu = (PMVU)(_pstdio->Pmvie()->PddgGet(0));
	if (pmvu == pvNil)
 		{
		Warn("No pmvu");
		return;
		}

	AssertPo(pmvu, 0);
	tag.sid = sid;
	tag.pcrf = pvNil;
	tag.ctg = kctgTmpl;
	tag.cno = (CNO)thumSelect;

	if (!_pstdio->Pmvie()->FInsActr(&tag))
		goto LFail;

	pmvu->StartPlaceActor(fTrue);

LFail:
	return;
}


/***************************************************************************
 *
 * Apply an Action Selection.
 *
 * thumSelect is a chid
 *
 **************************************************************************/
void BRWA::_ApplySelection(long thumSelect, long sid)
{
	AssertThis(0);
	AssertPo(_pstdio->Pmvie(), 0);

	PACTR pactr;
	PMVU pmvu;
	PGOK pgok;

 	// Apply the action to the actor
 	pactr = _pstdio->Pmvie()->Pscen()->PactrSelected();
 	if (!pactr->FSetAction(thumSelect, _celnStart, fFalse))
 		return;	 	// Error reported earlier

 	// Update the tool
 	pmvu = (PMVU)(_pstdio->Pmvie()->PddgActive());
 	AssertPo(pmvu, 0);
 	pmvu->SetTool(toolRecordSameAction);

 	// Update the UI
 	_pstdio->Pmvie()->Pmcc()->ChangeTool(toolRecordSameAction);

	// Reset the studio action button state	(record will be depressed)
	pgok = (PGOK)vapp.Pkwa()->PgobFromHid(kidActorsActionBrowser);

	if ((pgok != pvNil) && pgok->FIs(kclsGOK))
		{
		Assert(pgok->FIs(kclsGOK), "Invalid class");
		pgok->FChangeState(kstDefault);
		}					

 	return;
}


/***************************************************************************
 *
 * Apply a Music Selection.
 *
 * thumSelect is a cnoContent
 *
 **************************************************************************/
void BRWM::_ApplySelection(long thumSelect, long sid)
{
	AssertThis(0);
	AssertPo(_pstdio->Pmvie(), 0);

	PGOK pgok;
	PMVU pmvu;
	TAG tag;
	BOOL fClick = fTrue;

	pmvu = (PMVU)(_pstdio->Pmvie()->PddgGet(0));
 	AssertPo(pmvu, 0);
	
	tag.ctg = kctgMsnd;
	tag.cno = (CNO)thumSelect;
	tag.sid = sid;
	if (ksidUseCrf != sid)
		tag.pcrf = pvNil;
	else
		{
		if (!_pstdio->Pmvie()->FEnsureAutosave(&_pcrf))
			return;
		AssertDo(vptagm->FOpenTag(&tag, _pcrf),
			"Should never fail when not copying the tag");
		}

	// Set the tool to "play once", if necessary
	pgok = (PGOK)vpapp->Pkwa()->PgobFromHid(kidSoundsLooping);
	if (pgok != pvNil && pgok->FIs(kclsGOK) && (pgok->Sno() == kstSelected))
		{
		fClick = fFalse;
		}

	pgok = (PGOK)vpapp->Pkwa()->PgobFromHid(kidSoundsAttachToCell);
	if (pgok != pvNil && pgok->FIs(kclsGOK) && (pgok->Sno() == kstSelected) && (_sty != styMidi))
		{
		fClick = fFalse;
		}

	if (fClick)
		{
		pgok = (PGOK)vpapp->Pkwa()->PgobFromHid(kidSoundsPlayOnce);
		if (pgok != pvNil && pgok->FIs(kclsGOK) && (pgok->Sno() != kstSelected))
			{
			AssertPo(pgok, 0);
			vpcex->EnqueueCid(cidClicked, pgok, pvNil, pvNil);		
			}
		}

	pmvu->SetTagTool(&tag);	
	TAGM::CloseTag(&tag);																		
	return;
}


/***************************************************************************
 *
 * Apply an Import Music Selection.
 * Copy the msnd chunk from the open BRWI movie to the current movie
 * Then notify the underlying sound browser to update
 *
 * thumSelect is a cnoContent
 *
 **************************************************************************/
void BRWI::_ApplySelection(long thumSelect, long sid)
{
	AssertThis(0);
	AssertPo(_pstdio->Pmvie(), 0);
	CNO cnoDest;
	long kidBrws;

	switch (_sty)
		{
	case styMidi:
		kidBrws = kidMidiGlass;
		break;
	case stySfx:
		kidBrws = kidFXGlass;
		break;
	case stySpeech:
		kidBrws = kidSpeechGlass;
		break;
	default:
		Bug("Invalid _sty");
		break;
		}

	if (pvNil == _pcrf || pvNil == _pcrf->Pcfl())
		return;

	// Copy	sound from _pcrf->Pcfl() to current movie
	vpappb->BeginLongOp();
	if (!_pstdio->Pmvie()->FCopyMsndFromPcfl(_pcrf->Pcfl(), (CNO)thumSelect, &cnoDest))
		{
		vpappb->EndLongOp();
		return;
		}
	vpappb->EndLongOp();

	// Select the item, extend lists and hilite it
	vpcex->EnqueueCid(cidBrowserSelectThum, vpappb->PcmhFromHid(kidBrws),
		pvNil, cnoDest, sid, 1, 1);

	return;
}


/***************************************************************************
 *
 * Apply a Roll Call Selection.	
 * thumSelect equals ithum
 *
 **************************************************************************/
void BRWR::_ApplySelection(long thumSelect, long sid)
{
	AssertThis(0);

	PMVU pmvu;
	PMVIE pmvie = _pstdio->Pmvie();
	long arid;
	STN stn;
	long cactRef;

	long iarid = _IaridFromIthum(thumSelect);
	if (!pmvie->FGetArid(iarid, &arid, &stn, &cactRef))
		return;
	
	_fApplyingSel = fTrue;
	pmvu = (PMVU)pmvie->PddgActive();
	pmvie->FChooseArid(arid);
	if (!pmvu->FActrMode())
		{
		pmvu->SetTool(toolCompose);
		_pstdio->ChangeTool(toolCompose);
		}
	_fApplyingSel = fFalse;
}
