add pc, r0, start

start:
	add r1, r0, 1 + 2
	add r2, r0, 6 + 3 - 2 * (3 + 4 - 7)

	cmp r1, r2

	adde pc, equals - $

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
