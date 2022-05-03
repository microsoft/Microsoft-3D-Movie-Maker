/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

  tgob.cpp

  Author: Sean Selitrennikoff

  Date: June, 1995

  This file contains all functionality for a whole lite text-gob.

***************************************************************************/

#include "studio.h"

ASSERTNAME
RTCLASS(TGOB)

/****************************************************
 *
 * Constructor for text gobs.
 *
 ****************************************************/
TGOB::TGOB(GCB *pgcb) : GOB(pgcb)
{
	_acrFore = kacrBlack;
	_acrBack = kacrClear;
	_tav = tavTop;
	_tah = tahCenter;
	_onn = vapp.OnnDefVariable();
	_dypFont = vapp.DypTextDef();
}


/****************************************************
 *
 * Constructor for text gobs.
 *
 ****************************************************/
TGOB::TGOB(long hid) : GOB(hid)
{
	_acrFore = kacrBlack;
	_acrBack = kacrClear;
}


/***************************************************************************
 *
 * Draw this text.
 *
 * Parameters:
 *	pgnv - The environment to write to.
 *  prcClip - The clipping rectangle.
 *
 * Returns:
 *  None.
 *
***************************************************************************/
void TGOB::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);

	RC rc, rcText;
	long xp, yp;
 	STN stnDraw(_stn);

	// Use a temporary stn here for the displayed text. For example, the tgob 
	// text may be 'hello', but the dispayed text may be 'hel..'. This means 
	// any dots will be added to the displayed string if required every time 
	// we pass through here. If we don't do this then the following would
	// happen. On the first pass through here can find that we need to add 
	// the dots and left justify the text, (Eg 'hello' becomes 'hel..'). On
	// the next pass however, we would find that 'hel..' does indeed fit in the
	// tgob ok and we would centre justify it. The user would see the text shift.

	GetRc(&rc, cooLocal);

	/* REVIEW peted: what happens for right-justify?  Why do we have to do
		this at all?  Why doesn't Kauai just take an RC, and then left-, right-,
		or center-align as appropriate, depending on the "SetFontAlign"
		setting? */
	xp = (_tah != tahCenter) ? 0 : rc.Dxp() / 2;
	yp = (_tav != tavCenter) ? 0 : rc.Dyp() / 2;

	pgnv->SetOnn(_onn);
	pgnv->SetFontSize(_dypFont);

	// Center justify the text if it fits in the gob. Otherwise left justify it.
	pgnv->GetRcFromStn(&rcText, &stnDraw, 0, 0);

	if(rcText.Dxp() < rc.Dxp())
		{
		pgnv->SetFontAlign(_tah, _tav);
		pgnv->DrawStn(&stnDraw, xp, yp, _acrFore, _acrBack);
		}
	else
		{
		STN stnDots;
		RC rcDots;
		int iCh;

		stnDots.SetSz(PszLit(".."));
		pgnv->GetRcFromStn(&rcDots, &stnDots, 0, 0);

		for( iCh = stnDraw.Cch() - 1; iCh >= 0; --iCh)
			{
			stnDraw.Delete(iCh);
			stnDraw.FAppendStn(&stnDots);

			pgnv->GetRcFromStn(&rcText, &stnDraw, 0, 0);
				
			if(rcText.Dxp() < rc.Dxp())
				{
				pgnv->SetFontAlign(tahLeft, _tav);
				pgnv->DrawStn(&stnDraw, 0, yp, _acrFore, _acrBack);

				break;
				}
			}
		}
}


/****************************************************
 *
 * Create Tgob's for any of the text based browsers
 * 		using the font specified by idsFont
 * Static routine
 * Parameters
 *		kidFrm = kid of text frame
 *		idsFont = string registry id
 *		tav = text align vertical 
 *
 ****************************************************/	
PTGOB TGOB::PtgobCreate(long kidFrm, long idsFont, long tav, long hid)
{
	RC rcRel;
	RC rcAbs;
	PGOB pgob;
	STN	stn;
	GCB gcb;
	long onn;
	PTGOB ptgob;

	rcRel.xpLeft = rcRel.ypTop = krelZero;
	rcRel.xpRight = rcRel.ypBottom = krelOne;
	rcAbs.Set(0, 0, 0, 0);

	pgob = ((APP *)vpappb)->Pkwa()->PgobFromHid(kidFrm);

	if (pgob == pvNil)
		return pvNil;

	if (hidNil == hid)
		hid = GOB::HidUnique();
	gcb.Set(hid, pgob, fgobNil, kginDefault, &rcAbs, &rcRel);

	if (pvNil == (ptgob = NewObj TGOB(&gcb)))
		return pvNil;

	if (idsFont != idsNil)
		{
		PSTDIO pstdio;

		pstdio = (PSTDIO)vapp.Pkwa()->PgobFromCls(kclsSTDIO);
		Assert(pstdio != pvNil, "Creating a TGOB with no STDIO present");
		pstdio->GetStnMisc(idsFont, &stn);
		vapp.FGetOnn(&stn, &onn); //  Ignore failure
		if (onn != onnNil)
			ptgob->SetFont(onn);
		}
	else
		Assert(ptgob->_onn == vapp.OnnDefVariable(), "Someone else set the _onn?");


	ptgob->_tav = tav;
	return ptgob;
}


/******************************************************************************
	SetAlign
		set the text alignment for this TGOB
	
	Arguments:
		long tah  --  the horizontal alignment
		long tav  --  the vertical alignment
************************************************************ PETED ***********/
void TGOB::SetAlign(long tah, long tav)
{
	if (tah != tahLim)
		_tah = tah;
	if (tav != tavLim)
		_tav = tav;
}


/******************************************************************************
	GetAlign
		Gets the text alignment for this TGOB
	
	Arguments:
		long *ptah  --  pointer to take the horizontal alignment
		long *ptav  --  pointer to take the vertical alignment
************************************************************ PETED ***********/
void TGOB::GetAlign(long *ptah, long *ptav)
{
	if (ptah != pvNil)
		*ptah = _tah;
	if (ptav != pvNil)
		*ptav = _tav;
}


#ifdef DEBUG

/*****************************************************************************
 *
 *	Mark memory used by the TGOB
 *
 *	Parameters:
 *		None.
 *
 *	Returns:
 *		Nothing.
 *
 *****************************************************************************/
void TGOB::MarkMem(void)
{
	AssertThis(0);

	TGOB_PAR::MarkMem();
}

/*****************************************************************************\
 *
 *	Assert the validity of the TGOB
 *
 *	Parameters:
 *		grf - bit array of options.
 *
 *	Returns:
 *		Nothing.
 *
\*****************************************************************************/
void TGOB::AssertValid(ulong grf)
{
	TGOB_PAR::AssertValid(fobjAllocated);
}

#endif // DEBUG

