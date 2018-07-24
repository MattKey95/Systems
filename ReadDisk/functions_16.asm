; Various sub-routines that will be useful to the boot loader code	

; Output Carriage-Return/Line-Feed (CRLF) sequence to screen using BIOS

Read_Input:
	xor		dx,dx ;clear the registers we are going to be using
	xor		cx,cx
	inputLoop:
	mov		ah,00h 
	int 	16h ;move 0 into ah for the int 16 call to read from keyboard
	mov		cl,al
	cmp		al,0Dh ;check if key is enter, if so exit the loop
	je		exitLoop
	cmp		al,030h ;checking that input is a number via ascii code
	jl		inputLoop
	cmp		al,039h	;checking that the input is a number
	jg		inputLoop
	mov		al,0Ah
	mov		bx,dx ;move previous input into bx to be multiplied
	mul		bl
	mov		dx,ax 
	mov		al,cl
	mov		ah,0Eh
	int		10h	;print inut
	sub		cl,030h ;find number from ascii code
	add		dx,cx ;add number to previous input
	jmp		inputLoop
	exitLoop:
	call	Console_Write_CRLF
	ret

Console_Write_Ascii:
	cmp		bh,020h ; check if char is less than 20 hex on ascii table
	jae		write_bh
	mov		bh,05Fh	;if it is replace it with an underscore
write_bh:
	mov		al,bh
	mov		ah,0Eh
	int		10h		;write char in bh and repeat for bl
	cmp		bl,020h
	jae		write_bl
	mov		bl,05Fh
write_bl:
	mov		al,bl
	int		10h
	ret

add_space:			;writes a space to console
	mov		ah,0Eh
	mov		al,20h
	int 	10h		
	ret
	
Console_Write_Hex:
	mov		dx,0F000h
	mov		cl, 16
	
HexLoop:
	sub 	cl,4
	mov 	di,dx
	and		di,bx
	shr		di,cl
	mov		al,byte[di+HexChars]
	mov		ah,0Eh
	int		10h
	cmp		byte[space],1 ;should the hex have a space in the middle
	jne		no_space
	mov		ax,dx
	cmp		ax,0F00h ;is it the second char out of 4
	jne		no_space
	mov		ah,0Eh
	mov		al,20h
	int 	10h
no_space:
	shr		dx,4
	jne		HexLoop
	ret
HexChars db '0123456789ABCDEF'

Console_Write_Dec:
	mov		si, new_number+4
GetDigit:
	xor		dx,dx ;set dx to 0
	mov 	cx,10 ;set cx to 10
	div 	cx ; divid ax by the value in cx and stor the answer in ah (ax maybe) remader in dl (dx maybe)
	add		dl,48 ;add 48 to dl to get ascii
	mov		[si],dl	;move value of dl into the memory address of si
	dec 	si ;decriment what si points to by 1
	cmp		ax,0 ; check if ax is 0
	jne		GetDigit ;jump if not true
	inc		si ;increment si by 1
	call	Console_WriteLine_16 
	ret

Console_Write_CRLF:
	mov 	ah, 0Eh						; Output CR
    mov 	al, 0Dh
    int 	10h
    mov 	al, 0Ah						; Output LF
    int 	10h
    ret

; Write to the console using BIOS.
; 
; Input: SI points to a null-terminated string

Console_Write_16:
	mov 	ah, 0Eh						; BIOS call to output value in AL to screen

Console_Write_16_Repeat:
	lodsb								; Load byte at SI into AL and increment SI
    test 	al, al						; If the byte is 0, we are done
	je 		Console_Write_16_Done
	int 	10h							; Output character to screen
	jmp 	Console_Write_16_Repeat

Console_Write_16_Done:
    ret


; Write string to the console using BIOS followed by CRLF
; 
; Input: SI points to a null-terminated string

Console_WriteLine_16:
	call 	Console_Write_16
	call 	Console_Write_CRLF
	ret
	
new_number 	db	'    ',0
