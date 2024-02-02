#ifndef AS_H
#define AS_H


#include <lexer.h>
#include <preproc.h>


typedef struct {
	Lexer_result lexer_result;
} Compiler_state;



void print_help();


#endif
