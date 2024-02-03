#ifndef PREPROC_H
#define PREPROC_H


#include <lexer.h>


typedef struct {
	char* name;
	unsigned int tokens_count;
	Token* tokens;
} Define;


typedef struct {
	char* name;
	unsigned int args_count;
	Token* args;
	unsigned int tokens_count;
	Token* tokens;
} Macro;


Lexer_result preprocess(void* state, char* filename);


#endif
