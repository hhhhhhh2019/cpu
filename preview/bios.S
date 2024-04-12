add pc, pc, start - $

#include "devices.S"

#include "uart.S"
#include "utils.S"


msg_init_complete:
	db "init complete\n", 0


msg_arg1:
	db "\nаргумент 1: ", 0

msg_arg2:
	db "\nаргумент 2: ", 0

msg_op:
	db "\nоперация: ", 0

msg_err:
	db "\nнеизвестная операция!\n", 0

msg_result:
	db "\nрезультат: ", 0


devices_count:
	ds 0


arg1:
	dl 0

arg2:
	dl 0


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


	add r1, pc, msg_init_complete - $
	pushl r1
	add r1, r0, 3
	pushs r1
	add r1, pc, uart_puts - $
	call r1
	add sp, sp, 2 + 8


	.loop:
		add r1, pc, msg_arg1 - $
		pushl r1
		add r1, r0, 3
		pushs r1
		add r1, pc, uart_puts - $
		call r1
		add sp, sp, 2 + 8

		add r1, pc, buffer - $
		pushl r1
		add r1, r0, 3
		pushs r1
		add r1, pc, uart_gets - $
		call r1
		add sp, sp, 2 + 8

		add r1, pc, buffer - $
		pushl r1
		add r1, pc, atol - $
		call r1
		add sp, sp, 8

		stol r1, pc, arg1 - $



		add r1, pc, msg_arg2 - $
		pushl r1
		add r1, r0, 3
		pushs r1
		add r1, pc, uart_puts - $
		call r1
		add sp, sp, 2 + 8

		add r1, pc, buffer - $
		pushl r1
		add r1, r0, 3
		pushs r1
		add r1, pc, uart_gets - $
		call r1
		add sp, sp, 2 + 8

		add r1, pc, buffer - $
		pushl r1
		add r1, pc, atol - $
		call r1
		add sp, sp, 8

		stol r1, pc, arg2 - $


		add r1, pc, msg_op - $
		pushl r1
		add r1, r0, 3
		pushs r1
		add r1, pc, uart_puts - $
		call r1
		add sp, sp, 2 + 8

		add r1, r0, 3
		pushs r1
		add r1, pc, uart_getc - $
		call r1
		add sp, sp, 2

		pushb r1
		add r2, r0, 3
		pushs r2
		add r2, pc, uart_putc - $
		call r2
		add sp, sp, 2 + 1

		add r0, r1, '+'
		adde pc, pc, .op_sum - $
		add r0, r1, '-'
		adde pc, pc, .op_sub - $
		add r0, r1, '*'
		adde pc, pc, .op_mul - $
		add r0, r1, '/'
		adde pc, pc, .op_div - $
		add r0, r1, '&'
		adde pc, pc, .op_and - $
		add r0, r1, '|'
		adde pc, pc, .op_or - $
		add r0, r1, '^'
		adde pc, pc, .op_xor - $

		add r1, pc, msg_err - $
		pushl r1
		add r1, r0, 3
		pushs r1
		add r1, pc, uart_puts - $
		call r1
		add sp, sp, 2 + 8

		add pc, pc, .loop - $


		.op_sum:
			loal r1, pc, arg1 - $
			loal r2, pc, arg2 - $
			add r1, r1, r2
			add pc, pc, .print_result - $
		.op_sub:
			loal r1, pc, arg1 - $
			loal r2, pc, arg2 - $
			sub r1, r1, r2
			add pc, pc, .print_result - $
		.op_mul:
			loal r1, pc, arg1 - $
			loal r2, pc, arg2 - $
			mul r1, r1, r2
			add pc, pc, .print_result - $
		.op_div:
			loal r1, pc, arg1 - $
			loal r2, pc, arg2 - $
			div r1, r1, r2
			add pc, pc, .print_result - $
		.op_and:
			loal r1, pc, arg1 - $
			loal r2, pc, arg2 - $
			and r1, r1, r2
			add pc, pc, .print_result - $
		.op_or:
			loal r1, pc, arg1 - $
			loal r2, pc, arg2 - $
			or r1, r1, r2
			add pc, pc, .print_result - $
		.op_xor:
			loal r1, pc, arg1 - $
			loal r2, pc, arg2 - $
			xor r1, r1, r2

		.print_result:

		add r2, pc, buffer - $
		pushl r2
		pushl r1
		add r1, pc, ltoa - $
		call r1
		add sp, sp, 8 * 2


		add r1, pc, msg_result - $
		pushl r1
		add r1, r0, 3
		pushs r1
		add r1, pc, uart_puts - $
		call r1
		add sp, sp, 2 + 8

		add r1, pc, buffer - $
		pushl r1
		add r1, r0, 3
		pushs r1
		add r1, pc, uart_puts - $
		call r1
		add sp, sp, 2 + 8

		add pc, pc, .loop - $

end:
chst r0
