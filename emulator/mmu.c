#include <mmu.h>
#include <motherboard.h>

#include <stdlib.h>


void mmu_init(MMU* mmu, void *motherboard, unsigned long hz) {
	mmu->motherboard = motherboard;
	mmu->hz = hz;
	mmu->busy = 0;

	mmu->mmio_count = 0;
	mmu->mmio = malloc(0);

	mmu_init_dev(mmu);
}


void mmu_init_dev(MMU* mmu) {
	mmu_add_mmio(mmu, (MMIO){
	    .start_addr = MMU_MMIO_OFFSET,
	    .size = 11,
	    .data = mmu->registers
	});
}


void mmu_step(MMU* mmu) {
	unsigned char cmd = *(unsigned char*)(mmu->registers + 0);
	unsigned int start_addr = *(unsigned int*)(mmu->registers + 1);
	unsigned int size = *(unsigned int*)(mmu->registers + 5);
	unsigned short id = *(unsigned short*)(mmu->registers + 9);

	mmu->registers[0] = 0;

	Motherboard* motherboard = mmu->motherboard;

	if (cmd == 1) { // reset
		mmu_clear_mmio(mmu);
	}

	if (cmd == 2) { // add
		if (id < motherboard->devices_count) {
			mmu_add_mmio(mmu, (MMIO){
			    .start_addr = start_addr,
			    .size = size,
			    .data = motherboard->devices[id] + sizeof(void*) + sizeof(int) * 2 + sizeof(unsigned long)
			});
		}
	}
}


unsigned long mmu_v2p(MMU* mmu, unsigned long addr, unsigned long tp, char* rules) {
	char* ram = ((Motherboard*)mmu->motherboard)->ram.ram;

	*rules = 0b0000;

	unsigned char i3 = (addr >> 56) & 0xffff;
	unsigned char i2 = (addr >> 48) & 0xffff;
	unsigned char i1 = (addr >> 32) & 0xff;
	unsigned char i0 = (addr >> 16) & 0xff;

	unsigned long offset = (addr >> 0) & 0xffff;

	unsigned long v3 = cpu2lt64(*((unsigned long*)(ram + tp + i3 * 8)));
	unsigned long v2 = cpu2lt64(*((unsigned long*)(ram + v3 + i2 * 8)));
	unsigned long v1 = cpu2lt64(*((unsigned long*)(ram + v2 + i1 * 8)));
	unsigned long v0 = cpu2lt64(*((unsigned long*)(ram + v1 + i0 * 8)));

	*rules = (v0 & (0b1111l << 60)) >> 60;

	return (v0 & ~(0b1111l << 60)) + offset;
}


unsigned long mmu_read(MMU* mmu, char vaddr, unsigned long tp, unsigned long addr, unsigned char size, char* rules) {
	*rules = 0b1111;

	if (vaddr) {
		addr = mmu_v2p(mmu, addr, tp, rules);
	}

	unsigned long mask = 0;
	for (int i = 0; i < size; i++)
		mask |= 255 << (i * 8);

	for (int i = 0; i < mmu->mmio_count; i++) {
		if (addr < mmu->mmio[i].start_addr || addr >= mmu->mmio[i].start_addr + mmu->mmio[i].size)
			continue;

		return cpu2lt64(*((unsigned long*)(mmu->mmio[i].data + addr - mmu->mmio[i].start_addr)) & mask);
	}

	return cpu2lt64(*((unsigned long*)(((Motherboard*)mmu->motherboard)->ram.ram + addr)) & mask);
}


void mmu_write(MMU* mmu, char vaddr, unsigned long tp, unsigned long addr, unsigned char size, unsigned long value) {
	if (vaddr) {
		char rules;
		addr = mmu_v2p(mmu, addr, tp, &rules);
	}

	unsigned long mask = 0;
	for (int i = 0; i < size; i++)
		mask |= 255l << (i * 8l);

	for (int i = 0; i < mmu->mmio_count; i++) {
		if (addr < mmu->mmio[i].start_addr || addr >= mmu->mmio[i].start_addr + mmu->mmio[i].size)
			continue;

		for (int j = 0; j < size; j++)
			mmu->mmio[i].data[addr - mmu->mmio[i].start_addr + j] = value >> (j * 8) & 0xff;

		return;
	}
	
	for (int j = 0; j < size; j++)
		((Motherboard*)mmu->motherboard)->ram.ram[addr + j] = value >> (j * 8) & 0xff;
}


void mmu_add_mmio(MMU* mmu, MMIO mmio) {
	mmu->mmio = realloc(mmu->mmio, sizeof(MMIO) * (++mmu->mmio_count));
	mmu->mmio[mmu->mmio_count-1] = mmio;
}


void mmu_clear_mmio(MMU* mmu) {
	free(mmu->mmio);
	mmu->mmio = malloc(0);
	mmu->mmio_count = 0;

	mmu_init_dev(mmu);
}
