/***************************************************************************

	srec.cpp: Sound recording class

	Primary Author: ****** (based on ***** original srec)
	Review Status: reviewed

***************************************************************************/
#include "soc.h"

ASSERTNAME

RTCLASS(SREC)

/***************************************************************************
	Create a new SREC
***************************************************************************/
PSREC SREC::PsrecNew(long csampSec, long cchan, long cbSample, ulong dtsMax)
{
	PSREC psrec;

	psrec = NewObj(SREC);
	if (pvNil == psrec)
		return pvNil;
	if (!psrec->_FInit(csampSec, cchan, cbSample, dtsMax))
		{
		ReleasePpo(&psrec);
		return pvNil;
		}
	AssertPo(psrec, 0);
	return psrec;
}


/***************************************************************************
	Init this SREC
***************************************************************************/
bool SREC::_FInit(long csampSec, long cchan, long cbSample, ulong dtsMax)
{
	AssertBaseThis(0);
	AssertIn(cchan, 0, ksuMax);

	long cwid;

	_csampSec = csampSec;
	_cchan = cchan;
	_cbSample = cbSample;
	_dtsMax = dtsMax;
	_hwavein = pvNil;
	_priff = pvNil;
	_fBufferAdded = fFalse;
	_fRecording = fFalse;
	_fHaveSound = fFalse;

	vpsndm->Suspend(fTrue); // turn off sndm so we can get wavein device

	// See if sound recording is possible at all
	cwid = waveInGetNumDevs();
	if (0 == cwid)
		{
		PushErc(ercSocNoWaveIn);
		return fFalse;
		}

	// allocate a 10 second buffer
	_wavehdr.dwBufferLength = (cchan * csampSec * cbSample * dtsMax)/1000;
	if(!FAllocPv((void **)&_priff, sizeof(RIFF) + _wavehdr.dwBufferLength, fmemClear, mprNormal))
		return fFalse;
	
	_wavehdr.lpData = (LPBYTE)PvAddBv(_priff, sizeof(RIFF));

	// init RIFF structure
	_priff->Set(_cchan, _csampSec, _cbSample, 0);

	if (fFalse == _FOpenRecord())
		{
		return fFalse;
		}

	// get audioman 
	_pmixer = GetAudioManMixer();
	if (pvNil == _pmixer)
		return fFalse;

	// get a channel
	_pmixer->AllocChannel(&_pchannel);
	if (pvNil == _pchannel)
		return fFalse;

	return fTrue;
}


/***************************************************************************
	Clean up and delete this SREC
***************************************************************************/
SREC::~SREC(void)
{
	AssertBaseThis(0);
	
	// make sure nothing is playing or recording
	if (_fRecording || _fPlaying)
		FStop();

	if (_hwavein)
		_FCloseRecord();

	ReleasePpo(&_pchannel);
	ReleasePpo(&_pmixer);
	FreePpv((void **)&_priff);
	vpsndm->Suspend(fFalse); 	// restore sound mgr
}


/***************************************************************************
	Open Device for recording
***************************************************************************/
bool SREC::_FOpenRecord(void)
{
	AssertBaseThis(0);

	_fRecording = fFalse;

	if (pvNil == _hwavein)
		{
		// open a wavein device
		if (waveInOpen(&_hwavein, WAVE_MAPPER, (WAVEFORMAT *)_priff->PwfxGet(), (DWORD)_WaveInProc, (DWORD)this, CALLBACK_FUNCTION))
			{
			// it doesn't support this format
			return fFalse;
			}

		// prepare header on block of data
		_wavehdr.dwUser = (DWORD) this;
		if (waveInPrepareHeader(_hwavein, &_wavehdr, sizeof(WAVEHDR)))
			{
			waveInClose(_hwavein);
			_hwavein = pvNil;
			return fFalse;
			}
		}

	// add buffer to device
	if (!_fBufferAdded)
		if (waveInAddBuffer(_hwavein, &_wavehdr, sizeof(WAVEHDR)))
			{
			_FCloseRecord();
			_fRecording = fFalse;
			_hwavein = pvNil;
			return fFalse;
			}
		else
			_fBufferAdded = fTrue;

	return fTrue;
}

/***************************************************************************
	Close Device for recording
***************************************************************************/
bool SREC::_FCloseRecord(void)
{
	AssertThis(0);

	if (_hwavein)
		{
		// stop if necessary
		waveInReset(_hwavein);

		// unprepare header
		waveInUnprepareHeader(_hwavein, &_wavehdr, sizeof(WAVEHDR));
		_fRecording = fFalse;
				
		// close
		waveInClose(_hwavein);
		_hwavein = pvNil;
		}

	return fTrue;
}


/***************************************************************************
	Figure out if we're recording or not
***************************************************************************/
void SREC::_UpdateStatus(void)
{
	AssertThis(0);

	// ------------------------------------ 
	// Check playing mode
	// ------------------------------------ 
	if ((_fPlaying) && !_pchannel->IsPlaying())
		{
		// then we just stopped
		Sleep(250L);	// sleep a little bit to cover AudioMan bug 
		vpsndm->Suspend(fTrue); 	// suspend sound mgr
		}
	_fPlaying = _pchannel->IsPlaying();

	// ------------------------------------ 
	// 	Check Recording mode
	// If we are recording, AND our HaveSound flag
	// is set, then we must have just finished, so 
	// process the data, and turn off the recording flag
	// ------------------------------------ 
	if ((_fRecording) && (_fHaveSound))
		{
		LPSOUND psnd = pvNil;		// original psnd
		LPSOUND psndBias = pvNil;	// psnd Bias correction filter
		LPSOUND psndTrim = pvNil;	// psnd Trim filter

		_fRecording = fFalse;
		if (_wavehdr.dwBytesRecorded == 0)
			{
			_fHaveSound = fFalse;
			return;
			}

		// using the Audioman APIs, apply the gain and Trim filter, and save it back out
		// to a different temp file.
		_wavehdr.dwBytesRecorded -= 8*(_cchan * _cbSample);	// chop off last 8 samples worth, since some audio cards put garbage on end of data
		_priff->Set(_cchan, _csampSec, _cbSample, _wavehdr.dwBytesRecorded);

		// now use AudioMan API to load the temp file, apply a trim filter and place
		// trimmed sound out to our temp file
		if (FAILED(AllocSoundFromMemory(&psnd, (LPBYTE)_priff, _priff->Cb())))
			{
			PushErc(ercOomNew);
			_fHaveSound = fFalse;	
			return;
			}
		_fHaveSound = fTrue;

		if (FAILED(AllocBiasFilter(&psndBias, psnd)))
			{
			// then just return the sound raw
			_psnd = psnd;
			return;
			}

		// release the original sound, since it's now owned by the psndGain
		ReleasePpo(&psnd);

		if (FAILED(AllocTrimFilter(&_psnd, psndBias)))
			{
			// then just return the sound with the bias filter on it...
			_psnd = psndBias;
			return;
			}
		// release the psndBias, since it's now owned by the psndTrim
		ReleasePpo(&psndBias);
		}
}

/***************************************************************************
	Figure out if we're recording or not
***************************************************************************/
void SREC::_WaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	// the psrec pointer is a pointer to the class which generated the event and owns the device
	SREC *psrec = (SREC *)dwInstance;

	switch(uMsg)
		{
		case WIM_DATA:
			{
			// any time we get a block of data, we are done, we set our flag
			// to true, allowing _UpdateStatus to notice that we are _fRecording and _fHaveSound
			// at which point it will process the data...
			psrec->_fHaveSound = fTrue;
			psrec->_fBufferAdded = fFalse;
			}
		}
}

/***************************************************************************
	Start recording
***************************************************************************/
bool SREC::FStart(void)
{
	AssertThis(0);
	Assert(!_fRecording, "stop previous recording first");

	// make sure we are open
	if (_fPlaying)
		FStop();

	if (!_FOpenRecord())
		return fFalse;

	_fHaveSound = fFalse;
	_fRecording = fFalse;
	_wavehdr.dwBytesRecorded = 0;
	
	// now record data
	if (waveInStart(_hwavein))
		return fFalse;

	_fRecording = fTrue;

	return fTrue;
}

/***************************************************************************
	Stop recording or playing
***************************************************************************/
bool SREC::FStop(void)
{
	AssertThis(0);
	Assert(_fRecording || _fPlaying, "Nothing to stop");

	// if we are recording
	if (_fRecording)
		{
		// then stop the recording device
		waveInStop(_hwavein);
		}
	else if (_fPlaying)	// if we are playing
		{
		// then stop the playing device
		_pchannel->Stop();
		}

	// update status accordingly
	_UpdateStatus();

	return fTrue;
}


/***************************************************************************
	Start playing the current sound
***************************************************************************/
bool SREC::FPlay(void)
{
	AssertThis(0);
	Assert(_fHaveSound, "No sound to play");
	
	// open the _fniTrim file with MCI
	_FCloseRecord();

	if (_psnd && _pchannel)
		{
		vpsndm->StopAll(); 				//stop any outstanding bogus sounds from button pushs
		vpsndm->Suspend(fFalse); 		// restore sound mgr

		_pchannel->Stop();				// stop our channel (should be nop)
		_pchannel->SetSoundSrc(_psnd);	// give it our sound
		_pchannel->SetPosition(0);		// seek to the beginning

		if (FAILED(_pchannel->Play()))	// play the sound
			{
			_UpdateStatus();		// this will check play status, and clean up accordingly
			}
		else
			_fPlaying = fTrue;	
		}

	return _fPlaying;
}


/***************************************************************************
	Are we recording?
***************************************************************************/
bool SREC::FRecording(void)
{
	AssertThis(0);

	_UpdateStatus();
	return _fRecording;
}


/***************************************************************************
	Are we playing the current sound?
***************************************************************************/
bool SREC::FPlaying(void)
{
	AssertThis(0);

	_UpdateStatus();
	return _fPlaying;
}


/***************************************************************************
	Save the current sound to the given FNI
***************************************************************************/
bool SREC::FSave(PFNI pfni)
{
	AssertThis(0);
	Assert(_fHaveSound, "Nothing to save!");

	STN stn;

	if (_psnd)
		{
		pfni->GetStnPath(&stn);

		// now save _psnd to the FNI passed in 
		if (FAILED(SoundToFileAsWave(_psnd, stn.Psz())))
			{
			PushErc(ercSocWaveSaveFailure);	
			return fFalse;
			}
		return fTrue;
		}
	return fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the SREC.
***************************************************************************/
void SREC::AssertValid(ulong grf)
{
	SREC_PAR::AssertValid(fobjAllocated);
	Assert(pvNil != _pmixer, "No mixer?");
	Assert(pvNil != _pchannel, "No Channel?");
}


/***************************************************************************
	Mark memory used by the SREC
***************************************************************************/
void SREC::MarkMem(void)
{
	AssertThis(0);
	MarkPv(_priff);
	SREC_PAR::MarkMem();
}
#endif //DEBUG

