/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Audioman based sound classes.

***************************************************************************/
#include "frame.h"
#include "audioman.h"
#include "sndampri.h"
ASSERTNAME

// CHECK_AUDIO_DEVCAPS turns on using waveOutGetDevCaps to check for 
// audio device characteristics, to workaround AudioMan 1.0 always returning
// whatever device was asked for, regardless if device actually supports format.
// In case of asking for a 16 bit device, and there is only an 8 bit device, the
// open will succeed, causing sub-optimal audioman performance on an 8 bit card.
// With AudioMan 1.5, this should go away.
#define CHECK_AUDIO_DEVCAPS 

// Initialize the maximum mem footprint of wave sounds.
long SDAM::vcbMaxMemWave = 40 * 1024;

static IAMMixer *_pamix;	// the audioman mixer
static ulong _luGroup;		// the group number
static bool _fGrouped;		// whether new sounds are grouped
static long _cactGroup;		// group nesting count

static ulong _luFormat;		// format mixer is in
static ulong _luCacheTime;	// buffer size for mixer


RTCLASS(SDAM)
RTCLASS(CAMS)
RTCLASS(AMQUE)


/***************************************************************************
	Constructor for a streamed block.
***************************************************************************/
STBL::STBL(void)
{
	AssertThisMem();

	// WARNING: this is not allocated using our NewObj because STBL is not
	// based on BASE. So fields are not automatically initialized to 0.
	_cactRef = 1;
	_ib = 0;
}


/***************************************************************************
	Destructor for a streamed block.
***************************************************************************/
STBL::~STBL(void)
{
	AssertThisMem();
}


/***************************************************************************
	QueryInterface for STBL.
***************************************************************************/
STDMETHODIMP STBL::QueryInterface(REFIID riid, void ** ppv)
{
	AssertThis(0);

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IStream))
		{
		*ppv = (void *)this;
		AddRef();
		return S_OK;
		}

	*ppv = pvNil;
	return E_NOINTERFACE;
}


/***************************************************************************
	Increment the reference count.
***************************************************************************/
STDMETHODIMP_(ULONG) STBL::AddRef(void)
{
	AssertThis(0);
	return ++_cactRef;
}


/***************************************************************************
	Decrement the reference count.
***************************************************************************/
STDMETHODIMP_(ULONG) STBL::Release(void)
{
	AssertThis(0);
	long cactRef;

	if ((cactRef = --_cactRef) == 0)
		delete this;
	return cactRef;
}


/***************************************************************************
	Read some stuff.
***************************************************************************/
STDMETHODIMP STBL::Read(void * pv, ULONG cb, ULONG * pcb)
{
	AssertThis(0);
	AssertPvCb(pv, cb);
	AssertNilOrVarMem(pcb);

	cb = LwMin(_blck.Cb() - _ib, cb);
	if (_blck.FReadRgb(pv, cb, _ib))
		{
		_ib += cb;
		if (pvNil != pcb)
			*pcb = cb;
		return NOERROR;
		}

	if (pvNil != pcb)
		*pcb = 0;
	return ResultFromScode(STG_E_READFAULT);
}


/***************************************************************************
	Seek to a place.
***************************************************************************/
STDMETHODIMP STBL::Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin,
	ULARGE_INTEGER * plibNewPosition)
{
	AssertThis(0);
	AssertNilOrVarMem(plibNewPosition);

	switch (dwOrigin)
		{
	case STREAM_SEEK_SET:
		break;

	case STREAM_SEEK_CUR:
		dlibMove.QuadPart += _ib;
		break;

	case STREAM_SEEK_END:
		dlibMove.QuadPart += _blck.Cb();
		break;
		}

	if (dlibMove.QuadPart < 0 || dlibMove.QuadPart > _blck.Cb())
		{
		if (pvNil != plibNewPosition)
			plibNewPosition->QuadPart = _ib;
		return E_INVALIDARG;
		}

	_ib = (long)dlibMove.QuadPart;
	if (pvNil != plibNewPosition)
		plibNewPosition->QuadPart = _ib;
	return S_OK;
}


/***************************************************************************
	Static method to create a new stream wrapper around a flo.
***************************************************************************/
PSTBL STBL::PstblNew(FLO *pflo, bool fPacked)
{
	AssertPo(pflo, ffloReadable);
	PSTBL pstbl;
	BLCK blck;
	PBLCK pblck;

	if (pvNil == (pstbl = new STBL))
		return pvNil;

	pblck = &pstbl->_blck;
	if (fPacked)
		{
		// unpack the block
		pblck->Set(pflo, fPacked);
		if (!pblck->FUnpackData())
			{
			delete pstbl;
			return pvNil;
			}

		// see if it's too big to keep in memory
		if (pstbl->CbMem() > SDAM::vcbMaxMemWave)
			{
			// try to put the sound on disk
			HQ hq = pblck->HqFree();

			AssertHq(hq);
			if (pblck->FSetTemp(CbOfHq(hq), fTrue) &&
					pblck->FWriteHq(hq, 0))
				{
				FreePhq(&hq);
				}
			else
				pblck->SetHq(&hq);
			}
		}
	else
		{
		// see if it's on a removeable disk
		FNI fni;

		pflo->pfil->GetFni(&fni);
		if (fni.Grfvk() & (fvkFloppy | fvkCD | fvkRemovable))
			{
			// cache to the hard drive or memory, depending on the size
			BLCK blck(pflo);

			if (!pblck->FSetTemp(pflo->cb,
						blck.Cb() + size(STBL) > SDAM::vcbMaxMemWave) ||
					!blck.FWriteToBlck(pblck))
				{
				delete pstbl;
				return pvNil;
				}
			}
		else
			pblck->Set(pflo);
		}

	AssertPo(pstbl, 0);
	return pstbl;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a STBL.
***************************************************************************/
void STBL::AssertValid(ulong grf)
{
	AssertThisMem();
	AssertPo(&_blck, 0);
	AssertIn(_ib, 0, _blck.Cb() + 1);
}


/***************************************************************************
	Mark memory for the STBL.
***************************************************************************/
void STBL::MarkMem(void)
{
	AssertValid(0);
	MarkMemObj(&_blck);
}
#endif //DEBUG


/***************************************************************************
	Constructor for a cached AudioMan sound.
***************************************************************************/
CAMS::CAMS(void)
{
	AssertBaseThis(fobjAllocated);
}


/***************************************************************************
	Destructor for a cached AudioMan sound.
***************************************************************************/
CAMS::~CAMS(void)
{
	AssertBaseThis(fobjAllocated);
	ReleasePpo(&psnd);
	ReleasePpo(&_pstbl);
}


/***************************************************************************
	Static BACO reader method to put together a Cached AudioMan sound.
***************************************************************************/
bool CAMS::FReadCams(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, 0);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);
	FLO flo;
	bool fPacked;
	PCAMS pcams = pvNil;
	PSTBL pstbl = pvNil;

	*pcb = size(CAMS) + size(STBL);
	if (pvNil == ppbaco)
		return fTrue;

	*ppbaco = pvNil;
	if (!pcrf->Pcfl()->FFindFlo(ctg, cno, &flo))
		return fFalse;

	fPacked = pcrf->Pcfl()->FPacked(ctg, cno);
	if (pvNil == (pstbl = STBL::PstblNew(&flo, fPacked)))
		return fFalse;

	*pcb = size(CAMS) + pstbl->CbMem();
	if (pvNil == (pcams = NewObj CAMS) ||
			FAILED(AllocSoundFromStream(&pcams->psnd, pstbl, fTrue, pvNil)))
		{
		ReleasePpo(&pcams);
		}

	if (pvNil != pcams)
		pcams->_pstbl = pstbl;
	else
		ReleasePpo(&pstbl);

	AssertNilOrPo(pcams, 0);
	*ppbaco = pcams;

	return pvNil != *ppbaco;
}


/***************************************************************************
	Static BACO reader method to put together a Cached AudioMan sound.
***************************************************************************/
PCAMS CAMS::PcamsNewLoop(PCAMS pcamsSrc, long cactPlay)
{
	AssertPo(pcamsSrc, 0);
	Assert(cactPlay != 1, "bad loop count");
	PCAMS pcams = pvNil;

	if (pvNil == (pcams = NewObj CAMS) ||
		FAILED(AllocLoopFilter(&pcams->psnd, pcamsSrc->psnd,
			cactPlay - 1)))
		{
		ReleasePpo(&pcams);
		}

	if (pvNil != pcams)
		{
		pcams->_pstbl = pcamsSrc->_pstbl;
		pcams->_pstbl->AddRef();
		}

	AssertNilOrPo(pcams, 0);
	return pcams;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a CAMS.
***************************************************************************/
void CAMS::AssertValid(ulong grf)
{
	CAMS_PAR::AssertValid(0);
	AssertPo(_pstbl, 0);
	Assert(psnd != pvNil, 0);
}


/***************************************************************************
	Mark memory for the CAMS.
***************************************************************************/
void CAMS::MarkMem(void)
{
	AssertValid(0);
	CAMS_PAR::MarkMem();
	if (pvNil != _pstbl)
		_pstbl->MarkMem();
}
#endif //DEBUG


/***************************************************************************
	Constructor for our notify sink.
***************************************************************************/
AMNOT::AMNOT(void)
{
	_cactRef = 1;
	_pamque = pvNil;
}


/***************************************************************************
	Set the AMQUE that we're to notify.
***************************************************************************/
void AMNOT::Set(PAMQUE pamque)
{
	AssertNilOrVarMem(pamque);
	_pamque = pamque;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a AMNOT.
***************************************************************************/
void AMNOT::AssertValid(ulong grf)
{
	AssertThisMem();
	AssertNilOrVarMem(_pamque);
}
#endif //DEBUG


/***************************************************************************
	QueryInterface for AMNOT.
***************************************************************************/
STDMETHODIMP AMNOT::QueryInterface(REFIID riid, void ** ppv)
{
	AssertThis(0);

	if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IAMNotifySink))
		{
		*ppv = (void *)this;
		AddRef();
		return S_OK;
		}

	*ppv = pvNil;
	return E_NOINTERFACE;
}


/***************************************************************************
	Increment the reference count.
***************************************************************************/
STDMETHODIMP_(ULONG) AMNOT::AddRef(void)
{
	AssertThis(0);
	return ++_cactRef;
}


/***************************************************************************
	Decrement the reference count.
***************************************************************************/
STDMETHODIMP_(ULONG) AMNOT::Release(void)
{
	AssertThis(0);
	long cactRef;

	if ((cactRef = --_cactRef) == 0)
		delete this;
	return cactRef;
}


/***************************************************************************
	The indicated sound is done. Just tell the AMQUE that we got a notify.
***************************************************************************/
STDMETHODIMP_(void) AMNOT::OnCompletion(LPSOUND pSound, DWORD dwPosition)
{
	AssertThis(0);

	if (pvNil != _pamque)
		_pamque->Notify(pSound);
}


/***************************************************************************
	Constructor for an audioman queue.
***************************************************************************/
AMQUE::AMQUE(void)
{
}


/***************************************************************************
	Destructor for an audioman queue.
***************************************************************************/
AMQUE::~AMQUE(void)
{
	if (pvNil != _pchan)
		StopAll();
	ReleasePpo(&_pchan);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a AMQUE.
***************************************************************************/
void AMQUE::AssertValid(ulong grf)
{
	AMQUE_PAR::AssertValid(0);
	Assert(pvNil != _pchan, 0);
}
#endif //DEBUG


/***************************************************************************
	Static method to create a new audioman queue.
***************************************************************************/
PAMQUE AMQUE::PamqueNew(void)
{
	PAMQUE pamque;

	if (pvNil == (pamque = NewObj AMQUE))
		return pvNil;

	if (!pamque->_FInit())
		ReleasePpo(&pamque);

	AssertNilOrPo(pamque, 0);
	return pamque;
}


/***************************************************************************
	Initialize the audioman queue. Allocate the audioman channel and the
	_pglsndin.
***************************************************************************/
bool AMQUE::_FInit(void)
{
	AssertBaseThis(0);

	if (!AMQUE_PAR::_FInit())
		return fFalse;

	if (FAILED(_pamix->AllocChannel(&_pchan)))
		{
		_pchan = pvNil;
		return fFalse;
		}
	if (pvNil == _pchan)
		{
		Bug("Audioman messed up!");
		return fFalse;
		}

	_amnot.Set(this);
	if (FAILED(_pchan->RegisterNotify(&_amnot, NOTIFYSINK_ONCOMPLETION)))
		return fFalse;

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Enter the critical section protecting member variables.
***************************************************************************/
void AMQUE::_Enter(void)
{
	_mutx.Enter();
}


/***************************************************************************
	Leave the critical section protecting member variables.
***************************************************************************/
void AMQUE::_Leave(void)
{
	_mutx.Leave();
}


/***************************************************************************
	Fetch the given sound chunk as a CAMS.
***************************************************************************/
PBACO AMQUE::_PbacoFetch(PRCA prca, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertPo(prca, 0);

	return prca->PbacoFetch(ctg, cno, &CAMS::FReadCams);
}


/***************************************************************************
	An item was added to or deleted from the queue.
***************************************************************************/
void AMQUE::_Queue(long isndinMin)
{
	AssertThis(0);
	SNDIN sndin;
	long isndin;

	_Enter();

	if (pvNil != _pglsndin)
		{
		PCAMS pcams;

		for (isndin = isndinMin; isndin < _pglsndin->IvMac(); isndin++)
			{
			_pglsndin->Get(isndin, &sndin);

			if (1 == sndin.cactPlay)
				continue;

			// put a loop filter around it to get seamless sample based looping
			if (pvNil != (pcams = CAMS::PcamsNewLoop((PCAMS)sndin.pbaco,
					sndin.cactPlay)))
				{
				sndin.cactPlay = 1;	// now it's just one sound
				ReleasePpo(&sndin.pbaco);
				sndin.pbaco = pcams;
				_pglsndin->Put(isndin, &sndin);
				}
			}
		}

	if (_isndinCur == isndinMin && pvNil != _pglsndin)
		{
		for ( ; _isndinCur < _pglsndin->IvMac(); _isndinCur++)
			{
			_pglsndin->Get(_isndinCur, &sndin);
			if (0 <= sndin.cactPause)
				break;
			}

		if (_isndinCur < _pglsndin->IvMac() && 0 == sndin.cactPause)
			{
			// stop the channel
			_pchan->Stop();

			// set the volume
			_pchan->SetVolume(LuVolScale((ulong)(-1), sndin.vlm));

			// if the sound is in memory 	
			if (((PCAMS)sndin.pbaco)->FInMemory())
				{
				// set the sound source, with no cache (Since it's in memory)
				_pchan->SetSoundSrc(((PCAMS)sndin.pbaco)->psnd);
				}
			else
				{
				CacheConfig cc;
				cc.dwSize = size(cc);
				cc.fSrcFormat = fTrue;
				cc.lpFormat = pvNil;
				cc.dwFormat = _luFormat;
				cc.dwCacheTime = 2*_luCacheTime;
				
				// set the sound src, using cache cause it's not in memory
				_pchan->SetCachedSrc(((PCAMS)sndin.pbaco)->psnd, &cc);
				}

			// if there is a starting offset, apply it
			if (sndin.dtsStart != 0)
				_pchan->SetTimePos(sndin.dtsStart);

			if (!_fGrouped || FAILED(_pamix->EnlistGroup(_pchan, _luGroup)))
				{
				// start the channel
				_pchan->Play();
				}

			_tsStart = TsCurrentSystem() - sndin.dtsStart;
			}
		else
			{
			_pchan->Stop();
			_pchan->SetSoundSrc(pvNil);
			}
		}

	_Leave();
}


/***************************************************************************
	One or more items in the queue were paused.
***************************************************************************/
void AMQUE::_PauseQueue(long isndinMin)
{
	AssertThis(0);
	SNDIN sndin;

	_mutx.Enter();

	if (_isndinCur == isndinMin && _pglsndin->IvMac() > _isndinCur)
		{
		_pglsndin->Get(_isndinCur, &sndin);
		sndin.dtsStart = TsCurrentSystem() - _tsStart;
		_pglsndin->Put(_isndinCur, &sndin);

		_Queue(isndinMin);
		}

	_mutx.Leave();
}


/***************************************************************************
	One or more items in the queue were resumed.
***************************************************************************/
void AMQUE::_ResumeQueue(long isndinMin)
{
	AssertThis(0);

	_Queue(isndinMin);
}


/***************************************************************************
	Called by our notify sink to tell us that the indicated sound is done.
	WARNING: this is called in an auxillary thread.
***************************************************************************/
void AMQUE::Notify(LPSOUND psnd)
{
	AssertThis(0);
	SNDIN sndin;

	_Enter();

	if (pvNil != _pglsndin && _pglsndin->IvMac() > _isndinCur)
		{
		_pglsndin->Get(_isndinCur, &sndin);
		if (psnd == ((PCAMS)sndin.pbaco)->psnd)
			{
			if (--sndin.cactPlay == 0)
				{
				_isndinCur++;
				_Queue(_isndinCur);
				}
			else
				{
				// play the sound again
				_pglsndin->Put(_isndinCur, &sndin);
				_pchan->SetSoundSrc(((PCAMS)sndin.pbaco)->psnd);
				_tsStart = TsCurrentSystem();
				}
			}
		}

	_Leave();
}


/***************************************************************************
	Constructor for the audioman device.
***************************************************************************/
SDAM::SDAM(void)
{
	_vlm = kvlmFull;
	_luVolSys = (ulong)(-1);
}


/***************************************************************************
	Destructor for the audioman device.
***************************************************************************/
SDAM::~SDAM(void)
{
	AssertBaseThis(0);

	if (_fAudioManInited && 0 == _pamix->Release())
		_pamix = pvNil;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a SDAM.
***************************************************************************/
void SDAM::AssertValid(ulong grf)
{
	SDAM_PAR::AssertValid(0);
	Assert(_pamix != pvNil, 0);
}
#endif //DEBUG


/***************************************************************************
	Static method to create the audioman device.
***************************************************************************/
PSDAM SDAM::PsdamNew(long wav)
{
	PSDAM psdam;

	if (pvNil == (psdam = NewObj SDAM))
		return pvNil;

	if (!psdam->_FInit(wav))
		ReleasePpo(&psdam);

	AssertNilOrPo(psdam, 0);
	return psdam;
}


static long _mpwavfmt[] =
	{
	WAVE_FORMAT_1M08,
	WAVE_FORMAT_2M08,
	WAVE_FORMAT_4M08,
	WAVE_FORMAT_1S08,
	WAVE_FORMAT_2S08,
	WAVE_FORMAT_4S08,
	WAVE_FORMAT_1M16,
	WAVE_FORMAT_2M16,
	WAVE_FORMAT_4M16,
	WAVE_FORMAT_1S16,
	WAVE_FORMAT_2S16,
	WAVE_FORMAT_4S16,
	};

#ifdef CHECK_AUDIO_DEVCAPS
/******************************************************************************

@func   WORD | wHaveWaveDevice |

        Do we have a wave device capable of playing the passed PCM format(s).

@parm  DWORD | dwFormats | WAVE formats needed to be supported.  These can be
                            a bitwise combination of WAVE_FORMAT_???? flags
                            which are defined in mmsystem.h.  If you don't
                            care what formats are supported you can pass zero.

******************************************************************************/

bool FHaveWaveDevice(DWORD dwReqFormats)
{
	WORD        wNumWavDev;
	WAVEOUTCAPS WOC;
	WORD        wDevID;
	WORD        wErr;

	// Determine how many WAVE devices are in the user's system
	wNumWavDev = waveOutGetNumDevs();

	// If there are none, return indicating that
	if (0 == wNumWavDev)
		return(fFalse);

	// Cycle through the WAVE devices to determine if any support
	// the desired format.
	for (wDevID = 0; wDevID < wNumWavDev; wDevID++)
		{
		wErr = waveOutGetDevCaps(wDevID, &WOC, sizeof(WAVEOUTCAPS));

		// If we obtain a WAVE device's capabilities OK
		// and it supports the desired format
		if ((0 == wErr) && ((WOC.dwFormats & dwReqFormats) == dwReqFormats))
			{
			// then return success - we have a device that supports what we want
			return fTrue;
			}
		} 

	// it doesn't support this device
	return fFalse;
}
#endif


/***************************************************************************
	Initialize the audioman device.
***************************************************************************/
bool SDAM::_FInit(long wav)
{
	AssertBaseThis(0);
	MIXERCONFIG mixc;
	ADVMIXCONFIG amxc;

	if (!SDAM_PAR::_FInit())
		return fFalse;

	// get IAMMixer interface
	if (pvNil != _pamix)
		{
		_pamix->AddRef();
		_fAudioManInited = fTrue;
		}
	else
		{
		if (pvNil == (_pamix = GetAudioManMixer()))
			return fFalse;
		_fAudioManInited = fTrue;

		// REVIEW shonk: what values should we use?
		mixc.dwSize = size(mixc);
		mixc.lpFormat = pvNil;
		if (!FIn(wav, 0, kwavLim))
			wav = kwav22M16;
		mixc.dwFormat = _mpwavfmt[wav];
		amxc.dwSize = size(amxc);
		amxc.uVoices = 12;
		amxc.fRemixEnabled = fTrue;
		amxc.uBufferTime = 600;

#ifdef CHECK_AUDIO_DEVCAPS
		// if we don't have a device of this format...
		if (!FHaveWaveDevice(mixc.dwFormat))
			{
			// failed, so try dropping to 8 bit
			wav += kwav22M8 - kwav22M16;
			if (!FIn(wav, 0, kwavLim))
				return fFalse;
			mixc.dwFormat = _mpwavfmt[wav];
			// we'll try to open at 8, cause if the card doesn't 
			// support it, then WAVE_MAPPER will actually convert
			// to the 8 bit format.
			}

		_luFormat = mixc.dwFormat;
		_luCacheTime = amxc.uBufferTime;

		// initialize it (done only once...)
		if (FAILED(_pamix->Init(vwig.hinst, pvNil, &mixc, &amxc)))
			return fFalse;
#else
		_luFormat = mixc.dwFormat;
		_luCacheTime = amxc.uBufferTime;

		// initialize it (done only once...)
		if (FAILED(_pamix->Init(vwig.hinst, pvNil, &mixc, &amxc)))
			{
			// failed, so try at 8 bit
			wav += kwav22M8 - kwav22M16;
			if (!FIn(wav, 0, kwavLim))
				return fFalse;
			if (FAILED(_pamix->Init(vwig.hinst, pvNil, &mixc, &amxc)))
				return fFalse;
			}
#endif	
		if (FAILED(_pamix->Activate(fTrue)))
			return fFalse;
		}

	_Suspend(_cactSuspend > 0 || !_fActive);

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Allocate a new audioman queue.
***************************************************************************/
PSNQUE SDAM::_PsnqueNew(void)
{
	AssertThis(0);

	return AMQUE::PamqueNew();
}


/***************************************************************************
	Activate or deactivate audioman.
***************************************************************************/
void SDAM::_Suspend(bool fSuspend)
{
	AssertThis(0);

	if (fSuspend)
		_pamix->SetMixerVolume(_luVolSys);

	if (FAILED(_pamix->Suspend(fSuspend)) && !fSuspend)
		PushErc(ercSndamWaveDeviceBusy);
	else if (!fSuspend)
		{
		// becoming active
		_pamix->GetMixerVolume(&_luVolSys);
		vluSysVolFake = _luVolSys;
		_pamix->SetMixerVolume(LuVolScale(_luVolSys, _vlm));
		}
}


/***************************************************************************
	Set the volume.
***************************************************************************/
void SDAM::SetVlm(long vlm)
{
	AssertThis(0);

	if (_vlm != vlm)
		{
		_vlm = vlm;
		if (_cactSuspend <= 0 && _fActive)
			_pamix->SetMixerVolume(LuVolScale(_luVolSys, vlm));
		}
}


/***************************************************************************
	Get the current volume.
***************************************************************************/
long SDAM::VlmCur(void)
{
	AssertThis(0);

	return _vlm;
}


/***************************************************************************
	Begin a synchronization group.
***************************************************************************/
void SDAM::BeginSynch(void)
{
	AssertThis(0);

	if (0 == _cactGroup++)
		_fGrouped = SUCCEEDED(_pamix->AllocGroup(&_luGroup));
}


/***************************************************************************
	End a synchronization group.
***************************************************************************/
void SDAM::EndSynch(void)
{
	AssertThis(0);

	if ((0 == --_cactGroup) && _fGrouped)
		{
		_pamix->StartGroup(_luGroup, fTrue);
		_pamix->FreeGroup(_luGroup);
		_fGrouped = fFalse;
		}
}


