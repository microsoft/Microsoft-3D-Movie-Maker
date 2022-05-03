/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/******************************************************************************

@doc    WINMAN

@mod    WINMAN.CPP |

        <cp> Copyright 1993 Microsoft Corporation - All rights reserved

*******************************************************************************

        Description: Sendak Window Manager functions

        Current Owner: *****

*******************************************************************************

        Known bugs: None

*******************************************************************************

        How it could be Improved:
        
*******************************************************************************

        Revision History:

        10/22/93    *****    Created
        12/22/93    *****    Moved to DLL
        05/02/94    *****      Added LoadGenResource 
        08/24/94    *****      Free lpSenstate->DiskName
        10/27/94    *****      Added key handler for showing PID
        04/25/95    *****    Win32 Port
    06/11/95    *****      Use registry
    07/01/95    *****    #ifdef'ed UnloadGenResource for Win16 only

******************************************************************************/


#include <windows.h>
#include <windowsx.h>
#include <ole2.h>
#include <sendak.h>
#include <debug.h>
#include <senutil.h> 
#include <credits.h>    // function to show PID
#include <utils.h>
#include <image.h>
#include "winman.h" 
#ifdef BUILD_CHECK
#include <stdlib.h>
#endif

//
// globals
//
#ifdef BUILD_CHECK
    RECT           rectScrnMatt;                 // size of the matt screen
    BOOL          fStickyWicket;             // stop things from going away when they loose focus
#endif

extern SENUTILSTATE SenUtilState;
extern HDC          gWinGDC;
extern void far *   gpWinGBits;
extern INFO_HEADER  gWinGInfoHeader;
extern BOOL         gfTopDown;

static WORD          gwDragCode=0;  // Might need to move these to SenState so they are Inst specific
static int           giDX,giDY;
static RECT          grOrig, grOutline;

/******************************************************************************

@func   BOOL | fGlobalReAllocPtr |

        A better realloc function implementation than windows has.  This
        will prevent your only copy of a memory pointer from being overwritten
        with NULL as can be the case if you use GlobalReAllocPtr.

@parm   LPVOID FAR * | lplpVoid | Pointer to the pointer you want to realloc.

@parm   DWORD        | dwNewSize | New size of the memory chunk.

@parm   UINT         | fuAlloc | Allocation flags (GMEM_MOVEABLE if omitted)

@rdesc  TRUE if successful, FALSE if there was an error.  In case of error
        the passed pointer is still valid and the caller will need to free the
        memory at some point.

******************************************************************************/

BOOL _loadds WINAPI fGlobalReAllocPtr(LPVOID FAR *lplpVoid, DWORD dwNewSize, UINT fuAlloc)
{
    LPVOID lpNew;

    lpNew = GlobalReAllocPtr(*lplpVoid, dwNewSize, fuAlloc);

    if (lpNew == NULL) return FALSE;

    *lplpVoid = lpNew;

    return TRUE;
}
/******************************************************************************

@func   BOOL | fInitIni |

        This function reads the INI file constants from the resource file
        string table into global memory.  These constants can be accessed
        through a set of external long pointers.  In the event of an error,
        fReleaseIni() should still be called release any allocated memory.
        
        Note that pathnames in the ini file do not have to have a trailing
        backslash (\).  When they are read into memory a backslash (\) will
        be added if needed.

        INI_INSTPATH  entry should be a full path (eg. C:\SENDAK)
        INI_USERS     entry can be either a filename or a full pathname.  If
                      it is just a filename, the INSTPATH will prefix it.
        INI_BOOKPATH  entry should be a full path (eg. D:\)
        INI_DICTIONARY entry should be just a filename (eg. RENCF200.DAT)
                    

        This fills these entries in SenState:
        
          lpszDiskName    (eg. MDRNWRLD, NATURE, HISTORY, GEOGRPHY)
          lpszInstPath    (eg c:\mskids\)
          lpszMMIOPath    (eg d:\nature.m12+)
          lpszMediaPath   (eg d:\media\)
          lpszDataPath    (eg d:\)

@rdesc  TRUE if successful, FALSE if there was an error

******************************************************************************/

BOOL _loadds WINAPI fInitIni(LPSENSTATE lpSenState)
{
    char    rgchTemp        [INI_MAX_ENTRY];
    char    rgchFilename    [INI_MAX_FILENAME];
  char    rgchEntry       [INI_MAX_ENTRYNAME];
 #ifndef WIN32   
    char    rgchSection     [INI_MAX_SECTIONNAME];
 #else
    char    rgchSubKey      [INI_MAX_ENTRY];
 #endif  //WIN32
    char    rgchBookName    [INI_MAX_FILENAME];
    LPSTR   lpszChar;   
    int     iSize;
    
    LPSTR   lpszDefault     = "BAD_INI";
    LPSTR   lpszMediaDir    = "MEDIA\\";


    DPF3("fInitIni: Entering ...\n");
    
    // Get Disk Name (eg MDRNWRLD)
    if ((iSize = LoadString(lpSenState->hInst, STR_DISKNAME,  rgchFilename, INI_MAX_DISKNAME)) == 0) return FALSE;
    if ( (lpSenState->lpszDiskName = (LPSTR)GlobalAllocPtr(GMEM_MOVEABLE, iSize+1)) == NULL ) return FALSE;
    lstrcpy(lpSenState->lpszDiskName, rgchFilename);
  
  #ifndef WIN32
    // First load INI filename and Section name
    if (!LoadString(lpSenState->hInst, INI_FILENAME,  rgchFilename, INI_MAX_FILENAME))     return FALSE;
    if (!LoadString(lpSenState->hInst, INI_SEC_PATHS, rgchSection,  INI_MAX_SECTIONNAME))  return FALSE;
  #else
    if (!LoadString(lpSenState->hInst, INI_SUBKEY, rgchSubKey,INI_MAX_ENTRY)) return FALSE;
  #endif  //WIN32
    // Get the install path (hard drive)
    if (!LoadString(lpSenState->hInst, INI_INSTPATH, rgchEntry, INI_MAX_ENTRYNAME)) return FALSE;
  #ifndef WIN32
    if ( (iSize = GetPrivateProfileString(  (LPCSTR)rgchSection,
                                            (LPCSTR)rgchEntry,
                                            (LPCSTR)lpszDefault,
                                            rgchTemp,
                                            INI_MAX_ENTRY,
                                            (LPCSTR)rgchFilename)) == 0 ) 
    {
        return FALSE;
    }
  #else
    DWORD dwType = REG_SZ, dwSize = INI_MAX_ENTRY;
    HKEY  hkResult;
    if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
    rgchSubKey,
    0,
    KEY_READ,
    &hkResult))
  {
    rgchTemp[0]=0; //make sure we have no string when we start
    RegQueryValueEx(hkResult, // handle of key to Read value for  
        rgchEntry,  // address of value to Read 
        0,  // reserved 
        &dwType,  // flag for value type 
        (unsigned char *)rgchTemp,  // address of value data 
        &dwSize   // size of value data 
        );
    
    iSize = strlen(rgchTemp);
    if (!iSize)
    {
       strcpy(rgchTemp, lpszDefault);  
     iSize = strlen(rgchTemp);
    }
    }
  else
    {
   #ifdef DEBUG
      char rgchDBOut[256];
    wsprintf(rgchDBOut, "Couldn't open SubKey: %s\n", rgchSubKey);
    OutputDebugString(rgchDBOut);
   #endif
    return FALSE;
  }
  #endif //WIN32
    
    if ( rgchTemp[iSize-1] != '\\')      // Ensure entry has \ on end of it
    {        
      rgchTemp[iSize++] = '\\';
      rgchTemp[iSize] = '\0';
    }

    // Save in our state structure
    if ( (lpSenState->lpszInstPath = (LPSTR)GlobalAllocPtr(GMEM_MOVEABLE, iSize+1)) == NULL ) return FALSE;
    lstrcpy(lpSenState->lpszInstPath, rgchTemp);

    // Get the user preferences file ini entry
    if (!LoadString(lpSenState->hInst, INI_USERS, rgchEntry, INI_MAX_ENTRYNAME)) return FALSE;
  
  #ifndef WIN32
    if ( (iSize = GetPrivateProfileString(  (LPCSTR)rgchSection,
                                            (LPCSTR)rgchEntry,
                                            (LPCSTR)lpszDefault,
                                            rgchTemp,
                                            INI_MAX_ENTRY,
                                            (LPCSTR)rgchFilename)) == 0 ) 
    {
        return FALSE;
    }
  #else
    dwSize = INI_MAX_ENTRY;
    rgchTemp[0]=0; //make sure we have no string when we start
    RegQueryValueEx(hkResult, // handle of key to Read value for  
        rgchEntry,  // address of value to Read 
        0,  // reserved 
        &dwType,  // flag for value type 
        (unsigned char *)rgchTemp,  // address of value data 
        &dwSize   // size of value data 
        );
    
    iSize = strlen(rgchTemp);
    if (!iSize)
    {
       strcpy(rgchTemp, lpszDefault);  
     iSize = strlen(rgchTemp);
    }
  #endif
    for (lpszChar = rgchTemp ; *lpszChar ; lpszChar++)  // Check if entry specifies a full pathname or just filename
      if ( (*lpszChar == '\\') || (*lpszChar == ':') ) break;
      
    if (*lpszChar == '\0')
        iSize += lstrlen(lpSenState->lpszInstPath);  // If just filename, we'll add InstPath to beginning

    // Save in our state structure
    if ( (SenUtilState.lpszUserPath = (LPSTR)GlobalAllocPtr(GMEM_MOVEABLE, iSize+1)) == NULL )   return FALSE;

    lstrcpy(SenUtilState.lpszUserPath, (*lpszChar == '\0') ? lpSenState->lpszInstPath : "");
    lstrcat(SenUtilState.lpszUserPath, rgchTemp);


#ifdef WIN32
    // take care of setting fSwitchRes
    if ( !LoadString(lpSenState->hInst, INI_SWITCHRES, rgchEntry, INI_MAX_ENTRYNAME) )    return FALSE;

    dwSize = INI_MAX_ENTRY;
    rgchTemp[0]=0; //make sure we have no string when we start
    RegQueryValueEx(hkResult, // handle of key to Read value for  
        rgchEntry,  // address of value to Read 
        0,  // reserved 
        &dwType,  // flag for value type 
        (unsigned char *)rgchTemp,  // address of value data 
        &dwSize   // size of value data 
        );

    switch (rgchTemp[0] - '0')
    {
      case 0:
        SenUtilState.fSwitchRes = 0; break;
      case 1:
        SenUtilState.fSwitchRes = 1; break;
      default:
        #ifdef _DEBUG
        DPF("fInitIni: Bad value in SwitchResolution reg entry\n");
        #endif
        return FALSE;
    }
#endif //win32

    // Load our book name and ini entry name
    if ( !LoadString(lpSenState->hInst, INI_BOOKPATH,  rgchEntry,      INI_MAX_ENTRYNAME) )    return FALSE;
    if ( !LoadString(lpSenState->hInst, STBL_BOOKNAME, rgchBookName,   INI_MAX_ENTRYNAME) )    return FALSE;
 
  #ifndef WIN32
    if ( (iSize = GetPrivateProfileString(  (LPCSTR)rgchSection,
                                            (LPCSTR)rgchEntry,
                                            (LPCSTR)lpszDefault,
                                            rgchTemp,
                                            INI_MAX_ENTRY,
                                            (LPCSTR)rgchFilename)) == 0 ) 
    {
        return FALSE;
    }
  #else
    dwSize = INI_MAX_ENTRY;
    rgchTemp[0]=0; //make sure we have no string when we start
    RegQueryValueEx(hkResult, // handle of key to Read value for  
        rgchEntry,  // address of value to Read 
        0,  // reserved 
        &dwType,  // flag for value type 
        (unsigned char *)rgchTemp,  // address of value data 
        &dwSize   // size of value data 
        );

    iSize = strlen(rgchTemp);
    if (!iSize)
    {
      strcpy(rgchTemp, lpszDefault);  
      iSize = strlen(rgchTemp);
    }

    RegCloseKey(hkResult);
  #endif

    if ( rgchTemp[iSize-1] != '\\')
    {        // Ensure entry has \ on end of it
        rgchTemp[iSize++] = '\\';
        rgchTemp[iSize] = '\0';
    }

    // Add size of bookname to total size!    
    iSize += lstrlen(rgchBookName) + 2;     // one for the '+' one for the EOS
        
    if ( (lpSenState->lpszMMIOPath = (LPSTR)GlobalAllocPtr(GMEM_MOVEABLE, iSize)) == NULL )     return FALSE;

    lstrcpy(lpSenState->lpszMMIOPath, rgchTemp);        // now has path
    lstrcat(lpSenState->lpszMMIOPath, rgchBookName);    // now has book name
    lstrcat(lpSenState->lpszMMIOPath, "+");             // DONE!

    iSize = lstrlen(rgchTemp) + lstrlen(lpszMediaDir) + 1;
    
    if ( (lpSenState->lpszMediaPath = (LPSTR)GlobalAllocPtr(GMEM_MOVEABLE, iSize)) == NULL )    return FALSE;
    
    lstrcpy(lpSenState->lpszMediaPath, rgchTemp);       // now has path
    lstrcat(lpSenState->lpszMediaPath, lpszMediaDir);   // now has directory
    
    // Form Other Data pathname (used by Dictionary)
    iSize = lstrlen(rgchTemp) + 1;
    if ( (lpSenState->lpszDataPath = (LPSTR)GlobalAllocPtr(GMEM_MOVEABLE, iSize)) == NULL )    return FALSE;

    lstrcpy(lpSenState->lpszDataPath, rgchTemp);        // now has path     
/*    
#ifdef BUILD_CHECK
     if ( (iSize = GetPrivateProfileString(  (LPCSTR)"SCREENMATT",
                                            (LPCSTR)"TOP",
                                            (LPCSTR)"0",
                                            rgchTemp,
                                            INI_MAX_ENTRY,
                                            (LPCSTR)rgchFilename)) == 0 ) 
      rectScrnMatt.top = 0;
    else
      rectScrnMatt.top = atoi(rgchTemp);
    if ( (iSize = GetPrivateProfileString(  (LPCSTR)"SCREENMATT",
                                            (LPCSTR)"LEFT",
                                            (LPCSTR)"0",
                                            rgchTemp,
                                            INI_MAX_ENTRY,
                                            (LPCSTR)rgchFilename)) == 0 ) 
      rectScrnMatt.left = 0;
    else
      rectScrnMatt.left = atoi(rgchTemp);  
    if ( (iSize = GetPrivateProfileString(  (LPCSTR)"SCREENMATT",
                                            (LPCSTR)"RIGHT",
                                            (LPCSTR)"0",
                                            rgchTemp,
                                            INI_MAX_ENTRY,
                                            (LPCSTR)rgchFilename)) == 0 ) 
      rectScrnMatt.right = 0;
    else
      rectScrnMatt.right = atoi(rgchTemp); 
    if ( (iSize = GetPrivateProfileString(  (LPCSTR)"SCREENMATT",
                                            (LPCSTR)"BOTTOM",
                                            (LPCSTR)"0",
                                            rgchTemp,
                                            INI_MAX_ENTRY,
                                            (LPCSTR)rgchFilename)) == 0 ) 
     rectScrnMatt.bottom = 0;
    else
      rectScrnMatt.bottom = atoi(rgchTemp); 
    GetPrivateProfileString(  (LPCSTR)"DEBUG",
                                            (LPCSTR)"STICKY",
                                            (LPCSTR)"0",
                                            rgchTemp,
                                            INI_MAX_ENTRY,
                                            (LPCSTR)rgchFilename);    
    if (rgchTemp[0] == '0')
      fStickyWicket = FALSE;
    else
      fStickyWicket = TRUE;
#endif  // BUILD_CHECK
*/        
    DPF3("fInitIni: Exiting ...\n");

    return TRUE;
}


/******************************************************************************

@func   void | fReleaseIni |

        This function frees the memory allocated by the fInitIni() function.
        Note that the long pointers to the INI constants will be invalid after
        this call.

@rdesc  TRUE if successful, FALSE if there was an error.

******************************************************************************/

void _loadds WINAPI fReleaseIni(LPSENSTATE lpSenState)
{
    if ( lpSenState->lpszDiskName )
    {
      GlobalFreePtr(lpSenState->lpszDiskName);
      lpSenState->lpszDiskName = NULL;
    }
    if ( lpSenState->lpszMediaPath )
    {
      GlobalFreePtr(lpSenState->lpszMediaPath);
      lpSenState->lpszMediaPath = NULL;
    }
    
    if ( lpSenState->lpszMMIOPath )   
    {
      GlobalFreePtr(lpSenState->lpszMMIOPath);
      lpSenState->lpszMMIOPath = NULL;
    }
    
    if ( lpSenState->lpszInstPath )
    {
      GlobalFreePtr(lpSenState->lpszInstPath);
      lpSenState->lpszInstPath = NULL;
    }
    
    if ( lpSenState->lpszDataPath )
    {
      GlobalFreePtr(lpSenState->lpszDataPath);
      lpSenState->lpszDataPath = NULL;
    }
    
    if ( SenUtilState.lpszUserPath )
    {
      GlobalFreePtr(SenUtilState.lpszUserPath);
      SenUtilState.lpszUserPath = NULL;
    }
}


/******************************************************************************

@func   void | vRegisterPalette |

        This function registers a window with Sendak's palette handling code.
        Any top level window that is drawn using any palettized colors must be
        registered so that it will be properly redrawn if the palette changes.

@parm   HWND | hWnd | Handle of the window to register

@parm   BOOL | bReg | TRUE = Register window, FALSE = Unregister window

******************************************************************************/

void _loadds WINAPI vRegisterPalette(HWND hWnd,BOOL bReg)
{
LPPALETTELIST lpPL,lpPrevPL;
LPSENSTATE    lpSenState = lpGetSenState();

lpPL = lpSenState->lpPalList;
lpPrevPL = NULL;

while(lpPL)                             // Locate existing entry
  if(lpPL->hWnd == hWnd) break;
  else {
    lpPrevPL = lpPL;
    lpPL = lpPL->lpNext;
    }

if(bReg) {                              // Register a new window

  if(lpPL) return;                      // If it already exists bail out

  lpPL = (LPPALETTELIST)GlobalAllocPtr(0,sizeof(PALETTELIST));
  if(lpPL == NULL) return;
  
  lpPL->hWnd = hWnd;                    // Add a new entry to our linked list
  lpPL->lpNext = lpSenState->lpPalList;
  
  lpSenState->lpPalList = lpPL;

  return;
  }
                                        // Delete a registeration
  
if(lpPL == NULL) return;                // If window isn't registered, bail out

if(lpPrevPL)                            // Remove deleted node from linked list
  lpPrevPL->lpNext = lpPL->lpNext;
else
  lpSenState->lpPalList = lpPL->lpNext;
  
GlobalFreePtr(lpPL);                    // Free memory of deleted node

return;
}



/******************************************************************************

@func   void | vInvalidateWindows |

        This function invalidates all registered palette windows.

******************************************************************************/

void _loadds WINAPI vInvalidateWindows()
{
LPPALETTELIST lpPL;

lpPL = lpGetSenState()->lpPalList;

while(lpPL) {                           // Traverse the linked list
  InvalidateRect(lpPL->hWnd,NULL,TRUE); // Invalidate entire window & erase bkgnd
  lpPL = lpPL->lpNext;
  }
}



/******************************************************************************

@func   void | OnPaletteChanged |

        WM_PALETTECHANGED message handler.

******************************************************************************/

void _loadds WINAPI OnPaletteChanged(HWND hWnd, HWND hWndPaletteChange)
{
LPPALETTELIST lpPL;
LPSENSTATE  lpSenState = lpGetSenState();

DPF4("WinMan OnPaletteChanged: Entering...\n");

//if(hWnd != lpSenState->hMattWnd)
//  SendMessage(lpSenState->hMattWnd, WM_PALETTECHANGED, (WPARAM)hWndPaletteChange, 0L);
        
lpPL = lpSenState->lpPalList;
if (lpPL)
{
  for(;;) {                           // Traverse the linked list
  
    if(lpPL->hWnd == hWndPaletteChange) break;  // If one of our windows caused change, do nothing

    lpPL = lpPL->lpNext;

    if(lpPL == NULL) {                    // When checked all list, we didn't cause palette change

      DPF3("WinMan OnPaletteChanged: Realizing our palette.\n");
  
      OnQueryNewPalette(hWnd);              // Realize our palette

      break;                                // no need to check rest of list
    }
  }
}

DPF4("WinMan OnPaletteChanged: Exiting...\n");
}
        

        
/******************************************************************************

@func   BOOL | OnQueryNewPalette |

        WM_QUERYNEWPALETTE message handler.

******************************************************************************/

BOOL _loadds WINAPI OnQueryNewPalette(HWND hWnd)
{
    HDC         hDC;
    HPALETTE    hOldPal,
                hPal;
    int         i=0;
    LPSENSTATE  lpSenState = lpGetSenState();
            
    DPF4("WinMan OnQueryNewPalette: Entering...\n");

    hPal = lpSenState->hPal;

    if (hPal) {  // Only realize our palette if we have one - fixes bugs when app is closed

//        if(hWnd != lpSenState->hMattWnd)
//          i = (int)SendMessage(lpSenState->hMattWnd, WM_QUERYNEWPALETTE, 0, 0L);
//        else
          {
          hDC = GetDC(hWnd);
          hOldPal = SelectPalette(hDC, hPal, FALSE);//(lpGetSenState()->fMediaPalette)? TRUE :  FALSE);  // Select our palette as foreground palette if not MCI device is open
          i = RealizePalette(hDC);                  // If realization changes ...
          ReleaseDC(hWnd,hDC);
          }
        
        if(i)
          vInvalidateWindows();           // Force repaint of our window
              
        DPF3("WinMan OnQueryNewPalette: Changed %d palette entries.\n",i);
    }
        
    DPF4("WinMan OnQueryNewPalette: Exiting...\n");
    return(i ? TRUE : FALSE);  // Indicate if we changed any palette entries
}


/******************************************************************************

@func   void | OnLButtonDown |

        WM_LBUTTONDOWN message handler for window dragging.

******************************************************************************/

void _loadds WINAPI OnLButtonDown(HWND hWnd, BOOL, int x, int y, UINT)
{
  GetWindowRect(hWnd, &grOutline);
  grOrig = grOutline;

  giDX = x;
  giDY = y;

  gwDragCode = 1;
  SetCapture(hWnd);                       // Capture mouse messages so we'll get button up
}



/******************************************************************************

@func   void | OnLButtonUp |

        WM_LBUTTONUP message handler for window dragging.

******************************************************************************/

void _loadds WINAPI OnLButtonUp(HWND hWnd, int, int, UINT)
{
  DPF3("WinMan OnLButtonUp: Entering...\n");
  if ( GetCapture() == hWnd )
  {
    HDC  hDC;
    ReleaseCapture();
  
    if(gwDragCode == 2)
    {
      hDC = GetDC(NULL);                    // Get screen DC
      DrawFocusRect(hDC,(LPRECT)&grOutline);
      ReleaseDC(NULL,hDC);
    }

    SetWindowPos(hWnd, (HWND)NULL, grOutline.left, grOutline.top, 0, 0,
                 SWP_SHOWWINDOW|SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOZORDER);
  
    gwDragCode = 0;
  }
}


/******************************************************************************

@func   void | OnMouseMove |

        WM_MOUSEMOVE message handler for window dragging.

******************************************************************************/

void _loadds WINAPI OnMouseMove(HWND hWnd, int x, int y, UINT)
{
  if( (GetCapture() == hWnd) && (gwDragCode) )
  {
    HDC     hDC;
    int     nx,ny;
    
    nx = grOutline.left + x - giDX;
    ny = grOutline.top + y - giDY;
    giDX = x;
    giDY = y;
  
    hDC = GetDC(NULL);                    // Get screen DC
    
    if ( (nx != grOutline.left) || (ny != grOutline.top) )
    {   // If new position is desired ...
      if(gwDragCode == 1)                 // If 1st move don't need to erase rectangle
        gwDragCode = 2;
      else
        DrawFocusRect(hDC, &grOutline); // Erase old rectangle

      OffsetRect(&grOutline, nx - grOutline.left, ny - grOutline.top);
      DrawFocusRect(hDC, &grOutline);
    }
    
    ReleaseDC(NULL,hDC);
  }
}


/******************************************************************************

@func   void FAR* | LoadGenResource |

        This function loads a Wave from the resource file associated with
        hInstance.  The resource id can be constructed using the
        MAKEINTRESOURCE() macro or can be a pointer to a string identifier.

@parm   HINSTANCE | hInst | Handle of module containing resource

@parm   LPCSTR | lpResourceID | Resource identifier

@parm   LPXSTR | lpType | Type of resource

@rdesc  On success returns a long pointer to a locked Resource. 

        On failure returns NULL.

******************************************************************************/

extern "C" void FAR* WINAPI LoadGenResource(HINSTANCE hInst,LPCSTR lpResource,
                                        LPCSTR lpType)
{
        HRSRC         hResource;
        HGLOBAL       hGbl;

        hResource = FindResource(hInst, lpResource, lpType);

        if(hResource == NULL) return(NULL);

        hGbl = LoadResource(hInst, hResource);

        DPF4(" Handle = 0x%X\n", hGbl);
        
        return (void FAR*)LockResource(hGbl);
}

/******************************************************************************

@func   void | UnloadGenResource |

        This function will unload the resource that was loaded by 
        the LoadGenResource() function.  This function should be
        called by an application before it exits to free the global memory the
        resource uses.  Note that windows will cache the resource and will not
        discard it from memory even if freed until the memory is compacted.

@parm   LPCSTR | lpResource | Pointer to the packed Wave obtained from
                               LoadGenResource()

@parm   BOOL | fDump | Flag to indicate that the resouce should be discarded
                       from memory and not cached.  This means that future
                       calls to LoadDibResource will be forced to obtain the
                       DIB from the executable file.  This is an optional
                       parameter and defaults to FALSE meaning the DIB is
                       cached.

******************************************************************************/

extern "C" void WINAPI UnloadGenResource(LPCSTR lpResource, BOOL fDump)
{
  // WIN32: I know this reduces the function to nothingness,
  //        but GlobalHandle() raises an exception under NT and Win95
  //        and according to the SDK32, Win32 apps don't need to
  //        free their resources.  FreeResource() is obsolete. [*****]
  //        I chose to make one change here as opposed to 6 or 7 changes
  //        elsewhere in the code.

#ifndef WIN32
    HGLOBAL       hGbl;
    
    if (lpResource)
    {
      hGbl = GlobalPtrHandle(lpResource);   // somebody screwed the 16bit build by
                                            // changing this to GlobalHandle!!!!!!
      GlobalUnlock(hGbl);

      // WIN32: No need to unlock the above under Win32, and in fact it raises an
      // exception

      // WIN32: FreeResrouce is a vestigule function

      FreeResource(hGbl);

      if (fDump) FreeResource(hGbl); // Second call forces dump
    }
#endif
}



/******************************************************************************

@func   BOOL | OnIsGadget |

        WM_ISGADGET message handler.  Returns TRUE.

******************************************************************************/

BOOL _loadds WINAPI OnIsGadget(HWND)
{
  return TRUE;
}





/******************************************************************************
                     
@func   BOOL | GetNextGadgetWindow |

        This function gets the next gadget window in the window manager's list.

@parm   HWND | hWnd | The current window.

@parm   HWND FAR * | lphwndNext | A pointer to where to stick the new window handle.

@rdesc  Returns true if there was a next window. 

******************************************************************************/

BOOL _loadds WINAPI GetNextGadgetWindow(HWND hWnd, HWND FAR *lphwndNext)
{
  HWND  hwndMatt = lpGetSenState()->hMattWnd;
  
  if (!IsWindowVisible(hwndMatt) || hWnd == hwndMatt)
    return FALSE;
    
  *lphwndNext = hWnd;
  for(;;) {
    *lphwndNext = GetWindow(*lphwndNext, GW_HWNDNEXT);
    if (!*lphwndNext)                                    break;     // No more windows
    if (!IsWindowVisible(*lphwndNext))                   continue;  // This window is not visible, go to the next one
      break;
    }
  return (*lphwndNext != NULL);
}



/******************************************************************************

@func   void | GetBackground |

        This function will copy into hdcMem the background that is supposed to
        be there relative to hWnd.

@parm   HWND | hWnd | The current window.

@parm   HDC | hdcMem | The dc to draw into.  There should be an offscreen
                       bitmap selected into the dc.

@parm   RECT | rcPaint | The rectangle (in hWnd's client coordinates) to
                         redraw.  The bitmap selected into hdcMem should be
                         this big.

******************************************************************************/

void GetBackground(HWND hWnd, HDC hdcMem, RECT rcPaint)
{
  HWND        hwndBelow;
  PAINTSTRUCT psPass;
  POINT       pt;

  // Get the background from the window behind us
  GetNextGadgetWindow(hWnd, &hwndBelow);
  psPass.hdc = hdcMem;
  psPass.fErase = FALSE;
  psPass.rcPaint = rcPaint;

  MapWindowPoints(hWnd, hwndBelow, (POINT FAR *) &psPass.rcPaint, 2);
  SetViewportOrgEx(hdcMem, -psPass.rcPaint.left, -psPass.rcPaint.top, &pt); 
  SendMessage(hwndBelow, WM_GADGETPAINT, 0, (LPARAM) &psPass);

  MapWindowPoints(hwndBelow, hWnd, (POINT FAR *) &psPass.rcPaint, 2);
  SetViewportOrgEx(hdcMem, pt.x, pt.y, NULL);
}



/******************************************************************************

@func   BOOL | DrawChildWindows |

        This function repaints a child window.

@parm   HWND | hWnd | The child window to repaint.

@parm   LPARAM | lParam | A pointer to the PAINTSTRUNT to paint into.

******************************************************************************/


BOOL CALLBACK DrawChildWindows(HWND hWnd, LPARAM lParam)
{
  PAINTSTRUCT FAR * lpps = (PAINTSTRUCT FAR *) lParam;
  RECT  rClient, rIntersect, rSave;
  
  if (!IsWindowVisible(hWnd))
    return TRUE;

  MapWindowPoints((HWND) lpps->fRestore, hWnd, (POINT FAR *) &lpps->rcPaint, 2);
  GetClientRect(hWnd, &rClient);
  if (IntersectRect(&rIntersect, &lpps->rcPaint, &rClient))
  {

    SetViewportOrgEx(lpps->hdc, -lpps->rcPaint.left, -lpps->rcPaint.top, NULL);
    rSave = lpps->rcPaint;
    lpps->rcPaint = rIntersect;
    SendMessage(hWnd, WM_GADGETPAINT, 0, lParam);
//    ValidateRect(hWnd, &rIntersect);
    lpps->rcPaint = rSave;
  }
  MapWindowPoints(hWnd, (HWND) lpps->fRestore, (POINT FAR *) &lpps->rcPaint, 2);
  return TRUE;
}




/******************************************************************************

@func   BOOL | fBeginGadgetPaint |

        Call this function at the beginning of a WM_GADGETPAINT handler.

@parm   HWND | hWnd | The window.

@parm   LPARAM | lParam | The lParam of the WM_GADGETPAINT message.

@parm   LPPAINTSTRUCT | lpps | A pointer to a paintstruct we can use.

@parm   HDC FAR * | lphDC | A pointer to an hdc that you will paint into.

@parm   DWORD FAR * | lpdw | A pointer to a DWORD we can use to store secret stuff.

@rdesc  Returns true if you actually need to repaint anything.
                              
******************************************************************************/

BOOL _loadds WINAPI fBeginGadgetPaint(HWND hWnd, LPARAM lParam, LPPAINTSTRUCT lpps, HDC FAR *lphDC, DWORD FAR *lpdw)
{
  RECT  rClient;

  
  DPF3("BeginGadgetPaint: GO-GO-GADGET-PAINT!!!\n");

  if (lParam)
  {
    // If we are supposed to draw into an offscreen bitmap, don't make another one!

    *lpps = *((PAINTSTRUCT FAR *) lParam);
    *lphDC = lpps->hdc;
  }
  else
  {
    HPALETTE  hPal = lpGetSenState()->hPal;
    
    // If this is an honest-to-gosh WM_PAINT message, we'd better shove our stuff into an
    // offscreen bitmap.
    BeginPaint(hWnd, lpps);
    SETPALETTE(lpps->hdc, hPal);

    // Make sure the clipping region is a simple rectangle
//    if (GetClipBox(lpps->hdc, &rClip) == COMPLEXREGION)
//    {
//      HRGN   hRgn = CreateRectRgnIndirect(&rClip);
//      int    i;
//      
//      DPF3("BeginGadgetPaint: Complex clipping region!  This won't do!\n");
//      i = SelectClipRgn(lpps->hdc, hRgn);
//      DeleteObject(hRgn);
//    }

    *lphDC = CreateCompatibleDC(lpps->hdc);
    SETPALETTE(*lphDC, hPal);

    // Create the offscreen bitmap to use

    *((HBITMAP FAR *) lpdw) = CreateCompatibleBitmap(lpps->hdc, lpps->rcPaint.right - lpps->rcPaint.left,
                                               lpps->rcPaint.bottom - lpps->rcPaint.top);

    // Select the offscreen bitmap into the offscreen DC
    SelectObject(*lphDC, *((HBITMAP FAR *) lpdw));
    
    // Set the origin so that this DC uses the same coordinate system as the window

    SetViewportOrgEx(*lphDC, -lpps->rcPaint.left, -lpps->rcPaint.top, NULL);
  }

  // Have the windows below us (and our children) fill in our offscreen bitmap
  GetBackground(hWnd, *lphDC, lpps->rcPaint);
  
  // Find out if this window lies within the invalid region
  GetClientRect(hWnd, &rClient);

  DPF3("BeginGadgetPaint: Exiting\n");
  return IntersectRect(&rClient, &lpps->rcPaint, &rClient);
}




/******************************************************************************

@func   BOOL | EndGadgetPaint |

        Call this function at the end of a WM_GADGETPAINT handler.

@parm   HWND | hWnd | The window.

@parm   LPARAM | lParam | The lParam of the WM_GADGETPAINT message.

@parm   LPPAINTSTRUCT | lpps | A pointer to a paintstruct we can use.

@parm   HDC FAR * | lphDC | A pointer to an hdc that you will paint into.

@parm   DWORD FAR * | lpdw | A pointer to a DWORD we can use to store secret stuff.

******************************************************************************/

void _loadds WINAPI EndGadgetPaint(HWND hWnd, LPARAM lParam, LPPAINTSTRUCT lpps, HDC FAR *lphDC, DWORD FAR *lpdw)
{
  PAINTSTRUCT psPass;
  POINT       pt;

  
  DPF3("EndGadgetPaint: Entering...\n");

  // Have our child windows (mmbuttons) draw themselves
  if (!IsRectEmpty(&lpps->rcPaint))
  {
    psPass.hdc = *lphDC;
    psPass.fErase = FALSE;
    psPass.rcPaint = lpps->rcPaint;
    psPass.fRestore = (BOOL) hWnd;

    GetViewportOrgEx(*lphDC, &pt); 
    EnumChildWindows(hWnd, DrawChildWindows, (LPARAM) &psPass);

    SetViewportOrgEx(*lphDC, pt.x, pt.y, NULL);
  }

  if (!lParam)
  {
    RECT  rClip;

    // If the clip region is complex, then get a DC to the window that isn't restricted.
    // This prevents child windows from not being fullly redrawn (they will validate the whole
    // rectangle when only parts of it are actually drawn to the screen).
    if (GetClipBox(lpps->hdc, &rClip) == COMPLEXREGION)
    {
      HDC  hdcWnd = GetDC(hWnd);
      
      // Copy the offscreen buffer to the screen
      if (hdcWnd)
      {
        BitBlt(hdcWnd, lpps->rcPaint.left, lpps->rcPaint.top, lpps->rcPaint.right - lpps->rcPaint.left,
                                                              lpps->rcPaint.bottom - lpps->rcPaint.top,
                                                              *lphDC, lpps->rcPaint.left, lpps->rcPaint.top, SRCCOPY);
        ReleaseDC(hWnd, hdcWnd);
      }
    }
    else
      // Copy the offscreen buffer to the screen
      BitBlt(lpps->hdc, lpps->rcPaint.left, lpps->rcPaint.top, lpps->rcPaint.right - lpps->rcPaint.left,
                                                            lpps->rcPaint.bottom - lpps->rcPaint.top,
                                                            *lphDC, lpps->rcPaint.left, lpps->rcPaint.top, SRCCOPY);
    if (*lphDC)
      DeleteDC(*lphDC);

    if (*((HBITMAP FAR *) lpdw))
      DeleteObject(*((HBITMAP FAR *) lpdw));

    EndPaint(hWnd, lpps);
  }
  else
    ValidateRect(hWnd, &lpps->rcPaint);

  DPF3("EndGadgetPaint: Exiting\n");
}  




/******************************************************************************

@func   void | RefreshScreenRect |

        Call this function to refresh a portion of the screen.

@parm   HWND | hwndTop | The topmost window.

@parm   RECT | rRefresh | The rectangle to refresh, in screen coordinates..

******************************************************************************/

void _loadds WINAPI RefreshScreenRect(HWND hwndTop, RECT rRefresh)
{
  HDC       hDC, hdcScreen;
  HBITMAP   hbm;
  PAINTSTRUCT psPass;
  POINT       pt;
    
  hDC = CreateCompatibleDC(NULL);
  SETPALETTE(hDC, lpGetSenState()->hPal);

  // Create the offscreen bitmap to use
  hdcScreen = GetDC(NULL);
  hbm = CreateCompatibleBitmap(hdcScreen, rRefresh.right - rRefresh.left,
                                    rRefresh.bottom - rRefresh.top);

  // Select the offscreen bitmap into the offscreen DC
  SelectObject(hDC, hbm);
    
  // Set the origin so that this DC uses the same coordinate system as the screen

  SetViewportOrgEx(hDC, -rRefresh.left, -rRefresh.top, NULL);
  
  // Have all the windows (including hWndTop) fill in our offscreen bitmap
  psPass.hdc = hDC;
  psPass.fErase = FALSE;
  psPass.rcPaint = rRefresh;
  MapWindowPoints(NULL, hwndTop, (POINT FAR *) &psPass.rcPaint, 2);
  SetViewportOrgEx(hDC, -psPass.rcPaint.left, -psPass.rcPaint.top, &pt); 
  SendMessage(hwndTop, WM_GADGETPAINT, 0, (LPARAM) &psPass);
  MapWindowPoints(hwndTop, NULL, (POINT FAR *) &psPass.rcPaint, 2);
  SetViewportOrgEx(hDC, pt.x, pt.y, NULL); 

  // Copy the offscreen buffer to the screen
  BitBlt(hdcScreen, rRefresh.left, rRefresh.top, rRefresh.right - rRefresh.left, rRefresh.bottom - rRefresh.top,
         hDC, rRefresh.left, rRefresh.top, SRCCOPY);

  if (hDC)
    DeleteDC(hDC);
  if (hbm)
    DeleteObject(hbm);
  if (hdcScreen)
    ReleaseDC(NULL, hdcScreen);
}



typedef struct
{
  POINT pt;
  HWND  hwndChild;
} HTENUM, FAR *LPHTENUM;


/******************************************************************************

@func   BOOL | HitTestChildWindows |

        This function hit tests a child window.

@parm   HWND | hWnd | The child window.

@parm   LPARAM | lParam | A pointer to a HTENUM struct.

@rdesc  Returns false if the point is in the window, true otherwise.

******************************************************************************/

BOOL CALLBACK HitTestChildWindows(HWND hWnd, LPARAM lParam)
{
  LPHTENUM lphtenum = (LPHTENUM) lParam;
  
  if (!IsWindowVisible(hWnd))
    return TRUE;
  MapWindowPoints(GetParent(hWnd), hWnd, &lphtenum->pt, 1);
  if (SendMessage(hWnd, WM_GADGETHITTEST, 0, (LPARAM) MAKELPARAM(lphtenum->pt.x, lphtenum->pt.y)))
  {
    lphtenum->hwndChild = hWnd;
    return FALSE;
  }
  else
  {
    MapWindowPoints(hWnd, GetParent(hWnd), &lphtenum->pt, 1);
    return TRUE;
  }
}




/******************************************************************************

@func   BOOL | ForwardMouseDown |

        This function forwards a mouse down to the appropriate window.

@parm   HWND | hWnd | The window that received the WM_LBUTTONDOWN.

@parm   BOOL | fDoubleClick | From the WM_LBUTTONDOWN message.

@parm   int | x | From the WM_LBUTTONDOWN message.

@parm   int | y | From the WM_LBUTTONDOWN message.

@parm   UINT | keyFlags | From the WM_LBUTTONDOWN message.

******************************************************************************/

void _loadds WINAPI ForwardMouseDown(HWND hWnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
  HWND    hwndBelow;
  POINT   pt;
  RECT    rClient;
  HTENUM  htenum;
  
  pt.x = x;
  pt.y = y;
  hwndBelow = hWnd;
  while (GetNextGadgetWindow(hwndBelow, &hwndBelow))
  {
    MapWindowPoints(hWnd, hwndBelow, &pt, 1);
    GetClientRect(hwndBelow, &rClient);
    if (PtInRect(&rClient, pt))
    {
      // Check for a child window hit
      htenum.pt = pt;
      htenum.hwndChild = NULL;
      EnumChildWindows(hwndBelow, HitTestChildWindows, (LPARAM) &htenum);
      if (htenum.hwndChild)
      {
        BringWindowToTop(hwndBelow);
        UpdateWindow(hwndBelow);
        SendMessage(htenum.hwndChild, (fDoubleClick ? WM_LBUTTONDBLCLK : WM_LBUTTONDOWN),
                    (WPARAM) keyFlags, (LPARAM) MAKELPARAM(htenum.pt.x, htenum.pt.y));
        break;
      }
      // Check for parent window hit
      else if (SendMessage(hwndBelow, WM_GADGETHITTEST, 0, (LPARAM) MAKELPARAM(pt.x, pt.y)))
      {
        BringWindowToTop(hwndBelow);
        SendMessage(hwndBelow, (fDoubleClick ? WM_LBUTTONDBLCLK : WM_LBUTTONDOWN),
                    (WPARAM) keyFlags, (LPARAM) MAKELPARAM(pt.x, pt.y));
        break;
      }
    }
    MapWindowPoints(hwndBelow, hWnd, &pt, 1);
  }
}   




/******************************************************************************

@func   BOOL | ForwardMouseUp |

        This function forwards a mouse up to the appropriate window.

@parm   HWND | hWnd | The window that received the WM_LBUTTONUP.

@parm   BOOL | fDoubleClick | From the WM_LBUTTONUP message.

@parm   int | x | From the WM_LBUTTONUP message.

@parm   int | y | From the WM_LBUTTONUP message.

@parm   UINT | keyFlags | From the WM_LBUTTONUP message.

******************************************************************************/

void _loadds WINAPI ForwardMouseUp(HWND hWnd, int x, int y, UINT keyFlags)
{
  HWND    hwndBelow;
  POINT   pt;
  RECT    rClient;
  HTENUM  htenum;
  
  pt.x = x;
  pt.y = y;
  hwndBelow = hWnd;
  while (GetNextGadgetWindow(hwndBelow, &hwndBelow))
  {
    MapWindowPoints(hWnd, hwndBelow, &pt, 1);
    GetClientRect(hwndBelow, &rClient);
    if (PtInRect(&rClient, pt))
    {
      // Check for a child window hit
      htenum.pt = pt;
      htenum.hwndChild = NULL;
      EnumChildWindows(hwndBelow, HitTestChildWindows, (LPARAM) &htenum);
      if (htenum.hwndChild)
      {
        SendMessage(htenum.hwndChild, WM_LBUTTONUP, (WPARAM) keyFlags, (LPARAM) MAKELPARAM(pt.x, pt.y));
        break;
      }
      // Check for parent window hit
      else if (SendMessage(hwndBelow, WM_GADGETHITTEST, 0, (LPARAM) MAKELPARAM(pt.x, pt.y)))
      {
        SendMessage(hwndBelow, WM_LBUTTONUP, (WPARAM) keyFlags, (LPARAM) MAKELPARAM(pt.x, pt.y));
        break;
      }
    }
    MapWindowPoints(hwndBelow, hWnd, &pt, 1);
  }
}

/**********************************************************************
@func void | Winman_OnKey | Shows PID when the correct key combo is pressed.

***********************************************************************/
void _loadds Winman_OnKey(HWND hwnd, UINT vk, BOOL fDown, int, UINT) 
{                                                                                                                                                                     
if( (vk == 'I') &&
    (fDown) &&               
    (GetKeyState(VK_CONTROL) < 0) &&
    (GetKeyState(VK_SHIFT) < 0) )

  vShowPid(hwnd);
}

void _loadds vClearWinGBuffer()
{
  COPYBITSPARM  copyParms;
  RECT          rcWinG      = {0, 0, 640, 480};
  LPSENSTATE    lpSenState  = lpGetSenState();
  
  copyParms.pBufferHeader   = (LPBITMAPINFO)&gWinGInfoHeader;
  copyParms.pBufferBits     = (char huge *)gpWinGBits;
  copyParms.iXOriginSrc     = 0;
  copyParms.iYOriginSrc     = 0;
  copyParms.fReverseSource  = gfTopDown;
  
  if ( lpSenState->lpTitleAction )
  {
    if ( (lpSenState->lpTitleAction)(SENSPAM_COPY_BITMAP, (DWORD)&rcWinG, (DWORD)&copyParms) == FALSE )
    {
      // if failed make it BLACK!
      PatBlt(gWinGDC, rcWinG.left, rcWinG.top, rcWinG.right, rcWinG.bottom, BLACKNESS);
    }
  }
}

void _loadds ClientToWinG(HWND hWnd, POINT *ppt)
{
  /* Although once we ship this code would work (asumming every
     gadget is centered on the screen) this prevents us from
     using a smaller mask window which we need for testing.
     If you don't have a full screen mask you get a GP fault in the
     TransCopyDIBBits() function because the function that calls it,
     TransparentDIBits(), can pass -ve destination origins.

  ClientToScreen(hWnd, ppt);
  ppt->x -= (GetSystemMetrics(SM_CXSCREEN) - 640)/2;
  ppt->y -= (GetSystemMetrics(SM_CYSCREEN) - 480)/2;
  */

// What we really want to do is convert from our window's coordinates to
// its parent coordinates (since this function is called from mmb code
// where we're placing buttons onto the gadget)
  HWND hWndUse;
  hWndUse = FindWindow("ExploraStack", NULL);
  
  if ( !hWndUse )
    hWndUse = FindWindow("Visor", NULL);  
    
  if ( !hWndUse )
    hWndUse = FindWindow("ExploraCredits", NULL);  

  if ( hWndUse )
    MapWindowPoints(hWnd, hWndUse, ppt, 1);
}

BOOL _loadds WINAPI fShouldActivateSPAM()
{
  LPSTR lpszStackClass    = "ExploraStack";
  LPSTR lpszCreditsClass  = "ExploraCredits";
  LPSTR lpszVisorClass    = "Visor";
  
  if (  FindWindow(lpszStackClass, NULL) ||
        FindWindow(lpszCreditsClass, NULL) ||
        FindWindow(lpszVisorClass, NULL) )
  {
    return FALSE;
  }
  else
    return TRUE;
}

