/***************************************************************************
	Author: ShonK
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Script compiler for the gob based scripts.

***************************************************************************/
#ifndef SCRCOMG_H
#define SCRCOMG_H


//if you change this enum, bump the version numbers below
enum
	{
	kopCreateChildGob = 0x1000,
	kopCreateChildThis,
	kopDestroyGob,
	kopDestroyThis,
	kopResizeGob,
	kopResizeThis,
	kopMoveRelGob,
	kopMoveRelThis,
	kopMoveAbsGob,
	kopMoveAbsThis,
	kopGidThis,
	kopGidParGob,
	kopGidParThis,
	kopGidNextSib,
	kopGidPrevSib,
	kopGidChild,
	kopFGobExists,
	kopCreateClock,
	kopDestroyClock,
	kopStartClock,
	kopStopClock,
	kopTimeCur,
	kopSetAlarm,
	kopEnqueueCid,
	kopAlert,
	kopRunScriptGob,
	kopRunScriptThis,
	kopStateGob,
	kopStateThis,
	kopChangeStateGob,
	kopChangeStateThis,
	kopAnimateGob,
	kopAnimateThis,
	kopSetPictureGob,
	kopSetPictureThis,
	kopSetRepGob,
	kopSetRepThis,
	kopUNUSED100,
	kopUNUSED101,
	kopRunScriptCnoGob,
	kopRunScriptCnoThis,
	kopXMouseGob,
	kopXMouseThis,
	kopYMouseGob,
	kopYMouseThis,
	kopGidUnique,
	kopXGob,
	kopXThis,
	kopYGob,
	kopYThis,
	kopZGob,
	kopZThis,
	kopSetZGob,
	kopSetZThis,
	kopSetColorTable,
	kopCell,
	kopCellNoPause,
	kopGetModifierState,
	kopChangeModifierState,
	kopCreateHelpGob,
	kopCreateHelpThis,
	kopTransition,
	kopGetEdit,
	kopSetEdit,
	kopAlertStr,
	kopGetProp,
	kopSetProp,
	kopLaunch,
	kopPlayGob,
	kopPlayThis,
	kopPlayingGob,
	kopPlayingThis,
	kopStopGob,
	kopStopThis,
	kopCurFrameGob,
	kopCurFrameThis,
	kopCountFramesGob,
	kopCountFramesThis,
	kopGotoFrameGob,
	kopGotoFrameThis,
	kopFilterCmdsGob,
	kopFilterCmdsThis,
	kopDestroyChildrenGob,
	kopDestroyChildrenThis,
	kopPlaySoundThis,
	kopPlaySoundGob,
	kopStopSound,
	kopStopSoundClass,
	kopPlayingSound,
	kopPlayingSoundClass,
	kopPauseSound,
	kopPauseSoundClass,
	kopResumeSound,
	kopResumeSoundClass,
	kopPlayMouseSoundThis,
	kopPlayMouseSoundGob,
	kopWidthGob,
	kopWidthThis,
	kopHeightGob,
	kopHeightThis,
	kopSetNoSlipGob,
	kopSetNoSlipThis,
	kopFIsDescendent,
	kopPrint,
	kopPrintStr,
	kopSetMasterVolume,
	kopGetMasterVolume,
	kopStartLongOp,
	kopEndLongOp,
	kopSetToolTipSourceGob,
	kopSetToolTipSourceThis,
	kopSetAlarmGob,
	kopSetAlarmThis,
	kopModalHelp,
	kopFlushUserEvents,
	kopStreamGob,
	kopStreamThis,
	kopPrintStat,
	kopPrintStrStat,

	kopLimSccg
	};


const short kswCurSccg =  0x101D;	//this version
const short kswBackSccg = 0x101D;	//we can be read back to this version
const short kswMinSccg =  0x1015;	//we can read back to this version


/****************************************
	Gob based script compiler
****************************************/
typedef class SCCG *PSCCG;
#define SCCG_PAR SCCB
#define kclsSCCG 'SCCG'
class SCCG : public SCCG_PAR
	{
	RTCLASS_DEC

protected:
	virtual short _SwCur(void);
	virtual short _SwBack(void);
	virtual short _SwMin(void);

	virtual long _OpFromStn(PSTN pstn);
	virtual bool _FGetOpFromName(PSTN pstn, long *pop, long *pclwFixed,
		long *pclwVar, long *pcactMinVar, bool *pfVoid);
	virtual bool _FGetStnFromOp(long op, PSTN pstn);
	};


#endif //!SCRCOMG_H

