/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/******************************************************************************

@doc    AUTORUN

@mod    AUTORUN.CPP |

        <cp> Copyright 1995 Microsoft Corporation - All rights reserved

*******************************************************************************

        Description: This module contains functions to implement the autorun
        stub as documented in autorun.doc.  Flow chart as follows:

          32bit      - NO -->   Run Setup
              installed?
                  |
                 YES
                  |
          running?   - YES ->   Quit
                  |
                  NO
                  |
              Run 32bit.

        Current Owner: *****

*******************************************************************************

        Known bugs: None

*******************************************************************************

        How it could be Improved:

*******************************************************************************

        Revision History:

        06/27/95  *****  Created

******************************************************************************/
#include <windows.h>
#include "tlhelp32.h"         // in the sdk/h directory
#include "resource.h"

typedef struct
{
    HINSTANCE     hInstance;            // Instance of launched app
    HTASK         hTask;                // Task of launched app
    HWND          hWnd;                 // Window of launched app
} LINKEDAPP, FAR *LPLINKEDAPP;


typedef BOOL    (WINAPI *MODULEWALK)    (HANDLE hSnapshot, LPMODULEENTRY32 lpme);
typedef BOOL    (WINAPI *THREADWALK)    (HANDLE hSnapshot, LPTHREADENTRY32 lpte);
typedef BOOL    (WINAPI *PROCESSWALK)   (HANDLE hSnapshot, LPPROCESSENTRY32 lppe);
typedef HANDLE  (WINAPI *CREATESNAPSHOT)(DWORD dwFlags, DWORD th32ProcessID);

CREATESNAPSHOT  pCreateToolhelp32Snapshot = NULL;   // pointer to CreateToolhelp32Snapshot function
PROCESSWALK     pProcess32First           = NULL;   // pointer to Process32First function
PROCESSWALK     pProcess32Next            = NULL;   // pointer to Process32Next function
MODULEWALK      pModule32First            = NULL;   // pointer to Module32First function
MODULEWALK      pModule32Next             = NULL;   // pointer to Module32Next function
THREADWALK      pThread32First            = NULL;
THREADWALK      pThread32Next             = NULL;

BOOL GetThreadProcess(DWORD, DWORD, LPPROCESSENTRY32, DWORD);
BOOL GetProcessModule(DWORD, DWORD, LPMODULEENTRY32, DWORD);


// taken from the pview95 code in the sdk32.  enumerates Win32 modules

BOOL GetProcessModule(DWORD dwPID, DWORD dwModuleID, LPMODULEENTRY32 lpMe32, DWORD cbMe32)
{
    BOOL          bRet        = FALSE;
    BOOL          bFound      = FALSE;
    HANDLE        hModuleSnap = NULL;
    MODULEENTRY32 me32        = {0};

    // Take a snapshot of all modules in the specified process.

    hModuleSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

    if (hModuleSnap == (HANDLE)-1)
    {
        return (FALSE);
    }

    // Size of the MODULEENTRY32 structure must be initialized before use!

    me32.dwSize = sizeof(MODULEENTRY32);

    // Walk the module list of the process and find the module we are
    // interested in.  Then, copy the information to the buffer pointed to
    // by lpMe32 so that we can return it to the caller.

    if (pModule32First(hModuleSnap, &me32))
    {
        do
        {
            if (me32.th32ModuleID == dwModuleID)
            {
                CopyMemory (lpMe32, &me32, cbMe32);
                bFound = TRUE;
            }
        } while (!bFound && pModule32Next(hModuleSnap, &me32));

        bRet = bFound;   // If this sets bRet to FALSE, then dwModuleID
                         // no longer exists in the specified process.
    }
    else
    {
        bRet = FALSE;           // Couldn't walk module list.
    }

    // Don't forget to clean up the snapshot object...

    CloseHandle (hModuleSnap);

    return (bRet);
}

BOOL GetThreadProcess(DWORD dwThreadID, DWORD dwPID, LPPROCESSENTRY32 lpPe32, DWORD cbPe32)
{
    BOOL          bRet        = FALSE;
    BOOL          bFound      = FALSE;
    HANDLE        hProcessSnap = NULL;
    PROCESSENTRY32 pe32        = {0};

    // Take a snapshot of all processes in the system.

    hProcessSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == (HANDLE)-1)
    {
        return (FALSE);
    }

    // Size of the PROCESSENTRY32 structure must be initialized before use!

    pe32.dwSize = sizeof(PROCESSENTRY32);

    // Walk the process list and find the process we are
    // interested in.  Then, copy the information to the buffer pointed to
    // by lpPe32 so that we can return it to the caller.

    if (pProcess32First(hProcessSnap, &pe32))
    {
        do
        {
            if (pe32.th32ProcessID == dwPID)
            {
                CopyMemory (lpPe32, &pe32, cbPe32);
                bFound = TRUE;
            }
        } while (!bFound && pProcess32Next(hProcessSnap, &pe32));

        bRet = bFound;   // If this sets bRet to FALSE, then dwPID
                         // no longer exists in the specified process.
    }
    else
    {
        bRet = FALSE;           // Couldn't walk process list.
    }

    // Don't forget to clean up the snapshot object...

    CloseHandle (hProcessSnap);

    return (bRet);
}

/******************************************************************************

@func   BOOL | EnumTaskWndProc |

        This callback function finds the first top-level window of a particular
        task.

@parm   HWND | hWnd | Window to examine.

@parm   LPARAM | lParam | Pointer to a LINKEDAPP structure containing the
                          module instance.

@rdesc  Returns true (always finds the first top-level window).

******************************************************************************/

BOOL CALLBACK EnumTaskWndProc(HWND hWnd, LPARAM lParam)
{
    LPLINKEDAPP lpla;

    // This is needed because some apps (i.e. MSWord) have other, invisible top-level windows (God only knows what for!)
    if (!IsWindowVisible(hWnd))
      return TRUE;

    lpla = (LPLINKEDAPP) lParam;
    lpla->hWnd = hWnd;
    return FALSE;
}

BOOL IsAppRunning(LPSTR lpszPath, LPLINKEDAPP lpla)
{
    BOOL            fRet                      = FALSE;  // return value
    HANDLE          hKernel32                 = NULL;

    hKernel32 = GetModuleHandle("KERNEL32.DLL");

    if (hKernel32)
    {
      DWORD          dwExitCode;
      HANDLE         hProcessSnap = NULL;       // snapshot of all processes
      PROCESSENTRY32 pe32         = { 0 };
      THREADENTRY32  te32         = { 0 };

      pCreateToolhelp32Snapshot = (CREATESNAPSHOT)GetProcAddress((HMODULE)hKernel32, "CreateToolhelp32Snapshot");
      pProcess32First           = (PROCESSWALK)   GetProcAddress((HMODULE)hKernel32, "Process32First");
      pProcess32Next            = (PROCESSWALK)   GetProcAddress((HMODULE)hKernel32, "Process32Next");
      pModule32First            = (MODULEWALK)    GetProcAddress((HMODULE)hKernel32, "Module32First");
      pModule32Next             = (MODULEWALK)    GetProcAddress((HMODULE)hKernel32, "Module32Next");
      pThread32First            = (THREADWALK)    GetProcAddress((HMODULE)hKernel32, "Thread32First");
      pThread32Next             = (THREADWALK)    GetProcAddress((HMODULE)hKernel32, "Thread32Next");

      // all function pointers are valid

      if (pCreateToolhelp32Snapshot && pProcess32First && pProcess32Next &&
          pModule32First && pModule32Next && pThread32First && pThread32Next )
      {
        if (!IsWindow(lpla->hWnd) ||                                        // is there a window
            !(GetExitCodeProcess(lpla->hTask, &dwExitCode) &&               // can we get the process status
              dwExitCode == STILL_ACTIVE) ||                                // and is the process still active
            GetWindowTask(lpla->hWnd) != lpla->hTask)                       // is this the same task
        {
          // now we have to find another instance of this process, if one exists
          // Take a snapshot of all threads currently in the system.

          hProcessSnap = pCreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);

          if (hProcessSnap != (HANDLE)-1)
          {
            // Size of the PROCESSENTRY32 structure must be filled out before use.

            pe32.dwSize = sizeof(PROCESSENTRY32);
            te32.dwSize = sizeof(THREADENTRY32);

            // Walk the snapshot of processes and for each process, get info

            if (pThread32First(hProcessSnap, &te32))
            {
              MODULEENTRY32 me32;

              do
              {
                GetThreadProcess(te32.th32ThreadID, te32.th32OwnerProcessID, &pe32, sizeof(PROCESSENTRY32));
                GetProcessModule(pe32.th32ProcessID, pe32.th32ModuleID, &me32, sizeof(MODULEENTRY32));

                if (lstrcmpi((LPSTR)me32.szExePath, lpszPath) == 0)
                {
                  // found it!! Now need to fill in lpla struct
                  lpla->hWnd = NULL;
                  EnumThreadWindows(te32.th32ThreadID, (WNDENUMPROC) EnumTaskWndProc, (LPARAM)lpla);
                  fRet = TRUE;
                }
              } while (!fRet && pThread32Next(hProcessSnap, &te32));

              CloseHandle (hProcessSnap);
            }
          }
        }
      }
    }

    return fRet;
}


#define INI_MAX_ENTRY 120

BOOL FGetExePath(HINSTANCE hInstance, int isz, char rgchExePath[])
{
	char rgchSubKey[INI_MAX_ENTRY];
	char rgchValue[INI_MAX_ENTRY];
	HKEY  hkResult;
	DWORD dwType = REG_SZ, dwSize = INI_MAX_ENTRY;
	long cch;

	if (!LoadString(hInstance, isz, rgchSubKey, INI_MAX_ENTRY))
		return FALSE;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, rgchSubKey, 0, KEY_READ, &hkResult) != ERROR_SUCCESS)
		return FALSE;

	if (!LoadString(hInstance, isz+1, rgchValue, INI_MAX_ENTRY))
		return FALSE;
	rgchExePath[0] = '\0';

	RegQueryValueEx(hkResult, rgchValue, 0, &dwType,
		(unsigned char *)rgchExePath, &dwSize);

	if (rgchExePath[0] == '\0')
		return FALSE;

	if (isz == STBL_APPINSTALLKEY)
		{
		cch = strlen(rgchExePath);
	
		if (!LoadString(hInstance, isz+2, rgchValue, INI_MAX_ENTRY))
			return FALSE;
	
		RegQueryValueEx(hkResult, rgchValue, 0, &dwType,
			(unsigned char *)(&rgchExePath[cch]), &dwSize);
	
		if (rgchExePath[cch] == '\0')
			return FALSE;
	
		cch = strlen(rgchExePath);

		if (!LoadString(hInstance, STBL_APPINSTALLNAME, &rgchExePath[cch],
				INI_MAX_ENTRY - cch))
			{
			return FALSE;
			}

		}

	return TRUE;
}


/***************************************************************************
	WinMain
***************************************************************************/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  char  rgchPath  [INI_MAX_ENTRY * 2];
  int isz;
  BOOL  fRunSetup = FALSE;
  char rgchSetupClass[INI_MAX_ENTRY];


  if (!LoadString(hInstance, STBL_SETUPCLASS, rgchSetupClass, INI_MAX_ENTRY))
	  return FALSE;

  if (FindWindow(rgchSetupClass, NULL))
	  return 1;

  isz = STBL_APPPATHKEY;
  while (!fRunSetup)
	{
	if (FGetExePath(hInstance, isz, rgchPath))
		{
		LINKEDAPP la;
		
		la.hInstance = NULL;
		la.hTask = NULL;
		la.hWnd = NULL;
		
		if (IsAppRunning(rgchPath, &la))
			{
			if (IsIconic(la.hWnd))
				ShowWindow(la.hWnd, SW_RESTORE);
			
			SetForegroundWindow(la.hWnd);
	
			break;
			}
		else // Not running, so run it
			{
			PROCESS_INFORMATION pi;
			STARTUPINFO si;
			
			si.cb = sizeof(STARTUPINFO);
			si.lpReserved = NULL;
			si.lpDesktop  = NULL;
			si.lpTitle    = NULL;
			si.dwFlags    = STARTF_USESHOWWINDOW;
			si.wShowWindow = SW_SHOWDEFAULT;
			si.cbReserved2 = 0;
			si.lpReserved2 = NULL;
			
			if (CreateProcess(rgchPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
				{
				break;
				}
			}
		}
	if (isz == STBL_APPPATHKEY)
		isz = STBL_APPINSTALLKEY;
	else
		fRunSetup = TRUE;
	}
	
	if (fRunSetup) // App path not found or CreateProcess failed, Not installed
  {
    char rgchSetupPath[_MAX_PATH];
    char rgchSetupFName[_MAX_FNAME];
    char *pch;
    int i;

    if (!LoadString(hInstance, STBL_SETUPFNAME, rgchSetupFName, _MAX_FNAME)) return FALSE;

	/* Relative path? */
	if (rgchSetupFName[0] == '.')
		{
	    GetModuleFileName(NULL, rgchSetupPath, _MAX_PATH);

	    pch = strchr(rgchSetupPath, ' ');
	    if (NULL != pch)
	    {
	      *pch = '\0';
	    }

	    i = strlen(rgchSetupPath);
	    pch = rgchSetupPath + i;

	    while (pch != rgchSetupPath && *pch != '\\')
	      pch--;

	    *(pch+1) = '\0';

    	strcat(rgchSetupPath, rgchSetupFName);
		}
	else
		strcpy(rgchSetupPath, rgchSetupFName);

    PROCESS_INFORMATION pi;
    STARTUPINFO si;

    si.cb = sizeof(STARTUPINFO);
    si.lpReserved = NULL;
    si.lpDesktop  = NULL;
    si.lpTitle    = NULL;
    si.dwFlags    = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOWDEFAULT;
    si.cbReserved2 = 0;
    si.lpReserved2 = NULL;

    // Start setup
    CreateProcess(rgchSetupPath, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	}

  return 1;
}
