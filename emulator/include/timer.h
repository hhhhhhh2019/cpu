#ifndef TIMER_H
#define TIMER_H


#define TIMER_TYPE_ID 2


typedef struct {
	void* motherboard;
	unsigned int type_id;
	unsigned long hz;
	char registers[17];
} Timer;


void timer_init(Timer*, void* motherboard, unsigned long hz);
void timer_step(Timer*);


#endif
