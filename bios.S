.offset 0x10000

add pc, pc, $

start:
	add r1, r0, 2
	add r2, r0, 3

	cmp r1, r2

	adde pc, pc, equals - $
	addl pc, pc, less - $
	addg pc, pc, more - $

equals:
	add r3, r0, 1
	add pc, pc, end - $

less:
	add r3, r0, 2
	add pc, pc, end - $

more:
	add r3, r0, 3
	add pc, pc, end - $

end:
	chst r0
