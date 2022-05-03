/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
**
**	File:		MOVEFILE.C
**	Purpose:	Moves the file specified in the 3rd param of the object
**			data field to the specified destination directory.
**			Optionally you can specify a new name for the file.
**
**	STF Syntax:
**		ObjID	Batch	Title		Descr		Type		Data						Bmp Id	Vital	Shared	DirChng	DestDir	CheckDir	InstBy	InstData	InstDir
**		ID#		"Error Title"	"Error Text"		CustomAction	"EXPSETUP.DLL","Move File","%213,NewName.Ext"					%224
**
**
**	Author: ******
**	Written For:	Explorapedia Win32/Win16 versions of Nature and People
**
****************************************************************************/

#define MOVEFILE_C

#include <windows.h>
#include <stdlib.h>		/* _MAX_PATH */
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"


RC FAR PASCAL MoveFile ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );


#define SzDirFromPcamfd(pcamfd)	((SZ)(((PCAMFDDtor)(pcamfd))->lpvPriv))

#define RcCallBack(pcd, or, camf, pcamfd) \
			((*(pcd)->pfncacb)(or, camf, pcamfd))


//STATIC_FN BOOL PRIVATE FAppendSlashPath ( SZ sz );


/*************************************************************************


****************************************************************************/
RC FAR PASCAL MoveFile ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData )
{
	RC		rc = rcDoDefault;

	switch (camf)
		{
		case camfInitializeObject:
			break;

		case camfCalcDstDir:
			break;

		case camfDoNonVisualMods:
			{
			SZ	pName;

				// locate end of 1st param (Source Pathname)
			for(pName=szData ; *pName ; pName++)
			  if(',' == *pName)
			  	{
				*pName++ = '\0';
			    	break;
				}

			#ifdef _DEBUG
				{
				char Buf[128];
				wsprintf(Buf, "SrcPathname='%s' DestDir='%s' NewName='%s'",
					(LPSTR)szData, (LPSTR)pod->szInstallDir, (LPSTR)pName);
				DoMsgBox(Buf, "MoveFile - DoNonVisualMods", MB_OK);
				}
			#endif

			break;
			}

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

	if(rcFail == rc)
	  DoMsgBox(pod->szDescription, pod->szTitle, MB_OK | MB_ICONEXCLAMATION);

	return (rc);
}


/*
****************************************************************************/
/*
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
*/


