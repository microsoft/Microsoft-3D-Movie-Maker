/***************************************************************************

	portf.cpp: Portfolio handler

	Primary Author: guyb
	Review Status: peted has reviewed. Final version not yet approved.

***************************************************************************/
#include "studio.h"
#include "commctrl.h"

ASSERTNAME

/***************************************************************************

 FPortDisplayWithIds: Display the portfolio to open or save a file. Portfolio
 					title and filters generated using supplied string ids.

 Arguments: fni 			- Output FNI for file selected
 			fOpen			- fTrue if open portfolio, else save portfolio.
 			lFilterLabel	- id of portfolio filter label
 			lFilterExt		- id of portfolio filter extension
			lTitle			- id of portfolio title
			lpstrDefExt		- Ptr to string for default file extension for save if required.
			pstnDefFileName - Ptr to default extension stn if required.
			pfniInitialDir	- Ptr to initial directory fni if required.
			grfPrevType		- Bits for types of preview required, (eg movie, sound etc) == 0 if no preview
			cnoWave         - Wave cno for audio when portfolio is invoked

 Returns: 	TRUE	- File selected
 			FALSE	- User canceled portfolio.

***************************************************************************/
bool FPortDisplayWithIds(FNI *pfni, bool fOpen, long lFilterLabel, long lFilterExt, long lTitle,
	LPTSTR lpstrDefExt, PSTN pstnDefFileName, FNI *pfniInitialDir, ulong grfPrevType, CNO cnoWave)
{
	STN stnTitle;
	STN stnFilterLabel;
	STN stnFilterExt;
	int cChLabel, cChExt;
	SZ szFilter;
	bool fRet;

	AssertVarMem(pfni);
	AssertNilOrPo(pfniInitialDir, 0);
	AssertNilOrPo(pstnDefFileName, 0);

	if (!vapp.FGetStnApp(lTitle, &stnTitle))
		return fFalse;

	// Filter string contain non-terminating null characters, so must
	// build up string from separate label and file extension strings.

	if (!vapp.FGetStnApp(lFilterLabel, &stnFilterLabel))
		return fFalse;
	if (!vapp.FGetStnApp(lFilterExt, &stnFilterExt))
		return fFalse;

	// Kauai does not like internal null chars in an STN. So build
	// up the final final string as an SZ.

	cChLabel = stnFilterLabel.Cch();
	cChExt = stnFilterExt.Cch();

	stnFilterLabel.GetSz(szFilter);
	stnFilterExt.GetSz(&szFilter[cChLabel + 1]);

	szFilter[cChLabel + cChExt + 2] = chNil;

	// Now display the open or save portfolio as required
	StopAllMovieSounds();
	vapp.EnsureInteractive();
	vapp.SetFInPortfolio(fTrue);
	if(fOpen)
		{
		fRet = FPortGetFniOpen(pfni, szFilter, stnTitle.Prgch(),
				pfniInitialDir, grfPrevType, cnoWave);
		}
	else
		{
		fRet = FPortGetFniSave(pfni, szFilter, stnTitle.Prgch(),
				lpstrDefExt, pstnDefFileName, grfPrevType, cnoWave);
		}

	// Make sure no portfolio related audio is still playing.
	vpsndm->StopAll(sqnNil, sclNil);
	vapp.SetFInPortfolio(fFalse);

	// Let the script know what the outcome is.
	vpcex->EnqueueCid(cidPortfolioClosed, 0, 0, fRet);

	// We must also enqueue a message for the help system here.
	// Help doesn't get the above cidPortfolioClosed as it has
	// already been filtered.
	vpcex->EnqueueCid(cidPortfolioResult, 0, 0, fRet);

	return fRet;
}

/***************************************************************************

 FPortGetFniOpen: Display the portfolio to open a file.

 Arguments: pfni 		- Output FNI for file selected
 			lpstrFilter	- String containing files types to filter on
			lpstrTitle	- String containing title of portfolio
			pfniInitialDir	- Ptr to initial directory fni if required.
			grfPrevType	- Bits for types of preview required, (eg movie, sound etc) == 0 if no preview
			cnoWave     - Wave cno for audio when portfolio is invoked

 Returns: 	TRUE	- File selected
 			FALSE	- User canceled portfolio.

***************************************************************************/
bool FPortGetFniOpen(FNI *pfni, LPTSTR lpstrFilter, LPTSTR lpstrTitle,
	FNI *pfniInitialDir, ulong grfPrevType, CNO cnoWave)
{
	SZ szFile;
	DLGINFO diPortfolio;
	OPENFILENAME ofn;
	STN stn;
	bool fOKed;
	STN stnInitialDir;
	SZ szInitialDir;

	AssertPo(pfni, 0);
	AssertNilOrPo(pfniInitialDir, 0);

	ClearPb(&ofn, size(OPENFILENAME));
	ClearPb(&diPortfolio, size(DLGINFO));

	szFile[0] = 0;
	ofn.lStructSize = size(OPENFILENAME);
	ofn.hwndOwner = vwig.hwndApp;
	ofn.hInstance = vwig.hinst;
	ofn.nFilterIndex = 1L;	
	ofn.lpstrCustomFilter = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = kcchMaxSz;
	ofn.lpstrFileTitle = NULL;
	ofn.lpfnHook = OpenHookProc; 			
	ofn.lpTemplateName = MAKEINTRESOURCE(dlidPortfolio);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY |
				OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;

	// lpstrDefExt is used for appended to the user typed filename is
	// one is entered without an extension. As the user cannot type
	// anything during a portfolio open, we don't use the lpstrDefExt here.
	ofn.lpstrDefExt = NULL;

	// Initialize internal data for accessing from inside dlg hook proc.		
	diPortfolio.fIsOpen = fTrue;
	diPortfolio.fDrawnBkgnd = fFalse;
	diPortfolio.grfPrevType = grfPrevType;
	diPortfolio.cnoWave = cnoWave;
	ofn.lCustData = (DWORD)&diPortfolio;

	ofn.lpstrFilter = lpstrFilter;
	ofn.lpstrTitle = lpstrTitle;

	// Get the string for the initial directory if required.

	if(pfniInitialDir != pvNil)
		{
		pfniInitialDir->GetStnPath(&stnInitialDir);
		stnInitialDir.GetSz(szInitialDir);
		ofn.lpstrInitialDir = szInitialDir;
		}
	else
		{
		// Initial directory will be current directory.
		ofn.lpstrInitialDir = pvNil;
		}

	// Now display the portfolio.
	fOKed = (GetOpenFileName(&ofn) == FALSE ? fFalse : fTrue);

	if(!fOKed)
		{
		// Check if custom common dlg failed to initialize.	Don't rely on the returned
		// error being CDERR_INITIALIZATION, as who knows what error will be returned
		// in future versions.
		if(CommDlgExtendedError() != NOERROR)
			{
			// User never saw portfolio. Therefore attempt to display
			// common dlg with no customization.
			ofn.Flags &= ~(OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE);

			fOKed = (GetOpenFileName(&ofn) == FALSE ? fFalse : fTrue);
			}
		}

	// If the user selected a file, build up the associated fni.
	if(fOKed)
		{
		stn.SetSz(ofn.lpstrFile);

		pfni->FBuildFromPath(&stn, 0);
		}

	// Update the app window to make sure no parts of the portfolio
	// window are left on the screen while the file is being opened.
	// Calling UpdateMarked() does not have the desired effect here.
	UpdateWindow(vwig.hwndApp);

	// Report error to user if never displayed the portfolio.
	if(CommDlgExtendedError() != NOERROR)
		{
		PushErc(ercSocPortfolioFailed);
		}

	// Only return TRUE if the user selected a file.
	return(fOKed);
}

/***************************************************************************

 pfGetFniSave: Display the save portfolio.

 Arguments: pfni		- Output FNI for file selected
 			lpstrFilter	- String containing files types to filter on
			lpstrTitle	- String containing title of portfolio
 			lpstrDefExt	- String containing default extension for filenames
							typed by user without an extension.
			pstnDefFileName - Ptr to default extension stn if required.
			grfPrevType	- Bits for types of preview required, (eg movie, sound etc) == 0 if no preview
			cnoWave     - Wave cno for audio when portfolio is invoked

 Returns: 	TRUE	- File selected
 			FALSE	- User canceled portfolio, (or other error).

***************************************************************************/
bool FPortGetFniSave(FNI *pfni, LPTSTR lpstrFilter, LPTSTR lpstrTitle, LPTSTR lpstrDefExt,
	PSTN pstnDefFileName, ulong grfPrevType, CNO cnoWave)
{
	DLGINFO diPortfolio;
	OPENFILENAME ofn;
	bool fOKed;
	bool tRet;
	bool fRedisplayPortfolio = fFalse;
	bool fExplorer = fTrue;
	STN stnFile, stnErr;
	FNI fniUserDir;
	STN stnUserDir;
	SZ szUserDir;
	SZ szDefFileName;
	SZ szFileTitle;

	AssertPo(pfni, 0);
	AssertNilOrPo(pstnDefFileName, 0);

	ClearPb(&ofn, size(OPENFILENAME));
	ClearPb(&diPortfolio, size(DLGINFO));

	szFileTitle[0] = chNil;

	ofn.lStructSize = size(OPENFILENAME);
	ofn.hwndOwner = vwig.hwndApp;
	ofn.hInstance = vwig.hinst;
	ofn.nFilterIndex = 1L;	
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxFile = kcchMaxSz;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nMaxFileTitle = kcchMaxSz;
	ofn.lpfnHook = OpenHookProc; 			
	ofn.lpTemplateName = MAKEINTRESOURCE(dlidPortfolio);

	// Don't use OFN_OVERWRITEPROMPT here, otherwise we can't intercept
	// the btn press on the Save btn to display our own message. Note,
	// we don't do that either, because the help topic display mechanism
	// was not designed for use within a modal dlg box such as the portfolio.
	// Instead query overwrite after the portfolio has closed.
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY |
				OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE;

	// Do not allow the save portfolio to change the current directory.
	ofn.Flags |= OFN_NOCHANGEDIR;

	// Let the initial dir be the user's dir.	
	vapp.GetFniUser(&fniUserDir);
	fniUserDir.GetStnPath(&stnUserDir);
	stnUserDir.GetSz(szUserDir);
	ofn.lpstrInitialDir = szUserDir;

	// Initialize internal data for accessing from inside dlg hook proc.		
	diPortfolio.fIsOpen = fFalse;
	diPortfolio.fDrawnBkgnd = fFalse;
	diPortfolio.grfPrevType = grfPrevType;
	diPortfolio.cnoWave = cnoWave;
	ofn.lCustData = (DWORD)&diPortfolio;

	ofn.lpstrFilter = lpstrFilter;
	ofn.lpstrTitle = lpstrTitle;
	ofn.lpstrDefExt = lpstrDefExt;

	// Set the the portfolio default save file name if required.
	if(pstnDefFileName != pvNil)
		{
		pstnDefFileName->GetSz(szDefFileName);
		}
	else
		{
		szDefFileName[0] = chNil;
		}

	ofn.lpstrFile = szDefFileName;

	// We may need to display the portfolio multiple times if the user
	// selects an existing file, and then says they don't want to overwrite it.

	do // Display portfolio
		{
		fRedisplayPortfolio = fFalse;

		// Now display the portfolio.
		fOKed = (GetSaveFileName(&ofn) == FALSE ? fFalse : fTrue);

		if(!fOKed)
			{
			DWORD dwRet;

			// Check if custom common dlg failed to initialize.	Don't rely on the returned
			// error being CDERR_INITIALIZATION, as who knows what error will be returned
			// in future versions.
			if((dwRet = CommDlgExtendedError()) != NOERROR)
				{
				if (dwRet == FNERR_INVALIDFILENAME ||
						dwRet == FNERR_BUFFERTOOSMALL)
					{
					// Set the the portfolio default save file name if required.
					if(pstnDefFileName != pvNil)
						{
						pstnDefFileName->GetSz(szDefFileName);
						}
					else
						{
						szDefFileName[0] = chNil;
						}
					ofn.lpstrFile = szDefFileName;

					PushErc(ercSocInvalidFilename);
					fRedisplayPortfolio = fTrue;
					}
				else if (dwRet == CDERR_INITIALIZATION && fExplorer)
					{
					// User never saw portfolio. Therefore attempt to display
					// common dlg with no customization.
					ofn.Flags &= ~(OFN_EXPLORER | OFN_ENABLEHOOK | OFN_ENABLETEMPLATE);
					fRedisplayPortfolio = fTrue;
					fExplorer = fFalse;
					}
				else
					PushErc(ercSocPortfolioFailed);

				vapp.DisplayErrors();
				continue;
				}
			}
		else
			{
			// Build stn if user selected a file.
			stnFile.SetSz(ofn.lpstrFile);

			// Query any attempt to overwrite an existing file now.
			if(CchSz(ofn.lpstrFile) != 0)
				{
				bool tExists;

				// We always save the file with the default extension. If the
				// user used a different extension, add the default on the end.
				if(ofn.Flags & OFN_EXTENSIONDIFFERENT)
					{
					stnFile.FAppendCh('.');
					stnFile.FAppendSz(lpstrDefExt);
					}

				// Make sure the pfni is built from the appended file name if applicable.
				pfni->FBuildFromPath(&stnFile, 0);
	
				if((tExists = pfni->TExists()) != tNo)
					{
					long cch;
					achar *pch;
					// File already exists. Query user for overwrite.

					// The default name supplied to the user will only
					// be the file name without the path.
					stnFile.SetSz(ofn.lpstrFileTitle);

					/* Remove the extension */
					cch = stnFile.Cch();
					pch = stnFile.Psz() + cch;
					while (cch--)
						{
						pch--;
						if (*pch == ChLit('.'))
							{
							stnFile.Delete(cch);
							break;
							}
						}
					stnFile.GetSz(ofn.lpstrFile);

					// Only query if we know for sure the file's there.  If
					// it's not known for sure the file's there, just make the
					// user pick a new name.
					if (tExists == tYes)
						{
						AssertDo(vapp.FGetStnApp(idsReplaceFile, &stnErr), "String not present");
						tRet = vapp.TModal(vapp.PcrmAll(), ktpcQueryOverwrite,
							&stnErr, bkYesNo, kstidQueryOverwrite, &stnFile);
						}
					else
						{
						vapp.DisplayErrors();
						tRet = tNo;
						}

					// Redisplay the portfolio if no overwrite.
					if(tRet == tNo)
						{
						fRedisplayPortfolio = fTrue;

						// Make sure the app window is updated, otherwise the help topic
						// may still be displayed while the portfolio is redisplayed.
						InvalidateRect(vwig.hwndApp, NULL, TRUE);
						UpdateWindow(vwig.hwndApp);
						}
					}
				}
			else
				{
				// The user OKed the selection, and yet with have a zero length file name.
				// Something must have gone wrong with the common dialog. Treat this as a
				// cancel of the portfolio. (Remember we have not set up pfni in this case.)
				Bug("Portfolio selection of a file with no name");

				fOKed = fFalse;
				}
			}
		}
		while(fRedisplayPortfolio);

	// If the user selected a file, build up the associated fni.
	if(!fOKed)
		{
		pfni->SetNil();
		}

	// Update the app window to make sure no parts of the portfolio
	// window are left on the screen while the file is being saved.
	// Calling UpdateMarked() does not have the desired effect here.
	UpdateWindow(vwig.hwndApp);

	// Report error to user if never displayed the portfolio.
	if(CommDlgExtendedError() != NOERROR)
		{
		PushErc(ercSocPortfolioFailed);
		}

	// Only return fTrue if the user selected a file.
	return(fOKed);
}

/***************************************************************************

 OpenHookProc: Hook proc for get open/save common dlg.

 Arguments: standard dialog proc args.

 Returns: TRUE  - Common dlg will ignore message
          FALSE - Common dlg will process this message after custom dlg.

 Note - on win95, hwndCustom is the handle to the custom dlg created as a
 child of the common dlg.

***************************************************************************/
UINT CALLBACK OpenHookProc(HWND hwndCustom, UINT msg, UINT wParam, LONG lParam)
{
	switch (msg)
		{
	case WM_INITDIALOG:
		{
		PDLGINFO pdiPortfolio;
		LONG lwStyle, lwExstyle;
		OPENFILENAME *lpOfn;
		HWND hwndDlg;
		RC rc;
		WNDPROC lpOtherBtnProc;

		lpOfn = (OPENFILENAME *)lParam;
		pdiPortfolio = (PDLGINFO)(lpOfn->lCustData);

		SetWindowLong(hwndCustom, GWL_USERDATA, (LONG)pdiPortfolio);

		hwndDlg = GetParent(hwndCustom);

		// Give ourselves a way to access the custom dlg hwnd
		// from the common dlg subclass wndproc.
		SetWindowLong(hwndDlg, GWL_USERDATA, (LONG)hwndCustom);

		// Hide common dlg controls that we're not interested in here. Use the Common Dialog
		// Message for hiding the control. The documentation on CDM_HIDECONTROL doesn't really
		// describe what the command actually does, so explictly disable the OK/Cancel btns here.

		SendMessage(hwndDlg, CDM_HIDECONTROL, stc2, 0L);		// File type label
		EnableWindow(GetDlgItem(hwndDlg, stc2), FALSE);

		SendMessage(hwndDlg, CDM_HIDECONTROL, cmb1, 0L);		// File type combo box	
		EnableWindow(GetDlgItem(hwndDlg, cmb1), FALSE);

		SendMessage(hwndDlg, CDM_HIDECONTROL, chx1, 0L);		// Open as Read-only check box
		EnableWindow(GetDlgItem(hwndDlg, chx1), FALSE);

		// Even though we are hiding the OK/Cancel buttons, do not disable them.
		// By doing this, we retain some default common dialog behavior. When the
		// user hits the Enter key the highlighted file will be selected, and when
		// the user hits the Escape key the portfolio is dismissed.

		SendMessage(hwndDlg, CDM_HIDECONTROL, IDOK, 0L);		// OK btn.
		SendMessage(hwndDlg, CDM_HIDECONTROL, IDCANCEL, 0L);	// Cancel btn.

		if(pdiPortfolio->fIsOpen)
			{
			SendMessage(hwndDlg, CDM_HIDECONTROL, stc3, 0L);	// 'File Name'
			EnableWindow(GetDlgItem(hwndDlg, stc3), FALSE);

			SendMessage(hwndDlg, CDM_HIDECONTROL, edt1, 0L);	// File name edit ctrl
			EnableWindow(GetDlgItem(hwndDlg, edt1), FALSE);
			}

		// If no preview required then hide the preview window.

		if(pdiPortfolio->grfPrevType == 0)
			{
			SendMessage(hwndCustom, CDM_HIDECONTROL, IDC_PREVIEW, 0L);	
			EnableWindow(GetDlgItem(hwndCustom, IDC_PREVIEW), FALSE);
			}

		// Give the main common dlg the required style for custom display.
		// This means remove the caption bar, the system menu and the thick border.
		// Note, We do not need a frame round the window, as we customize its
		// background entirely. Therefore we should remove WS_CAPTION, (which is
		// WS_BORDER | WS_DLGFRAME), and WS_EX_DLGMODALFRAME. However, if we do
		// this, then when the user navigates around the list box in the dialog,
		// the app window gets repainted. Presumably this is due to win95
		// invalidating an area slightly larger than the dlg without any border.
		// Therefore maintain the thin border.

		lwStyle = GetWindowLong( hwndDlg, GWL_STYLE);
		lwStyle &= ~(WS_DLGFRAME | WS_SYSMENU);
		SetWindowLong( hwndDlg, GWL_STYLE, lwStyle);

		lwExstyle = GetWindowLong( hwndDlg, GWL_EXSTYLE);
		lwExstyle &= ~WS_EX_DLGMODALFRAME;
		SetWindowLong( hwndDlg, GWL_EXSTYLE, lwExstyle);

		// IMPORTANT NOTE. Cannot move or size the portfolio here. Otherwise this
		// confuses win95 when it calculates whether all the controls fit in the
		// common dlg or not. Instead we must wait for the INITDONE notification.
		// Note, since the portfolio is full screen now, we don't need to move
		// the window anyway..
		
		// Subclass the push btns to prevent the background flashing in the default color.
		lpBtnProc = (WNDPROC)SetWindowLong( GetDlgItem(hwndCustom, IDC_BUTTON1), GWL_WNDPROC, (LONG)SubClassBtnProc);		
		
		lpOtherBtnProc = (WNDPROC)SetWindowLong( GetDlgItem(hwndCustom, IDC_BUTTON2), GWL_WNDPROC, (LONG)SubClassBtnProc);
		Assert(lpBtnProc == lpOtherBtnProc, "Custom portfolio buttons (ok/cancel) have different window procs");

		lpOtherBtnProc = (WNDPROC)SetWindowLong( GetDlgItem(hwndCustom, IDC_BUTTON3), GWL_WNDPROC, (LONG)SubClassBtnProc);
		Assert(lpBtnProc == lpOtherBtnProc, "Custom portfolio buttons (ok/home) have different window procs");

		// Subclass the preview window to allow custom draw.
		lpPreviewProc = (WNDPROC)SetWindowLong( GetDlgItem(hwndCustom, IDC_PREVIEW), GWL_WNDPROC, (LONG)SubClassPreviewProc);		

		// Subclass the main common dlg window to stop static control backgrounds being
		// fill with the current system color. Instead use a color that matches our
		// custom background bitmap.
		lpDlgProc = (WNDPROC)SetWindowLong( hwndDlg, GWL_WNDPROC, (LONG)SubClassDlgProc);		

		// For the save portfolio we want the file name control to have focus when displayed.
		if(!pdiPortfolio->fIsOpen)
			{
			SetFocus(GetDlgItem(hwndDlg, edt1));

			// Select all the text in the file name edit control.
			SendMessage(GetDlgItem(hwndDlg, edt1), EM_SETSEL, 0, -1);

			return(1);
			}

		// Allow Windows to set the focus wherever it wants to.
		return(0);
		}
	case WM_COMMAND:
		{
		// If the user has clicked on one of our custom buttons, then
		// simulate a click on one of the hidden common dlg buttons.

		if (HIWORD(wParam) == BN_CLICKED)
			{
			switch (LOWORD(wParam))
				{
			case IDC_BUTTON1:
		
				PostMessage(GetParent(hwndCustom), WM_COMMAND, IDOK, 0);
				return(1);

			case IDC_BUTTON2:
		
				PostMessage(GetParent(hwndCustom), WM_COMMAND, IDCANCEL, 0);
				return(1);

			case IDC_BUTTON3:
				{
				FNI fniUserDir;
				STN stnUserDir;
				SZ szUserDir;
				SZ szCurFile;
				HWND hwndDlg = GetParent(hwndCustom);

				// The user has pressed the Go To User's Home Folder btn. So get a SZ
				// for the user's home folder.

				vapp.GetFniUser(&fniUserDir);
				fniUserDir.GetStnPath(&stnUserDir);
				stnUserDir.GetSz(szUserDir);

				// Ideally there would be some CDM_ message here to tell the common dialog
				// that the current folder is to be changed. Unfortunately there is no such
				// message. There is also no recommended way of doing this. Note that if we
				// call SetCurrentDirectory here, then the common dialog is oblivious of the
				// folder change.

				// SO, we shall simulate a folder change by the user. Do this by filling
				// the file name edit control with the target folder, and then sending an
				// OK message to the common dlg.
				
				// This would be enough to change the folder, but we would lose the current
				// contents of the file name control. Therefore store the control contents
				// before we overwrite it, and reset it afterwards. Ideally there would be
				// some message to retrieve the contents of a control, (eg CDM_GETCONTROLTEXT),
				// but there isn't, so use a regular Windows call for this.
				
				// Get the current text for the control.
				GetDlgItemText(hwndDlg, edt1, szCurFile, sizeof(szCurFile));

				// Now change folder!
				SendMessage(hwndDlg, CDM_SETCONTROLTEXT, edt1, (LPARAM)szUserDir);
				SendMessage(hwndDlg, WM_COMMAND, IDOK, 0);

				// Restore the edit control text.
				SendMessage(hwndDlg, CDM_SETCONTROLTEXT, edt1, (LPARAM)szCurFile);

				return(1);
				}
			default:
				break;
				}
			}

		break;
		}	
	case WM_DRAWITEM:
		{
		int iDlgId = (int)wParam;
		DRAWITEMSTRUCT *pDrawItem = (DRAWITEMSTRUCT*)lParam;
		CNO cnoDisplay = cnoNil;
		PMBMP pmbmp;	

		// Custom draw the our push btns here.
		switch (iDlgId)
			{
		case IDC_BUTTON1:

			if(pDrawItem->itemState & ODS_SELECTED)
				{
				cnoDisplay = kcnoMbmpPortBtnOkSel;
				}
			else
				{
				cnoDisplay = kcnoMbmpPortBtnOk;
				}

			break;

		case IDC_BUTTON2:
		
			if(pDrawItem->itemState & ODS_SELECTED)
				{
				cnoDisplay = kcnoMbmpPortBtnCancelSel;
				}
			else
				{
				cnoDisplay = kcnoMbmpPortBtnCancel;
				}

			break;

		case IDC_BUTTON3:
		
			if(pDrawItem->itemState & ODS_SELECTED)
				{
				cnoDisplay = kcnoMbmpPortBtnHomeSel;
				}
			else
				{
				cnoDisplay = kcnoMbmpPortBtnHome;
				}

			break;

		default:

			break;
			}

		if(cnoDisplay != cnoNil)		
			{
			// Select the appropriate bitmap to display.

			if((pmbmp = (PMBMP)vpapp->PcrmAll()->PbacoFetch(kctgMbmp, cnoDisplay, MBMP::FReadMbmp)))
				{
				PGPT pgpt;
				HPEN hpen, hpenold;
				HBRUSH hbr, hbrold;
				HFONT hfnt, hfntold;
				
				// To ensure that we don't return from here with different objects
				// selected in the supplied hdc, save them here and restore later.
				// (Note that Kauai will attempt to delete things it finds selected
				// in the hdc passed to PgptNew).

				hpen = (HPEN)GetStockObject(NULL_PEN);
				Assert(hpen != hNil, "Portfolio - draw items GetStockObject(NULL_PEN) failed");
				hpenold = (HPEN)SelectObject(pDrawItem->hDC, hpen);

				hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
				Assert(hbr != hNil, "Portfolio - draw items GetStockObject(WHITE_BRUSH) failed");
				hbrold = (HBRUSH)SelectObject(pDrawItem->hDC, hbr);

				hfnt = (HFONT)GetStockObject(SYSTEM_FONT);
				Assert(hfnt != hNil, "Portfolio - draw items GetStockObject(SYSTEM_FONT) failed");
				hfntold = (HFONT)SelectObject(pDrawItem->hDC, hfnt);

				if ((pgpt = GPT::PgptNew(pDrawItem->hDC)) != pvNil)
					{
					GNV gnv(pgpt);
					PGNV pgnvOff;
					PGPT pgptOff;
					RC rcItem(pDrawItem->rcItem);

					// Must create offscreen dc and blit into that. Then blit that
					// to dlg on screen. If we blit straight from mbmp to screen,
					// then screen flashes. (Due to white fillrect).

					if((pgptOff = GPT::PgptNewOffscreen(&rcItem, 8)) != pvNil)
						{
						if((pgnvOff = NewObj GNV(pgptOff)) != pvNil)
							{
							pgnvOff->DrawMbmp(pmbmp, rcItem.xpLeft, rcItem.ypTop);

							gnv.CopyPixels(pgnvOff, &rcItem, &rcItem);
							GPT::Flush();				

							ReleasePpo(&pgnvOff);
							}

						ReleasePpo(&pgptOff);
						}

					ReleasePpo(&pgpt);
					}						

				// Restore the currently secleted objects back into he supplied hdc.

				SelectObject(pDrawItem->hDC, hpenold);
				SelectObject(pDrawItem->hDC, hbrold);
				SelectObject(pDrawItem->hDC, hfntold);

				ReleasePpo(&pmbmp);
				}

			return(1);
			}

		break;
		}
	case WM_NOTIFY:
		{
		LPOFNOTIFY lpofNotify = (LPOFNOTIFY)lParam;

		switch (lpofNotify->hdr.code)
			{
		case CDN_INITDONE:
			{
			// Win95 has finished doing any resizing of the custom dlg and the controls.
			// So take any special action now to ensure the portfolio still looks good.

			PDLGINFO pdiPortfolio = (PDLGINFO)GetWindowLong(hwndCustom, GWL_USERDATA);
			RCS rcsApp;
			POINT ptBtn;
			int ypBtn;
			PMBMP pmbmpBtn;
			RC rcBmp;
			RCS rcsAppScreen, rcsPreview;
			int xOff = 0;
			int yOff = 0;
			LONG lStyle;
			PCRF pcrf;
			HWND hwndDlg = GetParent(hwndCustom);
			HWND hwndApp = GetParent(hwndDlg);
			HWND hwndPreview = GetDlgItem(hwndCustom, IDC_PREVIEW);

			// Store the current size of the portfolio. We need this later when we stretch
			// the background bitmap to match how win95 may have stretched the dlg controls.
			GetClientRect(hwndDlg, &(pdiPortfolio->rcsDlg));

			// Get the client area size of the app window.
			GetClientRect(hwndApp, &rcsApp);

			// Now move the custom buttons to always be to the right of the preview window.
			// Make sure we keep the buttons above the bottom of the screen.

			GetClientRect(hwndPreview, &rcsPreview);
			ptBtn.x = rcsPreview.right;
			ptBtn.y = (rcsPreview.top + rcsPreview.bottom) / 2;
			MapWindowPoints(hwndPreview, hwndCustom, (POINT*)&ptBtn, 1);

			// First the home button.
			if((pmbmpBtn = (PMBMP)vpapp->PcrmAll()->PbacoFetch(kctgMbmp, kcnoMbmpPortBtnHome, MBMP::FReadMbmp)))
				{
				pmbmpBtn->GetRc(&rcBmp);

				ptBtn.x += (3 * rcBmp.Dxp()) / 4;
				ypBtn = min(ptBtn.y - (rcBmp.Dyp() / 2), rcsApp.bottom - (2 * rcBmp.Dyp()));
			
				SetWindowPos(GetDlgItem(hwndCustom, IDC_BUTTON3), 0, ptBtn.x, ypBtn, rcBmp.Dxp(), rcBmp.Dyp(), SWP_NOZORDER);

				ReleasePpo(&pmbmpBtn);
				}

			// Now the cancel button.
			if((pmbmpBtn = (PMBMP)vpapp->PcrmAll()->PbacoFetch(kctgMbmp, kcnoMbmpPortBtnCancel, MBMP::FReadMbmp)))
				{
				pmbmpBtn->GetRc(&rcBmp);

				ptBtn.x += (5 * rcBmp.Dxp()) / 4;
				ypBtn = min(ptBtn.y - (rcBmp.Dyp() / 2), rcsApp.bottom - (2 * rcBmp.Dyp()));

				SetWindowPos(GetDlgItem(hwndCustom, IDC_BUTTON2), 0, ptBtn.x, ypBtn, rcBmp.Dxp(), rcBmp.Dyp(), SWP_NOZORDER);

				ReleasePpo(&pmbmpBtn);
				}

			// Now the ok button.
			if((pmbmpBtn = (PMBMP)vpapp->PcrmAll()->PbacoFetch(kctgMbmp, kcnoMbmpPortBtnOk, MBMP::FReadMbmp)))
				{
				pmbmpBtn->GetRc(&rcBmp);

				ptBtn.x += (5 * rcBmp.Dxp()) / 4;
				ypBtn = min(ptBtn.y - (rcBmp.Dyp() / 2), rcsApp.bottom - (2 * rcBmp.Dyp()));

				SetWindowPos(GetDlgItem(hwndCustom, IDC_BUTTON1), 0, ptBtn.x, ypBtn, rcBmp.Dxp(), rcBmp.Dyp(), SWP_NOZORDER);

				ReleasePpo(&pmbmpBtn);
				}

			// Note, win95 may have pushed the portfolio around depending on where the task bar is.
			// Ensure that the top left corner of the portfolio is in the top left of the client
			// area of the app window. Note that the common dlg is owned by the app window, but
			// it is not a child window itself.

			// Note, if we MapWindowPoints on (0,0) here, then the returned coords are shifted by
			// the task bar which is just what we don't want. So GetWindowRect instead.
			GetWindowRect(hwndApp, &rcsAppScreen);

			// If running in a window, then we must find the offset from the top left of the app
			// window's corner to its client area.
			lStyle = GetWindowLong(hwndApp, GWL_STYLE);

			if(lStyle & WS_CAPTION)
				{
				xOff = GetSystemMetrics(SM_CXDLGFRAME);

				yOff = GetSystemMetrics(SM_CYDLGFRAME);
				yOff += GetSystemMetrics(SM_CYCAPTION);
				}

			// Now move the common dialog itself. Resize the dialog too, to be the same size
			// as the app window.
			SetWindowPos(hwndDlg, 0, rcsAppScreen.left + xOff, rcsAppScreen.top + yOff, rcsApp.right, rcsApp.bottom, SWP_NOZORDER);

			// Custom dialog is a child of the common dlg, so (0, 0) wil position it it the common
			// dialog's client area.
			SetWindowPos(hwndCustom, 0, 0, 0, rcsApp.right, rcsApp.bottom, SWP_NOZORDER);

			// Now play the sound associated with this portfolio is there is one.
			// Note that we are not currently queueing this sound or terminating
			// any currently playing sound.
			if(pdiPortfolio->cnoWave != cnoNil)
				{
				// There is a sound for the portfolio, so find it.
				if((pcrf = ((APP *)vpappb)->PcrmAll()->PcrfFindChunk(kctgWave, pdiPortfolio->cnoWave)) != pvNil)
					{
					vpsndm->SiiPlay(pcrf, kctgWave, pdiPortfolio->cnoWave);
					}
				}

			break;
			}
		case CDN_SELCHANGE:
			{
			HWND hwndPreview = GetDlgItem(hwndCustom, IDC_PREVIEW);

			// User has changed the file selected, so update the preview window.
			InvalidateRect(hwndPreview, NULL, FALSE);
			UpdateWindow(hwndPreview);

			break;
			}
#ifdef QUERYINPORTFOLIO
// Do this if we ever have a way of querying the user with a help topic
// from inside the portfolio.
		case CDN_FILEOK:
			{
			PDLGINFO pdiPortfolio = (PDLGINFO)GetWindowLong(hwndCustom, GWL_USERDATA);

			// User has hit OK or Save. Is the user trying to save over an existing file?
			if(pdiPortfolio->fIsOpen != fTrue)
				{
				// Neither CommDlg_OpenSave_GetFilePath nor CommDlg_OpenSave_GetSpec
				// always return the string with the default extension already added
				// if applicable. Therefore get the file name from the returned OFN
				// structure, as this stores the complete file name.

				if(CchSz(lpofNotify->lpOFN->lpstrFile) != 0)
					{
					FNI fni;
					STN stnFile, stnErr;
					bool fHelp, tRet;
					long lSelect;

					// Now does the specified file already exist?

					stnFile.SetSz(lpofNotify->lpOFN->lpstrFile);

					fni.FBuildFromPath(&stnFile, 0);

					if(fni.TExists() != tNo)
						{
						if(vpappb->TGiveAlertSz(
								"The selected file already exists.\n\nDo you want to overwrite it?",
								bkYesNo, cokQuestion) != tYes)
							{
							// Move the focus back to the file name edit ctrl.
							SetFocus(GetDlgItem(GetParent(hwndCustom), edt1));

							// Let win95 know that the portfolio is to stay up.
							SetWindowLong(hwndCustom, DWL_MSGRESULT, 1);
							return(1);
							}
						}	
					}
				}

			break;
			}
#endif //QUERYINPORTFOLIO
		default:
			{
			break;
			}
			}

		break;
		}
	case WM_ERASEBKGND:

		// We never want the background painted in the system colors.
		return(1);

	case WM_PAINT:
		{		
		PDLGINFO pdiPortfolio;

		pdiPortfolio = (PDLGINFO)GetWindowLong(hwndCustom, GWL_USERDATA);

		// Repaint the entire portfolio.
		RepaintPortfolio(hwndCustom);

		// Now the background is drawn, disallow erasing of the background in the
		// system color before each future repaint.
		pdiPortfolio->fDrawnBkgnd = fTrue;

		return(1);
		}
	default:

		break;
		}

	return(0);
}

/***************************************************************************

 RepaintPortfolio: Repaint the entire portfolio.

 Arguments: hwndCustom	- Handle to our custom dialog, ie child of the main common dlg.

 Returns: 	nothing

***************************************************************************/
void RepaintPortfolio(HWND hwndCustom)
{
	PAINTSTRUCT ps;
	TEXTMETRIC tmCaption;
	SZ szCaption;
	PDLGINFO pdiPortfolio = (PDLGINFO)GetWindowLong(hwndCustom, GWL_USERDATA);
	PMBMP pmbmp, pmbmpBtn;
	int iBtn;
	CNO cnoBack;

	// Draw the custom background for the common dlg.
	BeginPaint(hwndCustom, &ps);

	// Display the open or save portfolio background bitmap as appropriate.
	if(pdiPortfolio->fIsOpen)
		{
		cnoBack = kcnoMbmpPortBackOpen;
		}
	else
		{
		cnoBack = kcnoMbmpPortBackSave;
		}

	// Get the background bitmap first.
	if((pmbmp = (PMBMP)vpapp->PcrmAll()->PbacoFetch(kctgMbmp, cnoBack, MBMP::FReadMbmp)))
		{
		PGPT pgpt;
		HPEN hpen, hpenold;
		HBRUSH hbr, hbrold;
		HFONT hfnt, hfntold;
				
		// To ensure that we don't return from here with different objects
		// selected in the supplied hdc, save them here and restore later.
		// (Note that Kauai will attempt to delete things it finds selected
		// in the hdc passed to PgptNew).

		hpen = (HPEN)GetStockObject(NULL_PEN);
		Assert(hpen != hNil, "Portfolio - draw background GetStockObject(NULL_PEN) failed");
		hpenold = (HPEN)SelectObject(ps.hdc, hpen);

		hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
		Assert(hbr != hNil, "Portfolio - draw background GetStockObject(WHITE_BRUSH) failed");
		hbrold = (HBRUSH)SelectObject(ps.hdc, hbr);

		hfnt = (HFONT)GetStockObject(SYSTEM_FONT);
		Assert(hfnt != hNil, "Portfolio - draw background GetStockObject(SYSTEM_FONT) failed");
		hfntold = (HFONT)SelectObject(ps.hdc, hfnt);

		if ((pgpt = GPT::PgptNew(ps.hdc)) != pvNil)
			{
			RC rcDisplay;
			RCS rcsPort, rcsPreview;
			GNV gnv(pgpt);
			PGNV pgnvOff;
			PGPT pgptOff;
			HWND hwndPreview;
			CNO cnoBtn;
			int iBtnId;

			// Get the current size of the Portfolio window.
			GetClientRect(hwndCustom, &rcsPort);
			rcDisplay = rcsPort;

			// Must create offscreen dc and blit into that. Then blit that to dlg on screen.
			// If we blit straight from	mbmp to screen, then screen flashes. (Due to white FILLRECT).

			if((pgptOff = GPT::PgptNewOffscreen(&rcDisplay, 8)) != pvNil)
				{
				if((pgnvOff = NewObj GNV(pgptOff)) != pvNil)
					{
					RC rcOrgPort(pdiPortfolio->rcsDlg);
					RC rcClip(ps.rcPaint);

					// Win95 may initially have streched the portfolio due to font sizing.
					// All the portfolio controls will also have been stretched. So we must
					// now stretch the portfolio background bitmap into the size win95
					// originally made it, to make the background fit the current size
					// of the controls.

					// Note, if the user has scaled down the font then the portfolio bitmap
					// will not fill the entire portfolio area now. The area outside the portfolio
					// will be black. While this is not ideal, it will be rare and everything will
					// still work.
					pgnvOff->DrawMbmp(pmbmp, &rcOrgPort);

					// While we have this offscreen dc, paint all our custom display into it now.

					// First the caption text. Position the text using its size.
					GetWindowText(GetParent(hwndCustom), szCaption, sizeof(szCaption));
					GetTextMetrics(ps.hdc, &tmCaption);
					pgnvOff->DrawRgch(szCaption, CchSz(szCaption), (tmCaption.tmAveCharWidth * 2), tmCaption.tmHeight / 4, kacrBlack, kacrClear);

					// If we were to blit the current offscreen dc to the screen, then the
					// display of the preview window and custom buttons will be overwritten.
					// Those windows would immediately be repainted to make the portfolio
					// whole again. However, the windows would flash between blitting the
					// background bitmap and repainting them. Therefore add the preview
					// and custom buttons to the offscreen dc now, so that when the dc
					// is blitted to the screen, these controls are good.

					// Note, that the preview window alone is repainted when the user selection
					// changes. Also the custom buttons are redrawn when we get the notification
					// to redraw them in the dlg hook proc.

					// Update the preview window now.
					hwndPreview = GetDlgItem(hwndCustom, IDC_PREVIEW);
					GetClientRect(hwndPreview, &rcsPreview);
					MapWindowPoints(hwndPreview, hwndCustom, (POINT*)&rcsPreview, 2);

					OpenPreview(hwndCustom, pgnvOff, &rcsPreview);

					// Now the custom buttons.
					for(iBtn = 0; iBtn < 3; ++iBtn)
						{
						switch (iBtn)
							{
						case 0:

							cnoBtn = kcnoMbmpPortBtnOk;
							iBtnId = IDC_BUTTON1;

							break;

						case 1:

							cnoBtn = kcnoMbmpPortBtnCancel;
							iBtnId = IDC_BUTTON2;

							break;

						case 2:
							
							cnoBtn = kcnoMbmpPortBtnHome;
							iBtnId = IDC_BUTTON3;

							break;

						default:
							continue;
							}

						if((pmbmpBtn = (PMBMP)vpapp->PcrmAll()->PbacoFetch(kctgMbmp, cnoBtn, MBMP::FReadMbmp)))
							{
							HWND hwndBtn = GetDlgItem(hwndCustom, iBtnId);
							RCS rcsBtn;
							RC rcItem;

							GetClientRect(hwndBtn, &rcsBtn);
							MapWindowPoints(hwndBtn, hwndCustom, (POINT*)&rcsBtn, 2);
							rcItem = rcsBtn;

							pgnvOff->DrawMbmp(pmbmpBtn, rcItem.xpLeft, rcItem.ypTop);

							ReleasePpo(&pmbmpBtn);
							}
						}

					// Clip the final blit, to the area which actually needs repainting.
					gnv.ClipRc(&rcClip);

					// Now finally blit our portfolio image to the screen.
					gnv.CopyPixels(pgnvOff, &rcDisplay, &rcDisplay);
					GPT::Flush();				
	
					ReleasePpo(&pgnvOff);
					}

				ReleasePpo(&pgptOff);
				}

			ReleasePpo(&pgpt);
			}

		// Restore the currently secleted objects back into he supplied hdc.
		SelectObject(ps.hdc, hpenold);
		SelectObject(ps.hdc, hbrold);
		SelectObject(ps.hdc, hfntold);

		ReleasePpo(&pmbmp);
		}

	EndPaint(hwndCustom, &ps);

	return;
}

/***************************************************************************

 OpenPreview: Generate display for the preview of a movie.

 Arguments: hwndCustom	- Handle to custom dlg window
 			pgnvOff		- Offscreen dc for displaying preview in.
			prcsPreview - RCS for displaying preview.

 Returns: nothing.

***************************************************************************/
void OpenPreview(HWND hwndCustom, PGNV pgnvOff, RCS *prcsPreview)
{
	STN stn;
	PCFL pcfl;
	PMBMP pmbmp;
	FNI fni;
	SZ szFile;
	ERS ersT;
	ERS *pers;
	PDLGINFO pdiPortfolio = (PDLGINFO)GetWindowLong(hwndCustom, GWL_USERDATA);
	bool fPreviewed = fFalse;
	RC rcPreview(*prcsPreview);

	// If no preview is required, then do nothing here.
	if(pdiPortfolio->grfPrevType == 0)
		return;

	// Do not allow default error reporting to take place. This is due
	// to the fact that currently any queued errors do not appear
	// until the portfolio has been dismissed.
	
	pers = vpers;
	vpers = &ersT;

	// Clear the current contents of the preview window.
	pgnvOff->FillRc(&rcPreview, kacrBlack);

	// Get the currently selected file name.
	CommDlg_OpenSave_GetSpec( GetParent(hwndCustom), szFile, sizeof(szFile));

	// Note the above call returns the name of the last selected file, even if the
	// user has since selected a folder! If the user hits the OK btn in this case,
	// win95 opens the last selected file anyway, so we are consistent here.

	if(CchSz(szFile) != 0)
		{
		// Get an fni for the selected file.
		stn.SetSz(szFile);
		fni.FBuildFromPath(&stn, 0);

		// If the user specified a directory, then don't preview it.
		if(!fni.FDir())
			{
			// The name specifies a file. How should we preview it?
			if(pdiPortfolio->grfPrevType & fpfPortPrevMovie)
				{
				// Preview it as a movie if we can.
				if ((pcfl = CFL::PcflOpen(&fni, fcflNil)) != pvNil)
					{
					CKI	ckiMovie;
					KID	kidScene, kidThumb;
					BLCK blck;

					// Get the movie chunk from the open file.
					if (pcfl->FGetCkiCtg(kctgMvie, 0, &ckiMovie))
						{
						// Now get the scene chunk and details.
						if (pcfl->FGetKidChidCtg(kctgMvie, ckiMovie.cno, 0, kctgScen, &kidScene))
							{
							// Get the mbmp for the movie thumbnail.
							if (pcfl->FGetKidChidCtg(kctgScen, kidScene.cki.cno, 0, kctgThumbMbmp, &kidThumb) &&
								pcfl->FFind(kidThumb.cki.ctg, kidThumb.cki.cno, &blck))
								{
								if ((pmbmp = MBMP::PmbmpRead(&blck)) != pvNil)
									{
									// Stretch the preview into the preview window.
									pgnvOff->DrawMbmp(pmbmp, &rcPreview);

									ReleasePpo(&pmbmp);
		   	
									fPreviewed = fTrue;
									}
								}
							}
						}

					ReleasePpo(&pcfl);	
					}
				}

			if(!fPreviewed &&
				(pdiPortfolio->grfPrevType & fpfPortPrevTexture))
				{
				// Preview the file as a .bmp file if we can.
				if ((pmbmp = MBMP::PmbmpReadNative(&fni, 0, 0, 0, fmbmpNil)) != pvNil)
					{
					// Stretch the bitmap in the preview window.
					pgnvOff->DrawMbmp(pmbmp, &rcPreview);
					ReleasePpo(&pmbmp);

					fPreviewed = fTrue;
					}
				}

			// Currently portfolio is not required to preview sound files.
			}
		}

	// Restore the default error reporting.
	vpers = pers;

	return;
}

/***************************************************************************

 SubClassBtnProc: Subclass proc for custom btn ctrls..

 Arguments: standard dialog proc args.

 Returns: TRUE/FALSE
***************************************************************************/
LRESULT CALLBACK SubClassBtnProc(HWND hwndBtn, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
		{
	case WM_ERASEBKGND:
		
		// We draw the button entirely later, so don't change the screen
		// at all here, This prevents any flashing while dlg repainted.
		return(1);

	default:

		break;
		}

	return(CallWindowProc (lpBtnProc, hwndBtn, msg, wParam, lParam));
}

/***************************************************************************

 SubClassPreviewProc: Subclass proc for preview window.

 Arguments: standard dialog proc args.

 Returns: TRUE/FALSE
***************************************************************************/
LRESULT CALLBACK SubClassPreviewProc(HWND hwndPreview, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
		{
	case WM_PAINT:
		{	
		PAINTSTRUCT ps;
		RCS rcsPreview;
		RC rcPreview;
		PGNV pgnvOff;
		PGPT pgpt, pgptOff;
		HPEN hpen, hpenold;
		HBRUSH hbr, hbrold;
		HFONT hfnt, hfntold;
				
		// Subclass this window so that when the user selects a file, we can only
		// invalidate and repaint this window, rather than repainting the entire dlg.
		BeginPaint(hwndPreview, &ps);

		// To ensure that we don't return from here with different objects
		// selected in the supplied hdc, save them here and restore later.
		// (Note that Kauai will attempt to delete things it finds selected
		// in the hdc passed to PgptNew).

		hpen = (HPEN)GetStockObject(NULL_PEN);
		Assert(hpen != hNil, "Portfolio - draw Preview GetStockObject(NULL_PEN) failed");
		hpenold = (HPEN)SelectObject(ps.hdc, hpen);

		hbr = (HBRUSH)GetStockObject(WHITE_BRUSH);
		Assert(hbr != hNil, "Portfolio - draw Preview GetStockObject(WHITE_BRUSH) failed");
		hbrold = (HBRUSH)SelectObject(ps.hdc, hbr);

		hfnt = (HFONT)GetStockObject(SYSTEM_FONT);
		Assert(hfnt != hNil, "Portfolio - draw Preview GetStockObject(SYSTEM_FONT) failed");
		hfntold = (HFONT)SelectObject(ps.hdc, hfnt);

		if ((pgpt = GPT::PgptNew(ps.hdc)) != pvNil)
			{
			GNV gnv(pgpt);

			GetClientRect(hwndPreview, &rcsPreview);
			rcPreview = rcsPreview;

			if((pgptOff = GPT::PgptNewOffscreen(&rcPreview, 8)) != pvNil)
				{
				if((pgnvOff = NewObj GNV(pgptOff)) != pvNil)
					{
					// Get the preview image into our offscreen dc.
					OpenPreview(GetParent(hwndPreview), pgnvOff, &rcsPreview);

					// Now update the screen.
					gnv.CopyPixels(pgnvOff, &rcPreview, &rcPreview);
					GPT::Flush();				

					ReleasePpo(&pgnvOff);
					}

				ReleasePpo(&pgptOff);
				}

			ReleasePpo(&pgpt);
			}

		// Restore the currently secleted objects back into he supplied hdc.
		SelectObject(ps.hdc, hpenold);
		SelectObject(ps.hdc, hbrold);
		SelectObject(ps.hdc, hfntold);

		EndPaint(hwndPreview, &ps);

		return(0);
		}
	default:

		break;
	}

	return(CallWindowProc (lpPreviewProc, hwndPreview, msg, wParam, lParam));
}

/***************************************************************************

 SubClassDlgProc: Subclass proc for common dlg window..

 Arguments: standard dialog proc args.

 Returns: TRUE/FALSE
***************************************************************************/
LRESULT CALLBACK SubClassDlgProc(HWND hwndDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{

	switch (msg)
		{
	case WM_HELP:
		{
		// Do not invoke any help while the portfolio is displayed.
		return TRUE;
		}
	case WM_CTLCOLORSTATIC:
		{
		HDC hdc = (HDC)wParam;		

		// Process this to ensure the common dlg static controls look as good
		// as possible on our custom portfolio background. We could explictly
		// do this only for the two static ctrl currently used,
		// (ie stc3 = File Name, stc4 = Look In), but by taking this general
		// approach, if any new static ctrls are added in future versions of
		// win95, they will also look ok. (Except for the fact they won't be
		// known to the current custom portfolio background). This assumes
		// that we never want any common dlg static controls to be drawn using
		// the default system colors, but that's ok due the extent of our
		// portfolio customization.

		// Do not overwrite the background when writing the static control text.
		SetBkMode(hdc, TRANSPARENT);

		// Note, on win95 that static text always appears black, regardless of
		// the current system color settings. However, rather than relying on
		// this always being true, set the text color explicitly here.
		SetTextColor(hdc, RGB(0, 0, 0));

		// Draw the control background in the light gray that matched the custom
		// background bitmap. Otherwise the background is drawn in the current
		// system color, which may not match the background at all.
		return((LONG)GetStockObject(LTGRAY_BRUSH));
		}
	case WM_SYSCOMMAND:
		{
		// Is a screen saver trying to start?
		if (wParam == SC_SCREENSAVE)
			{
			// Disable the screen saver if we don't allow them to run.
			if (!vpapp->FAllowScreenSaver())
				return fTrue;
			}

		break;
		}
	case WM_ERASEBKGND:
		{
		// Since the user never sees the common dialog background, ensure that 
		// it can never be erased in the system color. Force a repaint of the 
		// custom dlg now, to prevent the common dlg controls appearing before 
		// the portfolio background. Note that GetDlgItem(hwndDlg, <custom dlg id>)
		// returns zero here, as the Menu part of the custom dlg is zero.
		HWND hwndCustom = (HWND)GetWindowLong(hwndDlg, GWL_USERDATA);

		if(hwndCustom != 0)
			UpdateWindow(hwndCustom);

		return(1);
		}
	default:

		break;
		}

	return(CallWindowProc (lpDlgProc, hwndDlg, msg, wParam, lParam));
}


