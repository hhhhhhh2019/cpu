#ifndef LEXER_H
#define LEXER_H


#include "token.h"


typedef struct {
	unsigned int tokens_count;
	Token* tokens;
} Lexer_result;


extern char* token_type_names[];


Lexer_result lexer(char* data);


#endif
