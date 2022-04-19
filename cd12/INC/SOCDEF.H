/***************************************************************************

	Socrates #defines that might get used by a source file for a tool, such
	as source files for the chunky compiler.  This file should only contain
	#defines, and the values for the #defines should be constant values
	(no arithmetic).

***************************************************************************/
#ifndef SOCDEF_H
#define SOCDEF_H

#define BUG1866
#define BUG1870
#define BUG1888
#define BUG1899
#define BUG1906
#define BUG1907
#define BUG1929
#define BUG1932
#define BUG1959
#define BUG1960
#define BUG1961
#define BUG1973

#define kfps 6 				  // frames per second for playback and recording


/***************************************************************************
	Error codes
***************************************************************************/

/****************************************************
	100000 - 109999: Movie-engine-issued error codes
****************************************************/

// 100000 - 100099: general movie engine errors
#define ercSocSaveFailure         100000
#define ercSocSceneSwitch         100002
#define ercSocSceneChop           100003
#define ercSocBadFile             100004
#define ercSocNoTboxSelected      100005
#define ercSocNoActrSelected      100006
#define ercSocNotUndoable         100007
#define ercSocNoScene             100008
#define ercSocBadVersion          100009
#define ercSocNothingToPaste      100010
#define ercSocBadFrameSlider      100011
#define ercSocGotoFrameFailure    100012
#define ercSocDeleteBackFailure   100013
#define ercSocActionNotApplicable 100014
#define ercSocCannotPasteThatHere 100015
#define ercSocNoModlForChar       100016
#define ercSocNameTooLong         100017
#define ercSocTboxTooSmall        100018
#define ercSocNoThumbnails        100019
#define ercSocTdtTooLong          100020
#define ercSocBadTdf              100021
#define ercSocNoActrMidi          100022
#define ercSocNoImportRollCall    100023
#define ercSocNoNukeRollCall      100024
#define ercSocSceneSortError      100025
#define ercSocCantInitSceneSort   100026
#define ercSocCantInitSplot       100027
#define ercSocNoWaveIn            100028
#define ercSocWaveInProblems      100029
#define ercSocPortfolioFailed     100030
#define ercSocCantInitStudio      100031
#define ercSoc3DWordCreate        100032
#define ercSoc3DWordChange        100033
#define ercSocWaveSaveFailure     100034
#define ercSocNoSoundName         100035
#define ercSocNoKidSndsInMovie    100036
#define ercSocCreatedUserDir      100037
#define ercSocMissingMelanieDoc   100038
#define ercSocCantLoadMelanieDoc  100039
#define ercSocBadSceneSound       100040
#define ercSocBadSoundFile        100041
#define ercSocNoDefaultFont       100042
#define ercSocCantCacheTag        100043
#define ercSocInvalidFilename     100044
#define ercSocNoSndOnPaste        100045
#define ercSocCantCopyMsnd        100046
// *If you add anything below this line you need to notify leannp about it so she can make a help topic for it*



/***************************************************************************
	String IDs
***************************************************************************/

// For kcnoGstApp:
// REVIEW: should these (and other instances of string IDs) be kids...? */
#define idsNil                     (-1L)
#define idsWindowTitle             0
#define idsProductLong             1
#define idsProductShort            2
#define idsDefaultUser             3
#define idsEngineCopyOf            4
#define idsEngineDefaultTitle      5
#define idsDefaultFont             6
#define idsUsersDir                12
#define idsWNetError               13
#define idsDefaultDypFont          14
#define idsPortfSaveMovieTitle	   15
#define idsPortfOpenMovieTitle	   16
#define idsPortfOpenSoundTitle	   17
#define idsPortfOpenTextureTitle   18
#define idsPortfMovieFilterLabel   19
#define idsPortfMovieFilterExt	   20
#define idsPortfSoundFilterLabel   21
#define idsPortfSoundMidiFilterExt 22
#define idsPortfSoundWaveFilterExt 23
#define idsPortfTextureFilterLabel 24
#define idsPortfTextureFilterExt   25
#define idsMelanie                 26
#define idsOOM	                   27
#define idsExitStudio              28
#define idsSaveChangesBkp          29
#define idsConfirmExitBkp          30
#define idsDeleteSound             31
#define idsReplaceFile             32
#define idsPurgeSounds             33

/***************************************************************************
	Chunk numbers
***************************************************************************/

#define kcnoGstTitles             0
#define kcnoGstError              1
#define kcnoGstMisc	              2
#define kcnoGstAction             3
#define kcnoGstStudioFiles        4
#define kcnoGstBuildingFiles      5
#define kcnoGstApp                6
#define kcnoGstSharedFiles        7

#define kcnoGlcrInit              0

#define kcnoMbmpSplash            0
#define kcnoMbmpPortBackOpen      1
#define kcnoMbmpPortBtnOk         2
#define kcnoMbmpPortBtnOkSel      3
#define kcnoMbmpPortBtnCancel     4
#define kcnoMbmpPortBtnCancelSel  5
#define kcnoMbmpHomeLogo          6
#define kcnoMbmpPortBtnHome       7
#define kcnoMbmpPortBtnHomeSel    8
#define kcnoMbmpPortBackSave      9

#define kcnoMidiSplash            0

/***************************************************************************
	Commands
***************************************************************************/

#define cidCopyRoute                    44000
#define cidCopyTool						44001
#define cidCutTool						44002
#define cidPasteTool					44003
#define WM_QUERY_EXISTS					44004
#define WM_QUERY_LOCATION				44005
#define	WM_SET_MOVIE_POS				44006
#define cidRender						44007
#define cidActorPlaced                  44008
#define cidActorPlacedOutOfView         44009
#define cidActorClicked                 44010
#define cidMviePlaying                  44011
#define cidTboxClicked                  44012
#define cidMovieGoto					44013
#define cidShiftCut						44014
#define cidShiftCopy					44015
#define cidActorClickedDown             44016
#define cidDeactivate		            44017
#define cidSceneLoaded		            44018
#endif // SOCDEF_H

