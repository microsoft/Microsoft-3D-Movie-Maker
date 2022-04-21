/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	File name management.

***************************************************************************/
#include "util.h"
#include <commdlg.h>
ASSERTNAME


// This is the FTG to use for temp files - clients may set this to whatever
// they want.
FTG vftgTemp = kftgTemp;

typedef OFSTRUCT OFS;
typedef OPENFILENAME OFN;

// maximal number of short characters in an extension is 4 (so it fits in
// a long).
const long kcchsMaxExt = size(long);

priv void _CleanFtg(FTG *pftg, PSTN pstnExt = pvNil);
FNI _fniTemp;

RTCLASS(FNI)
RTCLASS(FNE)


/***************************************************************************
	Sets the fni to nil values.
***************************************************************************/
void FNI::SetNil(void)
{
	_ftg = ftgNil;
	_stnFile.SetNil();
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
bool FNI::FGetOpen(achar *prgchFilter, HWND hwndOwner)
{
	AssertThis(0);
	AssertNilOrVarMem(prgchFilter);

	OFN ofn;
	SZ sz;

	ClearPb(&ofn, size(OFN));
	SetNil();

	sz[0] = 0;
	ofn.lStructSize = size(OFN);
	ofn.hwndOwner = hwndOwner;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = prgchFilter;
	ofn.nFilterIndex = 1L;
	ofn.lpstrFile = sz;
	ofn.nMaxFile = kcchMaxSz;
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY |
				OFN_NOTESTFILECREATE | OFN_READONLY;

	if (!GetOpenFileName(&ofn))
		{
		SetNil();
		return fFalse;
		}

	_stnFile = ofn.lpstrFile;
	_SetFtgFromName();
	AssertThis(ffniFile);
	return fTrue;
}


/***************************************************************************
	Get an fni (for saving) from the user.
***************************************************************************/
bool FNI::FGetSave(achar *prgchFilter, HWND hwndOwner)
{
	AssertThis(0);
	AssertNilOrVarMem(prgchFilter);

	OFN ofn;
	SZ sz;

	ClearPb(&ofn, size(OFN));
	SetNil();

	sz[0] = 0;
	ofn.lStructSize = size(OFN);
	ofn.hwndOwner = hwndOwner;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = prgchFilter;
	ofn.nFilterIndex = 1L;
	ofn.lpstrFile = sz;
	ofn.nMaxFile = kcchMaxSz;
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
	ofn.lpstrDefExt = PszLit("");

	if (!GetSaveFileName(&ofn))
		{
		SetNil();
		return fFalse;
		}

	_stnFile = sz;
	_SetFtgFromName();
	AssertThis(ffniFile);
	return fTrue;
}

/***************************************************************************
	Builds the fni from the path.
***************************************************************************/
bool FNI::FBuildFromPath(PSTN pstn, FTG ftgDef)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	long cch;
	achar *pchT;
	SZ sz;

	if (kftgDir != ftgDef)
		{
		// if the path ends with a slash or only has periods after the last
		// slash, force the fni to be a directory.

		cch = pstn->Cch();
		for (pchT = pstn->Prgch() + cch - 1; ; pchT--)
			{
			if (cch-- <= 0 || *pchT == ChLit('\\') || *pchT == ChLit('/'))
				{
				ftgDef = kftgDir;
				break;
				}
			if (*pchT != ChLit('.'))
				break;
			}
		}

	/* ask windows to parse the file name (resolves ".." and ".") and returns
	   absolute filename "X:\FOO\BAR", relative to the current drive and
	   directory if no drive or directory is given in pstn */
	if ((cch = GetFullPathName(pstn->Psz(), kcchMaxSz, sz, &pchT)) == 0 ||
			cch > kcchMaxSz)
		{
		goto LFail;
		}
	Assert(cch <= kcchMaxSz, 0);
	_stnFile = sz;

	if (ftgDef == kftgDir)
		{
		achar ch = _stnFile.Prgch()[_stnFile.Cch() - 1];
		if (ch != ChLit('\\') && ch != ChLit('/'))
			{
			if (!_stnFile.FAppendCh(ChLit('\\')))
				{
				goto LFail;
				}
			}
		_ftg = kftgDir;
		}
	else
		{
		_SetFtgFromName();
		if (_ftg == 0 && ftgDef != ftgNil &&
				pstn->Prgch()[pstn->Cch() - 1] != ChLit('.') &&
				!FChangeFtg(ftgDef))
			{
LFail:
			SetNil();
			PushErc(ercFniGeneral);
			return fFalse;
			}
		}

	AssertThis(ffniFile | ffniDir);
	return fTrue;
}


/******************************************************************************
	Will attempt to build an FNI with the given filename.  Uses the
	Windows SearchPath API, and thus the Windows path*search rules.

	Arguments:
		PSTN pstn ** the filename to look for

	Returns: fTrue if it could find the file
******************************************************************************/
bool FNI::FSearchInPath(PSTN pstn, PSTN pstnEnv)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	AssertNilOrPo(pstnEnv, 0);

	long cch;
	SZ sz;
	achar *pchT;
	PSZ psz = (pstnEnv == pvNil) ? pvNil : pstnEnv->Psz();

	if ((cch = SearchPath(psz, pstn->Psz(), pvNil, kcchMaxSz, sz, &pchT)) == 0 ||
			cch > kcchMaxSz)
		{
		SetNil();
		PushErc(ercFniGeneral);
		return fFalse;
		}

	Assert(cch <= kcchMaxSz, 0);
	_stnFile = sz;
	_SetFtgFromName();

	AssertThis(ffniFile | ffniDir);
	return fTrue;
}


/***************************************************************************
	Get a unique filename in the directory currently indicated by the fni.
***************************************************************************/
bool FNI::FGetUnique(FTG ftg)
{
	AssertThis(ffniFile | ffniDir);
	static short _dsw = 0;
	STN stn;
	STN stnOld;
	short sw;
	long cact;

	if (Ftg() == kftgDir)
		stnOld.SetNil();
	else
		GetLeaf(&stnOld);

	sw = (short)TsCurrentSystem() + ++_dsw;
	for (cact = 20; cact != 0; cact--, sw += ++_dsw)
		{
		stn.FFormatSz(PszLit("Temp%04x"), (long)sw);
		if (stn.FEqual(&stnOld))
			continue;
		if (FSetLeaf(&stn, ftg) && TExists() == tNo)
			return fTrue;
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

	if (_fniTemp._ftg != kftgDir)
		{
		// get the temp directory
		SZ sz;

		if (GetTempPath(kcchMaxSz, sz) == 0)
			{
			PushErc(ercFniGeneral);
			return fFalse;
			}
		_fniTemp._stnFile = sz;
		_fniTemp._ftg = kftgDir;
		AssertPo(&_fniTemp, ffniDir);
		}
	*this = _fniTemp;
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
	AssertThis(ffniDir | ffniFile);
	STN stn;
	PSZ psz;
	ulong grfvk = fvkNil;

	psz = _stnFile.Psz();
	if (_stnFile.Cch() < 3 || psz[1] != ':' || psz[2] != '\\' && psz[2] != '/')
		return fvkNetwork;

	stn.FFormatSz(PszLit("%c:\\"), psz[0]);
	switch (GetDriveType(stn.Psz()))
		{
	case DRIVE_FIXED:
	case DRIVE_RAMDISK:
		break;
	case DRIVE_REMOVABLE:
		grfvk |= fvkRemovable;
		switch (stn.Psz()[0])
			{
		case ChLit('A'):
		case ChLit('B'):
		case ChLit('a'):
		case ChLit('b'):
			grfvk |= fvkFloppy;
			break;
			}
		break;
	case DRIVE_CDROM:
		grfvk |= fvkRemovable | fvkCD;
		break;
	case DRIVE_REMOTE:
	default:
		// treat anything else like a network drive
		grfvk |= fvkNetwork;
		break;
		}

	return grfvk;
}


/***************************************************************************
	Set the leaf to the given string and type.
***************************************************************************/
bool FNI::FSetLeaf(PSTN pstn, FTG ftg)
{
	AssertThis(ffniFile | ffniDir);
	AssertNilOrPo(pstn, 0);

	_CleanFtg(&ftg);
	Assert(FPure(ftg == kftgDir) == FPure(pstn == pvNil || pstn->Cch() == 0),
		"ftg doesn't match pstn");
	if (!_FChangeLeaf(pstn))
		goto LFail;

	if ((kftgDir != ftg) && (ftgNil != ftg) && !FChangeFtg(ftg))
		goto LFail;

	AssertThis(ffniFile | ffniDir);
	return fTrue;

LFail:
	SetNil();
	PushErc(ercFniGeneral);
	return fFalse;
}


/******************************************************************************
	Changes just the FTG of the FNI, leaving the file path and filename alone
	(but does change the extension). Returns: fTrue if it succeeds
******************************************************************************/
bool FNI::FChangeFtg(FTG ftg)
{
	AssertThis(ffniFile);
	Assert(ftg != ftgNil && ftg != kftgDir, "Bad FTG");
	STN stnFtg;
	long cchBase;

	_CleanFtg(&ftg, &stnFtg);
	if (_ftg == ftg)
		return fTrue;

	// set the extension
	cchBase = _stnFile.Cch() - _CchExt();

	//use >= to leave room for the '.'
	if (cchBase + stnFtg.Cch() >= kcchMaxStn)
		return fFalse;

	_stnFile.Delete(cchBase);
	_ftg = ftg;
	if (stnFtg.Cch() > 0)
		{
		_stnFile.FAppendCh(ChLit('.'));
		_stnFile.FAppendStn(&stnFtg);
		}
	return fTrue;
}


/***************************************************************************
	Get the leaf name.
***************************************************************************/
void FNI::GetLeaf(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	achar *pch;
	PSZ psz = _stnFile.Psz();

	for (pch = psz + _stnFile.Cch();
			pch-- > psz && *pch != '\\' && *pch != '/'; )
		{
		}
	Assert(pch > psz, "bad fni");

	pstn->SetSz(pch + 1);
}


/***************************************************************************
	Get a string representing the path of the fni.
***************************************************************************/
void FNI::GetStnPath(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	*pstn = _stnFile;
}


/***************************************************************************
	Determines if the file/directory exists.  Returns tMaybe on error or
	if the fni type (file or dir) doesn't match the disk object of the
	same name.
***************************************************************************/
bool FNI::TExists(void)
{
	AssertThis(ffniFile | ffniDir);
	STN stn;
	PSTN pstn;
	ulong lu;

	// strip off the trailing slash (if a directory).
	pstn = &_stnFile;
	if (_ftg == kftgDir)
		{
		long cch;

		stn = _stnFile;
		pstn = &stn;
		cch = stn.Cch();
		Assert(cch > 0 && (stn.Psz()[cch - 1] == '\\' ||
			stn.Psz()[cch - 1] == '/'), 0);
		stn.Delete(cch - 1);
		}

	if (0xFFFFFFFF == (lu = GetFileAttributes(pstn->Psz())))
		{
		/* Any of these are equivalent to "there's no file with that name" */
		if ((lu = GetLastError()) == ERROR_FILE_NOT_FOUND ||
				lu == ERROR_INVALID_DRIVE)
			{
			return tNo;
			}
		PushErc(ercFniGeneral);
		return tMaybe;
		}
	if ((_ftg == kftgDir) != FPure(lu & FILE_ATTRIBUTE_DIRECTORY))
		{
		PushErc(ercFniMismatch);
		return tMaybe;
		}
	if (lu & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))
		{
		PushErc(ercFniHidden);
		return tMaybe;
		}
	return tYes;
}


/***************************************************************************
	Delete the physical file.  Should not be open.
***************************************************************************/
bool FNI::FDelete(void)
{
	AssertThis(ffniFile);
	Assert(FIL::PfilFromFni(this) == pvNil, "file is open");

	if (DeleteFile(_stnFile.Psz()))
		return fTrue;
	PushErc(ercFniDelete);
	return fFalse;
}


/***************************************************************************
	Renames the file indicated by this to *pfni.
***************************************************************************/
bool FNI::FRename(FNI *pfni)
{
	AssertThis(ffniFile);
	AssertPo(pfni, ffniFile);

	if (!(FILE_ATTRIBUTE_READONLY & GetFileAttributes(_stnFile.Psz())) &&
			MoveFile(_stnFile.Psz(), pfni->_stnFile.Psz()))
		{
		return fTrue;
		}
	PushErc(ercFniRename);
	return fFalse;
}


/***************************************************************************
	Compare two fni's for equality.
***************************************************************************/
bool FNI::FEqual(FNI *pfni)
{
	AssertThis(ffniFile | ffniDir);
	AssertPo(pfni, ffniFile | ffniDir);

	return pfni->_stnFile.FEqualUser(&_stnFile);
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
	AssertThis(ffniFile | ffniDir);
	AssertPo(pfni, ffniFile | ffniDir);
	FNI fni1, fni2;

	fni1 = *this;
	fni2 = *pfni;
	fni1._FChangeLeaf(pvNil);
	fni2._FChangeLeaf(pvNil);
	return fni1.FEqual(&fni2);
}


/***************************************************************************
	Determine if the directory pstn in fni exists, optionally creating it
	and/or moving into it.  Specify ffniCreateDir to create it if it
	doesn't exist.  Specify ffniMoveTo to make the fni refer to it.
***************************************************************************/
bool FNI::FDownDir(PSTN pstn, ulong grffni)
{
	AssertThis(ffniDir);
	AssertPo(pstn, 0);

	FNI fniT;

	fniT = *this;
	//the +1 is for the \ character
	if (fniT._stnFile.Cch() + pstn->Cch() + 1 > kcchMaxStn)
		{
		PushErc(ercFniGeneral);
		return fFalse;
		}
	AssertDo(fniT._stnFile.FAppendStn(pstn), 0);
	AssertDo(fniT._stnFile.FAppendCh(ChLit('\\')), 0);
	fniT._ftg = kftgDir;
	AssertPo(&fniT, ffniDir);

	if (fniT.TExists() != tYes)
		{
		if (!(grffni & ffniCreateDir))
			return fFalse;
		// try to create it
		if (!CreateDirectory(fniT._stnFile.Psz(), NULL))
			{
			PushErc(ercFniDirCreate);
			return fFalse;
			}
		}
	if (grffni & ffniMoveToDir)
		*this = fniT;

	return fTrue;
}


/***************************************************************************
	Gets the lowest directory name (if pstn is not nil) and optionally
	moves the fni up a level (if ffniMoveToDir is specified).
***************************************************************************/
bool FNI::FUpDir(PSTN pstn, ulong grffni)
{
	AssertThis(ffniDir);
	AssertNilOrPo(pstn, 0);

	long cch;
	achar *pchT;
	SZ sz;
	STN stn;

	stn = _stnFile;
	if (!stn.FAppendSz(PszLit("..")))
		return fFalse;

	if ((cch = GetFullPathName(stn.Psz(), kcchMaxSz, sz, &pchT)) == 0 ||
			cch >= _stnFile.Cch() - 1)
		{
		return fFalse;
		}
	Assert(cch <= kcchMaxSz, 0);
	Assert(cch < _stnFile.Cch() + 2, 0);
	stn = sz;
	switch (stn.Psz()[cch - 1])
		{
	case ChLit('\\'):
	case ChLit('/'):
		break;
	default:
		AssertDo(stn.FAppendCh(ChLit('\\')), 0);
		cch++;
		break;
		}

	if (pvNil != pstn)
		{
		// copy the tail and delete the trailing slash
		pstn->SetSz(_stnFile.Psz() + cch);
		pstn->Delete(pstn->Cch() - 1);
		}

	if (grffni & ffniMoveToDir)
		{
		_stnFile = stn;
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
	AssertPo(&_stnFile, 0);

	SZ szT;
	long cch;
	PSZ pszT;

	if (grffni == 0)
		grffni = ffniEmpty | ffniDir | ffniFile;

	if (_ftg == ftgNil)
		{
		Assert(grffni & ffniEmpty, "unexpected empty");
		Assert(_stnFile.Cch() == 0, "named empty?");
		return;
		}

	if ((cch = GetFullPathName(_stnFile.Psz(), kcchMaxSz, szT, &pszT)) == 0 ||
			cch > kcchMaxSz || !_stnFile.FEqualUserRgch(szT, CchSz(szT)))
		{
		Bug("bad fni");
		return;
		}

	if (_ftg == kftgDir)
		{
		Assert(grffni & ffniDir, "unexpected dir");
		Assert(szT[cch - 1] == ChLit('\\') || szT[cch - 1] == ChLit('/'),
			"expected trailing slash");
		Assert(pszT == NULL, "unexpected filename");
		}
	else
		{
		Assert(grffni & ffniFile, "unexpected file");
		Assert(pszT >= szT && pszT < szT + cch, "expected filename");
		}
}
#endif //DEBUG


/***************************************************************************
	Find the length of the file extension on the fni (including the period).
	Allow up to kcchsMaxExt characters for the extension (plus one for the
	period).
***************************************************************************/
long FNI::_CchExt(void)
{
	AssertBaseThis(0);
	long cch;
	PSZ psz = _stnFile.Psz();
	achar *pch = psz + _stnFile.Cch() - 1;

	for (cch = 1; cch <= kcchsMaxExt + 1 && pch >= psz; cch++, pch--)
		{
		if ((achar)(schar)*pch != *pch)
			{
			// not an ANSI character - so doesn't qualify for our
			// definition of an extension
			return 0;
			}

		switch (*pch)
			{
		case ChLit('.'):
			return cch;
		case ChLit('\\'):
		case ChLit('/'):
			return 0;
			}
		}

	return 0;
}


/***************************************************************************
	Set the ftg from the file name.
***************************************************************************/
void FNI::_SetFtgFromName(void)
{
	AssertBaseThis(0);
	Assert(_stnFile.Cch() > 0, 0);
	long cch, ich;
	achar *pchLim = _stnFile.Psz() + _stnFile.Cch();

	if (pchLim[-1] == ChLit('\\') || pchLim[-1] == ChLit('/'))
		_ftg = kftgDir;
	else
		{
		_ftg = 0;
		cch = _CchExt() - 1;
		AssertIn(cch, -1, kcchsMaxExt + 1);
		pchLim -= cch;
		for (ich = 0; ich < cch; ich++)
			_ftg = (_ftg << 8) | (long)(byte)ChsUpper((schar)pchLim[ich]);
		}
	AssertThis(ffniFile | ffniDir);
}


/***************************************************************************
	Change the leaf of the fni.
***************************************************************************/
bool FNI::_FChangeLeaf(PSTN pstn)
{
	AssertThis(ffniFile | ffniDir);
	AssertNilOrPo(pstn, 0);

	achar *pch;
	PSZ psz;
	long cchBase, cch;

	psz = _stnFile.Psz();
	for (pch = psz + _stnFile.Cch();
			pch-- > psz && *pch != ChLit('\\')  && *pch != ChLit('/'); )
		{
		}
	Assert(pch > psz, "bad fni");

	cchBase = pch - psz + 1;
	_stnFile.Delete(cchBase);
	_ftg = kftgDir;
	if (pstn != pvNil && (cch = pstn->Cch()) > 0)
		{
		if (cchBase + cch > kcchMaxStn)
			return fFalse;
		AssertDo(_stnFile.FAppendStn(pstn), 0);
		_SetFtgFromName();
		}
	AssertThis(ffniFile | ffniDir);
	return fTrue;
}


/***************************************************************************
	Make sure the ftg is all uppercase and has no characters after a zero.
***************************************************************************/
priv void _CleanFtg(FTG *pftg, PSTN pstnExt)
{
	AssertVarMem(pftg);
	AssertNilOrPo(pstnExt, 0);

	long ichs;
	schar chs;
	bool fZero;
	FTG ftgNew;

	if (pvNil != pstnExt)
		pstnExt->SetNil();

	if (*pftg == kftgDir || *pftg == ftgNil)
		return;

	fZero = fFalse;
	ftgNew = 0;
	for (ichs = 0; ichs < kcchsMaxExt; ichs++)
		{
		chs = (schar)((ulong)*pftg >> (ichs * 8));
		fZero |= (chs == 0);
		if (!fZero)
			{
			chs = ChsUpper(chs);
			ftgNew |= (long)(byte)chs << (8 * ichs);
			if (pvNil != pstnExt)
				pstnExt->FInsertCh(0, (achar)(byte)chs);
			}
		}

	*pftg = ftgNew;
}


/***************************************************************************
	Constructor for a File Name Enumerator.
***************************************************************************/
FNE::FNE(void)
{
	AssertBaseThis(0);
	_prgftg = _rgftg;
	_pglfes = pvNil;
	_fesCur.hn = hBadWin;
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
	do
		{
		if (hBadWin != _fesCur.hn)
			FindClose(_fesCur.hn);
		}
	while (pvNil != _pglfes && _pglfes->FPop(&_fesCur));
	_fesCur.hn = hBadWin;
	_fInited = fFalse;
	ReleasePpo(&_pglfes);
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
	FTG *pftg;

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
		for (pftg = _prgftg + _cftg; pftg-- > _prgftg; )
			_CleanFtg(pftg);
		}

	if (pfniDir == pvNil)
		{
		_fesCur.chVol = 'A';
		_fesCur.grfvol = GetLogicalDrives();
		}
	else
		{
		STN stn;

		_fesCur.grfvol = 0;
		_fesCur.chVol = 0;
		_fesCur.fni = *pfniDir;
		stn = PszLit("*");
		if (!_fesCur.fni._FChangeLeaf(&stn))
			{
			PushErc(ercFneGeneral);
			_Free();
			AssertThis(0);
			return fFalse;
			}
		}
	_fesCur.hn = hBadWin;
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
	STN stn;
	bool fT;
	long fvol;
	long err;
	FTG *pftg;

	if (!_fInited)
		{
		Bug("must initialize the FNE before using it!");
		return fFalse;
		}

	if (grffneIn & ffneSkipDir)
		{
		//skip the rest of the stuff in this dir
		if (!_FPop())
			goto LDone;
		}

	if (_fesCur.chVol != 0)
		{
		//volume
		for (fvol = 1L << (_fesCur.chVol - 'A');
				_fesCur.chVol <= 'Z' && (_fesCur.grfvol & fvol) == 0;
				_fesCur.chVol++, fvol <<= 1)
			{
			}

		if (_fesCur.chVol > 'Z')
			goto LDone;
		//we've got one
		stn.FFormatSz(PszLit("%c:\\"), (long)_fesCur.chVol++);
		AssertDo(pfni->FBuildFromPath(&stn), 0);
		goto LGotOne;
		}

	// directory or file
	for (;;)
		{
		if (hBadWin == _fesCur.hn)
			{
			_fesCur.hn = FindFirstFile(_fesCur.fni._stnFile.Psz(), &_fesCur.wfd);
			if (hBadWin == _fesCur.hn)
				{
				err = GetLastError();
				goto LReportError;
				}
			}
		else if (!FindNextFile(_fesCur.hn, &_fesCur.wfd))
			{
			err = GetLastError();
LReportError:
			if (err != ERROR_NO_MORE_FILES)
				PushErc(ercFneGeneral);
			goto LPop;
			}

		if (_fesCur.wfd.dwFileAttributes & (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM))
			continue;

		stn.SetSz(_fesCur.wfd.cFileName);
		*pfni = _fesCur.fni;
		if (_fesCur.wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			if (stn.FEqualSz(PszLit(".")) || stn.FEqualSz(PszLit("..")))
				continue;
			AssertDo(pfni->_FChangeLeaf(pvNil), 0);
			fT = pfni->FDownDir(&stn, ffniMoveToDir);
			}
		else
			fT = pfni->_FChangeLeaf(&stn);
		if (!fT)
			{
			PushErc(ercFneGeneral);
			continue;
			}

		if (_cftg == 0)
			goto LGotOne;
		for (pftg = _prgftg + _cftg; pftg-- > _prgftg; )
			{
			if (*pftg == pfni->_ftg)
				goto LGotOne;
			}
		}
	Bug("How did we fall through to here?");

LPop:
	if (pvNil == _pglfes || _pglfes->IvMac() == 0)
		{
LDone:
		_Free();
		AssertThis(0);
		return fFalse;
		}

	//we're about to pop a directory, so send the current directory back
	//with ffnePost
	if (pvNil != pgrffneOut)
		*pgrffneOut = ffnePost;
	*pfni = _fesCur.fni;
	AssertDo(pfni->_FChangeLeaf(pvNil), 0);
	AssertDo(_FPop(), 0);
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
			_fesCur.fni = *pfni;
			stn = PszLit("*");
			if (!_fesCur.fni._FChangeLeaf(&stn))
				{
				AssertDo(_pglfes->FPop(&_fesCur), 0);
				}
			else
				{
				_fesCur.hn = hBadWin;
				_fesCur.grfvol = 0;
				_fesCur.chVol = 0;
				if (pvNil != pgrffneOut)
					*pgrffneOut = ffnePre;
				}
			}
		else
			PushErc(ercFneGeneral);
		}
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Pop a state in the FNE.
***************************************************************************/
bool FNE::_FPop(void)
{
	AssertBaseThis(0);
	if (hBadWin != _fesCur.hn)
		{
		FindClose(_fesCur.hn);
		_fesCur.hn = hBadWin;
		}
	return pvNil != _pglfes && _pglfes->FPop(&_fesCur);
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
		Assert((_cftg <= kcftgFneBase) == (_prgftg == _rgftg),
			"wrong _prgftg");
		}
	else
		Assert(_pglfes == pvNil, 0);
}


/***************************************************************************
	Mark memory for the FNE.
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


