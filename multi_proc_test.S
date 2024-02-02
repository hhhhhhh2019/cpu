add pc, pc, start - $


current_proccess:
	dl 0


registers:
	times 17 * 2 dl 0


proccess1:
	add r1, r0, 0

	proccess1_loop1:
    add r1, r1, 1
  add pc, pc, proccess1_loop1 - $


proccess2:
	add r2, r0, 0

	proccess2_loop1:
    add r2, r2, 1
  add pc, pc, proccess2_loop1 - $


int0:
	add r10, pc, current_proccess - $
	loal r10, r10, 0

	mul r11, r10, 17 * 8

	add r1, pc, registers - $
	add r1, r1, r11

	stolk r1,  r1, 8 * 0
	stolk r2,  r1, 8 * 1
	stolk r3,  r1, 8 * 2
	stolk r4,  r1, 8 * 3
	stolk r5,  r1, 8 * 4
	stolk r6,  r1, 8 * 5
	stolk r7,  r1, 8 * 6
	stolk r8,  r1, 8 * 7
	stolk r9,  r1, 8 * 8
	stolk r10, r1, 8 * 9
	stolk r11, r1, 8 * 10
	stolk r12, r1, 8 * 11
	stolk r13, r1, 8 * 12
	stolk r14, r1, 8 * 13
	stolk r15, r1, 8 * 14
	lotp   r2
	stol   r2, r1, 8 * 15
	loflag r2
	stol   r2, r1, 8 * 16


	xor r9, r10, 1

	add r10, pc, current_proccess - $
	stol r9, r10, 0

	mul r11, r9, 17 * 8

	add r1, pc, registers - $
	add r1, r1, r11

	loalk r1,  r1, 8 * 0
	loalk r2,  r1, 8 * 1
	loalk r3,  r1, 8 * 2
	loalk r4,  r1, 8 * 3
	loalk r5,  r1, 8 * 4
	loalk r6,  r1, 8 * 5
	loalk r7,  r1, 8 * 6
	loalk r8,  r1, 8 * 7
	loalk r9,  r1, 8 * 8
	loalk r10, r1, 8 * 9
	loalk r11, r1, 8 * 10
	loalk r12, r1, 8 * 11
	loalk r13, r1, 8 * 12
	loalk r14, r1, 8 * 13
	loalk r15, r1, 8 * 14
	loal   r2, r1, 8 * 15
	chtp   r2
	loal   r2, r1, 8 * 16
	chflag r2


	
	add r10, r0, 0x110 ; Timer MMIO offset
	
	add r1, r0, 0 ; interrupt
	stob r1, r10, 0x08

	add r1, r0, 100
	stol r1, r10, 0x00
iret


interrupts:
	dl int0
interrupts_end:



start:
	add r1, pc, interrupts - $
	add r2, pc, interrupts_end - $
	add r3, r0, 0
	sub r4, pc, $

	start_loop1:
		cmp r1, r2
		adde pc, pc, start_loop1_end - $
		
		loal r5, r1, 0
		add r5, r5, r4
		stol r5, r3, 0

		add r1, r1, 8
		add r3, r3, 8

		add pc, pc, start_loop1 - $
	start_loop1_end:



	add r10, r0, 0x100 ; MMU MMIO offset

	add r1, r0, 0x110 ; start addr
	stoi r1, r10, 0x01

	add r1, r0, 17 ; size
	stoi r1, r10, 0x05

	add r1, r0, 1 ; id
	stos r1, r10, 0x09

	add r1, r0, 2 ; command (add)
	stob r1, r10, 0x00


	add r1, pc, registers - $

	add r2, pc, proccess1 - $
	stol r2, r1, 8 * 17 * 0 + 8 * 14

	add r2, pc, proccess2 - $
	stol r2, r1, 8 * 17 * 1 + 8 * 14


	stol r0, r1, 8 * 17 * 0 + 8 * 15
	stol r0, r1, 8 * 17 * 0 + 8 * 16

	stol r0, r1, 8 * 17 * 1 + 8 * 15
	stol r0, r1, 8 * 17 * 1 + 8 * 16


	loalk r1,  r1, 8 * 0
	loalk r2,  r1, 8 * 1
	loalk r3,  r1, 8 * 2
	loalk r4,  r1, 8 * 3
	loalk r5,  r1, 8 * 4
	loalk r6,  r1, 8 * 5
	loalk r7,  r1, 8 * 6
	loalk r8,  r1, 8 * 7
	loalk r9,  r1, 8 * 8
	loalk r10, r1, 8 * 9
	loalk r11, r1, 8 * 10
	loalk r12, r1, 8 * 11
	loalk r13, r1, 8 * 12
	loalk r14, r1, 8 * 13
	loalk r15, r1, 8 * 14
	chtp r0
	chflag r0


	add r10, r0, 0x110 ; Timer MMIO offset
	
	add r1, r0, 0 ; interrupt
	stob r1, r10, 0x08

	add r1, r0, 100
	stol r1, r10, 0x00


	add r1, r0, 1 | 2 | 8
	chst r1
chst r0
