/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

	/***************************************************************************
	This c file is compiled to a command-line exe which when run produces
	the 80386 asm version of the KCDC decompression code.

	Register usage:
		eax: source bytes
		ebx: source address
		edi: destination address
		edx: working space
		ecx: for rep movsb
		esi: for rep movsb
***************************************************************************/

#include <stdio.h>
#include "codkpri.h"

#define kcbitMinMoveByDword 3

typedef long bool;
const bool fFalse = 0;
const bool fTrue = 1;

void Setup(void);
void End(void);
void Advance(long cb);
void Test(long ibit);
void Block(long ibit);
void Literal(long ibit);
void Offset(long ibit, long cbit, long dibBase, long cbBase);
void GetLen(long ibit, bool fTestForDword);
void CallCopy(long ibit, long cbit, bool fTestForDword);
void Copy(long ibit, bool fDword);


int main(void)
{
	long ibit;

	Setup();

	for (ibit = 0; ibit < 8; ibit++)
		{
		if (kcbitMinMoveByDword <= kcbitMaxLenKcdc)
			{
			Copy(ibit, fTrue);
			printf("	jmp LBlock%d\n", ibit);
			}
		Copy(ibit, fFalse);
		Block(ibit);
		}

	End();
	return 0;
}


void Setup(void)
{
	printf(
		"	// Setup\n"
		"	long cbTot;\n"
		"	byte *pbLimDst = (byte *)pvDst + cbDst;\n\n"
		"__asm\n"
		"	{\n"
		"	mov edi,pvDst\n"
		"	mov ebx,pvSrc\n"
		"	inc ebx\n"
		);

	Advance(4);

	printf("	jmp LBlock0\n");
}


void Copy(long ibit, bool fDword)
{
	if (fDword)
		printf("\n	// Copy Dword %d\nLCopyDword%d:\n", ibit, ibit);
	else
		printf("\n	// Copy Byte %d\nLCopyByte%d:\n", ibit, ibit);

	printf(
		"#ifdef SAFETY\n"
		"	push edx\n"
		"	lea edx,[edi+ecx]\n"
		"	cmp edx,pbLimDst\n"
		"	pop edx\n"
		"	ja LFail\n"
		"#endif //SAFETY\n"
		);

	printf(
		"	neg esi\n"
		"	add esi,edi\n"
		);

	printf(
		"#ifdef SAFETY\n"
		"	cmp esi,pvDst\n"
		"	jb LFail\n"
		"#endif //SAFETY\n"
		);

	if (fDword)
		{
		printf(
			"	mov edx,ecx\n"
			"	shr ecx,2\n"
			"	and edx,3\n"
			"	rep movsd\n"
			"	mov ecx,edx\n"
			"	rep movsb\n"
			);
		}
	else
		printf("	rep movsb\n");
}


void End(void)
{
	printf(
		"\nLDone:\n"
		"	sub edi,pvDst\n"
		"	mov cbTot,edi\n"
		"	}\n\n"
		);
}


void Advance(long cb)
{
	switch (cb)
		{
	case 0:
		break;
	case 1:
		printf(
			"	mov eax,[ebx-3]\n"
			"	inc ebx\n"
			);
		break;
	case 2:
		printf(
			"	mov eax,[ebx-2]\n"
			"	add ebx,2\n"
			);
		break;
	case 3:
		printf(
			"	mov eax,[ebx-1]\n"
			"	add ebx,3\n"
			);
		break;
	case 4:
		printf(
			"	mov eax,[ebx]\n"
			"	add ebx,4\n"
			);
		break;
	default:
		printf("*** BUG\n");
		break;
		}
}


void Test(long ibit)
{
	if (ibit < 8)
		printf("	test al,%d\n", 1 << ibit);
	else if (ibit < 16)
		printf("	test ah,%d\n", 1 << (ibit - 8));
	else
		printf("	test eax,%d\n", 1 << ibit);
}


void Block(long ibit)
{
	printf("\n	// Block %d\n", ibit);
	printf("LBlock%d:\n", ibit);

	Test(ibit);
	printf("	jz LLiteral%d\n", ibit + 1);
	Test(ibit + 1);
	printf("	jz L%dBit%d\n", kcbitKcdc0, ibit + 2);
	Test(ibit + 2);
	printf("	jz L%dBit%d\n", kcbitKcdc1, ibit + 3);
	Test(ibit + 3);
	printf("	jz L%dBit%d\n", kcbitKcdc2, ibit + 4);
	printf("	jmp L%dBit%d\n", kcbitKcdc3, ibit + 4);

	Literal(ibit + 1);
	Offset(ibit + 2, kcbitKcdc0, kdibMinKcdc0, 2);
	Offset(ibit + 3, kcbitKcdc1, kdibMinKcdc1, 2);
	Offset(ibit + 4, kcbitKcdc2, kdibMinKcdc2, 2);
	Offset(ibit + 4, kcbitKcdc3, kdibMinKcdc3, 3);
}


void Literal(long ibit)
{
	printf("\n	// Literal %d\n", ibit);
	printf("LLiteral%d:\n", ibit);

	printf(
		"#ifdef SAFETY\n"
		"	cmp edi,pbLimDst\n"
		"	jae LFail\n"
		"#endif //SAFETY\n"
		);

	if (ibit == 8)
		{
		printf(
			"	mov [edi],ah\n"
			"	inc edi\n"
			);
		}
	else
		{
		printf(
			"	mov edx,eax\n"
			"	shr edx,%d\n"
			"	mov [edi],dl\n"
			"	inc edi\n"
			, ibit);
		}

	ibit += 8;
	Advance(ibit / 8);
	printf("	jmp LBlock%d\n", ibit & 0x07);
}


void Offset(long ibit, long cbit, long dibBase, long cbBase)
{
	printf("\nL%dBit%d:\n", cbit, ibit);

	printf("	mov esi,eax\n");
	printf("	mov ecx,%d\n", cbBase);
	printf("	shr esi,%d\n", ibit);
	printf("	and esi,%d\n", (1 << cbit) - 1);

	if (kcbitKcdc3 == cbit)
		{
		// Put in the test for being done
		printf("	cmp esi,%d\n", (1 << cbit) - 1);
		printf("	je LDone\n");
		}

	if (1 == dibBase)
		printf("	inc esi\n");
	else
		printf("	add esi,%d\n", dibBase);

	ibit += cbit;
	Advance(ibit / 8);
	ibit &= 0x07;

	GetLen(ibit, dibBase < sizeof(long));
}


void GetLen(long ibit, bool fTestForDword)
{
	static long _cactCall = 0;
	long cbit;

	_cactCall++;
	for (cbit = 0; cbit <= kcbitMaxLenKcdc; cbit++)
		{
		Test(ibit + cbit);
		printf("	jz LLen%d_%d\n", _cactCall, cbit);
		}
	printf("	jmp LFail\n");

	for (cbit = 0; cbit <= kcbitMaxLenKcdc; cbit++)
		{
		printf("LLen%d_%d:\n", _cactCall, cbit);
		if (cbit > 0)
			{
			printf("	mov edx,eax\n");
			printf("	shr edx,%d\n", ibit + cbit + 1);
			printf("	add ecx,%d\n", (1 << cbit) - 1);
			printf("	and edx,%d\n", (1 << cbit) - 1);
			printf("	add ecx,edx\n");
			}
		CallCopy(ibit + cbit + cbit + 1, cbit, fTestForDword);
		}
}


void CallCopy(long ibit, long cbit, bool fTestForDword)
{
	Advance(ibit / 8);
	ibit &= 0x07;

	if (cbit < kcbitMinMoveByDword)
		printf("jmp LCopyByte%d\n", ibit);
	else
		{
		if (fTestForDword)
			{
			printf("	cmp esi,4\n");
			printf("	jb LCopyByte%d\n", ibit);
			}
		printf("	jmp LCopyDword%d\n", ibit);
		}
}


