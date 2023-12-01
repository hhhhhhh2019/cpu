#include <cpu.h>
#include <motherboard.h>
#include <utils.h>

#include <stdlib.h>
#include <stdio.h>



char instr_size[] = {
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
	2,  // cmp r r
	10, // add r r num64
	10, // sub r r num64
	10, // mul r r num64
	10, // div r r num64
	10, // smul r r num64
	10, // sdiv r r num64
	10, // cmp r r num64
	10,  // adde r r num64
	10,  // addne r r num64
	10,  // addl r r num64
	10,  // addb r r num64
	10,  // addo r r num64
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
	3,  // cint num8 num8
};



void cpu_init(CPU* cpu, void *motherboard, char cores_count, unsigned long hz) {
	cpu->motherboard = motherboard;

	cpu->cores_count = cores_count;
	cpu->cores = malloc(sizeof(Core) * cores_count);

	for (int i = 0; i < cores_count; i++) {
		cpu->cores[i].hz = hz;
		cpu->cores[i].motherboard = motherboard;
		cpu->cores[i].state = 0;
		cpu->cores[i].is_interrupt = 0;
		cpu->cores[i].registers = cpu->cores[i].registersk;

		for (int j = 0; j < 18; j++) {
			cpu->cores[i].registersk[j] = 0;
			cpu->cores[i].registersn[j] = 0;
		}
	}

	apic_init(&cpu->apic, motherboard, hz, cores_count);
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
	unsigned long div = 0;
	if (b != 0)
		div = a / b;
	long smul = a * b;
	long sdiv = 0;
	if (b != 0)
		div = a / b;

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
	if (core->is_interrupt == 1)
		return;

	char* ram = ((Motherboard*)core->motherboard)->ram.ram;

	core->is_interrupt = 1;
	core->registersk[REG_PC] = cpu2lt64(*(unsigned long*)(ram + id * 8));
}


void core_step(Core* core) {
	if ((core->state & STATE_ENABLE) == 0)
		return;

	core->registersk[0] = 0;
	core->registersn[0] = 0;

	if ((core->state & STATE_NORMAL_MODE) == 0 || core->is_interrupt)
		core->registers = core->registersk;
	else
		core->registers = core->registersn;

	Motherboard* motherboard = ((Motherboard*)core->motherboard);
	char* ram = motherboard->ram.ram;

	unsigned char instr = ram[core->registers[REG_PC]];

	unsigned char r1 = (ram[core->registers[REG_PC] + 1] & 0xf0) >> 4;
	unsigned char r2 = ram[core->registers[REG_PC] + 1] & 0x0f;
	unsigned char r3 = (ram[core->registers[REG_PC] + 2] & 0xf0) >> 4;

	unsigned long num1 = *(unsigned long*)(ram + core->registers[REG_PC] + 2);
	unsigned long num2 = *(unsigned long*)(ram + core->registers[REG_PC] + 1);
	unsigned char num3 = ram[core->registers[REG_PC] + 1];
	unsigned char num4 = ram[core->registers[REG_PC] + 2];


	LOG("%x\n", instr);
	LOG("r1: %d  r2: %d  r3: %d\n", r1,r2,r3);
	LOG("num1: %lx\n", num1);
	LOG("num2: %lx\n", num2);
	LOG("num3: %x\n", num3);

	
	long prev_pc_state = core->registers[REG_PC];


	if (instr == 0x00) { // stol r r num64
		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num2, 8, core->registers[r1]);
	}

	else if (instr == 0x01) { // stoi r r num64
		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num2, 4, core->registers[r1]);
	}

	else if (instr == 0x02) { // stos r r num64
		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num2, 2, core->registers[r1]);
	}

	else if (instr == 0x03) { // stob r r num64
		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num2, 1, core->registers[r1]);
	}

	else if (instr == 0x04) { // loal r r num64
		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num2, 8, &rules);
	}

	else if (instr == 0x05) { // loai r r num64
		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num2, 4, &rules);
	}

	else if (instr == 0x06) { // loas r r num64
		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num2, 2, &rules);
	}

	else if (instr == 0x07) { // loab r r num 64
		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num2, 1, &rules);
	}

	else if (instr == 0x08) { // add r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_ADD);
	}

	else if (instr == 0x09) { // sub r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_SUB);
	}

	else if (instr == 0x0a) { // mul r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_MUL);
	}

	else if (instr == 0x0b) { // div r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_DIV);
	}

	else if (instr == 0x0c) { // smul r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_SMUL);
	}

	else if (instr == 0x0d) { // sdiv r r r
		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_SDIV);
	}

	else if (instr == 0x0e) { // cmp r r
		core_alu(core, core->registers[r1], core->registers[r2], 0);
	}

	else if (instr == 0x0f) { // add r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
	}

	else if (instr == 0x10) { // sub r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_SUB);
	}

	else if (instr == 0x11) { // mul r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_MUL);
	}

	else if (instr == 0x12) { // div r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_DIV);
	}

	else if (instr == 0x13) { // smul r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_SMUL);
	}

	else if (instr == 0x14) { // sdiv r r num
		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_SDIV);
	}

	else if (instr == 0x15) { // cmp r num64
		core_alu(core, core->registers[r1], num1, ALU_SUB);
	}

	else if (instr == 0x16) { // adde r r num64
		if ((core->registers[REG_FLAG] & FLAG_ZERO) != 0)
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
	}

	else if (instr == 0x17) { // addne r r num64
		if ((core->registers[REG_FLAG] & FLAG_ZERO) == 0)
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
	}

	else if (instr == 0x18) { // addl r r num64
		if ((core->registers[REG_FLAG] & FLAG_CARRY) != 0)
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
	}

	else if (instr == 0x19) { // addg r r num64
		if ((core->registers[REG_FLAG] & FLAG_ZERO) == 0 && (core->registers[REG_FLAG] & FLAG_CARRY) == 0)
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
	}

	else if (instr == 0x1a) { // addo r r num64
		if ((core->registers[REG_FLAG] & FLAG_OVERFLOW) != 0)
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
	}

	else if (instr == 0x1b) { // addno r r num64
		if ((core->registers[REG_FLAG] & FLAG_OVERFLOW) == 0)
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
	}

	else if (instr == 0x1c) { // pushl r
		core->registers[REG_SP] -= 8;
		*(unsigned long*)(ram + core->registers[REG_SP]) = core->registers[r1];
	}

	else if (instr == 0x1d) { // pushi r
		core->registers[REG_SP] -= 4;
		*(unsigned int*)(ram + core->registers[REG_SP]) = core->registers[r1];
	}

	else if (instr == 0x1e) { // pushs r
		core->registers[REG_SP] -= 2;
		*(unsigned short*)(ram + core->registers[REG_SP]) = core->registers[r1];
	}

	else if (instr == 0x1f) { // pushb r
		core->registers[REG_SP] -= 1;
		*(unsigned char*)(ram + core->registers[REG_SP]) = core->registers[r1];
	}

	else if (instr == 0x20) { // popl r
		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]);
		core->registers[REG_SP] += 8;
	}

	else if (instr == 0x21) { // popi r
		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]) & 0xffffffff;
		core->registers[REG_SP] += 4;
	}

	else if (instr == 0x22) { // popl r
		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]) & 0xffff;
		core->registers[REG_SP] += 2;
	}

	else if (instr == 0x23) { // popl r
		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]) & 0xff;
		core->registers[REG_SP] += 1;
	}

	else if (instr == 0x24) { // call r
		core->registers[REG_SP] -= 8;
		*(unsigned long*)(ram + core->registers[REG_SP]) = core->registers[REG_PC];

		core->registers[REG_PC] = core->registers[r1];
	}

	else if (instr == 0x25) { // int num8
		apic_push(&motherboard->cpu.apic, num3);
	}

	else if (instr == 0x26) { // iret
		core->is_interrupt = 0;
	}

	else if (instr == 0x27) { // and r r r
		core->registers[r1] = core->registers[r2] & core->registers[r3];
	}

	else if (instr == 0x28) { // or r r r
		core->registers[r1] = core->registers[r2] | core->registers[r3];
	}

	else if (instr == 0x29) { // xor r r r
		core->registers[r1] = core->registers[r2] ^ core->registers[r3];
	}

	else if (instr == 0x2a) { // not r r
		core->registers[r1] = ~core->registers[r2];
	}

	else if (instr == 0x2b) { // shl r r r
		core->registers[r1] = core->registers[r2] << core->registers[r3];
	}

	else if (instr == 0x2c) { // shr r r r
		core->registers[r1] = core->registers[r2] >> core->registers[r3];
	}

	else if (instr == 0x2d) { // and r r num64
		core->registers[r1] = core->registers[r2] & num1;
	}

	else if (instr == 0x2e) { // or r r num64
		core->registers[r1] = core->registers[r2] | num1;
	}

	else if (instr == 0x2f) { // xor r r num64
		core->registers[r1] = core->registers[r2] ^ num1;
	}

	else if (instr == 0x30) { // shl r r num64
		core->registers[r1] = core->registers[r2] << num1;
	}

	else if (instr == 0x31) { // shr r r num64
		core->registers[r1] = core->registers[r2] >> num1;
	}

	else if (instr == 0x32) { // chst r
		if (core->registers == core->registersk) {
			core->state = core->registers[r1];
		} else {
			// panic
		}
	}

	else if (instr == 0x33) { // lost r
		if (core->registers == core->registersk) {
			core->registers[r1] = core->state;
		} else {
			// panic
		}
	}

	else if (instr == 0x34) { // stolk r r
		if (core->registers == core->registersk) {
			mmu_write(&motherboard->mmu,
			          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
			          core->registersk[r2] + num2, 8, core->registersn[r1]);
		} else {
			// panic
		}
	}

	else if (instr == 0x35) { // loal r r num64
		if (core->registers == core->registersk) {
			char rules;
			core->registersn[r1] = mmu_read(&motherboard->mmu,
			          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
			          core->registersk[r2] + num2, 8, &rules);
		} else {
			// panic
		}
	}

	else if (instr == 0x36) { // chtp r
		if (core->registers == core->registersk) {
			core->registersk[r1] = core->registersn[REG_TP];
		} else {
			// panic
		}
	}

	else if (instr == 0x37) { // lotp r
		if (core->registers == core->registersk) {
			core->registersn[REG_TP] = core->registersk[r1];
		} else {
			// panic
		}
	}

	else if (instr == 0x38) { // chflag r
		if (core->registers == core->registersk) {
			core->registersk[r1] = core->registersn[REG_FLAG];
		} else {
			// panic
		}
	}

	else if (instr == 0x39) { // loflag r
		if (core->registers == core->registersk) {
			core->registersn[REG_FLAG] = core->registersk[r1];
		} else {
			// panic
		}
	}

	else if (instr == 0x3a) { // cint num8 num8
		if (core->registers == core->registersk) {
			apic_push_core(&motherboard->cpu.apic, num3, num4);
		} else {
			// panic
		}
	}


	if (core->registers[REG_PC] == prev_pc_state)
		core->registers[REG_PC] += instr_size[instr];
}
