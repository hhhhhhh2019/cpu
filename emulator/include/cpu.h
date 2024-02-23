#ifndef CPU_H
#define CPU_H


#include <apic.h>


#define STATE_ENABLE      (1 << 0)
#define STATE_INTERRUPTS  (1 << 1)
#define STATE_PAGING      (1 << 2)
#define STATE_NORMAL_MODE (1 << 3)

#define FLAG_ZERO  1
#define FLAG_CARRY 2
#define FLAG_SIGN  4


#define REG_FLAG 17
#define REG_TP   16
#define REG_PC   15
#define REG_SP   14


enum ALU_OP {
	ALU_ADD,
	ALU_SUB,
	ALU_MUL,
	ALU_DIV,
	ALU_SMUL,
	ALU_SDIV
};


typedef struct {
	void* motherboard;

	unsigned long hz;

	unsigned long* registers;
	unsigned long registersn[18];
	unsigned long registersk[18];

	unsigned long state;

	char is_interrupt;
} Core;


typedef struct {
	void* motherboard;

	unsigned char cores_count;
	Core* cores;

	APIC apic;
} CPU;


void cpu_init(CPU*, void* motherboard, char cores_count, unsigned long hz);

void print_registers(Core*, int id);

void core_step(Core*);
void core_int(Core*, unsigned char);


#endif
