/***************************************************************************
**
**	File:			SetupKit.h
**	Purpose:		Toolkit types, defines, and prototypes.
**	Notes:
**
****************************************************************************/

#ifndef SETUPKIT_H
#define SETUPKIT_H

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif


/*
**	Purpose:
**		Calculates the number of bytes that a string occupies (not including
**		the terminating zero character).
**	Arguments:
**		sz: string whose length is to be calculated.
**	Returns:
**		0 if sz was NULL.
**		The number of bytes from the beginning of the string to its
**			terminating zero character.
*/
#define CbStrLen(sz)              ((CB)lstrlen(sz))


/*
**	Purpose:
**    Checks if the string is empty
**	Arguments:
**		sz:
**	Returns:
**		0 if sz was NULL.
**    1 otherwise
*/
#define FEmptySz(sz)              ((BOOL)((sz) == szNull || *(sz) == chEos))

#define FValidSz(sz)              (!FEmptySz(sz))


/*
 *	CHaracter Physical representation datatype
 */
typedef BYTE             CHP;		/* CHaracter Physical */
typedef CHP *            PCHP;		/* Ptr to CHaracter Physical */
typedef CB               CCHP;		/* Count of CHaracter Physical */
typedef CCHP *           PCCHP;		/* Ptr to Count of CHaracter Physical */

#define pchpNull         ((PCHP)NULL)
#define pcchpNull        ((PCCHP)NULL)
#define CbFromCchp(cchp) ((CB)(cchp))


/*
 *	CHaracter Logical representation datatype
 */
typedef CHP              CHL;		/* CHaracter Logical */
typedef CHL *            PCHL;		/* Ptr to CHaracter Logical */
typedef PCHL *           PPCHL;		/* Ptr to Ptr to CHaracter Logical */
typedef CB               CCHL;		/* Count of CHaracter Logical */

#define pchlNull         ((PCHL)NULL)
#define ppchlNull        ((PPCHL)NULL)

#define cchpFullPathMax  ((CCHP)(_MAX_PATH))

#define cchGrpNameMax  30  /* right for win31, WfW, WinNT */
#ifdef OLD
#define cchGrpNameMax  ((CB)(LOWORD(GetVersion()) == 0x0003 ? 24 : 29))
#endif /* OLD */

/*
 *	Maximum size block that can be allocated (bytes).
 */
#define cbAllocMax  ((CB)65520)


/*
 *	Path Verification Routines
 */

typedef UINT  FPV;		/* Flags for Path Validation */

#define fpvSub     0x0000  /* Must be subpath, cannot have Drive or UNC */
#define fpvUnc     0x0001  /* Validate UNC path, \\Node\share */
#define fpvDrive   0x0002  /* Validate Drive at start of path */
#define fpvFile    0x0004  /* Validate filename only, no subdirectories */
#define fpvDir     0x0010  /* Allow trailing directory separator */
#define fpvFat     0x0020  /* Restrict to only 8.3 FAT name, no LFNs */
#define fpvFull    (fpvDrive | fpvUnc) /* Full path, with Drive or UNC */
#define fpvModule  (fpvDrive | fpvUnc | fpvFile) /* Valid executable name */

/*
**	Purpose:
**		Checks the validity of a filepath or filename.
**		Allowable filepath components are controlled by bit flags.
**		Default behavior is to allow directories with filename.
**		Leading backslash is not permitted in directory name.
**	Arguments:
**		szcPath:      non-NULL string to validate
**		fpvFlags:     pathname options flags
**	Returns:
**		fTrue if szcPath is valid for requested type
**		fFalse if invalid path
***************************************************************************/

BOOL PUBLIC FValidFilePath ( SZC szcPath, FPV fpvFlags );

#define FValidDir(szcDir)          FValidFilePath(szcDir, fpvFull | fpvDir)
#define FValidPath(szcPath)        FValidFilePath(szcPath, fpvFull)
#define FValidSubDir(szc)          FValidFilePath(szc, fpvDir)
#define FValidSubPath(szc)         FValidFilePath(szc, fpvSub)
#define FValidFATSubPath(szc)      FValidFilePath(szc, fpvFat)
#define FValidFileName(szc)        FValidFilePath(szc, fpvFile)
#define FValidDllFileName(szc)     FValidFilePath(szc, fpvModule)
#define FValidDstDir(szcDir)       FValidFilePath(szcDir, fpvDrive | fpvDir)


#define FValidInfSect(cszcSect)												\
				(FValidSz(cszcSect) && !strchr(cszcSect, ']'))

#define FValidIniFile(szcFile)												\
				(FValidPath(szcFile)											\
				 || CrcStringCompareI(szcFile, "WIN.INI") == crcEqual)


/* String manipulation routines */

/*
**	Purpose:
**		Advances a string pointer to the beginning of the next valid
**		character.  This may include skipping a double-byte character.
**	Arguments:
**		sz: the string pointer to advance.  It can be NULL or empty, or else
**			it must point at the beginning of a valid character.
**	Returns:
**		NULL if sz was NULL.
**		sz unchanged if it was an empty string (*sz == '\0').
**		sz advanced past the current character and to the beginning of the
**			next valid character.
*/
#define SzNextChar(sz)            AnsiNext(sz)


/*
**	Purpose:
**		Retreats a string pointer to the beginning of the previous valid
**		character.  This may include skipping a double-byte character.
**	Arguments:
**		szStart: string pointer to the beginning of a valid character that
**			equals or preceeds the character szCur.
**		szCur:   string pointer to retreat.  It can be NULL or empty, or
**			can point to any byte in a valid character.
**	Returns:
**		NULL if szCur was NULL.
**		sz unchanged if szStart was NULL or if szCur equaled szStart.
**		sz retreated past the current character and to the beginning of the
**			previous valid character.
*/
#define SzPrevChar(szStart, szCur) AnsiPrev(szStart, szCur)


/*
**	Purpose:
**		Copies a string from one buffer to another.
**	Arguments:
**		szDst: string pointer to destination buffer.  This can be NULL or
**			else it must contain enough storage to copy szSrc with its
**			terminating zero character.
**		szSrc: string pointer to source buffer.  This can be NULL or else
**			must point to a zero terminated string (can be empty).
**	Returns:
**		NULL if either szDst or szSrc is NULL.
**		szDst signifying the operation succeeded.
*/
#define SzStrCopy(szDst, szSrc)   lstrcpy(szDst, szSrc)


/*
**	Purpose:
**		Appends a string from one buffer to another.
**	Arguments:
**		szDst: string pointer to destination buffer.  This can be NULL or
**			else it must contain a zero terminated string (can be empty)
**			and enough storage to append szSrc with its terminating zero
**			character.
**		szSrc: string pointer to source buffer.  This can be NULL or else
**			must point to a zero terminated string (can be empty).
**	Returns:
**		NULL if either szDst or szSrc is NULL.
**		szDst signifying the operation succeeded.
*/
#define SzStrCat(szDst, szSrc)    lstrcat(szDst, szSrc)


/*
**	Purpose:
**		Converts a zero-terminated string to upper case.
**	Arguments:
**		sz: the string to convert to upper case.  sz must be non-NULL though
**			it can be empty.
**	Returns:
**		A pointer to the converted string.
*/
/* REVIEW: define SzStrUpper(sz)            (sz) */


/*
**	Purpose:
**		Converts a zero-terminated string to lower case.
**	Arguments:
**		sz: the string to convert to lower case.  sz must be non-NULL though
**			it can be empty.
**	Returns:
**		A pointer to the converted string.
*/
#define SzStrLower(sz)            AnsiLower(sz)


/* Memory Handling routines */
extern PB    WINAPI PbAlloc ( CB cb );
extern BOOL  WINAPI FFree ( PB pb, CB cb );
extern PB    WINAPI PbRealloc ( PB pb, CB cbNew, CB cbOld );

#define FHandleOOM()              HandleOOM()

/*
**	Purpose:
**		Frees the memory used by an sz.  This assumes the terminating
**		zero occupies the final byte of the allocated buffer.
**	Arguments:
**		sz: the buffer to free.  this must be non-NULL though it can point
**			at an empty string.
**	Returns:
**		fTrue if the Free() operation succeeds.
**		fFalse if the Free() operation fails.
*/
#define FFreeSz(sz)               FFree((PB)(sz), CbStrLen(sz)+1)


/*
**	Purpose:
**		Shrinks a buffer to exactly fit a string.
**	Arguments:
**		sz: the string for which the buffer should shrink to.  sz must be
**			non-NULL though it can be empty.
**		cb: the size in bytes for the buffer that was originally allocated.
**			cb must be greater than or equal to CbStrLen(sz) + 1.
**	Returns:
**		A pointer to the original string if the Realloc() operation succeeds.
**		NULL if the Realloc() operation fails.
*/
#define SzReallocSz(sz, cb)   (SZ)(PbRealloc((PB)(sz), (CbStrLen(sz)+1), cb))


/*
 *	File Handle structure
 *	Fields:
 *		hFile:   Win32 file handle.
 *		iDosfh:  Only used to avoid changing old source code, same as hFile
 *		szPath:  Full path used when the file was opened.
 */
typedef union _fh		/* File Handle structure */
	{
	INT     iDosfh;  /* temp until code converted */
	HANDLE  hFile;
	} FH;

/*
 *	File Handle datatype
 */
typedef FH *    PFH;		/* Ptr to File Handle structure */
typedef PFH *   PPFH;		/* Ptr to Ptr to File Handle structure */

#define pfhNull   ((PFH)NULL)
#define ppfhNull  ((PPFH)NULL)


/*
 *	Open File Mode datatype
 */
typedef UINT  OFM;		/* Open File Mode */

#define ofmExist          ((OFM)OF_EXIST)
#define ofmRead           ((OFM)OF_READ      | OF_SHARE_DENY_WRITE)
#define ofmReadCompat     ((OFM)OF_READ      | OF_SHARE_COMPAT)
#define ofmWrite          ((OFM)OF_WRITE     | OF_SHARE_EXCLUSIVE)
#define ofmReadWrite      ((OFM)OF_READWRITE | OF_SHARE_EXCLUSIVE)
#define ofmCreate         ((OFM)OF_CREATE    | OF_SHARE_EXCLUSIVE)


/*
 *	Seek File Mode datatype
 */
typedef UINT  SFM;		/* Seek File Mode */

#define sfmSet   ((SFM)FILE_BEGIN)
#define sfmCur   ((SFM)FILE_CURRENT)
#define sfmEnd   ((SFM)FILE_END)


/*
 *	Long File Address datatype
 */
typedef unsigned long  LFA;		/* Long File Address */

#define lfaSeekError   ((LFA)HFILE_ERROR)


/*
 *	Expanded Error Return Code
 */
typedef unsigned int  EERC;		/* Expanded Error Return Code */

#define eercOkay    ((EERC)0)
#define eercAbort   ((EERC)1)
#define eercRetry   ((EERC)2)
#define eercIgnore  ((EERC)3)


/* File handling routines */
extern EERC  WINAPI EercOpenFile ( PPFH ppfh, CSZC cszcFile, OFM ofm,
								   BOOL fVital );
extern BOOL  WINAPI FFileExist ( CSZC cszcFile, OFM ofm );
extern BOOL  WINAPI FCloseFile ( PFH pfh );
extern CB    WINAPI CbReadFile ( PFH pfh, PB pbBuf, CB cbMax );
extern CB    WINAPI CbWriteFile ( PFH pfh, PB pbBuf, CB cbMax );
extern LFA   WINAPI LfaSeekFile ( PFH pfh, LONG l, SFM sfm );
extern BOOL  WINAPI FChmodFile ( CSZC cszcFileName, INT wFlags, BOOL fVital );
extern SZC WINAPI DriveNumToRootPath( INT iDrive );

/* Media source file handling routines */
extern EERC WINAPI EercOpenSrcFile ( PPFH ppfh, UINT did, SZ szSrcDir,
									 SZ szFile, BOOL fWrite );
extern EERC WINAPI EercReadSrcFile ( PFH pfh, PB pbBuf, CB cbMax );

extern EERC WINAPI EercWriteSrcFile ( PFH pfh, PB pbBuf, CB cbMax );

extern BOOL WINAPI FCloseSrcFile( PFH pfh, BOOL fRdOnly );


/*
 *	SetErrorMode type
 */
typedef unsigned int  SEM;		/* SetErrorMode type */


/*
 *	Comparison Return Code datatype
 */
typedef INT CRC;		/* Comparison Return Code */

#define crcError         ((CRC)(-2))
#define crcEqual         ((CRC)  0 )
#define crcFirstHigher   ((CRC)  1 )
#define crcSecondHigher  ((CRC)(-1))

extern SZ    PUBLIC SzDupl ( CSZC cszc );
extern CRC   WINAPI CrcStringCompare  ( CSZC cszc1, CSZC cszc2 );
extern CRC   WINAPI CrcStringCompareI ( CSZC cszc1, CSZC cszc2 );
extern SZ    WINAPI SzLastChar ( CSZC cszc );
extern CB    WINAPI CbStrCopyToBuffer ( PB pbBuf, CB cbMax, CSZC cszcSrc );


/*
 *	BindImage API, exported from MSSETUP.DLL, and/or IMAGEHLP.DLL
 */

#define IMAGEHELP_DLL  "imagehlp.dll"
#define BINDIMAGE_PROC "BindImage"
typedef BOOL (WINAPI *PFNBindImage)( SZ szImage, SZ szDllPath, SZ szSymPath);

extern BOOL WINAPI BindImage( SZ szImage, SZ szDllPath, SZ szSymPath);


/*
 *	API to set Admin mode in toolkit DLL
 */

VOID WINAPI SetAdminMode ( VOID );  /* called only by ACMSETUP.EXE */
BOOL WINAPI IsAdminMode ( VOID );

#ifdef __cplusplus
}                       /* End of extern "C" { */
#endif

#endif  /* SETUPKIT_H */
