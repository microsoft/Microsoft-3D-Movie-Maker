/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Main include file for util files.

***************************************************************************/
#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>

// define the endian-ness
#ifdef IN_80386
#define LITTLE_ENDIAN
#endif //IN_80386

#ifdef LITTLE_ENDIAN
#define BigLittle(a, b) b
#define Big(a)
#define Little(a) a
#else //!LITTLE_ENDIAN
#define BigLittle(a, b) a
#define Big(a) a
#define Little(a)
#endif //!LITTLE_ENDIAN

#ifdef MAC

#include "mac.h"

#elif defined(WIN)

#define CONST //so we don't have to use const everywhere that Windows wants it

#ifdef UNICODE
#define _UNICODE
#endif //UNICODE

//windef.h typedef's PSZ to char *, this fools it into using PSZS instead
#define PSZ PSZS
#include <windows.h>
#include <windowsx.h>
#include <vfw.h>
#undef PSZ

#define MIR(foo) MAKEINTRESOURCE(foo)
typedef HBITMAP HBMP;
typedef HENHMETAFILE HPIC;
typedef HPALETTE HPAL;
typedef HCURSOR HCRS;
#define hBadWin INVALID_HANDLE_VALUE	//some windows APIs return this

#endif //WIN

#define size(foo) ((long)sizeof(foo))
#define offset(FOO,field) ((int)&((FOO *)0)->field)
#define CvFromRgv(rgv) (size(rgv) / size(rgv[0]))
#define BLOCK

#ifdef DEBUG
#define priv
#else //!DEBUG
#define priv static
#endif //!DEBUG


//standard scalar types
typedef unsigned char byte;
const byte kbMax = 0xFF;
const byte kbMin = 0;

typedef unsigned short ushort;
const short kswMax = (short)0x7FFF;
const short kswMin = -kswMax;	// so -kswMin is positive
const ushort ksuMax = 0xFFFF;
const ushort ksuMin = 0;

typedef unsigned int uint;

typedef unsigned long ulong;
const long klwMax = 0x7FFFFFFF;
const long klwMin = -klwMax;	// so -klwMin is positive
const ulong kluMax = 0xFFFFFFFF;
const ulong kluMin = 0;

typedef int bool;

// standard character types:
// schar - short (skinny) character (1 byte)
// wchar - wide character (unicode)
// achar - application character
#ifdef MAC
typedef byte schar;
const schar kschMax = (schar)0xFF;
const schar kschMin = (schar)0;
#else //!MAC
typedef char schar;
const schar kschMax = (schar)0x7F;
const schar kschMin = (schar)0x80;
#endif //!MAC
typedef ushort wchar;
const wchar kwchMax = ksuMax;
const wchar kwchMin = ksuMin;
#ifdef UNICODE
typedef wchar achar;
typedef unsigned short uchar;
const achar kchMax = kwchMax;
const achar kchMin = kwchMin;
#define PszLit(sz) L##sz
#define ChLit(ch) L##ch
#else //!UNICODE
typedef schar achar;
typedef unsigned char uchar;
const achar kchMax = kschMax;
const achar kchMin = kschMin;
#define PszLit(sz) sz
#define ChLit(ch) ch
#endif //!UNICODE


typedef class GRPB *PGRPB;
typedef class GLB *PGLB;
typedef class GL *PGL;
typedef class AL *PAL;
typedef class GGB *PGGB;
typedef class GG *PGG;
typedef class AG *PAG;
typedef class GSTB *PGSTB;
typedef class GST *PGST;
typedef class AST *PAST;
typedef class SCPT *PSCPT;
typedef class BLCK *PBLCK;

#include "framedef.h"
#include "debug.h"
#include "base.h"
#include "utilint.h"
#include "utilcopy.h"
#include "utilstr.h"
#include "utilmem.h"
#include "fni.h"
#include "file.h"
#include "groups.h"
#include "utilerro.h"
#include "chunk.h"
#include "utilrnd.h"
#include "crf.h"
#include "codec.h"

//optional
#include "stream.h"
#include "lex.h"
#include "scrcom.h"
#include "screxe.h"
#include "chse.h"
#include "midi.h"

#include "utilglob.h"

#endif //!UTIL_H

