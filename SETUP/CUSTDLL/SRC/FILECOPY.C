/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
**
**	File:		FILECOPY.C
**	Purpose:	Copies the file specified by the object number in the
**			3rd param of the object	data field to the item's
**			destination directory.  Optionally you can specify a
**			new name for the file.
**
**	STF Syntax:
**		ObjID	Batch	Title		Descr		Type		Data						Bmp Id	Vital	Shared	DirChng	DestDir	CheckDir	InstBy	InstData	InstDir
**		ID#		"Error Title"	"Error Text"		CustomAction	"EXPSETUP.DLL","FileCopy","213,NewName.Ext"					%224
**		ID#		"Error Title"	"Error Text"		CustomAction	"EXPSETUP.DLL","FileMove","213,NewName.Ext"					%224
**
**	Author: ******
**	Written For:	Explorapedia Win32/Win16 versions of Nature and People
**
****************************************************************************/

#define FILEMOVE_C

#include <windows.h>
#include <stdlib.h>		/* _MAX_PATH */
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"


RC FAR PASCAL FileCopy ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );
RC FAR PASCAL FileMove ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );


#define SzDirFromPcamfd(pcamfd)	((SZ)(((PCAMFDDtor)(pcamfd))->lpvPriv))

#define RcCallBack(pcd, or, camf, pcamfd) \
			((*(pcd)->pfncacb)(or, camf, pcamfd))


#ifdef _DEBUG
  #define DEBUG_OUTPUT

  #define MAX_CAMF 41
static  const char camfText[][24] = {
  	"camfInitializeObject",
  	"camfInitializeTrigger",
  	"camfInitFromProof",
  	"camfInitSearchObject",
  	"camfSearchForObject",
  	"camfAnswerDependClause",
  	"camfAnswerDependRemove",
  	"camfAnswerCompanion",
  	"camfSetDstDirOfTrigger",
  	"camfPropagateDstDir",
  	"camfCalcDstDir",
  	"camfSelectModeOfObject",
  	"camfSetModeOfObject",
  	"camfSetModeOfTrigger",
  	"camfCheckObjectIBSE",
  	"camfFinalizeObject",
  	"camfAddToCopyList",
  	"camfGetNonCopyListCost",
  	"camfGetCost",
  	"camfGetOODSCostStr",
  	"camfGetOODSCostNum",
  	"camfAddBillBoards",
  	"camfDoNonVisualMods",
  	"camfDoVisualMods",
  	"camfPostInstall",
  	"camfDtor",
  	"camfSelectableSubOpts",
  	"camfSetRgfChecked",
  	"camfGetUIDstDirObj",
  	"camfGetCAddCRemove",
  	"camfSetOisState",
  	"camfSetInstallData",
  	"camfSetDstDir",
  	"camfSetDstDirWithSubdir",
  	"camfSetDstDirUserChoice",
  	"camfProcessDstDirTree",
  	"camfSetDstDirInTree",
  	"camfSetIBSEState",
  	"camfSetVisitedIBSE",
  	"camfSetIbyState",
  	"camfGetObjData",
  	"camfError"};

  #define MAX_OIS 6
  const char oisText[][27] = {
  	"oisUnknown",
  	"oisNotInTree",
  	"oisNotYetInstalled",
  	"oisInstalledByUs",
  	"oisToBeInstalled",
  	"oisToBeRemoved",
  	"oisInstalledBySomebodyElse"};

#endif


RC FileCopyInit ( PCD pcd, POD pod, PCAMFD pcamfd, SZ szData )
{
	RC		rc = rcDoDefault;

	if ( ((PCAMFDInitializeObject)pcamfd)->lpvPriv == lpvoidNull)
		{
		SZ	pEntry;
		
		while ( (pEntry = (SZ)PbAlloc(_MAX_PATH)) == szNull)
			{
			if (!FHandleOOM())
				{
				#ifdef _DEBUG
				  DoMsgBox(szData, "FileMove - Out of Memory", MB_OK);
				#endif
			    	return(rcFail);
				}
			}

		*pEntry = '\0';
		((PCAMFDInitializeObject)pcamfd)->lpvPriv = (LPVOID)pEntry;
		rc=rcOk;
		}

	return (rc);
}



RC FileCopyCalcDstDir ( PCD pcd, POD pod, PCAMFD pcamfd, SZ szData )
{
	RC		rc = rcDoDefault;

	/*#ifdef _DEBUG
	char Text[128];
	wsprintf(Text, "lpvPriv = %lx", ((PCAMFDCalcDstDir)pcamfd)->lpvPriv);
	DoMsgBox(Text, "FileMove - CalcDstDir", MB_OK);
	#endif*/

	if( *((LPSTR) ((PCAMFDCalcDstDir)pcamfd)->lpvPriv ) == '\0')
		{
		SZ		pName;
		CAMFDGetObjData camfd;
		OR		or;

			// locate end of 1st param (Source Pathname object #)
		for(pName=szData ; *pName ; pName++)
		  if(',' == *pName)
		  	{
			*pName++ = '\0';
		    	break;
			}
		// At this point *pName='\0' or points to the 2nd param (dest filename)

			// Get Src path object number from our data entry
		or = atoi(szData);

			// Get object data for our src path object
		rc = RcCallBack(pcd, or, camfGetObjData, &camfd);
	
			// If the object exists
		if(rcOk == rc)
			{
			SZ p1;

				// save our source filename
			lstrcpy( (LPSTR)((PCAMFDCalcDstDir)pcamfd)->lpvPriv, camfd.podRes->szDstDir);

				// move 2nd param to start of szData
			p1 = szData;
			while(*pName)
				{
				*p1++ = *pName++;
				}
			*p1 = '\0';

				// now do default handling to actually obtain destination directory
			rc=rcDoDefault;
			}
		else // Source path object doesn't exist
			{
			#ifdef _DEBUG
			char Text[128];
			wsprintf(Text, "Source path object '%s' doesn't exist", (LPSTR)szData);
			DoMsgBox(Text, "FileMove Error", MB_OK);
			#endif
			rc=rcFail;
			}
		}


	return (rc);
}



RC FileCopyDoNonVisualMods (BOOL fDel, PCD pcd, POD pod, PCAMFD pcamfd, SZ szData)
{
	RC		rc = rcDoDefault;

	if( (oisToBeInstalled == pod->ois) &&               // If we're in the tree (oisNotYetInstalled if we're not in tree)
	    ( ((PCAMFDDoNonVisualMods)pcamfd)->lpvPriv ) )  // and we've got our source/dest info
		{
		CHAR	szDestPath[_MAX_PATH];
		LPSTR	lpEnd;
		
		/*#ifdef _DEBUG
		char Text[128];
		wsprintf(Text, "lpvPriv = %lx", ((PCAMFDDoNonVisualMods)pcamfd)->lpvPriv);
		DoMsgBox(Text, "FileMove - DoNonVisualMods", MB_OK);
		#endif
		*/
			// Ensure there is no trailing backslash
		lpEnd = (LPSTR)((PCAMFDDoNonVisualMods)pcamfd)->lpvPriv +
			lstrlen( (LPSTR)((PCAMFDDoNonVisualMods)pcamfd)->lpvPriv ) - 1;
		
		if('\\' == *lpEnd)
		  *lpEnd = '\0';

			// Make sure source file exists
		if(DoesFileExist(((PCAMFDDoNonVisualMods)pcamfd)->lpvPriv, femExists))
			{
			if(DoesDirExist(pod->szDstDir))
				{
					// Make a copy of destination
				SzStrCopy(szDestPath, pod->szDstDir);

					// If we've got a destination filename
				if(*szData)
				  SzStrCat(szDestPath, szData);
		
				#ifdef _DEBUG
				{
				char Text[300];
				wsprintf(Text, "NewName='%s'\nDestDir='%s'\nCopyDest='%s'",
					szData, pod->szDstDir, szDestPath);
				DoMsgBox(Text, "FileMove - DoNonVisualMods", MB_OK);
				}
				#endif

					// Copy the file
				CopyFile(((PCAMFDDoNonVisualMods)pcamfd)->lpvPriv, szDestPath, cmoCopy, FALSE);

					// Delete the original file
				if(TRUE == fDel)
          {
				  RemoveFile(((PCAMFDDoNonVisualMods)pcamfd)->lpvPriv, cmoRemove);
          }
				}
			else // Destination path doesn't exist
				{
				#ifdef _DEBUG
				char Text[256];
				wsprintf(Text, "Destination path '%s' doesn't exist", (LPSTR)pod->szDstDir);
				DoMsgBox(Text, "FileMove Error", MB_OK);
				#endif
				rc=rcFail;
				}
			}
		else // Source pathname doesn't exist
			{
			#ifdef _DEBUG
			char Text[256];
			wsprintf(Text, "Source file '%s' doesn't exist", (LPSTR)((PCAMFDDoNonVisualMods)pcamfd)->lpvPriv);
			DoMsgBox(Text, "FileCopy/Move Error", MB_OK);
			#endif
			rc=rcDoDefault;
			}

		}

	return (rc);
}



RC FileCopyDtor ( PCD pcd, POD pod, PCAMFD pcamfd, SZ szData )
{
	RC		rc = rcDoDefault;

	if (((PCAMFDDtor)pcamfd)->lpvPriv != lpvoidNull)
		{
		FFreeSz((SZ)((PCAMFDDtor)pcamfd)->lpvPriv);
		((PCAMFDDtor)pcamfd)->lpvPriv = lpvoidNull;

		rc = rcOk;
		}

	return (rc);
}



/*************************************************************************
**  Name:
**      FileCopy
**
**  Purpose:
**      Custom Action Handler to move a file from one directory to another
**	and rename it optionally
**
**  Arguments:
**      pcd   : Ptr to class data.
**      pod   : Ptr to object data.
**      camf  : Custom action member function type.
**      pcamfd: Ptr to custom action member function data
**          (variable size, depends on camf type).
**  Returns:
**      Appropriate RC (return code) value.
**
****************************************************************************/
RC FAR PASCAL FileCopy ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData )
{
	RC		rc = rcDoDefault;

	switch (camf)
		{
		case camfInitializeObject:
      rc = FileCopyInit(pcd, pod, pcamfd, szData);
			break;

		case camfCalcDstDir:
			rc = FileCopyCalcDstDir(pcd, pod, pcamfd, szData);
			break;

		case camfDoNonVisualMods:
			rc = FileCopyDoNonVisualMods(FALSE, pcd, pod, pcamfd, szData);
			break;

		case camfDtor:
			rc = FileCopyDtor(pcd, pod, pcamfd, szData);
			break;
		}

	if(rcFail == rc)
	  DoMsgBox(pod->szDescription, pod->szTitle, MB_OK | MB_ICONEXCLAMATION);

  #ifdef DEBUG_OUTPUT
    if(camf > MAX_CAMF)  // if we've got an illegal value
      camf = MAX_CAMF;   // force error
    DoMsgBox(oisText[pod->ois], camfText[camf], MB_OK);
  #endif

	return (rc);
}



/*************************************************************************
**  Name:
**      FileMove
**
**  Purpose:
**      Custom Action Handler to move a file from one directory to another
**	and rename it optionally
**
**  Arguments:
**      pcd   : Ptr to class data.
**      pod   : Ptr to object data.
**      camf  : Custom action member function type.
**      pcamfd: Ptr to custom action member function data
**          (variable size, depends on camf type).
**  Returns:
**      Appropriate RC (return code) value.
**
****************************************************************************/
RC FAR PASCAL FileMove ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData )
{
	RC		rc = rcDoDefault;

	switch (camf)
		{
		case camfInitializeObject:
      rc = FileCopyInit(pcd, pod, pcamfd, szData);
			break;

		case camfCalcDstDir:
			rc = FileCopyCalcDstDir(pcd, pod, pcamfd, szData);
			break;

		case camfDoNonVisualMods:
			rc = FileCopyDoNonVisualMods(TRUE, pcd, pod, pcamfd, szData);
			break;

		case camfDtor:
			rc = FileCopyDtor(pcd, pod, pcamfd, szData);
			break;
		}

	if(rcFail == rc)
	  DoMsgBox(pod->szDescription, pod->szTitle, MB_OK | MB_ICONEXCLAMATION);

	return (rc);
}
