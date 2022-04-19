////////////////////////////////////////////////////////////////////////////////
//	File:			GETPATH.CPP
//	Purpose:		Sample hook routines for replacing the acme getpath ui
////////////////////////////////////////////////////////////////////////////////
//  Requires: MPR.LIB (ships with VC2.x) for WNetGetConnection()



#include "ui-hook.h"


typedef enum
	{
	vpcBadChars,
	vpcOk,
	vpcDestIsSrc,
	vpcNotWritable,
	vpcNoDir,
	vpcAppExistsOld,
	vpcAppExistsNew,
	vpcNotEmpty
	}  VPC, *PVPC;


BOOL CALLBACK GetPathHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OKWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK LBWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


static VOID InitDialog(PGPDHD pgpdhd, HWND hdlg);
static BOOL FCommand(PGPDHD pgpdhd, HWND hdlg, WPARAM wParam, LPARAM lParam);
static VOID ListDrives(HWND hdlg);
static VOID ListDirs(HWND hdlg);
static BOOL FAddAncestors(HWND hwndList, CHAR *prgch);
static CHAR *PchScanCh(CHAR *prgch, LONG ch);
static BOOL FDirAndSubnodesWritable(CHAR *prgchPath, UINT cbMaxDir);
static VPC VpcValidDestPath(PGPDHD pgpdhd);
static BOOL FNewOrEmptyDir(CHAR *prgch);
static BOOL FVerifyPath(HWND hdlg, PGPDHD pgpdhd);
static CFV CfvCheckForExistingApp(CHAR *prgchDir, CHAR *prgchAppName, CHAR *prgchVer);
static VOID MeasureItem(HWND hdlg, PMEASUREITEMSTRUCT pmis);
static VOID DrawItem(HWND hdlg, WPARAM wParam, PDRAWITEMSTRUCT pdis);
static BOOL FLoadDirDriveBitmap(VOID);
static VOID DeleteDirDriveBitmap(VOID);
static HBITMAP HbmpLoadAndMapTransparent(LONG id, COLORREF rgbReplace, COLORREF rgbInstead);
static LONG TSignOfL(LONG nTest);
static LONG IbmpFromDriveType(LRESULT wDriveType);
static COLORREF RgbInvertRgb(COLORREF rgbOld);
static BOOL FInitBrowseDialog(VOID);
static VOID EndBrowseDialog(VOID);
static VOID InvalidPathDialog(HWND hwnd, CHAR *prgchIdsText, CHAR *prgchPath);
static BOOL FCreatePathDialog(HWND hwnd, CHAR *prgchPath);
static BOOL FOverwriteDialog(HWND hwnd, CHAR *prgchIdsText, CHAR *prgchPath, CHAR *prgchComponent);
static VOID ChopPathForMsg(SZ szPath);


static LONG idirSub;
static WNDPROC pLBProc = (WNDPROC)NULL;
static WNDPROC pOKProc = (WNDPROC)NULL;


static LONG rgfGetPath;
#define rgfGetPathNone				0x0000
#define rgfGetPathDirSelChanged     0x0001
#define rgfGetPathDriveDown         0x0002
#define rgfGetPathChangeDir			0x0004


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Puts up the sample customized get path hook dialog
//	Arguments:
//		pgpdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		shrcOk - proceed with new path
//		shrcCancel - proceed with old path
//		shrcFail - failure - abort setup
////////////////////////////////////////////////////////////////////////////////
SHRC DECL SHRCGetPathBrowseDialogHookProc(PGPDHD pgpdhd)
{
	HWND hwndT;
	WORD idd;
	int idc;

	if (pgpdhd->hd.cb < sizeof *pgpdhd)
		return shrcFail;
	if (!FInitBrowseDialog())
		return shrcFail;
	rgfGetPath = rgfGetPathNone;
	hwndT = HwndGetDlgOwner();
	//idd = (pgpdhd->rgchSrcDir[0] == '\0') ? IDD_CONSACMEGETPATH : IDD_CONSACMEGETPATHWITHSRC;
	
	  // sendak - only allow drive change - we force install to a particular path
	idd = IDD_CONSACMECHANGEPATH;
	
	idc = DialogBoxParam(hinst, MAKEINTRESOURCE(idd), hwndT, (DLGPROC)GetPathHookDlgProc, (LPARAM)pgpdhd);
	UpdateWindow(hwndT);
	EndBrowseDialog();

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
//		Dialog proc for the sample customized get path hook dialog
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK GetPathHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
		{
	case WM_INITDIALOG:
		InitDialog((PGPDHD)lParam, hdlg);
		break;
	case WM_COMMAND:
		return FCommand((PGPDHD)GetWindowLong(hdlg, DWL_USER), hdlg, wParam, lParam);
	case WM_MEASUREITEM:
		MeasureItem(hdlg, (PMEASUREITEMSTRUCT)lParam);
		return TRUE;
	case WM_DRAWITEM:
		DrawItem(hdlg, (WPARAM)wParam, (PDRAWITEMSTRUCT)lParam);
		return TRUE;
	}
	return GenericHookDlgProc(hdlg, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Handles WM_INITDIALOG message for get path dialog
//	Arguments:
//		pgpdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//		hdlg, wParam, lParam - see win32 api docs
////////////////////////////////////////////////////////////////////////////////
static VOID InitDialog(PGPDHD pgpdhd, HWND hdlg)
{
	CHAR rgch[cbMaxSz];
//	CHAR *prgch;

  /*
	pLBProc = (WNDPROC)GetWindowLong(GetDlgItem(hdlg, IDC_LIST1), GWL_WNDPROC);
	pOKProc = (WNDPROC)GetWindowLong(GetDlgItem(hdlg, IDOK), GWL_WNDPROC);
	SetWindowLong(GetDlgItem(hdlg, IDC_LIST1), GWL_WNDPROC, (LPARAM)LBWndProc);
	SetWindowLong(GetDlgItem(hdlg, IDOK), GWL_WNDPROC, (LPARAM)OKWndProc);
	SendDlgItemMessage(hdlg, IDC_EDIT1, EM_LIMITTEXT, (WPARAM)(cbMaxSz - 1), (LPARAM)0L);
	*/
	SendDlgItemMessage(hdlg, IDC_COMBO1, CB_SETEXTENDEDUI, (WPARAM)1, (LPARAM)0);

	lstrcpy(rgch, pgpdhd->rgchPath);

  if( (rgch[2] == '\\') &&    // if C:\ format
      (rgch[1] == ':') )
    {
    rgch[3] = '\0';
		SetCurrentDirectory(rgch);

    CharLower(rgch);
   	SetIdcPath(hdlg, IDC_EDIT1, rgch);
    }

	ListDrives(hdlg);
	//ListDirs(hdlg);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Handles WM_COMMAND message for get path dialog
//	Arguments:
//		pgpdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//		hdlg, wParam, lParam - see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
static BOOL FCommand(PGPDHD pgpdhd, HWND hdlg, WPARAM wParam, LPARAM lParam)
{
//	WORD idir;
//	WORD idirNew;
//	LONG cch;
	HWND hwndCombo;
	CHAR rgchPath[cbMaxSz];
	CHAR rgchDriveLabel[cbMaxSz];

	switch (LOWORD(wParam))
		{
  /*
	case IDC_CREATE:
		//SetIdcPath(hdlg, IDC_EDIT1, pgpdhd->rgchSrcDir);
		return TRUE;
	case IDOK:
		
		if ((rgfGetPath & rgfGetPathChangeDir) || (rgfGetPath & rgfGetPathDirSelChanged))
			{
			goto ChangingDir;
			}
		//FetchIdcPath(hdlg, IDC_EDIT1, pgpdhd->rgchPath, sizeof pgpdhd->rgchPath);
		if (!FVerifyPath(hdlg, pgpdhd))
			{
			SetFocus(GetDlgItem(hdlg, IDC_EDIT1));
			return TRUE;
			}
		break;
	case IDC_EDIT1:
		switch (HIWORD(wParam))
			{
		case EN_SETFOCUS:
			SendDlgItemMessage(hdlg, IDC_EDIT1, EM_SETSEL, 0, (LPARAM)0x7fff7fff);
			break;
			}
		break;
	case IDC_LIST1:
		switch (HIWORD(wParam))
			{
		case LBN_SELCHANGE:
			if ((WORD)SendDlgItemMessage(hdlg, IDC_LIST1, LB_GETCURSEL, 0, 0L) != idirSub - 1)
				{
				rgfGetPath |= rgfGetPathDirSelChanged;
				}
			else
				{
				rgfGetPath &= ~rgfGetPathDirSelChanged;
				}
			return TRUE;
		case LBN_KILLFOCUS:
			if (rgfGetPath & rgfGetPathDirSelChanged)
				{
				rgfGetPath &= ~rgfGetPathDirSelChanged;
				}
			else
				{
				rgfGetPath &= ~rgfGetPathChangeDir;
				}
			break;
		case LBN_SETFOCUS:
			EnableWindow(GetDlgItem(hdlg, IDOK), TRUE);
			SendMessage(GetDlgItem(hdlg, IDCANCEL), BM_SETSTYLE, (WPARAM)BS_PUSHBUTTON, (LPARAM)TRUE);
			break;
		case LBN_DBLCLK:
			{
ChangingDir:
			rgfGetPath &= ~(rgfGetPathChangeDir | rgfGetPathDirSelChanged);
			idirNew = (WORD)SendDlgItemMessage(hdlg, IDC_LIST1, LB_GETCURSEL, 0, 0L);
			*rgchPath = '\0';
			if (idirNew >= idirSub)
				{
				SendDlgItemMessage(hdlg, IDC_LIST1, LB_GETTEXT, (WPARAM)idirNew, (LPARAM)rgchPath);
				}
			else
				{
				cch = SendDlgItemMessage(hdlg, IDC_LIST1, LB_GETTEXT, 0, (LPARAM)rgchPath);
				for (idir = 1; idir <= idirNew; ++idir)
					{
					cch += SendDlgItemMessage(hdlg, IDC_LIST1, LB_GETTEXT, (WPARAM)idir, (LPARAM)&rgchPath[cch]);
					rgchPath[cch++] = '\\';
					}
				if (idirNew)
					{
					rgchPath[cch - 1] = 0;
					}
				}
			SetCurrentDirectory(rgchPath);
			GetCurrentDirectory(sizeof rgchPath, rgchPath);
			//SetIdcPath(hdlg, IDC_EDIT1, rgchPath);
			ListDirs(hdlg);
			return TRUE;
			}
		default:
			break;
			}
		break;
  */
	case IDC_COMBO1:
		switch (HIWORD(wParam))
			{
		case CBN_DROPDOWN:
			rgfGetPath |= rgfGetPathDriveDown;
			return TRUE;
			break;
		case CBN_SELCHANGE:
			if (rgfGetPath & rgfGetPathDriveDown)
				{
				return TRUE;
				}
			// fall through
		case CBN_CLOSEUP:
			hwndCombo = GetDlgItem(hdlg, IDC_COMBO1);
			rgfGetPath &= ~rgfGetPathDriveDown;
			SendMessage(hwndCombo, CB_GETLBTEXT, (WPARAM)SendMessage(hwndCombo, CB_GETCURSEL, 0, 0), (LPARAM)rgchDriveLabel);
			rgchDriveLabel[1] = ':';
			rgchDriveLabel[2] = '\\';
			rgchDriveLabel[3] = '\0';
			GetCurrentDirectory(cbMaxSz, rgchPath);
			CharLower(rgchDriveLabel);
			CharLower(rgchPath);
			if (rgchDriveLabel[0] != rgchPath[0])
				{
				SetCurrentDirectory(rgchDriveLabel);
				GetCurrentDirectory(sizeof rgchDriveLabel, rgchDriveLabel);
          // update the dialog display to show new destination
				SetIdcPath(hdlg, IDC_EDIT1, rgchDriveLabel);
          // update Acme's internal destination so main dialog shows change when user presses OK
        pgpdhd->rgchPath[0]=rgchDriveLabel[0];
				//ListDirs(hdlg);
				}
			break;
		default:
			break;
			}
		break;
	default:
		break;
		}
	return GenericHookDlgProc(hdlg, WM_COMMAND, wParam, lParam);
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		WndProc to subclass the OK button on the get path dialog to handle
//		different OK button behaviour when selection in directory listbox
//		has changed
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK OKWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_KILLFOCUS)
		{
		if (rgfGetPath & rgfGetPathChangeDir)
			{
			SendDlgItemMessage(GetParent(hwnd), IDC_LIST1, LB_SETCURSEL, (WPARAM)(idirSub - 1), 0L);
			rgfGetPath &= ~rgfGetPathChangeDir;
			}
		}
	return CallWindowProc((WNDPROC)pOKProc, hwnd, uMsg, wParam, lParam);
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		WndProc to subclass the directory listbox on the get path dialog to
//		handle different OK button behaviour when selection in directory listbox
//		has changed
//	Arguments:
//		see win32 api docs
//	Returns:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK LBWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hdlg;

	if (uMsg == WM_KILLFOCUS)
		{
		hdlg = GetParent(hwnd);

		if (GetDlgItem(hdlg, IDOK) == (HWND)wParam)
			rgfGetPath |= rgfGetPathChangeDir;
		else
			rgfGetPath &= ~rgfGetPathChangeDir;
		if (!(rgfGetPath & rgfGetPathChangeDir))
			{
			SendMessage(hwnd, LB_SETCURSEL, (WPARAM)(idirSub - 1), 0L);
			}
		}
	return CallWindowProc((WNDPROC)pLBProc, hwnd, uMsg, wParam, lParam);
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Fills the drive listbox with the drive information.
//	Arguments:
//		hdlg - dialog handle
////////////////////////////////////////////////////////////////////////////////
static VOID ListDrives(HWND hdlg)
{
	HWND hwndCombo;
	LONG i;
	UINT iCurrentDriveType;
	CHAR rgchBuf[cbMaxSz];
	CHAR rgchVolumeName[cbMaxSz];
	CHAR rgchCurDir[cbMaxSz];
	DWORD dw;
	LONG iDrv;

	hwndCombo = GetDlgItem(hdlg, IDC_COMBO1);
	SendMessage(hwndCombo, WM_SETREDRAW, FALSE, 0L);
	SendMessage(hwndCombo, CB_RESETCONTENT, 0, 0L);
	GetCurrentDirectory(cbMaxSz, rgchCurDir);
	CharLower(rgchCurDir);
	i = 0;
	dw = GetLogicalDrives();
	for (i = 0, iDrv = 0; iDrv < 26; iDrv++, dw >>= 1)
		{
		if ((dw & 1) == 0)
			{
			continue;
			}
		// Note: it is very important that the uppercase 'A' be used for
		// the drive letter in rgchBuf[0], as the Novell Netware driver
		// will GP Fault if you pass in a lowercase drive letter.
		rgchBuf[0] = (CHAR)(iDrv + 'A');
		rgchBuf[1] = ':';
		rgchBuf[2] = '\\';
		rgchBuf[3] = '\0';
		iCurrentDriveType = GetDriveType(rgchBuf);
		if ((iCurrentDriveType < 2) ||              // got an error
		    (iCurrentDriveType == DRIVE_CDROM))     // its a CD-Rom drive - can't write to it
			{
			continue;
			}
		if (iCurrentDriveType != DRIVE_REMOVABLE)   // Fixed, Remote or RamDisk
			{
			if (iCurrentDriveType != DRIVE_REMOTE)    // Fixed or RamDisk
				{
				if (!GetVolumeInformation(rgchBuf, rgchVolumeName, cbMaxSz, (PDWORD)NULL, (PDWORD)NULL, (PDWORD)NULL, (PTSTR)NULL, 0))
					{
					continue;
					}
  			rgchBuf[2] = ' ';
  			lstrcpy(rgchBuf + 3, rgchVolumeName);
				}
			else                                      // Remote (net connection mapped to a drive letter)
				{
      	DWORD dwSel, 
      	      dwRet;

          // we must pass C: not C:\ to WNetGetConnection()
				rgchBuf[2] = '\0';

				dwSel = cbMaxSz-1;
        dwRet = WNetGetConnection(rgchBuf, rgchVolumeName, &dwSel);
				if (dwRet != NO_ERROR)
					{
          #ifdef _DEBUG
            wsprintf(rgchVolumeName, "Error getting connection %lu", dwRet);
            MessageBox(NULL, rgchVolumeName, rgchBuf, MB_OK);
          #endif
					continue;
					}
        /* we could do this but what happens if they don't have write permission
           on the root directory but they do on \program files\microsoft kids ?
    		rgchBuf[2] = '\\';
        if(!IsDirWritable(rgchBuf))             // If user doesn't have write permission on drive...
					{
					continue;                             // then don't display this drive letter
					}
        */
    		rgchBuf[2] = ' ';
    		lstrcpy(rgchBuf + 3, rgchVolumeName);
				}
			}
		CharLower(rgchBuf);
		SendMessage(hwndCombo, CB_INSERTSTRING, (WPARAM)i, (LPARAM)rgchBuf);
		SendMessage(hwndCombo, CB_SETITEMDATA, (WPARAM)i, (LPARAM)iCurrentDriveType);
		if (rgchBuf[0] == rgchCurDir[0])
			{
			SendMessage(hwndCombo, CB_SETCURSEL, (WPARAM)i, (LPARAM)0L);
			}
		i++;
		}
	SendMessage(hwndCombo, WM_SETREDRAW, (WPARAM)TRUE, 0L);
}


/***********************************************************
  The following is from the ACME95 source code - dirdlg.cpp
 ***********************************************************
STATIC_FN VOID PRIVATE ListDrives ( HWND hDlg, WORD cmb )
{
	short nDrvCount, i;
	char  rgchBuf[_MAX_PATH];
	char  rgchDrvNum[27];
	HWND  hCmb = GetDlgItem(hDlg, cmb);
	WORD  iCurrentDrive, iCurrentDriveType;

	wNoRedraw |= 2;
	SendMessage(hCmb, WM_SETREDRAW, fFalse, 0L);
	SendMessage(hCmb, CB_RESETCONTENT, 0, 0L);

	nDrvCount = FindValidDrives(rgchDrvNum);

	for(i = 0; i < nDrvCount; i++)
		{

		iCurrentDrive = (WORD)rgchDrvNum[i+1];

		// Note: it is very important that the uppercase 'A' be used for
		// the drive letter in rgchBuf[0], as the Novell Netware driver
		// will GP Fault if you pass in a lowercase drive letter.
		//
		rgchBuf[0] = (char) (iCurrentDrive + (WORD)'A');
		rgchBuf[1] = ':';
		rgchBuf[2] = '\0';

		iCurrentDriveType =
				(WORD)GetDriveType(DriveNumToRootPath(iCurrentDrive));
		if (iCurrentDriveType < 2)	// Is it a phantom?  Skip it!
			continue;

		if (iCurrentDriveType != DRIVE_REMOVABLE &&
			iCurrentDriveType != DRIVE_CDROM)
			{
			if (iCurrentDriveType != DRIVE_REMOTE)
				{
				char rgchVolumeName[_MAX_PATH];
				rgchBuf[2] = '\\';
				rgchBuf[3] = '\0';
				if (!GetVolumeInformation(rgchBuf, rgchVolumeName, _MAX_PATH,
												NULL, NULL, NULL, NULL, 0))
					{
					continue;
					}
				lstrcpy(rgchBuf+3, rgchVolumeName);
				}
			else
				{
				DWORD dwSel = _MAX_PATH-3;
				if (WNetGetConnection(rgchBuf, rgchBuf+3, &dwSel) != NO_ERROR)
					{
					DisplayAssertMsg();
					continue;
					}
				}

			rgchBuf[2] = ' ';
			}

#ifdef DBCS		// [J3]
		AnsiLower((LPSTR)rgchBuf);
#else
		StringLower((LPSTR)rgchBuf);
#endif
		LRESULT lresult;

		while ((lresult = SendMessage(hCmb, CB_INSERTSTRING, (WPARAM) -1,
					(LPARAM)(LPSTR)rgchBuf)) == CB_ERRSPACE)
			{
			if (!FHandleOOM())
				{
				break;
				}
			}
		if (lresult == CB_ERR || lresult == CB_ERRSPACE)
			{
			DebugMsgBox("SendMessage 1 failed in dirdlg.cpp", "Setup Message");
			continue;
			}
		lresult = SendMessage(hCmb, CB_SETITEMDATA, (WPARAM) lresult,
			(LPARAM)(DWORD) (GetDriveIndex(iCurrentDrive, iCurrentDriveType)));
		if (lresult == CB_ERR)
			{
			DebugMsgBox("SendMessage 2 failed in dirdlg.cpp", "Setup Message");
			continue;
			}
		if (iCurrentDrive == (WORD) rgchDrvNum[0])
			SendMessage(hCmb, CB_SETCURSEL, (WPARAM) i, 0);

		}

	wNoRedraw &= ~2;
	SendMessage(hCmb, WM_SETREDRAW, (WPARAM)fTrue, 0L);
	ChangeDrive((char)(rgchDrvNum[0] + 'a'));
***********************************************************/



/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Fills the directory listbox with current directory path, and the
//		subdirectories of the current directory.
//	Arguments:
//		hdlg - dialog handle
////////////////////////////////////////////////////////////////////////////////
static VOID ListDirs(HWND hdlg)
{
	LONG i;
	HWND hwndList;
	RECT rc;
	HANDLE h;
	WIN32_FIND_DATA w32fd;
	static CHAR rgchStarDotStar[12] = "*.*";
	CHAR rgch[cbMaxSz];

	hwndList = GetDlgItem(hdlg, IDC_LIST1);
	SendMessage(hwndList, WM_SETREDRAW, FALSE, 0L);
	SendMessage(hwndList, LB_RESETCONTENT, 0, 0L);
	h = FindFirstFile(rgchStarDotStar, &w32fd);
	if (INVALID_HANDLE_VALUE == h)
		{
		goto RedrawList;
		}
	do {
		if (w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			if (w32fd.cFileName[0] == '.')
				{
				continue;
				}
			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)w32fd.cFileName);
			}
		}
	while (FindNextFile(h, &w32fd));
	FindClose(h);
	GetCurrentDirectory(sizeof rgch, rgch);
	FAddAncestors(hwndList, rgch);
	SendMessage(hwndList, LB_SETCURSEL, idirSub - 1, 0L);
	i = idirSub - 2;
	if (i < 0)
		{
		i = 0;
		}
	SendMessage(hwndList, LB_SETTOPINDEX, i, 0L);
RedrawList:
	SendMessage(hwndList, WM_SETREDRAW, TRUE, 0L);
	GetWindowRect(hwndList, &rc);
	InflateRect(&rc, -1, -1);
	ScreenToClient(hdlg, (PPOINT)&(rc.left));
	ScreenToClient(hdlg, (PPOINT)&(rc.right));
	InvalidateRect(hdlg, &rc, (BOOL)FALSE);
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Adds the current directory path to the directory listbox
//	Arguments:
//		hwndList - window handle of listbox
//		prgch - current directory path
////////////////////////////////////////////////////////////////////////////////
static BOOL FAddAncestors(HWND hwndList, CHAR *prgch)
{
	CHAR *pchB;
	CHAR *pchF;
	CHAR chT;

	idirSub = 0;
	pchF = prgch;
	CharLower(prgch);
	pchB = PchScanCh(pchF, '\\');
	if (!pchB)
		{
		return FALSE;
		}
	chT = *++pchB;
	*pchB = 0;
	SendMessage(hwndList, LB_INSERTSTRING, idirSub++, (LPARAM)pchF);
	*pchB = chT;
	while (pchB && *pchB)
		{
		pchF = pchB;
		pchB = PchScanCh(pchF, '\\');
		if (pchB)
			{
			*pchB++ = 0;
			}
		SendMessage(hwndList, LB_INSERTSTRING, idirSub++, (LPARAM)pchF);
		}
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		scans for a character in a string
//	Arguments:
//		prgch - string to scan in
//		ch - charcter to scan for
//	Returns:
//		pointer to the next occurence of the character in the string if it exists,
//		else null pointer
////////////////////////////////////////////////////////////////////////////////
static CHAR *PchScanCh(CHAR *prgch, LONG ch)
{
	while (*prgch)
		{
		if (ch == *prgch)
			{
			return prgch;
			}
		prgch = CharNext(prgch);
		}
	return (CHAR *)NULL;
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Checks the validity of the path
//	Arguments:
//		pgpdhd = routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		vpcOk - path is okay
//		vpcNotEmpty - directory is not empty
//		vpcDestIsSrc - the directory matches the source directory
//		vpcNotWritable - the destination is not writable
//		vpcNoDir - the directory does not exist
//		vpcAppExistsOld - an older version of the app exists in the directory
//		vpcAppExistsNew - a current/newer version of the app exists
////////////////////////////////////////////////////////////////////////////////
static VPC VpcValidDestPath(PGPDHD pgpdhd)
{
	static BOOL fPathChecked = FALSE;
	static char rgchSav[cbMaxSz];
	CFV cfv;

	if (fPathChecked)
		{
		if (lstrcmp(pgpdhd->rgchPath, rgchSav) == 0)
			{
			return vpcOk;
			}
		fPathChecked = FALSE;
		rgchSav[0] = '\0';
		}
	if (lstrcmpi(pgpdhd->rgchPath, pgpdhd->rgchSrcDir) == 0)
		{
		return vpcDestIsSrc;
		}
	if (!FDirAndSubnodesWritable(pgpdhd->rgchPath, pgpdhd->cSubNodeMax))
		{
		return vpcNotWritable;
		}
	if (!DoesDirExist(pgpdhd->rgchPath))
		{
		return vpcNoDir;
		}
	else if (*pgpdhd->rgchFilename != '\0')
		{
		cfv = CfvCheckForExistingApp(pgpdhd->rgchPath, pgpdhd->rgchFilename, pgpdhd->rgchVersion);
		if (cfv != cfvNoFile)
			{
			return (cfv == cfvLoVer)? vpcAppExistsOld : vpcAppExistsNew;
			}
		}
	if (pgpdhd->fNeedNewEmptyDir && !FNewOrEmptyDir(pgpdhd->rgchPath))
		{
		return vpcNotEmpty;
		}
	fPathChecked = TRUE;
	lstrcpy(rgchSav, pgpdhd->rgchPath);
	return vpcOk;
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		determines if the directory is writable and the path won't be too long
//		for the sub directories
//	Arguments:
//		prgchPath - the directory path
//		cbMaxDir - the maximum number of path bytes needed below this dir
//	Returns:
//		whether the directory path is writable and supports the subdirectories
////////////////////////////////////////////////////////////////////////////////
static BOOL FDirAndSubnodesWritable(CHAR *prgchPath, UINT cbMaxDir)
{
	if (*prgchPath == '\0' || (CbStrLen(prgchPath) + cbMaxDir)  > (MAX_PATH-2))
		return (fFalse);	// Path empty or too long.

	return (IsDirWritable(prgchPath));
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		determines if the directory is new or empty
//	Arguments:
//		prgchPath - the directory path
//	Returns:
//		whether the directory is new or empty
////////////////////////////////////////////////////////////////////////////////
static BOOL FNewOrEmptyDir(CHAR *prgch)
{
	CHAR rgchDir[cbMaxSz];
	LONG cch;
	HANDLE h;
	WIN32_FIND_DATA w32fd;
	BOOL fRet;

	if (!FValidDir(prgch) || DoesFileExist(prgch, femExists))
		return FALSE;

	if (!DoesDirExist(prgch))
		return TRUE;

	lstrcpy(rgchDir, prgch);
	cch = lstrlen(rgchDir);
	if (rgchDir[cch - 1] != '\\')
		{
		rgchDir[cch++] = '\\';
		}
	lstrcpy(rgchDir + cch, "*.*");

	h = FindFirstFile(rgchDir, &w32fd);
	if (INVALID_HANDLE_VALUE == h)
		{
		return TRUE;
		}
	fRet = TRUE;
	do {
		if (w32fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			if (w32fd.cFileName[0] != '.')
				{
				fRet = FALSE;
				break;
				}
			}
		else
			{
			fRet = FALSE;
			break;
			}
		}
	while (FindNextFile(h, &w32fd));
	FindClose(h);
	return fRet;
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		determines if an existing copy of an app exists in the directory
//	Arguments:
//		prgchPath - the directory path
//		prgchAppName - the name of the application
//		prgchVer - the version info of the application being installed
//	Returns:
//		cfvNoFile - the application does not exist in the directory
//		cfvLoVer - an older version exists
//		cfvHiVer - a newer version exists
//		cfvEqVer - the same version exists
////////////////////////////////////////////////////////////////////////////////
static CFV CfvCheckForExistingApp(CHAR *prgchDir, CHAR *prgchAppName, CHAR *prgchVer)
{
	CHAR rgch[cbMaxSz];
	CHAR rgchCurVer[cbMaxSz];
	CFV cfv;

	cfv = cfvNoFile;
	lstrcpy(rgch, prgchDir);
	lstrcat(rgch, prgchAppName);
	if (DoesFileExist(rgch, femExists))
		{
		if (*prgchVer == '\0')
			{
			return cfvLoVer;
			}
		GetVersionOfFile(rgch, rgchCurVer, sizeof rgchCurVer);
		switch (CompareFileVersions(prgchVer, rgchCurVer))
			{
		case -1:
			cfv = cfvHiVer;
			break;
		case 0:
			cfv = cfvEqVer;
			break;
		case 1:
			cfv = cfvLoVer;
			break;
		default:
			break;
			}
		}
	return cfv;
}
*/


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		validates the path and checks with the user on whether to proceed
//	Arguments:
//		hdlg - dialog handle
//		pgpdhd - routine parameters and ACME global variables that can be
//		referenced/updated by this procedure
//	Returns:
//		whether or not to proceed with path, or allow new path to be input
////////////////////////////////////////////////////////////////////////////////
static BOOL FVerifyPath(HWND hdlg, PGPDHD pgpdhd)
{
	VPC vpc;
	vpc = VpcValidDestPath(pgpdhd);
	CHAR rgchIdsText[cbMaxSz];
	BOOL fRet;

	fRet = FALSE;
	switch (vpc)
		{
	case vpcOk:
		fRet = TRUE;
		break;
	case vpcDestIsSrc:
		if (FLoadIds(IDS_DESTISSRC, rgchIdsText, sizeof rgchIdsText))
			{
			InvalidPathDialog(hdlg, rgchIdsText, pgpdhd->rgchPath);
			}
		break;
	case vpcNotWritable:
		if (FLoadIds(IDS_INVALIDPATH, rgchIdsText, sizeof rgchIdsText))
			{
			InvalidPathDialog(hdlg, rgchIdsText, pgpdhd->rgchPath);
			}
		break;
	case vpcNoDir:
		if (FCreatePathDialog(hdlg, pgpdhd->rgchPath))
			{
			fRet = TRUE;
			}
		break;

	case vpcNotEmpty:
    // WSproule: this used to have its own dialog which said when using the /a
    //             switch (admin mode) you must select an empty directory.
    //             We'll fall through to vpcAppExistsOld

		//DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_CONSACMEADMINNEWDIR), hdlg,
		//	(DLGPROC)GenericHookDlgProc, (LPARAM)pgpdhd);
		//UpdateWindow(hdlg);
		//break;

	case vpcAppExistsOld:
		if (FLoadIds(IDS_OVERWRITEOLD, rgchIdsText, sizeof rgchIdsText))
			{
			if (FOverwriteDialog(hdlg, rgchIdsText, pgpdhd->rgchPath, pgpdhd->rgchComponent))
				{
				fRet = TRUE;
				}
			}
		break;
	case vpcAppExistsNew:
		if (FLoadIds(IDS_OVERWRITENEW, rgchIdsText, sizeof rgchIdsText))
			{
			if (FOverwriteDialog(hdlg, rgchIdsText, pgpdhd->rgchPath, pgpdhd->rgchComponent))
				{
				fRet = TRUE;
				}
			}
		break;
	case vpcBadChars:
		if (FLoadIds(IDS_BADCHARS, rgchIdsText, sizeof rgchIdsText))
			{
			InvalidPathDialog(hdlg, rgchIdsText, pgpdhd->rgchPath);
			}
		break;
	default:
		break;
		}
	return fRet;
}
*/


////////////////////////////////////////////////////////////////////////////////
// Drawing code
////////////////////////////////////////////////////////////////////////////////

static WORD dyDirDrive = 0;
static HDC hdcMemory = (HDC)NULL;
static HBITMAP hbmpOrigMemBmp = (HBITMAP)NULL;
static HBITMAP hbmpDirDrive = (HBITMAP)NULL;
static COLORREF rgbWindowColor = 0xFF000000;
static COLORREF rgbHiliteColor = 0xFF000000;
static COLORREF rgbWindowText = 0xFF000000;
static COLORREF rgbHiliteText = 0xFF000000;
static COLORREF rgbGrayText = 0xFF000000;
static COLORREF rgbDDWindow = 0xFF000000;
static COLORREF rgbDDHilite = 0xFF000000;
static WORD dxDirDrive = 0;
#define rgbSolidBlue    0x00FF0000
#define dxSpace			4

#define BMPHIOFFSET 8
#define OPENDIRBMP 0
#define CURDIRBMP 1
#define STDDIRBMP 2
#define FLOPPYBMP 3
#define HARDDRVBMP 4
#define CDDRVBMP 5
#define NETDRVBMP 6
#define RAMDRVBMP 7


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		determines the height of the items in the list in response to a
//		WM_MEASUREITEM message
//	Arguments:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
static VOID MeasureItem(HWND hdlg, PMEASUREITEMSTRUCT pmis)
{
	static LONG dyItem = 0;
	HWND hwnd;
	HFONT hfont;
	HDC hdc;
	TEXTMETRIC tm;

	if (0 == dyItem)
		{
		hwnd = GetDlgItem(hdlg, pmis->CtlID);
		hfont = (HFONT)SendMessage(hwnd, WM_GETFONT, 0, 0L);
		hdc = GetDC(hwnd);
		if ((HFONT)NULL != hfont)
			{
			hfont = (HFONT)SelectObject(hdc, hfont);
			}
		GetTextMetrics(hdc, &tm);
		if ((HFONT)NULL != hfont)
			{
			SelectObject(hdc, hfont);
			}
		ReleaseDC(hwnd, hdc);
		dyItem = max(dyDirDrive, tm.tmHeight);
		}
	pmis->itemHeight = dyItem;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		returns the sub-bitmap index for the bitmap to use for the given
//		drive type
//	Arguments:
//		wDriveType - as returned by GetDriveType
//	Returns:
//		the sub-bitmap index
////////////////////////////////////////////////////////////////////////////////
static LONG IbmpFromDriveType(LRESULT wDriveType)
{
	switch (wDriveType)
		{
	case 1:
		return 0;
	case DRIVE_CDROM:
		return CDDRVBMP;
	case DRIVE_REMOVABLE:
		return FLOPPYBMP;
	case DRIVE_REMOTE:
		return NETDRVBMP;
	case DRIVE_RAMDISK:
		return RAMDRVBMP;
		}
	return HARDDRVBMP;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		initialize the global variables used by the drawing code
//	Returns:
//		whether or not successfully initialized
////////////////////////////////////////////////////////////////////////////////
static BOOL FInitBrowseDialog(VOID)
{
	HDC hdcScreen;
	HBITMAP hbmpTemp;

	hdcScreen = GetDC((HWND)NULL);
	if (!hdcScreen)
		{
		goto CantInit;
		}
	hdcMemory = CreateCompatibleDC(hdcScreen);
	if ((HDC)NULL == hdcMemory)
		{
		goto ReleaseScreenDC;
		}
	hbmpTemp = CreateCompatibleBitmap(hdcMemory, 1, 1);
	if ((HBITMAP)NULL == hbmpTemp)
		{
		goto ReleaseMemDC;
		}
	hbmpOrigMemBmp = (HBITMAP)SelectObject(hdcMemory, hbmpTemp);
	if (!hbmpOrigMemBmp)
		{
ReleaseMemDC:
		DeleteDC(hdcMemory);
ReleaseScreenDC:
		ReleaseDC((HWND)NULL, hdcScreen);
CantInit:
		return FALSE;
		}
	SelectObject(hdcMemory, hbmpOrigMemBmp);
	DeleteObject(hbmpTemp);
	ReleaseDC((HWND)NULL, hdcScreen);
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		cleans up the global variables used by the drawing code
////////////////////////////////////////////////////////////////////////////////
static VOID EndBrowseDialog(VOID)
{
	SelectObject(hdcMemory, hbmpOrigMemBmp);
	if (hdcMemory)
		{
		DeleteDC(hdcMemory);
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		draws the item in the list in response to a	WM_DRAWITEM message
//	Arguments:
//		see win32 api docs
////////////////////////////////////////////////////////////////////////////////
static VOID DrawItem(HWND hdlg, WPARAM wParam, PDRAWITEMSTRUCT pdis)
{
	HDC hdcList;
	RECT rectHilite;
	CHAR rgchText[cbMaxSz];
	LONG dxAcross;
	LONG nHeight;
	COLORREF rgbBack;
	COLORREF rgbText;
	COLORREF rgbOldBack;
	COLORREF rgbOldText;
	LONG nShift;
	BOOL bSel;
	LONG BltItem;
	TEXTMETRIC tm;

	dxAcross = 0;
	nShift = 1;
	BltItem = 1;
	*rgchText = 0;
	if (pdis->CtlID != IDC_LIST1 && pdis->CtlID != IDC_COMBO1)
		{
		return;
		}
	hdcList = pdis->hDC;
	LONG cTm;
	if (pdis->CtlID == IDC_COMBO1)
		{
		cTm = SendDlgItemMessage(hdlg, pdis->CtlID, CB_GETCOUNT, 0, 0L);
		}
	SendDlgItemMessage(hdlg, pdis->CtlID, ((pdis->CtlID == IDC_LIST1)? LB_GETTEXT : CB_GETLBTEXT), (WPARAM)pdis->itemID, (LPARAM)rgchText);
	if (*rgchText == 0)
		{
		DefWindowProc(hdlg, WM_DRAWITEM, wParam, (LPARAM)pdis);
		return;
		}
	rgbWindowColor = GetSysColor(COLOR_WINDOW);
	rgbHiliteColor = GetSysColor(COLOR_HIGHLIGHT);
	rgbWindowText = GetSysColor(COLOR_WINDOWTEXT);
	rgbHiliteText = GetSysColor(COLOR_HIGHLIGHTTEXT);
	rgbGrayText = GetSysColor(COLOR_GRAYTEXT);
	FLoadDirDriveBitmap();
	CharLower(rgchText);
	nHeight = pdis->rcItem.bottom - pdis->rcItem.top;
	CopyRect(&rectHilite, &pdis->rcItem);
	rectHilite.bottom = rectHilite.top + nHeight;
	bSel = (pdis->itemState & (ODS_SELECTED | ODS_FOCUS));
	if ((bSel & ODS_SELECTED) && ((pdis->CtlID != IDC_LIST1) || (bSel & ODS_FOCUS)))
		{
		rgbBack = rgbHiliteColor;
		rgbText = rgbHiliteText;
		}
	else
		{
		rgbBack = rgbWindowColor;
		rgbText = rgbWindowText;
		}
	rgbOldBack = SetBkColor(hdcList, rgbBack);
	rgbOldText = SetTextColor(hdcList, rgbText);
	if (pdis->CtlID == IDC_COMBO1)
		{
		dxAcross = dxDirDrive / BMPHIOFFSET;
		BltItem = IbmpFromDriveType(SendDlgItemMessage(hdlg, IDC_COMBO1, CB_GETITEMDATA, (WPARAM)pdis->itemID, 0));
		if (bSel & ODS_SELECTED)
			{
			BltItem += BMPHIOFFSET;
			}
		}
	else if (pdis->CtlID == IDC_LIST1)
		{
		dxAcross = dxDirDrive / BMPHIOFFSET;
		if ((LONG)pdis->itemID > idirSub)
			{
			nShift = idirSub;
			}
		else
			{
			nShift = pdis->itemID;
			}
		nShift++;
		BltItem = 1 + TSignOfL(pdis->itemID + 1 - idirSub);
		if (bSel & ODS_FOCUS)
			{
			BltItem += BMPHIOFFSET;
			}
		}
	GetTextMetrics(hdcList, &tm);
	ExtTextOut(hdcList, rectHilite.left + dxSpace + dxAcross + dxSpace * nShift, rectHilite.top + (nHeight - tm.tmHeight) / 2, ETO_OPAQUE | ETO_CLIPPED, &rectHilite, rgchText, lstrlen(rgchText), (CONST INT *)NULL);
	BitBlt(hdcList, rectHilite.left+dxSpace*nShift, rectHilite.top + (nHeight - dyDirDrive)/2, dxAcross, dyDirDrive, hdcMemory, BltItem*dxAcross, 0, SRCCOPY);
	SetTextColor(hdcList, rgbOldText);
	SetBkColor(hdcList, rgbOldBack);
	if (pdis->itemState & ODS_FOCUS)
		{
		DrawFocusRect(hdcList, &pdis->rcItem);
		}
	DeleteDirDriveBitmap();
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		loads the directory/drive bitmap, and does the background colour
//		translation to retain a bitmap containing two copies of the original.
//		one is for drawing unhighlighted entries, and one for highlighted entries
//	Returns:
//		whether or not successfully initialized
////////////////////////////////////////////////////////////////////////////////
static BOOL FLoadDirDriveBitmap(VOID)
{
	BITMAP bmp;
	HBITMAP hbmp;
	HBITMAP hbmpOrig;
	HDC hdcTemp;
	BOOL fWorked;

	fWorked = FALSE;
	if ((hbmpDirDrive != (HBITMAP)NULL) && (rgbWindowColor == rgbDDWindow) && (rgbHiliteColor == rgbDDHilite))
		{
		if (SelectObject(hdcMemory, hbmpDirDrive))
			{
			return TRUE;
			}
		}
	DeleteDirDriveBitmap();
	rgbDDWindow = rgbWindowColor;
	rgbDDHilite = rgbHiliteColor;
	hdcTemp = CreateCompatibleDC(hdcMemory);
	if (!hdcTemp)
		{
		goto LoadExit;
		}
	hbmp = HbmpLoadAndMapTransparent(IDB_DIRDRIVE, rgbSolidBlue, rgbWindowColor);
	if (!hbmp)
		{
		goto DeleteTempDC;
		}
	GetObject(hbmp, sizeof(BITMAP), &bmp);
	dyDirDrive = (WORD)bmp.bmHeight;
	dxDirDrive = (WORD)bmp.bmWidth;
	hbmpOrig = (HBITMAP)SelectObject(hdcTemp, hbmp);
	hbmpDirDrive = CreateDiscardableBitmap(hdcTemp, dxDirDrive * 2, dyDirDrive);
	if (!hbmpDirDrive)
		{
		goto DeleteTempBmp;
		}
	if (!SelectObject(hdcMemory, hbmpDirDrive))
		{
		DeleteDirDriveBitmap();
		goto DeleteTempBmp;
		}
	BitBlt(hdcMemory, 0, 0, dxDirDrive, dyDirDrive, hdcTemp, 0, 0, SRCCOPY);
	SelectObject(hdcTemp, hbmpOrig);
	DeleteObject(hbmp);
	hbmp = HbmpLoadAndMapTransparent(IDB_DIRDRIVE, rgbSolidBlue, rgbHiliteColor);
	if (!hbmp)
		{
		goto DeleteTempDC;
		}
	hbmpOrig = (HBITMAP)SelectObject(hdcTemp, hbmp);
	BitBlt(hdcMemory, dxDirDrive, 0, dxDirDrive, dyDirDrive, hdcTemp, 0, 0, SRCCOPY);
	SelectObject(hdcTemp, hbmpOrig);
	fWorked = TRUE;
DeleteTempBmp:
	DeleteObject(hbmp);
DeleteTempDC:
	DeleteDC(hdcTemp);
LoadExit:
	return fWorked;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		deletes the directory/drive bitmap
////////////////////////////////////////////////////////////////////////////////
static VOID DeleteDirDriveBitmap(VOID)
{
	if (hbmpOrigMemBmp)
		{
		SelectObject(hdcMemory, hbmpOrigMemBmp);
		if (hbmpDirDrive != (HBITMAP)NULL)
			{
			DeleteObject(hbmpDirDrive);
			hbmpDirDrive = (HBITMAP)NULL;
			}
		}
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		loads a bitmap and does colour translation
//	Arguments:
//		id - identifier of bitmap resource to load
//		rgbReplace - colour to be replaced
//		rebInstead - colour to replace rgbReplace
//	Returns:
//		if successful, returns a bitmap handle for the resulting bitmap,
//		otherwise it returns null
////////////////////////////////////////////////////////////////////////////////
static HBITMAP HbmpLoadAndMapTransparent(LONG id, COLORREF rgbReplace, COLORREF rgbInstead)
{
	PBITMAPINFOHEADER qbihInfo;
	HDC hdcScreen;
	BOOL fFound;
	LONG iFoundIndex;
	HRSRC hresLoad;
	HRSRC hres;
	LPCOLORREF prgb;
	PBYTE qbBits;
	HBITMAP hbmp;
	LONG i;

	iFoundIndex = -1;
	hbmp = (HBITMAP)NULL;
	hresLoad = FindResource(hinst, MAKEINTRESOURCE(id), RT_BITMAP);
	if (hresLoad == (HRSRC)NULL)
		{
		return (HBITMAP)NULL;
		}
	hres = (HRSRC)LoadResource(hinst, hresLoad);
	if (hres == (HRSRC)NULL)
		{
		return (HBITMAP)NULL;
		}
	rgbReplace = RgbInvertRgb(rgbReplace);
	rgbInstead = RgbInvertRgb(rgbInstead);
	qbihInfo = (PBITMAPINFOHEADER)LockResource(hres);
	prgb = (LPCOLORREF)((PSTR)qbihInfo + qbihInfo->biSize);
	fFound = FALSE;
	for (i=0; !fFound && i < (1 << (qbihInfo->biBitCount)); i++)
		{
		if (*prgb == rgbReplace)
			{
			fFound = TRUE;
			*prgb = rgbInstead;
			iFoundIndex = i;
			}
		prgb++;
		}
	qbihInfo = (PBITMAPINFOHEADER)LockResource(hres);
	qbBits = (PBYTE)(qbihInfo + 1);
	qbBits += (1 << (qbihInfo->biBitCount))* sizeof(RGBQUAD);
	hdcScreen = GetDC((HWND)NULL);
	if (hdcScreen != (HDC)NULL)
		{
		hbmp = CreateDIBitmap(hdcScreen, qbihInfo, (LONG)CBM_INIT, qbBits, (PBITMAPINFO)qbihInfo, DIB_RGB_COLORS);
		ReleaseDC((HWND)NULL, hdcScreen);
		}
	if (fFound)
		{
		((LPCOLORREF)((PSTR)qbihInfo + qbihInfo->biSize))[iFoundIndex] = rgbReplace;
		}
	FreeResource(hres);
	return hbmp;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		returns the sign of a number
//	Arguments:
//		nTest - number to test
//	Returns:
//		-1 - nTest is less then zero
//		1 - nTest is greater then zero,
//		0 - nTest is zero
////////////////////////////////////////////////////////////////////////////////
static LONG TSignOfL(LONG nTest)
{
	if (nTest < 0)
		{
		return -1;
		}
	if (nTest > 0)
		{
		return 1;
		}
	return 0;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		inverts an rgb value for highlighting
//	Arguments:
//		rgbOld - rgb value to invert
//	Returns:
//		invert of rgbOld
////////////////////////////////////////////////////////////////////////////////
static COLORREF RgbInvertRgb(COLORREF rgbOld)
{
	return (LONG)RGB(GetBValue(rgbOld), GetGValue(rgbOld), GetRValue(rgbOld));
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		inverts an rgb value for highlighting
//	Arguments:
//		rgbOld - rgb value to invert
//	Returns:
//		invert of rgbOld
////////////////////////////////////////////////////////////////////////////////
static VOID InvalidPathDialog(HWND hwnd, CHAR *prgchIdsText, CHAR *prgchPath)
{
	CHAR rgch1[cchSzMax];
	CHAR rgch2[_MAX_PATH];
	CHAR cszcMsgCaption[cchSzMax];

	if (!FLoadIds(IDS_MSGCAPTION, cszcMsgCaption, sizeof cszcMsgCaption))
		{
		return;
		}

	lstrcpy(rgch2, prgchPath);
	ChopPathForMsg(rgch2);

	wsprintf(rgch1, prgchIdsText, rgch2);

	MessageBox(hwnd, rgch1, rgch2, MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
}


/* We don't use this

////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		inverts an rgb value for highlighting
//	Arguments:
//		rgbOld - rgb value to invert
//	Returns:
//		invert of rgbOld
////////////////////////////////////////////////////////////////////////////////
static BOOL FCreatePathDialog(HWND hwnd, CHAR *prgchPath)
{
	CHAR rgch1[cchSzMax];
	CHAR rgch2[_MAX_PATH];
	CHAR cszcCreateDir[cchSzMax];
	CHAR cszcMsgCaption[cchSzMax];

	if (!FLoadIds(IDS_CREATEDIR, cszcCreateDir, sizeof cszcCreateDir))
		{
		return FALSE;
		}
	if (!FLoadIds(IDS_MSGCAPTION, cszcMsgCaption, sizeof cszcMsgCaption))
		{
		return FALSE;
		}
	
	lstrcpy(rgch2, prgchPath);
	ChopPathForMsg(rgch2);

	wsprintf(rgch1, cszcCreateDir, rgch2);

	return MessageBox(hwnd, rgch1, rgch2,
		MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON1 | MB_APPLMODAL) == IDYES;
}
*/


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		inverts an rgb value for highlighting
//	Arguments:
//		prgchIdsText
//	Returns:
//		whether or not the component should be overwritten
////////////////////////////////////////////////////////////////////////////////
static BOOL FOverwriteDialog(HWND hwnd, CHAR *prgchIdsText, CHAR *prgchPath, CHAR *prgchComponent)
{
	CHAR rgch1[cbMaxSz * 2];
	CHAR rgch2[cbMaxSz];
	CHAR cszcMsgCaption[cbMaxSz];

	if (!FLoadIds(IDS_MSGCAPTION, cszcMsgCaption, sizeof cszcMsgCaption))
		{
		return FALSE;
		}

	lstrcpy(rgch2, prgchPath);
	ChopPathForMsg(rgch2);

	wsprintf(rgch1, prgchIdsText, prgchComponent, rgch2);

	return MessageBox(hwnd, rgch1, rgch2,
		MB_ICONQUESTION | MB_OKCANCEL | MB_DEFBUTTON2 | MB_APPLMODAL) == IDOK;
}


////////////////////////////////////////////////////////////////////////////////
//	Purpose:
//		Chops the given directory (in place) to a size to fit a MessageBox.
//	Arguments:
//		szPath - path to chop
//	Returns:
//		length of resulting string
////////////////////////////////////////////////////////////////////////////////
static VOID ChopPathForMsg(SZ szPath)
{
	HDC hdc;
	UINT dx;
	SIZE sSize;
	CHAR *pch;

	pch = szPath + lstrlen(szPath);
	if ('\\' == *--pch)
		{
		if (pch - szPath > 2)
			*pch = '\0';
		}
	hdc = GetDC(NULL);
	GetTextExtentPoint(hdc, "W", 1, &sSize);
	dx = sSize.cx * 24;
	//CchChopText(szPath, dx, hdc, lstrlen(szPath));
	ReleaseDC(NULL, hdc);
}
