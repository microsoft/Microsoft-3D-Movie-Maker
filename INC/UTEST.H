/***************************************************************************

	utest.h: Socrates main app class

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

***************************************************************************/
#ifndef UTEST_H
#define UTEST_H

/****************************************
	KidWorld for the App class
****************************************/
typedef class KWA *PKWA;
#define KWA_PAR WOKS
#define kclsKWA 'KWA'
class KWA : public KWA_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

protected:
	PMBMP _pmbmp;	// MBMP to draw in KWA (may be pvNil)
	bool _fAskForCD;
	
public:
	KWA(GCB *pgcb) : WOKS(pgcb) { _fAskForCD = fTrue; }
	~KWA(void);
	virtual void Draw(PGNV pgnv, RC *prcClip);
	virtual bool FFindFile(PSTN pstnSrc, PFNI pfni); // for finding AVIs
	virtual bool FModalTopic(PRCA prca, CNO cnoTopic, long *plwRet);
	void SetMbmp(PMBMP pmbmp);
	void SetCDPrompt(bool fAskForCD) { _fAskForCD = fAskForCD; }
	bool FAskForCD(void) { return _fAskForCD; }
	};


//
// If you change anything for the registry, notify SeanSe for setup changes.
//
#define kszSocratesKey PszLit("Software\\Microsoft\\Microsoft Kids\\3D Movie Maker")
#define kszWaveOutMsgValue PszLit("WaveOutMsg")
#define kszMidiOutMsgValue PszLit("MidiOutMsg")
#define kszGreaterThan8bppMsgValue PszLit("GreaterThan8bppMsg")
#define kszSwitchResolutionValue PszLit("SwitchResolution")
#define kszHomeDirValue PszLit("HomeDirectory")
#define kszInstallDirValue PszLit("InstallDirectory")
#define kszProductsKey PszLit("Software\\Microsoft\\Microsoft Kids\\3D Movie Maker\\Products")
#define kszUserDataValue PszLit("UserData")
#define kszBetterSpeedValue PszLit("BetterSpeed")


// FGetSetRegKey flags
enum
	{
	fregNil = 0,
	fregSetKey = 0x01,
	fregSetDefault = 0x02,
	fregString = 0x04,
	fregBinary = 0x08,	// not boolean
	fregMachine = 0x10
	};

/****************************************
	The app class
****************************************/
typedef class APP *PAPP;
#define APP_PAR APPB
#define kclsAPP 'APP'
class APP : public APP_PAR
	{
	RTCLASS_DEC
	CMD_MAP_DEC(APP)
	ASSERT
	MARKMEM

protected:
	bool _fDontReportInitFailure; // init failure was already reported
	bool _fOnscreenDrawing;
	PCFL _pcfl; 			// resource file for app
	PSTDIO _pstdio;			// Current studio
	PTATR _ptatr;			// Current theater
	PCRM _pcrmAll;			// The app CRM -- all crfs are loaded into this.
	PGL _pglicrfBuilding;	// List of crfs in _pcrmAll belonging to Building.
	PGL _pglicrfStudio;		// List of crfs in _pcrmAll belonging to Studio.
	bool _fDontMinimize:1,  // "/M" command-line switch
	    _fSlowCPU:1,        // running on slow CPU
	    _fSwitchedResolution:1,  // we successfully switched to 640x480 mode
	    _fMainWindowCreated:1,
	    _fMinimized:1,
	    _fRunInWindow:1,    // run in a window (as opposed to fullscreen)
	    _fFontError:1,      // Have we already seen a font error?
		_fInPortfolio:1;    // Is the portfolio active?
	PCEX _pcex;				// Pointer to suspended cex.
	FNI _fniPortfolioDoc;   // document last opened in portfolio
	PMVIE _pmvieHandoff;    // Stores movie for studio to use
	PKWA _pkwa;				// Kidworld for App
	PGST _pgstBuildingFiles;
	PGST _pgstStudioFiles;
	PGST _pgstSharedFiles;
	PGST _pgstApp;			// Misc. app global strings
	STN _stnAppName;		// App name
	STN _stnProductLong;	// Long version of product name
	STN _stnProductShort;	// Short version of product name
	STN _stnUser;			// User's name
	long _sidProduct;
	FNI _fniExe; 			// fni of this executable file
	FNI _fniMsKidsDir;		// e.g., \mskids
	FNI _fniUsersDir;		// e.g., \mskids\users
	FNI _fniMelanieDir;		// e.g., \mskids\users\melanie
	FNI _fniProductDir;		// e.g., \mskids\3dmovie or \mskids\otherproduct
	FNI _fniUserDir;		// User's preferred directory
	FNI _fni3DMovieDir;		// e.g., \mskids\3dMovie
	long _dypTextDef;		// Default text height

	long _cactDisable;		// disable count for keyboard accelerators
#ifdef BUG1085
	long _cactCursHide;     // hide count for cursor
	long _cactCursSav;      // saved count for cursor
#endif

	//
	//
	//
	bool _fDown;
	long _cactToggle;


#ifdef WIN
	HACCEL _haccel;
	HACCEL _haccelGlobal;
#endif

protected:
	bool _FAppAlreadyRunning(void);
	void _TryToActivateWindow(void);
	bool _FEnsureOS(void);
	bool _FEnsureAudio(void);
	bool _FEnsureVideo(void);
	bool _FEnsureColorDepth(void);
	bool _FEnsureDisplayResolution(void);
	bool _FDisplaySwitchSupported(void);
	void _ParseCommandLine(void);
	void _SkipToSpace(char **ppch);
	void _SkipSpace(char **ppch);
	bool _FEnsureProductNames(void);
	bool _FFindProductDir(PGST pgst);
	bool _FQueryProductExists(STN *pstnLong, STN *pstnShort, FNI *pfni);
	bool _FFindMsKidsDir(void);
	bool _FCantFindFileDialog(PSTN pstn);
	bool _FGetUserName(void);
	bool _FGetUserDirectories(void);
	bool _FReadUserData(void);
	bool _FWriteUserData(void);
	bool _FDisplayHomeLogo(void);
	bool _FDetermineIfSlowCPU(void);
	bool _FOpenResourceFile(void);
	bool _FInitKidworld(void);
	bool _FInitProductNames(void);
	bool _FReadTitlesFromReg(PGST *ppgst);
	bool _FInitTdt(void);
	PGST _PgstRead(CNO cno);
	bool _FReadStringTables(void);
	bool _FSetWindowTitle(void);
	bool _FInitCrm(void);
	bool _FAddToCrm(PGST pgstFiles, PCRM pcrm, PGL pglFiles);
	bool _FInitBuilding(void);
	bool _FInitStudio(PFNI pfniUserDoc, bool fFailIfDocOpenFailed = fTrue);
	void _GetWindowProps(long *pxp, long *pyp, long *pdxp, long *pdyp,
		DWORD *pdwStyle);
	void _RebuildMainWindow(void);
	bool _FSwitch640480(bool fTo640480);
	bool _FDisplayIs640480(void);
	bool _FShowSplashScreen(void);
	bool _FPlaySplashSound(void);
	PMVIE _Pmvie(void);
	void _CleanupTemp(void);
#ifdef WIN
	bool _FSendOpenDocCmd(HWND hwnd, PFNI pfniUserDoc);
	bool _FProcessOpenDocCmd(void);
#endif //WIN

	// APPB methods that we override
	virtual bool _FInit(ulong grfapp, ulong grfgob, long ginDef);
	virtual bool _FInitOS(void);
	virtual bool _FInitMenu(void) { return fTrue; } // no menubar
	virtual void _CopyPixels(PGNV pgvnSrc, RC *prcSrc, PGNV pgnvDst,
		RC *prcDst);
	virtual void _FastUpdate(PGOB pgob, PREGN pregnClip,
		ulong grfapp = fappNil, PGPT pgpt = pvNil);
	virtual void _CleanUp(void);
	virtual void _Activate(bool fActive);
	virtual bool _FGetNextEvt(PEVT pevt);
	#ifdef WIN
		virtual bool _FFrameWndProc(HWND hwnd, uint wm,	WPARAM wParam,
			LPARAM lw, long *plwRet);
	#endif // WIN


public:
	APP(void) { _dypTextDef = 0; }

	// Overridden APPB functions
	virtual void GetStnAppName(PSTN pstn);
	virtual long OnnDefVariable(void);
	virtual long DypTextDef(void);
	virtual bool TQuerySaveDoc(PDOCB pdocb, bool fForce);
	virtual void Quit(bool fForce);
	virtual void UpdateHwnd(HWND hwnd, RC *prc, ulong grfapp = fappNil);
	virtual void Run(ulong grfapp, ulong grfgob, long ginDef);
#ifdef BUG1085
	virtual void HideCurs(void);
	virtual void ShowCurs(void);

	// New cursor methods
	void PushCurs(void);
	void PopCurs(void);
#endif // BUG 1085

	// Command processors
	bool FCmdLoadStudio(PCMD pcmd);
	bool FCmdLoadBuilding(PCMD pcmd);
	bool FCmdTheaterOpen(PCMD pcmd);
	bool FCmdTheaterClose(PCMD pcmd);
	bool FCmdIdle(PCMD pcmd);
	bool FCmdInfo(PCMD pcmd);
	bool FCmdPortfolioClear(PCMD pcmd);
	bool FCmdPortfolioOpen(PCMD pcmd);
	bool FCmdDisableAccel(PCMD pcmd);
	bool FCmdEnableAccel(PCMD pcmd);
	bool FCmdInvokeSplot(PCMD pcmd);
	bool FCmdExitStudio(PCMD pcmd);
	bool FCmdDeactivate(PCMD pcmd);

	static bool FInsertCD(PSTN pstnTitle);
	void DisplayErrors(void);
	void SetPortfolioDoc(PFNI pfni) { _fniPortfolioDoc = *pfni; }
	void GetPortfolioDoc(PFNI pfni) { *pfni = _fniPortfolioDoc; }
	void SetFInPortfolio(bool fInPortfolio) { _fInPortfolio = fInPortfolio; }
	bool FInPortfolio(void) { return _fInPortfolio; }

	PSTDIO Pstdio(void) { return _pstdio; }
	PKWA Pkwa(void) { return _pkwa; }
	PCRM PcrmAll(void) { return _pcrmAll; }
	bool FMinimized() { return _fMinimized; }

	bool FGetStnApp(long ids, PSTN pstn)
		{ return _pgstApp->FFindExtra(&ids, pstn); }
	void GetStnProduct(PSTN pstn) { *pstn = _stnProductLong; }
	void GetStnUser(PSTN pstn) { *pstn = _stnUser; }
	void GetFniExe(PFNI pfni) { *pfni = _fniExe; }
	void GetFniProduct(PFNI pfni) { *pfni = _fniProductDir; }
	void GetFniUsers(PFNI pfni) { *pfni = _fniUsersDir; }
	void GetFniUser(PFNI pfni) { *pfni = _fniUserDir; }
	void GetFniMelanie(PFNI pfni) { *pfni = _fniMelanieDir; }
	long SidProduct(void) { return _sidProduct; }
	bool FGetOnn(PSTN pstn, long *ponn);
	void MemStat(long *pdwTotalPhys, long *pdwAvailPhys = pvNil);
	bool FSlowCPU(void) { return _fSlowCPU; }

	// Kid-friendly modal dialog stuff:
	void EnsureInteractive(void)
	{
#ifdef WIN
		if (_fMinimized || GetForegroundWindow() != vwig.hwndApp)
#else
		if (_fMinimized)
#endif
			{
#ifdef WIN
			SetForegroundWindow(vwig.hwndApp);
			ShowWindow(vwig.hwndApp, SW_RESTORE);
#else //!WIN
			RawRtn();
#endif //!WIN
			}
	}
	bool TModal(PRCA prca, long tpc, PSTN pstnBackup = pvNil,
		long bkBackup = ivNil, long stidSubst = ivNil,
		PSTN pstnSubst = pvNil);

	// Enable/disable accelerator keys
	void DisableAccel(void);
	void EnableAccel(void);
	
	// Registry access function
	bool FGetSetRegKey(PSZ pszValueName, void *pvData, long cbData,
		ulong grfreg = fregSetDefault, bool *pfNoValue = pvNil);

	// Movie handoff routines
	void HandoffMovie(PMVIE pmvie);
	PMVIE PmvieRetrieve(void);

	// Determines whether screen savers should be blocked.
	virtual bool FAllowScreenSaver(void);
	};

#define vpapp ((APP *)vpappb)

#endif //!UTEST_H

