/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	The midi player device.

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(MIDP)


const long kdtsMinSlip = kdtsSecond / 30;
const long klwInfinite = klwMax;


/***************************************************************************
	Midi output object.
***************************************************************************/
enum
	{
	fmidoNil = 0x0,
	fmidoFirst = 0x1,
	fmidoFastFwd = 0x2,
	};

typedef class MIDO *PMIDO;
#define MIDO_PAR BASE
#define kclsMIDO 'MIDO'
class MIDO : public MIDO_PAR
	{
	RTCLASS_DEC

protected:
	typedef HMIDIOUT HMO;

	MUTX _mutx;			// restricts access to member variables
	HMO _hmo;			// the output device

	// system volume level - to be saved and restored. The volume we set
	// is always relative to this
	ulong _luVolSys;

	long _vlmBase;		// our current volume relative to _luVolSys.
	long _vlm;			// our current volume relative to _vlmBase

	long _sii;			// the sound that owns the _hmo
	long _spr;			// the priority of sound that owns the _hmo

	bool _fRestart: 1;	// whether the device needs reset
	bool _fSetVol: 1;	// whether the volume needs set

	void _GetSysVol(void);
	void _SetSysVol(ulong luVol);
	void _SetSysVlm(void);
	void _Reset(void);

public:
	MIDO(void);
	~MIDO(void);

	void Suspend(bool fSuspend);
	void SetVlm(long vlm);
	long VlmCur(void);

	bool FPlay(long sii, long spr, MIDEV *pmidev, long vlm, ulong grfmido);
	void Transition(long siiOld, long siiNew, long sprNew);
	void Close(long sii);
	};


static MIDO _mido;


RTCLASS(MIDO)

/***************************************************************************
	Constructor for the low level midi output device.
***************************************************************************/
MIDO::MIDO(void)
{
	_hmo = hNil;
	_sii = siiNil;
	_luVolSys = (ulong)(-1);
	_vlmBase = _vlm = kvlmFull;
	_fSetVol = fFalse;

	AssertThis(0);
}


/***************************************************************************
	Destructor for the low level midi output device.
***************************************************************************/
MIDO::~MIDO(void)
{
	AssertThis(0);

	_mutx.Enter();
	Suspend(fTrue);
	_mutx.Leave();
}


/***************************************************************************
	Get the system volume level.
***************************************************************************/
void MIDO::_GetSysVol(void)
{
	Assert(hNil != _hmo, "calling _VlmGetSys with nil _hmo");

	if (0 != midiOutGetVolume((uint)_hmo, &_luVolSys))
		{
		// failed - assume full volume
		_luVolSys = (ulong)(-1);
		}
}


/***************************************************************************
	Set the system volume level.
***************************************************************************/
void MIDO::_SetSysVol(ulong luVol)
{
	Assert(hNil != _hmo, "calling _SetSysVol with nil _hmo");
	midiOutSetVolume((uint)_hmo, luVol);
}


/***************************************************************************
	Set the system volume level from the current values of _vlm, _vlmBase
	and _luVolSys. We set the system volume to the result of scaling
	_luVolSys by _vlm and _vlmBase.
***************************************************************************/
void MIDO::_SetSysVlm(void)
{
	ulong luVol;

	luVol = LuVolScale(_luVolSys, _vlmBase);
	luVol = LuVolScale(luVol, _vlm);
	_SetSysVol(luVol);
}


/***************************************************************************
	Reset the midi device. Assumes that the mutx is already ours.
***************************************************************************/
void MIDO::_Reset(void)
{
	AssertThis(0);

	if (hNil != _hmo)
		{
		// Reset channel pressure and pitch wheel on all channels
		MIDEV midev;
		long iv;

		midiOutReset(_hmo);

		for (iv = 0; iv < 16; iv++)
			{
			midev.lwSend = 0;
			midev.rgbSend[0] = (byte)(0xD0 | iv);
			midiOutShortMsg(_hmo, midev.lwSend);

			midev.rgbSend[0] = (byte)(0xE0 | iv);
			midev.rgbSend[2] = 0x40;
			midiOutShortMsg(_hmo, midev.lwSend);
			}
		}
}


/***************************************************************************
	Release or grab the midi output device depending on fSuspend.
***************************************************************************/
void MIDO::Suspend(bool fSuspend)
{
	AssertThis(0);

	_mutx.Enter();
	if (FPure(fSuspend) != (hNil == _hmo))
		{
		if (fSuspend)
			{
			// kill all notes
			_Reset();

			// restore the volume level and free the device
			_SetSysVol(_luVolSys);
			midiOutClose(_hmo);
			_hmo = hNil;
			}
		else
			{
			if (MMSYSERR_NOERROR == midiOutOpen(&_hmo, MIDI_MAPPER,
					0, 0, CALLBACK_NULL))
				{
				_GetSysVol();
				}
			else
				{
				_hmo = hNil;
				PushErc(ercSndMidiDeviceBusy);
				}
			}
		}

	_fSetVol = _fRestart = fTrue;
	_mutx.Leave();
}


/***************************************************************************
	Set the master volume for the device.
***************************************************************************/
void MIDO::SetVlm(long vlm)
{
	AssertThis(0);

	if (vlm != _vlmBase)
		{
		_vlmBase = vlm;
		_fSetVol = fTrue;
		}
}


/***************************************************************************
	Return the current master volume.
***************************************************************************/
long MIDO::VlmCur(void)
{
	AssertThis(0);

	return _vlmBase;
}


/***************************************************************************
	Play the given midi event. Returns false iff the midi stream should be
	started over from the beginning in fast forward mode.
***************************************************************************/
bool MIDO::FPlay(long sii, long spr, MIDEV *pmidev, long vlm, ulong grfmido)
{
	AssertThis(0);
	AssertVarMem(pmidev);

	// assume we don't have to restart
	bool fRet = fTrue;

	_mutx.Enter();

	// see if this sound has higher priority than the current one
	if (_sii == sii)
		Assert(_spr == spr, 0);
	else if (siiNil == _sii || spr >= _spr && (sii > _sii || spr > _spr))
		{
		// this sound is higher priority so play it.
		_sii = sii;
		_spr = spr;
		_fRestart = fTrue;
		}

	// if this sound isn't the current one or the output we're deactivated
	// just pretend we played the event
	if (_sii != sii || hNil == _hmo)
		goto LDone;

	// If we need to restart, reset the device. If this is the first event
	// in the stream, go ahead and play it - otherwise, return false to tell
	// the client to restart.
	if (_fRestart)
		{
		_Reset();
		_fRestart = fFalse;

		if (!(grfmido & fmidoFirst))
			{
			fRet = fFalse;
			goto LDone;
			}
		}

	// do fast forward filtering
	if (grfmido & fmidoFastFwd)
		{
		// don't play notes or do other stuff that doesn't affect
		// the (long term) device state.
		switch (pmidev->rgbSend[0] & 0xF0)
			{
		default:
			goto LDone;

		case 0xB0: // control change
		case 0xC0: // program change
		case 0xD0: // channel pressure
		case 0xF0: // special stuff
			break;
			}
		}

	// make sure the volume is set correctly
	if (_fSetVol || _vlm != vlm)
		{
		_vlm = vlm;
		_fSetVol = fFalse;
		_SetSysVlm();
		}

	// finally, we can play the event
	midiOutShortMsg(_hmo, pmidev->lwSend);

LDone:
	_mutx.Leave();

	return fRet;
}


/***************************************************************************
	siiOld is being replaced by siiNew.
***************************************************************************/
void MIDO::Transition(long siiOld, long siiNew, long sprNew)
{
	AssertThis(0);

	_mutx.Enter();
	if (_sii == siiOld && siiNil != siiOld)
		{
		_sii = siiNew;
		_spr = sprNew;
		_Reset();
		}
	_mutx.Leave();
}


/***************************************************************************
	sii is going away.
***************************************************************************/
void MIDO::Close(long sii)
{
	AssertThis(0);

	_mutx.Enter();
	if (_sii == sii && siiNil != sii)
		{
		_sii = siiNil;
		if (hNil != _hmo)
			_Reset();
		}
	_mutx.Leave();
}


/***************************************************************************
	Midi player queue.
***************************************************************************/
typedef class MPQUE *PMPQUE;
#define MPQUE_PAR SNQUE
#define kclsMPQUE 'mpqu'
class MPQUE : public MPQUE_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	HN _hevtQueue;		// the queue event object - to signal new input
	bool _fChanged;		// also signals new input - for extra protection
	HN _hth;			// the thread handle

	MUTX _mutx;			// mutex to restrict access to member variables
	MSTP _mstp;			// midi stream parser
	long _dtsSlip;		// amount of time we've slipped by
	long _sii;			// id and priority of sound we're currently serving
	long _spr;
	long _vlm;			// volume to play back at
	MIDEV _midev;		// current midi event
	ulong _tsStart;		// time current sound was started
	ulong _grfmido;		// options for midi output device

	bool _fMidevValid: 1;	// is _midev valid?
	bool _fDone: 1;		// should the thread terminate?

	MPQUE(void);

	virtual void _Enter(void);
	virtual void _Leave(void);

	virtual bool _FInit(void);
	virtual PBACO _PbacoFetch(PRCA prca, CTG ctg, CNO cno);
	virtual void _Queue(long isndinMin);
	virtual void _PauseQueue(long isndinMin);
	virtual void _ResumeQueue(long isndinMin);

	static ulong __stdcall _ThreadProc(void *pv);

	ulong _LuThread(void);
	void _DoEvent(bool fRestart, long *pdtsWait);
	bool _FGetEvt(void);
	bool _FStartQueue(void);
	void _PlayEvt(void);


public:
	static PMPQUE PmpqueNew(void);
	~MPQUE(void);
	};


RTCLASS(MPQUE)


/***************************************************************************
	MT: Constructor for a midi player queue.
***************************************************************************/
MPQUE::MPQUE(void)
{
}


/***************************************************************************
	MP: Destructor for a midi player queue.
***************************************************************************/
MPQUE::~MPQUE(void)
{
	AssertThis(0);

	if (hNil != _hth)
		{
		// tell the thread to end and wait for it to finish
		_fDone = fTrue;
		SetEvent(_hevtQueue);
		WaitForSingleObject(_hth, INFINITE);
		}

	_mutx.Enter();

	if (hNil != _hevtQueue)
		CloseHandle(_hevtQueue);

	// clear the midi stream parser
	_mstp.Init(pvNil);

	_mutx.Leave();
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a MPQUE.
***************************************************************************/
void MPQUE::AssertValid(ulong grf)
{
	_mutx.Enter();

	MPQUE_PAR::AssertValid(0);
	AssertPo(&_mstp, 0);

	_mutx.Leave();
}


/***************************************************************************
	Mark memory for the MPQUE.
***************************************************************************/
void MPQUE::MarkMem(void)
{
	AssertValid(0);
	MPQUE_PAR::MarkMem();

	_mutx.Enter();
	MarkMemObj(&_mstp);
	_mutx.Leave();
}
#endif //DEBUG


/***************************************************************************
	MT: Static method to create a new midi player queue.
***************************************************************************/
PMPQUE MPQUE::PmpqueNew(void)
{
	PMPQUE pmpque;

	if (pvNil == (pmpque = NewObj MPQUE))
		return pvNil;

	if (!pmpque->_FInit())
		ReleasePpo(&pmpque);

	AssertNilOrPo(pmpque, 0);
	return pmpque;
}


/***************************************************************************
	MT: Initialize the midi queue.
***************************************************************************/
bool MPQUE::_FInit(void)
{
	AssertBaseThis(0);
	ulong luThread;

	if (!MPQUE_PAR::_FInit())
		return fFalse;

	// create an auto-reset event to signal that the midi stream at
	// the head of the queue has changed.
	_hevtQueue = CreateEvent(pvNil, fFalse, fFalse, pvNil);
	if (hNil == _hevtQueue)
		return fFalse;

	// create the thread in a suspended state
	_hth = CreateThread(pvNil, 1024, MPQUE::_ThreadProc, this,
		CREATE_SUSPENDED, &luThread);
	if (hNil == _hth)
		return fFalse;
	SetThreadPriority(_hth, THREAD_PRIORITY_TIME_CRITICAL);

	// set other members
	_sii = siiNil;

	// start the thread
	ResumeThread(_hth);

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Enter the critical section protecting member variables.
***************************************************************************/
void MPQUE::_Enter(void)
{
	_mutx.Enter();
}


/***************************************************************************
	Leave the critical section protecting member variables.
***************************************************************************/
void MPQUE::_Leave(void)
{
	_mutx.Leave();
}


/***************************************************************************
	MT: Fetch the given sound chunk as a midi stream.
***************************************************************************/
PBACO MPQUE::_PbacoFetch(PRCA prca, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertPo(prca, 0);

	return prca->PbacoFetch(ctg, cno, &MIDS::FReadMids);
}


/***************************************************************************
	The element at the head of the queue changed, notify the thread.
***************************************************************************/
void MPQUE::_Queue(long isndinMin)
{
	AssertThis(0);

	_mutx.Enter();

	if (_isndinCur == isndinMin)
		{
		// signal the thread that data changed
		SetEvent(_hevtQueue);
		_fChanged = fTrue;
		}

	_mutx.Leave();
}


/***************************************************************************
	Pause the sound at the head of the queue.
***************************************************************************/
void MPQUE::_PauseQueue(long isndinMin)
{
	AssertThis(0);
	SNDIN sndin;

	_mutx.Enter();

	if (_isndinCur == isndinMin && _pglsndin->IvMac() > _isndinCur)
		{
		_pglsndin->Get(_isndinCur, &sndin);
		sndin.dtsStart = TsCurrentSystem() - _tsStart;
		_pglsndin->Put(_isndinCur, &sndin);

		_Queue(_isndinCur);
		}

	_mutx.Leave();
}


/***************************************************************************
	Resume the sound at the head of the queue.
***************************************************************************/
void MPQUE::_ResumeQueue(long isndinMin)
{
	AssertThis(0);

	_Queue(isndinMin);
}


/***************************************************************************
	AT: Static method. Thread function for the midi thread object.
***************************************************************************/
ulong __stdcall MPQUE::_ThreadProc(void *pv)
{
	PMPQUE pmpque = (PMPQUE)pv;

	AssertPo(pmpque, 0);

	return pmpque->_LuThread();
}


/***************************************************************************
	AT: The midi playback thread.
***************************************************************************/
ulong MPQUE::_LuThread(void)
{
	AssertThis(0);
	bool fRestart;
	long dtsWait = klwInfinite;

	for (;;)
		{
		// wait until our time has expired or there is new data
		fRestart = dtsWait > 0 &&
			WAIT_TIMEOUT != WaitForSingleObject(_hevtQueue,
				dtsWait == klwInfinite ? INFINITE : dtsWait);

		// check to see if this thread should end
		if (_fDone)
			return 0;

		_mutx.Enter();
		if (_fChanged && !fRestart)
			dtsWait = klwInfinite;
		else
			{
			_fChanged = fFalse;
			_DoEvent(fRestart, &dtsWait);
			}
		_mutx.Leave();
		}
}


/***************************************************************************
	Called when it's time to send the next midi event or when the queue
	has changed. Assumes the mutx is already checked out.
***************************************************************************/
void MPQUE::_DoEvent(bool fRestart, long *pdtsWait)
{
	if (fRestart && !_FStartQueue())
		*pdtsWait = klwInfinite;
	else if (!_FGetEvt())
		{
		// we're done playing this tune, so start the next one
		_isndinCur++;
		*pdtsWait = _FStartQueue() ? 0 : klwInfinite;
		}
	else
		{
		// we have a valid midi event
		*pdtsWait = (long)(_midev.ts - TsCurrentSystem());
		if (*pdtsWait <= 0)
			{
			// go ahead and send it
			if (*pdtsWait < -kdtsMinSlip && !(_grfmido & fmidoFastFwd))
				{
				_dtsSlip -= *pdtsWait;
				*pdtsWait = 0;
				}
			_PlayEvt();
			}
		else
			_grfmido &= ~fmidoFastFwd;
		}
}


/***************************************************************************
	AT: Start playing the sound at the head of the queue. Return non-zero
	iff the queue wasn't empty. Note that the sound is left in the queue.
***************************************************************************/
bool MPQUE::_FStartQueue(void)
{
	SNDIN sndin;

	_mutx.Enter();

	// set up the midi stream parser (_mstp).
	for ( ; _isndinCur < _pglsndin->IvMac(); _isndinCur++)
		{
		_pglsndin->Get(_isndinCur, &sndin);
		AssertPo(sndin.pbaco, 0);
		if (0 <= sndin.cactPause)
			break;
		}

	if (_isndinCur < _pglsndin->IvMac() && 0 == sndin.cactPause)
		{
		// transition to the new tune
		_mido.Transition(_sii, sndin.sii, sndin.spr);

		_sii = sndin.sii;
		_spr = sndin.spr;
		_vlm = sndin.vlm;
		_tsStart = TsCurrentSystem() - sndin.dtsStart;
		_mstp.Init((PMIDS)sndin.pbaco, _tsStart);
		_dtsSlip = TsCurrentSystem() - sndin.dtsStart - _tsStart;
		_grfmido = fmidoNil;
		_fMidevValid = fFalse;

		if (sndin.dtsStart > 0)
			_grfmido |= fmidoFastFwd;
		if (sndin.pbaco != pvNil)
			_grfmido |= fmidoFirst;
		}
	else
		{
		// close the old tune
		_mido.Close(_sii);
		_sii = siiNil;

		sndin.pbaco = pvNil;
		_mstp.Init(pvNil, 0);
		_fMidevValid = fFalse;
		}

	_mutx.Leave();

	return sndin.pbaco != pvNil;
}


/***************************************************************************
	AT: Get the next event. Assumes we already have the mutex.
***************************************************************************/
bool MPQUE::_FGetEvt(void)
{
	AssertThis(0);
	ulong ts;
	SNDIN sndin;

	if (_fMidevValid)
		return fTrue;

	ts = kluMax;
	while (_mstp.FGetEvent(&_midev))
		{
		_midev.ts += _dtsSlip;

		// skip empty events
		if (_midev.cb > 0)
			{
			_fMidevValid = fTrue;
			return fTrue;
			}
		ts = _midev.ts;
		}

	// see if we should repeat the current midi stream
	_pglsndin->Get(_isndinCur, &sndin);
	if (--sndin.cactPlay == 0)
		return fFalse;
	_pglsndin->Put(_isndinCur, &sndin);

	_tsStart = TsCurrentSystem();
	if (ts != kluMax && ts > _tsStart)
		_tsStart = ts;

	_mstp.Init((PMIDS)sndin.pbaco, _tsStart);
	_dtsSlip = TsCurrentSystem() - _tsStart;
	if (!_mstp.FGetEvent(&_midev))
		{
		// there's nothing in this midi stream
		return fFalse;
		}
	_midev.ts += _dtsSlip;
	_fMidevValid = fTrue;

	return fTrue;
}


/***************************************************************************
	AT: Play the current event. Assumes we have the member mutex (_mutx).
***************************************************************************/
void MPQUE::_PlayEvt(void)
{
	AssertThis(0);
	Assert(_fMidevValid, 0);

	if (!_mido.FPlay(_sii, _spr, &_midev, _vlm, _grfmido))
		{
		// restart the stream in fast forward mode
		SNDIN sndin;

		_pglsndin->Get(_isndinCur, &sndin);
		_mstp.Init((PMIDS)sndin.pbaco, _tsStart);
		_grfmido |= fmidoFastFwd;
		}
	_fMidevValid = fFalse;
	_grfmido &= ~fmidoFirst;
}


/***************************************************************************
	Constructor for the midi player device.
***************************************************************************/
MIDP::MIDP(void)
{
}


/***************************************************************************
	Destructor for the midi player device.
***************************************************************************/
MIDP::~MIDP(void)
{
	_Suspend(fTrue);
}


/***************************************************************************
	Static method to create the midiplayer device.
***************************************************************************/
PMIDP MIDP::PmidpNew(void)
{
	PMIDP pmidp;

	if (pvNil == (pmidp = NewObj MIDP))
		return pvNil;

	if (!pmidp->_FInit())
		ReleasePpo(&pmidp);

	pmidp->_Suspend(!pmidp->_fActive || pmidp->_cactSuspend > 0);

	AssertNilOrPo(pmidp, 0);
	return pmidp;
}


/***************************************************************************
	Allocate a new midi queue.
***************************************************************************/
PSNQUE MIDP::_PsnqueNew(void)
{
	AssertThis(0);

	return MPQUE::PmpqueNew();
}


/***************************************************************************
	Get or release the HMIDIOUT depending on fSuspend.
***************************************************************************/
void MIDP::_Suspend(bool fSuspend)
{
	_mido.Suspend(fSuspend);
}


/***************************************************************************
	Set the volume.
***************************************************************************/
void MIDP::SetVlm(long vlm)
{
	AssertThis(0);

	_mido.SetVlm(vlm);
}


/***************************************************************************
	Get the volume.
***************************************************************************/
long MIDP::VlmCur(void)
{
	AssertThis(0);

	return _mido.VlmCur();
}

