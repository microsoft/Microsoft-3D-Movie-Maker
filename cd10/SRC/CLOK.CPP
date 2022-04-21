/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Clock class. Clocks provide timing and alarm functionality. Clocks get
	CPU time by inserting themselves in the command handler list (attached
	to the CEX). This causes CLOK::FCmdAll to be called every time a command
	comes through the CEX. The time of a clock is updated only when
	CLOK::FCmdAll is called. So the following code will not time the operation:

		dtim = vclok.TimCur();
		for (...)
			{
			...
			}
		dtim = vlok.TimCur() - dtim;

	At the end of this, dtim will always be zero (unless CLOK::FCmdAll is
	somehow called in the loop - in which case dtim still won't be the exact
	timing of the loop). To do this type of timing use TsCurrent() or
	TsCurrentSystem(). Clocks use TsCurrent(), so scaling the application
	time scales all clocks.

	Added feature: TimCur now takes an optional boolean parameter indicating
	whether the time should be calculated or just the current value as
	described above. The default is false (return the current value ...).
	FSetAlarm also has an optional boolean specifying whether the alarm
	time should be computed from the current app time or from the current
	value (ie from TimCur(fTrue) or TimCur(fFalse)). The default is to use
	TimCur(fFalse) (old behavior).

	By default, if a clock's time is computed to exceed its next alarm's
	time, the clock's time "slips" back to the alarm's time. Eg, if an
	alarm is set for tim = 1000 and the app does something that takes a long
	time, so that the next time thru the clock's FCmdAll the clocks time
	is computed to be 1200, the clock's time is set back to 1000 and the
	alarm is triggered. If the clock should not slip in this way, fclokNoSlip
	should be specified in the constructor's grfclok parameter.

	fclokReset means that the clock's time should be reset to 0 every
	time the following commands come through the command queue:
	cidKey, cidTrackMouse, cidMouseMove, any cid less than cidMinNoMenu.
	Such a clock could be used for screen saver functionality or time-out
	animations, etc.

	WARNING: If alarms are not handled and set appropriately, the app can
	sometimes get into a tight loop that the user can't break into.
	Suppose a command handler "A" wants to do something every 1/10 of a second.
	It sets up a clock and sets an alarm for 1/10 of a second from now.
	When the alarm goes off, A sets the next alarm for 1/10 of a second from
	now, does some work that takes 1/2 second (we're running on a slow
	machine), then enqueues a command "cidFoo" to another object "B".
	Assuming the command queue was empty, the next command processed by the
	CEX is cidFoo. This causes the alarm to go off again (remember that alarms
	go off during the FCmdAll call). And the whole process repeats. The
	command queue is never empty in the main app loop, so we never check the
	system event queue, so the user can sit there and hit the keyboard or play
	with the mouse as much as they want and we will never see it. The way to
	avoid this is to set the next alarm after the work is done, and better
	yet, don't reset the alarm until all commands resulting from the alarm
	have been processed. Handler A should do the following: do its work,
	enqueue cidFoo to object B, enqueue cidBar to itself. When it gets cidBar,
	it sets an alarm for 1/10 of a second into the future. This guarantees
	a 1/10 second gap between then end of handling one alarm and starting
	to handle the next alarm.

***************************************************************************/
#include "frame.h"
ASSERTNAME


RTCLASS(CLOK)

BEGIN_CMD_MAP_BASE(CLOK)
END_CMD_MAP(&CLOK::FCmdAll, pvNil, kgrfcmmAll)


const long kcmhlClok = kswMin;	//put clocks at the head of the list
PCLOK CLOK::_pclokFirst;

/***************************************************************************
	Constructor for the clock - just zeros the time.  fclokReset specifies
	that this clock should reset itself to zero on key or mouse input.
	fclokNoSlip specifies that the clok should not let time slip.
***************************************************************************/
CLOK::CLOK(long hid, ulong grfclok) : CMH(hid)
{
	_pclokNext = _pclokFirst;
	_pclokFirst = this;
	_timBase = _timCur = _dtimAlarm = 0;
	_timNext = kluMax;
	_tsBase = 0;
	_grfclok = grfclok;
	_pglalad = pvNil;
	AssertThis(0);
}


/***************************************************************************
	Destructor for a CLOK - remove it from the linked list of clocks.
***************************************************************************/
CLOK::~CLOK(void)
{
	PCLOK *ppclok;

	for (ppclok = &_pclokFirst; *ppclok != pvNil && *ppclok != this;
			ppclok = &(*ppclok)->_pclokNext)
		{
		}
	if (*ppclok == this)
		*ppclok = _pclokNext;
	else
		Bug("clok not in linked list");

	ReleasePpo(&_pglalad);
}


/***************************************************************************
	Static method to find the first clok with the given id.
***************************************************************************/
PCLOK CLOK::PclokFromHid(long hid)
{
	PCLOK pclok;

	for (pclok = _pclokFirst; pvNil != pclok; pclok = pclok->_pclokNext)
		{
		AssertPo(pclok, 0);
		if (pclok->Hid() == hid)
			break;
		}
	return pclok;
}


/***************************************************************************
	Static method to remove all references to the given CMH from the clok
	ALAD lists.
***************************************************************************/
void CLOK::BuryCmh(PCMH pcmh)
{
	PCLOK pclok;

	for (pclok = _pclokFirst; pvNil != pclok; pclok = pclok->_pclokNext)
		pclok->RemoveCmh(pcmh);
}


/***************************************************************************
	Remove any alarms set by the given CMH.
***************************************************************************/
void CLOK::RemoveCmh(PCMH pcmh)
{
	AssertThis(0);
	ALAD *qalad;
	long ialad;

	if (pvNil == _pglalad)
		return;

	for (ialad = _pglalad->IvMac(); ialad-- > 0; )
		{
		qalad = (ALAD *)_pglalad->QvGet(ialad);
		if (qalad->pcmh == pcmh)
			_pglalad->Delete(ialad);
		}
}


/***************************************************************************
	Start the clock.
***************************************************************************/
void CLOK::Start(ulong tim)
{
	AssertThis(0);
	_timBase = _timCur = tim;
	_dtimAlarm = 0;
	_tsBase = TsCurrent();
	vpcex->RemoveCmh(this, kcmhlClok);
	vpcex->FAddCmh(this, kcmhlClok, kgrfcmmAll);
}


/***************************************************************************
	Stop the clock. The time will no longer advance on this clock.
***************************************************************************/
void CLOK::Stop(void)
{
	AssertThis(0);
	vpcex->RemoveCmh(this, kcmhlClok);
}


/***************************************************************************
	Return the current time. If fAdjustForDelay is true, the time is a more
	accurate time, but is not synchronized to the alarms or to the command
	stream. Normally, the clok's time is only updated when a command gets
	processed by the command dispatcher. If fAdjustForDelay is false, the
	last computed time value is returned, otherwise the time is computed
	from the current application time.
***************************************************************************/
ulong CLOK::TimCur(bool fAdjustForDelay)
{
	AssertThis(0);

	if (!fAdjustForDelay)
		return _timCur;

	return _timBase + LuMulDiv(TsCurrent() - _tsBase, kdtimSecond, kdtsSecond);
}


/***************************************************************************
	Set an alarm for the given time and for the given command handler.
	Alarms are sorted in _decreasing_ order.
***************************************************************************/
bool CLOK::FSetAlarm(long dtim, PCMH pcmhNotify, long lwUser,
	bool fAdjustForDelay)
{
	AssertThis(0);
	AssertIn(dtim, 0, kcbMax);
	AssertNilOrPo(pcmhNotify, 0);
	ALAD alad;
	ALAD *qalad;
	long ialad, ialadMin, ialadLim;

	alad.pcmh = pcmhNotify;
	alad.tim = TimCur(fAdjustForDelay) + LwMax(dtim, 1);
	alad.lw = lwUser;
	if (pvNil == _pglalad && pvNil == (_pglalad = GL::PglNew(size(ALAD), 1)))
		return fFalse;
	for (ialadMin = 0, ialadLim = _pglalad->IvMac(); ialadMin < ialadLim; )
		{
		ialad = (ialadMin + ialadLim) / 2;
		qalad = (ALAD *)_pglalad->QvGet(ialad);
		if (qalad->tim < alad.tim)
			ialadLim = ialad;
		else
			ialadMin = ialad + 1;
		}
	if (!_pglalad->FInsert(ialadMin, &alad))
		return fFalse;
	if (_timNext > alad.tim)
		_timNext = alad.tim;
	return fTrue;
}


/***************************************************************************
	Advance the clock and sound an alarm if one is due to go off.  This
	actually gets called every time through the command loop.
***************************************************************************/
bool CLOK::FCmdAll(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	CMD cmd;
	long ialad;
	ALAD alad;
	ulong tsCur, timCur;

	_dtimAlarm = 0;
	if (pcmd->cid == cidAlarm)
		return fFalse;

	tsCur = TsCurrent();
	timCur = _timBase + LuMulDiv(tsCur - _tsBase, kdtimSecond, kdtsSecond);

	if (_grfclok & fclokReset)
		{
		switch (pcmd->cid)
			{
		case cidKey:
		case cidTrackMouse:
		case cidMouseMove:
			goto LReset;
		default:
			if (pcmd->cid < cidMinNoMenu)
				{
LReset:
				_tsBase = tsCur;
				_timBase = 0;
				timCur = 0;
				}
			break;
			}
		}

	if (timCur < _timNext)
		{
		// just update the time
		_timCur = timCur;
		return fFalse;
		}

	// sound any alarms
	for (;;)
		{
		if (pvNil == _pglalad || 0 > (ialad = _pglalad->IvMac() - 1))
			{
			_timNext = kluMax;
			break;
			}
		_pglalad->Get(ialad, &alad);
		if (alad.tim > timCur)
			{
			_timNext = alad.tim;
			break;
			}
		_pglalad->Delete(ialad);

		// adjust the current time
		_timCur = alad.tim;
		_timNext = kluMax;
		if (timCur > alad.tim && !(_grfclok & fclokNoSlip))
			{
			// we've slipped
			timCur = _timBase = alad.tim;
			_tsBase = tsCur;
			}

		// send the alarm
		ClearPb(&cmd, size(CMD));
		cmd.cid = cidAlarm;
		cmd.pcmh = alad.pcmh;
		cmd.rglw[0] = Hid();
		cmd.rglw[1] = alad.tim;
		cmd.rglw[2] = alad.lw;

		if (pvNil != alad.pcmh)
			{
			// tell the CMH that the alarm went off
			AddRef();
			Assert(_cactRef > 1, 0);
			_dtimAlarm = timCur - _timCur;
			alad.pcmh->FDoCmd(&cmd);
			if (_cactRef == 1)
				{
				Release();
				return fFalse;
				}
			Release();
			AssertThis(0);
			}
		else
			vpcex->EnqueueCmd(&cmd);
		}

	_timCur = timCur;
	return fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a CLOK.
***************************************************************************/
void CLOK::AssertValid(ulong grf)
{
	CLOK_PAR::AssertValid(0);
	AssertNilOrPo(_pglalad, 0);
	Assert(_timCur <= _timNext, "_timNext too small");
	Assert((_grfclok & fclokNoSlip) || _dtimAlarm == 0,
		"_dtimAlarm should be 0");
}


/***************************************************************************
	Mark memory for the CLOK.
***************************************************************************/
void CLOK::MarkMem(void)
{
	AssertValid(0);
	CLOK_PAR::MarkMem();
	MarkMemObj(_pglalad);
}


/***************************************************************************
	Static method to mark all the CLOKs
***************************************************************************/
void CLOK::MarkAllCloks(void)
{
	PCLOK pclok;

	for (pclok = _pclokFirst; pvNil != pclok; pclok = pclok->_pclokNext)
		{
		AssertPo(pclok, 0);
		MarkMemObj(pclok);
		}
}
#endif //DEBUG

