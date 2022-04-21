/***************************************************************************

	DOCPIC methods.

***************************************************************************/
#include "ched.h"
ASSERTNAME

RTCLASS(DOCPIC)
RTCLASS(DCPIC)


/***************************************************************************
	Constructor for picture document.
***************************************************************************/
DOCPIC::DOCPIC(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno)
	: DOCE(pdocb, pcfl, ctg, cno)
{
	_ppic = pvNil;
}


/***************************************************************************
	Destructor for picture document.
***************************************************************************/
DOCPIC::~DOCPIC(void)
{
	ReleasePpo(&_ppic);
}


/***************************************************************************
	Static method to create a new picture document.
***************************************************************************/
PDOCPIC DOCPIC::PdocpicNew(PDOCB pdocb, PCFL pcfl, CTG ctg, CNO cno)
{
	DOCPIC *pdocpic;

	if (pvNil == (pdocpic = NewObj DOCPIC(pdocb, pcfl, ctg, cno)))
		return pvNil;
	if (!pdocpic->_FInit())
		{
		ReleasePpo(&pdocpic);
		return pvNil;
		}
	AssertPo(pdocpic, 0);
	return pdocpic;
}


/***************************************************************************
	Create a new display gob for the document.
***************************************************************************/
PDDG DOCPIC::PddgNew(PGCB pgcb)
{
	return DCPIC::PdcpicNew(this, _ppic, pgcb);
}


/***************************************************************************
	Return the size of the thing on file.
***************************************************************************/
long DOCPIC::_CbOnFile(void)
{
	return _ppic->CbOnFile();
}


/***************************************************************************
	Write the data out.
***************************************************************************/
bool DOCPIC::_FWrite(PBLCK pblck, bool fRedirect)
{
	AssertThis(0);
	AssertPo(pblck, 0);

	return _ppic->FWrite(pblck);
}


/***************************************************************************
	Read the PIC.
***************************************************************************/
bool DOCPIC::_FRead(PBLCK pblck)
{
	Assert(_ppic == pvNil, "losing existing PIC");
	AssertPo(pblck, 0);

	_ppic = PIC::PpicRead(pblck);
	return _ppic != pvNil;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a DOCPIC.
***************************************************************************/
void DOCPIC::AssertValid(ulong grf)
{
	DOCPIC_PAR::AssertValid(0);
	AssertPo(_ppic, 0);
}


/***************************************************************************
	Mark memory for the DOCPIC.
***************************************************************************/
void DOCPIC::MarkMem(void)
{
	AssertValid(0);
	DOCPIC_PAR::MarkMem();
	MarkMemObj(_ppic);
}
#endif //DEBUG


/***************************************************************************
	Constructor for a pic display gob.
***************************************************************************/
DCPIC::DCPIC(PDOCB pdocb, PPIC ppic, PGCB pgcb) : DDG(pdocb, pgcb)
{
	_ppic = ppic;
}


/***************************************************************************
	Get the min-max for a DCPIC.
***************************************************************************/
void DCPIC::GetMinMax(RC *prcMinMax)
{
	prcMinMax->Set(20, 20, kswMax, kswMax);
}


/***************************************************************************
	Static method to create a new DCPIC.
***************************************************************************/
PDCPIC DCPIC::PdcpicNew(PDOCB pdocb, PPIC ppic, PGCB pgcb)
{
	PDCPIC pdcpic;

	if (pvNil == (pdcpic = NewObj DCPIC(pdocb, ppic, pgcb)))
		return pvNil;

	if (!pdcpic->_FInit())
		{
		ReleasePpo(&pdcpic);
		return pvNil;
		}
	pdcpic->Activate(fTrue);

	AssertPo(pdcpic, 0);
	return pdcpic;
}


/***************************************************************************
	Draw the picture.
***************************************************************************/
void DCPIC::Draw(PGNV pgnv, RC *prcClip)
{
	RC rc, rcSrc;

	pgnv->GetRcSrc(&rcSrc);
	pgnv->FillRc(&rcSrc, kacrWhite);
	_ppic->GetRc(&rc);
	rcSrc.Inset(5, 5);
	rc.StretchToRc(&rcSrc);
	pgnv->DrawPic(_ppic, &rc);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a DCPIC.
***************************************************************************/
void DCPIC::AssertValid(ulong grf)
{
	DCPIC_PAR::AssertValid(0);
	AssertPo(_ppic, 0);
}


/***************************************************************************
	Mark memory for the DCPIC.
***************************************************************************/
void DCPIC::MarkMem(void)
{
	AssertValid(0);
	DCPIC_PAR::MarkMem();
	MarkMemObj(_ppic);
}
#endif //DEBUG
