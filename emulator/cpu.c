#include "mmu.h"
#include <cpu.h>
#include <motherboard.h>
#include <utils.h>

#include <stdlib.h>
#include <stdio.h>



char instr_size[] = {
	2,  // mov r r
	10, // stol r r num64
	10, // stoi r r num64
	10, // stos r r num64
	10, // stob r r num64
	10, // loal r r num64
	10, // loai r r num64
	10, // loas r r num64
	10, // loab r r num64
	3,  // add r r r
	3,  // sub r r r
	3,  // mul r r r
	3,  // div r r r
	3,  // smul r r r
	3,  // sdiv r r r
	3,  // cmp r r r
	10, // add r r num64
	10, // sub r r num64
	10, // mul r r num64
	10, // div r r num64
	10, // smul r r num64
	10, // sdiv r r num64
	10, // cmp r r num64
	9,  // je num64
	9,  // jne num64
	9,  // jl num64
	9,  // jb num64
	9,  // jo num64
	2,  // pushl r
	2,  // pushi r
	2,  // pushs r
	2,  // pushb r
	2,  // popl r
	2,  // popi r
	2,  // pops r
	2,  // popb r
	2,  // call r
	1,  // ret
	2,  // int num8
	1,  // iret
	3,  // and r r r
	3,  // or r r r
	3,  // xor r r r
	2,  // not r r
	3,  // shl r r r
	3,  // shr r r r
	10, // and r r num64
	10, // or r r num64
	10, // xor r r num64
	10, // shl r r num64
	10, // shr r r num64
	2,  // chst r
	2,  // lost r
	10, // stolk r r
	10, // loalk r r
	2,  // chtp r
	2,  // lotp r
	2,  // chflag r
	2,  // loflag r
};



void cpu_init(CPU* cpu, void *motherboard, char cores_count, unsigned long *hz) {
	cpu->motherboard = motherboard;

	cpu->cores_count = cores_count;
	cpu->cores = malloc(sizeof(Core) * cores_count);

	for (int i = 0; i < cores_count; i++) {
		cpu->cores[i].hz = hz[i];
		cpu->cores[i].motherboard = motherboard;
		cpu->cores[i].state = 0;
		cpu->cores[i].is_interrupt = 0;
	}
}


void print_registers(Core* core, int id) {
	char mode = 0;

	if ((core->state & STATE_NORMAL_MODE) != 0 || core->is_interrupt)
		mode = 1;

	LOG("+--- Core %02d --- %d ---------------------------------------------------------------------+\n", id, mode);

	LOG("| st: %016lx ", core->state);
	for (int j = 0; j < 64; j++)
		LOG("%c", (core->state >> (63 - j) & 1) + '0');
	LOG(" |\n");

	for (int i = 1; i < 18; i++) {
		LOG("| %02d: %016lx ", i, core->registers[i]);

		for (int j = 0; j < 64; j++)
			LOG("%c", (core->registers[i] >> (63 - j) & 1) + '0');

		LOG(" |\n");
	}

	LOG("+---------------------------------------------------------------------------------------+\n");
}


unsigned long core_alu(Core* core, unsigned long a, unsigned long b, enum ALU_OP op) {
	core->registers[REG_FLAG] &= ~(FLAG_ZERO | FLAG_CARRY | FLAG_OVERFLOW);

	long sa = *(&a);
	long sb = *(&b);

	unsigned long sum = a + b;
	unsigned long sub = a - b;
	unsigned long mul = a * b;
	unsigned long div = a / b;
	long smul = a * b;
	long sdiv = a / b;

	if (a == b)
		core->registers[REG_FLAG] |= FLAG_ZERO;
	if (a < b)
		core->registers[REG_FLAG] |= FLAG_CARRY;
	core->registers[REG_FLAG] |= ((core->registers[REG_FLAG] & FLAG_CARRY) << 1) | (sub >> 61) & 0b100;

	if (op == ALU_ADD)
		return sum;
	if (op == ALU_SUB)
		return sub;
	if (op == ALU_MUL)
		return mul;
	if (op == ALU_DIV)
		return div;
	if (op == ALU_SMUL)
		return *(&smul);
	if (op == ALU_SDIV)
		return *(&sdiv);

	return 0;
}


void core_int(Core* core, unsigned char id) {
	
}


void core_step(Core* core) {
	core->registersk[0] = 0;
	core->registersn[0] = 0;

	if ((core->state & STATE_NORMAL_MODE) == 0 || core->is_interrupt)
		core->registers = core->registersk;
	else
		core->registers = core->registersn;

	Motherboard* motherboard = ((Motherboard*)core->motherboard);
	char* ram = motherboard->ram.ram;

	char instr = ram[core->registers[REG_PC]];

	char r1 = (ram[core->registers[REG_PC] + 1] & 0xf0) >> 4;
	char r2 = ram[core->registers[REG_PC] + 1] & 0x0f;
	char r3 = (ram[core->registers[REG_PC] + 2] & 0xf0) >> 4;

	unsigned long num1 = *(unsigned long*)(ram + core->registers[REG_PC] + 2); // 1,2,3,4,5,6,7,8,10,11,12,13,14,15,16
	unsigned long num2 = *(unsigned long*)(ram + core->registers[REG_PC] + 1); // 17,18,19,1a
	unsigned char num3 = ram[core->registers[REG_PC] + 1]; // 25


	core->registers[REG_PC] += instr_size[instr];


	LOG("%x\n", instr);
	LOG("r1: %d  r2: %d  r3: %d\n", r1,r2,r3);
	LOG("num1: %lx\n", num1);
	LOG("num2: %lx\n", num2);
	LOG("num3: %x\n", num3);


	if (instr == 0x00) { // mov r r
		core->registers[r1] = core->registers[r2];
	}

	else if (instr == 0x01) { // stol r r num64
		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num2, 8, core->registers[r1]);
	}

	else if (instr == 0x02) { // stoi r r num64
		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num2, 4, core->registers[r1]);
	}

	else if (instr == 0x03) { // stos r r num64
		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num2, 2, core->registers[r1]);
	}

	else if (instr == 0x04) { // stob r r num64
		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num2, 1, core->registers[r1]);
	}

	else if (instr == 0x05) { // loal r r num64
		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num2, 8, &rules);
	}

	else if (instr == 0x06) { // loai r r num64
		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num2, 4, &rules);
	}

	else if (instr == 0x07) { // loas r r num64
		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num2, 2, &rules);
	}

	else if (instr == 0x08) { // loab r r num 64
		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num2, 1, &rules);
	}

	else if (instr == 0x09) { // add r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_ADD);
	}

	else if (instr == 0x0a) { // sub r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_SUB);
	}

	else if (instr == 0x0b) { // mul r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_MUL);
	}

	else if (instr == 0x0c) { // div r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_DIV);
	}

	else if (instr == 0x0d) { // smul r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_SMUL);
	}

	else if (instr == 0x0e) { // sdiv r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_SDIV);
	}

	else if (instr == 0x0f) { // cmp r r
		core_alu(core, core->registers[r1], core->registers[r2], 0);
	}

	else if (instr == 0x10) { // add r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
	}

	else if (instr == 0x11) { // sub r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_SUB);
	}

	else if (instr == 0x12) { // mul r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_MUL);
	}

	else if (instr == 0x13) { // div r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_DIV);
	}

	else if (instr == 0x14) { // smul r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_SMUL);
	}

	else if (instr == 0x15) { // sdiv r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_SDIV);
	}

	else if (instr == 0x16) { // cmp r num64
		core_alu(core, core->registers[r1], num1, ALU_ADD);
	}

	else if (instr == 0x17) { // je num64
		if ((core->registers[REG_FLAG] & FLAG_ZERO) != 0)
			core->registers[REG_PC] = num1;
	}

	else if (instr == 0x18) { // jne num64
		if ((core->registers[REG_FLAG] & FLAG_ZERO) == 0)
			core->registers[REG_PC] = num1;
	}

	else if (instr == 0x19) { // jl num64
		if ((core->registers[REG_FLAG] & FLAG_CARRY) != 0)
			core->registers[REG_PC] = num1;
	}

	else if (instr == 0x1a) { // jg num64
		if ((core->registers[REG_FLAG] & FLAG_ZERO) == 0 && (core->registers[REG_FLAG] & FLAG_CARRY) == 0)
			core->registers[REG_PC] = num1;
	}

	else if (instr == 0x1b) { // jo num64
		if ((core->registers[REG_FLAG] & FLAG_OVERFLOW) != 0)
			core->registers[REG_PC] = num1;
	}

	else if (instr == 0x1c) { // jno num64
		if ((core->registers[REG_FLAG] & FLAG_OVERFLOW) == 0)
			core->registers[REG_PC] = num1;
	}

	else if (instr == 0x1d) { // pushl r
		*(unsigned long*)(ram + core->registers[REG_SP]) = core->registers[r1];
		core->registers[REG_SP] -= 8;
	}

	else if (instr == 0x1e) { // pushi r
		*(unsigned int*)(ram + core->registers[REG_SP]) = core->registers[r1];
		core->registers[REG_SP] -= 4;
	}

	else if (instr == 0x1f) { // pushs r
		*(unsigned short*)(ram + core->registers[REG_SP]) = core->registers[r1];
		core->registers[REG_SP] -= 2;
	}

	else if (instr == 0x20) { // pushb r
		*(unsigned char*)(ram + core->registers[REG_SP]) = core->registers[r1];
		core->registers[REG_SP] -= 1;
	}

	else if (instr == 0x21) { // popl r
		core->registers[REG_SP] += 8;
		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]);
	}

	else if (instr == 0x22) { // popi r
		core->registers[REG_SP] += 4;
		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]) & 0xffffffff;
	}

	else if (instr == 0x23) { // popl r
		core->registers[REG_SP] += 2;
		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]) & 0xffff;
	}

	else if (instr == 0x24) { // popl r
		core->registers[REG_SP] += 1;
		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]) & 0xff;
	}

	else if (instr == 0x25) { // call r
		*(unsigned long*)(ram + core->registers[REG_SP]) = core->registers[REG_PC];
		core->registers[REG_SP] -= 8;

		core->registers[REG_PC] = core->registers[r1];
	}

	else if (instr == 0x26) { // int num8
		core_int(core, num3);
	}

	else if (instr == 0x27) { // iret
		core->is_interrupt = 0;
	}

	else if (instr == 0x28) { // and r r r
		core->registers[r1] = core->registers[r2] & core->registers[r3];
	}

	else if (instr == 0x29) { // or r r r
		core->registers[r1] = core->registers[r2] | core->registers[r3];
	}

	else if (instr == 0x2a) { // xor r r r
		core->registers[r1] = core->registers[r2] ^ core->registers[r3];
	}

	else if (instr == 0x2b) { // not r r
		core->registers[r1] = ~core->registers[r2];
	}

	else if (instr == 0x2c) { // shl r r r
		core->registers[r1] = core->registers[r2] << core->registers[r3];
	}

	else if (instr == 0x2d) { // shr r r r
		core->registers[r1] = core->registers[r2] >> core->registers[r3];
	}

	else if (instr == 0x2e) { // and r r num64
		core->registers[r1] = core->registers[r2] & num1;
	}

	else if (instr == 0x2f) { // or r r num64
		core->registers[r1] = core->registers[r2] | num1;
	}

	else if (instr == 0x30) { // xor r r num64
		core->registers[r1] = core->registers[r2] ^ num1;
	}

	else if (instr == 0x31) { // shl r r num64
		core->registers[r1] = core->registers[r2] << num1;
	}

	else if (instr == 0x32) { // shr r r num64
		core->registers[r1] = core->registers[r2] >> num1;
	}

	else if (instr == 0x33) { // chst r
		if (core->registers == core->registersk) {
			core->state = core->registers[r1];
		} else {
			// panic
		}
	}

	else if (instr == 0x34) { // lost r
		if (core->registers == core->registersk) {
			core->registers[r1] = core->state;
		} else {
			// panic
		}
	}

	else if (instr == 0x35) { // stolk r r
		if (core->registers == core->registersk) {
			mmu_write(&motherboard->mmu,
			          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
			          core->registersk[r2] + num2, 8, core->registersn[r1]);
		} else {
			// panic
		}
	}

	else if (instr == 0x36) { // loal r r num64
		if (core->registers == core->registersk) {
			char rules;
			core->registersn[r1] = mmu_read(&motherboard->mmu,
			          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
			          core->registersk[r2] + num2, 8, &rules);
		} else {
			// panic
		}
	}

	else if (instr == 0x37) { // chtp r
		if (core->registers == core->registersk) {
			core->registersk[r1] = core->registersn[REG_TP];
		} else {
			// panic
		}
	}

	else if (instr == 0x38) { // lotp r
		if (core->registers == core->registersk) {
			core->registersn[REG_TP] = core->registersk[r1];
		} else {
			// panic
		}
	}

	else if (instr == 0x39) { // chflag r
		if (core->registers == core->registersk) {
			core->registersk[r1] = core->registersn[REG_FLAG];
		} else {
			// panic
		}
	}

	else if (instr == 0x3a) { // loflag r
		if (core->registers == core->registersk) {
			core->registersn[REG_FLAG] = core->registersk[r1];
		} else {
			// panic
		}
	}
}
