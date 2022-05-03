/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* CSAPI.H - API entry header file for CSAPI
*
* See Csapi.Doc for details on the CSAPI.
* Note that the double slash comment // should not be used in this file
*  since THINK_C does not support this format.
*/
#ifndef CSAPI_H
#define CSAPI_H

#ifdef NT
typedef unsigned     SC_MDR;            	/* main dictionary reference*/
typedef unsigned     SC_UDR;            	/* user dictionary reference*/
#else
typedef unsigned short    SC_MDR;			/* main dictionary reference*/
typedef unsigned short    SC_UDR;          /* user dictionary reference*/
#endif
typedef unsigned short    SC_CC;         /* Spell Check Command Code */

/* SPLID is the replacement for SID due to a conflict with the Windows NT
	header files.  References to sid i.e. sidSA are left unchanged for
	the sake of less required code changes. */
typedef unsigned long     SC_SPLID;        /* Spell Id type */
typedef unsigned short    SC_SCIS;         /* SpellCheckInputStatus*/
typedef unsigned short    SC_SCRS;         /* SpellCheckReturnStatus */

typedef unsigned short SC_LID;  			/* two byte language identifier code */

/* Comment out as needed */
typedef unsigned char SC_BYTE;
typedef unsigned short SC_WORD;
typedef char SC_CHAR;
#ifdef NT
typedef int SC_BOOL;
#else
typedef short SC_BOOL;
#endif

/* All undefined or unused chars should be mapped to bIgnore. */
typedef struct WizSpecChars
{
	SC_BYTE bIgnore;
	SC_BYTE bHyphenHard;
	SC_BYTE bHyphenSoft;
	SC_BYTE bHyphenNonBreaking;
	SC_BYTE bEmDash;
	SC_BYTE bEnDash;
	SC_BYTE bEllipsis;
	SC_BYTE rgLineBreak[2];
	SC_BYTE rgParaBreak[2];
} SC_WSC;

#ifndef SC_FAR
#ifdef MAC
#define SC_FAR
#else
#define SC_FAR  far
#endif
#endif


/************************** Structure Typedefs *************/

typedef SC_WORD SC_SEC;  /* Spell Error Code.  Low byte for major code, High byte for minor.*/

typedef struct SpellInputBuffer
{
    unsigned short       cch;        /* Total characters in buffer area  */
    unsigned short       cMdr;       /* Count of MDR's specified in lrgMdr */
    unsigned short       cUdr;       /* Should not reference Exclusion UDR's.
	                                     Count of UDR's specified in lrgUdr */
	unsigned short       wSpellState; /* State relative to previous SpellCheck() call */
    SC_CHAR       SC_FAR       *lrgch;     /* ptr to buf area of text to be spell checked */
    SC_MDR        SC_FAR       *lrgMdr;    /* List of main dicts to use when spelling the buffer */
    SC_UDR        SC_FAR       *lrgUdr;    /* Should not reference Exclusion UDR's.
	                                     List of user dicts to use when spelling the buffer */
} SC_SIB;
typedef SC_SIB SC_FAR * LPSC_SIB;

typedef struct SpellReturnBuffer
{
	/* These fields are set by the SpellCheck() function */
    /* reference word in error or flagged into SC_SIB. */
    unsigned short       ichError;   /*position in the SC_SIB */
    unsigned short       cchError;   /*Length of error "word" in SC_SIB.*/

    /* These fields are set by the SpellCheck() function. */
    SC_SCRS                 scrs;       /*spell check return status. Set by SC()*/
    unsigned short       csz;        /*count of sz's put in buffer. Set by SC*/
    unsigned short       cchMac;     /* Current total of chars in buffer. */


	/* These fields MUST be set by the app, NULL pointers are invalid */
    unsigned short       cch;        /* total space in lrgch.  Set by App. */
    SC_CHAR     SC_FAR         *lrgsz;     /* ptr to alternatives.
                                        format: word\0word\0...word\0\0*/
    SC_BYTE     SC_FAR         *lrgbRating;/* ptr to Rating value for each sugg. returned.
										Parallel to lrgsz array.  Allocated by App.*/
    unsigned short       cbRate;     /* Number of elements in lrgbRating.
	                                    Set by App. lrgbRating must be this long.*/
} SC_SRB;
typedef SC_SRB SC_FAR * LPSC_SRB;

typedef struct mdrs
{
    SC_MDR                  mdr;
    SC_LID                  sclid;
    SC_UDR                  udrExc;
} SC_MDRS;
typedef SC_MDRS SC_FAR * LPSC_MDRS;

#ifndef MAC
typedef SC_CHAR SC_FAR * LPSC_PATH;    /* ptr to full Sz path string. */
#else
typedef struct spath
    {
    short volRefNum;
	long dirID;
    SC_CHAR *lpszFilePath;        /* lpSzFile is local Sz path string for MAC,
                                  it is the local file path which will be
                                  used with the volRefNum.
                               */
    } SC_PATH;
typedef SC_PATH * LPSC_PATH;
#endif /* !MAC */


/*-------------------------------------------------------*/
/* All Defines*/

/* Explicit word delimeters. */
#define chSpaceSpell   0x20    /* ' ' space.  Also used to delimit
                                  "change always" pairs */
#define chTabSpell     0x09    /* TAB.  Can be word delimeter or a
                                  string delimiter for "change once" lists.*/
/*** Additional Word Delimeters. */
/* [] {} () <> /  EmDash EnDash Ellipsis New_Paragraph */

#define chLParenSpell       0x28
#define chRParenSpell       0x29
#define chLBracketSpell     0x7B
#define chRBracketSpell     0x7D
#define chLBraceSpell       0x5B
#define chRBraceSpell       0x5D
#define chLessThanSpell     0x3C
#define chGreaterThanSpell  0x3E
#define chForwardSlashSpell 0x2F


/* Spell Id Engine Defines */
#define sidSA    1    /* SoftArt */
#define sidHM    2    /* Houghton-Mifflin (InfoSoft) */
#define sidML    3    /* MicroLytics */
#define sidLS    4    /* LanSer Data */
#define sidCT    5    /* Center of Educational Technology */
#define sidHS    6    /* HSoft */
#define sidMO    7    /* MorphoLogic */
#define sidTI    8    /* TiP */
#define sidKF    9    /* Korea Foreign Language University */
#define sidPI   10    /* Priberam Informatica Lince */
#define sidGS   11    /* Glyph Systems */
#define sidRA   12    /* Radiar */
#define sidIN   13    /* Intracom */


/* IPG two byte language id's.  Returned in SC_LID field. */
#define ksclidAmerican         0x0409  /* "AM" American English   */
#define ksclidAustralian       0x0c09  /* "EA" English Australian */
#define ksclidBritish          0x0809  /* "BR" English            */
#define ksclidCatalan          0x0403  /* "CT" Catalan            */
#define ksclidDanish           0x0406  /* "DA" Danish             */
#define ksclidDutch            0x0413  /* "NL" Dutch              */
#define ksclidDutchPreferred   0x0013  /* "NL" Dutch Preferred    */
#define ksclidFinnish          0x040b  /* "FI" Finish             */
#define ksclidFrench           0x040c  /* "FR" French             */
#define ksclidFrenchCanadian   0x0c0c  /* "FC" French Canadian    */
#define ksclidGerman           0x0407  /* "GE" German             */
#define ksclidSwissGerman      0x0807  /* "GS" German Swiss       */
#define ksclidItalian          0x0410  /* "IT" Italian            */
#define ksclidNorskBokmal      0x0414  /* "NO" Norwegian Bokmal   */
#define ksclidNorskNynorsk     0x0814  /* "NN" Norwegian Nynorsk  */
#define ksclidPortBrazil       0x0416  /* "PB" Portuguese Brazil  */
#define ksclidPortIberian      0x0816  /* "PT" Portuguese Iberian */
#define ksclidSpanish          0x040a  /* "SP" Spanish            */
#define ksclidSwedish          0x041d  /* "SW" Swedish            */
#define ksclidRussian          0x0419  /* "RU" Russian            */
#define ksclidCzech            0x0405  /* "CZ" Czech              */
#define ksclidHungarian        0x040e  /* "HU" Hungarian          */
#define ksclidPolish           0x0415  /* "PL" Polish             */
#define ksclidTurkish          0x041f  /* "TR" Turkish            */
/* African languages */
#define ksclidSutu             0x0430  /* "ST" Sutu               */
#define ksclidTsonga           0x0431  /* "TS" Tsonga             */
#define ksclidTswana           0x0432  /* "TN" Tswana             */
#define ksclidVenda            0x0433  /* "VE" Venda			   */
#define ksclidXhosa            0x0434  /* "XH" Xhosa              */
#define ksclidZulu             0x0435  /* "ZU" Zulu               */
#define ksclidAfrikaans        0x0436  /* "AF" Afrikaans          */
/* These are currently not used, but added for future support. */
#define ksclidArabic           0x0401
#define ksclidHebrew           0x040d
#define ksclidJapanese         0x0411
#define ksclidLatin            0x041a /* Croato-Serbian (Latin)   */
#define ksclidCyrillic         0x081a /* Serbo-Croatian (Cyrillic) */
#define ksclidSlovak           0x041b

#define LID_UNKNOWN         0xffff
#define ksclidUnknown          0xffff


/* Ram Cache User Dictionary Reference. */
#define udrChangeOnce       0xfffc  /* UDR reserved reference for Change Once list    */
#define udrChangeAlways     0xfffd  /* UDR reserved reference for Change Always list  */
#define udrIgnoreAlways     0xfffe  /* UDR reserved reference for Ingore Always list. */

/* Word List property types.  Note: Code relies on being == to above udr's! */
#define ChangeOnceProp       udrChangeOnce
#define ChangeAlwaysProp     udrChangeAlways
#define IgnoreAlwaysProp     udrIgnoreAlways

/* Bitfield definitions for SpellInit() Status */
#define fscisWildCardSupport        0x0001
#define fscisMultiDictSupport       0x0002
#define fscisHyphenationSupport     0x0004
#define scisNULL                    0x0000

/* Spell Check Command Definitions */
#define sccVerifyWord        1
#define sccVerifyBuffer      2
#define sccSuggest           3
#define sccSuggestMore       4
#define sccHyphInfo          5
#define sccWildcard			 6
#define sccAnagram			 7

/* Flag values for SpellState field in Sib. */
#define fssIsContinued            0x0001
    /* Call is continuing from where last call returned.  Must be cleared
       for first call into SpellCheck().
    */

#define fssStartsSentence         0x0002
   /* First word in buffer is known to be start of
      sentence/paragraph/document.  This is only used if the
      fSibIsContinued bit is not set.  It should not be needed if the
      fSibIsContinued bit is being used.  If this bit is set during a
      suggestion request, suggestions will be capitalized.
   */

#define fssIsEditedChange         0x0004
   /* The run of text represented in the SC_SIB is a change from either
      a change pair (change always or change once) edit, or from a
      user specified change, possibly from a suggestion list presented
      to the user.  This text should be checked for repeat word
      problems, and possibly sentence status, but should not be subject
      to subsequent spell verification or change pair substitutions.
      Note that if an app is not using the fSibIsContinued support,
      they do not need to pass in these edited changes, thus bypassing
      the potential problem, and working faster.
   */

#define fssNoStateInfo            0x0000
   /* App is responsible for checking for all repeat word and sentence
	  punctuation, and avoiding processing loops such as change always
	  can=can can.
   */
/* End of Sib Spell State flag definitions. */


/* Spell Check return status identifiers */
#define scrsNoErrors                      0  /* All buffer processed. */
#define scrsUnknownInputWord              1  /* Unknown word. */
#define scrsReturningChangeAlways         2  /* Returning a Change Always word in SC_SRB. */
#define scrsReturningChangeOnce           3  /* Returning a Change Once word in SC_SRB. */
#define scrsInvalidHyphenation            4  /* Error in hyphenation point.*/
#define scrsErrorCapitalization           5  /* Cap pattern not valid. */
#define scrsWordConsideredAbbreviation    6  /* Word is considered an abbreviation. */
#define scrsHyphChangesSpelling           7  /* Word changes spelling when not hyphenated. */
#define scrsNoMoreSuggestions             8  /* All methods used. */
#define scrsMoreInfoThanBufferCouldHold   9  /* More return data than fit in buffer */
#define scrsNoSentenceStartCap           10  /* Start of sentence was not capitalized. */
#define scrsRepeatWord                   11  /* Repeat word found. */
#define scrsExtraSpaces                  12  /* Too many spaces for context.*/
#define scrsMissingSpace                 13  /* Too few space(s) between words or sentences. */
#define scrsInitialNumeral				 14  /* Word starts with numeral & soFlagInitialNumeral set */

/* Spell Error Codes */
#define secNOERRORS                 0
/* Major Error Codes. Low Byte of SEC*/
#define secOOM                      1
#define secModuleError              2  /* Something wrong with parameters, or state of spell module. */
#define secIOErrorMdr               3  /* Read,write,or share error with Mdr. */
#define secIOErrorUdr               4  /* Read,write,or share error with Udr. */

/* Minor Error Codes. Not set unless major code also set. */
/* High Byte of SEC word var. */
#define secModuleAlreadyBusy      (128<<8)   /* For non-reentrant code */
#define secInvalidID              (129<<8)   /* Not yet inited or already terminated.*/
#define secInvalidWsc             (130<<8)	/* Illegal values in SC_WSC struct */
#define secInvalidMdr             (131<<8)   /* Mdr not registered with spell session */
#define secInvalidUdr             (132<<8)   /* Udr not registered with spell session */
#define secInvalidSCC             (133<<8)   /* SC_CC unknown (spellcheck() only ) */
#define secInvalidMainDict        (134<<8)   /* Specified dictionary not correct format */
#define secOperNotMatchedUserDict (135<<8)   /* Illegal operation for user dictionary type. */
#define secFileReadError          (136<<8)   /* Generic read error */
#define secFileWriteError         (137<<8)   /* Generic write error */
#define secFileCreateError        (138<<8)   /* Generic create error */
#define secFileShareError         (139<<8)   /* Generic share error */
#define secModuleNotTerminated    (140<<8)   /* Module not able to be terminated completely.*/
#define secUserDictFull           (141<<8)   /* Could not update Udr without exceeding limit.*/
#define secInvalidUdrEntry        (142<<8)   /* invalid chars in string(s) */
#define secUdrEntryTooLong        (143<<8)   /* Entry too long, or invalid chars in string(s) */
#define secMdrCountExceeded       (144<<8)   /* Too many Mdr references */
#define secUdrCountExceeded       (145<<8)   /* Too many udr references */
#define secFileOpenError          (146<<8)   /* Generic Open error */
#define secFileTooLargeError	  (147<<8)	 /* Generic file too large error */
#define secUdrReadOnly            (148<<8)   /* Attempt to add to or write RO udr */


/* Spell Options bitfield definitions */
#define soSuggestFromUserDict    0x00000001L /* Suggest from user dictionaries. */
#define soIgnoreAllCaps          0x00000002L /* Ignore words in all UPPERCASE. */
#define soIgnoreMixedDigits      0x00000004L /* Ignore words with any numbers in it. */
#define soIgnoreRomanNumerals    0x00000008L /* Ignore words composed of all roman numerals. */
#define soFindUncappedSentences  0x00000010L /* Flag sentences which don't start with a cap.
												* (Soft-Art only)
												*/
#define soFindMissingSpaces      0x00000020L /* Flag missing spaces between words/sentences.
												* (Soft-Art only)
												*/
#define soFindRepeatWord         0x00000040L /* Flag repeated words. */
#define soFindExtraSpaces        0x00000080L /* Flag extra spaces between words.
												* (Soft-Art only)
												*/
#define soFindSpacesBeforePunc   0x00000100L /* Flag spaces preceeding
												*  the chars below:
												*     ) ] } > , ; % . ? !
												* (Soft-Art only)
												*/
#define soFindSpacesAfterPunc    0x00000200L /* Flag spaces following
												*  the chars below:
												*     ( [ { $
												* (Soft-Art only)
												*/
#define soRateSuggestions        0x00000400L /* Rate the suggestions on scale
												*  of 1-255, 255 being most likely
												* (Soft-Art only)
												*/
#define soFindInitialNumerals    0x00000800L /* Flag words starting with number(s) */
#define soReportUDHits           0x00001000L /* Report (via scrsNoErrorsUDHit) where
												*  user dict was used during verification
												*/
#define soQuickSuggest           0x00002000L /* Don't use typo suggest code (Soft-Art only) */
#define soUseAllOpenUdr          0x00004000L /* Automatically use all udr's opened
												* after this option is set, or all opened udr's
												* with mdr's opened after this option is set.
												* This option does not allow exclusion dicts to
												* be edited.
												* (HM only)
												*/
#define soSwapMdr                0x00008000L /* Keep the most recent 2 mdr's around.
												* swap between them instead of actually closing
												* and reopening mdr's.
												* (HM only)
												*/
#define soSglStepSugg            0x00010000L /* Break after each suggestion task for faster
												* return of control to the application.
												* (HM only)
												*/
#define soIgnoreSingleLetter     0x00020000L /* Do not check single letters: e.g., "a)".
												* (HS only)
												*/

#define soLangMode               0xF0000000L /* Language Mode mask */
/* Hebrew Language Modes -- (CT only) */
#define soHebrewFullScript            0x00000000L
#define soHebrewPartialScript         0x10000000L
#define soHebrewMixedScript           0x20000000L
#define soHebrewMixedAuthorizedScript 0x30000000L
/* French Language Modes -- (HM only) */
#define soFrenchDialectDefault        0x00000000L
#define soFrenchUnaccentedUppercase   0x10000000L
#define soFrenchAccentedUppercase     0x20000000L
/* Russian Language Modes -- (HM only) */
#define soRussianDialectDefault       0x00000000L
#define soRussianIE                   0x10000000L
#define soRussianIO                   0x20000000L


#ifdef DEBUG

#define sdcDumpRCAll     200
#define sdcDumpRCIgnore  201
#define sdcDumpRCOnce    202
#define sdcDumpRCAlways  203

#define sdcDumpUdrAll    300
#define sdcDumpUdr1      301
#define sdcDumpUdr2      302
#define sdcDumpUdr3      303
#define sdcDumpUdr4      304

#define sdcDumpMdrAll    400
#define sdcDumpMdr1      401
#endif /* DEBUG */

#ifndef SC_PASCAL
#define SC_PASCAL pascal
#endif

#define GLOBAL		SC_FAR SC_PASCAL
#define GLOBALSEC	SC_SEC SC_FAR SC_PASCAL

#ifdef MAC

/* Exported Function Prototypes */

extern GLOBALSEC SpellVer(SC_WORD SC_FAR *, SC_WORD SC_FAR *, SC_WORD SC_FAR *);
extern GLOBALSEC SpellInit(SC_SPLID SC_FAR *, SC_WSC SC_FAR *);
extern GLOBALSEC SpellOptions(SC_SPLID, long);
extern GLOBALSEC SpellCheck(SC_SPLID, SC_CC, LPSC_SIB, LPSC_SRB);
extern GLOBALSEC SpellTerminate(SC_SPLID, SC_BOOL);
extern GLOBALSEC SpellVerifyMdr(LPSC_PATH, SC_LID, SC_LID SC_FAR *);
extern GLOBALSEC SpellOpenMdr(SC_SPLID, LPSC_PATH, LPSC_PATH, SC_BOOL, SC_BOOL, SC_LID, LPSC_MDRS);
extern GLOBALSEC SpellOpenUdr(SC_SPLID, LPSC_PATH, SC_BOOL, SC_WORD, SC_UDR SC_FAR *, SC_BOOL SC_FAR *);
extern GLOBALSEC SpellAddUdr(SC_SPLID, SC_UDR, SC_CHAR SC_FAR *);
extern GLOBALSEC SpellAddChangeUdr(SC_SPLID, SC_UDR, SC_CHAR SC_FAR *, SC_CHAR SC_FAR *);
extern GLOBALSEC SpellDelUdr(SC_SPLID, SC_UDR, SC_CHAR SC_FAR *);
extern GLOBALSEC SpellClearUdr(SC_SPLID, SC_UDR);
#ifdef NT
extern GLOBALSEC SpellGetSizeUdr(SC_SPLID, SC_UDR, int *);
#else
extern GLOBALSEC SpellGetSizeUdr(SC_SPLID, SC_UDR, SC_WORD SC_FAR *);
#endif
extern GLOBALSEC SpellGetListUdr(SC_SPLID, SC_UDR, SC_WORD, LPSC_SRB);
extern GLOBALSEC SpellCloseMdr(SC_SPLID, LPSC_MDRS);
extern GLOBALSEC SpellCloseUdr(SC_SPLID, SC_UDR, SC_BOOL);

extern Handle HCsapiResInit(SC_CHAR *stzsFileName, short vRef, long dirId);
extern void CsapiResTerm(void);
extern void CsapiResFlush(void);

#endif	/* MAC */

#endif /* !CSAPI_H */

