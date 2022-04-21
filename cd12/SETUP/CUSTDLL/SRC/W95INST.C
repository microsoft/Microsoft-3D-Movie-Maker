/***************************************************************************
**
**	File:		CODECINS.C
**	Purpose:	Installs the video for windows and audio CODECs
**			under Windows 95.
**
**	Dependencies:	This custom action uses RUNDLL.EXE to call SETUPX.DLL
**			which are part of the Windows95 setup that is installed
**			on a users machine.  Please see geninst.txt for some
**			more information on these files.
**
**	STF Syntax:
**		ObjID	Batch	Title		Descr		Type		Data						Bmp Id	Vital	Shared	DirChng	DestDir	CheckDir	InstBy	InstData	InstDir
**		ID#		"Error Title"	"Error Text"		CustomAction	"EXPSETUP.DLL","EnsureWin95CodecsInstalled",PARAMS
**
**	  where PARAMS is either nothing, VFW or Aud to ensure both, VFW only or AUDIO only codecs are installed
**
**	Author: ******
**	Written For:	Explorapedia Win32/Win16 versions of Nature and People
**
****************************************************************************/


#define FILEMOVE_C

#include <windows.h>
#include <stdlib.h>		/* _MAX_PATH , atol() */
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"

/*
To determine if ICM (VFW codecs) or ACM (Audio codecs) are installed
you (warning I'm not on the Win95 team - I'm a dev lead in Consumer):

X=RegEntryValue("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Setup\OptionalComponents\acm")
(X will be "acm" in the current builds of Win95).

Y=RegEntryValue("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Setup\OptionalComponents\X\Installed")

If Y is "0" acm is not installed.  If Y is "1" then it is installed.

Likewise for ICM.


If you need to install a component you get:

INF=RegEntryValue(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Curren Version\Setup\OptionalComponents\X\INF)

Section=RegEntryValue(HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Cu rentVersi
on\Setup\OptionalComponents\X\Section)

use INF and Section in the RunDll.exe setupx.dll command line below
*/


/* To ask Windows95 to install an optional component you WinExec the command line:

	RunDll.exe setupx.dll,InstallHinfSection <section> <reboot mode> <inf name>

   reboot modes are in defines section below */


//
// Defines
//

	// RUNDLL.EXE - SETUPX.DLL InstallHinfSection reboot modes

#define HOW_NEVER_REBOOT  		"0"	// Whatever happens the machine will not be rebooted.
						// Its up to the client to determin if the machine should
						// be rebooted. For setup, this means that there is a file
						// c:\windows\wininit.ini that is not zero bytes in size.

#define HOW_ALWAYS_SILENT_REBOOT	"1"	// The user will not be asked, the machine will just reboot - MIS win (?)

#define HOW_ALWAYS_PROMPT_REBOOT	"2"	// The user will be asked to Reboot the machine, Yes/No.

#define HOW_SILENT_REBOOT		"3"	// If setup determins that we have to reboot, we will with
						// no user interaction

#define HOW_PROMPT_REBOOT		"4"	// If setup determins that we have to reboot, we will ask first Yes/No.

	
	// Windows 95 registry subclasses (from Win95 sdk file regstr.h)

#define REGSTR_MAX_VALUE_LENGTH     256
#define REGSTR_PATH_SETUP	"Software\\Microsoft\\Windows\\CurrentVersion"
#define REGSTR_KEY_SETUP	"\\Setup"

	// Windows 95 registry subclass we need (gleemed from using Regedit on Win95)

#define REGSTR_PATH_OPTCOMP	REGSTR_PATH_SETUP REGSTR_KEY_SETUP "\\OptionalComponents"
#define REGSTR_NAME_INSTALLED	"Installed"
#define REGSTR_NAME_INF		"INF"
#define REGSTR_NAME_SECTION	"Section"

	// value names found in HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\Curren Version\Setup\OptionalComponents

#define REGSTR_NAME_ACM		"acm"
#define REGSTR_NAME_ICM		"icm"

	// Command line to install Optional Components under Windows 95

#define INST_CMD_LINE	"RunDll.exe setupx.dll,InstallHinfSection " // <section> <reboot mode> <inf name>

	// Supported components	(index into array that follows)

#define INSTCOMP_ACM	0	// Audio codecs
#define INSTCOMP_ICM	1	// Video codecs
	
const char InstCompName[][4] =
	{
	REGSTR_NAME_ACM,
	REGSTR_NAME_ICM
	};



//
// Function prototypes
//

RC SETUPAPI EnsureWin95CodecsInstalled( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData );


RC SETUPAPI rcInstallWin95Comp(BYTE bCompID);

DWORD SETUPAPI dwGetDWfromValue(BYTE *pBuf, DWORD dwType);


#define SzDirFromPcamfd(pcamfd)	((SZ)(((PCAMFDDtor)(pcamfd))->lpvPriv))

#define RcCallBack(pcd, or, camf, pcamfd) \
			((*(pcd)->pfncacb)(or, camf, pcamfd))




/*************************************************************************
**  Name:
**      EnsureWin95CodecsInstalled
**
**  Purpose:
**      Custom Action Handler to ensure that VFW and Audio CODECs are
**	installed under Windows95
**
**  Arguments:
**      pcd   : Ptr to class data.
**      pod   : Ptr to object data.
**      camf  : Custom action member function type.
**      pcamfd: Ptr to custom action member function data
**          (variable size, depends on camf type).
**	szData: Ptr to 3rd param of object data from STF
**
**  Returns:
**      Appropriate RC (return code) value.
**
****************************************************************************/
RC FAR PASCAL EnsureWin95CodecsInstalled ( PCD pcd, POD pod, CAMF camf, PCAMFD pcamfd, SZ szData )
{
RC rc = rcDoDefault;

switch (camf)
	{
	case camfDoVisualMods:
		{
		if(pcd->wvtWinVerType == wvtChicago)
			{
			rc = rcInstallWin95Comp(INSTCOMP_ACM);
			if(rcOk == rc)
				{
				rc = rcInstallWin95Comp(INSTCOMP_ICM);
				}
			}

		else if(pcd->wvtWinVerType == wvtWinNT)
			{
			}

		break;
		}
	}

if(rcFail == rc)
  DoMsgBox(pod->szDescription, pod->szTitle, MB_OK | MB_ICONEXCLAMATION);

return (rc);
}



/*************************************************************************
**  Name:
**      rcInstallWin95Comp
**
**  Purpose:
**      Called by EnsureWin95CodecsInstalled to ensure that the identified
**	Optional Component is installed on the user's system under Windows95
**
**  Arguments:
**	bCompID	- component ID (INSTCOMP_???)
**
**  Returns:
**      Appropriate RC (return code) value.
**
****************************************************************************/

RC SETUPAPI rcInstallWin95Comp(BYTE bCompID)
{
RC	rc = rcFail;
DWORD	dwType=0;
char	szValue[REGSTR_MAX_VALUE_LENGTH];
char	szSubKey[REGSTR_MAX_VALUE_LENGTH*2];

	// Get the subkey name for component

GetRegKeyValue32 ( HKEY_LOCAL_MACHINE,	// handle of root key
			  REGSTR_PATH_OPTCOMP,	// subkey path
			  InstCompName[bCompID], // value name
			  &dwType,		// pointer to DWORD for value type
			  szValue,		// pointer to buf for value to be placed into
			  REGSTR_MAX_VALUE_LENGTH);	// size of the buffer
Assert(dwType == REG_SZ);

if(*szValue)		// If we obtained the component subkey name ...
	{
	SzStrCopy(szSubKey, REGSTR_PATH_OPTCOMP);
	SzStrCat(szSubKey, "\\");
	SzStrCat(szSubKey, szValue);

	GetRegKeyValue32 ( HKEY_LOCAL_MACHINE,	// handle of root key
				  szSubKey,		// subkey path
				  REGSTR_NAME_INSTALLED, // value name
				  &dwType,		// pointer to DWORD for value type
				  szValue,		// pointer to buf for value to be placed into
				  REGSTR_MAX_VALUE_LENGTH);	// size of the buffer

	if(!dwGetDWfromValue(szValue, dwType))	// If the component is not installed then lets install it
		{
		char			szCommandLine[512] = INST_CMD_LINE;
		STARTUPINFO		si;
		PROCESS_INFORMATION	pi;
		DWORD			dwExitCode;

		// Command line should look like this:
		//   RunDll.exe setupx.dll,InstallHinfSection <section> <reboot mode> <inf name>

			// Add <section>
		GetRegKeyValue32 ( HKEY_LOCAL_MACHINE,	// handle of root key
					  szSubKey,		// subkey path
					  REGSTR_NAME_SECTION,	// value name
					  &dwType,		// pointer to DWORD for value type
					  szValue,		// pointer to buf for value to be placed into
					  REGSTR_MAX_VALUE_LENGTH);	// size of the buffer
		Assert(dwType == REG_SZ);

		SzStrCat(szCommandLine, szValue);

			// Add <reboot mode>
		SzStrCat(szCommandLine, " " HOW_NEVER_REBOOT " ");

			// Add <inf name>
		GetRegKeyValue32 ( HKEY_LOCAL_MACHINE,	// handle of root key
					  szSubKey,		// subkey path
					  REGSTR_NAME_INF,	// value name
					  &dwType,		// pointer to DWORD for value type
					  szValue,		// pointer to buf for value to be placed into
					  REGSTR_MAX_VALUE_LENGTH);	// size of the buffer
		Assert(dwType == REG_SZ);

		SzStrCat(szCommandLine, szValue);


			// Now execute the command line to cause the actual install
		si.cb			= sizeof(STARTUPINFO);
		si.lpReserved		= NULL;
		si.lpDesktop		= NULL;
		si.lpTitle		= NULL;
		si.dwFlags		= 0;
		si.cbReserved2		= 0;
		si.lpReserved2		= NULL;

		if(CreateProcess(
				NULL,     		//address of module name
				szCommandLine,		//address of command line
				NULL,     		//process security
				NULL,     		//thread security
				FALSE,    		//inherit handles?
				DETACHED_PROCESS,	//creation flags
				NULL,			//address of new environment
				NULL,			//address of current directory
				&si,			//STARTUPINFO
				&pi))			//PROCESS_INFORMATION
			{
				// Wait until process is finished
			WaitForSingleObject(pi.hProcess, INFINITE);  

				// Close the process down
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);

				// obtain the exit code from the process
			GetExitCodeProcess(pi.hProcess, &dwExitCode);
			
			// dwExitCode always seems to be 4 for acm and icm

				// Tell Acme95 we now need to restart
			ForceRestartOn();
			
			#ifdef _DEBUG
			{
			char szBuf[700];
			wsprintf(szBuf, "Completed running '%s'.  Exit code=%lx", (LPSTR)szCommandLine, dwExitCode);
			DoMsgBox(szBuf, "rcInstallWin95Comp", MB_OK);
			}
			#endif

			rc = rcOk;
			}
		} // end of component not installed
	else
		{ // component is installed
		#ifdef _DEBUG
		SzStrCat(szSubKey, "=TRUE");
		DoMsgBox(szSubKey, "rcInstallWin95Comp", MB_OK);
		#endif
		rc = rcOk;
		}
	} // end of obtaining component sub-key name

return(rc);
}



/*************************************************************************
**  Name:
**      dwGetDWfromValue(BYTE *pBuf, DWORD dwType)
**
**  Purpose:
**      Since Windows95 is inconsistent with its use of strings or DWORD
**	registry types for flags this function always returns a DWORD by
**	using the dwType param to determine how to do that
**
**  Arguments:
**	pBuf	Pointer to buffer that contains value
**	dwType	RegQueryValueEx() value type identifier
**
**  Returns:
**      DWORD
**
****************************************************************************/

DWORD SETUPAPI dwGetDWfromValue(BYTE *pBuf, DWORD dwType)
{
DWORD dwRet;

switch(dwType)
	{
	case REG_DWORD:			//A 32-bit number.
		dwRet = *((DWORD *)pBuf);
		break;

	case REG_DWORD_BIG_ENDIAN:	//A 32-bit number in big-endian format.
		dwRet = ((DWORD)pBuf[0] << 24) | ((DWORD)pBuf[1] << 16) | ((DWORD)pBuf[2] << 8) | ((DWORD)pBuf[3]);
		break;

	case REG_EXPAND_SZ:		//A null-terminated string that contains unexpanded references to environment variables (for example, "%PATH%"). It will be a Unicode or ANSI string depending on whether you use the Unicode or ANSI functions.
	case REG_SZ:			//A null-terminated string. It will be a Unicode or ANSI string depending on whether you use the Unicode or ANSI functions.
	case REG_MULTI_SZ:		//An array of null-terminated strings, terminated by two null characters.
		dwRet = atol(pBuf);
		break;

	case REG_LINK:			//A Unicode symbolic link.
	case REG_BINARY:		//Binary data in any form.
	case REG_NONE:			//No defined value type.
	case REG_RESOURCE_LIST:		//A device-driver resource list.
	default:
		dwRet = 0L;
		break;
	}

return(dwRet);
}
