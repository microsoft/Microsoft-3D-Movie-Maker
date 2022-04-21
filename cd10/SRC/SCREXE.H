/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Script interpreter.  See scrcom.h and scrcom.cpp for the script
	compiler and an explanation of what a compiled script consists of.

***************************************************************************/
#ifndef SCREXE_H
#define SCREXE_H


/****************************************
	Run-Time Variable Map structure
****************************************/
struct RTVM
	{
	RTVN rtvn;
	long lwValue;
	};

bool FFindRtvm(PGL pglrtvm, RTVN *prtvn, long *plwValue, long *pirtvm);
bool FAssignRtvm(PGL *ppglrtvm, RTVN *prtvn, long lw);

/***************************************************************************
	A script.  This is here rather than in scrcom.* because scrcom is
	rarely included in shipping products, but screxe.* is.
***************************************************************************/
typedef class SCPT *PSCPT;
#define SCPT_PAR BACO
#define kclsSCPT 'SCPT'
class SCPT : public SCPT_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:
	PGL _pgllw;
	PGST _pgstLiterals;

	SCPT(void) {}

	friend class SCEB;
	friend class SCCB;

public:
	static bool FReadScript(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
		PBACO *ppbaco, long *pcb);
	static PSCPT PscptRead(PCFL pcfl, CTG ctg, CNO cno);
	~SCPT(void);

	bool FSaveToChunk(PCFL pcfl, CTG ctg, CNO cno, bool fPack = fFalse);
	};


/***************************************************************************
	Runtime string registry.
***************************************************************************/
typedef class STRG *PSTRG;
#define STRG_PAR BASE
#define kclsSTRG 'STRG'
class STRG : public STRG_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(STRG)

protected:
	long _stidLast;
	PGST _pgst;

	bool _FFind(long stid, long *pistn);
	bool _FEnsureGst(void);

public:
	STRG(void);
	~STRG(void);

	bool FPut(long stid, PSTN pstn);
	bool FGet(long stid, PSTN pstn);
	bool FAdd(long *pstid, PSTN pstn);
	bool FMove(long stidSrc, long stidDst);
	void Delete(long stid);
	};


/***************************************************************************
	The script interpreter.
***************************************************************************/
enum
	{
	fscebNil = 0,
	fscebRunnable = 1,
	};

typedef class SCEB *PSCEB;
#define SCEB_PAR BASE
#define kclsSCEB 'SCEB'
class SCEB : public SCEB_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PRCA _prca;			//the chunky resource file list (may be nil)
	PSTRG _pstrg;
	PGL _pgllwStack;	//the execution stack
	PGL _pglrtvm;		//the local variables
	PSCPT _pscpt;		//the script
	long _ilwMac;		//the length of the script
	long _ilwCur;		//the current location in the script
	bool _fError: 1;	//an error has occured
	bool _fPaused: 1;	//if we're paused
	long _lwReturn;		//the return value from the script

	void _Push(long lw)
		{
		if (!_fError && !_pgllwStack->FPush(&lw))
			_Error(fFalse);
		}
	long _LwPop(void);
	long *_QlwGet(long clw);
	void _Error(bool fAssert);

	void _Rotate(long clwTot, long clwShift);
	void _Reverse(long clw);
	void _DupList(long clw);
	void _PopList(long clw);
	void _Select(long clw, long ilw);
	void _RndList(long clw);
	void _Match(long clw);
	void _CopySubStr(long stidSrc, long ichMin, long cch, long stidDst);
	void _MergeStrings(CNO cno, RSC rsc);
	void _NumToStr(long lw, long stid);
	void _StrToNum(long stid, long lwEmpty, long lwError);
	void _ConcatStrs(long stidSrc1, long stidSrc2, long stidDst);
	void _LenStr(long stid);

	virtual void _AddParameters(long *prglw, long clw);
	virtual void _AddStrings(PGST pgst);
	virtual bool _FExecVarOp(long op, RTVN *prtvn);
	virtual bool _FExecOp(long op);
	virtual void _PushVar(PGL pglrtvm, RTVN *prtvn);
	virtual void _AssignVar(PGL *ppglrtvm, RTVN *prtvn, long lw);
	virtual PGL _PglrtvmThis(void);
	virtual PGL *_PpglrtvmThis(void);
	virtual PGL _PglrtvmGlobal(void);
	virtual PGL *_PpglrtvmGlobal(void);
	virtual PGL _PglrtvmRemote(long lw);
	virtual PGL *_PpglrtvmRemote(long lw);

	virtual short _SwCur(void);
	virtual short _SwMin(void);

	#ifdef DEBUG
		void _WarnSz(PSZ psz, ...);
	#endif //DEBUG

public:
	SCEB(PRCA prca = pvNil, PSTRG pstrg = pvNil);
	~SCEB(void);

	virtual bool FRunScript(PSCPT pscpt, long *prglw = pvNil, long clw = 0,
		long *plwReturn = pvNil, bool *pfPaused = pvNil);
	virtual bool FResume(long *plwReturn = pvNil, bool *pfPaused = pvNil);
	virtual bool FAttachScript(PSCPT pscpt, long *prglw = pvNil, long clw = 0);
	virtual void Free(void);
	};


#endif //!SCREXE_H

