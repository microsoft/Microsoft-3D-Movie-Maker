/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
**
**	File:			DETECTOS.C
**	Purpose:		Sample Detection Custom Action Handler
**					routines for Acme Setup.
**	Notes:
**
****************************************************************************/

#define DETECTOS_C

#include "stdinc.h"
#include <stdlib.h>		/* _MAX_PATH */
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"
//#include "parseca.h"


/* OS's to check for:
*/
#define	STR_WIN95	"win95"
#define STR_WINNT	"winnt"


RC FAR PASCAL DetectOS ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData );


STATIC_FN RC PRIVATE RcInitializeTrigger ( PCD pcd, POD pod );
STATIC_FN RC PRIVATE RcSetDstDirOfTrigger ( PCD pcd, POD pod,
						PCAMFDSetDstDirOfTrigger pcamfd );
STATIC_FN RC PRIVATE RcSetModeOfTrigger ( PCD pcd, POD pod,
						PCAMFDSetModeOfTrigger pcamfd );
STATIC_FN RC PRIVATE RcAnswerDependClause ( PCD pcd, POD pod, SZ szData,
						PCAMFDAnswerDependClause pcamfd );


/*
**	Purpose:
**		Custom Action Handler for to detect Windows95 or WindowsNT.
**		Takes one .STF data argument, specifying either "Win95" or "WinNT".
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
RC FAR PASCAL DetectOS ( PCD pcd, POD pod, CAMF camf,
				PCAMFD pcamfd, SZ szData )
{
	RC	rc = rcDoDefault;

	Unused(szData);

	switch (camf)
		{
		case camfInitializeTrigger:
			rc = RcInitializeTrigger(pcd, pod);
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

		default:
			break;
		}

	return (rc);
}


/*
****************************************************************************/
STATIC_FN RC PRIVATE RcInitializeTrigger ( PCD pcd, POD pod )
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
STATIC_FN RC PRIVATE RcSetDstDirOfTrigger ( PCD pcd, POD pod,
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

	Assert(pcamfd != pcamfdNull);
	Assert(szData != szNull);

	if (CrcStringCompareI(szData, STR_WIN95) == crcEqual)
		pcamfd->fRes = (pcd->wvtWinVerType == wvtChicago) ? fTrue : fFalse;

	else if (CrcStringCompareI(szData, STR_WINNT) == crcEqual)
		pcamfd->fRes = (pcd->wvtWinVerType == wvtWinNT) ? fTrue : fFalse;

	else
		{
		pcamfd->fRes = fFalse;
		}

	return (rcOk);
}



