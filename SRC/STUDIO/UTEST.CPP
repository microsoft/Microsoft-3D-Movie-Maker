/***************************************************************************

	utest.cpp: Socrates main app class

	Primary Author: manders
	Review Status: REVIEWED - any changes to this file must be reviewed!

	The APP class handles initialization of the product, and global
	actions such as resolution-switching, switching between the building
	and the studio, and quitting.

	The KWA (App KidWorld) is the parent of the gob tree in the product.
	It also is used to display a splash screen, and to find AVIs on the CD.

***************************************************************************/
#include "studio.h"
#include "socres.h"
#include "mminstal.h"

ASSERTNAME

// If the following value is defined, 3DMM displays the dlidDesktopResizing 
// and dlidDesktopResized dialogs before and immediately after res-switching.
// The current thought is that these dialogs are unnecessary since we only
// res-switch to 640x480, which should always be safe.  We still display a
// dialog if the res-switch failed (dlidDesktopWontResize).
// #define RES_SWITCH_DIALOGS 


// If the following value is defined, 3DMM does a run-time performance test
// of the graphics, fixed-point math, and copying speed at startup and sets
// _fSlowCPU to fTrue if it thinks 3DMM is running on a slow computer.
// The current feeling is that we don't have the resources to tweak the 
// thresholds and verify that this gives us the result we want on all 
// computers.
//#define PERF_TEST

// Duration to display homelogo
const ulong kdtsHomeLogo = 4 * kdtsSecond;	

// Duration to display splash screen
const ulong kdtsSplashScreen = 4 * kdtsSecond;

// Duration before res-switch dialog cancels itself
const ulong kdtsMaxResSwitchDlg = 15 * kdtsSecond;

// 2MB cache per source for TAGM
const ulong kcbCacheTagm = 2048 * 1024;

static PSZ kpszAppWndCls = PszLit("3DMOVIE");
const PSZ kpszOpenFile = PszLit("3DMMOpen.tmp");

const long klwOpenDoc = 0x12123434; // arbitrary wParam for WM_USER

BEGIN_CMD_MAP(APP, APPB)
	ON_CID_GEN(cidInfo, FCmdInfo, pvNil)
	ON_CID_GEN(cidLoadStudio, FCmdLoadStudio, pvNil)
	ON_CID_GEN(cidLoadBuilding, FCmdLoadBuilding, pvNil)
	ON_CID_GEN(cidTheaterOpen, FCmdTheaterOpen, pvNil)
	ON_CID_GEN(cidTheaterClose, FCmdTheaterClose, pvNil)
	ON_CID_GEN(cidPortfolioOpen, FCmdPortfolioOpen, pvNil)
	ON_CID_GEN(cidPortfolioClear, FCmdPortfolioClear, pvNil)
	ON_CID_GEN(cidDisableAccel, FCmdDisableAccel, pvNil)
	ON_CID_GEN(cidEnableAccel, FCmdEnableAccel, pvNil)
	ON_CID_GEN(cidInvokeSplot, FCmdInvokeSplot, pvNil)
	ON_CID_GEN(cidExitStudio, FCmdExitStudio, pvNil)
	ON_CID_GEN(cidDeactivate, FCmdDeactivate, pvNil)
END_CMD_MAP_NIL()


APP vapp;
PTAGM vptagm;

RTCLASS(APP)
RTCLASS(KWA)

/***************************************************************************
	Entry point for a Kauai-based app.
***************************************************************************/
void FrameMain(void)
{
	Debug( vcactAV = 2; ) // Speeds up the debug build
	vapp.Run(fappOffscreen, fgobNil, kginMark);
}


/******************************************************************************
	Run
		Overridden APPB::Run method, so that we can attempt to recover
		gracefully from a crash.
	
	Arguments:
		ulong grfapp -- app flags
		ulong grfgob -- GOB flags
		long ginDef  -- default GOB invalidation
	
************************************************************ PETED ***********/
void APP::Run(ulong grfapp, ulong grfgob, long ginDef)
{
	/* Don't bother w/ AssertThis; we'd have to use AssertBaseThis, or
		possibly the parent's AssertValid, which gets done almost right
		away anyway */

	_CleanupTemp();

	__try
		{
		APP_PAR::Run(grfapp, grfgob, ginDef);
		}
	__except (UnhandledExceptionFilter(GetExceptionInformation()))
		{
		PDLG pdlg;

		pdlg = DLG::PdlgNew(dlidAbnormalExit, pvNil, pvNil);
		if (pdlg != pvNil)
			{
			pdlg->IditDo();
			ReleasePpo(&pdlg);
			}

		_fQuit = fTrue;
		MVU::RestoreKeyboardRepeat();
#ifdef WIN
		ClipCursor(NULL);
#endif // WIN
		_CleanUp();
		}
}


/***************************************************************************
	Get the name for the app.  Note that the string comes from a resource
	in the exe rather than a chunky file since the app name is needed
	before any chunky files are opened.  The app name should not change
	even for different products, i.e., Socrates and Playdo will have
	different _stnProduct, but the same _stnAppName.
***************************************************************************/
void APP::GetStnAppName(PSTN pstn)
{
	AssertBaseThis(0);
	AssertPo(pstn, 0);

	if (_stnAppName.Cch() == 0)
		{
#ifdef WIN
		SZ sz;

		if (0 != LoadString(vwig.hinst, stidAppName, sz, kcchMaxSz))
			_stnAppName = sz;
		else
			Warn("Couldn't read app name");
#else //MAC
		RawRtn();
#endif //MAC
		}
	*pstn = _stnAppName;
}


#ifdef DEBUG
struct DBINFO
	{
	long cactAV;
	};
#endif // DEBUG


/***************************************************************************
	Init the APP
***************************************************************************/
bool APP::_FInit(ulong grfapp, ulong grfgob, long ginDef)
{
	AssertBaseThis(0);

	ulong tsHomeLogo;
	ulong tsSplashScreen;
	FNI fniUserDoc;
	long fFirstTimeUser;

	// Only allow one copy of 3DMM to run at a time:
	if (_FAppAlreadyRunning())
		{
		_TryToActivateWindow();
		_fDontReportInitFailure = fTrue;
		goto LFail;
		}

#ifdef DEBUG
	{
	DBINFO dbinfo;

	dbinfo.cactAV = vcactAV;
	if (FGetSetRegKey(PszLit("DebugSettings"), &dbinfo, size(DBINFO),
			fregSetDefault | fregBinary))
		{
		vcactAV = dbinfo.cactAV;
		}
	}
#endif // DEBUG

	if (!_FEnsureOS())
		goto LFail;

	if (!_FEnsureAudio())
		goto LFail;

	if (!_FEnsureVideo())
		goto LFail;

	_ParseCommandLine();

	// If _ParseCommandLine doesn't set _stnProduct, set to 3dMovieMaker
	if (!_FEnsureProductNames())
		goto LFail;

	if (!_FFindMsKidsDir())
		goto LFail;

	// Init product names for tagman & _fniProductDir & potentially _stnProduct
	if (!_FInitProductNames())
		goto LFail;
					
	if (!_FOpenResourceFile())
		goto LFail;

	if (!_FReadStringTables())
		goto LFail;

	if (!_FEnsureDisplayResolution()) // may call _FInitOS
		goto LFail;
		
	if (!_FEnsureColorDepth())
		goto LFail;

	if (!APP_PAR::_FInit(grfapp, grfgob, ginDef)) // calls _FInitOS
		goto LFail;

	/* Ensure default font.  Do it here just so we get the error reported
		early. */
	OnnDefVariable();

	if (!_FInitKidworld())
		goto LFail;

	if (!_FDisplayHomeLogo())
		goto LFail;
	tsHomeLogo = TsCurrent();

	if (!_FDetermineIfSlowCPU())
		goto LFail;

	if (!_FInitTdt())
		goto LFail;

	if (!MTRL::FSetShadeTable(_pcfl, kctgTmap, 0))
		goto LFail;

	while (TsCurrent() - tsHomeLogo < kdtsHomeLogo)
		; // spin until home logo has been up long enough

	if (!_FShowSplashScreen())
		goto LFail;
	tsSplashScreen = TsCurrent();

	if (!_FPlaySplashSound())
		goto LFail;

	if (!_FGetUserName())
		goto LFail;

	if (!_FGetUserDirectories())
		goto LFail;

	if (!_FReadUserData())
		goto LFail;

	if (!_FInitCrm())
		goto LFail;

	while (TsCurrent() - tsSplashScreen < kdtsSplashScreen)
		; // spin until splash screen has been up long enough
	Pkwa()->SetMbmp(pvNil); // bring down splash screen

	// If the user specified a doc on the command line, go straight
	// to the studio.  Otherwise, start the building.
	GetPortfolioDoc(&fniUserDoc);

	if (fniUserDoc.Ftg() == ftgNil)
		{
		// Startup place depends on whether this is the user's first time in.
		if (!FGetProp(kpridFirstTimeUser, &fFirstTimeUser))
			goto LFail;

		if (!FSetProp(kpridBuildingGob, (fFirstTimeUser ? kgobCloset : kgobLogin)))
			goto LFail;
		if (!_FInitBuilding())
			goto LFail;
		}
	else
		{
		if (!FSetProp(kpridBuildingGob, kgobStudio1))
			goto LFail;
		if (!_FInitStudio(&fniUserDoc, fFalse))
			goto LFail;
		}

	EnsureInteractive();

	return fTrue;
LFail:
	_fQuit = fTrue;

	if (_fSwitchedResolution)
		{
		if (_FSwitch640480(fFalse)) // try to restore desktop
			_fSwitchedResolution = fFalse;
		}

	// _fDontReportInitFailure will be true if one of the above functions
	// has already posted an alert explaining why the app is shutting down.
	// If it's false, we put up a OOM or generic error dialog.
	if (!_fDontReportInitFailure)
		{
		PDLG pdlg;

		if (vpers->FIn(ercOomHq) || vpers->FIn(ercOomPv) || vpers->FIn(ercOomNew))
			pdlg = DLG::PdlgNew(dlidInitFailedOOM, pvNil, pvNil);
		else
			pdlg = DLG::PdlgNew(dlidInitFailed, pvNil, pvNil);
		if (pvNil != pdlg)
			pdlg->IditDo();
		ReleasePpo(&pdlg);
		}
	return fFalse;
}


/******************************************************************************
	_CleanupTemp
		Removes any temp files leftover from a previous execution.
	
************************************************************ PETED ***********/
void APP::_CleanupTemp(void)
{
	FNI fni;

	/* Attempt to cleanup any leftovers from a previous bad exit */
	vftgTemp = kftgSocTemp;
	if (fni.FGetTemp())
		{
		FTG ftgTmp = vftgTemp;
		FNE fne;

		if (fne.FInit(&fni, &ftgTmp, 1))
			{
			bool fFlushErs = fFalse;

			while (fne.FNextFni(&fni))
				{
				if (!fni.FDelete())
					fFlushErs = fTrue;
				}

			if (fFlushErs)
				vpers->Flush(ercFniDelete);
			}
		}
}


/***************************************************************************
	Determine if another copy of 3DMM is already running by trying to
	create a named semaphore.
***************************************************************************/
bool APP::_FAppAlreadyRunning(void)
{
	AssertBaseThis(0);
	
#ifdef WIN
	HANDLE hsem;
	STN stn;
	
	GetStnAppName(&stn);

	hsem = CreateSemaphore(NULL, 0, 1, stn.Psz());
	if (hsem != NULL && GetLastError() == ERROR_ALREADY_EXISTS)
		{
		CloseHandle(hsem);
		return fTrue;
		}
#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC
	return fFalse;
}


/***************************************************************************
	Try to find another instance of 3DMM and bring its window to front
	Also, if a document was on the command line, notify the other instance
***************************************************************************/
void APP::_TryToActivateWindow(void)
{
	AssertBaseThis(0);
	
#ifdef WIN
	HWND hwnd;
	STN stn;
	FNI fniUserDoc;
	
	GetStnAppName(&stn);	
	hwnd = FindWindow(kpszAppWndCls, stn.Psz());
	if (NULL != hwnd)
		{
		SetForegroundWindow(hwnd);
		ShowWindow(hwnd, SW_RESTORE); // in case it was minimized
		_ParseCommandLine();
		GetPortfolioDoc(&fniUserDoc);
		if (fniUserDoc.Ftg() != ftgNil)
			_FSendOpenDocCmd(hwnd, &fniUserDoc); // ignore failure
		}
#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC
}


/***************************************************************************
	If we're not running on at least Win95 or NT 3.51, complain and return
	fFalse.
***************************************************************************/
bool APP::_FEnsureOS(void)
{
	AssertBaseThis(0);
#ifdef WIN
	DWORD dwVersion;
	byte bVersionMajor;
	byte bVersionMinor;
	PDLG pdlg;
	
	dwVersion = GetVersion();
	bVersionMinor = (byte)((dwVersion & 0x0000ff00) >> 8);
	bVersionMajor = (byte)(dwVersion & 0x000000ff);

	if (bVersionMajor >= 4 || (bVersionMajor == 3 && bVersionMinor >= 51))
		return fTrue;

	// Put up an alert: OS too old
	pdlg = DLG::PdlgNew(dlidBadOS, pvNil, pvNil);
	if (pvNil == pdlg)
		return fFalse;
	pdlg->IditDo();
	ReleasePpo(&pdlg);
	_fDontReportInitFailure = fTrue;
#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC
	return fFalse; // Bad OS
}

	
/***************************************************************************
	Notifies the user if there is no suitable wave-out and/or midi-out
	hardware.
***************************************************************************/
bool APP::_FEnsureAudio(void)
{
	AssertBaseThis(0);

#ifdef WIN
	long cwod; // count of wave-out devices
	long cmod; // count of midi-out devices
	bool fShowMessage;
	PDLG pdlg;


	cwod = waveOutGetNumDevs();
	if (cwod <= 0)
		{
		fShowMessage = fTrue;
		if (!FGetSetRegKey(kszWaveOutMsgValue, &fShowMessage,
				size(fShowMessage), fregSetDefault | fregMachine))
			{
			Warn("Registry query failed");
			}
		if (fShowMessage)
			{
			// Put up an alert: no waveout
			pdlg = DLG::PdlgNew(dlidNoWaveOut, pvNil, pvNil);
			if (pvNil == pdlg)
				return fFalse;
			pdlg->IditDo();
			fShowMessage = !pdlg->FGetCheck(1); // 1 is ID of checkbox
			ReleasePpo(&pdlg);
			if (!fShowMessage)
				{
				// ignore failure
				FGetSetRegKey(kszWaveOutMsgValue, &fShowMessage,
					size(fShowMessage), fregSetKey | fregMachine);
				}
			}
		}
	if (HWD_SUCCESS == wHaveWaveDevice(WAVE_FORMAT_2M08)) // 22kHz, Mono, 8bit is our minimum
		{
		if(wHaveACM())
			{
			// audio compression manager (sound mapper) not installed
			wInstallComp(IC_ACM);
			_fDontReportInitFailure = fTrue;
			return fFalse;
			}

		if(wHaveACMCodec(WAVE_FORMAT_ADPCM))
			{
			// audio codecs not installed
			wInstallComp(IC_ACM_ADPCM);
			_fDontReportInitFailure = fTrue;
			return fFalse;
			}
		} // have wave device

	cmod = midiOutGetNumDevs();
	if (cmod <= 0)
		{
		fShowMessage = fTrue;
		if (!FGetSetRegKey(kszMidiOutMsgValue, &fShowMessage,
				size(fShowMessage), fregSetDefault | fregMachine))
			{
			Warn("Registry query failed");
			}
		if (fShowMessage)
			{
			// Put up an alert: no midiout
			pdlg = DLG::PdlgNew(dlidNoMidiOut, pvNil, pvNil);
			if (pvNil == pdlg)
				return fFalse;
			pdlg->IditDo();
			fShowMessage = !pdlg->FGetCheck(1); // 1 is ID of checkbox
			ReleasePpo(&pdlg);
			if (!fShowMessage)
				{
				// ignore failure
				FGetSetRegKey(kszMidiOutMsgValue, &fShowMessage,
					size(fShowMessage), fregSetKey | fregMachine);
				}
			}
		}
#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC
	return fTrue;
}

	
/***************************************************************************
	Notifies the user if there is no suitable video playback devices
***************************************************************************/
bool APP::_FEnsureVideo(void)
{
	if(wHaveMCI("AVIVIDEO"))
		{
		// MCI for video is not installed
		wInstallComp(IC_MCI_VFW);
		_fDontReportInitFailure = fTrue;
		return fFalse;
		}

	if(HIC_SUCCESS != wHaveICMCodec(MS_VIDEO1))
		{
		//  video 1 codec not installed
		wInstallComp(IC_ICM_VIDEO1);
		_fDontReportInitFailure = fTrue;
		return fFalse;
		}

	return fTrue;
}
	
/***************************************************************************
	Returns fTrue if color depth is >= 8 bits per pixel.  User is alerted
	if depth is less than 8, and function fails.  User is warned if depth
	is greater than 8, unless he has clicked the "don't show this messsage
	again" in the dialog before.
***************************************************************************/
bool APP::_FEnsureColorDepth(void)
{
	AssertBaseThis(0);

#ifdef WIN
	HDC hdc;
	long cbitPixel;
	PDLG pdlg;
	bool fShowMessage;
	bool fDontShowAgain = fFalse;
	
	hdc = GetDC(NULL);
	if (NULL == hdc)
		return fFalse;
	cbitPixel = GetDeviceCaps(hdc, BITSPIXEL);
	ReleaseDC(NULL, hdc);

	if (cbitPixel < 8)
		{
		// Put up an alert: Not enough colors
		pdlg = DLG::PdlgNew(dlidNotEnoughColors, pvNil, pvNil);
		if (pvNil == pdlg)
			return fFalse;
		pdlg->IditDo();
		ReleasePpo(&pdlg);
		_fDontReportInitFailure = fTrue;
		return fFalse;
		}
	if (cbitPixel > 8)
		{
		// warn user
		fShowMessage = fTrue;
		if (!FGetSetRegKey(kszGreaterThan8bppMsgValue, &fShowMessage,
				size(fShowMessage), fregSetDefault))
			{
			Warn("Registry query failed");
			}
		if (fShowMessage)
			{
			pdlg = DLG::PdlgNew(dlidTooManyColors, pvNil, pvNil);
			if (pvNil != pdlg)
				{
				pdlg->IditDo();
				fDontShowAgain = pdlg->FGetCheck(1); // 1 is ID of checkbox
				}
			ReleasePpo(&pdlg);
			if (fDontShowAgain)
				{
				fShowMessage = fFalse;
				FGetSetRegKey(kszGreaterThan8bppMsgValue, &fShowMessage,
					size(fShowMessage), fregSetKey); // ignore failure
				}
			}
		}
#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC
	return fTrue;
}


/***************************************************************************
	Dialog proc for Resolution Switch dialog.  Return fTrue (bring down
	the dialog) if kdtsMaxResSwitchDlg has passed since the dialog was
	created.
***************************************************************************/
bool _FDlgResSwitch(PDLG pdlg, long *pidit, void *pv)
{
	AssertPo(pdlg, 0);
	AssertVarMem(pidit);
	AssertPvCb(pv, size(long));

	long tsResize = *(long *)pv;
	long tsCur;

	if (*pidit != ivNil)
		{
		return fTrue; // Cancel or OK pressed
		}
	else
		{
		tsCur = TsCurrent();
		if (tsCur - tsResize >= kdtsMaxResSwitchDlg)
			return fTrue;
		}

	return fFalse;
}


/***************************************************************************
	Ensure that the screen is at the user's preferred resolution for 3DMM.
	If user has no registry preference, we offer to switch, try to switch,
	and save user's	preference.  Registry failures are non-fatal, but
	failing to create the main window causes this function to fail.

	When this function returns, _fRunInWindow is set correctly and the
	main app window *might* be created.
***************************************************************************/
bool APP::_FEnsureDisplayResolution(void)
{
	AssertBaseThis(0);

	PDLG pdlg;
	long idit;
	bool fSwitchRes;
	bool fNoValue;
	long tsResize;

	if (_FDisplayIs640480())
		{
		// System is already 640x480, so ignore registry and run fullscreen
		_fRunInWindow = fFalse;
		return fTrue;
		}

	if (!_FDisplaySwitchSupported())
		{
		// System can't switch res, so ignore registry and run in a window
		_fRunInWindow = fTrue;
		return fTrue;
		}

	// See if there's a res switch preference in the registry
	if (!FGetSetRegKey(kszSwitchResolutionValue, &fSwitchRes,
			size(fSwitchRes), fregNil, &fNoValue))
		{
		// Registry error...just run in a window
		_fRunInWindow = fTrue;
		return fTrue;
		}

	if (!fNoValue)
		{
		// User has a preference
		if (!fSwitchRes)
			{
			_fRunInWindow = fTrue;
			return fTrue;
			}
		else // try to switch res
			{
			_fRunInWindow = fFalse;
			if (!_FInitOS())
				{
				// we're screwed
				return fFalse;
				}
			if (!_FSwitch640480(fTrue))
				{
				_fRunInWindow = fTrue;
				_RebuildMainWindow();
				goto LSwitchFailed;
				}
			return fTrue;
			}
		}

	// User doesn't have a preference yet.  Do the interactive thing.
#ifdef RES_SWITCH_DIALOGS
	pdlg = DLG::PdlgNew(dlidDesktopResizing, pvNil, pvNil);
	if (pvNil == pdlg)
		return fFalse;
	idit = pdlg->IditDo();
	ReleasePpo(&pdlg);
#else //!RES_SWITCH_DIALOGS
	idit = 2; // OK
#endif //!RES_SWITCH_DIALOGS
	if (idit == 1) // cancel
		{
		_fRunInWindow = fTrue;
		// try to set pref to fFalse
		fSwitchRes = fFalse;
		goto LWriteReg;
		}
	_fRunInWindow = fFalse;
	if (!_FInitOS())
		return fFalse;
	if (!_FSwitch640480(fTrue))
		{
		_fRunInWindow = fTrue;
		_RebuildMainWindow();
		goto LSwitchFailed;
		}

	tsResize = TsCurrent();
#ifdef RES_SWITCH_DIALOGS
	pdlg = DLG::PdlgNew(dlidDesktopResized, _FDlgResSwitch, &tsResize);
	idit = ivNil; // if dialog fails to come up, treat like a cancel
	if (pvNil != pdlg)
		idit = pdlg->IditDo();
	ReleasePpo(&pdlg);
#else //!RES_SWITCH_DIALOGS
	idit = 2; // OK
#endif //!RES_SWITCH_DIALOGS
	if (idit == 1 || idit == ivNil) // cancel or timeout or dialog failure
		{
		_fSwitchedResolution = fFalse;
		_fRunInWindow = fTrue;
		_RebuildMainWindow();
		if (!_FSwitch640480(fFalse)) // restore desktop resolution
			return fFalse;
		goto LSwitchFailed;
		}
	// try to set pref to fTrue
	fSwitchRes = fTrue;
	goto LWriteReg;

LSwitchFailed:
	pdlg = DLG::PdlgNew(dlidDesktopWontResize, pvNil, pvNil);
	if (pvNil != pdlg)
		pdlg->IditDo();
	ReleasePpo(&pdlg);
	// try to set pref to fFalse
	fSwitchRes = fFalse;
LWriteReg:
	FGetSetRegKey(kszSwitchResolutionValue, &fSwitchRes, size(fSwitchRes),
		fregSetKey);
	return fTrue;
}


/***************************************************************************
	Return whether display is currently 640x480
***************************************************************************/
bool APP::_FDisplayIs640480(void)
{
#ifdef WIN
	return (GetSystemMetrics(SM_CXSCREEN) == 640 &&
		GetSystemMetrics(SM_CYSCREEN) == 480);
#endif //WIN
#ifdef MAC
	RawRtn();
	return fFalse;
#endif //MAC
}


/***************************************************************************
	Do OS specific initialization.
***************************************************************************/
bool APP::_FInitOS(void)
{
	AssertBaseThis(0);

#ifdef WIN
	long dxpWindow;
	long dypWindow;
	long xpWindow;
	long ypWindow;
	DWORD dwStyle = 0;
	STN stnWindowTitle;

	if (_fMainWindowCreated) // If someone else called _FInitOS already,
		return fTrue;		 // we can leave

	if (!FGetStnApp(idsWindowTitle, &stnWindowTitle))
		return fFalse;

	// register the window classes
	if (vwig.hinstPrev == hNil)
		{
		WNDCLASS wcs;

		wcs.style = CS_BYTEALIGNCLIENT | CS_OWNDC;
		wcs.lpfnWndProc = _LuWndProc;
		wcs.cbClsExtra = 0;
		wcs.cbWndExtra = 0;
		wcs.hInstance = vwig.hinst;
		wcs.hIcon = LoadIcon(vwig.hinst, MAKEINTRESOURCE(IDI_APP));
		wcs.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcs.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wcs.lpszMenuName = 0;
		wcs.lpszClassName = kpszAppWndCls;
		if (!RegisterClass(&wcs))
			return fFalse;

		wcs.lpfnWndProc = _LuMdiWndProc;
		wcs.lpszClassName = PszLit("MDI");
		if (!RegisterClass(&wcs))
			return fFalse;
		}

	_GetWindowProps(&xpWindow, &ypWindow, &dxpWindow, &dypWindow, &dwStyle);

	if ((vwig.hwndApp = CreateWindow(kpszAppWndCls, stnWindowTitle.Psz(),
		dwStyle, xpWindow, ypWindow, dxpWindow, dypWindow,
		hNil, hNil, vwig.hinst, pvNil)) == hNil)
		{
		return fFalse;
		}

	if (hNil == (vwig.hdcApp = GetDC(vwig.hwndApp)))
		return fFalse;

	//set a timer, so we can idle regularly.
	if (SetTimer(vwig.hwndApp, 0, 1, pvNil) == 0)
		return fFalse;

	_haccel = LoadAccelerators(vwig.hinst, MIR(acidMain));
	_haccelGlobal = LoadAccelerators(vwig.hinst, MIR(acidGlobal));
	vwig.haccel = _haccel;

	ShowWindow(vwig.hwndApp, vwig.wShow);
	_fMainWindowCreated = fTrue;
#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC
	return fTrue;
}


/***************************************************************************
	Determine window size, position, and window style.  Note that you
	should pass in the current dwStyle if the window already exists.  If
	the window does not already exist, pass in 0.
***************************************************************************/
void APP::_GetWindowProps(long *pxp, long *pyp, long *pdxp, long *pdyp,
	DWORD *pdwStyle)
{
	AssertBaseThis(0);
	AssertVarMem(pxp);
	AssertVarMem(pyp);
	AssertVarMem(pdxp);
	AssertVarMem(pdyp);
	AssertVarMem(pdwStyle);

	if (!_fRunInWindow)
		{
		*pdwStyle |= (WS_POPUP | WS_CLIPCHILDREN);
		*pdwStyle &= ~(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU |
			WS_MINIMIZEBOX);
		*pxp = 0;
		*pyp = 0;
		*pdxp = GetSystemMetrics(SM_CXSCREEN);
		*pdyp = GetSystemMetrics(SM_CYSCREEN);
		}
	else
		{
		RCS rcs;
		*pdwStyle |= (WS_OVERLAPPED | WS_CLIPCHILDREN | WS_CAPTION | WS_SYSMENU |
			WS_MINIMIZEBOX);
		*pdwStyle &= ~WS_POPUP;
		rcs.left = 0;
		rcs.top = 0;
		rcs.right = 640;
		rcs.bottom = 480;
		AdjustWindowRect(&rcs, *pdwStyle, fFalse);
		*pdxp = rcs.right - rcs.left;
		*pdyp = rcs.bottom - rcs.top;
		// Center window on screen
		// REVIEW manders: how do you adjust window appropriately for taskbar?
		*pxp = LwMax((GetSystemMetrics(SM_CXSCREEN) - *pdxp) / 2, 0);
		*pyp = LwMax((GetSystemMetrics(SM_CYSCREEN) - *pdyp) / 2, 0);
		}
}


/***************************************************************************
	Change main window properties based on whether we're running in a
	window or fullscreen.
***************************************************************************/
void APP::_RebuildMainWindow(void)
{
	AssertBaseThis(0);
	Assert(_fMainWindowCreated, 0);

#ifdef WIN
	long dxpWindow;
	long dypWindow;
	long xpWindow;
	long ypWindow;
	DWORD dwStyle;

	dwStyle = GetWindowLong(vwig.hwndApp, GWL_STYLE);
	_GetWindowProps(&xpWindow, &ypWindow, &dxpWindow, &dypWindow, &dwStyle);
	SetWindowLong(vwig.hwndApp, GWL_STYLE, dwStyle);
	SetWindowPos(vwig.hwndApp, HWND_TOP, xpWindow, ypWindow, dxpWindow,
		dypWindow, 0);
#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC
}


/***************************************************************************
	Open "3D Movie Maker.chk" or "3DMovie.chk"
***************************************************************************/
bool APP::_FOpenResourceFile(void)
{
	AssertBaseThis(0);
	AssertPo(&_fniProductDir, ffniDir);

	FNI fni;
	STN stn;

	fni = _fniProductDir;
	if (!fni.FSetLeaf(&_stnProductLong, kftgChunky) || tYes != fni.TExists())
		{
		fni = _fniProductDir;
		if (!fni.FSetLeaf(&_stnProductShort, kftgChunky) ||
			tYes != fni.TExists())
			{
			fni.GetStnPath(&stn);
	 		_FCantFindFileDialog(&stn); // ignore failure
			return fFalse;
			}
		}
	_pcfl = CFL::PcflOpen(&fni, fcflNil);
	if (pvNil == _pcfl)
		return fFalse;
	return fTrue;
}


/***************************************************************************
	Report that 3DMM can't find a file named pstnFile
***************************************************************************/
bool APP::_FCantFindFileDialog(PSTN pstnFile)
{
	AssertBaseThis(0);
	AssertPo(pstnFile, 0);

	PDLG pdlg;

	pdlg = DLG::PdlgNew(dlidCantFindFile, pvNil, pvNil);
	if (pvNil == pdlg)
		return fFalse;

	if (!pdlg->FPutStn(1, pstnFile))
		{
		ReleasePpo(&pdlg);
		return fFalse;
		}
	pdlg->IditDo();
	ReleasePpo(&pdlg);
	_fDontReportInitFailure = fTrue;
	return fTrue;
}


/******************************************************************************
	Finds _fniUsersDir, _fniMelanieDir, and _fniUserDir.  _fniUserDir is
	set from the registry.  This function also determines if this is a first
	time user.
************************************************************ PETED ***********/
bool APP::_FGetUserDirectories(void)
{
	AssertBaseThis(0);
	AssertPo(&_fniMsKidsDir, ffniDir);
	Assert(_stnUser.Cch() > 0, "need valid stnUser!");

	SZ szDir;
	STN stn;
	STN stnT;
	STN stnUsers;
	bool fFirstTimeUser;

	// First, find the Users directory
	_fniUsersDir = _fniMsKidsDir;
	if (!FGetStnApp(idsUsersDir, &stnUsers))
		return fFalse;
	if (!_fniUsersDir.FDownDir(&stnUsers, ffniMoveToDir))
		{
		_fniUsersDir.GetStnPath(&stn);
		if (!stn.FAppendStn(&stnUsers))
			return fFalse;
		_FCantFindFileDialog(&stn); // ignore failure
		return fFalse;
		}
	AssertPo(&_fniUsersDir, ffniDir);

	// Find Melanie's dir
	_fniMelanieDir = _fniUsersDir;
	if (!FGetStnApp(idsMelanie, &stn))
		return fFalse;
	if (!_fniMelanieDir.FDownDir(&stn, ffniMoveToDir))
		{
		_fniMelanieDir.GetStnPath(&stnT);
		if (!stnT.FAppendStn(&stn))
			return fFalse;
		_FCantFindFileDialog(&stnT); // ignore failure
		return fFalse;
		}
	AssertPo(&_fniMelanieDir, ffniDir);

	fFirstTimeUser = fFalse;
	szDir[0] = chNil;
	if (!FGetSetRegKey(kszHomeDirValue, szDir, size(szDir),
			fregSetDefault | fregString))
		{
		return fFalse;
		}
	stn.SetSz(szDir);
	if (stn.Cch() == 0 || !_fniUserDir.FBuildFromPath(&stn, kftgDir) ||
			tYes != _fniUserDir.TExists())
		{
		// Need to (find or create) and go to user's directory
		fFirstTimeUser = fTrue;
		_fniUserDir = _fniUsersDir;

		// Ensure that user's root directory exists
		if (!_fniUserDir.FDownDir(&_stnUser, ffniMoveToDir))
			{
			if (!_fniUserDir.FDownDir(&_stnUser, ffniCreateDir | ffniMoveToDir))
				return fFalse;
			}

		// Try to write path to user dir to the registry
		_fniUserDir.GetStnPath(&stn);
		stn.GetSz(szDir);
		FGetSetRegKey(kszHomeDirValue, szDir, CchSz(szDir) + kcchExtraSz,
			fregSetKey | fregString); // ignore failure
		}
#ifdef WIN
	if (SetCurrentDirectory(szDir) == FALSE)
		return fFalse;
#else //!WIN
	RawRtn();
#endif //!WIN
	AssertPo(&_fniUserDir, ffniDir);

	if (!FSetProp(kpridFirstTimeUser, fFirstTimeUser))
		return fFalse;
	return fTrue;
}


/******************************************************************************
	_FGetUserName
		Attempts to get the current user name.  May return a default value
		if we get a "non-serious" error (as defined in the spec).
	
	Arguments:
		None
	
	Returns:  fTrue if _stnUser has something usable in it on return
	
************************************************************ PETED ***********/
bool APP::_FGetUserName(void)
{
	AssertBaseThis(0);

#ifdef WIN
	bool fRet = fTrue;
	DWORD dwCbUser = size(SZ);
	SZ szT;

	switch (WNetGetUser(NULL, szT, &dwCbUser))
		{
	case ERROR_EXTENDED_ERROR:
		{
		DWORD dwError;
		SZ szProvider;
		STN stnMessage;

		if (WNetGetLastError(&dwError, szT, size(SZ), szProvider, size(SZ))
				== NO_ERROR)
			{
			STN stnFormat;

			if (FGetStnApp(idsWNetError, &stnFormat))
				{
				stnMessage.FFormat(&stnFormat, szProvider, dwError, szT);
				TGiveAlertSz(stnMessage.Psz(), bkOk, cokExclamation);
				}
			}
		else
			Bug("Call to WNetGetLastError failed; this should never happen");
		_stnUser.SetNil();
		fRet = fFalse;
		break;
		}

	case NO_ERROR:
		_stnUser = szT;
		if (_stnUser.Cch() > 0)
			break; // else fall through...

	case ERROR_MORE_DATA:
	case ERROR_NO_NETWORK:
	case ERROR_NO_NET_OR_BAD_PATH:
	default:
		if (!FGetStnApp(idsDefaultUser, &_stnUser))	
			fRet = fFalse;
		break;
		}

	Assert(!fRet || _stnUser.Cch() > 0, "Bug in _FGetUserName");
	return fRet;
#else // WIN
	RawRtn();
	return fFalse;
#endif // !WIN
}


/******************************
	The user-data structure
*******************************/
struct UDAT
	{
	long rglw[kcpridUserData];
	};

/***************************************************************************
	Reads the "user data" from the registry and SetProp's the data onto
	the app
***************************************************************************/
bool APP::_FReadUserData(void)
{
	AssertBaseThis(0);

	UDAT udat;
	long iprid;

	ClearPb(&udat, size(UDAT));

	if (!FGetSetRegKey(kszUserDataValue, &udat, size(UDAT),
		fregSetDefault | fregBinary))
		{
		return fFalse;
		}
	for (iprid = 0; iprid < kcpridUserData; iprid++)
		{
		if (!FSetProp(kpridUserDataBase + iprid, udat.rglw[iprid]))
			return fFalse;
		}

	return fTrue;
}


/***************************************************************************
	Writes the "user data" from the app props to the registry
***************************************************************************/
bool APP::_FWriteUserData(void)
{
	AssertBaseThis(0);

	UDAT udat;
	long iprid;

	for (iprid = 0; iprid < kcpridUserData; iprid++)
		{
		if (!FGetProp(kpridUserDataBase + iprid, &udat.rglw[iprid]))
			return fFalse;
		}
	if (!FGetSetRegKey(kszUserDataValue, &udat, size(UDAT),
		fregSetKey | fregBinary))
		{
		return fFalse;
		}

	return fTrue;
}


/******************************************************************************
	FGetSetRegKey
		Given a reg key (and option sub-key), attempts to either get or set
		the current value of the key.  If the reg key is created on a Get
		(fSetKey == fFalse), the data in pvData will be used to set the
		default value for the key.
	
	
	Arguments:
		PSZ pszValueName  --  The value name
		void *pvData      --  pointer to buffer to read or write key into or from
		long cbData       --  size of the buffer
		ulong grfreg      --  flags describing what we should do
		bool *pfNoValue  --  optional parameter, takes whether a real registry
		                      error occurred or not
	
	Returns:  fTrue if all actions necessary could be performed
	
************************************************************ PETED ***********/
bool APP::FGetSetRegKey(PSZ pszValueName, void *pvData, long cbData,
	ulong grfreg, bool *pfNoValue)
{
	AssertBaseThis(0);
	AssertSz(pszValueName);
	AssertPvCb(pvData, cbData);
	AssertNilOrVarMem(pfNoValue);

	bool fRet = fFalse;
	bool fSetKey, fSetDefault, fString, fBinary;
	
	fSetKey = grfreg & fregSetKey;
	fSetDefault = grfreg & fregSetDefault;
	fString = grfreg & fregString;
	fBinary = grfreg & fregBinary;

#ifdef WIN
	DWORD dwDisposition;
	DWORD dwCbData = cbData;
	DWORD dwType;
	HKEY hkey = 0;

	if (RegCreateKeyEx((grfreg & fregMachine) ? HKEY_LOCAL_MACHINE :
			HKEY_CURRENT_USER, kszSocratesKey, 0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hkey, &dwDisposition) != ERROR_SUCCESS)
		{
		goto LFail;
		}

	if ((dwDisposition == REG_CREATED_NEW_KEY && fSetDefault) || fSetKey)
		{
LWriteValue:
		if (fBinary)
			{
			dwType = REG_BINARY;
			}
		else if (fString)
			{
			if (!fSetKey)
				dwCbData = CchSz((PSZ)pvData) + kcchExtraSz;
			else
				Assert(CchSz((PSZ)pvData) < cbData, "Invalid string for reg key");
			dwType = REG_SZ;
			}
		else
			{
			Assert(cbData == size(DWORD), "Unknown reg key type");
			dwType = REG_DWORD;
			}

		if (RegSetValueEx(hkey, pszValueName, NULL, dwType, (byte *)pvData,
			dwCbData) != ERROR_SUCCESS)
			{
			goto LFail;
			}
		}
	else
		{
		long lwRet;

		if ((lwRet = RegQueryValueEx(hkey, pszValueName, NULL, &dwType,
				(byte *)pvData, &dwCbData)) != ERROR_SUCCESS)
			{
			if (lwRet == ERROR_FILE_NOT_FOUND && fSetDefault)
				goto LWriteValue;
			/* If the caller gave us a way to differentiate a genuine registry
				failure from simply not having set the value yet, do so */
			if (pfNoValue != pvNil)
				fRet = *pfNoValue = (lwRet == ERROR_FILE_NOT_FOUND);
			goto LFail;
			}
		Assert(dwType == (DWORD)(fString ? REG_SZ : (fBinary ? REG_BINARY : REG_DWORD)), "Invalid key type");
		}
	if (pfNoValue != pvNil)
		*pfNoValue = fFalse;
	fRet = fTrue;
LFail:
	if (hkey != 0)
		RegCloseKey(hkey);
#else // WIN
	RawRtn();
#endif // !WIN
	return fRet;
}


/***************************************************************************
	Set the palette and bring up the Microsoft Home Logo
***************************************************************************/
bool APP::_FDisplayHomeLogo(void)
{
	AssertBaseThis(0);
	AssertPo(_pcfl, 0);

	BLCK blck;
	PGL pglclr;
	PMBMP pmbmp;
	short bo;
	short osk;

	if (!_pcfl->FFind(kctgColorTable, kcnoGlcrInit, &blck))
		return fFalse;
	pglclr = GL::PglRead(&blck, &bo, &osk);
	if (pvNil == pglclr)
		return fFalse;
	GPT::SetActiveColors(pglclr, fpalIdentity);
	ReleasePpo(&pglclr);

	if (!_pcfl->FFind(kctgMbmp, kcnoMbmpHomeLogo, &blck))
		return fFalse;
	pmbmp = MBMP::PmbmpRead(&blck);
	if (pvNil == pmbmp)
		return fFalse;
	_pkwa->SetMbmp(pmbmp);
	ReleasePpo(&pmbmp);
	UpdateMarked();
	return fTrue;
}


/***************************************************************************
	Initialize tag manager
***************************************************************************/
bool APP::_FInitProductNames(void)
{
	AssertBaseThis(0);

	PGST pgst;
	BLCK blck;

	// Use kcbCacheTagm of cache per source, don't cache on CD
    vptagm = TAGM::PtagmNew(&_fniMsKidsDir, APP::FInsertCD, kcbCacheTagm);
 	if (vptagm == pvNil)
		return fFalse;

	if (!_FReadTitlesFromReg(&pgst))
		goto LFail;

	if (!vptagm->FMergeGstSource(pgst, kboCur, koskCur))
		goto LFail;

	if (!_FFindProductDir(pgst))
		goto LFail;

	if (!vptagm->FGetSid(&_stnProductLong, &_sidProduct))
		goto LFail;

	ReleasePpo(&pgst);
	return fTrue;

LFail:
	ReleasePpo(&pgst);
	return fFalse;
}


/***************************************************************************
	Read the sids and titles of installed 3DMovie products from the registry
***************************************************************************/
bool APP::_FReadTitlesFromReg(PGST *ppgst)
{
	AssertBaseThis(0);
	AssertVarMem(ppgst);

#ifdef WIN
	HKEY hkey = 0;
	DWORD dwDisposition;
	DWORD iValue;
	SZ szSid;
	STN stnSid;
	DWORD cchSid = kcchMaxSz;
	SZ szTitle;
	STN stnTitle;
	DWORD cchTitle = kcchMaxSz;
	PGST pgst;
	long sid;

	if ((pgst = GST::PgstNew(size(long))) == pvNil)
		goto LFail;
	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, kszProductsKey, 0, NULL, REG_OPTION_NON_VOLATILE,
			KEY_ALL_ACCESS, NULL, &hkey, &dwDisposition) != ERROR_SUCCESS)
		{
		Warn("Missing InstallDirectory registry entry or registry error");
		goto LFail;
		}
	
	for (iValue = 0; RegEnumValue(hkey, iValue, szSid, &cchSid, NULL, 
		NULL, (unsigned char *)szTitle, &cchTitle) != ERROR_NO_MORE_ITEMS; iValue++) 
		{
		stnSid.SetSz(szSid);
		if (!stnSid.FGetLw(&sid))
			{
			Warn("Invalid registry name for Products key value");
			continue;
			}
		stnTitle.SetSz(&szTitle[0]); 	
		if (!pgst->FAddStn(&stnTitle, &sid))
			goto LFail;
		cchTitle = kcchMaxSz;
		cchSid = kcchMaxSz;
		}

#else  //!WIN
	RawRtn();
#endif //!WIN
	*ppgst = pgst;
	return fTrue;

LFail:
	ReleasePpo(&pgst);
	*ppgst = pvNil;
	return fFalse;
}

/***************************************************************************
	Initialize 3-D Text
***************************************************************************/
bool APP::_FInitTdt(void)
{
	AssertBaseThis(0);
	AssertPo(_pcfl, 0);

	PGST pgst;

	// set TDT action names
	pgst = _PgstRead(kcnoGstAction);
	if (pvNil == pgst)
		return fFalse;
	Assert(pgst->CbExtra() == size(long), "bad Action string table");
	if (!TDT::FSetActionNames(pgst))
		{
		ReleasePpo(&pgst);
		return fFalse;
		}
	ReleasePpo(&pgst);
	return fTrue;
}


/***************************************************************************
	Read and byte-swap a GST from _pcfl.  Assumes that extra data, if any,
	is a long.
***************************************************************************/
PGST APP::_PgstRead(CNO cno)
{
	AssertBaseThis(0);
	AssertPo(_pcfl, 0);

	PGST pgst;
	BLCK blck;
	short bo;
	short osk;
	long istn;
	long lwExtra;

	if (!_pcfl->FFind(kctgGst, cno, &blck))
		return pvNil;
	pgst = GST::PgstRead(&blck, &bo, &osk);
	if (pvNil == pgst)
		return pvNil;
	Assert(pgst->CbExtra() == 0 || pgst->CbExtra() == size(long),
		"unexpected extra size");
	if (kboCur != bo && 0 != pgst->CbExtra())
		{
		for (istn = 0; istn < pgst->IvMac(); istn++)
			{
			pgst->GetExtra(istn, &lwExtra);
			SwapBytesRglw(&lwExtra, 1);
			pgst->PutExtra(istn, &lwExtra);
			}
		}
	return pgst;
}


/***************************************************************************
	Read various string tables from _pcfl
***************************************************************************/
bool APP::_FReadStringTables(void)
{
	AssertBaseThis(0);
	AssertPo(_pcfl, 0);

	// read studio filename list
	_pgstStudioFiles = _PgstRead(kcnoGstStudioFiles);
	if (pvNil == _pgstStudioFiles)
		return fFalse;

	// read building filename list
	_pgstBuildingFiles = _PgstRead(kcnoGstBuildingFiles);
	if (pvNil == _pgstBuildingFiles)
		return fFalse;

	// read shared filename list
	_pgstSharedFiles = _PgstRead(kcnoGstSharedFiles);
	if (pvNil == _pgstSharedFiles)
		return fFalse;

	// read misc app strings
	_pgstApp = _PgstRead(kcnoGstApp);
	if (pvNil == _pgstApp)
		return fFalse;

	return fTrue;
}


/***************************************************************************
	Initialize KWA (app kidworld)
***************************************************************************/
bool APP::_FInitKidworld(void)
{
	AssertBaseThis(0);

	RC rcRel(0, 0, krelOne, krelOne);
	GCB gcb(CMH::HidUnique(), GOB::PgobScreen(), fgobNil, kginMark, pvNil,
		&rcRel);

	_pkwa = NewObj KWA(&gcb);
	if (pvNil == _pkwa)
		return fFalse;

	return fTrue;
}


/***************************************************************************
	Show the app splash screen
***************************************************************************/
bool APP::_FShowSplashScreen(void)
{
	AssertBaseThis(0);

	BLCK blck;
	PMBMP pmbmp;

	if (!_pcfl->FFind(kctgMbmp, kcnoMbmpSplash, &blck))
		return fFalse;
	pmbmp = MBMP::PmbmpRead(&blck);
	if (pvNil == pmbmp)
		return fFalse;
	_pkwa->SetMbmp(pmbmp);
	ReleasePpo(&pmbmp);
	UpdateMarked();
	return fTrue;
}


/***************************************************************************
	Play the app splash sound
***************************************************************************/
bool APP::_FPlaySplashSound(void)
{
	AssertBaseThis(0);

	PCRF pcrf;

	pcrf = CRF::PcrfNew(_pcfl, 0);
	if (pvNil == pcrf)
		return fFalse;
	vpsndm->SiiPlay(pcrf, kctgMidi, kcnoMidiSplash);
	ReleasePpo(&pcrf);

	return fTrue;
}


/***************************************************************************
	Read the chunky files specified by _pgstSharedFiles, _pgstBuildingFiles
	and _pgstStudioFiles and create the global CRM; indices to the Building
	and Studio CRFs are stored in _pglicrfBuilding and _pglicrfStudio.
***************************************************************************/
bool APP::_FInitCrm(void)
{
	AssertBaseThis(0);
	AssertPo(_pkwa, 0);
	AssertPo(_pgstSharedFiles, 0);
	AssertPo(_pgstBuildingFiles, 0);
	AssertPo(_pgstStudioFiles, 0);

	PSCEG psceg = pvNil;
	PSCPT pscpt = pvNil;

	_pcrmAll = CRM::PcrmNew(_pgstSharedFiles->IvMac() +
		_pgstBuildingFiles->IvMac() + _pgstStudioFiles->IvMac());
	if (pvNil == _pcrmAll)
		goto LFail;

	_pglicrfBuilding = GL::PglNew(size(long), _pgstBuildingFiles->IvMac());
	if (pvNil == _pglicrfBuilding)
		goto LFail;

	_pglicrfStudio = GL::PglNew(size(long), _pgstStudioFiles->IvMac());
	if (pvNil == _pglicrfStudio)
		goto LFail;

	if (!_FAddToCrm(_pgstSharedFiles, _pcrmAll, pvNil))
		goto LFail;

	if (!_FAddToCrm(_pgstBuildingFiles, _pcrmAll, _pglicrfBuilding))
		goto LFail;

	if (!_FAddToCrm(_pgstStudioFiles, _pcrmAll, _pglicrfStudio))
		goto LFail;

	// Initialize the shared util gob.
	psceg = _pkwa->PscegNew(_pcrmAll, _pkwa);
	if (pvNil == psceg)
		goto LFail;

	pscpt = (PSCPT)_pcrmAll->PbacoFetch(kctgScript, kcnoInitShared,
		SCPT::FReadScript);
	if (pvNil == pscpt)
		goto LFail;

	if (!psceg->FRunScript(pscpt))
		goto LFail;

	ReleasePpo(&psceg);
	ReleasePpo(&pscpt);
	return fTrue;
LFail:
	ReleasePpo(&psceg);
	ReleasePpo(&pscpt);
	return fFalse;
}


/***************************************************************************
	Helper function for _FInitCrm.  Adds the list of chunky files specified
	in pgstFiles to the CRM pointed to by pcrm.  If pglFiles is not pvNil,
	it is filled in with the positions in the CRM of each of the loaded
	crfs.
***************************************************************************/
bool APP::_FAddToCrm(PGST pgstFiles, PCRM pcrm, PGL pglFiles)
{
	AssertBaseThis(0);
	AssertPo(&_fniProductDir, ffniDir);
	AssertPo(pgstFiles, 0);
	AssertPo(pcrm, 0);
	AssertNilOrPo(pglFiles, 0);

	bool fRet = fFalse;
	FNI fni;
	STN stn;
	long istn;
	long cbCache;
	PCFL pcfl = pvNil;
	long icfl;

	for (istn = 0; istn < pgstFiles->IvMac(); istn++)
		{
		pgstFiles->GetStn(istn, &stn);
		pgstFiles->GetExtra(istn, &cbCache);
#ifdef DEBUG
		{
		bool fAskForCDSav = Pkwa()->FAskForCD();
		bool fFoundFile;

		// In debug, we look for "buildingd.chk", "sharedd.chk", etc. and
		// use them instead of the normal files if they exist.  If they
		// don't exist, just use the normal files.
		STN stnT = stn;
		stnT.FAppendCh(ChLit('d'));
		stnT.FAppendSz(PszLit(".chk")); // REVIEW manders
		Pkwa()->SetCDPrompt(fFalse);
		fFoundFile = Pkwa()->FFindFile(&stnT, &fni);
		Pkwa()->SetCDPrompt(fAskForCDSav);
		if (fFoundFile)
			{
			pcfl = CFL::PcflOpen(&fni, fcflNil);
			}
		else
			{
#endif //DEBUG
			stn.FAppendSz(PszLit(".chk")); // REVIEW manders
			if (Pkwa()->FFindFile(&stn, &fni))
				pcfl = CFL::PcflOpen(&fni, fcflNil);
#ifdef DEBUG
			}
		}
#endif //DEBUG
		if (pvNil == pcfl)
			{
			if (fni.Ftg() != ftgNil)
				fni.GetStnPath(&stn);
			if (!_fDontReportInitFailure)
				_FCantFindFileDialog(&stn); // ignore failure
			goto LFail;
			}
 		if (!pcrm->FAddCfl(pcfl, cbCache, &icfl))
			goto LFail;
		ReleasePpo(&pcfl);
		if (pglFiles != pvNil && !pglFiles->FAdd(&icfl))
			goto LFail;
		}

	fRet = fTrue;
LFail:
	if (!fRet)
		ReleasePpo(&pcfl);
	return fRet;

}


/***************************************************************************
	Initialize and start the building script
***************************************************************************/
bool APP::_FInitBuilding(void)
{
	AssertBaseThis(0);

 	bool fRet = fFalse;
	long i;
	long cbCache;
	long iv;
	PCRF pcrfT;
	PSCEG psceg = pvNil;
	PSCPT pscpt = pvNil;

	BeginLongOp();

	psceg = _pkwa->PscegNew(_pcrmAll, _pkwa);
	if (pvNil == psceg)
		goto LFail;

	pscpt = (PSCPT)_pcrmAll->PbacoFetch(kctgScript, kcnoStartApp,
		SCPT::FReadScript);
	if (pvNil == pscpt)
		goto LFail;

	if (!psceg->FRunScript(pscpt))
		goto LFail;

	// Up the cache limits of the building crfs in the global crm.
	// Assumption: since the files were added to the crm in the order they
	// appear in _pgstBuilding, I can get the cache amounts from there.
	for (i = 0; i < _pglicrfBuilding->IvMac(); i++)
		{
		_pgstBuildingFiles->GetExtra(i, &cbCache);
		_pglicrfBuilding->Get(i, &iv);
		pcrfT = _pcrmAll->PcrfGet(iv);
		Assert(pcrfT != pvNil, "Main CRM is corrupt.");
		pcrfT->SetCbMax(cbCache);
		}

	// Zero the cache for the studio crfs in the global crm.
	for (i = 0; i < _pglicrfStudio->IvMac(); i++)
		{
		_pglicrfStudio->Get(i, &iv);
		pcrfT = _pcrmAll->PcrfGet(iv);
		Assert(pcrfT != pvNil, "Main CRM is corrupt.");
		pcrfT->SetCbMax(0);
		}

	fRet = fTrue;
LFail:
	if (!fRet)
		EndLongOp();
	ReleasePpo(&psceg);
	ReleasePpo(&pscpt);
	return fRet;
}


/***************************************************************************
	Initialize and start the studio script
***************************************************************************/
bool APP::_FInitStudio(PFNI pfniUserDoc, bool fFailIfDocOpenFailed)
{
	AssertBaseThis(0);

	long i;
	long cbCache;
	long iv;
	PCRF pcrfT;
	bool fRet = fFalse;

	_pstdio = STDIO::PstdioNew(khidStudio, _pcrmAll,
		(pfniUserDoc->Ftg() == ftgNil ? pvNil : pfniUserDoc),
		fFailIfDocOpenFailed);
	if (_pstdio == pvNil)
		{
		goto LFail;
		}

	// Up the cache limits of the studio crfs in the global crm.
	for (i = 0; i < _pglicrfStudio->IvMac(); i++)
		{
		_pgstStudioFiles->GetExtra(i, &cbCache);
		_pglicrfStudio->Get(i, &iv);
		pcrfT = _pcrmAll->PcrfGet(iv);
		Assert(pcrfT != pvNil, "Main CRM is corrupt.");
		pcrfT->SetCbMax(cbCache);
		}

	// Zero the cache for the building crfs in the global crm.
	for (i = 0; i < _pglicrfBuilding->IvMac(); i++)
		{
		_pglicrfBuilding->Get(i, &iv);
		pcrfT = _pcrmAll->PcrfGet(iv);
		Assert(pcrfT != pvNil, "Main CRM is corrupt.");
		pcrfT->SetCbMax(0);
		}

	fRet = fTrue;

LFail:
	if (!fRet)
		PushErc(ercSocCantInitStudio);
	return fRet;
}


/***************************************************************************
	Sets _fSlowCPU if this is a slow CPU.  It tests the graphics, 
	fixed-point math, and memory copying speed, and if any of them are
	slower than a threshold, _fSlowCPU is set.
***************************************************************************/
bool APP::_FDetermineIfSlowCPU(void)
{
	AssertBaseThis(0);

	bool fSlowCPU;

	// If user has a saved preference, read and use that
	if (FGetSetRegKey(kszBetterSpeedValue, &fSlowCPU, size(bool), fregNil))
		{
		_fSlowCPU = fSlowCPU;
		return fTrue;
		}

	_fSlowCPU = fFalse;
	
#ifndef PERF_TEST

	return fTrue;

#else //PERF_TEST

	PGPT pgptWnd = pvNil;
	PGPT pgptOff = pvNil;
	RC rc1;
	RC rc2;
	ulong ts;
	ulong dts1;
	ulong dts2;
	ulong dts3;
	long i;
	char *pch1 = pvNil;
	char *pch2 = pvNil;
	BRS r1;
	BRS r2;
	BRS r3;

	// Test 1: Graphics.  Copy some graphics to an offscreen buffer,
	// then blit it back to the window 100 times.
	rc1.Set(0, 0, 300, 300);
	rc2.Set(0, 0, 300, 300);
	pgptWnd = GPT::PgptNewHwnd(vwig.hwndApp);
	if (pvNil == pgptWnd)
		goto LFail;
	pgptOff = GPT::PgptNewOffscreen(&rc1, 8); // BWLD RGB buffer is 8-bit
	if (pvNil == pgptOff)
		{
		goto LFail;
		}
	// BLOCK
		{
		GNV gnvWnd(pgptWnd);
		GNV gnvOff(pgptOff);
		gnvOff.CopyPixels(&gnvWnd, &rc1, &rc1);
		ts = TsCurrent();
		for (i = 0; i < 100; i++)
			gnvWnd.CopyPixels(&gnvOff, &rc1, &rc2);
		dts1 = TsCurrent() - ts;
		}
	ReleasePpo(&pgptWnd);
	ReleasePpo(&pgptOff);

	// Test 2: Math.  Do 200,000 fixed-point adds and multiplies
	r1 = BR_SCALAR(1.12); // arbitrary number
	r2 = BR_SCALAR(3.14159); // arbitrary number
	r3 = rZero;
	ts = TsCurrent();
	for (i = 0; i < 200000; i++)
		{
		r2 = BrsMul(r1, r2);
		r3 = BrsAdd(r2, r3);
		}
	dts2 = TsCurrent() - ts;

	// Test 3: Copying.  Copy a 50,000 byte block 200 times.
	if (!FAllocPv((void **)&pch1, 50000, mprNormal, fmemClear))
		goto LFail;
	if (!FAllocPv((void **)&pch2, 50000, mprNormal, fmemClear))
		goto LFail;
	ts = TsCurrent();
	for (i = 0; i < 200; i++)
		CopyPb(pch1, pch2, 50000);
	dts3 = TsCurrent() - ts;
	FreePpv((void **)&pch1);
	FreePpv((void **)&pch2);

	if (dts1 > 700 || dts2 > 200 || dts3 > 500)
		_fSlowCPU = fTrue;

	return fTrue;
LFail:
	ReleasePpo(&pgptWnd);
	ReleasePpo(&pgptOff);
	FreePpv((void **)&pch1);
	FreePpv((void **)&pch2);
	return fFalse;

#endif //PERF_TEST
}


/***************************************************************************
	Reads command line.  Also sets _fniExe to path to this executable
	and _fniPortfolioDoc to file specified on command line (if any).
	Also initializes _stnProductLong and _stnProductShort.

	-f: fast mode
	-s: slow mode
	-p"longname": long productname
	-t"shortname": short productname
	-m: don't minimize window on deactivate

***************************************************************************/
void APP::_ParseCommandLine(void)
{
	AssertBaseThis(0);

#ifdef WIN
	SZ sz;
	STN stn;
	achar *pch;
	achar *pchT;
	FNI fniT;

	// Get path to exe
	GetModuleFileName(NULL, sz, kcchMaxSz);
	stn.SetSz(sz);
	if(!_fniExe.FBuildFromPath(&stn))
		Bug("Bad module filename?");

	pch = vwig.pszCmdLine;
	// first argument (app name) is useless...skip it
	_SkipToSpace(&pch);
	_SkipSpace(&pch);

	while (*pch != chNil)
		{
		// Look for /options or -options
		if (*pch == ChLit('/') || *pch == ChLit('-'))
			{
			switch(ChUpper(*(pch + 1)))
				{
			case ChLit('M'):
				_fDontMinimize = fTrue;
				break;
			case ChLit('S'):
				_fSlowCPU = fTrue;
				break;
			case ChLit('F'):
				_fSlowCPU = fFalse;
				break;
			case ChLit('P'):
				{
				pch += 2; // skip "-p"
				pchT = pch;
				_SkipToSpace(&pchT);
				// skip quotes
				if (*pch == ChLit('"'))
					_stnProductLong.SetRgch(pch + 1, pchT - pch - 2);
				else
					_stnProductLong.SetRgch(pch, pchT - pch);
				}
				break;
			case ChLit('T'):
				{
				pch += 2; // skip "-t"
				pchT = pch;
				_SkipToSpace(&pchT);
				// skip quotes
				if (*pch == ChLit('"'))
					_stnProductShort.SetRgch(pch + 1, pchT - pch - 2);
				else
					_stnProductShort.SetRgch(pch, pchT - pch);
				}
				break;
			default:
				Warn("Bad command-line switch");
				break;
				}
			_SkipToSpace(&pch);
			_SkipSpace(&pch);
			}
		else // try to parse as fni string
			{
			// get to end of string
			pchT = pch + CchSz(pch);

			// skip quotes since FBuildFromPath can't deal
			if (*pch == ChLit('"'))
				pch++;

			// move pchT to begginning of file name itself
			while(	(*pchT != '\\') &&
					(pchT != pch))
				pchT--;
			// pchT now points to last'\', move it forward one (only if not pch)
			if (*pchT == '\\')
				pchT++;

			// set STN to the path, which is the string up to the last '\'
			stn.SetRgch(pch, pchT - pch);

			// try to map to long file name if we can
			HANDLE hFile = pvNil;
			WIN32_FIND_DATA W32FindData;

			hFile = FindFirstFile(pch, &W32FindData);
			if (INVALID_HANDLE_VALUE != hFile)
				{
				// append the longfile name returned...
				stn.FAppendSz(W32FindData.cFileName);
				FindClose(hFile);
				}
			else
				// just use what was passed orginally...
				stn.SetSz(pch);


			// remove ending quotes since FBuildFromPath can't deal
			if (stn.Psz()[stn.Cch() - 1] == ChLit('\"'))
				stn.Delete(stn.Cch() - 1, 1);
			if(fniT.FBuildFromPath(&stn))
				{
				SetPortfolioDoc(&fniT);
				}
			// move to end of string, we now assume that everything past options is the document name
			pch += CchSz(pch);
			}
		}
#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC
}

/***************************************************************************
	Make sure that _stnProductLong and _stnProductShort are initialized to
	non-empty strings.  If _stnProductLong wasn't initialized from the
	command line, it and _stnProductShort are read from the app resource
	file.  If _stnProductLong is valid and _stnProductShort is not,
	_stnProductShort is just set to _stnProductLong.
***************************************************************************/
bool APP::_FEnsureProductNames(void)
{
	AssertBaseThis(0);
	
	if (_stnProductLong.Cch() == 0)
		{
#ifdef WIN
		SZ sz;

		if (0 == LoadString(vwig.hinst, stid3DMovieNameLong, sz, kcchMaxSz))
			return fFalse;
		_stnProductLong.SetSz(sz);

		if (0 == LoadString(vwig.hinst, stid3DMovieNameShort, sz, kcchMaxSz))
			return fFalse;
		_stnProductShort.SetSz(sz);
#else //MAC
		RawRtn();
#endif //MAC
		}

	if (_stnProductShort.Cch() == 0)
		_stnProductShort = _stnProductLong;

	return fTrue;
}


/***************************************************************************
	Find _fniMsKidsDir 
***************************************************************************/
bool APP::_FFindMsKidsDir(void)
{
	AssertBaseThis(0);
	Assert(_stnProductLong.Cch() > 0 && _stnProductShort.Cch() > 0,
		"_stnProductLong and _stnProductShort must exist");

	FNI fni;
	SZ szMsKidsDir;
	STN stn;
	STN stnUsers;

	szMsKidsDir[0] = chNil;
	if (!FGetSetRegKey(kszInstallDirValue, szMsKidsDir, size(SZ),
		fregMachine | fregString))
		{
		Warn("Missing InstallDirectory registry entry or registry error");
		}
	stn = szMsKidsDir;
	if (stn.Cch() == 0 || !_fniMsKidsDir.FBuildFromPath(&stn, kftgDir) ||
		tYes != _fniMsKidsDir.TExists())
		{
		// REVIEW manders: this artificial search is temp until we have a
		// real setup program and users have a InstallDirectory registry entry
		_fniMsKidsDir = _fniExe;
		if (!_fniMsKidsDir.FSetLeaf(pvNil, kftgDir))
			return fFalse;
		while (_fniMsKidsDir.FUpDir(pvNil, ffniMoveToDir))
			;
		/* REVIEW manders (peted): if you check for the MSKIDS dir first, then
			you don't have to reset the dir string before presenting the error
			to the user */
		stn = PszLit("Microsoft Kids"); // REVIEW manders
		if (!_fniMsKidsDir.FDownDir(&stn, ffniMoveToDir))
			{
			stn = PszLit("MSKIDS"); // REVIEW manders
			if (!_fniMsKidsDir.FDownDir(&stn, ffniMoveToDir))
				{
				Warn("Can't find Microsoft Kids or MSKIDS.");
				stn = PszLit("Microsoft Kids");
				_FCantFindFileDialog(&stn); // ignore failure
				return fFalse;
				}
			}
		}

	AssertPo(&_fniMsKidsDir, ffniDir);
	return fTrue;
}

/***************************************************************************
	Find _fniProductDir 
	At this point, _stnProduct* is either the command line parameter or
	3D Movie Maker (in the absence of a command line parameter)
	_FFindProductDir() locates the .chk files by checking:
		first, _stnProduct* directories, or 
		second, the registry of installed products.
	This routine updates _stnProductLong and _stnProductShort on return.
***************************************************************************/
bool APP::_FFindProductDir(PGST pgst)
{
	AssertBaseThis(0);
	AssertVarMem(pgst);

	STN stnLong;
	STN stnShort;
	STN stn;
	FNI fni;
	long istn;

	if (_FQueryProductExists(&_stnProductLong, &_stnProductShort, &_fniProductDir))
		return fTrue;

	for (istn = 0; istn < pgst->IstnMac(); istn++)
		{
		pgst->GetStn(istn, &stn);
		vptagm->SplitString(&stn, &stnLong, &stnShort);
		if (_FQueryProductExists(&stnLong, &stnShort, &fni))
			{
			_stnProductLong = stnLong;
			_stnProductShort = stnShort;
			_fniProductDir = fni;
			return fTrue;
			}
		}
	return fFalse;
}

/***************************************************************************
	See if the product exists.
	Method:  See if the directory and chunk file exist.
***************************************************************************/
bool APP::_FQueryProductExists(STN *pstnLong, STN *pstnShort, FNI *pfni)
{
	AssertBaseThis(0);
	AssertVarMem(pfni);
	AssertPo(pstnLong, 0);
	AssertPo(pstnShort, 0);

	FNI fni;
	STN stn;

	*pfni = _fniMsKidsDir;
	if (!pfni->FDownDir(pstnLong, ffniMoveToDir) &&
		!pfni->FDownDir(pstnShort, ffniMoveToDir))
		{
		pfni->GetStnPath(&stn);
		if (!stn.FAppendStn(&_stnProductLong))
			goto LFail;
		_FCantFindFileDialog(&stn); // ignore failure
		goto LFail;
		}

	fni = *pfni;
	if (fni.FSetLeaf(pstnLong, kftgChunky) && (tYes == fni.TExists()))
		return fTrue;
	fni = *pfni;
	if (fni.FSetLeaf(pstnShort, kftgChunky) && (tYes == fni.TExists()))
		return fTrue;
LFail:
	TrashVar(pfni);
	return fFalse;
}

/***************************************************************************
	Advances *ppch until it points to either the next space character or
	the end of the string.  Exception: if the space character is surrounded
	by double quotes, this function skips by it.
***************************************************************************/
void APP::_SkipToSpace(char **ppch)
{
	AssertBaseThis(0);
	AssertVarMem(ppch);
	
	bool fQuote = fFalse;

	while(**ppch != chNil && (fQuote || **ppch != kchSpace))
		{
		if (**ppch == ChLit('"'))
			fQuote = !fQuote;
		(*ppch)++;
		}
}


/***************************************************************************
	Advances *ppch to the next non-space character.
***************************************************************************/
void APP::_SkipSpace(char **ppch)
{
	AssertBaseThis(0);
	AssertVarMem(ppch);
	
	while(**ppch == kchSpace)
		(*ppch)++;
}


/***************************************************************************
	Socrates window was activated or deactivated.
***************************************************************************/
void APP::_Activate(bool fActive)
{
	AssertBaseThis(0);

#ifdef WIN
	bool fIsIconic;

	APP_PAR::_Activate(fActive);

	fIsIconic = IsIconic(vwig.hwndApp);

	if (!fActive) // app was just deactivated
		{
		if (_FDisplayIs640480() && !_fDontMinimize && !fIsIconic)
			{
			// Note: using SW_MINIMIZE causes a bug where alt-tabbing
			// from this app to a fullscreen DOS window reactivates
			// this app.  So use SW_SHOWMINNOACTIVE instead.
			ShowWindow(vwig.hwndApp, SW_SHOWMINNOACTIVE); // minimize app
			_fMinimized = fTrue;
				
			// Note that we examine _fMinimized during the WM_DISPLAYCHANGE message
			// received as a result of the following res change call. Therefore the
			// minimize operation MUST precede the res switch.
			if (_fSwitchedResolution)
				_FSwitch640480(fFalse);

			// When the portfolio is displayed, the main app is automatically disabled.
			// This means all keyboard/mouse input directed at the main app window will
			// be ignored until the portfolio is finished with. If the app is minimized
			// here while the portfolio is displayed, then we will be left with a disabled
			// app window on the win95 task bar. As a result, the app will not appear
			// only the task window invoked by an Alt-tab, nor is it resized when
			// the user clicks on it in the taskbar, (even though win95 tries to
			// activate it). We could do the following...
			// (1) Do not auto-minimize the app window while the portfolio is displayed.
			//		This is what happens on NT.
			// (2) Drop the portfolio here, so the app window is enabled on the taskbar.
			// (3) Make sure the app window is enabled now, by doing this...
			EnableWindow(vwig.hwndApp, TRUE);

			// The concern with doing this, is that when the app is later restored,
			// it is then enabled when it shouldn't be, as the portfolio is still
			// up in front of it. As it happens, this doesn't matter because the
			// portfolio is full screen. This means that the user can't direct any
			// mouse input to the main app window, and the portfolio will eat up any
			// keyboard input.

			}
		}

#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC

	/* Don't do this stuff unless we've got the CEX set up */
	if (vpcex != pvNil)
		{

		if (!fActive)
			{
			if (!vpcex->FCidIn(cidDeactivate))
				{
				vpcex->EnqueueCid(cidDeactivate);
				_fDown = fTrue;
				_cactToggle = 0;
				}
			}
		else if (_pcex != pvNil)
			{
			//
			// End the modal wait
			//
			Assert(CactModal() > 0, "AAAAAAAAAhhhhh! - P.Floyd (Encore performance)");

			// If there is no cidEndModal currently waiting to be processed,
			// enqueue one now. This ensures we don't get multiple cidEndModal's
			// processed. Note that we don't want to set _pcex null here as it
			// is later examined in the wndproc before we ultimately return
			// from FModalTopic.
			if (!_pcex->FCidIn(cidEndModal))
				_pcex->EnqueueCid(cidEndModal);

			}
		else
			{
			vpcex->FlushCid(cidDeactivate);	
			}

		}

}


/***************************************************************************
	Deactivate the app
***************************************************************************/
bool APP::FCmdDeactivate(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	GCB gcb;
	PWOKS pwoksModal;
	GTE gte;
	PGOB pgob;
	ulong grfgte;
	long lwRet;
	CMD_MOUSE cmd;
	PT pt;
	bool fDoQuit;

	if (_pcex != pvNil)
		{
		Assert(CactModal() > 0, "AAAAAAAAAhhhhh! - P.Floyd");
		return(fTrue);
		}

	pgob = vpcex->PgobTracking();
	if ((pgob != pvNil) && (_cactToggle < 500))
		{
		//
		// Toggle the mouse button
		//
		TrackMouse(pgob, &pt);

		ClearPb(&cmd, size(CMD_MOUSE));

		cmd.pcmh = pgob;
		cmd.cid = cidTrackMouse;
		cmd.xp = pt.xp;
		cmd.yp = pt.yp;
		cmd.grfcust = GrfcustCur();
		if (_fDown)
			{
			cmd.grfcust |= fcustMouse;
			}
		else
			{
			cmd.grfcust &= ~fcustMouse;
			}
		vpcex->EnqueueCmd((PCMD)&cmd);
		vpcex->EnqueueCid(cidDeactivate);
		_fDown = !_fDown;
		_cactToggle++;
		return fTrue;
		}

	gte.Init(Pkwa(), fgteNil);
	while (gte.FNextGob(&pgob, &grfgte, fgteNil))
		{
		if (!(grfgte & fgtePre) || !pgob->FIs(kclsGOK))
			continue;

		((PGOK)pgob)->Suspend();
		}

	if (FPushModal())
		{
		gcb.Set(CMH::HidUnique(), Pkwa(), fgobNil, kginMark);
		gcb._rcRel.Set(0, 0, krelOne, krelOne);

		_pcex = vpcex;

		if (pvNil != (pwoksModal = NewObj WOKS(&gcb, Pkwa()->Pstrg())))
			{
			vpcex->SetModalGob(pwoksModal);
			FModalLoop(&lwRet);  // If we cannot enter modal mode, then we just won't suspend.
			vpcex->SetModalGob(pvNil);
			}

		ReleasePpo(&pwoksModal);

		_pcex = pvNil;

		// The user may have selected Close the app system menu while the 
		// app was minimized on the taskbar. Depending on how Windows sent 
		// the messages to us, (ie the processing order of activate and 
		// close messages are not predictable it seems), we may or may not 
		// have a cidQuit message queued for the app. If PopModal destroys 
		// queued messages, then we would loose any Waiting cidQuit. 
		// Therefore check if we have a queued cidQuit, and if so, requeue 
		// it after the call to PopModal.
		fDoQuit = vpcex->FCidIn(cidQuit);

		PopModal();

		if(fDoQuit)
			vpcex->EnqueueCid(cidQuit);
		}


	gte.Init(Pkwa(), fgteNil);
	while (gte.FNextGob(&pgob, &grfgte, fgteNil))
		{
		if (!(grfgte & fgtePre) || !pgob->FIs(kclsGOK))
			continue;

		((PGOK)pgob)->Resume();
		}

	return(fTrue);
}


/***************************************************************************
	Copy pixels from an offscreen buffer (pgnvSrc, prcSrc) to the screen
	(pgnvDst, prcDst).  This is called to move bits from an offscreen
	buffer to the screen during a _FastUpdate cycle.  This gives us
	a chance to do a transition.
***************************************************************************/
void APP::_CopyPixels(PGNV pgnvSrc, RC *prcSrc, PGNV pgnvDst, RC *prcDst)
{
	AssertBaseThis(0);
	AssertPo(pgnvSrc, 0);
	AssertVarMem(prcSrc);
	AssertPo(pgnvDst, 0);
	AssertVarMem(prcDst);

	PMVIE pmvie = _Pmvie(); // Get the current movie, if any
	PGOB pgob;
	RC rcDst, rcSrc, rcWorkspace;

	if (pmvie == pvNil || pmvie->Trans() == transNil)
		{
		APP_PAR::_CopyPixels(pgnvSrc, prcSrc, pgnvDst, prcDst);
		return;
		}

	Assert(prcSrc->Dyp() == prcDst->Dyp() && prcSrc->Dxp() == prcDst->Dxp(),
		"rc's are scaled");

	// Need to do a transition, but if it's a slow transition (not a cut),
	// we want to do a regular copy on all the areas around the workspace,
	// then the slow transition on just the workspace.

	pgob = Pkwa()->PgobFromHid(kidWorkspace);

	if (pgob == pvNil || pmvie->Trans() == transCut)
		{
		pmvie->DoTrans(pgnvDst, pgnvSrc, prcDst, prcSrc);
		return;
		}

	// NOTE: This code assumes that the following will base rcWorkspace
	// in the same coordinate system as prcDst, which is always true
	// according to ShonK.
	pgob->GetRc(&rcWorkspace, cooHwnd);

	// Do the areas around the workspace without the transition
	if (prcDst->ypTop < rcWorkspace.ypTop)
		{
		rcSrc = *prcSrc;
		rcSrc.ypBottom = rcWorkspace.ypTop + (rcSrc.ypTop - prcDst->ypTop);
		rcDst = *prcDst;
		rcDst.ypBottom = rcWorkspace.ypTop;
		APP_PAR::_CopyPixels(pgnvSrc, &rcSrc, pgnvDst, &rcDst);
		}

	if (prcDst->ypBottom > rcWorkspace.ypBottom)
		{
		rcSrc = *prcSrc;
		rcSrc.ypTop = rcWorkspace.ypBottom + (rcSrc.ypTop - prcDst->ypTop);
		rcDst = *prcDst;
		rcDst.ypTop = rcWorkspace.ypBottom;
		APP_PAR::_CopyPixels(pgnvSrc, &rcSrc, pgnvDst, &rcDst);
		}
	if (prcDst->xpLeft < rcWorkspace.xpLeft)
		{
		rcSrc.ypTop = rcWorkspace.ypTop + (prcSrc->ypTop - prcDst->ypTop);
		rcSrc.ypBottom = rcWorkspace.ypBottom + (prcSrc->ypTop - prcDst->ypTop);
		rcSrc.xpLeft = prcSrc->xpLeft;
		rcSrc.xpRight = rcWorkspace.xpLeft + (prcSrc->xpLeft - prcDst->xpLeft);
		rcDst = *prcDst;
		rcDst.xpRight = rcWorkspace.xpLeft;
		rcDst.ypTop = rcWorkspace.ypTop;
		rcDst.ypBottom = rcWorkspace.ypBottom;
		APP_PAR::_CopyPixels(pgnvSrc, &rcSrc, pgnvDst, &rcDst);
		}

	if (prcDst->xpRight > rcWorkspace.xpRight)
		{
		rcSrc.ypTop = rcWorkspace.ypTop + (prcSrc->ypTop - prcDst->ypTop);
		rcSrc.ypBottom = rcWorkspace.ypBottom + (prcSrc->ypTop - prcDst->ypTop);
		rcSrc.xpLeft = rcWorkspace.xpRight + (prcSrc->xpLeft - prcDst->xpLeft);
		rcSrc.xpRight = prcSrc->xpRight;
		rcDst = *prcDst;
		rcDst.xpLeft = rcWorkspace.xpRight;
		rcDst.ypTop = rcWorkspace.ypTop;
		rcDst.ypBottom = rcWorkspace.ypBottom;
		APP_PAR::_CopyPixels(pgnvSrc, &rcSrc, pgnvDst, &rcDst);
		}

	//
	// Now do the workspace copy, with the transition
	//
	if (rcWorkspace.FIntersect(prcDst))
		{
		rcSrc = rcWorkspace;
		rcSrc.Offset(prcSrc->xpLeft - prcDst->xpLeft, prcSrc->ypTop - prcDst->ypTop);
		pmvie->DoTrans(pgnvDst, pgnvSrc, &rcWorkspace, &rcSrc);
		}
}



/***************************************************************************
	Load the Studio
***************************************************************************/
bool APP::FCmdLoadStudio(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	FNI fniUserDoc;
	CHID chidProject;
	long kidBuilding;
	PGOB pgob;
	PGOK pgokBackground;

	kidBuilding = pcmd->rglw[0];
	chidProject = pcmd->rglw[1];

	GetPortfolioDoc(&fniUserDoc); // might be ftgNil

	if (_FInitStudio(&fniUserDoc))
		{
		// Nuke the building gob
		pgob = Pkwa()->PgobFromHid(kidBuilding);
		if (pvNil != pgob)	
			pgob->Release();
	
		// Start a project, if requested
		if (chidProject != chidNil)
			{
			pgokBackground = (PGOK)Pkwa()->PgobFromHid(kidBackground);
			if ((pgokBackground != pvNil) && pgokBackground->FIs(kclsGOK))
				{
				AssertPo(pgokBackground, 0);
				// REVIEW manders: if this fails, what happens?
				pgokBackground->FRunScript((kstDefault << 16) | chidProject);
				}
			}
		}
	else
		{
		Assert(_pstdio == pvNil, "_FInitStudio didn't clean up");
		vpcex->EnqueueCid(cidLoadStudioFailed);
		}

	return fTrue;
}


/***************************************************************************
	Load the Building
***************************************************************************/
bool APP::FCmdLoadBuilding(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	if ((_pstdio != pvNil) && !_pstdio->FShutdown())
		{
		long kgobReturn;

		// This attempt may have destroyed the contents of kpridBuildingGob;
		// Try to reset it from kpridBuildingGobT.
		if (FGetProp(kpridBuildingGobT, &kgobReturn))
			FSetProp(kpridBuildingGob, kgobReturn);
		if (FGetProp(kpridBuildingStateT, &kgobReturn))
			FSetProp(kpridBuildingState, kgobReturn);
		Warn("Shutting down Studio failed");
		return fTrue;
		}

	if(_FInitBuilding())
		ReleasePpo(&_pstdio);
	return fTrue;
}


/***************************************************************************
	Exit the studio
***************************************************************************/
bool APP::FCmdExitStudio(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	bool tRet;
	STN stnBackup;

	// Now query the user to find whether they want to go to the
	// building or to exit the app completely.

	if (!FGetStnApp(idsExitStudio, &stnBackup))
		return fTrue;

	tRet = TModal(vpapp->PcrmAll(), ktpcQueryExitStudio, &stnBackup, bkYesNoCancel);

	// Take more action if the user did not hit cancel.
	if(tRet != tMaybe)
		{
		// Save the current movie if necessary.
		if ((_pstdio != pvNil) && !_pstdio->FShutdown(tRet == tYes))
			return fTrue;

		// Either go to the building, or leave the app.
		if (tRet == tYes)
			{
			// Go to the building.
			if(_FInitBuilding())
				ReleasePpo(&_pstdio);
			}
		else if (tRet == tNo)
			{
			// User wants to quit. We have have any dirty doc already by now.
			// Note that APP::Quit() doesn't release _pstdio when we quit so
			// we don't need to do it here either.
			_fQuit = fTrue;
			}
		}

	return fTrue;
}


/***************************************************************************
	Load the Theater
***************************************************************************/
bool APP::FCmdTheaterOpen(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	long kidParent;

	kidParent = pcmd->rglw[0];

	if (pvNil != _ptatr)
		{
		Bug("You forgot to close the last TATR!");
		AssertPo(_ptatr, 0);
		return fTrue;
		}
	_ptatr = TATR::PtatrNew(kidParent);
	// Let the script know whether the open succeeded or failed
	vpcex->EnqueueCid(cidTheaterOpenCompleted, pvNil, pvNil, (pvNil != _ptatr));

	return fTrue;
}

/***************************************************************************
	Close the Theater
***************************************************************************/
bool APP::FCmdTheaterClose(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	AssertPo(_ptatr, 0);
	ReleasePpo(&_ptatr);

	return fTrue;
}


/***************************************************************************
	Clear the portfolio doc
***************************************************************************/
bool APP::FCmdPortfolioClear(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	_fniPortfolioDoc.SetNil();

	return fTrue;
}


/***************************************************************************
	Display the customized open file common dlg.
***************************************************************************/
bool APP::FCmdPortfolioOpen(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	FNI fni;
	bool fOKed;
	long idsTitle, idsFilterLabel, idsFilterExt;
	FNI fniUsersDir;
	PFNI pfni;
	ulong grfPrevType;
	CNO cnoWave = cnoNil;

	// Set up strings specific to this use of the portfolio.

	switch (pcmd->rglw[0])
		{
	case kpfPortOpenMovie:
		idsTitle = idsPortfOpenMovieTitle;
		idsFilterLabel = idsPortfMovieFilterLabel;
		idsFilterExt = idsPortfMovieFilterExt;
		
		grfPrevType = fpfPortPrevMovie;
		cnoWave = kwavPortOpenMovie;

		break;

	case kpfPortOpenSound:
		// Only display extensions appropriate to the type of sound being imported.
		idsTitle = idsPortfOpenSoundTitle;
 		idsFilterLabel = idsPortfSoundFilterLabel;
		if(pcmd->rglw[1] == kidMidiGlass)
			idsFilterExt = idsPortfSoundMidiFilterExt;
		else
			idsFilterExt = idsPortfSoundWaveFilterExt;

		grfPrevType = fpfPortPrevMovie | fpfPortPrevSound;
		cnoWave = kwavPortOpenSound;

		break;

	case kpfPortOpenTexture:
		idsTitle = idsPortfOpenTextureTitle;
 		idsFilterLabel = idsPortfTextureFilterLabel;
		idsFilterExt = idsPortfTextureFilterExt;

		grfPrevType = fpfPortPrevTexture;
		// Currently no audio for open texture, because we don't open texture yet.

		break;

	default:
		Bug("Unrecognized portfolio open type.");
		return fFalse;
		}

	// Prepare to set the initial directory for the portfolio if necessary.

	switch (pcmd->rglw[2])
		{
	case kpfPortDirUsers:

		// Initial directory will be the 'Users' directory.

		vapp.GetFniUsers(&fniUsersDir);
		pfni = &fniUsersDir;

		break;

	default:

		// Initial directory will be current directory.

		pfni = pvNil;

		break;
		}

	// Now display the open dlg. Script is informed of the outcome
	// of the portfolio from beneath FPortDisplayWithIds.
	fOKed = FPortDisplayWithIds(&fni, fTrue, idsFilterLabel, idsFilterExt,
		idsTitle, pvNil, pvNil, pfni, grfPrevType, cnoWave);
	if (fOKed)
		{
		// User selected a file, so store fni for later use.
		SetPortfolioDoc(&fni);
		}

	return fTrue;
}


/******************************************************************************
	OnnDefVariable
		Retrieves the default onn for this app.  Gets the name from the app's
		string table.
************************************************************ PETED ***********/
long APP::OnnDefVariable(void)
{
	AssertBaseThis(0);

	if (_onnDefVariable == onnNil)
		{
		STN stn;

		if (!FGetStnApp(idsDefaultFont, &stn) ||
				!FGetOnn(&stn, &_onnDefVariable))
			{
			_onnDefVariable = APP_PAR::OnnDefVariable();
			}
		}
	return _onnDefVariable;
}


/******************************************************************************
	FGetOnn
		APP version of FGetOnn.  Mainly used so that we can easily report
		failure to the user if the font isn't around.  And no, I don't care
		that we'll call FGetOnn twice on failure...it's a failure case, and
		can stand to be slow.  :)  Maps the font on failure so that there's
		still a usable onn if the user doesn't want to mess with alternate
		solutions.
	
	Arguments:
		PSTN pstn   --  The font name
		long *ponn  --  takes the result
	
	Returns:  fTrue if the original font could be found.
	
************************************************************ PETED ***********/
bool APP::FGetOnn(PSTN pstn, long *ponn)
{
	AssertBaseThis(0);

	if (!vntl.FGetOnn(pstn, ponn))
		{
		if (!_fFontError)
			{
			PushErc(ercSocNoDefaultFont);
			_fFontError = fTrue;
			}
		*ponn = vntl.OnnMapStn(pstn);
		return fFalse;
		}
	return fTrue;
}


/***************************************************************************
	Return the size of memory
***************************************************************************/
void APP::MemStat(long *pdwTotalPhys, long *pdwAvailPhys)
{
#ifdef WIN
	MEMORYSTATUS ms;
	ms.dwLength = size(MEMORYSTATUS);
	GlobalMemoryStatus(&ms);
	if (pvNil != pdwTotalPhys)
		*pdwTotalPhys = ms.dwTotalPhys;
	if (pvNil != pdwAvailPhys)
		*pdwAvailPhys = ms.dwAvailPhys;
#else
	RawRtn();
#endif //!WIN
}


/******************************************************************************
	DypTextDef
		Retrieves the default dypFont for this app.  Gets the font size as
		a string from the app's string table, then converts it to the number
		value.
************************************************************ PETED ***********/
long APP::DypTextDef(void)
{
	AssertBaseThis(0);

	if (_dypTextDef == 0)
		{
		STN stn;

		if (pvNil == _pgstApp || !FGetStnApp(idsDefaultDypFont, &stn) ||
			!stn.FGetLw(&_dypTextDef) || _dypTextDef <= 0)
			{
			Warn("DypTextDef failed");
			_dypTextDef = APP_PAR::DypTextDef();
			}
		}
	return _dypTextDef;
}


/***************************************************************************
	Ask the user if they want to save changes to the given doc.
***************************************************************************/
bool APP::TQuerySaveDoc(PDOCB pdocb, bool fForce)
{
	AssertThis(0);
	AssertPo(pdocb, 0);

	STN stnName;
	long tpc;
	STN stnBackup;
	long bk;
	bool tResult;

	pdocb->GetName(&stnName);
	tpc = fForce ? ktpcQuerySave : ktpcQuerySaveWithCancel;
	if (!FGetStnApp(idsSaveChangesBkp, &stnBackup))
		stnBackup.SetNil();
	bk = fForce? bkYesNo : bkYesNoCancel;

	tResult = TModal(vpapp->PcrmAll(), tpc, &stnBackup, bk, kstidQuerySave,
		&stnName);
	vpcex->EnqueueCid(cidQuerySaveDocResult, pvNil, pvNil, tResult);

	return tResult;
}


/***************************************************************************
	Quit routine.  May or may not initiate the quit sequence (depending
	on user input).
***************************************************************************/
void APP::Quit(bool fForce)
{
	AssertThis(0);

	bool tRet;
	STN stnBackup;

	// If we already know we have to quit, or a modal topic is already
	// being  displayed, then do not query the user to quit here.
	if (_fQuit || vpappb->CactModal() > (_pcex != pvNil? 1 : 0)
			|| FInPortfolio())
		{
		// Make sure the app is visible to the user. Otherwise if this
		// return is preventing a system shutdown and we're minimized
		// on the taskbar, then the user won't know why the shutdown failed.
		if (!_fQuit)
			EnsureInteractive();
		return;
		}

	if (fForce)
		{
		// Force quit, don't ask the user if they want to quit.  But
		// do ask if they want to save their documents.
		DOCB::FQueryCloseAll(fdocForceClose);
		_fQuit = fTrue;

		return;
		}

	// If we're minimized, user is closing app from the taskbar.  Quit
	// without confirmation (we'll still confirm movie save if user has
	// a dirty doc)
	if (_fMinimized)
		{
		tRet = tYes;
		}
	else
		{
		if (!FGetStnApp(idsConfirmExitBkp, &stnBackup))
			stnBackup.SetNil();
		tRet = TModal(vpapp->PcrmAll(), ktpcQueryQuit, &stnBackup, bkYesNo);
		}

	if (tRet == tYes)
		{
		// User wants to quit, so shut down studio if necessary
		if (_pstdio == pvNil || _pstdio->FShutdown(fFalse))
			_fQuit = fTrue;
		}

}


/***************************************************************************
	Return a pointer to the current movie, if any.  The movie could be in
	the studio, theater, or splot machine.
***************************************************************************/
PMVIE APP::_Pmvie(void)
{
	AssertBaseThis(0);

	PMVIE pmvie = pvNil;
	PSPLOT psplot;

	if (_pstdio != pvNil && _pstdio->Pmvie() != pvNil)
		pmvie = _pstdio->Pmvie();
	else if (_ptatr != pvNil && _ptatr->Pmvie() != pvNil)
		pmvie = _ptatr->Pmvie();
	else if (Pkwa() != pvNil)
		{
		psplot = (PSPLOT)Pkwa()->PgobFromCls(kclsSPLOT);
		if (psplot != pvNil)
			pmvie = psplot->Pmvie();
		}
	return pmvie;
}


/****************************************
	Info dialog items (idits)
****************************************/
enum
	{
	iditOkInfo,
	iditProductId,
	iditWindowModeInfo,
#ifdef DEBUG
	iditCactAV,
#endif // DEBUG
	iditProductNameInfo,
	iditSaveChanges,
	iditRenderModeInfo,
	iditLimInfo
	};


#ifdef WIN

/***************************************************************************
	Useful function
***************************************************************************/
char *LoadGenResource(HINSTANCE hInst, LPCSTR lpResource, LPCSTR lpType)
{
	HRSRC hResource;
	HGLOBAL hGbl;

	hResource = FindResource(hInst, lpResource, lpType);

	if (hResource == NULL)
		return(NULL);

	hGbl = LoadResource(hInst, hResource);

	return (char *)LockResource(hGbl);
}

#endif

/***************************************************************************
	Put up info dialog
***************************************************************************/
bool APP::FCmdInfo(PCMD pcmd)
{
	AssertThis(0);
	PMVIE pmvie = pvNil;
	PDLG pdlg;
	long idit;
	bool fRunInWindowNew;
	STN stn;
	STN stnT;
	bool fSaveChanges;

	pmvie = _Pmvie();

	pdlg = DLG::PdlgNew(dlidInfo, pvNil, pvNil);
	if (pvNil == pdlg)
		return fTrue;
	pdlg->PutRadio(iditRenderModeInfo, _fSlowCPU ? 1 : 0);

	GetStnAppName(&stn);
#ifdef DEBUG
	stn.FAppendSz(PszLit(" (Debug)"));
#endif //DEBUG
	stnT.FFormatSz(PszLit(" %d.%d.%04d"), rmj, rmm, rup);
	stn.FAppendStn(&stnT);
	pdlg->FPutStn(iditProductNameInfo, &stn);

#ifdef WIN
	LPSTR lpszPid;
	char rgchPid[kcchMaxStn + 1];

	lpszPid = (LPSTR)LoadGenResource(vwig.hinst, MAKEINTRESOURCE(RC_PID_NUMBER), RT_RCDATA);

	if(lpszPid != pvNil)
		{
	
		#define PID_RPC       128
		#define PID_SITE      (PID_RPC + 5)
		#define PID_SERIAL    (PID_SITE + 3)
		#define PID_RANDOM    (PID_SERIAL + 7)

		wsprintf(rgchPid,
				  "%5.5s-%3.3s-%7.7s-%5.5s",
				  lpszPid+PID_RPC,
				  lpszPid+PID_SITE,
				  lpszPid+PID_SERIAL,
				  lpszPid+PID_RANDOM);
	
		stn.SetSz(rgchPid);
		pdlg->FPutStn(iditProductId, &stn);
		}
#endif

#ifdef DEBUG
	pdlg->FPutLwInEdit(iditCactAV, vcactAV);
#endif // DEBUG
	pdlg->PutRadio(iditWindowModeInfo, _fRunInWindow ? 1 : 0);

	idit = pdlg->IditDo();

	fSaveChanges = pdlg->FGetCheck(iditSaveChanges);
	if (FPure(_fSlowCPU) != FPure(pdlg->LwGetRadio(iditRenderModeInfo)))
		{
		PMVIE pmvie;

		_fSlowCPU = !_fSlowCPU;
		pmvie = _Pmvie();
		if (pvNil != pmvie)
			{
			pmvie->Pbwld()->FSetHalfMode(fFalse, _fSlowCPU);
			pmvie->InvalViews();
			}
		}
	if (fSaveChanges)
		{
		bool fSlowCPU = _fSlowCPU;
		FGetSetRegKey(kszBetterSpeedValue, &fSlowCPU,
			size(bool), fregSetKey);
		}

	fRunInWindowNew = pdlg->LwGetRadio(iditWindowModeInfo);
	if (FPure(_fRunInWindow) != FPure(fRunInWindowNew))
		{
		if (!fRunInWindowNew)
			{
			// user wants to be fullscreen
			if (_FDisplaySwitchSupported())
				{
				_fRunInWindow = fFalse;
				_RebuildMainWindow();
				if (!_FSwitch640480(fTrue))
					{
					_fRunInWindow = fTrue;
					_RebuildMainWindow();
					}
				}
			}
		else
			{
			// user wants to run in a window.
			// Don't allow user to run in a window at 640x480 resolution.
			if (!_FDisplayIs640480() || _fSwitchedResolution)
				{
				_fRunInWindow = fTrue;
				_RebuildMainWindow();
				if (_FSwitch640480(fFalse))
					{
					_fSwitchedResolution = fFalse;
					}
				else
					{
					// back to fullscreen
					_fRunInWindow = fFalse;
					_RebuildMainWindow();
					}
				}
			}
		}
	if (fSaveChanges)
		{
		bool fSwitchRes = !_fRunInWindow;

		FGetSetRegKey(kszSwitchResolutionValue, &fSwitchRes,
			size(fSwitchRes), fregSetKey);
		}

#ifdef DEBUG
	{
	bool fEmpty;
	long lwT;

	if (pdlg->FGetLwFromEdit(iditCactAV, &lwT, &fEmpty) && !fEmpty)
		{
		if (lwT < 0)
			{
			Debugger();
			}
		else
			{
			vcactAV = lwT;
			if (fSaveChanges)
				{
				DBINFO dbinfo;

				dbinfo.cactAV = vcactAV;
				AssertDo(FGetSetRegKey(PszLit("DebugSettings"), &dbinfo, size(DBINFO),
					fregSetKey | fregBinary),
					"Couldn't save current debug settings in registry");
				}
			}
		}
	}
#endif // DEBUG

	ReleasePpo(&pdlg);

	return fTrue;
}


#ifdef WIN
#ifdef UNICODE
typedef LONG (WINAPI *PFNCHDS) (LPDEVMODEW lpDevMode, DWORD dwFlags);
const PSZ kpszChds = PszLit("ChangeDisplaySettingsW");
#else
typedef LONG (WINAPI *PFNCHDS) (LPDEVMODEA lpDevMode, DWORD dwFlags);
const PSZ kpszChds = PszLit("ChangeDisplaySettingsA");
#endif // !UNICODE

#ifdef BUG1920
#ifdef UNICODE
typedef BOOL (WINAPI *PFNENUM) (LPCWSTR lpszDeviceName, DWORD iModeNum, LPDEVMODEW lpDevMode);
const PSZ kpszEnum = PszLit("EnumDisplaySettingsW");
#else
typedef BOOL (WINAPI *PFNENUM) (LPCSTR lpszDeviceName, DWORD iModeNum, LPDEVMODEA lpDevMode);
const PSZ kpszEnum = PszLit("EnumDisplaySettingsA");
#endif // !UNICODE
#endif // BUG1920
#endif //WIN

#ifndef DM_BITSPERPEL
#define DM_BITSPERPEL 0x00040000L // from wingdi.h
#define DM_PELSWIDTH 0x00080000L
#define DM_PELSHEIGHT 0x00100000L
#endif //!DM_BITSPERPEL

#ifndef CDS_FULLSCREEN
#define CDS_FULLSCREEN 4
#endif //!CDS_FULLSCREEN

#ifndef DISP_CHANGE_SUCCESSFUL
#define DISP_CHANGE_SUCCESSFUL 0
#endif //!DISP_CHANGE_SUCCESSFUL

/***************************************************************************
	Determine if display resolution switching is supported
***************************************************************************/
bool APP::_FDisplaySwitchSupported(void)
{
	AssertBaseThis(0);

#ifdef WIN
	DWORD dwVersion;
	byte bVersionMajor;
	byte bVersionMinor;
	bool fNT;
	
	dwVersion = GetVersion();
	bVersionMinor = (byte)((dwVersion & 0x0000ff00) >> 8);
	bVersionMajor = (byte)(dwVersion & 0x000000ff);
	fNT = ((dwVersion & 0x80000000) == 0);
	if (fNT)
		{
		// Note: At this time, no version of NT supports res-switching.  If
		//       this changes in the future, do the version-check here.  We
		//       can't just try ChangeDisplaySettings to see if it works,
		//       because on NT 3.51, the API exists, but hangs when we call
		//       it.
		return fFalse;
		}
	else
		{
		if (bVersionMajor >= 4)
			return fTrue;
		}
#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC
	return fFalse; // OS doesn't support res-switching
}


/***************************************************************************
	Switch to/from 640x480x8bit video mode.  It uses GetProcAddress so it
	can fail gracefully on systems that don't support
	ChangeDisplaySettings().
***************************************************************************/
bool APP::_FSwitch640480(bool fTo640480)
{
	AssertBaseThis(0);

#ifdef WIN
#ifdef BUG1920
	bool fSetMode = fFalse, fSetBbp = fTrue;
	DWORD iModeNum;
	PFNENUM pfnEnum;
#endif // BUG1920
	HINSTANCE hLibrary;
	PFNCHDS pfnChds;
	DEVMODE devmode;
	long lwResult;

	hLibrary = LoadLibrary(PszLit("USER32.DLL"));
	if (0 == hLibrary)
		goto LFail;

	pfnChds = (PFNCHDS)GetProcAddress(hLibrary, kpszChds);
	if (pvNil == pfnChds)
		goto LFail;

#ifdef BUG1920
	pfnEnum = (PFNENUM)GetProcAddress(hLibrary, kpszEnum);
	if (pvNil == pfnEnum)
		goto LFail;
#endif // BUG1920

	if (fTo640480)
		{
		// Try to switch to 640x480
#ifdef BUG1920
LRetry:
		for (iModeNum = 0; pfnEnum(NULL, iModeNum, &devmode); iModeNum++)
			{
			if ((fSetBbp ? devmode.dmBitsPerPel != 8 :
					devmode.dmBitsPerPel < 8) || devmode.dmPelsWidth != 640 ||
					devmode.dmPelsHeight != 480)
				{
				continue;
				}
			lwResult = pfnChds(&devmode, CDS_FULLSCREEN);
			if (lwResult == DISP_CHANGE_SUCCESSFUL)
				{
				fSetMode = fTrue;
				break;
				}
			}
		if (!fSetMode && fSetBbp)
			{
			fSetBbp = fFalse;
			goto LRetry;
			}

		if (fSetMode && _FDisplayIs640480())
#else  // BUG1920
		devmode.dmSize = size(DEVMODE);
		devmode.dmBitsPerPel = 8;
		devmode.dmPelsWidth = 640;
		devmode.dmPelsHeight = 480;
		devmode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		lwResult = pfnChds(&devmode, CDS_FULLSCREEN);
		if (DISP_CHANGE_SUCCESSFUL != lwResult)
			{
			// try without setting the bpp
			devmode.dmFields &= ~DM_BITSPERPEL;
			lwResult = pfnChds(&devmode, CDS_FULLSCREEN);
			}

		if (DISP_CHANGE_SUCCESSFUL == lwResult && _FDisplayIs640480())
#endif  // !BUG1920
			{
			_fSwitchedResolution = fTrue;
			SetWindowPos(vwig.hwndApp, HWND_TOP, 0, 0, 640, 480, 0);
			}
		else
			{
			goto LFail;
			}
		}
	else
		{
		// Try to restore user's previous resolution
		lwResult = pfnChds(NULL, CDS_FULLSCREEN);
		if (DISP_CHANGE_SUCCESSFUL != lwResult)
			goto LFail;
		}
	FreeLibrary(hLibrary);
	return fTrue;
LFail:
	if (0 != hLibrary)
		FreeLibrary(hLibrary);
#endif //WIN
#ifdef MAC
	RawRtn();
#endif //MAC
	return fFalse;
}


/***************************************************************************
	Clean up routine - app is shutting down
***************************************************************************/
void APP::_CleanUp(void)
{
	_FWriteUserData();
	ReleasePpo(&_pstdio);
	ReleasePpo(&_ptatr);
	ReleasePpo(&_pcfl);
	ReleasePpo(&_pmvieHandoff);
	ReleasePpo(&_pcrmAll);
	ReleasePpo(&_pglicrfBuilding);
	ReleasePpo(&_pglicrfStudio);
	ReleasePpo(&_pgstBuildingFiles);
	ReleasePpo(&_pgstStudioFiles);
	ReleasePpo(&_pgstSharedFiles);
	ReleasePpo(&_pgstApp);
	ReleasePpo(&_pkwa);
	BWLD::CloseBRender();
	APP_PAR::_CleanUp();
	if (_fSwitchedResolution)
		_FSwitch640480(fFalse); // try to restore desktop
}


/***************************************************************************
	Put up a modal help balloon
***************************************************************************/
bool APP::TModal(PRCA prca, long tpc, PSTN pstnBackup, long bkBackup,
	long stidSubst, PSTN pstnSubst)
{
	AssertThis(0);
	AssertNilOrPo(prca, 0);

	long lwSelect;
	bool tRet;
	STN stn;

	// If app is minimized, restore it so user can see the dialog
	EnsureInteractive();

	if (ivNil != stidSubst)
		{
		AssertPo(pstnSubst, 0);
		if (!Pkwa()->Pstrg()->FPut(stidSubst, pstnSubst))
			return tMaybe;
		}

	if (pvNil == prca || !Pkwa()->FModalTopic(prca, tpc, &lwSelect))
		{
		// Backup plan: use old TGiveAlertSz.
		if (pvNil != pstnSubst)
			stn.FFormat(pstnBackup, pstnSubst);
		else
			stn = *pstnBackup;
		tRet = TGiveAlertSz(stn.Psz(), bkBackup, cokExclamation);
		}
	else
		{
		// Help balloon appeared ok, so translate returned value into
		// something we can use here.  lwSelect is returned as the
		// (1 based) index of the btn clicked, out of Yes/No/Cancel.
		switch (lwSelect)
			{
		case 1:
			tRet = tYes;
			break;
		case 2:
			tRet = tNo;
			break;
		case 3:
			tRet = tMaybe;
			break;
		default:
			Bug("TModal() balloon returned unrecognized selection");
			tRet = tMaybe;
			}
		}

	if (ivNil != stidSubst)
		Pkwa()->Pstrg()->Delete(stidSubst);
	UpdateMarked(); // make sure alert is drawn over
	return tRet;
}


/***************************************************************************
	Static function to prompt the user to insert the CD named pstnTitle
***************************************************************************/
bool APP::FInsertCD(PSTN pstnTitle)
{
	AssertPo(pstnTitle, 0);

	STN stnBackup;
	bool tRet;

	stnBackup = PszLit("I can't find the CD '%s'  Please insert it.  Should I look again?");
	tRet = vpapp->TModal(vpapp->PcrmAll(), ktpcQueryCD, &stnBackup, bkYesNo,
		kstidQueryCD, pstnTitle);

	/* Don't tell the user that they told us not to try again; they know
		that already */
	if (tRet == tNo)
		vapp._fDontReportInitFailure = fTrue;

	return (tYes == tRet);
}


/******************************************************************************
	DisplayErrors
		Displays any errors that happen to be on the error stack.  Call this
		when you don't want to wait until idle time to show errors.
************************************************************ PETED ***********/
void APP::DisplayErrors(void)
{
	AssertThis(0);

	long erc;
	STN stnMessage;

	if (vpers->FPop(&erc))
		{
		STN stnErr;

		vpers->Clear();

		//
		// Convert to help topic number
		//
		switch (erc)
			{
		case ercOomHq:
			erc = ktpcercOomHq;
			break;

		case ercOomPv:
			erc = ktpcercOomPv;
			break;

		case ercOomNew:
			erc = ktpcercOomNew;
			break;

		/* We don't have any real specific information to present the user in
			these cases, plus they generally shouldn't come up anyway (a more
			informational error should have been pushed farther up the chain
			of error reporters) */
		case ercFilePerm:
		case ercFileOpen:
		case ercFileCreate:
		case ercFileSwapNames:
		case ercFileRename:
		case ercFniGeneral:
		case ercFniDelete:
		case ercFniRename:
		case ercFniMismatch:
		case ercFniDirCreate:
		case ercFneGeneral:
		case ercCflCreate:
		case ercCflSaveCopy:
		case ercSndmCantInit:
		case ercSndmPartialInit:
		case ercGfxCantDraw:
		case ercGfxCantSetFont:
		case ercGfxNoFontList:
		case ercGfxCantSetPalette:
		case ercDlgCantGetArgs:
		case ercDlgCantFind:
		case ercRtxdTooMuchText:
		case ercRtxdReadFailed:
		case ercRtxdSaveFailed:
		case ercCantOpenVideo:
		case ercMbmpCantOpenBitmap:

		/* In theory, these are obsolete. */
		case ercSocTdtTooLong:
		case ercSocWaveInProblems:
		case ercSocCreatedUserDir:

			/* Display a generic error message, with the error code in it */
			stnErr.FFormatSz(PszLit("%d"), erc);
			if (!vapp.Pkwa()->Pstrg()->FPut(kstidGenericError, &stnErr))
				stnErr.SetNil();
			erc = ktpcercSocGenericError;
			break;

		case ercFileGeneral:
			erc = ktpcercFileGeneral;
			break;

		case ercCflOpen:
			erc = ktpcercCflOpen;
			break;

		case ercCflSave:
			erc = ktpcercCflSave;
			break;

		case ercCrfCantLoad:
			erc = ktpcercCrfCantLoad;
			break;

		case ercFniHidden:
			erc = ktpcercFniHidden;
			break;

		case ercOomGdi:
			erc = ktpcercOomGdi;
			break;

		case ercDlgOom:
			erc = ktpcercDlgOom;
			break;

		case ercCantSave:
			erc = ktpcercCantSave;
			break;

		case ercSocSaveFailure:
			erc = ktpcercSocSaveFailure;
			break;

		case ercSocSceneSwitch:
			erc = ktpcercSocSceneSwitch;
			break;

		case ercSocSceneChop:
			erc = ktpcercSocSceneChop;
			break;

		case ercSocBadFile:
			erc = ktpcercSocBadFile;
			break;

		case ercSocNoTboxSelected:
			erc = ktpcercSocNoTboxSelected;
			break;

		case ercSocNoActrSelected:
			erc = ktpcercSocNoActrSelected;
			break;

		case ercSocNotUndoable:
			erc = ktpcercSocNotUndoable;
			break;

		case ercSocNoScene:
			erc = ktpcercSocNoScene;
			break;

		case ercSocBadVersion:
			erc = ktpcercSocBadVersion;
			break;

		case ercSocNothingToPaste:
			erc = ktpcercSocNothingToPaste;
			break;

		case ercSocBadFrameSlider:
			erc = ktpcercSocBadFrameSlider;
			break;

		case ercSocGotoFrameFailure:
			erc = ktpcercSocGotoFrameFailure;
			break;

		case ercSocDeleteBackFailure:
			erc = ktpcercSocDeleteBackFailure;
			break;

		case ercSocActionNotApplicable:
			erc = ktpcercSocActionNotApplicable;
			break;

		case ercSocCannotPasteThatHere:
			erc = ktpcercSocCannotPasteThatHere;
			break;

		case ercSocNoModlForChar:
			erc = ktpcercSocNoModlForChar;
			break;

		case ercSocNameTooLong:
			erc = ktpcercSocNameTooLong;
			break;

		case ercSocTboxTooSmall:
			erc = ktpcercSocTboxTooSmall;
			break;

		case ercSocNoThumbnails:
			erc = ktpcercSocNoThumbnails;
			break;

		case ercSocBadTdf:
			erc = ktpcercSocBadTdf;
			break;

		case ercSocNoActrMidi:
			erc = ktpcercSocNoActrMidi;
			break;

		case ercSocNoImportRollCall:
			erc = ktpcercSocNoImportRollCall;
			break;

		case ercSocNoNukeRollCall:
			erc = ktpcercSocNoNukeRollCall;
			break;

		case ercSocSceneSortError:
			erc = ktpcercSocSceneSortError;
			break;

		case ercSocCantInitSceneSort:
			erc = ktpcercSocCantInitSceneSort;
			break;

		case ercSocCantInitSplot:
			erc = ktpcercSocCantInitSplot;
			break;

		case ercSocNoWaveIn:
			erc = ktpcercSocNoWaveIn;
			break;

		case ercSocPortfolioFailed:
			erc = ktpcercSocPortfolioFailed;
			break;

		case ercSocCantInitStudio:
			erc = ktpcercSocCantInitStudio;
			break;

		case ercSoc3DWordCreate:
			erc = ktpcercSoc3DWordCreate;
			break;

		case ercSoc3DWordChange:
			erc = ktpcercSoc3DWordChange;
			break;

		case ercSocWaveSaveFailure:
			erc = ktpcercSocWaveSaveFailure;
			break;

		case ercSocNoSoundName:
			erc = ktpcercSocNoSoundName;
			break;

		case ercSndamWaveDeviceBusy:
			erc = ktpcercSndamWaveDeviceBusy;
			break;

		case ercSocNoKidSndsInMovie:
			erc = ktpcercSocNoKidSndsInMovie;
			break;
	
		case ercSocMissingMelanieDoc:
			erc = ktpcercSocMissingMelanieDoc;
			break;
	
		case ercSocCantLoadMelanieDoc:
			erc = ktpcercSocCantLoadMelanieDoc;
			break;

		case ercSocBadSoundFile:
			erc = ktpcercSocBadSoundFile;
			break;
	
		case ercSocBadSceneSound:
			erc = ktpcercSocBadSceneSound;
			break;
		
		case ercSocNoDefaultFont:
			erc = ktpcercSocNoDefaultFont;
			break;

		case ercSocCantCacheTag:
			erc = ktpcercSocCantCacheTag;
			break;
		
		case ercSocInvalidFilename:
			erc = ktpcercSocInvalidFilename;
			break;

		case ercSndMidiDeviceBusy:
			erc = ktpcercSndMidiDeviceBusy;
			break;

		case ercSocCantCopyMsnd:
			erc = ktpcercSocCantCopyMsnd;
			break;

		default:
			return;
			}

		FGetStnApp(idsOOM, &stnMessage);
		TModal(PcrmAll(), erc, &stnMessage, bkOk);
		if (stnErr.Cch() != 0)
			vapp.Pkwa()->Pstrg()->Delete(kstidGenericError);
		}
}


/***************************************************************************
	Idle routine.  Do APPB idle stuff, then report any runtime errors.
***************************************************************************/
bool APP::FCmdIdle(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	bool fFileError = fFalse;
	PCFL pcfl;

	APP_PAR::FCmdIdle(pcmd);

	/* Check all open chunky files for errors */
	for (pcfl = CFL::PcflFirst(); pcfl != pvNil; pcfl = (PCFL)pcfl->PbllNext())
		{
		if (pcfl->ElError() != elNil)
			{
			fFileError = fTrue;
			pcfl->ResetEl();
			}
		}
	/* Ensure that we report *something* if there was a file error */
	if (fFileError && vpers->Cerc() == 0)
		PushErc(ercFileGeneral);

	DisplayErrors();

	return fTrue;
}


#ifdef WIN
/***************************************************************************
	Tell another instance of the app to open a document.
***************************************************************************/
bool APP::_FSendOpenDocCmd(HWND hwnd, PFNI pfniUserDoc)
{
	AssertBaseThis(0);
	Assert(pvNil != hwnd, "bad hwnd");
	AssertPo(pfniUserDoc, ffniFile);

	STN stnUserDoc;
	STN stn;
	FNI fniTemp;
	PFIL pfil = pvNil;
	BLCK blck;
	DWORD dwProcId;

	// Write filename to 3DMMOpen.tmp in the temp dir
	pfniUserDoc->GetStnPath(&stnUserDoc);
	if (!fniTemp.FGetTemp())
		goto LFail;
	stn = kpszOpenFile;
	if (!fniTemp.FSetLeaf(&stn, kftgTemp))
		goto LFail;
	if (tYes == fniTemp.TExists())
		{
		// replace any existing open-doc request
		if (!fniTemp.FDelete())
			goto LFail;
		}
	pfil = FIL::PfilCreate(&fniTemp);
	if (pvNil == pfil)
		goto LFail;
	if (!pfil->FSetFpMac(stnUserDoc.CbData()))
		goto LFail;
	blck.Set(pfil, 0, stnUserDoc.CbData());
	if (!stnUserDoc.FWrite(&blck, 0))
		goto LFail;
	blck.Free(); // so it doesn't reference pfil anymore
	ReleasePpo(&pfil);
#ifdef WIN
	dwProcId = GetWindowThreadProcessId(hwnd, NULL);
	PostThreadMessage(dwProcId, WM_USER, klwOpenDoc, 0);
#else
	RawRtn();
#endif //!WIN
	return fTrue;
LFail:
	blck.Free(); // so it doesn't reference pfil anymore
	ReleasePpo(&pfil);
	return fFalse;
}
#endif //WIN


#ifdef WIN
/***************************************************************************
	Process a request (from another instance of the app) to open a document
***************************************************************************/
bool APP::_FProcessOpenDocCmd(void)
{
	AssertBaseThis(0);

	STN stnUserDoc;
	STN stn;
	FNI fniTemp;
	PFIL pfil = pvNil;
	FNI fniUserDoc;
	BLCK blck;

	// Find the temp file
	if (!fniTemp.FGetTemp())
		return fFalse;
	stn = kpszOpenFile;
	if (!fniTemp.FSetLeaf(&stn, kftgTemp))
		return fFalse;
	if (tYes != fniTemp.TExists())
		{
		Bug("Got a ProcessOpenDocCmd but there's no temp file!");
		return fFalse;
		}

	// See if we can accept open document commands now: If accelerators
	// are enabled, then ctrl-o is enabled, so open-document commands are
	// acceptable.
	if (_cactDisable > 0 || CactModal() > 0)
		goto LFail;
	
	// Read the document filename from temp file
	pfil = FIL::PfilOpen(&fniTemp);
	if (pvNil == pfil)
		goto LFail;
	blck.Set(pfil, 0, pfil->FpMac());
	if (!stnUserDoc.FRead(&blck, 0))
		goto LFail;
	blck.Free(); // so it doesn't reference pfil anymore
	ReleasePpo(&pfil);

	if (!fniUserDoc.FBuildFromPath(&stnUserDoc))
		goto LFail;
	if (pvNil == _pstdio)
		{
		SetPortfolioDoc(&fniUserDoc);
		vpcex->EnqueueCid(cidLoadStudioDoc); // this will load the portfolio doc
		}
	else
		{
		// ignore failure 
		_pstdio->FLoadMovie(&fniUserDoc);
		}

	fniTemp.FDelete(); // ignore failure
	return fTrue;
LFail:
	blck.Free(); // so it doesn't reference pfil anymore
	ReleasePpo(&pfil);
	fniTemp.FDelete(); // ignore failure
	return fFalse;
}
#endif //WIN


#ifdef WIN
/***************************************************************************
	Override standard _FGetNextEvt to catch WM_USER event.  Otherwise
	the event will get thrown away, because the event's hwnd is nil.
***************************************************************************/
bool APP::_FGetNextEvt(PEVT pevt)
{
	AssertThis(0);
	AssertVarMem(pevt);

	if (!APP_PAR::_FGetNextEvt(pevt))
		return fFalse;
	if (pevt->message != WM_USER || pevt->wParam != klwOpenDoc)
		return fTrue;
	_FProcessOpenDocCmd(); // ignore failure
	return fFalse; // we've handled the WM_USER event
}
#endif //WIN


/***************************************************************************
	Override default _FastUpdate to optionally skip offscreen buffer
***************************************************************************/
void APP::_FastUpdate(PGOB pgob, PREGN pregnClip, ulong grfapp, PGPT pgpt)
{
	AssertBaseThis(0);

	PMVIE pmvie;

	pmvie = _Pmvie();

	if (_fOnscreenDrawing && pvNil != pmvie && pmvie->FPlaying() &&
		pmvie->Pscen()->Nfrm() != pmvie->Pscen()->NfrmFirst())
		{
		APP_PAR::_FastUpdate(pgob, pregnClip, grfapp | fappOnscreen, pgpt);
		}
	else
		{
		APP_PAR::_FastUpdate(pgob, pregnClip, grfapp, pgpt);
		}
}


#ifdef WIN
/***************************************************************************
	Override default UpdateHwnd to optionally skip offscreen buffer
***************************************************************************/
void APP::UpdateHwnd(HWND hwnd, RC *prc, ulong grfapp)
{
	AssertBaseThis(0); // APP may not be completely valid

	PMVIE pmvie;

	pmvie = _Pmvie();

	if (_fOnscreenDrawing && pvNil != pmvie && pmvie->FPlaying() &&
		pmvie->Pscen()->Nfrm() != pmvie->Pscen()->NfrmFirst())
		{
		APP_PAR::UpdateHwnd(hwnd, prc, grfapp | fappOnscreen);
		}
	else
		{
		APP_PAR::UpdateHwnd(hwnd, prc, grfapp);
		}
}
#endif //WIN


#ifdef WIN

#ifndef WM_DISPLAYCHANGE
#define WM_DISPLAYCHANGE 0x007E   // from winuser.h
#endif // !WM_DISPLAYCHANGE

/***************************************************************************
	Handle Windows messages for the main app window. Return true iff the
	default window proc should _NOT_ be called.
***************************************************************************/
bool APP::_FFrameWndProc(HWND hwnd, uint wm, WPARAM wParam, LPARAM lw,
	long *plwRet)
{
	AssertBaseThis(0);
	AssertVarMem(plwRet);

	switch(wm)
		{
	case WM_ERASEBKGND:
		// Tell windows that we handled the Erase so it doesn't do one.
		// In general we don't want to erase our background ahead of time.
		// This prevents AVIs from flashing.
		*plwRet = fTrue;
		return fTrue;

	case WM_SIZE:
		{
		bool fRet;
		long lwStyle;

		fRet = APP_PAR::_FFrameWndProc(hwnd, wm, wParam, lw, plwRet);
		lwStyle = GetWindowLong(hwnd, GWL_STYLE);
		lwStyle &= ~WS_MAXIMIZEBOX;
		if (wParam == SIZE_MINIMIZED)
			{
			_fMinimized = fTrue;
			if (vpcex != pvNil)
				lwStyle |= WS_SYSMENU;
			else
				lwStyle &= ~WS_SYSMENU;
			}
		else if (!_fRunInWindow)
			lwStyle &= ~WS_SYSMENU;
		SetWindowLong(hwnd, GWL_STYLE, lwStyle);
		if (wParam == SIZE_RESTORED)
			{
			if (_fMainWindowCreated)
				_RebuildMainWindow();
			if (_fSwitchedResolution && _fMinimized)
				{
				if (!_FDisplayIs640480())
					_FSwitch640480(fTrue);
				}
			ShowWindow(vwig.hwndApp, SW_RESTORE); // restore app window
			_fMinimized = fFalse;
			}
		return fRet;
		}
	case WM_DISPLAYCHANGE:
		// Note that we don't need to do any of this if we're closing down
		if (_fQuit)
			break;

		if (_FDisplayIs640480())
			{
			_fRunInWindow = fFalse;
			_RebuildMainWindow();
			}
		else
			{
			// We're not running at 640x480 resolution now. Current design is that
			// if we switch from 640x480 to higher while the app is minimized, the
			// app it to still be full screen when restored. Therefore we don't
			// need to change _fRunInWindow here, as that we remain the same as
			// before the res switch, (as will the Windows properties for the app).
			// All we need to is make a note that we're no longer running in the
			// current windows settings resolution if we're running in full screen.

			if (!_fRunInWindow)
				{
				_fSwitchedResolution = fTrue;

				// If we're not minimized then we must switch to 640x480 resolution.
				// Don't switch res unless we're the active app window

				if (!_fMinimized && GetForegroundWindow() == vwig.hwndApp)
					{
					if (!_FSwitch640480(fTrue))
						_fSwitchedResolution = fFalse;
					}
				}

			// Call rebuild now to make sure the app window gets positioned
			// at the centre of the screen. Note that none of the other
			// window attributes will change beneath _RebuildMainWindow.
			if (!_fMinimized)
				_RebuildMainWindow();
			}
		return fTrue;

	case WM_INITMENUPOPUP:
		{
		// Disable the Close menu item if we are displaying a modal topic.
		// The user can't exit until a modal topic is dismissed.

		bool fDisableClose = (vpappb->CactModal() > (_pcex != pvNil ? 1 : 0));

		EnableMenuItem((HMENU)wParam, SC_CLOSE, MF_BYCOMMAND |
					(fDisableClose ? (MF_DISABLED | MF_GRAYED) : MF_ENABLED));

		break;
		}
		}

	if ((_pstdio == pvNil) || (_pstdio->Pmvie() == pvNil))
		{
	    return APP_PAR::_FFrameWndProc(hwnd, wm, wParam, lw, plwRet);
		}

	switch (wm)
		{
	case WM_QUERY_EXISTS:
		*plwRet = _pstdio->Pmvie()->LwQueryExists(wParam, lw);
		return(fTrue);

	case WM_QUERY_LOCATION:
		*plwRet = _pstdio->Pmvie()->LwQueryLocation(wParam, lw);
		return(fTrue);

	case WM_SET_MOVIE_POS:
		*plwRet = _pstdio->Pmvie()->LwSetMoviePos(wParam, lw);
		return(fTrue);

    default:
	    return APP_PAR::_FFrameWndProc(hwnd, wm, wParam, lw, plwRet);
		}

}
#endif //WIN

/***************************************************************************
 *
 * Returns whether or not screen savers should be allowed.
 *
 * Parameters:
 *  None
 *
 * Returns:
 *  fTrue  - Screen savers should be allowed
 *  fFalse - Screen savers should be blocked
 *
 **************************************************************************/
bool APP::FAllowScreenSaver(void)
{
	AssertBaseThis(0);

	// Disable the screen saver if...
	// 1. We're going to autominimize if a screen saver starts. Otherwise
	//    the user would be confused when they get back to the machine.
	// 2. We've switched resolutions, (ie we're full screen in a > 640x480 mode).
	//    Otherwise the screen save only acts on a portion of the screen.

	return !_FDisplayIs640480() && !_fSwitchedResolution;
}

/***************************************************************************
	Disable the application accelerators
***************************************************************************/
void APP::DisableAccel(void)
{
	AssertBaseThis(0); // Gets called from destructors

	if (_cactDisable == 0)
		{
#ifdef WIN
		_haccel = vwig.haccel;
		vwig.haccel = _haccelGlobal;
#else
		RawRtn();
#endif
		}

	_cactDisable++;

}

/***************************************************************************
	Enable the application accelerators
***************************************************************************/
void APP::EnableAccel(void)
{
	AssertBaseThis(0); // Gets called from destructors
	Assert(_cactDisable > 0, "Enable called w/o a disable");

	_cactDisable--;

	if (_cactDisable == 0)
		{
#ifdef WIN
		vwig.haccel = _haccel;
#else
		RawRtn();
#endif
		}

}

/***************************************************************************
 *
 * Handle disable accelerator command
 *
 * Parameters:
 *  pcmd - Pointer to the command to process.
 *
 * Returns:
 *  fTrue if it handled the command, else fFalse.
 *
 **************************************************************************/
bool APP::FCmdDisableAccel(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	DisableAccel();
	return fTrue;
}


/***************************************************************************
 *
 * Handle enable accelerator command
 *
 * Parameters:
 *  pcmd - Pointer to the command to process.
 *
 * Returns:
 *  fTrue if it handled the command, else fFalse.
 *
 **************************************************************************/
bool APP::FCmdEnableAccel(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);
	EnableAccel();
	return fTrue;
}


/******************************************************************************
	FCmdInvokeSplot
		Invokes the splot machine.
	
	Arguments:
		PCMD pcmd
			rglw[0]  --  contains the GOB id of the parent of the Splot machine
			rglw[1]  --  contains the GOB id of the Splot machine itself
	
	Returns: fTrue always
	
************************************************************ PETED ***********/
bool APP::FCmdInvokeSplot(PCMD pcmd)
{
	AssertThis(0);
	AssertVarMem(pcmd);

	PSPLOT psplot;

	psplot = SPLOT::PsplotNew(pcmd->rglw[0], pcmd->rglw[1], _pcrmAll);
	if (psplot == pvNil)
		PushErc(ercSocCantInitSplot);

	return fTrue;
}


/***************************************************************************
	Handoff a movie to the app so it can pass it on to the studio
***************************************************************************/
void APP::HandoffMovie(PMVIE pmvie)
{
	AssertThis(0);
	AssertPo(pmvie, 0);

	ReleasePpo(&_pmvieHandoff);
	_pmvieHandoff = pmvie;
	_pmvieHandoff->AddRef();
}

/***************************************************************************
	Grab the APP movie
***************************************************************************/
PMVIE APP::PmvieRetrieve(void)
{
	AssertThis(0);

	PMVIE pmvie = _pmvieHandoff;

	_pmvieHandoff = pvNil;  //  Caller now owns this pointer
	return pmvie;
}


#ifdef BUG1085
/******************************************************************************
	HideCurs
	ShowCurs
	PushCurs
	PopCurs
	
	    Some simple cursor restoration functionality, for use when a modal
		topic comes up.  Assumes that you won't try to mess with the cursor
		state while you've got the old cursor state pushed, and that you won't
		try to push the cursor state while you've already got the old cursor
		state pushed.
	
************************************************************ PETED ***********/
void APP::HideCurs(void)
{
	AssertThis(0);

	Assert(_cactCursHide != ivNil, "Can't hide cursor in Push/PopCurs pair");
	_cactCursHide++;
	APP_PAR::HideCurs();
}

void APP::ShowCurs(void)
{
	AssertThis(0);

	Assert(_cactCursHide > 0, "Unbalanced ShowCurs call");
	_cactCursHide--;
	APP_PAR::ShowCurs();
}

void APP::PushCurs(void)
{
	AssertThis(0);

	Assert(_cactCursHide != ivNil, "Can't nest cursor restoration");
	_cactCursSav = _cactCursHide;
	while (_cactCursHide)
		ShowCurs();
	_cactCursHide = ivNil;
}

void APP::PopCurs(void)
{
	AssertThis(0);

	Assert(_cactCursHide == ivNil, "Unbalanced cursor restoration");
	_cactCursHide = 0;
	while (_cactCursHide < _cactCursSav)
		HideCurs();
}
#endif // BUG1085


#ifdef DEBUG
/***************************************************************************
	Assert the validity of the APP
***************************************************************************/
void APP::AssertValid(ulong grf)
{
	APP_PAR::AssertValid(0);
	AssertNilOrPo(_pstdio, 0);
	AssertNilOrPo(_ptatr, 0);
	AssertNilOrPo(_pmvieHandoff, 0);
	AssertPo(_pcfl, 0);
	AssertPo(_pgstStudioFiles, 0);
	AssertPo(_pgstBuildingFiles, 0);
	AssertPo(_pgstSharedFiles, 0);
	AssertPo(_pgstApp, 0);
	AssertPo(_pkwa, 0);
	AssertPo(_pgstApp, 0);
	AssertPo(_pcrmAll, 0);
	AssertPo(_pglicrfBuilding, 0);
	AssertPo(_pglicrfStudio, 0);
	AssertNilOrPo(_pcex, 0);
}

/***************************************************************************
	Mark memory used by the APP
***************************************************************************/
void APP::MarkMem(void)
{
	AssertThis(0);
	APP_PAR::MarkMem();
	MarkMemObj(vptagm);
	MTRL::MarkShadeTable();
	TDT::MarkActionNames();
	MarkMemObj(_pstdio);
	MarkMemObj(_ptatr);
	MarkMemObj(_pmvieHandoff);
	MarkMemObj(_pcfl);
	MarkMemObj(_pgstStudioFiles);
	MarkMemObj(_pgstBuildingFiles);
	MarkMemObj(_pgstSharedFiles);
	MarkMemObj(_pgstApp);
	MarkMemObj(_pkwa);
	MarkMemObj(_pgstApp);
	MarkMemObj(_pcrmAll);
	MarkMemObj(_pglicrfBuilding);
	MarkMemObj(_pglicrfStudio);
	MarkMemObj(_pcex);
}
#endif // DEBUG


//
//
//
//  KWA (KidWorld for App) stuff begins here
//
//
//


/***************************************************************************
	KWA destructor
***************************************************************************/
KWA::~KWA(void)
{
	ReleasePpo(&_pmbmp);
}


/***************************************************************************
	Set the KWA's MBMP (for splash screen)
***************************************************************************/
void KWA::SetMbmp(PMBMP pmbmp)
{
	AssertThis(0);
	AssertNilOrPo(pmbmp, 0);

	RC rc;

	if (pvNil != pmbmp)
		pmbmp->AddRef();
	ReleasePpo(&_pmbmp);
	_pmbmp = pmbmp;
	GetRcVis(&rc, cooLocal);
	vpappb->MarkRc(&rc, this);
}

					
/***************************************************************************
	Draw the KWA's MBMP, if any (for splash screen)
***************************************************************************/
void KWA::Draw(PGNV pgnv, RC *prcClip)
{
	AssertThis(0);
	AssertPo(pgnv, 0);
	AssertVarMem(prcClip);

	if (pvNil != _pmbmp)
		pgnv->DrawMbmp(_pmbmp, 0, 0);
}


/***************************************************************************
	Find a file given a string.
***************************************************************************/
bool KWA::FFindFile(PSTN pstnSrc, PFNI pfni)
{
	AssertThis(0);
	AssertPo(pstnSrc, 0);
	AssertPo(pfni, 0);

	return vptagm->FFindFile(vapp.SidProduct(), pstnSrc, pfni, FAskForCD());
}

/***************************************************************************
	Do a modal help topic.
***************************************************************************/
bool KWA::FModalTopic(PRCA prca, CNO cnoTopic, long *plwRet)
{
	AssertThis(0);
	AssertPo(prca, 0);
	AssertVarMem(plwRet);

	bool fRet;

	// Take any special action here if necessary, before the
	// modal help topic is displayed. (Eg, disable help keys).

	// Now take the default action.
#ifdef BUG1085
	vapp.PushCurs();
	fRet = KWA_PAR::FModalTopic(prca, cnoTopic, plwRet);
	vapp.PopCurs();
#else
	fRet = KWA_PAR::FModalTopic(prca, cnoTopic, plwRet);
#endif  // !BUG1085

	// Let script know that the modal topic has been dismissed.
	// This is required for the projects.
	vpcex->EnqueueCid(cidModalTopicClosed, 0, 0, *plwRet);

	return fRet;
}

#ifdef DEBUG
/***************************************************************************
	Assert the validity of the KWA
***************************************************************************/
void KWA::AssertValid(ulong grf)
{
	KWA_PAR::AssertValid(0);
	AssertNilOrPo(_pmbmp, 0);
}


/***************************************************************************
	Mark memory used by the KWA
***************************************************************************/
void KWA::MarkMem(void)
{
	AssertThis(0);
	KWA_PAR::MarkMem();
	MarkMemObj(_pmbmp);
}
#endif // DEBUG
