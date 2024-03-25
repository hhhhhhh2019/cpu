add pc, pc, start - $

#include "devices.S"

#include "vvmhc.S"
#include "uart.S"
#include "utils.S"

msg_init_complete:
	db "init complete\n", 0

devices_count:
	ds 0


; device id, disk id
disks:
	times 16 ds 0 db 0


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


init_devices:
	; r12 - device id
	; r3 - offset in devices
	; r4 - device type

	or r12, r0, r0

	.loop1:
		add r3, pc, devices - $
		mul r2, r12, 1 + 2 + 4 + 4
		add r3, r3, r2

		loas r4, r3, 1
		
		; add r0, r4, SYSINFO_TYPE
		; add r0, r4, TIMER_TYPE
		add r0, r4, VVMHC_TYPE
		adde pc, pc, .vvmhc - $
		; add r0, r4, UART_TYPE

		add pc, pc, .loop1_end - $

	.vvmhc:
		pushs r12
		add r1, pc, init_vvmhc - $
		call r1
		add sp, sp, 8
		add pc, pc, .loop1_end - $
	.loop1_end:
	add r12, r12, 1
	add r0, r11, r12
	addne pc, pc, .loop1 - $


	add r1, pc, msg_init_complete - $
	pushl r1
	add r1, r0, 3
	pushs r1
	add r1, pc, uart_puts - $
	call r1
	add sp, sp, 2 + 8


	add r1, pc, buffer - $
	pushl r1
	add r1, r0, 123456789
	pushl r1
	add r1, pc, itoa - $
	call r1
	add sp, sp, 8 + 8

	add r1, pc, buffer - $
	pushl r1
	add r1, r0, 3
	pushs r1
	add r1, pc, uart_puts - $
	call r1
	add sp, sp, 2 + 8

	
end:
chst r0
