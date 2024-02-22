#include <sysinfo.h>
#include <motherboard.h>

#include <stdio.h>


void sysinfo_init(Sysinfo* sysinfo, void *motherboard, unsigned long hz) {
	sysinfo->motherboard = motherboard;
	sysinfo->type_id = SYSINFO_TYPE_ID;
	sysinfo->hz = hz;
}


void sysinfo_step(Sysinfo* sysinfo) {
	char  cmd  = *(char*) (sysinfo->registers + 0);
	short arg  = *(short*)(sysinfo->registers + 1);
	long* resp =  (long*) (sysinfo->registers + 3);

	Motherboard* motherboard = (Motherboard*)sysinfo->motherboard;

	sysinfo->registers[0] = 0;

	if (cmd == SYSINFO_CMD_GET_DEVICE_COUNT) {
		if (interactive_mode) {
			printf("Sysinfo: Device count: %d\n", motherboard->devices_count);
		}

		*resp = motherboard->devices_count;
	}

	if (cmd == SYSINFO_CMD_GET_RAMSIZE) {
		if (interactive_mode) {
			printf("Sysinfo: RAM size: %ld\n", motherboard->ram.ram_size);
		}

		*resp = motherboard->ram.ram_size;
	}

	if (cmd == SYSINFO_CMD_GET_DEVICE_TYPE) {
		if (interactive_mode) {
      printf("Sysinfo: Device type: id: %d\n", arg);
    }

		if (arg < motherboard->devices_count) {
			void* device = motherboard->devices[arg];
			*resp = *(unsigned int*)(device + sizeof(void*));
		} else {
			*resp = 0;
		}
	}
}
