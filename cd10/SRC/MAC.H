/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Mac standard header file - equivalent of windows.h.

***************************************************************************/


#ifndef SYMC
/* Wings includes Mac headers explicitly (this isn't all of them): */
//NOTE: avoid including Traps.h because it defines a bunch of stuff
//that we use as methods!
#define __TRAPS__

#include <types.h>
#include <errors.h>
#include <osutils.h>	//types.h
#include <memory.h>		//types.h
#include <diskinit.h>	//types.h
#include <fonts.h>		//types.h
#include <quickdra.h>	//types.h, qdtext.h
#include <qdoffscr.h>	//quickdra.h
#include <textedit.h>	//quickdra.h
#include <controls.h>	//quickdra.h
#include <menus.h>		//quickdra.h
#include <events.h>		//types.h quickdra.h osutils.h
#include <desk.h>		//types.h quickdra.h events.h
#include <windows.h>	//quickdra.h events.h controls.h
#include <palettes.h>	//quickdra.h windows.h
#include <dialogs.h>	//windows.h textedit.h
#include <files.h>		//types.h osutils.h segload.h
#include <resource.h>	//types.h files.h
#include <resource.h>	//types.h files.h
#include <folders.h>	//types.h files.h
#include <finder.h>		//
#include <standard.h>	//types.h dialogs.h files.h
#include <script.h>		//types.h quickdra.h intlreso.h
#include <textutil.h>	//types.h script.h osutils.h
#include <lowmem.h>
#else //!SYMC
#include <script.h>
#include <qdoffscreen.h>
#include <palettes.h>
#include <finder.h>
#define __cdecl
#define __pascal pascal
#define GetDialogItem GetDItem
#define GetDialogItemText GetIText
#define SetDialogItemText SetIText
#define UppercaseText(prgch,cch,scr) UpperText(prgch,cch)
#define LowercaseText(prgch,cch,scr) LowerText(prgch,cch)
#include <SysEqu.h>
inline long LMGetHeapEnd(void)
	{ return *(long *)ApplLimit; }
inline long LMGetCurrentA5(void)
	{ return *(long *)CurrentA5; }
#endif //!SYMC

typedef GrafPort PRT;
typedef GrafPort *PPRT;
typedef CGrafPort *PCPRT;
typedef GDHandle HGD;
typedef WindowRecord SWND;
typedef SWND *HWND;
typedef RgnHandle HRGN;
typedef GWorldPtr PGWR;
typedef PixMapHandle HPIX;
typedef BitMap *PBMP;
typedef PicHandle HPIC;
typedef PaletteHandle HPAL;
typedef CTabHandle HCLT;

