/***************************************************************************

	bwld.cpp: BRender world class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	To improve performance, BWLD can render into a reduced area, then
	enlarge the resulting image	at display time.  _fHalfX reduces the
	horizontal resolution by half, and _fHalfY reduces the vertical
	resolution by half.  Both modes can be used together to render 1/4 as
	many pixels.  _rcBuffer is the area being rendered into; _rcView is
	the area to copy _rcBuffer into (with stretching, if necessary).
	_pregnDirtyWorking and _pregnDirtyScreen are in _rcBuffer's coordinate
	system.  However, in MarkRenderedRegn, _pregnDirtyScreen is briefly
	enlarged to _rcView's coordinate system, since the gob will be drawn at
	full view resolution.

***************************************************************************/
#include "bren.h"

// REVIEW *****: _pgptStretch is completely unused...remove it!

ASSERTNAME

RTCLASS(BWLD)

const long kcbitPixelRGB = 8;	// RGB buffers are 8 bits deep
const long kcbPixelRGB = 1;

const long kcbitPixelZ = 16;	// Z buffers are 16 bits deep
const long kcbPixelZ = 2;

bool BWLD::_fBRenderInited = fFalse;

/***************************************************************************
	Allocate a new BRender world
***************************************************************************/
PBWLD BWLD::PbwldNew(long dxp, long dyp, bool fHalfX, bool fHalfY)
{
	AssertIn(dxp, 1, ksuMax); // BPMP's width and height are ushorts
	AssertIn(dyp, 1, ksuMax);
	Assert(dxp % 2 == 0, "dxp should be even");
	Assert(dyp % 2 == 0, "dyp should be even");

	PBWLD pbwld;

	pbwld = NewObj BWLD;

	if (pbwld == pvNil || !pbwld->_FInit(dxp, dyp, fHalfX, fHalfY))
		{
		ReleasePpo(&pbwld);
		return pvNil;
		}

    AssertPo(pbwld, 0);
	return pbwld;
}


/***************************************************************************
	Initialize the BWLD
***************************************************************************/
bool BWLD::_FInit(long dxp, long dyp, bool fHalfX, bool fHalfY)
{
	AssertBaseThis(0);
	AssertIn(dxp, 1, ksuMax); // BPMP's width and height are ushorts
	AssertIn(dyp, 1, ksuMax);

	if (!_fBRenderInited)
		{
		BrBegin();
		BrZbBegin(BR_PMT_INDEX_8, BR_PMT_DEPTH_16);
		_fBRenderInited = fTrue;
		}

	_rcView.Set(0, 0, dxp, dyp);

	if (!_FInitBuffers(dxp, dyp, fHalfX, fHalfY))
		return fFalse;

	// Create the world and initial camera
	_bactWorld.type = BR_ACTOR_NONE;
	_bactWorld.t.type = BR_TRANSFORM_MATRIX34;
	BrMatrix34Identity(&_bactWorld.t.t.mat);
	_bactWorld.identifier = (char *)this;
	_bactCamera.type = BR_ACTOR_CAMERA;
	_bactCamera.t.type = BR_TRANSFORM_MATRIX34;
	_bactCamera.type_data = &_bcam;
	_bcam.type = BR_CAMERA_PERSPECTIVE;
	// Note that the aspect ratio of the view is specified rather than the
	// ratio of the buffer so that even when rendering into a reduced
	// buffer, the actors come out right when stretched to _rcView in Draw().
	_bcam.aspect = BR_SCALAR((double)_rcView.Dxp() / (double)_rcView.Dyp());
 	BrActorAdd(&_bactWorld, &_bactCamera);

	// Set up dirty region stuff
	_pregnDirtyWorking = REGN::PregnNew(&_rcBuffer);
	if (pvNil == _pregnDirtyWorking)
		return fFalse;
	_pregnDirtyScreen = REGN::PregnNew(pvNil);
	if (pvNil == _pregnDirtyScreen)
		return fFalse;

	BrZbSetRenderBoundsCallback(_ActorRendered);

	return fTrue;
}


/***************************************************************************
	Initialize or reinitialize members that depend on the values of _fHalfX
	and _fHalfY.  This function gets called by _FInit, and again every time
	FSetHalfMode is called.
***************************************************************************/
bool BWLD::_FInitBuffers(long dxp, long dyp, bool fHalfX, bool fHalfY)
{
	AssertBaseThis(0);
	AssertIn(dxp, 1, ksuMax); // BPMP's width and height are ushorts
	AssertIn(dyp, 1, ksuMax);

	_fHalfX = fHalfX;
	_fHalfY = fHalfY;
	if (_fHalfX)
		dxp /= 2;
	if (_fHalfY)
		dyp /= 2;

	_rcBuffer.Set(0, 0, dxp, dyp);

	ReleasePpo(&_pzbmpBackground);
	_pzbmpBackground = ZBMP::PzbmpNew(dxp, dyp);
	if (pvNil == _pzbmpBackground)
		return fFalse;

	// Set up the working Z-buffer
	ReleasePpo(&_pzbmpWorking);
	_pzbmpWorking = ZBMP::PzbmpNew(dxp, dyp);
	if (pvNil == _pzbmpWorking)
		return fFalse;
	Assert(kcbitPixelZ == 16, "change _bpmpZ.type");
 	_bpmpZ.type = BR_PMT_DEPTH_16;
	_bpmpZ.row_bytes = (short)LwMul(dxp, kcbPixelZ);
	_bpmpZ.width = (ushort)dxp;
	_bpmpZ.height = (ushort)dyp;
	_bpmpZ.origin_x = dxp / 2;
	_bpmpZ.origin_y = dyp / 2;
	_bpmpZ.pixels = _pzbmpWorking->Prgb();

	// Set up background RGB buffer
	ReleasePpo(&_pgptBackground);
	_pgptBackground = GPT::PgptNewOffscreen(&_rcBuffer, kcbitPixelRGB);
	if (pvNil == _pgptBackground)
		return fFalse;

	// Set up the working RGB buffer
	if (pvNil != _pgptWorking)
		{
		_pgptWorking->Unlock();
		ReleasePpo(&_pgptWorking);
		}
	_pgptWorking = GPT::PgptNewOffscreen(&_rcBuffer, kcbitPixelRGB);
	if (pvNil == _pgptWorking)
		return fFalse;
	Assert(kcbitPixelRGB == 8, "change _bpmpRGB.type");
	_bpmpRGB.type = BR_PMT_INDEX_8;
	_bpmpRGB.row_bytes = (short)LwMul(dxp, kcbPixelRGB);
	_bpmpRGB.width = (ushort)dxp;
	_bpmpRGB.height = (ushort)dyp;
	_bpmpRGB.origin_x = dxp / 2;
	_bpmpRGB.origin_y = dyp / 2;
	_bpmpRGB.pixels = _pgptWorking->PrgbLockPixels();

	// If in _fHalfY and not _fHalfX, allocated a _rcView-sized buffer
	// for faster blitting -- see Draw()
	ReleasePpo(&_pgptStretch);
	if (!_fHalfX && _fHalfY)
		{
		_pgptStretch = GPT::PgptNewOffscreen(&_rcView, kcbitPixelRGB);
		if (pvNil == _pgptStretch)
			return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	Destructor for BWLD
***************************************************************************/
BWLD::~BWLD(void)
{
	AssertBaseThis(0);

	if (pvNil != _pgptWorking)
		{
		_pgptWorking->Unlock();
		ReleasePpo(&_pgptWorking);
		}
	ReleasePpo(&_pgptStretch);
	ReleasePpo(&_pgptBackground);
	ReleasePpo(&_pzbmpWorking);
	ReleasePpo(&_pzbmpBackground);
	ReleasePpo(&_pregnDirtyWorking);
	ReleasePpo(&_pregnDirtyScreen);
	ReleasePpo(&_pcrf);
}


/***************************************************************************
	Change reduced rendering mode
***************************************************************************/
bool BWLD::FSetHalfMode(bool fHalfX, bool fHalfY)
{
	AssertThis(0);

	if (FPure(_fHalfX) == FPure(fHalfX) && FPure(_fHalfY) == FPure(fHalfY))
		return fTrue;

	bool fHalfXSave = _fHalfX;
	bool fHalfYSave = _fHalfY;
	RC rcBufferSave = _rcBuffer;
	PGPT pgptWorkingSave = _pgptWorking;
	PGPT pgptStretchSave = _pgptStretch;
	PGPT pgptBackgroundSave = _pgptBackground;
	BPMP bpmpRGBSave = _bpmpRGB;
	PZBMP pzbmpWorkingSave = _pzbmpWorking;
	PZBMP pzbmpBackgroundSave = _pzbmpBackground;
	BPMP bpmpZSave = _bpmpZ;

	_pgptWorking = pvNil;
	_pgptStretch = pvNil;
	_pgptBackground = pvNil;
	_pzbmpWorking = pvNil;
	_pzbmpBackground = pvNil;

	if (!_FInitBuffers(_rcView.Dxp(), _rcView.Dyp(), fHalfX, fHalfY))
		goto LFail;

	if (pvNil != _pcrf)
		{
		// Reload the background at the new resolution
		if (!FSetBackground(_pcrf, _ctgRGB, _cnoRGB, _ctgZ, _cnoZ))
			goto LFail;
		}

	if (pvNil != pgptWorkingSave)
		{
		pgptWorkingSave->Unlock();
		ReleasePpo(&pgptWorkingSave);
		}
	ReleasePpo(&pgptStretchSave);
	ReleasePpo(&pgptBackgroundSave);
	ReleasePpo(&pzbmpWorkingSave);
	ReleasePpo(&pzbmpBackgroundSave);

	AssertThis(0);

	return fTrue;
LFail:
	// Get rid of newly allocated buffers
	if (pvNil != _pgptWorking)
		{
		_pgptWorking->Unlock();
		ReleasePpo(&_pgptWorking);
		}
	ReleasePpo(&_pgptStretch);
	ReleasePpo(&_pgptBackground);
	ReleasePpo(&_pzbmpWorking);
	ReleasePpo(&_pzbmpBackground);

	// restore everything
	_fHalfX = fHalfXSave;
	_fHalfY = fHalfYSave;
	_rcBuffer = rcBufferSave;
	_bpmpZ = bpmpZSave;
	_bpmpRGB = bpmpRGBSave;

	_pgptWorking = pgptWorkingSave;
	_pgptStretch = pgptStretchSave;
	_pgptBackground = pgptBackgroundSave;
	_pzbmpWorking = pzbmpWorkingSave;
	_pzbmpBackground = pzbmpBackgroundSave;

	AssertThis(0);
	return fFalse;
}


/***************************************************************************
	Completely close BRender, freeing all data structures that BRender
	knows about.  This invalidates all MODLs and MTRLs in existence.
***************************************************************************/
void BWLD::CloseBRender(void)
{
	if (_fBRenderInited)
		{
		BrZbEnd();
		BrEnd();
		_fBRenderInited = fFalse;
		}
}


/***************************************************************************
	Copy pvSrc into pvDst, skipping every other short.  This is called by
	FSetBackground for each row in a ZBMP when _fHalfX is fTrue.
***************************************************************************/
inline void SqueezePb(void *pvSrc, void *pvDst, long cbSrc)
{
	AssertIn(cbSrc, 0, kcbMax);
	Assert(cbSrc % (LwMul(2, kcbPixelZ)) == 0, "cbSrc is not aligned");
	AssertPvCb(pvSrc, cbSrc);
	AssertPvCb(pvDst, cbSrc / 2);

	Assert(size(short) == kcbPixelZ, 0);
	short *pswSrc = (short *)pvSrc;
	short *pswDst = (short *)pvDst;

	while (cbSrc != 0)
		{
		*pswDst++ = *pswSrc++;
		pswSrc++;
		cbSrc -= LwMul(2, kcbPixelZ);
		}
}


/***************************************************************************
	Load bitmaps from the given chunks into _pgptBackground and
	_pzbmpBackground.
***************************************************************************/
bool BWLD::FSetBackground(PCRF pcrf, CTG ctgRGB, CNO cnoRGB, CTG ctgZ,
	CNO cnoZ)
{
	AssertThis(0);
	AssertPo(pcrf, 0);

	PMBMP pmbmpNew;
	PZBMP pzbmpNew;

	pmbmpNew = (PMBMP)pcrf->PbacoFetch(ctgRGB, cnoRGB, MBMP::FReadMbmp);
	if (pvNil == pmbmpNew)
		return fFalse;

	pzbmpNew = (PZBMP)pcrf->PbacoFetch(ctgZ, cnoZ, ZBMP::FReadZbmp);
	if (pvNil == pzbmpNew)
		{
		ReleasePpo(&pmbmpNew);
		return fFalse;
		}

	// It's nice to cache these bitmaps if we can, but they should be
	// tossed first when memory gets tight because they take up a lot
	// of space, they're pretty fast to reload, and they are reloaded
	// at a time when it's okay for a pause (between scenes/views).
	pmbmpNew->SetCrep(crepTossFirst);
	pzbmpNew->SetCrep(crepTossFirst);

	if (_fHalfX || _fHalfY)
		{
		// Need to squeeze pmbmpNew and pzbmpNew into _pgptBackground
		// and _pzbmpBackground.  For pmbmpNew, it is drawn into a
		// full-size buffer, then reduced with CopyPixels.  For the
		// ZBMP, it is necessary to reduce it in code here.
		PGPT pgptFull;
		long yp;
		byte *pbSrc;
		byte *pbDst;
		long cbRowSrc;
		long cbRowDst;
		pgptFull = GPT::PgptNewOffscreen(&_rcView, kcbitPixelRGB);
		if (pvNil == _pgptBackground)
			{
			ReleasePpo(&pmbmpNew);
			ReleasePpo(&pzbmpNew);
			return fFalse;
			}
		GNV gnvFull(pgptFull);
		GNV gnvHalf(_pgptBackground);

		gnvFull.DrawMbmp(pmbmpNew, 0, 0);
		ReleasePpo(&pmbmpNew);
		gnvHalf.CopyPixels(&gnvFull, &_rcView, &_rcBuffer);
		ReleasePpo(&pgptFull);

		pbSrc = pzbmpNew->Prgb();
		pbDst = _pzbmpBackground->Prgb();
		cbRowSrc = pzbmpNew->CbRow();
		cbRowDst = _pzbmpBackground->CbRow();
		Assert(cbRowSrc - cbRowDst == (_fHalfX ? cbRowDst : 0),
			"bad src/dest width ratio");
		for (yp = 0; yp < _rcBuffer.Dyp(); yp++)
			{
			if (_fHalfX)
				SqueezePb(pbSrc, pbDst, cbRowSrc);
			else
				CopyPb(pbSrc, pbDst, cbRowSrc);
			pbSrc += cbRowSrc;
			if (_fHalfY)
				pbSrc += cbRowSrc; // skip rows in source
			pbDst += cbRowDst;
			}
		ReleasePpo(&pzbmpNew);
		}
	else // not in half mode
		{
		GNV gnv(_pgptBackground);
		gnv.DrawMbmp(pmbmpNew, 0, 0);
		ReleasePpo(&pmbmpNew);

		ReleasePpo(&_pzbmpBackground);
		_pzbmpBackground = pzbmpNew;
		}

	// entire working buffer is dirty because of background change
	_pregnDirtyWorking->SetRc(&_rcBuffer);
	_fWorldChanged = fTrue;

	// Keep a reference to the background, in case we change to/from
	// halfmode and need to reload it.
	pcrf->AddRef();
	ReleasePpo(&_pcrf);
	_pcrf = pcrf;
	_ctgRGB = ctgRGB;
	_cnoRGB = cnoRGB;
	_ctgZ = ctgZ;
	_cnoZ = cnoZ;

	return fTrue;	
}


/***************************************************************************
	Change the camera matrix
***************************************************************************/
void BWLD::SetCamera(BMAT34 *pbmat34, BRS zrHither, BRS zrYon, BRA aFov)
{
	AssertThis(0);
	AssertVarMem(pbmat34);
	Assert(zrYon > zrHither, "Yon must be further than hither");

	_bactCamera.t.t.mat = *pbmat34;
	_bcam.hither_z = zrHither;
	_bcam.yon_z = zrYon;
	_bcam.field_of_view = aFov;

	// entire working buffer is dirty because of camera change
	_pregnDirtyWorking->SetRc(&_rcBuffer);
}


/***************************************************************************
	Get the camera matrix
***************************************************************************/
void BWLD::GetCamera(BMAT34 *pbmat34, BRS *pzrHither, BRS *pzrYon,
	BRA *paFov)
{
 	AssertThis(0);
	AssertVarMem(pbmat34);
	AssertNilOrVarMem(pzrHither);
	AssertNilOrVarMem(pzrYon);
	AssertNilOrVarMem(paFov);

	*pbmat34 = _bactCamera.t.t.mat;
	if (pvNil != pzrHither)
		*pzrHither = _bcam.hither_z;
	if (pvNil != pzrYon)
		*pzrYon = _bcam.yon_z;
	if (pvNil != paFov)
		*paFov = _bcam.field_of_view;
}


/***************************************************************************
	Render the world.  First, notify all BODYs that we're about to render,
	so they can clear their _pregn's.  Then clean the RGB and Z working
	buffers, since they're probably dirty from the last render.  Update
	some regions, and render everything.
***************************************************************************/
void BWLD::Render(void)
{
	AssertThis(0);

	PBACT pbact;
	RC rc;

	if (!_fWorldChanged)
		return;

	// Note that we only call pfnbeginrend on immediate children of
	// the world, because that will hit all the BODYs in Socrates.
	if (pvNil != _pfnbeginrend)
		{
		for (pbact = _bactWorld.children; pvNil != pbact;
			pbact = pbact->next)
			{
			// BODY roots are BR_ACTOR_NONE
			if (pbact->type == BR_ACTOR_NONE)
				_pfnbeginrend(pbact);
			}
		}

	_CleanWorkingBuffers();

	// Now the working buffer is clean, but we should mark everything that
	// we just cleaned in _CleanWorkingBuffers as dirty in the screen buffer
	// so that the background shows through where actors used to be.
	// In most cases, _pregnDirtyScreen is empty now, so it's tempting to
	// just SwapVars the two regions.  But if Render() was called but Draw()
	// never was (as when you press play then stop playing before the end of
	// the movie), _pregnDirtyScreen may have some stuff still to copy.  So
	// we have to merge (union) _pregnDirtyWorking into _pregnDirtyScreen,
	// then clear _pregnDirtyWorking.
	_pregnDirtyScreen->FUnion(_pregnDirtyWorking);
	_pregnDirtyWorking->SetRc(pvNil);

	// Render the scene.  This will add stuff to _pregnDirtyWorking
    BrZbSceneRender(&_bactWorld, &_bactCamera, &_bpmpRGB, &_bpmpZ);

	for (pbact = _bactWorld.children; pvNil != pbact; pbact = pbact->next)
		{
		// BODY roots are BR_ACTOR_NONE
		if (pbact->type == BR_ACTOR_NONE && pvNil != _pfngetrect)
			{
			_pfngetrect(pbact, &rc);
			_pregnDirtyWorking->FUnionRc(&rc);
			}
		}

	// Everything dirty in working buffer is dirty on screen too
	_pregnDirtyScreen->FUnion(_pregnDirtyWorking);

	_fWorldChanged = fFalse;
}


/***************************************************************************
	"Prerender" the world.  That is, render the world, then copy it into
	the background buffers
***************************************************************************/
void BWLD::Prerender(void)
{
	AssertThis(0);

	GNV gnvBackground(_pgptBackground);
	GNV gnvWorking(_pgptWorking);

	Render();

	_pzbmpWorking->Draw((byte *)_pzbmpBackground->Prgb(), 
		_pzbmpBackground->CbRow(), _rcBuffer.Dyp(), 0, 0, &_rcBuffer, pvNil);
	
	// Need to detach _pzbmpBackground from the CRF so when we unprerender,
	// a fresh copy of the ZBMP is fetched
	_pzbmpBackground->Detach();

	gnvBackground.CopyPixels(&gnvWorking, &_rcBuffer, &_rcBuffer);

	// Need to ensure that the current contents of _pgptWorking (just the 
	// prerenderable actors) go into _pgptBackground
	GPT::Flush();
}


/***************************************************************************
	"Un-Prerender" the world.  That is, restore the background bitmaps to 
	the way they were before prerendering any actors
***************************************************************************/
void BWLD::Unprerender(void)
{
	AssertThis(0);

	// Ignore error...you'll just get weird visual effects and an error 
	// will be reported elsewhere
	FSetBackground(_pcrf, _ctgRGB, _cnoRGB, _ctgZ, _cnoZ);
}


/***************************************************************************
	Copy _pregnDirtyWorking from background Z and RGB buffers to working
	Z and RGB buffers.
***************************************************************************/
void BWLD::_CleanWorkingBuffers(void)
{
	AssertThis(0);

	REGSC regsc;
	long yp;
	long xpLeft;
	RC rcRegnBounds;
	RC rcClippedRegnBounds;
	byte *pbSrc;
	byte *pbDst;
	long cbRowCopy;
	RC rc;
	long cbRowSrc, cbRowDst;

	if (_pregnDirtyWorking->FEmpty(&rcRegnBounds))
		return;
	if (!rcClippedRegnBounds.FIntersect(&rcRegnBounds, &_rcBuffer))
		return;

	// Clean the Z buffer
	_pzbmpBackground->Draw((byte*)_bpmpZ.pixels, _bpmpZ.row_bytes,
		_bpmpZ.height, 0, 0, &rcClippedRegnBounds, _pregnDirtyWorking);

	// Clean the RGB buffer
	regsc.Init(_pregnDirtyWorking, &rcClippedRegnBounds);
	yp = rcClippedRegnBounds.ypTop;
	cbRowSrc = _pgptBackground->CbRow();
	pbSrc = _pgptBackground->PrgbLockPixels() + LwMul(yp, cbRowSrc) +
		rcClippedRegnBounds.xpLeft;
	cbRowDst = _pgptWorking->CbRow();
	pbDst = _pgptWorking->PrgbLockPixels() + LwMul(yp, cbRowDst) +
		rcClippedRegnBounds.xpLeft;
	for ( ; yp < rcClippedRegnBounds.ypBottom; yp++)
		{
		while (regsc.XpCur() < klwMax)
			{
			xpLeft = regsc.XpCur();
			cbRowCopy = regsc.XpFetch() - xpLeft;
			regsc.XpFetch();
			CopyPb(pbSrc + xpLeft, pbDst + xpLeft, cbRowCopy);
			}
		pbSrc += cbRowSrc;
		pbDst += cbRowDst;
		regsc.ScanNext(1);
		}
	_pgptWorking->Unlock();
	_pgptBackground->Unlock();
}


/***************************************************************************
	Callback for when a BACT is rendered.  Need to union with dirty
	region.
***************************************************************************/
void BWLD::_ActorRendered(PBACT pbact, PBMDL pbmdl, PBMTL pbmtl,
	br_uint_8 bStyle, br_matrix4 *pbmat4ModelToScreen, br_int_32 bounds[4])
{
	AssertVarMem(pbact);

	PBACT pbactT = pbact;
	PBWLD pbwld;
	RC rc(bounds[0], bounds[1], bounds[2] + 1, bounds[3] + 1);

	while (pbactT->parent != pvNil)
		pbactT = pbactT->parent;
	pbwld = (PBWLD)pbactT->identifier;
	AssertPo(pbwld, 0);
	if (pvNil != pbwld->_pfnbactrend)
		pbwld->_pfnbactrend(pbact, &rc); // call client callback
}


/***************************************************************************
	Mark the region that has been rendered (and needs to be copied to the
	screen)
***************************************************************************/
void BWLD::MarkRenderedRegn(PGOB pgob, long dxp, long dyp)
{
	AssertThis(0);
	AssertPo(pgob, 0);

	_pregnDirtyScreen->Scale((_fHalfX ? 2 : 1), 1, (_fHalfY ? 2 : 1), 1);
	_pregnDirtyScreen->Offset(dxp, dyp);
	vpappb->MarkRegn(_pregnDirtyScreen, pgob);
	_pregnDirtyScreen->SetRc(pvNil); // screen is clean
}


/***************************************************************************
	Draw the BWLD's working RGB buffer into pgnv.  The movie engine should
	have called BWLD::MarkRenderedRegn before calling this, so only
	_pregnDirtyScreen's bits will be copied.
***************************************************************************/
void BWLD::Draw(PGNV pgnv, RC *prcClip, long dxp, long dyp)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);

	RC rc;
	GNV gnvTemp(_pgptWorking);

	rc.OffsetCopy(&_rcView, -dxp, -dyp);
	pgnv->CopyPixels(&gnvTemp, &_rcBuffer, &rc);
}


/***************************************************************************
	Add an actor to the world
***************************************************************************/
void BWLD::AddActor(BACT *pbact)
{
	AssertThis(0);
	AssertVarMem(pbact);

	BrActorAdd(&_bactWorld, pbact);
}


/***************************************************************************
	Filter callback proc for FClickedActor().  Saves pbact if it's the
	closest one hit so far.
***************************************************************************/
int BWLD::_FFilter(BACT *pbact, PBMDL pbmdl, PBMTL pbmtl, 
	BVEC3 *pbvec3RayPos, BVEC3 *pbvec3RayDir, BRS dzpNear, BRS dzpFar, 
	void *pvData)
{
	AssertVarMem(pbact);
	AssertVarMem(pbvec3RayPos);
	AssertVarMem(pbvec3RayDir);

	PBWLD pbwld = (PBWLD)pvData;
	AssertPo(pbwld, 0);

	if (dzpNear < pbwld->_dzpClosestClicked)
		{
		pbwld->_pbactClosestClicked = pbact;
		pbwld->_dzpClosestClicked = dzpNear;
		}

	return fFalse; // fFalse means keep searching
}


/***************************************************************************
	Call pfnCallback for each actor under the point (xp, yp)
***************************************************************************/
void BWLD::IterateActorsInPt(br_pick2d_cbfn *pfnCallback, void *pvArg, 
	long xp, long yp)
{
	AssertThis(0);

	// Convert to _rcBuffer coordinates:
	if (_fHalfX)
		xp /= 2;
	if (_fHalfY)
		yp /= 2;
	xp -= _bpmpRGB.origin_x;
	yp -= _bpmpRGB.origin_y;

	BrScenePick2D(&_bactWorld, &_bactCamera, &_bpmpRGB, xp, yp, pfnCallback,
		pvArg);
}


/***************************************************************************
   If an actor is under (xp, yp), function returns fTrue and **pbact is the
   actor. If no actor is under (xp, yp), function returns fFalse.
***************************************************************************/
bool BWLD::FClickedActor(long xp, long yp, BACT **ppbact)
{
	AssertThis(0);
	AssertVarMem(ppbact);

	_pbactClosestClicked = pvNil;
	_dzpClosestClicked = BR_SCALAR_MAX;

	IterateActorsInPt(BWLD::_FFilter, this, xp, yp);

	if (pvNil != _pbactClosestClicked)
		{
		*ppbact = _pbactClosestClicked;
		return fTrue;
		}

	// nothing was clicked
	TrashVar(ppbact);
	return fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the BWLD.
***************************************************************************/
void BWLD::AssertValid(ulong grf)
{
	BWLD_PAR::AssertValid(fobjAllocated);
	AssertPo(_pgptWorking, 0);
	AssertPo(_pgptBackground, 0);
	AssertPo(_pzbmpWorking, 0);
	AssertPo(_pzbmpBackground, 0);
	AssertPo(_pregnDirtyWorking, 0);
	AssertPo(_pregnDirtyScreen, 0);
	AssertNilOrPo(_pcrf, 0);
	if (!_fHalfX && _fHalfY)
		AssertPo(_pgptStretch, 0);
	else
		Assert(pvNil == _pgptStretch, "don't need _pgptStretch!");
}


/***************************************************************************
	Mark memory used by the BWLD
***************************************************************************/
void BWLD::MarkMem(void)
{
	AssertThis(0);
	BWLD_PAR::MarkMem();
	MarkMemObj(_pgptWorking);
	MarkMemObj(_pgptBackground);
	MarkMemObj(_pzbmpWorking);
	MarkMemObj(_pzbmpBackground);
	MarkMemObj(_pregnDirtyWorking);
	MarkMemObj(_pregnDirtyScreen);
	MarkMemObj(_pcrf);
	MarkMemObj(_pgptStretch);
}


/******************************************************************************
	FWriteBmp
		Writes the current rendered buffer out to the given file
	
	Arguments:
		PFNI pfni -- the name of the file
	
	Returns: fTrue if the file could be written successfully
	
************************************************************ PETED ***********/
bool BWLD::FWriteBmp(PFNI pfni)
{
	AssertPo(pfni, 0);

	bool fRet;
	RC rc;

	fRet = FWriteBitmap(pfni, _pgptWorking->PrgbLockPixels(&rc),
		GPT::PglclrGetPalette(), _rcBuffer.Dxp(), _rcBuffer.Dyp());
	_pgptWorking->Unlock();
	return fRet;
}

#endif //DEBUG
