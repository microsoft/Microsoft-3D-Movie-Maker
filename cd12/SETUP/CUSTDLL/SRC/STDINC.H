/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Disable C4001: nonstandard extension 'single line comment' was used */
#pragma warning(disable:4001)
/* Disable C4115: 'IRpcStubBuffer' : named type definition in parentheses */
#pragma warning(disable:4115)
/* Disable C4201: nonstandard extension used : nameless struct/union */
#pragma warning(disable:4201)
/* Disable C4209: nonstandard extension used : benign typedef redefinition */
#pragma warning(disable:4209)
/* Disable C4214: nonstandard extension used : bit field types other than int */
#pragma warning(disable:4214)
/* Disable C4514: 'Int64Sh__Mod32' : unreferenced inline function has been removed */
#pragma warning(disable:4514)

#include <windows.h>

/* Warning 4514 does not kick in until the end of compilation unit so must
   always be disabled.  May be fixed in V3.
   #pragma warning(default:4514) */
#pragma warning(default:4214)
#pragma warning(default:4209)
#pragma warning(default:4201)
#pragma warning(default:4115)
#pragma warning(default:4001)
