#include <motherboard.h>
#include <utils.h>

#include <sysinfo.h>
#include <vvmhc.h>

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


	motherboard.devices = realloc(motherboard.devices, sizeof(void*) * (++motherboard.devices_count));
	motherboard.devices[motherboard.devices_count-1] = malloc(sizeof(Sysinfo));

	sysinfo_init(motherboard.devices[motherboard.devices_count-1], &motherboard, 100);


	motherboard.devices = realloc(motherboard.devices, sizeof(void*) * (++motherboard.devices_count));
	motherboard.devices[motherboard.devices_count-1] = malloc(sizeof(VVMHC));

	vvmhc_init(motherboard.devices[motherboard.devices_count-1], &motherboard, 100);

	vvmhc_add_disk(motherboard.devices[motherboard.devices_count-1], "disk.img", 0);

	char cpu_enabled = 1;


	unsigned long max_hz = 0;

	for (int i = 0; i < motherboard.cpu.cores_count; i++)
		max_hz = max(max_hz, motherboard.cpu.cores[i].hz);

	for (int i = 0; i < motherboard.devices_count; i++) {
		void* d = motherboard.devices[i];

		max_hz = max(max_hz, *(unsigned long*)(d + sizeof(void*) + sizeof(int) * 2)); // умножаю на 2, потому что выравнивание
	}

	max_hz = max(max_hz, motherboard.cpu.apic.hz);


	motherboard.cpu.cores[0].registersk[REG_PC] = BIOS_ADDRESS;
	motherboard.cpu.cores[0].registersk[REG_SP] = BIOS_ADDRESS;
	motherboard.cpu.cores[0].state = STATE_ENABLE;

	for (unsigned long tick = 0; cpu_enabled; tick++) {
		cpu_enabled = 0;

		printf("step tick: %lu\n", tick);

		for (int i = 0; i < motherboard.cpu.cores_count; i++) {
			cpu_enabled |= motherboard.cpu.cores[i].state & STATE_ENABLE;

			if (tick % (max_hz / motherboard.cpu.cores[i].hz) != 0)
				continue;

			print_registers(&motherboard.cpu.cores[0], 0);
		}

		getc(stdin);

		for (int i = 0; i < motherboard.cpu.cores_count; i++) {
			cpu_enabled |= motherboard.cpu.cores[i].state & STATE_ENABLE;

			if (tick % (max_hz / motherboard.cpu.cores[i].hz) != 0)
				continue;

			// print_registers(&motherboard.cpu.cores[0], 0);
			core_step(&motherboard.cpu.cores[0]);
		}

		for (int i = 0; i < motherboard.devices_count; i++) {
			unsigned long hz = *(unsigned long*)(
			    motherboard.devices[i] + sizeof(void*) + sizeof(int) * 2
			);

			if (tick % (max_hz / hz) == 0)
				device_step(motherboard.devices[i]);
		}

		if (tick % (max_hz / motherboard.mmu.hz) == 0)
			mmu_step(&motherboard.mmu);

		if (tick % (max_hz / motherboard.cpu.apic.hz) == 0)
			apic_step(&motherboard.cpu.apic);
	}


	vvmhc_close(motherboard.devices[1]);
}


void print_help() {
	
}


void device_step(void* device) {
	unsigned int type = *(unsigned int*)(device + sizeof(void*));

	if (type == SYSINFO_TYPE_ID)
		sysinfo_step(device);

	if (type == VVMHC_TYPE_ID)
		vvmhc_step(device);
}
