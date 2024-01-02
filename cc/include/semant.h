#ifndef SEMANT_H
#define SEMANT_H


#include <lexer.h>


typedef struct {
	Token name;
} Variable;


struct FieldOfView;

typedef struct {
	struct FieldOfView* parent;
	unsigned int vars_count;
	Variable* vars;
} FieldOfView;


#endif
