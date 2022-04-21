/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Clipboard object declarations.

***************************************************************************/
#ifndef CLIP_H
#define CLIP_H


/***************************************************************************
	Clipboard object.
***************************************************************************/
typedef class CLIP *PCLIP;
#define CLIP_PAR BASE
#define kclsCLIP 'CLIP'
class CLIP : public CLIP_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PDOCB _pdocb;

	bool _fDocCurrent: 1;
	bool _fExporting: 1;
	bool _fImporting: 1;
	bool _fDelayImport: 1;

	HN _hnExport;
	long _clfmExport;
	long _clfmImport;

	void _EnsureDoc();
	void _ExportCur(void);
	void _ImportCur(void);
	bool CLIP::_FImportFormat(long clfm, void *pv = pvNil, long cb = 0,
		PDOCB *ppdocb = pvNil, bool *pfDelay = pvNil);

public:
	CLIP(void);

	bool FDocIsClip(PDOCB pdocb);
	void Show(void);

	void Set(PDOCB pdocb = pvNil, bool fExport = fTrue);
	bool FGetFormat(long cls, PDOCB *pdocb = pvNil);

	bool FInitExport(void);
	void *PvExport(long cb, long clfm);
	void EndExport(void);

	void Import(void);
	};


extern PCLIP vpclip;

const long clfmNil = 0;
//REVIEW shonk: Mac unicode
const long kclfmUniText = MacWin('WTXT', CF_UNICODETEXT);
const long kclfmSbText = MacWin('TEXT', CF_TEXT);

#ifdef UNICODE
const long kclfmText = kclfmUniText;
#else //!UNICODE
const long kclfmText = kclfmSbText;
#endif //!UNICODE

#endif //!CLIP_H

