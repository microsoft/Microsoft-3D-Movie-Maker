/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Script compiler declarations.

***************************************************************************/
#ifndef SCRCOM_H
#define SCRCOM_H


/***************************************************************************
	Opcodes for scripts - these are the opcodes that can actually exist in
	script.  They don't necessarily map directly to the compiler's notion of
	an operator.
***************************************************************************/
//if you change this enum, bump the version numbers below
enum
	{
	opNil,

	//ops that act on variables come first
	kopPushLocVar,
	kopPopLocVar,
	kopPushThisVar,
	kopPopThisVar,
	kopPushGlobalVar,
	kopPopGlobalVar,
	kopPushRemoteVar,
	kopPopRemoteVar,

	kopMinArray = 0x80,
	kopPushLocArray = kopMinArray,
	kopPopLocArray,
	kopPushThisArray,
	kopPopThisArray,
	kopPushGlobalArray,
	kopPopGlobalArray,
	kopPushRemoteArray,
	kopPopRemoteArray,
	kopLimArray,

	kopLimVarSccb = kopLimArray,

	//ops that just act on the stack - no variables
	// for the comments below, a is the top value on the execution stack
	// b is the next to top value on the stack, and c is next on the stack
	kopAdd = 0x100,	// addition: b + a
	kopSub,			// subtraction: b - a
	kopMul,			// multiplication: b * a
	kopDiv,			// integer division: b / a
	kopMod,			// mod: b % a
	kopNeg,			// unary negation: -a
	kopInc,			// unary increment: a + 1
	kopDec,			// unary decrement: a - 1
	kopShr,			// shift right: b >> a
	kopShl,			// shift left: b << a
	kopBOr,			// bitwise or: b | a
	kopBAnd,		// bitwise and: b & a
	kopBXor,		// bitwise exclusive or: b ^ a
	kopBNot,		// unary bitwise not: ~a
	kopLXor,		// logical exclusive or: (b != 0) != (a != 0)
	kopLNot,		// unary logical not: !a
	kopEq,			// equality: b == a
	kopNe,			// not equal: b != a
	kopGt,			// greater than: b > a
	kopLt,			// less than: b < a
	kopGe,			// greater or equal: b >= a
	kopLe,			// less or equal: b <= a

	kopAbs,			// unary absolute value: LwAbs(a)
	kopRnd,			// a random number between 0 and a - 1 (inclusive)
	kopMulDiv,		// a * b / c without loss of precision
	kopDup,			// duplicates the top value
	kopPop,			// discards the top value
	kopSwap,		// swaps the top two values
	kopRot,			// rotates b values by a (c is placed a slots deeper in the stack)
	kopRev,			// reverses the next a slots on the stack
	kopDupList,		// duplicates the next a slots on the stack
	kopPopList,		// pops the next a slots on the stack
	kopRndList,		// picks one of the next a stack values at random
	kopSelect,		// picks the b'th entry among the next a stack values
	kopGoEq,		// if (b == a) jumps to label c
	kopGoNe,		// if (b != a) jumps to label c
	kopGoGt,		// if (b > a) jumps to label c
	kopGoLt,		// if (b < a) jumps to label c
	kopGoGe,		// if (b >= a) jumps to label c
	kopGoLe,		// if (b <= a) jumps to label c
	kopGoZ,			// if (a == 0) jumps to label b
	kopGoNz,		// if (a != 0) jumps to label b
	kopGo,			// jumps to label a
	kopExit,		// terminates the script
	kopReturn,		// terminates the script with return value a
	kopSetReturn,	// sets the return value to a
	kopShuffle,		// shuffles the numbers 0,1,..,a-1 for calls to NextCard
	kopShuffleList,	// shuffles the top a values for calls to NextCard
	kopNextCard,	// returns the next value from the shuffled values
					// when all values have been used, the values are reshuffled
	kopMatch,		// a is a count of pairs, b is the key, c is the default value
					// if b matches the first of any of the a pairs, the second
					// value of the pair is pushed. if not, c is pushed.
	kopPause,		// pause the script (can be resumed later from C code)
	kopCopyStr,		// copy a string within the registry
	kopMoveStr,		// move a string within the registry
	kopNukeStr,		// delete a string from the registry
	kopMergeStrs,	// merge a string table into the registry
	kopScaleTime,	// scale the application clock
	kopNumToStr,	// convert a number to a decimal string
	kopStrToNum,	// convert a string to a number
	kopConcatStrs,	// concatenate two strings
	kopLenStr,		// return the number of characters in the string
	kopCopySubStr,	// copy a piece of the string

	kopLimSccb
	};


// structure to map a string to an opcode (post-fix)
struct SZOP
	{
	long op;
	PSZ psz;
	};

// structure to map a string to an opcode and argument information (in-fix)
struct AROP
	{
	long op;
	PSZ psz;
	long clwFixed;		//number of fixed arguments
	long clwVar;		//number of arguments per variable group
	long cactMinVar;	//minimum number of variable groups
	bool fVoid;			//return a value?
	};

// script version numbers
// if you bump these, also bump the numbers in scrcomg.h
const short kswCurSccb =  0xA;	//this version
const short kswBackSccb = 0xA;	//we can be read back to this version
const short kswMinSccb =  0xA;	//we can read back to this version

// high byte of a label value
const byte kbLabel = 0xCC;


/***************************************************************************
	Run-time variable name.  The first 8 characters of the name are
	significant.  These 8 characters are packed into lu2 and the low
	2 bytes of lu1, so clients can store the info in 6 bytes. The high
	2 bytes of lu1 are used for array subscripts.
***************************************************************************/
struct RTVN
	{
	ulong lu1;
	ulong lu2;

	void SetFromStn(PSTN pstn);
	void GetStn(PSTN pstn);
	};


/***************************************************************************
	The script compiler base class.
***************************************************************************/
typedef class SCCB *PSCCB;
#define SCCB_PAR BASE
#define kclsSCCB 'SCCB'
class SCCB : public SCCB_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	enum
		{
		fsccNil = 0,
		fsccWantVoid = 1,
		fsccTop = 2
		};

	PLEXB _plexb;		// the lexer
	PSCPT _pscpt;		// the script we're building
	PGL _pgletnTree;	// expression tree (in-fix only)
	PGL _pgletnStack;	// token stack for building expression tree (in-fix only)
	PGL _pglcstd;		// control structure stack (in-fix only)
	PGST _pgstNames;	// encountered names (in-fix only)
	PGST _pgstLabel;	// encountered labels, sorted, extra long is label value
	PGST _pgstReq;		// label references, extra long is address of reference
	long _ilwOpLast;	// address of the last opcode
	long _lwLastLabel;	// for internal temporary labels
	bool _fError: 1;	// whether an error has occured during compiling
	bool _fForceOp: 1;	// when pushing a constant, make sure the last long
						// is an opcode (because a label references this loc)
	bool _fHitEnd: 1;	// we've exhausted our input stream
	long _ttEnd;		// stop compiling when we see this
	PMSNK _pmsnk;		// the message sink - for error reporting when compiling

	bool _FInit(PLEXB plexb, bool fInFix, PMSNK pmsnk);
	void _Free(void);

	// general compilation methods
	void _PushLw(long lw);
	void _PushString(PSTN pstn);
	void _PushOp(long op);
	void _EndOp(void);
	void _PushVarOp(long op, RTVN *prtvn);
	bool _FFindLabel(PSTN pstn, long *plwLoc);
	void _AddLabel(PSTN pstn);
	void _PushLabelRequest(PSTN pstn);
	void _AddLabelLw(long lw);
	void _PushLabelRequestLw(long lw);

	virtual void _ReportError(PSZ psz);
	virtual short _SwCur(void);
	virtual short _SwBack(void);
	virtual short _SwMin(void);

	virtual bool _FGetTok(PTOK ptok);

	// post-fix compiler routines
	virtual void _CompilePost(void);
	long _OpFromStnRgszop(PSTN pstn, SZOP *prgszop);
	virtual long _OpFromStn(PSTN pstn);
	bool _FGetStnFromOpRgszop(long op, PSTN pstn, SZOP *prgszop);
	virtual bool _FGetStnFromOp(long op, PSTN pstn);

	// in-fix compiler routines
	virtual void _CompileIn(void);
	bool _FResolveToOpl(long opl, long oplMin, long *pietn);
	void _EmitCode(long ietnTop, ulong grfscc, long *pclwArg);
	void _EmitVarAccess(long ietn, RTVN *prtvn,
		long *popPush, long *popPop, long *pclwStack);
	virtual bool _FGetOpFromName(PSTN pstn, long *pop, long *pclwFixed,
		long *pclwVar, long *pcactMinVar, bool *pfVoid);
	bool _FGetArop(PSTN pstn, AROP *prgarop, long *pop, long *pclwFixed,
		long *pclwVar, long *pcactMinVar, bool *pfVoid);
	void _PushLabelRequestIetn(long ietn);
	void _AddLabelIetn(long ietn);
	void _PushOpFromName(long ietn, ulong grfscc, long clwArg);
	void _GetIstnNameFromIetn(long ietn, long *pistn);
	void _GetRtvnFromName(long istn, RTVN *prtvn);
	bool _FKeyWord(PSTN pstn);
	void _GetStnFromIstn(long istn, PSTN pstn);
	void _AddNameRef(PSTN pstn, long *pistn);
	long _CstFromName(long ietn);
	void _BeginCst(long cst, long ietn);
	bool _FHandleCst(long ietn);
	bool _FAddToTree(struct ETN *petn, long *pietn);
	bool _FConstEtn(long ietn, long *plw);
	bool _FCombineConstValues(long op, long lw1, long lw2, long *plw);
	void _SetDepth(struct ETN *petn, bool fCommute = fFalse);
	void _PushStringIstn(long istn);

public:
	SCCB(void);
	~SCCB(void);

	virtual PSCPT PscptCompileLex(PLEXB plexb, bool fInFix, PMSNK pmsnk,
		long ttEnd = ttNil);
	virtual PSCPT PscptCompileFil(PFIL pfil, bool fInFix, PMSNK pmsnk);
	virtual PSCPT PscptCompileFni(FNI *pfni, bool fInFix, PMSNK pmsnk);
	virtual bool FDisassemble(PSCPT pscpt, PMSNK pmsnk, PMSNK pmsnkError = pvNil);
	};


#endif //!SCRCOM_H

