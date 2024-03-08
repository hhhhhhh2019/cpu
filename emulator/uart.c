#include <uart.h>

#include <string.h>


void uart_init(UART* uart, void *motherboard, unsigned long hz) {
	uart->motherboard = motherboard;
	uart->hz = hz;
	uart->type_id = UART_TYPE_ID;

	memset(uart->registers, 0, sizeof(uart->registers));
}


void uart_step(UART* uart) {
	
}
