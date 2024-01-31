add pc, pc, start - $


buffer:
	times 512 db 1


start:
	add r10, r0, 0x100 ; MMU offset

	add r1, r0, 0x110 ; startaddr
	stoi r1, r10, 1

	add r1, r0, 31 ; size
	stoi r1, r10, 5

	add r1, r0, 2 ; device id (at the moment vvmhc is third device)
	stos r1, r10, 9

	add r1, r0, 2 ; command (add)
	stob r1, r10, 0


	; init vvmhc
	
	add r10, r0, 0x110 ; VVMHC offset

	add r1, r0, 1 ; state
	stob r1, r10, 0


	start_loop1:
		loab r1, r10, 0
	cmp r1, r0
	adde pc, pc, start_loop1 - $


	; write into vvmhc

	add r1, r0, 1 ; block count
	stos r1, r10, 0x02

	add r1, r0, 0 ; disk addr
	stol r1, r10, 0x04

	add r1, pc, buffer - $ ; ram addr
	stol r1, r10, 0x0c

	add r1, r0, 0 ; id
	stob r1, r10, 0x1c

	add r1, r0, 0 ; tp
	stol r1, r10, 0x1d

	add r1, r0, 4 ; command (write)
	stob r1, r10, 1

	; wait end
	add r2, r0, 1

	start_loop2:
		loab r1, r10, 0
	cmp r1, r2
	addne pc, pc, start_loop2 - $



	; read from vvmhc

	add r1, r0, 1 ; block count
	stos r1, r10, 0x02

	add r1, r0, 0x200 ; disk addr
	stol r1, r10, 0x04

	add r1, pc, buffer - $ ; ram addr
	stol r1, r10, 0x0c

	add r1, r0, 3 ; command (read)
	stob r1, r10, 1

	add r2, r0, 1

	start_loop3:
		loab r1, r10, 0
	cmp r1, r2
	addne pc, pc, start_loop3 - $


	add r1, pc, buffer - $
	loal r1, r1, 0

chst r0
