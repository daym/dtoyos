.include "dictionary.inc"
.include "reader.inc"
.include "printer.inc"

repl:
	mov $'$', %al
	call i10_write_char
	mov $' ', %al
	call i10_write_char
repl_loop1:
	call read
	jmp repl_loop1
