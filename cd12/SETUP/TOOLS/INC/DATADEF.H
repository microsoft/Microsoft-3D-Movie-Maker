/***************************************************************************
**
**	File:			DataDef.h
**	Purpose:		This defines the data structures and types used by
**					the Object class.
**	Notes:
**
**			            ===  W A R N I N G  ===
**
**			CUSTOM ACTION WRITERS: DO *NOT* MODIFY THIS FILE,
**				OR ANY OF THE STANDARD ACME HEADER FILES.
**				DOING SO MAY CAUSE YOUR SETUP TO FAIL!
**
****************************************************************************/

#ifndef DATADEF_H
#define DATADEF_H


/* WARNING: This version number is used to detect old billboard/custom
*	action .DLL's.  It MUST be updated by Acme Development anytime a
*	data structure, defined value, API prototype, etc. changes in any of
*	the following Acme include files, requiring the client's .DLL to be
*	recompiled:
*
*		cui.h, datadef.h, setupapi.h, setupkit.h, stdtypes.h
*
*	NOTE: This version number does not necessarily represent the most
*	recent build of Acme.  It merely represents the build in which the
*	Acme header files last changed.
*/
#define SZ_DATADEF_VER		"1.20.0.1705"

/* Prototype for the function that returns SZ_DATADEF_VER.  This function
*	must be implemented and exported from every billboard/custom action
*	.DLL called by Acme.  Used to validate that the .DLL was built with
*	the correct version of the Acme headers files.
*/
extern SZ WINAPI SzGetDatadefVer ( VOID );

/*
*	Ptr to SzGetDatadefVer function.
*/
typedef SZ (WINAPI* PFNDATADEFVER)( VOID );


/*
 *	Object Reference typedefs and defines.
 */
typedef UINT     OR;	/* Object Reference */
typedef OR *     POR;	/* Ptr to Object Reference */
typedef POR      RGOR;	/* Array of Object References */

typedef struct _LOR		/* List of Object References */
	{
	UINT cor;
	RGOR rgor;
	}  LOR;
typedef LOR * PLOR;		/* Ptr to a List of Object References */
typedef PLOR * PPLOR;	/* Ptr to Ptr to a List of Object References */

#define orNil  ((OR)Nil)

/*
 *	Various enum typedefs.
 */
typedef enum _OIS		/* Object Internal State */
	{
	oisUnknown,
	oisNotInTree,
	oisNotYetInstalled,
	oisInstalledByUs,
	oisToBeInstalled,
	oisToBeRemoved,
	oisInstalledBySomebodyElse
	}  OIS;
typedef OIS * POIS;		/* Ptr to Object Internal State */

typedef enum _OT		/* Object Type */
	{
	otUnknown,
	otAddBillboard,
	otAddIncKeyIniLine,
	otAddIniLine,
	otAddProgmanItem,
	otAddProgmanItemQuiet,
	otAddRegData,
	otCopyBindExe,
	otAddPathToAutoexec,
	otAppendIniLine,
	otAppMainDlg,
	otAppSearch,
	otCompanionFile,
	otCopyFile,
	otCopyIniValue,
	otCopyIniValueToReg,
	otCopySection,
	otCreateDir,
	otCreateIniLine,
	otCustomAction,
	otCustomDlg,
	otDepend,
	otDetectIniLine,
	otDetectOlderFile,
	otGroup,
	otInstallShortcut,
	otInstallComplianceFile,
	otInstallFontFile,
	otInstallOLE,
	otInstallProofLex,
	otInstallProofTool,
	otInstallShared,
	otInstallSysFile,
	otInstallTTFFile,
	otOptionDlg,
	otRemoveFile,
	otRemoveIniLine,
	otRemoveRegEntry,
	otRemoveSection,
	otSearchDrives,
	otSearchEnv,
	otSearchIni,
	otSearchOpen,
	otSearchReg,
	otSelfReg,
	otSelfUnreg,
	otStampCDInfo,
	otRemoveDir,
	otStampRes,
	otUseSharedMode,
	otWriteTableFile,
	otYesNoDlg,
	otNone,
	otDone,
	otError,
	otDependAsk
	}  OT;
typedef OT *  POT;		/* Ptr to Object Type */

typedef enum _YN		/* Yes/No */
	{
	ynUnknown,
	ynYes,
	ynNo,
	ynNone
	}  YN;
typedef YN * PYN;		/* Ptr to Yes/No */

typedef enum _YNME	 	/* Yes/No/Maybe/Error result (IBSE) */
	{
	ynmeError,
	ynmeYes,
	ynmeNo,
	ynmeMaybe
	}  YNME;
typedef YNME * PYNME;	/* Ptr to Yes/No/Maybe/Error result (IBSE) */

typedef enum _ACD		/* Allow Configurable Directory */
	{
	acdUnknown,
	acdYes,
	acdYesWithoutSrc,
	acdNo,
	acdNone
	}  ACD;
typedef ACD * PACD;		/* Ptr to Allow Configurable Directory */

typedef enum _VA		/* Vital Attribute */
	{
	vaUnknown,
	vaVital,
	vaNotVital,
	vaNone
	}  VA;
typedef VA * PVA;		/* Ptr to Vital Attribute */

typedef enum _SA		/* Shared Attribute */
	{
	saUnknown,
	saShared,
	saNotShared,
	saNone
	}  SA;
typedef SA * PSA;		/* Ptr to Shared Attribute */

typedef enum _IBY		/* Installed BY */
	{
	ibyUnknown,
	ibyUs,
	ibySomebodyElse,
	ibyNotYetInstalled,
	ibyNone
	}  IBY;
typedef IBY * PIBY;		/* Ptr to Installed BY */

typedef enum _SIM		/* Setup Installation Mode */
	{
	simUsageM,
	simFloppyM,
	simAdminM,
	simPostAdminM,
	simMaintM,
	simMaintDeleteM,
	simMaintDeleteAllM,
	simMaintReinstallM,
	simMaintReserializeM
	}  SIM;
typedef SIM * PSIM;		/* Ptr to Setup Installation Mode */

typedef enum _WVT		/* Windows Version Type */
	{
	wvtUnknown,
	wvtWin30,
	wvtWin31,
	wvtWfW31,
	wvtChicago,
	wvtWinNT
	}  WVT;
typedef WVT * PWVT;		/* Ptr to Windows Version Type */

typedef enum _SLU		/* Shared/Local/UserChoice */
	{
	sluUnknown,
	sluShared,
	sluLocal,
	sluUserChoice,
	sluNone
	}  SLU;
typedef SLU * PSLU;		/* Ptr to Shared/Local/UserChoice */

typedef enum _SDU		/* Server/Drive User control */
	{
	sduUnknown,
	sduServer,
	sduDrive,
	sduNone
	}  SDU;
typedef SDU * PSDU;		/* Ptr Server/Drive User control */

typedef enum _RC		/* Return Code */
	{
	rcFail,
	rcOk,
	rcCancel,
	rcRetry,
	rcQuit,
	rcDoDefault
	}  RC;
typedef RC * PRC;		/* Ptr to Return Code */

typedef enum _DBP		/* Dialog Box Parent */
	{
	dbpTop,		/* No parent */
	dbpAppMain,	/* AppMain parent */
	dbpOther	/* CustomDlg, OptionDlg, AppSearchDlg, MsappsDirDlg, etc. */
	}  DBP;
typedef DBP * PDBP;		/* Ptr to Dialog Box Parent */

typedef enum _SMA		/* SetMode Argument */
	{
	smaOff,
	smaOn,
	smaReinstall
	}  SMA;
typedef SMA * PSMA;		/* Ptr to SetMode Argument */


typedef LONG     SCB;		/* Signed Count of Bytes */
typedef SCB *    PSCB;		/* Ptr to Signed Count of Bytes */
typedef SCB *    RGSCB;		/* Array of Signed Count of Bytes */
typedef PSCB *   PPSCB;		/* Ptr to Ptr to Signed Count of Bytes */
typedef PSCB *   RGPSCB;	/* Array of Ptrs to Signed Count of Bytes */
typedef RGSCB *  PRGSCB;	/* Ptr to Array of Signed Count of Bytes */
typedef RGPSCB * PRGPSCB;	/* Ptr to Array of Ptrs to SCBs */


/*
 *	The structure for the object member data.
 */
typedef struct _OD		/* Object Data */
	{
	OR   or;
	OIS  ois;
	OT   ot;
	YN   ynQuietMode;
	SZ   szTitle;
	SZ   szDescription;
	SZ   szObjectData;
	SZ   szBitmapId;
	VA   va;
	SA   sa;
	ACD  acd;
	SZ   szTblDstDir;
	OR   orCheckDir;
	SZ   szDstDir;
	BOOL fDstDirAllocated;	/* REVIEW: Flag instead of BOOLs? */
	BOOL fDstDirSet;
	BOOL fDstDirBase;
	BOOL fDstDirUserChoice;
	YNME ynmeIBSEState;
	BOOL fVisitedIBSE;
	IBY  iby;
	SZ   szInstallData;
	SZ   szInstallDir;
#ifdef __cplusplus
   _OD();						/* Default constructor (to avoid warning). */
#endif
	}  OD;
typedef const OD * POD;		/* Ptr to Object Data (const) */


/*
**  WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING
**	adding a camf anywhere but at the end means all existing
**	CustomActions need to be rebuilt or they will get all kinds of
**	bizarre problems because their camf values don't match ours!
**
**	This may be necessary because of the definition of
**	camfActionHandlerMax (see below) but at the very least we need to
**	strongly warn our clients to recompile.
*/
typedef enum _CAMF			/* Custom Action Member Function */
	{
	camfInitializeObject,
	camfInitializeTrigger,
	camfInitFromProof,
	camfInitSearchObject,
	camfSearchForObject,
	camfAnswerDependClause,
	camfAnswerDependRemove,
	camfAnswerCompanion,
	camfSetDstDirOfTrigger,
	camfPropagateDstDir,
	camfCalcDstDir,
	camfSelectModeOfObject,
	camfSetModeOfObject,
	camfSetModeOfTrigger,
	camfCheckObjectIBSE,
	camfFinalizeObject,
	camfAddToCopyList,
	camfGetNonCopyListCost,
	camfGetCost,
	camfGetOODSCostStr,
	camfGetOODSCostNum,
	camfAddBillBoards,
	camfDoNonVisualMods,
	camfDoVisualMods,
	camfPostInstall,
	camfDtor,
	camfSelectableSubOpts,
	camfSetRgfChecked,
	camfGetUIDstDirObj,
	camfGetCAddCRemove,

/*
**  WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING
**	adding a camf anywhere but at the end means all existing
**	CustomActions need to be rebuilt or they will get all kinds of
**	bizarre problems because their camf values don't match ours!
**
**	This may be necessary because of the definition of
**	camfActionHandlerMax (see below) but at the very least we need to
**	strongly warn our clients to recompile.
*/
	camfSetOisState,
	camfSetInstallData,
	camfSetDstDir,
	camfSetDstDirWithSubdir,
	camfSetDstDirUserChoice,
	camfProcessDstDirTree,
	camfSetDstDirInTree,
	camfSetIBSEState,
	camfSetVisitedIBSE,
	camfSetIbyState,
	camfGetObjData,

/*
**  WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING
**	adding a camf anywhere but at the end means all existing
**	CustomActions need to be rebuilt or they will get all kinds of
**	bizarre problems because their camf values don't match ours!
**
**	This may be necessary because of the definition of
**	camfActionHandlerMax (see below) but at the very least we need to
**	strongly warn our clients to recompile.
*/
	camfError
	}	CAMF;
typedef CAMF * PCAMF;	/* Ptr to Custom Action Member Function */

#define  camfActionHandlerMax  camfSetOisState
#define  camfCallbackMax       camfError


typedef void * PCAMFD;	/* Ptr to Custom Action Member Function Data */

/*
**	Ptr to Custom Action Call-Back Function.
*/
typedef RC (WINAPI* PFNCACB)( OR or, CAMF camf, PCAMFD pcamfd );


/*
 *	The structure for the global class data.
 *
 *	*** NOTE: When a new field is added, be sure to update Object::cd
 *		initialization in object.cpp. ***
 */
typedef struct _CD		/* Class Data */
	{
	CHAR rgchAppName          [cchSzMax];
	CHAR rgchAppVersion       [cchSzMax];
	CHAR rgchFrameBitmap      [cchSzMax];
	CHAR rgchFrameCaption     [cchSzMax];
	CHAR rgchDialogCaptionBase[cchSzMax];
	CHAR rgchUsageString      [cchSzMax];
	CHAR rgchAboutBoxString   [cchSzMax];
	CHAR rgchModules          [cchSzMax];
	CHAR rgchSrcDir           [_MAX_PATH];
	CHAR rgchMSAPPSLocation   [_MAX_PATH];
	CHAR rgchMSAPPSNetServer  [_MAX_PATH];
	CHAR rgchMSAPPSNetPath    [_MAX_PATH];
	CHAR rgchInfFile          [cchSzMax];
	CHAR rgchHlpFile          [cchSzMax];
	CHAR rgchCompliance       [cchSzMax];
	CHAR rgchCDInfo           [cchSzMax];
	CHAR rgchSetupVersion     [cchSzMax];
	CHAR rgchResponseFile     [cchSzMax];
	SZ   szComplianceDll;
	SZ   szComplianceProc;
	SZ   szComplianceData;
	SLU  sluMSAPPSMode;
	SLU  sluModeCur;
	YN   ynSluModeChecked;
	UINT cObjectsMax;
	OR   orAdminMode;
	OR   orFloppyMode;
	OR   orNetworkMode;
	OR   orMaintenanceMode;
	OR   orBatchMode;
	PLOR plorAdminMode;
	PLOR plorFloppyMode;
	PLOR plorNetworkMode;
	PLOR plorMaintenanceMode;
	PLOR plorBatchMode;

	SIM  simSetupMode;
	DWORD dwMachineId;
	UINT uInstallCount;
	UINT uCmdInstallCount;
	UINT uCmdBigButton;
	WVT  wvtWinVerType;
	CHAR rgchStfSrcDir        [_MAX_PATH];
	CHAR rgchStfCwdDir        [_MAX_PATH];
	CHAR rgchStfWinDir        [_MAX_PATH];
	CHAR rgchStfSysDir        [_MAX_PATH];
	CHAR rgchStfSys16Dir      [_MAX_PATH];
	CHAR rgchStfSharedDir	  [_MAX_PATH];
	BOOL fIsWindowsShared;
	BOOL fIsSysDirWritable;
	BOOL fNoLongFileNames;
	BOOL fLaunchRegWizard;
	BOOL fPiracyDeterrenceEnabled;
	CHAR chMSAPPSDrive;
	SDU  sduMSAPPSConnect;
	PFNCACB pfncacb;
	BOOL fNetMaintMode;
	OR   orNetMaintenanceMode;
	PLOR plorNetMaintenanceMode;
	BOOL fWorkstationButtonSelected;
	BOOL fAvailableIMEControl;
	OR 	 orUseSharedModeObj;
	WORD wInstallDate;
	WORD wInstallTime;
	BOOL fDisableOODSInstallNowButton;
	CHAR rgchModules2         [cchSzMax];
	CHAR rgchStfWin95PFDir        [_MAX_PATH];
	BOOL fNoBinding;
	}  CD;
typedef const CD * PCD;		/* Ptr to Class Data (const) */


/* Custom Action Member Function Data structs:
*/

typedef struct _CAMFDInitializeObject
	{
	LPVOID lpvPriv;
	}  CAMFDInitializeObject;
typedef CAMFDInitializeObject * PCAMFDInitializeObject;

typedef struct _CAMFDInitializeTrigger
	{
	LPVOID lpvPriv;
	}  CAMFDInitializeTrigger;
typedef CAMFDInitializeTrigger * PCAMFDInitializeTrigger;

typedef struct _CAMFDInitFromProof
	{
	LPVOID lpvPriv;
	}  CAMFDInitFromProof;
typedef CAMFDInitFromProof * PCAMFDInitFromProof;


typedef struct _CAMFDInitSearchObject
	{
	LPVOID lpvPriv;
	BOOL fCheckOnly;
	}  CAMFDInitSearchObject;
typedef CAMFDInitSearchObject * PCAMFDInitSearchObject;

typedef struct _CAMFDSearchForObject
	{
	LPVOID lpvPriv;
	SZ szFilename;
	}  CAMFDSearchForObject;
typedef CAMFDSearchForObject * PCAMFDSearchForObject;

typedef struct _CAMFDAnswerDependClause
	{
	LPVOID lpvPriv;
	BOOL   fRes;
	}  CAMFDAnswerDependClause;
typedef CAMFDAnswerDependClause * PCAMFDAnswerDependClause;

typedef struct _CAMFDAnswerDependRemove
	{
	LPVOID lpvPriv;
	BOOL   fRes;
	}  CAMFDAnswerDependRemove;
typedef CAMFDAnswerDependRemove * PCAMFDAnswerDependRemove;

typedef struct _CAMFDAnswerCompanion
	{
	LPVOID lpvPriv;
	BOOL   fRes;
	}  CAMFDAnswerCompanion;
typedef CAMFDAnswerCompanion * PCAMFDAnswerCompanion;

typedef struct _CAMFDSetDstDirOfTrigger
	{
	LPVOID lpvPriv;
	SZ     szParentDstDir;
	}  CAMFDSetDstDirOfTrigger;
typedef CAMFDSetDstDirOfTrigger * PCAMFDSetDstDirOfTrigger;

typedef struct _CAMFDPropagateDstDir
	{
	LPVOID lpvPriv;
	SZ     szCurDstDir;
	}  CAMFDPropagateDstDir;
typedef CAMFDPropagateDstDir * PCAMFDPropagateDstDir;

typedef struct _CAMFDCalcDstDir
	{
	LPVOID lpvPriv;
	SZ     szParentDstDir;
	BOOL   fRes;
	}  CAMFDCalcDstDir;
typedef CAMFDCalcDstDir * PCAMFDCalcDstDir;

typedef struct _CAMFDSelectModeOfObject
	{
	LPVOID lpvPriv;
	DBP    dbp;
	}  CAMFDSelectModeOfObject;
typedef CAMFDSelectModeOfObject * PCAMFDSelectModeOfObject;

typedef struct _CAMFDSetModeOfObject
	{
	LPVOID	lpvPriv;
	SMA		sma;
	BOOL	fActiveUI;
	}  CAMFDSetModeOfObject;
typedef CAMFDSetModeOfObject * PCAMFDSetModeOfObject;

typedef struct _CAMFDSetModeOfTrigger
	{
	LPVOID	lpvPriv;
	SMA		sma;
	BOOL	fActiveUI;
	}  CAMFDSetModeOfTrigger;
typedef CAMFDSetModeOfTrigger * PCAMFDSetModeOfTrigger;

typedef struct _CAMFDCheckObjectIBSE
	{
	LPVOID lpvPriv;
	YNME   ynmeRes;
	}  CAMFDCheckObjectIBSE;
typedef CAMFDCheckObjectIBSE * PCAMFDCheckObjectIBSE;

typedef struct _CAMFDFinalizeObject
	{
	LPVOID lpvPriv;
	}  CAMFDFinalizeObject;
typedef CAMFDFinalizeObject * PCAMFDFinalizeObject;

typedef struct _CAMFDAddToCopyList
	{
	LPVOID lpvPriv;
	}  CAMFDAddToCopyList;
typedef CAMFDAddToCopyList * PCAMFDAddToCopyList;


typedef struct _CAMFDGetNonCopyListCost
	{
	LPVOID lpvPriv;
	PSCB   pscb;
	}  CAMFDGetNonCopyListCost;
typedef CAMFDGetNonCopyListCost * PCAMFDGetNonCopyListCost;

typedef struct _CAMFDGetCost
	{
	LPVOID lpvPriv;
	PSCB   pscb;
	SZ     szDestDir;
	}  CAMFDGetCost;
typedef CAMFDGetCost * PCAMFDGetCost;

typedef struct _CAMFDGetOODSCostStr
	{
	LPVOID lpvPriv;
	SZ     szSym;
	CHAR   chDrv;
	UINT   depth;
	BOOL fExpandGrp;
	}  CAMFDGetOODSCostStr;
typedef CAMFDGetOODSCostStr * PCAMFDGetOODSCostStr;

typedef struct _CAMFDGetOODSCostNum
	{
	LPVOID lpvPriv;
	CHAR   chDrv;
	PSCB   pscb;
	}  CAMFDGetOODSCostNum;
typedef CAMFDGetOODSCostNum * PCAMFDGetOODSCostNum;

typedef struct _CAMFDAddBillBoards
	{
	LPVOID lpvPriv;
	}  CAMFDAddBillBoards;
typedef CAMFDAddBillBoards * PCAMFDAddBillBoards;

typedef struct _CAMFDDoNonVisualMods
	{
	LPVOID lpvPriv;
	}  CAMFDDoNonVisualMods;
typedef CAMFDDoNonVisualMods * PCAMFDDoNonVisualMods;

typedef struct _CAMFDDoVisualMods
	{
	LPVOID lpvPriv;
	}  CAMFDDoVisualMods;
typedef CAMFDDoVisualMods * PCAMFDDoVisualMods;

typedef struct _CAMFDPostInstall
	{
	LPVOID lpvPriv;
	}  CAMFDPostInstall;
typedef CAMFDPostInstall * PCAMFDPostInstall;

typedef struct _CAMFDDtor
	{
	LPVOID lpvPriv;
	}  CAMFDDtor;
typedef CAMFDDtor * PCAMFDDtor;

typedef struct _CAMFDSelectableSubOpts
	{
	LPVOID lpvPriv;
	BOOL   fRes;
	}  CAMFDSelectableSubOpts;
typedef CAMFDSelectableSubOpts * PCAMFDSelectableSubOpts;

typedef struct _CAMFDSetRgfChecked
	{
	LPVOID lpvPriv;
	BOOL   fChecked;
	}  CAMFDSetRgfChecked;
typedef CAMFDSetRgfChecked * PCAMFDSetRgfChecked;

typedef struct _CAMFDGetUIDstDirObj
	{
	LPVOID lpvPriv;
	OR     orObjIDRes;
	}  CAMFDGetUIDstDirObj;
typedef CAMFDGetUIDstDirObj * PCAMFDGetUIDstDirObj;

typedef struct _CAMFDGetCAddCRemove
	{
	LPVOID lpvPriv;
	BOOL   fInit;
	BOOL   fChecked;
	PUINT  pcAdd;
	PUINT  pcRemove;
	}  CAMFDGetCAddCRemove;
typedef CAMFDGetCAddCRemove * PCAMFDGetCAddCRemove;


typedef struct _CAMFDSetOisState
	{
	OIS  oisNew;
	BOOL fRes;
	}  CAMFDSetOisState;
typedef CAMFDSetOisState * PCAMFDSetOisState;

typedef struct _CAMFDSetInstallData
	{
	SZ   sz;
	}  CAMFDSetInstallData;
typedef CAMFDSetInstallData * PCAMFDSetInstallData;

typedef struct _CAMFDSetDstDir
	{
	SZ   szDir;
	BOOL fDup;
	BOOL fRes;
	}  CAMFDSetDstDir;
typedef CAMFDSetDstDir * PCAMFDSetDstDir;

typedef struct _CAMFDSetDstDirWithSubdir
	{
	SZ   szParDir;
	BOOL fRes;
	}  CAMFDSetDstDirWithSubdir;
typedef CAMFDSetDstDirWithSubdir * PCAMFDSetDstDirWithSubdir;

typedef struct _CAMFDSetDstDirUserChoice
	{
	BOOL f;
	}  CAMFDSetDstDirUserChoice;
typedef CAMFDSetDstDirUserChoice * PCAMFDSetDstDirUserChoice;

typedef struct _CAMFDProcessDstDirTree
	{
	SZ   szCurDstDir;
	BOOL fUserChoice;
	BOOL fForceRecalc;
	BOOL fRes;
	}  CAMFDProcessDstDirTree;
typedef CAMFDProcessDstDirTree * PCAMFDProcessDstDirTree;

typedef struct _CAMFDSetDstDirInTree
	{
	SZ szParentDstDir;
	BOOL fUserChoice;
	BOOL fForceRecalc;
	}  CAMFDSetDstDirInTree;
typedef CAMFDSetDstDirInTree * PCAMFDSetDstDirInTree;

typedef struct _CAMFDSetIBSEState
	{
	YNME ynme;
	}  CAMFDSetIBSEState;
typedef CAMFDSetIBSEState * PCAMFDSetIBSEState;

typedef struct _CAMFDSetVisitedIBSE
	{
	BOOL f;
	}  CAMFDSetVisitedIBSE;
typedef CAMFDSetVisitedIBSE * PCAMFDSetVisitedIBSE;

typedef struct _CAMFDSetIbyState
	{
	IBY iby;
	}  CAMFDSetIbyState;
typedef CAMFDSetIbyState * PCAMFDSetIbyState;

typedef struct _CAMFDGetObjData
	{
	POD podRes;
	}  CAMFDGetObjData;
typedef CAMFDGetObjData * PCAMFDGetObjData;

/*
 *	LOG file Info struct.
 */
typedef struct _loginfo{
		CHAR szFileName[cchSzMax];
		}LOGINFO;

/*
**	Ptr to Custom Action Handler Function.
*/
typedef RC (WINAPI* PFNCAH)
				( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );



#ifdef __cplusplus
#define plorNull     0
#define pplorNull    0
#define rgorNull     0
#define pcamfdNull   0
#define pfncacbNull  0
#define pfncahNull   0
#define podNull      0
#define pcdNull      0
#define pscbNull     0
#define rgscbNull    0
#define ppscbNull    0
#define rgpscbNull   0
#define prgscbNull   0
#define prgpscbNull  0
#define psmaNull     0
#else
#define plorNull     ( (PLOR)    NULL )
#define pplorNull    ( (PPLOR)   NULL )
#define rgorNull     ( (RGOR)    NULL )
#define pcamfdNull   ( (PCAMFD)  NULL )
#define pfncacbNull  (           NULL )
#define pfncahNull   (           NULL )
#define podNull      ( (POD)     NULL )
#define pcdNull      ( (PCD)     NULL )
#define pscbNull     ( (PSCB)    NULL )
#define rgscbNull    ( (RGSCB)   NULL )
#define ppscbNull    ( (PPSCB)   NULL )
#define rgpscbNull   ( (RGPSCB)  NULL )
#define prgscbNull   ( (PRGSCB)  NULL )
#define prgpscbNull  ( (PRGPSCB) NULL )
#define psmaNull     ( (PSMA)    NULL )
#endif


#endif  /* DATADEF_H */
