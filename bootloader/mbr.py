#!/usr/bin/env python
import struct
# load addr: 0x7c00, cs=0. OK registers: CS, SS, SP, DL (this one contains the drive letter), ES:DI (PnP Installation Check Structure (of BBS))
# mem map: < 0x400: interrupt vector table
# then < 0x500: BIOS reserved RAM

# Note: TODO: set up stack. Can't not use one - "int" uses it.
data = [
	# set up %ds

	0x8c, 0xc8, # mov %cs -> %ax
	0x8e, 0xd8, # mov %ax -> %ds

	# set up our moving of ourselves:

	0xb9, 0x00, 0x01, # mov $256, %cx
	0xbb, 0x00, 0x06, # mov $0x0600, %bx
	0xbe, 0x00, 0x7c, # mov $0x7C00, %si

	# copy_l1: move ourself from 0x7C00 to 0x0600

	0xe3, 0x0b, # jecxz copy_e1
	0x8b, 0x04, # mov %ds:(%si), %ax
	0x89, 0x07, # mov %ax, %ds:(%bx)
	0x43, # inc %bx
	0x43, # inc %bx
	0x46, # inc %si
	0x46, # inc %si
	0x49, # dec %cx
	0xeb, 0xf3, # jmp copy_l1

	# copy_e1: jump to our new place.
	0xe9, 0x00, 0x8a, # jmp 0x0000:0x7C00 + (copy_e1 - copy_l1)
	# copy_e2:

	# Note: our code is now located at 0x0600

	# check partition table

	0xb9, 0x04, 0x00, # mov $4, %cx
	0xbd, 0xbe, 0x07, # mov $0x07be, %bp

	# partl1:
	0xe3, 0x0b, # jecxz parte1
	0x3e, 0xf6, 0x46, 0x00, 0x80, # testb $0x80, ds:(%bp)
	0x75, 0x08, # jnz partok
	0x83, 0xc5, 0x10, # add $0x10, %bp
	0x49, # dec %cx
	0xeb, 0xf3, # jmp partl1

	# parte1: [redundant]
	0xcd, 0x18, # int 0x18

	# partok: fix up DAP_header
	0x8b, 0x46, 0x08, # mov 8(%bp), %ax
	0xa3, 0x5e - 2 + 8, 0x06, # mov %ax, (...)
	0x8b, 0x47, 0x0a, # mov 10(%bp), %ax
	0xa3, 0x5e - 2 + 10, 0x06, # mov %ax, (...)

	0x66, 0x8b, 0x4e, 0x0c, # mov 12(%bp), %cx (hopefully regsize is OK)
	0x89, 0x0e, 0x5e, 0x06, # mov %cx, (0x065e)

	# load sectors from disk. Note: max 255 sectors, though we would have space for 1218 sectors starting at 0x7C00.

	0x90, 0x90, # nop nop
	# ^ or: 0xb2, 0x80, # mov $0x80 -> %dl
	0xbe, 0x5e - 2, 0x06, # mov $DAP_header -> ds:%si
	0xb4, 0x42, # mov 0x42 -> %ah
	0xcd, 0x13, # int $0x13 # note: uses %ah, %dl, %ds:%si.
	0x72, 0x05, # jc hang

	# jmp to the loaded sectors. Note that they expect ds:si (MBR partition table entry), ds:bp (same), dx (drive: dl), es:di (Plug&Play).
	0x89, 0xee, # mov %bp, %si
	0xea, 0x00, 0x7C, 0x00, 0x00, # jmp 0x0000:0x7C00
	# hang: [TODO print error message]
	0xcd, 0x18, # int 0x18
	# DAP_header:
	0x10, # size of DAP
	0x00, # unused
	0x9b, 0xa1, # number of sectors to read
	0x00, 0x7C, 0x00, 0x00, # offset, segment of destination buffer
	0x01, 0x50, 0x50, 0x50, 0x00, 0x00, 0x00, 0x00, # starting sector
]
#print("LEN", len(data))
print("9b", data.index(0x9b))
assert(data.index(0x9b) == 0x5e)
while len(data) < 0x1B8:
	data.append(0)
assert(len(data) == 0x1B8)
data += [
	0xAF, 0xFE, 0x10, 0x13, # disk id
	0x00, 0x00,
	# partition table
	##0x40, 0x50, # magic
	##0x00, 0x00, # reserved
	##0x01, 0x00, 0x00, 0x00, # number of partitions
	# partition 1:
	0x80, # bootable
	0xFF, # starting head
	0xFF, # starting sector and starting cylinder.
	0xFF, # starting cylinder.
	0xda, # system ID: non-fs data. Alternative: 0x9 AIX bootable.
	0xFF, # ending head
	0xFF, # ending sector and ending cylinder.
	0xFF, # ending cylinder.
	0x01, 0x00, 0x00, 0x00, # 4 Byte starting sector.
	0x02, 0x00, 0x00, 0x00, # 4 Byte length in sectors.
]
#print("AF", data.index(0xAF))
assert(data.index(0xAF) == 0x1B8)
#print(len(data))
while len(data) < 0x1FE:
	data.append(0)
data += [
	0x55, 0xAA, # boot signature
]
#DAP_header_ref_off = data.index(0xbe) + 1

#print(hang_off)
with open("mbr.raw", "wb") as f:
	f.write(b"".join(struct.pack("B", x) for x in data))
