#ifndef VVMHC_H
#define VVMHC_H


#include <stdio.h>


#define VVMHC_TYPE_ID 3

#define VVMHC_STATE_INIT  1
#define VVMHC_STATE_BUSY  2

#define VVMHC_CMD_GET_CONNECTED 1
#define VVMHC_CMD_GET_SIZE 2
#define VVMHC_CMD_READ 3
#define VVMHC_CMD_WRITE 4


typedef struct {
	char* filename;
	FILE* fd;
	unsigned long size;
} VVMHC_disk;


typedef struct {
	void* motherboard;
	unsigned int type_id;
	unsigned long hz;
	char registers[37];

	char init;
	char init_timer;

	char state; // 0 - none, 1 - reading from disk, 2 - writing

	char* buffer;
	unsigned long buffer_size;
	unsigned long buff_offset;
	unsigned long ram_offset;
	unsigned long disk_offset;

	VVMHC_disk disks[16];
} VVMHC;


void vvmhc_init(VVMHC*, void* motherboard, unsigned long hz);
void vvmhc_step(VVMHC*);

void vvmhc_add_disk(VVMHC*, char* filename, char id);

void vvmhc_close(VVMHC*);


#endif
