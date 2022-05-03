/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	msnd.cpp: Movie Sound class

	Primary Authors: *****, *****
	Status:  Reviewed

	A MSND (movie sound) owns a MIDI or WAVE child chunk, and also
	specifies what sound type (sty) this sound is, and the default
	volume for the sound.
	
	Here's how the chunks look:

	MSND
	 |
	 +---MIDI or WAVE (chid 0) // actual sound data

	An MSND chunk with no child is a "no sound" or silent sound.
	An MSND chunk with _fInvalid set requires no child.

***************************************************************************/
#include "soc.h"
#include "audioman.h"

ASSERTNAME

RTCLASS(MSND)
RTCLASS(MSQ)

BEGIN_CMD_MAP(MSQ, CMH)
	ON_CID_ME(cidAlarm, &MSQ::FCmdAlarm, pvNil)
END_CMD_MAP_NIL()

// default sound import format
const WORD knSamplesPerSec = 11025;
const WORD knAvgBytesPerSec = 11025;
const WORD kwBitsPerSample = 8;
const WORD knChannels = 1;
const WORD knBlockAlign = 1;

/***************************************************************************

	A PFNRPO to read a MSND from a file

***************************************************************************/
bool MSND::FReadMsnd(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, 0);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);

	MSND *pmsnd;

	*pcb = size(MSND); // estimate MSND size
	if (pvNil == ppbaco)
		return fTrue;

	pmsnd = NewObj MSND();
	if (pvNil == pmsnd || !pmsnd->_FInit(pcrf->Pcfl(), ctg, cno))
		{
		TrashVar(ppbaco);
		TrashVar(pcb);
		ReleasePpo(&pmsnd);
		return fFalse;
		}

	pmsnd->_prca = pcrf;
	AssertPo(pmsnd, 0);
	*ppbaco = pmsnd;
	return fTrue;
}


/***************************************************************************

	Retrieve information contained in the msnd chunk

***************************************************************************/
bool MSND::FGetMsndInfo(PCFL pcfl, CTG ctg, CNO cno, bool *pfInvalid, long *psty, long *pvlm)
{
	AssertPo(pcfl, 0);

	PMSND pmsnd;
	pmsnd = NewObj MSND();
	if (pvNil == pmsnd)
		return pvNil;

	if (!pmsnd->_FInit(pcfl, ctg, cno))
		{
		ReleasePpo(&pmsnd);
		return fFalse;
		}

	if (pvNil != pfInvalid)
		*pfInvalid = pmsnd->_fInvalid;
	if (pvNil != psty)
		*psty = pmsnd->_sty;
	if (pvNil != pvlm)
		*pvlm = pmsnd->_vlm;
	ReleasePpo(&pmsnd);
	return fTrue;
}


/***************************************************************************

	Init a MSND from the given chunk of the given CFL

***************************************************************************/
bool MSND::_FInit(PCFL pcfl, CTG ctg, CNO cno)
{
	AssertBaseThis(0);
	AssertPo(pcfl, 0);

	BLCK blck;
	MSNDF msndf;
	KID kid;

	if (!pcfl->FFind(ctg, cno, &blck) || !blck.FUnpackData())
		goto LFail;
	if (blck.Cb() > size(MSNDF))
		goto LFail;
	if (!blck.FReadRgb(&msndf, size(MSNDF), 0))
		goto LFail;
	if (kboCur != msndf.bo)
		SwapBytesBom(&msndf, kbomBkgdf);
	Assert(kboCur == msndf.bo, "bad MSNDF");

	if (!pcfl->FGetName(ctg, cno, &_stn))
		return fFalse;

	_sty = msndf.sty;
	_vlm = msndf.vlmDefault;
	_fInvalid = msndf.fInvalid;
	if (_fInvalid)
		return fTrue;

	// If there is a SND child, it is not a "no sound"
	if (pcfl->FGetKidChid(ctg, cno, kchidSnd, &kid))
		{
		_cnoSnd = kid.cki.cno;
		_ctgSnd = kid.cki.ctg;
		_fNoSound = fFalse;
		}
	else
		_fNoSound = fTrue;

	return fTrue;
LFail:
	return fFalse;
}


/***************************************************************************

	Write an MSND MIDI chunk to file *pcfl
	ie, write the MSND chunk, its name, and the midi child

***************************************************************************/
bool MSND::FWriteMidi(PCFL pcflDest, PMIDS pmids, STN *pstnName, CNO *pcno)
{
   	AssertPo(pcflDest, 0);
	AssertPo(pmids, 0);
	AssertVarMem(pstnName);
	AssertVarMem(pcno);

	MSNDF msndf;
	BLCK blck;
	CNO cno;

	msndf.bo = kboCur;
	msndf.osk = koskCur;
	msndf.sty = styMidi;
	msndf.vlmDefault = kvlmFull;
	msndf.fInvalid = fFalse;

	// Create the msnd chunk
	if (!pcflDest->FAddPv(&msndf, size(MSNDF), kctgMsnd, pcno))
		return fFalse;
		
	// Create the midi chunk as a child of the msnd chunk	
	if (!pcflDest->FAddChild(kctgMsnd, *pcno, kchidSnd, pmids->CbOnFile(), kctgMidi, &cno, &blck))
		goto LFail;

	if (!pmids->FWrite(&blck))
		goto LFail;

	if (!pcflDest->FSetName(kctgMsnd, *pcno, pstnName))
		goto LFail;

	return fTrue;

LFail:
	pcflDest->Delete(kctgMsnd, *pcno);  // Deletes the midi chunk also
	return fFalse;
}

/***************************************************************************

	Write an MSND Wave file to a file
	ie, write the MSND chunk, its name, and the midi child

***************************************************************************/
bool MSND::FWriteWave(PFIL pfilSrc, PCFL pcflDest, long sty, STN *pstnName, CNO *pcno)
{
   	AssertPo(pfilSrc, 0);
	AssertIn(sty, 0, styLim);
	AssertVarMem(pstnName);
	AssertVarMem(pcno);

	MSNDF msndf;
	CNO cno;
	FLO floSrc;
	FLO floDest;

	msndf.bo = kboCur;
	msndf.osk = koskCur;
	msndf.sty = sty;
	msndf.vlmDefault = kvlmFull;
	msndf.fInvalid = fFalse;

	floSrc.pfil = pfilSrc;
	floSrc.cb = pfilSrc->FpMac();
	floSrc.fp = 0;

	// Create the msnd chunk
	if (!pcflDest->FAddPv(&msndf, size(MSNDF), kctgMsnd, pcno))
		return fFalse;
	
	// Create the wave chunk as a child of the msnd chunk	
	if (!pcflDest->FAddChild(kctgMsnd, *pcno, kchidSnd, floSrc.cb, kctgWave, &cno))
		goto LFail;

	if (!pcflDest->FFindFlo(kctgWave, cno, &floDest))
		goto LFail;

	if (!floSrc.FCopy(&floDest))	
		goto LFail;

	if (!pcflDest->FSetName(kctgMsnd, *pcno, pstnName))
		goto LFail;

	return fTrue;

LFail:
	pcflDest->Delete(kctgMsnd, *pcno);  // Deletes the wave chunk also
	return fFalse;
}


/***************************************************************************

	Copy the midi file to a chunk in the current movie
	The *pcno is returned

***************************************************************************/
bool MSND::FCopyMidi(PFIL pfilSrc, PCFL pcflDest, CNO *pcno, PSTN pstn)
{
	AssertPo(pfilSrc, 0);
	AssertNilOrPo(pstn, 0);

	PMIDS pmids = pvNil;
 	FNI fniSrc;
 	STN stnName;

 	pfilSrc->GetFni(&fniSrc);
	if (pvNil == pstn)
		fniSrc.GetLeaf(&stnName);
	else
		stnName = *pstn;

	pmids = MIDS::PmidsReadNative(&fniSrc);
	if (pmids == pvNil)
		{
		PushErc(ercSocBadSoundFile);
		goto LFail;
		}

	// Create the chunk & write it to this movie
	// Adopt it later as a child of kctgMvie
	if (!MSND::FWriteMidi(pcflDest, pmids, &stnName, pcno))
		goto LFail;

	ReleasePpo(&pmids);
	return fTrue;

LFail:
	ReleasePpo(&pmids);
	return fFalse;
}


/***************************************************************************

	Copy the wave file to a chunk in the current movie

***************************************************************************/
bool MSND::FCopyWave(PFIL pfilSrc, PCFL pcflDest, long sty, CNO *pcno, PSTN pstn)
{
	AssertPo(pfilSrc, 0);
	AssertPo(pcflDest, 0);
	AssertIn(sty, 0, styLim);
	AssertVarMem(pcno);
	Assert(sty != styMidi, "Illegal sty argument");
	AssertNilOrPo(pstn, 0);

	FNI fniSrc;			
	STN stnName;		// sound name
	STN stn;			// src file path name
	WAVEFORMATEX wfxSrc;
	LPSOUND psnd = pvNil;
	LPSOUND psndTemp = pvNil;
	FIL *pfilNew = pvNil;		
	STN stnNew;			
	FNI fniNew;			
	WAVEFORMATEX *pwfxDst = pvNil;
	DWORD cbwfx;			// size of waveformatex needed
	DWORD csampSrc;			// number of source samples
	DWORD cbSrc;			// size of source in bytes
	LPBYTE pbSrc = pvNil;	// src data buffer
	DWORD cbDst;			// size of dest in bytes
	LPBYTE pbDst = pvNil;	// dst data buffer
	HACMSTREAM hacmstream = pvNil;
	ACMSTREAMHEADER acmhdr;
	DWORD dwTag;
	DWORD dwLength;
	long cbOriginalFile = 0;
	long cbCompressedFile = 0;
	FP fpNew;

	pfilSrc->GetFni(&fniSrc);
	if (pvNil == pstn)
		fniSrc.GetLeaf(&stnName);
	else
		stnName = *pstn;
	fniSrc.GetStnPath(&stn);

	if (!fniNew.FGetTemp())			
		goto LFail;

	// get the original file size
	if (pvNil == (pfilNew = FIL::PfilOpen(&fniSrc)))
		goto LFailPushError;
	cbOriginalFile = pfilNew->FpMac();
	ReleasePpo(&pfilNew);

	// open the file as a pSound
	if (FAILED(AllocSoundFromFile(&psnd, stn.Psz(),  0, fTrue, pvNil)))
		goto LFailPushError;
	if (FAILED(psnd->GetFormat((LPWAVEFORMATEX)&wfxSrc, size(WAVEFORMATEX))))
		goto LFailPushError;
	
	// if the file format is not 11M8, then create a converter to 11M8
	if ((wfxSrc.nSamplesPerSec != knSamplesPerSec) ||
		(wfxSrc.wBitsPerSample != kwBitsPerSample )||
		(wfxSrc.nChannels != knChannels ) )
		{
		// Convert the wave import to 3mm's 11M8
		wfxSrc.nSamplesPerSec = knSamplesPerSec ;
		wfxSrc.nAvgBytesPerSec = knAvgBytesPerSec ;
		wfxSrc.wBitsPerSample = kwBitsPerSample;
		wfxSrc.nChannels = knChannels ;
		wfxSrc.nBlockAlign = knBlockAlign ;
		if (FAILED(AllocConvertFilter(&psndTemp, psnd, (LPWAVEFORMATEX)&wfxSrc)))
			goto LFailPushError;

		ReleasePpo(&psnd);
		psnd = psndTemp;
		psndTemp = pvNil;
		}

	// now figure out how many total samples there are
	csampSrc = psnd->GetSamples();
	if (csampSrc == -1)			// Don't allow infinite sources to be written
		goto LFailPushError;
	cbSrc = csampSrc * (wfxSrc.wBitsPerSample>>3) * wfxSrc.nChannels;

	if (FAILED(psnd->SetMode(TRUE,TRUE)))	// Activate the Sound Audio Stream
		goto LFailPushError;

	// allocate a buffer big enough to hold them all
	if (!FAllocPv((LPVOID *)&pbSrc, cbSrc, fmemNil, mprNormal))
		goto LFail;

	// get all of the samples
	if (FAILED(psnd->GetSampleData(pbSrc, 0, &csampSrc, NULL)))
		goto LFailPushError;

	// -------------------------------------
	// inplace compress the samples to ADPCM

	// figure out the correct size for dest waveformatex (so we get the codec info)
	if (acmMetrics(NULL, ACM_METRIC_MAX_SIZE_FORMAT, &cbwfx))
		goto LFailPushError;

	// allocate a buffer of this size
	if (!FAllocPv((LPVOID *)&pwfxDst, cbwfx, fmemNil, mprNormal))
		goto LFail;

	// allocate a destination buffer big enough to hold them all
	cbDst = cbSrc;
	if (!FAllocPv((LPVOID *)&pbDst, cbDst, fmemNil, mprNormal))
		goto LFail;

	// set up the dest wfx
	pwfxDst->wFormatTag = WAVE_FORMAT_ADPCM;
	pwfxDst->nChannels = knChannels;
	pwfxDst->nSamplesPerSec = knSamplesPerSec;
	// pwfxDst->wBitsPerSample, pwfxDst->nAvgBytesPerSec, and
	// pwfxDst->nBlockAlign  are calced by the acmFormatSuggest()
	pwfxDst->cbSize = (WORD)cbwfx - sizeof(WAVEFORMATEX);
	
	// fill in the coeffiecients in the destination WFX by calling acmFormatSuggest
	if (acmFormatSuggest(NULL, &wfxSrc, pwfxDst, cbwfx, ACM_FORMATSUGGESTF_WFORMATTAG |
						ACM_FORMATSUGGESTF_NCHANNELS | ACM_FORMATSUGGESTF_NSAMPLESPERSEC))
		goto LFailPushError;

	// open convert stream
	if (acmStreamOpen(&hacmstream, NULL, &wfxSrc, pwfxDst, NULL, 0, 0, ACM_STREAMOPENF_NONREALTIME))
		goto LFailPushError;

	acmhdr.cbStruct = sizeof(ACMSTREAMHEADER);
	acmhdr.fdwStatus = 0;
	acmhdr.dwUser = 0;
	acmhdr.pbSrc = pbSrc;
	acmhdr.cbSrcLength = cbSrc;
	acmhdr.cbSrcLengthUsed = 0;
	acmhdr.dwSrcUser = 0;
	acmhdr.pbDst = pbDst;
	acmhdr.cbDstLength = cbDst;
	acmhdr.cbDstLengthUsed = 0;
	acmhdr.dwDstUser = 0;
	
	// prep acm convert header
	if (acmStreamPrepareHeader(hacmstream, &acmhdr, NULL))
		goto LFailPushError;

	// now convert
	if (acmStreamConvert(hacmstream, &acmhdr, ACM_STREAMCONVERTF_END))
		goto LFailPushError;

	// unprep header
	if (acmStreamUnprepareHeader(hacmstream, &acmhdr, NULL))
		goto LFailPushError;

	// close stream
	acmStreamClose(hacmstream, NULL);
	hacmstream = pvNil;

	// OK, now we have the sound file converted in memory, write out the data file...
	// open file
	pfilNew = FIL::PfilCreate(&fniNew);
	if (pvNil == pfilNew)
		goto LFail;
	fpNew = 0;

	// write out 'riff' header
	dwTag = RIFF_TAG;
	if (!pfilNew->FWriteRgbSeq(&dwTag, sizeof(dwTag), &fpNew))
		goto LFail;
	cbCompressedFile = sizeof(RIFF) + pwfxDst->cbSize + 12 - 8 + acmhdr.cbDstLengthUsed;	// +12 is for fact chunk chunk, -8 for is riff chunk
	dwLength = cbCompressedFile;
	if (!pfilNew->FWriteRgbSeq(&dwLength, sizeof(dwLength), &fpNew))
		goto LFail;

	// write WAVE tag
	dwTag = WAVE_TAG;
	if (!pfilNew->FWriteRgbSeq(&dwTag, sizeof(dwTag), &fpNew))
		goto LFail;

	// write out 'fmt '
	dwTag = FMT__TAG;
	if (!pfilNew->FWriteRgbSeq(&dwTag, sizeof(dwTag), &fpNew))
		goto LFail;
	dwLength = sizeof(WAVEFORMATEX) + pwfxDst->cbSize;
	if (!pfilNew->FWriteRgbSeq(&dwLength, sizeof(dwLength), &fpNew))
		goto LFail;
	if (!pfilNew->FWriteRgbSeq(pwfxDst, dwLength, &fpNew))
		goto LFail;

	// write out 'fact'
	dwTag = FACT_TAG;
	if (!pfilNew->FWriteRgbSeq(&dwTag, sizeof(dwTag), &fpNew))
		goto LFail;
	dwLength = sizeof(DWORD);
	if (!pfilNew->FWriteRgbSeq(&dwLength, sizeof(dwLength), &fpNew))
		goto LFail;
	dwLength = csampSrc;
	if (!pfilNew->FWriteRgbSeq(&dwLength, sizeof(dwLength), &fpNew))
		goto LFail;
	
	// write out 'data'
	dwTag = DATA_TAG;
	if (!pfilNew->FWriteRgbSeq(&dwTag, sizeof(dwTag), &fpNew))
		goto LFail;
	dwLength = acmhdr.cbDstLengthUsed;
	if (!pfilNew->FWriteRgbSeq(&dwLength, sizeof(dwLength), &fpNew))
		goto LFail;
	if (!pfilNew->FWriteRgbSeq(pbDst, dwLength, &fpNew))
		goto LFail;

	// clean up conversion
	FreePpv((LPVOID *)&pwfxDst);// free up destination waveformat
	FreePpv((LPVOID *)&pbSrc);	// free convert src buffer
	FreePpv((LPVOID *)&pbDst);	// free convert dest buffer
	ReleasePpo(&pfilNew);		// release open file
	if (psnd)
	 	psnd->SetMode(FALSE,TRUE);
	ReleasePpo(&psnd);			// release sound

	// -------------------------------------------------------------------
	// Copy the original or converted file to a chunk in the current movie
	
	// if the compressed file is larger then the original
	if (cbCompressedFile > cbOriginalFile)
		{
		// then just use original file
		if (pvNil == (pfilNew = FIL::PfilOpen(&fniSrc)))
			goto LFail;
		}
	else
		{
		// then we use the newly compressed file
		if (pvNil == (pfilNew = FIL::PfilOpen(&fniNew)))
			goto LFail;
		}

	if (!MSND::FWriteWave(pfilNew, pcflDest, sty, &stnName, pcno))
		goto LFail;

	ReleasePpo(&pfilNew);
	if (tYes == fniNew.TExists())
		fniNew.FDelete();
	return fTrue;

LFailPushError:
	PushErc(ercSocBadSoundFile);
LFail:
	if (hacmstream)
		{
		acmStreamClose(hacmstream, NULL);
		hacmstream = pvNil;
		}
	if (psnd)
 		psnd->SetMode(FALSE,TRUE);

	FreePpv((LPVOID *)&pwfxDst);// free dest waveformatex struct
	FreePpv((LPVOID *)&pbSrc);	// free convert src buffer
	FreePpv((LPVOID *)&pbDst);	// free convert dest buffer
	ReleasePpo(&pfilNew);
	ReleasePpo(&psnd);
	if (tYes == fniNew.TExists())
		fniNew.FDelete();
	return fFalse;
}


/***************************************************************************

	Invalidate a sound

***************************************************************************/
bool MSND::FInvalidate(void)
{
	AssertThis(0);
	
	KID kid;
	MSNDF msndf;

	// Invalidate the msnd on file
	if (!Pcrf()->Pcfl()->FGetKidChid(kctgMsnd, Cno(), kchidSnd, &kid))
		return fFalse;
	msndf.bo = kboCur;
	msndf.osk = koskCur;
	msndf.sty = _sty;
	msndf.vlmDefault = _vlm;
	msndf.fInvalid = fTrue;
	if (!Pcrf()->Pcfl()->FPutPv(&msndf, size(MSNDF), Ctg(), Cno()))
		return fFalse;
	Pcrf()->Pcfl()->DeleteChild(Ctg(), Cno(), kid.cki.ctg, kid.cki.cno);

	// Invalidate the cache representation
	_fInvalid = fTrue;
    return fTrue;
}

/***************************************************************************

	Clean up and delete this movie sound

***************************************************************************/
MSND::~MSND(void)
{
	AssertBaseThis(0);
}

/***************************************************************************

	Return the sqn for an msnd attached to an actor object of
	id == objid
	Static function

***************************************************************************/
long MSND::SqnActr(long sty, long objid)
{
	AssertIn(sty, 0, styLim);

	long sqnsty = sty << ksqnStyShift;
	return (sqnActr | sqnsty | SwLow(objid));
}

/***************************************************************************

	Return the sqn for an msnd attached to an actor object of
	id == objid
	Static function

***************************************************************************/
long MSND::SqnBkgd(long sty, long objid)
{
	long sqnsty = sty << ksqnStyShift;
	return (sqnBkgd | sqnsty | SwLow(objid));
}


/***************************************************************************

	Return the priority for a tool,sty combination

***************************************************************************/
long MSND::Spr(long tool)
{
	AssertThis(0);
	Assert(tool == toolMatcher ||
		   tool == toolSounder ||
		   tool == toolLooper, "Invalid tool");

	switch(_sty)
		{
	case styMidi:
		Assert(tool != toolMatcher, "No midi motion matching");
		return 1;
		break;
	case stySpeech:
		if (tool == toolSounder)
			return 1;
		if (tool == toolMatcher)
			return 5;
		return 3;
		break;
	case stySfx:
		if (tool == toolSounder)
			return 2;
		if (tool == toolMatcher)
			return 6;
		return 4;
	default:
		Assert(0, "Invalid sty in MSND: Spr");
		}
	return 0;
}


/***************************************************************************

	Play this sound

***************************************************************************/
void MSND::Play(long objID, bool fLoop, bool fQueue, long vlm,
	long spr, bool fActr, ulong dtsStart)
{
	AssertThis(0);

	long cactRepeat;
	long sqn; // sound queue
	long scl; // sound class
	long sii;

	static long _siiLastMidi;
	static CTG _ctgLastMidi;
	static CNO _cnoLastMidi;

	if (_fInvalid)
		return;

	sqn = fActr ? SqnActr(objID) : SqnBkgd(objID);
	cactRepeat = fLoop ? klwMax : 1;
	scl = Scl(fLoop);

	if (_sty == styMidi && _ctgSnd == _ctgLastMidi &&
		_cnoSnd == _cnoLastMidi && vpsndm->FPlaying(_siiLastMidi))
		{
		// Don't restart midi if the same sound is still playing
		return;
		}

	if (!fQueue || _fNoSound)
		vpsndm->StopAll(sqn, sclNil);
	
	if (!_fNoSound)
		{
		sii = vpsndm->SiiPlay(_prca, _ctgSnd, _cnoSnd, sqn, vlm, cactRepeat,
			dtsStart, spr, scl);
		}
	
	if (_sty == styMidi)
		{
		_siiLastMidi = sii;
		_ctgLastMidi = _ctgSnd;
		_cnoLastMidi = _cnoSnd;
		}

}

/***************************************************************************

	New MSQ

***************************************************************************/
PMSQ MSQ::PmsqNew(void)
{
	PMSQ pmsq;
	if (pvNil == (pmsq = NewObj MSQ(khidMsq)))
		return pvNil;
		
	if (pvNil == (pmsq->_pglsqe = GL::PglNew(size(SQE), kcsqeGrow)))
		{
		ReleasePpo(&pmsq);
		return pvNil;
		}
	
	if (pvNil == (pmsq->_pclok = NewObj CLOK(khidMsqClock)))
		{
		ReleasePpo(&pmsq);
		return pvNil;
		}

	pmsq->_dtim = kdtim2Msq;
	return pmsq;
}


/***************************************************************************

	Enqueue a sound	in the MSQ.  Overwrites sounds of the same type.

***************************************************************************/
bool MSQ::FEnqueue(PMSND pmsnd, long objID, bool fLoop, bool fQueue,
		long vlm, long spr, bool fActr, ulong dtsStart, bool fLowPri)
{
	AssertThis(0);
	AssertPo(pmsnd, 0);

	SQE sqe;
	SQE *psqe;
	long sqn;
	long sqnT;
	long isqe;

	if (_dtim == kdtimOffMsq)
		return fTrue;

	sqn = fActr ? MSND::SqnActr(pmsnd->Sty(), objID) : MSND::SqnBkgd(pmsnd->Sty(), objID);

	if (!fQueue)
		for (isqe = 0; isqe < _pglsqe->IvMac(); isqe++)
			{
			psqe = (SQE *)_pglsqe->QvGet(isqe);
			sqnT = psqe->fActr ?
				MSND::SqnActr(psqe->pmsnd->Sty(), psqe->objID) : MSND::SqnBkgd(psqe->pmsnd->Sty(), psqe->objID);
			if (sqnT == sqn)
				{
				if (fLowPri)
					return fTrue;	// Nothing to enqueue;  same type already taken
				// Hi priority.  Get rid of lower priority sound.
				ReleasePpo(&psqe->pmsnd);
				_pglsqe->Delete(isqe);
				break;
				}
			}

	sqe.pmsnd = pmsnd;
	pmsnd->AddRef();
	sqe.objID = objID;
	sqe.fLoop = fLoop;
	sqe.fQueue = fQueue;
	sqe.vlmMod = vlm;
	sqe.spr = spr;
	sqe.fActr = fActr;
	sqe.dtsStart = dtsStart;

	if (fLowPri)
		{
		if (!_pglsqe->FPush(&sqe))
			{
			ReleasePpo(&sqe.pmsnd);
			return fFalse;
			}
		}
	else if (!_pglsqe->FEnqueue(&sqe))
		{
		ReleasePpo(&sqe.pmsnd);
		return fFalse;
		}

	return fTrue;
}


/***************************************************************************

	Dequeue and Play the MSQ sounds
	If _dtim == kdtimOff, empty the queue
	
***************************************************************************/
void MSQ::PlayMsq(void)
{
	AssertThis(0);

	SQE sqe;

	if (_pglsqe->IvMac() == 0)
		return;

	if (_dtim == kdtimOffMsq)
		{
		FlushMsq();
		return;
		}

	if (0 < _pglsqe->IvMac())
		{
		vpsndm->BeginSynch();

		while(_pglsqe->FDequeue(&sqe))
			{
			sqe.pmsnd->Play(sqe.objID, sqe.fLoop, sqe.fQueue, sqe.vlmMod,
				sqe.spr, sqe.fActr, sqe.dtsStart);
			ReleasePpo(&sqe.pmsnd);
			}

		vpsndm->EndSynch();
		}

   	if (_dtim < kdtimLongMsq)
		{
		_pclok->Start(0);
		if (!_pclok->FSetAlarm(_dtim, this))
			{
			StopAll();
			return;
			}
		}

	return;
}


/***************************************************************************

	Flush Queue	 -  without playing the sounds

***************************************************************************/
void MSQ::FlushMsq(void)
{
	AssertThis(0);
	SQE sqe;

	while( _pglsqe->FDequeue(&sqe))
		{
		ReleasePpo(&sqe.pmsnd);
		}
}

		
/***************************************************************************

	FCmdAlarm - Timeout has elapsed.  Stop all sounds

***************************************************************************/
bool MSQ::FCmdAlarm(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	_pclok->Stop();
	StopAll();		
	return fTrue;
}

/***************************************************************************

	Clean up and delete this movie sound queue

***************************************************************************/
MSQ::~MSQ(void)
{
	AssertBaseThis(0);
	StopAll();
	FlushMsq();
	ReleasePpo(&_pglsqe);
	ReleasePpo(&_pclok);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the MSND.
***************************************************************************/
void MSND::AssertValid(ulong grf)
{
	MSND_PAR::AssertValid(fobjAllocated);
	AssertNilOrPo(_prca, 0);
	AssertIn(_sty, 0, styLim);
}


/***************************************************************************
	Mark memory used by the MSND
***************************************************************************/
void MSND::MarkMem(void)
{
	AssertThis(0);
	MSND_PAR::MarkMem();
	// Note: don't mark _prca, because _prca marks us, and would cause
	// an infinite recursive loop.
}


/***************************************************************************
	Assert the validity of the MSQ.
***************************************************************************/
void MSQ::AssertValid(ulong grf)
{
	MSQ_PAR::AssertValid(fobjAllocated);
	AssertPo(_pglsqe, 0);
	AssertPo(_pclok, 0);
}


/***************************************************************************
	Mark memory used by the MSND
***************************************************************************/
void MSQ::MarkMem(void)
{
	AssertThis(0);
	MSQ_PAR::MarkMem();
	MarkMemObj(_pglsqe);
	MarkMemObj(_pclok);
}

#endif //DEBUG
