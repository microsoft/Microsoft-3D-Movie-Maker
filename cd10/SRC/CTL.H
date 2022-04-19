/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Standard controls (scroll bars, etc).

***************************************************************************/
#ifndef CTL_H
#define CTL_H

#ifdef WIN
typedef HWND HCTL;
#elif defined(MAC)
typedef ControlHandle HCTL;
#endif //MAC

//general control
typedef class CTL *PCTL;
#define CTL_PAR GOB
#define kclsCTL 'CTL'
class CTL : public CTL_PAR
	{
	RTCLASS_DEC

private:
	HCTL _hctl;

protected:
	CTL(PGCB pgcb);
	~CTL(void);

	virtual void _NewRc(void);
	HCTL _Hctl(void)
		{ return _hctl; }
	bool _FSetHctl(HCTL hctl);

public:
	static PCTL PctlFromHctl(HCTL hctl);

	#ifdef MAC
		virtual void Draw(PGNV pgnv, RC *prcClip);
	#endif //MAC
	};


//scroll bar
enum
	{
	fscbNil = 0,
	fscbVert = 1,
	fscbHorz = 2,
	fscbStandardRc = 4,

	//These are for GetStandardRc and GetClientRc.  They indicate that
	//the controls should not hide the indicated edge (ie, the edge should
	//be just inside the parent's rectangle).
	fscbShowLeft = 16,
	fscbShowRight = 32,
	fscbShowTop = 64,
	fscbShowBottom = 128
	};
#define kgrfscbShowHorz	(fscbShowLeft | fscbShowRight)
#define kgrfscbShowVert	(fscbShowTop | fscbShowBottom)
#define kgrfscbShowAll	(kgrfscbShowHorz | kgrfscbShowVert)


//scroll action
enum
	{
	scaNil,
	scaPageUp,
	scaPageDown,
	scaLineUp,
	scaLineDown,
	scaToVal,
	};


typedef class SCB *PSCB;
#define SCB_PAR CTL
#define kclsSCB 'SCB'
class SCB : public SCB_PAR
	{
	RTCLASS_DEC

private:
	long _val;
	long _valMin;
	long _valMax;
	bool _fVert: 1;
	#ifdef WIN
		bool _fSentEndScroll: 1;
	#endif //WIN

protected:
	SCB(PGCB pgcb) : CTL(pgcb) {}
	bool _FCreate(long val, long valMin, long valMax, ulong grfscb);

	#ifdef MAC
		virtual void _ActivateHwnd(bool fActive);
	#endif //MAC

public:
	static long DxpNormal(void);
	static long DypNormal(void);
	static void GetStandardRc(ulong grfscb, RC *prcAbs, RC *prcRel);
	static void GetClientRc(ulong grfscb, RC *prcAbs, RC *prcRel);
	static PSCB PscbNew(PGCB pgcb, ulong grfscb,
		long val = 0, long valMin = 0, long valMax = 0);

	void SetVal(long val, bool fRedraw = fTrue);
	void SetValMinMax(long val, long valMin, long valMax, bool fRedraw = fTrue);

	long Val(void)
		{ return _val; }
	long ValMin(void)
		{ return _valMin; }
	long ValMax(void)
		{ return _valMax; }

	#ifdef MAC
		virtual void MouseDown(long xp, long yp, long cact, ulong grfcust);
	#endif //MAC
	#ifdef WIN
		virtual void TrackScroll(long sb, long lwVal);
	#endif //WIN
	};


//size box
typedef class WSB *PWSB;
#define WSB_PAR CTL
#define kclsWSB 'WSB'
class WSB : public WSB_PAR
	{
	RTCLASS_DEC

protected:
	WSB(PGCB pgcb) : CTL(pgcb) {}

	#ifdef MAC
		virtual void _ActivateHwnd(bool fActive);
	#endif //MAC

public:
	static PWSB PwsbNew(PGOB pgob, ulong grfgob);

	#ifdef MAC
		virtual void Draw(PGNV pgnv, RC *prcClip);
	#endif //MAC
	};


#endif //!CTL_H

