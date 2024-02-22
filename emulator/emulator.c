#include <motherboard.h>
#include <utils.h>

#include <sysinfo.h>
#include <timer.h>
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

char interactive_mode = 0;

unsigned long timer = 1;

unsigned long max_hz = 0;
unsigned long tick = 0;


char print_regs = 1;


void print_help();
void parse_command(char*, unsigned int);
char* readline(unsigned long* len);
void step();


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

		else if (strcmp(argv[i], "--interactive") == 0 || strcmp(argv[i], "-i") == 0) {
			interactive_mode = 1;
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
	motherboard.devices[motherboard.devices_count-1] = malloc(sizeof(Timer));

	timer_init(motherboard.devices[motherboard.devices_count-1], &motherboard, 100);


	motherboard.devices = realloc(motherboard.devices, sizeof(void*) * (++motherboard.devices_count));
	motherboard.devices[motherboard.devices_count-1] = malloc(sizeof(VVMHC));

	vvmhc_init(motherboard.devices[motherboard.devices_count-1], &motherboard, 100);

	vvmhc_add_disk(motherboard.devices[motherboard.devices_count-1], "disk.img", 0);



	char cpu_enabled = 1;

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


	while (1) {
		if (interactive_mode) {
			putc('\n', stdout);
			putc('>', stdout);
			putc(' ', stdout);

			unsigned long len = 0;

			char* input = readline(&len);

			parse_command(input, len);

			free(input);
		} else {
			step();
		}
	}


	// vvmhc_close(motherboard.devices[2]);
}


void print_help() {
	
}


void device_step(void* device) {
	unsigned int type = *(unsigned int*)(device + sizeof(void*));

	if (type == SYSINFO_TYPE_ID)
		sysinfo_step(device);

	if (type == TIMER_TYPE_ID)
		timer_step(device);

	if (type == VVMHC_TYPE_ID)
		vvmhc_step(device);
}


void step() {
	printf("\nstep: %lu\n\n", tick);

	if (print_regs) {
		for (int i = 0; i < motherboard.cpu.cores_count; i++)
			print_registers(&motherboard.cpu.cores[i], i);
	}

	for (int i = 0; i < motherboard.cpu.cores_count; i++) {
		// cpu_enabled |= motherboard.cpu.cores[i].state & STATE_ENABLE;

		if (tick % (max_hz / motherboard.cpu.cores[i].hz) != 0)
			continue;

		core_step(&motherboard.cpu.cores[i]);
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

	tick++;
}


char* readline(unsigned long* len) {
	*len = 0;
	char* buffer = malloc(0);

	int c = getc(stdin);

	while (c != '\n' && c != EOF) {
		buffer = realloc(buffer, sizeof(char) * (++*len));
		buffer[*len - 1] = c;

		c = getc(stdin);
	}

	buffer = realloc(buffer, sizeof(char) * (*len + 1));
	buffer[*len] = 0;

	return buffer;
}


void parse_command(char* s, unsigned int len) {
	char* save_ptr;
	strtok(s, " ");

	char* cmd = s;

	if (strcmp(cmd, "step") == 0 || strcmp(cmd, "s") == 0 || len == 0) {
		if (len == strlen(cmd)) {
			step();
			return;
		}

		for (int i = 0; i < atol(s + strlen(cmd) + 1); i++)
			step();

		return;
	}


	if (strcmp(cmd, "registers") == 0 || strcmp(cmd, "r") == 0) {
		if (len == strlen(cmd)) {
			printf(": %d\n", motherboard.cpu.cores_count);
			for (int i = 0; i < motherboard.cpu.cores_count; i++)
				print_registers(&motherboard.cpu.cores[i], i);
			return;
		}

		char id = atoi(s + strlen(cmd) + 1);

		print_registers(&motherboard.cpu.cores[id], id);

		return;
	}


	if (strcmp(cmd, "toggle") == 0 || strcmp(cmd, "t") == 0) {
		if (len == strlen(cmd))
			return;

		char* a = s + strlen(cmd) + 1;

		if (strcmp(a, "registers") == 0 || strcmp(a, "r") == 0)
			print_regs ^= 1;

		return;
	}
}
