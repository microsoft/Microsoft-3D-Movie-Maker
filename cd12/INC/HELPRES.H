/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

//
// Ids for each mbmp per gob within the studio
//
// REVIEW:  these are currently the same ID's as the tools.  Conflicts?
//
// The tools are different visuals from the ones actually used in the studio
// for the following reason(s)
//
//  1.  the tools may need to be treated different visually - ie. grayed out
//              like in Creative Writer.  A wash may not cut it this time
//
//      2.      We can always name the mbmp's in the studio to these names if necessary
//
//  CAVEAT:  This list must keep up to date with the real studio graphics (for now)
//               and kid/mbmp IDs!
//

#define mbmpStudio                      0x20000
#define mbmpPalette                     0x20001
#define mbmpBackground          0x20002
#define mbmpSettingsCover       0x20005
#define mbmpActorsCover         0x20006
#define mbmpSoundsCover         0x20007
#define mbmpTextsCover          0x20008
#define mbmpScrollbarsCover     0x2000B
#define mbmpPlaysCover          0x2000C
#define mbmpUndosCover          0x2000D
#define mbmpBooksCover          0x2000E
#define mbmpDoors                       0x2000F
#define mbmpSettingsBackground  0x20010
#define mbmpActorsBackground    0x20011
#define mbmpSoundsBackground    0x20012
#define mbmpTextsBackground     0x20013
#define mbmpSettingsCameras     0x20014
#define mbmpActorsSelect                0x20015
#define mbmpActorsBiography     0x20016
#define mbmpActorsCompose       0x20017
#define mbmpActorsCostume       0x20018
#define mbmpActorsNormalizeRot  0x20019
#define mbmpBrwsAction          0x2001A
#define mbmpActorsRecordMode    0x2001B
#define mbmpActorsRotateX       0x2001C
#define mbmpActorsRotateY       0x2001D
#define mbmpActorsRotateZ       0x2001E
#define mbmpActorsSooner                0x2001F
#define mbmpActorsSquash                0x20020
#define mbmpActorsShrink                0x20021
#define mbmpActorsXY                    0x20022
#define mbmpActorsXZ                    0x20023
#define mbmpActorsGround                0x20024
#define mbmpSoundsMixer         0x20025
#define mbmpSoundsPlayOnce      0x20026
#define mbmpSoundsListen                0x20027
#define mbmpSoundsRecord                0x20028
#define mbmpSoundsLooping       0x20029
#define mbmpSoundsAttachToCell  0x2002A
#define mbmpSoundsClear         0x2002B
#define mbmpTextsBkgdColor      0x2002C
#define mbmpTextsFontShape      0x2002D
#define mbmpTextsFontColor      0x2002E
#define mbmpTextsFont           0x2002F
#define mbmpTextsSelect         0x20030
#define mbmpTextsScroll         0x20031
#define mbmpTextsFontSize       0x20032
#define mbmpTextsStory          0x20033
#define mbmpBook                                0x20034
#define mbmpMap                         0x20035
#define mbmpCutCopyPaste                0x20036
#define mbmpUndo                                0x20037
#define mbmpVolumeSlider                0x20038
#define mbmpPlay                                0x20039
#define mbmpSceneSorter         0x2003A
#define mbmpPortfolio           0x2003C
#define mbmpExit                                0x2003D
#define mbmpBrwsBackground      0x2003E
#define mbmpBrwsActor           0x2003F
#define mbmpBrwsProp                    0x20040
#define mbmpBrwsThreeD          0x20041
#define mbmpSettingsBrowser     0x20042
#define mbmpBrwsBkSound         0x20043
#define mbmpBrwsFXSound         0x20044
#define mbmpBrwsMusSound                0x20045
#define mbmpTextsCreate         0x20046
#define mbmpPortsBackground     0x20047
#define mbmpPortsPortfolio      0x20048
#define mbmpPortsNew                    0x20049
#define mbmpPortsSave           0x2004A
#define mbmpPausesMouse         0x2004C
#define mbmpPausesClear         0x2004D
#define mbmpPausesSound         0x2004E
#define mbmpCCPBackground       0x2004F
#define mbmpCCPSceneChopFwd     0x20050
#define mbmpCCPSceneNuke                0x20051
#define mbmpCCPActorNuke                0x20052
#define mbmpCCPCopyRoute                0x20053
#define mbmpCCPPaste                    0x20054
#define mbmpCCPCopy                     0x20055
#define mbmpCCPCut                      0x20056
#define mbmpGenericCancel       0x20058
#define mbmpGenericOk           0x20059
#define mbmpGenericPageFwd      0x2005A
#define mbmpGenericPageBack     0x2005B
#define mbmpBrowserCancel       0x2005C
#define mbmpBrowserOk           0x2005D
#define mbmpBrowserPageFwd      0x2005E
#define mbmpBrowserPageBack     0x2005F
#define mbmpBrowserFrame                0x20060 //Next 0x100 are reserved
#define mbmpBrowserFrameLim     0x2015F
#define mbmpCameraFrame         0x20160
#define mbmpWorkspace           0x20161
#define mbmpBackground1         0x20162
#define mbmpBackground2         0x20163
#define mbmpBackground3         0x20164
#define mbmpBackground4         0x20165
#define mbmpCCPSceneChopBack    0x20166
#define mbmpActorsContinue      0x20167
#define mbmpActorsRotate                0x20168
#define mbmpActorsTransform     0x20169
#define mbmpRotateBackground    0x2016A
#define mbmpTransformBackground 0x2016B
#define mbmpScrollBackground    0x2016C
#define mbmpTextsScrollType     0x2016D
#define mbmpBrowserObj          0x2016E
#define mbmpBrowserThumb                0x20170 //Next 0x100 are reserved
#define mbmpBrowserThumbLim     0X2026F
#define mbmpActorsRotateNorm    0x20270
#define mbmpActorsTransformNorm 0x20271
#define mbmpBrwsSceneFrame      0x20273 // CNO for Scene browser frame
#define mbmpBrwsActorFrame      0x20275 // CNO for Actor browser frame
#define mbmpBrwsMaterial                0x20276
#define mbmpBrwsCMaterial       0x20277
#define mbmpBrwsThreeD          0x20278
#define mbmpBrwsFonts           0x20279
#define mbmpBrwsForeColor       0x2027A
#define mbmpBrwsBackColor       0x2027B
#define mbmpBrwsTextSize                0x2027C
#define mbmpActorsBrowser       0x2027D         // the button itself
#define mbmpActorsPropBrowser   0x2027E // the button itself
#define mbmpActorsSpletters     0x2027F         // the button itself
#define mbmpSoundsEfxBrowser    0x20281         // the button itself
#define mbmpSoundsMicBrowser    0x20282         // the button itself
#define mbmpSoundsMusicBrowser  0x20283 // the button itself
#define mbmpPlayStop            0x20284
#define mbmpSceneChoices        0x20285
#define mbmpVolumeControl       0x20286
#define mbmpForward                     0x20287
#define mbmpForwardEnd          0x20288
#define mbmpRewind                      0x20289
#define mbmpRewindEnd           0x2028A
#define mbmpNukeIcon            0x2028B
#define mbmpSoundRecord         0x2028C
#define mbmpSoundPlay           0x2028D
#define mbmpCostChangeIcon      0x2028E
#define mbmpSceneSlider         0x2028F
#define mbmpFrameSlider         0x20290
#define mbmpSOTransition1       0x20291
#define mbmpSOTransition2       0x20292
#define mbmpSOTransition3       0x20293
#define mbmpSOTransition4       0x20294
#define mbmpRecordMic           0x20295
#define mbmpPortfolioOpen       0x20296
#define mbmpPortfolioSaveAs     0x20297
#define mbmpCCPRemoveBefore     0x20298
#define mbmpCCPRemoveAfter      0x20299
#define mbmpCCPRemoveScene      0x2029A
#define mbmpCCPRemoveObject     0x2029B
#define mbmpActorActions        0x2029C
#define mbmpLeannCancel         0x2029D
#define mbmpLeannOk                     0x2029E
#define mbmpFOScrolling         0x2029F
#define mbmpFONonScrolling      0x202A0
#define mbmpHandCursor          0x202A1
#define mbmpWordBoxCursor       0x202A2
#define mbmpCutWordBox          0x202A3

////////// temporary
#define helppic1016_bmp                 0x202B0
#define helppic1016d_bmp                0x202B1
#define helppic1016r_bmp                0x202B2
#define helppic1020_bmp                 0x202B3
#define helppic1020d_bmp                0x202B4
#define helppic1020r_bmp                0x202B5
#define mbmpOkButton_bmp                0x202B6
#define mbmpOkButtond_bmp               0x202B7
#define mbmpOkButtonr_bmp               0x202B8
#define mbmpQstButton_bmp               0x202B9
#define mbmpQstButtond_bmp              0x202BA
#define mbmpQstButtonr_bmp              0x202BB

#define mbmpRedo                        0x202BC
#define mbmpNukeSound					0x202BD
