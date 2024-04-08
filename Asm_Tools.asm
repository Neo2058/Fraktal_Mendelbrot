.code
;//------------------------------------------------------------------------------------------------------------
Asm_Draw proc
;extern "C" void Asm_Draw(char *video_buf, SSize size)
;RCX - video_buf
;RDX - size
;Return - none

	mov rdi, rcx
	mov eax, 0ffffffffh

	mov rcx, 100000

	rep stosd

	ret

Asm_Draw endp
;//------------------------------------------------------------------------------------------------------------
Get_Address proc
; ¬ычисл€ет адрес в пам€ти, соответствующий координатам X,Y
; ѕараметры:
; RDX - start_point
; R9 - buf_color
; R11 - video_buf
; ¬озврат: RDI - адрес пиксел€

	push rax
	push rbx
	push r10
	
	movzx r10, r9w	;R10 = R10W = buf_color.Buf_Size.Width

	mov rax, rdx
	shr rax, 16
	movzx rax, ax	;RAX = AX = start_point.Y

	movzx rbx, dx	;RBX = BX = start_point.X

	imul rax, r10	;RAX = RAX * R10 = start_point.Y * buf_color.Buf_Size.Width
	add rax, rbx	;RAX = index = start_point.Y * buf_color.Buf_Size.Width + start_point.X
	shl rax, 2		;RAX = index * 4 - адрес пиксел€ в буфере

	mov rdi, r11	;R11 = video_buf
	add rdi, rax

	pop r10
	pop rbx
	pop rax

	ret

Get_Address endp
;//------------------------------------------------------------------------------------------------------------
Asm_Draw_Line proc
;extern "C" void Asm_Draw_Line(char *video_buf, SPoint start_point, SPoint end_point, SBuf_Color buf_color)
;RCX - video_buf
;RDX - start_point
;R8 - end_point
;R9 - buf_color
;Return - none

	push rax
	push rbx
	push rcx
	push rdi
	push r11
	push r12
	push r13
	push r14
	push r15

	mov r11, rcx

	; Cheking the coordinates X
	cmp r8w, dx
	jbe _exit

	; Cheking the coordinates Y
	mov eax, edx
	shr eax, 16

	mov ebx, r8d
	shr ebx, 16

	cmp bx, ax
	jbe _exit

	call Get_Address	;RDI = адрес в буфере, соответствующий позиции start_point

	mov ax, r8w
	sub ax, dx ; delta.X
	inc ax
	mov r12w, ax

	mov ebx, r8d
	shr ebx, 16

	mov ecx, edx
	shr ecx, 16

	sub bx, cx ; delta.Y
	inc bx
	mov r13w, bx

	; Choise vector line
	cmp ax, bx
	jle _draw_vertical

_draw_horizontal:

	xor r14w, r14w

	movzx rcx, r13w

	movzx r15, r9w
	shl r15, 2

	mov rax, r9
	shr rax, 32		;RAX = EAX = buf_color.Color

_draw_horizontal_pixel:
	stosd

	add r14w, r13w
	cmp r14w, r12w
	jl _draw_horizontal_pixel

	sub r14w, r12w

	add rdi, r15
	loop _draw_horizontal_pixel

	jmp _exit

_draw_vertical:
	xor r14w, r14w

	movzx rcx, r12w

	movzx r15, r9w
	dec r15
	shl r15, 2

	mov rax, r9
	shr rax, 32		;RAX = EAX = buf_color.Color

_draw_vertical_pixel:
	stosd
	add rdi, r15

	add r14w, r12w
	cmp r14w, r13w
	jl _draw_vertical_pixel

	sub r14w, r13w

	add rdi, 4
	loop _draw_vertical_pixel


_exit:

	push r15
	push r14
	push r13
	push r12
	push r11
	push rdi
	push rcx
	push rbx
	push rax

	ret

Asm_Draw_Line endp
;//------------------------------------------------------------------------------------------------------------
Asm_Draw_Horizontal_Line proc
;extern "C" void Asm_Draw_Horizontal_Line(char *video_buf, SPoint start_point, int lenght, SBuf_Color buf_color)
;RCX - video_buf
;RDX - start_point
;R8 - end_point
;R9 - lenght
;Return - none

	push rax
	push rcx
	push rdi
	push r11

	mov r11, rcx

	call Get_Address	;RDI = адрес в буфере, соответствующий позиции start_point
	
	mov rcx, r8	;	RCX = lenght = количество пикселей в линии

	mov rax, r9
	shr rax, 32		;RAX = EAX = buf_color.Color

	rep stosd

	pop r11
	pop rdi
	pop rcx
	pop rax

	ret

Asm_Draw_Horizontal_Line endp
;//------------------------------------------------------------------------------------------------------------

end