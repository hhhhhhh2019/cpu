#ifndef SYNT_H
#define SYNT_H


#include <lexer.h>
#include <utils.h>


Stack(int)


typedef struct {
	Token value;
	unsigned int childs_count;
	void** childs;
} Node;


extern int table[171][171];
extern enum Token_type** todo;
void prepeare_todo();


#endif
