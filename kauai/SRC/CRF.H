/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Chunky resource file management. A CRF is a cache wrapped around a
	chunky file. A CRM is a list of CRFs. A BACO is an object that
	can be cached in a CRF. An RCA is an interface that CRF and CRM both
	implement (are a super set of).

***************************************************************************/
#ifndef CHRES_H
#define CHRES_H

typedef class CRF *PCRF;
typedef CNO RSC;
const RSC rscNil = 0L;


//chunky resource entry priority
enum
	{
	crepToss,
	crepTossFirst,
	crepNormal,
	};

/***************************************************************************
	Base cacheable object.  All cacheable objects must be based on BACO.
***************************************************************************/
typedef class BACO *PBACO;
#define BACO_PAR BASE
#define kclsBACO 'BACO'
class BACO : public BACO_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

private:
	// These fields are owned by the CRF
	PCRF _pcrf;		// The BACO has a ref count on this iff !_fAttached
	CTG _ctg;
	CNO _cno;
	long _crep: 16;
	long _fAttached: 1;

	friend class CRF;

protected:
	BACO(void);
	~BACO(void);

public:
	virtual void Release(void);

	virtual void SetCrep(long crep);
	virtual void Detach(void);

	CTG Ctg(void)
		{ return _ctg; }
	CNO Cno(void)
		{ return _cno; }
	PCRF Pcrf(void)
		{ return _pcrf; }
	long Crep(void)
		{ return _crep; }

	// Many objects know how big they are, and how to write themselves to a
	// chunky file.  Here are some useful prototypes so that the users of those
	// objects don't need to know what the actual class is.
	virtual bool FWrite(PBLCK pblck);
	virtual bool FWriteFlo(PFLO pflo);
	virtual long CbOnFile(void);
	};


/***************************************************************************
	Chunky resource cache - this is a pure virtual class that supports
	the crf and crm classes.
***************************************************************************/
// Object reader function - must handle ppo == pvNil, in which case, the
// *pcb should be set to an estimate of the size when read.
typedef bool FNRPO(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb);
typedef FNRPO *PFNRPO;

typedef class RCA *PRCA;
#define RCA_PAR BASE
#define kclsRCA 'RCA'
class RCA : public RCA_PAR
	{
	RTCLASS_DEC

public:
	virtual bool TLoad(CTG ctg, CNO cno, PFNRPO pfnrpo,
		RSC rsc = rscNil, long crep = crepNormal) = 0;
	virtual PBACO PbacoFetch(CTG ctg, CNO cno, PFNRPO pfnrpo,
		bool *pfError = pvNil, RSC rsc = rscNil) = 0;
	virtual PBACO PbacoFind(CTG ctg, CNO cno, PFNRPO pfnrpo,
		RSC rsc = rscNil) = 0;
	virtual bool FSetCrep(long crep, CTG ctg, CNO cno, PFNRPO pfnrpo,
		RSC rsc = rscNil) = 0;
	virtual PCRF PcrfFindChunk(CTG ctg, CNO cno, RSC rsc = rscNil) = 0;
	};


/***************************************************************************
	Chunky resource file.
***************************************************************************/
#define CRF_PAR RCA
#define kclsCRF 'CRF'
class CRF : public CRF_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	struct CRE
		{
		PFNRPO pfnrpo;		// object reader
		long cactRelease;	// the last time this object was released
		BACO *pbaco;		// the object
		long cb;			// size of data
		};

	PCFL _pcfl;
	PGL _pglcre;	// sorted by (cki, pfnrpo)
	long _cbMax;
	long _cbCur;
	long _cactRelease;

	CRF(PCFL pcfl, long cbMax);
	bool _FFindCre(CTG ctg, CNO cno, PFNRPO pfnrpo, long *picre);
	bool _FFindBaco(PBACO pbaco, long *picre);
	bool _FPurgeCb(long cbPurge, long crepLast);

public:
	~CRF(void);
	static PCRF PcrfNew(PCFL pcfl, long cbMax);

	virtual bool TLoad(CTG ctg, CNO cno, PFNRPO pfnrpo,
		RSC rsc = rscNil, long crep = crepNormal);
	virtual PBACO PbacoFetch(CTG ctg, CNO cno, PFNRPO pfnrpo,
		bool *pfError = pvNil, RSC rsc = rscNil);
	virtual PBACO PbacoFind(CTG ctg, CNO cno, PFNRPO pfnrpo, RSC rsc = rscNil);
	virtual bool FSetCrep(long crep, CTG ctg, CNO cno, PFNRPO pfnrpo,
		RSC rsc = rscNil);
	virtual PCRF PcrfFindChunk(CTG ctg, CNO cno, RSC rsc = rscNil);

	long CbMax(void)
		{ return _cbMax; }
	void SetCbMax(long cbMax);

	PCFL Pcfl(void)
		{ return _pcfl; }

	// These APIs are intended for BACO use only
	void BacoDetached(PBACO pbaco);
	void BacoReleased(PBACO pbaco);
	};


/***************************************************************************
	Chunky resource manager - a list of CRFs.
***************************************************************************/
typedef class CRM *PCRM;
#define CRM_PAR RCA
#define kclsCRM 'CRM'
class CRM : public CRM_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PGL _pglpcrf;

	CRM(void) {}

public:
	~CRM(void);
	static PCRM PcrmNew(long ccrfInit);

	virtual bool TLoad(CTG ctg, CNO cno, PFNRPO pfnrpo,
		RSC rsc = rscNil, long crep = crepNormal);
	virtual PBACO PbacoFetch(CTG ctg, CNO cno, PFNRPO pfnrpo,
		bool *pfError = pvNil, RSC rsc = rscNil);
	virtual PBACO PbacoFind(CTG ctg, CNO cno, PFNRPO pfnrpo, RSC rsc = rscNil);
	virtual bool FSetCrep(long crep, CTG ctg, CNO cno, PFNRPO pfnrpo,
		RSC rsc = rscNil);
	virtual PCRF PcrfFindChunk(CTG ctg, CNO cno, RSC rsc = rscNil);

	bool FAddCfl(PCFL pcfl, long cbMax, long *piv = pvNil);
	long Ccrf(void)
		{
		AssertThis(0);
		return _pglpcrf->IvMac();
		}
	PCRF PcrfGet(long icrf);
	};


/***************************************************************************
	An object (BACO) wrapper around a generic HQ.
***************************************************************************/
#define GHQ_PAR BACO
typedef class GHQ *PGHQ;
#define kclsGHQ 'GHQ'
class GHQ : public GHQ_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

public:
	HQ hq;

	GHQ(HQ hqT)
		{ hq = hqT; }
	~GHQ(void)
		{ FreePhq(&hq); }

	// An object reader for a GHQ.
	static bool FReadGhq(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
		PBACO *ppbaco, long *pcb);
	};


/***************************************************************************
	A BACO wrapper around a generic object.
***************************************************************************/
#define CABO_PAR BACO
typedef class CABO *PCABO;
#define kclsCABO 'CABO'
class CABO : public CABO_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

public:
	BASE *po;

	CABO(BASE *poT)
		{ po = poT; }
	~CABO(void)
		{ ReleasePpo(&po); }
	};

#endif //!CHRES_H

