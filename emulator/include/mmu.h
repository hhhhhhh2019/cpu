#ifndef MMU_H
#define MMU_H


#define MMU_RULE_INIT  1
#define MMU_RULE_READ  2
#define MMU_RULE_WRITE 4
#define MMU_RULE_EXEC  8


#define MMU_MMIO_OFFSET 0x100


typedef struct MMIO {
	unsigned long start_addr;
	unsigned long size;
	char* data;
} MMIO;


typedef struct MMU {
	void* motherboard;

	unsigned long hz; // частота обработки команд
	
	char registers[11];

	unsigned long mmio_count;
	MMIO* mmio;

	char busy;
} MMU;


void mmu_init(MMU*, void* motherboard, unsigned long hz);

void mmu_step(MMU*);

void mmu_init_dev(MMU*);

unsigned long mmu_read(MMU*, char vaddr, unsigned long tp, unsigned long addr, unsigned char size, char* rules);
void mmu_write(MMU*, char vaddr, unsigned long tp, unsigned long addr, unsigned char size, unsigned long value);

void mmu_add_mmio(MMU*, MMIO);
void mmu_clear_mmio(MMU*);


#endif
