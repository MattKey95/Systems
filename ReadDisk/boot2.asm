; Second stage of the boot loader

BITS 16

ORG 9000h
	jmp 	Second_Stage

%include "functions_16.asm"
%include "bpb.asm"						; The BIOS Parameter Block (i.e. information about the disk format)
%include "floppy16.asm"					; Routines to access the floppy disk drive

;	Start of the second stage of the boot loader

counter	dw 0020h
reps	dw	0000h
space 	db	0
sector  dw	0
Second_Stage:
    mov		[boot_device], dl			; Boot device number is passed in from first stage in DL. Save it to pass to kernel later.
	mov 	si, second_stage_msg		; Output our greeting message
    call 	Console_WriteLine_16
ask_for_sector:
	mov		si,enter_sector
	call	Console_Write_16
	call	Read_Input
	mov		di,dx
	mov		si,number_of_sectors
	call	Console_Write_16
	call	Read_Input
	mov		word[sector],di
	cmp		dx,0000h ;must read at least 1 sector otherwise the program will crash
	jne		set_reps 
	mov		dx,0001h ;set the number to read to be one if nothing is entered
	mov		si,read_waning_too_few
	call	Console_WriteLine_16
set_reps:
	cmp		dx,0018h ;can only read upto 23 sectors at a time otherwise program will crash
	jl		set_reps_two
	mov		dx,0017h ;set number to read to be 23 because they wanted to read more than max value
	mov		si,read_waning_too_many
	call	Console_WriteLine_16
set_reps_two:
	mov		ax,di ;sector to read
	mov		[reps],dx
	mov		cx,dx ;number of sectors to read
	mov		bx,0D000h ;start address of buffer
	mov		si,0D000h ;set address in si
	call 	ReadSectors
	mov		byte[space],0 ;don't want the write hex function to put a space every two chars
	mov		bx,0000h 	;set offset counter
	mov		di,0D000h	;set start of buffer for future use
	push	di
	push	si
	push	bx
print_sector_message:
	mov		si,contents_of_sector
	call	Console_Write_16
	mov		ax,word[sector]
	call	Console_Write_Dec
write_offset:		;write the offset of memory and increment 16 bytes after writing for next time
	pop		bx		
	mov		byte[space],0
	call	Console_Write_Hex
	call	add_space
	add		bx,0010h
	pop		si
	push	bx
	mov		cx,8 ;set up cx for next section as counter
	mov		byte[space],1 ;now we want a space every two chars for formatting
write_memory:	;write the hex char of memory 
	push	cx ;push cx because write hex uses it
	mov		bx,[si] ;move contents of memory into bx for write hex
	rol		bx,8 ;roll bx as the chars are backwards
	call 	Console_Write_Hex
	call	add_space
	add		si,2 ;move si memory adress along by two bytes
	pop		cx ;get the counter value off the stack
	dec		cx 
	jnz		write_memory
	pop		bx ;keep stack in order for next section
	pop		di
	push	si
	push	bx
	mov		cx,8 ;set up counter like before
write_ascii:		;same as the hex but calling write ascii instead
	push	cx
	mov		bx,[di]
	rol		bx,8
	call	Console_Write_Ascii
	add		di,2
	pop		cx 
	dec		cx
	jnz		write_ascii
	pop		bx 	;keeping stack in order
	pop		si
	push	di
	push	si
	push	bx
new_row:
	call	Console_Write_CRLF
	sub		word[counter],0001h
	cmp		word[counter],0010h ;printed half a sector
	je		Continue
	cmp		word[counter],0000h ;printed a whole sector
	je		Hault
	jmp		write_offset
Continue:
	mov		si,continue
	call	Console_Write_16	
	call	Read_Input	
	jmp		print_sector_message
Hault:	
	inc		word[sector]
	pop		bx
	mov		bx,0000h
	push	bx
	mov		word[counter],0020h
	dec		word[reps]
	jnz		Continue
	pop		bx
	pop		si
	pop		di
	jmp		ask_for_sector
	
second_stage_msg  db 'Second stage of boot loader running', 0
boot_device		  db  0
continue	db	'Press enter to continue...',0
enter_sector 	db	'Please enter sector number...',0
number_of_sectors	db	'Please enter the number of sectors you want to read...',0
contents_of_sector db 'Contents of sector ',0
read_waning_too_many db 'You can only read a maximum of 23 sectors at a time, only 23 sectors will be displayed.',0000h
read_waning_too_few	db 'You must read at least 1 sector, only 1 sector will be displayed.',0

