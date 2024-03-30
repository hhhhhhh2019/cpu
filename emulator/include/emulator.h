#ifndef EMULATOR_H
#define EMULATOR_H


enum Breakpoint_type {
	EQUALS,
	NOT_EQUALS,
	AND_MASK_ZERO,
	AND_MASK_NONZERO,
	OR_MASK_ZERO,
	OR_MASK_NONZERO,
	XOR_MASK_ZERO,
	XOR_MASK_NONZERO,
};

typedef struct {
	char id;
	unsigned long value;
} BrRegister;

typedef struct {
	enum Breakpoint_type type;
	BrRegister registers[18];
	unsigned int registers_count;
} Breakpoint;


#endif
