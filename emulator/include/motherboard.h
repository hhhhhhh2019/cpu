#ifndef MATHERBOARD_H
#define MATHERBOARD_H


#include <mmu.h>
#include <cpu.h>
#include <ram.h>


typedef struct {
	MMU mmu;
	CPU cpu;
	RAM ram;

	void** devices;
	unsigned int devices_count;
} Motherboard;


void device_step(void*);


#endif
