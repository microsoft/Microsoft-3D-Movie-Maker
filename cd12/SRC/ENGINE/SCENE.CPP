/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

  scene.cpp

  Author: Sean Selitrennikoff

  Date: August, 1994

  This file contains all functionality for scene manipulation.

  THIS IS A CODE REVIEWED FILE

	Basic scene private classes:

		Scene Chop Undo Object (SUNC)

			BASE ---> UNDB ---> MUNB ---> SUNC

		Scene Background Undo Object (SUNK)

			BASE ---> UNDB ---> MUNB ---> SUNK

		Scene Pause Undo Object (SUNP)

			BASE ---> UNDB ---> MUNB ---> SUNP

		Scene Text box Undo Object (SUNX)

			BASE ---> UNDB ---> MUNB ---> SUNX

		Scene Sound Undo Object (SUNS)

			BASE ---> UNDB ---> MUNB ---> SUNS

		Scene Title Undo Object (SUNT)

			BASE ---> UNDB ---> MUNB ---> SUNT

***************************************************************************/

#include "soc.h"
ASSERTNAME

//
// Scene event types
//
enum SEVT
    {				// StartEv	FrmEv	Param
	sevtAddActr,	//    X		  		pactr/chid
	sevtPlaySnd,	// 	  		  X		SSE (Scene Sound Event)
	sevtAddTbox,	// 	  X				ptbox/chid
	sevtChngCamera,	// 			  X		icam
	sevtSetBkgd,	// 	  X				Background Tag
	sevtPause		// 			  X		type, duration
	};

//
// Struct for saving event pause information
//
struct SEVP
	{
    WIT wit;
	long dts;
	};

//
// Scene thumbnails
//
const kdxpThumbnail = 144;
const kdypThumbnail = 81;
const kbTransparent = 250;

//
// Scene event
//
struct SEV
	{
	long nfrm; // frame number of the event.
	SEVT sevt; // event type
	};

const kbomSev = 0xF0000000;
const kbomLong = 0xC0000000;

//
// Header for the scene chunk when on file
//
struct SCENH
	{
    short bo;
	short osk;
    long nfrmLast;
    long nfrmFirst;
	TRANS trans;
	};

const kbomScenh = 0x5FC00000;
/****************************************
	TAGC - Tag,Chid combo
****************************************/
const BOM kbomChid = 0xC0000000;
const BOM kbomTagc = kbomChid | (kbomTag >> 2);
typedef struct TAGC *PTAGC;
struct TAGC
	{
	CHID chid;
	TAG tag;
	};

/****************************************
	SSE - scene sound event
****************************************/
const BOM kbomSse = 0xFF000000;
typedef struct SSE *PSSE;
struct SSE
	{
	long vlm;
	long sty; // sound type
	bool fLoop;
	long ctagc;
//	TAGC _rgtagcSnd[_ctagc]; // variable array of tagcs follows SSE

protected:
	static long _Cb(long ctagc)
		{ return size(SSE) + LwMul(ctagc, size(TAGC)); }
	SSE(void) {};

public:
	static PSSE PsseNew(long ctagc);
	static PSSE PsseNew(long vlm, long sty, bool fLoop, long ctagc,
		TAGC *prgtagc);
	static PSSE PsseDupFromGg(PGG pgg, long iv, bool fDupTags = fTrue);

	PTAG Ptag(long itagc)
		{
		PTAGC prgtagc = (PTAGC)PvAddBv(this, size(SSE));
		return &(prgtagc[itagc].tag);
		}
	PTAGC Ptagc(long itagc)
		{
		PTAGC prgtagc = (PTAGC)PvAddBv(this, size(SSE));
		return &(prgtagc[itagc]);
		}
	CHID *Pchid(long itagc)
		{
		PTAGC prgtagc = (PTAGC)PvAddBv(this, size(SSE));
		return &(prgtagc[itagc].chid);
		}
	PSSE PsseAddTagChid(PTAG ptag, long chid);
	PSSE PsseDup(void);
	void PlayAllSounds(PMVIE pmvie, ulong dtsStart = 0);
	void SwapBytes(void)
		{
		long itagc;

		SwapBytesBom(this, kbomSse);
		for (itagc = 0; itagc < ctagc; itagc++)
			{
			SwapBytesBom(Ptag(itagc), kbomTag);
			SwapBytesBom(Pchid(itagc), kbomChid);
			}
		}
	long Cb(void) { return size(SSE) + LwMul(ctagc, size(TAGC)); }
	};
void ReleasePpsse(PSSE *ppsse);

//
// Undo object for chopping operation.
//
typedef class SUNC *PSUNC;

#define SUNC_PAR MUNB
#define kclsSUNC 'SUNC'
class SUNC : public SUNC_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	CNO _cno;
	PCRF _pcrf;
	SUNC(void) { }

public:

	static PSUNC PsuncNew(void);
	~SUNC(void);

	bool FSave(PSCEN pscen);

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};


//
// Undo object for background operations
//
typedef class SUNK *PSUNK;

#define SUNK_PAR MUNB
#define kclsSUNK 'SUNK'
class SUNK : public SUNK_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	TAG _tag;
	long _icam;
	bool _fSetBkgd;
	SUNK(void) { }

public:

	static PSUNK PsunkNew(void);
	~SUNK(void);

	void SetTag(PTAG ptag) { _tag = *ptag; }
	void SetIcam(long icam) { _icam = icam; }
	void SetFBkgd(bool fSetBkgd) { _fSetBkgd = fSetBkgd; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};


//
// Undo object for transition operations
//
typedef class SUNR *PSUNR;

#define SUNR_PAR MUNB
#define kclsSUNR 'SUNR'
class SUNR : public SUNR_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	TRANS _trans;
	SUNR(void) { }

public:

	static PSUNR PsunrNew(void);
	~SUNR(void);

	void SetTrans(TRANS trans) { _trans = trans; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};

//
// Undo object for pause operations
//
typedef class SUNP *PSUNP;

#define SUNP_PAR MUNB
#define kclsSUNP 'SUNP'
class SUNP : public SUNP_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	WIT _wit;
	long _dts;
	bool _fAdd;
	SUNP(void) { }

public:

	static PSUNP PsunpNew(void);
	~SUNP(void);

	void SetWit(WIT wit) { _wit = wit; }
	void SetDts(long dts) { _dts = dts; }
	void SetAdd(bool fAdd) { _fAdd = fAdd; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};


//
// Undo object for text box operations
//
typedef class SUNX *PSUNX;

#define SUNX_PAR MUNB
#define kclsSUNX 'SUNX'
class SUNX : public SUNX_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	PTBOX _ptbox;
	bool _fAdd;
	long _itbox;
	long _nfrmFirst;
	long _nfrmLast;
	SUNX(void) { }

public:

	static PSUNX PsunxNew(void);
	~SUNX(void);

	void SetNfrmFirst(long nfrm) { _nfrmFirst = nfrm; }
	void SetNfrmLast(long nfrm) { _nfrmLast = nfrm; }
	void SetItbox(long itbox) { _itbox = itbox; }
	void SetTbox(PTBOX ptbox) { _ptbox = ptbox; }
	void SetAdd(bool fAdd) { _fAdd = fAdd; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};

//
// Undo object for sound operations
//
typedef class SUNS *PSUNS;

#define SUNS_PAR MUNB
#define kclsSUNS 'SUNS'
class SUNS : public SUNS_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	PSSE _psse;	// may be pvNil
	long _sty; 		// sty to use if _psse is pvNil

	SUNS(void) { }

public:

	static PSUNS PsunsNew(void);
	~SUNS(void);

	bool FSetSnd(PSSE psse)
		{
		PSSE psseDup = psse->PsseDup();
		if (psseDup == pvNil)
			return fFalse;
		ReleasePpsse(&_psse);
		_psse = psseDup;
		_sty = _psse->sty;
		return fTrue;
		}
	void SetSty(long sty) { _sty = sty; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};

//
// Undo object for title operations
//
typedef class SUNT *PSUNT;

#define SUNT_PAR MUNB
#define kclsSUNT 'SUNT'
class SUNT : public SUNT_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	STN _stn;
	SUNT(void) { }

public:

	static PSUNT PsuntNew(void);
	~SUNT(void);

	void SetName(PSTN pstn)
		{
		AssertPo(pstn, 0);
		_stn = *pstn;
		}

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};




RTCLASS(SCEN)
RTCLASS(SUNT)
RTCLASS(SUNS)
RTCLASS(SUNA)
RTCLASS(SUNK)
RTCLASS(SUNP)
RTCLASS(SUNX)
RTCLASS(SUNC)
RTCLASS(SUNR)

/****************************************************
 *
 * Constructor for scenes.  This function is private, use PscenNew()
 * for public construction.
 *
 * Parameters:
 *  pmvie - The movie this scene belongs to.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
SCEN::SCEN(PMVIE pmvie)
{
	AssertNilOrPo(pmvie, 0);

	_pmvie = pmvie;
	_nfrmCur = 1;
	_nfrmLast = 1;
	_nfrmFirst = 1;
	_trans = transDissolve; // default transition

	//
	// By default we disable pauses in the studio
	//
	_grfscen = fscenPauses;
}

/****************************************************
 *
 * Exported constructor for scenes.
 *
 * Parameters:
 *	pmvie - The movie this scene belongs to.
 *
 *
 * Returns:
 *  pvNil, on failure, else a pointer to an allocated SCEN object.
 *		
 ****************************************************/
PSCEN SCEN::PscenNew(PMVIE pmvie)
{
	AssertNilOrPo(pmvie, 0);

	PSCEN pscen;

	//
	// Create the object
	//
	pscen = NewObj SCEN(pmvie);
	if (pscen == pvNil)
		{
		goto LFail;
	    }

    //
	// Initialize event list
	//
	pscen->_pggsevFrm = GG::PggNew(size(SEV));
	if (pscen->_pggsevFrm == pvNil)
		{
		goto LFail;
		}
	pscen->_isevFrmLim = 0;

	pscen->_pggsevStart = GG::PggNew(size(SEV));
	if (pscen->_pggsevStart == pvNil)
		{
		goto LFail;
		}

	pscen->_pglpactr = GL::PglNew(size(PACTR), 0);
	if (pscen->_pglpactr == pvNil)
		{
		goto LFail;
		}

 	pscen->_pglptbox = GL::PglNew(size(PTBOX), 0);
	if (pscen->_pglptbox == pvNil)
		{
		goto LFail;
		}

	if (vpcex != pvNil)
		{
		vpcex->EnqueueCid(cidSceneLoaded);
		}


    AssertPo(pscen, 0);
	return(pscen);

LFail:

	ReleasePpo(&pscen);
	return(pvNil);
}

/****************************************************
 *
 * Destructor for scenes.
 *
 * Parameters:
 *  None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
SCEN::~SCEN(void)
{
	AssertBaseThis(0);

	long isev;
	PSEV qsev;
	PTBOX ptbox;
	PACTR pactr;

	//
	// Remove starting events
	//

	if (_pggsevStart != pvNil)
		{
	  	for (isev=0; isev < _pggsevStart->IvMac(); isev++)
	        {
			qsev = (PSEV)_pggsevStart->QvFixedGet(isev);
	        switch (qsev->sevt)
	 			{
			case sevtAddActr:
				_pggsevStart->Get(isev, &pactr);
				AssertPo(pactr, 0);
	 			ReleasePpo(&pactr);
	 			break;

	 		case sevtAddTbox:
				_pggsevStart->Get(isev, &ptbox);
		        AssertPo(ptbox, 0);
				ReleasePpo(&ptbox);
				break;

	 	    case sevtChngCamera:
			case sevtSetBkgd:
	 			break;

	 		case sevtPause:
	 		case sevtPlaySnd:
	 			Bug("Invalid event in event stream.");
	 			break;
			default:
				Bug("Unknown event type");
				break;
	 			}

	  		}

		}

  	ReleasePpo(&_pggsevStart);

	//
	// Walk and delete all frame events.
	//
	if (_pggsevFrm != pvNil)
		{
		for (isev=0; isev < _pggsevFrm->IvMac(); isev++)
			{

	        //
	  		// For each event, release any child objects.
	   		//
			qsev=(PSEV)_pggsevFrm->QvFixedGet(isev);
	        switch (qsev->sevt)
				{

	        case sevtAddTbox:
	        case sevtAddActr:
			case sevtSetBkgd:
				Assert(0, "Invalid event in event stream.");
	        	break;

			case sevtPlaySnd:
				{
				PSSE psse;
				long itagc;
				psse = (PSSE)_pggsevFrm->QvGet(isev);
				for (itagc = 0; itagc < psse->ctagc; itagc++)
					{
					TAGM::CloseTag(psse->Ptag(itagc));
					}
				break;
				}
	        case sevtPause:
	        case sevtChngCamera:
	        	break;

			default:
				Bug("Unknown event type");
				break;
				}

			}	

		}

	//
	// Delete frame event list.
	//
	ReleasePpo(&_pggsevFrm);

    //
	// Remove the GL of actors.  We do not Release the actors
	// themselves as our reference was released above in the
	// the _pggsevStart.
	//
    ReleasePpo(&_pglpactr);

    //
	// Remove the GL of tboxes.  We do not Release the tboxes
	// themselves as our reference was released above in the
	// the _pggsevStart.
	//
    ReleasePpo(&_pglptbox);

	//
	// Release the background
	//
	ReleasePpo(&_pbkgd);

	//
	// Release the thumbnail
	//
	ReleasePpo(&_pmbmp);

	//
	// Free the background sound
	//
	ReleasePpsse(&_psseBkgd);
}

/****************************************************
 *
 * Destructor for scenes.  This method is used to not only
 * destruct a scene, but to remove all lights, actors and
 * text boxes from the rendering area.  This is necessary
 * because Undo will hold references to actors, etc, causing
 * their destructors to not be called.
 *
 * Parameters:
 *  ppscen - A pointer to the scene to destroy.
 *
 * Returns:
 *  None.
 *
 ****************************************************/

void SCEN::Close(PSCEN *ppscen)
{
	AssertPo(*ppscen, 0);

	if (pvNil != (*ppscen)->_pbkgd)
		{
		(*ppscen)->_pbkgd->TurnOffLights();
		}
	(*ppscen)->HideActors();
	(*ppscen)->HideTboxes();
	ReleasePpo(ppscen);
}


#ifdef DEBUG

/****************************************************
 * Mark memory used by the SCEN
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SCEN::MarkMem(void)
{
	AssertThis(0);

	long iactr;
	long itbox;
	PACTR pactr;
	PTBOX ptbox;

	SCEN_PAR::MarkMem();

	MarkMemObj(_pggsevStart);
	MarkMemObj(_pggsevFrm);
	MarkMemObj(_pglpactr);
	MarkMemObj(_pglptbox);
	MarkMemObj(_pmbmp);

	for (iactr = 0; iactr < _pglpactr->IvMac(); iactr++)
		{
		_pglpactr->Get(iactr, &pactr);
		MarkMemObj(pactr);
		}

	for (itbox = 0; itbox < _pglptbox->IvMac(); itbox++)
		{
		_pglptbox->Get(itbox, &ptbox);
		MarkMemObj(ptbox);
		}

	if (_psseBkgd != pvNil)
		MarkPv(_psseBkgd);
}

/***************************************************************************
	Assert the validity of the SCEN.
***************************************************************************/
void SCEN::AssertValid(ulong grf)
{
	long isev;
	SEV sev;

	SCEN_PAR::AssertValid(fobjAllocated);

	AssertPo(&_stnName, 0);
	AssertNilOrPo(_pactrSelected, 0);
	AssertNilOrPo(_pbkgd, 0);
	AssertNilOrPo(_pmbmp, 0);
	AssertPo(_pglpactr, 0);
	AssertPo(_pglptbox, 0);
	AssertPo(_pggsevFrm, 0);
	AssertPo(_pggsevStart, 0);
	AssertPo(_pmvie, 0);

	for (isev = 0; isev < _pggsevFrm->IvMac(); isev++)
		{
		sev = *(PSEV)_pggsevFrm->QvFixedGet(isev);
		switch (sev.sevt)
			{
		case sevtPlaySnd:
		case sevtPause:
	    case sevtChngCamera:
			break;
		default:
			Bug("Unknown event type");
			}
		}

 	for (isev = 0; isev < _pggsevStart->IvMac(); isev++)
		{
		sev = *(PSEV)_pggsevStart->QvFixedGet(isev);
		switch (sev.sevt)
			{
		case sevtAddActr:
		case sevtSetBkgd:
		case sevtAddTbox:
			break;
		default:
			Bug("Unknown event type");
			}
		}

}

#endif

/****************************************************
 *
 * Sets the transition of the scene and creates an undo object.
 *
 * Parameters:
 *	trans - Transition type.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *		
 ****************************************************/
bool SCEN::FSetTransition(TRANS trans)
{
	AssertThis(0);

	PSUNR psunr;

	psunr = SUNR::PsunrNew();

	if (psunr == pvNil)
		{
		return(fFalse);
		}

	if (!_pmvie->FAddUndo(psunr))
		{
		return(fFalse);
		}

    ReleasePpo(&psunr);

	SetTransitionCore(trans);

	return(fTrue);
}

/****************************************************
 *
 * Sets the name of the scene and creates an undo object.
 *
 * Parameters:
 *	psz - Null terminated string.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *		
 ****************************************************/
bool SCEN::FSetName(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	PSUNT psunt;

	psunt = SUNT::PsuntNew();

	if (psunt != pvNil)
		{
        psunt->SetName(&_stnName);
		}

	if (psunt == pvNil)
		{
		return(fFalse);
		}

	if (!_pmvie->FAddUndo(psunt))
		{
		return(fFalse);
		}

    ReleasePpo(&psunt);

	SetNameCore(pstn);

	return(fTrue);
}


/****************************************************
 *
 * This routine jumps to an arbitrary frame, updating actors, and adding
 * new frames to the scene if needed.
 *
 * Parameters:
 *	nFrm - The frame number to jump to.  This may be positive or negative
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *		
 ****************************************************/

bool SCEN::FGotoFrm(long nfrm)
{
	AssertThis(0);

	bool fSoundInFrame = fFalse, fUpdateSndFrame = nfrm != _nfrmCur;
	SEV sev;
	PMVU pmvu;
	void *qvVar;
	long isev;
	long nfrmOld = _nfrmCur;

    if (nfrm < _nfrmCur)
		{

		//
		// Assume no pause type
		//
		Pmvie()->Pmcc()->PauseType(witNil);

		//
		// Go backwards
		//
		if (nfrm < _nfrmFirst)
			{
			//
			// Move first frame back in time
			//
			_MoveBackFirstFrame(nfrm);
			_MarkMovieDirty();
			}

        _nfrmCur = nfrm;

        //
		// Unplay all events to dest frame.
		//
		for (;_isevFrmLim > 0; _isevFrmLim--)
			{
			_pggsevFrm->GetFixed(_isevFrmLim - 1, &sev);
			if (sev.nfrm <= _nfrmCur)
				{
                break;
				}

			qvVar = _pggsevFrm->QvGet(_isevFrmLim - 1);
            if (!_FUnPlaySev(&sev, qvVar))
				{
                PushErc(ercSocGotoFrameFailure);
				return(fFalse);
				}

			}

		//
		// Play events in this frame
		//
        for (isev = _isevFrmLim - 1; isev >= 0;  isev--)
			{
			_pggsevFrm->GetFixed(isev, &sev);
			if (sev.nfrm < _nfrmCur)
				{
                break;
				}
			qvVar = _pggsevFrm->QvGet(isev);
            if (!_FPlaySev(&sev, qvVar, _grfscen))
				{
                PushErc(ercSocGotoFrameFailure);
				return(fFalse);
				}
			if (sev.sevt == sevtPlaySnd && sev.nfrm == _nfrmCur)
				fSoundInFrame = fTrue;
			}

		}
	else if (nfrm > _nfrmCur)
		{

		pmvu = (PMVU)Pmvie()->PddgGet(0);
		AssertNilOrPo(pmvu, 0);

		if ((pmvu != pvNil) && (pmvu->Tool() == toolRecordSameAction))
			{
			Pmvie()->Pmcc()->PlayUISound(toolRecordSameAction);
			}

        _nfrmCur = nfrm;

		//
		// Assume no pause type
		//
		Pmvie()->Pmcc()->PauseType(witNil);

		//
		// Go forwards
		//
		if (_nfrmCur > _nfrmLast)
			{
            _nfrmLast = _nfrmCur;
			_MarkMovieDirty();
			}

        //
		// Play all events to dest frame.
		//
        for (; _isevFrmLim < _pggsevFrm->IvMac();  _isevFrmLim++)
			{
			_pggsevFrm->GetFixed(_isevFrmLim, &sev);
			if (sev.nfrm > _nfrmCur)
				{
                break;
				}
			qvVar = _pggsevFrm->QvGet(_isevFrmLim);
            if (!_FPlaySev(&sev, qvVar, (sev.nfrm==_nfrmCur ?
											_grfscen :
											(_grfscen | fscenSounds | fscenPauses))))
				{
                PushErc(ercSocGotoFrameFailure);
				return(fFalse);
				}
			if (sev.sevt == sevtPlaySnd && sev.nfrm == _nfrmCur)
				fSoundInFrame = fTrue;
			}

		}

    if (!(_grfscen & fscenActrs) && !_FForceActorsToFrm(nfrm, &fSoundInFrame))
		{
		return(fFalse);
		}

	if (!(_grfscen & fscenTboxes) && !_FForceTboxesToFrm(nfrm))
		{
		return(fFalse);
		}

	if (!(_grfscen & fscenActrs))
		{
		_DoPrerenderingWork(fFalse);
		}

	Pmvie()->InvalViews();

	if (fUpdateSndFrame)
		Pmvie()->Pmcc()->SetSndFrame(fSoundInFrame);

    return(fTrue);


}



/****************************************************
 *
 * This function is an optimization which could be completely
 * skipped if desired.  The idea is to not render actors frame
 * after frame if they're not moving or changing at all.  So
 * when we hit a camera change, we look ahead to see if any
 * actors are unchanging before the next camera change.  If
 * there are, we hide the changing actors (so only the unchanging
 * ones are visible), then "take a snapshot" of the world with
 * the unchanging actors (via BWLD::Prerender()), and use the
 * snapshot as the background RGB and Z buffer until the next
 * camera view change.  We only prerender if the movie is
 * playing.
 *
 * Parameters:
 *	fStartNow - if fTrue, start prerendering even if there's no
 *              camera change in this frame.  If fFalse, only
 *              start prerendering if there is a camera view
 *              change in this frame, or if this is the first
 *              frame of the scene.
 *
 * Returns:
 *  none
 *		
 ****************************************************/
void SCEN::_DoPrerenderingWork(bool fStartNow)
{
	AssertThis(0);

	long isev;
	SEV sev;
	long nfrmNextChange;
	long ipactr;
	PACTR pactr;
	long cactrPrerendered;

	//
	// If the movie was playing and there was a camera view change
	// in this frame, prerender any actors that don't change from
	// here to the next camera view change or the end of the scene.
	//
	if (!Pmvie()->FPlaying())
		{
		return; // only prerender if the movie is playing
		}

	// Do prerender if this is the first frame of the scene
	// (even though there's no sevtChngCamera), or if fStartNow
	// is fTrue, or if there is a sevtChngCamera in this
	// frame.  Otherwise, just return.
	if (_nfrmCur != _nfrmFirst && !fStartNow)
		{
	    for (isev = _isevFrmLim - 1; isev >= 0; isev--)
			{
			_pggsevFrm->GetFixed(isev, &sev);
			if (sev.nfrm != _nfrmCur)
				{
				return; // no camera view change in this frame
				}
			if (sev.sevt == sevtChngCamera)
				{
				break; // found one!
				}
			}
		if (isev < 0)
			{
			return; // no camera view in this frame
			}
		}

	// Find when the next view change is
	nfrmNextChange = _nfrmLast; // if no more view changes, go til end of scene
	for (isev = _isevFrmLim; isev < _pggsevFrm->IvMac(); isev++)
		{
		_pggsevFrm->GetFixed(isev, &sev);
		if (sev.sevt == sevtChngCamera)
			{
			nfrmNextChange = sev.nfrm;
			break;
			}
		}

	// Hide all actors that can't be prerendered this time, and count how many
	// can be prerendered this time
	cactrPrerendered = 0;
	for (ipactr = 0; ipactr < _pglpactr->IvMac(); ipactr++)
		{
		_pglpactr->Get(ipactr, &pactr);
		if (pactr->FMustRender(nfrmNextChange))
			{
			pactr->Hide();
			}
		else
			{
			cactrPrerendered++;
			if (pactr->FPrerendered())
				{
				// Actor was prerendered in last view and in this
				// view.  Temporarily show the actor so it shows
				// up in the prerendered background
				pactr->Show();
				pactr->SetPrerendered(fFalse);
				}
			}
		}
	if (cactrPrerendered > 0)
		{
		Pmvie()->Pbwld()->Prerender();
		}
	// Show all the actors that were hidden (and show them again if they were
	// prerendered last time and can't be now), and hide the newly prerendered
	// actors.
	for (ipactr = 0; ipactr < _pglpactr->IvMac(); ipactr++)
		{
		_pglpactr->Get(ipactr, &pactr);
		if (pactr->FMustRender(nfrmNextChange))
			{
			pactr->Show();
			if (pactr->FPrerendered())
				{
				pactr->Show();
				pactr->SetPrerendered(fFalse);
				}
			}
		else
			{
			Assert(!pactr->FPrerendered(), "no actor should be marked prerendered here");
			pactr->Hide();
			pactr->SetPrerendered(fTrue);
			}
		}
}


/****************************************************
 *
 * 	Ends any current prerendering by restoring the background
 *  RGB and Z buffers of the BWLD, showing all previously
 *  hidden actors, and marking all actors as not prerendered.
 *
 * Parameters:
 *	none
 *
 * Returns:
 *  none
 *		
 ****************************************************/
void SCEN::_EndPrerendering(void)
{
	AssertThis(0);

	long ipactr;
	PACTR pactr;

	// Show all the actors that were being prerendered
	Pmvie()->Pbwld()->Unprerender();
	for (ipactr = 0; ipactr < _pglpactr->IvMac(); ipactr++)
		{
		_pglpactr->Get(ipactr, &pactr);
		if (pactr->FPrerendered())
			{
			pactr->Show();
			pactr->SetPrerendered(fFalse);
			}
		}
}


/****************************************************
 *
 * This routine replays all the events, filtered by
 * grfscen, in the current frame.
 *
 * Parameters:
 *	grfscen - Events to play.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *		
 ****************************************************/
bool SCEN::FReplayFrm(ulong grfscen)
{
	AssertThis(0);

	SEV sev;
	void *qvVar;
	long isev, iactr;
	long nfrmOld = _nfrmCur;
	PACTR pactr;

	//
	// Play events in this frame
	//
    for (isev = _isevFrmLim - 1; isev >= 0;  isev--)
		{
		_pggsevFrm->GetFixed(isev, &sev);
		if (sev.nfrm < _nfrmCur)
			{
            break;
			}
		qvVar = _pggsevFrm->QvGet(isev);
		// Note: FReplayFrm always suppresses camera changes
        if (!_FPlaySev(&sev, qvVar, (~grfscen) | fscenCams))
			{
            PushErc(ercSocGotoFrameFailure);
			return(fFalse);
			}
		}

    if (grfscen & fscenActrs)
		{

    	for (iactr = 0; iactr < _pglpactr->IvMac(); iactr++)
    		{
    		_pglpactr->Get(iactr, &pactr);
    		AssertPo(pactr, 0);

    		if (!pactr->FReplayFrame(grfscen))
    			{
    			return(fFalse);
    			}
    		}

		}

    return(fTrue);

}



/****************************************************
 *
 * This routine recalculates the length of the movie.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *	None.
 *
 ****************************************************/
void SCEN::InvalFrmRange(void)
{
	AssertThis(0);

	PACTR pactr;
	PTBOX ptbox;
	long ipo;
	long nfrmStart, nfrmLast;

	for (ipo=0; ipo < _pglpactr->IvMac(); ipo++)
		{
		_pglpactr->Get(ipo, &pactr);

		if (pactr->FGetLifetime(&nfrmStart, &nfrmLast))
			{

    		if (nfrmStart < _nfrmFirst)
    			{
				_MoveBackFirstFrame(nfrmStart);
    			}

    		if (nfrmLast > _nfrmLast)
    			{
                _nfrmLast = nfrmLast;
    			}

			}

		}

	for (ipo=0; ipo < _pglptbox->IvMac(); ipo++)
		{
		_pglptbox->Get(ipo, &ptbox);

		if (ptbox->FGetLifetime(&nfrmStart, &nfrmLast))
			{
				
    		if (nfrmStart < _nfrmFirst)
    			{
				_MoveBackFirstFrame(nfrmStart);
    			}

    		if (nfrmLast > _nfrmLast)
    			{
                _nfrmLast = nfrmLast;
    			}

			}

		}

}

/****************************************************
 *
 * This routine plays a single event.
 *
 * Be careful not to modify _pggsevFrm or _pggsevStart from
 * within this routine.
 *
 * Parameters:
 *	psev - Pointer to the scene event to play.
 *	qvVar- Pointer to the variable part of the event.
 *	grfscen - Flags of currently disabled event types.
 *
 * Returns:
 *	fTrue if the event was played, fFalse in the case of failure.
 *
 ****************************************************/
bool SCEN::_FPlaySev(PSEV psev, void *qvVar, ulong grfscen)
{
	AssertThis(0);
	AssertVarMem(psev);

	PACTR pactr;
	PTBOX ptbox;
	PBKGD pbkgd;
	TAG tag;
	WIT wit;
	long dts;

	switch (psev->sevt)
		{
	case sevtPlaySnd:
		PSSE psse;

		psse = (PSSE)qvVar;
		// If it's midi, copy it to _psseBkgd
		if (psse->sty == styMidi)
			{
			PSSE psseDup;	
			psseDup = psse->PsseDup();
			if (psseDup == pvNil)
				return fFalse;
			ReleasePpsse(&_psseBkgd);
			_psseBkgd = psseDup;
			_nfrmSseBkgd = psev->nfrm;
			}
		if (grfscen & fscenSounds)
			{
	        return (fTrue);
			}
		psse->PlayAllSounds(Pmvie());
		break;

	case sevtSetBkgd:

		tag = *(PTAG)qvVar;
		pbkgd = (PBKGD)vptagm->PbacoFetch(&tag, BKGD::FReadBkgd);
		if (pvNil == pbkgd)
			{
			return fFalse;
			}

		if (!pbkgd->FSetCamera(_pmvie->Pbwld(), 0))
			{
			ReleasePpo(&pbkgd);
			return(fFalse);
			}

		if (Pmvie()->Trans() == transNil)
			{
			Pmvie()->SetTrans(transCut); // so we do palette change at next draw
			}

		_pbkgd = pbkgd;
		_tagBkgd = tag;
		break;

	case sevtAddActr:

        //
		// Add the actor to the roll call.
		//
        pactr = *(PACTR *)qvVar;

		AssertPo(pactr, 0);

        if (_pglpactr->FPush(&pactr) == fFalse)
			{
			return(fFalse);
			}

		pactr->SetPscen(this);

		//
		// Plop them into this frame
		//
		if (!pactr->FGotoFrame(_nfrmCur))
			{
			_pglpactr->FPop(&pactr);
			return(fFalse);
			}

		break;

	case sevtPause:

	    wit = (WIT)(*(long *)qvVar);
		Pmvie()->Pmcc()->PauseType(wit);

		if (grfscen & fscenPauses)
			{
            return(fTrue);
			}

		dts = *((long *)qvVar + 1);
		Pmvie()->DoPause(wit, dts);
		break;

	case sevtAddTbox:

        //
		// Add the text box to the scene
		//
        ptbox = *(PTBOX *)qvVar;

		AssertPo(ptbox, 0);

		//
		// Insert at the end, because tbox ordering is important
		// since the client uses itbox to find text boxes.
		//
        if (!_pglptbox->FInsert(_pglptbox->IvMac(), &ptbox))
			{
			return(fFalse);
			}

		//
		// Plop them into this frame
		//
		if (!ptbox->FGotoFrame(_nfrmCur))
			{
			_pglptbox->Delete(_pglptbox->IvMac() - 1);
			return(fFalse);
			}

		break;

    case sevtChngCamera:

		Assert(_pbkgd != pvNil, "No background in the scene");
		if (grfscen & fscenCams)
			{
            return(fTrue);
			}
		if (!_pbkgd->FSetCamera(_pmvie->Pbwld(), *(long *)qvVar))
			{
			return(fFalse);
			}
		break;

	default:
		
		Bug("Unhandled sevt");
		break;

		}

	return(fTrue);
}

/****************************************************
 *
 * This routine undoes a single event.
 *
 * Parameters:
 *	psev - Pointer to the scene event to unplay.
 *	qvVar- Pointer to the variable part of the event.
 *
 *
 * Returns:
 *	fTrue if the unplay worked, else fFalse.
 *
 ****************************************************/
bool SCEN::_FUnPlaySev(PSEV psev, void *qvVar)
{
	AssertThis(0);
	AssertVarMem(psev);

	SEV sev;
	long isev;
    PSEV qsevTmp;

	switch (psev->sevt)
		{
	case sevtPlaySnd:
		//
		// Go backwards and find previous background sound.
		//
		for (isev = _isevFrmLim - 1; isev >= 0; isev--)
			{
			qsevTmp = (PSEV)_pggsevFrm->QvFixedGet(isev);

			if ((qsevTmp->sevt == sevtPlaySnd) && (qsevTmp->nfrm < _nfrmCur))
				{
                sev = *qsevTmp;
				_FPlaySev(&sev, _pggsevFrm->QvGet(isev), _grfscen | fscenSounds);  // Ignore failure.
				return(fTrue);
				}
            }

		//
		// Not found -- clear variables.
		//
		ReleasePpsse(&_psseBkgd);
		break;

	case sevtAddActr:
	case sevtAddTbox:
	case sevtPause:
		break;

    case sevtChngCamera:

		Assert(_pbkgd != pvNil, "No background in scene");

		//
		// Go backwards and find previous camera position.
		//
		for (isev = _isevFrmLim - 1; isev >= 0; isev--)
			{
			qsevTmp = (PSEV)_pggsevFrm->QvFixedGet(isev);

			if ((qsevTmp->sevt == sevtChngCamera) && (qsevTmp->nfrm < _nfrmCur))
				{
                sev = *qsevTmp;
				AssertDo(_FPlaySev(&sev, _pggsevFrm->QvGet(isev), _grfscen), "Should not fail.");
				return(fTrue);
				}
            }

		//
		// Not found -- use starting camera, which is always camera 0.
		//
		sev.sevt = sevtChngCamera;
		isev = 0;

 		if (!_FPlaySev(&sev, &isev, _grfscen))
			{
			Assert(0, "Should never happen");
			return(fFalse);
			}

		break;
		}

	return(fTrue);
}


/****************************************************
 *
 * This routine moves back stuff that is currently
 * in the first frame of the movie to the given frame.
 *
 * Parameters:
 *	nfrm - New first frame.
 *
 * Returns:
 *	None.
 *
 ****************************************************/
void SCEN::_MoveBackFirstFrame(long nfrm)
{
	AssertThis(0);
	Assert(nfrm < _nfrmFirst, "Can only be called to extend scene back.");

	long isev;
	SEV sev;

	//
	// Move back all events that must persist in the
	// first frame.
	//
	for (isev = 0; isev < _pggsevFrm->IvMac(); isev++)
		{
		_pggsevFrm->GetFixed(isev, &sev);

		if (sev.nfrm != _nfrmFirst)
			{
			break;
			}

		if (sev.sevt == sevtChngCamera)
			{

			//
			// Move this back
			//
			sev.nfrm = nfrm;
			_pggsevFrm->PutFixed(isev, &sev);
			_pggsevFrm->Move(isev, 0);
			}

		}

	//
	// Set new first frame
	//
	_nfrmFirst = nfrm;
}


/****************************************************
 *
 * This routine adds a sound to the event list for this frame.
 *
 * Parameters:
 *  fLoop - should the sound loop?
 *  fQueue - queue after existing sounds, or replace them?
 *  vlm - volume to play this sound at
 *  sty - sound type (midi, speech, or SFX)
 *  ctag - number of sounds
 *  prgtag - array of MSND tags
 *
 *
 * Returns:
 *	fTrue, if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FAddSndCore(bool fLoop, bool fQueue, long vlm, long sty, long ctag, PTAG prgtag)
{
	AssertThis(0);
	AssertPvCb(prgtag, LwMul(ctag, size(TAG)));
	Assert(!fQueue || (ctag == 1), "if fQueue'ing, you should only be adding one sound");
	Assert(!fQueue || !fLoop, "can't both queue and loop");
	AssertIn(sty, 0, styLim);

	SEV sev;
	PSSE psseOld;
	PSSE psseNew;
	long isev;
	CHID chid;
	long isevSnd = ivNil;
	PTAG ptag;
	PMSND pmsnd;
	long itag, itagBase;

	//
	// Find any other sevtPlaySnd events in this frame with the same sty
	//
	for (isev = _isevFrmLim - 1; isev >= 0; isev--)
		{
		_pggsevFrm->GetFixed(isev, &sev);
		if (sev.nfrm != _nfrmCur)
			break;
		if (sev.sevt == sevtPlaySnd)
			{
			psseOld = (PSSE)_pggsevFrm->QvGet(isev);
			if (psseOld->sty == sty)
				{
				// Found a match, which we will either add to or replace
				isevSnd = isev;
				break;
				}
			}
		}

	if (isevSnd == ivNil)
		{
		fQueue = fFalse; // nothing to queue to
		}

	sev.nfrm = _nfrmCur;
	sev.sevt = sevtPlaySnd;

	if (!fQueue)
		{
		PTAGC prgtagc;
		long itagc;

		if (!FAllocPv((void **)&prgtagc, LwMul(size(TAGC), ctag), fmemClear, mprNormal))
			return fFalse;
		for (itagc = 0; itagc < ctag; itagc++)
			{
			prgtagc[itagc].tag = prgtag[itagc];
		   	if (prgtagc[itagc].tag.sid == ksidUseCrf)
				{
				if (!_pmvie->FChidFromUserSndCno(prgtag[itagc].cno, &chid))
					{
					FreePpv((void **)&prgtagc);
					return fFalse;
					}
				prgtagc[itagc].chid = chid;
				}
			else
				{
				TrashVar(&prgtagc[itagc].chid);
				}
			}
		// Create new event, replace any old event of same sty
		itagBase = 0;
		psseNew = SSE::PsseNew(vlm, sty, fLoop, ctag, prgtagc);
		FreePpv((void **)&prgtagc);
		if (pvNil == psseNew)
			{
			return fFalse;
			}
		if (!_FAddSev(&sev, psseNew->Cb(), psseNew))
			{
			ReleasePpsse(&psseNew);
			return fFalse;
			}
		if (isevSnd != ivNil)
			{
			// Delete old event, if any
			PSSE psse;
			long itagc;
			psse = (PSSE)_pggsevFrm->QvGet(isevSnd);
			for (itagc = 0; itagc < psse->ctagc; itagc++)
				{
				TAGM::CloseTag(psse->Ptag(itagc));
				}
			_pggsevFrm->Delete(isevSnd);
			_isevFrmLim--;
			}
		}
	else // we're queueing
		{
		// Add this sound to isevSnd
		psseOld = SSE::PsseDupFromGg(_pggsevFrm, isevSnd);
		if (pvNil == psseOld)
			{
			return fFalse;
			}
		// ctag == 1
		if (prgtag[0].sid == ksidUseCrf)
			{
		    if (!_pmvie->FChidFromUserSndCno(prgtag[0].cno, &chid))
				{
				ReleasePpsse(&psseOld);
				return fFalse;
				}
			}
		else
			TrashVar(&chid);

		itagBase = psseOld->ctagc;
		psseNew = psseOld->PsseAddTagChid(prgtag, chid);
		if (pvNil == psseNew)
			{
			ReleasePpsse(&psseOld);
			return fFalse;
			}
		if (!_pggsevFrm->FPut(isevSnd, psseNew->Cb(), psseNew))
			{
			ReleasePpsse(&psseOld);
			ReleasePpsse(&psseNew);
			return fFalse;
			}
		ReleasePpsse(&psseOld);
		}

	//
	// Play only these sounds
	//
	for (itag = 0; itag < ctag; itag++)
		{
		ptag = &(prgtag[itag]);
		if (ptag->sid == ksidUseCrf)
			{
			if (!Pmvie()->FResolveSndTag(ptag, *(psseNew->Pchid(itag + itagBase))))
				continue;
			}

		pmsnd = (PMSND)vptagm->PbacoFetch(ptag, MSND::FReadMsnd);
		if (pvNil == pmsnd)
			continue;

		// Only queue if it's not the first sound.
		Pmvie()->Pmsq()->FEnqueue(pmsnd, 0, fLoop, (itag != 0), vlm, pmsnd->Spr(fLoop ? toolLooper : toolSounder),
			fFalse, 0);

		ReleasePpo(&pmsnd);
		}

	if (!_FPlaySev(&sev, psseNew, _grfscen | fscenSounds))
		{
		// non-fatal error...ignore it
		}
	FreePpv((void **)&psseNew); // don't ReleasePpsse because GG got the tags
	
	_MarkMovieDirty();
	Pmvie()->Pmcc()->SetSndFrame(fTrue);
	return fTrue;
}

/****************************************************
 *
 * This routine adds a sound to the event list for this frame.
 * Note: The chid's in the tagc are current
 *
 * Parameters:
 *  fLoop - should the sound loop?
 *  fQueue - queue after existing sounds, or replace them?
 *  vlm - volume to play this sound at
 *  sty - sound type (midi, speech, or SFX)
 *  ctag - number of sounds
 *  prgtagc - array of MSND tags
 *
 *
 * Returns:
 *	fTrue, if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FAddSndCoreTagc(bool fLoop, bool fQueue, long vlm, long sty, long ctagc, PTAGC prgtagc)
{
	AssertThis(0);
	AssertPvCb(prgtagc, LwMul(ctagc, size(TAGC)));
	Assert(!fQueue || (ctagc == 1), "if fQueue'ing, you should only be adding one sound");
	Assert(!fQueue || !fLoop, "can't both queue and loop");
	AssertIn(sty, 0, styLim);

	SEV sev;
	PSSE psseOld;
	PSSE psseNew;
	long isev;
	long isevSnd = ivNil;

	//
	// Find any other sevtPlaySnd events in this frame with the same sty
	//
	for (isev = _isevFrmLim - 1; isev >= 0; isev--)
		{
		_pggsevFrm->GetFixed(isev, &sev);
		if (sev.nfrm != _nfrmCur)
			break;
		if (sev.sevt == sevtPlaySnd)
			{
			psseOld = (PSSE)_pggsevFrm->QvGet(isev);
			if (psseOld->sty == sty)
				{
				// Found a match, which we will either add to or replace
				isevSnd = isev;
				break;
				}
			}
		}

	if (isevSnd == ivNil)
		{
		fQueue = fFalse; // nothing to queue to
		}

	sev.nfrm = _nfrmCur;
	sev.sevt = sevtPlaySnd;

	if (!fQueue)
		{
		// Create new event, replace any old event of same sty
		psseNew = SSE::PsseNew(vlm, sty, fLoop, ctagc, prgtagc);
		if (pvNil == psseNew)
			return fFalse;
		if (!_FAddSev(&sev, psseNew->Cb(), psseNew))
			{
			ReleasePpsse(&psseNew);
			return fFalse;
			}
		if (isevSnd != ivNil)
			{
			// Delete old event, if any
			PSSE psse;
			long itagc;
			psse = (PSSE)_pggsevFrm->QvGet(isevSnd);
			for (itagc = 0; itagc < psse->ctagc; itagc++)
				{
				TAGM::CloseTag(psse->Ptag(itagc));
				}
			_pggsevFrm->Delete(isevSnd);
			_isevFrmLim--;
			}
		}
	else // we're queueing
		{
		Bug("Should never queue when undoing");
		}

	if (!_FPlaySev(&sev, psseNew, _grfscen))
		{
		// non-fatal error...ignore it
		}

	FreePpv((void **)&psseNew); // don't ReleasePpsse because GG got the tags
	_MarkMovieDirty();
	Pmvie()->Pmcc()->SetSndFrame(fTrue);
	return fTrue;
}


/****************************************************
 *
 * This routine adds a sound to the event list for this frame
 * and creates an undo object for the action.
 *
 * Parameters:
 *	ptag - pointer to the tag for the sound.
 *  fLoop - whether to loop the sound
 *  fQueue - queue after existing sounds, or replace them?
 *  vlm - volume to play this sound at
 *  sty - sound type (midi, speech, or SFX)
 *
 * Returns:
 *	fTrue, if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FAddSnd(PTAG ptag, bool fLoop, bool fQueue, long vlm, long sty)
{
	AssertThis(0);
	AssertVarMem(ptag);
	Assert(!fQueue || !fLoop, "can't both queue and loop");
	AssertIn(sty, 0, styLim);

	PSUNS psuns;
	PSSE psse;
	bool fFound;

	// Create a SUNS with nil _psse
	psuns = SUNS::PsunsNew();

	if (psuns == pvNil)
		{
		return(fFalse);
		}
	psuns->SetSty(sty);

	// grab sound before edit, if any
	if (!FGetSnd(sty, &fFound, &psse))
		{
		ReleasePpo(&psuns);
		return fFalse;
		}
	if (fFound)
		{
		if (!psuns->FSetSnd(psse))
			{
			ReleasePpsse(&psse);
			ReleasePpo(&psuns);
			return fFalse;
			}
		ReleasePpsse(&psse);
		}

	if (!_pmvie->FAddUndo(psuns))
		{
		ReleasePpo(&psuns);
		return(fFalse);
		}

    ReleasePpo(&psuns);

	if (!FAddSndCore(fLoop, fQueue, vlm, sty, 1, ptag))
		{
        _pmvie->ClearUndo();
		return(fFalse);
		}

	return(fTrue);
}

/****************************************************
 *
 * This routine removes a sound from the event list for this frame.
 *
 * Parameters:
 *	sty - the type of sound to remove
 *
 *
 * Returns:
 *  None
 *
 ****************************************************/
void SCEN::RemSndCore(long sty)
{
	AssertThis(0);
	AssertIn(sty, 0, styLim);

	PSEV qsev;
	long isev;

	//
	// Find the sound
	//
	for (isev= _isevFrmLim - 1; isev >= 0; isev--)
		{
    	qsev = (PSEV)_pggsevFrm->QvFixedGet(isev);

        if (qsev->nfrm != _nfrmCur)
			{
			Bug("sty not found");
			return;
			}

		if ((qsev->sevt == sevtPlaySnd) &&
			((PSSE)_pggsevFrm->QvGet(isev))->sty == sty)
			{
			//
			// Remove it
			//
			PSSE psse;
			long itagc;
			psse = (PSSE)_pggsevFrm->QvGet(isev);
			for (itagc = 0; itagc < psse->ctagc; itagc++)
				{
				TAGM::CloseTag(psse->Ptag(itagc));
				}
			_pggsevFrm->Delete(isev);
			_isevFrmLim--;

			if (sty == styMidi)
				{
				ReleasePpsse(&_psseBkgd);
				}

			UpdateSndFrame();

			_MarkMovieDirty();
			return;
			}

 	    }

	Bug("No such sound");

}

/****************************************************
 *
 * This routine removes a sound from the event list for this frame
 * and creates an undo object for the action.
 *
 * Parameters:
 *	sty - the type of sound to remove
 *
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FRemSnd(long sty)
{
	AssertThis(0);
	AssertIn(sty, 0, styLim);

	PSUNS psuns;
	PSSE psse = pvNil;
	long isev;
	PSEV qsev;

	psuns = SUNS::PsunsNew();

	if (psuns == pvNil)
		{
		return(fFalse);
		}

	//
	// Find the sound
	//
	for (isev= _isevFrmLim - 1; isev >= 0; isev--)
		{
    	qsev = (PSEV)_pggsevFrm->QvFixedGet(isev);

        if (qsev->nfrm != _nfrmCur)
			{
			Bug("sty not found");
			return fTrue;
			}

		if ((qsev->sevt == sevtPlaySnd) &&
			((PSSE)_pggsevFrm->QvGet(isev))->sty == sty)
			{
			psse = SSE::PsseDupFromGg(_pggsevFrm, isev);
			if (psse == pvNil)	
				{
				return fFalse;
				}
			}

 	    }
	if (!psuns->FSetSnd(psse))
		{
		ReleasePpsse(&psse);
		return fFalse;
		}
	ReleasePpsse(&psse);

	if (!_pmvie->FAddUndo(psuns))
		{
		ReleasePpo(&psuns);
		return(fFalse);
		}

    ReleasePpo(&psuns);

	RemSndCore(sty);

	return(fTrue);
}


/****************************************************
 *
 * This routine finds a specific sound in the current frame.
 *
 * Parameters:
 *	sty - sound type to search for
 *  pfFound - set to fTrue if a sound is found, else fFalse
 *  ppsse - gets a pointer to the SSE if it is found
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FGetSnd(long sty, bool *pfFound, PSSE *ppsse)
{
	AssertThis(0);
	AssertIn(sty, 0, styLim);
	AssertVarMem(ppsse);

	PSEV qsev;
	long isev;

	*pfFound = fFalse;
    //
	// Check event list.
	//
	for (isev= _isevFrmLim - 1; isev >= 0; isev--)
		{
		qsev = (PSEV)_pggsevFrm->QvFixedGet(isev);

		if (qsev->nfrm != _nfrmCur)
			{
            break; // sound not found
			}

		if (qsev->sevt == sevtPlaySnd)
			{
			if (sty == ((PSSE)_pggsevFrm->QvGet(isev))->sty)
				{
				*ppsse = SSE::PsseDupFromGg(_pggsevFrm, isev);
				if (*ppsse == pvNil)
					{
					return fFalse; // memory error
					}
				*pfFound = fTrue;
				return fTrue;
				}
			}

		}

    //
	// End of list...sound not found
	//
	return(fTrue);
}


/****************************************************
 *
 * This routine plays the background sound, if any.
 * If the sound really started at an earlier frame,
 * this function tries to resume the sound at the
 * appropriate offset into the sound (dtsStart).
 *
 * Parameters:
 *  none
 *
 * Returns:
 *  none
 *
 ****************************************************/
void SCEN::PlayBkgdSnd(void)
{
	AssertThis(0);

	ulong dtsStart;
	ulong dfrm;

	if (pvNil != _psseBkgd)
		{
		dfrm = Nfrm() - _nfrmSseBkgd;
		Assert(dfrm >= 0, "this sound is in the future!");
		dtsStart = LwMulDiv(dfrm, kdtsSecond, kfps);
		_psseBkgd->PlayAllSounds(Pmvie(), dtsStart);
		}
}


/****************************************************
 *
 * This routine queries what sounds are attched to the
 * scene at the current frame
 *
 * Parameters:
 *  sty - the sound type to query
 *
 * Returns:
 *  fFalse if an error occurs
 *
 ****************************************************/
bool SCEN::FQuerySnd(long sty, PGL *ppgltagSnd, long *pvlm, bool *pfLoop)
{
	AssertThis(0);
	AssertVarMem(ppgltagSnd);
	AssertVarMem(pvlm);
	AssertVarMem(pfLoop);

	PSSE psse;
	bool fFound;
	long itag;

	*ppgltagSnd = pvNil;

	if (!FGetSnd(sty, &fFound, &psse))
		{
		return fFalse; // error
		}
	if (!fFound)
		{
		return fTrue; // no sounds (*ppglTagSnd is nil)
		}
	*ppgltagSnd = GL::PglNew(size(TAG), psse->ctagc);
	if (pvNil == *ppgltagSnd)
		{
		ReleasePpsse(&psse);
		return fFalse;		
		}
	AssertDo((*ppgltagSnd)->FSetIvMac(psse->ctagc),
		"PglNew should have ensured space");
	for (itag = 0; itag < psse->ctagc; itag++)
		{
		(*ppgltagSnd)->Put(itag, psse->Ptag(itag));
		}
	*pvlm = psse->vlm;
	*pfLoop = psse->fLoop;
	ReleasePpsse(&psse);

	return fTrue;
}


/****************************************************
 *
 * This routine changes the volume of the sound of
 * type sty at the current frame to vlmNew
 *
 * Parameters:
 *  sty - the sound type to query
 *  vlmNew - the new volume
 *
 * Returns:
 *  none
 *
 ****************************************************/
void SCEN::SetSndVlmCore(long sty, long vlmNew)
{
	AssertThis(0);
	AssertIn(sty, 0, styLim);
	AssertIn(vlmNew, 0, kvlmFull + 1);

	PSEV qsev;
	long isev;
	PSSE psse;

    //
	// Check event list.
	//
	for (isev = _isevFrmLim - 1; isev >= 0; isev--)
		{
		qsev = (PSEV)_pggsevFrm->QvFixedGet(isev);

		if (qsev->nfrm != _nfrmCur)
			{
			Bug("No such sound");
            break; // sound not found
			}

		if (qsev->sevt == sevtPlaySnd)
			{
			psse = ((PSSE)_pggsevFrm->QvGet(isev));
			if (sty == psse->sty)
				{
				psse->vlm = vlmNew;
				_MarkMovieDirty();
				return;
				}
			}

		}

    //
	// End of list...sound not found
	//
	Bug("No such sound");
}


/******************************************************************************
	UpdateSndFrame
		Enumerates all scene events for the current frame, and asks all actors
		to enumerate all of their actor events for the current frame, looking
		for a sound event.  Has the movie's MCC update the frame-sound state
		based on the results of the search.
************************************************************ PETED ***********/
void SCEN::UpdateSndFrame(void)
{
	bool fSoundInFrame = fFalse;
	long iv = _isevFrmLim;

	while (iv-- > 0)
		{
		PSEV qsev = (PSEV)_pggsevFrm->QvFixedGet(iv);

		if (qsev->nfrm != _nfrmCur)
			{
			iv = -1;
			break;
			}

		if (qsev->sevt == sevtPlaySnd)
			{
			fSoundInFrame = fTrue;
			goto LDone;
			}
		}

	while (++iv < _pglpactr->IvMac())
		{
		PACTR pactr;

		_pglpactr->Get(iv, &pactr);
		AssertPo(pactr, 0);

		if (pactr->FSoundInFrm())
			{
			fSoundInFrame = fTrue;
			goto LDone;
			}
		}

LDone:
	Pmvie()->Pmcc()->SetSndFrame(fSoundInFrame);
}


/****************************************************
 *
 * This routine adds an event to the current frame of the current scene.
 *
 * Parameters:
 *  psev - Pointer to the event to add.
 *	cbVar- Size of pvVar buffer, in bytes.
 *  pvVar- Pointer to the variable part of the event.
 *
 * Returns:
 *	fTrue, if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::_FAddSev(PSEV psev, long cbVar, void *pvVar)
{
	AssertThis(0);
	AssertVarMem(psev);
	AssertIn(cbVar, 0, klwMax);

	bool fRetValue;

    //
	// Add the event to the scene
	//
	_MarkMovieDirty();

	fRetValue = _pggsevFrm->FInsert(_isevFrmLim++, cbVar, pvVar, psev);

	if (!fRetValue)
		{
		_isevFrmLim--;
		}

	return(fRetValue);
}



/****************************************************
 *
 * This routine sets the selected actor to the given one.
 *
 * Parameters:
 *  pactr - Pointer to the actr to select.  pvNil is a
 *  	valid value and deselects the current actor and tbox.
 *
 * Returns:
 *	None
 *
 ****************************************************/
void SCEN::SelectActr(ACTR *pactr)
{
	AssertThis(0);
	AssertNilOrPo(pactr, 0);

	PMVU pmvu;

	pmvu = (PMVU)Pmvie()->PddgGet(0);
	AssertNilOrPo(pmvu, 0);

	if ((pmvu != pvNil) && !pmvu->FTextMode())
		{
		if (pvNil != _pactrSelected)
			{
			_pactrSelected->Unhilite();
			}

		if (pvNil != pactr)
			{
			pactr->Hilite();
			}

		if (_ptboxSelected != pvNil)
			{
			_ptboxSelected->Select(fFalse);
			}
		}

	_pmvie->InvalViews();
	_pactrSelected = pactr;

	_pmvie->BuildActionMenu();
}


/****************************************************
 *
 * This routine sets the selected text box to the given one.
 *
 * Parameters:
 *  ptbox - Pointer to the tbox to select.  pvNil is a
 *  	valid value and deselects the current actor and tbox.
 *
 * Returns:
 *	None
 *
 ****************************************************/
void SCEN::SelectTbox(PTBOX ptbox)
{
	AssertThis(0);
	AssertNilOrPo(ptbox, 0);

	PMVU pmvu;

	pmvu = (PMVU)Pmvie()->PddgGet(0);
	AssertNilOrPo(pmvu, 0);

	_pmvie->InvalViews();

	if ((pmvu != pvNil) && pmvu->FTextMode())
		{

		if (pvNil != _pactrSelected)
			{
			_pactrSelected->Unhilite();
			_pmvie->BuildActionMenu();
			}

		if ((ptbox == _ptboxSelected) && ((ptbox == pvNil) || ptbox->FSelected()))
			{
			return;
			}

		if (pvNil != _ptboxSelected)
			{
			_ptboxSelected->Select(fFalse);
			}

		if (pvNil != ptbox)
			{
			ptbox->Select(fTrue);
			}
	}

	_ptboxSelected = ptbox;
	_pmvie->Pmcc()->TboxSelected();

}


/****************************************************
 *
 * This routine adds an actor to the scene at the current frame,
 * if the arid is aridNil, else it replaces the actor in the
 * scene with the same arid.
 *
 * Parameters:
 *  pactr - Pointer to the actr to add.
 *
 *
 * Returns:
 *	fTrue, if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FAddActrCore(ACTR *pactr)
{
	AssertThis(0);
	AssertPo(pactr, 0);

	PSEV qsev;
	SEV sev;
	long isev;
	long ipactr;
	STN stn;
	PACTR pactrOld;
	bool fRetValue;

	//
	// Check if actor is in Scene already.
	//
	for (isev=0; isev < _pggsevStart->IvMac(); isev++)
		{
		qsev = (PSEV)_pggsevStart->QvFixedGet(isev);

        if (qsev->sevt != sevtAddActr)
			{
            continue;
			}

		_pggsevStart->Get(isev, (void *)&pactrOld);

		if (pactrOld->Arid() != pactr->Arid())
			{
			continue;
			}

		//
		// Replace actor in scene roll call
		//
		for (ipactr = 0; ipactr < _pglpactr->IvMac(); ipactr++)
			{
			_pglpactr->Get(ipactr, &pactrOld);

			if (pactrOld->Arid() == pactr->Arid())
				{

				pactr->AddRef();
				pactr->SetPscen(this);

				//
				// Plop them into this frame
				//
				if (!pactr->FGotoFrame(_nfrmCur))
					{
					ReleasePpo(&pactr);
					return(fFalse);
					}

				//
				// Replace starting actor
				//
				_pggsevStart->Put(isev, &pactr);
				_pglpactr->Put(ipactr, &pactr);

				SelectActr(pactr);
				pactrOld->Hide();
				ReleasePpo(&pactrOld);
				InvalFrmRange();
				_MarkMovieDirty();
				UpdateSndFrame();
				return(fTrue);

				}

			}

			Bug("Cannot find actor in Roll call");

		}

	//
	// Add actor to inital list of events to do.
	//
	sev.sevt = sevtAddActr;
	fRetValue = _pggsevStart->FInsert(0, size(PACTR), &pactr, &sev);

	if (fRetValue)
		{
        pactr->AddRef();

		pactr->Ptmpl()->GetName(&stn);
		if (!_pmvie->FAddToRollCall(pactr, &stn))
			{
			_pggsevStart->Delete(0);
			ReleasePpo(&pactr);
			return(fFalse);
			}

		fRetValue = _FPlaySev(&sev, &pactr, _grfscen);

		if (!fRetValue)
			{
			_pmvie->RemFromRollCall(pactr);
			_pggsevStart->Delete(0);
			ReleasePpo(&pactr);
			}
		else
			{
			InvalFrmRange();
			_MarkMovieDirty();	
			UpdateSndFrame();
			}

		}

	return(fRetValue);
}



/****************************************************
 *
 * This routine adds an actor to the scene at the current frame.
 * This auto magically selects the actor and places them on stage.
 *
 * Parameters:
 *  pactr - Pointer to the actr to add.
 *
 *
 * Returns:
 *	fTrue, if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FAddActr(ACTR *pactr)
{
	AssertThis(0);
	AssertPo(pactr, 0);

	if (!FAddActrCore(pactr))
		{
		return(fFalse);
		}

	if (!pactr->FAddOnStageCore())
		{
		RemActrCore(pactr->Arid());
		return(fFalse);
		}

	SelectActr(pactr);

	//
	// The MVU creates the undo object for this because of the mouse
	// placement.
	//
	return(fTrue);
}


/****************************************************
 *
 * This routine removes an actor from the scene.
 *
 * Parameters:
 *  arid - The actor id to remove.
 *
 *
 * Returns:
 *  None
 *
 ****************************************************/
void SCEN::RemActrCore(long arid)
{
	AssertThis(0);

	PSEV qsev;
	PACTR pactrTmp;
	long isev;

	//
	// Check if actor is in Scene already.
	//
	for (isev=0; isev < _pggsevStart->IvMac(); isev++)
		{
		qsev = (PSEV)_pggsevStart->QvFixedGet(isev);

        if (qsev->sevt != sevtAddActr)
			{
            continue;
			}

		_pggsevStart->Get(isev, &pactrTmp);
		if (pactrTmp->Arid() != arid)
			{
            continue;
			}

		//
		// Remove actor from inital list of events to do.
		//
		_pggsevStart->Delete(isev);

        //
		// Remove actor from the scene roll call
		//
		for (isev=0; isev < _pglpactr->IvMac(); isev++)
			{
			_pglpactr->Get(isev, &pactrTmp);

            if (pactrTmp->Arid() != arid)
				{
                continue;
				}

            _pglpactr->Delete(isev);

            //
			// Remove actor as the currently selected actor
			//
			if (_pactrSelected == pactrTmp)
				{
                PMVU pmvu;

                _pactrSelected = pvNil;
				pmvu = (PMVU)_pmvie->PddgGet(0);
				if (pmvu != pvNil)
					{
					pmvu->EndPlaceActor();
					}

				}

			//
			// Remove actor from the movie roll call
			//
			_pmvie->RemFromRollCall(pactrTmp);

			pactrTmp->Hide();

			ReleasePpo(&pactrTmp);
			_MarkMovieDirty();
			UpdateSndFrame();

			return;

			}

        Bug("Actor does not exist in roll call");
		_MarkMovieDirty();
		return;

		}

	Bug("Actor does not exist in scene");
}



/****************************************************
 *
 * This routine removes an actor from the scene, and
 * creates an undo object for the action.
 *
 * Parameters:
 *  arid - The actor id to remove.
 *
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FRemActr(long arid)
{
	AssertThis(0);
	AssertIn(arid, 0, 500);

	PSUNA psuna;
	long ipactr;
	PACTR pactr;

	//
	// Find the actor for undo purposes
	//
	for (ipactr=0; ipactr < _pglpactr->IvMac(); ipactr++)
		{
		_pglpactr->Get(ipactr, &pactr);
        if (pactr->Arid() == arid)
			{
			break;
			}
		}

    AssertPo(pactr, 0);
	pactr->AddRef();

	psuna = SUNA::PsunaNew();

	if (psuna == pvNil)
		{
		ReleasePpo(&pactr);
		return(fFalse);
		}

	psuna->SetActr(pactr);
	psuna->SetType(utDel);

	if (!_pmvie->FAddUndo(psuna))
		{
		return(fFalse);
		}

    ReleasePpo(&psuna);

	RemActrCore(arid);

	return(fTrue);
}

/****************************************************
 *
 * This routine returns the actor pointed at by the mouse.
 *
 * Parameters:
 *	xp - X position of the mouse within the display space.
 *	yp - Y position of the mouse within the display space.
 *  pibset - Place to store the index of the group of body parts hit.
 *
 * Returns:
 *  Pointer to the actor, pvNil if none.
 *
 ****************************************************/
ACTR *SCEN::PactrFromPt(long xp, long yp, long *pibset)
{
	AssertThis(0);
	AssertVarMem(pibset);

	ACTR *pactr;
	BODY *pbody;
	long ipactr;

    pbody = BODY::PbodyClicked(xp, yp, Pmvie()->Pbwld(), pibset);	
	if (pvNil == pbody)
		{
		return pvNil;
		}

    //
	// loop through actors, call FIsMyBody()
	//
	for (ipactr = 0; ipactr < _pglpactr->IvMac(); ipactr++)
		{
		_pglpactr->Get(ipactr, &pactr);
		if (pactr->FIsMyBody(pbody))
			{
			return pactr;
			}
		}

	Bug("weird...we clicked a pbody, but we don't know whose it is!");

	return pvNil;
}



/****************************************************
 *
 * This routine adds a text box to the scene at the current frame
 *
 * Parameters:
 *  ptbox - Pointer to the text box to add.
 *
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FAddTboxCore(PTBOX ptbox)
{
	AssertThis(0);
	AssertPo(ptbox, 0);

	SEV sev;
	bool fRetValue;

#ifdef DEBUG

	PSEV qsev;
	long isev;

	//
	// Search for duplicate tbox
	//
	for (isev= 0; isev < _pggsevStart->IvMac(); isev++)
		{
    	qsev = (PSEV)_pggsevStart->QvFixedGet(isev);
		if ((qsev->sevt == sevtAddTbox) &&
			(ptbox == (PTBOX)_pggsevStart->QvGet(isev)))
			{
			Bug("Error, adding same text box twice");
			return(fTrue);
			}

 	    }
#endif

	//
	// Add text box to event list.
	//
	sev.sevt = sevtAddTbox;
	ptbox->SetScen(this);
	fRetValue = _pggsevStart->FInsert(_pggsevStart->IvMac(),
	                                  size(PTBOX), &ptbox, &sev
									 );

	if (fRetValue)
		{
        ptbox->AddRef();
		fRetValue = _FPlaySev(&sev, &ptbox, _grfscen);

		if (!fRetValue)
			{
			_pggsevStart->Delete(_pggsevStart->IvMac() - 1);
			}
		else
			{
			_MarkMovieDirty();	
			}
		}

	return(fRetValue);
}



/****************************************************
 *
 * This routine adds a text box to the scene at the current frame
 * creates an undo object for the action.
 *
 * Parameters:
 *  ptbox - Pointer to the text box to add.
 *
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FAddTbox(PTBOX ptbox)
{
	PSUNX psunx;
	long itbox;
	long nfrmFirst, nfrmLast;

	psunx = SUNX::PsunxNew();

	if (psunx == pvNil)
		{
		return(fFalse);
		}

	if (!FAddTboxCore(ptbox))
		{
        ReleasePpo(&psunx);
		return(fFalse);
		}

	ptbox->AddRef();
	ptbox->FGetLifetime(&nfrmFirst, &nfrmLast);
	psunx->SetTbox(ptbox);	// transfers the reference count to the undo object
	psunx->SetNfrmFirst(nfrmFirst);
	psunx->SetNfrmLast((nfrmLast == _nfrmLast) ? klwMax : nfrmLast);

	for (itbox = 0; itbox < _pglptbox->IvMac(); itbox++)
		{
		if (ptbox == PtboxFromItbox(itbox))
			{
			psunx->SetItbox(itbox);
			break;
			}
		}

    AssertIn(itbox, 0, _pglptbox->IvMac());

	psunx->SetAdd(fTrue);

	if (!_pmvie->FAddUndo(psunx))
		{
        PushErc(ercSocNotUndoable);
		ReleasePpo(&psunx);
		return(fFalse);
		}

    ReleasePpo(&psunx);
	return(fTrue);
}



/****************************************************
 *
 * This routine removes a text box from the scene.
 *
 * Parameters:
 *  ptbox - Pointer to the text box to remove.
 *
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FRemTboxCore(PTBOX ptbox)
{
	AssertThis(0);
	AssertPo(ptbox, 0);

	PSEV qsev;
	long isev;
	long itbox;
	long nfrmStart, nfrmLast;


	//
	// Check if currently selected tbox.
	//
	if (ptbox == _ptboxSelected)
		{

		_ptboxSelected = pvNil;
		if (ptbox != pvNil)
			{
			ptbox->Select(fFalse);
			}

		}

	//
	// Find the text box
	//
	for (isev = 0; isev < _pggsevStart->IvMac(); isev++)
		{
		qsev = (PSEV)_pggsevStart->QvFixedGet(isev);
		
        if ((qsev->sevt == sevtAddTbox) &&
 		   ((*(PTBOX *)_pggsevStart->QvGet(isev)) == ptbox))
 		    {
			
         	//
 		    // Remove it.  Do not ReleasePpo() here as reference count
		    // gets transfered to callee.
 		    //
 		    _pggsevStart->Delete(isev);

		    //
		    // Find it in the _pglptbox
		    //
		    for (itbox = 0; itbox < _pglptbox->IvMac(); itbox++)
			    {
                if (*(PTBOX *)_pglptbox->QvGet(itbox) == ptbox)
			  	    {

					if (_ptboxSelected == ptbox)
						{
						_ptboxSelected = pvNil;
						}

                    _pglptbox->Delete(itbox);
			 		if (ptbox->FGetLifetime(&nfrmStart, &nfrmLast))
						{
                        //
						// This will guarantee that the tbox doesn't leave
						// any display on the rendering area.
						//
						AssertDo(ptbox->FGotoFrame(nfrmStart - 1), "Could not remove a text box");
						}
					ReleasePpo(&ptbox);
				    _MarkMovieDirty();
				    return(fTrue);

				    }
			    }

			Bug("Text box not found in GL");
			return(fFalse);

 		    }

 	    }

    Bug("Error! Could not find text box for removal");
	return(fFalse);
}



/****************************************************
 *
 * This routine removes a text box from the scene,
 * creates an undo object for the action.
 *
 * Parameters:
 *  ptbox - Pointer to the text box to remove.
 *
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FRemTbox(PTBOX ptbox)
{
	AssertThis(0);
	AssertPo(ptbox, 0);

	PSUNX psunx;
	long nfrmFirst, nfrmLast;

	psunx = SUNX::PsunxNew();

	if (psunx == pvNil)
		{
		return(fFalse);
		}

	ptbox->AddRef();
	ptbox->FGetLifetime(&nfrmFirst, &nfrmLast);
	psunx->SetTbox(ptbox);	// transfers the reference count to the undo object
	psunx->SetAdd(fFalse);
	psunx->SetNfrmFirst(nfrmFirst);
	psunx->SetNfrmLast((nfrmLast == _nfrmLast) ? klwMax : nfrmLast);

	if (!_pmvie->FAddUndo(psunx))
		{
		ReleasePpo(&psunx);
		return(fFalse);
		}

    ReleasePpo(&psunx);

	if (!FRemTboxCore(ptbox))
		{
        _pmvie->ClearUndo();
		return(fFalse);
		}

	return(fTrue);
}



/****************************************************
 *
 * This routine gets the ith text box.
 *
 * Parameters:
 *  itbox - Index of the text box to get.
 *
 * Returns:
 *  Pointer to the text box if itbox is valid, else pvNil.
 *
 ****************************************************/
TBOX *SCEN::PtboxFromItbox(long itbox)
{
	AssertThis(0);
	Assert(itbox >= 0, "Bad index value");

	long ipo;
	PTBOX ptbox;

    for (ipo = 0; ipo < _pglptbox->IvMac(); ipo++)
		{

        _pglptbox->Get(ipo, &ptbox);

		itbox--;
        if (itbox == -1)
			{
            break;
			}

		}

    if (itbox != -1)
		{
        ptbox = pvNil;
		}

	return(ptbox);
}


/****************************************************
 *
 * This routine adds/removes a pause to the scene at the current frame.
 *
 * Parameters:
 *	pwit - The pause type, witNil removes a pause, returns old pause type.
 *  pdts - Valid only if wit==witForTime, dts is in clock ticks, returns
 *		old wait time if old wit was witForTime.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FPauseCore(WIT *pwit, long *pdts)
{
	AssertThis(0);
	AssertIn(*pwit, witNil, witLim);
	AssertIn(*pdts, 0, klwMax);

	SEV sev;
	long isev;
	PSEV qsev;
	SEVP sevp;
	WIT witOld;
	long dtsOld;

	//
	// Start at the first event of this frame.
	//
	for (isev=_isevFrmLim-1; isev >= 0; isev--)
		{
		qsev = (PSEV)_pggsevFrm->QvFixedGet(isev);

		if (qsev->nfrm != _nfrmCur)
			{
            break;
			}

		//
		// Find a pause
		//
		if (qsev->sevt == sevtPause)
			{

			//
 			// Replace the event
 			//
			_pggsevFrm->Get(isev, &sevp);
			witOld = sevp.wit;
			dtsOld = sevp.dts;

			if (*pwit == witNil)
				{
                _pggsevFrm->Delete(isev);
				_isevFrmLim--;
				}
			else
				{
			    sevp.wit = *pwit;
				sevp.dts = *pdts;
				_pggsevFrm->Put(isev, &sevp);
				}

			*pwit = witOld;
			*pdts = dtsOld;
			return(fTrue);
            }

		}

	//
	// Add pause to event list.
	//
	sev.nfrm = _nfrmCur;
	sev.sevt = sevtPause;
	sevp.wit = *pwit;
	sevp.dts = *pdts;
	if (!_FAddSev(&sev, size(long) * 2, &sevp))
		{
		return(fFalse);
		}

    *pwit = witNil;
	*pdts = 0;
	return(fTrue);
}



/****************************************************
 *
 * This routine adds/removes a pause to the scene at the current frame
 * and creates an undo object for the action.
 *
 * Parameters:
 *	wit - The pause type, removes pause if wit==witNil.
 *  dts - Valid only if wit==witForTime, dts is in clock ticks.
 *
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FPause(WIT wit, long dts)
{
	PSUNP psunp;

	psunp = SUNP::PsunpNew();

	if (psunp == pvNil)
		{
		return(fFalse);
		}

	if (!FPauseCore(&wit, &dts))
		{
        ReleasePpo(&psunp);
		return(fFalse);
		}

	psunp->SetWit(wit);
	psunp->SetDts(dts);
	psunp->SetAdd(fTrue);

	if (!_pmvie->FAddUndo(psunp))
		{
		FPauseCore(&wit, &dts);
		ReleasePpo(&psunp);
		return(fFalse);
		}

    ReleasePpo(&psunp);
	return(fTrue);
}


/****************************************************
 *
 * This routine sets the background for the scene.
 *
 * Parameters:
 *  ptag - Pointer to the background tag to put on the scene.
 *  ptagOld - Place to store the old background tag.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FSetBkgdCore(PTAG ptag, PTAG ptagOld)
{
	AssertThis(0);
	AssertVarMem(ptag);
	AssertVarMem(ptagOld);

	SEV sev;
	long isev;
	TAG tag;
	long vlm;
	bool fLoop;
	PMSND pmsnd;
	long sty;

	if (_pbkgd != pvNil)
		{
		//
		// Replace background
		//

		for (isev = 0; isev < _pggsevStart->IvMac(); isev++)
			{
            _pggsevStart->GetFixed(isev, &sev);

			if (sev.sevt == sevtSetBkgd)
				{
                _pggsevStart->Get(isev, ptagOld);
				if (!_pggsevStart->FPut(isev, size(TAG), ptag))
					{
					return(fFalse);
					}

				_pbkgd->TurnOffLights();
				ReleasePpo(&_pbkgd);

				_MarkMovieDirty();
				if (!_FPlaySev(&sev, ptag, _grfscen))
					{
					_pggsevStart->FPut(isev, size(TAG), ptagOld);
					_FPlaySev(&sev, ptagOld, _grfscen);
					return(fFalse);
					}

                AssertPo(_pbkgd, 0);
                _pbkgd->GetName(&_stnName);
				goto LSuccess;
				}

			}

        Bug("No background event found");
		return(fFalse);
		}

	TrashVar(ptagOld);

	//
	// Add set background event and play it.
	//
	sev.sevt = sevtSetBkgd;

	if (!_pggsevStart->FInsert(0, size(TAG), ptag, &sev))
		{
		return(fFalse);
		}

	if (!_FPlaySev(&sev, ptag, _grfscen))
		{
        _pggsevStart->Delete(0);
		return(fFalse);
		}

    AssertPo(_pbkgd, 0);
	_pbkgd->GetName(&_stnName);
	_MarkMovieDirty();

LSuccess:

	while (_pggsevFrm->IvMac() != 0)
		{
        //
		// Remove stale scene events
		//
		_pggsevFrm->Delete(_pggsevFrm->IvMac() - 1);
		}

    _isevFrmLim = 0;

	// Add the default sound for the new background, if any
	_pbkgd->GetDefaultSound(&tag, &vlm, &fLoop);
	if (tag.sid != ksidInvalid) // new background sound
		{
		// Note: since FSetBkgdCore is only called at edit time,
		// it's okay to call FCacheTag.  The background default
		// sound is not an intrinsic part of the BKGD...it's more
		// of a "serving suggestion" that the user can remove
		// once the background is added.
		Assert(!_pmvie->FPlaying(), "Shouldn't cache tags if movie is playing!");
		if (vptagm->FCacheTagToHD(&tag))
			{
			pmsnd = (PMSND)vptagm->PbacoFetch(&tag, MSND::FReadMsnd);
			if (pvNil != pmsnd)
				{
				sty = pmsnd->Sty();
				ReleasePpo(&pmsnd);
				// non-destructive if we fail
				FAddSndCore(fLoop, fFalse, vlm, sty, 1, &tag);
				}
			}
		}

	_pmvie->Pmcc()->SceneChange();

	if (vpcex != pvNil)
		{
		vpcex->EnqueueCid(cidSceneLoaded);
		}


	return(fTrue);
}


/****************************************************
 *
 * This routine sets the background for the scene.
 *
 * Parameters:
 *  pbkgd - Pointer to the background to put on the scene.
 *
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FSetBkgd(PTAG ptag)
{
	AssertThis(0);
	AssertVarMem(ptag);

	TAG tagOld;
	PSUNK psunk;
	long icam;

	if (_pbkgd != pvNil)
		{
        icam = _pbkgd->Icam();
		}

	if (!FSetBkgdCore(ptag, &tagOld))
		{
		return(fFalse);
		}

#ifdef DEBUG
	long lw;

	TrashVar(&lw);
	Assert(tagOld.sid != lw, "Use CORE function to set first background");
#endif

	psunk = SUNK::PsunkNew();

	if (psunk == pvNil)
		{
		if (!FSetBkgdCore(&tagOld, ptag))
			{
			_pmvie->ClearUndo();
            PushErc(ercSocNotUndoable);
			}
		return(fFalse);
		}

	psunk->SetTag(&tagOld);
	psunk->SetIcam(icam);
	psunk->SetFBkgd(fTrue);

	if (!_pmvie->FAddUndo(psunk))
		{
		ReleasePpo(&psunk);

		if (!FSetBkgdCore(&tagOld, ptag))
			{
			_pmvie->ClearUndo();
            PushErc(ercSocNotUndoable);
			}
		return(fFalse);
		}

    ReleasePpo(&psunk);

	return(fTrue);
}


/****************************************************
 *
 * This routine returns if a scene is currently empty
 *
 * Parameters:
 *  None.
 *
 * Returns:
 *  fTrue if the scene contains only camera changes and sounds
 *    and is only 1 frame long, else fFalse.
 *
 ****************************************************/
bool SCEN::FIsEmpty(void)
{
	AssertThis(0);

	long isev;
	SEV sev;

	if ((_pggsevStart->IvMac() != 1) || ((_nfrmLast - _nfrmFirst) > 0))
		{
		return(fFalse);
		}

	for (isev = 0; isev < _pggsevFrm->IvMac(); isev++)
		{

        _pggsevFrm->GetFixed(isev, &sev);
		if (sev.sevt != sevtChngCamera && sev.sevt != sevtPlaySnd)
			{
			return(fFalse);
			}

		}

	return(fTrue);
}

/****************************************************
 *
 * This routine changes the camera view point at this frame.
 *
 * Parameters:
 *  icam - The camera number in the BKGD to switch to.
 *
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FChangeCamCore(long icam, long *picamOld)
{
	AssertThis(0);
	AssertIn(icam, 0, 500);
	AssertVarMem(picamOld);

	PSEV qsev, qsevOld;
	SEV sev;
	long isev, isevCam;

	//
	// Check for a current camera change.
	//
	*picamOld = 0;

	for (isev= _isevFrmLim - 1; isev >= 0; isev--)
		{
		qsev = (PSEV)_pggsevFrm->QvFixedGet(isev);

        if (qsev->sevt == sevtChngCamera)
			{

			if (qsev->nfrm == _nfrmCur)
				{

                //
				// Check if this new camera matches the previous
				// camera
				//
				for (isevCam = isev - 1; isevCam >= 0; isevCam--)
					{

					qsevOld = (PSEV)_pggsevFrm->QvFixedGet(isevCam);
					if (qsevOld->sevt == sevtChngCamera)
						{
						_pggsevFrm->Get(isevCam, picamOld);
						break;
						}

					}

                //
				// If they are equal, then change camera to
				// previous camera and remove this change event.
				//
                if (*picamOld == icam)
					{
					qsevOld = (PSEV)_pggsevFrm->QvFixedGet(isev);
					_pggsevFrm->Get(isev, picamOld);
					if (_FPlaySev(qsevOld, &icam, _grfscen))
						{
						_pggsevFrm->Delete(isev);
						_isevFrmLim--;
						_MarkMovieDirty();
						goto LSuccess;
						}
					return(fFalse);
					}

				//	
				// Change it
				//
				_pggsevFrm->Get(isev, picamOld);
				_pggsevFrm->Put(isev, &icam);
				_MarkMovieDirty();
				if (_FPlaySev(qsev, &icam, _grfscen))
					{
                    goto LSuccess;
					}
				return(fFalse);
			    }
            else
				{
				_pggsevFrm->Get(isev, picamOld);
				break;
				}

            }

 	    }

   	if (*picamOld == icam)
   		{
   		goto LSuccess;
   		}

	//
	// Add camera change to event list.
	//
	sev.nfrm = _nfrmCur;
	sev.sevt = sevtChngCamera;

	if (_FAddSev(&sev, size(long), &icam))
		{
		if (_FPlaySev(&sev, &icam, _grfscen))
			{
            goto LSuccess;
			}
		else
			{
			_pggsevFrm->Delete(--_isevFrmLim);
			}
	    }

	return(fFalse);

LSuccess:

	//
	// Check for later camera change
	//
	for (isev = _isevFrmLim; isev < _pggsevFrm->IvMac(); isev++)
		{
		long icamNext;
        qsev = (PSEV)_pggsevFrm->QvFixedGet(isev);
		if (qsev->sevt == sevtChngCamera)
			{
			_pggsevFrm->Get(isev, &icamNext);
            if (icamNext == icam)
				{
		        _pggsevFrm->Delete(isev);
				_isevFrmLim;
				}
            break;
			}
		}

	return(fTrue);

}



/****************************************************
 *
 * This routine changes the camera view point at this frame
 * and creates an undo object for the action.
 *
 * Parameters:
 *  icam - The camera number in the BKGD to switch to.
 *
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FChangeCam(long icam)
{
	long icamOld;
	PSUNK psunk;
	TAG tagCam;

	if (!vptagm->FBuildChildTag(&_tagBkgd, icam, kctgCam, &tagCam))
		{
		return fFalse;
		}

	if (!vptagm->FCacheTagToHD(&tagCam))
		{
		return fFalse;
		}

	if (!FChangeCamCore(icam, &icamOld))
		{
		return(fFalse);
		}

	psunk = SUNK::PsunkNew();

	if (psunk == pvNil)
		{
		if (!FChangeCamCore(icam, &icamOld))
			{
			_pmvie->ClearUndo();
			PushErc(ercSocNotUndoable);
			}
		return(fFalse);
		}

	psunk->SetIcam(icamOld);
	psunk->SetFBkgd(fFalse);

	if (!_pmvie->FAddUndo(psunk))
		{
		ReleasePpo(&psunk);

		if (!FChangeCamCore(icam, &icamOld))
			{
			_pmvie->ClearUndo();
			PushErc(ercSocNotUndoable);
			}
		return(fFalse);
		}

    ReleasePpo(&psunk);
	return(fTrue);
}

/****************************************************
 *
 * This routine reads in a scene from a chunky file.
 *
 * Parameters:
 *  pcrf - Pointer to the chunky file to read from.
 *  cno  - Cno within the chunky file to read.
 *
 * Returns:
 *  pvNil, if failure, else a pointer to the scene.
 *
 ****************************************************/
SCEN *SCEN::PscenRead(PMVIE pmvie, PCRF pcrf, CNO cno)
{
	AssertPo(pmvie, 0);
	AssertPo(pcrf, 0);

	PSCEN pscen = pvNil;
	BLCK blck;
	KID kid;
	long isevFrm = 0;
	long isevStart = 0;
	SEV sev;
	PSEV qsev;
	short bo;
	PACTR pactr;
	PTBOX ptbox;
	CHID chid;
	SCENH scenh;
	PCFL pcfl;

	pcfl = pcrf->Pcfl();

	//
	// Find the chunk and read in the header.
	//
	if (!pcfl->FFind(kctgScen, cno, &blck) || !blck.FUnpackData() ||
		(blck.Cb() != size(SCENH)) ||
		!blck.FReadRgb(&scenh, size(SCENH), 0))
		{
        goto LFail0;
		}

	//
	// Check header for byte swapping
	//
    if (scenh.bo == kboOther)
		{
		SwapBytesBom(&scenh, kbomScenh);
		}
    else
		{
        Assert(scenh.bo == kboCur, "Bad Chunky file");
		}

    //
	// Create our scene object.
	//
    pscen = NewObj SCEN(pmvie);

	if (pscen == pvNil)
		{
		goto LFail0;
		}

	pscen->_isevFrmLim = 0;

	//
	// Initialize roll call	for actors
	//
	pscen->_pglpactr = GL::PglNew(size(PACTR), 0);
	if (pscen->_pglpactr == pvNil)
		{
		goto LFail0;
		}

	//
	// Initialize roll call	for text boxes
	//
	pscen->_pglptbox = GL::PglNew(size(PTBOX), 0);
	if (pscen->_pglptbox == pvNil)
		{
		goto LFail0;
		}
		
	//
	// Read Frame information
	//
	pscen->_nfrmLast = scenh.nfrmLast;
	pscen->_nfrmFirst = scenh.nfrmFirst;
	pscen->_trans = scenh.trans;
	pscen->_nfrmCur = pscen->_nfrmFirst - 1;

	//
	// Read in thumbnail
	//
	if (pcfl->FGetKidChidCtg(kctgScen, cno, 0, kctgThumbMbmp, &kid) &&
		pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		{
		pscen->_pmbmp = MBMP::PmbmpRead(&blck);
		}

	//
	// Read in GG of Frame events
	//
	if (!pcfl->FGetKidChidCtg(kctgScen, cno, 0, kctgFrmGg, &kid) ||
		!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		{
		goto LFail0;
		}

	pscen->_pggsevFrm = GG::PggRead(&blck, &bo);
	if (pscen->_pggsevFrm == pvNil)
		{
		goto LFail0;
		}

    Assert(pscen->_pggsevFrm->CbFixed() == size(SEV), "Bad GG read for event");

    //
	// Convert all open tags to pointers.
	//
	for (; isevFrm < pscen->_pggsevFrm->IvMac(); isevFrm++)
		{
		qsev = (PSEV)pscen->_pggsevFrm->QvFixedGet(isevFrm);

		//
		// Swap byte ordering of entry
		//
		if (bo == kboOther)
			{
			SwapBytesBom((void *)qsev, kbomSev);
			}

		//
		// Open all tags
		//
		switch (qsev->sevt)
			{
		case sevtPlaySnd:

			PSSE psse;
			long itag;

			psse = SSE::PsseDupFromGg(pscen->_pggsevFrm, isevFrm, fFalse);
			if (pvNil == psse)
				goto LFail1;

			if (bo == kboOther)
				{
				psse->SwapBytes();
				}

			for (itag = 0; itag < psse->ctagc; itag++)
				{
				if (!TAGM::FOpenTag(psse->Ptag(itag), pcrf, pcfl))
					{
					while (itag-- > 0)
						TAGM::CloseTag(psse->Ptag(itag));
					FreePpv((void **)&psse); // don't ReleasePpsse...tags are already closed
					goto LFail1;
					}
				}
			// Put SSE with opened tags back in GG
			pscen->_pggsevFrm->Put(isevFrm, psse);
			FreePpv((void **)&psse); // don't ReleasePpsse because GG keeps the tags
			break;

		case sevtChngCamera:
		case sevtPause:
			break;

		case sevtAddActr:
		case sevtSetBkgd:
		case sevtAddTbox:
		default:
			Assert(0, "Bad event in frame event list");
			break;
			}

		}

	//
	// Read starting events
	//
	ReleasePpo(&pscen->_pggsevStart);

	if (!pcfl->FGetKidChidCtg(kctgScen, cno, 1, kctgStartGg, &kid) ||
		!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		{
		goto LFail1;
		}

	pscen->_pggsevStart = GG::PggRead(&blck, &bo);

	if (pscen->_pggsevStart == pvNil)
		{
		goto LFail1;
		}

    Assert(pscen->_pggsevStart->CbFixed() == size(SEV), "Bad GG read for event");

    //
	// Convert all open tags to pointers.
	//
	for (; isevStart < pscen->_pggsevStart->IvMac(); isevStart++)
		{
		qsev = (PSEV)pscen->_pggsevStart->QvFixedGet(isevStart);

		//
		// Swap byte ordering of entry
		//
		if (bo == kboOther)
			{
			SwapBytesBom((void *)qsev, kbomSev);
			}

		//
		// Convert CHIDs to pointers
		//
		switch (qsev->sevt)
			{
		case sevtAddActr:

			pscen->_pggsevStart->Get(isevStart, &chid);
			if (bo == kboOther)
				{
				SwapBytesBom((void *)&chid, kbomLong);
				}


			if (!pcfl->FGetKidChidCtg(kctgScen, cno, chid, kctgActr, &kid))
				{
				goto LFail1;
				}

			pactr = ACTR::PactrRead(pcrf, kid.cki.cno);
			AssertNilOrPo(pactr, 0);

			if (pactr == pvNil)
				{
				goto LFail1;
				}

			pscen->_pggsevStart->Put(isevStart, &pactr);
			break;

		case sevtAddTbox:

			pscen->_pggsevStart->Get(isevStart, &chid);
			if (bo == kboOther)
				{
				SwapBytesBom((void *)&chid, kbomLong);
				}

			if (!pcfl->FGetKidChidCtg(kctgScen, cno, chid, kctgTbox, &kid))
				{
				goto LFail1;
				}

			ptbox = TBOX::PtboxRead(pcrf, kid.cki.cno, pscen);
			AssertNilOrPo(ptbox, 0);

			if (ptbox == pvNil)
				{
				goto LFail1;
				}

			pscen->_pggsevStart->Put(isevStart, &ptbox);
			break;

		case sevtSetBkgd:
		case sevtChngCamera:
			break;

		case sevtPause:
		case sevtPlaySnd:
		default:
			Bug("Bad event in start event list");
			break;
			}

		}

	//
	// Read Name
	//
	pcfl->FGetName(kctgScen, cno, &pscen->_stnName);

	AssertPo(pscen, 0);

	return(pscen);

LFail1:

	//
	// Destroy all created objects
	//
	while (isevStart--)
		{
		pscen->_pggsevStart->GetFixed(isevStart, &sev);
		switch (sev.sevt)
			{
		case sevtAddActr:
			pscen->_pggsevStart->Get(isevStart, &pactr);
			ReleasePpo(&pactr);
 			break;
 		case sevtAddTbox:
			pscen->_pggsevStart->Get(isevStart, &ptbox);
			ReleasePpo(&ptbox);
 			break;

 	    case sevtChngCamera:
		case sevtSetBkgd:
 		case sevtPause:
 			break;
			}
		}
	ReleasePpo(&pscen->_pggsevStart);

	//
	// Destroy all created objects
	//
	while (isevFrm--)
		{
		pscen->_pggsevFrm->GetFixed(isevFrm, &sev);
		switch (sev.sevt)
			{
		case sevtAddActr:
 		case sevtAddTbox:
			Bug("Bad event in frame list");
 			break;

   		case sevtPlaySnd:
			PSSE qsse;
			long itag;

			qsse = (PSSE)pscen->_pggsevFrm->QvGet(isevFrm);
			if (qsse->Cb() != (pscen->_pggsevFrm->CbFixed() +
					pscen->_pggsevFrm->Cb(isevFrm)))
				{
				Bug("Wrong size for SSE in GG");
				continue;
				}

			/* Close all tags; retrieve qsse each time...in theory, nothing
				that happens during CloseTag should cause mem to move, but this
				is a failure case, so it's okay to be slow, especially when we
				can be safe-not-sorry.  */
			for (itag = 0; itag < qsse->ctagc; itag++)
				{
				qsse = (PSSE)pscen->_pggsevFrm->QvGet(isevFrm);
				TAGM::CloseTag(qsse->Ptag(itag));
				}
			break;

		case sevtSetBkgd:
 	    case sevtChngCamera:
 		case sevtPause:
 			break;
			}
		}
	ReleasePpo(&pscen->_pggsevFrm);

LFail0:
	ReleasePpo(&pscen);
	return(pvNil);
}


/****************************************************
 *
 * This routine plays all the starting events for a scene.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FPlayStartEvents(bool fActorsOnly)
{
	AssertThis(0);

	long isev;

	//
	// This needs to play all the events in _pggsevStart
	//
	for (isev=0; isev < _pggsevStart->IvMac(); isev++)
		{
		SEV sev;

		_pggsevStart->GetFixed(isev, &sev);
		if (fActorsOnly && sev.sevt != sevtAddActr)
			continue;

		if (!_FPlaySev(&sev, _pggsevStart->QvGet(isev), _grfscen))
			{
			return(fFalse);
			}

		}

	return(fTrue);
}


/****************************************************
 *
 * This routine returns the bkgd tag in *ptag
 *
 ****************************************************/
bool SCEN::FGetTagBkgd(PTAG ptag)
{
	AssertThis(0);
	AssertVarMem(ptag);

	SEV sev;
	long isevStart;

	for (isevStart = 0; isevStart < _pggsevStart->IvMac(); isevStart++)
		{
		sev = *(PSEV)_pggsevStart->QvFixedGet(isevStart);
		if (sevtSetBkgd == sev.sevt)
			{
			*ptag = *(PTAG)_pggsevStart->QvGet(isevStart);
			return fTrue;
			}
		}
	return fFalse;
}


/****************************************************
 *
 * This routine writes a scene into a chunky file.
 *
 * Parameters:
 *  pcrf - Pointer to the chunky file to write to.
 *  pcno  - Cno within the chunky file written to.
 *
 * Returns:
 *  fFalse if it fails, else fTrue.
 *
 ****************************************************/
bool SCEN::FWrite(PCRF pcrf, CNO *pcno)
{
	AssertThis(0);
	AssertPo(pcrf, 0);

	PGG pggFrmTemp = pvNil;
	PGG pggStartTemp = pvNil;
	SEV sev;
	CHID chidActr, chidTbox;
	CNO cnoChild, cnoFrmEvent, cnoStartEvent;
	SCENH scenh;
	long isevFrm = -1;
	long isevStart = -1;
	long cb;
	BLCK blck;
	PCFL pcfl;

	chidActr = chidTbox = 0;

 	pcfl = pcrf->Pcfl();

	*pcno = cnoNil;

	//
	// Get a new CNO for this chunk
	//
	if (!pcfl->FAdd(0, kctgScen, pcno))
		{
		goto LFail;
		}

	//
	// Copy frame event GG to temporary GG
	//
	pggFrmTemp = GG::PggNew(size(SEV));

	if (pggFrmTemp == pvNil)
		{
        goto LFail;
		}

	for (isevFrm = 0; isevFrm < _pggsevFrm->IvMac(); isevFrm++)
		{
		sev = *(PSEV)_pggsevFrm->QvFixedGet(isevFrm);

		//
		// Convert pointers in the GG to CHIDs
		//
        switch (sev.sevt)
			{
 		case sevtPlaySnd:
				{
				bool fSuccess = fFalse;
				PSSE psse;	
				long itag;
				KID kid;

				psse = SSE::PsseDupFromGg(_pggsevFrm, isevFrm);
				if (pvNil == psse)
					{
					goto LFail;
					}
				for (itag = 0; itag < psse->ctagc; itag++)
					{
					// For user sounds, the tag's cno must already be correct.
					// Note: FResolveSndTag can't succeed if the msnd chunk is
					// not yet a child of the scene.
					if (psse->Ptagc(itag)->tag.sid != ksidUseCrf)
						continue; // tag will be closed by ReleasePpsse

					// If the msnd chunk already exists as this chid of this scene, continue
					if (pcfl->FGetKidChidCtg(kctgScen, *pcno, *psse->Pchid(itag), kctgMsnd, &kid))
						continue; // tag will be closed by ReleasePpsse

					// If the msnd does not exist in this file, it exists in the main movie
					if (!pcfl->FFind(kctgMsnd, psse->Ptag(itag)->cno))
						continue; // tag will be closed by ReleasePpsse

					// The msnd chunk has not been adopted into the scene as the specified chid
					if (!pcfl->FAdoptChild(kctgScen, *pcno, kctgMsnd,
						psse->Ptag(itag)->cno, *psse->Pchid(itag)))
 						{
						goto LEndPlaySnd;
						}
					}

				if (pggFrmTemp->FInsert(isevFrm, psse->Cb(), psse, &sev))
					{
					fSuccess = fTrue;
					}
LEndPlaySnd:
				ReleasePpsse(&psse);
				if (!fSuccess)
					{
					goto LFail;
					}
				break;
				}

 	    case sevtChngCamera:
			if (!pggFrmTemp->FInsert(isevFrm, size(long),
				                     _pggsevFrm->QvGet(isevFrm), &sev))
				{
				goto LFail;
				}
            break;

		case sevtPause:
			if (!pggFrmTemp->FInsert(isevFrm, size(SEVP),
				                     _pggsevFrm->QvGet(isevFrm), &sev))
				{
				goto LFail;
				}

			break;

		case sevtAddActr:
		case sevtSetBkgd:
		case sevtAddTbox:
			Assert(0, "Bad event in frame event list");
			break;
			}

		}

	//
	// Copy start event GG to temporary GG
	//
	pggStartTemp = GG::PggNew(size(SEV));

	if (pggStartTemp == pvNil)
		{
		goto LFail;
		}

	for (isevStart = 0; isevStart < _pggsevStart->IvMac(); isevStart++)
		{
		sev = *(PSEV)_pggsevStart->QvFixedGet(isevStart);
		//
		// Convert pointers in the GG to CHIDs
		//
        switch (sev.sevt)
			{
		case sevtAddActr:

            if (!pcfl->FAddChild(kctgScen, *pcno, chidActr, 0,
					kctgActr, &cnoChild))
				{
				goto LFail;
				}

			if (!(*(PACTR *)_pggsevStart->QvGet(isevStart))->FWrite(pcfl, cnoChild, *pcno))
				{
				goto LFail;
				}

			if (!pggStartTemp->FInsert(isevStart, size(CHID), &chidActr, &sev))
				{
				goto LFail;
				}

			chidActr++;
			break;

		case sevtSetBkgd:
			if (!TAGM::FSaveTag((PTAG)_pggsevStart->QvGet(isevStart), pcrf, fFalse))
				{
				goto LFail;
				}				

			if (!pggStartTemp->FInsert(isevStart, size(TAG), _pggsevStart->QvGet(isevStart), &sev))
				{
				goto LFail;
				}				
			break;

 	    case sevtChngCamera:
			if (!pggStartTemp->FInsert(isevStart, size(long), _pggsevStart->QvGet(isevStart), &sev))
				{
				goto LFail;
				}				
			break;

		case sevtAddTbox:
            if (!pcfl->FAddChild(kctgScen, *pcno, chidTbox, 0,
				    kctgTbox, &cnoChild))
				{
				goto LFail;
				}

			if (!(*(PTBOX *)_pggsevStart->QvGet(isevStart))->FWrite(pcfl, cnoChild))
				{
				goto LFail;
				}

			if (!pggStartTemp->FInsert(isevStart, size(CHID), &chidTbox, &sev))
				{
				goto LFail;
				}

			chidTbox++;
 			break;

		case sevtPause:
   		case sevtPlaySnd:
	 	default:
			Assert(0, "Bad event in frame event list");
			break;
			}

		}

	//
	// Save info into scene chunk
	//
	cb = pggFrmTemp->CbOnFile();
	if (!pcfl->FAdd(cb, kctgFrmGg, &cnoFrmEvent, &blck))
		{
        goto LFail;
		}

	if (!pggFrmTemp->FWrite(&blck))
		{
		pcfl->Delete(kctgFrmGg, cnoFrmEvent);
        goto LFail;
		}

    if (!pcfl->FAdoptChild(kctgScen, *pcno, kctgFrmGg, cnoFrmEvent, 0))
		{
		pcfl->Delete(kctgFrmGg, cnoFrmEvent);
        goto LFail;
		}
    pcfl->SetLoner(kctgFrmGg, cnoFrmEvent, fFalse);


	cb = pggStartTemp->CbOnFile();
	if (!pcfl->FAdd(cb, kctgStartGg, &cnoStartEvent, &blck))
		{
        goto LFail;
		}

	if (!pggStartTemp->FWrite(&blck))
		{
		pcfl->Delete(kctgStartGg, cnoStartEvent);
        goto LFail;
		}

    if (!pcfl->FAdoptChild(kctgScen, *pcno, kctgStartGg, cnoStartEvent, 1))
		{
		pcfl->Delete(kctgStartGg, cnoStartEvent);
        goto LFail;
		}
    pcfl->SetLoner(kctgStartGg, cnoStartEvent, fFalse);


	//
	// Save thumbnail, if there is one.
	//
	_UpdateThumbnail();

	if (_pmbmp != pvNil)
		{
		cb = _pmbmp->CbOnFile();
		if (!pcfl->FAdd(cb, kctgThumbMbmp, &cnoChild, &blck))
			{
	        goto LFail;
			}

		if (!_pmbmp->FWrite(&blck))
			{
			pcfl->Delete(kctgThumbMbmp, cnoChild);
	        goto LFail;
			}

	    if (!pcfl->FAdoptChild(kctgScen, *pcno, kctgThumbMbmp, cnoChild, 0))
			{
			pcfl->Delete(kctgThumbMbmp, cnoChild);
	        goto LFail;
			}
	    pcfl->SetLoner(kctgThumbMbmp, cnoChild, fFalse);
		}

	//
	// Create header buffer for scene chunk
	//
	scenh.bo = kboCur;
	scenh.osk = koskCur;
	scenh.nfrmLast = _nfrmLast;
	scenh.nfrmFirst = _nfrmFirst;
	scenh.trans = _trans;

	//
	// Write scene chunk
	//
	if (!pcfl->FSetName(kctgScen, *pcno, &_stnName))
		{
        goto LFail;
		}

	if (!pcfl->FPutPv((void *)&scenh, size(SCENH), kctgScen, *pcno))
		{
        goto LFail;
		}

    ReleasePpo(&pggFrmTemp);
    ReleasePpo(&pggStartTemp);

	return(fTrue);

LFail:

	//
	// Delete chunks createdk.
	//
	if (*pcno != cnoNil)
		{
		pcfl->Delete(kctgScen, *pcno);
		}

    ReleasePpo(&pggStartTemp);
    ReleasePpo(&pggFrmTemp);

	return(fFalse);
}

/****************************************************
 *
 * This routine resolves all sound tags
 *
 * Parameters:
 *  None
 * Returns:
 *  None
 *
 ****************************************************/
bool SCEN::FResolveAllSndTags(CNO cnoScen)
{
	AssertThis(0);

	long ipactr, ipactrMac;
	long isev, isevMac;
	bool fSuccess = fFalse;

	ipactrMac = _pglpactr->IvMac();
	for (ipactr = 0; ipactr < ipactrMac; ipactr++)
		{
		PACTR pactr;

		_pglpactr->Get(ipactr, &pactr);
		if (!pactr->FResolveAllSndTags(cnoScen))
			goto LFail;
		}

	_pggsevFrm->Lock();
	isevMac = _pggsevFrm->IvMac();
	for (isev = 0; isev < isevMac; isev++)
		{
		long itag;
		PSSE psse;
		SEV sev;

		sev = *(PSEV)_pggsevFrm->QvFixedGet(isev);
		if (sev.sevt != sevtPlaySnd)
			continue;

		psse = (PSSE)_pggsevFrm->QvGet(isev);
		for (itag = 0; itag < psse->ctagc; itag++)
			{
			if (psse->Ptag(itag)->sid == ksidUseCrf)
				{
				if (!_pmvie->FResolveSndTag(psse->Ptag(itag),
						*psse->Pchid(itag), cnoScen))
					{
					goto LFail;
					}
				}
			}
		}
	_pggsevFrm->Unlock();

	fSuccess = fTrue;
LFail:
	return fSuccess;
 }


/****************************************************
 *
 * This routine removes all actors from movie's roll call.
 *
 * Parameters:
 *  fDelIfOnlyRef  --  fTrue indicates to remove the roll-call entry
 *        if a given actr is only referenced by this scene.
 *
 * Returns:
 *  None
 *
 ****************************************************/
void SCEN::RemActrsFromRollCall(bool fDelIfOnlyRef)
{
	AssertThis(0);

	PACTR pactr;
	long ipactr;

	for (ipactr = 0; ipactr < _pglpactr->IvMac(); ipactr++)
		{
        _pglpactr->Get(ipactr, &pactr);
		Pmvie()->RemFromRollCall(pactr, fDelIfOnlyRef);
		}

}



/****************************************************
 *
 * This routine adds all actors to movie's roll call.
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FAddActrsToRollCall(void)
{
	AssertThis(0);

	PACTR pactr;
	long ipactr;
	STN stn;

	for (ipactr = 0; ipactr < _pglpactr->IvMac(); ipactr++)
		{

        _pglpactr->Get(ipactr, &pactr);
		AssertPo(pactr, 0);
		pactr->GetName(&stn);

		if (!Pmvie()->FAddToRollCall(pactr, &stn))
			{

            for (; ipactr > 0 ; )
				{
                ipactr--;
				_pglpactr->Get(ipactr, &pactr);
				Pmvie()->RemFromRollCall(pactr);
				}

			return(fFalse);

			}

		}

	return(fTrue);
}


/****************************************************
 *
 * This routine returns the scene's thumbnail.
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  None
 *
 ****************************************************/
PMBMP SCEN::PmbmpThumbnail(void)
{
	AssertThis(0);

	_UpdateThumbnail();
	return(_pmbmp);
}


/****************************************************
 *
 * This routine updates the mbmp associated with the
 * thumbnail for the scene.
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  None
 *
 ****************************************************/
void SCEN::_UpdateThumbnail(void)
{
	AssertThis(0);

	long nfrmCur;
	PGPT pgpt, pgptThumb;
	PMVU pmvu;
	PTBOX ptbox = PtboxSelected();
	PACTR pactr = PactrSelected();
	RC rc, rcThumb;
	long grfscenSave;
	long dtimSnd;

	dtimSnd =  Pmvie()->Pmsq()->DtimSnd();
	Pmvie()->Pmsq()->SndOff();

	pmvu = (PMVU)Pmvie()->PddgGet(0);
	if ((_pbkgd == pvNil) || (pmvu == pvNil))
		{
		goto LEnd;
		}
	AssertPo(pmvu, 0);

	if (!Pmvie()->FDirty())
		{
		goto LEnd;
		}

	rc.Set(0, 0, Pmvie()->Pmcc()->Dxp(), Pmvie()->Pmcc()->Dyp());
	pgpt = GPT::PgptNewOffscreen(&rc, 8);

	if (pgpt == pvNil)
		{
        goto LEnd;
		}

    AssertPo(pgpt, 0);

	rcThumb.Set(0, 0, kdxpThumbnail, kdypThumbnail);
	pgptThumb = GPT::PgptNewOffscreen(&rcThumb, 8);

	if (pgptThumb == pvNil)
		{
        ReleasePpo(&pgpt);
        goto LEnd;
		}

    AssertPo(pgptThumb, 0);

	pgptThumb->SetOffscreenColors(Pmvie()->PglclrThumbPalette());

	grfscenSave = _grfscen;
	Disable(fscenPauses | fscenSounds);

	nfrmCur = _nfrmCur;
	if ((_nfrmCur != _nfrmFirst) && !FGotoFrm(_nfrmFirst))
		{
        ReleasePpo(&pgpt);
        ReleasePpo(&pgptThumb);
		_grfscen = grfscenSave;
		goto LEnd;
		}

	if (pmvu->FTextMode())
		{
		SelectTbox(pvNil);
		}
	else
		{
		SelectActr(pvNil);
		}
	pmvu->DrawTree(pgpt, pvNil, &rc, fgobNoVis);
	if (pmvu->FTextMode())
		{
		SelectTbox(ptbox);
		}
	else
		{
		SelectActr(pactr);
		}
	
	BLOCK{
		GNV gnv(pgpt);
		GNV gnvThumb(pgptThumb);
		gnvThumb.CopyPixels(&gnv, &rc, &rcThumb);

		ReleasePpo(&_pmbmp);

	    _pmbmp = MBMP::PmbmpNew(pgptThumb->PrgbLockPixels(),
								pgptThumb->CbRow(),
								kdypThumbnail,
								&rcThumb,
								0, 0, kbTransparent
							   );
		pgptThumb->Unlock();

	    ReleasePpo(&pgpt);
	    ReleasePpo(&pgptThumb);
		if ((nfrmCur > _nfrmFirst) && (nfrmCur <= _nfrmLast))
			{
			FGotoFrm(nfrmCur);
			}

		_grfscen = grfscenSave;
		}

LEnd:

	Pmvie()->Pmsq()->SndOnDtim(dtimSnd);
	return;
}


/****************************************************
 *
 * This routine marks the movie as dirty.
 *
 * Parameters:
 *  fDirty - Set dirty or not.
 *
 * Returns:
 *  None
 *
 ****************************************************/
void SCEN::MarkDirty(bool fDirty)
{
	AssertThis(0);
	if (fDirty)
		{
		_MarkMovieDirty();
		}
}


/****************************************************
 *
 * This routine pastes an actor into the current scene
 *
 * Parameters:
 *  pactr - Pointer to the actor to paste
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FPasteActrCore(PACTR pactr)
{
	AssertThis(0);
	AssertPo(pactr, 0);

	//
	// Paste the actor.
	//
	if (!pactr->FPaste(_nfrmCur, this))
		{
		return(fFalse);
		}

	//
	// Add the actor
	//
	if (!FAddActrCore(pactr))
		{
		return(fFalse);
		}

	InvalFrmRange();
	if (!pactr->FGotoFrame(_nfrmCur))
		{
		RemActrCore(pactr->Arid());
		return(fFalse);
		}
	_MarkMovieDirty();

	_pmvie->Pmcc()->UpdateRollCall();
	UpdateSndFrame();

	return(fTrue);
}


/****************************************************
 *
 * This routine pastes an actor into the current scene
 * and creates an undo object for the action.
 *
 * NOTE: This does not need an undo object, since the
 * tool is getting set to a "place" tool, which will
 * create the appropriate undo type.
 *
 * Parameters:
 *  pactr - Pointer to the actor to paste
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::FPasteActr(PACTR pactr)
{
	AssertThis(0);
	AssertPo(pactr, 0);

	if (!FPasteActrCore(pactr))
		{
		return(fFalse);
		}

	return(fTrue);
}


/****************************************************
 *
 * This routine makes all actors go to a specific frame.
 *
 * Parameters:
 *  nfrm - Frame number to go to.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::_FForceActorsToFrm(long nfrm, bool *pfSoundInFrame)
{
	AssertThis(0);
	AssertIn(nfrm, klwMin, klwMax);

	PACTR pactr;
	long iactr;

	for (iactr = 0; iactr < _pglpactr->IvMac(); iactr++)
		{
		_pglpactr->Get(iactr, &pactr);
		AssertPo(pactr, 0);

		if (!pactr->FGotoFrame(nfrm, pfSoundInFrame))
			{
			return(fFalse);
			}
		}

    return(fTrue);
}

/****************************************************
 *
 * This routine makes all text boxes go to a specific frame.
 *
 * Parameters:
 *  nfrm - Frame number to go to.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SCEN::_FForceTboxesToFrm(long nfrm)
{
	AssertThis(0);
	AssertIn(nfrm, klwMin, klwMax);

	PTBOX ptbox;
	long itbox;

	for (itbox = 0; itbox < _pglptbox->IvMac(); itbox++)
		{
		_pglptbox->Get(itbox, &ptbox);
		AssertPo(ptbox, 0);

		if (!ptbox->FGotoFrame(nfrm))
			{
			return(fFalse);
			}

		}

    return(fTrue);
}



/****************************************************
 *
 * Hides all text boxes in this scene
 *
 * Parameters:
 *  None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SCEN::HideTboxes(void)
{
	AssertThis(0);

	PTBOX ptbox;
	long iptbox;
	long nfrmStart, nfrmLast;

	for (iptbox = 0; iptbox < _pglptbox->IvMac(); iptbox++)
		{
		_pglptbox->Get(iptbox, &ptbox);
		AssertPo(ptbox, 0);
 		if (ptbox->FGetLifetime(&nfrmStart, &nfrmLast))
			{
			AssertDo(ptbox->FGotoFrame(nfrmStart - 1), "Could not remove a text box");
			}
		}
}

/****************************************************
 *
 * Hides all actors in this scene
 *
 * Parameters:
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SCEN::HideActors(void)
{
	AssertThis(0);

	PACTR pactr;
	long ipactr;

	for (ipactr = 0; ipactr < _pglpactr->IvMac(); ipactr++)
		{
		_pglpactr->Get(ipactr, &pactr);
		AssertPo(pactr, 0);
 		pactr->Hide();
		}
}


/****************************************************
 *
 * Shows all actors in this scene
 *
 * Parameters:
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SCEN::ShowActors(void)
{
	AssertThis(0);

	PACTR pactr;
	long ipactr;

	for (ipactr = 0; ipactr < _pglpactr->IvMac(); ipactr++)
		{
		_pglpactr->Get(ipactr, &pactr);
		AssertPo(pactr, 0);
 		pactr->Show();
		}
}


/****************************************************
 *
 * This routine marks the movie dirty.
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SCEN::_MarkMovieDirty()
{
	AssertThis(0);

	if (_pmvie != pvNil)
		{
		_pmvie->SetDirty();
		}
}

/****************************************************
 *
 * This routine sets the scene to the given movie
 *
 * Parameters:
 *  pmvie - The new parent movie
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SCEN::SetMvie(PMVIE pmvie)
{
	AssertThis(0);
	AssertPo(pmvie, 0);

	_pmvie = pmvie;
}


/****************************************************
 *
 * This routine reads from a chunky file and creates
 * a list of all the tags in the scene.
 *
 * Parameters:
 *  pcfl - Pointer to the chunky file to read from.
 *  cno  - Cno within the chunky file to read.
 *  ptagl - tag list to put tags in
 *
 * Returns:
 *  fFalse if an error occurred, else fTrue
 *
 ****************************************************/
bool SCEN::FAddTagsToTagl(PCFL pcfl, CNO cno, PTAGL ptagl)
{
	AssertPo(pcfl, 0);
	AssertPo(ptagl, 0);

	BLCK blck;
	KID kid;
	long isev;
	PSEV qsev;
	short bo;
	PGG pggsev;
	TAG tag;
	TAG tagBkgd;
	PGL pgltagSrc;
	TAG tagSrc;
	long itagSrc;
	CHID chid;

	tagBkgd.sid = ksidInvalid;

	//
	// Read starting events
	//
	if (!pcfl->FGetKidChidCtg(kctgScen, cno, 1, kctgStartGg, &kid))
		{
		return fFalse;
		}

	if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		{
		return fFalse;
		}

	pggsev = GG::PggRead(&blck, &bo);

	if (pggsev == pvNil)
		{
		return fFalse;
		}

    Assert(pggsev->CbFixed() == size(SEV), "Bad GG read for event");

    //
	// Find all tags in starting events
	//
	for (isev = 0; isev < pggsev->IvMac(); isev++)
		{
		qsev = (PSEV)pggsev->QvFixedGet(isev);

		//
		// Swap byte ordering of entry
		//
		if (bo == kboOther)
			{
            SwapBytesBom((void *)qsev, kbomSev);
			}

        //
		// Find appropriate event types
		//
        switch (qsev->sevt)
			{
		case sevtAddActr:

			pggsev->Get(isev, &chid);
    		if (bo == kboOther)
    			{
                SwapBytesBom((void *)&chid, kbomLong);
    			}

			if (!pcfl->FGetKidChidCtg(kctgScen, cno, chid, kctgActr, &kid))
				{
				ReleasePpo(&pggsev);
				return fFalse;
				}
			else
				{
				bool fError;
                pgltagSrc = ACTR::PgltagFetch(pcfl, kid.cki.cno, &fError);

				if (fError)
					{
					ReleasePpo(&pgltagSrc);
					ReleasePpo(&pggsev);
					return fFalse;
					}

				if (pgltagSrc != pvNil)
					{

                    for (itagSrc = 0; itagSrc < pgltagSrc->IvMac(); itagSrc++)
        				{
              			pgltagSrc->Get(itagSrc, &tagSrc);
						if (!ptagl->FInsertTag(&tagSrc))
               				{
       						ReleasePpo(&pgltagSrc);
							ReleasePpo(&pggsev);
							return fFalse;
							}

        				}

        			ReleasePpo(&pgltagSrc);

					}

				}
			break;

		case sevtSetBkgd:
			pggsev->Get(isev, &tag);
    		if (bo == kboOther)
    			{
                SwapBytesBom((void *)&tag, kbomTag);
    			}

			if (!BKGD::FAddTagsToTagl(&tag, ptagl))
   				{
				ReleasePpo(&pggsev);
				return fFalse;
				}
			tagBkgd = tag;

			break;

		case sevtAddTbox:
			break;			

		case sevtPlaySnd:
		case sevtChngCamera:
		case sevtPause:
		default:
			Bug("Bad event in start event list");
			break;
			}

		}

	ReleasePpo(&pggsev);

	//
	// Read in GG of Frame events
	//
	if (!pcfl->FGetKidChidCtg(kctgScen, cno, 0, kctgFrmGg, &kid))
		{
		return fFalse;
		}

	if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		{
		return fFalse;
		}

	pggsev = GG::PggRead(&blck, &bo);

	if (pggsev == pvNil)
		{
		return fFalse;
		}

    Assert(pggsev->CbFixed() == size(SEV), "Bad GG read for event");

    //
	// Look in all events for tags
	//
	for (isev = 0; isev < pggsev->IvMac(); isev++)
		{
		qsev = (PSEV)pggsev->QvFixedGet(isev);

		//
		// Swap byte ordering of entry
		//
		if (bo == kboOther)
			{
            SwapBytesBom((void *)qsev, kbomSev);
			}

		//
		// Find appropriate event types
		//
        switch (qsev->sevt)
			{
 	    case sevtChngCamera:
			pggsev->Get(isev, &chid);
    		if (bo == kboOther)
    			{
                SwapBytesBom((void *)&chid, kbomLong);
    			}
			if (tagBkgd.sid == ksidInvalid)
				{
				Bug("no background event!?");
				ReleasePpo(&pggsev);
				return fFalse;
				}
			if (!ptagl->FInsertChild(&tagBkgd, chid, kctgCam))
				{
				ReleasePpo(&pggsev);
				return fFalse;
				}
			break;

		case sevtPause:
 			break;

		case sevtPlaySnd:

			PSSE psse;
			long itag;

			psse = SSE::PsseDupFromGg(pggsev, isev, fFalse);
			if (pvNil == psse)
				{
				ReleasePpo(&pggsev);
				return fFalse;
				}
    		if (bo == kboOther)
    			{
                psse->SwapBytes();
    			}

            //
			// Insert the tags in order
			//
			for (itag = 0; itag < psse->ctagc; itag++)
				{
				if (!ptagl->FInsertTag(psse->Ptag(itag)))
					{
					ReleasePpsse(&psse);
					ReleasePpo(&pggsev);
					return fFalse;
					}
				}
			ReleasePpsse(&psse);
			break;

		case sevtAddActr:
		case sevtSetBkgd:
		case sevtAddTbox:
		default:
			Bug("Bad event in frame event list");
			break;
			}

		}

	ReleasePpo(&pggsev);

	return fTrue;
}


/****************************************************
 *
 * This returns the actor in the current scene with
 * the given arid.
 *
 * Parameters:
 *   Arid to look for.
 *
 * Returns:
 *   pvNil if failure, else the actor.
 *
 ****************************************************/
PACTR SCEN::PactrFromArid(long arid)
{
	AssertThis(0);
	Assert(arid != aridNil, "Bad long");

	long iactr;
	PACTR pactr;

	//
	// Search current scene for the actor.
	//
	for (iactr = 0; iactr < _pglpactr->IvMac(); iactr++)
		{
        _pglpactr->Get(iactr, &pactr);

		if (pactr->Arid() == arid)
			{
			return(pactr);
			}

		}
	
	return(pvNil);
}


/****************************************************
 *
 * This routine chops off the rest of the scene.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   fTrue is successful, else fFalse if failure.
 *
 ****************************************************/
bool SCEN::FChopCore()
{
	AssertThis(0);

	PTBOX ptbox;
	PACTR pactr;
	bool fAlive;
	long ipo;
	long nfrmStart, nfrmLast;

	if (_nfrmCur == _nfrmLast)
		{
        return(fTrue);
		}

    //
	// Chop all actors off
	//
	for (ipo = 0; ipo < _pglpactr->IvMac();)
		{
        _pglpactr->Get(ipo, &pactr);
		AssertPo(pactr, 0);
		pactr->DeleteFwdCore(fTrue, &fAlive);

		if (!fAlive)
			{
            RemActrCore(pactr->Arid());
			}
		else
			{
			ipo++;
			}

		}

    //
	// Chop all text boxes off
	//
	for (ipo = 0; ipo < _pglptbox->IvMac();)
		{
        _pglptbox->Get(ipo, &ptbox);
		AssertPo(ptbox, 0);

		if (ptbox->FGetLifetime(&nfrmStart, &nfrmLast) &&
			(nfrmStart > _nfrmCur))
			{
            //
			// Ok if this fails.
			//
			FRemTboxCore(ptbox);
			}
		else
			{
			if (nfrmLast >= _nfrmCur)
				{

				//
				// Here we extend the lifetime of the text box to
				// infinite, cuz we removed the frame with the
				// Hide().
				//
				if (ptbox->FGotoFrame(klwMax - 1))
					{
					ptbox->FShowCore();			
					ptbox->FGotoFrame(_nfrmCur);
					}

				}

			ipo++;
			
			}

		}

    //
	// Remove all scene events from here forward
	//
	for (; _isevFrmLim < _pggsevFrm->IvMac(); )
		{
        _pggsevFrm->Delete(_isevFrmLim);
		}

    //
	// Set new limit
	//
	if (_nfrmLast != _nfrmCur)
		{
		_nfrmLast = _nfrmCur;
		Pmvie()->SetDirty();
		}

	return(fTrue);
}


/****************************************************
 *
 * This routine chops off the rest of the scene and
 * creates an undo object as well.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   fTrue is successful, else fFalse if failure.
 *
 ****************************************************/
bool SCEN::FChop()
{
	PSUNC psunc;
	bool fValid;

	if (_nfrmCur == _nfrmLast)
		{
        return(fTrue);
		}

	psunc = SUNC::PsuncNew();
	if (psunc != pvNil)
		{
		fValid = psunc->FSave(this);
		}
	else
		{
        fValid = fFalse;
		}

    if (!fValid || !Pmvie()->FAddUndo(psunc))
		{
        ReleasePpo(&psunc);
		return(fFalse);
		}

	ReleasePpo(&psunc);

    if (!FChopCore())
		{
		Pmvie()->ClearUndo();
        return(fFalse);
		}

	return(fTrue);
}


/****************************************************
 *
 * This routine chops off the rest of the scene, backwards
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   fTrue is successful, else fFalse if failure.
 *
 ****************************************************/
bool SCEN::FChopBackCore()
{
	AssertThis(0);

	PTBOX ptbox;
	PACTR pactr;
	SEV sev;
	bool fAlive;
	bool fCopyCam;
	long ipo;
	long nfrmStart, nfrmLast;

	if (_nfrmCur == _nfrmFirst)
		{
        return(fTrue);
		}

    //
	// Chop all actors off
	//
	for (ipo = 0; ipo < _pglpactr->IvMac();)
		{
        _pglpactr->Get(ipo, &pactr);
		AssertPo(pactr, 0);
		pactr->DeleteBackCore(&fAlive);
			
		if (!fAlive)
			{
            RemActrCore(pactr->Arid());
			}
		else
			{
			ipo++;
			}

		}

    //
	// Chop all text boxes off
	//
	for (ipo = 0; ipo < _pglptbox->IvMac();)
		{
        _pglptbox->Get(ipo, &ptbox);
		AssertPo(ptbox, 0);

		if (ptbox->FGetLifetime(&nfrmStart, &nfrmLast) &&
			(nfrmLast < _nfrmCur))
			{
            //
			// Ok if this fails.
			//
			FRemTboxCore(ptbox);
			}
		else
			{
			if (nfrmStart < _nfrmCur)
				{
                Assert(ptbox->FIsVisible(), "Bad tbox");
				ptbox->SetStartFrame(_nfrmCur);
				}

			ipo++;
			
			}

		}

    //
	// Remove all scene events from here backward, except camera
	// changes, keep the most recent camera change.
	//
    fCopyCam = fTrue;
	for (; _isevFrmLim > 0; )
		{
        _isevFrmLim--;

		_pggsevFrm->GetFixed(_isevFrmLim, &sev);

        if ((sev.sevt == sevtChngCamera) && (fCopyCam))
			{

            fCopyCam = fFalse;
			sev.nfrm = _nfrmCur;
			_pggsevFrm->PutFixed(_isevFrmLim, &sev);

			}
		else
			{

			if (sev.nfrm == _nfrmCur)
				{
                continue;
				}

			_pggsevFrm->Delete(_isevFrmLim);

			}

		}

    _isevFrmLim = 0;
    for (; _isevFrmLim < _pggsevFrm->IvMac(); _isevFrmLim++)
		{

		_pggsevFrm->GetFixed(_isevFrmLim, &sev);
		if (sev.nfrm != _nfrmCur)
			{
            break;
			}

		}

    //
	// Set new limit
	//
	if (_nfrmFirst != _nfrmCur)
		{
		_nfrmFirst = _nfrmCur;
		Pmvie()->SetDirty();
		}

	//
	// If _psseBkgd got chopped, get rid of it
	//
	if (_psseBkgd != pvNil && _nfrmSseBkgd < _nfrmFirst)
		{
		ReleasePpsse(&_psseBkgd);
		TrashVar(&_nfrmSseBkgd);
		}

	return(fTrue);
}


/****************************************************
 *
 * This routine chops off the rest of the scene, backwards,
 * and creates an undo object as well.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   fTrue is successful, else fFalse if failure.
 *
 ****************************************************/
bool SCEN::FChopBack()
{
	PSUNC psunc;
	bool fValid;

	if (_nfrmCur == _nfrmFirst)
		{
        return(fTrue);
		}

	psunc = SUNC::PsuncNew();
	if (psunc != pvNil)
		{
		fValid = psunc->FSave(this);
		}
	else
		{
        fValid = fFalse;
		}

    if (!fValid || !Pmvie()->FAddUndo(psunc))
		{
        ReleasePpo(&psunc);
		return(fFalse);
		}

	ReleasePpo(&psunc);

    if (!FChopBackCore())
		{
		Pmvie()->ClearUndo();
        return(fFalse);
		}

	return(fTrue);
}


/****************************************************
 *
 * This routine does any startup for playback
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   None.
 *
 ****************************************************/
bool SCEN::FStartPlaying()
{
	AssertThis(0);

	long ipactr, iptbox;
	PACTR pactr;
	PTBOX ptbox;
	PTBXG ptbxg;
	long nfrmFirst, nfrmLast;

	//
	// Make sure all actors have state variables updated
	// (they might not be in the theater right after
	// loading the movie)
	//
	for (ipactr = 0; ipactr < _pglpactr->IvMac(); ipactr++)
		{
		_pglpactr->Get(ipactr, &pactr);
		if (!pactr->FGetLifetime(&nfrmFirst, &nfrmLast))
			return fFalse;
		}

	for (iptbox = 0; iptbox < _pglptbox->IvMac(); iptbox++)
		{
		_pglptbox->Get(iptbox, &ptbox);
		if ((ptbox->FStory() && ptbox->FIsVisible()) ||
			(!ptbox->FStory() && ptbox->FGetLifetime(&nfrmFirst, &nfrmLast) && (nfrmFirst == _nfrmCur)))
			{
			ptbxg = (PTBXG)ptbox->PddgGet(0);
			ptbxg->Scroll(scaNil);
			}
		}

	//
	// Start prerendering at this frame, even if there's
	// no camera view change.
	//
	_DoPrerenderingWork(fTrue);
	return fTrue;
}


/****************************************************
 *
 * This routine cleans up after a playback has stopped.
 *
 * Parameters:
 *   None.
 *
 * Returns:
 *   None.
 *
 ****************************************************/
void SCEN::StopPlaying()
{
	AssertThis(0);

	long itbox;
	PTBOX ptbox;

	_EndPrerendering();		// Stop prerendering

	for (itbox = 0; itbox < _pglptbox->IvMac(); itbox++)
		{
		_pglptbox->Get(itbox, &ptbox);
		AssertPo(ptbox, 0);
		ptbox->CleanDdg();
		}
}


/******************************************************************************
	FTransOnFile
		For a given SCEN chunk on a given CRF, get the scene transition
		state for the scene.
	
	Arguments:
		PCRF pcrf     -- the chunky resource file the SCEN lives on
		CNO cno       -- the CNO of the SCEN chunk
		TRANS *ptrans -- pointer to memory to take the transition setting
	
	Returns: fTrue if it was able to set *ptrans, fFalse if something failed
	
************************************************************ PETED ***********/
bool SCEN::FTransOnFile(PCRF pcrf, CNO cno, TRANS *ptrans)
{
	BLCK blck;
	SCENH scenh;
	PCFL pcfl = pcrf->Pcfl();

	TrashVar(ptrans);

	if (!pcfl->FFind(kctgScen, cno, &blck))
		goto LFail;
	if (!blck.FUnpackData() || blck.Cb() != size(SCENH))
		goto LFail;
	if (!blck.FReadRgb(&scenh, size(SCENH), 0))
		goto LFail;

	*ptrans = scenh.trans;
	return fTrue;
LFail:
	return fFalse;
}


/******************************************************************************
	FSetTransOnFile
		For a given SCEN chunk on a given CRF, set the scene transition
		state for the scene.
	
	Arguments:
		PCRF pcrf   -- the chunky resource file the SCEN lives on
		CNO cno     -- the CNO of the SCEN chunk
		TRANS trans -- the transition state to use
	
	Returns: fTrue if the routine could guarantee that the scene has the
		given transition state.
	
************************************************************ PETED ***********/
bool SCEN::FSetTransOnFile(PCRF pcrf, CNO cno, TRANS trans)
{
	BLCK blck;
	SCENH scenh;
	PCFL pcfl = pcrf->Pcfl();

	if (!pcfl->FFind(kctgScen, cno, &blck))
		goto LFail;
	if (!blck.FUnpackData() || blck.Cb() != size(SCENH))
		goto LFail;
	if (!blck.FReadRgb(&scenh, size(SCENH), 0))
		goto LFail;
	if (scenh.trans != trans)
		{
		scenh.trans = trans;
		if (!pcfl->FPutPv(&scenh, size(SCENH), kctgScen, cno))
			goto LFail;
		}

	return fTrue;
LFail:
	return fFalse;
}


//
//
//
// UNDO STUFF
//
//
//

/****************************************************
 *
 * Public constructor for scene undo objects for name
 * related commands.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PSUNT SUNT::PsuntNew()
{
	PSUNT psunt;
	psunt = NewObj SUNT();
	return(psunt);
}

/****************************************************
 *
 * Destructor for scene naming undo objects
 *
 ****************************************************/
SUNT::~SUNT(void)
{
	AssertBaseThis(0);
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
bool SUNT::FDo(PDOCB pdocb)
{
	AssertThis(0);

	STN stn;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		_pmvie->Pmsq()->FlushMsq();	
        _pmvie->ClearUndo();
   		return(fFalse);
		}

	
	_pmvie->Pscen()->GetName(&stn);
	_pmvie->Pscen()->SetNameCore(&_stn);
	_stn = stn;

	_pmvie->Pmsq()->FlushMsq();

	return(fTrue);
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
bool SUNT::FUndo(PDOCB pdocb)
{
	AssertThis(0);

	return(FDo(pdocb));
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the SUNT
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SUNT::MarkMem(void)
{
	AssertThis(0);
	SUNT_PAR::MarkMem();
}

/***************************************************************************
	Assert the validity of the SUNT.
***************************************************************************/
void SUNT::AssertValid(ulong grf)
{
}
#endif


/****************************************************
 *
 * Public constructor for scene undo objects for sound
 * related commands.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PSUNS SUNS::PsunsNew()
{
	PSUNS psuns;
	psuns = NewObj SUNS();
	return(psuns);
}

/****************************************************
 *
 * Destructor for scene sound undo objects
 *
 ****************************************************/
SUNS::~SUNS(void)
{
	AssertBaseThis(0);
	ReleasePpsse(&_psse);
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
bool SUNS::FDo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	long isevSnd = ivNil;
	bool fFound;
	PSSE psseOld = pvNil;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
        _pmvie->ClearUndo();
   		return(fFalse);
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
        _pmvie->ClearUndo();
   		return(fFalse);
		}

	_pmvie->Pmsq()->FlushMsq();

	// swap the event in the event list (if any) with _psse (if any)

	if (!_pmvie->Pscen()->FGetSnd(_sty, &fFound, &psseOld))
		{
        _pmvie->ClearUndo();
   		return(fFalse);
		}

	if (_psse != pvNil)
		{
		if (fFound)
			{
			_pmvie->Pscen()->RemSndCore(psseOld->sty);
			if (!_pmvie->Pscen()->FAddSndCoreTagc(_psse->fLoop, fFalse,
				_psse->vlm, _psse->sty, _psse->ctagc, _psse->Ptagc(0)))
				{
				ReleasePpsse(&psseOld);
		        _pmvie->ClearUndo();
		   		return(fFalse);
				}
			ReleasePpsse(&_psse);
			_psse = psseOld;
			_sty = psseOld->sty;
			}
		else // no sse to replace, just add this one
			{
			if (!_pmvie->Pscen()->FAddSndCoreTagc(_psse->fLoop, fFalse,
				_psse->vlm, _psse->sty, _psse->ctagc, _psse->Ptagc(0)))
				{
			    _pmvie->ClearUndo();
				return(fFalse);
				}
			ReleasePpsse(&_psse);
			}
		}
    else // _psse is pvNil...remember what's there then nuke the event
		{
		if (!fFound)
			{
			Bug("where's the sound event?");
			return(fFalse);
			}
		else
			{
			if (!FSetSnd(psseOld))
				{
			    _pmvie->ClearUndo();
				return(fFalse);
				}
			ReleasePpsse(&psseOld);
			_pmvie->Pscen()->RemSndCore(_sty);
			}
		}

	_pmvie->Pmsq()->PlayMsq();

	return(fTrue);
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
bool SUNS::FUndo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	return FDo(pdocb);
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the SUNS
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SUNS::MarkMem(void)
{
	AssertThis(0);
	SUNS_PAR::MarkMem();
	if (_psse != pvNil)
		MarkPv(_psse);
}

/***************************************************************************
	Assert the validity of the SUNS.
***************************************************************************/
void SUNS::AssertValid(ulong grf)
{
	SUNS_PAR::AssertValid(grf);
	if (_psse != pvNil)
		{
		AssertPvCb(_psse, size(SSE));
		AssertPvCb(_psse, _psse->Cb());
		Assert(_sty == _psse->sty, "sty's don't match");
		}
}
#endif




/****************************************************
 *
 * Public constructor for scene undo objects for actor
 * related commands.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PSUNA SUNA::PsunaNew()
{
	PSUNA psuna;
	psuna = NewObj SUNA();
	return(psuna);
}

/****************************************************
 *
 * Destructor for scene actor undo objects
 *
 ****************************************************/
SUNA::~SUNA(void)
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
bool SUNA::FDo(PDOCB pdocb)
{
	AssertThis(0);

	PACTR pactr, pactrDup;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
		goto LFail;
		}

	if (_ut == utAdd)
		{

		if (!_pmvie->Pscen()->FAddActrCore(_pactr))
			{
			goto LFail;
			}

		_pmvie->Pscen()->SelectActr(_pactr);

		if (!_pactr->FDup(&pactrDup, fTrue))
			{
			goto LFail;
			}

		pactrDup->SetArid(_pactr->Arid());
		ReleasePpo(&_pactr);
        SetActr(pactrDup);
		
		}
	else if (_ut == utDel)
		{

		_pmvie->Pscen()->RemActrCore(_pactr->Arid());

		}
    else
		{
        Assert(_ut == utRep, "Bad Grf");

		pactr = _pmvie->Pscen()->PactrFromArid(_pactr->Arid());
		if (!pactr->FDup(&pactrDup, fTrue))
			{
			goto LFail;
			}

		if (!_pmvie->Pscen()->FAddActrCore(_pactr))
			{
			goto LFail;
			}

		_pmvie->Pscen()->SelectActr(_pactr);

		pactrDup->SetArid(_pactr->Arid());
		ReleasePpo(&_pactr);
        SetActr(pactrDup);		
		}

	_pmvie->Pmsq()->FlushMsq();
	_pmvie->Pmcc()->UpdateRollCall();

	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();	
	_pmvie->ClearUndo();
	return(fFalse);
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
bool SUNA::FUndo(PDOCB pdocb)
{
	AssertThis(0);

	PACTR pactr, pactrDup;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
		goto LFail;
		}

	if (_ut == utAdd)
		{

		_pmvie->Pscen()->RemActrCore(_pactr->Arid());

		}
	else if (_ut == utDel)
		{

		if (!_pactr->FDup(&pactrDup, fTrue))
			{
			goto LFail;
			}

		pactrDup->SetArid(_pactr->Arid());
	
		if (!_pmvie->Pscen()->FAddActrCore(pactrDup))
			{
			ReleasePpo(&pactrDup);
			goto LFail;
			}

		_pmvie->Pscen()->SelectActr(pactrDup);

		ReleasePpo(&pactrDup);

		}
    else
		{
        Assert(_ut == utRep, "Bad Grf");

		pactr = _pmvie->Pscen()->PactrFromArid(_pactr->Arid());
		if (!pactr->FDup(&pactrDup, fTrue))
			{			
			goto LFail;
			}

		if (!_pmvie->Pscen()->FAddActrCore(_pactr))
			{
			goto LFail;
			}

		_pmvie->Pscen()->SelectActr(_pactr);

		pactrDup->SetArid(_pactr->Arid());
		ReleasePpo(&_pactr);
        SetActr(pactrDup);		
		}

	_pmvie->Pmsq()->FlushMsq();
	_pmvie->Pmcc()->UpdateRollCall();

	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the SUNA
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SUNA::MarkMem(void)
{
	AssertThis(0);
	SUNA_PAR::MarkMem();
	MarkMemObj(_pactr);
}												

/***************************************************************************
	Assert the validity of the SUNA.
***************************************************************************/
void SUNA::AssertValid(ulong grf)
{
	AssertPo(_pactr, 0);
}
#endif





/****************************************************
 *
 * Public constructor for scene undo objects for text box
 * related commands.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PSUNX SUNX::PsunxNew()
{
	PSUNX psunx;
	psunx = NewObj SUNX();
	return(psunx);
}

/****************************************************
 *
 * Destructor for scene text box undo objects
 *
 ****************************************************/
SUNX::~SUNX(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_ptbox);
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
bool SUNX::FDo(PDOCB pdocb)
{
	AssertThis(0);

	PTBOX ptbox;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (_fAdd)
		{

    	if (!_pmvie->Pscen()->FGotoFrm(_nfrmFirst))
    		{
			goto LFail;
    		}

		if (!_pmvie->Pscen()->FAddTboxCore(_ptbox))
			{
			_pmvie->Pscen()->FGotoFrm(_nfrm);
			goto LFail;
			}

		if (_nfrmLast < _pmvie->Pscen()->NfrmLast())
			{
			AssertDo(_ptbox->FGotoFrame(_nfrmLast + 1), "Should never fail");
			_ptbox->HideCore();
			}

		_pmvie->Pscen()->SelectTbox(_ptbox);

		if (_pmvie->Pscen()->FGotoFrm(_nfrm))
			{

    		//
    		// Find the new itbox for this tbox.
    		//
    		for (_itbox = 0; ; _itbox++)
    			{

                ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
    			AssertPo(ptbox, 0);

    			if (ptbox == _ptbox)
    				{
                    break;
    				}

    			}

			}

		}
	else
		{

		if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
			{
			goto LFail;
			}

        //
		// NOTE: ptbox may be different than _ptbox since
		// we may have switched away from the scene.
		//
        ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
		if (!_pmvie->Pscen()->FRemTboxCore(ptbox))
			{
			goto LFail;
			}

		}

	_pmvie->Pmsq()->FlushMsq();

	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);

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
bool SUNX::FUndo(PDOCB pdocb)
{
	AssertThis(0);

	PTBOX ptbox;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (_fAdd)
		{

    	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
    		{
			goto LFail;
    		}

        //
		// NOTE: ptbox may be different than _ptbox since
		// we may have switched away from the scene.
		//
        ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
		if (!_pmvie->Pscen()->FRemTboxCore(ptbox))
			{
            ReleasePpo(&ptbox);
			goto LFail;
			}

		}
    else
		{

    	if (!_pmvie->Pscen()->FGotoFrm(_nfrmFirst))
    		{
			goto LFail;
    		}

		if (!_pmvie->Pscen()->FAddTboxCore(_ptbox))
			{
			goto LFail;
			}

		if (_nfrmLast < _pmvie->Pscen()->NfrmLast())
			{
			AssertDo(_ptbox->FGotoFrame(_nfrmLast + 1), "Should never fail");
			_ptbox->HideCore();
			}
		_pmvie->Pscen()->SelectTbox(_ptbox);

		if (_pmvie->Pscen()->FGotoFrm(_nfrm))
			{

    		//
    		// Find the new itbox for this tbox.
    		//
    		for (_itbox = 0; ; _itbox++)
    			{

                ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
    			AssertPo(ptbox, 0);

    			if (ptbox == _ptbox)
    				{
                    break;
    				}

    			}

			}

		}

	_pmvie->Pmsq()->FlushMsq();

	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);

}
#ifdef DEBUG
/****************************************************
 * Mark memory used by the SUNX
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SUNX::MarkMem(void)
{
	AssertThis(0);
	SUNX_PAR::MarkMem();
	MarkMemObj(_ptbox);
}

/***************************************************************************
	Assert the validity of the SUNX.
***************************************************************************/
void SUNX::AssertValid(ulong grf)
{
	AssertPo(_ptbox, 0);
}
#endif




/****************************************************
 *
 * Public constructor for scene undo objects for transition
 * related commands.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PSUNR SUNR::PsunrNew()
{
	PSUNR psunr;
	psunr = NewObj SUNR();
	return(psunr);
}

/****************************************************
 *
 * Destructor for scene transition undo objects
 *
 ****************************************************/
SUNR::~SUNR(void)
{
	AssertBaseThis(0);
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
bool SUNR::FDo(PDOCB pdocb)
{
	AssertThis(0);

	TRANS trans;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
		goto LFail;
		}

    trans = _pmvie->Pscen()->Trans();
    _pmvie->Pscen()->SetTransitionCore(_trans);
	_trans = trans;

	_pmvie->Pmsq()->FlushMsq();

	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);
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
bool SUNR::FUndo(PDOCB pdocb)
{
	AssertThis(0);
	return(FDo(pdocb));
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the SUNR
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SUNR::MarkMem(void)
{
	AssertThis(0);
	SUNR_PAR::MarkMem();
}

/***************************************************************************
	Assert the validity of the SUNR.
***************************************************************************/
void SUNR::AssertValid(ulong grf)
{
}
#endif






/****************************************************
 *
 * Public constructor for scene undo objects for pause
 * related commands.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PSUNP SUNP::PsunpNew()
{
	PSUNP psunp;
	psunp = NewObj SUNP();
	return(psunp);
}

/****************************************************
 *
 * Destructor for scene pause undo objects
 *
 ****************************************************/
SUNP::~SUNP(void)
{
	AssertBaseThis(0);
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
bool SUNP::FDo(PDOCB pdocb)
{
	AssertThis(0);

	WIT wit = _wit;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FPauseCore(&_wit, &_dts))
		{
		goto LFail;
		}

	_pmvie->Pmcc()->PauseType(wit);
	_pmvie->Pmsq()->FlushMsq();

	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);
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
bool SUNP::FUndo(PDOCB pdocb)
{
	return(FDo(pdocb));
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the SUNP
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SUNP::MarkMem(void)
{
	AssertThis(0);
	SUNP_PAR::MarkMem();
}

/***************************************************************************
	Assert the validity of the SUNP.
***************************************************************************/
void SUNP::AssertValid(ulong grf)
{
}
#endif







/****************************************************
 *
 * Public constructor for scene undo objects for background
 * related commands.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PSUNK SUNK::PsunkNew()
{
	PSUNK psunk;
	psunk = NewObj SUNK();
	return(psunk);
}

/****************************************************
 *
 * Destructor for scene background undo objects
 *
 ****************************************************/
SUNK::~SUNK(void)
{
	AssertBaseThis(0);
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
bool SUNK::FDo(PDOCB pdocb)
{
	AssertThis(0);

	TAG tagOld;
	long icamOld, icam;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (_fSetBkgd)
		{

        if (_pmvie->Pscen()->Pbkgd() != pvNil)
			{
			icam = _pmvie->Pscen()->Pbkgd()->Icam();
			}

		if (!_pmvie->Pscen()->FSetBkgdCore(&_tag, &tagOld))
			{
			goto LFail;
			}

		if (!_pmvie->Pscen()->FChangeCamCore(_icam, &icamOld))
			{
			goto LFail;
			}

        _tag = tagOld;
		_icam = icam;

		}
	else
		{

		if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
    		{
			goto LFail;
    		}

		if (!_pmvie->Pscen()->FChangeCamCore(_icam, &icamOld))
			{
			goto LFail;
			}

        _icam = icamOld;

		}

	_pmvie->Pmsq()->FlushMsq();

	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);
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
bool SUNK::FUndo(PDOCB pdocb)
{
	AssertThis(0);
	return(FDo(pdocb));
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the SUNK
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SUNK::MarkMem(void)
{
	AssertThis(0);
	SUNK_PAR::MarkMem();
}

/***************************************************************************
	Assert the validity of the SUNK.
***************************************************************************/
void SUNK::AssertValid(ulong grf)
{
}
#endif








/****************************************************
 *
 * Public constructor for scene undo objects for background
 * chop commands.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PSUNC SUNC::PsuncNew()
{
	PSUNC psunc;
	psunc = NewObj SUNC();
	return(psunc);
}

/****************************************************
 *
 * Destructor for scene pause undo objects
 *
 ****************************************************/
SUNC::~SUNC(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pcrf);
}


/****************************************************
 *
 * This function saves away a copy of the scene.
 *
 * Parameters:
 *  pscen - The scene to save away.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool SUNC::FSave(PSCEN pscen)
{
	PCFL pcfl;
	bool fRet;

	pcfl = CFL::PcflCreateTemp();
	if (pcfl == pvNil)
  		{
		return(fFalse);
		}

	_pcrf = CRF::PcrfNew(pcfl, 0);
	if (_pcrf == pvNil)
		{
        ReleasePpo(&pcfl);
		return(fFalse);
		}
    ReleasePpo(&pcfl);

	vpappb->BeginLongOp();
	fRet = pscen->FWrite(_pcrf, &_cno);
	vpappb->EndLongOp();

	return(fRet);
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
bool SUNC::FDo(PDOCB pdocb)
{
	AssertThis(0);

	PSCEN pscen;
	PSCEN pscenSave;

	vpappb->BeginLongOp();

    pscen = SCEN::PscenRead(_pmvie, _pcrf, _cno);

	if (pscen == pvNil)
		{
		goto LFail;
		}

	if (!pscen->FPlayStartEvents())
		{
		SCEN::Close(&pscen);
		goto LFail;
		}

	if (!_pmvie->FSwitchScen(_iscen))
		{
		SCEN::Close(&pscen);
		goto LFail;
		}

    pscenSave = _pmvie->Pscen();
	pscenSave->AddRef();

    if (!_pmvie->FInsScenCore(_iscen, pscen))
		{
		ReleasePpo(&pscenSave);
		SCEN::Close(&pscen);
		goto LFail;
		}

	SCEN::Close(&pscen);

    _pcrf->Pcfl()->Delete(kctgScen, _cno);
	pscenSave->SetNfrmCur(pscenSave->NfrmFirst() - 1);
	if (!pscenSave->FWrite(_pcrf, &_cno))
		{
		_pmvie->FRemScenCore(_iscen + 1);
		SCEN::Close(&pscenSave);
		goto LFail;
		}

	SCEN::Close(&pscenSave);

    if (!_pmvie->FRemScenCore(_iscen + 1))
		{
		goto LFail;
		}

	//
	// We don't care if we can't switch, everything was restored.
	//	
   	if (_pmvie->FSwitchScen(_iscen))
		{
		_pmvie->Pscen()->FGotoFrm(_nfrm);
		_pmvie->InvalViewsAndScb();
		}

	vpappb->EndLongOp();

	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	vpappb->EndLongOp();
	return(fFalse);
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
bool SUNC::FUndo(PDOCB pdocb)
{
	AssertThis(0);
	return(FDo(pdocb));
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the SUNC
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void SUNC::MarkMem(void)
{
	AssertThis(0);
	SUNC_PAR::MarkMem();
	MarkMemObj(_pcrf);
}

/***************************************************************************
	Assert the validity of the SUNC.
***************************************************************************/
void SUNC::AssertValid(ulong grf)
{
	AssertNilOrPo(_pcrf, grf);
}
#endif

/***************************************************************************
	Static function to allocate a SSE with room for ctag TAGs.
***************************************************************************/
PSSE SSE::PsseNew(long ctag)
{
	Assert(ctag > 0, 0);

	PSSE psse;

	if (!FAllocPv((void **)&psse, _Cb(ctag), fmemNil, mprNormal))
		return pvNil;
	return psse;
}


/***************************************************************************
	Creates a new SSE
***************************************************************************/
PSSE SSE::PsseNew(long vlm, long sty, bool fLoop, long ctagc, TAGC *prgtagc)
{
	Assert(ctagc > 0, 0);
	AssertPvCb(prgtagc, LwMul(ctagc, size(TAGC)));
	PSSE psse;
	long itagc;

	psse = PsseNew(ctagc);
	if (pvNil == psse)
		return pvNil;
	psse->vlm = vlm;
	psse->sty = sty;
	psse->fLoop = fLoop;
	psse->ctagc = ctagc;
	for (itagc = 0; itagc < ctagc; itagc++)
		{
		*psse->Ptagc(itagc) = prgtagc[itagc];
		TAGM::DupTag(psse->Ptag(itagc));
		}
	return psse;
}


/***************************************************************************
	Properly cleans up and frees a SSE
***************************************************************************/
void ReleasePpsse(PSSE *ppsse)
{
	AssertVarMem(ppsse);

	if (*ppsse == pvNil)
		return;

	PSSE psse = *ppsse;
	long itagc;

	AssertIn(psse->ctagc, 0, 1000); // sanity check on ctagc
	AssertIn(psse->sty, styNil, styLim);


	for (itagc = 0; itagc < psse->ctagc; itagc++)
		{
		if (psse->Ptag(itagc)->pcrf != pvNil)
			TAGM::CloseTag(psse->Ptag(itagc));
		}

	FreePpv((void **)ppsse);
}


/***************************************************************************
	Static function to allocate and read a SSE from a GG.  This is tricky
	because I can't do a pgg->Get() since the SSE is variable-sized, and
	I need to do QvGet twice since I'm allocating memory in this function.
***************************************************************************/
PSSE SSE::PsseDupFromGg(PGG pgg, long iv, bool fDupTags)
{
	AssertPo(pgg, 0);
	AssertIn(iv, 0, pgg->IvMac());
	Assert(pgg->Cb(iv) >= size(SSE), "variable part too small");

	long ctagc;
	PSSE psse;
	long itagc;

	ctagc = ((PSSE)pgg->QvGet(iv))->ctagc;

	psse = PsseNew(ctagc);
	CopyPb(pgg->QvGet(iv), psse, _Cb(ctagc));

	for (itagc = 0; itagc < psse->ctagc; itagc++)
		{
		if (fDupTags)
			{
			if (psse->Ptag(itagc)->sid == ksidUseCrf)
				{
				AssertPo(psse->Ptag(itagc)->pcrf, 0);
				TAGM::DupTag(psse->Ptag(itagc));
				}
			}
		else
			{
			// Clear the crf on read, since the caller isn't having us dupe the tag
			psse->Ptag(itagc)->pcrf = pvNil;
			}
		}

	return psse;
}


/***************************************************************************
	Returns a PSSE just like this SSE except with ptag & chid added
***************************************************************************/
PSSE SSE::PsseAddTagChid(PTAG ptag, long chid)
{
	AssertVarMem(ptag);

	PSSE psseNew;
	TAGC tagc;

	tagc.tag = *ptag;
	tagc.chid = chid;

	psseNew = SSE::PsseNew(ctagc + 1);
	if (pvNil == psseNew)
		return pvNil;

	CopyPb(this, psseNew, Cb());
	*psseNew->Ptagc(psseNew->ctagc) = tagc;
	psseNew->ctagc++;
	TAGM::DupTag(ptag);
	return psseNew;
}


/***************************************************************************
	Return a duplicate of this SSE
***************************************************************************/
PSSE SSE::PsseDup(void)
{
	PSSE psse;
	long itagc;

	if (!FAllocPv((void **)&psse, size(SSE) + LwMul(ctagc, size(TAGC)),
		fmemNil, mprNormal))
		{
		return pvNil;
		}
	CopyPb(this, psse, size(SSE) + LwMul(ctagc, size(TAGC)));
	for (itagc = 0; itagc < psse->ctagc; itagc++)
		TAGM::DupTag(psse->Ptag(itagc));
	return psse;
}


/***************************************************************************
	Play all sounds in this SSE	-> Enqueue the sounds in the SSE
***************************************************************************/
void SSE::PlayAllSounds(PMVIE pmvie, ulong dtsStart)
{
	PMSND pmsnd;
	long itag;
	long tool = fLoop ? toolLooper : toolSounder;

	for (itag = 0; itag < ctagc; itag++)
		{
		if (Ptag(itag)->sid == ksidUseCrf)
			{
			if (!pmvie->FResolveSndTag(Ptag(itag), *Pchid(itag)))
				continue;
			}
		pmsnd = (PMSND)vptagm->PbacoFetch(Ptag(itag), MSND::FReadMsnd);
		if (pvNil == pmsnd)
			return;

		// Only queue if it's not the first sound; only start at dtsStart for first sound
		pmvie->Pmsq()->FEnqueue(pmsnd, 0, fLoop, (itag != 0), vlm, pmsnd->Spr(tool),
			fFalse, (itag == 0 ? dtsStart : 0));

		ReleasePpo(&pmsnd);
		}
}

