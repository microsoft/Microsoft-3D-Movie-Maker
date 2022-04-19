/*****************************************************************************
*
*  STDIOCRS.H
*
*  Copyright (C) Microsoft Corporation 1995.
*  All Rights reserved.
*
******************************************************************************
*
*  Module Intent
*
*  Contains all of the cnos for cursors used only in the studio.  Note that
*	cursors shared between the Studio and the Building are defined in
*	inc\buildgob.h and the chunks for them are in src\shared\cursors.cht.
*
******************************************************************************
*
*  Revision History:  Taken from inc\stdiodef.h on 06/20/95 by BethF.
*
*****************************************************************************/

#ifndef STDIOCRS_H
#define STDIOCRS_H

// Note, shared cursors have ids reserved from 0 through 100.
#define kcrsCompose				101
#define kcrsSquashStretch		102
#define kcrsResize				103
#define kcrsRotateX				104
#define kcrsRotateY				105
#define kcrsRotateZ				106
#define kcrsCostume				107
#define kcrsRecord				108
#define kcrsTweak				109
#define kcrsNormalizeRot		110
#define kcrsDefault				111
#define kcrsTboxMove			112
#define kcrsTboxUpDown			113
#define kcrsTboxLeftRight		114
#define kcrsTboxFalling			115
#define kcrsTboxRising			116
#define kcrsNuke				117
#define kcrsTboxCreate			118
#define kcrsIBeam				119
#define kcrsCutObject			120
#define kcrsCopyObject			121
#define kcrsCopyRte				122
#define kcrsPasteObject			123
#define kcrsSceneChop			124
#define kcrsCutText				125
#define kcrsCopyText			126
#define kcrsPasteText			127
#define kcrsPasteRte			128
#define kcrsPaintText			129
#define kcrsFillBkgd			130
#define kcrsMatcher				131	
#define kcrsLooper				132	
#define kcrsSounder				133	
#define kcrsListener			134
#define kcrsSoonerLater			135
#define kcrsComposeAll			136
#define kcrsNormalizeSize		137
#define kcrsTboxStory			138
#define kcrsTboxCredit			139
#define kcrsTboxFont			140
#define kcrsTboxFontSize		141
#define kcrsTboxFontStyle		142
#define kcrsSceneChopBack		143

#endif // !STDIOCRS_H
