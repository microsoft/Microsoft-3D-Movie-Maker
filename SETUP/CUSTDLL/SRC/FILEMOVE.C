/***************************************************************************
**
**	File:		FILEMOVE.C
**	Purpose:	Moves the file specified by the object number in the
**			3rd param of the object	data field to the item's
**			destination directory.  Optionally you can specify a
**			new name for the file.
**
**	STF Syntax:
**		ObjID	Batch	Title		Descr		Type		Data						Bmp Id	Vital	Shared	DirChng	DestDir	CheckDir	InstBy	InstData	InstDir
**		ID#		"Error Title"	"Error Text"		CustomAction	"EXPSETUP.DLL","Move File","213,NewName.Ext"					%224
**
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


RC FAR PASCAL FileMove ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );


#define SzDirFromPcamfd(pcamfd)	((SZ)(((PCAMFDDtor)(pcamfd))->lpvPriv))

#define RcCallBack(pcd, or, camf, pcamfd) \
			((*(pcd)->pfncacb)(or, camf, pcamfd))




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
			if ( ((PCAMFDInitializeObject)pcamfd)->lpvPriv == lpvoidNull)
				{
				SZ	pEntry;
				
				while ( (pEntry = (SZ)PbAlloc(_MAX_PATH)) == szNull)
					{
					if (!FHandleOOM())
						{
						#ifdef _DEBUG
						  MessageBox(NULL, szData, "FileMove - Out of Memory", MB_OK);
						#endif
					    	return(rcFail);
						}
					}

				*pEntry = '\0';
				((PCAMFDInitializeObject)pcamfd)->lpvPriv = (LPVOID)pEntry;
				rc=rcOk;
				}
			break;

		case camfCalcDstDir:
			{
			#ifdef _DEBUG
			char Text[128];
			wsprintf(Text, "lpvPriv = %lx", ((PCAMFDCalcDstDir)pcamfd)->lpvPriv);
			DoMsgBox(Text, "FileMove - CalcDstDir", MB_OK);
			#endif

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
			break;
			}

		case camfDoNonVisualMods:
			if( ((PCAMFDDoNonVisualMods)pcamfd)->lpvPriv )
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
						RemoveFile(((PCAMFDDoNonVisualMods)pcamfd)->lpvPriv, cmoRemove);
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
					DoMsgBox(Text, "FileMove Error", MB_OK);
					#endif
					rc=rcDoDefault;
					}

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
		}

	if(rcFail == rc)
	  DoMsgBox(pod->szDescription, pod->szTitle, MB_OK | MB_ICONEXCLAMATION);

	return (rc);
}
