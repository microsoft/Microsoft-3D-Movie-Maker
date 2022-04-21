/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Script compiler for gob based scripts.  The real compilation is
	done at the SCCB class level.  The SCCG class just provides mapping
	of identifiers to opcodes for GOB specific script primitives.

***************************************************************************/
#include "kidframe.h"
ASSERTNAME

RTCLASS(SCCG)


SZOP _rgszopSccg[] =
	{
	{ kopCreateChildGob, PszLit("CreateChildGob") },
	{ kopCreateChildThis, PszLit("CreateChildThis") },
	{ kopDestroyGob, PszLit("DestroyGob") },
	{ kopDestroyThis, PszLit("DestroyThis") },
	{ kopResizeGob, PszLit("ResizeGob") },
	{ kopResizeThis, PszLit("ResizeThis") },
	{ kopMoveRelGob, PszLit("MoveRelGob") },
	{ kopMoveRelThis, PszLit("MoveRelThis") },
	{ kopMoveAbsGob, PszLit("MoveAbsGob") },
	{ kopMoveAbsThis, PszLit("MoveAbsThis") },
	{ kopGidThis, PszLit("GidThis") },
	{ kopGidParGob, PszLit("GidParGob") },
	{ kopGidParThis, PszLit("GidParThis") },
	{ kopGidNextSib, PszLit("GidNextSib") },
	{ kopGidPrevSib, PszLit("GidPrevSib") },
	{ kopGidChild, PszLit("GidChild") },
	{ kopFGobExists, PszLit("FGobExists") },
	{ kopCreateClock, PszLit("CreateClock") },
	{ kopDestroyClock, PszLit("DestroyClock") },
	{ kopStartClock, PszLit("StartClock") },
	{ kopStopClock, PszLit("StopClock") },
	{ kopTimeCur, PszLit("TimeCur") },
	{ kopSetAlarm, PszLit("SetAlarm") },
	{ kopEnqueueCid, PszLit("EnqueueCid") },
	{ kopAlert, PszLit("Alert") },
	{ kopRunScriptGob, PszLit("RunScriptGob") },
	{ kopRunScriptThis, PszLit("RunScriptThis") },
	{ kopStateGob, PszLit("StateGob") },
	{ kopStateThis, PszLit("StateThis") },
	{ kopChangeStateGob, PszLit("ChangeStateGob") },
	{ kopChangeStateThis, PszLit("ChangeStateThis") },
	{ kopAnimateGob, PszLit("AnimateGob") },
	{ kopAnimateThis, PszLit("AnimateThis") },
	{ kopSetPictureGob, PszLit("SetPictureGob") },
	{ kopSetPictureThis, PszLit("SetPictureThis") },
	{ kopSetRepGob, PszLit("SetRepGob") },
	{ kopSetRepThis, PszLit("SetRepThis") },
	{ kopXMouseGob, PszLit("XMouseGob") },
	{ kopXMouseThis, PszLit("XMouseThis") },
	{ kopYMouseGob, PszLit("YMouseGob") },
	{ kopYMouseThis, PszLit("YMouseThis") },
	{ kopGidUnique, PszLit("GidUnique") },
	{ kopXGob, PszLit("XGob") },
	{ kopXThis, PszLit("XThis") },
	{ kopYGob, PszLit("YGob") },
	{ kopYThis, PszLit("YThis") },
	{ kopZGob, PszLit("ZGob") },
	{ kopZThis, PszLit("ZThis") },
	{ kopSetZGob, PszLit("SetZGob") },
	{ kopSetZThis, PszLit("SetZThis") },
	{ kopSetColorTable, PszLit("SetColorTable") },
	{ kopCell, PszLit("Cell") },
	{ kopCellNoPause, PszLit("CellNoPause") },
	{ kopGetModifierState, PszLit("GetModifierState") },
	{ kopChangeModifierState, PszLit("ChangeModifierState") },
	{ kopCreateHelpGob, PszLit("CreateHelpGob") },
	{ kopCreateHelpThis, PszLit("CreateHelpThis") },
	{ kopTransition, PszLit("Transition") },
	{ kopGetEdit, PszLit("GetEdit") },
	{ kopSetEdit, PszLit("SetEdit") },
	{ kopAlertStr, PszLit("AlertStr") },
	{ kopGetProp, PszLit("GetProp") },
	{ kopSetProp, PszLit("SetProp") },
	{ kopLaunch, PszLit("Launch") },
	{ kopPlayGob, PszLit("PlayGob") },
	{ kopPlayThis, PszLit("PlayThis") },
	{ kopPlayingGob, PszLit("PlayingGob") },
	{ kopPlayingThis, PszLit("PlayingThis") },
	{ kopStopGob, PszLit("StopGob") },
	{ kopStopThis, PszLit("StopThis") },
	{ kopCurFrameGob, PszLit("CurFrameGob") },
	{ kopCurFrameThis, PszLit("CurFrameThis") },
	{ kopCountFramesGob, PszLit("CountFramesGob") },
	{ kopCountFramesThis, PszLit("CountFramesThis") },
	{ kopGotoFrameGob, PszLit("GotoFrameGob") },
	{ kopGotoFrameThis, PszLit("GotoFrameThis") },
	{ kopFilterCmdsGob, PszLit("FilterCmdsGob") },
	{ kopFilterCmdsThis, PszLit("FilterCmdsThis") },
	{ kopDestroyChildrenGob, PszLit("DestroyChildrenGob") },
	{ kopDestroyChildrenThis, PszLit("DestroyChildrenThis") },
	{ kopPlaySoundThis, PszLit("PlaySoundThis") },
	{ kopPlaySoundGob, PszLit("PlaySoundGob") },
	{ kopStopSound, PszLit("StopSound") },
	{ kopStopSoundClass, PszLit("StopSoundClass") },
	{ kopPlayingSound, PszLit("PlayingSound") },
	{ kopPlayingSoundClass, PszLit("PlayingSoundClass") },
	{ kopPauseSound, PszLit("PauseSound") },
	{ kopPauseSoundClass, PszLit("PauseSoundClass") },
	{ kopResumeSound, PszLit("ResumeSound") },
	{ kopResumeSoundClass, PszLit("ResumeSoundClass") },
	{ kopPlayMouseSoundGob, PszLit("PlayMouseSoundGob") },
	{ kopPlayMouseSoundThis, PszLit("PlayMouseSoundThis") },
	{ kopRunScriptCnoGob, PszLit("RunScriptCnoGob") },
	{ kopRunScriptCnoThis, PszLit("RunScriptCnoThis") },
	{ kopWidthGob, PszLit("WidthGob") },
	{ kopWidthThis, PszLit("WidthThis") },
	{ kopHeightGob, PszLit("HeightGob") },
	{ kopHeightThis, PszLit("HeightThis") },
	{ kopSetNoSlipGob, PszLit("SetNoSlipGob") },
	{ kopSetNoSlipThis, PszLit("SetNoSlipThis") },
	{ kopFIsDescendent, PszLit("FIsDescendent") },
	{ kopPrint, PszLit("Print") },
	{ kopPrintStr, PszLit("PrintStr") },
	{ kopSetMasterVolume, PszLit("SetMasterVolume") },
	{ kopGetMasterVolume, PszLit("GetMasterVolume") },
	{ kopStartLongOp, PszLit("StartLongOp") },
	{ kopEndLongOp, PszLit("EndLongOp") },
	{ kopSetToolTipSourceGob, PszLit("SetToolTipSourceGob") },
	{ kopSetToolTipSourceThis, PszLit("SetToolTipSourceThis") },
	{ kopSetAlarmGob, PszLit("SetAlarmGob") },
	{ kopSetAlarmThis, PszLit("SetAlarmThis") },
	{ kopModalHelp, PszLit("ModalHelp") },
	{ kopFlushUserEvents, PszLit("FlushUserEvents") },
	{ kopStreamGob, PszLit("StreamGob") },
	{ kopStreamThis, PszLit("StreamThis") },
	{ kopPrintStat, PszLit("PrintStat") },
	{ kopPrintStrStat, PszLit("PrintStrStat") },

	{ opNil, pvNil },
	};


AROP _rgaropSccg[] =
	{
	{ kopCreateChildGob, PszLit("CreateChildGob"), 3, 0, 0, fFalse },
	{ kopCreateChildThis, PszLit("CreateChildThis"), 2, 0, 0, fFalse },
	{ kopDestroyGob, PszLit("DestroyGob"), 1, 0, 0, fTrue },
	{ kopDestroyThis, PszLit("DestroyThis"), 0, 0, 0, fTrue },
	{ kopResizeGob, PszLit("ResizeGob"), 3, 0, 0, fTrue },
	{ kopResizeThis, PszLit("ResizeThis"), 2, 0, 0, fTrue },
	{ kopMoveRelGob, PszLit("MoveRelGob"), 3, 0, 0, fTrue },
	{ kopMoveRelThis, PszLit("MoveRelThis"), 2, 0, 0, fTrue },
	{ kopMoveAbsGob, PszLit("MoveAbsGob"), 3, 0, 0, fTrue },
	{ kopMoveAbsThis, PszLit("MoveAbsThis"), 2, 0, 0, fTrue },
	{ kopGidThis, PszLit("GidThis"), 0, 0, 0, fFalse },
	{ kopGidParGob, PszLit("GidParGob"), 1, 0, 0, fFalse },
	{ kopGidParThis, PszLit("GidParThis"), 0, 0, 0, fFalse },
	{ kopGidNextSib, PszLit("GidNextSib"), 1, 0, 0, fFalse },
	{ kopGidPrevSib, PszLit("GidPrevSib"), 1, 0, 0, fFalse },
	{ kopGidChild, PszLit("GidChild"), 1, 0, 0, fFalse },
	{ kopFGobExists, PszLit("FGobExists"), 1, 0, 0, fFalse },
	{ kopCreateClock, PszLit("CreateClock"), 1, 0, 0, fFalse },
	{ kopDestroyClock, PszLit("DestroyClock"), 1, 0, 0, fTrue },
	{ kopStartClock, PszLit("StartClock"), 2, 0, 0, fTrue },
	{ kopStopClock, PszLit("StopClock"), 1, 0, 0, fTrue },
	{ kopTimeCur, PszLit("TimeCur"), 1, 0, 0, fFalse },
	{ kopSetAlarm, PszLit("SetAlarm"), 2, 0, 0, fTrue },
	{ kopEnqueueCid, PszLit("EnqueueCid"), 6, 0, 0, fTrue },
	{ kopAlert, PszLit("Alert"), 0, 1, 1, fTrue },
	{ kopRunScriptGob, PszLit("RunScriptGob"), 2, 1, 0, fFalse },
	{ kopRunScriptThis, PszLit("RunScriptThis"), 1, 1, 0, fFalse },
	{ kopStateGob, PszLit("StateGob"), 1, 0, 0, fFalse },
	{ kopStateThis, PszLit("StateThis"), 0, 0, 0, fFalse },
	{ kopChangeStateGob, PszLit("ChangeStateGob"), 2, 0, 0, fTrue },
	{ kopChangeStateThis, PszLit("ChangeStateThis"), 1, 0, 0, fTrue },
	{ kopAnimateGob, PszLit("AnimateGob"), 2, 0, 0, fTrue },
	{ kopAnimateThis, PszLit("AnimateThis"), 1, 0, 0, fTrue },
	{ kopSetPictureGob, PszLit("SetPictureGob"), 2, 0, 0, fTrue },
	{ kopSetPictureThis, PszLit("SetPictureThis"), 1, 0, 0, fTrue },
	{ kopSetRepGob, PszLit("SetRepGob"), 2, 0, 0, fTrue },
	{ kopSetRepThis, PszLit("SetRepThis"), 1, 0, 0, fTrue },
	{ kopXMouseGob, PszLit("XMouseGob"), 1, 0, 0, fFalse },
	{ kopXMouseThis, PszLit("XMouseThis"), 0, 0, 0, fFalse },
	{ kopYMouseGob, PszLit("YMouseGob"), 1, 0, 0, fFalse },
	{ kopYMouseThis, PszLit("YMouseThis"), 0, 0, 0, fFalse },
	{ kopGidUnique, PszLit("GidUnique"), 0, 0, 0, fFalse },
	{ kopXGob, PszLit("XGob"), 1, 0, 0, fFalse },
	{ kopXThis, PszLit("XThis"), 0, 0, 0, fFalse },
	{ kopYGob, PszLit("YGob"), 1, 0, 0, fFalse },
	{ kopYThis, PszLit("YThis"), 0, 0, 0, fFalse },
	{ kopZGob, PszLit("ZGob"), 1, 0, 0, fFalse },
	{ kopZThis, PszLit("ZThis"), 0, 0, 0, fFalse },
	{ kopSetZGob, PszLit("SetZGob"), 2, 0, 0, fTrue },
	{ kopSetZThis, PszLit("SetZThis"), 1, 0, 0, fTrue },
	{ kopSetColorTable, PszLit("SetColorTable"), 1, 0, 0, fTrue },
	{ kopCell, PszLit("Cell"), 4, 0, 0, fTrue },
	{ kopCellNoPause, PszLit("CellNoPause"), 4, 0, 0, fTrue },
	{ kopGetModifierState, PszLit("GetModifierState"), 0, 0, 0, fFalse },
	{ kopChangeModifierState, PszLit("ChangeModifierState"), 2, 0, 0, fTrue },
	{ kopCreateHelpGob, PszLit("CreateHelpGob"), 2, 0, 0, fFalse },
	{ kopCreateHelpThis, PszLit("CreateHelpThis"), 1, 0, 0, fFalse },
	{ kopTransition, PszLit("Transition"), 5, 0, 0, fTrue },
	{ kopGetEdit, PszLit("GetEdit"), 2, 0, 0, fTrue },
	{ kopSetEdit, PszLit("SetEdit"), 2, 0, 0, fTrue },
	{ kopAlertStr, PszLit("AlertStr"), 0, 1, 1, fTrue },
	{ kopGetProp, PszLit("GetProp"), 1, 0, 0, fFalse },
	{ kopSetProp, PszLit("SetProp"), 2, 0, 0, fTrue },
	{ kopLaunch, PszLit("Launch"), 1, 0, 0, fFalse },
	{ kopPlayGob, PszLit("PlayGob"), 1, 0, 0, fTrue },
	{ kopPlayThis, PszLit("PlayThis"), 0, 0, 0, fTrue },
	{ kopPlayingGob, PszLit("PlayingGob"), 1, 0, 0, fFalse },
	{ kopPlayingThis, PszLit("PlayingThis"), 0, 0, 0, fFalse },
	{ kopStopGob, PszLit("StopGob"), 1, 0, 0, fTrue },
	{ kopStopThis, PszLit("StopThis"), 0, 0, 0, fTrue },
	{ kopCurFrameGob, PszLit("CurFrameGob"), 1, 0, 0, fFalse },
	{ kopCurFrameThis, PszLit("CurFrameThis"), 0, 0, 0, fFalse },
	{ kopCountFramesGob, PszLit("CountFramesGob"), 1, 0, 0, fFalse },
	{ kopCountFramesThis, PszLit("CountFramesThis"), 0, 0, 0, fFalse },
	{ kopGotoFrameGob, PszLit("GotoFrameGob"), 2, 0, 0, fTrue },
	{ kopGotoFrameThis, PszLit("GotoFrameThis"), 1, 0, 0, fTrue },
	{ kopFilterCmdsGob, PszLit("FilterCmdsGob"), 4, 0, 0, fTrue },
	{ kopFilterCmdsThis, PszLit("FilterCmdsThis"), 3, 0, 0, fTrue },
	{ kopDestroyChildrenGob, PszLit("DestroyChildrenGob"), 1, 0, 0, fTrue },
	{ kopDestroyChildrenThis, PszLit("DestroyChildrenThis"), 0, 0, 0, fTrue },
	{ kopPlaySoundThis, PszLit("PlaySoundThis"), 7, 0, 0, fFalse },
	{ kopPlaySoundGob, PszLit("PlaySoundGob"), 8, 0, 0, fFalse },
	{ kopStopSound, PszLit("StopSound"), 1, 0, 0, fTrue },
	{ kopStopSoundClass, PszLit("StopSoundClass"), 2, 0, 0, fTrue },
	{ kopPlayingSound, PszLit("PlayingSound"), 1, 0, 0, fFalse },
	{ kopPlayingSoundClass, PszLit("PlayingSoundClass"), 2, 0, 0, fFalse },
	{ kopPauseSound, PszLit("PauseSound"), 1, 0, 0, fTrue },
	{ kopPauseSoundClass, PszLit("PauseSoundClass"), 2, 0, 0, fTrue },
	{ kopResumeSound, PszLit("ResumeSound"), 1, 0, 0, fTrue },
	{ kopResumeSoundClass, PszLit("ResumeSoundClass"), 2, 0, 0, fTrue },
	{ kopPlayMouseSoundGob, PszLit("PlayMouseSoundGob"), 3, 0, 0, fTrue },
	{ kopPlayMouseSoundThis, PszLit("PlayMouseSoundThis"), 2, 0, 0, fTrue },
	{ kopRunScriptCnoGob, PszLit("RunScriptCnoGob"), 2, 1, 0, fFalse },
	{ kopRunScriptCnoThis, PszLit("RunScriptCnoThis"), 1, 1, 0, fFalse },
	{ kopWidthGob, PszLit("WidthGob"), 1, 0, 0, fFalse },
	{ kopWidthThis, PszLit("WidthThis"), 0, 0, 0, fFalse },
	{ kopHeightGob, PszLit("HeightGob"), 1, 0, 0, fFalse },
	{ kopHeightThis, PszLit("HeightThis"), 0, 0, 0, fFalse },
	{ kopSetNoSlipGob, PszLit("SetNoSlipGob"), 2, 0, 0, fTrue },
	{ kopSetNoSlipThis, PszLit("SetNoSlipThis"), 1, 0, 0, fTrue },
	{ kopFIsDescendent, PszLit("FIsDescendent"), 2, 0, 0, fFalse },
	{ kopPrint, PszLit("Print"), 0, 1, 1, fTrue },
	{ kopPrintStr, PszLit("PrintStr"), 0, 1, 1, fTrue },
	{ kopSetMasterVolume, PszLit("SetMasterVolume"), 1, 0, 0, fTrue },
	{ kopGetMasterVolume, PszLit("GetMasterVolume"), 0, 0, 0, fFalse },
	{ kopStartLongOp, PszLit("StartLongOp"), 0, 0, 0, fTrue },
	{ kopEndLongOp, PszLit("EndLongOp"), 1, 0, 0, fTrue },
	{ kopSetToolTipSourceGob, PszLit("SetToolTipSourceGob"), 2, 0, 0, fTrue },
	{ kopSetToolTipSourceThis, PszLit("SetToolTipSourceThis"), 1, 0, 0, fTrue },
	{ kopSetAlarmGob, PszLit("SetAlarmGob"), 4, 0, 0, fTrue },
	{ kopSetAlarmThis, PszLit("SetAlarmThis"), 3, 0, 0, fTrue },
	{ kopModalHelp, PszLit("ModalHelp"), 2, 0, 0, fFalse },
	{ kopFlushUserEvents, PszLit("FlushUserEvents"), 1, 0, 0, fTrue },
	{ kopStreamGob, PszLit("StreamGob"), 2, 0, 0, fTrue },
	{ kopStreamThis, PszLit("StreamThis"), 1, 0, 0, fTrue },
	{ kopPrintStat, PszLit("PrintStat"), 0, 1, 1, fTrue },
	{ kopPrintStrStat, PszLit("PrintStrStat"), 0, 1, 1, fTrue },

	{ opNil, pvNil, 0, 0, 0, fTrue },
	};


/***************************************************************************
	Map a string to an operator.
***************************************************************************/
long SCCG::_OpFromStn(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	long op;
	if (opNil != (op = _OpFromStnRgszop(pstn, _rgszopSccg)))
		return op;
    return SCCG_PAR::_OpFromStn(pstn);
}


/***************************************************************************
	Map an op code to a string.
***************************************************************************/
bool SCCG::_FGetStnFromOp(long op, PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);

	if (_FGetStnFromOpRgszop(op, pstn, _rgszopSccg))
		return fTrue;
	return SCCG_PAR::_FGetStnFromOp(op, pstn);
}


/***************************************************************************
	Map a string to an operator with argument information (for in-fix
	compiler).
***************************************************************************/
bool SCCG::_FGetOpFromName(PSTN pstn, long *pop, long *pclwFixed,
	long *pclwVar, long *pcactMinVar, bool *pfVoid)
{
	if (_FGetArop(pstn, _rgaropSccg, pop, pclwFixed,
			pclwVar, pcactMinVar, pfVoid))
		{
		return fTrue;
		}
	return SCCG_PAR::_FGetOpFromName(pstn, pop, pclwFixed,
		pclwVar, pcactMinVar, pfVoid);
}


/***************************************************************************
	Return the current version number of the script compiler.
***************************************************************************/
short SCCG::_SwCur(void)
{
	return kswCurSccg;
}


/***************************************************************************
	Return the back version number of the script compiler.  Versions
	back to here can read this script.
***************************************************************************/
short SCCG::_SwBack(void)
{
	return kswBackSccg;
}


/***************************************************************************
	Return the min version number of the script compiler.  We can read
	scripts back to this version.
***************************************************************************/
short SCCG::_SwMin(void)
{
	return kswMinSccg;
}


