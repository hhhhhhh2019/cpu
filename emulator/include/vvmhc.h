#ifndef VVMHC_H
#define VVMHC_H


#include <stdio.h>


#define VVMHC_TYPE_ID 2

#define VVMHC_STATE_INIT  1
#define VVMHC_STATE_BUSY  2
#define VVMHC_STATE_PAUSE 4

#define VVMHC_CMD_GET_CONNECTED 1
#define VVMHC_CMD_GET_SIZE 2
#define VVMHC_CMD_READ 3
#define VVMHC_CMD_WRITE 4
#define VVMHC_CMD_SET_INTERRUPT 5


typedef struct {
	char connected;
	char* filename;
	FILE* fd;
	unsigned long size;
} VVMHC_disk;


typedef struct {
	void* motherboard;
	unsigned int type_id;
	unsigned long hz;
	char registers[31];

	char init;
	char init_timer;

	VVMHC_disk disks[16];
} VVMHC;


void vvmhc_init(VVMHC*, void* motherboard, unsigned long hz);
void vvmhc_step(VVMHC*);

void vvmhc_add_disk(VVMHC*, char* filename, char id);


#endif
