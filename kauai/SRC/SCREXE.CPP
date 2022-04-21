/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Script interpreter.  See scrcom.cpp for an explanation of the format
	of compiled scripts.

***************************************************************************/
#include "util.h"
ASSERTNAME

RTCLASS(SCEB)
RTCLASS(SCPT)
RTCLASS(STRG)


#ifdef DEBUG
// these strings are for debug only error messages
static STN _stn;
#endif //DEBUG


/***************************************************************************
	Constructor for the script interpreter.
***************************************************************************/
SCEB::SCEB(PRCA prca, PSTRG pstrg)
{
	AssertNilOrPo(prca, 0);
	AssertNilOrPo(pstrg, 0);

	_pgllwStack = pvNil;
	_pglrtvm = pvNil;
	_pscpt = pvNil;
	_fPaused = fFalse;

	_prca = prca;
	if (pvNil != _prca)
		_prca->AddRef();
	_pstrg = pstrg;
	if (pvNil != _pstrg)
		_pstrg->AddRef();

	AssertThis(0);
}


/***************************************************************************
	Destructor for the script interpreter.
***************************************************************************/
SCEB::~SCEB(void)
{
	Free();
	ReleasePpo(&_prca);
	ReleasePpo(&_pstrg);
}


/***************************************************************************
	Free our claim to all this stuff.
***************************************************************************/
void SCEB::Free(void)
{
	AssertThis(0);

	// nuke literal strings left in the global string table
	if (pvNil != _pscpt && pvNil != _pstrg && pvNil != _pscpt->_pgstLiterals &&
			pvNil != _pglrtvm)
		{
		RTVN rtvn;
		long stid;

		rtvn.lu1 = 0;
		for (rtvn.lu2 = _pscpt->_pgstLiterals->IvMac(); rtvn.lu2-- > 0; )
			{
			if (FFindRtvm(_pglrtvm, &rtvn, &stid, pvNil))
				_pstrg->Delete(stid);
			}
		}

	ReleasePpo(&_pgllwStack);
	ReleasePpo(&_pglrtvm);
	ReleasePpo(&_pscpt);
	_fPaused = fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a SCEB.
***************************************************************************/
void SCEB::AssertValid(ulong grfsceb)
{
	SCEB_PAR::AssertValid(0);
	if (grfsceb & fscebRunnable)
		{
		Assert(pvNil != _pgllwStack, "nil stack");
		Assert(pvNil != _pscpt, "nil script");
		Assert(_ilwMac == _pscpt->_pgllw->IvMac(), 0);
		AssertIn(_ilwCur, 1, _ilwMac + 1);
		Assert(!_fError, 0);
		}
	AssertNilOrPo(_pgllwStack, 0);
	AssertNilOrPo(_pscpt, 0);
	AssertNilOrPo(_pglrtvm, 0);
	AssertNilOrPo(_pstrg, 0);
	AssertNilOrPo(_prca, 0);
}


/***************************************************************************
	Mark memory for the SCEB.
***************************************************************************/
void SCEB::MarkMem(void)
{
	AssertValid(0);
	SCEB_PAR::MarkMem();
	MarkMemObj(_pgllwStack);
	MarkMemObj(_pscpt);
	MarkMemObj(_pglrtvm);
	MarkMemObj(_prca);
}
#endif //DEBUG


/***************************************************************************
	Run the given script.  (prglw, clw) is the list of parameters for the
	script.
***************************************************************************/
bool SCEB::FRunScript(PSCPT pscpt, long *prglw, long clw,
	long *plwReturn, bool *pfPaused)
{
	AssertThis(0);
	return FAttachScript(pscpt, prglw, clw) && FResume(plwReturn, pfPaused);
}


/***************************************************************************
	Attach a script to this SCEB and pause the script.
***************************************************************************/
bool SCEB::FAttachScript(PSCPT pscpt, long *prglw, long clw)
{
	AssertThis(0);
	AssertPo(pscpt, 0);
	AssertIn(clw, 0, kcbMax);
	AssertPvCb(prglw, LwMul(clw, size(long)));
	long lw;
	DVER dver;

	Free();
	_lwReturn = 0;
	_fError = fFalse;

	//create the stack GL
	if (pvNil == (_pgllwStack = GL::PglNew(size(long), 10)))
		goto LFail;
	_pgllwStack->SetMinGrow(10);

	//stake our claim on the code GL.
	_pscpt = pscpt;
	_pscpt->AddRef();

	//check the version
	//get and check the version number
	if ((_ilwMac = _pscpt->_pgllw->IvMac()) < 1)
		{
		Bug("No version info on script");
		goto LFail;
		}
	_pscpt->_pgllw->Get(0, &lw);
	dver.Set(SwHigh(lw), SwLow(lw));
	if (!dver.FReadable(_SwCur(), _SwMin()))
		{
		Bug("Script version doesn't match script interpreter version");
		goto LFail;
		}

	//add the parameters and literal strings
	if (clw > 0)
		_AddParameters(prglw, clw);
	if (pvNil != _pscpt->_pgstLiterals && !_fError)
		_AddStrings(_pscpt->_pgstLiterals);

	if (_fError)
		{
LFail:
		Free();
		return fFalse;
		}

	//set the pc and claim we're paused
	_ilwCur = 1;
	_fPaused = fTrue;
	AssertThis(fscebRunnable);
	return fTrue;
}


/***************************************************************************
	Resume a paused script.
***************************************************************************/
bool SCEB::FResume(long *plwReturn, bool *pfPaused)
{
	AssertThis(fscebRunnable);
	AssertNilOrVarMem(plwReturn);
	AssertNilOrVarMem(pfPaused);
	RTVN rtvn;
	long ilw, clwPush;
	long lw;
	long op;

	TrashVar(plwReturn);
	TrashVar(pfPaused);
	if (!_fPaused || _fError)
		{
		Bug("script not paused");
		goto LFail;
		}

	AssertIn(_ilwCur, 1, _ilwMac + 1);
	for (_fPaused = fFalse; _ilwCur < _ilwMac && !_fError && !_fPaused; )
		{
		lw = *(long *)_pscpt->_pgllw->QvGet(_ilwCur++);
		clwPush = B2Lw(lw);
		if (!FIn(clwPush, 0, _ilwMac - _ilwCur + 1))
			{
			Bug("bad instruction");
			goto LFail;
			}

		ilw = _ilwCur;
		if (opNil != (op = B3Lw(lw)))
			{
			//this instruction acts on a variable
			if (clwPush == 0)
				{
				Bug("bad var instruction");
				goto LFail;
				}
			clwPush--;
			rtvn.lu1 = (ulong)lw & 0x0000FFFF;
			rtvn.lu2 = *(ulong *)_pscpt->_pgllw->QvGet(_ilwCur++);
			ilw = _ilwCur;
			if (!_FExecVarOp(op, &rtvn))
				goto LFail;
			}
		else if (opNil != (op = SuLow(lw)))
			{
			//normal opcode
			if (!_FExecOp(op))
				goto LFail;
			}

		//push the stack stuff (if we didn't do a jump)
		if (clwPush > 0 && ilw == _ilwCur)
			{
			ilw = _pgllwStack->IvMac();
			if (!_pgllwStack->FSetIvMac(ilw + clwPush))
				{
LFail:
				_Error(fFalse);
				break;
				}
			CopyPb(_pscpt->_pgllw->QvGet(_ilwCur), _pgllwStack->QvGet(ilw),
				LwMul(clwPush, size(long)));
			_ilwCur += clwPush;
			}
		}

	if (_ilwCur >= _ilwMac || _fError)
		_fPaused = fFalse;
	if (!_fPaused)
		Free();
	if (!_fError && pvNil != plwReturn)
		*plwReturn = _lwReturn;
	if (pvNil != pfPaused)
		*pfPaused = _fPaused;

	AssertThis(0);
	return !_fError;
}


/***************************************************************************
	Put the parameters in the local variable list.
***************************************************************************/
void SCEB::_AddParameters(long *prglw, long clw)
{
	AssertThis(0);
	AssertIn(clw, 1, kcbMax);
	AssertPvCb(prglw, LwMul(clw, size(long)));
	STN stn;
	long ilw;
	RTVN rtvn;

	//put the parameters in the local variable gl
	stn = PszLit("_cparm");
	rtvn.SetFromStn(&stn);
	_AssignVar(&_pglrtvm, &rtvn, clw);
	stn = PszLit("_parm");
	rtvn.SetFromStn(&stn);
	for (ilw = 0; ilw < clw; ilw++)
		{
		rtvn.lu1 = LwHighLow(SwLow(ilw), SwLow(rtvn.lu1));
		_AssignVar(&_pglrtvm, &rtvn, prglw[ilw]);
		}
}


/***************************************************************************
	Put the literal strings into the registry.  And assign the string id's
	to the internal string variables.
***************************************************************************/
void SCEB::_AddStrings(PGST pgst)
{
	AssertThis(0);
	AssertPo(pgst, 0);
	RTVN rtvn;
	long stid;
	STN stn;

	if (pvNil == _pstrg)
		{
		_Error(fFalse);
		return;
		}

	rtvn.lu1 = 0;
	for (rtvn.lu2 = 0; (long)rtvn.lu2 < pgst->IvMac(); rtvn.lu2++)
		{
		pgst->GetStn(rtvn.lu2, &stn);
		if (!_pstrg->FAdd(&stid, &stn))
			{
			_Error(fFalse);
			break;
			}
		_AssignVar(&_pglrtvm, &rtvn, stid);
		if (_fError)
			{
			_pstrg->Delete(stid);
			break;
			}
		}
}


/***************************************************************************
	Return the current version number of the script compiler.
***************************************************************************/
short SCEB::_SwCur(void)
{
	AssertBaseThis(0);
	return kswCurSccb;
}


/***************************************************************************
	Return the min version number of the script compiler.  Read can read
	scripts back to this version.
***************************************************************************/
short SCEB::_SwMin(void)
{
	AssertBaseThis(0);
	return kswMinSccb;
}


/***************************************************************************
	Execute an instruction that has a variable as an argument.
***************************************************************************/
bool SCEB::_FExecVarOp(long op, RTVN *prtvn)
{
	AssertThis(0);
	AssertVarMem(prtvn);
	long lw;

	if (FIn(op, kopMinArray, kopLimArray))
		{
		// an array access, munge the rtvn
		lw = _LwPop();
		if (_fError)
			return fFalse;
		prtvn->lu1 = LwHighLow(SwLow(lw), SwLow(prtvn->lu1));
		op += kopPushLocVar - kopPushLocArray;
		}

	switch (op)
		{
	case kopPushLocVar:
		_PushVar(_pglrtvm, prtvn);
		break;
	case kopPopLocVar:
		_AssignVar(&_pglrtvm, prtvn, _LwPop());
		break;
	case kopPushThisVar:
		_PushVar(_PglrtvmThis(), prtvn);
		break;
	case kopPopThisVar:
		_AssignVar(_PpglrtvmThis(), prtvn, _LwPop());
		break;
	case kopPushGlobalVar:
		_PushVar(_PglrtvmGlobal(), prtvn);
		break;
	case kopPopGlobalVar:
		_AssignVar(_PpglrtvmGlobal(), prtvn, _LwPop());
		break;
	case kopPushRemoteVar:
		lw = _LwPop();
		if (!_fError)
			_PushVar(_PglrtvmRemote(lw), prtvn);
		break;
	case kopPopRemoteVar:
		lw = _LwPop();
		if (!_fError)
			_AssignVar(_PpglrtvmRemote(lw), prtvn, _LwPop());
		break;
	default:
		_Error(fTrue);
		break;
		}
	return !_fError;
}


/***************************************************************************
	Execute an instruction.
***************************************************************************/
bool SCEB::_FExecOp(long op)
{
	AssertThis(0);
	double dou;
	long lw1, lw2, lw3;

	// OP's that don't have any arguments
	switch (op)
		{
	case kopExit:
		//jump to the end
		_ilwCur = _ilwMac;
		return fTrue;
	case kopNextCard:
		_Push(vsflUtil.LwNext(0));
		return fTrue;
	case kopPause:
		_fPaused = fTrue;
		return fTrue;
		}

	// OP's that have at least one argument
	lw1 = _LwPop();
	switch (op)
		{
	case kopAdd:
		_Push(_LwPop() + lw1);
		break;
	case kopSub:
		_Push(_LwPop() - lw1);
		break;
	case kopMul:
		_Push(_LwPop() * lw1);
		break;
	case kopDiv:
		if (lw1 == 0)
			_Error(fTrue);
		else
			_Push(_LwPop() / lw1);
		break;
	case kopMod:
		if (lw1 == 0)
			_Error(fTrue);
		else
			_Push(_LwPop() % lw1);
		break;
	case kopNeg:
		_Push(-lw1);
		break;
	case kopInc:
		_Push(lw1 + 1);
		break;
	case kopDec:
		_Push(lw1 - 1);
		break;
	case kopShr:
		_Push((ulong)_LwPop() >> lw1);
		break;
	case kopShl:
		_Push((ulong)_LwPop() << lw1);
		break;
	case kopBOr:
		_Push(_LwPop() | lw1);
		break;
	case kopBAnd:
		_Push(_LwPop() & lw1);
		break;
	case kopBXor:
		_Push(_LwPop() ^ lw1);
		break;
	case kopBNot:
		_Push(~lw1);
		break;
	case kopLXor:
		_Push(FPure(_LwPop()) != FPure(lw1));
		break;
	case kopLNot:
		_Push(!lw1);
		break;
	case kopEq:
		_Push(_LwPop() == lw1);
		break;
	case kopNe:
		_Push(_LwPop() != lw1);
		break;
	case kopGt:
		_Push(_LwPop() > lw1);
		break;
	case kopLt:
		_Push(_LwPop() < lw1);
		break;
	case kopGe:
		_Push(_LwPop() >= lw1);
		break;
	case kopLe:
		_Push(_LwPop() <= lw1);
		break;
	case kopAbs:
		_Push(LwAbs(lw1));
		break;
	case kopRnd:
		if (lw1 <= 0)
			_Error(fTrue);
		else
			_Push(vrndUtil.LwNext(lw1));
		break;
	case kopMulDiv:
		lw2 = _LwPop();
		lw3 = _LwPop();
		if (lw3 == 0)
			_Error(fTrue);
		else
			{
			dou = (double)lw1 * lw2 / lw3;
			if (dou < (double)klwMin || dou > (double)klwMax)
				_Error(fTrue);
			else
				_Push((long)dou);
			}
		break;
	case kopDup:
		_Push(lw1);
		_Push(lw1);
		break;
	case kopPop:
		break;
	case kopSwap:
		lw2 = _LwPop();
		_Push(lw1);
		_Push(lw2);
		break;
	case kopRot:
		lw2 = _LwPop();
		_Rotate(lw2, lw1);
		break;
	case kopRev:
		_Reverse(lw1);
		break;
	case kopDupList:
		_DupList(lw1);
		break;
	case kopPopList:
		_PopList(lw1);
		break;
	case kopRndList:
		_RndList(lw1);
		break;
	case kopSelect:
		_Select(lw1, _LwPop());
		break;
	case kopGoEq:
		lw1 = (_LwPop() == lw1);
		goto LGoNz;
	case kopGoNe:
		lw1 = (_LwPop() != lw1);
		goto LGoNz;
	case kopGoGt:
		lw1 = (_LwPop() > lw1);
		goto LGoNz;
	case kopGoLt:
		lw1 = (_LwPop() < lw1);
		goto LGoNz;
	case kopGoGe:
		lw1 = (_LwPop() >= lw1);
		goto LGoNz;
	case kopGoLe:
		lw1 = (_LwPop() <= lw1);
		goto LGoNz;
	case kopGoZ:
		lw1 = !lw1;
		//fall through
	case kopGoNz:
LGoNz:
		lw2 = _LwPop();
		//labels should have their high byte equal to kbLabel
		if (B3Lw(lw2) != kbLabel || (lw2 &= 0x00FFFFFF) > _ilwMac)
			_Error(fTrue);
		else if (lw1 != 0)
			{
			//perform the goto
			_ilwCur = lw2;
			}
		break;
	case kopGo:
		//labels should have their high byte equal to kbLabel
		if (B3Lw(lw1) != kbLabel || (lw1 &= 0x00FFFFFF) > _ilwMac)
			_Error(fTrue);
		else
			{
			//perform the goto
			_ilwCur = lw1;
			}
		break;
	case kopReturn:
		//jump to the end
		_ilwCur = _ilwMac;
		//fall through
	case kopSetReturn:
		_lwReturn = lw1;
		break;
	case kopShuffle:
		if (lw1 <= 0)
			_Error(fTrue);
		else
			vsflUtil.Shuffle(lw1);
		break;
	case kopShuffleList:
		if (lw1 <= 0)
			_Error(fTrue);
		else
			{
			long *prglw;

			_pgllwStack->Lock();
			prglw = _QlwGet(lw1);
			if (pvNil != prglw)
				vsflUtil.ShuffleRglw(lw1, prglw);
			_pgllwStack->Unlock();
			}
		break;
	case kopMatch:
		_Match(lw1);
		break;
	case kopCopyStr:
		_CopySubStr(lw1, 0, kcchMaxStn, _LwPop());
		break;
	case kopMoveStr:
		if (pvNil == _pstrg)
			_Error(fTrue);
		else if (_pstrg->FMove(lw1, lw2 = _LwPop()))
			_Push(lw2);
		else
			_Push(stidNil);
		break;
	case kopNukeStr:
		if (pvNil == _pstrg)
			_Error(fTrue);
		else
			_pstrg->Delete(lw1);
		break;
	case kopMergeStrs:
		_MergeStrings(lw1, _LwPop());
		break;
	case kopScaleTime:
		vpusac->Scale(lw1);
		break;
	case kopNumToStr:
		_NumToStr(lw1, _LwPop());
		break;
	case kopStrToNum:
		lw2 = _LwPop();
		_StrToNum(lw1, lw2, _LwPop());
		break;
	case kopConcatStrs:
		lw2 = _LwPop();
		_ConcatStrs(lw1, lw2, _LwPop());
		break;
	case kopLenStr:
		_LenStr(lw1);
		break;
	case kopCopySubStr:
		lw2 = _LwPop();
		lw3 = _LwPop();
		_CopySubStr(lw1, lw2, lw3, _LwPop());
		break;

	default:
		_Error(fTrue);
		break;
		}

	return !_fError;
}


/***************************************************************************
	Pop a long off the stack.
***************************************************************************/
long SCEB::_LwPop(void)
{
	long lw, ilw;

	if (_fError)
		return 0;

	//this is faster than just doing FPop
	if ((ilw = _pgllwStack->IvMac()) == 0)
		{
		_Error(fTrue);
		return 0;
		}
	lw = *(long *)_pgllwStack->QvGet(--ilw);
	AssertDo(_pgllwStack->FSetIvMac(ilw), 0);
	return lw;
}


/***************************************************************************
	Get a pointer to the element that is clw elements down from the top.
***************************************************************************/
long *SCEB::_QlwGet(long clw)
{
	long ilwMac;

	if (_fError)
		return pvNil;
	ilwMac = _pgllwStack->IvMac();
	if (!FIn(clw, 1, ilwMac + 1))
		{
		_Error(fTrue);
		return pvNil;
		}
	return (long *)_pgllwStack->QvGet(ilwMac - clw);
}


/***************************************************************************
	Register an error.
***************************************************************************/
void SCEB::_Error(bool fAssert)
{
	AssertThis(0);
	if (!_fError)
		{
		Assert(!fAssert, "Runtime error in script");
		Debug(_WarnSz(PszLit("Runtime error")));
		_fError = fTrue;
		}
}


#ifdef DEBUG
/***************************************************************************
	Emits a warning with the given format string and optional parameters.
***************************************************************************/
void SCEB::_WarnSz(PSZ psz, ...)
{
	AssertThis(0);
	AssertSz(psz);
	STN stn1, stn2;
	SZS szs;

	stn1.FFormatRgch(psz, CchSz(psz), (ulong *)(&psz + 1));
	stn2.FFormatSz(PszLit("Script ('%f', 0x%x, %d): %s"),
			_pscpt->Ctg(), _pscpt->Cno(), _ilwCur, &stn1);
	stn2.GetSzs(szs);
	Warn(szs);
}
#endif //DEBUG


/***************************************************************************
	Rotate clwTot entries on the stack left by clwShift positions.
***************************************************************************/
void SCEB::_Rotate(long clwTot, long clwShift)
{
	AssertThis(0);
	long *qlw;

	if (clwTot == 0 || clwTot == 1)
		return;

	qlw = _QlwGet(clwTot);
	if (qlw != pvNil)
		{
		clwShift %= clwTot;
		if (clwShift < 0)
			clwShift += clwTot;
		AssertIn(clwShift, 0, clwTot);
		if (clwShift != 0)
			{
			SwapBlocks(qlw, LwMul(clwShift, size(long)),
				LwMul(clwTot - clwShift, size(long)));
			}
		}
}


/***************************************************************************
	Reverse clw entries on the stack.
***************************************************************************/
void SCEB::_Reverse(long clw)
{
	AssertThis(0);
	long *qlw, *qlw2;
	long lw;

	if (clw == 0 || clw == 1)
		return;

	qlw = _QlwGet(clw);
	if (qlw != pvNil)
		{
		for (qlw2 = qlw + clw - 1; qlw2 > qlw; )
			{
			lw = *qlw;
			*qlw++ = *qlw2;
			*qlw2-- = lw;
			}
		}
}


/***************************************************************************
	Duplicate clw entries on the stack.
***************************************************************************/
void SCEB::_DupList(long clw)
{
	AssertThis(0);
	long *qlw;

	if (clw == 0)
		return;

	//_QlwGet checks for bad values of clw
	if (_QlwGet(clw) == pvNil)
		return;

	if (!_pgllwStack->FSetIvMac(_pgllwStack->IvMac() + clw))
		_Error(fFalse);
	else
		{
		qlw = _QlwGet(clw * 2);
		Assert(qlw != pvNil, "why did _QlwGet fail?");
		CopyPb(qlw, qlw + clw, LwMul(clw, size(long)));
		}
}


/***************************************************************************
	Removes clw entries from the stack.
***************************************************************************/
void SCEB::_PopList(long clw)
{
	AssertThis(0);
	long ilwMac;

	if (clw == 0 || _fError)
		return;

	ilwMac = _pgllwStack->IvMac();
	if (!FIn(clw, 1, ilwMac + 1))
		_Error(fTrue);
	else
		AssertDo(_pgllwStack->FSetIvMac(ilwMac - clw), "why fail?");
}


/***************************************************************************
	Select the ilw'th entry from the top clw entries.  ilw is indexed from
	the top entry in and is zero based.
***************************************************************************/
void SCEB::_Select(long clw, long ilw)
{
	AssertThis(0);
	long *qlw;

	if (pvNil == (qlw = _QlwGet(clw)))
		return;

	if (!FIn(ilw, 0, clw))
		_Error(fTrue);
	else if (clw > 1)
		{
		qlw[0] = qlw[clw - 1 - ilw];
		_PopList(clw - 1);
		}
}


/***************************************************************************
	The top value is the key, the next is the default return, then come
	clw pairs of test values and return values.  If the key matches a
	test value, push the correspongind return value.  Otherwise, push
	the default return value.
***************************************************************************/
void SCEB::_Match(long clw)
{
	AssertThis(0);
	long *qrglw;
	long lwKey, lwPush, ilwTest;

	lwKey = _LwPop();
	lwPush = _LwPop();
	if (pvNil == (qrglw = _QlwGet(2 * clw)))
		return;

	// start at high memory (top of the stack).
	for (ilwTest = 2 * clw - 1; ilwTest > 0; ilwTest -= 2)
		{
		if (qrglw[ilwTest] == lwKey)
			{
			lwPush = qrglw[ilwTest - 1];
			break;
			}
		}
	qrglw[0] = lwPush;
	_PopList(2 * clw - 1);
}


/***************************************************************************
	Generates a random entry from a list of numbers on the stack.
***************************************************************************/
void SCEB::_RndList(long clw)
{
	AssertThis(0);

	if (clw <= 0)
		_Error(fTrue);
	else
		_Select(clw, vrndUtil.LwNext(clw));
}


/***************************************************************************
	Copy the string from stidSrc to stidDst.
***************************************************************************/
void SCEB::_CopySubStr(long stidSrc, long ichMin, long cch, long stidDst)
{
	AssertThis(0);
	STN stn;

	if (pvNil == _pstrg)
		_Error(fTrue);

	if (_fError)
		return;

	if (!_pstrg->FGet(stidSrc, &stn))
		Debug(_WarnSz(PszLit("Source string doesn't exist (stid = %d)"), stidSrc));

	if (ichMin > 0)
		stn.Delete(0, ichMin);
	if (cch < stn.Cch())
		stn.Delete(cch);
	if (!_pstrg->FPut(stidDst, &stn))
		{
		Debug(_WarnSz(PszLit("Setting dst string failed")));
		_Push(stidNil);
		}
	else
		_Push(stidDst);
}


/***************************************************************************
	Concatenate two strings and put the result in a third. Push the id
	of the destination.
***************************************************************************/
void SCEB::_ConcatStrs(long stidSrc1, long stidSrc2, long stidDst)
{
	AssertThis(0);
	STN stn1, stn2;

	if (pvNil == _pstrg)
		_Error(fTrue);

	if (_fError)
		return;

	if (!_pstrg->FGet(stidSrc1, &stn1))
		{
		Debug(_WarnSz(PszLit("Source string 1 doesn't exist (stid = %d)"),
			stidSrc1));
		}

	if (!_pstrg->FGet(stidSrc2, &stn2))
		{
		Debug(_WarnSz(PszLit("Source string 2 doesn't exist (stid = %d)"),
			stidSrc2));
		}

	stn1.FAppendStn(&stn2);
	if (!_pstrg->FPut(stidDst, &stn1))
		{
		Debug(_WarnSz(PszLit("Setting dst string failed")));
		_Push(stidNil);
		}
	else
		_Push(stidDst);
}


/***************************************************************************
	Push the length of the given string.
***************************************************************************/
void SCEB::_LenStr(long stid)
{
	AssertThis(0);
	STN stn;

	if (pvNil == _pstrg)
		_Error(fTrue);

	if (!_pstrg->FGet(stid, &stn))
		{
		Debug(_WarnSz(PszLit("Source string doesn't exist (stid = %d)"),
			stid));
		}

	_Push(stn.Cch());
}


/***************************************************************************
	CRF reader function to read a string registry string table.
***************************************************************************/
bool _FReadStringReg(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, fblckReadable);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);
	PGST pgst;
	PCABO pcabo;
	short bo;

	*pcb = pblck->Cb(fTrue);
	if (pvNil == ppbaco)
		return fTrue;

	if (!pblck->FUnpackData())
		return fFalse;
	*pcb = pblck->Cb();

	if (pvNil == (pgst = GST::PgstRead(pblck, &bo)) ||
			pgst->CbExtra() != size(long))
		{
		goto LFail;
		}

	if (kboOther == bo)
		{
		long istn, stid;

		for (istn = pgst->IvMac(); istn-- > 0; )
			{
			pgst->GetExtra(istn, &stid);
			SwapBytesRglw(&stid, 1);
			pgst->PutExtra(istn, &stid);
			}
		}

	if (pvNil == (pcabo = NewObj CABO(pgst)))
		{
LFail:
		ReleasePpo(&pgst);
		TrashVar(pcb);
		TrashVar(ppbaco);
		return fFalse;
		}

	*ppbaco = pcabo;
	return fTrue;
}


/***************************************************************************
	Merge a string table into the string registry.
***************************************************************************/
void SCEB::_MergeStrings(CNO cno, RSC rsc)
{
	AssertThis(0);
	PCABO pcabo;
	PGST pgst;
	long istn, stid;
	STN stn;
	bool fFail;

	if (_fError)
		return;

	if (pvNil == _pstrg)
		{
		Bug("no string registry to put the string in");
		_Error(fFalse);
		return;
		}

	if (pvNil == _prca)
		{
		Bug("no rca to read string table from");
		_Error(fFalse);
		return;
		}

	if (pvNil == (pcabo = (PCABO)_prca->PbacoFetch(kctgStringReg, cno,
			&_FReadStringReg, pvNil, rsc)))
		{
		Debug(_WarnSz(PszLit("Reading string table failed (cno = 0x%x)"),
			cno));
		return;
		}

	Assert(pcabo->po->FIs(kclsGST), 0);
	pgst = (PGST)pcabo->po;
	Assert(pgst->CbExtra() == size(long), 0);

	fFail = fFalse;
	for (istn = pgst->IvMac(); istn-- > 0; )
		{
		pgst->GetStn(istn, &stn);
		pgst->GetExtra(istn, &stid);
		fFail |= !_pstrg->FPut(stid, &stn);
		}

#ifdef DEBUG
	if (fFail)
		_WarnSz(PszLit("Merging string table failed"));
#endif //DEBUG

	pcabo->SetCrep(crepTossFirst);
	ReleasePpo(&pcabo);
}


/***************************************************************************
	Convert a number to a string and add the string to the registry.
***************************************************************************/
void SCEB::_NumToStr(long lw, long stid)
{
	AssertThis(0);
	STN stn;

	if (pvNil == _pstrg)
		_Error(fTrue);

	if (_fError)
		return;

	stn.FFormatSz(PszLit("%d"), lw);
	if (!_pstrg->FPut(stid, &stn))
		{
		Debug(_WarnSz(PszLit("Putting string in registry failed")));
		stid = stidNil;
		}
	_Push(stid);
}


/***************************************************************************
	Convert a string to a number and push the result. If the string is
	empty, push lwEmpty; if there is an error, push lwError.
***************************************************************************/
void SCEB::_StrToNum(long stid, long lwEmpty, long lwError)
{
	AssertThis(0);
	STN stn;
	long lw;

	if (pvNil == _pstrg)
		_Error(fTrue);

	if (_fError)
		return;

	_pstrg->FGet(stid, &stn);
	if (0 == stn.Cch())
		lw = lwEmpty;
	else if (!stn.FGetLw(&lw, 0))
		lw = lwError;
	_Push(lw);
}


/***************************************************************************
	Push the value of a variable onto the runtime stack.
***************************************************************************/
void SCEB::_PushVar(PGL pglrtvm, RTVN *prtvn)
{
	AssertThis(0);
	AssertVarMem(prtvn);
	AssertNilOrPo(pglrtvm, 0);
	long lw;

	if (_fError)
		return;

	if (pvNil == pglrtvm || !FFindRtvm(pglrtvm, prtvn, &lw, pvNil))
		{
#ifdef DEBUG
		prtvn->GetStn(&_stn);
		_WarnSz(PszLit("Pushing uninitialized script variable: %s"),
			&_stn);
#endif //DEBUG
		_Push(0);
		}
	else
		_Push(lw);
}


/***************************************************************************
	Pop the top value off the runtime stack into a variable.
***************************************************************************/
void SCEB::_AssignVar(PGL *ppglrtvm, RTVN *prtvn, long lw)
{
	AssertThis(0);
	AssertVarMem(prtvn);
	AssertNilOrVarMem(ppglrtvm);

	if (_fError)
		return;

	if (pvNil == ppglrtvm)
		{
		_Error(fTrue);
		return;
		}

	if (!FAssignRtvm(ppglrtvm, prtvn, lw))
		_Error(fFalse);
}


/***************************************************************************
	Get the variable map for "this" object.
***************************************************************************/
PGL SCEB::_PglrtvmThis(void)
{
	PGL *ppgl = _PpglrtvmThis();
	if (pvNil == ppgl)
		return pvNil;
	return *ppgl;
}


/***************************************************************************
	Get the adress of the variable map master pointer for "this" object
	(so we can create the variable map if need be).
***************************************************************************/
PGL *SCEB::_PpglrtvmThis(void)
{
	return pvNil;
}


/***************************************************************************
	Get the variable map for "global" variables.
***************************************************************************/
PGL SCEB::_PglrtvmGlobal(void)
{
	PGL *ppgl = _PpglrtvmGlobal();
	if (pvNil == ppgl)
		return pvNil;
	return *ppgl;
}


/***************************************************************************
	Get the adress of the variable map master pointer for "global" variables
	(so we can create the variable map if need be).
***************************************************************************/
PGL *SCEB::_PpglrtvmGlobal(void)
{
	return pvNil;
}


/***************************************************************************
	Get the variable map for a remote object.
***************************************************************************/
PGL SCEB::_PglrtvmRemote(long lw)
{
	PGL *ppgl = _PpglrtvmRemote(lw);
	if (pvNil == ppgl)
		return pvNil;
	return *ppgl;
}


/***************************************************************************
	Get the adress of the variable map master pointer for a remote object
	(so we can create the variable map if need be).
***************************************************************************/
PGL *SCEB::_PpglrtvmRemote(long lw)
{
	return pvNil;
}


/***************************************************************************
	Find a RTVM in the pglrtvm.  Assumes the pglrtvm is sorted by rtvn.
	If the RTVN is not in the GL, sets *pirtvm to where it would be if
	it were.
***************************************************************************/
bool FFindRtvm(PGL pglrtvm, RTVN *prtvn, long *plw, long *pirtvm)
{
	AssertPo(pglrtvm, 0);
	AssertVarMem(prtvn);
	AssertNilOrVarMem(plw);
	AssertNilOrVarMem(pirtvm);
	RTVM *qrgrtvm, *qrtvm;
	long irtvm, irtvmMin, irtvmLim;

	qrgrtvm = (RTVM *)pglrtvm->QvGet(0);
	for (irtvmMin = 0, irtvmLim = pglrtvm->IvMac(); irtvmMin < irtvmLim; )
		{
		irtvm = (irtvmMin + irtvmLim) / 2;
		qrtvm = qrgrtvm + irtvm;
		if (qrtvm->rtvn.lu1 < prtvn->lu1)
			irtvmMin = irtvm + 1;
		else if (qrtvm->rtvn.lu1 > prtvn->lu1)
			irtvmLim = irtvm;
		else if (qrtvm->rtvn.lu2 < prtvn->lu2)
			irtvmMin = irtvm + 1;
		else if (qrtvm->rtvn.lu2 > prtvn->lu2)
			irtvmLim = irtvm;
		else
			{
			//we found it
			if (pvNil != plw)
				*plw = qrtvm->lwValue;
			if (pvNil != pirtvm)
				*pirtvm = irtvm;
			return fTrue;
			}
		}
	TrashVar(plw);
	if (pvNil != pirtvm)
		*pirtvm = irtvmMin;
	return fFalse;
}


/***************************************************************************
	Put the given value into a runtime variable.
***************************************************************************/
bool FAssignRtvm(PGL *ppglrtvm, RTVN *prtvn, long lw)
{
	AssertVarMem(ppglrtvm);
	AssertNilOrPo(*ppglrtvm, 0);
	AssertVarMem(prtvn);
	RTVM rtvm;
	long irtvm;

	rtvm.lwValue = lw;
	rtvm.rtvn = *prtvn;
	if (pvNil == *ppglrtvm)
		{
		if (pvNil == (*ppglrtvm = GL::PglNew(size(RTVM))))
			return fFalse;
		(*ppglrtvm)->SetMinGrow(10);
		irtvm = 0;
		}
	else if (FFindRtvm(*ppglrtvm, prtvn, pvNil, &irtvm))
		{
		(*ppglrtvm)->Put(irtvm, &rtvm);
		return fTrue;
		}

	return (*ppglrtvm)->FInsert(irtvm, &rtvm);
}



/***************************************************************************
	A chunky resource reader to read a script.
***************************************************************************/
bool SCPT::FReadScript(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, fblckReadable);
	AssertNilOrVarMem(ppbaco);

	*pcb = pblck->Cb(fTrue);
	if (pvNil == ppbaco)
		return fTrue;

	*ppbaco = PscptRead(pcrf->Pcfl(), ctg, cno);
	return pvNil != *ppbaco;
}


/***************************************************************************
	Static method to read a script.
***************************************************************************/
PSCPT SCPT::PscptRead(PCFL pcfl, CTG ctg, CNO cno)
{
	AssertPo(pcfl, 0);
	short bo;
	KID kid;
	BLCK blck;
	PSCPT pscpt = pvNil;
	PGL pgllw = pvNil;
	PGST pgst = pvNil;

	if (!pcfl->FFind(ctg, cno, &blck))
		goto LFail;

	if (pvNil == (pgllw = GL::PglRead(&blck, &bo)) ||
			pgllw->CbEntry() != size(long))
		{
		goto LFail;
		}
	if (pcfl->FGetKidChidCtg(ctg, cno, 0, kctgScriptStrs, &kid))
		{
		if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck) ||
				pvNil == (pgst = GST::PgstRead(&blck)))
			{
			goto LFail;
			}
		}
	if (pvNil == (pscpt = NewObj SCPT))
		{
LFail:
		ReleasePpo(&pgllw);
		ReleasePpo(&pgst);
		return pvNil;
		}

	if (kboOther == bo)
		SwapBytesRglw(pgllw->QvGet(0), pgllw->IvMac());
	pscpt->_pgllw = pgllw;
	pscpt->_pgstLiterals = pgst;
	return pscpt;
}


/***************************************************************************
	Destructor for a script.
***************************************************************************/
SCPT::~SCPT(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_pgllw);
	ReleasePpo(&_pgstLiterals);
}


/***************************************************************************
	Save the script to the given chunky file.
***************************************************************************/
bool SCPT::FSaveToChunk(PCFL pcfl, CTG ctg, CNO cno, bool fPack)
{
	AssertThis(0);
	AssertPo(pcfl, 0);
	BLCK blck;
	CNO cnoT, cnoStrs;
	long cb;

	// write the script chunk
	cb = _pgllw->CbOnFile();
	if (!blck.FSetTemp(cb) || !_pgllw->FWrite(&blck))
		return fFalse;
	if (fPack)
		blck.FPackData();

	if (!pcfl->FFind(ctg, cno))
		{
		// chunk doesn't exist, just write it
		if (!pcfl->FPutBlck(&blck, ctg, cnoT = cno))
			return fFalse;
		}
	else
		{
		// chunk already exists - add a new temporary one
		if (!pcfl->FAddBlck(&blck, ctg, &cnoT))
			return fFalse;
		}

	// write the string table if there is one.  The cno for this is allocated
	// via FAdd.
	if (pvNil != _pgstLiterals)
		{
		cb = _pgstLiterals->CbOnFile();
		if (!blck.FSetTemp(cb) || !_pgstLiterals->FWrite(&blck))
			goto LFail;
		if (fPack)
			blck.FPackData();

		if (!pcfl->FAddBlck(&blck, kctgScriptStrs, &cnoStrs))
			goto LFail;
		if (!pcfl->FAdoptChild(ctg, cnoT, kctgScriptStrs, cnoStrs))
			{
			pcfl->Delete(kctgScriptStrs, cnoStrs);
LFail:
			pcfl->Delete(ctg, cnoT);
			return fFalse;
			}
		}

	// swap the data and children of the temporary chunk and the destination
	if (cno != cnoT)
		{
		pcfl->SwapData(ctg, cno, ctg, cnoT);
		pcfl->SwapChildren(ctg, cno, ctg, cnoT);
		pcfl->Delete(ctg, cnoT);
		}

	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a SCPT.
***************************************************************************/
void SCPT::AssertValid(ulong grf)
{
	SCPT_PAR::AssertValid(0);
	AssertPo(_pgllw, 0);
	AssertNilOrPo(_pgstLiterals, 0);
}


/***************************************************************************
	Mark memory for the SCPT.
***************************************************************************/
void SCPT::MarkMem(void)
{
	AssertValid(0);
	SCPT_PAR::MarkMem();
	MarkMemObj(_pgllw);
	MarkMemObj(_pgstLiterals);
}
#endif //DEBUG


/***************************************************************************
	Constructor for the runtime string registry.
***************************************************************************/
STRG::STRG(void)
{
	_pgst = pvNil;
	AssertThis(0);
}


/***************************************************************************
	Constructor for the runtime string registry.
***************************************************************************/
STRG::~STRG(void)
{
	AssertThis(0);
	ReleasePpo(&_pgst);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a STRG.
***************************************************************************/
void STRG::AssertValid(ulong grf)
{
	STRG_PAR::AssertValid(0);
	AssertNilOrPo(_pgst, 0);
}


/***************************************************************************
	Mark memory for the STRG.
***************************************************************************/
void STRG::MarkMem(void)
{
	AssertValid(0);
	STRG_PAR::MarkMem();
	MarkMemObj(_pgst);
}
#endif //DEBUG


/***************************************************************************
	Put the string in the registry with the given string id.
***************************************************************************/
bool STRG::FPut(long stid, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	long istn;

	if (!_FEnsureGst())
		return fFalse;

	if (_FFind(stid, &istn))
		return _pgst->FPutStn(istn, pstn);

	return _pgst->FInsertStn(istn, pstn, &stid);
}


/***************************************************************************
	Get the string with the given string id.  If the string isn't in the
	registry, sets pstn to an empty string and returns false.
***************************************************************************/
bool STRG::FGet(long stid, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	long istn;

	if (!_FFind(stid, &istn))
		{
		pstn->SetNil();
		return fFalse;
		}

	_pgst->GetStn(istn, pstn);
	return fTrue;
}


/***************************************************************************
	Add a string to the registry, assigning it an unused id.  The assigned
	id's are not repeated in the near future.  All assigned id's have their
	high bit set.
***************************************************************************/
bool STRG::FAdd(long *pstid, PSTN pstn)
{
	AssertThis(0);
	AssertVarMem(pstid);
	AssertPo(pstn, 0);
	long istn;

	for (;;)
		{
		_stidLast = (_stidLast + 1) | 0x80000000L;
		if (!_FFind(_stidLast, &istn))
			break;
		}

	*pstid = _stidLast;
	return FPut(_stidLast, pstn);
}


/***************************************************************************
	Delete a string from the registry.
***************************************************************************/
void STRG::Delete(long stid)
{
	AssertThis(0);
	long istn;

	if (_FFind(stid, &istn))
		_pgst->Delete(istn);
}


/***************************************************************************
	Change the id of a string from stidSrc to stidDst.  If stidDst already
	exists, it is replaced.  Returns false if the source string doesn't
	exist.  Can't fail if the source does exist.
***************************************************************************/
bool STRG::FMove(long stidSrc, long stidDst)
{
	AssertThis(0);
	long istnSrc, istnDst;

	if (stidSrc == stidDst)
		return _FFind(stidSrc, &istnSrc);

	if (!_FFind(stidDst, &istnSrc))
		return fFalse;

	if (_FFind(stidDst, &istnDst))
		{
		_pgst->Delete(istnDst);
		if (istnSrc > istnDst)
			istnSrc--;
		}

	_pgst->PutExtra(istnSrc, &stidDst);
	_pgst->Move(istnSrc, istnDst);
	return fTrue;
}


/***************************************************************************
	Do a binary search for the string id.  Returns true iff the string is
	in the registry.  In either case, sets *pistn with where the string
	should go.
***************************************************************************/
bool STRG::_FFind(long stid, long *pistn)
{
	AssertThis(0);
	AssertVarMem(pistn);
	long ivMin, ivLim, iv;
	long stidT;

	if (pvNil == _pgst)
		{
		*pistn = 0;
		return fFalse;
		}

	for (ivMin = 0, ivLim = _pgst->IvMac(); ivMin < ivLim; )
		{
		iv = (ivMin + ivLim) / 2;
		_pgst->GetExtra(iv, &stidT);
		if (stidT < stid)
			ivMin = iv + 1;
		else if (stidT > stid)
			ivLim = iv;
		else
			{
			*pistn = iv;
			return fTrue;
			}
		}

	*pistn = ivMin;
	return fFalse;
}


/***************************************************************************
	Make sure the GST exists.
***************************************************************************/
bool STRG::_FEnsureGst(void)
{
	AssertThis(0);

	if (pvNil != _pgst)
		return fTrue;
	_pgst = GST::PgstNew(size(long));
	AssertThis(0);

	return pvNil != _pgst;
}


