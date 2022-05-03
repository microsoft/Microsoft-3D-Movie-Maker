/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

    Defines for all the studio placing, cids, etc.

    *** USE SPACES FOR ALIGNMENT ***

***************************************************************************/

#ifndef STDIODEF_H
#define STDIODEF_H

// Define this flag to enable frame-per-second display code
// #define SHOW_FPS

#include "socdef.h"
#include "utestres.h"

//
// Cache size needed for the studio
//
#define kcbStudioCache                  (256 * 1024)

//
// Tooltip delay.
//
//  The time below is in 60ths of a second (see clok.h in kauai -
//    couldn't use kdtim... 'cause it's defined to be a const long )

#define kdtimToolTipDelay 				90


//
// Cids for script->code communication
//
#define cidXYAxis                       50000
#define cidXZAxis                       50001
#define cidRecordPath                   50002
#define cidRerecordPath                 50003
#define cidSetTool                      50004
#define cidPlay                         50005
#define cidNewScene                     50006
#define cidRespectGround                50009
#define cidPauseUntilClick              50010
#define cidPauseForSound                50011
#define cidClearPause                   50012
#define cidBrowserFwd                   50013
#define cidBrowserBack                  50014
#define cidBrowserCancel                50015
#define cidBrowserOk                    50016
#define cidBrowserSelect                50017
#define cidBrowserReady                 50018
#define cidBrowserPreview               50019
#define cidBrowserSelectThum            50020
#define cidFrameScrollbar               50021
#define cidSceneScrollbar               50022
#define cidSooner                       50023
#define cidLater                        50024
#define cidEaselTransmogrify            50025
#define cidEaselRotate                  50026
#define cidEaselCancel                  50027
#define cidEaselOk                      50028
#define cidNewSpletter                  50029
#define cidSceneSortInit                50030
#define cidSceneSortSelect              50031
#define cidSceneSortInsert              50032
#define cidSceneSortScroll              50033
#define cidSceneSortNuke                50034
#define cidSceneSortOk                  50035
#define cidSceneSortCancel              50036
#define cidSceneSortPortfolio           50037
#define cidSceneSortTransition          50038
#define cidSSorterNuke                  50039
#define cidSSorterHand                  50040
#define cidSSorterSelect                50041
#define cidApeSetAction                 50042
#define cidApeNextCel                   50043
#define cidEaselKey                     50044
#define cidStartScroll                  50045
#define cidEaselImport                  50046
#define cidEaselTexture                 50047
#define cidEaselShape                   50048
#define cidEaselFont                    50049
#define cidEaselSetFont                 50050
#define cidEaselSetShape                50051
#define cidEaselSetColor                50052
#define cidEaselClosing                 50053
#define cidEaselAccessories             50054
#define cidTextFont                     50055
#define cidTextStyle                    50056
#define cidTextSize                     50057
#define cidTextColor                    50058
#define cidTextBkgdColor                50059
#define cidLoadStudio                   50060
#define cidLoadBuilding                 50061
#define cidTextSetColor                 50062
#define cidTextSetBkgdColor             50063
#define cidTheaterOpen                  50064
#define cidTheaterLoad                  50065
#define cidTheaterPlay                  50066
#define cidTheaterStop                  50067
#define cidTheaterRewind                50068
#define cidTheaterClose                 50069
#define cidBrowserImport                50070
#define cidEaselVol                     50071
#define cidEaselPlay                    50072
#define cidListenVolSet                 50073
#define cidLaunchImport                 50074
#define cidPortfolioFile                50075
#define cidEaselRecord                  50076
#define cidEaselCostumes                50077
#define cidTextSetFont                  50078
#define cidTextSetSize                  50079
#define cidPortfolioOpen                50080
#define cidPortfolioSave                50081
#define cidPortfolioClosed              50082
#define cidSplotInit                    50083
#define cidSplotDo                      50084
#define cidSplotUpdate                  50085
#define cidSplotOk                      50086
#define cidSplotCancel                  50087
#define cidOpenSoundRecord              50088
#define cidRecordSetLength              50089
#define cidLoadProjectMovie             50090
#define cidPortfolioClear               50091
#define cidBrowserChangeCel             50092
#define cidDisableAccel                 50093
#define cidEnableAccel                  50094
#define cidInvokeSplot                  50095
#define cidBrowserDel                   50096
#define cidTextSetStyle                 50097
#define cidProjectMovieLoaded           50098
#define cidBrowserVisible               50099
#define cidTheaterOpenCompleted         50100
#define cidTheaterLoadCompleted         50101
#define cidEaselVisible                 50102
#define cidSoundsEnabled				50103
#define cidCreateTbox					50104
#define cidExitStudio					50105
#define cidModalTopicClosed				50106
#define cidLoadStudioFailed             50107
#define cidActorEaselOpen               50108
#define cidListenerEaselOpen            50109
#define cidLoadStudioDoc                50110
#define cidPortfolioResult              50111
#define cidFrameThumb                   50112
#define cidSceneThumb                   50113
#define cidQuerySaveDocResult           50114

// Misc Studio Strings
#define idsBrowserPage            0
#define idsSoundFont              7
#define idsActionFont             8
#define idsListenFont             9
#define idsRollCallFont           10
#define idsBrwsPageFont           11
#define idsTboxDypFont            12
#define idsStudioFont             13
#define idsMovieNameDypFont       29
			
//
// String Registry Entries
//
#define kstidActor                      0x00500	 	// Thru 504
#define kstidProp                       0x00510		// Thru 514
#define kstidQuerySave                  0x00525
#define kstidQueryCD                    0x00526
#define kstidQuerySoundDelete           0x00527
#define kstidQueryOverwrite             0x00528
#define kstidGenericError               0x00529

//
// Ids for each gob within the studio
//
#define kidStudio                       0x20000
#define kidPalette                      0x20001
#define kidBackground                   0x20002
#define kidSettingsCover                0x20005
#define kidActorsCover                  0x20006
#define kidSoundsCover                  0x20007
#define kidTextsCover                   0x20008
#define kidScrollbarsCover              0x2000B
#define kidUndosCover                   0x2000D
#define kidBooksCover                   0x2000E
#define kidDoors                        0x2000F
#define kidSettingsBackground           0x20010
#define kidActorsBackground             0x20011
#define kidSoundsBackground             0x20012
#define kidTextsBackground              0x20013
#define kidSettingsCameras              0x20014
#define kidActorsSelect                 0x20015
#define kidActorsBiography              0x20016
#define kidActorsCompose                0x20017
#define kidActorsCostume                0x20018
#define kidActorsNormalizeRot           0x20019
#define kidBrwsAction                   0x2001A
#define kidActorsRecordMode             0x2001B
#define kidActorsRotateX                0x2001C
#define kidActorsRotateY                0x2001D
#define kidActorsRotateZ                0x2001E
#define kidActorsSooner                 0x2001F
#define kidActorsSquash                 0x20020
#define kidActorsShrink                 0x20021
#define kidActorsXY                     0x20022
#define kidActorsXZ                     0x20023
#define kidActorsGround                 0x20024
#define kidSoundsMixer                  0x20025
#define kidSoundsPlayOnce               0x20026
#define kidSoundsListen                 0x20027
#define kidSoundsRecord                 0x20028
#define kidSoundsLooping                0x20029
#define kidSoundsAttachToCell           0x2002A
#define kidSoundsClear                  0x2002B
#define kidTextsBkgdColor               0x2002C
#define kidTextsFontShape               0x2002D
#define kidTextsFontColor               0x2002E
#define kidTextsFont                    0x2002F
#define kidTextsSelect                  0x20030
#define kidTextsScroll                  0x20031
#define kidTextsFontSize                0x20032
#define kidTextsStory                   0x20033
#define kidBook                         0x20034
#define kidMap                          0x20035
#define kidCutCopyPaste                 0x20036
#define kidUndo                         0x20037
#define kidVolumeSlider                 0x20038
#define kidPlay                         0x20039
#define kidSceneSorter                  0x2003A
#define kidPortfolio                    0x2003C
#define kidExit                         0x2003D
#define kidBrwsBackground               0x2003E
#define kidBrwsActor                    0x2003F
#define kidBrwsProp                     0x20040
#define kidBrwsThreeD                   0x20041
#define kidSettingsBrowser              0x20042
#define kidBrwsMidi		                0x20043
#define kidBrwsFX                  		0x20044
#define kidBrwsSpeech                 	0x20045
#define kidTextsCreate                  0x20046
#define kidPortsBackground              0x20047
#define kidPortsPortfolio               0x20048
#define kidPortsNew                     0x20049
#define kidPortsSave                    0x2004A
#define kidPausesMouse                  0x2004C
#define kidPausesClear                  0x2004D
#define kidPausesSound                  0x2004E
#define kidCCPBackground                0x2004F
#define kidCCPSceneChopFwd              0x20050
#define kidCCPSceneNuke                 0x20051
#define kidCCPActorNuke                 0x20052
#define kidCCPCopyRoute                 0x20053
#define kidCCPPaste                     0x20054
#define kidCCPCopy                      0x20055
#define kidCCPCut                       0x20056
#define kidGenericCancel                0x20058
#define kidGenericOk                    0x20059
#define kidGenericPageFwd               0x2005A
#define kidGenericPageBack              0x2005B
#define kidCameraFrame					0x2005C
#define kidWorkspace					0x2005D
#define kidBackground1					0x2005E
#define kidBackground2					0x2005F
#define kidBackground3					0x20060
#define kidBackground4					0x20061
#define kidCCPSceneChopBack 			0x20062
#define kidActorsContinue				0x20063
#define kidActorsRotate 				0x20064
#define kidActorsTransform				0x20065
#define kidRotateBackground 			0x20066
#define kidTransformBackground			0x20067
#define kidScrollBackground 			0x20068
#define kidTextsScrollType				0x20069
#define kidBrowserObj					0x2006A
#define kidActorsRotateNorm 			0x2006B
#define kidActorsTransformNorm			0x2006C
#define kidBrwsSceneFrame				0x2006D // CNO for Scene browser frame
#define kidBrwsActorFrame				0x2006E // CNO for Actor browser frame
#define kidBrwsMaterial 				0x2006F
#define kidBrwsCMaterial				0x20070
#define kidBrwsFonts					0x20071
#define kidBrwsForeColor				0x20072
#define kidBrwsBackColor				0x20073
#define kidBrwsTextSize 				0x20074
#define kidActorsBrowser				0x20075 // the button itself
#define kidActorsPropBrowser			0x20076 // the button itself
#define kidActorsSpletters				0x20077 // the button itself
#define kidSoundsEfxBrowser 			0x20078 // the button itself
#define kidSoundsMicBrowser 			0x20079 // the button itself
#define kidSoundsMusicBrowser			0x2007A // the button itself
#define kidActorsActionBrowser			0x2007B // the button itself
#define kidBrwsCamera					0x2007C
#define kidBrwsCameraFrame				0x2007D // CNO for Camera browser frame
#define kidBrwsPropFrame				0x2007E // CNO for Camera browser frame
#define kidBrwsActionFrame				0x2007F // CNO for Camera browser frame
#define kidBrwsActionPrev				0x20080 // The action preview frame.
#define kidGenericGlass 				0x20081 // Covers all the tools to protect them from clicks
#define kidFrameBackground				0x20082
#define kidFrameTitle					0x20083
#define kidFrameText					0x20084
#define kidFrameFW						0x20085
#define kidFrameFWEnd					0x20086
#define kidFrameRW						0x20087
#define kidFrameRWEnd					0x20088
#define kidFrameScrollbar				0x20089
#define kidFrameThumb					0x2008A
#define kidSceneBackground	 			0x2008B
#define kidSceneTitle					0x2008C
#define kidSceneText					0x2008D
#define kidSceneFW						0x2008E
#define kidSceneFWEnd					0x2008F
#define kidSceneRW						0x20090
#define kidSceneRWEnd					0x20091
#define kidSceneScrollbar				0x20092
#define kidSceneThumb					0x20093
#define	kidName 						0x20094
#define	kidPortsSaveAs					0x20095
#define kidRollCallActor    			0x20096
#define kidRollCallProp 				0x20097
#define kidCCPTboxNuke					0x20098
#define kidFps							0x20099
#define kidActorsSoonerButton			0x2009B
#define kidActorsLaterButton			0x2009C
#define kidBrwsRollCall					0x2009D
#define kidSpltBackground				0x2009E
#define kidSpltTransmogrify				0x2009F
#define kidSpltTextures					0x200A0
#define kidSpltShape					0x200A1
#define kidSpltRotate					0x200A2
#define kidSpltImportTexture			0x200A3
#define kidSpltCancel					0x200A4
#define kidSpltOk						0x200A5
#define	kidSpltPreviewFrame				0x200A6
#define kidSpltFont						0x200A7
#define kidCostBackground				0x200A8
#define kidCostCostumes					0x200A9
#define kidCostRotate					0x200AA
#define kidCostAccessories				0x200AB
#define kidCostCancel					0x200AC
#define kidCostOk						0x200AD
#define kidCostPreviewFrame				0x200AE

/* Scene sorter */
#define kidSSorterBackground            0x200AF
#define khidSceneSorter                 0x200B0
#define kidSSorterOpen                  0x200B1
#define kidSSorterScrollUp              0x200B2
#define kidSSorterScrollDn              0x200B3
#define kidSSorterCancel                0x200B4
#define kidSSorterOk                    0x200B5
#define kidSSorterHand                  0x200B6
/* NOTE: the thumb kids *must* be immediately before the frame kids;
 they are used in reverse order, starting at kidSSorterFrame - 1 */
#define kidSceneSortThumbMin            0x200B7 // Next 0x20 are reserved
#define kidSSorterFrame                 0x200D7 // Next 0x40 are reserved
#define kidSSorterNuke                  0x20117
#define kidTransition1                  0x20118
#define kidTransition2                  0x20119
#define kidTransition3                  0x2011A
#define kidTransition4                  0x2011B
#define kidSSorterThumbFrame            0x2011C
#define kidBrwsTextCreate               0x2011D
#define kidCTextsStory                  0x2011E
#define kidCTextsScroll                 0x2011F
#define kidSpltEditBox					0x20121
#define kidCostEditBox					0x20123
#define kidSpltGlass					0x20124
#define kidCostGlass					0x20125
#define kidSettingsGlass				0x20126
#define kidCameraGlass					0x20127
#define kidActorGlass					0x20128
#define kidPropGlass					0x20129
#define kidActionGlass					0x2012A
#define kidFXGlass						0x2012B
#define kidMidiGlass					0x2012C
#define kidSpeechGlass					0x2012D
#define kidBrwsSoundsBackground			0x2012E		
#define kidSSorterBar	                0x2012F

#define kidNewSoundBackground			0x20131		
#define kidNewSoundCancel				0x20132		
#define kidNewSoundOk					0x20133		
#define kidNewSoundEditBox				0x20134		
#define kidNewSoundRecord				0x20135		
#define kidNewSoundPlay					0x20136		
#define kidListenBackgroundActor        0x20137		
#define kidListenCancel			        0x20138		
#define kidListenOk				        0x20139		
#define kidListenGlassActor				0x2013A		
#define kidBrwsSoundFrame				0x2013B		
#define kidListenVolSpeech              0x2013C
#define kidListenVolFX                  0x2013D
#define kidListenVolSpeechMM            0x2013E
#define kidListenVolFXMM                0x2013F
#define kidListenVolMidi               0x20140
#define kidListenEditBoxSpeech		    0x20141
#define kidListenEditBoxFX  		    0x20142
#define kidListenEditBoxSpeechMM	    0x20143
#define kidListenEditBoxFXMM  		    0x20144
#define kidListenEditBoxMidi		    0x20145
#define kidListenNukeSpeech		        0x20146
#define kidListenNukeFX			        0x20147
#define kidListenNukeSpeechMM	        0x20148
#define kidListenNukeFXMM		        0x20149
#define kidListenNukeMidi			    0x2014A
#define kidSoundsImportGlass			0x2014B
#define kidBrwsSoundsImportBackground	0x2014C
#define kidBrwsSoundImportFrame			0x2014D
#define kidListenBackgroundBkgd	        0x2014E
#define kidListenGlassBkgd				0x2014F
#define kidListenMidiIcon				0x20150
#define kidListenSpeechIcon				0x20151
#define kidListenSpeechMMIcon			0x20152
#define kidListenFXIcon					0x20153
#define kidListenFXMMIcon				0x20154
#define kidBrwsImportFX					0x20155
#define kidBrwsImportMidi				0x20156
#define kidBrwsImportSpeech				0x20157
#define kidRecordGlass					0x20158
#define kidRecordBackground				0x20159
#define kidRecordSoundLength			0x2015A
#define kidRecordSoundName				0x2015B
#define kidRecordRecord					0x2015C
#define kidRecordPlay					0x2015D
#define kidRecordCancel					0x2015E
#define kidRecordOk						0x2015F
#define kidGadget						0x20160
#define kidHelpAnchor					0x20161
#define kidGenericPageNum               0x20162
#define kidVolumeThumb					0x20163
#define kidRollCallActorBorder1			0x20164
#define kidRollCallActorBorder2			0x20165
#define kidRollCallActorBorder3			0x20166
#define kidRollCallActorBorder4			0x20167
#define kidRollCallPropBorder1			0x20168
#define kidRollCallPropBorder2			0x20169
#define kidRollCallPropBorder3			0x2016A
#define kidRollCallPropBorder4			0x2016B
#define kidListenVolSpeechThumb 		0x2016C
#define kidListenVolFXThumb 			0x2016D
#define kidListenVolMidiThumb 			0x2016E
#define kidListenVolSpeechMMThumb 		0x2016F
#define kidListenVolFXMMThumb 			0x20170



//
// Browser Controls
// Browser buttons require a range of kids
//
#define kidRollCallActorDownArrow		0x21000		
#define kidRollCallActorUpArrow			0x21001
#define kidRollCallPropDownArrow		0x21002		
#define kidRollCallPropUpArrow		    0x21003	
#define kidImportPageFwd				0x21004		// Sound import	
#define kidImportPageBack				0x21005	
#define kidImportOk						0x21006	
#define kidImportCancel					0x21007	
#define kidImportPageNum                0x21008
#define kidReserveProjects				0x21009

// The following ordering of the control buttons must be preserved
#define kidBrowserPageFwd               0x21010	   	// Control buttons are ordered	
#define kidBrowserPageBack              0x21011
#define kidBrowserOk                    0x21012
#define kidBrowserCancel                0x21013
#define kidBrowserImport                0x21014
#define kidBrowserRecord				0x21015
#define kidBrowserPageNum               0x21016
#define kidBrowserCtrlLim               0x21020    //Reserved for future controls
// Browser Frames
#define kidRollCallActorFrame			0x21100	 	// Reserving future frames
#define kidRollCallActorFrame1			0x21100
#define kidRollCallActorFrame2			0x21101
#define kidRollCallActorFrame3			0x21102
#define kidRollCallActorFrame4			0x21103
#define kidRollCallPropFrame			0x21110	
#define kidRollCallPropFrame1			0x21110		
#define kidRollCallPropFrame2			0x21111
#define kidRollCallPropFrame3			0x21112		
#define kidRollCallPropFrame4			0x21113
#define kidBrowserFrame                 0x21120
#define kidBrowserFrameLim              0x21200
// Import Frames
#define kidImportFrame					0x21301		
#define kidImportThumbLim				0x21500


//
// Ids for popdowns
//
#define kidSpltsColor					0x21501
#define kidSpltsColorBackground			0x21502
#define kidSpltsColorPageDown			0x21503
#define kidSpltsColorPageUp				0x21504
#define kidSpltsColorFrame				0x21505
#define kidSpltsColorFrameLim			0x21510

#define kidSpltsShape					0x21511
#define kidSpltsShapeBackground			0x21512
#define kidSpltsShapePageDown			0x21513
#define kidSpltsShapePageUp				0x21514
#define kidSpltsShapeFrame				0x21515
#define kidSpltsShapeFrameLim			0x21520

#define kidSpltsFont					0x21521
#define kidSpltsFontBackground			0x21522
#define kidSpltsFontPageDown			0x21523
#define kidSpltsFontPageUp				0x21524
#define kidSpltsFontFrame				0x21525
#define kidSpltsFontFrameLim			0x21530

#define kidActorsAcc					0x21531
#define kidActorsAccBackground			0x21532
#define kidActorsAccPageDown			0x21533
#define kidActorsAccPageUp				0x21534
#define kidActorsAccFrame				0x21535
#define kidActorsAccFrameLim			0x21540

#define kidTextFont						0x21541
#define kidTextFontBackground			0x21542
#define kidTextFontPageDown				0x21543
#define kidTextFontPageUp				0x21544
#define kidTextFontFrame				0x21545
#define kidTextFontFrameLim				0x21550

#define kidTextStyle					0x21551
#define kidTextStyleBackground			0x21552
#define kidTextStylePageDown			0x21553
#define kidTextStylePageUp				0x21554
#define kidTextStyleFrame				0x21555
#define kidTextStyleFrameLim			0x21560

#define kidTextSize						0x21561
#define kidTextSizeBackground			0x21562
#define kidTextSizePageDown				0x21563
#define kidTextSizePageUp				0x21564
#define kidTextSizeFrame				0x21565
#define kidTextSizeFrameLim				0x21570

#define kidTextColor					0x21571
#define kidTextColorBackground			0x21572
#define kidTextColorPageDown			0x21573
#define kidTextColorPageUp				0x21574
#define kidTextColorFrame				0x21575
#define kidTextColorFrameLim			0x21580

#define kidTextBkgdColor				0x21581
#define kidTextBkgdColorBackground		0x21582
#define kidTextBkgdColorPageDown		0x21583
#define kidTextBkgdColorPageUp			0x21584
#define kidTextBkgdColorFrame			0x21585
#define kidTextBkgdColorFrameLim		0x21590

// Action preview buttons
#define kidBrowserActionBackCel			0x21591
#define kidBrowserActionChangeCel		0x21592
#define kidBrowserActionFwdCel			0x21593

#define kidGenericDisableGlass			0x21594 // Covers all the tools to protect them from clicks
#define kidTextFontThumb                0x21595

#define kidPressPageFwd					0x21596
#define kidPressPageBack				0x21597



//
// Start tool tip CNOs
//
#define kttGenericPageFwd               0x22300
#define kttGenericPageBack              0x22301
#define kttGenericCancel                0x22302
#define kttGenericOk                    0x22303
#define kttPortsPortfolio               0x22304
#define kttPortsNew                     0x22305
#define kttPortsSave                    0x22306
#define kttCCPSceneChopFwd              0x22307
#define kttCCPSceneChopBack             0x22308
#define kttCCPSceneNuke                 0x22309
#define kttCCPActorNuke                 0x2230A
#define kttCCPCopyRoute                 0x2230B
#define kttCCPPaste                     0x2230C
#define kttCCPCopy                      0x2230D
#define kttCCPCut                       0x2230E
#define kttActorsRotateX                0x2230F
#define kttActorsRotateY                0x22310
#define kttActorsRotateZ                0x22311
#define kttActorsRotateNorm             0x22312
#define kttActorsSquash                 0x22313
#define kttActorsShrink                 0x22314
#define kttActorsTransformNorm          0x22315
#define kttTextsScroll                  0x22316
#define kttTextsStory                   0x22317
#define kttSettingsCameras              0x22318
#define kttSceneSorter                  0x22319
#define kttPausesMouse                  0x2231A
#define kttPausesClear                  0x2231B
#define kttPausesSound                  0x2231C
#define kttActorsSelect                 0x2231D
#define kttActorsBiography              0x2231E
#define kttActorsCompose                0x2231F
#define kttActorsCostume                0x22320
#define kttActorsActionBrowser          0x22321
#define kttActorsContinue               0x22322
#define kttActorsRecordMode             0x22323
#define kttActorsSooner                 0x22324
#define kttActorsRotate                 0x22325
#define kttActorsTransform              0x22326
#define kttActorsXY                     0x22327
#define kttActorsGround                 0x22328
#define kttSoundsPlayOnce               0x22329
#define kttSoundsListen                 0x2232A
#define kttSoundsLooping                0x2232B
#define kttSoundsAttachToCell           0x2232C
#define kttTextsBkgdColor               0x2232D
#define kttTextsFontShape               0x2232E
#define kttTextsFontColor               0x2232F
#define kttTextsFont                    0x22330
#define kttTextsSelect                  0x22331
#define kttTextsFontSize                0x22332
#define kttTextsScrollType              0x22333
#define kttSettingsCover                0x22334
#define kttActorsCover                  0x22335
#define kttSoundsCover                  0x22336
#define kttTextsCover                   0x22337
#define kttBook                         0x22338
#define kttMap                          0x22339
#define kttCutCopyPaste                 0x2233A
#define kttUndo                         0x2233B
#define kttVolumeSlider                 0x2233C
#define kttPlay                         0x2233D
#define kttPortfolio                    0x2233E
#define kttExit                         0x2233F
#define kttSettingsBrowser              0x22340
#define kttActorsBrowser                0x22341
#define kttActorsPropBrowser            0x22342
#define kttActorsSpletters              0x22343
#define kttSoundsEfxBrowser             0x22344
#define kttSoundsMicBrowser             0x22345
#define kttSoundsMusicBrowser           0x22346
#define kttTextsCreate                  0x22347			
#define kttActorsSoonerButton			0x22348
#define kttActorsLaterButton			0x22349
#define kttSpltBackground				0x2234A
#define kttSpltTransmogrify				0x2234B
#define kttSpltTextures					0x2234C
#define kttSpltShape					0x2234D
#define kttSpltRotate					0x2234E
#define kttSpltImportTexture			0x2234F
#define kttSpltCancel					0x22350
#define kttSpltOk						0x22351
#define kttSpltFont						0x22352
#define kttCostBackground				0x22353
#define kttCostCostumes					0x22354
#define kttCostRotate					0x22355
#define kttCostAccessories				0x22356
#define kttCostCancel					0x22357
#define kttCostOk						0x22358
#define kttSSorterBackground			0x22359
#define kttSSorterFrame					0x2235A
#define kttTransition1					0x2235B
#define kttTransition2					0x2235C
#define kttTransition3					0x2235D
#define kttTransition4					0x2235E
#define kttSSorterScrollUp				0x2235F
#define kttSSorterScrollDn				0x22360
#define kttSSorterCancel				0x22361
#define kttSSorterOk					0x22362
#define kttSSorterHand					0x22363
#define kttSSorterNuke					0x22364
#define kttSSorterOpen					0x22365
#define kttSSorterThumbFrame			0x22366
#define kttBrwsTextCreate				0x22367
#define kttCTextsStory					0x22368
#define kttCTextsScroll					0x22369
#define kttBrowserRecord				0x2236A		
#define kttBrowserImportFX				0x2236B		
#define kttRollCallActorFrame			0x2236C	 	// Reserving future frames thru 2238B
#define kttRollCallActorFrame1			0x2236D
#define kttRollCallActorFrame2			0x2236E
#define kttRollCallActorFrame3			0x2236F
#define kttRollCallActorFrame4			0x22370
#define kttRollCallActorUpArrow 		0x2238A
#define kttRollCallActorDownArrow		0x2238B
#define kttRollCallPropFrame			0x2238C	 	// Reserving future frames thru 223AD
#define kttRollCallPropFrame1			0x2238D
#define kttRollCallPropFrame2			0x2238E
#define kttRollCallPropFrame3			0x2238F
#define kttRollCallPropFrame4			0x22390
#define kttRollCallPropUpArrow			0x223AE
#define kttRollCallPropDownArrow		0x223AF

// Note: ***** - 6.19.95  --these ids conflicted with spletter easel #defines...
//  any particular reason other than human error?

#define kttSceneBackground				0x223B0
#define kttRollCallProps				0x223B1
#define kttFrameBackground				0x223B2
#define	kttPortsSaveAs					0x223B3
#define kttRollCallActors				0x223B4
#define kttCCPTboxNuke					0x223B5
#define kttListenVolSpeech			    0x223B6
#define kttListenVolFX				    0x223B7
#define kttListenVolSpeechMM		    0x223B8
#define kttListenVolFXMM			    0x223B9
#define kttListenVolMidi			    0x223BA
#define kttListenNukeSpeech		        0x223BB
#define kttListenNukeFX			        0x223BC
#define kttListenNukeSpeechMM	        0x223BD
#define kttListenNukeFXMM		        0x223BE
#define kttListenNukeMidi			    0x223BF
#define kttListenCancel			        0x223C0
#define kttListenOk				        0x223C1
#define kttListenEditBoxSpeech		    0x223C2
#define kttListenEditBoxFX  		    0x223C3
#define kttListenEditBoxSpeechMM	    0x223C4
#define kttListenEditBoxFXMM  		    0x223C5
#define kttListenEditBoxMidi		    0x223C6
#define kttImportPageFwd                0x223C7
#define kttImportPageBack               0x223C8
#define kttImportCancel                 0x223C9
#define kttImportOk                     0x223CA
#define kttRecordSoundLength			0x223CB
#define kttRecordSoundName				0x223CC
#define kttRecordRecord					0x223CD
#define kttRecordPlay					0x223CE
#define kttRecordCancel					0x223CF
#define kttRecordOk						0x223D0
#define kttBrwsActionChangeCel			0x223D1
#define kttBrwsActionBackCel			0x223D2
#define kttBrwsActionFwdCel				0x223D3
#define kttBrowserDelete				0x223D4
#define kttEaselOk						0x223D5
#define kttEaselCancel					0x223D6
#define kttRemoveSound					0x223D7
#define	kttRewindEnd					0x223D8
#define kttRewind						0x223D9
#define kttFForward						0x223DA
#define kttFForwardEnd					0x223DB
#define kttFirstScene					0x223DC
#define kttPrevScene					0x223DD
#define kttNextScene					0x223DE
#define kttLastScene					0x223DF
#define kttSceneSlider					0x223E0
#define kttFrameSlider					0x223E1
#define kttBrowserImportSpeech			0x223E2
#define kttBrowserImportMidi			0x223E3		

//
// Sound CNOs
//
#define kcnoCoverWav					0x23001
#define kcnoPBrowseWav					0x23002
#define kcnoDropDnWav					0x23003
#define kcnoSBrowseWav					0x23004
#define kcnoBrwzPicWav					0x23005
#define kcnoBrwzBtnWav					0x23006
#define kcnoEzlOpenWav					0x23007
#define kcnoStateOnWav					0x23008
#define kcnoStateOffWav					0x23009
#define kcnoGrabSnWav					0x2300A
#define kcnoDropSnWav					0x2300B
#define kcnoSelectWav					0x2300C
#define kcnoSquashWav					0x2300D
#define kcnoStretchWav					0x2300E
#define kcnoGrowWav						0x2300F
#define kcnoShrinkWav					0x23010
#define kcnoCBackSWav					0x23011
#define kcnoRotateWav					0x23012
#define kcnoCBackRWav					0x23013
#define kcnoSFreezeWav					0x23014
#define kcnoSyStepFWav					0x23015
#define kcnoSyStepBWav					0x23016
#define kcnoMoveWav						0x23017
#define kcnoMove1Wav					0x23018
#define kcnoMoveAllWav					0x23019
#define kcnoActStepFWav					0x2301A
#define kcnoActStepBWav					0x2301B
#define kcnoTBkOpenWav					0x2301D
#define kcnoSPlayWav					0x2301E
#define kcnoSLoopWav					0x2301F
#define kcnoSActWav						0x23020
#define kcnoWFontWav					0x23022
#define kcnoWSizeWav					0x23023
#define kcnoWColorWav					0x23024
#define kcnoWStyleWav					0x23025
#define kcnoWBgClrWav					0x23026
#define kcnoWScrOnWav					0x23027
#define kcnoWScrOffWav					0x23028
#define kcnoGoFrstFWav					0x23029
#define kcnoGoPrevFWav					0x2302A
#define kcnoGoNextFWav					0x2302B
#define kcnoAddFrameWav					0x2302C
#define kcnoGoLastFWav					0x2302D
#define kcnoGoFrstSWav					0x2302E
#define kcnoGoPrevSWav					0x2302F
#define kcnoGoNextSWav					0x23030
#define kcnoGoLastSWav					0x23031
#define kcnoPlayWav						0x23032
#define kcnoUndoWav						0x23033
#define kcnoRedoWav						0x23034
#define kcnoRemBfrWav					0x23035
#define kcnoRemAftrWav					0x23036
#define kcnoRemScnWav					0x23037
#define kcnoRemActrWav					0x23038
#define kcnoRemWBoxWav					0x23039
#define kcnoPasteWav					0x2303A
#define kcnoCopyPWav					0x2303B
#define kcnoCopyWav						0x2303C
#define kcnoCutWav						0x2303D
#define kcnoStopWav						0x2303E
#define kcnoSecToolWav					0x2303F
#define kcnoOkWav						0x23040
#define kcnoCancelWav					0x23041
#define kcnoFNewWav						0x23042
#define kcnoFSaveWav					0x23043
#define kcnoFSaveAsWav					0x23044
#define kcnoPFOpenWav					0x23045
#define kcnoExitWav						0x23046
#define kcnoPageUpWav					0x23047
#define kcnoPageDnWav					0x23048
#define kcnoRSwitchWav					0x23049
#define kcnoLSelectWav					0x2304A
#define kcnoNukeSnWav					0x2304B
#define kcnoCostumeWav					0x2304C
#define kcnoAcesoryWav					0x2304D
#define kcnoSpinWav						0x2304E
#define kcnoMikeWav						0x2304F
#define kcnoRPlayWav					0x23050
#define kcnoPSelectWav					0x23051
#define kcnoRandomWav					0x23052
#define kcnoMapOpenWav					0x23053
#define kcnoRecordWav					0x23054


//
// the following are id's for help topics used as labels
//
#define klabFrameCounter                0x22500
#define klabSceneCounter                0x22501
#define klabName                        0x22502
#define klabFps                         0x22503


// Where to go in building after leaving studio.  The temp variable is
// used to fix the real variable if an aborted attempt to go to the building
// is made from the map. (Bug #1010)
#define kpridBuildingGob                0x23300
#define kpridBuildingGobT					0x23301
#define kpridBuildingState					0x23302
#define kpridBuildingStateT				0x23303

// the following are "Property Id's" for Browsers.
//
// The property foobarDef indicates the id of a default item that should
// show in the browser page the first time it's instantiated per session
// This will differ by series member.
//
// The property foobarPref is maintained by the browser code and IS
// OVERWRITEABLE from script.  Upon creation, the browser will search
// its content list and display the page which contains this content
//
// NOTE: the only ones that really matter for scripting are the
// actor, prop, background and sound* browsers.

#define kpridBrwsBackgroundDef          0x23400
#define kpridBrwsBackgroundPref         0x23401
#define kpridBrwsCameraDef              0x23402
#define kpridBrwsCameraPref             0x23403

#define kpridBrwsActorDef               0x23404
#define kpridBrwsActorPref              0x23405
#define kpridBrwsPropDef                0x23406
#define kpridBrwsPropPref               0x23407
#define kpridBrwsActionDef              0x23408
#define kpridBrwsActionPref             0x23409

#define kpridBrwsBkSoundDef             0x2340A
#define kpridBrwsBkSoundPref            0x2340B
#define kpridBrwsFXSoundDef             0x2340C
#define kpridBrwsFXSoundPref            0x2340D
#define kpridBrwsMusSoundDef            0x2340E
#define kpridBrwsMusSoundPref           0x2340F

#define kpridBrwsDefaultSid             0x23410
#define kpridBrwsDefaultThum			0x23411
#define kpridBrwsOverrideThum			0x23412
#define kpridBrwsOverrideSidThum        0x23413
#define kpridBrwsOverrideKidThum		0x23414

// kprids for user's data (bio pages, etc)
#define kpridUserDataBase               0x23500
// these two are used for BIO PAGES
#define kpridUserData0                  0x23500
#define kpridUserData1                  0x23501

// this one is used for Studio information
#define kpridStudioFlags				0x23502
#define kpridUserData2                  0x23502

// this one is used for maintaing audio help on /off status
#define kpridMcZeeAudio					0x23503
#define kpridUserData3                  0x23503

// bit flags for suppressing the above audio,
#define kflgMcZeeAudio 			0x01
#define kflgMelAudio			0x02

#define kpridUserData4                  0x23504
#define kpridUserData5                  0x23505
#define kpridUserData6                  0x23506
#define kpridUserData7                  0x23507
#define kcpridUserData                  8

#define kpridFirstTimeUser              0x23510


#define kidStudioLim                    0x23600



//
// Width of screen and workspace
//
#define kdxpBackground                  640
#define kdypBackground                  480
#define kdxpWorkspace                   544
#define kdypWorkspace                   306

#define khidStudio                      khidLimKidFrame + 5


//
// Current tool in use
//
#define chttNone                        0
#define chttCompose                     1
#define chttTboxSelect                  2
#define chttTboxStory                   3
#define chttTboxScroll                  4
#define chttNormalizeRot                5
#define chttSooner                      6
#define chttRotateX                     7
#define chttRotateY                     8
#define chttRotateZ                     9
#define chttSquash                      10
#define chttShrink                      11
#define chttSceneChopFwd                13
#define chttSceneNuke                   14
#define chttActorNuke                   15
#define chttSceneChopBack               16
#define chttNormalizeSize               17
#define chttRotateNorm                  20
#define chttTransformNorm               21
#define chttFButtonFW                   22
#define chttButtonFWEnd                 23
#define chttFButtonRW                   24
#define chttButtonRWEnd                 25
#define chttScrollbar                   26
#define chttThumb                       27
#define chttRecordSameAction            28
#define chttActorEasel					29
#define chttSButtonFW                   30
#define chttSButtonRW                   31
#define chttAction						32
#define chttLooper                      33
#define chttMatcher                     34
#define chttSounder                     35
#define chttListener                    36

//
// States for kcrsors
//
#define fcustHand						0x10000
#define fcustNuke						0x20000

//
// Types of text boxes
//
#define ktbxtScroll                     0
#define ktbxtStory                      1

//
// States of buttons
//
#define kstDisabled                     kst1
#define kstClosed						kst2
#define kstOpen                         kst3

#define kstDefault                      kst1
#define kstSelected                     kst2
#define kstRecording                    kst3 // for the play button to flash
#define kstFreeze						kst3 // for the action browser animation

#define kstSceneDoorsOpen				kst2
#define kstActorDoorsOpen				kst3
#define kstSoundDoorsOpen				kst4
#define kstTextDoorsOpen				kst5
#define kstSceneDoorsAllOpen			kst6
#define kstActorDoorsAllOpen			kst7
#define kstSoundDoorsAllOpen			kst8
#define kstTextDoorsAllOpen				kst9

#define kstBrowserInvisible             kst1
#define kstBrowserEnabled               kst2
#define kstBrowserDisabled              kst3
#define kstBrowserSelected              kst4
#define kstBrowserDragging              kst5
#define kstBrowserScrollingSel          kst6

#define kstListenDisabled				kst1
#define kstListenLooper  				kst2
#define kstListenSounderChain			kst3
#define kstListenMatcher				kst4
#define kstListenSounder				kst5

#define kstSliderInvisible				kst2
#define kstSliderEnabled				kst3

#define fgrfstDisabled                  fgrfst1
#define fgrfstClosed                    fgrfst2
#define fgrfstOpen                      fgrfst3

#define fgrfstDefault                   fgrfst1
#define fgrfstSelected                  fgrfst2

#define fgrfstSceneDoorsOpen			fgrfst2
#define fgrfstActorDoorsOpen			fgrfst3
#define fgrfstSoundDoorsOpen			fgrfst4
#define fgrfstTextDoorsOpen				fgrfst5
#define fgrfstSceneDoorsAllOpen			fgrfst6
#define fgrfstActorDoorsAllOpen			fgrfst7
#define fgrfstSoundDoorsAllOpen			fgrfst8
#define fgrfstTextDoorsAllOpen			fgrfst9

#define fgrfstBrowserInvisible          fgrfst1
#define fgrfstBrowserEnabled            fgrfst2
#define fgrfstBrowserDisabled           fgrfst3
#define fgrfstBrowserSelected           fgrfst4
#define fgrfstBrowserDragging           fgrfst5
#define fgrfstBrowserScrollingSel       fgrfst6



//
// Script IDs
//
#define kchidResetTools                 kchidScript2
#define kchidSetPauseType               kchidScript3
#define kchidEnableSceneTools           kchidScript5
#define kchidEnableActorTools           kchidScript6
#define kchidEnableTboxTools            kchidScript7
#define kchidBrowserDismiss             kchidScript2
#define kchidSSorterNuke                kchidScript2
#define kchidSSorterHand                kchidScript3
#define kchidMovieTechniques            kchidScript4
#define kchidFlyingLogo					kchidScript8
#define kchidResetXZAxisAndGround		kchidScript9
#define kchidOpenDoorsAll				kchidScript10
#define kchidPopoutSceneTools			kchidScript11

// All possible open portfolio types
#define kpfPortOpenMovie					1
#define kpfPortOpenSound					2
#define kpfPortOpenTexture					3

// All possible save portfolio types
#define kpfPortSaveMovie					1

// To set portfolio initial dir if necessary
#define kpfPortDirUsers						1

/***************************************************************************
	Sound classes
***************************************************************************/

#define ksclUISound				   10000


#endif // STDIODEF
