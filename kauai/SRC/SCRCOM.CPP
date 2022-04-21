/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Script compiler.  This supports a post-fix (RPN-style) stack base
	"assembly" language and an in-fix (standard) higher level language similar
	to "C" in its operator set and syntax.  The only data type supported
	is 32 bit signed integers.  The SCCB class can also "disassemble"
	a script.  Compilation is case sensitive.  The in-fix compiler supports
	all valid "C" expressions not involving arrays, pointers or structures,
	including the ternary operator (?:).  Supported control structures
	include While and If blocks, with the following syntax:

		While (exp);
			// statements
		End;

		If (exp);
			// statements
		Elif (exp);
			// statements
		Else;
			// statements
		End;

	The semicolons after the keywords are required (they make compilation
	easier); the Elif and Else clauses are (of course) optional.  A "Break;"
	primitive is also supported to exit a While loop early.  The in-fix
	compiler also supports "IfGoto(exp, label$);" and "Goto(label$);".  These
	are compiled directly to single op-codes (plus operands) so are treated
	differently than the control structures.  See the _rgarop array below
	for other function-like primitives supported by the in-fix compiler.

	Label references appear in the in-fix source as a name followed by a $ sign,
	eg, "Goto(label$);".  Label definitions are a name followed by an @ sign.

	In the post-fix source, label references are $label and label definitions
	are @label.

	Local variables as well as sub-class defined "this" variables and remote
	variables are supported.  Variables are not declared, are case sensitive
	and have 8 significant characters.  During execution, if a variable is used
	before being assigned to, its value is 0 and a warning is generated.

	For in-fix compilation, "this" variables are preceeded by a period and
	remote variables are preceeded by "exp->" where exp is a valid numeric
	expression.  The -> operator has very high precedence, so it's common
	to have () around the expression.

	For post-fix compilation, this variables are again prefixed by a period
	and remote variables are preceeded by a ~.  Remote variable accesses use
	the top stack value for their "address".  Use the < operator to push a
	variable and > to pop the top stack value into a variable.  Constants
	in the source are automatically pushed.  Here's a sample post-fix script
	that computes the 10th Fibonacci number and stores it in a remote variable.
	The in-fix equivalent is given in the comments.  Of course, an in-fix
	version of this could use a While loop.

		1 Dup >a >b 10 >n			// a = b = 1; n = 10;
		$LTest Go					// Goto (LTest$);
	@LLoop							// LLoop@
		<a Dup <b Add >a >b			// temp = a; a += b; b = temp;
	@LTest							// LTest@
		$LLoop <n Dec Dup >n GoNz	// IfGoto (--n, LLoop$);
		<a 100 >~answer				// 100->answer = a;


	A compiled script consists of a GL of longs.  Each long is one of 3 types:

		1) an opcode that acts on a variable:
			byte : opcode
			byte : number of immediate longs that follow (at least 1)
			short: 2 bytes of rtvn data, the other 4 bytes are in the next long
		2) an opcode that doesn't involve a variable:
			byte : 0
			byte : number of immediate longs that follow
			short: opcode
		3) immediate data

	After executing an opcode that doesn't jump, the immediate data is placed on
	the execution stack en-masse.  In the case of a variable opcode, the first
	long is used as part of the rtvn, so isn't placed on the stack.

	Label references are considered immediate data and consist of
	kbLabel in the high byte and the destination index (into the gl) in
	the low 3 bytes.

	The first long in the GL is version number information (a dver).

***************************************************************************/
#include "util.h"
ASSERTNAME


RTCLASS(SCCB)

// common error messages
PSZ _pszOom = PszLit("Out of memory");
PSZ _pszSyntax = PszLit("Syntax error");

// name to op lookup table for post-fix compilation
SZOP _rgszop[] =
	{
	{ kopAdd, PszLit("Add") },
	{ kopSub, PszLit("Sub") },
	{ kopMul, PszLit("Mul") },
	{ kopDiv, PszLit("Div") },
	{ kopMod, PszLit("Mod") },
	{ kopAbs, PszLit("Abs") },
	{ kopNeg, PszLit("Neg") },
	{ kopInc, PszLit("Inc") },
	{ kopDec, PszLit("Dec") },
	{ kopRnd, PszLit("Rnd") },
	{ kopMulDiv, PszLit("MulDiv") },
	{ kopBAnd, PszLit("BAnd") },
	{ kopBOr, PszLit("BOr") },
	{ kopBXor, PszLit("BXor") },
	{ kopBNot, PszLit("BNot") },
	{ kopLXor, PszLit("LXor") },
	{ kopLNot, PszLit("LNot") },
	{ kopEq, PszLit("Eq") },
	{ kopNe, PszLit("Ne") },
	{ kopGt, PszLit("Gt") },
	{ kopLt, PszLit("Lt") },
	{ kopGe, PszLit("Ge") },
	{ kopLe, PszLit("Le") },
	{ kopDup, PszLit("Dup") },
	{ kopPop, PszLit("Pop") },
	{ kopSwap, PszLit("Swap") },
	{ kopRot, PszLit("Rot") },
	{ kopRev, PszLit("Rev") },
	{ kopDupList, PszLit("DupList") },
	{ kopPopList, PszLit("PopList") },
	{ kopRndList, PszLit("RndList") },
	{ kopSelect, PszLit("Select") },
	{ kopGoEq, PszLit("GoEq") },
	{ kopGoNe, PszLit("GoNe") },
	{ kopGoGt, PszLit("GoGt") },
	{ kopGoLt, PszLit("GoLt") },
	{ kopGoGe, PszLit("GoGe") },
	{ kopGoLe, PszLit("GoLe") },
	{ kopGoZ, PszLit("GoZ") },
	{ kopGoNz, PszLit("GoNz") },
	{ kopGo, PszLit("Go") },
	{ kopExit, PszLit("Exit") },
	{ kopReturn, PszLit("Return") },
	{ kopSetReturn, PszLit("SetReturn") },
	{ kopShuffle, PszLit("Shuffle") },
	{ kopShuffleList, PszLit("ShuffleList") },
	{ kopNextCard, PszLit("NextCard") },
	{ kopMatch, PszLit("Match") },
	{ kopPause, PszLit("Pause") },
	{ kopCopyStr, PszLit("CopyStr") },
	{ kopMoveStr, PszLit("MoveStr") },
	{ kopNukeStr, PszLit("NukeStr") },
	{ kopMergeStrs, PszLit("MergeStrs") },
	{ kopScaleTime, PszLit("ScaleTime") },
	{ kopNumToStr, PszLit("NumToStr") },
	{ kopStrToNum, PszLit("StrToNum") },
	{ kopConcatStrs, PszLit("ConcatStrs") },
	{ kopLenStr, PszLit("LenStr") },
	{ kopCopySubStr, PszLit("CopySubStr") },
	{ opNil, pvNil },
	};


// name to op look up table for in-fix compilation
AROP _rgarop[] =
	{
	{ kopAbs, PszLit("Abs"), 1, 0, 0, fFalse },
	{ kopRnd, PszLit("Rnd"), 1, 0, 0, fFalse },
	{ kopMulDiv, PszLit("MulDiv"), 3, 0, 0, fFalse },
	{ kopRndList, PszLit("RndList"), 0, 1, 1, fFalse },
	{ kopSelect, PszLit("Select"), 1, 1, 1, fFalse },
	{ kopGoNz, PszLit("IfGoto"), 2, 0, 0, fTrue },
	{ kopGo, PszLit("Goto"), 1, 0, 0, fTrue },
	{ kopExit, PszLit("Exit") , 0, 0, 0, fTrue},
	{ kopReturn, PszLit("Return"), 1, 0, 0, fTrue },
	{ kopSetReturn, PszLit("SetReturn"), 1, 0, 0, fTrue },
	{ kopShuffle, PszLit("Shuffle"), 1, 0, 0, fTrue },
	{ kopShuffleList, PszLit("ShuffleList"), 0, 1, 1, fTrue },
	{ kopNextCard, PszLit("NextCard"), 0, 0, 0, fFalse },
	{ kopMatch, PszLit("Match"), 2, 2, 1, fFalse },
	{ kopPause, PszLit("Pause"), 0, 0, 0, fTrue },
	{ kopCopyStr, PszLit("CopyStr"), 2, 0, 0, fFalse },
	{ kopMoveStr, PszLit("MoveStr"), 2, 0, 0, fFalse },
	{ kopNukeStr, PszLit("NukeStr"), 1, 0, 0, fTrue },
	{ kopMergeStrs, PszLit("MergeStrs"), 2, 0, 0, fTrue },
	{ kopScaleTime, PszLit("ScaleTime"), 1, 0, 0, fTrue },
	{ kopNumToStr, PszLit("NumToStr"), 2, 0, 0, fFalse },
	{ kopStrToNum, PszLit("StrToNum"), 3, 0, 0, fFalse },
	{ kopConcatStrs, PszLit("ConcatStrs"), 3, 0, 0, fFalse },
	{ kopLenStr, PszLit("LenStr"), 1, 0, 0, fFalse },
	{ kopCopySubStr, PszLit("CopySubStr"), 4, 0, 0, fFalse },
	{ opNil, pvNil, 0, 0, 0, fTrue },
	};


PSZ _rgpszKey[] =
	{
	PszLit("If"),
	PszLit("Elif"),
	PszLit("Else"),
	PszLit("End"),
	PszLit("While"),
	PszLit("Break"),
	PszLit("Continue"),
	};

enum
	{
	kipszIf,
	kipszElif,
	kipszElse,
	kipszEnd,
	kipszWhile,
	kipszBreak,
	kipszContinue,
	};


// An opcode long can be followed by at most 255 immediate values
#define kclwLimPush 256


// operator flags - for in-fix compilation
enum
	{
	fopNil = 0,
	fopOp = 1,			// an operator
	fopAssign = 2,		// an assignment operator
	fopPostAssign = 4,	// unary operator acts after using the value
	fopFunction = 8,	// a function
	fopArray = 16,		// an array reference
	};

// expression tree node - for in-fix compilation
struct ETN
	{
	short tt;			// the token type
	short op;			// operator (or function) to generate
	short opl;			// operator precedence level
	short grfop;		// flags
	long lwValue;		// value if a ttLong; an istn if a ttName
	long ietn1;			// indices into _pgletnTree for the operands
	long ietn2;
	long ietn3;
	long cetnDeep;		// depth of the etn tree to here
	};

// control structure types - these are the keywords that are followed
// by a condition (which is why Else is not here).
enum
	{
	cstNil,
	cstIf,
	cstElif,
	cstWhile,
	};

// control structure descriptor
struct CSTD
	{
	long cst;
	long lwLabel1;	// use depends on cst
	long lwLabel2;	// use depends on cst
	long lwLabel3;	// use depends on cst
	PGL pgletnTree;	// for while loops - the expression tree
	long ietnTop;	// the top of the expression tree
	};


/***************************************************************************
	Constructor for a script compiler.
***************************************************************************/
SCCB::SCCB(void)
{
	AssertBaseThis(0);
	_plexb = pvNil;
	_pscpt = pvNil;
	_pgletnTree = pvNil;
	_pgletnStack = pvNil;
	_pglcstd = pvNil;
	_pgstNames = pvNil;
	_pgstLabel = pvNil;
	_pgstReq = pvNil;
	_pmsnk = pvNil;
	AssertThis(0);
}


/***************************************************************************
	Destructor for a script compiler.
***************************************************************************/
SCCB::~SCCB(void)
{
	AssertThis(0);
	_Free();
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a SCCB.
***************************************************************************/
void SCCB::AssertValid(ulong grf)
{
	SCCB_PAR::AssertValid(0);
	AssertNilOrPo(_plexb, 0);
	AssertNilOrPo(_pscpt, 0);
	AssertNilOrPo(_pgletnTree, 0);
	AssertNilOrPo(_pgletnStack, 0);
	AssertNilOrPo(_pglcstd, 0);
	AssertNilOrPo(_pgstNames, 0);
	AssertNilOrPo(_pgstLabel, 0);
	AssertNilOrPo(_pgstReq, 0);
	AssertNilOrPo(_pmsnk, 0);
}


/***************************************************************************
	Mark memory for the SCCB.
***************************************************************************/
void SCCB::MarkMem(void)
{
	AssertValid(0);

	SCCB_PAR::MarkMem();
	MarkMemObj(_plexb);
	MarkMemObj(_pscpt);
	MarkMemObj(_pgletnTree);
	MarkMemObj(_pgletnStack);
	if (pvNil != _pglcstd)
		{
		long icstd;
		CSTD cstd;

		MarkMemObj(_pglcstd);
		for (icstd = _pglcstd->IvMac(); icstd-- > 0; )
			{
			_pglcstd->Get(icstd, &cstd);
			MarkMemObj(cstd.pgletnTree);
			}
		}
	MarkMemObj(_pgstNames);
	MarkMemObj(_pgstLabel);
	MarkMemObj(_pgstReq);
	MarkMemObj(_pmsnk);
}
#endif //DEBUG


/***************************************************************************
	Initializes the script compiler to compile the stream from the given
	lexer object.  pmsnk is a message sink for error reporting.
***************************************************************************/
bool SCCB::_FInit(PLEXB plexb, bool fInFix, PMSNK pmsnk)
{
	AssertThis(0);
	AssertPo(plexb, 0);
	AssertPo(pmsnk, 0);
	long lw;

	_Free();
	_fError = fFalse;
	_fHitEnd = fFalse;
	_plexb = plexb;
	_plexb->AddRef();
	_pmsnk = pmsnk;
	_pmsnk->AddRef();

	if (fInFix)
		{
		// in-fix compilation requires an expression stack, expression parse
		// tree and a control structure stack
		if (pvNil == (_pgletnTree = GL::PglNew(size(ETN))) ||
			pvNil == (_pgletnStack = GL::PglNew(size(ETN))) ||
			pvNil == (_pglcstd = GL::PglNew(size(CSTD))))
			{
			_Free();
			return fFalse;
			}
		_pgletnTree->SetMinGrow(100);
		_pgletnStack->SetMinGrow(100);
		}

	if (pvNil == (_pscpt = NewObj SCPT) ||
			pvNil == (_pscpt->_pgllw = GL::PglNew(size(long))))
		{
		ReleasePpo(&_pscpt);
		}
	else
		_pscpt->_pgllw->SetMinGrow(100);

	// code starts at slot 1 because the version numbers are in slot 0.
	_ilwOpLast = 1;

	// compiled code must start with an operator
	_fForceOp = fTrue;

	// put version info in the script
	lw = LwHighLow(_SwCur(), _SwBack());
	if (pvNil == _pscpt || !_pscpt->_pgllw->FPush(&lw))
        _ReportError(_pszOom);

	_lwLastLabel = 0;	// for internal labels
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Free all memory hanging off this SCCB.
***************************************************************************/
void SCCB::_Free(void)
{
	AssertThis(0);
	CSTD cstd;

	ReleasePpo(&_pscpt);
	ReleasePpo(&_plexb);
	ReleasePpo(&_pmsnk);
	ReleasePpo(&_pgstLabel);
	ReleasePpo(&_pgstReq);
	ReleasePpo(&_pgletnTree);
	ReleasePpo(&_pgletnStack);
	if (pvNil != _pglcstd)
		{
		while (_pglcstd->FPop(&cstd))
			ReleasePpo(&cstd.pgletnTree);
		ReleasePpo(&_pglcstd);
		}
	ReleasePpo(&_pgstNames);
}


/***************************************************************************
	Compile a script given a lexer object.  Compilation is terminated when
	the lexer's stream is exhausted or token ttEnd is encountered.  ttEnd
	allows scripts to be embedded in source for other tools (such as
	chomp.exe).
***************************************************************************/
PSCPT SCCB::PscptCompileLex(PLEXB plexb, bool fInFix, PMSNK pmsnk, long ttEnd)
{
	AssertThis(0);
	AssertPo(plexb, 0);
	AssertPo(pmsnk, 0);
	PSCPT pscpt;

	if (!_FInit(plexb, fInFix, pmsnk))
		return pvNil;
	_ttEnd = ttEnd;

	if (fInFix)
		_CompileIn();
	else
		_CompilePost();

	// link all the label requests with the labels
	if (pvNil != _pgstReq)
		{
		AssertPo(_pgstReq, 0);
		long istn;
		long lw, ilw;
		STN stn;

		for (istn = _pgstReq->IstnMac(); istn-- > 0; )
			{
			_pgstReq->GetStn(istn, &stn);
			if (!_FFindLabel(&stn, &lw))
				{
				//undefined label
				_ReportError(PszLit("Undefined label"));
				}
			else if (pvNil != _pscpt)
				{
				AssertPo(_pscpt, 0);
				_pgstReq->GetExtra(istn, &ilw);
				Assert(B3Lw(lw) == kbLabel, 0);
				AssertIn(ilw, 1, _pscpt->_pgllw->IvMac());
				_pscpt->_pgllw->Put(ilw, &lw);
				}
			}
		}

	pscpt = _pscpt;
	_pscpt = pvNil;
	_Free();
	return pscpt;
}


/***************************************************************************
	Compile the given text file and return the executable script.
	Uses the in-fix or post-fix compiler according to fInFix.
***************************************************************************/
PSCPT SCCB::PscptCompileFil(PFIL pfil, bool fInFix, PMSNK pmsnk)
{
	AssertThis(0);
	AssertPo(pfil, 0);
	AssertPo(pmsnk, 0);
	PSCPT pscpt;
	PLEXB plexb;

	if (pvNil == (plexb = NewObj LEXB(pfil)))
		return pvNil;
	pscpt = PscptCompileLex(plexb, fInFix, pmsnk);
	ReleasePpo(&plexb);
	return pscpt;
}


/***************************************************************************
	Compile a script from the given text file name.
***************************************************************************/
PSCPT SCCB::PscptCompileFni(FNI *pfni, bool fInFix, PMSNK pmsnk)
{
	AssertPo(pfni, ffniFile);
	AssertPo(pmsnk, 0);
	PFIL pfil;
	PSCPT pscpt;

	if (pvNil == (pfil = FIL::PfilOpen(pfni)))
		return pvNil;
	pscpt = PscptCompileFil(pfil, fInFix, pmsnk);
	ReleasePpo(&pfil);
	return pscpt;
}


/***************************************************************************
	Get the next token.  Returns false if the token is a _ttEnd.
***************************************************************************/
bool SCCB::_FGetTok(PTOK ptok)
{
	AssertBaseThis(0);
	AssertPo(_plexb, 0);
	if (_fHitEnd || !_plexb->FGetTok(ptok) || ptok->tt == _ttEnd)
		{
		_fHitEnd = fTrue;
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Return the current version number of the script compiler.  This is
	a virtual method so subclasses of SCCB can provide their own
	version numbers.
***************************************************************************/
short SCCB::_SwCur(void)
{
	AssertBaseThis(0);
	return kswCurSccb;
}


/***************************************************************************
	Return the back version number of the script compiler.  Versions
	back to here can read this script.
***************************************************************************/
short SCCB::_SwBack(void)
{
	AssertBaseThis(0);
	return kswBackSccb;
}


/***************************************************************************
	Return the min version number of the script compiler.  We can read
	scripts back to this version.
***************************************************************************/
short SCCB::_SwMin(void)
{
	AssertBaseThis(0);
	return kswMinSccb;
}


/***************************************************************************
	An error occured.  Report it to the message sink.
***************************************************************************/
void SCCB::_ReportError(PSZ psz)
{
	AssertThis(0);
	AssertPo(_plexb, 0);
	AssertPo(_pmsnk, 0);
	STN stn;
	STN stnFile;

	ReleasePpo(&_pscpt);
	_fError = fTrue;
	_plexb->GetStnFile(&stnFile);
	stn.FFormatSz(PszLit("%s(%d:%d) : error : %z"), &stnFile, _plexb->LwLine(),
		_plexb->IchLine() + 1, psz);
	_pmsnk->ReportLine(stn.Psz());
}


/***************************************************************************
	The given long is immediate data to be pushed onto the execution stack.
***************************************************************************/
void SCCB::_PushLw(long lw)
{
	AssertThis(0);

	if (_fError)
		return;

	AssertPo(_pscpt, 0);
	if (_fForceOp || _pscpt->_pgllw->IvMac() - _ilwOpLast >= kclwLimPush)
		_PushOp(opNil);
	if (!_fError && !_pscpt->_pgllw->FPush(&lw))
		_ReportError(_pszOom);
}


/***************************************************************************
	"Push" a string constant.  Puts the string in the string table and emits
	code to push the corresponding internal variable.
***************************************************************************/
void SCCB::_PushString(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	RTVN rtvn;
	long istn;

	if (_fError)
		return;

	AssertPo(_pscpt, 0);
	if (pvNil == _pscpt->_pgstLiterals &&
			pvNil == (_pscpt->_pgstLiterals = GST::PgstNew()) ||
		!_pscpt->_pgstLiterals->FAddStn(pstn, pvNil, &istn))
		{
		_ReportError(_pszOom);
		return;
		}

	rtvn.lu1 = 0;
	rtvn.lu2 = istn;
	_PushVarOp(kopPushLocVar, &rtvn);
}


/***************************************************************************
	Add an opcode to the compiled script.
***************************************************************************/
void SCCB::_PushOp(long op)
{
	AssertThis(0);
	Assert((long)(short)op == op, "bad opcode");
	long lw;

	if (_fError)
		return;

	AssertPo(_pscpt, 0);
	_EndOp();	// complete the previous opcode
	_ilwOpLast = _pscpt->_pgllw->IvMac();
	lw = LwHighLow(0, (short)op);
	if (!_pscpt->_pgllw->FPush(&lw))
		_ReportError(_pszOom);
	_fForceOp = fFalse;
}


/***************************************************************************
	Close out an opcode by setting the number of immediate longs that follow.
***************************************************************************/
void SCCB::_EndOp(void)
{
	AssertThis(0);
	long ilw;
	long lw;

	if (_fError)
		return;

	AssertPo(_pscpt, 0);
	ilw = _pscpt->_pgllw->IvMac();
	if (_ilwOpLast < ilw - 1)
		{
		//set the count of longs in the previous op
		AssertIn(ilw - _ilwOpLast - 1, 1, kclwLimPush);
		_pscpt->_pgllw->Get(_ilwOpLast, &lw);
		lw = LwHighLow(SuHighLow(B3Lw(lw), (byte)(ilw - _ilwOpLast - 1)), SuLow(lw));
		_pscpt->_pgllw->Put(_ilwOpLast, &lw);
		}
}


/***************************************************************************
	Add an opcode that acts on a variable.
***************************************************************************/
void SCCB::_PushVarOp(long op, RTVN *prtvn)
{
	AssertThis(0);
	Assert((long)(byte)op == op, "bad opcode");
	AssertVarMem(prtvn);
	long lw;

	_PushOp(opNil);	// push a nil op, then fill it in below
	if (_fError)
		return;

	// the high byte is op, the low word is part of the rtvn, the rest of the
	// rtvn is in the next long
	AssertPo(_pscpt, 0);
	Assert(SuHigh(prtvn->lu1) == 0, "bad rtvn");
	lw = prtvn->lu1 | (op << 24);
	_pscpt->_pgllw->Put(_ilwOpLast, &lw);
	if (!_pscpt->_pgllw->FPush(&prtvn->lu2))
		_ReportError(_pszOom);
}


/***************************************************************************
	Look up the indicated label and put it's location in *plwLoc.
***************************************************************************/
bool SCCB::_FFindLabel(PSTN pstn, long *plwLoc)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	AssertVarMem(plwLoc);
	long istn;

	if (pvNil == _pgstLabel || !_pgstLabel->FFindStn(pstn, &istn, fgstSorted))
		{
		TrashVar(plwLoc);
		return fFalse;
		}
	_pgstLabel->GetExtra(istn, plwLoc);
	return fTrue;
}


/***************************************************************************
	Add the given label, giving it the current location.
***************************************************************************/
void SCCB::_AddLabel(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	long lw;
	long istn;

	if (pvNil == _pgstLabel &&
			pvNil == (_pgstLabel = GST::PgstNew(size(long), 5, 100)))
		{
		_ReportError(_pszOom);
		return;
		}
	if (_pgstLabel->FFindStn(pstn, &istn, fgstSorted))
		{
		_ReportError(PszLit("duplicate label"));
		return;
		}
	lw = LwFromBytes(kbLabel, 0, 0, 0);
	if (pvNil != _pscpt)
		{
		AssertPo(_pscpt, 0);
		lw |= _pscpt->_pgllw->IvMac();
		}
	Assert(B3Lw(lw) == kbLabel, 0);

	// a label must address an opcode, not immediate data
	_fForceOp = fTrue;

	if (!_pgstLabel->FInsertStn(istn, pstn, &lw))
		_ReportError(_pszOom);
}


/***************************************************************************
	Add the label request to the request gst and put an immediate value
	of 0 in the compiled script.  When compilation is finished, we'll
	write the actual value for the label.
***************************************************************************/
void SCCB::_PushLabelRequest(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	long lw;

	if (_fError)
		return;

	if (pvNil == _pgstReq &&
			pvNil == (_pgstReq = GST::PgstNew(size(long), 10, 200)))
		{
		_ReportError(_pszOom);
		return;
		}
	_PushLw(0);
	if (_fError)
		return;

	AssertPo(_pscpt, 0);
	lw = _pscpt->_pgllw->IvMac() - 1;
	if (!_pgstReq->FAddStn(pstn, &lw))
		_ReportError(_pszOom);
}


/***************************************************************************
	Add an internal label.  These are numeric to avoid conflicting with
	a user defined label.
***************************************************************************/
void SCCB::_AddLabelLw(long lw)
{
	AssertThis(0);
	STN stn;

	stn.FFormatSz(PszLit("0%x"), lw);
	_AddLabel(&stn);
}


/***************************************************************************
	Push an internal label request.
***************************************************************************/
void SCCB::_PushLabelRequestLw(long lw)
{
	AssertThis(0);
	STN stn;

	stn.FFormatSz(PszLit("0%x"), lw);
	_PushLabelRequest(&stn);
}


/***************************************************************************
	Find the opcode that corresponds to the given stn.
***************************************************************************/
long SCCB::_OpFromStn(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	return _OpFromStnRgszop(pstn, _rgszop);
}


/***************************************************************************
	Check the pstn against the strings in the prgszop and return the
	corresponding op code.
***************************************************************************/
long SCCB::_OpFromStnRgszop(PSTN pstn, SZOP *prgszop)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	AssertVarMem(prgszop);
	SZOP *pszop;

	for (pszop = prgszop; pszop->psz != pvNil; pszop++)
		{
		if (pstn->FEqualSz(pszop->psz))
			return pszop->op;
		}
	return opNil;
}


/***************************************************************************
	Find the string corresponding to the given opcode.  This is used during
	disassembly.
***************************************************************************/
bool SCCB::_FGetStnFromOp(long op, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	return _FGetStnFromOpRgszop(op, pstn, _rgszop);
}


/***************************************************************************
	Check the op against the ops in the prgszop and return the corresponding
	string.
***************************************************************************/
bool SCCB::_FGetStnFromOpRgszop(long op, PSTN pstn, SZOP *prgszop)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	AssertVarMem(prgszop);
	SZOP *pszop;

	for (pszop = prgszop; pszop->psz != pvNil; pszop++)
		{
		if (op == pszop->op)
			{
			*pstn = pszop->psz;
			return fTrue;
			}
		}
	pstn->SetNil();
	return fFalse;
}


/***************************************************************************
	Compile the post-fix script source in _plexb.
***************************************************************************/
void SCCB::_CompilePost(void)
{
	AssertThis(0);
	AssertPo(_plexb, 0);
	TOK tok;
	long op;
	RTVN rtvn;

	while (_FGetTok(&tok))
		{
		switch (tok.tt)
			{
		default:
			_ReportError(_pszSyntax);
			break;

		case ttLong:
			_PushLw(tok.lw);
			break;

		case ttString:
			_PushString(&tok.stn);
			break;

		case ttSub:
			// handle unary minus on constants only
			if (!_FGetTok(&tok) || tok.tt != ttLong)
				_ReportError(_pszSyntax);
			else
				_PushLw(-tok.lw);
			break;

		case ttName:
			op = _OpFromStn(&tok.stn);
			if (opNil == op)
				_ReportError(PszLit("Unknown name"));
			else
				_PushOp(op);
			break;

		case ttGt:
			// pop into a variable
			if (!_FGetTok(&tok))
				{
				_ReportError(_pszSyntax);
				break;
				}
			op = kopPopLocVar;
			switch (tok.tt)
				{
			case ttBNot:
				op = kopPopRemoteVar;
				goto LGetName;
			case ttDot:
				op = kopPopThisVar;
				goto LGetName;
			case ttScope:
				op = kopPopGlobalVar;
				goto LGetName;
				}
			goto LAcceptName;

		case ttLt:
			// push a variable
			if (!_FGetTok(&tok))
				{
				_ReportError(_pszSyntax);
				break;
				}
			op = kopPushLocVar;
			switch (tok.tt)
				{
			case ttBNot:
				op = kopPushRemoteVar;
				goto LGetName;
			case ttDot:
				op = kopPushThisVar;
				goto LGetName;
			case ttScope:
				op = kopPushGlobalVar;
LGetName:
				if (!_FGetTok(&tok))
					{
					_ReportError(_pszSyntax);
					break;
					}
				break;
				}

LAcceptName:
			if (tok.tt == ttBAnd)
				{
				// an array access
				op += kopPushLocArray - kopPushLocVar;
				if (!_FGetTok(&tok))
					{
					_ReportError(_pszSyntax);
					break;
					}
				}
			if (tok.tt == ttName && _OpFromStn(&tok.stn) == opNil)
				{
				rtvn.SetFromStn(&tok.stn);
				_PushVarOp(op, &rtvn);
				}
			else
				_ReportError(PszLit("Variable expected"));
			break;

		case ttDollar:
			// label reference
			if (_FGetTok(&tok) && tok.tt == ttName &&
					_OpFromStn(&tok.stn) == opNil)
				{
				_PushLabelRequest(&tok.stn);
				}
			else
				_ReportError(_pszSyntax);
			break;

		case ttAt:
			// label definition
			if (_FGetTok(&tok) && tok.tt == ttName &&
					_OpFromStn(&tok.stn) == opNil)
				{
				_AddLabel(&tok.stn);
				}
			else
				_ReportError(_pszSyntax);
			break;
			}
		}

	// complete the last opcode
	_EndOp();
}


/***************************************************************************
	In-fix compiler declarations and tables.
***************************************************************************/


// operator precedence levels
enum
	{
	oplNil = 0,
	koplComma,
	koplAssign,
	koplColon,
	koplLOr,
	koplLXor,
	koplLAnd,
	koplBOr,
	koplBXor,
	koplBAnd,
	koplEq,
	koplRel,
	koplShift,
	koplAdd,
	koplMul,
	koplPreUn,
	koplPostUn,
	koplGroup,
	koplNumber,
	koplRemoteName,
	koplThisName,
	koplGlobalName,
	koplArrayName,
	koplName,
	koplMax
	};


// token-operator map entry
struct TOME
	{
	short tt;
	short grfop;
	short op;
	short oplResolve;
	short oplMinRes;
	short ttPop;
	short oplPush;
	};


#define _TomeCore(tt,grfop,op,oplRes,oplMin,ttPop,oplPush) \
	{tt,grfop,op,oplRes,oplMin,ttPop,oplPush}
#define _TomeRes(tt,op,oplRes) {tt,fopOp,op,oplRes,oplRes,ttNil,oplRes}
#define _TomePush(tt,op,oplPush) {tt,fopOp,op,oplNil,oplNil,ttNil,oplPush}

// following a non-operator
TOME _rgtomeExp[] =
	{
	_TomeCore(ttOpenParen, fopOp|fopFunction, opNil, koplName, koplName, ttNil, oplNil), //function call
	_TomeCore(ttCloseParen, fopNil, opNil, koplComma, koplComma, ttOpenParen, koplGroup),
	_TomeCore(ttOpenRef, fopOp|fopArray, opNil, koplName, koplName, ttNil, oplNil), //array reference
	_TomeCore(ttCloseRef, fopNil, opNil, koplComma, koplComma, ttOpenRef, koplArrayName),

	_TomeRes(ttAdd, kopAdd, koplAdd),
	_TomeRes(ttSub, kopSub, koplAdd),
	_TomeRes(ttMul, kopMul, koplMul),
	_TomeRes(ttDiv, kopDiv, koplMul),
	_TomeRes(ttMod, kopMod, koplMul),
	_TomeCore(ttInc, fopAssign|fopPostAssign, kopInc, koplRemoteName, koplRemoteName, ttNil, koplPostUn),
	_TomeCore(ttDec, fopAssign|fopPostAssign, kopDec, koplRemoteName, koplRemoteName, ttNil, koplPostUn),
	_TomeRes(ttBOr, kopBOr, koplBOr),
	_TomeRes(ttBAnd, kopBAnd, koplBAnd),
	_TomeRes(ttBXor, kopBXor, koplBXor),
	_TomeRes(ttShr, kopShr, koplShift),
	_TomeRes(ttShl, kopShl, koplShift),
	_TomeRes(ttLOr, opNil, koplLOr),
	_TomeRes(ttLAnd, opNil, koplLAnd),
	_TomeRes(ttLXor, kopLXor, koplLXor),
	_TomeRes(ttEq, kopEq, koplEq),
	_TomeRes(ttNe, kopNe, koplEq),
	_TomeRes(ttGt, kopGt, koplRel),
	_TomeRes(ttGe, kopGe, koplRel),
	_TomeRes(ttLt, kopLt, koplRel),
	_TomeRes(ttLe, kopLe, koplRel),

	_TomeCore(ttAssign, fopAssign|fopOp, opNil, koplAssign + 1, koplRemoteName, ttNil, koplAssign),
	_TomeCore(ttAAdd, fopAssign|fopOp, kopAdd, koplAssign + 1, koplRemoteName, ttNil, koplAssign),
	_TomeCore(ttASub, fopAssign|fopOp, kopSub, koplAssign + 1, koplRemoteName, ttNil, koplAssign),
	_TomeCore(ttAMul, fopAssign|fopOp, kopMul, koplAssign + 1, koplRemoteName, ttNil, koplAssign),
	_TomeCore(ttADiv, fopAssign|fopOp, kopDiv, koplAssign + 1, koplRemoteName, ttNil, koplAssign),
	_TomeCore(ttAMod, fopAssign|fopOp, kopMod, koplAssign + 1, koplRemoteName, ttNil, koplAssign),
	_TomeCore(ttABOr, fopAssign|fopOp, kopBOr, koplAssign + 1, koplRemoteName, ttNil, koplAssign),
	_TomeCore(ttABAnd, fopAssign|fopOp, kopBAnd, koplAssign + 1, koplRemoteName, ttNil, koplAssign),
	_TomeCore(ttABXor, fopAssign|fopOp, kopBXor, koplAssign + 1, koplRemoteName, ttNil, koplAssign),
	_TomeCore(ttAShr, fopAssign|fopOp, kopShr, koplAssign + 1, koplRemoteName, ttNil, koplAssign),
	_TomeCore(ttAShl, fopAssign|fopOp, kopShl, koplAssign + 1, koplRemoteName, ttNil, koplAssign),

	_TomeCore(ttQuery, fopOp, opNil, koplLOr, koplLOr, ttNil, oplNil),
	_TomeCore(ttColon, fopOp, opNil, koplComma, koplComma, ttQuery, koplColon),
	_TomeRes(ttComma, opNil, koplComma),
	_TomeCore(ttArrow, fopOp, opNil, koplGroup, koplGroup, ttNil, koplRemoteName),
	_TomeCore(ttDollar, fopNil, opNil, koplName, koplName, ttNil, koplNumber),
	_TomeCore(ttAt, fopNil, opNil, koplComma, koplName, ttNil, koplComma),

	//end of list marker
	_TomeRes(ttNil, opNil, oplNil)
	};


// following an operator
TOME _rgtomeOp[] =
	{
	_TomeCore(ttName, fopNil, opNil, oplNil, oplNil, ttNil, koplName),
	_TomeCore(ttLong, fopNil, opNil, oplNil, oplNil, ttNil, koplNumber),
	_TomeCore(ttString, fopNil, opNil, oplNil, oplNil, ttNil, koplNumber),
	_TomeCore(ttOpenParen, fopOp, opNil, oplNil, oplNil, ttNil, oplNil), //grouping
	_TomeCore(ttCloseParen, fopNil, opNil, oplNil, oplNil, ttOpenParen, koplGroup), //empty group

	_TomePush(ttSub, kopNeg, koplPreUn),
	_TomePush(ttLNot, kopLNot, koplPreUn),
	_TomePush(ttBNot, kopBNot, koplPreUn),
	_TomeCore(ttInc, fopOp|fopAssign, kopInc, oplNil, oplNil, ttNil, koplPreUn),
	_TomeCore(ttDec, fopOp|fopAssign, kopDec, oplNil, oplNil, ttNil, koplPreUn),
	_TomePush(ttAdd, opNil, koplPreUn),

	_TomePush(ttDot, opNil, koplThisName),
	_TomePush(ttScope, opNil, koplGlobalName),

	//end of list marker
	_TomeRes(ttNil, opNil, oplNil)
	};


/***************************************************************************
	Find the TOME corresponding to this token and fOp.
***************************************************************************/
TOME *_PtomeFromTt(long tt, bool fOp)
{
	TOME *ptome;

	for (ptome = fOp ? _rgtomeOp : _rgtomeExp; ttNil != ptome->tt; ptome++)
		{
		if (tt == ptome->tt)
			return ptome;
		}
	return pvNil;
}


/***************************************************************************
	Resolve the ETN stack to something at the given opl.
***************************************************************************/
bool SCCB::_FResolveToOpl(long opl, long oplMin, long *pietn)
{
	AssertThis(0);
	AssertIn(opl, oplNil + 1, koplMax);
	AssertIn(oplMin, oplNil, koplMax);
	AssertPo(_pgletnStack, 0);
	AssertPo(_pgletnTree, 0);
	AssertVarMem(pietn);
	ETN etn, etnT;
	long ietn, cetn;

	_pgletnStack->Get(_pgletnStack->IvMac() - 1, &etn);
	if (etn.grfop & fopOp)
		{
		Bug("I think this will only happen if a table entry is wrong");
		_ReportError(_pszSyntax);
		return fFalse;
		}

	for (;;)
		{
		cetn = _pgletnStack->IvMac();
		Assert(cetn > 1, "bad stack");
		Assert(((ETN *)_pgletnStack->QvGet(cetn - 1))->opl >= opl,
			"bad opl on top of stack");
		_pgletnStack->Get(cetn - 2, &etn);
		if (etn.opl < opl)
			break;

		AssertDo(_pgletnStack->FPop(&etnT), 0);
		if (!_FAddToTree(&etnT, &ietn))
			{
			_ReportError(_pszOom);
			return fFalse;
			}
		if (ietn == ivNil)
			return fFalse;

		if (etn.ietn1 == ivNil)
			etn.ietn1 = ietn;
		else if (etn.ietn2 == ivNil)
			etn.ietn2 = ietn;
		else
			{
			Assert(etn.ietn3 == ivNil, "bad etn");
			etn.ietn3 = ietn;
			}
		etn.grfop &= ~fopOp;
		_pgletnStack->Put(cetn - 2, &etn);
		}

	AssertDo(_pgletnStack->FPop(&etnT), 0);
	if (etnT.opl < oplMin)
		{
		_ReportError(_pszSyntax);
		return fFalse;
		}
	if (!_FAddToTree(&etnT, pietn))
		{
		_ReportError(_pszOom);
		return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	See if we can simplify the expression because of constant operands and
	other optimizations, then add it to the tree.  Most of this routine
	is not necessary for non-optimized compilation.  This routine could
	be simplified to nuke () groupings and unary plus operators and just
	add the rest to _pgletnTree.
***************************************************************************/
bool SCCB::_FAddToTree(ETN *petn, long *pietn)
{
	AssertThis(0);
	AssertVarMem(petn);
	AssertVarMem(pietn);
	long lw1, lw2, lw;
	bool fConst1, fConst2;
	ETN etnT1, etnT2, etn;
	long ietnConst;
	bool fCommute = fFalse;

	Assert((petn->ietn3 == ivNil || petn->ietn2 != ivNil)
		&& (petn->ietn2 == ivNil || petn->ietn1 != ivNil),
		"bad etn");

	// get rid of non-function call groupings - they aren't necessary in
	// the parse tree.
	if (petn->op == opNil && petn->tt == ttCloseParen)
		{
		if (petn->grfop & fopFunction)
			goto LAdd;
		Assert(petn->ietn2 == ivNil, "shouldn't be a second operand");
		if (petn->ietn1 == ivNil)
			{
			_ReportError(_pszSyntax);
			*pietn = ivNil;
			return fTrue;
			}
		*pietn = petn->ietn1;
		return fTrue;
		}

	if (petn->ietn3 != ivNil || petn->ietn1 == ivNil)
		goto LAdd;

	// one or two operands
	fConst1 = _FConstEtn(petn->ietn1, &lw1);
	if (petn->ietn2 == ivNil)
		{
		// unary operators

		// nuke unary plus operator
		if (ttAdd == petn->tt && opNil == petn->op)
			{
			*pietn = petn->ietn1;
			return fTrue;
			}

		if (!fConst1)
			goto LAdd;

		// optimize unary operators with constant operand
		switch (petn->op)
			{
		default:
			goto LAdd;

		case kopNeg:
			lw = -lw1;
			break;
		case kopBNot:
			lw = ~lw1;
			break;
		case kopLNot:
			lw = !lw1;
			break;
			}
		*pietn = ietnConst = petn->ietn1;
		goto LSetConst;
		}

	// two operands - determine if they are constants
	fConst2 = _FConstEtn(petn->ietn2, &lw2);
	if (fConst1 && fConst2)
		{
		// optimize binary operators with both operands constants
		if (petn->op == opNil)
			{
			switch (petn->tt)
				{
			default:
				goto LAdd;
			case ttLOr:
				lw = lw1 || lw2;
				break;
			case ttLAnd:
				lw = lw1 && lw2;
				break;
				}
			}
		else if (!_FCombineConstValues(petn->op, lw1, lw2, &lw))
			goto LAdd;

		*pietn = ietnConst = petn->ietn1;
		goto LSetConst;
		}

	// try to combine operand subtrees for the binary operators that are
	// commutative and associative
	Assert(!fConst1 || !fConst2, 0);
	switch (petn->op)
		{
	default:
		break;

	case kopSub:
		if (!fConst2)
			break;

		// the second argument is constant, but the first is not, so negate the
		// constant and change the operator to kopAdd
		_pgletnTree->Get(petn->ietn2, &etnT2);
		etnT2.lwValue = -etnT2.lwValue;
		_pgletnTree->Put(petn->ietn2, &etnT2);
		petn->op = kopAdd;
		// fall thru
	case kopAdd:
	case kopMul:
	case kopBOr:
	case kopBAnd:
	case kopBXor:
	case kopLXor:
		// get the two etn's
		fCommute = fTrue;
		_pgletnTree->Get(petn->ietn1, &etnT1);
		_pgletnTree->Get(petn->ietn2, &etnT2);

		if (fConst1)
			{
			// first operand is constant, so swap them
			Assert(!fConst2, 0);
			SwapVars(&petn->ietn1, &petn->ietn2);
			SwapVars(&fConst1, &fConst2);
			SwapVars(&etnT1, &etnT2);
			}

		if (fConst2)
			{
			// the second is constant
			// see if the first has the same op as petn
			if (etnT1.op != petn->op)
				break;

			// see if the first one has a constant second operand
			if (!_FConstEtn(etnT1.ietn2, &lw1))
				break;

			// all we have to do is modify the first one's second operand
			AssertDo(_FCombineConstValues(petn->op, lw1, etnT2.lwValue, &lw), 0);
			*pietn = petn->ietn1;
			ietnConst = etnT1.ietn2;
			goto LSetConst;
			}

		// neither operand is constant
		Assert(!fConst1 && !fConst2, 0);

		// see if the right operands of the operands are constant
		fConst1 = (etnT1.op == petn->op) && _FConstEtn(etnT1.ietn2, &lw1);
		fConst2 = (etnT2.op == petn->op) && _FConstEtn(etnT2.ietn2, &lw2);

		if (fConst1 && fConst2)
			{
			// both are constant
			AssertDo(_FCombineConstValues(petn->op, lw1, lw2, &lw), 0);
			*pietn = petn->ietn2;
			etnT1.ietn2 = etnT2.ietn1;
			etnT2.ietn1 = petn->ietn1;
			_SetDepth(&etnT1, fTrue);
			_pgletnTree->Put(petn->ietn1, &etnT1);
			ietnConst = etnT2.ietn2;
			_SetDepth(&etnT2);\
			Assert(ietnConst == etnT2.ietn2, "why did _SetDepth move this?");
			_pgletnTree->Put(petn->ietn2, &etnT2);
LSetConst:
			_pgletnTree->Get(ietnConst, &etn);
			Assert(opNil == etn.op && ttLong == etn.tt, 0);
			Assert(etn.cetnDeep == 1, 0);
			etn.lwValue = lw;
			_pgletnTree->Put(ietnConst, &etn);
			return fTrue;
			}

		if (fConst1)
			{
			// only the first is constant
			SwapVars(&etnT1.ietn2, &petn->ietn2);
			_SetDepth(&etnT1, fTrue);
			_pgletnTree->Put(petn->ietn1, &etnT1);
			break;
			}

		if (fConst2)
			{
			// only the second is constant
			goto LPivot;
			}

		// neither is constant
		Assert(!fConst1 && !fConst2, 0);
		if ((etnT1.op == petn->op) == (etnT2.op == petn->op))
			{
			// both the same as petn->op or both different, let _SetDepth
			// do its thing
			break;
			}

		if (etnT1.op == petn->op)
			{
			if (etnT2.ietn1 == ivNil)
				break;
			// swap them
			SwapVars(&petn->ietn1, &petn->ietn2);
			SwapVars(&etnT1, &etnT2);
			}

		Assert(etnT1.op != petn->op && etnT2.op == petn->op, 0);
		//determine if we want to pivot or swap
		_pgletnTree->Get(etnT2.ietn2, &etn);
		if (etn.cetnDeep > etnT1.cetnDeep)
			{
			// swap them
			SwapVars(&petn->ietn1, &petn->ietn2);
			}
		else
			{
LPivot:
			// pivot
			Assert(etnT2.op == petn->op, 0);
			etn = *petn;
			etn.ietn2 = etnT2.ietn2;
			etn.ietn1 = petn->ietn2;
			etnT2.ietn2 = etnT2.ietn1;
			etnT2.ietn1 = petn->ietn1;
			_SetDepth(&etnT2, fTrue);
			_pgletnTree->Put(petn->ietn2, &etnT2);
			petn = &etn;
			}
		break;
		}

LAdd:
	_SetDepth(petn, fCommute);
	return _pgletnTree->FAdd(petn, pietn);
}


/***************************************************************************
	Set the depth of the ETN from its children.  If fCommute is true and
	the first two children are non-nil, puts the deepest child first.
***************************************************************************/
void SCCB::_SetDepth(ETN *petn, bool fCommute)
{
	AssertThis(0);
	AssertVarMem(petn);
	AssertPo(_pgletnTree, 0);
	ETN etn1, etn2;

	if (fCommute && petn->ietn1 != ivNil && petn->ietn2 != ivNil)
		{
		// put the deeper guy on the left
		_pgletnTree->Get(petn->ietn1, &etn1);
		_pgletnTree->Get(petn->ietn2, &etn2);
		if (etn1.cetnDeep < etn2.cetnDeep ||
			etn1.cetnDeep == etn2.cetnDeep && etn1.op == petn->op &&
				etn2.op != petn->op)
			{
			// swap them
			SwapVars(&petn->ietn1, &petn->ietn2);
			SwapVars(&etn1, &etn2);
			}
		// put as much stuff on the left as we can
		while (etn2.op == petn->op)
			{
			etn1 = etn2;
			etn1.ietn1 = petn->ietn1;
			etn1.ietn2 = etn2.ietn1;
			petn->ietn1 = petn->ietn2;
			petn->ietn2 = etn2.ietn2;
			_SetDepth(&etn1, fTrue);
			_pgletnTree->Put(petn->ietn1, &etn1);
			_pgletnTree->Get(petn->ietn2, &etn2);
			}
		}

	petn->cetnDeep = 1;
	if (petn->ietn1 != ivNil)
		{
		_pgletnTree->Get(petn->ietn1, &etn1);
		petn->cetnDeep = LwMax(petn->cetnDeep, etn1.cetnDeep + 1);
		if (petn->ietn2 != ivNil)
			{
			_pgletnTree->Get(petn->ietn2, &etn2);
			petn->cetnDeep = LwMax(petn->cetnDeep, etn2.cetnDeep + 1);
			if (petn->ietn3 != ivNil)
				{
				_pgletnTree->Get(petn->ietn3, &etn1);
				petn->cetnDeep = LwMax(petn->cetnDeep, etn1.cetnDeep + 1);
				}
			}
		}
}


/***************************************************************************
	Return true iff the given etn is constant.  If so, set *plw to its
	value.
***************************************************************************/
bool SCCB::_FConstEtn(long ietn, long *plw)
{
	AssertThis(0);
	AssertVarMem(plw);
	AssertPo(_pgletnTree, 0);
	ETN etn;

	if (ietn != ivNil)
		{
		AssertIn(ietn, 0, _pgletnTree->IvMac());
		_pgletnTree->Get(ietn, &etn);
		if (etn.op == opNil && etn.tt == ttLong)
			{
			*plw = etn.lwValue;
			return fTrue;
			}
		}
	TrashVar(plw);
	return fFalse;
}


/***************************************************************************
	Combine the constant values lw1 and lw2 using the given operator.
	Put the result in *plw.  Return false if we can't combine the values
	using op.
***************************************************************************/
bool SCCB::_FCombineConstValues(long op, long lw1, long lw2, long *plw)
{
	AssertBaseThis(0);
	AssertVarMem(plw);

	switch (op)
		{
	default:
		return fFalse;

	case kopAdd:
		*plw = lw1 + lw2;
		break;
	case kopSub:
		*plw = lw1 - lw2;
		break;
	case kopMul:
		*plw = lw1 * lw2;
		break;
	case kopDiv:
	case kopMod:
		if (lw2 == 0)
			{
			_ReportError(PszLit("divide by zero"));
			*plw = 0;
			}
		else
			*plw = op == kopDiv ? lw1 / lw2 : lw1 % lw2;
		break;
	case kopShr:
		*plw = (ulong)lw1 >> lw2;
		break;
	case kopShl:
		*plw = (ulong)lw1 << lw2;
		break;
	case kopBOr:
		*plw = lw1 | lw2;
		break;
	case kopBAnd:
		*plw = lw1 & lw2;
		break;
	case kopBXor:
		*plw = lw1 ^ lw2;
		break;
	case kopLXor:
		*plw = FPure(lw1) != FPure(lw2);
		break;
	case kopEq:
		*plw = lw1 == lw2;
		break;
	case kopNe:
		*plw = lw1 != lw2;
		break;
	case kopGt:
		*plw = lw1 > lw2;
		break;
	case kopLt:
		*plw = lw1 < lw2;
		break;
	case kopGe:
		*plw = lw1 >= lw2;
		break;
	case kopLe:
		*plw = lw1 <= lw2;
		break;
		}

	return fTrue;
}


/***************************************************************************
	Emit code for the expression tree with top at ietnTop.  If pclwArg
	is not nil, we are pushing function arguments, so the comma operator
	has to keep the values of both its operands.  Otherwise, the comma
	operator discards the value of its left operand.

	Values of grfscc include fsccTop, indicating whether a control structure
	primitive is legal, and fsccWantVoid, indicating whether the emitted
	code should (not) leave a value on the execution stack.  If fsccTop
	is set, fsccWantVoid should also be set.

	This is highly recursive, so limit the stack space needed.
***************************************************************************/
void SCCB::_EmitCode(long ietnTop, ulong grfscc, long *pclwArg)
{
	AssertThis(0);
	AssertPo(_pgletnTree, 0);
	AssertIn(ietnTop, 0, _pgletnTree->IvMac());
	Assert(!(grfscc & fsccTop) || (grfscc & fsccWantVoid),
		"fsccTop but not fsccWantVoid set");
	AssertNilOrVarMem(pclwArg);
	ETN etn;
	RTVN rtvn;
	long opPush, opPop;
	long clwStack;
	long lw1, lw2;
	long cst;

	_pgletnTree->Get(ietnTop, &etn);

	// all non-nil operands should come before any nils
	Assert((etn.ietn3 == ivNil || etn.ietn2 != ivNil)
		&& (etn.ietn2 == ivNil || etn.ietn1 != ivNil),
		"bad etn");

	if (etn.grfop & fopAssign)
		{
		//an operator that changes the value of a variable
		Assert(ivNil != etn.ietn1, "nil ietn1");
		Assert(ivNil == etn.ietn3, "non-nil ietn3");

		//if we're doing an operation before assigning (eg, +=),
		//push the variable first
		if (etn.op != opNil)
			{
			_EmitVarAccess(etn.ietn1, &rtvn, &opPush, &opPop, &clwStack);
			if (clwStack == 1)
				_PushOp(kopDup); //duplicate the variable access id
			else if (clwStack > 0)
				{
				_PushLw(clwStack);
				_PushOp(kopDupList);
				}
			_PushVarOp(opPush, &rtvn);
			}

		//generate the second operand
		if (etn.ietn2 != ivNil)
			_EmitCode(etn.ietn2, fsccNil, pvNil);

		//do the operation (if not post-inc or post-dec)
		if (etn.op != opNil && !(etn.grfop & fopPostAssign))
			_PushOp(etn.op);

		//duplicate the result
		if (!(grfscc & fsccWantVoid))
			_PushOp(kopDup);

		//do the operation if post-inc or post-dec
		if (etn.op != opNil && (etn.grfop & fopPostAssign))
			_PushOp(etn.op);

		//pop the result into the variable
		if (etn.op == opNil)
			_EmitVarAccess(etn.ietn1, &rtvn, pvNil, &opPop, pvNil);
		else if (clwStack == 1)
			{
			//need to bring the variable access result to the top
			if (grfscc & fsccWantVoid)
				_PushOp(kopSwap);
			else
				{
				//top two items on the stack are the result
				_PushLw(3);
				_PushOp(kopRev);
				}
			}
		else if (clwStack > 0)
			{
			// top one or two values are the result, next
			// clwStack values are the variable access values.
			_PushLw(clwStack + 1 + !(grfscc & fsccWantVoid));
			_PushLw(clwStack);
			_PushOp(kopRot);
			}

		_PushVarOp(opPop, &rtvn);
		return;
		}

	if (opNil != etn.op)
		{
		if (ivNil != etn.ietn1)
			{
			_EmitCode(etn.ietn1, fsccNil, pvNil);
			if (ivNil != etn.ietn2)
				{
				_EmitCode(etn.ietn2, fsccNil, pvNil);
				if (ivNil != etn.ietn3)
					_EmitCode(etn.ietn3, fsccNil, pvNil);
				}
			}
		_PushOp(etn.op);
		if (grfscc & fsccWantVoid)
			_PushOp(kopPop);
		return;
		}

	//special cases
	switch (etn.tt)
		{
	default:
		Bug("what is this?");
		break;
	case ttName:
		// first check for a control structure keyword
		if (_FHandleCst(ietnTop))
			{
			// if we're not at the top of the parse tree, it's an error
			if (!(grfscc & fsccTop))
				_ReportError(_pszSyntax);
			break;
			}
		// fall thru
	case ttCloseRef:
	case ttDot:
	case ttScope:
	case ttArrow:
		// handle pushing a variable
		_EmitVarAccess(ietnTop, &rtvn, &opPush, pvNil, pvNil);
		_PushVarOp(opPush, &rtvn);
		if (grfscc & fsccWantVoid)
			_PushOp(kopPop);
		break;
	case ttLong:
		// push a constant
		Assert(ivNil == etn.ietn1, "non-nil ietn1");
		if (!(grfscc & fsccWantVoid))
			_PushLw(etn.lwValue);
		break;
	case ttString:
		// "push" a constant string
		Assert(ivNil == etn.ietn1, "non-nil ietn1");
		if (!(grfscc & fsccWantVoid))
			_PushStringIstn(etn.lwValue);
		break;
	case ttDollar:
		//label reference
		Assert(ivNil != etn.ietn1, "nil ietn1");
		Assert(ivNil == etn.ietn2, "non-nil ietn2");
		if (!(grfscc & fsccWantVoid))
			_PushLabelRequestIetn(etn.ietn1);
		break;
	case ttAt:
		//label declaration
		Assert(ivNil != etn.ietn1, "nil ietn1");
		Assert(ivNil == etn.ietn2, "non-nil ietn2");
		Assert(grfscc & fsccTop, "fsccTop not set for label");
		_AddLabelIetn(etn.ietn1);
		break;
	case ttCloseParen:
		// a function call or control structure
		Assert(etn.grfop & fopFunction, 0);
		Assert(ivNil != etn.ietn1, "nil ietn1");
		Assert(ivNil == etn.ietn3, "non-nil ietn3");

		if ((grfscc & fsccTop) && cstNil != (cst = _CstFromName(etn.ietn1)))
			{
			// control structure
			_BeginCst(cst, etn.ietn2);
			}
		else
			{
			//get the arguments and count them
			long clwArg;

			if (etn.ietn2 == ivNil)
				clwArg = 0;
			else
				{
				clwArg = 1;
				_EmitCode(etn.ietn2, fsccNil, &clwArg);
				}
			_PushOpFromName(etn.ietn1, grfscc, clwArg);
			}
		break;
	case ttComma:
		if (pvNil != pclwArg)
			{
			//we're pushing function arguments
			Assert(!(grfscc & fsccWantVoid), "bad comma request");

			//push function arguments from right to left!
			_EmitCode(etn.ietn2, fsccNil, pclwArg);
			(*pclwArg)++;
			_EmitCode(etn.ietn1, fsccNil, pclwArg);
			}
		else
			{
			_EmitCode(etn.ietn1, fsccWantVoid, pvNil);
			_EmitCode(etn.ietn2, grfscc & ~fsccTop, pvNil);
			}
		break;
	case ttColon:
		//ternary operator
		Assert(ivNil != etn.ietn3, "nil ietn3");
		_PushLabelRequestLw(lw1 = ++_lwLastLabel);
		_EmitCode(etn.ietn1, fsccNil, pvNil);
		_PushOp(kopGoNz);
		_EmitCode(etn.ietn3, grfscc & ~fsccTop, pvNil);
		_PushLabelRequestLw(lw2 = ++_lwLastLabel);
		_PushOp(kopGo);
		_AddLabelLw(lw1);
		_EmitCode(etn.ietn2, grfscc & ~fsccTop, pvNil);
		_AddLabelLw(lw2);
		break;
	case ttLOr:
		//logical or - handles short circuiting
		if (!(grfscc & fsccWantVoid))
			_PushLw(1); //assume true
		_PushLabelRequestLw(lw1 = ++_lwLastLabel);
		_EmitCode(etn.ietn1, fsccNil, pvNil);
		_PushOp(kopGoNz);
		if (!(grfscc & fsccWantVoid))
			_PushLabelRequestLw(lw1);
		_EmitCode(etn.ietn2, grfscc & ~fsccTop, pvNil);
		if (!(grfscc & fsccWantVoid))
			{
			_PushOp(kopGoNz);
			_PushOp(kopDec);
			}
		_AddLabelLw(lw1);
		break;
	case ttLAnd:
		//logical and - handles short circuiting
		if (!(grfscc & fsccWantVoid))
			_PushLw(0); //assume false
		_PushLabelRequestLw(lw1 = ++_lwLastLabel);
		_EmitCode(etn.ietn1, fsccNil, pvNil);
		_PushOp(kopGoZ);
		if (!(grfscc & fsccWantVoid))
			_PushLabelRequestLw(lw1);
		_EmitCode(etn.ietn2, grfscc & ~fsccTop, pvNil);
		if (!(grfscc & fsccWantVoid))
			{
			_PushOp(kopGoZ);
			_PushOp(kopInc);
			}
		_AddLabelLw(lw1);
		break;
		}
}


/***************************************************************************
	If the etn is for a control structure, return the cst.
***************************************************************************/
long SCCB::_CstFromName(long ietn)
{
	AssertThis(0);
	long istn;
	STN stn;

	_GetIstnNameFromIetn(ietn, &istn);
	_GetStnFromIstn(istn, &stn);
	if (stn.FEqualSz(_rgpszKey[kipszIf]))
		return cstIf;
	if (stn.FEqualSz(_rgpszKey[kipszElif]))
		return cstElif;
	if (stn.FEqualSz(_rgpszKey[kipszWhile]))
		return cstWhile;

	return cstNil;
}


/***************************************************************************
	Begin a new control structure.
***************************************************************************/
void SCCB::_BeginCst(long cst, long ietn)
{
	AssertThis(0);
	AssertPo(_pglcstd, 0);
	CSTD cstd;
	CSTD cstdPrev;
	long cetn;

	ClearPb(&cstd, size(cstd));
	cstd.cst = cst;
	cstd.lwLabel1 = ++_lwLastLabel;
	switch (cst)
		{
	default:
		Bug("why are we here?");
		return;

	case cstWhile:
		// we do jump optimized loops (putting the conditional at the end).
		// label 1 is where Continue jumps to
		cstd.ietnTop = ietn;
		if (ietn == ivNil)
			{
			// no conditional expression, an "infinite" loop
			_AddLabelLw(cstd.lwLabel1);
			}
		else
			{
			// copy the etn tree
			cetn = _pgletnTree->IvMac();
			if (pvNil == (cstd.pgletnTree = GL::PglNew(size(ETN), cetn)))
				{
				_ReportError(_pszOom);
				return;
				}
			AssertDo(cstd.pgletnTree->FSetIvMac(cetn), 0);
			CopyPb(_pgletnTree->QvGet(0), cstd.pgletnTree->QvGet(0),
				LwMul(cetn, size(ETN)));

			// jump to where the condition will be
			_PushLabelRequestLw(cstd.lwLabel1);
			_PushOp(kopGo);

			// add the label for the top of the loop
			cstd.lwLabel2 = ++_lwLastLabel;
			_AddLabelLw(cstd.lwLabel2);
			}
		break;

	case cstIf:
		// label 1 is used for where to jump if the condition fails
		goto LTest;

	case cstElif:
		// label 1 is used for where to jump if the condition fails
		// label 2 is used for the end of the entire if block
		cstd.cst = cstIf;
		if (_pglcstd->FPop(&cstdPrev))
			{
			if (cstdPrev.cst != cstIf)
				{
				_pglcstd->FPush(&cstdPrev);
				goto LError;
				}
			}
		else
			{
LError:
			_ReportError(PszLit("unexpected Elif"));
			ClearPb(&cstdPrev, size(cstdPrev));
			cstdPrev.cst = cstIf;
			cstdPrev.lwLabel1 = ++_lwLastLabel;
			}

		// emit code for jumping to the end of the if block
		cstd.lwLabel2 = cstdPrev.lwLabel2;
		if (0 == cstd.lwLabel2)
			cstd.lwLabel2 = ++_lwLastLabel;
		_PushLabelRequestLw(cstd.lwLabel2);
		_PushOp(kopGo);

		// add label that previous condition jumps to on failure
		_AddLabelLw(cstdPrev.lwLabel1);

LTest:
		// emit code for the expression testing
		_PushLabelRequestLw(cstd.lwLabel1);
		if (ietn == ivNil)
			_ReportError(_pszSyntax);
		else
			_EmitCode(ietn, fsccNil, pvNil);
		_PushOp(kopGoZ);
		break;
		}

	// put the cstd on the stack
	if (!_pglcstd->FPush(&cstd))
		_ReportError(_pszOom);
}


/***************************************************************************
	If this name token is "End", "Break", "Continue" or "Else", deal with it
	and return true.
***************************************************************************/
bool SCCB::_FHandleCst(long ietn)
{
	AssertThis(0);
	AssertPo(_pglcstd, 0);
	long istn;
	STN stn;
	CSTD cstd;
	long icstd;
	long *plwLabel;

	_GetIstnNameFromIetn(ietn, &istn);
	_GetStnFromIstn(istn, &stn);
	if (stn.FEqualSz(_rgpszKey[kipszElse]))
		{
		// an Else
		if (0 == (icstd = _pglcstd->IvMac()))
			{
			_ReportError(PszLit("unexpected Else"));
			return fTrue;
			}
		_pglcstd->Get(--icstd, &cstd);
		if (cstd.cst != cstIf)
			{
			_ReportError(PszLit("unexpected Else"));
			return fTrue;
			}

		// emit code to jump to the end of the if block
		if (0 == cstd.lwLabel2)
			cstd.lwLabel2 = ++_lwLastLabel;
		_PushLabelRequestLw(cstd.lwLabel2);
		_PushOp(kopGo);

		// add label that previous condition jumps to on failure
		_AddLabelLw(cstd.lwLabel1);

		cstd.cst = cstNil;	// so we don't accept another else
		cstd.lwLabel1 = cstd.lwLabel2;
		_pglcstd->Put(icstd, &cstd);
		return fTrue;
		}

	if (stn.FEqualSz(_rgpszKey[kipszBreak]))
		{
		// a Break - label 3 is where a Break jumps to
		plwLabel = &cstd.lwLabel3;
		goto LWhileJump;
		}

	if (stn.FEqualSz(_rgpszKey[kipszContinue]))
		{
		// a Continue - label 1 is where a Continue jumps to
		plwLabel = &cstd.lwLabel1;

LWhileJump:
		// find the enclosing While
		for (icstd = _pglcstd->IvMac(); ; )
			{
            if (icstd-- <= 0)
				{
				_ReportError(plwLabel == &cstd.lwLabel1 ?
					PszLit("unexpected Continue") :
					PszLit("unexpected Break"));
				return fTrue;
				}
			_pglcstd->Get(icstd, &cstd);
			if (cstd.cst == cstWhile)
				break;
			}

		if (*plwLabel == 0)
			{
			Assert(plwLabel == &cstd.lwLabel3, 0);
			*plwLabel = ++_lwLastLabel;
			_pglcstd->Put(icstd, &cstd);
			}
		_PushLabelRequestLw(*plwLabel);
		_PushOp(kopGo);
		return fTrue;
		}

	if (stn.FEqualSz(_rgpszKey[kipszEnd]))
		{
		// an End
		if (!_pglcstd->FPop(&cstd))
			{
			_ReportError(PszLit("unexpected End"));
			return fTrue;
			}

		switch (cstd.cst)
			{
		case cstWhile:
			if (cstd.ietnTop == ivNil)
				{
				// an "infinite" loop - jump to the top
				Assert(cstd.pgletnTree == pvNil, 0);
				Assert(cstd.lwLabel2 == 0, 0);
				_PushLabelRequestLw(cstd.lwLabel1);
				_PushOp(kopGo);
				}
			else
				{
				AssertPo(cstd.pgletnTree, 0);
				_AddLabelLw(cstd.lwLabel1);

				// emit code for the expression testing
				_PushLabelRequestLw(cstd.lwLabel2);
				SwapVars(&_pgletnTree, &cstd.pgletnTree);
				_EmitCode(cstd.ietnTop, fsccNil, pvNil);
				SwapVars(&_pgletnTree, &cstd.pgletnTree);
				ReleasePpo(&cstd.pgletnTree);
				_PushOp(kopGoNz);
				}

			// add the Break label if it was used
			if (cstd.lwLabel3 != 0)
				_AddLabelLw(cstd.lwLabel3);
			break;

		case cstIf:
			if (cstd.lwLabel2 != 0)
				_AddLabelLw(cstd.lwLabel2);
			_AddLabelLw(cstd.lwLabel1);
			break;

		default:
			Assert(cstd.cst == cstNil, "bad cstd");
			_AddLabelLw(cstd.lwLabel1);
			break;
			}

		return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	Generate the code that handles remote variable access and fill
	*popPush and *popPop with the appropriate op-codes (kopPushLocVar, etc).
	Sets *pclwStack to the number of longs on the stack used to access the
	variable (iff the variable is a remote variable or an array access).
	Fills in *prtvn.
***************************************************************************/
void SCCB::_EmitVarAccess(long ietn, RTVN *prtvn,
	long *popPush, long *popPop, long *pclwStack)
{
	AssertThis(0);
	AssertPo(_pgletnTree, 0);
	AssertIn(ietn, 0, _pgletnTree->IvMac());
	AssertVarMem(prtvn);
	AssertNilOrVarMem(popPush);
	AssertNilOrVarMem(popPop);
	AssertNilOrVarMem(pclwStack);
	ETN etn;
	long opPop, opPush;
	long clwStack = 0;

	_pgletnTree->Get(ietn, &etn);
	switch (etn.tt)
		{
	default:
		opPop = kopPopLocVar;
		opPush = kopPushLocVar;
		break;

	case ttDot:
		Assert(etn.opl == koplThisName, "bad this name etn");
		Assert(etn.ietn1 != ivNil && etn.ietn2 == ivNil, "bad this name etn 2");
		ietn = etn.ietn1;
		opPop = kopPopThisVar;
		opPush = kopPushThisVar;
		break;

	case ttScope:
		Assert(etn.opl == koplGlobalName, "bad global name etn");
		Assert(etn.ietn1 != ivNil && etn.ietn2 == ivNil, "bad global name etn 2");
		ietn = etn.ietn1;
		opPop = kopPopGlobalVar;
		opPush = kopPushGlobalVar;
		break;

	case ttArrow:
		Assert(etn.opl == koplRemoteName, "bad remote name etn");
		Assert(etn.ietn1 != ivNil && etn.ietn2 != ivNil && etn.ietn3 == ivNil,
			"bad remote name etn 2");
		_EmitCode(etn.ietn1, fsccNil, pvNil);
		clwStack++;
		ietn = etn.ietn2;
		opPop = kopPopRemoteVar;
		opPush = kopPushRemoteVar;
		break;
		}

	_pgletnTree->Get(ietn, &etn);
	if (etn.tt == ttCloseRef)
		{
		// an array reference
		Assert(etn.opl == koplArrayName, "bad array name etn");
		Assert(etn.ietn1 != ivNil && etn.ietn2 != ivNil && etn.ietn3 == ivNil,
			"bad array name etn 2");
		_EmitCode(etn.ietn2, fsccNil, pvNil);
		clwStack++;
		ietn = etn.ietn1;
		opPop += kopPopLocArray - kopPopLocVar;
		opPush += kopPushLocArray - kopPushLocVar;
		}

	_GetIstnNameFromIetn(ietn, &etn.lwValue);
	_GetRtvnFromName(etn.lwValue, prtvn);

	if (pvNil != pclwStack)
		*pclwStack = clwStack;
	if (pvNil != popPush)
		*popPush = opPush;
	if (pvNil != popPop)
		*popPop = opPop;
}


/***************************************************************************
	Push the opcode for a function and verify parameters and return type.
***************************************************************************/
void SCCB::_PushOpFromName(long ietn, ulong grfscc, long clwArg)
{
	AssertThis(0);
	long istn;
	STN stn;
	long op, clwFixed, clwVar, cactMinVar;
	bool fVoid;

	_GetIstnNameFromIetn(ietn, &istn);
	_GetStnFromIstn(istn, &stn);
	if (!_FGetOpFromName(&stn, &op, &clwFixed, &clwVar, &cactMinVar, &fVoid))
		_ReportError(PszLit("unknown function"));
	else if (clwArg < clwFixed || clwVar == 0 && clwArg > clwFixed ||
		clwVar != 0 && (((clwArg - clwFixed) % clwVar) != 0 ||
			(clwArg - clwFixed) / clwVar < cactMinVar))
		{
		_ReportError(PszLit("Wrong number of parameters"));
		}
	else if (!(grfscc & fsccWantVoid) && fVoid)
		_ReportError(PszLit("Using void return value"));
	else
		{
		if (clwVar > 0)
			_PushLw((clwArg - clwFixed) / clwVar);
		_PushOp(op);
		if ((grfscc & fsccWantVoid) && !fVoid)
			_PushOp(kopPop); //toss the return value
		}
}


/***************************************************************************
	Find the string in the given rgarop and get the associated parameter
	and return type information.
***************************************************************************/
bool SCCB::_FGetArop(PSTN pstn, AROP *prgarop, long *pop, long *pclwFixed,
	long *pclwVar, long *pcactMinVar, bool *pfVoid)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	AssertVarMem(prgarop);
	AssertVarMem(pop);
	AssertVarMem(pclwFixed);
	AssertVarMem(pclwVar);
	AssertVarMem(pcactMinVar);
	AssertVarMem(pfVoid);
	AROP *parop;

	for (parop = prgarop; parop->psz != pvNil; parop++)
		{
		if (pstn->FEqualSz(parop->psz))
			{
			*pop = parop->op;
			*pclwFixed = parop->clwFixed;
			*pclwVar = parop->clwVar;
			*pcactMinVar = parop->cactMinVar;
			*pfVoid = parop->fVoid;
			return fTrue;
			}
		}
	return fFalse;
}


/***************************************************************************
	See if the given name is a function and give argument and return type
	information.
***************************************************************************/
bool SCCB::_FGetOpFromName(PSTN pstn, long *pop, long *pclwFixed,
	long *pclwVar, long *pcactMinVar, bool *pfVoid)
{
	AssertThis(0);
	return _FGetArop(pstn, _rgarop, pop, pclwFixed, pclwVar, pcactMinVar, pfVoid);
}


/***************************************************************************
	Add the given string to _pgstNames.
***************************************************************************/
void SCCB::_AddNameRef(PSTN pstn, long *pistn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	AssertVarMem(pistn);

	if (pvNil == _pgstNames && pvNil == (_pgstNames = GST::PgstNew(0, 5, 100)))
		{
		*pistn = 0;
		_ReportError(_pszOom);
		return;
		}
	//can't sort, because then indices can change
	if (_pgstNames->FFindStn(pstn, pistn, fgstNil))
		return;
	if (!_pgstNames->FInsertStn(*pistn, pstn))
		_ReportError(_pszOom);
}


/***************************************************************************
	Make sure (assert) the given ietn is a name and get the istn for the name.
***************************************************************************/
void SCCB::_GetIstnNameFromIetn(long ietn, long *pistn)
{
	AssertThis(0);
	AssertPo(_pgletnTree, 0);
	AssertIn(ietn, 0, _pgletnTree->IvMac());
	AssertVarMem(pistn);
	ETN etn;

	_pgletnTree->Get(ietn, &etn);
	Assert(etn.tt == ttName && etn.opl == koplName && etn.op == opNil &&
		etn.ietn1 == ivNil && etn.ietn2 == ivNil && etn.ietn3 == ivNil,
		"bad name etn");
	*pistn = etn.lwValue;
}


/***************************************************************************
	Get the rtvn for the given string (in _pgstNames).
***************************************************************************/
void SCCB::_GetStnFromIstn(long istn, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	if (pvNil == _pgstNames || !FIn(istn, 0, _pgstNames->IstnMac()))
		{
		Assert(_fError, "bad istn");
		pstn->SetNil();
		return;
		}
	_pgstNames->GetStn(istn, pstn);
}


/***************************************************************************
	Get the rtvn for the given string (in _pgstNames).
***************************************************************************/
void SCCB::_GetRtvnFromName(long istn, RTVN *prtvn)
{
	AssertThis(0);
	AssertVarMem(prtvn);
	STN stn;

	_GetStnFromIstn(istn, &stn);
	if (_FKeyWord(&stn))
		_ReportError(PszLit("Using keyword as variable"));
	prtvn->SetFromStn(&stn);
}


/***************************************************************************
	Determine if the given string is a keyword.
***************************************************************************/
bool SCCB::_FKeyWord(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	long op, clwFixed, clwVar, cactMinVar;
	bool fVoid;
	long ipsz;

	if (_FGetOpFromName(pstn, &op, &clwFixed, &clwVar, &cactMinVar, &fVoid))
		return fTrue;
	for (ipsz = CvFromRgv(_rgpszKey); ipsz-- > 0; )
		{
		if (pstn->FEqualSz(_rgpszKey[ipsz]))
			return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	Push a label request using the name in the given ietn.
***************************************************************************/
void SCCB::_PushLabelRequestIetn(long ietn)
{
	AssertThis(0);
	long istn;
	STN stn;

	_GetIstnNameFromIetn(ietn, &istn);
	_GetStnFromIstn(istn, &stn);
	_PushLabelRequest(&stn);
}


/***************************************************************************
	Add the label here.
***************************************************************************/
void SCCB::_AddLabelIetn(long ietn)
{
	AssertThis(0);
	long istn;
	STN stn;

	_GetIstnNameFromIetn(ietn, &istn);
	_GetStnFromIstn(istn, &stn);
	_AddLabel(&stn);
}


/***************************************************************************
	"Push" a string constant that is currently in the name string table.
***************************************************************************/
void SCCB::_PushStringIstn(long istn)
{
	AssertThis(0);
	STN stn;

	_GetStnFromIstn(istn, &stn);
	_PushString(&stn);
}


/***************************************************************************
	Compile the in-fix script source.
***************************************************************************/
void SCCB::_CompileIn(void)
{
	AssertThis(0);
	AssertPo(_plexb, 0);
	AssertPo(_pgletnStack, 0);
	AssertPo(_pgletnTree, 0);
	TOK tok;
	TOME *ptome;
	ETN etn, etnT;
	long ietn;
	bool fDone;

	// push the new record
	ClearPb(&etn, size(etn));
	etn.tt = ttNil;
	etn.grfop = fopOp;
	if (!_pgletnStack->FPush(&etn))
		{
		_ReportError(_pszOom);
		return;
		}

	for (;;)
		{
		fDone = !_FGetTok(&tok);
		if (fDone || tok.tt == ttSemi)
			{
			// end of a statement - emit the code
			if (_pgletnStack->IvMac() > 1)
				{
				//non-empty statement
				if (!_FResolveToOpl(koplComma, koplComma, &ietn))
					{
					Assert(_fError, "why wasn't an error reported?");
					_pgletnStack->FSetIvMac(1);
					}
				else if (_pgletnStack->IvMac() != 1)
					{
					_ReportError(_pszSyntax);
					_pgletnStack->FSetIvMac(1);
					}
				else
					_EmitCode(ietn, fsccTop | fsccWantVoid, pvNil);
				}
			_pgletnTree->FSetIvMac(0);
			if (fDone)
				{
				_EndOp();
				if (_pglcstd->IvMac() != 0)
					_ReportError(PszLit("unexpected end of source"));
				break;
				}
			continue;
			}

		_pgletnStack->Get(_pgletnStack->IvMac() - 1, &etn);
		ptome = _PtomeFromTt(tok.tt, etn.grfop & fopOp);
		if (pvNil == ptome)
			{
			_ReportError(_pszSyntax);
			goto LNextLine;
			}

		ClearPb(&etn, size(etn));
		etn.tt = (short)tok.tt;
		etn.lwValue = tok.lw;
		etn.op = ptome->op;
		etn.opl = ptome->oplPush;
		etn.grfop = ptome->grfop;
		etn.ietn1 = etn.ietn2 = etn.ietn3 = ivNil;
		if (tok.tt == ttName || tok.tt == ttString)
			_AddNameRef(&tok.stn, &etn.lwValue);

		// resolve
		if (oplNil != ptome->oplResolve)
			{
			if (!_FResolveToOpl(ptome->oplResolve, ptome->oplMinRes, &etn.ietn1))
				{
				Assert(_fError, "error not set");
				goto LNextLine;
				}
			}

		// pop an operator
		if (ttNil != ptome->ttPop)
			{
			if (!_pgletnStack->FPop(&etnT) || etnT.tt != ptome->ttPop ||
					!(etnT.grfop & fopOp))
				{
				_ReportError(_pszSyntax);
				goto LNextLine;
				}
			Assert(etnT.ietn2 == ivNil, "bad etn");
			if (etnT.ietn1 != ivNil)
				{
				etn.ietn2 = etn.ietn1;
				etn.ietn1 = etnT.ietn1;
				if (etnT.grfop & fopFunction)
					etn.grfop |= fopFunction;
				}
			}

		// push the new record
		if (!_pgletnStack->FPush(&etn))
			{
			_ReportError(_pszOom);
			goto LNextLine;
			}

		if (tok.tt == ttAt)
			{
			// label declaration - emit the code
			if (!_FResolveToOpl(koplComma, koplComma, &ietn))
				{
				Assert(_fError, "error not set");
				goto LNextLine;
				}
			if (_pgletnStack->IvMac() != 1)
				{
				_ReportError(_pszSyntax);
LNextLine:
				//start parsing after the next semi-colon
				while (_FGetTok(&tok) && tok.tt != ttSemi)
					;
				_pgletnStack->FSetIvMac(1);
				}
			else
				_EmitCode(ietn, fsccTop | fsccWantVoid, pvNil);
			_pgletnTree->FSetIvMac(0);
			}
		}
}


/***************************************************************************
	Disassemble the script into a message sink (MSNK) and return whether
	there was an error.
***************************************************************************/
bool SCCB::FDisassemble(PSCPT pscpt, PMSNK pmsnk, PMSNK pmsnkError)
{
	AssertThis(0);
	AssertPo(pscpt, 0);
	AssertPo(pmsnk, 0);
	AssertPo(pmsnkError, 0);
	RTVN rtvn;
	long ilwMac, ilw, clwPush;
	long lw;
	long op;
	STN stn;
	DVER dver;
	PGL pgllw = pscpt->_pgllw;
	PSZ pszError = pvNil;
	AssertPo(pgllw, 0);
	Assert(pgllw->CbEntry() == size(long), "bad script");

	ilwMac = pgllw->IvMac();
	if (ilwMac < 1)
		{
		pszError = PszLit("No version numbers on script");
		goto LFail;
		}

	//check the version
	pgllw->Get(0, &lw);
	dver.Set(SwHigh(lw), SwLow(lw));
	if (!dver.FReadable(_SwCur(), _SwMin()))
		{
		pszError = PszLit("Script version doesn't match script compiler version");
		goto LFail;
		}

	for (ilw = 1; ; )
		{
		//write the label
		stn.FFormatSz(PszLit("@L_%04x "), ilw);
		pmsnk->Report(stn.Psz());
		if (ilw >= ilwMac)
			break;

		pgllw->Get(ilw++, &lw);
		clwPush = B2Lw(lw);
		if (!FIn(clwPush, 0, ilwMac - ilw + 1))
			{
			pszError = PszLit("bad instruction");
			goto LFail;
			}

		//write the op string
		if (opNil != (op = B3Lw(lw)))
			{
			//this instruction acts on a variable
			if (clwPush == 0)
				{
				pszError = PszLit("bad var instruction");
				goto LFail;
				}
			clwPush--;
			rtvn.lu1 = (ulong)SuLow(lw);
			pgllw->Get(ilw++, &rtvn.lu2);
			if (rtvn.lu1 == 0)
				{
				if (op != kopPushLocVar)
					{
					pszError = PszLit("bad variable");
					goto LFail;
					}

				// string literal
				if (pvNil == pscpt->_pgstLiterals ||
						!FIn(rtvn.lu2, 0, pscpt->_pgstLiterals->IvMac()))
					{
					pszError = PszLit("bad internal variable");
					goto LFail;
					}
				pscpt->_pgstLiterals->GetStn(rtvn.lu2, &stn);
				stn.FExpandControls();
				pmsnk->Report(PszLit("\""));
				pmsnk->Report(stn.Psz());
				pmsnk->Report(PszLit("\" "));
				}
			else
				{
				bool fArray = fFalse;

				if (FIn(op, kopMinArray, kopLimArray))
					{
					op += kopPushLocVar - kopPushLocArray;
					fArray = fTrue;
					}

				stn.SetNil();
				switch (op)
					{
				case kopPushLocVar:
					stn.FAppendCh(ChLit('<'));
					break;
				case kopPopLocVar:
					stn.FAppendCh(ChLit('>'));
					break;
				case kopPushThisVar:
					stn.FAppendSz(PszLit("<."));
					break;
				case kopPopThisVar:
					stn.FAppendSz(PszLit(">."));
					break;
				case kopPushGlobalVar:
					stn.FAppendSz(PszLit("<::"));
					break;
				case kopPopGlobalVar:
					stn.FAppendSz(PszLit(">::"));
					break;
				case kopPushRemoteVar:
					stn.FAppendSz(PszLit("<~"));
					break;
				case kopPopRemoteVar:
					stn.FAppendSz(PszLit(">~"));
					break;
				default:
					pszError = PszLit("bad var op");
					goto LFail;
					}
				if (fArray)
					stn.FAppendCh(ChLit('&'));
				pmsnk->Report(stn.Psz());

				rtvn.GetStn(&stn);
				stn.FAppendCh(kchSpace);
				pmsnk->Report(stn.Psz());
				}
			}
		else if (opNil != (op = SuLow(lw)))
			{
			//normal opcode
			if (!_FGetStnFromOp(op, &stn))
				{
				pszError = PszLit("bad op in script");
LFail:
				if (pmsnkError != pvNil && pszError != pvNil)
					pmsnkError->ReportLine(pszError);
				return fFalse;
				}
			stn.FAppendCh(kchSpace);
			pmsnk->Report(stn.Psz());
			}

		//dump the stack stuff
		while (clwPush-- > 0)
			{
			pgllw->Get(ilw++, &lw);
			if (B3Lw(lw) == kbLabel && FIn(lw &= 0x00FFFFFF, 1, ilwMac + 1))
				{
				//REVIEW shonk: label identification: this isn't foolproof
				stn.FFormatSz(PszLit("$L_%04x "), lw);
				pmsnk->Report(stn.Psz());
				}
			else
				{
				stn.FFormatSz(PszLit("%d /*0x%x*/ "), lw, lw);
				pmsnk->Report(stn.Psz());
				}
			}
		pmsnk->ReportLine(PszLit(""));
		}
	pmsnk->ReportLine(PszLit(""));

	return fTrue;
}


/***************************************************************************
	Set the values of the RTVN from the given stn.  Only the first 8
	characters of the stn are significant.  The high word of lu1 is
	guaranteed to be zero.
***************************************************************************/
void RTVN::SetFromStn(PSTN pstn)
{
	AssertThisMem();
	AssertPo(pstn, 0);
	byte rgb[8];
	byte bT;
	long lw, ib, ibDst, cbit;
	long cch = pstn->Cch();
	PSZ psz = pstn->Psz();

	// There are 52 letters, plus 10 digits, plus the underscore character,
	// giving a total of 63 valid identifier characters.  We encode these
	// from 1 to 63 and pack them into 6 bits each.  This lets us put 8
	// characters in 6 bytes.  We pad the result with 0's.

	ClearPb(rgb, 8);
	ibDst = 0;
	cbit = 8;
	for (ib = 0; ib < 8; ib++)
		{
		// map the character to its encoded value
		if (ib >= cch)
			bT = 0;
		else
			{
			bT = (byte)(schar)psz[ib];
			if (FIn(bT, '0', '9' + 1))
				bT -= '0' - 1;
			else if (FIn(bT, 'A', 'Z' + 1))
				bT -= 'A' - 11;
			else if (FIn(bT, 'a', 'z' + 1))
				bT -= 'a' - 37;
			else
				{
				Assert(bT == '_', "bad identifier");
				bT = 63;
				}
			}

		// pack the encoded value into its 6 bit slot
		AssertIn(bT, 0, 64);
		lw = (long)bT << (cbit + 2);
		rgb[ibDst] = rgb[ibDst] & (-1L << cbit) | B1Lw(lw);
		if ((cbit += 2) <= 8)
			rgb[++ibDst] = B0Lw(lw);
		else
			cbit -= 8;
		}

	// put the bytes together into the rtvn's ulongs.
	lu1 = LwFromBytes(0, 0, rgb[0], rgb[1]);
	lu2 = LwFromBytes(rgb[2], rgb[3], rgb[4], rgb[5]);
}


/***************************************************************************
	Get the variable name that an rtvn stores.
***************************************************************************/
void RTVN::GetStn(PSTN pstn)
{
	AssertThisMem();
	AssertPo(pstn, 0);
	byte rgb[8];
	byte bT;
	long ib;

	//unpack the individual bytes
	rgb[0] = (byte)((lu1 & 0x0000FC00) >> 10);
	rgb[1] = (byte)((lu1 & 0x000003F0) >> 4);
	rgb[2] = (byte)(((lu1 & 0x0000000F) << 2) | ((lu2 & 0xC0000000) >> 30));
	rgb[3] = (byte)((lu2 & 0x3F000000) >> 24);
	rgb[4] = (byte)((lu2 & 0x00FC0000) >> 18);
	rgb[5] = (byte)((lu2 & 0x0003F000) >> 12);
	rgb[6] = (byte)((lu2 & 0x00000FC0) >> 6);
	rgb[7] = (byte)(lu2 & 0x0000003F);

	//convert the bytes to characters and append them to the stn
	pstn->SetNil();
	for (ib = 0; ib < 8; ib++)
		{
		bT = rgb[ib];
		if (bT == 0)
			break;
		if (FIn(bT, 1, 11))
			bT += '0' - 1;
		else if (FIn(bT, 11, 37))
			bT += 'A' - 11;
		else if (FIn(bT, 37, 63))
			bT += 'a' - 37;
		else
			{
			Assert(bT == 63, 0);
			bT = '_';
			}
		pstn->FAppendCh((achar)bT);
		}

	if (lu1 & 0xFFFF0000)
		{
		STN stn;

		stn.FFormatSz(PszLit("[%d]"), SuHigh(lu1));
		pstn->FAppendStn(&stn);
		}
}

