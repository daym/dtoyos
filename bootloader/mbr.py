#!/usr/bin/env python
import struct
# load addr: 0x7c00, cs=0. OK registers: CS, SS, SP, DL (this one contains the drive letter), ES:DI (PnP Installation Check Structure (of BBS))
# mem map: < 0x400: interrupt vector table
# then < 0x500: BIOS reserved RAM

# Note: TODO: set up stack. Can't not use one - "int" uses it.
data = [
	0x8c, 0xc8, # mov %cs -> %ax
	0x8e, 0xd8, # mov %ax -> %ds
	0x90, 0x90, # nop nop
	# ^ or: 8e, 0xc0, # mov %ax -> %es
	0x90, 0x90, # nop nop
	# ^ or: 0xb2, 0x80, # mov $0x80 -> %dl
	0xbe, 0x19, 0x7c, # mov $DAP_header -> ds:%si
	0xb4, 0x42, # mov 0x42 -> %ah
	0xcd, 0x13, # int $0x13 # note: uses %ah, %dl, ds:%si.
	0x72, 0x05, # jc hang
	0xea, 0x00, 0x02, 0x50, 0x00, # jmp 0x0050:0x0200
	# hang: [TODO print error]
	0xcd, 0x18, # int 0x18
	0x90, # nop
	# DAP_header:
	0x10, # size of DAP
	0x00, # unused
	3, 0, # number of sectors to read
	0x00, 0x00, 0x50, 0x00, # offset, segment of destination buffer
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, # starting sector
]
#print("LEN", len(data))
assert(data.index(0x10) == 0x19)
while len(data) < 0x1B8:
	data.append(0)
data += [
	0xAF, 0xFE, 0x10, 0x13, # disk id
	0x00, 0x00,
	# partition table
	0x40, 0x50, # magic
	0x00, 0x00, # reserved
	0x00, 0x00, 0x00, 0x00, # number of partitions
]
print(len(data))
while len(data) < 0x1FE:
	data.append(0)
data += [
	0x55, 0xAA, # boot signature
]
#DAP_header_ref_off = data.index(0xbe) + 1

#print(hang_off)
with open("mbr.raw", "wb") as f:
	f.write(b"".join(struct.pack("B", x) for x in data))
