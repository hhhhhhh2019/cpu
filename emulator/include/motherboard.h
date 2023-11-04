#ifndef MATHERBOARD_H
#define MATHERBOARD_H


#include <mmu.h>
#include <cpu.h>
#include <ram.h>


typedef struct Motherboard {
	MMU mmu;
	CPU cpu;
	RAM ram;
} Motherboard;


#endif
