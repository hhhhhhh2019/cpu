#include <uart.h>
#include <motherboard.h>

#include <string.h>
#include <stdio.h>


void uart_init(UART* uart, void *motherboard, unsigned long hz) {
	uart->motherboard = motherboard;
	uart->hz = hz;
	uart->type_id = UART_TYPE_ID;

	memset(uart->registers, 0, sizeof(uart->registers));

	uart->next_char = 0;
	uart->have_char = 0;
}


void uart_step(UART* uart) {
	char *state    = (char*)(uart->registers + 0x00);
	char *data_in  = (char*)(uart->registers + 0x01);
	char *data_out = (char*)(uart->registers + 0x02);

	if (uart->next_char) {
		if (interactive_mode)
			printf("UART input: ");
		uart->c = getc(stdin);
		if (interactive_mode)
		 putc(uart->c, stdout);
		uart->next_char = 0;
		uart->have_char = 1;
	}

	if ((*state & UART_STATE_READ) != 0) {
		uart->next_char = 1;
		uart->have_char = 0;
		*state &= ~UART_STATE_READ;
	}

	if ((*state & UART_STATE_WRITE) != 0) {
		if (interactive_mode)
			printf("UART output: ");
		putc(*data_out, stdout);
		fflush(stdout);
		*state &= ~UART_STATE_WRITE;
	}

	*state &= ~UART_STATE_IN_READY;
	*state |= UART_STATE_IN_READY * uart->have_char;
	*data_in = uart->c;
}
