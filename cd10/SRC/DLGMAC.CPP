/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Mac standard dialogs.

***************************************************************************/
#include "frame.h"
ASSERTNAME


//REVIEW shonk: implement combo items.

/***************************************************************************
	Read the dialog resource and construct the GGDIT.
***************************************************************************/
bool DLG::_FInit(void)
{
	HN hn;
	short ridDitl;
	long sit, sitLim;
	long idit;
	byte bState;
	byte *pbDitl;
	byte bType;
	long cbEntry;
	DIT dit;
	bool fAddDit;
	bool fRet = fFalse;

	if ((hn = (HN)GetResource('DLOG', (short)_rid)) == hNil)
		{
		PushErc(ercDlgCantFind);
		return fFalse;
		}

	//get the rid of the DITL resource
	ridDitl = (*(short **)hn)[9];
	if ((hn = GetResource('DITL', ridDitl)) == hNil)
		{
		PushErc(ercDlgCantFind);
		return fFalse;
		}

	//lock the DITL so it doesn't move and so it isn't purged while
	//we're looking at it.
	bState = HGetState(hn);
	HLock(hn);
	pbDitl = *(byte **)hn;


	/***********************************************************************
	This info comes from New Inside Macintosh - Toolbox Essentials,
	pages 6-152 to 6-156.

	A DITL is a group of items preceeded by a short containing the number
	of items - 1.  Each item contains:

		4 bytes of reserved info
		8 bytes for a bounding rectangle
		1 byte for the item type and enable flag

	The remaining data in the item depends on the type of item.  For
	Buttons(4), checkboxes(5), radio buttons(6), static text(8) and
	editable text items(16), the item data continues with:

		1 - 256 bytes (variable) - an st containing the name
		0 or 1 byte alignment (to a short word)

	For compiled controls(7), icons(32) and pictures(64):

		1 byte reserved
		2 bytes resource number

	For application defined items(0):

		1 byte reserved

	For Help items(1):

		1 byte size - 4 or 6 (so we don't have to use the help item type)
		2 byte help item type
		2 byte resource number
		0 or 2 bytes for an item number (2 bytes if help item type is 8,
			otherwise 0 bytes)

	For other values of the item type, we assert and bag out.
	***********************************************************************/

	//get the lim of system items (they start at 1) and grow the GGDIT
	//to a reasonable size
	sitLim = (*(short *)pbDitl) + 2;
	pbDitl += size(short);
	if (!FEnsureSpace(sitLim, size(long), fgrpNil))
		goto LFail;

	//look at the items in the DITL
	for (idit = 0, sit = 1; sit < sitLim; sit++)
		{
		//skip the reserved bytes and rectangle
		pbDitl += 12;

		//the high bit is an enble bit, so mask it off
		bType = *pbDitl & 0x7F;
		pbDitl++;

		fAddDit = fFalse;
		switch (bType)
			{
		default:
			Bug("unkown item in DITL resource");
			goto LFail;

		case btnCtrl + ctrlItem: //button
			dit.ditk = ditkButton;
			cbEntry = 0;
			fAddDit = fTrue;
			goto LSkipSt;

		case chkCtrl + ctrlItem: //check box
			dit.ditk = ditkCheckBox;
			cbEntry = size(long);
			fAddDit = fTrue;
			goto LSkipSt;

		case radCtrl + ctrlItem: //radio button
			//if the last item added was a radio group, add this button to
			//the group, otherwise create a new radio group
			if (idit > 0)
				{
				GetDit(idit - 1, &dit);
				if (dit.ditk == ditkRadioGroup && dit.sitLim == sit)
					{
					dit.sitLim++;
					PutDit(idit - 1, &dit);
					goto LSkipSt;
					}
				}
			dit.ditk = ditkRadioGroup;
			cbEntry = size(long);
			fAddDit = fTrue;
			goto LSkipSt;

		case statText: //static text item
			goto LSkipSt;

		case editText: //edit item
			dit.ditk = ditkEditText;
			cbEntry = kcbMaxStz - kcchMaxStz;
			fAddDit = fTrue;
LSkipSt:
			pbDitl += CbSt((achar *)pbDitl);
			if ((long)pbDitl & 1)
				pbDitl++;
			break;

		case 7: //control
		case iconItem: //icon
		case picItem: //picture
			pbDitl += 3;
			break;

		case userItem: //app defined
			pbDitl += 1;
			break;

		case 1: //help item
			pbDitl += CbSt((achar *)pbDitl);
			break;
			}

		if (fAddDit)
			{
			Assert(cbEntry >= 0, 0);
			dit.sitMin = sit;
			dit.sitLim = sit + 1;
			if (!FInsert(idit, cbEntry, pvNil, &dit))
				goto LFail;

			//zero the extra data
			if (cbEntry > 0)
				ClearPb(QvGet(idit), cbEntry);
			idit++;
			}
		}

	FEnsureSpace(0, 0, fgrpShrink);
	fRet = fTrue;

LFail:
	HSetState(hn, bState);
	if (!fRet)
		PushErc(ercDlgOom);
	return fRet;
}


/***************************************************************************
	Actually put up the dialog and don't return until it comes down.
	Returns the idit that dismissed the dialog.  Returns ivNil on failure.
***************************************************************************/
long DLG::IditDo(long iditFocus)
{
	HDLG hdlg;
	DIT dit;
	short swHit;
	long idit = ivNil;

	if ((hdlg = GetNewDialog((short)_rid, pvNil, (PPRT)-1L)) == pvNil)
		{
		PushErc(ercDlgOom);
		goto LDone;
		}

	if (pvNil == (_pgob = NewObj GOB(khidDialog)))
		{
		PushErc(ercDlgOom);
		goto LDone;
		}

	if (!_pgob->FAttachHwnd((HWND)hdlg))
		{
		PushErc(ercDlgOom);
		goto LDone;
		}

	SetValues(0, IvMac());
	ShowWindow(hdlg);
	if (ivNil != iditFocus)
		SelectDit(iditFocus);
	for (;;)
		{
		ModalDialog(pvNil, &swHit);
		if ((idit = IditFromSit((long)swHit)) == ivNil)
			continue;

		GetDit(idit, &dit);
		switch (dit.ditk)
			{
		default:
			continue;

		case ditkButton:
			break;

		case ditkCheckBox:
			//invert it
			_InvertCheckBox(idit);
			break;

		case ditkRadioGroup:
			//set the value to swHit - dit.sitMin
			_SetRadioGroup(idit, (long)swHit - dit.sitMin);
			break;
			}

		if (_FDitChange(&idit))
			break;
		}

LDone:
	if (hdlg != hNil)
		{
		if (_pgob != pvNil)
			{
			if (idit != ivNil && !FGetValues(0, IvMac()))
				{
				PushErc(ercDlgCantGetArgs);
				idit = ivNil;
				}
			_pgob->FAttachHwnd(hNil);
			_pgob->Release();
			_pgob = pvNil;
			}
		DisposeDialog(hdlg);
		}

	return idit;
}


/***************************************************************************
	Get the value of a radio group.
***************************************************************************/
long DLG::_LwGetRadioGroup(long idit)
{
	HDLG hdlg;
	DIT dit;
	short sitk;
	HCTL hctl;
	RCS rcs;
	long sit;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkRadioGroup, "not a radio group!");

	for (sit = dit.sitMin; sit < dit.sitLim; sit++)
		{
		GetDialogItem(hdlg, (short)sit, &sitk, (HN *)&hctl, &rcs);
		Assert(sitk == (radCtrl + ctrlItem), "not a radio button!");
		if (GetCtlValue(hctl))
			return sit - dit.sitMin;
		}
	Bug("no radio button set");
	return dit.sitLim - dit.sitMin;
}


/***************************************************************************
	Change a radio group value.
***************************************************************************/
void DLG::_SetRadioGroup(long idit, long lw)
{
	HDLG hdlg;
	DIT dit;
	short sitk;
	HCTL hctl;
	RCS rcs;
	long sit;
	short swT;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkRadioGroup, "not a radio group!");
	AssertIn(lw, 0, dit.sitLim - dit.sitMin);

	GNV gnv(_pgob);
	gnv.Set();
	for (sit = dit.sitMin; sit < dit.sitLim; sit++)
		{
		GetDialogItem(hdlg, (short)sit, &sitk, (HN *)&hctl, &rcs);
		Assert(sitk == (radCtrl + ctrlItem), "not a radio button!");
		swT = GetCtlValue(hctl);

		//set the value
		if (swT)
			{
			if (sit - dit.sitMin != lw)
				SetCtlValue(hctl, 0);
			}
		else
			{
			if (sit - dit.sitMin == lw)
				SetCtlValue(hctl, 1);
			}
		}
	gnv.Restore();
}


/***************************************************************************
	Returns the current value of a check box.
***************************************************************************/
bool DLG::_FGetCheckBox(long idit)
{
	HDLG hdlg;
	DIT dit;
	short sitk;
	HCTL hctl;
	RCS rcs;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkCheckBox, "not a check box!");
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on check box");

	GetDialogItem(hdlg, (short)dit.sitMin, &sitk, (HN *)&hctl, &rcs);
	Assert(sitk == (chkCtrl + ctrlItem), "not a check box!");
	return FPure(GetCtlValue(hctl));
}


/***************************************************************************
	Invert the value of a check box.
***************************************************************************/
void DLG::_InvertCheckBox(long idit)
{
	_SetCheckBox(idit, !_FGetCheckBox(idit));
}


/***************************************************************************
	Set the value of a check box.
***************************************************************************/
void DLG::_SetCheckBox(long idit, bool fOn)
{
	HDLG hdlg;
	DIT dit;
	short sitk;
	HCTL hctl;
	RCS rcs;
	short swT;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkCheckBox, "not a check box!");
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on check box");

	GNV gnv(_pgob);
	gnv.Set();
	GetDialogItem(hdlg, (short)dit.sitMin, &sitk, (HN *)&hctl, &rcs);
	Assert(sitk == (chkCtrl + ctrlItem), "not a check box!");
	swT = GetCtlValue(hctl);
	if (FPure(swT) != FPure(fOn))
		SetCtlValue(hctl, FPure(fOn));
	gnv.Restore();
}


/***************************************************************************
	Get the text from an edit control.
***************************************************************************/
void DLG::_GetEditText(long idit, PSTZ pstz)
{
	HDLG hdlg;
	DIT dit;
	short sitk;
	HN hn;
	RCS rcs;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkEditText, "not edit item!");
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on edit item");

	GetDialogItem(hdlg, (short)dit.sitMin, &sitk, &hn, &rcs);
	AssertVar(sitk == editText, "not an edit item!", &sitk);
	GetDialogItemText(hn, (byte *)pstz);
	StToStz(pstz);
}


/***************************************************************************
	Set the text in an edit control.
***************************************************************************/
void DLG::_SetEditText(long idit, PSTZ pstz)
{
	HDLG hdlg;
	DIT dit;
	short sitk;
	HN hn;
	RCS rcs;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkEditText, "not edit item!");
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on edit item");

	GNV gnv(_pgob);
	gnv.Set();
	GetDialogItem(hdlg, (short)dit.sitMin, &sitk, &hn, &rcs);
	AssertVar(sitk == editText, "not an edit item!", &sitk);
	SetDialogItemText(hn, (byte *)pstz);
	gnv.Restore();
}


/***************************************************************************
	Make the given item the "focused" item and select its contents.  The
	item should be a text item.
***************************************************************************/
void DLG::SelectDit(long idit)
{
	HDLG hdlg;
	DIT dit;

	if (hNil == (hdlg = (HDLG)_pgob->Hwnd()))
		goto LBug;

	GetDit(idit, &dit);
	if (dit.ditk != ditkEditText)
		{
LBug:
		Bug("bad call to DLG::SelectDit");
		return;
		}
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on edit item");
	SelIText(hdlg, (short)dit.sitMin, 0, kswMax);
}

