/***************************************************************************

  movie.cpp

  Author: ******

  Date: August, 1994

  This file contains all functionality for movie manipulation.

  THIS IS A CODE REVIEWED FILE

	Basic movie private classes:

		Movie Scene actions Undo Object (MUNS)

			BASE ---> UNDB ---> MUNB ---> MUNS


Note: The client of the movie engine should always do all actions through
MVIE level APIs, it should never use accessor functions to maniplate Scenes,
Actors, Text boxes, etc.

***************************************************************************/

#include "soc.h"
ASSERTNAME

#define krScaleMouseRecord BR_SCALAR(0.20)
#define krScaleMouseNonRecord BR_SCALAR(0.02)
#define kdwrMousePerSecond BR_SCALAR(100.0) // "mouse drag" speed for arrow keys

#define kdtsFrame (kdtsSecond / kfps)	// milliseconds per frame
#define kdtimFrame (kdtimSecond / kfps) // clock ticks per frame
#define kdtsCycleCels (kdtsFrame * 3) 	// delay when cycling cels
#define kdtsVlmFade 3					// number of seconds to fade
#define kdtimVlmFade (kdtimSecond/4)	// number of clock ticks necessary to split 1 sec into 4 events
#define kzrMouseScalingFactor BR_SCALAR(100.0)

const CHID kchidGstSource = 1;

//
// How many pixels from edge to warp cursor back to center
//
const long kdpInset = 50;

//
// Mouse scaling factor when rotating
//
const long krRotateScaleFactor = BR_SCALAR(0.001);

//
// Mouse scaling factor when sooner/latering.
//
const long krSoonerScaleFactor = BR_SCALAR(0.05);

//
// Number of ticks to pass before scrolling a single pixel
//
#define kdtsScrolling 5

//
//
// UNDO object for scene related actions:  Ins, New, and Rem
//
//
typedef class MUNS *PMUNS;

#define MUNS_PAR MUNB

enum MUNST
	{
    munstInsScen,
	munstRemScen,
	munstSetBkgd
	};

#define kclsMUNS 'MUNS'
class MUNS : public MUNS_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	long _iscen;
	TAG _tag;
	PSCEN _pscen;
	MUNST _munst;
	MUNS(void) { }

public:

	static PMUNS PmunsNew(void);
	~MUNS(void);

	void SetIscen(long iscen) { _iscen = iscen; }
	void SetPscen(PSCEN pscen) { _pscen = pscen; _pscen->AddRef(); }
	void SetMunst(MUNST munst) { _munst = munst; }
	void SetTag(PTAG ptag) { _tag = *ptag; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};



//
//
//
//  BEGIN MOVIE
//
//
//

RTCLASS(MVIE)
RTCLASS(MUNB)
RTCLASS(MUNS)


BEGIN_CMD_MAP(MVIE, CMH)
	ON_CID_ME(cidAlarm, &MVIE::FCmdAlarm, pvNil)
	ON_CID_ME(cidRender, &MVIE::FCmdRender, pvNil)
	ON_CID_GEN(cidSaveAndClose, pvNil, pvNil)
END_CMD_MAP_NIL()


//
// A file written by this version of movie.cpp receives this cvn.  Any
// file with this cvn value has exactly the same file format
//
const short kcvnCur = 2;

//
// A file written by this version of movie.cpp can be read by any version
// of movie.cpp whose kcvnCur is >= to this (this should be <= kcvnCur)
//
const short kcvnBack = 2;

//
// A file whose cvn is less than kcvnMin cannot be directly read by
// this version of movie.cpp (maybe a converter will read it).
// (this should be <= kcvnCur)
//
const short kcvnMin = 1;

//
// Movie file prefix
//
struct MFP
	{
	short bo;		// byte order
	short osk;		// which system wrote this
	DVER dver;		// chunky file version
	};
const BOM kbomMfp = 0x55000000;



//
// Used to keep track of the roll call list of the movie
//
struct MACTR
	{
	long arid;
	long cactRef;
	ulong grfbrws;  // browser properties
	TAG tagTmpl;
	};

typedef MACTR *PMACTR;

const BOM kbomMactr = (0xFC000000 | (kbomTag >> 4));


/****************************************************
 *
 * Constructor for movies.  This function is private, use PmvieNew()
 * for public construction.
 *
 ****************************************************/
MVIE::MVIE(void) : _clok(khidMvieClock)
{
	_aridLim = 0;
	_cno = cnoNil;
	_iscen = ivNil;
	_wit = witNil;
	_trans = transNil;
	_vlmOrg = 0;

	SetCundbMax(1);
}


/******************************************************************************
	_FSetPfilSave
		Given an FNI, looks for and remembers if found the FIL associated with
		it.  If the FIL was found, will also check to see if it's read-only.

	Returns:
		fFalse if the FIL wasn't found.
	
************************************************************ PETED ***********/
bool MVIE::_FSetPfilSave(PFNI pfni)
{
	AssertBaseThis(0);
	AssertPo(pfni, 0);

	long lAttrib;
	STN stnFile;

	/* Look for the file and remember FIL if found */
	ReleasePpo(&_pfilSave);
	_pfilSave = FIL::PfilFromFni(pfni);
	if (_pfilSave == pvNil)
		return fFalse;
	_pfilSave->AddRef();
	_fFniSaveValid = fTrue;

	/* Remember whether FIL is read-only; only relevant if we actually found
		the FIL, since if we didn't, we'll prompt for a new filename later
		anyway */
	pfni->GetStnPath(&stnFile);
#ifdef WIN
	_fReadOnly =
		(((lAttrib = GetFileAttributes(stnFile.Psz())) != 0xFFFFFFFF)
		&& (lAttrib & FILE_ATTRIBUTE_READONLY));
#else //MAC
	RawRtn();
#endif
	return fTrue;
}


/****************************************************
 *
 * Reads a movie from a file.
 *
 * Parameters:
 *  pmcc - Pointer to the movie client class block to use.
 *	pfni - File to read from.
 *	cno - CNO of the movie chunk, cnoNil if using the
 *		the first one in the file.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie object.
 *
 ****************************************************/
PMVIE MVIE::PmvieNew(bool fHalfMode, PMCC pmcc, FNI *pfni, CNO cno)
{
	AssertNilOrPo(pfni, 0);
	AssertPo(pmcc, 0);

	bool fSuccess = fFalse, fBeganLongOp = fFalse;
	PMVIE pmvie;
	KID kid;
	CHID chid;
	TAGL *ptagl;
	PCFL pcfl = pvNil;
	BLCK blck;
	short bo;
	short osk;
	PGST pgstSource;

	//
	// Create the movie object
	//
	pmvie = NewObj MVIE;
	if (pmvie == pvNil)
		{
		goto LFail;
	    }

	//
	// Create the GL for holding undo events
	//
	pmvie->_pglpundb = GL::PglNew(size(PUNDB), 1);
	if (pmvie->_pglpundb == pvNil)
		{
		goto LFail;
	    }

    //
	// Create GL of actors in the movie
	//
	if (pvNil == pfni)
		{
		pmvie->_pgstmactr = GST::PgstNew(size(MACTR));
		if (pmvie->_pgstmactr == pvNil)
			{
			goto LFail;
		    }

		}

    //
	// Create the brender world
	//
	pmvie->_pbwld = BWLD::PbwldNew(pmcc->Dxp(), pmcc->Dyp(), fFalse, fHalfMode);
	if (pvNil == pmvie->_pbwld)
		{
		goto LFail;
		}

    //
	// Create the movie sound queue
	//
	pmvie->_pmsq = MSQ::PmsqNew();
	if (pvNil == pmvie->_pmsq)
		{
		goto LFail;
		}

	//
	// Save other variables
	//
	pmvie->_pmcc = pmcc;
	pmcc->AddRef();

	//
	// Do we initialize from a file?
	//
	if (pfni == pvNil)
		{
		return(pmvie);
		}

	/* Don't bother putting up the wait cursor unless we're reading a movie */
	vpappb->BeginLongOp();
	fBeganLongOp = fTrue;

	//
	// Get file to read from
	//
	pcfl = CFL::PcflOpen(pfni, fcflNil);
	if (pcfl == pvNil)
		{
		goto LFail;
		}

	if (!pmvie->FVerifyVersion(pcfl, &cno))
	  	goto LFail;
	pmvie->_cno = cno;

    //
	// Keep a reference to the user's file
	//
	if (!pmvie->_FSetPfilSave(pfni))
		{
		Bug("Hey, we just opened this file!");
		goto LFail;
		}

	//
	// Note (by manders): CRF *must* have 0 cache size, because of
	// serious cache-coherency problems otherwise.  TMPL data is not
	// read-only, and chunk numbers change over time.
	//
	pmvie->_pcrfAutoSave = CRF::PcrfNew(pcfl, 0);  // cache size must be 0
	if (pvNil == pmvie->_pcrfAutoSave)
		{
		goto LFail;
		}

	//
	// Merge this document's source title list
	//
	if (pcfl->FGetKidChidCtg(kctgMvie, cno, kchidGstSource, kctgGst,
		&kid) && pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		{
		pgstSource = GST::PgstRead(&blck, &bo, &osk);
		if (pvNil != pgstSource)
			{
			// Ignore result...we can survive failure
			vptagm->FMergeGstSource(pgstSource, bo, osk);
			ReleasePpo(&pgstSource);
			}
		}

    //
	// Get the movie roll-call
	//
	Assert(pcfl == pmvie->_pcrfAutoSave->Pcfl(), "pcfl isn't right");
	if (!FReadRollCall(pmvie->_pcrfAutoSave, cno, &pmvie->_pgstmactr, &pmvie->_aridLim))
		{
		pmvie->_pgstmactr = pvNil;
		goto LFail;
		}

	//
	// Get all the content tags
	//
	ptagl = pmvie->_PtaglFetch();
	if (ptagl == pvNil)
		{
		goto LFail;
		}

    //
	// Now bring all the tags into cache
	//
	if (!ptagl->FCacheTags())
		{
		ReleasePpo(&ptagl);
		goto LFail;
		}

	ReleasePpo(&ptagl);

	//
	// Set the movie title
	//
	pmvie->_SetTitle(pfni);

	//
	// Count the number of scenes in the movie
	//
	for (chid = 0;
		 pcfl->FGetKidChidCtg(kctgMvie, cno, chid, kctgScen, &kid);
		 chid++, pmvie->_cscen++
		)
		{
		}

	fSuccess = fTrue;

LFail:
	ReleasePpo(&pcfl);
	if (!fSuccess)
		ReleasePpo(&pmvie);
	if (fBeganLongOp)
		vpappb->EndLongOp();

	return(pmvie);
}


/******************************************************************************
	FReadRollCall
		Reads the roll call off file for a given movie.  Will swapbytes the
		extra data in the GST if necessary, and will report back on the
		highest arid found.
	
	Arguments:
		PCFL pcfl       -- the file the movie is on
		PCRF pcrf       -- the autosave CRF for the movie's ACTR tags
		CNO cno         -- the cno of the movie
		PGST *ppgst     -- the PGST to fill in
		long *paridLim  -- the max arid to update
	
	Returns: fTrue if there were no failures, fFalse otherwise
	
************************************************************ PETED ***********/
bool MVIE::FReadRollCall(PCRF pcrf, CNO cno, PGST *ppgst, long *paridLim)
{
	AssertPo(pcrf, 0);
	AssertVarMem(ppgst);
	Assert(*ppgst == pvNil, "Overwriting existing GST");
	AssertNilOrVarMem(paridLim);

	short bo;
	long imactr, imactrMac;
	PCFL pcfl = pcrf->Pcfl();
	KID kid;
	BLCK blck;
	MACTR mactr;

	if (!pcfl->FGetKidChidCtg(kctgMvie, cno, 0, kctgGst, &kid) ||
		!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		{
        PushErc(ercSocBadFile);
		goto LFail;
		}

	*ppgst = GST::PgstRead(&blck, &bo);
	if (*ppgst == pvNil)
		goto LFail;

	imactrMac = (*ppgst)->IvMac();
	for (imactr = 0; imactr < imactrMac; imactr++)
		{
		(*ppgst)->GetExtra(imactr, &mactr);
		if (bo == kboOther)
			SwapBytesBom(&mactr, kbomMactr);

		if (paridLim != pvNil && mactr.arid >= *paridLim)
			*paridLim = mactr.arid + 1;

		// Open the tags, since they might be TDTs
		AssertDo(vptagm->FOpenTag(&mactr.tagTmpl, pcrf),
			"Should never fail when not copying the tag");

		(*ppgst)->PutExtra(imactr, &mactr);
		}

	return fTrue;
LFail:
	TrashVar(ppgst);
	return fFalse;
}


/******************************************************************************
	Flush
		Ensures that the data has been written to disk.
	
************************************************************ PETED ***********/
void MVIE::Flush(void)
{
	if (_fFniSaveValid)
		{
		AssertPo(_pfilSave, 0);
		_pfilSave->Flush();
		}
}


/****************************************************
 *
 * Nuke unused sounds from the file
 * Msnd chunks exist as children of the movie chunk.
 * They are also children of any scene which uses
 * them.
 * Note: The ref count does not reflect how many
 * scene or actor events reference the sound.
 *
 * Parms:
 *    bool fPurgeAll -- if fFalse, only purge invalid sounds
 *
 ****************************************************/
void MVIE::_DoSndGarbageCollection(bool fPurgeAll)
{
	AssertThis(0);

	// Before releasing, get rid of all msnd chunks with
	// cactref == 1 (the refcnt from being a child of the
	// movie chunk)

	long ikid;

	if (pvNil == _pcrfAutoSave)
		return;

	PCFL pcfl = _pcrfAutoSave->Pcfl();
	if (pvNil == pcfl)
		return;

	/* Go backwards, since we might delete some */
	ikid = pcfl->Ckid(kctgMvie, _cno);
	while (ikid--)
		{
		KID kid;
		KID kidT;

		if (!pcfl->FGetKid(kctgMvie, _cno, ikid, &kid))
			{
			Bug("CFL returned bogus Ckid()");
			break;
			}

		if (kid.cki.ctg != kctgMsnd)
			continue;

		if (pcfl->CckiRef(kctgMsnd, kid.cki.cno) > 1)
			continue;

		/* Always purge MSNDs with no actual sound attached */
		if (fPurgeAll || !pcfl->FGetKidChid(kctgMsnd, kid.cki.cno,
				kchidSnd, &kidT))
			{
			pcfl->DeleteChild(kctgMvie, _cno, kctgMsnd, kid.cki.cno, kid.chid);
			}
		}

	return;
}

/****************************************************
 *
 * Destructor for movies.
 *
 ****************************************************/
MVIE::~MVIE(void)
{
	AssertBaseThis(0);

	long imactr;
	MACTR mactr;

	ReleasePpo(&_pcrfAutoSave);
	ReleasePpo(&_pfilSave);

	if (FPlaying())
		{
		AssertPo(Pmcc(), 0);
		Pmcc()->EnableAccel();
		Pmcc()->PlayStopped();
		vpsndm->StopAll();

		// if we were fading, then restore sound volume
		if (_vlmOrg)
			{
			vpsndm->SetVlm(_vlmOrg);
			_vlmOrg = 0;
			}

		}

	if (Pmcc() != pvNil)
		{
		Pmcc()->UpdateRollCall();
		}

	if (Pscen() != pvNil)
		{

    	//
    	// Release open scene.
    	//
		SCEN::Close(&_pscenOpen);
		}

    //
	// Release the roll call
	//
	if (_pgstmactr != pvNil)
		{
		for (imactr = 0; imactr < _pgstmactr->IvMac(); imactr++)
			{
			_pgstmactr->GetExtra(imactr, &mactr);
			vptagm->CloseTag(&mactr.tagTmpl);
			}
		ReleasePpo(&_pgstmactr);
		}

	//
	// Release the call back class
	//
	ReleasePpo(&_pmcc);

	//
	// Release the brender world
	//
	ReleasePpo(&_pbwld);

	//
	// Release the sound queue
	//
	ReleasePpo(&_pmsq);

	ReleasePpo(&_pglclrThumbPalette);

    return;
}


#ifdef DEBUG

/****************************************************
 * Mark memory used by the MVIE
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void MVIE::MarkMem(void)
{
	AssertThis(0);

	MVIE_PAR::MarkMem();

	MarkMemObj(_pcrfAutoSave);

	MarkMemObj(_pfilSave);

	MarkMemObj(_pgstmactr);

	MarkMemObj(Pscen());

	MarkMemObj(Pbwld());

	MarkMemObj(_pmcc);

	MarkMemObj(_pmsq);

	MarkMemObj(_pglclrThumbPalette);
}

/***************************************************************************
 *
 * Assert the validity of the MVIE.
 *
 * Parameters:
 *	grf - Bit field of options
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVIE::AssertValid(ulong grf)
{
	MVIE_PAR::AssertValid(fobjAllocated);

	AssertNilOrPo(_pcrfAutoSave, 0);
	AssertPo(_pgstmactr, 0);
	AssertNilOrPo(Pbwld(), 0);
	AssertPo(&_clok, 0);
	AssertPo(_pmcc, 0);
}

#endif //DEBUG


/***************************************************************************
 *
 * Returns a list of all tags being used by this MVIE
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  A TAGL (list of tags that the movie uses)
 *
 **************************************************************************/
PTAGL MVIE::_PtaglFetch(void)
{
	AssertThis(0);
	Assert(_pcrfAutoSave != pvNil, "need pcrfAutosave");

	PTAGL ptagl;
	KID kid;
	CHID chid;

	ptagl = TAGL::PtaglNew();
	if (pvNil == ptagl)
		return pvNil;

	//
	// Add each scene's tags
	//
	for (chid = 0;
		 _pcrfAutoSave->Pcfl()->FGetKidChidCtg(kctgMvie, _cno, chid, kctgScen, &kid);
		 chid++)
		{
		if (!SCEN::FAddTagsToTagl(_pcrfAutoSave->Pcfl(), kid.cki.cno, ptagl))
			{
			ReleasePpo(&ptagl);
			return pvNil;
			}
		}

	return ptagl;
}


/****************************************************
 *
 * Fetches the iarid'th actor in the movie.
 *
 * Parameters:
 *   iarid - The arid index to fetch.
 *   parid - Pointer to storage for the found arid.
 *	 pstn  - Pointer to the actors name.
 *	 pcactRef - Number of scenes the actor is in.
 *	 *ptagTmpl - Template tag
 * Returns:
 *	 fTrue if successful, else fFalse if out of range.
 *	
 ****************************************************/
bool MVIE::FGetArid(long iarid, long *parid, PSTN pstn, long *pcactRef,
	PTAG ptagTmpl)
{
	AssertThis(0);
	AssertPvCb(parid, size(long));
	AssertVarMem(pcactRef);

	MACTR mactr;
	
	if (iarid < 0 || iarid >= _pgstmactr->IvMac())
		{
		return fFalse;
		}

	_pgstmactr->GetStn(iarid, pstn);
	_pgstmactr->GetExtra(iarid, &mactr);
	*parid = mactr.arid;
	*pcactRef = mactr.cactRef;
	if (pvNil != ptagTmpl)
		*ptagTmpl = mactr.tagTmpl;
	return fTrue;
}


/****************************************************
 *
 * User chose arid in the roll call.  If actor
 * exists in this scene and is onstage, select it.
 * Else if actor is offstage, bring it onstage.
 * If actor is not in this scene, create and add it.
 *
 * Parameters:
 *	arid - The arid to search for, or create.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool MVIE::FChooseArid(long arid)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	PACTR pactr, pactrDup;
	MACTR mactr;
	long imactr;
	PMVU pmvu;

	pmvu = (PMVU)PddgGet(0);
	if (pmvu == pvNil)
 		{
		return(fFalse);
		}
	AssertPo(pmvu, 0);

	pactr = Pscen()->PactrFromArid(arid);

	if (pvNil != pactr)
		{

		if (!pactr->FIsInView())
			{

			if (!pactr->FDup(&pactrDup, fTrue))
				{
				return(fFalse);
				}
			AssertPo(pactrDup, 0);

			if (!pactr->FAddOnStageCore())
				{
                ReleasePpo(&pactrDup);
				return fFalse;
				}
			Pscen()->SelectActr(pactr);
			AssertDo(FAddToRollCall(pactr, pvNil), "Should never fail");
			pmvu->StartPlaceActor();
            pmvu->SetActrUndo(pactrDup);

			}

		Pscen()->SelectActr(pactr);
		InvalViews();
		return(fTrue);
		}
	else
		{

		//
		// Search roll call for actor, and create
		// a new actor for it.
		//
		for (imactr = 0; imactr < _pgstmactr->IvMac(); imactr++)
			{
	        _pgstmactr->GetExtra(imactr, &mactr);
			if (mactr.arid == arid)
				{
				pactr = ACTR::PactrNew(&(mactr.tagTmpl));

				if (pactr == pvNil)
					{
					return(fFalse);
					}

				AssertPo(pactr, 0);

				pactr->SetArid(arid);
				
					
				if (!Pscen()->FAddActr(pactr))
					{
					ReleasePpo(&pactr);
					return(fFalse);
					}

				pmvu->StartPlaceActor();
				ReleasePpo(&pactr);
				return(fTrue);
				}

			}

		}

	return(fFalse);
}


/****************************************************
 *
 * Return the arid of the selected actor.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   Arid of the selected actor, else aridNil.
 *
 ****************************************************/
long MVIE::AridSelected(void)
{
	AssertThis(0);

	if ((pvNil != Pscen()) && (pvNil != Pscen()->PactrSelected()))
		{
		return Pscen()->PactrSelected()->Arid();
		}
	else
		{
		return aridNil;
		}
}

/****************************************************
 *
 * Fetches the name of the actor with arid.
 *
 * Parameters:
 *   arid - The arid to fetch.
 *   pstn - Pointer to storage for the found name.
 *
 * Returns:
 *	 fTrue if successful, else fFalse if failure.
 *	
 ****************************************************/
bool MVIE::FGetName(long arid, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	MACTR mactr;
	long imactr;

	for (imactr = 0; imactr < _pgstmactr->IvMac(); imactr++)
		{
		_pgstmactr->GetExtra(imactr, &mactr);
		if (mactr.arid == arid)
			{
			_pgstmactr->GetStn(imactr, pstn);
			return(fTrue);
			}
		}	

	return(fFalse);
}



/****************************************************
 *
 * Sets the name of the actor with arid.
 *
 * Parameters:
 *   arid - The arid to set.
 *   pstn - Pointer to the name.
 *
 * Returns:
 *	 fTrue if successful, else fFalse.
 *	
 ****************************************************/
bool MVIE::FNameActr(long arid, PSTN pstn)
{
	AssertThis(0);
	AssertIn(arid, 0, 500);
	AssertPo(pstn, 0);

	MACTR mactr;
	long imactr;

	for (imactr = 0; imactr < _pgstmactr->IvMac(); imactr++)
		{

		_pgstmactr->GetExtra(imactr, &mactr);
		if (mactr.arid == arid)
			{

			if(_pgstmactr->FPutStn(imactr, pstn))
				{

				_pmcc->UpdateRollCall();	
				return(fTrue);

				}

			return(fFalse);

			}

		}	

	return(fFalse);
}

/****************************************************
 *
 * Identifies whether the mactr is in prop browser
 *
 * Parameters:
 *   imactr - index in _pgstmactr
 *
 * Returns:
 *	 bool
 *	
 ****************************************************/
bool MVIE::FIsPropBrwsIarid(long iarid)
{
	AssertThis(0);
	AssertIn(iarid, 0, _pgstmactr->IvMac());

	MACTR mactr;
	_pgstmactr->GetExtra(iarid, &mactr);
	return FPure(mactr.grfbrws & fbrwsProp);
}

/****************************************************
 *
 * Identifies whether the mactr is in 3dtext object
 *
 * Parameters:
 *   imactr - index in _pgstmactr
 *
 * Returns:
 *	 bool
 *	
 ****************************************************/
bool MVIE::FIsIaridTdt(long iarid)
{
	AssertThis(0);
	AssertIn(iarid, 0, _pgstmactr->IvMac());

	MACTR mactr;
	_pgstmactr->GetExtra(iarid, &mactr);
	return FPure(mactr.grfbrws & fbrwsTdt);
}


/****************************************************
 *
 * Sets the tag of the actor with arid.
 *
 * Parameters:
 *   arid - The arid to set.
 *   ptag - Pointer to the TMPL tag.
 *
 * Returns:
 *	 none
 *	
 ****************************************************/
void MVIE::ChangeActrTag(long arid, PTAG ptag)
{
	AssertThis(0);
	AssertIn(arid, 0, 500);
	AssertVarMem(ptag);

	MACTR mactr;
	long imactr;

	for (imactr = 0; imactr < _pgstmactr->IvMac(); imactr++)
		{

		_pgstmactr->GetExtra(imactr, &mactr);
		if (mactr.arid == arid)
			{

			mactr.tagTmpl = *ptag;
			_pgstmactr->PutExtra(imactr, &mactr);
			return;

			}

		}	

	Bug("no such actor");
}



/****************************************************
 *
 * This adds an actor to the roll call, if not already there, or
 * adds a reference count if it already exists.
 *
 * Parameters:
 *   Pointer to the actor to add.
 *
 * Returns:
 *   fTrue if successful, else fFalse indicating out of resources.
 *
 ****************************************************/
bool MVIE::FAddToRollCall(ACTR *pactr, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pactr, 0);
	AssertNilOrPo(pstn, 0);  // can be pvNil if the actor is already in the movie.

	MACTR mactr;
	long imactr;

	if (pactr->Arid() != aridNil)
		{
    	//
    	// Search for an actor with the same arid
    	//
    	for (imactr = 0; imactr < _pgstmactr->IvMac(); imactr++)
    		{

            _pgstmactr->GetExtra(imactr, &mactr);
    		if (mactr.arid == pactr->Arid())
    			{
				TAG tagTmpl;
				mactr.cactRef++;
				// TDTs sometimes need to update tagTmpl
				pactr->GetTagTmpl(&tagTmpl);
				if (fcmpEq != TAGM::FcmpCompareTags(&mactr.tagTmpl, &tagTmpl))
					{
					TAGM::CloseTag(&mactr.tagTmpl);
					mactr.tagTmpl = tagTmpl;
 					// ACTR::GetTagTmpl doesn't AddRef the pcrf, so do it here:
					TAGM::DupTag(&tagTmpl);
					}
				_pgstmactr->PutExtra(imactr, &mactr);
				Pmcc()->UpdateRollCall();
    			return(fTrue);
    			}

    		}

		}
	else
		{
		pactr->SetArid(_aridLim++);
		}

	AssertPo(pstn, 0);

    //
	// This is a new actor, add it to the roll call
	//
    mactr.arid = pactr->Arid();
	mactr.grfbrws = fbrwsNil;
	if (pactr->FIsPropBrws())
		{
		mactr.grfbrws |= fbrwsProp;
		if (pactr->FIsTdt())
			{
			mactr.grfbrws |= fbrwsTdt;
			}
		}
	mactr.cactRef = 1;
	pactr->GetTagTmpl(&mactr.tagTmpl);
	// Open the tag, since it might be a TDT
	AssertDo(vptagm->FOpenTag(&mactr.tagTmpl, _pcrfAutoSave),
		"Should never fail when not copying the tag");
	if (_pgstmactr->FAddStn(pstn, &mactr))
		{
		Pmcc()->UpdateRollCall();
		return(fTrue);
		}

	return(fFalse);
}

/****************************************************
 *
 * This removes an actor from the roll call, if the reference count
 * drops to zero, else it decrements the reference count.
 *
 * Parameters:
 *   Pointer to the actor to remove.
 *
 * Returns:
 *   None.
 *
 ****************************************************/
void MVIE::RemFromRollCall(ACTR *pactr, bool fDelIfOnlyRef)
{
	AssertThis(0);
	AssertPo(pactr, 0);

	MACTR mactr;
	long imactr;

	//
	// Search for the actor in the roll call
	//
	for (imactr = 0; imactr < _pgstmactr->IvMac(); imactr++)
		{
		_pgstmactr->GetExtra(imactr, &mactr);
		if (mactr.arid == pactr->Arid())
			{
            mactr.cactRef--;
			Assert(mactr.cactRef >= 0, "Too many removes");
			if (fDelIfOnlyRef && mactr.cactRef == 0)
				{
				vptagm->CloseTag(&mactr.tagTmpl);
				_pgstmactr->Delete(imactr);
				}
			else
				{
				_pgstmactr->PutExtra(imactr, &mactr);
				}

			if (mactr.cactRef == 0)
				{
				Pmcc()->UpdateRollCall();
				}
			return;

			}
		}

    Bug("Tried to remove an invalid actor");
}

/****************************************************
 *
 * Changes the scene in the movie currently being referenced.
 *
 * Parameters:
 *	iscen - Scene number to go to.
 *
 * Returns:
 *
 *  fTrue, if successful, else fFalse, indicating the
 *  same scene is still open (if possible).
 *
 ****************************************************/
bool MVIE::FSwitchScen(long iscen)
{
	AssertThis(0);
	Assert(iscen == ivNil || FIn(iscen, 0, Cscen()), "iscen out of range");
	Assert((iscen == ivNil) || (_pcrfAutoSave != pvNil), "Invalid save file");

	PSCEN pscen;
	KID kid;
	long iscenOld;
	bool fRet = fTrue;

	if (iscen == _iscen)
		{
		return(fTrue);
		}
		
	//
	// Close the current scene.
	//
	iscenOld = _iscen;
	if (!_FCloseCurrentScene())
		{
		return(fFalse);
		}

	//
	// Stop all looping non-midi sounds
	//
	vpsndm->StopAll(sqnNil, sclLoopWav);

	//
	// If memory is low, release unreferenced content BACOs to reduce thrashing.
	// Note that APP::MemStat() is unavailable from the movie engine
	//
#ifdef WIN
	MEMORYSTATUS ms;
	ms.dwLength = size(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);
	if (ms.dwMemoryLoad == 100)
		{
		// No physical RAM to spare, so free some stuff
		vptagm->ClearCache(sidNil, ftagmMemory);
		}

#endif //WIN

	if (iscen == ivNil)
		{
		Assert(_pscenOpen == pvNil, "_FCloseCurrentScene didn't clear this");
		Assert(_iscen == ivNil, "_FCloseCurrentScene didn't clear this");
		return(fTrue);
		}

LRetry:

    //
	// Get info for next scene and read it in.
	//
	AssertDo(_pcrfAutoSave->Pcfl()->FGetKidChidCtg(kctgMvie, _cno, iscen, kctgScen, &kid),
			 "Should never fail");

	pscen = SCEN::PscenRead(this, _pcrfAutoSave, kid.cki.cno);

	if ((pscen == pvNil) || !pscen->FPlayStartEvents())
		{
		_pscenOpen = pvNil;
		_iscen = ivNil;

		if (pscen != pvNil)
			{
			SCEN::Close(&pscen);
			}

		if (iscenOld != ivNil)
			{
			iscen = iscenOld;
			iscenOld = ivNil;
			fRet = fFalse;
			goto LRetry;
			}

		_pmcc->SceneChange();
		return(fFalse);
		}

	_pscenOpen = pscen;
	_pmcc->SceneChange();
	_iscen = iscen;

	if (!pscen->FGotoFrm(pscen->NfrmFirst()))
		{

		_pscenOpen = pvNil;
		_iscen = ivNil;
		SCEN::Close(&pscen);

		if (iscenOld != ivNil)
			{
			iscen = iscenOld;
			iscenOld = ivNil;
			fRet = fFalse;
			goto LRetry;
			}

		return(fFalse);
		}

	pscen->UpdateSndFrame();
	InvalViewsAndScb();

	return(fRet);
}

/****************************************************
 *
 * Creates a new scene and inserts it as scene number iscen.
 *
 * Parameters:
 *	iscen - Scene number to insert the new scene as.
 *
 * Returns:
 *  fTrue, if successful, else fFalse.
 *
 ****************************************************/
bool MVIE::FNewScenInsCore(long iscen)
{
	AssertThis(0);
	AssertIn(iscen, 0, Cscen() + 1);

	PSCEN pscen;

	//
	// Create the new scene.
	//
	pscen = SCEN::PscenNew(this);
	if (pscen == pvNil)
		{
		return(fFalse);
		}

	if (!FInsScenCore(iscen, pscen))
		{
		SCEN::Close(&pscen);
		return(fFalse);
		}

	SCEN::Close(&pscen);
	return(fTrue);
}

/****************************************************
 *
 * Moves up/down all the chids by one.
 *
 * Parameters:
 *	chid - chid number to start at.
 *	fDown- fTrue if move down, else move up.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void MVIE::_MoveChids(CHID chid, bool fDown)
{
	AssertThis(0);

	PCFL pcfl = _pcrfAutoSave->Pcfl();
	KID kid;
	CHID chidTmp;

	if (fDown)
		{
    	//
    	// Move down chids of all old scenes (increase by one)
    	//
    	for (chidTmp = _cscen; chidTmp > chid;)
    		{
            chidTmp--;
    		AssertDo(pcfl->FGetKidChidCtg(kctgMvie, _cno, chidTmp, kctgScen, &kid),
    				 "Should never fail");

    		pcfl->ChangeChid(kctgMvie, _cno, kctgScen, kid.cki.cno, chidTmp, chidTmp + 1);
    		}
		}
	else
		{
    	//
    	// Move up chids of all old scenes (decrease by one)
    	//
    	for (chidTmp = chid; chidTmp < (CHID)_cscen; chidTmp++)
    		{
    		AssertDo(pcfl->FGetKidChidCtg(kctgMvie, _cno, chidTmp + 1, kctgScen, &kid),
    				 "Should never fail");

    		pcfl->ChangeChid(kctgMvie, _cno, kctgScen, kid.cki.cno, chidTmp + 1, chidTmp);
    		}

		}
}


/******************************************************************************
	_FIsChild
		Enumerates the children of the MVIE chunk and reports whether the
		given (ctg, cno) chunk is an actual child of the MVIE chunk.
	
	Arguments:
		PCFL pcfl  --  the file on which to check
		CTG ctg    --  these are self-explanatory
		CNO cno
	
	Returns:  fTrue if the (ctg, cno) chunk is an immediate child of the MVIE
	
************************************************************ PETED ***********/
bool MVIE::_FIsChild(PCFL pcfl, CTG ctg, CNO cno)
{
	bool fIsChild = fFalse;
	long ckid, ikid;
	KID kid;

	ckid = pcfl->Ckid(kctgMvie, _cno);
	for (ikid = 0; ikid < ckid; ikid++)
		{
		if (!pcfl->FGetKid(kctgMvie, _cno, ikid, &kid))
			{
			Bug("CFL returned bogus ckid");
			break;
			}

		if (kid.cki.ctg == ctg && kid.cki.cno == cno)
			{
			fIsChild = fTrue;
			break;
			}
		}

	return fIsChild;
}


/****************************************************
 *
 * Adopt the scene user sounds as children of the movie.
 * Msnds are children of the movie unless they are
 * unused and deleted when the movie closes.
 *
 * Parameters
 * pcfl
 * cnoScen
 *
 *	Returns:
 * success or failure
 *
 ****************************************************/
bool MVIE::_FAdoptMsndInMvie(PCFL pcfl, CNO cnoScen)
{
	AssertThis(0);
	AssertPo(pcfl, 0);

	CHID chidMvie;
	long ckid, ikid;
	KID kid;

	ckid = pcfl->Ckid(kctgScen, cnoScen);
	for (ikid = 0; ikid < ckid; ikid++)
		{
		if (!pcfl->FGetKid(kctgScen, cnoScen, ikid, &kid))
			{
			Bug("CFL returned bogus ckid");
			break;
			}

		if (kid.cki.ctg == kctgMsnd)
			{
			if (!_FIsChild(pcfl, kctgMsnd, kid.cki.cno))
				{
				// Adopt as a child of the movie
				chidMvie = _ChidMvieNewSnd();
				if (!pcfl->FAdoptChild(kctgMvie, _cno, kctgMsnd, kid.cki.cno,
						chidMvie))
					{
					goto LFail;
					}
				}
			}
		}

	return fTrue;
LFail:
	return fFalse;
}


/****************************************************
 *
 * Resolves a sound tag & chid to be a current tag
 *
 * Note: Msnds are children of the current scene.
 * Due to sound import, the cno can change.
 *
 * Parameters
 * ptag (with *ptag.cno possibly out of date)
 * chid (valid only for user snd)
 * cnoScen (if cnoNil, use current scene's cno
 *
 *	Returns:
 * Updated *ptag
 *
 ****************************************************/
bool MVIE::FResolveSndTag(PTAG ptag, CHID chid, CNO cnoScen, PCRF pcrf)
{
	AssertThis(0);
	AssertVarMem(ptag);
	AssertNilOrVarMem(pcrf);

	KID kidScen;
	KID kid;
	TAG tagNew = *ptag;
	PCFL pcfl;

	if (pvNil == pcrf)
		pcrf = _pcrfAutoSave;
	pcfl = pcrf->Pcfl();

	if (ptag->sid != ksidUseCrf)
		return fTrue;
	if (cnoNil == cnoScen)
	   	{
		if (!pcfl->FGetKidChidCtg(kctgMvie, _cno, _iscen, kctgScen, &kidScen))
			return fFalse;
		cnoScen = kidScen.cki.cno;
		}

	if (!pcfl->FGetKidChidCtg(kctgScen, cnoScen, chid, kctgMsnd, &kid))
			return fFalse;

	if (ptag->cno != kid.cki.cno)
		{
		// As the pcrf has not changed, it is not essential
		// to close & open the respective tags.
		tagNew.cno = kid.cki.cno;
		if (!TAGM::FOpenTag(&tagNew, pcrf))
			return fFalse;
		TAGM::CloseTag(ptag);
		*ptag = tagNew;
		}
	return fTrue;
}

/****************************************************
 *
 * Finds the chid (child of the current scene) for a
 * given sound tag.  Adopt it if not found.
 * Meaningful for user sounds only
 *
 * Parameters
 * cno
 *
 *	Returns:
 * Updated *pchid
 *
 ****************************************************/
bool MVIE::FChidFromUserSndCno(CNO cno, CHID *pchid)
{
	AssertThis(0);
	AssertVarMem(pchid);

	KID kidScen;
	KID kid;
	long ckid;
	long ikid;
	PCFL pcfl = _pcrfAutoSave->Pcfl();

	if (!pcfl->FGetKidChidCtg(kctgMvie, _cno, _iscen, kctgScen, &kidScen))
		return fFalse;
	ckid = pcfl->Ckid(kctgScen, kidScen.cki.cno);
	for (ikid = 0; ikid < ckid; ikid++)
		{
		if (!pcfl->FGetKid(kctgScen, kidScen.cki.cno, ikid, &kid))
			return fFalse;
		if (kid.cki.ctg != kctgMsnd)
			continue;
		if (kid.cki.cno != cno)
			continue;
		*pchid = kid.chid;
		return fTrue;
		}

	*pchid = _ChidScenNewSnd();
	if (!pcfl->FAdoptChild(kctgScen, kidScen.cki.cno, kctgMsnd, cno, *pchid))
		return fFalse;
	return fTrue;
}


/****************************************************
 *
 * Copies a sound file to the movie. (Importing snd)
 * Sounds are written as MSND children of the current
 * scene chunk
 *	 	
 * Parameters:
 *	pfilSrc, sty
 *
 * Returns:
 *  *pcno = cno of chunk written
 *  fFalse if there was a failure, else fTrue.
 *
 ****************************************************/
bool MVIE::FCopySndFileToMvie(PFIL pfilSrc, long sty, CNO *pcno, PSTN pstn)
{
	AssertThis(0);
	AssertVarMem(pfilSrc);
	AssertVarMem(pcno);
	AssertNilOrPo(pstn, 0);
	Assert(_pcrfAutoSave != pvNil, "Bad working file.");

	PCFL pcfl;
	FNI fniSrc;
	CHID chid;
	KID kidScen;

	pcfl = _pcrfAutoSave->Pcfl();
	
	//
	// Ensure we will be writing to the temp file
	//
	if (!_FUseTempFile())
		{
		return fFalse;
		}

	pfilSrc->GetFni(&fniSrc);
	if (fniSrc.Ftg() == kftgMidi)
		{
		if (!MSND::FCopyMidi(pfilSrc, pcfl, pcno, pstn))
			goto LFail;
		}
	else
		{
		if (!MSND::FCopyWave(pfilSrc, pcfl, sty, pcno, pstn))
			goto LFail;
		}

	AssertDo(pcfl->FGetKidChidCtg(kctgMvie, _cno, _iscen, kctgScen, &kidScen),
		"Scene chunk doesn't exist!");

	chid = _ChidMvieNewSnd();
	if (!pcfl->FAdoptChild(kctgMvie, _cno, kctgMsnd, *pcno, chid))
		{
		pcfl->Delete(kctgMsnd, *pcno);
		return fFalse;
		}
	if (!pcfl->FSave(kctgSoc))
		return fFalse;

	_fGCSndsOnClose = fTrue;

	return fTrue;
LFail:
	if (!vpers->FIn(ercSocBadSoundFile))
		PushErc(ercSocCantCopyMsnd);
	return fFalse;
}


/****************************************************
 *
 * Copy Msnd chunk from specified movie *pcfl to
 * current movie
 * Parameters:
 *	pcfl : Source file
 *
 * Returns:
 *  fFalse if there was a failure, else fTrue.
 *	*pcnoDest
 *
 ****************************************************/
bool MVIE::FCopyMsndFromPcfl(PCFL pcflSrc, CNO cnoSrc, CNO *pcnoDest)
{
	AssertBaseThis(0);
	AssertPo(pcflSrc, 0);
	AssertVarMem(pcnoDest);

	PCFL pcflDest;
	KID kidScen;
	CHID chid;
	FNI fni;

	if (!FEnsureAutosave())
		return fFalse;		
	pcflDest = _pcrfAutoSave->Pcfl();
	pcflSrc->GetFni(&fni);

	// Copy the msnd chunk from one movie to another
	// Wave or Midi
	if (!pcflSrc->FCopy(kctgMsnd, cnoSrc, pcflDest, pcnoDest))
		return fFalse;

	AssertDo(pcflDest->FGetKidChidCtg(kctgMvie, _cno, _iscen, kctgScen, &kidScen),
		"Scene chunk doesn't exist!");

	chid = _ChidMvieNewSnd(); 		// Find a unique chid for the new sound
	if (!pcflDest->FAdoptChild(kctgMvie, _cno, kctgMsnd, *pcnoDest, chid))
		{
		pcflDest->Delete(kctgMsnd, *pcnoDest);
		return fFalse;
		}
	if (!pcflDest->FSave(kctgSoc))
		return fFalse;
	return fTrue;
}


/****************************************************
 *
 * Choose Scene Chid for New Sound
 * Note: _pcrfAutoSave is expected to be current
 * Parameters:
 *	none
 *
 * Returns:
 *  unique chid for new msnd chunk child of scene
 *
 ****************************************************/
CHID MVIE::_ChidScenNewSnd(void)
{
	AssertBaseThis(0);
	PCFL pcfl = _pcrfAutoSave->Pcfl();
	long ckid;
	long chid;
	KID kidScen;
	KID kid;

	if (!pcfl->FGetKidChidCtg(kctgMvie, _cno, _iscen, kctgScen, &kidScen))
		return fFalse;

	ckid = pcfl->Ckid(kctgScen, kidScen.cki.cno);
	for (chid = 0; chid < ckid; chid++)
		{
		if (!pcfl->FGetKidChidCtg(kctgScen, kidScen.cki.cno, chid, kctgMsnd, &kid))
			return (CHID)chid;
		}
	return (CHID)chid;
}

/****************************************************
 *
 * Choose Mvie Chid for New Sound
 * Note: _pcrfAutoSave is expected to be current
 * Parameters:
 *	none
 *
 * Returns:
 *  unique chid for new msnd chunk child of scene
 *
 ****************************************************/
CHID MVIE::_ChidMvieNewSnd(void)
{
	AssertBaseThis(0);
	PCFL pcfl = _pcrfAutoSave->Pcfl();
	long ckid;
	long chid;
	KID kid;

	ckid = pcfl->Ckid(kctgMvie, _cno);
	for (chid = 0; chid < ckid; chid++)
		{
		if (!pcfl->FGetKidChidCtg(kctgMvie, _cno, chid, kctgMsnd, &kid))
			return (CHID)chid;
		}
	return (CHID)chid;
}


/****************************************************
 *
 * Verify the version number of a file
 *	 	
 * Parameters:
 *	pfni
 *	*pcno == cnoNil if using the first chunk in the file
 *
 * Returns:
 *  fFalse if there was a failure, else fTrue.
 *  *pcno updated
 *
 ****************************************************/
 bool MVIE::FVerifyVersion(PCFL pcfl, CNO *pcno)
 {
 	AssertBaseThis(0); // MVIE hasn't been loaded yet
	AssertPo(pcfl, 0);

	KID kid;
	CNO cnoMvie;
	MFP mfp;
	BLCK blck;

	// Get the cno of the first kid of the movie
	if (pvNil == pcno || cnoNil == *pcno)
		{
		if (!pcfl->FGetCkiCtg(kctgMvie, 0, &(kid.cki)))
			{
			PushErc(ercSocBadFile);
			return fFalse;
			}
		cnoMvie = kid.cki.cno;
		if (pvNil != pcno)
			*pcno = cnoMvie;
		}

	// Get version number of the file
	if (!pcfl->FFind(kctgMvie, cnoMvie, &blck) || !blck.FUnpackData() ||
		(blck.Cb() != size(MFP)) ||
		!blck.FReadRgb(&mfp, size(MFP), 0))
		{
        PushErc(ercSocBadFile);
		return fFalse;
		}

	if (mfp.bo == kboOther)
		{
		SwapBytesBom(&mfp, kbomMfp);
		}

	// Check the version numbers
	if (!mfp.dver.FReadable(kcvnCur, kcvnMin))
		{
        PushErc(ercSocBadVersion);
		return fFalse;
		}
	return fTrue;
}


/****************************************************
 *
 * Removes a scene from the movie.
 *
 * Parameters:
 *	iscen - Scene number to insert the new scene as.
 *
 * Returns:
 *  fFalse if there was a failure, else fTrue.
 *
 ****************************************************/
bool MVIE::FRemScenCore(long iscen)
{
	AssertThis(0);
	AssertIn(iscen, 0, Cscen());
	Assert(_pcrfAutoSave != pvNil, "Bad working file.");

	KID kid;
	PCFL pcfl;
	PSCEN pscen;
	long iscenOld;

	pcfl = _pcrfAutoSave->Pcfl();
	
	//
	// Ensure we are using the temp file
	//
	if (!_FUseTempFile())
		{
		return (fFalse);
		}

	//
	// Close this scene if it is open and different.
	//
    iscenOld = _iscen;
	if (_iscen != iscen)
		{

		//
		// Get the scen to remove.
		//
		if (!FSwitchScen(iscen))
			{
			FSwitchScen(iscenOld);
			return(fFalse);
			}

		}

    //
	// Keep the scene in memory for a second.
	//
	pscen = Pscen();
    pscen->AddRef();

	//
	// Close the current scene
	//
    SCEN::Close(&_pscenOpen);
	_iscen = ivNil;

	//
	// Remove its actors from the roll call
	//
    pscen->RemActrsFromRollCall();
    ReleasePpo(&pscen);

	//
	// Remove the scene chunk.
	//
	AssertDo(pcfl->FGetKidChidCtg(kctgMvie, _cno, iscen, kctgScen, &kid),
			 "Should never fail");
	pcfl->DeleteChild(kctgMvie, _cno, kctgScen, kid.cki.cno, iscen);

	//
	// Move up chids of all old scenes.	
	//
	_cscen--;
	_MoveChids((CHID)iscen, fFalse);

    //
	// Save changes, if this fails, we don't care.  It only
	// matters when the user tries to truly save.
	//
	pcfl->FSave(kctgSoc);

	//
	// Switch to a different scene
	//
	if (Cscen() == 0)
		{
		return(fTrue);
		}

	if (iscen < Cscen())
		{
		FSwitchScen(iscen);
		}
	else
		{
		FSwitchScen(iscen - 1);
		}

	SetDirty();
	
	if (Pscen() == pvNil)
		{
        PushErc(ercSocSceneSwitch);
		return(fTrue);
		}

	return(fTrue);
}


/****************************************************
 *
 * Removes a scene from the movie and creates an
 * undo object for the action.  If it was the
 * currently open scene, then the scene is not open.
 * The next scene is opened.
 *
 * Parameters:
 *	iscen - Scene number to remove.
 *
 * Returns:
 *  fFalse if there was a failure, else fTrue.
 *
 ****************************************************/
bool MVIE::FRemScen(long iscen)
{
	AssertThis(0);
	AssertIn(iscen, 0, Cscen());

	KID kid;
	PMUNS pmuns;
	PSCEN pscen;

	if (_iscen == iscen)
		{
		pscen = Pscen();
		pscen->AddRef();
		}
    else
		{

    	AssertDo(_pcrfAutoSave->Pcfl()->FGetKidChidCtg(kctgMvie, _cno, iscen, kctgScen, &kid),
				 "Should never fail");

    	pscen = SCEN::PscenRead(this, _pcrfAutoSave, kid.cki.cno);

		if ((pscen == pvNil) || !pscen->FPlayStartEvents())
			{
			SCEN::Close(&pscen);
			return(fFalse);
			}

		pscen->HideActors();
		pscen->HideTboxes();

		}

	pmuns = MUNS::PmunsNew();

	if (pmuns == pvNil)
		{
		ReleasePpo(&pscen);
		return(fTrue);
		}

	pmuns->SetIscen(iscen);
	pmuns->SetPscen(pscen);
	pmuns->SetMunst(munstRemScen);

	if (!FAddUndo(pmuns))
		{
		ReleasePpo(&pmuns);
		ReleasePpo(&pscen);
		return(fFalse);
		}

    ReleasePpo(&pmuns);
	ReleasePpo(&pscen);

	if (!FRemScenCore(iscen))
		{
        ClearUndo();
		return(fFalse);
		}

	return(fTrue);
}



/****************************************************
 *
 * Adds a new material to the user's document.
 *
 * Parameters:
 *	pmtrl - Pointer to the material to add.
 *  ptag - Pointer to the tag for the file.
 *
 * Returns:
 *  fTrue if success, fFalse if couldn't add the material
 *
 ****************************************************/
bool MVIE::FInsertMtrl(PMTRL pmtrl, PTAG ptag)
{
	AssertThis(0);
	AssertPo(pmtrl, 0);
	AssertVarMem(ptag);

    PCRF pcrf;
    PCFL pcfl;
	CNO cno;

	if (!FEnsureAutosave(&pcrf))
   		{
		TrashVar(ptag);
		return (fFalse);
		}

	pcfl = pcrf->Pcfl();
	if (!pmtrl->FWrite(pcfl, kctgMtrl, &cno))
		{
		TrashVar(ptag);
		return fFalse;
		}

	ptag->sid = ksidUseCrf;
	ptag->ctg = kctgMtrl;
	ptag->cno = cno;

	if (!TAGM::FOpenTag(ptag, _pcrfAutoSave))
		{
		return fFalse;
		}

	return fTrue;
}


/****************************************************
 *
 * Ensure an autosave file exists to use
 *
 * Parameters:
 *	Return the pcrf
 *
 * Returns:
 *  fTrue if success, fFalse if couldn't add the material
 *
 ****************************************************/
bool MVIE::FEnsureAutosave(PCRF *ppcrf)
{
	AssertThis(0);

    if (!_FMakeCrfValid())
		{
		return (fFalse);
		}

	if (pvNil != ppcrf)
		*ppcrf = _pcrfAutoSave;

	//
	// Switch to the autosave file
	//
	if (!_FUseTempFile())
		{
		return (fFalse);
		}
	return fTrue;
}


/****************************************************
 *
 * Adds a new 3-D Text object to the user's document.
 *
 * Parameters:
 *  pstn - TDT text
 *  tdts - the TDT shape
 *  ptagTdf - a tag to the TDT's font
 *
 * Returns:
 *  fTrue if success, fFalse if couldn't add the TDT
 *
 ****************************************************/
bool MVIE::FInsTdt(PSTN pstn, long tdts, PTAG ptagTdf)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	Assert(pstn->Cch() > 0, "can't insert 0-length TDT");
	AssertIn(tdts, 0, tdtsLim);
	AssertVarMem(ptagTdf);

    PCFL pcfl;
	CNO cno;
	PTDT ptdt;
	TAG tagTdt;

	ptdt = TDT::PtdtNew(pstn, tdts, ptagTdf);
	if (pvNil == ptdt)
		return fFalse;

    //
	// Make sure we have a file to switch to
	//
    if (!_FMakeCrfValid())
		{
		return(fFalse);
		}

	pcfl = _pcrfAutoSave->Pcfl();

	//
	// Switch to the autosave file
	//
	if (!_FUseTempFile())
		{
		return (fFalse);
		}

	if (!ptdt->FWrite(pcfl, kctgTmpl, &cno))
		{
		return fFalse;
		}
	ReleasePpo(&ptdt);

	tagTdt.sid = ksidUseCrf;
	tagTdt.ctg = kctgTmpl;
	tagTdt.cno = cno;

	if (!TAGM::FOpenTag(&tagTdt, _pcrfAutoSave))
		{
		return fFalse;
		}

	if (!FInsActr(&tagTdt))
		return fFalse;

	TAGM::CloseTag(&tagTdt);

	return fTrue;
}


/****************************************************
 *
 * Edits the TDT attached to pactr.
 *
 * Parameters:
 *  pactr - Pointer to actor to change
 *  pstn - New TDT text
 *	tdts - New TDT shape
 *  ptagTdf - New TDT font
 *
 * Returns:
 *  fTrue if success, fFalse if couldn't change the TDT
 *
 ****************************************************/
bool MVIE::FChangeActrTdt(PACTR pactr, PSTN pstn, long tdts, PTAG ptagTdf)
{
	AssertThis(0);
	AssertPo(pactr, 0);
	Assert(pactr->Ptmpl()->FIsTdt(), "actor must be a TDT");
	AssertPo(pstn, 0);
	AssertIn(tdts, 0, tdtsLim);
	AssertVarMem(ptagTdf);

	long ich;
	bool fNonSpaceFound;
	PTDT ptdtNew;
	TAG tagTdtNew;
	PCFL pcfl;
	CNO cno;
	PACTR pactrDup;

	Assert(pactr == Pscen()->PactrSelected(), 0);
	fNonSpaceFound = fFalse;
	for (ich = 0; ich < pstn->Cch(); ich++)
		{
		if (pstn->Psz()[ich] != ChLit(' '))
			{
			fNonSpaceFound = fTrue;
			break;
			}
		}
	if (!fNonSpaceFound) // delete the actor
		{
		return FRemActr();
		}

	ptdtNew = TDT::PtdtNew(pstn, tdts, ptagTdf);
	if (pvNil == ptdtNew)
		return fFalse;

    //
	// Make sure we have a file to switch to
	//
    if (!_FMakeCrfValid())
		{
		return(fFalse);
		}

	pcfl = _pcrfAutoSave->Pcfl();

	//
	// Switch to the autosave file
	//
	if (!_FUseTempFile())
		{
		return (fFalse);
		}

	if (!ptdtNew->FWrite(pcfl, kctgTmpl, &cno))
		{
		return fFalse;
		}
	ReleasePpo(&ptdtNew);

	tagTdtNew.sid = ksidUseCrf;
	tagTdtNew.ctg = kctgTmpl;
	tagTdtNew.cno = cno;

	if (!TAGM::FOpenTag(&tagTdtNew, _pcrfAutoSave))
		{
		return fFalse;
		}

	if (!pactr->FDup(&pactrDup))
		{
		TAGM::CloseTag(&tagTdtNew);
		return fFalse;
		}

	if (!pactr->FChangeTagTmpl(&tagTdtNew))
		{
		TAGM::CloseTag(&tagTdtNew);
		ReleasePpo(&pactrDup);
		return fFalse;
		}

	// this sequence is a little strange to make unwinding easier:
	// we add the actor to the roll call with aridNil to get a new
	// entry, then remove the old entry (using pactrDup).
	pactr->SetArid(aridNil);
	if (!FAddToRollCall(pactr, pstn))
		{
		pactr->Restore(pactrDup);
		ReleasePpo(&pactrDup);
		TAGM::CloseTag(&tagTdtNew);
		return fFalse;
		}
	RemFromRollCall(pactrDup);
	ReleasePpo(&pactrDup);

	TAGM::CloseTag(&tagTdtNew);
	SetDirty();

	return fTrue;
}


/****************************************************
 *
 * Closes and releases the current scene, if any
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if success, fFalse if couldn't autosave
 *
 ****************************************************/
bool MVIE::_FCloseCurrentScene(void)
{
	AssertThis(0);

	if (Pscen() != pvNil)
		{

		if (!FAutoSave(pvNil, fFalse)) // could not save...keep scene open
			{
			return fFalse;
			}

		SCEN::Close(&_pscenOpen);
		_iscen = ivNil;
		}
	return fTrue;
}


/****************************************************
 *
 * Makes sure that the current file in use is a temp file.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if success, fFalse if couldn't switch
 *
 ****************************************************/
bool MVIE::_FUseTempFile(void)
{
	AssertThis(0);

	PCFL pcfl;
	KID kid;
	FNI fni;

	pcfl = _pcrfAutoSave->Pcfl();

	//
	// Make sure we are using the temporary file
	//
	if (!pcfl->FTemp())
		{

        if (!fni.FGetTemp())
			{
			return(fFalse);
			}

		if (!pcfl->FSave(kctgSoc, &fni))
			{
			return(fFalse);
			}

		// Set the Temp flag
		AssertDo(pcfl->FSetGrfcfl(fcflTemp, fcflTemp), 0);

		//
		// Update _cno
		//
		AssertDo(pcfl->FGetCkiCtg(kctgMvie, 0, &(kid.cki)), "Should never fail");
        _cno = kid.cki.cno;

		}

	return fTrue;
}


/****************************************************
 *
 * Makes sure that there is a file to work with.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if success, fFalse if couldn't switch
 *
 ****************************************************/
bool MVIE::_FMakeCrfValid(void)
{
	AssertThis(0);

	PCFL pcfl;
	FNI fni;

	if (_pcrfAutoSave != pvNil)
		{
		return(fTrue);
		}

	//
	// Get a temp file
	//
	if (!fni.FGetTemp())
		{
		return(fFalse);
		}

	pcfl = CFL::PcflCreate(&fni, fcflTemp);
	if (pcfl == pvNil)
		{
		return(fFalse);
		}

    Assert(pcfl->FTemp(), "Bad CFL");

	//
	// Note (by manders): CRF *must* have 0 cache size, because of
	// serious cache-coherency problems otherwise.  TMPL data is not
	// read-only, and chunk numbers change over time.
	//
	_pcrfAutoSave = CRF::PcrfNew(pcfl, 0);  // cache size must be 0
	if (pvNil == _pcrfAutoSave)
		{
		ReleasePpo(&pcfl);
		return(fFalse);
		}

    //
	// Create movie chunk
	//
	SetDirty();
	if (!FAutoSave())
		{
		ReleasePpo(&pcfl);
		return(fFalse);
		}

	ReleasePpo(&pcfl);
	return fTrue;
}


/****************************************************
 *
 * Saves a movie to the temp file assigned to the movie.
 *
 * Parameters:
 *	pfni - File to save to, pvNil if to use a temp file.
 *	pCleanRollCall - Should actors that are not used be removed?
 *
 * Returns:
 *  fFalse if there was a failure, else fTrue.
 *
 ****************************************************/
bool MVIE::FAutoSave(PFNI pfni, bool fCleanRollCall)
{
	AssertThis(0);
	AssertNilOrPo(_pcrfAutoSave, 0);
	AssertNilOrPo(pfni, ffniFile);

#ifdef BUG1848
	bool fRetry = fTrue;
#endif // BUG1848
	BLCK blck;
	CNO cno;
	CNO cnoScen;
	CNO cnoSource;
	MFP mfp;
	KID kidScen, kidGstRollCall, kidGstSource;
	PCFL pcfl;
	PGST pgstSource = pvNil;

	if (_pcrfAutoSave == pvNil)
		{
		return(fFalse);
		}

	pcfl = _pcrfAutoSave->Pcfl();

	//
	// If no changes, then quit quick.
	//
	if (!_fAutosaveDirty && (pfni == pvNil) && !_fDocClosing)
		{
		return(fTrue);
		}

	vpappb->BeginLongOp();

	if ((pfni == pvNil) && !pcfl->FTemp() && !_FUseTempFile())
		{
		vpappb->EndLongOp();
		return(fFalse);
		}

#ifdef BUG1848
LRetry:
#endif // BUG1848
	//
	// Ensure movie chunk exists.
	//
	if (_cno == cnoNil)
		{
		if (!pcfl->FAdd(size(MFP), kctgMvie, &_cno, &blck))
			{
			goto LFail0;
			}

        mfp.bo = kboCur;
		mfp.osk= koskCur;
		mfp.dver.Set(kcvnCur, kcvnBack);

		if (!blck.FWrite(&mfp))
			{
			goto LFail0;
			}

   		}

	//
	// Save open scene.
	//
	if (Pscen() != pvNil)
		{

		//
		// Save scene in new chunk
		//
		if (!Pscen()->FWrite(_pcrfAutoSave, &cnoScen))
			{
			goto LFail0;
			}

        //
		// Delete old chunk with this scene
		//
		AssertDo(pcfl->FGetKidChidCtg(kctgMvie, _cno, _iscen, kctgScen,
			&kidScen), "Should never fail");

        //
		// Update chid for movie
		//
	    if (!pcfl->FAdoptChild(kctgMvie, _cno, kctgScen, cnoScen, _iscen))
			{
			pcfl->Delete(kctgScen, cnoScen);
			goto LFail0;
			}

		}

    //
	// Save the movie roll-call
	//
    if (fCleanRollCall)
		{
        MACTR mactr;
		long imactr;

		for (imactr = 0; imactr < _pgstmactr->IvMac();)
			{
            _pgstmactr->GetExtra(imactr, &mactr);
			if (mactr.cactRef == 0)
				{
                _pgstmactr->Delete(imactr);
				vptagm->CloseTag(&mactr.tagTmpl);
				}
            else
				{
                imactr++;
				}
			}

		_pmcc->UpdateRollCall();

		}

    //
	// Get old roll call if it exists.
	//
	if (!pcfl->FGetKidChidCtg(kctgMvie, _cno, 0, kctgGst, &kidGstRollCall))
		{
		kidGstRollCall.cki.cno = cnoNil;
		}

	if (!pcfl->FAdd(_pgstmactr->CbOnFile(), kctgGst, &cno, &blck))
		{
		goto LFail1;
		}

    if (!_pgstmactr->FWrite(&blck) ||
			!pcfl->FAdoptChild(kctgMvie, _cno, kctgGst, cno, 0))
		{
		pcfl->Delete(kctgGst, cno);
		goto LFail1;
		}

	//
	// Save the known sources list
	//

    //
	// Get old sources list if it exists.
	//
	if (!pcfl->FGetKidChidCtg(kctgMvie, _cno, kchidGstSource, kctgGst,
			&kidGstSource))
		{
		kidGstSource.cki.cno = cnoNil;
		}

	pgstSource = vptagm->PgstSource();
	if (pgstSource == pvNil)
		goto LFail2;

	if (!pcfl->FAdd(pgstSource->CbOnFile(), kctgGst, &cnoSource, &blck))
		{
		goto LFail2;
		}

    if (!pgstSource->FWrite(&blck) || !pcfl->FAdoptChild(kctgMvie, _cno,
			kctgGst, cnoSource, kchidGstSource))
		{
		pcfl->Delete(kctgGst, cnoSource);
		goto LFail2;
		}

	if (!pcfl->FSetName(kctgMvie, _cno, &_stnTitle))
		{
		goto LFail3;
		}

	//
	// Delete old scene if there is a scene.
	//
	if (Pscen() != pvNil)
		{
		pcfl->DeleteChild(kctgMvie, _cno, kctgScen, kidScen.cki.cno, _iscen);
		}

    //
	// Delete old roll call list if it exists.
	//
	if (kidGstRollCall.cki.cno != cnoNil)
		{
		pcfl->DeleteChild(kctgMvie, _cno, kctgGst, kidGstRollCall.cki.cno, 0);
		}

    //
	// Delete old sources list if it exists.
	//
	if (kidGstSource.cki.cno != cnoNil)
		{
		pcfl->DeleteChild(kctgMvie, _cno, kctgGst, kidGstSource.cki.cno,
			kchidGstSource);
		}

    //
	// If we fail, don't unwind, as everything is consistent.
	// Just let the client know we failed.
	//
	if (pfni != pvNil)
    	{
		bool fSuccess;
		PFIL pfil;
    	
		//
		// If we have this file open, then we need to release it
		// so we can do the save.  We will restore our open below.
		//
		pfil = FIL::PfilFromFni(pfni);
		if (pfil == _pfilSave)
			{
			ReleasePpo(&_pfilSave);
			_fFniSaveValid = fFalse;
			}
		else
			{
			pfil = pvNil;
			}
		//
		// All garbage collection -- ignore any errors, the file will just be
		// a widdle bigger than it has to be.
		//
		_FDoGarbageCollection(pcfl);
		
		fSuccess = pcfl->FSave(kctgSoc, pfni);

		if (pfil != pvNil)
			{
			_pfilSave = FIL::PfilFromFni(pfni);
			if (_pfilSave != pvNil)
				{
				_pfilSave->AddRef();
				_fFniSaveValid = fTrue;
				}
			}

		if (!fSuccess)
			{
			goto LFail0;
			}
		}
	else if (!pcfl->FSave(kctgSoc))
		{
		goto LFail0;
		}

    _fAutosaveDirty = fFalse;
	_fDirty = fTrue;

    //
	// Set the movie title
	//
	_SetTitle(pfni);

	vpappb->EndLongOp();
	return(fTrue);


LFail3:
	pcfl->DeleteChild(kctgMvie, _cno, kctgGst, cnoSource, kchidGstSource);

LFail2:
	pcfl->DeleteChild(kctgMvie, _cno, kctgGst, cno, 0);

LFail1:
	if (Pscen() != pvNil)
		{
		pcfl->DeleteChild(kctgMvie, _cno, kctgScen, cnoScen, _iscen);
		}

LFail0:
	if (pcfl->ElError() != elNil)
		{
		pcfl->ResetEl();
#ifdef BUG1848
		if (fRetry && pfni != pvNil)
			{
			FNI fniTemp;

			/* Effectively, move the temp file to the destination path */
			/* REVIEW seanse(peted): note that the autosave file could whined up
				on the floppy if we fail again (which SeanSe says is "Bad") */
			fniTemp = *pfni;
			if (fniTemp.FGetUnique(pfni->Ftg()) &&
					pcfl->FSave(kctgSoc, &fniTemp))
				{
				pcfl->SetTemp(fTrue);
				vpers->Clear();
				fRetry = fFalse;
				goto LRetry;
				}
			}
#endif // BUG1848
		PushErc(ercSocSaveFailure);
		}

	vpappb->EndLongOp();
	return(fFalse);
}


/****************************************************
 *
 * Do all garbage collection
 *
 * Parameters:
 *	pfni - File to remove chunks from
 *
 * Returns:
 *  fTrue on success, fFalse on failure
 *
 ****************************************************/
bool MVIE::_FDoGarbageCollection(PCFL pcfl)
{
	AssertThis(0);
	AssertPo(pcfl, 0);
	bool fSuccess, fHaveValid;

	// Material and Template garbage collection
	fSuccess = _FDoMtrlTmplGC(pcfl);
	
	// If closing, remove unused sounds
	if (_fDocClosing && FUnusedSndsUser(&fHaveValid))
		_DoSndGarbageCollection(!fHaveValid || Pmcc()->FQueryPurgeSounds());
	return fSuccess;
}


/****************************************************
 *
 * Removes all MTRL and TMPL chunks that are not
 * referenced by any actors in this movie.
 *
 * Parameters:
 *	pfni - File to remove chunks from
 *
 * Returns:
 *  fTrue on success, fFalse on failure
 *
 ****************************************************/
bool MVIE::_FDoMtrlTmplGC(PCFL pcfl)
{
	AssertThis(0);
	AssertPo(pcfl, 0);

	PTAGL ptagl = pvNil;
	long itag;
	TAG tag;
	long icki1 = 0;
	long icki2 = 0;
	CKI cki;
	PGL pglckiDoomed = pvNil;

	ptagl = _PtaglFetch();		// get all tags in user's document
	if (ptagl == pvNil)
		goto LEnd; // no work to do

	pglckiDoomed = GL::PglNew(size(CKI), 0);
	if (pvNil == pglckiDoomed)
		goto LFail;

	while (pcfl->FGetCkiCtg(kctgMtrl, icki1++, &cki) ||
		pcfl->FGetCkiCtg(kctgTmpl, icki2++, &cki))
		{
		// We're only interested in ksidUseCrf tags
		for (itag = 0; itag < ptagl->Ctag(); itag++)
			{
			ptagl->GetTag(itag, &tag);
			if (tag.sid != ksidUseCrf)
				{
				break; // stop..we're out of the ksidUseCrf tags
				}
			if (tag.ctg == cki.ctg && tag.cno == cki.cno)
				{
				break; // stop..this tag is used in the movie
				}
			}
		// Remember, tags are sorted by sid.  So if we got past the
		// ksidUseCrf tags in the movie, this chunk must not be used
		// in the movie.  So put it on the blacklist.
		if (tag.sid != ksidUseCrf || itag == ptagl->Ctag())
			{
			// this chunk is not referenced by a ksidUseCrf tag, so kill it
			if (!pglckiDoomed->FAdd(&cki))
				goto LFail;
			}
		}
	// Get rid of the blacklisted chunks
	for (icki1 = 0; icki1 < pglckiDoomed->IvMac(); icki1++)
		{
		pglckiDoomed->Get(icki1, &cki);
		pcfl->Delete(cki.ctg, cki.cno);
		if (pcfl == _pcrfAutoSave->Pcfl()) // remove chunk from CRF cache
			{
			PFNRPO pfnrpo;

			if (kctgMtrl == cki.ctg)
				{
				pfnrpo = MTRL::FReadMtrl;
				}
			else if (kctgTmpl == cki.ctg)
				{
				pfnrpo = TMPL::FReadTmpl;
				}
			else
				{
				Bug("unexpected ctg");
				}
			// ignore failure of FSetCrep, because return value of fFalse
			// just means that the chunk is not stored in the CRF's cache
			_pcrfAutoSave->FSetCrep(crepToss, cki.ctg, cki.cno, pfnrpo);
			}
		}
LEnd:
	ReleasePpo(&ptagl);
	ReleasePpo(&pglckiDoomed);
	return fTrue;
LFail:
	ReleasePpo(&ptagl);
	ReleasePpo(&pglckiDoomed);
	return fFalse;
}



/****************************************************
 *
 * Gets the file name to save the document to.
 *
 * Parameters:
 *	pfni - A pointer to a place to store the name
 *
 * Returns:
 *  fFalse if the fni is valid, else fTrue *and*
 *  pfni filled in.
 *
 ****************************************************/
bool MVIE::FGetFni(FNI *pfni)
{
	AssertThis(0);
	AssertPo(pfni, 0);

	if (_pfilSave != pvNil)
		{
        _pfilSave->GetFni(pfni);
		}

    return (_fFniSaveValid);
}

/***************************************************************************
 *
 * Saves a movie.
 *
 * Parameters:
 *  cid - type of save command issued for save
 *
 * Returns:
 *  fFalse if there was a failure, else fTrue.
 *
 ****************************************************/
bool MVIE::FSave(long cid)
{
	AssertThis(0);

	// If we are processing a Save command and the current movie is read-only,
	// then treat this as a Save-As command and invoke the Save portfolio.

	if (cid == cidSave && FReadOnly())
		cid = cidSaveAs;

	// Now take the default action.
	return MVIE_PAR::FSave(cid);
}

/****************************************************
 *
 * Saves a movie to the given fni.
 *
 * Parameters:
 *	pfni - File to write to.
 *  fSetFni - Should the file name be remembered.
 *
 * Returns:
 *  fFalse if there was a failure, else fTrue.
 *
 ****************************************************/
bool MVIE::FSaveToFni(FNI *pfni, bool fSetFni)
{
	AssertThis(0);
	AssertNilOrPo(pfni, ffniFile);

	CKI cki;
	PCFL pcfl;

	if (_pcrfAutoSave == pvNil)
		{
		return(pvNil);
		}

	pcfl = _pcrfAutoSave->Pcfl();

	//
	// Update the file
	//
	if (!FAutoSave(pfni, fTrue))
		{
		return(fFalse);
		}

    ClearUndo();

	// Set the AddToExtra flag and clear the Temp flag
    AssertDo(pcfl->FSetGrfcfl(fcflAddToExtra, fcflAddToExtra | fcflTemp), 0);

	if (fSetFni)
		{
		_FSetPfilSave(pfni);  // Ignore failure
		}

	//
	// Update _cno
	//
	AssertDo(pcfl->FGetCkiCtg(kctgMvie, 0, &cki), "Should never fail");

    _cno = cki.cno;

	_fDirty = fFalse;

	return(fTrue);
}

/***************************************************************************
 *
 * Called by docb.cpp to get an fni for a selected movie file using
 * the save portfolio.
 *
 * Parameters:
 *	pfni - fni for selected file
 *
 * Returns:
 *  TRUE  - User selected a file
 *  FALSE - User canceled, (or other error).
 *
***************************************************************************/
bool MVIE::FGetFniSave(FNI *pfni)
{
	AssertThis(0);
	AssertVarMem(pfni);

	return( _pmcc->GetFniSave(pfni, idsPortfMovieFilterLabel, idsPortfMovieFilterExt, idsPortfSaveMovieTitle, ksz3mm, &_stnTitle));
}

/****************************************************
 *
 * Creates a new view on a movie.
 *
 * Parameters:
 *	pgcb - The creation block describing the gob placement
 *
 * Returns:
 *  A pointer to the view, otw pvNil on failure
 *
 ****************************************************/
PDDG MVIE::PddgNew(PGCB pgcb)
{
	AssertThis(0);
	AssertVarMem(pgcb);
	return(MVU::PmvuNew(this, pgcb, _pmcc->Dxp(), _pmcc->Dyp()));
}

/****************************************************
 *
 * Is not used.  Not supported.  Stubbed out here for
 * debugging.
 *
 * Parameters:
 *	None
 *
 * Returns:
 *  pvNil
 *
 ****************************************************/
PDMD MVIE::PdmdNew(void)
{
	Bug("Movie does not support DMDs, use multiple DDGs.");
	return(pvNil);
}


/****************************************************
 *
 * Adds a single item to the undo list
 *
 * Parameters:
 *	pmund - A pointer to a movie undo item.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool MVIE::FAddUndo(PMUNB pmunb)
{
	AssertThis(0);

	pmunb->SetPmvie(this);
	pmunb->SetIscen(Iscen());

	AssertPo(pmunb, 0);

	if (Iscen() != ivNil)
		{
		pmunb->SetNfrm(Pscen()->Nfrm());
		}

    if (!DOCB::FAddUndo(pmunb))
		{
		Pmcc()->SetUndo(undoDisabled);
		return(fFalse);
		}

    Pmcc()->SetUndo(undoUndo);
	return(fTrue);
}


/****************************************************
 *
 * Clears out the undo buffer
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void MVIE::ClearUndo(void)
{
	AssertThis(0);

	MVIE_PAR::ClearUndo();
	Pmcc()->SetUndo(undoDisabled);
}

/***************************************************************************
 *
 * This routine changes the current camera view
 *
 * Parameters:
 *	icam - The new camera to use.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FChangeCam(long icam)
{
	AssertThis(0);
	AssertIn(icam, 0, kccamMax);
	AssertPo(Pscen(), 0);

	if (!Pscen()->FChangeCam(icam))
		{
		return(fFalse);
		}

    SetDirty();
	InvalViews();
	return(fTrue);
}

/***************************************************************************
 *
 * This command inserts a new text box into the open scene.
 *
 * Parameters:
 *	prc - The placement within the movie's view of the text box.
 *  fStory - Is this supposed to be a story text box?
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FInsTbox(RC *prc, bool fStory)
{
	AssertThis(0);
	AssertPvCb(prc, size(RC));
	AssertPo(Pscen(), 0);

	PTBOX ptbox;

	ptbox = TBOX::PtboxNew(Pscen(), prc, fStory);

	if (ptbox == pvNil)
		{
		return(fFalse);
		}
	ptbox->SetDypFontDef(Pmcc()->DypTboxDef());

	AssertPo(ptbox, 0);

	if (!Pscen()->FAddTbox(ptbox))
		{
        ReleasePpo(&ptbox);
		}

    Pscen()->SelectTbox(ptbox);
	ptbox->AttachToMouse();
	ReleasePpo(&ptbox);

	return(fTrue);
}


/***************************************************************************
 *
 * This command removes the currently selected text box.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FNukeTbox(void)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	PTBOX ptbox;

	ptbox = Pscen()->PtboxSelected();

	if (ptbox == pvNil)
		{
        PushErc(ercSocNoTboxSelected);
		return(fFalse);
		}

	return(Pscen()->FRemTbox(ptbox));
}

/***************************************************************************
 *
 * This command hides the currently selected text box at the current frame.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FHideTbox(void)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	PTBOX ptbox;

	ptbox = Pscen()->PtboxSelected();

	if (ptbox == pvNil)
		{
        PushErc(ercSocNoTboxSelected);
		return(fFalse);
		}

	return(ptbox->FHide());
}


/***************************************************************************
 *
 * This command selects the itbox'th text box in the current frame.
 *
 * Parameters:
 *	itbox - Index value of the text box to select.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVIE::SelectTbox(long itbox)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	PTBOX ptbox;
	long cVis = 0;

    while (fTrue)
		{
		ptbox = Pscen()->PtboxFromItbox(itbox);
    	if (ptbox == pvNil)
    		{
            return;
    		}

        if (ptbox->FIsVisible() && (cVis == itbox))
			{
            return;
			}

        if (ptbox->FIsVisible())
			{
			cVis++;
			}

		}


    Pscen()->SelectTbox(ptbox);
}

/***************************************************************************
 *
 * This sets the color to apply to a text box.
 *
 * Parameters:
 *  acr - The destination color.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
void MVIE::SetPaintAcr(ACR acr)
{
	AssertThis(0);

	PMVU pmvu;

	pmvu = (PMVU)PddgGet(0);
	AssertPo(pmvu, 0);
	pmvu->SetPaintAcr(acr);
}


/******************************************************************************
	SetDypFontTextCur
		Sets the current textbox font size
	
	Arguments:
		long dypFont  --  the new textbox font size
	
************************************************************ PETED ***********/
void MVIE::SetDypFontTextCur(long dypFont)
{
	AssertThis(0);

	PmvuFirst()->SetDypFontTextCur(dypFont);
}


/******************************************************************************
	SetStyleTextCur
		Sets the current textbox font style
	
	Arguments:
		long grfont  --  the new textbox font style
	
************************************************************ PETED ***********/
void MVIE::SetStyleTextCur(ulong grfont)
{
	AssertThis(0);

	PmvuFirst()->SetStyleTextCur(grfont);
}


/******************************************************************************
	SetOnnTextCur
		Sets the current textbox font face
	
	Arguments:
		long onn  -- the new textbox font face
	
************************************************************ PETED ***********/
void MVIE::SetOnnTextCur(long onn)
{
	AssertThis(0);

	PmvuFirst()->SetOnnTextCur(onn);
}


/******************************************************************************
	PmvuCur
		Returns the active MVU for this movie
	
************************************************************ PETED ***********/
PMVU MVIE::PmvuCur(void)
{
	AssertThis(0);
	PMVU pmvu = (PMVU)PddgActive();

	AssertPo(pmvu, 0);
	Assert(pmvu->FIs(kclsMVU), "Current DDG isn't an MVU");
	return pmvu;
}


/******************************************************************************
	PmvuFirst
		Returns the first MVU for this movie
	
************************************************************ PETED ***********/
PMVU MVIE::PmvuFirst(void)
{
	AssertThis(0);
	PMVU pmvu = (PMVU)PddgGet(0);

	AssertPo(pmvu, 0);
	Assert(pmvu->FIs(kclsMVU), "First DDG isn't an MVU");
	return pmvu;
}


/***************************************************************************
 *
 * This command inserts a new actor.
 *
 * Parameters:
 *	ptag - The tag of the actor to create.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FInsActr(PTAG ptag)
{
	AssertThis(0);
	AssertPvCb(ptag, size(TAG));
	AssertPo(Pscen(), 0);

	PACTR pactr;


	vpappb->BeginLongOp();

	//
	// Create the actor.
	//
    if (!vptagm->FCacheTagToHD(ptag))
		{
		vpappb->EndLongOp();
		return(fFalse);
		}
	
	vpappb->EndLongOp();

	pactr = ACTR::PactrNew(ptag);
	if (pactr == pvNil)
		{
		return(fFalse);
		}

	AssertPo(pactr, 0);
	if (!Pscen()->FAddActr(pactr))
		{
		ReleasePpo(&pactr);
		return(fFalse);
		}

	ReleasePpo(&pactr);

	SetDirty();
	InvalViewsAndScb();

	return(fTrue);
}

/***************************************************************************
 *
 * Brings an actor on to the stage.
 *
 * Parameters:
 *	arid - Arid of the actor to bring on.  aridNil implies the
 *		currently selected actor.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FAddOnstage(long arid)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	ACTR *pactr;
	//
	// Get the actor
	//
	if (arid == aridNil)
		{
		pactr = Pscen()->PactrSelected();
		}
    else
		{
        pactr = Pscen()->PactrFromArid(arid);
		Pscen()->SelectActr(pactr);
		}

    if (pactr == pvNil)
		{
        PushErc(ercSocNoActrSelected);
		return(fFalse);
		}

    AssertPo(pactr, 0);

	if (!pactr->FAddOnStage())
		{
		return(fFalse);
		}

    SetDirty();
	InvalViews();

	return(fTrue);
}

/***************************************************************************
 *
 * Removes the selected actor from the current scene
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FRemActr()
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	ACTR *pactr;

	//
	// Get the current actor
	//
	pactr = Pscen()->PactrSelected();
	AssertPo(pactr, 0);
	if (!Pscen()->FRemActr(pactr->Arid()))
		{
        return(fFalse);
		}

	SetDirty();
	InvalViewsAndScb();
	Pmcc()->ActorNuked();

	return(fTrue);
}

/***************************************************************************
 *
 * Rotates selected actor by degrees around an axis.
 *
 * Parameters:
 *  axis - Brender axis to rotate around.
 *	xa - Degrees to rotate by in X.
 *	ya - Degrees to rotate by in Y.
 *	za - Degrees to rotate by in Z.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FRotateActr(BRA xa, BRA ya, BRA za, bool fFromHereFwd)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	ACTR *pactr;

	//
	// Get the current actor
	//
	pactr = Pscen()->PactrSelected();
	AssertPo(pactr, 0);

	if (!pactr->FRotate(xa, ya, za, fFromHereFwd))
		{
		return(fFalse);
		}

    SetDirty();
	InvalViews();

	return(fTrue);
}

/***************************************************************************
 *
 * Squashes/Stretches the selected actor by a scalar.
 *
 * Parameters:
 *  brs - The scalar for squashing.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FSquashStretchActr(BRS brs)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	ACTR *pactr;

	//
	// Get the current actor
	//
	pactr = Pscen()->PactrSelected();
	AssertPo(pactr, 0);

	if (brs == rZero)
		{
		return(fTrue);
		}

	BRS brsy = BrsDiv(rOne, brs);

	if (!pactr->FPull(brs, brsy, brs))
		{
		return(fFalse);
		}

    SetDirty();
	InvalViews();

	return(fTrue);
}

/***************************************************************************
 *
 * Drags the actor in time
 *
 * Parameters:
 *  nfrm - The destination frame number.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FSoonerLaterActr(long nfrm)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	ACTR *pactr;
	PAUND paund;

	long dnfrm = nfrm - Pscen()->Nfrm();

	//
	// Get the current actor
	//
	pactr = Pscen()->PactrSelected();
	AssertPo(pactr, 0);

	if (!pactr->FSoonerLater(dnfrm))
		{
		return(fFalse);
		}

    if (CundbUndo() > 0)
		{

		_pglpundb->Get(_ipundbLimDone - 1, &paund);

    	if (paund->FIs(kclsAUND) && paund->FSoonerLater())
    		{
    		AssertPo(paund, 0);
    		paund->SetNfrmLast(nfrm);
    		}

		}

    SetDirty();
	InvalViews();

	return(fTrue);
}


/***************************************************************************
 *
 * Scales the selected actor by a scalar.
 *
 * Parameters:
 *  brs - The scalar for scaling.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FScaleActr(BRS brs)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	ACTR *pactr;

	//
	// Get the current actor
	//
	pactr = Pscen()->PactrSelected();
	AssertPo(pactr, 0);

	if (!pactr->FScale(brs))
		{
		return(fFalse);
		}

    SetDirty();
	InvalViewsAndScb();

	return(fTrue);
}


/****************************************************
 *
 * Adds a sound to the background
 *
 * Parameters:
 *  ptag - tag to the MSND to insert
 *  fLoop - play snd over and over?
 *  fQueue - replace existing sounds, or queue afterwards?
 *  vlm - volume to play sound
 *  sty - sound type
 *
 * Returns:
 *  fFalse if there was a failure, else fTrue.
 *
 ****************************************************/
bool MVIE::FAddBkgdSnd(PTAG ptag, bool fLoop, bool fQueue, long vlm, long sty)
{
	AssertThis(0);
	Assert(Pscen(), 0);

	if (vlm == vlmNil || sty == styNil)
		{
		PMSND pmsnd;
	
		pmsnd = (PMSND)vptagm->PbacoFetch(ptag, MSND::FReadMsnd);
		if (pmsnd == pvNil)
			return fFalse;
		if (vlm == vlmNil)
			vlm = pmsnd->Vlm();
		if (sty == styNil)
			sty = pmsnd->Sty();
		ReleasePpo(&pmsnd);
		}

	return Pscen()->FAddSnd(ptag, fLoop, fQueue, vlm, sty);
}


/****************************************************
 *
 * Adds a sound to an actor
 *
 * Parameters:
 *  pactr - actor to attach sound to
 *  ptag - tag to the MSND to insert
 *  fLoop - play snd over and over?
 *  fQueue - replace existing sounds, or queue afterwards?
 *  vlm - volume to use (vlmNil -> use pmsnd volume)
 *  sty - type to use (styNil -> use pmsnd sty)
 *
 * Returns:
 *  fFalse if there was a failure, else fTrue.
 *
 ****************************************************/
bool MVIE::FAddActrSnd(PTAG ptag, bool fLoop, bool fQueue, bool fActnCel,
	long vlm, long sty)
{
	AssertThis(0);
	ACTR *pactr;

	//
	// Get the current actor
	//
	pactr = Pscen()->PactrSelected();
	AssertPo(pactr, 0);

	return pactr->FSetSnd(ptag, fLoop, fQueue, fActnCel, vlm, sty);
}


/****************************************************
 *
 * Takes a scene and inserts it as scene number iscen,
 * and switches to the scene.
 *
 * Parameters:
 *	iscen - Scene number to insert the new scene as.
 *	pscen - Pointer to the scene to insert.
 *
 * Returns:
 *  fFalse if there was a failure, else fTrue.
 *
 ****************************************************/
bool MVIE::FInsScenCore(long iscen, SCEN *pscen)
{
	AssertThis(0);
	AssertIn(iscen, 0, Cscen() + 1);
	AssertPo(pscen, 0);
	Assert(pscen->Pmvie() == this, "Cannot insert a scene from another movie");

	CNO cnoScen;
	PCFL pcfl;

    //
	// Make sure we have a file to switch to
	//
    if (!_FMakeCrfValid())
		{
		goto LFail0;
		}

	pcfl = _pcrfAutoSave->Pcfl();

	//
	// Ensure we are using the temp file
	//
	if (!_FUseTempFile())
		{
		goto LFail0;
		}

	//
	// Save old scene.
	//
	if (!_FCloseCurrentScene())
		{
		goto LFail0;
		}

    //
	// Write it.
	//
    if (!pscen->FWrite(_pcrfAutoSave, &cnoScen))
		{
		goto LFail0;
		}

	//
	// Hide all bodies, textboxes, etc, created when the write updated the thumbnail.
	//
	pscen->AddRef();
	SCEN::Close(&pscen);

    _MoveChids((CHID)iscen, fTrue);

    _cscen++;

	//
	// Insert new scene as chid.
	//
	if (!pcfl->FAdoptChild(kctgMvie, _cno, kctgScen, cnoScen, iscen))
		{
		goto LFail2;
		}

    //
	// Save changes, if this fails, we don't care.  It only
	// matters when the user tries to truly save.
	//
	pcfl->FSave(kctgSoc);

	if (!FSwitchScen(iscen))
		{
		goto LFail3;
		}

	//
	// Fix up roll call, don't care about failure -- roll-call will just be messed up.
	//
	Pscen()->FAddActrsToRollCall();

    SetDirty();
	_pmcc->UpdateRollCall();

	if (FSoundsEnabled())
		{
		_pmsq->PlayMsq();
		}
	else
		{
		_pmsq->FlushMsq();
		}

	return(fTrue);

LFail3:
	_cscen--;
	pcfl->DeleteChild(kctgMvie, _cno, kctgScen, cnoScen, iscen);
	_MoveChids((CHID)iscen, fFalse);
	pcfl->FSave(kctgSoc);

	if (_cscen > 0)
		{

        if (iscen < _cscen)
			{
			FSwitchScen(iscen);
			}
		else
			{
			FSwitchScen(_cscen - 1);
			}

		}

	return(fFalse);

LFail2:
	_MoveChids((CHID)iscen, fFalse);
	pcfl->Delete(kctgScen, cnoScen);

LFail0:
	return(fFalse);
}


/***************************************************************************
 *
 * Adds a new scene after the current scene.
 *
 * Parameters:
 *	ptag - The tag of the scene to add.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FAddScen(PTAG ptag)
{
	AssertThis(0);
	AssertPvCb(ptag, size(TAG));

	long iscen;
 	TAG tagOld;
	PMUNS pmuns;

	vpappb->BeginLongOp();

	//
	// Set the background
	//
	if (!BKGD::FCacheToHD(ptag))
		{
		vpappb->EndLongOp();
		return(fFalse);
		}

	vpappb->EndLongOp();

	//
	// Set default name
	//
	if ((Pscen() == pvNil) && (_stnTitle.Cch() == 0))
		{
		_SetTitle();
		}

	//
	// Check if this is supposed to overwrite the current scene
	//
	if ((Pscen() == pvNil) || (!Pscen()->FIsEmpty()))
		{

        iscen = Iscen();
		if (iscen == ivNil)
			{
			iscen = -1;
			}


   		if (!FNewScenInsCore(iscen + 1))
			{
			return(fFalse);
			}

		if (!Pscen()->FSetBkgdCore(ptag, &tagOld))
			{
            FRemScenCore(iscen + 1);
			return(fFalse);
			}

		if (FSoundsEnabled())
			{
			_pmsq->PlayMsq();
			}
		else
			{
			_pmsq->FlushMsq();
			}

		pmuns = MUNS::PmunsNew();

		if (pmuns != pvNil)
			{
    		pmuns->SetMunst(munstInsScen);
    		pmuns->SetIscen(iscen + 1);
			pmuns->SetTag(ptag);

        	if (!FAddUndo(pmuns))
        		{
				ReleasePpo(&pmuns);
				FRemScenCore(iscen + 1);
				return(fFalse);
        		}

            ReleasePpo(&pmuns);
			}
		else
			{
            FRemScenCore(iscen + 1);
			return(fFalse);
			}
		}
	else
		{

		if (!Pscen()->FSetBkgd(ptag))
			{
			Bug("warning: set background failed.");
			return(fFalse);
			}
		}

    SetDirty();
	InvalViewsAndScb();

	return(fTrue);
}

/***************************************************************************
 *
 * Plays a movie.
 *
 * Parameters:
 *	None
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVIE::Play()
{
	AssertThis(0);

	if (FPlaying())
		{

		if (FStopPlaying())
			{
			//
			// We will stop it soon anyway.
			//
			return;
			}

        //
		// Kill playing timer
		//
		SetFStopPlaying(fTrue);

		if (_fPausing)
			{
			//
			// There is no clock timer, so call directly
			//
	        FCmdRender(pvNil);
			}

		}
	else
		{

		if (Pscen() == pvNil)
			{
			Pmcc()->PlayStopped();
			return;
			}

		// reset any outstanding sounds (from listener preview)
		_pmsq->StopAll();
		// flush any outstanding sound messages (from listener preview)
		_pmsq->FlushMsq();

		//
		// Check for if we need to rewind
		//
		if ((Iscen() + 1 == Cscen()) && (Pscen()->Nfrm() == Pscen()->NfrmLast()))
			{
			if (!FSwitchScen(0))
				{
				Pmcc()->PlayStopped();
				return;
				}
			if (!Pscen()->FGotoFrm(Pscen()->NfrmFirst()))
				{
				Pmcc()->PlayStopped();
				return;
				}
			}

        //
		// Start playing timer
		//
		_fOldSoundsEnabled = FSoundsEnabled();
		SetFSoundsEnabled(fTrue);
		_cnfrm = 0;
		_tsStart = TsCurrent();
		SetFStopPlaying(fFalse);
		_clok.Start(0);
        SetFPlaying(fTrue);
		vpcex->EnqueueCid(cidMviePlaying, pvNil, pvNil, fTrue);

		if (!_clok.FSetAlarm(0, this))
			{
			Pmcc()->PlayStopped();
			SetFPlaying(fFalse);
			_pmsq->PlayMsq();
			SetFSoundsEnabled(_fOldSoundsEnabled);
			return;
			}

		//
		// Check if we need to play the opening transition
		//
		_pmsq->SndOnLong();
		Pscen()->Enable(fscenPauses);
		Pscen()->SelectActr(pvNil);
		Pscen()->SelectTbox(pvNil);
		// Have to FReplayFrm *before* FStartPlaying because a camera view
		// change in FReplayFrm would wipe out prerendering (which FStartPlaying
		// initiates)
		if (!Pscen()->FReplayFrm(fscenPauses | fscenSounds | fscenActrs) ||
			!Pscen()->FStartPlaying())
			{
			Pmcc()->PlayStopped();
			SetFStopPlaying(fTrue);
			_pmsq->PlayMsq();
			SetFSoundsEnabled(_fOldSoundsEnabled);
			return;
			}

		Pmcc()->DisableAccel();

		if ((Iscen() == 0) &&
			(Pscen()->Nfrm() == Pscen()->NfrmFirst()))
			{
			_pmcc->UpdateScrollbars();
			InvalViews();
			vpappb->UpdateMarked();
			}

		Pscen()->PlayBkgdSnd();

		}

	// Play sound queue	
	if (FSoundsEnabled())
		{
		_pmsq->PlayMsq();
		}
	else
		{
		_pmsq->FlushMsq();
		}
	return;
}

/***************************************************************************
 *
 * Handle an alarm going off.
 *
 * This routine simply enqueues a command to render a frame.  We do it this
 * way because the clock pre-empts commands in the command queue.  If the
 * user mouse clicks, we need to process those clicks.  The best way to do
 * that is by doing rendering via the command queue.
 *
 * Parameters:
 *	pcmd - Pointer to the command to process.
 *
 * Returns:
 *  fTrue if it handled the command, else fFalse.
 *
***************************************************************************/
bool MVIE::FCmdAlarm(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	CMD cmd;

	if (FIdleSeen() || vpcex->PgobTracking() != pvNil)
		{

		SetFIdleSeen(fFalse);

		ClearPb(&cmd, size(CMD));
	
		cmd.pcmh = this;
		cmd.cid = cidRender;
		vpcex->EnqueueCmd(&cmd);
	
		}
	else
		{

		//
		// Check again, asap.
		//
		if (!_clok.FSetAlarm(0, this))
			{
			PMVU pmvu;
		
			//
			// Things are in a bad way.
			//
			Pmcc()->EnableAccel();
			SetFPlaying(fFalse);
			SetFStopPlaying(fFalse);
			SetFSoundsEnabled(_fOldSoundsEnabled);
			_clok.RemoveCmh(this);
			_fPausing = fFalse;
			_fScrolling = fFalse;
			_wit = witNil;
			pmvu = (PMVU)PddgGet(0);
			pmvu->PauseUntilClick(fFalse);
			Pscen()->Enable(fscenTboxes);
			Pscen()->Disable(fscenPauses);
	
			//
			// Update views and scroll bars
			//
			InvalViewsAndScb();
	
			//
			// Clean up anything else
			//
			Pscen()->StopPlaying();
			vpcex->EnqueueCid(cidMviePlaying, pvNil, pvNil, fFalse);
	
			//
			// Set sound queue state
			//
			_pmsq->SndOnShort();
	
			Pmcc()->PlayStopped();
			vpsndm->StopAll();

			}

		}

	return(fTrue);
}


/***************************************************************************
 *
 * Handle rendering a frame.
 *
 * This routine gets a little busy.  The basic premise is to render a
 * frame one frame ahead of the one currently displayed.  This means
 * that only actor stuff gets done first, then when the frame is to be
 * displayed, textboxes and sounds get started.  If there is a scrolling
 * text box, then we play nothing else until the scrolling is done, and
 * (finally) pauses in the frame.
 *
 * Parameters:
 *	pcmd - Pointer to the command to process.
 *
 * Returns:
 *  fTrue if it handled the command, else fFalse.
 *
***************************************************************************/
bool MVIE::FCmdRender(PCMD pcmd)
{
	AssertThis(0);
	AssertNilOrVarMem(pcmd);

	PMVU pmvu;
	PTBOX ptbox;
	long itbox;
	ulong tsCur = TsCurrent();

	pmvu = (PMVU)PddgGet(0);
    AssertPo(pmvu, 0);

	if (FStopPlaying())
		{
LStopPlaying:

		Pmcc()->EnableAccel();
        _clok.Stop();
		_clok.RemoveCmh(this);
		_fPausing = fFalse;
		_fScrolling = fFalse;
		_wit = witNil;
		SetFStopPlaying(fFalse);
		SetFPlaying(fFalse);
		SetFSoundsEnabled(_fOldSoundsEnabled);
		pmvu->PauseUntilClick(fFalse);
		if (Pscen() != pvNil)
			{
			Pscen()->Enable(fscenTboxes);
			Pscen()->Disable(fscenPauses);
			}
		else
			Assert(_iscen == ivNil, "Bogus scene state");

		//
		// Update views and scroll bars
		//
		InvalViewsAndScb();

		//
		// Clean up anything else
		//
		if (Pscen() != pvNil)
			Pscen()->StopPlaying();
		vpcex->EnqueueCid(cidMviePlaying, pvNil, pvNil, fFalse);

		//
		// Set sound queue state
		//
		_pmsq->SndOnShort();

		Pmcc()->PlayStopped();
		vpsndm->StopAll();

		// if we were fading, then restore sound volume
		if (_vlmOrg)
			{
			vpsndm->SetVlm(_vlmOrg);
			_vlmOrg = 0;
			}

		return(fTrue);
		}

	//	
	// if _vlmOrg is nonzero, then we are in the process of fading
	//
	if (_vlmOrg)
		{
		long vlm;

		// get the current volume
		vlm = vpsndm->VlmCur();

		// massage it
		vlm -= _vlmOrg/(kdtsVlmFade*4);	// kdtsVolFade seconds * 4 volume changes a second = total number deltas

		// if new volume is below 0, then we are done
		if (vlm <= 0)
			{
			SetFStopPlaying(fTrue);
			vpsndm->SetVlm(0);
			goto LStopPlaying;
			}

		// set the volume to new level
		vpsndm->SetVlm(vlm);

		if (!_clok.FSetAlarm(kdtimVlmFade, this))
			{
			goto LStopPlaying;
			}

		return fTrue;
		}

    if (_fScrolling)
		{
		//
		// Do next text box scroll.
		//
        _fScrolling = fFalse;

		for (itbox = 0; ; itbox++)
			{
            ptbox = Pscen()->PtboxFromItbox(itbox);
			AssertNilOrPo(ptbox, 0);
			if (ptbox == pvNil)	
				{
                break;
				}

            if (ptbox->FNeedToScroll())
				{
                ptbox->Scroll();
                _fScrolling = fTrue;
				}

			}

		if (_fScrolling && !_clok.FSetAlarm(kdtsScrolling, this))
			{
			_fScrolling = fFalse;
			goto LStopPlaying;
			}
		else if (_fScrolling)
			{
			return fTrue;
			}
		else
			{
			goto LCheckForPause;
			}

		}
	


	//
	// If we are not pausing, then start the stuff for this frame
	//
    if (!_fPausing)
		{

        //
    	// Now do all text boxes.
    	//
    	Pscen()->Enable(fscenTboxes);
    	Pscen()->Disable(fscenActrs);
        if (!Pscen()->FGotoFrm(Pscen()->Nfrm()))
    		{
    	    Pscen()->Enable(fscenActrs);
			goto LStopPlaying;
    		}
        Pscen()->Enable(fscenActrs);

		//
    	// Draw the previous frame
    	//
    	MarkViews();

    	//
    	// Update scroll bars
    	//
		_pmcc->UpdateScrollbars();

		//
		// Play outstanding sounds
		//
		if (FSoundsEnabled())
			{
			_pmsq->PlayMsq();
			}
		else
			{
			_pmsq->FlushMsq();
			}

		//
		// Flush to the screen
		//
    	vpappb->UpdateMarked();

		//
		// Check for any text box scrolling
		//
		//
		for (itbox = 0; ; itbox++)
			{
            ptbox = Pscen()->PtboxFromItbox(itbox);
			AssertNilOrPo(ptbox, 0);
			if (ptbox == pvNil)	
				{
                break;
				}

            if (ptbox->FNeedToScroll())
				{
				if (!_clok.FSetAlarm(kdtsScrolling, this))
					{
					goto LStopPlaying;
					}

                _fScrolling = fTrue;
				return(fTrue);
				}
			}

LCheckForPause:

		//
		// Check for a pause
		//
    	switch(_wit)
    		{
    	case witUntilClick:
    		_wit = witNil;
			_fPausing = fTrue;
    		pmvu->PauseUntilClick(fTrue);
    	    return(fTrue);
    	case witUntilSnd:
			if (_pmsq->FPlaying(fFalse))
				{
				_fPausing = fTrue;
				if (!_clok.FSetAlarm(0, this))
					{
					goto LStopPlaying;
					}
				return(fTrue);
				}
			
			_fPausing = fFalse;
			_wit = witNil;
			break;

    	case witForTime:
    		_wit = witNil;
			_fPausing = fTrue;
    		_clok.FSetAlarm(_dts, this);
    	    return(fTrue);
    	case witNil:
			_fPausing = fFalse;
    		break;
        default:
    		Bug("Bad Pause type");
    		}

		}
    else
		{
		if (_wit == witUntilSnd)
			{
			goto LCheckForPause;
			}
		else
			{
        	pmvu->PauseUntilClick(fFalse);
			_fPausing = fFalse;
			}
		}


	//
	// Advance everything to the next frame.
	//     Account for time spent between beginning of this routine and
	//     here.
	//
	if (!_clok.FSetAlarm(LwMax(0, kdtimFrame - LwMulDivAway((TsCurrent() - tsCur),
			kdtimSecond, kdtsSecond)),
			this, 0, fTrue))
		{
		goto LStopPlaying;
		}

	Pscen()->Disable(fscenTboxes);

	if (Pscen()->Nfrm() == Pscen()->NfrmLast())
		{

		if (Iscen() == (Cscen() - 1))
			{
			// since this is the last scene/last frame, we want to
			// fade out music, by setting _vlmOrg we turn off rendering and fade out
			// music until VlmCur is 0, at which point we go into stop state.
			
			if (!vpsndm->FPlayingAll()) 	// there are no sounds playing
				SetFStopPlaying(fTrue);		// there is nothing to fade
			else
				{
				_vlmOrg = vpsndm->VlmCur();		// get the current volume
				if ((0 == _vlmOrg))				// if there is volume to fade with
					SetFStopPlaying(fTrue);		// there is nothing to fade
				}
			return(fTrue);
			}

        _trans = Pscen()->Trans();

		if (FSwitchScen(Iscen() + 1))
			{
			AssertPo(Pscen(), 0);
			Pscen()->Disable(fscenTboxes);
			Pscen()->Enable(fscenPauses);
			if (!Pscen()->FReplayFrm(fscenPauses | fscenSounds | fscenActrs))
				{
				SetFStopPlaying(fTrue);
				return(fTrue);
				}

			Pscen()->PlayBkgdSnd();
			}
		else
			{
			SetFStopPlaying(fTrue);
			return(fTrue);
			}

		}
	else
		{

        if (!Pscen()->FGotoFrm( Pscen()->Nfrm() + 1))
			{
			SetFStopPlaying(fTrue);
			return(fTrue);
			}

		}

	Pbwld()->Render();
	_cnfrm++;

	return(fTrue);
}


/***************************************************************************
 *
 * This sets the costume of an actor.
 *
 * Parameters:
 *	ibprt - Id of the body part to set.
 *  ptag - Pointer to the tag of the costume, if fCustom != fTrue.
 *  cmid - The cmid of the costume, if fCustom == fTrue.
 *  fCustom - Is this a custom costume.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FCostumeActr(long ibprt, PTAG ptag, long cmid, bool fCustom)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	ACTR *pactr;

	//
	// Get the current actor
	//
	pactr = Pscen()->PactrSelected();
	AssertPo(pactr, 0);

	if (!pactr->FSetCostume(ibprt, ptag, cmid, fCustom))
		{
		return(fFalse);
		}

    SetDirty();
	InvalViews();

	return(fTrue);
}



/***************************************************************************
 *
 * This inserts a pause into the scene right now.
 *
 * Parameters:
 *	wit - The type of pause to insert, or witNil to clear pauses.
 *  dts - Number of clock ticks to pause.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FPause(WIT wit, long dts)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	if (!Pscen()->FPause(wit, dts))
		{
		return(fFalse);
		}

    SetDirty();
	return(fTrue);
}


#ifdef DEBUG
/******************************************************************************
	MarkMem
		Marks memory used by the CMVI
	
************************************************************ PETED ***********/
void CMVI::MarkMem(void)
{
	long iv, ivMac;
	MVIED mvied;
	SCEND scend;

	ivMac = pglmvied->IvMac();
	for (iv = 0; iv < ivMac; iv++)
		{
		pglmvied->Get(iv, &mvied);
		MarkMemObj(mvied.pcrf);
		}
	MarkMemObj(pglmvied);

	ivMac = pglscend->IvMac();
	for (iv = 0; iv < ivMac; iv++)
		{
		pglscend->Get(iv, &scend);
		MarkMemObj(scend.pmbmp);
		}
	MarkMemObj(pglscend);
}
#endif // DEBUG


/******************************************************************************
	FAddToCmvi
		Generates a GL of SCENDs that describes this movie.  A movie client
		that wishes to makes wholesale changes to a movie may get this GL,
		rearrange it, including inserting references to new movie files, and
		pass it back to the movie via FSetCmvi to modify the movie.
		Adds the movie to the GL of movie descriptors.

	Arguments:
		PCMVI pcmvi     --  the CMVI to add the movie to
		long iscendIns  --  the point at which to start inserting scenes

	Returns: fTrue if it was successful, fFalse otherwise
	
************************************************************ PETED ***********/
bool MVIE::FAddToCmvi(PCMVI pcmvi, long *piscendIns)
{
	AssertThis(0);
	AssertVarMem(pcmvi);
	AssertNilOrPo(pcmvi->pglscend, 0);

	long iscen = 0, iscenMac = Cscen(), imvied;
	SCEND scend;
	MVIED mvied;
	PCFL pcfl;

	scend.imvied = ivNil;

#ifdef BUG1929
	/* Ensure that _pcrfAutoSave points to the temp file, not the original movie file */
	if (!_FUseTempFile())
		goto LFail;
#endif //BUG1929

	if (!FAutoSave(pvNil, fFalse))
		goto LFail;

	if ((pcmvi->pglscend == pvNil) &&
			(pcmvi->pglscend = GL::PglNew(size(SCEND))) == pvNil)
		{
		goto LFail;
		}

	if ((pcmvi->pglmvied == pvNil) &&
			(pcmvi->pglmvied = GL::PglNew(size(MVIED))) == pvNil)
		{
		goto LFail;
		}

	mvied.pcrf = _pcrfAutoSave;
	mvied.cno = _cno;
	mvied.aridLim = _aridLim;
	if (!pcmvi->pglmvied->FAdd(&mvied, &imvied))
		goto LFail;
	scend.imvied = imvied;
	mvied.pcrf->AddRef();

	pcfl = mvied.pcrf->Pcfl();
	scend.fNuked = fFalse;

	for (iscen = 0; iscen < iscenMac; iscen++, (*piscendIns)++)
		{
		bool fSuccess;
		KID kid;

		/* Get CNO */
		AssertDo(pcfl->FGetKidChidCtg(kctgMvie, _cno, iscen, kctgScen, &kid),
			"Not enough scene chunks for movie");
		scend.cno = kid.cki.cno;
		scend.chid = iscen;
		scend.pmbmp = pvNil;

		/* Get PMBMP and TRANS from the scene */
		if (iscen != Iscen())
			{
			BLCK blck;

			if (!SCEN::FTransOnFile(mvied.pcrf, scend.cno, &scend.trans))
				goto LFail;

			AssertDo(pcfl->FGetKidChidCtg(
				kctgScen, scend.cno, 0, kctgThumbMbmp, &kid),
				"Scene doesn't have a thumbnail");
			if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
				goto LFail;
			if ((scend.pmbmp = MBMP::PmbmpRead(&blck)) == pvNil)
				goto LFail;
			}
		else
			{
			scend.trans = Pscen()->Trans();
			if ((scend.pmbmp = Pscen()->PmbmpThumbnail()) == pvNil)
				goto LFail;
			scend.pmbmp->AddRef();
			}

		/* Add to the list */
		fSuccess = _FInsertScend(pcmvi->pglscend, *piscendIns, &scend);
		ReleasePpo(&scend.pmbmp);
		if (!fSuccess)
			goto LFail;
		}

	return fTrue;
LFail:
	while (iscen--)
		_DeleteScend(pcmvi->pglscend, --(*piscendIns));
	if (pcmvi->pglscend != pvNil && pcmvi->pglscend->IvMac() == 0)
		ReleasePpo(&pcmvi->pglscend);
	if (scend.imvied != ivNil)
		{
		ReleasePpo(&mvied.pcrf);
		pcmvi->pglmvied->Delete(scend.imvied);
		}
	if (pcmvi->pglmvied != pvNil && pcmvi->pglmvied->IvMac() == 0)
		ReleasePpo(&pcmvi->pglmvied);
	return fFalse;
}


/******************************************************************************
	FSetCmvi
		Rebuilds the movie based on the given CMVI.  Any scenes
		marked for deletion are disowned by their MVIE chunk.  Any scenes that
		refer to a movie file other than this MVIE's auto save file are
		copied into this MVIE's auto save file.  SCEN chunks are given new
		CHIDs reflecting their new position within the movie.  The non-nuked
		scenes must appear in the GL in the order that they appear in the
		movie; other than that, there is no restriction on the order of the
		scenes (ie, nuked scenes can appear anywhere in the GL, even though
		currently the only client of this API keeps the nuked scenes at the
		end).

	Arguments:
		PCMVI pcmvi -- the CMVI that describes the new movie structure
	
	Returns: fTrue if it could accomplish all of the above, fFalse otherwise
	
************************************************************ PETED ***********/
bool MVIE::FSetCmvi(PCMVI pcmvi)
{
	AssertThis(0);
	AssertVarMem(pcmvi);
	AssertPo(pcmvi->pglmvied, 0);
	AssertPo(pcmvi->pglscend, 0);

	bool fRet = fFalse;
	long iscend, iscendMac = pcmvi->pglscend->IvMac();
	long iscenOld = Iscen();
	long imvied, imviedMac = pcmvi->pglmvied->IvMac();
	long aridMin = 0;
	CHID chidScen = 0;
	PCFL pcfl = _pcrfAutoSave->Pcfl();
	PCRF pcrf = _pcrfAutoSave;
	PGL pglmviedNew;

	pglmviedNew = pcmvi->pglmvied->PglDup();
	if (pglmviedNew == pvNil)
		goto LFail;

	/* Copy all the external movie chunks into this movie */
	for (imvied = 0; imvied < imviedMac; imvied++)
		{
		MVIED mvied;

		pglmviedNew->Get(imvied, &mvied);

		if (imvied > 0)
			{
			if (!mvied.pcrf->Pcfl()->FClone(kctgMvie, mvied.cno, pcfl, &mvied.cno))
				goto LFail;
			if (!_FAddMvieToRollCall(mvied.cno, aridMin))
				goto LFail;
			mvied.pcrf = pcrf;
			mvied.pcrf->AddRef();
			pglmviedNew->Put(imvied, &mvied);
			}
		else
			{
			Assert(mvied.pcrf == pcrf, "Invalid GL of MVIEDs");
			Assert(mvied.cno == _cno, "Invalid GL of MVIEDs");
			}
		aridMin += mvied.aridLim;
		}

	for (iscend = 0; iscend < iscendMac; iscend++)
		{
		CNO cnoScen = cnoNil;
		SCEND scend;
		MVIED mvied;

		pcmvi->pglscend->Get(iscend, &scend);
		pglmviedNew->Get(scend.imvied, &mvied);

		/* Was this scene imported? */
		if (scend.imvied > 0)
			{
			KID kid;

			if (!pcfl->FGetKidChidCtg(
					kctgMvie, mvied.cno, scend.chid, kctgScen, &kid))
				{
				goto LFail;
				}
			cnoScen = kid.cki.cno;

			/* If so, only bother keeping it if the user didn't delete it */
			if (!scend.fNuked)
				{
				PSCEN pscen;

				if (!pcfl->FAdoptChild(kctgMvie, _cno, kctgScen, cnoScen, chidScen++))
					goto LFail;
				if (!_FAdoptMsndInMvie(pcfl, cnoScen))
					goto LFail;
				if ((pscen = SCEN::PscenRead(this, pcrf, cnoScen)) == pvNil ||
						!pscen->FPlayStartEvents(fTrue) ||
						!pscen->FAddActrsToRollCall())
					{
					PushErc(ercSocNoImportRollCall);
					}

				Pmcc()->EnableActorTools();
				Pmcc()->EnableTboxTools();
				ReleasePpo(&pscen);
				}
			}
		else
			{
			if (scend.fNuked)
				{
				PSCEN pscen;

				if (scend.chid != (CHID)iscenOld)
					{
					pscen = SCEN::PscenRead(this, pcrf, scend.cno);
					if (pscen == pvNil || !pscen->FPlayStartEvents(fTrue))
						PushErc(ercSocNoNukeRollCall);
					}
				else
					{
					pscen = _pscenOpen;
					pscen->AddRef();
					}

				if (pscen != pvNil)
					{
					pscen->RemActrsFromRollCall(fTrue);
					ReleasePpo(&pscen);
					}
				pcfl->DeleteChild(
					kctgMvie, _cno, kctgScen, scend.cno, scend.chid);
				}
			else
				{
				/* Set the CHID to be the current scene number */
				cnoScen = scend.cno;
				pcfl->ChangeChid(kctgMvie, _cno, kctgScen, scend.cno,
					scend.chid, chidScen++);
				}
			}

		/* If we didn't delete the scene, go ahead and update its transition */
		if (scend.chid != (CHID)iscenOld || scend.imvied != 0)
			{
			if (!scend.fNuked)
				{
				Assert(mvied.pcrf == pcrf, "Scene's MVIE didn't get copied");
				Assert(cnoScen != cnoNil, "Didn't set the cnoScen");
				if (!SCEN::FSetTransOnFile(pcrf, cnoScen, scend.trans))
					goto LFail;
				}
			}
		else
			{
			if (scend.fNuked)
				{
				/* Basically, do an _FCloseCurrentScene w/out the autosave */
				SCEN::Close(&_pscenOpen);
				_iscen = ivNil;
				}
			else
				{
				/* If this is the scene that *used* to be Iscen(), change the
					transition in memory rather than on file */
				Pscen()->SetTransitionCore(scend.trans);
				_iscen = iscend;
				}
			}
		}

	_cscen = chidScen;
	_aridLim = aridMin;
	SetDirty();
	if (_cscen == 0)
		{
		Assert(_iscen == ivNil, 0);
		_pmcc->SceneNuked();
		}
	InvalViewsAndScb();
	Pmcc()->UpdateRollCall();

#ifdef DEBUG
	{
	long ckid = pcfl->Ckid(kctgMvie, _cno);
	KID kid;
	CHID chidLast = chidNil;

	for (long ikid = 0; ikid < ckid; ikid++)
		{
		if (pcfl->FGetKid(kctgMvie, _cno, ikid, &kid))
			{
			if (kid.cki.ctg == kctgScen)
				{
				Assert(chidLast == chidNil || kid.chid > chidLast, "Found duplicate CHID in scene children");
				chidLast = kid.chid;
				}
			}
		else
			{
			Bug("Can't guarantee validity of MVIE's SCEN children");
			break;
			}
		}
	}
#endif /* DEBUG */

	fRet = fTrue;
LFail:
	if (pglmviedNew != pvNil)
		{
		/* Remove any copied movies; leave the first MVIED alone */
		while (imvied-- > 1)
			{
			MVIED mvied;

			pglmviedNew->Get(imvied, &mvied);
			Assert(mvied.pcrf == pcrf, "Invalid MVIED during cleanup");
			pcfl->Delete(kctgMvie, mvied.cno);
			ReleasePpo(&mvied.pcrf);
			}
		ReleasePpo(&pglmviedNew);
		}
	return fRet;
}


/******************************************************************************
	_FAddMvieToRollCall
		Updates roll call (including remapping arids for the actors found in
		the new movie) for a given MVIE that's just been copied into this
		movie's file.
	
	Arguments:
		CNO cno       -- the CNO of the copied movie
		long aridMin  -- the new base arid for this movie's actors
	
	Returns: fTrue if it succeeds, fFalse otherwise
	
************************************************************ PETED ***********/
bool MVIE::_FAddMvieToRollCall(CNO cno, long aridMin)
{
	AssertThis(0);

	long imactr, imactrMac, icnoMac = 0;
	PCFL pcfl = _pcrfAutoSave->Pcfl();
	PGST pgstmactr = pvNil;

	/* Update the roll call GST */
	if (!FReadRollCall(_pcrfAutoSave, cno, &pgstmactr))
		{
		pgstmactr = pvNil;
		goto LFail;
		}
	imactrMac = pgstmactr->IvMac();
	for (imactr = 0; imactr < imactrMac; imactr++)
		{
		STN stn;
		MACTR mactr;

		pgstmactr->GetStn(imactr, &stn);
		pgstmactr->GetExtra(imactr, &mactr);

		mactr.arid += aridMin;
		mactr.cactRef = 0;
		if (!_pgstmactr->FAddStn(&stn, &mactr))
			goto LFail;
		}

	/* Remap all the arids on the file */
	if (aridMin > 0)
		{
		ulong grfcge, grfcgeIn = fcgeNil;
		PGL pglcno;
		CKI ckiParLast = { ctgNil, cnoNil }, ckiPar;
		KID kid;
		CGE cge;

		if ((pglcno = GL::PglNew(size(CNO))) == pvNil)
			goto LFail;
		cge.Init(pcfl, kctgMvie, cno);
		while (cge.FNextKid(&kid, &ckiPar, &grfcge, fcgeNil))
			{
			if (grfcge & fcgePre)
				{

				/* If we've found an ACTR chunk, remap its arid */
				if (kid.cki.ctg == kctgActr)
					{
					long icno;
					CNO cnoActr;

					/* Only do a given chunk once */
					Assert(icnoMac == pglcno->IvMac(), "icnoMac isn't up-to-date");
					for (icno = 0; icno < icnoMac; icno++)
						{
						pglcno->Get(icno, &cnoActr);
						if (kid.cki.cno == cnoActr)
							break;
						}
					if (icno < icnoMac)
						continue;
					if (!pglcno->FAdd(&kid.cki.cno))
						goto LFail1;

					/* Change the arid */
					if (!ACTR::FAdjustAridOnFile(pcfl, kid.cki.cno, aridMin))
						{
					/* Don't bother trying to fix the arids on file; the caller
						should be deleting the copied MVIE chunk anyway */
LFail1:
						ReleasePpo(&pglcno);
						goto LFail;
						}
					icnoMac++;

					/* Once we're at an ACTR chunk, set up so that we don't
						enumerate down again until returning to our parent's
						next sibling */
					ckiParLast = ckiPar;
					grfcgeIn = fcgeSkipToSib;
					}
				}
			else if (grfcge & fcgePost && grfcgeIn & fcgeSkipToSib)
				{
				if (ckiParLast.ctg != ckiPar.ctg || ckiParLast.cno != ckiPar.cno)
					grfcgeIn = fcgeNil;
				}
			}
		ReleasePpo(&pglcno);
		}

	ReleasePpo(&pgstmactr);

	return fTrue;
LFail:
	/* NOTE: I could use more variables and make the loops below faster,
		but this is a failure case so I'm not very concerned about
		performance here */
	if (pgstmactr != pvNil)
		{
		MACTR mactr;

		/* Remove added entries to the movie's roll call */
		imactrMac = _pgstmactr->IvMac();
		while (imactr--)
			{
			_pgstmactr->GetExtra(--imactrMac, &mactr);
			vptagm->CloseTag(&mactr.tagTmpl);
			_pgstmactr->Delete(imactrMac);
			pgstmactr->Delete(imactr);
			}

		/* Close any other uncopied tags */
		imactrMac = pgstmactr->IvMac();
		while (imactrMac--)
			{
			pgstmactr->GetExtra(imactrMac, &mactr);
			vptagm->CloseTag(&mactr.tagTmpl);
			}
		ReleasePpo(&pgstmactr);
		}
	return fFalse;
}

/******************************************************************************
	EmptyCmvi
		Frees up the memory used by the CMVI.  For each scene in the
		GL of SCENDs, releases memory that the SCEND referred to.  Likewise
		for each MVIED in the GL of MVIEDs.
	
	Arguments:
		PCMVI pcmvi -- the CMVI to empty
	
	Returns: Sets the client's pointer to pvNil when finished
	
************************************************************ PETED ***********/
void CMVI::Empty(void)
{
	AssertPo(pglscend, 0);
	AssertPo(pglmvied, 0);

	PGL pgl;


	if ((pgl = pglscend) != pvNil)
		{
		long iscend = pgl->IvMac();

		while (iscend-- > 0)
			{
			SCEND scend;
	
			pgl->Get(iscend, &scend);
			ReleasePpo(&scend.pmbmp);
			}
		ReleasePpo(&pgl);
		pglscend = pvNil;
		}

	if ((pgl = pglmvied) != pvNil)
		{
		long imvied = pgl->IvMac();

		while (imvied-- > 0)
			{
			MVIED mvied;

			pgl->Get(imvied, &mvied);
			ReleasePpo(&mvied.pcrf);
			}
		ReleasePpo(&pgl);
		pglmvied = pvNil;
		}
}


/******************************************************************************
	_FInsertScend
		Inserts the given SCEND into a GL of SCENDs that was created by this
		movie.
	
	Arguments:
		PGL pglscend  -- the GL of SCENDs to insert into
		long iscend   -- the position at which to insert this SCEND
		PSCEND pscend -- the SCEND to insert
	
	Returns: fTrue if successful, fFalse otherwise
	
************************************************************ PETED ***********/
bool MVIE::_FInsertScend(PGL pglscend, long iscend, PSCEND pscend)
{
	AssertPo(pglscend, 0);
	AssertPo(pscend->pmbmp, 0);

	if (!pglscend->FInsert(iscend, pscend))
		return fFalse;
	pscend->pmbmp->AddRef();
	return fTrue;
}


/******************************************************************************
	_DeleteScend
		Deletes the given SCEND from a GL of SCENDs that was created by this
		movie.
	
	Arguments:
		PGL pglscend -- the GL of SCENDs to delete from
		long iscend  -- which SCEND to delete
	
************************************************************ PETED ***********/
void MVIE::_DeleteScend(PGL pglscend, long iscend)
{
	AssertPo(pglscend, 0);
	AssertIn(iscend, 0, pglscend->IvMac());

	SCEND scend;

	pglscend->Get(iscend, &scend);
	AssertPo(scend.pmbmp, 0);
	pglscend->Delete(iscend);
	ReleasePpo(&scend.pmbmp);
}


/***************************************************************************
 *
 * This sets the transition type for the current scene.
 *
 * Parameters:
 *	trans - The transition type.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FSetTransition(TRANS trans)
{
	AssertThis(0);
	AssertIn(trans, 0, transLim);
	AssertPo(Pscen(), 0);

	if (!Pscen()->FSetTransition(trans))
		{
		return(fFalse);
		}

    SetDirty();
	return(fTrue);
}



/***************************************************************************
 *
 * This pastes an actor into the movie.
 *
 * Parameters:
 *	pactr - A pointer to the actor to paste.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FPasteActr(PACTR pactr)
{
	AssertThis(0);
	AssertPo(pactr, 0);
	AssertPo(Pscen(), 0);

	PMVU pmvu;

	pmvu = (PMVU)PddgGet(0);
	if (pmvu == pvNil)
		{
		return(fFalse);
		}
    AssertPo(pmvu, 0);

	//
	// Paste this actor in and select it.
	//
	if (!Pscen()->FPasteActr(pactr))
		{
		return(fFalse);
		}

	Pscen()->SelectActr(pactr);
	
	//
 	// Positioning a pasted actor must translate all subroutes
	//
 	pmvu->StartPlaceActor(fTrue);

    SetDirty();
	InvalViewsAndScb();

	return(fTrue);
}

/***************************************************************************
 *
 * This pastes an actor path onto the selected actor.
 *
 * Parameters:
 *	pactr - A pointer to the actor to paste from.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool MVIE::FPasteActrPath(PACTR pactr)
{
	AssertThis(0);
	AssertPo(pactr, 0);
	AssertPo(Pscen(), 0);
	AssertNilOrPo(Pscen()->PactrSelected(), 0);

	PACTR pactrDup;

	// REVIEW SeanSe(seanse): This is wrong.  Move the undo stuff into
	//     ACTR::FPasteRte() and make ACTR::FPasteRte into FPasteRteCore.
	if (Pscen()->PactrSelected() == pvNil)
		{
        PushErc(ercSocNoActrSelected);
		return(fFalse);
		}

	if (!Pscen()->PactrSelected()->FDup(&pactrDup))
		{
		return(fFalse);
		}	

	//
	// Paste this actor in and select it.
	//
	if (!Pscen()->PactrSelected()->FPasteRte(pactr))
		{
		ReleasePpo(&pactrDup);
		return(fFalse);
		}

	if (!Pscen()->PactrSelected()->FCreateUndo(pactrDup))
		{
		Pscen()->PactrSelected()->Restore(pactrDup);
		ReleasePpo(&pactrDup);
		return(fFalse);
		}

    SetDirty();
	InvalViewsAndScb();
	ReleasePpo(&pactrDup);

	return(fTrue);
}



/***************************************************************************
 *
 * This invalidates all the views on the movie.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVIE::InvalViews(void)
{
	AssertThis(0);

	long ipddg;
	PDDG pddg;

	for (ipddg = 0; pvNil != (pddg = PddgGet(ipddg)); ipddg++)
		{
		pddg->InvalRc(pvNil, pddg == PddgActive() ? kginMark : kginSysInval);
	    }
}



/***************************************************************************
 *
 * This invalidates scrollbars and all the views on the movie.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVIE::InvalViewsAndScb(void)
{
	AssertThis(0);

	_pmcc->UpdateScrollbars();
	InvalViews();
}

/***************************************************************************
 *
 * This updates all the views on the movie, updates will happen
 * asap.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVIE::MarkViews(void)
{
	AssertThis(0);

	long ipddg;
	PDDG pddg;

	//
	// Need this call in order to mark correctly the changed regions.
	//
	Pbwld()->Render();

#ifdef DEBUG
	if (FWriteBmps())
		{
		FNI fni;
		STN stn;

		if (stn.FFormatSz(PszLit("cel%04d.dib"), _lwBmp++))
			{
			if (fni.FBuildFromPath(&stn))
				{
				if (!Pbwld()->FWriteBmp(&fni))
					SetFWriteBmps(fFalse);
				}
			}
		}
#endif // DEBUG

	for (ipddg = 0; pvNil != (pddg = PddgGet(ipddg)); ipddg++)
		{
		Pbwld()->MarkRenderedRegn(pddg, 0, 0);
	    }
}


/***************************************************************************
 *
 * This returns the current name of the movie.
 *
 * Parameters:
 *	pstnTitle - An stn to copy the name into.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVIE::GetName(PSTN pstnTitle)
{
	AssertThis(0);
	AssertPo(pstnTitle, 0);

	*pstnTitle = _stnTitle;
}


/******************************************************************************
	ResetTitle
		Resets the movie title to whatever it's normal default would be (either
		from the filename or from the MCC string table).
************************************************************ PETED ***********/
void MVIE::ResetTitle(void)
{
	AssertThis(0);

	FNI fni;

	_stnTitle.SetNil();
	_SetTitle(FGetFni(&fni) ? &fni : pvNil);
}


/***************************************************************************
 *
 * Updates the external action menu.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVIE::BuildActionMenu()
{
	AssertThis(0);
	long arid = aridNil;

	if (pvNil != _pscenOpen && pvNil != _pscenOpen->PactrSelected())
		{
		arid = _pscenOpen->PactrSelected()->Arid();
		}
	_pmcc->ActorSelected(arid);
	_pmcc->UpdateAction();
}


const long kdtsTrans = 4 * kdtsSecond;

/***************************************************************************
 *
 * Does a transition.  Note that the rectangles should be the exact size
 * of the rendered area in order to get pushing, dissolve, etc to look perfect.
 *
 * Note: A cool fun number fact.  If you take a number from 1 - (klwPrime - 1),
 * multiply by klwPrimeRoot and modulo the result by klwPrime, you will get
 * a sequence of numbers which hits every number 1->(klwPrime-1) w/o repeating
 * until every number is hit.  This fact is used to create the dissolve effect.
 *
 * Parameters:
 *	pgnvDst - The destination GNV.
 *	pgnvSrc - The source GNV.
 *	prcDst - The clipping rectangle in the destination.
 *	prcSrc - The clipping rectangle in the source.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVIE::DoTrans(PGNV pgnvDst, PGNV pgnvSrc, RC *prcDst, RC *prcSrc)
{
	AssertThis(0);
	AssertPo(pgnvDst, 0);
	AssertPo(pgnvSrc, 0);
	AssertVarMem(prcDst);
	AssertVarMem(prcSrc);

	PGL pglclrSystem = pvNil;
	PGL pglclrBkgd = pvNil;
	long iclrMin;

	pglclrSystem = GPT::PglclrGetPalette();
	if (Pscen() == pvNil || !Pscen()->Pbkgd()->FGetPalette(&pglclrBkgd, &iclrMin))
		{
		pglclrBkgd = pvNil;
		}
	if (pvNil != pglclrSystem && pvNil != pglclrBkgd)
		{
		Assert(pglclrBkgd->IvMac() + iclrMin <= pglclrSystem->IvMac(),
			"Background palette too large");
		CopyPb(pglclrBkgd->QvGet(0), pglclrSystem->QvGet(iclrMin),
			LwMul(size(CLR), pglclrBkgd->IvMac()));
		}

	switch (_trans)
		{
	case transBlack:
		pgnvDst->FillRc(prcDst, kacrBlack);
		break;
		
	case transFadeToBlack:
		pgnvDst->Dissolve(0, 0, kacrBlack, pgnvSrc, prcSrc, prcDst, kdtsTrans / 2, pglclrSystem);
		break;

	case transFadeToWhite:
		pgnvDst->Dissolve(0, 0, kacrWhite, pgnvSrc, prcSrc, prcDst, kdtsTrans / 2, pglclrSystem);
		break;

	case transDissolve:
		pgnvDst->Dissolve(0, 0, kacrClear, pgnvSrc, prcSrc, prcDst, kdtsTrans, pglclrSystem);
 		break;

	case transCut:
		pgnvDst->FillRc(prcDst, kacrBlack);
		GPT::SetActiveColors(pglclrSystem, fpalIdentity);
		pgnvDst->CopyPixels(pgnvSrc, prcSrc, prcDst);
		break;

	default:
		Bug("bad trans");
		break;
		}

	ReleasePpo(&pglclrSystem);
	ReleasePpo(&pglclrBkgd);

	_trans = transNil;
}

/***************************************************************************
 *
 * Used to query if an actor or tbox exists.
 *
 * Parameters:
 *	lwType - 1 if searching for an actor, else 0.
 *  lwId - Arid or Itbox to search for.
 *
 * Returns:
 *  1 if exists, else 0.
 *
 **************************************************************************/
long MVIE::LwQueryExists(long lwType, long lwId)
{
	AssertThis(0);
	AssertIn(lwType, 0, 2);

	if (Pscen() == pvNil)
		{
		return(0);
		}
    AssertPo(Pscen(), 0);

	if (lwType == 1)
		{
		return(Pscen()->PactrFromArid(lwId) != pvNil ? 1 : 0);
		}
	
	return(Pscen()->PtboxFromItbox(lwId) != pvNil ? 1 : 0);
}


/***************************************************************************
 *
 * Used to query where an actor or tbox exists.
 *
 * Parameters:
 *	lwType - 1 if searching for an actor, else 0.
 *  lwId - Arid or Itbox to search for.
 *
 * Returns:
 *  -1 if nonexistent or non-visible, else x in high word, y in low word.
 *
 **************************************************************************/
long MVIE::LwQueryLocation(long lwType, long lwId)
{
	AssertThis(0);
	AssertIn(lwType, 0, 2);

	PACTR pactr;
	PTBOX ptbox;
	long xp, yp;
	RC rc;

	if (Pscen() == pvNil)
		{
		return(-1);
		}
    AssertPo(Pscen(), 0);

	if (lwType == 1)
		{
		RC rcBounds;
		long cactGuessPt = 0;
		RND rnd;
		long ibset;

		pactr = Pscen()->PactrFromArid(lwId);
		if (pactr == pvNil)
			{
			return(-1);
			}

        AssertPo(pactr, 0);
		if (!pactr->FIsInView())
			{
			return(-1);
			}

        pactr->GetCenter(&xp, &yp);
		pactr->GetRcBounds(&rcBounds);
		// The center of the actor may not be a selectable point (as in
		// an arched spletter), so try some random points if the center
		// point doesn't select the actor.
		while (pactr != Pscen()->PactrFromPt(xp, yp, &ibset) && cactGuessPt < 1000)
			{
			cactGuessPt++;
			xp = rnd.LwNext(rcBounds.Dxp() + rcBounds.xpLeft);
			yp = rnd.LwNext(rcBounds.Dyp() + rcBounds.ypTop);
			}
		if (cactGuessPt == 1000)
			{
			xp = -1;
			yp = -1;
			}
		return((xp << 16) | yp);
		}
	
	ptbox = Pscen()->PtboxFromItbox(lwId);
	if (ptbox == pvNil)
		{
		return(-1);
		}

    AssertPo(ptbox, 0);
	if (!ptbox->FIsVisible())
		{
		return(-1);
		}

    ptbox->GetRc(&rc);
	return((rc.xpLeft << 16) | rc.ypTop);
}

/***************************************************************************
 *
 * Used to set the position within the movie.
 *
 * Parameters:
 *	lwScene - Scene to go to.
 *  lwFrame - Frame to go to.
 *
 * Returns:
 *  0 if successful, else -1.
 *
 **************************************************************************/
long MVIE::LwSetMoviePos(long lwScene, long lwFrame)
{
	AssertThis(0);

	if (!FSwitchScen(lwScene))
		{
		return(-1);
		}

    if (!Pscen()->FGotoFrm(lwFrame))
		{
        return(-1);
		}

	InvalViewsAndScb();

	return(0);
}

/***************************************************************************
 *
 * Unused user sounds in this movie?
 *
 * Parms:
 *     bool pfHaveValid -- bool to take whether any of the unused sounds were
 *                         actually usable
 **************************************************************************/

bool MVIE::FUnusedSndsUser(bool *pfHaveValid)
{
	AssertThis(0);
	AssertNilOrVarMem(pfHaveValid);

	bool fUnused = fFalse;
	long icki, ccki;
	PCFL pcfl;

	if (pfHaveValid != pvNil)
		*pfHaveValid = fFalse;

	if (pvNil == _pcrfAutoSave)
		return fFalse;

	pcfl = _pcrfAutoSave->Pcfl();
	ccki = pcfl->CckiCtg(kctgMsnd);
	for (icki = 0; icki < ccki; icki++)
		{
		CKI cki;
		KID kid;

		AssertDo(pcfl->FGetCkiCtg(kctgMsnd, icki, &cki), "Should never fail");
		Assert(_FIsChild(pcfl, cki.ctg, cki.cno), "Not a child of MVIE chunk");
		if (pcfl->CckiRef(cki.ctg, cki.cno) < 2)
			{
			fUnused = fTrue;
			if (pfHaveValid != pvNil)
				{
				if (pcfl->FGetKidChid(cki.ctg, cki.cno, kchidSnd, &kid))
					{
					*pfHaveValid = fTrue;
					break;
					}
				}
			else
				break;
			}
		}
	return fUnused;
}



/***************************************************************************
 *
 * Used to set the title of the movie, based on a file name.
 *
 * Parameters:
 *  pfni - File name.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVIE::_SetTitle(PFNI pfni)
{
	AssertThis(0);

	achar *pch;

	if (pfni == pvNil)
		{
		if (_stnTitle.Cch() == 0)
			{
			Pmcc()->GetStn(idsEngineDefaultTitle, &_stnTitle);
			Pmcc()->UpdateTitle(&_stnTitle);
			}
		return;
		}

	pfni->GetLeaf(&_stnTitle);

	for (pch = _stnTitle.Psz() + _stnTitle.Cch(); (pch > _stnTitle.Psz()) && (*pch != ChLit('.')); pch--)
		{
		}

	if (*pch == ChLit('.'))
		{
		_stnTitle.Delete(pch - _stnTitle.Psz());
		}

	if (_stnTitle.Cch() == 0)
		{
        Pmcc()->GetStn(idsEngineDefaultTitle, &_stnTitle);
		}

	Pmcc()->UpdateTitle(&_stnTitle);
}

//
//
//
//  BEGIN MVU GOODIES
//
//
//

BEGIN_CMD_MAP(MVU, DDG)
	ON_CID_GEN(cidCopyRoute, &MVU::FCmdClip, pvNil)
	ON_CID_GEN(cidCutTool, &MVU::FCmdClip, pvNil)
	ON_CID_GEN(cidShiftCut, &MVU::FCmdClip, pvNil)
	ON_CID_GEN(cidCopyTool, &MVU::FCmdClip, pvNil)
	ON_CID_GEN(cidShiftCopy, &MVU::FCmdClip, pvNil)
	ON_CID_GEN(cidPasteTool, &MVU::FCmdClip, pvNil)
	ON_CID_GEN(cidClose, pvNil, pvNil)
	ON_CID_GEN(cidSave, &MVU::FCmdSave, pvNil)
	ON_CID_GEN(cidSaveAs, &MVU::FCmdSave, pvNil)
	ON_CID_GEN(cidSaveCopy, &MVU::FCmdSave, pvNil)
	ON_CID_GEN(cidIdle, &MVU::FCmdIdle, pvNil)
	ON_CID_GEN(cidRollOff, &MVU::FCmdRollOff, pvNil)
END_CMD_MAP_NIL()

RTCLASS(MVU)

/****************************************************
 *
 * Destructor for movie view objects
 *
 ****************************************************/
MVU::~MVU(void)
{
	if (_tagTool.sid != ksidInvalid)
		TAGM::CloseTag(&_tagTool);
}

/***************************************************************************
 *
 * Create a new mvu.
 *
 * Parameters:
 *	pmvie - Pointer to the creating movie.
 *  pgcb - Pointer to the creation block describing placement, etc.
 *	dxp - Width of the rendered area.
 *	dyp - Height of the rendered area.
 *
 * Returns:
 *  A pointer to the view, otw pvNil on failure
 *
***************************************************************************/
MVU *MVU::PmvuNew(PMVIE pmvie, PGCB pgcb, long dxp, long dyp)
{
	AssertPo(pmvie, 0);
	AssertVarMem(pgcb);

	MVU *pmvu;
	BRS rgr[3][3] = {{rOne, rZero, rZero}, {rZero, rZero, rOne},
		{rZero, -rOne, rZero}};

	//
	// Create the new view
	//
	if ((pmvu = NewObj MVU(pmvie, pgcb)) == pvNil)
		return pvNil;

    //
	// Init it
	//
    if (!pmvu->_FInit())
		{
        ReleasePpo(&pmvu);
		return(pvNil);
		}

    CopyPb(rgr, pmvu->_rgrAxis, size(rgr));
	pmvu->_fRecordDefault = fTrue;
	pmvu->_fRespectGround = fFalse;
	pmvu->_dxp = dxp;
	pmvu->_dyp = dyp;
	pmvu->_tool = toolCompose;
	pmvu->_tagTool.sid = ksidInvalid;

	//
	// Make this the active view
	//
	pmvu->Activate(fTrue);
	return pmvu;
}



/***************************************************************************
 *
 * Set the tool type
 *
 * Parameters:
 *	tool - The new tool to use.
 *
 * Returns:
 *  None.
 *
***************************************************************************/
void MVU::SetTool(long tool)
{
	AssertThis(0);
	AssertPo(Pmvie(), 0);
	AssertNilOrPo(Pmvie()->Pscen(), 0);

	long lwMode; // -1 = Textbox mode, 0 = either mode, 1 = Actor mode
	PTBOX ptbox = pvNil;
	PACTR pactr = pvNil;

	if (Pmvie()->Pscen() != pvNil)
		{

		AssertNilOrPo(Pmvie()->Pscen()->PactrSelected(), 0);
		AssertNilOrPo(Pmvie()->Pscen()->PtboxSelected(), 0);

		pactr = Pmvie()->Pscen()->PactrSelected();
		ptbox = Pmvie()->Pscen()->PtboxSelected();

		if (pactr != pvNil)
			{
			_ptmplTool = pactr->Ptmpl();
			AssertPo(_ptmplTool, 0);
			}

		}
    else
		{
        _ptmplTool = pvNil;
		}

	//
	// Get old tool type
	//
	switch (_tool)
		{
	case toolSoonerLater:

		lwMode = 1;
		if (tool != toolSoonerLater)
			{
			Pmvie()->Pmcc()->EndSoonerLater();

			if ((pactr != pvNil) && pactr->FTimeFrozen())
				{
				pactr->SetTimeFreeze(fFalse);
				pactr->Hilite();
				Pmvie()->InvalViewsAndScb();
				}

			}
		break;

	case toolActorNuke:
	case toolCopyObject:
	case toolCopyRte:
	case toolCutObject:
	case toolPasteObject:
		lwMode = 0;
		break;

	case toolSceneNuke:
	case toolDefault:
	case toolSounder:
	case toolLooper:
	case toolMatcher:
	case toolListener:
	case toolSceneChop:
	case toolSceneChopBack:
	case toolAction:
	case toolActorSelect:
    case toolPlace:
	case toolCompose:
	case toolRecordSameAction:
	case toolRotateX:
	case toolRotateY:
	case toolRotateZ:
	case toolCostumeCmid:
	case toolSquashStretch:
	case toolResize:
	case toolNormalizeRot:
	case toolNormalizeSize:
	case toolActorEasel:

		lwMode = 1;
		break;

	case toolTboxMove:
	case toolTboxUpDown:
	case toolTboxLeftRight:
	case toolTboxFalling:
	case toolTboxRising:
	case toolTboxPaintText:
	case toolTboxFillBkgd:
	case toolTboxStory:
	case toolTboxCredit:
	case toolTboxFont:
	case toolTboxStyle:
	case toolTboxSize:
		lwMode = -1;
		break;

    default:
	    Bug("Unknown tool type");
		}

    //
	// Check if we've changed primary tool type
	//
	switch (tool)
		{

	case toolActorNuke:
	case toolCopyObject:
	case toolCopyRte:
	case toolCutObject:
	case toolPasteObject:
		break;

	case toolSoonerLater:
		_fMouseDownSeen = fFalse;
	
	case toolSceneNuke:
	case toolDefault:
	case toolSounder:
	case toolLooper:
	case toolMatcher:
	case toolListener:
	case toolSceneChop:
	case toolSceneChopBack:
	case toolAction:
	case toolActorSelect:
    case toolPlace:
	case toolCompose:
	case toolRecordSameAction:
	case toolRotateX:
	case toolRotateY:
	case toolRotateZ:
	case toolCostumeCmid:
	case toolSquashStretch:
	case toolResize:
	case toolNormalizeRot:
	case toolNormalizeSize:
	case toolActorEasel:

		_fTextMode = fFalse;

		if (lwMode > 0)
			{
			break;
			}

		if (Pmvie()->Pscen() != pvNil)
			{
			Pmvie()->Pscen()->SelectActr(pactr);
			}
		break;

	case toolTboxMove:
	case toolTboxUpDown:
	case toolTboxLeftRight:
	case toolTboxFalling:
	case toolTboxRising:
	case toolTboxStory:
	case toolTboxCredit:
	case toolTboxFillBkgd:

		_fTextMode = fTrue;

		if (lwMode < 0)
			{
			break;
			}

LSelectTbox:
		if (Pmvie()->Pscen() != pvNil)
			{
			Pmvie()->Pscen()->SelectTbox(ptbox);
			}
	    break;

	case toolTboxPaintText:

		_fTextMode = fTrue;

		if ((lwMode < 0) && (ptbox != pvNil))
			{
			ptbox->FSetAcrText(AcrPaint());
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(tool);
			break;
			}

		goto LSelectTbox;

	case toolTboxFont:
		_fTextMode = fTrue;
		if ((lwMode < 0) && (ptbox != pvNil))
			{
			ptbox->FSetOnnText(OnnTextCur());
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(tool);
			break;
			}
		goto LSelectTbox;

	case toolTboxSize:
		_fTextMode = fTrue;
		if ((lwMode < 0) && (ptbox != pvNil))
			{
			ptbox->FSetDypFontText(DypFontTextCur());
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(tool);
			break;
			}
		goto LSelectTbox;

	case toolTboxStyle:
		_fTextMode = fTrue;
		if ((lwMode < 0) && (ptbox != pvNil))
			{
			ptbox->FSetStyleText(GrfontStyleTextCur());
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(tool);
			break;
			}
		goto LSelectTbox;

	default:
	    Bug("Unknown tool type");
		}

	_tool = tool;
}


/***************************************************************************
 *
 * Change the current loaded tag attached to the cursor.  If the previous
 * tag was a "ksidUseCrf" tag, it must be closed to release its refcount on
 * the tag's pcrf.
 *
 * Parameters:
 *	ptag - The new tag to attach to the cursor
 *
 * Returns:
 *  None.
 *
***************************************************************************/
void MVU::SetTagTool(PTAG ptag)
{
	AssertThis(0);
	AssertVarMem(ptag);

	if (_tagTool.sid != ksidInvalid)
		{
		TAGM::CloseTag(&_tagTool);
		}

#ifdef DEBUG
	// Make sure the new tag has been opened, if it's a "ksidUseCrf" tag
	if (ptag->sid == ksidUseCrf)
		{
		AssertPo(ptag->pcrf, 0);
		}
#endif

	_tagTool = *ptag;
	if (_tagTool.sid != ksidInvalid)
		TAGM::DupTag(ptag);
}



/***************************************************************************
 *
 * Draw this view.
 *
 * Parameters:
 *	pgnv - The environment to write to.
 *  prcClip - The clipping rectangle.
 *
 * Returns:
 *  None.
 *
***************************************************************************/
void MVU::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);

	RC rcDest;

	//
	// Clear non-rendering areas
	//
	if (prcClip->xpRight > _dxp)
		{
		rcDest = *prcClip;
		rcDest.xpLeft = _dxp;
		pgnv->FillRc(&rcDest, kacrWhite);
		}
	if (prcClip->ypBottom > _dyp)
		{
		rcDest = *prcClip;
		rcDest.ypTop = _dyp;
		pgnv->FillRc(&rcDest, kacrWhite);
		}

	//
	// Render
	//
	if (Pmvie()->Pscen() != pvNil)
		{
        Pmvie()->Pbwld()->Render();
		Pmvie()->Pbwld()->Draw(pgnv, prcClip, 0, 0);

		//
		// This draws a currently being dragged out text box frame.
		//
		if (!_rcFrame.FEmpty())
			{
			pgnv->FrameRcApt(&_rcFrame, &vaptLtGray, kacrBlack, kacrWhite);
			}
		}
	else
		{
		rcDest.Set(0, 0, _dxp, _dyp);
		pgnv->FillRc(&rcDest, kacrBlack);
		}
}


/***************************************************************************
 *
 * Warps the cursor to the center of this gob.  Also sets _xpPrev and
 * _ypPrev so that future mouse deltas are from the center.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *	None.
 *
 **************************************************************************/
void MVU::WarpCursToCenter(void)
{
	AssertThis(0);

	PT pt;

	_xpPrev = _dxp / 2;
	_ypPrev = _dyp / 2;
	_dzrPrev = rZero;
	pt.xp = _xpPrev;
	pt.yp = _ypPrev;
	MapPt(&pt, cooLocal, cooGlobal);
	vpappb->PositionCurs(pt.xp, pt.yp);
}


/***************************************************************************
 *
 * Warps the cursor to the center of the given actor.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *	None.
 *
 **************************************************************************/
void MVU::WarpCursToActor(PACTR pactr)
{
	AssertThis(0);
	AssertPo(pactr, 0);

	PT pt;

	pactr->GetCenter(&pt.xp, &pt.yp);
	MapPt(&pt, cooLocal, cooGlobal);
	vpappb->PositionCurs(pt.xp, pt.yp);
}


/***************************************************************************
 *
 * Call this function when you have the cursor hidden and you want to
 * "reset" its position after using the mouse position to adjust an actor.
 * It updates _xpPrev and _ypPrev.  Then, if the cursor has gone outside
 * the gob, it warps the cursor to the center of the gob.  This way, the
 * actor doesn't seem to hit an invisible "wall" just because the (hidden)
 * cursor has hit the edge of the screen.
 *
 * Parameters:
 *	(xp, yp): current cursor position
 *
 * Returns:
 *	None.
 *
 **************************************************************************/
void MVU::AdjustCursor(long xp, long yp)
{
	AssertThis(0);

	RC rc;

	GetRc(&rc, cooLocal);
	rc.Inset(kdpInset, kdpInset); // warp before the cursor gets close to the gob's edge
	if (rc.FPtIn(xp, yp))
		{
		_xpPrev = xp;
		_ypPrev = yp;
		_dzrPrev = rZero;
		}
	else
		{
		WarpCursToCenter();
		}
}


/***************************************************************************
 *
 * Converts from mouse coordinates to world coordinates
 *
 * Parameters:
 *	dxrMouse - BRender scalar representation of mouse X coordinate
 *	dyrMouse - BRender scalar representation of mouse Y coordinate
 *	dzrMouse - BRender scalar representation of mouse Z coordinate
 *  pdxrWld  - Place to store world X coordinate.
 *  pdyrWld  - Place to store world Y coordinate.
 *  pdzrWld  - Place to store world Z coordinate.
 *  fRecord  - Is the conversion to be scaled according to the recording
 *				scaling factor, or the non-recording scaling factor.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVU::MouseToWorld(BRS dxrMouse, BRS dyrMouse, BRS dzrMouse, BRS *pdxrWld,
	BRS *pdyrWld, BRS *pdzrWld, bool fRecord)
{
	AssertThis(0);
	AssertVarMem(pdxrWld);
	AssertVarMem(pdyrWld);
	AssertVarMem(pdzrWld);

	BRS dxrScr, dyrScr, dzrScr;
	BMAT34 bmat34Cam;
	BRS rScaleMouse;

	dxrScr = BR_MAC3(dxrMouse, _rgrAxis[0][0], dyrMouse, _rgrAxis[0][1], dzrMouse, _rgrAxis[0][2]);
	dyrScr = BR_MAC3(dxrMouse, _rgrAxis[1][0], dyrMouse, _rgrAxis[1][1], dzrMouse, _rgrAxis[1][2]);
	dzrScr = BR_MAC3(dxrMouse, _rgrAxis[2][0], dyrMouse, _rgrAxis[2][1], dzrMouse, _rgrAxis[2][2]);

	rScaleMouse = fRecord ? krScaleMouseRecord : krScaleMouseNonRecord;

	//
	// apply some scaling so that 1 pixel of mouse movement is rScaleMouse world units
	//
	dxrScr = BrsMul(dxrScr, rScaleMouse);
	dyrScr = BrsMul(dyrScr, rScaleMouse);
	dzrScr = BrsMul(dzrScr, rScaleMouse);

	Pmvie()->Pscen()->Pbkgd()->GetMouseMatrix(&bmat34Cam);
	*pdxrWld = BR_MAC3(dxrScr, bmat34Cam.m[0][0], dyrScr, bmat34Cam.m[1][0], dzrScr, bmat34Cam.m[2][0]);
	*pdyrWld = BR_MAC3(dxrScr, bmat34Cam.m[0][1], dyrScr, bmat34Cam.m[1][1], dzrScr, bmat34Cam.m[2][1]);
	*pdzrWld = BR_MAC3(dxrScr, bmat34Cam.m[0][2], dyrScr, bmat34Cam.m[1][2], dzrScr, bmat34Cam.m[2][2]);

}


bool MVU::_fKbdDelayed = fFalse;
long MVU::_dtsKbdDelay;
long MVU::_dtsKbdRepeat;

/***************************************************************************
 *
 * Slows down keyboard auto-repeat as much as possible.  Saves user's
 * previous setting so it can be restored in RestoreKeyboardRepeat.
 *
 * Parameters:
 *  none
 *
 * Returns
 *  none
 *
 **************************************************************************/
void MVU::SlowKeyboardRepeat(void)
{
	if (_fKbdDelayed)
		return;
#ifdef WIN
	if (!SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &_dtsKbdDelay, fFalse))
		{
		Bug("why could this fail?");
		return;
		}
	if (!SystemParametersInfo(SPI_SETKEYBOARDDELAY, klwMax, pvNil, fFalse))
		{
		Bug("why could this fail?");
		return;
		}
	if (!SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &_dtsKbdRepeat, fFalse))
		{
		Bug("why could this fail?");
		return;
		}
	if (!SystemParametersInfo(SPI_SETKEYBOARDSPEED, 0, pvNil, fFalse))
		{
		Bug("why could this fail?");
		return;
		}
#endif
#ifdef MAC
	RawRtn();
#endif
	_fKbdDelayed = fTrue;
}


/***************************************************************************
 *
 * Restores the keyboard auto-repeat to the user's previous setting.
 *
 * Parameters:
 *  none
 *
 * Returns
 *  none
 *
 **************************************************************************/
void MVU::RestoreKeyboardRepeat(void)
{
	if (!_fKbdDelayed)
		return;
#ifdef WIN
	if (!SystemParametersInfo(SPI_SETKEYBOARDDELAY, _dtsKbdDelay, pvNil, fFalse))
		{
		Bug("why could this fail?");
		return;
		}
	if (!SystemParametersInfo(SPI_SETKEYBOARDSPEED, _dtsKbdRepeat, pvNil, fFalse))
		{
		Bug("why could this fail?");
		return;
		}
#endif
#ifdef MAC
	RawRtn();
#endif
	_fKbdDelayed = fFalse;
}


/***************************************************************************
 *
 * An actor has just been added, so enter "place actor" mode, where the
 * actor floats with the cursor.
 *
 * Parameters:
 *  fEntireScene flags whether the whole scene's route is to be translated on
 *  positioning, or whether only the current subroute is to be translated.
 *
 * Returns
 *  None.
 *
 **************************************************************************/
void MVU::StartPlaceActor(bool fEntireScene)
{
	AssertThis(0);
	AssertPo(Pmvie()->Pscen(), 0);

	PACTR pactr = Pmvie()->Pscen()->PactrSelected();

	AssertPo(pactr, 0);

	vpappb->HideCurs();
	WarpCursToCenter();

	_fEntireScene = fEntireScene;
	SetTool(toolPlace);
	Pmvie()->RemFromRollCall(pactr, fFalse);
	Pmvie()->Pmcc()->NewActor();
	vpcex->TrackMouse(this);

	// While tracking the mouse, don't allow the cursor out of capture window.
	// If we don't do this, then the user can click on another window in the
	// middle of the cursor tracking, (if tracking with mouse btn up). Note,
	// this call clips the cursor movement to an area on the screen, so we are
	// assuming there is no way for the capture window to move during tracking.
#ifdef WIN
	RECT rectCapture;
	GetWindowRect(HwndContainer(), &rectCapture);
	ClipCursor(&rectCapture);
#endif //WIN

	_fMouseDownSeen = fFalse;
	_tsLastSample = TsCurrent();
	SlowKeyboardRepeat();

	return;
}



/***************************************************************************
 *
 * Undoes the place tool.
 *
 * Parameters:
 *  None.
 *
 * Returns
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
void MVU::EndPlaceActor()
{
	AssertThis(0);
	AssertPo(Pmvie()->Pscen(), 0);

	if (Tool() != toolPlace)
		{
		return;
		}

	vpappb->ShowCurs();
	WarpCursToCenter();
	SetTool(toolCompose);
	AssertDo(Pmvie()->FAddToRollCall(Pmvie()->Pscen()->PactrSelected(), pvNil), "Should never fail");
	Pmvie()->Pmcc()->ChangeTool(toolCompose);
	vpcex->EndMouseTracking();

	_fMouseDownSeen = fFalse;

	return;
}
/***************************************************************************
 *
 * Track the mouse moves and set the cursor appropriately.
 *
 * Parameters:
 *	pcmd - The command information.
 *
 * Returns:
 *  fTrue - indicating that the command was processed.
 *
***************************************************************************/
bool MVU::FCmdMouseMove(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PACTR pactr;
	long ibset;
	PDOCB pdocb;

	AssertPo(Pmvie(), 0);
	if (Pmvie()->Pscen() == pvNil)
		{
		Pmvie()->Pmcc()->SetCurs(toolDefault);
		return(fTrue);
		}

	if (Pmvie()->FPlaying())
		{
		Pmvie()->Pmcc()->SetCurs(toolDefault);
		return(fTrue);
		}

	switch (Tool())
		{
	case toolTboxStory:
	case toolTboxCredit:
	case toolTboxPaintText:
	case toolTboxFillBkgd:
	case toolTboxMove:
	case toolTboxFont:
	case toolTboxSize:
	case toolTboxStyle:
		Pmvie()->Pmcc()->SetCurs(toolDefault);
		break;

	case toolPlace:
		return(fFalse);

	case toolSceneNuke:
	case toolSceneChop:
	case toolSceneChopBack:
		Pmvie()->Pmcc()->SetCurs(Tool());
		break;

	case toolSounder:
	case toolLooper:
	case toolMatcher:
		if (_tagTool.sid == ksidInvalid)
			Pmvie()->Pmcc()->SetCurs(toolDefault);
		else
			Pmvie()->Pmcc()->SetCurs(Tool());
		break;

	case toolListener:
		Pmvie()->Pmcc()->SetCurs(Tool());
		// Audition sound if over an actor
		pactr = Pmvie()->Pscen()->PactrFromPt(pcmd->xp, pcmd->yp, &ibset);
 		if (pvNil != pactr)
 			{
			if (pactr != _pactrListener)
				{
				Pmvie()->Pmsq()->StopAll();
	 			pactr->FReplayFrame(fscenSounds);	// Ignore audition error; non-fatal
				// Play outstanding sounds
				Pmvie()->Pmsq()->PlayMsq();
				}
			_fMouseOn = fFalse;
			}
		else if (!_fMouseOn)
			{
			_fMouseOn = fTrue;
			Pmvie()->Pmsq()->StopAll();
			Pmvie()->Pscen()->FReplayFrm(fscenSounds);
			// Play outstanding sounds
			Pmvie()->Pmsq()->PlayMsq();
			}

		_pactrListener = pactr;
		break;

	case toolSoonerLater:
		if (!_fTextMode)
			{
	        AssertPo(Pmvie()->Pscen(), 0);
			pactr = Pmvie()->Pscen()->PactrFromPt(pcmd->xp, pcmd->yp, &ibset);
			AssertNilOrPo(pactr, 0);
			if (pactr == pvNil)
				{
				Pmvie()->Pmcc()->SetCurs(toolDefault);
				break;
				}
			else if (_fMouseDownSeen)
				{
				Pmvie()->Pmcc()->SetCurs(toolCompose);
				}
			else
				{
				Pmvie()->Pmcc()->SetCurs(Tool());
				}
			}
		else
			{
			Pmvie()->Pmcc()->SetCurs(toolDefault);
			}
		break;

	case toolAction:
	case toolActorNuke:
	case toolActorSelect:
	case toolCompose:
	case toolRecordSameAction:
	case toolRotateX:
	case toolRotateY:
	case toolRotateZ:
	case toolCostumeCmid:
	case toolSquashStretch:
	case toolResize:
	case toolNormalizeRot:
	case toolNormalizeSize:
	case toolCopyObject:
	case toolPasteObject:
	case toolCopyRte:
	case toolCutObject:
	case toolActorEasel:

		if (!_fTextMode)
			{
	        AssertPo(Pmvie()->Pscen(), 0);
			pactr = Pmvie()->Pscen()->PactrFromPt(pcmd->xp, pcmd->yp, &ibset);
			AssertNilOrPo(pactr, 0);
			if (pactr == pvNil)
				{
				Pmvie()->Pmcc()->SetCurs(toolDefault);
				break;
				}
			else if ((Tool() == toolCompose) && (pcmd->grfcust & fcustCmd))
				{
				Pmvie()->Pmcc()->SetCurs(toolTweak);
				}
			else if ((Tool() == toolCompose) && (pcmd->grfcust & fcustShift))
				{
				Pmvie()->Pmcc()->SetCurs(toolComposeAll);
				}
			else if ((Tool() == toolPasteObject) &&
					vpclip->FGetFormat(kclsACLP, &pdocb))
				{
				if (((PACLP)pdocb)->FRouteOnly())
					Pmvie()->Pmcc()->SetCurs(toolPasteRte);
				else
					Pmvie()->Pmcc()->SetCurs(Tool());
				ReleasePpo(&pdocb);
				}
			else
				{
				Pmvie()->Pmcc()->SetCurs(Tool());
				}
			}
		else
			{
			Pmvie()->Pmcc()->SetCurs(toolDefault);
			}
		break;

	case toolDefault:
		Pmvie()->Pmcc()->SetCurs(toolDefault);
		break;

	default:
	    Bug("Unknown tool type");
		}

	return(fTrue);
}

/***************************************************************************
 *
 * Track the mouse and do the appropriate command.
 *
 * Parameters:
 *	pcmdTrack - The command information.
 *
 * Returns:
 *  fTrue - indicating that the command was processed.
 *
***************************************************************************/
bool MVU::FCmdTrackMouse(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	RC rc;

	if (pcmd->cid == cidMouseDown)
		{
		Assert(vpcex->PgobTracking() == pvNil, "mouse already being tracked!");
		vpcex->TrackMouse(this);
		}
	else
		{
		Assert(vpcex->PgobTracking() == this, "not tracking mouse!");
		Assert(pcmd->cid == cidTrackMouse, 0);
		}

	if ((pcmd->cid == cidMouseDown) || ((pcmd->grfcust & fcustMouse) && !_fMouseDownSeen))
		{
		_MouseDown(pcmd);
		}
	else
		{
		_MouseDrag(pcmd);
		}	

	if (!(pcmd->grfcust & fcustMouse))
		{
		_MouseUp(pcmd);
		}

	return fTrue;
}


/***************************************************************************
 *
 * Handle positioning an actor when the place tool is in effect.
 *
 * Parameters:
 *  dxrWld, dyrWld, dzrWld - the change in position in worldspace
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVU::_PositionActr(BRS dxrWld, BRS dyrWld, BRS dzrWld)
{
	AssertThis(0);
	Assert(Tool() == toolPlace, "Wrong tool in effect");

	PMVIE pmvie;
	PSCEN pscen;
	bool  fMoved;
	PACTR pactr = pvNil;
	ulong grfmaf = fmafOrient;

	pmvie = Pmvie();
	AssertPo(pmvie, 0);

	pscen = pmvie->Pscen();
	AssertPo(pscen, 0);

	pactr = pscen->PactrSelected();
	AssertPo(pactr, 0);

	if (_fEntireScene)
		{
		grfmaf |= fmafEntireScene;
		}
	else
		{
		grfmaf |= fmafEntireSubrte;
		}

	if (FRespectGround())
		{
		grfmaf |= fmafGround;
		}

	// FMoveRouteCore cannot fail on fmafEntireSubrte as no events are added
	if (pactr->FMoveRoute(dxrWld, dyrWld, dzrWld, &fMoved, grfmaf) && fMoved)
		{
		Pmvie()->Pbwld()->MarkDirty();
		Pmvie()->MarkViews();
		pscen->Pbkgd()->ReuseActorPlacePoint();
		}
}

/***************************************************************************
 *
 * Notify script that an actor was clicked.  Note that we sometimes call
 * this function with fDown fFalse even though the user hasn't mouseup'ed
 * yet, because of bringing up easels on mousedown.
 *
 * Parameters:
 *	pactr - the actor that was clicked
 *  fDown - fTrue if we're mousedown'ing the actor, fFalse if mouseup
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVU::_ActorClicked(PACTR pactr, bool fDown)
{
	AssertThis(0);
	AssertPo(pactr, 0);

	ulong grftmpl = 0;

	if (pactr->Ptmpl()->FIsTdt())
		{
		grftmpl |= ftmplTdt;
		}

	if (pactr->Ptmpl()->FIsProp())
		{
		grftmpl |= ftmplProp;
		}

	if (fDown)
		{
		vpcex->EnqueueCid(cidActorClickedDown, pvNil, pvNil, pactr->Arid(),
			pactr->Ptmpl()->Cno(), grftmpl);
		}
	else
		{
		vpcex->EnqueueCid(cidActorClicked, pvNil, pvNil, pactr->Arid(),
			pactr->Ptmpl()->Cno(), grftmpl);
		}
}

/***************************************************************************
 *
 * Handle Mousedown
 *
 * Parameters:
 *	pcmd - The mouse command
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVU::_MouseDown(CMD_MOUSE *pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PACTR pactr = pvNil;
	PACTR pactrDup;
	PTBOX ptbox;
	PAUND paund;
	PT pt;
	long ibset;
	PDOCB pdocb;

	SlowKeyboardRepeat();

	if (Pmvie()->FPlaying())
		{
		//
		// If we are in the middle of playing, ignore mouse down.
		//
		return;
		}

	AssertPo(Pmvie(), 0);
	if (pvNil == Pmvie()->Pscen())
		{
		return;
		}
	AssertPo(Pmvie()->Pscen(), 0);

	if (_fTextMode)
		{
		ptbox = Pmvie()->Pscen()->PtboxSelected();
		}
	else if ((Tool() != toolPlace) &&
			 (Tool() != toolSceneChop) &&
			 (Tool() != toolSceneChopBack))
		{

		//
		// Select the actor under the cursor
		//
		pactr = Pmvie()->Pscen()->PactrSelected();
		AssertNilOrPo(pactr, 0);

		if ((pactr != pvNil) && pactr->FTimeFrozen())
			{
			pactr->SetTimeFreeze(fFalse);
			}

		pactrDup = Pmvie()->Pscen()->PactrFromPt(pcmd->xp, pcmd->yp, &ibset);

        //
		// Use previously selected actor if mouse in the actor.
		// Don't change the selected actor if we're using the default tool
		//
		if (((pactr == pvNil) ||
			 !pactr->FIsInView() ||
			 !pactr->FPtIn(pcmd->xp, pcmd->yp, &ibset)) &&
			Tool() != toolDefault)
			{
			pactr = pactrDup;
			AssertNilOrPo(pactr, 0);
			}

		if (pvNil != pactr)
			{
			_ActorClicked(pactr, fTrue);
			}
		Pmvie()->Pscen()->SelectActr(pactr); 	   // okay even if pactr is pvNil
		Pmvie()->Pbwld()->MarkDirty();
		}

#ifdef DEBUG
	// Authoring hack to write out background starting pos data

	if (pvNil != pactr && ((vpappb->GrfcustCur(fFalse) & (fcustShift | fcustCmd | fcustOption)) ==
		(fcustShift | fcustCmd | fcustOption)))
		{
		BRS xr;
		BRS yr;
		BRS zr;

		pactr->Pbody()->GetPosition(&xr, &yr, &zr);
		if (!Pmvie()->Pscen()->Pbkgd()->FWritePlaceFile(xr, yr, zr))
			{
			Bug("ouch.  bkgd write failed.");
			}
		else
			{
			Warn("Wrote bkgd actor start point.");
			}
		}
#endif // DEBUG

	_xpPrev = pcmd->xp;
	_ypPrev = pcmd->yp;
	_dzrPrev = rZero;
	_grfcust = pcmd->grfcust;
	_tsLastSample = TsCurrent();

	switch(Tool())
		{

	// Sound tools get handled together:
	case toolSounder:
	case toolLooper:
	case toolMatcher:
		if (pactr == pvNil) // scene sound
			{
			bool fLoop = (Tool() == toolLooper);
			bool fQueue = FPure(_grfcust & fcustCmd);

			if ((Tool() == toolMatcher) && (ksidInvalid != _tagTool.sid))
				{
			    PushErc(ercSocBadSceneSound);				
				break;
				}
			if (ksidInvalid != _tagTool.sid)
				{
				Pmvie()->FAddBkgdSnd(&_tagTool, fLoop, fQueue, vlmNil, styNil);
				}
			}
		else // actor sound
			{
			bool fLoop = (Tool() == toolLooper);
			bool fQueue = FPure(_grfcust & fcustCmd);
			bool fActnCel = FPure(Tool() == toolMatcher);

			if (ksidInvalid != _tagTool.sid)
				{
				Pmvie()->FAddActrSnd(&_tagTool, fLoop, fQueue, fActnCel, vlmNil, styNil);
				}
			}
		break;

	case toolListener:
		// Start the listener easel
		vpcex->EndMouseTracking();
		RestoreKeyboardRepeat();
		if (pvNil != pactr)
			{
			_ActorClicked(pactr, fFalse);
			}
		Pmvie()->Pmcc()->StartListenerEasel();
		break;

	case toolActorSelect:
		Pmvie()->Pmcc()->PlayUISound(Tool());
		break;

	case toolSceneNuke:		
		if (Pmvie()->FRemScen(Pmvie()->Iscen()))
			{
			Pmvie()->Pmcc()->UpdateRollCall();
			Pmvie()->Pmcc()->SceneNuked();
			Pmvie()->InvalViewsAndScb();
			Pmvie()->Pmcc()->PlayUISound(Tool());
			}
		break;

	case toolActorNuke:
		if (pactr == pvNil)
			{
            break;
			}

		Pmvie()->FRemActr();
		Pmvie()->Pmcc()->PlayUISound(Tool());
		break;

	case toolSceneChop:
		if (Pmvie()->Pscen()->FChop())
			{
			Pmvie()->InvalViewsAndScb();
			Pmvie()->Pmcc()->PlayUISound(Tool());
			}
		break;

	case toolSceneChopBack:
		if (Pmvie()->Pscen()->FChopBack())
			{
			Pmvie()->InvalViewsAndScb();
			Pmvie()->Pmcc()->PlayUISound(Tool());
			}
		break;

	case toolCutObject:
	case toolCopyObject:
	case toolCopyRte:
		if (!_fTextMode)
			{
			FDoClip(Tool());
			}
		break;

	case toolPasteObject:
		if (!_fTextMode)
			{
			if (vpclip->FGetFormat(kclsACLP, &pdocb) &&
				((PACLP)pdocb)->FRouteOnly() &&
				(pactr != pvNil))
				{
				FDoClip(Tool());
				ReleasePpo(&pdocb);
				}
			}
		break;

	case toolTboxStory:
	case toolTboxCredit:
	case toolTboxPaintText:
	case toolTboxFillBkgd:
	case toolTboxMove:
	case toolTboxFont:
	case toolTboxSize:
	case toolTboxStyle:
		Pmvie()->Pscen()->SelectTbox(pvNil);
		break;

	case toolPlace:
		_fMouseDownSeen = fTrue;
		break;
		
	case toolSoonerLater:	
		if ((pactr != pvNil) && !_fMouseDownSeen)
			{
			pactr->SetTimeFreeze(fTrue);
			pactr->Hilite();
			_fMouseDownSeen = fTrue;
			Pmvie()->Pmcc()->PlayUISound(Tool());
			}
		else
			{
			goto LEnd;
			}
		break;

	case toolCompose:
	case toolRotateX:
	case toolRotateY:
	case toolRotateZ:
	case toolResize:
	case toolSquashStretch:
		if (pactr != pvNil)
			{

			vpappb->HideCurs();

			//
			// Create an actor undo object
			//
			paund = AUND::PaundNew();
			if ((paund == pvNil) || !pactr->FDup(&pactrDup, fTrue))
				{
                Pmvie()->ClearUndo();
			    PushErc(ercSocNotUndoable);
				}
			else
				{
				paund->SetPactr(pactrDup);
				ReleasePpo(&pactrDup);	  	
				paund->SetArid(pactr->Arid());

				//
				// Store it.  We will only add it if there is a change done
				// to the actor.
				//
				_paund = paund;
				}

			if ((Tool() != toolResize) && (Tool() != toolSquashStretch))
				{
				Pmvie()->Pmcc()->PlayUISound(Tool(), _grfcust);
				}
			else
				{
				_lwLastTime = 0;
				}

			}
		break;

	case toolNormalizeRot:
		if (pactr != pvNil)
			{
			Pmvie()->Pmcc()->PlayUISound(Tool());		
			pactr->FNormalize(fnormRotate);
			}
		break;
	case toolNormalizeSize:
		if (pactr != pvNil)
			{
			Pmvie()->Pmcc()->PlayUISound(Tool());		
			pactr->FNormalize(fnormSize);
			}
		break;

	case toolRecordSameAction:

		if (pactr != pvNil)
			{
			long anidTool = pactr->AnidCur();
			long anid = anidTool;
			long celn = 0;
			bool fFrozen;
			
			SetAnidTool(pactr->AnidCur());
			_ptmplTool = pactr->Ptmpl();

			if ((pcmd->grfcust & fcustShift) &&
				(pcmd->grfcust & fcustCmd) &&
				FRecordDefault())
				{
				fFrozen = fFalse;
				_fCyclingCels = fFalse;
				_fSetFRecordDefault = fTrue;
				SetFRecordDefault(fFalse);
				}
			else
				{
				fFrozen = FPure(pcmd->grfcust & fcustShift);
				_fCyclingCels = FPure(pcmd->grfcust & fcustCmd);
				}

			if ((pactr->Ptmpl() != _ptmplTool) &&
				!(_ptmplTool->FIsTdt() && pactr->Ptmpl()->FIsTdt()))
				{
                PushErc(ercSocActionNotApplicable);
				return;
				}

			vpappb->HideCurs();
			
			// first, call FSetAction	
			if (anidTool == ivNil)
				{
				anid = pactr->AnidCur();
				}
			if (pactr->AnidCur() == anid)
				{
				celn = pactr->CelnCur();
				}

			// note that FSetAction creates an undo object
			// NOTE:  FSetAction() must be called on each use
			// of toolRecordSameAction. (It is not redundant).
			// Otherwise, resizing can break wysiwyg, as the final
			// path point probably won't be a complete cel's distance
			// from the previous step.
			Assert(pvNil == _pactrRestore, "_pactrRestore should not require releasing");
			ReleasePpo(&_pactrRestore); 	// To be safe
			if (!pactr->FSetAction(anid, celn, fFrozen, &_pactrRestore))
				{
				break; // an Oom erc has already been pushed
				}
			SetAnidTool(ivNil);
			_tsLast = TsCurrent();
			pactr->SetTsInsert(_tsLast);			
			Pmvie()->Pmcc()->PlayUISound(Tool());
			}

		break;

	case toolAction:
		//
		// Start the action browser
		//
		vpcex->EndMouseTracking();
		RestoreKeyboardRepeat();
	
		if (pactr != pvNil)
			{
			_ActorClicked(pactr, fFalse);
			Pmvie()->Pscen()->SelectActr(pactr);
			_ptmplTool = pactr->Ptmpl();
			Pmvie()->Pmcc()->StartActionBrowser();
			}
		break;

	case toolCostumeCmid:
		if (pactr != pvNil)
			{
			TAG tag; // unused
			TrashVar(&tag);
			TrashVar(&ibset);
			Pmvie()->FCostumeActr(ibset, &tag, CmidTool(), fTrue);
			}
		break;
	case toolActorEasel:
		if (pactr != pvNil)
			{
			bool fActrChanged;

			_ActorClicked(pactr, fFalse);
			Pmvie()->Pmcc()->ActorEasel(&fActrChanged);
			if (fActrChanged)
				{
			    Pmvie()->SetDirty();
				Pmvie()->ClearUndo();
				}
			}
		break;

	case toolDefault:
		/* Do nothing */
		break;

	default:
		Bug("Tool unknown on mouse down");
		}

	if (Pmvie()->FSoundsEnabled())
		{
		Pmvie()->Pmsq()->PlayMsq();
		}
	else
		{
		Pmvie()->Pmsq()->FlushMsq();
		}
	_fMouseDownSeen = fTrue;

LEnd:
	Pmvie()->MarkViews();

}


/***************************************************************************
 *
 * Handle Mouse drag (mouse move while button down)
 *
 * Parameters:
 *  pcmd - The mouse command
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVU::_MouseDrag(CMD_MOUSE *pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PMVIE pmvie;
	PSCEN pscen;
	PACTR pactr = pvNil;
	BRS dxrMouse, dyrMouse, dzrMouse;
	BRS dxrWld, dyrWld, dzrWld; // amount moved from previous point in world space
	BRS zrActr, zrCam, dzrActr;
	bool fArrowKey = fFalse;
	RC rc;
	PT pt;

	if (Pmvie()->FPlaying() || Pmvie()->Pmcc()->FMinimized())
		{
		//
		// If we are in the middle of playing, or been minimized, ignore mouse dragging.
		//
		return;
		}

	pmvie = Pmvie();
	AssertPo(pmvie, 0);
	pscen = pmvie->Pscen();
	AssertNilOrPo(pscen, 0);

	if (pvNil == pscen)
		{
        return;
		}
	
	pactr = pscen->PactrSelected();

	AssertNilOrPo(pactr, 0);

	if (pactr == pvNil)
		{
		return;
		}


	dxrMouse = BrsSub(BrIntToScalar(pcmd->xp), BrIntToScalar(_xpPrev));
	dyrMouse = BrsSub(BrIntToScalar(_ypPrev), BrIntToScalar(pcmd->yp));
	dzrMouse = _dzrPrev;
#ifdef WIN
	//
	// Get the "mouse Z" by sampling the arrow keys.  If an arrow key
	// is down, the number of pixels moved is the number of seconds
	// since the keyboard was last sampled times kdwrMousePerSecond.
	//
	ulong dts = LwMax(1, TsCurrent() - _tsLastSample);
	BRS drSec = BrsDiv(BrIntToScalar(dts), BR_SCALAR(kdtsSecond));
	if (GetKeyState(VK_UP) < 0)
		{
		fArrowKey = fTrue;
		dzrMouse += BrsMul(drSec, kdwrMousePerSecond);
		}
	if (GetKeyState(VK_DOWN) < 0)
		{
		fArrowKey = fTrue;
		dzrMouse -= BrsMul(drSec, kdwrMousePerSecond);
		}
	if (fArrowKey && Tool() == toolRecordSameAction &&
		!_fCyclingCels && pactr->FIsModeRecord())
		{
		if ((BrsAbs(dxrMouse) * 4 < BrsAbs(dzrMouse)) &&
			(BrsAbs(dyrMouse) * 4 < BrsAbs(dzrMouse)))
			{
			// When the up/down arrow keys are used, infinitesimal
			// mouse movement should not be observed or determine path direction.
			dxrMouse = rZero;
			dyrMouse = rZero;
			}
		}
	_dzrPrev = dzrMouse;
	_tsLastSample = TsCurrent();
#endif
#ifdef MAC
	RawRtn();
#endif

	MouseToWorld(dxrMouse, dyrMouse, dzrMouse, &dxrWld, &dyrWld, &dzrWld, Tool() == toolRecordSameAction);

	//
	// Scale movement based on selected actor
	//
	pactr->GetXyzWorld(pvNil, pvNil, &zrActr);
	pscen->Pbkgd()->GetCameraPos(pvNil, pvNil, &zrCam);
	dzrActr = BrsAbs(BrsSub(zrActr, zrCam));
	dzrActr = BrsDiv(dzrActr, kzrMouseScalingFactor);
	if (dzrActr < rOne)
		{
		dzrActr = rOne;
		}
	dxrWld = BrsMul(dxrWld, BrsMul(dzrActr, BR_SCALAR(1.1)));
	dyrWld = BrsMul(dyrWld, BrsMul(dzrActr, BR_SCALAR(1.1)));
	dzrWld = BrsMul(dzrWld, BrsMul(dzrActr, BR_SCALAR(1.1)));


	switch(Tool())
		{
	default:
		Bug("Tool unknown on mouse move");
		break;

	case toolDefault:
	case toolActorSelect:
		break;

	case toolSceneNuke:
	case toolActorNuke:
	case toolSceneChop:
	case toolSceneChopBack:
	case toolCutObject:
	case toolCopyObject:
	case toolPasteObject:
	case toolCopyRte:
	case toolTboxPaintText:
	case toolTboxFillBkgd:
	case toolTboxMove:
	case toolTboxFont:
	case toolTboxSize:
	case toolTboxStory:
	case toolTboxCredit:
	case toolTboxStyle:
	case toolActorEasel:
	case toolSounder:
	case toolLooper:
	case toolMatcher:
		break;

	case toolPlace:
		_PositionActr(dxrWld, dyrWld, dzrWld);
		AdjustCursor(pcmd->xp, pcmd->yp);
		break;

	case toolCompose:
		{
		ulong grfmaf = fmafNil;
		bool fMoved;

		if (_fRespectGround)
			{
			grfmaf |= fmafGround;
			}

		if (_grfcust & fcustCmd)
			{
    		AdjustCursor(pcmd->xp, pcmd->yp);
	
			if (pactr->FTweakRoute(dxrWld, dyrWld, dzrWld, grfmaf))
    			{
				if (fMoved)
					{
					if ((_paund != pvNil) && !Pmvie()->FAddUndo(_paund))
						{
						PushErc(ercSocNotUndoable);
						Pmvie()->ClearUndo();
						}

					ReleasePpo(&_paund);
					}
				}

    		Pmvie()->MarkViews();
			}
		else
			{

			if (_grfcust & fcustShift)
				{
				grfmaf |= fmafEntireSubrte;
				}

    		// FMoveRoute returns fTrue if the distance moved was non-zero
			if (pactr->FMoveRoute(dxrWld, dyrWld, dzrWld, &fMoved, grfmaf))
    			{
				if (fMoved)
					{
					if ((_paund != pvNil) && !Pmvie()->FAddUndo(_paund))
						{
						PushErc(ercSocNotUndoable);
						Pmvie()->ClearUndo();
						}

					ReleasePpo(&_paund);

					AdjustCursor(pcmd->xp, pcmd->yp);
					Pmvie()->Pbwld()->MarkDirty();
					Pmvie()->MarkViews();
					}
    			}
			}
		}
   		break;

	case toolRotateX:
	case toolRotateY:
	case toolRotateZ:
		{
		BRS brs;
		BRA xa, ya, za;

		brs = BrsMul(dxrMouse + dyrMouse, -krRotateScaleFactor);

		xa = aZero;
		ya = aZero;
		za = aZero;

		switch (Tool())
			{
		case toolRotateX:
			xa = BrScalarToAngle(brs);
			break;
		case toolRotateY:
			ya = -BrScalarToAngle(brs);	
			break;
		case toolRotateZ:
			za = BrScalarToAngle(brs);
			break;
			}

		if (pmvie->FRotateActr(xa, ya, za, FPure(_grfcust & fcustCmd)))
			{
   			if ((_paund != pvNil) && !Pmvie()->FAddUndo(_paund))
   				{
   				PushErc(ercSocNotUndoable);
   				Pmvie()->ClearUndo();
   				}

   			ReleasePpo(&_paund);			
			}

		Pmvie()->Pbwld()->MarkDirty();
		Pmvie()->MarkViews();
		AdjustCursor(pcmd->xp, pcmd->yp);
		}
		break;
	
	case toolResize:
		{
		BRS brs;
		BRS brs2;

		brs = BrsMul(dxrMouse + dyrMouse, krRotateScaleFactor);
		brs2 = BrsAdd(brs, rOne);

		//
		// Play UI sound
		//
		if ((((dxrMouse + dyrMouse) < 0) && !(_lwLastTime < 0)) ||
            (((dxrMouse + dyrMouse) > 0) && !(_lwLastTime > 0)))
			{
			_lwLastTime = dxrMouse + dyrMouse;
			Pmvie()->Pmcc()->StopUISound();
			Pmvie()->Pmcc()->PlayUISound(Tool(), (dxrMouse + dyrMouse > 0) ? 0 : fcustShift);
			}

		if (pmvie->FScaleActr(brs2))
			{
   			if ((_paund != pvNil) && !Pmvie()->FAddUndo(_paund))
   				{
   				PushErc(ercSocNotUndoable);
   				Pmvie()->ClearUndo();
   				}

   			ReleasePpo(&_paund);			
			}

		Pmvie()->Pbwld()->MarkDirty();
		Pmvie()->MarkViews();
		AdjustCursor(pcmd->xp, pcmd->yp);
		}
		break;

	case toolSquashStretch:
		{
		BRS brs;
		BRS brs2;

		brs = BrsMul(-dxrMouse - dyrMouse, krRotateScaleFactor);
		brs2 = BrsAdd(brs, rOne);

		//
		// Play UI sound
		//
		if ((((-dxrMouse - dyrMouse) < 0) && !(_lwLastTime < 0)) ||
            (((-dxrMouse - dyrMouse) > 0) && !(_lwLastTime > 0)))
			{
			_lwLastTime = -dxrMouse - dyrMouse;
			Pmvie()->Pmcc()->StopUISound();
			Pmvie()->Pmcc()->PlayUISound(Tool(), (-dxrMouse - dyrMouse < 0) ? 0 : fcustShift);
			}

		if (pmvie->FSquashStretchActr(brs2))
			{
   			if ((_paund != pvNil) && !Pmvie()->FAddUndo(_paund))
   				{
   				PushErc(ercSocNotUndoable);
   				Pmvie()->ClearUndo();
   				}

   			ReleasePpo(&_paund);			
			}

		Pmvie()->Pbwld()->MarkDirty();
		Pmvie()->MarkViews();
		AdjustCursor(pcmd->xp, pcmd->yp);
		}
		break;

	case toolSoonerLater:
	case toolNormalizeRot:
	case toolNormalizeSize:
	case toolCostumeCmid:
		break;

	case toolRecordSameAction:
		{
		bool fLonger;
		bool fStep;
		ulong tsCurrent = TsCurrent();
		ulong grfmaf = 0;
		bool fFrozen = FPure((pcmd->grfcust & fcustShift) && !(pcmd->grfcust & fcustCmd));

		if ((pactr->Ptmpl() != _ptmplTool) &&
			!(_ptmplTool->FIsTdt() && pactr->Ptmpl()->FIsTdt()))
			{
			return;
			}

		// If have stopped cycling cels
		if (_fCyclingCels && !(pcmd->grfcust & fcustCmd))
			{
			_fCyclingCels = fFalse;
			_tsLast = tsCurrent;
			}

		// Start recording unless we're cycling cels
		if (!_fCyclingCels && !pactr->FIsModeRecord() &&
			pactr->FIsRecordValid(dxrWld, dyrWld, dzrWld, tsCurrent))
			{
			if (!pactr->FBeginRecord(tsCurrent, FRecordDefault(), _pactrRestore))
				break; // an Oom erc has already been pushed
			
			// If rerecording, nuke the remainder of the subroute
			if (FRecordDefault())
				pactr->DeleteFwdCore(fFalse);

			Pmvie()->Pmcc()->Recording(fTrue, FRecordDefault());

			}
	
		if (_fCyclingCels)
			{
			if (pcmd->grfcust & fcustCmd) // still cycling
				{
				if ((tsCurrent - _tsLast) < kdtsCycleCels)
					{
					break;
					}
				_tsLast = tsCurrent;
				if (!pactr->FSetActionCore(pactr->AnidCur(), pactr->CelnCur() + 1, fFrozen))
					{
					break; // an Oom erc has already been pushed
					}
				Pmvie()->MarkViews();
				}
			}
		else if (pactr->FIsModeRecord()) // just recording
			{
			if ((tsCurrent - _tsLast) < kdtsFrame)
				{
				break;
				}
			_tsLast = tsCurrent;
			if (fFrozen)
				{
				grfmaf |= fmafFreeze;
				}

            if (_fRespectGround)
				{
                grfmaf |= fmafGround;
				}

			if (!pactr->FRecordMove(dxrWld, dyrWld, dzrWld, grfmaf,	tsCurrent,
				&fLonger, &fStep, _pactrRestore))
				{
		        // Oom erc already pushed
				break;
				}
			if (fLonger)  	//If a point was added to the path
				{
				// update scroll bars
				Pmvie()->Pmcc()->UpdateScrollbars();
				if (fStep)
					AdjustCursor(pcmd->xp, pcmd->yp);
				Pmvie()->MarkViews();
				}
			}
		}
		break;
		}
}


/***************************************************************************
 *
 * Handle Mouseup
 *
 * Parameters:
 *  pcmd - The mouse command.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVU::_MouseUp(CMD_MOUSE *pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PMVIE pmvie;
	PSCEN pscen;
	PACTR pactr = pvNil;
	PACTR pactrDup;
	PSUNA psuna;

	pmvie = Pmvie();
	AssertPo(pmvie, 0);

	_grfcust = fcustNil;

	if (Tool() != toolPlace || _fMouseDownSeen)
		RestoreKeyboardRepeat();

	if (_fPause)
		{

		Assert(Pmvie()->FPlaying(), "Bad Pause type");

		//
		// If we are pausing in the middle of playing, restart playing
		//
        if (!pmvie->Pclok()->FSetAlarm(0, pmvie))
			{
            CMD cmd;

			pmvie->SetFStopPlaying(fTrue);
			cmd.pcmh = pmvie;
			cmd.cid = cidAlarm;
			pmvie->FCmdAlarm(&cmd);
			}

			goto LEndTracking;
		}

    if (Pmvie()->FPlaying())
		{
		goto LEndTracking;
		}

	Pmvie()->Pmcc()->StopUISound();

	pscen = pmvie->Pscen();
	AssertNilOrPo(pscen, 0);
	if (pvNil == pscen)
		{
		goto LEndTracking;
		}

	pactr = pscen->PactrSelected();
	AssertNilOrPo(pactr, 0);
	if (pvNil != pactr && Tool() != toolPlace)
		{
		_ActorClicked(pactr, fFalse);
		}

	switch(Tool())
		{
	case toolDefault:
	case toolActorSelect:
		break;

	case toolPlace:

		if (!_fMouseDownSeen)
			{
            return;
			}

		pactr = Pmvie()->Pscen()->PactrSelected();
		AssertPo(pactr, 0);
		pactrDup = _pactrUndo;
		AssertNilOrPo(pactrDup, 0);

	    SetTool(toolCompose);
		Pmvie()->Pmcc()->ChangeTool(toolCompose);

		//
		// Now check if the actor is out of view
		//
		if (!pactr->FIsInView())
			{

			//
			// _pactrUndo is pvNil if this is a new actor, else it is
			// an actor from the roll call.
			//
            if (_pactrUndo != pvNil)
				{
				Pmvie()->Pscen()->FAddActrCore(_pactrUndo);	// Replace old actor with saved version.
				vpcex->EnqueueCid(cidActorPlacedOutOfView, pvNil, pvNil, _pactrUndo->Arid());
				ReleasePpo(&_pactrUndo);
				}
            else
				{
				pactr->AddRef();
                AssertDo(Pmvie()->FAddToRollCall(pactr, pvNil), "Should never fail");
				Pmvie()->Pscen()->RemActrCore(pactr->Arid());
				vpcex->EnqueueCid(cidActorPlacedOutOfView, pvNil, pvNil, pactr->Arid());
				ReleasePpo(&pactr);
				}

			WarpCursToCenter();
			vpappb->ShowCurs();
			break;

			}
		else if (_pactrUndo == pvNil)
			{
			//
			// _pactrUndo is pvNil if this is a new actor, else it is
			// an actor from the roll call.
			//
			AssertDo(Pmvie()->FAddToRollCall(pactr, pvNil), "Should never fail");
			}

		//
		// Now build an undo object for the placing of the actor
		//
		psuna = SUNA::PsunaNew();

		if ((psuna == pvNil) ||
			((_pactrUndo == pvNil) && !pactr->FDup(&pactrDup, fTrue)))
			{

            PushErc(ercSocNotUndoable);
			ReleasePpo(&pactrDup);
	        Pmvie()->ClearUndo();

			}
		else
			{

			pactrDup->SetArid(pactr->Arid());
			psuna->SetType(_pactrUndo == pvNil ? utAdd : utRep);
			psuna->SetActr(pactrDup);
			if ( _pactrUndo != pvNil)
				{
				pactrDup->AddRef();
				}

			if (!Pmvie()->FAddUndo(psuna))
				{
				PushErc(ercSocNotUndoable);
	        	Pmvie()->ClearUndo();
				}

            Pmvie()->Pmcc()->EnableActorTools();

			}

		ReleasePpo(&_pactrUndo);
	    ReleasePpo(&psuna);
		WarpCursToActor(pactr);
		vpappb->ShowCurs();
		vpcex->EnqueueCid(cidActorPlaced, pvNil, pvNil, pactr->Arid());
		break;

	case toolCompose:
	case toolRotateX:
	case toolRotateY:
	case toolRotateZ:
	case toolResize:
	case toolSquashStretch:
		if (pactr != pvNil)
			{
			WarpCursToActor(pactr);
			vpappb->ShowCurs();
			}
		break;

	case toolRecordSameAction:

		if (pvNil != pactr)
			{

			if ((pactr->Ptmpl() != _ptmplTool) &&
				!(_ptmplTool->FIsTdt() && pactr->Ptmpl()->FIsTdt()))
				{
				break;
				}

			pactr->FEndRecord(FRecordDefault(), _pactrRestore);	// On error, Oom already pushed
			ReleasePpo(&_pactrRestore);
			Pmvie()->InvalViewsAndScb();
			WarpCursToActor(pactr);
			vpappb->ShowCurs();

			}

		if (_fSetFRecordDefault)
			{
			_fSetFRecordDefault = fFalse;
			SetFRecordDefault(fTrue);
			}

		Pmvie()->Pmcc()->Recording(fFalse, FRecordDefault());

		break;

	case toolSoonerLater:
		if (pactr != pvNil)
			{
            Assert(pactr->FTimeFrozen(), "Something odd is going on");

			PACTR pactrDup;
			PAUND paund;

			paund = AUND::PaundNew();
    		if ((paund == pvNil) || !pactr->FDup(&pactrDup, fTrue))
    			{
    			Pmvie()->ClearUndo();
    			PushErc(ercSocNotUndoable);
    			}
    		else
    			{
    			paund->SetPactr(pactrDup);
    			ReleasePpo(&pactrDup);	  	
    			paund->SetArid(pactr->Arid());
				paund->SetSoonerLater(fTrue);
				paund->SetNfrmLast(Pmvie()->Pscen()->Nfrm());

				if (!Pmvie()->FAddUndo(paund))
					{
					Pmvie()->ClearUndo();
					PushErc(ercSocNotUndoable);
					}

    		    }

    		ReleasePpo(&paund);

        	Pmvie()->Pbwld()->MarkDirty();
        	Pmvie()->MarkViews();

			Pmvie()->Pmcc()->StartSoonerLater();
			}

		break;

	case toolNormalizeRot:
	case toolNormalizeSize:
	case toolCostumeCmid:
	case toolSceneNuke:
	case toolActorNuke:
	case toolSceneChop:
	case toolSceneChopBack:
	case toolCutObject:
	case toolCopyObject:
	case toolPasteObject:
	case toolCopyRte:
	case toolTboxPaintText:
	case toolTboxFillBkgd:
	case toolTboxMove:
	case toolTboxFont:
	case toolTboxSize:
	case toolTboxStyle:
	case toolActorEasel:
	case toolTboxStory:
	case toolTboxCredit:
	case toolSounder:
	case toolLooper:
	case toolMatcher:
		break;

	default:
		Bug("Tool unknown on mouse up");
		}
	AssertNilOrPo(_paund, 0);
	ReleasePpo(&_paund); 	// If you just did a mousedown then mouseup, we have a leftover
							// undo object that we don't want.  So nuke it.

LEndTracking:

	vpcex->EndMouseTracking();

	// Remove any cursor clipping we may have begun when mouse tracking started.
#ifdef WIN
	ClipCursor(NULL);
#endif //WIN

}


/***************************************************************************
 *
 * Handles the Cut, Copy, Paste and Clear commands, by setting the appropriate
 * tool.
 *
 * Parameters:
 *  pcmd - Pointer to the command to process.
 *
 * Returns:
 *  fTrue if it processed the command, else fFalse.
 *
 **************************************************************************/
bool MVU::FCmdClip(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PTBOX ptbox;
	PDOCB pdocb;
	bool fOV = fFalse;
	CMD cmd;

	//
	// Check for O-V model for text in text box.
	//
	if (Pmvie()->Pscen() != pvNil)
		{
		AssertPo(Pmvie()->Pscen(), 0);
		
		if (FTextMode())
			{
			ptbox = Pmvie()->Pscen()->PtboxSelected();

			if ((ptbox != pvNil) && !ptbox->FIsVisible())
				{
				ptbox = pvNil;
				}
			}
		else
			{
			ptbox = pvNil;
			}

		AssertNilOrPo(ptbox, 0);	

		if (pcmd->cid == cidPasteTool &&
				vpclip->FGetFormat(kclsACLP, &pdocb))
			{
			fOV = !((PACLP)pdocb)->FRouteOnly();
			ReleasePpo(&pdocb);
			}
		else if (((ptbox != pvNil) && ptbox->FTextSelected()) ||
					((pcmd->cid == cidPasteTool) && vpclip->FGetFormat(kclsTCLP)))
			{
			if (ptbox != pvNil)
				{
				CMD cmd = *pcmd;

				cmd.pcmh = ptbox->PddgGet(0);
				vpcex->EnqueueCmd(&cmd);
				}
			fOV = fTrue;
			}

		}

	cmd = *pcmd;

	switch (pcmd->cid)
		{
	case cidCutTool:
		if (fOV)
			{
			cmd.cid = cidCut;
			vpcex->EnqueueCmd(&cmd);
			}
		else
			{
			SetTool(toolCutObject);
			}
		break;

	case cidCopyTool:
		if (fOV)
			{
			cmd.cid = cidCopy;
			vpcex->EnqueueCmd(&cmd);
			}
        else
			{
			SetTool(toolCopyObject);
			}

		break;

	case cidPasteTool:
		if (fOV)
			{

			if (vpclip->FGetFormat(kclsTCLP) ||	vpclip->FGetFormat(kclsACLP))
				{
				FDoClip(toolPasteObject);
				}
			else
				{
				cmd.cid = cidPaste;
				vpcex->EnqueueCmd(&cmd);
				}
			}
        else
			{
			SetTool(toolPasteObject);
			}

		break;

	case cidCopyRoute:
		SetTool(toolCopyRte);
		break;

	case cidPaste:

		FDoClip(toolPasteObject);
		break;

	case cidShiftCut:
		_grfcust = fcustShift;
	case cidCut:
        FDoClip(toolCutObject);
		_grfcust = fcustNil;
		break;

	case cidShiftCopy:
		_grfcust = fcustShift;
	case cidCopy:
        FDoClip(toolCopyObject);
		_grfcust = fcustNil;
		break;

	default:
		Bug("Unknown command");
		}
	return(fTrue);
}

/***************************************************************************
 *
 * Handles the Cut, Copy, Paste and Clear commands.
 *
 * Parameters:
 *  tool - The tool to apply.
 *
 * Returns:
 *  fTrue if it processed the command, else fFalse.
 *
 **************************************************************************/
bool MVU::FDoClip(long tool)
{
	AssertThis(0);

	PDOCB pdocb = pvNil;

	switch (tool)
		{
	case toolCutObject:
	case toolCopyObject:
	case toolCopyRte:

        //
		// copy the selection
		//
		if (!_FCopySel(&pdocb, tool == toolCopyRte))
			{
			return fTrue;
			}
		vpclip->Set(pdocb);
		ReleasePpo(&pdocb);

		if (tool == toolCutObject)
			{
			_ClearSel();
			}

		Pmvie()->Pmcc()->PlayUISound(tool);

		break;

	case toolPasteObject:
		if (!vpclip->FDocIsClip(pvNil))
			{
			PTCLP ptclp;

			if (vpclip->FGetFormat(kclsTCLP, (PDOCB *)&ptclp))
				{
    			AssertPo(ptclp, 0);

    			if (Pmvie()->Pscen() == pvNil)
    				{
					ReleasePpo(&ptclp);
    				return(fFalse);
    				}

				if(ptclp->FPaste(Pmvie()->Pscen()))
					{
					ReleasePpo(&ptclp);
				    Pmvie()->Pmcc()->EnableTboxTools();
					Pmvie()->Pmcc()->PlayUISound(tool);		
					return(fTrue);
					}

				ReleasePpo(&ptclp);
				Pmvie()->Pmcc()->PlayUISound(tool);
				return(fFalse);
    			}
			else
				{
				_FPaste(vpclip);
				Pmvie()->Pmcc()->PlayUISound(tool);	
				}
			}
		break;

	default:
		Bug("Bad Tool");
		}

	return fTrue;
}


/***************************************************************************
 *
 * Handles the Undo and Redo commands.
 *
 * Parameters:
 *  pcmd - Pointer to the command to process.
 *
 * Returns:
 *  fTrue if it processed the command, else fFalse.
 *
 **************************************************************************/
bool MVU::FCmdUndo(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	bool fRet;

	if (pcmd->cid == cidUndo)
		{
		Pmvie()->Pmcc()->PlayUISound(toolUndo);
		}
	else
		{
		Pmvie()->Pmcc()->PlayUISound(toolRedo);
		}

	fRet = MVU_PAR::FCmdUndo(pcmd);
    Pmvie()->Pmcc()->SetUndo(Pmvie()->CundbUndo() != 0 ? undoUndo :
							 Pmvie()->CundbRedo() != 0 ? undoRedo :
							 undoDisabled);

	Pmvie()->InvalViewsAndScb();
	return(fRet);
}

/***************************************************************************
 *
 * Handles the Copying whatever is currently selected.
 *
 * Don't worry about tboxes, because if a tbox is selected it will
 * get the cut/copy/paste command.
 *
 * Parameters:
 *  ppdocb - Pointer to a place to store a pointer to the resulting docb.
 *	fRteOnly - fTrue if to copy an actors route only, else fFalse.
 *
 * Returns:
 *  fTrue if it was successful, else fFalse.
 *
 **************************************************************************/
bool MVU::_FCopySel(PDOCB *ppdocb, bool fRteOnly)
{
	AssertThis(0);

	PACTR pactr;
	PACLP paclp;

	if (FTextMode())
		{
		return(fFalse);
		}

	if (Pmvie()->Pscen() == pvNil)
		{
		return(fFalse);
		}

	pactr = Pmvie()->Pscen()->PactrSelected();
	AssertNilOrPo(pactr, 0);

	if ((pactr == pvNil) || !pactr->FIsInView())
		{
        PushErc(ercSocNoActrSelected);
		return(fFalse);
		}

	paclp = ACLP::PaclpNew(pactr,
		fRteOnly,
		FPure(_grfcust & fcustShift));
	AssertNilOrPo(paclp, 0);

	*ppdocb = (PDOCB)paclp;

	return(paclp != pvNil);
}


/***************************************************************************
 *
 * Handles the deleting whatever is currently selected.
 *
 * Don't worry about tboxes, because if a tbox is selected it will
 * get the cut/copy/paste command.
 *
 * Parameters:
 *  None.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVU::_ClearSel()
{
	AssertThis(0);

	PACTR pactr;
	bool fAlive;
	bool fEnableSounds;

	if (Pmvie()->Pscen() == pvNil)
		{
		return;
		}

	pactr = Pmvie()->Pscen()->PactrSelected();

	if (pactr == pvNil)
		{
        PushErc(ercSocNoActrSelected);
		return;
		}

	AssertPo(pactr, 0);

	fEnableSounds = !(FPure(Pmvie()->Pscen()->GrfScen() & fscenSounds));
	Pmvie()->Pscen()->Disable(fscenSounds);
	if (!pactr->FDelete(&fAlive, FPure(_grfcust & fcustShift))) 		
		{
		if (fEnableSounds)
			Pmvie()->Pscen()->Enable(fscenSounds);
        return;
		}
	if (fEnableSounds)
		Pmvie()->Pscen()->Enable(fscenSounds);

	if (!fAlive)
		{
		Pmvie()->Pscen()->RemActrCore(pactr->Arid());
		}
	else
		{
		//
		// According to design, if this fails, it is
		// ok to leave the actor on the stage.
		//
		pactr->FRemFromStageCore();
		}

	Pmvie()->Pscen()->MarkDirty();
	Pmvie()->InvalViews();
}


/***************************************************************************
 *
 * Handles the Pasting whatever is currently in the clipboard.
 *
 * Don't worry about tboxes, because if a tbox is selected it will
 * get the cut/copy/paste command.
 *
 * Parameters:
 *  pdocb - The pointer to the resulting docb.
 *
 * Returns:
 *  fTrue if it was successful, else fFalse.
 *
 **************************************************************************/
bool MVU::_FPaste(PCLIP pclip)
{
	AssertThis(0);
	AssertPo(pclip, 0);

	PACLP paclp;
	PTCLP ptclp;
	PACTR pactr;
	bool fRet;

	if (pclip->FGetFormat(kclsACLP, (PDOCB *)&paclp))
		{
		AssertPo(paclp, 0);

		if (Pmvie()->Pscen() == pvNil)
			{
			PushErc(ercSocNoScene);
			return(fFalse);
			}

		if (paclp->FRouteOnly() && FTextMode())
			{
			PushErc(ercSocCannotPasteThatHere);
			ReleasePpo(&paclp);
			return(fTrue);
			}

		pactr = Pmvie()->Pscen()->PactrSelected();
		AssertNilOrPo(pactr, 0);

		if (paclp->FRouteOnly() && ((pactr == pvNil) || !pactr->FIsInView()))
			{
			PushErc(ercSocNoActrSelected);
			ReleasePpo(&paclp);
			return(fTrue);
			}

		fRet = paclp->FPaste(Pmvie());
		ReleasePpo(&paclp);
		return fRet;
		}

	if (pclip->FGetFormat(kclsTCLP, (PDOCB *)&ptclp))
		{
		AssertPo(ptclp, 0);

		if (Pmvie()->Pscen() == pvNil)
			{
			PushErc(ercSocNoScene);
			return(fFalse);
			}

		fRet = ptclp->FPaste(Pmvie()->Pscen());
		ReleasePpo(&ptclp);
		return fRet;
		}

	PushErc(ercSocCannotPasteThatHere);
	return(fFalse);
}



/***************************************************************************
 *
 * Handle a close command.
 *
 * Parameters:
 *	fAssumeYes - Should the dialog assume yes.
 *
 * Returns:
 * 	fTrue if the client should close this document.
 *
 **************************************************************************/
bool MVU::FCloseDoc(bool fAssumeYes, bool fSaveDDG)
{
	AssertThis(0);
	bool fRet;
	FNI fni;

	//
	// FQueryClose calls FAutosave depending on the result of the query.
	// FAutosave needs to know whether the doc is closing as unused user
	// sounds are to be deleted from the movie only upon close.
	//
	Pmvie()->SetDocClosing(fTrue);
	// If not dirty, flush snds on close without user query
	// Irrelevant if there are no user sounds in the movie or if
	// the file is read-only (can't save to the original file)
	if (!Pmvie()->FDirty() && Pmvie()->FUnusedSndsUser() &&
			!Pmvie()->FReadOnly() && Pmvie()->FGetFni(&fni))
		{
		vpappb->BeginLongOp();
		fRet = _pdocb->FSave();   	// Flush sounds
		goto LSaved;
		}

	if (Pmvie()->Cscen() > 0)
		{
		fRet = _pdocb->FQueryClose(fAssumeYes ? fdocAssumeYes : fdocNil);
		}
	else
		{
		fRet = fTrue;
		}
	vpappb->BeginLongOp();

LSaved:
	Pmvie()->SetDocClosing(fFalse);
	if (fRet && !fSaveDDG)
		{
		// Beware: the following line destroys the this pointer!
		_pdocb->CloseAllDdg();
		}
	vpappb->EndLongOp();
	return fRet;
}


		
/***************************************************************************
 *
 * Handle a save, save as or save a copy command.
 *
 * Parameters:
 *	pcmd - The command to process
 *
 * Returns:
 *  fTrue.
 *
 **************************************************************************/
bool MVU::FCmdSave(PCMD pcmd)
{
	if (Pmvie()->Cscen() < 1)
		{
		PushErc(ercSocSaveFailure);
		}
	else
		{
		_pdocb->FSave(pcmd->cid);
		}

	return fTrue;
}

/***************************************************************************
 *
 * Note that we have had an idle loop.
 *
 * Parameters:
 *	pcmd - Pointer to the command to process.
 *
 * Returns:
 *  fFalse.
 *
***************************************************************************/
bool MVU::FCmdIdle(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	Pmvie()->SetFIdleSeen(fTrue);
	return(fFalse);
}


/***************************************************************************
 *
 * Note that the mouse is no longer on the view.
 *
 * Parameters:
 *	pcmd - Pointer to the command to process.
 *
 * Returns:
 *  fFalse.
 *
***************************************************************************/
bool MVU::FCmdRollOff(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if (_fMouseOn && (Tool() == toolListener))
		{
		Pmvie()->Pmsq()->StopAll();
		}

	_fMouseOn = fFalse;

	return(fFalse);
}


#ifdef DEBUG
/***************************************************************************
 *
 * Assert the validity of the MVU.
 *
 * Parameters:
 *  grf - Bit field of options
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVU::AssertValid(ulong grf)
{
	MVU_PAR::AssertValid(fobjAllocated);
}

/***************************************************************************
 *
 * Mark memory used by the MVU
 *
 * Parameters:
 *  None.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MVU::MarkMem(void)
{
	AssertThis(0);
	MVU_PAR::MarkMem();
	MarkMemObj(Pmvie());
}
#endif // DEBUG


//
//
//
// UNDO STUFF
//
//
//

/****************************************************
 *
 * Public constructor for movie undo objects for scene
 * related commands.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PMUNS MUNS::PmunsNew()
{
	PMUNS pmuns;
	pmuns = NewObj MUNS();
	return(pmuns);
}

/****************************************************
 *
 * Destructor for movies undo objects
 *
 ****************************************************/
MUNS::~MUNS(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pscen);
}


/****************************************************
 *
 * Does a command stored in an undo object.
 *
 * Parameters:
 *	pdocb - The owning document.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool MUNS::FDo(PDOCB pdocb)
{
	AssertThis(0);

	TAG tagOld;

	switch (_munst)
		{
	case munstInsScen:
		if (!_pmvie->FNewScenInsCore(_iscen))
			{
			goto LFail;
			}
		if (!_pmvie->Pscen()->FSetBkgdCore(&_tag, &tagOld))
			{
            _pmvie->FRemScenCore(_iscen);
			goto LFail;
			}

        _pmvie->Pmcc()->SceneUnnuked();
		break;

	case munstRemScen:
		if (!_pmvie->FRemScenCore(_iscen))
			{
			goto LFail;
			}
        _pmvie->Pmcc()->SceneNuked();
		break;

    default:
		Bug("Unknown munst");
		goto LFail;
		}

	_pmvie->Pmsq()->FlushMsq();
	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();  //  After this, _pmvie is invalid
	return (fFalse);
}

/****************************************************
 *
 * Undoes a command stored in an undo object.
 *
 * Parameters:
 *	pdocb - The owning document.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool MUNS::FUndo(PDOCB pdocb)
{
	AssertThis(0);

	switch (_munst)
		{
	case munstInsScen:
		if (!_pmvie->FRemScenCore(_iscen))
			{
			goto LFail;
			}
        _pmvie->Pmcc()->SceneNuked();
		break;

	case munstRemScen:
		if (!_pmvie->FInsScenCore(_iscen, _pscen))
			{
			goto LFail;
			}
        _pmvie->Pmcc()->SceneUnnuked();

		break;

    default:
		Bug("Unknown munst");
		goto LFail;
		}

	_pmvie->Pmsq()->FlushMsq();
	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();  //  After this, _pmvie is invalid
	return (fFalse);
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the MUNS
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void MUNS::MarkMem(void)
{
	AssertThis(0);
	MUNS_PAR::MarkMem();
	MarkMemObj(_pscen);
}

/***************************************************************************
 *
 * Assert the validity of the MUNS.
 *
 * Parameters:
 *  grf - Bit field of options
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MUNS::AssertValid(ulong grf)
{
	AssertNilOrPo(_pscen, 0);
}
#endif //DEBUG


#ifdef DEBUG
/***************************************************************************
 *
 * Assert the validity of the MUNB.
 *
 * Parameters:
 *  grf - Bit field of options
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void MUNB::AssertValid(ulong grf)
{
	MUNB_PAR::AssertValid(fobjAllocated);
	AssertPo(_pmvie, 0);
}
#endif //DEBUG
