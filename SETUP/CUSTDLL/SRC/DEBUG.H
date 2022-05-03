#ifdef _DEBUG

const char camfText[][24] = {
	"camfInitializeObject",
	"camfInitializeTrigger",
	"camfInitFromProof",
	"camfInitSearchObject",
	"camfSearchForObject",
	"camfAnswerDependClause",
	"camfAnswerDependRemove",
	"camfAnswerCompanion",
	"camfSetDstDirOfTrigger",
	"camfPropagateDstDir",
	"camfCalcDstDir",
	"camfSelectModeOfObject",
	"camfSetModeOfObject",
	"camfSetModeOfTrigger",
	"camfCheckObjectIBSE",
	"camfFinalizeObject",
	"camfAddToCopyList",
	"camfGetNonCopyListCost",
	"camfGetCost",
	"camfGetOODSCostStr",
	"camfGetOODSCostNum",
	"camfAddBillBoards",
	"camfDoNonVisualMods",
	"camfDoVisualMods",
	"camfPostInstall",
	"camfDtor",
	"camfSelectableSubOpts",
	"camfSetRgfChecked",
	"camfGetUIDstDirObj",
	"camfGetCAddCRemove",
	"camfSetOisState",
	"camfSetInstallData",
	"camfSetDstDir",
	"camfSetDstDirWithSubdir",
	"camfSetDstDirUserChoice",
	"camfProcessDstDirTree",
	"camfSetDstDirInTree",
	"camfSetIBSEState",
	"camfSetVisitedIBSE",
	"camfSetIbyState",
	"camfGetObjData",
	"camfError"};

void vDebugCA(POD pod, CAMF camf, SZ pTitle)
{
char Title[40], Text[512];

if(camf >= 42)
  camf=41;

wsprintf(Title, "%s - %s", (LPSTR)pTitle, (LPSTR)camfText[camf]);

if(pod)

  wsprintf(Text,  "or=%u\n"
		"ois=%u\n"
		"ot=%u\n"
		"ynQuietMode=%u\n"
		"szTitle='%s'\n"
		"szDescription='%s'\n"
		"szObjectData='%s'\n"
		"szBitmapId='%s'\n"
		"va=%u\n"
		"sa=%u\n"
		"acd=%u\n"
		"szTblDstDir='%s'\n"
		"orCheckDir=%u\n"
		"szDstDir='%s'\n"
		"fDstDirAllocated=%u\n"
		"fDstDirSet=%u\n"
		"fDstDirBase=%u\n"
		"fDstDirUserChoice=%u\n"
		"ynmeIBSEState;=%u\n"
		"fVisitedIBSE=%u\n"
		"iby=%u\n"
		"szInstallData='%s'\n"
		"szInstallDir='%s'",
	pod->or,
	pod->ois,
	pod->ot,
	pod->ynQuietMode,
	(LPSTR)pod->szTitle,
	(LPSTR)pod->szDescription,
	(LPSTR)pod->szObjectData,
	(LPSTR)pod->szBitmapId,
	pod->va,
	pod->sa,
	pod->acd,
	(LPSTR)pod->szTblDstDir,
	pod->orCheckDir,
	(LPSTR)pod->szDstDir,
	pod->fDstDirAllocated,
	pod->fDstDirSet,
	pod->fDstDirBase,
	pod->fDstDirUserChoice,
	pod->ynmeIBSEState,
	pod->fVisitedIBSE,
	pod->iby,
	(LPSTR)pod->szInstallData,
	(LPSTR)pod->szInstallDir);

else
  lstrcpy(Text, "pod == NULL");

DoMsgBox(Text, Title, MB_OK);
}
#endif
