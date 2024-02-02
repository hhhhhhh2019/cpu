#ifndef PREPROC_H
#define PREPROC_H


#include <lexer.h>


typedef struct {
	char* name;
	char* value;
} Macros;


Lexer_result preprocess(char* filename);


#endif
