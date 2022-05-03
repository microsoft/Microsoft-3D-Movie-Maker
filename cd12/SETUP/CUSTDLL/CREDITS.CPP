/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/*******************************************************************
 *
 * CREDITS.CPP
 *
 * Copyright (C) Microsoft Corporation 1993.
 * All Rights reserved.
 *
 *******************************************************************
 *
 * Description: Sendak main stack procedures.
 *
 * Current Owner: *****
 *
 * *****: This module needs a code review in a bad way!!
 *
 *******************************************************************
 *
 * Revision History:
 * 09/01/94 *****    Created
 * 10/09/94 *****  Modified PID box code so the text is in the
 *                    disc resource file (eg Nature.rc)
 * 10/14/94 *****  Changed gadget cursor to fly with wings in
 * 04/14/95 *****    MediaView 1.4 port
 * 04/19/95 *****    MediaView & 16bit clean up
 * 04/25/95 *****  Win32 port
 * 06/20/95 *****  Created PID dialog for System Info button
 * 07/01/95 *****  Fixed resource modification and memory allocation bugs in PID DlgProc
 * 07/03/95 *****  locked PID dialog's colors
 * 07/06/95 *****  modified version-checking to use import libs
 * 07/07/95 *****  Fixed PID dialog, accepts default buttons and hotkeys
 * 07/10/95 *****  Modified PID for version 2.0 of PID
 * 07/11/95 *****  Fixed minor bug preventing PID 2.0 text from being visible
 * 07/11/95 *****  Fixed centering of PID after res-switch and task-switch
 * 07/12/95 *****  PID box uses Arial font
 * 07/17/95 *****  Credits screen rejects screen saver
 *
 ******************************************************************/

#include <windows.h>
#include <windowsx.h>
#include <ole2.h> 
#include <medv14.h>
#include <mvttl14.h>
#include <sendak.h>
#include <senutil.h> 
#include <image.h>
#include <debug.h>
#include <listman.h>
#include <stack.h>
#include <stackinf.h>
#include <winman.h>
#include <mmbutton.h> 
#include <missions.h>
#include <utils.h>
#include <find.h>
#include <wing.h>
#include "credits.h"                   
#include <slmver.h>

#ifdef WIN32
  #include <winver.h>
#else
  #include <ver.h>
#endif

#include <malloc.h>

#define OK_X                    566
#define OK_Y                    409

// function protos
BOOL  fLoadCreditResources(void);
void  vTermCreditResources(void);

BOOL CALLBACK _loadds fPIDDlg(HWND, UINT, WPARAM, LPARAM);

// Globals
BOOL    gfCreditsMagSet = FALSE;


extern  SENUTILSTATE  SenUtilState;

LPBITMAPINFO          lpbmGadget    = NULL;
LPBITMAPINFO          lpbmOK        = NULL;
LPBITMAPINFO          lpbmSeeAlso   = NULL;
LPBITMAPINFO          lpbmMore      = NULL;
LPBITMAPINFO          lpbmLess      = NULL;

HTITLE                ghCreditTitle;
FINDINFO              CreditInfo;

extern HDC          gWinGDC;
extern INFO_HEADER  gWinGInfoHeader;
extern void far *   gpWinGBits;

#define GWL_LPCREDITMV            0

#define CMD_CREDITPREV  101
#define CMD_CREDITMORE  102

#define CHILD_ID        103

#define CREDITS_DX      (POPUP_DX - 2)


/******************************************************************/
/*
 @doc   EXTERNAL

 @func  RC | rcShowCredits | 
        Entry point into displaying the credits.
        
 @parm  HWND | hWndParent |
        Parent window handle.
        
 @rdesc A return code.
*/

RC WINAPI _loadds rcShowCredits(HWND hWndParent) 
{
  char  rgchClassName[MAX_CLASS_NAME];
  RECT  rect;
  
  // If missions up... disable this!
  if ( hGetMissionsWnd(NULL) )
    return RC_OK;
  
  // WIN32: Must center based on matt window for Win95 resolution-changing/task-switching bug

  GetWindowRect(lpGetSenState()->hMattWnd, &rect);
  rect.left += (rect.right -rect.left - STACK_WIDTH) / 2;
  rect.top += (rect.bottom -rect.top - STACK_HEIGHT) / 2;
  
  // Shut down SPAM prior to creating the WINDOW!
  (lpGetSenState()->lpTitleAction)(SENSPAM_SPAM_ENABLE, FALSE, 0);
  
  if ( gWinGDC == NULL )
  {
    DPF("rcShowCredits Initing the WinG bitmap\n");
    if ( !fInitWinG(SenUtilState.hInst, NULL, 640, 480) )
      return RC_NOMEM;
  }
  
  if ( !fLoadCreditResources() )
    goto error;
    
  if ( !LoadString(SenUtilState.hInst, STBL_CLS_CREDITS, rgchClassName, MAX_CLASS_NAME) )
    goto error;
  
  if ( (CreateWindow(rgchClassName, rgchClassName,
                      WS_POPUP | WS_VISIBLE,
                      rect.left, rect.top, 
                      STACK_WIDTH, STACK_HEIGHT,
                      hWndParent, (HMENU)NULL,
                      SenUtilState.hInst,
                      NULL)) == NULL )
  {
    DPF("rcShowCredits: Error, could not create Credits Window\n");
    goto error;
  }
  
  // Set our StackWinG dirty flag!
  fStackWinGDirty = TRUE;    

  // Clear out our WING buffer!
  vClearWinGBuffer();
  return RC_OK;

error:
  // Free any bitmaps that may have been created!
  vTermCreditResources();
  
  // Free our WinG thing
  DPF("rcShowCredits terminating WinG bitmap\n");
  vTermWinG(NULL);
  
  // Activate SPAM upon this error!
  (lpGetSenState()->lpTitleAction)(SENSPAM_SPAM_ENABLE, TRUE, 0);
  return RC_NOMEM;
}

BOOL fLoadCreditResources()
{
  // Load our bitmaps!
  if ( (lpbmGadget  = lpLoadDibResource(SenUtilState.hInst, MAKEINTRESOURCE(BM_GADGET))) == NULL )
    return FALSE;

  if ( (lpbmOK      = lpLoadDibResource(SenUtilState.hInst, MAKEINTRESOURCE(BM_OK))) == NULL )
    return FALSE;
    
  if ( (lpbmSeeAlso = lpLoadDibResource(SenUtilState.hInst, MAKEINTRESOURCE(BM_SEEALSO))) == NULL )
    return FALSE;
    
  if ( (lpbmMore    = lpLoadDibResource(SenUtilState.hInst, MAKEINTRESOURCE(BM_MORE))) == NULL )
    return FALSE;
    
  if ( (lpbmLess    = lpLoadDibResource(SenUtilState.hInst, MAKEINTRESOURCE(BM_LESS))) == NULL )
    return FALSE;
    
  return TRUE;
}

void vTermCreditResources()
{
  if ( lpbmGadget )
  {
      vUnloadDibResource(lpbmGadget);
      lpbmGadget = NULL;
  }

  if ( lpbmOK )
  {
      vUnloadDibResource(lpbmOK);
      lpbmOK = NULL;
  }

  if ( lpbmSeeAlso )
  {
      vUnloadDibResource(lpbmSeeAlso);
      lpbmSeeAlso = NULL;
  }
  
  if ( lpbmMore )
  {
      vUnloadDibResource(lpbmMore);
      lpbmMore = NULL;
  }
  
  if ( lpbmLess )
  {
      vUnloadDibResource(lpbmLess);
      lpbmLess = NULL;
  }
}

/******************************************************************/
/*
 @doc   EXTERNAL

 @func  LRESULT | lCreditsWndProc |
        Main Window procedure for credits.

 @param HWND | hWnd |
        Window handle.
        
 @param UINT | message |
        Window message.
        
 @param WPARAM | wParam |
        Message dependant parameter.
        
 @param LPARAM | lParam |
        Message dependant parameter.
    
 @rdesc Message dependant.
*/

extern "C" LRESULT CALLBACK lCreditsWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
  switch (wMessage)
  {
		//HANDLE_MSG(hWnd, WM_KILLFOCUS,					CreditsWnd_OnKillFocus);
    HANDLE_MSG(hWnd, WM_CREATE,             CreditsWnd_OnCreate);
    HANDLE_MSG(hWnd, WM_DESTROY,            CreditsWnd_OnDestroy);
    HANDLE_MSG(hWnd, WM_PAINT,              CreditsWnd_OnPaint);
    HANDLE_MSG(hWnd, WM_COMMAND,            CreditsWnd_OnCommand);
    HANDLE_MSG(hWnd, WM_PALETTECHANGED,     OnPaletteChanged);
    HANDLE_MSG(hWnd, WM_QUERYNEWPALETTE,    OnQueryNewPalette);
    HANDLE_MSG(hWnd, WM_KEYDOWN,            CreditsWnd_OnKey);

    case WM_SYSCOMMAND:
        if (wParam == SC_SCREENSAVE)
        {
            DPF("Balloon ScreenSaver: REJECTED!!!\n");

            return (TRUE);      // reject the screen saver
        }

        break;
  }

  return DefWindowProc(hWnd, wMessage, wParam, lParam);
} 

/******************************************************************/
/* This function is to deal with ALT+F6 */
/*
void CreditsWnd_OnKillFocus(HWND hwnd, HWND hwndNewFocus)
{
  if (hwndNewFocus == GetParent(lpGetSenState()->hMattWnd))
		FORWARD_WM_COMMAND(hwnd, CMD_CLOSE, NULL, 0, PostMessage);
}  	 */

/******************************************************************/
/*
 @doc   EXTERNAL

 @func  BOOL | CreditsWnd_OnCreate
        WM_CREATE message cracker for main stack.

 @param HWND | hWnd |
        Window handle.
        
 @param LPCREATESTRUCT | lpCreateStruct |
        Pointer to window creation structure.
    
 @rdesc TRUE upon succes, FALSE otherwise.
*/

BOOL CreditsWnd_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
  char        rgchMVClass[MAX_CLASS_NAME];
  
  MMBUTTONCS  Button;
    
  // Register palette handler
  vRegisterPalette(hWnd, TRUE);
  
  if ( !LoadString(lpCreateStruct->hInstance, STBL_CLS_MVCHILD, rgchMVClass, MAX_CLASS_NAME) )
    return FALSE;

  // Create child windows
  Button.NumTiles     = 2;
  Button.NextButtonID = 0;

  Button.lpBmp        = lpbmOK;
  Button.wForSnd      = (WORD)(SD_OFFBUTT);
  Button.iTop         = 0;
  Button.wBltMode     = BLT_GADGET;
      
  if ( (CreateWindow( lpszMMButton, NULL,
                      WS_CHILD | WS_VISIBLE,
                      OK_X, OK_Y,
                      Button.lpBmp->bmiHeader.biWidth/2,
                      Button.lpBmp->bmiHeader.biHeight,
                      hWnd,
                      (HMENU)(CMD_CLOSE),
                      lpCreateStruct->hInstance,
                      (LPVOID)&Button)) == NULL )
  {
    DPF("Couldn't create OFF button!\n");
    return FALSE;
  }
    
  Button.lpBmp        = lpbmSeeAlso;
  Button.wForSnd      = SD_SEEALSO;
      
  if ( (CreateWindow( lpszMMButton, NULL,
                      WS_CHILD | WS_VISIBLE,
                      SEEALSO_BUTT_X, SEEALSO_BUTT_Y,
                      Button.lpBmp->bmiHeader.biWidth/2,
                      Button.lpBmp->bmiHeader.biHeight,
                      hWnd,
                      (HMENU)(CMD_SEEALSO),
                      lpCreateStruct->hInstance,
                      (LPVOID)&Button)) == NULL )
  { 
    DPF("Couldn't create info button\n");
    return FALSE;
  }

  Button.wBltMode   = BLT_GADGET;
  Button.wForSnd    = SD_BUTT1 + BUTT_LESS;
  Button.lpBmp      = lpbmLess;
  Button.NumTiles   = 14;
  Button.iTop       = 0;
      
  if ((CreateWindow(lpszMMButton, NULL,
                    WS_CHILD | MMBS_Check,  //starts off hidden, and is a MMBS_Check to prevent reversing
                    MED_BUTT_X, MED_BUTT_Y,
                    Button.lpBmp->bmiHeader.biWidth / Button.NumTiles,
                    Button.lpBmp->bmiHeader.biHeight,
                    hWnd,
                    (HMENU)(CMD_CREDITPREV),
                    lpCreateStruct->hInstance,
                    (LPVOID)&Button)) == NULL )
  { 
    DPF("Couldn't create less button\n");
    return FALSE;
  }

  Button.wBltMode   = BLT_GADGET;
  Button.wForSnd    = SD_BUTT1 + BUTT_MORE;
  Button.lpBmp      = lpbmMore;
  Button.NumTiles   = 13;
      
  if ((CreateWindow(lpszMMButton, NULL,
                    WS_CHILD | WS_VISIBLE | MMBS_Check,
                    MED_BUTT_X, MED_BUTT_Y, // starts off as only button
                    Button.lpBmp->bmiHeader.biWidth / Button.NumTiles,
                    Button.lpBmp->bmiHeader.biHeight,
                    hWnd,
                    (HMENU)(CMD_CREDITMORE),
                    lpCreateStruct->hInstance,
                    (LPVOID)&Button)) == NULL )
  { 
    DPF("Couldn't create more button\n");
    return FALSE;
  }
     
  if ( (CreateWindow( rgchMVClass, rgchMVClass,
                      WS_CHILD  | WS_VISIBLE | WS_VSCROLL,
                      POPUP_X, POPUP_Y,
                      CREDITS_DX, POPUP_DY,
                      hWnd, (HMENU)CHILD_ID,
                      lpCreateStruct->hInstance,
                      NULL)) == NULL )
  { 
    DPF("Couldn't create MVChild window!\n");
    return FALSE;
  }


  DPF3("\n");
  return TRUE;
}


/******************************************************************/
/*
 @doc   EXTERNAL

 @func  void | CreditsWnd_OnDestroy
        WM_DESTROY message cracker for Credits.

 @param HWND | hWnd |
        Window handle.
    
 @rdesc Nothing.
*/
void CreditsWnd_OnDestroy(HWND hWnd)
{  
  // Tell the palette manager we are done       
  vRegisterPalette(hWnd, FALSE);
    
  gfCreditsMagSet = FALSE;

  // Free resources
  vTermCreditResources();
  
  // Free WinG thing
  DPF("Credits terminating WinG bitmap\n");
  vTermWinG(NULL);
}

/******************************************************************************

@func   void | CreditsWnd_OnPaint |

        WM_PAINT message handler.

******************************************************************************/

void CreditsWnd_OnPaint(HWND hWnd)
{
  DPF4("CreditsWnd_OnGadgetPaint: Entering...\n");

  PAINTSTRUCT ps;
  BeginPaint(hWnd, &ps);
  
  HPALETTE  hPal = lpGetSenState()->hPal;
  SETPALETTE(ps.hdc, hPal);

#ifdef NEWTRANSBLT
  TransparentDIBits((BITMAPINFO far *)&gWinGInfoHeader, gpWinGBits, 0, 0,
                      DibPtr((LPBITMAPINFOHEADER)lpbmGadget),
                      DibInfo((LPBITMAPINFOHEADER)lpbmGadget),
                      0, 0, ps.rcPaint.right, ps.rcPaint.bottom,
                      0xFF);
#else
  TransparentDIBits((BITMAPINFO far *)&gWinGInfoHeader, gpWinGBits, 0, 0,
                      DibPtr((LPBITMAPINFOHEADER)lpbmGadget),
                      DibInfo((LPBITMAPINFOHEADER)lpbmGadget),
                      0, 0, ps.rcPaint.right, ps.rcPaint.bottom,
                      TRUE, 0xFF);
#endif
  
  WinGBitBlt(ps.hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, gWinGDC, 0, 0);
  EndPaint(hWnd, &ps);
  
  DPF4("CreditsWnd_OnGadgetPaint: Exiting...\n");
}


/******************************************************************/
/*
 @doc   EXTERNAL

 @func  void | CreditsWnd_OnCommand
        WM_COMMAND message cracker for credits.

 @parm  HWND | hWnd |
        Window handle.
        
 @parm  int | iCtlID |
        Control ID value.
        
 @parm  HWND | hWndCtl |
        Window handle to control.
        
 @parm  UINT | wNotify |
        Notification message. 
    
 @rdesc Nothing.
*/

void CreditsWnd_OnCommand(HWND hWnd, int iCtlID, HWND, UINT)
{
  DPF("Command id is: %d\n", iCtlID);
  
  switch(iCtlID)
  {
    case CMD_CLOSE:
      PostMessage(hWnd, WM_CLOSE, NULL, NULL);
      break;
      
    case CMD_SEEALSO:
      vShowPid(hWnd);
      break;
        
    case CMD_CREDITPREV:
      RECT  rcWindow;
      
      MMB_SetState(GetDlgItem(hWnd, CMD_CREDITPREV), MMB_FirstState);  // reset the window
      
      if (CreditInfo.lFindPos == CreditInfo.lWWLength - 1)
      {
        ShowWindow(GetDlgItem(hWnd, CMD_CREDITMORE), TRUE);
      }

      CreditInfo.lFindPos--;
      vGetCreditTopic(GetDlgItem(hWnd, CHILD_ID));
            
      if (CreditInfo.lFindPos == 0)
      { 
        HWND    hWndTemp = GetDlgItem(hWnd, CMD_CREDITMORE);

        GetClientRect(hWndTemp, &rcWindow);
        ShowWindow(GetDlgItem(hWnd, CMD_CREDITPREV), FALSE);
        MoveWindow(hWndTemp, MED_BUTT_X, MED_BUTT_Y, rcWindow.right, rcWindow.bottom, TRUE); 
      }

      break;

    case CMD_CREDITMORE:

      MMB_SetState(GetDlgItem(hWnd, CMD_CREDITMORE), MMB_FirstState);  // reset the window

      if (CreditInfo.lFindPos == 0)
      {
        RECT    rcWindow;
        HWND    hWndTemp = GetDlgItem(hWnd, CMD_CREDITMORE);

        GetClientRect(hWndTemp, &rcWindow);
        ShowWindow(GetDlgItem(hWnd, CMD_CREDITPREV), TRUE);
        MoveWindow(hWndTemp, MED_BUTT_X, MED_BUTT_Y + MED_BUTT_DY, rcWindow.right, rcWindow.bottom, TRUE); 
      }

      CreditInfo.lFindPos++;
      vGetCreditTopic(GetDlgItem(hWnd, CHILD_ID));

      if (CreditInfo.lFindPos == CreditInfo.lWWLength - 1)
      {
        ShowWindow(GetDlgItem(hWnd, CMD_CREDITMORE), FALSE);
      }
      break;
  }
}
 
/************************************************************************
         MVChildWindow functions
************************************************************************/
/******************************************************************/
/*
 @doc   EXTERNAL

 @func  LRESULT | lCreditsWndProc |
        Main Window procedure for credits.

 @param HWND | hWnd |
        Window handle.
        
 @param UINT | message |
        Window message.
        
 @param WPARAM | wParam |
        Message dependant parameter.
        
 @param LPARAM | lParam |
        Message dependant parameter.
    
 @rdesc Message dependant.
*/

extern "C" LRESULT CALLBACK lMVChildWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
  switch (wMessage)
  {
    HANDLE_MSG(hWnd, WM_CREATE,             MVChild_OnCreate);
    HANDLE_MSG(hWnd, WM_DESTROY,            MVChild_OnDestroy);
    HANDLE_MSG(hWnd, WM_PAINT,              MVChild_OnPaint);              
    HANDLE_MSG(hWnd, WM_VSCROLL,            MVChild_OnVScroll);
    HANDLE_MSG(hWnd, WM_PALETTECHANGED,     OnPaletteChanged);
    HANDLE_MSG(hWnd, WM_QUERYNEWPALETTE,    OnQueryNewPalette);
  }

  return DefWindowProc(hWnd, wMessage, wParam, lParam);
}

/******************************************************************/
/*
 @doc   EXTERNAL

 @func  BOOL | MVChild_OnCreate
        WM_CREATE message cracker for main stack.

 @param HWND | hWnd |
        Window handle.
        
 @param LPCREATESTRUCT | lpCreateStruct |
        Pointer to window creation structure.
    
 @rdesc TRUE upon succes, FALSE otherwise.
*/

BOOL MVChild_OnCreate(HWND hWnd, CREATESTRUCT FAR*)
{
  ERR     err;    
  char    rgchMVFile[_MAX_PATH];
  LPSTR   lpszTemp;
  LPMV    lpMV; 
  HANDLE  hFntTable;

  // copy our book path         
  lstrcpy((LPSTR)rgchMVFile, lpGetSenState()->lpszMMIOPath);
      
  // Nuke the '+'
  lpszTemp = rgchMVFile;
      
  while ( *lpszTemp && *lpszTemp != '+' )
      ++lpszTemp;
          
  *lpszTemp = '\0';
  

  if ( (ghCreditTitle = hMVTitleOpen((LPCSTR)rgchMVFile)) == NULL )
  {
    DPF("Credits: Couldn't open title '%s'.\n", rgchMVFile);
    return FALSE;
  }
  else
  {
    if ( (lpMV = lpMVNew(ghCreditTitle, (LPERR)&err)) == NULL )
      DPF("Credits: Couldn't create MV.\n");
    else
    {
      MVSetTextColor(lpMV, RGB_BLACK);
      MVSetBkColor(lpMV, RGB_WHITE);

      // WIN32: This was casting (HANDLE)(WORD), which would be bad for Win32

      hFntTable = (HANDLE)lMVTitleGetInfo(ghCreditTitle, TTLINF_FONTTABLE, 0L, 0L);
      
      if(hFntTable) 
      {
        hMVSetFontTable(lpMV, hFntTable);                
            
        goto MV_OK;
      }
      

      MVDelete(lpMV);
    }


    MVTitleClose(ghCreditTitle);
    return FALSE;
  }

MV_OK:
  SetWindowLong(hWnd, GWL_LPCREDITMV, (LONG)lpMV);

  if ( !fMVSetWindow(lpMV, hWnd, (LPERR)&err) )
    return FALSE; 
  
  // comment this line if bug 3404 gets reopened:
  // MVHidePartialLines(lpMV, TRUE);

  vShowInitialTopic(hWnd);

  DPF3("MVChild Window Created...");
  return TRUE;
}

/******************************************************************/
/*
 @doc   INTERNAL

 @func  void | vShowInitialTopic
        Opens word wheel and shows topic 0

 @param HWND | hWnd |
        Window handle.
    
 @rdesc Nothing.
*/
  
void vShowInitialTopic(HWND hWnd)
{
  char    rgchText[_MAX_PATH];
  LPSTR   lpszTemp;

  lstrcpy((LPSTR)rgchText, lpGetSenState()->lpszMMIOPath);
  lpszTemp = rgchText;

  while (*lpszTemp && *lpszTemp != '+')
  {
    ++lpszTemp;
  }

  *lpszTemp = '\0';

      
  CreditInfo.hWheel = hMVWordWheelOpenTitle(ghCreditTitle, "Credits");

  if (CreditInfo.hWheel != NULL)
  {
    CreditInfo.lWWLength = lMVWordWheelLength(CreditInfo.hWheel);
    CreditInfo.lFindPos  = 0l;
  }

  vGetCreditTopic(hWnd);
}

/******************************************************************/
/*
 @doc   INTERNAL

 @func  void | vGetCreditTopic |
        Gets and displays the credit topic referenced by CreditInfo.lFindPos
    
 @rdesc Nothing.
*/

void vGetCreditTopic(HWND hWnd)
{
    char    rgchText[128];
    DWORD   dwContextID;

    if (!nMVWordWheelLookup(CreditInfo.hWheel, CreditInfo.lFindPos, (LPBYTE)rgchText, 128))
    {
        dwContextID = dwHexToDW(rgchText);

        DPF("Fetched %s (0x%8lx) from word wheel\n", rgchText, dwContextID);

        vGotoCredits(hWnd, dwContextID);
    }
    else
    {
        DPF("Wordwheellookup failed in vGetCreditTopic\n");
    }
}  

/******************************************************************/
/*
 @doc   EXTERNAL

 @func  void | CreditsWnd_OnDestroy
        WM_DESTROY message cracker for Credits.

 @param HWND | hWnd |
        Window handle.
    
 @rdesc Nothing.
*/
  
void MVChild_OnDestroy(HWND hWnd)
{ 
    DPF4("MVChild OnDestroy\n");
    LPMV lpMV = (LPMV)GetWindowLong(hWnd, GWL_LPCREDITMV);

    DPF("lpMV: %lx\n", (DWORD)lpMV);
   
    if(lpMV )
    {
        MVDelete(lpMV);
    }

    if (CreditInfo.hWheel != NULL)
    {

        MVWordWheelClose(CreditInfo.hWheel);
        CreditInfo.hWheel = NULL;
    }
}

/******************************************************************************

@func   void | MVChild_OnPaint |

        WM_PAINT message handler.

******************************************************************************/
void MVChild_OnPaint(HWND hWnd)
{
    PAINTSTRUCT     ps;
    LPMV   lpMV;
    ERR             err;

    lpMV = (LPMV)GetWindowLong(hWnd, GWL_LPCREDITMV);

    DPF4("MVChild OnPaint\n");
    
    BeginPaint(hWnd, &ps);

    if (lpMV)
    {
      RECT  rectClient;
      
      GetClientRect(hWnd, &rectClient);
      fMVApplyToDC(lpMV, ps.hdc, (LPRECT)&rectClient, (LPERR)&err); 
    }

    EndPaint(hWnd, &ps);
}  


void MVChild_OnVScroll(HWND hWnd, HWND, UINT code, int pos)
{
  int     y;
  ERR     err;
  POINT   pt;
//  RECT    rc;
  int     yTop = 0, yBottom = 0;
  
  LPMV    lpMV = (LPMV)GetWindowLong(hWnd, GWL_LPCREDITMV);    

  if ( !lpMV )
    return;

  switch (code)
  {
    case SB_TOP:
    case SB_BOTTOM:
    case SB_LINEUP:
    case SB_LINEDOWN:
    case SB_PAGEUP:
    case SB_PAGEDOWN:
      
      y = yMVScrollY(lpMV, &pt, &yTop, &yBottom, code, 0, &err);
      SetScrollPos (hWnd, SB_VERT, y, TRUE);
      ScrollWindow(hWnd, pt.x, pt.y, 0, 0);

      // this doesn't need to be done, and causes flash if it is

/*     
      if( (code == SB_LINEUP) || (code == SB_PAGEUP) )
      {
        GetClientRect(hWnd, &rc);
        rc.top = rc.bottom - yBottom;
        ScrollWindow(hWnd, 0, yBottom, &rc, NULL);
      }     
      else if( (code == SB_LINEDOWN) || (code == SB_PAGEDOWN) )
      {
        GetClientRect(hWnd, &rc);
        rc.bottom = rc.top + yTop;
        ScrollWindow(hWnd, 0, -yTop, &rc, NULL);
      }
*/

      break;

    case SB_THUMBPOSITION:
      /*
       * pt comes back empty in this case, but the MediaView has
       * done the proper layout. Redraw the window
       */
      y = yMVScrollY(lpMV, &pt, &yTop, &yBottom, code, pos, (LPERR)&err);
      SetScrollPos(hWnd, SB_VERT, y, TRUE);
      RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE);
      UpdateWindow(hWnd);
      break;

    case SB_THUMBTRACK:
      /* track thumb, but don't update the display until SB_THUMBPOSITION */
      SetScrollPos (hWnd, SB_VERT, pos, TRUE);
      break;
  }
}

/*****************************************************
@func  void | vGotoCredits |
          Sets media view to the credits topic and sets up scrolling.

@parm  HWND | hWnd | Handle to the scrolling window.

@rdesc Nothing.
*/

void vGotoCredits(HWND hWnd, DWORD dwTopicID)
{
    VA              va;
    RECT            rect;
    ERR             err;
    POINT           ptScroll;
    int             nScrollMin, nScrollMax;
    BOOL            fBogus = FALSE;
    BOOL            fScrollChange = FALSE;
    LPMV            lpMV = (LPMV)GetWindowLong(hWnd, GWL_LPCREDITMV);

    DPF("Entered vGotoCredits\n");

    if( !lpMV )
      return;
    
        DPF4("vGotoCredits ...");

        va = vaMVConvertHash(ghCreditTitle, dwTopicID);
  //      if( va.dword == vaNil ) 
  // I don't understand the construct above. va is a DWORD not a struct
        if (va == vaNil)
        {  
            fBogus = TRUE;
            goto NEWMV_UPDATE;
        }

        if( !fMVSetAddress(lpMV, va, 1, 0L, (LPERR)&err) )
        { 
            DPF("Couldn't set media view address\n");
            fBogus = TRUE;
            goto NEWMV_UPDATE;
        }

       
        GetClientRect(hWnd, &rect);
        if( !fMVRealize(lpMV, (LPRECT)&rect, (LPERR)&err) )
        {
            DPF("Couldn't realize media view topic\n");
            fBogus = TRUE;                                                
        }

        // initialize credits' font ONCE

        // unfortunately, there's no one place where we realize
        // our MV DC, and the credits and visor can be opened
        // before the gadget (where we usually set the magnification) -
        // so this is necessarily redundant

        if (!gfCreditsMagSet)
        {
            HDC     hDC = GetDC(NULL);

            if (hDC)
            {
                int iLogPix = GetDeviceCaps(hDC, LOGPIXELSY);

                ReleaseDC(NULL, hDC);

                if (iLogPix != SMALL_FONT_DY)
                {
                    DPF("vSetMagnificationFactor: Adjusting font size... (%i)\n", (SMALL_FONT_DY * 100) / iLogPix);

                    if (!fMVSetMagnifier(lpMV, (SMALL_FONT_DY * 100) / iLogPix, &err))
                    {
                        DPF("vSetMagnificationFactor: Couldn't set magnifier (err = %u)\n", err);
                    }
                }
            }

            gfCreditsMagSet = TRUE;
        }


NEWMV_UPDATE:
    //
    // Set up the vertical scrollbar.
    //
    if ( !fBogus )
    {
        ptScroll = ptMVGetScrollSizes(lpMV);
        if( ptScroll.y == 0 )
        {
            GetScrollRange(hWnd, SB_VERT, &nScrollMin, &nScrollMax);
            if ( (nScrollMax - nScrollMin) > 0 )
                fScrollChange = TRUE;

            SetScrollRange(hWnd, SB_VERT, 0, 0, FALSE);
            SetScrollPos(hWnd, SB_VERT, 0, TRUE);
            
        }
        else
        {
            GetScrollRange(hWnd, SB_VERT, &nScrollMin, &nScrollMax);
            if ( (nScrollMax - nScrollMin) == 0 || GetScrollPos(hWnd, SB_VERT) != 1 )
                fScrollChange = TRUE;

            SetScrollRange(hWnd, SB_VERT, 1, ptScroll.y, FALSE);
            SetScrollPos(hWnd, SB_VERT, 1, TRUE);
        }

        // We need to re-realize the content with the new client rect size if
        // the scrollbar status changes.
        if (fScrollChange)
        {
            MVUnrealize(lpMV);
            GetClientRect(hWnd, &rect);
            fMVRealize(lpMV, (LPRECT)&rect, (LPERR)&err);
        }
    }
    else
    {
        SetScrollRange(hWnd, SB_VERT, 0, 0, FALSE);
        SetScrollPos(hWnd, SB_VERT, 0, FALSE);
        MVUnrealize(lpMV);   
        DPF("MVUnrealize Called!\n");
    }
    
    InvalidateRect(hWnd, NULL, TRUE);

    if (fScrollChange)
        FORWARD_WM_NCPAINT(hWnd, 0, SendMessage);
}    


BOOL FGetFileVersion(char *szFullPath, DWORD *pdwMS, DWORD *pdwLS);
/***************************************************************
@func  void | vShowPID |
       This function display the PID number in a message box. It is assumed the PID is a raw data
       string in the resource file with an ID of 128.
        
       The form of the displayed PID is NNNNN-SSS-XXXXXXX.
@rdesc None.
******************************************************************/

void _loadds WINAPI vShowPid(HWND hWnd)
{    
  // REVIEW: [*****] This should be a resource string, of course.
 
  LPSTR lpszPIDDlg = "IDD_PID";
  DialogBox(SenUtilState.hInst, lpszPIDDlg, hWnd, (DLGPROC)fPIDDlg);

  // all the text-displaying code has been moved into the dialog callback
} 

typedef DWORD (FAR PASCAL *LPFNVIS)(char*, DWORD FAR *);
typedef BOOL (FAR PASCAL *LPFNVI)(char*, DWORD, DWORD, char*);
typedef BOOL (FAR PASCAL *LPFNVQV)(void FAR *, char*, void FAR * FAR *, WORD FAR *);

/*  This function was stolen from \sendak\dev\newsetup\common\win\detwin.c
**
**
**  Purpose:
**    Gets the file version values from the given file and sets the
**    given pdwMS/pdwLS variables.
**  Arguments:
**    szFullPath: a zero terminated character string containing the fully
**      qualified path (including disk drive) to the file.
**    pdwMS: Most significant 32 bits of source file version stamp.
**    pdwLS: Least significant 32 bits of source file version stamp.
**  Returns:
**    TRUE if file and file version resource found and retrieved,
**    FALSE if not.
**************************************************************************/
BOOL FGetFileVersion(char *szFullPath, DWORD *pdwMS, DWORD *pdwLS)
{
  VS_FIXEDFILEINFO * pvsfi;
  DWORD dwHandle;
  UINT  cbLen;
  BOOL  fRet = FALSE;
  DWORD cbBuf = 2*sizeof(WORD) + 28*sizeof(char*) + sizeof(VS_FIXEDFILEINFO);
  BYTE  rgbBuf[2*sizeof(WORD) + 28*sizeof(char*) + sizeof(VS_FIXEDFILEINFO)];

  // REVIEW: We should be using the ver.lib and version.lib import libraries instead
  // of dynamically linking like this.

/* ------------------------------------------------------------------------
  LPFNVIS lpfnVIS;
  LPFNVI  lpfnVI;
  LPFNVQV lpfnVQV;
  HINSTANCE  hinstDLL;

  if ((hinstDLL = LoadLibrary("VER.DLL")) == 0)
      return(FALSE);

  if ((int)hinstDLL < 32)
    return(FALSE);

  lpfnVIS = (LPFNVIS)GetProcAddress(hinstDLL,"GetFileVersionInfoSize");
  lpfnVI  = (LPFNVI)GetProcAddress(hinstDLL,"GetFileVersionInfo");
  lpfnVQV = (LPFNVQV)GetProcAddress(hinstDLL,"VerQueryValue");

  if (lpfnVIS == NULL || lpfnVI == NULL || lpfnVQV == NULL)
    {
    fRet = FALSE;
    }
  if ((*lpfnVIS)(szFullPath, &dwHandle) != NULL
    && (*lpfnVI)(szFullPath, dwHandle, cbBuf, (char*)rgbBuf)
    && (*lpfnVQV)((void FAR *)rgbBuf, (char*)"\\", (void FAR * FAR *)&pvsfi, (WORD FAR *)&cbLen))
  ------------------------------------------------------------------------ */

  // And now we do

  if (GetFileVersionInfoSize(szFullPath, &dwHandle) != NULL
      && GetFileVersionInfo(szFullPath, dwHandle, cbBuf, (char *)rgbBuf)
      && VerQueryValue((void FAR *)rgbBuf, (char *)"\\", (void FAR * FAR *)&pvsfi, (UINT FAR *)&cbLen))
  {
    *pdwMS = pvsfi->dwFileVersionMS;
    *pdwLS = pvsfi->dwFileVersionLS;
    fRet = TRUE;
  }
  else
  {
    fRet = FALSE;
  }

  //FreeLibrary(hinstDLL);

  return(fRet);
}



/******************************************************************/
/*
 @doc   EXTERNAL

 @func  void | CreditsWnd_OnKey |
    Handles key-down messages

 @parm  HWND | hwnd |
    window getting the message

 @parm  UINT | vk |
    keycode

 @rdesc nothing
*/

void CreditsWnd_OnKey(HWND hwnd, UINT vk, BOOL, int, UINT)
{
  if (vk == 'I' &&
    GetKeyState(VK_CONTROL) < 0 &&
    GetKeyState(VK_SHIFT) < 0)
  {
    vShowPid(hwnd);
  }
}


/******************************************************************/
/*
 @doc   EXTERNAL

 @func  BOOL | fPIDDlg |
        Dialog box window procedure for the PID dialog
        
 @parm  HWND | hWnd |
        Handle to this dialog box.
        
 @parm  UINT | msg |
        Message for this dialog box.
        
 @parm  WPARAM | wParam |
        WPARAM, message dependent.
        
 @parm  LPARAM | lParam |
        LPARAM, message dependent.
        
 @resdc Message dependent.
*/

BOOL CALLBACK _loadds fPIDDlg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static DWORD  dwVersionMS;
    static DWORD  dwVersionLS;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
          // Get the version info straight from the dll's rc version stamp

          LPSENSTATE    lpSenState = lpGetSenState();
          char    szPathname[132];                 // longest ini entry we accept is 120 plus 11 for explora.ini and 1 for \0
          RECT    rcSpam;
          RECT    rcClient;

          GetWindowRect(lpSenState->hMattWnd, &rcSpam);
          GetClientRect(hWnd, &rcClient);

          // center the dialog box in the middle of the SPAM window

          MoveWindow(hWnd, 
          (rcSpam.left + rcSpam.right) / 2 - (rcClient.right) / 2,
          (rcSpam.top + rcSpam.bottom) / 2 - (rcClient.bottom) / 2,
          rcClient.right, rcClient.bottom + GetSystemMetrics(SM_CYMENU) + GetSystemMetrics(SM_CYDLGFRAME), FALSE);

          // Localize: The explora.dll name should change for each language (eg. exploraz.dll for UK English)
          // ToDo: This name should be in a string table

          // WIN32: name changed

#ifdef WIN32
          char  rgchDllName[MAX_STRLEN];

          if (LoadString(lpSenState->hInst, STR_SHAREDDLLNAME, rgchDllName, MAX_STRLEN))
          {
            GetSystemDirectory(szPathname, 132);
            lstrcat(szPathname, "\\");
            lstrcat(szPathname, (LPSTR)rgchDllName);
          }
#else          
          lstrcpy(szPathname, lpSenState->lpszInstPath);
          lstrcat(szPathname, "explor16.dll");
#endif

          if (!(FGetFileVersion(szPathname, &dwVersionMS, &dwVersionLS)))
          {
            // if we fail to get the file version from the rc stamp, use the slm rmj values
            // ToDo: if we increment the version number, this value will need to be incr as well

            dwVersionMS = (1 << 16) + rmj;
          }
        }
          
        return TRUE;

          // WIN32: New message is used, with same functionality as Win16

#ifdef WIN32            
        case WM_CTLCOLORDLG:
#else
        case WM_CTLCOLOR:
          if (HIWORD(lParam) == CTLCOLOR_DLG)
#endif
          return (BOOL)GetStockObject(LTGRAY_BRUSH);

        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            LPSENSTATE  lpSenState = lpGetSenState();
            LPSTR       lpszAboutRes;
            LPSTR       lpszPid;
            char        rgchVersion[MAX_STRLEN], rgchTitle[MAX_STRLEN];  
            LPSTR       lpszOutput;
            LPSTR       lpsz;
            RECT        rcClient;
            int         iCount;
            HFONT       hFont;

            BeginPaint(hWnd, &ps);

            // Create the font we will use!
            HLOCAL      hLocal = LocalAlloc(LPTR, sizeof(LOGFONT));
          
            if ( hLocal )
            {
                PLOGFONT    plfont = (PLOGFONT)LocalLock(hLocal);
                if ( plfont )
                {
                    plfont->lfHeight = -12;
                    strcpy(plfont->lfFaceName, "Arial");
                    hFont = CreateFontIndirect(plfont);
                    LocalUnlock(hLocal);
                    LocalFree(hLocal);
                }
            }
            
            HFONT   hOldFont = NULL;
            if ( hFont )
            {
                hOldFont = (HFONT)SelectObject(ps.hdc, hFont);
            }
            
            SetBkMode(ps.hdc, TRANSPARENT);
            SetTextColor(ps.hdc, RGB(0, 0, 0));

            /* Format of resources:

              RC_ABOUT_BOX:           Title\n\n
                                      Copyright\n\n
                                      Product ID: %s\n\n
                                      Text

              RC_PID_NUMBER(Win16)    12345-123-1234567
              
              RC_PID_NUMBER(Win32)    Garbage for 128 bytes
                         RPC   ->     12345
                         Site  ->     123
                         Serial->     1234567
                         Random->     12345

              STBL_VERSION            Version %u.%u
            
            We want the PID to look like 12345-123-1234567-12345 for Win32 (23 characters - PID 2.0 spec)
            We want the PID to look like 12345-123-1234567 for Win16 (17 characters - PID 1.0 spec)
            
            Dialog should look like:
            
                Title
                Version

                Copyright

                Product ID

                Text
            */                          
              
            if(LoadString(lpSenState->hInst, STR_TITLE, rgchTitle, MAX_STRLEN))
            {	 /* this is here because Win95 and NT 3.51 will not load the resource after
						   the first '\n' if you alt+tab away and back. So we have '\t's in the resource
						    and replace them with '\n's after the resource has been loaded. */
						  lpszAboutRes = (LPSTR)LoadGenResource(lpSenState->hInst, MAKEINTRESOURCE(RC_ABOUT_BOX), RT_RCDATA);
							if (lpszAboutRes)
							{
								
	              lpszPid = (LPSTR)LoadGenResource(lpSenState->hInst, MAKEINTRESOURCE(RC_PID_NUMBER), RT_RCDATA);
    
	              if(lpszPid)
	              {    
	                if (LoadString(SenUtilState.hInst, STBL_VERSION, rgchVersion, MAX_STRLEN))
	                {  
	#ifdef WIN32
	                  lpszOutput = (LPSTR)GlobalAllocPtr(GMEM_MOVEABLE, lstrlen(lpszAboutRes) + lstrlen(rgchVersion) + 10 + 23);
	#else // Win16
	                  lpszOutput = (LPSTR)GlobalAllocPtr(GMEM_MOVEABLE, lstrlen(lpszAboutRes) + lstrlen(rgchVersion) + 10 + lstrlen(lpszPid));
	#endif

	                  if(lpszOutput)
	                  {
	                    //for(iCount=0 ; (lpszAboutRes[iCount]) && (lpszAboutRes[iCount] != '\n') ; iCount++);
											iCount = lstrlen(rgchTitle);
	                    // need one little '+ 1' in here and it works perfectly

	                    lstrcpyn(lpszOutput, rgchTitle, iCount + 1);     // Put title into output buffer (terminated with \0)
	                    SetWindowText(hWnd, lpszOutput);                    // use output buffer to set window's caption
                    
	                    lpsz = lpszOutput + iCount;                         // calculate the append point of the output buffer

	                    *lpsz++ = '\n';                             // add a carriage return to output buffer (end of 1st line)

	                    lpsz += wsprintf(lpsz,                      // append to the output buffer
	                                      rgchVersion,                // the version info line
	                                      HIWORD(dwVersionMS),        // Major version #
	                                      LOWORD(dwVersionMS));       // Minor version #

	#ifdef WIN32
	                    #define PID_RPC       128
	                    #define PID_SITE      (PID_RPC + 5)
	                    #define PID_SERIAL    (PID_SITE + 3)
	                    #define PID_RANDOM    (PID_SERIAL + 7)

	                    wsprintf(rgchVersion,                       // In Win32 we must format the PID ourselves
	                              "%5.5s-%3.3s-%7.7s-%5.5s",
	                              lpszPid+PID_RPC,
	                              lpszPid+PID_SITE,
	                              lpszPid+PID_SERIAL,
	                              lpszPid+PID_RANDOM);

	                    UnloadGenResource(lpszPid, FALSE);          // we're done with the resource we loaded now

	                    lpszPid = rgchVersion;                      // fake out following Win16 code to display formated pid
	#endif

	                    wsprintf(lpsz,                              // append the rest of the about box to the output string
	                              lpszAboutRes+iCount,
	                              lpszPid);                         // passing in the PID # string to be inserted in the %s

	#ifndef WIN32
	                    UnloadGenResource(lpszPid, FALSE);          // If we're Win16 we need to unload the PID resource now
	#endif

	                    // paint the output

	                    GetClientRect(hWnd, &rcClient);
	                    rcClient.left += 15;
	                    rcClient.right -= 30;
	                    rcClient.top += 15;
	                    rcClient.bottom -= 30;

	                    DrawText(ps.hdc, lpszOutput, -1, &rcClient, DT_LEFT | DT_WORDBREAK);

	                    GlobalFreePtr(lpszOutput);
	                  } // if alloc lpszOutput
	                } // if loaded version string
	              } // if loaded lpszPid
								UnloadGenResource(lpszAboutRes, FALSE);
							}
            } // if loaded lpszAboutRes

            if ( hFont )
            {
                hOldFont = (HFONT)SelectObject(ps.hdc, hFont);
                DeleteObject(hFont);
            }
            
            EndPaint(hWnd, &ps);
            
            return TRUE;
        }

        case WM_COMMAND:
          switch(LOWORD(wParam))      // works for both Win16 and Win32
          {
            case IDOK:
              EndDialog(hWnd, TRUE);

              break;

            case IDD_PIDINFO:
            {
              char  rgchMSInfo[128];
              LPSTR lpsz;
              
              lstrcpy((LPSTR)rgchMSInfo, lpGetSenState()->lpszMMIOPath);

              lpsz = (LPSTR)rgchMSInfo;
              lpsz += lstrlen(lpsz);

              while (*lpsz != '\\' && lpsz != (LPSTR)rgchMSInfo)
              {
                lpsz--;
              }

              // if we have reached the beginning of the buffer, something is wrong.  screw it

              *lpsz = '\0';

              #ifdef WIN32
              lstrcat((LPSTR)rgchMSInfo, "\\WINSYS\\MSINFO32.EXE");
              #else
              lstrcat((LPSTR)rgchMSInfo, "\\WINSYS\\MSINFO.EXE");
              #endif

              EndDialog(hWnd, TRUE);

              WinExec((LPSTR)rgchMSInfo, SW_SHOWNORMAL);
            }
              
              break;
          }

          return TRUE;
    }

    return FALSE;
}
