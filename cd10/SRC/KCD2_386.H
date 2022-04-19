	// Setup
	long cbTot;
	byte *pbLimDst = (byte *)pvDst + cbDst;
	byte *pbLimSrc = (byte *)pvSrc + cbSrc - kcbTailKcd2;

__asm
	{
	mov edi,pvDst
	mov ebx,pvSrc
	inc ebx
	mov eax,[ebx]
	add ebx,4
	jmp LBlock0

LLitDword7:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	mov eax,ecx
	shr ecx,2
	and eax,3
	rep movsd
	mov ecx,eax
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	jmp LBlock0

L6BitOffDword0:
	mov esi,eax
	inc ecx
	shr esi,2
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte0
	jmp LCopyDword0

L9BitOffDword4:
	mov esi,eax
	inc ecx
	shr esi,7
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0

L12BitOffDword0:
	mov esi,eax
	inc ecx
	shr esi,4
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword0

L20BitOffDword0:
	mov esi,eax
	add ecx,2
	shr esi,4
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword0

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

LLitSingle7:
#ifdef SAFETY
	cmp edi,pbLimDst
	jae LFail
#endif //SAFETY
	mov [edi],ah
	inc edi
	mov eax,[ebx-2]
	add ebx,2
	jmp LBlock0

LLitByte7:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	jmp LBlock0

L6BitOffByte0:
	mov esi,eax
	inc ecx
	shr esi,2
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte0

L9BitOffByte4:
	mov esi,eax
	inc ecx
	shr esi,7
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte0

L12BitOffByte0:
	mov esi,eax
	inc ecx
	shr esi,4
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte0

L20BitOffByte0:
	mov esi,eax
	add ecx,2
	shr esi,4
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyByte0

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
	jz LLen0_0
	test al,2
	jz LLen0_1
	test al,4
	jz LLen0_2
	test al,8
	jz LLen0_3
	test al,16
	jz LLen0_4
	test al,32
	jz LLen0_5
	test al,64
	jz LLen0_6
	test al,128
	jz LLen0_7
	test ah,1
	jz LLen0_8
	test ah,2
	jz LLen0_9
	test ah,4
	jz LLen0_10
	test ah,8
	jz LLen0_11
	jmp LDone
LLen0_0:
	mov ecx,1
	test al,2
	jz LLitSingle1
	test al,4
	jz L6BitOffByte1
	test al,8
	jz L9BitOffByte1
	test al,16
	jz L12BitOffByte1
	jmp L20BitOffByte1
LLen0_1:
	mov ecx,eax
	shr ecx,2
	and ecx,1
	or ecx,2
	test al,8
	jz LLitByte3
	test al,16
	jz L6BitOffByte3
	test al,32
	jz L9BitOffByte3
	test al,64
	jz L12BitOffByte3
	jmp L20BitOffByte3
LLen0_2:
	mov ecx,eax
	shr ecx,3
	and ecx,3
	or ecx,4
	test al,32
	jz LLitByte5
	test al,64
	jz L6BitOffByte5
	test al,128
	jz L9BitOffByte5
	test ah,1
	jz L12BitOffByte5
	jmp L20BitOffByte5
LLen0_3:
	mov ecx,eax
	shr ecx,4
	and ecx,7
	or ecx,8
	test al,128
	jz LLitDword7
	test ah,1
	jz L6BitOffDword7
	test ah,2
	jz L9BitOffDword7
	test ah,4
	jz L12BitOffDword7
	jmp L20BitOffDword7
LLen0_4:
	mov ecx,eax
	shr ecx,5
	and ecx,15
	or ecx,16
	mov eax,[ebx-3]
	inc ebx
	test al,2
	jz LLitDword1
	test al,4
	jz L6BitOffDword1
	test al,8
	jz L9BitOffDword1
	test al,16
	jz L12BitOffDword1
	jmp L20BitOffDword1
LLen0_5:
	mov ecx,eax
	shr ecx,6
	and ecx,31
	or ecx,32
	mov eax,[ebx-3]
	inc ebx
	test al,8
	jz LLitDword3
	test al,16
	jz L6BitOffDword3
	test al,32
	jz L9BitOffDword3
	test al,64
	jz L12BitOffDword3
	jmp L20BitOffDword3
LLen0_6:
	mov ecx,eax
	shr ecx,7
	and ecx,63
	or ecx,64
	mov eax,[ebx-3]
	inc ebx
	test al,32
	jz LLitDword5
	test al,64
	jz L6BitOffDword5
	test al,128
	jz L9BitOffDword5
	test ah,1
	jz L12BitOffDword5
	jmp L20BitOffDword5
LLen0_7:
	mov ecx,eax
	shr ecx,8
	and ecx,127
	or ecx,128
	mov eax,[ebx-3]
	inc ebx
	test al,128
	jz LLitDword7
	test ah,1
	jz L6BitOffDword7
	test ah,2
	jz L9BitOffDword7
	test ah,4
	jz L12BitOffDword7
	jmp L20BitOffDword7
LLen0_8:
	mov ecx,eax
	shr ecx,9
	and ecx,255
	or ecx,256
	mov eax,[ebx-2]
	add ebx,2
	test al,2
	jz LLitDword1
	test al,4
	jz L6BitOffDword1
	test al,8
	jz L9BitOffDword1
	test al,16
	jz L12BitOffDword1
	jmp L20BitOffDword1
LLen0_9:
	mov ecx,eax
	shr ecx,10
	and ecx,511
	or ecx,512
	mov eax,[ebx-2]
	add ebx,2
	test al,8
	jz LLitDword3
	test al,16
	jz L6BitOffDword3
	test al,32
	jz L9BitOffDword3
	test al,64
	jz L12BitOffDword3
	jmp L20BitOffDword3
LLen0_10:
	mov ecx,eax
	shr ecx,11
	and ecx,1023
	or ecx,1024
	mov eax,[ebx-2]
	add ebx,2
	test al,32
	jz LLitDword5
	test al,64
	jz L6BitOffDword5
	test al,128
	jz L9BitOffDword5
	test ah,1
	jz L12BitOffDword5
	jmp L20BitOffDword5
LLen0_11:
	mov ecx,eax
	shr ecx,12
	and ecx,2047
	or ecx,2048
	mov eax,[ebx-2]
	add ebx,2
	test al,128
	jz LLitDword7
	test ah,1
	jz L6BitOffDword7
	test ah,2
	jz L9BitOffDword7
	test ah,4
	jz L12BitOffDword7
	jmp L20BitOffDword7

LLitDword0:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,1
	and edx,127
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	mov eax,ecx
	shr ecx,2
	and eax,3
	rep movsd
	mov ecx,eax
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,7
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock1

L6BitOffDword1:
	mov esi,eax
	inc ecx
	shr esi,3
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte1
	jmp LCopyDword1

L9BitOffDword5:
	mov esi,eax
	inc ecx
	shr esi,8
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1

L12BitOffDword1:
	mov esi,eax
	inc ecx
	shr esi,5
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword1

L20BitOffDword1:
	mov esi,eax
	add ecx,2
	shr esi,5
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword1

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

LLitSingle0:
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

LLitByte0:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,1
	and edx,127
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,7
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock1

L6BitOffByte1:
	mov esi,eax
	inc ecx
	shr esi,3
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte1

L9BitOffByte5:
	mov esi,eax
	inc ecx
	shr esi,8
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte1

L12BitOffByte1:
	mov esi,eax
	inc ecx
	shr esi,5
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte1

L20BitOffByte1:
	mov esi,eax
	add ecx,2
	shr esi,5
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyByte1

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
	jz LLen1_0
	test al,4
	jz LLen1_1
	test al,8
	jz LLen1_2
	test al,16
	jz LLen1_3
	test al,32
	jz LLen1_4
	test al,64
	jz LLen1_5
	test al,128
	jz LLen1_6
	test ah,1
	jz LLen1_7
	test ah,2
	jz LLen1_8
	test ah,4
	jz LLen1_9
	test ah,8
	jz LLen1_10
	test ah,16
	jz LLen1_11
	jmp LDone
LLen1_0:
	mov ecx,1
	test al,4
	jz LLitSingle2
	test al,8
	jz L6BitOffByte2
	test al,16
	jz L9BitOffByte2
	test al,32
	jz L12BitOffByte2
	jmp L20BitOffByte2
LLen1_1:
	mov ecx,eax
	shr ecx,3
	and ecx,1
	or ecx,2
	test al,16
	jz LLitByte4
	test al,32
	jz L6BitOffByte4
	test al,64
	jz L9BitOffByte4
	test al,128
	jz L12BitOffByte4
	jmp L20BitOffByte4
LLen1_2:
	mov ecx,eax
	shr ecx,4
	and ecx,3
	or ecx,4
	test al,64
	jz LLitByte6
	test al,128
	jz L6BitOffByte6
	test ah,1
	jz L9BitOffByte6
	test ah,2
	jz L12BitOffByte6
	jmp L20BitOffByte6
LLen1_3:
	mov ecx,eax
	shr ecx,5
	and ecx,7
	or ecx,8
	mov eax,[ebx-3]
	inc ebx
	test al,1
	jz LLitDword0
	test al,2
	jz L6BitOffDword0
	test al,4
	jz L9BitOffDword0
	test al,8
	jz L12BitOffDword0
	jmp L20BitOffDword0
LLen1_4:
	mov ecx,eax
	shr ecx,6
	and ecx,15
	or ecx,16
	mov eax,[ebx-3]
	inc ebx
	test al,4
	jz LLitDword2
	test al,8
	jz L6BitOffDword2
	test al,16
	jz L9BitOffDword2
	test al,32
	jz L12BitOffDword2
	jmp L20BitOffDword2
LLen1_5:
	mov ecx,eax
	shr ecx,7
	and ecx,31
	or ecx,32
	mov eax,[ebx-3]
	inc ebx
	test al,16
	jz LLitDword4
	test al,32
	jz L6BitOffDword4
	test al,64
	jz L9BitOffDword4
	test al,128
	jz L12BitOffDword4
	jmp L20BitOffDword4
LLen1_6:
	mov ecx,eax
	shr ecx,8
	and ecx,63
	or ecx,64
	mov eax,[ebx-3]
	inc ebx
	test al,64
	jz LLitDword6
	test al,128
	jz L6BitOffDword6
	test ah,1
	jz L9BitOffDword6
	test ah,2
	jz L12BitOffDword6
	jmp L20BitOffDword6
LLen1_7:
	mov ecx,eax
	shr ecx,9
	and ecx,127
	or ecx,128
	mov eax,[ebx-2]
	add ebx,2
	test al,1
	jz LLitDword0
	test al,2
	jz L6BitOffDword0
	test al,4
	jz L9BitOffDword0
	test al,8
	jz L12BitOffDword0
	jmp L20BitOffDword0
LLen1_8:
	mov ecx,eax
	shr ecx,10
	and ecx,255
	or ecx,256
	mov eax,[ebx-2]
	add ebx,2
	test al,4
	jz LLitDword2
	test al,8
	jz L6BitOffDword2
	test al,16
	jz L9BitOffDword2
	test al,32
	jz L12BitOffDword2
	jmp L20BitOffDword2
LLen1_9:
	mov ecx,eax
	shr ecx,11
	and ecx,511
	or ecx,512
	mov eax,[ebx-2]
	add ebx,2
	test al,16
	jz LLitDword4
	test al,32
	jz L6BitOffDword4
	test al,64
	jz L9BitOffDword4
	test al,128
	jz L12BitOffDword4
	jmp L20BitOffDword4
LLen1_10:
	mov ecx,eax
	shr ecx,12
	and ecx,1023
	or ecx,1024
	mov eax,[ebx-2]
	add ebx,2
	test al,64
	jz LLitDword6
	test al,128
	jz L6BitOffDword6
	test ah,1
	jz L9BitOffDword6
	test ah,2
	jz L12BitOffDword6
	jmp L20BitOffDword6
LLen1_11:
	mov ecx,eax
	shr ecx,13
	and ecx,2047
	or ecx,2048
	mov eax,[ebx-1]
	add ebx,3
	test al,1
	jz LLitDword0
	test al,2
	jz L6BitOffDword0
	test al,4
	jz L9BitOffDword0
	test al,8
	jz L12BitOffDword0
	jmp L20BitOffDword0

LLitDword1:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,2
	and edx,63
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	mov eax,ecx
	shr ecx,2
	and eax,3
	rep movsd
	mov ecx,eax
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,6
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock2

L6BitOffDword2:
	mov esi,eax
	inc ecx
	shr esi,4
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte2
	jmp LCopyDword2

L9BitOffDword6:
	mov esi,eax
	inc ecx
	shr esi,9
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2

L12BitOffDword2:
	mov esi,eax
	inc ecx
	shr esi,6
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword2

L20BitOffDword2:
	mov esi,eax
	add ecx,2
	shr esi,6
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword2

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

LLitSingle1:
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

LLitByte1:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,2
	and edx,63
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,6
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock2

L6BitOffByte2:
	mov esi,eax
	inc ecx
	shr esi,4
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte2

L9BitOffByte6:
	mov esi,eax
	inc ecx
	shr esi,9
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte2

L12BitOffByte2:
	mov esi,eax
	inc ecx
	shr esi,6
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte2

L20BitOffByte2:
	mov esi,eax
	add ecx,2
	shr esi,6
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyByte2

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
	jz LLen2_0
	test al,8
	jz LLen2_1
	test al,16
	jz LLen2_2
	test al,32
	jz LLen2_3
	test al,64
	jz LLen2_4
	test al,128
	jz LLen2_5
	test ah,1
	jz LLen2_6
	test ah,2
	jz LLen2_7
	test ah,4
	jz LLen2_8
	test ah,8
	jz LLen2_9
	test ah,16
	jz LLen2_10
	test ah,32
	jz LLen2_11
	jmp LDone
LLen2_0:
	mov ecx,1
	test al,8
	jz LLitSingle3
	test al,16
	jz L6BitOffByte3
	test al,32
	jz L9BitOffByte3
	test al,64
	jz L12BitOffByte3
	jmp L20BitOffByte3
LLen2_1:
	mov ecx,eax
	shr ecx,4
	and ecx,1
	or ecx,2
	test al,32
	jz LLitByte5
	test al,64
	jz L6BitOffByte5
	test al,128
	jz L9BitOffByte5
	test ah,1
	jz L12BitOffByte5
	jmp L20BitOffByte5
LLen2_2:
	mov ecx,eax
	shr ecx,5
	and ecx,3
	or ecx,4
	test al,128
	jz LLitByte7
	test ah,1
	jz L6BitOffByte7
	test ah,2
	jz L9BitOffByte7
	test ah,4
	jz L12BitOffByte7
	jmp L20BitOffByte7
LLen2_3:
	mov ecx,eax
	shr ecx,6
	and ecx,7
	or ecx,8
	mov eax,[ebx-3]
	inc ebx
	test al,2
	jz LLitDword1
	test al,4
	jz L6BitOffDword1
	test al,8
	jz L9BitOffDword1
	test al,16
	jz L12BitOffDword1
	jmp L20BitOffDword1
LLen2_4:
	mov ecx,eax
	shr ecx,7
	and ecx,15
	or ecx,16
	mov eax,[ebx-3]
	inc ebx
	test al,8
	jz LLitDword3
	test al,16
	jz L6BitOffDword3
	test al,32
	jz L9BitOffDword3
	test al,64
	jz L12BitOffDword3
	jmp L20BitOffDword3
LLen2_5:
	mov ecx,eax
	shr ecx,8
	and ecx,31
	or ecx,32
	mov eax,[ebx-3]
	inc ebx
	test al,32
	jz LLitDword5
	test al,64
	jz L6BitOffDword5
	test al,128
	jz L9BitOffDword5
	test ah,1
	jz L12BitOffDword5
	jmp L20BitOffDword5
LLen2_6:
	mov ecx,eax
	shr ecx,9
	and ecx,63
	or ecx,64
	mov eax,[ebx-3]
	inc ebx
	test al,128
	jz LLitDword7
	test ah,1
	jz L6BitOffDword7
	test ah,2
	jz L9BitOffDword7
	test ah,4
	jz L12BitOffDword7
	jmp L20BitOffDword7
LLen2_7:
	mov ecx,eax
	shr ecx,10
	and ecx,127
	or ecx,128
	mov eax,[ebx-2]
	add ebx,2
	test al,2
	jz LLitDword1
	test al,4
	jz L6BitOffDword1
	test al,8
	jz L9BitOffDword1
	test al,16
	jz L12BitOffDword1
	jmp L20BitOffDword1
LLen2_8:
	mov ecx,eax
	shr ecx,11
	and ecx,255
	or ecx,256
	mov eax,[ebx-2]
	add ebx,2
	test al,8
	jz LLitDword3
	test al,16
	jz L6BitOffDword3
	test al,32
	jz L9BitOffDword3
	test al,64
	jz L12BitOffDword3
	jmp L20BitOffDword3
LLen2_9:
	mov ecx,eax
	shr ecx,12
	and ecx,511
	or ecx,512
	mov eax,[ebx-2]
	add ebx,2
	test al,32
	jz LLitDword5
	test al,64
	jz L6BitOffDword5
	test al,128
	jz L9BitOffDword5
	test ah,1
	jz L12BitOffDword5
	jmp L20BitOffDword5
LLen2_10:
	mov ecx,eax
	shr ecx,13
	and ecx,1023
	or ecx,1024
	mov eax,[ebx-2]
	add ebx,2
	test al,128
	jz LLitDword7
	test ah,1
	jz L6BitOffDword7
	test ah,2
	jz L9BitOffDword7
	test ah,4
	jz L12BitOffDword7
	jmp L20BitOffDword7
LLen2_11:
	mov ecx,eax
	shr ecx,14
	and ecx,2047
	or ecx,2048
	mov eax,[ebx-1]
	add ebx,3
	test al,2
	jz LLitDword1
	test al,4
	jz L6BitOffDword1
	test al,8
	jz L9BitOffDword1
	test al,16
	jz L12BitOffDword1
	jmp L20BitOffDword1

LLitDword2:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,3
	and edx,31
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	mov eax,ecx
	shr ecx,2
	and eax,3
	rep movsd
	mov ecx,eax
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,5
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock3

L6BitOffDword3:
	mov esi,eax
	inc ecx
	shr esi,5
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte3
	jmp LCopyDword3

L9BitOffDword7:
	mov esi,eax
	inc ecx
	shr esi,10
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3

L12BitOffDword3:
	mov esi,eax
	inc ecx
	shr esi,7
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword3

L20BitOffDword3:
	mov esi,eax
	add ecx,2
	shr esi,7
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword3

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

LLitSingle2:
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

LLitByte2:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,3
	and edx,31
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,5
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock3

L6BitOffByte3:
	mov esi,eax
	inc ecx
	shr esi,5
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte3

L9BitOffByte7:
	mov esi,eax
	inc ecx
	shr esi,10
	and esi,511
	add esi,65
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte3

L12BitOffByte3:
	mov esi,eax
	inc ecx
	shr esi,7
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte3

L20BitOffByte3:
	mov esi,eax
	add ecx,2
	shr esi,7
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyByte3

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
	jz LLen3_0
	test al,16
	jz LLen3_1
	test al,32
	jz LLen3_2
	test al,64
	jz LLen3_3
	test al,128
	jz LLen3_4
	test ah,1
	jz LLen3_5
	test ah,2
	jz LLen3_6
	test ah,4
	jz LLen3_7
	test ah,8
	jz LLen3_8
	test ah,16
	jz LLen3_9
	test ah,32
	jz LLen3_10
	test ah,64
	jz LLen3_11
	jmp LDone
LLen3_0:
	mov ecx,1
	test al,16
	jz LLitSingle4
	test al,32
	jz L6BitOffByte4
	test al,64
	jz L9BitOffByte4
	test al,128
	jz L12BitOffByte4
	jmp L20BitOffByte4
LLen3_1:
	mov ecx,eax
	shr ecx,5
	and ecx,1
	or ecx,2
	test al,64
	jz LLitByte6
	test al,128
	jz L6BitOffByte6
	test ah,1
	jz L9BitOffByte6
	test ah,2
	jz L12BitOffByte6
	jmp L20BitOffByte6
LLen3_2:
	mov ecx,eax
	shr ecx,6
	and ecx,3
	or ecx,4
	mov eax,[ebx-3]
	inc ebx
	test al,1
	jz LLitByte0
	test al,2
	jz L6BitOffByte0
	test al,4
	jz L9BitOffByte0
	test al,8
	jz L12BitOffByte0
	jmp L20BitOffByte0
LLen3_3:
	mov ecx,eax
	shr ecx,7
	and ecx,7
	or ecx,8
	mov eax,[ebx-3]
	inc ebx
	test al,4
	jz LLitDword2
	test al,8
	jz L6BitOffDword2
	test al,16
	jz L9BitOffDword2
	test al,32
	jz L12BitOffDword2
	jmp L20BitOffDword2
LLen3_4:
	mov ecx,eax
	shr ecx,8
	and ecx,15
	or ecx,16
	mov eax,[ebx-3]
	inc ebx
	test al,16
	jz LLitDword4
	test al,32
	jz L6BitOffDword4
	test al,64
	jz L9BitOffDword4
	test al,128
	jz L12BitOffDword4
	jmp L20BitOffDword4
LLen3_5:
	mov ecx,eax
	shr ecx,9
	and ecx,31
	or ecx,32
	mov eax,[ebx-3]
	inc ebx
	test al,64
	jz LLitDword6
	test al,128
	jz L6BitOffDword6
	test ah,1
	jz L9BitOffDword6
	test ah,2
	jz L12BitOffDword6
	jmp L20BitOffDword6
LLen3_6:
	mov ecx,eax
	shr ecx,10
	and ecx,63
	or ecx,64
	mov eax,[ebx-2]
	add ebx,2
	test al,1
	jz LLitDword0
	test al,2
	jz L6BitOffDword0
	test al,4
	jz L9BitOffDword0
	test al,8
	jz L12BitOffDword0
	jmp L20BitOffDword0
LLen3_7:
	mov ecx,eax
	shr ecx,11
	and ecx,127
	or ecx,128
	mov eax,[ebx-2]
	add ebx,2
	test al,4
	jz LLitDword2
	test al,8
	jz L6BitOffDword2
	test al,16
	jz L9BitOffDword2
	test al,32
	jz L12BitOffDword2
	jmp L20BitOffDword2
LLen3_8:
	mov ecx,eax
	shr ecx,12
	and ecx,255
	or ecx,256
	mov eax,[ebx-2]
	add ebx,2
	test al,16
	jz LLitDword4
	test al,32
	jz L6BitOffDword4
	test al,64
	jz L9BitOffDword4
	test al,128
	jz L12BitOffDword4
	jmp L20BitOffDword4
LLen3_9:
	mov ecx,eax
	shr ecx,13
	and ecx,511
	or ecx,512
	mov eax,[ebx-2]
	add ebx,2
	test al,64
	jz LLitDword6
	test al,128
	jz L6BitOffDword6
	test ah,1
	jz L9BitOffDword6
	test ah,2
	jz L12BitOffDword6
	jmp L20BitOffDword6
LLen3_10:
	mov ecx,eax
	shr ecx,14
	and ecx,1023
	or ecx,1024
	mov eax,[ebx-1]
	add ebx,3
	test al,1
	jz LLitDword0
	test al,2
	jz L6BitOffDword0
	test al,4
	jz L9BitOffDword0
	test al,8
	jz L12BitOffDword0
	jmp L20BitOffDword0
LLen3_11:
	mov ecx,eax
	shr ecx,15
	and ecx,2047
	or ecx,2048
	mov eax,[ebx-1]
	add ebx,3
	test al,4
	jz LLitDword2
	test al,8
	jz L6BitOffDword2
	test al,16
	jz L9BitOffDword2
	test al,32
	jz L12BitOffDword2
	jmp L20BitOffDword2

LLitDword3:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,4
	and edx,15
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	mov eax,ecx
	shr ecx,2
	and eax,3
	rep movsd
	mov ecx,eax
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,4
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock4

L6BitOffDword4:
	mov esi,eax
	inc ecx
	shr esi,6
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte4
	jmp LCopyDword4

L9BitOffDword0:
	mov esi,eax
	inc ecx
	shr esi,3
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword4

L12BitOffDword4:
	mov esi,eax
	inc ecx
	shr esi,8
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword4

L20BitOffDword4:
	mov esi,eax
	add ecx,2
	shr esi,8
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword4

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

LLitSingle3:
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

LLitByte3:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,4
	and edx,15
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,4
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock4

L6BitOffByte4:
	mov esi,eax
	inc ecx
	shr esi,6
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte4

L9BitOffByte0:
	mov esi,eax
	inc ecx
	shr esi,3
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte4

L12BitOffByte4:
	mov esi,eax
	inc ecx
	shr esi,8
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte4

L20BitOffByte4:
	mov esi,eax
	add ecx,2
	shr esi,8
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyByte4

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
	jz LLen4_0
	test al,32
	jz LLen4_1
	test al,64
	jz LLen4_2
	test al,128
	jz LLen4_3
	test ah,1
	jz LLen4_4
	test ah,2
	jz LLen4_5
	test ah,4
	jz LLen4_6
	test ah,8
	jz LLen4_7
	test ah,16
	jz LLen4_8
	test ah,32
	jz LLen4_9
	test ah,64
	jz LLen4_10
	test ah,128
	jz LLen4_11
	jmp LDone
LLen4_0:
	mov ecx,1
	test al,32
	jz LLitSingle5
	test al,64
	jz L6BitOffByte5
	test al,128
	jz L9BitOffByte5
	test ah,1
	jz L12BitOffByte5
	jmp L20BitOffByte5
LLen4_1:
	mov ecx,eax
	shr ecx,6
	and ecx,1
	or ecx,2
	test al,128
	jz LLitByte7
	test ah,1
	jz L6BitOffByte7
	test ah,2
	jz L9BitOffByte7
	test ah,4
	jz L12BitOffByte7
	jmp L20BitOffByte7
LLen4_2:
	mov ecx,eax
	shr ecx,7
	and ecx,3
	or ecx,4
	mov eax,[ebx-3]
	inc ebx
	test al,2
	jz LLitByte1
	test al,4
	jz L6BitOffByte1
	test al,8
	jz L9BitOffByte1
	test al,16
	jz L12BitOffByte1
	jmp L20BitOffByte1
LLen4_3:
	mov ecx,eax
	shr ecx,8
	and ecx,7
	or ecx,8
	mov eax,[ebx-3]
	inc ebx
	test al,8
	jz LLitDword3
	test al,16
	jz L6BitOffDword3
	test al,32
	jz L9BitOffDword3
	test al,64
	jz L12BitOffDword3
	jmp L20BitOffDword3
LLen4_4:
	mov ecx,eax
	shr ecx,9
	and ecx,15
	or ecx,16
	mov eax,[ebx-3]
	inc ebx
	test al,32
	jz LLitDword5
	test al,64
	jz L6BitOffDword5
	test al,128
	jz L9BitOffDword5
	test ah,1
	jz L12BitOffDword5
	jmp L20BitOffDword5
LLen4_5:
	mov ecx,eax
	shr ecx,10
	and ecx,31
	or ecx,32
	mov eax,[ebx-3]
	inc ebx
	test al,128
	jz LLitDword7
	test ah,1
	jz L6BitOffDword7
	test ah,2
	jz L9BitOffDword7
	test ah,4
	jz L12BitOffDword7
	jmp L20BitOffDword7
LLen4_6:
	mov ecx,eax
	shr ecx,11
	and ecx,63
	or ecx,64
	mov eax,[ebx-2]
	add ebx,2
	test al,2
	jz LLitDword1
	test al,4
	jz L6BitOffDword1
	test al,8
	jz L9BitOffDword1
	test al,16
	jz L12BitOffDword1
	jmp L20BitOffDword1
LLen4_7:
	mov ecx,eax
	shr ecx,12
	and ecx,127
	or ecx,128
	mov eax,[ebx-2]
	add ebx,2
	test al,8
	jz LLitDword3
	test al,16
	jz L6BitOffDword3
	test al,32
	jz L9BitOffDword3
	test al,64
	jz L12BitOffDword3
	jmp L20BitOffDword3
LLen4_8:
	mov ecx,eax
	shr ecx,13
	and ecx,255
	or ecx,256
	mov eax,[ebx-2]
	add ebx,2
	test al,32
	jz LLitDword5
	test al,64
	jz L6BitOffDword5
	test al,128
	jz L9BitOffDword5
	test ah,1
	jz L12BitOffDword5
	jmp L20BitOffDword5
LLen4_9:
	mov ecx,eax
	shr ecx,14
	and ecx,511
	or ecx,512
	mov eax,[ebx-2]
	add ebx,2
	test al,128
	jz LLitDword7
	test ah,1
	jz L6BitOffDword7
	test ah,2
	jz L9BitOffDword7
	test ah,4
	jz L12BitOffDword7
	jmp L20BitOffDword7
LLen4_10:
	mov ecx,eax
	shr ecx,15
	and ecx,1023
	or ecx,1024
	mov eax,[ebx-1]
	add ebx,3
	test al,2
	jz LLitDword1
	test al,4
	jz L6BitOffDword1
	test al,8
	jz L9BitOffDword1
	test al,16
	jz L12BitOffDword1
	jmp L20BitOffDword1
LLen4_11:
	mov ecx,eax
	shr ecx,16
	and ecx,2047
	or ecx,2048
	mov eax,[ebx-1]
	add ebx,3
	test al,8
	jz LLitDword3
	test al,16
	jz L6BitOffDword3
	test al,32
	jz L9BitOffDword3
	test al,64
	jz L12BitOffDword3
	jmp L20BitOffDword3

LLitDword4:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,5
	and edx,7
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	mov eax,ecx
	shr ecx,2
	and eax,3
	rep movsd
	mov ecx,eax
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,3
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock5

L6BitOffDword5:
	mov esi,eax
	inc ecx
	shr esi,7
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte5
	jmp LCopyDword5

L9BitOffDword1:
	mov esi,eax
	inc ecx
	shr esi,4
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword5

L12BitOffDword5:
	mov esi,eax
	inc ecx
	shr esi,9
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword5

L20BitOffDword5:
	mov esi,eax
	add ecx,2
	shr esi,9
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword5

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

LLitSingle4:
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

LLitByte4:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,5
	and edx,7
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,3
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock5

L6BitOffByte5:
	mov esi,eax
	inc ecx
	shr esi,7
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte5

L9BitOffByte1:
	mov esi,eax
	inc ecx
	shr esi,4
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte5

L12BitOffByte5:
	mov esi,eax
	inc ecx
	shr esi,9
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte5

L20BitOffByte5:
	mov esi,eax
	add ecx,2
	shr esi,9
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyByte5

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
	jz LLen5_0
	test al,64
	jz LLen5_1
	test al,128
	jz LLen5_2
	test ah,1
	jz LLen5_3
	test ah,2
	jz LLen5_4
	test ah,4
	jz LLen5_5
	test ah,8
	jz LLen5_6
	test ah,16
	jz LLen5_7
	test ah,32
	jz LLen5_8
	test ah,64
	jz LLen5_9
	test ah,128
	jz LLen5_10
	test eax,65536
	jz LLen5_11
	jmp LDone
LLen5_0:
	mov ecx,1
	test al,64
	jz LLitSingle6
	test al,128
	jz L6BitOffByte6
	test ah,1
	jz L9BitOffByte6
	test ah,2
	jz L12BitOffByte6
	jmp L20BitOffByte6
LLen5_1:
	mov ecx,eax
	shr ecx,7
	and ecx,1
	or ecx,2
	mov eax,[ebx-3]
	inc ebx
	test al,1
	jz LLitByte0
	test al,2
	jz L6BitOffByte0
	test al,4
	jz L9BitOffByte0
	test al,8
	jz L12BitOffByte0
	jmp L20BitOffByte0
LLen5_2:
	mov ecx,eax
	shr ecx,8
	and ecx,3
	or ecx,4
	mov eax,[ebx-3]
	inc ebx
	test al,4
	jz LLitByte2
	test al,8
	jz L6BitOffByte2
	test al,16
	jz L9BitOffByte2
	test al,32
	jz L12BitOffByte2
	jmp L20BitOffByte2
LLen5_3:
	mov ecx,eax
	shr ecx,9
	and ecx,7
	or ecx,8
	mov eax,[ebx-3]
	inc ebx
	test al,16
	jz LLitDword4
	test al,32
	jz L6BitOffDword4
	test al,64
	jz L9BitOffDword4
	test al,128
	jz L12BitOffDword4
	jmp L20BitOffDword4
LLen5_4:
	mov ecx,eax
	shr ecx,10
	and ecx,15
	or ecx,16
	mov eax,[ebx-3]
	inc ebx
	test al,64
	jz LLitDword6
	test al,128
	jz L6BitOffDword6
	test ah,1
	jz L9BitOffDword6
	test ah,2
	jz L12BitOffDword6
	jmp L20BitOffDword6
LLen5_5:
	mov ecx,eax
	shr ecx,11
	and ecx,31
	or ecx,32
	mov eax,[ebx-2]
	add ebx,2
	test al,1
	jz LLitDword0
	test al,2
	jz L6BitOffDword0
	test al,4
	jz L9BitOffDword0
	test al,8
	jz L12BitOffDword0
	jmp L20BitOffDword0
LLen5_6:
	mov ecx,eax
	shr ecx,12
	and ecx,63
	or ecx,64
	mov eax,[ebx-2]
	add ebx,2
	test al,4
	jz LLitDword2
	test al,8
	jz L6BitOffDword2
	test al,16
	jz L9BitOffDword2
	test al,32
	jz L12BitOffDword2
	jmp L20BitOffDword2
LLen5_7:
	mov ecx,eax
	shr ecx,13
	and ecx,127
	or ecx,128
	mov eax,[ebx-2]
	add ebx,2
	test al,16
	jz LLitDword4
	test al,32
	jz L6BitOffDword4
	test al,64
	jz L9BitOffDword4
	test al,128
	jz L12BitOffDword4
	jmp L20BitOffDword4
LLen5_8:
	mov ecx,eax
	shr ecx,14
	and ecx,255
	or ecx,256
	mov eax,[ebx-2]
	add ebx,2
	test al,64
	jz LLitDword6
	test al,128
	jz L6BitOffDword6
	test ah,1
	jz L9BitOffDword6
	test ah,2
	jz L12BitOffDword6
	jmp L20BitOffDword6
LLen5_9:
	mov ecx,eax
	shr ecx,15
	and ecx,511
	or ecx,512
	mov eax,[ebx-1]
	add ebx,3
	test al,1
	jz LLitDword0
	test al,2
	jz L6BitOffDword0
	test al,4
	jz L9BitOffDword0
	test al,8
	jz L12BitOffDword0
	jmp L20BitOffDword0
LLen5_10:
	mov ecx,eax
	shr ecx,16
	and ecx,1023
	or ecx,1024
	mov eax,[ebx-1]
	add ebx,3
	test al,4
	jz LLitDword2
	test al,8
	jz L6BitOffDword2
	test al,16
	jz L9BitOffDword2
	test al,32
	jz L12BitOffDword2
	jmp L20BitOffDword2
LLen5_11:
	mov ecx,eax
	shr ecx,17
	and ecx,2047
	or ecx,2048
	mov eax,[ebx-1]
	add ebx,3
	test al,16
	jz LLitDword4
	test al,32
	jz L6BitOffDword4
	test al,64
	jz L9BitOffDword4
	test al,128
	jz L12BitOffDword4
	jmp L20BitOffDword4

LLitDword5:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,6
	and edx,3
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	mov eax,ecx
	shr ecx,2
	and eax,3
	rep movsd
	mov ecx,eax
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,2
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock6

L6BitOffDword6:
	mov esi,eax
	inc ecx
	shr esi,8
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte6
	jmp LCopyDword6

L9BitOffDword2:
	mov esi,eax
	inc ecx
	shr esi,5
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword6

L12BitOffDword6:
	mov esi,eax
	inc ecx
	shr esi,10
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword6

L20BitOffDword6:
	mov esi,eax
	add ecx,2
	shr esi,10
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword6

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

LLitSingle5:
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

LLitByte5:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,6
	and edx,3
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,2
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock6

L6BitOffByte6:
	mov esi,eax
	inc ecx
	shr esi,8
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte6

L9BitOffByte2:
	mov esi,eax
	inc ecx
	shr esi,5
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte6

L12BitOffByte6:
	mov esi,eax
	inc ecx
	shr esi,10
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte6

L20BitOffByte6:
	mov esi,eax
	add ecx,2
	shr esi,10
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyByte6

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
	jz LLen6_0
	test al,128
	jz LLen6_1
	test ah,1
	jz LLen6_2
	test ah,2
	jz LLen6_3
	test ah,4
	jz LLen6_4
	test ah,8
	jz LLen6_5
	test ah,16
	jz LLen6_6
	test ah,32
	jz LLen6_7
	test ah,64
	jz LLen6_8
	test ah,128
	jz LLen6_9
	test eax,65536
	jz LLen6_10
	test eax,131072
	jz LLen6_11
	jmp LDone
LLen6_0:
	mov ecx,1
	test al,128
	jz LLitSingle7
	test ah,1
	jz L6BitOffByte7
	test ah,2
	jz L9BitOffByte7
	test ah,4
	jz L12BitOffByte7
	jmp L20BitOffByte7
LLen6_1:
	mov ecx,eax
	shr ecx,8
	and ecx,1
	or ecx,2
	mov eax,[ebx-3]
	inc ebx
	test al,2
	jz LLitByte1
	test al,4
	jz L6BitOffByte1
	test al,8
	jz L9BitOffByte1
	test al,16
	jz L12BitOffByte1
	jmp L20BitOffByte1
LLen6_2:
	mov ecx,eax
	shr ecx,9
	and ecx,3
	or ecx,4
	mov eax,[ebx-3]
	inc ebx
	test al,8
	jz LLitByte3
	test al,16
	jz L6BitOffByte3
	test al,32
	jz L9BitOffByte3
	test al,64
	jz L12BitOffByte3
	jmp L20BitOffByte3
LLen6_3:
	mov ecx,eax
	shr ecx,10
	and ecx,7
	or ecx,8
	mov eax,[ebx-3]
	inc ebx
	test al,32
	jz LLitDword5
	test al,64
	jz L6BitOffDword5
	test al,128
	jz L9BitOffDword5
	test ah,1
	jz L12BitOffDword5
	jmp L20BitOffDword5
LLen6_4:
	mov ecx,eax
	shr ecx,11
	and ecx,15
	or ecx,16
	mov eax,[ebx-3]
	inc ebx
	test al,128
	jz LLitDword7
	test ah,1
	jz L6BitOffDword7
	test ah,2
	jz L9BitOffDword7
	test ah,4
	jz L12BitOffDword7
	jmp L20BitOffDword7
LLen6_5:
	mov ecx,eax
	shr ecx,12
	and ecx,31
	or ecx,32
	mov eax,[ebx-2]
	add ebx,2
	test al,2
	jz LLitDword1
	test al,4
	jz L6BitOffDword1
	test al,8
	jz L9BitOffDword1
	test al,16
	jz L12BitOffDword1
	jmp L20BitOffDword1
LLen6_6:
	mov ecx,eax
	shr ecx,13
	and ecx,63
	or ecx,64
	mov eax,[ebx-2]
	add ebx,2
	test al,8
	jz LLitDword3
	test al,16
	jz L6BitOffDword3
	test al,32
	jz L9BitOffDword3
	test al,64
	jz L12BitOffDword3
	jmp L20BitOffDword3
LLen6_7:
	mov ecx,eax
	shr ecx,14
	and ecx,127
	or ecx,128
	mov eax,[ebx-2]
	add ebx,2
	test al,32
	jz LLitDword5
	test al,64
	jz L6BitOffDword5
	test al,128
	jz L9BitOffDword5
	test ah,1
	jz L12BitOffDword5
	jmp L20BitOffDword5
LLen6_8:
	mov ecx,eax
	shr ecx,15
	and ecx,255
	or ecx,256
	mov eax,[ebx-2]
	add ebx,2
	test al,128
	jz LLitDword7
	test ah,1
	jz L6BitOffDword7
	test ah,2
	jz L9BitOffDword7
	test ah,4
	jz L12BitOffDword7
	jmp L20BitOffDword7
LLen6_9:
	mov ecx,eax
	shr ecx,16
	and ecx,511
	or ecx,512
	mov eax,[ebx-1]
	add ebx,3
	test al,2
	jz LLitDword1
	test al,4
	jz L6BitOffDword1
	test al,8
	jz L9BitOffDword1
	test al,16
	jz L12BitOffDword1
	jmp L20BitOffDword1
LLen6_10:
	mov ecx,eax
	shr ecx,17
	and ecx,1023
	or ecx,1024
	mov eax,[ebx-1]
	add ebx,3
	test al,8
	jz LLitDword3
	test al,16
	jz L6BitOffDword3
	test al,32
	jz L9BitOffDword3
	test al,64
	jz L12BitOffDword3
	jmp L20BitOffDword3
LLen6_11:
	mov ecx,eax
	shr ecx,18
	and ecx,2047
	or ecx,2048
	mov eax,[ebx-1]
	add ebx,3
	test al,32
	jz LLitDword5
	test al,64
	jz L6BitOffDword5
	test al,128
	jz L9BitOffDword5
	test ah,1
	jz L12BitOffDword5
	jmp L20BitOffDword5

LLitDword6:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,7
	and edx,1
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	mov eax,ecx
	shr ecx,2
	and eax,3
	rep movsd
	mov ecx,eax
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,1
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock7

L6BitOffDword7:
	mov esi,eax
	inc ecx
	shr esi,9
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	cmp esi,4
	jb LCopyByte7
	jmp LCopyDword7

L9BitOffDword3:
	mov esi,eax
	inc ecx
	shr esi,6
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyDword7

L12BitOffDword7:
	mov esi,eax
	inc ecx
	shr esi,11
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyDword7

L20BitOffDword7:
	mov esi,eax
	add ecx,2
	shr esi,11
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyDword7

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

LLitSingle6:
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

LLitByte6:
#ifdef SAFETY
	lea edx,[edi+ecx]
	cmp edx,pbLimDst
	ja LFail
#endif //SAFETY
	dec ecx
	mov edx,eax
	shr edx,7
	and edx,1
#ifdef SAFETY
	lea esi,[ebx-3+ecx]
	cmp esi,pbLimSrc
	ja LFail
	sub esi,ecx
#else //!SAFETY
	lea esi,[ebx-3]
#endif //!SAFETY
	rep movsb
	lea ebx,[esi+4]
	mov eax,[esi]
	mov esi,eax
	shl esi,1
	or edx,esi
	mov [edi],dl
	inc edi
	jmp LBlock7

L6BitOffByte7:
	mov esi,eax
	inc ecx
	shr esi,9
	and esi,63
	inc esi
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte7

L9BitOffByte3:
	mov esi,eax
	inc ecx
	shr esi,6
	and esi,511
	add esi,65
	mov eax,[ebx-3]
	inc ebx
	jmp LCopyByte7

L12BitOffByte7:
	mov esi,eax
	inc ecx
	shr esi,11
	and esi,4095
	add esi,577
	mov eax,[ebx-2]
	add ebx,2
	jmp LCopyByte7

L20BitOffByte7:
	mov esi,eax
	add ecx,2
	shr esi,11
	and esi,1048575
	add esi,4673
	mov eax,[ebx-1]
	add ebx,3
	jmp LCopyByte7

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
	jz LLen7_0
	test ah,1
	jz LLen7_1
	test ah,2
	jz LLen7_2
	test ah,4
	jz LLen7_3
	test ah,8
	jz LLen7_4
	test ah,16
	jz LLen7_5
	test ah,32
	jz LLen7_6
	test ah,64
	jz LLen7_7
	test ah,128
	jz LLen7_8
	test eax,65536
	jz LLen7_9
	test eax,131072
	jz LLen7_10
	test eax,262144
	jz LLen7_11
	jmp LDone
LLen7_0:
	mov ecx,1
	mov eax,[ebx-3]
	inc ebx
	test al,1
	jz LLitSingle0
	test al,2
	jz L6BitOffByte0
	test al,4
	jz L9BitOffByte0
	test al,8
	jz L12BitOffByte0
	jmp L20BitOffByte0
LLen7_1:
	mov ecx,eax
	shr ecx,9
	and ecx,1
	or ecx,2
	mov eax,[ebx-3]
	inc ebx
	test al,4
	jz LLitByte2
	test al,8
	jz L6BitOffByte2
	test al,16
	jz L9BitOffByte2
	test al,32
	jz L12BitOffByte2
	jmp L20BitOffByte2
LLen7_2:
	mov ecx,eax
	shr ecx,10
	and ecx,3
	or ecx,4
	mov eax,[ebx-3]
	inc ebx
	test al,16
	jz LLitByte4
	test al,32
	jz L6BitOffByte4
	test al,64
	jz L9BitOffByte4
	test al,128
	jz L12BitOffByte4
	jmp L20BitOffByte4
LLen7_3:
	mov ecx,eax
	shr ecx,11
	and ecx,7
	or ecx,8
	mov eax,[ebx-3]
	inc ebx
	test al,64
	jz LLitDword6
	test al,128
	jz L6BitOffDword6
	test ah,1
	jz L9BitOffDword6
	test ah,2
	jz L12BitOffDword6
	jmp L20BitOffDword6
LLen7_4:
	mov ecx,eax
	shr ecx,12
	and ecx,15
	or ecx,16
	mov eax,[ebx-2]
	add ebx,2
	test al,1
	jz LLitDword0
	test al,2
	jz L6BitOffDword0
	test al,4
	jz L9BitOffDword0
	test al,8
	jz L12BitOffDword0
	jmp L20BitOffDword0
LLen7_5:
	mov ecx,eax
	shr ecx,13
	and ecx,31
	or ecx,32
	mov eax,[ebx-2]
	add ebx,2
	test al,4
	jz LLitDword2
	test al,8
	jz L6BitOffDword2
	test al,16
	jz L9BitOffDword2
	test al,32
	jz L12BitOffDword2
	jmp L20BitOffDword2
LLen7_6:
	mov ecx,eax
	shr ecx,14
	and ecx,63
	or ecx,64
	mov eax,[ebx-2]
	add ebx,2
	test al,16
	jz LLitDword4
	test al,32
	jz L6BitOffDword4
	test al,64
	jz L9BitOffDword4
	test al,128
	jz L12BitOffDword4
	jmp L20BitOffDword4
LLen7_7:
	mov ecx,eax
	shr ecx,15
	and ecx,127
	or ecx,128
	mov eax,[ebx-2]
	add ebx,2
	test al,64
	jz LLitDword6
	test al,128
	jz L6BitOffDword6
	test ah,1
	jz L9BitOffDword6
	test ah,2
	jz L12BitOffDword6
	jmp L20BitOffDword6
LLen7_8:
	mov ecx,eax
	shr ecx,16
	and ecx,255
	or ecx,256
	mov eax,[ebx-1]
	add ebx,3
	test al,1
	jz LLitDword0
	test al,2
	jz L6BitOffDword0
	test al,4
	jz L9BitOffDword0
	test al,8
	jz L12BitOffDword0
	jmp L20BitOffDword0
LLen7_9:
	mov ecx,eax
	shr ecx,17
	and ecx,511
	or ecx,512
	mov eax,[ebx-1]
	add ebx,3
	test al,4
	jz LLitDword2
	test al,8
	jz L6BitOffDword2
	test al,16
	jz L9BitOffDword2
	test al,32
	jz L12BitOffDword2
	jmp L20BitOffDword2
LLen7_10:
	mov ecx,eax
	shr ecx,18
	and ecx,1023
	or ecx,1024
	mov eax,[ebx-1]
	add ebx,3
	test al,16
	jz LLitDword4
	test al,32
	jz L6BitOffDword4
	test al,64
	jz L9BitOffDword4
	test al,128
	jz L12BitOffDword4
	jmp L20BitOffDword4
LLen7_11:
	mov ecx,eax
	shr ecx,19
	and ecx,2047
	or ecx,2048
	mov eax,[ebx-1]
	add ebx,3
	test al,64
	jz LLitDword6
	test al,128
	jz L6BitOffDword6
	test ah,1
	jz L9BitOffDword6
	test ah,2
	jz L12BitOffDword6
	jmp L20BitOffDword6

LDone:
	sub edi,pvDst
	mov cbTot,edi
	}

