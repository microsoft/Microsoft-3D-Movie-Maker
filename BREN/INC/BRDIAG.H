/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brdiag.h 1.1 1995/07/28 18:57:13 sam Exp $
 * $Locker:  $
 *
 * Brender's interface to diagnostic handling
 */

#ifndef _BRDIAG_H_
#define _BRDIAG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Instance of an error handler
 */
typedef void BR_CALLBACK br_diag_warning_cbfn(char * message);
typedef void BR_CALLBACK br_diag_failure_cbfn(char * message);

typedef struct br_diaghandler {
    char *identifier;
    br_diag_warning_cbfn *warning;
    br_diag_failure_cbfn *failure;
} br_diaghandler;

/*
 * For backwards compatibility
 */
typedef struct br_errorhandler {
    char *identifier;
    br_diag_warning_cbfn *message;
    br_diag_failure_cbfn *error;

} br_errorhandler;

/**
 ** Macros for diagnostic generation
 **/

/*
 * Report message and exit - should not return to application
 */
#define BR_FAILURE(s) BrFailure(s)
#define BR_FAILURE0(s) BrFailure(s)
#define BR_FAILURE1(s,a) BrFailure(s,a)
#define BR_FAILURE2(s,a,b) BrFail(s,a,b)
#define BR_FAILURE3(s,a,b,c) BrFailure(s,a,b,c)
#define BR_FAILURE4(s,a,b,c,d) BrFailure(s,a,b,c,d)
#define BR_FAILURE5(s,a,b,c,d,e) BrFailure(s,a,b,c,d,e)
#define BR_FAILURE6(s,a,b,c,d,e,f) BrFailure(s,a,b,c,d,e,f)

/*
 * Report message and continue
 */
#define BR_WARNING(s) BrWarning(s)
#define BR_WARNING0(s) BrWarning(s)
#define BR_WARNING1(s,a) BrWarning(s,a)
#define BR_WARNING2(s,a,b) BrWarning(s,a,b)
#define BR_WARNING3(s,a,b,c) BrWarning(s,a,b,c)
#define BR_WARNING4(s,a,b,c,d) BrWarning(s,a,b,c,d)
#define BR_WARNING5(s,a,b,c,d,e) BrWarning(s,a,b,c,d,e)
#define BR_WARNING6(s,a,b,c,d,e,f) BrWarning(s,a,b,c,d,e,f)

/*
 * Report message and exit, including source file and line number
 */
#define BR_FATAL(s) BrFatal(__FILE__,__LINE__,s)
#define BR_FATAL0(s) BrFatal(__FILE__,__LINE__,s)
#define BR_FATAL1(s,a) BrFatal(__FILE__,__LINE__,s,a)
#define BR_FATAL2(s,a,b) BrFatal(__FILE__,__LINE__,s,a,b)
#define BR_FATAL3(s,a,b,c) BrFatal(__FILE__,__LINE__,s,a,b,c)
#define BR_FATAL4(s,a,b,c,d) BrFatal(__FILE__,__LINE__,s,a,b,c,d)
#define BR_FATAL5(s,a,b,c,d,e) BrFatal(__FILE__,__LINE__,s,a,b,c,d,e)
#define BR_FATAL6(s,a,b,c,d,e,f) BrFatal(__FILE__,__LINE__,s,a,b,c,d,e,f)

/*
 * Backwards compatibility
 */
#define BR_ERROR(s) BrFailure(s)
#define BR_ERROR0(s) BrFailure(s)
#define BR_ERROR1(s,a) BrFailure(s,a)
#define BR_ERROR2(s,a,b) BrFailure(s,a,b)
#define BR_ERROR3(s,a,b,c) BrFailure(s,a,b,c)
#define BR_ERROR4(s,a,b,c,d) BrFailure(s,a,b,c,d)
#define BR_ERROR5(s,a,b,c,d,e) BrFailure(s,a,b,c,d,e)
#define BR_ERROR6(s,a,b,c,d,e,f) BrFailure(s,a,b,c,d,e,f)

#ifdef __cplusplus
};
#endif
#endif
