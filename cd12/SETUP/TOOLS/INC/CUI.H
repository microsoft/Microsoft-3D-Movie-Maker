/****************************************************************************
**
**	File:			CUI.H
**	Purpose:		Custom User Interface Include File.
**	Notes:
**
****************************************************************************/

#ifndef CUI_H
#define CUI_H

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif


#define  fFalse  ((BOOL)0)
#define  fTrue   ((BOOL)1)

#define  cbSymValMax        (0x2000)
#define  cListItemsMax      (0x07FF)

#define  STF_MESSAGE        (WM_USER + 0x8000)
#define  STF_REINITDIALOG   (STF_MESSAGE + 12)
#define  STF_ACTIVATEAPP    (STF_MESSAGE + 17)


extern void WINAPI FailAssert ( LPSTR szFile,  UINT uiLine );
#ifndef Assert
  #ifdef DEBUG
	#define  Assert(f)  ((f) ? (void)0 : FailAssert(__FILE__,__LINE__))
  #else
    #define  Assert(f)
  #endif
#endif  /* !Assert */


#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif

#endif  /* CUI_H */

