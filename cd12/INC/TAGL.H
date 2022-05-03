/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	tagl.h: Tag List class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	BASE ---> TAGL

***************************************************************************/
#ifndef TAGL_H
#define TAGL_H

/****************************************
	The tag list class
****************************************/
typedef class TAGL *PTAGL;
#define TAGL_PAR BASE
#define kclsTAGL 'TAGL'
class TAGL : public TAGL_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PGG _pggtagf;	// TAGF for fixed part, array of cc's for variable part

protected:
	bool _FInit(void);
	bool _FFindTag(PTAG ptag, long *pitag);

public:
	static PTAGL PtaglNew(void);
	~TAGL(void);

	long Ctag(void);
	void GetTag(long itag, PTAG ptag);
	
	bool FInsertTag(PTAG ptag, bool fCacheChildren = fTrue);
	bool FInsertChild(PTAG ptag, CHID chid, CTG ctg);

	bool FCacheTags(void);
	};


#endif TAGL_H
