#include <as.h>


Instruction_descr instr_args[] = {
	{"STOL",   0x00, 10, 3, (char[]){0, 0, 1}},
	{"STOI",   0x01, 10, 3, (char[]){0, 0, 1}},
	{"STOS",   0x02, 10, 3, (char[]){0, 0, 1}},
	{"STOB",   0x03, 10, 3, (char[]){0, 0, 1}},

	{"LOAL",   0x04, 10, 3, (char[]){0, 0, 1}},
	{"LOAI",   0x05, 10, 3, (char[]){0, 0, 1}},
	{"LOAS",   0x06, 10, 3, (char[]){0, 0, 1}},
	{"LOAB",   0x07, 10, 3, (char[]){0, 0, 1}},

	{"ADD",    0x08,  3, 3, (char[]){0, 0, 0}},
	{"SUB",    0x09,  3, 3, (char[]){0, 0, 0}},
	{"MUL",    0x0a,  3, 3, (char[]){0, 0, 0}},
	{"DIV",    0x0b,  3, 3, (char[]){0, 0, 0}},
	{"SMUL",   0x0c,  3, 3, (char[]){0, 0, 0}},
	{"SDIV",   0x0d,  3, 3, (char[]){0, 0, 0}},
	{"CMP",    0x0e,  2, 2, (char[]){0, 0}},

	{"ADD",    0x0f, 10, 3, (char[]){0, 0, 1}},
	{"SUB",    0x10, 10, 3, (char[]){0, 0, 1}},
	{"MUL",    0x11, 10, 3, (char[]){0, 0, 1}},
	{"DIV",    0x12, 10, 3, (char[]){0, 0, 1}},
	{"SMUL",   0x13, 10, 3, (char[]){0, 0, 1}},
	{"SDIV",   0x14, 10, 3, (char[]){0, 0, 1}},
	{"CMP",    0x15, 10, 2, (char[]){0, 1}},

	{"ADDE",   0x16, 10, 3, (char[]){0, 0, 1}},
	{"ADDNE",  0x17, 10, 3, (char[]){0, 0, 1}},
	{"ADDL",   0x18, 10, 3, (char[]){0, 0, 1}},
	{"ADDG",   0x19, 10, 3, (char[]){0, 0, 1}},
	{"ADDSL",  0x1a, 10, 3, (char[]){0, 0, 1}},
	{"ADDSG",  0x1b, 10, 3, (char[]){0, 0, 1}},

	{"PUSHL",  0x1c,  2, 1, (char[]){0}},
	{"PUSHI",  0x1d,  2, 1, (char[]){0}},
	{"PUSHS",  0x1e,  2, 1, (char[]){0}},
	{"PUSHB",  0x1f,  2, 1, (char[]){0}},

	{"POPL",   0x20,  2, 1, (char[]){0}},
	{"POPI",   0x21,  2, 1, (char[]){0}},
	{"POPS",   0x22,  2, 1, (char[]){0}},
	{"POPB",   0x23,  2, 1, (char[]){0}},

	{"CALL",   0x24,  2, 1, (char[]){0}},
	{"INT",    0x25,  2, 1, (char[]){1}},
	{"IRET",   0x26,  0, 0, (char[]){}},

	{"AND",    0x27,  3, 3, (char[]){0, 0, 0}},
	{"OR",     0x28,  3, 3, (char[]){0, 0, 0}},
	{"XOR",    0x29,  3, 3, (char[]){0, 0, 0}},
	{"NOT",    0x2a,  2, 2, (char[]){0, 0}},
	{"SHL",    0x2b,  3, 3, (char[]){0, 0, 0}},
	{"SHR",    0x2c,  3, 3, (char[]){0, 0, 0}},

	{"AND",    0x2d, 10, 3, (char[]){0, 0, 1}},
	{"OR",     0x2e, 10, 3, (char[]){0, 0, 1}},
	{"XOR",    0x2f, 10, 3, (char[]){0, 0, 1}},
	{"SHL",    0x30, 10, 3, (char[]){0, 0, 1}},
	{"SHR",    0x31, 10, 3, (char[]){0, 0, 1}},

	{"CHST",   0x32,  2, 1, (char[]){0}},
	{"LOST",   0x33,  2, 1, (char[]){0}},
	{"STOLK",  0x34, 10, 3, (char[]){0, 0, 1}},
	{"LOALK",  0x35, 10, 3, (char[]){0, 0, 1}},
	{"CHTP",   0x36,  2, 1, (char[]){0}},
	{"LOTP",   0x37,  2, 1, (char[]){0}},
	{"CHFLAG", 0x38,  2, 1, (char[]){0}},
	{"LOFLAG", 0x39,  2, 1, (char[]){0}},

	//{CINT, 0x3c, 1, (char[]){0}},
};

unsigned int instr_count = sizeof(instr_args) / sizeof(Instruction_descr);


void semant(Compiler_state* state) {
	
}
