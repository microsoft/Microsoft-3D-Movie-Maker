/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Code for implementing help balloons in kidspace.

***************************************************************************/
#ifndef KIDHELP_H
#define KIDHELP_H


/***************************************************************************
	Help topic construction information.
***************************************************************************/
struct HTOP
	{
	CNO cnoBalloon;
	long hidThis;
	long hidTarget;
	CNO cnoScript;
	long dxp;
	long dyp;
	CKI ckiSnd;
	};
typedef HTOP *PHTOP;
const BOM kbomHtop = 0xFFF00000;


// help topic on file
struct HTOPF
	{
	short bo;
	short osk;
	HTOP htop;
	};


// edit control object
struct ECOS
	{
	CTG ctg;		// kctgEditControl
	long dxp;		// width
	};


/***************************************************************************
	Help text document
***************************************************************************/
enum
	{
	ftxhdNil = 0,
	ftxhdCopyText = 1,
	ftxhdExpandStrings = 2,
	};

typedef class TXHD *PTXHD;
#define TXHD_PAR TXRD
#define kclsTXHD 'TXHD'
class TXHD : public TXHD_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	enum
		{
		sprmGroup = 64,		// grouped (hot) text - uses the AG
		};

	PRCA _prca;			// source of pictures and buttons
	HTOP _htop;			// our gob creation information
	bool _fHideButtons;	// whether to draw buttons

	TXHD(PRCA prca, PDOCB pdocb = pvNil, ulong grfdoc = fdocNil);
	~TXHD(void);

	virtual bool _FReadChunk(PCFL pcfl, CTG ctg, CNO cno, PSTRG pstrg = pvNil,
		ulong grftxhd = ftxhdNil);
	virtual bool _FOpenArg(long icact, byte sprm, short bo, short osk);
	virtual bool _FGetObjectRc(long icact, byte sprm, PGNV pgnv, PCHP pchp,
		RC *prc);
	virtual bool _FDrawObject(long icact, byte sprm, PGNV pgnv,
		long *pxp, long yp, PCHP pchp, RC *prcClip);

public:
	static PTXHD PtxhdReadChunk(PRCA prca, PCFL pcfl, CTG ctg, CNO cno,
		PSTRG pstrg = pvNil, ulong grftxhd = ftxhdExpandStrings);

	virtual bool FSaveToChunk(PCFL pcfl, CKI *pcki, bool fRedirectText = fFalse);

	bool FInsertPicture(CNO cno, void *pvExtra, long cbExtra,
		long cp, long ccpDel, PCHP pchp = pvNil, ulong grfdoc = fdocUpdate);
	bool FInsertButton(CNO cno, CNO cnoTopic, void *pvExtra, long cbExtra,
		long cp, long ccpDel, PCHP pchp = pvNil, ulong grfdoc = fdocUpdate);
	PRCA Prca(void)
		{ return _prca; }
	bool FGroupText(long cp1, long cp2, byte bGroup,
		CNO cnoTopic = cnoNil, PSTN pstnTopic = pvNil);
	bool FGrouped(long cp, long *pcpMin = pvNil, long *pcpLim = pvNil,
		byte *pbGroup = pvNil, CNO *pcnoTopic = pvNil, PSTN pstnTopic = pvNil);

	void GetHtop(PHTOP phtop);
	void SetHtop(PHTOP phtop);
	void HideButtons(bool fHide = fTrue)
		{ _fHideButtons = FPure(fHide); }
	};


/***************************************************************************
	A runtime DDG for a help topic.
***************************************************************************/
typedef class TXHG *PTXHG;
#define TXHG_PAR TXRG
#define kclsTXHG 'TXHG'
class TXHG : public TXHG_PAR
	{
	RTCLASS_DEC
	CMD_MAP_DEC(TXHG)

protected:
	byte _bTrack;
	CNO _cnoTrack;
	long _hidBase;
	ulong _grfcust;
	PWOKS _pwoks;

	TXHG(PWOKS pwoks, PTXHD ptxhd, PGCB pgcb);
	virtual bool _FInit(void);
	virtual bool _FRunScript(byte bGroup, ulong grfcust, long hidHit, achar ch,
		CNO cnoTopic = cnoNil, long *plwRet = pvNil);

public:
	static PTXHG PtxhgNew(PWOKS pwoks, PTXHD ptxhd, PGCB pgcb);

	PTXHD Ptxhd(void)
		{ return (PTXHD)_ptxtb; }
	virtual bool FPtIn(long xp, long yp);
	virtual bool FCmdTrackMouse(PCMD_MOUSE pcmd);
	virtual bool FCmdMouseMove(PCMD_MOUSE pcmd);
	virtual bool FCmdBadKey(PCMD_BADKEY pcmd);
	virtual bool FGroupFromPt(long xp, long yp, byte *pbGroup = pvNil,
		CNO *pcnoTopic = pvNil);
	virtual void DoHit(byte bGroup, CNO cnoTopic, ulong grfcust, long hidHit);
	virtual void SetCursor(ulong grfcust);
	};


/***************************************************************************
	Help balloon.
***************************************************************************/
typedef class HBAL *PHBAL;
#define HBAL_PAR GOK
#define kclsHBAL 'HBAL'
class HBAL : public HBAL_PAR
	{
	RTCLASS_DEC

protected:
	PTXHG _ptxhg;

	HBAL(GCB *pgcb);
	virtual void _SetGorp(PGORP pgorp, long dxp, long dyp);
	virtual bool _FInit(PWOKS pwoks, PTXHD ptxhd, HTOP *phtop, PRCA prca);
	virtual bool _FSetTopic(PTXHD ptxhd, PHTOP phtop, PRCA prca);

public:
	static PHBAL PhbalCreate(PWOKS pwoks, PGOB pgobPar, PRCA prca,
		CNO cnoTopic, PHTOP phtop = pvNil);
	static PHBAL PhbalNew(PWOKS pwoks, PGOB pgobPar, PRCA prca,
		PTXHD ptxhd, PHTOP phtop = pvNil);

	virtual bool FSetTopic(PTXHD ptxhd, PHTOP phtop, PRCA prca);
	};


/***************************************************************************
	Help balloon button.
***************************************************************************/
typedef class HBTN *PHBTN;
#define HBTN_PAR GOK
#define kclsHBTN 'HBTN'
class HBTN : public HBTN_PAR
	{
	RTCLASS_DEC

protected:
	HBTN(GCB *pgcb);

	byte _bGroup;
	CNO _cnoTopic;

public:
	static PHBTN PhbtnNew(PWOKS pwoks, PGOB pgobPar, long hid, CNO cno,
		PRCA prca, byte bGroup, CNO cnoTopic, long xpLeft, long ypBottom);

	virtual bool FPtIn(long xp, long yp);
	virtual bool FCmdClicked(PCMD_MOUSE pcmd);
	};


#endif //!KIDHELP_H

