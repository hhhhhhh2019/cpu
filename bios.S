add pc, pc, start - $


inta:


devices_count:
	dl 0

devices:
	times 16 db 0


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


	; read devices count

	add r10, r0, 0x110 ; sysinfo MMIO offset

	add r1, r0, 1 ; get_device_count
	stob r1, r10, 0

	loal r9, r10, 3 ; response

	stol r9, pc, devices_count - $


	; for (int i = 1; i < devices_count; i++)
	;    devices[i - 1] = device_type(i);

	add r1, r0, 1
	add r2, r0, 3 ; get_device_type
	add r5, pc, devices - $

	start_loop1:
		cmp r1, r9
		adde pc, pc, start_loop1_end - $

		stob r1, r10, 1 ; argument
		stob r2, r10, 0 ; command

		loal r4, r10, 3 ; response
		stol r4, r5, 0 ; *devices = r4

		add r5, r5, 8 ; devices += 8
		add r1, r1, 1 ; i++

		add pc, pc, start_loop1 - $
	start_loop1_end:


end:
	chst r0
