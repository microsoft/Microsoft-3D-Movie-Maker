/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Contains the declarations of all globals used by util. The order of the
	globals within this file is critical. C++ specifies that constructors
	get called in source order within a module. It leaves construction
	order unspecified between modules. The mutexes at the top of this
	file should be constructed before anything else in the app. One way
	to guarantee this is for all globals to be in a single module. The
	app's global .cpp file can include utilglob.cpp (before any of its
	global declarations).

	For the MSVC tools, constructors seem to be executed in link order.

***************************************************************************/
#include "util.h"
ASSERTNAME

RTCLASS(USAC)

#ifdef DEBUG
// protects our debug linked list object management
MUTX vmutxBase;
#endif //DEBUG

MUTX vmutxMem;

// Shuffler and random number generator for the script interpreter
SFL vsflUtil;
RND vrndUtil;


// Standard Kauai codec
KCDC vkcdcUtil;


// Standard compression manager - gets initialized with the standard
// Kauai codec. Clients can add additional codecs or redirect vpcodmUtil
// to a different compression manager with their own codecs
CODM vcodmUtil(&vkcdcUtil, kcfmtKauai2);
PCODM vpcodmUtil = &vcodmUtil;


// Standard scalable application clok.
USAC _usac;
PUSAC vpusac = &_usac;

#ifdef DEBUG

// Debug memory globals
DMGLOB vdmglob;

#endif //DEBUG

