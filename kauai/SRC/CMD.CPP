/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Basic command classes: CEX (command dispatcher), CMH (command handler).

	The command dispatcher (CEX) has a command (CMD) queue and a list of
	command handlers (CMH). During normal operation (CEX::FDispatchNextCmd),
	the CEX takes the next CMD from the queue, passes it to each CMH in its
	list (by calling CMH::FDoCmd) until one of the handlers returns true.
	If none of the handlers in the list returns true, the command is passed
	to the handler specified in the CMD itself (cmd.pcmh, if not nil).

	A CMH is placed in the handler list by a call to CEX::FAddCmh. The
	cmhl parameter determines the order of CMH's in the list. The grfcmm
	parameter indicates which targets the CMH wants to see commands for.
	The options are fcmmThis, fcmmNobody, fcmmOthers.

	The CEX class supports command stream recording and playback.

***************************************************************************/
#include "frame.h"
ASSERTNAME


// command map shared by every command handler
BEGIN_CMD_MAP_BASE(CMH)
END_CMD_MAP_NIL()


RTCLASS(CMH)
RTCLASS(CEX)


long CMH::_hidLast;


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a CMD.
***************************************************************************/
void CMD::AssertValid(ulong grf)
{
	AssertThisMem();
	AssertNilOrPo(pgg, 0);
	AssertNilOrPo(pcmh, 0);
}
#endif //DEBUG


/***************************************************************************
	Static method to return a hid (command handler ID) such that the numbers
	{ hid, hid + 1, ... , hid + ccmh - 1 } are not currently in use and all
	have their high bit set. To avoid possible conflicts, hard-wired
	handler ID's should have their high bit clear. This guarantees that
	the values will not be returned again in the near future (whether or not
	they are in use).

	This calls vpappb->PcmhFromHid to determine if a hid is in use. This
	means that the returned hid is only unique over handlers that the
	application class knows about.
***************************************************************************/
long CMH::HidUnique(long ccmh)
{
	AssertIn(ccmh, 1, 1000);
	long ccmhT;

	_hidLast |= 0x80000000L;
	for (ccmhT = ccmh; ccmhT-- > 0; )
		{
		_hidLast++;
		if (!(_hidLast & 0x80000000L))
			{
			_hidLast = 0x80000000L;
			ccmhT = ccmh;
			continue;
			}
		if (pvNil != vpappb->PcmhFromHid(_hidLast))
			ccmhT = ccmh;
		}

	return _hidLast - (ccmh - 1);
}


/***************************************************************************
	Constructor for a command handler - set the handler id.
***************************************************************************/
CMH::CMH(long hid)
{
	AssertBaseThis(0);
	Assert(hid != hidNil, "bad hid");
	_hid = hid;
}


/***************************************************************************
	Destructor for a command handler - purge any global references to it
	from the app. The app purges it from the command dispatcher.
***************************************************************************/
CMH::~CMH(void)
{
	AssertThis(0);
	if (pvNil != vpappb)
		vpappb->BuryCmh(this);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a CMH.
***************************************************************************/
void CMH::AssertValid(ulong grf)
{
	CMH_PAR::AssertValid(0);
	Assert(_hid != hidNil, 0);
}
#endif //DEBUG


/***************************************************************************
	Protected virtual function to find a CMME (command map entry) for the
	given command id.
***************************************************************************/
bool CMH::_FGetCmme(long cid, ulong grfcmmWanted, CMME *pcmme)
{
	AssertThis(0);
	AssertVarMem(pcmme);
	Assert(cid != cidNil, "why is the cid nil?");
	CMM *pcmm;
	CMME *pcmmeT;
	CMME *pcmmeDef = pvNil;

	for (pcmm = Pcmm(); pcmm != pvNil; pcmm = pcmm->pcmmBase)
		{
		for (pcmmeT = pcmm->prgcmme; pcmmeT->cid != cidNil; pcmmeT++)
			{
			if (pcmmeT->cid == cid && (pcmmeT->grfcmm & grfcmmWanted))
				{
				*pcmme = *pcmmeT;
				return fTrue;
				}
			}

		// check for a default function
		if (pcmmeT->pfncmd != pvNil && (pcmmeT->grfcmm & grfcmmWanted) &&
				pcmmeDef == pvNil)
			{
			pcmmeDef = pcmmeT;
			}
		}

	// no specific one found, return the default one
	if (pcmmeDef != pvNil)
		{
		*pcmme = *pcmmeDef;
		return fTrue;
		}
	return fFalse;
}


/***************************************************************************
	Determines whether this command handler can handle the given command. If
	not, returns false (and does nothing else). If so, executes the command
	and returns true.

	NOTE: a true return indicates that the command was handled and should
	not be passed on down the command handler list - regardless of the
	success/failure of the execution of the command. Do not return false
	to indicate that command execution failed.
***************************************************************************/
bool CMH::FDoCmd(PCMD pcmd)
{
	AssertThis(0);
	AssertPo(pcmd, 0);
	CMME cmme;
	ulong grfcmm;

	if (pvNil == pcmd->pcmh)
		grfcmm = fcmmNobody;
	else if (this == pcmd->pcmh)
		grfcmm = fcmmThis;
	else
		grfcmm = fcmmOthers;

	if (!_FGetCmme(pcmd->cid, grfcmm, &cmme) || pvNil == cmme.pfncmd)
		return fFalse;

	return (this->*cmme.pfncmd)(pcmd);
}


/***************************************************************************
	Determines whether the command is enabled. If this command handler
	doesn't normally handle the command, this returns false (and does
	nothing else). Otherwise sets the grfeds and returns true.
***************************************************************************/
bool CMH::FEnableCmd(PCMD pcmd, ulong *pgrfeds)
{
	AssertThis(0);
	AssertPo(pcmd, 0);
	AssertVarMem(pgrfeds);
	CMME cmme;
	ulong grfcmm;

	if (pvNil == pcmd->pcmh)
		grfcmm = fcmmNobody;
	else if (this == pcmd->pcmh)
		grfcmm = fcmmThis;
	else
		grfcmm = fcmmOthers;

	if (!_FGetCmme(pcmd->cid, grfcmm, &cmme) || pvNil == cmme.pfncmd)
		return fFalse;

	if (cmme.pfneds == pvNil)
		{
		if (cidNil == cmme.cid)
			return fFalse;
		*pgrfeds = fedsEnable;
		}
	else if (!(this->*cmme.pfneds)(pcmd, pgrfeds))
		return fFalse;

	return fTrue;
}


/***************************************************************************
	Command dispatcher constructor.
***************************************************************************/
CEX::CEX(void)
{
	AssertBaseThis(0);
}


/***************************************************************************
	Destructor for a CEX.
***************************************************************************/
CEX::~CEX(void)
{
	AssertBaseThis(0);
	CMD cmd;
	long icmd;

	if (pvNil != _pglcmd)
		{
		for (icmd = _pglcmd->IvMac(); icmd-- != 0; )
			{
			_pglcmd->Get(icmd, &cmd);
			ReleasePpo(&cmd.pgg);
			}
		ReleasePpo(&_pglcmd);
		}

	ReleasePpo(&_pglcmhe);
	ReleasePpo(&_pcfl);
	ReleasePpo(&_pglcmdf);
	ReleasePpo(&_cmdCur.pgg);
}


/***************************************************************************
	Static method to create a new CEX object.
***************************************************************************/
PCEX CEX::PcexNew(long ccmdInit, long ccmhInit)
{
	AssertIn(ccmdInit, 0, kcbMax);
	AssertIn(ccmhInit, 0, kcbMax);
	PCEX pcex;

	if (pvNil == (pcex = NewObj CEX))
		return pvNil;

	if (!pcex->_FInit(ccmdInit, ccmhInit))
		ReleasePpo(&pcex);

	AssertNilOrPo(pcex, 0);
	return pcex;
}


/***************************************************************************
	Initialization of the command dispatcher.
***************************************************************************/
bool CEX::_FInit(long ccmdInit, long ccmhInit)
{
	AssertBaseThis(0);
	AssertIn(ccmdInit, 0, kcbMax);
	AssertIn(ccmhInit, 0, kcbMax);

	if (pvNil == (_pglcmd = GL::PglNew(size(CMD), ccmdInit)) ||
			pvNil == (_pglcmhe = GL::PglNew(size(CMHE), ccmhInit)))
		{
		return fFalse;
		}

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Start recording a macro to the given chunky file.
***************************************************************************/
void CEX::Record(PCFL pcfl)
{
	AssertThis(0);
	AssertPo(pcfl, 0);

	if (_rs != rsNormal)
		{
		Bug("already recording or playing");
		return;
		}

	_rs = rsRecording;
	_rec = recNil;
	_pcfl = pcfl;
	_pcfl->AddRef();
	_cno = cnoNil;
	_icmdf = 0;
	_chidLast = 0;
	_cact = 0;
	Assert(_pglcmdf == pvNil, "why isn't _pglcmdf nil?");

	if ((_pglcmdf = GL::PglNew(size(CMDF), 100)) == pvNil)
		_rec = recMemError;
	else if (!_pcfl->FAdd(0, kctgMacro, &_cno))
		{
		_rec = recFileError;
		_cno = cnoNil;
		}
}


/***************************************************************************
	Stop recording a command stream, and write the command stream to
	file. If there were any errors, delete the command stream from the
	chunky file. Pushes a command notifying the world that recording
	has stopped. The command (cidCexRecordDone) contains the error code
	(rec), and cno in the first two lw's of the command. If the
	rec is not recNil, the cno is cnoNil and wasn't actually created.
***************************************************************************/
void CEX::StopRecording(void)
{
	AssertThis(0);
	BLCK blck;

	if (_rs != rsRecording)
		return;

	if (_rec == recNil)
		{
		long cb;

		if (_cact > 1)
			{
			// rewrite the last one's _cact
			CMDF cmdf;

			_pglcmdf->Get(_icmdf, &cmdf);
			cmdf.cact = _cact;
			_pglcmdf->Put(_icmdf, &cmdf);
			}
		cb = _pglcmdf->CbOnFile();
		if (!_pcfl->FPut(cb, kctgMacro, _cno, &blck) ||
				!_pglcmdf->FWrite(&blck))
			{
			_rec = recFileError;
			}
		}

	if (_rec != recNil && _cno != cnoNil)
		{
		_pcfl->Delete(kctgMacro, _cno);
		_cno = cnoNil;
		}

	if (_cno != cnoNil)
		{
		if (_pcfl->FSave(kctgFramework))
			_pcfl->SetTemp(fFalse);
		else
			{
			_rec = recFileError;
			_cno = cnoNil;
			}
		}
	_rs = rsNormal;
	ReleasePpo(&_pglcmdf);
	ReleasePpo(&_pcfl);

	PushCid(cidCexRecordDone, pvNil, pvNil, _rec, _cno);
}


/***************************************************************************
	Record a command.
***************************************************************************/
void CEX::RecordCmd(PCMD pcmd)
{
	AssertThis(0);
	AssertPo(pcmd, 0);
	Assert(_rs == rsRecording, "not recording");
	CMDF cmdf;

	if (_rec != recNil)
		return;

	if (_cact > 0)
		{
		if (_cmd.pgg == pvNil && FEqualRgb(pcmd, &_cmd, size(_cmd)))
			{
			// commands are the same, just increment the _cact
			if (pcmd->cid < cidMinNoRepeat || pcmd->cid >= cidLimNoRepeat)
				_cact++;
			return;
			}

		// new command is not the same as the previous one
		if (_cact > 1)
			{
			//rewrite the previous one's _cact
			_pglcmdf->Get(_icmdf, &cmdf);
			cmdf.cact = _cact;
			_pglcmdf->Put(_icmdf, &cmdf);
			}

		// increment _icmdf
		_icmdf++;
		_cact = 0;
		}

	// fill in the cmdf and save it in the list
	cmdf.cid = pcmd->cid;
	cmdf.hid = pcmd->pcmh == pvNil ? hidNil : pcmd->pcmh->Hid();
	cmdf.cact = 1;
	cmdf.chidGg = pcmd->pgg != pvNil ? ++_chidLast : 0;
	CopyPb(pcmd->rglw, cmdf.rglw, kclwCmd * size(long));

	if (!_pglcmdf->FInsert(_icmdf, &cmdf))
		{
		//out of memory
		_rec = recMemError;
		return;
		}

	// write the group and make it a child of the macro
	if (pvNil != pcmd->pgg)
		{
		BLCK blck;
		long cb;
		CNO cno;

		cb = pcmd->pgg->CbOnFile();
		if (!_pcfl->FAddChild(kctgMacro, _cno, cmdf.chidGg,
				cb, kctgGg, &cno, &blck))
			{
			goto LFileError;
			}
		if (!pcmd->pgg->FWrite(&blck))
			{
			_pcfl->DeleteChild(kctgMacro, _cno, kctgGg, cno, cmdf.chidGg);
LFileError:
			_pglcmdf->Delete(_icmdf);
			_rec = recFileError;
			return;
			}
		}
	_cact = 1;
	_cmd = *pcmd;
}


/***************************************************************************
	Play back the command stream starting in the given pcfl with the given
	cno.
***************************************************************************/
void CEX::Play(PCFL pcfl, CNO cno)
{
	AssertThis(0);
	AssertPo(pcfl, 0);
	BLCK blck;
	short bo, osk;

	if (_rs != rsNormal)
		{
		Bug("already recording or playing");
		return;
		}

	_rs = rsPlaying;
	_rec = recNil;
	_pcfl = pcfl;
	_pcfl->AddRef();
	_cno = cno;
	_icmdf = 0;
	_cact = 0;
	Assert(_pglcmdf == pvNil, "why isn't _pglcmdf nil?");

	if (!_pcfl->FFind(kctgMacro, _cno, &blck) ||
		(_pglcmdf = GL::PglRead(&blck, &bo, &osk)) == pvNil)
		{
		_rec = recFileError;
		StopPlaying();
		}
	else if (bo != kboCur || osk != koskCur)
		{
		_rec = recWrongPlatform;
		StopPlaying();
		}
}


/***************************************************************************
	Stop play back of a command stream. Pushes a command notifying the
	world that play back has stopped. The command (cidCexPlayDone) contains
	the error code (rec), and cno in the first two lw's of the command.
***************************************************************************/
void CEX::StopPlaying(void)
{
	AssertThis(0);

	if (_rs != rsPlaying)
		return;

	if (_rec == recNil && (_cact > 0 || _icmdf < _pglcmdf->IvMac()))
		_rec = recAbort;

	PushCid(cidCexPlayDone, pvNil, pvNil, _rec, _cno);

	_rs = rsNormal;
	ReleasePpo(&_pcfl);
	ReleasePpo(&_pglcmdf);
}


/***************************************************************************
	Read the next command.
***************************************************************************/
bool CEX::_FReadCmd(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	Assert(_rs == rsPlaying, "not playing a command stream");
	AssertPo(_pglcmdf, 0);
	CMDF cmdf;

	if (_cact > 0)
		{
		//this command is being repeated
		*pcmd = _cmd;
		_cact--;
		return fTrue;
		}
	if (_icmdf >= _pglcmdf->IvMac())
		goto LStop;

	_pglcmdf->Get(_icmdf, &cmdf);

	ClearPb(pcmd, size(*pcmd));
	pcmd->cid = cmdf.cid;
	pcmd->pcmh = vpappb->PcmhFromHid(cmdf.hid);
	pcmd->pgg = pvNil;
	CopyPb(cmdf.rglw, pcmd->rglw, kclwCmd * size(long));

	if (cmdf.chidGg != 0)
		{
		BLCK blck;
		KID kid;
		short bo, osk;

		Assert(cmdf.cact <= 1, 0);

		//read the gg
		if (!_pcfl->FGetKidChidCtg(kctgMacro, _cno, cmdf.chidGg,
				kctgGg, &kid) ||
			!_pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck) ||
			pvNil == (pcmd->pgg = GG::PggRead(&blck, &bo, &osk)))
			{
			_rec = recFileError;
			goto LStop;
			}
		if (bo != kboCur || osk != koskCur)
			{
			//don't know how to change byte order or translate strings
			ReleasePpo(&pcmd->pgg);
			_rec = recWrongPlatform;
			goto LStop;
			}
		}
	AssertPo(pcmd, 0);

	_icmdf++;
	if ((_cact = cmdf.cact - 1) > 0)
		_cmd = *pcmd;
	return fTrue;

	//error handling
LStop:
	StopPlaying();
	return fFalse;
}


/***************************************************************************
	Determine whether it's OK to communicate with the CMH. Default is to
	return true iff there is no current modal gob or the cmh is not a gob
	or it is a gob in the tree of the modal gob.
***************************************************************************/
bool CEX::_FCmhOk(PCMH pcmh)
{
	AssertNilOrPo(pcmh, 0);
	PGOB pgob;

	if (pvNil == _pgobModal || pvNil == pcmh || !pcmh->FIs(kclsGOB))
		return fTrue;

	for (pgob = (PGOB)pcmh; pgob != _pgobModal; pgob = pgob->PgobPar())
		{
		if (pvNil == pgob)
			return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	Add a command handler to the filter list. These command handlers get
	a crack at every command whether or not it is for them. grfcmm
	determines which targets the handler will see commands for (as in
	command map entries). The cmhl is a command handler level - indicating
	the priority of the command handler. Handlers with lower cmhl values
	get first crack at commands. It is legal for a handler to be in the
	list more than once (even with the same cmhl value).
***************************************************************************/
bool CEX::FAddCmh(PCMH pcmh, long cmhl, ulong grfcmm)
{
	AssertThis(0);
	AssertPo(pcmh, 0);
	CMHE cmhe;
	long icmhe;

	if (fcmmNil == (grfcmm & kgrfcmmAll))
		{
		// no sense adding this
		Bug("why is grfcmm nil?");
		return fFalse;
		}

	if (!_FCmhOk(pcmh))
		return fFalse;

	_FFindCmhl(cmhl, &icmhe);
	cmhe.pcmh = pcmh;
	cmhe.cmhl = cmhl;
	cmhe.grfcmm = grfcmm;
	if (!_pglcmhe->FInsert(icmhe, &cmhe))
		return fFalse;
	if (icmhe <= _icmheNext)
		_icmheNext++;
	return fTrue;
}


/***************************************************************************
	Removes the the handler (at the given cmhl level) from the handler list.
***************************************************************************/
void CEX::RemoveCmh(PCMH pcmh, long cmhl)
{
	AssertThis(0);
	AssertPo(pcmh, 0);
	long icmhe, ccmhe;
	CMHE cmhe;

	if (!_FFindCmhl(cmhl, &icmhe))
		return;

	for (ccmhe = _pglcmhe->IvMac(); icmhe < ccmhe; icmhe++)
		{
		_pglcmhe->Get(icmhe, &cmhe);
		if (cmhe.cmhl != cmhl)
			break;
		if (cmhe.pcmh == pcmh)
			{
			_pglcmhe->Delete(icmhe);
			if (icmhe < _icmheNext)
				_icmheNext--;
			break;
			}
		}
}


/***************************************************************************
	Remove all references to the handler from the command dispatcher,
	including from the handler list and the command queue.
***************************************************************************/
void CEX::BuryCmh(PCMH pcmh)
{
	AssertThis(0);
	Assert(pcmh != pvNil, 0);
	long icmhe, icmd;
	CMHE cmhe;
	CMD cmd;

	if (_pgobModal == pcmh)
		_pgobModal = pvNil;

	if (_pgobTrack == pcmh)
		{
#ifdef WIN
		if (hNil != _hwndCapture && GetCapture() == _hwndCapture)
			ReleaseCapture();
		_hwndCapture = hNil;
#endif //WIN
		_pgobTrack = pvNil;
		}
	if (_cmdCur.pcmh == pcmh)
		_cmdCur.pcmh = pvNil;
	if (_cmd.pcmh == pcmh)
		{
		_cmd.pcmh = pvNil;
		_cact = 0;
		}

	for (icmhe = _pglcmhe->IvMac(); icmhe-- != 0; )
		{
		_pglcmhe->Get(icmhe, &cmhe);
		if (cmhe.pcmh == pcmh)
			{
			_pglcmhe->Delete(icmhe);
			if (icmhe < _icmheNext)
				_icmheNext--;
			}
		}

	for (icmd = _pglcmd->IvMac(); icmd-- != 0; )
		{
		_pglcmd->Get(icmd, &cmd);
		if (cmd.pcmh == pcmh)
			{
			_pglcmd->Delete(icmd);
			ReleasePpo(&cmd.pgg);
			}
		}
}


/***************************************************************************
	Finds the first item with the given cmhl in the handler list. If there
	aren't any, still sets *picmhe to where they would be.
***************************************************************************/
bool CEX::_FFindCmhl(long cmhl, long *picmhe)
{
	AssertThis(0);
	AssertVarMem(picmhe);
	long icmhe, icmheMin, icmheLim;
	CMHE *qrgcmhe;

	qrgcmhe = (CMHE *)_pglcmhe->QvGet(0);
	for (icmheMin = 0, icmheLim = _pglcmhe->IvMac(); icmheMin < icmheLim; )
		{
		icmhe = (icmheMin + icmheLim) / 2;
		if (qrgcmhe[icmhe].cmhl < cmhl)
			icmheMin = icmhe + 1;
		else
			icmheLim = icmhe;
		}

	*picmhe = icmheMin;
	return icmheMin < _pglcmhe->IvMac() && qrgcmhe[icmheMin].cmhl == cmhl;
}


/***************************************************************************
	Adds a command to the tail of the queue.
***************************************************************************/
void CEX::EnqueueCid(long cid, PCMH pcmh, PGG pgg,
	long lw0, long lw1, long lw2, long lw3)
{
	Assert(cid != cidNil, 0);
	AssertNilOrPo(pcmh, 0);
	AssertNilOrPo(pgg, 0);
	CMD cmd;

	cmd.cid = cid;
	cmd.pcmh = pcmh;
	cmd.pgg = pgg;
	cmd.rglw[0] = lw0;
	cmd.rglw[1] = lw1;
	cmd.rglw[2] = lw2;
	cmd.rglw[3] = lw3;
	EnqueueCmd(&cmd);
}


/***************************************************************************
	Pushes a command onto the head of the queue.
***************************************************************************/
void CEX::PushCid(long cid, PCMH pcmh, PGG pgg,
	long lw0, long lw1, long lw2, long lw3)
{
	Assert(cid != cidNil, 0);
	AssertNilOrPo(pcmh, 0);
	AssertNilOrPo(pgg, 0);
	CMD cmd;

	cmd.cid = cid;
	cmd.pcmh = pcmh;
	cmd.pgg = pgg;
	cmd.rglw[0] = lw0;
	cmd.rglw[1] = lw1;
	cmd.rglw[2] = lw2;
	cmd.rglw[3] = lw3;
	PushCmd(&cmd);
}


/***************************************************************************
	Adds a command to the tail of the queue. This asserts if it can't add
	it to the queue. Clients should make sure that the value of ccmdInit
	passed to PcexNew is large enough to handle the busiest session.
***************************************************************************/
void CEX::EnqueueCmd(PCMD pcmd)
{
	AssertThis(0);
	AssertPo(pcmd, 0);
	Assert(pcmd->cid != cidNil, "why enqueue a nil command?");

	if (!_pglcmd->FEnqueue(pcmd))
		{
		Bug("event queue not big enough");
		ReleasePpo(&pcmd->pgg);
		}
#ifdef DEBUG
	if (_ccmdMax < _pglcmd->IvMac())
		_ccmdMax = _pglcmd->IvMac();
#endif //DEBUG
}


/***************************************************************************
	Pushes a command onto the head of the queue. This asserts if it can't
	add it to the queue. Clients should make sure that the value of ccmdInit
	passed to PcexNew is large enough to handle the busiest session.
***************************************************************************/
void CEX::PushCmd(PCMD pcmd)
{
	AssertThis(0);
	AssertPo(pcmd, 0);
	Assert(pcmd->cid != cidNil, "why enqueue a nil command?");

	if (!_pglcmd->FPush(pcmd))
		{
		Bug("event queue not big enough");
		ReleasePpo(&pcmd->pgg);
		}
#ifdef DEBUG
	if (_ccmdMax < _pglcmd->IvMac())
		_ccmdMax = _pglcmd->IvMac();
#endif //DEBUG
}


/***************************************************************************
	Checks if a cid is in the queue.
***************************************************************************/
bool CEX::FCidIn(long cid)
{
	AssertThis(0);
	Assert(cid != cidNil, "why check for a nil command?");

	long icmd;
	CMD cmd;

	for (icmd = _pglcmd->IvMac(); icmd-- > 0; )
		{
		_pglcmd->Get(icmd, &cmd);
		if (cmd.cid == cid)
			return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	Flushes all instances of a cid in the queue.
***************************************************************************/
void CEX::FlushCid(long cid)
{
	AssertThis(0);
	Assert(cid != cidNil, "why flush a nil command?");

	long icmd;
	CMD cmd;

	for (icmd = _pglcmd->IvMac(); icmd-- > 0; )
		{
		_pglcmd->Get(icmd, &cmd);
		if (cmd.cid == cid)
			{
			_pglcmd->Delete(icmd);
			ReleasePpo(&cmd.pgg);
			}
		}
}


/***************************************************************************
	Get the next command to be dispatched (put it in _cmdCur). Return tYes
	if there was a command and it should be dispatched. Return tNo if there
	wasn't a command (if the system queue should be checked). Return tMaybe
	if the command shouldn't be dispatched, but we shouldn't check the
	system queue.
***************************************************************************/
bool CEX::_TGetNextCmd(void)
{
	AssertThis(0);

	// get the next command from the command stream
	if (!_pglcmd->FPop(&_cmdCur))
		{
		ClearPb(&_cmdCur, size(_cmdCur));
		if (pvNil == _pgobTrack)
			return tNo;

		AssertPo(_pgobTrack, 0);
		PT pt;
		PCMD_MOUSE pcmd = (PCMD_MOUSE)&_cmdCur;

		_cmdCur.pcmh = _pgobTrack;
		_cmdCur.cid = cidTrackMouse;
		vpappb->TrackMouse(_pgobTrack, &pt);
		pcmd->xp = pt.xp;
		pcmd->yp = pt.yp;
		pcmd->grfcust = vpappb->GrfcustCur();

		if (!vpappb->FForeground())
			{
			// if we're not in the foreground, toggle the state of
			// fcustMouse repeatedly. Most of the time, this will cause
			// the client to stop tracking the mouse. Clients
			// whose tracking state depends on something other than
			// the mouse state should call vpappb->FForeground() to
			// determine if tracking should be aborted.
			static bool _fDown;

			_fDown = !_fDown;
			if (!_fDown)
				pcmd->grfcust ^= fcustMouse;
			else
				pcmd->grfcust &= ~fcustMouse;
			}
		}
	AssertPo(&_cmdCur, 0);

	// handle playing and recording
	if (rsPlaying == _rs)
		{
		// We're playing back. Throw away the incoming command and play
		// one from the stream.
		ReleasePpo(&_cmdCur.pgg);

		// let a cidCexStopPlay go through and handle it at the end.
		// this is so a cmh can intercept it.
		if (_cmdCur.cid != cidCexStopPlay && !_FReadCmd(&_cmdCur))
			return tMaybe;
		}

	if (!_FCmhOk(_cmdCur.pcmh))
		{
		vpappb->BadModalCmd(&_cmdCur);
		ReleasePpo(&_cmdCur.pgg);
		return tMaybe;
		}

	return tYes;
}


/***************************************************************************
	Send the command (_cmdCur) to the given command handler.
***************************************************************************/
bool CEX::_FSendCmd(PCMH pcmh)
{
	AssertPo(pcmh, 0);

	if (!_FCmhOk(pcmh))
		return fFalse;

	return pcmh->FDoCmd(&_cmdCur);
}


/***************************************************************************
	Handle post processing on the command - record it if we're recording,
	free the pgg, etc.
***************************************************************************/
void CEX::_CleanUpCmd(void)
{
	// If the handler went away during command dispatching, we should
	// have heard about it (via BuryCmh) and should have set _cmdCur.pcmh
	// to nil.
	AssertNilOrPo(_cmdCur.pcmh, 0);

	// record the command after dispatching, in case arguments got added
	// or the cid was set to nil.
	if (rsRecording == _rs && !FIn(_cmdCur.cid, cidMinNoRecord, cidLimNoRecord)
			&& cidNil != _cmdCur.cid && cidCexStopRec != _cmdCur.cid)
		{
		RecordCmd(&_cmdCur);
		}

	// check for a stop record or stop play command
	if (rsNormal != _rs)
		{
		if (cidCexStopPlay == _cmdCur.cid && rsPlaying == _rs)
			StopPlaying();
		else if (cidCexStopRec == _cmdCur.cid && rsRecording == _rs)
			StopRecording();
		}

	ReleasePpo(&_cmdCur.pgg);
}


/***************************************************************************
	If there is a command in the queue, this dispatches it and returns
	true. If there aren't any commands in the queue, it simply returns
	false. If a gob is tracking the mouse and the queue is empty, a
	cidTrackMouse command is generated and dispatched to the gob.

	NOTE: care has to be taken here because a CMH may go away while
	dispatching the command. That's why _cmdCur and _icmheNext are
	member variables - so BuryCmh can adjust them if needed.
***************************************************************************/
bool CEX::FDispatchNextCmd(void)
{
	AssertThis(0);
	CMHE cmhe;
	bool fHandled;
	bool tRet;

	if (_fDispatching)
		{
		Bug("recursing into FDispatchNextCmd!");
		return fFalse;
		}
	_fDispatching = fTrue;

	tRet = _TGetNextCmd();
	if (tYes != tRet)
		{
		_fDispatching = fFalse;
		return tRet != tNo;
		}

	// pipe it through the command handlers, then to the target
	fHandled = fFalse;
	for (_icmheNext = 0; _icmheNext < _pglcmhe->IvMac() && !fHandled; )
		{
		_pglcmhe->Get(_icmheNext++, &cmhe);
		if (pvNil == _cmdCur.pcmh)
			{
			if (!(cmhe.grfcmm & fcmmNobody))
				continue;
			}
		else if (cmhe.pcmh == _cmdCur.pcmh)
			{
			if (!(cmhe.grfcmm & fcmmThis))
				continue;
			}
		else if (!(cmhe.grfcmm & fcmmOthers))
			continue;

		fHandled = _FSendCmd(cmhe.pcmh);
		}

	if (!fHandled && pvNil != _cmdCur.pcmh)
		{
		AssertPo(_cmdCur.pcmh, 0);
		fHandled = _FSendCmd(_cmdCur.pcmh);
		}

	_CleanUpCmd();

	_fDispatching = fFalse;
	return fTrue;
}


/***************************************************************************
	Give the handler a crack at enabling/disabling the command.
***************************************************************************/
bool CEX::_FEnableCmd(PCMH pcmh, PCMD pcmd, ulong *pgrfeds)
{
	AssertPo(pcmh, 0);
	AssertPo(pcmd, 0);
	AssertVarMem(pgrfeds);

	if (!_FCmhOk(pcmh))
		return fFalse;

	return pcmh->FEnableCmd(pcmd, pgrfeds);
}


/***************************************************************************
	Determines whether the given command is currently enabled. This is
	normally used for menu graying/checking etc and toolbar enabling/status.
***************************************************************************/
bool CEX::GrfedsForCmd(PCMD pcmd)
{
	AssertThis(0);
	AssertPo(pcmd, 0);
	long icmhe, ccmhe;
	CMHE cmhe;
	ulong grfeds;

	// pipe it through the command handlers, then to the target
	for (icmhe = 0, ccmhe = _pglcmhe->IvMac(); icmhe < ccmhe; icmhe++)
		{
		_pglcmhe->Get(icmhe, &cmhe);
		grfeds = fedsNil;
		if (_FEnableCmd(cmhe.pcmh, pcmd, &grfeds))
			goto LDone;
		}
	if (pcmd->pcmh != pvNil)
		{
		AssertPo(pcmd->pcmh, 0);
		grfeds = fedsNil;
		if (_FEnableCmd(pcmd->pcmh, pcmd, &grfeds))
			goto LDone;
		}

	// handle the CEX commands
	switch (pcmd->cid)
		{
	case cidCexStopRec:
		grfeds = rsRecording == _rs ? fedsEnable : fedsDisable;
		break;

	case cidCexStopPlay:
		grfeds = rsPlaying == _rs ? fedsEnable : fedsDisable;
		break;

	default:
		grfeds = fedsDisable;
		break;
		}

LDone:
	return grfeds;
}


/***************************************************************************
	Determines whether the given command is currently enabled. This is
	normally used for menu graying/checking etc and toolbar enabling/status.
***************************************************************************/
bool CEX::GrfedsForCid(long cid, PCMH pcmh, PGG pgg,
	long lw0, long lw1, long lw2, long lw3)
{
	AssertThis(0);
	Assert(cid != cidNil, 0);
	AssertNilOrPo(pcmh, 0);
	AssertNilOrPo(pgg, 0);
	CMD cmd;

	cmd.cid = cid;
	cmd.pcmh = pcmh;
	cmd.pgg = pgg;
	cmd.rglw[0] = lw0;
	cmd.rglw[1] = lw1;
	cmd.rglw[2] = lw2;
	cmd.rglw[3] = lw3;
	return GrfedsForCmd(&cmd);
}


/***************************************************************************
	See if the next command is a key command and if so, put it in *pcmd
	(and remove it from the queue).
***************************************************************************/
bool CEX::FGetNextKey(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	long iv;

	if (_rs != rsNormal)
		goto LFail;
	if ((iv = _pglcmd->IvMac()) > 0)
		{
		//get next cmd
		_pglcmd->Get(iv - 1, pcmd);
		if (pcmd->cid == cidKey)
			{
			AssertDo(_pglcmd->FPop(pvNil), 0);
			return fTrue;
			}
LFail:
		TrashVar(pcmd);
		return fFalse;
		}

	return vpappb->FGetNextKeyFromOsQueue((PCMD_KEY)pcmd);
}


/***************************************************************************
	The given GOB wants to track the mouse.
***************************************************************************/
void CEX::TrackMouse(PGOB pgob)
{
	AssertThis(0);
	AssertPo(pgob, 0);
	Assert(_pgobTrack == pvNil, "some other gob is already tracking the mouse");

	_pgobTrack = pgob;
#ifdef WIN
	_hwndCapture = pgob->HwndContainer();
	SetCapture(_hwndCapture);
#endif //WIN
}


/***************************************************************************
	Stop tracking the mouse.
***************************************************************************/
void CEX::EndMouseTracking(void)
{
	AssertThis(0);

#ifdef WIN
	if (pvNil != _pgobTrack)
		{
		if (hNil != _hwndCapture && GetCapture() == _hwndCapture)
			ReleaseCapture();
		_hwndCapture = hNil;
		}
#endif //WIN
	_pgobTrack = pvNil;
}


/***************************************************************************
	Return the gob that is tracking the mouse.
***************************************************************************/
PGOB CEX::PgobTracking(void)
{
	AssertThis(0);
	return _pgobTrack;
}


/***************************************************************************
	Suspend or resume the command dispatcher. All this does is
	release (capture) the mouse if we're current tracking the mouse and
	we're being suspended (resumed).
***************************************************************************/
void CEX::Suspend(bool fSuspend)
{
	AssertThis(0);

#ifdef WIN
	if (pvNil == _pgobTrack || hNil == _hwndCapture)
		return;

	if (fSuspend && GetCapture() == _hwndCapture)
		ReleaseCapture();
	else if (!fSuspend && GetCapture() != _hwndCapture)
		SetCapture(_hwndCapture);
#endif //WIN
}


/***************************************************************************
	Set the modal GOB.
***************************************************************************/
void CEX::SetModalGob(PGOB pgob)
{
	AssertThis(0);
	AssertNilOrPo(pgob, 0);

	_pgobModal = pgob;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the command dispatcher
***************************************************************************/
void CEX::AssertValid(ulong grf)
{
	CEX_PAR::AssertValid(fobjAllocated);
	AssertPo(_pglcmhe, 0);
	AssertPo(_pglcmd, 0);
	AssertNilOrPo(_pglcmdf, 0);
	AssertNilOrPo(_pcfl, 0);
	AssertNilOrPo(_cmdCur.pgg, 0);
}


/***************************************************************************
	Mark the memory associated with the command dispatcher.
***************************************************************************/
void CEX::MarkMem(void)
{
	AssertThis(0);
	CMD cmd;
	long icmd;

	CEX_PAR::MarkMem();
	MarkMemObj(_pglcmhe);
	MarkMemObj(_pglcmd);
	MarkMemObj(_pglcmdf);
	MarkMemObj(_cmdCur.pgg);

	for (icmd = _pglcmd->IvMac(); icmd-- != 0; )
		{
		_pglcmd->Get(icmd, &cmd);
		if (cmd.pgg != pvNil)
			MarkMemObj(cmd.pgg);
		}
}
#endif //DEBUG


