
dict_init:
	mov $1050, %ax
	mov %ax, %fs
	mov $0, %di
	movw $0, %fs:(%di)
	mov $2, %di
	movw $4, %fs:(%di)
	ret
# first word: head (near) addr, if any.
# second word: first free addr.

# record: next, name, ...
.equiv dict, 0
.macro dict_get
	# finds the word-ascii at the top of the stack in the dict. Result: %si: the record found, if any.
	pop %cx
	mov %sp, %bp

	# print debug output
	push %es
	mov %ss, %ax
	mov %ax, %es
	call i10_write_wstring
	pop %es

	# read list head into %si
	mov $0, %di
	mov %fs:(%di), %si

dict_loop1:
	cmp $0, %si # no list item
	je dict_endloop1
dict_check:
	mov %si, %di
	# compare string lengths
	add $2, %di
	cmp %cx, %fs:(%di)
	jne dict_next
	# skip string length
	add $2, %di
	push %cx
dict_string_loop1:
	jcxz dict_string_endloop1_successful
	mov %ss:(%bp), %ax
	cmp %ax, %fs:(%di)
	jne dict_string_endloop1
	add $2, %bp
	add $2, %di
	dec %cx
	jmp dict_string_loop1
dict_string_endloop1_successful:
	pop %cx
	jmp dict_endloop1
dict_string_endloop1:
	pop %cx
dict_next:
	mov (%si), %si
	jmp dict_loop1
dict_endloop1:
	add %sp, %cx
	add %sp, %cx
.endm

.macro dict_put
	# adds the word on the data stack to the dict, with target %ax

.endm
