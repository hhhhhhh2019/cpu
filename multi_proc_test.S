add pc, pc, start - $


current_proccess:
dl 0


registers:
times 17 * 2 dl 0


int0:
	add r1, pc, current_proccess - $
	loal r1, r1, 0

	mul r1, r1, 17

	add r2, pc, registers - $
	add r2, r2, r1


	stolk r1,  r2, 0
	stolk r2,  r2, 8
	stolk r3,  r2, 16
	stolk r4,  r2, 24
	stolk r5,  r2, 32
	stolk r6,  r2, 40
	stolk r7,  r2, 48
	stolk r8,  r2, 56
	stolk r9,  r2, 64
	stolk r10, r2, 72
	stolk r11, r2, 80
	stolk r12, r2, 88
	stolk r13, r2, 96
	stolk r14, r2, 104
	stolk r15, r2, 112
	lotp r3
	stol r3, r2, 120
	loflag r3
	stol r3, r2, 128


	add r2, pc, current_proccess - $
	loal r1, r2, 0
	xor r1, r1, 1
	stol r1, r2, 0


	mul r1, r1, 17

	add r2, pc, registers - $
	add r2, r2, r1

	
	loalk r1,  r2, 0
	loalk r2,  r2, 8
	loalk r3,  r2, 16
	loalk r4,  r2, 24
	loalk r5,  r2, 32
	loalk r6,  r2, 40
	loalk r7,  r2, 48
	loalk r8,  r2, 56
	loalk r9,  r2, 64
	loalk r10, r2, 72
	loalk r11, r2, 80
	loalk r12, r2, 88
	loalk r13, r2, 96
	loalk r14, r2, 104
	loalk r15, r2, 112
	loal r3, r2, 120
	chtp r3
	loal r3, r2, 128
	chflag r3



	add r10, r0, 0x110 ; timer MMIO offset
	
	add r1, r0, 0 ; interrupt
	stob r1, r10, 0x08

	add r1, r0, 10 ; delay
	stol r1, r10, 0x00
iret


interrupts:
dl int0
interrupts_end:



proccess1:
	add r1, r0, 0

	proccess1_loop1:
    add r1, r1, 1
  add pc, pc, proccess1_loop1 - $


proccess2:
	add r2, r0, 0

	proccess2_loop1:
    add r2, r1, 1
  add pc, pc, proccess2_loop1 - $


start:
	; init interrupts

	add r2, pc, interrupts - $
	add r3, pc, interrupts_end - $
	add r4, r0, 0
	sub r5, pc, $

	start_loop1:
		cmp r2, r3
		adde pc, pc, start_loop1_end - $

		loal r1, r2, 0
		add r1, r1, r5
		stol r1, r4, 0

		add r2, r2, 8
		add r4, r4, 8
	start_loop1_end:


	; мне лень включать paging, обойдёмся без него
	add r1, r0, 3 ; ENABLE | INTERRUPTS
	chst r1


	; add timer MMIO

	add r10, r0, 0x100 ; MMU MMIO offset

	add r1, r0, 0x110 ; start addr
	stoi r1, r10, 0x01

	add r1, r0, 17 ; size
	stoi r1, r10, 0x05

	add r1, r0, 1 ; device id
	stob r1, r10, 0x09

	add r1, r0, 2 ; command (add)
	stob r1, r10, 0x00


	add r2, pc, proccess1 - $
	add r3, pc, proccess2 - $

	add r1, pc, registers - $

	stol r2, r1, 112 + 17 * 8 * 0
	stol r3, r1, 112 + 17 * 8 * 1


	loalk r1,  r1, 0
	loalk r2,  r1, 8
	loalk r3,  r1, 16
	loalk r4,  r1, 24
	loalk r5,  r1, 32
	loalk r6,  r1, 40
	loalk r7,  r1, 48
	loalk r8,  r1, 56
	loalk r9,  r1, 64
	loalk r10, r1, 72
	loalk r11, r1, 80
	loalk r12, r1, 88
	loalk r13, r1, 96
	loalk r14, r1, 104
	loalk r15, r1, 112
	chtp r0
	chflag r0


	int 0

chst r0
