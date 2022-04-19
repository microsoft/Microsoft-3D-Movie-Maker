/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	File name management.

***************************************************************************/
#include "util.h"
#include <folders.h>
ASSERTNAME


// This is the FTG to use for temp files - clients may set this to whatever
// they want.
FTG vftgTemp = kftgTemp;


/***************************************************************************
	Implementation notes:  The _fss is a standard FSSpec record and is
	always filled in via a call to FSMakeFSSpec.  If the fni is a directory,
	_ftg will be kftgDir.  For files, _lwDir is the same as _fss.parID.
	For directories, _lwDir is the directory id for the directory described
	by the fni (while _fss.parID is the id of its parent).
***************************************************************************/


typedef StandardFileReply SFR;
priv bool _FFssDir(FSS *pfss, long *plwDir);

RTCLASS(FNI)
RTCLASS(FNE)


/***************************************************************************
	Sets the fni to nil values.
***************************************************************************/
void FNI::SetNil(void)
{
	_ftg = ftgNil;
	_lwDir = 0;
	ClearPb(&_fss, size(_fss));
	AssertThis(ffniEmpty);
}


/***************************************************************************
	Constructor for fni class.
***************************************************************************/
FNI::FNI(void)
{
	SetNil();
}


/***************************************************************************
	Get an fni (for opening) from the user.
***************************************************************************/
bool FNI::FGetOpen(FTG *prgftg, short cftg)
{
	AssertThis(0);
	AssertNilOrVarMem(prgftg);

	SFR sfr;

	StandardGetFile(pvNil, cftg <= 0 ? -1 : cftg, (ulong *)prgftg, &sfr);
	if (sfr.sfGood)
		{
		_fss = sfr.sfFile;
		_ftg = sfr.sfType;
		_lwDir = _fss.parID;
		AssertThis(ffniFile);
		}
	else
		SetNil();

	return sfr.sfGood;
}


/***************************************************************************
	Get an fni (for saving) from the user.
***************************************************************************/
bool FNI::FGetSave(FTG ftg, PST pstPrompt, PST pstDefault)
{
	AssertThis(0);
	AssertNilOrSt(pstPrompt);
	AssertNilOrSt(pstDefault);

	SFR sfr;

	StandardPutFile((byte *)pstPrompt, (byte *)pstDefault, &sfr);
	if (sfr.sfGood)
		{
		_fss = sfr.sfFile;
		_ftg = ftg;
		_lwDir = _fss.parID;
		AssertThis(ffniFile);
		}
	else
		SetNil();

	return sfr.sfGood;
}


/***************************************************************************
	Get a unique filename in the given directory.
***************************************************************************/
bool FNI::FGetUnique(FTG ftg)
{
	AssertThis(ffniFile | ffniDir);
	static long _dlw = 0;
	long lw;
	short cact;
	short err;
	STN stn;
	long lwDir = _lwDir;
	short swVol = _fss.vRefNum;

	Assert(ftg != kftgDir && ftg != ftgNil, "bad ftg");
	lw = TsSystemCurrent() + ++_dlw;
	for (cact = 0; cact < 20; cact++, lw += ++_dlw)
		{
		AssertDo(stn.FFormatSz(PszLit("~TempFile#%08x"), lw), 0);
		err = FSMakeFSSpec(swVol, lwDir, &stn, &_fss);
		if (err == fnfErr)
			{
			_ftg = ftg;
			_lwDir = _fss.parID;
			AssertThis(ffniFile);
			return fTrue;
			}
		}
	SetNil();
	PushErc(ercFniGeneral);
	return fFalse;
}


/***************************************************************************
	Get a temporary fni.
***************************************************************************/
bool FNI::FGetTemp(void)
{
	AssertThis(0);
	static short _swVolTemp = 0;
	static long _lwDirTemp = -1;

	// This is so we only call FindFolder once.
	if (_swVolTemp == 0 && _lwDirTemp == -1 &&
			FindFolder(0, kTemporaryFolderType, kCreateFolder,
				&_swVolTemp, &_lwDirTemp) != noErr)
		{
		_swVolTemp = 0;
		_lwDirTemp = 0;
		}

	if (FSMakeFSSpec(_swVolTemp, _lwDirTemp, pvNil, &_fss) != noErr)
		{
		SetNil();
		PushErc(ercFniGeneral);
		return fFalse;
		}

	_ftg = kftgDir;
	_lwDir = _lwDirTemp;
	return FGetUnique(vftgTemp);
}


/***************************************************************************
	Return the file type of the fni.
***************************************************************************/
FTG FNI::Ftg(void)
{
	AssertThis(0);
	return _ftg;
}


/***************************************************************************
	Return the volume kind for the given fni.
***************************************************************************/
ulong FNI::Grfvk(void)
{
	AssertThis(0);
	ulong grfvk = fvkNil;

	//REVIEW shonk: Mac Grfvk: implement
	RawRtn();
	return grfvk;
}


/***************************************************************************
	Set the leaf for the fni.
***************************************************************************/
bool FNI::FSetLeaf(PSTZ pstz, FTG ftg)
{
	AssertThis(ffniFile | ffniDir);
	return FBuild(_fss.vRefNum, _lwDir, pstz, ftg);
}


/***************************************************************************
	Set the leaf for the fni.
***************************************************************************/
bool FNI::FBuild(long lwVol, long lwDir, PSTZ pstz, FTG ftg)
{
	AssertNilOrStz(pstz);
	short err;
	FSS fss;

	Assert(FPure(ftg == kftgDir) == FPure(pstz == pvNil || CchStz(pstz) == 0),
		"pstz doesn't match ftg");
	if (ftg == kftgDir)
		pstz = pvNil;

	err = FSMakeFSSpec((short)lwVol, lwDir, (byte *)pstz, &fss);
	if (ftg == kftgDir)
		{
		//a directory (it had better exist)
		if (noErr != err)
			goto LFail;
		_lwDir = lwDir;
		}
	else
		{
		//not supposed to be a directory - so make sure it isn't
		if (fnfErr != err)
			{
			if (noErr != err)
				goto LFail;
			if (_FFssDir(&fss, pvNil))
				goto LFail;
			}
		_lwDir = fss.parID;
		}
	_fss = fss;
	_ftg = ftg;
	AssertThis(ffniFile | ffniDir);
	return fTrue;

LFail:
	SetNil();
	PushErc(ercFniGeneral);
	return fFalse;
}


/***************************************************************************
	Set the FTG for the fni.
***************************************************************************/
bool FNI::FChangeFtg(FTG ftg)
{
	AssertThis(ffniFile);
	Assert(ftg != ftgNil && ftg != kftgDir, "Bad FTG");

	_ftg = ftg;
	return fTrue;
}


/***************************************************************************
	Get the leaf name for the fni.
***************************************************************************/
void FNI::GetLeaf(PSTZ pstz)
{
	AssertThis(0);
	AssertMaxStz(pstz);

	if (_ftg == kftgDir)
		SetStzNil(pstz);
	else
		CopyStStz((achar *)_fss.name, pstz);
}


/***************************************************************************
	Get a string representing the path of the fni.
***************************************************************************/
void FNI::GetStzPath(PSTZ pstz)
{
	AssertThis(0);
	AssertMaxStz(pstz);
	RawRtn(); //REVIEW shonk: Mac GetStzPath: implement for real
	CopyStStz((achar *)_fss.name, pstz);
}


/***************************************************************************
	Determines if the file/directory exists.  Returns tMaybe on error or
	if the fni type (file or dir) doesn't match the disk object of the
	same name or if the file/dir is invisible or is an alias.  Pushes an
	erc if it returns tMaybe.
***************************************************************************/
bool FNI::TExists(void)
{
	AssertThis(ffniFile | ffniDir);
    CInfoPBRec iob;
	short err;
	achar st[kcbMaxSt];

	ClearPb(&iob, size(iob));
	CopySt((achar *)_fss.name, st);
	iob.hFileInfo.ioNamePtr = (byte *)st;
	iob.hFileInfo.ioVRefNum = _fss.vRefNum;
	iob.hFileInfo.ioDirID = _fss.parID;
	if ((err = PBGetCatInfo(&iob, fFalse)) != noErr)
		{
		if (err != fnfErr)
			{
			PushErc(ercFniGeneral);
			return tMaybe;
			}
		return tNo;
		}
	if ((_ftg == kftgDir) != FPure(iob.hFileInfo.ioFlAttrib & 0x0010))
		{
		PushErc(ercFniMismatch);
		return tMaybe;
		}
	if (iob.hFileInfo.ioFlFndrInfo.fdFlags & (kIsInvisible | kIsAlias))
		{
		PushErc(ercFniHidden);
		return tMaybe;
		}
	return tYes;
}


/***************************************************************************
	Delete the file.
***************************************************************************/
bool FNI::FDelete(void)
{
	AssertThis(ffniFile);
	if (FSpDelete(&_fss) == noErr)
		return fTrue;
	PushErc(ercFniDelete);
	return fFalse;
}


/***************************************************************************
	Rename the file as indicated by *pfni.  The directories must match.
***************************************************************************/
bool FNI::FRename(FNI *pfni)
{
	AssertThis(ffniFile);
	AssertPo(pfni, ffniFile);
	Assert(_fss.vRefNum == pfni->_fss.vRefNum &&
		_fss.parID == pfni->_fss.parID, "directory change");
	Assert(_ftg == pfni->_ftg, "ftg's don't match");

	if (FSpRename(&_fss, pfni->_fss.name) == noErr)
		return fTrue;
	PushErc(ercFniRename);
	return fFalse;
}


/***************************************************************************
	Compare two fni's for equality.  Doesn't consider the ftg's.
***************************************************************************/
bool FNI::FEqual(FNI *pfni)
{
	//NOTE: see IM:Text, pg 5-17.  It's not documented whether the comparison
	//should be case sensitive and/or diacritical sensitive.  Experimenting
	//with the US finder indicates that we should use case insensitive,
	//diacritical sensitive comparison.
	AssertThis(ffniFile | ffniDir);
	AssertPo(pfni, ffniFile | ffniDir);

	return pfni->_fss.vRefNum == _fss.vRefNum && pfni->_fss.parID == _fss.parID &&
		EqualString(pfni->_fss.name, _fss.name, fFalse, fTrue);
}


/***************************************************************************
	Return whether the fni refers to a directory.
***************************************************************************/
bool FNI::FDir(void)
{
	AssertThis(0);
	return _ftg == kftgDir;
}


/***************************************************************************
	Return whether the directory portions of the fni's are the same.
***************************************************************************/
bool FNI::FSameDir(FNI *pfni)
{
	AssertThis(ffniDir | ffniFile);
	AssertPo(pfni, ffniDir | ffniFile);

	return pfni->_fss.vRefNum == _fss.vRefNum &&
		pfni->_lwDir == _lwDir;
}


/***************************************************************************
	Determine if the directory pstz in fni exists, optionally creating it
	and/or moving into it.  Specify ffniCreateDir to create it if it
	doesn't exist.  Specify ffniMoveTo to make the fni refer to it.
	If this fails, the fni is untouched.
***************************************************************************/
bool FNI::FDownDir(PSTZ pstz, ulong grffni)
{
	AssertThis(ffniDir);
	AssertStz(pstz);
	FSS fss;
	long lwDir;
	short err;

	//make the fss
	err = FSMakeFSSpec(_fss.vRefNum, _lwDir, (byte *)pstz, &fss);
	if (noErr == err)
		{
		//exists, make sure it is a directory and get the directory id
		if (!_FFssDir(&fss, &lwDir))
			{
			PushErc(ercFniMismatch);
			return fFalse;
			}
		}
	else
		{
		if (fnfErr != err)
			{
			PushErc(ercFniGeneral);
			return fFalse;
			}

		//doesn't exist
		if (!(grffni & ffniCreateDir))
			return fFalse;

		// create it
		err = FSpDirCreate(&fss, smSystemScript, &lwDir);
		if (err != noErr)
			{
			PushErc(ercFniDirCreate);
			return fFalse;
			}
		}

	if (grffni & ffniMoveToDir)
		{
		_fss = fss;
		_lwDir = lwDir;
		_ftg = kftgDir;
		}
	return fTrue;
}


/***************************************************************************
	Gets the lowest directory name (if pstz is not nil) and optionally
	moves the fni up a level (if ffniMoveToDir is specified).  If this
	fails, the fni is untouched.
***************************************************************************/
bool FNI::FUpDir(PSTZ pstz, ulong grffni)
{
	AssertThis(ffniDir);
	AssertNilOrMaxStz(pstz);

    CInfoPBRec iob;
	short err;
	FSS fss;

	if (_lwDir == fsRtDirID)
		return fFalse;

	ClearPb(&iob, size(iob));
	iob.dirInfo.ioFDirIndex = -1; //ignore name, look at vol/dir
	iob.dirInfo.ioNamePtr = (byte *)pstz;
	iob.dirInfo.ioVRefNum = _fss.vRefNum;
	iob.dirInfo.ioDrDirID = _lwDir;
	if (PBGetCatInfo(&iob, fFalse) != noErr)
		{
		PushErc(ercFniGeneral);
		return fFalse;
		}
	if (pstz != pvNil)
		SetStzCch(pstz, CchSt(pstz));
	Assert(iob.dirInfo.ioFlAttrib & 0x0010, "not a directory?!");
	if (grffni & ffniMoveToDir)
		{
		err = FSMakeFSSpec(_fss.vRefNum, iob.dirInfo.ioDrParID, pvNil, &fss);
		if (noErr != err)
			{
			PushErc(ercFniGeneral);
			return fFalse;
			}
		_fss = fss;
		_lwDir = iob.dirInfo.ioDrParID;
		_ftg = kftgDir;
		AssertThis(ffniDir);
		}
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert validity of the FNI.
***************************************************************************/
void FNI::AssertValid(ulong grffni)
{
	FNI_PAR::AssertValid(0);
	if (grffni == 0)
		grffni = ffniEmpty | ffniFile | ffniDir;

	if (_ftg == ftgNil)
		{
		Assert(grffni & ffniEmpty, "unexpected nil fni");
		Assert(_fss.vRefNum == 0 && _fss.parID == 0 && _lwDir == 0 &&
			CchSt((achar *)_fss.name) == 0, "bad nil");
		}
	else if (_ftg == kftgDir)
		{
		Assert(grffni & ffniDir, "unexpected dir");
		Assert(_lwDir != _fss.parID, "dir is its own parent?");
		}
	else
		{
		Assert(grffni & ffniFile, "unexpected file");
		Assert(CchSt((achar *)_fss.name) > 0, "no name for file");
		Assert(_lwDir == _fss.parID, "parent not consistent?");
		}
}
#endif //DEBUG


/***************************************************************************
	Low level routine to determine if the given fss points to an existing
	directory.  If so and plwDir is not nil, sets *plwDir to the id of
	the directory.
***************************************************************************/
priv bool _FFssDir(FSS *pfss, long *plwDir)
{
	CInfoPBRec iob;

	//find the entry
	ClearPb(&iob, size(iob));
	iob.hFileInfo.ioNamePtr = (StringPtr)pfss->name;
	iob.hFileInfo.ioVRefNum = pfss->vRefNum;
	iob.hFileInfo.ioDirID = pfss->parID;
	if (PBGetCatInfo(&iob, fFalse) != noErr)
		{
		PushErc(ercFniGeneral);
		goto LFail;
		}

	//entry exists so see if it's a directory
	if (!(iob.hFileInfo.ioFlAttrib & 0x0010))
		{
LFail:
		TrashVar(plwDir);
		return fFalse;
		}

	if (plwDir != pvNil)
		*plwDir = iob.dirInfo.ioDrDirID;
	return fTrue;
}


/***************************************************************************
	Constructor for a File Name Enumerator.
***************************************************************************/
FNE::FNE(void)
{
	AssertBaseThis(0);
	_prgftg = _rgftg;
	_pglfes = pvNil;
	_fInited = fFalse;
	AssertThis(0);
}


/***************************************************************************
	Destructor for an FNE.
***************************************************************************/
FNE::~FNE(void)
{
	AssertBaseThis(0);
	_Free();
}


/***************************************************************************
	Free all the memory associated with the FNE.
***************************************************************************/
void FNE::_Free(void)
{
	if (_prgftg != _rgftg)
		{
		FreePpv((void **)&_prgftg);
		_prgftg = _rgftg;
		}
	ReleasePpo(&_pglfes);
	_fInited = fFalse;
	AssertThis(0);
}


/***************************************************************************
	Initialize the fne to do an enumeration.
***************************************************************************/
bool FNE::FInit(FNI *pfniDir, FTG *prgftg, long cftg, ulong grffne)
{
	AssertThis(0);
	AssertNilOrVarMem(pfniDir);
	AssertIn(cftg, 0, kcbMax);
	AssertPvCb(prgftg, LwMul(cftg, size(FTG)));

	//free the old stuff
	_Free();

	if (0 >= cftg)
		_cftg = 0;
	else
		{
		long cb = LwMul(cftg, size(FTG));

		if (cftg > kcftgFneBase &&
				!FAllocPv((void **)&_prgftg, cb, fmemNil, mprNormal))
			{
			_prgftg = _rgftg;
			PushErc(ercFneGeneral);
			AssertThis(0);
			return fFalse;
			}
		CopyPb(prgftg, _prgftg, cb);
		_cftg = cftg;
		}

	if (pfniDir == pvNil)
		_fesCur.lwVol = 0;
	else
		{
		_fesCur.lwVol = (long)pfniDir->_fss.vRefNum;
		_fesCur.lwDir = pfniDir->_lwDir;
		}
	_fesCur.iv = 0;
	_fRecurse = FPure(grffne & ffneRecurse);
	_fInited = fTrue;
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Get the next FNI in the enumeration.
***************************************************************************/
bool FNE::FNextFni(FNI *pfni, ulong *pgrffneOut, ulong grffneIn)
{
	AssertThis(0);
	AssertVarMem(pfni);
	AssertNilOrVarMem(pgrffneOut);
	short err;

	if (!_fInited)
		{
		Bug("must initialize the FNE before using it!");
		return fFalse;
		}

	if (grffneIn & ffneSkipDir)
		{
		//skip the rest of the stuff in this dir
		if (pvNil == _pglfes || !_pglfes->FPop(&_fesCur))
			goto LDone;
		}

	if (_fesCur.lwVol == 0)
		{
		//volume
		ParamBlockRec iob;

		do
			{
			ClearPb(&iob, size(iob));
			iob.volumeParam.ioVolIndex = (short)++_fesCur.iv;
			if ((err = PBGetVInfoSync(&iob)) != noErr)
				{
				if (err != nsvErr)
					PushErc(ercFneGeneral);
				goto LDone;
				}
			}
		while (!pfni->FBuild(iob.volumeParam.ioVRefNum, 0, pvNil, kftgDir));

		//we've got one
		goto LGotOne;
		}

	// directory or file
	for (;;)
		{
		int ich;
		bool fT;
		FTG *pftg;
		PSZ pszExt;
		CInfoPBRec iob;
		achar stz[kcbMaxStz];

		ClearPb(&iob, size(iob));
		iob.hFileInfo.ioNamePtr = (byte *)stz;
		iob.hFileInfo.ioVRefNum = (short)_fesCur.lwVol;
		iob.hFileInfo.ioDirID = _fesCur.lwDir;
		iob.hFileInfo.ioFDirIndex = (short)++_fesCur.iv;
		if ((err = PBGetCatInfoSync(&iob)) != noErr)
			{
			if (err != fnfErr)
				PushErc(ercFneGeneral);
			goto LPop;
			}

		if (iob.hFileInfo.ioFlFndrInfo.fdFlags &
				(kIsInvisible | kNameLocked | kIsAlias))
			{
			continue;
			}

		if (iob.hFileInfo.ioFlAttrib & 0x0010)
			fT = pfni->FBuild(_fesCur.lwVol, iob.hFileInfo.ioDirID, pvNil, kftgDir);
		else
			{
			StToStz(stz);
			fT = pfni->FBuild(_fesCur.lwVol, _fesCur.lwDir, stz,
				iob.hFileInfo.ioFlFndrInfo.fdType);
			}
		if (!fT)
			continue;

		if (_cftg == 0)
			goto LGotOne;
		for (pftg = _prgftg + _cftg; pftg-- > _prgftg; )
			{
			if (*pftg == pfni->_ftg)
				goto LGotOne;

			// see if the file has a dos-like extension matching the ftg
			if (pfni->_ftg != kftgDir && (pszExt = (achar *) pftg)[3] == 0 &&
					(ich = CchStz(stz) - CchSz(pszExt)) > 0 &&
					PszStz(stz)[ich - 1] == '.' &&
					fcmpEq == FcmpCompareInsSz(PszStz(stz) + ich, pszExt))
				{
				goto LGotOne;
				}
			}
		}
	Bug("How did we fall through to here?");

LPop:
	if (_pglfes == pvNil || _pglfes->IvMac() == 0)
		goto LDone;

	//we're about to pop a directory, so send the current directory back
	//with ffnePost
	if (pvNil != pgrffneOut)
		*pgrffneOut = ffnePost;
	if (!pfni->FBuild(_fesCur.lwVol, _fesCur.lwDir, pvNil, kftgDir))
		{
		PushErc(ercFneGeneral);
LDone:
		_Free();
		AssertThis(0);
		return fFalse;
		}
	Assert(_pglfes->FPop(&_fesCur), 0);
	AssertPo(pfni, ffniDir);
	AssertThis(0);
	return fTrue;

LGotOne:
	AssertPo(pfni, ffniFile | ffniDir);
	if (pvNil != pgrffneOut)
		*pgrffneOut = ffnePre | ffnePost;

	if (_fRecurse && pfni->_ftg == kftgDir)
		{
		if ((pvNil != _pglfes || pvNil != (_pglfes = GL::PglNew(size(FES), 5))) &&
			_pglfes->FPush(&_fesCur))
			{
			//set up the new fes
			_fesCur.lwVol = pfni->_fss.vRefNum;
			_fesCur.lwDir = pfni->_lwDir;
			_fesCur.iv = 0;
			if (pvNil != pgrffneOut)
				*pgrffneOut = ffnePre;
			}
		else
			PushErc(ercFneGeneral);
		}
	AssertThis(0);
	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a FNE.
***************************************************************************/
void FNE::AssertValid(ulong grf)
{
	FNE_PAR::AssertValid(0);
	if (_fInited)
		{
		AssertNilOrPo(_pglfes, 0);
		AssertIn(_cftg, 0, kcbMax);
		AssertPvCb(_prgftg, LwMul(size(FTG), _cftg));
		Assert((_cftg > kcftgFneBase) == (_prgftg == _rgftg),
			"wrong _prgftg");
		}
	else
		Assert(_pglfes == pvNil, 0);
}


/***************************************************************************
	Mark memory used by the FNE.
***************************************************************************/
void FNE::MarkMem(void)
{
	AssertValid(0);
	FNE_PAR::MarkMem();
	if (_prgftg != _rgftg)
		MarkPv(_prgftg);
	MarkMemObj(_pglfes);
}
#endif //DEBUG

