/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
**
**	File:			SAMPCAH.C
**	Purpose:		Sample application-specific DLL Custom
**					Action Handler routines for Acme Setup.
**	Notes:
**
****************************************************************************/

#define SAMPCAH_C

#include "stdinc.h"
#include <stdlib.h>		/* _MAX_PATH */
#include <dos.h>		/* _dos_getdiskfree() */
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"
#include "parseca.h"
#include "sampcacb.h"

/* REVIEW: Replace these sample strings with your own.
*/
#define STR_DETECT_TITLE "Sample Setup Message"
#define STR_DETECT_MSG   "This product requires at least a 386 " \
			"processor, a VGA display, and Microsoft Windows 3.1."


typedef enum _WNC		/* File Needs Copying status */
	{
	wncHiVer,
	wncEqVer,
	wncLoVer,
	wncNoFile,
	wncUnknown
	}  WNC;

typedef struct _IWPFD		/* InstallWinPermFile Data */
	{
	SZ  szSect;
	SZ  szKey;
	WNC wnc;
	SCB scb;
	}  IWPFD;
typedef IWPFD FAR * PIWPFD;


#define iscbTotal	26   /* index for total visual size; 0-25 for drives */


RC FAR PASCAL DetectMinimumHardware ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData );
RC FAR PASCAL InstallWinPermFile ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData );
RC FAR PASCAL InstallSys16PermFile ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData );

RC PUBLIC RcInitializeObjectIWPF ( POD pod, PCAMFDInitializeObject pcamfd,
				SZ szData );
RC PUBLIC RcDtorIWPF ( PCAMFDDtor pcamfd );
RC PUBLIC RcCheckObjectIBSE_IWPF ( PCD pcd, POD pod,
				PCAMFDCheckObjectIBSE pcamfd );
RC PUBLIC RcCalcDstDirIWPF ( PCD pcd, POD pod, PCAMFDCalcDstDir pcamfd );
RC PUBLIC RcCalcDstDirIS16PF ( PCD pcd, POD pod, PCAMFDCalcDstDir pcamfd );
RC PUBLIC RcGetCostIWPF ( PCD pcd, POD pod, PCAMFDGetCost pcamfd );
RC PUBLIC RcGetOODSCostNumIWPF ( POD pod, PCAMFDGetOODSCostNum pcamfd );
RC PUBLIC RcFinalizeObjectIWPF ( PCD pcd, POD pod, PCAMFDFinalizeObject pcamfd );
RC PUBLIC RcAddToCopyListIWPF ( PCD pcd, POD pod, PCAMFDAddToCopyList pcamfd );
RC PUBLIC RcAnswerDependRemoveIWPF ( PCAMFDAnswerDependRemove pcamfd );
RC PUBLIC RcAnswerCompanionIWPF ( POD pod, PCAMFDAnswerCompanion pcamfd );

BOOL PUBLIC FFreePiwpfd ( PIWPFD piwpfd );
WNC  PUBLIC WncWinPermFileNeedsCopy ( SZ szInfSect, SZ szInfKey,
							SZC szcWinDir );
SZ   PUBLIC SzGetKeyFilenameRoot ( SZ szSect, SZ szKey, SZ szBuf, CB cbBuf );
BOOL PUBLIC FGetKeyFullPath ( SZ szInfSect, SZ szInfKey, SZC szcDir,
							SZ szFullPath, CB cbBufMax );
SZ   PUBLIC SzStripPath ( SZ szPath );
WNC  PUBLIC WncInfFileNeedsCopy ( SZ szInfSect, SZ szInfKey, SZ szPath );
INT  PUBLIC WCompareVersion ( SZ szVer1, SZ szVer2 );
BOOL PUBLIC FAddToCostVector ( PSCB pscb, SCB scb, SZ szDir, BOOL fRoundUp );
BOOL PUBLIC FAddToCostTotal ( PSCB pscb, SCB scb, CHAR chDrv, BOOL fRoundUp );
CB   PUBLIC CbGetDrvCluster ( UINT iDrv );


#define cpu8086  0
#define cpu80186 1
#define cpu80286 2

#define wmdStandard 1


/*
**	Purpose:
**		Custom Action Handler for DetectMinimumHardware action (sample).
**	Arguments:
**		pcd   : Ptr to class data.
**		pod   : Ptr to object data.
**		camf  :	Custom action member function type.
**		pcamfd: Ptr to custom action member function data
**		        (variable size, depends on camf type).
**	Returns:
**		For all but camfInitialize, it returns rcDoDefault.  For
**		camfInitialize, it returns rcFail or rcDoDefault.
**	Notes:
**		REVIEW: Should get its message strings and definition of 'minimum'
**		from the .STF table data, but we will hardcode it here for simplicity.
**
***************************************************************************/
RC FAR PASCAL DetectMinimumHardware ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData )
{
	static BOOL fDetectHardwareInitialized = fFalse;

	Unused(pcd);
	Unused(pod);
	Unused(pcamfd);
	Unused(szData);

	if (camf == camfInitializeObject)
		{
		/* REVIEW: Replace the sample hardware tests below with your
		*	own requirements.
		*/
		if (!fDetectHardwareInitialized
			&& (GetWindowsMajorVersion() < 3
				|| (GetWindowsMajorVersion() == 3
					&& GetWindowsMinorVersion() < 10)
				|| GetProcessorType() == cpu8086
				|| GetProcessorType() == cpu80186
				|| GetProcessorType() == cpu80286
				|| (GetScreenHeight() < 351 && GetScreenWidth() < 641)))
			{
			fDetectHardwareInitialized = fTrue;
			DoMsgBox(STR_DETECT_MSG, STR_DETECT_TITLE,
					MB_OK | MB_ICONEXCLAMATION);
			return (rcFail);
			}
		fDetectHardwareInitialized = fTrue;
		}

	return (rcDoDefault);
}


/*
**	Purpose:
**		Custom Action Handler for InstallSys16PermFile action (sample).
**	Arguments:
**		pcd   : Ptr to class data.
**		pod   : Ptr to object data.
**		camf  :	Custom action member function type.
**		pcamfd: Ptr to custom action member function data
**		        (variable size, depends on camf type).
**	Returns:
**		Appropriate RC (return code) values, including the new
**		rcDoDefault (i.e. okay, but do the default action for this
**		function too).
**	Notes:
**		This code is ripped off from InstallWinPermFile with one modification:
**		DestDir is set to %WINDOWSDIR%\SYSTEM by new function RcCalcDstDirIS16PF()
**
***************************************************************************/
RC FAR PASCAL InstallSys16PermFile ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData )
{
	RC rc = rcDoDefault;

	switch (camf)
		{

	/* Following camf members called by ACME.
	*/

		case camfInitializeObject:
			rc = RcInitializeObjectIWPF(pod, (PCAMFDInitializeObject)pcamfd,
					szData);
			break;

		case camfCalcDstDir:
			rc = RcCalcDstDirIS16PF(pcd, pod, (PCAMFDCalcDstDir)pcamfd);
			break;

		case camfFinalizeObject:
			rc = RcFinalizeObjectIWPF(pcd, pod, (PCAMFDFinalizeObject)pcamfd);
			break;

		case camfAddToCopyList:
			rc = RcAddToCopyListIWPF(pcd, pod, (PCAMFDAddToCopyList)pcamfd);
			break;

		case camfDtor:
			rc = RcDtorIWPF((PCAMFDDtor)pcamfd);
			break;


	/* Following camf members called by various actions (including this one).
	*/

		case camfCheckObjectIBSE:
			rc = RcCheckObjectIBSE_IWPF(pcd, pod,
					(PCAMFDCheckObjectIBSE)pcamfd);
			break;

		case camfGetCost:
			rc = RcGetCostIWPF(pcd, pod, (PCAMFDGetCost)pcamfd);
			break;

		case camfGetOODSCostNum:
			rc = RcGetOODSCostNumIWPF(pod, (PCAMFDGetOODSCostNum)pcamfd);
			break;

		case camfAnswerDependRemove:
			rc = RcAnswerDependRemoveIWPF((PCAMFDAnswerDependRemove)pcamfd);
			break;

		case camfAnswerCompanion:
			rc = RcAnswerCompanionIWPF(pod, (PCAMFDAnswerCompanion)pcamfd);
			break;

		default:
			break;
		}

	return (rc);
}


/*
**	Purpose:
**		Custom Action Handler for InstallWinPermFile action (sample).
**	Arguments:
**		pcd   : Ptr to class data.
**		pod   : Ptr to object data.
**		camf  :	Custom action member function type.
**		pcamfd: Ptr to custom action member function data
**		        (variable size, depends on camf type).
**	Returns:
**		Appropriate RC (return code) values, including the new
**		rcDoDefault (i.e. okay, but do the default action for this
**		function too).
**	Notes:
**
***************************************************************************/
RC FAR PASCAL InstallWinPermFile ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData )
{
	RC rc = rcDoDefault;

	switch (camf)
		{
		case camfInitializeObject:
			rc = RcInitializeObjectIWPF(pod, (PCAMFDInitializeObject)pcamfd,
					szData);
			break;

		case camfDtor:
			rc = RcDtorIWPF((PCAMFDDtor)pcamfd);
			break;

		case camfCheckObjectIBSE:
			rc = RcCheckObjectIBSE_IWPF(pcd, pod,
					(PCAMFDCheckObjectIBSE)pcamfd);
			break;

		case camfCalcDstDir:
			rc = RcCalcDstDirIWPF(pcd, pod, (PCAMFDCalcDstDir)pcamfd);
			break;

		case camfGetCost:
			rc = RcGetCostIWPF(pcd, pod, (PCAMFDGetCost)pcamfd);
			break;

		case camfGetOODSCostNum:
			rc = RcGetOODSCostNumIWPF(pod, (PCAMFDGetOODSCostNum)pcamfd);
			break;

		case camfFinalizeObject:
			rc = RcFinalizeObjectIWPF(pcd, pod, (PCAMFDFinalizeObject)pcamfd);
			break;

		case camfAddToCopyList:
			rc = RcAddToCopyListIWPF(pcd, pod, (PCAMFDAddToCopyList)pcamfd);
			break;

		case camfAnswerDependRemove:
			rc = RcAnswerDependRemoveIWPF((PCAMFDAnswerDependRemove)pcamfd);
			break;

		case camfAnswerCompanion:
			rc = RcAnswerCompanionIWPF(pod, (PCAMFDAnswerCompanion)pcamfd);
			break;

		default:
			break;
		}

	return (rc);
}


/*
***************************************************************************/
RC PUBLIC RcInitializeObjectIWPF ( POD pod, PCAMFDInitializeObject pcamfd,
				SZ szData )
{
	PIWPFD piwpfd;
	RC   rc;

	if (pcamfd->lpvPriv == lpvoidNull)
		{
		while ((piwpfd = (PIWPFD)PbAlloc((CB)sizeof(IWPFD))) == lpvoidNull)
			{
			if (!HandleOOM())
				return (rcFail);
			}
		piwpfd->szSect = szNull;
		piwpfd->szKey  = szNull;
		piwpfd->wnc    = wncNoFile;
		piwpfd->scb    = 0L;
		rc = RcParseInfSectKey(pod->or, szData, &(piwpfd->szSect),
				&(piwpfd->szKey));
		if (rc != rcOk)
			{
			FFreePiwpfd(piwpfd);
			return (rc);
			}
		pcamfd->lpvPriv = (LPVOID)piwpfd;
		}

	return (rcDoDefault);
}


/*
***************************************************************************/
RC PUBLIC RcDtorIWPF ( PCAMFDDtor pcamfd )
{
	if (pcamfd->lpvPriv != lpvoidNull)
		{
		FFreePiwpfd((PIWPFD)pcamfd->lpvPriv);
		pcamfd->lpvPriv = lpvoidNull;
		}

	return (rcDoDefault);
}


/*
***************************************************************************/
RC PUBLIC RcCheckObjectIBSE_IWPF ( PCD pcd, POD pod,
				PCAMFDCheckObjectIBSE pcamfd )
{
	YNME ynme;
	WNC  wnc;

	Assert(pod->ois != oisNotInTree);

	if (pod->fVisitedIBSE)
		{
		pcamfd->ynmeRes = pod->ynmeIBSEState;
		return (rcOk);
		}
	
	Assert(pcd->simSetupMode != simAdminM);
	SetVisitedIBSE(pcd, pod->or, fTrue);

	if (pod->iby == ibyUs)
		{
		SetIBSEState(pcd, pod->or, ynmeNo);
		pcamfd->ynmeRes = ynmeNo;
		return (rcOk);
		}

	FCalcDstDir(pcd, pod->or, "C:\\");
	Assert(pcamfd->lpvPriv != lpvoidNull);
	wnc = ((PIWPFD)(pcamfd->lpvPriv))->wnc;

	ynme = (wnc == wncNoFile || wnc == wncLoVer)
			? ynmeNo : ynmeYes;

	SetIBSEState(pcd, pod->or, ynme);
	pcamfd->ynmeRes = ynme;
	return (rcOk);
}

			
/*
***************************************************************************/
RC PUBLIC RcCalcDstDirIWPF ( PCD pcd, POD pod, PCAMFDCalcDstDir pcamfd )
{
	PIWPFD piwpfd = (PIWPFD)(pcamfd->lpvPriv);
	CHAR   rgch[_MAX_PATH];

	Assert(piwpfd != lpvoidNull);
	Assert(FValidDir(pcamfd->szParentDstDir));

	if (pcd->simSetupMode == simAdminM)
		{
		piwpfd->wnc = wncNoFile;
		pcamfd->fRes = fTrue;
		return (rcDoDefault);
		}
	else if (pcd->simSetupMode == simMaintM && pod->iby == ibyUs)
		{
		piwpfd->wnc = wncEqVer;
		pcamfd->fRes = fTrue;

		/*
		**	We used to store the original DestDir in the pod->rgchTblDstDir
		**	(now called pod->szTblDstDir after memory optimization)
		**	column by calling SetDstDirUserChoice(pcd, pod->or, fTrue)
		**	during DoNonVisualMods, but that is no longer necessary since
		**	ACME now stores that original DestDir in the InstallDir
		**	column of the maintenance table file (after InstallData), and
		**	returning rcDoDefault will handle this for us.
		*/

		return (rcDoDefault);
		}

	piwpfd->wnc = WncWinPermFileNeedsCopy(piwpfd->szSect, piwpfd->szKey,
					pcd->rgchStfWinDir);

	/* Convert const sz into non-const local.
	*/
	Assert(CbStrLen(pcd->rgchStfWinDir) < sizeof(rgch));
	SzStrCopy(rgch, pcd->rgchStfWinDir);

	/*
	 *	Because we are copying the string into a local buffer, we need
	 *	to set fDup to fTrue (the 4th argument).
	 */
	pcamfd->fRes = FSetDstDir(pcd, pod->or, rgch, fTrue);
	return (rcOk);
}


/*
***************************************************************************/
RC PUBLIC RcGetCostIWPF ( PCD pcd, POD pod, PCAMFDGetCost pcamfd )
{
	PIWPFD piwpfd = (PIWPFD)(pcamfd->lpvPriv);

	Assert(piwpfd != lpvoidNull);
	Assert(pcamfd->pscb != lpvoidNull);

	if (piwpfd->scb == 0L)
		{
		piwpfd->scb = (SCB)GetSectionKeySize(piwpfd->szSect, piwpfd->szKey);
		if (piwpfd->scb == 0L)
			piwpfd->scb = 1;
		}

	if (pod->szDstDir != szNull)
		pcamfd->szDestDir = pod->szDstDir;
	else
		pcamfd->szDestDir = (SZ)pcd->rgchStfWinDir;

	FAddToCostVector(pcamfd->pscb, piwpfd->scb, pcamfd->szDestDir, fTrue);
	return (rcOk);
}


/*
***************************************************************************/
RC PUBLIC RcCalcDstDirIS16PF ( PCD pcd, POD pod, PCAMFDCalcDstDir pcamfd )
{
	PIWPFD piwpfd = (PIWPFD)(pcamfd->lpvPriv);
	CHAR   rgch[_MAX_PATH];

	Assert(piwpfd != lpvoidNull);
	Assert(FValidDir(pcamfd->szParentDstDir));

	if (pcd->simSetupMode == simAdminM)
		{
		piwpfd->wnc = wncNoFile;
		pcamfd->fRes = fTrue;
		return (rcDoDefault);
		}
	else if (pcd->simSetupMode == simMaintM && pod->iby == ibyUs)
		{
		piwpfd->wnc = wncEqVer;
		pcamfd->fRes = fTrue;

		/*
		**	We used to store the original DestDir in the pod->rgchTblDstDir
		**	(now called pod->szTblDstDir after memory optimization)
		**	column by calling SetDstDirUserChoice(pcd, pod->or, fTrue)
		**	during DoNonVisualMods, but that is no longer necessary since
		**	ACME now stores that original DestDir in the InstallDir
		**	column of the maintenance table file (after InstallData), and
		**	returning rcDoDefault will handle this for us.
		*/

		return (rcDoDefault);
		}

	piwpfd->wnc = WncWinPermFileNeedsCopy(piwpfd->szSect, piwpfd->szKey,
					pcd->rgchStfSys16Dir);

	/* Convert const sz into non-const local.
	*/
	Assert(CbStrLen(pcd->rgchStfSys16Dir) < sizeof(rgch));
	SzStrCopy(rgch, pcd->rgchStfSys16Dir);

	/*
	 *	Because we are copying the string into a local buffer, we need
	 *	to set fDup to fTrue (the 4th argument).
	 */

	pcamfd->fRes = FSetDstDir(pcd, pod->or, rgch, fTrue);
	return (rcOk);
}


/*
****************************************************************************/
RC PUBLIC RcGetOODSCostNumIWPF ( POD pod, PCAMFDGetOODSCostNum pcamfd )
{
	PIWPFD piwpfd = (PIWPFD)(pcamfd->lpvPriv);
	Assert(piwpfd != lpvoidNull);

	Assert(pcamfd->chDrv >= 'A');
	Assert(pcamfd->chDrv <= 'Z');
	Assert(pcamfd->pscb  != lpvoidNull);

	if (pod->ois == oisToBeInstalled)
		{
		Assert(piwpfd->scb != 0L);
		Assert(FValidDir(pod->szDstDir));

		if (*(pod->szDstDir) == pcamfd->chDrv)
			FAddToCostTotal(pcamfd->pscb, piwpfd->scb, pcamfd->chDrv, fTrue);
		}
	return (rcOk);
}


/*
***************************************************************************/
RC PUBLIC RcFinalizeObjectIWPF ( PCD pcd, POD pod, PCAMFDFinalizeObject pcamfd )
{

	OIS ois = pod->ois;
	WNC wnc;


	if (ois == oisNotInTree || ois == oisNotYetInstalled)
		return (rcOk);

	Assert(pcamfd->lpvPriv != lpvoidNull);
	wnc = ((PIWPFD)(pcamfd->lpvPriv))->wnc;

	switch (ois)
		{
		case oisToBeInstalled:
			if (wnc == wncEqVer || wnc == wncHiVer)
				FSetOisState(pcd, pod->or, oisInstalledBySomebodyElse);
			break;

		case oisToBeRemoved:
			/* We don't remove these, so change ois value.
			*/
			FSetOisState(pcd, pod->or, oisInstalledBySomebodyElse);
			break;

		case oisInstalledByUs:
			if (wnc == wncLoVer || wnc == wncNoFile)
				FSetOisState(pcd, pod->or, oisToBeInstalled);
			break;

		case oisInstalledBySomebodyElse:
			break;
		case oisNotInTree:			/* handled above */
		case oisNotYetInstalled:
			Assert(fFalse);
		case oisUnknown:
		default:
			Assert(fFalse);
			break;
		}

	return (rcOk);
}


/*
****************************************************************************/
RC PUBLIC RcAddToCopyListIWPF ( PCD pcd, POD pod, PCAMFDAddToCopyList pcamfd )
{
	PIWPFD piwpfd;
	SZ	   szInfSect;
	SZ     szInfKey;
	CHAR   rgch[_MAX_PATH];


	switch (pod->ois)
		{
		case oisToBeInstalled:
			piwpfd    = (PIWPFD)(pcamfd->lpvPriv);
			szInfSect = piwpfd->szSect;
			szInfKey  = piwpfd->szKey;

			Assert(piwpfd    != lpvoidNull);
			Assert(szInfSect != szNull);
			Assert(szInfKey  != szNull);
			Assert(FValidInfSection(szInfSect));
			Assert(FValidInfKey(szInfKey));

			Assert(FValidDir(pcd->rgchStfSrcDir));
			Assert(FValidDir(pcd->rgchStfWinDir));
			/* 	Because this is also called by InstallSys16PermFile
			*/
			Assert(FValidDir(pcd->rgchStfSys16Dir));
			Assert(pod->szDstDir != szNull);
			Assert(FValidDir(pod->szDstDir));

			/* NOTE: Copy SrcDir into temp buffer because
			*	AddSectionKeyFileToCopyList cannot take a
			*	const string.
			*/
			Assert(CbStrLen(pcd->rgchStfSrcDir) < sizeof(rgch));
			SzStrCopy(rgch, pcd->rgchStfSrcDir);

			AddSectionKeyFileToCopyList(szInfSect, szInfKey, 
					rgch, pod->szDstDir);
			break;

		case oisToBeRemoved:
			Assert(fFalse);		/* Don't remove */
			break;

		case oisInstalledByUs:
		case oisInstalledBySomebodyElse:
		case oisNotYetInstalled:
		case oisNotInTree:
			break;
		case oisUnknown:
			Assert(fFalse);
		default:
			Assert(fFalse);
			return (rcFail);
		}

	return (rcOk);
}


/*
************************************************************************/
RC PUBLIC RcAnswerDependRemoveIWPF ( PCAMFDAnswerDependRemove pcamfd )
{
	pcamfd->fRes = fFalse;		/* Don't remove Depend 'then' items */
	return (rcOk);
}


/*
************************************************************************/
RC PUBLIC RcAnswerCompanionIWPF ( POD pod, PCAMFDAnswerCompanion pcamfd )
{
	Assert(pod->ois != oisUnknown);

	pcamfd->fRes = (pod->ois == oisToBeInstalled);
	return (rcOk);
}


/*
****************************************************************************/
BOOL PUBLIC FFreePiwpfd ( PIWPFD piwpfd )
{
	if (piwpfd != lpvoidNull)
		{
		if (piwpfd->szSect != szNull)
			FFreeSz(piwpfd->szSect);
		if (piwpfd->szKey != szNull)
			FFreeSz(piwpfd->szKey);
		FFree((PB)piwpfd, (CB)sizeof(IWPFD));
		}
	return (fTrue);
}


/*
****************************************************************************/
WNC PUBLIC WncWinPermFileNeedsCopy ( SZ szInfSect, SZ szInfKey,
							SZC szcWinDir )
{
	CHAR rgchFile[_MAX_PATH];

	if (!FGetKeyFullPath(szInfSect, szInfKey, szcWinDir, rgchFile,
						 sizeof(rgchFile)))
		{
		/* REVIEW: Correct for FGetKeyFullPath/SzGetKeyFilenameRoot failure?
		*/
		return (wncNoFile);
		}

	return (WncInfFileNeedsCopy(szInfSect, szInfKey, rgchFile));
}


/*
**	Purpose:
**		Construct a full path for a file from an inf file.
**	Arguments:
**		szInfSect:  .inf section name
**		szInfKey:   .inf key value
**		szFullPath: buffer to receive the result
**		szDir:      the directory for the full path.
**		cbBufMax:   size of buffer in bytes
**	Returns:
**		fTrue if successful, fFalse otherwise
**	Notes:
**
****************************************************************************/
BOOL PUBLIC FGetKeyFullPath ( SZ szInfSect, SZ szInfKey, SZC szcDir,
							SZ szFullPath, CB cbBufMax )
{
	CHAR rgchFile[_MAX_PATH];

	Assert(FValidInfSection(szInfSect));
	Assert(FValidInfKey(szInfKey));
	Assert(FValidDir(szcDir));
	Assert(szFullPath != szNull);

	Unused(cbBufMax);		/* Used only in DEBUG */

	if (SzGetKeyFilenameRoot(szInfSect, szInfKey, rgchFile,
			(CB)sizeof(rgchFile)) == szNull)
		{
		return (fFalse);
		}

	Assert(CbStrLen(szcDir) + CbStrLen(rgchFile) < cbBufMax);
	SzStrCopy(szFullPath, szcDir);
	SzStrCat(szFullPath, rgchFile);

	return (fTrue);
}


/*
**	Purpose:
**		Get the root src filename from an .inf entry.
**	Arguments:
**		szSect: .inf section name
**		szKey:  .inf key value
**		szBuf:  buffer to receive the result
**		cbBuf:  size of buffer in bytes
**	Returns:
**		Returns szNull if it fails, otherwise returns szBuf.
**	Notes:
**		Assumes cbBuf > largest possible filename.
**
****************************************************************************/
SZ PUBLIC SzGetKeyFilenameRoot ( SZ szSect, SZ szKey, SZ szBuf, CB cbBuf )
{
	CHAR rgch[cchSzMax];
	CB   cb;
	SZ   sz;

	cb = GetSectionKeyFilename(szSect, szKey, rgch, sizeof rgch);
	Assert(cb < (sizeof rgch) - 1);
	sz = SzStripPath(rgch);
	Assert(CbStrLen(sz) < cbBuf);
	if (!FValidFileName(sz))
		{
		DebugMsgBox(sz, "Invalid filename in INF file");
		return (szNull);
		}
	CbStrCopyToBuffer((PB)szBuf, cbBuf, sz);
	return (szBuf);
}


/*
**	Purpose:
**		Locates the filename in a given full or partial path string.
**	Arguments:
**		szPath: Fully, partially, or non-qualified filename.
**	Returns:
**		Pointer to filename in given string.
**	Notes:
**		Does not validate path syntax.
**
****************************************************************************/
SZ PUBLIC SzStripPath ( SZ szPath )
{
	SZ sz = (SZ)(szPath + CbStrLen(szPath));

	while ((sz = AnsiPrev(szPath, sz)) != szPath && *sz != '\\')
		;
	if (*sz == '\\')
		sz = AnsiNext(sz);
	return (sz);
}


/*
**	Purpose:
**		Compare an .inf file entry to a potentially existing file.
**	Arguments:
**		szInfSect: .inf section name
**		szInfKey:  .inf key value
**		szPath:    full path filename
**	Returns:
**		wncHiVer,  if inf ver <  szPath ver
**		wncEqVer,  if inf ver == szPath ver
**		wncLoVer,  if inf ver >  szPath ver
**		wncNoFile, if szPath file does not exists
**	Notes:
**
****************************************************************************/
WNC PUBLIC WncInfFileNeedsCopy ( SZ szInfSect, SZ szInfKey, SZ szPath )
{
	CHAR rgchInfVer[cchSzMax];
	CHAR rgchCurVer[cchSzMax];
	WNC  wnc;

	Assert(FValidInfSection(szInfSect));
	Assert(FValidInfKey(szInfKey));
	Assert(FValidPath(szPath));

	if (!DoesFileExist(szPath, femExists))
		return (wncNoFile);

	GetSectionKeyVersion(szInfSect, szInfKey, rgchInfVer, cchSzMax);
	GetVersionOfFile(szPath, rgchCurVer, cchSzMax);
	switch(WCompareVersion(rgchInfVer, rgchCurVer))
		{
		default:
			Assert(fFalse);
			/* fall through */
		case -1:
			wnc = wncHiVer;
			break;
		case 0:
			wnc = (FLanguageMismatchInf(szInfSect, szInfKey, szPath))
					? wncLoVer : wncEqVer;
			break;
		case 1:
			wnc = wncLoVer;
			break;
		}

	return (wnc);
}


/*
**	Purpose:
**		Compare two file versions.
**	Arguments:
**		szVer1:	Version string 1
**		szVer2: Version string 2
**	Returns:
**		1 if szVer1 > szVer2;  0 if equal;  -1 if szVer1 < szVer2
**	Notes:
**
****************************************************************************/
INT PUBLIC WCompareVersion ( SZ szVer1, SZ szVer2 )
{
	INT i;

	if (*szVer1 == '\0')
		if (*szVer2 == '\0')
			return (0);
		else
			return (-1);

	if (*szVer2 == '\0')
		return (1);

	for (i = 1; i <= 4; i++)
		{
		LONG piece1 = GetVersionNthField(szVer1, i);
		LONG piece2 = GetVersionNthField(szVer2, i);

		if (piece1 > piece2)
			return (1);
		if (piece1 < piece2)
			return (-1);
		}

	return (0);
}


/*
************************************************************************/
BOOL PUBLIC FAddToCostVector ( PSCB pscb, SCB scb, SZ szDir, BOOL fRoundUp )
{
	UINT iDrv;

	Assert(pscb != lpvoidNull);

	if (szDir == szNull)
		iDrv = 2;   /* "C:\" */
	else if (*szDir >= 'A' && *szDir <= 'Z')
		iDrv = *szDir - 'A';
	else if (*szDir >= 'a' && *szDir <= 'z')
		iDrv = *szDir - 'a';
	else
		iDrv = 2;   /* "C:\" */

	if (fRoundUp)
		{
		SCB scbCluster = (SCB)CbGetDrvCluster(iDrv);

		/* REVIEW Assert(FPowerOfTwo(scbCluster));  */
		scb = (scb + scbCluster - 1) & ~(scbCluster - 1);
		}

	*(pscb + iDrv)      += scb;
	*(pscb + iscbTotal) += scb;

	return (fTrue);
}


/*
************************************************************************/
BOOL PUBLIC FAddToCostTotal ( PSCB pscb, SCB scb, CHAR chDrv, BOOL fRoundUp )
{
	Assert(pscb != lpvoidNull);
	Assert(scb >= 0);
	/*
	**	note - some files to be copied ARE zero bytes (eg CUSTOM.DIC)
	**		but this will also be triggered by files being removed -
	**		solution for them is to put them in a separate INF section
	**		and call RemoveSection instead of CopySection.
	*/
	if (scb == 0)
		scb = 80;
	Assert(chDrv >= 'A');
	Assert(chDrv <= 'Z');

	if (fRoundUp)
		{
		SCB scbCluster = (SCB)CbGetDrvCluster(chDrv - 'A');

		/* REVIEW Assert(FPowerOfTwo(scbCluster));  */
		scb = (scb + scbCluster - 1) & ~(scbCluster - 1);
		}

	*(pscb) += scb;

	return (fTrue);
}


/*
************************************************************************/
CB PUBLIC CbGetDrvCluster ( UINT iDrv )
{
	static CB rgcbDrvCluster[26] =
			{ 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0 };

	Assert(iDrv < 26);

	if (rgcbDrvCluster[iDrv] == 0)
		{
		DWORD dwSectorsPerCluster, dwBytesPerSectors,
				dwFreeClusters, dwClusters;
		struct _diskfree_t df;
		SEM semSav = SetErrorMode(SEM_FAILCRITICALERRORS);

		/* Note that GetDiskFreeSpace return BOOL, unlike _dos_getdiskfree */
		if (GetDiskFreeSpace(DriveNumToRootPath(iDrv), &dwSectorsPerCluster,
							&dwBytesPerSectors, &dwFreeClusters, &dwClusters))
			{
			df.sectors_per_cluster = dwSectorsPerCluster;
			df.bytes_per_sector = dwBytesPerSectors;
			df.avail_clusters = dwFreeClusters;
			df.total_clusters = dwClusters;
			rgcbDrvCluster[iDrv] = df.bytes_per_sector
					* df.sectors_per_cluster;
			}
		else
			rgcbDrvCluster[iDrv] = 4096;
		SetErrorMode(semSav);
		}

	Assert(rgcbDrvCluster[iDrv] > 0);
	return (rgcbDrvCluster[iDrv]);
}
