add pc, pc, start - $


#include "MMIO.S"

; #include "uart.S"
; #include "vvmhc.S"


devices_count:
	ds 0


buffer:
	times 512 db 0


start:
	; add sysinfo MMIO
	add r10, r0, MMU_MMIO_OFFSET
	
	add r1, r0, SYSINFO_MMIO_OFFSET
	stoi r1, r10, MMU_START_ADDR_OFFSET

	add r1, r0, 11
	stoi r1, r10, MMU_SIZE_OFFSET

	add r1, r0, 0
	stos r1, r10, MMU_DEVICE_ID_OFFSET

	add r1, r0, MMU_COMMAND_ADD
	stob r1, r10, MMU_COMMAND_OFFSET


	add r10, r0, SYSINFO_MMIO_OFFSET

	add r1, r0, SYSINFO_COMMAND_GET_DEVICE_COUNT
	stob r1, r10, SYSINFO_COMMAND_OFFSET

	loal r11, r10, SYSINFO_RESPONSE_OFFSET

	stos r11, pc, devices_count - $


find_devices:
	; r11 - devices count
	; r12 - id
	; r2  - type
	; r3  - offset
	; r4  - size
	or r12, r0, r0 ; mov r12, 0

	add r3, r0, 0x110

	.loop1:
		stos r12, r10, SYSINFO_ARGUMENT_OFFSET

		add r1, r0, SYSINFO_COMMAND_GET_DEVICE_TYPE
		stob r1, r10, SYSINFO_COMMAND_OFFSET

		loal r2, r10, SYSINFO_RESPONSE_OFFSET

		mul r5, r12, 1 + 2 + 4 + 4
		add r6, pc, devices - $
		add r5, r5, r6

		add r6, pc, devices_size - $
		mul r7, r12, 2
		add r6, r6, r7
		loas r4, r6, 0

		stob r12, r5, 0
		stos r2,  r5, 1
		stoi r4,  r5, 1 + 2
		stoi r3,  r5, 1 + 2 + 4

		add r3, r3, r4

		; round to 0x**0
		not r4, r3
		and r4, r4, 0b1111
		add r3, r3, r4
		add r3, r3, 1
	add r12, r12, 1
	add r0, r11, r12
	addne pc, pc, .loop1 - $


mount_devices:
	; r11 - devices count
	; r12 - id
	; r3  - offset
	; r4  - size
	
	or r12, r0, r0

	add r10, r0, MMU_MMIO_OFFSET

	add r1, r0, MMU_COMMAND_RESET
	stob r1, r10, MMU_COMMAND_OFFSET

	.loop1:
		mul r5, r12, 1 + 2 + 4 + 4
		add r6, pc, devices - $
		add r5, r5, r6

		loai r4, r5, 1 + 2
		loai r3, r5, 1 + 2 + 4

		stoi r3, r10, MMU_START_ADDR_OFFSET
		stoi r4, r10, MMU_SIZE_OFFSET
		stos r12, r10, MMU_DEVICE_ID_OFFSET

		add r1, r0, MMU_COMMAND_ADD
		stob r1, r10, MMU_COMMAND_OFFSET
	add r12, r12, 1
	add r0, r11, r12
	addne pc, pc, .loop1 - $

end:
chst r0
