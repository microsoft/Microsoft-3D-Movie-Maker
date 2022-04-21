/***************************************************************************

	body.cpp: Body class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	A BODY holds the BRender-related data structures that make up what
	Socrates calls an actor.  The BODY keeps track of all the body parts'
	models, matrices, and materials that make up the actor's shape,
	position, orientation, and costume.  ACTR and TMPL are the main
	clients of BODY.  From the client's point of view, a BODY consists
	not of a tree of body parts, but an array of parts and part sets.  The
	"ibact"s and "ibset"s in the BODY APIs are indices into these arrays.

	PbodyNew() takes a parameter called pglibactPar, which is a GL of
	shorts.  Each short is the body part number of a body part's parent
	body part.	For example, suppose you passed in a pglibactPar of:

	(ivNil, 0, 0, 1, 2, 2)

	Body part 0 would have no parent (the first number in the GL is
	always ivNil).  Body part 1's parent would be body part 0.  Body part
	2's	parent would also be body part 0.  Body part 3's parent would be
	body part 1, etc.  The resulting tree would be:
		
				0
				|
	         +--+--+
			 |	   |
			 1   +-2-+
			 |	 |   |
			 3	 4	 5

	BODY needs to keep track of three sets of BACTs: first, a root BACT
	whose transformation is changed to position and orient the BODY.
	Second, a BACT for the actor hilighting.  Finally, there are
	the body parts for the BODY.  So the BRender tree *really* looks like:

				            root
				              |
					   +------+-----+
					   |			|
				     part0		 hilite
				       |
	           +-------+--------+
		 	   |		        |
	         part1            part2
	           |	            |
		       |		    +---+---+
               |		    |	    |
		     part3	      part4	  part5
				              			

	All these BACTs are allocated and released in one _prgbact.  The
	root is _prgbact[0].  The hilite BACT is _prgbact[1].  The real
	body parts are _prgbact[2] through _prgbact[1 + 1 + _cbactPart].
	There are APIs to access various members of this array more easily.

	The second parameter to PbodyNew is pglibset, which divides the body
	parts into "body part sets."  A body part set is one or more body
	parts which are texture-mapped as a group.  For example, putting
	a shirt on a human would affect the torso and both arms, so those
	three body parts would be grouped into a body part set.  If body
	parts 0, 1, and 2 were in set 0; part 3 was in set 1; and parts 4
	and 5 were in set 2, _pglibset would be {0, 0, 0, 1, 2, 2}.

	When applying a MTRL to a body part set, the same MTRL is
	attached to each body part in the set.  When a CMTL is applied,
	the CMTL has a different MTRL per body part in the set.

	The way that MODLs, MTRLs, and CMTLs attach to the BODY is a little
	strange.  In the case of models, the br_actor's model needs to be set
	to the MODL's BMDL (which is a BRender data structure), not the
	MODL itself (which is a Socrates data structure).  When it is time
	to remove the model, the BODY has a pointer to the BMDL, but
	not the MODL!  Fortunately, MODL sets the BMDL's identifier field
	to point to its owning MODL.  So ReleasePpo can then be called on the
	MODL.  But I don't want to set the BMDL's identifier to pvNil in
	the process, because other BODYs might be counting on the BMDL's
	identifier to point to the MODL.  So this code has to be careful using
	ReleasePpo, since that sets the given pointer to pvNil.  Same
	problem for MTRLs:

	BODY
	 |					 MODL<--+
     v					   |	|
	 BACT   			   |	|
	  | |				   |	|
	  |	v				   v	|
	  |model------------>BMDL	|
	  |					   |	|
	  |					   v	|
	  |					  identifier
	  |
	  +--+
	     |			     MTRL<--+
         |                 |	|
		 v				   v	|
	   material--------->BMTL	|
						   |	|
	                       v	|
						  identifier

	CMTLs are potentially even messier, since they're an abstraction on
	top of MTRLs.  Here, an array of PCMTLs on the BODY is explicitly kept
	so it is easy to find out what CMTL is attached to what body part set
	instead of working backwards from the BACT's material field.

***************************************************************************/
#include "soc.h"
ASSERTNAME

RTCLASS(BODY)
RTCLASS(COST)

// Specification of hilite color.  REVIEW manders: should these
// values (or the PBMTL itself?) be passed in by the client?
const br_colour kbrcHilite = BR_COLOUR_RGB(255,255,255);
const byte kbOpaque = 0xff;
const br_ufraction kbrufKaHilite = BR_UFRACTION(0.10);
const br_ufraction kbrufKdHilite = BR_UFRACTION(0.60);
const br_ufraction kbrufKsHilite = BR_UFRACTION(0.60);
const BRS krPowerHilite = BR_SCALAR(50);
const long kiclrHilite = 0;  // default to no highlighting

// Hilighting material
PBMTL BODY::_pbmtlHilite = pvNil;

PBODY BODY::_pbodyClosestClicked;
long BODY::_dzpClosestClicked;
PBACT BODY::_pbactClosestClicked;


/***************************************************************************
	Builds a tree of BACTs for the BODY.
***************************************************************************/
BODY *BODY::PbodyNew(PGL pglibactPar, PGL pglibset)
{
	AssertPo(pglibactPar, 0);
	Assert(pglibactPar->CbEntry() == size(short), "bad pglibactPar");
	AssertPo(pglibset, 0);
	Assert(pglibset->CbEntry() == size(short), "bad pglibset");

	BODY *pbody;

	if (pvNil == _pbmtlHilite)
		{
		_pbmtlHilite = BrMaterialAllocate("Hilite");
		if (pvNil == _pbmtlHilite)
			return pvNil;
		_pbmtlHilite->colour = kbrcHilite;
		_pbmtlHilite->ka = kbrufKaHilite;
		_pbmtlHilite->kd = kbrufKdHilite,
		_pbmtlHilite->ks = kbrufKsHilite;
		_pbmtlHilite->power = krPowerHilite;
		_pbmtlHilite->flags = BR_MATF_LIGHT | BR_MATF_GOURAUD | BR_MATF_FORCE_Z_0;
		_pbmtlHilite->index_base = kiclrHilite;
		_pbmtlHilite->index_range = 1;
		BrMaterialAdd(_pbmtlHilite);
		}

	pbody = NewObj BODY;
	if (pvNil == pbody || !pbody->_FInit(pglibactPar, pglibset))
		{
		ReleasePpo(&pbody);
		return pvNil;
		}

	AssertPo(pbody, fobjAssertFull);
	return pbody;
}


/***************************************************************************
	Build the BODY
***************************************************************************/
bool BODY::_FInit(PGL pglibactPar, PGL pglibset)
{
	AssertBaseThis(0);
	AssertPo(pglibactPar, 0);
	AssertPo(pglibset, 0);

	_cactHidden = 1; // body starts out hidden

	if (!_FInitShape(pglibactPar, pglibset))
		return fFalse;

	return fTrue;
}


/***************************************************************************
	Build the BODY
***************************************************************************/
bool BODY::_FInitShape(PGL pglibactPar, PGL pglibset)
{
	AssertBaseThis(0);
	AssertPo(pglibactPar, 0);
	Assert(pglibactPar->CbEntry() == size(short), "bad pglibactPar");
	AssertPo(pglibset, 0);
	Assert(pglibset->CbEntry() == size(short), "bad pglibset");
	Assert(pglibactPar->IvMac() == 0 ||
		ivNil == *(short *)pglibactPar->QvGet(0),
		"bad first item in pglibactPar");
	Assert(pglibactPar->IvMac() == pglibset->IvMac(),
		"pglibactPar must be same size as pglibset");

	BACT *pbact;
	BACT *pbactPar;
	short ibactPar;
	short ibact;
	short ibset;

	// Copy pglibset into _pglibset
	_pglibset = pglibset->PglDup();
	if (pvNil == _pglibset)
		return fFalse;

	// _cbset is (highest entry in _pglibset) + 1.
	_cbset = -1;
	for (ibact = 0; ibact < _pglibset->IvMac(); ibact++)
		{
		_pglibset->Get(ibact, &ibset);
		if (ibset > _cbset)
			_cbset = ibset;
		}
	_cbset++;

	if (!FAllocPv((void **)&_prgpcmtl, LwMul(_cbset, size(PCMTL)),
		fmemClear, mprNormal))
		{
		return fFalse;
		}

	_cbactPart = pglibactPar->IvMac();
	Assert(_cbset <= _cbactPart, "More sets than body parts?");
	if (!FAllocPv((void **)&_prgbact, LwMul(_Cbact(), size(BACT)),
		fmemClear, mprNormal))
		{
		return fFalse;
		}
	// first, set up the root
	pbact = _PbactRoot();
	pbact->type = BR_ACTOR_NONE;
	pbact->t.type = BR_TRANSFORM_MATRIX34;
	BrMatrix34Identity(&pbact->t.t.mat);
	pbact->identifier = (schar *)this; // to find BODY from a BACT

	// next, set up hilite actor
	pbact = _PbactHilite();
	pbact->type = BR_ACTOR_NONE;
	pbact->t.type = BR_TRANSFORM_MATRIX34;
	BrMatrix34Identity(&pbact->t.t.mat);
	pbact->identifier = (schar *)this; // to find BODY from a BACT
	pbact->material = _pbmtlHilite;
	pbact->render_style = BR_RSTYLE_BOUNDING_EDGES;
	BrActorAdd(_PbactRoot(), pbact);

	// now set up the body part BACTs
	for (ibact = 0; ibact < _cbactPart; ibact++)
		{
		pbact = _PbactPart(ibact);
		pbact->type = BR_ACTOR_MODEL;
		pbact->render_style = BR_RSTYLE_FACES;
		pbact->t.type = BR_TRANSFORM_MATRIX34;
		BrMatrix34Identity(&pbact->t.t.mat);
		pbact->identifier = (schar *)this; // to find BODY from a BACT

		// Find parent of this body part
		pglibactPar->Get(ibact, &ibactPar);
		if (ivNil == ibactPar)
			{
			pbactPar = _PbactRoot();
			}
		else
			{
			AssertIn(ibactPar, 0, ibact);
			pbactPar = _PbactPart(ibactPar);
			}
		BrActorAdd(pbactPar, pbact);
		}
	return fTrue;
}


/***************************************************************************
	Change the body part hierarchy and/or the body part sets of this BODY.
	Models, materials, and matrices are not changed for body parts that
	exist in both the old and reshaped BODYs.
***************************************************************************/
bool BODY::FChangeShape(PGL pglibactPar, PGL pglibset)
{
	AssertThis(fobjAssertFull);
	AssertPo(pglibactPar, 0);
	AssertPo(pglibset, 0);

	PBODY pbodyDup;
	long ibset;
	bool fMtrl;
	PMTRL pmtrl;
	PCMTL pcmtl;
	long ibact;
	PBACT pbactDup;
	PBACT pbact;
	long cactHidden = _cactHidden;

	pbodyDup = PbodyDup();
	if (pvNil == pbodyDup)
		goto LFail;

	_DestroyShape();	// note: hides the body
	if (!_FInitShape(pglibactPar, pglibset))
		goto LFail;
	if (cactHidden == 0) // if body was visible
		Show();
	// Restore materials
	for (ibset = 0; ibset < LwMin(_cbset, pbodyDup->_cbset); ibset++)
		{
		pbodyDup->GetPartSetMaterial(ibset, &fMtrl, &pmtrl, &pcmtl);
		if (fMtrl)
			SetPartSetMtrl(ibset, pmtrl);
		else
			SetPartSetCmtl(pcmtl);
		}	
	// Restore models and matrices
	for (ibact = 0; ibact < LwMin(_cbactPart, pbodyDup->_cbactPart); ibact++)
		{
		pbactDup = pbodyDup->_PbactPart(ibact);
		pbact = _PbactPart(ibact);
		if (pvNil != pbactDup->model)
			SetPartModel(ibact, MODL::PmodlFromBmdl(pbactDup->model));
		pbact->t.t.mat = pbactDup->t.t.mat;
		}
	_PbactRoot()->t.t.mat = pbodyDup->_PbactRoot()->t.t.mat;
	// Restore hilite state
	if (pbodyDup->_PbactHilite()->type == BR_ACTOR_MODEL)
		Hilite(); // body was hilited, so hilite it now
	ReleasePpo(&pbodyDup);
	AssertThis(fobjAssertFull);
	return fTrue;
LFail:
	if (pvNil != pbodyDup)
		{
		Restore(pbodyDup);
		if (cactHidden == 0)
			Show();
		ReleasePpo(&pbodyDup);
		}
	AssertThis(fobjAssertFull);
	return fFalse;
}


/***************************************************************************
	Returns the BODY that owns the given BACT.  Also, if pibset is not
	nil, returns what body part set this BACT is in.  If the pbact is
	the hilite BACT, pibset is set to ivNil.
***************************************************************************/
BODY *BODY::PbodyFromBact(BACT *pbact, long *pibset)
{				
	AssertVarMem(pbact);
	AssertNilOrVarMem(pibset);

	PBODY pbody;
	long ibact;

	pbody = (BODY *)pbact->identifier;
	if (pvNil == pbody)
		{
		Bug("What actor is this?  It has no BODY");
		return pvNil;
		}
	AssertPo(pbody, 0);
	if (pvNil != pibset)
		{
		*pibset = ivNil;
		for (ibact = 0; ibact < pbody->_cbactPart; ibact++)
			{
			if (pbact == pbody->_PbactPart(ibact))
				{
				*pibset = pbody->_Ibset(ibact);
				break;
				}
			}
		}
	return pbody;
}


/***************************************************************************
	Returns the BODY that is under the given point.  Also, if pibset is not
	nil, returns what body part set this point is in.
***************************************************************************/
BODY *BODY::PbodyClicked(long xp, long yp, PBWLD pbwld, long *pibset)
{				
	AssertNilOrVarMem(pibset);
	AssertPo(pbwld, 0);

	PBODY pbody;
	long ibact;

	_pbodyClosestClicked = pvNil;
	_dzpClosestClicked = BR_SCALAR_MAX;

	pbwld->IterateActorsInPt(BODY::_FFilter, pvNil, xp, yp);

	pbody = _pbodyClosestClicked;
	if (pvNil == _pbodyClosestClicked)
		{
		return pvNil;
		}
	AssertPo(_pbodyClosestClicked, 0);
	if (pvNil != pibset)
		{
		*pibset = ivNil;
		for (ibact = 0; ibact < _pbodyClosestClicked->_cbactPart; ibact++)
			{
			if (_pbactClosestClicked == _pbodyClosestClicked->_PbactPart(ibact))
				{
				*pibset = _pbodyClosestClicked->_Ibset(ibact);
				break;
				}
			}
		}
	return _pbodyClosestClicked;
}


/***************************************************************************
	Filter callback proc for PbodyClicked().  Saves pbody if it's the
	closest one hit so far and visible
***************************************************************************/
int BODY::_FFilter(BACT *pbact, PBMDL pbmdl, PBMTL pbmtl, 
	BVEC3 *pbvec3RayPos, BVEC3 *pbvec3RayDir, BRS dzpNear, BRS dzpFar, 
	void *pv)
{
	AssertVarMem(pbact);
	AssertVarMem(pbvec3RayPos);
	AssertVarMem(pbvec3RayDir);

	PBODY pbody;

	pbody = BODY::PbodyFromBact(pbact);

	if ((dzpNear < _dzpClosestClicked) &&
		(pbody != pvNil) &&
		(pbody->FIsInView()))
		{
		_pbodyClosestClicked = pbody;
		_dzpClosestClicked = dzpNear;
		_pbactClosestClicked = pbact;
		}

	return fFalse; // fFalse means keep searching
}


/***************************************************************************
	Destroy the BODY's body parts, including their attached materials and
	models.
***************************************************************************/
void BODY::_DestroyShape(void)
{
	AssertBaseThis(0);
	long ibset;
	long ibact;
	BACT *pbact;
	MODL *pmodl;

	// Must hide body before destroying it
	if (_cactHidden == 0)
		Hide();

	if (pvNil != _prgbact)
		{
		for (ibact = 0; ibact < _cbactPart; ibact++)
			{
			pbact = _PbactPart(ibact);
			if (pvNil != pbact->model)
				{
				pmodl = MODL::PmodlFromBmdl(pbact->model);
				AssertPo(pmodl, 0);
				ReleasePpo(&pmodl);
				pbact->model = pvNil;
				}
			}
		if (pvNil != _prgpcmtl)
			{
			for (ibset = 0; ibset < _cbset; ibset++)
				_RemoveMaterial(ibset);
			}
		FreePpv((void **)&_prgbact);
		}
	FreePpv((void **)&_prgpcmtl);
	ReleasePpo(&_pglibset);
}


/***************************************************************************
	Free the BODY and all attached MODLs, MTRLs, and CMTLs.
***************************************************************************/
BODY::~BODY(void)
{
	AssertBaseThis(0);
	_DestroyShape();
}


/***************************************************************************
	Create a duplicate of this BODY.  The duplicate will be hidden
	(_cactHidden == 1) regardless of the state of this BODY.
***************************************************************************/
PBODY BODY::PbodyDup(void)
{
	AssertThis(0);

	PBODY pbodyDup;
	long ibact;
	long ibset;
	PBACT pbact;
	long bv; // delta in bytes from _prgbact to _pbody->_prgbact
	bool fMtrl;
	PMTRL pmtrl;
	PCMTL pcmtl;
	bool fVis;

	fVis = (_cactHidden == 0);
	if (fVis)
		Hide(); // temporarily hide this BODY

	pbodyDup = NewObj BODY;
	if (pvNil == pbodyDup)
		goto LFail;

	pbodyDup->_cactHidden = 1;
	pbodyDup->_cbset = _cbset;
	pbodyDup->_cbactPart = _cbactPart;
	pbodyDup->_pbwld = _pbwld;
	pbodyDup->_fFound = _fFound;
	pbodyDup->_ibset = _ibset;

	if (!FAllocPv((void **)&pbodyDup->_prgbact, LwMul(_Cbact(), size(BACT)),
		fmemClear, mprNormal))
		{
		goto LFail;
		}
	CopyPb(_prgbact, pbodyDup->_prgbact, LwMul(_Cbact(), size(BACT)));
	// need to update BACT parent, child, next, prev pointers
	bv = BvSubPvs(pbodyDup->_prgbact, _prgbact);
	for (ibact = 0; ibact < _Cbact(); ibact++)
		{
		pbact = &pbodyDup->_prgbact[ibact];
		pbact->identifier = (schar *)pbodyDup;
		if (ibact == 0)
			{
			pbact->parent = pvNil;
			if (pvNil != pbact->children)
				pbact->children = (PBACT)PvAddBv(pbact->children, bv);
			pbact->next = pvNil;
			pbact->prev = pvNil;
			}
		else
			{
			if (pvNil != pbact->parent)
				pbact->parent = (PBACT)PvAddBv(pbact->parent, bv);
			if (pvNil != pbact->children)
				pbact->children = (PBACT)PvAddBv(pbact->children, bv);
			if (pvNil != pbact->next)
				pbact->next = (PBACT)PvAddBv(pbact->next, bv);
			if (pvNil != pbact->prev)
				pbact->prev = (PBACT *)PvAddBv(pbact->prev, bv);
			}
		}
	for (ibact = 0; ibact < pbodyDup->_cbactPart; ibact++)
		{
		pbact = _PbactPart(ibact);
		if (pvNil != pbact->model)
			MODL::PmodlFromBmdl(pbact->model)->AddRef();
		}

	pbodyDup->_pglibset = _pglibset->PglDup();
	if (pvNil == pbodyDup->_pglibset)
		goto LFail;

	if (!FAllocPv((void **)&pbodyDup->_prgpcmtl, LwMul(_cbset, size(PCMTL)),
		fmemClear, mprNormal))
		{
		goto LFail;
		}
	CopyPb(_prgpcmtl, pbodyDup->_prgpcmtl, LwMul(_cbset, size(PCMTL)));

	pbodyDup->_rcBounds = _rcBounds;
	pbodyDup->_rcBoundsLastVis = _rcBoundsLastVis;

	for (ibset = 0; ibset < _cbset; ibset++)
		{
		pbodyDup->GetPartSetMaterial(ibset, &fMtrl, &pmtrl, &pcmtl);
		if (fMtrl)
			{
			// need to AddRef once per body part in this set
			for (ibact = 0; ibact < pbodyDup->_cbactPart; ibact++)
				{
				if (ibset == _Ibset(ibact))
					pmtrl->AddRef();
				}
			}
		else
			{
			pcmtl->AddRef();
			}
		}
	if (fVis)
		Show(); // re-show this BODY (but not the duplicate)

	AssertPo(pbodyDup, 0);
	return pbodyDup;
LFail:
	if (fVis)
		Show();
	ReleasePpo(&pbodyDup);
	return pvNil;
}


/***************************************************************************
	Replace this BODY with pbodyDup.  Preserve this BODY's hidden-ness.
***************************************************************************/
void BODY::Restore(PBODY pbodyDup)
{
	AssertBaseThis(0);
	AssertPo(pbodyDup, 0);
	Assert(pbodyDup->_cactHidden == 1, "dup hidden count must be 1");

	long cactHidden = _cactHidden;
	long ibact;

	SwapVars(this, pbodyDup);
	for (ibact = 0; ibact < _Cbact(); ibact++)
		_prgbact[ibact].identifier = (schar *)this;
	if (cactHidden == 0)
		Show();
	_cactHidden = cactHidden;

	AssertThis(fobjAssertFull);
}


/***************************************************************************
	Make this BODY visible, if it was invisible.  Keeps a refcount.
***************************************************************************/
void BODY::Show(void)
{
	AssertThis(0);
	Assert(_cactHidden > 0, "object is already visible!");
	if (--_cactHidden == 0)
		{
		AssertPo(_pbwld, 0);
		_pbwld->AddActor(_PbactRoot());
		_pbwld->SetBeginRenderCallback(_PrepareToRender);
		_pbwld->SetActorRenderedCallback(_BactRendered);
		_pbwld->SetGetRcCallback(_GetRc);
		_pbwld->MarkDirty(); // need to render
		}
}


/***************************************************************************
	Make this BODY invisible, if it was visible.  Keeps a refcount.
***************************************************************************/
void BODY::Hide(void)
{
	AssertThis(0);

	RC rc;

	if (_cactHidden++ == 0)
		{
		_rcBounds.Zero();
		BrActorRemove(_PbactRoot());
		AssertPo(_pbwld, 0);
		_pbwld->MarkDirty(); // need to render
		}
}


/***************************************************************************
	Sets the hilite color to use.
***************************************************************************/
void BODY::SetHiliteColor(long iclr)
{
	if (_pbmtlHilite != pvNil)
		{
		_pbmtlHilite->index_base = (UCHAR)iclr;
		}
}


/***************************************************************************
	Hilites the BODY
***************************************************************************/
void BODY::Hilite(void)
{
	AssertThis(0);

	_PbactHilite()->type = BR_ACTOR_MODEL;
	if (_cactHidden == 0)
		{
		AssertPo(_pbwld, 0);
		_pbwld->MarkDirty(); // need to render
		}
}


/***************************************************************************
	Unhilites the BODY
***************************************************************************/
void BODY::Unhilite(void)
{
	AssertThis(0);

	_PbactHilite()->type = BR_ACTOR_NONE;
	if (_cactHidden == 0)
		{
		AssertPo(_pbwld, 0);
		_pbwld->MarkDirty(); // need to render
		}
}


/***************************************************************************
	Position the body at (xr, yr, zr) in worldspace	oriented by pbmat34
***************************************************************************/
void BODY::LocateOrient(BRS xr, BRS yr, BRS zr, BMAT34 *pbmat34)
{
	AssertThis(0);
	AssertVarMem(pbmat34);

	_PbactRoot()->t.t.mat = *pbmat34;
	BrMatrix34PostTranslate(&_PbactRoot()->t.t.mat, xr, yr, zr);

	if (_cactHidden == 0)
		{
		AssertPo(_pbwld, 0);
		_pbwld->MarkDirty(); // need to render
		}
}


/***************************************************************************
	Set the ibact'th body part to use model pmodl
***************************************************************************/
void BODY::SetPartModel(long ibact, MODL *pmodl)
{
	AssertThis(0);
	AssertIn(ibact, 0, _cbactPart);
	AssertPo(pmodl, 0);

	BACT *pbact = _PbactPart(ibact);
	PMODL pmodlOld;

	if (pvNil != pbact->model)	// Release old MODL, unless it's pmodl
		{
		pmodlOld = MODL::PmodlFromBmdl(pbact->model);
		AssertPo(pmodlOld, 0);
		if (pmodl == pmodlOld)
			return; // We're already using that MODL, so do nothing
		ReleasePpo(&pmodlOld);
		}

	pbact->model = pmodl->Pbmdl();
	Assert(MODL::PmodlFromBmdl(pbact->model) == pmodl, "MODL problem");
	pmodl->AddRef();
	if (_cactHidden == 0)
		{										
		AssertPo(_pbwld, 0);
		_pbwld->MarkDirty(); // need to render
		}
}


/***************************************************************************
	Set the ibact'th body part to use matrix pbmat34
***************************************************************************/
void BODY::SetPartMatrix(long ibact, BMAT34 *pbmat34)
{
	AssertThis(0);
	AssertIn(ibact, 0, _cbactPart);
	AssertVarMem(pbmat34);

	_PbactPart(ibact)->t.t.mat = *pbmat34;
	if (_cactHidden == 0)
		{
		AssertPo(_pbwld, 0);
		_pbwld->MarkDirty(); // need to render
		}
}


/***************************************************************************
	Remove old MTRL or CMTL from ibset.  This is nontrivial because there
	could either be a CMTL attached to the bset (in which case we just free
	the CMTL) or a bunch of MTRLs (in which case we free each MTRL).
	Actually, in the latter case, it would be a bunch of copies of the same
	MTRL, but we keep one reference per body part in the set.
***************************************************************************/
void BODY::_RemoveMaterial(long ibset)
{
	AssertThis(0);
	AssertIn(ibset, 0, _cbset);

	PCMTL pcmtlOld;
	BACT *pbact;
	long ibact;
	bool fCmtl = fFalse;

	pcmtlOld = _prgpcmtl[ibset];
	if (pvNil != pcmtlOld) // there was a CMTL on this bset
		{
		fCmtl = fTrue;
		AssertPo(pcmtlOld, 0);
		ReleasePpo(&pcmtlOld); // free all old MTRLs
		_prgpcmtl[ibset] = pvNil;
		}
	// for each body part, if this part is in the set ibset, free the MTRL
	for (ibact = 0; ibact < _cbactPart; ibact++)
		{
		if (ibset == _Ibset(ibact))
			{
			pbact = _PbactPart(ibact);
			if (pbact->material != pvNil) // free old MTRL
				{
				if (!fCmtl)
					MTRL::PmtrlFromBmtl(pbact->material)->Release();
				pbact->material = pvNil;
				}
			}
		}
}


/***************************************************************************
	Set the ibset'th body part set to use material pmtrl
***************************************************************************/
void BODY::SetPartSetMtrl(long ibset, MTRL *pmtrl)
{
	AssertThis(0);
	AssertIn(ibset, 0, _cbset);
	AssertPo(pmtrl, 0);

	BACT *pbact;
	long ibact;

	_RemoveMaterial(ibset); // remove existing MTRL/CMTL, if any

	// for each body part, if this part is in the set ibset, set the MTRL
	for (ibact = 0; ibact < _cbactPart; ibact++)
		{
		if (ibset == _Ibset(ibact))
			{
			pbact = _PbactPart(ibact);
			pbact->material = pmtrl->Pbmtl();
			pmtrl->AddRef();
			}
		}
	if (_cactHidden == 0)
		{
		AssertPo(_pbwld, 0);
		_pbwld->MarkDirty(); // need to render
		}
}


/***************************************************************************
	Apply the given CMTL to the appropriate body part set (the CMTL knows
	which body part set to apply to).
***************************************************************************/
void BODY::SetPartSetCmtl(CMTL *pcmtl)
{
	AssertThis(0);
	AssertPo(pcmtl, 0);

	BACT *pbact;
	long ibact;
	long ibmtl = 0;
	long ibset = pcmtl->Ibset();
	PMODL pmodl;

	pcmtl->AddRef();
	_RemoveMaterial(ibset); // remove existing MTRL/CMTL
	_prgpcmtl[ibset] = pcmtl;

	// for each body part, if this part is in the set ibset, set the MTRL
	for (ibact = 0; ibact < _cbactPart; ibact++)
		{
		if (ibset == _Ibset(ibact))
			{
			pbact = _PbactPart(ibact);
			// Handle model changes for accessories
			pmodl = pcmtl->Pmodl(ibmtl);
			if (pvNil != pmodl)
				SetPartModel(ibact, pmodl);
			pbact->material = pcmtl->Pbmtl(ibmtl);
			ibmtl++;
			}
		}
	Assert(ibmtl == pcmtl->Cbprt(), "didn't use all custom materials!");
	if (_cactHidden == 0)
		{
		AssertPo(_pbwld, 0);
		_pbwld->MarkDirty(); // need to render
		}
}


/***************************************************************************
	Determines the current CMTL or MTRL applied to the given ibset.
	If a CMTL is attached, *pfMtrl is fFalse and *ppcmtl holds the
	PCMTL.  If a MTRL is attached, *pfMtrl is fTrue and *ppmtrl holds
	the PMTRL.
***************************************************************************/
void BODY::GetPartSetMaterial(long ibset, bool *pfMtrl, MTRL **ppmtrl,
	CMTL **ppcmtl)
{
	AssertThis(0);
	AssertIn(ibset, 0, _cbset);
	AssertVarMem(pfMtrl);
	AssertVarMem(ppmtrl);
	AssertVarMem(ppcmtl);

	BACT *pbact;
	long ibact;

	*ppcmtl = _prgpcmtl[ibset];
	if (pvNil != *ppcmtl) // there is a CMTL on this bset
		{
		*pfMtrl = fFalse;
		AssertPo(*ppcmtl, 0);
		TrashVar(ppmtrl);
		}
	else
		{
		*pfMtrl = fTrue;
		TrashVar(ppcmtl);
		// Find any body part of ibset...they'll all have the same MTRL
		for (ibact = 0; ibact < _cbactPart; ibact++)
			{
			if (ibset == _Ibset(ibact))
				{
				pbact = _PbactPart(ibact);
				Assert(pvNil != pbact->material, "Why does this body part "
					"set have neither MTRL nor CMTL attached?");
				*ppmtrl = MTRL::PmtrlFromBmtl(pbact->material);
				AssertPo(*ppmtrl, 0);
				return;
				}
			}
		Assert(0, "why are we here?");
		}
}


/***************************************************************************
	Filter callback proc for FPtInActor(). Stops when the BODY is hit.
***************************************************************************/
int BODY::_FFilterSearch(BACT *pbact, PBMDL pbmdl, PBMTL pbmtl, 
	BVEC3 *ray_pos, BVEC3 *ray_dir, BRS dzpNear, BRS dzpFar, void *pvArg)
{
	AssertVarMem(pbact);
	AssertVarMem(ray_pos);
	AssertVarMem(ray_dir);

	PBODY pbody = (PBODY)pvArg;
	PBODY pbodyFound;
	long ibset;

	AssertPo(pbody, 0);

	pbodyFound = BODY::PbodyFromBact(pbact, &ibset);
	if (pbodyFound == pvNil)
		{
		Bug("What actor is this?  It has no BODY");
		return fFalse;
		}

	AssertPo(pbodyFound, 0);

	if (pbody != pbodyFound)
		return fFalse; // keep searching

	pbody->_fFound = fTrue;
	if (pbact == pbody->_PbactHilite())
		return fFalse; // keep searching
	pbody->_ibset = ibset;
	return fTrue; // stop searching
}


/***************************************************************************
   Returns fTrue if this body is under (xp, yp)
***************************************************************************/
bool BODY::FPtInBody(long xp, long yp, long *pibset)
{
	AssertThis(0);

	_fFound = fFalse;
	_ibset = ivNil;
	_pbwld->IterateActorsInPt(&BODY::_FFilterSearch, (void *)this, xp, yp);
	*pibset = _ibset;
	return _fFound;
}


/***************************************************************************
	BWLD is about to render the world, so clear out this BODY's _rcBounds
	(but save the last good bounds in _rcBoundsLastVis).  Also size the
	bounding box correctly.
***************************************************************************/
void BODY::_PrepareToRender(PBACT pbact)
{
	AssertVarMem(pbact);
	PBODY pbody;
	RC rc;
	BCB bcb;
	
	pbody = PbodyFromBact(pbact);
	AssertPo(pbody, 0);

	if (pbody->FIsInView())
		{
		pbody->_rcBoundsLastVis = pbody->_rcBounds;
		pbody->_rcBounds.Zero();
		}

	// Prepare bounding box, if BODY is highlighted
	if (pbody->_PbactHilite()->type == BR_ACTOR_MODEL)
		{
		// Need to temporarily change type to 'none' so that the bounding
		// box isn't counted when calculating size of actor
		pbody->GetBcbBounds(&bcb);
		Assert(size(BRB) == size(BCB), "should be same structure");
		BrBoundsToMatrix34(&pbody->_PbactHilite()->t.t.mat, (BRB *)&bcb);
		}
}


/***************************************************************************
	Return the bounds of the BODY contaning PBACT
***************************************************************************/
void BODY::_GetRc(PBACT pbact, RC *prc)
{
	AssertVarMem(pbact);
	AssertVarMem(prc);

	PBODY pbody;
	
	pbody = PbodyFromBact(pbact);
	AssertPo(pbody, 0);

	if (pvNil != pbody)
		*prc = pbody->_rcBounds;
}


/***************************************************************************
	Compute the world-space bounding box of the BODY.  The code temporarily
	changes the hilite BACT's type to BR_ACTOR_NONE so that BrActorToBounds
	doesn't include the size of the bounding box when computing the size of
	the actor.
***************************************************************************/
void BODY::GetBcbBounds(BCB *pbcb, bool fWorld)
{
	AssertThis(0);
	AssertVarMem(pbcb);

	BRB brb;
	byte type = _PbactHilite()->type;
	long ibv3;
	br_vector3 bv3;

	_PbactHilite()->type = BR_ACTOR_NONE;
	Assert(size(BRB) == size(BCB), "should be same structure");
	BrActorToBounds(&brb, _PbactRoot());
	_PbactHilite()->type = type;
	*(BRB *)pbcb = brb;

	if (fWorld)
		{
		br_vector3 rgbv3[8];
		BMAT34 bmat34;

		rgbv3[0] = brb.min;
		rgbv3[1] = brb.min;
		rgbv3[1].v[0] = brb.max.v[0];
		rgbv3[2] = brb.min;
		rgbv3[2].v[1] = brb.max.v[1];
		rgbv3[3] = brb.min;
		rgbv3[3].v[2] = brb.max.v[2];
		rgbv3[4] = brb.max;
		rgbv3[5] = brb.max;
		rgbv3[5].v[0] = brb.min.v[0];
		rgbv3[6] = brb.max;
		rgbv3[6].v[1] = brb.min.v[1];
		rgbv3[7] = brb.max;
		rgbv3[7].v[2] = brb.min.v[2];

		bmat34 = _PbactRoot()->t.t.mat;

		for (ibv3 = 0; ibv3 < 8; ibv3++)
			{
			bv3.v[0] = BR_MAC4(rgbv3[ibv3].v[0], bmat34.m[0][0],
				rgbv3[ibv3].v[1], bmat34.m[1][0], rgbv3[ibv3].v[2],
				bmat34.m[2][0], BR_SCALAR(1.0), bmat34.m[3][0]);
			bv3.v[1] = BR_MAC4(rgbv3[ibv3].v[0], bmat34.m[0][1],
				rgbv3[ibv3].v[1], bmat34.m[1][1], rgbv3[ibv3].v[2],
				bmat34.m[2][1], BR_SCALAR(1.0), bmat34.m[3][1]);
			bv3.v[2] = BR_MAC4(rgbv3[ibv3].v[0], bmat34.m[0][2],
				rgbv3[ibv3].v[1], bmat34.m[1][2], rgbv3[ibv3].v[2],
				bmat34.m[2][2], BR_SCALAR(1.0), bmat34.m[3][2]);
			rgbv3[ibv3].v[0] = bv3.v[0];
			rgbv3[ibv3].v[1] = bv3.v[1];
			rgbv3[ibv3].v[2] = bv3.v[2];
			}
		pbcb->xrMin = pbcb->yrMin = pbcb->zrMin = BR_SCALAR(10000.0);
		pbcb->xrMax = pbcb->yrMax = pbcb->zrMax = BR_SCALAR(-10000.0);
		// Union with body's bounds
		for (ibv3 = 0; ibv3 < 8; ibv3++)
			{
			if (rgbv3[ibv3].v[0] < pbcb->xrMin)
				pbcb->xrMin = rgbv3[ibv3].v[0];
			if (rgbv3[ibv3].v[1] < pbcb->yrMin)
				pbcb->yrMin = rgbv3[ibv3].v[1];
			if (rgbv3[ibv3].v[2] < pbcb->zrMin)
				pbcb->zrMin = rgbv3[ibv3].v[2];
			if (rgbv3[ibv3].v[0] > pbcb->xrMax)
				pbcb->xrMax = rgbv3[ibv3].v[0];
			if (rgbv3[ibv3].v[1] > pbcb->yrMax)
				pbcb->yrMax = rgbv3[ibv3].v[1];
			if (rgbv3[ibv3].v[2] > pbcb->zrMax)
				pbcb->zrMax = rgbv3[ibv3].v[2];
			}

		}
}


/***************************************************************************
	BWLD calls this function when each BACT is rendered.  It unions the
	BACT bounds with the BODY's _rcBounds
***************************************************************************/
void BODY::_BactRendered(PBACT pbact, RC *prc)
{
	AssertVarMem(pbact);
	AssertVarMem(prc);
	PBODY pbody;
	
	pbody = PbodyFromBact(pbact);
	if (pvNil != pbody)
		pbody->_rcBounds.Union(prc);
}


/***************************************************************************
	Returns whether the BODY was in view the last time it was rendered.
***************************************************************************/
bool BODY::FIsInView(void)
{
	AssertThis(0);
	return !_rcBounds.FEmpty();
}


/***************************************************************************
	Fills in the 2D bounds of the BODY, the last time it was rendered.  If
	the BODY was not in view at the last render, this function fills in the
	bounds of the BODY the last time it was onstage.
***************************************************************************/
void BODY::GetRcBounds(RC *prc)
{
	AssertThis(0);
	AssertVarMem(prc);

	if (FIsInView())
		*prc = _rcBounds;
	else
		*prc = _rcBoundsLastVis;
}


/***************************************************************************
	Fills in the 2D coordinates of the center of the BODY, the last time
	it was rendered.  If the BODY was not in view at the last render, this
	function fills in the center of the BODY the last time it was onstage.
***************************************************************************/
void BODY::GetCenter(long *pxp, long *pyp)
{
	AssertThis(0);
	AssertVarMem(pxp);
	AssertVarMem(pyp);

	if (FIsInView())
		{
		*pxp = _rcBounds.XpCenter();
		*pyp = _rcBounds.YpCenter();
		}
	else
		{
		*pxp = _rcBoundsLastVis.XpCenter();
		*pyp = _rcBoundsLastVis.YpCenter();
		}
}


/***************************************************************************
	Fills in the current position of the origin of this BODY.
***************************************************************************/
void BODY::GetPosition(BRS *pxr, BRS *pyr, BRS *pzr)
{
	AssertThis(0);
	AssertVarMem(pxr);
	AssertVarMem(pyr);
	AssertVarMem(pzr);

	*pxr = _PbactRoot()->t.t.mat.m[3][0];
	*pyr = _PbactRoot()->t.t.mat.m[3][1];
	*pzr = _PbactRoot()->t.t.mat.m[3][2];
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the BODY.
***************************************************************************/
void BODY::AssertValid(ulong grf)
{
	long ibact;
	long ibset;
	BACT *pbact;

	BODY_PAR::AssertValid(fobjAllocated);
	AssertIn(_cactHidden, 0, 100); // 100 is sanity check
	AssertIn(_cbset, 0, _cbactPart + 1);
	AssertPvCb(_prgbact, LwMul(_Cbact(), size(BACT)));
	AssertPvCb(_prgpcmtl, LwMul(_cbset, size(PCMTL)));
	Assert(pvNil == _PbactRoot()->model,
		"BODY root shouldn't have a model!!");
	Assert(pvNil == _PbactRoot()->material,
		"BODY root shouldn't have a material!!");
	AssertPo(_pglibset, 0);
	Assert(_pglibset->CbEntry() == size(short), "bad _pglibset");

	if (grf & fobjAssertFull)
		{
		for (ibact = 0; ibact < _cbactPart; ibact++)
			{
			pbact = _PbactPart(ibact);
			if (pvNil != pbact->model)
				AssertPo(MODL::PmodlFromBmdl(pbact->model), 0);
			if (pvNil != pbact->material)
				AssertPo(MTRL::PmtrlFromBmtl(pbact->material), 0);
			}
		for (ibset = 0; ibset < _cbset; ibset++)
			{
			AssertNilOrPo(_prgpcmtl[ibset], 0);
			}
		}
}


/***************************************************************************
	Mark memory used by the BODY
***************************************************************************/
void BODY::MarkMem(void)
{
	AssertThis(0);

	long ibact;
	PBACT pbact;
	PMODL pmodl;
	long ibset;
	bool fMtrl;
	PMTRL pmtrl;
	PCMTL pcmtl;

	BODY_PAR::MarkMem();
	MarkPv(_prgbact);
	MarkPv(_prgpcmtl);
	MarkMemObj(_pglibset);

	for (ibact = 0; ibact < _cbactPart; ibact++)
		{
		pbact = _PbactPart(ibact);
		if (pvNil != pbact->model)
			{
			pmodl = MODL::PmodlFromBmdl(pbact->model);
			AssertPo(pmodl, 0);
			MarkMemObj(pmodl);
			}
		}
	for (ibset = 0; ibset < _cbset; ibset++)
		{
		GetPartSetMaterial(ibset, &fMtrl, &pmtrl, &pcmtl);
		if (fMtrl)
			MarkMemObj(pmtrl);
		else
			MarkMemObj(pcmtl);
		}
}
#endif //DEBUG


/***************************************************************************
	Create a blank costume -- no materials are attached yet
***************************************************************************/
COST::COST(void)
{
	TrashVar(&_cbset);
	_prgpo = pvNil;
}


/***************************************************************************
	Destroy a costume
***************************************************************************/
COST::~COST(void)
{
	AssertBaseThis(0);
	_Clear();
}


/***************************************************************************
	Release all arrays and references
***************************************************************************/
void COST::_Clear(void)
{
	AssertBaseThis(0);

	long ibset;

	if (pvNil != _prgpo)
		{
	 	for (ibset = 0; ibset < _cbset; ibset++)
			ReleasePpo(&_prgpo[ibset]); // Release the PCMTL or PMTRL
		FreePpv((void **)&_prgpo);
		}
	TrashVar(&_cbset);
}


/***************************************************************************
	Get a costume from a BODY
***************************************************************************/
bool COST::FGet(BODY *pbody)
{
	AssertThis(0);
	AssertPo(pbody, 0);

	long ibset;
	PCMTL pcmtl;
	PMTRL pmtrl;
	bool fMtrl;

	_Clear(); // drop previous costume, if any

	if (!FAllocPv((void **)&_prgpo, LwMul(size(BASE *), pbody->Cbset()), fmemClear,
		mprNormal))
		{
		return fFalse;
		}
	_cbset = pbody->Cbset();
	for (ibset = 0; ibset < _cbset; ibset++)
		{
		pbody->GetPartSetMaterial(ibset, &fMtrl, &pmtrl, &pcmtl);
		if (fMtrl)
			_prgpo[ibset] = pmtrl;
		else
			_prgpo[ibset] = pcmtl;
		_prgpo[ibset]->AddRef();
		}
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Set a costume onto a BODY.  The flag fAllowDifferentShape should usually
	be fFalse; it should be fTrue in the rare cases where it is appropriate
	to apply a costume with one number of body part sets to a BODY with
	a (possibly) different number of body part sets.  In that case, the
	smaller number of materials are copied.  For example, when changing
	the number of characters in a 3-D Text object, the code grabs the
	current costume, resizes the TDT and BODY, sets the TDT's BODY to the
	default costume, then restores the old costume to the BODY as much as
	is appropriate.
***************************************************************************/
void COST::Set(PBODY pbody, bool fAllowDifferentShape)
{
	AssertThis(0);
	AssertPo(pbody, 0);

	long ibset;
	BASE *po;
	long cbset;

	if (fAllowDifferentShape) // see comment in function header
		{
		cbset = LwMin(_cbset, pbody->Cbset());
		}	
	else
		{
		Assert(_cbset == pbody->Cbset(), "different BODY shapes!");
		cbset = _cbset;
		}

	for (ibset = 0; ibset < cbset; ibset++)
		{
		po = _prgpo[ibset];
		AssertPo(po, 0);
		if (po->FIs(kclsMTRL))
			pbody->SetPartSetMtrl(ibset, (PMTRL)_prgpo[ibset]);
		else
			pbody->SetPartSetCmtl((PCMTL)_prgpo[ibset]);
		}
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the COST.
***************************************************************************/
void COST::AssertValid(ulong grf)
{
	long ibset;
	BASE *po;

	if (pvNil != _prgpo)
		{
		AssertPvCb(_prgpo, LwMul(size(BASE *), _cbset));
		for (ibset = 0; ibset < _cbset; ibset++)
			{
			po = _prgpo[ibset];
			if (po->FIs(kclsMTRL))
				AssertPo((PMTRL)po, 0);
			else
				AssertPo((PCMTL)po, 0);
			}
		}
}


/***************************************************************************
	Mark memory used by the COST
***************************************************************************/
void COST::MarkMem(void)
{
	AssertThis(0);

	long ibset;

	if (pvNil != _prgpo)
		{
		MarkPv(_prgpo);
		for (ibset = 0; ibset < _cbset; ibset++)
			MarkMemObj(_prgpo[ibset]);
		}
}

#endif DEBUG
