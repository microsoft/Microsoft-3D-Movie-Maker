/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Script interpreter for the gob based scripts.

***************************************************************************/
#ifndef SCREXEG_H
#define SCREXEG_H


/****************************************
	Gob based script interpreter
****************************************/
typedef class SCEG *PSCEG;
#define SCEG_PAR SCEB
#define kclsSCEG 'SCEG'
class SCEG : public SCEG_PAR
	{
	RTCLASS_DEC
	ASSERT

protected:
	//CAUTION: _pgob may be nil (even if the gob still exists)! Always access
	//thru _PgobThis.  When something is done that may cause the gob to be
	//freed (such as calling another script), set this to nil.
	PGOB _pgob;
	long _hid;		// the handler id of the initialization gob
	long _grid;		// the unique gob run-time id of the initialization gob
	PWOKS _pwoks;	// the kidspace world this script belongs to

	virtual PGOB _PgobThis(void);
	virtual PGOB _PgobFromHid(long hid);

	virtual bool _FExecOp(long op);
	virtual PGL *_PpglrtvmThis(void);
	virtual PGL *_PpglrtvmGlobal(void);
	virtual PGL *_PpglrtvmRemote(long lw);

	virtual short _SwCur(void);
	virtual short _SwMin(void);

	void _DoAlert(long op);
	void _SetColorTable(CHID chid);
	void _DoEditControl(long hid, long stid, bool fGet);
	PGL _PglclrGet(CNO cno);
	bool _FLaunch(long stid);

public:
	SCEG(PWOKS pwoks, PRCA prca, PGOB pgob);

	void GobMayDie(void)
		{ _pgob = pvNil; }
	virtual bool FResume(long *plwReturn = pvNil, bool *pfPaused = pvNil);
	};

//a Chunky resource reader for a color table
bool FReadColorTable(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb);

#endif //!SCREXEG_H


