#include <uart.h>
#include <motherboard.h>

#include <string.h>
#include <stdio.h>


void uart_init(UART* uart, void *motherboard, unsigned long hz) {
	uart->motherboard = motherboard;
	uart->hz = hz;
	uart->type_id = UART_TYPE_ID;

	memset(uart->registers, 0, sizeof(uart->registers));

	uart->ready = 0;
}


void uart_step(UART* uart) {
	if (interactive_mode)
		printf("UART input: ");

	char *state    = (char*)(uart->registers + 0x00);
	char *data_in  = (char*)(uart->registers + 0x01);
	char *data_out = (char*)(uart->registers + 0x02);

	if (uart->ready) {
		*data_in = getc(stdin);
		uart->ready = 0;
		*state |= UART_STATE_IN_READY;
	}

	if ((*state & UART_STATE_READ) != 0) {
		uart->ready = 1;
		*state &= ~UART_STATE_READ;
	}

	if ((*state & UART_STATE_WRITE) != 0) {
		if (interactive_mode)
			printf("UART output: ");
		putc(*data_out, stdout);
		*state &= ~UART_STATE_WRITE;
	}
}
