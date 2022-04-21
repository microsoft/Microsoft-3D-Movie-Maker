/***************************************************************************
	Author: ******
	Project: Kauai
	Reviewed:
	Copyright (c) Microsoft Corporation

	Basic lexer class

***************************************************************************/
#include <stdio.h>
#include "util.h"
ASSERTNAME


RTCLASS(LEXB)

// #line handling
achar _szPoundLine[] = PszLit("#line");
#define kcchPoundLine (CvFromRgv(_szPoundLine) - 1)

ushort LEXB::_mpchgrfct[128] =
	{
	// 0x00 - 0x07
	fctNil, fctNil, fctNil, fctNil, fctNil, fctNil, fctNil, fctNil,
	// 0x08; 0x09=tab; 0x0A=line-feed; 0x0B; 0x0C; 0x0D=return; 0x0E; 0x0F
	fctNil, fctSpc, fctSpc, fctNil, fctNil, fctSpc, fctNil, fctNil,

	// 0x10 - 0x17
	fctNil, fctNil, fctNil, fctNil, fctNil, fctNil, fctNil, fctNil,
	// 0x18; 0x19; 0x1A=Ctrl-Z; 0x1B - 0x1F
	fctNil, fctNil, fctSpc, fctNil, fctNil, fctNil, fctNil, fctNil,

	// space ! " #
	fctSpc, fctOpr|fctOp1, fctQuo, fctOpr,
	// $ % & '
		fctOpr, fctOpr|fctOp1, fctOpr|fctOp1|fctOp2, fctQuo,
	// ( ) * +
	fctOpr, fctOpr, fctOpr|fctOp1, fctOpr|fctOp1|fctOp2,
	// , - . /
		fctOpr, fctOpr|fctOp1|fctOp2, fctOpr, fctOpr|fctOp1,

	// 0 1 2 3
	kgrfctDigit, kgrfctDigit, kgrfctDigit, kgrfctDigit,
	// 4 5 6 7
		kgrfctDigit, kgrfctDigit, kgrfctDigit, kgrfctDigit,
	// 8 9 : ;
	fctDec|fctHex, fctDec|fctHex, fctOpr|fctOp1|fctOp2, fctOpr,
	// < = > ?
		fctOpr|fctOp1|fctOp2, fctOpr|fctOp1|fctOp2, fctOpr|fctOp1|fctOp2, fctOpr,

	// @ A B C
	fctOpr, fctUpp|fctHex, fctUpp|fctHex, fctUpp|fctHex,
	// D E F G
		fctUpp|fctHex, fctUpp|fctHex, fctUpp|fctHex, fctUpp,
	// H I J K L M N O
	fctUpp, fctUpp, fctUpp, fctUpp, fctUpp, fctUpp, fctUpp, fctUpp,

	// P Q R S T U V W
	fctUpp, fctUpp, fctUpp, fctUpp, fctUpp, fctUpp, fctUpp, fctUpp,
	// X Y Z [ \ ] ^ _
	fctUpp, fctUpp, fctUpp, fctOpr, fctOpr, fctOpr, fctOpr|fctOp1|fctOp2, fctUpp|fctLow,

	// ` a b c
	fctOpr, fctLow|fctHex, fctLow|fctHex, fctLow|fctHex,
	// d e f g
		fctLow|fctHex, fctLow|fctHex, fctLow|fctHex, fctLow,
	// h i j k l m n o
	fctLow, fctLow, fctLow, fctLow, fctLow, fctLow, fctLow, fctLow,

	// p q r s t u v w
	fctLow, fctLow, fctLow, fctLow, fctLow, fctLow, fctLow, fctLow,
	// x y z {
	fctLow, fctLow, fctLow, fctOpr,
	// | } ~ 0x7F=del
		fctOpr|fctOp1|fctOp2, fctOpr, fctOpr|fctOp1, fctNil,
	};


// token values for single characters
#define kchMinTok ChLit('!')
short _rgtt[] =
	{
	// ! " # $ % & '
	ttLNot, ttNil, ttPound, ttDollar, ttMod, ttBAnd, ttNil,
	// ( ) * + , - . /
	ttOpenParen, ttCloseParen, ttMul, ttAdd, ttComma, ttSub, ttDot, ttDiv,
	// 0-7
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// 8 9 : ; < = > ?
	ttNil, ttNil, ttColon, ttSemi, ttLt, ttAssign, ttGt, ttQuery,
	// @ A-G
	ttAt, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// H-O
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// P-W
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// X Y Z [ \ ] ^ _
	ttNil, ttNil, ttNil, ttOpenRef, ttBackSlash, ttCloseRef, ttBXor, ttNil,
	// ` a-g
	ttAccent, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// h-o
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// p-w
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// x y a { | } ~
	ttNil, ttNil, ttNil, ttOpenBrace, ttBOr, ttCloseBrace, ttBNot
	};
long _TtFromCh(achar ch);


/***************************************************************************
	Return the token type of a single character operator.
***************************************************************************/
long _TtFromCh(achar ch)
{
	AssertIn(ch, kchMinTok, kchMinTok + size(_rgtt) / size(_rgtt[0]));
	return _rgtt[(byte)ch - kchMinTok];
}


#define kchMinDouble ChLit('&')
#define kchLastDouble ChLit('|')
short _rgttDouble[] =
	{
	// & '
	ttLAnd, ttNil,
	// ( ) * + , - . /
	ttNil, ttNil, ttNil, ttInc, ttNil, ttDec, ttNil, ttNil,
	// 0-7
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// 8 9 : ; < = > ?
	ttNil, ttNil, ttScope, ttNil, ttShl, ttEq, ttShr, ttNil,
	// @ A-G
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// H-O
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// P-W
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// X Y Z [ \ ] ^ _
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttLXor, ttNil,
	// ` a-g
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// h-o
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// p-w
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// x y a { |
	ttNil, ttNil, ttNil, ttNil, ttLOr
	};

#define kchMinEqual ChLit('!')
#define kchLastEqual ChLit('|')
short _rgttEqual[] =
	{
	// ! " # $ % & '
	ttNe, ttNil, ttNil, ttNil, ttAMod, ttABAnd, ttNil,
	// ( ) * + , - . /
	ttNil, ttNil, ttAMul, ttAAdd, ttNil, ttASub, ttNil, ttADiv,
	// 0-7
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// 8 9 : ; < = > ?
	ttNil, ttNil, ttNil, ttNil, ttLe, ttEq, ttGe, ttNil,
	// @ A-G
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// H-O
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// P-W
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// X Y Z [ \ ] ^ _
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttABXor, ttNil,
	// ` a-g
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// h-o
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// p-w
	ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil, ttNil,
	// x y a { |
	ttNil, ttNil, ttNil, ttNil, ttABOr
	};
long _TtFromChCh(achar ch1, achar ch2);


/***************************************************************************
	Return the token type of a double character token.
***************************************************************************/
long _TtFromChCh(achar ch1, achar ch2)
{
	if (ch1 == ch2)
		{
		return FIn(ch1, kchMinDouble, kchLastDouble + 1) ?
			_rgttDouble[(byte)ch1 - kchMinDouble] : ttNil;
		}

	if (ch2 == ChLit('='))
		{
		return FIn(ch1, kchMinEqual, kchLastEqual + 1) ?
			_rgttEqual[ch1 - kchMinEqual] : ttNil;
		}

	if (ch1 == ChLit('-') && ch2 == ChLit('>'))
		return ttArrow;

	return ttNil;
}


/***************************************************************************
	Constructor for the lexer.
***************************************************************************/
LEXB::LEXB(PFIL pfil, bool fUnionStrings)
{
	AssertPo(pfil, 0);

	_pfil = pfil;
	_pbsf = pvNil;
	_pfil->AddRef();
	_pfil->GetStnPath(&_stnFile);
	_lwLine = 1;
	_ichLine = 0;
	_fpCur = 0;
	_fpMac = pfil->FpMac();
	_ichLim = _ichCur = 0;
	_fLineStart = fTrue;
	_fSkipToNextLine = fFalse;
	_fUnionStrings = fUnionStrings;
	AssertThis(0);
}


/***************************************************************************
	Constructor for the lexer.
***************************************************************************/
LEXB::LEXB(PBSF pbsf, PSTN pstnFile, bool fUnionStrings)
{
	AssertPo(pbsf, 0);
	AssertPo(pstnFile, 0);

	_pfil = pvNil;
	_pbsf = pbsf;
	_pbsf->AddRef();
	_stnFile = *pstnFile;
	_lwLine = 1;
	_ichLine = 0;
	_fpCur = 0;
	_fpMac = pbsf->IbMac();
	_ichLim = _ichCur = 0;
	_fLineStart = fTrue;
	_fSkipToNextLine = fFalse;
	_fUnionStrings = fUnionStrings;
	AssertThis(0);
}


/***************************************************************************
	Destructor for the lexer.
***************************************************************************/
LEXB::~LEXB(void)
{
	ReleasePpo(&_pfil);
	ReleasePpo(&_pbsf);
}


#ifdef DEBUG
/***************************************************************************
	Assert the validity of a LEXB.
***************************************************************************/
void LEXB::AssertValid(ulong grf)
{
	LEXB_PAR::AssertValid(0);
	AssertNilOrPo(_pfil, 0);
	AssertNilOrPo(_pbsf, 0);
	Assert((_pfil == pvNil) != (_pbsf == pvNil),
		"exactly one of _pfil, _pbsf should be non-nil");
	AssertPo(&_stnFile, 0);
	AssertIn(_lwLine, 0, kcbMax);
	AssertIn(_ichLine, 0, kcbMax);
	AssertIn(_fpCur, 0, _fpMac + 1);
	AssertIn(_fpMac, 0, kcbMax);
	AssertIn(_ichCur, 0, _ichLim + 1);
	AssertIn(_ichLim, 0, size(_rgch) + 1);
}


/***************************************************************************
	Mark memory for the LEXB.
***************************************************************************/
void LEXB::MarkMem(void)
{
	AssertValid(0);
	LEXB_PAR::MarkMem();
	MarkMemObj(_pfil);
	MarkMemObj(_pbsf);
}
#endif //DEBUG


/***************************************************************************
	Get the current file that we're reading tokens from.
***************************************************************************/
void LEXB::GetStnFile(PSTN pstn)
{
	AssertThis(0);
	AssertPo(pstn, 0);
	*pstn = _stnFile;
}


/***************************************************************************
	Fetch some characters.  Don't advance the pointer into the file.  Can
	fetch at most kcchLexbBuf characters at a time.
***************************************************************************/
bool LEXB::_FFetchRgch(achar *prgch, long cch)
{
	AssertThis(0);
	AssertIn(cch, 1, kcchLexbBuf);
	AssertPvCb(prgch, cch * size(achar));

	if (_ichLim < _ichCur + cch)
		{
		// need to read some more data
		long cchT;

		if (_fpCur + (_ichCur + cch - _ichLim) * size(achar) > _fpMac)
			{
			// hit the eof
			return fFalse;
			}

		// keep any valid characters
		if (_ichCur < _ichLim)
			{
			BltPb(_rgch + _ichCur, _rgch, (_ichLim - _ichCur) * size(achar));
			_ichLim -= _ichCur;
			}
		else
			_ichLim = 0;
		_ichCur = 0;

		// read new stuff
		cchT = LwMin((_fpMac - _fpCur) / size(achar), kcchLexbBuf - _ichLim);
		AssertIn(cchT, cch - _ichLim, kcchLexbBuf + 1);
		if (pvNil != _pfil)
			{
			AssertPo(_pfil, 0);
			if (!_pfil->FReadRgb(_rgch + _ichLim, cchT * size(achar), _fpCur))
				{
				Warn("Error reading file, truncating logical file");
				_fpMac = _fpCur;
				return fFalse;
				}
			}
		else
			{
			AssertPo(_pbsf, 0);
			_pbsf->FetchRgb(_fpCur, cchT * size(achar), _rgch + _ichLim);
			}
		_ichLim += cchT;
		_fpCur += cchT * size(achar);
		AssertIn(_ichLim, _ichCur + cch, kcchLexbBuf + 1);
		}

	// get the text
	CopyPb(_rgch + _ichCur, prgch, cch * size(achar));
	AssertThis(0);
	return fTrue;
}


/***************************************************************************
	Skip any white space at the current location in the buffer.  This
	handles #line directives and comments.  Comments are not allowed on
	the same line as a #line directive.
***************************************************************************/
bool LEXB::_FSkipWhiteSpace(void)
{
	AssertThis(0);
	achar ch;
	bool fStar, fSkipComment, fSlash;
	long lwLineSav;
	achar rgch[kcchPoundLine + 1];
	STN stn;

	fSkipComment = fFalse;
	while (_FFetchRgch(&ch))
		{
		if ((_GrfctCh(ch) & fctSpc) || _fSkipToNextLine || fSkipComment)
			{
			_Advance();
			if (kchReturn == ch)
				{
				_lwLine++;
				_ichLine = 0;
				_fLineStart = fTrue;
				_fSkipToNextLine = fFalse;
				}
			else if (ChLit('\xA') == ch && 1 == _ichLine)
				_ichLine = 0;

			if (fSkipComment)
				{
				if (fStar && ch == ChLit('/'))
					fSkipComment = fFalse;
				fStar = (ch == ChLit('*'));
				}
			continue;
			}

		//not a white space character

		//check for a comment
		if (ChLit('/') == ch && _FFetchRgch(rgch, 2))
			{
			switch (rgch[1])
				{
			case ChLit('/'):
				//line comment - skip characters until we hit a return
				_Advance(2);
				_fSkipToNextLine = fTrue;
				continue;

			case ChLit('*'):
				//normal comment
				_Advance(2);
				fSkipComment = fTrue;
				fStar = fFalse;
				continue;
				}
			}

		//if this is at the beginning of a line, check for a #line directive
		if (!_fLineStart || (ch != _szPoundLine[0]) ||
				!_FFetchRgch(rgch, kcchPoundLine + 1) ||
				!FEqualRgb(rgch, _szPoundLine, kcchPoundLine) ||
				!(_GrfctCh(rgch[kcchPoundLine]) & fctSpc))
			{
			_fLineStart = fFalse;
			break;
			}

		//a #line directive - skip it and white space
		_Advance(kcchPoundLine);
		while (_FFetchRgch(&ch) && (_GrfctCh(ch) & fctSpc) && ch != kchReturn)
			_Advance();

		//read the line number
		lwLineSav = _lwLine;
		if (!_FFetchRgch(&ch) || !(_GrfctCh(ch) & fctDec))
			goto LBadDirective;
		_Advance();
		_ReadNumber(&_lwLine, ch, 10, klwMax);
		_lwLine--;

		//skip white space (and make sure there is some)
		if (!_FFetchRgch(&ch))
			break; //eof
		if (!(_GrfctCh(ch) & fctSpc))
			goto LBadDirective;
		while (_FFetchRgch(&ch) && (_GrfctCh(ch) & fctSpc) && ch != kchReturn)
			_Advance();
		if (!_FFetchRgch(&ch))
			break; //eof
		if (ch == kchReturn)
			continue; //end of #line

		// read file name
		if (ch != ChLit('"'))
			goto LBadDirective;
		_Advance();
		stn.SetNil();
		for (fSlash = fFalse; ; )
			{
			if (!_FFetchRgch(&ch) || ch == kchReturn)
				goto LBadDirective;
			_Advance();
			if (ch == ChLit('"'))
				break;
			if (ch == ChLit('\\'))
				{
				// if this is the second of a pair of slashes, skip it
				fSlash = !fSlash;
				if (!fSlash)
					continue;
				}
			else
				fSlash = fFalse;
			stn.FAppendCh(ch);
			}

		//skip white space to end of line
		if (!_FFetchRgch(&ch))
			goto LSetFileName; //eof
		if (!(_GrfctCh(ch) & fctSpc))
			goto LBadDirective;
		while (_FFetchRgch(&ch) && (_GrfctCh(ch) & fctSpc) && ch != kchReturn)
			_Advance();
		if (!_FFetchRgch(&ch))
			goto LSetFileName; //eof
		if (ch != kchReturn)
			{
LBadDirective:
			//Bad #line directive - restore the line number
			_lwLine = lwLineSav;
			return fFalse;
			}
		else
			{
LSetFileName:
			_stnFile = stn;
			}
		}

	//if fSkipComment is true, we hit the eof in a comment
	return !fSkipComment;
}


/***************************************************************************
	Get the next token from the file.
***************************************************************************/
bool LEXB::FGetTok(PTOK ptok)
{
	AssertThis(0);
	AssertVarMem(ptok);
	achar ch, ch2;
	ulong grfct;
	long cch;

	ptok->stn.SetNil();
	if (!_FSkipWhiteSpace())
		{
		_fSkipToNextLine = fTrue;
		goto LError;
		}

	if (!_FFetchRgch(&ch))
		{
		ptok->tt = ttNil;
		return fFalse;
		}
	_Advance();

	grfct = _GrfctCh(ch);
	if (grfct & fctDec)
		{
		//numeric value
		ptok->tt = ttLong;
		if (ch == ChLit('0'))
			{
			//hex or octal
			if (!_FFetchRgch(&ch))
				{
				ptok->lw = 0;
				return fTrue;
				}

			if (ch == ChLit('x') || ch == ChLit('X'))
				{
				//hex
				_Advance();
				if (!_FReadHex(&ptok->lw))
					goto LError;
				}
			else
				{
				//octal
				_ReadNumTok(ptok, ChLit('0'), 8, klwMax);
				}
			}
		else
			{
			//decimal
			_ReadNumTok(ptok, ch, 10, klwMax);
			}

		//check for bad termination
		if (_FFetchRgch(&ch) &&
				(_GrfctCh(ch) & (fctDec | fctUpp | fctLow | fctQuo)))
			{
			goto LError;
			}
		return fTrue;
		}

	if (grfct & fctQuo)
		{
		//single or double quote
		if (ch == ChLit('"'))
			{
			//string
			ptok->tt = ttString;
			for (;;)
				{
				if (!_FFetchRgch(&ch))
					goto LError;
				_Advance();
				switch (ch)
					{
				case kchReturn:
					goto LError;
				case ChLit('"'):
					//check for another string immediately following this one
					if (!_fUnionStrings)
						return fTrue;
					if (!_FSkipWhiteSpace())
						{
						_fSkipToNextLine = fTrue;
						goto LError;
						}
					if (!_FFetchRgch(&ch) || ch != ChLit('"'))
						return fTrue;
					_Advance();
					break;
				case ChLit('\\'):
					//control sequence
					if (!_FReadControlCh(&ch))
						{
						_fSkipToNextLine = fTrue;
						goto LError;
						}
					if (chNil != ch)
						ptok->stn.FAppendCh(ch);
					break;
				default:
					ptok->stn.FAppendCh(ch);
					break;
					}
				}
			Assert(fFalse, "how'd we get here?");
			}

		Assert(ch == ChLit('\''), "bad grfct");
		ptok->tt = ttLong;
		ptok->lw = 0;
		//ctg type long
		for (cch = 0; cch < 5; )
			{
			if (!_FFetchRgch(&ch))
				goto LError;
			_Advance();
			switch (ch)
				{
			case kchReturn:
				goto LError;
			case ChLit('\''):
				return fTrue;
			case ChLit('\\'):
				if (!_FReadControlCh(&ch))
					{
					_fSkipToNextLine = fTrue;
					goto LError;
					}
				break;
				}
			ptok->lw = (ptok->lw << 8) + (byte)ch;
			cch++;
			}
		//constant too long
		goto LError;
		}

	if (grfct & fctOp1)
		{
		//check for multi character token
		if (_FFetchRgch(&ch2) && (_GrfctCh(ch2) & fctOp2) &&
				ttNil != (ptok->tt = _TtFromChCh(ch, ch2)))
			{
			_Advance();

			//special case <<= and >>=
			if ((ptok->tt == ttShr || ptok->tt == ttShl) &&
					_FFetchRgch(&ch2) && ch2 == ChLit('='))
				{
				ptok->tt = (ptok->tt == ttShr) ? ttAShr : ttAShl;
				_Advance();
				}

			return fTrue;
			}
		}

	if (grfct & fctOpr)
		{
		/* single character token */
		ptok->tt = _TtFromCh(ch);
		Assert(ttNil != ptok->tt, "bad table entry");
		return fTrue;
		}

	if (grfct & (fctLow | fctUpp))
		{
		//identifier
		ptok->tt = ttName;
		ptok->stn.FAppendCh(ch);
		while (_FFetchRgch(&ch) &&
				(_GrfctCh(ch) & (fctUpp | fctLow | fctDec)))
			{
			ptok->stn.FAppendCh(ch);
			_Advance();
			}
		return fTrue;
		}

LError:
	ptok->tt = ttError;
	ptok->stn.SetNil();

	return fTrue;
}


/***************************************************************************
	Return the size of extra data associated with the last token returned.
***************************************************************************/
long LEXB::CbExtra(void)
{
	AssertThis(0);
	return 0;
}


/***************************************************************************
	Get the extra data for the last token returned.
***************************************************************************/
void LEXB::GetExtra(void *pv)
{
	AssertThis(0);
	Bug("no extra data");
}


/***************************************************************************
	Read a number.  The first character is passed in ch.  lwBase is the base
	of the number (must be <= 10).
***************************************************************************/
void LEXB::_ReadNumber(long *plw, achar ch, long lwBase, long cchMax)
{
	AssertThis(0);
	AssertVarMem(plw);
	AssertIn(ch - ChLit('0'), 0, lwBase);
	AssertIn(lwBase, 2, 11);

	*plw = ch - ChLit('0');
	while (--cchMax > 0 && _FFetchRgch(&ch) &&
			(_GrfctCh(ch) & fctDec) && (ch - ChLit('0') < lwBase))
		{
		*plw = *plw * lwBase + (ch - ChLit('0'));
		_Advance();
		}
}


/***************************************************************************
	Read in a hexadecimal value (without the 0x).
***************************************************************************/
bool LEXB::_FReadHex(long *plw)
{
	AssertThis(0);
	AssertVarMem(plw);
	achar ch;
	ulong grfct;

	*plw = 0;
	if (!_FFetchRgch(&ch) || !((grfct = _GrfctCh(ch)) & fctHex))
		return fFalse;

	do
		{
		if (grfct & fctDec)
			*plw = *plw * 16 + (ch - ChLit('0'));
		else if (grfct & fctLow)
			*plw = *plw * 16 + (10 + ch - ChLit('a'));
		else
			{
			Assert(grfct & fctUpp, "bad grfct");
			*plw = *plw * 16 + (10 + ch - ChLit('A'));
			}
		_Advance();
		}
	while (_FFetchRgch(&ch) && ((grfct = _GrfctCh(ch)) & fctHex));

	return fTrue;
}


/***************************************************************************
	Read a control character (eg, \x3F).  This code assumes the \ has
	already been read.
***************************************************************************/
bool LEXB::_FReadControlCh(achar *pch)
{
	AssertThis(0);
	AssertVarMem(pch);
	//control sequence
	achar ch;
	long lw;

	if (!_FFetchRgch(&ch))
		return fFalse;
	_Advance();

	switch (ch)
		{
	case kchReturn:
		while (_FFetchRgch(&ch) && ch == ChLit('\xA'))
			_Advance();
		*pch = chNil;
		break;
	case ChLit('t'):
		*pch = kchTab;
		break;
	case ChLit('n'):
		*pch = kchReturn;
		break;
	case ChLit('x'):
	case ChLit('X'):
		if (!_FReadHex(&lw))
			return fFalse;
		*pch = (achar)lw;
		break;
	default:
		if (_GrfctCh(ch) & fctOct)
			{
			_ReadNumber(&lw, ch, 8, 3);
			*pch = (achar)lw;
			}
		else
			*pch = ch;
		break;
		}
	return fTrue;
}


