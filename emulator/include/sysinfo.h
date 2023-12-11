#ifndef SYSINFO_H
#define SYSINFO_H


#define SYSINFO_TYPE_ID 1

#define SYSINFO_CMD_GET_DEVICE_COUNT 1
#define SYSINFO_CMD_GET_RAMSIZE 2
#define SYSINFO_CMD_GET_DEVICE_TYPE 3


typedef struct Sysinfo {
	void* motherboard;
	unsigned int type_id;
	unsigned long hz;
	char registers[11];
} Sysinfo;


void sysinfo_init(Sysinfo*, void* motherboard, unsigned long hz);
void sysinfo_step(Sysinfo*);


#endif
