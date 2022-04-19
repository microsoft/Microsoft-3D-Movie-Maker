/*
 * Copyright (c) 1992,1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: wtcifix.h 1.6 1995/02/22 21:37:27 sam Exp $
 * $Locker:  $
 *
 * Inline fixed point ops for Watcom C
 *
 */
long IFixedMul(long a, long b);
#pragma aux IFixedMul = \
	"imul	ebx" \
	"shrd	eax,edx,16" \
	parm nomemory [eax] [ebx] \
	modify nomemory [edx] ;

long IFixedDiv(long a, long b);
#pragma aux IFixedDiv = \
	"mov	eax,edx" \
	"shl	eax,16" \
	"sar	edx,16" \
	"idiv	ebx" \
	parm nomemory [edx] [ebx] \
	modify nomemory [eax] ;

long IFixedDivR(long a, long b);
#pragma aux IFixedDivR = \
	"cdq" \
	"xchg	edx,eax" \
	"shrd	eax,edx,16" \
	"sar	edx,16" \
	"idiv	ebx" \
	parm nomemory [eax] [ebx] \
	modify nomemory [edx] ;

long IFixedDivF(long a, long b);
#pragma aux IFixedDivF = \
	"xor	eax,eax" \
	"sar	edx,1" \
	"rcr	eax,1" \
	"div	ebx" \
	parm nomemory [edx] [ebx] \
	modify nomemory [eax] ;

long IFixedRcp(long a);
#pragma aux IFixedRcp = \
	"mov	edx,1" \
	"xor	eax,eax" \
	"idiv	ebx" \
	parm nomemory [ebx] \
	modify nomemory [eax edx] ;

long IFixedMulDiv(long a, long b, long c);
#pragma aux IFixedMulDiv = \
	"imul	ebx" \
	"idiv	ecx" \
	parm nomemory [eax] [ebx] [ecx]\
	modify nomemory [edx] ;

long IFixedMac2(long a, long b, long c, long d);
#pragma aux IFixedMac2 =\
	"imul	edx"\
	"shrd	eax,edx,16"\
	"xchg	ecx,eax"\
	"imul	ebx"\
	"shrd	eax,edx,16"\
	"add	eax,ecx"\
	parm nomemory [eax] [edx] [ecx] [ebx] \
	modify nomemory;

long IFixedMac3(long a, long b, long c, long d, long e, long f);
#pragma aux IFixedMac3 =\
	"imul	edx"\
	"shrd	eax,edx,16"\
	"xchg	ecx,eax"\
	"imul	ebx"\
	"shrd	eax,edx,16"\
	"add	ecx,eax"\
	"mov	eax,esi"\
	"imul	edi"\
	"shrd	eax,edx,16"\
	"add	eax,ecx"\
	parm nomemory [eax] [edx] [ecx] [ebx] [esi] [edi]\
	modify nomemory;

long IFixedFMac2(short a, long b, short c, long d);
#pragma aux IFixedFMac2 =\
	"cwde"\
	"imul	edx"\
	"shrd	eax,edx,15"\
	"xchg	ecx,eax"\
	"cwde"\
	"imul	ebx"\
	"shrd	eax,edx,15"\
	"add	eax,ecx"\
	parm nomemory [ax] [edx] [cx] [ebx] \
	modify nomemory;

long IFixedFMac3(short a, long b, short c, long d, short e, long f);
#pragma aux IFixedFMac3 =\
	"cwde"\
	"imul	edx"\
	"shrd	eax,edx,15"\
	"xchg	ecx,eax"\
	"cwde"\
	"imul	ebx"\
	"shrd	eax,edx,15"\
	"add	ecx,eax"\
	"mov	eax,esi"\
	"cwde"\
	"imul	edi"\
	"shrd	eax,edx,15"\
	"add	eax,ecx"\
	parm nomemory [ax] [edx] [cx] [ebx] [si] [edi]\
	modify nomemory;

long IFixedSqr(long a);
#pragma aux IFixedSqr = \
	"imul	eax" \
	"shrd	eax,edx,16" \
	parm nomemory [eax] \
	modify nomemory [edx] ;

long IFixedSqr2(long a, long b);
#pragma aux IFixedSqr2 =\
	"imul	eax"\
	"xchg	ebx,eax"\
	"mov	ecx,edx"\
	"imul	eax"\
	"add	eax,ebx"\
	"adc	edx,ecx"\
	"shrd	eax,edx,16"\
	parm nomemory [eax] [ebx]\
	modify nomemory [eax ebx ecx edx];

long IFixedSqr3(long a, long b, long c);
#pragma aux IFixedSqr3 =\
	"imul	eax"\
	"xchg	ebx,eax"\
	"mov	ecx,edx"\
	"imul	eax"\
	"add	ebx,eax"\
	"adc	ecx,edx"\
	"mov	eax,esi"\
	"imul	eax"\
	"add	eax,ebx"\
	"adc	edx,ecx"\
	"shrd	eax,edx,16"\
	parm nomemory [eax] [ebx] [esi]\
	modify nomemory [eax ebx ecx edx];

long IFixedSqr4(long a, long b, long c, long d);
#pragma aux IFixedSqr4 =\
	"imul	eax"\
	"xchg	ebx,eax"\
	"mov	ecx,edx"\
	"imul	eax"\
	"add	ebx,eax"\
	"adc	ecx,edx"\
	"mov	eax,esi"\
	"imul	eax"\
	"add	ebx,eax"\
	"adc	ecx,edx"\
	"mov	eax,edi"\
	"imul	eax"\
	"add	eax,ebx"\
	"adc	edx,ecx"\
	"shrd	eax,edx,16"\
	parm nomemory [eax] [ebx] [esi] [edi]\
	modify nomemory [eax ebx ecx edx];

#define BrFixedMul(a,b)					IFixedMul(a,b)
#define BrFixedMac2(a,b,c,d)	 		IFixedMac2(a,b,c,d)
#define BrFixedMac3(a,b,c,d,e,f)		IFixedMac3(a,b,c,d,e,f)
#define BrFixedMac4(a,b,c,d,e,f,g,h)	(IFixedMac2(a,b,c,d)+IFixedMac2(e,f,g,h))
#define BrFixedDiv(a,b)					IFixedDiv(a,b)
#define BrFixedDivR(a,b)				IFixedDivR(a,b)
#define BrFixedDivF(a,b)				IFixedDivF(a,b)
#define BrFixedMulDiv(a,b,c)			IFixedMulDiv(a,b,c)
#define BrFixedRcp(a)					IFixedRcp(a)

#define BrFixedSqr(a)					IFixedSqr(a)
#define BrFixedSqr2(a,b)				IFixedSqr2(a,b)
#define BrFixedSqr3(a,b,c)				IFixedSqr3(a,b,c)
#define BrFixedSqr4(a,b,c,d)			IFixedSqr4(a,b,c,d)

#define BrFixedFMac2(a,b,c,d)	 		IFixedFMac2(a,b,c,d)
#define BrFixedFMac3(a,b,c,d,e,f)		IFixedFMac3(a,b,c,d,e,f)
#define BrFixedFMac4(a,b,c,d,e,f,g,h)	(IFixedFMac2(a,b,c,d)+IFixedFMac2(e,f,g,h))

