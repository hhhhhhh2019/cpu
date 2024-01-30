#include "mmu.h"
#include <stdio.h>
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
	vvmhc->state = 0;
	vvmhc->buffer = NULL;

	for (int i = 0; i < 16; i++)
		vvmhc->disks[i].fd = NULL;
}


void vvmhc_close(VVMHC* vvmhc) {
	for (int i = 0; i < 16; i++) {
		if (vvmhc->disks[i].fd != NULL)
			fclose(vvmhc->disks[i].fd);
	}
}


void vvmhc_add_disk(VVMHC* vvmhc, char *filename, char id) {
	if (vvmhc->disks[id].fd != NULL)
		return;

	vvmhc->disks[id].filename = filename;
	vvmhc->disks[id].fd = fopen(filename, "rwb+");

	if (vvmhc->disks[id].fd == NULL) {
		perror("fopen");
		exit(errno);
	}

	fseek(vvmhc->disks[id].fd, 0, SEEK_END);
	vvmhc->disks[id].size = ftell(vvmhc->disks[id].fd);
	fseek(vvmhc->disks[id].fd, 0, SEEK_SET);
}


void vvmhc_step(VVMHC* vvmhc) {
	Motherboard* motherboard = vvmhc->motherboard;

	char* state       =  (char* )(vvmhc->registers + 0x00);
	char  cmd         = *(char* )(vvmhc->registers + 0x01);
	short block_count = *(short*)(vvmhc->registers + 0x02);
	long  start_addr  = *(long* )(vvmhc->registers + 0x04);
	long  addr        = *(long* )(vvmhc->registers + 0x0c);
	long* resp        =  (long* )(vvmhc->registers + 0x14);
	char  id          = *(char* )(vvmhc->registers + 0x1c);
	long  tp          = *(long* )(vvmhc->registers + 0x1d);

	vvmhc->registers[0x01] = 0;

	if (vvmhc->init == 0) {
		if ((*state & VVMHC_STATE_INIT) == 0)
			return;

		*state = 0;

		vvmhc->init_timer = 10;
		vvmhc->init = 1;

		return;
	}

	if (vvmhc->init_timer != 0) {
		vvmhc->init_timer--;
		return;
	}


	if (vvmhc->state == 1) { // reading from disk
		mmu_write(&motherboard->mmu, tp != 0, tp, vvmhc->ram_offset++, 1, vvmhc->buffer[vvmhc->buff_offset++]);

		if (vvmhc->buff_offset == vvmhc->buffer_size) {
			*state &= ~VVMHC_STATE_BUSY;
			vvmhc->state = 0;

			free(vvmhc->buffer);
		}

		return;
	}


	if (vvmhc->state == 2) { // writing
		char rules;
		char val = mmu_read(&motherboard->mmu, tp != 0, tp, vvmhc->ram_offset++, 1, &rules);
		vvmhc->buffer[vvmhc->buff_offset++] = val;

		if (vvmhc->buff_offset == vvmhc->buffer_size) {
			*state &= ~VVMHC_STATE_BUSY;
			vvmhc->state = 0;

			fseek(vvmhc->disks[id].fd, vvmhc->disk_offset, SEEK_SET);
			fwrite(vvmhc->buffer, vvmhc->buffer_size, 1, vvmhc->disks[id].fd);

			free(vvmhc->buffer);
		}

		return;
	}


	vvmhc->init = 1;
	*state |= VVMHC_STATE_INIT;


	if (cmd == VVMHC_CMD_GET_CONNECTED) {
		*resp = 0;

		for (int i = 0; i < 16; i++) {
			*resp |= ((vvmhc->disks[i].fd != NULL) & 1) << i;
		}
	}


	if (cmd == VVMHC_CMD_GET_SIZE) {
		*resp = vvmhc->disks[id].size;
	}


	if (cmd == VVMHC_CMD_READ) {
		if (vvmhc->disks[id].fd == NULL)
			return;

		vvmhc->buffer_size = 512 * block_count;
		vvmhc->buffer = malloc(vvmhc->buffer_size);

		vvmhc->ram_offset = addr;
    vvmhc->buff_offset = 0;
    vvmhc->state = 1;
		vvmhc->disk_offset = start_addr;

		fseek(vvmhc->disks[id].fd, start_addr, SEEK_SET);
		fread(vvmhc->buffer, 512, block_count, vvmhc->disks[id].fd);

		*state |= VVMHC_STATE_BUSY;
	}


	if (cmd == VVMHC_CMD_WRITE) {
		if (vvmhc->disks[id].fd == NULL)
			return;

		vvmhc->buffer_size = 512 * block_count;
		vvmhc->buffer = malloc(vvmhc->buffer_size);

		vvmhc->ram_offset = addr;
		vvmhc->buff_offset = 0;
		vvmhc->state = 2;
		vvmhc->disk_offset = start_addr;

		*state |= VVMHC_STATE_BUSY;
	}
}
