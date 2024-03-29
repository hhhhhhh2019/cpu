#include "error.h"
#include "lexer.h"
#include "utils.h"
#include <as.h>

#include <stdlib.h>
#include <string.h>


Instruction_descr instr_args[] = {
	{"stol",   0x00, 10, 3, (char[]){0, 0, 1}},
	{"stoi",   0x01, 10, 3, (char[]){0, 0, 1}},
	{"stos",   0x02, 10, 3, (char[]){0, 0, 1}},
	{"stob",   0x03, 10, 3, (char[]){0, 0, 1}},

	{"loal",   0x04, 10, 3, (char[]){0, 0, 1}},
	{"loai",   0x05, 10, 3, (char[]){0, 0, 1}},
	{"loas",   0x06, 10, 3, (char[]){0, 0, 1}},
	{"loab",   0x07, 10, 3, (char[]){0, 0, 1}},

	{"add",    0x08,  3, 3, (char[]){0, 0, 0}},
	{"sub",    0x09,  3, 3, (char[]){0, 0, 0}},
	{"mul",    0x0a,  3, 3, (char[]){0, 0, 0}},
	{"div",    0x0b,  3, 3, (char[]){0, 0, 0}},
	{"mod",    0x0c,  3, 3, (char[]){0, 0, 0}},
	{"smul",   0x0d,  3, 3, (char[]){0, 0, 0}},
	{"sdiv",   0x0e,  3, 3, (char[]){0, 0, 0}},

	{"add",    0x0f, 10, 3, (char[]){0, 0, 1}},
	{"sub",    0x10, 10, 3, (char[]){0, 0, 1}},
	{"mul",    0x11, 10, 3, (char[]){0, 0, 1}},
	{"div",    0x12, 10, 3, (char[]){0, 0, 1}},
	{"mod",    0x13, 10, 3, (char[]){0, 0, 1}},
	{"smul",   0x14, 10, 3, (char[]){0, 0, 1}},
	{"sdiv",   0x15, 10, 3, (char[]){0, 0, 1}},

	{"adde",   0x16, 10, 3, (char[]){0, 0, 1}},
	{"addne",  0x17, 10, 3, (char[]){0, 0, 1}},
	{"addl",   0x18, 10, 3, (char[]){0, 0, 1}},
	{"addg",   0x19, 10, 3, (char[]){0, 0, 1}},
	{"addsl",  0x1a, 10, 3, (char[]){0, 0, 1}},
	{"addsg",  0x1b, 10, 3, (char[]){0, 0, 1}},

	{"pushl",  0x1c,  2, 1, (char[]){0}},
	{"pushi",  0x1d,  2, 1, (char[]){0}},
	{"pushs",  0x1e,  2, 1, (char[]){0}},
	{"pushb",  0x1f,  2, 1, (char[]){0}},

	{"popl",   0x20,  2, 1, (char[]){0}},
	{"popi",   0x21,  2, 1, (char[]){0}},
	{"pops",   0x22,  2, 1, (char[]){0}},
	{"popb",   0x23,  2, 1, (char[]){0}},

	{"call",   0x24,  2, 1, (char[]){0}},
	{"int",    0x25,  2, 1, (char[]){1}},
	{"iret",   0x26,  0, 0, (char[]){}},

	{"and",    0x27,  3, 3, (char[]){0, 0, 0}},
	{"or",     0x28,  3, 3, (char[]){0, 0, 0}},
	{"xor",    0x29,  3, 3, (char[]){0, 0, 0}},
	{"not",    0x2a,  2, 2, (char[]){0, 0}},
	{"shl",    0x2b,  3, 3, (char[]){0, 0, 0}},
	{"shr",    0x2c,  3, 3, (char[]){0, 0, 0}},

	{"and",    0x2d, 10, 3, (char[]){0, 0, 1}},
	{"or",     0x2e, 10, 3, (char[]){0, 0, 1}},
	{"xor",    0x2f, 10, 3, (char[]){0, 0, 1}},
	{"shl",    0x30, 10, 3, (char[]){0, 0, 1}},
	{"shr",    0x31, 10, 3, (char[]){0, 0, 1}},

	{"chst",   0x32,  2, 1, (char[]){0}},
	{"lost",   0x33,  2, 1, (char[]){0}},
	{"stolk",  0x34, 10, 3, (char[]){0, 0, 1}},
	{"loalk",  0x35, 10, 3, (char[]){0, 0, 1}},
	{"chtp",   0x36,  2, 1, (char[]){0}},
	{"lotp",   0x37,  2, 1, (char[]){0}},
	{"chflag", 0x38,  2, 1, (char[]){0}},
	{"loflag", 0x39,  2, 1, (char[]){0}},

	//{CINT, 0x3c, 1, (char[]){0}},
};

unsigned int instr_count = sizeof(instr_args) / sizeof(Instruction_descr);


static unsigned long offset;
static char* last_label;


static void check_label(Compiler_state* state, Node* node) {
	char* label = malloc(strlen(node->token.value) + 1);
	strcpy(label, node->token.value);

	if (label[0] == '.') {
		if (last_label != NULL) {
			int l = strlen(label);
			label = realloc(label, strlen(last_label) + l + 1);
			memcpy(label + strlen(last_label), label, l);
			memcpy(label, last_label, strlen(last_label));
			label[l + strlen(last_label)] = 0;
		}
	} else
		last_label = label;

	for (int i = 0; i < state->semant_result.labels_count; i++) {
		if (strcmp(state->semant_result.labels[i].name, label) != 0)
			continue;

		add_error((Error){
		    .type = LABEL_REDEFINE,
		    .token = node->token
		});
	}

	state->semant_result.labels = realloc(
	    state->semant_result.labels,
	    sizeof(Semant_result) * (++state->semant_result.labels_count));

	state->semant_result.labels[state->semant_result.labels_count - 1] = (Label){
	    .name = label,
	    .offset = offset
	};
}

static void check_data(Compiler_state* state, Node* node) {
	unsigned long times = 1;
	Node* root = node;
	root->size = 0;

	if (node->token.type == TIMES) {
		times = solve_expression(state, 0, NULL, node->childs[0], 0);
		node = node->childs[1];
	}

	while (1) {
		char size = 0;

		if (strcmp(node->token.value, "dl") == 0)
			size = 8;
		else if (strcmp(node->token.value, "di") == 0)
			size = 4;
		else if (strcmp(node->token.value, "ds") == 0)
			size = 2;
		else if (strcmp(node->token.value, "db") == 0)
			size = 1;
		
		int count = 0;

		for (int i = 0; i < node->childs[0]->childs_count; i++) {
			if (node->childs[0]->childs[i]->token.type == STRING)
				count += strlen(node->childs[0]->childs[i]->token.value);
			else
				count += 1;
		}

		root->size += size * count;

		if (node->childs_count == 1)
			break;

		node = node->childs[1];
	}

	root->size *= times;

	offset += root->size;
}


static void check_instruction_no_args(Compiler_state* state, Node* node) {
	for (int i = 0; i < instr_count; i++) {
		if (strcmp(node->token.value, instr_args[i].name) != 0)
			continue;

		node->code = instr_args[i].code;
		node->size = 1;
		node->offset = offset;
		offset += 1;

		break;
	}
}


static void check_instruction(Compiler_state* state, Node* node) {
	if (node->childs_count == 0) {
		check_instruction_no_args(state, node);
		return;
	}

	char* suitable = malloc(0);
	unsigned int suitable_size = 0;

	char* suitable_buffer;
	unsigned int suitable_buffer_size = 0;

	for (int i = 0; i < instr_count; i++) {
		if (strcmp(node->token.value, instr_args[i].name) != 0)
			continue;

		suitable = realloc(suitable, sizeof(char) * (++suitable_size));
		suitable[suitable_size-1] = i;
	}

	Node* instr = node;

	// node = node->childs[1];
	
	unsigned int arg_id = 0;

	while (arg_id < instr->childs_count) {
		Node* arg = instr->childs[arg_id];

		suitable_buffer = malloc(0);
		suitable_buffer_size = 0;

		char arg_code = 1;
		if (arg->token.type == REGISTER)
			arg_code = 0;

		for (int i = 0; i < suitable_size; i++) {
			if (instr_args[suitable[i]].args_count == arg_id)
				continue;

			if (instr_args[suitable[i]].args[arg_id] != arg_code)
				continue;

			suitable_buffer = realloc(suitable_buffer, sizeof(char) * (++suitable_buffer_size));
			suitable_buffer[suitable_buffer_size-1] = suitable[i];
		}

		if (suitable_buffer_size == 0)
			break;

		free(suitable);
		suitable = suitable_buffer;
		suitable_size = suitable_buffer_size;

		// if (node->childs_count == 1)
		// 	break;
		// node = node->childs[0];
		arg_id++;
	}

	if (suitable_size != 1 && arg_id == 0) {
		add_error((Error){
		    .type = INSTRUCTION_NOT_FOUND,
		    .token = instr->token
		});

		// fprintf(stderr, "Instr not found!\n");
		free(suitable);
		// free(suitable_buffer);
		return;
	}

	if (arg_id != instr_args[suitable[0]].args_count) {
		enum Token_type* types = malloc(sizeof(enum Token_type) * suitable_size);

		for (int i = 0; i < suitable_size; i++)
			types[i] = instr_args[suitable[i]].args[arg_id] == 0 ? REGISTER : DEC_NUMBER;

		add_error((Error){
		    .type = EXPECT_TOKEN,
		    .token = instr->childs[arg_id]->token,
		    .excepted_tokens_count = suitable_size,
		    .excepted_tokens = types
	});

		// fprintf(stderr, "Instr args error!\n");
		return;
	}

	instr->code = instr_args[suitable[0]].code;
	instr->size = instr_args[suitable[0]].size;

	offset += instr->size;

	free(suitable);
	// free(suitable_buffer);
}


void semant(Compiler_state* state) {
	offset = 0;
	last_label = NULL;

	for (int i = 0; i < state->synt_result.nodes_count; i++) {
		state->synt_result.nodes[i]->offset = offset;

		if (state->synt_result.nodes[i]->type == SLABEL)
			check_label(state, state->synt_result.nodes[i]);
		else if (state->synt_result.nodes[i]->type == SDATA)
			check_data(state, state->synt_result.nodes[i]);
		else if (state->synt_result.nodes[i]->type == SINSTRUCTION)
			check_instruction(state, state->synt_result.nodes[i]);
	}
}
