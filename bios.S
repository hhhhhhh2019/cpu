add pc, pc, start - $


#include "MMIO.S"


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


	; add timer MMIO
	add r10, r0, MMU_MMIO_OFFSET

	add r1, r0, TIMER_MMIO_OFFSET
	stoi r1, r10, MMU_START_ADDR_OFFSET

	add r1, r0, 17
	stoi r1, r10, MMU_SIZE_OFFSET

	add r1, r0, 1
	stos r1, r10, MMU_DEVICE_ID_OFFSET

	add r1, r0, MMU_COMMAND_ADD
	stob r1, r10, MMU_COMMAND_OFFSET


	; add UART MMIO
	add r10, r0, MMU_MMIO_OFFSET

	add r1, r0, UART_MMIO_OFFSET
	stoi r1, r10, MMU_START_ADDR_OFFSET

	add r1, r0, 3
	stoi r1, r10, MMU_SIZE_OFFSET

	add r1, r0, 3
	stos r1, r10, MMU_DEVICE_ID_OFFSET

	add r1, r0, MMU_COMMAND_ADD
	stob r1, r10, MMU_COMMAND_OFFSET


	; uart read
	add r10, r0, UART_MMIO_OFFSET

	add r1, r0, UART_STATE_READ
	stob r1, r10, UART_STATE_OFFSET

	; wait input
	uart_loop1:
		loab r1, r10, UART_STATE_OFFSET
		and r1, r1, UART_STATE_IN_READY
		cmp r1, r0
	adde pc, pc, uart_loop1 - $

	loab r2, r10, UART_DATA_IN_OFFSET


	; uart print
	add r10, r0, UART_MMIO_OFFSET

	add r1, r0, r2
	stob r1, r10, UART_DATA_OUT_OFFSET

	add r1, r0, UART_STATE_WRITE
	stob r1, r10, UART_STATE_OFFSET
chst r0
