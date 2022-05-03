/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Region stuff.

***************************************************************************/
#ifndef REGION_H
#define REGION_H

typedef class REGSC *PREGSC;


/***************************************************************************
	The region class.
***************************************************************************/
typedef class REGN *PREGN;
#define REGN_PAR BASE
#define kclsREGN 'REGN'
class REGN : public REGN_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	friend class REGSC;

	// The _pglxp contains a bunch of rows.  Each row consists of:
	// dyp, xp0, xp1, ..., klwMax.
	// The dyp is the height of the strip in pixels, the xp's are boundary points
	// The xp's are relative to _rc.xpLeft + _dxp. _pglxp is nil iff the region
	// is strictly rectangular.
	RC _rc;		// bounding rectangle
	long _dxp;	// additional offset for xp values
	PGL _pglxp;	// region data - see above
	HRGN _hrgn;	// for HrgnEnsure
	PT _dptRgn;	// offset of _hrgn relative to this region

	REGN(void) {}

	bool _FUnionCore(RC *prc, PREGSC pregsc1, PREGSC pregsc2);
	bool _FIntersectCore(RC *prc, PREGSC pregsc1, PREGSC pregsc2);
	bool _FDiffCore(RC *prc, PREGSC pregsc1, PREGSC pregsc2);

public:
	static PREGN PregnNew(RC *prc = pvNil);
	~REGN(void);

	void SetRc(RC *prc = pvNil);
	void Offset(long xp, long yp);
	bool FEmpty(RC *prc = pvNil);
	bool FIsRc(RC *prc = pvNil);
	void Scale(long lwNumX, long lwDenX, long lwNumY, long lwDenY);

	bool FUnion(PREGN pregn1, PREGN pregn2 = pvNil);
	bool FUnionRc(RC *prc, PREGN pregn2 = pvNil);
	bool FIntersect(PREGN pregn1, PREGN pregn2 = pvNil);
	bool FIntersectRc(RC *prc, PREGN pregn = pvNil);
	bool FDiff(PREGN pregn1, PREGN pregn2 = pvNil);
	bool FDiffRc(RC *prc, PREGN pregn = pvNil);
	bool FDiffFromRc(RC *prc, PREGN pregn = pvNil);

	HRGN HrgnCreate(void);
	HRGN HrgnEnsure(void);
	};


/***************************************************************************
	Region scanner class.
***************************************************************************/
#define REGSC_PAR BASE
#define kclsREGSC 'rgsc'
class REGSC : public REGSC_PAR
	{
	RTCLASS_DEC

protected:
	PGL _pglxpSrc;		//the list of points
	long *_pxpLimSrc;	//the end of the list
	long *_pxpLimCur;	//the end of the current row

	long _xpMinRow;		//the first xp value of the active part of the current row
	long *_pxpMinRow;	//the beginning of the active part of the currrent row
	long *_pxpLimRow;	//the end of the active part of the current row
	long *_pxp;			//the current postion in the current row

	long _dxp;			//this gets added to all source xp values
	long _xpRight;		//the right edge of the active area - left edge is 0

	//current state
	bool _fOn;			//whether the current xp is a transition to on or off
	long _xp;			//the current xp
	long _dyp;			//the remaining height that this scan is effective for
	long _dypTot;		//the remaining total height of the active area

	// When scanning rectangles, _pglxpSrc will be nil and _pxpSrc et al will
	// point into this.
	long _rgxpRect[4];

	void _InitCore(PGL pglxp, RC *prc, RC *prcRel);
	void _ScanNextCore(void);

public:

	REGSC(void);
	~REGSC(void);
	void Free(void);

	void Init(PREGN pregn, RC *prcRel);
	void InitRc(RC *prc, RC *prcRel);
	void ScanNext(long dyp)
		{
		_dypTot -= dyp;
		if ((_dyp -= dyp) <= 0)
			_ScanNextCore();
		_pxp = _pxpMinRow;
		_xp = _xpMinRow;
		_fOn = fTrue;
		}
	long XpFetch(void)
		{
		if (_pxp < _pxpLimRow - 1)
			{
			_xp = _dxp + *_pxp++;
			_fOn = !_fOn;
			}
		else if (_pxp < _pxpLimRow)
			{
			AssertH(_fOn);
			_xp = LwMin(_xpRight, _dxp + *_pxp++);
			_fOn = !_fOn;
			}
		else
			{
			_xp = klwMax;
			_fOn = fTrue;
			}
		return _xp;
		}

	bool FOn(void)
		{ return _fOn; }
	long XpCur(void)
		{ return _xp; }
	long DypCur(void)
		{ return _dyp; }
	long CxpCur(void)
		{ return _pxpLimRow - _pxpMinRow + 1; }
	};


#endif //!REGION_H

