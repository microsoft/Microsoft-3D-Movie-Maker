/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/*****************************************************************************\
 *
 *	stdioscb.h
 *
 *	Author: ******
 *	Date: March, 1995
 *
 *	This file contains the studio scrollbar class SSCB.
 *
\*****************************************************************************/

#ifndef STDIOSCB_H
#define STDIOSCB_H

//
//	The studio scrollbar class.
//

const long kctsFps = 20;

#define SSCB_PAR BASE
typedef class SSCB *PSSCB;
#define kclsSSCB 'SSCB'
class SSCB : public SSCB_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

private:
	long _nfrmFirstOld;
	bool _fNoAutoadjust;

	bool _fBtnAddsFrames;

	//
	//	Private methods
	//
	long _CxScrollbar(long kidScrollbar, long kidThumb);

protected:

	PTGOB _ptgobFrame;
	PTGOB _ptgobScene;

#ifdef SHOW_FPS
	// Frame descriptor
	struct FDSC
		{
		ulong ts;
		long cfrm;
		};

	PTGOB _ptgobFps;
	FDSC _rgfdsc[kctsFps];
	long _itsNext;
#endif // SHOW_FPS

	PMVIE _pmvie;
	SSCB(PMVIE pmvie);

public:

	//
	//	Constructors and destructors
	//
	static PSSCB PsscbNew(PMVIE pmvie);
    ~SSCB(void);

	//
	//	Notification
	//
	virtual void Update(void);
	void SetMvie(PMVIE pmvie);
	void StartNoAutoadjust(void);
	void EndNoAutoadjust(void) { AssertThis(0); _fNoAutoadjust = fFalse; }
	void SetSndFrame(bool fSoundInFrame);

	//
	//	Event handling
	//
	bool FCmdScroll(PCMD pcmd);
	};

#endif // STDIOSCB_H
