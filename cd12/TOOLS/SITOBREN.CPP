/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/***************************************************************************

	sitobren.cpp
	Main module for sitobren, a utility for converting SoftImage
	.hrc files into chunky files with the appropriate TMPL chunk
	and all its associated children.

	Syntax of a sitobren script (.s2b file):

	ACTOR CNO <TMPL-id> NAMED "<actor_name>" [XREST <xaRest>]
		[YREST <yaRest>] [ZREST <zaRest>] [FLAGS <cust_mat_only>]
		[BPSETS <bpsets>]
	ACTION NAMED "<action_name>" FILEBASE "<file_name_base>"
		FIRST <start-#> LAST <end-#> FLAGS <action_flags>
		[SCALE <dwrScale>] [SKIP <skip_count>] [SUBMODEL "<submodel>"]
	BACKGROUND CNO <BKGD-id> NAMED "<bkgd_name>" [LIGHTS <#lights>]
		[CAMERAS <#cameras>] [FIRST <first_pal>] [LENGTH <#entries>]
	COSTUME FILE "<filename>" USE_SETS <set #> [<set #> [<set #> [...]]]

	As many actions as are necessary may be defined.  Actions are
	defined for the actor which they follow.  Actors should only
	be defined once; this implies that all actions should be defined
	before specifying a new actor.

	Parameters in [] are optional.  The parameters for a given command
	can appear in any order, but all desired parameters must appear before
	the next command, of course.  Any number of spaces, tabs, line-feeds,
	commas, and semi-colons may appear between defined tokens; feel free to
	insert them as necessary to improve the readability of your script file.
	The tokens are case-insensitive.  Feel free to write "ACTOR," "actor,"
	"Actor," "aCtOr," etc.

	Valid values:
		<actor_name>
		<action_name>
		<bkgd_name>      -- any string conforming to actor/action naming
			conventions
		<TMPL-id>
		<BKGD-id>        -- Chunk number for TMPL or BKGD chunk
		<xaRest>
		<yaRest>
		<zaRest>         -- Any valid integer, units are degrees
		<cust_mat_only>  -- 1 if actor can only use custom materials, 0 otherwise
		<file_name_base> -- A model's action cel filename follows the
			following convention: "<drive>:\<path>\<model_action>-<cel#>.hrc"
			The <drive> <path> and <model_action> parts, with their delimiters,
			must be provided if the files aren't in the current directory.  The
			converter will supply the remaining portions, reading in all cel
			files for the given action
		<start-#>        -- The # of the 1st cel file (usually either 0 or 1)
		<end-#>          -- The # of the last cel file
		<dwrScale>       -- the distance between cels for static actions; for
			non-static actions, specifies a scaling to apply to the distance
			specified in the cel data.  Can be an integer or a decimal
		<action_flags>   -- action flags; includes three path orientation flags
			and one flag to indicate a static action
		<skip_count>     -- increment for cel file numbers; sitobren will skip
			<skip_count>-1 files between each cel.
		<submodel>       -- name of submodel to extract from action; if this
			actor has multiple actions, each action must extract the same
			submodel, so that the actor's heirarchy is identical from one
			action to the next.
		<bpsets>          -- 1 if Body Part Sets are required, 0 otherwise
		<#lights>         -- number of lights in scene
		<#cameras>        -- number of cameras in scene
		<first_pal>       -- first palette entry to use
		<#entries>        -- how many palette entries to use
		<filename>        -- a complete filename specifying the .hrc to extract
			costume information from
		<set #>           -- a valid body part set number; refers to the body
			part set given in the .hrc file, not the mapped Brender body part
			set number.

	Default values for optional items:
		<xaRest> <yaRest> <zaRest> -- 0
		<cust_mat_only>            -- 0
		<dwrScale>                 -- 1.0
		<action_flags>             -- 0
		<skip_count>               -- 1
		<submodel>                 -- "" (simply means extract everything)
		<bpsets>                   -- 1
		<#lights>                  -- 2
		<#cameras>                 -- 9
		<first_pal>                -- 151
		<#entries>                 -- 95

	Command line:
		sitobren <script[.s2b]> [<outfile[.cht]>] [flags]

		The extensions may be ommited, in which case the defaults listed will
		be used.  The user can override the default extension by supplying
		the full filename, including the extension.

		-n[ologo]  don't display the banner
		-v[#]      verbose; optional level 0-2 (quiet to verbose (1 is default))
		-h         swap the handedness (negate z positions and translations)
		-r         don't do any rounding
		-r#        round coordinates of models losing # bits
		-k         Kontinue on errors
		-p         include Preprocessing info
		-i         specify Include filename for preprocessing info
		-t#        round transformation values losing # bits (default is to
		           use the "-r" setting)
		-w         don't attempt to fix up seams in texture wrapping
		-b         DEBUG-only; break (INT 3) on Assert
		-d         DEBUG-only; simply dump out the tokens in the source

		Flags come come anywhere in the command line.  They may be combined
		after a single '-' or they may appear separately.  The input script
		file must be listed before the output Chomp script filename.

***************************************************************************/

#include "sitobren.h"
ASSERTNAME

#ifdef DEBUG
bool _fEnableWarnings = fTrue;
bool _fBreak = fFalse;
#endif // DEBUG

int __cdecl main(int cpsz, achar *prgpsz[])
{
	STN stnSrc, stnDst;
	achar **ppszParm = &prgpsz[1];
	bool fBanner = fTrue, fSwapHand, fUsage = fTrue, fDumpLex = fFalse;
	bool fContinue = fFalse, fPreprocess = fFalse, fHaveInc = fFalse;
	bool fIncNext = fFalse, fHaveRoundXF = fFalse, fFixWrap = fTrue;
	uint mdVerbose, iRound = 3, iRet = 1, iRoundXF;
	FNI fniSrc, fniDst, fniInc;
	PFIL pfilSrc = pvNil;
	FILE *pfileDst = pvNil;
	MSSIO *pmssioErr = pvNil, *pmssioDst = pvNil;
	S2B *ps2b = pvNil;
	ulong grfs2b = fs2bNil;

	fSwapHand = fFalse;
	mdVerbose = kmdQuiet;
	if (cpsz == 1)
		goto LUsage;
	while (--cpsz)
		{
		PSZ pszParm = *ppszParm;

		if (fIncNext)
			{
			STN stnT;

			stnT = pszParm;
			fHaveInc = fniInc.FBuildFromPath(&stnT);
			fIncNext = fFalse;
			}
		else if (pszParm[0] == ChLit('-') || pszParm[0] == ChLit('/'))
			{
			int cch = CchSz(pszParm), ich;

			for (ich = 1; ich < cch; ich++)
				{
				achar *pch;
				uint *pi;

				switch (pszParm[ich])
					{
				case ChLit('n'):
					fBanner = fFalse;
					if ((cch - ich >= 6) && FEqualRgb(PszLit("nologo"), &pszParm[ich], 6 * size(achar)))
						ich += 5;
					break;
				case ChLit('h'):
					fSwapHand = !fSwapHand;
					break;
				case ChLit('v'):
					{
					achar ch = pszParm[ich + 1];

					if (ch >= ChLit('0') && ch <= ChLit('9'))
						{
						mdVerbose = min(kmdVerbose, ch - ChLit('0'));
						ich++;
						}
					else
						mdVerbose = kmdHelpful;
					break;
					}
				case ChLit('t'):
					pi = &iRoundXF;
					fHaveRoundXF = fTrue;
					goto LGetRound;
				case ChLit('r'):
					pi = &iRound;
LGetRound:
                    pch = &pszParm[ich + 1];
					*pi = 0;
					while (*pch && (*pch >= ChLit('0') && *pch <= ChLit('9')))
						{
						*pi = *pi * 10 + *pch - ChLit('0');
						ich++, pch++;
						}
					*pi = min(*pi, 15);
					break;
				case ChLit('k'):
					fContinue = !fContinue;
					break;
				case ChLit('p'):
					fPreprocess = !fPreprocess;
					break;
				case ChLit('w'):
					fFixWrap = !fFixWrap;
					break;
				case ChLit('i'):
					fIncNext = fTrue;
					break;
#ifdef DEBUG
				case ChLit('b'):
					_fBreak = !_fBreak;
					break;
				case ChLit('d'):
					fDumpLex = !fDumpLex;
					break;
#endif // DEBUG
				case ChLit('?'):
					goto LUsage;
				default:
					printf("Error: unrecognized option '%c'\n", pszParm[ich]);
					goto LUsage;
					}
				}
			}
		else if (stnSrc.Cch() == 0)
			stnSrc = pszParm;
		else if (stnDst.Cch() == 0)
			stnDst = pszParm;
		else goto LUsage;
		ppszParm++;
		}

	fUsage = fFalse;

	if (fContinue)
		grfs2b |= kfs2bContinue;
	if (fPreprocess)
		grfs2b |= kfs2bPreprocess;
	if (fFixWrap)
		grfs2b |= kfs2bFixWrap;

	if (fBanner)
		{
		printf("\nMicrosoft (R) SoftImage (R) to Chunky File Converter\n");
		printf("Copyright (C) Microsoft Corp 1995. All rights reserved.\n\n");
		}

	if (!fHaveRoundXF)
		iRoundXF = iRound;

	if (mdVerbose)
		{
		if (iRound > 0)
			printf("Rounding off %d binary places\n", iRound);
		if (iRoundXF != iRound)
			printf("Rounding off transformations %d binary places\n", iRoundXF);
#if HASH_FIXED
		printf("Fixed length hash table\n");
#else
		printf("Variable length hash table\n");
#endif
		}

	if (!fniSrc.FBuildFromPath(&stnSrc, kftgS2b))
		{
		printf("Invalid script file name\n");
		goto LUsage;
		}
	else if ((pfilSrc = FIL::PfilOpen(&fniSrc)) == pvNil)
		{
		fniSrc.GetStnPath(&stnSrc);
		printf("Could not open source file (%s)\n", stnSrc.Psz());
		goto LUsage;
		}

	if (fDumpLex)
		{
		STN stn;
		S2BLX s2blx(pfilSrc);
		S2BTK s2btk;
		long cch = 0;

		while (s2blx.FGetS2btk(&s2btk))
			{
			if (!s2blx.FTextFromS2btk(&s2btk, &stn))
				stn.FFormatSz(PszLit("<Token %d>"), s2btk.tok.tt);
			if (cch + stn.Cch() + 1 >= 80)
				{
				printf("\n");
				cch = 0;
				}
			printf("%s ", stn.Psz());
			cch += stn.Cch() + 1;
			}
		fUsage = fFalse;
		goto LUsage;
		}

	if (stnDst.Cch() == 0)
		{
		fniDst = fniSrc;
		AssertDo(fniDst.FChangeFtg(kftgCht), 0);
		}
	else if (!fniDst.FBuildFromPath(&stnDst, kftgCht))
		{
		printf("Invalid destination file name\n");
		goto LUsage;
		}
	fniDst.GetStnPath(&stnDst);
	if ((pfileDst = fopen(stnDst.Psz(), "wt")) == pvNil)
		{
		printf("Could not create destination file (%s)\n", stnDst.Psz());
		goto LUsage;
		}
	if (fPreprocess && !fHaveInc)
		{
		fniInc = fniDst;
		fHaveInc = fniInc.FChangeFtg(kftgInc);
		if (!fHaveInc)
			printf("Warning: couldn't build include file name\n");
		}

	ps2b = S2B::Ps2bNew(pfilSrc, fSwapHand, mdVerbose, iRound, iRoundXF, prgpsz[0]);
	if (ps2b == pvNil)
		goto LOom;

	pmssioErr = NewObj MSSIO(stderr);
	if (pmssioErr == pvNil)
		goto LOom;

	pmssioDst = NewObj MSSIO(pfileDst);
	if (pmssioDst == pvNil)
		{
LOom:
		printf("Error: not enough memory\n");
		}
	else if (ps2b->FConvertSI(pmssioErr, pmssioDst, fHaveInc ? &fniInc : pvNil, grfs2b))
		iRet = 0;

	ReleasePpo(&ps2b);
	ReleasePpo(&pmssioDst);
	ReleasePpo(&pmssioErr);

LUsage:
	ReleasePpo(&pfilSrc);
	Assert(FIL::PfilFirst() == pvNil, "Files left in the file chain");
	if (pfileDst != pvNil)
		fclose(pfileDst);

#ifdef DEBUG
	UnmarkAllMem();
	UnmarkAllObjs();
	MarkUtilMem();		//marks all util memory
	AssertUnmarkedMem();
	AssertUnmarkedObjs();
#endif // DEBUG

	if (fUsage)
		{
		Assert(iRet != 0, "won't return error status");
		printf("Usage:\n    sitobren [-<flag>] <srcScriptFile> "
			"[<dstChunkFile>]\n");
		printf(        "    flags:\n"
					   "        -n[ologo]  don't print banner\n"
					   "        -v[#]      Verbose, optional # 0-2 (1 is default)\n");
		printf(        "        -h         swap z-axis (Handedness)\n"
					   "        -r         don't Round positions\n"
					   "        -r#        Round off # places\n");
		printf(        "        -t#        Round off transformations # places\n"
					   "        -w         don't attempt to fix up seams in texture wrapping\n");
		printf(        "        -k         Kontinue after errors\n"
					   "        -p         include Preprocessing info\n"
					   "        -i         specify Include filename for preprocessing info\n");
		printf(        "        -?         This information\n");
		printf(      "\n    The default extension for the script file is .s2b\n"
					   "    and the default extension for the output file is .cht\n");
		}

	return iRet;
}


RTCLASS(S2B)
RTCLASS(S2BLX)

#ifdef DEBUG
void S2B::AssertValidBmhr(PBMHR pbmhr)
{
	while (pbmhr != pvNil)
		{
		AssertVarMem(pbmhr);
		AssertValidBmhr(pbmhr->pbmhrChild);
		pbmhr = pbmhr->pbmhrSibling;
		}
}

void S2B::AssertValid(ulong grf)
{
	S2B_PAR::AssertValid(grf);
	AssertPo(_ps2blx, grf);
	AssertPo(&_chse, grf);
	AssertNilOrPo(_pglibactPar, grf);
	AssertNilOrPo(_pglbs, grf);
	AssertNilOrPo(_pglcmtld, grf);
	AssertNilOrPo(_pggcl, grf);
	AssertNilOrPo(_pglxf, grf);
	AssertNilOrPo(_pglcrng, grf);
	AssertNilOrPo(_pglclr, grf);
	AssertNilOrPo(_pglibps, grf);
	AssertNilOrPo(_pggcm, grf);
	AssertNilOrPo(_pggtmapd, grf);
#if HASH_FIXED
	AssertNilOrPvCb(_prgpbmdb, kcbrgpbmdb);
#else /* HASH_FIXED */
	AssertNilOrPo(_pglpbmdb, grf);
	AssertNilOrPo(_pglpbmatdb, grf);
#endif /* HASH_FIXED */
	AssertValidBmhr(_pbmhr);
	AssertNilOrPvCb(_prgcps, size(CPS) * _cMesh);
}

void S2BLX::AssertValid(ulong grf)
{
	S2BLX_PAR::AssertValid(grf);
}

void S2B::MarkBmhr(PBMHR pbmhr)
{
	while (pbmhr != pvNil)
		{
		MarkPv(pbmhr);
		MarkPv(pbmhr->pmodlf);
		MarkPv(pbmhr->pszName);
		MarkBmhr(pbmhr->pbmhrChild);
		pbmhr = pbmhr->pbmhrSibling;
		}
}

void S2B::MarkMem(void)
{
	S2B_PAR::MarkMem();
	MarkMemObj(_ps2blx);
	MarkMemObj(_pglibactPar);
	MarkMemObj(_pglbs);
	MarkMemObj(_pglcmtld);
	MarkMemObj(_pggcl);
	MarkMemObj(_pglxf);
	MarkMemObj(_pglcrng);
	MarkMemObj(_pglclr);
	MarkMemObj(_pglibps);
	MarkMemObj(_pggcm);
	MarkMemObj(_pggtmapd);
#if HASH_FIXED
	MarkPv(_prgpbmdb);
#else /* HASH_FIXED */
	MarkMemObj(_pglpbmdb);
	MarkMemObj(_pglpbmatdb);
#endif /* !HASH_FIXED */
	MarkPv(_prgcps);
	MarkBmhr(_pbmhr);
	MarkMemObj(&_chse);
}

void S2BLX::MarkMem(void)
{
	S2BLX_PAR::MarkMem();
}
#endif // DEBUG

/*-----------------------------------------------------------------------------
|	Ps2bNew
|		Static instantiator for the S2B class.  Will attempt to allocate the
|	S2BLX member and will fail if it cannot.
|	
|	Arguments:
|		PFIL pfilSrc    -- file to read script from
|		bool fSwapHand  -- the rest are all passed to the S2B constructor
|		bool mdVerbose
|		int iRound
|		PSZ pszApp
|	
|	Returns: a pointer to the new S2B instance, pvNil if it fails
|	
-------------------------------------------------------------PETED-----------*/
PS2B S2B::Ps2bNew(PFIL pfilSrc, bool fSwapHand, uint mdVerbose, int iRound, int iRoundXF, PSZ pszApp)
{
	PS2B ps2b = NewObj S2B(fSwapHand, mdVerbose, iRound, iRoundXF, pszApp);

	if (ps2b == pvNil)
		return pvNil;

	ps2b->_ps2blx = NewObj S2BLX(pfilSrc);
	if (ps2b->_ps2blx == pvNil)
		ReleasePpo(&ps2b);

	return ps2b;
}

/*-----------------------------------------------------------------------------
|	S2B
|		Constructor for the S2B class.  Initializes the S2BLX and sets
|	some pointers to pvNil.
|	
|	Arguments:
|		PFIL pfilSrc -- used to initialize the S2BLX
|	
-------------------------------------------------------------PETED-----------*/
S2B::S2B(bool fSwapHand, uint mdVerbose, int iRound, int iRoundXF, PSZ pszApp)
{
	short bo;
	FNI fni;
	CFL *pcfl;
	BLCK blck;
	STN stnPal;

	_ibpCur = 0;
	_chidActn = 0;
	_chidBmdl = 0;
	_chidCmtl = 0;
	_pglibactPar = pvNil;
	_pglbs = pvNil;
	_pglcmtld = pvNil;
	_pggcl = pvNil;
#if HASH_FIXED
	_prgpbmdb = pvNil;
#else /* HASH_FIXED */
	_pglpbmdb = pvNil;
	_pglpbmatdb = pvNil;
#endif /* !HASH_FIXED */
	_InitCrcTable();
	_pbmhr = pvNil;
	_pglxf = pvNil;
	_pglibps = pvNil;
	_pggcm = pvNil;
	_pggtmapd = pvNil;
	_iZsign = fSwapHand ? -1 : 1;
	_mdVerbose = mdVerbose;
	Assert(_mdVerbose == mdVerbose, "mdVerbose overflow");
	_uRound = iRound;
	_uRoundXF = iRoundXF;
	_mdBPS = kmdbpsAllMesh;
	_fColorOnly = fFalse;
	_cnoCur = cnoNil;

	_pglclr = _pglcrng = pvNil;
	stnPal = pszApp;
	AssertDo(fni.FBuildFromPath(&stnPal), "Couldn't build .chk path?");
	AssertDo(fni.FChangeFtg(kftgContent), "Couldn't change FTG?");
	fni.GetLeaf(&stnPal);
	if (fni.FSearchInPath(&stnPal) &&
			(pcfl = CFL::PcflOpen(&fni, fcflNil)) != pvNil)
		{
		if (pcfl->FFind(kctgColorTable, 0, &blck))
			{
			_pglclr = GL::PglRead(&blck, &bo);
			if (_pglclr != pvNil && bo == kboCur)
				{
				KID kidGlcg;

				if (pcfl->FGetKidChidCtg(kctgColorTable, 0, 0, kctgGlcg, &kidGlcg) &&
						pcfl->FFind(kidGlcg.cki.ctg, kidGlcg.cki.cno, &blck))
					{
					_pglcrng = GL::PglRead(&blck, &bo);
					Assert(bo == kboCur, "GLCG byte-order not same as GLCR");
					}
				else
					_pglcrng = PglcrngFromPal(_pglclr);
				}
			else
				printf("Unable to read GLCR chunk\n");
			}
		else
			printf("Couldn't find GLCR chunk\n");
		ReleasePpo(&pcfl);
		}
	else
		{
		if (fni.Ftg() != ftgNil)
			fni.GetStnPath(&stnPal);
		printf("Couldn't find my resource file (%s);"
				"no color information available\n", stnPal.Psz());
		}
}

/*-----------------------------------------------------------------------------
|	~S2B
|		Destructor for the S2B class.  Simply asserts that things have
|	already been cleaned up properly.
|	
-------------------------------------------------------------PETED-----------*/
S2B::~S2B(void)
{
#ifdef DEBUG
	UnmarkAllMem();
	UnmarkAllObjs();
	MarkMem();			//marks all S2B memory
	MarkUtilMem();		//marks all util memory
	AssertUnmarkedMem();
	AssertUnmarkedObjs();
#endif // DEBUG

	// All allocated objects should already be free
	Assert(_pglibactPar == pvNil, 0);
	Assert(_pglbs == pvNil, 0);
	Assert(_pglcmtld == pvNil, 0);
	Assert(_pggcl == pvNil, 0);
	Assert(_pglxf == pvNil, 0);
	Assert(_pglibps == pvNil, 0);
	Assert(_pggcm == pvNil, 0);
	Assert(_pggtmapd == pvNil, 0);
#if HASH_FIXED
	Assert(_prgpbmdb == pvNil, 0);
#else /* HASH_FIXED */
	Assert(_pglpbmdb == pvNil, 0);
	Assert(_pglpbmatdb == pvNil, 0);
#endif /* !HASH_FIXED */
	Assert(_prgcps == pvNil, 0);
	Assert(_pbmhr == pvNil, 0);
	// Except these
	ReleasePpo(&_pglcrng);
	ReleasePpo(&_pglclr);
	ReleasePpo(&_ps2blx);
}


/******************************************************************************
	_DumpHeader
		Dumps a customized "#ifdef"'d header for a chunk.  This allows
		sitobren-generate .cht files to only keep chunks named for those
		that need names in the final product.
	
	Arguments:
		CTG ctg       -- the CTG of the chunk; used in the #ifdef
		CNO cno       -- the remainder of the arguments are just passed
		PSTN pstnName    directly to the CHSE DumpHeader()
		bool fPack
	
************************************************************ PETED ***********/
void S2B::_DumpHeader(CTG ctg, CNO cno, PSTN pstnName, bool fPack)
{
	if (_fPreprocess)
		{
		long ich;
		STN stnCtg;
		STN stnT;

		stnCtg.FFormatSz(PszLit("%f"), ctg);
		ich = 0;
		while (ich < stnCtg.Cch())
			{
			achar ch;

			ch = stnCtg.Psz()[ich];
			if ((ch >= ChLit('A') && ch <= ChLit('Z')) ||
					ch >= ChLit('a') && ch <= ChLit('z') ||
					ch >= ChLit('0') && ch <= ChLit('9'))
				{
				ich++;
				continue;
				}
			stnCtg.Delete(ich, 1);
			}

		if (stnCtg.Cch() > 0)
			{
			stnT.FFormatSz(
				PszLit("#if !defined(DEBUG) && !defined(%s_NAMED)"), &stnCtg);
			_chse.DumpSz(stnT.Psz());
			_chse.DumpHeader(ctg, cno, pvNil, fPack);
			_chse.DumpSz(PszLit("#else"));
			_chse.DumpHeader(ctg, cno, pstnName, fPack);
			_chse.DumpSz(PszLit("#endif"));

			return;
			}

		printf("Warning: CTG w/out any alphanumerics; preprocessor directives"
			" skipped for that chunk\n");
		}

	_chse.DumpHeader(ctg, cno, pstnName, fPack);
}


/*-----------------------------------------------------------------------------
|	FConvertSI
|		Reads commands from the current _s2blx file, and generates an
|	appropriate Chunky source file.
|	
|	Arguments:
|		PMSNK pmsnkErr -- where to send error messages
|		PMSNK pmsnkDst -- where to actually send the source file
|	
|	Returns:
|		fTrue if it succeeds, fFalse otherwise
|	
-------------------------------------------------------------PETED-----------*/
bool S2B::FConvertSI(PMSNK pmsnkErr, PMSNK pmsnkDst, PFNI pfniInc, ulong grfs2b)
{
	bool fGotTok, fHaveActor = fFalse, fRet = fFalse;

	_fContinue = FPure(grfs2b & kfs2bContinue);
	_fPreprocess = FPure(grfs2b & kfs2bPreprocess);
	_fFixWrap = FPure(grfs2b & kfs2bFixWrap);

	if (_fFixWrap)
		BrBegin();

	_chse.Init(pmsnkDst, pmsnkErr);

	if (_fPreprocess && pfniInc != pvNil)
		{
		STN stnT;

		pfniInc->GetStnPath(&stnT);
		_stnT.FFormatSz(PszLit("#include \"%s\""), &stnT);
		_chse.DumpSz(_stnT.Psz());
		}

	_chse.DumpSz(PszLit("BYTE"));
	_chse.DumpSz(PszLit(""));

#if HASH_FIXED
	if (!FAllocPv((void **)&_prgpbmdb, kcbrgpbmdb, fmemClear, mprNormal))
#else /* HASH_FIXED */
	if ((_pglpbmatdb = GL::PglNew(size(PBMATDB))) == pvNil)
		goto LFail;
	if ((_pglpbmdb = GL::PglNew(size(PBMDB))) == pvNil)
#endif /* !HASH_FIXED */
		goto LFail;

	fGotTok = _ps2blx->FGetS2btk(&_s2btk);
	while (fGotTok)
		{
		switch (_s2btk.tok.tt)
			{
		case ttActor:
			fGotTok = _FDoTtActor(&fHaveActor);
			break;
		case ttActionS2B:
			if (fHaveActor)
				fGotTok = _FDoTtActionS2B();
			else
				{
				printf("Error: found 'action' token before 'actor' definition\n");
				goto LContinue;
				}
			break;
		case ttBackgroundS2B:
			fGotTok = _FDoTtBackgroundS2B();
			break;
		case ttCostume:
			if (fHaveActor)
				fGotTok = _FDoTtCostume();
			else
				{
				printf("Error: found 'costume' token before 'actor' definition\n");
				goto LContinue;
				}
			break;
		default:
			{
			STN stnTok;

			printf("Unexpected token at line %d, character %d: \n",
				_ps2blx->LwLine(), _ps2blx->IchLine());
			if (_ps2blx->FTextFromS2btk(&_s2btk, &stnTok))
				printf("    %s\n", stnTok.Psz());
			else
				printf("    %08x\n", _s2btk.tok.tt);
			}
LContinue:
			fGotTok = _ps2blx->FGetS2btk(&_s2btk);
			break;
			}
		}

	_FlushTmplKids();

#if HASH_FIXED
#ifdef DEBUG
	{
	PBMDB *ppbmdb;

	for (ppbmdb = _prgpbmdb; ppbmdb < (_prgpbmdb + kcpbmdb); ppbmdb++)
		Assert(*ppbmdb == pvNil, "Didn't empty BMDL database!");
	}
#endif // DEBUG
	FreePpv((void **)&_prgpbmdb);
#else /* HASH_FIXED */
	Assert(_pglpbmdb->IvMac() == 0, "Didn't empty BMDL database");
	ReleasePpo(&_pglpbmdb);
	Assert(_pglpbmatdb->IvMac() == 0, "Didn't empty XF database");
	ReleasePpo(&_pglpbmatdb);
#endif /* !HASH_FIXED */

LFail:
	if (_fFixWrap)
		BrEnd();
	return fRet;
}


/******************************************************************************
	_FReadCmdline
		General purpose script command reader.  With an array of parameter
	descriptions, and pointers to the appropriate variables to take the
	results, this will read the specific parameters, putting the appropriate
	data into the appropriate variable for the given token.
	
	Arguments:
		PSZ pszResult       -- Error string for command
		bool *pfGotTok      -- pointer to flag to set based on lex state
		const SCRP rgscrp[] -- description of command parameters
		...                 -- Additional variable parameters
	
	Returns: fTrue if all required parameters were found, and all parameters
		were successfully read.  If a token could be pre-fetched from the
		lexer, *pfGotTok is set to fTrue, otherwise it's set to fFalse.

************************************************************ PETED ***********/
bool S2B::_FReadCmdline(PSZ pszResult, bool *pfGotTok, const SCRP rgscrp[], ...)
{
	AssertVarMem(pfGotTok);
	Assert(kcscrpMax <= size(long) * 8, "Too many parms");

	va_list args;
	bool fRet = fFalse;
	long cscrpMac = 0, iscrp, grfFound = 0, grfMatch = 0;
	void *rgpv[kcscrpMax];

	/* Grab the arguments */
	va_start(args, rgscrp);
	while (rgscrp[cscrpMac].pt != ptNil)
		{
		Assert(rgscrp[cscrpMac].tt != ttNil, "Bad RGSCRP");
		if (rgscrp[cscrpMac].szErr[0] != 0)
			grfMatch |= (0x01 << cscrpMac);
		rgpv[cscrpMac] = va_arg(args, void *);
		cscrpMac++;
		}
	va_end(args);

	/* Try to read the data, quit when we get an unknown token */
	while (*pfGotTok = _ps2blx->FGetS2btk(&_s2btk))
		{
		for (iscrp = 0; iscrp < cscrpMac; iscrp++)
			if (rgscrp[iscrp].tt == _s2btk.tok.tt)
				break;
		if (iscrp == cscrpMac)
			break;

		if (!(*pfGotTok = _ps2blx->FGetS2btk(&_s2btk)))
			goto LDone;
		switch (rgscrp[iscrp].pt)
			{
		case ptString:
			if (_s2btk.tok.tt != ttString)
				{
				printf("Error: expected string\n");
				goto LDone;
				}
			*((PSTN)rgpv[iscrp]) = _s2btk.tok.stn;
			break;
		case ptLong:
			if (_s2btk.tok.tt != ttLong)
				{
				printf("Error: expected integer\n");
				goto LDone;
				}
			*((long *)rgpv[iscrp]) = _s2btk.tok.lw;
			break;
		case ptBRS:
			if (!_FBrsFromS2btk(&_s2btk, (BRS *)rgpv[iscrp]))
				{
				printf("Error: expected integer or floating point\n");
				goto LDone;
				}
			break;
		case ptBRA:
			if (_s2btk.tok.tt != ttLong)
				{
				printf("Error: expected degrees as an integer\n");
				goto LDone;
				}
			if (_s2btk.tok.lw >= 360 || _s2btk.tok.lw < 0)
				_s2btk.tok.lw %= 360;
			*((BRA *)rgpv[iscrp]) = BrDegreeToAngle(BrIntToFixed(_s2btk.tok.lw));
			break;
			}
		grfFound |= ((0x01 << iscrp) & grfMatch);
		}

	if (!(fRet = (grfFound == grfMatch)))
		{
		for (iscrp = 0, grfMatch ^= grfFound;
				iscrp < cscrpMac && !(grfMatch & 0x01);
				iscrp++, grfMatch >>= 0x01)
			;
		Assert(iscrp < cscrpMac, "Inconsistent error info");
		printf("Error: %s; %s\n", rgscrp[iscrp].szErr, pszResult);
		}
LDone:
	return fRet;
}

const SCRP rgscrpActor[] =
	{
		{ ptLong, ttCno, "missing CNO for actor" },
		{ ptString, ttCalled, "missing name for actor" },
		{ ptBRA, ttXRest, "" },
		{ ptBRA, ttYRest, "" },
		{ ptBRA, ttZRest, "" },
		{ ptLong, ttFlags, "" },
		{ ptLong, ttBPS, ""},
		{ ptLong, ttMaterials, "" },
		{ ptNil, ttNil, "" }
	};

/*-----------------------------------------------------------------------------
|	_FDoTtActor
|		Processes a token of type ttActor.  That is, the ttActor
|	indicates the start of a new TMPL to create.  This handles all of
|	the tokens for that command, and prefetches the next token following
|	all data for this command.
|	
|	Returns:
|		fTrue if another token is available, fFalse otherwise.
|	
-------------------------------------------------------------PETED-----------*/
bool S2B::_FDoTtActor(bool *pfHaveActor)
{
	bool fRet = fTrue;
	uint mdBPS = _mdBPS, mdMaterials = 2;
	CNO cno;
	TMPLF tmplf;

	_FlushTmplKids();

	tmplf.bo = kboCur;
	tmplf.osk = koskCur;
	tmplf.xaRest = tmplf.yaRest = tmplf.zaRest = 0;
	tmplf.grftmpl = 0;

	if (!_FReadCmdline("actor ignored", &fRet, rgscrpActor, &cno, &_stnTmpl,
			&tmplf.xaRest, &tmplf.yaRest, &tmplf.zaRest, &tmplf.grftmpl,
			&mdBPS, &mdMaterials))
		goto LFail1;

	if (mdBPS < kmdbpsNone || mdBPS >= kmdbpsLim)
		{
		printf("Error: body part sets state out of range [%d-%d]\n",
			kmdbpsNone, kmdbpsLim-1);
		goto LFail;
		}
	_mdBPS = mdBPS;

	_fColorOnly = (mdMaterials < 2);

	if (_mdVerbose > kmdQuiet)
		{
		printf("New actor: %s\n", _stnTmpl.Psz());
		switch (_mdBPS)
			{
		case kmdbpsNone:
			printf("Body part sets will not be generated\n");
			break;
		case kmdbpsNumberedOnly:
			printf("Only nodes with body part sets will be included\n");
			break;
		case kmdbpsAllMesh:
			printf("All mesh nodes, and null nodes with a body part set, will be included\n");
			break;
		case kmdbpsAllMeshNull:
			printf("All mesh and null nodes will be included\n");
			break;
			}
		}

	_ctgPar = kctgTmpl;
	_cnoPar = cno;
	if (_cnoCur == cnoNil)
		_cnoCur = cno;
	_DumpHeader(kctgTmpl, cno, &_stnTmpl, fTrue);
	_chse.DumpRgb(&tmplf, size(tmplf));
	_chse.DumpSz(PszLit("ENDCHUNK"));
	_chse.DumpSz(PszLit(""));

	*pfHaveActor = fTrue;
LFail:
	if (!*pfHaveActor)
		{
		printf("Error occured during processing for command line:\n");
		printf("    ACTOR CNO %d, NAMED \"%s\",\n"
		       "        XREST %f, YREST %f, ZREST %f FLAGS %08x, BPSETS %d\n",
			   cno, _stnTmpl.Psz(),
			   BrScalarToFloat(BrAngleToDegree(tmplf.xaRest)),
			   BrScalarToFloat(BrAngleToDegree(tmplf.yaRest)),
			   BrScalarToFloat(BrAngleToDegree(tmplf.zaRest)),
			   tmplf.grftmpl, mdBPS);
		}
LFail1:
	return fRet;
}

const SCRP rgscrpAction[] =
	{
		{ ptString, ttCalled, "missing name for action" },
		{ ptString, ttFilebase, "missing filename for action" },
		{ ptLong, ttFirst, "missing starting cel for action" },
		{ ptLong, ttLast, "missing ending cel for action" },
		{ ptLong, ttFlags, "missing flags for action" },
		{ ptBRS, ttScale, "" },
		{ ptLong, ttSkip, "" },
		{ ptString, ttSubmodel, "" },
		{ ptBRS, ttStep, "" },
		{ ptNil, ttNil, "" }
	};

/*-----------------------------------------------------------------------------
|	_FDoTtActionS2B
|		Processes a token of type ttNameS2B.  That is, the ttNameS2B
|	indicates the start of a new ACTN to create.  This handles all of
|	the tokens for that command, and prefetches the next token following
|	all data for this command.
|	
|	Returns:
|		fTrue if another token is available, fFalse otherwise.
|	
-------------------------------------------------------------PETED-----------*/
bool S2B::_FDoTtActionS2B(void)
{
	bool fRet = fTrue, fSuccess = fTrue;
	STN stnFileBase, stnFileAddon, stnSubmodel;
	int cCel, iCel, iCelBase, dCel = 1, iCelMac;
	long grfactn;
	ACTNF actnf;
	CNO cnoActn;
	BRS brsScale = BrFloatToScalar(1.0), brsStep = BR_SCALAR(-1);
	BRS rgbrsDwr[3] = {BR_SCALAR(0), BR_SCALAR(0), BR_SCALAR(0)};

	/* Read in all of the command data */
	if (!_FReadCmdline("action ignored", &fRet, rgscrpAction, &_stnActn,
			&stnFileBase, &iCelBase, &iCelMac, &grfactn, &brsScale, &dCel,
			&stnSubmodel, &brsStep))
		goto LFail;

	if (dCel < 1)
		{
		printf("Error: non-positive increment for cel files (%d)\n", dCel);
		goto LFail;
		}

	cCel = iCelMac - iCelBase + 1;
	if (!(grfactn & factnStatic))
		{
		if (cCel == 1)
			printf("Warning: single-cel non-static action\n");
		if (brsStep != BR_SCALAR(-1))
			printf("Warning: step size parameter ignored for non-static action\n");
		}
	else if (brsStep == BR_SCALAR(-1))
		brsStep = BR_SCALAR(5.0);

	/* Create the ACTN chunk itself */
	actnf.bo = kboCur;
	actnf.osk = koskCur;
	actnf.grfactn = grfactn;
	cnoActn = CnoNext();

	_stnT = _stnActn;
	_DumpHeader(kctgActn, cnoActn, &_stnT, fTrue);
	_chse.DumpParentCmd(_ctgPar, _cnoPar, _chidActn++);
	_chse.DumpRgb(&actnf, size(actnf));
	_chse.DumpSz(PszLit("ENDCHUNK"));
	_chse.DumpSz(PszLit(""));

	/* Process the model data files for this action */
	for (iCel = 0; iCel < cCel && (fSuccess || _fContinue); iCel += dCel)
		{
		Model *pmodel;
		BMAT34 bmat34;
		CEL cel;
		long cbrgcps;

		/* Get the SoftImage data */
		AssertDo(stnFileAddon.FFormatSz(PszLit("-%d.hrc"), iCel + iCelBase), 0);
		_stnT = stnFileBase;
		if (!_stnT.FAppendStn(&stnFileAddon))
			{
			printf("Error: Computed filename too long (%s%s)\n", _stnT.Psz(),
				stnFileAddon.Psz());
			goto LFail;
			}
		if ((pmodel = DK_modelReadFile(_stnT.Psz())) == pvNil)
			{
			printf("Error: Can't read cel file (%s)\n", _stnT.Psz());
			goto LFail;
			}

		/* Convert the SoftImage data to Brender data */

		/* Determine step size for this cel */
		cel.chidSnd = 0;
		if (grfactn & factnStatic)
			cel.dwr = brsStep;
		else
			cel.dwr = _BrsdwrFromModel(pmodel, rgbrsDwr);
		if (cel.dwr == BR_SCALAR(0))
			cel.dwr = BrFloatToScalar(0.01);
        cel.dwr = BR_MUL(brsScale, cel.dwr);

		/* Actually process the model hierarchy */
		BrMatrix34Identity(&bmat34);
		_cFace = _cMesh = 0;

		if (!_FInitGlpiCost(fFalse))
			{
			Assert(_pbmhr == pvNil, "Bad error state");
			Assert(_pglibactPar == pvNil, "Bad error state");
			Assert(_pglbs == pvNil, "Bad error state");
			Assert(_pglcmtld == pvNil, "Bad error state");
			goto LFail;
			}
		if (_fMakeCostume)
			printf("Warning: costume info will be extracted from ACTION\n");

		fSuccess = _FProcessModel(pmodel, bmat34, &_pbmhr, &stnSubmodel) &&
			_FEnsureOneRoot(&_pbmhr);
		DK_modelDispose(&pmodel);
		if (!fSuccess)
			{
			printf("Error: processing cel heirarchy (%s)\n", _stnT.Psz());
			if (!_fContinue)
				goto LFail;
			else
				continue;
			}

		/* Create GGCL and GLXF for action */

		/* Also, generate GLPI if necessary, and accumulate BMDLs */
		if (_mdVerbose > kmdQuiet)
			{
			printf("Distance from previous cel is %5.2f\n", BrScalarToFloat(cel.dwr));
			if (_mdVerbose > kmdHelpful || (iCel + dCel >= cCel))
				printf("Found %d mesh nodes, totalling %d polygons\n", _cMesh, _cFace);
			}
		cbrgcps = size(CPS) * _cMesh;
		fSuccess = fFalse;

		if (FAllocPv((void **)&_prgcps, cbrgcps, fmemNil, mprNormal))
			{
			if ((_pglxf != pvNil) || (_pglxf = GL::PglNew(size(BMAT34))) != pvNil)
				{
				_ibpCur = 0;
				if (_FProcessBmhr(&_pbmhr))
					{
					if (_pggcl != pvNil || (_pggcl = GG::PggNew(size(CEL))) != pvNil)
						fSuccess = _pggcl->FAdd(cbrgcps, pvNil, _prgcps, &cel);
					}
				}
			else
				printf("Couldn't create GLXF -- OOM\n");
			FreePpv((void **)&_prgcps);
			}
		else
			printf("Couldn't create CPS array -- OOM\n");

		if (!fSuccess)
			{
			_DisposeBmhr(&_pbmhr);
			if (_fMakeGlpi)
				{
				ReleasePpo(&_pglibactPar);
				ReleasePpo(&_pglbs);
				}
			}
#ifdef DEBUG
		else
			Assert(_pbmhr == pvNil, "Didn't free model hierarchy");
#endif // DEBUG
		}

	ReleasePpo(&_pglcmtld);
	if (fSuccess)
		{
		/* Write out the GGCL and GLXF for the action*/
		CnoNext();
		_stnT.FFormatSz(PszLit("%s %s Cels"), &_stnTmpl, &_stnActn);
		_DumpHeader(kctgGgcl, _cnoCur, &_stnT, fTrue);
		_chse.DumpParentCmd(kctgActn, cnoActn, 0);
		_chse.DumpGroup(_pggcl);
		_chse.DumpSz(PszLit("ENDCHUNK"));
		_chse.DumpSz(PszLit(""));
		ReleasePpo(&_pggcl);
	
		CnoNext();
		_stnT.FFormatSz(PszLit("%s %s Transforms"), &_stnTmpl, &_stnActn);
		_DumpHeader(kctgGlxf, _cnoCur, &_stnT, fTrue);
		_chse.DumpParentCmd(kctgActn, cnoActn, 0);
		_chse.DumpList(_pglxf);
		_chse.DumpSz(PszLit("ENDCHUNK"));
		_chse.DumpSz(PszLit(""));
		ReleasePpo(&_pglxf);
		}

LFail:

#if !HASH_FIXED
	/* Empty the XF database */
	{
	PBMATDB pbmatdb, pbmatdbNext;
	long ipbmatdb = _pglpbmatdb->IvMac();
	long cChain = 0, cEntriesTot = 0, cEntries;

	if (_mdVerbose > kmdQuiet)
		printf("Emptying XF database\n");
	while (ipbmatdb > 0)
		{
		ipbmatdb--;
		_pglpbmatdb->Get(ipbmatdb, &pbmatdb);
		cEntries = 0;
		while (pbmatdb != pvNil)
			{
			cEntries++;
			pbmatdbNext = (PBMATDB)pbmatdb->hshdb.phshdbNext;
			FreePpv((void **)&pbmatdb);
			pbmatdb = pbmatdbNext;
			}
		_pglpbmatdb->Delete(ipbmatdb);
		cEntriesTot += cEntries;
		if (_mdVerbose > kmdHelpful)
			printf("XF collision chain had %d entries\n", cEntries);
		cChain++;
		}
	if (_mdVerbose > kmdQuiet && cChain > 0)
		{
		double flAvg = (double)cEntriesTot / (double)cChain;
		printf("Average XF collision chain had %1.2f entries (%d chains)\n", flAvg, cChain);
		}
	Assert(_pglpbmatdb->IvMac() == 0, 0);
	}
#endif /* !HASH_FIXED */

	if (!fSuccess)
		{
		printf("Error occured during processing for command line:\n");
		printf("    ACTION NAMED \"%s\" FILEBASE \"%s\",\n"
		       "        FIRST %d, LAST %d, FLAGS %08x, SCALE %f, SKIP %d,\n"
			   "        SUBMODEL \"%s\", STEP %f\n",
			   _stnActn.Psz(), stnFileBase.Psz(), iCelBase, iCelMac, grfactn,
			   BrScalarToFloat(brsScale), dCel, stnSubmodel.Psz(),
			   BrScalarToFloat(brsStep));
		}
	return fRet;
}


/******************************************************************************
	_FInitGlpiCost
		Routine to setup the GLPI and Costume creation states.  By default,
		if either does not yet exist, we will create them.  The caller can
		override the _fMakeCostume if it wants to create additional costumes,
		by setting the flag to fTrue if it's fFalse when we return from here.

	Arguments:
		fForceCost -- fTrue if we always want to generate a costume

	Returns: fTrue if no fatal errors occurred
	
************************************************************ PETED ***********/
bool S2B::_FInitGlpiCost(bool fForceCost)
{
	_fMakeGlpi = (_pglibactPar == pvNil);
	_fMakeCostume = (_pggcm == pvNil || fForceCost);

	if (_fMakeGlpi)
		{
		Assert(_pglbs == pvNil, "Non-nil body part set");
		if ((_pglibactPar = GL::PglNew(size(short))) == pvNil)
			{
			printf("Error: Couldn't create GLPI -- OOM\n");
			goto LFail;
			}
		if ((_pglbs = GL::PglNew(size(short))) == pvNil)
			{
			printf("Warning: no body part set info -- OOM\n");
			_fMakeCostume = fFalse;
			}
		}

	if (_fMakeCostume)
		{
		Assert(_pglcmtld == pvNil, "Non-nil GLCMTLD");
		if ((_pglcmtld = GL::PglNew(size(CMTLD))) == pvNil)
			goto LFailCost;
		if (_pggcm == pvNil && (_pggcm = GG::PggNew(size(long))) == pvNil)
			{
			ReleasePpo(&_pglcmtld);
LFailCost:
			printf("Warning: no custom material info -- OOM\n");
			_fMakeCostume = fFalse;
			}
		}

	return fTrue;
LFail:
	return fFalse;
}


const SCRP rgscrpBackground[] =
	{
		{ ptLong, ttCno, "missing CNO for background" },
		{ ptString, ttCalled, "missing name for background" },
		{ ptLong, ttLights, "" },
		{ ptLong, ttCameras, "" },
		{ ptLong, ttFirst, "" },
		{ ptLong, ttLength, "" },
		{ ptNil, ttNil, "" }
	};

/******************************************************************************
	_FDoTtBackgroundS2B
		Handles a ttBackground command from the script.  Will generate the
	necessary .Zbmp files (Brender z-buffer), and output appropriate .cht
	data for the given background.
	
	Returns: fTrue if it returns with a token available
	
************************************************************ PETED ***********/
bool S2B::_FDoTtBackgroundS2B(void)
{
	bool fGotTok, fSuccess = fFalse;
	long cLite = 2, cCam = 9, iPalBase = 151, cPal = 95;
	CTG ctgSav;
	CNO cnoBkgd, cnoSav;
	STN stnBkgd;
	BKGDF bkgdf;

	ctgSav = _ctgPar;
	cnoSav = _cnoPar;

	/* Get the command parameters */
	if (!_FReadCmdline("background ignored", &fGotTok, rgscrpBackground,
			&cnoBkgd, &stnBkgd, &cLite, &cCam, &iPalBase, &cPal))
		goto LFail;

	/* Generate the BKGD chunk */
	bkgdf.bo = kboCur;
	bkgdf.osk = koskCur;
	Assert(iPalBase >= 0 && iPalBase <= kbMax, "Palette base out of range");
	bkgdf.bIndexBase = (byte)iPalBase;

	_ctgPar = kctgBkgd;
	_cnoPar = cnoBkgd;
	if (_cnoCur == cnoNil)
		_cnoCur = cnoBkgd;
	_stnT.FFormatSz(PszLit("%s Background"), &stnBkgd);
	_DumpHeader(kctgBkgd, cnoBkgd, &_stnT, fTrue);
	_chse.DumpRgb(&bkgdf, size(bkgdf));
	_chse.DumpSz(PszLit("ENDCHUNK"));
	_chse.DumpSz(PszLit(""));

	/* Create the lights */
	if (!_FDumpLites(cLite, &stnBkgd))
		goto LFail;

	/* Create the cameras */
	if (!_FDumpCameras(cCam, &stnBkgd, iPalBase, cPal))
		goto LFail;

	fSuccess = fTrue;
LFail:
	if (!fSuccess)
		{
		printf("Error occured during processing for command line:\n");
		printf("    BACKGROUND CNO %d, NAMED \"%s\""
		       "        LIGHTS %d, CAMERAS %d, FIRST %d, LENGTH %d\n",
			   cnoBkgd, stnBkgd.Psz(), cLite, cCam, iPalBase, cPal);
		}
	_ctgPar = ctgSav;
	_cnoPar = cnoSav;
	return fGotTok;
}


const SCRP rgscrpCostume[] =
	{
		{ ptString, ttFilename, "missing filename for costume" },
		{ ptLong, ttUseSets, "no body part sets specified" },
		{ ptNil, ttNil, "" }
	};

/******************************************************************************
	_FDoTtCostume
		Handles a COSTUME script command.  Reads the specified .hrc file
		and extracts costume information from the file.
	
	Returns: fTrue if there's a new token available
	
************************************************************ PETED ***********/
bool S2B::_FDoTtCostume(void)
{
	bool fRet, fSuccess = fFalse;
	long ibpsCur;
	Model *pmodel;
	STN stnCostume;
	BMAT34 bmat34;

	/* Read in all of the command data */
	if (!_FReadCmdline("costume ignored", &fRet, rgscrpCostume, &stnCostume, &ibpsCur))
		goto LFail;

	/* Get list of body part sets to pay attention to */
	if ((_pglibps = GL::PglNew(size(long))) == pvNil)
		goto LOOM1;
	if (!_pglibps->FAdd(&ibpsCur))
		goto LOOM1;
	while (fRet && (_s2btk.tok.tt == ttLong))
		{
		if (!_pglibps->FAdd(&_s2btk.tok.lw))
			{
LOOM1:
			printf("Error: can't build list of body part sets -- OOM\n");
			goto LFail;
			}
        fRet = _ps2blx->FGetS2btk(&_s2btk);
		}

	pmodel = DK_modelReadFile(stnCostume.Psz());
	if (pmodel == pvNil)
		{
		printf("Error: Can't read costume file (%s)\n", stnCostume.Psz());
		goto LFail;
		}

	/* Read and process the SoftImage .hrc file */
	if (!_FInitGlpiCost(fTrue))
		goto LFail;
	_fCostumeOnly = fTrue;
	BrMatrix34Identity(&bmat34);
	_cFace = _cMesh = 0;


	BrMatrix34Identity(&bmat34);
	_cFace = _cMesh = 0;

	Assert(_pglibactPar != pvNil, "haven't processed base hierarchy yet");
	fSuccess = _FProcessModel(pmodel, bmat34, &_pbmhr) && _FEnsureOneRoot(&_pbmhr);
	DK_modelDispose(&pmodel);
	if (!fSuccess)
		{
		printf("Error: processing SoftImage data for costume (%s)\n", stnCostume.Psz());
		goto LFail;
		}
	_ibpCur = 0;
    fSuccess = _FProcessBmhr(&_pbmhr);
	if (!fSuccess)
		{
		_DisposeBmhr(&_pbmhr);
		printf("Error: processing hierarchy for costume (%s)\n", stnCostume.Psz());
		goto LFail;
		}
	else
		Assert(_pbmhr == pvNil, "Didn't free model hierarchy");

LFail:
	if (!fSuccess)
		{
		long iibpsMac = _pglibps != pvNil ? _pglibps->IvMac() : 0;
		long cchLine = 16; /* length of "        USE_SETS" */

		printf("Error occured during processing for command line:\n");
		printf("    COSTUME FILE \"%s\",\n", stnCostume.Psz());
		printf("        USE_SETS");
		for (long iibps = 0; iibps < iibpsMac; iibps++)
			{
			long ibps;
			STN stnT;

			_pglibps->Get(iibps, &ibps);
			stnT.FFormatSz(PszLit(", %d"), ibps);
			if (cchLine + stnT.Cch() > 72)
				{
				printf("\n");
				cchLine = 0;
				}
			printf("%s", stnT.Psz());
			cchLine += stnT.Cch();
			}
		printf("\n");
		}
	_fCostumeOnly = fFalse;
	ReleasePpo(&_pglcmtld);
	ReleasePpo(&_pglibps);
	return fRet;
}


/******************************************************************************
	_FDumpLites
		Outputs the appropriate .cht data for light definitions.  Reads
	the necessary SoftImage ASCII light files, converts to Brender data,
	and generates the proper chunk data.
	
	Arguments:
		int cLite     -- the number of lights
		PSTN pstnBkgd -- string containing the background name
	
	Returns: fTrue if it succeeds, fFalse otherwise
	
************************************************************ PETED ***********/
bool S2B::_FDumpLites(int cLite, PSTN pstnBkgd)
{
	bool fRet = fFalse;
	int iLite;
	LITE lite;
	PGL pgllite;

	/* Create a GL of LITEs */
	if ((pgllite = GL::PglNew(size(LITE))) == pvNil)
		{
		printf("Couldn't allocate GLLT\n");
		goto LFail;
		}

	/* Read in the SoftImage lights, adding each to the GL */
	for (iLite = 0; iLite < cLite; iLite++)
		{
		if (!_stnT.FFormatSz(kszLight, pstnBkgd, iLite+1))
			{
			printf("Computed light filename too long (" kszLight ")\n",
				pstnBkgd->Psz(), iLite+1);
			goto LFail;
			}
		BrMatrix34Identity(&lite.bmat34);
		lite.rIntensity = BrIntToScalar(1.0);
		_ReadLite(&_stnT, &lite);
		lite.lt = BR_LIGHT_DIRECT;
		if (!pgllite->FAdd(&lite))
			{
			printf("Couldn't add light to GLLT -- OOM\n");
			goto LFail;
			}
		}

	/* Emit the LITE chunk */
	CnoNext();
	_stnT.FFormatSz(PszLit("%s Lights"), pstnBkgd);
	_DumpHeader(kctgGllt, _cnoCur, &_stnT, fTrue);
	Assert(_ctgPar == kctgBkgd, "Odd parent for GLLT");
	_chse.DumpParentCmd(_ctgPar, _cnoPar, 0);
	_chse.DumpList(pgllite);
	_chse.DumpSz(PszLit("ENDCHUNK"));
	_chse.DumpSz(PszLit(""));

	fRet = fTrue;
LFail:
	ReleasePpo(&pgllite);
	return fRet;
}

/******************************************************************************
	_FDumpCameras
		Outputs the necessary camera chunk data.  Reads in the appropriate
	SoftImage ASCII camera file, converts the data to Brender data, and
	generates the proper chunk data.
	
	Arguments:
		int cCam      -- the number of cameras
		PSTN pstnBkgd -- the name of the background
		int iPalBase  -- the first palette entry used by the background
		int cPal      -- the number of palette entries used by the background
	
	Returns: fTrue if it succeeds, fFalse otherwise
	
************************************************************ PETED ***********/
bool S2B::_FDumpCameras(int cCam, PSTN pstnBkgd, int iPalBase, int cPal)
{
	int iCam;
	long dxp, dyp;
	CAM cam;
	STN stnFile;

	/* Cameras are kept as individual chunks;
		read each file and create the chunk */
	for (iCam = 1; iCam <= cCam; iCam++)
		{
		CNO cnoCam;
		FNI fni;
		PGL pglapos = pvNil;

		/* Get the file */
		if (!stnFile.FFormatSz(kszCam, pstnBkgd, iCam))
			{
			printf("Computed camera filename too long (" kszCam ")\n",
				pstnBkgd->Psz(), iCam);
			goto LFail;
			}
		cam.bo = kboCur;
		cam.osk = koskCur;
		_ReadCam(&stnFile, &cam, &pglapos);

		/* Process camera's background bitmap */
		if (!stnFile.FFormatSz(kszBmp, pstnBkgd, iCam))
			{
			printf("Computed bitmap filename too long (" kszBmp ")\n",
				pstnBkgd->Psz(), iCam);
			goto LFail;
			}
		_ps2blx->GetFni(&fni);
		if (!fni.FSetLeaf(&stnFile, kftgBmp))
			{
			printf("Error: Couldn't create palette filename\n");
			goto LFail;
			}
		fni.GetStnPath(&stnFile);

		/* Only extract the palette once */
		if (iCam == 1)
			{
			PGL pglclr;

			/* Read the palette */
			if (FReadBitmap(&fni, pvNil, &pglclr, &dxp, &dyp, pvNil))
				{
				/* Chop off unneeded data from the palette */
				Assert(iPalBase + cPal <= pglclr->IvMac(),
					"Not enough colors in bitmap");
				if (iPalBase > 0)
					pglclr->Delete(0, iPalBase);
				if (cPal < pglclr->IvMac())
					pglclr->Delete(cPal, pglclr->IvMac() - cPal);

				/* Generate the palette chunk */
				_stnT.FFormatSz(PszLit("%s Palette"), pstnBkgd);
				_DumpHeader(kctgColorTable, _cnoCur, &_stnT, fTrue);
				Assert(_ctgPar == kctgBkgd, "Odd parent for GLCR");
				_chse.DumpParentCmd(_ctgPar, _cnoPar, 0);
				_chse.DumpList(pglclr);
				_chse.DumpSz(PszLit("ENDCHUNK"));
				_chse.DumpSz(PszLit(""));
				ReleasePpo(&pglclr);
				}
			else
				{
				_stnT.FFormatSz(
					PszLit("Error: Couldn't read camera palette (%s)"),
					&stnFile);
				printf("%s\n", _stnT.Psz());
				goto LFail;
				}
			}

		/* Generate the camera chunk */
		cnoCam = CnoNext();
		_stnT.FFormatSz(PszLit("%s Camera %d"), pstnBkgd, iCam);
		_DumpHeader(kctgCam, cnoCam, &_stnT, fTrue);
		Assert(_ctgPar == kctgBkgd, "Odd parent for CAM");
		_chse.DumpParentCmd(_ctgPar, _cnoPar, iCam-1);
		_chse.DumpRgb(&cam, size(cam));
		if (pglapos != pvNil)
			{
			long capos = pglapos->IvMac();
			APOS apos;

			for (long iapos = 0; iapos < capos; iapos++)
				{
				pglapos->Get(iapos, &apos);
				_chse.DumpRgb(&apos, size(APOS));
				}
			ReleasePpo(&pglapos);
			}
		_chse.DumpSz(PszLit("ENDCHUNK"));
		_chse.DumpSz(PszLit(""));

		/* Generate the background chunk */
		CnoNext();
		_stnT.FFormatSz(PszLit("%s Bitmap %d"), pstnBkgd, iCam);
		_DumpHeader(kctgMbmp, _cnoCur, &_stnT, fTrue);
		_chse.DumpParentCmd(kctgCam, cnoCam, 0);
		_chse.DumpBitmapCmd(0, dxp, dyp, &stnFile);
		_chse.DumpSz(PszLit("ENDCHUNK"));
		_chse.DumpSz(PszLit(""));

		/* Generate the z-buffer chunk for the camera */
		if (!_FZbmpFromZpic(pstnBkgd, cnoCam, iCam, dxp, dyp, &cam))
			goto LFail;
		}

	return fTrue;
LFail:
	return fFalse;
}

/******************************************************************************
	_FZbmpFromZpic
		Converts a SoftImage z-buffer data file into Brender z-buffer
	data.  If it can write the data to a new file, it will do so and
	generate a chunk that refers to that file; otherwise, it will include
	the chunk data explicitly.
	
	Arguments:
		PSTN pstnBkgd -- the name of the background
		CNO cnoPar    -- the CNO of the parent camera chunk
		int iCam      -- the number of the camera
		long dxp      -- the width of the background
		long dyp      -- the height of the background
		CAM *pcam     -- pointer to the camera data
	
	Returns: fTrue if it could successfully read and process the z-buffer
		data, fFalse otherwise.
	
************************************************************ PETED ***********/
bool S2B::_FZbmpFromZpic(PSTN pstnBkgd, CNO cnoPar, int iCam,
	long dxp, long dyp, CAM *pcam)
{
	Assert(dxp > 0, "Invalid z-buffer width");
	Assert(dyp > 0, "Invalid z-buffer height");

	bool fRet = fFalse;
	short *prgsw = pvNil;
	float *prgfl = pvNil;
	FNI fni;
	FIL *pfil = pvNil;

	/* Try to find and open the SoftImage data file */
	if (!_stnT.FFormatSz(kszZpic, pstnBkgd, iCam))
		{
		printf("Computed z-buffer filename too long (" kszZpic ")\n",
			pstnBkgd->Psz(), iCam);
		goto LFail;
		}
	_ps2blx->GetFni(&fni);
	if (fni.FSetLeaf(&_stnT, kftgZpic))
		{
		if ((pfil = FIL::PfilOpen(&fni)) != pvNil)
			{
			bool fWroteZbmp = fFalse;
			short *psw, *prgsw;
			long cPix = dxp * dyp, cbSw, cbBuf, cbLeft;
			float fl, *prgfl;
			FNI fniZbmp;
			FP fpZbmp;
			FIL *pfilZbmp = pvNil;
			FP fpRead = 0;
			ZBMPF zbmpf;

			/* Allocate buffer for Zbmp and buffer for reading */
            cbSw = cPix * size(short);
			if (!FAllocPv((void **)&prgsw, cbSw, fmemNil, mprNormal))
				{
				printf("Error: Not enough memory for z-buffer\n");
				goto LFail;
				}
			cbBuf = dxp * size(float);
			if (!FAllocPv((void **)&prgfl, cbBuf, fmemNil, mprNormal))
				{
				prgfl = &fl;
				cbBuf = size(float);
				}

			/* Read the available floats */
			cbLeft = pfil->FpMac();
			psw = prgsw;
			while (cbLeft && cPix)
				{
				long cFl, cbRead;
				float *pfl = prgfl;

				/* Process a buffer's-worth of data */
				cbRead = LwMin(cbBuf, cbLeft);
				cFl = cbRead / size(float);
				Assert(cFl * size(float) == cbRead, "Partial data at EOF");
				AssertDo(pfil->FReadRgbSeq(prgfl, cbRead, &fpRead),
					"Failed z-buffer read");
				SwapBytesRglw(prgfl, cFl);
				cPix -= cFl;
				while (cFl-- > 0)
					{
					BRS brsZCam = BrFloatToScalar(*pfl++);

					/* Convert the SoftImage data to Brender data */
					Assert(*(pfl-1) < 0, "Non-negative zbuffer values");
					if (BR_ABS(brsZCam) > pcam->zrYon)
						*psw++ = -1;
					else
						{
						BRS brsZ, brsDepth = BR_SUB(pcam->zrYon,pcam->zrHither);

						brsZ = BR_ADD(
							BR_MULDIV(brsZCam,
							    BR_ADD(pcam->zrYon,pcam->zrHither), brsDepth),
							BR_CONST_MUL(BR_MULDIV(pcam->zrYon,pcam->zrHither,
							    brsDepth),2));

						*psw++ = (short)(BrScalarToFloat(brsZ) * -32768.0 /
							BrScalarToFloat(-brsZCam));
						Assert(*(psw-1) > 0, "Non-positive calculated value");
						}
					}
				cbLeft -= cbRead;
				}

			/* Fill in the rest with the max value */
			while (cPix-- > 0)
				*psw++ = -1;

			/* Write the chunk */
			CnoNext();
			_stnT.FFormatSz(PszLit("%s Z-Buffer %d"), pstnBkgd, iCam);
			_DumpHeader(kctgZbmp, _cnoCur, &_stnT, fTrue);
			_chse.DumpParentCmd(kctgCam, cnoPar, 0);

			zbmpf.bo = kboCur;
			zbmpf.osk = koskCur;
			zbmpf.xpLeft = zbmpf.ypTop = 0;
			Assert(dxp <= kswMax, "Zbmp too wide");
			Assert(dyp <= kswMax, "Zbmp too tall");
			zbmpf.dxp = (short)dxp;
			zbmpf.dyp = (short)dyp;

			if (_stnT.FFormatSz(kszZbmp, pstnBkgd, iCam))
				{
				_ps2blx->GetFni(&fniZbmp);
				if (fniZbmp.FSetLeaf(&_stnT, kftgZbmp))
					{
					pfilZbmp = FIL::PfilCreate(&fniZbmp);
					fpZbmp = 0;
					if (pfilZbmp != pvNil)
						{
						if (pfilZbmp->FWriteRgbSeq(&zbmpf, size(zbmpf), &fpZbmp) &&
								pfilZbmp->FWriteRgbSeq(prgsw, cbSw, &fpZbmp))
							{
							fWroteZbmp = fTrue;
							fniZbmp.GetStnPath(&_stnT);
							}
						else
							pfilZbmp->SetTemp();
						ReleasePpo(&pfilZbmp);
						}
					}
				}
			if (!fWroteZbmp)
				{
				_chse.DumpRgb(&zbmpf, size(zbmpf));
				_chse.DumpRgb(prgsw, cbSw);
				}
			else
				_chse.DumpFileCmd(&_stnT);
			_chse.DumpSz(PszLit("ENDCHUNK"));
			_chse.DumpSz(PszLit(""));

			/* Prevent us from freeing a local */
			if (prgfl != &fl)
				FreePpv((void **)&prgfl);
			FreePpv((void **)&prgsw);
			ReleasePpo(&pfil);
			}
		else
			{
			printf("Error: Couldn't open z-buffer file\n");
			goto LFail;
			}
		}
	else
		{
		printf("Error: Couldn't build z-buffer filename\n");
		goto LFail;
		}

	fRet = fTrue;
LFail:
	ReleasePpo(&pfil);
	return fRet;
}

/******************************************************************************
	_FBvec3Read
		Reads a vector (triplet) from the given token stream
	
	Arguments:
		PS2BLX ps2blx -- the lexer to use (token stream)
		BVEC3 *pbvec3 -- the vector to fill in
		S2BTK *ps2btk -- the token to use
	
	Returns: fTrue if ptok contains a new unused token
	
************************************************************ PETED ***********/
bool S2B::_FBvec3Read(PS2BLX ps2blx, BVEC3 *pbvec3, S2BTK *ps2btk)
{
	bool fRet;
	int iBrs;

	for (iBrs = 0; iBrs < 3; iBrs++)
		{
		fRet = ps2blx->FGetS2btk(ps2btk);
		if (!fRet || !_FBrsFromS2btk(ps2btk, &pbvec3->v[iBrs]))
			{
			printf("Error: Invalid triplet data\n");
			goto LFail;
			}
		}
LFail:
	return fRet;
}

/******************************************************************************
	_Bmat34FromVec3
		Given a vector, will calculate a transformation matrix that rotates
	the actor's coordinate system so that the positive Z axis points along
	the vector, and the X axis is horizontal.
	
	Arguments:
		BVEC3 *pbvec3   -- The vector to use
		BMAT34 *pbmat34 -- the matrix to fill in
	
************************************************************ PETED ***********/
void S2B::_Bmat34FromVec3(BVEC3 *pbvec3, BMAT34 *pbmat34)
{
	BRS brs, brsAdj, brsHyp;
	BRXFM brxfm;

	/* Set up */
	ClearPb(&brxfm, size(brxfm));
	brxfm.type = BR_TRANSFORM_EULER;
	brxfm.t.euler.e.order = BR_EULER_YXZ_R;

	/* Calculate rotation about Y */
	brsHyp = BR_LENGTH2(pbvec3->v[0], pbvec3->v[2]);
	brs = BrAngleToScalar(BR_ACOS(BR_DIV(pbvec3->v[2], brsHyp)));
	if (pbvec3->v[0] < 0)
		brs = BR_SUB(BR_SCALAR(1), brs);
	Assert(brs >= 0 && BrScalarToFloat(brs) <= 1.0, "Bad angle");
	brxfm.t.euler.e.a = BrScalarToAngle(brs);;

	/* Calculate rotation about X */
	brsAdj = brsHyp;
	brsHyp = BR_LENGTH2(pbvec3->v[1], brsAdj);
	brs = BrAngleToScalar(BR_ACOS(BR_DIV(brsAdj, brsHyp)));
	if (pbvec3->v[1] > 0)
		brs = BR_SUB(BR_SCALAR(1), brs);
	Assert(brs >= 0 && BrScalarToFloat(brs) <= 1.0, "Bad angle");
	brxfm.t.euler.e.b = BrScalarToAngle(brs);

	/* Actually perform the rotations */
	brxfm.t.euler.e.c = BrScalarToAngle(BR_SCALAR(0));
	BrMatrix34Transform(pbmat34, &brxfm);
}

/******************************************************************************
	_ReadLite
		Reads a SoftImage ASCII light file and fills in the LITE structure
	as appropriate.
	
	Arguments:
		PSTN pstnLite -- the name of the light file
		LITE *plite   -- pointer to LITE structure to fill in
	
************************************************************ PETED ***********/
void S2B::_ReadLite(PSTN pstnLite, LITE *plite)
{
	FNI fni;
	FIL *pfil;

	/* Attempt to open the file */
	_ps2blx->GetFni(&fni);
	if (fni.FSetLeaf(pstnLite, kftgALite))
		{
		Assert(fni.Ftg() == kftgALite, "Light file has wrong extension");
		if ((pfil = FIL::PfilOpen(&fni)) != pvNil)
			{
			bool fGotTok;
			BVEC3 bvec3Int = { BR_SCALAR(0), BR_SCALAR(0), BR_SCALAR(0) };
			BVEC3 bvec3Pos;
			S2BLX s2blx(pfil, fFalse);
			S2BTK s2btk;

			/* Parse the data in the file */
			fGotTok = s2blx.FGetS2btk(&s2btk);
			while (fGotTok)
				{
				switch (s2btk.tok.tt)
					{
				case ttPosition:
					if (!(fGotTok = s2blx.FGetS2btk(&s2btk)) ||
							s2btk.tok.tt != ttStatic)
						{
						printf("Error: Expected " DK_A_POS_STATIC_TOKEN
								" after " DK_A_LIGHT_POS_TOKEN "\n");
						goto LFail;
						}

					/* Read the vector */
					if (!_FBvec3Read(&s2blx, &bvec3Pos, &s2btk))
						goto LFail;
					break;
				case ttInterest:
					if (!(fGotTok = s2blx.FGetS2btk(&s2btk)) ||
							s2btk.tok.tt != ttStatic)
						{
						printf("Error: Expected " DK_A_POS_STATIC_TOKEN
								" after " DK_A_LIGHT_INT_TOKEN "\n");
						goto LFail;
						}
					/* Read the vector */
					if (!_FBvec3Read(&s2blx, &bvec3Int, &s2btk))
						goto LFail;

					/* Generate vector for light's +Z axis */
					BrVector3Sub(&bvec3Int, &bvec3Pos, &bvec3Int);
					/* Create light's transformation matrix */
					_Bmat34FromVec3(&bvec3Int, &plite->bmat34);
					BrMatrix34PostTranslate(&plite->bmat34, bvec3Pos.v[0],
						bvec3Pos.v[1], bvec3Pos.v[2]);
					break;
				default:
					break;
					}
				fGotTok = s2blx.FGetS2btk(&s2btk);
				}
			ReleasePpo(&pfil);
			}
		else
			printf("Error: Couldn't open light file\n");
		}
	else
		printf("Error: Couldn't build light filename\n");
LFail:
	return;
}

/******************************************************************************
	_ReadCam
		Reads a SoftImage ASCII camera file and fills in the given CAM
	structure as appropriate.
	
	Arguments:
		PSTN pstnCam -- the name of the camera file
		CAM *pcam    -- pointer to the CAM to fill in
	
************************************************************ PETED ***********/
void S2B::_ReadCam(PSTN pstnCam, CAM *pcam, PGL *ppglapos)
{
	bool fGotActorPos = fFalse;
	FNI fni;
	FIL *pfil;

	/* Attempt to open the camera file */
	_ps2blx->GetFni(&fni);
	if (fni.FSetLeaf(pstnCam, kftgACam))
		{
		Assert(fni.Ftg() == kftgACam, "Camera file has wrong extension");
		if ((pfil = FIL::PfilOpen(&fni)) != pvNil)
			{
			bool fGotTok;
			BVEC3 bvec3Int;
			BVEC3 bvec3Pos;
			S2BLX s2blx(pfil, fFalse);
			S2BTK s2btk;

			/* Parse the data in the camera file */
			fGotTok = s2blx.FGetS2btk(&s2btk);
			while (fGotTok)
				{
				switch (s2btk.tok.tt)
					{
				case ttPosition:
					if (!(fGotTok = s2blx.FGetS2btk(&s2btk)) ||
							s2btk.tok.tt != ttStatic)
						{
						printf("Error: Expected " DK_A_POS_STATIC_TOKEN
								" after " DK_A_CAMERA_POS_TOKEN "\n");
						goto LFail;
						}
					/* Read the position */
					if (!_FBvec3Read(&s2blx, &bvec3Pos, &s2btk))
						goto LFail;
					break;
				case ttInterest:
					if (!(fGotTok = s2blx.FGetS2btk(&s2btk)) ||
							s2btk.tok.tt != ttStatic)
						{
						printf("Error: Expected " DK_A_POS_STATIC_TOKEN
								" after " DK_A_CAMERA_INT_TOKEN "\n");
						goto LFail;
						}
					/* Read the interest (where the camera's looking) */
					if (!_FBvec3Read(&s2blx, &bvec3Int, &s2btk))
						goto LFail;

					/* Create vector for camera's positive X-axis (away from
						where it's looking */
					BrVector3Sub(&bvec3Int, &bvec3Pos, &bvec3Int);
					/* Get transformation for camera */
					_Bmat34FromVec3(&bvec3Int, &pcam->bmat34Cam);
					BrMatrix34PostTranslate(&pcam->bmat34Cam, bvec3Pos.v[0],
						bvec3Pos.v[1], bvec3Pos.v[2]);
					break;
				case ttNearCam: // Near plane
					if (!(fGotTok = s2blx.FGetS2btk(&s2btk)) ||
							!_FBrsFromS2btk(&s2btk, &pcam->zrHither))
						{
						printf("Error: Invalid " DK_A_CAMERA_NEAR_TOKEN " data\n");
						goto LFail;
						}
					break;
				case ttFarCam: // Far plane
					if (!(fGotTok = s2blx.FGetS2btk(&s2btk)) ||
							!_FBrsFromS2btk(&s2btk, &pcam->zrYon))
						{
						printf("Error: Invalid " DK_A_CAMERA_FAR_TOKEN " data\n");
						goto LFail;
						}
					break;
				case ttFovCam: // Field Of View
					{
					BRS brs;

					if (!(fGotTok = s2blx.FGetS2btk(&s2btk)) ||
							!_FBrsFromS2btk(&s2btk, &brs))
						{
						printf("Error: Invalid " DK_A_CAMERA_FOV_TOKEN " data\n");
						goto LFail;
						}
					pcam->aFov = BrRadianToAngle(brs);
					break;
					}
				case ttNewActorPos:
					{
					BVEC3 bvec3Actor;

					if (!_FBvec3Read(&s2blx, &bvec3Actor, &s2btk))
						goto LFail;
					if (!fGotActorPos)
						{
						pcam->apos.bvec3Actor = bvec3Actor;
						fGotActorPos = fTrue;
						}
					else
						{
						if (*ppglapos != pvNil ||
								(*ppglapos = GL::PglNew(size(APOS))) != pvNil)
							{
							APOS apos;

							apos.bvec3Actor = bvec3Actor;
							(*ppglapos)->FAdd(&apos);  // Ignore failure
							}
						}
					break;
					}
				default:
					break;
					}
				fGotTok = s2blx.FGetS2btk(&s2btk);
				}
			ReleasePpo(&pfil);
			if (!fGotActorPos)
				{
				BRS zrCam = kzrDefault;

				/* Ensure depth of location is within the bounds of the cam */
				if (BR_ABS(zrCam) > pcam->zrYon)
					zrCam = BR_CONST_DIV(BR_ADD(pcam->zrYon, pcam->zrHither), -2);

				/* Translate (0,0,zrCam) in cam coords into world coords */
				pcam->apos.bvec3Actor.v[0] = BR_ADD(
					BR_MUL(kzrDefault, pcam->bmat34Cam.m[2][0]),
					pcam->bmat34Cam.m[3][0]);
				pcam->apos.bvec3Actor.v[1] = rZero;
				pcam->apos.bvec3Actor.v[2] = BR_ADD(
					BR_MUL(kzrDefault, pcam->bmat34Cam.m[2][2]),
					pcam->bmat34Cam.m[3][2]);
				}
			}
		else
			printf("Error: Couldn't open camera file\n");
		}
	else
		printf("Error: Couldn't build camera filename\n");
LFail:
	return;
}

/*-----------------------------------------------------------------------------
|	_FProcessModel
|		Converts a SoftImage model hierarchy into a tree of Brender models.
|	Only mesh nodes are saved, but transformation information in propogated
|	to children nodes as necessary.
|	
|	Arguments:
|		Model *pmodel -- points to the root of the model hierarchy to convert
|		BMAT34 bmat34Acc -- transformation matrix to be included in current
|			node's transformation.
|		PBMHR *ppbmhr -- points to the pointer to the root of the Brender
|			model hierarchy.
|		PBMHR pbmhrParent -- points to the actual parent node of this node
|	
|	Returns:
|		fTrue if succeeds, fFalse otherwise (usually due to OOM)
|	
-------------------------------------------------------------PETED-----------*/
bool S2B::_FProcessModel(Model *pmodel, BMAT34 bmat34Acc, PBMHR *ppbmhr,
	PSTN pstnSubmodel, PBMHR pbmhrParent, int cLevel)
{
	bool fRet = fTrue, fSubmodel, fKeepNode;
	BRA braX, braY, braZ;
	BRS brsX, brsY, brsZ;
	Model *pmodelChild = pmodel->child;

	if ((fSubmodel = (pstnSubmodel == pvNil || (pstnSubmodel->Cch() == 0) ||
			pstnSubmodel->FEqualSz(pmodel->name))) != fFalse)
		{
		bool fHaveBPS = fFalse;

		pstnSubmodel = pvNil;
		
		/* Include translation */
		brsX = BrFloatToFixed(pmodel->translation.x);
		brsY = BrFloatToFixed(pmodel->translation.y);
		brsZ = BrFloatToFixed(_iZsign * pmodel->translation.z);
		if (_uRoundXF > 0)
			{
			brsX = BR_ROUND(brsX, _uRoundXF);
			brsY = BR_ROUND(brsY, _uRoundXF);
			brsZ = BR_ROUND(brsZ, _uRoundXF);
			}
		BrMatrix34PreTranslate(&bmat34Acc, brsX, brsY, brsZ);
	
		/* Include rotation */
		braX = BrRadianToAngle(BrFloatToFixed(pmodel->rotation.x));
		braY = BrRadianToAngle(BrFloatToFixed(pmodel->rotation.y));
		braZ = BrRadianToAngle(BrFloatToFixed(pmodel->rotation.z));
		if (_uRoundXF > 0)
			{
			braX = BrScalarToAngle(BR_ROUND(BrAngleToScalar(braX), _uRoundXF));
			braY = BrScalarToAngle(BR_ROUND(BrAngleToScalar(braY), _uRoundXF));
			braZ = BrScalarToAngle(BR_ROUND(BrAngleToScalar(braZ), _uRoundXF));
			}
		BrMatrix34PreRotateZ(&bmat34Acc, braZ);
		BrMatrix34PreRotateY(&bmat34Acc, braY);
		BrMatrix34PreRotateX(&bmat34Acc, braX);
	
		/* Include scaling (comes last for SoftImage) */
		brsX = BrFloatToFixed(pmodel->scaling.x);
		brsY = BrFloatToFixed(pmodel->scaling.y);
		brsZ = BrFloatToFixed(pmodel->scaling.z);
		if (_uRoundXF > 0)
			{
			brsX = BR_ROUND(brsX, _uRoundXF);
			brsY = BR_ROUND(brsY, _uRoundXF);
			brsZ = BR_ROUND(brsZ, _uRoundXF);
			}
		BrMatrix34PreScale(&bmat34Acc, brsX, brsY, brsZ);

		fHaveBPS = CchSz(pmodel->name) > 1 && _ps2blx->FIsDigit(pmodel->name[0])
			&& _ps2blx->FIsDigit(pmodel->name[1]);
		fKeepNode = (pmodel->type==DK_MDL_MESH || pmodel->type==DK_MDL_NILL);
		switch (_mdBPS)
			{
		case kmdbpsNone:
			fHaveBPS = fFalse;
			break;
		case kmdbpsNumberedOnly:
			fKeepNode = fKeepNode && fHaveBPS;
			break;
		case kmdbpsAllMesh:
			fKeepNode = fKeepNode && (fHaveBPS || pmodel->type == DK_MDL_MESH);
			break;
		case kmdbpsAllMeshNull:
			break;
			}
		if (fKeepNode)
			{
			bool fAccessory;
			int ibps;

			if (fHaveBPS)
				{
				fAccessory = (CchSz(pmodel->name) > 4) &&
					(pmodel->name[3] == ChLit('A')) &&
					(pmodel->name[4] == ChLit('C'));
				ibps =  (pmodel->name[0] - ChLit('0')) * 10 +
					pmodel->name[1] - ChLit('0');
				}
			else
				{
				fAccessory = fFalse;
				ibps = 0;
				}
			/* Output info */
			if (_fMakeGlpi && (_mdVerbose > kmdQuiet) || _mdVerbose > kmdHelpful)
				{
				if (cLevel > 0)
					printf("%*c", cLevel * 2, ' ');
				printf("node \"%s\" (body part set %d)\n", pmodel->name, ibps);
				}
			cLevel++;
	
			/* create model node */
			if ((pbmhrParent = _PbmhrFromModel(pmodel, &bmat34Acc, ppbmhr,
					pbmhrParent, ibps, fAccessory)) == pvNil)
				{
				printf("Out of memory allocating model node\n");
				goto LFail;
				}
			pbmhrParent->fAccessory = fAccessory;
	
			/* Anchor this node */
			_cMesh++;
			_cFace += pbmhrParent->pmodlf->cfac;
			ppbmhr = &(pbmhrParent->pbmhrChild);
			BrMatrix34Identity(&bmat34Acc);
			}
		else if (pmodel->type == DK_MDL_MESH && _fMakeGlpi)
			printf("Warning: mesh node \"%s\" has no body part set\n",
				pmodel->name);
		}

	/* Descend and process any children */
	while (pmodelChild != pvNil)
		{
		Assert(*ppbmhr == pvNil, "Non-nil parent pointer");
		if (!_FProcessModel(pmodelChild, bmat34Acc, ppbmhr, pstnSubmodel, pbmhrParent, cLevel))
			goto LFail;
		while (*ppbmhr != pvNil)
			ppbmhr = &((*ppbmhr)->pbmhrSibling);
		pmodelChild = pmodelChild->sibling;
		}

	return fTrue;

	/* Clean up if we fail */
LFail:
	_DisposeBmhr(&_pbmhr);
	return fFalse;
}

/*-----------------------------------------------------------------------------
|	_PbmhrFromModel
|		Converts a single SoftImage model node into a single Brender model
|	node.  Adds the new node as a child to the parent node.
|	
|	Arguments:
|		Model *pmodel -- points to SoftImage model node to convert
|		BMAT34 *pbmat34 -- specifies the transformation for this node
|		PBMHR *ppbmhr -- points to pointer that points to this new child
|		PBMHR pbmhrParent -- points to the actual parent of this new child
|		int ibps -- body part set number
|		bool fAccessory -- fTrue if the body part is an accessory
|	
|	Returns:
|		A pointer to the new node, pvNil if there was some error
|	
-------------------------------------------------------------PETED-----------*/
PBMHR S2B::_PbmhrFromModel(Model *pmodel, BMAT34 *pbmat34, PBMHR *ppbmhr,
	PBMHR pbmhrParent, int ibps, bool fAccessory)
{
	bool fMesh;
	long cb;
	long cbrgver;
	long cbrgfac;
	PBMHR pbmhrCur = pvNil;
	MODLF *pmodlf;
	Mesh *pmesh = (Mesh *)pmodel->definition;
	Material *pmaterial;
	CRNG crng;
	BRCLR brclr;

	/* Ignore geometry if it doesn't exist, or if we're only interested in
		the material information.  Theoretically, we should really just not
		bother with the modlf at all if we're only making the costume, but
		we gain back most of the overhead with this simple change. */
	fMesh = pmesh != pvNil && (!_fCostumeOnly || fAccessory);

	/* Allocate necessary pieces: BMHR, appropriately sized MODLF, and name */
	if (fMesh)
		{
		Assert(pmodel->type == DK_MDL_MESH, "Data present in non-mesh model");
		cbrgver = LwMul(pmesh->nbVertices, size(br_vertex));
		cbrgfac = LwMul(pmesh->nbPolygons, size(br_face));
		cb = size(MODLF) + cbrgver + cbrgfac;
		}
	else
		{
		Assert(_fCostumeOnly || pmodel->type != DK_MDL_MESH, "Mesh model has no data");
		cb = size(MODLF);
		}
	if (!FAllocPv((void **)&pmodlf, cb, fmemClear, mprNormal))
		{
		printf("Couldn't allocate MODLF structure\n");
		goto LFail;
		}

	if (!FAllocPv((void **)&pbmhrCur, size(BMHR), fmemClear, mprNormal))
		{
		printf("Couldn't allocate Brender hierarchy node\n");
		FreePpv((void **)&pmodlf);
		goto LFail;
		}

	if (FAllocPv((void **)&pbmhrCur->pszName,
			CchTotSz(pmodel->name) * size(achar), fmemNil, mprNormal))
		{
		CopyPb(pmodel->name, pbmhrCur->pszName,
			CchTotSz(pmodel->name) * size(achar));
		}
#ifdef DEBUG
	else
		Assert(pbmhrCur->pszName == pvNil, "Bad pointer");
#endif // DEBUG

	pbmhrCur->pmodlf = pmodlf;
	_InitBmhr(pbmhrCur);

#ifdef DEBUG
	/* Just to be nice, keep siblings in same order as in SoftImage data */
	/* This housework is done in FProcessModel now so as to avoid iterating
		over the sibling chain over and over */
	Assert(*ppbmhr == pvNil, "Improper call to _PbmhrFromModel");
	while (*ppbmhr != pvNil)
		ppbmhr = &((*ppbmhr)->pbmhrSibling);
#endif // DEBUG

	/* Fill in MODLF */
	if (fMesh)
		{
		pmodlf->cver = (short)pmesh->nbVertices;
		pmodlf->cfac = (short)pmesh->nbPolygons;
		_CopyVertices(pmesh->vertices,
			PvAddBv(pmodlf, size(MODLF)), pmesh->nbVertices);
		_CopyFaces(pmesh->polygons,
			PvAddBv(pmodlf, size(MODLF) + cbrgver), pmesh->nbPolygons,
			(BRV *)PvAddBv(pmodlf, size(MODLF)), pmodlf->cver);

		/* I considered having the _Copy... routines do the hashing, since
			they're already stepping through the bytes.  However, they do
			such little pieces at a time, I believe that the function-call
			overhead would obliterate any savings from doing so. */
		}
	else
		pmodlf->cver = pmodlf->cfac = 0;

	/* Fill in BMHR */
	pbmhrCur->pmodlf = pmodlf;
	pbmhrCur->cbModlf = cb;
	pbmhrCur->bmat34 = *pbmat34;
	pbmhrCur->ibps = ibps;
	pbmhrCur->fFixWrap = fFalse;

	/* Handle material information */
	pmaterial = pmodel->materials;
	if (pmaterial != pvNil)
		_TextureFileFromModel(pmodel, pbmhrCur, !_fMakeCostume || _pglclr == pvNil);

	if (_fMakeCostume && _pglclr != pvNil)
		{
		Assert(pmodel->currentMaterial == 0, "Using the wrong material");
		/* Get material from this model, if it exists */
		if (pmaterial != pvNil)
			{
			brclr = BR_DK_TO_BR(pmaterial->ambient);
			pbmhrCur->fMtrlf = fTrue;
			_pglcrng->Get(LwcrngNearestBrclr(brclr, _pglclr, _pglcrng), &crng);
			if (_fColorOnly)
				goto LColorOnly;
			pbmhrCur->mtrlf.brufKa = BR_DKRGB_TO_FRGRAY(pmaterial->ambient);
			pbmhrCur->mtrlf.brufKd = BR_DKRGB_TO_FRGRAY(pmaterial->diffuse);
			if (pmaterial->type == DK_MAT_LAMBERT)
				pbmhrCur->mtrlf.brufKs = BR_SCALAR(0);
			else
				pbmhrCur->mtrlf.brufKs = BR_DKRGB_TO_FRGRAY(pmaterial->specular);
			goto LFinishMtrl;
			}
		/* If this model has no material of its own and it has a parent,
			get the parent's material */
		else if (pbmhrParent != pvNil)
			{
			Assert(!fMesh || pbmhrParent->fMtrlf, "Warning: mesh node has no material parent");
			pbmhrCur->mtrlf = pbmhrParent->mtrlf;
			/* REVIEW peted: bogus...if an STN were derived from the BASE class,
				I could just AddRef it and copy the pointer here rather than
				allocating a whole new STN */
			if (pbmhrParent->pstnMtrlFile != pvNil &&
					(pbmhrCur->pstnMtrlFile = new STN()) != pvNil)
				{
				*pbmhrCur->pstnMtrlFile = *pbmhrParent->pstnMtrlFile;
				}
			pbmhrCur->fMtrlf = pbmhrParent->fMtrlf;
			}
		/* If there's no parent, and no material, make something up */
		else
			{
			if (fMesh)
				printf("Warning: missing material for mesh node!\n");
			Assert(!pbmhrCur->fMtrlf, 0);
			_pglcrng->Get(0, &crng);
LColorOnly:
			pbmhrCur->mtrlf.brufKa = BR_UFRACTION(0);
			pbmhrCur->mtrlf.brufKd = BR_UFRACTION(1.0);
			pbmhrCur->mtrlf.brufKs = BR_UFRACTION(0);

			/* Finish filling in the material information */
LFinishMtrl:
			pbmhrCur->mtrlf.brc = 0; // unused by Socrates
			Assert(crng.lwBase < kbMax, "Color index base too high");
			Assert(crng.lwRange < kbMax, "Color index range too high");
			pbmhrCur->mtrlf.bIndexBase = (byte)crng.lwBase;
			/* Brender color range is defined by max val rather than count */
			pbmhrCur->mtrlf.cIndexRange = (byte)crng.lwRange - 1;
			pbmhrCur->mtrlf.rPower = BrIntToScalar(50); // REVIEW peted: need real val here
			}
		}
	Assert(pbmhrCur->pbmhrChild == pvNil, 0);

	/* Hook it in */
	*ppbmhr = pbmhrCur;
LFail:
	return pbmhrCur;
}


/******************************************************************************
	_TextureFileFromModel
		Extracts the base texture name from the file name given in a model's
		material information.  First looks in the model itself for a texture
		filename, and if no texture is found there, checks the first material
		listed for the model.  Copies texture translation and cropping info
		if the texture is present.
	
	Arguments:
		Model *pmodel  -- The model to process
		PBMHR pbmhr    -- the actual hiearchy node
		bool fWrapOnly -- just fill in the wrap-fix stuff
	
************************************************************ PETED ***********/
void S2B::_TextureFileFromModel(Model *pmodel, PBMHR pbmhr, bool fWrapOnly)
{
	AssertVarMem(pmodel);
	AssertVarMem(pmodel->materials);
	AssertVarMem(pbmhr);

	char *pch, *pszName;
	PSTN pstn = pvNil;
	Texture *ptexture;

	/* Look for a texture in the model; if the model doesn't have one, check
		its first material */
	if (pmodel->nbTextures < 1)
		{
		if (pmodel->materials->nbTextures < 1)
			goto LNotexture;
		ptexture = pmodel->materials->textures;
		}
	else
		ptexture = pmodel->textures;

	if (fWrapOnly)
		goto LDoWrap;

	pstn = new STN();
	if (pstn != pvNil)
		{
		pszName = ptexture->pic_name;
		pch = pszName + strlen(pszName);
		while (*(--pch) != ChLit('/') && pch > pszName);
		if (*pch == ChLit('/'))
			pch++;
		*pstn = pch;

		pbmhr->brufrUOffset = BrScalarToUFraction(BrFloatToScalar(ptexture->uOffset));
		pbmhr->brufrVOffset = BrScalarToUFraction(BrFloatToScalar(ptexture->vOffset));
		pbmhr->fCrop = FPure(ptexture->cropping) && (ptexture->uMinCrop ||
			ptexture->uMaxCrop || ptexture->vMinCrop || ptexture->vMaxCrop);
		if (pbmhr->fCrop)
			{
			pbmhr->uMinCrop = ptexture->uMinCrop;
			pbmhr->uMaxCrop = ptexture->uMaxCrop;
			pbmhr->vMinCrop = ptexture->vMinCrop;
			pbmhr->vMaxCrop = ptexture->vMaxCrop;
			}
LDoWrap:
		switch (ptexture->method)
			{
		case DK_TXT_SPHERICAL:
			pbmhr->fSpherical = fTrue;
		case DK_TXT_CYLINDRICAL:
			pbmhr->fFixWrap = fTrue;
			break;
		default:
			Assert(!pbmhr->fFixWrap, "Inconsistent fixwrap setting for node");
			break;
			}
		}

LNotexture:
	pbmhr->pstnMtrlFile = pstn;
}


/******************************************************************************
	_FTmapFromBmp
		Given a texture name, adds the texture to the MTRL with the given CNO.
		If this texture has never been seen before, the .bmp file is converted
		to an appropriate TMAP chunk file.  The reference to the parent MTRL's
		CNO is added to our list of generated TMAPs for use later in actually
		dumping out the TMAP chunk definition.
	
	Arguments:
		PSTN pstnBmpFile  -- the name of the texture
		CNO cnoPar        -- the CNO of the parent MTRL
		pstnMtrl          -- the string used for the MTRL name
	
	Returns: fTrue if the texture was successfully added
	
************************************************************ PETED ***********/
bool S2B::_FTmapFromBmp(PBMHR pbmhr, CNO cnoPar, PSTN pstnMtrl)
{
	Assert(pbmhr != pvNil, 0);
	AssertPo(pbmhr->pstnMtrlFile, 0);

	bool fRet = fFalse;
	long itmapd, itmapdMac;
	FNI fni;
	PTMAP ptmap = pvNil;
	TMAPD tmapd;
	PSTN pstnBmpFile = pbmhr->pstnMtrlFile;

	/* Look for the bitmap file in our list */
	if (_pggtmapd == pvNil && (_pggtmapd = GG::PggNew(size(TMAPD))) == pvNil)
		goto LFail;
	itmapdMac = _pggtmapd->IvMac();
	for (itmapd = 0; itmapd < itmapdMac; itmapd++)
		{
		_pggtmapd->GetFixed(itmapd, &tmapd);
		if (pstnBmpFile->FEqual(tmapd.pstn))
			break;
		}

	if (itmapd == itmapdMac)
		{
		_stnT = *pstnBmpFile;
		_ps2blx->GetFni(&fni);
		if (!fni.FSetLeaf(&_stnT, kftgBmp))
			goto LFail;
	
		ptmap = TMAP::PtmapReadNative(&fni);
		if (ptmap == pvNil)
			goto LFail;
	
		if (!fni.FChangeFtg(kftgTmapChkFile))
			goto LFail;

		if ((tmapd.pstn = new STN()) == pvNil)
			goto LFail;
		*tmapd.pstn = *pstnBmpFile;
		tmapd.ccnoPar = 1;
		tmapd.xp = ptmap->Pbpmp()->width;
		tmapd.yp = ptmap->Pbpmp()->height;
		if (!_pggtmapd->FAdd(size(CNO), pvNil, &cnoPar, &tmapd))
			goto LFailAdd;

		/* Write the file out last; it's easier to remove the reference to a
			failed file than to delete a file that's not referenced :) */
		if(!ptmap->FWriteTmapChkFile(&fni, fTrue))
			{
			_pggtmapd->Delete(itmapd);
LFailAdd:
			delete tmapd.pstn;
			goto LFail;
			}
		}
	else
		{
		if (!_pggtmapd->FInsertRgb(itmapd, tmapd.ccnoPar * size(CNO), size(CNO), &cnoPar))
			goto LFail;
		tmapd.ccnoPar++;
		_pggtmapd->PutFixed(itmapd, &tmapd);
		}

	if (pbmhr->brufrUOffset != BR_UFRACTION(0) ||
			pbmhr->brufrVOffset != BR_UFRACTION(0) || pbmhr->fCrop)
		{
		BRS brsXScale, brsYScale;
		BRS brsdu, brsdv;
		TXXFF txxff;

		txxff.bo = kboCur;
		txxff.osk = koskCur;

		/* Scaling is the ratio of the width and height of the bitmap left
			after removing the cropped areas to the width and height of the
			original bitmap.  The pixel u/vMax is included in the cropped
			and offset texture. */
		brsXScale = BR_DIV(
			BrIntToScalar(pbmhr->uMaxCrop - pbmhr->uMinCrop + 1),
			BrIntToScalar(tmapd.xp));
		brsYScale = BR_DIV(
			BrIntToScalar(pbmhr->vMaxCrop - pbmhr->vMinCrop + 1),
			BrIntToScalar(tmapd.yp));
		BrMatrix23Scale(&txxff.bmat23, brsXScale, brsYScale);

		/* Total offset is the specified offset, plus the necessary offset
			for any cropping */
		brsdu = BR_ADD(BrUFractionToScalar(pbmhr->brufrUOffset),
			BR_DIV(BrIntToScalar(pbmhr->uMinCrop), BrIntToScalar(tmapd.xp)));
		brsdv = BR_SUB(
			BR_DIV(BrIntToScalar(tmapd.yp - pbmhr->vMaxCrop),
				BrIntToScalar(tmapd.yp)),
			BrUFractionToScalar(pbmhr->brufrVOffset));
		BrMatrix23PostTranslate(&txxff.bmat23, brsdu, brsdv);

		/* Dump out the TXXFF chunk for this material */
		pstnMtrl->FAppendSz(PszLit(" Texture transform"));
		CnoNext();
		_DumpHeader(kctgTxxf, _cnoCur, pstnMtrl, fTrue);
		_chse.DumpParentCmd(kctgMtrl, cnoPar, 0);
		_chse.DumpRgb(&txxff, size(txxff));
		_chse.DumpSz(PszLit("ENDCHUNK"));
		_chse.DumpSz(PszLit(""));
		}

	fRet = fTrue;
LFail:
	ReleasePpo(&ptmap);
	return fRet;
}


/******************************************************************************
	_FFlushTmaps
		Actually writes out the TMAP definitions to the chunk source file.
		Each unique TMAP chunk is added once, with each MTRL that refers to
		it being included as a parent of the TMAP chunk.
	
	Returns: fTrue if all of the TMAP declarations could be generated; in
		theory, since we would have failed to even add a given TMAP to the list
		of TMAPs if we couldn't successfully generate the complete filename
		*and* failing to generate the complete filename is the only way for
		this routine to fail, this routine should never fail.
	
************************************************************ PETED ***********/
bool S2B::_FFlushTmaps(void)
{
	AssertPo(_pggtmapd, 0);

	bool fRet = fTrue;
	long itmapd, itmapdMac = _pggtmapd->IvMac();
	long icnoPar;
	CNO cnoPar;
	TMAPD tmapd;

	for (itmapd = 0; itmapd < itmapdMac; itmapd++)
		{
		FNI fni;

		_pggtmapd->GetFixed(itmapd, &tmapd);

		/* Generate the full filename for this texture */
		_stnT = *tmapd.pstn;
		_ps2blx->GetFni(&fni);
		if (!fni.FSetLeaf(&_stnT, kftgTmapChkFile))
			{
			fRet = fFalse;
			continue;
			}

		/* Emit the header */
		CnoNext();
		fni.GetLeaf(&_stnT);
		_DumpHeader(kctgTmap, _cnoCur, &_stnT, fFalse);

		/* Dump out all the necessary PARENT declarations */
		for (icnoPar = 0; icnoPar < tmapd.ccnoPar; icnoPar++)
			{
			_pggtmapd->GetRgb(itmapd, icnoPar * size(CNO), size(CNO), &cnoPar);
			_chse.DumpParentCmd(kctgMtrl, cnoPar, 0);
			}

		/* Finish up the chunk */
		fni.GetStnPath(&_stnT);
		_chse.DumpFileCmd(&_stnT, fTrue);
		_chse.DumpSz(PszLit("ENDCHUNK"));
		_chse.DumpSz(PszLit(""));
		}

	Assert(fRet, "_FFlushTmaps should never fail");
	return fRet;
}


/******************************************************************************
	_BrsdwrFromModel
		Computes a distance-between-cels for the given model, given the
	position of the last model.
	
	Arguments:
		Model *pmodel  -- the new model
		BRS rgbrsDwr[] -- the position of the last model
	
	Returns: a Brender scalar that's the distance
	
************************************************************ PETED ***********/
BRS S2B::_BrsdwrFromModel(Model *pmodel, BRS rgbrsDwr[])
{
	BRS brsDwr, brsX, brsY, brsZ;

	/* Calculate difference */
	brsX = BrFloatToScalar(pmodel->translation.x);
	brsY = BrFloatToScalar(pmodel->translation.y);
	brsZ = BrFloatToScalar(pmodel->translation.z);
	brsDwr = BR_LENGTH3(BR_SUB(rgbrsDwr[0], brsX),
						BR_SUB(rgbrsDwr[1], brsY),
						BR_SUB(rgbrsDwr[2], brsZ));

	/* Remember current model position */
	rgbrsDwr[0] = brsX;
	rgbrsDwr[1] = brsY;
	rgbrsDwr[2] = brsZ;

	/* Set model back to origin */
	pmodel->translation.x = pmodel->translation.y =
	pmodel->translation.z = (float)0.0;

	return brsDwr;
}

/*-----------------------------------------------------------------------------
|	_CopyVertices
|		Copy floating point vertices from the SoftImage data structure to
|	Brender scalar vertices needed by the Brender data structure.
|	
|	Arguments:
|		DK_Vertex *vertices -- pointer to SoftImage vertices array
|		void *pvDst -- pointer to start of Brender vertices in MODLF
|		long cVertices -- count of vertices
|	
-------------------------------------------------------------PETED-----------*/
void S2B::_CopyVertices(DK_Vertex *vertices, void *pvDst, long cVertices)
{
	BRV *pbrv = (BRV *)pvDst;
	DK_Point vtxMin = {(float)klwMax, (float)klwMax, (float)klwMax},
		vtxMax = {(float)klwMin, (float)klwMin, (float)klwMin};

	for ( ; cVertices; cVertices--, vertices++, pbrv++)
		{
		BRS *pbrs = &pbrv->p.v[0], *pbrsMac = &pbrv->p.v[3];
		float *pflPos = &vertices->position.x;

		vertices->position.z *= _iZsign;

		while (pbrs < pbrsMac)
			{
			*pbrs = BrFloatToScalar(*pflPos);
			if (_uRound > 0)
				*pbrs = BR_ROUND(*pbrs, _uRound);
			pbrs++;
			pflPos++;
			}

		if (_mdVerbose > kmdHelpful)
			{
			if (vertices->position.x < vtxMin.x)
				vtxMin.x = vertices->position.x;
			if (vertices->position.x > vtxMax.x)
				vtxMax.x = vertices->position.x;
			if (vertices->position.y < vtxMin.y)
				vtxMin.y = vertices->position.y;
			if (vertices->position.y > vtxMax.y)
				vtxMax.y = vertices->position.y;
			if (vertices->position.z < vtxMin.z)
				vtxMin.z = vertices->position.z;
			if (vertices->position.z > vtxMax.z)
				vtxMax.z = vertices->position.z;
			}
		}
	if (_mdVerbose > kmdHelpful)
		{
		if (_iZsign < 0)
			SwapVars(&vtxMin.z, &vtxMax.z);
		printf("Node bounds: (% 6.2f, % 6.2f, % 6.2f) - (% 6.2f, % 6.2f, % 6.2f)\n",
			vtxMin.x, vtxMin.y, _iZsign * vtxMin.z,
			vtxMax.x, vtxMax.y, _iZsign * vtxMax.z );
		}
}

/*-----------------------------------------------------------------------------
|	_CopyFaces
|		Copy all of the face data from the SoftImage list to the
|	Brender list.
|	
|	Arguments:
|		DK_Polygon *polygons -- list of SoftImage polygons (faces)
|		void *pvDst	 -- pointer to array of Brender faces
|		long cFaces	 -- count of faces to copy
|	
-------------------------------------------------------------PETED-----------*/
void S2B::_CopyFaces(DK_Polygon *polygons, void *pvDst, long cFaces, BRV rgbrv[], long cVertices)
{
	BRFC *pbrfc = (BRFC *)pvDst;

	for ( ; cFaces; cFaces--, polygons++, pbrfc++)
		{
		int cNodes = min(polygons->nbNodes, 3), i;
		DK_PolyNode *ppolynode = polygons->nodes;
		br_uint_16 *pvtx = &pbrfc->vertices[0];

		/* If we get an invalid node count, warn about it and then fill in
			if necessary (REVIEW peted: seems highly unlikely that we'll
			ever have too few vertices, seeing as how any less than 3
			gives a very uninteresting face indeed) */
		if (polygons->nbNodes != 3)
			{
			printf("SoftImage data has %d vertices for face\n", polygons->nbNodes);
			Assert(cNodes == 3, "Better keep that 'fill-in' code");
			for (i=cNodes; i < 3; i++)
				pvtx[i] = (br_uint_16)ppolynode->vertex;
			}

		/* copy the vertices for this face */
		for ( ; cNodes; cNodes--, ppolynode++, pvtx++)
			{
			*pvtx = (br_uint_16)ppolynode->vertex;
			if (*pvtx < cVertices)
				{
				rgbrv[*pvtx].map.v[0] = BrFloatToScalar(ppolynode->uTxt);
				rgbrv[*pvtx].map.v[1] = BrFloatToScalar(ppolynode->vTxt);
				}
			else
				printf("Warning: vertex out of range for mesh\n");
			}

		pbrfc->smoothing = 0x0001;
		}
}


/******************************************************************************
	_FDoBodyPart
		Handles a single body part.  Sets the Brender body part set for the
		given body part and stores that in the GL for the body part sets, and
		then handles actually generating whatever costume information for this
		body part is needed.
	
	Arguments:
		PBMHR pbmhr  -- The BMHR that describes this body part
		long ibp     -- the actual body part number for this body part
	
	Returns:  fTrue if the body part could be added, and its basic costume
		information could be written out; failing to add the TMAP information
		simply generates a warning without causing this routine to return a
		failure.
	
************************************************************ PETED ***********/
bool S2B::_FDoBodyPart(PBMHR pbmhr, long ibp)
{
	bool fRet = fFalse, fDoMtrl = (_pglclr != pvNil && _pglcmtld != pvNil);
	bool fUseMtrl;
	short ibps;
	long ibpGl;
	CMTLD cmtld;
	long icmtld, icmtldMac, iibps, iibpsMac;

	/* Find the appropriate CMTL */
	if (fDoMtrl)
		{
		/* We may be only making costumes for a subset of the body part sets */
		if (_pglibps != pvNil)
			{
			iibpsMac = _pglibps->IvMac();
			for (iibps = 0; iibps < iibpsMac; iibps++)
				{
				long ibpsUse;
	
				_pglibps->Get(iibps, &ibpsUse);
				if (ibpsUse == pbmhr->ibps)
					break;
				}
			fUseMtrl = iibps < iibpsMac;
			}
		else
			fUseMtrl = fTrue;

		/* Go ahead and figure out the mapping from SI body part sets to
			Brender body part sets */
		icmtldMac = _pglcmtld->IvMac();
		for (icmtld = 0; icmtld < icmtldMac; icmtld++)
			{
			_pglcmtld->Get(icmtld, &cmtld);
			if (cmtld.ibps == pbmhr->ibps)
				break;
			}
		AssertIn(icmtld, kswMin, kswMax + 1);
		ibps = (short)icmtld;
		}
	else
		ibps = pbmhr->ibps;

	if (_fMakeGlpi && !_pglbs->FAdd(&ibps, &ibpGl))
		goto LFail;

	if (_fMakeCostume)
		{
		Assert(!_fMakeGlpi || ibpGl == ibp, "Weird order for GLBS");

		/* Handle material stuff, if we have color info */
		if (fDoMtrl)
			{
			long ccmid;

			/* Create the CMTL chunk if necessary */
			if (icmtld == icmtldMac)
				{
				long icmtldT;

				/* Add it to our list */
				cmtld.cno = CnoNext();
				cmtld.chidCur = 0;
				cmtld.ibps = pbmhr->ibps;
				cmtld.chid = _chidCmtl;
				if (!_pglcmtld->FAdd(&cmtld, &icmtldT))
					goto LFail;
				Assert(icmtldT == icmtld, "Bogus icmtld");

				/* Add it to the GGCM */
				Assert(ibps <= _pggcm->IvMac(), "ibps out of range");
				if (ibps == _pggcm->IvMac())
					{
					long iv;

					ccmid = 0;
					if (!_pggcm->FAdd(0, &iv, pvNil, &ccmid))
						goto LFail;
					Assert(iv == ibps, "bogus iv");
					}

				if (fUseMtrl)
					{
					CMTLF cmtlf;

					_chidCmtl++;

					_pggcm->GetFixed(ibps, &ccmid);
					if (!_pggcm->FInsertRgb(ibps, size(long) * ccmid, size(long), &cmtld.chid))
						goto LFail;
					ccmid++;
					_pggcm->PutFixed(ibps, &ccmid);

					/* Write a nice CMTL chunk */
					cmtlf.bo = kboCur;
					cmtlf.osk = koskCur;
					cmtlf.ibset = ibps;
					if (ccmid == 1)
						{
						_stnT.FFormatSz(PszLit("%s Body Part Set %d Default"),
							&_stnTmpl, ibps);
						}
					else
						{
						_stnT.FFormatSz(PszLit("%s Body Part Set %d Costume %d"),
							&_stnTmpl, ibps, ccmid);
						}
					_DumpHeader(kctgCmtl, cmtld.cno, &_stnT, fTrue);
					_chse.DumpParentCmd(kctgTmpl, _cnoPar, cmtld.chid);
					_chse.DumpRgb(&cmtlf, size(cmtlf));
					_chse.DumpSz(PszLit("ENDCHUNK"));
					_chse.DumpSz(PszLit(""));
					}
				}
			else if (fUseMtrl)
				_pggcm->GetFixed(ibps, &ccmid);

			/* Write a nice MTRL chunk for this body part */
			if (fUseMtrl)
				{
				if (ccmid == 1)
					{
					_stnT.FFormatSz(PszLit("%s Part %d Default MTRL"),
						&_stnTmpl, ibp);
					}
				else
					{
					_stnT.FFormatSz(PszLit("%s Part %d Costume %d MTRL"),
						&_stnTmpl, ibp, ccmid);
					}
				CnoNext();
				_DumpHeader(kctgMtrl, _cnoCur, &_stnT, fTrue);
				_chse.DumpParentCmd(kctgCmtl, cmtld.cno, cmtld.chidCur);
				_chse.DumpRgb(&pbmhr->mtrlf, size(MTRLF));
				_chse.DumpSz(PszLit("ENDCHUNK"));
				_chse.DumpSz(PszLit(""));

				if (pbmhr->pstnMtrlFile != pvNil)
					{
					if (!_FTmapFromBmp(pbmhr, _cnoCur, &_stnT))
						{
						printf("Warning: texture BMP file does not exist or is invalid (%s)\n",
							pbmhr->pstnMtrlFile->Psz());
						}
					}

				if (pbmhr->fAccessory)
					{
					PBMDB pbmdb;
					KID kidBmdl;

					_ApplyBmdlXF(pbmhr);
					if (!_FChidFromModlf(pbmhr, pvNil, &pbmdb))
						goto LFail;
					kidBmdl.cki.ctg = ctgNil;
					kidBmdl.cki.cno = cmtld.cno;
					kidBmdl.chid = cmtld.chidCur;
					if (!_FAddBmdlParent(pbmdb, &kidBmdl))
						goto LFail;
					}

				cmtld.chidCur++;
				_pglcmtld->Put(icmtld, &cmtld);
				}
			}
		}
	fRet = fTrue;
LFail:
	return fRet;
}


void S2B::_ApplyBmdlXF(PBMHR pbmhr)
{
	long cver = pbmhr->pmodlf->cver;
	BRV *pbrv = (BRV *)PvAddBv(pbmhr->pmodlf, size(MODLF));

	while (cver--)
		{
		BrMatrix34ApplyP(&pbrv->p, &pbrv->p, &pbmhr->bmat34);
		pbrv++;
		}
}


/*-----------------------------------------------------------------------------
|	_FProcessBmhr
|		Iterates through a Brender model hierarchy, creating the necessary
|	chunk data as it goes.  The GLPI is made the first time, and then
|	verified each subsequent time for the template.  The array of CPSs
|	is filled in for the given node of the given cel.  If the MODLF is no
|	longer needed, it's freed; the model node is always freed.
|	
|	Arguments:
|		PBMHR *ppbmhr -- points to pointer that points to this node
|		short ibpPar -- index of this node's parent body part
|	
|	Returns:
|		fTrue if it succeeds, fFalse otherwise
|	
-------------------------------------------------------------PETED-----------*/
bool S2B::_FProcessBmhr(PBMHR *ppbmhr, short ibpPar)
{
	short ibpThis;
	PBMHR pbmhrCur = *ppbmhr, pbmhrNext;

	while (pbmhrCur != pvNil)
		{
		pbmhrNext = pbmhrCur->pbmhrSibling;

		/* Create Cel Part Spec for this node */
		ibpThis = _ibpCur++;
		if (!_fCostumeOnly)
			{
			if (!_FSetCps(pbmhrCur, &_prgcps[ibpThis]))
				goto LFail;	// _FSetCps already displayed the error
			}
		else
			Assert(_prgcps == pvNil, "Why is there an array of CPS allocated?");

		/* Update or verify GLPI */
		if (_fMakeGlpi)
			{
			long ibpGl;
	
			if (!_pglibactPar->FAdd(&ibpPar, &ibpGl))
				{
				printf("Error: Couldn't add node to GLPI -- OOM\n");
				goto LFail;
				}
			Assert(ibpGl == ibpThis, "Weird order for GLPI");
			}
		else
			{
			short ibpParGlpi;
	
			_pglibactPar->Get(ibpThis, &ibpParGlpi);
			if (ibpPar != ibpParGlpi)
				{
				printf("Error: cel doesn't match previous GLPI hierarchy");
				if (pbmhrCur->pszName != pvNil)
					printf(" (node %s)\n", pbmhrCur->pszName);
				else
					printf("\n");
				goto LFail;
				}
			}

		if (_pglbs != pvNil && (_fMakeGlpi || _fMakeCostume) && !_FDoBodyPart(pbmhrCur, ibpThis))
			{
			printf("Warning: no body part set information -- OOM\n");
			ReleasePpo(&_pglbs);
			ReleasePpo(&_pglcmtld);
			}

		/* Process the children */
		if (!_FProcessBmhr(&pbmhrCur->pbmhrChild, ibpThis))
			{
			Assert(*ppbmhr == pbmhrCur, 0);
			goto LFail;
			}

		/* Clean up and go to next sibling */
		FreePpv((void **)&pbmhrCur->pmodlf);
		FreePpv((void **)&pbmhrCur->pszName);
		if ((*ppbmhr)->pstnMtrlFile != pvNil)
			delete (*ppbmhr)->pstnMtrlFile;
		FreePpv((void **)&pbmhrCur);
		*ppbmhr = pbmhrCur = pbmhrNext;
		}

	return fTrue;
LFail:
	return fFalse;
}

/*-----------------------------------------------------------------------------
|	_FEnsureOneRoot
|		Makes sure that the root of our Brender hierarchy has no siblings.
|	Will create a new null root if required.  Will delete the entire tree
|	if it was unsuccessful.
|	
|	Arguments:
|		PBMHR *ppbmhr -- points to pointer to the root of the hierarchy
|	
|	Returns:
|		fTrue if it could make sure the tree had siblingless root
|	
-------------------------------------------------------------PETED-----------*/
bool S2B::_FEnsureOneRoot(PBMHR *ppbmhr)
{
	MODLF *pmodlf;
	PBMHR pbmhr;

	/* REVIEW peted: don't bother doing this, 'cause it messes up the material
		stuff.  If we really care (which I'm not sure we do, since Socrates
		now handles multiple root-level nodes), we can fix this so that any
		automatically generated body part sets are also included in the list
		of body part sets to create material info for.  Alternatively, Socrates
		could handle not having a material for a body part set more gracefully */
	return fTrue;

	/* If the tree's empty, fail since there's no use making a null model */
	if (*ppbmhr == pvNil)
		{
		printf("Error: no valid nodes found in model\n");
		return fFalse;
		}

	/* If the tree's already good, great! */
	if ((*ppbmhr)->pbmhrSibling == pvNil)
		return fTrue;

	/* Allocate necessary structures.  The MODLF and the BMHR */
	if (!FAllocPv((void **)&pmodlf, size(MODLF), fmemClear, mprNormal))
		goto LFail;
	if (!FAllocPv((void **)&pbmhr, size(BMHR), fmemClear, mprNormal))
		{
		FreePpv((void **)&pmodlf);
		goto LFail;
		}
	pbmhr->pmodlf = pmodlf;
	_InitBmhr(pbmhr);

	/* Fill in the MODLF; no faces or vertices */
	Assert(pmodlf->cver == 0, "Didn't clear MODLF");
	Assert(pmodlf->cfac == 0, "Didn't clear MODLF");

	/* Fill in the BMHR */
	pbmhr->cbModlf = size(MODLF);
	BrMatrix34Identity(&pbmhr->bmat34);
	pbmhr->pbmhrSibling = pvNil;

	/* Hook it in and count it */
	pbmhr->pbmhrChild = *ppbmhr;
	*ppbmhr = pbmhr;
	_cMesh++;

	return fTrue;
LFail:
	_DisposeBmhr(ppbmhr);
	return fFalse;
}

/******************************************************************************
	_InitBmhr
		Initalize the chunk "F" structures for the BMHR
	
	Arguments:
		PBMHR pbmhr  -- the BMHR to init
	
************************************************************ PETED ***********/
void S2B::_InitBmhr(PBMHR pbmhr)
{
	pbmhr->pmodlf->bo = pbmhr->mtrlf.bo = kboCur;
	pbmhr->pmodlf->osk = pbmhr->mtrlf.osk = koskCur;
}


/* Wrap fixing code from Sam at Argonaut begins here */

/*
 * Flags to ModelFixWrapping()
 */
#define WRAP_U_0 0x40
#define WRAP_V_0 0x80
#define WRAP_U_1 0x10
#define WRAP_V_1 0x20

/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: $
 * $Locker:  $
 */

/*
 * Scratch structures used by ModelFixWrapping()
 */
struct wrap_face_info {
	br_uint_8 edge_dir[3];
	br_uint_8 vflags[3];
};

struct wrap_edge_info {
	br_uint_16 old_vertices[2];
	br_uint_16 vertices[2];
	br_uint_8 count;
	br_uint_8 flags;
	br_uint_8 vflags[2];
};

/*
 * Temp. workspace used to build new vertices
 */
static br_vertex *WrapNewVertices;
static int WrapTotalVertices;
static int WrapNextVertex;

/*
 * Adds a new vertex to model that has had it's
 * wrapping fixed
 */

static int WrapAddVertex(br_vertex *new_vp)
{
	int i;
	struct br_vertex *vp;

	/*
	 * Try and find vertex in current set
	 */
	vp = WrapNewVertices;

	for(i=0; i < WrapNextVertex; i++, vp++) {
		if( new_vp->p.v[0]   == vp->p.v[0] &&
			new_vp->p.v[1]   == vp->p.v[1] &&
			new_vp->p.v[2]   == vp->p.v[2] &&
			new_vp->map.v[0] == vp->map.v[0] &&
			new_vp->map.v[1] == vp->map.v[1])
			return i;
	}

	/*
	 * Add to end of list
	 */
	if(WrapNextVertex >= WrapTotalVertices)
		BR_ERROR0("WrapAddVertex: no space");

	WrapNewVertices[WrapNextVertex] = *new_vp;

	return WrapNextVertex++;
}


/*
 * ModelFixWrapping()
 *
 * Processes a model and attempts to fix up seams where the texture
 * coordinates implicitly wrap.
 *
 * Takes a pointer to an integer mask of [WRAP_U , WRAP_V]
 * Each bit indicates that wrapping should be explored in that
 * texture axis.
 */
bool FModelFixWrapping(br_model *model, int *mask)
{
	Assert(!(*mask & WRAP_V_0), "WRAP_V NYI");

	bool fRet = fFalse;
	br_vertex *vp0,*vp1;
	br_face *fp;
	int f,e,e0,e1,v,num0_wraps,num1_wraps,v0,v1;
	struct wrap_edge_info *edge_info,*eip;
	struct wrap_face_info *face_info,*fip;
	br_vertex tempv;
	BRS brsMinDist0, brsMinDist1;

	/* Ensure edge info */
	BrModelPrepare(model, BR_MPREP_EDGES);

//	edge_info = BrMemCalloc(model->nedges,sizeof(*edge_info),BR_MEMORY_APPLICATION);
	if (!FAllocPv((void **)&edge_info, model->nedges * size(wrap_edge_info),
			fmemClear, mprNormal))
		{
		goto LFail;
		}

//	face_info = BrMemCalloc(model->nfaces,sizeof(*face_info),BR_MEMORY_APPLICATION);
	if (!FAllocPv((void **)&face_info, model->nfaces * size(wrap_face_info),
			fmemClear, mprNormal))
		{
		goto LFail;
		}

	/*
	 * Go through faces to build edge table
	 */
	fp = model->faces;
	fip = face_info;
	for(f=0 ; f < model->nfaces; f++, fp++,fip++) {
		for(e0 = 0; e0 < 3; e0++) {
			e1 = (e0 == 2)?0:(e0 + 1);
			eip = edge_info + fp->edges[e0];
			eip->count++;
			if(eip->count == 1) {
				/*
				 * First time edge has been used
				 */
				eip->vertices[0] = fp->vertices[e0];
				eip->vertices[1] = fp->vertices[e1];

				eip->old_vertices[0] = eip->vertices[0];
				eip->old_vertices[1] = eip->vertices[1];
			}

			/*
			 * See which direction edge is used
			 */
			if(eip->vertices[0] == fp->vertices[e0] &&
			   eip->vertices[1] == fp->vertices[e1]) {
			   	fip->edge_dir[e0] = 0;
			} else if(
			   eip->vertices[0] == fp->vertices[e1] &&
			   eip->vertices[1] == fp->vertices[e0]) {
				fip->edge_dir[e0] = 1;
			} else
				BR_ERROR1("Face %d has a bad edge\n",f);
		}
	}

	/*
	 * For each edge - find if U or V wraps
	 */
	num0_wraps = 0;
	num1_wraps = 0;
	brsMinDist0 = brsMinDist1 = BR_SCALAR(1.0);

	eip = edge_info;
	for(e=0; e< model->nedges; e++, eip++) {
		vp0 = model->vertices + eip->vertices[0];
		vp1 = model->vertices + eip->vertices[1];

		if((*mask & WRAP_U_0) && (BR_ABS(vp0->map.v[0]-vp1->map.v[0]) >
				BR_SCALAR(0.5)))
			{
			br_uint_8 vf0, vf1;

			Assert(vp0->map.v[0] >= BR_SCALAR(0.0), "Vertex U less than 0");
			Assert(vp0->map.v[0] <= BR_SCALAR(1.0), "Vertex U greater than 1");
			Assert(vp1->map.v[0] >= BR_SCALAR(0.0), "Vertex U less than 0");
			Assert(vp1->map.v[0] <= BR_SCALAR(1.0), "Vertex U greater than 1");

			vf0 = vf1 = 0;
			if (vp0->map.v[0] > vp1->map.v[0])
				{
				BRS brsDist;

				brsDist = BR_SUB(BR_SCALAR(1.0), vp0->map.v[0]);

				/* Put the new vertex colocated with whatever vertex is closest
					to the 0/1 boundary */
				if (brsDist < vp1->map.v[0])
					vf0 = WRAP_U_0;
				else
					vf1 = WRAP_U_1;
				}
			else
				{
				BRS brsDist;

				brsDist = BR_SUB(BR_SCALAR(1.0), vp1->map.v[0]);

				/* Ditto */
				if (brsDist < vp0->map.v[0])
					vf1 = WRAP_U_0;
				else
					vf0 = WRAP_U_1;
				}

			/*
			 * Mark lower vertex as wrapping - keep a count
			 * new wraps
			 */
			if (!(eip->vflags[0] & vf0))
				{
				eip->vflags[0] |= vf0;
				if (vf0 & WRAP_U_0)
					num0_wraps++;
				else
					num1_wraps++;
				}
			if (!(eip->vflags[1] & vf1))
				{
				eip->vflags[1] |= vf1;
				if (vf1 & WRAP_U_0)
					num0_wraps++;
				else
					num1_wraps++;
				}
			/*
			 * mark face
			 */
			eip->flags |= WRAP_U_0;
		}

#ifdef NOT
		if((*mask & WRAP_V) && (BR_ABS(vp0->map.v[1]-vp1->map.v[1]) > BR_SCALAR(0.5))) {
			vw = (vp0->map.v[1] > vp1->map.v[1])?1:0;

			if(!(eip->vflags[vw] & WRAP_V)) {
				eip->vflags[vw] |= WRAP_V;
				num_wraps++;
			}
			/*
			 * mark face
			 */
			eip->flags |= WRAP_V;
		}
#endif

	}

	/*
	 * Go through faces and accumulate per face-vertex flags
	 */
	fp = model->faces;
	fip = face_info;

	for(f=0; f< model->nfaces; f++, fp++, fip++) {
		for(e0 = 0; e0 < 3; e0++) {
			e1 = (e0 == 2)?0:(e0 + 1);
			eip = edge_info + fp->edges[e0];

			if(fip->edge_dir[e0])
				v0 = 1, v1 = 0;
			else
				v0 = 0, v1 = 1;

			fip->vflags[e0] |= eip->vflags[v0];
			fip->vflags[e1] |= eip->vflags[v1];
		}
	}


	/*
	 * Allocate a new vertex array (may be bigger than required)
	 */
	WrapTotalVertices = model->nvertices + num1_wraps + num0_wraps;
	WrapNewVertices = (BRV *)BrResAllocate(model,WrapTotalVertices * sizeof(*WrapNewVertices),BR_MEMORY_VERTICES);
	memcpy(WrapNewVertices,model->vertices,sizeof(*WrapNewVertices) * model->nvertices);

	WrapNextVertex = model->nvertices;

	/*
	 * Go through faecs and generate new vertices
	 */
	fp = model->faces;
	fip = face_info;

	for(f=0; f< model->nfaces; f++, fp++, fip++)
		{
		br_uint_8 vflags = 0, vbad = 0;
		int v0, v1;

		v0 = v1 = -1;
		for(v=0; v < 3; v++)
			{
			br_vertex *pbrv;

			if(!(fip->vflags[v]))
				continue;

			Assert((fip->vflags[v] & (WRAP_U_0 | WRAP_U_1)) !=
				(WRAP_U_0 | WRAP_U_1), "Both wrap flags set!");
			Assert(v0 == -1 || v1 == -1, "Already seen two bad vertices");

			/*
			 * Make a copy of the vertex, and wrap
			 * mapping coordinates
			 */
			pbrv = &WrapNewVertices[fp->vertices[v]];
			tempv = *pbrv;

			vbad |= (0x01 << v);
			if (fip->vflags[v] & WRAP_U_0)
				{
				if (vflags & WRAP_U_0)
					v1 = v;
				else
					{
					vflags |= WRAP_U_0;
					v0 = v;
					}
				}
			else
				{
				Assert(fip->vflags[v] & WRAP_U_1, "Unknown wrap flag set");
				if (vflags & WRAP_U_1)
					v0 = v;
				else
					{
					vflags |= WRAP_U_1;
					v1 = v;
					}
				}

#ifdef NOT
			if(fip->vflags[v] & WRAP_U)
				tempv.map.v[0] = /* tempv.map.v[0] + */ BR_SCALAR(1.0);

			if(fip->vflags[v] & WRAP_V)
				tempv.map.v[1] = /* tempv.map.v[1] + */ BR_SCALAR(1.0);
#endif
			}

		if (!vflags)
			continue;

		if ((vflags & (WRAP_U_0 | WRAP_U_1)) == (WRAP_U_0 | WRAP_U_1))
			{
			int v_move, v_good;
			br_vertex *pbrv_good, *pbrv_move;
			br_scalar brs_new_u;

			/* Need to figure out which to actually move */
			vbad ^= 0x07;
			Assert(!((vbad - 1) & vbad), "Should only have one bit set");
			for (v_good = 0; vbad > 1; v_good++, vbad >>= 1);
			Assert(FIn(v_good, 0, 3), "vbad out of range");
			pbrv_good = &WrapNewVertices[fp->vertices[v_good]];

			/* The good vertex will be within 0.5 of the new u value */
			if (pbrv_good->map.v[0] >= BR_SCALAR(0.5))
				{
				brs_new_u = BR_SCALAR(1.0);
				v_move = v1;
				}
			else
				{
				brs_new_u = BR_SCALAR(0.0);
				v_move = v0;
				}
			pbrv_move = &WrapNewVertices[fp->vertices[v_move]];
			Assert(BR_ABS(BR_SUB(pbrv_good->map.v[0], pbrv_move->map.v[0])) > BR_SCALAR(0.5),
				"Moved vertex is within 0.5 of good vertex");
			tempv = *pbrv_move;
			tempv.map.v[0] = brs_new_u;
			fp->vertices[v_move] = WrapAddVertex(&tempv);
			}
		else
			{
			br_scalar brs_new_u;

			if (vflags & WRAP_U_0)
				brs_new_u = BR_SCALAR(0.0);
			else
				{
				Assert(vflags & WRAP_U_1, "Unknown wrap flag");
				brs_new_u = BR_SCALAR(1.0);
				}

			/* Possibly need to move both vertices over to new u value */
			if (v0 != -1)
				{
				tempv = WrapNewVertices[fp->vertices[v0]];
				tempv.map.v[0] = brs_new_u;
				fp->vertices[v0] = WrapAddVertex(&tempv);
				}
			else
				Assert(vflags & WRAP_U_1, "Inconsistent wrap flag");

			if (v1 != -1)
				{
				tempv = WrapNewVertices[fp->vertices[v1]];
				tempv.map.v[0] = brs_new_u;
				fp->vertices[v1] = WrapAddVertex(&tempv);
				}
			else
				Assert(vflags & WRAP_U_0, "Inconsistent wrap flag");
			}
		}

//	BrMemFree(face_info);
	FreePpv((void **)&face_info);
//	BrMemFree(edge_info);
	FreePpv((void **)&edge_info);

	/*
	 * Connect new vertex array to faces
	 */
	BrResFree(model->vertices);

	model->vertices = WrapNewVertices;
	model->nvertices = WrapNextVertex;
	WrapNewVertices = NULL;

	fRet = fTrue;
LFail:
	return fRet;
}

/* Wrap fix code from Sam at Argonaut ends here */


/*-----------------------------------------------------------------------------
|	_FlushTmplKids
|		Writes whatever the current template accumulators contain and
|	resets them.
|	
-------------------------------------------------------------PETED-----------*/
void S2B::_FlushTmplKids(void)
{
	int cBmdbTotal = 0, cBmdbChains = 0;
#if HASH_FIXED
	PBMDB *ppbmdb;
#endif /* HASH_FIXED */
	PBMDB pbmdb, pbmdbNext;

	if (_pglibactPar != pvNil)
		{
#if HASH_FIXED
		Assert(_prgpbmdb != pvNil, 0);
		PBMDB *ppbmdbMac = (_prgpbmdb + kcpbmdb);
#else /* HASH_FIXED */
		Assert(_pglpbmdb != pvNil, 0);
		long ipbmdb = _pglpbmdb->IvMac();
#endif /* !HASH_FIXED */

		Assert(_ctgPar == kctgTmpl, "Odd parent for body part chunks");
		
		CnoNext();
		_stnT.FFormatSz(PszLit("%s Tree"), &_stnTmpl);
		_DumpHeader(kctgGlpi, _cnoCur, &_stnT, fTrue);
		_chse.DumpParentCmd(_ctgPar, _cnoPar, 0);
		_chse.DumpList(_pglibactPar);
		_chse.DumpSz(PszLit("ENDCHUNK"));
		_chse.DumpSz(PszLit(""));

		/* Create dummy GLBS if necessary */
		if (_pglbs == pvNil)
			{
			short swZero = 0;

			Assert(_pglcmtld == pvNil,
				"Custom material info with no body part set info");
			for (long lw = 0; lw < _pglibactPar->IvMac(); lw++)
				_pglibactPar->Put(lw, &swZero);
			_pglbs = _pglibactPar;
			_pglibactPar = pvNil;
			}
		else
			{
			/* Don't need this any more */
			ReleasePpo(&_pglibactPar);

			/* Do material stuff if we have the CMTL GL and there's valid
				color info */
			if (_pggcm != pvNil && _pglclr != pvNil)
				{
				/* Dump out GGCM */
				Assert(_ctgPar == kctgTmpl, "Weird parent for GGCM");
				CnoNext();
				_stnT.FFormatSz(PszLit("%s Costumes"), &_stnTmpl);
				_DumpHeader(kctgGgcm, _cnoCur, &_stnT, fTrue);
				_chse.DumpParentCmd(_ctgPar, _cnoPar, 0);
				_chse.DumpGroup(_pggcm);
				_chse.DumpSz(PszLit("ENDCHUNK"));
				_chse.DumpSz(PszLit(""));
				}
#ifdef DEBUG
			else if (_pggcm != pvNil)
				Assert(_pggcm->IvMac() == 0,
					"Non-empty GGCM without any color info");
#endif // DEBUG
			ReleasePpo(&_pggcm);

			if (_pggtmapd != pvNil)
				{
				if (!_FFlushTmaps())
					printf("Warning: couldn't write all TMAPs; some textures may not be present\n");
				ReleasePpo(&_pggtmapd);
				}
			}

		CnoNext();
		_stnT.FFormatSz(PszLit("%s Body Part Sets"), &_stnTmpl);
		_DumpHeader(kctgGlbs, _cnoCur, &_stnT, fTrue);
		_chse.DumpParentCmd(_ctgPar, _cnoPar, 0);
		_chse.DumpList(_pglbs);
		_chse.DumpSz(PszLit("ENDCHUNK"));
		_chse.DumpSz(PszLit(""));
		ReleasePpo(&_pglbs);

#if HASH_FIXED
		for (ppbmdb = _prgpbmdb; ppbmdb < ppbmdbMac; ppbmdb++)
#else /* HASH_FIXED */
		while (ipbmdb > 0)
#endif /* !HASH_FIXED */
			{
			int cBmdbList = 0;

#if HASH_FIXED
			pbmdb = *ppbmdb;
#else /* HASH_FIXED */
			ipbmdb--;
			_pglpbmdb->Get(ipbmdb, &pbmdb);
#endif /* !HASH_FIXED */
			while (pbmdb != pvNil)
				{
				/* Don't bother fixing up a model with no vertices; it wastes
					time, and the Argonaut code doesn't handle that case
					gracefully anyway */
				if (_fFixWrap && pbmdb->pmodlf->cver > 0 && pbmdb->fFixWrap)
					{
					bool fSuccess = fFalse;
					PBMDL pbmdl = pvNil;

					if (_FModlfToBmdl(pbmdb->pmodlf, &pbmdl))
						{
						int mask;

						mask = pbmdb->fSpherical ? (WRAP_U_0 | WRAP_V_0) : WRAP_U_0;
						if (FModelFixWrapping(pbmdl, &mask))
							{
							fSuccess = _FBmdlToModlf(pbmdl, &pbmdb->pmodlf,
								&pbmdb->cbModlf);
							}
						BrModelFree(pbmdl);
						}
					if (!fSuccess)
						{
						printf("Warning: couldn't fix up texture wrapping for");
						if (pbmdb->pszName != pvNil)
							printf(" node %s\n", pbmdb->pszName);
						else
							printf(" unnamed node\n");
						}
					}

				cBmdbList++;
				if (pbmdb->pszName == pvNil)
					_stnT.FFormatSz(PszLit("%s Model %d"), &_stnTmpl, pbmdb->chidBmdl);
				else
					_stnT.FFormatSz(PszLit("%s Model %z"), &_stnTmpl, pbmdb->pszName);
				_DumpHeader(kctgBmdl, pbmdb->cnoBmdl, &_stnT, fTrue);
				if (pbmdb->pglkidCmtl != pvNil)
					{
					long icno, icnoMac = pbmdb->pglkidCmtl->IvMac();
					KID kidBmdl;

					for (icno = 0; icno < icnoMac; icno++)
						{
						pbmdb->pglkidCmtl->Get(icno, &kidBmdl);
						_chse.DumpParentCmd(kctgCmtl, kidBmdl.cki.cno, kidBmdl.chid);
						}
					ReleasePpo(&pbmdb->pglkidCmtl);
					}
				if (pbmdb->chidBmdl != ivNil)
					_chse.DumpParentCmd(_ctgPar, _cnoPar, pbmdb->chidBmdl);
				_chse.DumpRgb(pbmdb->pmodlf, pbmdb->cbModlf);
				_chse.DumpSz(PszLit("ENDCHUNK"));
				_chse.DumpSz(PszLit(""));

				pbmdbNext = (PBMDB)pbmdb->hshdb.phshdbNext;
				FreePpv((void **)&pbmdb->pmodlf);
				FreePpv((void **)&pbmdb->pszName);
				FreePpv((void **)&pbmdb);
				pbmdb = pbmdbNext;
				}
			cBmdbTotal += cBmdbList;
			if (_mdVerbose > kmdQuiet && cBmdbList > 0)
				{
				cBmdbChains++;
				printf("Model collision chain has %d entries\n", cBmdbList);
				}
#if HASH_FIXED
			*ppbmdb = pvNil;
#else /* HASH_FIXED */
			_pglpbmdb->Delete(ipbmdb);
#endif /* !HASH_FIXED */
			}
		if (_mdVerbose > kmdQuiet)
			{
			float flAvg = (float)cBmdbTotal / (float)cBmdbChains;
			printf("Average collision chain has %1.2f entries (%d chains)\n", flAvg, cBmdbChains);
			}
		}
	Assert(_pglibactPar == pvNil, 0);
	Assert(_pglbs == pvNil, 0);
	Assert(_pglcmtld == pvNil, 0);
#if HASH_FIXED
	Assert(*_prgpbmdb == pvNil, 0);
#else /* HASH_FIXED */
	Assert(_pglpbmdb->IvMac() == 0, 0);
#endif /* !HASH_FIXED */
	_chidActn = 0;
	_chidBmdl = 0;
	_chidCmtl = 0;
}


/******************************************************************************
	_FModlfToBmdl
		Converts a MODLF structure to the corresponding Brender br_model
		structure (BMDL in Socrates nomenclature).  Simply allocates a
		Brender model and copies the vertices.
	
	Arguments:
		PMODLF pmodlf  --  the MODLF to convert
		PBMDL *ppbmdl  --  takes the pointer to the new Brender model
	
	Returns: fTrue on success, fFalse otherwise
	
************************************************************ PETED ***********/
bool S2B::_FModlfToBmdl(PMODLF pmodlf, PBMDL *ppbmdl)
{
	AssertVarMem(pmodlf);
	AssertVarMem(ppbmdl);
	Assert(*ppbmdl == pvNil, "Already have BMDL");

	bool fRet = fFalse;
	long cbrgbrv, cbrgbrf;
	PBMDL pbmdl;

	pbmdl = BrModelAllocate(pvNil, pmodlf->cver, pmodlf->cfac);
	if (pbmdl == pvNil)
		goto LFail;

	cbrgbrv = LwMul(pmodlf->cver, size(BRV));
	cbrgbrf = LwMul(pmodlf->cfac, size(BRF));
	CopyPb(PvAddBv(pmodlf, size(MODLF)), pbmdl->vertices, cbrgbrv);
	CopyPb(PvAddBv(pmodlf, size(MODLF) + cbrgbrv), pbmdl->faces, cbrgbrf);

	*ppbmdl = pbmdl;

	fRet = fTrue;
LFail:
	return fRet;
}


/******************************************************************************
	_FBmdlToModlf
		Converts a Brender br_model to a MODLF structure.  Ensures that the
		Brender model has been prepared and then copies all relevant
		information from the Brender model to the MODLF.  Uses the passed in
		MODLF to initialize the newly created one so that untouched fields
		remain the same.  Frees the old MODLF only on success.
	
	Arguments:
		PBMDL pbmdl      --  the Brender model to convert
		PMODLF *ppmodlf  --  provides the template for the MODLF, and takes
			the pointer to the new MODLF structure on success
	
	Returns: fTrue on success, fFalse otherwise
	
************************************************************ PETED ***********/
bool S2B::_FBmdlToModlf(PBMDL pbmdl, PMODLF *ppmodlf, long *pcb)
{
	AssertVarMem(pbmdl);
	AssertVarMem(ppmodlf);
	AssertVarMem(*ppmodlf);

	bool fRet = fFalse;
	long cbrgbrv, cbrgbrf;
	PMODLF pmodlf;

	BrModelPrepare(pbmdl, BR_MPREP_ALL);

	/* REVIEW peted: is there an accepted way of determining if BrModelPrepare
		has failed?  Can it fail?  I think I can at least do the following... */
	if (pbmdl->nvertices > 0 && pbmdl->nprepared_vertices == 0)
		goto LFail;
	if (pbmdl->nfaces > 0 && pbmdl->nprepared_faces == 0)
		goto LFail;

	cbrgbrv = LwMul(pbmdl->nprepared_vertices, size(BRV));
	cbrgbrf = LwMul(pbmdl->nprepared_faces, size(BRF));
	if (!FAllocPv((void **)&pmodlf, size(MODLF) + cbrgbrv + cbrgbrf, fmemClear, mprNormal))
		goto LFail;

	CopyPb(*ppmodlf, pmodlf, size(MODLF));
	pmodlf->cver = pbmdl->nprepared_vertices;
	pmodlf->cfac = pbmdl->nprepared_faces;
	pmodlf->rRadius = pbmdl->radius;
	pmodlf->brb = pbmdl->bounds;
	CopyPb(pbmdl->prepared_vertices, PvAddBv(pmodlf, size(MODLF)), cbrgbrv);
	CopyPb(pbmdl->prepared_faces, PvAddBv(pmodlf, size(MODLF) + cbrgbrv), cbrgbrf);

	FreePpv((void **)ppmodlf);
	*ppmodlf = pmodlf;
	*pcb = size(MODLF) + cbrgbrv + cbrgbrf;

	fRet = fTrue;
LFail:
	return fRet;
}


/*-----------------------------------------------------------------------------
|	_FSetCps
|		Sets a Cel Part Spec.
|	
|	Arguments:
|		PBMHR pbmhr -- pointer to Brender mode hierarchy node to use
|		CPS *pcps -- pointer to CPS to fill in
|	
|	Returns:
|		fTrue if it was successful, fFalse otherwise
|	Keywords:
|	
-------------------------------------------------------------PETED-----------*/
bool S2B::_FSetCps(PBMHR pbmhr, CPS *pcps)
{
	long imat34;
	CHID chid;

	if (pbmhr->fAccessory)
		{
		if (pbmhr->pmodlf->cver != 0)
			printf("Warning: non-null accessory node during action definition;"
				" discarding mesh info\n");
		pcps->chidModl = (short)ivNil;
		BrMatrix34Identity(&pbmhr->bmat34);
		}
	else
		{
		if (!_FChidFromModlf(pbmhr, &chid))
			goto LFail;
		pcps->chidModl = (short)chid;
		}

	if (!_FImat34GetBmat34(&pbmhr->bmat34, &imat34))
		{
		printf("Couldn't add entry to GLXF -- OOM\n");
		goto LFail;
		}
	Assert(imat34 <= kswMax, 0);
	pcps->imat34 = (short)imat34;
	return fTrue;
LFail:
	return fFalse;
}

/*-----------------------------------------------------------------------------
|	_FChidFromModlf
|		Looks up a given MODLF, and adds a new one to the MODLF database
|	if necessary.
|	
|	Arguments:
|		PBMHR pbmhr -- points to BMHR node that contains the pmodlf
|		CHID *pchid -- points to CHID var that takes the result
|	
|	Returns:
|		fTrue if it could find or allocate the new MODLF node, fFalse otherwise
|		*pchid takes the CHID for the MODL chunk
|	
-------------------------------------------------------------PETED-----------*/
bool S2B::_FChidFromModlf(PBMHR pbmhr, CHID *pchid, PBMDB *ppbmdb)
{
	AssertNilOrVarMem(pchid);
	AssertNilOrVarMem(ppbmdb);

#if HASH_FIXED
	PBMDB *ppbmdb;
#endif /* HASH_FIXED */
	PBMDB pbmdb;
	uint luHashList;

	if ((pbmdb = _PbmdbFindModlf(pbmhr->pmodlf, pbmhr->cbModlf, &luHashList)) == pvNil)
		{
		if (FAllocPv((void **)&pbmdb, size(BMDB), fmemNil, mprNormal))
			{
			/* Copy the pointers */
			pbmdb->pmodlf = pbmhr->pmodlf;
			pbmhr->pmodlf = pvNil;
			pbmdb->pszName = pbmhr->pszName;
			pbmhr->pszName = pvNil;

			pbmdb->cbModlf = pbmhr->cbModlf;
			if (ppbmdb != pvNil)
				pbmdb->chidBmdl = _chidBmdl++;
			else
				pbmdb->chidBmdl = chidNil;
			pbmdb->cnoBmdl = CnoNext();
			pbmdb->pglkidCmtl = pvNil;
			pbmdb->hshdb.luHash = luHashList;

			pbmdb->fSpherical = pbmhr->fSpherical;
			pbmdb->fFixWrap = pbmhr->fFixWrap;

#if HASH_FIXED
			pbmdb->hshdb.phshdbNext = (PHSHDB)*(ppbmdb = (_prgpbmdb + luHashList));
			*ppbmdb = pbmdb;
#else /* HASH_FIXED */
			Assert(pbmdb == (PBMDB)&pbmdb->hshdb, "Bad struct definition for BMDB");
			if (!_FInsertPhshdb(&pbmdb->hshdb, _pglpbmdb))
				{
				FreePpv((void **)&pbmdb);
				printf("Couldn't add node to model database\n");
				return fFalse;
				}
#endif /* !HASH_FIXED */
			}
		else
			{
			printf("Couldn't allocate Model DB node\n");
			return fFalse;
			}
		}
	else if (pbmdb->fSpherical != pbmhr->fSpherical ||
			pbmdb->fFixWrap != pbmdb->fFixWrap)
		{
		printf("Warning: mesh found with identical vertices and u/v coordinates"
			"\n    but different texture mapping modes\n");
		}

	if (ppbmdb != pvNil)
		*ppbmdb = pbmdb;
	else if (pbmdb->chidBmdl == chidNil)
		pbmdb->chidBmdl = _chidBmdl++;

	if (pchid != pvNil)
		*pchid = pbmdb->chidBmdl;

	return fTrue;
}


bool S2B::_FAddBmdlParent(PBMDB pbmdb, KID *pkid)
{
	AssertVarMem(pkid);

	if (pbmdb->pglkidCmtl == pvNil &&
			(pbmdb->pglkidCmtl = GL::PglNew(size(KID))) == pvNil)
		{
		goto LFail;
		}
	if (!pbmdb->pglkidCmtl->FAdd(pkid))
		goto LFail;
	return fTrue;
LFail:
	return fFalse;
}


#if !HASH_FIXED

/******************************************************************************
	_FInsertPhshdb
		Inserts an hash database entry into a sparse, sorted hash table
	
	Arguments:
		PHSHDB phshdb  -- pointer to the hash DB entry to insert
		PGL pglphshdb  -- the GL used to maintain the sorted hash table
	
	Returns: fTrue if the entry could be inserted
	
************************************************************ PETED ***********/
bool S2B::_FInsertPhshdb(PHSHDB phshdb, PGL pglphshdb)
{
	long iphshdb;

	if (!_FIphshdbFromLuHash(phshdb->luHash, &iphshdb, pglphshdb))
		{
		phshdb->phshdbNext = pvNil;
		if (!pglphshdb->FInsert(iphshdb, &phshdb))
			return fFalse;
		}
	else
		{
		pglphshdb->Get(iphshdb, &phshdb->phshdbNext);
		pglphshdb->Put(iphshdb, &phshdb);
		}
	return fTrue;
}

/******************************************************************************
	_FIphshdbFromLuHash
		Determines the position that a given hash value does, or should be,
		located in the given sparse, sorted hash table.
	
	Arguments:
		uint luHash     -- the hash value to look for
		long *piphshdb  -- takes the position in the hash table
		PGL pglphshdb   -- the hash table
	
	Returns: fTrue if the hash value is already in the hash table, fFalse if
		it needs to be added
	
************************************************************ PETED ***********/
bool S2B::_FIphshdbFromLuHash(uint luHash, long *piphshdb, PGL pglphshdb)
{
	bool fRet = fFalse;
	long iphshdbMin = 0, iphshdbMac = pglphshdb->IvMac();
	long iphshdbCur;
	PHSHDB phshdb;

	while (iphshdbMin != iphshdbMac)
		{
		iphshdbCur = (iphshdbMin + iphshdbMac) / 2;
		pglphshdb->Get(iphshdbCur, &phshdb);
		if (luHash > phshdb->luHash)
			iphshdbMin = iphshdbCur + 1;
		else
			iphshdbMac = iphshdbCur;
		}
	if (iphshdbMin < pglphshdb->IvMac())
		{
		if (iphshdbCur != iphshdbMin)
			pglphshdb->Get(iphshdbMin, &phshdb);
		if (luHash == phshdb->luHash)
			fRet = fTrue;
		}
	*piphshdb = iphshdbMin;

	return fRet;
}

#endif /* !HASH_FIXED */


/******************************************************************************
	_PbmdbFindModlf
		Given a MODLF, look for an identical one in our hash table.
	
	Arguments:
		MODLF *pmodlf   -- the MODLF to look for
		int *pluHashList -- returns the hash value for the MODLF
	
	Returns: if the MODLF could be found, returns the pointer to the
		BMDB for that MODLF, otherwise returns pvNil.
	
************************************************************ PETED ***********/
PBMDB S2B::_PbmdbFindModlf(MODLF *pmodlf, long cbModlf, uint *pluHashList)
{
	PBMDB pbmdb;
#if !HASH_FIXED
	long ipbmdb;
#endif /* !HASH_FIXED */

	*pluHashList = _LuHashBytes(kluHashInit, pmodlf, cbModlf);
#if HASH_FIXED
	pbmdb = *(_prgpbmdb + *pluHashList);
#else /* HASH_FIXED */
	if (!_FIphshdbFromLuHash(*pluHashList, &ipbmdb, _pglpbmdb))
		return pvNil;
	_pglpbmdb->Get(ipbmdb, &pbmdb);
#endif /* !HASH_FIXED */
	while (pbmdb != pvNil)
		{
		if (pbmdb->cbModlf == cbModlf &&
				(FcmpCompareRgb(pbmdb->pmodlf, pmodlf, cbModlf) == fcmpEq))
			break;
		pbmdb = (PBMDB)pbmdb->hshdb.phshdbNext;
		}
	return pbmdb;
}


uint _rgluHashCrc[256];
Debug(bool _fTableValid = fFalse;)

/******************************************************************************
	_InitCrcTable
		Initialize the 8-bit hash value look-up table
	
************************************************************ PETED ***********/
void S2B::_InitCrcTable(void)
{
	int icrc;

	/* Compute the hash value for each 8-bit byte value */
	for (icrc = 0; icrc < 256; icrc++)
		{
		byte bcrc = (byte)icrc;

		_rgluHashCrc[bcrc] = _LuHashBytesNoTable(0, &bcrc, 1);
		}

	Debug(_fTableValid = fTrue;)
}

/******************************************************************************
	_LuHashBytesNoTable
		Compute a hash value the hard way.
	
	Arguments:
		uint luHash  -- initial hash value
		void *pv     -- pointer to the data to hash
		long cb      -- number bytes in the data to hash
	
	Returns: the hash value for the bytes
	
************************************************************ PETED ***********/
uint S2B::_LuHashBytesNoTable(uint luHash, void *pv, long cb)
{
	byte *pb = (byte *)pv;
	int ibit;

	while (cb--)
		{
		luHash = (luHash ^ *pb++) & 0xFF;
		for (ibit = 0; ibit < 8; ibit++)
			if (luHash & 0x01)
				luHash = (luHash >> 1) ^ kluCrcPoly;
			else
				luHash = luHash >> 1;
		}
	return luHash;
}

/******************************************************************************
	_LuHashBytes
		Compute a hash value using the lookup table
	
	Arguments:
		uint luHash  -- initial hash value
		void *pv     -- pointer to the data to hash
		long cb      -- number of bytes in the data to hash
	
	Returns: the hash value for the bytes
	
************************************************************ PETED ***********/
uint S2B::_LuHashBytes(uint luHash, void *pv, long cb)
{
	byte *pb = (byte *)pv;
	byte bCval;

	Assert(_fTableValid, "Can't use table hashing with uninited table");

	while (cb--)
		{
		bCval = (luHash ^ *pb++) & 0xFF;
		luHash = (luHash >> 8) ^ _rgluHashCrc[bCval];
		}
	return luHash;
}

/*-----------------------------------------------------------------------------
|	_FImat34GetBmat34
|		Looks up a given BMAT34 in the transformation database, and returns
|	with the index for the found or created transformation.  (NOTE: for now,
|	always adds a new transformation)
|	
|	Arguments:
|		BMAT34 *pbmat34 -- points to BMAT34 to look for
|		long *pimat34 -- points to long var that takes the result
|	
|	Returns:
|		fTrue if it succeeds, fFalse otherwise
|		*pimat34 takes the index for the BMAT34 in the GL
|
-------------------------------------------------------------PETED-----------*/
bool S2B::_FImat34GetBmat34(BMAT34 *pbmat34, long *pimat34)
{
	BMAT34 *pbmat34T;
#if HASH_FIXED
	long i, iMac = _pglxf->IvMac();
#else /* HASH_FIXED */
	long ipbmatdb;
	uint luHash;
	PBMATDB pbmatdb;
#endif /* !HASH_FIXED */

#if HASH_FIXED
	for (i = 0; i < iMac; i++)
		{
		pbmat34T = (BMAT34 *)_pglxf->QvGet(i);
		if (FcmpCompareRgb(pbmat34, pbmat34T, size(BMAT34)) == fcmpEq)
			{
			*pimat34 = i;
			return fTrue;
			}
		}
	return _pglxf->FAdd(pbmat34, pimat34);
#else /* HASH_FIXED */
	luHash = _LuHashBytes(kluHashInit, pbmat34, size(BMAT34));

	if (!_FIphshdbFromLuHash(luHash, &ipbmatdb, _pglpbmatdb))
		goto LAddXF;

	_pglpbmatdb->Get(ipbmatdb, &pbmatdb);
	while (pbmatdb != pvNil)
		{
		pbmat34T = (BMAT34 *)_pglxf->QvGet(pbmatdb->ixf);
		if (FcmpCompareRgb(pbmat34, pbmat34T, size(BMAT34)) == fcmpEq)
			break;
		pbmatdb = (PBMATDB)pbmatdb->hshdb.phshdbNext;
		}
	if (pbmatdb == pvNil)
		{
LAddXF:
		/* Add the XF to the GL and to the database */
		if (_pglxf->FAdd(pbmat34, pimat34))
			{
			if (FAllocPv((void **)&pbmatdb, size(BMATDB), fmemNil, mprNormal))
				{
				pbmatdb->ixf = *pimat34;
				pbmatdb->hshdb.luHash = luHash;
				Assert(pbmatdb == (PBMATDB)&pbmatdb->hshdb, "Bad struct definition for BMATDB");
				if (!_FInsertPhshdb(&pbmatdb->hshdb, _pglpbmatdb))
					{
					FreePpv((void **)&pbmatdb);
					printf("Error: Couldn't add node to transform database\n");
					goto LFail;
					}
				}
			else
				{
				printf("Error: Couldn't allocate Transform DB node\n");
				goto LFail;
				}
			}
		else
			goto LFail;
		}
	*pimat34 = pbmatdb->ixf;
	return fTrue;
LFail:
	return fFalse;
#endif /* !HASH_FIXED */

}

/*-----------------------------------------------------------------------------
|	_DisposeBmhr
|		Deletes the entire tree of Brender model nodes.
|	
|	Arguments:
|		PBMHR *ppbmhr -- points to pointer that points to the tree
|	
-------------------------------------------------------------PETED-----------*/
void S2B::_DisposeBmhr(PBMHR *ppbmhr)
{
	PBMHR pbmhrNext;

	while (*ppbmhr != pvNil)
		{
		pbmhrNext = (*ppbmhr)->pbmhrSibling;
		_DisposeBmhr(&(*ppbmhr)->pbmhrChild);
		FreePpv((void **)&(*ppbmhr)->pmodlf);
		FreePpv((void **)&(*ppbmhr)->pszName);
		if ((*ppbmhr)->pstnMtrlFile != pvNil)
			delete (*ppbmhr)->pstnMtrlFile;
		FreePpv((void **)ppbmhr);
		*ppbmhr = pbmhrNext;
		}
}

/******************************************************************************
	PglcrngFromPal
		Will attempt to extract color ranges from a palette.  May be
	obsolete.
	
	Arguments:
		PGL pglclr -- the palette to use
	
	Returns: a GL of CRNGs, each entry is the description of one color range
	
************************************************************ PETED ***********/
PGL PglcrngFromPal(PGL pglclr)
{
	long lwCur, lwStart, lwMac = pglclr->IvMac();
	BRCLR brclr;
	BRS brsRLast, brsGLast, brsBLast;
	BRS brsNorm, brsR, brsG, brsB;
	CRNG crng;
	PGL pglcrng = GL::PglNew(size(CRNG));

	if (pglcrng == pvNil)
		goto LFail;

	for (lwCur = 0, lwStart = 0; lwCur < lwMac; lwCur++)
		{
		/* Get a normalized color */
		pglclr->Get(lwCur, &brclr);
		brsR = BrIntToScalar(BR_RED(brclr));
		brsG = BrIntToScalar(BR_GRN(brclr));
		brsB = BrIntToScalar(BR_BLU(brclr));
		brsNorm = max(max(brsR, brsG), brsB);
		if (brsNorm > 0)
			{
			brsR = BR_ROUND(BR_DIV(brsR, brsNorm), 2);
			brsG = BR_ROUND(BR_DIV(brsG, brsNorm), 2);
			brsB = BR_ROUND(BR_DIV(brsB, brsNorm), 2);
			}

		/* Is it possible that we can include it in our range? */
		if (lwCur > 0 && brsNorm > 0)
			{
			/* If the potential starting entry was 0,0,0 then set it to
				the current normalized color, and continue */
			if (brsRLast == 0 && brsGLast == 0 && brsBLast == 0)
				{
				brsRLast = brsR;
				brsGLast = brsG;
				brsBLast = brsB;
				continue;
				}

			/* Well, at least this criteria catches exactly the ranges we
				currently define.  I have no idea how well it will fare with
				the final ranges. */
			if (brsRLast == brsGLast && brsR == brsG)
				continue;
			if (brsRLast == brsBLast && brsR == brsB)
				continue;
			if (brsGLast == brsBLast && brsG == brsB)
				continue;
			}

		/* Should we add this range to our list? */
		if ((lwCur - lwStart) > 2)
			{
			crng.lwBase = lwStart;
			crng.lwRange = lwCur - lwStart;
			if (!pglcrng->FAdd(&crng))
				{
				ReleasePpo(&pglcrng);
				goto LFail;
				}
			}

		brsRLast = brsR;
		brsGLast = brsG;
		brsBLast = brsB;
		lwStart = lwCur;
		}

LFail:
	return pglcrng;
}

/******************************************************************************
	LwcrngNearestBrclr
		Given a Brender color, find the color range that the color will most
	accurately fit into.
	
	Arguments:
		BRCLR brclr -- the Brender color
		PGL pglclr  -- the palette
		PGL pglcrng -- description of the color ranges in the palette
	
	Returns: returns the entry in the color range GL that corresponds to the
		given color
	
************************************************************ PETED ***********/
long LwcrngNearestBrclr(BRCLR brclr, PGL pglclr, PGL pglcrng)
{
	long lwclr, lwclrMac = pglclr->IvMac(), lwclrNear,
		lwcrng, lwcrngMac = pglcrng->IvMac(), lwcrngNear,
		dclrNear = klwMax, dclrT;
	CRNG crng;
	BRCLR brclrT, brclrNear;

	/* For each color range */
	for (lwcrng = 0; lwcrng < lwcrngMac; lwcrng++)
		{
		pglcrng->Get(lwcrng, &crng);
		lwclrMac = crng.lwBase + crng.lwRange;
		Assert(crng.lwRange > 0 && lwclrMac <= pglclr->IvMac(), "Invalid range");

		/* For each color in the given range */
		for (lwclr = crng.lwBase; lwclr < lwclrMac; lwclr++)
			{
			/* Calculate a city-block distance for the color */
			pglclr->Get(lwclr, &brclrT);
			dclrT = LwAbs(BR_RED(brclrT) - BR_RED(brclr));
			dclrT += LwAbs(BR_GRN(brclrT) - BR_GRN(brclr));
			dclrT += LwAbs(BR_BLU(brclrT) - BR_BLU(brclr));

			/* If it's closer than what we've found so far, remember it */
			if (dclrT < dclrNear)
				{
				lwclrNear = lwclr;
				lwcrngNear = lwcrng;
				dclrNear = dclrT;
				brclrNear = brclrT;
				if (dclrT == 0)
					break;
				}
			}

		/* If we found an exact match, look no further */
		if (dclrT == 0)
			break;
		}

	/* Report and return */
	if (dclrT != 0)
		printf("Didn't find exact match for color %06x; nearest was %06x\n",
				brclr, brclrNear);
	return lwcrngNear;
}

/* Array of keywords known by our simple script interpreter */
static KEYTT _rgkeyttS2B[] =
	{
	"ACTOR", ttActor,
	"ACTION", ttActionS2B,
	"BACKGROUND", ttBackgroundS2B,
    DK_A_CAMERA_POS_TOKEN, ttPosition,
    DK_A_CAMERA_INT_TOKEN, ttInterest,
	DK_A_CAMERA_NEAR_TOKEN, ttNearCam,
	DK_A_CAMERA_FAR_TOKEN, ttFarCam,
	DK_A_CAMERA_FOV_TOKEN, ttFovCam,
    DK_A_POS_STATIC_TOKEN, ttStatic,
	"CNO", ttCno,
	"NAMED", ttCalled,
	"XREST", ttXRest,
	"YREST", ttYRest,
	"ZREST", ttZRest,
	"FILEBASE", ttFilebase,
	"FIRST", ttFirst,
	"LAST", ttLast,
	"FLAGS", ttFlags,
	"SCALE", ttScale,
	"SKIP", ttSkip,
	"SUBMODEL", ttSubmodel,
	"BPSETS", ttBPS,
	"LIGHTS", ttLights,
	"CAMERAS", ttCameras,
	"LENGTH", ttLength,
	"STEP", ttStep,
	"COSTUME", ttCostume,
	"USE_SETS", ttUseSets,
	"NEW_ACTOR_POS", ttNewActorPos,
	"MATERIALS", ttMaterials,
	"FILE", ttFilename
	};

#define kckeyttS2B (size(_rgkeyttS2B)/size(_rgkeyttS2B[0]))

/***************************************************************************
	Read a number.  The first character is passed in ch.  lwBase is the base
	of the number (must be <= 10).
***************************************************************************/
void S2BLX::_ReadNumTok(PTOK ptok, achar ch, long lwBase, long cchMax)
{
	AssertThis(0);
	AssertVarMem(ptok);
	AssertIn(ch, ChLit('0'), ChLit('0') + lwBase);
	AssertIn(lwBase, 2, 11);
	long lw;

	lw = ch - ChLit('0');
	while (--cchMax > 0 && _FFetchRgch(&ch) &&
			(_GrfctCh(ch) & fctDec) && (ch - ChLit('0') < lwBase))
		{
		lw = lw * lwBase + (ch - ChLit('0'));
		_Advance();
		}
	if (ch == ChLit('.'))
		{
		/* HACK: if we didn't read any non-zero digits yet, and we hit
			a decimal point, assume we're really reading lwBase 10 */
		if (lw == 0)
			lwBase = 10;

		long lwFrac = lwBase;
		double fl = (double)0.0;

		_Advance();
		while (--cchMax > 0 && _FFetchRgch(&ch) &&
				(_GrfctCh(ch) & fctDec) && (ch - ChLit('0') < lwBase))
			{
			fl += ((double)(ch - ChLit('0'))) / (double)lwFrac;
			lwFrac *= lwBase;
			_Advance();
			}
		ptok->tt = ttFloat;
		_fl = lw + fl;
		}
	else
		ptok->lw = lw;
}


/***************************************************************************
	Reads in the next token.  Resolves certain names to keyword tokens.
***************************************************************************/
bool S2BLX::FGetS2btk(PS2BTK ps2btk)
{
	AssertThis(0);
	AssertVarMem(ps2btk);
	bool fRet;
	long ikeytt;
	PTOK ptok = &ps2btk->tok;

	while ((fRet = S2BLX_PAR::FGetTok(ptok)) &&
		(ptok->tt == ttComma || ptok->tt == ttSemi))
		;

	if (fRet)
		{
		/* Ignore minus signs, unless they precede a number */
		if (ptok->tt == ttSub)
			{
			if (!(fRet = S2BLX_PAR::FGetTok(ptok)))
				goto LDone;
			switch (ptok->tt)
				{
			case ttLong:
				ptok->lw = -ptok->lw;
				goto LDone;
			case ttFloat:
				_fl = -_fl;
				goto LDone;
			default:
				break;
				}
			}
	
		if (ptok->tt != ttName)
			goto LDone;
	
		//check for a keyword
		for (ikeytt = 0; ikeytt < kckeyttS2B; ikeytt++)
			{
			if (ptok->stn.FEqualUserSz(_rgkeyttS2B[ikeytt].pszKeyword))
				{
				ptok->tt = _rgkeyttS2B[ikeytt].tt;
				break;
				}
			}
		}

LDone:
	_ttCur = ptok->tt;
	if (_ttCur == ttFloat)
		ps2btk->fl = _fl;
	return fRet;
}


/******************************************************************************
	FGetTok
		Gets a base TOK.  Will filter out ttFloat.
	
	Arguments:
		PTOK ptok -- pointer to TOK to fill in
	
	Returns: fTrue if it got a valid token
	
************************************************************ PETED ***********/
bool S2BLX::FGetTok(PTOK ptok)
{
	bool fRet;
	S2BTK s2btk;

	fRet = FGetS2btk(&s2btk);
	if (fRet)
		{
		*ptok = s2btk.tok;
		/* Convert token if necessary */
		if (ptok->tt == ttFloat)
			{
			ptok->lw = (long)_fl;
			ptok->tt = ttLong;
			}
		}
	return fRet;
}

/******************************************************************************
	FTextFromTt
		Sets an STN to the text string that corresponds to the given token.
	
	Arguments:
		long tt   -- the token
		PSTN pstn -- the STN to set
	
	Returns: fTrue if if knew about the token, fFalse otherwise
	
************************************************************ PETED ***********/
bool S2BLX::FTextFromTt(long tt, PSTN pstn)
{
	AssertPo(pstn, 0);

	long ikeytt;

	for (ikeytt = 0; ikeytt < kckeyttS2B; ikeytt++)
		if (tt == _rgkeyttS2B[ikeytt].tt)
			break;

	if (ikeytt == kckeyttS2B)
//		return S2BLX_PAR::FTextFromTt(tt, pstn);
		return fFalse;

	pstn->SetSz(_rgkeyttS2B[ikeytt].pszKeyword);
	return fTrue;
}


/******************************************************************************
	FTextFromS2btk
		Fills in the given STN with text that represents the given token.
	
	Arguments:
		PS2BTK ps2btk -- the token to use
		PSTN pstn     -- the STN to fill in
	
	Returns: fTrue if the token was successfully recognized and converted
		to text, fFalse otherwise
	
************************************************************ PETED ***********/
bool S2BLX::FTextFromS2btk(PS2BTK ps2btk, PSTN pstn)
{
	AssertPo(pstn, 0);

	pstn->SetNil();
	if (ps2btk->tok.tt == ttFloat)
		{
		long lwInt, lwFrac;

		lwInt = (long)ps2btk->fl;
		lwFrac = (long)((ps2btk->fl - lwInt) * 100000.0);
		AssertIn(lwFrac, 0, 100000);
		AssertDo(pstn->FFormatSz(PszLit("%d.%05d"), lwInt, lwFrac), 0);
		return fTrue;
		}

//	return FTextFromTok(&ps2btk->tok, pstn);
	PTOK ptok = &ps2btk->tok;

	switch (ptok->tt)
		{
	case ttLong:
		AssertDo(pstn->FFormatSz(PszLit("%d"), ptok->lw), 0);
		return fTrue;
	case ttName:
		*pstn = ptok->stn;
		return fTrue;
	case ttString:
		/* Creating the string this way will guarantee the double-quotes will
			make it into the string; the source string *may* be truncated by
			a character or two. */
		*pstn = PszLit("\"");
		pstn->FAppendStn(&ptok->stn);
		pstn->FInsertCh(pstn->Cch(), ChLit('"'));
		return fTrue;
	default:
		return FTextFromTt(ptok->tt, pstn);
		}
}

#ifdef DEBUG
/***************************************************************************

	Warning proc called by Warn() macro

***************************************************************************/
void WarnProc(PSZ pszFile, long lwLine, PSZ pszMessage)
{
	if (_fEnableWarnings)
		{
		if (pszMessage != pvNil)
			fprintf(stderr, "Warning: %s\n", pszMessage);
		else
			fprintf(stderr, "%s(%ld) : warning\n", pszFile, lwLine);
		}
}


/***************************************************************************

	Returning true breaks into the debugger.

***************************************************************************/
bool FAssertProc(PSZ pszFile, long lwLine, PSZ pszMessage,
	void *pv, long cb)
{
	fprintf(stderr, "An assert occurred: \n");
	if (pszMessage != pvNil)
		fprintf(stderr, "   Message: %s\n", pszMessage);
	if (pv != pvNil)
		{
		fprintf(stderr, "   Address %x\n", pv);
		if (cb != 0)
			{
			fprintf(stderr, "   Value: ");
			switch (cb)
				{
			default:
				{
				byte *pb;
				byte *pbLim;

				for (pb = (byte *)pv, pbLim = pb + cb; pb < pbLim; pb++)
					fprintf(stderr, "%02x", (int)*pb);
				}
				break;

			case 2:
				fprintf(stderr, "%04x", (int)*(short *)pv);
				break;

			case 4:
				fprintf(stderr, "%08lx", *(long *)pv);
				break;
				}
			printf("\n");
			}
		}
	fprintf(stderr, "   File: %s\n", pszFile);
	fprintf(stderr, "   Line: %ld\n", lwLine);

	return _fBreak;
}
#endif
