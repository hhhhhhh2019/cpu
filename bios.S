add pc, pc, start - $

; #include "include.S"

#ifndef INT_OFFSET
	#define INT_OFFSET(x) x * 16
#endif


var:
	times 2 \
	    dl 0x1234,10 \
	    ds 12


start:
	add r1, r0, INT_OFFSET(3)

	.loop1:
		add r1, r1, 1
	add pc, pc, .loop1 - $
