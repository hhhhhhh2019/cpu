#include "apic.h"
#include "motherboard.h"
#include <timer.h>


void timer_init(Timer* timer, void *motherboard, unsigned long hz) {
	timer->type_id = TIMER_TYPE_ID;
	timer->motherboard = motherboard;
	timer->hz = hz;
}


void timer_step(Timer* timer) {
	unsigned long* delay  =  (unsigned long*)(timer->registers + 0);
	unsigned char  int_id = *(unsigned char*)(timer->registers + 8);
	unsigned long* hz     =  (unsigned long*)(timer->registers + 9);

	*hz = timer->hz;

	if (*delay == 0)
		return;

	Motherboard* motherboard = timer->motherboard;

	if (--(*delay) == 0)
		apic_push(&motherboard->cpu.apic, int_id);
}
