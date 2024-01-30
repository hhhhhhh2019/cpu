#ifndef SEMANT_H
#define SEMANT_H


#include <synt.h>
#include <lexer.h>


typedef struct {
	Node* var;
} Variable;


struct FieldOfView;

typedef struct FieldOfView {
	struct FieldOfView* parent;
	unsigned int vars_count;
	Variable* vars;
} FieldOfView;


#endif
