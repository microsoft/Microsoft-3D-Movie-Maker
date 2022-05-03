/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	File management.

***************************************************************************/
#include "util.h"
#include <script.h>
ASSERTNAME


/***************************************************************************
	Open or create the file.  If the file is already open, sets the
	permissions according to grffil.  Note that the ffilDenyRead and
	ffilDenyWrite do not work as in Windows.  Effectively, this routine
	picks one of the following combinations:
		ffilWriteEnable | ffilDenyRead | ffilDenyWrite
		ffilDenyWrite
***************************************************************************/
bool FIL::_FOpen(bool fCreate, ulong grffil)
{
	AssertBaseThis(0);
	AssertPo(&_fni, ffniFile);
	short fref;

	if (_el != elNil)
		goto LFail;

	//we just care about the permissions
	if (grffil & ffilWriteEnable)
		grffil = ffilWriteEnable | ffilDenyRead | ffilDenyWrite;
	else
		grffil = ffilDenyWrite;

	//create the file if we need to
	if (fCreate)
		{
		Assert(!_fOpen, "Creating an open file");
		short cact, err;

		for (cact = 0; ; cact++)
			{
			if ((err = FSpCreate(&_fni._fss, vftgCreator,
					_fni._ftg, smSystemScript)) == noErr)
				{
				break;
				}
			if (cact > 0 || err != dupFNErr || FSpDelete(&_fni._fss) != noErr)
				goto LFail;
			}
		}
	else if (_fOpen && (~_grffil & grffil) == 0)
		{
		//permissions are already set high enough
		return fTrue;
		}

	if (noErr != FSpOpenDF(&_fni._fss,
			(grffil & ffilWriteEnable) ? fsRdWrPerm : fsRdPerm, &fref))
		{
LFail:
		if (!_fOpen)
			_el = kelCritical;
		return fFalse;
		}
	if (_fOpen && _fref != fref)
		FSClose(_fref);
	_fref = fref;
	_fOpen = fTrue;
	_grffil = _grffil & ~kgrffilPerm | grffil;

	return fTrue;
}


/***************************************************************************
	Close the file.
***************************************************************************/
void FIL::_Close(void)
{
	AssertBaseThis(0);

	if (_fOpen)
		{
		FSClose(_fref);
		if (_fWrote)
			FlushVol(pvNil, _fni._fss.vRefNum);
		_fOpen = fFalse;
		_fref = 0;

		if (_grffil & ffilTemp)
			FSpDelete(&_fni._fss);
		}
	_el = elNil;
}


/***************************************************************************
	Flush the file (and its volume?).
***************************************************************************/
void FIL::Flush(void)
{
	AssertThis(0);
	FlushVol(pvNil, _fni._fss.vRefNum);
}


/***************************************************************************
	Seek to the given fp.
***************************************************************************/
void FIL::_SetFpPos(FP fp)
{
	AssertThis(0);
	if (_el < kelSeek && SetFPos(_fref, fsFromStart, fp) != noErr)
		{
		_el = kelSeek;
		PushErc(ercFileGeneral);
		}
}


/***************************************************************************
	Set the length of the file.
***************************************************************************/
bool FIL::FSetFpMac(FP fp)
{
	AssertThis(0);
	AssertIn(fp, 0, kcbMax);
	if (_el >= kelWrite)
		return fFalse;

	_fWrote = fTrue;
	Assert(_grffil & ffilWriteEnable, "file not open for write");
	if (SetEOF(_fref, fp) != noErr)
		{
		_el = kelWrite;
		PushErc(ercFileGeneral);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Return the length of the file.
***************************************************************************/
FP FIL::FpMac(void)
{
	AssertThis(0);
	FP fp;

	if (_el < kelSeek && GetEOF(_fref, &fp) != noErr)
		{
		_el = kelSeek;
		PushErc(ercFileGeneral);
		}
	return _el < kelSeek ? fp : 0;
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

	Debug( FP dfp = FpMac() - fp; )

	_SetFpPos(fp);
	if (_el >= kelRead)
		return fFalse;

	Assert(dfp >= cb, "read past EOF");
	if (cb > 0 && FSRead(_fref, &cb, pv) != noErr)
		{
		_el = kelRead;
		PushErc(ercFileGeneral);
		return fFalse;
		}
	return fTrue;
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

	_SetFpPos(fp);
	if (_el >= kelWrite)
		return fFalse;

	Assert(_grffil & ffilWriteEnable, "file not open for write");
	_fWrote = fTrue;
	if (cb > 0 && FSWrite(_fref, &cb, pv) != noErr)
		{
		_el = kelWrite;
		PushErc(ercFileGeneral);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Swap the names of the two files.  They should be in the same directory.
***************************************************************************/
bool FIL::FSwapNames(PFIL pfil)
{
	AssertThis(0);
	AssertPo(pfil, 0);

	FNI fni;

	if (_el != elNil || pfil->_el != elNil)
		return fFalse;
	Assert(_fni.FSameDir(&pfil->_fni),
		"trying to change directories with FSwapNames");

	// swap the data
	if (FSpExchangeFiles(&_fni._fss, &pfil->_fni._fss) != noErr)
		{
		PushErc(ercFileSwapNames);
		return fFalse;
		}

	// swap the fni's
	fni = _fni;
	_fni = pfil->_fni;
	pfil->_fni = fni;

	AssertThis(0);
	AssertPo(pfil, 0);
	return fTrue;
}


/***************************************************************************
	Rename the file.
***************************************************************************/
bool FIL::FRename(FNI *pfni)
{
	AssertThis(0);
	AssertPo(pfni, ffniFile);

	if (_el != elNil)
		return fFalse;
	Assert(_fni.FSameDir(pfni), "trying to change directories with FRename");

	if (FSpRename(&_fni._fss, pfni->_fss.name) != noErr)
		{
		PushErc(ercFileRename);
		return fFalse;
		}
	CopySt((achar *)pfni->_fss.name, (achar *)_fni._fss.name);
	AssertThis(0);
	return fTrue;
}
