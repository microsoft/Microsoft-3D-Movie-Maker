/***************************************************************************
**
**	File:		GETPARNT.C
**	Purpose:	Sets the custom action's destination path to the parent
**			      directory of the passed input object's destination directory.
**
**	STF Syntax:
**		ObjID	Batch	Title	Descr	Type		Data						Bmp Id	Vital	Shared	DirChng	DestDir	CheckDir	InstBy	InstData	InstDir
**		ID#				CustomAction	"EXPSETUP.DLL","GetParentDir","ObjID#"
**
**	Author:		WSproule
**	Written For:	Explorapedia Win32/Win16 versions of Nature and People
**
****************************************************************************/


#define GETPARNT_C

#include <windows.h>
#include <stdlib.h>		/* _MAX_PATH */
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"


#ifdef _DEBUG
  #define DEBUG_OUTPUT
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
#endif




RC SETUPAPI GetParentDir ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );


#define SzDirFromPcamfd(pcamfd)	((SZ)(((PCAMFDDtor)(pcamfd))->lpvPriv))

#define RcCallBack(pcd, or, camf, pcamfd) \
			((*(pcd)->pfncacb)(or, camf, pcamfd))




/*************************************************************************
**  Name:
**      GetParent
**
**  Purpose:
**      Custom Action handler sets its destination directory to the parent
**      directory of the passed object id's destination directory.
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
RC SETUPAPI GetParentDir( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData )
{
RC	rc = rcDoDefault;

switch (camf)
	{
	case camfCalcDstDir:
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
				CHAR	szParentPathname[_MAX_PATH];
				CAMFDSetDstDir camfdSetDstDir;
        short iPos;
				
      	lstrcpy(szParentPathname, camfdGetObjData.podRes->szDstDir);

        iPos = lstrlen(szParentPathname)-1;

        while(iPos > 0)
          if(szParentPathname[iPos] == '\\')
            {
            szParentPathname[iPos] = '\0';
            break;
            }
          else
            iPos--;

        //if(IsValidPath(szParentPathname))
          {
  				camfdSetDstDir.szDir = szParentPathname;
  				camfdSetDstDir.fDup  = fTrue;
  				rc = RcCallBack(pcd, pod->or, camfSetDstDir, &camfdSetDstDir);
  				Assert(rc == rcOk);

  				((PCAMFDCalcDstDir)pcamfd)->fRes = camfdSetDstDir.fRes;
          }
				
				}
			else
				{
				#ifdef DEBUG_OUTPUT
				char Text[256];
				wsprintf(Text, "Object ID '%s' doesn't exist", (LPSTR)szData);
				DoMsgBox(Text, "GetParent Error", MB_OK);
				#endif
        rc = rcFail;
				}
			}
		break;

  #ifdef DEBUG_OUTPUT
    default:
      DoMsgBox(camfText[camf], "GetParent", MB_OK);
      break;
  #endif
	}
return (rc);
}
