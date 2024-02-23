all: compile run

compile: make_as
	as/as paging_test.S -o bios
	#fasm bios.asm bios

run: make_emulator
	emulator/emulator -b bios -i

make_as:
	make -C as

make_emulator:
	make -C emulator

clean:
	make -C as clean
	make -C emulator clean
	make -C cc clean
	-rm bios -r
