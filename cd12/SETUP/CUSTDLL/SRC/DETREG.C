/***************************************************************************
**
**	File:			DETREG.C
**	Purpose:		Sample Detection Custom Action Handler
**					routines for Acme Setup.
**	Notes:
**
****************************************************************************/

#define DETREG_C

#include <windows.h>
#include <stdlib.h>  
#include <string.h> 
#include <shellapi.h>
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"


RC FAR PASCAL DetectRegData ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData );


STATIC_FN RC PRIVATE DRDRcInitializeTrigger ( PCD pcd, POD pod );
STATIC_FN RC PRIVATE DRDRcSetDstDirOfTrigger ( PCD pcd, POD pod,
						PCAMFDSetDstDirOfTrigger pcamfd );
STATIC_FN RC PRIVATE DRDRcSetModeOfTrigger ( PCD pcd, POD pod,
						PCAMFDSetModeOfTrigger pcamfd );
STATIC_FN RC PRIVATE DRDRcAnswerDependClause ( PCAMFDAnswerDependClause pcamfd );

#define MAX_STRING 256
char RegKey[MAX_STRING];
char RegValue[MAX_STRING];

/*
**	Purpose:
**		Custom Action Handler for sample Detect EGA.  Used as a
**		"trigger" for a Depend object.
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
RC FAR PASCAL DetectRegData ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData )
{
	RC	rc = rcDoDefault;

	strcpy(RegKey,szData);
		
	switch (camf)
		{
		case camfInitializeTrigger:
			rc = DRDRcInitializeTrigger(pcd, pod);
			break;

		case camfSetDstDirOfTrigger:
			rc = DRDRcSetDstDirOfTrigger(pcd, pod,
						(PCAMFDSetDstDirOfTrigger)pcamfd);
			break;

		case camfSetModeOfTrigger:
			rc = DRDRcSetModeOfTrigger(pcd, pod,
						(PCAMFDSetModeOfTrigger)pcamfd);
			break;

		case camfAnswerDependClause:
			rc = DRDRcAnswerDependClause((PCAMFDAnswerDependClause)pcamfd);
			break;

		default:
			break;
		}

	return (rc);
}


/*
****************************************************************************/
STATIC_FN RC PRIVATE DRDRcInitializeTrigger ( PCD pcd, POD pod )
{
	CAMFDSetIBSEState		camfdIBSE;
	CAMFDSetVisitedIBSE		camfdVis;
	CAMFDInitializeObject	camfdInit;
	RC	rc;
	
	camfdIBSE.ynme = ynmeNo;
	rc = (*pcd->pfncacb)(pod->or, camfSetIBSEState, &camfdIBSE);
	if (rc != rcOk)
		return (rc);

	camfdVis.f = fTrue;
	rc = (*pcd->pfncacb)(pod->or, camfSetVisitedIBSE, &camfdVis);
	if (rc != rcOk)
		return (rc);

	return ((*pcd->pfncacb)(pod->or, camfInitializeObject, &camfdInit));
}


/*
****************************************************************************/
STATIC_FN RC PRIVATE DRDRcSetDstDirOfTrigger ( PCD pcd, POD pod,
						PCAMFDSetDstDirOfTrigger pcamfd )
{
	CAMFDCalcDstDir	camfdCalc;
	RC	rc;

	Assert(pcamfd->szParentDstDir != szNull);
	Assert(FValidDir(pcamfd->szParentDstDir));

	if (!pod->fDstDirSet)
		{
		camfdCalc.szParentDstDir = pcamfd->szParentDstDir;
		
		rc = (*pcd->pfncacb)(pod->or, camfCalcDstDir, &camfdCalc);
		if (rc != rcOk)
			return (rc);

		if (!camfdCalc.fRes)
			return (rcFail);
		}
	return (rcOk);
}


/*
****************************************************************************/
STATIC_FN RC PRIVATE DRDRcSetModeOfTrigger ( PCD pcd, POD pod,
						PCAMFDSetModeOfTrigger pcamfd )
{
	CAMFDSetModeOfObject	camfdMode;

	camfdMode.sma = pcamfd->sma;
	return ((*pcd->pfncacb)(pod->or, camfSetModeOfObject, &camfdMode));
}


/*
****************************************************************************/
STATIC_FN RC PRIVATE DRDRcAnswerDependClause ( PCAMFDAnswerDependClause pcamfd )
{
	HKEY h;
	long size;
	
	pcamfd->fRes = fFalse;
	
	if(RegOpenKey(HKEY_CLASSES_ROOT,RegKey,&h)==ERROR_SUCCESS)
	  {
		size = sizeof(RegValue);
		if(RegQueryValue(h,"",RegValue,&size)==ERROR_SUCCESS)
			pcamfd->fRes = fTrue;		
		RegCloseKey(h);
	  }

	return (rcOk);
}
