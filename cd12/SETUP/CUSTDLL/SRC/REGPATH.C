/***************************************************************************
**
**	File:			REGPATH.C
**	Purpose:		Sample Detection Custom Action Handler
**					routines for Acme Setup.
**	Notes:
**
****************************************************************************/

#define REGPATH_C

#include "stdinc.h"
#include <stdlib.h>		/* _MAX_PATH */
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"
#include "parseca.h"


#ifdef DEBUG
/* Strings used in debug messages only.
*/
CSZC cszcTitle   = "Setup Message";
CSZC cszcBadPath =
			"ObjID %u: GetPathFromReg: The following invalid destination path "
			"was found in the registry:\n\n%s\n\nSetup will use the object's "
			"default path instead.";
CSZC cszcBadPathUNC =
			"ObjID %u: GetPathFromReg: The following path was found in the "
			"the registry, but can't be used because it is UNC format:"
			"\n\n%s\n\nSetup will use the object's default path instead.";
CHAR rgchDebMsg[cchSzMax];
#endif


typedef struct _RPD		/* Reg Path Data */
	{
	SZ   szDir;
	}  RPD;
typedef RPD FAR * PRPD;


RC FAR PASCAL GetPathFromReg ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData );


#define SzDirFromPcamfd(pcamfd) \
			((((PCAMFDDtor)pcamfd)->lpvPriv == lpvoidNull) \
				? szNull : ((PRPD)(((PCAMFDDtor)pcamfd)->lpvPriv))->szDir)

#define RcCallBack(pcd, or, camf, pcamfd) \
			((*(pcd)->pfncacb)(or, camf, pcamfd))

STATIC_FN BOOL PRIVATE FFreePrpd ( PRPD prpd );
STATIC_FN BOOL PRIVATE FAppendSlashPath ( SZ sz );
STATIC_FN RC   PRIVATE RcParseGetRegPath ( OR orCur, SZ szData, PHKEY phkeyRoot,
								PSZ pszKey, PSZ pszValueName );
STATIC_FN HKEY PRIVATE HkeyFromSzRootKey ( SZ szRootKey );
STATIC_FN BOOL PRIVATE FValidRegKey ( SZ sz );


RC FAR PASCAL GetPathFromReg ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );


STATIC_FN RC PRIVATE RcInitializeTrigger ( PCD pcd, POD pod, SZ szData, PCAMFDInitializeTrigger pcamfd );
STATIC_FN RC PRIVATE RcSetDstDirOfTrigger ( PCD pcd, POD pod, PCAMFDSetDstDirOfTrigger pcamfd );
STATIC_FN RC PRIVATE RcSetModeOfTrigger ( PCD pcd, POD pod, PCAMFDSetModeOfTrigger pcamfd );
STATIC_FN RC PRIVATE RcAnswerDependClause ( PCD pcd, POD pod, SZ szData, PCAMFDAnswerDependClause pcamfd );
STATIC_FN RC PRIVATE RcDtor ( PCAMFDDtor pcamfd );


/*
**	Purpose:
**		Custom Action Handler to detect and return a path in a registry entry.
**		Used as a "trigger" for a Depend object.
**
**	Arguments:
**		pcd   : Ptr to class data.
**		pod   : Ptr to object data.
**		camf  :	Custom action member function type.
**		pcamfd: Ptr to custom action member function data
**		        (variable size, depends on camf type).
**	Returns:
**		Appropriate RC (return code) value.
**
***************************************************************************/
RC FAR PASCAL GetPathFromReg ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData )
{
	RC	rc = rcDoDefault;

	Unused(szData);

	switch (camf)
		{
		case camfInitializeTrigger:
			rc = RcInitializeTrigger(pcd, pod, szData, (PCAMFDInitializeTrigger)pcamfd);
			break;

		case camfSetDstDirOfTrigger:
			rc = RcSetDstDirOfTrigger(pcd, pod,
						(PCAMFDSetDstDirOfTrigger)pcamfd);
			break;

		case camfSetModeOfTrigger:
			rc = RcSetModeOfTrigger(pcd, pod,
						(PCAMFDSetModeOfTrigger)pcamfd);
			break;

		case camfAnswerDependClause:
			rc = RcAnswerDependClause(pcd, pod, szData,
						(PCAMFDAnswerDependClause)pcamfd);
			break;

		case camfDtor:
			rc = RcDtor((PCAMFDDtor)pcamfd);
			break;

		default:
			break;
		}

	return (rc);
}


/*
****************************************************************************/
STATIC_FN RC PRIVATE RcInitializeTrigger ( PCD pcd, POD pod, SZ szData, PCAMFDInitializeTrigger pcamfd)
{
	CAMFDSetIBSEState		camfdIBSE;
	CAMFDSetVisitedIBSE		camfdVis;
	CAMFDInitializeObject	camfdInit;
	HKEY  hRootKey    = (HKEY)NULL;
	SZ    szKey       = szNull;
	SZ    szValueName = szNull;
	CHAR  rgchPath[_MAX_PATH] = "";
	DWORD dwType;
	PRPD  prpd;
	RC	  rc = rcDoDefault;
	
	camfdIBSE.ynme = ynmeNo;
	rc = (*pcd->pfncacb)(pod->or, camfSetIBSEState, &camfdIBSE);
	if (rc != rcOk)
		return (rc);

	camfdVis.f = fTrue;
	rc = (*pcd->pfncacb)(pod->or, camfSetVisitedIBSE, &camfdVis);
	if (rc != rcOk)
		return (rc);

	if (pcamfd->lpvPriv != lpvoidNull)
		{
		return (rc);		/* Already done, or not needed. */
		}
	
	/* Alloc our private data structure.
	*/
	while ((prpd = (PRPD)PbAlloc((CB)sizeof(RPD))) == lpvoidNull)
		{
		if (!HandleOOM())
			{
			rc = rcFail;
			goto LExitInit;
			}
		}
	prpd->szDir = szNull;
	pcamfd->lpvPriv = (LPVOID)prpd;

	/* Parse the data string.
	*/
	rc = RcParseGetRegPath(pod->or, szData, &hRootKey, &szKey,
							&szValueName);
	if (rc != rcOk)
		goto LExitInit;
	rc = rcDoDefault;	/* Reset */

	if (GetRegKeyValue32(hRootKey, szKey, szValueName, &dwType,
							(PBYTE)rgchPath, sizeof rgchPath) <= 1)
		{
		goto LExitInit;		/* Not found or empty string. */
		}

	/* Only save if valid dir (syntax) is obtained.
	*/
	if (FValidDstDir(rgchPath))
		{
		CharUpperBuff(rgchPath, 1);
		FAppendSlashPath(rgchPath);
		while ((prpd->szDir = (SZ)PbAlloc(CbStrLen(rgchPath) + 1))
				== szNull)
			{
			if (!FHandleOOM())
				{
				rc = rcFail;
				goto LExitInit;
				}
			}
		SzStrCopy(prpd->szDir, rgchPath);
		}
#ifdef DEBUG
	else if (FValidDir(rgchPath))	/* Check for UNC */
		{
		wsprintf(rgchDebMsg, cszcBadPathUNC, pod->or, rgchPath);
		DoMsgBox(rgchDebMsg, cszcTitle, MB_OK);
		}
	else
		{
		wsprintf(rgchDebMsg, cszcBadPath, pod->or, rgchPath);
		DoMsgBox(rgchDebMsg, cszcTitle, MB_OK);
		}
#endif /* DEBUG */


LExitInit:
	if (szKey != szNull)
		FFreeSz(szKey);
	if (szValueName != szNull)
		FFreeSz(szValueName);

	return ((*pcd->pfncacb)(pod->or, camfInitializeObject, &camfdInit));
}


/*
****************************************************************************/
STATIC_FN RC PRIVATE RcSetDstDirOfTrigger ( PCD pcd, POD pod,
						PCAMFDSetDstDirOfTrigger pcamfd )
{
	CAMFDSetDstDir camfd;
	RC rc;

	Assert(pcamfd->szParentDstDir != szNull);
	Assert(FValidDir(pcamfd->szParentDstDir));

	/* If we got a path on init, use it here.
	*/
	if (SzDirFromPcamfd(pcamfd) != szNull)
		{
		camfd.szDir = SzDirFromPcamfd(pcamfd);
		camfd.fDup  = fTrue;
		rc = RcCallBack(pcd, pod->or, camfSetDstDir, &camfd);
		Assert(rc == rcOk);
		}
	return (rcOk);
}


/*
****************************************************************************/
STATIC_FN RC PRIVATE RcSetModeOfTrigger ( PCD pcd, POD pod,
						PCAMFDSetModeOfTrigger pcamfd )
{
	CAMFDSetModeOfObject	camfdMode;

	camfdMode.sma = pcamfd->sma;
	return ((*pcd->pfncacb)(pod->or, camfSetModeOfObject, &camfdMode));
}


/*
**	Checks for Windows 95 (wvtChicago) and Windows NT (wvtWinNT).
**
**		NOTE: Acme95 does not support vtWin30, wvtWin31, or wvtWfW31.
**
****************************************************************************/
STATIC_FN RC PRIVATE RcAnswerDependClause ( PCD pcd, POD pod, SZ szData,
						PCAMFDAnswerDependClause pcamfd )
{
	Unused(pod);	/* Used in debug only */
	pcamfd->fRes = (SzDirFromPcamfd(pcamfd) != szNull);
	return (rcOk);
}

/*
***************************************************************************/
STATIC_FN RC PRIVATE RcDtor ( PCAMFDDtor pcamfd )
{
	if (pcamfd->lpvPriv != lpvoidNull)
		{
		FFreePrpd((PRPD)pcamfd->lpvPriv);
		pcamfd->lpvPriv = lpvoidNull;
		}

	return (rcDoDefault);
}



/*
****************************************************************************/
STATIC_FN BOOL PRIVATE FFreePrpd ( PRPD prpd )
{
	if (prpd != lpvoidNull)
		{
		if (prpd->szDir != szNull)
			FFreeSz(prpd->szDir);
		FFree((PB)prpd, (CB)sizeof(RPD));
		}
	return (fTrue);
}

/*
****************************************************************************/
STATIC_FN BOOL PRIVATE FAppendSlashPath ( SZ sz )
{
	Assert(sz != szNull);

	if (*SzLastChar(sz) != chDirSep)
		{
		Assert(CbStrLen(sz) + CbStrLen(szDirSep) < _MAX_PATH);
		SzStrCat(sz, szDirSep);
		}

	return (fTrue);
}


/*
****************************************************************************/
STATIC_FN RC PRIVATE RcParseGetRegPath ( OR orCur, SZ szData, PHKEY phkeyRoot,
							PSZ pszKey, PSZ pszValueName )
{
	CHAR rgch[cchSzMax];

	Assert(szData       != szNull);
	Assert(phkeyRoot    != (PHKEY)NULL);
	Assert(pszKey       != pszNull);
	Assert(pszValueName != pszNull);

	if (*phkeyRoot != (HKEY)NULL)
		return (rcOk);		/* Already parsed and alloc'ed */

	szData = SzGetTableField(szData, rgch, sizeof rgch, chComma);
	if (szData == szNull)
		{
		return (rcFail);
		}
	*phkeyRoot = HkeyFromSzRootKey(rgch);
	if (*phkeyRoot == NULL)
		{
		return (rcFail);
		}

	if (!FReadDataFieldString(&szData, pszKey))
		return (rcFail);
	Assert(*pszKey != szNull);
	if (!FValidRegKey(*pszKey))
		{
		return (rcFail);
		}

	/* KLUDGE: The read string routine can't tell the difference between
	*	an empty last field and a missing last field, so we need to look
	*	back and check szData for a comma.
	*/
	if (szData[-1] == ',' && FReadDataFieldString(&szData, pszValueName))
		{
		Assert(*pszValueName != szNull);

		if (**pszValueName != chEos && !FValidRegKey(*pszValueName))
			{
			return (rcFail);
			}
		}
	else
		{
		/* No value name field, so just set pointer to szNull.
		*/
		*pszValueName = szNull;
		}

	if (*szData != chEos)
		{
		return (rcFail);
		}

	return (rcOk);
}


/*
**	Purpose:
**		Converts a root key name string to an HKEY.
**	Arguments:
**		szRootKey: the root key name.
**	Returns:
**		The corresponding HKEY, or NULL if it doesn't exist.  If szRootKey
**		is NULL, HKEY_CLASSES_ROOT is returned by default.
**	Notes:
**
****************************************************************************/
STATIC_FN HKEY PRIVATE HkeyFromSzRootKey ( SZ szRootKey )
{
	if (szRootKey == szNull || *szRootKey == chEos)
		{
		return (HKEY_CLASSES_ROOT);
		}
	else if (lstrcmpi(szRootKey, "HKEY_CLASSES_ROOT") == 0
			|| lstrcmpi(szRootKey, "CLASSES") == 0)
		{
		return (HKEY_CLASSES_ROOT);
		}
	else if (lstrcmpi(szRootKey, "HKEY_CURRENT_USER") == 0
			|| lstrcmpi(szRootKey, "CURRENT") == 0)
		{
		return (HKEY_CURRENT_USER);
		}
	else if (lstrcmpi(szRootKey, "HKEY_LOCAL_MACHINE") == 0
			|| lstrcmpi(szRootKey, "LOCAL") == 0)
		{
		return (HKEY_LOCAL_MACHINE);
		}
	else if (lstrcmpi(szRootKey, "HKEY_USERS") == 0
			|| lstrcmpi(szRootKey, "USERS") == 0)
		{
		return (HKEY_USERS);
		}

	return (NULL);
}


/*
****************************************************************************/
STATIC_FN BOOL PRIVATE FValidRegKey ( SZ sz )
{
	UINT cch = 0;

	if (sz == szNull || *sz == chEos)
		return (fFalse);

	while (*sz != chEos)
		{
		if (*sz == chDirSep)
			{
			if (cch == 0)
				return (fFalse);
			cch = 0;
			}
		else if (++cch > 63)
			return (fFalse);

		sz = SzNextChar(sz);
		}

	return (cch > 0 && cch <= 63);
}



