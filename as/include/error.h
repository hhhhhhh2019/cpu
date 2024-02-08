#ifndef ERROR_H
#define ERROR_H


#include <lexer.h>


enum Error_type {
	PREPROCESS_REDEFINITION,
	FILE_NOT_FOUND,
	STRING_NOT_CLOSED,
	EXPECT_TOKEN,
	UNEXPECTED_TOKEN,
	LABEL_REDEFINE,
};


typedef struct {
	enum Error_type type;
	Token token;
	enum Token_type excepted_token;
} Error;


extern unsigned int errors_count;
extern Error* errors;


void add_error(Error);
void print_errors();


#endif
