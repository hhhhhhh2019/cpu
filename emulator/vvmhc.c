#include <vvmhc.h>
#include <motherboard.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>


void vvmhc_init(VVMHC* vvmhc, void* motherboard, unsigned long hz) {
	vvmhc->motherboard = motherboard;
	vvmhc->type_id = VVMHC_TYPE_ID;
	vvmhc->hz = hz;

	memset(vvmhc->registers, 0, sizeof(vvmhc->registers));

	vvmhc->init = 0;

	for (int i = 0; i < 16; i++)
		vvmhc->disks[i].connected = 0;
}


void vvmhc_add_disk(VVMHC* vvmhc, char *filename, char id) {
	if (vvmhc->disks[id].connected == 1)
		return;

	vvmhc->disks[id].connected = 1;
	vvmhc->disks[id].filename = filename;
	vvmhc->disks[id].fd = fopen(filename, "rwb");

	if (vvmhc->disks[id].fd == NULL) {
		perror("fopen");
		exit(errno);
	}

	fseek(vvmhc->disks[id].fd, 0, SEEK_END);
	vvmhc->disks[id].size = ftell(vvmhc->disks[id].fd);
	fseek(vvmhc->disks[id].fd, 0, SEEK_SET);
}


void vvmhc_step(VVMHC* vvmhc) {
	
}
