
.equiv I10_SET_VIDEOMODE, 0x0
.equiv I10_SET_CURSOR_SHAPE, 0x1
.equiv I10_SET_CURSOR_POSITION, 0x2
.equiv I10_SET_ACTIVE_PAGE, 0x05
.equiv I10_SCROLL_UP, 0x06
.equiv I10_SCROLL_DOWN, 0x07
.equiv I10EXT_SET_PALETTE, 0x0B
.equiv I10_PRINT_TTY, 0x0e
.equiv I10_WRITE_STRING, 0x13
.equiv I10VIDEOMODE_40x25, 0x00
.equiv I10VIDEOMODE_80x25, 0x02
.equiv I10VIDEOMODE_80x60, 0x26
# C000h:0031h and around contain ID magic
# Some: 128K A000h-BFFFh range for the video buffer,

# int 10h:
# ah=00h: set video mode (al=video mode)
# ah=01h: set text-mode cursor shape (ch=scan row start, cl=scan row end incl)
# ah=02h: set cursor position (bh=page number, dh=row, dl=column)
# ah=03h: set cursor position and shape
# ah=05h: set active display page (al=page number)
# ah=06h: scroll up window (al=lines to scroll (0=clear), bh=background and foreground color, ch=upper row, cl=left col, dh=lower row, dl=right col; all inc)
# ah=07h: scroll down window
# ah=08h: read character and attribute at cursor position (bh=page number; result: ah=color, al=character)
# ah=09h: write character and attribute at cursor position (al=char, bh=page, bl=color, cx=number of times)
# ah=0ah: write character only at cursor position (al=char, bh=page, bl=color, cx=number of times)
# ah=0bh, bh=00h: set background/border color (bl=color)
# ah=0bh, bh=01h: set palette
# ah=0ch: write graphics pixel (al=color, bh=page number, cx=x, dx=y)
# ah=0dh: read graphics pixel (al=color, bh=page number, cx=x, dx=y)
# ah=0eh: print tty (al=char, bh=page number, bl=color[graphics]); result: bp is destroyed.
# ah=0fh: get current video mode
# ah=13h: write string (al=write mode, bh=page number, bl=color, cx=string length, dh=row, dl=col, es:bp=string)
#                        write mode: bit 0: update cursor after writing
#                                    bit 1: buffer contains alternative characters and attributes
#                                    bit 2-7: reserved

i10_init:
	mov $I10_SET_VIDEOMODE, %ah
	mov $I10VIDEOMODE_80x60, %al
	int $0x10
	ret

i10_cls:
	mov $I10_SCROLL_UP, %ah
	mov $0, %al
	mov $0x12, %bh # color
	mov $0, %cx # Left Upper Corner
	mov $0x1030, %dx # Right Lower Corner
	int $0x10
	ret

i10_write_pstring_at: # parameters: row:%dh, col: %dl, text: %es:%bp
	mov $I10_WRITE_STRING, %ah
	mov $1, %al # update cursor.
	mov $0, %bh # page number. The default, apparently.
	mov $0x52, %bl # color.
	mov %es:(%bp), %cx
	add $2, %bp
	int $0x10
	ret

i10_write_char: # parameters: char:%al
	mov $I10_PRINT_TTY, %ah
	mov $0, %bh # page number
	mov $0x52, %bl # color (graphics)
	int $0x10
	# %bp is sometimes destroyed
	ret

i10_write_pstring: # text: %es:%bp
	mov %es:(%bp), %cx
	add $2, %bp
i10_write_string:
i10_write_pstring_loop1:
	jcxz i10_write_pstring_end
	mov %es:(%bp), %al
	push %bp
	push %cx
	call i10_write_char
	pop %cx
	pop %bp
	dec %cx
	inc %bp
	jmp i10_write_pstring_loop1
i10_write_pstring_end:
	ret

i10_write_wstring: # text: %es:%bp
i10_write_wstring_loop1:
	jcxz i10_write_wstring_end
	mov %es:(%bp), %al
	push %bp
	push %cx
	call i10_write_char
	pop %cx
	pop %bp
	dec %cx
	inc %bp
	inc %bp
	jmp i10_write_wstring_loop1
i10_write_wstring_end:
	ret
