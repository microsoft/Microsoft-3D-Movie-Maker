/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Frame #defines that might get used by a source file for a tool, such
	as source files for the chunky compiler.  This file should only contain
	#defines, and the values for the #defines should be constant values
	(no arithmetic).

***************************************************************************/
#ifndef FRAMEDEF_H
#define FRAMEDEF_H

#ifdef MAC
#define MacWin(mac,win) mac
#define Mac(foo) foo
#define Win(foo)
#elif defined(WIN)
#define MacWin(mac,win) win
#define Mac(foo)
#define Win(foo) foo
#endif //WIN


/***************************************************************************
	Miscellaneous defines
***************************************************************************/
#define kdzpInch 72
#define klwSigPackedFile 'kapa'	
#define klwSigUnpackedFile 'kaup'
#define stidNil 0xFFFFFFFF			// nil string id
#define kdtimSecond 60


/***************************************************************************
	Compression formats.
***************************************************************************/
#define cfmtNil			0
#define kcfmtKauai		'KCDC'
#define kcfmtKauai2		'KCD2'


/***************************************************************************
	For flushing events.
***************************************************************************/
#define fevtNil			0x00000000
#define fevtMouse		0x00000001
#define fevtKey			0x00000002
#define kgrfevtAll		0xFFFFFFFF


/***************************************************************************
	Sound manager constants.
***************************************************************************/
#define sclNil 0xFFFFFFFF	// nil sound class
#define sqnNil 0xFFFFFFFF	// nil queue (for wild card)
#define ksqnNone 0			// non-queued sound
#define kvlmFull 0x00010000	// normal volume level


/***************************************************************************
	Cursor state constants.
		bits 0-7 are reserved by GUI Kauai.
		bits 8-15 are reserved by Kidspace Kauai.
		bits 16-31 are reserved for application use.
***************************************************************************/
#define fcustNil		0
#define fcustCmd		1
#define fcustShift		2
#define fcustOption		4
#define kgrfcustKeys	7

#define fcustMouse		8
#define kgrfcustUser	15

#define kgrfcustFrame	0x000000FF
#define kgrfcustKid		0x0000FF00
#define kgrfcustApp		0xFFFF0000


/***************************************************************************
	Property id's.  For APPB::FSetProp and APPB::FGetProp.
***************************************************************************/
#define kpridMaximized		1
#define kpridFullScreen		2
#define kpridToolTipDelay	3


/***************************************************************************
	Transitions that Kauai knows how to do.
***************************************************************************/
#define gftNil				0
#define kgftWipe			1
#define kgftSlide			2
#define kgftDissolve		3
#define kgftFade			4
#define kgftIris			5

// transition directions for Wipe and Slide
#define kgfdLeft			0x00 // 0000
#define kgfdRight			0x05 // 0101
#define kgfdUp				0x0A // 1010
#define kgfdDown			0x0F // 1111
#define kgfdLeftRight		0x04 // 0100
#define kgfdRightLeft		0x01 // 0001
#define kgfdUpDown			0x0E // 1110
#define kgfdDownUp			0x0B // 1011

// transition directions for Iris
#define kgfdOpen			0x00 // 00
#define kgfdClose			0x03 // 11
#define kgfdCloseOpen		0x01 // 01
#define kgfdOpenClose		0x02 // 10


/***************************************************************************
	Standard command handler IDs.
		GUI Kauai reserves values below 10000.
		Kidspace Kauai reserves values below 20000.
		Values >= 20000 can be used by the application.
***************************************************************************/
//The framework reserves values below khidLimFrame
#define hidNil				0
#define khidApp				1
#define khidScreen			2
#define khidMdi				3	//generic mdi windows should get this
#define khidSizeBox			4	//all size boxes (WSBs) get this
#define khidDialog			5	//for modal dialogs
#define khidHScroll			6	//standard horizontal scroll bar
#define khidVScroll			7	//standard vertical scroll bar
#define khidDoc				8	//standard hid for a document
#define khidDsg				9	//standard dsg (child of dmw)
#define khidDdg				10	//standard ddg (child of dsg)
#define khidDsspHorz		11	//standard horizontal document window split box
#define khidDsspVert		12	//standard vertical document window split box
#define khidDssm			13	//standard split mover
#define khidDmw				14	//convenient for single DMW window
#define khidDmd				15	//standard dmd
#define khidEdit			16	//edit control
#define khidToolTip			17	//tool tip

#define khidLimFrame		10000



/***************************************************************************
	Chunky file constants.
***************************************************************************/
//convenient to indicate none (chunk places no restrictions on these)
#define cnoNil 0xFFFFFFFF
#define ctgNil 0
#define chidNil 0xFFFFFFFF

#define kctgFramework	'FRAM' //for chunky file creator

#define kctgMacro		'GLCM'
#define kctgGl			'GL  '
#define kctgAl			'AL  '
#define kctgGg			'GG  '
#define kctgAg			'AG  '
#define kctgGst			'GST '
#define kctgAst			'AST '
#define kctgRsc			'RSC '
#define kctgMeta		'META'
#define kctgMacPict		'MPIC'
#define kctgPictNative	MacWin(kctgMacPict,kctgMeta)
#define kctgGraphic		'GRAF'
#define kctgScript		'GLOP'
#define kctgMask		'MASK'
#define kctgMbmp		'MBMP'
#define kctgMidi		'MIDS'
#define kctgWave		'WAVE'
#define kctgChkCmp		'CHMP'
#define kctgColorTable	'GLCR'
#define kctgCursor		'GGCR'
#define kctgText		'TEXT'
#define kctgTxtProps	'GLMP'
#define kctgTxtPropArgs	'AGPA'
#define kctgRichText	'RTXT'
#define kctgScriptStrs	'GSTX'
#define kctgStringReg	'GSTR'


/***************************************************************************
	Command IDs

	Commands above 64K cannot be put on Win menus.

	Commands between 40000 and 50000 should be reserved for AppStudio
	defined values (these are defined in .h files generated by AppStudio).

	Commands between 50000 and 65535 are reserved for Windows menu list
	handling.

	Commands between cidMinNoRepeat and cidLimNoRepeat will be recorded
	only once when multiple instances of the command occur consecutively.

	Commands between cidMinNoRecord and cidLimNoRecord will not be recorded
	at all.
***************************************************************************/

//id of main key accelerator table (if one is used)
#define acidMain			128

#define cidNil				0

//Windows MDI reserves ids 1-10, so start at 100

// command IDs
#define wcidMinApp			100
#define cidNew				100
#define cidOpen				101
#define cidClose			102
#define cidSave				103
#define cidSaveAs			104
#define cidSaveCopy			105	// save a copy of the doc
#define cidQuit				106
#define cidAbout			107
#define cidNewWnd			108
#define cidCloseWnd			109	// just the current window, not the whole doc
#define cidChooseWnd		110	// a dynamic list
#define cidOpenDA			111	// (Mac only) a desk accessory list
#define cidChooseFont		112	// a font menu list
#define cidCexStopPlay		113	// stop playing a command stream
#define cidCexStopRec		114	// stop recording a command stream
#define cidCexPlayDone		115	// notify the world that play stopped
#define cidCexRecordDone	116	// notify the world that record stopped
#define cidSaveAndClose		117
#define cidCut				118
#define cidCopy				119
#define cidPaste			120
#define cidClear			121
#define cidShowClipboard	122
#define cidJustifyLeft		123
#define cidJustifyCenter	124
#define cidJustifyRight		125
#define cidIndentNone		126
#define cidIndentFirst		127
#define cidIndentRest		128
#define cidIndentAll		129
#define cidBold				130
#define cidItalic			131
#define cidUnderline		132
#define cidUndo				133
#define cidRedo				134
#define cidChooseFontSize	135	// a dynamic list
#define cidPlain			136
#define cidChooseSubSuper	137 // a dynamic list
#define cidPrint			138
#define cidPrintSetup		139
#define cidPasteSpecial		140


#define wcidListBase		50000	// for windows menu list handling
#define dwcidList			500		// increment between list base values
#define wcidLimApp			0xF000	// windows reserves larger values


/****************************************
	non-menu, non-key invoked commands
	These commands cannot be put on
	menus
****************************************/
#define cidMinNoMenu		100000
#define cidDoScroll			100000
#define cidEndScroll		100001
#define cidSplitDsg			100002
#define cidKey				100003
#define cidBadKey			100004
#define cidAlarm			100005
#define cidActivateSel		100006
#define cidMouseDown		100007
#define cidClicked			100008
#define cidEndModal			100009


/****************************************
	no-repeat commands:
	when recording, multiple (identical)
	instances of these are recorded only
	once.
****************************************/
#define cidMinNoRepeat		200000
#define cidTrackMouse		200000

#define cidLimNoRepeat		400000


/****************************************
	no-record commands:
	when recording, don't record
	these at all.
****************************************/
#define cidMinNoRecord		400000
#define cidIdle				400000
#define cidSelIdle			400001	// idle for settting/clearing selection.
#define cidMouseMove		400002	// mouse moved
#define cidRollOff			400003	// mouse rolled off the GOB

#define cidLimNoRecord		600000


/***************************************************************************
	Error codes
***************************************************************************/

/****************************************
	0 - 9999: Util-issued error codes
****************************************/

// 00000 - 00099: low-memory errors
#define ercNil						0xFFFFFFFF
#define ercOomHq					0
#define ercOomPv					1
#define ercOomNew					2

// 00100 - 00199: file errors
#define ercFileGeneral				100 //enum files and check el's
#define ercFilePerm					101 //can't set write permissions
#define ercFileOpen					102 //can't open a file
#define ercFileCreate				103 //can't create a file
#define ercFileSwapNames			104 //FSwapNames failed
#define ercFileRename				105 //FRename failed
#define ercStnRead					106 //reading a string failed

// 00200 - 00299: fni errors
#define ercFniGeneral				200 //couldn't build an fni
#define ercFniDelete				201 //delete failed
#define ercFniRename				202 //rename failed
#define ercFniMismatch				203 //requested dir is a file or file is dir
#define ercFniHidden				204 //requested file/dir is hidden or alias
#define ercFniDirCreate				205 //can't create directory

// 00300 - 00399: fne errors
#define ercFneGeneral				300

// 00400 - 00499: chunk errors
#define ercCflOpen					400
#define ercCflCreate				401
#define ercCflSave					402
#define ercCflSaveCopy				403

// 00500 - 00599: crf errors
#define ercCrfCantLoad				500

// 00600 - 00699: sound manager
#define ercSndmCantInit				600
#define ercSndmPartialInit			601
#define ercSndamWaveDeviceBusy		602
#define ercSndMidiDeviceBusy		603


/*******************************************
	10000 - 19999: Frame-issued error codes
*******************************************/

// 10000 - 10099: gdi errors
#define ercOomGdi					10000

// 10100 - 10199: gfx errors
#define ercGfxCantDraw				10100
#define ercGfxCantSetFont			10101
#define ercGfxNoFontList			10102
#define ercGfxCantSetPalette		10103

// 10200 - 10299: dlg errors
#define ercDlgCantGetArgs			10200
#define ercDlgCantFind				10201
#define ercDlgOom					10202

// 10300 - 10399: rtxd errors
#define ercCantSave					10300
#define ercRtxdTooMuchText			10301
#define ercRtxdReadFailed			10302
#define ercRtxdSaveFailed			10303

// 11000 - 11999: misc errors
#define ercCantOpenVideo			11000
#define ercMbmpCantOpenBitmap		11001
#define ercSpellNoDll				11002
#define ercSpellNoDict				11003
#define ercSpellNoUserDict			11004


/***************************************************************************
	Custom window messages for testing
	WARNING: because of Chicago stupidity, these have to be less than 64K.
	Chicago truncates to 16 bits! Long live NT!
***************************************************************************/
#define WM_GOB_STATE		0x00004000
#define WM_GOB_LOCATION		0x00004001
#define WM_GLOBAL_STATE		0x00004002
#define WM_CURRENT_CURSOR	0x00004003
#define WM_GET_PROP			0x00004004
#define WM_SCALE_TIME		0x00004005
#define WM_GOB_FROM_PT		0x00004006
#define WM_FIRST_CHILD		0x00004007
#define WM_NEXT_SIB			0x00004008
#define WM_PARENT			0x00004009
#define WM_GOB_TYPE			0x0000400A
#define WM_IS_GOB			0x0000400B

#endif //!FRAMEDEF_H

