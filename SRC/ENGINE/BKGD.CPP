/***************************************************************************

	bkgd.cpp: Background class

	Primary Author: manders
	Review Status: REVIEWED - any changes to this file must be reviewed!

	A BKGD (background) consists of a set of light sources (GLLT), a
	background sound (SND), and one or more camera views.  Each camera view
	consists of a camera specification, a pre-rendered RGB bitmap, and a
	pre-rendered Z-buffer.  Here's how the chunks look:

	BKGD // Contains stage bounding cuboid
	 |
	 +---GLLT (chid 0) // GL of light position specs (LITEs)
	 |
	 +---SND  (chid 0) // Background sound/music
	 |
	 +---CAM  (chid 0) // Contains camera pos/orient matrix, hither, yon
	 |    |
	 |    +---MBMP (chid 0) // Background RGB bitmap
	 |    |
	 |    +---ZBMP (chid 0) // Background Z-buffer
	 |
	 +---CAM (chid 1)
	 |    |
	 |    +---MBMP (chid 0)
	 |    |
	 |    +---ZBMP (chid 0)
	 |
	 +---CAM (chid 2)
	 .    |
	 .    +---MBMP (chid 0)
	 .    |
	      +---ZBMP (chid 0)

***************************************************************************/
#include "soc.h"
ASSERTNAME

RTCLASS(BKGD)

const CHID kchidBds = 0;  // Background default sound
const CHID kchidGllt = 0; // GL of LITEs
const CHID kchidGlcr = 0; // Palette
const br_colour kbrcLight = BR_COLOUR_RGB(0xff, 0xff, 0xff);

/***************************************************************************
	Add the background's chunks (excluding camera views) to the tag list
***************************************************************************/
bool BKGD::FAddTagsToTagl(PTAG ptagBkgd, PTAGL ptagl)
{
	AssertVarMem(ptagBkgd);
	AssertPo(ptagl, 0);

	if (!ptagl->FInsertTag(ptagBkgd, fFalse))
		return fFalse;
	if (!ptagl->FInsertChild(ptagBkgd, kchidBds, kctgBds))
		return fFalse;
	if (!ptagl->FInsertChild(ptagBkgd, kchidGllt, kctgGllt))
		return fFalse;
	if (!ptagl->FInsertChild(ptagBkgd, kchidGlcr, kctgColorTable))
		return fFalse;

	// Have to cache first camera view since scene switches to it
	// automatically
	if (!ptagl->FInsertChild(ptagBkgd, 0, kctgCam))
		return fFalse;
	return fTrue;
}


/***************************************************************************
	Cache the background's chunks (excluding camera views) to HD
***************************************************************************/
bool BKGD::FCacheToHD(PTAG ptagBkgd)
{
	AssertVarMem(ptagBkgd);

	TAG tagBds;
	TAG tagGllt;
	TAG tagGlcr;
	TAG tagCam;

	// Build the child tags
	if (!vptagm->FBuildChildTag(ptagBkgd, kchidBds, kctgBds, &tagBds))
		return fFalse;
	if (!vptagm->FBuildChildTag(ptagBkgd, kchidGllt, kctgGllt, &tagGllt))
		return fFalse;
	if (!vptagm->FBuildChildTag(ptagBkgd, kchidGlcr, kctgColorTable, &tagGlcr))
		return fFalse;
	if (!vptagm->FBuildChildTag(ptagBkgd, 0, kctgCam, &tagCam))
		return fFalse;

	// Cache the BKGD chunk
	if (!vptagm->FCacheTagToHD(ptagBkgd, fFalse))
		return fFalse;

	// Cache the child chunks
	if (!vptagm->FCacheTagToHD(&tagBds))
		return fFalse;
	if (!vptagm->FCacheTagToHD(&tagGllt))
		return fFalse;
	if (!vptagm->FCacheTagToHD(&tagGlcr))
		return fFalse;

	// Have to cache first camera view since scene switches to it
	// automatically
	if (!vptagm->FCacheTagToHD(&tagCam))
		return fFalse;

	return fTrue;
}


/***************************************************************************
	A PFNRPO to read a BKGD from a file
***************************************************************************/
bool BKGD::FReadBkgd(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, 0);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);

	BKGD *pbkgd;

	*pcb = size(BKGD) + size(BACT) + size(BLIT); // estimate BKGD size
	if (pvNil == ppbaco)
		return fTrue;
	pbkgd = NewObj BKGD;
	if (pvNil == pbkgd || !pbkgd->_FInit(pcrf->Pcfl(), ctg, cno))
		{
		TrashVar(ppbaco);
		TrashVar(pcb);
		ReleasePpo(&pbkgd);
		return fFalse;
		}
	AssertPo(pbkgd, 0);
	*ppbaco = pbkgd;
	*pcb = size(BKGD) + LwMul(pbkgd->_cbactLight, size(BACT)) +
		LwMul(pbkgd->_cbactLight, size(BLIT)); // actual BKGD size
	return fTrue;
}


/***************************************************************************
	Read a BKGD from the given chunk of the given CFL.
	Note: Although we read the data for the lights here, we don't turn
	them on yet because we don't have a BWLD to add them to.  The lights
	are	turned on with the first FSetCamera() call.
***************************************************************************/
bool BKGD::_FInit(PCFL pcfl, CTG ctg, CNO cno)
{
	AssertBaseThis(0);
	AssertPo(pcfl, 0);

	BLCK blck;
	BKGDF bkgdf;
	KID kid;
	PGL pgllite = pvNil;
	short bo;

	_ccam = _Ccam(pcfl, ctg, cno);	// compute # of views in this background
	_icam = ivNil;

	if (!pcfl->FFind(ctg, cno, &blck) || !blck.FUnpackData())
		goto LFail;
	if (blck.Cb() != size(BKGDF))
		goto LFail;
	if (!blck.FReadRgb(&bkgdf, size(BKGDF), 0))
		goto LFail;
	if (kboCur != bkgdf.bo)
		SwapBytesBom(&bkgdf, kbomBkgdf);
	Assert(kboCur == bkgdf.bo, "bad BKGDF");

	if (!pcfl->FGetName(ctg, cno, &_stn))
		goto LFail;

	// Get the default sound
	if (pcfl->FGetKidChidCtg(ctg, cno, kchidBds, kctgBds, &kid))
		{
		if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck) || !blck.FUnpackData())
			goto LFail;
		if (blck.Cb() != size(BDS))
			goto LFail;
		if (!blck.FReadRgb(&_bds, size(BDS), 0))
			goto LFail;
		if (kboCur != _bds.bo)
			SwapBytesBom(&_bds, kbomBds);
		Assert(kboCur == _bds.bo, "bad BDS");
		}
	else
		{
		_bds.tagSnd.sid = ksidInvalid;
		}


	// If there is a GLCR child, get it
	if (pcfl->FGetKidChidCtg(ctg, cno, kchidGlcr, kctgColorTable, &kid))
		{
		if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
			goto LFail;
		_pglclr = GL::PglRead(&blck, &bo);
		if (_pglclr != pvNil)
			{
			if (kboOther == bo)
				SwapBytesRglw(_pglclr->QvGet(0), _pglclr->IvMac());
			}
		_bIndexBase = bkgdf.bIndexBase;
		}
	else
		_pglclr = pvNil;

	// Read the GL of LITEs (GLLT)
	if (!pcfl->FGetKidChidCtg(ctg, cno, kchidGllt, kctgGllt, &kid))
		goto LFail;
	if (!pcfl->FFind(kid.cki.ctg, kid.cki.cno, &blck))
		goto LFail;
	pgllite = GL::PglRead(&blck, &bo);
	if (pvNil == pgllite)
		goto LFail;
	Assert(pgllite->CbEntry() == size(LITE),
		"bad pgllite...you may need to update bkgds.chk");
	AssertBomRglw(kbomLite, size(LITE));
	if (kboOther == bo)
		{
		SwapBytesRglw(pgllite->QvGet(0), LwMul(pgllite->IvMac(),
			size(LITE) / size(long)));
		}
	_cbactLight = pgllite->IvMac();
	if (!FAllocPv((void **)&_prgbactLight, LwMul(_cbactLight, size(BACT)),
		fmemClear, mprNormal))
		{
		goto LFail;
		}
	if (!FAllocPv((void **)&_prgblitLight, LwMul(_cbactLight, size(BLIT)),
		fmemClear, mprNormal))
		{
		goto LFail;
		}
	_SetupLights(pgllite);
	ReleasePpo(&pgllite);
	return fTrue;
LFail:
	Warn("Error reading background");
	ReleasePpo(&pgllite);
	return fFalse;
}


/***************************************************************************
	Return the number of camera views in this scene.  CAM chunks need to be
	contiguous CHIDs starting at CHID 0.
***************************************************************************/
long BKGD::_Ccam(PCFL pcfl, CTG ctg, CNO cno)
{
	AssertBaseThis(0);
	AssertPo(pcfl, 0);

	KID kid;
	long ccam;

	for (ccam = 0; pcfl->FGetKidChidCtg(ctg, cno, ccam, kctgCam, &kid);
		ccam++)
		{
		}
#ifdef DEBUG
	// Make sure chids are consecutive
	long ckid;
	long ccamT = 0;
	for (ckid = 0; pcfl->FGetKid(ctg, cno, ckid, &kid); ckid++)
		{
		if (kid.cki.ctg == kctgCam)
			ccamT++;
		}
	Assert(ccamT == ccam, "cam chids are not consecutive!");
#endif
	return ccam;
}


/***************************************************************************
	Fill _prgbactLight and _prgblitLight using a GL of LITEs
***************************************************************************/
void BKGD::_SetupLights(PGL pgllite)
{
	AssertBaseThis(0);
	AssertPo(pgllite, 0);

	long ilite;
	LITE *qlite;
	BACT *pbact;
	BLIT *pblit;

	for (ilite = 0; ilite < _cbactLight; ilite++)
		{
		qlite = (LITE *)pgllite->QvGet(ilite);
		pbact = &_prgbactLight[ilite];
		pblit = &_prgblitLight[ilite];
		pblit->type = (byte)qlite->lt;
		pblit->colour = kbrcLight;
		pblit->attenuation_c = qlite->rIntensity;
		pbact->type = BR_ACTOR_LIGHT;
		pbact->type_data = pblit;
		pbact->t.type = BR_TRANSFORM_MATRIX34;
		pbact->t.t.mat = qlite->bmat34;
		}
}


/***************************************************************************
	Clean up and delete this background
***************************************************************************/
BKGD::~BKGD(void)
{
	AssertBaseThis(0);
	Assert(!_fLeaveLitesOn, "Shouldn't be freeing background now");
	if (pvNil != _prgbactLight && pvNil != _prgblitLight)
		TurnOffLights();
	FreePpv((void **)&_prgbactLight);
	FreePpv((void **)&_prgblitLight);
	ReleasePpo(&_pglclr);
	ReleasePpo(&_pglapos);
}


/***************************************************************************
	Get the background's name
***************************************************************************/
void BKGD::GetName(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	*pstn = _stn;
}


/***************************************************************************
	Get the custom palette for this background, if any.  Returns fFalse if
	an error occurs.  Sets *ppglclr to an empty GL and *piclrMin to 0 if
	this background has no custom palette.
***************************************************************************/
bool BKGD::FGetPalette(PGL *ppglclr, long *piclrMin)
{
	AssertThis(0);
	AssertVarMem(ppglclr);
	AssertVarMem(piclrMin);

	*piclrMin = _bIndexBase;
	if (pvNil == _pglclr) // no custom palette
		{
		*ppglclr = GL::PglNew(size(CLR)); // "palette" with 0 entries
		}
	else
		{
		*ppglclr = _pglclr->PglDup();
		}
	return (pvNil != *ppglclr);
}


/***************************************************************************
	Get the camera position in worldspace
***************************************************************************/
void BKGD::GetCameraPos(BRS *pxr, BRS *pyr, BRS *pzr)
{
	AssertThis(0);
	AssertNilOrVarMem(pxr);
	AssertNilOrVarMem(pyr);
	AssertNilOrVarMem(pzr);

	if (pvNil != pxr)
		*pxr = _xrCam;
	if (pvNil != pyr)
		*pyr = _yrCam;
	if (pvNil != pzr)
		*pzr = _zrCam;
}


/***************************************************************************
	Turn on lights in pbwld
***************************************************************************/
void BKGD::TurnOnLights(PBWLD pbwld)
{
	AssertThis(0);
	AssertPo(pbwld, 0);

	long ilite;
	BACT *pbact;

	if (!_fLites)
		{
		for (ilite = 0; ilite < _cbactLight; ilite++)
			{
			pbact = &_prgbactLight[ilite];
			pbwld->AddActor(pbact);
			BrLightEnable(pbact);
			}
	
		_fLites = fTrue;
		}
}


/***************************************************************************
	Turn off lights in pbwld
***************************************************************************/
void BKGD::TurnOffLights(void)
{
	AssertThis(0);

	long ilite;
	BACT *pbact;

	if (!_fLites || _fLeaveLitesOn)
		return;
	for (ilite = 0; ilite < _cbactLight; ilite++)
		{
		pbact = &_prgbactLight[ilite];
		BrLightDisable(pbact);
		BrActorRemove(pbact);
		}
	_fLites = fFalse;
}


/***************************************************************************
	Set the camera and associated bitmaps to icam
***************************************************************************/
bool BKGD::FSetCamera(PBWLD pbwld, long icam)
{
	AssertThis(0);
	AssertPo(pbwld, 0);
	AssertIn(icam, 0, Ccam());

	long capos;
 	KID kidCam;
	KID kidRGB;
	KID kidZ;
	BLCK blck;
	CAM cam;
	PCFL pcfl = Pcrf()->Pcfl();
	BREUL breul;

	TurnOnLights(pbwld);

	// read new camera data
	if (!pcfl->FGetKidChidCtg(Ctg(), Cno(), icam, kctgCam, &kidCam))
		return fFalse;
	if (!pcfl->FFind(kidCam.cki.ctg, kidCam.cki.cno, &blck) || !blck.FUnpackData())
		return fFalse;

	// Need at least one actor position
	if (blck.Cb() < size(CAM))
		{
		Bug("CAM chunk not large enough");
		return fFalse;
		}
	capos = (blck.Cb() - size(CAM)) / size(APOS);
	if ((capos * size(APOS) + size(CAM)) != blck.Cb())
		{
		Bug("CAM chunk's extra data not an even multiple of size(APOS)");
		return fFalse;
		}

	if (!blck.FReadRgb(&cam, size(CAM), 0))
		return fFalse;

#ifdef DEBUG
	{
	BOM bomCam = kbomCam, bomCamOld = kbomCamOld;
	Assert(bomCam == bomCamOld, "BOM macros aren't right");
	}
#endif // DEBUG

	Assert((size(APOS) / size(long)) * size(long) == size(APOS),
		"APOS not an even number of longs");
	if (kboOther == cam.bo)
		{
		SwapBytesBom(&cam, kbomCam);
		SwapBytesRglw(PvAddBv(&cam, offset(CAM, bmat34Cam)),
			size(cam.bmat34Cam) / size(long));
		SwapBytesRglw(PvAddBv(&cam, size(CAM)),
			capos * (size(APOS) / size(long)));
		}
	Assert(kboCur == cam.bo, "bad cam");

	// find RGB pict
	if (!pcfl->FGetKidChidCtg(kidCam.cki.ctg, kidCam.cki.cno, 0, kctgMbmp,
		&kidRGB))
		{
		return fFalse;
		}
	// find Z pict
	if (!pcfl->FGetKidChidCtg(kidCam.cki.ctg, kidCam.cki.cno, 0, kctgZbmp,
		&kidZ))
		{
		return fFalse;
		}
	if (!pbwld->FSetBackground(Pcrf(), kidRGB.cki.ctg, kidRGB.cki.cno,
		kidZ.cki.ctg, kidZ.cki.cno))
		{
		return fFalse;
		}

	// Get actor placements
	ReleasePpo(&_pglapos);
	_iaposNext = _iaposLast = 0;
	if (capos > 0 && (_pglapos = GL::PglNew(size(APOS), capos)) != pvNil)
		{
		AssertDo(_pglapos->FSetIvMac(capos), "Should never fail");
		_pglapos->Lock();
		if (!blck.FReadRgb(_pglapos->QvGet(0), size(APOS) * capos, size(CAM)))
			{
			ReleasePpo(&_pglapos);
			return fFalse;
			}
		_pglapos->Unlock();
		}

	_icam = icam;
	_xrPlace = cam.apos.xrPlace;
	_yrPlace = cam.apos.yrPlace;
	_zrPlace = cam.apos.zrPlace;

	_xrCam = cam.bmat34Cam.m[3][0];
	_yrCam = cam.bmat34Cam.m[3][1];
	_zrCam = cam.bmat34Cam.m[3][2];

	// Find bmat34 without X & Z rotation
	breul.order = BR_EULER_YXY_R;
	BrMatrix34ToEuler(&breul, &cam.bmat34Cam);
	_braRotY = breul.a + breul.c;
	BrMatrix34RotateY(&_bmat34Mouse, _braRotY);
	BrMatrix34PostTranslate(&_bmat34Mouse, cam.bmat34Cam.m[3][0],
		cam.bmat34Cam.m[3][1], cam.bmat34Cam.m[3][2]);

	pbwld->SetCamera(&cam.bmat34Cam, cam.zrHither, cam.zrYon, cam.aFov);
	pbwld->MarkDirty();
	return fTrue;
}


/***************************************************************************
	Gets the matrix for mouse-dragging relative to the camera
***************************************************************************/
void BKGD::GetMouseMatrix(BMAT34 *pbmat34)
{
	AssertThis(0);
	AssertVarMem(pbmat34);

	*pbmat34 = _bmat34Mouse;
}


/***************************************************************************
	Gets the point at which to place new actors for this bkgd/view.
***************************************************************************/
void BKGD::GetActorPlacePoint(BRS *pxr, BRS *pyr, BRS *pzr)
{
	AssertThis(0);
	AssertVarMem(pxr);
	AssertVarMem(pyr);
	AssertVarMem(pzr);

	APOS apos;

	if (_iaposNext == 0)
		{
		*pxr = _xrPlace;
		*pyr = _yrPlace;
		*pzr = _zrPlace;
		}
	else
		{
		_pglapos->Get(_iaposNext - 1, &apos);
		*pxr = apos.xrPlace;
		*pyr = apos.yrPlace;
		*pzr = apos.zrPlace;
		}

	_iaposLast = _iaposNext;
	if (_pglapos != pvNil)
		{
		_iaposNext++;
		if (_iaposNext > _pglapos->IvMac())
			_iaposNext = 0;
		}
}


/******************************************************************************
	ReuseActorPlacePoint
		Resets the current actor place point to the last one used.  Call this
		from the actor placement code if the actor was placed at a point other
		than the one you just asked for.
************************************************************ PETED ***********/
void BKGD::ReuseActorPlacePoint(void)
{
	_iaposNext = _iaposLast;
}


#ifdef DEBUG
/***************************************************************************
	Authoring only.  Writes a special file with the given place info.
***************************************************************************/
bool BKGD::FWritePlaceFile(BRS xrPlace, BRS yrPlace, BRS zrPlace)
{
	AssertThis(0);
	Assert(yrPlace == rZero, "are you sure you want non-zero Y?");

	STN stnFile;
	FNI fni;
	PFIL pfil = pvNil;
	STN stnData;
	FP fp;
	long xr1 = BrScalarToInt(xrPlace);
	long xr2 = LwAbs((long)(1000000.0 * BrScalarToFloat(xrPlace -
		BrIntToScalar(xr1))));
	long yr1 = BrScalarToInt(yrPlace);
	long yr2 = LwAbs((long)(1000000.0 * BrScalarToFloat(yrPlace -
		BrIntToScalar(yr1))));
	long zr1 = BrScalarToInt(zrPlace);
	long zr2 = LwAbs((long)(1000000.0 * BrScalarToFloat(zrPlace -
		BrIntToScalar(zr1))));

	if (!stnFile.FFormatSz("%s-cam.1-%d.pos", &_stn, _icam + 1))
		goto LFail;
	if (!fni.FBuildFromPath(&stnFile))
		goto LFail;
	if (fni.TExists() == tYes)
		pfil = FIL::PfilOpen(&fni, ffilWriteEnable);
	else
		pfil = FIL::PfilCreate(&fni);
	if (pvNil == pfil)
		goto LFail;
	if (!stnData.FFormatSz("NEW_ACTOR_POS %d.%06d %d.%06d %d.%06d\n\r",
		xr1, xr2, yr1, yr2, zr1, zr2))
		{
		goto LFail;
		}
	if ((fp = pfil->FpMac()) > 0)
		{
		// Go to the end of the file (and write over null byte at end
		// of previous string)
		fp--;
		}
	if (!pfil->FWriteRgb(stnData.Psz(), stnData.Cch() + 1, fp))
		goto LFail;
	ReleasePpo(&pfil);
	return fTrue;
LFail:	
	ReleasePpo(&pfil);
	return fFalse;	
}
#endif // DEBUG


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the BKGD.
***************************************************************************/
void BKGD::AssertValid(ulong grf)
{
	BKGD_PAR::AssertValid(fobjAllocated);
	AssertIn(_cbactLight, 1, 100); // 100 is sanity check
	AssertIn(_ccam, 1, 100); // 100 is sanity check
	Assert(_icam == ivNil || (_icam >= 0 && _icam < _ccam), "bad _icam");
	AssertPvCb(_prgbactLight, LwMul(_cbactLight, size(BACT)));	
	AssertPvCb(_prgblitLight, LwMul(_cbactLight, size(BLIT)));	
	AssertPo(&_stn, 0);
	AssertNilOrPo(_pglapos, 0);
}


/***************************************************************************
	Mark memory used by the BKGD
***************************************************************************/
void BKGD::MarkMem(void)
{
	AssertThis(0);
	BKGD_PAR::MarkMem();
	MarkPv(_prgbactLight);
	MarkPv(_prgblitLight);
	MarkMemObj(_pglclr);
	MarkMemObj(_pglapos);
}
#endif //DEBUG
