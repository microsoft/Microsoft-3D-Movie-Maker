/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Sound manager class implementation.

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(SNDV)
RTCLASS(SNDM)
RTCLASS(SNDMQ)
RTCLASS(SNQUE)

long SNDV::_siiLast;


/***************************************************************************
	This is the volume to use as the system volume if it is determined
	by a device that the system lies when asked what the current
	volume setting is. Eg, for many drivers, midiOutGetVolume on Win95
	always returns full volume. When we determine that this is the case,
	we'll use this value instead. Any device that determines that its
	reading is valid should set this so other devices can use the same
	value.
***************************************************************************/
ulong vluSysVolFake = (ulong)-1;


/***************************************************************************
	Start a synchronized group.
***************************************************************************/
void SNDV::BeginSynch(void)
{
}


/***************************************************************************
	End a synchronized group.
***************************************************************************/
void SNDV::EndSynch(void)
{
}


/***************************************************************************
	Constructor for the sound manager.
***************************************************************************/
SNDM::SNDM(void)
{
}


/***************************************************************************
	Destructor for the sound manager.
***************************************************************************/
SNDM::~SNDM(void)
{
	AssertBaseThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	if (pvNil != _pglsndmpe)
		{
		for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
			{
			_pglsndmpe->Get(isndmpe, &sndmpe);
			ReleasePpo(&sndmpe.psndv);
			}
		ReleasePpo(&_pglsndmpe);
		}
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a SNDM.
***************************************************************************/
void SNDM::AssertValid(ulong grf)
{
	SNDM_PAR::AssertValid(0);
	AssertPo(_pglsndmpe, 0);
}


/***************************************************************************
	Mark memory for the SNDM.
***************************************************************************/
void SNDM::MarkMem(void)
{
	AssertValid(0);
	long isndmpe;
	SNDMPE sndmpe;

	SNDM_PAR::MarkMem();
	MarkMemObj(_pglsndmpe);
	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		MarkMemObj(sndmpe.psndv);
		}
}
#endif //DEBUG


/***************************************************************************
	Create the sound manager.
***************************************************************************/
PSNDM SNDM::PsndmNew(void)
{
	PSNDM psndm;

	if (pvNil == (psndm = NewObj SNDM))
		return pvNil;

	if (!psndm->_FInit())
		ReleasePpo(&psndm);

	AssertNilOrPo(psndm, 0);
	return psndm;
}


/***************************************************************************
	Initialize the sound manager.
***************************************************************************/
bool SNDM::_FInit(void)
{
	AssertBaseThis(0);

	if (pvNil == (_pglsndmpe = GL::PglNew(size(SNDMPE))))
		return fFalse;

	_pglsndmpe->SetMinGrow(1);
	_fActive = fTrue;

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Find the device that sounds of the given ctg are to be played on.
***************************************************************************/
bool SNDM::_FFindCtg(CTG ctg, SNDMPE *psndmpe, long *pisndmpe)
{
	AssertThis(0);
	AssertNilOrVarMem(psndmpe);
	AssertNilOrVarMem(pisndmpe);
	long isndmpe;
	SNDMPE sndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		if (sndmpe.ctg == ctg)
			{
			if (pvNil != psndmpe)
				*psndmpe = sndmpe;
			if (pvNil != pisndmpe)
				*pisndmpe = isndmpe;
			return fTrue;
			}
		}

	TrashVar(psndmpe);
	if (pvNil != pisndmpe)
		*pisndmpe = _pglsndmpe->IvMac();
	return fFalse;
}


/***************************************************************************
	Add a device to the device map to handle the particular ctg.
***************************************************************************/
bool SNDM::FAddDevice(CTG ctg, PSNDV psndv)
{
	AssertThis(0);
	AssertPo(psndv, 0);
	SNDMPE sndmpe;
	long isndmpe;
	long cact;

	psndv->AddRef();
	if (_FFindCtg(ctg, &sndmpe, &isndmpe))
		{
		ReleasePpo(&sndmpe.psndv);
		sndmpe.psndv = psndv;
		_pglsndmpe->Put(isndmpe, &sndmpe);
		}
	else
		{
		sndmpe.ctg = ctg;
		sndmpe.psndv = psndv;
		if (!_pglsndmpe->FInsert(isndmpe, &sndmpe))
			{
			ReleasePpo(&psndv);
			return fFalse;
			}
		}

	psndv->Activate(_fActive);
	for (cact = 0; cact < _cactSuspend; cact++)
		psndv->Suspend(fTrue);
	return fTrue;
}


/***************************************************************************
	Return the sound device that is registered for the given ctg.
***************************************************************************/
PSNDV SNDM::PsndvFromCtg(CTG ctg)
{
	AssertThis(0);
	SNDMPE sndmpe;

	if (!_FFindCtg(ctg, &sndmpe))
		return pvNil;

	return sndmpe.psndv;
}


/***************************************************************************
	Remove the sound device for the given ctg.
***************************************************************************/
void SNDM::RemoveSndv(CTG ctg)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	if (!_FFindCtg(ctg, &sndmpe, &isndmpe))
		return;

	_pglsndmpe->Delete(isndmpe);
	ReleasePpo(&sndmpe.psndv);
}


/***************************************************************************
	Return whether the sound manager is active.
***************************************************************************/
bool SNDM::FActive(void)
{
	AssertThis(0);

	return _fActive;
}


/***************************************************************************
	Activate or deactivate the sound manager.
***************************************************************************/
void SNDM::Activate(bool fActive)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	if (FPure(fActive) == FPure(_fActive))
		return;

	_fActive = FPure(fActive);
	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->Activate(fActive);
		}
}


/***************************************************************************
	Suspend or resume the sound manager.
***************************************************************************/
void SNDM::Suspend(bool fSuspend)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	if (fSuspend)
		_cactSuspend++;
	else
		_cactSuspend--;

	Assert(_cactSuspend >= FPure(fSuspend), "bad _cactSuspend");
	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->Suspend(fSuspend);
		}
}


/***************************************************************************
	Set the volume of all the devices.
***************************************************************************/
void SNDM::SetVlm(long vlm)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->SetVlm(vlm);
		}
}


/***************************************************************************
	Get the max of the volumes of all the devices.
***************************************************************************/
long SNDM::VlmCur(void)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;
	long vlm;

	vlm = 0;
	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		vlm = LwMax(vlm, sndmpe.psndv->VlmCur());
		}

	return vlm;
}


/***************************************************************************
	Play the given sound.
***************************************************************************/
long SNDM::SiiPlay(PRCA prca, CTG ctg, CNO cno, long sqn, long vlm,
	long cactPlay, ulong dtsStart, long spr, long scl)
{
	AssertThis(0);
	AssertPo(prca, 0);
	SNDMPE sndmpe;

	if (!_FFindCtg(ctg, &sndmpe))
		return _SiiAlloc();

	return sndmpe.psndv->SiiPlay(prca, ctg, cno, sqn, vlm, cactPlay, dtsStart,
		spr, scl);
}


/***************************************************************************
	Stop the given sound instance.
***************************************************************************/
void SNDM::Stop(long sii)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->Stop(sii);
		}
}


/***************************************************************************
	Stop all sounds of the given queue and class (one or both may be nil).
***************************************************************************/
void SNDM::StopAll(long sqn, long scl)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->StopAll(sqn, scl);
		}
}


/***************************************************************************
	Pause the given sound.
***************************************************************************/
void SNDM::Pause(long sii)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->Pause(sii);
		}
}


/***************************************************************************
	Pause all sounds of the given queue and class (one or both may be nil).
***************************************************************************/
void SNDM::PauseAll(long sqn, long scl)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->PauseAll(sqn, scl);
		}
}


/***************************************************************************
	Resume the given sound.
***************************************************************************/
void SNDM::Resume(long sii)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->Resume(sii);
		}
}


/***************************************************************************
	Resume all sounds of the given queue and class (one or both may be nil).
***************************************************************************/
void SNDM::ResumeAll(long sqn, long scl)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->ResumeAll(sqn, scl);
		}
}


/***************************************************************************
	Return whether the given sound is playing.
***************************************************************************/
bool SNDM::FPlaying(long sii)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		if (sndmpe.psndv->FPlaying(sii))
			return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	Return whether any sounds of the given queue and class are playing
	(one or both of (sqn, scl) may be nil).
***************************************************************************/
bool SNDM::FPlayingAll(long sqn, long scl)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		if (sndmpe.psndv->FPlayingAll(sqn, scl))
			return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	Free anything that's no longer in use.
***************************************************************************/
void SNDM::Flush(void)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->Flush();
		}
}


/***************************************************************************
	Start a synchronized group.
***************************************************************************/
void SNDM::BeginSynch(void)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->BeginSynch();
		}
}


/***************************************************************************
	End a synchronized group.
***************************************************************************/
void SNDM::EndSynch(void)
{
	AssertThis(0);
	SNDMPE sndmpe;
	long isndmpe;

	for (isndmpe = 0; isndmpe < _pglsndmpe->IvMac(); isndmpe++)
		{
		_pglsndmpe->Get(isndmpe, &sndmpe);
		sndmpe.psndv->EndSynch();
		}
}


/***************************************************************************
	A convenient base class for a multiple queue sound device.
***************************************************************************/

/***************************************************************************
	Destructor for a multiple queue sound device.
***************************************************************************/
SNDMQ::~SNDMQ(void)
{
	AssertBaseThis(0);
	long isnqd;
	SNQD snqd;

	if (pvNil != _pglsnqd)
		{
		for (isnqd = 0; isnqd < _pglsnqd->IvMac(); isnqd++)
			{
			_pglsnqd->Get(isnqd, &snqd);
			ReleasePpo(&snqd.psnque);
			}
		ReleasePpo(&_pglsnqd);
		}
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a SNDMQ.
***************************************************************************/
void SNDMQ::AssertValid(ulong grf)
{
	SNDMQ_PAR::AssertValid(0);
	AssertPo(_pglsnqd, 0);
}


/***************************************************************************
	Mark memory for the SNDMQ.
***************************************************************************/
void SNDMQ::MarkMem(void)
{
	AssertValid(0);
	long isnqd;
	SNQD snqd;

	SNDMQ_PAR::MarkMem();
	MarkMemObj(_pglsnqd);
	for (isnqd = 0; isnqd < _pglsnqd->IvMac(); isnqd++)
		{
		_pglsnqd->Get(isnqd, &snqd);
		MarkMemObj(snqd.psnque);
		}
}
#endif //DEBUG


/***************************************************************************
	Initialize the multiple queue device.
***************************************************************************/
bool SNDMQ::_FInit(void)
{
	AssertBaseThis(0);

	if (pvNil == (_pglsnqd = GL::PglNew(size(SNQD))))
		return fFalse;

	_fActive = fTrue;
	AssertThis(0);

	return fTrue;
}


/***************************************************************************
	Ensure a queue exists for the given sqn and return it.
***************************************************************************/
bool SNDMQ::_FEnsureQueue(long sqn, SNQD *psnqd, long *pisnqd)
{
	AssertThis(0);
	AssertNilOrVarMem(psnqd);
	AssertNilOrVarMem(pisnqd);

	long isnqd, isnqdEmpty;
	SNQD snqd;

	isnqdEmpty = ivNil;
	for (isnqd = _pglsnqd->IvMac(); isnqd-- > 0; )
		{
		_pglsnqd->Get(isnqd, &snqd);
		if (snqd.sqn == sqn && sqn != ksqnNone)
			{
			// we found it
			goto LFound;
			}

		if (ivNil == isnqdEmpty && !snqd.psnque->FPlayingAll())
			{
			// here's an empty queue
			isnqdEmpty = isnqd;
			if (sqn == ksqnNone)
				break;
			}
		}

	if (ivNil != (isnqd = isnqdEmpty))
		{
		_pglsnqd->Get(isnqd, &snqd);
		snqd.sqn = sqn;
		_pglsnqd->Put(isnqd, &snqd);
		goto LFound;
		}

	snqd.sqn = sqn;
	if (pvNil == (snqd.psnque = _PsnqueNew()) ||
			!_pglsnqd->FAdd(&snqd, &isnqd))
		{
		ReleasePpo(&snqd.psnque);
		TrashVar(psnqd);
		TrashVar(pisnqd);
		return fFalse;
		}

LFound:
	if (pvNil != psnqd)
		*psnqd = snqd;
	if (pvNil != pisnqd)
		*pisnqd = isnqd;
	return fTrue;
}


/***************************************************************************
	Return whether the device is active.
***************************************************************************/
bool SNDMQ::FActive(void)
{
	AssertThis(0);

	return _fActive;
}


/***************************************************************************
	Activate or deactivate the device.
***************************************************************************/
void SNDMQ::Activate(bool fActive)
{
	AssertThis(0);

	if (FPure(fActive) == FPure(_fActive))
		return;

	_fActive = FPure(fActive);
	_Suspend(_cactSuspend > 0 || !_fActive);
}


/***************************************************************************
	Suspend or resume the device.
***************************************************************************/
void SNDMQ::Suspend(bool fSuspend)
{
	AssertThis(0);

	if (fSuspend)
		_cactSuspend++;
	else
		_cactSuspend--;

	Assert(_cactSuspend >= FPure(fSuspend), "bad _cactSuspend");
	_Suspend(_cactSuspend > 0 || !_fActive);
}


/***************************************************************************
	Play the given sound.
***************************************************************************/
long SNDMQ::SiiPlay(PRCA prca, CTG ctg, CNO cno, long sqn, long vlm,
	long cactPlay, ulong dtsStart, long spr, long scl)
{
	AssertThis(0);
	AssertPo(prca, 0);
	long isnqd;
	SNQD snqd;
	long sii = _SiiAlloc();

	if (sqn == sqnNil)
		sqn = ksqnNone;

	if (_FEnsureQueue(sqn, &snqd, &isnqd))
		{
		snqd.psnque->Enqueue(sii, prca, ctg, cno, vlm, cactPlay, dtsStart,
			spr, scl);
		}
	else
		Warn("couldn't allocate queue");

	return sii;
}


/***************************************************************************
	Stop the given sound instance.
***************************************************************************/
void SNDMQ::Stop(long sii)
{
	AssertThis(0);
	long isnqd;
	SNQD snqd;

	for (isnqd = 0; isnqd < _pglsnqd->IvMac(); isnqd++)
		{
		_pglsnqd->Get(isnqd, &snqd);
		snqd.psnque->Stop(sii);
		}
}


/***************************************************************************
	Stop all sounds of the given queue and class (one or both may be nil).
***************************************************************************/
void SNDMQ::StopAll(long sqn, long scl)
{
	AssertThis(0);
	long isnqd;
	SNQD snqd;

	for (isnqd = 0; isnqd < _pglsnqd->IvMac(); isnqd++)
		{
		_pglsnqd->Get(isnqd, &snqd);
		if (sqnNil == sqn || snqd.sqn == sqn)
			snqd.psnque->StopAll(scl);
		}
}


/***************************************************************************
	Pause the given sound.
***************************************************************************/
void SNDMQ::Pause(long sii)
{
	AssertThis(0);
	long isnqd;
	SNQD snqd;

	for (isnqd = 0; isnqd < _pglsnqd->IvMac(); isnqd++)
		{
		_pglsnqd->Get(isnqd, &snqd);
		snqd.psnque->Pause(sii);
		}
}


/***************************************************************************
	Pause all sounds of the given queue and class (one or both may be nil).
***************************************************************************/
void SNDMQ::PauseAll(long sqn, long scl)
{
	AssertThis(0);
	long isnqd;
	SNQD snqd;

	for (isnqd = 0; isnqd < _pglsnqd->IvMac(); isnqd++)
		{
		_pglsnqd->Get(isnqd, &snqd);
		if (sqnNil == sqn || snqd.sqn == sqn)
			snqd.psnque->PauseAll(scl);
		}
}


/***************************************************************************
	Resume the given sound.
***************************************************************************/
void SNDMQ::Resume(long sii)
{
	AssertThis(0);
	long isnqd;
	SNQD snqd;

	for (isnqd = 0; isnqd < _pglsnqd->IvMac(); isnqd++)
		{
		_pglsnqd->Get(isnqd, &snqd);
		snqd.psnque->Resume(sii);
		}
}


/***************************************************************************
	Resume all sounds of the given queue and class (one or both may be nil).
***************************************************************************/
void SNDMQ::ResumeAll(long sqn, long scl)
{
	AssertThis(0);
	long isnqd;
	SNQD snqd;

	for (isnqd = 0; isnqd < _pglsnqd->IvMac(); isnqd++)
		{
		_pglsnqd->Get(isnqd, &snqd);
		if (sqnNil == sqn || snqd.sqn == sqn)
			snqd.psnque->ResumeAll(scl);
		}
}


/***************************************************************************
	Return whether the given sound is playing.
***************************************************************************/
bool SNDMQ::FPlaying(long sii)
{
	AssertThis(0);
	long isnqd;
	SNQD snqd;

	for (isnqd = 0; isnqd < _pglsnqd->IvMac(); isnqd++)
		{
		_pglsnqd->Get(isnqd, &snqd);
		if (snqd.psnque->FPlaying(sii))
			return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	Return whether any sounds of the given queue and class are playing
	(one or both of (sqn, scl) may be nil).
***************************************************************************/
bool SNDMQ::FPlayingAll(long sqn, long scl)
{
	AssertThis(0);
	long isnqd;
	SNQD snqd;

	for (isnqd = 0; isnqd < _pglsnqd->IvMac(); isnqd++)
		{
		_pglsnqd->Get(isnqd, &snqd);
		if ((sqnNil == sqn ||
				snqd.sqn == sqn) && snqd.psnque->FPlayingAll(scl))
			{
			return fTrue;
			}
		}

	return fFalse;
}


/***************************************************************************
	Free anything that's no longer being used.
***************************************************************************/
void SNDMQ::Flush()
{
	AssertThis(0);
	long isnqd;
	SNQD snqd;

	// Don't free the last channel
	for (isnqd = _pglsnqd->IvMac(); isnqd-- > 0; )
		{
		_pglsnqd->Get(isnqd, &snqd);
		snqd.psnque->Flush();
		if (!snqd.psnque->FPlayingAll() && _pglsnqd->IvMac() > 1)
			{
			ReleasePpo(&snqd.psnque);
			_pglsnqd->Delete(isnqd);
			}
		}
}


/***************************************************************************
	Constructor for an sound queue.
***************************************************************************/
SNQUE::SNQUE(void)
{
}


/***************************************************************************
	Destructor for an sound queue.
***************************************************************************/
SNQUE::~SNQUE(void)
{
	AssertBaseThis(0);

	_Enter();
	if (pvNil != _pglsndin)
		{
		long isndin;
		SNDIN sndin;

		for (isndin = _pglsndin->IvMac(); isndin-- > 0; )
			{
			_pglsndin->Get(isndin, &sndin);
			ReleasePpo(&sndin.pbaco);
			}
		ReleasePpo(&_pglsndin);
		}
	_Leave();
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a SNQUE.
***************************************************************************/
void SNQUE::AssertValid(ulong grf)
{
	SNQUE_PAR::AssertValid(0);

	_Enter();
	AssertPo(_pglsndin, 0);
	AssertIn(_isndinCur, 0, _pglsndin->IvMac() + 1);
	_Leave();
}


/***************************************************************************
	Mark memory for the SNQUE.
***************************************************************************/
void SNQUE::MarkMem(void)
{
	AssertValid(0);
	long isndin;
	SNDIN sndin;

	SNQUE_PAR::MarkMem();

	_Enter();
	MarkMemObj(_pglsndin);
	for (isndin = 0; isndin < _pglsndin->IvMac(); isndin++)
		{
		_pglsndin->Get(isndin, &sndin);
		MarkMemObj(sndin.pbaco);
		}
	_Leave();
}
#endif //DEBUG


/***************************************************************************
	Initialize the sound queue. Allocate the _pglsndin.
***************************************************************************/
bool SNQUE::_FInit(void)
{
	AssertBaseThis(0);

	if (pvNil == (_pglsndin = GL::PglNew(size(SNDIN))))
		return fFalse;

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Enter any critical section that's necessary to ensure access to
	member variables.
***************************************************************************/
void SNQUE::_Enter(void)
{
	AssertBaseThis(0);
}


/***************************************************************************
	Leave any critical section that's necessary to ensure access to
	member variables.
***************************************************************************/
void SNQUE::_Leave(void)
{
	AssertBaseThis(0);
}


/***************************************************************************
	Free any sounds below _isndinCur.
***************************************************************************/
void SNQUE::_Flush(void)
{
	AssertThis(0);
	SNDIN sndin;

	_Enter();

	while (_isndinCur > 0)
		{
		_isndinCur--;
		_pglsndin->Get(_isndinCur, &sndin);
		_pglsndin->Delete(_isndinCur);
		ReleasePpo(&sndin.pbaco);
		}

	_Leave();
}


/***************************************************************************
	Put the given sound on the queue.
***************************************************************************/
void SNQUE::Enqueue(long sii, PRCA prca, CTG ctg, CNO cno, long vlm,
	long cactPlay, ulong dtsStart, long spr, long scl)
{
	AssertThis(0);
	AssertPo(prca, 0);
	SNDIN sndin;
	long isndin;

	if (pvNil == (sndin.pbaco = _PbacoFetch(prca, ctg, cno)))
		return;

	_Enter();
	_Flush();

	sndin.sii = sii;
	sndin.vlm = vlm;
	sndin.cactPlay = cactPlay;
	sndin.dtsStart = dtsStart;
	sndin.spr = spr;
	sndin.scl = scl;
	sndin.cactPause = 0;
	if (!_pglsndin->FAdd(&sndin, &isndin))
		{
		ReleasePpo(&sndin.pbaco);
		_Leave();
		return;
		}

	_Queue(isndin);

	_Leave();
}


/***************************************************************************
	Return the priority of the frontmost sound in the queue.
***************************************************************************/
long SNQUE::SprCur(void)
{
	AssertThis(0);
	SNDIN sndin;
	long spr;

	_Enter();
	_Flush();

	if (_pglsndin->IvMac() >= _isndinCur)
		spr = klwMin;
	else
		{
		_pglsndin->Get(_isndinCur, &sndin);
		spr = sndin.spr;
		}

	_Leave();
	return spr;
}


/***************************************************************************
	If the given sound is in our queue, nuke it.
***************************************************************************/
void SNQUE::Stop(long sii)
{
	AssertThis(0);
	long isndin;
	SNDIN sndin;

	_Enter();
	_Flush();

	for (isndin = _pglsndin->IvMac(); isndin-- > _isndinCur; )
		{
		_pglsndin->Get(isndin, &sndin);
		if (sndin.sii == sii)
			{
			if (0 <= sndin.cactPause)
				{
				sndin.cactPause = -1;
				_pglsndin->Put(isndin, &sndin);
				_Queue(isndin);
				}

			_Leave();
			return;
			}
		}

	_Leave();
}


/***************************************************************************
	Nuke all sounds of the given sound class. sclNil means nuke all.
***************************************************************************/
void SNQUE::StopAll(long scl)
{
	AssertThis(0);
	long isndin;
	SNDIN sndin;
	long isndinMin = klwMax;

	_Enter();
	_Flush();

	for (isndin = _pglsndin->IvMac(); isndin-- > _isndinCur; )
		{
		_pglsndin->Get(isndin, &sndin);
		if (0 <= sndin.cactPause && (sclNil == scl || sndin.scl == scl))
			{
			sndin.cactPause = -1;
			_pglsndin->Put(isndin, &sndin);
			isndinMin = isndin;
			}
		}

	if (isndinMin < klwMax)
		_Queue(isndinMin);

	_Leave();
}


/***************************************************************************
	If the given sound is in our queue, pause it.
***************************************************************************/
void SNQUE::Pause(long sii)
{
	AssertThis(0);
	long isndin;
	SNDIN sndin;

	_Enter();
	_Flush();

	for (isndin = _pglsndin->IvMac(); isndin-- > _isndinCur; )
		{
		_pglsndin->Get(isndin, &sndin);
		if (sndin.sii == sii)
			{
			if (0 <= sndin.cactPause)
				{
				sndin.cactPause++;
				_pglsndin->Put(isndin, &sndin);
				if (1 == sndin.cactPause)
					_PauseQueue(isndin);
				}

			_Leave();
			return;
			}
		}

	_Leave();
}


/***************************************************************************
	Pause all sounds of the given sound class. sclNil means nuke all.
***************************************************************************/
void SNQUE::PauseAll(long scl)
{
	AssertThis(0);
	long isndin;
	SNDIN sndin;
	long isndinMin = klwMax;

	_Enter();
	_Flush();

	for (isndin = _pglsndin->IvMac(); isndin-- > _isndinCur; )
		{
		_pglsndin->Get(isndin, &sndin);
		if (0 <= sndin.cactPause && (sclNil == scl || sndin.scl == scl))
			{
			if (0 == sndin.cactPause++)
				isndinMin = isndin;
			_pglsndin->Put(isndin, &sndin);
			}
		}

	if (isndinMin < klwMax)
		_PauseQueue(isndinMin);

	_Leave();
}


/***************************************************************************
	If the given sound is in our queue, make sure it's not paused.
***************************************************************************/
void SNQUE::Resume(long sii)
{
	AssertThis(0);
	long isndin;
	SNDIN sndin;

	_Enter();
	_Flush();

	for (isndin = _pglsndin->IvMac(); isndin-- > _isndinCur; )
		{
		_pglsndin->Get(isndin, &sndin);
		if (sndin.sii == sii)
			{
			if (0 < sndin.cactPause)
				{
				sndin.cactPause--;
				_pglsndin->Put(isndin, &sndin);
				if (0 == sndin.cactPause)
					_ResumeQueue(isndin);
				}

			_Leave();
			return;
			}
		}

	_Leave();
}


/***************************************************************************
	Resume all sounds of the given sound class. sclNil means nuke all.
***************************************************************************/
void SNQUE::ResumeAll(long scl)
{
	AssertThis(0);
	long isndin;
	SNDIN sndin;
	long isndinMin = klwMax;

	_Enter();
	_Flush();

	for (isndin = _pglsndin->IvMac(); isndin-- > _isndinCur; )
		{
		_pglsndin->Get(isndin, &sndin);
		if (0 < sndin.cactPause && (sclNil == scl || sndin.scl == scl))
			{
			if (0 == --sndin.cactPause)
				isndinMin = isndin;
			_pglsndin->Put(isndin, &sndin);
			}
		}

	if (isndinMin < klwMax)
		_ResumeQueue(isndinMin);

	_Leave();
}


/***************************************************************************
	Return whether the given sound is in our queue.
***************************************************************************/
bool SNQUE::FPlaying(long sii)
{
	AssertThis(0);
	long isndin;
	SNDIN sndin;

	_Enter();
	_Flush();

	for (isndin = _pglsndin->IvMac(); isndin-- > _isndinCur; )
		{
		_pglsndin->Get(isndin, &sndin);
		if (sndin.sii == sii)
			{
			_Leave();
			return 0 <= sndin.cactPause;
			}
		}

	_Leave();

	return fFalse;
}


/***************************************************************************
	Return whether any sounds of the given sound class are in our queue.
	sclNil means any sounds at all.
***************************************************************************/
bool SNQUE::FPlayingAll(long scl)
{
	AssertThis(0);
	long isndin;
	SNDIN sndin;

	_Enter();
	_Flush();

	for (isndin = _pglsndin->IvMac(); isndin-- > _isndinCur; )
		{
		_pglsndin->Get(isndin, &sndin);
		if (0 <= sndin.cactPause && (sclNil == scl || sndin.scl == scl))
			{
			_Leave();
			return fTrue;
			}
		}

	_Leave();

	return fFalse;
}


/***************************************************************************
	Free anything that's not being used. This should only be called from
	the main thread.
***************************************************************************/
void SNQUE::Flush(void)
{
	AssertThis(0);

	_Flush();
}


/***************************************************************************
	Scale the given system volume by the given Kauai volume.
***************************************************************************/
ulong LuVolScale(ulong luVol, long vlm)
{
	Assert(kvlmFull == 0x10000, "this code assumes kvlmFull is 0x10000");
	ulong luHigh, luLow;
	ushort suHigh, suLow;

	MulLu(SuLow(luVol), vlm, &luHigh, &luLow);
	suLow = (luHigh > 0) ? (ushort)(-1) : SuHigh(luLow);

	MulLu(SuHigh(luVol), vlm, &luHigh, &luLow);
	suHigh = (luHigh > 0) ? (ushort)(-1) : SuHigh(luLow);

	return LuHighLow(suHigh, suLow);
}


