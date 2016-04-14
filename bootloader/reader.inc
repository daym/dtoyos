
parse_numeral:
	# FIXME
eval_word:
	dict_get # into %si
	# FIXME get default, if applicable.
	ret

read:
	mov $0, %cx # length of word
	push %cx
read_loop1:
	i16_read_key
	cmp $'\b', %al
	je handle_backspace
	cmp $32, %al # is a special char?
	jb read_loop1
	cmp $' ', %al
	je handle_space
handle_regular_key:
	pop %cx
	# doesn't work: mov $0, %ah
	push %ax
	inc %cx
	push %cx
	call i10_write_char
	jmp read_loop1
handle_space:
	pop %cx
	push %cx
	jcxz read_loop1
	# we do this immediately here since we can't return with data on the stack.
	jmp eval_word
handle_backspace:
clear_word:
	pop %cx
	push %cx
	jcxz clear_word_done
	mov $'\b', %al
	call i10_write_char
	mov $' ', %al
	call i10_write_char
	mov $'\b', %al
	call i10_write_char
	pop %cx
	pop %ax # char
	dec %cx
	push %cx
	jmp clear_word
clear_word_done:
	pop %cx
	push %cx
	jcxz read_loop1
	ret
