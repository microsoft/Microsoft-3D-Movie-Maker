/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Standard dialogs.

***************************************************************************/
#include "frame.h"
ASSERTNAME


//dialog init structure
struct DLGI
	{
	PDLG pdlg;
	long iditFocus;
	};

achar _szDlgProp[] = PszLit("DLG");

/***************************************************************************
	Read the dialog resource and construct the GGDIT.
	The dialog resource consists of:

		DLGTEMPLATE structure
		(0xFFFF, 2-byte value) or unicode string specifying the menu
		(0xFFFF, 2-byte value) or unicode string specifying the class
		unicode string for the title

	The following fields only exist if the dialog has the DS_SETFONT style:

		2-byte value specifying the font size
		unicode string specifying the font

	Next comes:

		0-2 bytes align to dword

	For each dialog item the resource contains:

		DLGITEMTEMPLATE structure
		(0xFFFF, 2-byte value) or unicode string specifying the class
		(0xFFFF, 2-byte value) or unicode string specifying the title
***************************************************************************/
bool DLG::_FInit(void)
{
	HN hn;
	long cbEntry;
	long idit, csit;
	bool fAddDit;
	DIT dit;
	short *psw;
	short swClass;
	DLGTEMPLATE dtm;
	DLGITEMTEMPLATE ditm;
	bool fRet = fFalse;

	if ((hn = (HN)FindResource(vwig.hinst, MIR(_rid), RT_DIALOG)) == hNil ||
			(hn = (HN)LoadResource(vwig.hinst, (HRSRC)hn)) == hNil ||
			(psw = (short *)LockResource(hn)) == pvNil)
		{
		PushErc(ercDlgCantFind);
		return fFalse;
		}

	//get and skip the dtm
	dtm = *(DLGTEMPLATE *)psw;
	psw = (short *)PvAddBv(psw, size(dtm));

	//get the number of items and ensure space in the GGDIT
	Assert(dtm.cdit > 0, "no items in this dialog");
	if (!FEnsureSpace(dtm.cdit, size(long), fgrpNil))
		goto LFail;

	//skip over the menu field
	if (*psw == -1)
		psw += 2;
	else
		{
		while (*psw++ != 0)
			;
		}

	//skip over the class field
	if (*psw == -1)
		psw += 2;
	else
		{
		while (*psw++ != 0)
			;
		}

	//skip over the title
	while (*psw++ != 0)
		;

	if (dtm.style & DS_SETFONT)
		{
		//skip over the point size
		psw++;

		//skip over the font name
		while (*psw++ != 0)
			;
		}

	//look at the items
	for (csit = dtm.cdit, idit = 0; csit > 0; csit--)
		{
		//align to dword
		if ((long)psw & 2)
			psw++;

		//get and skip the ditm
		ditm = *(DLGITEMTEMPLATE *)psw;
		psw = (short *)PvAddBv(psw, size(ditm));

		//get and skip the class
		if (*psw == -1)
			{
			swClass = psw[1];
			psw += 2;
			}
		else
			{
			swClass = 0;
			while (*psw++ != 0)
				;
			}

		//skip the title
		if (*psw == -1)
			psw += 2;
		else
			{
			while (*psw++ != 0)
				;
			}

		//the next word is a size of extra stuff
		psw = (short *)PvAddBv(psw, psw[0] + size(short));

		//We should be at the end of this item (except for possible padding).
		//Now figure out what to do with the item.

		fAddDit = fTrue;
		switch (swClass)
			{
		default:
			fAddDit = fFalse;
			break;

		case 0x0080: //button, radio button or check box
			switch (ditm.style & 0x000F)
				{
			case 0:
			case 1:
				//button
				dit.ditk = ditkButton;
				cbEntry = 0;
				break;

			case 2:
			case 3:
				//check box
				dit.ditk = ditkCheckBox;
				cbEntry = size(long);
				break;

			case 4:
			case 9:
				//radio button
				//if the radio button has the WS_GROUP style or the last
				//dit is not a radio group, start a new group.
				if (!(ditm.style & WS_GROUP) && idit > 0)
					{
					GetDit(idit - 1, &dit);
					if (dit.ditk == ditkRadioGroup && dit.sitLim == ditm.id)
						{
						dit.sitLim++;
						PutDit(idit - 1, &dit);
						fAddDit = fFalse;
						break;
						}
					}

				//new group
				dit.ditk = ditkRadioGroup;
				cbEntry = size(long);
				break;

			default:
				fAddDit = fFalse;
				break;
				}
			break;

		case 0x0081: //edit item
			dit.ditk = ditkEditText;
			cbEntry = 0;
			break;

		case 0x0085: //combo item
			dit.ditk = ditkCombo;
			cbEntry = 0;
			break;
			}

		if (fAddDit)
			{
			Assert(cbEntry >= 0, 0);
			dit.sitMin = ditm.id;
			dit.sitLim = dit.sitMin + 1;
			if (!FInsert(idit, cbEntry, pvNil, &dit))
				goto LFail;

			//zero the extra data
			if (cbEntry > 0)
				ClearPb(QvGet(idit), cbEntry);
			idit++;
			}
		}

	Assert(idit > 0, "no dits in this dialog");
	FEnsureSpace(0, 0, fgrpShrink);
	fRet = fTrue;

LFail:
	if (!fRet)
		PushErc(ercDlgOom);

	return fRet;
}


/***************************************************************************
	Windows dialog proc.
***************************************************************************/
BOOL CALLBACK _FDlgCore(HWND hdlg, UINT msg, WPARAM w, LPARAM lw)
{
	PDLG pdlg;
	DIT dit;
	long idit;
	RC rcDlg;
	RC rcDsp;

	//this may return nil
	pdlg = (PDLG)GetProp(hdlg, _szDlgProp);

	switch (msg)
		{
	case WM_SYSCOMMAND:
		if (w == SC_SCREENSAVE && pvNil != vpappb &&
				!vpappb->FAllowScreenSaver())
			{
			return fTrue;
			}
		break;

	case WM_INITDIALOG:
		DLGI *pdlgi;
		RCS rcs;

		//the pdlgi should be in the lParam
		pdlgi = (DLGI *)lw;
		pdlg = pdlgi->pdlg;
		AssertPo(pdlg, 0);

		//set the DLG property so we can find the pdlg easily
		if (!SetProp(hdlg, _szDlgProp, (HANDLE)pdlg))
			goto LFail;

		//create a timer so we can do idle processing
		if (SetTimer(hdlg, (uint)hdlg, 10, pvNil) == 0)
			goto LFail;

		//create a container gob and attach the hdlg
		pdlg->_pgob = NewObj GOB(khidDialog);
		if (pdlg->_pgob == pvNil)
			goto LFail;
		if (!pdlg->_pgob->FAttachHwnd((HWND)hdlg))
			{
LFail:
			PushErc(ercDlgOom);
			idit = ivNil;
			goto LEndDialog;
			}

		//set the dialog values
		pdlg->SetValues(0, pdlg->IvMac());
		if (ivNil != pdlgi->iditFocus)
			pdlg->SelectDit(pdlgi->iditFocus);

		GetWindowRect(hdlg, &rcs);
		rcDlg = rcs;

		rcDsp.Set(0, 0, GetSystemMetrics(SM_CXSCREEN),
			GetSystemMetrics(SM_CYSCREEN));
		if (hNil != vwig.hwndApp)
			{
			RC rcApp;

			GetWindowRect(vwig.hwndApp, &rcs);
			rcApp = rcs;
			rcDlg.CenterOnRc(&rcApp);
			}
		else
			rcDlg.CenterOnRc(&rcDsp);

		rcDlg.PinToRc(&rcDsp);
		MoveWindow(hdlg, rcDlg.xpLeft, rcDlg.ypTop,
			rcDlg.Dxp(), rcDlg.Dyp(), fFalse);

		return ivNil == pdlgi->iditFocus;

	case WM_TIMER:
		if (pvNil == pdlg)
			break;

		idit = ivNil;
		if (pdlg->_FDitChange(&idit))
			goto LEndDialog;
		break;

	case WM_COMMAND:
		if (pvNil == pdlg)
			break;

		if ((idit = pdlg->IditFromSit(GET_WM_COMMAND_ID(w, lw))) == ivNil)
			break;

		pdlg->GetDit(idit, &dit);
		switch (dit.ditk)
			{
		default:
			return fFalse;

		case ditkEditText:
			if (SwHigh(w) != EN_CHANGE)
				return fFalse;
			break;

		case ditkButton:
		case ditkCheckBox:
		case ditkRadioGroup:
			break;
			}

		if (pdlg->_FDitChange(&idit))
			{
LEndDialog:
			if (pdlg->_pgob != pvNil)
				{
				if (idit != ivNil && !pdlg->FGetValues(0, pdlg->IvMac()))
					idit = ivNil;
				pdlg->_pgob->FAttachHwnd(hNil);
				pdlg->_pgob->Release();
				pdlg->_pgob = pvNil;
				}
			else
				idit = ivNil;

			// remove the pdlg property and kill the timer
			RemoveProp(hdlg, _szDlgProp);
			KillTimer(hdlg, (uint)hdlg);

			EndDialog(hdlg, idit);
			return fTrue;
			}

		break;
		}

	return fFalse;
}


/***************************************************************************
	Actually put up the dialog and don't return until it comes down.
	Returns the idit that dismissed the dialog.  Returns ivNil on failure.
***************************************************************************/
long DLG::IditDo(long iditFocus)
{
	long idit;
	DLGI dlgi;

	dlgi.pdlg = this;
	dlgi.iditFocus = iditFocus;
	idit = DialogBoxParam(vwig.hinst, MIR(_rid), vwig.hwndApp,
		&_FDlgCore, (long)&dlgi);

	return idit;
}


/***************************************************************************
	Make the given item the "focused" item and select its contents.  The
	item should be a text item or combo item.
***************************************************************************/
void DLG::SelectDit(long idit)
{
	HDLG hdlg;
	DIT dit;

	if (pvNil == _pgob || hNil == (hdlg = (HDLG)_pgob->Hwnd()))
		goto LBug;

	GetDit(idit, &dit);
	if (dit.ditk != ditkEditText && dit.ditk != ditkCombo)
		{
LBug:
		Bug("bad call to DLG::SelectDit");
		return;
		}
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on edit item");
	SetFocus(GetDlgItem(hdlg, dit.sitMin));
	SendDlgItemMessage(hdlg, dit.sitMin, EM_SETSEL, GET_EM_SETSEL_MPS(0, -1));
}


/***************************************************************************
	Get the value of a radio group.
***************************************************************************/
long DLG::_LwGetRadioGroup(long idit)
{
	HDLG hdlg;
	DIT dit;
	long sit;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkRadioGroup, "not a radio group!");

	for (sit = dit.sitMin; sit < dit.sitLim; sit++)
		{
		if (IsDlgButtonChecked(hdlg, sit))
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

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkRadioGroup, "not a radio group!");
	AssertIn(lw, 0, dit.sitLim - dit.sitMin);

	CheckRadioButton(hdlg, dit.sitMin, dit.sitLim - 1, dit.sitMin + lw);
}


/***************************************************************************
	Returns the current value of a check box.
***************************************************************************/
bool DLG::_FGetCheckBox(long idit)
{
	HDLG hdlg;
	DIT dit;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkCheckBox, "not a check box!");
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on check box");

	return FPure(IsDlgButtonChecked(hdlg, dit.sitMin));
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

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkCheckBox, "not a check box!");
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on check box");

	CheckDlgButton(hdlg, dit.sitMin, FPure(fOn));
}


/***************************************************************************
	Get the text from an edit control or combo.
***************************************************************************/
void DLG::_GetEditText(long idit, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	HDLG hdlg;
	DIT dit;
	SZ sz;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkEditText || dit.ditk == ditkCombo,
		"not edit item or combo!");
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on item");

	GetDlgItemText(hdlg, dit.sitMin, sz, kcchMaxSz);
	*pstn = sz;
}


/***************************************************************************
	Set the text in an edit control or combo.
***************************************************************************/
void DLG::_SetEditText(long idit, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	HDLG hdlg;
	DIT dit;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkEditText || dit.ditk == ditkCombo,
		"not edit item or combo!");
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on item");

	SetDlgItemText(hdlg, dit.sitMin, pstn->Psz());
}


/***************************************************************************
	Add a string to a combo item.
***************************************************************************/
bool DLG::_FAddToList(long idit, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	HDLG hdlg;
	HWND hwndCombo;
	DIT dit;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkCombo, "not combo!");
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on item");

	if (hNil == (hwndCombo = GetDlgItem(hdlg, dit.sitMin)))
		{
		Warn("Couldn't get combo hwnd");
		return fFalse;
		}

	return 0 <= (long)SendMessage(hwndCombo, CB_ADDSTRING, 0,
		(LPARAM)pstn->Psz());
}


/***************************************************************************
	Empty the list portion of the combo item.
***************************************************************************/
void DLG::_ClearList(long idit)
{
	AssertThis(0);
	HDLG hdlg;
	HWND hwndCombo;
	DIT dit;

	GetDit(idit, &dit);
	hdlg = (HDLG)_pgob->Hwnd();
	Assert(hdlg != hNil, "no dialog!");
	Assert(dit.ditk == ditkCombo, "not combo!");
	Assert(dit.sitLim == dit.sitMin + 1, "wrong lim on item");

	if (hNil == (hwndCombo = GetDlgItem(hdlg, dit.sitMin)))
		{
		Warn("Couldn't get combo hwnd");
		return;
		}

	SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0);
}


