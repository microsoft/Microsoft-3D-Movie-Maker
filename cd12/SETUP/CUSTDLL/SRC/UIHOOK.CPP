/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

////////////////////////////////////////////////////////////////////////////////
//	File:			SAMPHOOK.CPP
//	Purpose:		Sample hook routines for replacing the standard acme ui
////////////////////////////////////////////////////////////////////////////////


#include "ui-hook.h"


#define ilFont 0
#define ilBarRange 4
#define	ilBarPos 8
#define BAR_SETPOS		WM_USER
#define BAR_SETRANGE	WM_USER + 1
#define BAR_DELTAPOS	WM_USER + 2

typedef struct
	{
	BOOL *pfAbort;
	HWND hdlgGaugePrev;
	} GD, *PGD;



static long _cactAttach;
static WNDPROC _lpfnButtonWndProc;
static int _idcOODS;
static HWND _hdlgOODS;
static TAB _rgtabCombo1[] = { { 19, ttabLeft }, { 9, ttabRight }, { 9, ttabRight }, { 9, ttabRight } };
static TAB _rgtabList1[] = { { 38, ttabLeft }, { 9, ttabRight } };
static int _idcOptions;
static HWND _hdlgOptions;
static TAB _rgtabOptions[] = { { 26, ttabLeft }, { 9, ttabRight } };
static CHAR _szClassGizmo[] = "sample_hook_gizmo";
static CHAR _szClassCText[] = "sample_hook_ctext";
static HWND _hdlgGaugeCur;

  // sendak addition - holds PID until we need it for final dialog
static CHAR _szPID[24];   // 12345-123-1234567-12345


BOOL CALLBACK GenericHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfirmExitHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GetCDInfoHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfirmCDInfoHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SerializationHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SerialChangeHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AlreadyUsedHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DirectoryDialogHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ProgmanGroupHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ModulesHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK MsAppsDirHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK SharedFilesHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ConfirmServerHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK GaugeDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OODSHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AppMainHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK OptionsHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK PiracyHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK DiskLockHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK WelcomeHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RemoveSharedHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

  // sendak addition
BOOL CALLBACK ExitHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


LRESULT CALLBACK GizmoWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK CTextWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ButtonWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


HWND HwndGetDlgOwner(VOID);
VOID FetchIdcPath(HWND hdlg, int idc, LPSTR sz, LONG cb);
VOID SetIdcPath(HWND hdlg, int idc, LPSTR sz);


static VOID _InsertIdcPath(HWND hdlg, int idc, LPSTR sz);
static HWND _HwndTop(HWND hwnd);
static VOID _InsertIdcText(HWND hdlg, int idc, LPSTR sz);
static VOID _InsertIdcVal(HWND hdlg, int idc, LONG l);
static DWORD _DwClicksModelessStop(VOID);
static VOID _WaitToEndModeless(DWORD dwClicks);
static VOID _InitOptions(HWND hdlg, POPDHD popdhd, BOOL fNewDialog);
static VOID _InitOODS(HWND hdlg, PDSDHD pdsdhd, BOOL fNewDialog);
static VOID _SubclassButton(HWND hdlg, int idc);
static LONG _CkbOfCb(LONG cb);
static VOID _CentreWindow(HWND hwnd);
static BOOL _FValidCDKey(CHAR *prgch);
static BOOL _FValidOEM(CHAR *prgch);
static BOOL _FValidPidSerial(CHAR *prgch);
static BOOL _FIsDigit(CHAR ch);


SHRC DECL SHRCAskQuitDialogHookProc(PAQDHD paqdhd);	// forward


/* We don't use this - see dllmain.c

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		called when processes attach/detach from the DLL. we just use it to
//		get the instance handle, and the init/term the 3d dialog crap
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL WINAPI DllEntryPoint(HINSTANCE hinstCur, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
		{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
		if (0 == _cactAttach)
			{
			hinst = hinstCur;
			}
		_cactAttach++;
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_DETACH:
		_cactAttach--;
		if (0 == _cactAttach)
			{
			hinst = (HINSTANCE)NULL;
			}
		break;
		}

	return TRUE;
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Generical dialog proc for the sample customized dialogs that confirms
//		exiting setup
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK ConfirmExitHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (WM_COMMAND == uMsg && IDCANCEL == LOWORD(wParam))
		{
		AQDHD aqdhd;
		aqdhd.hd.cb = sizeof aqdhd;
		aqdhd.hd.rgchHelpFile[0] = '\0';
		aqdhd.hd.dwHelpContext = 0;
		if (shrcCancel != SHRCAskQuitDialogHookProc(&aqdhd))
	    	EndDialog(hdlg, IDCANCEL);
		return TRUE;
		}
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Generic dialog proc for the sample customized dialogs
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK GenericHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
		{
	case WM_INITDIALOG:
		SetWindowLong(hdlg, DWL_USER, lParam);
		_CentreWindow(hdlg);
		Ctl3dSubclassDlg(hdlg, CTL3D_ALL);
		return TRUE;

	case WM_COMMAND:
	    switch (LOWORD (wParam))
			{
		case IDOK:
		case IDCANCEL:
		case IDC_KEEP:
		case IDC_NONE:
		case IDC_REMOVE:
		case IDC_REINSTALL:
		case IDC_REMOVEALL:
		case IDC_CHANGE:
		case IDC_CREATE:
//		case IDC_IGNORE:
		case IDC_BUTTON1:
//		case IDC_BUTTON2:
//		case IDC_BUTTON3:
//		case IDC_BUTTON4:
		    EndDialog(hdlg, LOWORD (wParam));
			return TRUE;
		case IDC_BUTTONHELP:
			PHD phd;
			HWND hwndT;
			phd = (PHD)GetWindowLong(hdlg, DWL_USER);
			if ((HWND)NULL == (hwndT = GetWindow(hdlg, GW_OWNER)))
				hwndT = GetParent(hdlg);
			WinHelp(hwndT, phd->rgchHelpFile, HELP_CONTEXT,
				phd->dwHelpContext);
			break;
			}
		break;

	case WM_CTLCOLORMSGBOX:
	case WM_CTLCOLOREDIT:
	case WM_CTLCOLORLISTBOX:
	case WM_CTLCOLORBTN:
	case WM_CTLCOLORDLG:
	case WM_CTLCOLORSCROLLBAR:
	case WM_CTLCOLORSTATIC:
		return (BOOL)Ctl3dCtlColorEx(uMsg, wParam, lParam);

	case WM_NCACTIVATE:
		Ctl3dDlgFramePaint(hdlg, uMsg, wParam, lParam);
		return (BOOL)wParam;

	case WM_NCPAINT:
		Ctl3dDlgFramePaint(hdlg, uMsg, wParam, lParam);
		return TRUE;
		}
	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample customized welcome hook dialog
//	Arguments:
//		pwdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
//		shrcRemoveAll - in maintenance mode, remove all button pushed
//		shrcReinstall - in maintenance mode, reinstall button pushed
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCWelcomeDialogHookProc(PWDHD pwdhd)
{
	HWND hwndT;
	WORD idd;
	int idc;

	if (pwdhd->hd.cb < sizeof *pwdhd)
		return shrcFail;

  if(!pwdhd->fMaint)                  // If we're not in maintenance mode
    return shrcOk;                    // don't show welcome dialog and continue

	//idd = pwdhd->fMaint ? IDD_CONSACMEWELCOMEMAINT: IDD_CONSACMEWELCOME;
  idd = IDD_CONSACMEWELCOMEMAINT;

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(idd), hwndT,
		(DLGPROC)WelcomeHookDlgProc, (LPARAM)pwdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
	case IDC_REINSTALL:
		if (!pwdhd->fMaint)
			return shrcFail;
    _szPID[5]='\1';         // If we're reinstalling, flag condition for exit dialog
		return shrcReinstall;
	case IDC_REMOVEALL:
		if (!pwdhd->fMaint)
			return shrcFail;
    _szPID[5]='\0';         // If we're removing the product, flag condition for exit dialog
		return shrcRemoveAll;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized CDInfo dialogs
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK WelcomeHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  /*
    switch (uMsg)
		{
	case WM_COMMAND:
	    switch (LOWORD (wParam))
			{
		case IDC_REMOVEALL:
			int idRet;
			if (FMsgBox(hdlg, IDS_REMOVEAPP, MB_YESNO, &idRet) &&
				idRet != IDYES)
				return FALSE;
			break;
			}
		break;
		}
  */
	return ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//
//  "Setup has detected that the following application(s) are running:"
//
//	Purpose:
//		Puts up the sample customized modules hook dialog
//	Arguments:
//		pmudhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCModulesDialogHookProc (PMUDHD pmudhd)
{
	HWND hwndT;
	int idc;

	if (pmudhd->hd.cb < sizeof *pmudhd)
		return shrcFail;
	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEMODULES), hwndT,
		(DLGPROC)ModulesHookDlgProc, (LPARAM)pmudhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized CDInfo dialogs
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK ModulesHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PMUDHD pmudhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		pmudhd = (PMUDHD)lParam;
		_InsertIdcText(hdlg, IDC_TEXT1, pmudhd->rgchModuleList);
		break;
		}
	return ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample customized CD name/org hook dialog
//	Arguments:
//		pnodhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////

SHRC DECL SHRCGetCDInfoDialogHookProc (PCDDHD pcddhd)
{
	HWND hwndT;
	WORD idd;
	int idc;

	if (pcddhd->hd.cb < sizeof *pcddhd)
		return shrcFail;
	switch (pcddhd->cddhm)
		{
	default:
		return shrcFail;
	case cddhmNameOrg:
		idd = IDD_CONSACMECDNAMEORG;
		break;
	case cddhmName:
		idd = IDD_CONSACMECDNAME;
		break;
	case cddhmOrg:
		idd = IDD_CONSACMECDORG;
		break;
		}

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(idd), hwndT,
		(DLGPROC)GetCDInfoHookDlgProc, (LPARAM)pcddhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}

}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized CDInfo dialogs
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK GetCDInfoHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PCDDHD pcddhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		SendDlgItemMessage(hdlg, IDC_EDIT1, EM_LIMITTEXT, 52, 0);
		pcddhd = (PCDDHD)lParam;
		switch (pcddhd->cddhm)
			{
		case cddhmNameOrg:
			_InsertIdcText(hdlg, IDC_EDIT1, pcddhd->rgchName);
			_InsertIdcText(hdlg, IDC_EDIT2, pcddhd->rgchOrg);
			SendDlgItemMessage(hdlg, IDC_EDIT2, EM_LIMITTEXT, 52, 0);
			break;
		case cddhmName:
			_InsertIdcText(hdlg, IDC_EDIT1, pcddhd->rgchName);
			break;
		case cddhmOrg:
			_InsertIdcText(hdlg, IDC_EDIT1, pcddhd->rgchOrg);
			break;
			}
		break;
	case WM_COMMAND:
	    switch (LOWORD (wParam))
			{
		case IDOK:
			pcddhd = (PCDDHD)GetWindowLong(hdlg, DWL_USER);

			HWND hwndT = HwndGetDlgOwner();
			WORD idd;
			switch (pcddhd->cddhm)
				{
			case cddhmNameOrg:
				idd = IDD_CONSACMECDNAMEORGCONFIRM;
				break;
			case cddhmName:
				idd = IDD_CONSACMECDNAMECONFIRM;
				break;
			case cddhmOrg:
				idd = IDD_CONSACMECDORGCONFIRM;
				break;
				}
			switch (pcddhd->cddhm)
				{
			case cddhmNameOrg:
				GetDlgItemText(hdlg, IDC_EDIT1, pcddhd->rgchName,
					sizeof pcddhd->rgchName);
				GetDlgItemText(hdlg, IDC_EDIT2, pcddhd->rgchOrg,
					sizeof pcddhd->rgchOrg);
				break;
			case cddhmName:
				GetDlgItemText(hdlg, IDC_EDIT1, pcddhd->rgchName,
					sizeof pcddhd->rgchName);
				break;
			case cddhmOrg:
				GetDlgItemText(hdlg, IDC_EDIT1, pcddhd->rgchOrg,
					sizeof pcddhd->rgchOrg);
				break;
				}
			switch (pcddhd->cddhm)
				{
			case cddhmNameOrg:
			case cddhmName:
				if ('\0' == pcddhd->rgchName[0])
					{
					int idRet;
					FMsgBox(hdlg, IDS_NAMEREQUIRED, MB_OK, &idRet);
					goto LEdit1;
					}
				break;
			case cddhmOrg:
				if ('\0' == pcddhd->rgchName[0])
					{
					int idRet;
					FMsgBox(hdlg, IDS_ORGREQUIRED, MB_OK, &idRet);
					goto LEdit1;
					}
				break;
				}
			if (DialogBoxParam(hinst, MAKEINTRESOURCE(idd), hwndT,
				(DLGPROC)ConfirmCDInfoHookDlgProc, (LPARAM)pcddhd) != IDOK)
				{
LEdit1:
				SetFocus(GetDlgItem(hdlg, IDC_EDIT1));
				SendDlgItemMessage(hdlg, IDC_EDIT1, EM_SETSEL, 0, -1);
				return FALSE;
				}
			UpdateWindow(hwndT);
			break;
			}
		break;

		}
	return ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized CDInfo confirm dialogs
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK ConfirmCDInfoHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PCDDHD pcddhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		pcddhd = (PCDDHD)lParam;
		switch (pcddhd->cddhm)
			{
		case cddhmNameOrg:
			_InsertIdcText(hdlg, IDC_TEXT1, pcddhd->rgchName);
			_InsertIdcText(hdlg, IDC_TEXT2, pcddhd->rgchOrg);
			break;
		case cddhmName:
			_InsertIdcText(hdlg, IDC_TEXT1, pcddhd->rgchName);
			break;
		case cddhmOrg:
			_InsertIdcText(hdlg, IDC_TEXT1, pcddhd->rgchOrg);
			break;
			}
		break;
		}
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample customized CD serial number hook dialog
//	Arguments:
//		psndhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCSerializationDialogHookProc(PSNDHD psndhd)
{
	if (psndhd->hd.cb < sizeof *psndhd)
		return shrcFail;

	switch (psndhd->sndhm)
		{
  	default:
	  	return shrcFail;

  	case sndhmNormal:
  	case sndhmCDKey:
  	case sndhmOEM:
        // save the PID so we can display it in the exit dialog
  		lstrcpy(_szPID, psndhd->rgchSerNum);
    	return shrcOk;
		}
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized serialization dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK SerializationHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PSNDHD psndhd;
	static UINT cPID1;
	static UINT cPID2;
	static CHAR szPID1[3+1];
	static CHAR szPID2[7+1];
	static BOOL fNoSkip;
	CHAR rgch[cbMaxSz];

    switch (uMsg)
		{
	case WM_INITDIALOG:
		psndhd = (PSNDHD)lParam;
		switch (psndhd->sndhm)
			{
		case sndhmNormal:
			_InsertIdcText(hdlg, IDC_TEXT1, psndhd->rgchSerNum);
			break;
		case sndhmCDKey:
			SendDlgItemMessage(hdlg, IDC_TEXT1, EM_LIMITTEXT, 3, 0);
			SendDlgItemMessage(hdlg, IDC_TEXT2, EM_LIMITTEXT, 7, 0);
			cPID1 = cPID2 = 0;
			fNoSkip = fFalse;
			break;
		case sndhmOEM:
			SendDlgItemMessage(hdlg, IDC_TEXT1, EM_LIMITTEXT, 5, 0);
			SendDlgItemMessage(hdlg, IDC_TEXT2, EM_LIMITTEXT, 7, 0);
			SendDlgItemMessage(hdlg, IDC_TEXT3, EM_LIMITTEXT, 5, 0);
			fNoSkip = fFalse;
			break;
			}
		break;
	case WM_COMMAND:
		psndhd = (PSNDHD)GetWindowLong(hdlg, DWL_USER);
		switch (psndhd->sndhm)
			{
		case sndhmNormal:
			break;
		case sndhmCDKey:
			switch (LOWORD(wParam))
				{
			case IDOK:
				if (cPID1 < 3)
					{
					SetFocus(GetDlgItem(hdlg, IDC_TEXT1));
					return fTrue;
					}
				if (cPID2 < 7)
					{
					SetFocus(GetDlgItem(hdlg, IDC_TEXT2));
					return fTrue;
					}
				lstrcpy(rgch, psndhd->rgchSerNum);
				MoveMemory(rgch +  6, szPID1, 3);
				MoveMemory(rgch + 10, szPID2, 7);
				if (!_FValidCDKey(rgch))
					{
					int idRet;
					SetFocus(GetDlgItem(hdlg, IDC_TEXT1));
					FMsgBox(hdlg, IDS_PIDBADCDKEY, MB_OK, &idRet);
					return fTrue;
					}
				lstrcpy(psndhd->rgchSerNum, rgch);
				break;
			case IDC_TEXT1:
				if (HIWORD(wParam) == EN_CHANGE)
					{
					BOOL fNumOk;
					GetDlgItemInt(hdlg, IDC_TEXT1, &fNumOk, fFalse);
					if (!fNumOk && GetDlgItemText(hdlg, IDC_TEXT1, rgch, 3+1))
						{
						SetDlgItemText(hdlg, IDC_TEXT1, szPID1);
						SendDlgItemMessage(hdlg, IDC_TEXT1, EM_SETSEL, (WPARAM)0, 3);
						SendDlgItemMessage(hdlg, IDC_TEXT1, EM_SETSEL, (WPARAM)-1, 3);
						}
					else
						{
						cPID1 = GetDlgItemText(hdlg, IDC_TEXT1, szPID1, 3+1);
						if (cPID1 == 3)
							SetFocus(GetDlgItem(hdlg, IDC_TEXT2));
						}
					}
				break;
			case IDC_TEXT2:
				if (HIWORD(wParam) == EN_CHANGE)
					{
					BOOL fNumOk;
					GetDlgItemInt(hdlg, IDC_TEXT2, &fNumOk, fFalse);
					if (!fNumOk && GetDlgItemText(hdlg, IDC_TEXT2, rgch, 7+1))
						{
						fNoSkip = fTrue;
						SetDlgItemText(hdlg, IDC_TEXT2, szPID2);
						SendDlgItemMessage(hdlg, IDC_TEXT2, EM_SETSEL, (WPARAM)0, 7);
						SendDlgItemMessage(hdlg, IDC_TEXT2, EM_SETSEL, (WPARAM)-1, 7);
						}
					else if (!fNoSkip)
						{
						cPID2 = GetDlgItemText(hdlg, IDC_TEXT2, szPID2, 7+1);
						if (cPID2 == 0)
							SetFocus(GetDlgItem(hdlg, IDC_TEXT1));
						if (cPID2 == 7)
							SetFocus(GetDlgItem(hdlg, IDOK));
						}
					else
						fNoSkip = fFalse;
					}
				break;
				}
			break;
		case sndhmOEM:
			switch (LOWORD(wParam))
				{
			case IDOK:
				lstrcpy(rgch, psndhd->rgchSerNum);
				if (GetDlgItemText(hdlg, IDC_TEXT1, rgch+0, 5+1) < 5)
					{
					SetFocus(GetDlgItem(hdlg, IDC_TEXT1));
					return fTrue;
					}
				rgch[5] = '-';
				if (GetDlgItemText(hdlg, IDC_TEXT2, rgch+10, 7+1) < 7)
					{
					SetFocus(GetDlgItem(hdlg, IDC_TEXT2));
					return fTrue;
					}
				rgch[17] = '-';
				if (GetDlgItemText(hdlg, IDC_TEXT3, rgch+18, 5+1) < 5)
					{
					SetFocus(GetDlgItem(hdlg, IDC_TEXT3));
					return fTrue;
					}
				if (!_FValidOEM(rgch))
					{
					int idRet;
					FMsgBox(hdlg, IDS_PIDBADOEM, MB_OK, &idRet);
					return fTrue;
					}
				lstrcpy(psndhd->rgchSerNum, rgch);
				break;

			case IDC_TEXT1:
				if (HIWORD(wParam) == EN_CHANGE
				 && GetDlgItemText(hdlg, IDC_TEXT1, rgch, 5+1) == 5)
					SetFocus(GetDlgItem(hdlg, IDC_TEXT2));
				break;

			case IDC_TEXT2:
				if (HIWORD(wParam) == EN_CHANGE)
					{
					UINT cb = GetDlgItemText(hdlg, IDC_TEXT2, rgch, 7+1);
					if (fNoSkip)
						fNoSkip = fFalse;
					else if (cb == 7)
						SetFocus(GetDlgItem(hdlg, IDC_TEXT3));
					else if (cb == 0)
						SetFocus(GetDlgItem(hdlg, IDC_TEXT1));
					else if (!_FIsDigit(*rgch))
						{
						fNoSkip = fTrue;
						SetDlgItemText(hdlg, IDC_TEXT2, "");
						}
					}
				break;
			case IDC_TEXT3:
				if (HIWORD(wParam) == EN_CHANGE)
					{
					UINT cb = GetDlgItemText(hdlg, IDC_TEXT3, rgch, 5+1);
					if (cb == 5)
						SetFocus(GetDlgItem(hdlg, IDOK));
					if (cb == 0)
						SetFocus(GetDlgItem(hdlg, IDC_TEXT2));
					}
				break;
				}
			break;
			}
		break;
		}
	return ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		determine if the pid string is a valid CD Key
//	Arguments:
//		prgch - pid string
//	Returns:
//		whether the pid string is valid
////////////////////////////////////////////////////////////////////////////////
static BOOL _FValidCDKey(CHAR *prgch)
{
	if (!_FIsDigit(prgch[6]))
		return fFalse;
	if (!_FIsDigit(prgch[7]))
		return fFalse;
	if (!_FIsDigit(prgch[8]))
		return fFalse;
	return _FValidPidSerial(prgch);
}



////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		determine if the pid string is a valid oem pid
//	Arguments:
//		prgch - pid string
//	Returns:
//		whether the pid string is valid
////////////////////////////////////////////////////////////////////////////////
static BOOL _FValidOEM(CHAR *prgch)
{
	if (!_FIsDigit(prgch[0]))
		return fFalse;
	if (!_FIsDigit(prgch[1]))
		return fFalse;
	if (!_FIsDigit(prgch[2]))
		return fFalse;
	if (!_FIsDigit(prgch[3]))
		return fFalse;
	if (!_FIsDigit(prgch[4]))
		return fFalse;
	if (!_FIsDigit(prgch[18]))
		return fFalse;
	if (!_FIsDigit(prgch[19]))
		return fFalse;
	if (!_FIsDigit(prgch[20]))
		return fFalse;
	if (!_FIsDigit(prgch[21]))
		return fFalse;
	if (!_FIsDigit(prgch[22]))
		return fFalse;
	return _FValidPidSerial(prgch);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		determine if the serial number part of the pid string is valid
//	Arguments:
//		prgch - pid string
//	Returns:
//		whether the serial number part of the pid string is valid
////////////////////////////////////////////////////////////////////////////////
static BOOL _FValidPidSerial(CHAR *prgch)
{
	LONG i;
	LONG lSum;

	lSum = 0;
	prgch += 10;
	for (i = 0; i < 7; i++)
		{
		if (!_FIsDigit(*prgch))
			return fFalse;
		lSum += *prgch++ - '0';
		}
	return lSum % 7 == 0;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		determine if the character is a digit
//	Arguments:
//		ch - the character
//	Returns:
//		whether the character is a digit
////////////////////////////////////////////////////////////////////////////////
static BOOL _FIsDigit(CHAR ch)
{
	if (IsCharAlpha(ch))
		return fFalse;
	if (IsCharAlphaNumeric(ch))
		return fTrue;
	return fFalse;
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the appropriate sample customized "already used" hook dialog
//	Arguments:
//		paudhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCCDAlreadyUsedDialogHookProc(PAUDHD paudhd)
{
	HWND hwndT;
	WORD idd;
	int idc;

	if (paudhd->hd.cb < sizeof *paudhd)
		return shrcFail;
	switch (paudhd->audhm)
		{
	default:
		return shrcFail;
	case audhmBoth:
		idd = IDD_CONSACMEAUNAMEORG;
		break;
	case audhmName:
		idd = IDD_CONSACMEAUNAME;
		break;
	case audhmOrg:
		idd = IDD_CONSACMEAUORG;
		break;
	case audhmBad:
		idd = IDD_CONSACMEAUBAD;
		break;
		}

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(idd), hwndT,
		(DLGPROC)AlreadyUsedHookDlgProc, (LPARAM)paudhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized already used dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK AlreadyUsedHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAUDHD paudhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		paudhd = (PAUDHD)lParam;
		switch (paudhd->audhm)
			{
		case audhmBoth:
			_InsertIdcText(hdlg, IDC_EDIT1, paudhd->rgchName);
			_InsertIdcText(hdlg, IDC_EDIT2, paudhd->rgchOrg);
			break;
		case audhmName:
			_InsertIdcText(hdlg, IDC_EDIT1, paudhd->rgchName);
			break;
		case audhmOrg:
			_InsertIdcText(hdlg, IDC_EDIT1, paudhd->rgchOrg);
			break;
			}
		break;
		}
	return ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the appropriate sample customized "disk locked" hook dialog
//	Arguments:
//		pdldhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcIgore - ignore error
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCCDDiskLockDialogHookProc(PDLDHD pdldhd)
{
	HWND hwndT;
	int idc;

	if (pdldhd->hd.cb < sizeof *pdldhd)
		return shrcFail;

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMECDDISKLOCKED),
		hwndT, (DLGPROC)DiskLockHookDlgProc, (LPARAM)pdldhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDCANCEL:
		return shrcCancel;
	case IDOK:
		return shrcOk;
	case IDC_IGNORE:
		return shrcIgnore;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized CDInfo dialogs
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK DiskLockHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PDLDHD pdldhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		pdldhd = (PDLDHD)lParam;
		_InsertIdcPath(hdlg, IDC_TEXT1, pdldhd->rgchDir);
		break;
		}
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the appropriate sample customized CD piracy hook dialog
//	Arguments:
//		pcpdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCCDPiracyDialogHookProc(PCPDHD pcpdhd)
{
	HWND hwndT;
	int idd;
	int idc;

	if (pcpdhd->hd.cb < sizeof *pcpdhd)
		return shrcFail;

	switch (pcpdhd->cpdhm)
		{
	case cpdhmRemind:
		idd = IDD_CONSACMECDPIRACYREMIND;
		break;
	case cpdhmWarn:
		idd = IDD_CONSACMECDPIRACYWARN;
		break;
	case cpdhmWarn2:
		idd = IDD_CONSACMECDPIRACYWARN2;
		break;
		}

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(idd),
		hwndT, (DLGPROC)PiracyHookDlgProc, (LPARAM)pcpdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDCANCEL:
		return shrcCancel;
	case IDOK:
		return shrcOk;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized piracy dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK PiracyHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PCPDHD pcpdhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		pcpdhd = (PCPDHD)lParam;
		switch (pcpdhd->cpdhm)
			{
		case cpdhmRemind:
		case cpdhmWarn2:
			EnableWindow(GetDlgItem(hdlg, IDOK), FALSE);
			break;
			}
		break;
	case WM_COMMAND:
		pcpdhd = (PCPDHD)GetWindowLong(hdlg, DWL_USER);
		switch (pcpdhd->cpdhm)
			{
		case cpdhmRemind:
			switch (LOWORD(wParam))
				{
			case IDC_RADIO1:
				EnableWindow(GetDlgItem(hdlg, IDOK),
					IsDlgButtonChecked(hdlg, IDC_RADIO1));
				break;
			case IDC_RADIO2:
				EnableWindow(GetDlgItem(hdlg, IDOK),
					IsDlgButtonChecked(hdlg, IDC_RADIO2));
				break;
			case IDC_RADIO3:
				EnableWindow(GetDlgItem(hdlg, IDOK),
					IsDlgButtonChecked(hdlg, IDC_RADIO3));
				break;
				}
		case cpdhmWarn2:
			if (IDC_EDIT1 != LOWORD(wParam))
				{
				break;
				}
			EnableWindow(GetDlgItem(hdlg, IDOK),
				GetWindowTextLength(GetDlgItem(hdlg, IDC_EDIT1)) != 0);
			break;
			}
		}
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the appropriate sample customized directory hook dialog
//	Arguments:
//		pdrdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcDirectory - change directory button pushed
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCDirectoryDialogHookProc(PDRDHD pdrdhd)
{
	HWND hwndT;
	WORD idd;
	int idc;

	if (pdrdhd->hd.cb < sizeof *pdrdhd)
		return shrcFail;
	switch (pdrdhd->drdhm)
		{
	default:
		return shrcFail;
	case drdhmNotExistDir:
		idd = IDD_CONSACMEDIRNOT;
		break;
	case drdhmAppExistsOld:
		idd = IDD_CONSACMEDIROLD;
		break;
	case drdhmAppExistsNew:
		idd = IDD_CONSACMEDIRNEW;
		break;
		}

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(idd), hwndT,
		(DLGPROC)DirectoryDialogHookDlgProc, (LPARAM)pdrdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
	case IDC_CHANGE:
		return shrcDirectory;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized directory dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK DirectoryDialogHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PDRDHD pdrdhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		pdrdhd = (PDRDHD)lParam;
		_InsertIdcText(hdlg, IDC_TEXT1, pdrdhd->rgchComponent);
		_InsertIdcText(hdlg, IDC_TEXT2, pdrdhd->rgchComponent);
		_InsertIdcPath(hdlg, IDC_TEXT3, pdrdhd->rgchPath);
		break;
		}
	return ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample customized app main hook dialog
//	Arguments:
//		pamdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
//		shrcDirectory - change directory button pushed
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCAppMainDialogHookProc(PAMDHD pamdhd)
{
	HWND hwndT;
	WORD idd;
	int idc;

	if (pamdhd->hd.cb < sizeof *pamdhd)
		return shrcFail;

	switch (pamdhd->cambtn)
		{
	default:
		return shrcFail;
	case 1:
		idd = IDD_CONSACMEAPPMAIN1;
		break;

  /* We don't use this

	case 2:
		idd = IDD_CONSACMEAPPMAIN2;
		break;
	case 3:
		idd = IDD_CONSACMEAPPMAIN3;
		break;
	case 4:
		idd = IDD_CONSACMEAPPMAIN4;
		break;
  */
		}

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(idd), hwndT,
		(DLGPROC)AppMainHookDlgProc, (LPARAM)pamdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDC_BUTTON1:
		pamdhd->iambtnSelected = 0;
		return shrcOk;

/* We don't use this

	case IDC_BUTTON2:
		pamdhd->iambtnSelected = 1;
		return shrcOk;
	case IDC_BUTTON3:
		pamdhd->iambtnSelected = 2;
		return shrcOk;
	case IDC_BUTTON4:
		pamdhd->iambtnSelected = 3;
		return shrcOk;
*/
	case IDCANCEL:
		return shrcCancel;
	case IDC_CHANGE:
		return shrcDirectory;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample app main dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK AppMainHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAMDHD pamdhd;
	PAMBTN pambtn;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
		pamdhd = (PAMDHD)lParam;
		pambtn = (PAMBTN)GlobalLock(pamdhd->hrgambtn);
		switch (pamdhd->cambtn)
			{
    /* We don't use this
		case 4:
			_InsertIdcText(hdlg, IDC_TEXT4, pambtn[3].rgchText);
			_InsertIdcText(hdlg, IDC_LABEL4, pambtn[3].rgchLabel);
			_InsertIdcText(hdlg, IDC_BUTTON4, pambtn[3].rgchLabel);
			// fall through
		case 3:
			_InsertIdcText(hdlg, IDC_TEXT3, pambtn[2].rgchText);
			_InsertIdcText(hdlg, IDC_LABEL3, pambtn[2].rgchLabel);
			_InsertIdcText(hdlg, IDC_BUTTON3, pambtn[2].rgchLabel);
			// fall through
		case 2:
			_InsertIdcText(hdlg, IDC_TEXT2, pambtn[1].rgchText);
			_InsertIdcText(hdlg, IDC_LABEL2, pambtn[1].rgchLabel);
			_InsertIdcText(hdlg, IDC_BUTTON2, pambtn[1].rgchLabel);
			// fall through
    */
		case 1:
			_InsertIdcText(hdlg, IDC_TEXT1, pambtn[0].rgchText);
			_InsertIdcText(hdlg, IDC_LABEL1, pambtn[0].rgchLabel);
			_InsertIdcText(hdlg, IDC_BUTTON1, pambtn[0].rgchLabel);
			break;
			}
		GlobalUnlock(pamdhd->hrgambtn);
		_SubclassButton(hdlg, IDC_LABEL1);

/* We don't use this

		_SubclassButton(hdlg, IDC_LABEL2);
		_SubclassButton(hdlg, IDC_LABEL3);
		_SubclassButton(hdlg, IDC_LABEL4);
*/
		_InsertIdcPath(hdlg, IDC_TEXT5, pamdhd->rgchPath);
		switch (pamdhd->iambtnSelected)
			{
		case 0:
			SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
			break;

/* We don't use this

		case 1:
			SetFocus(GetDlgItem(hdlg, IDC_BUTTON2));
			break;
		case 2:
			SetFocus(GetDlgItem(hdlg, IDC_BUTTON3));
			break;
		case 3:
			SetFocus(GetDlgItem(hdlg, IDC_BUTTON4));
			break;
*/
			}
		return FALSE;
	case WM_DRAWITEM:
		HBITMAP hbmp;

		pamdhd = (PAMDHD)GetWindowLong(hdlg, DWL_USER);
		pambtn = (PAMBTN)GlobalLock(pamdhd->hrgambtn);
		switch (wParam)
			{
		case IDC_BUTTON1:
        	hbmp = pambtn[0].hbm;
			break;

/* We don't use this

		case IDC_BUTTON2:
        	hbmp = pambtn[1].hbm;
			break;
		case IDC_BUTTON3:
        	hbmp = pambtn[2].hbm;
			break;
		case IDC_BUTTON4:
        	hbmp = pambtn[3].hbm;
			break;
*/
			}
		GlobalUnlock(pamdhd->hrgambtn);
		DrawButton((LPDRAWITEMSTRUCT)lParam, hbmp);
        return TRUE;
	case WM_COMMAND:
	    if (LOWORD (wParam) == IDOK)
			{
			EndDialog(hdlg, GetWindowLong(GetFocus(), GWL_ID));
			return TRUE;
			}
		}
	return ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Window proc for the buttons on the sample app main dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK ButtonWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
		{
	case WM_GETDLGCODE:
		return DLGC_WANTCHARS;
		}
	return CallWindowProc(_lpfnButtonWndProc, hwnd, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Set the window proc for the buttons on the sample app main dialog
//	Arguments:
//		hdlg - the dialog handle
//		idc - the button control id
////////////////////////////////////////////////////////////////////////////////
static VOID _SubclassButton(HWND hdlg, int idc)
{
	WNDPROC lpfnT;

	if ((WNDPROC)NULL == _lpfnButtonWndProc)
		{
		_lpfnButtonWndProc = (WNDPROC)GetWindowLong(GetDlgItem(hdlg, idc),
			GWL_WNDPROC);
		}
	lpfnT = (WNDPROC)SetWindowLong(GetDlgItem(hdlg, idc), GWL_WNDPROC, (LONG)ButtonWndProc);
	if (lpfnT != _lpfnButtonWndProc)
		{
		SetWindowLong(GetDlgItem(hdlg, idc), GWL_WNDPROC, (LONG)ButtonWndProc);
		}
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample customized progman group hook dialog
//	Arguments:
//		ppmdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCProgmanGroupDialogHookProc(PPMDHD ppmdhd)
{
	HWND hwndT;
	int idc;

	if (ppmdhd->hd.cb < sizeof *ppmdhd)
		return shrcFail;

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEPROGMAN), hwndT,
		(DLGPROC)ProgmanGroupHookDlgProc, (LPARAM)ppmdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized progman group dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK ProgmanGroupHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PPMDHD ppmdhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		ppmdhd = (PPMDHD)lParam;
		_InsertIdcText(hdlg, IDC_EDIT1, ppmdhd->rgchGroup);
		LONG irgch;
		CHAR *pch;
		HWND hwnd;
		pch = (CHAR *)GlobalLock(ppmdhd->hrgrgchExisting);
		hwnd = GetDlgItem(hdlg, IDC_LIST1);
		for (irgch = 0; irgch < ppmdhd->crgchExisting; irgch++)
			{
			if (lstrcmpi(pch, ppmdhd->rgchGroup) != 0)
				SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)pch);
			pch += cbMaxSz;
			}
		GlobalUnlock(ppmdhd->hrgrgchExisting);
		SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)ppmdhd->rgchGroup);
		break;
	case WM_COMMAND:
	    switch (LOWORD (wParam))
			{
		case IDOK:
			ppmdhd = (PPMDHD)GetWindowLong(hdlg, DWL_USER);
			GetDlgItemText(hdlg, IDC_EDIT1, ppmdhd->rgchGroup,
				sizeof ppmdhd->rgchGroup);
			break;
		case IDC_LIST1:
			if (LBN_SELCHANGE == HIWORD(wParam))
				{
				LONG i;
				if (LB_ERR !=
					(i = (LONG)SendMessage((HWND)lParam, LB_GETCURSEL, 0, 0)))
					{
					CHAR rgch[cbMaxSz];
					SendMessage((HWND)lParam, LB_GETTEXT, i, (LPARAM)rgch);
					SetDlgItemText(hdlg, IDC_EDIT1, rgch);
					}
				}
			}
		break;
		}
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample reboot windows hook dialog
//	Arguments:
//		prbdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with reboot
//		shrcCancel - cancel reboot
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCRebootDialogHookProc(PRBDHD prbdhd)
{
	HWND hwndT;
	int idc;

	if (prbdhd->hd.cb < sizeof *prbdhd)
		return shrcFail;

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEREBOOT), hwndT,
		(DLGPROC)GenericHookDlgProc, (LPARAM)prbdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample reboot failed windows hook dialog
//	Arguments:
//		prbdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed
//		shrcFail - failure
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCRebootFailedDialogHookProc(PRFDHD prfdhd)
{
	HWND hwndT;
	int idc;

	if (prfdhd->hd.cb < sizeof *prfdhd)
		return shrcFail;

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEREBOOTFAILED),
		hwndT, (DLGPROC)GenericHookDlgProc, (LPARAM)prfdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
	case IDCANCEL:
		return shrcOk;
		}
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the appropriate sample customized exit hook dialog
//	Arguments:
//		pxtdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with exit
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCExitDialogHookProc(PXTDHD pxtdhd)
{
	HWND hwndT;
	WORD idd;
	int idc;

	if (pxtdhd->hd.cb < sizeof *pxtdhd)
		return shrcFail;
	switch (pxtdhd->xtdhm)
		{
	default:
		return shrcFail;
	case xtdhmOk:
    if(_szPID[5]!='\0')                   // If we have a valid PID (12345-123-1234567-12345)
		  idd = IDD_CONSACMEEXITOK;           // assume we were installing the product
    else
		  idd = IDD_CONSACMEEXITUNINSTALL;    // if not, we were uninstalling
		break;
	case xtdhmError:
		idd = IDD_CONSACMEEXITERROR;
		break;
	case xtdhmQuit:
		idd = IDD_CONSACMEEXITQUIT;
		break;
		}
	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(idd), hwndT,
		(DLGPROC)ExitHookDlgProc, (LPARAM)pxtdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
	case IDCANCEL:
		return shrcOk;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Exit dialog proc
//
//    We handle the WM_INITDIALOG message to place the PID number into the
//    dialog and then let GenericHookDlgProc() handler do the rest.
//
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK ExitHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
/*
	if (WM_INITDIALOG == uMsg)
		{
    if(_szPID[5]=='-')          // we have a valid PID
                                // set IDC_TEXT1 to PID number
      SetDlgItemText(hdlg, IDC_TEXT1, _szPID);

    else if(_szPID[5]!='\0')    // we don't have a valid PID but we're not in maintenance mode
                                // ie we're reinstalling the product (acme doesn't tell us the
                                // pid in this case).
      SetDlgItemText(hdlg, IDC_TEXT2, "");  // clear the PID caption text
		}
*/
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}



/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the appropriate sample customized disk space hook dialog
//	Arguments:
//		pdsdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcOption - change option button pushed
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCOODSDialogHookProc(PDSDHD pdsdhd)
{
	HWND hwndT;
	WORD idd;
	MSG  msg;

	if (pdsdhd->hd.cb < sizeof *pdsdhd)
		return shrcFail;

	if ((HWND)NULL == _hdlgOODS)
		{
		idd = pdsdhd->fAdmin ? IDD_CONSACMEOODSADMIN : IDD_CONSACMEOODSCOMBO;
		hwndT = HwndGetDlgOwner();
		_hdlgOODS = CreateDialogParam(hinst, MAKEINTRESOURCE(idd), hwndT,
			(DLGPROC)OODSHookDlgProc, (LPARAM)pdsdhd);
		if ((HWND)NULL == _hdlgOODS)
			{
			return shrcFail;
			}
		}
	else
		{
		SetWindowLong(_hdlgOODS, DWL_USER, (LPARAM)pdsdhd);
		_InitOODS(_hdlgOODS, pdsdhd, FALSE);
		}

	_idcOODS = IDC_STATIC;
	while (IDC_STATIC == _idcOODS && GetMessage(&msg, (HWND)NULL, 0, 0))
		{
		if (!IsDialogMessage(_hdlgOODS, &msg))
			{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		}
	UpdateWindow(hwndT);

	switch (_idcOODS)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
	case IDC_CHANGE:
		return shrcOption;
	case IDC_REFRESH:
		return shrcRefresh;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized oods dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK OODSHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PDSDHD pdsdhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		_InitOODS(hdlg, (PDSDHD)lParam, TRUE);
		break;
	case WM_COMMAND:
		pdsdhd = (PDSDHD)GetWindowLong(hdlg, DWL_USER);
		switch (LOWORD(wParam))
			{
		default:
			break;
		case IDC_COMBO1:
			if (CBN_SELCHANGE == HIWORD(wParam))
				{
				LONG idsdrv;
				LONG itm;
				CHAR rgch[cbMaxSz];
				itm = SendMessage((HWND)lParam, CB_GETCURSEL, 0, 0);
				SendMessage((HWND)lParam, CB_GETLBTEXT, itm, (LPARAM)rgch);
				idsdrv = rgch[0] - 'A';
				if (idsdrv >= 0 && idsdrv < 26 && idsdrv != pdsdhd->idsdrvCur)
					{
					pdsdhd->idsdrvCur = idsdrv;
					_idcOODS = IDC_REFRESH;
					}
				return TRUE;
				}
			break;
		case IDCANCEL:
			{
			AQDHD aqdhd;
			aqdhd.hd.cb = sizeof aqdhd;
			aqdhd.hd.rgchHelpFile[0] = '\0';
			aqdhd.hd.dwHelpContext = 0;
			if (shrcCancel == SHRCAskQuitDialogHookProc(&aqdhd))
				return TRUE;
			}
			// fall through
		case IDOK:
		case IDC_OPTION:
		case IDC_CHANGE:
			_hdlgOODS = (HWND)NULL;
			_idcOODS = LOWORD (wParam);
		    EndDialog(hdlg, LOWORD (wParam));
			return TRUE;
			}
		break;
	case WM_DRAWITEM:
		switch (wParam)
			{
		case IDC_COMBO1:
			DrawList((PDRAWITEMSTRUCT)lParam, sizeof _rgtabCombo1 / sizeof _rgtabCombo1[0],
				_rgtabCombo1, TRUE);
			break;
		case IDC_LIST1:
			((PDRAWITEMSTRUCT)lParam)->itemState &= ~ODS_SELECTED;
			DrawList((PDRAWITEMSTRUCT)lParam, sizeof _rgtabList1 / sizeof _rgtabList1[0],
				_rgtabList1, FALSE);
			break;
			}
		return TRUE;
		}
	return ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Set the values in the OODS dialog to match pdsdhd
//	Arguments:
//		pdsdhd - routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
////////////////////////////////////////////////////////////////////////////////
static VOID _InitOODS(HWND hdlg, PDSDHD pdsdhd, BOOL fNewDialog)
{
	LONG idstm;
	PDSTM pdstm;
	LONG idsdrv;
	PDSDRV pdsdrv;

	if (!pdsdhd->fAdmin)
		{
		if (!fNewDialog)
			{
			SendDlgItemMessage(hdlg, IDC_LIST1, LB_RESETCONTENT, 0, 0);
			}
		pdstm = (PDSTM)GlobalLock(pdsdhd->hrgdstm);
		for (idstm = 0; idstm < pdsdhd->cdstm; idstm++)
			{
			CHAR rgch[cbMaxSz];
			if ('\0' == pdstm->rgchSize[0])
				wsprintf(rgch, "%s", pdstm->rgchDesc);
			else
				wsprintf(rgch, "%s\t%s K", pdstm->rgchDesc, pdstm->rgchSize);
			SendDlgItemMessage(hdlg, IDC_LIST1, LB_ADDSTRING, 0, (LPARAM)rgch);
			pdstm++;
			}
		GlobalUnlock(pdsdhd->hrgdstm);
		}
	if (fNewDialog)
		{
		pdsdrv = (PDSDRV)GlobalLock(pdsdhd->hrgdsdrv);
		for (idsdrv = 0; idsdrv < pdsdhd->cdsdrv; idsdrv++)
			{
			CHAR rgch[cbMaxSz];
			CHAR rgchDrv[3];
			if (pdsdrv->cbReq != 0 && pdsdrv->cbNeed != 0)
				{
				rgchDrv[0] = (CHAR)('A' + idsdrv);
				rgchDrv[1] = ':';
				rgchDrv[2] = '\0';
				wsprintf(rgch, "%s\t%ld K\t%ld K\t%ld K", rgchDrv,
					_CkbOfCb(pdsdrv->cbReq),
					max(0, _CkbOfCb(pdsdrv->cbReq - pdsdrv->cbNeed)),
					_CkbOfCb(pdsdrv->cbNeed));
				SendDlgItemMessage(hdlg, IDC_COMBO1, CB_ADDSTRING, 0, (LPARAM)rgch);
				}
			pdsdrv++;
			}
		GlobalUnlock(pdsdhd->hrgdsdrv);
		SendDlgItemMessage(hdlg, IDC_COMBO1, CB_SETCURSEL, pdsdhd->idsdrvCur, 0L);
		}
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the appropriate sample customized option hook dialog
//	Arguments:
//		popdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcDirectory - change directory button pushed
//		shrcOption - change option button pushed
//		shrcRefresh - refresh sizes/strings in data structure
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCOptionsDialogHookProc(POPDHD popdhd)
{
	HWND hwndT;
	WORD idd;
	POPTM poptm;
	LONG ioptm;
	BOOL fCustom;
	MSG  msg;

	if (popdhd->hd.cb < sizeof *popdhd)
		return shrcFail;

	if ((HWND)NULL == _hdlgOptions)
		{
		poptm = (POPTM)GlobalLock(popdhd->hrgoptm);
		fCustom = FALSE;
		for (ioptm = 0; ioptm < popdhd->coptm; ioptm++)
			{
			if (poptm->fChangeOption || poptm->fChangeDirectory)
				fCustom = TRUE;
			poptm++;
			}
		GlobalUnlock(popdhd->hrgoptm);

		if (fCustom)
			idd = popdhd->fMaint ? IDD_CONSACMECUSTOMDEL : IDD_CONSACMECUSTOM;
		else
			idd = popdhd->fMaint ? IDD_CONSACMEOPTIONDEL : IDD_CONSACMEOPTION;

		hwndT = HwndGetDlgOwner();
		_hdlgOptions = CreateDialogParam(hinst, MAKEINTRESOURCE(idd),
			hwndT, (DLGPROC)OptionsHookDlgProc, (LPARAM)popdhd);

		if ((HWND)NULL == _hdlgOptions)
			{
			return shrcFail;
			}
		}
	else
		{
		SetWindowLong(_hdlgOptions, DWL_USER, (LPARAM)popdhd);
		_InitOptions(_hdlgOptions, popdhd, FALSE);
		}

	_idcOptions = IDC_STATIC;
	while (IDC_STATIC == _idcOptions && GetMessage(&msg, (HWND)NULL, 0, 0))
		{
		if (!IsDialogMessage(_hdlgOptions, &msg))
			{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			}
		}
	UpdateWindow(hwndT);

	switch (_idcOptions)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
	case IDC_CHANGE:
		return shrcDirectory;
	case IDC_OPTION:
		return shrcOption;
	case IDC_REFRESH:
		return shrcRefresh;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample option dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK OptionsHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POPDHD popdhd;
	POPTM poptm;
	LONG ioptm;

    switch (uMsg)
		{
	case WM_INITDIALOG:
		_InitOptions(hdlg, (POPDHD)lParam, TRUE);
		break;
	case WM_COMMAND:
		popdhd = (POPDHD)GetWindowLong(hdlg, DWL_USER);
		switch (LOWORD(wParam))
			{
		default:
		case IDC_LIST1:
			if (LBN_SELCHANGE == HIWORD(wParam))
				{
				poptm = (POPTM)GlobalLock(popdhd->hrgoptm);
				for (ioptm = 0; ioptm < popdhd->coptm; ioptm++)
					{
					BOOL f;

					f = (BOOL)SendMessage((HWND)lParam, LB_GETSEL, ioptm, 0);
					if ((f != FALSE) != (poptm->fChecked != FALSE))
						{
						_idcOptions = IDC_REFRESH;
						if (poptm->fVital && !f && poptm->fChecked)
							{
							int idRet;
							FMsgBox(hdlg, IDS_VITALCOMPONENT, MB_OK, &idRet);
							}
						poptm->fChecked = f != FALSE;
						}
					poptm++;
					}
				ioptm = SendMessage((HWND)lParam, LB_GETCARETINDEX, 0, 0);
				if (ioptm != popdhd->ioptmCur)
					{
					popdhd->ioptmCur = ioptm;
					_idcOptions = IDC_REFRESH;
					}
				GlobalUnlock(popdhd->hrgoptm);
				return TRUE;
				}
			break;
		case IDOK:
			if (popdhd->fTop && popdhd->fMaint)
				{
				int idRet;
				if (popdhd->cToAdd == 0 && popdhd->cToRemove == 0)
					{
					FMsgBox(hdlg, IDS_NOADDREMOVE,
						MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL, &idRet);
					return TRUE;
					}
				else if (popdhd->cToRemove > 0)
					{
					if (FMsgBox(hdlg, (popdhd->cToRemove == 1) ?
						IDS_CONFIRMREMOVE1 : IDS_CONFIRMREMOVEN,
						MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON1 | MB_APPLMODAL,
							&idRet) && idRet == IDNO)
						{
						return TRUE;
						}
					}
				}
			// fall through
		case IDCANCEL:
		case IDC_OPTION:
		case IDC_CHANGE:
			_hdlgOptions = (HWND)NULL;
			_idcOptions = LOWORD (wParam);
		    EndDialog(hdlg, LOWORD (wParam));
			return TRUE;
			}
		break;
	case WM_DRAWITEM:
		DrawList((PDRAWITEMSTRUCT)lParam, sizeof _rgtabOptions / sizeof _rgtabOptions[0],
			_rgtabOptions, FALSE);
		return TRUE;
		}
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Set the values in the options dialog to match popdhd
//	Arguments:
//		popdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
////////////////////////////////////////////////////////////////////////////////
static VOID _InitOptions(HWND hdlg, POPDHD popdhd, BOOL fNewDialog)
{
	POPTM poptm;
	HWND hwnd;
	BOOL fCustom;
	LONG ioptm;
	CHAR rgch[cbMaxSz];
	CHAR rgchT[cbMaxSz];

	poptm = (POPTM)GlobalLock(popdhd->hrgoptm);
	SetDlgItemText(hdlg, IDC_TEXT1, poptm[popdhd->ioptmCur].rgchDesc);
	SetDlgItemText(hdlg, IDC_TEXT2, poptm[popdhd->ioptmCur].rgchDir);

	FLoadIds(IDS_SPACEREQUIRED, rgchT, sizeof rgchT);
	wsprintf(rgch, rgchT, popdhd->rgchDrvCur);
	SetDlgItemText(hdlg, IDC_TEXT3, rgch);
	FLoadIds(IDS_SPACEAVAILABLE, rgchT, sizeof rgchT);
	wsprintf(rgch, rgchT, popdhd->rgchDrvCur);
	SetDlgItemText(hdlg, IDC_TEXT4, rgch);
	wsprintf(rgch, "%ld K", _CkbOfCb(popdhd->cbDrvCurReq));
	SetDlgItemText(hdlg, IDC_TEXT5, rgch);
	wsprintf(rgch, "%ld K", _CkbOfCb(popdhd->cbDrvCurAvail));
	SetDlgItemText(hdlg, IDC_TEXT6, rgch);
	SetDlgItemInt(hdlg, IDC_TEXT7, popdhd->cToAdd, FALSE);
	SetDlgItemInt(hdlg, IDC_TEXT8, popdhd->cToRemove, FALSE);
	hwnd = GetDlgItem(hdlg, IDC_LIST1);
	fCustom = FALSE;
	for (ioptm = 0; ioptm < popdhd->coptm; ioptm++)
		{
		CHAR rgch[cbMaxSz];
		BOOL f;
		if (fNewDialog)
			{
			wsprintf(rgch, "%s\t%ld K", poptm->rgchTitle, _CkbOfCb(poptm->cb));
			SendMessage(hwnd, LB_ADDSTRING, 0, (LPARAM)rgch);
			}
		// only send LB_SETSEL if the selection has changed to avoid flicker
		f = (BOOL)SendMessage(hwnd, LB_GETSEL, ioptm, 0);
		if ((f != FALSE) != (poptm->fChecked != FALSE))
			SendMessage(hwnd, LB_SETSEL, poptm->fChecked, ioptm);
		if (poptm->fChangeOption || poptm->fChangeDirectory)
			fCustom = TRUE;
		poptm++;
		}
	SendMessage(hwnd, LB_SETCARETINDEX, popdhd->ioptmCur, MAKELPARAM(TRUE, 0));
	poptm -= popdhd->coptm;
	if (fCustom)
		{
		EnableWindow(GetDlgItem(hdlg, IDC_OPTION),
			poptm[popdhd->ioptmCur].fChecked &&
				poptm[popdhd->ioptmCur].fChangeOption);
		EnableWindow(GetDlgItem(hdlg, IDC_CHANGE),
			poptm[popdhd->ioptmCur].fChecked &&
				poptm[popdhd->ioptmCur].fChangeDirectory);
		}
	GlobalUnlock(popdhd->hrgoptm);
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample admin warning hook dialog
//	Arguments:
//		pawdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCAdminWarnDialogHookProc(PAWDHD pawdhd)
{
	HWND hwndT;
	int idc;

	if (pawdhd->hd.cb < sizeof *pawdhd)
		return shrcFail;

	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEADMINWARN), hwndT,
		(DLGPROC)ConfirmExitHookDlgProc, (LPARAM)pawdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample Ms Apps directory hook dialog
//	Arguments:
//		pmadhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
//		shrcDirectory - change directory button pushed
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCMsAppsDirDialogHookProc(PMADHD pmadhd)
{
	HWND hwndT;
	int idc;

	if (pmadhd->hd.cb < sizeof *pmadhd)
		return shrcFail;
	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEMSAPPSDIR), hwndT,
		(DLGPROC)MsAppsDirHookDlgProc, (LPARAM)pmadhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
	case IDC_CHANGE:
		return shrcDirectory;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized msapps directory dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK MsAppsDirHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PMADHD pmadhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		pmadhd = (PMADHD)lParam;
		_InsertIdcPath(hdlg, IDC_TEXT1, pmadhd->rgchPath);
		break;
		}
	return ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample confirm server hook dialog
//	Arguments:
//		pcsdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCConfirmServerDialogHookProc(PCSDHD pcsdhd)
{
	HWND hwndT;
	WORD idd;
	int idc;

	if (pcsdhd->hd.cb < sizeof *pcsdhd)
		return shrcFail;
	hwndT = HwndGetDlgOwner();
	idd = (pcsdhd->rgchServer[0] == '\0') ? IDD_CONSACMECONFIRMSERVERNONE :
		IDD_CONSACMECONFIRMSERVER;
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(idd), hwndT,
		(DLGPROC)ConfirmServerHookDlgProc, (LPARAM)pcsdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized msapps directory dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK ConfirmServerHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PCSDHD pcsdhd;
	CHAR rgch[cbMaxSz];
	CHAR ch;

    switch (uMsg)
		{
	case WM_INITDIALOG:
		pcsdhd = (PCSDHD)lParam;
		_InsertIdcText(hdlg, IDC_TEXT1, pcsdhd->rgchName);
		_InsertIdcPath(hdlg, IDC_TEXT2, pcsdhd->rgchPath);
		_InsertIdcText(hdlg, IDC_EDIT1, pcsdhd->rgchServer);
		_InsertIdcText(hdlg, IDC_EDIT2, pcsdhd->rgchNetPath);
		SendDlgItemMessage(hdlg, IDC_COMBO1, CB_SETEXTENDEDUI, (WPARAM)1, (LPARAM)0);
		rgch[1] = ':';
		rgch[2] = '\0';
		for (ch = 'A'; ch <= 'Z'; ch++)
			{
			rgch[0] = ch;
			SendDlgItemMessage(hdlg, IDC_COMBO1, CB_INSERTSTRING, (WPARAM)ch - 'A', (LPARAM)rgch);
			}
		pcsdhd->chDrive = (CHAR)CharUpper((PSTR)pcsdhd->chDrive);
		SendDlgItemMessage(hdlg, IDC_COMBO1, CB_SETCURSEL, pcsdhd->chDrive - 'A', 0);
		switch (pcsdhd->csdhm)
			{
		default:
		case csdhmName:
			SendDlgItemMessage(hdlg, IDC_RADIO1, BM_SETCHECK, TRUE, 0);
			break;
		case csdhmLetter:
			SendDlgItemMessage(hdlg, IDC_RADIO2, BM_SETCHECK, TRUE, 0);
			break;
			}
		break;
	case WM_COMMAND:
	    switch (LOWORD (wParam))
			{
		case IDOK:
			pcsdhd = (PCSDHD)GetWindowLong(hdlg, DWL_USER);
			GetDlgItemText(hdlg, IDC_EDIT1, pcsdhd->rgchServer,
				sizeof pcsdhd->rgchServer);
			GetDlgItemText(hdlg, IDC_EDIT2, pcsdhd->rgchNetPath,
				sizeof pcsdhd->rgchNetPath);
			SendDlgItemMessage(hdlg, IDC_COMBO1, CB_GETLBTEXT,
				(WPARAM)SendDlgItemMessage(hdlg, IDC_COMBO1, CB_GETCURSEL, 0, 0),
				(LPARAM)rgch);
			pcsdhd->chDrive = rgch[0];
			if (SendDlgItemMessage(hdlg, IDC_RADIO1, BM_GETCHECK, 0, 0))
				{
				pcsdhd->csdhm = csdhmName;
				}
			else // if (SendDlgItemMessage(hdlg, IDC_RADIO2, BM_GETCHECK, 0, 0))
				{
				pcsdhd->csdhm = csdhmLetter;
				}
			break;
			}
		break;
		}
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample shared files hook dialog
//	Arguments:
//		psfdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCSharedFilesDialogHookProc(PSFDHD psfdhd)
{
	HWND hwndT;
	WORD idd;
	int idc;

	if (psfdhd->hd.cb < sizeof *psfdhd)
		return shrcFail;

	hwndT = HwndGetDlgOwner();
	idd = psfdhd->fAdmin ? IDD_CONSACMESHAREDFILESADMIN :
		IDD_CONSACMESHAREDFILES;
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(idd), hwndT,
		(DLGPROC)SharedFilesHookDlgProc, (LPARAM)psfdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized msapps directory dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK SharedFilesHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PSFDHD psfdhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		psfdhd = (PSFDHD)lParam;
		switch (psfdhd->sfdhm)
			{
		case sfdhmServer:
			SendDlgItemMessage(hdlg, IDC_RADIO1, BM_SETCHECK, TRUE, 0);
			break;
		case sfdhmLocal:
			SendDlgItemMessage(hdlg, IDC_RADIO2, BM_SETCHECK, TRUE, 0);
			break;
		case sfdhmChoice:
			SendDlgItemMessage(hdlg, IDC_RADIO3, BM_SETCHECK, TRUE, 0);
			break;
			}
		break;
	case WM_COMMAND:
	    switch (LOWORD (wParam))
			{
		case IDOK:
			psfdhd = (PSFDHD)GetWindowLong(hdlg, DWL_USER);
			if (SendDlgItemMessage(hdlg, IDC_RADIO1, BM_GETCHECK, 0, 0))
				{
				psfdhd->sfdhm = sfdhmServer;
				}
			else if (SendDlgItemMessage(hdlg, IDC_RADIO2, BM_GETCHECK, 0, 0))
				{
				psfdhd->sfdhm = sfdhmLocal;
				}
			else // if (SendDlgItemMessage(hdlg, IDC_RADIO3, BM_GETCHECK, 0, 0))
				{
				psfdhd->sfdhm = sfdhmChoice;
				}
			break;
			}
		break;
		}
	return ConfirmExitHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample hook dialog
//	Arguments:
//		prsdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcNone - don't remove any shared components
//		shrcKeep - don't remove this shared component
//		shrcRemoveAll - remove all shared components
//		shrcRemove - remove this shared component
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCRemoveSharedDialogHookProc(PRSDHD prsdhd)
{
	HWND hwndT;
	int idc;

	if (prsdhd->hd.cb < sizeof *prsdhd)
		return shrcFail;
	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEREMOVESHARED),
		hwndT, (DLGPROC)RemoveSharedHookDlgProc, (LPARAM)prsdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDC_NONE:
		return shrcNone;
	case IDC_KEEP:
		return shrcKeep;
	case IDC_REMOVEALL:
		return shrcRemoveAll;
	case IDC_REMOVE:
		return shrcRemove;
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample customized remove shared dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK RemoveSharedHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PRSDHD prsdhd;
    switch (uMsg)
		{
	case WM_INITDIALOG:
		prsdhd = (PRSDHD)lParam;
		_InsertIdcText(hdlg, IDC_TEXT1, prsdhd->rgchComponent);
		break;
		}
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample Ask Quit hook dialog
//	Arguments:
//		paqdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCAskQuitDialogHookProc(PAQDHD paqdhd)
{
	HWND hwndT;
	int idc;

	if (paqdhd->hd.cb < sizeof *paqdhd)
		return shrcFail;
	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEASKQUIT), hwndT,
		(DLGPROC)GenericHookDlgProc, (LPARAM)paqdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample Invalid Net Path hook dialog
//	Arguments:
//		pindhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - re-enter path
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCInvalidNetPathDialogHookProc(PINDHD pindhd)
{
	HWND hwndT;
	int idc;

	if (pindhd->hd.cb < sizeof *pindhd)
		return shrcFail;
	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEINVALNETPATH),
		hwndT, (DLGPROC)GenericHookDlgProc, (LPARAM)pindhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample No Connection hook dialog
//	Arguments:
//		pncdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCNoConnectionDialogHookProc(PNCDHD pncdhd)
{
	HWND hwndT;
	int idc;

	if (pncdhd->hd.cb < sizeof *pncdhd)
		return shrcFail;
	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMENOCONNECTION),
		hwndT, (DLGPROC)GenericHookDlgProc, (LPARAM)pncdhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the specified sample modeless hook dialog
//	Arguments:
//		psmdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - abort setup
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCStartModelessDialogHookProc(PSMDHD psmdhd)
{
	WORD idd;
	HWND hdlg;
	DWORD dwClicks;

	if (psmdhd->hd.cb < sizeof *psmdhd)
		return shrcFail;
	dwClicks = _DwClicksModelessStop();
	switch (psmdhd->smdhm)
		{
	default:
		return shrcFail;
	case smdhmSearching:
		idd = IDD_CONSACMESEARCHING;
		break;
	case smdhmCheckingDisk:
		idd = IDD_CONSACMECHECKINGDISK;
		break;
	case smdhmModifyingSys:
		idd = IDD_CONSACMEMODIFYINGSYS;
		break;
		}
	hdlg = CreateDialogParam(hinst, MAKEINTRESOURCE(idd),
		psmdhd->hwndParent, (DLGPROC)GenericHookDlgProc, (LPARAM)dwClicks);
	if ((HWND)NULL == hdlg)
		{
		return shrcFail;
		}
	psmdhd->dwHook = (DWORD)hdlg;
	return shrcOk;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Ends the specified sample modeless hook dialog
//	Arguments:
//		pemdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCEndModelessDialogHookProc(PEMDHD pemdhd)
{
	HWND hdlg;
	DWORD dwClicks;

	hdlg = (HWND)pemdhd->dwHook;
	dwClicks = (DWORD)GetWindowLong(hdlg, DWL_USER);
	_WaitToEndModeless(dwClicks);
	EndDialog(hdlg, IDOK);
	return shrcOk;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample create frame hook procedure
//	Arguments:
//		pcfhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcFrameRegisterFail - failed to register class
//		shrcFrameCreateFail - failed to create the window
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCCreateFrameHookProc(PCFHD pcfhd)
{
	HMENU hmenu;

	if (pcfhd->cbCFHD < sizeof *pcfhd)
		return shrcFail;

	if (!RegisterClass(&pcfhd->wc))
		{
		return shrcFrameRegisterFail;
		}

	pcfhd->hwndFrame = CreateWindow(pcfhd->wc.lpszClassName,
		pcfhd->cs.lpszName, pcfhd->cs.style, pcfhd->cs.x, pcfhd->cs.y,
		pcfhd->cs.cx, pcfhd->cs.cy, pcfhd->cs.hwndParent, pcfhd->cs.hMenu,
		pcfhd->wc.hInstance, pcfhd->cs.lpCreateParams);

	if ((HWND)NULL == pcfhd->hwndFrame)
		{
		UnregisterClass(pcfhd->wc.lpszClassName, pcfhd->wc.hInstance);
		return shrcFrameCreateFail;
		}

	hmenu = GetSystemMenu(pcfhd->hwndFrame, FALSE);

	if ((HMENU)NULL != hmenu)
		{
		CHAR rgch[cbMaxSz];

		DeleteMenu(hmenu, 5, MF_BYPOSITION);
		DeleteMenu(hmenu, 5, MF_BYPOSITION);

		AppendMenu(hmenu, MF_SEPARATOR, 0, (LPSTR)NULL);
		if (FLoadIds(IDS_ABOUT, rgch, sizeof rgch))
			AppendMenu(hmenu, MF_STRING, 0x10, "");
		}

	ShowWindow(pcfhd->hwndFrame, pcfhd->nCmdShow);
	UpdateWindow(pcfhd->hwndFrame);

	return shrcOk;
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample message box hook procedure
//	Arguments:
//		pmbhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - succeeded in putting up message box
//		shrcFail - failed to put up message box
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCMessageBoxHookProc(PMBHD pmbhd)
{
	if (pmbhd->cbMBHD < sizeof *pmbhd)
		return shrcFail;

	pmbhd->idRet = MessageBox(pmbhd->hwndOwner, pmbhd->lpszText,
		pmbhd->lpszTitle, pmbhd->uStyle);
	return (0 == pmbhd->idRet) ? shrcFail : shrcOk;
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample gauge init hook procedure
//	Arguments:
//		pgihd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - succeeded in putting up message box
//		shrcFail - failed to put up message box
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCGaugeInitHookProc(PGIHD pgihd)
{
	WNDCLASS cls;

	if (pgihd->cbGIHD < sizeof *pgihd)
		return shrcFail;

	cls.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
	cls.hIcon = (HICON)NULL;
	cls.lpszMenuName = (LPSTR)NULL;
	cls.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	cls.hInstance = hinst;
	cls.style = CS_HREDRAW | CS_VREDRAW;
	cls.cbClsExtra = 0;

	cls.lpszClassName = _szClassGizmo;
	cls.lpfnWndProc = GizmoWndProc;
	cls.cbWndExtra = 3 * sizeof (LONG);
	if (!RegisterClass(&cls))
		{
		return shrcFail;
		}

	cls.lpszClassName = _szClassCText;
	cls.lpfnWndProc = CTextWndProc;
	cls.cbWndExtra = sizeof (LONG);
	if (!RegisterClass(&cls))
		{
		UnregisterClass(_szClassGizmo, hinst);
		return shrcFail;
		}

	return shrcOk;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample gauge open hook procedure
//	Arguments:
//		pgohd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - succeeded in putting up message box
//		shrcFail - failed to put up message box
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCGaugeOpenHookProc(PGOHD pgohd)
{
	HWND hdlg;
	HGLOBAL hgd;
	PGD pgd;

	if (pgohd->cbGOHD < sizeof *pgohd)
		return shrcFail;
	hgd = GlobalAlloc(GMEM_MOVEABLE, sizeof (GD));
	if ((HGLOBAL)NULL == hgd)
		return shrcFail;
	pgd = (PGD)GlobalLock(hgd);
	pgd->hdlgGaugePrev = _hdlgGaugeCur;
	pgd->pfAbort = pgohd->pfAbort;
	GlobalUnlock(hgd);
	hdlg = CreateDialogParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEGAUGE),
		pgohd->hwndParent, (DLGPROC)GaugeDlgProc, (LPARAM)hgd);
	if ((HWND)NULL == hdlg)
		{
		return shrcFail;
		}
	SetWindowPos(hdlg, hwndNull, 0, 0, 0, 0,
			SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	_hdlgGaugeCur = hdlg;
	return shrcOk;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Dialog proc for the sample progress gauge dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK GaugeDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
		{
	case WM_DESTROY:
		GlobalFree((HGLOBAL)GetWindowLong(hdlg, DWL_USER));
		break;
	case WM_COMMAND:
	    switch (LOWORD (wParam))
			{
		case IDCANCEL:
			if (DialogBox(hinst, MAKEINTRESOURCE(IDD_CONSACMEINTERRUPT),
				hdlg, (DLGPROC)GenericHookDlgProc) == IDOK)
				{
				HGLOBAL hgd;
				PGD pgd;
				hgd = (HGLOBAL)GetWindowLong(hdlg, DWL_USER);
				pgd = (PGD)GlobalLock(hgd);
				*pgd->pfAbort = TRUE;
				GlobalUnlock(hgd);
				}
			return TRUE;
			}
		break;
		}
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample gauge close hook procedure
//	Arguments:
//		pgchd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - succeeded in putting up message box
//		shrcFail - failed to put up message box
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCGaugeCloseHookProc(PGCHD pgchd)
{
	HWND hdlg;
	HGLOBAL hgd;
	PGD pgd;

	if (pgchd->cbGCHD < sizeof *pgchd)
		return shrcFail;
	if ((HWND)NULL == _hdlgGaugeCur)
		return shrcFail;
	hdlg = _hdlgGaugeCur;
	hgd = (HGLOBAL)GetWindowLong(hdlg, DWL_USER);
	if ((HGLOBAL)hgd == NULL)
		return shrcFail;
	pgd = (PGD)GlobalLock(hgd);
	_hdlgGaugeCur = pgd->hdlgGaugePrev;
	GlobalUnlock(hgd);
	EndDialog(hdlg, IDCANCEL);
	return shrcOk;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample gauge title hook procedure
//	Arguments:
//		pgthd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - succeeded in putting up message box
//		shrcFail - failed to put up message box
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCGaugeTitleHookProc(PGTHD pgthd)
{
	if (pgthd->cbGTHD < sizeof *pgthd)
		return shrcFail;
	if ((HWND)NULL == _hdlgGaugeCur)
		return shrcFail;
	SetWindowText(_hdlgGaugeCur, pgthd->rgchTitle);
	return shrcOk;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample gauge text hook procedure
//	Arguments:
//		pgxhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - succeeded in putting up message box
//		shrcFail - failed to put up message box
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCGaugeTextHookProc(PGXHD pgxhd)
{
	if (pgxhd->cbGXHD < sizeof *pgxhd)
		return shrcFail;
	if ((HWND)NULL == _hdlgGaugeCur)
		return shrcFail;
	SetDlgItemText(_hdlgGaugeCur, IDC_TEXT1, pgxhd->rgchSrc);
	SetDlgItemText(_hdlgGaugeCur, IDC_TEXT2, pgxhd->rgchDst);
	return shrcOk;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample gauge range hook procedure
//	Arguments:
//		pgrhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - succeeded in putting up message box
//		shrcFail - failed to put up message box
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCGaugeRangeHookProc(PGRHD pgrhd)
{
	if (pgrhd->cbGRHD < sizeof *pgrhd)
		return shrcFail;
	if ((HWND)NULL == _hdlgGaugeCur)
		return shrcFail;
	SendDlgItemMessage(_hdlgGaugeCur, IDC_BAR1, BAR_SETRANGE, pgrhd->iLim, 0L);
	return shrcOk;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample gauge position hook procedure
//	Arguments:
//		pgphd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - succeeded in putting up message box
//		shrcFail - failed to put up message box
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCGaugePosHookProc(PGPHD pgphd)
{
	if (pgphd->cbGPHD < sizeof *pgphd)
		return shrcFail;
	if ((HWND)NULL == _hdlgGaugeCur)
		return shrcFail;
	SendDlgItemMessage(_hdlgGaugeCur, IDC_BAR1, BAR_SETPOS, pgphd->iCur, 0L);
	return shrcOk;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample gauge delta hook procedure
//	Arguments:
//		pgdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - succeeded in putting up message box
//		shrcFail - failed to put up message box
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCGaugeDeltaHookProc(PGDHD pgdhd)
{
	if (pgdhd->cbGDHD < sizeof *pgdhd)
		return shrcFail;
	if ((HWND)NULL == _hdlgGaugeCur)
		return shrcFail;
	SendDlgItemMessage(_hdlgGaugeCur, IDC_BAR1, BAR_DELTAPOS, pgdhd->dCur, 0L);
	return shrcOk;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample gauge yield hook procedure
//	Arguments:
//		pgyhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - WM_QUIT encountered, abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCGaugeYieldHookProc(PGYHD pgyhd)
{
	MSG msg;
	SHRC shrc;

	if (pgyhd->cbGYHD < sizeof *pgyhd)
		return shrcFail;

	shrc = shrcOk;
	while (PeekMessage(&msg, (HWND)NULL, 0, 0, PM_REMOVE))
		{
		if ((HWND)NULL != _hdlgGaugeCur &&
			IsDialogMessage(_hdlgGaugeCur, &msg))
			{
			continue;
			}

		if (msg.message == WM_QUIT)
			shrc = shrcCancel;

		TranslateMessage(&msg);
		DispatchMessage(&msg);
		}

	if (HwndFrame())
		UpdateWindow(HwndFrame());
	if ((HWND)NULL != _hdlgGaugeCur)
		UpdateWindow(_hdlgGaugeCur);

	return shrc;
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Sample file copy warning hook procedure (activated by cd piracy
//		mechanism). It requires the user for confirm every file copy.
//	Arguments:
//		pfwhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with setup
//		shrcCancel - WM_QUIT encountered, abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCFileCopyWarningHookProc(PFWHD pfwhd)
{
	HWND hwndT;
	int idc;

	if (pfwhd->cbFWHD < sizeof *pfwhd)
		return shrcFail;
	hwndT = HwndGetDlgOwner();
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEFILECOPYWARN),
		hwndT, (DLGPROC)GenericHookDlgProc, (LPARAM)pfwhd);
	UpdateWindow(hwndT);
	switch (idc)
		{
	default:
		return shrcFail;
	case IDOK:
		return shrcOk;
	case IDCANCEL:
		return shrcCancel;
		}
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		window proc for the progress gizmo control
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK GizmoWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
		{
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	case WM_CREATE:
		SetWindowLong(hwnd, ilBarRange, 1L);
		SetWindowLong(hwnd, ilBarPos, 0L);
		break;
	case WM_ERASEBKGND:
		break;
	case BAR_SETPOS:
		SetWindowLong(hwnd, ilBarPos, wParam);
		InvalidateRect(hwnd, (LPRECT)NULL, TRUE);
		UpdateWindow(hwnd);
		break;
	case BAR_SETRANGE:
		SetWindowLong(hwnd, ilBarRange, wParam);
		InvalidateRect(hwnd, (LPRECT)NULL, TRUE);
		UpdateWindow(hwnd);
		break;
	case BAR_DELTAPOS:
		SetWindowLong(hwnd, ilBarPos, GetWindowLong(hwnd, ilBarPos) + wParam);
		InvalidateRect(hwnd, (LPRECT)NULL, TRUE);
		UpdateWindow(hwnd);
		break;
	case WM_SETFONT:
		SetWindowLong(hwnd, ilFont, wParam);
		if (!lParam)
			break;
		InvalidateRect(hwnd, (LPRECT)NULL, TRUE);
		UpdateWindow(hwnd);
		break;
	case WM_PAINT:
		PAINTSTRUCT ps;
		RECT rect1;
		RECT rect2;
		INT dx;
		INT x;
		LONG iRange;
		LONG iPos;
		CHAR rgch[5];
		LONG cch;
		SIZE size;
		HFONT hfontCur;
		HFONT hfontSav;
		COLORREF clrBackGround;
		COLORREF clrForeGround;

		BeginPaint(hwnd, &ps);
		hfontCur = (HFONT)GetWindowLong(hwnd, ilFont);
		if ((HFONT)NULL != hfontCur)
			hfontSav = (HFONT)SelectObject(ps.hdc, hfontCur);
		else
			hfontSav = (HFONT)NULL;
		
		GetClientRect(hwnd, &rect1);
		rect2 = rect1;

		iRange = GetWindowLong(hwnd, ilBarRange);
		if (iRange <= 0)
			iRange = 1;
		iPos   = GetWindowLong(hwnd, ilBarPos);
		if (iPos > iRange)
			iPos = iRange;

		dx = rect1.right;
		x = (WORD)((DWORD)iPos * dx / iRange) + 1;
		rect1.right = x;
		rect2.left  = x;

		if (iPos < iRange)
			iPos++;
		wsprintf(rgch, "%3d%%", iPos * 100 / iRange);
		cch = lstrlen(rgch);
		GetTextExtentPoint(ps.hdc, rgch, cch, &size);

		clrBackGround = RGB(  0,   0, 255);
		clrForeGround = RGB(255, 255, 255);
		SetBkColor(ps.hdc, clrBackGround);
		SetTextColor(ps.hdc, clrForeGround);
		ExtTextOut(ps.hdc, (dx - size.cx) / 2, (rect1.bottom - size.cy) / 2,
			ETO_OPAQUE | ETO_CLIPPED, &rect1, rgch, cch, NULL);

		SetBkColor(ps.hdc, clrForeGround);
		SetTextColor(ps.hdc, clrBackGround);
		ExtTextOut(ps.hdc, (dx - size.cx) / 2, (rect1.bottom - size.cy) / 2,
			ETO_OPAQUE | ETO_CLIPPED, &rect2, rgch, cch, NULL);

		if ((HFONT)NULL != hfontSav)
			SelectObject(ps.hdc, hfontSav);
		EndPaint(hwnd, (LPPAINTSTRUCT)&ps);
		break;
		}
	return (LPARAM)0L;
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		window proc for the non-blinking text control that chops full path using
//		elipses if it won't fit in the window.
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CTextWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
		{
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	case WM_SETTEXT:
		DefWindowProc(hwnd, uMsg, wParam, lParam);
		InvalidateRect(hwnd, (LPRECT)NULL, TRUE);
		UpdateWindow(hwnd);
		break;
	case WM_ERASEBKGND:
		break;
	case WM_SETFONT:
		SetWindowLong(hwnd, ilFont, wParam);
		if (!lParam)
			break;
		InvalidateRect(hwnd, (LPRECT)NULL, TRUE);
		UpdateWindow(hwnd);
		break;
	case WM_PAINT:
		PAINTSTRUCT  ps;
		RECT rect;
		CHAR rgch[cbMaxSz];
		LONG cch;
		HFONT hfontCur;
		HFONT hfontSav;

		BeginPaint(hwnd, &ps);
		hfontCur = (HFONT)GetWindowLong(hwnd, ilFont);
		if ((HFONT)NULL != hfontCur)
			hfontSav = (HFONT)SelectObject(ps.hdc, hfontCur);
		else
			hfontSav = (HFONT)NULL;

		GetClientRect(hwnd, &rect);
		cch = GetWindowText(hwnd, rgch, sizeof (rgch));
		cch = CchChopText(rgch, rect.right - rect.left, ps.hdc, cch);

		SetBkColor(ps.hdc, GetSysColor(COLOR_BTNFACE));
		SetTextColor(ps.hdc, GetSysColor(COLOR_BTNTEXT));
		ExtTextOut(ps.hdc, 0, 0, ETO_OPAQUE, &rect, rgch, cch, NULL);

		if ((HFONT)NULL != hfontSav)
			SelectObject(ps.hdc, hfontSav);
		EndPaint(hwnd, &ps);
		break;
		}
	return (LPARAM)0L;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		'Chop' the text to fit specified width
//	Returns:
//		the hwnd to use
////////////////////////////////////////////////////////////////////////////////
LONG CchChopText(LPSTR sz, INT dx, HDC hdc, LONG cch)
{
	LPSTR	pch = sz;
	SIZE sSize;

	GetTextExtentPoint(hdc, pch, cch, &sSize);

	if ((WORD)dx < sSize.cx)
		{			// need to chop
		LPSTR	pchT;
		LPSTR	pchL;   // last slash
		LPSTR	pchLim; // limit of chopping

		pch[3] = '.';
		pch[4] = '.';
		pch[5] = '.';

		// Strategy: first chop whole nodes till you reach the
		// last one.  Then chop character by character.

		pchL = pch + cch;

		// We know that there is at least one slash in the path,
		// namely the one following the drive

		while (*--pchL != '\\')
			;

		if (pchL != pch + 2)	// at least 2 slashes
			{
			// need to chop whole nodes
			pchT = pch + 6;
			while (*pchT != '\\')
				pchT++;

			while (pchT <= pchL)
				{
				INT cchDec = (INT)(pchT - pch) - 6;

				MoveMemory(pch + 6, pchT, cch - (INT)(pchT - pch) + 1);
				cch -= cchDec;

				pchL -= cchDec;

				GetTextExtentPoint(hdc, pch, cch, &sSize);

				if ((WORD)dx >= sSize.cx)
					goto Done;

				// if not yet done, having eaten this whole
				// node, try one last thing: eat the slash,
				// before eating the next node

				pchT = pch + 7;

				if (*(pch + 6) == '\\')
					continue;

				if (pchT > pchL)
					break;

				while (*pchT != '\\')
					pchT++;
				}
			}
		else
			pchL += 4;

		// At this point, we must start the char by char chop
		pchLim = pch + cch - 3;	  // need at least 3 chars at end

		// Use the comma operator to make life easier
		while (GetTextExtentPoint(hdc, pch, cch, &sSize), (WORD)dx < sSize.cx)
			{
			MoveMemory(pchL, pchL + 1, cch - (INT)(pchL + 1 - pch) + 1);
			--cch;
			--pchLim;
			}
		}
Done:
	return cch;
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Determine what window to use as the dialog owner
//	Returns:
//		the hwnd to use
////////////////////////////////////////////////////////////////////////////////
HWND HwndGetDlgOwner(VOID)
{
	return _HwndTop(GetActiveWindow());
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		return the topmost ancestor of the given window
//	Arguments:
//		hwnd - the window of which to find the topmost ancestor
//	Returns:
//		the hwnd of the topmost ancestor
////////////////////////////////////////////////////////////////////////////////
static HWND _HwndTop(HWND hwnd)
{
	HWND hwndT;

	if ((HWND)NULL == hwnd)
		hwnd = HwndFrame();

	for (;;)
		{
		LONG lStyle;
		lStyle = GetWindowLong(hwnd, GWL_STYLE);
		if (lStyle & WS_CHILD)
			hwndT = GetParent(hwnd);
		else
			break;
		if ((HWND)NULL == hwndT)
			break;
		hwnd = hwndT;
		}
	return hwnd;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Centers the given window on its parent window (or owner window if hwnd
//		is a popup window)
//	Arguments:
//		hwnd - the window of which to find the topmost ancestor
////////////////////////////////////////////////////////////////////////////////
static VOID _CentreWindow(HWND hwnd)
{
	LONG lStyle;
	HWND hwndParent;
	HWND hwndOwner;
	RECT rect;
	RECT rectOwner;
	POINT pt;

	lStyle = GetWindowLong(hwnd, GWL_STYLE);
	if (lStyle & WS_CHILD)
		{
		hwndParent = GetParent(hwnd);
		hwndOwner = hwndParent;
		}
	else if (lStyle & WS_POPUP)
		{
		hwndParent = GetDesktopWindow();
		hwndOwner = GetWindow(hwnd, GW_OWNER);
		}
	else
		{
		hwndParent = GetDesktopWindow();
		hwndOwner = hwndParent;
		}
	GetWindowRect(hwnd, &rect);
	GetWindowRect(hwndOwner, &rectOwner);

	pt.x = rectOwner.left +
		((rectOwner.right - rectOwner.left) - (rect.right - rect.left)) / 2;
	pt.y = rectOwner.top +
		((rectOwner.bottom - rectOwner.top) - (rect.bottom - rect.top)) / 2;

	MapWindowPoints(HWND_DESKTOP, hwndParent, &pt, 1);
	SetWindowPos(hwnd, (HWND)NULL, pt.x, pt.y, 0, 0, (SWP_NOSIZE | SWP_NOZORDER));
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Uses GetDlgItemText to get the given path from the given
//		dialog control text, and adds a trailing back slash.
//	Arguments:
//		hdlg - handle to dialog.
//		idc - id of dialog control to be modified.
//		sz - buffer to receive the string
//		cb - size of the buffer
////////////////////////////////////////////////////////////////////////////////
VOID FetchIdcPath(HWND hdlg, int idc, LPSTR sz, LONG cb)
{
	CHAR *pchT;
	LONG cch;

	cch = GetDlgItemText(hdlg, idc, sz, cb);
	if (cch <= 3)
		return;
	if (sz[1] != ':')
			return;
	pchT = sz + cch - 1;
	if ('\\' != *pchT && cch < cb - 1)
		{
		*++pchT = '\\';
		*++pchT = '\0';
		}
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Uses _InsertIdcText to insert the given path into the given
//		dialog control text after stripping trailing back slash.
//	Arguments:
//		hdlg - handle to dialog.
//		idc - id of dialog control to be modified.
//		sz - string to be wsprintf'ed into the template string
////////////////////////////////////////////////////////////////////////////////
static VOID _InsertIdcPath(HWND hdlg, int idc, LPSTR sz)
{
	CHAR rgch[cbMaxSz];
	CHAR *pchSrc;
	CHAR *pchDst;

	pchDst = rgch;
	pchSrc = sz;
	while ('\0' != (*pchDst = *pchSrc))
		{
		pchDst++;
		pchSrc++;
		}
	if ('\\' == *--pchDst)
		{
		if (pchDst - rgch > 2)
			*pchDst = '\0';
		}
	_InsertIdcText(hdlg, idc, rgch);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Uses _InsertIdcText to insert the given path into the given
//		dialog control text after stripping trailing back slash.
//	Arguments:
//		hdlg - handle to dialog.
//		idc - id of dialog control to be modified.
//		sz - string to be wsprintf'ed into the template string
////////////////////////////////////////////////////////////////////////////////
VOID SetIdcPath(HWND hdlg, int idc, LPSTR sz)
{
	CHAR rgch[cbMaxSz];
	CHAR *pchSrc;
	CHAR *pchDst;

	pchDst = rgch;
	pchSrc = sz;
	while ('\0' != (*pchDst = *pchSrc))
		{
		pchDst++;
		pchSrc++;
		}
	if ('\\' == *--pchDst)
		{
		if (pchDst - rgch > 2)
			*pchDst = '\0';
		}
	SetDlgItemText(hdlg, idc, rgch);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Uses wsprintf to insert the given string into the given
//		dialog control text.
//	Arguments:
//		hdlg - handle to dialog.
//		idc - id of dialog control to be modified.
//		sz - string to be wsprintf'ed into the template string
////////////////////////////////////////////////////////////////////////////////
static VOID _InsertIdcText(HWND hdlg, int idc, LPSTR sz)
{
	CHAR rgch1[cbMaxSz];
	CHAR rgch2[cbMaxSz * 2];
	UINT cch;

	cch = GetDlgItemText(hdlg, idc, rgch1, sizeof rgch1);
	if (0 != cch)
		{
		wsprintf(rgch2, rgch1, sz);
		SetDlgItemText(hdlg, idc, rgch2);
		}
	else
		{
		SetDlgItemText(hdlg, idc, sz);
		}
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Uses wsprintf to insert the given long value into the given
//		dialog control text.
//	Arguments:
//		hdlg - handle to dialog.
//		idc - id of dialog control to be modified.
//		l - long value to be wsprintf'ed into the template string
////////////////////////////////////////////////////////////////////////////////
static VOID _InsertIdcVal(HWND hdlg, int idc, LONG l)
{
	CHAR rgch1[cbMaxSz];
	CHAR rgch2[cbMaxSz * 2];
	UINT cch;

	cch = GetDlgItemText(hdlg, idc, rgch1, sizeof rgch1);
	if (0 != cch)
		{
		wsprintf(rgch2, rgch1, l);
		}
	else
		{
		wsprintf(rgch2, "%ld", l);
		}
	SetDlgItemText(hdlg, idc, rgch2);
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		return the minimum tick count when the modeless dialog should end
////////////////////////////////////////////////////////////////////////////////
static DWORD _DwClicksModelessStop(VOID)
{
	DWORD dw;

	dw = GetTickCount();
	if (dw > 0xffffeeee)
		dw  = 0x1000;     /* 4 seconds */
	else
		dw += 0x1000;

	return dw;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		wait until the modeless dialog should end
////////////////////////////////////////////////////////////////////////////////
static VOID _WaitToEndModeless(DWORD dwClicks)
{
	if (dwClicks == 0x1000)
 		{
		DWORD dw;
		while ((dw = GetTickCount()) > 0xffffeeee || dw < 0x1000)
			;
		}
	else
		while (GetTickCount() < dwClicks)
			;
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		convert the count of bytes to the count of kilobytes
//	Arguments:
//		cb - count of bytes
//	Returns:
//		count of kilobytes
////////////////////////////////////////////////////////////////////////////////
static LONG _CkbOfCb(LONG cb)
{
	return (cb + 1023) / 1024;
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		load the string specified
//	Arguments:
//		ids - identifier of string to load
//		prgchIdsText - buffer to put string into
//		cbIdsText - number of bytes in prgchIdsText
//	Returns:
//		success or failure
////////////////////////////////////////////////////////////////////////////////
BOOL FLoadIds(LONG ids, CHAR *prgchIdsText, LONG cbIdsText)
{
	return LoadString(hinst, ids, prgchIdsText, cbIdsText) != 0;
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		put up a message box
//	Arguments:
//		hwnd - owner of message box
//		idsText - identifier of string for the message
//		uType - type of message box
//		pidRet - where to put the return value from MessageBox
//	Returns:
//		success or failure
////////////////////////////////////////////////////////////////////////////////
BOOL FMsgBox(HWND hwnd, LONG idsText, UINT uType, int *pidRet)
{
	CHAR rgchText[cbMaxSz];
	CHAR rgchCaption[cbMaxSz];

	if (FLoadIds(idsText, rgchText, sizeof rgchText) &&
		FLoadIds(IDS_MSGCAPTION, rgchCaption, sizeof rgchCaption))
		{
		*pidRet = MessageBox(hwnd, rgchText, rgchCaption, uType);
		return TRUE;
		}
	return FALSE;
}
*/
