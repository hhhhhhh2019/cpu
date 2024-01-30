#include <sysinfo.h>
#include <motherboard.h>


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
		*resp = motherboard->devices_count;
	}

	if (cmd == SYSINFO_CMD_GET_RAMSIZE) {
		*resp = motherboard->ram.ram_size;
	}

	if (cmd == SYSINFO_CMD_GET_DEVICE_TYPE) {
		if (arg < motherboard->devices_count) {
			void* device = motherboard->devices[arg];
			*resp = *(unsigned int*)(device + sizeof(void*));
		} else {
			*resp = 0;
		}
	}
}
