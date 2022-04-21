/***************************************************************************
	This c file is compiled to a command-line exe which when run produces
	the 80386 asm version of the KCD2 decompression code.

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
void Literal(long ibit, bool fDword, bool fSingle);
void Offset(long ibit, bool fDword, long cbit, long cbitOH, long dibBase,
	long cbBase);
void Copy(long ibit, bool fDword);


int main(void)
{
	long ibit;

	Setup();

	for (ibit = 0; ibit < 8; ibit++)
		{
		if (kcbitMinMoveByDword <= kcbitMaxLenKcd2)
			{
			Literal(ibit, fTrue, fFalse);
			Offset(ibit, fTrue, kcbitKcd2_0, 2, kdibMinKcd2_0, 1);
			Offset(ibit, fTrue, kcbitKcd2_1, 3, kdibMinKcd2_1, 1);
			Offset(ibit, fTrue, kcbitKcd2_2, 4, kdibMinKcd2_2, 1);
			Offset(ibit, fTrue, kcbitKcd2_3, 4, kdibMinKcd2_3, 2);
			Copy(ibit, fTrue);
			printf("	jmp LBlock%d\n", ibit);
			}
		Literal(ibit, fFalse, fTrue);
		Literal(ibit, fFalse, fFalse);
		Offset(ibit, fFalse, kcbitKcd2_0, 2, kdibMinKcd2_0, 1);
		Offset(ibit, fFalse, kcbitKcd2_1, 3, kdibMinKcd2_1, 1);
		Offset(ibit, fFalse, kcbitKcd2_2, 4, kdibMinKcd2_2, 1);
		Offset(ibit, fFalse, kcbitKcd2_3, 4, kdibMinKcd2_3, 2);
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
		"	byte *pbLimDst = (byte *)pvDst + cbDst;\n"
		"	byte *pbLimSrc = (byte *)pvSrc + cbSrc - kcbTailKcd2;\n\n"
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
	long cbit, ibitT;

	printf("\n	// Block %d\n", ibit);
	printf("LBlock%d:\n", ibit);

	for (cbit = 0; cbit <= kcbitMaxLenKcd2; cbit++)
		{
		Test(ibit + cbit);
		printf("	jz LLen%d_%d\n", ibit, cbit);
		}
	printf("	jmp LDone\n");

	for (cbit = 0; cbit <= kcbitMaxLenKcd2; cbit++)
		{
		printf("LLen%d_%d:\n", ibit, cbit);
		if (cbit == 0)
			printf("	mov ecx,1\n");
		else
			{
			printf("	mov ecx,eax\n");
			printf("	shr ecx,%d\n", ibit + cbit + 1);
			printf("	and ecx,%d\n", (1 << cbit) - 1);
			printf("	or ecx,%d\n", (1 << cbit));
			}

		ibitT = ibit + cbit + cbit + 1;
		Advance(ibitT / 8);
		ibitT &= 0x07;
		Test(ibitT);
		if (cbit == 0)
			printf("	jz LLitSingle%d\n", ibitT);
		else if (cbit < kcbitMinMoveByDword)
			printf("	jz LLitByte%d\n", ibitT);
		else
			printf("	jz LLitDword%d\n", ibitT);

		Test(ibitT + 1);
		if (cbit < kcbitMinMoveByDword)
			{
			printf("	jz L%dBitOffByte%d\n", kcbitKcd2_0, ibitT);
			Test(ibitT + 2);
			printf("	jz L%dBitOffByte%d\n", kcbitKcd2_1, ibitT);
			Test(ibitT + 3);
			printf("	jz L%dBitOffByte%d\n", kcbitKcdc2, ibitT);
			printf("	jmp L%dBitOffByte%d\n", kcbitKcdc3, ibitT);
			}
		else
			{
			printf("	jz L%dBitOffDword%d\n", kcbitKcd2_0, ibitT);
			Test(ibitT + 2);
			printf("	jz L%dBitOffDword%d\n", kcbitKcd2_1, ibitT);
			Test(ibitT + 3);
			printf("	jz L%dBitOffDword%d\n", kcbitKcdc2, ibitT);
			printf("	jmp L%dBitOffDword%d\n", kcbitKcdc3, ibitT);
			}
		}
}


void Offset(long ibit, bool fDword, long cbit, long cbitOH, long dibBase,
	long cbBase)
{
	ibit = (ibit - cbit - cbitOH) & 0x07;
	if (fDword)
		printf("\nL%dBitOffDword%d:\n", cbit, ibit);
	else
		printf("\nL%dBitOffByte%d:\n", cbit, ibit);

	printf("	mov esi,eax\n");
	if (cbBase == 1)
		printf("	inc ecx\n");
	else
		printf("	add ecx,%d\n", cbBase);
	printf("	shr esi,%d\n", ibit + cbitOH);
	printf("	and esi,%d\n", (1 << cbit) - 1);
	if (1 == dibBase)
		printf("	inc esi\n");
	else
		printf("	add esi,%d\n", dibBase);

	ibit += cbitOH + cbit;
	Advance(ibit / 8);
	ibit &= 0x07;

	if (fDword)
		{
		if (dibBase < 4 && fDword)
			{
			printf("	cmp esi,4\n");
			printf("	jb LCopyByte%d\n", ibit);
			}
		printf("	jmp LCopyDword%d\n", ibit);
		}
	else
		printf("	jmp LCopyByte%d\n", ibit);
}


void Literal(long ibit, bool fDword, bool fSingle)
{
	ibit = (ibit - 1) & 0x07;
	if (fDword)
		printf("\nLLitDword%d:\n", ibit);
	else if (fSingle)
		printf("\nLLitSingle%d:\n", ibit);
	else
		printf("\nLLitByte%d:\n", ibit);

	ibit++;
	if (fSingle)
		{
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
		return;
		}

	printf(
		"#ifdef SAFETY\n"
		"	lea edx,[edi+ecx]\n"
		"	cmp edx,pbLimDst\n"
		"	ja LFail\n"
		"#endif //SAFETY\n"
		);

	if (ibit != 8)
		{
		// get the low bits of the last byte
		printf("	dec ecx\n");
		printf("	mov edx,eax\n");
		printf("	shr edx,%d\n", ibit);
		printf("	and edx,%d\n", (1 << (8 - ibit)) - 1);
		}

	printf(
		"#ifdef SAFETY\n"
		"	lea esi,[ebx-3+ecx]\n"
		"	cmp esi,pbLimSrc\n"
		"	ja LFail\n"
		"	sub esi,ecx\n"
		"#else //!SAFETY\n"
		"	lea esi,[ebx-3]\n"
		"#endif //!SAFETY\n"
		);

	if (fDword)
		{
		printf(
			"	mov eax,ecx\n"
			"	shr ecx,2\n"
			"	and eax,3\n"
			"	rep movsd\n"
			"	mov ecx,eax\n"
			"	rep movsb\n"
			);
		}
	else
		printf("	rep movsb\n");

	printf(
		"	lea ebx,[esi+4]\n"
		"	mov eax,[esi]\n"
		);

	if (ibit != 8)
		{
		printf("	mov esi,eax\n");
		printf("	shl esi,%d\n", 8 - ibit);
		printf(
			"	or edx,esi\n"
			"	mov [edi],dl\n"
			"	inc edi\n"
			);
		}

	printf("	jmp LBlock%d\n", ibit & 0x07);
}

