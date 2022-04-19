/****************************************************************************
**
**	File:			SAMPCACB.H
**	Purpose:		Header file for Custom Action Callback function
**	        		wrappers for the Sample App DLL.
**	Notes:
**
****************************************************************************/

#ifndef SAMPCACB_H
#define SAMPCACB_H



RC   PUBLIC RcInitializeObject ( PCD pcd, OR or );
BOOL PUBLIC FCalcDstDir ( PCD pcd, OR or, SZ szParentDstDir );
RC   PUBLIC RcSetModeOfObject ( PCD pcd, OR or, SMA sma );
YNME PUBLIC YnmeCheckObjectIBSE ( PCD pcd, OR or );
RC   PUBLIC RcGetCost ( PCD pcd, OR or, PSCB pscb, SZ szDestDir );
UINT PUBLIC OrGetUIDstDirObj ( PCD pcd, OR or );
RC   PUBLIC RcGetOODSCostNum ( PCD pcd, OR or, CHAR chDrv, PSCB pscb );
RC   PUBLIC RcGetOODSCostStr ( PCD pcd, OR or, SZ szSym, CHAR chDrv,
				UINT depth, BOOL fExtendGrp );

BOOL PUBLIC FSetOisState ( PCD pcd, OR or, OIS oisNew );
BOOL PUBLIC FSetDstDir ( PCD pcd, OR or, SZ szDir, BOOL fDup );
VOID PUBLIC SetDstDirUserChoice ( PCD pcd, OR or, BOOL f );
RC   PUBLIC RcSetDstDirInTree ( PCD pcd, OR or, SZ szParentDstDir,
				BOOL fUserChoice, BOOL fForceRecalc );
VOID PUBLIC SetIBSEState ( PCD pcd, OR or, YNME ynme );
VOID PUBLIC SetVisitedIBSE ( PCD pcd, OR or, BOOL f );
POD  PUBLIC PodGetObjData ( PCD pcd, OR or );


#endif  /* SAMPCACB_H */

