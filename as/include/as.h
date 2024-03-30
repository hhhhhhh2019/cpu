#ifndef AS_H
#define AS_H


#include <lexer.h>
#include <preproc.h>
#include <synt.h>
#include <semant.h>


typedef struct {
	char* output_filename;
	char* map_filename;

	Lexer_result  lexer_result;
	Synt_result   synt_result;
	Semant_result semant_result;
	
	unsigned int defines_count;
	Define* defines;

	unsigned int macros_count;
	Macro* macros;
} Compiler_state;


void synt(Compiler_state*);
void semant(Compiler_state*);


void print_help();


#endif
