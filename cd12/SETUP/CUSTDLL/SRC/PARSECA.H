/****************************************************************************
**
**	File:			PARSECA.H
**	Purpose:		Header file for Custom Action data parsing routines
**					for the Sample App DLL.
**	Notes:
**
****************************************************************************/

#ifndef PARSECA_H
#define PARSECA_H


RC   PUBLIC RcParseInfSectKey ( OR orCur, SZ szData, PSZ pszInfSect,
					PSZ pszInfKey );
RC PUBLIC RcParseLor ( PCD pcd, OR orCur, SZ szData, PPLOR pplor );

BOOL PUBLIC FValidInfSection ( SZ szSection );
BOOL PUBLIC FValidGroupPlor ( PCD pcd, PLOR plor, OR orGroup );
SZ   PUBLIC SzStrDupl ( SZ sz );
BOOL PUBLIC FReadDataFieldString ( PSZ pszData, PSZ pszMember );
BOOL PUBLIC FValidInfKey ( SZ szKey );
VOID PUBLIC ParseError ( OR or, SZ szMsg );
SZ   PUBLIC SzGetTableField ( SZ szLine, SZ szField, CB cbFieldMax,
							CHAR chSep );
VOID PUBLIC CopyCharToBuf ( PSZ pszCur, PSZ pszBuf, PCB pcbBuf );
PLOR PUBLIC PlorFromSz ( OR orCur, SZ sz );
BOOL PUBLIC FFreePlor ( PPLOR pplor );
PLOR PUBLIC PlorAlloc ( UINT cor );


#define FValidOr(pcd, or)	(or != orNil && or < pcd->cObjectsMax)



#ifdef DEBUG
#define DebugMsgBox(cszcText,cszcTitle) DoMsgBox(cszcText, cszcTitle,				\
											   MB_OK | MB_ICONEXCLAMATION)
#else
#define DebugMsgBox(cszcText,cszcTitle)
#endif


#endif  /* PARSECA_H */

