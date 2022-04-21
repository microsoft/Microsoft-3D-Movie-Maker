/***************************************************************************

	popup.h: Popup menu classes

	Primary Author: ******
	         MPFNT: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	BASE ---> CMH ---> GOB ---> GOK ---> BRWD ---> BRWL ---> MP
                                          |
                                          +------> BRWT ---> MPFNT

***************************************************************************/
#ifndef POPUP_H
#define POPUP_H

/************************************
	MP - Generic popup menu class
*************************************/
#define MP_PAR BRWL
#define kclsMP 'MP'
typedef class MP *PMP;
class MP :  public MP_PAR
	{
	ASSERT
	MARKMEM
	RTCLASS_DEC
	CMD_MAP_DEC(MP)

protected:
	long _cid; // cid to enqueue to apply selection
	PCMH _pcmh; // command handler to enqueue command to

protected:
	virtual void _ApplySelection(long ithumSelect, long sid);
	virtual long _IthumFromThum(long thumSelect, long sidSelect);
	MP(PGCB pgcb) : MP_PAR(pgcb) { }
	bool _FInit(PRCA prca);

public:
	static PMP PmpNew(long kidParent, long kidMenu, PRCA prca, PCMD pcmd,
		BWS bws, long ithumSelect, long sidSelect, CKI ckiRoot, CTG ctg,
		PCMH pcmh, long cid, bool fMoveTop);

	virtual bool FCmdSelIdle(PCMD pcmd);
	};


/************************************
	MPFNT - Font popup menu class
*************************************/
#define MPFNT_PAR BRWT
#define kclsMPFNT 'mpft'
typedef class MPFNT *PMPFNT;
class MPFNT : public MPFNT_PAR
	{
	ASSERT
	MARKMEM
	RTCLASS_DEC
	CMD_MAP_DEC(MPFNT)

protected:
	void _AdjustRc(long cthum, long cfrm);

	virtual void _ApplySelection(long ithumSelect, long sid);
	virtual bool _FSetThumFrame(long istn, PGOB pgobPar);
	MPFNT(PGCB pgcb) : MPFNT_PAR(pgcb) { }

public:
	static PMPFNT PmpfntNew(PRCA prca, long kidParent, long kidMenu,
		PCMD pcmd, long ithumSelect, PGST pgst);

	virtual bool FCmdSelIdle(PCMD pcmd);
	};

#endif // POPUP_H
