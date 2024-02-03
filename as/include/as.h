#ifndef AS_H
#define AS_H


#include <lexer.h>
#include <preproc.h>


typedef struct {
	Lexer_result lexer_result;
	
	unsigned int defines_count;
	Define* defines;

	unsigned int macros_count;
	Macro* macros;
} Compiler_state;



void print_help();


#endif
