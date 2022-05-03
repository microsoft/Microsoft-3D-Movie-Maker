/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	tatr.h: Theater class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	BASE ---> BACO ---> CMH ---> TATR

***************************************************************************/
#ifndef TATR_H
#define TATR_H

#ifdef DEBUG // Flags for TATR::AssertValid()
enum
	{
	ftatrNil = 0x0000,
	ftatrMvie = 0x0001,
	};
#endif //DEBUG

/****************************************
	The theater class
****************************************/
typedef class TATR *PTATR;
#define TATR_PAR CMH
#define kclsTATR 'TATR'
class TATR : public TATR_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	CMD_MAP_DEC(TATR)

protected:
	long _kidParent;	// ID of gob parent of MVU
	PMVIE _pmvie;		// Currently loaded movie

protected:
	TATR(long hid) : CMH(hid) { }
	bool _FInit(long kidParent);

public:
	static PTATR PtatrNew(long kidParent);
	~TATR(void);

	bool FCmdLoad(PCMD pcmd);
	bool FCmdPlay(PCMD pcmd);
	bool FCmdStop(PCMD pcmd);
	bool FCmdRewind(PCMD pcmd);

	PMVIE Pmvie(void) { return _pmvie; }
	};


#endif TATR_H
