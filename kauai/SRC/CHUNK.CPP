/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Chunky file classes.

	Chunky files store distinct "chunks" of data. Each chunk is identified
	by a chunk tag (CTG) and chunk number (CNO) pair. By convention, CTGs
	consist of 4 ASCII characters. CTGs that are shorter than 4 characters
	should be padded with spaces (eg, 'GST '). The CFL class does not
	enforce these conventions, but many Kauai tools assume that CTGs
	consist of 4 printable characters.

	Chunks store arbitrary data. Chunks can have names. Most importantly,
	chunks can have child chunks. A chunk can also be a child of more
	than one chunk and can be a child of a chunk mutliple times. The main
	restriction on parent-child relationships is that there can be no loops.
	Mathematically speaking, the chunks form an acyclic directed graph.

	A parent-child relationship has a client specified number associated
	with it. This is called the child id (CHID). Child id's don't have to
	be unique. CFL places no significance on child id's. The only
	restriction concerning CHIDs is that if chunk B is to be a child of
	chunk A twice, then those two parent-child relationships must have
	different child id's. In a nutshell, a parent-child relationship
	(an arrow in the acyclic directed graph) is uniquely identified by
	the 5-tuple (ctgPar, cnoPar, ctgChild, cnoChild, chid).

	On disk, chunky files are organized as follows:

		* A header (CFP struct defined below).
		* The raw chunk data arranged in arbitrary order (a heap).
		* The index for the chunky file.
		* An optional free map indicating which portions of the heap
		  are currently not being used to store chunk data.

	The header stores signatures, version numbers, and pointers to the
	index and free map. The heap contains only the raw chunk data.
	The index is not updated on disk until FSave is called.

	The index is implemented as a general group (GG). The fixed portion
	of each entry is a CRP (defined below). The variable portion contains
	the list of children of the chunk (including CHID values) and the
	chunk name (if it has one).

	The CRP indicates where in the heap the chunk data is, whether
	the chunk data is compressed, whether the chunk is a "loner" and how
	many parents the chunk has. Loner chunks are ones that can exist without
	a parent. When pcfl->Delete(ctg, cno) is called, the chunk's loner
	flag is cleared. If the chunk has no parents, the chunk is deleted
	(by removing its CRP from the index and adding the space in the
	heap that it occupied to the free map). We also decrement the
	parent counts of the children of the chunk. Child chunks whose
	parent count becomes zero and are not loner chunks are then deleted,
	and the process continues down through the chunk graph.

	The CFL class implements intelligent chunk copying within a run-time
	session. Suppose a source CFL contains two chunks A and B, with B a
	child of A. If chunk A is copied to another chunky file, its entire
	subgraph is copied, so B is copied with it. Suppose however that chunk
	B was previously copied to the destination, then A is copied. We see
	that chunk B is already in the destination CFL, so don't actually copy
	the chunk - we just copy A and make the previously coped B a child of
	the newly copied A. If this is not what you want, use FClone instead
	of FCopy. FClone copies the entire chunk subgraph, but does not use
	any previously copied chunks. The intelligent chunk copying only
	happens within a single run-time session (from the files point of view).
	If the chunky files are closed and re-opened, no sharing will occur.

***************************************************************************/
#include "util.h"
ASSERTNAME


/* HISTORY of CFL version numbers

	1	ShonK: instantiated, 10/28/93
	2	ShonK: added compression support, 2/14/95
	3	ShonK: changed format of chunk names to be a saved STN, 3/30/95
	4	ShonK: implemented support for compact index (CRP is smaller). 8/21/95
	5	ShonK: added the fcrpForest flag. 9/4/95

*/

// A file written by this version of chunk.cpp receives this cvn.  Any
// file with this cvn value has exactly the same file format
const short kcvnCur = 5;

// A file written by this version of chunk.cpp can be read by any version
// of chunk.cpp whose kcvnCur is >= to this (this should be <= kcvnCur)
const short kcvnBack = 4;

// A file whose cvn is less than kcvnMin cannot be directly read by
// this version of chunk.cpp (maybe a converter will read it).
// (this should be <= kcvnCur)
// NOTE: (ShonK, 3/30/95): if this is >= 3, the fOldNames handling in
// _FReadIndex can be removed!
// NOTE: (ShonK, 8/21/95): if this is >= 4, the fOldIndex handling in
// _FReadIndex can be removed!
const short kcvnMin = 1;

const kcvnMinStnNames = 3;
const kcvnMinGrfcrp = 4;
const kcvnMinSmallIndex = 4;
const kcvnMinForest = 5;


const long klwMagicChunky = BigLittle('CHN2', '2NHC'); //chunky file signature

// chunky file prefix
struct CFP
	{
	long lwMagic;	// identifies this as a chunky file
	CTG ctgCreator;	// program that created this file
	DVER dver;		// chunky file version
	short bo;		// byte order
	short osk;		// which system wrote this

	FP fpMac;		// logical end of file
	FP fpIndex;		// location of chunky index
	long cbIndex;	// size of chunky index
	FP fpMap;		// location of free space map
	long cbMap;		// size of free space map (may be 0)

	long rglwReserved[23];	// reserved for future use - should be zero
	};
const BOM kbomCfp = 0xB55FFC00L;


// free space map entry
struct FSM
	{
	FP fp;
	long cb;
	};
const BOM kbomFsm = 0xF0000000L;


enum
	{
	fcrpNil = 0,
	fcrpOnExtra = 0x01,	// data is on the extra file
	fcrpLoner = 0x02,	// the chunk can stand (may also be a child)
	fcrpPacked = 0x04,	// the data is compressed
	fcrpMarkT = 0x08,	// used for consistency checks (see _TValidIndex)
	fcrpForest = 0x10,	// the chunk contains a forest of chunks
	};


// Chunk Representation (big version) - fixed element in pggcrp
// variable part of group element is an rgkid and stn data (the name)
const kcbMaxCrpbg = klwMax;
struct CRPBG
	{
	CKI cki;		// chunk id
	FP fp;			// location on file
	long cb;		// size of data on file
	long ckid;		// number of owned chunks
	long ccrpRef;	// number of owners of this chunk
	long rti;		// run-time id
	union
		{
		struct
			{
			// for cvn <= kcvnMinGrfcrp
			byte fOnExtra;	// fcrpOnExtra
			byte fLoner;	// fcrpLoner
			byte fPacked;	// fcrpPacked
			byte bT;		// fcrpMarkT
			};

		// for cvn >= kcvnMinGrfcrp
		ulong grfcrp;
		};

	long BvRgch(void)
		{ return LwMul(ckid, size(KID)); }
	long CbRgch(long cbVar)
		{ return cbVar - BvRgch(); }

	ulong Grfcrp(ulong grfcrpMask = (ulong)(-1))
		{ return grfcrp & grfcrpMask; }
	void ClearGrfcrp(ulong grfcrpT)
		{ grfcrp &= ~grfcrpT; }
	void SetGrfcrp(ulong grfcrpT)
		{ grfcrp |= grfcrpT; }
	void AssignGrfcrp(ulong grfcrpT, ulong grfcrpMask = (ulong)(-1))
		{ grfcrp = grfcrp & ~grfcrpMask | grfcrpT & grfcrpMask; }
	long Cb(void)
		{ return cb; }
	void SetCb(long cbT)
		{ cb = cbT; }
	};
const BOM kbomCrpbgGrfcrp = 0xFFFF0000L;
const BOM kbomCrpbgBytes = 0xFFFE0000L;


// Chunk Representation (small version) - fixed element in pggcrp
// variable part of group element is an rgkid and stn data (the name)
const kcbMaxCrpsm = 0x00FFFFFF;
const long kcbitGrfcrp = 8;
const ulong kgrfcrpAll = (1 << kcbitGrfcrp) - 1;
struct CRPSM
	{
	CKI cki;			// chunk id
	FP fp;				// location on file
	ulong luGrfcrpCb;	// low byte is the grfcrp, high 3 bytes is cb
	ushort ckid;		// number of owned chunks
	ushort ccrpRef;		// number of owners of this chunk

	long BvRgch(void)
		{ return LwMul(ckid, size(KID)); }
	long CbRgch(long cbVar)
		{ return cbVar - BvRgch(); }

	ulong Grfcrp(ulong grfcrpMask = (ulong)(-1))
		{ return luGrfcrpCb & grfcrpMask & kgrfcrpAll; }
	void ClearGrfcrp(ulong grfcrpT)
		{ luGrfcrpCb &= ~(grfcrpT & kgrfcrpAll); }
	void SetGrfcrp(ulong grfcrpT)
		{ luGrfcrpCb |= grfcrpT & kgrfcrpAll; }
	void AssignGrfcrp(ulong grfcrpT, ulong grfcrpMask = (ulong)(-1))
		{
		luGrfcrpCb = luGrfcrpCb & ~(grfcrpMask & kgrfcrpAll) |
			grfcrpT & grfcrpMask & kgrfcrpAll;
		}
	long Cb(void)
		{ return luGrfcrpCb >> kcbitGrfcrp; }
	void SetCb(long cbT)
		{ luGrfcrpCb = (cbT << kcbitGrfcrp) | luGrfcrpCb & kgrfcrpAll; }
	};
const BOM kbomCrpsm = 0xFF500000L;


#ifdef CHUNK_BIG_INDEX

typedef CRPBG CRP;
typedef CRPSM CRPOTH;
const long kcbMaxCrp = kcbMaxCrpbg;
typedef long CKID;
const long kckidMax = kcbMax;
const BOM kbomCrpsm = kbomCrpbgGrfcrp;

#else //!CHUNK_BIG_INDEX

typedef CRPSM CRP;
typedef CRPBG CRPOTH;
const long kcbMaxCrp = kcbMaxCrpsm;
typedef ushort CKID;
const long kckidMax = ksuMax;
const BOM kbomCrp = kbomCrpsm;

#endif //!CHUNK_BIG_INDEX


#define _BvKid(ikid) LwMul(ikid, size(KID))


const long rtiNil = 0;	// no rti assigned
long CFL::_rtiLast = rtiNil;
PCFL CFL::_pcflFirst;


#ifdef CHUNK_STATS
bool vfDumpChunkRequests = fTrue;
PFIL _pfilStats;
FP _fpStats;

/***************************************************************************
	Static method to dump a string to the chunk stats file
***************************************************************************/
void CFL::DumpStn(PSTN pstn, PFIL pfil)
{
	FNI fni;
	STN stn;

	if (pvNil == _pfilStats)
		{
		stn = PszLit("ChnkStat.txt");
		if (!fni.FGetTemp() || !fni.FSetLeaf(&stn) ||
				pvNil == (_pfilStats = FIL::PfilCreate(&fni)))
			{
			return;
			}
		}

	if (pvNil != pfil)
		{
		pfil->GetFni(&fni);
		fni.GetStnPath(&stn);
		stn.FAppendSz(PszLit(": "));
		_pfilStats->FWriteRgbSeq(stn.Prgch(), LwMul(stn.Cch(), size(achar)),
			&_fpStats);
		}

	_pfilStats->FWriteRgbSeq(pstn->Prgch(), LwMul(pstn->Cch(), size(achar)),
		&_fpStats);
	_pfilStats->FWriteRgbSeq(PszLit("\xD\xA"),
		MacWin(size(achar), 2 * size(achar)), &_fpStats);
}
#endif //CHUNK_STATS


RTCLASS(CFL)
RTCLASS(CGE)


/***************************************************************************
	Constructor for CFL - private.
***************************************************************************/
CFL::CFL(void)
{
	// add it to the linked list
	_Attach(&_pcflFirst);
	AssertBaseThis(0);
}


/***************************************************************************
	Close the associated files.  Private.
***************************************************************************/
CFL::~CFL(void)
{
	AssertBaseThis(0);
	ReleasePpo(&_csto.pfil);
	ReleasePpo(&_csto.pglfsm);
	ReleasePpo(&_cstoExtra.pfil);
	ReleasePpo(&_cstoExtra.pglfsm);
	ReleasePpo(&_pggcrp);
#ifndef CHUNK_BIG_INDEX
	ReleasePpo(&_pglrtie);
#endif //!CHUNK_BIG_INDEX
}


/***************************************************************************
	Static method: open an existing file as a chunky file.  Increments the
	open count.
***************************************************************************/
PCFL CFL::PcflOpen(FNI *pfni, ulong grfcfl)
{
	AssertPo(pfni, ffniFile);
	PCFL pcfl;
	ulong grffil;

	Assert(!(grfcfl & fcflTemp), "can't open a file as temp");
	if (pvNil != (pcfl = PcflFromFni(pfni)))
		{
		if (!pcfl->FSetGrfcfl(grfcfl))
			return pvNil;
		pcfl->AddRef();
		return pcfl;
		}

	grffil = _GrffilFromGrfcfl(grfcfl);
	if ((pcfl = NewObj CFL()) == pvNil)
		goto LFail;

	if ((pcfl->_csto.pfil = FIL::PfilOpen(pfni, grffil)) == pvNil ||
			!pcfl->_FReadIndex())
		{
		goto LFail;
		}
	if (tYes != pcfl->_TValidIndex())
		{
LFail:
		ReleasePpo(&pcfl);
		PushErc(ercCflOpen);
		return pvNil;
		}
	AssertDo(pcfl->FSetGrfcfl(grfcfl), 0);

	// We don't assert with fcflGraph, because we've already
	// called _TValidIndex.
	AssertPo(pcfl, fcflFull);
	return pcfl;
}


/***************************************************************************
	If the file is read-only, toss the index and extra file and re-read
	the index.
***************************************************************************/
bool CFL::FReopen(void)
{
	AssertThis(0);
	CSTO csto, cstoExtra;
	PGG pggcrp;
#ifndef CHUNK_BIG_INDEX
	PGL pglrtie;
#endif //CHUNK_BIG_INDEX
	bool fFreeMapNotRead;
	bool fRet;

	if (_csto.pfil->GrffilCur() & ffilWriteEnable || _fInvalidMainFile)
		return fFalse;

	pggcrp = _pggcrp;
	_pggcrp = pvNil;

#ifndef CHUNK_BIG_INDEX
	pglrtie = _pglrtie;
	_pglrtie = pvNil;
#endif //CHUNK_BIG_INDEX

	fFreeMapNotRead = _fFreeMapNotRead;
	_fFreeMapNotRead = fFalse;

	csto = _csto;
	ClearPb(&_csto, size(csto));
	_csto.pfil = csto.pfil;
	_csto.pfil->AddRef();

	cstoExtra = _cstoExtra;
	ClearPb(&_cstoExtra, size(cstoExtra));

	fRet = _FReadIndex() && tYes == _TValidIndex();
	if (!fRet)
		{
		SwapVars(&_pggcrp, &pggcrp);
#ifndef CHUNK_BIG_INDEX
		SwapVars(&_pglrtie, &pglrtie);
#endif //CHUNK_BIG_INDEX
		SwapVars(&_csto, &csto);
		SwapVars(&_cstoExtra, &cstoExtra);
		_fFreeMapNotRead = FPure(fFreeMapNotRead);
		}
	ReleasePpo(&pggcrp);
#ifndef CHUNK_BIG_INDEX
	ReleasePpo(&pglrtie);
#endif //CHUNK_BIG_INDEX
	ReleasePpo(&csto.pfil);
	ReleasePpo(&csto.pglfsm);
	ReleasePpo(&cstoExtra.pfil);
	ReleasePpo(&cstoExtra.pglfsm);

	AssertThis(0);
	return fRet;
}


/***************************************************************************
	Static method to get file options corresponding to the given chunky
	file options.
***************************************************************************/
ulong CFL::_GrffilFromGrfcfl(ulong grfcfl)
{
	ulong grffil = ffilDenyWrite;

	if (grfcfl & fcflWriteEnable)
		grffil |= ffilWriteEnable | ffilDenyRead;

	if (grfcfl & fcflTemp)
		grffil |= ffilTemp;

	return grffil;
}


/***************************************************************************
	Static method: create a new file.  Increments the open count.
***************************************************************************/
PCFL CFL::PcflCreate(FNI *pfni, ulong grfcfl)
{
	AssertPo(pfni, ffniFile);
	PCFL pcfl;
	ulong grffil;

	grfcfl |= fcflWriteEnable;
	grffil = _GrffilFromGrfcfl(grfcfl);

	if (pvNil != (pcfl = CFL::PcflFromFni(pfni)))
		{
		Bug("trying to create an open file");
		goto LFail;
		}

	if ((pcfl = NewObj CFL()) == pvNil)
		goto LFail;

	if ((pcfl->_pggcrp = GG::PggNew(size(CRP))) == pvNil ||
		(pcfl->_csto.pfil = FIL::PfilCreate(pfni, grffil)) == pvNil ||
		!pcfl->_csto.pfil->FSetFpMac(size(CFP)))
		{
		ReleasePpo(&pcfl);
LFail:
		PushErc(ercCflCreate);
		return pvNil;
		}
	pcfl->_csto.fpMac = size(CFP);
	AssertDo(pcfl->FSetGrfcfl(grfcfl), 0);

	AssertPo(pcfl, fcflFull | fcflGraph);
	return pcfl;
}


/***************************************************************************
	Static method to create a temporary chunky file in the same directory as
	*pfni and with the same ftg.  If pfni is nil, the file is created in
	the standard place with a temp ftg.
***************************************************************************/
PCFL CFL::PcflCreateTemp(FNI *pfni)
{
	AssertNilOrPo(pfni, ffniFile);
	FNI fni;

	if (pvNil != pfni)
		{
		fni = *pfni;
		if (!fni.FGetUnique(pfni->Ftg()))
			goto LFail;
		}
	else if (!fni.FGetTemp())
		{
LFail:
		PushErc(ercCflCreate);
		return pvNil;
		}

	return PcflCreate(&fni, fcflTemp);
}


/***************************************************************************
	Static method: if we have the chunky file indicated by fni open, returns
	the pcfl, otherwise returns pvNil.  Doesn't affect the open count.
***************************************************************************/
PCFL CFL::PcflFromFni(FNI *pfni)
{
	AssertPo(pfni, ffniFile);
	PFIL pfil;
	PCFL pcfl;

	if ((pfil = FIL::PfilFromFni(pfni)) == pvNil)
		return pvNil;
	for (pcfl = _pcflFirst; pcfl != pvNil; pcfl = pcfl->PcflNext())
		{
		Assert(pfil != pcfl->_cstoExtra.pfil, "who is using this FNI?");
		if (pcfl->_csto.pfil == pfil && !pcfl->_fInvalidMainFile)
			break;
		}
	return pcfl;
}


/***************************************************************************
	Embedded chunk descriptor on file. Chunks and their subtrees can
	be combined into a single stream by the following rules:

	CF stands for a chunk forest. CT stands for a chunk tree.
	ECDF(n, m) stands for an ECDF structure with cb == n and ckid == m.
	data(n) stands for n bytes of data.

		CF -> (empty)
		CF -> CT CF
		CT -> ECDF(n, 0) data(n)
		CT -> EDCF(n, m) data(n) CT CT ... CT (m times)

	CFL::FWriteChunkTree writes a CT from a chunk and its children.

	CFL::PcflReadForestFromFlo reads a CF from a flo and creates a CFL
	around the data.
***************************************************************************/
struct ECDF
	{
	short bo;
	short osk;
	CTG ctg;
	CHID chid;
	long cb;
	long ckid;
	ulong grfcrp;
	};
const BOM kbomEcdf = 0x5FFC0000L;


/***************************************************************************
	Combine the indicated chunk and its children into an embedded chunk.
	If pfil is pvNil, just computes the size.
***************************************************************************/
bool CFL::FWriteChunkTree(CTG ctg, CNO cno, PFIL pfilDst, FP fpDst, long *pcb)
{
	AssertThis(0);
	AssertNilOrPo(pfilDst, 0);
	AssertNilOrVarMem(pcb);
	CGE cge;
	ECDF ecdf;
	FLO floSrc, floDst;
	KID kid;
	CKI ckiPar;
	ulong grfcge;

	if (pvNil != pcb)
		*pcb = 0;

	cge.Init(this, ctg, cno);
	while (cge.FNextKid(&kid, &ckiPar, &grfcge, fcgeNil))
		{
		if (grfcge & fcgeError)
			goto LFail;

		if (!(grfcge & fcgePre))
			continue;

		AssertDo(FFindFlo(kid.cki.ctg, kid.cki.cno, &floSrc), 0);
		if (pvNil != pcb)
			*pcb += size(ECDF) + floSrc.cb;

		if (pvNil != pfilDst)
			{
			// write the data
			ecdf.bo = kboCur;
			ecdf.osk = koskCur;
			ecdf.ctg = kid.cki.ctg;
			ecdf.chid = (grfcge & fcgeRoot) ? chidNil : kid.chid;
			ecdf.cb = floSrc.cb;
			ecdf.ckid = Ckid(kid.cki.ctg, kid.cki.cno);
			ecdf.grfcrp = fcrpNil;
			if (FPacked(kid.cki.ctg, kid.cki.cno))
				ecdf.grfcrp |= fcrpPacked;
			if (FForest(kid.cki.ctg, kid.cki.cno))
				ecdf.grfcrp |= fcrpForest;

			if (!pfilDst->FWriteRgbSeq(&ecdf, size(ecdf), &fpDst))
				goto LFail;

			floDst.pfil = pfilDst;
			floDst.fp = fpDst;
			floDst.cb = floSrc.cb;
			if (!floSrc.FCopy(&floDst))
				{
LFail:
				TrashVar(pcb);
				return fFalse;
				}
			fpDst += floDst.cb;
			}
		}

	return fTrue;
}


/***************************************************************************
	Static method to read a serialized chunk stream (as written by a series
	of calls to FWriteChunkTree) and create a CFL around it. If fCopyData
	is false, we construct the CFL to use pointers to the data in the FLO.
	Otherwise, we copy the data to a new file.
***************************************************************************/
PCFL CFL::PcflReadForestFromFlo(PFLO pflo, bool fCopyData)
{
	AssertVarMem(pflo);
	AssertPo(pflo->pfil, 0);

	// embedded chunk stack descriptor
	struct ECSD
		{
		CTG ctg;
		CNO cno;
		long ckid;
		};

	PCFL pcfl;
	ECDF ecdf;
	ECSD ecsdT, ecsdCur;
	FP fpSrc, fpLimSrc;
	PGL pglecsd = pvNil;

	if (pvNil == (pglecsd = GL::PglNew(size(ECSD))))
		goto LFail;

	if ((pcfl = NewObj CFL()) == pvNil)
		goto LFail;

	if (pvNil == (pcfl->_pggcrp = GG::PggNew(size(CRP))))
		goto LFail;

	if (fCopyData)
		{
		if (pvNil == (pcfl->_csto.pfil = FIL::PfilCreateTemp()) ||
				!pcfl->_csto.pfil->FSetFpMac(size(CFP)))
			{
			goto LFail;
			}
		pcfl->_csto.fpMac = size(CFP);
		}
	else
		{
		pcfl->_csto.pfil = pflo->pfil;
		pcfl->_csto.pfil->AddRef();
		pcfl->_csto.fpMac = pflo->fp + pflo->cb;
		pcfl->_fInvalidMainFile = fTrue;
		pcfl->_fAddToExtra = fTrue;
		}

	AssertPo(pcfl, fcflFull | fcflGraph);

	ClearPb(&ecsdCur, size(ecsdCur));

	fpSrc = pflo->fp;
	fpLimSrc = pflo->fp + pflo->cb;
	for (;;)
		{
		if (fpSrc >= fpLimSrc)
			break;

		if (fpSrc > fpLimSrc + size(ecdf) ||
				!pflo->pfil->FReadRgbSeq(&ecdf, size(ecdf), &fpSrc))
			{
			goto LFail;
			}

		if (kboOther == ecdf.bo)
			SwapBytesBom(&ecdf, kbomEcdf);
		else if (kboCur != ecdf.bo)
			goto LFail;

		if (!FIn(ecdf.cb, 0, fpLimSrc - fpSrc + 1))
			goto LFail;

		if (fCopyData)
			{
			BLCK blck(pflo->pfil, fpSrc, ecdf.cb, ecdf.grfcrp & fcrpPacked);

			if (!pcfl->FAddBlck(&blck, ecdf.ctg, &ecsdT.cno))
				goto LFail;
			}
		else
			{
			CRP *qcrp;
			long icrp;

			pcfl->_GetUniqueCno(ecdf.ctg, &icrp, &ecsdT.cno);
			if (!pcfl->_FAdd(0, ecdf.ctg, ecsdT.cno, icrp, pvNil))
				goto LFail;

			qcrp = (CRP *)pcfl->_pggcrp->QvFixedGet(icrp);
			qcrp->fp = ecdf.cb > 0 ? fpSrc : 0;
			qcrp->SetCb(ecdf.cb);
			qcrp->AssignGrfcrp(ecdf.grfcrp, fcrpPacked | fcrpForest);
			}
		fpSrc += ecdf.cb;

		if (pglecsd->IvMac() > 0)
			{
			Assert(ecsdCur.ckid > 0, 0);

			// Make this a child
			if (!pcfl->FAdoptChild(ecsdCur.ctg, ecsdCur.cno,
					ecdf.ctg, ecsdT.cno, ecdf.chid))
				{
				goto LFail;
				}
			ecsdCur.ckid--;
			}

		if (ecdf.ckid > 0)
			{
			// This one has children, so we need to push the current ecsd and
			// make this the current one
			ecsdT.ctg = ecdf.ctg;
			ecsdT.ckid = ecdf.ckid;

			if (!pglecsd->FPush(&ecsdCur))
				goto LFail;

			ecsdCur = ecsdT;
			}
		else
			{
			// pop up while there are no more children and there's something
			// to pop.
			while (ecsdCur.ckid <= 0 && pglecsd->IvMac() > 0)
				pglecsd->FPop(&ecsdCur);
			}
		}

	if (pglecsd->IvMac() > 0 || ecsdCur.ckid != 0)
		{
LFail:
		// something failed or the data was bad
		ReleasePpo(&pcfl);
		}

	ReleasePpo(&pglecsd);
	AssertNilOrPo(pcfl, fcflFull | fcflGraph);

	return pcfl;
}


/***************************************************************************
	Return whether the chunk contains a forest of subchunks.
***************************************************************************/
bool CFL::FForest(CTG ctg, CNO cno)
{
	AssertThis(0);
	long icrp;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not there");
		return fFalse;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	Assert(qcrp->cki.ctg == ctg && qcrp->cki.cno == cno, 0);
	return FPure(qcrp->Grfcrp(fcrpForest));
}


/***************************************************************************
	Set or clear the "forest" flag.
***************************************************************************/
void CFL::SetForest(CTG ctg, CNO cno, bool fForest)
{
	AssertThis(0);
	long icrp;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not there");
		return;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	Assert(qcrp->cki.ctg == ctg && qcrp->cki.cno == cno, 0);
	if (FPure(qcrp->Grfcrp(fcrpForest)) != FPure(fForest))
		{
		if (fForest)
			qcrp->SetGrfcrp(fcrpForest);
		else
			qcrp->ClearGrfcrp(fcrpForest);
		_FSetRti(ctg, cno, rtiNil);
		}
}


/***************************************************************************
	Read the chunk's data as an embedded forest and construct a CFL around
	the data.
***************************************************************************/
PCFL CFL::PcflReadForest(CTG ctg, CNO cno, bool fCopyData)
{
	AssertThis(0);
	FLO flo;

	if (!FFindFlo(ctg, cno, &flo))
		return pvNil;

	if (FPacked(ctg, cno))
		{
		BLCK blck(&flo, fTrue);

		if (!blck.FUnpackData() || !blck.FGetFlo(&flo))
			return pvNil;
		fCopyData = fFalse;
		}

	return PcflReadForestFromFlo(&flo, fCopyData);
}


/***************************************************************************
	Static method: clear the marks for chunky files.
***************************************************************************/
void CFL::ClearMarks(void)
{
	PCFL pcfl;

	for (pcfl = _pcflFirst; pcfl != pvNil; pcfl = pcfl->PcflNext())
		{
		AssertPo(pcfl, 0);
		pcfl->_fMark = fFalse;
		}
}


/***************************************************************************
	Static method: close any chunky files that are unmarked and have 0
	open count.
***************************************************************************/
void CFL::CloseUnmarked(void)
{
	PCFL pcfl, pcflNext;

	for (pcfl = _pcflFirst; pcfl != pvNil; pcfl = pcflNext)
		{
		AssertPo(pcfl, 0);
		pcflNext = pcfl->PcflNext();
		if (!pcfl->_fMark && pcfl->_cactRef == 0)
			delete pcfl;
		}
}


/***************************************************************************
	Set the grfcfl options.  This sets or clears fcflTemp and only sets
	(never clears) fcflMark, fcflWriteEnable and fcflAddToExtra.  This
	can only fail if fcflWriteEnable is specified and we can't make
	the base file write enabled.
***************************************************************************/
bool CFL::FSetGrfcfl(ulong grfcfl, ulong grfcflMask)
{
	AssertThis(0);
	ulong grffil, grffilMask;

	grfcfl &= grfcflMask;
	grffil = _GrffilFromGrfcfl(grfcfl);
	grffilMask = _GrffilFromGrfcfl(grfcflMask);

	if ((grffil ^ _csto.pfil->GrffilCur()) & grffilMask)
		{
		// need to change some file properties
		if (_fInvalidMainFile || !_csto.pfil->FSetGrffil(grffil, grffilMask))
			return fFalse;
		}

	// If we're becoming write enabled and we haven't read the free map yet,
	// do so now.  Reading the free map is non-critical.
	if ((grfcfl & fcflWriteEnable) && _fFreeMapNotRead)
		_ReadFreeMap();

	// don't clear the mark field if it's already set
	if (grfcfl & fcflMark)
		_fMark = fTrue;

	// don't clear the _fAddToExtra field if it's already set
	if ((grfcfl & fcflAddToExtra) || ((grfcfl ^ grfcflMask) & fcflWriteEnable))
		_fAddToExtra = fTrue;

	if (grfcfl & fcflReadFromExtra)
		_fReadFromExtra = fTrue;

	return fTrue;
}


/***************************************************************************
	Return the level of error that we've encountered for this chunky file.
***************************************************************************/
long CFL::ElError(void)
{
	AssertThis(0);
	long elT;

	elT = _csto.pfil->ElError();
	if (pvNil != _cstoExtra.pfil)
		elT = LwMax(elT, _cstoExtra.pfil->ElError());
	return elT;
}


/***************************************************************************
	Make sure the el level on both files is at or below el.
***************************************************************************/
void CFL::ResetEl(long el)
{
	AssertThis(0);
	AssertIn(el, elNil, kcbMax);

	if (_csto.pfil->ElError() > el)
		_csto.pfil->SetEl(el);

	if (pvNil != _cstoExtra.pfil && _cstoExtra.pfil->ElError() > el)
		_cstoExtra.pfil->SetEl(el);
}


/***************************************************************************
	Decrement the open count.  If it is zero and the chunky file isn't
	marked, the file is closed.
***************************************************************************/
void CFL::Release(void)
{
	AssertBaseThis(0);
	if (_cactRef <= 0)
		{
		Bug("calling Release without an AddRef");
		return;
		}
	if (--_cactRef == 0 && !_fMark)
		delete this;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the chunky file.  If fcflFull, does some checking
	of the index.  If fcflGraph, does fcflFull and checks the graph structure
	for cycles.
***************************************************************************/
void CFL::AssertValid(ulong grfcfl)
{
	CFL_PAR::AssertValid(fobjAllocated);
	bool fFirstCrp;
	long cbTot = 0;
	long cbTotExtra = 0;
	CKI ckiOld;
	CKI ckiNew;
	KID kid, kidOld;
	bool fFirstKid;
	long icrp, icrpT;
	long ccrpRefTot = 0;
	long ckidTot = 0;
	long cbVar, cbRgch;
	CRP crp;
	long ikid;
	FP fpBase = _fInvalidMainFile ? 0 : size(CFP);

	Assert(!_fInvalidMainFile || _fAddToExtra, 0);

	AssertPo(_pggcrp, 0);
	AssertPo(_csto.pfil, 0);
	if (_csto.pglfsm != pvNil)
		{
		AssertPo(_csto.pglfsm, 0);
		Assert(!_fFreeMapNotRead, "free map never read, but exists!");
		}
	Assert(_csto.fpMac >= fpBase, "fpMac wrong");

	if (_cstoExtra.pfil != pvNil)
		{
		AssertPo(_cstoExtra.pfil, 0);
		if (_cstoExtra.pglfsm != pvNil)
			AssertPo(_cstoExtra.pglfsm, 0);
		}
	else
		Assert(_cstoExtra.pglfsm == pvNil, 0);

#ifndef CHUNK_BIG_INDEX
	AssertNilOrPo(_pglrtie, 0);
#endif //!CHUNK_BIG_INDEX

	if (!(grfcfl & (fcflFull | fcflGraph)))
		return;

	SuspendAssertValid();

	// Verify that the index and free map(s) are in sorted order.
	// Would be nice to verify ccrpRef and that crp's don't overlap,
	// but this is hard, so instead we verify that the sum of ccrpRef
	// values is correct and that the total length of all blocks is not
	// too big.
	fFirstCrp = fTrue;
	for (icrp = _pggcrp->IvMac(); icrp-- != 0; )
		{
		_pggcrp->GetFixed(icrp, &crp);
		cbVar = _pggcrp->Cb(icrp);
		cbRgch = crp.CbRgch(cbVar);

		AssertIn(crp.ckid, 0, kckidMax + 1);
		AssertIn(cbRgch, 0, kcbMaxDataStn + 1);

		// we use this in checking the graph structure
		Assert(!crp.Grfcrp(fcrpMarkT), "fcrpMarkT set");

		ckiNew = crp.cki;
		ccrpRefTot += crp.ccrpRef;
		ckidTot += crp.ckid;

#ifdef CHUNK_BIG_INDEX
		AssertIn(crp.rti, 0, _rtiLast + 1);
#endif //CHUNK_BIG_INDEX

		// assert that the file storage (fp,cb) is OK
		if (crp.Grfcrp(fcrpOnExtra))
			{
			Assert(_cstoExtra.pfil != pvNil, "fcrpOnExtra wrong");
			AssertIn(crp.fp, 0, _cstoExtra.fpMac);
			AssertIn(crp.Cb(), 1, _cstoExtra.fpMac - crp.fp + 1);
			cbTotExtra += crp.Cb();
			}
		else
			{
			AssertVar(
				crp.Cb() > 0 && (crp.fp >= fpBase) ||
				crp.Cb() == 0 && crp.fp == 0, "bad fp", &crp.fp);
			AssertVar(crp.fp <= _csto.fpMac, "bad fp", &crp.fp);
			AssertIn(crp.Cb(), 0, _csto.fpMac - crp.fp + 1);
			cbTot += crp.Cb();
			}

		//assert that this crp is in order
		AssertVar(fFirstCrp || ckiNew.ctg < ckiOld.ctg ||
			ckiNew.ctg == ckiOld.ctg && ckiNew.cno < ckiOld.cno,
			"crp not in order", &ckiNew);
		ckiOld = ckiNew;
		fFirstCrp = fFalse;

		fFirstKid = fTrue;
		for (ikid = 0; ikid < crp.ckid; ikid++)
			{
			_pggcrp->GetRgb(icrp, _BvKid(ikid), size(KID), &kid);
			AssertVar(_FFindCtgCno(kid.cki.ctg, kid.cki.cno, &icrpT),
				"child doesn't exist", &kid.cki);
			Assert(kid.cki.ctg != ckiOld.ctg || kid.cki.cno != ckiOld.cno,
				"chunk is child of itself!");

			Assert(fFirstKid || kidOld.chid < kid.chid ||
				kidOld.chid == kid.chid && kidOld.cki.ctg < kid.cki.ctg ||
				kidOld.chid == kid.chid && kidOld.cki.ctg == kid.cki.ctg &&
					kidOld.cki.cno < kid.cki.cno,
				"kid's not sorted or duplicate kid");
			kidOld = kid;
			fFirstKid = fFalse;
			}
		}
	Assert(ccrpRefTot == ckidTot, "ref counts messed up");
	Assert(cbTotExtra <= _cstoExtra.fpMac, "overlapping chunks on extra");
	Assert(cbTot <= _csto.fpMac - fpBase, "overlapping chunks on file");

	if (_csto.pglfsm != pvNil)
		{
		long ifsm;
		FSM fsm;
		FP fpOld;

		fpOld = _csto.fpMac;
		for (ifsm = _csto.pglfsm->IvMac(); ifsm-- != 0; )
			{
			_csto.pglfsm->Get(ifsm, &fsm);
			Assert(fsm.fp >= fpBase && fsm.fp < fpOld, "bad fp in fsm");
			Assert(fsm.cb + fsm.fp > fsm.fp && fsm.cb + fsm.fp < fpOld,
				"bad cb in fsm");
			fpOld = fsm.fp;
			cbTot += fsm.cb;
			}
		Assert(cbTot <= _csto.fpMac - fpBase, "too much free space");
		}
	if (_cstoExtra.pglfsm != pvNil)
		{
		long ifsm;
		FSM fsm;
		FP fpOld;

		fpOld = _cstoExtra.fpMac;
		for (ifsm = _cstoExtra.pglfsm->IvMac(); ifsm-- != 0; )
			{
			_cstoExtra.pglfsm->Get(ifsm, &fsm);
			Assert(fsm.fp >= 0 && fsm.fp < fpOld, "bad fp in fsm");
			Assert(fsm.cb + fsm.fp > fsm.fp && fsm.cb + fsm.fp < fpOld,
				"bad cb in fsm");
			fpOld = fsm.fp;
			cbTotExtra += fsm.cb;
			}
		Assert(cbTotExtra <= _cstoExtra.fpMac, "too much free space on extra");
		}

	if (grfcfl & fcflGraph)
		Assert(_TValidIndex() != tNo, "bad index");

#ifndef CHUNK_BIG_INDEX
	// verify that the _pglrtie is in sorted order and all the chunks exist
	if (pvNil != _pglrtie && _pglrtie->IvMac() > 0)
		{
		long irtie;
		RTIE rtie, rtiePrev;
		long icrp;

		irtie = _pglrtie->IvMac() - 1;
		_pglrtie->Get(irtie, &rtie);
		Assert(rtiNil != rtie.rti && _FFindCtgCno(rtie.ctg, rtie.cno, &icrp),
			"Bad RTIE");

		while (irtie-- > 0)
			{
			rtiePrev = rtie;
			_pglrtie->Get(irtie, &rtie);
			Assert(rtiNil != rtie.rti &&
				_FFindCtgCno(rtie.ctg, rtie.cno, &icrp), "Bad RTIE");
			Assert(rtie.ctg < rtiePrev.ctg ||
				rtie.ctg == rtiePrev.ctg && rtie.cno < rtiePrev.cno,
				"RTIE out of order");
			}
		}
#endif //!CHUNK_BIG_INDEX

	ResumeAssertValid();
}


/***************************************************************************
	Mark memory used by the CFL
***************************************************************************/
void CFL::MarkMem(void)
{
	AssertThis(0);
	CFL_PAR::MarkMem();
	MarkMemObj(_pggcrp);
	MarkMemObj(_csto.pglfsm);
	MarkMemObj(_cstoExtra.pglfsm);
#ifndef CHUNK_BIG_INDEX
	MarkMemObj(_pglrtie);
#endif //!CHUNK_BIG_INDEX
}
#endif //DEBUG


/***************************************************************************
	Checks for cycles and other consistency in the chunky index.  If
	we find something wrong, return tNo.  If a memory error occurs,
	return tMaybe.  If it's AOK, return tYes.
***************************************************************************/
bool CFL::_TValidIndex(void)
{
	//WARNING: this is called by a full CFL::AssertValid().
	long icrp, icrpT;
	CRP *qcrp;
	CGE cge;
	ulong grfcge, grfcgeIn;
	KID kid;
	long cbVar;
	long ccrpRefTot;
	long ckidTot;


	//first clear all fcrpMarkT fields
	ccrpRefTot = ckidTot = 0;
	for (icrp = _pggcrp->IvMac(); icrp-- != 0; )
		{
		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp, &cbVar);
		if (!FIn(qcrp->ckid, 0, cbVar / size(KID) + 1))
			return tNo;
		if (!FIn(qcrp->ccrpRef, 0, kckidMax + 1))
			return tNo;
		ccrpRefTot += qcrp->ccrpRef;
		ckidTot += qcrp->ckid;
		qcrp->ClearGrfcrp(fcrpMarkT);
		}
	if (ccrpRefTot != ckidTot)
		return tNo;

	SuspendAssertValid();
	SuspendCheckPointers();

	// now enumerate over root level graphs, marking descendents on the
	// pre-pass and unmarking on the post pass - this catches cycles
	// NOTE: we must do this before checking for orphan subgraphs, so we don't
	// end up in an infinite loop in the orphan check.
	for (icrp = _pggcrp->IvMac(); icrp-- != 0; )
		{
		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		if (qcrp->ccrpRef > 0 || qcrp->ckid == 0)
			continue;

		//this is not a child node and has children, so enum its subgraph
		cge.Init(this, qcrp->cki.ctg, qcrp->cki.cno);
		while (cge.FNextKid(&kid, pvNil, &grfcge, fcgeNil))
			{
			if (grfcge & fcgeError)
				{
				ResumeAssertValid();
				ResumeCheckPointers();
				return tMaybe;
				}
			if (!_FFindCtgCno(kid.cki.ctg, kid.cki.cno, &icrpT))
				{
				ResumeAssertValid();
				ResumeCheckPointers();
				return tNo;
				}
			qcrp = (CRP *)_pggcrp->QvFixedGet(icrpT);
			if (grfcge & fcgePre)
				{
				if (qcrp->Grfcrp(fcrpMarkT))
					{
					// has a cycle here
					ResumeAssertValid();
					ResumeCheckPointers();
					return tNo;
					}
				qcrp->SetGrfcrp(fcrpMarkT);
				}
			if (grfcge & fcgePost)
				{
				Assert(qcrp->Grfcrp(fcrpMarkT), "why isn't this marked?");
				qcrp->ClearGrfcrp(fcrpMarkT);
				}
			}
		}

	// now enumerate over root level graphs, marking all descendents
	// this will find orphan subgraphs and validate (ccrpRef > 0).
	for (icrp = _pggcrp->IvMac(); icrp-- != 0; )
		{
		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		if (qcrp->ccrpRef > 0 || qcrp->ckid == 0)
			continue;

		// this is not a child node and has some children, so enum its subgraph
		cge.Init(this, qcrp->cki.ctg, qcrp->cki.cno);
		grfcgeIn = fcgeNil;
		while (cge.FNextKid(&kid, pvNil, &grfcge, grfcgeIn))
			{
			grfcgeIn = fcgeNil;
			if (grfcge & fcgeError)
				{
				ResumeAssertValid();
				ResumeCheckPointers();
				return tMaybe;
				}
			if ((grfcge & fcgePre) && !(grfcge & fcgeRoot))
				{
				if (!_FFindCtgCno(kid.cki.ctg, kid.cki.cno, &icrpT))
					{
					ResumeAssertValid();
					ResumeCheckPointers();
					return tNo;
					}
				qcrp = (CRP *)_pggcrp->QvFixedGet(icrpT);
				if (qcrp->Grfcrp(fcrpMarkT))
					grfcgeIn = fcgeSkipToSib;
				else
					qcrp->SetGrfcrp(fcrpMarkT);
				}
			}
		}

	// make sure the fcrpMarkT fields are set iff (ccrpRef > 0)
	for (icrp = _pggcrp->IvMac(); icrp-- != 0; )
		{
		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		if ((qcrp->ccrpRef != 0) != FPure(qcrp->Grfcrp(fcrpMarkT)))
			{
			ResumeAssertValid();
			ResumeCheckPointers();
			return tNo;
			}
		qcrp->ClearGrfcrp(fcrpMarkT);
		}

	ResumeAssertValid();
	ResumeCheckPointers();
	return tYes;
}


/***************************************************************************
	Verifies that this is a chunky file and reads the index into memory.
	Sets the _fpFreeMap and _cbFreeMap fields and sets the _fFreeMapNotRead
	flag. The free map is read separately by _ReadFreeMap. This is to avoid
	hogging memory for the free map when we may never need it.
***************************************************************************/
bool CFL::_FReadIndex(void)
{
	AssertBaseThis(0);
	AssertPo(_csto.pfil, 0);
	Assert(!_fInvalidMainFile, 0);

	CFP cfp;
	FP fpMac;
	short bo;
	short osk;
	long cbVar;
	long cbRgch;
	long icrp, ccrp;
	long cbFixed;
	BOM bom;

	// used for old name stuff
	SZS szsName;
	STN stn;
	bool fOldNames;

	// used for old index stuff
	bool fOldIndex;

	Assert(_pggcrp == pvNil && _csto.pglfsm == pvNil &&
		_cstoExtra.pfil == pvNil && _cstoExtra.pglfsm == pvNil,
		"cfl has wrong non-nil entries");

	// verify that this is a chunky file
	if ((fpMac = _csto.pfil->FpMac()) < size(CFP))
		return fFalse;

	if (!_csto.pfil->FReadRgb(&cfp, size(cfp), 0))
		return fFalse;

	// check the magic number and byte order indicator
	if (cfp.lwMagic != klwMagicChunky ||
			cfp.bo != kboCur && cfp.bo != kboOther)
		{
		return fFalse;
		}

	if (cfp.bo == kboOther)
		SwapBytesBom(&cfp, kbomCfp);

	// check the version numbers
	if (!cfp.dver.FReadable(kcvnCur, kcvnMin))
		return fFalse;

	// if the file has old style chunk names, translate them
	fOldNames = cfp.dver._swCur < kcvnMinStnNames;

	// whether the index needs converted
	fOldIndex = cfp.dver._swCur < kcvnMinGrfcrp;

	// verify the fp's and cb's
	// the index and map should be last
	if (!FIn(cfp.fpMac, size(cfp), fpMac + 1) ||
		!FIn(cfp.fpIndex, size(cfp), cfp.fpMac + 1) ||
		!FIn(cfp.cbIndex, 1, cfp.fpMac - cfp.fpIndex + 1) ||
		cfp.fpMap != cfp.fpIndex + cfp.cbIndex ||
			cfp.fpMap + cfp.cbMap != cfp.fpMac)
		{
		return fFalse;
		}

	// read and validate the index
	if ((_pggcrp = GG::PggRead(_csto.pfil, cfp.fpIndex, cfp.cbIndex,
			&bo, &osk)) == pvNil)
		{
		return fFalse;
		}

	cbFixed = _pggcrp->CbFixed();
	if (cbFixed != size(CRPBG) && (fOldIndex || cbFixed != size(CRPSM)))
		return fFalse;

	// Clean the index
	AssertBomRglw(kbomKid, size(KID));
	_pggcrp->Lock();

	if (cbFixed == size(CRPBG))
		{
		// Big index
		CRPBG *pcrpbg;

		bom = (bo != kboCur) ?
			(fOldIndex ? kbomCrpbgBytes : kbomCrpbgGrfcrp) : bomNil;

		for (icrp = _pggcrp->IvMac(); icrp-- != 0; )
			{
			pcrpbg = (CRPBG *)_pggcrp->QvFixedGet(icrp, &cbVar);
#ifndef CHUNK_BIG_INDEX
			// make sure we can convert this CRP to a small index CRP
			if (pcrpbg->cb > kcbMaxCrpsm || pcrpbg->ckid > kckidMax ||
					pcrpbg->ccrpRef > kckidMax)
				{
				goto LBadFile;
				}
#endif //!CHUNK_BIG_INDEX
			if (!FIn(cbVar, 0, kcbMax))
				goto LBadFile;
			if (bomNil != bom)
				SwapBytesBom(pcrpbg, bom);
			if (!FIn(pcrpbg->ckid, 0, cbVar / size(KID) + 1) ||
					(cbRgch = pcrpbg->CbRgch(cbVar)) > kcbMaxDataStn)
				{
				goto LBadFile;
				}
			if (bomNil != bom && pcrpbg->ckid > 0)
				{
				SwapBytesRglw(_pggcrp->QvGet(icrp),
					pcrpbg->ckid * (size(KID) / size(long)));
				}
			pcrpbg->rti = rtiNil;

			if (fOldIndex)
				{
				ulong grfcrp = fcrpNil;

				if (pcrpbg->fLoner)
					grfcrp |= fcrpLoner;
				if (pcrpbg->fPacked)
					grfcrp |= fcrpPacked;

				pcrpbg->grfcrp = grfcrp;
				}
			else
				pcrpbg->ClearGrfcrp(fcrpOnExtra);

			if (pcrpbg->ccrpRef == 0)
				pcrpbg->SetGrfcrp(fcrpLoner);

			AssertIn(cbRgch, 0, kcbMaxDataStn + 1);
			if (fOldNames && cbRgch > 0)
				{
				// translate the name
				long bvRgch = pcrpbg->BvRgch();

				if (cbRgch < size(szsName))
					{
					long cbNew;

					CopyPb(PvAddBv(_pggcrp->QvGet(icrp), bvRgch), szsName,
						cbRgch);
					szsName[cbRgch] = 0;
					stn.SetSzs(szsName);
					cbNew = stn.CbData();
					if (cbRgch != cbNew)
						{
						_pggcrp->Unlock();
						if (cbRgch < cbNew)
							{
							if (!_pggcrp->FInsertRgb(icrp, bvRgch,
									cbNew - cbRgch, pvNil))
								{
								goto LNukeName;
								}
							}
						else
							_pggcrp->DeleteRgb(icrp, bvRgch, cbRgch - cbNew);
						_pggcrp->Lock();
						}
					stn.GetData(PvAddBv(_pggcrp->QvGet(icrp), bvRgch));
					}
				else
					{
					// just nuke the name
					_pggcrp->Unlock();
LNukeName:
					_pggcrp->DeleteRgb(icrp, bvRgch, cbRgch);
					_pggcrp->Lock();
					}
				}
			}
		}
	else
		{
		// Small index
		Assert(size(CRPSM) == cbFixed, 0);
		CRPSM *pcrpsm;

		bom = (bo != kboCur) ? kbomCrpsm : bomNil;
		for (icrp = _pggcrp->IvMac(); icrp-- != 0; )
			{
			pcrpsm = (CRPSM *)_pggcrp->QvFixedGet(icrp, &cbVar);
			if (!FIn(cbVar, 0, kcbMax))
				goto LBadFile;
			if (bomNil != bom)
				SwapBytesBom(pcrpsm, bom);
			if (!FIn(pcrpsm->ckid, 0, cbVar / size(KID) + 1) ||
					(cbRgch = pcrpsm->CbRgch(cbVar)) > kcbMaxDataStn)
				{
				goto LBadFile;
				}
			if (pcrpsm->ckid > 0 && bomNil != bom)
				{
				SwapBytesRglw(_pggcrp->QvGet(icrp),
					pcrpsm->ckid * (size(KID) / size(long)));
				}

			pcrpsm->ClearGrfcrp(fcrpOnExtra);
			if (pcrpsm->ccrpRef == 0 && !pcrpsm->Grfcrp(fcrpLoner))
				{
LBadFile:
				_pggcrp->Unlock();
				Bug("corrupt crp");
				ReleasePpo(&_pggcrp);
				return fFalse;
				}
			}
		}

	if (size(CRP) != cbFixed)
		{
		// need to convert the index (from big to small or small to big)
		PGG pggcrp;
		CRPOTH *pcrpOld;
		CRP crp;

		if (pvNil == (pggcrp = GG::PggNew(size(CRP), _pggcrp->IvMac())))
			goto LFail;

		for (ccrp = _pggcrp->IvMac(), icrp = 0; icrp < ccrp; icrp++)
			{
			pcrpOld = (CRPOTH *)_pggcrp->QvFixedGet(icrp, &cbVar);
			crp.cki = pcrpOld->cki;
			crp.fp = pcrpOld->fp;
			crp.SetCb(pcrpOld->Cb());
			crp.ckid = (CKID)pcrpOld->ckid;
			crp.ccrpRef = (CKID)pcrpOld->ccrpRef;
			crp.AssignGrfcrp(pcrpOld->Grfcrp());
#ifdef CHUNK_BIG_INDEX
			crp.rti = rtiNil;
#endif //CHUNK_BIG_INDEX

			if (!pggcrp->FInsert(icrp,
					_pggcrp->Cb(icrp), _pggcrp->QvGet(icrp), &crp))
				{
				ReleasePpo(&pggcrp);
LFail:
				_pggcrp->Unlock();
				return fFalse;
				}
			}
		_pggcrp->Unlock();
		ReleasePpo(&_pggcrp);

		_pggcrp = pggcrp;
		}
	else
		_pggcrp->Unlock();

	_cbFreeMap = cfp.cbMap;
	_fpFreeMap = cfp.fpMap;
	_fFreeMapNotRead = cfp.cbMap > 0;

	_csto.fpMac = cfp.fpIndex;
	return fTrue;
}


/***************************************************************************
	Assert that the free map hasn't been read (and doesn't exist).  Read
	the free map from the file.  This _cannot_ be called after chunk
	data has been added to the main file (as opposed to the extra file).
	This _cannot_ be called before _FReadIndex has been called.
***************************************************************************/
void CFL::_ReadFreeMap(void)
{
	AssertBaseThis(0);
	Assert(_fFreeMapNotRead, "free map already read");
	Assert(!_fInvalidMainFile, 0);

	short bo;
	short osk;
	long cfsm;

	//clear this even if reading the free map fails - so we don't try to
	//read again
	_fFreeMapNotRead = fFalse;

	if (_csto.pglfsm != pvNil)
		{
		Bug("free map already exists");
		return;
		}

	if (_cbFreeMap > 0)
		{
		if ((_csto.pglfsm = GL::PglRead(_csto.pfil, _fpFreeMap, _cbFreeMap,
				&bo, &osk)) == pvNil ||
			_csto.pglfsm->CbEntry() != size(FSM))
			{
			// it failed, but so what
			ReleasePpo(&_csto.pglfsm);
			return;
			}
		// swap bytes
		AssertBomRglw(kbomFsm, size(FSM));
		if (bo != kboCur && (cfsm = _csto.pglfsm->IvMac()) > 0)
			{
			SwapBytesRglw(_csto.pglfsm->QvGet(0),
				cfsm * (size(FSM) / size(long)));
			}
		}
}


/***************************************************************************
	If we have don't have write permission or there's an extra file,
	write out a new file and do the rename stuff.  If not, just write
	the index and free map.
***************************************************************************/
bool CFL::FSave(CTG ctgCreator, FNI *pfni)
{
	AssertThis(fcflFull | fcflGraph);
	AssertNilOrPo(pfni, ffniFile);

	FNI fni;
	FLO floSrc, floDst;
	long ccrp, icrp;
	CRP *qcrp;
	PFIL pfilOld;

	if (_fInvalidMainFile)
		{
		if (pvNil == pfni)
			{
			Bug("can't save a CFL that has no file attached!");
			return fFalse;
			}
		fni = *pfni;
		}
	else
		{
		GetFni(&fni);
		if (pfni != pvNil)
			{
			if (pfni->FEqual(&fni))
				pfni = pvNil;
			else
				fni = *pfni;
			}
		}

	if (!_fAddToExtra && _cstoExtra.pfil == pvNil && pfni == pvNil)
		{
		// just write the index
		Assert(!_fFreeMapNotRead, "why hasn't the free map been read?");
		if (!_FWriteIndex(ctgCreator))
			goto LError;
		return fTrue;
		}

	// get a temp name in the same directory as the target
	if ((floDst.pfil = FIL::PfilCreateTemp(&fni)) == pvNil)
		goto LError;
	if (!floDst.pfil->FSetFpMac(size(CFP)))
		goto LFail;

	floDst.fp = size(CFP);
	ccrp = _pggcrp->IvMac();
	for (icrp = 0; icrp < ccrp; icrp++)
		{
		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		floSrc.pfil = qcrp->Grfcrp(fcrpOnExtra) ? _cstoExtra.pfil : _csto.pfil;
		floSrc.fp = qcrp->fp;
		floSrc.cb = floDst.cb = qcrp->Cb();

		if (floSrc.cb > 0 && !floSrc.FCopy(&floDst))
			{
LFail:
			Assert(floDst.pfil->FTemp(), "file not a temp");
			ReleasePpo(&floDst.pfil);
			goto LError;
			}
		floDst.fp += floDst.cb;
		}

	// All the data has been copied.  Update the index to point to the new file.
	floSrc.fp = size(CFP);
	for (icrp = 0; icrp < ccrp; icrp++)
		{
		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		qcrp->ClearGrfcrp(fcrpOnExtra);
		qcrp->fp = qcrp->Cb() > 0 ? floSrc.fp : 0;
		floSrc.fp += qcrp->Cb();
		}
	Assert(floSrc.fp == floDst.fp, "what happened? - file messed up");

	// update the csto's and write the index
	pfilOld = _csto.pfil;
	ReleasePpo(&_csto.pglfsm);
	_fFreeMapNotRead = fFalse;
	_csto.pfil = floDst.pfil;
	_csto.fpMac = floDst.fp;
	_csto.pfil->SetTemp(_fInvalidMainFile || pfilOld->FTemp());

	if (_cstoExtra.pfil != pvNil)
		{
		_cstoExtra.fpMac = 0;
		ReleasePpo(&_cstoExtra.pfil);
		ReleasePpo(&_cstoExtra.pglfsm);
		}

	// write the index
	if (!_FWriteIndex(ctgCreator))
		goto LIndexFail;

	if (pfni != pvNil)
		{
		// delete any existing file with this name, then rename our output
		// file to the given name
		if (pfni->TExists() != tNo)
			pfni->FDelete();
		if (!_csto.pfil->FRename(pfni))
			goto LIndexFail;
		_fInvalidMainFile = fFalse;
		}
	else if (!_csto.pfil->FSwapNames(pfilOld))
		{
LIndexFail:
		if (_fInvalidMainFile)
			{
			// we can just use the new file now.
			ReleasePpo(&pfilOld); // release our claim on the old file
			}
		else
			{
			// restore the original csto and make floDst.pfil the extra file
			_csto.pfil = pfilOld;
			_csto.fpMac = size(CFP);
			for (icrp = 0; icrp < ccrp; icrp++)
				{
				qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
				if (qcrp->Cb() > 0)
					qcrp->SetGrfcrp(fcrpOnExtra);
				}
			_cstoExtra.pfil = floDst.pfil;
			floDst.pfil->SetTemp(fTrue);
			_cstoExtra.fpMac = floDst.fp;
			}
LError:
		PushErc(ercCflSave);
		AssertThis(fcflFull | fcflGraph);
		return fFalse;
		}

	// everything worked
	if (pfni == pvNil)
		pfilOld->SetTemp(fTrue);

	ReleasePpo(&pfilOld); // release our claim on the old file
	AssertThis(fcflFull | fcflGraph);
	return fTrue;
}


/***************************************************************************
	Write the chunky index and free map to the end of the file.
***************************************************************************/
bool CFL::_FWriteIndex(CTG ctgCreator)
{
	AssertBaseThis(0);
	AssertPo(_pggcrp, 0);
	AssertPo(_csto.pfil, 0);

	CFP cfp;
	BLCK blck;

	ClearPb(&cfp, size(cfp));
	cfp.lwMagic = klwMagicChunky;
	cfp.ctgCreator = ctgCreator;
	cfp.dver.Set(kcvnCur, kcvnBack);
	cfp.bo = kboCur;
	cfp.osk = koskCur;

	blck.Set(_csto.pfil, cfp.fpIndex = _csto.fpMac,
		cfp.cbIndex = _pggcrp->CbOnFile());
	if (!_pggcrp->FWrite(&blck))
		return fFalse;
	cfp.fpMap = cfp.fpIndex + cfp.cbIndex;
	if (_csto.pglfsm != pvNil)
		{
		AssertDo(blck.FMoveMin(cfp.cbIndex), 0);
		AssertDo(blck.FMoveLim(cfp.cbMap = _csto.pglfsm->CbOnFile()), 0);
		if (!_csto.pglfsm->FWrite(&blck))
			return fFalse;
		}
	else
		cfp.cbMap = 0;

	cfp.fpMac = cfp.fpMap + cfp.cbMap;
	return _csto.pfil->FWriteRgb(&cfp, size(cfp), 0);
}


/***************************************************************************
	Save a copy of the chunky file out to *pfni.  The CFL and its FIL
	are untouched.
***************************************************************************/
bool CFL::FSaveACopy(CTG ctgCreator, FNI *pfni)
{
	AssertThis(fcflFull | fcflGraph);
	AssertPo(pfni, ffniFile);
	PCFL pcflDst;
	long icrp, ccrp;
	CRP *pcrp;
	CRP crp;
	FLO floSrc, floDst;

	if (pvNil == (pcflDst = CFL::PcflCreate(pfni, fcflWriteEnable)))
		goto LError;

	// initialize the destination FLO.
	floDst.pfil = pcflDst->_csto.pfil;
	floDst.fp = size(CFP);

	// need to lock the _pggcrp for the FInsert operations below
	ccrp = _pggcrp->IvMac();
	_pggcrp->Lock();

	for (icrp = 0; icrp < ccrp; icrp++, floDst.fp += floDst.cb)
		{
		pcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		crp = *pcrp;

		// get the source and destination FLOs
		floSrc.pfil = pcrp->Grfcrp(fcrpOnExtra) ? _cstoExtra.pfil : _csto.pfil;
		floSrc.fp = pcrp->fp;
		floDst.cb = floSrc.cb = pcrp->Cb();

		// copy the data
		if (!floSrc.FCopy(&floDst))
			{
			_pggcrp->Unlock();
			goto LFail;
			}

		// create the index entry - the only things that change are the
		// (fp, cb) and the fcrpOnExtra flag.
		crp.fp = floDst.fp;
		crp.SetCb(floDst.cb);
		crp.ClearGrfcrp(fcrpOnExtra);
		if (!pcflDst->_pggcrp->FInsert(icrp, _pggcrp->Cb(icrp),
				_pggcrp->QvGet(icrp), &crp))
			{
			_pggcrp->Unlock();
			goto LFail;
			}
		}
	_pggcrp->Unlock();

	// set the fpMac of the destination CFL
	pcflDst->_csto.fpMac = floDst.fp;

	if (!pcflDst->FSave(ctgCreator))
		{
LFail:
		pcflDst->SetTemp(fTrue);
		ReleasePpo(&pcflDst);
LError:
		PushErc(ercCflSaveCopy);
		return fFalse;
		}

	ReleasePpo(&pcflDst);
	return fTrue;
}


/***************************************************************************
	Return whether the chunk's data is on the extra file.
***************************************************************************/
bool CFL::FOnExtra(CTG ctg, CNO cno)
{
	AssertThis(0);
	long icrp;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		return fFalse;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	return FPure(qcrp->Grfcrp(fcrpOnExtra));
}


/***************************************************************************
	Make sure the given chunk's data is on the extra file. Fails iff the
	chunk doesn't exist or copying the data failed.
***************************************************************************/
bool CFL::FEnsureOnExtra(CTG ctg, CNO cno)
{
	AssertThis(0);
	long icrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		return fFalse;

	return _FEnsureOnExtra(icrp);
}


/***************************************************************************
	Make sure the given chunk's data is on the extra file. Optionally get
	the flo for the data.
***************************************************************************/
bool CFL::_FEnsureOnExtra(long icrp, FLO *pflo)
{
	AssertThis(0);
	AssertNilOrVarMem(pflo);

	CRP *qcrp;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	if (qcrp->Grfcrp(fcrpOnExtra) || qcrp->Cb() == 0)
		{
		if (pvNil != pflo)
			{
			pflo->pfil = _cstoExtra.pfil;
			pflo->fp = qcrp->fp;
			pflo->cb = qcrp->Cb();
			}
		}
	else
		{
		FLO floSrc, floDst;

#ifdef CHUNK_STATS
		if (vfDumpChunkRequests)
			{
			STN stn;

			qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
			stn.FFormatSz(
				PszLit("Cache: '%f', 0x%08x, fp = 0x%08x, cb = 0x%08x"),
				qcrp->cki.ctg, qcrp->cki.cno, qcrp->fp, qcrp->Cb());
			DumpStn(&stn, _csto.pfil);
			}
#endif //CHUNK_STATS

		floSrc.pfil = _csto.pfil;
		floSrc.fp = qcrp->fp;
		floSrc.cb = qcrp->Cb();

		if (!_FAllocFlo(floSrc.cb, &floDst, fTrue))
			return fFalse;
		if (!floSrc.FCopy(&floDst))
			{
			_FreeFpCb(fTrue, floDst.fp, floDst.cb);
			return fFalse;
			}

		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		qcrp->SetGrfcrp(fcrpOnExtra);
		qcrp->fp = floDst.fp;
		if (pvNil != pflo)
			*pflo = floDst;
		}

#ifdef CHUNK_STATS
	if (pvNil != pflo && vfDumpChunkRequests)
		{
		STN stn;

		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		stn.FFormatSz(
			PszLit("Fetch from extra: '%f', 0x%08x, fp = 0x%08x, cb = 0x%08x"),
			qcrp->cki.ctg, qcrp->cki.cno, qcrp->fp, qcrp->Cb());
		DumpStn(&stn, _csto.pfil);
		}
#endif //CHUNK_STATS

	return fTrue;
}


/***************************************************************************
	Get the FLO from the chunk.
***************************************************************************/
void CFL::_GetFlo(long icrp, PFLO pflo)
{
	AssertThis(0);
	AssertVarMem(pflo);

	if (!_fReadFromExtra || !_FEnsureOnExtra(icrp, pflo))
		{
		CRP *qcrp;

		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		pflo->pfil = qcrp->Grfcrp(fcrpOnExtra) ? _cstoExtra.pfil : _csto.pfil;
		pflo->fp = qcrp->fp;
		pflo->cb = qcrp->Cb();

#ifdef CHUNK_STATS
		if (vfDumpChunkRequests)
			{
			STN stn;

			qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
			stn.FFormatSz(
				PszLit("Fetch from %z: '%f', 0x%08x, fp = 0x%08x, cb = 0x%08x"),
				qcrp->Grfcrp(fcrpOnExtra) ? PszLit("extra") : PszLit("main"),
				qcrp->cki.ctg, qcrp->cki.cno, qcrp->fp, qcrp->Cb());
			DumpStn(&stn, _csto.pfil);
			}
#endif //CHUNK_STATS
		}
	AssertPo(pflo->pfil, 0);
}


/***************************************************************************
	Get the BLCK from the chunk.
***************************************************************************/
void CFL::_GetBlck(long icrp, PBLCK pblck)
{
	AssertThis(0);
	AssertPo(pblck, 0);

	FLO flo;
	CRP *qcrp;

	_GetFlo(icrp, &flo);
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	pblck->Set(&flo, qcrp->Grfcrp(fcrpPacked));
}


/***************************************************************************
	Map the (ctg, cno) to a BLCK.
***************************************************************************/
bool CFL::FFind(CTG ctg, CNO cno, BLCK *pblck)
{
	AssertThis(0);
	AssertNilOrPo(pblck, 0);

	long icrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		if (pvNil != pblck)
			pblck->Free();
		return fFalse;
		}

	if (pvNil != pblck)
		_GetBlck(icrp, pblck);

	return fTrue;
}


/***************************************************************************
	Map the (ctg, cno) to a pflo.
***************************************************************************/
bool CFL::FFindFlo(CTG ctg, CNO cno, PFLO pflo)
{
	AssertThis(0);
	AssertVarMem(pflo);

	long icrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		TrashVar(pflo);
		return fFalse;
		}

	_GetFlo(icrp, pflo);
	return fTrue;
}


/***************************************************************************
	Reads the chunk (if it exists) into an hq.  Returns false if the
	chunk doesn't exist or something failed, in which case *phq is set
	to hqNil.  Doesn't unpack the data if it's packed.
***************************************************************************/
bool CFL::FReadHq(CTG ctg, CNO cno, HQ *phq)
{
	AssertThis(0);
	AssertVarMem(phq);
	BLCK blck;

	*phq = hqNil;	//in case FFind fails
	return FFind(ctg, cno, &blck) && blck.FReadHq(phq, fTrue);
}


/***************************************************************************
	Make sure the packed flag is set or clear according to fPacked.
	This doesn't affect the data at all.
***************************************************************************/
void CFL::SetPacked(CTG ctg, CNO cno, bool fPacked)
{
	AssertThis(0);
	long icrp;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not there");
		return;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	Assert(qcrp->cki.ctg == ctg && qcrp->cki.cno == cno, 0);
	if (FPure(qcrp->Grfcrp(fcrpPacked)) != FPure(fPacked))
		{
		if (fPacked)
			qcrp->SetGrfcrp(fcrpPacked);
		else
			qcrp->ClearGrfcrp(fcrpPacked);
		_FSetRti(ctg, cno, rtiNil);
		}
}


/***************************************************************************
	Return the value of the packed flag.
***************************************************************************/
bool CFL::FPacked(CTG ctg, CNO cno)
{
	AssertThis(0);
	long icrp;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not there");
		return fFalse;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	Assert(qcrp->cki.ctg == ctg && qcrp->cki.cno == cno, 0);
	return FPure(qcrp->Grfcrp(fcrpPacked));
}


/***************************************************************************
	If the data for the chunk is packed, unpack it.
***************************************************************************/
bool CFL::FUnpackData(CTG ctg, CNO cno)
{
	AssertThis(0);
	BLCK blck;

	if (!FPacked(ctg, cno))
		return fTrue;

	AssertDo(FFind(ctg, cno, &blck), 0);
	if (!blck.FUnpackData())
		return fFalse;
	return FPutBlck(&blck, ctg, cno);
}


/***************************************************************************
	If the data isn't already packed, pack it.
***************************************************************************/
bool CFL::FPackData(CTG ctg, CNO cno)
{
	AssertThis(0);
	BLCK blck;

	if (FPacked(ctg, cno))
		return fTrue;

	AssertDo(FFind(ctg, cno, &blck), 0);
	if (!blck.FPackData())
		return fFalse;
	return FPutBlck(&blck, ctg, cno);
}


/***************************************************************************
	Create the extra file.  Note: the extra file doesn't have a CFP -
	just raw data.
***************************************************************************/
bool CFL::_FCreateExtra(void)
{
	AssertBaseThis(0);
	Assert(_cstoExtra.pfil == pvNil, 0);
	Assert(_cstoExtra.pglfsm == pvNil, 0);

	if (pvNil == (_cstoExtra.pfil = FIL::PfilCreateTemp()))
		return fFalse;

	_cstoExtra.fpMac = 0;
	return fTrue;
}


/***************************************************************************
	Find a place to put a block the given size.
***************************************************************************/
bool CFL::_FAllocFlo(long cb, PFLO pflo, bool fForceOnExtra)
{
	AssertBaseThis(0);
	AssertIn(cb, 0, kcbMax);
	AssertVarMem(pflo);

	CSTO *pcsto;
	long cfsm, ifsm;
	FSM *qfsm;

	if (cb > kcbMaxCrp)
		{
		Bug("Requested FLO too big");
		return fFalse;
		}

	if ((fForceOnExtra || _fAddToExtra) && cb > 0)
		{
		pcsto = &_cstoExtra;
		if (pcsto->pfil == pvNil && !_FCreateExtra())
			{
			TrashVar(pflo);
			return fFalse;
			}
		}
	else
		{
		Assert(!_fFreeMapNotRead, "free map not read yet");
		pcsto = &_csto;
		}

	// set the file and cb - just need to find an fp
	AssertPo(pcsto->pfil, 0);
	pflo->pfil = pcsto->pfil;
	pflo->cb = cb;

	if (cb <= 0)
		{
		pflo->fp = 0;
		pflo->cb = 0;	//for safety
		return fTrue;
		}

	// look for a free spot in the free space map
	if (pcsto->pglfsm != pvNil && (cfsm = pcsto->pglfsm->IvMac()) > 0)
		{
		qfsm = (FSM *)pcsto->pglfsm->QvGet(0);
		for (ifsm = 0; ifsm < cfsm; ifsm++, qfsm++)
			{
			if (qfsm->cb >= cb)
				{
				// can put it here
				pflo->fp = qfsm->fp;
				if (qfsm->cb == cb)
					pcsto->pglfsm->Delete(ifsm);
				else
					{
					qfsm->cb -= cb;
					qfsm->fp += cb;
					}
				return fTrue;
				}
			}
		}

	// put it at the end of the file
	if (pcsto->fpMac + cb > pcsto->pfil->FpMac() &&
			!pcsto->pfil->FSetFpMac(pcsto->fpMac + cb))
		{
		TrashVar(pflo);
		return fFalse;
		}

	pflo->fp = pcsto->fpMac;
	pcsto->fpMac += cb;
	return fTrue;
}


/***************************************************************************
	Look for the (ctg, cno) pair.  Fills *picrp with where it should be.
	Returns whether or not it was found.  Assumes the _pggcrp is sorted
	by (ctg, cno).  Does a binary search.
***************************************************************************/
bool CFL::_FFindCtgCno(CTG ctg, CNO cno, long *picrp)
{
	//WARNING:  this is called by CFL::AssertValid, so be careful about
	//asserting stuff in here
	AssertBaseThis(0);
	AssertVarMem(picrp);
	AssertPo(_pggcrp, 0);

	long ccrp, icrpMin, icrpLim, icrp;
	CKI cki;

	if ((ccrp = _pggcrp->IvMac()) == 0)
		{
		*picrp = 0;
		return fFalse;
		}

	for (icrpMin = 0, icrpLim = ccrp; icrpMin < icrpLim; )
		{
		icrp = (icrpMin + icrpLim) / 2;
		cki = ((CRP *)_pggcrp->QvFixedGet(icrp))->cki;

		if (cki.ctg < ctg)
			icrpMin = icrp + 1;
		else if (cki.ctg > ctg)
			icrpLim = icrp;
		else if (cki.cno < cno)
			icrpMin = icrp + 1;
		else if (cki.cno > cno)
			icrpLim = icrp;
		else
			{
			*picrp = icrp;
			return fTrue;
			}
		}

	*picrp = icrpMin;
	return fFalse;
}


/***************************************************************************
	Find an unused cno for the given ctg.  Fill in *picrp and *pcno.
***************************************************************************/
void CFL::_GetUniqueCno(CTG ctg, long *picrp, CNO *pcno)
{
	AssertBaseThis(0);
	AssertVarMem(picrp);
	AssertVarMem(pcno);

	long icrp, ccrp;
	CNO cno;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, 0, picrp))
		{
		*pcno = 0;
		return;
		}

	// 0 already exists so do a linear search for the first useable slot
	ccrp = _pggcrp->IvMac();
	for (icrp = *picrp + 1, cno = 1; icrp < ccrp; icrp++, cno++)
		{
		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		if (qcrp->cki.ctg != ctg || qcrp->cki.cno != cno)
			break;
		}
	*picrp = icrp;
	*pcno = cno;
}


/***************************************************************************
	Add a new chunk.
***************************************************************************/
bool CFL::FAdd(long cb, CTG ctg, CNO *pcno, PBLCK pblck)
{
	AssertThis(0);
	AssertIn(cb, 0, kcbMax);
	AssertVarMem(pcno);
	AssertNilOrPo(pblck, 0);
	long icrp;

	_GetUniqueCno(ctg, &icrp, pcno);
	if (!_FAdd(cb, ctg, *pcno, icrp, pblck))
		{
		TrashVar(pcno);
		AssertThis(0);
		return fFalse;
		}
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Add a new chunk and write the pv to it.
***************************************************************************/
bool CFL::FAddPv(void *pv, long cb, CTG ctg, CNO *pcno)
{
	AssertThis(0);
	AssertVarMem(pcno);
	AssertIn(cb, 0, kcbMax);
	AssertPvCb(pv, cb);
	BLCK blck;

	if (!FAdd(cb, ctg, pcno, &blck))
		return fFalse;
	if (!blck.FWrite(pv))
		{
		Delete(ctg, *pcno);
		TrashVar(pcno);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Add a new chunk and write the hq to it.
***************************************************************************/
bool CFL::FAddHq(HQ hq, CTG ctg, CNO *pcno)
{
	AssertThis(0);
	AssertVarMem(pcno);
	AssertHq(hq);

	BLCK blck;
	long cb = CbOfHq(hq);

	if (!FAdd(cb, ctg, pcno, &blck))
		return fFalse;
	if (!blck.FWriteHq(hq, 0))
		{
		Delete(ctg, *pcno);
		TrashVar(pcno);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Add a new chunk and write the block to it.
***************************************************************************/
bool CFL::FAddBlck(PBLCK pblckSrc, CTG ctg, CNO *pcno)
{
	AssertThis(0);
	AssertPo(pblckSrc, 0);
	AssertVarMem(pcno);

	BLCK blck;
	long cb = pblckSrc->Cb(fTrue);

	if (!FAdd(cb, ctg, pcno, &blck))
		return fFalse;
	if (!pblckSrc->FWriteToBlck(&blck, fTrue))
		{
		Delete(ctg, *pcno);
		TrashVar(pcno);
		return fFalse;
		}
	if (pblckSrc->FPacked())
		SetPacked(ctg, *pcno, fTrue);

	return fTrue;
}


/***************************************************************************
	Adds the chunk and makes it a child of (ctgPar, cnoPar). The loner flag
	of the new chunk will be clear.
***************************************************************************/
bool CFL::FAddChild(CTG ctgPar, CNO cnoPar, CHID chid,
	long cb, CTG ctg, CNO *pcno, PBLCK pblck)
{
	AssertThis(0);
	AssertVarMem(pcno);
	AssertNilOrPo(pblck, 0);

	if (!FAdd(cb, ctg, pcno, pblck))
		return fFalse;
	if (!FAdoptChild(ctgPar, cnoPar, ctg, *pcno, chid, fTrue))
		{
		Delete(ctg, *pcno);
		TrashVar(pcno);
		if (pvNil != pblck)
			pblck->Free();
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Adds the chunk and makes it a child of (ctgPar, cnoPar).  The child's
	loner flag will be clear.
***************************************************************************/
bool CFL::FAddChildPv(CTG ctgPar, CNO cnoPar, CHID chid,
	void *pv, long cb, CTG ctg, CNO *pcno)
{
	if (!FAddPv(pv, cb, ctg, pcno))
		return fFalse;
	if (!FAdoptChild(ctgPar, cnoPar, ctg, *pcno, chid, fTrue))
		{
		Delete(ctg, *pcno);
		TrashVar(pcno);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Adds the chunk and makes it a child of (ctgPar, cnoPar).  The child's
	loner flag will be clear.
***************************************************************************/
bool CFL::FAddChildHq(CTG ctgPar, CNO cnoPar, CHID chid,
	HQ hq, CTG ctg, CNO *pcno)
{
	if (!FAddHq(hq, ctg, pcno))
		return fFalse;
	if (!FAdoptChild(ctgPar, cnoPar, ctg, *pcno, chid, fTrue))
		{
		Delete(ctg, *pcno);
		TrashVar(pcno);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Low level add.  Sets the loner flag.
***************************************************************************/
bool CFL::_FAdd(long cb, CTG ctg, CNO cno, long icrp, PBLCK pblck)
{
	AssertBaseThis(0);
	AssertIn(cb, 0, kcbMax);
	AssertNilOrPo(pblck, 0);

	CRP *qcrp;
	FLO flo;

	if (!_pggcrp->FInsert(icrp, 0))
		{
		if (pvNil != pblck)
			pblck->Free();
		return fFalse;
		}

	if (!_FAllocFlo(cb, &flo))
		{
		_pggcrp->Delete(icrp);
		AssertThis(0);
		if (pvNil != pblck)
			pblck->Free();
		return fFalse;
		}
	Assert(flo.pfil == _csto.pfil || flo.pfil == _cstoExtra.pfil, 0);

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	ClearPb(qcrp, size(CRP));
	qcrp->cki.ctg = ctg;
	qcrp->cki.cno = cno;
	qcrp->fp = flo.fp;
	qcrp->SetCb(flo.cb);
	if (flo.pfil == _cstoExtra.pfil)
		qcrp->SetGrfcrp(fcrpOnExtra);
	qcrp->SetGrfcrp(fcrpLoner);

	if (pvNil != pblck)
		pblck->Set(&flo);
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Replace or create a chunk of a particular cno.
***************************************************************************/
bool CFL::FPut(long cb, CTG ctg, CNO cno, PBLCK pblck)
{
	AssertThis(0);
	AssertNilOrPo(pblck, 0);

	return _FPut(cb, ctg, cno, pblck, pvNil, pvNil);
}


/***************************************************************************
	Replace or create a chunk with the given cno and put the data in it.
***************************************************************************/
bool CFL::FPutPv(void *pv, long cb, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertIn(cb, 0, kcbMax);
	AssertPvCb(pv, cb);

	return _FPut(cb, ctg, cno, pvNil, pvNil, pv);
}


/***************************************************************************
	Replace or create a chunk with the given cno and put the hq's
	data in it.
***************************************************************************/
bool CFL::FPutHq(HQ hq, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertHq(hq);
	bool fRet;
	HQ hqT = hq;
	BLCK blckSrc(&hq);

	fRet = _FPut(blckSrc.Cb(), ctg, cno, pvNil, &blckSrc, pvNil);
	AssertDo(hqT == blckSrc.HqFree(),
		"blckSrc.HqFree() returned a differnt hq!");

	return fRet;
}


/***************************************************************************
	Replace or create a chunk with the given cno and put the block's
	data in it.  Set the packed flag as in the block.
***************************************************************************/
bool CFL::FPutBlck(PBLCK pblckSrc, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertPo(pblckSrc, 0);

	if (!_FPut(pblckSrc->Cb(fTrue), ctg, cno, pvNil, pblckSrc, pvNil))
		return pvNil;
	SetPacked(ctg, cno, pblckSrc->FPacked());
	return fTrue;
}


/***************************************************************************
	Low level put.  Writes data from pblckSrc or pv (or neither).  If the
	chunk doesn't already exist, this has the same affect as doing an add.
	If it does exist, this doesn't change the fcrpLoner flag or anything
	else except the data.  If pblck isn't nil, this sets it to the location
	of the new data.  Doesn't change the packed flag.
***************************************************************************/
bool CFL::_FPut(long cb, CTG ctg, CNO cno, PBLCK pblck, PBLCK pblckSrc,
	void *pv)
{
	AssertBaseThis(0);
	AssertIn(cb, 0, kcbMax);
	AssertNilOrPo(pblck, 0);
	AssertNilOrPo(pblckSrc, 0);

	long icrp;
	CRP *qcrp;
	FLO flo;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		BLCK blck;

		if (pvNil == pblck)
			pblck = &blck;

		if (!_FAdd(cb, ctg, cno, icrp, pblck))
			return fFalse;

		if (pv != pvNil)
			{
			Assert(pvNil == pblckSrc, 0);
			if (!pblck->FWrite(pv))
				goto LFailNew;
			}
		else if (pvNil != pblckSrc && !pblckSrc->FWriteToBlck(pblck, fTrue))
			{
LFailNew:
			Delete(ctg, cno);
			pblck->Free();
			AssertThis(0);
			return fFalse;
			}

		AssertThis(0);
		return fTrue;
		}

	if (!_FAllocFlo(cb, &flo))
		{
		AssertThis(0);
		return fFalse;
		}
	Assert(flo.pfil == _csto.pfil || flo.pfil == _cstoExtra.pfil, 0);
	if (pvNil != pblck)
		pblck->Set(&flo);

	if (pv != pvNil)
		{
		Assert(pvNil == pblckSrc, 0);
		if (!flo.FWrite(pv))
			goto LFailOld;
		}
	else if (pvNil != pblckSrc && !pblckSrc->FWriteToFlo(&flo, fTrue))
		{
LFailOld:
		_FreeFpCb(_fAddToExtra, flo.fp, flo.cb);
		if (pvNil != pblck)
			pblck->Free();
		AssertThis(0);
		return fFalse;
		}

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	Assert(qcrp->cki.ctg == ctg, 0);
	Assert(qcrp->cki.cno == cno, 0);
	_FreeFpCb(qcrp->Grfcrp(fcrpOnExtra), qcrp->fp, qcrp->Cb());

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	qcrp->fp = flo.fp;
	qcrp->SetCb(flo.cb);
	if (flo.pfil == _cstoExtra.pfil)
		qcrp->SetGrfcrp(fcrpOnExtra);
	else
		qcrp->ClearGrfcrp(fcrpOnExtra);
	_FSetRti(ctg, cno, rtiNil);

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Swaps the data for the two chunks.  This allows a "safe save" of
	individual chunks (create a temp chunk, write the data, swap the data,
	delete the temp chunk).  Doesn't affect child/parent relationships
	or the loner flag.
***************************************************************************/
void CFL::SwapData(CTG ctg1, CNO cno1, CTG ctg2, CNO cno2)
{
	AssertThis(0);
	long icrp1, icrp2;
	CRP *qcrp1, *qcrp2;
	FP fp;
	long cb;
	ulong grfcrpT;
	const ulong kgrfcrpMask = fcrpOnExtra | fcrpPacked | fcrpForest;

	if (!_FFindCtgCno(ctg1, cno1, &icrp1) ||
			!_FFindCtgCno(ctg2, cno2, &icrp2))
		{
		Bug("can't find the chunks");
		return;
		}
	qcrp1 = (CRP *)_pggcrp->QvFixedGet(icrp1);
	qcrp2 = (CRP *)_pggcrp->QvFixedGet(icrp2);

	fp = qcrp1->fp;
	qcrp1->fp = qcrp2->fp;
	qcrp2->fp = fp;
	cb = qcrp1->Cb();
	qcrp1->SetCb(qcrp2->Cb());
	qcrp2->SetCb(cb);

	// swap the bits of grfcrp in grfcrpMask
	grfcrpT = qcrp1->Grfcrp(kgrfcrpMask);
	qcrp1->AssignGrfcrp(qcrp2->Grfcrp(kgrfcrpMask), kgrfcrpMask);
	qcrp2->AssignGrfcrp(grfcrpT, kgrfcrpMask);

	_FSetRti(ctg1, cno1, rtiNil);
	_FSetRti(ctg2, cno2, rtiNil);

	AssertThis(fcflFull);
}


/***************************************************************************
	Swaps the children for the two chunks.  This allows a "safe save" of
	chunk graphs (create a temp chunk, write the data, swap the data,
	swap the children, delete the temp chunk).  Doesn't affect the data
	or the loner flag.
***************************************************************************/
void CFL::SwapChildren(CTG ctg1, CNO cno1, CTG ctg2, CNO cno2)
{
	AssertThis(0);
	long icrp1, icrp2;
	CRP *qcrp1, *qcrp2;
	long cb1, cb2;

	if (!_FFindCtgCno(ctg1, cno1, &icrp1) ||
			!_FFindCtgCno(ctg2, cno2, &icrp2))
		{
		Bug("can't find the chunks");
		return;
		}

	// Swap the child lists.
	qcrp1 = (CRP *)_pggcrp->QvFixedGet(icrp1);
	qcrp2 = (CRP *)_pggcrp->QvFixedGet(icrp2);
	cb1 = LwMul(qcrp1->ckid, size(KID));
	cb2 = LwMul(qcrp2->ckid, size(KID));
	SwapVars(&qcrp1->ckid, &qcrp2->ckid);

	// These FMoveRgb calls won't fail, because no padding is necessary for
	// child entries. (FMoveRgb can fail only if the number of bytes being
	// moved is not a multiple of size(long)).
	if (0 < cb1)
		AssertDo(_pggcrp->FMoveRgb(icrp1, 0, icrp2, cb2, cb1), 0);
	if (0 < cb2)
		AssertDo(_pggcrp->FMoveRgb(icrp2, 0, icrp1, 0, cb2), 0);

	AssertThis(fcflFull);
}


/***************************************************************************
	Move the chunk from (ctg, cno) to (ctgNew, cnoNew).  Asserts that there
	is not already a chunk labelled (ctgNew, cnoNew).  If the chunk has
	parents, updates the parent links to point to (ctgNew, cnoNew).
	Adjusting the links is slow.
***************************************************************************/
void CFL::Move(CTG ctg, CNO cno, CTG ctgNew, CNO cnoNew)
{
	AssertThis(0);
	long icrpCur, icrpTarget;
	CRP *qcrp;
	long ccrpRef;
	long rti;

	if (ctg == ctgNew && cno == cnoNew)
		return;

	if (!_FFindCtgCno(ctg, cno, &icrpCur) ||
			_FFindCtgCno(ctgNew, cnoNew, &icrpTarget))
		{
		Bug("bad move request");
		return;
		}

	rti = _Rti(ctg, cno);
	if (rtiNil != rti)
		_FSetRti(ctg, cno, rtiNil);

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrpCur);
	ccrpRef = qcrp->ccrpRef;
	qcrp->cki.ctg = ctgNew;
	qcrp->cki.cno = cnoNew;
	_pggcrp->Move(icrpCur, icrpTarget);

	if (ccrpRef > 0)
		{
		// chunk has some parents
		CRP crp;
		long icrp, ikid, ikidNew;
		KID *qkid, *qrgkid;

		//In debug, increment ccrpRef so we'll traverse the entire
		//index.  In ship, we'll stop once we changed ccrpRef KIDs.
		Debug( ccrpRef++; )
		for (icrp = _pggcrp->IvMac(); icrp-- > 0 && ccrpRef > 0; )
			{
			_pggcrp->GetFixed(icrp, &crp);
			qkid = (KID *)_pggcrp->QvGet(icrp);
			for (ikid = 0; ikid < crp.ckid; )
				{
				if (qkid->cki.ctg != ctg || qkid->cki.cno != cno)
					{
					ikid++;
					qkid++;
					continue;
					}

				//replace this kid
				AssertDo(!_FFindChild(icrp, ctgNew, cnoNew, qkid->chid,
					&ikidNew), "already a child");

				//refresh the qkid and qrgkid pointers
				qkid = (qrgkid = (KID *)_pggcrp->QvGet(icrp)) + ikid;
				qkid->cki.ctg = ctgNew;
				qkid->cki.cno = cnoNew;

				MoveElement(qrgkid, size(KID), ikid, ikidNew);
				ccrpRef--;
				}
			}
		Assert(ccrpRef == 1, "corrupt chunky index");
		//in ship, ccrpRef should be 0 here
		}

	if (rtiNil != rti && ctgNew == ctg)
		_FSetRti(ctgNew, cnoNew, rti);

	AssertThis(fcflFull);
}


/***************************************************************************
	Delete the given chunk.  Handles deleting child chunks that are
	no longer referenced.  If the chunk has the loner flag set, this clears
	it.  If the chunk has no parents, the chunk is also physically deleted
	from the CFL.
***************************************************************************/
void CFL::Delete(CTG ctg, CNO cno)
{
	AssertThis(0);
	long icrp;
	CGE cge;
	ulong grfcgeIn, grfcge;
	KID kid;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not there");
		return;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	Assert(qcrp->Grfcrp(fcrpLoner) || qcrp->ccrpRef == 0,
		"can't directly delete a child chunk");
	qcrp->ClearGrfcrp(fcrpLoner);
	if (qcrp->ccrpRef > 0)
		return;

	cge.Init(this, ctg, cno);
	for (grfcgeIn = fcgeNil; cge.FNextKid(&kid, pvNil, &grfcge, grfcgeIn); )
		{
		grfcgeIn = fcgeSkipToSib;
		if (!_FFindCtgCno(kid.cki.ctg, kid.cki.cno, &icrp))
			{
			Bug("MIA");
			continue;
			}
		if (grfcge & fcgePre)
			{
			if (!(grfcge & fcgeRoot) && !_FDecRefCount(icrp))
				continue;
			grfcgeIn = fcgeNil;
			}
		if (grfcge & fcgePost)
			{
			// actually delete the node
			if (grfcge & fcgeError)
				{
				Warn("memory failure in CFL::Delete - adjusting ref counts");
				//memory failure - adjust the ref counts of this chunk's
				//children, but don't try to delete them
				long ikid, icrpChild;
				KID kid;

				qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
				for (ikid = qcrp->ckid; ikid-- != 0; )
					{
					_pggcrp->GetRgb(icrp, _BvKid(ikid), size(kid), &kid);
					if (!_FFindCtgCno(kid.cki.ctg, kid.cki.cno, &icrpChild))
						{
						Bug("child not there");
						continue;
						}
					_FDecRefCount(icrpChild);
					}
				}
			_DeleteCore(icrp);
			}
		}
	AssertThis(fcflFull);
}


/***************************************************************************
	Make sure the loner flag is set or clear according to fLoner.  If
	fLoner is false and (ctg, cno) is not currently the child of anything,
	it will be deleted.
***************************************************************************/
void CFL::SetLoner(CTG ctg, CNO cno, bool fLoner)
{
	AssertThis(0);
	long icrp;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not there");
		return;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);

	if (fLoner)
		qcrp->SetGrfcrp(fcrpLoner);
	else if (qcrp->Grfcrp(fcrpLoner))
		Delete(ctg, cno);
}


/***************************************************************************
	Return the value of the loner flag.
***************************************************************************/
bool CFL::FLoner(CTG ctg, CNO cno)
{
	AssertThis(0);
	long icrp;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not there");
		return fFalse;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	return FPure(qcrp->Grfcrp(fcrpLoner));
}


/***************************************************************************
	Returns the number of parents of this chunk
***************************************************************************/
long CFL::CckiRef(CTG ctg, CNO cno)
{
	AssertThis(0);
	long icrp;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not there");
		return 0;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	return qcrp->ccrpRef;
}


/***************************************************************************
	Determines if (ctgSub, cnoSub) is in the chunk subgraph of (ctg, cno).
	Returns tMaybe on error.
***************************************************************************/
bool CFL::TIsDescendent(CTG ctg, CNO cno, CTG ctgSub, CNO cnoSub)
{
	AssertThis(0);
	CGE cge;
	KID kid;
	ulong grfcge;

	if (CckiRef(ctgSub, cnoSub) == 0)
		return tNo;

	cge.Init(this, ctg, cno);
	while (cge.FNextKid(&kid, pvNil, &grfcge, fcgeNil))
		{
		if (kid.cki.ctg == ctgSub && kid.cki.cno == cnoSub)
			return tYes;
		if (grfcge & fcgeError)
			return tMaybe;
		}
	return tNo;
}


/***************************************************************************
	Delete the given child chunk.  Handles deleting child chunks that are
	no longer referenced and don't have the fcrpLoner flag set.
***************************************************************************/
void CFL::DeleteChild(CTG ctgPar, CNO cnoPar, CTG ctgChild, CNO cnoChild,
	CHID chid)
{
	AssertThis(0);
	long icrpPar, icrpChild, ikid;
	CRP *qcrp;

	if (!_FFindCtgCno(ctgPar, cnoPar, &icrpPar) ||
		!_FFindCtgCno(ctgChild, cnoChild, &icrpChild))
		{
		Bug("chunk not there");
		return;
		}
	if (!_FFindChild(icrpPar, ctgChild, cnoChild, chid, &ikid))
		{
		Bug("not a child");
		return;
		}

	// remove the reference
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrpPar);
	qcrp->ckid--;
	_pggcrp->DeleteRgb(icrpPar, _BvKid(ikid), size(KID));

	// now decrement the ref count and nuke it if the ref count is zero
	if (_FDecRefCount(icrpChild))
		{
		// delete the chunk
		Delete(ctgChild, cnoChild);
		}
	AssertThis(fcflFull);
}


/***************************************************************************
	Decrements the reference count on the chunk.  Return true if the ref
	count becomes zero (after decrementing) and fcrpLoner is not set.
***************************************************************************/
bool CFL::_FDecRefCount(long icrp)
{
	AssertBaseThis(0);
	CRP *qcrp;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	if (qcrp->ccrpRef <= 0)
		{
		Bug("ref count wrong");
		return fFalse;
		}
	return --(qcrp->ccrpRef) == 0 && !qcrp->Grfcrp(fcrpLoner);
}


/***************************************************************************
	Remove entry icrp from _pggcrp and add the file space to the free map.
***************************************************************************/
void CFL::_DeleteCore(long icrp)
{
	AssertBaseThis(0);
	CRP *qcrp;
	CKI cki;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	cki = qcrp->cki;
	_FreeFpCb(qcrp->Grfcrp(fcrpOnExtra), qcrp->fp, qcrp->Cb());
	_FSetRti(cki.ctg, cki.cno, rtiNil);
	_pggcrp->Delete(icrp);
}


/***************************************************************************
	Add the (fp, cb) to the free map.
***************************************************************************/
void CFL::_FreeFpCb(bool fOnExtra, FP fp, long cb)
{
	AssertBaseThis(0);
	Assert(cb > 0 || cb == 0 && fp == 0, "bad cb");
	Assert(fp >= 0 && (fOnExtra || fp >= size(CFP) ||
		cb == 0 || _fInvalidMainFile), "bad fp");

	PGL pglfsm;
	long ifsm, ifsmMin, ifsmLim;
	FSM fsm, fsmT;
	CSTO *pcsto;

	// no space allocated to the chunk
	if (cb == 0)
		return;

	// if the free map hasn't been read yet, read it now
	if (!fOnExtra && _fFreeMapNotRead)
		_ReadFreeMap();

	pcsto = fOnExtra ? &_cstoExtra : &_csto;
	pglfsm = pcsto->pglfsm;
	AssertPo(pcsto->pfil, 0);
	Assert(fp + cb <= pcsto->fpMac, "bad (fp,cb)");

	if (fp + cb >= pcsto->fpMac)
		{
		// it's at the end of the file, just change fpMac and
		// compact the free map if possible
		pcsto->fpMac = fp;
		if (pglfsm == pvNil || (ifsm = pglfsm->IvMac()) == 0)
			return;
		ifsm--;
		pglfsm->Get(ifsm, &fsm);
		if (fsm.fp + fsm.cb >= pcsto->fpMac)
			{
			// fsm extends to the new end of the file, so delete the fsm
			// and adjust fpMac
			Assert(fsm.fp + fsm.cb == pcsto->fpMac, "bad fsm?");
			pglfsm->Delete(ifsm);
			pcsto->fpMac = fsm.fp;
			}
		return;
		}

	// Chunk is not at the end of the file.  We need to add it
	// to the free map.
	if (pglfsm == pvNil &&
			(pglfsm = pcsto->pglfsm = GL::PglNew(size(FSM), 1)) == pvNil)
		{
		// can't create the free map, just drop the space
		return;
		}

	for (ifsmMin = 0, ifsmLim = pglfsm->IvMac(); ifsmMin < ifsmLim; )
		{
		ifsm = (ifsmMin + ifsmLim) / 2;
		pglfsm->Get(ifsm, &fsm);
		Assert(fp + cb <= fsm.fp || fp >= fsm.fp + fsm.cb,
			"freeing space that overlaps free space");
		if (fsm.fp < fp)
			ifsmMin = ifsm + 1;
		else
			ifsmLim = ifsm;
		}

	ifsmLim = pglfsm->IvMac();
	if (ifsmMin > 0)
		{
		// check for adjacency to previous free block
		pglfsm->Get(ifsmMin - 1, &fsm);
		Assert(fsm.fp < fp, "bad ifsmMin");
		if (fsm.fp + fsm.cb >= fp)
			{
			// extend the previous free block
			Assert(fsm.fp + fsm.cb == fp, "overlap");
			fsm.cb = fp + cb - fsm.fp;
			if (ifsmMin < ifsmLim)
				{
				// check for adjacency to next free block
				pglfsm->Get(ifsmMin, &fsmT);
				if (fsmT.fp <= fsm.fp + fsm.cb)
					{
					// merge the two
					Assert(fsmT.fp == fsm.fp + fsm.cb, "overlap");
					fsm.cb = fsmT.fp + fsmT.cb - fsm.fp;
					pglfsm->Delete(ifsmMin);
					}
				}
			pglfsm->Put(ifsmMin - 1, &fsm);
			return;
			}
		}

	if (ifsmMin < ifsmLim)
		{
		pglfsm->Get(ifsmMin, &fsm);
		Assert(fsm.fp > fp, "bad ifsmMin");
		if (fsm.fp <= fp + cb)
			{
			Assert(fsm.fp == fp + cb, "overlap");
			fsm.cb = fsm.fp + fsm.cb - fp;
			fsm.fp = fp;
			pglfsm->Put(ifsmMin, &fsm);
			return;
			}
		}

	fsm.fp = fp;
	fsm.cb = cb;

	//if it fails, we lose some space - so what
	pglfsm->FInsert(ifsmMin, &fsm);
}


/***************************************************************************
	Set the name of the chunk.
***************************************************************************/
bool CFL::FSetName(CTG ctg, CNO cno, PSTN pstn)
{
	AssertThis(0);
	AssertNilOrPo(pstn, 0);
	long icrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not there");
		return fFalse;
		}

	if (!_FSetName(icrp, pstn))
		return fFalse;

	_FSetRti(ctg, cno, rtiNil);

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Set the name of the chunk at the given index.
***************************************************************************/
bool CFL::_FSetName(long icrp, PSTN pstn)
{
	AssertBaseThis(0);
	AssertIn(icrp, 0, _pggcrp->IvMac());
	AssertNilOrPo(pstn, 0);
	long cbVar, cbOld, cbNew;
	CRP *qcrp;
	long bvRgch;

	if (pstn != pvNil && pstn->Cch() > 0)
		cbNew = pstn->CbData();
	else
		cbNew = 0;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp, &cbVar);
	bvRgch = qcrp->BvRgch();
	cbOld = cbVar - bvRgch;
	AssertIn(cbOld, 0, kcbMaxDataStn + 1);

	if (cbOld > cbNew)
		_pggcrp->DeleteRgb(icrp, bvRgch, cbOld - cbNew);
	else if (cbOld < cbNew && !_pggcrp->FInsertRgb(icrp, bvRgch,
			cbNew - cbOld, pvNil))
		{
		return fFalse;
		}

	if (cbNew > 0)
		{
		pstn->GetData(PvAddBv(_pggcrp->PvLock(icrp), bvRgch));
		_pggcrp->Unlock();
		}

	return fTrue;
}


/***************************************************************************
	Retrieve the name of the chunk.  Returns whether the string is
	non-empty.
***************************************************************************/
bool CFL::FGetName(CTG ctg, CNO cno, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	long icrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not there");
		pstn->SetNil();
		return fFalse;
		}

	return _FGetName(icrp, pstn);
}


/***************************************************************************
	Retrieve the name of the chunk at the given index.  Returns whether
	the string is non-empty.
***************************************************************************/
bool CFL::_FGetName(long icrp, PSTN pstn)
{
	AssertBaseThis(0);
	AssertIn(icrp, 0, _pggcrp->IvMac());
	AssertPo(pstn, 0);

	long cbRgch, bvRgch;
	long cbVar;
	CRP *qcrp;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp, &cbVar);
	cbRgch = qcrp->CbRgch(cbVar);
	AssertIn(cbRgch, 0, kcbMaxDataStn + 1);
	if (cbRgch <= 0)
		{
		pstn->SetNil();
		return fFalse;
		}
	bvRgch = qcrp->BvRgch();

	if (!pstn->FSetData(PvAddBv(_pggcrp->PvLock(icrp), bvRgch), cbRgch))
		pstn->SetNil();
	_pggcrp->Unlock();

	return pstn->Cch() > 0;
}


/***************************************************************************
	Make a node a child of another node.  If fClearLoner is set, the loner
	flag of the child is cleared.
***************************************************************************/
bool CFL::FAdoptChild(CTG ctgPar, CNO cnoPar, CTG ctgChild, CNO cnoChild,
	CHID chid, bool fClearLoner)
{
	AssertThis(0);
	long icrpPar;
	long ikid;

	if (!_FFindCtgCno(ctgPar, cnoPar, &icrpPar))
		{
		Bug("parent not there");
		return fFalse;
		}
	if (_FFindChild(icrpPar, ctgChild, cnoChild, chid, &ikid))
		return fTrue; //already a child

	if (!_FAdoptChild(icrpPar, ikid, ctgChild, cnoChild, chid, fClearLoner))
		{
		AssertThis(0);
		return fFalse;
		}

	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Make (ctgChild, cnoChild) a child of icrpPar.
***************************************************************************/
bool CFL::_FAdoptChild(long icrpPar, long ikid, CTG ctgChild, CNO cnoChild,
	CHID chid, bool fClearLoner)
{
	AssertBaseThis(0);
	AssertIn(icrpPar, 0, _pggcrp->IvMac());
	long icrp;
	CRP *qcrp;
	KID kid;

	if (!_FFindCtgCno(ctgChild, cnoChild, &icrp))
		{
		Bug("child not there");
		return fFalse;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrpPar);
	AssertIn(ikid, 0, (long)qcrp->ckid + 1);
	if (tNo != TIsDescendent(ctgChild, cnoChild, qcrp->cki.ctg, qcrp->cki.cno))
		{
		Warn("Performing this adoption may cause a cycle");
		return fFalse;
		}

	kid.cki.ctg = ctgChild;
	kid.cki.cno = cnoChild;
	kid.chid = chid;
	if (!_pggcrp->FInsertRgb(icrpPar, _BvKid(ikid), size(KID), &kid))
		{
		AssertThis(0);
		return fFalse;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icrpPar);
	if (qcrp->ckid >= kckidMax)
		goto LOverFlow;
	qcrp->ckid++;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	if (qcrp->ccrpRef >= kckidMax)
		{
		qcrp = (CRP *)_pggcrp->QvFixedGet(icrpPar);
		qcrp->ckid--;
LOverFlow:
		_pggcrp->DeleteRgb(icrpPar, _BvKid(ikid), size(KID));
		AssertThis(0);
		return fFalse;
		}
	qcrp->ccrpRef++;

	if (fClearLoner)
		qcrp->ClearGrfcrp(fcrpLoner);
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Changes the chid value of the given child link.  Assert that
	(ctgChild, cnoChild, chidOld) is a child of (ctgPar, cnoPar) and that
	(ctgChild, cnoChild, chidNew) is not currently a child.
***************************************************************************/
void CFL::ChangeChid(CTG ctgPar, CNO cnoPar, CTG ctgChild, CNO cnoChild,
	CHID chidOld, CHID chidNew)
{
	AssertThis(0);
	long icrp, ikidOld, ikidNew;
	KID *qkid, *qrgkid;

	if (chidOld == chidNew)
		return;

	if (!_FFindCtgCno(ctgPar, cnoPar, &icrp))
		{
		Bug("parent not there");
		return;
		}
	if (!_FFindChild(icrp, ctgChild, cnoChild, chidOld, &ikidOld) ||
			_FFindChild(icrp, ctgChild, cnoChild, chidNew, &ikidNew))
		{
		Bug("src not a child or dest already a child");
		return;
		}

	qkid = (qrgkid = (KID *)_pggcrp->QvGet(icrp)) + ikidOld;
	qkid->chid = chidNew;

	MoveElement(qrgkid, size(KID), ikidOld, ikidNew);

	AssertThis(0);
}


/***************************************************************************
	Return the total number of chunks.
***************************************************************************/
long CFL::Ccki(void)
{
	AssertThis(0);
	return _pggcrp->IvMac();
}


/***************************************************************************
	Finds the icki'th chunk.  If there is such a chunk (icki isn't too big),
	fills in *pcki and *pckid and returns true.  Otherwise, returns fFalse.
***************************************************************************/
bool CFL::FGetCki(long icki, CKI *pcki, long *pckid, PBLCK pblck)
{
	AssertThis(0);
	AssertNilOrVarMem(pcki);
	AssertNilOrVarMem(pckid);
	AssertNilOrPo(pblck, 0);
	AssertIn(icki, 0, kcbMax);
	CRP *qcrp;

	if (!FIn(icki, 0, _pggcrp->IvMac()))
		{
		TrashVar(pcki);
		TrashVar(pckid);
		if (pvNil != pblck)
			pblck->Free();
		return fFalse;
		}
	qcrp = (CRP *)_pggcrp->QvFixedGet(icki);
	if (pvNil != pcki)
		*pcki = qcrp->cki;
	if (pvNil != pckid)
		*pckid = qcrp->ckid;
	if (pvNil != pblck)
		_GetBlck(icki, pblck);

	return fTrue;
}


/***************************************************************************
	Finds the icki corresponding to the given (ctg, cno).  If the (ctg, cno)
	is not in the CFL, fills *picki with where it would be.
***************************************************************************/
bool CFL::FGetIcki(CTG ctg, CNO cno, long *picki)
{
	AssertThis(0);
	AssertVarMem(picki);
	return _FFindCtgCno(ctg, cno, picki);
}


/***************************************************************************
	Return the total number of chunks of the given type in the file.
***************************************************************************/
long CFL::CckiCtg(CTG ctg)
{
	AssertThis(0);
	long icrpMac;
	long icrpMin;
	long icrpLim;

	if ((icrpMac = _pggcrp->IvMac()) == 0)
		return 0;

	_FFindCtgCno(ctg, 0, &icrpMin);
	if (ctg + 1 < ctg)
		{
		// ctg is the largest possible ctg!
		return icrpMac - icrpMin;
		}

	_FFindCtgCno(ctg + 1, 0, &icrpLim);
	return icrpLim - icrpMin;
}


/***************************************************************************
	Finds the icki'th chunk of type ctg.  If there is such a chunk,
	fills in *pcki and returns true.  Otherwise, returns fFalse.
***************************************************************************/
bool CFL::FGetCkiCtg(CTG ctg, long icki, CKI *pcki, long *pckid, PBLCK pblck)
{
	AssertThis(0);
	AssertIn(icki, 0, kcbMax);
	AssertNilOrVarMem(pcki);
	AssertNilOrVarMem(pckid);
	AssertNilOrPo(pblck, 0);
	CRP *qcrp;
	long icrpMac;
	long icrp;

	if (!FIn(icki, 0, (icrpMac = _pggcrp->IvMac())))
		goto LFail;

	_FFindCtgCno(ctg, 0, &icrp);
	icrp += icki;
	if (!FIn(icrp, 0, icrpMac))
		goto LFail;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	if (qcrp->cki.ctg != ctg)
		{
LFail:
		TrashVar(pcki);
		TrashVar(pckid);
		if (pvNil != pblck)
			pblck->Free();
		return fFalse;
		}
	if (pvNil != pcki)
		*pcki = qcrp->cki;
	if (pvNil != pckid)
		*pckid = qcrp->ckid;
	if (pvNil != pblck)
		_GetBlck(icki, pblck);

	return fTrue;
}


/***************************************************************************
	Return the number of children of the given chunk.
***************************************************************************/
long CFL::Ckid(CTG ctg, CNO cno)
{
	AssertThis(0);
	CRP *qcrp;
	long icrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not found");
		return 0;
		}

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	return qcrp->ckid;
}


/***************************************************************************
	If ikid is less than number of children of the given chunk,
	fill *pkid and return true.  Otherwise, return false.
***************************************************************************/
bool CFL::FGetKid(CTG ctg, CNO cno, long ikid, KID *pkid)
{
	AssertThis(0);
	AssertVarMem(pkid);
	AssertIn(ikid, 0, kcbMax);
	CRP *qcrp;
	long icrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		{
		Bug("chunk not found");
		TrashVar(pkid);
		return fFalse;
		}

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	if (!FIn(ikid, 0, qcrp->ckid))
		{
		TrashVar(pkid);
		return fFalse;
		}
	_pggcrp->GetRgb(icrp, _BvKid(ikid), size(KID), pkid);
	Assert(_FFindCtgCno(pkid->cki.ctg, pkid->cki.cno, &icrp),
		"child not there");
	Assert(pkid->cki.ctg != ctg || pkid->cki.cno != cno,
		"chunk is child of itself!");
	return fTrue;
}


/***************************************************************************
	Look for a child of (ctgPar, cnoPar) with the given chid value.  If one
	is found, fill in *pkid and return true; else return false.  Kid's are
	sorted by (chid, ctg, cno).
***************************************************************************/
bool CFL::FGetKidChid(CTG ctgPar, CNO cnoPar, CHID chid, KID *pkid)
{
	AssertThis(0);
	AssertVarMem(pkid);
	return _FFindChidCtg(ctgPar, cnoPar, chid, (CTG)0, pkid);
}


/***************************************************************************
	Look for a child of (ctgPar, cnoPar) with the given chid and ctg value.
	If one is found, fill in *pkid and return true; else return false.
	Kid's are sorted by (chid, ctg, cno).
***************************************************************************/
bool CFL::FGetKidChidCtg(CTG ctgPar, CNO cnoPar, CHID chid, CTG ctg, KID *pkid)
{
	AssertThis(0);
	AssertVarMem(pkid);

	//the kid returned from _FFindChidCtg should have ctg >= the given ctg,
	//but not necessarily equal
	if (!_FFindChidCtg(ctgPar, cnoPar, chid, ctg, pkid) ||
			pkid->cki.ctg != ctg)
		{
		TrashVar(pkid);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Find the first child with the given chid and with ctg >= the given ctg.
	Returns true iff there is such a child and fills in the *pkid.
***************************************************************************/
bool CFL::_FFindChidCtg(CTG ctgPar, CNO cnoPar, CHID chid, CTG ctg, KID *pkid)
{
	AssertThis(0);
	AssertVarMem(pkid);
	long ikidMin, ikidLim, ikid;
	CRP *qcrp;
	KID *qrgkid, *qkid;
	long ckid, icrp;

	if (!_FFindCtgCno(ctgPar, cnoPar, &icrp))
		{
		Bug("chunk not found");
		goto LFail;
		}

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
	if ((ckid = qcrp->ckid) <= 0)
		{
		Assert(0 == ckid, "bad crp");
		goto LFail;
		}

	qrgkid = (KID *)_pggcrp->QvGet(icrp);
	for (ikidMin = 0, ikidLim = ckid; ikidMin < ikidLim; )
		{
		ikid = (ikidMin + ikidLim) / 2;
		qkid = &qrgkid[ikid];

		if (qkid->chid < chid)
			ikidMin = ikid + 1;
		else if (qkid->chid > chid)
			ikidLim = ikid;
		else if (qkid->cki.ctg < ctg)
			ikidMin = ikid + 1;
		else
			ikidLim = ikid;
		}

	if (ikidMin < qcrp->ckid)
		{
		qkid = &qrgkid[ikidMin];
		if (qkid->chid == chid && qkid->cki.ctg >= ctg)
			{
			*pkid = *qkid;
			return fTrue;
			}
		}
LFail:
	TrashVar(pkid);
	return fFalse;
}


/***************************************************************************
	Finds the ikid value associated with the given child (ctg, cno, chid) of
	the given chunk.  If the (ctg, cno, chid) is not a child of
	(ctgPar, cnoPar), fills *pikid with where it would be if it were.
***************************************************************************/
bool CFL::FGetIkid(CTG ctgPar, CNO cnoPar, CTG ctg, CNO cno, CHID chid,
	long *pikid)
{
	AssertThis(0);
	AssertVarMem(pikid);
	long icrp;

	if (!_FFindCtgCno(ctgPar, cnoPar, &icrp))
		{
		Bug("chunk not found");
		*pikid = 0;
		return fFalse;
		}

	return _FFindChild(icrp, ctg, cno, chid, pikid);
}


/***************************************************************************
	If (ctgChild, cnoChild, chid) is a child of icrpPar, return true and put
	the index in *pikid.  If not set *pikid to where to insert it.  Kids are
	sorted by (chid, ctg, cno).
***************************************************************************/
bool CFL::_FFindChild(long icrpPar, CTG ctgChild, CNO cnoChild, CHID chid,
	long *pikid)
{
	AssertBaseThis(0);
	AssertVarMem(pikid);
	AssertIn(icrpPar, 0, _pggcrp->IvMac());

	long ikidMin, ikidLim, ikid, ckid;
	CRP *qcrp;
	KID *qrgkid, *qkid;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrpPar);
	if ((ckid = qcrp->ckid) <= 0)
		{
		Assert(0 == ckid, "bad crp");
		*pikid = 0;
		return fFalse;
		}

	qrgkid = (KID *)_pggcrp->QvGet(icrpPar);
	for (ikidMin = 0, ikidLim = ckid; ikidMin < ikidLim; )
		{
		ikid = (ikidMin + ikidLim) / 2;
		qkid = &qrgkid[ikid];

		if (qkid->chid < chid)
			ikidMin = ikid + 1;
		else if (qkid->chid > chid)
			ikidLim = ikid;
		else if (qkid->cki.ctg < ctgChild)
			ikidMin = ikid + 1;
		else if (qkid->cki.ctg > ctgChild)
			ikidLim = ikid;
		else if (qkid->cki.cno < cnoChild)
			ikidMin = ikid + 1;
		else if (qkid->cki.cno > cnoChild)
			ikidLim = ikid;
		else
			{
			*pikid = ikid;
			return fTrue;
			}
		}

	*pikid = ikidMin;
	return fFalse;
}


// cno map entry
struct CNOM
	{
	CTG ctg;
	CNO cnoSrc;
	CNO cnoDst;
	};

bool _FFindCnom(PGL pglcnom, CTG ctg, CNO cno, CNOM *pcnom = pvNil,
	long *picnom = pvNil);
bool _FAddCnom(PGL *ppglcnom, CNOM *pcnom);


/***************************************************************************
	Look for a cnom for the given (ctg, cno). Whether or not it exists,
	fill *picnom with where it would go in the pglcnom.
***************************************************************************/
bool _FFindCnom(PGL pglcnom, CTG ctg, CNO cno, CNOM *pcnom, long *picnom)
{
	AssertNilOrPo(pglcnom, 0);
	AssertNilOrVarMem(pcnom);
	AssertNilOrVarMem(picnom);
	long ivMin, ivLim, iv;
	CNOM cnom;

	if (pvNil == pglcnom)
		{
		TrashVar(pcnom);
		if (pvNil != picnom)
			*picnom = 0;
		return fFalse;
		}

	for (ivMin = 0, ivLim = pglcnom->IvMac(); ivMin < ivLim; )
		{
		iv = (ivMin + ivLim) / 2;
		pglcnom->Get(iv, &cnom);
		if (cnom.ctg < ctg)
			ivMin = iv + 1;
		else if (cnom.ctg > ctg)
			ivLim = iv;
		else if (cnom.cnoSrc < cno)
			ivMin = iv + 1;
		else if (cnom.cnoSrc > cno)
			ivLim = iv;
		else
			{
			if (pvNil != pcnom)
				*pcnom = cnom;
			if (pvNil != picnom)
				*picnom = iv;
			return fTrue;
			}
		}

	TrashVar(pcnom);
	if (pvNil != picnom)
		*picnom = ivMin;
	return fFalse;
}


/***************************************************************************
	Add a cnom to the *ppglcnom. Allocated *ppglcnom if it is nil.
***************************************************************************/
bool _FAddCnom(PGL *ppglcnom, CNOM *pcnom)
{
	AssertVarMem(ppglcnom);
	AssertNilOrPo(*ppglcnom, 0);
	AssertVarMem(pcnom);
	long icnom;

	if (pvNil == *ppglcnom && pvNil == (*ppglcnom = GL::PglNew(size(CNOM))))
		return fFalse;

	AssertDo(!_FFindCnom(*ppglcnom, pcnom->ctg, pcnom->cnoSrc, pvNil, &icnom),
		"why is this cnom already in the gl?");
	return (*ppglcnom)->FInsert(icnom, pcnom);
}


/***************************************************************************
	Copy a chunk (ctgSrc, cnoSrc) from this chunky file to pcflDst.
	The new cno is put in *pcno.  The destination chunk is marked as a
	loner. If possible, the cno in the destination file will be the same
	as the cno in the source file. If fClone is set, no chunk sharing will
	occur. Otherwise, this does intelligent chunk sharing.
***************************************************************************/
bool CFL::_FCopy(CTG ctgSrc, CNO cnoSrc, PCFL pcflDst, CNO *pcnoDst,
	bool fClone)
{
	AssertThis(fcflFull);
	AssertPo(pcflDst, fcflFull);
	AssertVarMem(pcnoDst);

	long icrpSrc, icrpDst;
	long rtiSrc;
	CGE cge;
	KID kid;
	CKI ckiPar;
	BLCK blckSrc;
	ulong grfcge, grfcgeIn;
	CNOM cnom, cnomPar;
	STN stn;
	CRP *qcrp;

	bool fFreeDstOnFailure = fFalse;
	PGL pglcnom = pvNil;
	bool fRet = fFalse;

	if (!_FFindCtgCno(ctgSrc, cnoSrc, &icrpSrc))
		{
		Bug("chunk not found");
		TrashVar(pcnoDst);
		return fFalse;
		}

	if (!fClone && this == pcflDst)
		{
		SetLoner(ctgSrc, cnoSrc, fTrue);
		*pcnoDst = cnoSrc;
		return fTrue;
		}

	// copy chunks to the destination CFL
	cge.Init(this, ctgSrc, cnoSrc);
	grfcgeIn = fcgeNil;
	while (cge.FNextKid(&kid, &ckiPar, &grfcge, grfcgeIn))
		{
		grfcgeIn = fcgeNil;
		if (grfcge & fcgeError)
			goto LFail;

		// do pre-order handling only
		if (!(grfcge & fcgePre))
			continue;

		if (_FFindCnom(pglcnom, kid.cki.ctg, kid.cki.cno, &cnom))
			{
			// chunk has already been copied - just link it to the parent
			// and skip to its sibling
			Assert(!(grfcge & fcgeRoot),
				"how can the root already have been copied?");
			grfcgeIn = fcgeSkipToSib;
			}
		else if (rtiNil != (rtiSrc = _Rti(kid.cki.ctg, kid.cki.cno)) &&
			!fClone &&
			_FFindMatch(kid.cki.ctg, kid.cki.cno, pcflDst, &cnom.cnoDst))
			{
			// chunk and its subgraph exists in the destination, just link it
			// to the parent and skip to its sibling
			grfcgeIn = fcgeSkipToSib;
			}
		else
			{
			// must copy the chunk

			// assign the source chunk an rti (if it doesn't have one)
			if (rtiNil == rtiSrc &&
					_FSetRti(kid.cki.ctg, kid.cki.cno, _rtiLast + 1))
				{
				rtiSrc = ++_rtiLast;
				}

			cnom.ctg = kid.cki.ctg;
			cnom.cnoSrc = kid.cki.cno;

			// find the source icrp
			AssertDo(_FFindCtgCno(kid.cki.ctg, kid.cki.cno, &icrpSrc), 0);

			// get the source blck
			_GetBlck(icrpSrc, &blckSrc);

			// allocate the dst chunk and copy the data - use the source cno
			// if possible
			if (this != pcflDst && !pcflDst->FFind(kid.cki.ctg, kid.cki.cno))
				{
				// can preserve cno
				cnom.cnoDst = kid.cki.cno;
				if (!pcflDst->FPutBlck(&blckSrc, kid.cki.ctg, cnom.cnoDst))
					goto LFail;
				}
			else
				{
				if (!pcflDst->FAddBlck(&blckSrc, kid.cki.ctg, &cnom.cnoDst))
					goto LFail;
				if (this == pcflDst)
					{
					AssertDo(
						_FFindCtgCno(kid.cki.ctg, kid.cki.cno, &icrpSrc), 0);
					}
				}

			AssertDo(pcflDst->_FFindCtgCno(kid.cki.ctg, cnom.cnoDst, &icrpDst),
				"_FFindCtgCno doesn't work");

			// make sure the forest flags match
			qcrp = (CRP *)_pggcrp->QvFixedGet(icrpSrc);
			if (qcrp->Grfcrp(fcrpForest))
				{
				qcrp = (CRP *)pcflDst->_pggcrp->QvFixedGet(icrpDst);
				qcrp->SetGrfcrp(fcrpForest);
				}

			// set the dst name and rti to the src name and rti
			if (_FGetName(icrpSrc, &stn) && !pcflDst->_FSetName(icrpDst, &stn))
				{
				pcflDst->Delete(kid.cki.ctg, cnom.cnoDst);
				goto LFail;
				}
			if (rtiNil != rtiSrc)
				pcflDst->_FSetRti(kid.cki.ctg, cnom.cnoDst, rtiSrc);

			if (!_FAddCnom(&pglcnom, &cnom))
				goto LFail;
			fFreeDstOnFailure = fTrue;
			}

		// if it's the root, it has no parent and we need to set *pcnoDst
		if (grfcge & fcgeRoot)
			{
			*pcnoDst = cnom.cnoDst;
			continue;
			}

		// get the source parent's cnom
		AssertDo(_FFindCnom(pglcnom, ckiPar.ctg, ckiPar.cno, &cnomPar), 0);

		// make sure the dst child is a child of the dst parent
		if (!pcflDst->FAdoptChild(ckiPar.ctg, cnomPar.cnoDst,
				kid.cki.ctg, cnom.cnoDst, kid.chid, grfcgeIn == fcgeNil))
			{
			if (grfcgeIn == fcgeNil)
				pcflDst->Delete(kid.cki.ctg, cnom.cnoDst);
			goto LFail;
			}
		}

	fRet = fTrue;
	pcflDst->SetLoner(ctgSrc, *pcnoDst, fTrue);

LFail:
	AssertThis(fcflFull);
	AssertPo(pcflDst, fcflFull);
	ReleasePpo(&pglcnom);
	if (!fRet && fFreeDstOnFailure)
		pcflDst->Delete(ctgSrc, *pcnoDst);
	TrashVarIf(!fRet, pcnoDst);

	return fRet;
}


/***************************************************************************
	See if there is a subgraph of pcflDst matching the subgraph at
	(ctgSrc, cnoSrc). Subgraphs match if there is one-to-one correspondence
	of nodes and arcs of the two subgraphs and the rti's of corresponding
	nodes are equal.
***************************************************************************/
bool CFL::_FFindMatch(CTG ctgSrc, CNO cnoSrc, PCFL pcflDst, CNO *pcnoDst)
{
	AssertBaseThis(0);
	AssertPo(pcflDst, 0);
	AssertVarMem(pcnoDst);

	long rtiSrc, rtiKid;
	long ckidSrc;
	CNO cnoMin, cnoDst;
	CGE cgeSrc, cgeDst;
	KID kidSrc, kidDst;
	CKI ckiParSrc, ckiParDst;
	ulong grfcgeSrc, grfcgeDst;
	bool fKidSrc, fKidDst;

	if (this == pcflDst || rtiNil == (rtiSrc = _Rti(ctgSrc, cnoSrc)))
		return fFalse;

	ckidSrc = Ckid(ctgSrc, cnoSrc);
	for (cnoDst = cnoMin = 0; ; cnoMin = cnoDst + 1)
		{
		// get the next chunk with the same rti
		if (cnoDst == (CNO)(-1) ||
				!pcflDst->_FFindCtgRti(ctgSrc, rtiSrc, cnoMin, &cnoDst))
			{
			return fFalse;
			}

		if (pcflDst->Ckid(ctgSrc, cnoDst) != ckidSrc)
			continue;

		cgeSrc.Init(this, ctgSrc, cnoSrc);
		cgeDst.Init(pcflDst, ctgSrc, cnoDst);
		for (;;)
			{
			// get the next element of the the source graph
			fKidSrc = cgeSrc.FNextKid(&kidSrc, &ckiParSrc, &grfcgeSrc, fcgeNil);

			// if the source chunk doesn't have an rti, there's no hope
			if (fKidSrc &&
					rtiNil == (rtiKid = _Rti(kidSrc.cki.ctg, kidSrc.cki.cno)))
				{
				return fFalse;
				}

			// get the next element of the destination graph
			fKidDst = cgeDst.FNextKid(&kidDst, &ckiParDst, &grfcgeDst, fcgeNil);

			if (FPure(fKidSrc) != FPure(fKidDst))
				{
				// the two graphs have different numbers of nodes, so they
				// don't match
				break;
				}

			if (!fKidSrc)
				{
				// we're finished with the enumeration and everything matched
				*pcnoDst = cnoDst;
				return fTrue;
				}
			if ((grfcgeSrc | grfcgeDst) & fcgeError)
				return fFalse;

			if ((grfcgeSrc ^ grfcgeDst) & (fcgePre | fcgePost | fcgeRoot) ||
				kidSrc.cki.ctg != kidDst.cki.ctg ||
				!(grfcgeSrc & fcgeRoot) && kidSrc.chid != kidDst.chid ||
				rtiKid != pcflDst->_Rti(kidDst.cki.ctg, kidDst.cki.cno) ||
				Ckid(kidSrc.cki.ctg, kidSrc.cki.cno) !=
					pcflDst->Ckid(kidDst.cki.ctg, kidDst.cki.cno))
				{
				// children don't match
				break;
				}
			}
		}
}


/***************************************************************************
	Looks for a chunk of the given type with assigned rti and cno at least
	cnoMin.
***************************************************************************/
bool CFL::_FFindCtgRti(CTG ctg, long rti, CNO cnoMin, CNO *pcno)
{
	AssertBaseThis(0);
	AssertNilOrVarMem(pcno);

#ifdef CHUNK_BIG_INDEX

	long icrp, ccrp;
	CRP *qcrp;

	_FFindCtgCno(ctg, cnoMin, &icrp);
	ccrp = Ccki();
	for ( ; icrp < ccrp; icrp++)
		{
		qcrp = (CRP *)_pggcrp->QvFixedGet(icrp);
		if (qcrp->cki.ctg != ctg)
			break; //done
		if (qcrp->rti == rti)
			{
			// found one
			if (pcno != pvNil)
				*pcno = qcrp->cki.cno;
			return fTrue;
			}
		}

#else //!CHUNK_BIG_INDEX

	long irtie, crtie;
	RTIE rtie;

	if (pvNil != _pglrtie && 0 < (crtie = _pglrtie->IvMac()))
		{
		_FFindRtie(ctg, cnoMin, pvNil, &irtie);
		for ( ; irtie < crtie; irtie++)
			{
			_pglrtie->Get(irtie, &rtie);
			if (rtie.ctg != ctg)
				break; //done
			if (rtie.rti == rti)
				{
				// found one
				if (pcno != pvNil)
					*pcno = rtie.cno;
				return fTrue;
				}
			}
		}

#endif //!CHUNK_BIG_INDEX

	TrashVar(pcno);
	return fFalse;
}


/***************************************************************************
	Copy a chunk (ctgSrc, cnoSrc) from this chunky file to pcflDst.
	The new cno is put in *pcno.  The destination chunk is marked as a
	loner. If possible, the cno in the destination file will be the same
	as the cno in the source file. This does intelligent chunk sharing.
***************************************************************************/
bool CFL::FCopy(CTG ctgSrc, CNO cnoSrc, PCFL pcflDst, CNO *pcnoDst)
{
	return _FCopy(ctgSrc, cnoSrc, pcflDst, pcnoDst, fFalse);
}


/***************************************************************************
	Clone a chunk subgraph from this chunky file to pcflDst. This will
	make a copy of the the chunk and its descendents without using any
	previously existing chunks in the destination. The new cno is put in
	*pcno.  The destination chunk is marked as a loner. If possible, the
	cno in the destination file will be the same as the cno in the
	source file.
***************************************************************************/
bool CFL::FClone(CTG ctgSrc, CNO cnoSrc, PCFL pcflDst, CNO *pcnoDst)
{
	return _FCopy(ctgSrc, cnoSrc, pcflDst, pcnoDst, fTrue);
}


/***************************************************************************
	Return the run time id of the given chunk.
***************************************************************************/
long CFL::_Rti(CTG ctg, CNO cno)
{
	AssertBaseThis(0);

#ifdef CHUNK_BIG_INDEX

	long icrp;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		return rtiNil;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp, &cbVar);
	return qcrp->rti;

#else //!CHUNK_BIG_INDEX

	RTIE rtie;

	if (!_FFindRtie(ctg, cno, &rtie))
		return rtiNil;

	return rtie.rti;

#endif //!CHUNK_BIG_INDEX
}


/***************************************************************************
	Set the run time id of the given chunk.
***************************************************************************/
bool CFL::_FSetRti(CTG ctg, CNO cno, long rti)
{
	AssertBaseThis(0);

#ifdef CHUNK_BIG_INDEX

	long icrp;
	CRP *qcrp;

	if (!_FFindCtgCno(ctg, cno, &icrp))
		return fFalse;

	qcrp = (CRP *)_pggcrp->QvFixedGet(icrp, &cbVar);
	qcrp->rti = rti;
	return fTrue;

#else //!CHUNK_BIG_INDEX

	RTIE rtie;
	long irtie;

	if (_FFindRtie(ctg, cno, &rtie, &irtie))
		{
		if (rtiNil == rti)
			_pglrtie->Delete(irtie);
		else
			{
			rtie.rti = rti;
			_pglrtie->Put(irtie, &rtie);
			}
		return fTrue;
		}

	if (rtiNil == rti)
		return fTrue;

	rtie.ctg = ctg;
	rtie.cno = cno;
	rtie.rti = rti;
	if (pvNil == _pglrtie && pvNil == (_pglrtie = GL::PglNew(size(RTIE), 1)))
		return fFalse;

	return _pglrtie->FInsert(irtie, &rtie);

#endif //!CHUNK_BIG_INDEX
}


#ifndef CHUNK_BIG_INDEX
/***************************************************************************
	Look for an RTIE entry for the given (ctg, cno).
***************************************************************************/
bool CFL::_FFindRtie(CTG ctg, CNO cno, RTIE *prtie, long *pirtie)
{
	AssertBaseThis(0);
	AssertNilOrVarMem(prtie);
	AssertNilOrVarMem(pirtie);

	long ivMin, ivLim, iv;
	RTIE rtie;

	if (pvNil == _pglrtie)
		{
		if (pvNil != pirtie)
			*pirtie = 0;
		TrashVar(prtie);
		return fFalse;
		}

	for (ivMin = 0, ivLim = _pglrtie->IvMac(); ivMin < ivLim; )
		{
		iv = (ivMin + ivLim) / 2;
		_pglrtie->Get(iv, &rtie);
		if (rtie.ctg < ctg)
			ivMin = iv + 1;
		else if (rtie.ctg > ctg)
			ivLim = iv;
		else if (rtie.cno < cno)
			ivMin = iv + 1;
		else if (rtie.cno > cno)
			ivLim = iv;
		else
			{
			if (pvNil != prtie)
				*prtie = rtie;
			if (pvNil != pirtie)
				*pirtie = iv;
			return fTrue;;
			}
		}

	if (pvNil != pirtie)
		*pirtie = ivMin;
	TrashVar(prtie);
	return fFalse;
}
#endif //!CHUNK_BIG_INDEX


/***************************************************************************
	Constructor for chunk graph enumerator.
***************************************************************************/
CGE::CGE(void)
{
	AssertThisMem();

	_es = esDone;
	_pgldps = pvNil;
	AssertThis(0);
}


/***************************************************************************
	Destructor for chunk graph enumerator.
***************************************************************************/
CGE::~CGE(void)
{
	AssertThis(0);
	ReleasePpo(&_pgldps);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the cge
***************************************************************************/
void CGE::AssertValid(ulong grf)
{
	CGE_PAR::AssertValid(0);
	AssertIn(_es, esStart, esDone + 1);
	if (FIn(_es, esStart, esDone))
		{
		AssertPo(_pcfl, 0);
		AssertNilOrPo(_pgldps, 0);
		}
	else
		Assert(_pgldps == pvNil, "_pgldps not nil");
}


/***************************************************************************
	Mark memory used by the cge.
***************************************************************************/
void CGE::MarkMem(void)
{
	AssertThis(0);
	CGE_PAR::MarkMem();
	MarkMemObj(_pgldps);
}
#endif //DEBUG


/***************************************************************************
	Start a new enumeration.
***************************************************************************/
void CGE::Init(PCFL pcfl, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertPo(pcfl, 0);

	ReleasePpo(&_pgldps);
	_es = esStart;
	_pcfl = pcfl;
	_dps.kid.cki.ctg = ctg;
	_dps.kid.cki.cno = cno;
	TrashVar(&_dps.kid.chid);
	AssertThis(0);
}


/***************************************************************************
	Fetch the next node in the graph enumeration.  Generally, parent nodes
	are returned twice (once with fcgePre and again with fcgePost).  Nodes
	without children are returned only once (with both fcgePre and fcgePost
	set).  The new node is put in *pkid, and the node's parent (if the node
	is not the root of the enumeration) is put in *pckiPar.  pckiPar may
	be nil.

	If (grfcgeIn & fcgeSkipToSib), skips all children and the upward touch
	of the last node returned.

	The *pgrfcgeOut value can contain any combination of: fcgePre, fcgePost,
	fcgeError, fcgeRoot.  These have the following meanings:

		fcgePre:  *pkid is valid; haven't traversed the node's children yet
		fcgePost:  *pkid is valid; have already traversed the children (or
			there aren't any children)
		fcgeError:  a memory error occurred; may be set in conjunction with
			other flags
		fcgeRoot:  *pkid is valid (except the chid value); *pckiPar is
			invalid; the node is the root of the enumeration
***************************************************************************/
bool CGE::FNextKid(KID *pkid, CKI *pckiPar, ulong *pgrfcgeOut, ulong grfcgeIn)
{
	AssertThis(0);
	AssertVarMem(pkid);
	AssertNilOrVarMem(pckiPar);
	AssertVarMem(pgrfcgeOut);

	*pgrfcgeOut = fcgeNil;
	switch (_es)
		{
	case esStart:
		// starting the enumeration
		// hit the node on the way down
		*pgrfcgeOut |= fcgePre | fcgeRoot;
		if (_pcfl->Ckid(_dps.kid.cki.ctg, _dps.kid.cki.cno) == 0)
			goto LPost;
		*pkid = _dps.kid;
		_dps.ikid = 0;
		_es = esGo;
		break;

	case esGo:
		if ((grfcgeIn & fcgeSkipToSib) &&
				(_pgldps == pvNil || !_pgldps->FPop(&_dps)))
			{
			goto LDone;
			}
		// fall through
	case esGoNoSkip:
		if (!_pcfl->FGetKid(_dps.kid.cki.ctg, _dps.kid.cki.cno,
				_dps.ikid++, pkid))
			{
LPost:
			// no more children, hit the node on the way up
			*pgrfcgeOut |= fcgePost;
			*pkid = _dps.kid;
			if (_pgldps == pvNil || !_pgldps->FPop(&_dps))
				{
				// this is the root
				*pgrfcgeOut |= fcgeRoot;
				_es = esDone;
				ReleasePpo(&_pgldps);
				}
			else
				{
				_es = esGoNoSkip;
				if (pckiPar != pvNil)
					*pckiPar = _dps.kid.cki;
				}
			break;
			}

		// hit the child
		if (pckiPar != pvNil)
			*pckiPar = _dps.kid.cki;
		if (_pcfl->Ckid(pkid->cki.ctg, pkid->cki.cno) > 0)
			{
			// child has children, need to push the dps
			if (_pgldps == pvNil &&
					(_pgldps = GL::PglNew(size(DPS), 10)) == pvNil ||
				!_pgldps->FPush(&_dps))
				{
				// mem failure, pretend it has no children
				*pgrfcgeOut |= fcgeError;
				goto LNoChildren;
				}
			_dps.kid = *pkid;
			_dps.ikid = 0;
			*pgrfcgeOut |= fcgePre;
			_es = esGo;
			}
		else
			{
LNoChildren:
			// child doesn't have children, just handle it
			*pgrfcgeOut |= fcgePost | fcgePre;
			_es = esGoNoSkip;
			}
		break;

	default:
LDone:
		_es = esDone;
		ReleasePpo(&_pgldps);
		TrashVar(pkid);
		TrashVar(pckiPar);
		TrashVar(pgrfcgeOut);
		return fFalse;
		}

	TrashVarIf((*pgrfcgeOut & fcgeRoot), pckiPar);
	TrashVarIf((*pgrfcgeOut & fcgeRoot), &pkid->chid);
	return fTrue;
}


