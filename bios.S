add pc, pc, start - $

start:
	; adding sysinfo device MMIO
	add r10, r0, 0x100 ; MMU offset

	add r1, r0, 0x110 ; STARTADDR
	stoi r1, r10, 1

	add r1, r0, 11 ; SIZE
	stoi r1, r10, 5

	add r1, r0, 0 ; DEVICE ID (sysinfo)
	stos r1, r10, 9

	add r1, r0, 2 ; COMMAND (add)
	stob r1, r10, 0


	; read ram size

	add r10, r0, 0x110

	add r1, r0, 2 ; COMMAND (get_ram_size)
	stos r1, r10, 0

	loal r1, r10, 3 ; read RESPONSE

end:
	chst r0
