/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

//////////////////////////////////////////////
//	defines for projmain
//////////////////////////////////////////////
#ifndef PROJMAIN_H
#define PROJMAIN_H

// the following are the file names of the project movies loaded into the workspace
// localize at will...

#define PROJECT1_MOVIENAME	"DIALOGUE.3mm"
#define PROJECT2_MOVIENAME	"JOG.3mm"
#define PROJECT3_MOVIENAME	"SURPRISE.3mm"
#define PROJECT4_MOVIENAME	"MUMMY.3mm"
#define PROJECT5_MOVIENAME	"BONGO.3mm"
#define PROJECT6_MOVIENAME	"BRIDGE.3mm"

// gadget objects
#define kidGadgetPersistent 	0x00003030
#define kidGadgetParent			0x00003031
#define kidGadgetBody			0x00003032	
#define kidGadgetPlay			0x00003033
#define kidGadgetVolume	   		0x00003034
#define kidGadgetVoice			0x00003035
#define kidGadgetCancel			0x00003036
#define kidGadgetSelectorUp		0x00003037
#define kidGadgetSelectorNum	0x00003038
#define kidGadgetSelectorDown	0x00003039
#define kidGadgetAvi			0x0000303A
#define kidGadgetWord			0x0000303B
#define kidGadgetLockerFill		0x0000303C
#define kidGadgetHow			0x0000303D
#define kidGadgetWhy			0x0000303E
#define kidGadgetLockerPict		0x0000303F
#define kidGadgetAviFrame		0x00003040
#define kidGadgetVolumeThumb  	0x00003041
#define	kidGadgetAlertGlass		0x00003042
#define kidGadgetVolumePath		0x00003043
#define kidGadgetTextBox		0x00003044
#define kidGadgetStartUp		0x00003046 //3045 is used in helpbook.h
#define kidGadgetStartOk		0x00003047

// Instruction Ids
#define EndTable				0	// not an instruction, just a marker
#define InitState				1
#define	MatchGoto				2
#define MatchDescGoto			3
#define	MatchChildrenGoto		4
#define	MatchAllGoto			5
#define	IgnoreMatch				6
#define	IgnoreChildren			7
#define	IgnoreAll				8
#define	BoolCondGoto			9
#define StateCondGoto			10
#define SpecialInstr			11
#define WaitEvent				12
#define JumpToState				13
#define EndState				14
#define MatchParentGoto 		15
#define IgnoreParentMatch		16

// Special Instruction Ids
#define QuitMM					1
#define QuitHelp				2
#define QuitGadgetHelp			3
#define SetHelpAlarm			4
#define ProjectsRoom			5
#define ResetTipDelay			6
#define ClearStudio				7
#define LoadProjectMovie		8
#define OpenDoorCovers			9
#define	SetSceneAndFrame		10
#define SetBlinkingGadget		11
#define OpenAllDoors			12
#define ChangeToNextMovie		13
#define SimulateClick			14
#define SetBrowserItem			15
#define SetNextProject			16

// Parent States
#define ParState2				0
#define	ParState3				1
#define	ParState4				2
#define	ParState5				3

#define NotRel					0
#define	NoHelpHere				0
#define PrevState				-1

// Definitions of controls(parent object and table objects)
#define	kidSequenceParent		0x00030000
// logo wizard
#define	kidLogoWizard			0x00030001
// how to scenes
#define	kidHowToScenes01		0x00030002
#define	kidHowToScenes02		0x00030003
#define	kidHowToScenes03		0x00030004
#define	kidHowToScenes04		0x00030005
#define	kidHowToScenes05		0x00030006
#define	kidHowToScenes06		0x00030007
#define	kidHowToScenes07		0x00030008
#define	kidHowToScenes08		0x00030009
#define	kidHowToScenes09		0x0003000A
#define	kidHowToScenes10		0x0003000B
#define	kidHowToScenes11		0x0003000C
#define	kidHowToScenes12		0x0003000D
#define	kidHowToScenes13		0x0003000E
#define	kidHowToScenes14		0x0003000F
#define	kidHowToScenes15		0x00030010
// how to sounds
#define kidHowToSounds01		0x00030020
#define kidHowToSounds02		0x00030021
#define kidHowToSounds03		0x00030022
#define kidHowToSounds04		0x00030023
#define kidHowToSounds05		0x00030024
#define kidHowToSounds06		0x00030025
#define kidHowToSounds07		0x00030026
#define kidHowToSounds08		0x00030027
#define kidHowToSounds09		0x00030028
#define kidHowToSounds10		0x00030029
#define kidHowToSounds11		0x0003002A
#define kidHowToSounds12		0x0003002B
// how to words
#define kidHowToWords01			0x00030040
#define kidHowToWords02			0x00030041
#define kidHowToWords03			0x00030042
#define kidHowToWords04			0x00030043
#define kidHowToWords05			0x00030044
#define kidHowToWords06			0x00030045
#define kidHowToWords07			0x00030046
#define kidHowToWords08			0x00030047
#define kidHowToWords09			0x00030048
#define kidHowToWords10			0x00030049
#define kidHowToWords11			0x0003004A
#define kidHowToWords12			0x0003004B
#define kidHowToWords13			0x0003004C
#define kidHowToWords14			0x0003004D
#define kidHowToWords15			0x0003004E
// how to actors
#define kidHowToActors01		0x00030060
#define kidHowToActors02		0x00030061
#define kidHowToActors03		0x00030062
#define kidHowToActors04		0x00030063
#define kidHowToActors05		0x00030064
#define kidHowToActors06		0x00030065
#define kidHowToActors07		0x00030066
#define kidHowToActors08		0x00030067
#define kidHowToActors09		0x00030068
#define kidHowToActors10		0x00030069
#define kidHowToActors11		0x0003006A
#define kidHowToActors12		0x0003006B
#define kidHowToActors13		0x0003006C
#define kidHowToActors14		0x0003006D
#define kidHowToActors15		0x0003006E
#define kidHowToActors16		0x0003006F
#define kidHowToActors17		0x00030070
#define kidHowToActors18		0x00030071
#define kidHowToActors19		0x00030072
#define kidHowToActors20		0x00030073
#define kidHowToActors21		0x00030074
#define kidHowToActors22		0x00030075
#define kidHowToActors23		0x00030076
#define kidHowToActors24		0x00030077
#define kidHowToActors25		0x00030078
#define kidHowToActors26		0x00030079
#define kidHowToActors27		0x0003007A
#define kidHowToActors28		0x0003007B
// Project 1
#define	kid1Project01			0x00030100			
#define	kid1Project02			0x00030101			
#define	kid1Project03			0x00030102			
#define	kid1Project04			0x00030103			
#define	kid1Project05			0x00030104			
#define	kid1Project06			0x00030105			
#define	kid1Project07			0x00030106			
// Project 2
#define kid2Project01			0x00030200
#define kid2Project02			0x00030201
#define kid2Project03			0x00030202
#define kid2Project04			0x00030203
#define kid2Project05			0x00030204
#define kid2Project06			0x00030205
#define kid2Project07			0x00030206
// Project 3
#define	kid3Project01			0x00030300
#define	kid3Project02			0x00030301
#define	kid3Project03			0x00030302
#define	kid3Project04			0x00030303
#define	kid3Project05			0x00030304
// Project 4
#define kid4Project01			0x00030400
#define kid4Project02			0x00030401
#define kid4Project03			0x00030402
#define kid4Project04			0x00030403
#define kid4Project05			0x00030404
// Project 5
#define kid5Project01			0x00030500
#define kid5Project02			0x00030501
#define kid5Project03			0x00030502
#define kid5Project04			0x00030503
#define kid5Project05			0x00030504
#define kid5Project06			0x00030505
#define kid5Project07			0x00030506
#define kid5Project08			0x00030507
#define kid5Project09			0x00030508
#define kid5Project10			0x00030509
// Project 6
#define kid6Project01			0x00030510
#define kid6Project02			0x00030511
#define kid6Project03			0x00030512
#define kid6Project04			0x00030513
#define kid6Project05			0x00030514
#define kid6Project06			0x00030515
// basics
#define kidBasics01				0x00030520
#define kidBasics02				0x00030521
#define kidBasics03				0x00030522
#define kidBasics04				0x00030523
#define kidBasics05				0x00030524
#define kidBasics06				0x00030525
#define kidBasics07				0x00030526
#define kidBasics08				0x00030527
#define kidBasics09				0x00030528
// guidehelp
#define kidGuideHelp			0x000305A0

// script for playing help sounds
#define sclHelpSoundClass		999
#define ksclBldgSound           998
#define kidPlayHelpSounds		0x000305F0
#define kidPlayProjectSounds	0x000305F1
#define	kidPlayLogoSounds		0x000305F2
#define	kidPlayGuideSounds		0x000305F3
#define kidPlayToolTipSounds	0x000305F4

////////////////////////////// control ids
//#define kgobOkButton			0x00030100
#define kgobLogoWizard			0x00030600
#define kgobHowtoSequence		0x00030601
#define kgobHowToSequenceHot	0x00030602
#define kgobProject				0x00030603
#define kgobProjectHot			0x00030604
#define kgobGuideHelp			0x00030605
#define kgobGuideHelpHot		0x00030606
#define kgobGadgetText			0x00030607

/////////////////////////Filtering defines
// for projmain
#define	kFltrMouse				1	
#define	kFltrClicked			2
#define kFltrKey				4
#define kFltrBrwOk				8
#define kFltrBrwCancel			16
#define kFltrBrwSelect			32
#define	kFltrEslOk				64
#define	kFltrEslCancel			128
#define kFltrEslClosing			256
#define	kFltrActorIn			512
#define	kFltrActorOut			1024
#define kFltrActorClicked		2048
#define kFltrScnSortOk			4096
#define kFltrScnSortCancel		8192
#define kFltrAlarmOff			16384
#define kFltrMoviePlaying		32768
#define kFltrTBoxClicked		65536
#define kFltrBrwVisible			131072
#define	kFltrEslVisible			262144
#define	kFltrScnSortVisible		524288
#define kFltrGadgetAviStop		1048576
#define kFltrScnSortSelect		2097152
#define	kFltrActorClickedDown	4194304
#define	kFltrMovieLoaded		8388608
#define kFltrModalClosed	   	16777216
#define kFltrSceneLoaded		33554432
#define kFltrPortfolioResult	67108864
#define kFltrMouseBoth			3

// message id
#define cidParse				100100
#define cidHelpAlarmOff			100101
#define cidGadgetAviStop		100102

// string id's for movies
#define stidMovie1				701
#define stidMovie2				702
#define stidMovie3				703
#define stidMovie4				704
#define stidMovie5				705
#define stidMovie6				706

// position of gadget controls
#define kxpGdPlay				443
#define kypGdPlay				269
#define kxpregGdPlay			24
#define kypregGdPlay			34

#define kxpGdHow				443
#define kypGdHow				300
#define kxpregGdHow				24
#define kypregGdHow				0

#define kxpGdSelUp				419
#define kypGdSelUp				131
#define kxpregGdSelUp			0
#define kypregGdSelUp			0

#define kxpGdSelDn	 			419
#define kypGdSelDn				175
#define kxpregGdSelDn			0
#define kypregGdSelDn			0

#define kxpGdMovieNum			420			
#define kypGdMovieNum			155
#define kxpregGdMovieNum		0
#define kypregGdMovieNum		0

#define kxpGdMelVoice			273
#define kypGdMelVoice			424
#define kxpregGdMelVoice		23
#define kypregGdMelVoice		43

#define kxpGdWhy				322
#define kypGdWhy				424
#define kxpregGdWhy				23
#define kypregGdWhy				43

#define kxpGdClose				371
#define kypGdClose				424
#define kxpregGdClose			23
#define kypregGdClose			43

#define	kxpGadgetSlider			210
#define	kypGadgetSlider			234
#define	kdxpGadgetSlider		11
#define	kdypGadgetSlider		91
#define	kdypGadgetSliderTotal	98

// locker fill stuff
#define kLockerFillLeft				247
#define kLockerFillTop				229
#define kLockerFillRight			397
#define kLockerFillBottom			371
// text box stuff
#define kTextBoxFillLeft			5
#define kTextBoxFillTop				3
#define kTextBoxFillRight			167
#define kTextBoxFillBottom			43

// where to jump for each movie in projects(at start)
// conversation
#define	Proj1Scene	 	0
#define	Proj1Frame		48
// jog
#define	Proj2Scene		0
#define	Proj2Frame		113
// graveyard - surprise!
#define	Proj3Scene		0
#define	Proj3Frame		1
// mummy
#define	Proj4Scene		0
#define	Proj4Frame		1
// bongo
#define	Proj5Scene		0
#define	Proj5Frame		138
// bridge
#define	Proj6Scene		0
#define	Proj6Frame		165

// Objects for browsers, what to point at
// project 1
#define	Prj1Action1		29
#define Prj1SoundsMic1	0x0000400f
#define	Prj1Camera1		5
#define	Prj1SoundsMic2	0x00004009
#define	Prj1Camera2		4
#define	Prj1SoundsMic3	0x00004012
// project 3(2)
#define	Prj3Action1		3
#define Prj3SoundsMic1	0x0000400e
#define	Prj3Action2		32
#define	Prj3SoundsMic2	0x00004016
// project 4(6)
#define	Prj4Action1		11
#define	Prj4Prop1		0x00001011
#define	Prj4Action2		5
#define	Prj4Action3		25
// project 5
#define	Prj5Action1		27
#define	Prj5Action2		28
#define	Prj5Action3		23
#define	Prj5Camera1		2
#define	Prj5Prop1		0x00001021
#define	Prj5Action4		1
#define	Prj5SoundsEfx1	0x00003210
#define Prj5Actor1		0x00002030
#define	Prj5Action5		3
#define	Prj5Action6		1
// project 6(4)
#define Prj6SoundsMus1	0x0000505a
#define	Prj6SoundsMus2	0x0000505b
#define	Prj6SoundsMus3	0x0000505e
#define	Prj6SoundsMus4	0x00005064
#define	Prj6SoundsEfx1	0x000032a3
#define	Prj6SoundsEfx2	0x000032a2
// *maybe* #define

#endif	// !PROJMAIN_H

