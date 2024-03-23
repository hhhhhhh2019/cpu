add pc, pc, start - $


#define MMU_MMIO_OFFSET       0x100

#define MMU_COMMAND_OFFSET    0x0
#define MMU_START_ADDR_OFFSET 0x1
#define MMU_SIZE_OFFSET       0x5
#define MMU_DEVICE_ID_OFFSET  0x9

#define MMU_COMMAND_RESET 0x1
#define MMU_COMMAND_ADD   0x2


#define SYSINFO_MMIO_OFFSET    0x110

#define SYSINFO_COMMAND_OFFSET 0x0
#define SYSINFO_ARGUMENT       0x1
#define SYSINFO_RESPONSE       0x3

#define SYSINFO_COMMAND_GET_DEVICE_COUNT 0x1
#define SYSINFO_COMMAND_GET_RAM_SIZE     0x2
#define SYSINFO_COMMAND_GET_DEVICE_TYPE  0x3


#define TIMER_MMIO_OFFSET      0x120

#define TIMER_DELAY_OFFSET     0x0
#define TIMER_INTERRUPT_OFFSET 0x8
#define TIMER_HZ_OFFSET        0x9


#define VVMHC_TYPE 3

#define VVMHC_MMIO_OFFSET        0x140

#define VVMHC_STATE_OFFSET       0x00
#define VVMHC_COMMAND_OFFSET     0x01
#define VVMHC_BLOCK_COUNT_OFFSET 0x02
#define VVMHC_STARTADDR_OFFSET   0x04
#define VVMHC_ADDRESS_OFFSET     0x0c
#define VVMHC_RESPONSE_OFFSET    0x14
#define VVMHC_ID_OFFSET          0x1c
#define VVMHC_TP_OFFSET          0x1d

#define VVMHC_STATE_INIT 0b01
#define VVMHC_STATE_BUSY 0b10

#define VVMHC_COMMAND_GET_CONNECTED 0x1
#define VVMHC_COMMAND_GET_SIZE      0x2
#define VVMHC_COMMAND_READ          0x3
#define VVMHC_COMMAND_WRITE         0x4


#define UART_TYPE 4

#define UART_MMIO_OFFSET 0x170

#define UART_STATE_OFFSET    0x0
#define UART_DATA_IN_OFFSET  0x1
#define UART_DATA_OUT_OFFSET 0x2

#define UART_STATE_IN_READY 0b001
#define UART_STATE_READ     0b010
#define UART_STATE_WRITE    0b100


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


	loop1:
		add r10, r0, UART_MMIO_OFFSET

		add r1, r0, UART_STATE_READ
		stob r1, r10, UART_STATE_OFFSET

		; wait input
		uart_loop1:
			loab r1, r10, UART_STATE_OFFSET
			and r1, r1, UART_STATE_IN_READY
			add r0, r1, r0
		adde pc, pc, uart_loop1 - $

		loab r2, r10, UART_DATA_IN_OFFSET

		add r0, r2, 3
		adde pc, pc, end - $


		add r1, r0, r2
		stob r1, r10, UART_DATA_OUT_OFFSET

		add r1, r0, UART_STATE_WRITE
		stob r1, r10, UART_STATE_OFFSET
	add pc, pc, loop1 - $

end:
chst r0
