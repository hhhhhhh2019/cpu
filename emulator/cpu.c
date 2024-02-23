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
	10, // adde r r num64
	10, // addne r r num64
	10, // addl r r num64
	10, // addb r r num64
	10, // addo r r num64
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

	if (core->registers == core->registersn)
		mode = 1;

	LOG("+--- Core %02d --- %d ---------------------------------------------------------------------+\n", id, mode);

	LOG("│ st: %016lx ", core->state);
	for (int j = 0; j < 64; j++)
		LOG("%c", (core->state >> (63 - j) & 1) + '0');
	LOG(" │\n");

	for (int i = 1; i < 18; i++) {
		LOG("│ %02d: %016lx ", i, core->registers[i]);

		for (int j = 0; j < 64; j++)
			LOG("%c", (core->registers[i] >> (63 - j) & 1) + '0');

		LOG(" │\n");
	}

	LOG("+---------------------------------------------------------------------------------------+\n");
}


unsigned long core_alu(Core* core, unsigned long a, unsigned long b, enum ALU_OP op) {
	core->registers[REG_FLAG] &= ~(FLAG_ZERO | FLAG_CARRY | FLAG_SIGN);

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
	core->registers[REG_FLAG] |= ((sub >> 63) & 1) << 2;

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
	MMU* mmu = &((Motherboard*)core->motherboard)->mmu;

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


	if (instr == 0x00) { // stol r r num64
		if (interactive_mode) {
			printf("stol r%d r%d %016lx\n", r1, r2, num1);
		}

		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num1, 8, core->registers[r1]);
	}

	else if (instr == 0x01) { // stoi r r num64
		if (interactive_mode) {
			printf("stoi r%d r%d %016lx\n", r1, r2, num1);
		}

		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num1, 4, core->registers[r1]);
	}

	else if (instr == 0x02) { // stos r r num64
		if (interactive_mode) {
			printf("stos r%d r%d %016lx\n", r1, r2, num1);
		}

		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num1, 2, core->registers[r1]);
	}

	else if (instr == 0x03) { // stob r r num64
		if (interactive_mode) {
			printf("stob r%d r%d %016lx\n", r1, r2, num1);
		}

		mmu_write(&motherboard->mmu,
		          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		          core->registers[r2] + num1, 1, core->registers[r1]);
	}

	else if (instr == 0x04) { // loal r r num64
		if (interactive_mode) {
			printf("loal r%d r%d %016lx\n", r1, r2, num1);
		}

		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num1, 8, &rules);
	}

	else if (instr == 0x05) { // loai r r num64
		if (interactive_mode) {
			printf("loai r%d r%d %016lx\n", r1, r2, num1);
		}

		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num1, 4, &rules);
	}

	else if (instr == 0x06) { // loas r r num64
		if (interactive_mode) {
			printf("loas r%d r%d %016lx\n", r1, r2, num1);
		}

		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num1, 2, &rules);
	}

	else if (instr == 0x07) { // loab r r num 64
		if (interactive_mode) {
			printf("loab r%d r%d %016lx\n", r1, r2, num1);
		}

		char rules;
		core->registers[r1] = mmu_read(&motherboard->mmu,
		         (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
		         core->registers[r2] + num1, 1, &rules);
	}

	else if (instr == 0x08) { // add r r r
		if (interactive_mode) {
			printf("add r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_ADD);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x09) { // sub r r r
		if (interactive_mode) {
			printf("sub r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_SUB);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x0a) { // mul r r r
		if (interactive_mode) {
			printf("mul r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_MUL);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x0b) { // div r r r
		if (interactive_mode) {
			printf("div r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_DIV);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x0c) { // smul r r r
		if (interactive_mode) {
			printf("smul r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_SMUL);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x0d) { // sdiv r r r
		if (interactive_mode) {
			printf("sdiv r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], core->registers[r3], ALU_SDIV);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x0e) { // cmp r r
		if (interactive_mode) {
			printf("cmp r%d r%d\n", r1, r2);
		}

		core_alu(core, core->registers[r1], core->registers[r2], 0);
	}

	else if (instr == 0x0f) { // add r r num
		if (interactive_mode) {
			printf("add r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x10) { // sub r r num
		if (interactive_mode) {
			printf("sub r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_SUB);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x11) { // mul r r num
		if (interactive_mode) {
			printf("mul r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_MUL);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x12) { // div r r num
		if (interactive_mode) {
			printf("div r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_DIV);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x13) { // smul r r num
		if (interactive_mode) {
			printf("smul r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_SMUL);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x14) { // sdiv r r num
		if (interactive_mode) {
			printf("sidv r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_SDIV);

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x15) { // cmp r num64
		if (interactive_mode) {
			printf("cmp r%d %016lx\n", r1, num1);
		}

		core_alu(core, core->registers[r1], num1, ALU_SUB);
	}

	else if (instr == 0x16) { // adde r r num64
		if (interactive_mode) {
			printf("adde r%d r%d %016lx\n", r1, r2, num1);
		}

		if ((core->registers[REG_FLAG] & FLAG_ZERO) != 0) {
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
			
			if (r1 == REG_PC)
				return;
		}
	}

	else if (instr == 0x17) { // addne r r num64
		if (interactive_mode) {
			printf("addne r%d r%d %016lx\n", r1, r2, num1);
		}

		if ((core->registers[REG_FLAG] & FLAG_ZERO) == 0) {
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
			
			if (r1 == REG_PC)
				return;
		}
	}

	else if (instr == 0x18) { // addl r r num64
		if (interactive_mode) {
			printf("addl r%d r%d %016lx\n", r1, r2, num1);
		}

		if ((core->registers[REG_FLAG] & FLAG_CARRY) != 0) {
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
			
			if (r1 == REG_PC)
				return;
		}
	}

	else if (instr == 0x19) { // addg r r num64
		if (interactive_mode) {
			printf("addg r%d r%d %016lx\n", r1, r2, num1);
		}

		if ((core->registers[REG_FLAG] & FLAG_ZERO) == 0 &&
		    (core->registers[REG_FLAG] & FLAG_CARRY) == 0) {
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
			
			if (r1 == REG_PC)
				return;
		}
	}

	else if (instr == 0x1a) { // addsl r r num64
		if (interactive_mode) {
			printf("addsl r%d r%d %016lx\n", r1, r2, num1);
		}

		if ((core->registers[REG_FLAG] & FLAG_SIGN) != 0) {
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);
			
			if (r1 == REG_PC)
				return;
		}
	}

	else if (instr == 0x1b) { // addsg r r num64
		if (interactive_mode) {
			printf("addsg r%d r%d %016lx\n", r1, r2, num1);
		}

		if ((core->registers[REG_FLAG] & FLAG_ZERO) == 0 &&
		    (core->registers[REG_FLAG] & FLAG_SIGN) == 0) {
			core->registers[r1] = core_alu(core, core->registers[r2], num1, ALU_ADD);

			if (r1 == REG_PC)
				return;
		}
	}

	else if (instr == 0x1c) { // pushl r
		if (interactive_mode) {
			printf("pushl r%d\n", r1);
		}

		core->registers[REG_SP] -= 8;
		*(unsigned long*)(ram + core->registers[REG_SP]) = core->registers[r1];
	}

	else if (instr == 0x1d) { // pushi r
		if (interactive_mode) {
			printf("pushi r%d\n", r1);
		}

		core->registers[REG_SP] -= 4;
		*(unsigned int*)(ram + core->registers[REG_SP]) = core->registers[r1];
	}

	else if (instr == 0x1e) { // pushs r
		if (interactive_mode) {
			printf("pushs r%d\n", r1);
		}

		core->registers[REG_SP] -= 2;
		*(unsigned short*)(ram + core->registers[REG_SP]) = core->registers[r1];
	}

	else if (instr == 0x1f) { // pushb r
		if (interactive_mode) {
			printf("pushb r%d\n", r1);
		}

		core->registers[REG_SP] -= 1;
		*(unsigned char*)(ram + core->registers[REG_SP]) = core->registers[r1];
	}

	else if (instr == 0x20) { // popl r
		if (interactive_mode) {
			printf("popl r%d\n", r1);
		}

		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]);
		core->registers[REG_SP] += 8;

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x21) { // popi r
		if (interactive_mode) {
			printf("popi r%d\n", r1);
		}

		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]) & 0xffffffff;
		core->registers[REG_SP] += 4;

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x22) { // pops r
		if (interactive_mode) {
			printf("pops r%d\n", r1);
		}

		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]) & 0xffff;
		core->registers[REG_SP] += 2;

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x23) { // popb r
		if (interactive_mode) {
			printf("popb r%d\n", r1);
		}

		core->registers[r1] = *(unsigned long*)(ram + core->registers[REG_SP]) & 0xff;
		core->registers[REG_SP] += 1;

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x24) { // call r
		if (interactive_mode) {
			printf("call r%d\n", r1);
		}

		core->registers[REG_SP] -= 8;
		*(unsigned long*)(ram + core->registers[REG_SP]) = core->registers[REG_PC];

		core->registers[REG_PC] = core->registers[r1];

		return;
	}

	else if (instr == 0x25) { // int num8
		if (interactive_mode) {
			printf("int %d\n", num3);
		}

		apic_push(&motherboard->cpu.apic, num3);
		return;
	}

	else if (instr == 0x26) { // iret
		if (interactive_mode) {
			printf("iret\n");
		}

		core->is_interrupt = 0;
	}

	else if (instr == 0x27) { // and r r r
		if (interactive_mode) {
			printf("and r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core->registers[r2] & core->registers[r3];

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x28) { // or r r r
		if (interactive_mode) {
			printf("or r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core->registers[r2] | core->registers[r3];

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x29) { // xor r r r
		if (interactive_mode) {
			printf("xor r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core->registers[r2] ^ core->registers[r3];

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x2a) { // not r r
		if (interactive_mode) {
			printf("not r%d r%d\n", r1, r2);
		}

		core->registers[r1] = ~core->registers[r2];

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x2b) { // shl r r r
		if (interactive_mode) {
			printf("shl r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core->registers[r2] << core->registers[r3];

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x2c) { // shr r r r
		if (interactive_mode) {
			printf("shr r%d r%d r%d\n", r1, r2, r3);
		}

		core->registers[r1] = core->registers[r2] >> core->registers[r3];

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x2d) { // and r r num64
		if (interactive_mode) {
			printf("and r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core->registers[r2] & num1;

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x2e) { // or r r num64
		if (interactive_mode) {
			printf("or r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core->registers[r2] | num1;

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x2f) { // xor r r num64
		if (interactive_mode) {
			printf("xor r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core->registers[r2] ^ num1;

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x30) { // shl r r num64
		if (interactive_mode) {
			printf("shl r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core->registers[r2] << num1;

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x31) { // shr r r num64
		if (interactive_mode) {
			printf("shr r%d r%d %016lx\n", r1, r2, num1);
		}

		core->registers[r1] = core->registers[r2] >> num1;

		if (r1 == REG_PC)
			return;
	}

	else if (instr == 0x32) { // chst r
		if (interactive_mode) {
			printf("chst r%d\n", r1);
		}

		if (core->registers == core->registersk) {
			core->state = core->registers[r1];
		} else {
			// panic
		}
	}

	else if (instr == 0x33) { // lost r
		if (interactive_mode) {
			printf("lost r%d\n", r1);
		}

		if (core->registers == core->registersk) {
			core->registers[r1] = core->state;
		} else {
			// panic
		}
	}

	else if (instr == 0x34) { // stolk r r num64
		if (interactive_mode) {
			printf("stolk r%d r%d %016lx\n", r1, r2, num1);
		}

		if (core->registers == core->registersk) {
			mmu_write(&motherboard->mmu,
			          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
			          core->registersk[r2] + num1, 8, core->registersn[r1]);
		} else {
			// panic
		}
	}

	else if (instr == 0x35) { // loalk r r num64
		if (interactive_mode) {
			printf("loalk r%d r%d %016lx\n", r1, r2, num1);
		}

		if (core->registers == core->registersk) {
			char rules;
			core->registersn[r1] = mmu_read(&motherboard->mmu,
			          (core->state & STATE_PAGING) != 0, core->registers[REG_TP],
			          core->registersk[r2] + num1, 8, &rules);
		} else {
			// panic
		}
	}

	else if (instr == 0x36) { // chtp r
		if (interactive_mode) {
			printf("chtp r%d\n", r1);
		}

		if (core->registers == core->registersk) {
			core->registersk[r1] = core->registersn[REG_TP];
		} else {
			// panic
		}
	}

	else if (instr == 0x37) { // lotp r
		if (interactive_mode) {
			printf("lotp r%d\n", r1);
		}

		if (core->registers == core->registersk) {
			core->registersn[REG_TP] = core->registersk[r1];
		} else {
			// panic
		}
	}

	else if (instr == 0x38) { // chflag r
		if (interactive_mode) {
			printf("chflag r%d\n", r1);
		}

		if (core->registers == core->registersk) {
			core->registersk[r1] = core->registersn[REG_FLAG];
		} else {
			// panic
		}
	}

	else if (instr == 0x39) { // loflag r
		if (interactive_mode) {
			printf("loflag r%d\n", r1);
		}

		if (core->registers == core->registersk) {
			core->registersn[REG_FLAG] = core->registersk[r1];
		} else {
			// panic
		}
	}

	else if (instr == 0x3a) { // cint num8 num8
		if (interactive_mode) {
			printf("cint %d %d\n", num3, num4);
		}

		if (core->registers == core->registersk) {
			apic_push_core(&motherboard->cpu.apic, num3, num4);
		} else {
			// panic
		}
	}


	core->registers[REG_PC] += instr_size[instr];
}
