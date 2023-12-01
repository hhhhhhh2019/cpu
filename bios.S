.offset 0x10000


add pc, pc, label1 - $

add r1, r0, 1

label1:

chst r0


; add pc, r0, start
;
; start:
; 	add r1, r0, 0
; 	add r2, r0, 3
;
; 	cmp r1, r2
;
; 	adde pc, r0, equals
; 	addl pc, r0, less
; 	addg pc, r0, more
;
; equals:
; 	add r3, r0, 1
; 	add pc, r0, end
;
; less:
; 	add r3, r0, 2
; 	add pc, r0, end
;
; more:
; 	add r3, r0, 3
; 	add pc, r0, end
;
; end:
; 	chst r0
