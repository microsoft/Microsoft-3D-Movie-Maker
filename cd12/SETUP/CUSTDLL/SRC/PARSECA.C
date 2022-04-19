/***************************************************************************
**
**	File:			PARSECA.C
**	Purpose:		Custom Action data parsing routines
**					for the Sample App DLL.
**	Notes:
**
****************************************************************************/

#define PARSECA_C

#include "stdinc.h"
#include <stdlib.h>		/* _MAX_PATH */
#include <ctype.h>		/* isdigit */
#include "setupapi.h"
#include "cui.h"
#include "stdtypes.h"
#include "setupkit.h"
#include "datadef.h"
#include "parseca.h"
#include "sampcacb.h"


/* REVIEW: Put the strings in sampsz.h.
*/
#define STR_BADSECT_ERR  "InstallWinPermFile Object: Bad INF Section data value"
#define STR_BADKEY_ERR   "InstallWinPermFile Object: Bad INF Key data value"
#define STR_XTRADATA_ERR "InstallWinPermFile Object: extra unrecognized data values"
#define STR_PARSE_ERR    "Setup Parse Error in Initialize pass"
#define STR_OBJID        "Object ID %d: %s."
#define STR_MISSING_ERR  "SingSelListDlg Object: Missing data value"
#define STR_BADDATA_ERR  "SingSelListDlg Object: Bad data value"
#define STR_OBJID_ERR    "Syntax Error in List of Obj IDs"


const CHAR chFieldSep = chTab;
#define FWhiteSpaceCh(ch)	((BOOL)((ch) == chSpace || (ch) == chTab))
#define chLorSep	chComma


/*
****************************************************************************/
RC PUBLIC RcParseInfSectKey ( OR orCur, SZ szData, PSZ pszInfSect,
					PSZ pszInfKey )
{
	Assert(szData     != szNull);
	Assert(pszInfSect != pszNull);
	Assert(pszInfKey  != pszNull);

	if (*pszInfSect != szNull)	/* Already parsed and alloc'ed? */
		return (rcOk);

	if (!FReadDataFieldString(&szData, pszInfSect))
		return (rcFail);
	Assert(*pszInfSect != szNull);
	if (!FValidInfSection(*pszInfSect) || !DoesInfSectionExist(*pszInfSect))
		{
		ParseError(orCur, STR_BADSECT_ERR);
		return (rcFail);
		}

	if (!FReadDataFieldString(&szData, pszInfKey))
		return (rcFail);
	Assert(*pszInfKey != szNull);
	if (!FValidInfKey(*pszInfKey)
			|| !DoesInfSectionKeyExist(*pszInfSect, *pszInfKey))
		{
		ParseError(orCur, STR_BADKEY_ERR);
		return (rcFail);
		}

	if (*szData != chEos)
		{
		ParseError(orCur, STR_XTRADATA_ERR);
		return (rcFail);
		}

	return (rcOk);
}


/*
************************************************************************/
RC PUBLIC RcParseLor ( PCD pcd, OR orCur, SZ szData, PPLOR pplor )
{
	Assert(pcd != lpvoidNull);
	Assert(FValidOr(pcd, orCur));
	Assert(szData != szNull);
	Assert(pplor  != pplorNull);

	if (*pplor != plorNull)
		return (rcOk);

	while (FWhiteSpaceCh(*szData))
		szData = SzNextChar(szData);

	if (*szData == chEos)
		{
		ParseError(orCur, STR_MISSING_ERR);
		return (rcFail);
		}

	/* OOM or non-digit chars msg handled inside PlorFromSz()
	*/
	if ((*pplor = PlorFromSz(orCur, szData)) == plorNull)
		return (rcFail);

	if (!FValidGroupPlor(pcd, *pplor, orCur))
		{
		FFreePlor(pplor);
		ParseError(orCur, STR_BADDATA_ERR);
		return (rcFail);
		}

	return (rcOk);
}


/*
************************************************************************/
BOOL PUBLIC FReadDataFieldString ( PSZ pszData, PSZ pszMember )
{
	CHAR rgch[cchSzMax];

	Assert(pszData != pszNull);
	Assert(*pszData != szNull);
	Assert(pszMember != pszNull);

	*pszData = SzGetTableField(*pszData, rgch, cchSzMax, chComma);

	if (*pszData == szNull)
		return (fFalse);

	if ((*pszMember = SzStrDupl(rgch)) == szNull)
		return (fFalse);

	return (fTrue);
}


/*
**	Purpose:
**		Duplicates a string.
**	Arguments:
**		sz: string to duplicate.
**	Returns:
**		duplicated string if successful, szNull if not.
**	Notes:
****************************************************************************/
SZ PUBLIC SzStrDupl ( SZ sz )
{
	SZ szNew;

	Assert(sz != szNull);

	while ((szNew = (SZ)PbAlloc(CbStrLen(sz) + 1)) == szNull)
		{
		if (!FHandleOOM())
			return (szNull);
		}
	SzStrCopy(szNew, sz);
	return (szNew);
}


/*
**	Purpose:
**		Determine if a string is a valid Inf section.
**	Arguments:
**		szSection: The section.
**	Returns:
**		fTrue if the string is valid, fFalse otherwise.
**	Notes:
**
****************************************************************************/
BOOL PUBLIC FValidInfSection ( SZ szSection )
{
	SZ szLast;

	if (FEmptySz(szSection))
		return (fFalse);

	if (*szSection == chSpace)
		return (fFalse);

	szLast = szSection;
	while (*szSection != chEos)
		{
		if (*szSection == chRgtSqBracket)
			return (fFalse);
		szLast = szSection;
		szSection = SzNextChar(szSection);
		}

	if (*szLast == chSpace)
		return (fFalse);

	return (fTrue);
}


/*
**	Purpose:
**		Determine if a string is a valid Inf key.
**	Arguments:
**		szKey: The key.
**	Returns:
**		fTrue if the string is valid, fFalse otherwise.
**	Notes:
**
****************************************************************************/
BOOL PUBLIC FValidInfKey ( SZ szKey )
{
	SZ szLast;

	if (FEmptySz(szKey))
		return (fFalse);

	if (*szKey == chSpace || *szKey == chLftSqBracket)
		return (fFalse);

	szLast = szKey;
	while (*szKey != chEos)
		{
		if (*szKey == chEquals)
			return (fFalse);
		szLast = szKey;
		szKey = SzNextChar(szKey);
		}

	if (*szLast == chSpace)
		return (fFalse);

	return (fTrue);
}


/*
**	Notes
**		Each OR in the PLOR must be greater than the group OR (which
**		implicitly is greater than orNil).  This prevents circular
**		references and makes processing easier since it can run top
**		to bottom.
************************************************************************/
BOOL PUBLIC FValidGroupPlor ( PCD pcd, PLOR plor, OR orGroup )
{
	UINT ior;

	Assert(plor != plorNull);
	Assert(plor->cor > 0);
	Assert(plor->rgor != rgorNull);
	Assert(orGroup != orNil);

	for (ior = 0; ior < plor->cor; ior++)
		{
		OR   or = plor->rgor[ior];
		UINT iorT;

		if (or <= orGroup
				|| or >= pcd->cObjectsMax
				|| PodGetObjData(pcd, or) == podNull)
			{
			return (fFalse);
			}


		for (iorT = ior + 1; iorT < plor->cor; iorT++)
			{
			if (or == plor->rgor[iorT])
				return (fFalse);
			}
		}

	return (fTrue);
}


/*
**	Purpose:
**		Show a Message Box with the appropriate info.
**	Arguments:
**		or:     object reference number.
**		szcMsg: The error message.
**	Returns:
**		none.
**	Notes:
**
****************************************************************************/
VOID PUBLIC ParseError ( OR or, SZ szMsg )
{
	CHAR rgchText[2*cchSzMax];

	Assert(CbStrLen(STR_OBJID) + 5 + CbStrLen(szMsg) < sizeof rgchText);
	wsprintf(rgchText, STR_OBJID, or, szMsg);
	DoMsgBox(rgchText, STR_PARSE_ERR, MB_OK | MB_ICONEXCLAMATION);
}


/*
**	Purpose:
**		Extract the first field in szLine.
**	Arguments:
**		szLine:     The buffer to parse for a field.
**		szField:    The buffer to receive the field.
**		cbFieldMax: The size of the output buffer.
**		chSep:		The separator character.
**	Returns:
**		A pointer to the start of the next field in szLine.
**		szNull if an error was detected.
**	Notes:
**
****************************************************************************/
SZ PUBLIC SzGetTableField ( SZ szLine, SZ szField, CB cbFieldMax,
							CHAR chSep )
{
	BOOL fQuotedField   = fFalse;
	BOOL fInQuotedField = fFalse;
	CB   cbFieldMaxCur  = cbFieldMax;
	SZ   szFieldEnd     = szField;
#ifdef DEBUG
	SZ   szLineStart    = szLine;
	SZ   szFieldStart   = szField;
#endif

	Assert(szLine  != szNull);
	Assert(szField != szNull);
	Assert(cbFieldMax > 0);

	*szField = chEos;
	cbFieldMaxCur--;

	Assert(chSep != chSpace);
	while (FWhiteSpaceCh(*szLine) && *szLine != chSep)
		szLine = SzNextChar(szLine);

	if (*szLine == chDblQuote)	/* Strip off the outermost set of quotes. */
		{
		fQuotedField   = fTrue;
		fInQuotedField = fTrue;
		szLine = SzNextChar(szLine);
		}

	while (*szLine != chEos && (fInQuotedField || *szLine != chSep))
		{
		BOOL fNonSpace;

		if (*szLine == chDblQuote)
			{
			if (!fInQuotedField)
				{
				if (chSep == chFieldSep)  /* in XL tab file only */
					{
					DebugMsgBox(szLineStart, "Badly formed field: Quote in "
								"unquoted field");
					return (szNull);
					}
				}
			else
				{
				szLine = SzNextChar(szLine);
				if (*szLine == chSep || *szLine == chEos)
					{
					fInQuotedField = fFalse;
					break;
					}
				/* Consecutive double quotes are treated as one quote. */
				if (*szLine != chDblQuote)
					{
					/* Strip off trailing spaces
					*/
					while (FWhiteSpaceCh(*szLine) && *szLine != chSep)
						szLine = SzNextChar(szLine);
					if (*szLine == chSep || *szLine == chEos)
						{
						fInQuotedField = fFalse;
						break;
						}
					DebugMsgBox(szLineStart, "Badly formed field: "
								"Non-doubled quote in quoted field");
					return (szNull);
					}
				}
			}
		if (cbFieldMaxCur == 0)
			{
#ifdef DEBUG
			CHAR rgch[cchSzMax];
			
			wsprintf(rgch, "Error: Field is too long (>= %d)", cbFieldMax);
			DebugMsgBox(szLineStart, rgch);
#endif
			*szField = chEos;
			return (szNull);
			}
		fNonSpace = (*szLine != chSpace);
		CopyCharToBuf(&szLine, &szField, &cbFieldMaxCur);
		if (fNonSpace)
			szFieldEnd = szField;
		}

	if (!fQuotedField)
		szField = szFieldEnd;  /* Strip off trailing spaces */
	*szField = chEos;

	if (fInQuotedField)
		{
		DebugMsgBox(szLineStart, "Badly formed field: Unmatched quote");
		return (szNull);
		}
	Assert(*szLine == chEos || *szLine == chSep);
	if (*szLine == chSep)
		szLine = SzNextChar(szLine);

	return (szLine);
}


/*
**	Purpose:
**		Copies a character from one buffer to another and advancing pointers.
**	Arguments:
**		pszCur: Pointer to string to copy character from.
**		pszBuf: Pointer to string to copy character to.
**		pcbBuf: Pointer to useable size of pszBuf.
**	Returns:
**		none.
**	Notes:
**
****************************************************************************/
VOID PUBLIC CopyCharToBuf ( PSZ pszCur, PSZ pszBuf, PCB pcbBuf )
{
	SZ szEnd = SzNextChar(*pszCur);

	if ((CB)(szEnd - *pszCur) <= *pcbBuf)
		{
		while (*pszCur < szEnd)
			{
			*(*pszBuf)++ = *(*pszCur)++;
			(*pcbBuf)--;
			}
		}
	else
		{
		*pszCur = szEnd;
		*pcbBuf = 0;
		}
}


/*
************************************************************************/
PLOR PUBLIC PlorFromSz ( OR orCur, SZ sz )
{
	PLOR plor = plorNull;
	SZ   szCur;
	UINT cor;

	Assert(sz != szNull);

LWalkStringReadingLOR:
	szCur = sz;
	cor = 0;
	while (*szCur != chEos)
		{
		SZ szSep;

		while (FWhiteSpaceCh(*szCur))
			szCur = SzNextChar(szCur);
		if (*szCur == chEos)
			break;
		szSep = szCur;
		while (*szSep != chEos
			&& !FWhiteSpaceCh(*szSep)
			&& *szSep != chLorSep)
			{
			if (!isdigit(*szSep))
				{
				ParseError(orCur, STR_OBJID_ERR);
				FFreePlor(&plor);
				return (plorNull);
				}
			szSep = SzNextChar(szSep);
			}

		if (plor != plorNull)
			{
			CHAR chSav;

			Assert(plor->rgor != rgorNull);
			chSav = *szSep;
			*szSep = chEos;
			plor->rgor[cor] = atoi(szCur);
			*szSep = chSav;
			}

		cor++;
		szCur = szSep;
		if (*szCur != chEos)
			szCur = SzNextChar(szCur);
		}

	if (plor == plorNull
		&& cor > 0
		&& (plor = PlorAlloc(cor)) != plorNull)
		{
		Assert(plor->rgor != rgorNull);
		Assert(plor->cor == cor);
		goto LWalkStringReadingLOR;
		}

	return (plor);
}


/*
************************************************************************/
BOOL PUBLIC FFreePlor ( PPLOR pplor )
{
	PLOR plor = *pplor;

	if (plor != plorNull)
		{
		Assert(plor->cor > 0);
		Assert(plor->rgor != rgorNull);

		FFree((PB)(plor->rgor), (CB)(plor->cor * sizeof(OR)));
		FFree((PB)plor, (CB)sizeof(LOR));
		*pplor = plorNull;
		}

	return (fTrue);
}


/*
************************************************************************/
PLOR PUBLIC PlorAlloc ( UINT cor )
{
	PLOR plor;

	Assert(cor > 0);

	while ((plor = (PLOR)PbAlloc((CB)sizeof(LOR))) == plorNull)
		{
		if (!FHandleOOM())
			return (plorNull);
		}

	plor->cor = cor;
	while ((plor->rgor = (RGOR)PbAlloc((CB)(cor * sizeof(OR)))) == rgorNull)
		{
		if (!FHandleOOM())
			{
			FFree((PB)plor, (CB)sizeof(LOR));
			return (plorNull);
			}
		}

	return (plor);
}



