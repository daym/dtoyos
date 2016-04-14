.code16
.section .text
.globl start
start:
.globl stage2
.incbin "mbr.raw"
# TODO setup stacks.
stage2:
	jmp main
.include "textconsole.inc"
.include "keyboard.inc"
.include "repl.inc"
main:
	#mov $0x50, %ax
	mov %cs, %ax
	#mov %ax, %ds
	mov %ax, %es # data "stack"

	call i10_init
	call i10_cls
	mov $message, %bp
	call i10_write_pstring
	call dict_init
	call repl
	jmp hang
hang:
	mov $system_halted_message, %bp
	call i10_write_pstring
hang_loop1:
	hlt
	jmp hang_loop1
message:
	.word 18
	.asciz "Stage 2 was here\r\n"
system_halted_message:
	.word 18
	.asciz "System was halted.\r\n"
#heap:
