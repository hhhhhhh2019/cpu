#ifndef SYNT_H
#define SYNT_H


#include <lexer.h>
#include <utils.h>


struct Node;

typedef struct Node {
	Token value;
	unsigned int childs_count;
	struct Node** childs;

	struct Node* parent;

	int id;
} Node;



Stack(int)

Stack(long)

extern int table[171][171];
extern enum Token_type** todo;
void prepeare_todo();


void create_dot_from_node(Node*);


#endif
