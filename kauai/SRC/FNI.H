/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	File name handling.

***************************************************************************/
#ifndef FNI_H
#define FNI_H

#ifdef MAC
typedef FSSpec FSS;
#endif //MAC


enum
	{
	ffniNil = 0x0000,
	ffniCreateDir = 0x0001,
	ffniMoveToDir = 0x0002,

// for FNI::AssertValid
#ifdef DEBUG
	ffniFile = 0x10000,
	ffniDir = 0x20000,
	ffniEmpty = 0x40000,
#endif
	};

// Volume kinds:
enum
	{
	fvkNil = 0x0000,
	fvkFloppy = 0x0001,
	fvkNetwork = 0x0002,
	fvkCD = 0x0004,
	fvkRemovable = 0x0008,
	};


typedef long FTG;		// file type

const FTG ftgNil = '...,';
const FTG kftgDir = '....';
const FTG kftgTemp = MacWin('temp', 'TMP');		// the standard temp file ftg
const FTG kftgText = MacWin('TEXT', 'TXT');

extern FTG vftgTemp;	// the ftg to use for temp files


/****************************************
	File name class
****************************************/
typedef class FNI *PFNI;
#define FNI_PAR BASE
#define kclsFNI 'FNI'
class FNI : public FNI_PAR
	{
	RTCLASS_DEC
	ASSERT

	friend class FIL;
	friend class FNE;

private:
	FTG _ftg;
	#ifdef MAC
		long _lwDir; //the directory id
		FSS _fss;
	#elif defined(WIN)
		STN _stnFile;
	#endif //WIN

	#ifdef WIN
		void _SetFtgFromName(void);
		long _CchExt(void);
		bool _FChangeLeaf(PSTN pstn);
	#endif //WIN

public:
	FNI(void);

	// building FNIs
	#ifdef MAC
		bool FGetOpen(FTG *prgftg, short cftg);
		bool FGetSave(FTG ftg, PST pstPrompt, PST pstDefault);
		bool FBuild(long lwVol, long lwDir, PSTN pstn, FTG ftg);
	#elif defined(WIN)
		bool FGetOpen(achar *prgchFilter, HWND hwndOwner);
		bool FGetSave(achar *prgchFilter, HWND hwndOwner);
		bool FSearchInPath(PSTN pstn, PSTN pstnEnv = pvNil);
	#endif //WIN
	bool FBuildFromPath(PSTN pstn, FTG ftgDef = ftgNil); //REVIEW shonk: Mac: implement
	bool FGetUnique(FTG ftg);
	bool FGetTemp(void);
	void SetNil(void);

	FTG Ftg(void);
	ulong Grfvk(void); // volume kind (floppy/net/CD/etc)
	bool FChangeFtg(FTG ftg);

	bool FSetLeaf(PSTN pstn, FTG ftg = ftgNil);
	void GetLeaf(PSTN pstn);
	void GetStnPath(PSTN pstn);

	bool TExists(void);
	bool FDelete(void);
	bool FRename(PFNI pfniNew);
	bool FEqual(PFNI pfni);

	bool FDir(void);
	bool FSameDir(PFNI pfni);
	bool FDownDir(PSTN pstn, ulong grffni);
	bool FUpDir(PSTN pstn, ulong grffni);
	};

#ifdef MAC
#define FGetFniOpenMacro(pfni, prgftg, cftg, prgchFilter, hwndOwner) \
	(pfni)->FGetOpen(prgftg, cftg)
#define FGetFniSaveMacro(pfni, ftg, pstPrompt, pstDef, prgchFilter, hwndOwner) \
	(pfni)->FGetSave(ftg, pstPrompt, pstDef)
#endif //MAC
#ifdef WIN
#define FGetFniOpenMacro(pfni, prgftg, cftg, prgchFilter, hwndOwner) \
	(pfni)->FGetOpen(prgchFilter, hwndOwner)
#define FGetFniSaveMacro(pfni, ftg, pstPrompt, pstDef, prgchFilter, hwndOwner) \
	(pfni)->FGetSave(prgchFilter, hwndOwner)
#endif //WIN


/****************************************
	File name enumerator.
****************************************/
const long kcftgFneBase = 20;

enum
	{
	ffneNil = 0,
	ffneRecurse = 1,

	//for FNextFni
	ffnePre = 0x10,
	ffnePost = 0x20,
	ffneSkipDir = 0x80,
	};

#define FNE_PAR BASE
#define kclsFNE 'FNE'
class FNE : public FNE_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	NOCOPY(FNE)

private:
	//file enumeration state
	struct FES
		{
		#ifdef MAC
			long lwVol;
			long lwDir;
			long iv;
		#endif //MAC
		#ifdef WIN
			FNI fni;		//directory fni
			HN hn;			//for enumerating files/directories
			WIN32_FIND_DATA wfd;
			ulong grfvol;	//which volumes are available (for enumerating volumes)
			long chVol;		//which volume we're on (for enumerating volumes)
		#endif //WIN
		};

	FTG _rgftg[kcftgFneBase];
	FTG *_prgftg;
	long _cftg;
	bool _fRecurse: 1;
	bool _fInited: 1;
	PGL _pglfes;
	FES _fesCur;

	void _Free(void);
	#ifdef WIN
		bool _FPop(void);
	#endif //WIN

public:
	FNE(void);
	~FNE(void);

	bool FInit(FNI *pfniDir, FTG *prgftg, long cftg, ulong grffne = ffneNil);
	bool FNextFni(FNI *pfni, ulong *pgrffneOut = pvNil, ulong grffneIn = ffneNil);
	};


#endif //!FNI_H

