/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	modl.cpp: Model class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

***************************************************************************/
#include "soc.h"
ASSERTNAME

RTCLASS(MODL)


/***************************************************************************
	Create a new PMODL based on some vertices and faces.
***************************************************************************/
PMODL MODL::PmodlNew(long cbrv, BRV *prgbrv, long cbrf, BRF *prgbrf)
{
	AssertIn(cbrv, 0, ksuMax); // ushort in br_model
	AssertPvCb(prgbrv, LwMul(cbrv, size(BRV)));
	AssertIn(cbrf, 0, ksuMax); // ushort in br_model
	AssertPvCb(prgbrf, LwMul(cbrf, size(BRF)));

	PMODL pmodl;
	char szIdentifier[size(PMODL) + 1];

	pmodl = NewObj MODL;
	if (pvNil == pmodl)
		goto LFail;
	ClearPb(szIdentifier, size(PMODL) + 1);
	pmodl->_pbmdl = BrModelAllocate(szIdentifier, cbrv, cbrf);
	if (pvNil == pmodl->_pbmdl)
		goto LFail;
	CopyPb(&pmodl, pmodl->_pbmdl->identifier, size(PMODL));
	CopyPb(prgbrv, pmodl->_pbmdl->vertices, LwMul(cbrv, size(BRV)));
	CopyPb(prgbrf, pmodl->_pbmdl->faces, LwMul(cbrf, size(BRF)));
	BrModelAdd(pmodl->_pbmdl);
	AssertPo(pmodl, 0);
	return pmodl;
LFail:
	ReleasePpo(&pmodl);
	return pvNil;
}

/***************************************************************************
	A PFNRPO to read a MODL from a file
***************************************************************************/
bool MODL::FReadModl(PCRF pcrf, CTG ctg, CNO cno, PBLCK pblck,
	PBACO *ppbaco, long *pcb)
{
	AssertPo(pcrf, 0);
	AssertPo(pblck, 0);
	AssertNilOrVarMem(ppbaco);
	AssertVarMem(pcb);

	MODL *pmodl;

	*pcb = pblck->Cb(fTrue);
	if (pvNil == ppbaco)
		return fTrue;

	if (!pblck->FUnpackData())
		goto LFail;
	*pcb = pblck->Cb();

	pmodl = NewObj MODL;
	if (pvNil == pmodl || !pmodl->_FInit(pblck))
		{
		ReleasePpo(&pmodl);
LFail:
		TrashVar(ppbaco);
		TrashVar(pcb);
		return pvNil;
		}
	AssertPo(pmodl, 0);

	*ppbaco = pmodl;
	return fTrue;
}


/***************************************************************************
	Reads a MODL from a BLCK
***************************************************************************/
bool MODL::_FInit(PBLCK pblck)
{
	AssertBaseThis(0);
	AssertPo(pblck, 0);

	MODLF modlf;
	long cbrgbrv;
	long cbrgbrf;
	long ibrv;
	BRV *pbrv;
	long ibrf;
	BRF *pbrf;
	MODL *pmodlThis = this;
	char szIdentifier[size(PMODL) + 1];

	ClearPb(szIdentifier, size(PMODL) + 1);
	if (!pblck->FUnpackData())
		return fFalse;
	if (pblck->Cb() < size(MODLF))
		return fFalse;
	if (!pblck->FReadRgb(&modlf, size(MODLF), 0))
		return fFalse;
	if (kboOther == modlf.bo)
		SwapBytesBom(&modlf, kbomModlf);
	Assert(kboCur == modlf.bo, "bad MODL!");

	// Allocate space for the BMDL, array of vertices, and array of faces
	cbrgbrv = LwMul(modlf.cver, size(BRV));
	cbrgbrf = LwMul(modlf.cfac, size(BRF));
	
	if (modlf.rRadius == rZero)
		{
		// unprepared model.  Gotta prepare it.  

		_pbmdl = BrModelAllocate(szIdentifier, modlf.cver, modlf.cfac);
		if (pvNil == _pbmdl)
			return fFalse;
		CopyPb(&pmodlThis, _pbmdl->identifier, size(PMODL));
		if (!pblck->FReadRgb(_pbmdl->vertices, cbrgbrv, size(MODLF)))
			return fFalse;
		if (!pblck->FReadRgb(_pbmdl->faces, cbrgbrf, size(MODLF) + cbrgbrv))
			return fFalse;
                           
		BrModelAdd(_pbmdl);

// REVIEW *****: uncomment and expand the following code to prelight models
//		BVEC3 bvec3;
//		if (!_FPrelight(1, &bvec3))
//			return fFalse;

		}
	else
		{
		// pre-prepared model.
		_pbmdl = BrModelAllocate(szIdentifier, 0, 0);
		if (pvNil == _pbmdl)
			return fFalse;
		CopyPb(&pmodlThis, _pbmdl->identifier, size(PMODL));

		_pbmdl->prepared_vertices = (BRV *)BrResAllocate(_pbmdl, 
			LwMul(modlf.cver, size(BRV)), BR_MEMORY_PREPARED_VERTICES);
		if (pvNil == _pbmdl->prepared_vertices)
			return fFalse;
		_pbmdl->prepared_faces = (BRF *)BrResAllocate(_pbmdl, 
			LwMul(modlf.cfac, size(BRF)), BR_MEMORY_PREPARED_FACES);
		if (pvNil == _pbmdl->prepared_faces)
			return fFalse;

		if (!pblck->FReadRgb(_pbmdl->prepared_vertices, cbrgbrv, 
			size(MODLF)))
			{
			return fFalse;
			}
		if (kboOther == modlf.bo)
			{
			for (ibrv = 0, pbrv = _pbmdl->prepared_vertices; 
				ibrv < modlf.cver; ibrv++, pbrv++)
				{
				SwapBytesBom(pbrv, kbomBrv);
				}
			}
		if (!pblck->FReadRgb(_pbmdl->prepared_faces, cbrgbrf, 
			size(MODLF) + cbrgbrv))
			{
			return fFalse;
			}
		if (kboOther == modlf.bo)
			{
			for (ibrf = 0, pbrf = _pbmdl->prepared_faces; ibrf < modlf.cfac;
				ibrf++, pbrf++)
				{
				SwapBytesBom(pbrf, kbomBrf);
				}
			}

		_pbmdl->flags = BR_MODF_PREPREPARED;
		_pbmdl->nprepared_vertices = (ushort)modlf.cver;
		_pbmdl->nprepared_faces = (ushort)modlf.cfac;

		// The following code assumes that there is no material data
		// in the models.  If there is material data, the code will have
		// to change to read vertex groups and face groups from file.
		_pbmdl->nvertex_groups = 1;
		_pbmdl->nface_groups = 1;
		_pbmdl->vertex_groups = (br_vertex_group *)BrResAllocate(_pbmdl, 
			size(br_vertex_group), BR_MEMORY_GROUPS);
		if (pvNil == _pbmdl->vertex_groups)
			return fFalse;
		_pbmdl->vertex_groups->material = pvNil;
		_pbmdl->vertex_groups->vertices = _pbmdl->prepared_vertices;
		_pbmdl->vertex_groups->nvertices = _pbmdl->nprepared_vertices;
		_pbmdl->face_groups = (br_face_group *)BrResAllocate(_pbmdl, 
			size(br_face_group), BR_MEMORY_GROUPS);
		if (pvNil == _pbmdl->face_groups)
			return fFalse;
		_pbmdl->face_groups->material = pvNil;
		_pbmdl->face_groups->faces = _pbmdl->prepared_faces;
		_pbmdl->face_groups->nfaces = _pbmdl->nprepared_faces;
		_pbmdl->radius = modlf.rRadius;
		_pbmdl->bounds = modlf.brb;
		_pbmdl->pivot = modlf.bvec3Pivot;
		BrModelAdd(_pbmdl);
		}
	return fTrue;
}


/***************************************************************************
	Reads a BRender model from a .DAT file
***************************************************************************/
PMODL MODL::PmodlReadFromDat(FNI *pfni)
{
	AssertPo(pfni, ffniFile);

	STN stn;
	PMODL pmodl;

	pmodl = NewObj MODL;
	if (pvNil == pmodl)
		goto LFail;
	pfni->GetStnPath(&stn);
	pmodl->_pbmdl = BrModelLoad(stn.Psz());
	if (pvNil == pmodl->_pbmdl)
		goto LFail;
	pmodl->_pbmdl->flags |= BR_MODF_KEEP_ORIGINAL;
	BrModelPrepare(pmodl->_pbmdl, BR_MPREP_ALL);
	Assert(CchSz(pmodl->_pbmdl->identifier) >= size(void *),
		"no room for pmodl ptr");
	CopyPb(&pmodl, pmodl->_pbmdl->identifier, size(void *));
	AssertPo(pmodl, 0);
	return pmodl;
LFail:
	ReleasePpo(&pmodl);
	return pvNil;
}


/***************************************************************************
	Returns a pointer to the MODL that owns this BMDL
***************************************************************************/
PMODL MODL::PmodlFromBmdl(PBMDL pbmdl)
{
	AssertVarMem(pbmdl);
	PMODL pmodl = (PMODL)*(long *)pbmdl->identifier;
	AssertPo(pmodl, 0);
	return pmodl;
}


/***************************************************************************
	Destructor
***************************************************************************/
MODL::~MODL(void)
{
	AssertBaseThis(0);
	if (pvNil != _pbmdl)
		{
		BrModelRemove(_pbmdl);
		BrModelFree(_pbmdl);
		}
}


/***************************************************************************
	Writes a MODL to a chunk
***************************************************************************/
bool MODL::FWrite(PCFL pcfl, CTG ctg, CNO cno)
{
	AssertThis(0);
	AssertPo(pcfl, 0);

	long cb;
	long cbrgbrv;
	long cbrgbrf;
	MODLF *pmodlf;

	cbrgbrv = LwMul(_pbmdl->nprepared_vertices, size(br_vertex));
	cbrgbrf = LwMul(_pbmdl->nprepared_faces, size(br_face));
	cb = size(MODLF) + cbrgbrv + cbrgbrf;
	if (!FAllocPv((void **)&pmodlf, cb, fmemClear, mprNormal))
		goto LFail;
	pmodlf->bo = kboCur;
	pmodlf->osk = koskCur;
	pmodlf->cver = _pbmdl->nprepared_vertices;
	pmodlf->cfac = _pbmdl->nprepared_faces;
	pmodlf->rRadius = _pbmdl->radius;
	pmodlf->brb = _pbmdl->bounds;
	pmodlf->bvec3Pivot = _pbmdl->pivot;
	CopyPb(_pbmdl->prepared_vertices, PvAddBv(pmodlf, size(MODLF)), cbrgbrv);
	CopyPb(_pbmdl->prepared_faces, PvAddBv(pmodlf, size(MODLF) + cbrgbrv), 
		cbrgbrf);
	if (!pcfl->FPutPv(pmodlf, cb, ctg, cno))
		goto LFail;
	FreePpv((void **)&pmodlf);
	return fTrue;
LFail:
	Warn("model save failed.");
	FreePpv((void **)&pmodlf);
	return fFalse;
}


/***************************************************************************
	Adjust glyph for a TDF.  It is centered in X and Z, with Y at the
	baseline, and we do some voodoo to get "kerning" (really "variable 
	interletter spacing") to work.
***************************************************************************/
void MODL::AdjustTdfCharacter(void)
{
	AssertThis(0);

	BRS dxrModl = Dxr();
	BRS dxrSpacing = _pbmdl->bounds.min.v[0];
	BRS dxr = BrsHalf(dxrModl) + dxrSpacing;
	BRS dzrModl = Dzr();
	BRS dzr = BrsHalf(dzrModl);
	long cbrv = _pbmdl->nprepared_vertices;
	long ibrv;

	for (ibrv = 0; ibrv < cbrv; ibrv++)
		{
		_pbmdl->prepared_vertices[ibrv].p.v[0] -= dxr;
		_pbmdl->prepared_vertices[ibrv].p.v[2] -= dzr;
		}
	_pbmdl->bounds.min.v[0] -= dxr;
	_pbmdl->bounds.max.v[0] -= dxr;
	_pbmdl->pivot.v[0] -= dxr;
	_pbmdl->bounds.min.v[0] -= dxrSpacing;
	if (dxrSpacing < BR_SCALAR(-0.01))
		dxrSpacing = BR_SCALAR(0.5);
	_pbmdl->bounds.max.v[0] += dxrSpacing;

	_pbmdl->bounds.min.v[2] -= dzr;
	_pbmdl->bounds.max.v[2] -= dzr;
	_pbmdl->pivot.v[2] -= dzr;
}

/***************************************************************************
	Prelight a model
	REVIEW *****: make this code more general
***************************************************************************/
bool MODL::_FPrelight(long cblit, BVEC3 *prgbvec3Light)
{
	AssertIn(cblit, 1, 10);
	AssertPvCb(prgbvec3Light, LwMul(cblit, size(BVEC3)));
	AssertBaseThis(0);

	PBACT pbactWorld;
	PBACT pbactCamera;
	PBPMP pbpmpRGB;
	PBPMP pbpmpZ;
	PBACT pbactLight;
	BLIT blit;
	PBMTL pbmtl;
	long iblit;
	BCAM bcam = 
		{
		"bah",
		BR_CAMERA_PERSPECTIVE,
		BR_ANGLE_DEG(60.0), // REVIEW *****
		BR_SCALAR(1.0),
		BR_SCALAR(100.0),
		BR_SCALAR(16.0/9.0),
		544,
		306
		};

	const br_colour kbrcHilite = BR_COLOUR_RGB(255,255,255);
	const byte kbOpaque = 0xff;
	const br_ufraction kbrufKaHilite = BR_UFRACTION(0.10);
	const br_ufraction kbrufKdHilite = BR_UFRACTION(0.60);
	const br_ufraction kbrufKsHilite = BR_UFRACTION(0.00);
	const BRS krPowerHilite = BR_SCALAR(50);
	const long kiclrHilite = 108;  // palette index for hilite color

	ClearPb(&blit, size(BLIT));
	blit.colour = BR_COLOUR_RGB(0xff, 0xff, 0xff);
	blit.type = BR_LIGHT_DIRECT;
	blit.attenuation_c = BR_SCALAR(1.0);

	pbactWorld = BrActorAllocate(BR_ACTOR_NONE, pvNil);
	if (pvNil == pbactWorld)
		goto LFail;

	pbactCamera = BrActorAllocate(BR_ACTOR_CAMERA, &bcam);
	if (pvNil == pbactCamera)
		goto LFail;
	BrActorAdd(pbactWorld, pbactCamera);

	for (iblit = 0; iblit < cblit; iblit++)
		{
		pbactLight = BrActorAllocate(BR_ACTOR_LIGHT, pvNil);
		if (pvNil == pbactLight)
			goto LFail;
		pbactLight->type_data = &blit;
		BrActorAdd(pbactWorld, pbactLight);
		BrLightEnable(pbactLight);
		}								     

	pbpmpRGB = BrPixelmapAllocate(BR_PMT_INDEX_8, 544, 306, 0, 0);
	pbpmpZ = BrPixelmapAllocate(BR_PMT_DEPTH_16, 544, 306, 0, 0);

	pbmtl = BrMaterialAllocate("Prelighting material");
	if (pvNil == pbmtl)
		goto LFail;
	pbmtl->colour = kbrcHilite;
	pbmtl->ka = kbrufKaHilite;
	pbmtl->kd = kbrufKdHilite,
	pbmtl->ks = kbrufKsHilite;
	pbmtl->power = krPowerHilite;
	pbmtl->flags = BR_MATF_LIGHT | BR_MATF_GOURAUD;
	pbmtl->index_base = kiclrHilite;
	pbmtl->index_range = 8;
	BrMaterialAdd(pbmtl);
	BrZbSceneRenderBegin(pbactWorld, pbactCamera, pbpmpRGB, pbpmpZ);
	BrSceneModelLight(_pbmdl, pbmtl, pbactWorld, pvNil);
	BrZbSceneRenderEnd();
	BrMaterialRemove(pbmtl);

	Assert(cblit == 1, "code needs to get smarter...");
	BrLightDisable(pbactLight); 

	BrActorFree(pbactWorld);

	return fTrue;
LFail:
	BrActorFree(pbactWorld);
	return fFalse;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the MODL.
***************************************************************************/
void MODL::AssertValid(ulong grf)
{
	MODL_PAR::AssertValid(fobjAllocated);
	AssertVarMem(_pbmdl);
	Assert((PMODL)*(long *)_pbmdl->identifier == this, 
		"Bad MODL identifier");
}


/***************************************************************************
	Mark memory used by the MODL
***************************************************************************/
void MODL::MarkMem(void)
{
	AssertThis(0);

	MODL_PAR::MarkMem();
}
#endif //DEBUG


