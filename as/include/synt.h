#ifndef SYNT_H
#define SYNT_H


#include <lexer.h>


extern int table[99][99];
extern enum Token_type** todo;

extern char** token_type_names;

void prepeare_todo();
void prepeare_names();

void free_todo();
void free_names();


typedef struct Node {
	Token value;

	struct Node* parent;
	unsigned int childs_count;
	struct Node** childs;

	int id;

	long size;
	long offset;
	long code;
} Node;


typedef struct Synt_result {
	Node* root;
} Synt_result;


char collapse_node(Node*);
void create_dot_from_node(Node*);
void free_node(Node*);


#endif
