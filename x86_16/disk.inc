
.equiv I13EXT_CHECK, 0x41
.equiv I13EXT_READ, 0x42
.equiv I13EXT_WRITE, 0x43
.equiv I13EXT_SET_DRIVE_LOCK, 0x45
.equiv I13EXT_READ_DRIVE_PARAMS, 0x48
.equiv I13EXT_CHECK_MEDIA, 0x49

# int 13h: dl=0x80: first hard disk
# ah=00h: reset disk drives (carry set on error)
# ah=01h: get status of last drive operation; result in ah.
# ah=02h: read sectors from drive
# ah=03h: write sectors to drive
# ah=04h: verify sectors
# ah=05h: format track
# ah=06h: format track set bad sector flags
# ah=07h: format drive starting at track
# ah=08h: read drive parameters
# ah=09h: initialize HD disk controller
# ah=0ah: read HD long sectors from drive [!!!]
# ah=0bh: write HD long sectors to drive [!!!]
# ah=0ch: move drive head to cylinder
# ah=0dh: reset disk drives
# ah=0eh: controller read test
# ah=0fh: controller write test
# ah=10h: test whether drive is ready [!!!!]
# ah=11h: recalibrate drive
# ah=12h: [PS/2] controller RAM test
# ah=13h: [PS/2] drive test
# ah=14h: controller diagnostics
# ah=15h: read drive type [!!!]
# ah=16h: [FD] detect media change
# ah=17h: [FD] set media type for format (old)
# ah=18h: [FD] set media type for format
# ah=19h: park heads
# ah=41h: [ext] test whether extensions are available; bx=55aah; [C] ah error code; bx=AA55h; cx interface support bitmask (bits: 0=packet, 1=ejecting, 2=EDD)
# ah=42h: [ext] read sectors from drive [C] DS:SI to DAP buffer
# ah=43h: [ext] write sectors to drive
# ah=44h: [ext] verify sectors
# ah=45h: [ext] lock/unlock drive
# ah=46h: [ext] eject drive
# ah=47h: [ext] move drive head to sector
# ah=48h: [ext] read drive parameters
# ah=49h: [ext] detect media change

i13_init_disk:
	mov $I13EXT_CHECK, %ah
	mov $0x55aa, %bx
	clc
	int $0x13
	# carry; cx interface support.
	jc ext_error
	cmp $0xaa55, %bx
	jne handle_EDD_unsupported
	and $4, %cx
	jz handle_EDD_unsupported
	ret
ext_error:
handle_EDD_unsupported:
	mov $EDD_unsupported_message, %bp
	call i10_write_string
	jmp hang

i13_read_sectors: # parameters: %si.
	mov $I13EXT_READ, %ah
	int $0x13
	jc handle_read_error
	ret
handle_read_error:
	mov $disk_read_error_message, %bp
	call i10_write_string
	jmp hang

hang:
	hlt
	jmp hang

EDD_unsupported_message:
	.asciz "EDD unsupported\r\n"
disk_read_error_message:
	.asciz "disk read error\r\n"
DAP_buffer:
	.byte 0x10 # size of DAP
	.byte 0x0 # unused
	.word 63 # number of sectors to be read (Phoenix maximum is 127, i.e. about 64 KiB)
destbuf_offset_fixup:
	.word 0x0 # dest buffer offset
destbuf_segment_fixup:
	.word 0x100 # stage 2 buffer segment # FIXME dyna calculate where stage2 segment is.
destbuf_sector_fixup:
	.quad 19, 0, 0, 0 # first sector (size of stage 1... eughhh)

