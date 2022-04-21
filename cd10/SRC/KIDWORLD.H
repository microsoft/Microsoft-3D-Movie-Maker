/***************************************************************************
	Author: ******
	Project: Kauai
	Copyright (c) Microsoft Corporation

	This is a class that knows how to create GOKs, Help Balloons and
	Kidspace script interpreters. It exists so an app can customize
	default behavior.

***************************************************************************/
#ifndef KIDWORLD_H
#define KIDWORLD_H


/***************************************************************************
	Base GOK descriptor.
***************************************************************************/
//location from parent map structure
struct LOP
	{
	long hidPar;
	long xp;
	long yp;
	long zp;	//the z-plane number used for placing the GOK in the GOB tree
	};

//cursor map entry
struct CUME
	{
	ulong grfcustMask;	// what cursor states this CUME is good for
	ulong grfcust;
	ulong grfbitSno;	// what button states this CUME is good for
	CNO cnoCurs;		// the cursor to use
	CHID chidScript;	// execution script (absolute)
	long cidDefault;	// default command
	CNO cnoTopic;		// tool tip topic
	};

typedef class GOKD *PGOKD;
#define GOKD_PAR BACO
#define kclsGOKD 'GOKD'
class GOKD : public GOKD_PAR
	{
	RTCLASS_DEC

protected:
	GOKD(void) {}

public:
	virtual long Gokk(void) = 0;
	virtual bool FGetCume(ulong grfcust, long sno, CUME *pcume) = 0;
	virtual void GetLop(long hidPar, LOP *plop) = 0;
	};


/***************************************************************************
	Standard GOK descriptor. Contains location information and cursor
	map stuff.
***************************************************************************/
//GOK construction descriptor on file - these are stored in chunky resource files
struct GOKDF
	{
	short bo;
	short osk;
	long gokk;
	//LOP rglop[];		ends with a default entry (hidPar == hidNil)
	//CUME rgcume[];	the cursor map
	};
const BOM kbomGokdf = 0x0C000000;


typedef class GKDS *PGKDS;
#define GKDS_PAR GOKD
#define kclsGKDS 'GKDS'
class GKDS : public GKDS_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	HQ _hqData;
	long _gokk;
	long _clop;
	long _ccume;

	GKDS(void) {}

public:
	//An object reader for a GOKD.
	static bool FReadGkds(PCRF pcrf, CTG ctg, CNO cno, BLCK *pblck,
		PBACO *ppbaco, long *pcb);
	~GKDS(void);

	virtual long Gokk(void);
	virtual bool FGetCume(ulong grfcust, long sno, CUME *pcume);
	virtual void GetLop(long hidPar, LOP *plop);
	};


/***************************************************************************
	World of Kidspace class.
***************************************************************************/
typedef class WOKS *PWOKS;
#define WOKS_PAR GOB
#define kclsWOKS 'WOKS'
class WOKS : public WOKS_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PSTRG _pstrg;
	STRG _strg;
	ulong _grfcust;

	CLOK _clokAnim;
	CLOK _clokNoSlip;
	CLOK _clokGen;
	CLOK _clokReset;

public:
	WOKS(GCB *pgcb, PSTRG pstrg = pvNil);
	~WOKS(void);

	PSTRG Pstrg(void)
		{ return _pstrg; }

	virtual bool FGobIn(PGOB pgob);
	virtual PGOKD PgokdFetch(CTG ctg, CNO cno, PRCA prca);
	virtual PGOK PgokNew(PGOB pgobPar, long hid, CNO cno, PRCA prca);
	virtual PSCEG PscegNew(PRCA prca, PGOB pgob);
	virtual PHBAL PhbalNew(PGOB pgobPar, PRCA prca, CNO cnoTopic,
		PHTOP phtop = pvNil);
	virtual PCMH PcmhFromHid(long hid);
	virtual PGOB PgobParGob(PGOB pgob);
	virtual bool FFindFile(PSTN pstnSrc, PFNI pfni);
	virtual bool TGiveAlert(PSTN pstn, long bk, long cok);
	virtual void Print(PSTN pstn);

	virtual ulong GrfcustCur(bool fAsynch = fFalse);
	virtual void ModifyGrfcust(ulong grfcustOr, ulong grfcustXor);
	virtual ulong GrfcustAdjust(ulong grfcust);

	virtual bool FModalTopic(PRCA prca, CNO cnoTopic, long *plwRet);
	virtual PCLOK PclokAnim(void)
		{ return &_clokAnim; }
	virtual PCLOK PclokNoSlip(void)
		{ return &_clokNoSlip; }
	virtual PCLOK PclokGen(void)
		{ return &_clokGen; }
	virtual PCLOK PclokReset(void)
		{ return &_clokReset; }
	virtual PCLOK PclokFromHid(long hid);
	};

#endif //!KIDWORLD_H

