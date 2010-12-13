PUBLIC lass_cas8, lass_inc8, lass_dec8, lass_dcas8, lass_dcas16, lass_dcas32

.CODE

lass_cas8 PROC
	; rcx = dest
	; dl = newValue
	; r8b = expected
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
	mov r8b, dl
	mov ah, BYTE PTR [rsp+28h]
	lock cmpxchg WORD PTR [rcx], r8w
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
	mov r8w, dx
	mov ax, r9w
	lock cmpxchg DWORD PTR [rcx], r8d
	mov rax, 0
	sete al
	ret
lass_dcas16 ENDP

lass_dcas32 PROC
	; rcx = dest
	; edx = new1 = rdx
	; r8d = new2 = r8
	; r9d = expected1 = r9
	; [rsp+28h] = expected2
	shl r8, 32
	mov rax, QWORD PTR [rsp+28h]	
	shl rax, 32
	or r8, rdx
	or rax, r9
	lock cmpxchg QWORD PTR [rcx], r8
	mov rax, 0
	sete al
	ret
lass_dcas32 ENDP

lass_dcas64 PROC
	; rcx = dest
	; rdx = new1
	; r8 = new2
	; r9 = expected1
	; [rsp+28h] = expected2
	mov r10, rbx ; backup
	mov r11, rcx
	mov rbx, rdx
	mov rcx, r8
	mov rax, r9
	mov rdx, QWORD PTR [rsp+28h]
	lock cmpxchg16b [r11]
	mov rax, 0
	sete al
	mov rbx, r10 ; restore
	ret
lass_dcas64 ENDP

END

