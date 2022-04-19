/***************************************************************************
**
**	File:			SAMPCACB.C
**	Purpose:		Sample application-specific DLL Custom
**					Action Callback routine wrappers.
**	Notes:
**
****************************************************************************/

#define SAMPCACB_C

#include "stdinc.h"
#include <stdlib.h>		/* _MAX_PATH */
#include "setupapi.h"
#include "stdtypes.h"
#include "datadef.h"


/*
***************************************************************************/
RC PUBLIC RcInitializeObject ( PCD pcd, OR or )
{
	CAMFDInitializeObject camfd;

	return ((*pcd->pfncacb)(or, camfInitializeObject, &camfd));
}


/*
***************************************************************************/
BOOL PUBLIC FCalcDstDir ( PCD pcd, OR or, SZ szParentDstDir )
{
	CAMFDCalcDstDir camfd;

	camfd.szParentDstDir = szParentDstDir;
	(*pcd->pfncacb)(or, camfCalcDstDir, &camfd);
	return (camfd.fRes);
}


/*
***************************************************************************/
RC PUBLIC RcSetModeOfObject ( PCD pcd, OR or, SMA sma )
{
	CAMFDSetModeOfObject camfd;

	camfd.sma = sma;
	return ((*pcd->pfncacb)(or, camfSetModeOfObject, &camfd));
}


/*
***************************************************************************/
YNME PUBLIC YnmeCheckObjectIBSE ( PCD pcd, OR or )
{
	CAMFDCheckObjectIBSE camfd;

	(*pcd->pfncacb)(or, camfCheckObjectIBSE, &camfd);
	return (camfd.ynmeRes);
}


/*
***************************************************************************/
RC PUBLIC RcGetCost ( PCD pcd, OR or, PSCB pscb, SZ szDestDir )
{
	CAMFDGetCost camfd;

	camfd.pscb      = pscb;
	camfd.szDestDir = szDestDir;
	return ((*pcd->pfncacb)(or, camfGetCost, &camfd));
}


/*
***************************************************************************/
RC PUBLIC RcGetOODSCostNum ( PCD pcd, OR or, CHAR chDrv, PSCB pscb )
{
	CAMFDGetOODSCostNum camfd;

	camfd.chDrv = chDrv;
	camfd.pscb  = pscb;
	return ((*pcd->pfncacb)(or, camfGetOODSCostNum, &camfd));
}


/*
***************************************************************************/
RC PUBLIC RcGetOODSCostStr ( PCD pcd, OR or, SZ szSym, CHAR chDrv,
				UINT depth, BOOL fExpandGrp )
{
	CAMFDGetOODSCostStr camfd;

	camfd.szSym      = szSym;
	camfd.chDrv      = chDrv;
	camfd.depth      = depth;
	camfd.fExpandGrp = fExpandGrp;
	return ((*pcd->pfncacb)(or, camfGetOODSCostStr, &camfd));
}




/*
***************************************************************************/
UINT PUBLIC OrGetUIDstDirObj ( PCD pcd, OR or )
{
	CAMFDGetUIDstDirObj camfd;

	(*pcd->pfncacb)(or, camfGetUIDstDirObj, &camfd);
	return (camfd.orObjIDRes);
}




/*
***************************************************************************/
BOOL PUBLIC FSetOisState ( PCD pcd, OR or, OIS oisNew )
{
	CAMFDSetOisState camfd;

	camfd.oisNew = oisNew;
	(*pcd->pfncacb)(or, camfSetOisState, &camfd);
	return (camfd.fRes);
}


/*
***************************************************************************/
BOOL PUBLIC FSetDstDir ( PCD pcd, OR or, SZ szDir, BOOL fDup )
{
	CAMFDSetDstDir camfd;

	camfd.szDir = szDir;
	camfd.fDup  = fDup;
	(*pcd->pfncacb)(or, camfSetDstDir, &camfd);
	return (camfd.fRes);
}


/*
***************************************************************************/
VOID PUBLIC SetDstDirUserChoice ( PCD pcd, OR or, BOOL f )
{
	CAMFDSetDstDirUserChoice camfd;

	camfd.f = f;
	(*pcd->pfncacb)(or, camfSetDstDirUserChoice, &camfd);
}


/*
***************************************************************************/
RC PUBLIC RcSetDstDirInTree ( PCD pcd, OR or, SZ szParentDstDir,
				BOOL fUserChoice, BOOL fForceRecalc )
{
	CAMFDSetDstDirInTree camfd;

	camfd.szParentDstDir = szParentDstDir;
	camfd.fUserChoice    = fUserChoice;
	camfd.fForceRecalc   = fForceRecalc;
	return ((*pcd->pfncacb)(or, camfSetDstDirInTree, &camfd));
}


/*
***************************************************************************/
VOID PUBLIC SetIBSEState ( PCD pcd, OR or, YNME ynme )
{
	CAMFDSetIBSEState camfd;

	camfd.ynme = ynme;
	(*pcd->pfncacb)(or, camfSetIBSEState, &camfd);
}


/*
***************************************************************************/
VOID PUBLIC SetVisitedIBSE ( PCD pcd, OR or, BOOL f )
{
	CAMFDSetVisitedIBSE camfd;

	camfd.f = f;
	(*pcd->pfncacb)(or, camfSetVisitedIBSE, &camfd);
}


/*
***************************************************************************/
POD PUBLIC PodGetObjData ( PCD pcd, OR or )
{
	CAMFDGetObjData camfd;

	(*pcd->pfncacb)(or, camfGetObjData, &camfd);
	return (camfd.podRes);
}


