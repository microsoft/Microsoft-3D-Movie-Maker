/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
**
**	File:		INIPATH.C
**	Purpose:	GetPathFromIni Custom Action Handler for Acme Setup.
**
**	STF Syntax:
**		ObjID	Batch	Title		Descr		Type		Data						Bmp Id	Vital	Shared	DirChng	DestDir	CheckDir	InstBy	InstData	InstDir
**		ID#		"Error Title"	"Error Text"	CustomAction	"filename.DLL","GetPathFromIni","filename.ini,Section,Entry"					
**
**	Author: ******
**	Written For:	Explorapedia Win32/Win16 versions of Nature and People
**	History:	Started with \\conman\release\usrswap\customa\inipath
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


STATIC_FN BOOL PRIVATE FAppendSlashPath ( SZ sz );


/*************************************************************************

  Custom Action entry point

****************************************************************************/
RC FAR PASCAL GetPathFromIni ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData )
{
	RC		rc = rcDoDefault;

	switch (camf)
		{
		case camfInitializeObject:
			if ( ((PCAMFDInitializeObject)pcamfd)->lpvPriv == lpvoidNull
					&& pcd->simSetupMode != simMaintM
					&& pcd->simSetupMode != simAdminM)
				{
				CHAR	rgchPath[_MAX_PATH];
				SZ	pEntry, pSection;

					// locate end of 1st param (ini filename)
				for(pSection=szData ; *pSection != ',' ; pSection++)
				  if('\0' == *pSection)
				  	{
					#ifdef _DEBUG
					  MessageBox(NULL, szData, "IniPath - Missing 2nd param", MB_OK);
					#endif
				    	rc = rcFail;
				    	break;
					}
				*pSection++ = '\0';
		
					// locate end of 2nd param (ini section name)
				for(pEntry = pSection ; *pEntry != ',' ; pEntry++)
				  if('\0' == *pEntry)
				  	{
					#ifdef _DEBUG
					  MessageBox(NULL, pSection, "IniPath - Missing 3rd param", MB_OK);
					#endif
				    	rc = rcFail;
				    	break;
				    	}
				*pEntry++ = '\0';

				//szData points to start of 1st param (ini filename)
				//pSection points to start of 2nd param (ini section name)
				//pEntry points to start of 3rd param (ini entry name)

				//cb = GetIniKeyString(szData, pSection, pEntry,
				//		rgchPath, sizeof(rgchPath));
				if(GetPrivateProfileString(pSection, pEntry, "", rgchPath, sizeof(rgchPath), szData) == 0)
					{
					#ifdef _DEBUG
					  MessageBox(pEntry, rgchPath, "IniPath - Entry not found", MB_OK);
					#endif
					SzStrCopy(rgchPath, "\\");
					}
				
				else if(!FValidDir(rgchPath))
					{
					#ifdef _DEBUG
					  MessageBox(NULL, rgchPath, "IniPath - Found Invalid path", MB_OK);
					#endif
					SzStrCopy(rgchPath, "\\");
					}

				else FAppendSlashPath(rgchPath);

				while ( (pEntry = (SZ)PbAlloc(CbStrLen(rgchPath) + 1)) == szNull)
					{
					if (!FHandleOOM())
						{
						#ifdef _DEBUG
						  MessageBox(NULL, rgchPath, "IniPath - Out of Memory", MB_OK);
						#endif
					    	return(rcFail);
						}
					}
				SzStrCopy(pEntry, rgchPath);

				((PCAMFDInitializeObject)pcamfd)->lpvPriv = (LPVOID)pEntry;

				#ifdef _DEBUG
				  MessageBox(NULL, pEntry, "IniPath - Exit", MB_OK);
				#endif
				
				rc = rcOk;
				}
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

				rc = rcOk;
				}
			break;

		case camfDtor:
			if (((PCAMFDDtor)pcamfd)->lpvPriv != lpvoidNull)
				{
				FFreeSz((SZ)((PCAMFDDtor)pcamfd)->lpvPriv);
				((PCAMFDDtor)pcamfd)->lpvPriv = lpvoidNull;

				rc = rcOk;
				}
			break;

		default:
			break;
		}

	if(rcFail == rc)
	  DoMsgBox(pod->szDescription, pod->szTitle, MB_OK | MB_ICONEXCLAMATION);

	return (rc);
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
