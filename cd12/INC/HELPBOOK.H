/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

// defines for the help book

#ifndef HELPBOOK_H
#define HELPBOOK_H

// cursors
#define crsArrow1				0x00001001
#define crsHand1				0x00001002

// waves
#define wavBingo	 			0x00001003

#define	kidResPal				0x00001004

#define kcnoHelpMainWav 		0x00001010
#define kcnoHelpNextWav 		0x00001011
#define kcnoHelpBackWav 		0x00001012

// objects
#define kgobHelpBookPersistent 	0x00003001
#define kgobHelpBookParent		0x00003002
#define kgobHelpBookGizmo		0x00003003
#define kgobHelpBookFill		0x00003004
#define kgobHelpBookFillPar		0x00003005

#define kgobHelpBookPrimary1 	0x00003006
#define kgobHelpBookPrimary2	0x00003007
#define kgobHelpBookPrimary3	0x00003008
#define kgobHelpBookPrimary4	0x00003009
#define kgobHelpBookPrimary5	0x00003010

#define kgobHelpBookVolume 		0x00003011
#define kgobHelpBookVoice		0x00003012
#define kgobHelpBookPageFwd		0x00003013
#define kgobHelpBookPageBack	0x00003014
#define kgobHelpBookCancel		0x00003015

#define kgobHelpBookHowSub		0x00003016
#define kgobHelpHowButton1		0x00003017
#define kgobHelpHowButton2		0x00003018
#define kgobHelpHowButton3		0x00003019
#define kgobHelpHowButton4		0x00003020
#define kgobHelpHowButton5		0x00003021
#define kgobHelpHowButton6		0x00003022
 	
#define kgobHelpBookStyle		0x00003023
#define kgobHelpBookBkgd		0x00003024
#define kgobHelpBookPageLeft	0x00003025
#define kgobHelpBookPageRight	0x00003026
#define kgobHelpBookPageNum		0x00003027

#define kgobHelpBookVolumeSlider	0x00003028
#define kgobHelpBookVolumePath		0x00003029
#define kgobHelpTitlePar		0x00003045

#define ktpcBookToolTopics		0x2000015A
#define	kidHelpBookHowTo		0x2000015B
#define kidHelpBookBasics		0x2000015C
									
#define ktpcQuerySave               0x20000160
#define kgobQuerySave               0x20000161
#define ktpcQuerySaveWithCancel     0x20000162
#define kgobQuerySaveWithCancel     0x20000163
#define ktpcQueryQuit               0x20000164
#define kgobQueryQuit               0x20000165
#define ktpcQueryExitStudio         0x20000166
#define kgobQueryExitStudio         0x20000167
#define ktpcQuerySoundDelete        0x20000168
#define kgobQuerySoundDelete        0x20000169
#define ktpcQueryCD                 0x2000016A
#define kgobQueryCD                 0x2000016B
#define ktpcQueryOverwrite          0x2000016C
#define kgobQueryOverwrite          0x2000016D
#define ktpcQueryPurgeSounds        0x2000016E
#define kgobQueryPurgeSounds        0x2000016F
										 
// string ids
#define stidMaxPage		2
#define stidCurrPage	1

// positions of help book controls
#define kxpHelpBookCancel		479
#define kypHelpBookCancel		403
#define kxpregHelpBookCancel	25	
#define kypregHelpBookCancel	0

#define kxpHelpBookPageFwd		364
#define kypHelpBookPageFwd		412
#define kxpregHelpBookPageFwd 	0
#define kypregHelpBookPageFwd	0

#define kxpHelpBookPageBack		241
#define kypHelpBookPageBack		412
#define kxpregHelpBookPageBack	25
#define kypregHelpBookPageBack	0

#define kxpHelpBookSlider		57
#define kypHelpBookSlider		245
#define kdxpHelpBookSlider		11
#define kdypHelpBookSlider		67

#define kdypHelpBookSliderTotal	74

#define kmaxVolume				0x00010000

#define kxpHelpBookVoice		163
#define kypHelpBookVoice		404	
#define kxpregHelpBookVoice		25
#define kypregHelpBookVoice		0

#define kxpPageNumLeft			280
#define kypPageNumTop			428
#define kxpPageNumRight			360
#define kypPageNumBot			443

// fill width inside help book
#define kFillLeft 	70
#define kFillTop	79
#define kFillRight	571
#define kFillBot	420

// page positions inside help book fill
#define kLeftPageL	  	100
#define kLeftPageL2		178			// when the how to section is present
#define kLeftPageR	  	546
#define kTopPage		130
#define kBottomPage 	390

#define kxpHelpTitle	312
#define kypHelpTitle	90
#define kzpHelpTitle	600

#define kTitleLeft		-240
#define kTitleTop		0
#define kTitleRight 	240
#define kTitleBottom 	60



#endif	// !HELPBOOK_H

