#ifndef APIC_H
#define APIC_H


#include <utils.h>


typedef struct {
	void* motherboard;

	unsigned long hz;

	char core_id[256]; // номер ядра для каждого прерывания

	Queue* queues;
	unsigned char queues_count;
} APIC;


void apic_init(APIC*, void* motherboard, unsigned long hz, unsigned char queues_count);
void apic_step(APIC*);
void apic_push(APIC*, unsigned char id);
void apic_push_core(APIC*, unsigned char id, unsigned char core_id);


#endif
