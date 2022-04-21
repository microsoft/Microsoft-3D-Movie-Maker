//
//  tbox.cpp
//
//  Author: ******
//
//  Status: All changes must be code reviewed.
//
//  Date: November, 1994
//
//  This file contains all functionality for text box manipulation.
//
#include "soc.h"
ASSERTNAME

RTCLASS(TBOX)
RTCLASS(TBXG)
RTCLASS(TBXB)
RTCLASS(TCLP)

//
// How many pixels a scrolling text box scrolls by
//
#define kdypScroll 1

//
//
// UNDO objects for text boxes
//
//


//
// Undoes changing textbox type operations
//
typedef class TUNT *PTUNT;

#define TUNT_PAR MUNB
#define kclsTUNT 'TUNT'
class TUNT : public TUNT_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	long _itbox;
	bool _fStory;
	TUNT(void) { }

public:

	static PTUNT PtuntNew(void);
	~TUNT(void);

	void SetType(bool fStory) { _fStory = fStory; }
	void SetItbox(long itbox) { _itbox = itbox; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};

RTCLASS(TUNT)


//
// Undoes sizing operations
//
typedef class TUNS *PTUNS;

#define TUNS_PAR MUNB
#define kclsTUNS 'TUNS'
class TUNS : public TUNS_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	long _itbox;
	RC _rc;
	TUNS(void) { }

public:

	static PTUNS PtunsNew(void);
	~TUNS(void);

	void SetRc(RC *prc) { _rc = *prc; }
	void SetItbox(long itbox) { _itbox = itbox; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};

RTCLASS(TUNS)


//
// Undoes hiding/showing operations
//
typedef class TUNH *PTUNH;

#define TUNH_PAR MUNB
#define kclsTUNH 'TUNH'
class TUNH : public TUNH_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	long _itbox;
	long _nfrmFirst;
	long _nfrmMax;

	TUNH(void) { }

public:


	static PTUNH PtunhNew(void);
	~TUNH(void);

	void SetFrmFirst(long nfrmFirst) { _nfrmFirst = nfrmFirst; }
	void SetFrmLast(long nfrmMax) { _nfrmMax = nfrmMax; }
	void SetItbox(long itbox) { _itbox = itbox; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};

RTCLASS(TUNH)

//
// Undoes all editing operations
//
typedef class TUND *PTUND;

#define TUND_PAR MUNB
#define kclsTUND 'TUND'
class TUND : public TUND_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	PUNDB _pundb;
	long _itbox;
	TUND(void) { }

public:

	static PTUND PtundNew(PUNDB pundb);
	~TUND(void);

	void SetItbox(long itbox) { _itbox = itbox; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};

RTCLASS(TUND)


//
// Undoes coloring background operations
//
typedef class TUNC *PTUNC;

#define TUNC_PAR MUNB
#define kclsTUNC 'TUNC'
class TUNC : public TUNC_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:

	long _itbox;
	ACR _acr;
	TUNC(void) { }

public:

	static PTUNC PtuncNew(void);
	~TUNC(void);

	void SetItbox(long itbox) { _itbox = itbox; }
	void SetAcrBack(ACR acr) { _acr = acr; }

	virtual bool FDo(PDOCB pdocb);
	virtual bool FUndo(PDOCB pdocb);
	};

RTCLASS(TUNC)


//
//
// BEGIN TBXB and TBXG
//
//
/****************************************************
 *
 * Creates a textbox display area.
 *
 * Parameters:
 *	ptbox - The owning text box for this view.
 *	pgcb - Creation block.
 *
 * Returns:
 *  A pointer to the view, else pvNil.
 *
 ****************************************************/
PTBXB TBXB::PtbxbNew(PTBOX ptbox, PGCB pgcb)
{
	AssertPo(ptbox, 0);
	AssertPvCb(pgcb, size(GCB));

	PTBXB ptbxb;
	PTBXG ptbxg;
	RC rcRel, rcAbs;
	ACR acr;

	//
	// Create the border
	//
	ptbxb = NewObj TBXB(ptbox, pgcb);
	if (ptbxb == pvNil)
		{
		return(pvNil);
		}

	//
	// Now create the DDG area for the text
	//
	rcAbs.Set(kdzpBorderTbox, kdzpBorderTbox, -kdzpBorderTbox, -kdzpBorderTbox);
	rcRel.Set(krelZero, krelZero, krelOne, krelOne);
	pgcb->Set(pgcb->_hid, ptbxb, pgcb->_grfgob, pgcb->_gin, &rcAbs, &rcRel);
	if (pvNil == (ptbxg = (PTBXG)ptbox->PddgNew(pgcb)))
		{
        ReleasePpo(&ptbxb);
		return(pvNil);
		}

    ptbxg->SetTbxb(ptbxb);

	return ptbxb;
}

/****************************************************
 *
 * Draws a textbox border
 *
 * Parameters:
 *	pgnv - The graphic environment describing the draw.
 *	prcClip - the clipping rectangle to draw into.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBXB::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertPvCb(prcClip, size(RC));

	RC rc;
	RC rcClip;
	long lwSave;
	
	Assert(_ptbox->FIsVisible(), "DDG existing for invisible tbox");

	if (!_ptbox->FSelected())
		{
        return;
		}

	//
	// Draw border
	//
	pgnv->GetRcSrc(&rc);
	rc.Inset(kdzpBorderTbox / 2, kdzpBorderTbox / 2);

	if (_ptbox->FStory())
		{
		pgnv->FrameRcApt(&rc, &vaptLtGray, kacrBlack, kacrClear);
		}
    else
		{
		pgnv->FrameRcApt(&rc, &vaptLtGray, kacrYellow, kacrClear);
		}

	//
	// Upper left anchor
	//
	pgnv->GetRcSrc(&rc);
	rc.ypBottom = rc.ypTop + kdzpBorderTbox;
	lwSave = rc.xpRight;
	rc.xpRight = rc.xpLeft + kdzpBorderTbox;
	if (rcClip.FIntersect(&rc, prcClip))
		{
		pgnv->FillRc(&rcClip, kacrWhite);
		pgnv->FrameRc(&rcClip, kacrBlack);
		}

	//
	// Upper Middle anchor
	//
	rc.xpLeft = (lwSave - rc.xpLeft - kdzpBorderTbox) / 2;
	rc.xpRight = rc.xpLeft + kdzpBorderTbox;
	if (rcClip.FIntersect(&rc, prcClip))
		{
		pgnv->FillRc(&rcClip, kacrWhite);
		pgnv->FrameRc(&rcClip, kacrBlack);
		}

	//
	// Upper right anchor
	//
	rc.xpLeft = (lwSave - kdzpBorderTbox);
	rc.xpRight = lwSave;
	if (rcClip.FIntersect(&rc, prcClip))
		{
		pgnv->FillRc(&rcClip, kacrWhite);
		pgnv->FrameRc(&rcClip, kacrBlack);
		}
	
	//
	// Middle left anchor
	//
	pgnv->GetRcSrc(&rc);
	rc.ypTop = (rc.ypBottom - rc.ypTop - kdzpBorderTbox) / 2;
	rc.ypBottom = rc.ypTop + kdzpBorderTbox;
	lwSave = rc.xpRight;
	rc.xpRight = rc.xpLeft + kdzpBorderTbox;
	if (rcClip.FIntersect(&rc, prcClip))
		{
		pgnv->FillRc(&rcClip, kacrWhite);
		pgnv->FrameRc(&rcClip, kacrBlack);
		}

	//
	// Middle right anchor
	//
	rc.xpLeft = (lwSave - kdzpBorderTbox);
	rc.xpRight = lwSave;
	if (rcClip.FIntersect(&rc, prcClip))
		{
		pgnv->FillRc(&rcClip, kacrWhite);
		pgnv->FrameRc(&rcClip, kacrBlack);
		}
	
	//
	// Lower left anchor
	//
	pgnv->GetRcSrc(&rc);
	rc.ypTop = rc.ypBottom - kdzpBorderTbox;
	lwSave = rc.xpRight;
	rc.xpRight = rc.xpLeft + kdzpBorderTbox;
	if (rcClip.FIntersect(&rc, prcClip))
		{
		pgnv->FillRc(&rcClip, kacrWhite);
		pgnv->FrameRc(&rcClip, kacrBlack);
		}
	
	//
	// Lower middle anchor
	//
	rc.xpLeft = (lwSave - rc.xpLeft - kdzpBorderTbox) / 2;
	rc.xpRight = rc.xpLeft + kdzpBorderTbox;
	if (rcClip.FIntersect(&rc, prcClip))
		{
		pgnv->FillRc(&rcClip, kacrWhite);
		pgnv->FrameRc(&rcClip, kacrBlack);
		}
	
	//
	// Lower right anchor
	//	
	rc.xpLeft = (lwSave - kdzpBorderTbox);
	rc.xpRight = lwSave;
	if (rcClip.FIntersect(&rc, prcClip))
		{
		pgnv->FillRc(&rcClip, kacrWhite);
		pgnv->FrameRc(&rcClip, kacrBlack);
		}

}


/***************************************************************************
 *
 * Handles mouse commands for clicking and dragging.
 *
 * Parameters:
 *  pcmd - Pointer to the mouse command.
 *
 * Returns:
 *  fTrue if it handles the command, else fFalse.
 *
 **************************************************************************/
bool TBXB::FCmdTrackMouse(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PMVU pmvu;
	PT pt;
	RC rc, rcOld;
	RC rcBound(0, 0, _ptbox->Pscen()->Pmvie()->Pmcc()->Dxp(),
		_ptbox->Pscen()->Pmvie()->Pmcc()->Dyp());
	static long itbox = ivNil;

	pmvu = (PMVU)_ptbox->Pscen()->Pmvie()->PddgGet(0);
	AssertPo(pmvu, 0);

	pt.xp = pcmd->xp;
	pt.yp = pcmd->yp;
	MapPt(&pt, cooLocal, cooGlobal);

	if (_fTrackingMouse)
		{

		if (pcmd->grfcust & fcustMouse)
			{
			_fTrackingMouse = fFalse;
			vpcex->EndMouseTracking();
			vpappb->ShowCurs();
			vpcex->EnqueueCid(cidTboxClicked, pvNil, pvNil, _ptbox->Itbox(), fTrue);
			return(fTrue);
			}

		}

	if (pcmd->cid == cidMouseDown)
		{

		Assert(vpcex->PgobTracking() == pvNil, "mouse already being tracked!");
		//
		// Select this text box if not already
		//
		if (_ptbox->Pscen()->PtboxSelected() != _ptbox)
			{
			_ptbox->Pscen()->SelectTbox(_ptbox);
			}

		itbox = _ptbox->Itbox();

		vpcex->EnqueueCid(cidTboxClicked, pvNil, pvNil, itbox, fTrue);

        //
		// Check for nuker
		//
		if (pmvu->Tool() == toolActorNuke)
			{
			_ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(toolActorNuke, fcustShift);
            _ptbox->Pscen()->FRemTbox(_ptbox);
			return(fTrue);
			}

        if ((pmvu->Tool() != toolCutObject) &&
			(pmvu->Tool() != toolCopyObject) &&
            (pmvu->Tool() != toolPasteObject))
			{
			_tbxt = _TbxtAnchor(pcmd->xp, pcmd->yp);
			GetRc(&_rcOrig, cooParent);
			_xpPrev = pt.xp;
			_ypPrev = pt.yp;
			vpappb->HideCurs();
			}

        //
		// Store all initial information
		//
		vpcex->TrackMouse(this);
		}
	else
		{

		if (pmvu->Tool() == toolActorNuke)
			{
			return(fTrue);
			}

		//
		// mouse drag/up
		//
		Assert(vpcex->PgobTracking() == this, "not tracking mouse!");
		Assert(pcmd->cid == cidTrackMouse, 0);

 		if (pmvu->Tool() == toolPasteObject)
			{

			if (!(pcmd->grfcust & fcustMouse))
				{
				vpcex->EnqueueCid(cidTboxClicked, pvNil, pvNil, itbox, fFalse);
				vpcex->EndMouseTracking();
				}

			return(fTrue);

			}

        if ((pmvu->Tool() == toolCutObject) ||
			(pmvu->Tool() == toolCopyObject))
			{

			if (!(pcmd->grfcust & fcustMouse))
				{
				PTBXG ptbxg = (PTBXG)_ptbox->PddgGet(0);
				AssertPo(ptbxg,0);
				ptbxg->_FDoClip(pmvu->Tool());
				vpcex->EndMouseTracking();
				vpcex->EnqueueCid(cidTboxClicked, pvNil, pvNil, itbox, fFalse);
				}

			return(fTrue);

			}

		//
		// Now do work based on what we are doing -- dragging, stretching, shrinking.
		//
		GetRc(&rc, cooParent);
		rcOld = rc;

		//
		// Do vertical adjustment
		//
		switch (_tbxt)
			{
		case tbxtDown:
		case tbxtDownRight:
		case tbxtDownLeft:
			rc.ypBottom = LwBound(rcOld.ypBottom + pt.yp - _ypPrev,
				rc.ypTop + kdypMinTbox + LwMul(2, kdzpBorderTbox), rcBound.ypBottom + 1);
			_ypPrev += rc.ypBottom - rcOld.ypBottom;
			break;

		case tbxtUp:
		case tbxtUpRight:
		case tbxtUpLeft:
			rc.ypTop = LwBound(rcOld.ypTop + pt.yp - _ypPrev,
				rcBound.ypTop, rc.ypBottom - kdypMinTbox - LwMul(2, kdzpBorderTbox) + 1);
			_ypPrev += rc.ypTop - rcOld.ypTop;
			break;

		case tbxtMove:
			break;
			}

        //
		// Do horizontal adjustment
		//
		switch (_tbxt)
			{
		case tbxtRight:
		case tbxtDownRight:
		case tbxtUpRight:
			rc.xpRight = LwBound(rcOld.xpRight + pt.xp - _xpPrev,
				rc.xpLeft + kdxpMinTbox + LwMul(2, kdzpBorderTbox), rcBound.xpRight + 1);
			_xpPrev += rc.xpRight - rcOld.xpRight;
			break;

		case tbxtLeft:
		case tbxtDownLeft:
		case tbxtUpLeft:
			rc.xpLeft = LwBound(rcOld.xpLeft + pt.xp - _xpPrev,
				rcBound.xpLeft, (rc.xpRight - (kdxpMinTbox + LwMul(2, kdzpBorderTbox))) + 1);
			_xpPrev += rc.xpLeft - rcOld.xpLeft;
			break;

		case tbxtMove:
			rc.Offset(pt.xp - _xpPrev, pt.yp - _ypPrev);
			rc.PinToRc(&rcBound);
			_xpPrev += rc.xpLeft - rcOld.xpLeft;
			_ypPrev += rc.ypTop - rcOld.ypTop;
			break;
			}

		vpappb->PositionCurs(_xpPrev, _ypPrev);			

		if ((rc != rcOld) && !_ptbox->Pscen()->Pmvie()->Pmcc()->FMinimized())
			{
			//
			// Reposition the textbox.
			//
			
			SetPos(&rc);
			_ptbox->SetRc(&rc);
			}

        //
		// If mouse up
		//
		if (!(pcmd->grfcust & fcustMouse) && !_fTrackingMouse)
			{

            //
			// Add an undo object for the resize/move
			//
			GetRc(&rc, cooLocal);
			if (_rcOrig != rc)
				{
                PTUNS ptuns;

				ptuns = TUNS::PtunsNew();
				if (ptuns == pvNil)
					{
                    PushErc(ercSocNotUndoable);
                    _ptbox->Pscen()->Pmvie()->ClearUndo();
					}
                else
					{
					ptuns->SetItbox(_ptbox->Itbox());
                    ptuns->SetRc(&_rcOrig);

					if (!_ptbox->Pscen()->Pmvie()->FAddUndo(ptuns))
						{
						PushErc(ercSocNotUndoable);
						_ptbox->Pscen()->Pmvie()->ClearUndo();
						}
                    ReleasePpo(&ptuns);
					}
				}

			vpcex->EnqueueCid(cidTboxClicked, pvNil, pvNil, _ptbox->Itbox(), fFalse);
	
			vpcex->EndMouseTracking();
			vpappb->ShowCurs();
			}
		}

	return(fTrue);
}



/***************************************************************************
 *
 * Handles mouse move commands
 *
 * Parameters:
 *  pcmd - Pointer to the mouse move command.
 *
 * Returns:
 *  fTrue if it handles the command, else fFalse.
 *
 **************************************************************************/
bool TBXB::FCmdMouseMove(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PMVU pmvu;

	pmvu = (PMVU)_ptbox->Pscen()->Pmvie()->PddgGet(0);
	AssertPo(pmvu, 0);

	//
	// Check for cut, copy, paste, nuke tools
	//
	if ((pmvu->Tool() == toolCutObject) ||
        (pmvu->Tool() == toolCopyObject) ||
		(pmvu->Tool() == toolPasteObject) ||
		(pmvu->Tool() == toolActorNuke))
		{
		_ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(pmvu->Tool());
		return(fTrue);
		}

	switch(_TbxtAnchor(pcmd->xp, pcmd->yp))
		{
	case tbxtUp:
	case tbxtDown:
		_ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(toolTboxUpDown);
		break;
	
	case tbxtUpRight:
	case tbxtDownLeft:
		_ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(toolTboxRising);
		break;

	case tbxtLeft:
	case tbxtRight:
		_ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(toolTboxLeftRight);
		break;

	case tbxtUpLeft:
	case tbxtDownRight:
		_ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(toolTboxFalling);
		break;

	default:
		_ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(toolTboxMove);
		break;
		}

	return(fTrue);
}

/***************************************************************************
 *
 * Finds which anchor point the given point is in.
 *
 * Parameters:
 *  xp - X position in local coordinates
 *	yp - Y position in local coordinates
 *
 * Returns:
 *  Anchor point number.
 *
 **************************************************************************/
TBXT TBXB::_TbxtAnchor(long xp, long yp)
{
	AssertThis(0);

	RC rc;

    GetRc(&rc, cooLocal);

    //
	// Is the cursor in upper-left anchor?
	//
	if ((xp < kdzpBorderTbox) &&
		(yp < kdzpBorderTbox))
		{
		return(tbxtUpLeft);
		}

    //
	// Is the cursor in lower-right anchor?
	//
	if ((xp > rc.xpRight - kdzpBorderTbox) &&
		(yp > rc.ypBottom - kdzpBorderTbox))
		{
		return(tbxtDownRight);
		}

    //
	// Is the cursor in upper-right anchor?
	//
	if ((xp > rc.xpRight - kdzpBorderTbox) &&
		(yp < kdzpBorderTbox))
		{
		return(tbxtUpRight);
		}

    //
	// Is the cursor in lower-left anchor?
	//
	if ((xp < kdzpBorderTbox) &&
		(yp > rc.ypBottom - kdzpBorderTbox))
		{
		return(tbxtDownLeft);
		}

    //
	// Is the cursor in middle top?
	//
	if ((xp <= (rc.xpRight - rc.xpLeft + kdzpBorderTbox)/2) &&
		(xp >= (rc.xpRight - rc.xpLeft - kdzpBorderTbox)/2) &&	
		(yp < kdzpBorderTbox))
		{
		return(tbxtUp);
		}
		
    //
	// Is the cursor in middle bottom?
	//
	if ((xp <= (rc.xpRight - rc.xpLeft + kdzpBorderTbox)/2) &&
		(xp >= (rc.xpRight - rc.xpLeft - kdzpBorderTbox)/2) &&	
		(yp > rc.ypBottom - kdzpBorderTbox))
		{
		return(tbxtDown);
		}

    //
	// Is the cursor in middle left?
	//
	if ((yp <= (rc.ypBottom - rc.ypTop + kdzpBorderTbox)/2) &&
		(yp >= (rc.ypBottom - rc.ypTop - kdzpBorderTbox)/2) &&	
		(xp < kdzpBorderTbox))
		{
		return(tbxtLeft);
		}
		
    //
	// Is the cursor in middle right?
	//
	if ((yp <= (rc.ypBottom - rc.ypTop + kdzpBorderTbox)/2) &&
		(yp >= (rc.ypBottom - rc.ypTop - kdzpBorderTbox)/2) &&	
		(xp > rc.xpRight - kdzpBorderTbox))
		{
		return(tbxtRight);
		}

    //
	// Not in an anchor.
	//
	return(tbxtMove);
}



/***************************************************************************
 *
 * Callback for when this text box window gets activated
 *
 * Parameters:
 *  fActive - Is this the active window?
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void TBXB::Activate(bool fActive)
{
	PDDG pddg;

	pddg = _ptbox->PddgGet(0);
	AssertPo(pddg, 0);
	pddg->Activate(fActive);
	
	if (fActive)
		{
		BringToFront();
		_ptbox->Pscen()->SelectTbox(_ptbox);
		}
}

/***************************************************************************
 *
 * Will return fFalse if tbox is to be ignored.
 *
 * Parameters:
 *  xp, yp - The current mouse point.
 *
 * Returns:
 *  fTrue if it handles the command, else fFalse.
 *
 **************************************************************************/
bool TBXB::FPtIn(long xp, long yp)
{
	AssertThis(0);

	PMVU pmvu;

	pmvu = (PMVU)_ptbox->Pscen()->Pmvie()->PddgGet(0);
	AssertPo(pmvu, 0);

	//
	// Pass through if not in text mode, or in a tool that
	// does not select this text box.
	//
	if (!pmvu->FTextMode() ||
		((pmvu->Tool() == toolSceneNuke) ||
		 (pmvu->Tool() == toolSceneChop) ||
		 (pmvu->Tool() == toolSceneChopBack)) ||
		((_ptbox->Pscen()->PtboxSelected() != _ptbox) &&
		 ((pmvu->Tool() == toolTboxStory) ||
		  (pmvu->Tool() == toolTboxCredit))))
		{
		return(fFalse);
		}

	return(TBXB_PAR::FPtIn(xp, yp));
}


/****************************************************
 * Attach the mouse to this border.
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBXB::AttachToMouse(void)
{
	AssertThis(0);

	PMVU pmvu;
	PT pt;

	pmvu = (PMVU)_ptbox->Pscen()->Pmvie()->PddgGet(0);
	AssertPo(pmvu, 0);

	_fTrackingMouse = fTrue;
	_tbxt = tbxtMove;
	GetRc(&_rcOrig, cooParent);
	pt.xp = _rcOrig.xpLeft;
	pt.yp = _rcOrig.ypTop;
	MapPt(&pt, cooParent, cooGlobal);
	_xpPrev = pt.xp;
	_ypPrev = pt.yp;
	pmvu->SetTool(toolTboxMove);
	_ptbox->Pscen()->Pmvie()->Pmcc()->ChangeTool(toolTboxMove);
	vpappb->PositionCurs(_xpPrev, _ypPrev);			
	vpappb->HideCurs();
	vpcex->TrackMouse(this);
}



#ifdef DEBUG

/****************************************************
 * Mark memory used by the TBXB
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBXB::MarkMem(void)
{
	AssertThis(0);
	TBXB_PAR::MarkMem();
}

/***************************************************************************
 *
 * Assert the validity of the TBXB.
 *
 * Parameters:
 *	grf - Bit field of options
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void TBXB::AssertValid(ulong grf)
{
	TBXB_PAR::AssertValid(fobjAllocated);
}

#endif




//
// Disable the some default rich text functionality,
// and then intercept other commands.
//
BEGIN_CMD_MAP(TBXG, DDG)
	ON_CID_GEN(cidSave, pvNil, pvNil)
	ON_CID_GEN(cidClose, pvNil, pvNil)
	ON_CID_GEN(cidSaveAndClose, pvNil, pvNil)
	ON_CID_GEN(cidSaveAs, pvNil, pvNil)
	ON_CID_GEN(cidSaveCopy, pvNil, pvNil)
	ON_CID_GEN(cidCutTool, &TBXG::FCmdClip, &TBXG::FEnableDdgCmd)
	ON_CID_GEN(cidCopyTool, &TBXG::FCmdClip, &TBXG::FEnableDdgCmd)
	ON_CID_GEN(cidPasteTool, &TBXG::FCmdClip, &TBXG::FEnableDdgCmd)
	ON_CID_GEN(cidPaste, &TBXG::FCmdClip, &TBXG::FEnableDdgCmd)
	ON_CID_GEN(cidCut, &TBXG::FCmdClip, pvNil)
	ON_CID_GEN(cidCopy, &TBXG::FCmdClip, pvNil)
	ON_CID_GEN(cidUndo, pvNil, pvNil)
	ON_CID_GEN(cidRedo, pvNil, pvNil)
END_CMD_MAP_NIL()


/****************************************************
 *
 * Destructor for text box DDGs.
 *
 ****************************************************/
TBXG::~TBXG()
{
}

/****************************************************
 *
 * Creates a textbox display area.
 *
 * Parameters:
 *	ptbox - The owning text box for this view.
 *	pgcb - Creation block.
 *
 * Returns:
 *  A pointer to the view, else pvNil.
 *
 ****************************************************/
PTBXG TBXG::PtbxgNew(PTBOX ptbox, PGCB pgcb)
{
	AssertPo(ptbox, 0);
	AssertPvCb(pgcb, size(GCB));

	PTBXG ptbxg;

	if (pvNil == (ptbxg = NewObj TBXG(ptbox, pgcb)))
		{
		return(pvNil);
		}

	if (!ptbxg->_FInit())
		{
		ReleasePpo(&ptbxg);
		return pvNil;
		}

	ptbxg->GetRc(&ptbxg->_rcOld, cooLocal);
	return ptbxg;
}


/****************************************************
 *
 * Draws a textbox innards
 *
 * Parameters:
 *	pgnv - The graphic environment describing the draw.
 *	prcClip - the clipping rectangle to draw into.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBXG::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertPvCb(prcClip, size(RC));

	RC rc;

	//
	// In order to do scrolling text boxex, the easiest
	// way to get the text to scroll is to grow the DDG
	// upward (to the top of the screen), but then clip
	// the drawing to within the border.
	//
	// The DDG will automatically be clipped to within
	// the border GOB, but the drawn border (dashes and
	// anchors) must then be subtracted.
	//
	GetRc(&rc, cooParent);
	rc.ypTop = kdzpBorderTbox;
	MapRc(&rc, cooParent, cooLocal);
	rc.FIntersect(prcClip);
	pgnv->ClipRc(&rc);

	TBXG_PAR::Draw(pgnv, &rc);
}


/***************************************************************************
 *
 * Callback for when this text box window gets activated
 *
 * Parameters:
 *  fActive - Is this the active window?
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void TBXG::Activate(bool fActive)
{
	AssertThis(0);

	PTBOX ptbox = (PTBOX)_pdocb;
	AssertPo(ptbox, 0);

	TBXG_PAR::Activate(fActive);	
	ptbox->Select(fActive);
}

/***************************************************************************
 *
 * Handles when the user starts typing
 *
 * Parameters:
 *  cp - Character position.
 *	ccpIns - Number of characters to insert.
 *	ccpDel - Number of characters to delete.
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void TBXG::InvalCp(long cp, long ccpIns, long ccpDel)
{
	PMVU pmvu;
	PTBOX ptbox = (PTBOX)_pdocb;
	AssertPo(ptbox, 0);

	pmvu = (PMVU)ptbox->Pscen()->Pmvie()->PddgGet(0);
	AssertPo(pmvu, 0);

	if (((pmvu->Tool() == toolTboxPaintText) ||
         (pmvu->Tool() == toolTboxFont) ||
         (pmvu->Tool() == toolTboxSize) ||
         (pmvu->Tool() == toolTboxStyle)) &&
		(ccpIns != ccpDel))
		{
		pmvu->SetTool(toolTboxMove);
		ptbox->Pscen()->Pmvie()->Pmcc()->ChangeTool(toolTboxMove);
		}

	TBXG_PAR::InvalCp(cp, ccpIns, ccpDel);
}


/***************************************************************************
 *
 * Handles mouse move commands
 *
 * Parameters:
 *  pcmd - Pointer to the mouse move command.
 *
 * Returns:
 *  fTrue if it handles the command, else fFalse.
 *
 **************************************************************************/
bool TBXG::FCmdMouseMove(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PTBOX ptbox = (PTBOX)_pdocb;
	PMVU pmvu;

	pmvu = (PMVU)ptbox->Pscen()->Pmvie()->PddgGet(0);
	AssertPo(pmvu, 0);

	//
	// Check for cut, copy, paste tools
	//
	switch(pmvu->Tool())
		{
	case toolCutObject:
		ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(toolCutText);
		return(fTrue);

	case toolCopyObject:
		ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(toolCopyText);
		return(fTrue);

	case toolPasteObject:
		ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(toolPasteText);
		return(fTrue);

	case toolTboxFillBkgd:
	case toolTboxPaintText:
	case toolActorNuke:
	case toolTboxStory:
	case toolTboxCredit:
	case toolTboxFont:
	case toolTboxSize:
	case toolTboxStyle:
		ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(pmvu->Tool());
		return(fTrue);

		}

	ptbox->Pscen()->Pmvie()->Pmcc()->SetCurs(toolIBeam);

	return(fTrue);
}

/***************************************************************************
 *
 * Handles other mouse commands
 *
 * Parameters:
 *  pcmd - Pointer to the mouse command.
 *
 * Returns:
 *  fTrue if it handles the command, else fFalse.
 *
 **************************************************************************/
bool TBXG::FCmdTrackMouse(PCMD_MOUSE pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PTBOX ptbox = (PTBOX)_pdocb;
	PMVU pmvu;
	CHP chpNew, chpDiff;

	chpNew.Clear();
	chpDiff.Clear();

	pmvu = (PMVU)ptbox->Pscen()->Pmvie()->PddgGet(0);
	AssertPo(pmvu, 0);

	//
	// Check for the nuker
	//
	if (pmvu->Tool() == toolActorNuke)
		{
		if (pcmd->cid == cidMouseDown)
			{
			vpcex->EnqueueCid(cidTboxClicked, pvNil, pvNil, ptbox->Itbox(), fTrue);

			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(toolActorNuke, fcustShift);
			ptbox->Pscen()->FRemTbox(ptbox);
			}
		return(fTrue);
		}

	//
	// No selecting text with the fill bucket or type changers
	//
	if ((pmvu->Tool() != toolTboxFillBkgd) &&
		(pmvu->Tool() != toolTboxStory) &&
        (pmvu->Tool() != toolTboxCredit))
		{
		TBXG_PAR::FCmdTrackMouse(pcmd);
		}

	if ((pcmd->cid == cidMouseDown) &&
		(pcmd->grfcust & fcustMouse))
		{

		vpcex->EnqueueCid(cidTboxClicked, pvNil, pvNil, ptbox->Itbox(), fTrue);

		switch(pmvu->Tool())
			{
		case toolTboxFillBkgd:
			ptbox->FSetAcrBack(pmvu->AcrPaint());
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(pmvu->Tool());
			break;

		case toolTboxStory:
			ptbox->FSetType(fTrue);
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(pmvu->Tool());
			break;

		case toolTboxCredit:
			ptbox->FSetType(fFalse);
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(pmvu->Tool());
			break;

			}

		return(fTrue);

		}

	if ((pcmd->cid != cidMouseDown) && !(pcmd->grfcust & fcustMouse))
		{

		vpcex->EnqueueCid(cidTboxClicked, pvNil, pvNil, ptbox->Itbox(), fFalse);

        switch(pmvu->Tool())
			{
        case toolCutObject:
			_FDoClip(toolCutText);
			break;

        case toolCopyObject:
			_FDoClip(toolCopyText);
            SetSel(_cpOther, _cpOther);
			break;

        case toolPasteObject:
			_FDoClip(toolPasteText);
			break;

        case toolTboxPaintText:
			if (!FTextSelected())
				break;
			chpNew.acrFore = pmvu->AcrPaint();
			chpDiff.acrFore.SetFromLw(~chpNew.acrFore.LwGet());
			goto LApplyFormat;
		case toolTboxFont:
			if (!FTextSelected())
				break;
			chpNew.onn = pmvu->OnnTextCur();
			chpDiff.onn = ~chpNew.onn;
			goto LApplyFormat;
		case toolTboxStyle:
			if (!FTextSelected())
				break;
			chpNew.grfont = pmvu->GrfontStyleTextCur();
			chpDiff.grfont = ~chpNew.grfont;
			goto LApplyFormat;
		case toolTboxSize:
			if (!FTextSelected())
				break;
			chpNew.dypFont = pmvu->DypFontTextCur();
			chpDiff.dypFont = ~chpNew.dypFont;
LApplyFormat:
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(pmvu->Tool());
			FApplyChp(&chpNew, &chpDiff);
			break;

			}

		}

	return(fTrue);
}


/***************************************************************************
 *
 * Will return fFalse if tbox is to be ignored.
 *
 * Parameters:
 *  xp, yp - The current mouse point.
 *
 * Returns:
 *  fTrue if it handles the command, else fFalse.
 *
 **************************************************************************/
bool TBXG::FPtIn(long xp, long yp)
{
	AssertThis(0);

	PTBOX ptbox = (PTBOX)_pdocb;
	PMVU pmvu;

	AssertPo(ptbox,0);

	pmvu = (PMVU)ptbox->Pscen()->Pmvie()->PddgGet(0);
	AssertPo(pmvu, 0);

	//
	// Pass through if not in text mode, or in a tool that
	// does not select this text box.
	//
	if (!pmvu->FTextMode() ||
		((pmvu->Tool() == toolSceneNuke) ||
		 (pmvu->Tool() == toolSceneChop) ||
		 (pmvu->Tool() == toolSceneChopBack)))
		{
		return(fFalse);
		}

	return(TBXG_PAR::FPtIn(xp, yp));
}


/***************************************************************************
 *
 * Used for resizing.  We don't actually change the width of the
 * document, only the size of the view.
 *
 * NOTE: Do not AssertThis(0) in this routine, as this routine
 * is called during a time the doc is invalid.
 *
 * Parameters:
 *  None.
 *
 * Returns:
 *	Width in pixels.
 *
 **************************************************************************/
long TBXG::_DxpDoc()
{
	AssertBaseThis(0);

	RC rc;

	GetRc(&rc, cooLocal);
	return(rc.Dxp() - 2 * kdxpIndentTxtg);
}

/***************************************************************************
 *
 * Notification that there is a new rectangle.  Here we reformat the text
 * to fit into the new rectangle.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 * 	None.
 *
 **************************************************************************/
void TBXG::_NewRc(void)
{
	AssertBaseThis(0);

	RC rc;

	GetRc(&rc, cooLocal);

	//
	// Only reformat if the width changes
	//
	if (_rcOld.Dxp() == rc.Dxp())
		{
		return;
		}

	_rcOld = rc;

	TBXG_PAR::_NewRc();

	long cpLim = _ptxtb->CpMac() - 1;
	
	_Reformat(0, cpLim, cpLim);
}

/***************************************************************************
 *
 * Handles preparing for cut, copy or paste.
 *
 * Parameters:
 *	pcmd - The command to process.
 *
 * Returns:
 * 	fTrue if it handled the command, else fFalse.
 *
 **************************************************************************/
bool TBXG::FCmdClip(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PTBOX ptbox = (PTBOX)_pdocb;
	PDOCB pdocb;
	PMVU pmvu;

	pmvu = (PMVU)ptbox->Pscen()->Pmvie()->PddgGet(0);
	AssertPo(pmvu, 0);

	Assert(pmvu->FTextMode(), "Bad mode");

	if (((pcmd->cid == cidPaste) || (pcmd->cid == cidPasteTool)) &&
		vpclip->FGetFormat(kclsACLP, &pdocb))
		{
		CMD cmd;

		if (((PACLP)pdocb)->FRouteOnly())
			{
			PushErc(ercSocCannotPasteThatHere);
			ReleasePpo(&pdocb);
			return(fTrue);
			}
		//
		// Pass this onto the MVU for pasting
		//

		cmd = *pcmd;
		cmd.pcmh = pmvu;

		vpcex->EnqueueCmd(&cmd);
		ReleasePpo(&pdocb);
		return(fTrue);
		}

    switch (pcmd->cid)
		{

	case cidPaste:
		_FDoClip(vpclip->FGetFormat(kclsTCLP) ? toolPasteObject : toolPasteText);
		break;

	case cidCut:
		_FDoClip(FTextSelected() ? toolCutText : toolCutObject);
		break;

	case cidCopy:
		_FDoClip(FTextSelected() ? toolCopyText : toolCopyObject);
		break;

    default:
		return(fFalse);
		}

	return(fTrue);
}



/***************************************************************************
 *
 * Handles enabling of all cut, copy, paste commands.
 *
 * Parameters:
 *	pcmd - The command to process.
 *
 * Returns:
 * 	fTrue if it handled the command, else fFalse.
 *
 **************************************************************************/
bool TBXG::FEnableDdgCmd(PCMD pcmd, ulong *pgrfeds)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	AssertVarMem(pgrfeds);

	PTBOX ptbox = (PTBOX)_pdocb;
	AssertPo(ptbox, 0);

	*pgrfeds = fedsEnable;
    switch (pcmd->cid)
		{
	case cidCutTool:
	case cidCopyTool:		
		break;

	case cidPaste:
	case cidPasteTool:
	    //
		// First check if the clipboard contains any text
		// which we might need to paste *into* the text box.
		//
		if (ptbox->FSelected() && !vpclip->FDocIsClip(pvNil) &&
				_FPaste(vpclip, fFalse, cidPaste))
			{
			*pgrfeds &= fedsEnable;
			return(fTrue);
			}

		//
		// Now check if clipboard is a text box.
		//
		if (!vpclip->FGetFormat(kclsTCLP))
			{
            *pgrfeds = fedsDisable;
			}
		break;

    default:
		return (TBXG_PAR::FEnableDdgCmd(pcmd, pgrfeds));
		}

	return(fTrue);
}


/***************************************************************************
 *
 * Actually does cut, copy or paste command.
 *
 * Parameters:
 *	tool - The tool to use.
 *
 * Returns:
 * 	fTrue if it handled the command, else fFalse.
 *
 **************************************************************************/
bool TBXG::_FDoClip(long tool)
{
	AssertThis(0);

	PTBOX ptbox = (PTBOX)_pdocb;
	PTBOX ptboxDup;
	PTCLP ptclp;
	CMD cmd;

	AssertPo(ptbox, 0);

	switch (tool)
		{
	case toolCutText:
	case toolCopyText:

    	if (FTextSelected())
    		{
			ClearPb(&cmd, size(CMD));
    		cmd.cid = (tool == toolCutText) ? cidCut : cidCopy;
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(tool);
    		if (!TBXG_PAR::FCmdClip(&cmd))
				{
				return(fFalse);
				}
			return(fTrue);
    		}
		return(fTrue);

	case toolPasteText:

		if (!vpclip->FGetFormat(kclsACLP) && !vpclip->FGetFormat(kclsTCLP))
			{
			ClearPb(&cmd, size(CMD));
   			cmd.cid = cidPaste;
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(tool);
			return(TBXG_PAR::FCmdClip(&cmd));
			}
		else
			{
			PushErc(ercSocCannotPasteThatHere);
			}
		return(fTrue);

	case toolCutObject:
	case toolCopyObject:

    	//
    	// Copy the text box
    	//
    	if (!ptbox->FDup(&ptboxDup))
    		{
    		return(fFalse);
    		}
        AssertPo(ptboxDup, 0);

		//
		// Create the clip board object
		//
        ptclp = TCLP::PtclpNew(ptboxDup);
		AssertNilOrPo(ptclp, 0);
		if (ptclp == pvNil)
			{
            ReleasePpo(&ptboxDup);
			return(fFalse);
			}

		//
		// Hide the current tbox
		//
		if (tool == toolCutObject)
			{
			if (!ptbox->Pscen()->Pmvie()->FHideTbox())
				{
                ReleasePpo(&ptclp);
				ReleasePpo(&ptboxDup);
				return(fFalse);
				}
			}

		vpclip->Set(ptclp);
		ReleasePpo(&ptclp);
		ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(tool);
		break;

	case toolPasteObject:

		if (vpclip->FGetFormat(kclsTCLP, (PDOCB *)&ptclp))
			{
    		AssertPo(ptclp, 0);
			bool fRet;

    		fRet = ptbox->Pscen() != pvNil && ptclp->FPaste(ptbox->Pscen());
			ReleasePpo(&ptclp);
			ptbox->Pscen()->Pmvie()->Pmcc()->PlayUISound(tool);
			return fRet;
			}
		else
    		{
			PushErc(ercSocNothingToPaste);
    		}
		break;

    default:
	    Bug("Unknown tool type");
		}

	return(fTrue);
}

/****************************************************
 *
 * Decides if a text box needs to scroll right now.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if yes, else fFalse.
 *
 ****************************************************/
bool TBXG::FNeedToScroll()
{
	AssertThis(0);

	RC rc;
	long dyp;

	//
	// Get the height of the remaining text
	//
	GetNaturalSize(pvNil, &dyp);
	dyp -= _dypDisp;
	GetRc(&rc, cooLocal);

	//
	// Check vs the height of the box.
	//
	return dyp > rc.Dyp();
}

/****************************************************
 *
 * Scrolls up by one pixel, or to the beginning of the text.
 *
 * Parameters:
 *	scaVert - How to scroll vertically, scaNil == go to top,
 *		anything else scrolls down by 1 pixel.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBXG::Scroll(long scaVert)
{
	AssertThis(0);

	RC rcAbs, rcRel;

	if (scaVert == scaNil)
		{
		_Scroll(scaToVal, scaToVal, 0, 0);
		}
	else
		{
    	GetPos(&rcAbs, &rcRel);
		rcAbs.ypTop -= kdypScroll;
		SetPos(&rcAbs, &rcRel);
		}
}

/****************************************************
 *
 * Tells if any text is selected or not.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
bool TBXG::FTextSelected(void)
{
	AssertThis(0);
    return(_cpAnchor != _cpOther);
}


/****************************************************
 *
 * Get the character properties for displaying the given cp.
 *
 * Parameters:
 *	cp - Character position.
 *	pchp - Destination chp.
 *	pcpMin - Starting character position.
 *	pcpLim - Ending character position.
 *
 * Returns:
 * 	None.
 *
 ****************************************************/
void TBXG::_FetchChp(long cp, PCHP pchp, long *pcpMin, long *pcpLim)
{
	TBXG_PAR::_FetchChp(cp, pchp, pcpMin, pcpLim);

	if (pchp->acrFore == kacrBlack)
		{
		pchp->acrFore = kacrYellow;
		}
}



#ifdef DEBUG

/****************************************************
 * Mark memory used by the TBXG
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBXG::MarkMem(void)
{
	AssertThis(0);
	TBXG_PAR::MarkMem();
	MarkMemObj(_ptbxb);
}

/***************************************************************************
 *
 * Assert the validity of the TBXG.
 *
 * Parameters:
 *	grf - Bit field of options
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void TBXG::AssertValid(ulong grf)
{
	TBXG_PAR::AssertValid(fobjAllocated);
	AssertPo(_ptbxb, 0);
}

#endif //DEBUG




//
//
// BEGIN TBOX
//
//

#define kbomTboxh 0x5FFFC000

//
// header information for saving text boxes to a file
//
struct TBOXH
	{
	short bo;
	short osk;
	long nfrmFirst;
	long nfrmMax;
	long xpLeft;
	long xpRight;
	long ypTop;
	long ypBottom;
	CHID chid;
	bool fStory;
	};



/****************************************************
 *
 * Creates a textbox
 *
 * Parameters:
 *	pscen - Scene which owns this textbox.
 *	prcRel - The bounding rectangle of the DDG for the text box within the
 *		the owning MVU.
 *	fStory - Is this a story text box?
 *
 * Returns:
 *  None.
 *
 ****************************************************/
PTBOX TBOX::PtboxNew(PSCEN pscen, RC *prcRel, bool fStory)
{
	AssertNilOrPo(pscen, 0);
	AssertPvCb(prcRel, size(RC));

	PTBOX ptbox;

	ptbox = NewObj TBOX;
	if (ptbox == pvNil)
		{
		return(pvNil);
		}

	if (!ptbox->_FInit(pvNil))
		{
        ReleasePpo(&ptbox);
		return(pvNil);
		}

    if (prcRel != pvNil)
		{
        ptbox->_rc = *prcRel;
		ptbox->_dxpDef = prcRel->xpRight - prcRel->xpLeft - 2*kdzpBorderTbox;
		}

    ptbox->_pscen = pscen;
	ptbox->_fStory = fStory;
	ptbox->SetAcrBack(kacrClear, fdocNil);
	Assert(pscen == pvNil || pscen->Pmvie()->CundbMax() == 1,
		"TUND assumes there is one level of undo!");
	ptbox->SetCundbMax(1);

	return ptbox;
}



/****************************************************
 *
 * Sets the dirty flag on the movie.
 *
 * Parameters:
 *	fDirty - To dirty, or not to dirty, that is the question.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::SetDirty(bool fDirty)
{
	AssertThis(0);

	if (_pscen != pvNil)
		{
		_pscen->MarkDirty(fDirty);
		}
}

/****************************************************
 *
 * Reads a textbox
 *
 * Parameters:
 *	pcfl - Chunky file to read from.
 *	cno - The chunk number to read.
 *	pscen - Scene which owns this textbox.
 *
 * Returns:
 *  Pointer to a new tbox, else pvNil.
 *
 ****************************************************/
PTBOX TBOX::PtboxRead(PCRF pcrf, CNO cno, PSCEN pscen)
{
	AssertPo(pcrf, 0);
	AssertNilOrPo(pscen, 0);

	PTBOX ptbox;
	BLCK blck;
	TBOXH tboxh;
	KID kid;
	PCFL pcfl = pcrf->Pcfl();

	//
	// Find the chunk and read in the header.
	//
	if (!pcfl->FFind(kctgTbox, cno, &blck) || !blck.FUnpackData() ||
		(blck.Cb() != size(TBOXH)) ||
		!blck.FReadRgb(&tboxh, size(TBOXH), 0))
		{
        PushErc(ercSocBadFile);
        return(pvNil);
		}

	//
	// Check header for byte swapping
	//
    if (tboxh.bo == kboOther)
		{
		SwapBytesBom(&tboxh, kbomTboxh);
		}
    else
		{
        Assert(tboxh.bo == kboCur, "Bad Chunky file");
		}

	if (!pcfl->FGetKidChidCtg(kctgTbox, cno, tboxh.chid, kctgRichText, &kid))
		{
		return(pvNil);
		}


	ptbox = NewObj TBOX;
	if (ptbox == pvNil)
		{
		return(pvNil);
		}

	if (!ptbox->_FReadChunk(pcfl, kid.cki.ctg, kid.cki.cno, fTrue))
		{
		ReleasePpo(&ptbox);
		return(pvNil);
		}

    ptbox->_pscen = pscen;
    ptbox->_fStory = tboxh.fStory;
	ptbox->_nfrmFirst = tboxh.nfrmFirst;
	ptbox->_nfrmCur = tboxh.nfrmFirst - 1;
	ptbox->_nfrmMax = tboxh.nfrmMax;
	ptbox->_rc.Set(tboxh.xpLeft, tboxh.ypTop, tboxh.xpRight, tboxh.ypBottom);
	AssertPo(ptbox, 0);

	return ptbox;
}



/****************************************************
 *
 * Writes the text box to a specifc chunk number
 *
 * Parameters:
 *	pcfl - The chunky file to write to.
 *	cno - The chunk number to write to.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TBOX::FWrite(PCFL pcfl, CNO cno)
{
	AssertThis(0);
	AssertPo(pcfl, 0);

	TBOXH tboxh;
	CKI cki;

	tboxh.bo = kboCur;
	tboxh.osk= koskCur;
	tboxh.nfrmFirst = _nfrmFirst;
	tboxh.nfrmMax = _nfrmMax;
	tboxh.xpLeft = _rc.xpLeft;
	tboxh.xpRight = _rc.xpRight;
	tboxh.ypTop = _rc.ypTop;
	tboxh.ypBottom = _rc.ypBottom;
	tboxh.fStory = _fStory;
	tboxh.chid = 0;

	if (!FSaveToChunk(pcfl, &cki))
		{
		return(fFalse);
        }

	Assert(cki.ctg == kctgRichText, "bad ctg");

	if (!pcfl->FAdoptChild(kctgTbox, cno, cki.ctg, cki.cno, 0))
		{
        pcfl->Delete(cki.ctg, cki.cno);
		return(fFalse);
		}

	if (!pcfl->FPutPv((void *)&tboxh, size(TBOXH), kctgTbox, cno))
		{
		pcfl->DeleteChild(kctgTbox, cno, cki.ctg, cki.cno, 0);
		return(fFalse);
		}

	return(fTrue);
}


/****************************************************
 *
 * Sets the owning scene of the textbox.
 *
 * Parameters:
 *	pscen - The owning scene of the textbox.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::SetScen(PSCEN pscen)
{
	AssertThis(0);
	AssertPo(pscen, 0);

	_nfrmCur = pscen->Nfrm();
	_nfrmFirst = pscen->Nfrm();
	_nfrmMax = klwMax;
	_pscen = pscen;
}



/****************************************************
 *
 * Sets the type of the textbox.
 *
 * Parameters:
 *	fStory - fTrue if it is to become a story textbox,
 *		else fFalse.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::SetTypeCore(bool fStory)
{
	AssertThis(0);

	PTBXG ptbxg;

	_fStory = fStory;
	ptbxg = (PTBXG)PddgGet(0);
	AssertNilOrPo(ptbxg, 0);

	if (ptbxg == pvNil)
		{
        return;
		}

	ptbxg->Ptbxb()->InvalRc(pvNil);	
}


/****************************************************
 *
 * Attaches the textbox to the mouse.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::AttachToMouse(void)
{
	AssertThis(0);

	PTBXG ptbxg;

	ptbxg = (PTBXG)PddgGet(0);
	AssertNilOrPo(ptbxg, 0);

	if (ptbxg == pvNil)
		{
        return;
		}

	ptbxg->Ptbxb()->AttachToMouse();	
}


/****************************************************
 *
 * Sets the type of the textbox and creates an undo object
 *
 * Parameters:
 *	fStory - fTrue if it is to become a story textbox,
 *		else fFalse.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TBOX::FSetType(bool fStory)
{
	AssertThis(0);

	PTUNT ptunt;

	if (_fStory == fStory)
		{
		return(fTrue);
		}

	ptunt = TUNT::PtuntNew();

	if (ptunt == pvNil)
		{
		return(fFalse);
		}

	ptunt->SetType(!fStory);
	ptunt->SetItbox(Itbox());

	if (!Pscen()->Pmvie()->FAddUndo(ptunt))
		{
		ReleasePpo(&ptunt);
		return(fFalse);
		}

	ReleasePpo(&ptunt);
	SetTypeCore(fStory);
	return(fTrue);
}


/****************************************************
 *
 * Sets the bounding rectangle of the textbox.
 *
 * Parameters:
 *	prc - The new rectangle
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::SetRc(RC *prc)
{
	AssertThis(0);
	AssertPvCb(prc, size(RC));

	if (_rc != *prc)
		{
        if (Pscen() != pvNil)
        	{
        	Pscen()->MarkDirty();
			}

		_rc = *prc;
		}
}


/****************************************************
 *
 * Returns if the text box is currently visible or not.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if visible, else fFalse.
 *
 ****************************************************/
bool TBOX::FIsVisible(void)
{
	AssertThis(0);
	return((_nfrmCur >= _nfrmFirst) && (_nfrmCur < _nfrmMax));
}


/***************************************************************************
 *
 * Returns the starting and ending frames the text box appears in.
 *
 * Parameters:
 *  pnfrmStart - Pointer to storage for first frame.
 *  pnfrmLast - Pointer to storage for final frame.
 *
 * Returns:
 *  fTrue if the lifetime is valid, else fFalse.
 *
 **************************************************************************/
bool TBOX::FGetLifetime(long *pnfrmStart, long *pnfrmLast)
{
	AssertThis(0);
	AssertVarMem(pnfrmStart);
	AssertVarMem(pnfrmLast);

	if (_nfrmMax < _nfrmFirst)
		{
		return(fFalse);
		}
	
	*pnfrmStart = _nfrmFirst;

	if (_nfrmMax == klwMax)
		{
        *pnfrmLast = Pscen()->NfrmLast();
		}
	else
		{
		*pnfrmLast = _nfrmMax - 1;
		}

	return(fTrue);
}

/****************************************************
 *
 * Selects/Deselects a text box.
 *
 * Parameters:
 *	fSel - fTrue if the text box is to be selected, else
 *		fFalse.
 *	
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::Select(bool fSel)
{
	AssertThis(0);

	PTBXG ptbxg;

	if (fSel == _fSel)
		{
		return;
		}

	_fSel = fSel;

	if (Cddg() > 0)
		{
		Assert(Cddg() == 1, "Multiple views on text boxes not allowed");
		ptbxg = (PTBXG)PddgGet(0);
		AssertPo(ptbxg,0);
		ptbxg->Ptbxb()->Activate(fSel);
		ptbxg->Ptbxb()->InvalRc(pvNil);
		}
}


/****************************************************
 *
 * Makes a text box goto a certain frame.
 *
 * Parameters:
 *	nfrm - The destination frame number.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TBOX::FGotoFrame(long nfrm)
{
	AssertThis(0);

	GCB gcb;
	PTBXG ptbxg;
	PTBXB ptbxb;

	if (Cddg() == 0)
		{
        ptbxg = pvNil;
		}
    else
		{
        ptbxg = (PTBXG)PddgGet(0);
		}
	AssertNilOrPo(ptbxg, 0);

	_nfrmCur = nfrm;

	if (FIsVisible())
		{

		if (ptbxg == pvNil)
			{

	        //
			// Create a GOB for this text box
			//
			gcb.Set(khidDdg, Pscen()->Pmvie()->PddgActive(), fgobNil,
				kginMark, &_rc, pvNil);
			ptbxb = TBXB::PtbxbNew(this, &gcb);
			if (ptbxb == pvNil)
				{
				return(fFalse);
				}

			if (Pscen()->Pmvie()->FPlaying())
				{
				ptbxb->Activate(fFalse);
				}

			}

		}

	if (!FIsVisible() && (ptbxg != pvNil))
		{
        //
		// Release the GOB for the text box.
		//
        ptbxb = ptbxg->Ptbxb();
		ReleasePpo(&ptbxb);
		}

	return(fTrue);
}



/****************************************************
 *
 * Makes a text box visible at a certain frame.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TBOX::FShowCore(void)
{
	AssertThis(0);

	Pscen()->MarkDirty();
	if (_nfrmMax < _nfrmCur)
		{
		Assert(_nfrmCur < klwMax, "Current frame too big");
		_nfrmMax = _nfrmCur + 1;
		}
	else if (_nfrmCur < _nfrmFirst)
		{
		_nfrmFirst = _nfrmCur;
		}

	return (FGotoFrame(_nfrmCur));
}


/****************************************************
 *
 * Makes a text box visible at a certain frame and an undo.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TBOX::FShow(void)
{
	AssertThis(0);

    PTUNH ptunh;

	ptunh = TUNH::PtunhNew();
	if (ptunh == pvNil)
		{
		return(fFalse);
		}

    ptunh->SetFrmLast(_nfrmMax);
    ptunh->SetFrmFirst(_nfrmFirst);
	ptunh->SetItbox(Itbox());

	if (!Pscen()->Pmvie()->FAddUndo(ptunh))
		{
		ReleasePpo(&ptunh);
		return(fFalse);
		}

    ReleasePpo(&ptunh);

	if (FShowCore())
		{
        Pscen()->Pmvie()->ClearUndo();
		return(fTrue);
		}

	return(fFalse);
}

/****************************************************
 *
 * Removes a text box at a certain frame.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
void TBOX::HideCore(void)
{
	AssertThis(0);

    _nfrmMax = _nfrmCur;
	Pscen()->MarkDirty();
	AssertDo(FGotoFrame(_nfrmCur), "Could not goto frame");
	
	if (Pscen()->PtboxSelected() == this)
		{
		Pscen()->SelectTbox(pvNil);
		}
}

/****************************************************
 *
 * Removes a text box at a certain frame and has an undo.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TBOX::FHide(void)
{
	AssertThis(0);

    PTUNH ptunh;

	if (_nfrmCur <= _nfrmFirst)
		{
		return(Pscen()->FRemTbox(this));
		}

	ptunh = TUNH::PtunhNew();
	if (ptunh == pvNil)
		{
		return(fFalse);
		}

    ptunh->SetFrmLast(_nfrmMax);
    ptunh->SetFrmFirst(_nfrmFirst);
	ptunh->SetItbox(Itbox());

	if (!Pscen()->Pmvie()->FAddUndo(ptunh))
		{
		ReleasePpo(&ptunh);
		return(fFalse);
		}

    ReleasePpo(&ptunh);

	HideCore();

	return(fTrue);
}


/****************************************************
 *
 * Copies an entire text box.
 *
 * Parameters:
 *	pptbox - Place to store the new text box.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TBOX::FDup(PTBOX *pptbox)
{
	AssertThis(0);
	*pptbox = TBOX::PtboxNew(pvNil, &_rc);
	AssertNilOrPo(*pptbox, 0);

	if (*pptbox == pvNil)
		{
		return(fFalse);
		}

	(*pptbox)->SuspendUndo();
	if (!(*pptbox)->FReplaceTxrd(this, 0, CpMac(), 0, (*pptbox)->CpMac()))
		{
        ReleasePpo(pptbox);
		return(fFalse);
		}
	(*pptbox)->_onnDef = _onnDef;
	(*pptbox)->_dypFontDef = _dypFontDef;
	(*pptbox)->ResumeUndo();
	(*pptbox)->SetAcrBack(AcrBack(), fdocNil);
	(*pptbox)->SetTypeCore(FStory());

	return(fTrue);
}

/****************************************************
 *
 * Sets the background color of the text box.
 *
 * Parameters:
 *	acr - The color for the background.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TBOX::FSetAcrBack(ACR acr)
{
	AssertThis(0);

	PTUNC ptunc;

	ptunc = TUNC::PtuncNew();

	if (ptunc == pvNil)
		{
		return(fFalse);
		}

    AssertPo(ptunc, 0);

	ptunc->SetItbox(Itbox());
	ptunc->SetAcrBack(AcrBack());

	if (!Pscen()->Pmvie()->FAddUndo(ptunc))
		{
        ReleasePpo(&ptunc);
		return(fFalse);
		}

    ReleasePpo(&ptunc);

	SetAcrBack(acr);
	return(fTrue);
}

/****************************************************
 *
 * Sets the color for text in the text box.
 *
 * Parameters:
 *	acr - The color for the text.
 *
 * Returns:
 *  fTrue if possible, else fFalse.
 *
 ****************************************************/
bool TBOX::FSetAcrText(ACR acr)
{
	AssertThis(0);

	PTBXG ptbxg;
	CHP chpNew, chpDiff;

	ptbxg = (PTBXG)PddgGet(0);
	AssertNilOrPo(ptbxg, 0);

	if (ptbxg == pvNil)
		{
        return(fFalse);
		}

	chpNew.Clear();
	chpDiff.Clear();

	chpNew.acrFore = acr;
	return(ptbxg->FApplyChp(&chpNew, &chpDiff));
}

/******************************************************************************
	FSetDypFontText
		Sets the font size for the current selection of the textbox
	
	Arguments:
		long dypFont  --  The new font size
	
	Returns:  fTrue if it succeeds
	
************************************************************ PETED ***********/
bool TBOX::FSetDypFontText(long dypFont)
{
	AssertThis(0);

	PTBXG ptbxg;
	CHP chpNew, chpDiff;

	ptbxg = (PTBXG)PddgGet(0);
	if (ptbxg == pvNil)
		return fFalse;
	AssertPo(ptbxg, 0);
	Assert(ptbxg->FIs(kclsTBXG), "DDG isn't a TBXG");

	chpNew.Clear();
	chpDiff.Clear();
	chpNew.dypFont = dypFont;
	chpDiff.dypFont = ~dypFont;
	return ptbxg->FApplyChp(&chpNew, &chpDiff);
}

/******************************************************************************
	FSetStyleText
		Sets the font style for the current selection of the textbox
	
	Arguments:
		long grfont  --  The new font style
	
	Returns:  fTrue if it succeeds
	
************************************************************ PETED ***********/
bool TBOX::FSetStyleText(ulong grfont)
{
	AssertThis(0);

	PTBXG ptbxg;
	CHP chpNew, chpDiff;

	ptbxg = (PTBXG)PddgGet(0);
	if (ptbxg == pvNil)
		return fFalse;
	AssertPo(ptbxg, 0);
	Assert(ptbxg->FIs(kclsTBXG), "DDG isn't a TBXG");

	chpNew.Clear();
	chpDiff.Clear();
	chpNew.grfont = grfont;
	chpDiff.grfont = ~grfont;
	return ptbxg->FApplyChp(&chpNew, &chpDiff);
}

/******************************************************************************
	FSetOnnText
		Sets the font face for the current selection of the textbox
	
	Arguments:
		long onn  --  the new font face
	
	Returns:  fTrue if it succeeds
	
************************************************************ PETED ***********/
bool TBOX::FSetOnnText(long onn)
{
	AssertThis(0);

	PTBXG ptbxg;
	CHP chpNew, chpDiff;

	ptbxg = (PTBXG)PddgGet(0);
	if (ptbxg == pvNil)
		return fFalse;
	AssertPo(ptbxg, 0);
	Assert(ptbxg->FIs(kclsTBXG), "DDG isn't a TBXG");

	chpNew.Clear();
	chpDiff.Clear();
	chpNew.onn = onn;
	chpDiff.onn = ~onn;
	return ptbxg->FApplyChp(&chpNew, &chpDiff);
}


/******************************************************************************
	FetchChpSel
		Gets the character formatting for the current selection of the active
		DDG for this TBOX.  Returns the formatting of the first character of
		the selection in the CHP, and sets the corresponding bit in *pgrfchp
		if that particular formatting holds for the entire selection.
	
	Arguments:
		PCHP pchp       --  the CHP to take the formatting info
		ulong *pgrfchp  --  bitfield that indicates which formatting attributes
			hold for the entire selection.
************************************************************ PETED ***********/
void TBOX::FetchChpSel(PCHP pchp, ulong *pgrfchp)
{
	AssertVarMem(pchp);
	AssertVarMem(pgrfchp);

	long cpMin, cpMac;
	long cpMinChp, cpMacChp;
	CHP chp;
	PTXTG ptxtg;

	ptxtg = (PTXTG)PddgActive();
	if (ptxtg == pvNil)
		goto LFail;
	if (!ptxtg->FIs(kclsTXTG))
		{
		Bug("DDG isn't a TXTG");
LFail:
		*pgrfchp = 0;
		return;
		}
	ptxtg->GetSel(&cpMin, &cpMac);
	if (cpMin > cpMac)
		SwapVars(&cpMin, &cpMac);

	*pgrfchp = kgrfchpAll;
	FetchChp(cpMin, pchp, &cpMinChp, &cpMacChp);
	while (cpMacChp < cpMac && *pgrfchp != grfchpNil)
		{
		FetchChp(cpMacChp, &chp, &cpMinChp, &cpMacChp);
		if ((*pgrfchp & kfchpOnn) && pchp->onn != chp.onn)
			*pgrfchp ^= kfchpOnn;
		if ((*pgrfchp & kfchpDypFont) && pchp->dypFont != chp.dypFont)
			*pgrfchp ^= kfchpDypFont;
		if ((*pgrfchp & kfchpBold) && ((pchp->grfont ^ chp.grfont) & fontBold))
			{
			*pgrfchp ^= kfchpBold;
			}
		if ((*pgrfchp & kfchpItalic) && ((pchp->grfont ^ chp.grfont) & fontItalic))
			{
			*pgrfchp ^= kfchpItalic;
			}
		}
}


/****************************************************
 *
 * Decides if a text box needs to scroll right now.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if yes, else fFalse.
 *
 ****************************************************/
bool TBOX::FNeedToScroll(void)
{
	AssertThis(0);

	PTBXG ptbxg;

	if ((_nfrmCur == _nfrmFirst) && !_fStory)
		{
        ptbxg = (PTBXG)PddgGet(0);
		AssertPo(ptbxg, 0);
		return (ptbxg->FNeedToScroll());
		}
	return(fFalse);
}

/****************************************************
 *
 * Scrolls up by one line.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::Scroll(void)
{
	AssertThis(0);

	PTBXG ptbxg;

    ptbxg = (PTBXG)PddgGet(0);
	AssertPo(ptbxg, 0);
    ptbxg->Scroll(scaLineDown);
}

/****************************************************
 *
 * Tells if any text is selected or not.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
bool TBOX::FTextSelected(void)
{
	AssertThis(0);

	PTBXG ptbxg;

	if (!FIsVisible())
		{
		return(fFalse);
		}

    ptbxg = (PTBXG)PddgGet(0);
	AssertPo(ptbxg, 0);
    return(ptbxg->FTextSelected());
}

/****************************************************
 *
 * Sets the starting frame number for a text box.
 *
 * Parameters:
 *	nfrm - The new starting frame number.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::SetStartFrame(long nfrm)
{
	AssertThis(0);
	_nfrmFirst = nfrm;
}

/****************************************************
 *
 * Adds an undo object to the movie.
 *
 * Parameters:
 *	pundb - The undo object to do.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TBOX::FAddUndo(PUNDB pundb)
{
	AssertThis(0);
	AssertPo(Pscen(), 0);

	PTUND ptund;

	ptund = TUND::PtundNew(pundb);

	if (ptund == pvNil)
		{
		return(fFalse);
		}

    AssertPo(ptund, 0);

	ptund->SetItbox(Itbox());

	if (!Pscen()->Pmvie()->FAddUndo(ptund))
		{
        Pscen()->Pmvie()->ClearUndo();
        ReleasePpo(&ptund);
		return(fFalse);
		}

 	if (!TBOX_PAR::FAddUndo(pundb))
		{
        ReleasePpo(&ptund);
		return(fFalse);
		}

   	ReleasePpo(&ptund);
	return(fTrue);
}

/****************************************************
 *
 * Clears the undo buffer.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::ClearUndo()
{
	AssertThis(0);
	AssertPo(Pscen(), 0);
	TBOX_PAR::ClearUndo();
	Pscen()->Pmvie()->ClearUndo();
}



/****************************************************
 *
 * Ensure that the DDG for this tbox is the proper size,
 * used for cleaning after a playback.
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::CleanDdg(void)
{
	AssertThis(0);

	PDDG pddg;
	RC rcAbs, rcRel;

	pddg = PddgGet(0);
	if (pddg == pvNil)
		{
		return;
		}

	AssertPo(pddg, 0);

	pddg->GetPos(&rcAbs, &rcRel);
	rcAbs.Set(kdzpBorderTbox, kdzpBorderTbox, -kdzpBorderTbox, -kdzpBorderTbox);
	pddg->SetPos(&rcAbs, &rcRel);
}

/****************************************************
 *
 * Get the Itbox number for this tbox.
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  Itbox.
 *
 ****************************************************/
long TBOX::Itbox(void)
{
	AssertThis(0);

	long itbox;

	for (itbox = 0; ; itbox++)
		{
		if (this == Pscen()->PtboxFromItbox(itbox))
			{
			break;
			}
		}

	return(itbox);
}


#ifdef DEBUG

/****************************************************
 * Mark memory used by the TBOX
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TBOX::MarkMem(void)
{
	AssertThis(0);
	TBOX_PAR::MarkMem();
}

/***************************************************************************
 *
 * Assert the validity of the TBOX.
 *
 * Parameters:
 *	grf - Bit field of options
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void TBOX::AssertValid(ulong grf)
{
	TBOX_PAR::AssertValid(fobjAllocated);
	if (PddgGet(0) != pvNil)
		{
        AssertPo(PddgGet(0), 0);
		}
}

#endif //DEBUG



//
//
//
// BEGIN UNDO STUFF
//
//
//



/****************************************************
 *
 * Public constructor for textbox undo objects.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PTUNT TUNT::PtuntNew()
{
	PTUNT ptunt;
	ptunt = NewObj TUNT();
	return(ptunt);
}

/****************************************************
 *
 * Destructor for text box undo objects
 *
 ****************************************************/
TUNT::~TUNT(void)
{
}


/****************************************************
 *
 * Does a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TUNT::FDo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	PTBOX ptbox;
	bool fStory;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
		goto LFail;
		}

    ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
	AssertNilOrPo(ptbox, 0);

	if (ptbox == pvNil)
		{
		goto LFail;
		}

	fStory = ptbox->FStory();
	ptbox->SetTypeCore(_fStory);
	_fStory = fStory;
	_pmvie->Pscen()->SelectTbox(ptbox);
	_pmvie->Pmsq()->FlushMsq();
	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);

}

/****************************************************
 *
 * Undoes a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TUNT::FUndo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	return(FDo(pdocb));
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the TUNT
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TUNT::MarkMem(void)
{
	AssertThis(0);
	TUNT_PAR::MarkMem();
}

/***************************************************************************
	Assert the validity of the TUNT.
***************************************************************************/
void TUNT::AssertValid(ulong grf)
{
	TUNT_PAR::AssertValid(fobjAllocated);
}
#endif


/****************************************************
 *
 * Public constructor for textbox undo objects.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PTUNS TUNS::PtunsNew()
{
	PTUNS ptuns;
	ptuns = NewObj TUNS();
	return(ptuns);
}

/****************************************************
 *
 * Destructor for text box undo objects
 *
 ****************************************************/
TUNS::~TUNS(void)
{
}


/****************************************************
 *
 * Does a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TUNS::FDo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	PTBOX ptbox;
	PTBXG ptbxg;
	RC rc;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
		goto LFail;
		}

    ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
	AssertNilOrPo(ptbox, 0);

	if (ptbox == pvNil)
		{
		goto LFail;
		}

	ptbox->GetRc(&rc);
	ptbox->SetRc(&_rc);

	ptbxg = (PTBXG)ptbox->PddgGet(0);
	AssertPo(ptbxg, 0);
	ptbxg->Ptbxb()->SetPos(&_rc);
	_pmvie->Pscen()->SelectTbox(ptbox);
	_rc = rc;
	_pmvie->Pmsq()->FlushMsq();
	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);
}

/****************************************************
 *
 * Undoes a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TUNS::FUndo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	return(FDo(pdocb));
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the TUNS
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TUNS::MarkMem(void)
{
	AssertThis(0);
	TUNS_PAR::MarkMem();
}

/***************************************************************************
	Assert the validity of the TUNS.
***************************************************************************/
void TUNS::AssertValid(ulong grf)
{
	TUNS_PAR::AssertValid(fobjAllocated);
}
#endif





/****************************************************
 *
 * Public constructor for textbox undo objects.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PTUNH TUNH::PtunhNew()
{
	PTUNH ptunh;
	ptunh = NewObj TUNH();
	return(ptunh);
}

/****************************************************
 *
 * Destructor for text box undo objects
 *
 ****************************************************/
TUNH::~TUNH(void)
{
	AssertBaseThis(0);
}


/****************************************************
 *
 * Does a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TUNH::FDo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	PTBOX ptbox;
	long nfrmFirst, nfrmMax;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
		goto LFail;
		}

    ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
	AssertNilOrPo(ptbox, 0);

	if (ptbox == pvNil)
		{
		goto LFail;
		}

    nfrmFirst = ptbox->NfrmFirst();
	nfrmMax = ptbox->nfrmMax();

	if (!ptbox->FGotoFrame(_nfrmFirst))
		{
		goto LFail;
		}

    if (!ptbox->FShowCore())
		{
		goto LFail;
		}

	if (!ptbox->FGotoFrame(_nfrmMax))
		{
		goto LFail;
		}

    ptbox->HideCore();
	ptbox->FGotoFrame(_nfrm);
	_pmvie->Pscen()->SelectTbox(ptbox);

	_nfrmFirst = nfrmFirst;
	_nfrmMax = nfrmMax;
	_pmvie->Pmsq()->FlushMsq();
	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);
}

/****************************************************
 *
 * Undoes a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TUNH::FUndo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	return(FDo(pdocb));
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the TUNH
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TUNH::MarkMem(void)
{
	AssertThis(0);
	TUNH_PAR::MarkMem();
}

/***************************************************************************
	Assert the validity of the TUNH.
***************************************************************************/
void TUNH::AssertValid(ulong grf)
{
	TUNH_PAR::AssertValid(fobjAllocated);
}
#endif





/****************************************************
 *
 * Public constructor for textbox undo objects which
 * are the result of the document changing.
 *
 * Parameters:
 *	pundb - the undo object to encapsulate.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PTUND TUND::PtundNew(PUNDB pundb)
{
	AssertPo(pundb, 0);

	PTUND ptund;
	ptund = NewObj TUND();
	if (ptund != pvNil)
		{
        ptund->_pundb = pundb;
		pundb->AddRef();
		AssertPo(pundb, 0);
		}
	return(ptund);
}

/****************************************************
 *
 * Destructor for text box undo objects
 *
 ****************************************************/
TUND::~TUND(void)
{
	AssertBaseThis(0);

	PTBOX ptbox;

	//
	// Clear the owning tbox undo list
	//
	if (_iscen == _pmvie->Iscen())
		{
		ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
		AssertNilOrPo(ptbox, 0);
	
		if (ptbox != pvNil)
			{
			ptbox->ParClearUndo();
			}
		}

	ReleasePpo(&_pundb);
}


/****************************************************
 *
 * Does a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TUND::FDo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	PTBOX ptbox;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
		goto LFail;
		}

    ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
	AssertNilOrPo(ptbox, 0);

	if (ptbox == pvNil)
		{
		goto LFail;
		}

	_pmvie->Pscen()->SelectTbox(ptbox);
	_pmvie->Pmsq()->FlushMsq();
	return(_pundb->FDo(ptbox));

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);
}

/****************************************************
 *
 * Undoes a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TUND::FUndo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	PTBOX ptbox;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
		goto LFail;
		}

    ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
	AssertNilOrPo(ptbox, 0);

	if (ptbox == pvNil)
		{
		goto LFail;
		}

	_pmvie->Pscen()->SelectTbox(ptbox);
	_pmvie->Pmsq()->FlushMsq();
    return(_pundb->FUndo(ptbox));

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the TUND
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TUND::MarkMem(void)
{
	AssertThis(0);
	TUND_PAR::MarkMem();
	MarkMemObj(_pundb);
}

/***************************************************************************
	Assert the validity of the TUND.
***************************************************************************/
void TUND::AssertValid(ulong grf)
{
	AssertBaseThis(0);
}
#endif





/****************************************************
 *
 * Public constructor for textbox undo objects which
 * are the result of background color changing.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  pvNil if failure, else a pointer to the movie undo.
 *
 ****************************************************/
PTUNC TUNC::PtuncNew(void)
{
	PTUNC ptunc;

	ptunc = NewObj TUNC();
	return(ptunc);
}

/****************************************************
 *
 * Destructor for text box undo objects
 *
 ****************************************************/
TUNC::~TUNC(void)
{
	AssertBaseThis(0);
}


/****************************************************
 *
 * Does a command stored in an undo object.
 *
 * Parameters:
 *	pdocb - The owning docb.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TUNC::FDo(PDOCB pdocb)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	PTBOX ptbox;
	ACR acr;

   	if (!_pmvie->FSwitchScen(_iscen))
   		{
		goto LFail;
		}

	if (!_pmvie->Pscen()->FGotoFrm(_nfrm))
		{
		goto LFail;
		}

    ptbox = _pmvie->Pscen()->PtboxFromItbox(_itbox);
	AssertNilOrPo(ptbox, 0);

	if (ptbox == pvNil)
		{
		goto LFail;
		}

    acr = ptbox->AcrBack();
	ptbox->SetAcrBack(_acr);
	_acr = acr;
	_pmvie->Pscen()->SelectTbox(ptbox);
	_pmvie->Pmsq()->FlushMsq();
	return(fTrue);

LFail:
	_pmvie->Pmsq()->FlushMsq();
	_pmvie->ClearUndo();
	return(fFalse);
}

/****************************************************
 *
 * Undoes a command stored in an undo object.
 *
 * Parameters:
 *	None.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 ****************************************************/
bool TUNC::FUndo(PDOCB pdocb)
{
    return(FDo(pdocb));
}

#ifdef DEBUG
/****************************************************
 * Mark memory used by the TUNC
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TUNC::MarkMem(void)
{
	AssertThis(0);
	TUNC_PAR::MarkMem();
}

/***************************************************************************
	Assert the validity of the TUNC.
***************************************************************************/
void TUNC::AssertValid(ulong grf)
{
	AssertBaseThis(0);
}
#endif


//
//
//
//
// BEGIN TCLP
//
//
//
//

/***************************************************************************
 *
 * Destructor for text box clipboard documents
 *
 **************************************************************************/
TCLP::~TCLP(void)
{
	ReleasePpo(&_ptbox);
}

/***************************************************************************
 *
 * Destructor for text box clipboard documents
 *
 * Parameters:
 *	ptbox - The tbox to associate with the clipboard.
 *
 * Returns:
 *  Pointer to a clipboard document, or pvNil if failure.
 *
 **************************************************************************/
PTCLP TCLP::PtclpNew(PTBOX ptbox)
{
	AssertPo(ptbox, 0);

	PTCLP ptclp;

	ptclp = NewObj TCLP();

	if (ptclp == pvNil)
		{
		return(pvNil);
		}

	ptclp->_ptbox = ptbox;
	return(ptclp);
}

/***************************************************************************
 *
 * Pastes the text box associated with this clipboard object to the
 * current frame.
 *
 * Parameters:
 *	pscen - The scene to paste into.
 *
 * Returns:
 *  fTrue if successful, else fFalse.
 *
 **************************************************************************/
bool TCLP::FPaste(PSCEN pscen)
{
	AssertThis(0);
	AssertPo(pscen, 0);

	PTBOX ptboxDup;
	RC rc;
	RC rcWorkspace(kxpDefaultTbox, kypDefaultTbox, pscen->Pmvie()->Pmcc()->Dxp(), pscen->Pmvie()->Pmcc()->Dyp());

	//
	// Copy the text box
	//
	if (!_ptbox->FDup(&ptboxDup))
		{
        pscen->Pmvie()->ClearUndo();
		return(fFalse);
		}
    AssertPo(ptboxDup, 0);

	ptboxDup->GetRc(&rc);

	rc.Offset(kxpDefaultTbox - rc.xpLeft, kypDefaultTbox - rc.ypTop);
	rc.FIntersect(&rcWorkspace);

	ptboxDup->SetRc(&rc);

	if (!pscen->FAddTbox(ptboxDup))
		{
        pscen->Pmvie()->ClearUndo();
		ReleasePpo(&ptboxDup);
		return(fFalse);
		}

	pscen->SelectTbox(ptboxDup);
	ptboxDup->AttachToMouse();
	ReleasePpo(&ptboxDup);

	return(fTrue);
}

#ifdef DEBUG

/****************************************************
 * Mark memory used by the TCLP
 *
 * Parameters:
 * 	None.
 *
 * Returns:
 *  None.
 *
 ****************************************************/
void TCLP::MarkMem(void)
{
	AssertThis(0);
	TCLP_PAR::MarkMem();
	MarkMemObj(_ptbox);
}

/***************************************************************************
 *
 * Assert the validity of the TCLP.
 *
 * Parameters:
 *	grf - Bit field of options
 *
 * Returns:
 *  None.
 *
 **************************************************************************/
void TCLP::AssertValid(ulong grf)
{
	TCLP_PAR::AssertValid(fobjAllocated);
	AssertPo(_ptbox, 0);
}

#endif //DEBUG


