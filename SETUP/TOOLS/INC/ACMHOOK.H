#pragma warning(disable:4001)

////////////////////////////////////////////////////////////////////////////////
//	File:		ACMHOOK.H
//	Purpose:	Types, data structures, and function definitions for
//				hooking out the standard acme ui
//	Notes:		I - input
//				O - output
////////////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif //__cplusplus

#define cbMaxSz 256						// maximum string buffer size
#define DECL __cdecl					// calling convention of hook procedures

typedef enum {							// Dialog Hook Return Code
	shrcOk,								// proceed with setup
	shrcCancel,							// cancel/quit button was pushed
	shrcIgnore,							// ignore button was pushed
	shrcOption,							// "change option" button was pushed
	shrcDirectory,						// "change directory" button was pushed
	shrcRefresh,						// activation change - refresh data
	shrcFail,							// error occurred - cancel setup
	shrcNone,							// don't remove any shared components
	shrcKeep,							// don't remove this shared component
	shrcRemoveAll,						// remove all app/shared components
	shrcRemove,							// remove this shared component
	shrcReinstall,						// re install components
	shrcFrameRegisterFail,				// error registering class
	shrcFrameCreateFail					// error creating frame window
	} SHRC;

typedef struct {						// Hook Data
	LONG cb;							// structure size (I)
	CHAR rgchHelpFile[cbMaxSz];			// help file (I)
	DWORD dwHelpContext;				// help context (I)
	} HD, *PHD;

typedef struct {						// Welcome Dialog Hook Data
	HD hd;								// common hook data
	BOOL fMaint;						// maintenace mode setup (I)
	} WDHD, *PWDHD;

typedef
	SHRC (DECL *LPFNSHRCWDH)(PWDHD);	// Welcome Dialog Hook

typedef struct {						// Modules in Use Dialog Hook Data
	HD hd;								// common hook data
	CHAR rgchModuleList[cbMaxSz];		// names of modules in use (I)
	} MUDHD, *PMUDHD;

typedef
	SHRC (DECL *LPFNSHRCMUDH)(PMUDHD);	// Modules in Use Dialog Hook

typedef enum {							// Copy Disincentive Dialog Hook Mode
	cddhmNameOrg,						// allow the name and org to be modified
	cddhmName,							// allow only the name to be modified
	cddhmOrg							// allow only the org to be modified
	} CDDHM;

typedef struct {						// Copy Disincentive Dialog Hook Data
	HD hd;								// common hook data
	CDDHM cddhm;						// CD mode (I)
	CHAR rgchName[cbMaxSz];				// name string (I/O)
	CHAR rgchOrg[cbMaxSz];				// organization string (I/O)
	} CDDHD, *PCDDHD;

typedef
	SHRC (DECL *LPFNSHRCCDDH)(PCDDHD);	// Copy Disincentive Dialog Hook

typedef enum {							// Serial Number Dialog Hook Mode
	sndhmNormal,						// the normal/default functionality
	sndhmCDKey,							// the cdkey functionality
	sndhmOEM							// the OEM functionality
	} SNDHM;

typedef struct {						// Serial Number Dialog Hook Data
	HD hd;								// common hook data
	CHAR rgchSerNum[cbMaxSz];			// serial number string (I/O)
	SNDHM sndhm;						// serial number dialog hook mode (I)
	} SNDHD, *PSNDHD;

typedef
	SHRC (DECL *LPFNSHRCSNDH)(PSNDHD);	// Serial Number Dialog Hook

typedef enum {							// Already Used Dialog Hook Mode
	audhmBad,							// setup.ini corrupted
	audhmOrg,							// organization used
	audhmName,							// name used
	audhmBoth							// both name and org used
	} AUDHM;

typedef struct {						// Already Used Dialog Hook Data
	HD hd;								// common hook data
	AUDHM audhm;						// mode (I)
	CHAR rgchName[cbMaxSz];				// name string (I)
	CHAR rgchOrg[cbMaxSz];				// organization string (I)
	} AUDHD, *PAUDHD;

typedef
	SHRC (DECL *LPFNSHRCAUDH)(PAUDHD);	// Already Used Dialog Hook

typedef struct {						// Disk Locked Dialog Hook Data
	HD hd;								// common hook data
	CHAR rgchDir[cbMaxSz];				// directory string (I)
	} DLDHD, *PDLDHD;

typedef
	SHRC (DECL *LPFNSHRCDLDH)(PDLDHD);	// Disk Locked Dialog Hook

typedef enum {							// Copy Disincentive Dialog Hook Mode
	cpdhmRemind,						// piracy reminder
	cpdhmWarn,							// piracy warning
	cpdhmWarn2							// nasty ;-) piracy warning
	} CPDHM;

typedef struct {						// CD Piracy Dialog Hook Data
	HD hd;								// common hook data
	CPDHM cpdhm;						// mode (I)
	} CPDHD, *PCPDHD;

typedef
	SHRC (DECL *LPFNSHRCCPDH)(PCPDHD);	// CD Piracy Dialog Hook

typedef enum {							// DiRectory Dialog Hook Mode
	drdhmNotExistDir,					// directory does not exist
	drdhmAppExistsOld,					// older version in directory
	drdhmAppExistsNew					// new version in directory
	} DRDHM;

typedef struct {						// DiRectory Dialog Hook Data
	HD hd;								// common hook data
	DRDHM drdhm;						// mode (I)
	CHAR rgchComponent[cbMaxSz];		// component name string (I)
	CHAR rgchPath[cbMaxSz];				// path (I)
	} DRDHD, *PDRDHD;

typedef
	SHRC (DECL *LPFNSHRCDRDH)(PDRDHD);	// DiRectory Dialog Hook

typedef struct {						// Get Path Dialog Hook Data
	HD hd;								// common hook data
	CHAR rgchFilename[cbMaxSz];			// filename string - may be "" (I)
	CHAR rgchVersion[cbMaxSz];			// version string - may be "" (I)
	CHAR rgchSrcDir[cbMaxSz];			// source distory string - may be "" (I)
	LONG cSubNodeMax;					// count of maximumum subnodes (I)
	BOOL fAllowExtendedChars;			// if extended characters allowed (I)
	CHAR rgchComponent[cbMaxSz];		// component name string - may be "" (I)
	BOOL fNeedNewEmptyDir;				// the directory should be empty (I)
	CHAR rgchPath[cbMaxSz];				// path to edit (I/O)
	} GPDHD, *PGPDHD;

typedef
	SHRC (DECL *LPFNSHRCGPDH)(PGPDHD);	// Get Path Dialog Hook

typedef struct {						// App Main BuTtoN
	CHAR rgchLabel[cbMaxSz];			// button label string (I)
	CHAR rgchText[cbMaxSz];				// description string (I)
	HBITMAP hbm;						// bitmap for button (I)
	} AMBTN, *PAMBTN;
	
typedef struct {						// App Main Dialog Hook Data
	HD hd;								// common hook data
	LONG iambtnSelected;				// which button selected (I/O)
	LONG cambtn;						// number of buttons (I)
	CHAR rgchPath[cbMaxSz];				// directory path (I)
	HGLOBAL hrgambtn;					// button data (I)
	} AMDHD, *PAMDHD;

typedef
	SHRC (DECL *LPFNSHRCAMDH)(PAMDHD);	// App Main Dialog Hook

typedef struct {						// ProgMan Dialog Hook Data
	HD hd;								// common hook data
	CHAR rgchGroup[cbMaxSz];			// suggested group name (I/O)
	LONG crgchExisting;					// number of existing groups (I)
	HGLOBAL hrgrgchExisting;			// existing group names (I)
										// [crgchExisting][cbMaxSz]
	} PMDHD, *PPMDHD;

typedef
	SHRC (DECL *LPFNSHRCPMDH)(PPMDHD);	// ProgMan Dialog Hook Data

typedef struct {						// ReBoot Dialog Hook Data
	HD hd;								// common hook data
	} RBDHD, *PRBDHD;

typedef
	SHRC (DECL *LPFNSHRCRBDH)(PRBDHD);	// ReBoot Dialog Hook

typedef struct {						// Reboot Failed Dialog Hook Data
	HD hd;								// common hook data
	} RFDHD, *PRFDHD;

typedef
	SHRC (DECL *LPFNSHRCRFDH)(PRFDHD);	// Reboot Failed Dialog Hook

typedef enum {							// eXiT Dialog Hook Mode
	xtdhmOk,							// installation was successful
	xtdhmError,							// installation failed
	xtdhmQuit							// installation was cancelled
	} XTDHM;

typedef struct {						// eXiT Dialog Hook  Data
	HD hd;								// common hook data
	XTDHM xtdhm;						// mode (I)
	} XTDHD, *PXTDHD;

typedef
	SHRC (DECL *LPFNSHRCXTDH)(PXTDHD);	// eXiT Dialog Hook 

typedef struct {						// Disk Space iTeMs
	CHAR rgchDesc[cbMaxSz];				// description string (I)
	CHAR rgchSize[cbMaxSz];				// size (I)
	} DSTM, *PDSTM;

typedef struct {						// Disk Space iTeMs
	LONG cbReq;							// bytes required on current drive (I)
	LONG cbNeed;						// bytes needed on current drive (I)
	} DSDRV, *PDSDRV;

typedef struct {						// Disk Space Dialog Hook Data
	HD hd;								// common hook data
	BOOL fAdmin;						// whether in admin mode or not (I)
	LONG idsdrvCur;						// the selected drive (I/O)
	LONG cdstm;							// number of items (I)
	HGLOBAL hrgdstm;					// the items
	LONG cdsdrv;						// number of drives (I)
	HGLOBAL hrgdsdrv;					// the drives
	} DSDHD, *PDSDHD;

typedef
	SHRC (DECL *LPFNSHRCDSDH)(PDSDHD);	// Disk Space Dialog Hook

typedef struct {						// OPtions Dialog iTeMs
	CHAR rgchTitle[cbMaxSz];			// title string (for listbox) (I)
	CHAR rgchDesc[cbMaxSz];				// description string (I)
	CHAR rgchDir[cbMaxSz];				// the directory string (I)
	LONG cb;							// number of bytes item requires (I)
	BOOL fChecked;						// if the item is chosen (I/O)
	BOOL fChangeOption;					// enable the "change option" button
	BOOL fChangeDirectory;				// enable the "change directory" button
	BOOL fVital;						// the item is vital
	} OPTM, *POPTM;

typedef struct {						// OPtions Dialog Hook Data
	HD hd;								// common hook data
	BOOL fMaint;						// maintenace mode setup (I)
	BOOL fTop;							// if top level dialog (I)
	LONG coptm;							// number of option items (I)
	LONG ioptmCur;						// currently selected item (I/O)
	CHAR rgchDrvCur[cbMaxSz];			// drive of currently selected item (I)
	LONG cbDrvCurReq;					// bytes needed on current drive (I)
	LONG cbDrvCurAvail;					// bytes available on current drive (I)
	LONG cToAdd;						// number of items to add (I)
	LONG cToRemove;						// number of items to remove (I)
	HGLOBAL hrgoptm;					// the option items (I)
	} OPDHD, *POPDHD;

typedef
	SHRC (DECL *LPFNSHRCOPDH)(POPDHD);	// OPtions Dialog Hook

typedef struct {						// Admin Warn Hook Data
	HD hd;								// common hook data
	} AWDHD, *PAWDHD;

typedef
	SHRC (DECL *LPFNSHRCAWDH)(PAWDHD);	// Admin Warn Hook

typedef struct {						// MsApps directory Hook Data
	HD hd;								// common hook data
	CHAR rgchPath[cbMaxSz];				// directory path (I)
	} MADHD, *PMADHD;

typedef
	SHRC (DECL *LPFNSHRCMADH)(PMADHD);	// MsApps directory Hook

typedef enum {							// Confirm Server Dialog Hook Mode'
	csdhmName,							// use server name
	csdhmLetter							// use drive letter
	} CSDHM;

typedef struct {						// Confirm Server Dialog Hook Data
	HD hd;								// common hook data
	CHAR rgchName[cbMaxSz];				// component name (I)
	CHAR rgchPath[cbMaxSz];				// directory path (I)
	CHAR rgchNetPath[cbMaxSz];			// network path (I/O)
	CHAR rgchServer[cbMaxSz];			// network server (I/O)
	CHAR chDrive;						// drive letter (I/O)
	CSDHM csdhm;						// use name or drive letter (O)
	} CSDHD, *PCSDHD;

typedef
	SHRC (DECL *LPFNSHRCCSDH)(PCSDHD);	// Confirm Server Dialog Hook

typedef enum {							// Shared Files Dialog Hook Mode
	sfdhmServer,						// put shared files on server;
	sfdhmLocal,							// put shared files on local hard drive
	sfdhmChoice							// user can choose in network mode
	} SFDHM;

typedef struct {						// Shared Files Dialog Hook Data
	HD hd;								// common hook data
	BOOL fAdmin;						// whether in admin mode or not (I)
	SFDHM sfdhm;						// shared files mode (O)
	} SFDHD, *PSFDHD;

typedef 
	SHRC (DECL *LPFNSHRCSFDH)(PSFDHD);	// Shared Files Dialog Hook

typedef struct {						// Remove Shared Dialog Hook Data
	HD hd;								// common hook data
	CHAR rgchComponent[cbMaxSz];		// component name (I)
	} RSDHD, *PRSDHD;

typedef
	SHRC (DECL *LPFNSHRCRSDH)(PRSDHD);	// Remove Shared Dialog Hook

typedef struct {						// Ask Quit Dialog Hook Data
	HD hd;								// common hook data
	} AQDHD, *PAQDHD;

typedef
	SHRC (DECL *LPFNSHRCAQDH)(PAQDHD);	// Ask Quit Dialog Hook

typedef struct {						// Invalid Net Path Dialog Hook Data
	HD hd;								// common hook data
	} INDHD, *PINDHD;

typedef
	SHRC (DECL *LPFNSHRCINDH)(PINDHD);	// Invalid Net Path Dialog Hook

typedef struct {						// No Connection Dialog Hook Data
	HD hd;								// common hook data
	} NCDHD, *PNCDHD;

typedef
	SHRC (DECL *LPFNSHRCNCDH)(PNCDHD);	// No Connection Dialog Hook

typedef enum {							// Start Modeless Dialog Hook Mode
	smdhmSearching,						// searching for components
	smdhmCheckingDisk,					// checking disk space
	smdhmModifyingSys					// modifying system
	} SMDHM;

typedef struct {						// Start Modeless Dialog Hook
	HD hd;								// common hook data
	SMDHM smdhm;						// modeless dialog type (I)
	HWND hwndParent;					// parent for modeless dialog (I)
	DWORD dwHook;						// value for End Modeless (O)
	} SMDHD, *PSMDHD;

typedef
	SHRC (DECL *LPFNSHRCSMDH)(PSMDHD);	// Start Modeless Dialog Hook

typedef struct {						// End Modeless Dialog Hook
	HD hd;								// common hook data
	DWORD dwHook;						// value from Start Modeless (I)
	} EMDHD, *PEMDHD;

typedef
	SHRC (DECL *LPFNSHRCEMDH)(PEMDHD);	// End Modeless Dialog Hook

typedef struct {						// Create Frame Hook Data
	LONG cbCFHD;						// structure size (I)
	WNDCLASS wc;						// default RegisterClass data (I)
	CREATESTRUCT cs;					// default CreateWindow data (I)
	HWND hwndFrame;						// new frame window handle (O)
	int nCmdShow;						// default parm for ShowWindow (I)
	} CFHD, *PCFHD;

typedef
	SHRC (DECL *LPFNSHRCCFH)(PCFHD);	// Create Frame Hook

typedef struct {						// Message Box Hook Data
	LONG cbMBHD;						// structure size (I)
	HWND hwndOwner;						// handle of owner window (I)
	LPCSTR lpszText;					// text in message box (I)
	LPCSTR lpszTitle;					// title of message box (I)
	UINT uStyle;						// style of message box (I)
	int idRet;							// return value of message box (O)
	} MBHD, *PMBHD;

typedef
	SHRC (DECL *LPFNSHRCMBH)(PMBHD);	// Message Box Hook

typedef struct {						// Gauge Init Hook Data
	LONG cbGIHD;						// structure size (I)
	} GIHD, *PGIHD;

typedef
	SHRC (DECL *LPFNSHRCGIH)(PGIHD);	// Gauge Init Hook

typedef struct {						// Gauge Open Hook Data
	LONG cbGOHD;						// structure size (I)
	HWND hwndParent;					// handle of parent (frame) window (I)
	BOOL *pfAbort;						// flag to set when cancel pushed
	} GOHD, *PGOHD;

typedef
	SHRC (DECL *LPFNSHRCGOH)(PGOHD);	// Gauge Open Hook

typedef struct {						// Gauge Close Hook Data
	LONG cbGCHD;						// structure size (I)
	} GCHD, *PGCHD;

typedef
	SHRC (DECL *LPFNSHRCGCH)(PGCHD);	// Gauge Close Hook

typedef struct {						// Gauge Title Hook Data
	LONG cbGTHD;						// structure size (I)
	CHAR rgchTitle[cbMaxSz];			// new title string (I)
	} GTHD, *PGTHD;

typedef
	SHRC (DECL *LPFNSHRCGTH)(PGTHD);	// Gauge Title Hook

typedef struct {						// Gauge teXt Hook Data
	LONG cbGXHD;						// structure size (I)
	CHAR rgchSrc[cbMaxSz];				// new source string (I)
	CHAR rgchDst[cbMaxSz];				// new destination string (I)
	} GXHD, *PGXHD;

typedef
	SHRC (DECL *LPFNSHRCGXH)(PGXHD);	// Gauge teXt Hook

typedef struct {						// Gauge Range Hook Data
	LONG cbGRHD;						// structure size (I)
	LONG iLim;							// new range limit (I)
	} GRHD, *PGRHD;

typedef
	SHRC (DECL *LPFNSHRCGRH)(PGRHD);	// Gauge Range Hook

typedef struct {						// Gauge Position Hook Data
	LONG cbGPHD;						// structure size (I)
	LONG iCur;							// new current position (I)
	} GPHD, *PGPHD;

typedef
	SHRC (DECL *LPFNSHRCGPH)(PGPHD);	// Gauge Position Hook

typedef struct {						// Gauge Delta Hook Data
	LONG cbGDHD;						// structure size (I)
	LONG dCur;							// delta to current position (I)
	} GDHD, *PGDHD;

typedef
	SHRC (DECL *LPFNSHRCGDH)(PGDHD);	// Gauge Delta Hook

typedef struct {						// Gauge Yield Hook Data
	LONG cbGYHD;						// structure size (I)
	} GYHD, *PGYHD;

typedef
	SHRC (DECL *LPFNSHRCGYH)(PGYHD);	// Gauge Yield Hook

typedef struct {						// Filecopy Warning Hook Data
	LONG cbFWHD;						// structure size (I)
	CHAR rgchPath[cbMaxSz];					// path to file being copied
	} FWHD, *PFWHD;

typedef
	SHRC (DECL *LPFNSHRCFWH)(PFWHD);	// Filecopy Warning Hook

#if defined(__cplusplus)
}
#endif //__cplusplus

#pragma warning(default:4001)
