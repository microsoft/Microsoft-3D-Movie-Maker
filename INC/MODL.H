/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/*************************************************************************

	modl.h: Model class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	BASE ---> BACO ---> MODL

*************************************************************************/
#ifndef MODL_H
#define MODL_H

// Model on file:
struct MODLF
	{
	short bo;
	short osk;
	short cver; // count of vertices
	short cfac; // count of faces
	BRS rRadius;
	BRB brb;	// bounds
	BVEC3 bvec3Pivot;
//	br_vertex rgbrv[]; // vertices
//	br_face rgbrf[]; // faces
	};
typedef MODLF *PMODLF;
const BOM kbomModlf = 0x55fffff0;

/****************************************
	MODL: a wrapper for BRender models
****************************************/
typedef class MODL *PMODL;
#define MODL_PAR BACO
#define kclsMODL 'MODL'
class MODL : public MODL_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	BMDL *_pbmdl;	 // BRender model data
protected:
	MODL(void) {}
	bool _FInit(PBLCK pblck);
	bool _FPrelight(long cblit, BVEC3 *prgbvec3Light);

public:
	static PMODL PmodlNew(long cbrv, BRV *prgbrv, long cbrf, BRF *prgbrf);
	static bool FReadModl(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
		PBACO *ppbaco, long *pcb);
	static PMODL PmodlReadFromDat(FNI *pfni);
	static PMODL PmodlFromBmdl(PBMDL pbmdl);
	~MODL(void);
	PBMDL Pbmdl(void) { return _pbmdl; }
	void AdjustTdfCharacter(void);
	bool FWrite(PCFL pcfl, CTG ctg, CNO cno);

	BRS Dxr(void)
		{ return _pbmdl->bounds.max.v[0] - _pbmdl->bounds.min.v[0]; }
	BRS Dyr(void)
		{ return _pbmdl->bounds.max.v[1] - _pbmdl->bounds.min.v[1]; }
	BRS Dzr(void)
		{ return _pbmdl->bounds.max.v[2] - _pbmdl->bounds.min.v[2]; }
	};


#endif TMPL_H
