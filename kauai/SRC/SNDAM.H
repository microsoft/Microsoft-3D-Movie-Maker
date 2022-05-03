/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Audioman sound support. Contains the audioman sound sink, audioman
	channel and audioman cached sound classes.

***************************************************************************/
#ifndef SNDAM_H
#define SNDAM_H


// allowable formats for the audioman device
// WARNING: code assumes that (kwav*16 - kwav*8) is constant!
enum
	{
	kwav11M8,
	kwav22M8,
	kwav44M8,
	kwav11S8,
	kwav22S8,
	kwav44S8,
	kwav11M16,
	kwav22M16,
	kwav44M16,
	kwav11S16,
	kwav22S16,
	kwav44S16,
	kwavLim
	};


/***************************************************************************
	Audioman sound device class.
***************************************************************************/
typedef class SDAM *PSDAM;
#define SDAM_PAR SNDMQ
#define kclsSDAM 'SDAM'
class SDAM : public SDAM_PAR
	{
	RTCLASS_DEC
	ASSERT

protected:
	ulong _luVolSys;
	long _vlm;
	bool _fAudioManInited: 1;

	SDAM(void);
	virtual bool _FInit(long wav);

	// inherited methods
	virtual PSNQUE _PsnqueNew(void);
	virtual void _Suspend(bool fSuspend);

public:
	static long vcbMaxMemWave;

	static PSDAM PsdamNew(long wav);
	~SDAM(void);

	virtual void SetVlm(long vlm);
	virtual long VlmCur(void);
	virtual void BeginSynch(void);
	virtual void EndSynch(void);
	};

#endif //!SNDAM_H

