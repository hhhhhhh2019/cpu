#ifndef UART_H
#define UART_H


#include "MMIO.S"


uart_getc:
	
popl pc


; ch: byte
uart_putc:
pushl r1
pushl r10

	loai r10, pc, devices_params.uart - $ + 1

	loab r1, sp, 8 + 8 + 8

	stob r1, r10, UART_DATA_OUT_OFFSET
	
	add r1, r0, UART_STATE_WRITE
	stob r1, r10, UART_STATE_OFFSET

popl r10
popl r1
popl pc


#endif
