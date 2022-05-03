/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Main include file for frame files.

***************************************************************************/
#ifndef FRAME_H
#define FRAME_H

#include "frameres.h" //frame resource id's
#include "util.h"
#include "keys.h"


class GPT;	// graphics port
class GNV;	// graphics environment
class CMH;	// command handler
class GOB;	// graphic object
class MUB;	// menu bar
class DOCB;	// base document
class DMD;	// document mdi window
class DMW;	// main document window
class DSG;	// document scroll gob
class DDG;	// document display gob
class SNDM;	// sound manager

typedef class GPT *PGPT;
typedef class GNV *PGNV;
typedef class CMH *PCMH;
typedef class GOB *PGOB;
typedef class MUB *PMUB;
typedef class DOCB *PDOCB;
typedef class DMD *PDMD;
typedef class DMW *PDMW;
typedef class DSG *PDSG;
typedef class DDG *PDDG;
typedef class SNDM *PSNDM;

#include "region.h"
#include "pic.h"
#include "mbmp.h"
#include "gfx.h"
#include "cmd.h"
#include "cursor.h"
#include "appb.h"
#include "menu.h"
#include "gob.h"
#include "ctl.h"
#include "sndm.h"
#include "video.h"

//these are optional
#include "dlg.h"
#include "clip.h"
#include "docb.h"
#include "text.h"
#include "clok.h"
#include "textdoc.h"
#include "rtxt.h"
#include "chcm.h"
#include "spell.h"
#include "sndam.h"
#include "mididev.h"
#include "mididev2.h"

#endif //!FRAME_H

