#include <ram.h>

#include <stdlib.h>


void ram_init(RAM* ram, void *motherboard) {
	ram->motherboard = motherboard;
	ram->ram = malloc(ram->ram_size);
}
