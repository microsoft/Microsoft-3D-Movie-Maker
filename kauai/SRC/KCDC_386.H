/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

	// Setup
	long cbTot;
	byte *pbLimDst = (byte *)pvDst + cbDst;

__asm
	{
	mov edi,pvDst
	mov ebx,pvSrc
	inc ebx
	mov eax,[ebx]
	add ebx,4
	jmp LBlock0

	// Copy Dword 0
LCopyDword0:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	mov edx,ecx
	shr ecx,2
	and edx,3
	rep movsd
	mov ecx,edx
	rep movsb
	jmp LBlock0

	// Copy Byte 0
LCopyByte0:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	rep movsb

	// Block 0
LBlock0:
	test al,1
	jz LLiteral1
	test al,2
	jz L6Bit2
	test al,4
	jz L9Bit3
	test al,8
	jz L12Bit4
	jmp L20Bit4

	// Literal 1
LLiteral1:
#ifdef SAFETY
	cmp edi,pbLimDst
	jae LFail
#endif //SAFETY
	mov edx,eax
	shr edx,1
	mov [edi],dl
	inc edi
	mov eax,[ebx-3]
	inc ebx
	jmp LBlock1

L6Bit2:
	mov esi,eax
	mov ecx,2
	shr esi,2
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	test al,1
	jz LLen1_0
	test al,2
	jz LLen1_1
	test al,4
	jz LLen1_2
	test al,8
	jz LLen1_3
	test al,16
	jz LLen1_4
	test al,32
	jz LLen1_5
	test al,64
	jz LLen1_6
	test al,128
	jz LLen1_7
	test ah,1
	jz LLen1_8
	test ah,2
	jz LLen1_9
	test ah,4
	jz LLen1_10
	test ah,8
	jz LLen1_11
	jmp LFail
LLen1_0:
jmp LCopyByte1
LLen1_1:
	mov edx,eax
	shr edx,2
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte3
LLen1_2:
	mov edx,eax
	shr edx,3
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte5
LLen1_3:
	mov edx,eax
	shr edx,4
	add ecx,7
	and edx,7
	add ecx,edx
	cmp esi,4
	jb LCopyByte7
	jmp LCopyDword7
LLen1_4:
	mov edx,eax
	shr edx,5
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte1
	jmp LCopyDword1
LLen1_5:
	mov edx,eax
	shr edx,6
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte3
	jmp LCopyDword3
LLen1_6:
	mov edx,eax
	shr edx,7
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte5
	jmp LCopyDword5
LLen1_7:
	mov edx,eax
	shr edx,8
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte7
	jmp LCopyDword7
LLen1_8:
	mov edx,eax
	shr edx,9
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte1
	jmp LCopyDword1
LLen1_9:
	mov edx,eax
	shr edx,10
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte3
	jmp LCopyDword3
LLen1_10:
	mov edx,eax
	shr edx,11
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte5
	jmp LCopyDword5
LLen1_11:
	mov edx,eax
	shr edx,12
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte7
	jmp LCopyDword7

L9Bit3:
	mov esi,eax
	mov ecx,2
	shr esi,3
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	test al,16
	jz LLen2_0
	test al,32
	jz LLen2_1
	test al,64
	jz LLen2_2
	test al,128
	jz LLen2_3
	test ah,1
	jz LLen2_4
	test ah,2
	jz LLen2_5
	test ah,4
	jz LLen2_6
	test ah,8
	jz LLen2_7
	test ah,16
	jz LLen2_8
	test ah,32
	jz LLen2_9
	test ah,64
	jz LLen2_10
	test ah,128
	jz LLen2_11
	jmp LFail
LLen2_0:
jmp LCopyByte5
LLen2_1:
	mov edx,eax
	shr edx,6
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte7
LLen2_2:
	mov edx,eax
	shr edx,7
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte1
LLen2_3:
	mov edx,eax
	shr edx,8
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword3
LLen2_4:
	mov edx,eax
	shr edx,9
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen2_5:
	mov edx,eax
	shr edx,10
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen2_6:
	mov edx,eax
	shr edx,11
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen2_7:
	mov edx,eax
	shr edx,12
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen2_8:
	mov edx,eax
	shr edx,13
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen2_9:
	mov edx,eax
	shr edx,14
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7
LLen2_10:
	mov edx,eax
	shr edx,15
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword1
LLen2_11:
	mov edx,eax
	shr edx,16
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword3

L12Bit4:
	mov esi,eax
	mov ecx,2
	shr esi,4
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	test al,1
	jz LLen3_0
	test al,2
	jz LLen3_1
	test al,4
	jz LLen3_2
	test al,8
	jz LLen3_3
	test al,16
	jz LLen3_4
	test al,32
	jz LLen3_5
	test al,64
	jz LLen3_6
	test al,128
	jz LLen3_7
	test ah,1
	jz LLen3_8
	test ah,2
	jz LLen3_9
	test ah,4
	jz LLen3_10
	test ah,8
	jz LLen3_11
	jmp LFail
LLen3_0:
jmp LCopyByte1
LLen3_1:
	mov edx,eax
	shr edx,2
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte3
LLen3_2:
	mov edx,eax
	shr edx,3
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte5
LLen3_3:
	mov edx,eax
	shr edx,4
	add ecx,7
	and edx,7
	add ecx,edx
	jmp LCopyDword7
LLen3_4:
	mov edx,eax
	shr edx,5
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword1
LLen3_5:
	mov edx,eax
	shr edx,6
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword3
LLen3_6:
	mov edx,eax
	shr edx,7
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen3_7:
	mov edx,eax
	shr edx,8
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen3_8:
	mov edx,eax
	shr edx,9
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen3_9:
	mov edx,eax
	shr edx,10
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen3_10:
	mov edx,eax
	shr edx,11
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen3_11:
	mov edx,eax
	shr edx,12
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7

L20Bit4:
	mov esi,eax
	mov ecx,3
	shr esi,4
	and esi,1048575
	cmp esi,1048575
	je LDone
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	test al,1
	jz LLen4_0
	test al,2
	jz LLen4_1
	test al,4
	jz LLen4_2
	test al,8
	jz LLen4_3
	test al,16
	jz LLen4_4
	test al,32
	jz LLen4_5
	test al,64
	jz LLen4_6
	test al,128
	jz LLen4_7
	test ah,1
	jz LLen4_8
	test ah,2
	jz LLen4_9
	test ah,4
	jz LLen4_10
	test ah,8
	jz LLen4_11
	jmp LFail
LLen4_0:
jmp LCopyByte1
LLen4_1:
	mov edx,eax
	shr edx,2
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte3
LLen4_2:
	mov edx,eax
	shr edx,3
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte5
LLen4_3:
	mov edx,eax
	shr edx,4
	add ecx,7
	and edx,7
	add ecx,edx
	jmp LCopyDword7
LLen4_4:
	mov edx,eax
	shr edx,5
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword1
LLen4_5:
	mov edx,eax
	shr edx,6
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword3
LLen4_6:
	mov edx,eax
	shr edx,7
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen4_7:
	mov edx,eax
	shr edx,8
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen4_8:
	mov edx,eax
	shr edx,9
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen4_9:
	mov edx,eax
	shr edx,10
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen4_10:
	mov edx,eax
	shr edx,11
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen4_11:
	mov edx,eax
	shr edx,12
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7

	// Copy Dword 1
LCopyDword1:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	mov edx,ecx
	shr ecx,2
	and edx,3
	rep movsd
	mov ecx,edx
	rep movsb
	jmp LBlock1

	// Copy Byte 1
LCopyByte1:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	rep movsb

	// Block 1
LBlock1:
	test al,2
	jz LLiteral2
	test al,4
	jz L6Bit3
	test al,8
	jz L9Bit4
	test al,16
	jz L12Bit5
	jmp L20Bit5

	// Literal 2
LLiteral2:
#ifdef SAFETY
	cmp edi,pbLimDst
	jae LFail
#endif //SAFETY
	mov edx,eax
	shr edx,2
	mov [edi],dl
	inc edi
	mov eax,[ebx-3]
	inc ebx
	jmp LBlock2

L6Bit3:
	mov esi,eax
	mov ecx,2
	shr esi,3
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	test al,2
	jz LLen5_0
	test al,4
	jz LLen5_1
	test al,8
	jz LLen5_2
	test al,16
	jz LLen5_3
	test al,32
	jz LLen5_4
	test al,64
	jz LLen5_5
	test al,128
	jz LLen5_6
	test ah,1
	jz LLen5_7
	test ah,2
	jz LLen5_8
	test ah,4
	jz LLen5_9
	test ah,8
	jz LLen5_10
	test ah,16
	jz LLen5_11
	jmp LFail
LLen5_0:
jmp LCopyByte2
LLen5_1:
	mov edx,eax
	shr edx,3
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte4
LLen5_2:
	mov edx,eax
	shr edx,4
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte6
LLen5_3:
	mov edx,eax
	shr edx,5
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte0
	jmp LCopyDword0
LLen5_4:
	mov edx,eax
	shr edx,6
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte2
	jmp LCopyDword2
LLen5_5:
	mov edx,eax
	shr edx,7
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte4
	jmp LCopyDword4
LLen5_6:
	mov edx,eax
	shr edx,8
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte6
	jmp LCopyDword6
LLen5_7:
	mov edx,eax
	shr edx,9
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte0
	jmp LCopyDword0
LLen5_8:
	mov edx,eax
	shr edx,10
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte2
	jmp LCopyDword2
LLen5_9:
	mov edx,eax
	shr edx,11
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte4
	jmp LCopyDword4
LLen5_10:
	mov edx,eax
	shr edx,12
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte6
	jmp LCopyDword6
LLen5_11:
	mov edx,eax
	shr edx,13
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte0
	jmp LCopyDword0

L9Bit4:
	mov esi,eax
	mov ecx,2
	shr esi,4
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	test al,32
	jz LLen6_0
	test al,64
	jz LLen6_1
	test al,128
	jz LLen6_2
	test ah,1
	jz LLen6_3
	test ah,2
	jz LLen6_4
	test ah,4
	jz LLen6_5
	test ah,8
	jz LLen6_6
	test ah,16
	jz LLen6_7
	test ah,32
	jz LLen6_8
	test ah,64
	jz LLen6_9
	test ah,128
	jz LLen6_10
	test eax,65536
	jz LLen6_11
	jmp LFail
LLen6_0:
jmp LCopyByte6
LLen6_1:
	mov edx,eax
	shr edx,7
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen6_2:
	mov edx,eax
	shr edx,8
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte2
LLen6_3:
	mov edx,eax
	shr edx,9
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword4
LLen6_4:
	mov edx,eax
	shr edx,10
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen6_5:
	mov edx,eax
	shr edx,11
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen6_6:
	mov edx,eax
	shr edx,12
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen6_7:
	mov edx,eax
	shr edx,13
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen6_8:
	mov edx,eax
	shr edx,14
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen6_9:
	mov edx,eax
	shr edx,15
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0
LLen6_10:
	mov edx,eax
	shr edx,16
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword2
LLen6_11:
	mov edx,eax
	shr edx,17
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword4

L12Bit5:
	mov esi,eax
	mov ecx,2
	shr esi,5
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	test al,2
	jz LLen7_0
	test al,4
	jz LLen7_1
	test al,8
	jz LLen7_2
	test al,16
	jz LLen7_3
	test al,32
	jz LLen7_4
	test al,64
	jz LLen7_5
	test al,128
	jz LLen7_6
	test ah,1
	jz LLen7_7
	test ah,2
	jz LLen7_8
	test ah,4
	jz LLen7_9
	test ah,8
	jz LLen7_10
	test ah,16
	jz LLen7_11
	jmp LFail
LLen7_0:
jmp LCopyByte2
LLen7_1:
	mov edx,eax
	shr edx,3
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte4
LLen7_2:
	mov edx,eax
	shr edx,4
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte6
LLen7_3:
	mov edx,eax
	shr edx,5
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword0
LLen7_4:
	mov edx,eax
	shr edx,6
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword2
LLen7_5:
	mov edx,eax
	shr edx,7
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword4
LLen7_6:
	mov edx,eax
	shr edx,8
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen7_7:
	mov edx,eax
	shr edx,9
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen7_8:
	mov edx,eax
	shr edx,10
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen7_9:
	mov edx,eax
	shr edx,11
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen7_10:
	mov edx,eax
	shr edx,12
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen7_11:
	mov edx,eax
	shr edx,13
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0

L20Bit5:
	mov esi,eax
	mov ecx,3
	shr esi,5
	and esi,1048575
	cmp esi,1048575
	je LDone
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	test al,2
	jz LLen8_0
	test al,4
	jz LLen8_1
	test al,8
	jz LLen8_2
	test al,16
	jz LLen8_3
	test al,32
	jz LLen8_4
	test al,64
	jz LLen8_5
	test al,128
	jz LLen8_6
	test ah,1
	jz LLen8_7
	test ah,2
	jz LLen8_8
	test ah,4
	jz LLen8_9
	test ah,8
	jz LLen8_10
	test ah,16
	jz LLen8_11
	jmp LFail
LLen8_0:
jmp LCopyByte2
LLen8_1:
	mov edx,eax
	shr edx,3
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte4
LLen8_2:
	mov edx,eax
	shr edx,4
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte6
LLen8_3:
	mov edx,eax
	shr edx,5
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword0
LLen8_4:
	mov edx,eax
	shr edx,6
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword2
LLen8_5:
	mov edx,eax
	shr edx,7
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword4
LLen8_6:
	mov edx,eax
	shr edx,8
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen8_7:
	mov edx,eax
	shr edx,9
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen8_8:
	mov edx,eax
	shr edx,10
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen8_9:
	mov edx,eax
	shr edx,11
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen8_10:
	mov edx,eax
	shr edx,12
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen8_11:
	mov edx,eax
	shr edx,13
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0

	// Copy Dword 2
LCopyDword2:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	mov edx,ecx
	shr ecx,2
	and edx,3
	rep movsd
	mov ecx,edx
	rep movsb
	jmp LBlock2

	// Copy Byte 2
LCopyByte2:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	rep movsb

	// Block 2
LBlock2:
	test al,4
	jz LLiteral3
	test al,8
	jz L6Bit4
	test al,16
	jz L9Bit5
	test al,32
	jz L12Bit6
	jmp L20Bit6

	// Literal 3
LLiteral3:
#ifdef SAFETY
	cmp edi,pbLimDst
	jae LFail
#endif //SAFETY
	mov edx,eax
	shr edx,3
	mov [edi],dl
	inc edi
	mov eax,[ebx-3]
	inc ebx
	jmp LBlock3

L6Bit4:
	mov esi,eax
	mov ecx,2
	shr esi,4
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	test al,4
	jz LLen9_0
	test al,8
	jz LLen9_1
	test al,16
	jz LLen9_2
	test al,32
	jz LLen9_3
	test al,64
	jz LLen9_4
	test al,128
	jz LLen9_5
	test ah,1
	jz LLen9_6
	test ah,2
	jz LLen9_7
	test ah,4
	jz LLen9_8
	test ah,8
	jz LLen9_9
	test ah,16
	jz LLen9_10
	test ah,32
	jz LLen9_11
	jmp LFail
LLen9_0:
jmp LCopyByte3
LLen9_1:
	mov edx,eax
	shr edx,4
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte5
LLen9_2:
	mov edx,eax
	shr edx,5
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte7
LLen9_3:
	mov edx,eax
	shr edx,6
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte1
	jmp LCopyDword1
LLen9_4:
	mov edx,eax
	shr edx,7
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte3
	jmp LCopyDword3
LLen9_5:
	mov edx,eax
	shr edx,8
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte5
	jmp LCopyDword5
LLen9_6:
	mov edx,eax
	shr edx,9
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte7
	jmp LCopyDword7
LLen9_7:
	mov edx,eax
	shr edx,10
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte1
	jmp LCopyDword1
LLen9_8:
	mov edx,eax
	shr edx,11
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte3
	jmp LCopyDword3
LLen9_9:
	mov edx,eax
	shr edx,12
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte5
	jmp LCopyDword5
LLen9_10:
	mov edx,eax
	shr edx,13
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte7
	jmp LCopyDword7
LLen9_11:
	mov edx,eax
	shr edx,14
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte1
	jmp LCopyDword1

L9Bit5:
	mov esi,eax
	mov ecx,2
	shr esi,5
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	test al,64
	jz LLen10_0
	test al,128
	jz LLen10_1
	test ah,1
	jz LLen10_2
	test ah,2
	jz LLen10_3
	test ah,4
	jz LLen10_4
	test ah,8
	jz LLen10_5
	test ah,16
	jz LLen10_6
	test ah,32
	jz LLen10_7
	test ah,64
	jz LLen10_8
	test ah,128
	jz LLen10_9
	test eax,65536
	jz LLen10_10
	test eax,131072
	jz LLen10_11
	jmp LFail
LLen10_0:
jmp LCopyByte7
LLen10_1:
	mov edx,eax
	shr edx,8
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte1
LLen10_2:
	mov edx,eax
	shr edx,9
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte3
LLen10_3:
	mov edx,eax
	shr edx,10
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen10_4:
	mov edx,eax
	shr edx,11
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen10_5:
	mov edx,eax
	shr edx,12
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen10_6:
	mov edx,eax
	shr edx,13
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen10_7:
	mov edx,eax
	shr edx,14
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen10_8:
	mov edx,eax
	shr edx,15
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7
LLen10_9:
	mov edx,eax
	shr edx,16
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword1
LLen10_10:
	mov edx,eax
	shr edx,17
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword3
LLen10_11:
	mov edx,eax
	shr edx,18
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword5

L12Bit6:
	mov esi,eax
	mov ecx,2
	shr esi,6
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	test al,4
	jz LLen11_0
	test al,8
	jz LLen11_1
	test al,16
	jz LLen11_2
	test al,32
	jz LLen11_3
	test al,64
	jz LLen11_4
	test al,128
	jz LLen11_5
	test ah,1
	jz LLen11_6
	test ah,2
	jz LLen11_7
	test ah,4
	jz LLen11_8
	test ah,8
	jz LLen11_9
	test ah,16
	jz LLen11_10
	test ah,32
	jz LLen11_11
	jmp LFail
LLen11_0:
jmp LCopyByte3
LLen11_1:
	mov edx,eax
	shr edx,4
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte5
LLen11_2:
	mov edx,eax
	shr edx,5
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte7
LLen11_3:
	mov edx,eax
	shr edx,6
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword1
LLen11_4:
	mov edx,eax
	shr edx,7
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword3
LLen11_5:
	mov edx,eax
	shr edx,8
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen11_6:
	mov edx,eax
	shr edx,9
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen11_7:
	mov edx,eax
	shr edx,10
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen11_8:
	mov edx,eax
	shr edx,11
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen11_9:
	mov edx,eax
	shr edx,12
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen11_10:
	mov edx,eax
	shr edx,13
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7
LLen11_11:
	mov edx,eax
	shr edx,14
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword1

L20Bit6:
	mov esi,eax
	mov ecx,3
	shr esi,6
	and esi,1048575
	cmp esi,1048575
	je LDone
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	test al,4
	jz LLen12_0
	test al,8
	jz LLen12_1
	test al,16
	jz LLen12_2
	test al,32
	jz LLen12_3
	test al,64
	jz LLen12_4
	test al,128
	jz LLen12_5
	test ah,1
	jz LLen12_6
	test ah,2
	jz LLen12_7
	test ah,4
	jz LLen12_8
	test ah,8
	jz LLen12_9
	test ah,16
	jz LLen12_10
	test ah,32
	jz LLen12_11
	jmp LFail
LLen12_0:
jmp LCopyByte3
LLen12_1:
	mov edx,eax
	shr edx,4
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte5
LLen12_2:
	mov edx,eax
	shr edx,5
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte7
LLen12_3:
	mov edx,eax
	shr edx,6
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword1
LLen12_4:
	mov edx,eax
	shr edx,7
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword3
LLen12_5:
	mov edx,eax
	shr edx,8
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen12_6:
	mov edx,eax
	shr edx,9
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen12_7:
	mov edx,eax
	shr edx,10
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen12_8:
	mov edx,eax
	shr edx,11
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen12_9:
	mov edx,eax
	shr edx,12
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen12_10:
	mov edx,eax
	shr edx,13
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7
LLen12_11:
	mov edx,eax
	shr edx,14
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword1

	// Copy Dword 3
LCopyDword3:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	mov edx,ecx
	shr ecx,2
	and edx,3
	rep movsd
	mov ecx,edx
	rep movsb
	jmp LBlock3

	// Copy Byte 3
LCopyByte3:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	rep movsb

	// Block 3
LBlock3:
	test al,8
	jz LLiteral4
	test al,16
	jz L6Bit5
	test al,32
	jz L9Bit6
	test al,64
	jz L12Bit7
	jmp L20Bit7

	// Literal 4
LLiteral4:
#ifdef SAFETY
	cmp edi,pbLimDst
	jae LFail
#endif //SAFETY
	mov edx,eax
	shr edx,4
	mov [edi],dl
	inc edi
	mov eax,[ebx-3]
	inc ebx
	jmp LBlock4

L6Bit5:
	mov esi,eax
	mov ecx,2
	shr esi,5
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	test al,8
	jz LLen13_0
	test al,16
	jz LLen13_1
	test al,32
	jz LLen13_2
	test al,64
	jz LLen13_3
	test al,128
	jz LLen13_4
	test ah,1
	jz LLen13_5
	test ah,2
	jz LLen13_6
	test ah,4
	jz LLen13_7
	test ah,8
	jz LLen13_8
	test ah,16
	jz LLen13_9
	test ah,32
	jz LLen13_10
	test ah,64
	jz LLen13_11
	jmp LFail
LLen13_0:
jmp LCopyByte4
LLen13_1:
	mov edx,eax
	shr edx,5
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte6
LLen13_2:
	mov edx,eax
	shr edx,6
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen13_3:
	mov edx,eax
	shr edx,7
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte2
	jmp LCopyDword2
LLen13_4:
	mov edx,eax
	shr edx,8
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte4
	jmp LCopyDword4
LLen13_5:
	mov edx,eax
	shr edx,9
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte6
	jmp LCopyDword6
LLen13_6:
	mov edx,eax
	shr edx,10
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte0
	jmp LCopyDword0
LLen13_7:
	mov edx,eax
	shr edx,11
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte2
	jmp LCopyDword2
LLen13_8:
	mov edx,eax
	shr edx,12
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte4
	jmp LCopyDword4
LLen13_9:
	mov edx,eax
	shr edx,13
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte6
	jmp LCopyDword6
LLen13_10:
	mov edx,eax
	shr edx,14
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte0
	jmp LCopyDword0
LLen13_11:
	mov edx,eax
	shr edx,15
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte2
	jmp LCopyDword2

L9Bit6:
	mov esi,eax
	mov ecx,2
	shr esi,6
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	test al,128
	jz LLen14_0
	test ah,1
	jz LLen14_1
	test ah,2
	jz LLen14_2
	test ah,4
	jz LLen14_3
	test ah,8
	jz LLen14_4
	test ah,16
	jz LLen14_5
	test ah,32
	jz LLen14_6
	test ah,64
	jz LLen14_7
	test ah,128
	jz LLen14_8
	test eax,65536
	jz LLen14_9
	test eax,131072
	jz LLen14_10
	test eax,262144
	jz LLen14_11
	jmp LFail
LLen14_0:
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen14_1:
	mov edx,eax
	shr edx,9
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte2
LLen14_2:
	mov edx,eax
	shr edx,10
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte4
LLen14_3:
	mov edx,eax
	shr edx,11
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen14_4:
	mov edx,eax
	shr edx,12
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen14_5:
	mov edx,eax
	shr edx,13
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen14_6:
	mov edx,eax
	shr edx,14
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen14_7:
	mov edx,eax
	shr edx,15
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen14_8:
	mov edx,eax
	shr edx,16
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0
LLen14_9:
	mov edx,eax
	shr edx,17
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword2
LLen14_10:
	mov edx,eax
	shr edx,18
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword4
LLen14_11:
	mov edx,eax
	shr edx,19
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword6

L12Bit7:
	mov esi,eax
	mov ecx,2
	shr esi,7
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	test al,8
	jz LLen15_0
	test al,16
	jz LLen15_1
	test al,32
	jz LLen15_2
	test al,64
	jz LLen15_3
	test al,128
	jz LLen15_4
	test ah,1
	jz LLen15_5
	test ah,2
	jz LLen15_6
	test ah,4
	jz LLen15_7
	test ah,8
	jz LLen15_8
	test ah,16
	jz LLen15_9
	test ah,32
	jz LLen15_10
	test ah,64
	jz LLen15_11
	jmp LFail
LLen15_0:
jmp LCopyByte4
LLen15_1:
	mov edx,eax
	shr edx,5
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte6
LLen15_2:
	mov edx,eax
	shr edx,6
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen15_3:
	mov edx,eax
	shr edx,7
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword2
LLen15_4:
	mov edx,eax
	shr edx,8
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword4
LLen15_5:
	mov edx,eax
	shr edx,9
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen15_6:
	mov edx,eax
	shr edx,10
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen15_7:
	mov edx,eax
	shr edx,11
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen15_8:
	mov edx,eax
	shr edx,12
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen15_9:
	mov edx,eax
	shr edx,13
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen15_10:
	mov edx,eax
	shr edx,14
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0
LLen15_11:
	mov edx,eax
	shr edx,15
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword2

L20Bit7:
	mov esi,eax
	mov ecx,3
	shr esi,7
	and esi,1048575
	cmp esi,1048575
	je LDone
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	test al,8
	jz LLen16_0
	test al,16
	jz LLen16_1
	test al,32
	jz LLen16_2
	test al,64
	jz LLen16_3
	test al,128
	jz LLen16_4
	test ah,1
	jz LLen16_5
	test ah,2
	jz LLen16_6
	test ah,4
	jz LLen16_7
	test ah,8
	jz LLen16_8
	test ah,16
	jz LLen16_9
	test ah,32
	jz LLen16_10
	test ah,64
	jz LLen16_11
	jmp LFail
LLen16_0:
jmp LCopyByte4
LLen16_1:
	mov edx,eax
	shr edx,5
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte6
LLen16_2:
	mov edx,eax
	shr edx,6
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen16_3:
	mov edx,eax
	shr edx,7
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword2
LLen16_4:
	mov edx,eax
	shr edx,8
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword4
LLen16_5:
	mov edx,eax
	shr edx,9
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen16_6:
	mov edx,eax
	shr edx,10
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen16_7:
	mov edx,eax
	shr edx,11
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen16_8:
	mov edx,eax
	shr edx,12
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen16_9:
	mov edx,eax
	shr edx,13
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen16_10:
	mov edx,eax
	shr edx,14
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0
LLen16_11:
	mov edx,eax
	shr edx,15
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword2

	// Copy Dword 4
LCopyDword4:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	mov edx,ecx
	shr ecx,2
	and edx,3
	rep movsd
	mov ecx,edx
	rep movsb
	jmp LBlock4

	// Copy Byte 4
LCopyByte4:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	rep movsb

	// Block 4
LBlock4:
	test al,16
	jz LLiteral5
	test al,32
	jz L6Bit6
	test al,64
	jz L9Bit7
	test al,128
	jz L12Bit8
	jmp L20Bit8

	// Literal 5
LLiteral5:
#ifdef SAFETY
	cmp edi,pbLimDst
	jae LFail
#endif //SAFETY
	mov edx,eax
	shr edx,5
	mov [edi],dl
	inc edi
	mov eax,[ebx-3]
	inc ebx
	jmp LBlock5

L6Bit6:
	mov esi,eax
	mov ecx,2
	shr esi,6
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	test al,16
	jz LLen17_0
	test al,32
	jz LLen17_1
	test al,64
	jz LLen17_2
	test al,128
	jz LLen17_3
	test ah,1
	jz LLen17_4
	test ah,2
	jz LLen17_5
	test ah,4
	jz LLen17_6
	test ah,8
	jz LLen17_7
	test ah,16
	jz LLen17_8
	test ah,32
	jz LLen17_9
	test ah,64
	jz LLen17_10
	test ah,128
	jz LLen17_11
	jmp LFail
LLen17_0:
jmp LCopyByte5
LLen17_1:
	mov edx,eax
	shr edx,6
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte7
LLen17_2:
	mov edx,eax
	shr edx,7
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte1
LLen17_3:
	mov edx,eax
	shr edx,8
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte3
	jmp LCopyDword3
LLen17_4:
	mov edx,eax
	shr edx,9
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte5
	jmp LCopyDword5
LLen17_5:
	mov edx,eax
	shr edx,10
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte7
	jmp LCopyDword7
LLen17_6:
	mov edx,eax
	shr edx,11
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte1
	jmp LCopyDword1
LLen17_7:
	mov edx,eax
	shr edx,12
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte3
	jmp LCopyDword3
LLen17_8:
	mov edx,eax
	shr edx,13
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte5
	jmp LCopyDword5
LLen17_9:
	mov edx,eax
	shr edx,14
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte7
	jmp LCopyDword7
LLen17_10:
	mov edx,eax
	shr edx,15
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte1
	jmp LCopyDword1
LLen17_11:
	mov edx,eax
	shr edx,16
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte3
	jmp LCopyDword3

L9Bit7:
	mov esi,eax
	mov ecx,2
	shr esi,7
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	test al,1
	jz LLen18_0
	test al,2
	jz LLen18_1
	test al,4
	jz LLen18_2
	test al,8
	jz LLen18_3
	test al,16
	jz LLen18_4
	test al,32
	jz LLen18_5
	test al,64
	jz LLen18_6
	test al,128
	jz LLen18_7
	test ah,1
	jz LLen18_8
	test ah,2
	jz LLen18_9
	test ah,4
	jz LLen18_10
	test ah,8
	jz LLen18_11
	jmp LFail
LLen18_0:
jmp LCopyByte1
LLen18_1:
	mov edx,eax
	shr edx,2
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte3
LLen18_2:
	mov edx,eax
	shr edx,3
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte5
LLen18_3:
	mov edx,eax
	shr edx,4
	add ecx,7
	and edx,7
	add ecx,edx
	jmp LCopyDword7
LLen18_4:
	mov edx,eax
	shr edx,5
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword1
LLen18_5:
	mov edx,eax
	shr edx,6
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword3
LLen18_6:
	mov edx,eax
	shr edx,7
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen18_7:
	mov edx,eax
	shr edx,8
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen18_8:
	mov edx,eax
	shr edx,9
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen18_9:
	mov edx,eax
	shr edx,10
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen18_10:
	mov edx,eax
	shr edx,11
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen18_11:
	mov edx,eax
	shr edx,12
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7

L12Bit8:
	mov esi,eax
	mov ecx,2
	shr esi,8
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	test al,16
	jz LLen19_0
	test al,32
	jz LLen19_1
	test al,64
	jz LLen19_2
	test al,128
	jz LLen19_3
	test ah,1
	jz LLen19_4
	test ah,2
	jz LLen19_5
	test ah,4
	jz LLen19_6
	test ah,8
	jz LLen19_7
	test ah,16
	jz LLen19_8
	test ah,32
	jz LLen19_9
	test ah,64
	jz LLen19_10
	test ah,128
	jz LLen19_11
	jmp LFail
LLen19_0:
jmp LCopyByte5
LLen19_1:
	mov edx,eax
	shr edx,6
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte7
LLen19_2:
	mov edx,eax
	shr edx,7
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte1
LLen19_3:
	mov edx,eax
	shr edx,8
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword3
LLen19_4:
	mov edx,eax
	shr edx,9
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen19_5:
	mov edx,eax
	shr edx,10
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen19_6:
	mov edx,eax
	shr edx,11
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen19_7:
	mov edx,eax
	shr edx,12
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen19_8:
	mov edx,eax
	shr edx,13
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen19_9:
	mov edx,eax
	shr edx,14
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7
LLen19_10:
	mov edx,eax
	shr edx,15
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword1
LLen19_11:
	mov edx,eax
	shr edx,16
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword3

L20Bit8:
	mov esi,eax
	mov ecx,3
	shr esi,8
	and esi,1048575
	cmp esi,1048575
	je LDone
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	test al,16
	jz LLen20_0
	test al,32
	jz LLen20_1
	test al,64
	jz LLen20_2
	test al,128
	jz LLen20_3
	test ah,1
	jz LLen20_4
	test ah,2
	jz LLen20_5
	test ah,4
	jz LLen20_6
	test ah,8
	jz LLen20_7
	test ah,16
	jz LLen20_8
	test ah,32
	jz LLen20_9
	test ah,64
	jz LLen20_10
	test ah,128
	jz LLen20_11
	jmp LFail
LLen20_0:
jmp LCopyByte5
LLen20_1:
	mov edx,eax
	shr edx,6
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte7
LLen20_2:
	mov edx,eax
	shr edx,7
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte1
LLen20_3:
	mov edx,eax
	shr edx,8
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword3
LLen20_4:
	mov edx,eax
	shr edx,9
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen20_5:
	mov edx,eax
	shr edx,10
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen20_6:
	mov edx,eax
	shr edx,11
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen20_7:
	mov edx,eax
	shr edx,12
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen20_8:
	mov edx,eax
	shr edx,13
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen20_9:
	mov edx,eax
	shr edx,14
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7
LLen20_10:
	mov edx,eax
	shr edx,15
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword1
LLen20_11:
	mov edx,eax
	shr edx,16
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword3

	// Copy Dword 5
LCopyDword5:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	mov edx,ecx
	shr ecx,2
	and edx,3
	rep movsd
	mov ecx,edx
	rep movsb
	jmp LBlock5

	// Copy Byte 5
LCopyByte5:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	rep movsb

	// Block 5
LBlock5:
	test al,32
	jz LLiteral6
	test al,64
	jz L6Bit7
	test al,128
	jz L9Bit8
	test ah,1
	jz L12Bit9
	jmp L20Bit9

	// Literal 6
LLiteral6:
#ifdef SAFETY
	cmp edi,pbLimDst
	jae LFail
#endif //SAFETY
	mov edx,eax
	shr edx,6
	mov [edi],dl
	inc edi
	mov eax,[ebx-3]
	inc ebx
	jmp LBlock6

L6Bit7:
	mov esi,eax
	mov ecx,2
	shr esi,7
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	test al,32
	jz LLen21_0
	test al,64
	jz LLen21_1
	test al,128
	jz LLen21_2
	test ah,1
	jz LLen21_3
	test ah,2
	jz LLen21_4
	test ah,4
	jz LLen21_5
	test ah,8
	jz LLen21_6
	test ah,16
	jz LLen21_7
	test ah,32
	jz LLen21_8
	test ah,64
	jz LLen21_9
	test ah,128
	jz LLen21_10
	test eax,65536
	jz LLen21_11
	jmp LFail
LLen21_0:
jmp LCopyByte6
LLen21_1:
	mov edx,eax
	shr edx,7
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen21_2:
	mov edx,eax
	shr edx,8
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte2
LLen21_3:
	mov edx,eax
	shr edx,9
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte4
	jmp LCopyDword4
LLen21_4:
	mov edx,eax
	shr edx,10
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte6
	jmp LCopyDword6
LLen21_5:
	mov edx,eax
	shr edx,11
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte0
	jmp LCopyDword0
LLen21_6:
	mov edx,eax
	shr edx,12
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte2
	jmp LCopyDword2
LLen21_7:
	mov edx,eax
	shr edx,13
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte4
	jmp LCopyDword4
LLen21_8:
	mov edx,eax
	shr edx,14
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte6
	jmp LCopyDword6
LLen21_9:
	mov edx,eax
	shr edx,15
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte0
	jmp LCopyDword0
LLen21_10:
	mov edx,eax
	shr edx,16
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte2
	jmp LCopyDword2
LLen21_11:
	mov edx,eax
	shr edx,17
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte4
	jmp LCopyDword4

L9Bit8:
	mov esi,eax
	mov ecx,2
	shr esi,8
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	test al,2
	jz LLen22_0
	test al,4
	jz LLen22_1
	test al,8
	jz LLen22_2
	test al,16
	jz LLen22_3
	test al,32
	jz LLen22_4
	test al,64
	jz LLen22_5
	test al,128
	jz LLen22_6
	test ah,1
	jz LLen22_7
	test ah,2
	jz LLen22_8
	test ah,4
	jz LLen22_9
	test ah,8
	jz LLen22_10
	test ah,16
	jz LLen22_11
	jmp LFail
LLen22_0:
jmp LCopyByte2
LLen22_1:
	mov edx,eax
	shr edx,3
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte4
LLen22_2:
	mov edx,eax
	shr edx,4
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte6
LLen22_3:
	mov edx,eax
	shr edx,5
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword0
LLen22_4:
	mov edx,eax
	shr edx,6
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword2
LLen22_5:
	mov edx,eax
	shr edx,7
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword4
LLen22_6:
	mov edx,eax
	shr edx,8
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen22_7:
	mov edx,eax
	shr edx,9
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen22_8:
	mov edx,eax
	shr edx,10
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen22_9:
	mov edx,eax
	shr edx,11
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen22_10:
	mov edx,eax
	shr edx,12
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen22_11:
	mov edx,eax
	shr edx,13
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0

L12Bit9:
	mov esi,eax
	mov ecx,2
	shr esi,9
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	test al,32
	jz LLen23_0
	test al,64
	jz LLen23_1
	test al,128
	jz LLen23_2
	test ah,1
	jz LLen23_3
	test ah,2
	jz LLen23_4
	test ah,4
	jz LLen23_5
	test ah,8
	jz LLen23_6
	test ah,16
	jz LLen23_7
	test ah,32
	jz LLen23_8
	test ah,64
	jz LLen23_9
	test ah,128
	jz LLen23_10
	test eax,65536
	jz LLen23_11
	jmp LFail
LLen23_0:
jmp LCopyByte6
LLen23_1:
	mov edx,eax
	shr edx,7
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen23_2:
	mov edx,eax
	shr edx,8
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte2
LLen23_3:
	mov edx,eax
	shr edx,9
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword4
LLen23_4:
	mov edx,eax
	shr edx,10
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen23_5:
	mov edx,eax
	shr edx,11
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen23_6:
	mov edx,eax
	shr edx,12
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen23_7:
	mov edx,eax
	shr edx,13
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen23_8:
	mov edx,eax
	shr edx,14
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen23_9:
	mov edx,eax
	shr edx,15
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0
LLen23_10:
	mov edx,eax
	shr edx,16
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword2
LLen23_11:
	mov edx,eax
	shr edx,17
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword4

L20Bit9:
	mov esi,eax
	mov ecx,3
	shr esi,9
	and esi,1048575
	cmp esi,1048575
	je LDone
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	test al,32
	jz LLen24_0
	test al,64
	jz LLen24_1
	test al,128
	jz LLen24_2
	test ah,1
	jz LLen24_3
	test ah,2
	jz LLen24_4
	test ah,4
	jz LLen24_5
	test ah,8
	jz LLen24_6
	test ah,16
	jz LLen24_7
	test ah,32
	jz LLen24_8
	test ah,64
	jz LLen24_9
	test ah,128
	jz LLen24_10
	test eax,65536
	jz LLen24_11
	jmp LFail
LLen24_0:
jmp LCopyByte6
LLen24_1:
	mov edx,eax
	shr edx,7
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen24_2:
	mov edx,eax
	shr edx,8
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte2
LLen24_3:
	mov edx,eax
	shr edx,9
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword4
LLen24_4:
	mov edx,eax
	shr edx,10
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen24_5:
	mov edx,eax
	shr edx,11
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen24_6:
	mov edx,eax
	shr edx,12
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen24_7:
	mov edx,eax
	shr edx,13
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen24_8:
	mov edx,eax
	shr edx,14
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen24_9:
	mov edx,eax
	shr edx,15
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0
LLen24_10:
	mov edx,eax
	shr edx,16
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword2
LLen24_11:
	mov edx,eax
	shr edx,17
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword4

	// Copy Dword 6
LCopyDword6:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	mov edx,ecx
	shr ecx,2
	and edx,3
	rep movsd
	mov ecx,edx
	rep movsb
	jmp LBlock6

	// Copy Byte 6
LCopyByte6:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	rep movsb

	// Block 6
LBlock6:
	test al,64
	jz LLiteral7
	test al,128
	jz L6Bit8
	test ah,1
	jz L9Bit9
	test ah,2
	jz L12Bit10
	jmp L20Bit10

	// Literal 7
LLiteral7:
#ifdef SAFETY
	cmp edi,pbLimDst
	jae LFail
#endif //SAFETY
	mov edx,eax
	shr edx,7
	mov [edi],dl
	inc edi
	mov eax,[ebx-3]
	inc ebx
	jmp LBlock7

L6Bit8:
	mov esi,eax
	mov ecx,2
	shr esi,8
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	test al,64
	jz LLen25_0
	test al,128
	jz LLen25_1
	test ah,1
	jz LLen25_2
	test ah,2
	jz LLen25_3
	test ah,4
	jz LLen25_4
	test ah,8
	jz LLen25_5
	test ah,16
	jz LLen25_6
	test ah,32
	jz LLen25_7
	test ah,64
	jz LLen25_8
	test ah,128
	jz LLen25_9
	test eax,65536
	jz LLen25_10
	test eax,131072
	jz LLen25_11
	jmp LFail
LLen25_0:
jmp LCopyByte7
LLen25_1:
	mov edx,eax
	shr edx,8
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte1
LLen25_2:
	mov edx,eax
	shr edx,9
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte3
LLen25_3:
	mov edx,eax
	shr edx,10
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte5
	jmp LCopyDword5
LLen25_4:
	mov edx,eax
	shr edx,11
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte7
	jmp LCopyDword7
LLen25_5:
	mov edx,eax
	shr edx,12
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte1
	jmp LCopyDword1
LLen25_6:
	mov edx,eax
	shr edx,13
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte3
	jmp LCopyDword3
LLen25_7:
	mov edx,eax
	shr edx,14
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte5
	jmp LCopyDword5
LLen25_8:
	mov edx,eax
	shr edx,15
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte7
	jmp LCopyDword7
LLen25_9:
	mov edx,eax
	shr edx,16
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte1
	jmp LCopyDword1
LLen25_10:
	mov edx,eax
	shr edx,17
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte3
	jmp LCopyDword3
LLen25_11:
	mov edx,eax
	shr edx,18
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte5
	jmp LCopyDword5

L9Bit9:
	mov esi,eax
	mov ecx,2
	shr esi,9
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	test al,4
	jz LLen26_0
	test al,8
	jz LLen26_1
	test al,16
	jz LLen26_2
	test al,32
	jz LLen26_3
	test al,64
	jz LLen26_4
	test al,128
	jz LLen26_5
	test ah,1
	jz LLen26_6
	test ah,2
	jz LLen26_7
	test ah,4
	jz LLen26_8
	test ah,8
	jz LLen26_9
	test ah,16
	jz LLen26_10
	test ah,32
	jz LLen26_11
	jmp LFail
LLen26_0:
jmp LCopyByte3
LLen26_1:
	mov edx,eax
	shr edx,4
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte5
LLen26_2:
	mov edx,eax
	shr edx,5
	add ecx,3
	and edx,3
	add ecx,edx
jmp LCopyByte7
LLen26_3:
	mov edx,eax
	shr edx,6
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword1
LLen26_4:
	mov edx,eax
	shr edx,7
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword3
LLen26_5:
	mov edx,eax
	shr edx,8
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen26_6:
	mov edx,eax
	shr edx,9
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen26_7:
	mov edx,eax
	shr edx,10
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen26_8:
	mov edx,eax
	shr edx,11
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen26_9:
	mov edx,eax
	shr edx,12
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen26_10:
	mov edx,eax
	shr edx,13
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7
LLen26_11:
	mov edx,eax
	shr edx,14
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword1

L12Bit10:
	mov esi,eax
	mov ecx,2
	shr esi,10
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	test al,64
	jz LLen27_0
	test al,128
	jz LLen27_1
	test ah,1
	jz LLen27_2
	test ah,2
	jz LLen27_3
	test ah,4
	jz LLen27_4
	test ah,8
	jz LLen27_5
	test ah,16
	jz LLen27_6
	test ah,32
	jz LLen27_7
	test ah,64
	jz LLen27_8
	test ah,128
	jz LLen27_9
	test eax,65536
	jz LLen27_10
	test eax,131072
	jz LLen27_11
	jmp LFail
LLen27_0:
jmp LCopyByte7
LLen27_1:
	mov edx,eax
	shr edx,8
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte1
LLen27_2:
	mov edx,eax
	shr edx,9
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte3
LLen27_3:
	mov edx,eax
	shr edx,10
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen27_4:
	mov edx,eax
	shr edx,11
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen27_5:
	mov edx,eax
	shr edx,12
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen27_6:
	mov edx,eax
	shr edx,13
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen27_7:
	mov edx,eax
	shr edx,14
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen27_8:
	mov edx,eax
	shr edx,15
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7
LLen27_9:
	mov edx,eax
	shr edx,16
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword1
LLen27_10:
	mov edx,eax
	shr edx,17
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword3
LLen27_11:
	mov edx,eax
	shr edx,18
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword5

L20Bit10:
	mov esi,eax
	mov ecx,3
	shr esi,10
	and esi,1048575
	cmp esi,1048575
	je LDone
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	test al,64
	jz LLen28_0
	test al,128
	jz LLen28_1
	test ah,1
	jz LLen28_2
	test ah,2
	jz LLen28_3
	test ah,4
	jz LLen28_4
	test ah,8
	jz LLen28_5
	test ah,16
	jz LLen28_6
	test ah,32
	jz LLen28_7
	test ah,64
	jz LLen28_8
	test ah,128
	jz LLen28_9
	test eax,65536
	jz LLen28_10
	test eax,131072
	jz LLen28_11
	jmp LFail
LLen28_0:
jmp LCopyByte7
LLen28_1:
	mov edx,eax
	shr edx,8
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte1
LLen28_2:
	mov edx,eax
	shr edx,9
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte3
LLen28_3:
	mov edx,eax
	shr edx,10
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5
LLen28_4:
	mov edx,eax
	shr edx,11
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7
LLen28_5:
	mov edx,eax
	shr edx,12
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1
LLen28_6:
	mov edx,eax
	shr edx,13
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3
LLen28_7:
	mov edx,eax
	shr edx,14
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5
LLen28_8:
	mov edx,eax
	shr edx,15
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7
LLen28_9:
	mov edx,eax
	shr edx,16
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword1
LLen28_10:
	mov edx,eax
	shr edx,17
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword3
LLen28_11:
	mov edx,eax
	shr edx,18
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword5

	// Copy Dword 7
LCopyDword7:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	mov edx,ecx
	shr ecx,2
	and edx,3
	rep movsd
	mov ecx,edx
	rep movsb
	jmp LBlock7

	// Copy Byte 7
LCopyByte7:
#ifdef SAFETY
	push edx
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	pop edx
	ja LFail
#endif //SAFETY
	neg esi
	add esi,edi
#ifdef SAFETY
	cmp esi,pvDst
	jb LFail
#endif //SAFETY
	rep movsb

	// Block 7
LBlock7:
	test al,128
	jz LLiteral8
	test ah,1
	jz L6Bit9
	test ah,2
	jz L9Bit10
	test ah,4
	jz L12Bit11
	jmp L20Bit11

	// Literal 8
LLiteral8:
#ifdef SAFETY
	cmp edi,pbLimDst
	jae LFail
#endif //SAFETY
	mov [edi],ah
	inc edi
	mov eax,[ebx-2]
	add ebx,2
	jmp LBlock0

L6Bit9:
	mov esi,eax
	mov ecx,2
	shr esi,9
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	test al,128
	jz LLen29_0
	test ah,1
	jz LLen29_1
	test ah,2
	jz LLen29_2
	test ah,4
	jz LLen29_3
	test ah,8
	jz LLen29_4
	test ah,16
	jz LLen29_5
	test ah,32
	jz LLen29_6
	test ah,64
	jz LLen29_7
	test ah,128
	jz LLen29_8
	test eax,65536
	jz LLen29_9
	test eax,131072
	jz LLen29_10
	test eax,262144
	jz LLen29_11
	jmp LFail
LLen29_0:
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen29_1:
	mov edx,eax
	shr edx,9
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte2
LLen29_2:
	mov edx,eax
	shr edx,10
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte4
LLen29_3:
	mov edx,eax
	shr edx,11
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte6
	jmp LCopyDword6
LLen29_4:
	mov edx,eax
	shr edx,12
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte0
	jmp LCopyDword0
LLen29_5:
	mov edx,eax
	shr edx,13
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte2
	jmp LCopyDword2
LLen29_6:
	mov edx,eax
	shr edx,14
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte4
	jmp LCopyDword4
LLen29_7:
	mov edx,eax
	shr edx,15
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	cmp esi,4
	jb LCopyByte6
	jmp LCopyDword6
LLen29_8:
	mov edx,eax
	shr edx,16
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte0
	jmp LCopyDword0
LLen29_9:
	mov edx,eax
	shr edx,17
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte2
	jmp LCopyDword2
LLen29_10:
	mov edx,eax
	shr edx,18
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte4
	jmp LCopyDword4
LLen29_11:
	mov edx,eax
	shr edx,19
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	cmp esi,4
	jb LCopyByte6
	jmp LCopyDword6

L9Bit10:
	mov esi,eax
	mov ecx,2
	shr esi,10
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	test al,8
	jz LLen30_0
	test al,16
	jz LLen30_1
	test al,32
	jz LLen30_2
	test al,64
	jz LLen30_3
	test al,128
	jz LLen30_4
	test ah,1
	jz LLen30_5
	test ah,2
	jz LLen30_6
	test ah,4
	jz LLen30_7
	test ah,8
	jz LLen30_8
	test ah,16
	jz LLen30_9
	test ah,32
	jz LLen30_10
	test ah,64
	jz LLen30_11
	jmp LFail
LLen30_0:
jmp LCopyByte4
LLen30_1:
	mov edx,eax
	shr edx,5
	add ecx,1
	and edx,1
	add ecx,edx
jmp LCopyByte6
LLen30_2:
	mov edx,eax
	shr edx,6
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen30_3:
	mov edx,eax
	shr edx,7
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword2
LLen30_4:
	mov edx,eax
	shr edx,8
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword4
LLen30_5:
	mov edx,eax
	shr edx,9
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen30_6:
	mov edx,eax
	shr edx,10
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen30_7:
	mov edx,eax
	shr edx,11
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen30_8:
	mov edx,eax
	shr edx,12
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen30_9:
	mov edx,eax
	shr edx,13
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen30_10:
	mov edx,eax
	shr edx,14
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0
LLen30_11:
	mov edx,eax
	shr edx,15
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword2

L12Bit11:
	mov esi,eax
	mov ecx,2
	shr esi,11
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	test al,128
	jz LLen31_0
	test ah,1
	jz LLen31_1
	test ah,2
	jz LLen31_2
	test ah,4
	jz LLen31_3
	test ah,8
	jz LLen31_4
	test ah,16
	jz LLen31_5
	test ah,32
	jz LLen31_6
	test ah,64
	jz LLen31_7
	test ah,128
	jz LLen31_8
	test eax,65536
	jz LLen31_9
	test eax,131072
	jz LLen31_10
	test eax,262144
	jz LLen31_11
	jmp LFail
LLen31_0:
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen31_1:
	mov edx,eax
	shr edx,9
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte2
LLen31_2:
	mov edx,eax
	shr edx,10
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte4
LLen31_3:
	mov edx,eax
	shr edx,11
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen31_4:
	mov edx,eax
	shr edx,12
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen31_5:
	mov edx,eax
	shr edx,13
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen31_6:
	mov edx,eax
	shr edx,14
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen31_7:
	mov edx,eax
	shr edx,15
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen31_8:
	mov edx,eax
	shr edx,16
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0
LLen31_9:
	mov edx,eax
	shr edx,17
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword2
LLen31_10:
	mov edx,eax
	shr edx,18
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword4
LLen31_11:
	mov edx,eax
	shr edx,19
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword6

L20Bit11:
	mov esi,eax
	mov ecx,3
	shr esi,11
	and esi,1048575
	cmp esi,1048575
	je LDone
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	test al,128
	jz LLen32_0
	test ah,1
	jz LLen32_1
	test ah,2
	jz LLen32_2
	test ah,4
	jz LLen32_3
	test ah,8
	jz LLen32_4
	test ah,16
	jz LLen32_5
	test ah,32
	jz LLen32_6
	test ah,64
	jz LLen32_7
	test ah,128
	jz LLen32_8
	test eax,65536
	jz LLen32_9
	test eax,131072
	jz LLen32_10
	test eax,262144
	jz LLen32_11
	jmp LFail
LLen32_0:
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte0
LLen32_1:
	mov edx,eax
	shr edx,9
	add ecx,1
	and edx,1
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte2
LLen32_2:
	mov edx,eax
	shr edx,10
	add ecx,3
	and edx,3
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
jmp LCopyByte4
LLen32_3:
	mov edx,eax
	shr edx,11
	add ecx,7
	and edx,7
	add ecx,edx
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6
LLen32_4:
	mov edx,eax
	shr edx,12
	add ecx,15
	and edx,15
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0
LLen32_5:
	mov edx,eax
	shr edx,13
	add ecx,31
	and edx,31
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2
LLen32_6:
	mov edx,eax
	shr edx,14
	add ecx,63
	and edx,63
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4
LLen32_7:
	mov edx,eax
	shr edx,15
	add ecx,127
	and edx,127
	add ecx,edx
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6
LLen32_8:
	mov edx,eax
	shr edx,16
	add ecx,255
	and edx,255
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0
LLen32_9:
	mov edx,eax
	shr edx,17
	add ecx,511
	and edx,511
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword2
LLen32_10:
	mov edx,eax
	shr edx,18
	add ecx,1023
	and edx,1023
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword4
LLen32_11:
	mov edx,eax
	shr edx,19
	add ecx,2047
	and edx,2047
	add ecx,edx
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword6

LDone:
	sub edi,pvDst
	mov cbTot,edi
	}

