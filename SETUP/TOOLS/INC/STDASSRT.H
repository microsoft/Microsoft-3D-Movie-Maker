/****************************************************************************
**
**	File:			STDASSRT.H
**	Purpose:		Standard Assert macros and common error handling defines.
**	Notes:
**
****************************************************************************/

#ifndef STDASSRT_H
#define STDASSRT_H

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif

/* REVIEW: need to make this DEBUG only when RTChkArg is removed */
#define EnableAssertCode  static char THIS_FILE[] = __FILE__;


/*  Common Error Validation and Handling */

typedef enum {
	vrcFail  = 0,  /* if IDOK or IDIGNORE or IDNO or no message box */
	vrcValid = 1,  /* if no error condition */
	vrcRetry = 2,  /* if IDRETRY or IDYES */
	vrcQuit  = 3,  /* if IDABORT or IDCANCEL */
	vrcYes   = vrcRetry,
	vrcNo    = vrcFail,
	vrcIgnore= vrcFail,
	vrcCancel= vrcQuit,
	vrcAbort = vrcQuit,
	vrcOk    = vrcFail
} VRC;

#define ERROR_PARAM_MAX 4   /* parameters %1, %2, %3, %4 */

/* sets an error parameter from a temp string value, good until Validate() */
VOID PUBLIC SetErrorParam ( UINT iParam, SZC szValue );

/* sets an error parameter from a stable string value, can Validate() later */
VOID PUBLIC SetErrorParamConst ( UINT iParam, CSZC szValue );

/* sets an error message parameter from a string value, make a string copy */
VOID PUBLIC SetErrorParamCopy  ( UINT iParam, SZC  szValue );

/* sets an error message parameter from an integer value */
VOID PUBLIC SetErrorParamInt   ( UINT iParam, INT   iValue );

/* sets an error message parameter from an character value */
VOID PUBLIC SetErrorParamChar  ( UINT iParam, CHAR chValue );

/* tests a condition, handles error and clears error parameters if false */
VRC  PUBLIC Validate ( BOOL fCondition, UINT iStringId );

/* loads a string resource or debug string, calls ProcessMessage() */
VRC  PUBLIC ProcessMessageId ( UINT iStringId );

/* formats message template, optionally displays message box, write to log */
VRC  PUBLIC ProcessMessage ( SZC szMsg );

/* loads a debug or resource message and writes to the logfile if open */
VRC  PUBLIC LogMessageId ( UINT iResId );  /* return status of log write */

/* loads a message, sets status for %V & %v, writes to the logfile if open */
VRC  PUBLIC LogValidate ( BOOL fCondition, UINT iResId );/* return log status */

/* prototype for callback to obtain object or pass-specific information */
/* returns the number of characters copied to the buffer, or 0 if no info */
typedef INT (WINAPI *PFNErrorParam)( INT chParam, SZ rgchBuf );

/* entry for ACME only, to setup pass calback for error parameters */
VOID PUBLIC SetPassInfo(SZC szPassName, PFNErrorParam pfnCallback);

/* internal entry to process debug messages, called using DebugMessage(sz) */
VRC  PUBLIC ProcessDebugMessage ( SZC szMsg );

#ifdef DEBUG
# define DebugErrorParam(p,v)	   SetErrorParam(p,v)
# define DebugErrorParamCopy(p,v)  SetErrorParamCopy(p,v)
# define DebugErrorParamConst(p,v) SetErrorParamConst(p,v)
# define DebugErrorParamInt(p,v)   SetErrorParamInt(p,v)
# define DebugErrorParamChar(p,v)  SetErrorParamChar(p,v)
# define DebugMessageId(i)         ProcessMessageId(i)
# define DebugMessage(s)           ProcessDebugMessage(s)
# define DebugLogMessageId(i)      LogMessageId(i)
#else
# define DebugErrorParam(p,v)
# define DebugErrorParamCopy(p,v)
# define DebugErrorParamConst(p,v)
# define DebugErrorParamInt(p,v)
# define DebugErrorParamChar(p,v)
# define DebugMessageId(i)
# define DebugMessage(s)
# define DebugLogMessageId(i)
#endif

#ifdef DEBUG
BOOL PUBLIC ResponseFile ( SZ szFile );
#endif

/* Must call as the first call (with hinstAcme) AND as the last
*	call (with hinstNull).
*/
BOOL PUBLIC SetAcmeInst(HINSTANCE hInst, LPSTR szCmdLine);

/*  RunTime Argument Checking  */
/*  REVIEW: this should be removed, all uses replaced by calls to Validate */
#define RTChkArg(f, retVal) \
	{ if (!(f)) { FailRTChkArg(THIS_FILE, __LINE__); return (retVal); } }

VRC  PUBLIC FailRTChkArg ( SZC szFile, UINT uiLine );

/*	Assert macros */

VOID PUBLIC FailAssert (SZC szFile, UINT uiLine );  /* also PreCond, BadParam */

#ifdef DEBUG

#define FailAssertObj(f, l, i) FailAssert(f, (UINT)(l + ((i)<<16)))
#define FailChkArg(f, l, i)    FailAssert(f, (UINT)(l + (1<<31) + ((i)<<16)))
#define FailPreCond(f, l)      FailAssert(f, (UINT)(l + (1<<31)))

#define Assert(f)      ((f) ? (VOID)0 : (VOID)FailAssert(THIS_FILE, __LINE__))
#define EvalAssert(f)  ((f) ? (VOID)0 : (VOID)FailAssert(THIS_FILE, __LINE__))

#define AssertRet(f, retVal) \
		{if (!(f)) {FailAssert( THIS_FILE, __LINE__); return (retVal);} }
#define EvalAssertRet(f, retVal) \
		{if (!(f)) {FailAssert( THIS_FILE, __LINE__); return (retVal);} }

#define PreCondition(f, retVal)	\
		{if (!(f)) {FailPreCond(THIS_FILE, __LINE__); return (retVal);} }
#define ChkArg(f, iArg, retVal)	\
		{if (!(f)) {FailChkArg(THIS_FILE, __LINE__, iArg); return (retVal);} }

#else

#define Assert(f)
#define EvalAssert(f)				((VOID)(f))
#define AssertRet(    f, retVal)
#define EvalAssertRet(f, retVal)	((VOID)(f))
#define PreCondition( f, retVal)	RTChkArg(f, retVal)
#define ChkArg( f, iArg, retVal)

#endif


/* REVIEW: These are obsolete, remove when files using them are updated */

#ifdef DEBUG
#define DisplayAssertMsg()    Assert(fFalse);
#define DebugLine(expr)       expr;
#define DisplayErrorMsg(sz)   ((VOID)FDisplaySystemMsg(sz))
#else
#define DisplayAssertMsg() /* in objodbc.cpp, copylist.c */
#define DebugLine(expr)    /* once in copylist.c */
#define DisplayErrorMsg(sz)
#endif
extern BOOL WINAPI FDisplaySystemMsg     ( SZ szErr );

#ifdef DEVELOPER
#define BadArgErr(nArg, szApi, szArgs) SetupApiErr(nArg+saeMax, szApi, szArgs)
#else
#define BadArgErr(nArg, szApi, szArgs)
#endif

/* REVIEW: end of obsolete */

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif

#endif  /* STDASSRT_H */
