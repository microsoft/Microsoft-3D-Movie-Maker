/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Memory handling

***************************************************************************/
#ifndef UTILMEM_H
#define UTILMEM_H


// used for asserts and limiting memory
const byte kbGarbage = 0xA3;		// new blocks are filled with this
const long kcbMax = 0x08000000;		// 128 Megabytes
const short kswMagicMem = (short)0xA253;
const long klwMagicMem = (long)0xA253A253;


/***************************************************************************
	When an allocation fails, vpfnlib is called to free some memory (if it's
	not nil).
***************************************************************************/
typedef long (*PFNLIB)(long cb, long mpr);
extern PFNLIB vpfnlib;
extern bool _fInAlloc;


/****************************************
	OS memory handles and management
****************************************/
#ifdef MAC
typedef Handle HN;
// version of SetHandleSize that returns an error code
inline short ErrSetHandleSize(HN hn, Size cb)
	{ SetHandleSize(hn, cb); return MemError(); }

//address stipper
class ADST
	{
private:
	long _lwMaskAddress;

public:
	ADST(void);

	void *PvStrip(void *pv)
		{ return (void *)((long)pv & _lwMaskAddress); }
	};
extern ADST vadst;

#elif defined(WIN)
typedef HGLOBAL HN;
#endif


/****************************************
	Moveable/resizeable memory management
****************************************/
typedef void *HQ;
#define hNil 0
#define hqNil ((HQ)0)

//memory request priority
enum
	{
	//lower priority
	mprDebug,
	mprForSpeed,
	mprNormal,
	mprCritical,
	//higher priority
	};

// memory allocation options
enum
	{
	fmemNil = 0,
	fmemClear = 1,
	};

void FreePhq(HQ *phq);
long CbOfHq(HQ hq);
bool FCopyHq(HQ hqSrc, HQ *phqDst, long mpr);
bool FResizePhq(HQ *phq, long cb, ulong grfmem, long mpr);
void *PvLockHq(HQ hq);
void UnlockHq(HQ hq);


#ifdef DEBUG

// debug memory allocator globals
// enter vmutxMem before modifying these...
struct DMAGL
	{
	long cv;		// number of allocations
	long cvTot;		// total number of allocations over all time
	long cvRun;		// running max of cv
	long cb;		// total size of allocations
	long cbRun;		// running max of cb

	long cactDo;	// number of times to succeed before failing
	long cactFail;	// number of times to fail

	bool FFail(void);
	void Allocate(long cbT);
	void Resize(long dcb);
	void Free(long cbT);
	};


// debug memory globals
struct DMGLOB
	{
	DMAGL dmaglBase;	// for NewObj
	DMAGL dmaglHq;		// for HQs
	DMAGL dmaglPv;		// for FAllocPv, etc
	};
extern DMGLOB vdmglob;


extern long vcactSuspendCheckPointers;
#define SuspendCheckPointers() vcactSuspendCheckPointers++;
#define ResumeCheckPointers() vcactSuspendCheckPointers--;

bool FAllocHqDebug(HQ *phq, long cb, ulong grfmem, long mpr,
	schar *pszsFile, long lwLine);
#define FAllocHq(phq,cb,grfmem,mpr) \
	FAllocHqDebug(phq,cb,grfmem,mpr,__szsFile,__LINE__)
void *QvFromHq(HQ hq);

void AssertHq(HQ hq);
void MarkHq(HQ hq);
#ifdef MAC
void _AssertUnmarkedHqs(void);
void _UnmarkAllHqs(void);
#endif //MAC

#else //!DEBUG

#define FAllocHqDebug(phq,cb,grfmem,mpr,pszsFile,luLine) \
	FAllocHq(phq,cb,grfmem,mpr)
bool FAllocHq(HQ *phq, long cb, ulong grfmem, long mpr);
#ifdef MAC
inline void *QvFromHq(HQ hq)
	{ return vadst.PvStrip(*(void **)hq); }
#elif defined(WIN)
inline void *QvFromHq(HQ hq)
	{ return (void *)hq; }
#endif //WIN

#define AssertHq(hq)
#define MarkHq(hq)

#endif //!DEBUG


/****************************************
	Fixed (non-moveable) memory.
****************************************/
#ifdef DEBUG

// allocation routine
bool FAllocPvDebug(void **ppv, long cb, ulong grfmem, long mpr,
	schar *pszsFile, long lwLine, DMAGL *pdmagl);
#define FAllocPv(ppv,cb,grfmem,mpr) \
	FAllocPvDebug(ppv,cb,grfmem,mpr,__szsFile,__LINE__,&vdmglob.dmaglPv)

// resizing routine - WIN only
#ifdef WIN
bool _FResizePpvDebug(void **ppv, long cbNew, long cbOld,
	ulong grfmem, long mpr, DMAGL *pdmagl);
#endif //WIN

// freeing routine
void FreePpvDebug(void **ppv, DMAGL *pdmagl);
#define FreePpv(ppv) FreePpvDebug(ppv, &vdmglob.dmaglPv)

void AssertPvAlloced(void *pv, long cb);
void AssertUnmarkedMem(void);
void UnmarkAllMem(void);
void MarkPv(void *pv);

#else //!DEBUG

#define SuspendCheckPointers()
#define ResumeCheckPointers()

// allocation routine
#define FAllocPvDebug(ppv,cb,grfmem,mpr,pszsFile,luLine,pdmagl) \
	FAllocPv(ppv,cb,grfmem,mpr)
bool FAllocPv(void **ppv, long cb, ulong grfmem, long mpr);

// resizing routine - WIN only
#ifdef WIN
#define _FResizePpvDebug(ppv,cbNew,cbOld,grfmem,mpr,pdmagl) \
	_FResizePpv(ppv,cbNew,cbOld,grfmem,mpr)
bool _FResizePpv(void **ppv, long cbNew, long cbOld, ulong grfmem, long mpr);
#endif //WIN

// freeing routine
#define FreePpvDebug(ppv,pdmagl) FreePpv(ppv)
void FreePpv(void **ppv);

#define AssertPvAlloced(pv, cb)
#define AssertUnmarkedMem()
#define UnmarkAllMem()
#define MarkPv(pv)
#endif //!DEBUG


/****************************************
	Memory trashing
****************************************/
#ifdef DEBUG

#define TrashVar(pfoo) \
	if (pvNil != (pfoo)) \
		FillPb(pfoo, size(*(pfoo)), kbGarbage); \
	else (void)0
#define TrashVarIf(f,pfoo) \
	if ((f) && pvNil != (pfoo)) \
		FillPb(pfoo, size(*(pfoo)), kbGarbage); \
	else (void)0
#define TrashPvCb(pv,cb) \
	if (pvNil != (pv)) \
		FillPb(pv, cb, kbGarbage); \
	else (void)0
#define TrashPvCbIf(f,pv,cb) \
	if ((f) && pvNil != (pv)) \
		FillPb(pv, cb, kbGarbage); \
	else (void)0

#else //!DEBUG

#define TrashVar(pfoo)
#define TrashVarIf(f, pfoo)
#define TrashPvCb(pv,cb)
#define TrashPvCbIf(f,pv,cb)

#endif //!DEBUG


/****************************************
	Pointer arithmetic
****************************************/
inline void *PvAddBv(void *pv, long bv)
	{ return (byte *)pv + bv; }
inline void *PvSubBv(void *pv, long bv)
	{ return (byte *)pv - bv; }
inline long BvSubPvs(void *pv1, void *pv2)
	{ return (byte *)pv1 - (byte *)pv2; }



/****************************************
	Mutex (critical section) object
****************************************/
typedef class MUTX *PMUTX;
class MUTX
	{
protected:
	#ifdef WIN
		CRITICAL_SECTION _crit;
	#endif //WIN

public:
	MUTX(void)
		{ Win( InitializeCriticalSection(&_crit); ) }
	~MUTX(void)
		{ Win( DeleteCriticalSection(&_crit); ) }

	void Enter(void)
		{ Win( EnterCriticalSection(&_crit); ) }
	void Leave(void)
		{ Win( LeaveCriticalSection(&_crit); ) }
	};


extern MUTX vmutxMem;


/****************************************
	Current thread id
****************************************/
inline long LwThreadCur(void)
	{ return MacWin(0, GetCurrentThreadId()); }

#endif //!UTILMEM_H

