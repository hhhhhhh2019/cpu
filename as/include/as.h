#ifndef AS_H
#define AS_H


#include <lexer.h>
#include <synt.h>
#include <semant.h>


typedef struct Parser_state {
	char* filepath;
	char* filename;
	char* dirname;

	char* filedata;
	unsigned long filedata_size;

	char ok;

	Lexer_result        lresult;
	Synt_result         sresult;
	Semant_result       seresult;
} Parser_state;


Parser_state parse_file_stage_1(char*);
void free_state(Parser_state);

void lexer_parse(Parser_state*);
void synt_parse(Parser_state*);
void semant_parse(Parser_state*);


#endif
