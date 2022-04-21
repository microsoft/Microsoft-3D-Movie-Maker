/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	GFX classes: graphics port (GPT), graphics environment (GNV)

***************************************************************************/
#ifndef GFX_H
#define GFX_H


/****************************************
	Text and fonts.
****************************************/
// DeScription of a Font.
struct DSF
	{
	long onn;		// Font number.
	ulong grfont;	// Font style.
	long dyp;		// Font height in points.
	long tah;		// Horizontal Text Alignment
	long tav;		// Vertical Text Alignment

	ASSERT
	};


// fONT Styles - note that these match the Mac values
enum
	{
	fontNil = 0,
	fontBold = 1,
	fontItalic = 2,
	fontUnderline = 4,
	fontBoxed = 8,
	};


// Horizontal Text Alignment.
enum
	{
	tahLeft,
	tahCenter,
	tahRight,
	tahLim
	};

// Vertical Text Alignment
enum
	{
	tavTop,
	tavCenter,
	tavBaseline,
	tavBottom,
	tavLim
	};


/****************************************
	Font List
****************************************/
const long onnNil = -1;

#ifdef WIN
int CALLBACK _FEnumFont(LOGFONT *plgf, TEXTMETRIC *ptxm, ulong luType,
	LPARAM luParam);
#endif //WIN

#define NTL_PAR BASE
#define kclsNTL 'NTL'
class NTL : public NTL_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(NTL)

private:
	#ifdef WIN
		friend int CALLBACK _FEnumFont(LOGFONT *plgf, TEXTMETRIC *ptxm,
			ulong luType, LPARAM luParam);
	#endif //WIN
	PGST _pgst;
	long _onnSystem;

public:
	NTL(void);
	~NTL(void);

	#ifdef WIN
		HFONT HfntCreate(DSF *pdsf);
	#endif //WIN
	#ifdef MAC
		short FtcFromOnn(long onn);
	#endif //MAC

	bool FInit(void);
	long OnnSystem(void)
		{ return _onnSystem; }
	void GetStn(long onn, PSTN pstn);
	bool FGetOnn(PSTN pstn, long *ponn);
	long OnnMapStn(PSTN pstn, short osk = koskCur);
	long OnnMac(void);
	bool FFixedPitch(long onn);

	#ifdef DEBUG
		bool FValidOnn(long onn);
	#endif //DEBUG
	};
extern NTL vntl;


/****************************************
	Color and pattern
****************************************/
#ifdef WIN
typedef COLORREF SCR;
#elif defined(MAC)
typedef RGBColor SCR;
#endif //!MAC

//NOTE: this matches the Windows RGBQUAD structure
struct CLR
	{
	byte bBlue;
	byte bGreen;
	byte bRed;
	byte bZero;
	};

#ifdef DEBUG
enum
	{
	facrNil,
	facrRgb = 1,
	facrIndex = 2,
	};
#endif //DEBUG

enum
	{
	kbNilAcr = 0,
	kbRgbAcr = 1,
	kbIndexAcr = 0xFE,
	kbSpecialAcr = 0xFF
	};

const ulong kluAcrInvert = 0xFF000000L;
const ulong kluAcrClear  = 0xFFFFFFFFL;

// Abstract ColoR
class ACR
	{
	friend class GPT;
	ASSERT

private:
	ulong _lu;

	#ifdef WIN
		SCR _Scr(void);
	#endif //WIN
	#ifdef MAC
		void _SetFore(void);
		void _SetBack(void);
	#endif //MAC

public:
	ACR(void) { _lu = 0; }
	ACR(CLR clr)
		{ _lu = LwFromBytes(kbRgbAcr, clr.bRed, clr.bGreen, clr.bBlue); }
	void Set(CLR clr)
		{ _lu = LwFromBytes(kbRgbAcr, clr.bRed, clr.bGreen, clr.bBlue); }
	ACR(byte bRed, byte bGreen, byte bBlue)
		{ _lu = LwFromBytes(kbRgbAcr, bRed, bGreen, bBlue); }
	void Set(byte bRed, byte bGreen, byte bBlue)
		{ _lu = LwFromBytes(kbRgbAcr, bRed, bGreen, bBlue); }
	ACR(byte iscr)
		{ _lu = LwFromBytes(kbIndexAcr, 0, 0, iscr); }
	void SetToIndex(byte iscr)
		{ _lu = LwFromBytes(kbIndexAcr, 0, 0, iscr); }
	ACR(bool fClear, bool fIgnored)
		{ _lu = fClear ? kluAcrClear : kluAcrInvert; }
	void SetToClear(void)
		{ _lu = kluAcrClear; }
	void SetToInvert(void)
		{ _lu = kluAcrInvert; }

	void SetFromLw(long lw);
	long LwGet(void) const;
	void GetClr(CLR *pclr);

	bool operator==(const ACR &acr) const
		{ return _lu == acr._lu; }
	bool operator!=(const ACR &acr) const
		{ return _lu != acr._lu; }
	};

#ifdef SYMC
extern ACR kacrBlack;
extern ACR kacrDkGray;
extern ACR kacrGray;
extern ACR kacrLtGray;
extern ACR kacrWhite;
extern ACR kacrRed;
extern ACR kacrGreen;
extern ACR kacrBlue;
extern ACR kacrYellow;
extern ACR kacrCyan;
extern ACR kacrMagenta;
extern ACR kacrClear;
extern ACR kacrInvert;
#else //!SYMC
const ACR kacrBlack(0, 0, 0);
const ACR kacrDkGray(0x3F, 0x3F, 0x3F);
const ACR kacrGray(0x7F, 0x7F, 0x7F);
const ACR kacrLtGray(0xBF, 0xBF, 0xBF);
const ACR kacrWhite(kbMax, kbMax, kbMax);
const ACR kacrRed(kbMax, 0, 0);
const ACR kacrGreen(0, kbMax, 0);
const ACR kacrBlue(0, 0, kbMax);
const ACR kacrYellow(kbMax, kbMax, 0);
const ACR kacrCyan(0, kbMax, kbMax);
const ACR kacrMagenta(kbMax, 0, kbMax);
const ACR kacrClear(fTrue, fTrue);
const ACR kacrInvert(fFalse, fFalse);
#endif //!SYMC


// abstract pattern
struct APT
	{
	byte rgb[8];

	bool operator==(APT &apt)
		{
		return ((long *)rgb)[0] == ((long *)apt.rgb)[0] &&
			((long *)rgb)[1] == ((long *)apt.rgb)[1];
		}
	bool operator!=(APT &apt)
		{
		return ((long *)rgb)[0] != ((long *)apt.rgb)[0] ||
			((long *)rgb)[1] != ((long *)apt.rgb)[1];
		}

	void SetSolidFore(void)
		{ ((long *)rgb)[0] = -1L; ((long *)rgb)[1] = -1L; }
	bool FSolidFore(void)
		{ return (((long *)rgb)[0] & ((long *)rgb)[1]) == -1L; }
	void SetSolidBack(void)
		{ ((long *)rgb)[0] = 0L; ((long *)rgb)[1] = 0L; }
	bool FSolidBack(void)
		{ return (((long *)rgb)[0] | ((long *)rgb)[1]) == 0L; }
	void Invert(void)
		{
		((long *)rgb)[0] = ~((long *)rgb)[0];
		((long *)rgb)[1] = ~((long *)rgb)[1];
		}
	void MoveOrigin(long dxp, long dyp);
	};
extern APT vaptGray;
extern APT vaptLtGray;
extern APT vaptDkGray;


/****************************************
	Polygon structure - designed to be
	compatible with the Mac's
	Polygon.
****************************************/
struct OLY // pOLYgon
	{
	#ifdef MAC
		short cb;		//size of the whole thing
		RCS rcs;		//bounding rectangle
		PTS rgpts[1];

		long Cpts(void) { return (cb - offset(OLY, rgpts[0])) / size(PTS); }
	#else //!MAC
		long cpts;
		PTS rgpts[1];

		long Cpts(void) { return cpts; }
	#endif //!MAC

	ASSERT
	};
const long kcbOlyBase = size(OLY) - size(PTS);


/****************************************
	High level polygon - a GL of PT's.
****************************************/
enum
	{
	fognNil = 0,
	fognAutoClose = 1,
	fognLim
	};

typedef class OGN *POGN;
#define OGN_PAR GL
#define kclsOGN 'OGN'
class OGN : public OGN_PAR
	{
	RTCLASS_DEC

private:
	struct AEI // Add Edge Info.
		{
		PT *prgpt;
		long cpt;
		long iptPenCur;
		PT ptCur;
		POGN pogn;
		long ipt;
		long dipt;
		};
	bool _FAddEdge(AEI *paei);

protected:
	OGN(void);

public:
	PT *PrgptLock(long ipt = 0)
		{ return (PT *)PvLock(ipt); }
	PT *QrgptGet(long ipt = 0)
		{ return (PT *)QvGet(ipt); }

	POGN PognTraceOgn(POGN pogn, ulong grfogn);
	POGN PognTraceRgpt(PT *prgpt, long cpt, ulong grfogn);

	// static methods
	static POGN PognNew(long cvInit = 0);
	};


long IptFindLeftmost(PT *prgpt, long cpt, long dxp, long dyp);


/****************************************
	Graphics drawing data - a parameter
	to drawing apis in the GPT class
****************************************/
enum
	{
	fgddNil = 0,
	fgddFill = fgddNil,
	fgddFrame = 1,
	fgddPattern = 2,
	fgddAutoClose = 4,
	};

//graphics drawing data
struct GDD
	{
	ulong grfgdd;	//what to do
	APT apt;		//pattern to use
	ACR acrFore;	//foreground color (used for solid fills also)
	ACR acrBack;	//background color
	long dxpPen;	//pen width (used if framing)
	long dypPen;	//pen height
	RCS *prcsClip;	//clipping (may be pvNil)
	};


/****************************************
	Graphics environment
****************************************/
#define GNV_PAR BASE
#define kclsGNV 'GNV'
class GNV : public GNV_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

private:
	PGPT _pgpt;		//the port

	//coordinate mapping
	RC _rcSrc;
	RC _rcDst;

	//current pen location and clipping
	long _xp;
	long _yp;
	RCS _rcsClip;
	RC _rcVis;		//always clipped to - this is in Dst coordinates

	// Current font
	DSF _dsf;

	//contains the current pen size and prcsClip
	//this is passed to the GPT
	GDD _gdd;

	void _Init(PGPT pgpt);
	bool _FMapRcRcs(RC *prc, RCS *prcs);
	void _MapPtPts(long xp, long yp, PTS *ppts);
	HQ _HqolyCreate(POGN pogn, ulong grfogn);
	HQ _HqolyFrame(POGN pogn, ulong grfogn);

	// transition related methods
	bool _FInitPaletteTrans(PGL pglclr, PGL *ppglclrOld, PGL *ppglclrTrans,
		long cbitPixel = 0);
	void _PaletteTrans(PGL pglclrOld, PGL pglclrNew, long lwNum, long lwDen,
		PGL pglclrTrans, CLR *pclrSub = pvNil);
	bool _FEnsureTempGnv(PGNV *ppgnv, RC *prc);

public:
	GNV(PGPT pgpt);
	GNV(PGOB pgob);
	GNV(PGOB pgob, PGPT pgpt);
	~GNV(void);

	void SetGobRc(PGOB pgob);
	PGPT Pgpt(void)
		{ return _pgpt; }
	#ifdef MAC
		void Set(void);
		void Restore(void);
	#endif //MAC
	#ifdef WIN
		// this gross API is for AVI playback
		void DrawDib(HDRAWDIB hdd, BITMAPINFOHEADER *pbi, RC *prc);
	#endif //WIN

	void SetPenSize(long dxp, long dyp);

	void FillRcApt(RC *prc, APT *papt, ACR acrFore, ACR acrBack);
	void FillRc(RC *prc, ACR acr);
	void FrameRcApt(RC *prc, APT *papt, ACR acrFore, ACR acrBack);
	void FrameRc(RC *prc, ACR acr);
	void HiliteRc(RC *prc, ACR acrBack);

	void FillOvalApt(RC *prc, APT *papt, ACR acrFore, ACR acrBack);
	void FillOval(RC *prc, ACR acr);
	void FrameOvalApt(RC *prc, APT *papt, ACR acrFore, ACR acrBack);
	void FrameOval(RC *prc, ACR acr);

	void FillOgnApt(POGN pogn, APT *papt, ACR acrFore, ACR acrBack);
	void FillOgn(POGN pogn, ACR acr);
	void FrameOgnApt(POGN pogn, APT *papt, ACR acrFore, ACR acrBack);
	void FrameOgn(POGN pogn, ACR acr);
	void FramePolyLineApt(POGN pogn, APT *papt, ACR acrFore, ACR acrBack);
	void FramePolyLine(POGN pogn, ACR acr);

	void MoveTo(long xp, long yp)
		{ _xp = xp; _yp = yp; }
	void MoveRel(long dxp, long dyp)
		{ _xp += dxp; _yp += dyp; }
	void LineToApt(long xp, long yp, APT *papt, ACR acrFore, ACR acrBack)
		{ LineApt(_xp, _yp, xp, yp, papt, acrFore, acrBack); }
	void LineTo(long xp, long yp, ACR acr)
		{ Line(_xp, _yp, xp, yp, acr); }
	void LineRelApt(long dxp, long dyp, APT *papt, ACR acrFore, ACR acrBack)
		{ LineApt(_xp, _yp, _xp + dxp, _yp + dyp, papt, acrFore, acrBack); }
	void LineRel(long dxp, long dyp, ACR acr)
		{ Line(_xp, _yp, _xp + dxp, _yp + dyp, acr); }
	void LineApt(long xp1, long yp1, long xp2, long yp2, APT *papt,
		ACR acrFore, ACR acrBack);
	void Line(long xp1, long yp1, long xp2, long yp2, ACR acr);

	void ScrollRc(RC *prc, long dxp, long dyp,
		RC *prc1 = pvNil, RC *prc2 = pvNil);
	static void GetBadRcForScroll(RC *prc, long dxp, long dyp,
		RC *prc1, RC *prc2);

	//for mapping
	void GetRcSrc(RC *prc);
	void SetRcSrc(RC *prc);
	void GetRcDst(RC *prc);
	void SetRcDst(RC *prc);
	void SetRcVis(RC *prc);
	void IntersectRcVis(RC *prc);

	//set clipping
	void ClipRc(RC *prc);
	void ClipToSrc(void);

	// Text & font.
	void SetFont(long onn, ulong grfont, long dypFont,
		long tah = tahLeft, long tav = tavTop);
	void SetOnn(long onn);
	void SetFontStyle(ulong grfont);
	void SetFontSize(long dyp);
	void SetFontAlign(long tah, long tav);
	void GetDsf(DSF *pdsf);
	void SetDsf(DSF *pdsf);
	void DrawRgch(achar *prgch, long cch, long xp, long yp,
		ACR acrFore = kacrBlack, ACR acrBack = kacrClear);
	void DrawStn(PSTN pstn, long xp, long yp,
		ACR acrFore = kacrBlack, ACR acrBack = kacrClear);
	void GetRcFromRgch(RC *prc, achar *prgch, long cch,
		long xp = 0, long yp = 0);
	void GetRcFromStn(RC *prc, PSTN pstn, long xp = 0, long yp = 0);

	//bitmaps and pictures
	void CopyPixels(PGNV pgnvSrc, RC *prcSrc, RC *prcDst);
	void DrawPic(PPIC ppic, RC *prc);
	void DrawMbmp(PMBMP pmbmp, long xp, long yp);
	void DrawMbmp(PMBMP pmbmp, RC *prc);

	// transitions
	void Wipe(long gfd, ACR acrFill, PGNV pgnvSrc, RC *prcSrc, RC *prcDst,
		ulong dts, PGL pglclr = pvNil);
	void Slide(long gfd, ACR acrFill, PGNV pgnvSrc, RC *prcSrc, RC *prcDst,
		ulong dts, PGL pglclr = pvNil);
	void Dissolve(long crcWidth, long crcHeight, ACR acrFill,
		PGNV pgnvSrc, RC *prcSrc, RC *prcDst, ulong dts, PGL pglclr = pvNil);
	void Fade(long cactMax, ACR acrFade, PGNV pgnvSrc, RC *prcSrc, RC *prcDst,
		ulong dts, PGL pglclr = pvNil);
	void Iris(long gfd, long xp, long yp, ACR acrFill,
		PGNV pgnvSrc, RC *prcSrc, RC *prcDst, ulong dts, PGL pglclr = pvNil);
	};


// palette setting options
enum
	{
	fpalNil = 0,
	fpalIdentity = 1,	// make this an identity palette
	fpalInitAnim = 2,	// make the palette animatable
	fpalAnimate = 4,	// animate the current palette with these colors
	};


/****************************************
	Graphics port
****************************************/
#define GPT_PAR BASE
#define kclsGPT 'GPT'
class GPT : public GPT_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

private:
	PREGN _pregnClip;
	RC _rcClip;
	PT _ptBase;		//coordinates assigned to top-left of the GPT

	#ifdef WIN
		#ifdef DEBUG
			static bool _fFlushGdi;
		#endif
		static HPAL _hpal;
		static HPAL _hpalIdentity;
		static CLR *_prgclr;
		static long _cclrPal;
		static long _cactPalCur;
		static long _cactFlush;
		static bool _fPalettized;	// whether the screen is palettized

		HDC _hdc;
		HWND _hwnd;
		HBMP _hbmp;			// nil if not an offscreen port
		byte *_prgbPixels;	// nil if not a dib section port
		long _cbitPixel;
		long _cbRow;
		RC _rcOff;			// bounding rectangle for a metafile or dib port
		long _cactPal;		// which palette this port has selected
		long _cactDraw;		// last draw - for knowing when to call GdiFlush
		long _cactLock;		// lock count

		//selected brush and its related info
		enum //brush kind
			{
			bkNil,
			bkApt,
			bkAcr,
			bkStock
			};
		HBRUSH _hbr;
		long _bk;
		APT _apt;	//for bkApt
		ACR _acr;	//for bkAcr
		int _wType;	//for bkStock (stock brush)

		HFONT _hfnt;
		DSF _dsf;

		bool _fNewClip: 1;		// _pregnClip has changed
		bool _fMetaFile: 1;
		bool _fMapIndices: 1;	// SelectPalette failed, map indices to RGBs
		bool _fOwnPalette: 1;	// this offscreen has its own palette

		void _SetClip(RCS *prcsClip);
		void _EnsurePalette(void);
		void _SetTextProps(DSF *pdsf);
		void _SetAptBrush(APT *papt);
		void _SetAcrBrush(ACR acr);
		void _SetStockBrush(int wType);

		void _FillRcs(RCS *prcs);
		void _FillOval(RCS *prcs);
		void _FillPoly(OLY *poly);
		void _FillRgn(HRGN *phrgn);
		void _FrameRcsOval(RCS *prcs, GDD *pgdd, bool fOval);
		SCR _Scr(ACR acr);

		bool _FInit(HDC hdc);
	#endif //WIN

	#ifdef MAC
		static HCLT _hcltDef;
		static bool _fForcePalOnSys;
		static HCLT _HcltUse(long cbitPixel);

		//WARNING: the PPRT's below may be GWorldPtr's instead of GrafPtr's
		//Only use SetGWorld or GetGWorld on these.  Don't assume they
		//point to GrafPort's.
		PPRT _pprt;			//may be a GWorldPtr
		HGD _hgd;
		PPRT _pprtSav;		//may be a GWorldPtr
		HGD _hgdSav;
		short _cactLock;		//lock count for pixels (if offscreen)
		short _cbitPixel;		//depth of bitmap (if offscreen)
		bool _fSet: 1;
		bool _fOffscreen: 1;
		bool _fNoClip: 1;
		bool _fNewClip: 1;		//_pregnClip is new

		//for picture based GPT's
		RC _rcOff;			//also valid for offscreen GPTs
		HPIC _hpic;

		HPIX _Hpix(void);
		void _FillRcs(RCS *prcs);
		void _FrameRcs(RCS *prcs);
		void _FillOval(RCS *prcs);
		void _FrameOval(RCS *prcs);
		void _FillPoly(HQ *phqoly);
		void _FramePoly(HQ *phqoly);
		void _DrawLine(PTS *prgpts);
		void _GetRcsFromRgch(RCS *prcs, achar *prgch, short cch,
			PTS *ppts, DSF *pdsf);
	#endif //MAC

	// low level draw routine
	typedef void (GPT::*PFNDRW)(void *);
	void _Fill(void *pv, GDD *pgdd, PFNDRW pfn);

	GPT(void) {}
	~GPT(void);

public:
	#ifdef WIN
		static PGPT PgptNew(HDC hdc);
		static PGPT PgptNewHwnd(HWND hwnd);

		static long CclrSetPalette(HWND hwnd, bool fInval);

		// this gross API is for AVI playback
		void DrawDib(HDRAWDIB hdd, BITMAPINFOHEADER *pbi, RCS *prcs, GDD *pgdd);
	#endif //WIN
	#ifdef MAC
		static PGPT PgptNew(PPRT pprt, HGD hgd = hNil);

		static bool FCanScreen(long cbitPixel, bool fColor);
		static bool FSetScreenState(long cbitPixel, bool tColor);
		static void GetScreenState(long *pcbitPixel, bool *pfColor);

		void Set(RCS *prcsClip);
		void Restore(void);
	#endif //MAC
	#ifdef DEBUG
		static void MarkStaticMem(void);
	#endif //DEBUG

	static void SetActiveColors(PGL pglclr, ulong grfpal);
	static PGL PglclrGetPalette(void);
	static void Flush(void);

	static PGPT PgptNewOffscreen(RC *prc, long cbitPixel);
	static PGPT PgptNewPic(RC *prc);
	PPIC PpicRelease(void);
	void SetOffscreenColors(PGL pglclr = pvNil);

	void ClipToRegn(PREGN *ppregn);
	void SetPtBase(PT *ppt);
	void GetPtBase(PT *ppt);

	void DrawRcs(RCS *prcs, GDD *pgdd);
	void HiliteRcs(RCS *prcs, GDD *pgdd);
	void DrawOval(RCS *prcs, GDD *pgdd);
	void DrawLine(PTS *ppts1, PTS *ppts2, GDD *pgdd);
	void DrawPoly(HQ hqoly, GDD *pgdd);
	void ScrollRcs(RCS *prcs, long dxp, long dyp, GDD *pgdd);

	void DrawRgch(achar *prgch, long cch, PTS pts, GDD *pgdd, DSF *pdsf);
	void GetRcsFromRgch(RCS *prcs, achar *prgch, long cch, PTS pts, DSF *pdsf);

	void CopyPixels(PGPT pgptSrc, RCS *prcsSrc, RCS *prcsDst, GDD *pgdd);
	void DrawPic(PPIC ppic, RCS *prcs, GDD *pgdd);
	void DrawMbmp(PMBMP pmbmp, RCS *prcs, GDD *pgdd);

	void Lock(void);
	void Unlock(void);
	byte *PrgbLockPixels(RC *prc = pvNil);
	long CbRow(void);
	long CbitPixel(void);
	};


/****************************************
	Regions
****************************************/
bool FCreateRgn(HRGN *phrgn, RC *prc);
void FreePhrgn(HRGN *phrgn);
bool FSetRectRgn(HRGN *phrgn, RC *prc);
bool FUnionRgn(HRGN hrgnDst, HRGN hrgnSrc1, HRGN hrgnSrc2);
bool FIntersectRgn(HRGN hrgnDst, HRGN hrgnSrc1, HRGN hrgnSrc2,
	bool *pfEmpty = pvNil);
bool FDiffRgn(HRGN hrgnDst, HRGN hrgnSrc, HRGN hrgnSrcSub,
	bool *pfEmpty = pvNil);
bool FRectRgn(HRGN hrgn, RC *prc = pvNil);
bool FEmptyRgn(HRGN hrgn, RC *prc = pvNil);
bool FEqualRgn(HRGN hrgn1, HRGN hrgn2);


/****************************************
	Misc.
****************************************/
bool FInitGfx(void);

// stretch by a factor of 2 in each dimension.
void DoubleStretch(byte *prgbSrc, long cbRowSrc, long dypSrc, RC *prcSrc,
	byte *prgbDst, long cbRowDst, long dypDst, long xpDst, long ypDst,
	RC *prcClip, PREGN pregnClip);

// stretch by a factor of 2 in vertical direction only.
void DoubleVertStretch(byte *prgbSrc, long cbRowSrc, long dypSrc, RC *prcSrc,
	byte *prgbDst, long cbRowDst, long dypDst, long xpDst, long ypDst,
	RC *prcClip, PREGN pregnClip);

// Number of times that the palette has changed (via a call to CclrSetPalette
// or SetActiveColors). This can be used by other modules to detect a palette
// change.
extern long vcactRealize;

#endif //!GFX_H

