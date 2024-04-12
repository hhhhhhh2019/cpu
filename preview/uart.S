#include "devices.S"


; id: short
uart_getc:
pushl r10
	
	add r10, pc, devices - $
	loas r1, sp, 8 + 8
	mul r1, r1, 1 + 2 + 4 + 4
	add r10, r10, r1

	loai r10, r10, 1 + 2 + 4 ; device MMIO offset

	add r1, r0, UART_STATE_READ
	stob r1, r10, UART_STATE_OFFSET

	; wait input
	.loop:
		loab r1, r10, UART_STATE_OFFSET
		and r1, r1, UART_STATE_IN_READY
		add r0, r1, r0
	adde pc, pc, .loop - $

	loab r1, r10, UART_DATA_IN_OFFSET
	
popl r10
popl pc


; id: short, ch: char
uart_putc:
pushl r10
pushl r1
	
	add r10, pc, devices - $
	loas r1, sp, 8 + 8 + 8
	mul r1, r1, 1 + 2 + 4 + 4
	add r10, r10, r1

	loai r10, r10, 1 + 2 + 4 ; device MMIO offset

	loab r1, sp, 8 + 8 + 8 + 2 ; ch
	stob r1, r10, UART_DATA_OUT_OFFSET

	add r1, r0, UART_STATE_WRITE
	stob r1, r10, UART_STATE_OFFSET

popl r1
popl r10
popl pc


; id: short, s: char*
uart_puts:
pushl r10
pushl r1
pushl r2

	add r10, pc, devices - $
	loas r1, sp, 8 + 8 + 8 + 8
	mul r1, r1, 1 + 2 + 4 + 4
	add r10, r10, r1

	loai r10, r10, 1 + 2 + 4 ; device MMIO offset

	loal r2, sp, 8 + 8 + 8 + 8 + 2 ; s

	.loop:
		loab r1, r2, 0
		add r0, r1, r0
		adde pc, pc, .loop_end - $

		stob r1, r10, UART_DATA_OUT_OFFSET

		add r1, r0, UART_STATE_WRITE
		stob r1, r10, UART_STATE_OFFSET

		add r2, r2, 1
	add pc, pc, .loop - $

	.loop_end:

popl r2
popl r1
popl r10
popl pc



; id: short, s: char*
uart_gets:
pushl r10
pushl r1
pushl r2

	add r10, pc, devices - $
	loas r1, sp, 8 + 8 + 8 + 8 ; id
	mul r1, r1, 1 + 2 + 4 + 4
	add r10, r10, r1

	loai r10, r10, 1 + 2 + 4 ; device MMIO offset

	loal r2, sp, 8 + 8 + 8 + 8 + 2 ; s

	.loop:
		loas r1, sp, 8 + 8 + 8 + 8 ; id
		pushs r1
		add r1, pc, uart_getc - $
		call r1
		add sp, sp, 2
	
		add r0, r1, 3 ; enter
		adde pc, pc, .loop_end - $

		stob r1, r2, 0

		pushb r1
		add r1, r0, 3
		pushs r1
		add r1, pc, uart_putc - $
		call r1
		add sp, sp, 1 + 2


		add r2, r2, 1
	add pc, pc, .loop - $

	.loop_end:
		; add r2, r2, 1
		or r1, r0,r0
		stob r1, r2, 0

popl r2
popl r1
popl r10
popl pc
