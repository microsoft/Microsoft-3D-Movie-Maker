/***************************************************************************
**
**      File:                   SETUPAPI.H
**      Purpose:                Prototypes for the Setup API.
**      Notes:
**
****************************************************************************/

#ifndef SETUPAPI_H
#define SETUPAPI_H

#define ACME_LOGFILE    1
/* Plenty of macros to make the port to Win32 easier.
 * e.g. #define _fmemcmp memcmp
 */
#include <windowsx.h>

/* Ignore the following declarators */
#define _based(seg)
#define _loadds
#define __loadds
#define __export

/* Undef CopyFile, which WIN32 defines as CopyFileA, an API */
#undef CopyFile

/*
 * In Win32, all the Api's work directly with ANSI buffers, so we no longer
 * want to use conversions.
 */
#undef OemToAnsi
#undef AnsiToOem
#define OemToAnsi(o,a) lstrcpy(a,o)
#define AnsiToOem(a,o) lstrcpy(o,a)

#ifdef _DEBUG
#define DEBUG
#endif


#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif

/* Private toolkit messages used by ACMSETUP and custom actions */

#define  STF_MESSAGE        (WM_USER + 0x8000)
#define  STF_REINITDIALOG   (STF_MESSAGE + 12)
#define  STF_ACTIVATEAPP    (STF_MESSAGE + 17)

/*
 *      Setup Error Codes.  These are the values passed to the user supplied
 *      error function.
 */
typedef unsigned int  SEC;              /* Setup Error Code */
typedef SEC *         PSEC;             /* Ptr to Setup Error Code */

#define secOk         ((SEC)   0)
#define secErr        ((SEC)1024)
#define secQuit       ((SEC)1025)


/*
 *      Window Visibility Modes - InitSetupToolkit parameter
 */
typedef UINT WVM;       /* Window Visibility Mode */

#define wvmNormal     ((WVM)0)
#define wvmMaximized  ((WVM)1)
#define wvmMinimized  ((WVM)2)


/*
 *      CoMmand Option flags
 */
typedef UINT CMO;       /* CoMmand Option */

#define cmoVital      ((CMO)0x1)
#define cmoCopy       ((CMO)0x2)
#define cmoUndo       ((CMO)0x4)
#define cmoRoot       ((CMO)0x8)
#define cmoDecompress ((CMO)0x10)
#define cmoTimeStamp  ((CMO)0x20)
#define cmoReadOnly   ((CMO)0x40)
#define cmoBackup     ((CMO)0x80)
#define cmoForce      ((CMO)0x100)
#define cmoRemove     ((CMO)0x200)
#define cmoOverwrite  ((CMO)0x400)
#define cmoAppend     ((CMO)0x800)
#define cmoPrepend    ((CMO)0x1000)
#define cmoShared     ((CMO)0x2000)
#define cmoSystem     ((CMO)0x4000)
/* NOTE oefCabinet in COMSTF.H uses 0x8000 */
#define cmoNone       ((CMO)0x0)
#define cmoAll        ((CMO)0xffff)


/*
 *      File Exist Modes - DoesFileExist parameter
 *      (order is important)
 */
typedef UINT FEM;       /* File Exist Mode */

#define femExists     ((FEM)0)
#define femRead       ((FEM)1)
#define femWrite      ((FEM)2)
#define femReadWrite  ((FEM)3)
#define femMin        ((FEM)0)
#define femMax        ((FEM)4)


/*
 *      Size ChecK Modes - SetSizeCheckMode parameter
 */
typedef UINT SCKM;      /* Size ChecK Mode */

#define sckmOff       ((SCKM)0)
#define sckmOnIgnore  ((SCKM)1)
#define sckmOnFatal   ((SCKM)2)


/*
 *      Silent Mode - SetSilentMode parameter
 */
typedef UINT SM;        /* Silent Mode */

#define smNormal        ((SM)0x0000)
#define smNoDialogs     ((SM)0x0001)
#define smNoCopyGauge   ((SM)0x0002)
#define smTotallySilent ((SM) ~(0))


/*
 *      Current File Version - DoesSharedFileNeedCopying return type
 *
 *      NOTE - order is important - see FIBSECfv(cfv) macro below.
 */
typedef enum _CFV               /* Current File Version */
	{
	cfvNoFile,
	cfvLoVer,
	cfvDiffLang,
	cfvEqVer,
	cfvHiVer,
	cfvUnknown
	}  CFV;
typedef CFV * PCFV;             /* Ptr to Current File Version */
#define FIBSECfv(cfv)   (cfv > cfvEqVer)


typedef UINT WMD;               /* Windows MoDe */

#define wmdReal      0
#define wmdStandard  1
#define wmdEnhanced  2
#define wmdNT            3
#define wmdUnknown  10


typedef UINT CPU;               /* type of Central Processor Unit (or coprocessor) */

#define cpu8086          0
#define cpu80186         1
#define cpu80286         2
#define cpu80386         3
#define cpu80486         4
#define cpuR4000     5  /* for compatibility with CDrivers : Setup 2.0 */
						/* Ideally this should have been ifdef'd  to > 5 */
#define cpuMin           0
#define cpuMax           5
#define cpuUnknown  20


typedef VOID ( WINAPI *LPFNERROREXIT )( UINT );

#define SETUPAPI        WINAPI
#define SZ                      char *
#define CSZ                     char * const
#define SZC                     const char *
#define CSZC            const char * const

extern VOID SETUPAPI AddBlankToBillboardList ( LONG lTicks );
extern VOID SETUPAPI AddDos5Help ( SZ szProgName, SZ szProgHelp, CMO cmo );
extern VOID SETUPAPI AddLineToRestartFile ( SZ szLine );
extern VOID SETUPAPI AddListItem ( SZ szSymbol, SZ szItem );
extern VOID SETUPAPI AddSectionFilesToCopyList ( SZ szSect, SZ szSrcDir, SZ szDstDir );
extern VOID SETUPAPI AddSectionKeyFileToCopyList ( SZ szSect, SZ szKey, SZ szSrcDir, SZ szDstDir );
extern VOID SETUPAPI AddSpecialFileToCopyList ( SZ szSect, SZ szKey, SZ szSrcDir, SZ szDstPath );
extern VOID SETUPAPI AddSrcFileWithAttribsToCopyList ( SZ szSect, SZ szKey, SZ szSrcPath, SZ szDstPath );
extern VOID SETUPAPI AddShareToAutoexec ( SZ szSrc, SZ szDst, BOOL fConfig, BOOL fNeedsNewLine, SZ szDefPath, UINT wMinLock, UINT wMinBuf, UINT wMaxLock, UINT wMaxBuf, CMO cmo );
extern VOID SETUPAPI AddToBillboardList ( SZ szDll, UINT idDlg, SZ szProc, LONG lTicks );
extern VOID SETUPAPI BackupFile ( SZ szFullPath, SZ szBackup );
extern UINT SETUPAPI CbGetListItem ( SZ szSym, UINT uiItem, SZ szItem, UINT cbMax );
extern UINT SETUPAPI CbGetSymbolValue ( SZ szSymbol, SZ szValue, UINT cbMax );
extern VOID SETUPAPI ClearBillboardList ( VOID );
extern VOID SETUPAPI ClearCopyList ( VOID );
extern int  SETUPAPI CompareFileVersions ( SZ szVer1, SZ szVer2 );
extern VOID SETUPAPI CopyFile ( SZ szFullPathSrc, SZ szFullPathDst, CMO cmo, BOOL fAppend );
extern VOID SETUPAPI CopyFilesInCopyList ( BOOL fDeterrentEnabled );
extern VOID SETUPAPI CreateDir ( SZ szDir, CMO cmo );
extern VOID SETUPAPI CreateIniKeyValue ( CSZC cszcFile, CSZC cszcSect, CSZC cszcKey, CSZC cszcValue, CMO cmo );
extern VOID SETUPAPI CreateProgmanGroup ( SZ szGroup, SZ szPath, CMO cmo );
extern VOID SETUPAPI CreateProgmanItem ( SZ szGroup, SZ szItem, SZ szCmd, SZ szOther, CMO cmo );
extern VOID SETUPAPI CreateSysIniKeyValue ( SZ szFile, SZ szSect, SZ szKey, SZ szValue, CMO cmo );
extern VOID SETUPAPI DebugMessagesOn ( BOOL fMsgOn );
extern VOID SETUPAPI DeleteProgmanGroup ( SZ szGroup, CMO cmo );
extern VOID SETUPAPI DeleteProgmanItem ( SZ szGroup, SZ szItem, CMO cmo, BOOL fDelEmptyGroup );
extern BOOL SETUPAPI DeleteRegKeyValue32 ( HKEY hRootKey, CSZC cszcSubKey, CSZC cszcValueName );
extern int  SETUPAPI DoMsgBox ( CSZC cszcText, CSZC cszcCaption, UINT uiType );
extern int  SETUPAPI DoMsgBoxProc( CSZC cszcText, CSZC cszcCaption, UINT uiType );
extern BOOL SETUPAPI DoesDirExist ( SZ szDir );
extern BOOL SETUPAPI DoesFileExist ( SZ szFileName, FEM fem );
extern BOOL SETUPAPI DoesInfSectionExist ( SZ szSect );
extern BOOL SETUPAPI DoesInfSectionKeyExist ( SZ szSect, SZ szKey );
extern BOOL SETUPAPI DoesIniKeyExist ( SZ szFile, SZ szSect, SZ szKey );
extern BOOL SETUPAPI DoesIniSectionExist ( SZ szFile, SZ szSect );
extern CFV  SETUPAPI DoesSharedFileNeedCopying ( VOID );
extern BOOL SETUPAPI DoesSysIniKeyValueExist ( SZ szFile, SZ szSect, SZ szKey, SZ szValue );
extern VOID SETUPAPI DumpCopyList ( SZ szFile );
extern VOID SETUPAPI EndSetupToolkit ( VOID );
extern BOOL SETUPAPI ExitExecRestart ( VOID );
extern BOOL SETUPAPI FAddLineToRestartFile ( SZ szLine );
extern BOOL SETUPAPI FAddListItem ( SZ szSym, SZ szItem );
extern BOOL SETUPAPI FCloseHelp ( VOID );
extern BOOL SETUPAPI FGetShareParamsFromFile ( CSZC cszcFile, BOOL fConfig, UINT * puiLock, UINT * puiBuf, BOOL * pfAnyShare, SZ szPath, UINT cbBuf );
extern BOOL SETUPAPI FixupWinIniMsappsSection ( VOID );
extern BOOL SETUPAPI FLanguageMismatchInf ( SZ szSection, SZ szKey, SZ szDestPath );
extern BOOL SETUPAPI FLongFileNamesSuppressed ( VOID );
extern BOOL SETUPAPI FOpenLogFile ( CSZC cszcFile, BOOL fAppend );
extern BOOL SETUPAPI FRegCloseKey (HKEY hKey);
extern BOOL SETUPAPI FGetKeyInfo(HKEY hkey,LPDWORD lpcSubKeys,LPDWORD lpcValues);
extern BOOL SETUPAPI FRegCreateKey32(HKEY hRootKey, SZ szRegKey,HKEY* phKey);
extern BOOL SETUPAPI FWriteToLogFile ( CSZC cszc, BOOL fRequireLog );
extern BOOL SETUPAPI FCloseLogFile ( VOID );
extern BOOL SETUPAPI ForceNewDialog ( BOOL fForce );
extern BOOL SETUPAPI HandleOOM ( VOID );
extern UINT SETUPAPI HandleSharedFile ( SZ szInfSect, SZ szInfKey, CSZC cszcRegKey,
								CSZC cszcFullPath, HKEY hRegRootKey, SZ szRegDbKey,
								SZ szRegValueName, UINT uiRegDbField, SZ szWinIniSect,
								SZ szWinIniKey, UINT uiWinIniField, SZ szBuf, UINT cbBufMax);
extern BOOL SETUPAPI FRemoveSymbol ( SZ szSymbol );
extern BOOL SETUPAPI FReplaceListItem ( SZ szSym, UINT uiItem, SZ szItem );
extern BOOL SETUPAPI FSetSymbolValue ( SZ szSymbol, SZ szValue );
extern UINT SETUPAPI FindFileInTree ( SZ szFile, SZ szDir, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI FindFileUsingFileOpen ( SZ szFile, SZ szPath, UINT cbPathMax );
extern UINT SETUPAPI FindTargetOnEnvVar ( SZ szFile, SZ szEnvVar, SZ szBuf, UINT cbBufMax );
extern VOID SETUPAPI ForceRestartOn ( VOID );
extern VOID SETUPAPI FreeMemInf ( VOID * pvMemInf );
extern char SETUPAPI GetConfigLastDrive ( VOID );
extern UINT SETUPAPI GetConfigNumBuffers ( VOID );
extern UINT SETUPAPI GetConfigNumFiles ( VOID );
extern UINT SETUPAPI GetConfigRamdriveSize ( VOID );
extern UINT SETUPAPI GetConfigSmartdrvSize ( VOID );
extern LONG SETUPAPI GetCopyListCost ( SZ szExtraList, SZ szCostList, SZ szNeedList );
extern UINT SETUPAPI GetDOSMajorVersion ( VOID );
extern UINT SETUPAPI GetDOSMinorVersion ( VOID );
extern UINT SETUPAPI GetDateOfFile ( SZ szFile, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI GetDayFromDate ( SZ szDate );
extern UINT SETUPAPI GetEnvVariableValue ( SZ szEnvVar, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI GetExistingFOTFileForTTF ( SZ szFile, SZ szBuf, UINT cbBufMax );
extern LONG SETUPAPI GetFreeSpaceForDrive ( SZ szDrive );
extern UINT SETUPAPI GetHourFromDate ( SZ szDate );
extern UINT SETUPAPI GetIniKeyString ( CSZC cszcFile, CSZC cszcSect, CSZC cszcKey, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI GetListItem ( SZ szListSymbol, UINT uiItem, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI GetListLength ( SZ szSymbol );
extern VOID SETUPAPI GetLocalHardDrivesList ( SZ szSymbol );
extern WORD SETUPAPI GetLocalLanguage ( VOID );
extern UINT SETUPAPI GetMinuteFromDate ( SZ szDate );
extern UINT SETUPAPI GetMonthFromDate ( SZ szDate );
extern VOID SETUPAPI GetNetworkDrivesList ( SZ szSymbol );
extern UINT SETUPAPI GetNthFieldFromIniString ( SZ szLine, UINT uiField, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI GetNumWinApps ( VOID );
extern VOID SETUPAPI GetParallelPortsList ( SZ szSymbol );
extern UINT SETUPAPI GetProcessorType ( VOID );
extern UINT SETUPAPI GetRegKeyValue32 ( HKEY hRootKey, CSZC cszcSubKey, CSZC cszcValueName, PDWORD pdwType, PBYTE pbData, UINT cbData );
extern VOID SETUPAPI GetRemovableDrivesList ( SZ szSymbol );
extern int  SETUPAPI GetScreenHeight ( VOID );
extern int  SETUPAPI GetScreenWidth ( VOID );
extern UINT SETUPAPI GetSecondFromDate ( SZ szDate );
extern UINT SETUPAPI GetSectionKeyDate ( SZ szSect, SZ szKey, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI GetSectionKeyFilename ( SZ szSect, SZ szKey, SZ szBuf, UINT cbBufMax );
extern LONG SETUPAPI GetSectionKeySize ( SZ szSect, SZ szKey );
extern UINT SETUPAPI GetSectionKeyVersion ( SZ szSect, SZ szKey, SZ szBuf, UINT cbBufMax );
extern VOID SETUPAPI GetSerialPortsList ( SZ szSymbol );
extern SM   SETUPAPI GetSilentMode ( VOID );
extern LONG SETUPAPI GetSizeOfFile ( SZ szFile );
extern UINT SETUPAPI GetSymbolValue ( SZ szSymbol, SZ szBuf, UINT cbBufMax );
extern LONG SETUPAPI GetTotalSpaceForDrive ( SZ szDrive );
extern UINT SETUPAPI GetTypeFaceNameFromTTF ( SZ szFile, SZ szBuf, UINT cbBufMax );
extern VOID SETUPAPI GetValidDrivesList ( SZ szSymbol );
extern LONG SETUPAPI GetVersionNthField ( SZ szVersion, UINT uiField );
extern UINT SETUPAPI GetVersionOfFile ( SZ szFile, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI GetWindowsDirPath ( SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI GetWindowsMajorVersion ( VOID );
extern UINT SETUPAPI GetWindowsMinorVersion ( VOID );
extern UINT SETUPAPI GetWindowsMode ( VOID );
extern UINT SETUPAPI GetWindowsSysDir ( SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI GetYearFromDate ( SZ szDate );
extern BOOL SETUPAPI Has87MathChip ( VOID );
extern BOOL SETUPAPI HasMonochromeDisplay ( VOID );
extern BOOL SETUPAPI HasMouseInstalled ( VOID );
extern HWND SETUPAPI HdlgShowHelp (UINT uiMsg, WPARAM wParam, LPARAM lParam);
extern HINSTANCE SETUPAPI HinstFrame ( VOID );
extern HWND SETUPAPI HwndFrame ( VOID );
extern VOID SETUPAPI InitLongFileNames ( VOID );
extern UINT SETUPAPI InitSetupToolkit ( SZ szCmdLine, WVM wvmVisibilityMode, BOOL fUseRegDb, LPFNERROREXIT lpfnErrorExit );
extern BOOL SETUPAPI IsCtl3dEnabled ( VOID );
extern BOOL SETUPAPI IsDirWritable ( SZ szDir );
extern BOOL SETUPAPI IsDriveLocalHard ( SZ szDrive );
extern BOOL SETUPAPI IsDriveNetwork ( SZ szDrive );
extern BOOL SETUPAPI IsDriveRemovable ( SZ szDrive );
extern BOOL SETUPAPI IsDriveValid ( SZ szDrive );
extern BOOL SETUPAPI IsDriverInConfig ( SZ szDrv );
extern BOOL SETUPAPI IsFileInUseBySystem ( SZ szFullPath );
extern BOOL SETUPAPI IsFileWritable ( SZ szFile );
extern BOOL SETUPAPI IsValidPath ( SZC szcPath );
extern BOOL SETUPAPI IsWindowsShared ( VOID );
extern VOID SETUPAPI MakeListFromProgmanGroups ( SZ szSymbol );
extern VOID SETUPAPI MakeListFromSectionDate ( SZ szSym, SZ szSect );
extern VOID SETUPAPI MakeListFromSectionFilename ( SZ szSym, SZ szSect );
extern VOID SETUPAPI MakeListFromSectionKeys ( SZ szSymbol, SZ szSect );
extern VOID SETUPAPI MakeListFromSectionSize ( SZ szSym, SZ szSect );
extern VOID SETUPAPI MakeListFromSectionVersion ( SZ szSym, SZ szSect );
extern VOID SETUPAPI PrependToPath ( SZ szSrc, SZ szDst, SZ szDir, CMO cmo );
extern VOID * SETUPAPI PVSaveMemInf ( VOID );
extern VOID SETUPAPI ReactivateSetupScript ( VOID );
extern VOID SETUPAPI ReadInfFile ( SZ szFile );
extern VOID SETUPAPI RemoveDir ( SZ szDir, CMO cmo );
extern VOID SETUPAPI RemoveFile ( SZ szFullPathSrc, CMO cmo );
extern VOID SETUPAPI RemoveIniKey ( CSZC cszcFile, CSZC cszcSect, CSZC cszcKey, CMO cmo );
extern VOID SETUPAPI RemoveIniSection ( CSZC cszcFile, CSZC cszcSect, CMO cmo );
extern VOID SETUPAPI RemoveSectionFilesToCopyList ( SZ szSect, SZ szDstDir );
extern VOID SETUPAPI RemoveSectionKeyFileToCopyList ( SZ szSect, SZ szKey, SZ szDstDir );
extern VOID SETUPAPI RemoveSpecialFileToCopyList ( SZ szSect, SZ szKey, SZ szDstPath );
extern VOID SETUPAPI RemoveSymbol ( SZ szSym );
extern VOID SETUPAPI RenameFile ( SZ szFullPath, SZ szBackup );
extern VOID SETUPAPI ReplaceListItem ( SZ szSymbol, UINT uiItem, SZ szItem );
extern BOOL SETUPAPI RestartListEmpty ( VOID );
extern VOID SETUPAPI RestoreCursor ( HCURSOR hcursorPrev );
extern VOID SETUPAPI RestoreMemInf ( VOID * pvMemInf );
extern VOID * SETUPAPI SaveMemInf ( VOID );
extern UINT SETUPAPI SearchForLocationForSharedFile ( HKEY hRegRootKey,
								SZ szRegDbKey, SZ szRegValueName, UINT uiRegDbField,
								SZ szWinIniSect, SZ szWinIniKey, UINT uiWinIniField,
								SZ szDefSharedPath, SZ szVersion, SZ szLangSrc,
								SZ szBuf, UINT cbBufMax );
extern VOID SETUPAPI SetAbout ( SZ szAbout1, SZ szAbout2 );
extern BOOL SETUPAPI SetBeepingMode ( BOOL fMode );
extern VOID SETUPAPI SetBitmap ( SZ szDll, UINT idBitmap );
extern VOID SETUPAPI SetCopyGaugePosition ( int x, int y );
extern BOOL SETUPAPI SetCopyMode ( BOOL fMode );
extern BOOL SETUPAPI SetDecompMode ( BOOL fMode );
extern BOOL SETUPAPI SetRegKeyValue32 ( HKEY hRootKey, CSZC cszcSubKey, CSZC cszcValueName, DWORD fdwType, CONST BYTE* lpbData, DWORD cbData );
extern BOOL SETUPAPI SetRegKeyValue32Ex (HKEY hKey,HKEY hRootKey, CSZC cszcRegKey, CSZC cszcValueName, DWORD fdwType,CONST BYTE* lpbData, DWORD cbData,BOOL fClose );
extern BOOL SETUPAPI GetRegKeyValue32Ex (HKEY hKey,CSZC cszcValueName, PDWORD pdwType,PBYTE pbData, UINT cbData, BOOL fClose );
extern VOID SETUPAPI SetRestartDir ( SZ szDir );
extern SM   SETUPAPI SetSilentMode ( SM smSilentMode );
extern UINT SETUPAPI SetSizeCheckMode ( SCKM sckmMode );
extern VOID SETUPAPI SetSymbolValue ( SZ szSymbol, SZ szValue );
extern BOOL SETUPAPI SetTimeValue ( UINT uiHours, UINT uiMinutes, UINT uiSeconds );
extern VOID SETUPAPI SetTitle ( SZ sz );
extern VOID SETUPAPI ShowProgmanGroup ( SZ szGroup, UINT Cmd, CMO cmo );
extern HCURSOR SETUPAPI ShowWaitCursor ( VOID );
extern VOID SETUPAPI StampResource ( SZ szFilePath, UINT uiResType, UINT uiResId, SZ szData, UINT cbData, BOOL fNotCopied );
extern VOID SETUPAPI SuppressLongFileNames ( VOID );
extern VOID SETUPAPI UIPop ( UINT cDlgs );
extern VOID SETUPAPI UIPopAll ( VOID );
extern UINT SETUPAPI UIStartDlg ( SZ szDll, UINT Dlg, SZ szDlgProc, UINT HelpDlg, SZ szHelpProc, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI UIStartExeDlg ( HINSTANCE hinstExe, UINT Dlg, FARPROC lpfnDlgProc, UINT HelpDlg, FARPROC lpfnHelpProc, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI UIStartExeDlgWinHelp ( HINSTANCE hinstExe, UINT Dlg, FARPROC lpfnDlgProc, SZ szFile, DWORD* rgdwHelpID, SZ szBuf, UINT cbBufMax );
extern UINT SETUPAPI UsGetListLength ( SZ szSym );
extern VOID SETUPAPI WriteLogDivider ( VOID );
extern UINT SETUPAPI EnsurePathExists( SZ szFullPathDst );
extern BOOL SETUPAPI FRemoveOurDir ( SZ szFullPath, BOOL fStrip );
#if defined(UI_HOOK)
extern HMODULE SETUPAPI HmodUIHookDll(VOID);
#endif /* UI_HOOK */

#undef  SZ
#undef  CSZ
#undef  SZC
#undef  CSZC

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif

#endif  /* SETUPAPI_H */
