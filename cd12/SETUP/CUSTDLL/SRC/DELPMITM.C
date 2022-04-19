/***************************************************************************
**
**	File:		DELPMITM.C
**	Purpose:	Deletes a program manager item or a Win95 start menu
**			item and the group its in if there are no other
**			items in the group.
**
**	STF Syntax:
**		ObjID	Batch	Title	Descr	Type		Data						Bmp Id	Vital	Shared	DirChng	DestDir	CheckDir	InstBy	InstData	InstDir
**		ID#				CustomAction	"EXPSETUP.DLL","DelProgManItem","Group,Item"
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


RC SETUPAPI DelProgManItem ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );


#define SzDirFromPcamfd(pcamfd)	((SZ)(((PCAMFDDtor)(pcamfd))->lpvPriv))

#define RcCallBack(pcd, or, camf, pcamfd) \
			((*(pcd)->pfncacb)(or, camf, pcamfd))




/*************************************************************************
**  Name:
**      DelProgManItem
**
**  Purpose:
**      Custom Action Handler to delete a program manager item or a Win95
**	start menu item.
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
RC SETUPAPI DelProgManItem ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData )
{
RC	rc = rcDoDefault;

switch (camf)
	{
	case camfDoVisualMods:
    if(oisToBeInstalled == pod->ois)  // If we're in the tree (oisNotYetInstalled if we're not in tree)
		{
  		SZ		pItem;

  			// locate end of 1st param (PM Group)
  		for(pItem=szData ; *pItem ; pItem++)
  		  if(',' == *pItem)
  		  {
  			  *pItem++ = '\0';
  		    break;
  			}
  		// At this point *pItem='\0' or points to the 2nd param (PM Item)

  		DeleteProgmanItem(szData,	// Program manager group
  				  pItem,	// Program manager item
  				  cmoRemove,
  				  TRUE);	// Delete group if its now empty

  		rc=rcOk;
		}
		break;
	}
return (rc);
}
