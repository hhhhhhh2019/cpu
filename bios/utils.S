#ifndef UTILS_H
#define UTILS_H


; number: long, s: char*
itoa:
pushl r1
pushl r2
pushl r3

	loal r2, sp, 8 * 4 ; number
	or r1, r0, r0 ; reversed number

	; reverse number
	.rev_loop:
		mod r3, r2, 10
		mul r1, r1, 10
		add r1, r1, r3
		div r2, r2, 10
	add r0, r2, r0
	addne pc, pc, .rev_loop - $


	loal r2, sp, 8 * 4 + 8 ; s

	.loop:
		mod r3, r1, 10
		add r3, r3, '0'
		stob r3, r2, 0

		div r1, r1, 10
		add r2, r2, 1
	add r0, r1, r0
	addne pc, pc, .loop - $

popl r3
popl r2
popl r1
popl pc


#endif
