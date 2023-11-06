#include <apic.h>
#include <motherboard.h>
#include <utils.h>

#include <stdlib.h>


void apic_init(APIC* apic, void* motherboard, unsigned long hz, unsigned char queues_count) {
	apic->motherboard = motherboard;
	apic->hz = hz;
	
	for (int i = 0; i < 256; i++)
		apic->core_id[i] = 0;

	apic->queues_count = queues_count;
	apic->queues = malloc(sizeof(Queue) * queues_count);

	for (int i = 0; i < queues_count; i++)
		queue_init(&apic->queues[i], 64);
}


void apic_step(APIC* apic) {
	Motherboard* motherboard = apic->motherboard;

	for (int i = 0; i < apic->queues_count; i++) {
		if (motherboard->cpu.cores[i].is_interrupt)
			continue;

		if (queue_can_pop(&apic->queues[i]) == 0)
			continue;

		core_int(&motherboard->cpu.cores[i], queue_pop(&apic->queues[i]));
	}
}


void apic_push(APIC* apic, unsigned char id) {
	queue_push(&apic->queues[apic->core_id[id]], id);
}


void apic_push_core(APIC* apic, unsigned char id, unsigned char core_id) {
	queue_push(&apic->queues[apic->core_id[core_id]], id);
}
