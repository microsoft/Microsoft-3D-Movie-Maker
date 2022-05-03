/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	Portfolio related includes.

	Primary Author: ******
	Review Status: Not yet reviewed

***************************************************************************/

// Top level portoflio routines.
bool FPortGetFniMovieOpen(FNI *pfni);
bool FPortDisplayWithIds(FNI *pfni, bool fOpen, long lFilterLabel, long lFilterExt, long lTitle, LPTSTR lpstrDefExt, PSTN pstnDefFileName, FNI *pfniInitialDir, ulong grfPrevType, CNO cnoWave);
bool FPortGetFniOpen(FNI *pfni, LPTSTR lpstrFilter, LPTSTR lpstrTitle, FNI *pfniInitialDir, ulong grfPrevType, CNO cnoWave);
bool FPortGetFniSave(FNI *pfni, LPTSTR lpstrFilter, LPTSTR lpstrTitle, LPTSTR lpstrDefExt, PSTN pstnDefFileName, ulong grfPrevType, CNO cnoWave);

UINT CALLBACK OpenHookProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);
void OpenPreview(HWND hwnd, PGNV pgnvOff, RCS *prcsPreview);
void RepaintPortfolio(HWND hwndCustom);

static WNDPROC lpBtnProc;
LRESULT CALLBACK SubClassBtnProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC lpPreviewProc;
LRESULT CALLBACK SubClassPreviewProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static WNDPROC lpDlgProc;
LRESULT CALLBACK SubClassDlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef struct dlginfo
	{
		bool fIsOpen; 		// fTrue if Open file, (ie not Save file)
		bool fDrawnBkgnd;	// fTrue if portfolio background bitmap has been displayed.
		RCS rcsDlg;			// Initial size of the portfolio common dlg window client area.
		ulong grfPrevType;	// Bits for types of preview required, (eg movie, sound etc) == 0 if no preview
		CNO cnoWave;		// Wave file cno for audio when portfolio is invoked.
	}
	DLGINFO;
typedef DLGINFO * PDLGINFO;

enum
	{
	fpfNil = 0x0000,
	fpfPortPrevMovie = 0x0001,
	fpfPortPrevSound = 0x0002,
	fpfPortPrevTexture = 0x0004
	};

// The remainder of this file contains temporary stuff which are defined
// in the win95 include files which we are not using yet.

// From WINUSER.H...
typedef struct tagNMHDR
{
    HWND  hwndFrom;
    UINT  idFrom;
    UINT  code;         // NM_ code
}   NMHDR;
typedef NMHDR FAR * LPNMHDR;

// From COMMDLG.H...
#define OFN_EXPLORER                 0x00080000     // new look commdlg

#define SC_CONTEXTHELP  0xF180

// Structure used for all OpenFileName notifications
typedef struct _OFNOTIFYA
{
        NMHDR           hdr;
        LPOPENFILENAMEA lpOFN;
        LPSTR           pszFile;        // May be NULL
} OFNOTIFYA, FAR *LPOFNOTIFYA;
// Structure used for all OpenFileName notifications
typedef struct _OFNOTIFYW
{
        NMHDR           hdr;
        LPOPENFILENAMEW lpOFN;
        LPWSTR          pszFile;        // May be NULL
} OFNOTIFYW, FAR *LPOFNOTIFYW;
#ifdef UNICODE
typedef OFNOTIFYW OFNOTIFY;
typedef LPOFNOTIFYW LPOFNOTIFY;
#else
typedef OFNOTIFYA OFNOTIFY;
typedef LPOFNOTIFYA LPOFNOTIFY;
#endif // UNICODE

#define CDN_FIRST   (0U-601U)
#define CDN_LAST    (0U-699U)

// Notifications when Open or Save dialog status changes
#define CDN_INITDONE            (CDN_FIRST - 0x0000)
#define CDN_SELCHANGE           (CDN_FIRST - 0x0001)
#define CDN_FOLDERCHANGE        (CDN_FIRST - 0x0002)
#define CDN_SHAREVIOLATION      (CDN_FIRST - 0x0003)
#define CDN_HELP                (CDN_FIRST - 0x0004)
#define CDN_FILEOK              (CDN_FIRST - 0x0005)
#define CDN_TYPECHANGE          (CDN_FIRST - 0x0006)

#define WM_NOTIFY                       0x004E
#define WM_HELP                         0x0053

#define CDM_FIRST       (WM_USER + 100)
#define CDM_LAST        (WM_USER + 200)

#define SNDMSG SendMessage

// lParam = pointer to text buffer that gets filled in
// wParam = max number of characters of the text buffer (including NULL)
// return = < 0 if error; number of characters needed (including NULL)
#define CDM_GETSPEC             (CDM_FIRST + 0x0000)
#define CommDlg_OpenSave_GetSpecA(_hdlg, _psz, _cbmax) \
        (int)SNDMSG(_hdlg, CDM_GETSPEC, (WPARAM)_cbmax, (LPARAM)(LPSTR)_psz)
#define CommDlg_OpenSave_GetSpecW(_hdlg, _psz, _cbmax) \
        (int)SNDMSG(_hdlg, CDM_GETSPEC, (WPARAM)_cbmax, (LPARAM)(LPWSTR)_psz)
#ifdef UNICODE
#define CommDlg_OpenSave_GetSpec  CommDlg_OpenSave_GetSpecW
#else
#define CommDlg_OpenSave_GetSpec  CommDlg_OpenSave_GetSpecA
#endif // !UNICODE

#define CDM_GETFILEPATH         (CDM_FIRST + 0x0001)
#define CommDlg_OpenSave_GetFilePathA(_hdlg, _psz, _cbmax) \
        (int)SNDMSG(_hdlg, CDM_GETFILEPATH, (WPARAM)_cbmax, (LPARAM)(LPSTR)_psz)
#define CommDlg_OpenSave_GetFilePathW(_hdlg, _psz, _cbmax) \
        (int)SNDMSG(_hdlg, CDM_GETFILEPATH, (WPARAM)_cbmax, (LPARAM)(LPWSTR)_psz)
#ifdef UNICODE
#define CommDlg_OpenSave_GetFilePath  CommDlg_OpenSave_GetFilePathW
#else
#define CommDlg_OpenSave_GetFilePath  CommDlg_OpenSave_GetFilePathA
#endif // !UNICODE

// lParam = not used
// wParam = ID of control to change
// return = not used
#define CDM_HIDECONTROL         (CDM_FIRST + 0x0005)
#define CommDlg_OpenSave_HideControl(_hdlg, _id) \
        (void)SNDMSG(_hdlg, CDM_HIDECONTROL, (WPARAM)_id, 0)

// lParam = pointer to a string
// wParam = ID of control to change
// return = not used
#define CDM_SETCONTROLTEXT      (CDM_FIRST + 0x0004)
#define CommDlg_OpenSave_SetControlText(_hdlg, _id, _text) \
        (void)SNDMSG(_hdlg, CDM_SETCONTROLTEXT, (WPARAM)_id, (LPARAM)(LPSTR)_text)

