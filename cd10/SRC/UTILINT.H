/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Scalar, rectangle and point declarations

***************************************************************************/
#ifndef UTILINT_H
#define UTILINT_H


/****************************************
	Scalar constants
****************************************/
const bool fTrue = 1;
const bool fFalse = 0;
enum
	{
	tNo,
	tYes,
	tMaybe,
	tLim
	};
#define AssertT(t) AssertIn(t, 0, tLim)

// standard comparison flags
enum
	{
	fcmpEq = 0x0001,
	fcmpGt = 0x0002,
	fcmpLt = 0x0004,
	};
const ulong kgrfcmpGe = (fcmpEq | fcmpGt);
const ulong kgrfcmpLe = (fcmpEq | fcmpLt);
const ulong kgrfcmpNe = (fcmpGt | fcmpLt);


#define FPure(f) ((f) != fFalse)
#define ivNil (-1L)
#define bvNil (-1L)
#define cvNil (-1L)
#define pvNil 0


/****************************************
	Memory access asserts
****************************************/
#ifdef DEBUG
void AssertPvCb(void *pv, long cb);
inline void AssertNilOrPvCb(void *pv, long cb)
	{ if (pv != pvNil) AssertPvCb(pv, cb); }
#else //!DEBUG
#define AssertPvCb(pv, cb)
#define AssertNilOrPvCb(pv, cb)
#endif //!DEBUG

#define AssertThisMem() AssertPvCb(this, size(*this))
#define AssertVarMem(pvar) AssertPvCb(pvar, size(*(pvar)))
#define AssertNilOrVarMem(pvar) AssertNilOrPvCb(pvar, size(*(pvar)))


/****************************************
	Scalar APIs
****************************************/
inline bool FIn(long lw, long lwMin, long lwLim)
	{ return lw >= lwMin && lw < lwLim; }
inline long LwBound(long lw, long lwMin, long lwMax)
	{ return lw < lwMin ? lwMin : lw >= lwMax ? lwMax - 1 : lw; }
void SortLw(long *plw1, long *plw2);

inline short SwHigh(long lw)
	{ return (short)(lw >> 16); }
inline short SwLow(long lw)
	{ return (short)lw; }
inline long LwHighLow(short swHigh, short swLow)
	{ return ((long)swHigh << 16) | (long)(ushort)swLow; }
inline ulong LuHighLow(ushort suHigh, ushort suLow)
	{ return ((ulong)suHigh << 16) | (ulong)suLow; }
inline byte B0Lw(long lw)
	{ return (byte)lw; }
inline byte B1Lw(long lw)
	{ return (byte)(lw >> 8); }
inline byte B2Lw(long lw)
	{ return (byte)(lw >> 16); }
inline byte B3Lw(long lw)
	{ return (byte)(lw >> 24); }
inline long LwFromBytes(byte b3, byte b2, byte b1, byte b0)
	{ return ((long)b3 << 24) | ((long)b2 << 16) | ((long)b1 << 8) | (long)b0; }

inline ushort SuHigh(long lw)
	{ return (ushort)((ulong)lw >> 16); }
inline ushort SuLow(long lw)
	{ return (ushort)lw; }

inline byte BHigh(short sw)
	{ return (byte)((ushort)sw >> 8); }
inline byte BLow(short sw)
	{ return (byte)sw; }
inline short SwHighLow(byte bHigh, byte bLow)
	{ return ((short)bHigh << 8) | (short)bLow; }
inline ushort SuHighLow(byte bHigh, byte bLow)
	{ return ((ushort)bHigh << 8) | (ushort)bLow; }

inline short SwTruncLw(long lw)
	{ return lw <= kswMax ? (lw >= kswMin ? (short)lw : kswMin) : kswMax; }
inline short SwMin(short sw1, short sw2)
	{ return sw1 < sw2 ? sw1 : sw2; }
inline short SwMax(short sw1, short sw2)
	{ return sw1 >= sw2 ? sw1 : sw2; }
inline ushort SuMin(ushort su1, ushort su2)
	{ return su1 < su2 ? su1 : su2; }
inline ushort SuMax(ushort su1, ushort su2)
	{ return su1 >= su2 ? su1 : su2; }
inline long LwMin(long lw1, long lw2)
	{ return lw1 < lw2 ? lw1 : lw2; }
inline long LwMax(long lw1, long lw2)
	{ return lw1 >= lw2 ? lw1 : lw2; }
inline ulong LuMin(ulong lu1, ulong lu2)
	{ return lu1 < lu2 ? lu1 : lu2; }
inline ulong LuMax(ulong lu1, ulong lu2)
	{ return lu1 >= lu2 ? lu1 : lu2; }

inline short SwAbs(short sw)
	{ return sw < 0 ? -sw : sw; }
inline long LwAbs(long lw)
	{ return lw < 0 ? -lw : lw; }

inline long LwMulSw(short sw1, short sw2)
	{ return (long)sw1 * sw2; }


#ifdef MC_68020

/***************************************************************************
	Motorola 68020 routines.
***************************************************************************/
extern "C"
	{
	long __cdecl LwMulDiv(long lw, long lwMul, long lwDiv);
	void __cdecl MulLw(long lw1, long lw2, long *plwHigh, ulong *pluLow);
	ulong __cdecl LuMulDiv(ulong lu, ulong luMul, ulong luDiv);
	void __cdecl MulLu(ulong lu1, ulong lu2, ulong *pluHigh, ulong *pluLow);
	}
long LwMulDivMod(long lw, long lwMul, long lwDiv, long *plwRem);

#elif defined(IN_80386)

/***************************************************************************
	Intel 80386 routines.
***************************************************************************/
inline long LwMulDiv(long lw, long lwMul, long lwDiv)
	{
	AssertH(lwDiv != 0);
	__asm
		{
		mov		eax,lw
		imul	lwMul
		idiv	lwDiv
		mov		lw,eax
		}
	return lw;
	}

inline long LwMulDivMod(long lw, long lwMul, long lwDiv, long *plwRem)
	{
	AssertH(lwDiv != 0);
	AssertVarMem(plwRem);
	__asm
		{
		mov		eax,lw
		imul	lwMul
		idiv	lwDiv
		mov		ecx,plwRem
		mov		DWORD PTR[ecx],edx
		mov		lw,eax
		}
	return lw;
	}

void MulLw(long lw1, long lw2, long *plwHigh, ulong *pluLow);
ulong LuMulDiv(ulong lu, ulong luMul, ulong luDiv);
void MulLu(ulong lu1, ulong lu2, ulong *pluHigh, ulong *pluLow);

#else //!MC_68020 && !IN_80386

/***************************************************************************
	Other processors.  These generally use floating point.
***************************************************************************/
long LwMulDiv(long lw, long lwMul, long lwDiv);
long LwMulDivMod(long lw, long lwMul, long lwDiv, long *plwRem);
void MulLw(long lw1, long lw2, long *plwHigh, ulong *pluLow);
ulong LuMulDiv(ulong lu, ulong luMul, ulong luDiv);
void MulLu(ulong lu1, ulong lu2, ulong *pluHigh, ulong *pluLow);

#endif //!MC_68020 && !IN_80386

long LwMulDivAway(long lw, long lwMul, long lwDiv);
ulong LuMulDivAway(ulong lu, ulong luMul, ulong luDiv);

ulong FcmpCompareFracs(long lwNum1, long lwDen1, long lwNum2, long lwDen2);


long LwDivAway(long lwNum, long lwDen);
long LwDivClosest(long lwNum, long lwDen);
long LwRoundAway(long lwSrc, long lwBase);
long LwRoundToward(long lwSrc, long lwBase);
long LwRoundClosest(long lwSrc, long lwBase);

inline long CbRoundToLong(long cb)
	{ return (cb + size(long) - 1) & ~(long)(size(long) - 1); }
inline long CbRoundToShort(long cb)
	{ return (cb + size(short) - 1) & ~(long)(size(short) - 1); }
inline long CbFromCbit(long cbit)
	{ return (cbit + 7) / 8; }
inline byte Fbit(long ibit)
	{ return 1 << (ibit & 0x0007); }
inline long IbFromIbit(long ibit)
	{ return ibit >> 3; }

long LwGcd(long lw1, long lw2);
ulong LuGcd(ulong lu1, ulong lu2);


bool FAdjustIv(long *piv, long iv, long cvIns, long cvDel);

#ifdef DEBUG
void AssertIn(long lw, long lwMin, long lwLim);
long LwMul(long lw1, long lw2);
#else //!DEBUG
#define AssertIn(lw, lwMin, lwLim)
inline long LwMul(long lw1, long lw2)
	{ return lw1 * lw2; }
#endif //!DEBUG


/****************************************
	Byte Swapping
****************************************/

//byte order mask
typedef ulong BOM;

void SwapBytesBom(void *pv, BOM bom);
void SwapBytesRgsw(void *psw, long csw);
void SwapBytesRglw(void *plw, long clw);

const BOM bomNil = 0;
const BOM kbomSwapShort =  0x40000000;
const BOM kbomSwapLong =   0xC0000000;
const BOM kbomLeaveShort = 0x00000000;
const BOM kbomLeaveLong =  0x80000000;

/* You can chain up to 16 of these (2 bits each) */
#define BomField(bomNew, bomLast) ((bomNew) | ((bomLast) >> 2))

#ifdef DEBUG
void AssertBomRglw(BOM bom, long cb);
void AssertBomRgsw(BOM bom, long cb);
#else //!DEBUG
#define AssertBomRglw(bom, cb)
#define AssertBomRgsw(bom, cb)
#endif //!DEBUG


/****************************************
	OS level rectangle and point
****************************************/

#ifdef MAC
typedef Rect RCS;
typedef Point PTS;
#elif defined(WIN)
typedef RECT RCS;
typedef POINT PTS;
#endif //WIN


/****************************************
	Rectangle and point stuff
****************************************/
// options for PT::Transform and RC::Transform
enum
	{
	fptNil,
	fptNegateXp = 1,	// negate xp values (and swap them in an RC)
	fptNegateYp = 2,	// negate yp values (and swap them in an RC)
	fptTranspose = 4,	// swap xp and yp values (done after negating)
	};

class RC;
typedef class RC *PRC;

class PT
	{
public:
	long xp;
	long yp;

public:
	//constructors
	PT(void) {}
	PT(long xpT, long ypT) { xp = xpT, yp = ypT; }

	//for assigning to/from a PTS
	operator PTS(void);
	PT & operator = (PTS &pts);
	PT(PTS &pts)
		{ *this = pts; }

	//interaction with other points
	bool operator==(PT &pt)
		{ return xp == pt.xp && yp == pt.yp; }
	bool operator!=(PT &pt)
		{ return xp != pt.xp || yp != pt.yp; }
	PT operator+(PT &pt)
		{ return PT(xp + pt.xp, yp + pt.yp); }
	PT operator-(PT &pt)
		{ return PT(xp - pt.xp, yp - pt.yp); }
	PT & operator+=(PT &pt)
		{ xp += pt.xp; yp += pt.yp; return *this; }
	PT & operator-=(PT &pt)
		{ xp -= pt.xp; yp -= pt.yp; return *this; }
	void Offset(long dxp, long dyp)
		{ xp += dxp; yp += dyp; }

	//map the point from prcSrc to prcDst coordinates
	void Map(RC *prcSrc, RC *prcDst);
	PT PtMap(RC *prcSrc, RC *prcDst);

	void Transform(ulong grfpt);
	};

class RC
	{
public:
	long xpLeft;
	long ypTop;
	long xpRight;
	long ypBottom;

public:
	//constructors
	RC(void) {}
	RC(long xpLeftT, long ypTopT, long xpRightT, long ypBottomT)
		{
		AssertThisMem();
		xpLeft = xpLeftT;
		ypTop = ypTopT;
		xpRight = xpRightT;
		ypBottom = ypBottomT;
		}

	//for assigning to/from an RCS
	operator RCS(void);
	RC & operator = (RCS &rcs);
	RC(RCS &rcs)
		{ *this = rcs; }

	void Zero(void)
		{ AssertThisMem(); xpLeft = ypTop = xpRight = ypBottom = 0; }
	void Set(long xp1, long yp1, long xp2, long yp2)
		{
		AssertThisMem();
		xpLeft = xp1;
		ypTop = yp1;
		xpRight = xp2;
		ypBottom = yp2;
		}
	// use klwMin / 2 and klwMax / 2 so Dxp and Dyp are correct
	void Max(void)
		{
		AssertThisMem();
		xpLeft = ypTop = klwMin / 2;
		xpRight = ypBottom = klwMax / 2;
		}
	bool FMax(void)
		{
		AssertThisMem();
		return xpLeft == klwMin / 2 && ypTop == klwMin / 2 &&
			xpRight == klwMax / 2 && ypBottom == klwMax / 2;
		}

	//interaction with other rc's and pt's
	bool operator==(RC &rc);
	bool operator!=(RC &rc);
	RC & operator+=(PT &pt)
		{
		xpLeft += pt.xp;
		ypTop += pt.yp;
		xpRight += pt.xp;
		ypBottom += pt.yp;
		return *this;
		}
	RC & operator-=(PT &pt)
		{
		xpLeft -= pt.xp;
		ypTop -= pt.yp;
		xpRight -= pt.xp;
		ypBottom -= pt.yp;
		return *this;
		}
	RC operator+(PT &pt)
		{
		return RC(xpLeft + pt.xp, ypTop + pt.yp,
			xpRight + pt.xp, ypBottom + pt.yp);
		}
	RC operator-(PT &pt)
		{
		return RC(xpLeft - pt.xp, ypTop - pt.yp,
			xpRight - pt.xp, ypBottom - pt.yp);
		}
	PT PtTopLeft(void)
		{ return PT(xpLeft, ypTop); }
	PT PtBottomRight(void)
		{ return PT(xpRight, ypBottom); }
	PT PtTopRight(void)
		{ return PT(xpRight, ypTop); }
	PT PtBottomLeft(void)
		{ return PT(xpLeft, ypBottom); }

	//map the rectangle from prcSrc to prcDst coordinates
	void Map(RC *prcSrc, RC *prcDst);

	void Transform(ulong grfpt);

	long Dxp(void)
		{ AssertThisMem(); return xpRight - xpLeft; }
	long Dyp(void)
		{ AssertThisMem(); return ypBottom - ypTop; }
	long XpCenter(void)
		{ AssertThisMem(); return (xpLeft + xpRight) / 2; }
	long YpCenter(void)
		{ AssertThisMem(); return (ypTop + ypBottom) / 2; }
	bool FEmpty(void)
		{  AssertThisMem(); return ypBottom <= ypTop || xpRight <= xpLeft; }

	void CenterOnRc(RC *prcBase);
	void CenterOnPt(long xp, long yp);
	bool FIntersect(RC *prc1, RC *prc2);
	bool FIntersect(RC *prc);
	bool FPtIn(long xp, long yp);
	void InsetCopy(RC *prc, long dxp, long dyp);
	void Inset(long dxp, long dyp);
	void OffsetCopy(RC *prc, long dxp, long dyp);
	void Offset(long dxp, long dyp);
	void OffsetToOrigin(void);
	void PinPt(PT *ppt);
	void PinToRc(RC *prc);
	void SqueezeIntoRc(RC *prcBase);
	void StretchToRc(RC *prcBase);
	void Union(RC *prc1, RC *prc2);
	void Union(RC *prc);
	long LwArea(void);
	bool FContains(RC *prc);
	void SetToCell(RC *prcSrc, long crcWidth, long crcHeight,
		long ircWidth, long ircHeight);
	bool FMapToCell(long xp, long yp, long crcWidth, long crcHeight,
		long *pircWidth, long *pircHeight);
	};


/****************************************
	fractions (ratio/rational)
****************************************/
class RAT
	{
	ASSERT

private:
	long _lwNum;
	long _lwDen;

	//the third argument of this constructor is bogus.  This constructor is
	//provided so the GCD calculation can be skipped when we already know
	//the numerator and denominator are relatively prime.
	RAT(long lwNum, long lwDen, long lwJunk)
		{
		//lwNum and lwDen are already relatively prime
		if (lwDen > 0)
			{
			_lwNum = lwNum;
			_lwDen = lwDen;
			}
		else
			{
			_lwNum = -lwNum;
			_lwDen = -lwDen;
			}
		AssertThis(0);
		}
public:
	//constructors
	RAT(void) { _lwDen = 0; }
	RAT(long lw) { _lwNum = lw; _lwDen = 1; }
	RAT(long lwNum, long lwDen)
		{ Set(lwNum, lwDen); }
	void Set(long lwNum, long lwDen)
		{
		long lwGcd = LwGcd(lwNum, lwDen);
		if (lwDen < 0)
			lwGcd = -lwGcd;
		_lwNum = lwNum / lwGcd;
		_lwDen = lwDen / lwGcd;
		AssertThis(0);
		}

	//unary minus
	RAT operator - (void) const
		{ return RAT(-_lwNum, _lwDen, 0); }

	//access functions
	long LwNumerator(void) { return _lwNum; }
	long LwDenominator(void) { return _lwDen; }
	long LwAway(void) { return LwDivAway(_lwNum, _lwDen); }
	long LwToward(void) { return _lwNum / _lwDen; }
	long LwClosest(void) { return LwDivClosest(_lwNum, _lwDen); }

	operator long(void)
		{ return _lwNum / _lwDen; }

	//applying to a long (as a multiplicative operator)
	long LwScale(long lw)
		{ return (_lwNum != _lwDen) ? LwMulDiv(lw, _lwNum, _lwDen) : lw; }
	long LwUnscale(long lw)
		{ return (_lwNum != _lwDen) ? LwMulDiv(lw, _lwDen, _lwNum) : lw; }

	//operator functions
	friend RAT operator + (const RAT &rat1, const RAT &rat2);
	friend RAT operator + (const RAT &rat, long lw)
		{ return RAT(rat._lwNum + LwMul(rat._lwDen, lw), rat._lwDen); }
	friend RAT operator + (long lw, const RAT &rat)
		{ return RAT(rat._lwNum + LwMul(rat._lwDen, lw), rat._lwDen); }

	friend RAT operator - (const RAT &rat1, const RAT &rat2)
		{ return rat1 + (-rat2); }
	friend RAT operator - (const RAT &rat, long lw)
		{ return RAT(rat._lwNum + LwMul(rat._lwDen, -lw), rat._lwDen); }
	friend RAT operator - (long lw, const RAT &rat)
		{ return RAT(rat._lwNum + LwMul(rat._lwDen, -lw), rat._lwDen); }

	friend RAT operator * (const RAT &rat1, const RAT &rat2)
		{
		long lwGcd1 = LwGcd(rat1._lwNum, rat2._lwDen);
		long lwGcd2 = LwGcd(rat1._lwDen, rat2._lwNum);
		return RAT(LwMul(rat1._lwNum / lwGcd1, rat2._lwNum / lwGcd2),
			LwMul(rat1._lwDen / lwGcd2, rat2._lwDen / lwGcd1), 0);
		}
	friend RAT operator * (const RAT &rat, long lw)
		{
		long lwGcd = LwGcd(rat._lwDen, lw);
		return RAT(LwMul(lw / lwGcd, rat._lwNum), rat._lwDen / lwGcd, 0);
		}
	friend RAT operator * (long lw, const RAT &rat)
		{
		long lwGcd = LwGcd(rat._lwDen, lw);
		return RAT(LwMul(lw / lwGcd, rat._lwNum), rat._lwDen / lwGcd, 0);
		}

	friend RAT operator / (const RAT &rat1, const RAT &rat2)
		{
		long lwGcd1 = LwGcd(rat1._lwNum, rat2._lwNum);
		long lwGcd2 = LwGcd(rat1._lwDen, rat2._lwDen);
		return RAT(LwMul(rat1._lwNum / lwGcd1, rat2._lwDen / lwGcd2),
			LwMul(rat1._lwDen / lwGcd2, rat2._lwNum / lwGcd1), 0);
		}
	friend RAT operator / (const RAT &rat, long lw)
		{
		long lwGcd = LwGcd(rat._lwNum, lw);
		return RAT(rat._lwNum / lwGcd, LwMul(lw / lwGcd, rat._lwDen), 0);
		}
	friend RAT operator / (long lw, const RAT &rat)
		{
		long lwGcd = LwGcd(rat._lwNum, lw);
		return RAT(LwMul(lw / lwGcd, rat._lwDen), rat._lwNum / lwGcd, 0);
		}

	friend int operator == (const RAT &rat1, const RAT &rat2)
		{ return rat1._lwNum == rat2._lwNum && rat1._lwDen == rat2._lwDen; }
	friend int operator == (const RAT &rat, long lw)
		{ return rat._lwDen == 1 && rat._lwNum == lw; }
	friend int operator == (long lw, const RAT &rat)
		{ return rat._lwDen == 1 && rat._lwNum == lw; }

	friend int operator != (const RAT &rat1, const RAT &rat2)
		{ return rat1._lwNum != rat2._lwNum || rat1._lwDen != rat2._lwDen; }
	friend int operator != (const RAT &rat, long lw)
		{ return rat._lwDen != 1 || rat._lwNum != lw; }
	friend int operator != (long lw, const RAT &rat)
		{ return rat._lwDen != 1 || rat._lwNum != lw; }

	//operator methods
	RAT & operator = (long lw)
		{
		_lwNum = lw;
		_lwDen = 1;
		return *this;
		}

	RAT & operator += (const RAT &rat)
		{ *this = *this + rat; return *this; }
	RAT & operator += (long lw)
		{ *this = *this + lw; return *this; }

	RAT & operator -= (const RAT &rat)
		{ *this = *this - rat; return *this; }
	RAT & operator -= (long lw)
		{ *this = *this + (-lw); return *this; }

	RAT & operator *= (const RAT &rat)
		{ *this = *this * rat; return *this; }
	RAT & operator *= (long lw)
		{ *this = *this * lw; return *this; }

	RAT & operator /= (const RAT &rat)
		{ *this = *this / rat; return *this; }
	RAT & operator /= (long lw)
		{ *this = *this / lw; return *this; }
	};


/***************************************************************************
	Data versioning utility
***************************************************************************/
struct DVER
	{
	short _swCur;
	short _swBack;

	void Set(short swCur, short swBack);
	bool FReadable(short swCur, short swMin);
	};


#endif //UTILINT_H

