add pc, pc, start - $


int0:
	add r6, r0, 0x12345
	chst r0
iret


interrupts:
dl int0
interrupts_end:


start:
	; update interrupts table

	add r1, pc, interrupts - $ ; start pointer
	add r2, pc, interrupts_end - $ ; end pointer
	add r3, r0, 0
	sub r5, pc, $

	start_loop1:
		; cmp r1, r2
		add r0, r1, r2
		adde pc, pc, start_loop1_end - $

		loal r4, r1, 0
		add r4, r4, r5
		stol r4, r3, 0

		add r1, r1, 8
		add r3, r3, 8

		add pc, pc, start_loop1 - $
	start_loop1_end:


	add r1, r0, 3 ; ENABLE | INTERRUPTS
	chst r1


	; add timer MMIO

	add r10, r0, 0x100 ; MMU MMIO offset

	add r1, r0, 0x110 ; start addr
	stol r1, r10, 0x01

	add r1, r0, 17 ; size
	stoi r1, r10, 0x05

	add r1, r0, 1 ; device id
	stos r1, r10, 0x09

	add r1, r0, 2 ; command (add)
	stob r1, r10, 0x00


	; delay(10)

	add r10, r0, 0x110 ; Timer MMIO offset

	add r1, r0, 0 ; interrups
	stob r1, r10, 0x08

	add r1, r0, 10 ; delay
	stol r1, r10, 0x00

	sub pc, pc, 0 ; inf. loop
; chst r0
