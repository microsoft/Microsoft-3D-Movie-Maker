/***************************************************************************
	Author: ******
	Project: Kauai
	Copyright (c) Microsoft Corporation

	Spell checker support using CSAPI compliant spell checkers.

***************************************************************************/
#ifndef SPELL_H
#define SPELL_H

//REVIEW shonk: dictionary type on Mac
const FTG kftgDictionary = MacWin('DICT', 'DIC');

// include the standard spell checker API header
#ifdef WIN
#define NT
#endif //WIN

extern "C"
	{
	#include "csapi.h"
	};

enum
	{
	fsplcNil = 0,
	fsplcSuggestFromUserDict = soSuggestFromUserDict,
	fsplcIgnoreAllCaps = soIgnoreAllCaps,
	fsplcIgnoreMixedDigits = soIgnoreMixedDigits,
	fsplcIgnoreRomanNumerals = soIgnoreRomanNumerals,
	fsplcFindUncappedSentences = soFindUncappedSentences,
	fsplcFindMissingSpaces = soFindMissingSpaces,
	fsplcFindRepeatWord = soFindRepeatWord,
	fsplcFindExtraSpaces = soFindExtraSpaces,
	fsplcFindSpacesBeforePunc = soFindSpacesBeforePunc,
	fsplcFindSpacesAfterPunc = soFindSpacesAfterPunc,
	fsplcFindInitialNumerals = soFindInitialNumerals,
	fsplcQuickSuggest = soQuickSuggest,
	fsplcUseAllOpenUdr = soUseAllOpenUdr,
	fsplcSglStepSugg = soSglStepSugg,
	fsplcIgnoreSingleLetter = soIgnoreSingleLetter,
	};


typedef class SPLC *PSPLC;
#define SPLC_PAR BASE
#define kclsSPLC 'SPLC'
class SPLC : public SPLC_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:
	bool _fSplidValid: 1;
	bool _fMdrsValid: 1;
	bool _fUdrValid: 1;
	ulong _splid;
	SC_MDRS _mdrs;
	SC_UDR _udr;

	achar _rgchSuggest[512];
	long _ichSuggest;

	#ifdef WIN
		HINSTANCE _hlib;

		SC_SEC (__cdecl *_pfnInit)(SC_SPLID *psplid, SC_WSC *pwsc);
		SC_SEC (__cdecl *_pfnOptions)(SC_SPLID splid, long grfso);
		SC_SEC (__cdecl *_pfnCheck)(SC_SPLID splid, SC_CC sccc, LPSC_SIB psib,
			LPSC_SRB psrb);
		SC_SEC (__cdecl *_pfnTerminate)(SC_SPLID splid, SC_BOOL fForce);
		SC_SEC (__cdecl *_pfnOpenMdr)(SC_SPLID splid, LPSC_PATH ppath,
			LPSC_PATH ppathExclude, SC_BOOL fCreateExclude, SC_BOOL fCache,
			SC_LID lidExpected, LPSC_MDRS pmdrs);
		SC_SEC (__cdecl *_pfnOpenUdr)(SC_SPLID splid, LPSC_PATH ppath,
			SC_BOOL fCreate, SC_WORD udrprop, SC_UDR * pudr, SC_BOOL *pfReadOnly);
		SC_SEC (__cdecl *_pfnAddUdr)(SC_SPLID splid, SC_UDR udr, SC_CHAR *pszAdd);
		SC_SEC (__cdecl *_pfnAddChangeUdr)(SC_SPLID splid, SC_UDR udr,
			SC_CHAR *pszAdd, SC_CHAR *pszChange);
		SC_SEC (__cdecl *_pfnClearUdr)(SC_SPLID splid, SC_UDR udr);
		SC_SEC (__cdecl *_pfnCloseMdr)(SC_SPLID splid, LPSC_MDRS pmdrs);
		SC_SEC (__cdecl *_pfnCloseUdr)(SC_SPLID splid, SC_UDR udr,
			SC_BOOL fForce);

		//SC_SEC SpellVer(SC_WORD *pwVer, SC_WORD *pwEngine, SC_WORD *pwType);
		SC_SEC SpellInit(SC_SPLID *psplid, SC_WSC *pwsc);
		SC_SEC SpellOptions(SC_SPLID splid, long grfso);
		SC_SEC SpellCheck(SC_SPLID splid, SC_CC sccc, LPSC_SIB psib, LPSC_SRB psrb);
		SC_SEC SpellTerminate(SC_SPLID splid, SC_BOOL fForce);
		//SC_SEC SpellVerifyMdr(LPSC_PATH ppath, SC_LID lidExpected, SC_LID *plid);
		SC_SEC SpellOpenMdr(SC_SPLID splid, LPSC_PATH ppath,
			LPSC_PATH ppathExclude, SC_BOOL fCreateExclude, SC_BOOL fCache,
			SC_LID lidExpected, LPSC_MDRS pmdrs);
		SC_SEC SpellOpenUdr(SC_SPLID splid, LPSC_PATH ppath, SC_BOOL fCreate,
			SC_WORD udrprop, SC_UDR * pudr, SC_BOOL *pfReadOnly);
		SC_SEC SpellAddUdr(SC_SPLID splid, SC_UDR udr, SC_CHAR *pszAdd);
		SC_SEC SpellAddChangeUdr(SC_SPLID splid, SC_UDR udr,
			SC_CHAR *pszAdd, SC_CHAR *pszChange);
		//SC_SEC SpellDelUdr(SC_SPLID splid, SC_UDR udr, SC_CHAR *pszDel);
		SC_SEC SpellClearUdr(SC_SPLID splid, SC_UDR udr);
		//SC_SEC SpellGetSizeUdr(SC_SPLID splid, SC_UDR udr, int *pcsz);
		//SC_SEC SpellGetListUdr(SC_SPLID splid, SC_UDR udr, SC_WORD iszStart,
		//	LPSC_SRB psrb);
		SC_SEC SpellCloseMdr(SC_SPLID splid, LPSC_MDRS pmdrs);
		SC_SEC SpellCloseUdr(SC_SPLID splid, SC_UDR udr, SC_BOOL fForce);
	#endif //WIN

	SPLC(void);
	virtual bool _FInit(SC_LID sclid, PSTN pstnCustom = pvNil);
	virtual bool _FEnsureDll(SC_LID sclid);
	virtual bool _FEnsureMainDict(SC_LID sclid, PFNI pfniDic = pvNil);
	virtual bool _FEnsureUserDict(PSTN pstnCustom, PFNI pfniDef = pvNil);
	virtual bool _FLoadDictionary(SC_LID sclid, PSZ psz, SC_MDRS *pmdrs);
	virtual bool _FLoadUserDictionary(PSZ psz, SC_UDR *pudr,
		bool fCreate = fFalse);

public:
	~SPLC(void);
	static PSPLC PsplcNew(SC_LID sclid, PSTN pstnCustom = pvNil);

	virtual bool FSetOptions(ulong grfsplc);
	virtual bool FCheck(achar *prgch, long cch,
		long *pichMinBad, long *pichLimBad, PSTN pstn, long *pscrs);
	virtual bool FSuggest(achar *prgch, long cch, bool fFirst, PSTN pstn);

	virtual bool FIgnoreAll(PSTN pstn);
	virtual bool FChange(PSTN pstnSrc, PSTN pstnDst, bool fAll);
	virtual bool FAddToUser(PSTN pstn);

	virtual void FlushIgnoreList(void);
	virtual void FlushChangeList(bool fAll);
	};

#endif //!SPELL_H

