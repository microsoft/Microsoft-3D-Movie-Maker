/***************************************************************************

	tagman.cpp: Tag Manager class (TAGM)

	Primary Author: manders
	Review Status: REVIEWED - any changes to this file must be reviewed!

	It is important to keep in mind that there are two layers of caching
	going on in TAGM: Caching content from the CD (or other slow source)
	to the local hard disk, and caching resources in RAM using chunky
	resources (the CRF and CRM classes).

	For each source, TAGM maintains (in an SFS) a CRM (Chunky Resource
	Manager) of all the content	files on the source and a CRF (Chunky
	Resource File), which is a single file on the HD which can be used
	for faster access to the source.  Both CRFs and CRMs can cache
	resources in RAM.  Since Socrates copies *all* content from the CD
	to the cache file, the CRM is told not to cache its resources in
	RAM.  However, if the source is actually on the HD, TAGM notices and
	doesn't copy any content to a cache file, since that would be a waste
	of time.  Instead, the content is read directly from the CRM.  In this
	case, TAGM does tell the CRM to cache its resources in RAM.

	Source names: every source has a long and short name.  This is so we
	can use long names for the source directory on the HD (e.g., "3D Movie
	Maker"), and short names for the source directory on the CD 
	("3DMOVIE").  We have to support short names because CD-ROMs currently
	do not allow long filenames.  So everywhere that we look for the 
	source directory, we accept either the long or short name.  
	_pgstSource keeps track of these names.  Rather than have one GST for
	short names and one for long names, each string in _pgstSource is the
	"merged name", which is the long name followed by a slash character 
	(/) followed by the short name, e.g., "3D Movie Maker/3DMOVIE".  The
	SplitString() function splits the merged name into the long and
	short names.

***************************************************************************/
#include "soc.h"
ASSERTNAME

RTCLASS(TAGM)

const BOM kbomSid = 0xc0000000;

// Source File Structure...keeps track of known sources and caches
struct SFS
	{
public:
	long sid;			// ID for this source
	FNI fniHD;			// FNI of the HD directory
	FNI fniCD;			// FNI of the CD directory
	PCRM pcrmSource;	// CRM of files on the CD (or possibly HD)
	bool tContentOnHD;  // Is the content on the HD or CD?

public:
	void Clear(void)	// Zeros out an SFS
		{
		sid = ksidInvalid;
		fniHD.SetNil();
		fniCD.SetNil();
		pcrmSource = pvNil;
		tContentOnHD = tMaybe;
		}
	};


/***************************************************************************
	Initialize the tag manager
***************************************************************************/
PTAGM TAGM::PtagmNew(PFNI pfniHDRoot, PFNINSCD pfninscd, long cbCache)
{
	AssertPo(pfniHDRoot, ffniDir);
	Assert(pvNil != pfninscd, "bad pfninscd");
	AssertIn(cbCache, 0, kcbMax);

	PTAGM ptagm;

	ptagm = NewObj TAGM;
	if (pvNil == ptagm)
		goto LFail;	

	ptagm->_fniHDRoot = *pfniHDRoot;
	ptagm->_cbCache = cbCache;
	ptagm->_pfninscd = pfninscd;

	ptagm->_pglsfs = GL::PglNew(size(SFS));
	if (pvNil == ptagm->_pglsfs)
		goto LFail;	

	ptagm->_pgstSource = GST::PgstNew(size(long)); // extra data is sid
	if (pvNil == ptagm->_pgstSource)
		goto LFail;

	AssertPo(ptagm, 0);
	return ptagm;
LFail:
	ReleasePpo(&ptagm);
	return pvNil;
}


/***************************************************************************
	Tag Manager destructor
***************************************************************************/
TAGM::~TAGM(void)
{
	AssertBaseThis(0);

	long isfs;
	SFS sfs;

	if (pvNil != _pglsfs)
		{
		for (isfs = 0; isfs < _pglsfs->IvMac(); isfs++)
			{
			_pglsfs->Get(isfs, &sfs);
			ReleasePpo(&sfs.pcrmSource);
			}
		ReleasePpo(&_pglsfs);
		}
	ReleasePpo(&_pgstSource);
}


/***************************************************************************
	Split a merged string into its long and short components
***************************************************************************/
void TAGM::SplitString(PSTN pstnMerged, PSTN pstnLong, PSTN pstnShort)
{
	AssertPo(pstnMerged, 0);
	AssertVarMem(pstnLong);
	AssertVarMem(pstnShort);

	achar *pchStart = pstnMerged->Psz();
	achar *pch;

	for (pch = pchStart; *pch != chNil; pch++)
		{
		if (*pch == ChLit('/'))
			{
			pstnLong->SetRgch(pchStart, (pch - pchStart));
			pstnShort->SetSz(pch + 1);
			return;
			}
		}
	// no slash, so set both pstnLong and pstnShort to pstnMerged
	*pstnLong = *pstnMerged;
	*pstnShort = *pstnMerged;
}


/***************************************************************************
	Return source title string table so it can be embedded in documents
***************************************************************************/
PGST TAGM::PgstSource(void)
{
	AssertThis(0);
	return _pgstSource;
}


/***************************************************************************
	If there is an stn for the given sid in _pgstSource, return the
	location of the stn.
***************************************************************************/
bool TAGM::_FFindSid(long sid, long *pistn)
{
	AssertThis(0);
	Assert(sid >= 0, "Invalid sid");
	AssertNilOrVarMem(pistn);

	long istn;
	long sidT;

	for (istn = 0; istn < _pgstSource->IvMac(); istn++)
		{
		_pgstSource->GetExtra(istn, &sidT);
		if (sid == sidT)  // it's already in there
			{
			if (pvNil != pistn)
				*pistn = istn;
			return fTrue;
			}
		}
	TrashVar(pistn);
	return fFalse;
}


/***************************************************************************
	Add source title string table entries to tag manager, if it doesn't
	already know them.
***************************************************************************/
bool TAGM::FMergeGstSource(PGST pgst, short bo, short osk)
{
	AssertThis(0);
	AssertPo(pgst, 0);
	Assert(size(long) == pgst->CbExtra(), "bad pgstSource");

	long istn;
	STN stn;
	long sid;

	for (istn = 0; istn < pgst->IvMac(); istn++)
		{
		pgst->GetStn(istn, &stn);
		pgst->GetExtra(istn, &sid);
		if (kboOther == bo)
			SwapBytesBom(&sid, kbomSid);
		if (!FAddStnSource(&stn, sid))
			return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	Add source title string to tag manager, if it's not already there
***************************************************************************/
bool TAGM::FAddStnSource(PSTN pstn, long sid)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	Assert(sid >= 0, "Invalid sid");
	
	if (_FFindSid(sid))
		return fTrue; // String is already there

	return _pgstSource->FAddStn(pstn, &sid); // Try to add it
}


/***************************************************************************
	Find the sid with the given string as its source name.  pstn can be
	the merged name, the short name, or the long name.
***************************************************************************/
bool TAGM::FGetSid(PSTN pstn, long *psid)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	AssertVarMem(psid);

	long istn;
	STN stnMerged;
	STN stnLong;
	STN stnShort;

	for (istn = 0; istn < _pgstSource->IvMac(); istn++)
		{
		_pgstSource->GetStn(istn, &stnMerged);
		if (stnMerged.FEqualUser(pstn, fstnIgnoreCase))
			{
			_pgstSource->GetExtra(istn, psid);
			return fTrue;
			}
		SplitString(&stnMerged, &stnLong, &stnShort);
		if (stnLong.FEqualUser(pstn, fstnIgnoreCase) ||
			stnShort.FEqualUser(pstn, fstnIgnoreCase))
			{
			_pgstSource->GetExtra(istn, psid);
			return fTrue;
			}
		}
	return fFalse;
}


/***************************************************************************
	Find the string of the source with the given sid	
***************************************************************************/
bool TAGM::_FGetStnMergedOfSid(long sid, PSTN pstn)
{
	AssertThis(0);
	AssertVarMem(pstn);

	long istn;
	long sidT;

	for (istn = 0; istn < _pgstSource->IvMac(); istn++)
		{
		_pgstSource->GetStn(istn, pstn);
		_pgstSource->GetExtra(istn, &sidT);
		if (sid == sidT)
			return fTrue;
		}
	return fFalse;
}


/***************************************************************************
	Find the string of the source with the given sid	
***************************************************************************/
bool TAGM::_FGetStnSplitOfSid(long sid, PSTN pstnLong, PSTN pstnShort)
{
	AssertThis(0);
	AssertVarMem(pstnLong);
	AssertVarMem(pstnShort);

	STN stnMerged;

	if (!_FGetStnMergedOfSid(sid, &stnMerged))
		return fFalse;
	SplitString(&stnMerged, pstnLong, pstnShort);
	return fTrue;
}


/***************************************************************************
	Builds the FNI to the HD files for a given sid
	- If we don't even know the string for the sid, return fFalse
	- If there is no fniHD, set *pfExists to fFalse and return fTrue
	- If we find the fniHD, put it in *pfniHD, set *pfExists to fTrue,
	  and return fTrue
***************************************************************************/
bool TAGM::_FBuildFniHD(long sid, PFNI pfniHD, bool *pfExists)
{
	AssertThis(0);
	Assert(sid >= 0, "Invalid sid");
	AssertVarMem(pfniHD);
	AssertVarMem(pfExists);

	STN stnLong;
	STN stnShort;
	FNI fni;

	*pfExists = fFalse;
	if (!_FGetStnSplitOfSid(sid, &stnLong, &stnShort))
		{
		return fFalse; // can't even determine if fniHD exists or not
		}

	fni = _fniHDRoot;
	if (!fni.FDownDir(&stnLong, ffniMoveToDir) &&
		!fni.FDownDir(&stnShort, ffniMoveToDir))
		{
		return fTrue; // fniHD doesn't exist
		}
	*pfniHD = fni;
	*pfExists = fTrue;
	return fTrue;
}


/***************************************************************************
	See if there are any content files in the directory specified by pfni
***************************************************************************/
bool TAGM::_FDetermineIfContentOnFni(PFNI pfni, bool *pfContentOnFni)
{
	AssertThis(0);
	AssertPo(pfni, ffniDir);
	AssertVarMem(pfContentOnFni);

	FNE fne;
	FTG ftgContent = kftgContent;
	FNI fni;

	if (!fne.FInit(pfni, &ftgContent, 1))
		return fFalse;
	if (fne.FNextFni(&fni))
		*pfContentOnFni = fTrue;
	else
		*pfContentOnFni = fFalse;
	return fTrue;
}


/***************************************************************************
	Returns whether the directory pointed to by pfniCD exists where we
	think it does.  Or, if pstn is non-nil, try to go down from pfniCD
	to pstn.
***************************************************************************/
bool TAGM::_FEnsureFniCD(long sid, FNI *pfniCD, PSTN pstn)
{
	AssertThis(0);
	Assert(sid >= 0, "Invalid sid");
	AssertPo(pfniCD, ffniDir);
	AssertNilOrPo(pstn, 0);

	ERS ersT;
	ERS *pers;
	bool fRet;

	pers = vpers;
	vpers = &ersT;

	#ifdef WIN
		// Block the Windows "There is no disk in the drive" message
		UINT em;
		em = SetErrorMode(SEM_FAILCRITICALERRORS);
	#endif // WIN

	if (pvNil != pstn)
		fRet = pfniCD->FDownDir(pstn, ffniMoveToDir);
	else
		fRet = (pfniCD->TExists() == tYes);

	#ifdef WIN
		SetErrorMode(em); // restore the error mode
	#endif // WIN

	vpers = pers;
	return fRet;
}


/***************************************************************************
	This function verifies that the source (e.g., CD) is where we think it
	is, and searches for it otherwise.  Pass the previously determined FNI
	of the CD directory file in pfniCD.  Or if this is the first time
	looking for this source, pass in any FNI with a FTG of ftgNil.	If it
	can't find the CD directory, it returns fFalse with pfniInfo untouched.
***************************************************************************/
bool TAGM::_FFindFniCD(long sid, PFNI pfniCD, bool *pfFniChanged)
{
	AssertThis(0);
	Assert(sid >= 0, "Invalid sid");
	AssertPo(pfniCD, ffniEmpty | ffniDir); // could be a blank FNI
	AssertVarMem(pfFniChanged);

	FNE fne;
	FNI fni;
	STN stnLong;
	STN stnShort;
	FNI fniCD;

	*pfFniChanged = fFalse;

	// If pfniCD's ftg is not ftgNil, we've opened this source before,
	// so look for it where it was last time.
	if (ftgNil != pfniCD->Ftg())
		{
		if (_FEnsureFniCD(sid, pfniCD))
			{
			// The source is where we thought it was
			return fTrue;
			}
		else
			{
			// With the way the CRM stuff works now, the CRM can't
			// move to another path.  So fail if the CD isn't exactly
			// where it was before.
			return fFalse;
			}
		}

	if (!_FGetStnSplitOfSid(sid, &stnLong, &stnShort))
		return fFalse;
	// The source has moved/disappeared, or we're opening it for the first
	// time, so search all drives for it.
	if (!fne.FInit(pvNil, pvNil, 0))
		return fFalse;
	while (fne.FNextFni(&fni))
		{
		if (fni.Grfvk() & fvkFloppy) // don't buzz floppies
			continue;
		fniCD = fni;
		if (!_FEnsureFniCD(sid, &fniCD, &stnShort) &&
			!_FEnsureFniCD(sid, &fniCD, &stnLong))
			{
			continue;
			}
		*pfFniChanged = fTrue;
		*pfniCD = fniCD;
		return fTrue;
		}
	// Couldn't find the source
	return fFalse;
}


/***************************************************************************
	Calls the client-supplied callback, which should tell the user that
	the source named stn cannot be found.  Returns fTrue if the user wants
	to retry, else fFalse.	
***************************************************************************/
bool TAGM::_FRetry(long sid)
{
	AssertThis(0);
	Assert(sid >= 0, "Invalid sid");

	STN stnLong;
	STN stnShort;

	if (!_FGetStnSplitOfSid(sid, &stnLong, &stnShort))
		{
		Bug("_pgstSource has no string for this sid!");
		return fFalse;
		}
	return _pfninscd(&stnLong);
}


/***************************************************************************
	Builds the CRM for the given sid's source.  pfniDir tells where the
	content files are.	
***************************************************************************/
PCRM TAGM::_PcrmSourceNew(long sid, PFNI pfniDir)
{
	AssertThis(0);
	Assert(sid >= 0, "Invalid sid");
	AssertPo(pfniDir, ffniDir);

	STN stn;
	FNI fni;
	PCRM pcrmSource = pvNil;
	FNE fne;
	FTG ftgChk = kftgContent;
	PCFL pcfl = pvNil;

	pcrmSource = CRM::PcrmNew(0);
	if (pvNil == pcrmSource)
		goto LFail;

	// Add all chunky files in content directory to pcrmSource
	if (!fne.FInit(pfniDir, &ftgChk, 1))
		goto LFail;
	while (fne.FNextFni(&fni))
		{
		pcfl = CFL::PcflOpen(&fni, fcflNil);
		if (pvNil == pcfl)
			goto LFail;
		if (!pcrmSource->FAddCfl(pcfl, _cbCache))
			goto LFail;
		ReleasePpo(&pcfl);
		}
	return pcrmSource;
LFail:
	ReleasePpo(&pcfl);
	ReleasePpo(&pcrmSource);
	return pvNil;
}


/***************************************************************************
	Returns the source CRM for the given sid, creating (and remembering) a
	new one if there isn't one already.  It verifies that the CD is still
	in the drive, unless fDontHitCD is fTrue.
***************************************************************************/
PCRM TAGM::_PcrmSourceGet(long sid, bool fDontHitCD)
{
	AssertThis(0);
	Assert(sid >= 0, "Invalid sid");

	long isfs;
	SFS sfs;
	bool fContentOnFni;
	bool fFniChanged;
	bool fExists;

	for (isfs = 0; isfs < _pglsfs->IvMac(); isfs++)
		{
		_pglsfs->Get(isfs, &sfs);
		if (sid == sfs.sid)
			goto LSetupSfs;
		}
	// SFS for this sid doesn't exist in _pglsfs, so make one
	sfs.Clear();
	sfs.sid = sid;
	if (!_pglsfs->FAdd(&sfs, &isfs))
		return pvNil;
LSetupSfs:
	if (sfs.tContentOnHD == tMaybe)
		{
		if (sfs.fniHD.Ftg() == ftgNil)
			{
			if (!_FBuildFniHD(sid, &sfs.fniHD, &fExists))
				return pvNil;
			if (!fExists)
				sfs.tContentOnHD = tNo;
			_pglsfs->Put(isfs, &sfs);
			}
		if (sfs.tContentOnHD == tMaybe)
			{
			if (!_FDetermineIfContentOnFni(&sfs.fniHD, &fContentOnFni))
				return pvNil;
			sfs.tContentOnHD = (fContentOnFni ? tYes : tNo);
			_pglsfs->Put(isfs, &sfs);
			}
		}
	if (tYes == sfs.tContentOnHD)
		{
		if (pvNil == sfs.pcrmSource)
			{
			sfs.pcrmSource = _PcrmSourceNew(sid, &sfs.fniHD);
			if (pvNil == sfs.pcrmSource)
				return pvNil;
			_pglsfs->Put(isfs, &sfs);
			}
		return sfs.pcrmSource;
		}

	// Else content is not on HD, so look at CD
	if (!fDontHitCD)
		{
		// Verify that CD is where we thought it was, or find
		// it if we haven't found it before
		while (!_FFindFniCD(sid, &sfs.fniCD, &fFniChanged))
			{
			// Ask user to insert the CD
			if (!_FRetry(sid))
				return pvNil;
			}
		if (fFniChanged)
			{
			Assert(sfs.pcrmSource == pvNil, 
				"fniCD can't change once pcrm is opened!");
			_pglsfs->Put(isfs, &sfs); // update sfs.fniCD
			}
		}
	if (pvNil == sfs.pcrmSource)
		{
		if (fDontHitCD)
			{
			Bug("should have valid pcrmSource!");
			}
		else
			{
			sfs.pcrmSource = _PcrmSourceNew(sid, &sfs.fniCD);
			if (pvNil == sfs.pcrmSource)
				return pvNil;
			_pglsfs->Put(isfs, &sfs);
			}
		}
	return sfs.pcrmSource;
}


/***************************************************************************
	Determines whether source is on HD (if it is, don't cache its stuff to
	HD!)  Note that the function return value is whether the function
	completed without error, not whether the source is on HD.
***************************************************************************/
bool TAGM::_FDetermineIfSourceHD(long sid, bool *pfIsOnHD)
{
	AssertThis(0);
	Assert(sid >= 0, "Invalid sid");
	AssertVarMem(pfIsOnHD);

	long isfs;
	SFS sfs;
	bool fContentOnFni;
	bool fExists;

	for (isfs = 0; isfs < _pglsfs->IvMac(); isfs++)
		{
		_pglsfs->Get(isfs, &sfs);
		if (sid == sfs.sid)
			goto LSetupSfs;
		}
	// SFS for this sid doesn't exist in _pglsfs, so make one
	sfs.Clear();
	sfs.sid = sid;
	if (!_pglsfs->FAdd(&sfs, &isfs))
		return fFalse;
LSetupSfs:
	if (sfs.tContentOnHD == tMaybe)
		{
		if (sfs.fniHD.Ftg() == ftgNil)
			{
			if (!_FBuildFniHD(sid, &sfs.fniHD, &fExists))
				return fFalse;
			if (!fExists)
				sfs.tContentOnHD = tNo;
			_pglsfs->Put(isfs, &sfs);
			}
		if (sfs.tContentOnHD == tMaybe)
			{
			if (!_FDetermineIfContentOnFni(&sfs.fniHD, &fContentOnFni))
				return fFalse;
			sfs.tContentOnHD = (fContentOnFni ? tYes : tNo);
			_pglsfs->Put(isfs, &sfs);
			}
		}
	*pfIsOnHD = (sfs.tContentOnHD == tYes);
	return fTrue;
}


/***************************************************************************
	Get the FNI for the HD directory
***************************************************************************/
bool TAGM::_FGetFniHD(long sid, PFNI pfniHD)
{
	AssertThis(0);
	AssertVarMem(pfniHD);

	long isfs;
	SFS sfs;
	bool fExists;

	for (isfs = 0; isfs < _pglsfs->IvMac(); isfs++)
		{
		_pglsfs->Get(isfs, &sfs);
		if (sid == sfs.sid)
			goto LSetupSFS;
		}
	// SFS for this sid doesn't exist in _pglsfs, so make one
	sfs.Clear();
	sfs.sid = sid;
	if (!_pglsfs->FAdd(&sfs, &isfs))
		return fFalse;
LSetupSFS:
	if (sfs.fniHD.Ftg() == ftgNil)
		{
		if (!_FBuildFniHD(sid, &sfs.fniHD, &fExists) || !fExists)
			return fFalse;
		}
	_pglsfs->Put(isfs, &sfs);
	*pfniHD = sfs.fniHD;
	return fTrue;
}


/***************************************************************************
	Get the FNI for the CD directory
***************************************************************************/
bool TAGM::_FGetFniCD(long sid, PFNI pfniCD, bool fAskForCD)
{
	AssertThis(0);
	AssertVarMem(pfniCD);

	long isfs;
	SFS sfs;
	bool fFniChanged;

	for (isfs = 0; isfs < _pglsfs->IvMac(); isfs++)
		{
		_pglsfs->Get(isfs, &sfs);
		if (sid == sfs.sid)
			goto LSetupSFS;
		}
	// SFS for this sid doesn't exist in _pglsfs, so make one
	sfs.Clear();
	sfs.sid = sid;
	if (!_pglsfs->FAdd(&sfs, &isfs))
		return fFalse;
LSetupSFS:
	while (!_FFindFniCD(sid, &sfs.fniCD, &fFniChanged))
		{
		if (!fAskForCD)
			return fFalse;
		if (!_FRetry(sid))
			return fFalse;
		}
	if (fFniChanged)
		_pglsfs->Put(isfs, &sfs);
	*pfniCD = sfs.fniCD;
	return fTrue;
}


/***************************************************************************
	Finds the file with name pstn on the HD or CD.
***************************************************************************/
bool TAGM::FFindFile(long sid, PSTN pstn, PFNI pfni, bool fAskForCD)
{
	AssertThis(0);
	Assert(sid >= 0, "Invalid sid");
	AssertPo(pstn, 0);
	AssertVarMem(pfni);

	FTG ftg;

	if (!pfni->FBuildFromPath(pstn))
		return fFalse;
	ftg = pfni->Ftg();

	// First, look on the HD
	if (!_FGetFniHD(sid, pfni))
		return fFalse;
	if (pfni->FSetLeaf(pstn, ftg) && tYes == pfni->TExists())
		return fTrue;

	// Now look on the CD, asking for it if fAskForCD
	if (!_FGetFniCD(sid, pfni, fAskForCD))
		return fFalse;
	if (pfni->FSetLeaf(pstn, ftg) && tYes == pfni->TExists())
		return fTrue;

	pfni->SetNil();
	return fFalse; // file not found
}


/***************************************************************************
	Build a tag for a child of another tag.  Note that this may hit the 
	CD if _PcrmSourceGet has not yet been called for ptagPar->sid.
***************************************************************************/
bool TAGM::FBuildChildTag(PTAG ptagPar, CHID chid, CTG ctgChild, 
	PTAG ptagChild)
{
	AssertThis(0);
	AssertVarMem(ptagPar);
	Assert(ptagPar->sid >= 0, "Invalid sid");
	AssertVarMem(ptagChild);

	PCRM pcrmSource;
	PCRF pcrfSource;
	KID kid;

	TrashVar(ptagChild);

	if (ksidUseCrf == ptagPar->sid)
		{
		AssertPo(ptagPar->pcrf, 0);
		if (!ptagPar->pcrf->Pcfl()->FGetKidChidCtg(ptagPar->ctg, 
			ptagPar->cno, chid, ctgChild, &kid))
			{
			return fFalse; // child chunk not found
			}
		ptagChild->sid = ksidUseCrf;
		ptagChild->pcrf = ptagPar->pcrf;
		ptagPar->pcrf->AddRef();
		ptagChild->ctg = kid.cki.ctg;
		ptagChild->cno = kid.cki.cno;
		return fTrue;
		}
	pcrmSource = _PcrmSourceGet(ptagPar->sid);
	if (pvNil == pcrmSource)
		return fFalse;
	pcrfSource = pcrmSource->PcrfFindChunk(ptagPar->ctg, ptagPar->cno);
	if (pvNil == pcrfSource)
		return fFalse; // parent chunk not found
	if (!pcrfSource->Pcfl()->FGetKidChidCtg(ptagPar->ctg, ptagPar->cno, chid, 
		ctgChild, &kid))
		{
		return fFalse; // child chunk not found
		}
	ptagChild->sid = ptagPar->sid;
	ptagChild->ctg = kid.cki.ctg;
	ptagChild->cno = kid.cki.cno;
	
	return fTrue;	
}


/***************************************************************************
	Put specified chunk in cache file, if it's not there yet
***************************************************************************/
bool TAGM::FCacheTagToHD(PTAG ptag, bool fCacheChildChunks)
{
	AssertThis(0);
	AssertVarMem(ptag);
	Assert(ptag->sid >= 0, "Invalid sid");

	PCRM pcrmSource;
	PCRF pcrfSource;
	bool fSourceIsOnHD;
	PCFL pcfl;

	if (ksidUseCrf == ptag->sid)
		return fTrue;

	// Do nothing if the source itself is already on HD
	if (!_FDetermineIfSourceHD(ptag->sid, &fSourceIsOnHD))
		goto LFail;
	if (fSourceIsOnHD)
		return fTrue;

	pcrmSource = _PcrmSourceGet(ptag->sid);
	if (pvNil == pcrmSource)
		goto LFail;
	pcrfSource = pcrmSource->PcrfFindChunk(ptag->ctg, ptag->cno);
	if (pvNil == pcrfSource)
		goto LFail;	// chunk not found

	pcfl = pcrfSource->Pcfl();
	if (fCacheChildChunks)
		{
		// Cache the chunk specified by the tag, and all its child
		// chunks.  
		CGE cge;
		KID kid;
		ulong grfcgeIn = 0;
		ulong grfcgeOut;

		cge.Init(pcfl, ptag->ctg, ptag->cno);
		while (cge.FNextKid(&kid, pvNil, &grfcgeOut, grfcgeIn))
			{
			if (grfcgeOut & fcgePre)
				{
				if (!pcfl->FEnsureOnExtra(kid.cki.ctg, kid.cki.cno))
					goto LFail;
				}
			}

		}
	else
		{
		// Just cache the chunk specified by the tag
		if (!pcfl->FEnsureOnExtra(ptag->ctg, ptag->cno))
			goto LFail;
		}
	return fTrue;
LFail:
	PushErc(ercSocCantCacheTag);
	return fFalse;
}


/***************************************************************************
	Resolve the TAG to a BACO.  Only use HD cache files, unless fUseCD is
	fTrue.
***************************************************************************/
PBACO TAGM::PbacoFetch(PTAG ptag, PFNRPO pfnrpo, bool fUseCD)
{
	AssertThis(0);
	AssertVarMem(ptag);
	Assert(ptag->sid >= 0, "Invalid sid");
	Assert(pvNil != pfnrpo, "bad rpo");

	PBACO pbaco = pvNil;
	PCRM pcrmSource;

	if (ptag->sid == ksidUseCrf)
		{
		// Tag knows pcrf, so just read from there.  Nothing we can do if
		// it's not there.
		AssertPo(ptag->pcrf, 0);
		return ptag->pcrf->PbacoFetch(ptag->ctg, ptag->cno, pfnrpo);
		}

	// fTrue parameter ensures that _PcrmSourceGet won't hit the CD
	pcrmSource = _PcrmSourceGet(ptag->sid, fTrue);
	if (pvNil == pcrmSource)
		return pvNil;

	pbaco = pcrmSource->PbacoFetch(ptag->ctg, ptag->cno, pfnrpo);
	return pbaco;
}


/***************************************************************************
	Clear the cache for source sid.  If sid is sidNil, clear all caches.
***************************************************************************/
void TAGM::ClearCache(long sid, ulong grftagm)
{
	AssertThis(0);
	Assert(sid >= 0, "Invalid sid");

	long isfs, isfsMac;
	long cbMax;

	vpappb->BeginLongOp();

	isfsMac = _pglsfs->IvMac();
	for (isfs = 0; isfs < isfsMac; isfs++)
		{
		long icrf, icrfMac;
		SFS sfs;
		PCRM pcrmSource;

		_pglsfs->Get(isfs, &sfs);
		if ((sid != sidNil && sfs.sid != sid) || sfs.pcrmSource == pvNil)
			continue;
		if (grftagm & ftagmFile)
			{
			// The following line may seem silly, since we already have
			// sfs.pcrmSource.  But it ensures that the crm's CD is inserted, 
			// since the FReopen() will need the CD to be in.
			pcrmSource = _PcrmSourceGet(sfs.sid);
			if (pvNil == pcrmSource)
				continue;
			}
		else
			{
			// Just doing a memory purge, so sfs.pcrmSource is valid.
			pcrmSource = sfs.pcrmSource;
			}
		icrfMac = pcrmSource->Ccrf();
		for (icrf = 0; icrf < icrfMac; icrf++)
			{
			PCRF pcrf;

			pcrf = pcrmSource->PcrfGet(icrf);
			AssertPo(pcrf->Pcfl(), 0);
			if (pcrf->Pcfl() != pvNil)
				{
				if (grftagm & ftagmFile)
					{
					// Clear the HD cache by reopening the file
					pcrf->Pcfl()->FReopen();  // Ignore error
					}
				if (grftagm & ftagmMemory)
					{
					// Clear RAM cache (for BACOs with 0 cactRef) by
					// temporarily setting the CRF's cbMax to 0
					cbMax = pcrf->CbMax();
					pcrf->SetCbMax(0);
					pcrf->SetCbMax(cbMax);
					}
				}
			}
		}

	vpappb->EndLongOp();
}


/***************************************************************************
	Prepares this tag to be used (resolved).  The tag is invalid until you
	call this, *except* you can pass the tag to FCacheTagToHD() before
	calling FOpenTag().  If you FOpenTag() a tag, you must CloseTag() it
	when you're done with it.
***************************************************************************/
bool TAGM::FOpenTag(PTAG ptag, PCRF pcrfDest, PCFL pcflSrc)
{
	AssertVarMem(ptag);
	Assert(ptag->sid >= 0, "Invalid sid");
	AssertPo(pcrfDest, 0);
	AssertNilOrPo(pcflSrc, 0);

	CNO cnoDest;	

	if (ptag->sid != ksidUseCrf)
		return fTrue;
	if (pvNil != pcflSrc && pcrfDest->Pcfl() != pcflSrc)
		{
		if (!pcflSrc->FCopy(ptag->ctg, ptag->cno, pcrfDest->Pcfl(),	
			&cnoDest))
			{
			ptag->pcrf = pvNil;
			return fFalse; // copy failed
			}
		ptag->cno = cnoDest;
		}
	ptag->pcrf = pcrfDest;
	pcrfDest->AddRef();
	return fTrue;
}


/***************************************************************************
	Save tag's data in the given CRF.  If fRedirect, the tag now points
	to the copy in the CRF.
***************************************************************************/
bool TAGM::FSaveTag(PTAG ptag, PCRF pcrf, bool fRedirect)
{
	AssertVarMem(ptag);
	Assert(ptag->sid >= 0, "Invalid sid");
	AssertPo(pcrf, 0);

	CNO cnoDest;
		
	if (ptag->sid != ksidUseCrf)
		return fTrue;

	AssertPo(ptag->pcrf, 0);
		
	if (!ptag->pcrf->Pcfl()->FCopy(ptag->ctg, ptag->cno, pcrf->Pcfl(),
		&cnoDest))
		{
		return fFalse; // copy failed
		}
	
	if (fRedirect)	
		{
		pcrf->AddRef();
		ReleasePpo(&ptag->pcrf);
		ptag->pcrf = pcrf;
		ptag->cno = cnoDest;
		}

	return fTrue;
}	


/***************************************************************************
	Call this for each tag when you're duplicating it.  Increments
	refcount on the tag's CRF.
***************************************************************************/
void TAGM::DupTag(PTAG ptag)
{
	AssertVarMem(ptag);
	Assert(ptag->sid >= 0, "Invalid sid");

	if (ptag->sid == ksidUseCrf)
		{
		AssertPo(ptag->pcrf, 0);
		ptag->pcrf->AddRef();
		}
}


/***************************************************************************
	Close the tag
***************************************************************************/
void TAGM::CloseTag(PTAG ptag)
{
	AssertVarMem(ptag);
	// Client destructors often call CloseTag on an uninitialized tag, so
	// don't Assert on ksidInvalid tags...just ignore them
	Assert(ptag->sid == ksidInvalid || ptag->sid >= 0, "Invalid sid");

	if (ptag->sid == ksidUseCrf)
		{
		AssertPo(ptag->pcrf, 0);
		ReleasePpo(&ptag->pcrf);
		}
}


/***************************************************************************
	Compare two tags.  Tags are sorted first by sid, then CTG, then CNO.
***************************************************************************/
ulong TAGM::FcmpCompareTags(PTAG ptag1, PTAG ptag2)
{
	AssertVarMem(ptag1);
	Assert(ptag1->sid >= 0, "Invalid sid");
	AssertVarMem(ptag2);
	Assert(ptag2->sid >= 0, "Invalid sid");

    if (ptag1->sid < ptag2->sid)
		return fcmpLt;
    if (ptag1->sid > ptag2->sid)
		return fcmpGt;
    if (ptag1->ctg < ptag2->ctg)
		return fcmpLt;
    if (ptag1->ctg > ptag2->ctg)
		return fcmpGt;
    if (ptag1->cno < ptag2->cno)
		return fcmpLt;
    if (ptag1->cno > ptag2->cno)
		return fcmpGt;
	// If both sids are ksidUseCrf, compare CRFs
	if (ptag1->sid == ksidUseCrf) // implies ptag2->sid == ksidUseCrf
		{
	    if (ptag1->pcrf < ptag2->pcrf)
			return fcmpLt;
	    if (ptag1->pcrf > ptag2->pcrf)
			return fcmpGt;
		}
	return fcmpEq;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the TAGM.
***************************************************************************/
void TAGM::AssertValid(ulong grf)
{
	long isfs;
	SFS sfs;

    TAGM_PAR::AssertValid(fobjAllocated);
	AssertPo(_pglsfs, 0);
	AssertPo(_pgstSource, 0);
	Assert(pvNil != _pfninscd, "bad _pfninscd");

	for (isfs = 0; isfs < _pglsfs->IvMac(); isfs++)
		{
		_pglsfs->Get(isfs, &sfs);
		AssertPo(&sfs.fniHD, ffniDir | ffniEmpty);
		AssertPo(&sfs.fniCD, ffniDir | ffniEmpty);
		AssertNilOrPo(sfs.pcrmSource, 0);
		}
}


/***************************************************************************
	Mark memory used by the TAGM.
***************************************************************************/
void TAGM::MarkMem(void)
{
	AssertThis(0);

	long isfs;
	SFS sfs;

	TAGM_PAR::MarkMem();
	MarkMemObj(_pglsfs);
	MarkMemObj(_pgstSource);
	for (isfs = 0; isfs < _pglsfs->IvMac(); isfs++)
		{
		_pglsfs->Get(isfs, &sfs);
		MarkMemObj(sfs.pcrmSource);
		}
}
#endif // DEBUG


#ifdef DEBUG
/***************************************************************************
	Mark memory used by the TAG.
***************************************************************************/
void TAG::MarkMem(void)
{
	if (sid == ksidUseCrf)
		{
		AssertPo(pcrf, 0);
		MarkMemObj(pcrf);
		}
}
#endif // DEBUG
