#define BIOS_ADDRESS 0x10000

add pc, pc, start - $


zero:
	dl 0


table3:
	dl table2 + BIOS_ADDRESS
table3_end:

table2:
	dl table1 + BIOS_ADDRESS
table2_end:

table1:
	dl table0 + BIOS_ADDRESS
table1_end:

table0:
	di proccess + BIOS_ADDRESS
	times 3 db 0
	db 0xf0
table0_end:


proccess:
	add pc, r0, .start - proccess

	.var:
		dl 0

	.start:
		add r1, r0, .var - proccess
	
		.loop:
			loal r2, r1, 0
			add r2, r2, 1
			stol r2, r1, 0
		add pc, r0, .loop - proccess





start:
	add r1, pc, table3 - $
	chtp r1

	add r1, pc, zero - $
	loalk pc, r1, 0

	add r1, r0, 1 | 4 | 8
	chst r1
	
	
chst r0
