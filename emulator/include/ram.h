#ifndef RAM_H
#define RAM_H


typedef struct RAM {
	void* motherboard;

	unsigned long ram_size;
	char* ram;
} RAM;


void ram_init(RAM*, void* motherboard);


#endif
