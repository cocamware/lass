PUBLIC lass_cas8, lass_inc8, lass_dec8, lass_dcas8, lass_dcas16, lass_dcas32

.CODE

lass_cas8 PROC
	; rcx = dest
	; rdx = newValue
	; r8 = expected
	mov rax, r8
	lock cmpxchg BYTE PTR [rcx], dl
	ret
lass_cas8 ENDP

lass_inc8 PROC
	; rcx = value
	lock inc byte ptr [rcx]
	ret
lass_inc8 ENDP

lass_dec8 PROC
	; rcx = value
	lock dec byte ptr [rcx]
	ret
lass_dec8 ENDP

lass_dcas8 PROC
	; rcx = dest
	; dl = new1
	; r8b = new2
	; r9b = expected1
	; [rsp+0x28] = expected2
	shl r8, 8
	mov rax, r9
	or rdx, r8
	mov ah, BYTE PTR [rsp+28h]	
	lock cmpxchg WORD PTR [rcx], dx
	mov rax, 0
	sete al
	ret
lass_dcas8 ENDP

lass_dcas16 PROC
	; rcx = dest
	; dx = new1
	; r8w = new2
	; r9w = expected1
	; [rsp+0x28] = expected2
	shl r8, 16
	mov rax, QWORD PTR [rsp+28h]	
	shl rax, 16
	or rdx, r8
	mov ax, r9w
	lock cmpxchg DWORD PTR [rcx], edx
	mov rax, 0
	sete al
	ret
lass_dcas16 ENDP

lass_dcas32 PROC
	; rcx = dest
	; rdx = new1
	; r8 = new2
	; r9 = expected1
	; [rsp+28h] = expected2
	mov rax, QWORD PTR [rsp+28h]	
	shl r8, 32
	shl rax, 32
	or rdx, r8
	or rax, r9
	lock cmpxchg QWORD PTR [rcx], rdx
	mov rax, 0
	sete al
	ret
lass_dcas32 ENDP

END

