/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Clock class. See comments in clok.cpp.

***************************************************************************/
#ifndef CLOK_H
#define CLOK_H

enum
	{
	fclokNil = 0,
	fclokReset = 1,
	fclokNoSlip = 2,
	};

typedef class CLOK *PCLOK;
#define CLOK_PAR CMH
#define kclsCLOK 'CLOK'
class CLOK : public CLOK_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	CMD_MAP_DEC(CLOK)

protected:
	//alarm descriptor
	struct ALAD
		{
		PCMH pcmh;
		ulong tim;
		long lw;
		};

	static PCLOK _pclokFirst;

	PCLOK _pclokNext;
	ulong _tsBase;
	ulong _timBase;
	ulong _timCur;		// current time
	ulong _dtimAlarm;	// processing alarms up to _timCur + _dtimAlarm
	ulong _timNext;		// next alarm time to process (for speed)
	ulong _grfclok;
	PGL _pglalad;		//the registered alarms

public:
	CLOK(long hid, ulong grfclok = fclokNil);
	~CLOK(void);
	static PCLOK PclokFromHid(long hid);
	static void BuryCmh(PCMH pcmh);
	void RemoveCmh(PCMH pcmh);

	void Start(ulong tim);
	void Stop(void);
	ulong TimCur(bool fAdjustForDelay = fFalse);
	ulong DtimAlarm(void)
		{ return _dtimAlarm; }

	bool FSetAlarm(long dtim, PCMH pcmhNotify = pvNil, long lwUser = 0,
		bool fAdjustForDelay = fFalse);

	//idle handling
	virtual bool FCmdAll(PCMD pcmd);

	#ifdef DEBUG
		static void MarkAllCloks(void);
	#endif //DEBUG
	};

#endif //!CLOK_H

