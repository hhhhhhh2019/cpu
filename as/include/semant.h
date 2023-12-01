#ifndef SEMANT_H
#define SEMANT_H


#include <lexer.h>



// Label is nonterminal name
typedef struct Label_t {
	char* name;
	unsigned int offset;
} Label_t;



typedef struct Instruction_descr {
	enum Token_type token;
	char code;
	char size;
	int args_count;
	char* args; // 0 - register, 1 - number
} Instruction_descr;



typedef struct Semant_result {
	Label_t* labels;
	unsigned int labels_count;
} Semant_result;


#endif
