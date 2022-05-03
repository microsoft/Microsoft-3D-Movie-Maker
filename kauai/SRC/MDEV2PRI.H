/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Private declarations for mididev2 (streamed midi).

***************************************************************************/
#ifndef MDEV2PRI_H
#define MDEV2PRI_H

// This corresponds to the Win95 MIDIEVENT structure (with no optional data).
// We're using the older headers, so need to define our own.
struct MEV
	{
	DWORD dwDeltaTime;	// midi ticks between this and previous event
	DWORD dwStreamID;	// reserved - must be zero
	DWORD dwEvent;
	};
typedef MEV *PMEV;

// This corresponds to the Win95 MIDIHDR structure.
// We're using the older headers, so need to define our own.
struct MH
	{
	byte *lpData;
	DWORD dwBufferLength;
	DWORD dwBytesRecorded;
	DWORD dwUser;
	DWORD dwFlags;
	MH *lpNext;
	DWORD reserved;
	DWORD dwOffset;
	DWORD dwReserved[8];
	};
typedef MH *PMH;

typedef MIDIHDR *PMHO;

// A midi stream handle can be used as a midi out handle.
typedef HMIDIOUT HMS;


/***************************************************************************
	This is the midi stream cached object.
***************************************************************************/
typedef class MDWS *PMDWS;
#define MDWS_PAR BACO
#define kclsMDWS 'MDWS'
class MDWS : public MDWS_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PGL _pglmev;
	ulong _dts;

	MDWS(void);
	bool _FInit(PMIDS pmids);

public:
	static bool FReadMdws(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
		PBACO *ppbaco, long *pcb);
	static PMDWS PmdwsRead(PBLCK pblck);

	~MDWS(void);

	ulong Dts(void)
		{ return _dts; }
	void *PvLockData(long *pcb);
	void UnlockData(void);
	};


// forward declaration
typedef class MSMIX *PMSMIX;
typedef class MISI *PMISI;


/***************************************************************************
	Midi stream queue.
***************************************************************************/
typedef class MSQUE *PMSQUE;
#define MSQUE_PAR SNQUE
#define kclsMSQUE 'msqu'
class MSQUE : public MSQUE_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	MUTX _mutx;			// restricts access to member variables
	ulong _tsStart;		// when we started the current sound
	PMSMIX _pmsmix;

	MSQUE(void);

	virtual void _Enter(void);
	virtual void _Leave(void);

	virtual bool _FInit(PMSMIX pmsmix);
	virtual PBACO _PbacoFetch(PRCA prca, CTG ctg, CNO cno);
	virtual void _Queue(long isndinMin);
	virtual void _PauseQueue(long isndinMin);
	virtual void _ResumeQueue(long isndinMin);

public:
	static PMSQUE PmsqueNew(PMSMIX pmsmix);
	~MSQUE(void);

	void Notify(PMDWS pmdws);
	};


/***************************************************************************
	Midi Stream "mixer". It really just chooses which midi stream to play
	(based on the (spr, sii) priority).
***************************************************************************/
typedef class MSMIX *PMSMIX;
#define MSMIX_PAR BASE
#define kclsMSMIX 'msmx'
class MSMIX : public MSMIX_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	struct MSOS
		{
		PMSQUE pmsque;		// the "channel" or queue that the sound is on
		PMDWS pmdws;		// the sound
		long sii;			// its sound id (for a priority tie breaker)
		long spr;			// its priority
		long cactPlay;		// how many times to play the sound
		ulong dts;			// length of this sound
		ulong dtsStart;		// position to start at
		long vlm;			// volume to play at
		ulong tsStart;		// when we "started" the sound (minus dtsStart)
		};

	// Mutex to protect our member variables
	MUTX _mutx;
	HN _hevt;	// to notify the thread that the sound list changed
	HN _hth;	// thread to terminate non-playing sounds

	PMISI _pmisi;		// the midi stream interface
	PGL _pglmsos;		// the list of current sounds, in priority order
	long _cpvOut;		// number of buffers submitted (0, 1, or 2)

	PGL _pglmevKey;		// to accumulate state events for seeking
	bool _fPlaying: 1;	// whether we're currently playing the first stream
	bool _fWaiting: 1;	// we're waiting for our buffers to get returned
	bool _fDone: 1;		// tells the aux thread to terminate

	long _vlmBase;		// the base device volume
	long _vlmSound;		// the volume for the current sound

	MSMIX(void);
	bool _FInit(void);
	void _StopStream(void);
	bool _FGetKeyEvents(PMDWS pmdws, ulong dtsSeek, long *pcbSkip);
	void _Restart(bool fNew = fFalse);
	void _WaitForBuffers(void);
	void _SubmitBuffers(ulong tsCur);

	static void _MidiProc(ulong luUser, void *pvData, ulong luData);
	void _Notify(void *pvData, PMDWS pmdws);

	static ulong __stdcall _ThreadProc(void *pv);
	ulong _LuThread(void);

public:
	static PMSMIX PmsmixNew(void);
	~MSMIX(void);

	bool FPlay(PMSQUE pmsque, PMDWS pmdws = pvNil, long sii = siiNil,
		long spr = 0, long cactPlay = 1, ulong dtsStart = 0, long vlm = kvlmFull);

	void Suspend(bool fSuspend);
	void SetVlm(long vlm);
	long VlmCur(void);
	};


// Define these so we can use old (msvc 2.1) header files
#ifndef MEVT_SHORTMSG
#define MEVT_SHORTMSG		((BYTE)0x00)	// parm = shortmsg for midiOutShortMsg
#define MEVT_TEMPO			((BYTE)0x01)	// parm = new tempo in microsec/qn
#define MEVT_NOP			((BYTE)0x02)	// parm = unused; does nothing
#define MIDIPROP_SET		0x80000000L
#define MIDIPROP_GET		0x40000000L
#define MIDIPROP_TIMEDIV	0x00000001L
#endif //!MEVT_SHORTMSG


/***************************************************************************
	The midi stream interface.
***************************************************************************/
typedef void (*PFNMIDI)(ulong luUser, void *pvData, ulong luData);

typedef class MISI *PMISI;
#define MISI_PAR BASE
#define kclsMISI 'MISI'
class MISI : public MISI_PAR
	{
	RTCLASS_DEC

protected:
	HMS _hms;			// the midi stream handle
	PFNMIDI _pfnCall;	// call back function
	ulong _luUser;		// user data to send back

	// system volume level - to be saved and restored. The volume we set
	// is always relative to this
	bool _tBogusDriver;	// to indicate whether midiOutGetVolume really works
	ulong _luVolSys;
	long _vlmBase;		// our current volume relative to _luVolSys.

	MISI(PFNMIDI pfn, ulong luUser);

	virtual bool _FOpen(void) = 0;
	virtual bool _FClose(void) = 0;

	void _Reset(void);
	void _GetSysVol(void);
	void _SetSysVol(ulong luVol);
	void _SetSysVlm(void);

public:
	virtual void SetVlm(long vlm);
	virtual long VlmCur(void);

	virtual bool FActive(void);
	virtual bool FActivate(bool fActivate);

	virtual bool FQueueBuffer(void *pvData, long cb, long ibStart,
		long cactPlay, ulong luData) = 0;
	virtual void StopPlaying(void) = 0;
	};


/***************************************************************************
	The midiStreamStop API has a bug in it where it doesn't reset the
	current "buffer position" so that after calling midiStreamStop, then
	midiStreamOut and midiStreamRestart, the new buffer isn't played
	immediately, but the system waits until the previous buffer position
	expires before playing the new buffer.

	When this bug is fixed, STREAM_BUG can be undefined.
***************************************************************************/
#define STREAM_BUG

/***************************************************************************
	The real midi stream interface.
***************************************************************************/
typedef class WMS *PWMS;
#define WMS_PAR MISI
#define kclsWMS 'WMS'
class WMS : public WMS_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	#define kcmhMsir 2
	struct MSIR
		{
		void *pvData;
		long cb;
		long cactPlay;
		ulong luData;
		long ibNext;

		MH rgmh[kcmhMsir];
		long rgibLim[kcmhMsir];
		};
	typedef MSIR *PMSIR;

	MUTX _mutx;
	HINSTANCE _hlib;
	PGL _pglpmsir;
	long _ipmsirCur;
	long _cmhOut;

	HN _hevt;	// event to wake up the thread
	HN _hth;	// thread to do callbacks and cleanup after a notify

#ifdef STREAM_BUG
	bool _fActive: 1;
#endif //STREAM_BUG
	bool _fDone: 1;		// tells the aux thread to terminate

	MMRESULT (WINAPI *_pfnOpen)(HMS *phms, LPUINT puDeviceID, DWORD cMidi,
		DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
	MMRESULT (WINAPI *_pfnClose)(HMS hms);
	MMRESULT (WINAPI *_pfnProperty)(HMS hms, LPBYTE lpb, DWORD dwProperty);
	MMRESULT (WINAPI *_pfnPosition)(HMS hms, LPMMTIME lpmmt, UINT cbmmt);
	MMRESULT (WINAPI *_pfnOut)(HMS hms, LPMIDIHDR pmh, UINT cbmh);
	MMRESULT (WINAPI *_pfnPause)(HMS hms);
	MMRESULT (WINAPI *_pfnRestart)(HMS hms);
	MMRESULT (WINAPI *_pfnStop)(HMS hms);

	WMS(PFNMIDI pfn, ulong luUser);
	bool _FInit(void);

	virtual bool _FOpen(void);
	virtual bool _FClose(void);

	bool _FSubmit(PMH pmh);
	void _DoCallBacks(void);
	long _CmhSubmitBuffers(void);
	void _ResetStream(void);

	static void __stdcall _MidiProc(HMS hms, ulong msg, ulong luUser,
		ulong lu1, ulong lu2);
	void _Notify(HMS hms, PMH pmh);

	static ulong __stdcall _ThreadProc(void *pv);
	ulong _LuThread(void);

public:
	static PWMS PwmsNew(PFNMIDI pfn, ulong luUser);
	~WMS(void);

#ifdef STREAM_BUG
	virtual bool FActive(void);
	virtual bool FActivate(bool fActivate);
#endif //STREAM_BUG

	virtual bool FQueueBuffer(void *pvData, long cb, long ibStart,
		long cactPlay, ulong luData);
	virtual void StopPlaying(void);
	};


/***************************************************************************
	Our fake midi stream class.
***************************************************************************/
typedef class OMS *POMS;
#define OMS_PAR MISI
#define kclsOMS 'OMS'
class OMS : public OMS_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	struct MSB
		{
		void *pvData;
		long cb;
		long ibStart;
		long cactPlay;

		ulong luData;
		};

	MUTX _mutx;
	HN _hevt;	// event to notify the thread that the stream data has changed
	HN _hth;	// thread to play the stream data

	bool _fChanged: 1;	// the event has been signalled
	bool _fStop: 1;		// tells the aux thread to stop all buffers
	bool _fDone: 1;		// tells the aux thread to return

	long _imsbCur;
	PGL _pglmsb;
	PMEV _pmev;
	PMEV _pmevLim;
	ulong _tsCur;

	OMS(PFNMIDI pfn, ulong luUser);
	bool _FInit(void);

	virtual bool _FOpen(void);
	virtual bool _FClose(void);

	static ulong __stdcall _ThreadProc(void *pv);
	ulong _LuThread(void);
	void _ReleaseBuffers(void);

public:
	static POMS PomsNew(PFNMIDI pfn, ulong luUser);
	~OMS(void);

	virtual bool FQueueBuffer(void *pvData, long cb, long ibStart,
		long cactPlay, ulong luData);
	virtual void StopPlaying(void);
	};

#endif //!MDEV2PRI_H

