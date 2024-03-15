add pc, pc, start - $


#include "MMIO.S"

#include "uart.S"


devices_count:
	ds 0


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
	or r12, r0, r0 ; mov r12, 0

	.loop1:
		stos r12, r10, SYSINFO_ARGUMENT_OFFSET

		add r1, r0, SYSINFO_COMMAND_GET_DEVICE_TYPE
		stob r1, r10, SYSINFO_COMMAND_OFFSET

		loal r2, r10, SYSINFO_RESPONSE_OFFSET


		add r0, r2, SYSINFO_TYPE
		adde pc, pc, .sysinfo - $

		add r0, r2, TIMER_TYPE
		adde pc, pc, .timer - $

		add r0, r2, VVMHC_TYPE
		adde pc, pc, .vvmhc - $

		add r0, r2, UART_TYPE
		adde pc, pc, .uart - $

		add pc, pc, .loop1_end - $

		.sysinfo:
			add r1, pc, devices_params.sysinfo - $
			add pc, pc, .loop1_write - $
		.timer:
			add r1, pc, devices_params.timer - $
			add pc, pc, .loop1_write - $
		.vvmhc:
			add r1, pc, devices_params.vvmhc - $
			add pc, pc, .loop1_write - $
		.uart:
			add r1, pc, devices_params.uart - $
			add pc, pc, .loop1_write - $

	.loop1_write:
		stob r12, r1, 0
	.loop1_end:
	add r12, r12, 1
	add r0, r11, r12
	addne pc, pc, .loop1 - $


mount_devices:
	; reset MMU

	add r10, r0, MMU_MMIO_OFFSET

	add r1, r0, MMU_COMMAND_RESET
	stob r1, r0, MMU_COMMAND_OFFSET

	or r12, r0, r0


	add r9, pc, devices_params - $
	
	.loop1:
		loab r1, r9, 0
		stob r1, r10, MMU_DEVICE_ID_OFFSET

		loai r1, r9, 1
		stoi r1, r10, MMU_START_ADDR_OFFSET

		loai r1, r9, 1 + 4
		stoi r1, r10, MMU_SIZE_OFFSET

		add r1, r0, MMU_COMMAND_ADD
		stob r1, r10, MMU_COMMAND_OFFSET

		add r9, r9, 1 + 4 + 4
	add r12, r12, 1
	add r0, r11, r12
	addne pc, pc, .loop1 - $
end:
chst r0
