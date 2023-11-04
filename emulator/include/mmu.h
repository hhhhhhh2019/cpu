#ifndef MMU_H
#define MMU_H


#define MMU_RULE_INIT  1
#define MMU_RULE_READ  2
#define MMU_RULE_WRITE 4
#define MMU_RULE_EXEC  8


typedef struct MMIO {
	unsigned long start_addr;
	unsigned long size;
	char* data;
} MMIO;


typedef struct MMU {
	void* motherboard;

	unsigned long mmio_count;
	MMIO* mmio;

	char busy;
} MMU;


void mmu_init(MMU*, void* motherboard);

unsigned long mmu_read(MMU*, char vaddr, unsigned long tp, unsigned long addr, unsigned char size, char* rules);
void mmu_write(MMU*, char vaddr, unsigned long tp, unsigned long addr, unsigned char size, unsigned long value);


#endif
