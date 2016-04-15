
all:
	python3 mbr.py
	as --32 -o stage2.o -I x86_16 stage2.S
	ld -mi386linux -e start --oformat binary -o a.img -T linker.ld -b binary mbr.raw -b elf32-i386 stage2.o
.PHONY: clean distclean debug1 debug2
clean:
	rm -f *.o mbr.raw a.img
distclean: clean
debug1: all
	qemu-system-x86_64 -hda a.img -d in_asm

debug2: all
	qemu-system-x86_64 -hda a.img -s -S -monitor stdio
	# in parallel: gdb
