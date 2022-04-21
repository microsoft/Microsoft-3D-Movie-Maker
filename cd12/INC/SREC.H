/***************************************************************************

	srec.h: Sound Recording class

	Primary Author: ****** (based on ***** original srec)
	Review Status: reviewed

	BASE ---> SREC

***************************************************************************/
#ifndef SREC_H
#define SREC_H

#include "audioman.h"

/****************************************
	RIFF Header helper class
****************************************/
#ifdef MAC
#define RIFF_TAG    'RIFF'
#define WAVE_TAG    'WAVE'
#define FMT__TAG    'fmt '
#define DATA_TAG    'data'
#define FACT_TAG    'fact'
#else
#define RIFF_TAG    'FFIR'	// RIFF
#define WAVE_TAG    'EVAW'	// WAVE
#define FMT__TAG    ' tmf'	// fmt_
#define DATA_TAG    'atad'	// data
#define FACT_TAG    'tcaf'	// fact
#endif


#pragma pack(push, _SOCPACK_)
#pragma pack(1)

class RIFF
	{
private:
    DWORD _dwRiffTag;
	DWORD _dwRiffLength;
	DWORD _dwWaveTag;
	DWORD _dwFmtTag;
	DWORD _dwFmtLength;
	WAVEFORMATEX _wfx;
	DWORD _dwDataTag;
	DWORD _dwDataLength;
public:
	void Set(long cchan, long csampSec, long cbSample, DWORD dwLength)
		{
		_dwRiffTag = RIFF_TAG;
		_dwRiffLength = sizeof(RIFF) + dwLength;
		_dwWaveTag = WAVE_TAG;
		_dwFmtTag = FMT__TAG;
		_dwFmtLength = sizeof(WAVEFORMATEX);
		_wfx.wFormatTag = WAVE_FORMAT_PCM;
		_wfx.nChannels = (ushort)cchan;
		_wfx.nSamplesPerSec = csampSec;
		_wfx.nAvgBytesPerSec = csampSec * cbSample * cchan;
		_wfx.nBlockAlign = (ushort)cchan * (ushort)cbSample;
		_wfx.wBitsPerSample = (ushort)LwMul(8, cbSample);
		_wfx.cbSize = 0;
		_dwDataTag = DATA_TAG;
		_dwDataLength = dwLength;
		}
	
	LPWAVEFORMATEX PwfxGet() { return &_wfx; };
	
	DWORD Cb() { return sizeof(RIFF) + _dwDataLength; };
	};
#pragma pack(pop, _SOCPACK_)

/****************************************
	The sound recording class
****************************************/
typedef class SREC *PSREC;
#define SREC_PAR BASE
#define kclsSREC 'SREC'
class SREC : public SREC_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	long _csampSec;	// sampling rate (number of samples per second)
	long _cchan;	// 1 = mono, 2 = stereo
	long _cbSample;	// bytes per sample (1 = 8 bit, 2 = 16 bit, etc)
	ulong _dtsMax;	// maximum length to record
	bool _fRecording;
	bool _fPlaying;
	bool _fHaveSound; 	// have you recorded a sound yet?
	bool _fBufferAdded;	// have added record buffer

	HWAVEIN _hwavein;	// handle to wavein device
	WAVEHDR _wavehdr;	// wave hdr for buffer
	
	LPMIXER _pmixer;	// pointer to Audioman Mixer
	LPCHANNEL _pchannel;// pointer to Audioman Channel
	LPSOUND	_psnd;		// psnd for current sound
	RIFF *_priff;		// pointer to riff in memory

	bool _FOpenRecord();
	bool _FCloseRecord();
	static void _WaveInProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

protected:
	bool _FInit(long csampSec, long cchan, long cbSample, ulong dtsMax);
	void _UpdateStatus(void);

public:
	static PSREC PsrecNew(long csampSec, long cchan, long cbSample, ulong dtsMax);
	~SREC(void);

	bool FStart(void);
	bool FStop(void);
	bool FPlay(void);
	bool FRecording(void);
	bool FPlaying(void);
	bool FSave(PFNI pfni);
	bool FHaveSound(void) { return _fHaveSound; }
	};


#endif SREC_H
