/***************************************************************************
**
**	File:			INIPATH.C (from \\conman\release\usrswap\customa\inipath)
**	Purpose:		GetPathFromIni Custom Action Handler for
**					Acme Setup.
**	Notes:
**		REVIEW: Replace STR_ #define's with your own strings.
**
****************************************************************************/

#define INIPATH_C

#include <windows.h>
#include <stdlib.h>		/* _MAX_PATH */
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"


RC FAR PASCAL GetPathFromIni ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData );


#define SzDirFromPcamfd(pcamfd)	((SZ)(((PCAMFDDtor)(pcamfd))->lpvPriv))

#define RcCallBack(pcd, or, camf, pcamfd) \
			((*(pcd)->pfncacb)(or, camf, pcamfd))


STATIC_FN RC   PRIVATE RcInitializeObject ( PCD pcd,
						PCAMFDInitializeObject pcamfd,
						SZ szData);

STATIC_FN RC   PRIVATE RcDtor ( PCAMFDDtor pcamfd );
STATIC_FN BOOL PRIVATE FAppendSlashPath ( SZ sz );


/*
***************************************************************************/
RC FAR PASCAL GetPathFromIni ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData )
{
	RC		rc = rcDoDefault;

	//Unused(szData);

	switch (camf)
		{
		case camfInitializeObject:
			rc = RcInitializeObject(pcd, (PCAMFDInitializeObject)pcamfd, szData);
			
			if(rcFail == rc)
			  DoMsgBox(pod->szDescription, pod->szTitle, MB_OK | MB_ICONEXCLAMATION);
			break;

		case camfCalcDstDir:
			if (pcd->simSetupMode != simMaintM
					&& pcd->simSetupMode != simAdminM)
				{
				CAMFDSetDstDir camfd;

				camfd.szDir = SzDirFromPcamfd(pcamfd);
				camfd.fDup  = fTrue;
				rc = RcCallBack(pcd, pod->or, camfSetDstDir, &camfd);
				Assert(rc == rcOk);
				((PCAMFDCalcDstDir)pcamfd)->fRes = camfd.fRes;
				}
			break;

		case camfDtor:
			if (((PCAMFDDtor)pcamfd)->lpvPriv != lpvoidNull)
				{
				FFreeSz((SZ)((PCAMFDDtor)pcamfd)->lpvPriv);
				((PCAMFDDtor)pcamfd)->lpvPriv = lpvoidNull;
				}
			break;

		default:
			break;
		}

	return (rc);
}


/*************************************************************************

  STF entry should look like this:

201		"Setup Error"	"Setup couldn't located a necessary Explora.ini entry"	CustomAction	"EXPSETUP.DLL","GetPathFromIni","Explora.ini,Paths,InstPath"					

where the 3rd param of the data item is INI filename, Section, Entry name

****************************************************************************/
STATIC_FN RC PRIVATE RcInitializeObject ( PCD pcd,
						PCAMFDInitializeObject pcamfd,
						SZ szData)
{
	RC	rc = rcDoDefault;

	if (pcamfd->lpvPriv == lpvoidNull
			&& pcd->simSetupMode != simMaintM
			&& pcd->simSetupMode != simAdminM)
		{
		CHAR	rgchPath[_MAX_PATH];
		CB		cb;
		SZ	pEntry, pSection;

			// locate end of 1st param (ini filename)
		for(pSection=szData ; *pSection != ',' ; pSection++)
		  if('\0' == *pSection)
		  	{
			#ifdef _DEBUG
			  MessageBox(NULL, szData, "IniPath - Missing 2nd param", MB_OK);
			#endif
		    	return(rcFail);
			}
		*pSection++ = '\0';
		
			// locate end of 2nd param (ini section name)
		for(pEntry = pSection ; *pEntry != ',' ; pEntry++)
		  if('\0' == *pEntry)
		  	{
			#ifdef _DEBUG
			  MessageBox(NULL, pSection, "IniPath - Missing 3rd param", MB_OK);
			#endif
		    	return(rcFail);
		    	}
		*pEntry++ = '\0';

		//szData points to start of 1st param (ini filename)
		//pSection points to start of 2nd param (ini section name)
		//pEntry points to start of 3rd param (ini entry name)

		cb = GetPrivateProfileString(pSection, pEntry, "", rgchPath, sizeof(rgchPath), szData);
		//cb = GetIniKeyString(szData, pSection, pEntry,
		//		rgchPath, sizeof(rgchPath));

		if (cb == 0 || !FValidDir(rgchPath))
			{
			rc = rcFail;
			}
		else
			{
			SZ	sz;

			CharUpper(rgchPath);
			FAppendSlashPath(rgchPath);
			while ( (sz = (SZ)PbAlloc(CbStrLen(rgchPath) + 1)) == szNull)
				{
				if (!FHandleOOM())
					{
					#ifdef _DEBUG
					  MessageBox(NULL, rgchPath, "IniPath - Out of Memory", MB_OK);
					#endif
					return (rcFail);
					}
				}
			SzStrCopy(sz, rgchPath);
			pcamfd->lpvPriv = (LPVOID)sz;

			#ifdef _DEBUG
			  MessageBox(NULL, sz, "IniPath - Exit", MB_OK);
			#endif
			}
		}

	return (rc);
}


/*
***************************************************************************/
STATIC_FN RC PRIVATE RcDtor ( PCAMFDDtor pcamfd )
{
	if (pcamfd->lpvPriv != lpvoidNull)
		{
		FFreeSz((SZ)pcamfd->lpvPriv);
		pcamfd->lpvPriv = lpvoidNull;
		}

	return (rcDoDefault);
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



