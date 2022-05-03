/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Clipboard object implementation.

***************************************************************************/
#include "frame.h"
ASSERTNAME

RTCLASS(CLIP)

PCLIP vpclip;
CLIP _clip;


/***************************************************************************
	Constructor for the clipboard.
***************************************************************************/
CLIP::CLIP(void)
{
	vpclip = this;
	AssertThis(0);
}


/***************************************************************************
	Return whether the given document is the clipboard document.  If pdocb
	is nil this returns true if the clipboard is empty and false if it's
	not empty.
***************************************************************************/
bool CLIP::FDocIsClip(PDOCB pdocb)
{
	AssertThis(0);
	AssertNilOrPo(pdocb, 0);

	if (pvNil == pdocb)
		_EnsureDoc();

	return pdocb == _pdocb;
}


/***************************************************************************
	Show the clipboard document.
***************************************************************************/
void CLIP::Show(void)
{
	AssertThis(0);

	_EnsureDoc();
	_ImportCur();
	if (pvNil != _pdocb)
		{
		if (_pdocb->Cddg() > 0)
			_pdocb->ActivateDmd();
		else
			_pdocb->PdmdNew();
		}
}


/***************************************************************************
	Make the given document the clipboard document.
***************************************************************************/
void CLIP::Set(PDOCB pdocb, bool fExport)
{
	AssertThis(0);
	AssertNilOrPo(pdocb, 0);

	if (_fExporting || _fImporting)
		{
		Bug("can't change the clipboard while exporting or importing");
		return;
		}

	if (pdocb == _pdocb)
		return;

	_fDocCurrent = fTrue;
	_fDelayImport = fFalse;
	_clfmImport = clfmNil;

	//throw away the old clip document and replace it with the new one
	SwapVars(&pdocb, &_pdocb);
	if (pvNil != pdocb)
		{
		if (pdocb->Cddg() > 0)
			pdocb->UpdateName();
		ReleasePpo(&pdocb);
		}

	if (pvNil != _pdocb)
		{
		_pdocb->AddRef();
		if (_pdocb->Cddg() > 0)
			_pdocb->UpdateName();

		if (fExport)
			{
			_pdocb->ExportFormats(this);
			EndExport();
			}
		}
}


/***************************************************************************
	See if the clipboard supports this format and if so, get it.
***************************************************************************/
bool CLIP::FGetFormat(long cls, PDOCB *ppdocb)
{
	AssertThis(0);
	AssertNilOrVarMem(ppdocb);

	_EnsureDoc();
	if (pvNil != ppdocb)
		_ImportCur();

	if (pvNil == _pdocb)
		return fFalse;

	if (_pdocb->FIs(cls))
		{
		if (pvNil != ppdocb)
			{
			*ppdocb = _pdocb;
			(*ppdocb)->AddRef();
			}
		return fTrue;
		}

	return _pdocb->FGetFormat(cls, ppdocb);
}


/***************************************************************************
	Import stuff from the external clipboard.
***************************************************************************/
void CLIP::Import(void)
{
	AssertThis(0);

	if (pvNil != _pdocb && _pdocb->Cddg() > 0)
		Set();
	_fDocCurrent = fFalse;
}


/***************************************************************************
	Make sure the _pdocb is current - import the current system clipboard
	if it isn't.
***************************************************************************/
void CLIP::_EnsureDoc(void)
{
	if (_fExporting || _fImporting)
		{
		Bug("can't import the clipboard while exporting or importing");
		return;
		}

	if (_fDocCurrent)
		return;

	_fDocCurrent = fTrue;

#ifdef WIN
	HN hn;
	PDOCB pdocb;
	bool fDelay;
	long clfm;

	if (GetClipboardOwner() == vwig.hwndApp || !OpenClipboard(vwig.hwndApp))
		return;

	for (clfm = 0; 0 != (clfm = EnumClipboardFormats(clfm)); )
		{
		if (!_FImportFormat(clfm))
			continue;

		if (hNil == (hn = GetClipboardData(clfm)))
			continue;

		pdocb = pvNil;
		fDelay = fFalse;
		if (_FImportFormat(clfm, GlobalLock(hn), GlobalSize(hn),
				&pdocb, &fDelay))
			{
			GlobalUnlock(hn);
			Set(pdocb, fFalse);
			_fDelayImport = FPure(fDelay) && pdocb != pvNil;
			_clfmImport = clfm;
			ReleasePpo(&pdocb);
			break;
			}
		GlobalUnlock(hn);
		}
	CloseClipboard();
#endif //WIN

#ifdef MAC
	RawRtn(); //REVIEW shonk: Mac: implement CLIP::Import
#endif //MAC
}


/***************************************************************************
	Import the actual data for the current clipboard (if importing was
	delayed).
***************************************************************************/
void CLIP::_ImportCur(void)
{
	AssertThis(0);
	Assert(_fDocCurrent, 0);

	if (pvNil == _pdocb || !_fDelayImport || clfmNil == _clfmImport)
		return;

#ifdef WIN
	HN hn;
	bool fRet;
	PDOCB pdocb;

	if (GetClipboardOwner() == vwig.hwndApp || !OpenClipboard(vwig.hwndApp))
		return;

	if (hNil != (hn = GetClipboardData(_clfmImport)))
		{
		pdocb = _pdocb;
		if (pvNil != pdocb)
			pdocb->AddRef();
		fRet = _FImportFormat(_clfmImport, GlobalLock(hn), GlobalSize(hn),
			&pdocb);
		GlobalUnlock(hn);
		if (fRet)
			Set(pdocb, fFalse);
		_fDelayImport = fFalse;
		_clfmImport = clfmNil;
		ReleasePpo(&pdocb);
		}
	CloseClipboard();
#endif //WIN

#ifdef MAC
	RawRtn(); //REVIEW shonk: Mac: implement CLIP::_ImportCur
#endif //MAC
}


/***************************************************************************
	Import a particular format.
***************************************************************************/
bool CLIP::_FImportFormat(long clfm, void *pv, long cb,
	PDOCB *ppdocb, bool *pfDelay)
{
	AssertThis(0);
	AssertPvCb(pv, cb);
	AssertNilOrVarMem(ppdocb);
	AssertNilOrVarMem(pfDelay);
	Assert(_fDocCurrent, 0);
	bool fRet;

#ifdef WIN
	if (pvNil != pv && cb > 0)
		{
		// adjust cb for text - remove the stupid trailing null and compensate
		// for the fact that cb might be too big (GlobalSize(hn) might be bigger
		// than what was allocated).
		switch (clfm)
			{
		case kclfmUniText:
			wchar *pchw, *pchwLim;

			pchw = (wchar *)pv;
			pchwLim = (wchar *)pv + cb / size(wchar);
			while (pchw < pchwLim && *pchw != 0)
				pchw++;
			cb = BvSubPvs(pchw, pv);
			break;

		case kclfmSbText:
			schar *pchs, *pchsLim;

			pchs = (schar *)pv;
			pchsLim = (schar *)pv + cb;
			while (pchs < pchsLim && *pchs != 0)
				pchs++;
			cb = BvSubPvs(pchs, pv);
			break;
			}
		}
#endif //WIN

	_fImporting = fTrue;
	fRet = vpappb->FImportClip(clfm, pv, cb, ppdocb, pfDelay);
    _fImporting = fFalse;

	return fRet;
}


/***************************************************************************
	Start an export session.
***************************************************************************/
bool CLIP::FInitExport(void)
{
	AssertThis(0);

	if (_fExporting || _fImporting)
		{
		Bug("Already exporting or importing");
		return fFalse;
		}

	_hnExport = hNil;

#ifdef WIN
	if (!OpenClipboard(vwig.hwndApp))
		return fFalse;
	if (!EmptyClipboard())
		{
		CloseClipboard();
		return fFalse;
		}
	_fExporting = fTrue;
#endif //WIN

#ifdef MAC
	RawRtn(); //REVIEW shonk: Mac: implement CLIP::FInitExport
#endif //MAC

	return _fExporting;
}


/***************************************************************************
	Allocate a buffer to export to.
***************************************************************************/
void *CLIP::PvExport(long cb, long clfm)
{
	AssertThis(0);
	AssertIn(cb, 1, kcbMax);

	if (!_fExporting)
		{
		Bug("not exporting");
		return pvNil;
		}

	_ExportCur();

#ifdef WIN
	switch (clfm)
		{
	case kclfmText:
		// need to add size(achar) for the terminating zero character
		cb = LwRoundAway(cb + size(achar), size(achar));
		break;
		}

	if (hNil == (_hnExport = GlobalAlloc(
			GMEM_DDESHARE | GMEM_MOVEABLE | GMEM_ZEROINIT, cb)))
		{
		return pvNil;
		}

	_clfmExport = clfm;
	return GlobalLock(_hnExport);
#endif //WIN

#ifdef MAC
	RawRtn(); //REVIEW shonk: Mac: implement CLIP::PvExport
	return pvNil;
#endif //MAC
}


/***************************************************************************
	End an exporting session.
***************************************************************************/
void CLIP::EndExport(void)
{
	AssertThis(0);

	if (!_fExporting)
		return;

	_ExportCur();

#ifdef WIN
	CloseClipboard();
#endif //WIN

#ifdef MAC
	RawRtn(); //REVIEW shonk: Mac: implement CLIP::EndExport
#endif //MAC
	_fExporting = fFalse;
}


/***************************************************************************
	Export the current format.
***************************************************************************/
void CLIP::_ExportCur(void)
{
	AssertThis(0);
	Assert(_fExporting, 0);

#ifdef WIN
	if (hNil != _hnExport)
		{
		GlobalUnlock(_hnExport);
		SetClipboardData(_clfmExport, _hnExport);
		_hnExport = hNil;
		}
#endif //WIN

#ifdef MAC
	RawRtn(); //REVIEW shonk: Mac: implement CLIP::_ExportCur
#endif //MAC
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a CLIP.
***************************************************************************/
void CLIP::AssertValid(ulong grf)
{
	CLIP_PAR::AssertValid(0);
	AssertNilOrPo(_pdocb, 0);
	Assert(!_fExporting || !_fImporting, "both importing and exporting!");
}


/***************************************************************************
	Mark memory for the CLIP.
***************************************************************************/
void CLIP::MarkMem(void)
{
	AssertValid(0);
	CLIP_PAR::MarkMem();
	MarkMemObj(_pdocb);
}
#endif //DEBUG

