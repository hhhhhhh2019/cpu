#include "apic.h"
#include <motherboard.h>
#include <utils.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


#define BIOS_ADDRESS 0x10000

#define max(a,b) (a > b ? a : b)


Motherboard motherboard;

char* bios_name = NULL;
char* bios;
unsigned long bios_size;



void print_help();


int main(int argc, char** argv) {
	motherboard.ram.ram_size = 128 * 1024 * 1024; // 128 MiB
	motherboard.cpu.cores_count = 1;

	for (int i = 0; i < argc; i++) {
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
			print_help();
			return 0;
		}

		else if (strcmp(argv[i], "--ram") == 0 || strcmp(argv[i], "-m") == 0) {
			motherboard.ram.ram_size = size2int(argv[++i]);
		}

		else if (strcmp(argv[i], "--bios") == 0 || strcmp(argv[i], "-b") == 0) {
			bios_name = argv[++i];
		}

		else if (strcmp(argv[i], "--cores") == 0 || strcmp(argv[i], "-c") == 0) {
			motherboard.cpu.cores_count = atoi(argv[++i]);
		}
	}

	if (bios_name == NULL) {
		printf("No BIOS file name! --help to show help message\n");
		return 1;
	}


	FILE* f = fopen(bios_name, "rb");

	if (f == NULL) {
		perror("fopen");
		return errno;
	}

	fseek(f, 0, SEEK_END);
	bios_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	bios = malloc(bios_size);
	fread(bios, bios_size, 1, f);

	fclose(f);


	ram_init(&motherboard.ram, &motherboard);
	mmu_init(&motherboard.mmu, &motherboard, 100);
	cpu_init(&motherboard.cpu, &motherboard, motherboard.cpu.cores_count, 100);


	memcpy(motherboard.ram.ram + BIOS_ADDRESS, bios, bios_size);


	char cpu_enabled = 1;


	unsigned long max_hz = 0;

	for (int i = 0; i < motherboard.cpu.cores_count; i++)
		max_hz = max(max_hz, motherboard.cpu.cores[i].hz);

	max_hz = max(max_hz, motherboard.cpu.apic.hz);


	motherboard.cpu.cores[0].registersk[REG_PC] = BIOS_ADDRESS;
	motherboard.cpu.cores[0].registersk[REG_SP] = BIOS_ADDRESS;
	motherboard.cpu.cores[0].state = STATE_ENABLE;


	for (int tick = 0; cpu_enabled; tick++) {
		cpu_enabled = 0;

		for (int i = 0; i < motherboard.cpu.cores_count; i++) {
			cpu_enabled |= motherboard.cpu.cores[i].state & STATE_ENABLE;

			if (tick % (max_hz / motherboard.cpu.cores[i].hz) != 0)
				continue;

			print_registers(&motherboard.cpu.cores[0], 0);
			core_step(&motherboard.cpu.cores[0]);

			getc(stdin);
		}


		if (tick % (max_hz / motherboard.mmu.hz) == 0)
			mmu_step(&motherboard.mmu);

		if (tick % (max_hz / motherboard.cpu.apic.hz) == 0)
			apic_step(&motherboard.cpu.apic);
	}
}


void print_help() {
	
}
