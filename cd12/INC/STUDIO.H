/***************************************************************************

	Studio Stuff

		The Studio

				STDIO 	--->   	GOB

***************************************************************************/

#ifndef STUDIO_H
#define STUDIO_H

#include "soc.h"
#include "kidgsdef.h"
#include "stdiodef.h"
#include "buildgob.h"
#include "sharedef.h"
#include "stdiocrs.h"
#include "tgob.h"
#include "stdioscb.h"
#include "ape.h"
#include "browser.h"
#include "popup.h"
#include "esl.h"
#include "scnsort.h"
#include "tatr.h"
#include "utest.h"
#include "version.h"
#include "portf.h"
#include "splot.h"
#include "helpbook.h"
#include "helptops.h"

typedef class SMCC *PSMCC;

const long kcmhlStudio = 0x10000;	// nice medium level for the Studio

extern APP vapp;


//
// Studio class
//
#define STDIO_PAR GOB
#define kclsSTDIO 'stio'
class STDIO : public STDIO_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM
	CMD_MAP_DEC(STDIO)

protected:

	PCRM  _pcrm;
	PGST  _pgstMisc;
	PMVIE _pmvie;
	PSMCC _psmcc;
	PGL _pglpbrcn;
	long _aridSelected;
	PBRWR _pbrwrActr;
	PBRWR _pbrwrProp;
	PGL _pglcmg;   		//Cno map tmpl->gokd for rollcall
	PGL _pglclr;		// Color table for common palette
	bool _fDisplayCast;	// Display movie's cast

	CMD _cmd;
	long _dtimToolTipDelay;
	bool _fStopUISound;
	PTGOB _ptgobTitle;
	bool _fStartedSoonerLater;

	STDIO(PGCB pgcb) : GOB(pgcb) {};
	bool _FOpenStudio(bool fPaletteFade);
	void _SetToolStates(void);
	bool _FBuildMenuCidCtg(long cid, CTG ctg, PGL pgl,
		ulong grfHotKey, ulong grfNum, bool fNew);
	PBRCN _PbrcnFromBrwdid(long brwdid);
#ifdef BUG1959
	bool _FLoadMovie(PFNI pfni, CNO cno, bool *pfClosedOld);
#endif // BUG1959

public:

	//
	// Create and destroy functions
	//
	static PSTDIO PstdioNew(long hid, PCRM pcrmStudio, PFNI pfniUserDoc = pvNil,
		bool fFailIfDocOpenFailed = fTrue);
	void ReleaseBrcn(void);
	~STDIO(void);

	//
	// Command functions for getting from scripts to here.
	//
	bool FCmdXYAxis(PCMD pcmd);
	bool FCmdXZAxis(PCMD pcmd);
	bool FCmdRecordPath(PCMD pcmd);
	bool FCmdRerecordPath(PCMD pcmd);
	bool FCmdSetTool(PCMD pcmd);
	bool FCmdPlay(PCMD pcmd);
	bool FCmdNewScene(PCMD pcmd);
	bool FCmdRespectGround(PCMD pcmd);
	bool FCmdPause(PCMD pcmd);
	bool FCmdOpen(PCMD pcmb);
	bool FCmdBrowserReady(PCMD pcmd);
	bool FCmdScroll(PCMD pcmd);
	bool FCmdSooner(PCMD pcmd);
	bool FCmdLater(PCMD pcmd);
	bool FCmdNewSpletter(PCMD pcmd);
	bool FCmdCreatePopup(PCMD pcmd);
	bool FCmdTextSetColor(PCMD pcmd);
	bool FCmdTextSetBkgdColor(PCMD pcmd);
	bool FCmdTextSetFont(PCMD pcmd);
	bool FCmdTextSetStyle(PCMD pcmd);
	bool FCmdTextSetSize(PCMD pcmd);
	bool FCmdOpenSoundRecord(PCMD pcmd);
	bool FBuildActorMenu(void);
	bool FCmdToggleXY(PCMD pcmd);
	bool FCmdHelpBook(PCMD pcmd);
	bool FCmdMovieGoto(PCMD pcmd);
	bool FCmdLoadProjectMovie(PCMD pcmd);
	bool FCmdSoundsEnabled(PCMD pcmd);
	bool FCmdCreateTbox(PCMD pcmd);
	bool FCmdActorEaselOpen(PCMD pcmd);
	bool FCmdListenerEaselOpen(PCMD pcmd);

#ifdef DEBUG
	bool FCmdWriteBmps(PCMD pcmd);
#endif // DEBUG

	//
	// Call back functions
	//
	void PlayStopped(void);
	void ChangeTool(long tool);
	void SceneNuked(void);
	void SceneUnnuked(void);
	void ActorNuked(void);
	void EnableActorTools(void);
	void EnableTboxTools(void);
	void TboxSelected(void);
	void SetUndo(long undo);
	void SetCurs(long tool);
	void ActorEasel(bool *pfActrChanged);
	void SceneChange(void);
	void PauseType(WIT wit);
	void Recording(bool fRecording, bool fRecord);
	void StartSoonerLater(void);
	void EndSoonerLater(void);
	void NewActor(void);
	void StartActionBrowser(void);
	void StartListenerEasel(void);
	void PlayUISound(long tool, long grfcust);
	void StopUISound(void);
	void UpdateTitle(PSTN pstnTitle);

	bool FEdit3DText(PSTN pstn, long *ptdts);
	void SetAridSelected(long arid) { _aridSelected = arid; }
	long AridSelected(void) { return _aridSelected; }
	PBRWR PbrwrActr(void) { return _pbrwrActr; }
	PBRWR PbrwrProp(void) { return _pbrwrProp; }
	bool FAddCmg(CNO cnoTmpl, CNO cnoGokd);
	CNO CnoGokdFromCnoTmpl(CNO cnoTmpl);
	void SetDisplayCast(bool fDisplayCast) { _fDisplayCast = fDisplayCast; }

	bool FShutdown(bool fClearCache = fTrue);

	// Stop and restart the action button's animation
	static void PauseActionButton(void);
	static void ResumeActionButton(void);

	// Misc Studio strings
	void GetStnMisc(long ids, PSTN pstn);

	//
	// Movie changing
	//
	bool FLoadMovie(PFNI pfni = pvNil, CNO cno=cnoNil);
	bool FSetMovie(PMVIE pmvie);
	PMVIE Pmvie() {return _pmvie; };
	bool FGetFniMovieOpen(PFNI pfni)
	{
		return FPortDisplayWithIds(pfni, fTrue, idsPortfMovieFilterLabel, idsPortfMovieFilterExt, 
				idsPortfOpenMovieTitle, pvNil, pvNil, pvNil, fpfPortPrevMovie, kwavPortOpenMovie);
	}
	PSMCC Psmcc(void) { return _psmcc; }
	};

#define SMCC_PAR MCC
#define kclsSMCC 'SMCC'
class SMCC : public SMCC_PAR
	{
	RTCLASS_DEC
	ASSERT
	MARKMEM

private:
	PSSCB _psscb;
	PSTDIO _pstdio;
	long _dypTextTbox;

public:
	~SMCC(void) { ReleasePpo(&_psscb); }
	SMCC(long dxp, long dyp, long cbCache, PSSCB psscb, PSTDIO pstdio);

	virtual long Dxp(void) { return _dxp; }
	virtual long Dyp(void) { return _dyp; }
	virtual long CbCache(void) { return _cbCache; }
	virtual PSSCB Psscb(void) { return _psscb; }
	virtual void SetCurs(long tool) { _pstdio->SetCurs(tool); }
	virtual void ActorSelected(long arid)
		{ _pstdio->SetAridSelected(arid); UpdateRollCall(); }
	virtual void UpdateAction(void) { }		// Update selected action
	virtual void UpdateRollCall(void);
	virtual void UpdateScrollbars(void) { if (pvNil != _psscb) _psscb->Update(); }
	virtual void SetSscb(PSSCB psscb)
		{
		AssertNilOrPo(psscb, 0);
		ReleasePpo(&_psscb);
		_psscb = psscb;
		if (pvNil != _psscb)
			_psscb->AddRef();
		}

	virtual void PlayStopped(void) { _pstdio->PlayStopped(); }
	virtual void ChangeTool(long tool) { _pstdio->ChangeTool(tool); }
	virtual void SceneNuked(void) { _pstdio->SceneNuked(); }
	virtual void SceneUnnuked(void) { _pstdio->SceneUnnuked(); }
	virtual void ActorNuked(void) { _pstdio->ActorNuked(); }
	virtual void EnableActorTools(void) { _pstdio->EnableActorTools(); }
	virtual void EnableTboxTools(void) { _pstdio->EnableTboxTools(); }
	virtual void TboxSelected(void) { _pstdio->TboxSelected(); }
	virtual void ActorEasel(bool *pfActrChanged) { _pstdio->ActorEasel(pfActrChanged); }
	virtual void SetUndo(long undo) { _pstdio->SetUndo(undo); }
	virtual void SceneChange(void) { _pstdio->SceneChange(); }
	virtual void PauseType(WIT wit) { _pstdio->PauseType(wit); }
	virtual void Recording(bool fRecording, bool fRecord) { _pstdio->Recording(fRecording, fRecord); }
	virtual void StartSoonerLater(void) { _pstdio->StartSoonerLater(); }
	virtual void EndSoonerLater(void) { _pstdio->EndSoonerLater(); }
	virtual void NewActor(void) { _pstdio->NewActor(); }
	virtual void StartActionBrowser(void) { _pstdio->StartActionBrowser(); }
	virtual void StartListenerEasel(void) { _pstdio->StartListenerEasel(); }
	virtual bool GetFniSave(FNI *pfni, long lFilterLabel, long lFilterExt, long lTitle, LPTSTR lpstrDefExt, PSTN pstnDefFileName)
					{ return(FPortDisplayWithIds(pfni, fFalse, lFilterLabel, lFilterExt, lTitle, lpstrDefExt, pstnDefFileName, 
						pvNil, fpfPortPrevMovie, kwavPortSaveMovie)); }
	virtual void PlayUISound(long tool, long grfcust) { _pstdio->PlayUISound(tool, grfcust); }
	virtual void StopUISound(void) { _pstdio->StopUISound(); }
	virtual void UpdateTitle(PSTN pstnTitle) { _pstdio->UpdateTitle(pstnTitle); }
	virtual void EnableAccel(void) { vpapp->EnableAccel(); }
	virtual void DisableAccel(void) { vpapp->DisableAccel(); }
	virtual void GetStn(long ids, PSTN pstn) { vpapp->FGetStnApp(ids, pstn); }
	virtual long DypTboxDef(void);
	virtual void SetSndFrame(bool fSoundInFrame) { _psscb->SetSndFrame(fSoundInFrame); }
	virtual bool FMinimized(void) { return(vpapp->FMinimized()); }
	virtual bool FQueryPurgeSounds(void);
	};

#endif //STUDIO_H
