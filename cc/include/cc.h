#ifndef CC_H
#define CC_H


#include <lexer.h>


enum Error_type {
	CANT_OPEN_FILE,
};


typedef struct {
	enum Error_type type;
	char* text;
	unsigned long start_token;
	unsigned long end_token;
} Error;


typedef struct {
	unsigned int errors_count;
	Error* errors;

	unsigned long code_size;
	char* code;

	unsigned long tokens_count;
	Token* tokens;

	unsigned long real_tokens_count;
	Token* real_tokens; // для отчистки, value всегда динамически выделен

	char* filepath;
	char* filename;
	char* dirname;
} Compiler_state;


void print_help();

Compiler_state parse_file_prepoc(char*);
void lexer(Compiler_state*);

void add_error(Compiler_state*, Error);

char print_errors(Compiler_state*);
void free_and_exit(Compiler_state, int code);


#endif
