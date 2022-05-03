/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

////////////////////////////////////////////////////////////////////////////////
//	File:			SAMPHOOK.H
//	Purpose:		Header file for sample hook routines for replacing the
//					standard acme ui
////////////////////////////////////////////////////////////////////////////////


#include <windows.h>
#include <acmhook.h>
#include <setupapi.h>
#include <stdtypes.h>
#include <setupkit.h>

#include <ctl3d.h>

#include "resource.h"
#include "ownrdraw.h"


#if !defined(IDC_STATIC)
#define IDC_STATIC -1
#endif //!IDC_STATIC


extern "C" HINSTANCE hinst;


extern HWND HwndGetDlgOwner(VOID);
extern BOOL CALLBACK GenericHookDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern VOID FetchIdcPath(HWND hdlg, int idc, LPSTR sz, LONG cb);
extern VOID SetIdcPath(HWND hdlg, int idc, LPSTR sz);
extern BOOL FLoadIds(LONG ids, CHAR *prgchIdsText, LONG cbIdsText);
extern LONG CchChopText(LPSTR sz, INT dx, HDC hdc, LONG cch);
extern BOOL FMsgBox(HWND hwnd, LONG idsText, UINT uType, int *pidRet);
