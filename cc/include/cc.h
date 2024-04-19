#ifndef CC_H
#define CC_H


#include "lexer.h"
#include "cc.h"


typedef struct {
	char* output_filename;

	Lexer_result  lexer_result;
	// Synt_result   synt_result;
	// Semant_result semant_result;
	
	// unsigned int defines_count;
	// Define* defines;
	//
	// unsigned int macros_count;
	// Macro* macros;
} Compiler_state;


void print_help();

void synt(Compiler_state* state);


#endif
