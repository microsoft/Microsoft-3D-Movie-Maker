/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

// -----------------------------------------------------
// #define'd values for US / Z Version of Socrates
// -----------------------------------------------------


// -----------------------------------------------------
// the Place object descriptors
// these values are typically the 'base' of the objects
// within the place.
// -----------------------------------------------------
#include <buildgob.h>

// Shared objects -- cursors, map, portfolio.
#include <sharedef.h>

// -----------------------------------------------------
// Help balloon and control definitions
// -----------------------------------------------------
#include <balloons.chh>


#ifndef SOCDEFN_H
#define SOCDEFN_H



#define kgobInit				0x00012100		// Initialize globals
#define kgobPortfolio			0x00012200		// Portfolio object
#define kgobMovie				0x00012300		// Movie viewer object

// -----------------------------------------------------
// Definitions of objects within various places/objects
// -----------------------------------------------------

#define kgobMcZeeRoller		0x00010301
#define kgobMcZeePacker		0x00010302

#define kgobStreetMcZee 	0x00010401
#define kgobStreetMask	 	0x00010402

#define kgobTicket1Doors	0x00010501
#define kgobTicket1Waiter	0x00010502

#define kgobTicket2Exit		0x00010581
#define kgobTicket2Map		0x00010582
#define kgobTicket2McZee	0x00010583
#define kgobTicket2Cans		0x00010584
#define kgobTicket2Camera	0x00010585
#define kgobTic1m1			0x00010586
#define kgobTicketMcZeeAVI	0x00010587
#define kgobTicketMcZeeAVI2	0x00010588

#define kgobLobby1Poster1	0x00010601
#define kgobLobby1Poster2	0x00010602
#define kgobLobby1Poster3	0x00010603
#define kgobLobby1Poster4	0x00010604
#define kgobLobby1SnackSign	0x00010605
#define kgobLobby1Door 		0x00010607
#define kgobLobby1McZee		0x00010608
#define kgobLobby1m1		0x00010609
#define kgobLobbyAVI		0x0001060a

#define kgobLobby2Doors		0x00010703
#define kgobLobby2Exit		0x00010704
#define kgobLobby2Map		0x00010705
#define kgobLobby2m1		0x00010706
#define kgobLobby2m2		0x00010707
#define kgobLobby2m3		0x00010708

#define	kgobSnackbarMczee	0x00010806
#define kgobLobby3m1		0x00010807
#define kgobLobby3m2		0x00010808

#define kgobWaitingCouch	0x00010901
#define kgobWaitingPicture	0x00010902

#define kgobInsp1McZee		0x00011401
#define kgobSplotMachine	0x00011402
#define kgobTalentBook		0x00011403
#define kgobIdea1m1			0x00011404

#define	kgobIdea2m1			0x00011405

#define	kgobIdea3m1			0x00011406
#define	kgobIdea3m2			0x00011407
#define	kgobIdea3m3			0x00011408
#define	kgobIdea3m4			0x00011409

#define	kgobIdea4m1			0x0001140a
#define	kgobIdea4m2			0x0001140b
#define	kgobIdea4m3			0x0001140c

#define	kgobIdeaMcZeeAVI1 	0x0001140d
#define	kgobIdeaMcZeeAVI2 	0x0001140e
#define	kgobIdeaMcZeeAVI3 	0x0001140f


#define kgobStudio1Tools	0x00011801
#define kgobStudio1McZee	0x00011802
#define kgobToolsIntroAVI   0x00011803
#define kgobStudio1TV		0x00011804
#define kgobStdo1m1			0x00011805
	
#define kgobStudio2Doors	0x00011901
#define kgobStudio2Exit 	0x00011902
#define kgobStudio2Map	 	0x00011903
#define kgobStudio2ExitWord	0x00011904

#define	kgobStdo4m1			0x00011905
#define	kgobStdo4m2			0x00011906
#define	kgobStdo4m3			0x00011907
#define	kgobStdo4m4			0x00011908

#define	kgobStdo3m1			0x00011909
#define	kgobStdo3m2			0x0001190a
#define	kgobStdo3m3			0x0001190b

#define	kgobStdo2m1			0x0001190c
#define	kgobStdo2m2			0x0001190d
#define	kgobStdo2m3			0x0001190e
#define	kgobStdo2m4			0x0001190f


#define kgobTheatre1LeftDoor 	0x00010c01
#define kgobTheatre1RightDoor 	0x00010c02
#define kgobTheatre1McZee		0x00010c03
#define kgobPlayButton			0x00010c04
#define kgobStopButton			0x00010c05
#define kgobRewindButton		0x00010c06
#define kgobVolumeButton		0x00010c07
#define kgobCountDn				0x00010c08
#define kgobMovieClickFilter	0x00010c09
#define kgobTheaterVolume		0x00010c0a

#define kgobTheatre2Door		0x00010d01
#define kgobTheatre2Exit		0x00010d02
#define kgobTheatre2Map			0x00010d03
#define kgobTheater2m1			0x00010d04
#define kgobTheater2m2			0x00010d05
#define kgobTheater2m3			0x00010d06

#define kgobTheatre3Right		0x00010e01
#define kgobTheatre3Door		0x00010e02
#define kgobTheatre3Exit		0x00010e03
#define kgobTheatre3Map			0x00010e04

#define kgobTheatre4Left		0x00010f01
#define kgobTheatre4Door		0x00010f02
#define kgobTheatre4Exit		0x00010f03
#define kgobTheatre4Map			0x00010f04
#define kgobTheatre4McZee		0x00010f05
#define kgobThe4OpenDoor		0x00010f06

#define kgobBackstage1Insp		0x00011001
#define kgobBackstage1Studio	0x00011002
#define kgobBackstage1Proj		0x00011003
#define kgobIdeaDoor			0x00011004
#define kgobBackstage1Bag		0x00011005
#define kgobProjectDoor			0x00011006
#define kgobStudioDoor			0x00011007
#define kgobBackstage1Shadow	0x00011008
#define kgobBackstageMcZeeAVI	0x00011009
#define kgobBackstageMcZee		0x0001100a
#define kgobBackstageMcZeeAVI1	0x0001100b
#define kgobBackstageMcZeeAVI2	0x0001100c
#define kgobBackstageMcZeeAVI3	0x0001100d
#define kgobBackstageMcZeeAVI4	0x0001100e
#define kgobBackstageMcZeeAVI5	0x0001100f
#define kgobBackstageMcZeeAVI6	0x00011010


#define kgobBackstage2Left		0x00011103
#define kgobBackstage2Right		0x00011104
#define kgobBackstage2Exit		0x00011105
#define kgobBackstage2Map		0x00011106
#define kgobBackstage2m1		0x00011107
#define kgobBackstage2m2		0x00011108


#define kgobInsp2Doors			0x00011501
#define kgobInsp2Exit			0x00011502
#define kgobInsp2Map			0x00011503

#define kgobProj1Melanie		0x00011b01
#define kgobProj1Mon1			0x00011b02
#define kgobProj1Mon2			0x00011b03
#define kgobProj1Cart			0x00011b04
#define kgobFlyingLogos			0x00011b05
#define kgobMovieTech			0x00011b06
#define kgobPrjt1m1				0x00011b07
#define kgobMelAVI				0x00011b08

#define kgobProj2Doors			0x00011c01
#define kgobProj2Exit			0x00011c02
#define kgobProj2Map			0x00011c03

#define	kgobPrjt2m1				0x00011c04
#define	kgobPrjt2m2				0x00011c05
#define	kgobPrjt2m3				0x00011c06
#define	kgobPrjt2m4				0x00011c07
#define	kgobPrjt2m5				0x00011c08

#define	kgobPrjt4m1				0x00011c09
#define	kgobPrjt4m2				0x00011c0a
#define	kgobPrjt4m3				0x00011c0b
#define	kgobPrjt4m4				0x00011c0c

#define kgobTvViewL				0x00011c0d
#define kgobTvViewR				0x00011c0e

// Duplicate definition in shared\map.chh
#define kgobMapHere			0x00012003

#define kgobTalentBookObj       0x00012204
#define kgobSplotMachineObj     0x00012205
#define	kgobBSplot1             0x00012206
#define	kgobBSplot2             0x00012207
#define	kgobBSplot3             0x00012208
#define	kgobBSplot4             0x00012209
#define	kgobBSplot5             0x0001220a
#define	kgobBSplot6             0x0001220b
#define	kgobBSplot7             0x0001220c
#define kgobSplotView           0x0001220d
#define	kgobSplotGlass			0x0001220e

// -----------------------------------------------------
// Common object definitions
// -----------------------------------------------------

#define kgobPortfolio		0x00010101
#define kgobPortIcon		0x00010102
#define kgobCarpetBag		0x00010105
#define kgobVolume			0x00010106
#define kgobSwitcher		0x00010107
#define kgobNavbarLeft		0x00010108
#define kgobNavbarRight		0x00010109
#define kgobNavbarDown		0x0001010a

// -----------------------------------------------------
// Common object bit flag definitions (for Util)
// -----------------------------------------------------

#define kflgPortfolio		0x00000001
#define kflgPortIcon		0x00000002
#define kflgMap				0x00000004
#define kflgCarpetBag		0x00000020
#define kflgVolume			0x00000040
#define kflgSwitcher		0x00000080
#define kflgNavbarLeft		0x00000100
#define kflgNavbarRight		0x00000200
#define kflgNavbarDown		0x00000400


// -----------------------------------------------------
// Palette Definition
// -----------------------------------------------------
#define kpalSocBase			0x00010000
#define kpalImaginopolis	0x00010001
#define kpalSocTicket		0x00010002
#define kpalSocLobby		0x00010003
#define kpalSocTheatre		0x00010004
#define kpalSocBackstage	0x00010005					 
#define kpalSocInspiration	0x00010006
#define kpalSocStudio		0x00010007
#define kpalSocProjects		0x00010008
#define kpalSocStreet		0x00010009

// -----------------------------------------------------
// Sound definitions?  Midi definitions?
// -----------------------------------------------------

#define kwavSplash			0x00010001
#define kwavBingo			0x00010003
//							0x00010010 Reserved for sound 									
//	 						0x00010030 definations in sharutil.chh.

// -----------------------------------------------------
// help TOPIC definitions
// -----------------------------------------------------

#define ktpcLoginGetName	0x00018540
#define ktpcLoginInst		0x00018541
#define ktpcLoginChoice		0x00018542

#define ktpcQuitAlert		0x00018543
#define ktpcYouHere			0x00018547
#define ktpcTicketChoices	0x00018548

#define ktpcSplashCredits	0x0001854e
#define ktpcHelpAbout		0x0001854f
#define ktpcInit			0x00018550

#define ktpcLobbyTip		0x00018551
								  
#define ktpcTheatre1		0x00018552
#define ktpcTheatre2		0x00018553

#define ktpcBackstage		0x00018555
								   
#define ktpcProject1		0x00018556

#define ktpcIdeas			0x00018557
#define ktpcSplotIdea		0x00018558
#define ktpcSplotIdea2		0x00018559
#define ktpcStudio1			0x0001855a

// voice over help topics + waves
#define ktpcVO04AA				0x00018000
#define ktpcVO5					0x00018001
#define ktpcVO6					0x00018002
#define ktpcVO6A				0x00018003
#define ktpcVO6B				0x00018004
#define ktpcVO6BB				0x00018005
#define ktpcVO6C				0x00018006
#define ktpcVO8					0x00018007
#define ktpcVO10				0x00018008
#define ktpcVO11C				0x00018009
#define ktpcVO11CC				0x0001800a
#define ktpcVO11CCC				0x0001800b
#define ktpcVO01A				0x0001800c
#define ktpcVO16				0x0001800d
#define ktpcVO17				0x0001800e
#define ktpcVO28A				0x0001800f
#define ktpcVO28B				0x00018010
#define ktpcVO28				0x00018011
#define ktpcVO31				0x00018012
#define ktpcVO32				0x00018013
#define ktpcVO32B				0x00018014
#define ktpc04_logn3			0x00018015
#define ktpcVO36				0x00018016
#define ktpcVO37				0x00018017
#define ktpcVO38				0x00018018
#define ktpcVO39				0x00018019
#define ktpcVO40				0x0001801a
#define ktpcVO41				0x0001801b
#define ktpcVO43				0x0001801c
#define ktpcTBCY2				0x0001801e
#define ktpcTBTH3				0x0001801f
#define ktpcTKTBTH				0x00018020
#define ktpcVO54				0x00018021
#define ktpcVO56B				0x00018022
#define ktpcVO57A				0x00018023
#define ktpcVO58				0x00018024
#define ktpcVO59				0x00018025
#define ktpcVO60				0x00018026
#define ktpc21_thr1a			0x00018027
#define ktpcVO71A				0x00018028
#define ktpcVO74B				0x00018029
#define ktpcVO76				0x0001802a
#define ktpcVO75D				0x0001802b
#define ktpcVO79				0x0001802c
#define ktpcVO80AA				0x0001802d
#define ktpcVO80A				0x0001802e
#define ktpcVO80B				0x0001802f
#define ktpcVO83				0x00018030
#define ktpcVO86				0x00018031
#define ktpcVO91				0x00018032
#define ktpcVO93				0x00018033
#define ktpcVO93A				0x00018034
#define ktpcVO94				0x00018035
#define ktpcVO57				0x00018037
#define ktpcStd1B				0x00018038

#define ktpcVO57AA				0x0001803a

#define ktpcStd1A				0x0001803b	// These three MUST be consecutive!
#define ktpcVO97A				0x0001803c	// Used in Studio1 for Random McZee
#define ktpcVO97B				0x0001803d	// wait state.

#define ktpcVO53				0x00018040
#define ktpc30_thr4				0x00018041
#define ktpcVO75				0x00018043

#define ktpcPrjt2A 				0x00018044
#define ktpcVO40A				0x00018048
#define ktpcVO40E				0x00018049

#define kwavVO04AA				0x00018000
#define kwavVO5					0x00018001
#define kwavVO6					0x00018002
#define kwavVO6A				0x00018003
#define kwavVO6B				0x00018004
#define kwavVO6BB				0x00018005
#define kwavVO6C				0x00018006
#define kwavVO8					0x00018007
#define kwavVO10				0x00018008
#define kwavVO11C				0x00018009
#define kwavVO11CC				0x0001800a
#define kwavVO11CCC				0x0001800b
#define kwavVO01A				0x0001800c
#define kwavVO16				0x0001800d
#define kwavVO17				0x0001800e
#define kwavVO28A				0x0001800f
#define kwavVO28B				0x00018010
#define kwavVO28				0x00018011
#define kwavVO31				0x00018012
#define kwavVO32				0x00018013
#define kwavVO32B				0x00018014
#define kwav04_logn3			0x00018015
#define kwavVO37				0x00018017
#define kwavVO38				0x00018018
#define kwavVO39				0x00018019
#define kwavVO40				0x0001801a
#define kwavVO41				0x0001801b
#define kwavVO43				0x0001801c
#define kwavTBCY2				0x0001801d
#define kwavTBTH3				0x0001801f
#define kwavTKTBTH				0x00018020
#define kwavVO54				0x00018021
#define kwavVO56B				0x00018022
#define kwavVO57A				0x00018023
#define kwavVO58				0x00018024
#define kwavVO59				0x00018025
#define kwavVO60				0x00018026
#define kwav21_thr1a			0x00018027
#define kwavVO71A				0x00018028
#define kwavVO74B				0x00018029
#define kwavVO76				0x0001802a
#define kwavVO75D				0x0001802b
#define kwavVO79				0x0001802c
#define kwavVO80AA				0x0001802d
#define kwavVO80A				0x0001802e
#define kwavVO80B				0x0001802f
#define kwavVO83				0x00018030
#define kwavVO86				0x00018031
#define kwavIdea1a				0x00018032
#define kwavIdea1b				0x00018033
#define kwavIdea2				0x00018034
#define kwavIdea3				0x00018035
#define kwavIdea4				0x00018036
#define kwavVO94B				0x00018037
#define kwavVO57				0x00018039									 
#define kwavStd1B				0x0001803a
#define kwavVO57AA				0x0001803c

#define kwavVO292A				0x00018100
#define kwavVO86A				0x00018101
#define kwavVO86B				0x00018102

#define kwavStd1A				0x00018103	// These three MUST be consecutive!
#define kwavVO97A				0x00018104	// Used in Studio1 for Random McZee
#define kwavVO97B				0x00018105	// wait state.

#define kwavVO53				0x00018106
#define kwav30_thr4				0x00018107
#define kwavVO75				0x00018108
#define kwavPrjt1 				0x00018109
#define kwavPrjt2A 				0x0001810a
#define kwav21_thr1b			0x0001810d
#define kwavStudioIntro			0x0001810e
#define kwavVO54				0x0001810f
#define kwavRollerCoaster		0x00018110
#define	kwavTicketFar			0x00018111
#define kwavPrjtMonitor			0x00018112
#define	kwavStudioTVPop			0x00018113
#define kwavBioCancel 			0x00018114
#define kwavBioPopup 			0x00018115
// music definitions

//-----------------------------------------------------
// Shared MIDI music definitions
//-----------------------------------------------------

#define	 kmidBackstage		0x00010011
#define	 kmidBoothjam		0x00010012
#define	 kmidCloset			0x00010013
#define	 kmidCoaster		0x00010014
#define	 kmidIdeas			0x00010015
#define	 kmidJamloop		0x00010016
#define	 kmidLobby			0x00010017
#define	 kmidProject		0x00010018
#define	 kmidSnack			0x00010019
#define	 kmidStudio			0x0001001a
#define	 kmidTag			0x0001001b
#define	 kmidTheatre		0x0001001c

#define  kmidRollerCoaster  0x0001001d
#endif


