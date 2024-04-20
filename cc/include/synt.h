#ifndef SYNT_H
#define SYNT_H


#include "token.h"

struct Node;
typedef struct Node {
	Token value;
	struct Node* childs;
	unsigned int childs_count;
} Node;


void print_node(Node, int);


#endif
