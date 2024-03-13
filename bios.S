add pc, pc, start - $


#include "MMIO.S"

; id, size, offset
devices_params:
.sysinfo:
	db 0
	di SYSINFO_MMIO_OFFSET
	di 11
.timer:
	db 1
	di TIMER_MMIO_OFFSET
	di 11
.uart:
	db 0
	di UART_MMIO_OFFSET
	di 3
.vvmhc:
	db 0
	di VVMHC_MMIO_OFFSET
	di 37


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


find_devices:
	add r12, r0, 0

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
		stob r2, r1, 0
	.loop1_end:
	add r12, r12, 1
	add r0, r11, r12
	addne pc, pc, .loop1 - $


	add r1, pc, devices_params.sysinfo - $
	loab r2, r1, 0
	add r1, pc, devices_params.timer - $
	loab r3, r1, 0
	add r1, pc, devices_params.vvmhc - $
	loab r4, r1, 0
	add r1, pc, devices_params.uart - $
	loab r5, r1, 0
end:
chst r0
