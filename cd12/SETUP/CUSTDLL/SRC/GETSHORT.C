/***************************************************************************
**
**	File:		GET8-3.C
**	Purpose:	Sets the custom action's destination path to a valid
**			8.3 or short filename path.  You can use this to
**			obtain a short filename to be written to an ini
**			or registry entry for use by a 16bit app.
**
**	STF Syntax:
**		ObjID	Batch	Title	Descr	Type		Data						Bmp Id	Vital	Shared	DirChng	DestDir	CheckDir	InstBy	InstData	InstDir
**		ID#				CustomAction	"EXPSETUP.DLL","GetShortPath","ObjID#"
**
**	Author:		WSproule
**	Written For:	Explorapedia Win32/Win16 versions of Nature and People
**
****************************************************************************/


#define DELPMITM_C

#include <windows.h>
#include <stdlib.h>		/* _MAX_PATH */
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"


#ifdef _DEBUG
  #define DEBUG_OUTPUT
  const char camfText[][24] = {
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
#endif




RC SETUPAPI GetShortPath ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );


#define SzDirFromPcamfd(pcamfd)	((SZ)(((PCAMFDDtor)(pcamfd))->lpvPriv))

#define RcCallBack(pcd, or, camf, pcamfd) \
			((*(pcd)->pfncacb)(or, camf, pcamfd))




/*************************************************************************
**  Name:
**      GetShortPath
**
**  Purpose:
**      Custom Action handler sets its destination directory to the short
**	pathname equivalent of the passed object ID's destination directory.
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
RC SETUPAPI GetShortPath( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData )
{
RC	rc = rcDoDefault;

switch (camf)
	{
    // Note that in the CalcDstDir pass your new directories don't yet exist.
    // This is important because the GetShortPathName() function will fail
    // if its input directory does not yet exist.
    // Acme creates directories after the AddBillBoards pass but before the
    // DoNonVisualMods pass.  Since ACME will handle copying files to long
    // pathnames properly you should only need this custom action to save
    // short filenames to reg or ini entries.  Writing reg and ini entries
    // occures in the DoNonVisualMods pass of Acme which is a table walk pass.
    // Make sure this custom action is before (has a lower object number) than
    // any object that depends on it and everything will work fine.

	case camfDoNonVisualMods:
		if (pcd->simSetupMode != simMaintM
				&& pcd->simSetupMode != simAdminM)
			{
			CAMFDGetObjData camfdGetObjData;
			OR		or;

				// Get Src path object number from our data entry
			or = atoi(szData);

				// Get object data for our src path object
			rc = RcCallBack(pcd, or, camfGetObjData, &camfdGetObjData);
			
				// If the object exists
			if(rcOk == rc)
				{
				CHAR	szShortPathname[_MAX_PATH];
				CAMFDSetDstDir camfdSetDstDir;
				
          // This function will fail (returns 0) when the passed long filename directory (param 1)
          // does not yet exist.  I handle this situation by setting the short filename to \.
				if(!GetShortPathName(camfdGetObjData.podRes->szDstDir, (LPSTR)szShortPathname, _MAX_PATH))
				  lstrcpy(szShortPathname, "\\");

      	if (*SzLastChar(szShortPathname) != chDirSep)
      		SzStrCat(szShortPathname, szDirSep);

				camfdSetDstDir.szDir = szShortPathname;
				camfdSetDstDir.fDup  = fTrue;
				rc = RcCallBack(pcd, pod->or, camfSetDstDir, &camfdSetDstDir);
				Assert(rc == rcOk);

				((PCAMFDCalcDstDir)pcamfd)->fRes = camfdSetDstDir.fRes;
				
				#ifdef DEBUG_OUTPUT
				{
				char szBuf[1600];
				wsprintf(szBuf, "ObjID=%u\nShort='%s'\nLong='%s'\nTblDstDir='%s'\nDstDir='%s'\nInstallDir='%s'",
				    or, (LPSTR)szShortPathname,  (LPSTR)camfdGetObjData.podRes->szDstDir,
				    (LPSTR)pod->szTblDstDir, (LPSTR)pod->szDstDir, (LPSTR)pod->szInstallDir);
				DoMsgBox(szBuf, "GetShortPath Success", MB_OK);
				}
				#endif

				}
			else
				{
				#ifdef DEBUG_OUTPUT
				char Text[256];
				wsprintf(Text, "Object ID '%s' doesn't exist", (LPSTR)szData);
				DoMsgBox(Text, "GetShortPath Error", MB_OK);
				#endif
				}
			}
		break;

  #ifdef DEBUG_OUTPUT
    default:
      DoMsgBox(camfText[camf], "GetShort", MB_OK);
      break;
  #endif
	}
return (rc);
}
