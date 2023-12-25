#ifndef SYNT_H
#define SYNT_H


#include <lexer.h>
#include <utils.h>

#include <stdlib.h>


struct Node;

typedef struct Node {
	Token value;
	unsigned int childs_count;
	struct Node** childs;

	struct Node* parent;

	int id;
} Node;



static inline Node* empty_node() {
	Node* node = calloc(sizeof(Node),1);
	node->childs = malloc(0);

	return node;
}


static inline void node_add_child(Node* node, Node* child) {
	if (child == NULL)
		return;

	node->childs = realloc(node->childs, sizeof(Node) * (++node->childs_count));
	node->childs[node->childs_count-1] = child;
	child->parent = node;
}


Stack(int);

Stack(long);

extern int table[171][171];
extern enum Token_type** todo;
void prepeare_todo();


void create_dot_from_node(Node*);


#endif
