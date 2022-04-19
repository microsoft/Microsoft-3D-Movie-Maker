/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Windows menu code.
	REVIEW shonk: error codes

***************************************************************************/
#include "frame.h"
ASSERTNAME

const achar kchList = '_';
const achar kchFontList = '$';

PMUB vpmubCur;
RTCLASS(MUB)

/***************************************************************************
	Destructor - make sure vpmubCur is not this mub.
***************************************************************************/
MUB::~MUB(void)
{
	//REVIEW shonk: free mem and the _hmenu
	if (vpmubCur == this)
		vpmubCur = pvNil;
}


/***************************************************************************
	Static method to load and set a new menu bar.
***************************************************************************/
PMUB MUB::PmubNew(ulong ridMenuBar)
{
	PMUB pmub;

	if ((pmub = NewObj MUB) == pvNil)
		return pvNil;
	if ((pmub->_hmenu = LoadMenu(vwig.hinst, MIR(ridMenuBar))) == hNil)
		{
		ReleasePpo(&pmub);
		return pvNil;
		}
	pmub->_cmnu = GetMenuItemCount(pmub->_hmenu);
	if (!pmub->_FInitLists())
		{
		ReleasePpo(&pmub);
		return pvNil;
		}

	pmub->Set();
	return pmub;
}


/***************************************************************************
	Make this the current menu bar.
***************************************************************************/
void MUB::Set(void)
{
	if (vwig.hwndClient != hNil)
		{
		Assert(IsWindow(vwig.hwndClient), "bad client window");
		SendMessage(vwig.hwndClient, WM_MDISETMENU, (WPARAM)_hmenu, hNil);
		}
	else
		SetMenu(vwig.hwndApp, _hmenu);

	vpmubCur = this;
}


/***************************************************************************
	Make sure the menu's are clean - ie, items are enabled/disabled/marked
	correctly.  Called immediately before dropping the menus.
***************************************************************************/
void MUB::Clean(void)
{
	long imnu, imni, cmnu;
	ulong grfeds;
	HMENU hmenu;
	long wcid;
	CMD cmd;

	//adjust for the goofy mdi window's menu
	cmnu = GetMenuItemCount(_hmenu);
	Assert(cmnu >= _cmnu, "somebody took some menus out of the menu bar!");

	if (cmnu > _cmnu)
		{
		imnu = 1;
		cmnu = _cmnu + 1;
		}
	else
		imnu = 0;

	for ( ; imnu < cmnu; imnu++)
		{
		if ((hmenu = GetSubMenu(_hmenu, imnu)) == hNil)
			continue;

		for (imni = GetMenuItemCount(hmenu); imni-- != 0; )
			{
			if ((wcid = GetMenuItemID(hmenu, imni)) == cidNil)
				continue;

			if (!_FGetCmdForWcid(wcid, &cmd))
				grfeds = fedsDisable;
			else
				{
				grfeds = vpcex->GrfedsForCmd(&cmd);
				ReleasePpo(&cmd.pgg);
				}

			if (grfeds & fedsEnable)
				EnableMenuItem(hmenu, imni, MF_BYPOSITION | MF_ENABLED);
			else if (grfeds & fedsDisable)
				EnableMenuItem(hmenu, imni, MF_BYPOSITION | MF_GRAYED);

			if (grfeds & kgrfedsMark)
				{
				//REVIEW shonk: bullet doesn't work (uses a check mark)
				CheckMenuItem(hmenu, imni,
					(grfeds & (fedsCheck | fedsBullet)) ?
						MF_BYPOSITION | MF_CHECKED :
						MF_BYPOSITION | MF_UNCHECKED);
				}
			}
		}
}


/***************************************************************************
	The given wcid is the value Windows handed us in a WM_COMMAND message.
	Remap it to a real command id and enqueue the result.
***************************************************************************/
void MUB::EnqueueWcid(long wcid)
{
	CMD cmd;

	if (_FGetCmdForWcid(wcid, &cmd))
		vpcex->EnqueueCmd(&cmd);
}


/***************************************************************************
	Adds an item identified by the given list cid, long parameter
	and string.
***************************************************************************/
bool MUB::FAddListCid(long cid, long lw0, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	long imlst, ilw;
	MLST mlst;
	HMENU hmenuPrev;
	long dimni;
	bool fSeparator;
	bool fRet = fTrue;

	if (pvNil == _pglmlst)
		return fTrue;

	hmenuPrev = hNil;
	dimni = 0;
	for (imlst = 0; imlst < _pglmlst->IvMac(); imlst++)
		{
		_pglmlst->Get(imlst, &mlst);
		if (mlst.hmenu == hmenuPrev)
			{
			mlst.imniBase += dimni;
			_pglmlst->Put(imlst, &mlst);
			}
		else
			{
			hmenuPrev = mlst.hmenu;
			dimni = 0;
			}
		if (cid != mlst.cid)
			goto LAdjustSeparator;

		if (pvNil == mlst.pgllw)
			{
			if (pvNil == (mlst.pgllw = GL::PglNew(size(long))))
				{
				fRet = fFalse;
				goto LAdjustSeparator;
				}
			_pglmlst->Put(imlst, &mlst);
			mlst.pgllw->SetMinGrow(10);
			}

		ilw = mlst.pgllw->IvMac();
		if (!mlst.pgllw->FPush(&lw0))
			{
			fRet = fFalse;
			goto LAdjustSeparator;
			}
		if (!InsertMenu(mlst.hmenu, mlst.imniBase + ilw, MF_BYPOSITION | MF_STRING,
				mlst.wcidList + ilw, pstn->Psz()))
			{
			fRet = fFalse;
			AssertDo(mlst.pgllw->FPop(), 0);
			goto LAdjustSeparator;
			}
		dimni++;

LAdjustSeparator:
		fSeparator = mlst.imniBase > FPure(mlst.fSeparator) && pvNil != mlst.pgllw &&
			mlst.pgllw->IvMac() > 0;
		if (fSeparator && !mlst.fSeparator)
			{
			//add a separator
			if (!InsertMenu(mlst.hmenu, mlst.imniBase, MF_BYPOSITION | MF_SEPARATOR,
					cidNil, pvNil))
				{
				fRet = fFalse;
				}
			else
				{
				mlst.imniBase++;
				mlst.fSeparator = fTrue;
				_pglmlst->Put(imlst, &mlst);
				dimni++;
				}
			}
		else if (!fSeparator && mlst.fSeparator)
			{
			//delete a separator
			if (!DeleteMenu(mlst.hmenu, mlst.imniBase - 1, MF_BYPOSITION))
				fRet = fFalse;
			else
				{
				mlst.imniBase--;
				mlst.fSeparator = fFalse;
				_pglmlst->Put(imlst, &mlst);
				dimni--;
				}
			}
		}

	return fRet;
}


/***************************************************************************
	Removes all items identified by the given list cid, and long parameter
	or string.  If pstn is non-nil, it is used to find the item.
	If pstn is nil, lw0 is used to identify the item.
***************************************************************************/
bool MUB::FRemoveListCid(long cid, long lw0, PSTN pstn)
{
	AssertThis(0);
	AssertNilOrPo(pstn, 0);
	long imlst, ilw, cch;
	MLST mlst;
	SZ sz;
	HMENU hmenuPrev;
	long dimni;
	long lw;
	bool fSeparator, fSetWcid;
	bool fRet = fTrue;

	if (pvNil == _pglmlst)
		return fTrue;

	hmenuPrev = hNil;
	dimni = 0;
	for (imlst = 0; imlst < _pglmlst->IvMac(); imlst++)
		{
		_pglmlst->Get(imlst, &mlst);
		if (mlst.hmenu == hmenuPrev)
			{
			mlst.imniBase += dimni;
			Assert(mlst.imniBase >= FPure(mlst.fSeparator), "bad imniBase");
			_pglmlst->Put(imlst, &mlst);
			}
		else
			{
			hmenuPrev = mlst.hmenu;
			dimni = 0;
			}
		if (cid != mlst.cid || pvNil == mlst.pgllw)
			goto LAdjustSeparator;

		fSetWcid = fFalse;
		for (ilw = 0; ilw < mlst.pgllw->IvMac(); ilw++)
			{
			if (pvNil == pstn)
				{
				mlst.pgllw->Get(ilw, &lw);
				if (lw != lw0)
					goto LSetWcid;
				}
			else
				{
				cch = GetMenuString(mlst.hmenu, mlst.imniBase + ilw,
					sz, kcchMaxSz, MF_BYPOSITION);
				if (!pstn->FEqualRgch(sz, cch))
					goto LSetWcid;
				}
			if (!DeleteMenu(mlst.hmenu,	mlst.imniBase + ilw, MF_BYPOSITION))
				{
				fRet = fFalse;
LSetWcid:
				if (fSetWcid)
					{
					cch = GetMenuString(mlst.hmenu, mlst.imniBase + ilw,
						sz, kcchMaxSz, MF_BYPOSITION);
					if (cch == 0 || !ModifyMenu(mlst.hmenu, mlst.imniBase + ilw,
							MF_BYPOSITION, mlst.wcidList + ilw, sz))
						{
						fRet = fFalse;
						}
					}
				continue;
				}

			mlst.pgllw->Delete(ilw--);
			dimni--;
			fSetWcid = fTrue;
			}

LAdjustSeparator:
		fSeparator = mlst.imniBase > FPure(mlst.fSeparator) && pvNil != mlst.pgllw &&
			mlst.pgllw->IvMac() > 0;
		if (fSeparator && !mlst.fSeparator)
			{
			//add a separator
			if (!InsertMenu(mlst.hmenu, mlst.imniBase, MF_BYPOSITION | MF_SEPARATOR,
					cidNil, pvNil))
				{
				fRet = fFalse;
				}
			else
				{
				mlst.imniBase++;
				mlst.fSeparator = fTrue;
				_pglmlst->Put(imlst, &mlst);
				dimni++;
				}
			}
		else if (!fSeparator && mlst.fSeparator)
			{
			//delete a separator
			if (!DeleteMenu(mlst.hmenu, mlst.imniBase - 1, MF_BYPOSITION))
				fRet = fFalse;
			else
				{
				mlst.imniBase--;
				mlst.fSeparator = fFalse;
				_pglmlst->Put(imlst, &mlst);
				dimni--;
				}
			}
		}

	return fRet;
}


/***************************************************************************
	Removes all items identified by the given list cid.
***************************************************************************/
bool MUB::FRemoveAllListCid(long cid)
{
	AssertThis(0);
	long imlst, ilw;
	MLST mlst;
	HMENU hmenuPrev;
	long dimni;
	bool fSeparator;
	bool fRet = fTrue;

	if (pvNil == _pglmlst)
		return fTrue;

	hmenuPrev = hNil;
	dimni = 0;
	for (imlst = 0; imlst < _pglmlst->IvMac(); imlst++)
		{
		_pglmlst->Get(imlst, &mlst);
		if (mlst.hmenu == hmenuPrev)
			{
			mlst.imniBase += dimni;
			Assert(mlst.imniBase >= FPure(mlst.fSeparator), "bad imniBase");
			_pglmlst->Put(imlst, &mlst);
			}
		else
			{
			hmenuPrev = mlst.hmenu;
			dimni = 0;
			}
		if (cid == mlst.cid && pvNil != mlst.pgllw)
			{
			for (ilw = 0; ilw < mlst.pgllw->IvMac(); ilw++)
				{
				if (!DeleteMenu(mlst.hmenu,	mlst.imniBase + ilw, MF_BYPOSITION))
					{
					fRet = fFalse;
					continue;
					}
				mlst.pgllw->Delete(ilw--);
				dimni--;
				}
			}

		fSeparator = mlst.imniBase > FPure(mlst.fSeparator) && pvNil != mlst.pgllw &&
			mlst.pgllw->IvMac() > 0;
		if (fSeparator && !mlst.fSeparator)
			{
			//add a separator
			if (!InsertMenu(mlst.hmenu, mlst.imniBase, MF_BYPOSITION | MF_SEPARATOR,
					cidNil, pvNil))
				{
				fRet = fFalse;
				}
			else
				{
				mlst.imniBase++;
				mlst.fSeparator = fTrue;
				_pglmlst->Put(imlst, &mlst);
				dimni++;
				}
			}
		else if (!fSeparator && mlst.fSeparator)
			{
			//delete a separator
			if (!DeleteMenu(mlst.hmenu, mlst.imniBase - 1, MF_BYPOSITION))
				fRet = fFalse;
			else
				{
				mlst.imniBase--;
				mlst.fSeparator = fFalse;
				_pglmlst->Put(imlst, &mlst);
				dimni--;
				}
			}
		}

	return fRet;
}


/***************************************************************************
	Changes the long parameter and the menu text associated with a menu
	list item.  If pstnOld is non-nil, it is used to find the item.
	If pstnOld is nil, lwOld is used to identify the item.  In either case
	lwNew is set as the new long parameter and if pstnNew is non-nil,
	it is used as the new menu item text.
***************************************************************************/
bool MUB::FChangeListCid(long cid, long lwOld, PSTN pstnOld, long lwNew,
	PSTN pstnNew)
{
	AssertThis(0);
	AssertNilOrPo(pstnOld, 0);
	AssertNilOrPo(pstnNew, 0);
	long imlst, ilw, cch, lw;
	MLST mlst;
	SZ sz;
	bool fRet = fTrue;

	if (pvNil == _pglmlst)
		return fTrue;

	for (imlst = 0; imlst < _pglmlst->IvMac(); imlst++)
		{
		_pglmlst->Get(imlst, &mlst);
		if (cid != mlst.cid || pvNil == mlst.pgllw)
			continue;

		for (ilw = 0; ilw < mlst.pgllw->IvMac(); ilw++)
			{
			if (pvNil == pstnOld)
				{
				mlst.pgllw->Get(ilw, &lw);
				if (lw != lwOld)
					continue;
				}
			else
				{
				cch = GetMenuString(mlst.hmenu, mlst.imniBase + ilw,
					sz, kcchMaxSz, MF_BYPOSITION);
				if (!pstnOld->FEqualRgch(sz, cch))
					continue;
				}
			if (pvNil != pstnNew)
				{
				//change the string
				fRet = ModifyMenu(mlst.hmenu, mlst.imniBase + ilw,
					MF_BYPOSITION | MF_STRING, mlst.wcidList + ilw,
					pstnNew->Psz()) && fRet;
				}
			mlst.pgllw->Put(ilw, &lwNew);
			}
		}

	return fRet;
}


/***************************************************************************
	Fill in the CMD structure for the given wcid.
***************************************************************************/
bool MUB::_FGetCmdForWcid(long wcid, PCMD pcmd)
{
	AssertVarMem(pcmd);
	MLST mlst;

	ClearPb(pcmd, size(*pcmd));
	if (wcid >= wcidListBase && _FFindMlst(wcid, &mlst))
		{
		long lw, cch;
		SZ sz;
		STN stn;

		mlst.pgllw->Get(wcid - mlst.wcidList, &lw);
		cch = GetMenuString(mlst.hmenu, mlst.imniBase + wcid - mlst.wcidList,
			sz, kcchMaxSz, MF_BYPOSITION);
		stn = sz;
		if (cch == 0 || (pcmd->pgg = GG::PggNew(0, 1, stn.CbData())) == pvNil)
			return fFalse;
		AssertDo(pcmd->pgg->FInsert(0, stn.CbData(), pvNil), 0);
		stn.GetData(pcmd->pgg->PvLock(0));
		pcmd->pgg->Unlock();
		pcmd->cid = mlst.cid;
		pcmd->rglw[0] = lw;
		}
	else
		pcmd->cid = wcid;
	return fTrue;
}


/***************************************************************************
	See if the given item is in a list.
***************************************************************************/
bool MUB::_FFindMlst(long wcid, MLST *pmlst, long *pimlst)
{
	long imlst;
	MLST mlst;

	if (pvNil == _pglmlst)
		return fFalse;

	for (imlst = _pglmlst->IvMac(); imlst-- > 0; )
		{
		_pglmlst->Get(imlst, &mlst);
		if (wcid < mlst.wcidList || pvNil == mlst.pgllw)
			continue;
		if (wcid < mlst.pgllw->IvMac() + mlst.wcidList)
			{
			if (pvNil != pmlst)
				*pmlst = mlst;
			if (pvNil != pimlst)
				*pimlst = imlst;
			return fTrue;
			}
		}
	TrashVar(pmlst);
	TrashVar(pimlst);
	return fFalse;
}


/***************************************************************************
	If the menu bar has a font list item or other list item, do the right
	thing.
***************************************************************************/
bool MUB::_FInitLists(void)
{
	long imnu, imni, cmni, cmnu;
	HMENU hmenu;
	long cid;
	MLST mlst;
	SZ sz;
	STN stn;
	long onn;
	long wcidList = wcidListBase;

	cmnu = GetMenuItemCount(_hmenu);
	Assert(cmnu == _cmnu, "bad _cmnu");

	for (imnu = 0; imnu < cmnu; imnu++)
		{
		if ((hmenu = GetSubMenu(_hmenu, imnu)) == hNil)
			continue;

		for (cmni = GetMenuItemCount(hmenu), imni = 0; imni < cmni; imni++)
			{
			if ((cid = GetMenuItemID(hmenu, imni)) == cidNil)
				continue;

			if (1 != GetMenuString(hmenu, imni, sz, kcchMaxSz,
					MF_BYPOSITION))
				{
				continue;
				}
			switch (sz[0])
				{
			default:
				break;

			case kchFontList:
				//insert all the fonts
				mlst.fSeparator = (0 < imni);
				if (!mlst.fSeparator)
					{
					if (!DeleteMenu(hmenu, imni, MF_BYPOSITION))
						return fFalse;
					imni--;
					cmni--;
					}
				else if (!ModifyMenu(hmenu, imni, MF_BYPOSITION | MF_SEPARATOR,
						cidNil, pvNil))
					{
					return fFalse;
					}
				mlst.imniBase = imni + 1;
				mlst.hmenu = hmenu;
				mlst.wcidList = wcidList;
				wcidList += dwcidList;
				mlst.cid = cid;
				if (pvNil == (mlst.pgllw = GL::PglNew(size(long), vntl.OnnMac())))
					return fFalse;

				for (onn = 0; onn < vntl.OnnMac(); onn++)
					{
					vntl.GetStn(onn, &stn);
					if (!mlst.pgllw->FPush(&onn) || !InsertMenu(hmenu, ++imni,
							MF_BYPOSITION | MF_STRING, mlst.wcidList + onn,
							stn.Psz()))
						{
						ReleasePpo(&mlst.pgllw);
						return fFalse;
						}
					cmni++;
					}
				goto LInsertMlst;

			case kchList:
				mlst.hmenu = hmenu;
				mlst.imniBase = imni;
				mlst.wcidList = wcidList;
				wcidList += dwcidList;
				mlst.cid = cid;
				mlst.fSeparator = fFalse;
				mlst.pgllw = pvNil;
				if (!DeleteMenu(hmenu, imni, MF_BYPOSITION))
					return fFalse;
				imni--;
				cmni--;
LInsertMlst:
				if (pvNil == _pglmlst &&
						pvNil == (_pglmlst = GL::PglNew(size(MLST), 1)) ||
					!_pglmlst->FPush(&mlst))
					{
					ReleasePpo(&mlst.pgllw);
					return fFalse;
					}
				break;
				}
			}
		}

	return fTrue;
}


#ifdef DEBUG
/***************************************************************************
	Mark mem used by the menu bar.
***************************************************************************/
void MUB::MarkMem(void)
{
	AssertThis(0);
	long imlst;
	MLST mlst;

	MUB_PAR::MarkMem();
	if (pvNil == _pglmlst)
		return;

	MarkMemObj(_pglmlst);
	for (imlst = _pglmlst->IvMac(); imlst-- > 0; )
		{
		_pglmlst->Get(imlst, &mlst);
		MarkMemObj(mlst.pgllw);
		}
}
#endif //DEBUG

