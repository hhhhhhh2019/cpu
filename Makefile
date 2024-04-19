all: compile run

compile: make_as
	# as/as vvmhc_test.S -o bios
	as/as bios/bios.S -o bios.bin -m bios.map
	#fasm bios.asm bios

run: make_emulator
	emulator/emulator -b bios.bin

make_as:
	make -C as

make_emulator:
	make -C emulator

clean:
	make -C as clean
	make -C emulator clean
	make -C cc clean
	-rm bios.bin
	-rm bios.map
	-rm -r __pycache__
