/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Windows specific file management.

***************************************************************************/
#include "util.h"
ASSERTNAME


const ulong kfpError = 0xFFFFFFFF;
priv HANDLE _HfileOpen(PSZ pszFile, bool fCreate, ulong grffil);


/***************************************************************************
	Open or create the file by calling CreateFile.  Returns hBadWin on
	failure.
***************************************************************************/
priv HANDLE _HfileOpen(PSZ psz, bool fCreate, ulong grffil)
{
	ulong luAccess = GENERIC_READ;
	ulong luShare = 0;

	if (grffil & ffilWriteEnable)
		luAccess |= GENERIC_WRITE;
	if (!(grffil & ffilDenyRead))
		luShare |= FILE_SHARE_READ;
	if (!(grffil & ffilDenyWrite))
		luShare |= FILE_SHARE_WRITE;

	return CreateFile(psz, luAccess, luShare, pvNil,
		fCreate ? CREATE_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, pvNil);
}


/***************************************************************************
	Open or create the file.  If the file is already open, sets the
	permissions according to grffil.
***************************************************************************/
bool FIL::_FOpen(bool fCreate, ulong grffil)
{
	AssertBaseThis(0);
	bool fRet = fFalse;

	_mutx.Enter();

	if (_el >= kelCritical)
		goto LRet;

	grffil &= kgrffilPerm;
	if (_fOpen)
		{
		Assert(!fCreate, "can't create an open file");
		if ((~_grffil & grffil) == 0)
			{
			// permissions are already set high enough
			fRet = fTrue;
			goto LRet;
			}
		CloseHandle(_hfile);
		_hfile = hBadWin;

		// maintain the permissions we had before
		grffil |= _grffil & kgrffilPerm;
		}

	_hfile = _HfileOpen(_fni._stnFile.Psz(), fCreate, grffil);

	if (hBadWin == _hfile)
		{
		// if it was open, re-open it with old permissions
		if (_fOpen)
			{
			_hfile = _HfileOpen(_fni._stnFile.Psz(), fFalse, _grffil);
			if (hBadWin != _hfile)
				goto LRet;
			}
		_fOpen = fFalse;
		_el = kelCritical;
		goto LRet;
		}

	_fOpen = fTrue;
	_fEverOpen = fTrue;
	_grffil = (_grffil & ~kgrffilPerm) | grffil;
	fRet = fTrue;

LRet:
	_mutx.Leave();

	return fRet;
}


/***************************************************************************
	Close the file.
***************************************************************************/
void FIL::_Close(bool fFinal)
{
	AssertBaseThis(0);

	_mutx.Enter();

	if (_fOpen)
		{
		Flush();
		CloseHandle(_hfile);
		_fOpen = fFalse;
		_hfile = hBadWin;
		}

	if ((_grffil & ffilTemp) && fFinal && _fEverOpen)
		{
		if (!DeleteFile(_fni._stnFile.Psz()))
			Warn("Deleting temp file failed");
		}

	_mutx.Leave();
}


/***************************************************************************
	Flush the file (and its volume?).
***************************************************************************/
void FIL::Flush(void)
{
	AssertThis(0);

	_mutx.Enter();

	if (_fOpen)
		FlushFileBuffers(_hfile);

	_mutx.Leave();
}


/***************************************************************************
	Seek to the given fp - assumes the mutx is already entered.
***************************************************************************/
void FIL::_SetFpPos(FP fp)
{
	AssertThis(0);

	if (!_fOpen)
		_FOpen(fFalse, _grffil);

	if (_el < kelSeek &&
			SetFilePointer(_hfile, fp, pvNil, FILE_BEGIN) == kfpError)
		{
		PushErc(ercFileGeneral);
		_el = kelSeek;
		}
}


/***************************************************************************
	Set the length of the file.  This doesn't zero the appended portion.
***************************************************************************/
bool FIL::FSetFpMac(FP fp)
{
	AssertThis(0);
	AssertIn(fp, 0, kcbMax);
	bool fRet;

	_mutx.Enter();

	Assert(_grffil & ffilWriteEnable, "can't write to read only file");
	_SetFpPos(fp);

	if (_el < kelWrite)
		{
		_fWrote = fTrue;
		if (!SetEndOfFile(_hfile))
			{
			PushErc(ercFileGeneral);
			_el = kelWrite;
			}
		}
	fRet = _el < kelWrite;

	_mutx.Leave();

	return fRet;
}


/***************************************************************************
	Return the length of the file.
***************************************************************************/
FP FIL::FpMac(void)
{
	AssertThis(0);
	FP fp;

	_mutx.Enter();

	if (!_fOpen)
		_FOpen(fFalse, _grffil);

	if (_el < kelSeek &&
			(fp = SetFilePointer(_hfile, 0, pvNil, FILE_END)) == kfpError)
		{
		PushErc(ercFileGeneral);
		_el = kelSeek;
		}

	if (_el >= kelSeek)
		fp = 0;

	_mutx.Leave();

	return fp;
}


/***************************************************************************
	Read a block from the file.
***************************************************************************/
bool FIL::FReadRgb(void *pv, long cb, FP fp)
{
	AssertThis(0);
	AssertIn(cb, 0, kcbMax);
	AssertIn(fp, 0, klwMax);
	AssertPvCb(pv, cb);

	long cbT;
	bool fRet = fFalse;

	if (cb <= 0)
		return fTrue;

	_mutx.Enter();

	if (!_fOpen)
		_FOpen(fFalse, _grffil);

	Debug( FP dfp = FpMac() - fp; )

	_SetFpPos(fp);
	if (_el >= kelRead)
		goto LRet;

	Assert(dfp >= cb, "read past EOF");
	if (!ReadFile(_hfile, pv, cb, (ulong *)&cbT, pvNil) || cb != cbT)
		{
		PushErc(ercFileGeneral);
		_el = kelRead;
		goto LRet;
		}

	fRet = fTrue;

LRet:
	_mutx.Leave();
	return fRet;
}


/***************************************************************************
	Write a block to the file.
***************************************************************************/
bool FIL::FWriteRgb(void *pv, long cb, FP fp)
{
	AssertThis(0);
	AssertIn(cb, 0, kcbMax);
	AssertIn(fp, 0, klwMax);
	AssertPvCb(pv, cb);

	long cbT;
	bool fRet = fFalse;

	if (cb <= 0)
		return fTrue;

	_mutx.Enter();

	Assert(_grffil & ffilWriteEnable, "can't write to read only file");

	if (!_fOpen)
		_FOpen(fFalse, _grffil);

	_SetFpPos(fp);
	if (_el >= kelWrite)
		goto LRet;

	_fWrote = fTrue;
	if (!WriteFile(_hfile, pv, cb, (ulong *)&cbT, pvNil) || cb != cbT)
		{
		PushErc(ercFileGeneral);
		_el = kelWrite;
		goto LRet;
		}

	fRet = fTrue;

LRet:
	_mutx.Leave();
	return fRet;
}


/***************************************************************************
	Swap the names of the two files.  They should be in the same directory.
***************************************************************************/
bool FIL::FSwapNames(PFIL pfil)
{
	AssertThis(0);
	AssertPo(pfil, 0);
	FNI fni;
	FNI fniT;
	bool fRet = fFalse;

	if (this == pfil)
		{
		Bug("Why are you calling FSwapNames on the same file?");
		return fTrue;
		}

	_mutx.Enter();
	pfil->_mutx.Enter();

	Assert(_fni.FSameDir(&pfil->_fni),
		"trying to change directories with FSwapNames");

	fni = pfil->_fni;
	if (!fni.FGetUnique(fni.Ftg()))
		goto LRet;

	_Close();
	pfil->_Close();

	if (!_fni.FRename(&fni))
		goto LFail;
	if (!pfil->_fni.FRename(&_fni))
		goto LRenameFail;
	if (!fni.FRename(&pfil->_fni))
		{
		if (!_fni.FRename(&pfil->_fni))
			{
			Bug("rename failure");
			pfil->_fni = _fni;
			_fni = fni;
			goto LFail;
			}

LRenameFail:
		if (!fni.FRename(&_fni))
			{
			Bug("rename failure");
			_fni = fni;
			}
		goto LFail;
		}

	fni = _fni;
	_fni = pfil->_fni;
	pfil->_fni = fni;
	fRet = fTrue;

LFail:
	// reopen the files
	_FOpen(fFalse, _grffil);
	pfil->_FOpen(fFalse, pfil->_grffil);

LRet:
	_mutx.Leave();
	pfil->_mutx.Leave();

	if (!fRet)
		PushErc(ercFileSwapNames);

	AssertThis(0);
	AssertPo(pfil, 0);

	return fRet;
}


/***************************************************************************
	Rename a file.  The new fni should be on the same volume.
	This may fail without an error code being set.
***************************************************************************/
bool FIL::FRename(FNI *pfni)
{
	AssertThis(0);
	AssertPo(pfni, ffniFile);
	FNI fni;
	bool fRet = fFalse;

	_mutx.Enter();

	Assert(_fni.FSameDir(pfni), "trying to change directories with FRename");

	_Close();
	if (fRet = _fni.FRename(pfni))
		_fni = *pfni;

	// reopen the file
	if (!_FOpen(fFalse, _grffil))
		fRet = fFalse;

	_mutx.Leave();

	if (!fRet)
		PushErc(ercFileRename);

	AssertThis(0);
	return fRet;
}
