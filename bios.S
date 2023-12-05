add pc, pc, start - $

start:
	add r1, r0, -2
	add r2, r0, -1

	cmp r1, r2

	adde pc, pc, equals - $
	addsl pc, pc, less - $
	addsg pc, pc, more - $

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


; add pc, pc, start - $
;
;
; int0:
; 	chst r0
; iret
;
;
; interrupts:
; 	dl int0
; interrupts_end:
;
;
; start:
; 	add r1, pc, interrupts - $
; 	add r2, pc, interrupts_end - $
; 	add r3, r0, 0
;
; 	loop1:
; 		loal r4, r1, 0
; 		add r4, pc, int0 - $
; 		stol r4, r3, 0
;
; 		add r1, r1, 8
; 		add r3, r3, 8
; 		cmp r1, r2
; 	addne pc, pc, loop1 - $
;
;
; 	int 0
; 	add r1, r0, 10
;
;
; end:
; 	chst r0
