/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Midi playback class.

***************************************************************************/
#ifndef MIDI_H
#define MIDI_H


typedef class MIDS *PMIDS;

// midi event
struct MIDEV
	{
	ulong ts;		// time stamp of this event
	long cb;		// number of bytes to send (in rgbSend)
	long lwTempo;	// the current tempo - at a tempo change, cb will be 0
	union
		{
		byte rgbSend[4];	// bytes to send if pvLong is nil
		long lwSend;		// for convenience
		};
	};
typedef MIDEV *PMIDEV;


/***************************************************************************
	Midi stream parser. Knows how to parse standard MIDI streams.
***************************************************************************/
typedef class MSTP *PMSTP;
#define MSTP_PAR BASE
#define kclsMSTP 'MSTP'
class MSTP : public MSTP_PAR
	{
	RTCLASS_DEC
	NOCOPY(MSTP)
	ASSERT
	MARKMEM

protected:
	ulong _tsCur;
	long _lwTempo;
	byte *_prgb;
	byte *_pbLim;
	byte *_pbCur;
	byte _bStatus;

	Debug( long _cactLongLock; )
	PMIDS _pmids;

	bool _FReadVar(byte **ppbCur, long *plw);

public:
	MSTP(void);
	~MSTP(void);

	void Init(PMIDS pmids, ulong tsStart = 0, long lwTempo = 500000);
	bool FGetEvent(PMIDEV pmidev, bool fAdvance = fTrue);
	};


/***************************************************************************
	Midi Stream object - this is like a MTrk chunk in a standard MIDI file,
	with timing in milliseconds.
***************************************************************************/
#define MIDS_PAR BACO
#define kclsMIDS 'MIDS'
class MIDS : public MIDS_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	HQ _hqrgb;

	friend MSTP;

	MIDS(void);

	static long _CbEncodeLu(ulong lu, byte *prgb);

public:
	static bool FReadMids(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
		PBACO *ppbaco, long *pcb);
	static PMIDS PmidsRead(PBLCK pblck);
	static PMIDS PmidsReadNative(FNI *pfni);
	~MIDS(void);

	virtual bool FWrite(PBLCK pblck);
	virtual long CbOnFile(void);
	};

#endif //!MIDI_H


