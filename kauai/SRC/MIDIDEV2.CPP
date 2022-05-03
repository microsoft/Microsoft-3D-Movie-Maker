/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	The midi player device using a Midi Stream.

***************************************************************************/
#include "frame.h"
#include "mdev2pri.h"
ASSERTNAME



RTCLASS(MDWS)
RTCLASS(MSQUE)
RTCLASS(MDPS)
RTCLASS(MSMIX)
RTCLASS(MISI)
RTCLASS(WMS)
RTCLASS(OMS)

const long kdtsMinSlip = kdtsSecond / 30;
const long kcbMaxWmsBuffer = 0x0000FFFF / size(MEV) * size(MEV);


/***************************************************************************
	Constructor for the midi stream device.
***************************************************************************/
MDPS::MDPS(void)
{
}


/***************************************************************************
	Destructor for the midi stream device.
***************************************************************************/
MDPS::~MDPS(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pmsmix);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a MDPS.
***************************************************************************/
void MDPS::AssertValid(ulong grf)
{
	MDPS_PAR::AssertValid(0);
	AssertPo(_pmsmix, 0);
}


/***************************************************************************
	Mark memory for the MDPS.
***************************************************************************/
void MDPS::MarkMem(void)
{
	AssertValid(0);
	MDPS_PAR::MarkMem();
	MarkMemObj(_pmsmix);
}
#endif //DEBUG


/***************************************************************************
	Static method to create the midi stream device.
***************************************************************************/
PMDPS MDPS::PmdpsNew(void)
{
	PMDPS pmdps;

	if (pvNil == (pmdps = NewObj MDPS))
		return pvNil;

	if (!pmdps->_FInit())
		ReleasePpo(&pmdps);

	AssertNilOrPo(pmdps, 0);
	return pmdps;
}


/***************************************************************************
	Initialize the midi stream device.
***************************************************************************/
bool MDPS::_FInit(void)
{
	AssertBaseThis(0);

	if (!MDPS_PAR::_FInit())
		return fFalse;

	// Create the midi stream output scheduler
	if (pvNil == (_pmsmix = MSMIX::PmsmixNew()))
		return fFalse;

	_Suspend(_cactSuspend > 0 || !_fActive);

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Allocate a new midi stream queue.
***************************************************************************/
PSNQUE MDPS::_PsnqueNew(void)
{
	AssertThis(0);

	return MSQUE::PmsqueNew(_pmsmix);
}


/***************************************************************************
	Activate or deactivate the midi stream device.
***************************************************************************/
void MDPS::_Suspend(bool fSuspend)
{
	AssertThis(0);

	_pmsmix->Suspend(fSuspend);
}


/***************************************************************************
	Set the volume.
***************************************************************************/
void MDPS::SetVlm(long vlm)
{
	AssertThis(0);

	_pmsmix->SetVlm(vlm);
}


/***************************************************************************
	Get the current volume.
***************************************************************************/
long MDPS::VlmCur(void)
{
	AssertThis(0);

	return _pmsmix->VlmCur();
}


/***************************************************************************
	Constructor for a midi stream object.
***************************************************************************/
MDWS::MDWS(void)
{
}


/***************************************************************************
	Destructor for a Win95 midi stream object.
***************************************************************************/
MDWS::~MDWS(void)
{
	ReleasePpo(&_pglmev);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a MDWS.
***************************************************************************/
void MDWS::AssertValid(ulong grf)
{
	MDWS_PAR::AssertValid(0);
	AssertPo(_pglmev, 0);
}


/***************************************************************************
	Mark memory for the MDWS.
***************************************************************************/
void MDWS::MarkMem(void)
{
	AssertValid(0);
	MDWS_PAR::MarkMem();
	MarkMemObj(_pglmev);
}
#endif //DEBUG


/***************************************************************************
	A baco reader for a midi stream.
***************************************************************************/
bool MDWS::FReadMdws(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, fblckReadable);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);
	PMDWS pmdws;

	*pcb = pblck->Cb(fTrue);
	if (pvNil == ppbaco)
		return fTrue;

	if (!pblck->FUnpackData())
		goto LFail;

	if (pvNil == (pmdws = PmdwsRead(pblck)))
		{
LFail:
		TrashVar(ppbaco);
		TrashVar(pcb);
		return fFalse;
		}
	*pcb = pmdws->_pglmev->IvMac() * size(MEV) + size(MDWS);

	*ppbaco = pmdws;
	return fTrue;
}


/***************************************************************************
	Read a midi stream from the given block.
***************************************************************************/
PMDWS MDWS::PmdwsRead(PBLCK pblck)
{
	AssertPo(pblck, 0);

	PMIDS pmids;
	PMDWS pmdws;

	if (pvNil == (pmids = MIDS::PmidsRead(pblck)))
		return pvNil;

	if (pvNil != (pmdws = NewObj MDWS) && !pmdws->_FInit(pmids))
		ReleasePpo(&pmdws);

	ReleasePpo(&pmids);
	AssertNilOrPo(pmdws, 0);

	return pmdws;
}


/***************************************************************************
	Initialize the MDWS with the midi data in *pmids.
***************************************************************************/
bool MDWS::_FInit(PMIDS pmids)
{
	AssertPo(pmids, 0);

	MSTP mstp;
	ulong tsCur;
	MEV rgmev[100];
	PMEV pmev, pmevLim;
	MIDEV midev;
	bool fEvt;

	if (pvNil == (_pglmev = GL::PglNew(size(MEV))))
		return fFalse;

	Assert(MEVT_SHORTMSG == 0,
		"this code assumes MEVT_SHORTMSG is 0 and it's not");

	ClearPb(rgmev, size(rgmev));
	pmev = rgmev;
	pmevLim = rgmev + CvFromRgv(rgmev);

	// use 1 second per quarter. We'll use 1000 ticks per quarter when
	// setting up the stream. The net result is that milliseconds correspond
	// to ticks, so no conversion is necessary here.
	pmev->dwEvent = ((ulong)MEVT_TEMPO << 24) | 1000000;
	pmev++;

	mstp.Init(pmids, 0);
	tsCur = 0;
	for (;;)
		{
		fEvt = mstp.FGetEvent(&midev);
		if (pmev >= pmevLim || !fEvt)
			{
			// append the MEVs in rgmev to the _pglmev
			long imev, cmev;

			imev = _pglmev->IvMac();
			cmev = pmev - rgmev;
			if (!_pglmev->FSetIvMac(imev + cmev))
				return fFalse;
			CopyPb(rgmev, _pglmev->QvGet(imev), LwMul(cmev, size(MEV)));
			if (!fEvt)
				{
				// Add a final NOP so when we seek and there's only one
				// event left, it's not an important one.
				rgmev[0].dwDeltaTime = 0;
				rgmev[0].dwStreamID = 0;
				rgmev[0].dwEvent = (ulong)MEVT_NOP << 24;
				if (!_pglmev->FAdd(&rgmev[0]))
					return fFalse;

				_pglmev->FEnsureSpace(0, fgrpShrink);
				break;
				}
			pmev = rgmev;
			}

		if (midev.cb > 0)
			{
			pmev->dwDeltaTime = midev.ts - tsCur;
			pmev->dwEvent = midev.lwSend & 0x00FFFFFF;
			pmev++;
			tsCur = midev.ts;
			}
		}

	_dts = tsCur;
	return fTrue;
}


/***************************************************************************
	Return a locked pointer to the data.
***************************************************************************/
void *MDWS::PvLockData(long *pcb)
{
	AssertThis(0);
	AssertVarMem(pcb);

	*pcb = LwMul(_pglmev->IvMac(), size(MEV));
	return _pglmev->PvLock(0);
}


/***************************************************************************
	Balance a call to PvLockData.
***************************************************************************/
void MDWS::UnlockData(void)
{
	AssertThis(0);

	_pglmev->Unlock();
}


/***************************************************************************
	Constructor for a midi stream queue.
***************************************************************************/
MSQUE::MSQUE(void)
{
}


/***************************************************************************
	Destructor for a midi stream queue.
***************************************************************************/
MSQUE::~MSQUE(void)
{
	if (pvNil != _pmsmix)
		{
		_pmsmix->FPlay(this);
		ReleasePpo(&_pmsmix);
		}
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a MSQUE.
***************************************************************************/
void MSQUE::AssertValid(ulong grf)
{
	MSQUE_PAR::AssertValid(0);
	AssertPo(_pmsmix, 0);
}


/***************************************************************************
	Mark memory for the MSQUE.
***************************************************************************/
void MSQUE::MarkMem(void)
{
	AssertValid(0);
	MSQUE_PAR::MarkMem();
	MarkMemObj(_pmsmix);
}
#endif //DEBUG


/***************************************************************************
	Static method to create a new midi stream queue.
***************************************************************************/
PMSQUE MSQUE::PmsqueNew(PMSMIX pmsmix)
{
	AssertPo(pmsmix, 0);
	PMSQUE pmsque;

	if (pvNil == (pmsque = NewObj MSQUE))
		return pvNil;

	if (!pmsque->_FInit(pmsmix))
		ReleasePpo(&pmsque);

	AssertNilOrPo(pmsque, 0);
	return pmsque;
}


/***************************************************************************
	Initialize the midi stream queue.
***************************************************************************/
bool MSQUE::_FInit(PMSMIX pmsmix)
{
	AssertPo(pmsmix, 0);
	AssertBaseThis(0);

	if (!MSQUE_PAR::_FInit())
		return fFalse;

	_pmsmix = pmsmix;
	_pmsmix->AddRef();

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Enter the critical section protecting member variables.
***************************************************************************/
void MSQUE::_Enter(void)
{
	_mutx.Enter();
}


/***************************************************************************
	Leave the critical section protecting member variables.
***************************************************************************/
void MSQUE::_Leave(void)
{
	_mutx.Leave();
}


/***************************************************************************
	Fetch the given sound chunk as an MDWS.
***************************************************************************/
PBACO MSQUE::_PbacoFetch(PRCA prca, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertPo(prca, 0);

	return prca->PbacoFetch(ctg, cno, &MDWS::FReadMdws);
}


/***************************************************************************
	An item was added to or deleted from the queue.
***************************************************************************/
void MSQUE::_Queue(long isndinMin)
{
	AssertThis(0);
	SNDIN sndin;

	_mutx.Enter();

	if (_isndinCur == isndinMin && pvNil != _pglsndin)
		{
		for ( ; _isndinCur < _pglsndin->IvMac(); _isndinCur++)
			{
			_pglsndin->Get(_isndinCur, &sndin);
			if (0 < sndin.cactPause)
				break;

			if (0 == sndin.cactPause &&
				_pmsmix->FPlay(this, (PMDWS)sndin.pbaco, sndin.sii, sndin.spr,
					sndin.cactPlay, sndin.dtsStart, sndin.vlm))
				{
				_tsStart = TsCurrentSystem() - sndin.dtsStart;
				goto LDone;
				}
			}

		_pmsmix->FPlay(this);
		}

LDone:
	_mutx.Leave();
}


/***************************************************************************
	One or more items in the queue were paused.
***************************************************************************/
void MSQUE::_PauseQueue(long isndinMin)
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
void MSQUE::_ResumeQueue(long isndinMin)
{
	AssertThis(0);

	_Queue(isndinMin);
}


/***************************************************************************
	Called by the MSMIX to tell us that the indicated sound is done.
	WARNING: this is called in an auxillary thread.
***************************************************************************/
void MSQUE::Notify(PMDWS pmdws)
{
	AssertThis(0);
	SNDIN sndin;

	_mutx.Enter();

	if (pvNil != _pglsndin && _pglsndin->IvMac() > _isndinCur)
		{
		_pglsndin->Get(_isndinCur, &sndin);
		if (pmdws == sndin.pbaco)
			{
			_isndinCur++;
			_Queue(_isndinCur);
			}
		}

	_mutx.Leave();
}


/***************************************************************************
	Constructor for the midi stream output object.
***************************************************************************/
MSMIX::MSMIX(void)
{
	_vlmBase = kvlmFull;
	_vlmSound = kvlmFull;
}


/***************************************************************************
	Destructor for the midi stream output object.
***************************************************************************/
MSMIX::~MSMIX(void)
{
	Assert(pvNil == _pmisi || !_pmisi->FActive(), "MISI still active!");

	if (hNil != _hth)
		{
		// tell the thread to end and wait for it to finish
		_fDone = fTrue;
		SetEvent(_hevt);
		WaitForSingleObject(_hth, INFINITE);
		}

	if (hNil != _hevt)
		CloseHandle(_hevt);

	if (pvNil != _pglmsos)
		{
		Assert(_pglmsos->IvMac() == 0, "MSMIX still has active sounds");
		ReleasePpo(&_pglmsos);
		}
	ReleasePpo(&_pmisi);
	ReleasePpo(&_pglmevKey);
}


/***************************************************************************
	Static method to create a new MSMIX.
***************************************************************************/
PMSMIX MSMIX::PmsmixNew(void)
{
	PMSMIX pmsmix;

	if (pvNil == (pmsmix = NewObj MSMIX))
		return pvNil;

	if (!pmsmix->_FInit())
		ReleasePpo(&pmsmix);

	AssertNilOrPo(pmsmix, 0);
	return pmsmix;
}


/***************************************************************************
	Initialize the MSMIX - allocate the pglmsos and the midi stream api
	object.
***************************************************************************/
bool MSMIX::_FInit(void)
{
	AssertBaseThis(0);
	ulong luThread;

	if (pvNil == (_pglmsos = GL::PglNew(size(MSOS))))
		return fFalse;
	_pglmsos->SetMinGrow(1);

	if (pvNil == (_pmisi = WMS::PwmsNew(_MidiProc, (ulong)this)) &&
			pvNil == (_pmisi = OMS::PomsNew(_MidiProc, (ulong)this)))
		{
		return fFalse;
		}

	if (hNil == (_hevt = CreateEvent(pvNil, fFalse, fFalse, pvNil)))
		return fFalse;

	// create the thread
	if (hNil == (_hth = CreateThread(pvNil, 1024, MSMIX::_ThreadProc,
			this, 0, &luThread)))
		{
		return fFalse;
		}

	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a MSMIX.
***************************************************************************/
void MSMIX::AssertValid(ulong grf)
{
	MSMIX_PAR::AssertValid(0);
	_mutx.Enter();
	Assert(hNil != _hevt, "nil event");
	Assert(hNil != _hth, "nil thread");
	AssertPo(_pglmsos, 0);
	AssertPo(_pmisi, 0);
	AssertNilOrPo(_pglmevKey, 0);
	_mutx.Leave();
}


/***************************************************************************
	Mark memory for the MSMIX.
***************************************************************************/
void MSMIX::MarkMem(void)
{
	AssertValid(0);

	MSMIX_PAR::MarkMem();

	_mutx.Enter();
	MarkMemObj(_pglmsos);
	MarkMemObj(_pmisi);
	MarkMemObj(_pglmevKey);
	_mutx.Leave();
}
#endif //DEBUG


/***************************************************************************
	Suspend or resume the midi stream mixer.
***************************************************************************/
void MSMIX::Suspend(bool fSuspend)
{
	AssertThis(0);

	_mutx.Enter();

	if (fSuspend)
		_StopStream();
	if (!_pmisi->FActivate(!fSuspend) && !fSuspend)
		PushErc(ercSndMidiDeviceBusy);
	_Restart();

	_mutx.Leave();
}


/***************************************************************************
	If we're currently playing a midi stream stop it. Assumes the mutx is
	already checked out exactly once.
***************************************************************************/
void MSMIX::_StopStream(void)
{
	AssertThis(0);

	if (!_fPlaying)
		return;

	// set _fPlaying to false first so the call back knows that we're
	// aborting the current stream - so it doesn't notify us.
	_fPlaying = fFalse;

	_pmisi->StopPlaying();

	// Wait for the buffers to be returned
	_fWaiting = fTrue;
	_mutx.Leave();

	while (_cpvOut > 0)
		Sleep(0);

	_mutx.Enter();
	_fWaiting = fFalse;
}


/***************************************************************************
	Set the volume for the midi stream output device.
***************************************************************************/
void MSMIX::SetVlm(long vlm)
{
	AssertThis(0);
	ulong luHigh, luLow;

	_mutx.Enter();

	_vlmBase = vlm;
	MulLu(_vlmBase, _vlmSound, &luHigh, &luLow);
	_pmisi->SetVlm(LuHighLow(SuLow(luHigh), SuHigh(luLow)));

	_mutx.Leave();
}


/***************************************************************************
	Get the current volume.
***************************************************************************/
long MSMIX::VlmCur(void)
{
	AssertThis(0);

	return _vlmBase;
}


/***************************************************************************
	Play the given midi stream from the indicated queue.
***************************************************************************/
bool MSMIX::FPlay(PMSQUE pmsque, PMDWS pmdws, long sii, long spr,
	long cactPlay, ulong dtsStart, long vlm)
{
	AssertThis(0);
	AssertPo(pmsque, 0);
	AssertNilOrPo(pmdws, 0);
	long imsos;
	MSOS msos;
	bool fNew = fFalse;
	bool fRet = fTrue;

	if (pvNil != pmdws && pmdws->Dts() == 0)
		return fFalse;

	_mutx.Enter();

	// stop any current midi stream on this msque
	for (imsos = _pglmsos->IvMac(); imsos-- > 0; )
		{
		_pglmsos->Get(imsos, &msos);
		if (msos.pmsque == pmsque)
			{
			if (0 == imsos)
				_StopStream();
			_pglmsos->Get(imsos, &msos);
			_pglmsos->Delete(imsos);
			break;
			}
		}

	// start up the new midi stream
	if (pvNil != pmdws)
		{
		// find the position to insert the new one
		for (imsos = 0; imsos < _pglmsos->IvMac(); imsos++)
			{
			_pglmsos->Get(imsos, &msos);
			if (msos.spr < spr || msos.spr == spr && msos.sii < sii)
				{
				// insert before the current one
				break;
				}
			}

		if (0 == imsos)
			{
			fNew = fTrue;
			_StopStream();
			}

		ClearPb(&msos, size(msos));
		msos.pmsque = pmsque;
		msos.pmdws = pmdws;
		msos.sii = sii;
		msos.spr = spr;
		msos.cactPlay = cactPlay;
		msos.dts = pmdws->Dts();
		msos.dtsStart = dtsStart;
		msos.vlm = vlm;
		msos.tsStart = TsCurrentSystem() - msos.dtsStart;

		if (!_pglmsos->FInsert(imsos, &msos))
			{
			fRet = fFalse;
			fNew = fFalse;
			}
		}

	_Restart(fNew);

	_mutx.Leave();
	return fRet;
}


/***************************************************************************
	The sound list changed so make sure we're playing the first tune.
	Assumes the mutx is already checked out.
***************************************************************************/
void MSMIX::_Restart(bool fNew)
{
	AssertThis(0);

	if (_pmisi->FActive() && !_fPlaying && _pglmsos->IvMac() > 0)
		{
		// start playing the first MSOS
		MSOS msos;
		ulong tsCur = TsCurrentSystem();

		if (fNew)
			{
			_pglmsos->Get(0, &msos);
			msos.tsStart = tsCur - msos.dtsStart;
			_pglmsos->Put(0, &msos);
			}
		_SubmitBuffers(tsCur);
		}

	// signal the aux thread that the list changed
	SetEvent(_hevt);
}


/***************************************************************************
	Submit the buffer(s) for the current MSOS. Assumes the mutx is already
	checked out.
***************************************************************************/
void MSMIX::_SubmitBuffers(ulong tsCur)
{
	Assert(!_fPlaying, "already playing!");
	long cb, cbSkip;
	MSOS msos;
	long imsos;
	void *pvData;
	long cactSkip;

	for (imsos = 0; imsos < _pglmsos->IvMac(); imsos++)
		{
		_pglmsos->Get(imsos, &msos);

		cactSkip = (tsCur - msos.tsStart) / msos.dts;
		if (cactSkip > 0)
			{
			ulong dtsSeek;

			// we need to skip at least one loop of this sound
			if (msos.cactPlay > 0 && (msos.cactPlay -= cactSkip) <= 0)
				goto LTryNext;

			dtsSeek = (tsCur - msos.tsStart) % msos.dts;
			msos.tsStart = tsCur - dtsSeek;
			_pglmsos->Put(imsos, &msos);
			}

		// Calling SetVlm causes us to tell the MISI about the new volume
		_vlmSound = msos.vlm;
		SetVlm(_vlmBase);

		cbSkip = 0;
		if (tsCur != msos.tsStart)
			{
			// have to seek into the stream
			if (!_FGetKeyEvents(msos.pmdws, tsCur - msos.tsStart, &cbSkip))
				goto LTryNext;

			cb = LwMul(_pglmevKey->IvMac(), size(MEV));
			if (0 < cb)
				{
				pvData = _pglmevKey->PvLock(0);
				if (!_pmisi->FQueueBuffer(pvData, cb, 0, 1, 0))
					{
					// streaming the key events failed
					_pglmevKey->Unlock();
					goto LTryNext;
					}

				_cpvOut++;
				_fPlaying = fTrue;
				}
			}

		_cpvOut++;
		pvData = msos.pmdws->PvLockData(&cb);
		if (_pmisi->FQueueBuffer(pvData, cb, cbSkip, msos.cactPlay,
				(ulong)msos.pmdws))
			{
			// it worked!
			_fPlaying = fTrue;
			break;
			}

		// submitting the buffer failed
		msos.pmdws->UnlockData();
		_cpvOut--;

LTryNext:
		// make this one disappear
		// stop the seek buffer from playing
		_StopStream();

		// make this MSOS have lowest possible priority and 0 time
		// remaining to play - we'll move it to the end of _pglmsos
		// in the code below.
		msos.tsStart = tsCur - msos.dtsStart;
		msos.cactPlay = 1;
		msos.sii = klwMin;
		msos.spr = klwMin;
		_pglmsos->Put(imsos, &msos);
		}

	if (_fPlaying && imsos > 0)
		{
		// move the skipped ones to the end of the list
		long cmsos = _pglmsos->IvMac();

		AssertIn(imsos, 1, cmsos);
		SwapBlocks(_pglmsos->QvGet(0), LwMul(imsos, size(MSOS)),
			LwMul(cmsos - imsos, size(MSOS)));
		}
}


/***************************************************************************
	Seek into the pmdws the given amount of time, and accumulate key events
	in _pglmevKey.
***************************************************************************/
bool MSMIX::_FGetKeyEvents(PMDWS pmdws, ulong dtsSeek, long *pcbSkip)
{
	AssertPo(pmdws, 0);
	AssertVarMem(pcbSkip);

	// This keeps track of which events we've seen (so we only record the
	// most recent one. We record tempo changes, program changes and
	// controller changes.
	struct MKEY
		{
		ushort grfbitProgram;
		ushort grfbitChannelPressure;
		ushort grfbitPitchWheel;
		ushort fTempo: 1;

		ushort rggrfbitControl[120];
		};

	MKEY mkey;
	PMEV pmev;
	PMEV pmevMin;
	PMEV pmevLim;

	MEV rgmev[100];
	PMEV pmevDst;
	PMEV pmevLimDst;

	long cb;
	ulong dts;
	long igrfbit;
	ushort fbit;
	ushort *pgrfbit;
	byte bT;

	ClearPb(&mkey, size(mkey));
	ClearPb(rgmev, size(rgmev));

	if (pvNil == _pglmevKey && (pvNil == (_pglmevKey = GL::PglNew(size(MEV)))))
		return fFalse;
	_pglmevKey->FSetIvMac(0);

	pmevMin = (PMEV)pmdws->PvLockData(&cb);
	cb = LwRoundToward(cb, size(MEV));
	pmevLim = (PMEV)PvAddBv(pmevMin, cb);

	dts = 0;
	for (pmev = pmevMin; pmev < pmevLim; pmev++)
		{
		dts += pmev->dwDeltaTime;
		if (dts >= dtsSeek)
			break;
		}

	if (pmev + 1 >= pmevLim)
		{
		// dtsSeek goes past the end!
		goto LFail;
		}

	// get the destination pointer - this walks backwards
	pmevLimDst = rgmev + CvFromRgv(rgmev);
	pmevDst = pmevLimDst;

	// put the first event in the key frame list with a smaller time
	--pmevDst;
	pmevDst->dwDeltaTime = dts - dtsSeek;
	pmevDst->dwEvent = pmev->dwEvent;
	*pcbSkip = BvSubPvs(pmev + 1, pmevMin);

	for (;;)
		{
		if (pmevDst <= rgmev || pmev <= pmevMin)
			{
			// destination buffer is full - write it out
			long cmev, cmevNew;
			PMEV qrgmev;

			cmev = _pglmevKey->IvMac();
			cmevNew = pmevLimDst - pmevDst;

			if (!_pglmevKey->FSetIvMac(cmev + cmevNew))
				{
LFail:
				_pglmevKey->FSetIvMac(0);
				_pglmevKey->FEnsureSpace(0, fgrpShrink);
				pmdws->UnlockData();
				return fFalse;
				}

			qrgmev = (PMEV)_pglmevKey->QvGet(0);
			BltPb(qrgmev, qrgmev + cmevNew, LwMul(cmev, size(MEV)));
			CopyPb(pmevDst, qrgmev, LwMul(cmevNew, size(MEV)));

			if (pmev <= pmevMin)
				break;

			pmevDst = pmevLimDst;
			}

		--pmev;
		switch (pmev->dwEvent >> 24)
			{
		case MEVT_SHORTMSG:
			bT = (byte)pmev->dwEvent;

			// The high nibble of bT is the status value
			// The low nibble is the channel
			switch (bT & 0xF0)
				{
			case 0xB0: // control change
				igrfbit = BHigh(SuLow(pmev->dwEvent));
				if (!FIn(igrfbit, 0, CvFromRgv(mkey.rggrfbitControl)))
					break;
				pgrfbit = &mkey.rggrfbitControl[igrfbit];
				goto LTest;

			case 0xC0: // program change
				pgrfbit = &mkey.grfbitProgram;
				goto LTest;

			case 0xD0: // channel pressure
				pgrfbit = &mkey.grfbitChannelPressure;
				goto LTest;

			case 0xE0: // pitch wheel
				pgrfbit = &mkey.grfbitPitchWheel;
LTest:
				fbit = 1 << (bT & 0x0F);
				if (!(*pgrfbit & fbit))
					{
					// first time we've seen this event on this channel
					*pgrfbit |= fbit;
					goto LCopy;
					}
				break;
				}
			break;

		case MEVT_TEMPO:
			if (!mkey.fTempo)
				{
				mkey.fTempo = fTrue;
LCopy:
				pmevDst--;
				pmevDst->dwDeltaTime = 0;
				pmevDst->dwEvent = pmev->dwEvent;
				}
			break;
			}
		}

	_pglmevKey->FEnsureSpace(0, fgrpShrink);
	pmdws->UnlockData();
	return fTrue;
}


/***************************************************************************
	Call back from the midi stream stuff.
***************************************************************************/
void MSMIX::_MidiProc(ulong luUser, void *pvData, ulong luData)
{
	PMSMIX pmsmix;
	PMDWS pmdws;

	pmsmix = (PMSMIX)luUser;
	AssertPo(pmsmix, 0);
	pmdws = (PMDWS)luData;
	AssertNilOrPo(pmdws, 0);

	pmsmix->_Notify(pvData, pmdws);
}


/***************************************************************************
	The midi stream is done with the given header.
***************************************************************************/
void MSMIX::_Notify(void *pvData, PMDWS pmdws)
{
	AssertNilOrPo(pmdws, 0);
	MSOS msos;

	_mutx.Enter();

	Assert(_cpvOut > 0, "what buffer is this?");
	_cpvOut--;
	if (pvNil != pmdws)
		{
		AssertVar(_pglmsos->IvMac() > 0 &&
			((MSOS *)_pglmsos->QvGet(0))->pmdws == pmdws,
			"Wrong pmdws", &pmdws);
		pmdws->UnlockData();
		}
	else if (pvNil != _pglmevKey && pvData == _pglmevKey->QvGet(0))
		_pglmevKey->Unlock();
	else
		{
		Bug("Bad pvData/pmdws combo");
		}

	if (!_fPlaying)
		{
		// we don't need to notify or start the next sound.
		_mutx.Leave();
		return;
		}

	if (_fPlaying && _cpvOut == 0)
		{
		// all headers are in and we're supposed to be playing - so notify the
		// previous pmdws and start up the next one.
		_fPlaying = fFalse;
		if (0 < _pglmsos->IvMac())
			{
			_pglmsos->Get(0, &msos);
			_pglmsos->Delete(0);

			_mutx.Leave();

			// do the notify
			msos.pmsque->Notify(msos.pmdws);

			_mutx.Enter();
			}

		if (_pglmsos->IvMac() > 0)
			_Restart();
		}

	_mutx.Leave();
}


/***************************************************************************
	AT: Static method. Thread function for the MSMIX object.
***************************************************************************/
ulong __stdcall MSMIX::_ThreadProc(void *pv)
{
	PMSMIX pmsmix = (PMSMIX)pv;

	AssertPo(pmsmix, 0);

	return pmsmix->_LuThread();
}


/***************************************************************************
	AT: This thread just sleeps until the next sound is due to expire, then
	wakes up and nukes any expired sounds.
***************************************************************************/
ulong MSMIX::_LuThread(void)
{
	AssertThis(0);
	ulong tsCur;
	long imsos;
	MSOS msos;
	long cactSkip;
	ulong dtsNextStop = kluMax;

	for (;;)
		{
		WaitForSingleObject(_hevt, dtsNextStop);

		if (_fDone)
			return 0;

		_mutx.Enter();

		if (_fWaiting)
			{
			// we're waiting for buffers to be returned, so don't touch
			// anything!
			dtsNextStop = 1;
			}
		else
			{
			// See if any sounds have expired...
			tsCur = TsCurrentSystem();
			dtsNextStop = kluMax;
			for (imsos = _pglmsos->IvMac(); imsos-- > 0; )
				{
				if (imsos == 0 && _fPlaying)
					break;
				_pglmsos->Get(imsos, &msos);

				cactSkip = (tsCur - msos.tsStart) / msos.dts;
				if (cactSkip > 0)
					{
					ulong dtsSeek;

					if (msos.cactPlay > 0 && (msos.cactPlay -= cactSkip) <= 0)
						{
						// this sound is done
						_pglmsos->Delete(imsos);
						_mutx.Leave();
	
						// do the notify
						msos.pmsque->Notify(msos.pmdws);
	
						_mutx.Enter();
						dtsNextStop = 0;
						break;
						}

					// adjust the values in the MSOS
					dtsSeek = (tsCur - msos.tsStart) % msos.dts;
					msos.tsStart = tsCur - dtsSeek;
					_pglmsos->Put(imsos, &msos);
					}

				dtsNextStop = LuMin(dtsNextStop,
					msos.dts - (tsCur - msos.tsStart));
				}
			}

		_mutx.Leave();
		}
}


/***************************************************************************
	Constructor for the MIDI stream interface.
***************************************************************************/
MISI::MISI(PFNMIDI pfn, ulong luUser)
{
	AssertBaseThis(0);
	Assert(pvNil != pfn, 0);

	_pfnCall = pfn;
	_luUser = luUser;

	_tBogusDriver = tMaybe;
	_luVolSys = (ulong)(-1);
	_vlmBase = kvlmFull;
}


/***************************************************************************
	Reset the midi device.
***************************************************************************/
void MISI::_Reset(void)
{
	Assert(hNil != _hms, 0);
	long iv;

	midiOutReset(_hms);

	// Reset channel pressure and pitch wheel on all channels.
	// We shouldn't have to do this, but some drivers don't reset these.
	for (iv = 0; iv < 16; iv++)
		{
		midiOutShortMsg(_hms, 0xD0 | iv);
		midiOutShortMsg(_hms, 0x004000E0 | iv);
		}
}


/***************************************************************************
	Get the system volume level.
***************************************************************************/
void MISI::_GetSysVol(void)
{
	Assert(hNil != _hms, "calling _GetSysVol with nil _hms");
	ulong lu0, lu1, lu2;

	switch (_tBogusDriver)
		{
	case tYes:
		// just use vluSysVolFake...
		_luVolSys = vluSysVolFake;
		return;

	case tMaybe:
		// need to determine if midiOutGetVolume really works for this
		// driver.

		// Some drivers will only ever tell us what we last gave them -
		// irregardless of what the user has set the value to. Those drivers
		// will always give us full volume the first time we ask.

		// We also look for drivers that give us nonsense values.

		if (0 != midiOutGetVolume((uint)_hms, &_luVolSys) ||
				_luVolSys == (ulong)(-1) ||
			0 != midiOutSetVolume((uint)_hms, (ulong)0) ||
            0 != midiOutGetVolume((uint)_hms, &lu0) ||
			0 != midiOutSetVolume((uint)_hms, (ulong)0x7FFF7FFF) ||
            0 != midiOutGetVolume((uint)_hms, &lu1) ||
			0 != midiOutSetVolume((uint)_hms, (ulong)0xFFFFFFFF) ||
            0 != midiOutGetVolume((uint)_hms, &lu2) ||
			lu0 >= lu1 || lu1 >= lu2)
			{
			_tBogusDriver = tYes;
			_luVolSys = vluSysVolFake;
			}
		else
			{
			_tBogusDriver = tNo;
			vluSysVolFake = _luVolSys;
			}
		midiOutSetVolume((uint)_hms, _luVolSys);
		break;

	default:
		if (0 != midiOutGetVolume((uint)_hms, &_luVolSys))
			{
			// failed - use the fake value
			_luVolSys = vluSysVolFake;
			}
		else
			vluSysVolFake = _luVolSys;
		break;
		}
}


/***************************************************************************
	Set the system volume level.
***************************************************************************/
void MISI::_SetSysVol(ulong luVol)
{
	Assert(hNil != _hms, "calling _SetSysVol with nil _hms");
	midiOutSetVolume((uint)_hms, luVol);
}


/***************************************************************************
	Set the system volume level from the current values of _vlmBase
	and _luVolSys. We set the system volume to the result of scaling
	_luVolSys by _vlmBase.
***************************************************************************/
void MISI::_SetSysVlm(void)
{
	ulong luVol;

	luVol = LuVolScale(_luVolSys, _vlmBase);
	_SetSysVol(luVol);
}


/***************************************************************************
	Set the volume for the midi stream output device.
***************************************************************************/
void MISI::SetVlm(long vlm)
{
	AssertThis(0);

	if (vlm != _vlmBase)
		{
		_vlmBase = vlm;
		if (hNil != _hms)
			_SetSysVlm();
		}
}


/***************************************************************************
	Get the current volume.
***************************************************************************/
long MISI::VlmCur(void)
{
	AssertThis(0);

	return _vlmBase;
}


/***************************************************************************
	Return whether the midi stream output device is active.
***************************************************************************/
bool MISI::FActive(void)
{
	return hNil != _hms;
}


/***************************************************************************
	Activate or deactivate the Midi stream output object.
***************************************************************************/
bool MISI::FActivate(bool fActivate)
{
	AssertThis(0);

	return fActivate ? _FOpen() : _FClose();
}


/***************************************************************************
	Constructor for the Win95 Midi stream class.
***************************************************************************/
WMS::WMS(PFNMIDI pfn, ulong luUser) : MISI(pfn, luUser)
{
}


/***************************************************************************
	Destructor for the Win95 Midi stream class.
***************************************************************************/
WMS::~WMS(void)
{
	if (hNil != _hth)
		{
		// tell the thread to end and wait for it to finish
		_fDone = fTrue;
		SetEvent(_hevt);
		WaitForSingleObject(_hth, INFINITE);
		}

	if (hNil != _hevt)
		CloseHandle(_hevt);

	if (pvNil != _pglpmsir)
		{
		Assert(0 == _pglpmsir->IvMac(), "WMS still has some active buffers");
		ReleasePpo(&_pglpmsir);
		}
	if (hNil != _hlib)
		{
		FreeLibrary(_hlib);
		_hlib = hNil;
		}
}


/***************************************************************************
	Create a new WMS.
***************************************************************************/
PWMS WMS::PwmsNew(PFNMIDI pfn, ulong luUser)
{
	PWMS pwms;

	if (pvNil == (pwms = NewObj WMS(pfn, luUser)))
		return pvNil;

	if (!pwms->_FInit())
		ReleasePpo(&pwms);

	return pwms;
}


/***************************************************************************
	Initialize the WMS: get the addresses of the stream API.
***************************************************************************/
bool WMS::_FInit(void)
{
	OSVERSIONINFO osv;
	ulong luThread;

	// Make sure we're on Win95 and not NT, since the API exists on NT 3.51
	// but it fails.
	osv.dwOSVersionInfoSize = size(osv);
	if (!GetVersionEx(&osv))
		return fFalse;

// Old header files don't have this defined!
#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS 1
#endif //!VER_PLATFORM_WIN32_WINDOWS

	if (VER_PLATFORM_WIN32_WINDOWS != osv.dwPlatformId)
		{
		// don't bother trying - NT's scheduler works fine anyway.
		return fFalse;
		}

	if (hNil == (_hlib = LoadLibrary(PszLit("WINMM.DLL"))))
		return fFalse;

#define _Get(n) \
	if (pvNil == (*(void **)&_pfn##n = \
			(void *)GetProcAddress(_hlib, "midiStream" #n))) \
		{ \
		return fFalse; \
		}

	_Get(Open);
	_Get(Close);
	_Get(Property);
	_Get(Position);
	_Get(Out);
	_Get(Pause);
	_Get(Restart);
	_Get(Stop);

#undef _Get

	if (pvNil == (_pglpmsir = GL::PglNew(size(PMSIR))))
		return fFalse;
	_pglpmsir->SetMinGrow(1);

	if (hNil == (_hevt = CreateEvent(pvNil, fFalse, fFalse, pvNil)))
		return fFalse;

	// create the thread
	if (hNil == (_hth = CreateThread(pvNil, 1024, WMS::_ThreadProc,
			this, 0, &luThread)))
		{
		return fFalse;
		}

	AssertThis(0);
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a WMS.
***************************************************************************/
void WMS::AssertValid(ulong grf)
{
	WMS_PAR::AssertValid(0);
	Assert(hNil != _hlib, 0);
	long cpmsir;

	_mutx.Enter();
	Assert(hNil != _hevt, "nil event");
	Assert(hNil != _hth, "nil thread");
	AssertPo(_pglpmsir, 0);
	cpmsir = _pglpmsir->IvMac();
	Assert(_cmhOut >= 0, "negative _cmhOut");
	AssertIn(_ipmsirCur, 0, cpmsir + 1);
	_mutx.Leave();
}


/***************************************************************************
	Mark memory for the WMS.
***************************************************************************/
void WMS::MarkMem(void)
{
	AssertValid(0);
	PMSIR pmsir;
	long ipmsir;

	WMS_PAR::MarkMem();

	_mutx.Enter();
	for (ipmsir = _pglpmsir->IvMac(); ipmsir-- > 0; )
		{
		_pglpmsir->Get(ipmsir, &pmsir);
		AssertVarMem(pmsir);
		MarkPv(pmsir);
		}
	MarkMemObj(_pglpmsir);
	_mutx.Leave();
}
#endif //DEBUG


/***************************************************************************
	Opens the midi stream and sets the time division to 1000 ticks per
	quarter note. It is assumed that the midi data has a tempo record
	indicating 1 quarter note per second (1000000 microseconds per quarter).
	The end result is that ticks are milliseconds.
***************************************************************************/
bool WMS::_FOpen(void)
{
	AssertThis(0);

	// MIDIPROPTIMEDIV struct
	struct MT
		{
		DWORD cbStruct;
		DWORD dwTimeDiv;
		};

	MT mt;
	UINT uT = MIDI_MAPPER;

	_mutx.Enter();

	if (hNil != _hms)
		goto LDone;

	if (MMSYSERR_NOERROR != (*_pfnOpen)(&_hms, &uT, 1, (ulong)_MidiProc,
			(ulong)this, CALLBACK_FUNCTION))
		{
		goto LFail;
		}

	// We set the time division to 1000 ticks per beat, so clients can
	// use 1 beat per second and just use milliseconds for timing.
	// We also un-pause the stream.
	mt.cbStruct = size(mt);
	mt.dwTimeDiv = 1000;

	if (MMSYSERR_NOERROR != (*_pfnProperty)(_hms, (byte *)&mt,
			MIDIPROP_SET | MIDIPROP_TIMEDIV))
		{
		(*_pfnClose)(_hms);
LFail:
		_hms = hNil;
		_mutx.Leave();
		return fFalse;
		}

	// we know there are no buffers submitted
	AssertVar(_cmhOut == 0, "why is _cmhOut non-zero?", &_cmhOut);
	_cmhOut = 0;

	// get the system volume level
	_GetSysVol();

	// set our volume level
	_SetSysVlm();

LDone:
	_mutx.Leave();

	return fTrue;
}


/***************************************************************************
	Close the midi stream.
***************************************************************************/
bool WMS::_FClose(void)
{
	AssertThis(0);

	_mutx.Enter();

	if (hNil == _hms)
		{
		_mutx.Leave();
		return fTrue;
		}

	if (0 < _cmhOut)
		{
		BugVar("closing a stream that still has buffers!", &_cmhOut);
		_mutx.Leave();
		return fFalse;
		}

	// reset the device
	_Reset();

	// restore the volume level
	_SetSysVol(_luVolSys);

	// free the device
	(*_pfnClose)(_hms);
	_hms = hNil;

	_mutx.Leave();

	return fTrue;
}


#ifdef STREAM_BUG
/***************************************************************************
	Just return the value of our flag, not (hNil != _hms).
***************************************************************************/
bool WMS::FActive(void)
{
	return _fActive;
}


/***************************************************************************
	Need to set _fActive as well.
***************************************************************************/
bool WMS::FActivate(bool fActivate)
{
	bool fRet;

	fRet = WMS_PAR::FActivate(fActivate);
	if (fRet)
		_fActive = FPure(fActivate);
	return fRet;
}
#endif //STREAM_BUG


/***************************************************************************
	Reset the midi stream so it's ready to accept new input. Assumes we
	already have the mutx.
***************************************************************************/
void WMS::_ResetStream(void)
{
	if (!FActive())
		return;

#ifdef STREAM_BUG
	if (hNil == _hms)
		_FOpen();
	else
		{
		(*_pfnStop)(_hms);
		_FClose();
		_FOpen();
		}
#else //!STREAM_BUG
	(*_pfnStop)(_hms);
	_Reset();
#endif //!STREAM_BUG
}


/***************************************************************************
	This submits a buffer and restarts the midi stream. If the data is
	bigger than 64K, this (in conjunction with _Notify) deals with it.
***************************************************************************/
bool WMS::FQueueBuffer(void *pvData, long cb, long ibStart, long cactPlay,
	ulong luData)
{
	AssertThis(0);
	AssertPvCb(pvData, cb);
	AssertIn(ibStart, 0, cb);
	Assert(cb % size(MEV) == 0, "bad cb");
	Assert(ibStart % size(MEV) == 0, "bad cb");

	long ipmsir;
	PMSIR pmsir = pvNil;
	bool fRet = fTrue;

	_mutx.Enter();

	if (hNil == _hms)
		goto LFail;

	if (!FAllocPv((void **)&pmsir, size(MSIR), fmemClear, mprNormal))
		goto LFail;

	pmsir->pvData = pvData;
	pmsir->cb = cb;
	pmsir->cactPlay = cactPlay;
	pmsir->luData = luData;
	pmsir->ibNext = ibStart;

	if (_hms == hNil || !_pglpmsir->FAdd(&pmsir, &ipmsir))
		goto LFail;

	if (0 == _CmhSubmitBuffers() && ipmsir == 0)
		{
		// submitting the buffers failed
		_pglpmsir->Delete(0);
		_ipmsirCur = 0;
LFail:
		FreePpv((void **)pmsir);
		fRet = fFalse;
		}

	_mutx.Leave();

	return fRet;
}


/***************************************************************************
	Submits buffers. Assumes the _mutx is already ours.
***************************************************************************/
long WMS::_CmhSubmitBuffers(void)
{
	PMSIR pmsir;
	long cbMh;
	PMH pmh;
	long imh;
	long cmh = 0;

	while (_ipmsirCur < _pglpmsir->IvMac())
		{
		_pglpmsir->Get(_ipmsirCur, &pmsir);
		if (pmsir->ibNext >= pmsir->cb)
			{
			// see if the sound should be repeated
			if (pmsir->cactPlay == 1)
				{
				_ipmsirCur++;
				continue;
				}
			pmsir->cactPlay--;
			pmsir->ibNext = 0;
			}

		// see if one of the buffers is free
		for (imh = 0; ; imh++)
			{
			if (imh >= kcmhMsir)
				{
				// all buffers are busy
				Assert(_cmhOut >= kcmhMsir, 0);
				return cmh;
				}
			if (pmsir->rgibLim[imh] == 0)
				break;
			}

		// fill the buffer and submit it
		pmh = &pmsir->rgmh[imh];
		pmh->lpData = (byte *)PvAddBv(pmsir->pvData, pmsir->ibNext);
		cbMh = LwMin(pmsir->cb - pmsir->ibNext, kcbMaxWmsBuffer);
		pmh->dwBufferLength = cbMh;
		pmh->dwBytesRecorded = cbMh;
		pmh->dwUser = (ulong)pmsir;
		pmsir->ibNext += cbMh;
		pmsir->rgibLim[imh] = pmsir->ibNext;

		if (_FSubmit(pmh))
			cmh++;
		else
			{
			// just play the previous buffers and forget about this one
			pmsir->ibNext = pmsir->cb;
			pmsir->rgibLim[imh] = 0;
			pmsir->cactPlay = 1;
			_ipmsirCur++;
			}
		}

	return cmh;
}


/***************************************************************************
	Prepare and submit the given buffer. Assumes the mutx is ours.
***************************************************************************/
bool WMS::_FSubmit(PMH pmh)
{
	bool fRestart = (0 == _cmhOut);

	if (hNil == _hms)
		return fFalse;

	// prepare and submit the buffer
	if (MMSYSERR_NOERROR != midiOutPrepareHeader(_hms, (PMHO)pmh, sizeof(*pmh)))
		return fFalse;

	if (MMSYSERR_NOERROR != (*_pfnOut)(_hms, (PMHO)pmh, size(*pmh)))
		{
		midiOutUnprepareHeader(_hms, (PMHO)pmh, size(*pmh));
		return fFalse;
		}
	_cmhOut++;

	if (fRestart)
		(*_pfnRestart)(_hms);

	return fTrue;
}


/***************************************************************************
	Stop the midi stream.
***************************************************************************/
void WMS::StopPlaying(void)
{
	AssertThis(0);

	_mutx.Enter();

	if (hNil != _hms && _cmhOut > 0)
		{
		(*_pfnStop)(_hms);
		_ipmsirCur = _pglpmsir->IvMac();
		}

	_mutx.Leave();
}


/***************************************************************************
	Call back from the midi stream. If the number of active buffers returns
	to 0, this stops the midi stream. If the indicated sound is done,
	we notify the client.
***************************************************************************/
void __stdcall WMS::_MidiProc(HMS hms, ulong msg, ulong luUser,
	ulong lu1, ulong lu2)
{
	PWMS pwms;
	PMH pmh;

	if (msg != MOM_DONE)
		return;

	pwms = (PWMS)luUser;
	AssertPo(pwms, 0);
	pmh = (PMH)lu1;
	AssertVarMem(pmh);
	pwms->_Notify(hms, pmh);
}


/***************************************************************************
	The this-based callback.

	The mmsys guys claim that it's illegal to call midiOutUnprepareHeader,
	midiStreamStop and midiOutReset. So we just signal another thread to
	do this work.
***************************************************************************/
void WMS::_Notify(HMS hms, PMH pmh)
{
	AssertThis(0);
	Assert(hNil != hms, 0);
	AssertVarMem(pmh);

	PMSIR pmsir;
	long imh;

	_mutx.Enter();

	Assert(hms == _hms, "wrong hms");

	midiOutUnprepareHeader(hms, (PMHO)pmh, size(*pmh));
	pmsir = (PMSIR)pmh->dwUser;
	AssertVarMem(pmsir);
	AssertPvCb(pmsir->pvData, pmsir->cb);

	for (imh = 0; ; imh++)
		{
		if (imh >= kcmhMsir)
			{
			Bug("corrupt msir");
			_mutx.Leave();
			return;
			}

		if (pmh == &pmsir->rgmh[imh])
			break;
		}

	Assert(pmh->lpData ==
		PvAddBv(pmsir->pvData, pmsir->rgibLim[imh] - pmh->dwBufferLength),
		"pmh->lpData is wrong");

	// mark this buffer free
	pmsir->rgibLim[imh] = 0;

	// fill and submit buffers
	_CmhSubmitBuffers();

	// update the submitted buffer count
	--_cmhOut;

	// wake up the auxillary thread to do callbacks and stop and reset
	// the device it there's nothing more to play
	SetEvent(_hevt);

	_mutx.Leave();
}


/***************************************************************************
	AT: Static method. Thread function for the WMS object. This thread
	just waits for the event to be triggered, indicating that we got
	a callback from the midiStream stuff and it's time to do our callbacks.
***************************************************************************/
ulong __stdcall WMS::_ThreadProc(void *pv)
{
	PWMS pwms = (PWMS)pv;

	AssertPo(pwms, 0);

	return pwms->_LuThread();
}


/***************************************************************************
	AT: This thread just sleeps until the next sound is due to expire, then
	wakes up and nukes any expired sounds.
***************************************************************************/
ulong WMS::_LuThread(void)
{
	AssertThis(0);

	for (;;)
		{
		WaitForSingleObject(_hevt, INFINITE);

		if (_fDone)
			return 0;

		_mutx.Enter();

		if (hNil != _hms && 0 == _cmhOut)
			_ResetStream();

		_DoCallBacks();

		_mutx.Leave();
		}
}


/***************************************************************************
	Check for MSIRs that are done and do the callback on them and free them.
	Assumes the _mutx is checked out exactly once.
***************************************************************************/
void WMS::_DoCallBacks()
{
	PMSIR pmsir;

	AssertIn(_ipmsirCur, 0, _pglpmsir->IvMac() + 1);
	while (0 < _ipmsirCur)
		{
		_pglpmsir->Get(0, &pmsir);

		if (_cmhOut > 0)
			{
			// see if the MSIR is done
			long imh;

			for (imh = 0; imh < kcmhMsir; imh++)
				{
				if (0 < pmsir->rgibLim[imh])
					{
					// this one is busy
					return;
					}
				}
			}

		// this one is done
		_pglpmsir->Delete(0);
		_ipmsirCur--;
		_mutx.Leave();

		// notify the client that we're done with the sound
		(*_pfnCall)(_luUser, pmsir->pvData, pmsir->luData);
		FreePpv((void **)&pmsir);

		_mutx.Enter();
		AssertIn(_ipmsirCur, 0, _pglpmsir->IvMac() + 1);
		}
}


/***************************************************************************
	Constructor for our own midi stream api implementation.
***************************************************************************/
OMS::OMS(PFNMIDI pfn, ulong luUser) : MISI(pfn, luUser)
{
}


/***************************************************************************
	Destructor for our midi stream.
***************************************************************************/
OMS::~OMS(void)
{
	if (hNil != _hth)
		{
		// tell the thread to end and wait for it to finish
		_fDone = fTrue;
		SetEvent(_hevt);
		WaitForSingleObject(_hth, INFINITE);
		}

	_mutx.Enter();

	if (hNil != _hevt)
		CloseHandle(_hevt);

	Assert(_hms == hNil, "Still have an HMS");
	Assert(_pglmsb->IvMac() == 0, "Still have some buffers");
	ReleasePpo(&_pglmsb);

	_mutx.Leave();
}


/***************************************************************************
	Create a new OMS.
***************************************************************************/
POMS OMS::PomsNew(PFNMIDI pfn, ulong luUser)
{
	POMS poms;

	if (pvNil == (poms = NewObj OMS(pfn, luUser)))
		return pvNil;

	if (!poms->_FInit())
		ReleasePpo(&poms);

	return poms;
}


/***************************************************************************
	Initialize the OMS.
***************************************************************************/
bool OMS::_FInit(void)
{
	AssertBaseThis(0);
	ulong luThread;

	if (pvNil == (_pglmsb = GL::PglNew(size(MSB))))
		return fFalse;
	_pglmsb->SetMinGrow(1);

	if (hNil == (_hevt = CreateEvent(pvNil, fFalse, fFalse, pvNil)))
		return fFalse;

	// create the thread in a suspended state
	if (hNil == (_hth = CreateThread(pvNil, 1024, OMS::_ThreadProc,
			this, CREATE_SUSPENDED, &luThread)))
		{
		return fFalse;
		}

	SetThreadPriority(_hth, THREAD_PRIORITY_TIME_CRITICAL);

	// start the thread
	ResumeThread(_hth);

	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a OMS.
***************************************************************************/
void OMS::AssertValid(ulong grf)
{
	OMS_PAR::AssertValid(0);

	_mutx.Enter();
	Assert(hNil != _hth, "nil thread");
	Assert(hNil != _hevt, "nil event");
	AssertPo(_pglmsb, 0);
	_mutx.Leave();
}


/***************************************************************************
	Mark memory for the OMS.
***************************************************************************/
void OMS::MarkMem(void)
{
	AssertValid(0);
	OMS_PAR::MarkMem();

	_mutx.Enter();
	MarkMemObj(_pglmsb);
	_mutx.Leave();
}
#endif //DEBUG


/***************************************************************************
	Open the stream.
***************************************************************************/
bool OMS::_FOpen(void)
{
	AssertThis(0);

	_mutx.Enter();

	if (hNil != _hms)
		goto LDone;

	_fChanged = _fStop = fFalse;
	if (MMSYSERR_NOERROR != midiOutOpen(&_hms, MIDI_MAPPER,
			0, 0, CALLBACK_NULL))
		{
		_hms = hNil;
		_mutx.Leave();
		return fFalse;
		}

	// get the system volume level
	_GetSysVol();

	// set our volume level
	_SetSysVlm();

LDone:
	_mutx.Leave();

	return fTrue;
}


/***************************************************************************
	Close the stream.
***************************************************************************/
bool OMS::_FClose(void)
{
	AssertThis(0);

	_mutx.Enter();

	if (hNil == _hms)
		{
		_mutx.Leave();
		return fTrue;
		}

	if (_pglmsb->IvMac() > 0)
		{
		Bug("closing a stream that still has buffers!");
		_mutx.Leave();
		return fFalse;
		}

	// reset the device
	_Reset();

	// restore the volume level
	_SetSysVol(_luVolSys);

	midiOutClose(_hms);
	_hms = hNil;

	_mutx.Leave();

	return fTrue;
}


/***************************************************************************
	Queue a buffer to the midi stream.
***************************************************************************/
bool OMS::FQueueBuffer(void *pvData, long cb, long ibStart, long cactPlay,
	ulong luData)
{
	AssertThis(0);
	AssertPvCb(pvData, cb);
	AssertIn(ibStart, 0, cb);
	Assert(cb % size(MEV) == 0, "bad cb");
	Assert(ibStart % size(MEV) == 0, "bad cb");

	MSB msb;

	_mutx.Enter();

	if (hNil == _hms)
		goto LFail;

	msb.pvData = pvData;
	msb.cb = cb;
	msb.ibStart = ibStart;
	msb.cactPlay = cactPlay;
	msb.luData = luData;

	if (!_pglmsb->FAdd(&msb))
		{
LFail:
		_mutx.Leave();
		return fFalse;
		}

	if (1 == _pglmsb->IvMac())
		{
		// Start the buffer
		SetEvent(_hevt);
		_fChanged = fTrue;
		}

	_mutx.Leave();

	return fTrue;
}


/***************************************************************************
	Stop the stream and release all buffers. The buffer notifies are
	asynchronous.
***************************************************************************/
void OMS::StopPlaying(void)
{
	AssertThis(0);

	_mutx.Enter();

	if (hNil != _hms)
		{
		_fStop = fTrue;
		SetEvent(_hevt);
		_fChanged = fTrue;
		}

	_mutx.Leave();
}


/***************************************************************************
	AT: Static method. Thread function for the midi stream object.
***************************************************************************/
ulong __stdcall OMS::_ThreadProc(void *pv)
{
	POMS poms = (POMS)pv;

	AssertPo(poms, 0);

	return poms->_LuThread();
}


/***************************************************************************
	AT: The midi stream playback thread.
***************************************************************************/
ulong OMS::_LuThread(void)
{
	AssertThis(0);
	MSB msb;
	bool fChanged;		// whether the event went off
	ulong tsCur;
	const long klwInfinite = klwMax;
	long dtsWait = klwInfinite;

	for (;;)
		{
		fChanged = dtsWait > 0 && WAIT_TIMEOUT != WaitForSingleObject(_hevt,
			dtsWait == klwInfinite ? INFINITE : dtsWait);

		if (_fDone)
			return 0;

		_mutx.Enter();
		if (_fChanged && !fChanged)
			{
			// the event went off before we got the mutx.
			dtsWait = klwInfinite;
			goto LLoop;
			}

		_fChanged = fFalse;
		if (!fChanged)
			{
			// play the event
			if (_pmev < _pmevLim)
				{
				if (MEVT_SHORTMSG == (_pmev->dwEvent >> 24))
					midiOutShortMsg(_hms, _pmev->dwEvent & 0x00FFFFFF);

				_pmev++;
				if (_pmev >= _pmevLim)
					dtsWait = 0;
				else
					{
					ulong tsNew = TsCurrentSystem();

					tsCur += _pmev->dwDeltaTime;
					dtsWait = tsCur - tsNew;
					if (dtsWait < -kdtsMinSlip)
						{
						tsCur = tsNew;
						dtsWait = 0;
						}
					}
				goto LLoop;
				}

			// ran out of events in the current buffer - see if we should
			// repeat it
			_pglmsb->Get(0, &msb);
			if (msb.cactPlay == 1)
				{
				_imsbCur = 1;
				_ReleaseBuffers();
				}
			else
				{
				// repeat the current buffer
				if (msb.cactPlay > 0)
					msb.cactPlay--;
				msb.ibStart = 0;
				_pglmsb->Put(0, &msb);
				}
			}
		else if (_fStop)
			{
			// release all buffers
			_fStop = fFalse;
			_imsbCur = _pglmsb->IvMac();
			_ReleaseBuffers();
			}

		if (0 == _pglmsb->IvMac())
			{
			// no buffers to play
			dtsWait = klwInfinite;
			}
		else
			{
			// start playing the new buffers
			_pglmsb->Get(0, &msb);
			_pmev = (PMEV)PvAddBv(msb.pvData, msb.ibStart);
			_pmevLim = (PMEV)PvAddBv(msb.pvData, msb.cb);
			if (_pmev >= _pmevLim)
				dtsWait = 0;
			else
				{
				dtsWait = _pmev->dwDeltaTime;
				tsCur = TsCurrentSystem() + dtsWait;
				}
			}
LLoop:
		_mutx.Leave();
		}
}


/***************************************************************************
	Release all buffers up to _imsbCur. Assumes that we have the mutx
	checked out exactly once.
***************************************************************************/
void OMS::_ReleaseBuffers(void)
{
	MSB msb;

	if (_imsbCur >= _pglmsb->IvMac() && hNil != _hms)
		_Reset();

	while (_imsbCur > 0)
		{
		_pglmsb->Get(0, &msb);
		_pglmsb->Delete(0);
		_imsbCur--;

		_mutx.Leave();

		// call the notify proc
		(*_pfnCall)(_luUser, msb.pvData, msb.luData);

		_mutx.Enter();
		}
}


