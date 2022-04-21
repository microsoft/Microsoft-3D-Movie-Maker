/*************************************************************************

	tagman.h: Tag Manager class (TAGM)

	Primary Author: ******
	Review Status: REVIEWED - any changes to this file must be reviewed!

	BASE ---> TAGM

	A TAG is a reference to a piece of content: a background, actor
	template, sound, etc.  In addition to a CTG and CNO, a TAG specifies
	a SID, or source ID, that helps TAGM find the content.

	A source (identified by a SID) is a group of chunky files (managed
	by a CRM) in one directory of one disk whose chunks all have unique
	CTG/CNOs.  Each Socrates series member will be a source, and the
	user rolls might also be implemented as a source.  A SID of less
	than 0 is invalid; a TAG with a negative SID is an invalid TAG.

	Each source has a name, managed by _pgstSource.  This name is used
	with the _pfninscd callback when the source cannot be found.  The
	callback should put up an alert saying (for example) "The source
	*Socrates* cannot be found...please insert the CD."
	
	TAGM supports caching chunks to the local hard disk.  In Socrates,
	the studio should call FCacheTagToHD as soon as the chunk is
	requested by the kid, and when the tag is resolved to a BACO, the
	HD copy is used.  This reduces headaches about dealing with a missing
	CD all over the place.

	If a tag has ksidUseCrf for its sid, the chunk is read from the tag's
	pcrf rather than a source or a source's cache.  Use this
	functionality for things like content chunks embedded in user
	documents.

*************************************************************************/
#ifndef TAGM_H
#define TAGM_H

const long ksidInvalid = -1;		// negative SIDs imply an invalid TAG
const long sidNil = 0;
const long ksidUseCrf = 0;			// chunk is in ptag->pcrf

typedef struct TAG *PTAG;
struct TAG
	{
#ifdef DEBUG
	// I can't use the MARKMEM macro because that makes MarkMem() virtual,
	// which changes size(TAG), which I don't want to do.
	void MarkMem(void);
#endif // DEBUG

	long sid;	// Source ID (or ksidUseCrf)
	PCRF pcrf;	// File to look in for this chunk if sid is ksidUseCrf
	CTG ctg;	// CTG of chunk
	CNO cno;	// CNO of chunk
	};
const BOM kbomTag = 0xFF000000;


// FNINSCD is a client-supplied callback function to alert the user to
// insert the given CD.  The name of the source is passed to the callback.
// The function should return fTrue if the user wants to retry searching
// for the chunk, or fFalse to cancel.
typedef bool FNINSCD(PSTN pstnSourceTitle);
typedef FNINSCD *PFNINSCD;

enum
	{
	ftagmNil = 0x0000,
	ftagmFile = 0x0001,    // for ClearCache: clear HD cache
	ftagmMemory = 0x0002,  // for ClearCache: clear CRF RAM cache
	};

/****************************************
	Tag Manager class
****************************************/
typedef class TAGM *PTAGM;
#define TAGM_PAR BASE
#define kclsTAGM 'TAGM'
class TAGM : public TAGM_PAR
	{
	RTCLASS_DEC
	MARKMEM
	ASSERT

protected:
	FNI _fniHDRoot;		// Root HD directory to search for content
	long _cbCache;		// Size of RAM Cache on files in CRM for each source
	PGL _pglsfs;		// GL of source file structs
	PGST _pgstSource;	// String table of source descriptions
	PFNINSCD _pfninscd;	// Function to call when source is not found

protected:
	TAGM(void) {}
	bool _FFindSid(long sid, long *pistn = pvNil);
	bool _FGetStnMergedOfSid(long sid, PSTN pstn);
	bool _FGetStnSplitOfSid(long sid, PSTN pstnLong, PSTN pstnShort);
	bool _FRetry(long sid);
	bool _FEnsureFniCD(long sid, PFNI pfniCD, PSTN pstn = pvNil);
	bool _FFindFniCD(long sid, PFNI pfniCD, bool *pfFniChanged);
	bool _FDetermineIfSourceHD(long sid, bool *pfSourceIsOnHD);
	bool _FDetermineIfContentOnFni(PFNI pfni, bool *pfContentOnFni);

	bool _FGetFniHD(long sid, PFNI pfniHD);
	bool _FGetFniCD(long sid, PFNI pfniHD, bool fAskForCD);

	bool _FBuildFniHD(long sid, PFNI pfniHD, bool *pfExists);
	PCRM _PcrmSourceNew(long sid, PFNI pfniInfo);
	PCRM _PcrmSourceGet(long sid, bool fDontHitCD = fFalse);
	PCFL _PcflFindTag(PTAG ptag);

public:
    static PTAGM PtagmNew(PFNI pfniHDRoot, PFNINSCD pfninscd, long cbCache);
	~TAGM(void);

	// GstSource stuff:
	PGST PgstSource(void);
	bool FMergeGstSource(PGST pgst, short bo, short osk);
	bool FAddStnSource(PSTN pstnMerged, long sid);
	bool FGetSid(PSTN pstn, long *psid); // pstn can be short or long

	bool FFindFile(long sid, PSTN pstn, PFNI pfni, bool fAskForCD);
	void SplitString(PSTN pstnMerged, PSTN pstnLong, PSTN pstnShort);

	bool FBuildChildTag(PTAG ptagPar, CHID chid, CTG ctgChild, 
		PTAG ptagChild);
	bool FCacheTagToHD(PTAG ptag, bool fCacheChildChunks = fTrue);
	PBACO PbacoFetch(PTAG ptag, PFNRPO pfnrpo, bool fUseCD = fFalse);
	void ClearCache(long sid = sidNil, 
		ulong grftagm = ftagmFile | ftagmMemory); // sidNil clears all caches

	// For ksidUseCrf tags:
	static bool FOpenTag(PTAG ptag, PCRF pcrfDest, PCFL pcflSrc = pvNil);
	static bool FSaveTag(PTAG ptag, PCRF pcrf, bool fRedirect);
	static void DupTag(PTAG ptag); // call this when you're copying a tag
	static void CloseTag(PTAG ptag);

	static ulong FcmpCompareTags(PTAG ptag1, PTAG ptag2);
	};

#endif // TAGM_H
