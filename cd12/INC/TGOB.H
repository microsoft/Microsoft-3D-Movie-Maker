/***************************************************************************

	Lite, low-cholestoral, politically correct, ethinically and genderally
	mixed text gobs.

		TGOB 	--->   	GOB

***************************************************************************/

#ifndef TGOB_H
#define TGOB_H

#include "frame.h"

//
// Tgob class
//
#define TGOB_PAR GOB
#define kclsTGOB 'tgob'
typedef class TGOB *PTGOB;
class TGOB : public TGOB_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:

	long _onn;
	long _dypFont;
	STN _stn;
	long _tah;
	long _tav;
	ACR _acrFore;
	ACR _acrBack;
	~TGOB(void) {}

public:

	//
	// Create and destroy functions
	//
	TGOB(PGCB pgcb);
	TGOB(long hid);

	void SetFont(long onn) { AssertThis(0); _onn = onn; }
	void SetFontSize(long dypFont) { AssertThis(0); _dypFont = dypFont; }
	void SetText(PSTN pstn) { AssertThis(0); _stn = *pstn; InvalRc(pvNil, kginMark); }
	void SetAcrFore(ACR acrFore) { AssertThis(0); _acrFore = acrFore; }
	void SetAcrBack(ACR acrBack) { AssertThis(0); _acrBack = acrBack; }
	void SetAlign(long tah = tahLim, long tav = tavLim);
	long GetFont(void) { AssertThis(0); return(_onn); }
	long GetFontSize(void) { AssertThis(0); return _dypFont; }
	ACR GetAcrFore(void) { AssertThis(0); return(_acrFore); }
	ACR GetAcrBack(void) { AssertThis(0); return(_acrBack); }
	void GetAlign(long *ptah = pvNil, long *ptav = pvNil);
	static PTGOB PtgobCreate(long kidFrm, long idsFont, long tav = tavTop, long hid = hidNil);

	virtual void Draw(PGNV pgnv, RC *prcClip);
	};

#endif

