#include "lexer.h"
#include <as.h>
#include <utils.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


unsigned long offset;



Instruction_descr instr_args[] = {
	{STOL,   0x00, 10, 3, (char[]){0, 0, 1}},
	{STOI,   0x01, 10, 3, (char[]){0, 0, 1}},
	{STOS,   0x02, 10, 3, (char[]){0, 0, 1}},
	{STOB,   0x03, 10, 3, (char[]){0, 0, 1}},

	{LOAL,   0x04, 10, 3, (char[]){0, 0, 1}},
	{LOAI,   0x05, 10, 3, (char[]){0, 0, 1}},
	{LOAS,   0x06, 10, 3, (char[]){0, 0, 1}},
	{LOAB,   0x07, 10, 3, (char[]){0, 0, 1}},

	{ADD,    0x08,  3, 3, (char[]){0, 0, 0}},
	{SUB,    0x09,  3, 3, (char[]){0, 0, 0}},
	{MUL,    0x0a,  3, 3, (char[]){0, 0, 0}},
	{DIV,    0x0b,  3, 3, (char[]){0, 0, 0}},
	{SMUL,   0x0c,  3, 3, (char[]){0, 0, 0}},
	{SDIV,   0x0d,  3, 3, (char[]){0, 0, 0}},
	{CMP,    0x0e,  2, 2, (char[]){0, 0}},

	{ADD,    0x0f, 10, 3, (char[]){0, 0, 1}},
	{SUB,    0x10, 10, 3, (char[]){0, 0, 1}},
	{MUL,    0x11, 10, 3, (char[]){0, 0, 1}},
	{DIV,    0x12, 10, 3, (char[]){0, 0, 1}},
	{SMUL,   0x13, 10, 3, (char[]){0, 0, 1}},
	{SDIV,   0x14, 10, 3, (char[]){0, 0, 1}},
	{CMP,    0x15, 10, 2, (char[]){0, 1}},

	{ADDE,   0x16, 10, 3, (char[]){0, 0, 1}},
	{ADDNE,  0x17, 10, 3, (char[]){0, 0, 1}},
	{ADDL,   0x18, 10, 3, (char[]){0, 0, 1}},
	{ADDG,   0x19, 10, 3, (char[]){0, 0, 1}},
	{ADDO,   0x1a, 10, 3, (char[]){0, 0, 1}},
	{ADDNO,  0x1b, 10, 3, (char[]){0, 0, 1}},

	{PUSHL,  0x1c,  2, 1, (char[]){0}},
	{PUSHI,  0x1d,  2, 1, (char[]){0}},
	{PUSHS,  0x1e,  2, 1, (char[]){0}},
	{PUSHB,  0x1f,  2, 1, (char[]){0}},

	{POPL,   0x20,  2, 1, (char[]){0}},
	{POPI,   0x21,  2, 1, (char[]){0}},
	{POPS,   0x22,  2, 1, (char[]){0}},
	{POPB,   0x23,  2, 1, (char[]){0}},

	{CALL,   0x24,  2, 1, (char[]){0}},
	{INT,    0x25,  2, 1, (char[]){1}},
	{IRET,   0x26,  0, 0, (char[]){}},

	{AND,    0x27,  3, 3, (char[]){0, 0, 0}},
	{OR,     0x28,  3, 3, (char[]){0, 0, 0}},
	{XOR,    0x29,  3, 3, (char[]){0, 0, 0}},
	{NOT,    0x2a,  2, 2, (char[]){0, 0}},
	{SHL,    0x2b,  3, 3, (char[]){0, 0, 0}},
	{SHR,    0x2c,  3, 3, (char[]){0, 0, 0}},

	{AND,    0x2d, 10, 3, (char[]){0, 0, 1}},
	{OR,     0x2e, 10, 3, (char[]){0, 0, 1}},
	{XOR,    0x2f, 10, 3, (char[]){0, 0, 1}},
	{SHL,    0x30, 10, 3, (char[]){0, 0, 1}},
	{SHR,    0x31, 10, 3, (char[]){0, 0, 1}},

	{CHST,   0x32,  2, 1, (char[]){0}},
	{LOST,   0x33,  2, 1, (char[]){0}},
	{STOLK,  0x34,  2, 1, (char[]){0}},
	{LOALK,  0x35,  2, 1, (char[]){0}},
	{CHTP,   0x36,  2, 1, (char[]){0}},
	{LOTP,   0x37,  2, 1, (char[]){0}},
	{CHFLAG, 0x38,  2, 1, (char[]){0}},
	{LOFLAG, 0x39,  2, 1, (char[]){0}},

	//{CINT, 0x3c, 1, (char[]){0}},
};

unsigned int instr_count = sizeof(instr_args) / sizeof(Instruction_descr);



void check_label(Parser_state* state, Node* node) {
	char* name = node->childs[1]->value.value;

	for (int i = 0; i < state->seresult.labels_count; i++) {
		if (strcmp(name, state->seresult.labels[i].name) == 0) {
			printf("%s %lu:%lu Переопределение метки!\n",
			    node->childs[1]->value.filepath,
			    node->childs[1]->value.line,
			    node->childs[1]->value.col);
			state->ok = 0;
		}
	}

	state->seresult.labels = realloc(state->seresult.labels,
	   sizeof(Label_t) * (++state->seresult.labels_count));
	state->seresult.labels[state->seresult.labels_count-1] = (Label_t){
		name,
		offset
	};
}


void check_data(Parser_state* state, Node* node) {
	node->offset = offset;

	int count = 1;

	if (node->childs_count == 3)
		count = get_number(state, node->childs[node->childs_count-1]->childs[0], node);

	int one_size = 0;

	if (node->childs[1]->value.type == DB)
		one_size = 1;
	else if (node->childs[1]->value.type == DS)
		one_size = 2;
	else if (node->childs[1]->value.type == DI)
		one_size = 4;
	else if (node->childs[1]->value.type == DL)
		one_size = 8;

	node->size = 0;

	Node* elem = node->childs[0];

	while (1) {
		node->size += one_size;

		if (elem->childs_count == 1)
			break;

		elem = elem->childs[0];
	}

	node->size *= count;

	offset += node->size;
}


void check_instruction(Parser_state* state, Node* node) {
	node->offset = offset;

	enum Token_type type = node->childs[1]->value.type;

	char* suitable = malloc(0);
	unsigned int suitable_size = 0;

	char* suitable_buffer = malloc(0);
	unsigned int suitable_buffer_size = 0;


	for (int i = 0; i < instr_count; i++) {
		if (instr_args[i].token != type)
			continue;

		suitable = realloc(suitable, sizeof(char) * (++suitable_size));
		suitable[suitable_size-1] = i;
	}

	Node* instr = node;

	node = node->childs[0];
	unsigned int arg_id = 0;

	while (1) {
		Node* arg = node->childs[node->childs_count-1];

		char arg_code = 1;
		if (arg->value.type == REGISTER)
			arg_code = 0;

		for (int i = 0; i < suitable_size; i++) {
			if (instr_args[suitable[i]].args_count == arg_id)
				continue;

			if (instr_args[suitable[i]].args[arg_id] != arg_code)
				continue;

			suitable_buffer = realloc(suitable_buffer, sizeof(char) * (++suitable_buffer_size));
			suitable_buffer[suitable_buffer_size-1] = suitable[i];
		}

		free(suitable);
		suitable = suitable_buffer;
		suitable_size = suitable_buffer_size;
		suitable_buffer = malloc(0);
		suitable_buffer_size = 0;

		if (node->childs_count == 1)
			break;
		node = node->childs[0];
		arg_id++;
	}

	if (suitable_size != 1) {
		fprintf(stderr, "Instr not found!\n");
		free(suitable);
		free(suitable_buffer);
		state->ok = 0;
	}

	if (arg_id >= instr_args[suitable[0]].args_count) {
		fprintf(stderr, "Instr args error!\n");
		state->ok = 0;
	}

	instr->code = instr_args[suitable[0]].code;
	instr->size = instr_args[suitable[0]].size;

	offset += instr->size;

	free(suitable);
	free(suitable_buffer);
}


void check_offset(Parser_state* state, Node* node) {
	offset = get_number(state, node, node->childs[0]);
}


void check_node(Parser_state* state, Node* node) {
	if (node->value.type == Label)
		check_label(state, node);
	else if (node->value.type == Data)
		check_data(state, node);
	else if (node->value.type == Offset)
		check_offset(state, node);
	else
		check_instruction(state, node);
}


void semant_parse(Parser_state* state) {
	offset = 0;

	state->seresult.labels_count = 0;
	state->seresult.labels = malloc(0);

	Node* node = state->sresult.root;

	while (1) {
		if (node->childs_count == 1) {
			if (node->childs[0]->value.type == S) {
				node = node->childs[0];
				continue;
			}

			check_node(state, node->childs[0]);

			break;
		}

		if (node->childs_count == 0)
			break;

		check_node(state, node->childs[1]);

		node = node->childs[0];
	}
}
