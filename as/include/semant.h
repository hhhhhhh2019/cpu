#ifndef SEMANT_H
#define SEMANT_H


#include <lexer.h>



// Label is nonterminal name
typedef struct {
	char* name;
	unsigned int offset;
} Label;



typedef struct Instruction_descr {
	char* name;
	char code;
	char size;
	int args_count;
	char* args; // 0 - register, 1 - number
} Instruction_descr;



typedef struct Semant_result {
	Label* labels;
	unsigned int labels_count;
} Semant_result;


#endif
