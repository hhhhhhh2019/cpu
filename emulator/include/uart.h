#ifndef UART_H
#define UART_H


#define UART_TYPE_ID 4

#define VVMHC_STATE_IN_READY 1
#define VVMHC_STATE_READ     2
#define VVMHC_STATE_WRITE    4


typedef struct {
	void* motherboard;
	unsigned int type_id;
	unsigned long hz;
	char registers[3];
} UART;


void uart_init(UART*, void* motherboard, unsigned long hz);
void uart_step(UART*);


#endif
