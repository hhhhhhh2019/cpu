#ifndef SYNT_H
#define SYNT_H


#include <lexer.h>


enum Node_type {
	SLABEL,
	SINSTRUCTION,
	SDATA,
	SGROUP,
};


typedef struct Node {
	enum Node_type type;
	unsigned long id;
	struct Node* parent;
	unsigned long childs_count;
	struct Node** childs;
	Token token;
} Node;


typedef struct {
	unsigned long nodes_count;
	Node** nodes;
} Synt_result;


void create_dot_from_node(Node*);


#endif
