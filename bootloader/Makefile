
all:
	python3 mbr.py
	as --32 -o stage2.o -I x86_16 stage2.S
	ld -mi386linux -e start --oformat binary -o a.img -T linker.ld -b binary mbr.raw -b elf32-i386 stage2.o

clean:
	rm -f *.o mbr.raw a.img

distclean: clean
