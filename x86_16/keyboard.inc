
.equiv I16_READ_CHAR, 0x0
.equiv I16_READ_INPUT_STATE, 0x1
.equiv I16_READ_SHIFT_STATE, 0x2
.equiv I16_UNGETKEY, 0x05
.equiv I16_READ_CHAR_EXT, 0x10
.equiv I16_READ_INPUT_STATE_EXT, 0x11
.equiv I16_READ_SHIFT_STATE_EXT, 0x12

.equiv I16_SHIFTMASK_RSHIFT, 1
.equiv I16_SHIFTMASK_LSHIFT, 2
.equiv I16_SHIFTMASK_CTRL, 4
.equiv I16_SHIFTMASK_ALT, 8

.macro i16_read_key #: -> (scancode: %ah, ASCIIcode: %al)
	mov $0x10, %ah
	int $0x16
.endm
.macro i16_test_key #: -> (ZF=0 if key pressed; AX=0 if no scan code is available; otherwise: scancode: %ah, ASCIIcode: %al)
	mov $0x11, %ah
	int $0x16
.endm
.macro i16_test_shift #: -> (AX bits: sysreq:15, capslock, numlock, scrolllock, rightalt, rightctrl, leftalt, leftctrl, insert, capslock, numlock, scrolllock, alt, ctrl, leftshift, rightshift)
	mov $0x12, %ah
	int $0x16
.endm
.macro i16_unget_key #: (scancode: %ch, asciicode: %al)
	mov $5, %ah
	int $0x16
.endm
