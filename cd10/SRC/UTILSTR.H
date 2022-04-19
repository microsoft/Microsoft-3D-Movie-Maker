/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	String manipulation declarations.

	We use four types of strings: stn, stz, st, sz.  Unless there's good reason
	not to, all code should use stn's.

	sz - zero terminated (standard C) string.
	st - length byte prefixed (standard Pascal) string.
	stz - zero terminated and length prefixed string.
	stn - string class.

***************************************************************************/
#ifndef UTILSTR_H
#define UTILSTR_H


/***************************************************************************
	OS kind for string translation - these should always have their high
	and low bytes equal.
***************************************************************************/
const short oskNil = 0; //signifies unknown
const short koskSbMac = 0x0202;
const short koskSbWin = 0x0303;
const short koskUniMac = 0x0404;	// big endian unicode
const short koskUniWin = 0x0505;	// little endian unicode

#ifdef UNICODE
const short koskMac = koskUniMac;
const short koskWin = koskUniWin;
#else //!UNICODE
const short koskMac = koskSbMac;
const short koskWin = koskSbWin;
#endif //!UNICODE
const short koskCur = MacWin(koskMac, koskWin);
const short koskSb = MacWin(koskSbMac, koskSbWin);
const short koskUni = MacWin(koskUniMac, koskUniWin);

#ifdef DEBUG
void AssertOsk(short osk);
#else //!DEBUG
#define AssertOsk(osk)
#endif //!DEBUG


/***************************************************************************
	Return the number of bytes a character occupies in the given osk.
***************************************************************************/
inline long CbCharOsk(short osk)
	{
	switch (osk)
		{
	case koskSbMac:
	case koskSbWin:
		return size(schar);

	case koskUniMac:
	case koskUniWin:
		return size(wchar);

	default:
		return 0;
		}
	}


/***************************************************************************
	Constants
***************************************************************************/
const achar chNil = ChLit('\x0');
const achar kchReturn = ChLit('\xD');
const achar kchLineFeed = ChLit('\xA');
const achar kchTab = ChLit('\x9');
const achar kchSpace = ChLit(' ');
const wchar kchwUnicode = 0xFEFF;
const wchar kchwUnicodeSwap = 0xFFFE;

const long kcchMaxSz = 255;
const long kcchMaxSt = 255;
const long kcchMaxStz = 255;
const long kcchExtraSz = 1;
const long kcchExtraSt = 1;
const long kcchExtraStz = 2;
const long kcchTotSz = kcchMaxSz + kcchExtraSz;
const long kcchTotSt = kcchMaxSt + kcchExtraSt;
const long kcchTotStz = kcchMaxStz + kcchExtraStz;

const long kcchMaxStn = 255;
const long kcbMaxDataStn = kcchTotStz * size(wchar) + size(short);


enum
	{
	fstnNil = 0,
	fstnIgnoreCase,
	};


/***************************************************************************
	String types
***************************************************************************/
typedef achar *PSZ;
typedef achar *PST;
typedef achar *PSTZ;
typedef achar SZ[kcchTotSz];
typedef achar ST[kcchTotSt];
typedef achar STZ[kcchTotStz];
typedef schar *PSZS;
typedef schar SZS[kcchTotSz];


/***************************************************************************
	String related asserts
***************************************************************************/
#ifdef DEBUG
void AssertRgch(achar *prgch, long cch);
void AssertStz(PSTZ pstz);
void AssertSt(PST pst);
void AssertSz(PSZ psz);
void AssertNilOrSz(PSZ psz);
#else
#define AssertRgch(prgch, cch)
#define AssertStz(pstz)
#define AssertSt(pst)
#define AssertSz(psz)
#define AssertNilOrSz(psz)
#endif


/***************************************************************************
	Testing validity of an stz or st
***************************************************************************/
bool FValidStz(PSTZ pstz);
bool FValidSt(PST pst);


/***************************************************************************
	Cch means the number of characters (not including prefix and termination
	bytes) and CchTot means the total number of characters including
	overhead.
***************************************************************************/
long CchSz(PSZ psz);
inline long CchTotSz(PSZ psz)
	{ return CchSz(psz) + kcchExtraSz; }
inline long CchSt(PST pst)
	{ AssertSt(pst); return (long)(byte)pst[0]; }
inline long CchTotSt(PST pst)
	{ AssertSt(pst); return (long)(byte)pst[0] + kcchExtraSt; }
inline long CchStz(PSTZ pstz)
	{ AssertStz(pstz); return (long)(byte)pstz[0]; }
inline long CchTotStz(PSTZ pstz)
	{ AssertStz(pstz); return (long)(byte)pstz[0] + kcchExtraStz; }

inline achar *PrgchSt(PST pst)
	{ return pst + 1; }
inline PSZ PszStz(PSTZ pstz)
	{ return pstz + 1; }


/***************************************************************************
	Byte-wise comparison and sorting.
	WARNING: these don't do normal UI level compares they do byte-wise
	comparison, including any length and/or terminating bytes and should
	be used only for internal sorting (for binary search etc).
***************************************************************************/
bool FEqualRgch(achar *prgch1, long cch1, achar *prgch2, long cch2);
ulong FcmpCompareRgch(achar *prgch1, long cch1, achar *prgch2, long cch2);


/***************************************************************************
	User level (case insensitive, locale aware) comparison and sorting.
***************************************************************************/
bool FEqualUserRgch(achar *prgch1, long cch1, achar *prgch2, long cch2,
	ulong grfstn = fstnIgnoreCase);
ulong FcmpCompareUserRgch(achar *prgch1, long cch1, achar *prgch2, long cch2,
	ulong grfstn = fstnIgnoreCase);


/***************************************************************************
	Upper and lower case utilies
***************************************************************************/
void UpperRgchs(schar *prgchs, long cchs);
void LowerRgchs(schar *prgchs, long cchs);
inline schar ChsUpper(schar chs)
	{ UpperRgchs(&chs, 1); return chs; }
inline schar ChsLower(schar chs)
	{ LowerRgchs(&chs, 1); return chs; }
void UpperRgchw(wchar *prgchw, long cchw);
void LowerRgchw(wchar *prgchw, long cchw);
inline wchar ChwUpper(wchar chw)
	{ UpperRgchw(&chw, 1); return chw; }
inline wchar ChwLower(wchar chw)
	{ LowerRgchw(&chw, 1); return chw; }

#ifdef UNICODE
inline void UpperRgch(achar *prgch, long cch)
	{ UpperRgchw(prgch, cch); }
inline void LowerRgch(achar *prgch, long cch)
	{ LowerRgchw(prgch, cch); }
inline achar ChUpper(achar ch)
	{ UpperRgchw(&ch, 1); return ch; }
inline achar ChLower(achar ch)
	{ LowerRgchw(&ch, 1); return ch; }
#else //!UNICODE
inline void UpperRgch(achar *prgch, long cch)
	{ UpperRgchs(prgch, cch); }
inline void LowerRgch(achar *prgch, long cch)
	{ LowerRgchs(prgch, cch); }
inline achar ChUpper(achar ch)
	{ UpperRgchs(&ch, 1); return ch; }
inline achar ChLower(achar ch)
	{ LowerRgchs(&ch, 1); return ch; }
#endif //!UNICODE


/***************************************************************************
	Translation from one OS to another (eg, Win to Mac, single byte to
	unicode, etc).
***************************************************************************/
long CchTranslateRgb(void *pvSrc, long cbSrc, short oskSrc,
	achar *prgchDst, long cchMaxDst);


/***************************************************************************
	These APIs assert if osk specifies a different sized character than
	koskCur uses.
***************************************************************************/
void TranslateRgch(achar *prgch, long cch, short osk, bool fToCur = fTrue);
inline void TranslateSt(PST pst, short osk, bool fToCur = fTrue)
	{ TranslateRgch(pst + 1, CchSt(pst), osk, fToCur); }
inline void TranslateStz(PSTZ pstz, short osk, bool fToCur = fTrue)
	{ TranslateRgch(pstz + 1, CchStz(pstz), osk, fToCur); }
inline void TranslateSz(PSZ psz, short osk, bool fToCur = fTrue)
	{ TranslateRgch(psz, CchSz(psz), osk, fToCur); }


/***************************************************************************
	Testing for type of character.
***************************************************************************/
enum
	{
	fchNil =			0x00,

	// can overhang the end of a line and doesn't need to be draw
	fchWhiteOverhang =	0x01,

	// should break a line
	fchBreak =			0x02,

	// may break a line
	fchMayBreak =		0x04,

	// should be totally ignored (and not draw)
	fchIgnore = 		0x08,

	// some sort of control character
	fchControl =		0x10,

	// a tab character
	fchTab = 			0x20,
	};

ulong GrfchFromCh(achar ch);


/***************************************************************************
	The hexadecimal digits 0 - 9, A - F.
***************************************************************************/
extern const achar vrgchHex[];


/***************************************************************************
	General string class.
***************************************************************************/
typedef class STN *PSTN;
class STN
	{
	ASSERT

private:
	achar _rgch[kcchMaxStn + 2];

public:
	STN(void)
		{ _rgch[0] = _rgch[1] = 0; AssertThis(0); }
	STN(STN &stnSrc);
	STN(PSZ pszSrc);

	// pointers to the data - these should be considered readonly!
	achar *Prgch(void)
		{ AssertThis(0); return _rgch + 1; }
	PSZ Psz(void)
		{ AssertThis(0); return _rgch + 1; }
	PST Pst(void)
		{ AssertThis(0); return _rgch; }
	PSTZ Pstz(void)
		{ AssertThis(0); return _rgch; }
	long Cch(void)
		{ AssertThis(0); return (uchar)_rgch[0]; }

	// setting the string
	void SetNil(void)
		{ AssertThis(0); _rgch[0] = _rgch[1] = 0; }
	void SetRgch(achar *prgchSrc, long cch);
	void SetSz(PSZ pszSrc)
		{ SetRgch(pszSrc, CchSz(pszSrc)); }
	void SetSt(PST pstSrc)
		{ SetRgch(PrgchSt(pstSrc), CchSt(pstSrc)); }
	void SetStz(PSTZ pstzSrc)
		{ SetRgch(PszStz(pstzSrc), CchStz(pstzSrc)); }
	void SetSzs(PSZS pszsSrc);

	// assignment operators
	STN & operator=(STN &stnSrc);
	STN & operator=(PSZ pszSrc)
		{ SetSz(pszSrc); return *this; }

	// getting the string into a buffer
	void GetRgch(achar *prgchDst)
		{
		AssertThis(0);
		CopyPb(Prgch(), prgchDst, Cch() * size(achar));
		}
	void GetSz(PSZ pszDst)
		{
		AssertThis(0);
		CopyPb(Psz(), pszDst, (Cch() + kcchExtraSz) * size(achar));
		}
	void GetSt(PST pstDst)
		{
		AssertThis(0);
		CopyPb(Pst(), pstDst, (Cch() + kcchExtraSt) * size(achar));
		}
	void GetStz(PSTZ pstzDst)
		{
		AssertThis(0);
		CopyPb(Pstz(), pstzDst, (Cch() + kcchExtraStz) * size(achar));
		}
	void GetSzs(PSZS pszs);

	// modifying the string
	void Delete(long ich, long cch = kcchMaxStn);
	bool FAppendRgch(achar *prgchSrc, long cch);
	bool FAppendCh(achar chSrc)
		{ return FAppendRgch(&chSrc, 1); }
	bool FAppendSz(achar *pszSrc)
		{ return FAppendRgch(pszSrc, CchSz(pszSrc)); }
	bool FAppendStn(PSTN pstnSrc)
		{ return FAppendRgch(pstnSrc->Prgch(), pstnSrc->Cch()); }
	bool FInsertRgch(long ich, achar *prgchSrc, long cch);
	bool FInsertCh(long ich, achar chSrc)
		{ return FInsertRgch(ich, &chSrc, 1); }
	bool FInsertStn(long ich, PSTN pstnSrc)
		{ return FInsertRgch(ich, pstnSrc->Prgch(), pstnSrc->Cch()); }

	// for testing equality
	bool FEqualRgch(achar *prgch, long cch);
	bool FEqualSz(PSZ psz)
		{ return FEqualRgch(psz, CchSz(psz)); }
	bool FEqual(PSTN pstn)
		{ return FEqualRgch(pstn->Prgch(), pstn->Cch()); }
	bool FEqualUserRgch(achar *prgch, long cch, ulong grfstn = fstnIgnoreCase);
	bool FEqualUserSz(PSZ psz, ulong grfstn = fstnIgnoreCase)
		{ return FEqualUserRgch(psz, CchSz(psz), grfstn); }
	bool FEqualUser(PSTN pstn, ulong grfstn = fstnIgnoreCase)
		{ return FEqualUserRgch(pstn->Prgch(), pstn->Cch(), grfstn); }

	// for sorting
	ulong FcmpCompare(PSTN pstn)
		{ return ::FcmpCompareRgch(Prgch(), Cch(), pstn->Prgch(), pstn->Cch()); }
	ulong FcmpCompareUser(PSTN pstn, ulong grfstn = fstnIgnoreCase)
		{
		return ::FcmpCompareUserRgch(Prgch(), Cch(), pstn->Prgch(), pstn->Cch(),
			grfstn);
		}

	// storing and retrieving strings to other buffers (or file).
	long CbData(void);
	void GetData(void *pv);
	bool FSetData(void *pv, long cbMax, long *pcbRead = pvNil);
	bool FWrite(PBLCK pblck, long ib);
	bool FRead(PBLCK pblck, long ib, long *pcbRead = pvNil);

	bool FFormat(PSTN pstnFormat, ...);
	bool FFormatSz(PSZ pszFormat, ...);
	bool FFormatRgch(achar *prgchFormat, long cchFormat, ulong *prgluData);
	bool FGetLw(long *plw, long lwBase = 0);
	bool FExpandControls(void);
	};

#endif //UTILSTR_H

