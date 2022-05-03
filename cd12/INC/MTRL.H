/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/*************************************************************************

	mtrl.h: Material and custom material classes

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	BASE ---> BACO ---> MTRL
	BASE ---> BACO ---> CMTL

*************************************************************************/
#ifndef MTRL_H
#define MTRL_H

// CMTL on File
struct CMTLF
	{
	short bo;
	short osk;
	long ibset; // which body part set this CMTL attaches to
	};
const BOM kbomCmtlf = 0x5c000000;

// material on file (MTRL chunk)
struct MTRLF
	{
	short bo;				// byte order
	short osk;				// OS kind
	br_colour brc;			// RGB color
	br_ufraction brufKa;	// ambient component
	br_ufraction brufKd;	// diffuse component
	br_ufraction brufKs;	// specular component
	byte bIndexBase;		// base of palette for this color
	byte cIndexRange;		// count of entries in palette for this color
	BRS rPower;				// specular exponent
	};
const BOM kbomMtrlf = 0x5D530000;


/****************************************
	The MTRL class.  There are two kinds
	of MTRLs: solid-color MTRLs and
	texmap materials.  Texmap MTRLs have
	TMAPs under the MTRL chunk with chid
	0.
****************************************/
typedef class MTRL *PMTRL;
#define MTRL_PAR BACO
#define kclsMTRL 'MTRL'
class MTRL : public MTRL_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	static PTMAP _ptmapShadeTable; // shade table for all MTRLs
	PBMTL _pbmtl;

protected:
	MTRL(void) { _pbmtl = pvNil; } // can't instantiate directly; must use FReadMtrl
	bool _FInit(PCRF pcrf, CTG ctg, CNO cno);

public:
	static bool FSetShadeTable(PCFL pcfl, CTG ctg, CNO cno);
	static PMTRL PmtrlNew(long iclrBase = ivNil, long cclr = ivNil);
	static bool FReadMtrl(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
		PBACO *ppbaco, long *pcb);
	static PMTRL PmtrlNewFromPix(PFNI pfni);
	static PMTRL PmtrlNewFromBmp(PFNI pfni, PGL pglclr = pvNil);
	static PMTRL PmtrlFromBmtl(PBMTL pbmtl);
	~MTRL(void);
	PTMAP Ptmap(void);
	PBMTL Pbmtl(void) { return _pbmtl; }
	bool FWrite(PCFL pcfl, CTG ctg, CNO *pcno);
#ifdef DEBUG
	static void MarkShadeTable(void);
#endif // DEBUG
	};


/****************************************
	The CMTL (custom material) class
	This manages a set of materials to
	apply to a body part set
****************************************/
typedef class CMTL *PCMTL;
#define CMTL_PAR BACO
#define kclsCMTL 'CMTL'
class CMTL : public CMTL_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PMTRL *_prgpmtrl; // _cbprt PMTRLs, one per body part in this CMTL's set
	PMODL *_prgpmodl; // _cbprt PMODLs, one per body part in this CMTL's set
	long _cbprt; // count of body parts in this CMTL
	long _ibset; // body part set that this CMTL should be applied to

protected:
	bool _FInit(PCRF pcrf, CTG ctg, CNO cno);
	CMTL(void) {}	// can't instantiate directly; must use PcmtlRead

public:
	static PCMTL PcmtlNew(long ibset, long cbprt, PMTRL *prgpmtrl);
	static bool FReadCmtl(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
		PBACO *ppbaco, long *pcb);
	static bool FHasModels(PCFL pcfl, CTG ctg, CNO cno);
	static bool FEqualModels(PCFL pcfl, CNO cno1, CNO cno2);
	~CMTL(void);
	PBMTL Pbmtl(long ibmtl);
	PMODL Pmodl(long imodl);
	long Ibset(void) { return _ibset; }
	long Cbprt(void) { return _cbprt; }
	bool FHasModels(void);
	};

#endif // !MTRL_H
