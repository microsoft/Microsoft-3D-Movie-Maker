/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

//
//  soc.h
//
//  Author: Sean Selitrennikoff
//
//  Date: August, 1994
//

#ifndef SOC_H
#define SOC_H

#include "frame.h"
#include "socdef.h"
#include "socutil.h"
#include "tagman.h"
#include "tagl.h"
#include "bren.h"
#include "tmap.h"
#include "modl.h"
#include "mtrl.h"
#include "body.h"
#include "tmpl.h"
#include "tdf.h"
#include "tdt.h"
#include "msnd.h"
#include "srec.h"
#include "actor.h"
#include "scene.h"
#include "movie.h"
#include "bkgd.h"
#include "tbox.h"

#define kctgActn 'ACTN'
#define kctgActr 'ACTR'
#define kctgBds  'BDS '
#define kctgBkgd 'BKGD'
#define kctgBmdl 'BMDL'
#define kctgBpmp 'BPMP'
#define kctgCam  'CAM '
#define kctgGgae 'GGAE'
#define kctgGldc 'GLDC' // REVIEW *****: obsolete
#define kctgGgcm 'GGCM'
#define kctgGllt 'GLLT'
#define kctgGlms 'GLMS' // motion-match sounds (under ACTN)
#define kctgGgcl 'GGCL'
#define kctgGlxf 'GLXF'
#define kctgMsnd 'MSND'
#define kctgMtrl 'MTRL'
#define kctgCmtl 'CMTL'
#define kctgMvie 'MVIE'
#define kctgPath 'PATH'
#define kctgPict 'PICT'
#define kctgScen 'SCEN'
#define kctgSnd  'SND '
#define kctgSoc  'SOC '
#define kctgTbox 'TBOX'
#define kctgTdf  'TDF '
#define kctgTdt  'TDT '
#define kctgTmpl 'TMPL'
#define kctgGlpi 'GLPI'
#define kctgGlbs 'GLBS'
#define kctgInfo 'INFO'
#define kctgFrmGg 	'GGFR'
#define kctgStartGg	'GGST'
#define kctgThumbMbmp 'THUM'
#define kctgGltm 'GLTM'
#define kctgGlbk 'GLBK'
#define kctgGlcg 'GLCG'
#define kctgBkth 'BKTH'		// Background thumbnail
#define kctgCath 'CATH'		// Camera thumbnail
#define kctgTmth 'TMTH'		// Template thumbnail (non-prop)
#define kctgPrth 'PRTH'		// Prop thumbnail
#define kctgAnth 'ANTH'		// Action thumbnail
#define kctgSvth 'SVTH'		// Sounds (voice) thumbnail
#define kctgSfth 'SFTH'		// Sounds (FX) thumbnail
#define kctgSmth 'SMTH'		// Sounds (midi) thumbnail
#define kctgMtth 'MTTH'		// Materials thumbnail
#define kctgCmth 'CMTH'		// Custom materials thumbnail
#define kctgTsth 'TSTH'		// 3d shape thumbnail
#define kctgTfth 'TFTH'		// 3d font thumbnail
#define kctgTcth 'TCTH'		// Text color thumbnail
#define kctgTbth 'TBTH'     // Text background thumbnail
#define kctgTzth 'TZTH'		// Text size thumbnail
#define kctgTyth 'TYTH'     // Text style thumbnail

#define khidMscb			khidLimKidFrame
#define khidMvieClock		khidLimKidFrame + 1
#define khidRcd 			khidLimKidFrame + 2
#define khidMsq				khidLimKidFrame + 3
#define khidMsqClock		khidLimKidFrame + 4
//khidStudio 				khidLimKidFrame + 5  Defined in stdiodef.h

#define kftgChunky MacWin('chnk', 'CHK')
#define kftgContent MacWin('3con', '3CN')
#define kftgThumbDesc MacWin('3thd', '3TH')
#define kftg3mm MacWin('3mm', '3MM')
#define kftgSocTemp MacWin('3tmp', '3TP')

#define ksz3mm PszLit("3mm")

// Global variables
extern PTAGM vptagm;

#endif !SOC_H

